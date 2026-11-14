#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/characters/core/characters.h, legoapi/items/base/apiobject.h,
 * legoapi/menus/screens/customise.h, legoapi/world/world.h, nu2api/nu3d/nu3d_includes.h, nu2api/nufile/nufpar.h (not
 * all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufile/nufpar.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)

typedef struct CUSTOMPIECE_LAYERS_PC {
    u8 unknown_00[0x12];
    u8 layer_flags;
    u8 unknown_13[0x15];
} CUSTOMPIECE_LAYERS_PC;

STATIC_ASSERT(sizeof(CUSTOMPIECE_LAYERS_PC) == 0x28);
STATIC_ASSERT(offsetof(CUSTOMPIECE_LAYERS_PC, layer_flags) == 0x12);
#ifdef __cplusplus
extern "C" {
#endif
    void Customiser_Dump(void);
    void Customiser_DumpAccessories(void *customiser);
    void Customiser_RestoreModelTextureIDs(void *customiser);
    extern i32 customiser_quit;
    extern i32 customiser_quit;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)

typedef struct KILLPARTS_ACCESSORY_PC {
    void *scene;
    NUSPECIALHANDLE special;
    u8 unknown_10[0x10];
} KILLPARTS_ACCESSORY_PC;

STATIC_ASSERT(sizeof(KILLPARTS_ACCESSORY_PC) == 0x20);
STATIC_ASSERT(offsetof(KILLPARTS_ACCESSORY_PC, special) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern KILLPARTS_ACCESSORY_PC KillPartsAccessoriesPc[2][9];
    void Customiser_DumpAll(void *, WORLDINFO *);
    void Customiser_SaveModelTextureIDs(void *customiser, struct CHARACTER_MODEL_LAYERS_PC *model);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    void Customiser_SetAnimsToLoad(CUSTOMISER_LAYERS_PC *, i32);
    void Customiser_ResetModelTextureIDs(CUSTOMISER_LAYERS_PC *);

    void Customiser_InitNames(CUSTOMISER_LAYERS_PC *);
    void Customiser_AddPartAccessories(CUSTOMISER_LAYERS_PC *, GAMEOBJECT *, i32, i32, f32);

    CUSTOMISER_LAYERS_PC *Customiser_Configure(char *, VARIPTR *, VARIPTR *, i32, i32, i32 (*)(CUSTOMPIECE_LAYERS_PC *),
                                               void (*)(CUSTOMPIECE_LAYERS_PC *, NUFPAR_PC *), i32 (*)(char *), void *,
                                               i16 *);
    void Customiser_CopyDefaultPiecesToSave(CUSTOMISER_LAYERS_PC *, void *);
    void Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *, APICHARACTERMODELLIST *);
    void Customiser_Set100PercentPieces(CUSTOMISER_LAYERS_PC *);
    CUSTOMPIECE_LAYERS_PC *Customiser_FindPieceByName(CUSTOMISER_LAYERS_PC *, char *, i32 *, i32 *);
    void Customiser_LoadAll(CUSTOMISER_LAYERS_PC *, WORLDINFO *);
    void Customiser_TransformToPanel(CUSTOMISER_LAYERS_PC *);
    i32 Customiser_NextPieceLeft(CUSTOMISER_LAYERS_PC *, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
