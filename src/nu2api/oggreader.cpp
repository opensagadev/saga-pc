#include "saga.h"
#include <string.h>
#include "../audio/vorbis/vorbis/vorbisfile.h"
struct PC_OGG_CACHE_RECORD_PC {
    char path[0x104];
    u8 *data;
    i32 bytes;
    u32 last_used;
    i32 references;
};
STATIC_ASSERT(sizeof(PC_OGG_CACHE_RECORD_PC) == 0x114);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, data) == 0x104);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, bytes) == 0x108);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, last_used) == 0x10c);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, references) == 0x110);
#pragma pack(push, 4)
class OggReader : public WavReader {
  public:
    OggReader(void);
    virtual ~OggReader(void);
    virtual HRESULT Open(NUDATHDR *, char *, WAVEFORMATEX *);
    virtual HRESULT Unknown08Pc(u32, u32);
    virtual HRESULT Close(void);
    virtual WAVEFORMATEX *GetFormat(void);
    virtual i32 GetSize(void);
    virtual HRESULT Reset(void);
    virtual HRESULT Read(void *, u32, u32 *);
    virtual i32 SeekTime(f32);
    static u32 __cdecl ReadCallbackPc(void *, u32, u32, void *);
    static i32 __cdecl SeekCallbackPc(void *, i64, i32);
    static i32 __cdecl CloseCallbackPc(void *);
    // NOLINTNEXTLINE(google-runtime-int): Original upstream long type preserves the native C ABI.
    static long __cdecl TellCallbackPc(void *);
    u8 compressed;
    u8 unknown_041[7];
    OggVorbis_File vorbis_state;
    PC_OGG_CACHE_RECORD_PC *cache;
    i32 position;
    WAVEFORMATEX compressed_format;
    u8 unknown_332[6];
};
#pragma pack(pop)
STATIC_ASSERT(sizeof(OggReader) == 0x338);
STATIC_ASSERT(offsetof(OggReader, compressed) == 0x40);
STATIC_ASSERT(offsetof(OggReader, vorbis_state) == 0x48);
STATIC_ASSERT(offsetof(OggReader, cache) == 0x318);
STATIC_ASSERT(offsetof(OggReader, position) == 0x31c);
STATIC_ASSERT(offsetof(OggReader, compressed_format) == 0x320);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, path) == 0);
STATIC_ASSERT(sizeof(OggVorbis_File) == 0x2d0);
STATIC_ASSERT(offsetof(OggVorbis_File, callbacks) == 0x2c0);

