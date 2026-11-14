#ifndef SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/numath/numtx.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/numath/numtx.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART == 1 && !defined(SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP)

struct NUANIM_TIME2_PC;
struct NUHGOBJ_EVAL_PC;
struct cut_load_locator_instance_s;
struct nuspecial_handle_s;

typedef struct instnugcutscene_s {
    struct instnugcutscene_s *next;
    struct instnugcutscene_s *previous;
    char name[16];
    NUMTX matrix;
    void *scene;
    u8 opaque_5c_88[0x2c];
    u32 flags_88;
    u32 flags_8c;
    f32 current_frame;
    f32 render_frame;
    f32 rate;
    u8 opaque_9c_b0[0x14];
    struct instnugcutscene_s *chained_instance;
    i32 unknown_b4;
    u8 opaque_b8_f8[0x40];
} INSTNUGCUTSCENE;

STATIC_ASSERT(offsetof(INSTNUGCUTSCENE, scene) == 0x58);
STATIC_ASSERT(offsetof(INSTNUGCUTSCENE, flags_88) == 0x88);
STATIC_ASSERT(offsetof(INSTNUGCUTSCENE, current_frame) == 0x90);
STATIC_ASSERT(offsetof(INSTNUGCUTSCENE, rate) == 0x98);
STATIC_ASSERT(offsetof(INSTNUGCUTSCENE, chained_instance) == 0xb0);
STATIC_ASSERT(sizeof(INSTNUGCUTSCENE) == 0xf8);
#ifdef __cplusplus
extern "C" {
#endif
    extern void (*UpdateLocatorVfxFn)(i32, NUMTX *);
    extern i32 (*TriggerLocatorVfxFn)(i32, NUMTX *);
    extern void (*NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    extern void (*NuCutSceneRigidPostRender)(void *, struct nuspecial_handle_s *, NUMTX *);
    void NuSetCutSceneResetCharactersFn(void (*)(INSTNUGCUTSCENE *));
    extern i32 (*LookupLocatorVfxFn)(char *);
    extern i32 (*NuCutSceneRequestSFX)(char *);
    void instNuGCutSceneSetMtx(INSTNUGCUTSCENE *inst, NUMTX *mtx);

    extern void (*NuCutSceneDestroyCharacters)(void *);
    extern void (*NuCutSceneCharacterDestroyData)(void *, void *);
    extern void (*NuCutSceneCharacterEval)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
    extern void (*NuCutSceneCharacterRelease)(void *, void *);
    extern void (*ReleaseLocatorVfxFn)(i32);
    void NuGCutSceneDestroy(void *);
    extern void (*NuCutSceneCharacterProcess)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    extern void (*NuCutSceneSFXFixUp)(INSTNUGCUTSCENE *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP
#endif
#endif

#if SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART == 2 && !defined(SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct NUHGOBJ_EVAL_PC *(*NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);
    extern void (*NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, struct cut_load_locator_instance_s *,
                                       CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    void NuGCutSceneSysInit(CUT_LOCATOR_FUNCTION_ENTRY_PC *);
    void NuSetCutSceneCharacterRenderFn(CUT_DRAW_RENDER_CALLBACK_PC);
    void NuSetCutSceneCharacterEvalFn(CUT_DRAW_EVAL_CALLBACK_PC);
    void NuSetCutSceneFindCharactersFn(void (*)(CUT_SCENE_LOAD_PC *));
    void NuSetCutSceneCharacterCreateDataFn(void (*)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *));
    void NuSetCutSceneRigidPostRenderFn(CUT_DRAW_RIGID_CALLBACK_PC);
    i32 FindMtlInHGObj(struct NUHGOBJ_EVAL_PC *, i32);
    extern void (*NuCutSceneFindCharacters)(CUT_SCENE_LOAD_PC *);
    extern void (*NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *);
    extern void (*NuCutSceneRigidCollisionCheck)(struct cut_load_rigid_s *, NUMTX *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP
#endif
#endif

#if SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART == 3 && !defined(SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuGCutLocatorIsVisble_3(CUT_LOCATOR_GENERIC_PC *, f32, f32 *, f32 *);
    i32 NuGCutLocatorIsVisble(CUT_LOCATOR_GENERIC_PC *, f32, struct NUANIM_TIME2_PC *, f32 *, f32 *);
    f32 *NuGCutLocatorCalcMtx(CUT_LOCATOR_GENERIC_PC *, f32, NUMTX *, struct NUANIM_TIME2_PC *);
    void NuGCutCharAnimProcess(CUT_CHARACTER_GENERIC_PC *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUCORE_NUGCUTSCENE_H_STOP
