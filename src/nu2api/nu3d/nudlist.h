/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NU3D_NUDLIST_H_PART
#define SAGA_NU2API_NU3D_NUDLIST_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUDLIST_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUDLIST_H_STOP)

struct nudisplay_list_s;
struct numtl_s;
struct numtx_s;
struct nurndrlibstate_s;
struct nuhspecial_s;

typedef struct nudisplaylist_pc_s NUDISPLAYLIST_PC;

typedef struct nudisplayscene_pc_s { // size 0x78
    u8 unknown_0000[0x58];           // 0x00
    u8 *material_used_front;         // 0x58
    u8 *material_used_back;          // 0x5c
    u8 unknown_0060[0x14];           // 0x60

    union { // size 0x4
        u16 flags;
        u32 render_flags;
    }; // 0x74
} NUDISPLAYSCENE_PC;

typedef struct nudisplaylistitem_s { // size 0x10
    u8 type;                         // 0x0
    u8 id;                           // 0x1
    u8 unknown_02[2];                // 0x2
    void *next;                      // 0x4
    u32 data[2];                     // 0x8
} NUDISPLAYLISTITEM;

#ifdef __cplusplus
extern "C" {
#endif
    void DisplayListSetShadowCasterFlagPS(NUDISPLAYLISTITEM *, NUDISPLAYLISTITEM *, i32);
    void NuDisplayListInit(VARIPTR *cursor, i32 arg2);
    i32 NuDisplayListAddRenderScene(void);
    void DisplayListLinkDynamicMtls(void);
    void DisplayListSetAlphaPS(NUDISPLAYLISTITEM *, NUDISPLAYLISTITEM *, f32);
    void DisplayListSwapBuffersPS(void);
    void NuDisplayListAddClut(NUDISPLAYLISTITEM *, i32);
    void NuDisplayListAddTexture(NUDISPLAYLISTITEM *, i32);
    void NuDisplayListSwapBuffersEndFrame(void);
    void NuDisplayListExecute(NUDISPLAYLISTITEM *, void (**)(void *));
    extern const char PCDynamicDisplaySceneNameText[31];
    void NuDisplayListCaptureBegin(void);
    void NuDisplayListDrawRenderScene(i32);
    void NuDisplayListDrawItems(NUDISPLAYLISTITEM *);
    i32 DisplayListDebugPS(struct nudisplaylistitem_s *item, char *text);
    void DisplayListPrintItemPS(struct nudisplaylistitem_s *item, i32 file_handle);
    void NuHtmlWrite(char *text, ...);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUDLIST_H_PART
#define SAGA_NU2API_NU3D_NUDLIST_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUDLIST_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUDLIST_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUDLIST_H_STOP)

struct nudisplaylist_pc_s {   // size 0x28
    NUDISPLAYSCENE_PC *scene; // 0x00
    i32 material_id;          // 0x04
    void *render_state;       // 0x08
    u8 unknown_000c[8];       // 0x0c
    void *material_last;      // 0x14
    void *dynamic_geometry;   // 0x18
    u8 unknown_001c[8];       // 0x1c
    void *items;              // 0x24
};

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuDisplayListRndrSpecial(struct nuhspecial_s *, struct numtx_s *, i32, struct numtx_s *, void *);
    VARIPTR *NuDisplayListLinkItems(NUDISPLAYLIST_PC *, i32);
    void *DisplayListCreateGeomTransformPS(VARIPTR *, struct numtx_s *, struct numtl_s *, void *, void *);
    void *DisplayListCreateSkinTransformPS(VARIPTR *, struct numtx_s *, void *, void *, void **, void *);
    void *NuDisplayListPrepareFaceonPS(VARIPTR *, void *, struct numtx_s *);
    VARIPTR *NuDisplayListLinkItem(NUDISPLAYLIST_PC *list, u8 type, void *target);
    void NuDisplayListLinkMtl(NUDISPLAYLIST_PC *list, struct numtl_s *material);
    void NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddLightState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListUpdateSpecial(struct nuhspecial_s *special);
    void DisplayListUpdateSpecialTransformPS(struct nuhspecial_s *special, struct numtx_s *matrix);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUDLIST_H_PART
#define SAGA_NU2API_NU3D_NUDLIST_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUDLIST_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUDLIST_H_PART == 3 && !defined(SAGA_NU2API_NU3D_NUDLIST_H_STOP)

typedef struct nudisplaylist_s {         // size 0x38
    struct nudisplay_list_s *scene;      // 0x00
    i32 material_id;                     // 0x04
    struct nurndrlibstate_s *state;      // 0x08
    NUDISPLAYLISTITEM *mtl_item;         // 0x0c
    NUDISPLAYLISTITEM *first;            // 0x10
    NUDISPLAYLISTITEM *mtl_last;         // 0x14
    NUDISPLAYLISTITEM *dynamic_geometry; // 0x18
    u32 texture_slots;                   // 0x1c
    u32 bitfield;                        // 0x20
    struct nudisplaylistitem_s *items;   // 0x24
    NUDISPLAYLISTITEM *scene_first[2];   // 0x28
    NUDISPLAYLISTITEM *scene_next;       // 0x30
    i32 scene_buffer;                    // 0x34
} NUDISPLAYLIST;

typedef struct nudisplayscene_s { // size 0x78
    u8 opaque_00_54[0x54];        // 0x00
    NUDISPLAYLIST **dlist_mtls;   // 0x54
    u8 opaque_58_74[0x1c];        // 0x58
    u8 flags;                     // 0x74
    u8 opaque_75_78[0x3];         // 0x75
} NUDISPLAYSCENE;

typedef struct nusortpri_s {      // size 0x24
    i32 sort_pri;                 // 0x00
    NUDISPLAYLISTITEM *items;     // 0x04
    struct nusortpri_s *sys_next; // 0x08
    u8 opaque_0c_14[0x8];         // 0x0c
    u32 flags;                    // 0x14
    u32 sort_flags;               // 0x18

    union { // size 0x4
        struct nudisplay_list_s *display_scene;
        struct nudisplayscene_s *legacy_display_scene;
    }; // 0x1c

    u16 nmtls;     // 0x20
    u16 mtl_first; // 0x22
} NUSORTPRI;

typedef struct nudisplaylistrenderscene_s { // size 0x18
    i32 nsort_pris;                         // 0x00
    NUSORTPRI **sort_pris;                  // 0x04
    NUDISPLAYLISTITEM render_2d_first;      // 0x08
} NUDISPLAYLISTRENDERSCENE;

#ifdef __cplusplus
extern "C" {
#endif
    struct nudisplay_list_s *NuDisplaySceneClone(struct nudisplay_list_s *, VARIPTR *);
    void NuDisplaySceneClonePS(struct nudisplay_list_s *, struct nudisplay_list_s *, VARIPTR *);
    void NuDisplaySceneDestroy(struct nudisplay_list_s *);
    void NuDisplaySceneDestroyPS(struct nudisplay_list_s *);
    void NuDisplaySceneAdd(struct nudisplay_list_s *);
    void NuDisplaySceneAddPS(struct nudisplay_list_s *);
    /* New public original-source routines; original private colour helper remains local. */
    void NuDisplayListCreate(struct nudisplay_list_s *, VARIPTR *, VARIPTR, i32, i32, i32, i32, i32, i32, i32);
    void DisplayListCreatePS(struct nudisplay_list_s *, VARIPTR *, VARIPTR);

    void NuDisplaySceneRndr(struct nudisplay_list_s *);
    void NuDisplayListBurstRndrSpecial(struct nuhspecial_s *handle, u32 count, struct numtx_s *matrices, i32 clip);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUDLIST_H_PART
#define SAGA_NU2API_NU3D_NUDLIST_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUDLIST_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUDLIST_H_STOP
