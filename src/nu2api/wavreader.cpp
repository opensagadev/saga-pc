#include "saga.h"
#include <string.h>
struct PC_WAVE_CHUNK_PC {
    u32 kind;
    u32 bytes;
    i32 offset;
};
#pragma pack(push, 4)
class WavReader {
  public:
    WavReader(void);
    virtual ~WavReader(void);
    virtual HRESULT Open(NUDATHDR *, char *, WAVEFORMATEX *);
    virtual HRESULT Unknown08Pc(u32, u32);
    virtual HRESULT Close(void);
    virtual WAVEFORMATEX *GetFormat(void);
    virtual i32 GetSize(void);
    virtual HRESULT Reset(void);
    virtual HRESULT Read(void *, u32, u32 *);
    virtual i32 SeekTime(f32);
    u32 FourCcPc(const char *);
    i32 FindChunkPc(HANDLE, const char *, PC_WAVE_CHUNK_PC *);
    i32 SkipChunkPc(HANDLE, const PC_WAVE_CHUNK_PC *);
    u32 unknown_004;
    HANDLE file;
    u32 unknown_00c;
    i64 base_position;
    WAVEFORMATEX format;
    u8 unknown_02a[2];
    i32 first_sample;
    i32 end_sample;
    i32 current_sample;
    i32 data_offset;
    u32 data_bytes;
};
#pragma pack(pop)
STATIC_ASSERT(sizeof(WavReader) == 0x40);
STATIC_ASSERT(offsetof(WavReader, file) == 8);
STATIC_ASSERT(offsetof(WavReader, base_position) == 0x10);
STATIC_ASSERT(offsetof(WavReader, format) == 0x18);
STATIC_ASSERT(offsetof(WavReader, first_sample) == 0x2c);
STATIC_ASSERT(offsetof(WavReader, end_sample) == 0x30);
STATIC_ASSERT(offsetof(WavReader, current_sample) == 0x34);
STATIC_ASSERT(offsetof(WavReader, data_offset) == 0x38);
STATIC_ASSERT(offsetof(WavReader, data_bytes) == 0x3c);
STATIC_ASSERT(sizeof(PC_WAVE_CHUNK_PC) == 12);
STATIC_ASSERT(offsetof(PC_WAVE_CHUNK_PC, kind) == 0);
STATIC_ASSERT(offsetof(PC_WAVE_CHUNK_PC, bytes) == 4);
STATIC_ASSERT(offsetof(PC_WAVE_CHUNK_PC, offset) == 8);
extern "C" void __cdecl PCDecoderDiagnosticNoOpPc(...);

