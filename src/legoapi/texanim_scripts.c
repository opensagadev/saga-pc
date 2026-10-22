#include "saga.h"

extern char **TexAnimScriptTablePc;
void __cdecl NuTexAnimScriptMissingPc(char *path);

void __cdecl InitTexAnimScripts(char **table) {
    char path[0x3c];
    char **entry;
    TexAnimScriptTablePc = table;
    if (table == NULL) {
        return;
    }
    for (entry = table; *entry != NULL; ++entry) {
        permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
        NuStrCpy(path, "stuff\\ats\\");
        NuStrCat(path, *entry);
        NuStrCat(path, ".ats");
        if (NuTexAnimProgReadScript(path, &permbuffer_ptr, permbuffer_end, (i32)DEFAULTFPS) == NULL) {
            NuTexAnimScriptMissingPc(path);
        }
    }
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 15) & ~15u;
}
