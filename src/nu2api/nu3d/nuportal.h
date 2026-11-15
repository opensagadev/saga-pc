#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"

struct NUGSCN_s;
struct NUPORTAL_FRUSTUM;

typedef struct NUPORTAL { // size 0x20
    NUPLANE plane;        // 0x00
    NUVEC *vertices;      // 0x10
    i16 vertex_count;     // 0x14
    u16 front_room;       // 0x16
    u16 back_room;        // 0x18
    i8 id;                // 0x1a
    u8 unknown_1b[0x1];   // 0x1b
    u32 flags;            // 0x1c
} NUPORTAL;

typedef struct NUPORTALSPHERE { // size 0x10
    NUVEC center;               // 0x0
    f32 radius;                 // 0xc
} NUPORTALSPHERE;

#ifdef __cplusplus
extern "C" {
#endif
    i32 clipTestSphere(NUPORTALSPHERE *, struct NUPORTAL_FRUSTUM *);
    void NuPortalInit(void);
    /* All aggregate views stay named/local and already asserted; no new shared field/data required. */
    i32 clipTestBox(NUVEC *, NUVEC *, NUPLANE *, i32);
    i32 NuPortalClipTestBox(NUVEC *, NUVEC *, struct NUPORTAL_FRUSTUM *);
    void NuPortalSetActiveDirect(NUPORTAL *portal, i32 active);
#ifdef __cplusplus
}
#endif

enum { NUPORTAL_FLAG_ACTIVE = 1 };

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
