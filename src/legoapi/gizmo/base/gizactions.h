/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_STOP)

struct AIPACKET;
struct AISCRIPTPROCESS;
struct AISYS_PC;
struct API_ACTION_INFO;
struct API_EXTRA_ACTION;
struct FLOWBOX;

typedef struct GIZFLOW {       // size 0x10
    void *gizmo_sys;           // 0x0
    i32 flowbox_count;         // 0x4
    struct FLOWBOX *flowboxes; // 0x8
    u8 frame;                  // 0xc
    u8 pointers_need_reset;    // 0xd
} GIZFLOW;

#ifdef __cplusplus
extern "C" {
#endif
    extern const f32 PlayerBuildItNegativeLimit;
    void SetActionInfo(struct API_ACTION_INFO *, struct API_EXTRA_ACTION *);
    i32 ActionFromName(const char *);
    extern const char AICameraLiteral7580f0[12];
    extern const char AICameraLiteral7580e4[12];
    extern const char AICameraLiteral7580dc[7];
    extern const char AICameraLiteral7580cc[13];
    extern const char AICameraLiteral7580c0[9];
    extern const char AICameraLiteral7580b0[13];
    extern const char AICameraLiteral7580a4[9];
    extern const char AITransferLiteral7583f8[8];
    extern const char AITransferLiteral7581d4[10];
    extern const char AITransferLiteral7581ec[9];
    extern const char AITransferLiteral7581e0[9];
    extern const char GizObstacleActionNamePC[5];
    extern const char GizObstacleActionBackwardPC[9];
    extern const char GizObstacleActionForwardPC[8];
    extern const char GizObstacleActionStayOpenPC[9];
    extern const char GizObstacleActionStayShutPC[9];
    extern const char NuMusicDebugClassInvalid[8];
    extern const char NuMusicDebugClassQuiet[6];
    extern const char NuMusicDebugClassAction[7];
    extern const char NuMusicDebugClassSignature[10];
    extern const char NuMusicDebugClassOverlay[8];
    extern const char NuMusicDebugClassCutscene[9];
    extern const char NuMusicDebugClassNoMusic[8];
    extern const char NuMusicDebugVoiceFormat[9];
    extern const char NuMusicDebugTrackFormat[9];
    extern const char NuMusicDebugClassFormat[9];
    extern const char NuMusicDebugTrackNone[11];
    extern const char NuMusicDebugClassNone[11];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    i32 Action_CameraCut(struct AISYS_PC *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_EndCameraCut(struct AISYS_PC *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    void RegisterGizActions(GIZACTIONDEF *definitions);
    extern GIZACTIONDEF game_gizactiondefs[24];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMO_BASE_GIZACTIONS_H_STOP
