#include "batman.h"
#include "nu2api/bgproc_pc.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/bgproc_platform.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore_pc_includes.h"
#include "legogame/pcbatman.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nuvideo.h"
#include <string.h>
#include "gamelib/util/CRC16.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nuerror_gen.h"
#include <stdarg.h>
#include <stdio.h>
#include "gameapi/apisave.h"
#include "gameapi/edtools/edui.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <windows.h>
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/numaths.h"
#include "legoapi/characters/motion.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nucore/nuerror.h"

extern "C" {
    /* PC-only state for the critical-section wrappers; their higher-level names
     * remain provisional until the original thread initialization path is mapped. */
    extern i32 PCThreadCriticalSectionCount;
}
typedef void (*NUERRORFUNCTION)(char *, ...);
typedef void (*NUDEBUGTTYFUNCTION)(i32 channel, char *format, ...);

extern "C" {
    volatile u32 nu_current_thread_id = 0;
    void (*NuThreadDisableThreadSwap)(void) = PcDisableThreadSwap;
    void (*NuThreadEnableThreadSwap)(void) = PcEnableThreadSwap;
    NUFILE_DEVICE host_device = {
        0,
        0,
        0x0e,
        0,
        '\\',
        0x400,
        -1,
        -1,
        -1,
        {0},
        {0},
        4,
        6,
        {0},
        "Host pc filesystem",
        {0},
        {0},
        {0},
        DEV_FormatName,
        DEVHOST_Interrogate,
    };
}
extern "C" {

    void NuErrorPrint(char *text) {
        PCNativeErrorPrint(&PCNativeWarningDebugState, "%s", text);
    }

    void NuWarningPrint(char *text) {
        PCNativeWarningPrint(&PCNativeWarningDebugState, "%s", text);
    }

    void NuDebugMsgPrint(char *text) {
        PCNativeDebugPrint(&PCNativeWarningDebugState, "%s", text);
    }

    /* The PC build has no online service: achievements report success and no
     * player is ever signed in. */
    i32 NuOnlineAchievementAchievedPS(i32 achievement, NUONLINEACHIEVEMENTCALLBACK callback) {
        return 1;
    }

    i32 NuOnlineAchievementAchievedExPS(i32 player, i32 achievement, NUONLINEACHIEVEMENTCALLBACK callback) {
        return 1;
    }

    void NuOnlineInitPS(void) {
    }

    i32 NuOnlineHasPlayerSignedInExPS(void) {
        return 0;
    }

    i32 NuOnlineHasPlayerSignedInPS(void) {
        return 0;
    }

    i32 NuOnlineHasPlayerDownloadedPS(u32 argument) {
        return 0;
    }

    void NuSetErrorMsgHandler(void (*handler)(char *)) {
        nuerror_handler = handler;
    }

    void NuSetWarningMsgHandler(void (*handler)(char *)) {
        nuwarning_handler = handler;
    }

    void (*PCWarningHandler)(char *) = 0;

    void NuSetDebugMsgHandler(void (*handler)(char *)) {
        PCWarningHandler = handler;
    }

    static void NuErrorFunction(char *format, ...) {
        va_list args;
        sprintf(PCWarningOutputBuffer, "NuError - %s(%d) : ", nufile, nuline);
        va_start(args, format);
        vsprintf(PCWarningFormatBuffer, format, args);
        va_end(args);
        NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
        NuStrCat(PCWarningOutputBuffer, "\n");
        if (nuerror_handler != NULL) {
            nuerror_handler(PCWarningOutputBuffer);
        }
        NuErrorPrint(PCWarningOutputBuffer);
    }

    NUERRORFUNCTION NuErrorProlog(char *file, i32 line) {
        nufile = file;
        nuline = line;
        return NuErrorFunction;
    }

    static void NuWarningFunction(char *format, ...) {
        va_list args;
        if (!DisableDebugMsg) {
            sprintf(PCWarningOutputBuffer, "NuWarning - %s(%d) : ", nufile, nuline);
            va_start(args, format);
            vsprintf(PCWarningFormatBuffer, format, args);
            va_end(args);
            NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
            NuStrCat(PCWarningOutputBuffer, "\n");
            if (nuwarning_handler != NULL) {
                nuwarning_handler(PCWarningOutputBuffer);
            }
            NuWarningPrint(PCWarningOutputBuffer);
        }
    }

    NUERRORFUNCTION NuWarningProlog(char *file, i32 line) {
        nufile = file;
        nuline = line;
        return NuWarningFunction;
    }

    static void NuDebugMsgFunction(char *format, ...) {
        char captxt[1024];
        va_list args;
        if (!DisableDebugMsg) {
            if (err_msg_filter[0] == 0 || strstr(nufile, err_msg_filter) != NULL) {
                sprintf(captxt, "NuDebugMsg - %s(%d) :", nufile, nuline);
                va_start(args, format);
                vsprintf(PCWarningFormatBuffer, format, args);
                va_end(args);
                NuStrCat(captxt, PCWarningFormatBuffer);
                NuStrCat(captxt, "\n");
                if (PCWarningHandler != NULL) {
                    PCWarningHandler(captxt);
                }
                PCNativeDebugPrint(&PCNativeWarningDebugState, "%s", captxt);
            }
        }
    }

    static void NuDebugMsgFunctionTTY(i32 channel, char *format, ...) {
    }

    NUDEBUGMSGFUNCTION NuDebugMsgProlog(char *file, i32 line) {
        nufile = file;
        nuline = line;
        return NuDebugMsgFunction;
    }

    NUDEBUGTTYFUNCTION NuDebugMsgPrologTTY(char *file, i32 line) {
        nufile = file;
        nuline = line;
        return NuDebugMsgFunctionTTY;
    }

    void NuErrorSetFilter(char *filter) {
        if (filter == NULL) {
            NuStrCpy(err_msg_filter, "");
        } else {
            NuStrNCpy(err_msg_filter, filter, sizeof(err_msg_filter));
        }
    }

    char *NuGetErrN(i32 entry) {
        i32 offset = 0;
        i32 current = 0;
        while (current < entry && offset < bHaveErr) {
            if (PCWarningMessages[offset++] == '\0') {
                ++current;
            }
        }
        return PCWarningMessages + offset;
    }

    char *NuErrorGetWarnings(void) {
        return bHaveErr ? PCWarningMessages : NULL;
    }

    void NuSevereWarning(const char *format, ...) {
        va_list args;
        char *message;
        sprintf(PCWarningOutputBuffer, " ");
        va_start(args, format);
        vsprintf(PCWarningFormatBuffer, format, args);
        va_end(args);
        NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
        NuStrCat(PCWarningOutputBuffer, "\n");
        if (PCWarningHandler != NULL) {
            PCWarningHandler(PCWarningOutputBuffer);
        }
        PCNativeDebugPrint(&PCNativeWarningDebugState, "%s", PCWarningOutputBuffer);
        message = PCWarningOutputBuffer;
        while (*message != '\0' && bHaveErr < 1023) {
            PCWarningMessages[bHaveErr++] = *message++;
        }
        PCWarningMessages[bHaveErr++] = '\0';
    }

    i32 NuHasError(void) {
        return bHaveErr;
    }

    void NuClearError(void) {
        bHaveErr = 0;
    }

    void NuVideoSetSwapModePS(i32 mode) {
    }

    void NuVideoSetBrightnessPS(void) {
    }

    NULSTHDR *bgproc_procinfo_pool = 0;

    BGPROCINFO *bgGetProcActive(void) {
        if (bgproc_active != NULL) {
            return bgproc_active;
        }
        return (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
    }

    BGPROCINFO *bgPostRequestV(bgprocdofn *do_fn, bgprocdofn *ack_fn, i32 first_type, ...) {
        BGPROCINFO *info;
        BGVAR *var;
        u8 *string_end;
        va_list args;
        i32 type;

        EnterCriticalSection(&bgproc_critical_section);
        info = (BGPROCINFO *)NuLstAllocTail(bgproc_procinfo_pool);
        if (info != NULL) {
            var = (BGVAR *)info->data;
            string_end = (u8 *)&info->vars;
            info->vars = var;
            info->work_started = 0;
            info->work_finished = 0;
            type = first_type;
            va_start(args, first_type);
            info->do_fn = do_fn;
            info->ack_fn = ack_fn;
            info->var_count = 0;
            while (type != 7) {
                var->type = type;
                switch (type) {
                    case 1:
                    case 5:
                        var->value.i32_value = va_arg(args, i32);
                        break;
                    case 2:
                        var->value.float_value = va_arg(args, f32);
                        break;
                    case 3: {
                        char *string = va_arg(args, char *);
                        string_end -= (u32)NuStrLen(string) + 1u;
                        var->value.string_value = (char *)string_end;
                        NuStrCpy((char *)string_end, string);
                        break;
                    }
                    case 4:
                        var->value.u32_value = va_arg(args, u32);
                        break;
                    case 6:
                        var->value.i32_value = va_arg(args, i32);
                        break;
                    default:
                        bgProcDataOversize(type);
                        break;
                }
                type = va_arg(args, i32);
                ++info->var_count;
                ++var;
            }
            va_end(args);
        }
        if (info == NULL || (u8 *)var > string_end) {
            ((void (*)(void))bgProcDataOversize)();
        }
        LeaveCriticalSection(&bgproc_critical_section);
        if (info != NULL) {
            SetEvent(bgproc_work_event);
        }
        return info;
    }

    BGPROCINFO *bgPostRequest(bgprocdofn *do_fn, bgprocdofn *ack_fn, void *data, i32 data_size) {
        BGPROCINFO *info;

        EnterCriticalSection(&bgproc_critical_section);
        info = (BGPROCINFO *)NuLstAllocTail(bgproc_procinfo_pool);
        if (info != NULL) {
            info->do_fn = do_fn;
            info->ack_fn = ack_fn;
            info->work_started = 0;
            info->work_finished = 0;
            info->vars = NULL;
            info->var_count = 0;
            if (data != NULL && data_size != 0) {
                if (data_size > 0x100) {
                    bgProcDataOversize(0x100);
                }
                memcpy(info->data, data, data_size);
            }
        }
        LeaveCriticalSection(&bgproc_critical_section);
        if (info != NULL) {
            SetEvent(bgproc_work_event);
        }
        return info;
    }

    /* Native Windows worker: the callback never returns during process lifetime. */
    static DWORD WINAPI bgProcWorker(void *unused) {
        WaitForSingleObject(bgproc_work_event, INFINITE);
        EnterCriticalSection(&bgproc_critical_section);
        bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
        LeaveCriticalSection(&bgproc_critical_section);
        for (;;) {
            if (WaitForSingleObject(bgproc_resume_event, 0) == WAIT_TIMEOUT) {
                bgproc_frozen = 1;
                WaitForSingleObject(bgproc_resume_event, INFINITE);
                bgproc_frozen = 0;
            }
            if (bgproc_active != NULL) {
                bgproc_active->work_started = 1;
                if (bgproc_active->do_fn != NULL) {
                    bgproc_active->do_fn(bgproc_active);
                }
                if (bgproc_active->ack_fn != NULL) {
                    bgproc_active->ack_fn(bgproc_active);
                }
            }
            EnterCriticalSection(&bgproc_critical_section);
            if (bgproc_active != NULL) {
                NuLstFree((NULNKHDR *)bgproc_active);
            }
            bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
            LeaveCriticalSection(&bgproc_critical_section);
            while (bgproc_active == NULL) {
                if (WaitForSingleObject(bgproc_resume_event, 0) == WAIT_TIMEOUT) {
                    bgproc_frozen = 1;
                    WaitForSingleObject(bgproc_resume_event, INFINITE);
                    bgproc_frozen = 0;
                }
                WaitForSingleObject(bgproc_work_event, INFINITE);
                EnterCriticalSection(&bgproc_critical_section);
                bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
                LeaveCriticalSection(&bgproc_critical_section);
            }
        }
    }

    void bgProcInit(void) {
        if (bgproc_initialized == 0) {
            bgproc_initialized = 1;
            bgproc_procinfo_pool = NuLstCreate(0x10, sizeof(BGPROCINFO));
            InitializeCriticalSection(&bgproc_critical_section);
            bgproc_work_event = CreateEventA(NULL, FALSE, FALSE, "BGWorkToDo");
            bgproc_resume_event = CreateEventA(NULL, TRUE, TRUE, "BGFreeze");
            bgproc_active = NULL;
            bgproc_thread_handle = CreateThread(NULL, 0x10000, bgProcWorker, NULL, CREATE_SUSPENDED, &bgproc_thread_id);
            SetThreadPriority(bgproc_thread_handle, -1);
            ResumeThread(bgproc_thread_handle);
        }
    }

    void bgProcCancelPending(void) {
        BGPROCINFO *info = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, (NULNKHDR *)bgproc_active);
        while (info != NULL) {
            BGPROCINFO *next = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, (NULNKHDR *)info);
            info->work_finished = 1;
            if (info->ack_fn != NULL) {
                info->ack_fn(info);
            }
            NuLstFree((NULNKHDR *)info);
            info = next;
        }
        if (bgproc_active != NULL) {
            bgproc_active->work_finished = 1;
        }
    }

    void bgprocFreeze(void) {
        bgproc_freeze = 1;
        ResetEvent(bgproc_resume_event);
        SetEvent(bgproc_work_event);
    }

    void bgprocUnFreeze(void) {
        bgproc_freeze = 0;
        SetEvent(bgproc_resume_event);
    }

    i32 bgprocIsFrozen(void) {
        return bgproc_frozen != 0;
    }

    i32 bgprocIsFreezing(void) {
        return bgproc_frozen != 0 || bgproc_freeze != 0;
    }

    i32 bgProcIsBgThread(void) {
        i32 result = 0;
        if (GetCurrentThreadId() == bgproc_thread_id) {
            result = 1;
        }
        return result;
    }

    void bgprocFrameSleep(i32 milliseconds) {
        if (bgproc_procinfo_pool != NULL) {
            if (bgproc_active != NULL || NuLstGetNext(bgproc_procinfo_pool, NULL) != NULL) {
                Sleep(milliseconds);
            }
        }
    }

    void fun_004ff0f0(void) {
    }

    void PcInitializeStartupCriticalSection(void) {
        if (PcStartupCriticalSectionInitialized == 0) {
            InitializeCriticalSection(&PcStartupCriticalSection);
            PcStartupCriticalSectionInitialized = 1;
        }
    }

    i32 NuFileExists(char *filepath) {
        return NuFileExistQuiet(filepath);
    }

    i32 DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, i32 buf_size) {
        char buffer[512];
        i32 device_name_len;
        char *subpath;

        device_name_len = NuStrLen(device->name);

        /* PC accepts an already-qualified path unchanged. */
        if (NuStrStr(path, ":") != NULL) {
            NuStrCpy(buffer, path);
        } else if (NuStrNICmp(path, device->name, device_name_len) == 0) {
            subpath = path + NuStrLen(device->name);
            if (*subpath == '\\' || *subpath == '/') {
                NuStrCpy(buffer, path);
            } else {
                NuStrCpy(buffer, device->root);
                NuStrCat(buffer, device->cur_dir);
                NuStrCat(buffer, subpath);
            }
        } else {
            NuStrCpy(buffer, device->root);
            NuStrCat(buffer, device->cur_dir);
            NuStrCat(buffer, path);
        }

        NuFileUpCase(device, buffer + NuStrLen(device->root));
        NuFileReldirFix(device, buffer);

        if (NuStrLen(buffer) < buf_size) {
            NuStrCpy(formatted_name, buffer);
            return 1;
        }

        return 0;
    }

    i32 DEVHOST_InterrogateAlt(NUFILE_DEVICE *device) {
        device->status = 1;
        return 1;
    }

    i32 DEVHOST_Interrogate(NUFILE_DEVICE *device) {
        device->status = 1;
        return 1;
    }

    i32 DEVHOST_InterrogateAlt2(NUFILE_DEVICE *device) {
        device->status = 1;
        return 1;
    }

    void fun_004ff2b0(void) {
    }

    i32 fun_004ff2c0(void) {
        return 0;
    }

    HANDLE pc_file_handles[32] = {
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
    };

    /* Original bugs: scans 64 entries of the 32-entry table, and NuPSFileOpen then stores
       into entry -1 when none is free. */
    static i32 NuPSFileFindFreeHandle(void) {
        i32 index;
        for (index = 0; index < 64; index++) {
            if (pc_file_handles[index] == INVALID_HANDLE_VALUE) {
                return index;
            }
        }
        return -1;
    }

    i32 NuPSFileOpen(const char *path, i32 mode) {
        DWORD access = 0;
        DWORD creation = 0;
        i32 index;

        switch (mode) {
            case NUFILE_READ:
                access = GENERIC_READ;
                creation = OPEN_EXISTING;
                break;
            case NUFILE_WRITE:
                access = GENERIC_WRITE;
                creation = CREATE_ALWAYS;
                break;
            case NUFILE_APPEND:
                access = GENERIC_WRITE;
                creation = OPEN_ALWAYS;
                break;
        }

        index = NuPSFileFindFreeHandle();
        pc_file_handles[index] = CreateFileA(path, access, FILE_SHARE_READ, NULL, creation, 0, NULL);
        if (pc_file_handles[index] == INVALID_HANDLE_VALUE) {
            return -1;
        }
        return index;
    }

    i32 NuPSFileClose(i32 index) {
        BOOL closed = CloseHandle(pc_file_handles[index]);
        pc_file_handles[index] = INVALID_HANDLE_VALUE;
        return closed == FALSE;
    }

    i32 NuPSFileRead(i32 index, void *destination, i32 size) {
        DWORD count = 0;
        ReadFile(pc_file_handles[index], destination, size, &count, NULL);
        return count;
    }

    i32 NuPSFileWrite(i32 index, const void *source, i32 size) {
        DWORD count = 0;
        if (WriteFile(pc_file_handles[index], source, size, &count, NULL)) {
            return count;
        }
        return 0;
    }

    i64 NuPSFileLSeek(i32 index, i64 offset, i32 origin) {
        DWORD method;
        LARGE_INTEGER distance;
        LARGE_INTEGER position;

        switch (origin) {
            case 0:
                method = FILE_BEGIN;
                break;
            case 1:
                method = FILE_CURRENT;
                break;
            default:
                method = FILE_END;
                break;
        }

        distance.QuadPart = offset;
        position.QuadPart = 0;
        if (SetFilePointerEx(pc_file_handles[index], distance, &position, method)) {
            return position.QuadPart;
        }
        return -1;
    }

    void NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject) {
    }

    char dat_0091f0c8[256] = {0};

    void fun_004ff4b0(const char *path) {
        strcpy(dat_0091f0c8, path);
    }

    void fun_004ff4d0(char *path) {
        strcpy(path, dat_0091f0c8);
    }

    i32 FileHostMovePath(char *destination, char *source) {
        char formatted_destination[256];
        char formatted_source[256];
        FileDeviceFormatDefaultName(formatted_source, source, 256);
        FileDeviceFormatDefaultName(formatted_destination, destination, 256);
        DeleteFileA(formatted_destination);
        return MoveFileA(formatted_source, formatted_destination);
    }

    char *FileDirectoryPathSlots[16] = {0};
    HANDLE FileDirectoryHostHandles[16] = {0};
    WIN32_FIND_DATAA FileDirectoryFindData[16] = {0};
    i32 FileDirectoryOpenCount = 0;

    i32 FileDirectoryHostOpen(char *path) {
        char search[200];
        i32 slot;
        if (FileDirectoryOpenCount == 16) {
            return 0;
        }
        for (slot = 0; slot < 16; ++slot) {
            if (FileDirectoryPathSlots[slot] == NULL) {
                break;
            }
        }
        if (slot == 16) {
            return 0;
        }
        strcpy(search, path);
        if (search[strlen(search) - 1] != '\\') {
            strcat(search, "\\");
        }
        strcat(search, "*.*");
        FileDirectoryPathSlots[slot] =
            (char *)NuAlloc(strlen(search) + 4, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x21a);
        strcpy(FileDirectoryPathSlots[slot], search);
        ++FileDirectoryOpenCount;
        FileDirectoryHostHandles[slot] = FindFirstFileA(FileDirectoryPathSlots[slot], &FileDirectoryFindData[slot]);
        if (FileDirectoryHostHandles[slot] == INVALID_HANDLE_VALUE) {
            NuFree(FileDirectoryPathSlots[slot], "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x222);
            --FileDirectoryOpenCount;
            /* Clears the slot before this one (slot 0 hits the word in front of the table). */
            FileDirectoryPathSlots[slot - 1] = NULL;
            return -1;
        }
        return slot;
    }

    i32 FileDirectoryHostClose(i32 slot) {
        HANDLE handle;
        NuFree(FileDirectoryPathSlots[slot], "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x22e);
        handle = FileDirectoryHostHandles[slot];
        /* Clears the slot before this one, as in FileDirectoryHostOpen. */
        FileDirectoryPathSlots[slot - 1] = NULL;
        FindClose(handle);
        --FileDirectoryOpenCount;
        return 0;
    }

    i32 FileDirectoryHostRead(i32 file, FILE_DIRECTORY_HOST_ENTRY *entry) {
        SYSTEMTIME time;
        u32 flags = 0x100;
        if ((FileDirectoryFindData[file].dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0) {
            flags = 0x180;
        }
        if ((FileDirectoryFindData[file].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            flags += 0x2000;
        }
        entry->flags = flags;
        entry->size = FileDirectoryFindData[file].nFileSizeLow;
        entry->unused_24 = 0;
        strcpy(entry->name, FileDirectoryFindData[file].cFileName);
        FileTimeToSystemTime(&FileDirectoryFindData[file].ftCreationTime, &time);
        entry->created.second = (u8)time.wSecond;
        entry->created.minute = (u8)time.wMinute;
        entry->created.hour = (u8)time.wHour;
        entry->created.day = (u8)time.wDay;
        entry->created.month = (u8)time.wMonth;
        entry->created.year_low = (u8)time.wYear;
        entry->created.year_high = 0;
        FileTimeToSystemTime(&FileDirectoryFindData[file].ftLastWriteTime, &time);
        entry->modified.second = (u8)time.wSecond;
        entry->modified.minute = (u8)time.wMinute;
        entry->modified.hour = (u8)time.wHour;
        entry->modified.day = (u8)time.wDay;
        entry->modified.month = (u8)time.wMonth;
        entry->modified.year_low = (u8)time.wYear;
        entry->modified.year_high = 0;
        FileTimeToSystemTime(&FileDirectoryFindData[file].ftLastAccessTime, &time);
        entry->accessed.second = (u8)time.wSecond;
        entry->accessed.minute = (u8)time.wMinute;
        entry->accessed.hour = (u8)time.wHour;
        entry->accessed.day = (u8)time.wDay;
        entry->accessed.month = (u8)time.wMonth;
        entry->accessed.year_low = (u8)time.wYear;
        entry->accessed.year_high = 0;
        if (FileDirectoryFindData[file].dwFileAttributes == 0xffffffff) {
            return 0;
        }
        if (FindNextFileA(FileDirectoryHostHandles[file], &FileDirectoryFindData[file]) != 1) {
            FileDirectoryFindData[file].dwFileAttributes = 0xffffffff;
        }
        return strlen(entry->name);
    }

    i32 SaveCreateDirectory(char *path) {
        SECURITY_ATTRIBUTES security;
        security.bInheritHandle = 1;
        security.nLength = sizeof(security);
        security.lpSecurityDescriptor = NULL;
        return CreateDirectoryA(path, &security);
    }

    i32 fun_004ff960(void) {
        return 0;
    }

    i32 NuFileGetInfo(char *path, AI_EDITOR_FILE_INFO *info) {
        WIN32_FILE_ATTRIBUTE_DATA attributes;
        SYSTEMTIME time;
        u32 flags;

        GetFileAttributesExA(path, GetFileExInfoStandard, &attributes);
        flags = (attributes.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) | 0x400;
        flags >>= 9;
        flags |= attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        flags >>= 1;
        flags |= ~(attributes.dwFileAttributes << 1) & 2;
        info->flags = flags;
        info->name[0] = 0;
        info->size = attributes.nFileSizeLow;
        FileTimeToSystemTime(&attributes.ftCreationTime, &time);
        info->created_year = (i16)time.wYear;
        info->created_month = (i8)time.wMonth;
        info->created_day = (i8)time.wDay;
        info->created_hour = (i8)time.wHour;
        info->created_minute = (i8)time.wMinute;
        info->created_second = (i8)time.wSecond;
        FileTimeToSystemTime(&attributes.ftLastWriteTime, &time);
        info->year = (i16)time.wYear;
        info->month = (i8)time.wMonth;
        info->day = (i8)time.wDay;
        info->hour = (i8)time.wHour;
        info->minute = (i8)time.wMinute;
        info->second = (i8)time.wSecond;
        return 1;
    }

    i32 FileHostDeletePath(const char *path) {
        return DeleteFileA(path);
    }

    CRITICAL_SECTION PCThreadCriticalSections[12] = {0};

    i32 PCThreadCriticalSectionCreate_candidate(void) {
        if (PCThreadCriticalSectionCount <= 12) {
            ++PCThreadCriticalSectionCount;
            InitializeCriticalSection(&PCThreadCriticalSections[PCThreadCriticalSectionCount - 1]);
            return PCThreadCriticalSectionCount;
        }

        return 0;
    }

    void PCThreadCriticalSectionDeleteNoOp(i32 index) {
    }

    // noinline: the original calls NuThreadCriticalSectionBegin out of line from
    // PCDisplayListCriticalSectionEnter; VC8 would inline it there.
    __declspec(noinline) void NuThreadCriticalSectionBegin(i32 section_index) {
        if (section_index >= 1 && section_index <= 12) {
            EnterCriticalSection(&PCThreadCriticalSections[section_index - 1]);
        }
    }

    // noinline: the original calls NuThreadCriticalSectionEnd out of line from
    // PCDisplayListCriticalSectionLeave; VC8 would inline it there.
    __declspec(noinline) void NuThreadCriticalSectionEnd(i32 section_index) {
        if (section_index >= 1 && section_index <= 12) {
            LeaveCriticalSection(&PCThreadCriticalSections[section_index - 1]);
        }
    }

    i64 PCThreadMilliseconds(void) {
        NuTime time;
        NuTimeGet(&time);
        return (((i64)time.high << 32) | time.low) / 1000;
    }

    i64 NuTime_TickBase = 0;

    void NuTimeGetTicksPS(u32 *low, i32 *high) {
        LARGE_INTEGER counter;
        i64 elapsed;
        QueryPerformanceCounter(&counter);
        if (counter.QuadPart > NuTime_TickBase) {
            elapsed = counter.QuadPart - NuTime_TickBase;
        } else {
            elapsed = (counter.QuadPart - NuTime_TickBase) + 0x7fffffffffffffffLL;
        }
        *low = (u32)elapsed;
        *high = (i32)(elapsed >> 32);
    }

    i64 NuTime_PerformanceFrequency = 0;

    void NuTimeGetTicksPerSecondPS(u32 *low, u32 *high) {
        *low = (u32)NuTime_PerformanceFrequency;
        *high = (u32)(NuTime_PerformanceFrequency >> 32);
    }

    void PcInitializePerformanceTiming(void) {
        DWORD_PTR process_mask = 1;
        DWORD_PTR system_mask = 1;
        if (!GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask)) {
            process_mask = 1;
        }
        SetProcessAffinityMask(GetCurrentProcess(), 1);
        QueryPerformanceCounter((LARGE_INTEGER *)&NuTime_TickBase);
        QueryPerformanceFrequency((LARGE_INTEGER *)&NuTime_PerformanceFrequency);
        SetProcessAffinityMask(GetCurrentProcess(), process_mask);
    }

    NUFILE_DEVICE *FileDirectoryDefaultDevice = &host_device;

    NUFILE NuFileOpenDir(char *path) {
        char formatted[256];
        NUFILE_DEVICE *device = NuFileGetDeviceFromPath(path);
        i32 result;
        if (device == NULL) {
            device = FileDirectoryDefaultDevice;
        }
        device->format_name_fn(device, formatted, path, 256);
        if (device->id == 2) {
            strcat(formatted, ";1");
        }
        if (device->id == 1) {
            i32 port = formatted[2] - '0';
            i32 slot = formatted[3] - '0';
            result = FileDirectoryMcOpen(port, slot, formatted + device->name_len);
            if (result >= 0) {
                return result + 0x1000;
            }
            return 0;
        }
        result = FileDirectoryHostOpen(formatted);
        if (result < 0) {
            return 0;
        }
        return result + 1;
    }

    void NuFileCloseDir(NUFILE file) {
        if (file >= 0x1000) {
            FileDirectoryMcClose(file - 0x1000);
            return;
        }
        FileDirectoryHostClose(file - 1);
    }

    i32 NuFileReadDir(NUFILE file, FILE_DIRECTORY_ENTRY *entry) {
        FILE_DIRECTORY_HOST_ENTRY host;
        i32 result;
        u32 flags;
        i32 length = 0;
        if (file >= 0x1000) {
            return FileDirectoryMcRead(file - 0x1000, entry);
        }
        result = FileDirectoryHostRead(file - 1, &host);
        if (result > 0) {
            flags = (((host.flags >> 2) & 0x800) | (host.flags & 0x100));
            flags = ((flags >> 2) | (host.flags & 0x80));
            entry->flags = (((flags >> 2) | (host.flags & 0x40)) >> 4);
            entry->size = host.size;
            entry->created.second = host.created.second;
            entry->created.minute = host.created.minute;
            entry->created.hour = host.created.hour;
            entry->created.day = host.created.day;
            entry->created.month = host.created.month;
            entry->created.year = (u16)(((u16)host.created.year_high << 8) + host.created.year_low);
            entry->modified.second = host.modified.second;
            entry->modified.minute = host.modified.minute;
            entry->modified.hour = host.modified.hour;
            entry->modified.day = host.modified.day;
            entry->modified.month = host.modified.month;
            entry->modified.year = (u16)(((u16)host.modified.year_high << 8) + host.modified.year_low);
            strcpy(entry->name, host.name);
            length = strlen(entry->name);
        } else if (result < 0) {
            FileDirectoryError(result);
        }
        return length;
    }
}
