// Generate independent BSim hypotheses without modifying the analyzed program.
//@category Saga
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;

import com.google.gson.*;
import ghidra.app.script.GhidraScript;
import ghidra.framework.Application;
import ghidra.features.bsim.query.*;
import ghidra.features.bsim.query.description.*;
import ghidra.features.bsim.query.protocol.*;
import ghidra.program.model.listing.Function;

/** Headless arguments: one JSON configuration path. See tools/bsim.py. */
public class ExportBSimCandidates extends GhidraScript {
    private static final String TEMPLATE = "medium_nosize";
    private final Gson gson = new GsonBuilder().setPrettyPrinting().disableHtmlEscaping().create();

    private String digest(Path path, String algorithm) throws Exception {
        MessageDigest md = MessageDigest.getInstance(algorithm);
        try (InputStream stream = Files.newInputStream(path)) {
            byte[] buffer = new byte[65536];
            int count;
            while ((count = stream.read(buffer)) > 0) md.update(buffer, 0, count);
        }
        return HexFormat.of().formatHex(md.digest());
    }

    private long address(JsonElement value) {
        String text = value.getAsString();
        return text.startsWith("0x") ? Long.parseUnsignedLong(text.substring(2), 16) : Long.parseLong(text);
    }

    private String hex(long value) { return String.format("0x%08x", value); }

    private JsonObject validateProgram(JsonObject spec) throws Exception {
        Path original = Path.of(spec.get("original_path").getAsString());
        String md5 = digest(original, "MD5");
        String sha256 = digest(original, "SHA-256");
        if (!md5.equals(currentProgram.getExecutableMD5()) ||
                !sha256.equals(spec.get("sha256").getAsString()) ||
                currentProgram.getImageBase().getOffset() != address(spec.get("image_base")) ||
                !currentProgram.getLanguageID().toString().equals(spec.get("language_id").getAsString()) ||
                !currentProgram.getCompilerSpec().getCompilerSpecID().toString().equals(spec.get("compiler_id").getAsString())) {
            throw new IOException("Program identity/base/language/compiler mismatch");
        }
        JsonObject metadata = spec.deepCopy();
        metadata.addProperty("md5", md5);
        metadata.addProperty("program_name", currentProgram.getName());
        metadata.addProperty("function_count", currentProgram.getFunctionManager().getFunctionCount());
        return metadata;
    }

    private JsonObject envelope(String mode, JsonObject program) {
        JsonObject result = new JsonObject();
        result.addProperty("schema", "saga-bsim-v1");
        result.addProperty("mode", mode);
        result.addProperty("producer", "ExportBSimCandidates.java/v1");
        result.addProperty("ghidra_version", Application.getApplicationVersion());
        result.addProperty("ghidra_release", Application.getApplicationReleaseName());
        result.addProperty("template", TEMPLATE);
        result.add("program", program);
        return result;
    }

    private String id(JsonObject program, long va) {
        return program.get("sha256").getAsString() + ":ram:" + hex(va) + ":function";
    }

    private void write(Path path, JsonObject result) throws IOException {
        Files.createDirectories(path.toAbsolutePath().getParent());
        Path pending = path.resolveSibling(path.getFileName() + ".tmp");
        Files.writeString(pending, gson.toJson(result) + "\n", StandardCharsets.UTF_8);
        Files.move(pending, path, StandardCopyOption.REPLACE_EXISTING, StandardCopyOption.ATOMIC_MOVE);
    }

    private JsonObject failure(long va, String reason) {
        JsonObject item = new JsonObject();
        item.addProperty("address", hex(va));
        item.addProperty("reason", reason);
        return item;
    }

