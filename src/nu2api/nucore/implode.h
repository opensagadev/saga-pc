/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 ExplodeBufferSize(char *);
    i32 ExplodeCompressedSize(char *);
    extern char *implode_inbuffer;
    extern char *implode_outbuffer;
    extern u32 implode_bitbuf;
    extern u32 implode_compsize;
    extern u16 implode_left[0x3fb];
    extern u16 implode_right[0x3fb];
    extern u32 implode_origsize;

    i32 ExplodeBufferNoHeader(char *, char *, i32, i32);
    u32 ImplodeDecodeC(void);
    u32 ImplodeDecodeOffset(void);
    u32 ImplodeGetBits(i32 count);
    u8 ImplodeGetByteFromMem(void);
    void ImplodeError(char *msg, ...);
    void ImplodeFillBuf(i32 count);
    void ImplodeHufDecodeStart(void);
    void ImplodeInitGetBits(void);
    void ImplodeMakeTable(i32, u8 *, i32, u16 *);
#ifdef __cplusplus
}
#endif
