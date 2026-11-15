#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);

    extern i16 GrabMachineId;
    extern i16 GrabMagnetId;
    extern i16 RobotBaseId;
    void Grabber_Configure(WORLDINFO *, char *);
#ifdef __cplusplus
}
#endif

/* grabber_s.flags (grabber.c): the bits of its bit-field; rotate and shadow are the grabbers config keywords. */
#define GRABBER_DRAWN 0x1          /* APIDrawCharacterModel drew the grabber this frame */
#define GRABBER_LIFTED 0x2         /* the load has been lifted (pick-up frame, Grabber_Drop) */
#define GRABBER_SWITCH_LATCHED 0x4 /* released without a controller: wait for the action switch to reset */
#define GRABBER_ROTATE 0x8         /* "rotate" */
#define GRABBER_SHADOW 0x10        /* "shadow" (on unless "shadow off") */
