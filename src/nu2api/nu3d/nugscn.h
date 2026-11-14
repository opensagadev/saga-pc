/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct NUGSCNROOM_s;
struct NUPORTAL;
struct NUPORTALSPHERE_PC;
struct NUPORTAL_FRUSTUM_PC;
struct nudisplay_list_pc_s;
struct nuinstance_s;
struct numtl_s;
struct nuspecial_handle_s;
struct nuspecial_legacy_pc_s;
struct nutexdim_pc_s;

typedef struct NUGSCN_s {
    i32 *texture_ids;
    i32 texture_count;
    struct nutexdim_pc_s **textures;
    struct numtl_s **materials;
    i32 material_count;
    i32 num_objects;
    struct nugobj_s **objects;
    i32 num_instances;
    struct nuinstance_s *instances;
    i32 special_count;
    struct nuspecial_legacy_pc_s *specials;
    u32 unknown_002c;
    i32 spline_count;
    struct nugspline_s *splines;
    u32 unknown_0038[2];
    struct NUGSCN_s **additional_scenes;
    i32 frame_reset_marker;
    i16 num_instance_animations;
    i16 num_instance_animation_data;
    struct nuinstanim_s *instance_animations;
    struct numtx_s *instance_animation_matrices;
    void **instance_animation_data;
    i32 num_instance_ids;
    void *instance_ids;
    i32 num_texture_anims;
    void *texture_animations;
    u16 *texture_anim_ids;
    i32 max_portals;
    struct NUPORTAL *portals;
    i32 num_rooms;
    struct NUGSCNROOM_s *rooms;
    u32 unknown_007c[8];
    struct NUPORTAL_FRUSTUM_PC *portal_frusta[16];
    i32 num_portal_frusta;
    i32 camera_room;
    i32 portal_depth;
    u32 unknown_e8[2];
    i32 sphere_count;
    i32 portal_instance_count;
    struct NUPORTALSPHERE_PC *portal_spheres;
    void *portal_boxes;
    u32 unknown_100[4];
    struct nudisplay_list_pc_s *display_list;
    u32 unknown_0114[10];
    u8 *instance_visibility_flags;
    u32 unknown_0140[10];
    i32 character_version_selector;
    u32 unknown_016c[0x19];
    void *native_scene;
    u32 unknown_01d4[3];
    struct nugscn_anim_lookup_pc_s *end_frame_lookup;
    u32 unknown_01e4[4];
    u32 legacy_format;
    u32 unknown_01f8[5];
} NUGSCN;

STATIC_ASSERT(offsetof(NUGSCN, objects) == 0x18);
STATIC_ASSERT(offsetof(NUGSCN, frame_reset_marker) == 0x44);
STATIC_ASSERT(offsetof(NUGSCN, portal_depth) == 0xe4);
STATIC_ASSERT(offsetof(NUGSCN, additional_scenes) == 0x40);
STATIC_ASSERT(offsetof(NUGSCN, texture_animations) == 0x64);
STATIC_ASSERT(offsetof(NUGSCN, native_scene) == 0x1d0);
STATIC_ASSERT(offsetof(NUGSCN, texture_ids) == 0x0);
STATIC_ASSERT(offsetof(NUGSCN, texture_count) == 0x4);
STATIC_ASSERT(offsetof(NUGSCN, textures) == 0x8);
STATIC_ASSERT(offsetof(NUGSCN, materials) == 0xc);
STATIC_ASSERT(offsetof(NUGSCN, material_count) == 0x10);
STATIC_ASSERT(offsetof(NUGSCN, special_count) == 0x24);
STATIC_ASSERT(offsetof(NUGSCN, specials) == 0x28);
STATIC_ASSERT(offsetof(NUGSCN, spline_count) == 0x30);
STATIC_ASSERT(offsetof(NUGSCN, splines) == 0x34);
STATIC_ASSERT(offsetof(NUGSCN, instance_animations) == 0x4c);
STATIC_ASSERT(offsetof(NUGSCN, instance_animation_matrices) == 0x50);
STATIC_ASSERT(offsetof(NUGSCN, instance_animation_data) == 0x54);
STATIC_ASSERT(offsetof(NUGSCN, portals) == 0x70);
STATIC_ASSERT(offsetof(NUGSCN, num_rooms) == 0x74);
STATIC_ASSERT(offsetof(NUGSCN, rooms) == 0x78);
STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);
STATIC_ASSERT(offsetof(NUGSCN, max_portals) == 0x6c);
STATIC_ASSERT(offsetof(NUGSCN, portal_frusta) == 0x9c);
STATIC_ASSERT(offsetof(NUGSCN, num_portal_frusta) == 0xdc);
STATIC_ASSERT(offsetof(NUGSCN, camera_room) == 0xe0);
#ifdef __cplusplus
extern "C" {
#endif

    void NuGScnCreatePS(NUGSCN *, VARIPTR *, VARIPTR *);
    i32 NuSpecialFind(NUGSCN *scene, struct nuspecial_handle_s *handle, char *name, i32 flags);
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
STATIC_ASSERT(offsetof(NUGSCN, max_portals) == 0x6c);
STATIC_ASSERT(offsetof(NUGSCN, portal_frusta) == 0x9c);
STATIC_ASSERT(offsetof(NUGSCN, num_portal_frusta) == 0xdc);
STATIC_ASSERT(offsetof(NUGSCN, camera_room) == 0xe0);

#ifdef __cplusplus
extern "C" {
#endif
    NUGSCN *NuGScnRead(VARIPTR *, VARIPTR, char *);

    /* Globals and constants transferred from the Android sources (tools/globals.py). */
    void NuGScnRndr3(NUGSCN *);
#ifdef __cplusplus
}
#endif
