#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec4.h"

struct NUGSCNROOM_s;
struct NUPORTAL;
struct NUPORTALSPHERE;
struct NUPORTAL_FRUSTUM;
struct nudisplay_list_s;
struct nuinstance_s;
struct numtl_s;
struct nuhspecial_s;
struct nuspecial_legacy_s;
struct nutexdim_s;

/* World bounds of a scene instance: the centre (w the bounding radius) and the half extent. */
typedef struct nugscn_instance_bounds_s { // size 0x20
    NUVEC4 center;                        // 0x00
    NUVEC4 extent;                        // 0x10
} NUGSCN_INSTANCE_BOUNDS;

typedef struct NUGSCN_s {                                      // size 0x20c
    i32 *texture_ids;                                          // 0x000
    i32 texture_count;                                         // 0x004
    struct nutexdim_s **textures;                              // 0x008
    struct numtl_s **materials;                                // 0x00c
    i32 material_count;                                        // 0x010
    i32 num_objects;                                           // 0x014
    struct nugobj_s **objects;                                 // 0x018
    i32 num_instances;                                         // 0x01c
    struct nuinstance_s *instances;                            // 0x020
    i32 special_count;                                         // 0x024
    struct nuspecial_legacy_s *specials;                       // 0x028
    u32 unknown_002c;                                          // 0x02c
    i32 spline_count;                                          // 0x030
    struct nugspline_s *splines;                               // 0x034
    u32 unknown_0038[2];                                       // 0x038
    struct NUGSCN_s **additional_scenes;                       // 0x040
    i32 frame_reset_marker;                                    // 0x044
    i16 num_instance_animations;                               // 0x048
    i16 num_instance_animation_data;                           // 0x04a
    struct nuinstanim_s *instance_animations;                  // 0x04c
    struct numtx_s *instance_animation_matrices;               // 0x050
    void **instance_animation_data;                            // 0x054
    i32 num_instance_ids;                                      // 0x058
    void *instance_ids;                                        // 0x05c
    i32 num_texture_anims;                                     // 0x060
    struct SCENE_INGEST_TEXTURE_ANIMATION *texture_animations; // 0x064
    u16 *texture_anim_ids;                                     // 0x068
    i32 max_portals;                                           // 0x06c
    struct NUPORTAL *portals;                                  // 0x070
    i32 num_rooms;                                             // 0x074
    struct NUGSCNROOM_s *rooms;                                // 0x078
    u32 unknown_007c[8];                                       // 0x07c
    struct NUPORTAL_FRUSTUM *portal_frusta[16];                // 0x09c
    i32 num_portal_frusta;                                     // 0x0dc
    i32 camera_room;                                           // 0x0e0
    i32 portal_depth;                                          // 0x0e4
    u32 unknown_e8[2];                                         // 0x0e8
    i32 sphere_count;                                          // 0x0f0
    i32 portal_instance_count;                                 // 0x0f4
    struct NUPORTALSPHERE *portal_spheres;                     // 0x0f8
    NUGSCN_INSTANCE_BOUNDS *portal_boxes;                      // 0x0fc
    u32 unknown_100[4];                                        // 0x100
    struct nudisplay_list_s *display_list;                     // 0x110
    u32 unknown_0114[10];                                      // 0x114
    u8 *instance_visibility_flags;                             // 0x13c
    u32 unknown_0140[6];                                       // 0x140
    i32 shader_texture_animation_enabled;                      // 0x158
    u32 unknown_015c[3];                                       // 0x15c
    i32 character_version_selector;                            // 0x168
    u32 unknown_016c[0x19];                                    // 0x16c
    struct SCENE_FIXUP_NATIVE *native_scene;                   // 0x1d0
    u32 unknown_01d4[3];                                       // 0x1d4
    struct nugscn_anim_lookup_s *end_frame_lookup;             // 0x1e0
    u32 unknown_01e4[4];                                       // 0x1e4
    u32 legacy_format;                                         // 0x1f4
    u32 unknown_01f8[5];                                       // 0x1f8
} NUGSCN;

#ifdef __cplusplus
extern "C" {
#endif

    void NuGScnCreatePS(NUGSCN *, VARIPTR *, VARIPTR *);
    i32 NuSpecialFind(NUGSCN *scene, struct nuhspecial_s *handle, char *name, i32 report_missing);
    void NuGScnRemove(NUGSCN *);
    void NuGScnFixupTIDs(NUGSCN *);
    void NuGScnFixupTIDsPS(NUGSCN *);
    i32 NuGScnRestoreTID(NUGSCN *, i32);
    void NuGScnRestoreTIDs(NUGSCN *);
    void NuGScnRestoreTIDsPS(NUGSCN *);
    void NuGScnDestroyPS(NUGSCN *);
    i32 NuGScnFixupTID(NUGSCN *, i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    NUGSCN *NuGScnRead(VARIPTR *, VARIPTR, char *);

    /* Globals and constants. */
    void NuGScnRndr3(NUGSCN *);
    void NuGScnFixupPS(NUGSCN *);
#ifdef __cplusplus
}
#endif
