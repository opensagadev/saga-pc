#ifndef SAGA_TRANSITIONAL_UNUSED_H
#define SAGA_TRANSITIONAL_UNUSED_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

#define QRAND_FLOAT() (qrand() * (1.0f / 65535.0f))

/* Types imported from the Android sources (tools/typeimport.py). */
typedef struct AIAREA_s AIAREA_s;
typedef struct AIPATHCNXCONTROLSYS_s AIPATHCNXCONTROLSYS_s;
typedef struct AREA_GLOBAL_VALUES AREA_GLOBAL_VALUES;
typedef struct CHARACTER_SHADOW_s CHARACTER_SHADOW_s;
typedef struct CLIMBOBJECTSYS_s CLIMBOBJECTSYS_s;
typedef struct CLIMBOBJECT_s CLIMBOBJECT_s;
typedef struct CUSTOMPIECE CUSTOMPIECE;
typedef struct CUSTOMPIECECATEGORY CUSTOMPIECECATEGORY;
typedef struct CUTSYS CUTSYS;
typedef struct HINT_s HINT_s;
typedef struct LEVELSCRIPTPROGRESS_s LEVELSCRIPTPROGRESS_s;
typedef struct LEVEL_PROGRESS_DATA_s LEVEL_PROGRESS_DATA_s;
typedef struct LEVEL_PROGRESS_s LEVEL_PROGRESS_s;
typedef struct MINEENTRY_s MINEENTRY_s;
typedef struct MINESYS_s MINESYS_s;
typedef struct NuLegacyInstanceLayout NuLegacyInstanceLayout;
typedef struct NuPlatform NuPlatform;
typedef struct NuSpecialBoundsDisplayLayout NuSpecialBoundsDisplayLayout;
typedef struct NuSpecialLegacyLayout NuSpecialLegacyLayout;
typedef struct NuSpecialLegacyObjectBoundsLayout NuSpecialLegacyObjectBoundsLayout;
typedef struct NuSpecialLegacySceneLayout NuSpecialLegacySceneLayout;
typedef struct PARTLIGHTSOURCE_s PARTLIGHTSOURCE_s;
typedef struct PLAYERCHARACTERCONFIG_s PLAYERCHARACTERCONFIG_s;
typedef struct PLAYERITEM_s PLAYERITEM_s;
typedef struct PLAYERPACKET_s PLAYERPACKET_s;
typedef struct PLAYERSUBOBJ2_s PLAYERSUBOBJ2_s;
typedef struct PLAYERSUBOBJ_s PLAYERSUBOBJ_s;
typedef struct PLUG_s PLUG_s;
typedef struct RUMBLEPACKET RUMBLEPACKET;
typedef struct SCENEPROGRESS_s SCENEPROGRESS_s;
typedef struct SCENESPECIALPROGRESS_s SCENESPECIALPROGRESS_s;
typedef struct SHOPINPUT SHOPINPUT;
typedef struct SOCKSYS SOCKSYS;
typedef struct TintStack TintStack;
typedef struct _vuv_s _vuv_s;
typedef struct instNUGCUTRIGID_s instNUGCUTRIGID_s;
typedef struct nuclipbounds_s nuclipbounds_s;
typedef struct nuclipobject_s nuclipobject_s;
typedef struct nucolour3_s nucolour3_s;
typedef struct rtldata_s rtldata_s;
typedef struct spacelevel_cross_s spacelevel_cross_s;
typedef struct storepack_s storepack_s;

struct spacelevel_cross_s {
    u8 reserved_00[16];
    NUVEC4 local_position;
    u8 reserved_20[16];
    NUVEC4 world_position;
    u16 flags;
    u16 enabled;
    u8 reserved_44[4];
    u32 colour;
    f32 scale;
};

STATIC_ASSERT(sizeof(struct spacelevel_cross_s) == 80);

struct _vuv_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

STATIC_ASSERT(sizeof(struct _vuv_s) == 16);

struct storepack_s {
    char *name;

    union {
        char *product_id;

        struct {
            u8 field1_0x4;
            u8 field2_0x5;
            u8 field3_0x6;
            u8 field4_0x7;
        };
    };

    union {
        i16 message_text_index;

        struct {
            u8 field5_0x8;
            u8 field6_0x9;
        };
    };

    u8 field7_0xa;
    u8 field8_0xb;
    void (*unlock_fn)(void);
    char *floor_target_door_name;
    char *custodian_locator_set;
    f32 offset_floor_target;
    struct DOOR_s *floor_target_door;
    i16 *id;
    NUVEC custodian_position;
    u16 custodian_angle;
    u8 field44_0x32;
    u8 field45_0x33;
};

