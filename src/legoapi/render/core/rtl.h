#ifndef SAGA_LEGOAPI_RENDER_CORE_RTL_H
#define SAGA_LEGOAPI_RENDER_CORE_RTL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/render/fx/game_deb.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/render/fx/game_deb.h"
#endif

typedef struct RTL_DATA_PC {
    struct RTL_LIGHT_PC *directional_lights[3];
    f32 directional_strengths[3];
    struct RTL_LIGHT_PC *ambient_lights[3];
    f32 ambient_strengths[3];
    struct RTL_LIGHT_PC *anti_lights[3];
    f32 anti_strengths[3];
    i32 anti_light_count;
    struct RTL_LIGHT_PC *cached_light;
    NUVEC shadow_direction;
    f32 cached_value;
    NUVEC previous_shadow_direction;
    f32 previous_shadow_value;
    f32 shadow_blend;
    u16 cached_light_uid;
    u16 unknown_76;
    NUVEC intensity_vectors[3];
    NUVEC direction[3];
    NUVEC ambient;
    u32 unknown_cc[0x15];
    f32 field_120;
    NUVEC blended_shadow_direction;
    f32 blended_shadow_value;
    NUVEC field_134;
    f32 specular_value;
} RTL_DATA_PC;
#ifdef __cplusplus
extern "C" {
#endif

    /* Private-register helpers remain local; no public stack prototypes. */
    typedef struct RTL_LIGHT_PC {
        NUVEC position;
        NUVEC direction;
        NUVEC ambient;
        NUVEC colour;
        NUVEC secondary_colour;
        f32 inner_radius;
        f32 outer_radius;
        f32 parameters[4];
        f32 parameter_54;
        u8 type;

        union {
            u8 flags;

            struct {
                u8 disabled : 1;
                u8 cast_shadow : 1;
                u8 has_specular : 1;
                u8 reserved_flags : 5;
            };
        };

        i16 pitch;
        i16 yaw;
        i16 field_5e;
        i16 field_60;
        u8 unknown_62[2];
        f32 field_64;
        u16 field_68;
        i16 uid;
        f32 intensity;
        f32 blend_rate;
        f32 blend;
        u8 group_id;
        i8 field_79;
        i8 field_7a;
        i8 field_7b;
        struct RTL_LIGHT_PC *field_7c;
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */
        u8 unknown_82[10];
    } RTL_LIGHT_PC;

    STATIC_ASSERT(sizeof(RTL_LIGHT_PC) == 0x8c);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, colour) == 0x24);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, secondary_colour) == 0x30);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, parameters) == 0x44);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, parameter_54) == 0x54);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_64) == 0x64);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_68) == 0x68);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, blend_rate) == 0x70);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, blend) == 0x74);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, group_id) == 0x78);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7c) == 0x7c);

    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, position) == 0x0);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, direction) == 0xc);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, ambient) == 0x18);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, inner_radius) == 0x3c);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, outer_radius) == 0x40);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, flags) == 0x59);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, pitch) == 0x5a);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, yaw) == 0x5c);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_60) == 0x60);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_79) == 0x79);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7b) == 0x7b);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, type) == 0x58);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_5e) == 0x5e);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, uid) == 0x6a);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, intensity) == 0x6c);
    STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7a) == 0x7a);
    STATIC_ASSERT(sizeof(RTL_DATA_PC) == 0x144);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, directional_lights) == 0x0);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, directional_strengths) == 0xc);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, ambient_lights) == 0x18);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, ambient_strengths) == 0x24);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, anti_lights) == 0x30);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, anti_strengths) == 0x3c);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, shadow_direction) == 0x50);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, cached_value) == 0x5c);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, previous_shadow_direction) == 0x60);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, previous_shadow_value) == 0x6c);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, shadow_blend) == 0x70);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, blended_shadow_direction) == 0x124);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, blended_shadow_value) == 0x130);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, field_134) == 0x134);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, anti_light_count) == 0x48);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, cached_light) == 0x4c);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, cached_light_uid) == 0x74);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, intensity_vectors) == 0x78);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, direction) == 0x9c);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, ambient) == 0xc0);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, field_120) == 0x120);
    STATIC_ASSERT(offsetof(RTL_DATA_PC, specular_value) == 0x140);

    typedef struct RTL_SET_PC {
        u32 header;
        RTL_LIGHT_PC lights[128];
        u8 fog[0x980];
    } RTL_SET_PC;

    STATIC_ASSERT(sizeof(RTL_SET_PC) == 0x4f84);

    typedef struct rtl_fog_migration_pc_s {
        f32 start;
        f32 end;
        u32 colour;
        i32 low_quality_density;
        u32 low_quality_colour;
        i32 type;
        u8 depth_of_field_fstop;
        u8 unknown_19[3];
        f32 radius;
        NUVEC position;
        f32 density;
        f32 start_psp;
        f32 end_psp;
        i16 led_file; /* Placeable::CurrentLedFile when the editor creates it */
        u8 unknown_3a[2];
        f32 density_wii;
        u8 unknown_40[0xc];
    } RTL_FOG_MIGRATION_PC;

    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, start) == 0);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, end) == 4);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, depth_of_field_fstop) == 0x18);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, start_psp) == 0x30);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, end_psp) == 0x34);
    STATIC_ASSERT(sizeof(RTL_FOG_MIGRATION_PC) == 0x4c);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, colour) == 8);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, low_quality_density) == 0xc);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, low_quality_colour) == 0x10);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, type) == 0x14);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, radius) == 0x1c);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, position) == 0x20);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, density) == 0x2c);
    STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, density_wii) == 0x3c);
    STATIC_ASSERT(offsetof(RTL_SET_PC, header) == 0);
    STATIC_ASSERT(offsetof(RTL_SET_PC, lights) == 4);
    STATIC_ASSERT(offsetof(RTL_SET_PC, fog) == 0x4604);
    RTL_FOG_MIGRATION_PC *rtlGetFogSet(RTL_SET_PC *set, NUVEC *position);

    typedef struct RTL_INDEX_PC {
        i32 enabled;
        u8 unknown_04[8];
        i32 width;
        i32 depth;
        f32 scale;
        f32 offset_x;
        f32 offset_z;
        i8 **cells;
        u8 unknown_24[4];
    } RTL_INDEX_PC;

    STATIC_ASSERT(sizeof(RTL_INDEX_PC) == 0x28);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, enabled) == 0);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, width) == 0xc);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, depth) == 0x10);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, scale) == 0x14);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, offset_x) == 0x18);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, offset_z) == 0x1c);
    STATIC_ASSERT(offsetof(RTL_INDEX_PC, cells) == 0x20);
    /* The editor keeps a set's light index directly behind it: the
     * RTL_INDEX_PC header, then IndexLights' cell table and index lists, in
     * one 0x2028-byte area (0x6fac bytes with the set). rtlLoadSet still advances by sizeof(RTL_SET_PC)
     * only; the game builds the index from its own buffer. */
