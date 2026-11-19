#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/props/objects/legoapi_signal.h"

/* SUIT_INITIALIZER_RECORD.flags, SUIT_INITIALIZER_RECORD.flags: which suit it is, from SuitRecordInit (each record's text id
 * names its suit); 0x100, set for the glide, demolition, water, technology and attract suits, is not named yet. */
#define SUIT_SHADOW 0x1
#define SUIT_GLIDE 0x2
#define SUIT_DEMOLITION 0x4
#define SUIT_SONAR 0x8
#define SUIT_WATER 0x10
#define SUIT_TECHNOLOGY 0x20
#define SUIT_MAGNET 0x40
#define SUIT_ATTRACT 0x80

#ifdef __cplusplus
extern "C" {
#endif
    SUIT_INITIALIZER_RECORD *Suit_GetDefault(i32);
    extern SUIT_INITIALIZER_RECORD SuitRecordInit[10];
    i32 Suit_GetIndex(SUIT_INITIALIZER_RECORD *);
    void Suits_Init(void);
    SUIT_INITIALIZER_RECORD *Suit_GetLast(i32, i32);
    struct SUIT_INITIALIZER_RECORD *Suit_FindFromLetter(char letter);
#ifdef __cplusplus
}
#endif