OggReader::OggReader(void) {
    compressed = 0;
    cache = NULL;
    position = 0;
    if (PCOggCriticalSectionPc == 0) {
        PCOggCriticalSectionPc = PCThreadCriticalSectionCreate_candidate();
        PCOggCachePoolPc = (PC_OGG_CACHE_RECORD_PC *)NuAlloc(0x564, ".\\pc\\oggreader.cpp", 36);
    }
    for (i32 i = 0; i < 5; ++i)
        memset(&PCOggCachePoolPc[i], 0, sizeof(PC_OGG_CACHE_RECORD_PC));
}
OggReader::~OggReader(void) {
    if (!compressed)
        WavReader::Close();
    else {
        if (PCOggCriticalSectionPc != 0) {
            PCThreadCriticalSectionDeleteNoOpPc(PCOggCriticalSectionPc);
            PCOggCriticalSectionPc = 0;
        }
        Close();
    }
}
HRESULT OggReader::Unknown08Pc(u32 a, u32 b) {
    (void)a;
    (void)b;
    return E_FAIL;
}
WAVEFORMATEX *OggReader::GetFormat(void) {
    return compressed ? &compressed_format : &format;
}
i32 OggReader::GetSize(void) {
    if (!compressed)
        return WavReader::GetSize();
    if (cache == NULL)
        return 0;
    return (i32)ov_pcm_total(&vorbis_state, -1) * compressed_format.nBlockAlign;
}
HRESULT OggReader::Reset(void) {
    if (!compressed)
        return WavReader::Reset();
    if (cache == NULL)
        return E_FAIL;
    return ov_raw_seek(&vorbis_state, 0) >= 0 ? S_OK : E_FAIL;
}
HRESULT OggReader::Read(void *output, u32 requested, u32 *read_count) {
    if (!compressed)
        return WavReader::Read(output, requested, read_count);
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    if (cache == NULL) {
        *read_count = 0;
        return E_FAIL;
    }
    PCThreadMillisecondsPc();
    i32 bitstream = 0;
    char *cursor = (char *)output;
    i32 got = ov_read(&vorbis_state, cursor, requested, 0, 2, 1, &bitstream);
    if (got < 0) {
        if (read_count != NULL)
            *read_count = 0;
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return E_FAIL;
    }
    cursor += got;
    i32 total = got;
    while (got > 0 && (u32)total < requested) {
        got = ov_read(&vorbis_state, cursor, requested - total, 0, 2, 1, &bitstream);
        if (got < 0) {
            if (read_count != NULL)
                *read_count = 0;
            PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
            return E_FAIL;
        }
        total += got;
        cursor += got;
    }
    if ((u32)total < requested)
        memset(cursor, 0, requested - total);
    if (read_count != NULL)
        *read_count = total;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return S_OK;
}
HRESULT OggReader::Close(void) {
    if (!compressed)
        return WavReader::Close();
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    if (cache == NULL) {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return E_FAIL;
    }
    ov_clear(&vorbis_state);
    if (cache != NULL)
        --cache->references;
    cache = NULL;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return S_OK;
}
i32 OggReader::SeekTime(f32 seconds) {
    if (!compressed)
        return WavReader::SeekTime(seconds);
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    if (cache == NULL) {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return -1;
    }
    ov_time_seek(&vorbis_state, seconds);
    i32 result = (i32)ov_pcm_tell(&vorbis_state) * compressed_format.nBlockAlign;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return result;
}
u32 __cdecl OggReader::ReadCallbackPc(void *output, u32 element_size, u32 element_count, void *source) {
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    OggReader *reader = (OggReader *)source;
    if (reader->cache == NULL || reader->cache->data == NULL) {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return 0;
    }
    i32 bytes = reader->cache->bytes - reader->position;
    i32 wanted = element_size * element_count;
    if (wanted < bytes)
        bytes = wanted;
    if (bytes < 0) {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return 0;
    }
    PCByteCopy_candidate(output, reader->cache->data + reader->position, bytes);
    reader->position += bytes;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return (u32)bytes / element_size;
}
i32 __cdecl OggReader::SeekCallbackPc(void *source, i64 offset, i32 origin) {
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    OggReader *reader = (OggReader *)source;
    if (reader->cache == NULL) {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return 0;
    }
    i32 result;
    if (origin == 0)
        result = (i32)offset;
    else if (origin == 1)
        result = reader->position + (i32)offset;
    else if (origin == 2)
        result = reader->cache->bytes + (i32)offset;
    else {
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return 0;
    }
    reader->position = result;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return result;
}
i32 __cdecl OggReader::CloseCallbackPc(void *source) {
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    OggReader *reader = (OggReader *)source;
    if (reader->cache != NULL)
        --reader->cache->references;
    reader->cache = NULL;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return 1;
}
// NOLINTNEXTLINE(google-runtime-int): Original upstream long type preserves the native C ABI.
long __cdecl OggReader::TellCallbackPc(void *source) {
    OggReader *reader = (OggReader *)source;
    return reader->cache != NULL ? reader->position : 0;
}
HRESULT OggReader::Open(NUDATHDR *header, char *path, WAVEFORMATEX *output) {
    PCThreadMillisecondsPc();
    if (NuStrIStr(path, ".ogg") == NULL) {
        compressed = 0;
        return WavReader::Open(header, path, output);
    }
    PCThreadCriticalSectionEnter_candidate(PCOggCriticalSectionPc);
    compressed = 1;
    cache = NULL;
    for (i32 i = 0; i < 5; ++i) {
        if (NuStrICmp(PCOggCachePoolPc[i].path, path) == 0 && PCOggCachePoolPc[i].references <= 0) {
            cache = &PCOggCachePoolPc[i];
            break;
        }
    }
    if (cache == NULL) {
        i32 index = -1;
        for (i32 i = 0; i < 5; ++i) {
            if (PCOggCachePoolPc[i].references <= 0) {
                if (PCOggCachePoolPc[i].bytes == 0) {
                    index = i;
                    break;
                }
                if (index == -1 || PCOggCachePoolPc[i].last_used < PCOggCachePoolPc[index].last_used)
                    index = i;
            }
        }
        if (index == -1)
            index = 0;
        cache = &PCOggCachePoolPc[index];
        if (cache->data != NULL) {
            memset(cache->data, 0xcd, cache->bytes);
            NuFree(cache->data, ".\\pc\\oggreader.cpp", 132);
            cache->data = NULL;
        }
        HANDLE input = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0x08000001, NULL);
        if (input == INVALID_HANDLE_VALUE) {
            cache = NULL;
            PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
            return E_FAIL;
        }
        cache->bytes = GetFileSize(input, NULL);
        if (cache->bytes <= 0) {
            cache = NULL;
            CloseHandle(input);
            PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
            return E_FAIL;
        }
        cache->data = (u8 *)NuAlloc(cache->bytes, ".\\pc\\oggreader.cpp", 161);
        position = 0;
        if (cache->data == NULL) {
            cache = NULL;
            CloseHandle(input);
            PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
            return E_FAIL;
        }
        cache->last_used = (u32)PCThreadMillisecondsPc();
        ++cache->references;
        NuStrCpy(cache->path, path);
        DWORD bytes = 0;
        ReadFile(input, cache->data, cache->bytes, &bytes, NULL);
        CloseHandle(input);
    } else {
        position = 0;
        cache->last_used = (u32)PCThreadMillisecondsPc();
        ++cache->references;
    }
    PCThreadMillisecondsPc();
    ov_callbacks callbacks = {ReadCallbackPc, SeekCallbackPc, CloseCallbackPc, TellCallbackPc};
    if (ov_open_callbacks(this, &vorbis_state, NULL, 0, callbacks) < 0) {
        if (cache != NULL)
            --cache->references;
        cache = NULL;
        PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
        return E_FAIL;
    }
    vorbis_info *info = ov_info(&vorbis_state, 0);
    compressed_format.wFormatTag = 1;
    compressed_format.nChannels = (u16)info->channels;
    compressed_format.nSamplesPerSec = info->rate;
    compressed_format.nAvgBytesPerSec = (info->rate * info->channels * 16) / 8;
    compressed_format.nBlockAlign = (u16)((info->channels * 16) / 8);
    compressed_format.wBitsPerSample = 16;
    compressed_format.cbSize = 0;
    *output = compressed_format;
    PCThreadCriticalSectionLeave_candidate(PCOggCriticalSectionPc);
    return S_OK;
}