STATIC_ASSERT(sizeof(struct storepack_s) == 52);

struct PLAYERCHARACTERCONFIG_s {
    u8 unknown_00[48];
    f32 reset_scale;
    u8 unknown_34[24];
    f32 collision_origin_radius;
    u8 unknown_50[60];
    f32 shadow_radius;
    u32 flags_090;
    u32 flags_094;
    u8 unknown_98[88];
    u16 shadow_joint_mask;
    u8 unknown_f2[4];
    u8 blob_shadow_alpha;
    u8 unknown_f7;
    i8 weapon_joints[4];
    i8 weapon_shoot_joints[4];
    u8 unknown_100[8];
    i8 hand_joints[2];
    i8 grapple_joint_a;
    i8 grapple_joint_b;
    u8 unknown_10c;
    i8 collision_origin_joint;
    u8 unknown_10e;
    i8 model_origin_joint;
    u8 unknown_110;
    i8 helmet_locator;

    union {
        u8 unknown_112[5];

        struct {
            i8 batarang_joint;
            u8 unknown_113[4];
        };
    };

    u8 variant;
};

STATIC_ASSERT(sizeof(struct PLAYERCHARACTERCONFIG_s) == 280);

struct CUSTOMPIECE {
    char *name;
    i16 character_id;
    i16 icon_character_id;

    union {
        u8 unknown_08[10];

        struct {
            i16 weapon_model;
            i16 unknown_indices_0a[3];
            i8 field_0x10;
            i8 collection_type;
        };
    };

    union {
        struct {
            u8 layer_flags;
            u8 availability_flags_high;
        };

        u16 availability_flags;
    };

    u32 model_flags;
    u32 gameplay_flags;
    u8 unknown_1c[12];
};

STATIC_ASSERT(sizeof(struct CUSTOMPIECE) == 40);

struct CUSTOMPIECECATEGORY {
    char *name;
    u8 uses_special;
    i8 material_tag;
    u8 pad_06[2];
    char *shared_scene;
};

STATIC_ASSERT(sizeof(struct CUSTOMPIECECATEGORY) == 12);

struct rtldata_s {
    RTL_DATA_PC base;
};

STATIC_ASSERT(sizeof(struct rtldata_s) == 324);

struct CHARACTER_SHADOW_s {
    NUVEC position;
    u16 x_rotation;
    u16 z_rotation;
    f32 opacity;
};

STATIC_ASSERT(sizeof(struct CHARACTER_SHADOW_s) == 20);

struct PLAYERPACKET_s {
    struct CHARACTER_SHADOW_s character_shadows[5];
    u8 pad_064[132];
    u16 context_animation;
    u8 pad_0ea[4];
    u8 context_mode;
    u8 pad_0ef[2];
    i8 build_context;
    u8 pad_0f2[2];
    u8 action_movement_state;
    u8 action_movement_variant;
    u8 pad_0f6[6];
    f32 context_animation_time;
    i8 context_target;
    u8 context_state_flags;
    u8 pad_102[1234];
    NUVEC reset_up_direction;
    struct GAMEPAD_PC *gamepad;
    u8 pad_5e4[32];
    void *field_0x604;
    u8 pad_608[4];
    struct gameobject_s *linked_object;
    u8 pad_610[4];
    u32 field_0x614;
    u32 field_0x618;
    u32 field_0x61c;
    u32 field_0x620;
    u32 field_0x624;
    u32 field_0x628;
    u32 field_0x62c;
    u32 field_0x630;
    u8 pad_634[20];
    u32 field_0x648;
    u8 pad_64c[8];

    union {
        i32 force_glow_mode;
        void *force_glow_object;
    };

    union {
        i32 force_glow_state;
        void *force_glow_candidate;
    };

