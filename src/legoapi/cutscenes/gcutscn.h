/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 1 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

struct ANI3_HEADER_PC;
struct CUT_LOOKAT_STATE_PC;
union DWA_CURVE_CELL;
struct NUANIMDATA2_SKELETAL;
struct NUHGOBJ_EVAL_PC;
struct cut_load_locator_instance_s;
struct instnugcutscene_s;
struct numtl_s;
struct nuspecial_handle_s;

typedef union DWA_FRAME_HEADER {
    f32 duration;
    u32 magic;
} DWA_FRAME_HEADER;

typedef union DWA_NODE_COUNT {
    i16 legacy;
    u16 modern;
} DWA_NODE_COUNT;

typedef struct NUANIMDATA2 {      // size 0x14
    DWA_FRAME_HEADER header;      // 0x00
    DWA_NODE_COUNT node_count;    // 0x04
    i16 curve_count;              // 0x06
    i16 chunk_count;              // 0x08
    u16 modern_frame_count;       // 0x0a
    union DWA_CURVE_CELL *curves; // 0x0c
    i8 *curve_types;              // 0x10
} NUANIMDATA2;

typedef struct CUT_LOCATOR_ANIMATION_PC { // size 0x54
    NUMTX base_matrix;                    // 0x00
    NUVEC pivot;                          // 0x40
    f32 locator_scale;                    // 0x4c
    struct ANI3_HEADER_PC *animation;     // 0x50
} CUT_LOCATOR_ANIMATION_PC;

typedef struct CUT_CHARACTER_ANIMATION_PC { // size 0x64
    NUMTX base_matrix;                      // 0x00
    u8 unknown_40[4];                       // 0x40
    struct ANI3_HEADER_PC *animation;       // 0x44
    struct NUANIMDATA2 *face_animation;     // 0x48
    struct NUANIMDATA2 *extra_animation;    // 0x4c
    u8 unknown_50[0x4];                     // 0x50
    void *locator;                          // 0x54
    f32 animation_rate;                     // 0x58
    u8 flags;                               // 0x5c
    u8 animation_index;                     // 0x5d
    u8 has_locator;                         // 0x5e
    u8 field_5f;                            // 0x5f
    u8 locator_index;                       // 0x60
    u8 blend_time;                          // 0x61
    u8 unknown_62;                          // 0x62
    i8 start_frame;                         // 0x63
} CUT_CHARACTER_ANIMATION_PC;

