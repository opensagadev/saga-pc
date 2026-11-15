#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/numath/numtx.h"

struct NUANIM_TIME2;
struct NUHGOBJ_EVAL;
struct cut_load_locator_instance_s;
struct nuhspecial_s;

/* instnugcutscene_s.flags_88 (and the cut scene instance views): what instNuGCutSceneStart, End, the
 * instNuGCutSceneSet* transforms and instNuGCutSceneClipTest set and test. */
#define NUGCUTSCENE_INST_PLAYING 0x2       /* set by Start, cleared by End */
#define NUGCUTSCENE_INST_OWN_MATRIX 0x80   /* set by every SetTranslation/Rotate call: draw and clip with the instance matrix */
#define NUGCUTSCENE_INST_CLIP_BOUNDS 0x100 /* ClipTest tests the scene bounds */
#define NUGCUTSCENE_INST_CLIP_RADIUS 0x200 /* ClipTest tests the camera distance */
#define NUGCUTSCENE_INST_VISIBLE 0x400     /* ClipTest's result */
#define NUGCUTSCENE_INST_ENDED 0x1000      /* set by End, cleared by Start and Reset */

typedef struct instnugcutscene_s {              // size 0xf8
    struct instnugcutscene_s *next;             // 0x00
    struct instnugcutscene_s *previous;         // 0x04
    char name[16];                              // 0x08
    NUMTX matrix;                               // 0x18
    void *scene;                                // 0x58
    u8 opaque_5c_88[0x2c];                      // 0x5c
    u32 flags_88;                               // 0x88
    u32 flags_8c;                               // 0x8c
    f32 current_frame;                          // 0x90
    f32 render_frame;                           // 0x94
    f32 rate;                                   // 0x98
    u8 opaque_9c_b0[0x14];                      // 0x9c
    struct instnugcutscene_s *chained_instance; // 0xb0
    i32 unknown_b4;                             // 0xb4
    u8 opaque_b8_f8[0x40];                      // 0xb8
} INSTNUGCUTSCENE;

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*UpdateLocatorVfxFn)(i32, NUMTX *);
    extern i32 (*TriggerLocatorVfxFn)(i32, NUMTX *);
    extern void (*NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    extern void (*NuCutSceneRigidPostRender)(void *, struct nuhspecial_s *, NUMTX *);
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

#ifdef __cplusplus
extern "C" {
#endif
    extern struct NUHGOBJ_EVAL *(*NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);
    extern void (*NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, struct cut_load_locator_instance_s *, CUT_LOAD_LOCATOR *, f32, NUMTX *,
                                       i32);
    void NuGCutSceneSysInit(CUT_LOCATOR_FUNCTION_ENTRY *);
    void NuSetCutSceneCharacterRenderFn(CUT_DRAW_RENDER_CALLBACK);
    void NuSetCutSceneCharacterEvalFn(CUT_DRAW_EVAL_CALLBACK);
    void NuSetCutSceneFindCharactersFn(void (*)(CUT_SCENE_LOAD *));
    void NuSetCutSceneCharacterCreateDataFn(void (*)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *));
    void NuSetCutSceneRigidPostRenderFn(CUT_DRAW_RIGID_CALLBACK);
    i32 FindMtlInHGObj(struct NUHGOBJ_EVAL *, i32);
    extern void (*NuCutSceneFindCharacters)(CUT_SCENE_LOAD *);
    extern void (*NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *);
    extern void (*NuCutSceneRigidCollisionCheck)(struct cut_load_rigid_s *, NUMTX *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuGCutLocatorIsVisble_3(CUT_LOCATOR_GENERIC *, f32, f32 *, f32 *);
    i32 NuGCutLocatorIsVisble(CUT_LOCATOR_GENERIC *, f32, struct NUANIM_TIME2 *, f32 *, f32 *);
    f32 *NuGCutLocatorCalcMtx(CUT_LOCATOR_GENERIC *, f32, NUMTX *, struct NUANIM_TIME2 *);
    void NuGCutCharAnimProcess(CUT_CHARACTER_GENERIC *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
#ifdef __cplusplus
}
#endif