    private void index(JsonObject config, JsonObject program, Path output) throws Exception {
        String dbUrl = config.get("database_url").getAsString();
        // A new database only. A pre-existing DB must be accompanied by a validated prior manifest.
        Path dbFile = Path.of(config.get("database_file").getAsString());
        if (Files.exists(dbFile)) throw new IOException("Refusing to replace existing BSim database");
        long delta = program.get("address_delta").getAsLong();
        JsonObject result = envelope("index", program);
        JsonArray failures = new JsonArray();
        JsonArray signatures = new JsonArray();
        Map<Long, String> scannedAddresses = new TreeMap<>();
        Set<Long> failedAddresses = new HashSet<>();
        try (FunctionDatabase db = BSimClientFactory.buildClient(BSimClientFactory.deriveBSimURL(dbUrl), false)) {
            CreateDatabase create = new CreateDatabase();
            create.info = new DatabaseInformation();
            create.info.databasename = "saga-android";
            create.info.trackcallgraph = false;
            create.config_template = TEMPLATE;
            if (create.execute(db) == null) throw new IOException(db.getLastError().message);
            if (!db.initialize()) throw new IOException(db.getLastError().message);
            result.addProperty("signature_settings", db.getInfo().settings);
            GenSignatures generator = new GenSignatures(false);
            try {
                generator.setVectorFactory(db.getLSHVectorFactory());
                generator.openProgram(currentProgram, null, null, null, null, null);
                Iterator<Function> functions = currentProgram.getFunctionManager().getFunctions(true);
                int scanned = 0;
                while (functions.hasNext()) {
                    monitor.checkCancelled();
                    Function function = functions.next();
                    if (function.isExternal() || function.getBody().isEmpty()) continue;
                    long va = function.getEntryPoint().getOffset() + delta;
                    scannedAddresses.put(va, function.isThunk() ? "filtered_thunk" : "no_signature_or_decompile_failure");
                    try {
                        generator.scanFunction(function);
                    } catch (Exception ex) {
                        failures.add(failure(va, ex.getClass().getSimpleName() + ": " + ex.getMessage()));
                        failedAddresses.add(va);
                    }
                    if (++scanned % 250 == 0) println("BSim index scanned " + scanned);
                }
                DescriptionManager manager = generator.getDescriptionManager();
                Iterator<FunctionDescription> entries = manager.listAllFunctions();
                Set<Long> indexedAddresses = new HashSet<>();
                while (entries.hasNext()) {
                    FunctionDescription description = entries.next();
                    long va = description.getAddress() + delta;
                    if (description.getSignatureRecord() == null) {
                        if (failedAddresses.add(va)) failures.add(failure(va, "no_signature"));
                        continue;
                    }
                    indexedAddresses.add(va);
                    JsonObject item = new JsonObject();
                    item.addProperty("address", hex(va));
                    item.addProperty("id", id(program, va));
                    item.addProperty("name", description.getFunctionName());
                    item.addProperty("self_significance", db.getLSHVectorFactory().getSelfSignificance(description.getSignatureRecord().getLSHVector()));
                    signatures.add(item);
                }
                for (Map.Entry<Long, String> entry : scannedAddresses.entrySet()) {
                    if (!indexedAddresses.contains(entry.getKey()) && failedAddresses.add(entry.getKey()))
                        failures.add(failure(entry.getKey(), entry.getValue()));
                }
                result.addProperty("signature_major", manager.getMajorVersion());
                result.addProperty("signature_minor", manager.getMinorVersion());
                // Transfer only valid signatures to prevent failed decompilations entering the DB.
                DescriptionManager valid = new DescriptionManager();
                valid.transferSettings(manager);
                entries = manager.listAllFunctions();
                while (entries.hasNext()) {
                    FunctionDescription item = entries.next();
                    if (item.getSignatureRecord() != null) valid.transferFunction(item, true);
                }
                InsertRequest insert = new InsertRequest();
                insert.manage = valid;
                if (insert.execute(db) == null) throw new IOException(db.getLastError().message);
                result.addProperty("indexed_functions", valid.numFunctions());
                result.addProperty("scanned_functions", scanned);
            } finally { generator.dispose(); }
        }
        result.add("signatures", signatures);
        result.add("failures", failures);
        write(output, result);
    }

