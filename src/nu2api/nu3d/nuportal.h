#ifndef SAGA_NU2API_NU3D_NUPORTAL_H_PART
#define SAGA_NU2API_NU3D_NUPORTAL_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUPORTAL_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUPORTAL_H_STOP)

struct NUGSCN_s;
struct NUPORTAL_FRUSTUM_PC;

typedef struct NUPORTAL {
    NUPLANE plane;
    NUVEC *vertices;
    i16 vertex_count;
    u16 front_room;
    u16 back_room;
    i8 id;
    u8 unknown_1b[0x1];
    u32 flags;
} NUPORTAL;

STATIC_ASSERT(offsetof(NUPORTAL, flags) == 0x1c);
STATIC_ASSERT(offsetof(NUPORTAL, vertex_count) == 0x14);
STATIC_ASSERT(sizeof(NUPORTAL) == 0x20);
STATIC_ASSERT(offsetof(NUPORTAL, vertices) == 0x10);
STATIC_ASSERT(offsetof(NUPORTAL, front_room) == 0x16);
STATIC_ASSERT(offsetof(NUPORTAL, back_room) == 0x18);

typedef struct NUPORTALSPHERE_PC {
    NUVEC center;
    f32 radius;
} NUPORTALSPHERE_PC;

STATIC_ASSERT(sizeof(NUPORTALSPHERE_PC) == 0x10);
STATIC_ASSERT(offsetof(NUPORTALSPHERE_PC, radius) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    i32 clipTestSphere(NUPORTALSPHERE_PC *, struct NUPORTAL_FRUSTUM_PC *);
    void NuPortalInit(void);
    /* All aggregate views stay named/local and already asserted; no new shared field/data required. */
    i32 clipTestBox(NUVEC *, NUVEC *, NUPLANE *, i32);
    i32 NuPortalClipTestBox(NUVEC *, NUVEC *, struct NUPORTAL_FRUSTUM_PC *);
    void NuPortalSetActiveDirect(NUPORTAL *portal, i32 active);
#ifdef __cplusplus
}
#endif

enum { NUPORTAL_FLAG_ACTIVE = 1 };

#undef SAGA_NU2API_NU3D_NUPORTAL_H_PART
#define SAGA_NU2API_NU3D_NUPORTAL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUPORTAL_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUPORTAL_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUPORTAL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuPortalClipTest(struct NUGSCN_s *, NUVEC *, f32, i16);
    i16 NuPortalWhichRoom(struct NUGSCN_s *, NUVEC *);
    i32 NuPortalVisibility(struct NUGSCN_s *);
    void NuPortalMaxDepth(struct NUGSCN_s *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUPORTAL_H_PART
#define SAGA_NU2API_NU3D_NUPORTAL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUPORTAL_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUPORTAL_H_STOP
