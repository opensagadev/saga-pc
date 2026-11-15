#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/core/gameliball.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct CHARACTER_SHADOW_PACKET {    // size 0x64
        CHARACTER_SHADOW_RESET_SLOT shadows[5]; // 0x00
    } CHARACTER_SHADOW_PACKET;

    void CharShadows_Reset(CHARACTER_SHADOW_PACKET *);
    void CharShadows_Update(void);
    void CharShadows_Draw(void);
#ifdef __cplusplus
}
#endif
