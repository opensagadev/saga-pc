#ifndef SAGA_TRANSITIONAL_LIBOGG_OS_TYPES_H
#define SAGA_TRANSITIONAL_LIBOGG_OS_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#ifdef __cplusplus
extern "C" {
#endif
    // NOLINTBEGIN(google-runtime-int): Preserve the verified upstream Win32 C long/short type identities.
    typedef __int64 ogg_int64_t;
    typedef __int32 ogg_int32_t;
    typedef unsigned __int32 ogg_uint32_t;
    typedef __int16 ogg_int16_t;
    typedef unsigned __int16 ogg_uint16_t;
// NOLINTEND(google-runtime-int)
#ifdef __cplusplus
}
#endif

#endif
