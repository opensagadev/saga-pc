// Explicit reviewed typed-data repair; disposable project only, no analysis.
// Args PACKET ORIGINAL REPORT. Schema identification-reviewed-data-v1:
// binary_sha256,image_base,disposable_project,source_project_file,disposable_project_file;
// bounds:[{start,size,range_sha256}], superseded:[{start,size,data_type_path,range_sha256}],
// definitions:[{start,size,kind:"char-array"|"float64",range_sha256}].
// Every touched byte must be inside packet bounds. Only exact superseded data
// may be cleared; all other defined data/instructions/functions are conflicts.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import com.google.gson.*;
import java.nio.file.*;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;

public class RepairReviewedData extends GhidraScript {
    private static final String ORIGINAL = "239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113";
    private static final long BASE = 0x400000L;
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();
    private static class Item {
        Address start, end; AddressSet range; int size; String hash, type, kind;
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
                require((u32(file, row + 36) & 0x20000000L) == 0, "Range must be non-executable original data section");
                long offset = u32(file, row + 20) + rva - start;
                require(offset + size <= file.length(), "Original range truncated");
                byte[] bytes = new byte[size]; file.seek(offset); file.readFully(bytes); return bytes;
            }
        }
        throw new IllegalArgumentException("Range not wholly original file-backed data bytes");
    }
    private Item item(JsonObject row) {
        Item i = new Item(); long start = number(row.get("start")), size = number(row.get("size"));
        require(start >= BASE && start <= 0xffffffffL && size > 0 && size <= 65536 && start + size <= 0x100000000L, "Invalid data range");
        i.start = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(start);
        i.size = (int)size; i.end = i.start.add(size - 1); i.range = new AddressSet(i.start, i.end);
        i.hash = row.get("range_sha256").getAsString();
        if (row.has("data_type_path")) i.type = row.get("data_type_path").getAsString();
        if (row.has("kind")) i.kind = row.get("kind").getAsString();
        return i;
    }
    private void verifyBytes(Item i, RandomAccessFile file) throws Exception {
        MemoryBlock block = currentProgram.getMemory().getBlock(i.start);
        require(block != null && block.isInitialized() && !block.isExecute() && block.contains(i.end), "Range must be initialized non-executable data");
        byte[] bytes = originalBytes(file, i.start.getOffset(), i.size), actual = new byte[i.size];
        require(currentProgram.getMemory().getBytes(i.start, actual) == i.size && Arrays.equals(bytes, actual), "Program differs from original data bytes");
        require(digest(bytes).equals(i.hash), "Reviewed data hash mismatch");
    }
    @Override public void run() throws Exception {
        String[] args = getScriptArgs(); require(args.length == 3, "Args: packet original report");
        Path packetPath = Paths.get(args[0]), original = Paths.get(args[1]), reportPath = Paths.get(args[2]);
        require(Files.size(packetPath) <= 65536, "Packet exceeds64KiB");
        JsonObject packet;
        try (Reader reader = Files.newBufferedReader(packetPath)) { packet = JsonParser.parseReader(reader).getAsJsonObject(); }
        require(packet.get("schema").getAsString().equals("identification-reviewed-data-v1"), "Wrong packet schema");
        require(packet.get("disposable_project").getAsBoolean(), "Disposable declaration required");
        Path actual = currentProgram.getDomainFile().getProjectLocator().getMarkerFile().toPath().toRealPath();
        Path copied = Paths.get(packet.get("disposable_project_file").getAsString()).toRealPath();
        Path source = Paths.get(packet.get("source_project_file").getAsString()).toRealPath();
        require(actual.equals(copied) && !Files.isSameFile(actual, source), "Refusing source or unpinned project");
        require(!Files.isSameFile(source.resolveSibling(source.getFileName().toString().replace(".gpr", ".rep")),
                                  actual.resolveSibling(actual.getFileName().toString().replace(".gpr", ".rep"))), "Same source/project storage");
        require(packet.get("binary_sha256").getAsString().equals(ORIGINAL) && fileDigest(original).equals(ORIGINAL)
                && ORIGINAL.equalsIgnoreCase(currentProgram.getExecutableSHA256()), "Original SHA256 mismatch");
        require(number(packet.get("image_base")) == BASE && currentProgram.getImageBase().getOffset() == BASE
                && currentProgram.getLanguageID().toString().equals("x86:LE:32:default"), "PC base/language mismatch");
        Listing listing = currentProgram.getListing();
        AddressSet bounds = new AddressSet(), replacement = new AddressSet();
        List<Item> boundItems = new ArrayList<>(), old = new ArrayList<>(), fresh = new ArrayList<>();
        try (RandomAccessFile file = new RandomAccessFile(original.toFile(), "r")) {
            for (JsonElement value : packet.getAsJsonArray("bounds")) {
                Item i = item(value.getAsJsonObject()); require(!bounds.intersects(i.range), "Bounds overlap");
                verifyBytes(i, file); bounds.add(i.range); boundItems.add(i);
            }
            require(boundItems.size() > 0 && boundItems.size() <= 16 && bounds.getNumAddresses() <= 65536, "Excessive reviewed bounds");
            require(!listing.getInstructions(bounds, true).hasNext(), "Instructions in data repair bounds");
            for (AddressRange range : bounds)
                require(listing.getInstructionContaining(range.getMinAddress()) == null
                    && listing.getInstructionContaining(range.getMaxAddress()) == null, "Instruction crosses repair boundary");
            require(!currentProgram.getFunctionManager().getFunctionsOverlapping(bounds).hasNext(), "Function body in data repair bounds");
            for (JsonElement value : packet.getAsJsonArray("superseded")) {
                Item i = item(value.getAsJsonObject()); require(bounds.contains(i.range), "Superseded outside bounds");
                require(!replacement.intersects(i.range), "Superseded definitions overlap"); verifyBytes(i, file);
                Data d = listing.getDefinedDataAt(i.start);
                require(d != null && d.getLength() == i.size && d.getDataType().getPathName().equals(i.type), "Actual superseded start/length/type differs from reviewed packet");
                replacement.add(i.range); old.add(i);
            }
            require(old.size() > 0 && old.size() <= 16, "Expected1..16 explicit superseded definitions");
            DataIterator existing = listing.getDefinedData(bounds, true);
            while (existing.hasNext()) {
                Data d = existing.next();
                require(old.stream().anyMatch(i -> d.getAddress().equals(i.start) && d.getLength() == i.size), "Unreviewed existing data in bounds");
            }
            for (AddressRange range : bounds) {
                Data first = listing.getDefinedDataContaining(range.getMinAddress()), last = listing.getDefinedDataContaining(range.getMaxAddress());
                require(first == null || replacement.contains(first.getMinAddress(), first.getMaxAddress()), "Data crosses bound start");
                require(last == null || replacement.contains(last.getMinAddress(), last.getMaxAddress()), "Data crosses bound end");
            }
            AddressSet newRanges = new AddressSet();
            for (JsonElement value : packet.getAsJsonArray("definitions")) {
                Item i = item(value.getAsJsonObject()); require(bounds.contains(i.range) && !newRanges.intersects(i.range), "New definitions outside bounds or overlapping");
                require("char-array".equals(i.kind) || "float64".equals(i.kind), "Unsupported reviewed data type");
                require(!"float64".equals(i.kind) || i.size == 8, "float64 requires8 bytes");
                verifyBytes(i, file);
                if ("char-array".equals(i.kind)) {
                    byte[] bytes = originalBytes(file, i.start.getOffset(), i.size);
                    require(bytes[i.size - 1] == 0, "char-array requires complete terminal NUL");
                    for (int k = 0; k < i.size - 1; k++) require(bytes[k] >= 32 && bytes[k] <= 126, "char-array must be exact printable literal with first NUL last");
                }
                newRanges.add(i.range); fresh.add(i);
            }
            require(fresh.size() > 0 && fresh.size() <= 16, "Expected1..16 definitions");
        }
        int tx = currentProgram.startTransaction("Explicit reviewed data repair"); boolean success = false;
        List<Map<String,Object>> outcomes = new ArrayList<>();
        try {
            for (Item i : old) { monitor.checkCancelled(); listing.clearCodeUnits(i.start, i.end, false, monitor); }
            for (Item i : fresh) {
                monitor.checkCancelled(); DataType type = "float64".equals(i.kind) ? DoubleDataType.dataType
                    : new ArrayDataType(CharDataType.dataType, i.size, 1);
                Data d = listing.createData(i.start, type, i.size);
                require(d != null && d.getLength() == i.size, "Created data extent mismatch");
                Map<String,Object> row = new LinkedHashMap<>(); row.put("start", i.start.toString()); row.put("size", i.size);
                row.put("kind", i.kind); row.put("status", "created"); row.put("data_type_path", d.getDataType().getPathName()); row.put("range_sha256", i.hash); outcomes.add(row);
            }
            try (RandomAccessFile file = new RandomAccessFile(original.toFile(), "r")) {
                for (Item i : boundItems) verifyBytes(i, file);
            }
            require(!listing.getInstructions(bounds, true).hasNext()
                && !currentProgram.getFunctionManager().getFunctionsOverlapping(bounds).hasNext(), "Code appeared during data repair");
            success = true;
        } finally { currentProgram.endTransaction(tx, success); }
        Map<String,Object> report = new LinkedHashMap<>(); report.put("schema", "identification-reviewed-data-repair-v1");
        report.put("binary_sha256", ORIGINAL); report.put("packet_sha256", fileDigest(packetPath));
        report.put("disposable_project_file", actual.toString()); report.put("definitions", outcomes);
        report.put("superseded", packet.get("superseded")); report.put("bounds", packet.get("bounds"));
        report.put("analysis", "none; explicit typed-data replacement only; original bytes unchanged");
        try (Writer writer = Files.newBufferedWriter(reportPath)) { gson.toJson(report, writer); }
        println("Repaired " + fresh.size() + " explicitly reviewed data definitions in disposable project.");
    }
}
