/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct CHARACTER_SHADOW_PACKET {    // size 0x64
        CHARACTER_SHADOW_RESET_SLOT shadows[5]; // 0x00
    } CHARACTER_SHADOW_PACKET;

    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT) * 0 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT, position) + offsetof(NUVEC, y) ==
                  4);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT) * 1 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT, position) + offsetof(NUVEC, y) ==
                  0x18);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT) * 2 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT, position) + offsetof(NUVEC, y) ==
                  0x2c);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT) * 3 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT, position) + offsetof(NUVEC, y) ==
                  0x40);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT) * 4 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT, position) + offsetof(NUVEC, y) ==
                  0x54);

    void CharShadows_Reset(CHARACTER_SHADOW_PACKET *);
    void CharShadows_Update(void);
    void CharShadows_Draw(void);
#ifdef __cplusplus
}
#endif
