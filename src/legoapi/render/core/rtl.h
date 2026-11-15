/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nulst.h"
#include "nu2api/numath/nuvec.h"
#endif

struct BURN_PARAMETERS;
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
    u16 unknown_76;                          // 0x076
    NUVEC intensity_vectors[3];              // 0x078
    NUVEC direction[3];                      // 0x09c
    NUVEC ambient;                           // 0x0c0
    u32 unknown_cc[0x15];                    // 0x0cc
    f32 unknown_120;                         // 0x120
    NUVEC blended_shadow_direction;          // 0x124
    f32 blended_shadow_value;                // 0x130
    NUVEC unknown_134;                       // 0x134
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
        f32 parameter_54;       // 0x54
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
        i16 unknown_5e;                                                          // 0x5e
        i16 unknown_60;                                                          // 0x60
        u8 unknown_62[2];                                                        // 0x62
        f32 unknown_64;                                                          // 0x64
        u16 unknown_68;                                                          // 0x68
        i16 uid;                                                                 // 0x6a
        f32 intensity;                                                           // 0x6c
        f32 blend_rate;                                                          // 0x70
        f32 blend;                                                               // 0x74
        u8 group_id;                                                             // 0x78
        i8 unknown_79;                                                           // 0x79
        i8 unknown_7a;                                                           // 0x7a
        i8 unknown_7b;                                                           // 0x7b
        struct RTL_LIGHT *unknown_7c;                                            // 0x7c
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */ // 0x80
        u8 unknown_82[10];                                                       // 0x82
    } RTL_LIGHT;

    typedef struct RTL_SET {   // size 0x4f84
        u32 header;            // 0x0000
        RTL_LIGHT lights[128]; // 0x0004
        u8 fog[0x980];         // 0x4604
    } RTL_SET;

    typedef struct rtl_fog_migration_s {                                         // size 0x4c
        f32 start;                                                               // 0x00
        f32 end;                                                                 // 0x04
        u32 colour;                                                              // 0x08
        i32 low_quality_density;                                                 // 0x0c
        u32 low_quality_colour;                                                  // 0x10
        i32 type;                                                                // 0x14
        u8 depth_of_field_fstop;                                                 // 0x18
        u8 unknown_19[3];                                                        // 0x19
        f32 radius;                                                              // 0x1c
        NUVEC position;                                                          // 0x20
        f32 density;                                                             // 0x2c
        f32 start_psp;                                                           // 0x30
        f32 end_psp;                                                             // 0x34
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */ // 0x38
        u8 unknown_3a[2];                                                        // 0x3a
        f32 density_wii;                                                         // 0x3c
        u8 unknown_40[0xc];                                                      // 0x40
    } RTL_FOG_MIGRATION_PC;

    RTL_FOG_MIGRATION_PC *rtlGetFogSet(RTL_SET *set, NUVEC *position);

    typedef struct RTL_INDEX { // size 0x28
        i32 enabled;           // 0x00
        u8 unknown_04[8];      // 0x04
        i32 width;             // 0x0c
        i32 depth;             // 0x10
        f32 scale;             // 0x14
        f32 offset_x;          // 0x18
        f32 offset_z;          // 0x1c
        i8 **cells;            // 0x20
        u8 unknown_24[4];      // 0x24
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

    typedef struct BURN_SET BURN_SET;

    void edrtlInitBurnset(BURN_SET *);
    void edrtlDetermineNearestBurn(f32, BURN_SET *);
    i32 edrtlBurnoutLoadSet(char *, BURN_SET *);
    BURN_SET *edrtlBurnoutLoad(char *, VARIPTR *, i32);
    void edrtlCalculateBurnout(BURN_SET *, f32 *, f32 *, f32 *, NUVEC *, f32);
    void edrtlCalculateBurnoutEx(BURN_SET *, struct BURN_PARAMETERS *, NUVEC *, f32);
    void edrtlDrawLightEx(i32 index, i32 style);
    i32 rtlGetDirection(RTL_SET *, i32, NUVEC **);
    f32 rtlSpecularValue(RTL_DATA *);
    void rtlSetSpecularValue(RTL_DATA *, f32);
    void rtlSetSpecularLight(RTL_DATA *);
#ifdef __cplusplus
}
#endif

typedef struct GAME_LIGHT_VIEW {
    u8 unknown_000[0x285];
    i8 floor_surface;
    u8 unknown_286[0x4d8 - 0x286];
    RTL_DATA light_data;
    OBJECT_LIGHT_STATE_GAME lighting_state;
} GAME_LIGHT_VIEW;
