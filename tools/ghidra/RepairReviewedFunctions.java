// Repair only explicitly reviewed entries in a disposable PC project.
// analyzeHeadless COPY_PARENT COPY_NAME -process PROGRAM -noanalysis
//   -scriptPath ISOLATED_SCRIPT_DIRECTORY -postScript RepairReviewedFunctions.java PACKET ORIGINAL REPORT
// Packet schema identification-reviewed-functions-v1:
// {binary_sha256,image_base:"0x400000",disposable_project:true,
//  source_project_file:"/absolute/original.gpr",
//  disposable_project_file:"/absolute/copy.gpr",functions:[
//  {entry:"0x514990",size:8,range_sha256:"...",complete_extent_reviewed:true,
//   superseded_instructions:[{start:"0x...",size:2,range_sha256:"..."}]}]}
// Optional excluded_ranges pin padding or switch-table32 original bytes;
// tables pin guard_reviewed:true and every original target, and may pin an
// exact retained existing_data:{data_type_path}. No data type is created.
// Optional replacement_body pins exact old ranges+hashes/body_bytes before
// extending that same existing entry to the whole reviewed mixed extent.
// Optional superseded instructions require independently reviewed exact listing
// bounds and original bytes. Other conflicts remain fatal; no analysis is run.
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import com.google.gson.*;
import java.nio.file.*;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;

