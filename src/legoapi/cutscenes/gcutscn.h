#ifndef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/cutscenes/cutscenes.h, nu2api/nu3d/nu3d_includes.h, nu2api/nu3d/nugscn.h,
 * nu2api/nu3d/numtl.h, nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/nucore/nugcutscene.h,
 * nu2api/numath/numtx.h, nu2api/numath/nuvec.h, nu2api/trigger.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/cutscenes/cutscenes.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/trigger.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 1 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef union DWA_FRAME_HEADER_PC {
    f32 duration;
    u32 magic;
} DWA_FRAME_HEADER_PC;

typedef union DWA_NODE_COUNT_PC {
    i16 legacy;
    u16 modern;
} DWA_NODE_COUNT_PC;

typedef struct NUANIMDATA2_PC {
    DWA_FRAME_HEADER_PC header;
    DWA_NODE_COUNT_PC node_count;
    i16 curve_count;
    i16 chunk_count;
    u16 modern_frame_count;
    DWA_CURVE_CELL_PC *curves;
    i8 *curve_types;
} NUANIMDATA2_PC;

typedef struct CUT_LOCATOR_ANIMATION_PC {
    NUMTX base_matrix;
    NUVEC pivot;
    f32 locator_scale;
    ANI3_HEADER_PC *animation;
} CUT_LOCATOR_ANIMATION_PC;

typedef struct CUT_CHARACTER_ANIMATION_PC {
    NUMTX base_matrix;
    u8 unknown_40[4];
    ANI3_HEADER_PC *animation;
    struct NUANIMDATA2_PC *face_animation;
    struct NUANIMDATA2_PC *extra_animation;
    u8 unknown_50[0x4];
    void *locator;
    f32 animation_rate;
    u8 flags;
    u8 animation_index;
    u8 has_locator;
    u8 field_5f;
    u8 locator_index;
    u8 blend_time;
    u8 unknown_62;
    i8 start_frame;
} CUT_CHARACTER_ANIMATION_PC;

STATIC_ASSERT(offsetof(CUT_LOCATOR_ANIMATION_PC, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation) == 0x44);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation_rate) == 0x58);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, flags) == 0x5c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, blend_time) == 0x61);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, start_frame) == 0x63);

STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation_index) == 0x5d);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_count) == 6);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, chunk_count) == 8);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, modern_frame_count) == 0x0a);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curves) == 0x0c);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_types) == 0x10);

STATIC_ASSERT(offsetof(NUANIMDATA2_PC, header) == 0);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, node_count) == 4);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_types) == 0x10);
STATIC_ASSERT(sizeof(NUANIMDATA2_PC) == 0x14);

typedef struct cut_scene_load_pc_s CUT_SCENE_LOAD_PC;
typedef struct cut_load_character_s CUT_LOAD_CHARACTER;
typedef struct cut_load_character_instance_s CUT_LOAD_CHARACTER_INSTANCE;
typedef struct cut_locator_function_entry_pc_s CUT_LOCATOR_FUNCTION_ENTRY_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CutSceneCameraFocalLengthPc;

    typedef struct cut_load_locator_s CUT_LOAD_LOCATOR;
    typedef struct cut_load_locator_type_s CUT_LOAD_LOCATOR_TYPE;
    typedef struct cut_load_locator_system_s CUT_LOAD_LOCATOR_SYSTEM;
    extern void (*NuCutSceneResetCharacters)(INSTNUGCUTSCENE *);
    typedef void (*CUT_DRAW_RENDER_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    typedef void (*CUT_DRAW_EVAL_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
    typedef void (*CUT_DRAW_RIGID_CALLBACK_PC)(void *, NUSPECIALHANDLE *, NUMTX *);
    void instGetLookAtLocatorInfo(INSTNUGCUTSCENE *, CUT_LOOKAT_STATE_PC *);
    void instNuGCutLocatorUpdate(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                                 CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    extern i32 CutSceneSingleStreamBufferPc;
    extern CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctionsPc;
    f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION_PC *, NUMTX *, f32);
    void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION_PC *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
    i32 PCCutAni3CurveInteger(ANI3_HEADER_PC *, f32 *, i32);
    extern const f64 AIPathAngularScale;

    typedef struct state_anim_pc_s {
        u16 count;
        u16 unknown_0002;
        f32 *times;
        u8 *values;
    } STATE_ANIM_PC;

    STATIC_ASSERT(sizeof(STATE_ANIM_PC) == 12);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, count) == 0);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, times) == 4);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, values) == 8);
    void instNuGCutSceneDestroy(INSTNUGCUTSCENE *);
    void instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *);

    void instNuGCutSceneEndFirstFrame(INSTNUGCUTSCENE *);
    void instNuGCutSceneStart(INSTNUGCUTSCENE *);
    void instNuGCutSceneReset(INSTNUGCUTSCENE *);
    INSTNUGCUTSCENE *instNuGCutSceneFindActivePc(char *name);
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
    extern INSTNUGCUTSCENE *CutSceneLoadingInstancePc;
    extern i32 CutSceneUpdateCamerasPc;
    i32 instNuGCutSceneIsFinished(INSTNUGCUTSCENE *);
    void instNuGCutScenePause(INSTNUGCUTSCENE *, i32 paused);
    void instNuGCutSceneStop(INSTNUGCUTSCENE *);
    void NuGCutSceneSysPostBackgroundLoad(void);
    i32 instNuGCutSceneSwapBuffers(INSTNUGCUTSCENE *, i32 force);
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

typedef struct CUT_RIGID_ANIMATION_PC {
    NUMTX base_matrix;
    char *name;
    struct NUGSCN_s *scene;
    void *special_object;
    ANI3_HEADER_PC *animation;
} CUT_RIGID_ANIMATION_PC;

STATIC_ASSERT(offsetof(CUT_RIGID_ANIMATION_PC, animation) == 0x4c);
#ifdef __cplusplus
extern "C" {
#endif
    i32 FindTexAnimFromMtl(NUHGOBJ_EVAL_PC *, NUMTL *);
    void CutGetJointOverrideMatrixPc(NUHGOBJ_EVAL_PC *, i32, NUMTX *);
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

typedef struct CUT_LOCATOR_GENERIC_PC {
    NUMTX base_matrix;
    u8 unknown_40_4f[16];
    CUT_ANIMATION_PC *animation;
} CUT_LOCATOR_GENERIC_PC;

typedef struct CUT_RIGID_GENERIC_PC {
    NUMTX base_matrix;
    u8 unknown_40_4b[12];
    CUT_ANIMATION_PC *animation;
} CUT_RIGID_GENERIC_PC;

typedef struct CUT_CHARACTER_GENERIC_PC {
    NUMTX base_matrix;
    char *name;
    CUT_ANIMATION_PC *animation;
    struct NUANIMDATA2_PC *face_animation;
    struct NUANIMDATA2_PC *extra_animation;
    u8 unknown_50[0x4];
    void *locator;
    f32 animation_rate;
    u8 flags;
    u8 animation_index;
    u8 has_locator;
    u8 field_5f;
    u8 locator_index;
    u8 blend_time;
    u8 unknown_62;
    i8 start_frame;
} CUT_CHARACTER_GENERIC_PC;

STATIC_ASSERT(offsetof(CUT_LOCATOR_GENERIC_PC, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_RIGID_GENERIC_PC, animation) == 0x4c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation) == 0x44);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation_rate) == 0x58);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, flags) == 0x5c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation_index) == 0x5d);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, blend_time) == 0x61);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, start_frame) == 0x63);
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