    u8 pad_65c[4];
    u32 input_state;
    u32 field_0x664;
    u32 field_0x668;
    u8 pad_66c[4];
    u32 field_0x670;
    u32 field_0x674;
    u32 field_0x678;
    f32 context_blend;
    f32 context_distance;
    u8 pad_684[4];
    u32 field_0x688;
    f32 delayed_turn_timer;
    u32 field_0x690;
    u32 field_0x694;
    u8 pad_698[4];
    u32 field_0x69c;
    u32 field_0x6a0;
    u32 field_0x6a4;
    u8 pad_6a8[32];
    u32 field_0x6c8;
    f32 force_glow_x;
    f32 force_glow_y;
    f32 force_glow_intensity;
    u8 pad_6d8[4];
    f32 force_glow_z;
    u32 field_0x6e0;
    u8 pad_6e4[4];
    u32 field_0x6e8;
    f32 ground_height;
    u8 pad_6f0[8];
    u32 field_0x6f8;
    u32 field_0x6fc;
    u8 pad_700[16];
    u32 field_0x710;
    u8 pad_714[4];
    u32 field_0x718;
    u32 field_0x71c;
    u32 field_0x720;
    u32 field_0x724;
    u32 field_0x728;
    u8 pad_72c[4];
    u32 field_0x730;
    u32 field_0x734;
    u8 pad_738[4];
    u32 field_0x73c;
    u8 pad_740[28];
    i16 movement_angle_0;
    i16 movement_angle_1;
    i16 movement_angle_2;
    i16 movement_angle_3;
    i16 movement_lean_angle;
    i16 secondary_lean_angle;
    i16 tertiary_lean_angle;
    i16 force_glow_index;
    u8 movement_flags;
    u8 secondary_flags;
    u8 animation_flags;
    u8 pad_76f;
    u8 render_flags;
    u8 field_0x771;
    u8 pad_772;
    u8 random_state[4];
    u8 pad_777[6];
    u8 field_0x77d;
    u8 field_0x77e;
    u8 pad_77f[13];
    u8 random_context;
    u8 pad_78d;
    i8 surface_type;
    u8 pad_78f[5];
    u32 field_0x794;
};

STATIC_ASSERT(sizeof(struct PLAYERPACKET_s) == 1944);

struct HINT_s {
    i16 control_mode_ids[2];
    u8 flags;
    u8 pad_0x05[3];
    i32 shop_price;
    f32 display_duration;
    f32 repeat_delay;

    union {
        i32 (*availability_fn)(struct HINT_s *arg0);
        u8 pad_0x14[4];
    };

    void (*on_display)(struct HINT_s *arg0);
    u8 completion_flags[4];
    f32 field_0x20;
};

STATIC_ASSERT(sizeof(struct HINT_s) == 36);

struct AREA_GLOBAL_VALUES {
    i32 field_0x00;
    i32 field_0x04;
    i32 field_0x08;
    i32 field_0x0c;
    i32 field_0x10;
    i32 field_0x14;
    i32 field_0x18;
    i32 field_0x1c;
    i32 field_0x20;

    union {
        struct {
            i32 field_0x24;
            i32 field_0x28;
        };

        u32 arcade_player_kills[2];
    };

    union {
        struct {
            i32 field_0x2c;
            i32 field_0x30;
        };

        u32 arcade_ai_kills[2];
    };
};

STATIC_ASSERT(sizeof(struct AREA_GLOBAL_VALUES) == 52);

struct SCENESPECIALPROGRESS_s {
    u32 frame : 11;
    u32 visible : 1;
    u32 playing : 1;
    u32 waiting : 1;
    u32 repeating : 1;
    u32 reserved : 17;
    f32 tfactor;
};

STATIC_ASSERT(sizeof(struct SCENESPECIALPROGRESS_s) == 8);

struct SCENEPROGRESS_s {
    struct SCENESPECIALPROGRESS_s specials[1280];
};

STATIC_ASSERT(sizeof(struct SCENEPROGRESS_s) == 10240);

struct AIAREA_s {
    char name[16];

    union {
        NUVEC position;

        struct {
            f32 min_x;
            f32 min_y;
            f32 min_z;
        };
    };

    union {
        struct {
            f32 half_width;
            f32 height;
            f32 half_depth;
        };

        struct {
            f32 max_x;
            f32 max_y;
            f32 max_z;
        };
    };

    union {
        i16 rotation;
        i16 flags;
    };

    u8 runtime_flags;
    u8 game_flags;
    u8 padding_0x2c[8];
    struct AISYS_PC *system;
    u8 padding_0x38[4];
};

STATIC_ASSERT(sizeof(struct AIAREA_s) == 60);

struct AIPATHCNXCONTROLSYS_s {
    i32 controller_count;
    struct AIPATHCNXCONTROLLER_s *controllers;
    NULISTHDR available_controllers;
    NULISTHDR active_controllers;
};

STATIC_ASSERT(sizeof(struct AIPATHCNXCONTROLSYS_s) == 24);

struct nucolour3_s {
    f32 r;
    f32 g;
    f32 b;
};

STATIC_ASSERT(sizeof(struct nucolour3_s) == 12);

struct TintStack {
    struct nucolour3_s ambient;
};

STATIC_ASSERT(sizeof(struct TintStack) == 12);

struct LEVEL_PROGRESS_DATA_s {
    u32 words[2560];
};

STATIC_ASSERT(sizeof(struct LEVEL_PROGRESS_DATA_s) == 10240);

struct LEVELSCRIPTPROGRESS_s {
    char name[16];
    f32 params[4];
};

