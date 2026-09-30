// Repair only explicitly reviewed entries in a disposable PC project.
// analyzeHeadless COPY_PARENT COPY_NAME -process PROGRAM -noanalysis
//   -scriptPath ISOLATED_SCRIPT_DIRECTORY -postScript RepairReviewedFunctions.java PACKET ORIGINAL REPORT
// Packet schema identification-reviewed-functions-v1:
// {binary_sha256,image_base:"0x400000",disposable_project:true,
//  source_project_file:"/absolute/original.gpr",
//  disposable_project_file:"/absolute/copy.gpr",functions:[
//  {entry:"0x514990",size:8,range_sha256:"...",complete_extent_reviewed:true}]}
// Does not repair ABI/type, follow external tails, clear conflicts or run analysis.
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
        AddressSet body;
        int size;
        String digest;
        boolean existing;
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
                Iterator<Function> overlaps = manager.getFunctionsOverlapping(r.body);
                while (overlaps.hasNext()) {
                    Function f = overlaps.next();
                    require(f.getEntryPoint().equals(r.entry) && f.getBody().equals(r.body), "Existing function conflict"); r.existing = true;
                }
                CodeUnit first = listing.getCodeUnitContaining(r.entry), last = listing.getCodeUnitContaining(r.end);
                require(first == null || first.getMinAddress().equals(r.entry), "Entry crosses existing code/data unit");
                require(last == null || last.getMaxAddress().compareTo(r.end) <= 0, "End crosses existing code/data unit");
                DataIterator data = listing.getDefinedData(r.body, true);
                require(!data.hasNext(), "Defined data conflicts with reviewed code; explicit separate review required");
                work.add(r);
            }
        }
        int transaction = currentProgram.startTransaction("Explicit reviewed callback repair"); boolean success = false;
        List<Map<String,Object>> outcomes = new ArrayList<>();
        try {
            for (Reviewed r : work) {
                monitor.checkCancelled();
                DisassembleCommand command = new DisassembleCommand(r.entry, r.body, true);
                command.enableCodeAnalysis(false);
                require(command.applyTo(currentProgram, monitor), "Restricted disassembly failed: " + command.getStatusMsg());
                require(r.body.contains(command.getDisassembledAddressSet()), "Disassembly escaped reviewed body");
                Address cursor = r.entry;
                while (cursor.compareTo(r.end) <= 0) {
                    Instruction instruction = listing.getInstructionAt(cursor);
                    if (instruction == null) {
                        // Reviewed complete extents may contain unreachable but
                        // explicitly decoded padding after an early return.
                        DisassembleCommand gap = new DisassembleCommand(cursor, r.body, true);
                        gap.enableCodeAnalysis(false);
                        require(gap.applyTo(currentProgram, monitor) && r.body.contains(gap.getDisassembledAddressSet()),
                                "Restricted gap disassembly failed");
                        instruction = listing.getInstructionAt(cursor);
                    }
                    require(instruction != null && r.body.contains(instruction.getMaxAddress()), "Range has undecoded bytes or crossing instruction");
                    cursor = instruction.getMaxAddress().add(1);
                }
                Function f = manager.getFunctionAt(r.entry);
                if (f == null) f = manager.createFunction(null, r.entry, r.body, SourceType.ANALYSIS);
                require(f != null && f.getBody().equals(r.body), "Created body differs from reviewed complete extent");
                Map<String,Object> outcome = new LinkedHashMap<>();
                outcome.put("entry", r.entry.toString()); outcome.put("size", r.size); outcome.put("range_sha256", r.digest);
                outcome.put("status", r.existing ? "already-exact" : "created"); outcomes.add(outcome);
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