#define RTL_SET_INDEX_PC(set) ((RTL_INDEX_PC *)((set) + 1))
#define RTL_SET_INDEX_AREA_SIZE_PC 0x2028
#define RTL_EDITOR_SET_SIZE_PC (sizeof(RTL_SET_PC) + RTL_SET_INDEX_AREA_SIZE_PC)

    struct RTL_DATA_PC;
    struct RTL_INDEX_PC;
    void rtlResetEx(struct RTL_DATA_PC *, i32);
    i32 rtlDynamicMasterEnable(i32);
    void rtlSetLights(struct RTL_DATA_PC *);
    void rtlApplySetScale(void *, struct RTL_DATA_PC *, NUVEC *, NUMTX *, i32, f32);
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
    extern struct RTL_INDEX_PC *dbg_rtlindex;
    extern const char RtlSearchTimebarNamePc[];
    i32 rtlDynamicEnable(i32 id, i32 enabled);
    extern NULSTHDR *rtl_dynamic_pool;
    extern i32 rtl_dynamic_max;
    i32 rtlDynamicAlloc(void);
    i32 rtlFindByUserId(RTL_SET_PC *, i32);
    void rtlDynamicFree(i32);
    i32 rtlDynamicAllocTemplate(RTL_SET_PC *, i32);
    i32 rtlDynamicSetType(i32, i32);
    i32 rtlDynamicSetPos(i32 id, NUVEC *position);
    i32 rtlDynamicSetRadii(i32, f32, f32);
    i32 rtlDynamicSetColours(i32, NUVEC *, NUVEC *);
    i32 rtlDynamicSetDirection(i32, NUVEC *);
    void IndexLights(RTL_SET_PC *, VARIPTR *, i32);
    void rtlProcessLights(void *, f32);
    void rtlSwapSetEndianess(void *);
    RTL_SET_PC *rtlLoadSet(char *, VARIPTR *, i32);
    i32 rtlInitDynamic(VARIPTR *, VARIPTR, i32);

    typedef struct BURN_SET_PC BURN_SET_PC;

    void edrtlInitBurnset(BURN_SET_PC *);
    void edrtlDetermineNearestBurn(f32, BURN_SET_PC *);
    i32 edrtlBurnoutLoadSet(char *, BURN_SET_PC *);
    BURN_SET_PC *edrtlBurnoutLoad(char *, VARIPTR *, i32);
    void edrtlCalculateBurnout(BURN_SET_PC *, f32 *, f32 *, f32 *, NUVEC *, f32);
    void edrtlCalculateBurnoutEx(BURN_SET_PC *, BURN_PARAMETERS_PC *, NUVEC *, f32);
    void edrtlDrawLightEx(i32 index, i32 style);
    i32 rtlGetDirection(RTL_SET_PC *, i32, NUVEC **);
    f32 rtlSpecularValue(RTL_DATA_PC *);
    void rtlSetSpecularValue(RTL_DATA_PC *, f32);
    void rtlSetSpecularLight(RTL_DATA_PC *);
#ifdef __cplusplus
}
#endif

#endif
