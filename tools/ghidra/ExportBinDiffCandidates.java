// Export a program for the optional BinDiff evidence channel.
// Requires the separately installed official BinExport Ghidra extension.
// BinDiff matches remain candidates; this script changes no program labels.
import ghidra.app.script.GhidraScript;
import ghidra.app.util.exporter.Exporter;
import ghidra.util.classfinder.ClassSearcher;
import java.io.File;

public class ExportBinDiffCandidates extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("Expected output .BinExport path");
        }
        // Reflection keeps ordinary script compilation independent of this optional extension.
        // Ghidra's OSGi bundle scanner also treats literal reflective class names
        // as imports. Build the optional name at runtime so other scripts remain usable.
        String exporterClass = String.join(".", "com", "google", "security", "binexport", "BinExportExporter");
        Exporter exporter = null;
        for (Class<? extends Exporter> type : ClassSearcher.getClasses(Exporter.class)) {
            if (type.getName().equals(exporterClass)) {
                exporter = type.getDeclaredConstructor().newInstance();
                break;
            }
        }
        if (exporter == null) {
            throw new IllegalStateException("Install the official BinExport extension first");
        }
        File output = new File(args[0]);
        if (output.exists()) {
            throw new IllegalArgumentException("Refusing to replace existing export: " + output);
        }
        // Default exporter settings preserve the program's analyzed addresses and names.
        // Address normalization must follow the independently validated graph manifest.
        if (!exporter.export(output, currentProgram, currentProgram.getMemory(), monitor)) {
            throw new IllegalStateException("BinExport failed: " + exporter.getMessageLog());
        }
        println("BinExport complete: " + output.getAbsolutePath());
        println("Image base: " + currentProgram.getImageBase());
        println("Analyzed functions: " + currentProgram.getFunctionManager().getFunctionCount());
    }
}
