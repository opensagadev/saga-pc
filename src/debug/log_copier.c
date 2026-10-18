/* Independent Debug stream copier; never linked into game executables. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>

typedef struct CopyStream {
    HANDLE input, terminal, file;
    DWORD bytes, failures;
    CRITICAL_SECTION *file_lock;
} CopyStream;

static BOOL write_all(HANDLE output, const char *bytes, DWORD length) {
    while (length != 0) {
        DWORD written;
        if (!WriteFile(output, bytes, length, &written, NULL) || written == 0)
            return FALSE;
        bytes += written;
        length -= written;
    }
    return TRUE;
}

static DWORD WINAPI copy_stream(void *argument) {
    CopyStream *stream = (CopyStream *)argument;
    char buffer[4096];
    DWORD count;
    for (;;) {
        if (!ReadFile(stream->input, buffer, sizeof(buffer), &count, NULL)) {
            if (GetLastError() != ERROR_BROKEN_PIPE)
                stream->failures |= 1;
            break;
        }
        if (count == 0)
            break;
        stream->bytes += count;
        EnterCriticalSection(stream->file_lock);
        if (!write_all(stream->file, buffer, count))
            stream->failures |= 2;
        LeaveCriticalSection(stream->file_lock);
        if (!write_all(stream->terminal, buffer, count))
            stream->failures |= 4;
    }
    CloseHandle(stream->input);
    return 0;
}

static HANDLE handle_argument(const char *text) {
    return (HANDLE)(ULONG_PTR)strtoul(text, NULL, 16);
}

int main(int argc, char **argv) {
    HANDLE threads[2], ready;
    CopyStream streams[2];
    CRITICAL_SECTION file_lock;

    int index;
    /* Invoked only with the parent's explicit inherited handle list. */
    if (argc != 8 || strcmp(argv[7], "saga-debug-capture-v1") != 0)
        return 10;
    InitializeCriticalSection(&file_lock);
    streams[0].input = handle_argument(argv[1]);
    streams[1].input = handle_argument(argv[2]);
    streams[0].terminal = handle_argument(argv[3]);
    streams[1].terminal = handle_argument(argv[4]);
    streams[0].file = streams[1].file = handle_argument(argv[5]);
    ready = handle_argument(argv[6]);
    if (GetFileType(streams[0].input) != FILE_TYPE_PIPE || GetFileType(streams[1].input) != FILE_TYPE_PIPE ||
        GetFileType(streams[0].file) != FILE_TYPE_DISK)
        return 12;
    for (index = 0; index != 2; ++index) {
        streams[index].bytes = streams[index].failures = 0;
        streams[index].file_lock = &file_lock;
        threads[index] = CreateThread(NULL, 0, copy_stream, &streams[index], 0, NULL);
        if (threads[index] == NULL)
            return 13;
    }
    if (!SetEvent(ready))
        return 14;
    CloseHandle(ready);
    /* No parent-death timer: drain independently until every real writer closes. */
    if (WaitForMultipleObjects(2, threads, TRUE, INFINITE) != WAIT_OBJECT_0)
        return 15;
    FlushFileBuffers(streams[0].file);
    CloseHandle(streams[0].file);
    CloseHandle(streams[0].terminal);
    CloseHandle(streams[1].terminal);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    DeleteCriticalSection(&file_lock);
    return streams[0].failures || streams[1].failures;
}
