#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufile.h"

#ifdef __cplusplus

struct PC_WAVE_CHUNK { // size 0xc
    u32 kind;          // 0x0
    u32 bytes;         // 0x4
    i32 offset;        // 0x8
};

#pragma pack(push, 4)

class WavReader { // size 0x40
  public:
    WavReader(void);
    virtual ~WavReader(void);
    virtual HRESULT Open(NUDATHDR *, char *, WAVEFORMATEX *);
    virtual HRESULT Unknown08(u32, u32);
    virtual HRESULT Close(void);
    virtual WAVEFORMATEX *GetFormat(void);
    virtual i32 GetSize(void);
    virtual HRESULT Reset(void);
    virtual HRESULT Read(void *, u32, u32 *);
    virtual i32 SeekTime(f32);
    u32 FourCc(const char *);
    i32 FindChunk(HANDLE, const char *, PC_WAVE_CHUNK *);
    i32 SkipChunk(HANDLE, const PC_WAVE_CHUNK *);
    u32 unknown_004;     // 0x04
    HANDLE file;         // 0x08
    u32 unknown_00c;     // 0x0c
    i64 base_position;   // 0x10
    WAVEFORMATEX format; // 0x18
    u8 unknown_02a[2];   // 0x2a
    i32 first_sample;    // 0x2c
    i32 end_sample;      // 0x30
    i32 current_sample;  // 0x34
    i32 data_offset;     // 0x38
    u32 data_bytes;      // 0x3c
};

#pragma pack(pop)
#endif