STATIC_ASSERT(sizeof(struct LEVELSCRIPTPROGRESS_s) == 32);

struct LEVEL_PROGRESS_s {
    struct LEVEL_PROGRESS_DATA_s data;

    union {
        i32 flags;

        struct {
            u8 flags_low;
            u8 flags_upper[3];
        };
    };

    u32 disabled_ai_object_mask[2];
    f32 grabber_field_0x48c;
    f32 grabber_field_0x484;
    f32 grabber_field_0x494;
    u32 destroyed_trooper_cannon_mask;
    u32 played_cutscene_mask;
    struct LEVELSCRIPTPROGRESS_s scripts[32];
    GIZFLOWPROGRESS_PC giz_flow_progress;
    char disabled_effect_names[12][16];
};

STATIC_ASSERT(sizeof(struct LEVEL_PROGRESS_s) == 11812);

struct RUMBLEPACKET {
    u32 field_0x00;
    f32 rumble_amount;
    u32 field_0x08;
    f32 rumble_time;

    union {
        u32 field_0x10;

        struct {
            u8 active;
            u8 pad_0x11[3];
        };
    };
};

STATIC_ASSERT(sizeof(struct RUMBLEPACKET) == 20);

struct PARTLIGHTSOURCE_s {
    u8 reserved_00[120];
    NUVEC values[7];
};

STATIC_ASSERT(sizeof(struct PARTLIGHTSOURCE_s) == 204);

struct PLAYERITEM_s {
    struct PLAYERITEMTYPE_s *type;
    u8 ammunition;
    u8 field_0x5;
    u8 reserved[2];
};

STATIC_ASSERT(sizeof(struct PLAYERITEM_s) == 8);

struct SHOPINPUT {
    union {
        i32 value[10];

        struct {
            i32 left_pressed;
            i32 right_pressed;
            i32 up_pressed;
            i32 down_pressed;
            i32 left_held;
            i32 right_held;
            i32 up_held;
            i32 down_held;
            i32 confirm;
            i32 cancel;
        };
    };
};

STATIC_ASSERT(sizeof(struct SHOPINPUT) == 40);

struct NuSpecialBoundsDisplayLayout {
    NUMTX mtx;
    NUMTX draw_mtx;
    NUVEC min;
    f32 min_w;
    NUVEC max;
    f32 max_w;
    NUVEC center;
    f32 radius;
    struct nuclipobject_s *clip_objects;
    char *name;
    u32 flags;
    f32 *clip_range;
    i32 instance_ix;
    struct nuinstanim_s *instance_animation;
    i16 wind_speed;
    i16 wind_scale;
    u32 pad_cc;
};

STATIC_ASSERT(sizeof(struct NuSpecialBoundsDisplayLayout) == 208);

struct NuSpecialLegacyObjectBoundsLayout {
    struct NuSpecialLegacyMaterialLink *materials;
    f32 origin_radius;
    u8 pad_08[4];
    NUVEC minimum;
    NUVEC maximum;
    NUVEC center;
    f32 radius;
    u8 pad_34[4];
    struct NuSpecialLegacyObjectBoundsLayout *next;
};

STATIC_ASSERT(sizeof(struct NuSpecialLegacyObjectBoundsLayout) == 60);

struct NuSpecialLegacySceneLayout {
    u8 pad_00[24];
    void **objects;
    i32 instance_count;
    u8 *instances;
};

STATIC_ASSERT(sizeof(struct NuSpecialLegacySceneLayout) == 36);

struct MINEENTRY_s {
    u32 active;
    NUVEC pos;
    f32 grow;
    f32 grow_alpha;
    u16 rotx;
    u16 roty;
};

STATIC_ASSERT(sizeof(struct MINEENTRY_s) == 28);

struct MINESYS_s {
    struct nuhspecial_s mine_special;
    struct MINEENTRY_s mines[64];
    f32 mine_radius;
    f32 spawn_timer;
    void *nomine_areas[10];
    i16 nomine_count;
    i16 mine_debris;
    i32 mine_part;
    f32 update_timer;
};

STATIC_ASSERT(sizeof(struct MINESYS_s) == 1864);

struct SOCKSYS {
    struct sock_pc_s *sock;
    i32 count;
};

STATIC_ASSERT(sizeof(struct SOCKSYS) == 8);

struct PLAYERSUBOBJ2_s {
    char pad_0x00[28];
    f32 value;
};

STATIC_ASSERT(sizeof(struct PLAYERSUBOBJ2_s) == 32);

struct PLAYERSUBOBJ_s {
    char pad_0x00[36];
    void *field_0x24;
};

