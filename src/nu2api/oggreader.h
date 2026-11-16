#pragma once

#include "nu2api/nualloc.h"
#include "nu2api/nucore/common.h"
#include "nu2api/wavreader.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct PC_OGG_CACHE_RECORD;
    extern struct PC_OGG_CACHE_RECORD *PCOggCachePool;
    extern i32 PCOggCriticalSection;
#ifdef __cplusplus
}
#endif

/* The reader needs OggVorbis_File: units include vorbis/vorbisfile.h first, with their own name for its static
 * seek callback (see nustream.cpp). */
#if defined(__cplusplus) && defined(_OV_FILE_H_)
#pragma pack(push, 4)

class OggReader : public WavReader { // size 0x338
  public:
    OggReader(void);
    virtual ~OggReader(void);
    virtual HRESULT Open(NUDATHDR *, char *, WAVEFORMATEX *);
    virtual HRESULT Unknown08(u32, u32);
    virtual HRESULT Close(void);
    virtual WAVEFORMATEX *GetFormat(void);
    virtual i32 GetSize(void);
    virtual HRESULT Reset(void);
    virtual HRESULT Read(void *, u32, u32 *);
    virtual i32 SeekTime(f32);
    static u32 ReadCallback(void *, u32, u32, void *);
    static i32 SeekCallback(void *, i64, i32);
    static i32 CloseCallback(void *);
    // NOLINTNEXTLINE(google-runtime-int): the upstream long type keeps the C ABI.
    static long TellCallback(void *);
    u8 compressed;                  // 0x040
    u8 unknown_041[7];              // 0x041
    OggVorbis_File vorbis_state;    // 0x048
    PC_OGG_CACHE_RECORD *cache;     // 0x318
    i32 position;                   // 0x31c
    WAVEFORMATEX compressed_format; // 0x320
    u8 unknown_332[6];              // 0x332
};

#pragma pack(pop)
#endif
