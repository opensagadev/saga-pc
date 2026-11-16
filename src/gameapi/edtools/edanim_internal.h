#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"

/* edanim_param_s.control_mode: the animation editor's Switch Type menu (edanim_switchtype_menu labels). */
#define EDANIM_CONTROL_NONE 0
#define EDANIM_CONTROL_SWITCH 1
#define EDANIM_CONTROL_SWITCH_ONE_CYCLE 2
#define EDANIM_CONTROL_SWITCH_CONTINUOUS 3
#define EDANIM_CONTROL_PROXIMITY 4
#define EDANIM_CONTROL_PROXIMITY_ONE_CYCLE 5
#define EDANIM_CONTROL_PROXIMITY_CONTINUOUS 6
#define EDANIM_CONTROL_TERRAIN 7
#define EDANIM_CONTROL_TERRAIN_ONE_CYCLE 8
#define EDANIM_CONTROL_TERRAIN_CONTINUOUS 9
#define EDANIM_CONTROL_OVERRIDE_NO_ANIM 10
#define EDANIM_CONTROL_OVERRIDE_PLAY 11
#define EDANIM_CONTROL_OVERRIDE_PLAY_CONTINUOUS 12

typedef struct edanim_param_s { // size 0x2d4
    i32 instance_id;            // 0x000
    i32 effect_count;           // 0x004
    i32 sound_count;            // 0x008
    i32 control_mode;           // 0x00c
    i32 control_switch;         // 0x010
    f32 control_distance;       // 0x014
    f32 switch_delay;           // 0x018
    char effect_names[8][16];   // 0x01c
    i32 effect_ids[8];          // 0x09c
    i32 effect_intervals[8];    // 0x0bc
    i32 effect_flags[8];        // 0x0dc
    f32 effect_positions[8][3]; // 0x0fc
    i16 effect_angles[8];       // 0x15c
    i16 effect_angle_ranges[8]; // 0x16c
    f32 previous_time;          // 0x17c
    char sound_names[8][16];    // 0x180
    i32 sound_ids[8];           // 0x200
    i32 sound_flags[8];         // 0x220
    f32 sound_values[8];        // 0x240
    NUVEC sound_positions[8];   // 0x260
    i32 platform_id;            // 0x2c0
    f32 bounce_impulse;         // 0x2c4
    f32 bounce_spring;          // 0x2c8
    f32 bounce_damping;         // 0x2cc
    i8 page;                    // 0x2d0
    u8 unknown_2d1[3];          // 0x2d1
} EDANIM_PARAM;

#ifdef __cplusplus
extern "C" {
#endif
    void edanimParamReset(void);
    void edanimStartAllPages(void);
    extern NUGSCN *edbits_base_scene;
    extern EDANIM_PARAM AnimParams[64];
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
    extern struct vufnt_s *ed_fnt;
    void edanimDrawCursor(void);
    void edanimRenderParticleEmitters(i32 parameter_index);
    void edanimRenderSoundEmitters(i32 parameter_index);
    extern struct eduimenu_s *edanim_active_menu;
    extern struct eduimenu_s *edanim_bouncy_menu;
    extern struct eduimenu_s *edanim_localsoundtype_menu;
    extern struct numtl_s *edanim_mtl;
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
    extern struct numtl_s *edanim_mtl_zoff;
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
    void edanimParticleDestroy(i32 parameter_index, i32 particle_index);
#ifdef __cplusplus
}
#endif
