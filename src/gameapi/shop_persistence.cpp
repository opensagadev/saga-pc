#include "saga.h"
#include <stdio.h>
#include <string.h>

/* Complete 0x2028-byte native header written before optional API data and Game. */
typedef struct PC_SAVE_HEADER_PC {
    u32 magic;                    // 0x0000
    u32 version;                  // 0x0004
    u32 size;                     // 0x0008
    u32 reserved_0c;              // 0x000c
    u32 reserved_10;              // 0x0010
    u32 api_data_size;            // 0x0014
    u8 metadata[16];              // 0x0018
    WCHAR title[1024];            // 0x0028
    WCHAR slot_caption[1024];     // 0x0828
    WCHAR reserved_caption[1024]; // 0x1028
    WCHAR date_time[1024];        // 0x1828
} PC_SAVE_HEADER_PC;              // size 0x2028

extern "C" {
    extern char SaveFolderPathPc[256];
    /* User-approved Debug save-location exception; the game requires the res cwd. */
#ifdef SAGA_DEBUG
    char SaveCreateFolderPathPc[256] = "SavedGames";
#else
    char SaveCreateFolderPathPc[256] = {0};
#endif
    u8 PcSaveHeaderMetadataPc[16] = {0};
    i32 saveload_slotid = 0;
    extern i32 SaveloadPendingPc;
    extern u32 savetimer;
    extern i32 saveload_slotused[6];
    extern char *__cdecl fullslotname(i32);
    extern char SaveSlotNamePc[256];
    extern const char SaveSlotCaptionFormatPc[13] = "Save Slot %d";

    void __cdecl PcRegisterSaveMetadataPc(const void *metadata) {
        if (metadata != NULL) {
            PCByteCopy(PcSaveHeaderMetadataPc, metadata, 16);
        }
    }

    i32 __cdecl SaveCreateDirectoryPc(char *path);

    void __cdecl createslotfolder(i32) {
#ifndef SAGA_DEBUG
        PCOptionsBuildFolder(SaveFolderPathPc);
        strcat(SaveFolderPathPc, "SavedGames");
        i32 i = 0;
        do {
            SaveCreateFolderPathPc[i] = SaveFolderPathPc[i];
        } while (SaveFolderPathPc[i++] != 0);
#endif
        SaveCreateDirectoryPc(SaveCreateFolderPathPc);
    }

    i32 __cdecl PCSaveSlot(i32 slot, void *buffer, i32 size, u32 hash) {
        PC_SAVE_HEADER_PC header;
        char date_time[1024];
        char time[64];
        if (saveload_slotused[slot] == 0) {
            createslotfolder(slot);
        }
        char *name = fullslotname(slot);
        FILE *file = fopen(name, "wb");
        memset(&header, 0, sizeof(header));
        header.magic = 0x52474d48;
        header.version = 1;
        header.size = sizeof(header);
        header.reserved_0c = 0;
        header.reserved_10 = 0;
        header.api_data_size = PcApiGetCallBufferCount();
        PCByteCopy(header.metadata, PcSaveHeaderMetadataPc, 16);
        header.reserved_caption[0] = 0;
        GetDateFormatA(LOCALE_USER_DEFAULT, 0, NULL, NULL, date_time, 64);
        strcat(date_time, " ");
        GetTimeFormatA(LOCALE_USER_DEFAULT, 0, NULL, NULL, time, 64);
        strcat(date_time, time);
        MultiByteToWideChar(0, 0, date_time, 1024, header.date_time, 1024);
        MultiByteToWideChar(0, 0, NuHwDefaultParameterPc, 1024, header.title, 1024);
        sprintf(date_time, SaveSlotCaptionFormatPc, slot);
        MultiByteToWideChar(0, 0, date_time, 1024, header.slot_caption, 1024);
        fwrite(&header, sizeof(header), 1, file);
        if (PcApiGetCallBuffer() != NULL) {
            fwrite(PcApiGetCallBuffer(), PcApiGetCallBufferCount(), 1, file);
        }
        fwrite(buffer, size, 1, file);
        fwrite(&hash, 4, 1, file);
        fclose(file);
        return 1;
    }

    i32 __cdecl saveloadSaveSlot(i32 slot, void *buffer, i32 size) {
        return PCSaveSlot(slot, buffer, size, (u32)-1);
    }

    i32 __cdecl saveloadLoadSlot(i32 slot, void *buffer, i32 size) {
        PC_SAVE_HEADER_PC header;
        char *name = fullslotname(slot);
        FILE *file = fopen(name, "rb");
        if (file != NULL) {
            fread(&header, sizeof(header), 1, file);
            if (header.api_data_size != 0) {
                fseek(file, header.api_data_size, SEEK_CUR);
            }
            fread(buffer, size, 1, file);
            fclose(file);
            return 1;
        }
        return 0;
    }

    i32 __cdecl saveloadDeleteSlot(i32 slot) {
        sprintf(SaveSlotNamePc, "SaveGame%d.%s_SavedGame", slot, PcProductName);
        remove(SaveSlotNamePc);
        return 1;
    }

    void __cdecl saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash) {
        SaveloadPendingPc = 1;
        savetimer = GetTickCount();
        saveload_status = 11;
        PCSaveSlot(slot, buffer, size, hash);
        saveload_autosave = slot;
        saveload_slotid = slot;
    }

    void __cdecl saveloadASLoad(i32 slot, void *buffer, i32 size) {
        saveload_status = 7;
        saveloadLoadSlot(slot, buffer, size);
        SaveloadPendingPc = 1;
        u32 tick = GetTickCount();
        saveload_slotid = slot;
        saveload_autosave = slot;
        savetimer = tick;
    }

    /* PC has no memory card to format. */
    void __cdecl saveloadASFormat(void) {
    }

    void __cdecl saveloadASDelete(i32 slot) {
        sprintf(SaveSlotNamePc, "SaveGame%d.%s_SavedGame", slot, PcProductName);
        remove(SaveSlotNamePc);
        saveload_slotid = slot;
        if (saveload_autosave == slot) {
            saveload_autosave = -1;
        }
    }
}
