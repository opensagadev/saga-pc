#pragma once

#include "nu2api/nucore/common.h"

struct nudisplay_list_s;
struct numtl_s;
struct numtx_s;
struct nurndrlibstate_s;
struct nuhspecial_s;

/* NUDISPLAYLISTITEM.type, named after the text DisplayListPrintItem prints for each. */
#define NUDLIST_MTL 0x80
#define NUDLIST_TXTRLOAD 0x81
#define NUDLIST_GEOMCALL 0x82
#define NUDLIST_MTXLOAD 0x83
#define NUDLIST_TERMINATE 0x84
#define NUDLIST_MTL_CLIP 0x85
#define NUDLIST_CLUT 0x86
#define NUDLIST_DUMMY 0x87
#define NUDLIST_DLIST 0x88
#define NUDLIST_DYNAMIC_GEOMETRY 0x8b
#define NUDLIST_TRANSFORM_REF 0x8c
#define NUDLIST_NEXT 0x8d
#define NUDLIST_END_ITEM 0x8e
#define NUDLIST_FACEON 0x8f
#define NUDLIST_FACEON_TRANSFORM 0x90
#define NUDLIST_GEOMCALL2D 0x93
#define NUDLIST_LIGHTS 0x94
#define NUDLIST_DEBUG_GEOMCALL 0x95
#define NUDLIST_LINECALL2D 0x97
#define NUDLIST_CAMERA 0x9a
#define NUDLIST_FX_SCREEN_COPY 0x9b
#define NUDLIST_FX_SUN_FLARE 0x9c
#define NUDLIST_FX_SCREEN_FILTER 0x9d
#define NUDLIST_FX_SPEED_BLUR 0x9e
#define NUDLIST_LINEBUFFER 0x9f
#define NUDLIST_GEOM_CLIPPED 0xa0
#define NUDLIST_INST_SURF_GEOM 0xad
#define NUDLIST_LIGHTMAP 0xb0
/* Render state packets DisplayListUpdateRenderState links, named after the packet cache each carries. */
#define NUDLIST_KONST 0xa5
#define NUDLIST_FOG 0xa6
#define NUDLIST_VERTEX_GROUPS 0xa9
#define NUDLIST_REFLECTION 0xab

/* NUDISPLAYLISTITEM.id, what NuDisplayListExecute does with the item. */
#define NUDLISTITEM_SKIP 0 /* step to the next item */
#define NUDLISTITEM_JUMP 1 /* continue at the item `next` points to */
#define NUDLISTITEM_CALL 3 /* call the handler of `type` with `next`, then step on */
#define NUDLISTITEM_END 4  /* stop */

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
    void NuDisplayListDrawRenderScenes(void);
    void NuDisplayListDrawItems(NUDISPLAYLISTITEM *);
    i32 DisplayListDebugPS(struct nudisplaylistitem_s *item, char *text);
    void DisplayListPrintItemPS(struct nudisplaylistitem_s *item, i32 file_handle);
    void NuHtmlWrite(char *text, ...);
#ifdef __cplusplus
}
#endif

typedef struct nudisplaylist_s {         // size 0x44
    struct nudisplay_list_s *scene;      // 0x00
    i32 material_id;                     // 0x04
    struct nurndrlibstate_s *state;      // 0x08
    NUDISPLAYLISTITEM *mtl_item;         // 0x0c
    NUDISPLAYLISTITEM *first;            // 0x10
    NUDISPLAYLISTITEM *mtl_last;         // 0x14
    NUDISPLAYLISTITEM *dynamic_geometry; // 0x18
    u32 texture_slots;                   // 0x1c
    u32 bitfield;                        // 0x20
    NUDISPLAYLISTITEM *items;            // 0x24
    NUDISPLAYLISTITEM *scene_first[2];   // 0x28
    NUDISPLAYLISTITEM *scene_next;       // 0x30
    i32 scene_buffer;                    // 0x34
    i32 scene_item_count;                // 0x38
    u8 unknown_003c[8];                  // 0x3c
} NUDISPLAYLIST;

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuDisplayListRndrSpecial(struct nuhspecial_s *, struct numtx_s *, i32, struct numtx_s *, void *);
    VARIPTR *NuDisplayListLinkItems(NUDISPLAYLIST *, i32);
    void *DisplayListCreateGeomTransformPS(VARIPTR *, struct numtx_s *, struct numtl_s *, void *, void *);
    void *DisplayListCreateSkinTransformPS(VARIPTR *, struct numtx_s *, void *, void *, void **, void *);
    void *NuDisplayListPrepareFaceonPS(VARIPTR *, void *, struct numtx_s *);
    VARIPTR *NuDisplayListLinkItem(NUDISPLAYLIST *list, u8 type, void *target);
    void NuDisplayListLinkMtl(NUDISPLAYLIST *list, struct numtl_s *material);
    void NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListAddLightState(NUDISPLAYLISTITEM *, struct numtl_s *);
    void NuDisplayListUpdateSpecial(struct nuhspecial_s *special);
    void DisplayListUpdateSpecialTransformPS(struct nuhspecial_s *special, struct numtx_s *matrix);
#ifdef __cplusplus
}
#endif

typedef struct nusortpri_s {      // size 0x24
    i32 sort_pri;                 // 0x00
    NUDISPLAYLISTITEM *items;     // 0x04
    struct nusortpri_s *sys_next; // 0x08
    u8 opaque_0c_10[0x4];         // 0x0c
    struct nusortpri_s *self;     // 0x10
    u32 flags;                    // 0x14
    u32 sort_flags;               // 0x18

    struct nudisplay_list_s *display_scene; // 0x1c

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
    void NuHtmlEnd(void);
    void NuHtmlHeading(char *text, ...);
#ifdef __cplusplus
}
#endif
