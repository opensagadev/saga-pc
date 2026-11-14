#include "saga.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    extern char SaveFolderPathPc[256];
    extern char SaveFullSlotPathPc[256];
    extern char SaveSlotNamePc[256];
    extern i32 saveload_slotcode[6];
    void saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash);

    char *__cdecl fullslotname(i32 slot) {
#ifndef SAGA_DEBUG
        PCOptionsBuildFolder(SaveFolderPathPc);
        strcat(SaveFolderPathPc, "SavedGames");
#endif
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

    i32 fun_00543910(void) {
        return 0;
    }

    void __cdecl saveloadInit(VARIPTR *buffer, VARIPTR end, i32 size, const char *product, const char *icon,
                              const char *title, i32 flags) {
        saveload_getinfo();
        saveload_status = 1;
        saveload_autosave = -1;
    }

    i32 saveloadAutoSave(void *buffer, i32 size, u32 hash) {
        if (saveload_autosave == -1 || saveload_autosave >= 6) {
            return 0;
        }
        saveloadASSave(saveload_autosave, buffer, size, hash);
        return 1;
    }

    void fun_00543ed0(void) {
    }

    i32 __cdecl saveloadCheckSlotsUsed(void) {
        i32 used = 0;
        for (i32 slot = 0; slot < 6; ++slot) {
            char *name = fullslotname(slot);
            FILE *file = fopen(name, "rb");
            if (file != NULL) {
                ++used;
                fclose(file);
            }
        }
        return used;
    }

    i32 __cdecl saveloadCheckCardPresent(void) {
        return 1;
    }

    i32 __cdecl saveloadCheckCardType(void) {
        return 1;
    }

    i32 __cdecl saveloadCheckCardFormatted(void) {
        return 1;
    }

    i32 __cdecl saveloadCheckCardFreeSpace(i32 slot) {
        return 1;
    }

    i32 __cdecl saveloadFormatCard(void) {
        return 1;
    }

    i32 __cdecl saveloadUnFormatCard(void) {
        return 1;
    }

    i32 fun_00543980(void) {
        return 0;
    }

    i32 __cdecl saveloadCheckKeyCode(char *id, u8 *code) {
        return 0;
    }

    i32 __cdecl saveloadWriteKeyCode(char *id, u8 *code) {
        return 0;
    }
}