    private void query(JsonObject config, JsonObject program, Path output) throws Exception {
        JsonObject index = JsonParser.parseString(Files.readString(Path.of(config.get("index_manifest").getAsString()))).getAsJsonObject();
        if (!index.get("schema").getAsString().equals("saga-bsim-v1") ||
                !index.get("mode").getAsString().equals("index") ||
                !index.get("ghidra_version").getAsString().equals(Application.getApplicationVersion()) ||
                !index.get("ghidra_release").getAsString().equals(Application.getApplicationReleaseName()) ||
                !index.get("template").getAsString().equals(TEMPLATE)) throw new IOException("Incompatible BSim index manifest");
        JsonObject target = index.getAsJsonObject("program");
        Map<Long, String> indexedFunctions = new HashMap<>();
        for (JsonElement item : index.getAsJsonArray("signatures")) {
            JsonObject signature = item.getAsJsonObject();
            indexedFunctions.put(address(signature.get("address")), signature.get("name").getAsString());
        }
        long delta = program.get("address_delta").getAsLong();
        long targetDelta = target.get("address_delta").getAsLong();
        JsonObject result = envelope("query", program);
        result.add("target", target.deepCopy());
        result.addProperty("index_manifest_sha256", digest(Path.of(config.get("index_manifest").getAsString()), "SHA-256"));
        result.addProperty("max_candidates", config.get("max_candidates").getAsInt());
        result.addProperty("similarity_threshold", config.get("similarity_threshold").getAsDouble());
        result.addProperty("significance_threshold", config.get("significance_threshold").getAsDouble());
        JsonArray queries = new JsonArray();
        List<Long> addresses = new ArrayList<>();
        for (JsonElement item : config.getAsJsonArray("addresses")) addresses.add(address(item));
        Collections.sort(addresses);
        if (new HashSet<>(addresses).size() != addresses.size()) throw new IOException("Duplicate query addresses");
        try (FunctionDatabase db = BSimClientFactory.buildClient(BSimClientFactory.deriveBSimURL(config.get("database_url").getAsString()), false)) {
            if (!db.initialize()) throw new IOException(db.getLastError().message);
            QueryExeCount countQuery = new QueryExeCount();
            ResponseExe countResponse = countQuery.execute(db);
            if (countResponse == null || countResponse.recordCount != 1)
                throw new IOException("Expected exactly one indexed Android executable");
            if (db.getInfo().settings != index.get("signature_settings").getAsInt()) throw new IOException("Index signature settings changed");
            result.addProperty("signature_settings", db.getInfo().settings);
            GenSignatures generator = new GenSignatures(false);
            try {
                generator.setVectorFactory(db.getLSHVectorFactory());
                generator.openProgram(currentProgram, null, null, null, null, null);
                for (long va : addresses) {
                    monitor.checkCancelled();
                    JsonObject row = new JsonObject();
                    row.addProperty("pc_address", hex(va));
                    row.addProperty("pc_id", id(program, va));
                    JsonArray candidates = new JsonArray();
                    row.add("candidates", candidates);
                    Function function = currentProgram.getFunctionManager().getFunctionAt(toAddr(va - delta));
                    if (function == null) {
                        row.addProperty("status", "missing_function");
                        queries.add(row);
                        continue;
                    }
                    row.addProperty("body_bytes", function.getBody().getNumAddresses());
                    // Names are deliberately absent: reviewed PC labels must not inform retrieval.
                    try {
                        generator.getDescriptionManager().clearFunctions();
                        generator.scanFunction(function);
                        DescriptionManager manager = generator.getDescriptionManager();
                        if (manager.getMajorVersion() != index.get("signature_major").getAsInt() ||
                                manager.getMinorVersion() != index.get("signature_minor").getAsInt())
                            throw new IOException("Signature generator version changed");
                        FunctionDescription signature = manager.listAllFunctions().hasNext() ? manager.listAllFunctions().next() : null;
                        if (signature == null || signature.getSignatureRecord() == null) {
                            row.addProperty("status", "no_signature");
                        } else {
                            row.addProperty("self_significance", db.getLSHVectorFactory().getSelfSignificance(signature.getSignatureRecord().getLSHVector()));
                            QueryNearest query = new QueryNearest();
                            query.manage = manager;
                            query.max = config.get("max_candidates").getAsInt();
                            query.thresh = config.get("similarity_threshold").getAsDouble();
                            query.signifthresh = config.get("significance_threshold").getAsDouble();
                            ResponseNearest response = query.execute(db);
                            if (response == null) throw new IOException(db.getLastError().message);
                            List<SimilarityNote> notes = new ArrayList<>();
                            for (SimilarityResult match : response.result) {
                                Iterator<SimilarityNote> iter = match.iterator();
                                while (iter.hasNext()) {
                                    SimilarityNote note = iter.next();
                                    if (!note.getFunctionDescription().getExecutableRecord().getMd5().equals(target.get("md5").getAsString())) throw new IOException("Foreign executable in BSim database");
                                    notes.add(note);
                                }
                            }
                            notes.sort(Comparator.comparingDouble(SimilarityNote::getSimilarity).reversed()
                                .thenComparing(Comparator.comparingDouble(SimilarityNote::getSignificance).reversed())
                                .thenComparingLong(n -> n.getFunctionDescription().getAddress())
                                .thenComparing(n -> n.getFunctionDescription().getFunctionName()));
                            int rank = 0;
                            for (SimilarityNote note : notes) {
                                FunctionDescription description = note.getFunctionDescription();
                                long aa = description.getAddress() + targetDelta;
                                if (!description.getFunctionName().equals(indexedFunctions.get(aa)))
                                    throw new IOException("Candidate not in pinned BSim index manifest");
                                JsonObject candidate = new JsonObject();
                                candidate.addProperty("android_address", hex(aa));
                                candidate.addProperty("android_id", id(target, aa));
                                candidate.addProperty("name", description.getFunctionName());
                                candidate.addProperty("rank", ++rank);
                                candidate.addProperty("similarity", note.getSimilarity());
                                candidate.addProperty("significance", note.getSignificance());
                                candidates.add(candidate);
                            }
                            row.addProperty("status", notes.isEmpty() ? "no_candidates" : "ok");
                        }
                    } catch (Exception ex) {
                        candidates = new JsonArray();
                        row.add("candidates", candidates);
                        row.addProperty("status", "query_failure");
                        row.addProperty("reason", ex.getClass().getSimpleName() + ": " + ex.getMessage());
                    }
                    queries.add(row);
                    if (queries.size() % 10 == 0) println("BSim queries " + queries.size() + "/" + addresses.size());
                }
            } finally { generator.dispose(); }
        }
        result.add("queries", queries);
        write(output, result);
    }

    @Override public void run() throws Exception {
        if (currentProgram == null) throw new IOException("An analyzed program is required");
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IOException("Expected one JSON configuration path");
        JsonObject config = JsonParser.parseString(Files.readString(Path.of(args[0]))).getAsJsonObject();
        JsonObject program = validateProgram(config.getAsJsonObject("program"));
        Path output = Path.of(config.get("output").getAsString());
        String mode = config.get("mode").getAsString();
        if (mode.equals("index")) index(config, program, output);
        else if (mode.equals("query")) query(config, program, output);
        else throw new IOException("Unknown mode " + mode);
    }
}