typedef struct cut_scene_load_s CUT_SCENE_LOAD_PC;
typedef struct cut_load_character_s CUT_LOAD_CHARACTER;
typedef struct cut_load_character_instance_s CUT_LOAD_CHARACTER_INSTANCE;
typedef struct cut_locator_function_entry_s CUT_LOCATOR_FUNCTION_ENTRY_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CutSceneCameraFocalLengthPc;

    typedef struct cut_load_locator_s CUT_LOAD_LOCATOR;
    typedef struct cut_load_locator_type_s CUT_LOAD_LOCATOR_TYPE;
    typedef struct cut_load_locator_system_s CUT_LOAD_LOCATOR_SYSTEM;
    extern void (*NuCutSceneResetCharacters)(struct instnugcutscene_s *);
    typedef void (*CUT_DRAW_RENDER_CALLBACK_PC)(struct instnugcutscene_s *, void *, void *, void *, f32, i32);
    typedef void (*CUT_DRAW_EVAL_CALLBACK_PC)(struct instnugcutscene_s *, void *, void *, void *, f32);
    typedef void (*CUT_DRAW_RIGID_CALLBACK_PC)(void *, struct nuspecial_handle_s *, NUMTX *);
    void instGetLookAtLocatorInfo(struct instnugcutscene_s *, struct CUT_LOOKAT_STATE_PC *);
    void instNuGCutLocatorUpdate(struct instnugcutscene_s *, CUT_LOAD_LOCATOR_SYSTEM *,
                                 struct cut_load_locator_instance_s *, CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    extern i32 CutSceneSingleStreamBufferPc;
    extern CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctionsPc;
    f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION_PC *, NUMTX *, f32);
    void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION_PC *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
    i32 PCCutAni3CurveInteger(struct ANI3_HEADER_PC *, f32 *, i32);
    extern const f64 AIPathAngularScale;

    typedef struct state_anim_s { // size 0xc
        u16 count;                // 0x0
        u16 unknown_0002;         // 0x2
        f32 *times;               // 0x4
        u8 *values;               // 0x8
    } STATE_ANIM_PC;

    void instNuGCutSceneDestroy(struct instnugcutscene_s *);
    void instNuGCutSceneEndButNotSystems(struct instnugcutscene_s *);

    void instNuGCutSceneEndFirstFrame(struct instnugcutscene_s *);
    void instNuGCutSceneStart(struct instnugcutscene_s *);
    void instNuGCutSceneReset(struct instnugcutscene_s *);
    struct instnugcutscene_s *instNuGCutSceneFindActivePc(char *name);
    void NuGCutSceneSysRender(i32);
    struct cut_load_rigid_s;
    extern i32 CutSceneRenderRigidsPc;
    extern i32 CutSceneRenderCharactersPc;
    extern i32 CutSceneRenderLocatorsPc;

    void NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed);
    extern NUMTX CutSceneCameraMatrixPc;
    extern NUVEC CutSceneFocusLocationPc;
    extern u8 CutSceneUseFocusLocationPc;
    extern f32 CutSceneFocusDistancePc;
    extern f32 CutSceneFStopPc;
    extern i32 CutSceneCameraChangedPc;
    extern i32 CutSceneCameraDofModePc;
    extern struct instnugcutscene_s *CutSceneLoadingInstancePc;
    extern i32 CutSceneUpdateCamerasPc;
    i32 instNuGCutSceneIsFinished(struct instnugcutscene_s *);
    void instNuGCutScenePause(struct instnugcutscene_s *, i32 paused);
    void instNuGCutSceneStop(struct instnugcutscene_s *);
    void NuGCutSceneSysPostBackgroundLoad(void);
    i32 instNuGCutSceneSwapBuffers(struct instnugcutscene_s *, i32 force);
    void instNuGCutSceneServiceLoad(void);
    extern i32 CutSceneStreamLoadGuardPc;
    extern void *cutscene_load_instance;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 2 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef struct CUT_RIGID_ANIMATION_PC { // size 0x50
    NUMTX base_matrix;                  // 0x00
    char *name;                         // 0x40
    struct NUGSCN_s *scene;             // 0x44
    void *special_object;               // 0x48
    struct ANI3_HEADER_PC *animation;   // 0x4c
} CUT_RIGID_ANIMATION_PC;

#ifdef __cplusplus
extern "C" {
#endif
    i32 FindTexAnimFromMtl(struct NUHGOBJ_EVAL_PC *, struct numtl_s *);
    void CutGetJointOverrideMatrixPc(struct NUHGOBJ_EVAL_PC *, i32, NUMTX *);
    void PCCutRigidAni3(CUT_RIGID_ANIMATION_PC *, f32, NUMTX *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 3 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef struct CUT_LOCATOR_GENERIC_PC {     // size 0x54
    NUMTX base_matrix;                      // 0x00
    u8 unknown_40_4f[16];                   // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x50
} CUT_LOCATOR_GENERIC_PC;

typedef struct CUT_RIGID_GENERIC_PC {       // size 0x50
    NUMTX base_matrix;                      // 0x00
    u8 unknown_40_4b[12];                   // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x4c
} CUT_RIGID_GENERIC_PC;

typedef struct CUT_CHARACTER_GENERIC_PC {   // size 0x64
    NUMTX base_matrix;                      // 0x00
    char *name;                             // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x44
    struct NUANIMDATA2 *face_animation;     // 0x48
    struct NUANIMDATA2 *extra_animation;    // 0x4c
    u8 unknown_50[0x4];                     // 0x50
    void *locator;                          // 0x54
    f32 animation_rate;                     // 0x58
    u8 flags;                               // 0x5c
    u8 animation_index;                     // 0x5d
    u8 has_locator;                         // 0x5e
    u8 field_5f;                            // 0x5f
    u8 locator_index;                       // 0x60
    u8 blend_time;                          // 0x61
    u8 unknown_62;                          // 0x62
    i8 start_frame;                         // 0x63
} CUT_CHARACTER_GENERIC_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void NuGCutRigidCalcMtx(CUT_RIGID_GENERIC_PC *, f32, NUMTX *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