STATIC_ASSERT(sizeof(struct PLAYERSUBOBJ_s) == 40);

struct instNUGCUTRIGID_s {
    struct NUGSCN_s *scene;
    void *special;
    void *display_special;
    u8 state_index;
    u8 visible;
    u8 pad_0e[2];
};

STATIC_ASSERT(sizeof(struct instNUGCUTRIGID_s) == 16);

struct CUTSYS {
    void **cuts;
    i32 count;
    u32 *character_bits;
};

STATIC_ASSERT(sizeof(struct CUTSYS) == 12);

struct CLIMBOBJECTSYS_s {
    struct CLIMBOBJECT_s *objects;
    u16 count;
    u16 capacity;
};

STATIC_ASSERT(sizeof(struct CLIMBOBJECTSYS_s) == 8);

struct CLIMBOBJECT_s {
    NUVEC normal;
    f32 impact_x;
    f32 impact_z;
    struct AIPATHCNX_PC *connection;
    struct AIPATH_PC *path;
    u8 flags;
    u8 field_0x1d[3];
};

STATIC_ASSERT(sizeof(struct CLIMBOBJECT_s) == 32);

struct NuPlatform {
    char *name;
    char *texture_extension;
    char *font_extension;
    i32 current_platform;
};

STATIC_ASSERT(sizeof(struct NuPlatform) == 16);

struct PLUG_s {
    char name[16];
    NUVEC position;
    u16 x_rotation;
    u16 y_rotation;
    u16 z_rotation;
    u8 enabled;

    union {
        u8 flags;

        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 reserved_flags : 6;
        };
    };

    u8 reserved[16];
};

STATIC_ASSERT(sizeof(struct PLUG_s) == 52);

struct nuclipbounds_s {
    NUVEC center;
    f32 center_w;
    NUVEC extent;
    f32 extent_w;
};

STATIC_ASSERT(sizeof(struct nuclipbounds_s) == 32);

struct nuclipobject_s {
    i32 nmaterials;
    i32 *material_ids;
    i32 *indices;
};

STATIC_ASSERT(sizeof(struct nuclipobject_s) == 12);

struct NuLegacyInstanceLayout {
    u8 pad_00[68];

    union {
        u8 flags;

        struct {
            u8 visible : 1;
            u8 on_screen : 1;
            u8 unused_flag_2 : 1;
            u8 no_visibility_test : 1;
            u8 unused_flags_4_7 : 4;
        };
    };

    u8 pad_45[3];
    struct nuinstanim_s *animation;
};

STATIC_ASSERT(sizeof(struct NuLegacyInstanceLayout) == 76);

struct NuSpecialLegacyLayout {
    u8 pad_00[64];
    void *instance;
    char *name;
    u32 flags;
};

