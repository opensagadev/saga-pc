#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CHARSHADOWS_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHARSHADOWS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct CHARACTER_SHADOW_PACKET_PC {
        CHARACTER_SHADOW_RESET_SLOT_PC shadows[5];
    } CHARACTER_SHADOW_PACKET_PC;

    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET_PC, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT_PC) * 0 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, position) + offsetof(NUVEC, y) ==
                  4);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET_PC, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT_PC) * 1 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, position) + offsetof(NUVEC, y) ==
                  0x18);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET_PC, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT_PC) * 2 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, position) + offsetof(NUVEC, y) ==
                  0x2c);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET_PC, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT_PC) * 3 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, position) + offsetof(NUVEC, y) ==
                  0x40);
    STATIC_ASSERT(offsetof(CHARACTER_SHADOW_PACKET_PC, shadows) + sizeof(CHARACTER_SHADOW_RESET_SLOT_PC) * 4 +
                      offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, position) + offsetof(NUVEC, y) ==
                  0x54);

    void CharShadows_Reset(CHARACTER_SHADOW_PACKET_PC *);
    void CharShadows_Update(void);
    void CharShadows_Draw(void);
#ifdef __cplusplus
}
#endif

#endif
