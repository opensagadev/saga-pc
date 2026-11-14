#include "saga.h"

void __cdecl StartupOpenDats(const char **names, i32 count, VARIPTR *arena) {
    i32 index;

    StartupDatCount = count;
    if (names != NULL && count > 0) {
        for (index = 0; index < StartupDatCount; index++) {
            StartupDatList[index] = NuDatOpen(names[index], arena, NULL);
        }
        NuDatSet(StartupDatList[0]);
        NuSound3SetDat(StartupDatList[0]);
    }
}

void __cdecl NuWin32SetDFS(i32 index) {
    if (StartupDatCount > 0 && StartupDatList[0] != NULL) {
        NuDatSet(StartupDatList[index]);
    }
}

void fun_006d34f0(void) {
    if (PcWindowOptions.usehires != 0 && StartupDatCount > 0 && StartupDatList[0] != NULL) {
        NuDatSet(StartupDatList[0]);
    }
}

void fun_006d3520(void) {
    if (PcWindowOptions.usehires != 0 && StartupDatCount > 0 && StartupDatList[0] != NULL) {
        NuDatSet(StartupDatList[1]);
    }
}
