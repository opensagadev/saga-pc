/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct edanim_param_pc_s {
    i32 instance_id;
    i32 effect_count;
    i32 sound_count;
    i32 control_mode;
    i32 control_switch;
    f32 control_distance;
    f32 field_018;
    char effect_names[8][16];
    i32 effect_ids[8];
    i32 effect_intervals[8];
    i32 effect_flags[8];
    f32 effect_positions[8][3];
    i16 effect_angles[8];
    i16 effect_angle_ranges[8];
    f32 field_17c;
    char sound_names[8][16];
    i32 sound_ids[8];
    i32 sound_flags[8];
    f32 sound_values[8];
    f32 sound_positions[8][3];
    i32 platform_id;
    f32 bounce_impulse;
    f32 bounce_spring;
    f32 bounce_damping;
    i8 page;
    u8 unknown_2d1[3];
} EDANIM_PARAM_PC;

STATIC_ASSERT(sizeof(EDANIM_PARAM_PC) == 0x2d4);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, control_mode) == 0xc);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, control_switch) == 0x10);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, control_distance) == 0x14);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, field_018) == 0x18);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_count) == 4);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_count) == 8);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_ids) == 0x9c);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_intervals) == 0xbc);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_flags) == 0xdc);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_positions) == 0xfc);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_angles) == 0x15c);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, effect_angle_ranges) == 0x16c);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, field_17c) == 0x17c);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_names) == 0x180);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_ids) == 0x200);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_flags) == 0x220);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_values) == 0x240);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, sound_positions) == 0x260);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, page) == 0x2d0);
#ifdef __cplusplus
extern "C" {
#endif
    void edanimParamReset(void);
    void edanimStartAllPages(void);
    extern NUGSCN *edbits_base_scene;
    extern EDANIM_PARAM_PC AnimParams[64];
    extern i32 edanim_page_on[8];
    extern i32 edanim_page_used[8];
    extern NUGSCN *edanim_page_scene[8];
    extern i32 edanim_next_param;
    extern i32 edanim_params_used;
    i32 edanimLoadPage(char *, NUGSCN *);
    extern NUGSCN *edbits_base_scene;
    extern i32 edbits_anim_page;
    extern i32 edanim_particle_mode;
    extern i32 edanim_nearest;
    extern i32 edanim_nearest_param_id;
    extern NUVEC edanim_cam_pos;
    void edanimDetermineNearestAnim(f32);

    i32 edanimLoadPage(char *, NUGSCN *);
    extern void *ed_fnt;
    void edanimDrawCursor(void);
    void edanimRenderParticleEmitters(i32 parameter_index);
    void edanimRenderSoundEmitters(i32 parameter_index);
    extern struct eduimenu_s *edanim_active_menu;
    extern struct eduimenu_s *edanim_bouncy_menu;
    extern struct eduimenu_s *edanim_localsoundtype_menu;
    extern void *edanim_mtl;
    extern i32 edanim_nearest_sound;
    extern struct eduimenu_s *edanim_sound_menu;
    extern i32 edanim_sound_mode;
    extern i32 edanim_sound_type;
    extern struct eduimenu_s *edanim_soundtype_menu;
    extern char edbits_level_save_directory[256];
    extern char edbits_level_save_extension[256];
    extern char edbits_level_save_name[256];
    extern struct eduimenu_s *edanim_localparticle_menu;
    extern struct eduimenu_s *edanim_localparticletype_menu;
    extern struct eduimenu_s *edanim_mctb_menu;
    extern i32 edanim_nearest_particle;
    extern struct eduimenu_s *edanim_particle_menu;
    extern i32 edanim_particle_type;
    extern struct eduimenu_s *edanim_particletype_menu;
    extern struct eduimenu_s *edanim_switchtype_menu;
    extern i32 edanim_emitroty;
    extern i32 edanim_emitrotz;
    extern struct PC_SCENE_MTL_VIEW *edanim_mtl_zoff;
    extern char edbits_general_save_directory[256];
    extern char edbits_general_save_extension[256];
    extern char edbits_general_save_name[256];
    void PlatInstBounce(i32 index, f32 impulse, f32 spring, f32 damping);
    void edanimParticleCreate(struct nuvec_s *position);
    void edanimParticlePlace(i32 particle_index, struct nuvec_s *position);
    void edanimSoundCreate(struct nuvec_s *position);
    void edanimSoundDestroy(i32 parameter_index, i32 sound_index);
    void edanimSoundPlace(i32 sound_index, struct nuvec_s *position);
    extern i32 edanim_cam_ax;
    extern i32 edanim_cam_ay;
    extern struct eduimenu_s *edanim_options_menu;
#ifdef __cplusplus
}
#endif
