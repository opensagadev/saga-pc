#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/numath/nuvec.h"

struct eduiitem_s;
struct eduimenu_s;
struct numtx_s;

typedef struct RTL_DATA {                    // size 0x144
    struct RTL_LIGHT *directional_lights[3]; // 0x000
    f32 directional_strengths[3];            // 0x00c
    struct RTL_LIGHT *ambient_lights[3];     // 0x018
    f32 ambient_strengths[3];                // 0x024
    struct RTL_LIGHT *anti_lights[3];        // 0x030
    f32 anti_strengths[3];                   // 0x03c
    i32 anti_light_count;                    // 0x048
    struct RTL_LIGHT *cached_light;          // 0x04c
    NUVEC shadow_direction;                  // 0x050
    f32 cached_value;                        // 0x05c
    NUVEC previous_shadow_direction;         // 0x060
    f32 previous_shadow_value;               // 0x06c
    f32 shadow_blend;                        // 0x070
    u16 cached_light_uid;                    // 0x074
    u16 reserved_76;                          // 0x076
    NUVEC intensity_vectors[3];              // 0x078
    NUVEC direction[3];                      // 0x09c
    NUVEC ambient;                           // 0x0c0
    u32 reserved_cc[0x15];                    // 0x0cc
    f32 light_scale;                         // 0x120
    NUVEC blended_shadow_direction;          // 0x124
    f32 blended_shadow_value;                // 0x130
    NUVEC specular_direction;                // 0x134
    f32 specular_value;                      // 0x140
} RTL_DATA;
#ifdef __cplusplus
extern "C" {
#endif

    /* Private-register helpers remain local; no public stack prototypes. */
/* RTL_LIGHT.type; 0 is an unused slot. The light editor's Light Type menu names the others. */
#define RTL_LIGHT_AMBIENT 1
#define RTL_LIGHT_POINT 2
#define RTL_LIGHT_POINT_FLICKER 3
#define RTL_LIGHT_DIRECTIONAL 4
#define RTL_LIGHT_CAMDIR 5
#define RTL_LIGHT_POINT_BLEND 6
#define RTL_LIGHT_ANTILIGHT 7
#define RTL_LIGHT_JON_FLICKER 8

    typedef struct RTL_LIGHT {  // size 0x8c
        NUVEC position;         // 0x00
        NUVEC direction;        // 0x0c
        NUVEC ambient;          // 0x18
        NUVEC colour;           // 0x24
        NUVEC secondary_colour; // 0x30
        f32 inner_radius;       // 0x3c
        f32 outer_radius;       // 0x40
        f32 parameters[4];      // 0x44
        f32 phase_timer;        // 0x54
        u8 type;                // 0x58

        union { // size 0x1
            u8 flags;

            struct {                   // size 0x1
                u8 disabled : 1;       // 0x59:0
                u8 cast_shadow : 1;    // 0x59:1
                u8 has_specular : 1;   // 0x59:2
                u8 reserved_flags : 5; // 0x59:3
            };
        }; // 0x59

        i16 pitch;                                                               // 0x5a
        i16 yaw;                                                                 // 0x5c
        i16 association_mask;                                                    // 0x5e
        i16 exclusion_mask;                                                      // 0x60
        u8 reserved_62[2];                                                        // 0x62
        f32 blend_period;                                                        // 0x64
        u16 user_id;                                                             // 0x68
        i16 uid;                                                                 // 0x6a
        f32 intensity;                                                           // 0x6c
        f32 blend_rate;                                                          // 0x70
        f32 blend;                                                               // 0x74
        u8 group_id;                                                             // 0x78
        i8 next_index;                                                           // 0x79
        i8 previous_index;                                                       // 0x7a
        i8 modifier;                                                             // 0x7b
        struct RTL_LIGHT *chain_lights;                                          // 0x7c
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */ // 0x80
        u8 reserved_82[10];                                                       // 0x82
    } RTL_LIGHT;

/* RTL_LIGHT.flags, the bitfields */
#define RTL_LIGHT_FLAG_DISABLED 0x1
#define RTL_LIGHT_FLAG_CAST_SHADOW 0x2
#define RTL_LIGHT_FLAG_HAS_SPECULAR 0x4

    typedef struct rtl_fog_migration_s {                                         // size 0x4c
        f32 start;                                                               // 0x00
        f32 end;                                                                 // 0x04
        u32 colour;                                                              // 0x08
        i32 low_quality_density;                                                 // 0x0c
        u32 low_quality_colour;                                                  // 0x10
        i32 type;                                                                // 0x14
        u8 depth_of_field_fstop;                                                 // 0x18
        u8 reserved_19[3];                                                        // 0x19
        f32 radius;                                                              // 0x1c
        NUVEC position;                                                          // 0x20
        f32 density;                                                             // 0x2c
        f32 start_psp;                                                           // 0x30
        f32 end_psp;                                                             // 0x34
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */ // 0x38
        u8 reserved_3a[2];                                                        // 0x3a
        f32 density_wii;                                                         // 0x3c
        u8 reserved_40[0xc];                                                      // 0x40
    } RTL_FOG_MIGRATION;

    typedef struct RTL_SET {       // size 0x4f84
        u32 header;                // 0x0000
        RTL_LIGHT lights[128];     // 0x0004
        RTL_FOG_MIGRATION fog[32]; // 0x4604
    } RTL_SET;

    RTL_FOG_MIGRATION *rtlGetFogSet(RTL_SET *set, NUVEC *position);

    typedef struct RTL_INDEX { // size 0x28
        i32 enabled;           // 0x00
        u8 reserved_04[8];      // 0x04
        i32 width;             // 0x0c
        i32 depth;             // 0x10
        f32 scale;             // 0x14
        f32 offset_x;          // 0x18
        f32 offset_z;          // 0x1c
        i8 **cells;            // 0x20
        u8 reserved_24[4];      // 0x24
    } RTL_INDEX;

    /* The editor keeps a set's light index directly behind it: the
     * RTL_INDEX header, then IndexLights' cell table and index lists, in
     * one 0x2028-byte area (0x6fac bytes with the set). rtlLoadSet still advances by sizeof(RTL_SET)
     * only; the game builds the index from its own buffer. */
#define RTL_SET_INDEX(set) ((RTL_INDEX *)((set) + 1))
#define RTL_SET_INDEX_AREA_SIZE 0x2028
#define RTL_EDITOR_SET_SIZE (sizeof(RTL_SET) + RTL_SET_INDEX_AREA_SIZE)

    struct RTL_DATA;
    struct RTL_INDEX;
    void rtlResetEx(struct RTL_DATA *, i32);
    i32 rtlDynamicMasterEnable(i32);
    void rtlSetLights(struct RTL_DATA *);
    void rtlApplySetScale(void *, struct RTL_DATA *, NUVEC *, struct numtx_s *, i32, f32);
    extern i32 rtl_dynamic_lights_enabled;
    extern i32 rtl_error;
    extern f32 rtl_frametime;
    extern f32 rtl_shadow_blend_rate;
    extern f32 rtlwob;
    extern NUVEC rtl_shadow_flicker;
    extern NUVEC RtlUnitDirectionYPc;
    extern NUVEC RtlUnitDirectionZPc;
    extern f32 *modifiers;
    extern i32 rtltimer1;
    extern f32 rtltimer1adv;
    void rtlFrameUpdate(f32 frame_time);
    extern struct RTL_INDEX *dbg_rtlindex;
    extern const char RtlSearchTimebarName[];
    i32 rtlDynamicEnable(i32 id, i32 enabled);
    extern NULSTHDR *rtl_dynamic_pool;
    extern i32 rtl_dynamic_max;
    i32 rtlDynamicAlloc(void);
    i32 rtlFindByUserId(RTL_SET *, i32);
    void rtlDynamicFree(i32);
    i32 rtlDynamicAllocTemplate(RTL_SET *, i32);
    i32 rtlDynamicSetType(i32, i32);
    i32 rtlDynamicSetPos(i32 id, NUVEC *position);
    i32 rtlDynamicSetRadii(i32, f32, f32);
    i32 rtlDynamicSetColours(i32, NUVEC *, NUVEC *);
    i32 rtlDynamicSetDirection(i32, NUVEC *);
    void IndexLights(RTL_SET *, VARIPTR *, i32);
    void rtlProcessLights(void *, f32);
    void rtlSwapSetEndianess(void *);
    RTL_SET *rtlLoadSet(char *, VARIPTR *, i32);
    i32 rtlInitDynamic(VARIPTR *, VARIPTR, i32);

    typedef struct BURN_SET {            // size 0x564
        BURN_PARAMETERS parameters;      // 0x000
        BURN_PARAMETERS parameters_copy; // 0x054
        i32 overshooting_up;             // 0x0a8
        i32 overshooting_down;           // 0x0ac
        i32 settling;                    // 0x0b0
        i32 snap_next;                   // 0x0b4
        f32 normal_rate;                 // 0x0b8
        f32 overshoot_rate;              // 0x0bc
        f32 overshoot_cutin;             // 0x0c0
        f32 overshoot_amount;            // 0x0c4
        f32 overbright_cap;              // 0x0c8
        f32 overdark_cap;                // 0x0cc
        BURN_OUT burnouts[32];           // 0x0d0
        i32 active_count;                // 0x550
        i32 selected_index;              // 0x554
        f32 default_radius;              // 0x558
        f32 default_falloff;             // 0x55c
        i32 loaded;                      // 0x560
    } BURN_SET;

    void edrtlInitBurnset(BURN_SET *);
    void edrtlDetermineNearestBurn(f32, BURN_SET *);
    i32 edrtlBurnoutLoadSet(char *, BURN_SET *);
    BURN_SET *edrtlBurnoutLoad(char *, VARIPTR *, i32);
    void edrtlCalculateBurnout(BURN_SET *, f32 *, f32 *, f32 *, NUVEC *, f32);
    void edrtlCalculateBurnoutEx(BURN_SET *, BURN_PARAMETERS *, NUVEC *, f32);
    void edrtlDrawLightEx(i32 index, i32 style);
    i32 rtlGetDirection(RTL_SET *, i32, NUVEC **);
    f32 rtlSpecularValue(RTL_DATA *);
    void rtlSetSpecularValue(RTL_DATA *, f32);
    void rtlSetSpecularLight(RTL_DATA *);
    extern RTL_LIGHT *rtl_locked;
    extern RTL_SET *curr_set;
    extern char **modifier_names;
    extern f32 def_fr;
    extern f32 min_r;
    extern i32 modifier_cnt;
    extern i32 rtl_helper_radius_mode;
    void fogFree(RTL_FOG_MIGRATION *fog);
    void rtlFree(RTL_LIGHT *light);
    void cbModifierAdjust(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    void RtlUiCreateFailed(void);
#ifdef __cplusplus
}
#endif

typedef struct GAME_LIGHT_VIEW {
    u8 reserved_00[0x285];
    i8 floor_surface;
    u8 reserved_286[0x4d8 - 0x286];
    RTL_DATA light_data;
    OBJECT_LIGHT_STATE_GAME lighting_state;
} GAME_LIGHT_VIEW;
