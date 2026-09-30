// Export an existing analyzed program without modifying it.
// Args: output-directory original-file signed-address-delta [reviewed-data-evidence.jsonl]
// Run analyzeHeadless with -readOnly -noanalysis.
import ghidra.app.script.GhidraScript;
import ghidra.framework.Application;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.*;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;

public class ExportReferenceGraph extends GhidraScript {
    private final Gson json = new GsonBuilder().serializeNulls().disableHtmlEscaping().create();
    private final Map<String, Map<String, Object>> nodes = new TreeMap<>();
    private final List<Map<String, Object>> edges = new ArrayList<>();
    private String binaryHash;
    private long delta;
    private final List<long[]> originalRanges = new ArrayList<>();

    private Map<String, Object> record(Object... values) {
        Map<String, Object> result = new LinkedHashMap<>();
        for (int i = 0; i < values.length; i += 2) result.put((String) values[i], values[i + 1]);
        return result;
    }

    private String va(Address address) {
        return String.format("0x%08x", address.getOffset() + delta);
    }

    private String id(Address address, String kind) {
        return binaryHash + ":" + address.getAddressSpace().getName() + ":" + va(address) + ":" + kind;
    }

    private boolean originalMemory(Address address) {
        MemoryBlock block = currentProgram.getMemory().getBlock(address);
        long normalized = address.getOffset() + delta;
        boolean owned = originalRanges.stream().anyMatch(range -> normalized >= range[0] && normalized < range[1]);
        return address.isMemoryAddress() && address.getAddressSpace().equals(currentProgram.getAddressFactory().getDefaultAddressSpace()) &&
            normalized >= 0 && normalized <= 0xffffffffL && owned && (block == null || !block.isExternalBlock());
    }

    private String referenceAddress(Address address) {
        if (originalMemory(address)) return va(address);
        MemoryBlock block = currentProgram.getMemory().getBlock(address);
        if (block != null && block.isExternalBlock()) return "EXTERNAL-BLOCK:" + address;
        return address.isMemoryAddress() ? "UNNORMALIZED:" + address : address.toString();
    }