public class RepairReviewedFunctions extends GhidraScript {
    private static final String ORIGINAL = "239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113";
    private static final long BASE = 0x400000L;
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();
    private static class Reviewed {
        Address entry, end;
        AddressSet body, code, oldBody;
        boolean extension;
        JsonObject replacement;
        List<Excluded> excluded = new ArrayList<>();
        int size;
        String digest;
        boolean existing;
        List<Superseded> superseded = new ArrayList<>();
    }
    private static class Excluded {
        Address start, end;
        int size;
        String kind, digest, dataType;
        JsonObject spec;
        List<Address> targets = new ArrayList<>();
    }
    private static class Superseded {
        Address start, end;
        int size;
        String digest;
    }
    private void require(boolean value, String error) {
        if (!value) throw new IllegalArgumentException(error);
    }
    private long number(JsonElement value) {
        String s = value.getAsString();
        return s.startsWith("0x") ? Long.parseLong(s.substring(2), 16) : Long.parseLong(s);
    }
    private String digest(byte[] bytes) throws Exception {
        return HexFormat.of().formatHex(MessageDigest.getInstance("SHA-256").digest(bytes));
    }
    private String fileDigest(Path file) throws Exception {
        MessageDigest hash = MessageDigest.getInstance("SHA-256");
        try (InputStream stream = Files.newInputStream(file)) {
            byte[] chunk = new byte[65536]; int n;
            while ((n = stream.read(chunk)) != -1) hash.update(chunk, 0, n);
        }
        return HexFormat.of().formatHex(hash.digest());
    }
    private long u32(RandomAccessFile file, long offset) throws Exception {
        file.seek(offset); return Integer.toUnsignedLong(Integer.reverseBytes(file.readInt()));
    }
    private int u16(RandomAccessFile file, long offset) throws Exception {
        file.seek(offset); return Short.toUnsignedInt(Short.reverseBytes(file.readShort()));
    }
    private byte[] originalBytes(RandomAccessFile file, long entry, int size) throws Exception {
        require(u16(file, 0) == 0x5a4d, "Not PE MZ");
        long header = u32(file, 0x3c);
        require(u32(file, header) == 0x4550 && u16(file, header + 4) == 0x14c, "Not x86 PE");
        require(u16(file, header + 24) == 0x10b && u32(file, header + 52) == BASE, "Original PE base/format mismatch");
        int sections = u16(file, header + 6), optional = u16(file, header + 20);
        require(sections > 0 && sections <= 96, "Invalid PE section count");
        long table = header + 24 + optional, rva = entry - BASE;
        for (int i = 0; i < sections; i++) {
            long row = table + 40L * i, start = u32(file, row + 12), rawSize = u32(file, row + 16);
            if (rva >= start && rva + size <= start + rawSize) {
                require((u32(file, row + 36) & 0x20000000L) != 0, "Range not in executable original section");
                long offset = u32(file, row + 20) + rva - start;
                require(offset + size <= file.length(), "Original range truncated");
                byte[] bytes = new byte[size]; file.seek(offset); file.readFully(bytes); return bytes;
            }
        }
        throw new IllegalArgumentException("Range not wholly original file-backed executable bytes");
    }
    @Override public void run() throws Exception {
        String[] args = getScriptArgs();
        require(args.length == 3, "Args: packet.json original.exe report.json");
        Path packetPath = Paths.get(args[0]), original = Paths.get(args[1]), reportPath = Paths.get(args[2]);
        require(Files.size(packetPath) <= 65536, "Packet exceeds64KiB");
        JsonObject packet;
        try (Reader reader = Files.newBufferedReader(packetPath)) { packet = JsonParser.parseReader(reader).getAsJsonObject(); }
        require(packet.get("schema").getAsString().equals("identification-reviewed-functions-v1"), "Wrong packet schema");
        require(packet.get("disposable_project").getAsBoolean(), "Disposable project declaration required");
        Path expectedProject = Paths.get(packet.get("disposable_project_file").getAsString()).toRealPath();
        Path actualProject = currentProgram.getDomainFile().getProjectLocator().getMarkerFile().toPath().toRealPath();
        require(expectedProject.equals(actualProject), "Project differs from explicit disposable copy");
        Path sourceProject = Paths.get(packet.get("source_project_file").getAsString()).toRealPath();
        require(!Files.isSameFile(sourceProject, actualProject), "Refusing retained source project");
        require(!Files.isSameFile(sourceProject.resolveSibling(sourceProject.getFileName().toString().replace(".gpr", ".rep")),
                                  actualProject.resolveSibling(actualProject.getFileName().toString().replace(".gpr", ".rep"))),
                "Source and disposable project storage are identical");
        require(packet.get("binary_sha256").getAsString().equals(ORIGINAL) && fileDigest(original).equals(ORIGINAL), "Original SHA256 mismatch");
        require(ORIGINAL.equalsIgnoreCase(currentProgram.getExecutableSHA256()), "Program original SHA256 mismatch");
        require(number(packet.get("image_base")) == BASE && currentProgram.getImageBase().getOffset() == BASE, "Image base mismatch");
        require(currentProgram.getLanguageID().toString().equals("x86:LE:32:default"), "PC x86 language required");
        JsonArray functions = packet.getAsJsonArray("functions");
        require(functions.size() > 0 && functions.size() <= 32, "Reviewed batch must contain1..32 functions");
        List<Reviewed> work = new ArrayList<>(); AddressSet complete = new AddressSet();
        Listing listing = currentProgram.getListing(); FunctionManager manager = currentProgram.getFunctionManager();
        try (RandomAccessFile file = new RandomAccessFile(original.toFile(), "r")) {
            for (JsonElement value : functions) {
                JsonObject row = value.getAsJsonObject(); Reviewed r = new Reviewed();
                require(row.get("complete_extent_reviewed").getAsBoolean(), "Complete extent review required");
                long entry = number(row.get("entry")), size = number(row.get("size"));
                require(entry >= BASE && entry <= 0xffffffffL && size > 0 && size <= 65536 && entry + size <= 0x100000000L, "Invalid reviewed range");
                r.size = (int)size; r.entry = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(entry);
                r.end = r.entry.add(size - 1); r.body = new AddressSet(r.entry, r.end);
                require(!complete.intersects(r.body), "Reviewed ranges overlap"); complete.add(r.body);
                MemoryBlock block = currentProgram.getMemory().getBlock(r.entry);
                require(block != null && block.isExecute() && block.isInitialized() && block.contains(r.end), "Range not in initialized executable program block");
                byte[] bytes = originalBytes(file, entry, r.size), programBytes = new byte[r.size];
                require(currentProgram.getMemory().getBytes(r.entry, programBytes) == r.size && Arrays.equals(bytes, programBytes), "Program range differs from original bytes");
                r.digest = digest(bytes); require(r.digest.equals(row.get("range_sha256").getAsString()), "Reviewed range SHA256 mismatch");
                r.code = new AddressSet(r.body);
                if (row.has("excluded_ranges")) {
                    JsonArray exclusions = row.getAsJsonArray("excluded_ranges");
                    require(exclusions.size() <= 32, "Too many excluded ranges");
                    AddressSet used = new AddressSet();
                    for (JsonElement item : exclusions) {
                        JsonObject spec = item.getAsJsonObject(); Excluded x = new Excluded();
                        long low = number(spec.get("start")), width = number(spec.get("size"));
                        require(low > entry && width > 0 && width <= 65536 && low + width <= entry + size,
                                "Excluded range escapes function");
                        x.start = r.entry.getAddressSpace().getAddress(low); x.size = (int)width; x.end = x.start.add(width-1);
                        require(!used.intersects(x.start,x.end), "Excluded ranges overlap"); used.add(x.start,x.end);
                        x.kind = spec.get("kind").getAsString();
                        require(x.kind.equals("padding") || x.kind.equals("switch-table32"), "Unknown exclusion kind");
                        byte[] excludedBytes = originalBytes(file, low, x.size); x.digest = digest(excludedBytes);
                        require(x.digest.equals(spec.get("range_sha256").getAsString()), "Excluded bytes hash mismatch");
                        if (x.kind.equals("switch-table32")) {
                            require(low % 4 == 0 && width % 4 == 0 && new JsonPrimitive(true).equals(spec.get("guard_reviewed")),
                                    "Aligned table and explicit guard review required");
                            JsonArray targets = spec.getAsJsonArray("targets");
                            require(targets.size() == width/4, "One exact target per table cell required");
                            for (int i=0; i<targets.size(); i++) {
                                long target = number(targets.get(i));
                                long actual = Integer.toUnsignedLong(java.nio.ByteBuffer.wrap(excludedBytes,i*4,4)
                                        .order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt());
                                require(target == actual && target >= entry && target < entry + size,
                                        "Table cell differs from explicit original target");
                                x.targets.add(r.entry.getAddressSpace().getAddress(target));
                            }
                            if (spec.has("existing_data")) {
                                JsonObject owner = spec.getAsJsonObject("existing_data");
                                require(owner.size()==1 && owner.has("data_type_path"), "Exact existing data type required");
                                x.dataType = owner.get("data_type_path").getAsString();
                                Data defined = listing.getDefinedDataAt(x.start);
                                require(defined != null && defined.getMaxAddress().equals(x.end) &&
                                        defined.getDataType().getPathName().equals(x.dataType), "Existing table owner differs from review");
                            }
                        } else {
                            require(!spec.has("targets") && !spec.has("guard_reviewed") && !spec.has("existing_data"),
                                    "Padding cannot authorize table metadata");
                        }
                        x.spec = spec.deepCopy(); r.excluded.add(x); r.code.delete(x.start,x.end);
                    }
                    for (Excluded x : r.excluded) for (Address target : x.targets)
                        require(r.code.contains(target), "Table target lies in excluded bytes");
                }
                if (row.has("replacement_body")) {
                    require(!r.excluded.isEmpty(), "Body extension requires mixed review");
                    r.replacement = row.getAsJsonObject("replacement_body").deepCopy();
                    JsonArray oldRanges = r.replacement.getAsJsonArray("ranges");
                    require(oldRanges.size()>0 && oldRanges.size()<=32, "Expected bounded old body ranges");
                    r.oldBody = new AddressSet(); long oldSize=0;
                    for (JsonElement oldRange : oldRanges) {
                        JsonObject old = oldRange.getAsJsonObject();
                        long low = number(old.get("start")), high = number(old.get("end_exclusive"));
                        require(low>=entry && high>low && high<=entry+size, "Old range outside replacement");
                        Address a = r.entry.getAddressSpace().getAddress(low), b = r.entry.getAddressSpace().getAddress(high-1);
                        require(!r.oldBody.intersects(a,b) && r.code.contains(new AddressSet(a,b)), "Old body overlaps/excludes code");
                        require(digest(originalBytes(file,low,(int)(high-low))).equals(old.get("range_sha256").getAsString()),
                                "Old body original range hash mismatch");
                        r.oldBody.add(a,b); oldSize += high-low;
                    }
                    require(r.oldBody.getMinAddress().equals(r.entry) && oldSize<size &&
                            oldSize == number(r.replacement.get("body_bytes")), "Old body is not exact strict subset");
                }
                Iterator<Function> overlaps = manager.getFunctionsOverlapping(r.body);
                while (overlaps.hasNext()) {
                    Function f = overlaps.next();
                    require(f.getEntryPoint().equals(r.entry) && f.getBody().equals(r.oldBody != null ? r.oldBody : r.body), "Existing function conflict");
                    r.existing = true; r.extension = r.oldBody != null;
                }
                require(r.oldBody == null || r.extension, "Approved old body is missing");
                CodeUnit first = listing.getCodeUnitContaining(r.entry), last = listing.getCodeUnitContaining(r.end);
                require(first == null || first.getMinAddress().equals(r.entry), "Entry crosses existing code/data unit");
                require(last == null || last.getMaxAddress().compareTo(r.end) <= 0, "End crosses existing code/data unit");
                DataIterator data = listing.getDefinedData(r.body, true);
                while (data.hasNext()) {
                    Data defined = data.next(); boolean allowed = false;
                    for (Excluded x : r.excluded) allowed |= x.dataType != null &&
                        defined.getMinAddress().equals(x.start) && defined.getMaxAddress().equals(x.end) &&
                        defined.getDataType().getPathName().equals(x.dataType);
                    require(allowed, "Defined data conflicts outside explicitly reviewed exact table owners");
                }
                if (row.has("superseded_instructions")) {
                    JsonArray prior = row.getAsJsonArray("superseded_instructions");
                    require(prior.size() <= 32 && (!r.existing || r.extension || prior.size() == 0),
                            "Cannot supersede instructions in an existing function");
                    AddressSet cleared = new AddressSet();
                    for (JsonElement old : prior) {
                        JsonObject spec = old.getAsJsonObject(); Superseded s = new Superseded();
                        long start = number(spec.get("start")), width = number(spec.get("size"));
                        require(start > entry && width > 0 && width <= 15 && start + width <= entry + size,
                                "Superseded instruction outside reviewed body");
                        s.start = r.entry.getAddressSpace().getAddress(start);
                        s.size = (int)width; s.end = s.start.add(width - 1);
                        require(!cleared.intersects(s.start, s.end), "Superseded instructions overlap");
                        cleared.add(s.start, s.end);
                        Instruction instruction = listing.getInstructionAt(s.start);
                        require(instruction != null && instruction.getMaxAddress().equals(s.end),
                                "Superseded listing instruction differs from explicit bounds");
                        byte[] oldBytes = originalBytes(file, start, s.size);
                        s.digest = digest(oldBytes);
                        require(s.digest.equals(spec.get("range_sha256").getAsString()),
                                "Superseded instruction original SHA256 mismatch");
                        require(r.oldBody == null || !r.oldBody.intersects(s.start,s.end),
                                "Cannot clear instructions in approved old body");
                        r.superseded.add(s);
                    }
                }
                InstructionIterator instructions = listing.getInstructions(r.body,true);
                while (instructions.hasNext()) {
                    Instruction instruction = instructions.next();
                    if (r.code.contains(new AddressSet(instruction.getMinAddress(),instruction.getMaxAddress()))) continue;
                    boolean approved = false;
                    for (Superseded old : r.superseded) approved |= old.start.equals(instruction.getMinAddress()) && old.end.equals(instruction.getMaxAddress());
                    require(approved, "Existing instruction conflicts with reviewed excluded bytes");
                }
                work.add(r);
            }
        }
        int transaction = currentProgram.startTransaction("Explicit reviewed callback repair"); boolean success = false;
        List<Map<String,Object>> outcomes = new ArrayList<>();
        try {
            for (Reviewed r : work) {
                monitor.checkCancelled();
                List<Map<String,Object>> replaced = new ArrayList<>();
                for (Superseded s : r.superseded) {
                    listing.clearCodeUnits(s.start, s.end, false);
                    Map<String,Object> old = new LinkedHashMap<>();
                    old.put("start", s.start.toString()); old.put("size", s.size);
                    old.put("range_sha256", s.digest); old.put("status", "cleared"); replaced.add(old);
                }
                DisassembleCommand command = new DisassembleCommand(r.entry, r.code, true);
                command.enableCodeAnalysis(false);
                require(command.applyTo(currentProgram, monitor), "Restricted disassembly failed: " + command.getStatusMsg());
                require(r.code.contains(command.getDisassembledAddressSet()), "Disassembly escaped reviewed body");
                AddressRangeIterator codeRanges = r.code.getAddressRanges();
                while (codeRanges.hasNext()) {
                    AddressRange codeRange = codeRanges.next(); Address cursor = codeRange.getMinAddress();
                    while (cursor.compareTo(codeRange.getMaxAddress()) <= 0) {
                        Instruction instruction = listing.getInstructionAt(cursor);
                        if (instruction == null) {
                            DisassembleCommand gap = new DisassembleCommand(cursor,r.code,true);
                            gap.enableCodeAnalysis(false);
                            require(gap.applyTo(currentProgram,monitor) && r.code.contains(gap.getDisassembledAddressSet()),
                                    "Restricted gap disassembly failed");
                            instruction = listing.getInstructionAt(cursor);
                        }
                        require(instruction != null && r.code.contains(new AddressSet(cursor,instruction.getMaxAddress())),
                                "Range has undecoded bytes or crossing instruction at " + cursor + " in " + r.entry);
                        cursor = instruction.getMaxAddress().add(1);
                    }
                }
                for (Excluded x : r.excluded) {
                    for (Address target : x.targets)
                        require(listing.getInstructionAt(target) != null && r.code.contains(target),
                                "Reviewed table target is not an exact decoded code boundary");
                    require(!listing.getInstructions(new AddressSet(x.start,x.end),true).hasNext(),
                            "Disassembly created instructions in reviewed excluded bytes");
                    if (x.dataType != null) {
                        Data owner = listing.getDefinedDataAt(x.start);
                        require(owner != null && owner.getMaxAddress().equals(x.end) &&
                                owner.getDataType().getPathName().equals(x.dataType),
                                "Repair changed explicitly retained table owner");
                    }
                }
                Function f = manager.getFunctionAt(r.entry);
                if (r.extension) {
                    require(f != null && f.getBody().equals(r.oldBody), "Old body changed after preflight");
                    f.setBody(r.body);
                }
                if (f == null) f = manager.createFunction(null, r.entry, r.body, SourceType.ANALYSIS);
                require(f != null && f.getBody().equals(r.body), "Created body differs from reviewed complete extent");
                byte[] unchanged = new byte[r.size];
                require(currentProgram.getMemory().getBytes(r.entry, unchanged) == r.size &&
                        digest(unchanged).equals(r.digest), "Repair changed original program bytes");
                Map<String,Object> outcome = new LinkedHashMap<>();
                outcome.put("entry", r.entry.toString()); outcome.put("size", r.size); outcome.put("range_sha256", r.digest);
                outcome.put("superseded_instructions", replaced);
                if (!r.excluded.isEmpty()) {
                    List<JsonObject> exclusions = new ArrayList<>();
                    for (Excluded x : r.excluded) exclusions.add(x.spec);
                    outcome.put("excluded_ranges", exclusions);
                }
                if (r.replacement != null) outcome.put("replacement_body", r.replacement);
                outcome.put("status", r.extension ? "extended-reviewed" : r.existing ? "already-exact" : "created"); outcomes.add(outcome);
            }
            success = true;
        } finally { currentProgram.endTransaction(transaction, success); }
        Map<String,Object> report = new LinkedHashMap<>(); report.put("schema", "identification-reviewed-function-repair-v1");
        report.put("binary_sha256", ORIGINAL); report.put("packet_sha256", fileDigest(packetPath));
        report.put("disposable_project_file", actualProject.toString()); report.put("functions", outcomes);
        report.put("analysis", "none; range-restricted disassembly only");
        try (Writer writer = Files.newBufferedWriter(reportPath)) { gson.toJson(report, writer); }
        println("Repaired " + outcomes.size() + " explicitly reviewed functions; save disposable program only.");
    }
}
