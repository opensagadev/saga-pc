#ifndef SAGA_NU2API_NUCORE_IMPLODE_H
#define SAGA_NU2API_NUCORE_IMPLODE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

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
#ifdef __cplusplus
}
#endif

#endif
