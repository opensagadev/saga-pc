#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"

struct AIAREA;

typedef struct EDAIAREA_s { // size 0x48
    /* Editor area record: a list link ahead of an AIAREA image (rebuild copies it). */
    NULISTLNK link;    // 0x00
    char name[16];     // 0x08
    NUVEC position;    // 0x18
    NUVEC size;        // 0x24
    i16 rotation;      // 0x30
    u8 reserved_32;     // 0x32
    u8 flags;          // 0x33
    u8 reserved_34[20]; // 0x34
} EDAIAREA_s;
