#include "saga.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    extern char SaveFolderPathPc[256];
    extern char SaveFullSlotPathPc[256];
    extern char SaveSlotNamePc[256];
    extern i32 saveload_slotcode[6];
    char *__cdecl fullslotname(i32 slot) {
        PCOptionsBuildFolder(SaveFolderPathPc);
        strcat(SaveFolderPathPc, "SavedGames");
        i32 i = 0;
        do {
            SaveFullSlotPathPc[i] = SaveFolderPathPc[i];
        } while (SaveFolderPathPc[i++] != 0);
        strcat(SaveFullSlotPathPc, "\\");
        sprintf(SaveSlotNamePc, "SaveGame%d.%s_SavedGame", slot, PcProductName);
        strcat(SaveFullSlotPathPc, SaveSlotNamePc);
        return SaveFullSlotPathPc;
    }
    i32 __cdecl saveload_getinfo(void) {
        saveload_savepresent = 0;
        i32 count = 0;
        for (i32 slot = 0; slot < 6; ++slot) {
            saveload_slotused[slot] = 0;
            saveload_slotcode[slot] = 0;
            char *path = fullslotname(slot);
            FILE *file = fopen(path, "rb");
            if (file != NULL) {
                ++count;
                saveload_slotused[slot] = 1;
                fseek(file, -4, SEEK_END);
                fread(&saveload_slotcode[slot], 4, 1, file);
                saveload_savepresent = 1;
                fclose(file);
            }
        }
        return count;
    }
    void __cdecl saveloadInit(VARIPTR *buffer, VARIPTR end, i32 size, const char *product, const char *icon,
                              const char *title, i32 flags) {
        saveload_getinfo();
        saveload_status = 1;
        saveload_autosave = -1;
    }
}
