#include "saga.h"

extern NUDATHDR *StartupDatList[];
extern i32 StartupDatCount;
extern void __cdecl NuSound3SetDat(NUDATHDR *header);

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