    private String hash(Path path) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        try (InputStream stream = Files.newInputStream(path)) {
            byte[] buffer = new byte[65536];
            int count;
            while ((count = stream.read(buffer)) >= 0) digest.update(buffer, 0, count);
        }
        return HexFormat.of().formatHex(digest.digest());
    }

    private long u32(RandomAccessFile file, long offset) throws Exception {
        file.seek(offset);
        return Integer.toUnsignedLong(Integer.reverseBytes(file.readInt()));
    }

    // Read original header metadata only; no relocation or comparison implementation.
    private long originalImageBase(Path original) throws Exception {
        try (RandomAccessFile file = new RandomAccessFile(original.toFile(), "r")) {
            long magic = u32(file, 0);
            if (magic == 0x464c457fL) {
                file.seek(4);
                if (file.readUnsignedByte() != 1 || file.readUnsignedByte() != 1)
                    throw new IllegalArgumentException("Expected little-endian ELF32");
                long table = u32(file, 28);
                file.seek(42);
                int stride = Short.toUnsignedInt(Short.reverseBytes(file.readShort()));
                int count = Short.toUnsignedInt(Short.reverseBytes(file.readShort()));
                long base = Long.MAX_VALUE;
                for (int i = 0; i < count; i++) {
                    long entry = table + i * stride;
                    if (u32(file, entry) == 1) {
                        long start = u32(file, entry + 8);
                        base = Math.min(base, start);
                        originalRanges.add(new long[] {start, start + u32(file, entry + 20)});
                    }
                }
                if (base == Long.MAX_VALUE) throw new IllegalArgumentException("ELF has no load segment");
                return base;
            }
            if ((magic & 0xffff) == 0x5a4d) {
                long header = u32(file, 0x3c);
                if (u32(file, header) != 0x4550) throw new IllegalArgumentException("Invalid PE header");
                file.seek(header + 24);
                if (Short.toUnsignedInt(Short.reverseBytes(file.readShort())) != 0x10b)
                    throw new IllegalArgumentException("Expected PE32");
                long base = u32(file, header + 52);
                originalRanges.add(new long[] {base, base + u32(file, header + 80)});
                return base;
            }
            throw new IllegalArgumentException("Expected original ELF32 or PE32");
        }
    }

    private List<Map<String, Object>> aliases(Address address) {
        List<Map<String, Object>> result = new ArrayList<>();
        for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(address)) {
            result.add(record("name", symbol.getName(true), "source", symbol.getSource().toString(),
                "primary", symbol.isPrimary(), "symbol_type", symbol.getSymbolType().toString(),
                "provenance", "ghidra-symbol-table"));
        }
        result.sort(Comparator.comparing(json::toJson));
        return result;
    }

    private Map<String, Object> node(Address address, String kind) {
        String key = id(address, kind);
        if (nodes.containsKey(key)) return nodes.get(key);
        MemoryBlock block = currentProgram.getMemory().getBlock(address);
        boolean fileBacked = false;
        if (block != null)
            for (MemoryBlockSourceInfo info : block.getSourceInfos())
                if (info.contains(address) && info.getFileBytesOffset(address) >= 0) fileBacked = true;
        Map<String, Object> result = record("id", key, "key",
            (kind.equals("function") ? "fn:" : kind.equals("data-object") ? "object:" : "data:") + va(address),
            "kind", kind, "address", va(address), "address_space", address.getAddressSpace().getName(),
            "original_binary_owned", true,
            "aliases", aliases(address), "section", block == null ? null : block.getName(),
            "initialized", block == null ? null : fileBacked, "memory_initialized", block == null ? null : block.isInitialized(),
            "zero_filled", block == null ? null : !fileBacked, "size", null,
            "boundary_status", "unknown", "evidence", "ghidra-analysis");
        nodes.put(key, result);
        return result;
    }

    private Data owner(Address target) {
        Data data = currentProgram.getListing().getDefinedDataContaining(target);
        return data == null || data.getDataType().isNotYetDefined() ? null : data;
    }

    private String dataTarget(Address target) {
        Map<String, Object> site = node(target, "data-site");
        Data data = owner(target);
        if (data != null) {
            Map<String, Object> object = node(data.getAddress(), "data-object");
            object.put("size", data.getLength());
            object.put("boundary_status", "analysis-defined-data");
            object.put("data_type", data.getDataType().getPathName());
            object.put("extent_certainty", "analysis-only");
            site.put("owner", object.get("id"));
            site.put("offset", target.subtract(data.getAddress()));
        }
        return (String) site.get("id");
    }

    // A raw P-code load/store with a constant address establishes its access width.
    // Registers carrying an address and address-taking operations remain unknown.
    private Integer width(Instruction instruction, Address target, String role) {
        Set<Integer> widths = new HashSet<>();
        for (PcodeOp op : instruction.getPcode()) {
            if (role.equals("read") || role.equals("read-write")) {
                for (Varnode input : op.getInputs())
                    if (input.isAddress() && input.getAddress().equals(target)) widths.add(input.getSize());
            }
            Varnode output = op.getOutput();
            if ((role.equals("write") || role.equals("read-write")) && output != null &&
                output.isAddress() && output.getAddress().equals(target)) widths.add(output.getSize());
            boolean load = op.getOpcode() == PcodeOp.LOAD;
            boolean store = op.getOpcode() == PcodeOp.STORE;
            if (!load && !store) continue;
            Varnode pointer = op.getInput(1);
            if (!pointer.isConstant() || pointer.getOffset() != target.getOffset()) continue;
            if (load && (role.equals("read") || role.equals("read-write"))) widths.add(op.getOutput().getSize());
            if (store && (role.equals("write") || role.equals("read-write"))) widths.add(op.getInput(2).getSize());
        }
        return widths.size() == 1 ? widths.iterator().next() : null;
    }

    private void addEdge(Function function, Instruction instruction, Reference reference) {
        Address target = reference.getToAddress();
        RefType type = reference.getReferenceType();
        if (type.isData() && !originalMemory(target)) {
            if (target.isMemoryAddress()) edges.add(record("source", id(function.getEntryPoint(), "function"),
                "target", null, "site", va(instruction.getAddress()), "kind", "unresolved", "role", "address",
                "target_address", referenceAddress(target), "operand_index", reference.getOperandIndex(), "width", null,
                "raw_reference_type", type.toString(), "evidence", "outside-original-address-domain"));
            return;
        }
        boolean tail = type.isJump() && !function.getBody().contains(target);
        if (!type.isData() && !type.isCall() && !tail) return;
        String kind = type.isCall() ? "call" : tail ? "tail" : "access";
        String role = type.isCall() || tail ? (type.isComputed() ? "indirect" : "direct") :
            type.isRead() && type.isWrite() ? "read-write" : type.isRead() ? "read" : type.isWrite() ? "write" : "address";
        String targetId = null;
        Data data = null;
        if (originalMemory(target)) {
            Function callee = currentProgram.getFunctionManager().getFunctionAt(target);
            if (!kind.equals("access") && callee != null && !callee.isExternal()) targetId = id(target, "function");
            else {
                targetId = dataTarget(target);
                data = owner(target);
            }
        }
        Integer accessWidth = kind.equals("access") ? width(instruction, target, role) : null;
        edges.add(record("source", id(function.getEntryPoint(), "function"), "target", targetId,
            "site", va(instruction.getAddress()), "kind", kind, "role", role,
            "target_address", referenceAddress(target),
            "operand_index", reference.getOperandIndex(), "raw_reference_type", type.toString(),
            "owner", data == null ? null : id(data.getAddress(), "data-object"),
            "offset", data == null ? null : target.subtract(data.getAddress()), "width", accessWidth,
            "evidence", record("origin", "ghidra-reference", "source_type", reference.getSource().toString(),
                "width_origin", accessWidth == null ? "unknown" : "constant-address-pcode",
                "tail_certainty", tail ? "external-body-branch-candidate" : null)));
    }

    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 3 || args.length > 4) throw new IllegalArgumentException("output-directory original-file signed-address-delta [reviewed-data-evidence.jsonl] required");
        Path output = Paths.get(args[0]);
        Path original = Paths.get(args[1]);
        delta = Long.decode(args[2]);
        binaryHash = hash(original);
        if (!binaryHash.equalsIgnoreCase(currentProgram.getExecutableSHA256()))
            throw new IllegalArgumentException("Original SHA-256 differs from analyzed program");
        long originalBase = originalImageBase(original);
        if (currentProgram.getImageBase().getOffset() + delta != originalBase)
            throw new IllegalArgumentException("Address delta does not normalize original image base");
        Files.createDirectories(output);
        List<Map<String, Object>> blocks = new ArrayList<>();
        int byteChecks = 0;
        try (RandomAccessFile originalFile = new RandomAccessFile(original.toFile(), "r")) {
            for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
                if (!originalMemory(block.getStart())) continue;
                List<Map<String, Object>> sources = new ArrayList<>();
                for (MemoryBlockSourceInfo info : block.getSourceInfos()) {
                    long offset = info.getFileBytesOffset();
                    sources.add(record("start", va(info.getMinAddress()), "length", info.getLength(), "file_offset", offset));
                    if (block.isExecute() && offset >= 0 && info.getLength() >= 16) {
                        byte[] expected = new byte[16];
                        byte[] actual = new byte[16];
                        originalFile.seek(offset);
                        originalFile.readFully(expected);
                        currentProgram.getMemory().getBytes(info.getMinAddress(), actual);
                        if (!Arrays.equals(expected, actual)) throw new IllegalArgumentException("Executable source bytes differ at " + info.getMinAddress());
                        byteChecks++;
                    }
                }
                blocks.add(record("name", block.getName(), "start", va(block.getStart()), "size", block.getSize(),
                    "initialized", block.isInitialized(), "execute", block.isExecute(), "sources", sources));
            }
        }
        if (byteChecks == 0) throw new IllegalArgumentException("No executable block byte checks available");
        FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            monitor.checkCancelled();
            Function function = functions.next();
            if (function.isExternal() || !originalMemory(function.getEntryPoint())) continue;
            Map<String, Object> result = node(function.getEntryPoint(), "function");
            List<Map<String, Object>> ranges = new ArrayList<>();
            for (AddressRange range : function.getBody().getAddressRanges())
                ranges.add(record("start", va(range.getMinAddress()), "end_exclusive", String.format("0x%08x", range.getMaxAddress().getOffset() + delta + 1)));
            result.put("ranges", ranges);
            result.put("body_bytes", function.getBody().getNumAddresses());
            result.put("name", function.getName());
            result.put("name_provenance", function.getSymbol().getSource().toString());
            result.put("boundary_status", "analyzed-body");
            Function thunk = function.getThunkedFunction(false);
            result.put("thunk_target", thunk == null ? null : referenceAddress(thunk.getEntryPoint()));
            InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
            while (instructions.hasNext()) {
                Instruction instruction = instructions.next();
                for (Reference reference : instruction.getReferencesFrom()) {
                    addEdge(function, instruction, reference);
                }
                if (instruction.getFlowType().isComputed()) {
                    edges.add(record("source", id(function.getEntryPoint(), "function"), "target", null,
                        "site", va(instruction.getAddress()), "kind", "unresolved", "role", "indirect",
                        "target_address", null, "expression", instruction.toString(), "width", null,
                        "evidence", "ghidra-computed-flow"));
                }
            }
        }
        DataIterator objects = currentProgram.getListing().getDefinedData(true);
        while (objects.hasNext()) {
            monitor.checkCancelled();
            Data data = objects.next();
            if (!originalMemory(data.getAddress())) continue;
            dataTarget(data.getAddress());
        }
        SymbolIterator symbols = currentProgram.getSymbolTable().getAllSymbols(true);
        while (symbols.hasNext()) {
            Symbol symbol = symbols.next();
            Address address = symbol.getAddress();
            MemoryBlock block = currentProgram.getMemory().getBlock(address);
            if (originalMemory(address) && block != null && !block.isExecute()) dataTarget(address);
        }
        AddressIterator sourceSites = currentProgram.getReferenceManager().getReferenceSourceIterator(currentProgram.getMemory(), true);
        while (sourceSites.hasNext()) {
            monitor.checkCancelled();
            Address site = sourceSites.next();
            if (!originalMemory(site)) continue;
            if (currentProgram.getListing().getInstructionContaining(site) != null) continue;
            Data data = owner(site);
            String sourceId = data == null ? dataTarget(site) : id(data.getAddress(), "data-object");
            if (data != null) dataTarget(data.getAddress());
            for (Reference reference : currentProgram.getReferenceManager().getReferencesFrom(site)) {
                Address target = reference.getToAddress();
                if (!originalMemory(target)) {
                    if (target.isMemoryAddress()) edges.add(record("source", sourceId, "target", null,
                        "site", va(site), "kind", "unresolved", "role", "address", "width", null,
                        "target_address", referenceAddress(target), "raw_reference_type", reference.getReferenceType().toString(),
                        "evidence", "data-reference-outside-original-address-domain"));
                    continue;
                }
                Function function = currentProgram.getFunctionManager().getFunctionAt(target);
                String targetId = function == null || function.isExternal() ? dataTarget(target) : id(target, "function");
                edges.add(record("source", sourceId, "target", targetId,
                    "site", va(site), "kind", "pointer", "role", "address",
                    "target_address", va(target), "operand_index", reference.getOperandIndex(), "width", null,
                    "raw_reference_type", reference.getReferenceType().toString(), "evidence", record("origin", "ghidra-defined-data-reference",
                        "source_type", reference.getSource().toString(), "relocation_verified", false)));
            }
        }
        String reviewedEvidenceHash = null;
        if (args.length == 4) {
            Path evidencePath = Paths.get(args[3]);
            reviewedEvidenceHash = hash(evidencePath);
            for (String line : Files.readAllLines(evidencePath, StandardCharsets.UTF_8)) {
                if (line.isBlank()) continue;
                Map<?, ?> annotation = json.fromJson(line, Map.class);
                if (!binaryHash.equals(annotation.get("binary_sha256")))
                    throw new IllegalArgumentException("Reviewed evidence original hash differs");
                long normalizedAddress = Long.decode(annotation.get("address").toString());
                Address address = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(normalizedAddress - delta);
                if (!originalMemory(address) || !currentProgram.getMemory().contains(address))
                    throw new IllegalArgumentException("Reviewed address outside original memory");
                dataTarget(address);
                node(address, "data-site").put("reviewed_evidence", annotation);
            }
        }
        edges.sort(Comparator.comparing(json::toJson));
        writeLines(output.resolve("nodes.jsonl"), nodes.values());
        writeLines(output.resolve("edges.jsonl"), edges);
        List<Map<String, Object>> loadRanges = new ArrayList<>();
        for (long[] range : originalRanges) loadRanges.add(record("start", String.format("0x%08x", range[0]),
            "end_exclusive", String.format("0x%08x", range[1])));
        Map<String, Object> manifest = record("schema", "identification-reference-graph-v1",
            "binary_sha256", binaryHash, "binary_md5", currentProgram.getExecutableMD5(),
            "address_space", "ram", "address_transform", record("delta", delta, "program_image_base", currentProgram.getImageBase().toString(),
                "original_image_base", String.format("0x%08x", originalBase), "original_header_verified", true),
            "tool", "Ghidra " + Application.getApplicationVersion(), "language", currentProgram.getLanguageID().toString(),
            "compiler", currentProgram.getCompilerSpec().getCompilerSpecID().toString(),
            "program", currentProgram.getName(), "analysis", "existing-program-readonly-noanalysis",
            "nodes_file", "nodes.jsonl", "edges_file", "edges.jsonl", "node_count", nodes.size(), "edge_count", edges.size(),
            "sha256", record("nodes", hash(output.resolve("nodes.jsonl")), "edges", hash(output.resolve("edges.jsonl"))),
            "exporter_sha256", hash(Paths.get(getSourceFile().getAbsolutePath())),
            "reviewed_data_evidence_sha256", reviewedEvidenceHash,
            "original_image_ranges", loadRanges,
            "memory_blocks", blocks, "verification", record("program_original_sha256_equal", true, "executable_block_byte_checks", byteChecks),
            "limitations", Arrays.asList("Data extents are Ghidra analysis hypotheses", "External-body jumps are tail candidates", "No cross-ABI vtable slot inference", "Ghidra data references are not proven relocations"));
        Path temporary = output.resolve("manifest.json.tmp");
        Files.writeString(temporary, json.toJson(manifest) + "\n", StandardCharsets.UTF_8);
        Files.move(temporary, output.resolve("manifest.json"), StandardCopyOption.REPLACE_EXISTING, StandardCopyOption.ATOMIC_MOVE);
        println("Exported " + nodes.size() + " nodes / " + edges.size() + " edges; SHA-256 and " + byteChecks + " executable block byte controls verified.");
    }

    private void writeLines(Path path, Collection<Map<String, Object>> records) throws Exception {
        try (BufferedWriter writer = Files.newBufferedWriter(path, StandardCharsets.UTF_8)) {
            for (Map<String, Object> value : records) { writer.write(json.toJson(value)); writer.newLine(); }
        }
    }
}