STATIC_ASSERT(sizeof(struct NuSpecialLegacyLayout) == 76);
#ifdef __cplusplus
extern "C" {
#endif
    extern u32 edpp_page_scene[8];
    extern i32 LEGOMENU_PAUSECUT;
    extern struct gameobject_s *pod_pacemaker;
    extern void *gunship_weapons[4];
    extern struct level_data_pc_s *CLOUDCITYTRAPOUTRO_LDATA;
    extern i16 id_DARTHVADER;
    extern void (*LEGO_SET_SLOWDOWNFN)(struct gameobject_s *arg0);
    extern i32 (*GizObstacle_CheckExcludeFlagsFn)(struct gizobstacle_s *arg0, struct gameobject_s *arg1);
    extern i16 trooper_boltid[2];
    extern i32 grab_screen_image;
    extern u8 Reflections_On;
    extern i32 nudlist_debug_level;
    extern i32 NUSOUND_STREAM_3;
    void aieditor_ClearMainMenu(void);
    void aieditor_cbCancelMainMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void aieditor_cbCancelSaveMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void locatorEditor_cbCancelRenameMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void locatorEditor_cbRenameLocator(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void routeEditor_cbCancelRouteUsers(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void routeEditor_cbSetRouteUsers(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    char *edbitsGetSoundName(i32 sound_type);
    u32 FS_EncodedDateKey(char *name);
    void DebrisStatusNormal(i32 *handle);
    extern i16 tCHALLENGECOMPLETE;
    extern i16 tWINNER;
    extern f32 GIZPANEL_PLAYERPOSLIFT;
    void GizTurret_ReadAnimSetData(struct gameanimobj_s *object, u8 version);
    u8 Store_IsPackUnlocked(i32 pack);
    extern struct level_data_pc_s *PODRACEC_LDATA;
    extern char Door_ExitCameraSplineName[32];
    void PodSprint_InitAISpline(struct WORLDINFO_s *world, void *ai, char *name);
    extern i32 max_nsnipers;
    extern struct gameobject_s *gameobj;
    extern i16 id_MILLENNIUMFALCON;
    void GizBuildIt_TurnOff(struct GIZBUILDIT_s *buildit);
    extern f32 ICONSIZE;
    extern u8 MENUEXITB;
    extern u8 MENUEXITG;
    extern u8 MENUEXITR;
    extern u8 MENULOSTA1;
    extern u8 MENULOSTA2;
    extern u8 MENULOSTB1;
    extern u8 MENULOSTB2;
    extern u8 MENULOSTG1;
    extern u8 MENULOSTG2;
    extern u8 MENULOSTR1;
    extern u8 MENULOSTR2;
    extern f32 MENUTEXTSCALE;
    extern i16 tALLLEVELPROGRESSWILLBELOST;
    extern i16 tALLSUPERSTORYPROGRESSWILLBELOST;
    extern i16 tARCADE_NOWIN;
    extern i16 tCANCEL;
    extern i16 tEXITTOMAP;
    extern i16 tEXITWITHOUTSAVING;
    extern i16 tGAMEPROGRESSWILLBELOST;
    extern i16 tNO;
    extern i16 tQUITGAME;
    extern i16 tRESTARTDEMO2;
    extern i16 tRESTARTLEVEL;
    extern i16 tSAVEANDEXIT;
    extern i16 tYES;
    extern i16 id_PROBEDROID;
    extern f32 ForceBackRadius;
    void edpartFinishMenu(struct eduimenu_s *menu);
    extern f32 CameraZoom;
    extern f32 menu_pulse;
    extern i32 smarttextex_drawmessagebox;
    extern i16 tCONTINUESTORY;
    extern i16 tFINALCHAPTERLOCKED;
    extern i16 tFINISHSTORY;
    extern i16 tMAP;
    extern i16 tNEXTCHAPTERLOCKED;
    extern i16 tRESTART;
    extern const f64 ShopCharacterAmplitudePc;
    extern struct level_data_pc_s *DAGOBAHA_LDATA;
    extern struct level_data_pc_s *DAGOBAHD_LDATA;
    extern struct level_data_pc_s *DAGOBAHE_LDATA;
    void NuTexAddReference(i32 tex_id, struct NUGSCN_s *scene);
    extern void (*nudebug_handler)(char *arg0);
    void NuHtmlFlush(i32 force);
    void NuRndrPrimUV(f32 u, f32 v);
    extern i32 (*FindAlternativeSpecialObjectFn)(struct AISYS_PC *arg0, NUSPECIALHANDLE *arg1);
    extern f32 SpaceRumbleTimer;
    void Hub_MarkCharacterUnavailable(i16 *characters, i16 character);
    extern storepack_s StorePack[11];
    extern i16 id_JABBA;
    extern i16 id_TRAININGREMOTE;
    extern i16 id_WEIRDO1;
    extern i16 id_WEIRDO2;
    extern i16 id_WHIP;
    extern i16 id_WOMPRAT;
    extern i16 id_ATST;
    extern i16 id_JEDISTARFIGHTERREDEP3;
    extern i16 id_MINIIMPERIALSHUTTLE;
    extern i16 id_MINIMILLENNIUMFALCON;
    extern i16 id_MINITIEINTERCEPTOR;
    extern i16 id_CLONEARC;
    extern i16 id_IMPERIALSHUTTLE;
    extern i16 id_NABOOSTARFIGHTER;
    extern i16 id_SNOWSPEEDER;
    extern struct level_data_pc_s *TEMPLESTATUS_LDATA;
    i32 ImplodeGetI(void *buf, i32 size);
    void *NuFramebufferGetFrontBuffer(void);
    extern struct level_data_pc_s *HOTHBATTLED_LDATA;
    extern i32 TERRAINMASK_NONDROID;
    u32 ParseAIPathCnxFlag(char *name);
    void *CreatePodRaceMine(struct nuvec_s *pos);
    void HothBattle_AssignSpawnLocator(struct gameobject_s *object, struct AILOCATOR_PC *locator,
                                       struct AILOCATORSET_PC *set);
    extern i16 id_SPEEDERBIKESNOW;
    void eduiMenuEnsureSelection(struct eduimenu_s *menu);
    i32 eduiMenuProcessSelectedItem(struct eduimenu_s *menu, f32 delta_time, struct FRAME_PAD_PC *pad);
    extern u8 hub_makefreeplaylist_addotherid;
    extern i32 hub_new_level;
    void edpartRemoveInstance(struct parttype_lookup_pc_s *type, i32 index);
    extern const char NuFileIopImageNamePc[13];
    void MakePlayCorner(struct gamecamera_s *camera, f32 screen_x, f32 screen_y, struct nuvec_s *near_corner,
                        struct nuvec_s *far_corner);
    void SetPlayPlane(struct KEEP_SCREEN_PLANE_PC *plane, struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *c,
                      struct nuvec_s *d, struct nuvec_s *edge_a0, struct nuvec_s *edge_a1, struct nuvec_s *edge_b0,
                      struct nuvec_s *edge_b1);
    void edcamSetSpeed(f32 position_x, f32 position_y, f32 position_z, f32 distance);
    extern char nullobjectname[16];
    extern GRAPPLE_DYNAMIC_PC DynamicGrapple[4];
    void Grapples_DrawList(struct WORLDINFO_s *world, struct GRAPPLE_DYNAMIC_PC *grapples, i32 grapple_count,
                           u16 target_rotation, f32 target_alpha);
    extern NUVEC edpart_cam_pos;
    extern struct PC_SCENE_MTL_VIEW *SolidMtl3D;
    extern i16 tCHEAT_SELFDESTRUCT;
    void eduiFntPrintEx(void *font, i32 x, i32 y, i32 alignment, char *format, ...);
    void edanimParamDestroy(i32 parameter_index);
    extern i32 g_loadingCharacterInHub;
    extern f32 KITPOS2Y;
    extern i32 LEGOOBJ_CHARKIT;
    extern i32 LEGOOBJ_MINIKIT;
    f32 GetAspectRatio(void);
    extern f32 LastTouchTime;
    extern i32 subitemselected;
    u8 Text_MenuColourLerp(u8 first, u8 second, f32 amount);
    extern i32 PickUpFlickerFrames;
    extern i32 PickUpFlickerTest;
    extern i32 PickupFlickerFrame;
#ifdef __cplusplus
}
#endif

enum { GIZTORPMACHINE_FLAG_ACTIVE = 1 };

enum { GIZTORPMACHINE_FLAG_VISIBLE = 2 };

enum { GIZTORP_LEVEL_OBJECT_ALTERNATE = 291 };

enum { GIZTORP_LEVEL_OBJECT_PRIMARY = 290 };

enum { GIZMOPICKUP_STATE_ALTERNATE_TYPE = 64 };

enum { GIZPANEL_DRAW_FLAG_ALT_MODEL = 1 };

enum { GIZPANEL_DRAW_FLAG_LEGACY = 4 };

enum { GIZPANEL_DRAW_FLAG_NO_SHADOW_RESET = 2 };

enum { GIZPANEL_FLAG_BADDIE = -128 };

enum { GIZPANEL_FLAG_HIDE_BASE = 32 };

enum { GIZPANEL_FLAG_HIDE_TARGET = 64 };

enum { NUINSTANIM_FLAG_PLAYING = 1 };

enum { NUINSTANIM_FLAG_REPEATING = 8 };

enum { GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE = 128 };

enum { GIZBUILDIT_AVAILABILITY_LINKED = 3 };

enum { TECHNO_FLAG_COMPLETE = 8 };

enum { GAMEANIMSET_VISIBILITY_NONE = 0 };

enum { SPINNER_RUNTIME_AT_OUTPUT = 64 };

enum { SPINNER_RUNTIME_AUTO_RETURN = 16 };

enum { SPINNER_RUNTIME_INPUT_DISABLED = 8 };

enum { SPINNER_STATE_ROTATING = 64 };

enum { SPINNER_STATE_STOP_ANIMATION = 32 };

enum { GIZSPINNER_STATE_SHADOW_PLATFORM = 2048 };

enum { APIOBJECT_FLAG_PLAYER_ACTIVE = 128 };

enum { APIOBJECT_STATE_FLAG_IGNORE_DOORS = 262144 };

enum { DOOR_FLAG_ONE_WAY = 1 };

enum { LEVEL_OUTRO = 128 };

enum { RESETBIT_DOOR_TRANSITION = 8 };

enum { GIZMOPICKUP_STATE_COLLECTED = 8 };

enum { GIZFORCE_CONFIG_RESET_STATE_ON_ACTIVATE = 8 };

enum { GIZFORCE_PROGRESS_ENABLED = 1 };

enum { GIZFORCE_RUNTIME_HAS_PLATFORM = 1 };

enum { GIZFORCE_STATE_DESTROYED_OR_THROWN = 1 };

enum { GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS = 64 };

enum { GIZBUILDIT_STATE_REISSUE_REWARD_ON_ACTIVATE = 512 };

enum { AREAFLAG_BONUS_AREA = 4 };

enum { AREAFLAG_VEHICLE_AREA = 1 };

enum { HUB_FREEPLAY_MODEL_MINIKIT = 67108864 };

enum { HUB_FREEPLAY_MODEL_VEHICLE = 8192 };

enum { GIZSPINNER_FLAG_HIDE_ARM = 2 };

enum { GIZSPINNER_FLAG_HIDE_BASE = 4 };

enum { SPINNER_RUNTIME_ANIMATION_HIDDEN = 32 };

enum { GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT = 16 };

enum { GIZBUILDIT_AVAILABILITY_INTERACTING = 4 };

enum { CHARACTER_CONTEXT_DOOMED = 43 };

enum { CHARACTER_CONTEXT_NONE = -1 };

enum { CHARACTER_CONTEXT_SIGNAL = 76 };

enum { GIZFORCE_CONFIG_ALONG_SOCKET = 64 };

enum { GIZFORCE_CONFIG_CAST_SHADOW = 4096 };

enum { GIZFORCE_CONFIG_DRAW_REFLECTION = 4 };

enum { GIZFORCE_PROGRESS_VISIBLE = 2 };

enum { GIZFORCE_RUNTIME_ALONG_SOCKET_HIDDEN = 8 };

enum { GIZBUILDIT_AVAILABILITY_ACTIVE = 1 };

enum { GIZBUILDIT_AVAILABILITY_VISIBLE = 2 };

enum { GIZBUILDIT_RUNTIME_REWARD_RELEASED = 2 };

enum { GIZBUILDIT_STATE_TURN_OFF_WHEN_COMPLETE = 2 };

enum { DOOR_FLAG_CAMERA_LOOK_AT_PLAYERS = 2 };

enum { DOOR_FLAG_DO_NOT_USE = 4 };

enum { GIZMOPICKUP_STATE_ENABLED = 2 };

enum { GIZMOPICKUP_STATE_VISIBLE = 4 };

enum { GIZTORPMACHINE_FLAG_ALTERNATE_MODEL = 4 };

enum { GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID = 1 };

enum { GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING = 8 };

enum { NUQFNT_CSMODE_ABSOLUTE = 4 };

enum { NUQFNT_CSMODE_NORMALISED = 3 };
#ifdef __cplusplus
extern "C" {
#endif
    extern const char GameAudioName0075c1ecPc[9];
    extern const char GameAudioName0075c1f8Pc[10];
    extern const char GameAudioName0075c204Pc[11];
    extern const char GameAudioName0075c210Pc[9];
    extern const char GameAudioName0075c21cPc[10];
    extern const char GameAudioName0075c228Pc[10];
    extern const char GameAudioName0075c234Pc[11];
    extern const char GameAudioName0075c284Pc[10];
    extern const char GameAudioName0075c3acPc[9];
    extern const char GameAudioName0075c3fcPc[11];
    extern const char GameAudioName0075c408Pc[11];
    extern const char GameAudioName0075c420Pc[10];
    extern const char GameAudioName0075c42cPc[10];
    extern const char GameAudioName0075c438Pc[12];
    extern const char GameAudioName0075c444Pc[11];
    extern const char GameAudioName0075c490Pc[19];
    extern const char GameAudioName0075c4a4Pc[9];
    extern const char GameAudioName0075c4b0Pc[10];
    extern const char GameAudioName0075c4bcPc[10];
    extern const char GameAudioName0075c4c8Pc[10];
    extern const char GameAudioName0075c4d4Pc[10];
    extern const char GameAudioName0075c4e0Pc[10];
    extern const char GameAudioName0075c4ecPc[12];
    extern const char GameAudioName0075c4f8Pc[9];
    extern const char GameAudioName0075c504Pc[10];
    extern const char GameAudioName0075c510Pc[12];
    extern const char GameAudioName0075c51cPc[11];
    extern const char GameAudioName0075c528Pc[12];
    extern const char MenuVideoAspectErrorFormatPc[10];
    extern const char MenuVideoAspect1610FormatPc[10];
    extern const char MenuVideoAspect169FormatPc[9];
    extern const char MenuVideoAspect43FormatPc[8];
    extern const char MenuVideoRefreshFormatPc[9];
    extern const char MenuVideoResolutionFormatPc[12];
    extern const char MenuEffects16xFormatPc[8];
    extern const char MenuEffects8xFormatPc[7];
    extern const char MenuEffects4xFormatPc[7];
    extern const char MenuEffects2xFormatPc[7];
    extern const char MenuVideoCountdownFormatPc[9];

    extern const char ComboSaber1NamePc[12];
    extern const char ComboSaber2NamePc[12];
#ifdef __cplusplus
}
#endif

#endif