WavReader::WavReader(void) {
    file = NULL;
}
WavReader::~WavReader(void) {
    if (file != NULL) {
        CloseHandle(file);
        file = NULL;
    }
}
HRESULT WavReader::Unknown08Pc(u32 unused0, u32 unused1) {
    (void)unused0;
    (void)unused1;
    return E_FAIL;
}
WAVEFORMATEX *WavReader::GetFormat(void) {
    return &format;
}
i32 WavReader::GetSize(void) {
    if (file != NULL)
        return (end_sample - first_sample) * format.nBlockAlign;
    return 0;
}
HRESULT WavReader::Reset(void) {
    if (file == NULL)
        return E_FAIL;
    current_sample = first_sample;
    return S_OK;
}
HRESULT WavReader::Close(void) {
    if (file == NULL)
        return E_FAIL;
    CloseHandle(file);
    file = NULL;
    return S_OK;
}
i32 WavReader::SeekTime(f32 time) {
    i32 sample = (i32)((f32)(format.nAvgBytesPerSec / format.nBlockAlign) * time);
    current_sample = sample;
    i32 limit = end_sample < sample ? end_sample : sample;
    if (first_sample > limit)
        current_sample = first_sample;
    else
        current_sample = end_sample < sample ? end_sample : sample;
    return current_sample * format.nBlockAlign;
}
u32 WavReader::FourCcPc(const char *text) {
    return ((u32)(i32)text[3] << 24) | ((u32)(i32)text[2] << 16) | ((u32)(i32)text[1] << 8) | (u32)(i32)text[0];
}
i32 WavReader::FindChunkPc(HANDLE input, const char *name, PC_WAVE_CHUNK_PC *chunk) {
    u32 wanted = FourCcPc(name);
    DWORD bytes;
    LARGE_INTEGER move, position;
    while (ReadFile(input, chunk, 8, &bytes, NULL) && bytes == 8) {
        move.QuadPart = 0;
        if (chunk->kind == wanted) {
            SetFilePointerEx(input, move, &position, FILE_CURRENT);
            chunk->offset = (i32)(position.LowPart - (u32)base_position);
            return 1;
        }
        move.QuadPart = chunk->bytes;
        if (SetFilePointerEx(input, move, &position, FILE_CURRENT) == (BOOL)-1)
            return 0;
    }
    return 0;
}
i32 WavReader::SkipChunkPc(HANDLE input, const PC_WAVE_CHUNK_PC *chunk) {
    LARGE_INTEGER move, position;
    move.QuadPart = base_position + (i64)(u32)chunk->offset + chunk->bytes;
    return SetFilePointerEx(input, move, &position, FILE_BEGIN) != (BOOL)-1;
}
HRESULT WavReader::Open(NUDATHDR *header, char *path, WAVEFORMATEX *output) {
    file = NULL;
    base_position = 0;
    i32 archive_size;
    LARGE_INTEGER move, position;
    if (header != NULL) {
        if (NuDatGetFileInfo(header, path, &base_position, &archive_size)) {
            file = CreateFileA(header->name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               NULL);
            if (file != INVALID_HANDLE_VALUE) {
                move.QuadPart = base_position;
                SetFilePointerEx(file, move, &position, FILE_BEGIN);
            } else
                PCDecoderDiagnosticNoOpPc(header->name);
        }
    } else {
        PCDecoderDiagnosticNoOpPc(path);
        file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0x08000001, NULL);
    }
    if (file == INVALID_HANDLE_VALUE) {
        PCDecoderDiagnosticNoOpPc(path);
        return E_FAIL;
    }
    struct {
        u32 riff;
        u32 bytes;
        u32 wave;
    } riff;
    DWORD bytes;
    PC_WAVE_CHUNK_PC chunk;
    if (!ReadFile(file, &riff, 12, &bytes, NULL) || bytes != 12 || riff.riff != 0x46464952 || riff.wave != 0x45564157)
        goto fail;
    if (!FindChunkPc(file, "fmt ", &chunk) || chunk.bytes < 16)
        goto fail;
    memset(&format, 0, sizeof(format));
    if (!ReadFile(file, &format, 16, &bytes, NULL) || bytes != 16)
        goto fail;
    format.cbSize = 0;
    if (output != NULL)
        *output = format;
    if (format.wFormatTag != 1)
        goto fail;
    SkipChunkPc(file, &chunk);
    if (!FindChunkPc(file, "data", &chunk))
        goto fail;
    data_offset = chunk.offset;
    data_bytes = chunk.bytes;
    if (data_bytes == 0)
        goto fail;
    first_sample = current_sample = 0;
    end_sample = (i32)data_bytes / format.nBlockAlign - 1;
    return S_OK;
fail:
    CloseHandle(file);
    file = NULL;
    return E_FAIL;
}
HRESULT WavReader::Read(void *output, u32 requested, u32 *read_count) {
    if (file != NULL) {
        u32 frames = requested / format.nBlockAlign;
        i32 remaining = end_sample - current_sample;
        i32 count = (i32)frames;
        if (count >= remaining)
            count = remaining;
        i32 amount = count * format.nBlockAlign;
        i32 offset = current_sample * format.nBlockAlign;
        if ((u32)amount != requested) {
            PCDecoderDiagnosticNoOpPc();
            PCDecoderDiagnosticNoOpPc(requested, amount);
        }
        LARGE_INTEGER move, position;
        move.QuadPart = base_position + (i64)offset + data_offset;
        DWORD bytes;
        if (SetFilePointerEx(file, move, &position, FILE_BEGIN)) {
            if (ReadFile(file, output, amount, &bytes, NULL) && bytes == (u32)amount) {
                if (read_count != NULL)
                    *read_count = bytes;
                current_sample += count;
                return S_OK;
            }
            PCDecoderDiagnosticNoOpPc();
            PCDecoderDiagnosticNoOpPc(amount);
            PCDecoderDiagnosticNoOpPc(bytes);
            PCDecoderDiagnosticNoOpPc(requested);
        } else {
            PCDecoderDiagnosticNoOpPc();
            PCDecoderDiagnosticNoOpPc(offset);
            PCDecoderDiagnosticNoOpPc(data_offset);
            PCDecoderDiagnosticNoOpPc(GetSize());
        }
    }
    if (read_count != NULL)
        *read_count = 0;
    return E_FAIL;
}
