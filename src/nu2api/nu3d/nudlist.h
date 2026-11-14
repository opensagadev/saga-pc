#ifndef SAGA_NU2API_NU3D_NUDLIST_H_PART
#define SAGA_NU2API_NU3D_NUDLIST_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUDLIST_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUDLIST_H_STOP)

struct nudisplay_list_pc_s;
struct numtl_s;
struct numtx_s;
struct nurndrlibstate_s;
struct nuspecial_handle_s;

typedef struct nudisplaylist_pc_s NUDISPLAYLIST_PC;

typedef struct nudisplayscene_pc_s {
    u8 unknown_0000[0x58];
    u8 *material_used_front;
    u8 *material_used_back;
    u8 unknown_0060[0x14];

    union {
        u16 flags;
        u32 render_flags;
    };
} NUDISPLAYSCENE_PC;

STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, material_used_front) == 0x58);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, material_used_back) == 0x5c);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, flags) == 0x74);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, render_flags) == 0x74);

typedef struct nudisplaylistitem_s {
    u8 type;
    u8 id;
    u8 unknown_02[2];
    void *next;
    u32 data[2];
} NUDISPLAYLISTITEM;

STATIC_ASSERT(sizeof(NUDISPLAYLISTITEM) == 0x10);
STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, next) == 0x4);
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

struct nudisplaylist_pc_s {
    NUDISPLAYSCENE_PC *scene;
    i32 material_id;
    void *render_state;
    u8 unknown_000c[8];
    void *material_last;
    void *dynamic_geometry;
    u8 unknown_001c[8];
    void *items;
};

STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, scene) == 0x00);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, material_id) == 0x04);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, render_state) == 0x08);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, material_last) == 0x14);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, dynamic_geometry) == 0x18);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, items) == 0x24);

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuDisplayListRndrSpecial(struct nuspecial_handle_s *, struct numtx_s *, i32, struct numtx_s *, void *);
    VARIPTR *NuDisplayListLinkItems(NUDISPLAYLIST_PC *, i32);
    void *DisplayListCreateGeomTransformPS(VARIPTR *, struct numtx_s *, struct numtl_s *, void *, void *);
    void *DisplayListCreateSkinTransformPS(VARIPTR *, struct numtx_s *, void *, void *, void **, void *);
    void *NuDisplayListPrepareFaceonPS(VARIPTR *, void *, struct numtx_s *);
    VARIPTR *NuDisplayListLinkItem(NUDISPLAYLIST_PC *list, u8 type, void *target);
    void NuDisplayListLinkMtl(NUDISPLAYLIST_PC *list, struct numtl_s *material);
    void NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddLightState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListUpdateSpecial(struct nuspecial_handle_s *special);
    void DisplayListUpdateSpecialTransformPS(struct nuspecial_handle_s *special, struct numtx_s *matrix);
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

typedef struct nudisplaylist_s {
    struct nudisplay_list_pc_s *scene;
    i32 material_id;
    struct nurndrlibstate_s *state;
    NUDISPLAYLISTITEM *mtl_item;
    NUDISPLAYLISTITEM *first;
    NUDISPLAYLISTITEM *mtl_last;
    NUDISPLAYLISTITEM *dynamic_geometry;
    u32 texture_slots;
    u32 bitfield;
    struct nudisplaylistitem_s *items;
    NUDISPLAYLISTITEM *scene_first[2];
    NUDISPLAYLISTITEM *scene_next;
    i32 scene_buffer;
} NUDISPLAYLIST;

STATIC_ASSERT(offsetof(NUDISPLAYLIST, scene_first) == 0x28);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, scene_next) == 0x30);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, scene_buffer) == 0x34);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, state) == 0x08);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, scene) == 0);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, material_id) == 4);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, dynamic_geometry) == 0x18);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, mtl_item) == 0x0c);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, first) == 0x10);
STATIC_ASSERT(offsetof(NUDISPLAYLIST, mtl_last) == 0x14);

typedef struct nudisplayscene_s {
    u8 opaque_00_54[0x54];
    NUDISPLAYLIST **dlist_mtls;
    u8 opaque_58_74[0x1c];
    u8 flags;
    u8 opaque_75_78[0x3];
} NUDISPLAYSCENE;

STATIC_ASSERT(offsetof(NUDISPLAYSCENE, dlist_mtls) == 0x54);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE, flags) == 0x74);

typedef struct nusortpri_s {
    i32 sort_pri;
    NUDISPLAYLISTITEM *items;
    struct nusortpri_s *sys_next;
    u8 opaque_0c_14[0x8];
    u32 flags;
    u32 sort_flags;

    union {
        struct nudisplay_list_pc_s *display_scene;
        struct nudisplayscene_s *legacy_display_scene;
    };

    u16 nmtls;
    u16 mtl_first;
} NUSORTPRI;

STATIC_ASSERT(sizeof(NUSORTPRI) == 0x24);
STATIC_ASSERT(offsetof(NUSORTPRI, items) == 0x04);
STATIC_ASSERT(offsetof(NUSORTPRI, sys_next) == 0x08);
STATIC_ASSERT(offsetof(NUSORTPRI, flags) == 0x14);
STATIC_ASSERT(offsetof(NUSORTPRI, sort_flags) == 0x18);
STATIC_ASSERT(offsetof(NUSORTPRI, display_scene) == 0x1c);
STATIC_ASSERT(offsetof(NUSORTPRI, nmtls) == 0x20);
STATIC_ASSERT(offsetof(NUSORTPRI, mtl_first) == 0x22);

typedef struct nudisplaylistrenderscene_s {
    i32 nsort_pris;
    NUSORTPRI **sort_pris;
    NUDISPLAYLISTITEM render_2d_first;
} NUDISPLAYLISTRENDERSCENE;

STATIC_ASSERT(sizeof(NUDISPLAYLISTRENDERSCENE) == 0x18);
STATIC_ASSERT(offsetof(NUDISPLAYLISTRENDERSCENE, render_2d_first) == 0x08);
#ifdef __cplusplus
extern "C" {
#endif
    struct nudisplay_list_pc_s *NuDisplaySceneClone(struct nudisplay_list_pc_s *, VARIPTR *);
    void NuDisplaySceneClonePS(struct nudisplay_list_pc_s *, struct nudisplay_list_pc_s *, VARIPTR *);
    void NuDisplaySceneDestroy(struct nudisplay_list_pc_s *);
    void NuDisplaySceneDestroyPS(struct nudisplay_list_pc_s *);
    void NuDisplaySceneAdd(struct nudisplay_list_pc_s *);
    void NuDisplaySceneAddPS(struct nudisplay_list_pc_s *);
    /* New public original-source routines; original private colour helper remains local. */
    void NuDisplayListCreate(struct nudisplay_list_pc_s *, VARIPTR *, VARIPTR, i32, i32, i32, i32, i32, i32, i32);
    void DisplayListCreatePS(struct nudisplay_list_pc_s *, VARIPTR *, VARIPTR);

    void NuDisplaySceneRndr(struct nudisplay_list_pc_s *);
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
