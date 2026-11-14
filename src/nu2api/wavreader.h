#ifndef SAGA_NU2API_WAVREADER_H
#define SAGA_NU2API_WAVREADER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

#ifdef __cplusplus

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
#endif

#endif
