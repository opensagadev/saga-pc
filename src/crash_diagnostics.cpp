/* Debug-only first-chance evidence; never handles or recovers from an exception.
 * Stack overflow is excluded: this observer cannot guarantee available handler stack. */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // NOLINT(readability-identifier-naming): SDK-required Windows XP API selector.
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

struct FAULT_MODULE_PC {
    DWORD base, size;
    char name[64];
};
static volatile LONG fault_active = 0;
static volatile LONG fault_initialized = 0;
static HANDLE fault_file = INVALID_HANDLE_VALUE;
static HANDLE fault_latest = INVALID_HANDLE_VALUE;
static unsigned fault_count = 0;
static char fault_build[96] = {0};
static char fault_image[MAX_PATH] = {0};
static char fault_path[MAX_PATH] = {0};
static FAULT_MODULE_PC fault_modules[64] = {0};
static unsigned fault_module_count = 0;
static char fault_text[4096] = {0};
static unsigned fault_used = 0;
static EXCEPTION_POINTERS fault_pointers = {0};
static EXCEPTION_RECORD fault_record = {0};
static CONTEXT fault_context = {0};
static DWORD fault_stack[16] = {0};
static unsigned char fault_code[16] = {0};

static void fault_copy(char *to, unsigned capacity, const char *from) {
    unsigned i = 0;
    if (from != NULL) {
        while (i + 1 < capacity && from[i] != 0) {
            to[i] = from[i];
            ++i;
        }
    }
    to[i] = 0;
}
static void fault_append(const char *text) {
    while (*text != 0 && fault_used + 1 < sizeof(fault_text))
        fault_text[fault_used++] = *text++;
    fault_text[fault_used] = 0;
}
static void fault_hex(DWORD value) {
    static const char digits[] = "0123456789ABCDEF";
    int shift;
    fault_append("0x");
    for (shift = 28; shift >= 0 && fault_used + 1 < sizeof(fault_text); shift -= 4)
        fault_text[fault_used++] = digits[(value >> shift) & 15];
    fault_text[fault_used] = 0;
}
static void fault_value(const char *label, DWORD value) {
    fault_append(label);
    fault_hex(value);
}
static BOOL fault_read(const void *address, void *target, SIZE_T size) {
    SIZE_T copied = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, target, size, &copied) && copied == size;
}
static BOOL fault_selected(DWORD code) {
    return code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR ||
           code == EXCEPTION_ILLEGAL_INSTRUCTION || code == EXCEPTION_PRIV_INSTRUCTION ||
           code == EXCEPTION_INT_DIVIDE_BY_ZERO || code == EXCEPTION_INT_OVERFLOW ||
           code == EXCEPTION_FLT_DIVIDE_BY_ZERO || code == EXCEPTION_FLT_INVALID_OPERATION;
}
static LONG CALLBACK fault_observe(EXCEPTION_POINTERS *pointers) {
    DWORD saved_error = GetLastError();
    DWORD written;
    unsigned i;
    BOOL have_context;
    MEMORY_BASIC_INFORMATION memory;
    /* No CRT calls here: in particular, accessing errno can allocate CRT thread state. */
    if (InterlockedCompareExchange(&fault_active, 1, 0) != 0) {
        SetLastError(saved_error);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (!fault_read(pointers, &fault_pointers, sizeof(fault_pointers)) ||
        !fault_read(fault_pointers.ExceptionRecord, &fault_record, sizeof(fault_record)) ||
        !fault_selected(fault_record.ExceptionCode))
        goto finished;
    have_context = fault_read(fault_pointers.ContextRecord, &fault_context, sizeof(fault_context));
    if (fault_count != 0xffffffffu)
        ++fault_count;
    fault_used = 0;
    fault_append("\r\n[FAULT-DIAGNOSTIC FIRST-CHANCE handled-status=unknown] build=");
    fault_append(fault_build);
    fault_value(" event=", fault_count);
    fault_value(" pid=", GetCurrentProcessId());
    fault_value(" tid=", GetCurrentThreadId());
    fault_append("\r\nimage=");
    fault_append(fault_image);
    fault_value("\r\nexception=", fault_record.ExceptionCode);
    fault_value(" address=", (DWORD)fault_record.ExceptionAddress);
    fault_value(" flags=", fault_record.ExceptionFlags);
    for (i = 0; i < fault_record.NumberParameters && i < 3; ++i)
        fault_value(" info=", (DWORD)fault_record.ExceptionInformation[i]);
    fault_append("\r\nmodule-at-init=");
    for (i = 0; i < fault_module_count; ++i) {
        DWORD ip = (DWORD)fault_record.ExceptionAddress;
        if (ip >= fault_modules[i].base && ip - fault_modules[i].base < fault_modules[i].size) {
            fault_append(fault_modules[i].name);
            fault_value(" base=", fault_modules[i].base);
            fault_value(" rva=", ip - fault_modules[i].base);
            break;
        }
    }
    if (i == fault_module_count)
        fault_append("unknown");
    if (VirtualQuery(fault_record.ExceptionAddress, &memory, sizeof(memory)) == sizeof(memory)) {
        fault_value(" allocation=", (DWORD)memory.AllocationBase);
        fault_value(" protect=", memory.Protect);
        fault_value(" type=", memory.Type);
    }
    if (have_context) {
        fault_value("\r\nEIP=", fault_context.Eip);
        fault_value(" ESP=", fault_context.Esp);
        fault_value(" EBP=", fault_context.Ebp);
        fault_value(" EFLAGS=", fault_context.EFlags);
        fault_value("\r\nEAX=", fault_context.Eax);
        fault_value(" EBX=", fault_context.Ebx);
        fault_value(" ECX=", fault_context.Ecx);
        fault_value(" EDX=", fault_context.Edx);
        fault_value(" ESI=", fault_context.Esi);
        fault_value(" EDI=", fault_context.Edi);
        fault_append("\r\nstack-words(no-unwind):");
        for (i = 0; i < 16; ++i) {
            DWORD address = fault_context.Esp + i * sizeof(DWORD);
            if (address < fault_context.Esp || !fault_read((void *)address, &fault_stack[i], sizeof(DWORD))) {
                fault_append(" unreadable");
                break;
            }
            fault_append(" ");
            fault_hex(fault_stack[i]);
        }
        fault_append("\r\ninstruction-bytes:");
        if (fault_read((void *)fault_context.Eip, fault_code, sizeof(fault_code))) {
            static const char digits[] = "0123456789ABCDEF";
            for (i = 0; i < sizeof(fault_code); ++i) {
                char pair[4];
                pair[0] = ' ';
                pair[1] = digits[fault_code[i] >> 4];
                pair[2] = digits[fault_code[i] & 15];
                pair[3] = 0;
                fault_append(pair);
            }
        } else {
            fault_append(" unreadable");
        }
    } else {
        fault_append("\r\ncontext unreadable");
    }
    fault_append("\r\nlatest-selected-fault-sidecar=");
    fault_append(fault_latest == INVALID_HANDLE_VALUE ? "unavailable" : fault_path);
    fault_append("\r\nThis is first-chance evidence, not proof the exception was unhandled.\r\n");
    if (fault_count == 8) {
        if (fault_latest != INVALID_HANDLE_VALUE)
            fault_append("Normal-log capture limit reached; bounded latest-selected-fault sidecar remains active.\r\n");
        else
            fault_append("Normal-log capture limit reached; latest-selected-fault sidecar is unavailable.\r\n");
    }
    if (fault_file != INVALID_HANDLE_VALUE && fault_count <= 8)
        WriteFile(fault_file, fault_text, fault_used, &written, NULL);
    if (fault_latest != INVALID_HANDLE_VALUE) {
        SetLastError(NO_ERROR);
        if (SetFilePointer(fault_latest, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER ||
            GetLastError() == NO_ERROR) {
            if (WriteFile(fault_latest, fault_text, fault_used, &written, NULL) && written == fault_used)
                SetEndOfFile(fault_latest);
        }
    }
finished:
    InterlockedExchange(&fault_active, 0);
    SetLastError(saved_error);
    return EXCEPTION_CONTINUE_SEARCH;
}

extern "C" void __cdecl SagaDebugCrashInitPc(HANDLE existing_log_file, const char *build_id) {
    DWORD saved_error = GetLastError();
    HANDLE modules;
    MODULEENTRY32 entry;
    unsigned length, slash, attempt;
    char suffix[64];
    unsigned suffix_used;
    DWORD tokens[3];
    unsigned token, digit;
    static const char hex[] = "0123456789ABCDEF";
    static const char prefix[] = "log\\latest-fault-";
    if (InterlockedCompareExchange(&fault_initialized, 1, 0) != 0) {
        SetLastError(saved_error);
        return;
    }
    fault_copy(fault_build, sizeof(fault_build), build_id);
    GetModuleFileNameA(NULL, fault_image, sizeof(fault_image));
    fault_image[sizeof(fault_image) - 1] = 0;
    if (existing_log_file != NULL && existing_log_file != INVALID_HANDLE_VALUE)
        DuplicateHandle(GetCurrentProcess(), existing_log_file, GetCurrentProcess(), &fault_file, 0, FALSE,
                        DUPLICATE_SAME_ACCESS);
    modules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (modules != INVALID_HANDLE_VALUE) {
        entry.dwSize = sizeof(entry);
        if (Module32First(modules, &entry)) {
            do {
                FAULT_MODULE_PC *module = &fault_modules[fault_module_count++];
                module->base = (DWORD)entry.modBaseAddr;
                module->size = entry.modBaseSize;
                fault_copy(module->name, sizeof(module->name), entry.szModule);
            } while (fault_module_count < 64 && Module32Next(modules, &entry));
        }
        CloseHandle(modules);
    }
    length = 0;
    slash = 0;
    while (fault_image[length] != 0) {
        if (fault_image[length] == '\\' || fault_image[length] == '/')
            slash = length + 1;
        ++length;
    }
    if (slash != 0 && slash + 64 < sizeof(fault_path)) {
        for (attempt = 0; attempt < 16; ++attempt) {
            for (length = 0; length < slash; ++length)
                fault_path[length] = fault_image[length];
            fault_copy(fault_path + slash, sizeof(fault_path) - slash, prefix);
            length = slash + sizeof(prefix) - 1;
            tokens[0] = GetCurrentProcessId();
            tokens[1] = GetTickCount();
            tokens[2] = attempt;
            suffix_used = 0;
            for (token = 0; token < 3; ++token) {
                for (digit = 0; digit < 8; ++digit)
                    suffix[suffix_used++] = hex[(tokens[token] >> (28 - digit * 4)) & 15];
                suffix[suffix_used++] = '-';
            }
            suffix[suffix_used - 1] = '.';
            suffix[suffix_used++] = 'l';
            suffix[suffix_used++] = 'o';
            suffix[suffix_used++] = 'g';
            suffix[suffix_used] = 0;
            fault_copy(fault_path + length, sizeof(fault_path) - length, suffix);
            fault_latest = CreateFileA(fault_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW,
                                       FILE_ATTRIBUTE_NORMAL, NULL);
            if (fault_latest != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS)
                break;
        }
    }
    AddVectoredExceptionHandler(1, fault_observe);
    SetLastError(saved_error);
}
