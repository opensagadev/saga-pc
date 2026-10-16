#include "saga.h"
#include <stdio.h>
#include <string.h>

/* Complete 0x2028-byte native header written before optional API data and Game. */
typedef struct PC_SAVE_HEADER_PC {
    u32 magic;
    u32 version;
    u32 size;
    u32 reserved_0c;
    u32 reserved_10;
    u32 api_data_size;
    u8 metadata[16];
    WCHAR title[1024];
    WCHAR slot_caption[1024];
    WCHAR reserved_caption[1024];
    WCHAR date_time[1024];
} PC_SAVE_HEADER_PC;
STATIC_ASSERT(sizeof(PC_SAVE_HEADER_PC) == 0x2028);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, api_data_size) == 0x14);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, metadata) == 0x18);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, title) == 0x28);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, slot_caption) == 0x828);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, reserved_caption) == 0x1028);
STATIC_ASSERT(offsetof(PC_SAVE_HEADER_PC, date_time) == 0x1828);

extern "C" {
    extern char SaveFolderPathPc[256];
    /* User-approved Debug save-location exception; the game requires the res cwd. */
#ifdef SAGA_DEBUG
    char SaveCreateFolderPathPc[256] = "SavedGames";
#else
    char SaveCreateFolderPathPc[256] = {0};
#endif
    u8 PcSaveHeaderMetadataPc[16];
    i32 saveload_slotid;
    extern i32 SaveloadPendingPc;
    extern u32 SaveloadStartedTickPc;
    extern i32 saveload_slotused[6];
    extern char *__cdecl fullslotname(i32);
    extern const char SaveSlotCaptionFormatPc[13] = "Save Slot %d";

    void __cdecl PcRegisterSaveMetadataPc(const void *metadata) {
        if (metadata != NULL)
            PCByteCopy_candidate(PcSaveHeaderMetadataPc, metadata, 16);
    }
    i32 __cdecl PcApiGetCallBufferCount(void) {
        return PcApiCallBufferCount;
    }
    void *__cdecl PcApiGetCallBuffer(void) {
        return PcApiCallBuffer;
    }
    i32 __cdecl SaveCreateDirectoryPc(char *path) {
        SECURITY_ATTRIBUTES security;
        security.nLength = sizeof(security);
        security.lpSecurityDescriptor = NULL;
        security.bInheritHandle = 1;
        return CreateDirectoryA(path, &security);
    }
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
        if (saveload_slotused[slot] == 0)
            createslotfolder(slot);
        FILE *file = fopen(fullslotname(slot), "wb");
        memset(&header, 0, sizeof(header));
        header.magic = 0x52474d48;
        header.version = 1;
        header.size = sizeof(header);
        header.reserved_0c = 0;
        header.reserved_10 = 0;
        header.api_data_size = PcApiGetCallBufferCount();
        PCByteCopy_candidate(header.metadata, PcSaveHeaderMetadataPc, 16);
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
        if (PcApiGetCallBuffer() != NULL)
            fwrite(PcApiGetCallBuffer(), PcApiGetCallBufferCount(), 1, file);
        fwrite(buffer, size, 1, file);
        fwrite(&hash, 4, 1, file);
        fclose(file);
        return 1;
    }
    void __cdecl saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash) {
        SaveloadPendingPc = 1;
        SaveloadStartedTickPc = GetTickCount();
        saveload_status = 11;
        PCSaveSlot(slot, buffer, size, hash);
        saveload_autosave = slot;
        saveload_slotid = slot;
    }
}
