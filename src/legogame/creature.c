#include "saga.h"
#include <string.h>

typedef struct PLAYER_PACKET_RESET_PC {
    u8 unknown_000[0x101];
    u8 context_state_flags;
    u8 unknown_102[0x4d2];
    NUVEC reset_up_direction;
    GAMEPAD_PC *gamepad;
    u8 unknown_5e4[0x20];
    void *field_604;
    u8 unknown_608[0xc];
    u32 field_614[8];
    u8 unknown_634[0x28];
    i32 input_state;
    f32 field_660;
    u8 unknown_664[0x20];
    f32 delayed_turn_timer;
    f32 field_688;
    u8 unknown_68c[0x4];
    f32 field_690;
    u8 unknown_694[0x24];
    f32 field_6b8;
    u8 unknown_6bc[0x1c];
    f32 field_6d8;
    f32 ground_height;
    u8 unknown_6e0[0xc];
    f32 field_6ec;
    u8 unknown_6f0[0x18];
    f32 field_708;
    f32 field_70c;
    f32 field_710;
    u8 unknown_714[0x4];
    f32 field_718;
    u8 unknown_71c[0x10];
    f32 field_72c;
    u8 unknown_730[0x1c];
    i16 movement_angle_0;
    i16 movement_angle_1;
    u8 unknown_750[0x4];
    u16 movement_lean_angle;
    u16 secondary_lean_angle;
    u16 tertiary_lean_angle;
    i16 force_glow_index;
    u32 secondary_flags;
    u32 render_flags;
    u8 random_state[4];
    u8 unknown_768[0x17];
    i8 surface_type;
    u8 unknown_780[0x4];
    f32 field_784;
} PLAYER_PACKET_RESET_PC;

STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, context_state_flags) == 0x101);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, reset_up_direction) == 0x5d4);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, gamepad) == 0x5e0);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_604) == 0x604);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_614) == 0x614);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, input_state) == 0x65c);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_660) == 0x660);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, delayed_turn_timer) == 0x684);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_688) == 0x688);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_690) == 0x690);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_6b8) == 0x6b8);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_6d8) == 0x6d8);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, ground_height) == 0x6dc);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_6ec) == 0x6ec);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_708) == 0x708);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_70c) == 0x70c);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_710) == 0x710);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_718) == 0x718);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_72c) == 0x72c);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, movement_angle_0) == 0x74c);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, movement_angle_1) == 0x74e);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, movement_lean_angle) == 0x754);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, secondary_lean_angle) == 0x756);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, tertiary_lean_angle) == 0x758);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, force_glow_index) == 0x75a);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, secondary_flags) == 0x75c);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, render_flags) == 0x760);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, random_state) == 0x764);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, surface_type) == 0x77f);
STATIC_ASSERT(offsetof(PLAYER_PACKET_RESET_PC, field_784) == 0x784);

typedef struct GAMEPAD_RESET_PC {
    void *bound_pad;
    u32 buttons_held;
    u32 buttons_pressed;
    u32 buttons_released;
    u32 left_directions;
    u32 previous_left_directions;
    u32 right_directions;
    u32 previous_right_directions;
    u32 field_20;
    u8 unknown_024[0x1c];
    f32 rumble_field_40;
    f32 rumble_amount;
    f32 rumble_field_48;
    f32 rumble_time;
    u8 unknown_050[0x4];
    void *operator_data;
    u8 unknown_058[0x2];
    u16 runtime_flags;
} GAMEPAD_RESET_PC;

STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, bound_pad) == 0x0);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, buttons_held) == 0x4);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, buttons_pressed) == 0x8);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, buttons_released) == 0xc);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, left_directions) == 0x10);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, previous_left_directions) == 0x14);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, right_directions) == 0x18);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, previous_right_directions) == 0x1c);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, rumble_field_40) == 0x40);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, rumble_amount) == 0x44);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, rumble_field_48) == 0x48);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, rumble_time) == 0x4c);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, operator_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEPAD_RESET_PC, runtime_flags) == 0x5a);

typedef struct TERRAIN_PLAYER_IMPACT_PC {
    NUVEC position, normal;
    u8 material[2], flags, normal_flags;
} TERRAIN_PLAYER_IMPACT_PC;

STATIC_ASSERT(sizeof(TERRAIN_PLAYER_IMPACT_PC) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_IMPACT_PC, normal) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_IMPACT_PC, material) == 0x18);

typedef struct TERRAIN_PLAYER_PLATFORM_RECORD_PC {
    u8 unknown_0[0xe];
    i16 platform_id;
    GAMEOBJECT *object;
} TERRAIN_PLAYER_PLATFORM_RECORD_PC;

typedef struct TERRAIN_PLAYER_PLATFORM_SYS_PC {
    u8 unknown_0[4];
    i32 count;
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1];
} TERRAIN_PLAYER_PLATFORM_SYS_PC;

STATIC_ASSERT(sizeof(TERRAIN_PLAYER_PLATFORM_RECORD_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, object) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, entries) == 8);

typedef struct TERRAIN_PLAYER_GRABBER_PC {
    u8 unknown_0[0x554];
    f32 platform_contact_timer;
    u8 unknown_558[8];
    i16 platform_id;
} TERRAIN_PLAYER_GRABBER_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_GRABBER_PC, platform_contact_timer) == 0x554);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_GRABBER_PC, platform_id) == 0x560);

typedef struct TERRAIN_PLAYER_WORLD_PC {
    u8 unknown_0[0x46ec];
    TERRAIN_PLAYER_GRABBER_PC *grabber;
} TERRAIN_PLAYER_WORLD_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_WORLD_PC, grabber) == 0x46ec);

typedef struct TERRAIN_PLAYER_BLOWUP_PC {
    u8 unknown_0[0x102];
    i16 platform_id;
} TERRAIN_PLAYER_BLOWUP_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_BLOWUP_PC, platform_id) == 0x102);

typedef struct TERRAIN_PLAYER_COINS_PC {
    u32 coins;
    f32 scale;
} TERRAIN_PLAYER_COINS_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_COINS_PC, coins) == 0);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_COINS_PC, scale) == 4);

typedef struct TERRAIN_PLAYER_SET_PC {
    u8 unknown_0[0x1bac];
    i32 minimum_height;
} TERRAIN_PLAYER_SET_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_SET_PC, minimum_height) == 0x1bac);

typedef struct TERRAIN_PLAYER_CHARACTER_PC {
    u8 unknown_0[0x18];
    void(__cdecl *move_callback)(GAMEOBJECT *);
} TERRAIN_PLAYER_CHARACTER_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLAYER_CHARACTER_PC, move_callback) == 0x18);
void __cdecl TakeOverCode(GAMEOBJECT *, i32);
void __cdecl ApplyGravity(GAMEOBJECT *, f32 *, f32, f32, f32 *);
void __cdecl Tag_Check(GAMEOBJECT *);
void __cdecl GameObjectRotation(GAMEOBJECT *, i32);
#define AIPATHCNX_FLAG_SET(connection, field, flag)                                                                    \
    ((connection) != NULL && ((connection)->field[0] & (u32)(flag)) != 0)

typedef struct level_ai_progress_pc_s {
    u8 unknown_0000[0x2808];

    union {
        u32 disabled_ai_object_mask[2];
        u64 disabled_ai_objects;
    };
} LEVEL_AI_PROGRESS_PC;

typedef struct AI_CREATURE_RESET_VIEW_PC {
    u8 unknown_00[0x10];
    char script_name[0x10];
    u8 unknown_20[0x2c];
    u8 creature_set;
    u8 unknown_4d[4];
    u8 count_across;
} AI_CREATURE_RESET_VIEW_PC;

typedef struct AI_RESET_EXTRA_PC {
    u8 unknown_000[0x1086];
    u8 search_state;
    u8 avoidance_state;
    u8 unknown_1088[8];
    f32 reset_timer;
} AI_RESET_EXTRA_PC;

typedef struct AI_CREATURE_RESET_FLAGS_PC {
    u8 unknown_0000[0x1386];
    u16 reset_flags;
} AI_CREATURE_RESET_FLAGS_PC;

typedef struct KEEP_SCREEN_CAMERA_FIELDS_PC {
    u8 unknown_000[0x110];
    NUVEC direction;
    u8 unknown_11c[0xdc];
    u16 pitch;
} KEEP_SCREEN_CAMERA_FIELDS_PC;

void __cdecl MovePlayer_VEHICLEDIRECTIONAL(GAMEOBJECT *object);
void __cdecl MovePlayer_ROLLING(GAMEOBJECT *object);
i32 __cdecl MovePlayer_TWIST(GAMEOBJECT *object);
i32 __cdecl MovePlayer_CIRCLE(GAMEOBJECT *object);
i32 __cdecl MovePlayer_GUNSHIPIN(GAMEOBJECT *object);
i32 __cdecl MovePlayer_POD(GAMEOBJECT *object);
void __cdecl MovePlayer_DIRECTIONAL(GAMEOBJECT *object);

typedef struct moveplayer_pad_bits_pc_s {
    u16 low : 3;
    u16 waggle : 1;
    u16 high : 12;
} MOVEPLAYER_PAD_BITS_PC;

static __inline f32 MovePlayerFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

typedef struct CHARACTER_IDLE_STATE_PC {
    u8 unknown_000[0x2b8];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;
} CHARACTER_IDLE_STATE_PC;

typedef struct CHARACTER_IDLE_POLICY_PC {
    u8 unknown_000[0x116];
    u8 use_standard_idle;
} CHARACTER_IDLE_POLICY_PC;

f32 animduration_blendouttime;
// Append to src/legoapi/characters.cpp. Retail 00419e90/284 and 00419fb0/808.
i32 CharacterLoadAreaModelsPc;
i16 CharacterFixupIdPc007f18c4;
i32 __cdecl AnimPacketModelsCompatiblePc(ANIMPACKET_PC *packet, i32 source_id, i32 target_id);

typedef struct SPECIAL_LAYER_PC_s {
    i16 *character_id;
    const char *name;
    u32 mask;
} SPECIAL_LAYER_PC;

typedef struct player_reset_fields_pc_s {
    u8 unknown_0[0xc50];
    NUVEC weapon_trail_offset;
    u8 unknown_c5c[0xa4];
    void *incoming_special;
    u8 unknown_d04[0x14];
    void *force_glow_candidate;
    u8 unknown_d1c[8];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_da4[0x14];
    f32 pre_reset_hover_height;
    u8 unknown_dbc[0x28];
    f32 hold_timer;
    u8 unknown_de8[0x1c];
    i32 dynamic_light_id;
    u8 unknown_e08[0x275];
    u8 pre_reset_state;
} PLAYER_RESET_FIELDS_PC;

typedef struct player_suit_name_pc_s {
    u8 unknown_000[0x4];
    char *name;
} PLAYER_SUIT_NAME_PC;

typedef struct player_packet_storage_pc_s {
    u8 unknown_000[0x6c4];
    u8 player_packet_storage[0x788];
} PLAYER_PACKET_STORAGE_PC;

typedef struct player_batarang_idle_pc_s {
    u8 unknown_00[0x7d];
    u8 active;
} PLAYER_BATARANG_IDLE_PC;

typedef struct player_idle_character_pc_s {
    u8 unknown_00[0x116];
    u8 use_standard_idle;
} PLAYER_IDLE_CHARACTER_PC;

/* Two-DWORD consumer view of the existing genuine mutable default mask. */
typedef struct PLAYER_ROUTE_MASK_WORDS_PC {
    u32 low;
    u32 high;
} PLAYER_ROUTE_MASK_WORDS_PC;

extern u64 CharacterMaskAllPc;
extern i32 complexsockposition_forcesock;

typedef struct CREATURE_RESET_PACKET_VIEW_PC {
    u8 unknown_000[0x6c4];
    u8 packet[0x788];
} CREATURE_RESET_PACKET_VIEW_PC;

typedef struct PLAYER_LAYER_FLAGS_PC {
    u32 unknown_0 : 9;
    u32 bob : 1;
    u32 unknown_10 : 22;
} PLAYER_LAYER_FLAGS_PC;

extern i16 CharacterFixupIdPc007f1810;

typedef struct PARAPHERNALIA_EXTRA_PC {
    u8 prefix[0xc68];
    NUVEC force_position;
    u8 gap_c74[0x10c];
    f32 force_alpha;
    u8 gap_d84[8];
    f32 force_radius;
    u8 gap_d90[0x2ec];
    u8 draw_reflection;
} PARAPHERNALIA_EXTRA_PC;

void __cdecl DrawForceGlowSprite(NUVEC *, f32, i32, f32, GAMEOBJECT *);
void __cdecl DrawSnakeBody(GAMEOBJECT *);

typedef struct draw_objects_flags_pc_s {
    u8 unknown_000[0xe24];
    u32 flags_e24_bits_0_2 : 3;
    u32 locators_updated : 1;
} DRAW_OBJECTS_FLAGS_PC;

extern f32 DrawGameObjectsFarClipPc, DrawGameObjectsVehicleFarClipPc;

typedef struct new_player_flags_pc_s {
    u8 unknown_000[0xef0];
    u32 flags_ef0_bits_0_8 : 9;
    u32 geonosian_variant : 1;
    u32 flags_ef0_bits_10_20 : 11;
    u32 flags_ef0_bit_21 : 1;
} NEW_PLAYER_FLAGS_PC;

typedef struct player_init_progress_pc_s {
    u32 coins;
    i16 previous_state;
    u8 active, stored_flag;
    i8 hitpoints;
    u8 unknown_09, helmet, unknown_0b;
    f32 powerup_time;
} PLAYER_INIT_PROGRESS_PC;

typedef struct player_init_extra_pc_s {
    u8 unknown_0000[0x454];
    f32 field_454;
    u8 unknown_0458[0xcc0 - 0x458];
    PLAYER_BATARANG_SLOT_PC *batarang;
    u8 unknown_0cc4[0x1062 - 0xcc4];
    i16 previous_state;
} PLAYER_INIT_EXTRA_PC;

typedef struct player_init_flags_pc_s {
    u8 unknown_000[0xe20];
    u32 flags_e20_bits_0_15 : 16;
    u32 stored_flag : 1;
} PLAYER_INIT_FLAGS_PC;

extern i32 movegamecamera_forcesock;

typedef union DRAW_ATTACHMENTS_PACKET_PC {
    ANIMPACKET_PC packet;
    u32 words[18];
} DRAW_ATTACHMENTS_PACKET_PC;

/* Private players.c helper. Retail 0x419130, 158 bytes through RET at 0x4191cd.
 * Retail private ABI: object=EAX; f32 result=ST0.
 * The two calculated return paths require a final f32 store/reload.
 */
extern const f32 CharacterWeaponScaleUpperBoundPc;
f32 __cdecl DropInOutScale(GAMEOBJECT *object);

/* Private native PC 41cb50/2038 draft; integrate only after source review. */
typedef struct CHARACTER_DRAW_HIERARCHY_PC {
    u8 unknown_000[0x1a0];
    u8 suppress_shadow_surface_points;
} CHARACTER_DRAW_HIERARCHY_PC;

extern i32 GameKeepMergeActionPc;
extern i32 CharacterBlobShadowsOnPc;
extern i32 GameDrawCharacterModelFlagPc;

/* Selective append to players.c after the existing five private matrix helpers. */
typedef struct DRAW_PREPASS_TELEPORT_PC {
    u8 unknown_00[0x4c];
    u8 flags;
} DRAW_PREPASS_TELEPORT_PC;

typedef struct DRAW_PREPASS_TELEPORT_OBJECT_PC {
    u8 unknown_000[0x798];
    DRAW_PREPASS_TELEPORT_PC *teleport;
} DRAW_PREPASS_TELEPORT_OBJECT_PC;

f32 __cdecl PodSprint_RollMul(GAMEOBJECT *);
void __cdecl ApplyExtraRotation(GAMEOBJECT *, NUMTX *);

typedef struct draw_process_flags_pc_s {
    u8 unknown_000[0xe20];
    u32 flags_e20_bits_0_26 : 27;
    u32 origin_joint_was_ready : 1;
    u32 flags_e20_bits_28_31 : 4;
    u8 unknown_e24[0xef4 - 0xe24];
    u32 flags_ef4_bits_0_7 : 8;
    u32 previous_model_draw_result : 1;
} DRAW_PROCESS_FLAGS_PC;

extern i32 DrawParaphernaliaEnabledPc;
extern i16 CharacterFixupIdPc007f1820;
void __cdecl CustomiserDrawAccessoriesPc(CUSTOMISER_LAYERS_PC *, GAMEOBJECT *, NUMTX *);
void __cdecl Grapple_DrawLine(GAMEOBJECT *);
void __cdecl SuperCarry_DrawObject(GAMEOBJECT *);
void __cdecl DrawTorpedos(GAMEOBJECT *);
void __cdecl GizDrawBuildItPiece(GAMEOBJECT *, i32);
void __cdecl TransformDrawTargetPc(NUVEC *, f32, f32);

/* Native 0x0041fe90, 526 bytes. */
typedef struct ai_creature_init_packet_pc_s {
    u8 unknown_000[0xa0];
    void *area;
    void *locator;
} AI_CREATURE_INIT_PACKET_PC;

#pragma const_seg(".rdata$SpecialLayerName_bombbackpack")
static const char SpecialLayerName_bombbackpack[13] = "bombbackpack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_sonargun")
static const char SpecialLayerName_sonargun[9] = "sonargun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_infrared_goggles")
static const char SpecialLayerName_infrared_goggles[17] = "infrared_goggles";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_black")
static const char SpecialLayerName_mask_black[11] = "mask_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_blue")
static const char SpecialLayerName_mask_blue[10] = "mask_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_red")
static const char SpecialLayerName_mask_red[9] = "mask_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_black")
static const char SpecialLayerName_cape_black[11] = "cape_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_blue")
static const char SpecialLayerName_cape_blue[10] = "cape_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_nextgen")
static const char SpecialLayerName_body_grey_nextgen[18] = "body_grey_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_high")
static const char SpecialLayerName_body_grey_high[15] = "body_grey_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_low")
static const char SpecialLayerName_body_grey_low[14] = "body_grey_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_nextgen")
static const char SpecialLayerName_body_black_nextgen[19] = "body_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_high")
static const char SpecialLayerName_body_black_high[16] = "body_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_low")
static const char SpecialLayerName_body_black_low[15] = "body_black_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_nextgen")
static const char SpecialLayerName_body_blue_nextgen[18] = "body_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_high")
static const char SpecialLayerName_body_blue_high[15] = "body_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_low")
static const char SpecialLayerName_body_blue_low[14] = "body_blue_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_black")
static const char SpecialLayerName_face_hands_black[17] = "face_hands_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_blue")
static const char SpecialLayerName_face_hands_blue[16] = "face_hands_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_red")
static const char SpecialLayerName_face_hands_red[15] = "face_hands_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_nextgen")
static const char SpecialLayerName_hips_black_nextgen[19] = "hips_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_high")
static const char SpecialLayerName_hips_black_high[16] = "hips_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_nextgen")
static const char SpecialLayerName_hips_blue_nextgen[18] = "hips_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_high")
static const char SpecialLayerName_hips_blue_high[15] = "hips_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_nextgen")
static const char SpecialLayerName_hips_red_nextgen[17] = "hips_red_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_high")
static const char SpecialLayerName_hips_red_high[14] = "hips_red_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_magnetic_boots")
static const char SpecialLayerName_magnetic_boots[15] = "magnetic_boots";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_scuba_gear")
static const char SpecialLayerName_scuba_gear[11] = "scuba_gear";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hack_pack")
static const char SpecialLayerName_hack_pack[10] = "hack_pack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_vacuum_gun")
static const char SpecialLayerName_vacuum_gun[11] = "vacuum_gun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_green")
static const char SpecialLayerName_nextgen_limbs_green[20] = "nextgen_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_green")
static const char SpecialLayerName_hires_limbs_green[18] = "hires_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_green")
static const char SpecialLayerName_lowres_limbs_green[19] = "lowres_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_white")
static const char SpecialLayerName_nextgen_limbs_white[20] = "nextgen_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_white")
static const char SpecialLayerName_hires_limbs_white[18] = "hires_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_white")
static const char SpecialLayerName_lowres_limbs_white[19] = "lowres_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_grey")
static const char SpecialLayerName_nextgen_limbs_grey[19] = "nextgen_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_grey")
static const char SpecialLayerName_hires_limbs_grey[17] = "hires_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_grey")
static const char SpecialLayerName_lowres_limbs_grey[18] = "lowres_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_blue")
static const char SpecialLayerName_nextgen_limbs_blue[19] = "nextgen_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_blue")
static const char SpecialLayerName_hires_limbs_blue[17] = "hires_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_blue")
static const char SpecialLayerName_lowres_limbs_blue[18] = "lowres_limbs_blue";
#pragma const_seg()

/* Private source fragment for src/legoapi/players.c; not a standalone TU.
 * Retail private ABI: Copy src=EAX,dst=ECX; other helpers matrix=EAX,angle=ECX.
 * Same-TU compiler convention must be checked with all retail callers.
 */
static void GameDrawMtxCopyPc(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static void GameDrawMtxSetYRotationPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0.0f;
    matrix->m02 = -sine;
    matrix->m03 = 0.0f;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0.0f;
    matrix->m23 = 0.0f;
    matrix->m22 = cosine;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void GameDrawMtxRotateXPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m01 = matrix->m01;
    f32 m11 = matrix->m11;
    f32 m21 = matrix->m21;
    f32 m31 = matrix->m31;

    matrix->m01 = (m01 * cosine) - (matrix->m02 * sine);
    matrix->m02 = (m01 * sine) + (matrix->m02 * cosine);
    matrix->m11 = (m11 * cosine) - (matrix->m12 * sine);
    matrix->m12 = (m11 * sine) + (matrix->m12 * cosine);
    matrix->m21 = (m21 * cosine) - (matrix->m22 * sine);
    matrix->m22 = (m21 * sine) + (matrix->m22 * cosine);
    matrix->m31 = (m31 * cosine) - (matrix->m32 * sine);
    matrix->m32 = (m31 * sine) + (matrix->m32 * cosine);
}

static void GameDrawMtxRotateYPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;

    matrix->m00 = m00 * cosine + matrix->m02 * sine;
    matrix->m02 = matrix->m02 * cosine - m00 * sine;
    matrix->m10 = m10 * cosine + matrix->m12 * sine;
    matrix->m12 = matrix->m12 * cosine - m10 * sine;
    matrix->m20 = m20 * cosine + matrix->m22 * sine;
    matrix->m22 = matrix->m22 * cosine - m20 * sine;
    matrix->m30 = m30 * cosine + matrix->m32 * sine;
    matrix->m32 = matrix->m32 * cosine - m30 * sine;
}

static void GameDrawMtxRotateZPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;

    matrix->m00 = (m00 * cosine) - (matrix->m01 * sine);
    matrix->m01 = (m00 * sine) + (matrix->m01 * cosine);
    matrix->m10 = (m10 * cosine) - (matrix->m11 * sine);
    matrix->m11 = (m10 * sine) + (matrix->m11 * cosine);
    matrix->m20 = (m20 * cosine) - (matrix->m21 * sine);
    matrix->m21 = (m20 * sine) + (matrix->m21 * cosine);
    matrix->m30 = (m30 * cosine) - (matrix->m31 * sine);
    matrix->m31 = (m30 * sine) + (matrix->m31 * cosine);
}

void __cdecl FixUpCharacters(CHARFIXUP_PC *fixup) {
    i32 i;
    for (i = 0; i < CHARCOUNT; ++i) {
        GCDataList[i] = GCDATA_DEFAULT;
        if (CDataList[i].name_id == -1) {
            CDataList[i].name_id = tUNKNOWN;
        }
        if (CDataList[i].move_fn == NULL) {
            CDataList[i].move_fn = Move_CHARACTER;
        }
        if (CDataList[i].animate_fn == NULL) {
            CDataList[i].animate_fn = Animate_CHARACTER;
        }
    }
    if (fixup != NULL) {
        while (fixup->name != NULL) {
            if (fixup->id != NULL) {
                *fixup->id = (i16)CharIDFromName(fixup->name);
            }
            ++fixup;
        }
    }
}

void __cdecl ExtraCharacterFixUpAfterConfig(void) {
    i32 i;
    SetMoveAndAnimateFunctions(8, 8, 0, 0, -1, Move_JEDI, Animate_JEDI, NULL);
    SetMoveAndAnimateFunctions(0x1000010, 0x10, 0, 0, -1, Move_DROIDGENERIC, NULL, NULL);
    SetMoveAndAnimateFunctions(0x30, 0x30, 0, 0, -1, Move_DROIDGENERIC, Animate_PROTOCOL, NULL);
    SetMoveAndAnimateFunctions(0x50, 0x50, 0, 0, -1, Move_DROIDGENERIC, Animate_ASTROMECH, PostAnimate_ASTROMECH);
    SetMoveAndAnimateFunctions(0, 0, 0x800, 0x800, -1, Move_CANNON, Animate_CANNON, NULL);
    SetMoveAndAnimateFunctions(0x2000, 0x2000, 0, 0, -1, Move_VEHICLE, Animate_VEHICLE, NULL);
    SetMoveAndAnimateFunctions(0x40000000, 0x40000000, 0, 0, -1, Move_BEAST, Animate_BEAST, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 4, Move_DROIDGENERIC, Animate_BATTLEDROID, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 17, Move_HOVERDROID, Animate_HOVERDROID, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 15, Move_WALKER, Animate_WALKER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 16, Move_ATAT, Animate_ATAT, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 18, Move_CRITTER, Animate_CRITTER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 18, Move_CRITTER, Animate_CRITTER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 20, Move_POD, Animate_POD, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 2, NULL, NULL, PostAnimate_FETT);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0, Move_WEIRDO, Animate_WEIRDO, NULL);
    for (i = 0; i < CHARCOUNT; ++i) {
        if ((GCDataList[i].flags_094 & 0x10000000) != 0) {
            CDataList[i].move_fn = Move_GEONOSIAN;
            CDataList[i].animate_fn = Animate_GEONOSIAN;
        }
    }
    if (id_DROIDEKA != -1) {
        CDataList[id_DROIDEKA].move_fn = Move_DROIDEKA;
        CDataList[id_DROIDEKA].animate_fn = Animate_DROIDEKA;
    }
    if (id_MINIDROIDEKA != -1) {
        CDataList[id_MINIDROIDEKA].move_fn = Move_DROIDEKA;
        CDataList[id_MINIDROIDEKA].animate_fn = Animate_DROIDEKA;
    }
    if (id_SUPERBATTLEDROID != -1) {
        CDataList[id_SUPERBATTLEDROID].move_fn = Move_SUPERBATTLEDROID;
        CDataList[id_SUPERBATTLEDROID].animate_fn = Animate_SUPERBATTLEDROID;
    }
    if (id_BARMAN != -1) {
        CDataList[id_BARMAN].move_fn = Move_BARMAN;
        CDataList[id_BARMAN].animate_fn = Animate_BARMAN;
    }
    if (id_JAWA != -1) {
        CDataList[id_JAWA].move_fn = Move_JAWA;
    }
    if (id_UGNAUGHT != -1) {
        CDataList[id_UGNAUGHT].move_fn = Move_JAWA;
    }
    if ((i16)id_DRAGBOMB != -1) {
        CDataList[(i16)id_DRAGBOMB].move_fn = Move_DRAGBOMB;
    }
    if (id_REPUBLICGUNSHIP != -1) {
        CDataList[id_REPUBLICGUNSHIP].move_fn = Move_REPUBLICGUNSHIP;
        CDataList[id_REPUBLICGUNSHIP].animate_fn = Animate_REPUBLICGUNSHIP;
    }
    if (id_REPUBLICGUNSHIP_GREEN != -1) {
        CDataList[id_REPUBLICGUNSHIP_GREEN].move_fn = Move_REPUBLICGUNSHIP;
        CDataList[id_REPUBLICGUNSHIP_GREEN].animate_fn = Animate_REPUBLICGUNSHIP;
    }
    if (id_SPEEDERBIKE != -1) {
        CDataList[id_SPEEDERBIKE].move_fn = Move_SPEEDERBIKE;
        CDataList[id_SPEEDERBIKE].animate_fn = Animate_SPEEDERBIKE;
    }
    if (VehicleBoltId7f18b4Pc != -1) {
        CDataList[VehicleBoltId7f18b4Pc].animate_fn = Animate_DEFAULT;
    }
    if (id_BODYGUARD != -1) {
        CDataList[id_BODYGUARD].move_fn = Move_JEDI;
        CDataList[id_BODYGUARD].animate_fn = Animate_JEDI;
    }
    if (id_IMPERIALGUARD != -1) {
        CDataList[id_IMPERIALGUARD].move_fn = Move_JEDI;
        CDataList[id_IMPERIALGUARD].animate_fn = Animate_JEDI;
    }
}

void __cdecl ResetPlayerPacket(void *opaque_packet, CHARACTERDATA_PC *character) {
    PLAYER_PACKET_RESET_PC *packet = (PLAYER_PACKET_RESET_PC *)opaque_packet;
    i32 i;
    (void)character;
    packet->context_state_flags &= (u8)~1;
    packet->field_660 = 0.0f;
    packet->delayed_turn_timer = 0.0f;
    packet->movement_lean_angle = 0;
    packet->secondary_lean_angle = 0;
    packet->tertiary_lean_angle = 0;
    Player_ResetContexts(packet);
    ((GAMEPAD_RESET_PC *)packet->gamepad)->buttons_held = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->buttons_pressed = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->buttons_released = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->left_directions = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->previous_left_directions = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->right_directions = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->previous_right_directions = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->field_20 = 0;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->rumble_field_40 = 0.0f;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->rumble_amount = 0.0f;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->rumble_field_48 = 0.0f;
    ((GAMEPAD_RESET_PC *)packet->gamepad)->rumble_time = 0.0f;
    packet->gamepad->runtime_flags &= (u16)~2;
    packet->gamepad->runtime_flags &= (u16)~4;
    packet->gamepad->runtime_flags &= (u16)~0x10;
    packet->secondary_flags &= ~0x400u;
    packet->input_state = 0;
    packet->gamepad->operator_data = NULL;
    CharShadows_Reset((CHARACTER_SHADOW_PACKET_PC *)packet);
    for (i = 0; i < 8; ++i) {
        packet->field_614[i] = 0;
    }
    packet->field_6b8 = 0.0f;
    packet->field_6d8 = 0.0f;
    ResetForceGlow(packet);
    packet->ground_height = 2000000.0f;
    packet->secondary_flags &= ~1u;
    packet->field_72c = 0.0f;
    packet->force_glow_index = -1;
    packet->field_6ec = 0.0f;
    packet->movement_angle_0 = -1;
    packet->movement_angle_1 = qrand() < 0x8000 ? 0x26 : 0x4f;
    packet->field_708 = 0.0f;
    for (i = 0; i < 4; ++i) {
        packet->random_state[i] = (u8)(qrand() >> 8);
    }
    packet->render_flags &= 0xfffff3feu;
    packet->field_70c = 0.0f;
    packet->field_604 = NULL;
    packet->field_710 = 0.0f;
    packet->surface_type = -1;
    packet->field_718 = 0.0f;
    packet->reset_up_direction = v010;
    packet->field_690 = 0.0f;
    packet->field_688 = 0.0f;
    packet->field_784 = 0.0f;
}

void __cdecl SetFlicker(GAMEOBJECT *object, f32 duration) {
    object->flags_e24 &= ~0x70000;
    object->vehicle_hit_timer = duration;
}

void __cdecl ResetPlayerAI(GAMEOBJECT *object) {
    object->special_move_timer_pc = 0.0f;
    object->ai_packet.animation_override_from = -1;
    object->ai_packet.animation_override_to = -1;
    object->flags_ef0 &= ~0x200000u;
    object->ai_packet.flags &= 0xfffffd9f;
    object->ai_packet.intersection_connection = NULL;
    object->ai_packet.intersection_target_connection = NULL;
    object->ai_packet.path_target = NULL;
    object->field_1086 = 0;
    object->field_1087 = 0;
    object->field_108c = 0;
    object->field_1088 = 0;
    object->can_use_object = NULL;
    object->use_action_word = 0;
    object->use_action_parameter_word = 0;
    object->ai_packet.frame_state = 0;
    object->route_frame_state = 0;
    memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
    object->ai_packet.inside_path_node = -1;
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
}

void __cdecl DeactivateGameObject(GAMEOBJECT *object) {
    if (object == NULL) {
        return;
    }
    if (object->rider != NULL) {
        if ((object->flags_1f8 & 0x4000) != 0) {
            ReleaseTakeOver(object, 1);
        } else {
            KillGameObject(object->rider, 4, 0);
        }
    }
    object->flags_1fc_word &= ~0x1000u;
    if (object->ai_packet.creature_index != 0xff &&
        AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)GameObjectState_InActive) != 0) {
        object->ai_packet.reset_mode = 0;
        WORLD->ai_sys->creatures[object->ai_packet.creature_index].activation_type = 2;
    } else {
        object->ai_packet.reset_mode = 4;
    }
}

void DeactivateCharacter(char *name) {
    struct gameobject_s *object;
    if (Mission_Active(NULL) != NULL || name == NULL) {
        return;
    }
    object = GetNamedGameObject(WORLD->ai_sys, name);
    if (object != NULL && (FreePlay == 0 || (object->flags_1f8 & 0x400) != 0)) {
        DeactivateGameObject(object);
    }
}

void __cdecl SetGameObjectCharacterData(GAMEOBJECT *object) {
    CHARACTERDATA_PC *data = object->character_data;
    object->origin_scale = data->model_scale;
    object->collision_radius_unscaled = data->collision_radius;
    object->character_bottom = data->bottom;
    object->character_top = data->top;
    ScaleGameObject(object);
}

static __declspec(noinline) void InitCreatureModelFlagsPc(GAMEOBJECT *object) {
    if ((object->character_data->game_character->flags_090 & 0x8000) != 0) {
        object->flags_1f8 |= 0x20000;
    } else {
        object->flags_1f8 &= ~0x20000u;
    }
}

/* Original source name unknown. Retail places this with the game object code
   at 0x414d10, and PlayersDropInOut calls it rather than inlining it, so the
   definition lives in its own unit. */
void __cdecl PlayerSetCurrentHpPc(GAMEOBJECT *object, i8 hitpoints) {
    object->current_hp = hitpoints;
    if ((i32)hitpoints > (i32)object->field_107e) {
        object->current_hp = object->field_107e;
    }
}

i32 __cdecl MakePlayerList(i32 count) {
    i32 player_count = 0;
    i32 i;
    GAMEOBJECT *player;
    PLAYERPROGRESS *progress;
    for (i = 0; i < count; ++i) {
        player = Player[i];
        if (player == NULL) {
            continue;
        }
        if (makeplayerlist_freeplay == 1) {
            PlayerList[player_count] = FreePlayModelList[i].model_id;
        } else if (makeplayerlist_freeplay != 2) {
            PlayerList[player_count] = player->id;
        }
        progress = &PlayerProgress[player_count];
        progress->active = player->player_controlled;
        if (player->character_context != 6 && player->field_e33 != 2 &&
            ((player->field_e22 & 1) != 0 || player->field_e33 != 0)) {
            progress->field_7 = 1;
        } else {
            progress->field_7 = 0;
        }
        if (player->coin_packet != NULL) {
            progress->coins = ((PLAYER_COIN_PACKET_PC *)player->coin_packet)->coins;
        } else {
            progress->coins = 0;
        }
        progress->field_b = player->jump_variant_state == 1;
        if (player->dead != 0) {
            progress->hitpoints = player->field_107e;
            progress->field_9 = 4;
        } else {
            progress->hitpoints = player->current_hp;
            progress->field_9 = player->saved_hitpoints;
        }
        progress->field_a = player->helmet;
        progress->unknown_0004 = player->field_1062;
        progress->field_c = player->powerup_time;
        PlayerSuit[player_count] = player->suit;
        if (player->torpedo != NULL) {
            PlayerTorpedoCount[player_count] = player->torpedo->count;
        } else {
            PlayerTorpedoCount[player_count] = 0;
        }
        ++player_count;
    }
    makeplayerlist_freeplay = 0;
    for (i = player_count; i < 8; ++i) {
        PlayerList[i] = -1;
    }
    return player_count;
}

void ClearAICreatures() {
    i32 i;
    GAMEOBJECT *object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0) {
            FreeTorpedoPacket(&object->torpedo);
            RemoveGameObject(object, 1);
        }
    }
}

void __cdecl StoreProgressAICharacter(LEVEL_PROGRESS *progress) {
    LEVEL_AI_PROGRESS_PC *saved = (LEVEL_AI_PROGRESS_PC *)progress;
    GAMEOBJECT *object;
    i32 index;

    if (saved == NULL) {
        return;
    }
    object = Obj;
    saved->disabled_ai_object_mask[0] = 0;
    saved->disabled_ai_object_mask[1] = 0;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.reset_mode == 4) {
            u64 bit = (u64)1 << index;
            saved->disabled_ai_object_mask[0] |= (u32)bit;
            saved->disabled_ai_object_mask[1] |= (u32)(bit >> 32);
        }
    }
}

void AICreatureResumeScript(GAMEOBJECT *object) {
    AISCRIPT_PC *script = object->ai_packet.script_processor.base_script;
    if (script != NULL) {
        AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, NULL, NULL, NULL,
                              0, script, script->base_state);
        object->ai_packet.script_processor.active_ref_count = 0;
    }
}

i32 __cdecl GetDefaultIdle(GAMEOBJECT *object) {
    i32 animation;
    PLAYER_BATARANG_IDLE_PC *batarang;
    void **actions;
    void *idle, *standard;
    if (((PLAYER_IDLE_CHARACTER_PC *)object->character_data->game_character)->use_standard_idle == 0 &&
        (object->character_data->model_flags & 0x80) != 0) {
        animation = 0x76;
    } else {
        animation = 0x19;
    }
    batarang = (PLAYER_BATARANG_IDLE_PC *)object->batarang;
    if (batarang != NULL && batarang->active != 0) {
        return 0x97;
    }
    actions = object->character_model->action_animations;
    standard = actions[1];
    if (standard != NULL && actions[animation] == NULL) {
        return 1;
    }
    idle = actions[animation];
    if (idle != NULL && standard == NULL) {
        return animation;
    }
    if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) && idle != NULL) {
        return animation;
    }
    return 1;
}

void __cdecl ResetCharacterIdle(GAMEOBJECT *object, i32 mode, i32 animation) {
    CHARACTERANIM_CONFIG_PC *info;
    i32 minimum, maximum;
    if (animation == 0x97) {
        return;
    }
    object->idle_animation = (i16)animation;
    object->idle_animation_time = 0.0f;
    minimum = 0;
    if (object->character_model->action_animations[(i16)animation] != NULL) {
        info = object->character_model->animation_configs[(i16)animation];
        minimum = info->minimum_repetitions;
    }
    if (minimum > 0) {
        maximum = info->maximum_repetitions;
        if (maximum > minimum) {
            minimum += qrand() / (0xffff / (maximum - minimum) + 1);
        }
        object->idle_animation_limit =
            (f32)((f64)AnimDuration((i16)object->id, object->idle_animation, 0.0f, 0.0f, 0) * (f64)minimum -
                  (f64)animduration_blendouttime);
    } else {
        object->idle_animation_limit = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 7.0 + 8.0);
    }
    if (mode > 0) {
        object->idle_total_time = 0.0f;
        if (mode > 1) {
            object->previous_idle_animation = -1;
        }
    }
}

static void NewCharacterIdle(GAMEOBJECT *object, i32 default_idle) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character = object->character_data;
    i32 alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                            (character->model_flags & 0x80) != 0
                        ? 118
                        : 25;
    i32 candidates[233];
    i32 count = 0;
    i32 animation, repetitions, minimum, maximum;
    CHARACTERANIM_CONFIG_PC *info;
    for (animation = 0; animation < 233; ++animation) {
        if (object->character_model->action_animations[animation] == NULL) {
            continue;
        }
        info = object->character_model->animation_configs[animation];
        if ((info->flags & 0x10) == 0) {
            continue;
        }
        if (default_idle == alternate) {
            if ((info->flags & 0x800) == 0) {
                continue;
            }
        } else if ((info->flags & 0x800) != 0) {
            continue;
        }
        candidates[count++] = animation;
    }
    if (count > 0) {
        do {
            state->idle_animation = (i16)candidates[count > 1 ? qrand() / (65535 / count + 1) : 0];
        } while (count > 1 && state->idle_animation == state->previous_idle_animation);
        info = object->character_model->animation_configs[state->idle_animation];
        minimum = info->minimum_repetitions;
        repetitions = 1;
        if (minimum > 0) {
            maximum = info->maximum_repetitions;
            repetitions = minimum;
            if (maximum > 0 && maximum > minimum) {
                repetitions += qrand() / (65535 / (maximum - minimum) + 1);
            }
        }
        state->previous_idle_animation = state->idle_animation;
        if (repetitions > 1 && (object->character_model->animation_configs[state->idle_animation]->flags & 2) == 0) {
            repetitions = 1;
        }
        state->idle_animation_time = 0.0f;
        state->idle_animation_limit =
            AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * repetitions -
            animduration_blendouttime;
    } else {
        ResetCharacterIdle(object, 0, default_idle);
    }
}

void __cdecl UpdateCharacterIdle(GAMEOBJECT *object) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character;
    i32 alternate, default_idle;
    i16 requested;
    if (object->character_model == NULL) {
        return;
    }
    character = object->character_data;
    alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                        (character->model_flags & 0x80) != 0
                    ? 118
                    : 25;
    requested = object->anim_packet.requested_animation;
    if (requested != 1 && requested != alternate) {
        default_idle = GetDefaultIdle(object);
    } else if (requested == 1 && object->anim_packet.previous_animation == alternate) {
        default_idle = 1;
    } else if (requested == alternate && object->anim_packet.previous_animation == 1) {
        default_idle = alternate;
    } else {
        default_idle = -1;
    }
    if (default_idle != -1) {
        ResetCharacterIdle(object, 1, default_idle);
        return;
    }
    if (requested == 1) {
        state->idle_total_time += FRAMETIME;
        state->idle_animation_time += FRAMETIME;
        if (state->idle_animation_time >= state->idle_animation_limit) {
            if (state->idle_animation == requested) {
                NewCharacterIdle(object, 1);
            } else {
                ResetCharacterIdle(object, 1, 1);
            }
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    if (requested == alternate) {
        state->idle_total_time += FRAMETIME;
        state->idle_animation_time += FRAMETIME;
        if (state->idle_animation_time >= state->idle_animation_limit) {
            if (state->idle_animation == alternate) {
                NewCharacterIdle(object, alternate);
            } else {
                ResetCharacterIdle(object, 1, alternate);
            }
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

static void GunshipKeepOnScreenPc(GAMEOBJECT *object) {
    f32 push_speed = (f32)((f64)object->character_data->game_character->run_speed * 5.0);
    f32 count = 0.0f;
    NUVEC normal, response;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x6000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0xa000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x2000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[4].point, &PlayPlanePc[4].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0xe000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (count > 0.0f) {
        count = 1.0f / count;
        object->movement_direction_x = response.x * count;
        object->movement_direction_z = response.z * count;
    }
}

void __cdecl KeepOnScreen(GAMEOBJECT *object) {
    f32 previous_keep_time = object->field_da8;
    i32 controlled = 0, index, two_players, constrained;
    PLAYERCHARACTERCONFIG_PC *character;
    f32 push_distance;
    NUVEC movement;
    object->field_da8 = 0.0f;
    if (newgamecam != 0 || (object->flags_ef4 & 0x10000000) != 0) {
        return;
    }
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *controller = Player[index];
        if (controller != NULL && controller->character_context == 0x51 && controller->techno != NULL &&
            controller->techno->controlled_object == object) {
            controlled = 1;
        }
    }
    if (!(object->flags_1fc & 0x80) && !controlled) {
        return;
    }
    if (object->dead != 0 || object->character_context == 0x2b) {
        return;
    }
    if (!(object->flags_ef0 & 0x40000)) {
        return;
    }
    if (object->character_context == 0x0f || object->character_context == 0x47 || object->character_context == 0x1f ||
        object->character_context == 0x46 || object->character_context == 0x51) {
        return;
    }
    if (MiniCutCam != 0 || GetMenuID() != -1) {
        return;
    }
    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && GameCam->mode == 0x0b) {
        GunshipKeepOnScreenPc(object);
        return;
    }
    if (VehicleArea == 0) {
        if (object == Player[0] && Player[1] != NULL &&
            (!(((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) || ((GAMEOBJECT *)Player[1])->character_context == 0x2b)) {
            return;
        }
        if (object == Player[1] && Player[0] != NULL &&
            (!(((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) || ((GAMEOBJECT *)Player[0])->character_context == 0x2b)) {
            return;
        }
    }
    two_players = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) && Player[1] != NULL &&
                  (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80);
    character = object->character_data->game_character;
    push_distance = VehicleArea != 0 ? character->walk_speed : character->run_speed;
    movement = object->velocity;
    constrained = 0;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[0].point, &PlayPlanePc[0].normal, NULL, 0.0f, NULL)) {
        Surface_Deflect(&PlayPlanePc[0].normal, &movement, &movement, 0);
        movement.x += PlayPlanePc[0].normal.x * push_distance;
        movement.z += PlayPlanePc[0].normal.z * push_distance;
        constrained = 1;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL,
                        -object->bounds_radius, NULL)) {
        Surface_Deflect(&PlayPlanePc[2].normal, &object->velocity, &movement, 0);
        movement.x += PlayPlanePc[2].normal.x * push_distance;
        movement.z += PlayPlanePc[2].normal.z * push_distance;
        constrained = 1;
    } else if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL,
                               -object->bounds_radius, NULL)) {
        Surface_Deflect(&PlayPlanePc[1].normal, &object->velocity, &movement, 0);
        movement.x += PlayPlanePc[1].normal.x * push_distance;
        movement.z += PlayPlanePc[1].normal.z * push_distance;
        constrained = 1;
    }
    if ((VehicleArea != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) || KEEPONSCREEN_SIDESONLY == 0 ||
        (two_players && abs(RotDiff(0, ((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->pitch)) > 0x2000)) {
        if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[4].point, &PlayPlanePc[4].normal, NULL,
                            -object->bounds_half_height, NULL)) {
            index = !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? 5 : 0;
        } else if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL,
                                   -object->bounds_half_height, NULL)) {
            index = !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? 0 : 5;
        } else {
            index = -1;
        }
        if (index != -1) {
            Surface_Deflect(&PlayPlanePc[index].normal, &object->velocity, &movement, 0);
            movement.x += PlayPlanePc[index].normal.x * push_distance;
            movement.z += PlayPlanePc[index].normal.z * push_distance;
            constrained = 1;
        }
    }
    if (constrained) {
        object->movement_direction_x = movement.x;
        object->movement_direction_z = movement.z;
        object->field_da8 = previous_keep_time + FRAMETIME;
        SpecialMove_Cancel(object);
    } else {
        object->field_da8 = 0.0f;
    }
}

void __cdecl MovePlayerSpline(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    f32 speed;
    f32 distance;
    f32 hover;
    NUVEC position;
    NUVEC offset;
    u32 pitch;
    u32 angle;
    i32 diff;
    i32 roll;
    i32 target_roll;

    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->previous_movement_angle = object->field_0x276;
    object->flags_ef0 &= ~0x4000;

    if (object->model_draw_result != 0) {
        object->field_f10 = 0.0f;
    } else {
        object->field_f10 += FRAMETIME;
    }
    if (object->character_context == 0x2b) {
        object->turn_braking += FRAMETIME;
    }

    if ((f64)object->run_speed_override != MoveSplineRunSpeedSentinel) {
        speed = object->run_speed_override;
    } else {
        speed = object->character_data->game_character->run_speed;
    }
    distance = FRAMETIME * speed;
    MoveSplinePosition(&object->movement_spline_position, distance);
    PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position,
                     (u16 *)&angle, (u16 *)&pitch, object->movement_spline_position.looping);
    if (distance < 0.0f) {
        angle += 0x8000;
        pitch = (u16)-pitch;
    }
    object->movement_facing_angle = angle;
    object->field_0x276 = object->facing_angle = SeekRot(object->facing_angle, angle, spline_seek_ang);
    object->pitch_angle = SeekRot(object->pitch_angle, -pitch, spline_seek_ang);

    if (WORLD->current_level != PODSPRINTA_LDATA || speed > 0.0f) {
        object->movement_spline_lateral_speed = SeekValF(object->movement_spline_lateral_speed,
                                                         -object->movement_spline_offset.x, spline_xoffset_speed_seek);
    }
    object->movement_spline_offset.x += object->movement_spline_lateral_speed * FRAMETIME;
    if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
        object->movement_spline_offset.z != 0.0f) {
        NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
        NuVecRotateY(&offset, &offset, object->field_0x276);
        NuVecAdd(&position, &position, &offset);
    }

    if ((object->flags_ef4 & 0x1000) != 0) {
        hover = GetVehicleHoverHeight(object, NULL);
        hover += object->collision_position.y - object->lower_position.y;
        if (WORLD->current_level == PODSPRINTA_LDATA) {
            NewTerrPlatformsOff();
        }
        object->floor_height = GameShadow(object, &position, 5.0f, 0x1f);
        if (object->floor_height == 2000000.0) {
            position.y = SeekValF(object->position.y, hover, 5.0f);
        } else {
            hover += object->floor_height;
            position.y = SeekValF(object->position.y, hover, 5.0f);
            if (position.y < object->floor_height) {
                position.y = object->floor_height;
            }
        }
        if (WORLD->current_level == PODSPRINTA_LDATA) {
            position.y += object->movement_spline_offset.y;
        }
    }
    if (object->delayed_turn_timer > 0.0f && object->character_context != 0x2a && object->character_context != 0x36 &&
        object->character_context != 0x3a) {
        object->delayed_turn_timer -= FRAMETIME;
    }
    if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
        position.y += VehicleTurnOrLoopOffset(object);
    }

    SeekVec(&object->position, &object->position, &position, spline_seek_pos);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    object->flags_107a = 0;

    if ((object->character_data->game_character->flags_090 & 1) != 0) {
        roll = 0;
        if (object->character_context != 0x2a && object->character_context != 0x36 &&
            object->character_context != 0x3a) {
            diff = RotDiff(object->previous_movement_angle, object->field_0x276);
            roll = (i32)(diff / FRAMETIME);
            roll += (i32)(object->movement_spline_lateral_speed * spline_xoffset_speed_roll);
            if (roll < -0x10000) {
                roll = -0x10000;
            } else if (roll > 0x10000) {
                roll = 0x10000;
            }
        }
        target_roll = roll / 4;
        if (target_roll < -0x2000) {
            target_roll = -0x2000;
        } else if (target_roll > 0x2000) {
            target_roll = 0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, target_roll, MoveSplineTurnRate);
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

void __cdecl MovePlayer(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    i32 analog_x;
    i32 analog_y;
    i32 menu_id, index, waggle;
    u32 buttons;
    f32 x, z, magnitude, inverse, run_speed, walk_speed;
    NUVEC input, axis;
    NUANG angle;

    object->flags_e20 &= ~0x200000;
    if (pad->bound_pad != NULL) {
        analog_x = pad->bound_pad->analog_left_x;
        analog_y = pad->bound_pad->analog_left_y;
    } else {
        analog_x = analog_y = 0;
    }
    if (pad->input_magnitude != pad->previous_input_magnitude) {
        pad->runtime_flags |= 0x40;
    }
    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->flags_ef0 &= ~0x4000;
    object->previous_movement_angle = object->field_0x276;
    if (object->model_draw_result != 0) {
        object->field_f10 = 0.0f;
    } else {
        object->field_f10 += FRAMETIME;
    }
    if (object->character_context == 0x2b) {
        object->turn_braking += FRAMETIME;
    }
    if ((object->flags_ef4 & 0x4000000) == 0) {
        if (!object->dead) {
            pad->runtime_flags &= ~8;
            pad->waggle_magnitude = 0.0f;
            object->field_106e = -1;
            object->flags_ef0 &= ~0x400;
            menu_id = GetMenuID();
            if ((object->flags_1fc_word & 0x80) != 0 && (object->flags_1fc_word & 0x100) == 0) {
                if (FadeSys.fade > 0.0f || newgamecam != 0 || MiniCutCam == 2 || (menu_id != -1 && menu_id != 0x0e)) {
                    input.x = input.z = 0.0f;
                    pad->input_mode = 1;
                    pad->input_direction_x = pad->input_direction_z = pad->input_magnitude = 0.0f;
                } else if (object->pad == ViewCamGetGamePad()) {
                    pad->input_magnitude = 0.0f;
                } else {
                    buttons = pad->buttons_held;
                    if ((buttons & (GAMEPAD_DUP | GAMEPAD_DDOWN | GAMEPAD_DLEFT | GAMEPAD_DRIGHT)) != 0) {
                        x = (buttons & GAMEPAD_DLEFT) != 0 ? -127.5f : (buttons & GAMEPAD_DRIGHT) != 0 ? 127.5f : 0.0f;
                        z = (buttons & GAMEPAD_DDOWN) != 0 ? -127.5f : (buttons & GAMEPAD_DUP) != 0 ? 127.5f : 0.0f;
                        if (x != 0.0f || z != 0.0f) {
                            pad->input_mode = 1;
                        }
                    } else {
                        x = (f32)((f64)analog_x - 127.5);
                        z = (f32) - ((f64)analog_y - 127.5);
                        if (x * x + z * z < 1806.25) {
                            x = z = 0.0f;
                        } else if (x != 0.0f || z != 0.0f) {
                            pad->input_mode = 2;
                        }
                    }
                    x = (f32)(x * (f64)0.007843137718737125f);
                    z = (f32)(z * (f64)0.007843137718737125f);
                    angle = NuAtan2D(x, z);
                    axis.x = axis.y = 0.0f;
                    axis.z = MovePlayerFabs(x) > MovePlayerFabs(z) ? MovePlayerFabs(x) : MovePlayerFabs(z);
                    NuVecRotateY(&input, &axis, angle);
                    magnitude = NuFsqrt(input.x * input.x + input.z * input.z);
                    if (object == Player[0]) {
                        index = 0;
                    } else if (object == Player[1]) {
                        index = 1;
                    } else {
                        goto skip_pad_history;
                    }
                    {
                        PlayerPadPreviousSpeed[index] = PlayerPadCurrentSpeed[index];
                        PlayerPadPreviousAngle[index] = PlayerPadCurrentAngle[index];
                        PlayerPadCurrentSpeed[index] = pad->input_magnitude;
                        PlayerPadCurrentAngle[index] = pad->input_angle;
                    }
                skip_pad_history:
                    if ((f64)0.2f > magnitude) {
                        pad->input_magnitude = 0.0f;
                    } else {
                        if ((object->character_data->model_flags & 0x20) != 0 &&
                            ((object->player_index != -1 && object->vehicle_secondary_hp < 3) ||
                             (object->player_index == -1 && object->current_hp < 3))) {
                            pad->input_magnitude = object->character_data->game_character->walk_speed;
                        } else if (magnitude < 0.5f && (object->character_data->game_character->flags_090 & 8) == 0) {
                            pad->input_magnitude = object->character_data->game_character->tiptoe_speed;
                        } else if (magnitude < (f64)0.8f) {
                            pad->input_magnitude = object->character_data->game_character->walk_speed;
                        } else {
                            pad->input_magnitude = object->character_data->game_character->run_speed;
                        }
                    }
                    pad->input_direction_z = input.x;
                    pad->input_direction_x = input.z;
                    if (pad->input_magnitude > 0.0f) {
                        pad->input_angle = NuAtan2D(pad->input_direction_z, input.z);
                    }
                    waggle = GamePad_Waggle(object->pad);
                    ((MOVEPLAYER_PAD_BITS_PC *)&pad->runtime_flags)->waggle = waggle;
                    pad->waggle_magnitude = object == Player[0] || object == Player[1] ? GamePad_Rotate(object) : 0.0f;
                }
            } else {
                if ((object->flags_ef0 & 0x10) != 0) {
                    pad->input_magnitude = 0.0f;
                    object->pad->buttons_pressed = object->pad->buttons_held = 0;
                } else if (object->ai_packet.movement_stopped == 1) {
                    pad->input_direction_x = pad->input_direction_z = pad->input_magnitude = 0.0f;
                    pad->input_mode = 1;
                } else if (object->character_context == 0x17 && (object->field_ef8 & 1) == 0) {
                    pad->input_magnitude = 0.0f;
                } else {
                    axis.x = object->ai_packet.movement_position.x - object->position.x;
                    axis.z = object->ai_packet.movement_position.z - object->position.z;
                    if (axis.x == 0.0f && axis.z == 0.0f) {
                        pad->input_angle = 0;
                        pad->input_direction_x = pad->input_direction_z = pad->input_magnitude = 0.0f;
                    } else {
                        magnitude = NuFsqrt(axis.x * axis.x + axis.z * axis.z);
                        pad->input_magnitude = magnitude / ai_moveradius;
                        if (pad->input_magnitude > 1.0f) {
                            pad->input_magnitude = 1.0f;
                        }
                        inverse = 1.0f / magnitude;
                        pad->input_direction_x = inverse * axis.x;
                        pad->input_direction_z = inverse * axis.z;
                        pad->input_angle = NuAtan2D(pad->input_direction_x, pad->input_direction_z);
                        pad->input_angle = NuAngSub(pad->input_angle, GameCam->input_yaw);
                    }
                    pad->input_mode = 1;
                }
                if (object->context_target_position != NULL) {
                    pad->input_magnitude = 0.0f;
                } else {
                    run_speed = object->run_speed_override != 1000000000.0
                                    ? object->run_speed_override
                                    : object->character_data->game_character->run_speed;
                    walk_speed = object->walk_speed_override != 1000000000.0
                                     ? object->walk_speed_override
                                     : object->character_data->game_character->walk_speed;
                    if ((object->ai_packet.flags & 0x40000) != 0) {
                        pad->input_magnitude = 0.0f;
                    } else {
                        pad->input_magnitude *= run_speed;
                    }
                    if ((object->ai_packet.flags & 0x80000) == 0 &&
                        pad->input_magnitude < object->character_data->game_character->tiptoe_speed * 0.5) {
                        pad->input_magnitude = 0.0f;
                    }
                    if ((object->ai_packet.flags & 8) == 0) {
                        if (object->ai_packet.goal_speed_mode != 0) {
                            if (object->ai_packet.goal_speed_mode == 1) {
                                if (pad->input_magnitude > walk_speed) {
                                    pad->input_magnitude = walk_speed;
                                }
                            } else if (object->ai_packet.goal_speed_mode == 2) {
                                if (pad->input_magnitude > object->character_data->game_character->tiptoe_speed) {
                                    pad->input_magnitude = object->character_data->game_character->tiptoe_speed;
                                }
                            }
                        }
                    }
                    if ((object->flags_1f8 & 0x400) == 0 && (object->portal_flags & 0x2000000) == 0 &&
                        (object->field_f10 >= OFFSCREEN_CATCHUP_TIME ||
                         (object->model_draw_result == 0 && (object->tag_flags & 2) == 0 &&
                          drop_back_in_timer > 0.0f)) &&
                        (pad->input_magnitude + (f64)0.001f >= walk_speed || object->id == (u16)id_SPEEDERBIKE)) {
                        object->flags_ef0 |= 0x400;
                    }
                }
                object->pad->operator_data = object->ai_packet.movement_look_target;
            }
            if ((object->flags_1fc & 0x80) != 0 && menu_id == 0x0e && (object == Player[0] || object == Player[1])) {
                index = object != Player[0];
                Hub_PadSpeed[index] = pad->input_magnitude;
                Hub_PadAngle[index] = pad->input_angle;
                pad->input_mode = 1;
                pad->input_magnitude = 0.0f;
                pad->input_direction_x = pad->input_direction_z = 0.0f;
            }
            if (pad->input_magnitude > 0.0f) {
                pad->animation_input_magnitude = pad->input_magnitude;
            }
            if (object->character_context == 0x3b || object->character_context == 0x39) {
                return;
            }
            object->movement_mode = 2;
            ShoveSystemCheckGameObject(object);
            if (object->id == (u16)id_DRAGBOMB) {
                MovePlayer_ROLLING(object);
            } else if (!MovePlayer_TWIST(object) && !MovePlayer_CIRCLE(object) && !MovePlayer_GUNSHIPIN(object) &&
                       !MovePlayer_POD(object)) {
                if ((object->character_data->model_flags & 0x2000) != 0) {
                    MovePlayer_VEHICLEDIRECTIONAL(object);
                } else {
                    MovePlayer_DIRECTIONAL(object);
                }
            }
            if (LIFTPLAYER != 0 && menu_id == -1 && !object->dead && object->character_context != 0x2b &&
                (object->character_data->game_character->flags_090 & 0x80) == 0 && (object->flags_1fc & 0x80) != 0 &&
                (pad->buttons_held & GAMEPAD_LIFT) != 0 && pad != ViewCamGetGamePad()) {
                if (object->character_context == 0x51 && object->techno != NULL && object->techno->target_mode == 1 &&
                    object->techno->controlled_player != NULL) {
                    object->techno->controlled_player->position.y += FRAMETIME * 1.5;
                    object->techno->controlled_player->velocity.y = 0.0f;
                    object->techno->controlled_player->field_281 = 0;
                    object->techno->controlled_player->lift_clear_state = 0;
                    if (object->techno->controlled_player->character_context == 0x33) {
                        object->techno->controlled_player->character_context = -1;
                    }
                } else {
                    object->position.y += FRAMETIME * 1.5;
                    object->velocity.y = 0.0f;
                    object->field_281 = 0;
                    object->lift_clear_state = 0;
                    if (object->character_context == 0x33) {
                        object->character_context = -1;
                    }
                }
            }
            object->flags_1fc_word |= 0x10000;
            object->previous_velocity.x = object->velocity.x;
            object->previous_velocity.y = object->velocity.y;
            object->previous_velocity.z = object->velocity.z;
            APIObjectVelocities(object);
            GameObjectOrigin(object);
        }
    } else {
        APIObjectVelocities(object);
        GameObjectOrigin(object);
    }
}

static void TerrainPlayerResetContactPc(GAMEOBJECT *object) {
    object->jump_animation_state = 0;
    object->raw_surface_id_pc = 0;
    object->surface_normal = v010;
    object->water_height = 2000000.0f;
    object->roof_height_pc = 2000000.0f;
    object->field_283 = -1;
    object->roof_surface_id_pc = (u8)-1;
    object->terrain_platform_id = -1;
}

static i32 TerrainKillPlayer(GAMEOBJECT *object, i32 surface, NUVEC *normal) {
    u32 flags;

    flags = TerSurface[surface].flags;
    if ((flags & 1) == 0 &&
        ((flags & 0x4000) == 0 || (object->character_data->model_flags & 0x10) != 0 ||
         (object->character_data->game_character->flags_090 & 0x8000) != 0) &&
        ((flags & 0x8000) == 0 || ((i8)object->field_280 != -1 && !(normal->y > 0.0f)))) {
        return 0;
    }
    InstantKillParts(object, 1, 0.0f);
    if (((object->flags_1fc_word & 0x80) != 0) && BonusWinner == -1) {
        i32 coins;

        coins = LoseCoins(object, 1);
        AddPickups(coins, 0, 0, 0, &object->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, object, 1, 0);
    }
    KillPlayer(object, 2, 1, NULL);
    return 1;
}

void __cdecl TerrainPlayer(GAMEOBJECT *object) {
    NUVEC forward;
    GAMEPAD_PC *entry_pad;
    i32 entry_underwater;
    i32 entry_intersects_water;
    i32 entry_menu_id;
    i32 entry_platform;
    TERRAIN_PLAYER_IMPACT_PC impact_records[8];
    i32 impact_count;
    f32 movement_scale;
    i32 terrain_moved;

    movement_scale = 1.0f;
    entry_pad = object->pad;
    entry_underwater = object->is_underwater;
    entry_intersects_water = object->water_intersects;
    entry_menu_id = GetMenuID();
    entry_platform = object->supporting_platform_id;

    terrain_moved = 0;
    impact_count = -1;

    if (object->dead == 0) {
        if (object->takeover_source != NULL && object->character_context == 0x3b) {
            TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
        } else {
            GAMEOBJECT *api;
            i32 terrain_mask;
            f32 entry_vertical_velocity;
            const AIPATHINFO_PC *path_info;
            i32 ai_controlled;
            i32 special_path_endpoints;
            i32 shadow_grounding;
            i32 retain_floor;
            NUVEC incoming_velocity;
            i32 player_edge_probe;
            f32 movement_threshold;
            f32 speed_squared;

            api = object;
            terrain_mask = GetSurfaceInfoDefaultMaskPc.word;
            if ((api->character_data->model_flags & 0x10) == 0 &&
                (api->character_data->game_character->flags_090 & 0x8000) == 0) {
                terrain_mask |= GetSurfaceInfoRuntimeMaskPc.word;
            }
            ai_controlled = api->flags_1f8 & 0x400;
            if (ai_controlled && object->id != (u16)id_DRAGBOMB &&
                (WORLD->area != HOTHBATTLE_ADATA || api->character_data->game_character->field_028 != 0.0f)) {
                terrain_mask |= 0x40;
            }
            entry_vertical_velocity = api->velocity.y;
            object->flags_e20 &= ~8u;
            path_info = &object->ai_packet.path_info;
            special_path_endpoints = 0;
            if (path_info->path != NULL && path_info->connection != NULL) {
                const AIPATHNODE_PC *nodes;

                nodes = path_info->path->nodes;
                if ((nodes[path_info->connection->node_indices[path_info->direction == 0]].runtime_flags |
                     nodes[path_info->connection->node_indices[path_info->direction]].runtime_flags) &
                    0x82) {
                    special_path_endpoints = 1;
                }
            }
            if (object == CarWashHack) {
                shadow_grounding = 1;
                retain_floor = 0;
            } else if ((api->character_data->game_character->flags_090 & 0x80) != 0) {
                api->velocity.x = 0.0f;
                api->velocity.z = 0.0f;
                shadow_grounding = 1;
                retain_floor = 0;
            } else if ((api->flags_1fc_word & 0x40) == 0 &&
                       (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000) == 0) {
                i32 ordinary_ai_path;

                ordinary_ai_path =
                    ai_controlled && object->movement_spline_position.spline == NULL && (path_info->flags & 1) != 0 &&
                    (path_info->flags & 8) == 0 && object->character_context != 0x1c &&
                    object->context_target_position == NULL && !special_path_endpoints &&
                    Technos_FindControllingTechno(object) == NULL && (api->flags_1fc_word & 0x40000) == 0 &&
                    !AIPATHCNX_FLAG_SET(path_info->connection, original_traversal_flags, LEGO_AIPATHCNX_BLOCKAGE) &&
                    !AIPATHCNX_FLAG_SET(path_info->connection, traversal_flags, LEGO_AIPATHCNX_WALLSHUFFLE) &&
                    !AIPATHCNX_FLAG_SET(path_info->connection, traversal_flags, LEGO_AIPATHCNX_FULLTERRAIN) &&
                    (object->character_context != 0x5a || object->field_7b3 != 0);
                if (ordinary_ai_path) {
                    retain_floor = (api->flags_1fc_word & 0x10) != 0;
                    shadow_grounding = 1;
                } else {
                    retain_floor = ai_controlled ? 0 : (0x10 & api->flags_1fc_word) != 0;
                    if (0 != (object->flags_ef4 & 0x40)) {
                        object->flags_ef4 &= ~0x40u;
                        shadow_grounding = 1;
                    } else {
                        shadow_grounding = (api->flags_1fc_word & 8) != 0;
                    }
                }
            } else {
                shadow_grounding = 0;
                retain_floor = 0;
            }

            player_edge_probe = ((api->flags_1fc_word & 0x80) != 0) && object->spawn_protection_timer > 1.25f;

            if (VehicleArea == 0 && api->dead == 0 &&
                (api->field_281 != 0 ||
                 (player_edge_probe && object->character_context == -1 && object->jump_variant_state != 1))) {
                i32 walkable_floor;
                i32 probe_edge;

                walkable_floor = api->floor_height != 2000000.0f && object->surface_normal.y > NuTrigTable[0x238e];
                probe_edge = walkable_floor
                                 ? (player_edge_probe ||
                                    0 != ((U64_PAIR(&CInfo[object->character_context].flags) & 0x100000000ULL)))
                                 : 0;
                if (probe_edge && api->movement_direction_x == 0.0f && api->movement_direction_z == 0.0f &&
                    (api->velocity.x != 0.0f || api->velocity.z != 0.0f)) {
                    NUVEC next_position;
                    f32 next_floor;

                    next_position.x = api->position.x + api->velocity.x * FRAMETIME;
                    next_position.y = api->position.y;
                    next_position.z = api->position.z + api->velocity.z * FRAMETIME;
                    next_floor = GameShadow(object, &next_position, 5.0f, terrain_mask | 0x1f);
                    if (next_floor != 2000000.0f && ShadNorm.y > NuTrigTable[0x238e] &&
                        (player_edge_probe ? api->collision_min.y - next_floor : api->floor_height - next_floor) >
                            0.2f) {
                        api->velocity.z = 0.0f;
                        api->velocity.x = 0.0f;
                    }
                }
            }
            if (!(object->pad->input_magnitude > 0.0f) && object->character_context != 0x33 &&
                !(api->field_281 != 0 && (i8)api->jump_animation_state >= 0 && (i8)api->jump_animation_state < 32 &&
                  TerSurface[(i8)api->jump_animation_state].movement_scale < 1.0f) &&
                AnimSpeed(api->character_model, CurrentAnim(&api->anim_packet)) == 0.0f &&
                (api->character_data->model_flags & 0x800) == 0) {
                movement_threshold = api->character_data->game_character->stop_speed * api->origin_scale;
            } else {
                movement_threshold = 0.0f;
            }

            speed_squared = (api->velocity.x * api->velocity.x + api->velocity.y * api->velocity.y) +
                            api->velocity.z * api->velocity.z;
            if ((object->flags_ef0 & 0x20) == 0 && (object->flags_ef4 & 0x400000) != 0 && !special_path_endpoints &&
                object->ai_packet.path_target == NULL && !((api->flags_1fc_word & 0x80) != 0) &&
                (object->flags_e20 & 0x10000000) == 0 && object->jump_variant_state == 0 &&
                object->character_context != 0 && object->character_context != 0x1c && api->jump_animation_state != 8 &&
                (((u32)(GameTimer.update_count)) >= 2 || (api->flags_1f8 & 5) != 0) && (api->field_281 & 3) != 0 &&
                api->supporting_platform_id == -1 && movement_threshold * movement_threshold >= speed_squared &&
                object->character_context != 0x0f && object->character_context != 0x0b &&
                object->character_context != 0x1e) {
                api->flags_1fc_word |= 4;
            } else {
                api->flags_1fc_word &= ~4u;
            }
            incoming_velocity = api->velocity;
            object->contact_count = 0;
            if ((api->flags_1fc_word & 0x20) != 0 || (object->flags_e20 & 0x20) != 0 ||
                object->movement_spline_position.spline != NULL || object->move_override != NULL ||
                (object->character_context == 0x0f && (object->field_7b3 == 1 || object->field_7b3 == 0)) ||
                (object->character_context == 0x2c && object->field_7b3 == 0) ||
                (object->character_context == 0x47 && object->action_movement_state == 1)) {
                api->respawn_timer = 0.0f;
                object->flags_e20 |= 2;
                api->field_281 = 0;
                api->supporting_platform_id = -1;
                object->contact_material_word = 0;
                api->position.x += api->velocity.x * FRAMETIME;
                api->position.y += api->velocity.y * FRAMETIME;
                api->position.z += api->velocity.z * FRAMETIME;
            } else if (shadow_grounding || retain_floor || (WORLD->current_level->flags & 0x10) != 0) {
                api->respawn_timer = 0.0f;
                if ((api->flags_1fc_word & 4) == 0) {
                    f32 grounded_lower_bound;

                    object->flags_e20 |= 2;
                    api->supporting_platform_id = -1;
                    api->position.x += api->velocity.x * FRAMETIME;
                    api->position.y += api->velocity.y * FRAMETIME;
                    api->position.z += api->velocity.z * FRAMETIME;

                    if (shadow_grounding) {
                        if (object == CarWashHack) {
                            NewTerrPlatformsOff();
                        }
                        if (TerrainPlayerShadowFixPc != 0) {
                            NUVEC shadow_position;

                            shadow_position = api->collision_position;
                            shadow_position.y = api->initial_position.y > api->collision_position.y
                                                    ? api->initial_position.y
                                                    : api->collision_position.y;
                            api->floor_height = GameShadow(object, &shadow_position, 5.0f, terrain_mask | 0x1f);
                        } else {
                            api->floor_height = GameShadow(object, &api->collision_position, 5.0f, terrain_mask | 0x1f);
                        }
                        api->supporting_platform_id = ((i16)(NewShadowOnPlatform()));
                        if (api->floor_height != 2000000.0f) {
                            GetSurfaceInfo(object, 1, api->floor_height);
                        } else {
                            GetSurfaceInfo(object, 0, api->floor_height);
                            TerrainPlayerResetContactPc(object);
                        }
                        api->is_underwater = ((u8)(UnderWater(object)));
                        api->water_intersects = ((u8)(IntersectWater(object)));
                        if (api->supporting_platform_id == -1 && api->field_281 != 0 &&
                            object->terrain_platform_id != -1) {
                            api->supporting_platform_id = object->terrain_platform_id;
                        }
                    }
                    grounded_lower_bound = object->character_bottom * api->origin_scale;
                    if (api->floor_height != 2000000.0f && api->position.y + grounded_lower_bound < api->floor_height) {
                        api->velocity.y = 0.0f;
                        api->position.y = api->floor_height - grounded_lower_bound;
                    }

                    api->field_281 = 0;
                    if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                        api->field_281 |= 2;
                    }
                    if (api->collision_min.y <= api->floor_height) {
                        api->field_281 |= 1;
                    }
                    if ((api->field_281 & 1) != 0) {
                        object->contact_count = 1;
                        object->contact_position.x = api->position.x;
                        object->contact_position.y = api->floor_height;
                        object->contact_position.z = api->position.z;
                        object->contact_material[0] = api->jump_animation_state;
                        if (shadow_grounding) {
                            object->contact_normal = object->surface_normal;
                        } else {
                            object->contact_normal = v010;
                        }
                        object->terrain_impact_speed = -api->velocity.y;
                    } else {
                        object->contact_material_word = 0;
                    }
                }
            } else {
                f32 floor_height;
                i32 stopped_without_terrain;

                if ((api->flags_1fc_word & 4) != 0) {
                    api->velocity.x = api->velocity.y = api->velocity.z = 0.0f;
                } else {
                    NUVEC collision_position;
                    NUVEC movement;
                    i32 object_index;
                    i32 excluded_platforms[32];
                    i32 excluded_count;
                    TERRAIN_PLAYER_PLATFORM_SYS_PC *platforms;
                    i32 i;

                    collision_position.x = api->position.x;
                    collision_position.y = api->origin_scale * object->character_bottom + api->position.y;
                    collision_position.z = api->position.z;
                    object_index = ((u32)(((u32)(object)) - ((u32)(Obj)))) / sizeof(GAMEOBJECT);

                    NuVecScale(&movement, &api->velocity, FRAMETIME);

                    object->contact_material_word = 0;

                    if (object->use_model_origin > 0 && api->field_281 != 0 &&
                        (movement.x != 0.0f || movement.z != 0.0f) &&
                        !(WORLD->current_level == ENDORBATTLEC_LDATA && object->id == GameObjectRotationAtstId)) {
                        i32 deflection_mode;

                        deflection_mode = 0;
                        if (VehicleArea != 0 && (api->character_data->model_flags & 0x2000) != 0 &&
                            api->character_data->game_character->field_028 != 0.0f) {
                            deflection_mode = 2;
                        } else if (object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
                            deflection_mode = 1;
                        }
                        if (deflection_mode != 0) {
                            NUVEC deflected;

                            Surface_Deflect(&object->surface_normal, &movement, &deflected, deflection_mode);
                            if (deflected.y > movement.y) {
                                movement = deflected;
                                if (deflection_mode == 1) {
                                    object->flags_e20 |= 8;
                                }
                            }
                        }
                    }
                    TerrainSetPlatConnectTol(
                        (api->character_data->model_flags & 0x800) != 0 ? 0.0f : TerrainPlayerPlatConnectTolerancePc);
                    if ((object->ai_packet.flags & 0x80) != 0) {
                        IgnoreWallSplines = 1;
                    }
                    TerrainSetImpactData(impact_records, &impact_count, 8);
                    if ((object->flags_ef0 & 0x400) != 0) {
                        if (object->id == (u16)id_SPEEDERBIKE) {
                            if (object->ai_packet.goal_speed_mode == 1) {
                                movement_scale =
                                    TerrainPlayerBikeMovementScalePc / api->character_data->game_character->walk_speed;
                            } else if (object->ai_packet.goal_speed_mode == 2) {
                                movement_scale = TerrainPlayerBikeMovementScalePc /
                                                 api->character_data->game_character->tiptoe_speed;
                            } else {
                                movement_scale =
                                    TerrainPlayerBikeMovementScalePc / api->character_data->game_character->run_speed;
                            }
                        } else {
                            movement_scale =
                                TerrainPlayerOtherMovementScalePc / api->character_data->game_character->run_speed;
                        }
                        movement.x *= movement_scale;
                        movement.z *= movement_scale;
                    }
                    excluded_count = 0;
                    if (object->field_106e != -1) {
                        excluded_platforms[excluded_count++] = object->field_106e;
                    }
                    if (object->level_object_index != -1) {
                        excluded_platforms[excluded_count++] = object->level_object_index;
                    }
                    if (object->character_context == 0) {
                        GAMEOBJECT *target;

                        target = object->airborne_collision_target;
                        if (target != NULL && target->level_object_index != -1) {
                            excluded_platforms[excluded_count++] = target->level_object_index;
                        }
                    } else if (object->character_context == 0x58 && object->field_7b3 == 1) {
                        TERRAIN_PLAYER_BLOWUP_PC *blowup;

                        blowup = ((TERRAIN_PLAYER_BLOWUP_PC *)(object->grapple));
                        if (blowup->platform_id != -1) {
                            excluded_platforms[excluded_count++] = blowup->platform_id;
                        }
                    }
                    platforms = ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc));
                    if (platforms != NULL && VehicleArea == 0) {
                        i32 player;

                        player = ((api->flags_1fc_word & 0x80) != 0);
                        if (!player || object->character_context == 0x3d || object->takeover_source != NULL ||
                            object->character_context == 0x3b ||
                            (api->character_data->game_character->flags_090 & 0x8040) != 0) {
                            i32 i;

                            for (i = 0; platforms->count > i; ++i) {
                                GAMEOBJECT *platform_object;

                                platform_object = platforms->entries[i].object;
                                if (platform_object != NULL && ((object->flags_ef4 & 0x80000) == 0 || player)) {
                                    excluded_platforms[excluded_count++] = platform_object->level_object_index;
                                }
                            }
                        }
                    }
                    for (i = 0; i < excluded_count; ++i) {
                        PlatOnOff(excluded_platforms[i], 0);
                    }

                    NewTerrainScaleYMask(&collision_position, &movement, ((u8 *)(&object->field_1050)), object_index,
                                         entry_pad->input_magnitude > 0.0f ? 0.0f : movement_threshold * FRAMETIME,
                                         api->collision_radius, object->collision_y_scale, 0, 0, terrain_mask);
                    terrain_moved = 1;
                    IgnoreWallSplines = 0;
                    for (i = 0; i < excluded_count; ++i) {
                        PlatOnOff(excluded_platforms[i], 1);
                    }
                    if ((object->flags_ef0 & 0x400) != 0) {
                        movement.x /= movement_scale;
                        movement.z /= movement_scale;
                    }

                    api->supporting_platform_id = ((i16)(NewShadowOnPlatform()));

                    object->contact_count = ((u8)(TerrImpact));
                    if (object->contact_count != 0 && impact_count > 0) {
                        memcpy(&object->contact_position, &impact_records[impact_count - 1],
                               sizeof(TERRAIN_PLAYER_IMPACT_PC));
                        object->terrain_impact_speed = -NuVecDot(&object->contact_normal, &incoming_velocity);
                    } else {
                        object->contact_count = 0;
                    }
                    NuVecScale(&api->velocity, &movement, 1.0f / FRAMETIME);
                    api->position.x = collision_position.x;
                    api->position.z = collision_position.z;
                    api->position.y = collision_position.y - object->character_bottom * api->origin_scale;

                    if (api->dead == 0 && api->supporting_platform_id != -1) {
                        NUMTX *previous;
                        NUMTX *current;

                        previous = NULL;
                        current = NULL;
                        TerrainPlatGetMtx(api->supporting_platform_id, &previous, &current);
                        if (previous != NULL && current != NULL) {
                            NUVEC previous_forward, current_forward;
                            NUANG previous_heading;
                            NUANG current_heading;
                            NUANG rotation;

                            forward.x = 0.0f;
                            forward.y = 0.0f;
                            forward.z = 1.0f;

                            NuVecMtxRotate(&previous_forward, &forward, previous);
                            previous_heading = NuAtan2D(previous_forward.x, previous_forward.z);
                            NuVecMtxRotate(&current_forward, &forward, current);
                            current_heading = NuAtan2D(current_forward.x, current_forward.z);
                            rotation = NuAngSub(current_heading, previous_heading);
                            if (rotation != 0) {
                                api->field_0x276 = NuAngAdd(rotation, api->field_0x276);
                                api->facing_angle = NuAngAdd(rotation, api->facing_angle);
                                api->movement_facing_angle = NuAngAdd(rotation, api->movement_facing_angle);
                            }
                        }
                    }
                    if (api->dead == 0) {
                        i32 wall_impact;

                        wall_impact = 0;
                        if (object->contact_count != 0 && 0 == (api->flags_1fc_word & 0x84) &&
                            object->character_context != 0 && object->character_context != 0x43 &&
                            object->character_context != 0x45 && (object->flags_ef4 & 0x10000) == 0) {
                            i32 i;

                            for (i = 0; i < impact_count; ++i) {
                                if (impact_records[i].normal.y < 0.707f) {
                                    wall_impact = 1;
                                    break;
                                }
                            }
                        }
                        if (wall_impact) {
                            api->respawn_timer += FRAMETIME;
                        } else {
                            api->respawn_timer -= FRAMETIME * 0.333f;
                            if (api->respawn_timer < 0.0f) {
                                api->respawn_timer = 0.0f;
                            }
                        }
                    }
                }
                floor_height = (api->flags_1fc_word & 4) == 0
                                   ? GameShadow(object, &api->position, 5.0f, terrain_mask | 0x1f)
                                   : api->floor_height;
                stopped_without_terrain = 0;
                if (floor_height == 2000000.0f) {
                    object->flags_e20 &= ~2u;
                    if (NOTERRAINSTOP != 0) {
                        api->position.x = api->start_position.x;
                        api->position.z = api->start_position.z;
                        api->velocity.x = 0.0f;
                        api->velocity.z = 0.0f;
                        stopped_without_terrain = 1;
                    }
                } else {
                    object->flags_e20 |= 2;
                    if (terrain_moved) {
                        f32 top;
                        f32 bottom_offset;
                        f32 bottom;
                        f32 height;

                        top = object->character_top * api->origin_scale + api->position.y;
                        bottom_offset = object->character_bottom * api->origin_scale;
                        bottom = api->position.y + bottom_offset;
                        height = top - bottom;
                        if (floor_height > 0.1f * height + bottom && height + 0.01f >= (floor_height + height) - top) {
                            api->position.y = floor_height - bottom_offset;
                        }
                    }
                }
                if (!stopped_without_terrain && (api->flags_1fc_word & 4) == 0) {
                    api->floor_height = 2000000.0f;
                    if (floor_height != 2000000.0f) {
                        api->floor_height = floor_height;
                        GetSurfaceInfo(object, 1, floor_height);
                    } else {
                        GetSurfaceInfo(object, 0, floor_height);
                    }
                    if (api->supporting_platform_id == -1 && api->field_281 != 0 && object->terrain_platform_id != -1) {
                        api->supporting_platform_id = object->terrain_platform_id;
                    }
                    if ((0x8000 & api->character_data->game_character->flags_090) == 0 &&
                        object->reflection_height == 2000000.0f && object->terrain_platform_id != -1) {
                        f32 reflection_height;

                        reflection_height = FindReflectionNoPlatforms(&api->position);
                        if (reflection_height != 2000000.0f) {
                            object->reflection_height = reflection_height;
                            object->reflection_enabled = 2;
                        }
                    }
                }

                api->field_281 = object->field_1050 != 0 ? 1 : 0;
                if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                    api->field_281 |= 2;
                }
                if (api->field_281 != 0) {
                    object->flags_ef0 |= 0x2000000;
                }
            }

            if (api->field_281 != 0 && entry_platform == -1 && api->supporting_platform_id != -1 &&
                api->supporting_platform_id == object->terrain_platform_id && entry_vertical_velocity < 0.0f) {
                TERRAIN_PLAYER_GRABBER_PC *grabber;

                grabber = ((TERRAIN_PLAYER_WORLD_PC *)(WORLD))->grabber;
                if (grabber != NULL && grabber->platform_id == api->supporting_platform_id) {
                    if (grabber->platform_contact_timer <= 0.0f) {
                        grabber->platform_contact_timer = 0.25f;
                    }
                } else {
                    GAMEOBJECT *platform_owner;

                    platform_owner = CharPlatform_FindObjFromPlatID(
                        ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc)),
                        api->supporting_platform_id);
                    if (platform_owner != NULL && platform_owner->character_data->game_character->field_028 > 0.0f) {
                        platform_owner->velocity.y = entry_vertical_velocity * 0.5f + platform_owner->velocity.y;
                    }
                }
            }
            if (api->field_281 == 0 && entry_platform != -1 && api->supporting_platform_id == -1 &&
                object->terrain_platform_id == entry_platform) {
                TERRAIN_PLAYER_GRABBER_PC *grabber;

                grabber = ((TERRAIN_PLAYER_WORLD_PC *)(WORLD))->grabber;
                if (grabber != NULL && grabber->platform_id == entry_platform) {
                    if (grabber->platform_contact_timer <= 0.0f) {
                        grabber->platform_contact_timer = 0.25f;
                    }
                } else {
                    GAMEOBJECT *platform_owner;

                    platform_owner = CharPlatform_FindObjFromPlatID(
                        ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc)), entry_platform);
                    if (platform_owner != NULL && platform_owner->character_data->game_character->field_028 > 0.0f) {
                        platform_owner->velocity.y -= entry_vertical_velocity * 0.5f;
                    }
                }
            }

            if (VehicleArea == 0 && ((api->flags_1fc_word & 0x80) != 0) && api->supporting_platform_id != -1) {
                GAMEOBJECT *platform_owner;

                platform_owner = CharPlatform_FindObjFromPlatID(
                    ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc)), api->supporting_platform_id);
                if (platform_owner != NULL) {
                    const PLAYERCHARACTERCONFIG_PC *data;

                    data = platform_owner->character_data->game_character;
                    if ((data->flags_090 & 0x1000) != 0 && api->position.y < platform_owner->position.y &&
                        platform_owner->horizontal_velocity_magnitude > 0.5f * data->run_speed) {
                        StartFlatten(platform_owner, object);
                    }
                }
            }

            if (api->dead != 0) {
                if (api->field_281 != 0) {
                    api->velocity.y = 0.0f;
                    api->flags_1fc_word |= 4;
                }
                if ((api->flags_1fc_word & 4) == 0) {
                    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
                }
            } else {
                u32 held_buttons;
                u32 pressed_buttons;

                if (object->character_context == 0x2b) {
                    if (api->field_281 != 0 || api->model_draw_result == 0 || object->turn_braking >= 5.0f ||
                        (object->wall_run_state == 3 && object->contact_count != 0)) {
                        if (api->model_draw_result != 0) {
                            InstantKillParts(object, 0, 0.0f);
                        }
                        KillPlayer(object, 3, 1, NULL);
                        if (WORLD->current_level == CRUISERD_LDATA && object->wall_run_state == 3) {
                            MiscTime = 0.1f;
                        }
                    }
                } else if ((api->flags_1fc_word & 4) == 0 && gone_through_door_to_new_level == 0) {
                    TERRAIN_PLAYER_IMPACT_PC *impacts;
                    i32 count;

                    if (impact_count == -1) {
                        count = 1;
                        impacts = (TERRAIN_PLAYER_IMPACT_PC *)&object->contact_position;
                    } else {
                        impacts = impact_records;
                        count = impact_count;
                    }
                    if (CannotKill(object) == 0 && api->dead == 0 &&
                        (VehicleArea == 0 || ((api->flags_1fc_word & 0x80) != 0)) &&
                        (object->flags_ef0 & 0x40000) != 0 && (object->portal_flags & 0x40000) == 0 &&
                        object->character_context != 0x5d) {
                        if (object->contact_count != 0) {
                            i32 i;

                            for (i = 0; i < count; ++i) {
                                i32 surface;
                                NUVEC *normal;

                                surface = ((i8)(impacts[i].material[0]));
                                normal = &impacts[i].normal;
                                if (((u32)(surface)) > 31) {
                                    continue;
                                }
                                if (WORLD->current_level == BONUS_GUNSHIPA_LDATA ||
                                    (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA &&
                                     GameTimer.time_elapsed >= 1.0f)) {
                                    NewRumble(object->pad->bound_pad, (((f32)(qrand())) * (1.0f / 65535.0f)) * 0.5f, 0);
                                    if (api->model_draw_result == 0 && object->character_context != 0x23) {
                                        if (((api->flags_1fc_word & 0x80) != 0)) {
                                            LoseCoins(object, 2);
                                        }
                                        KillPlayer(object, 2, 1, NULL);
                                        break;
                                    }
                                } else if (((api->character_data->model_flags & 0x2000) != 0 ||
                                            (api->character_data->game_character->flags_090 & 0x04000000) != 0) &&
                                           api->field_283 == 6) {
                                    break;
                                }
                                if ((WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA) &&
                                    TerrainKillPlayer(object, surface, normal) != 0) {
                                    if (WORLD->current_level == ASTEROIDCHASEC_LDATA) {
                                        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
                                    }
                                    break;
                                }
                                if ((TerSurface[surface].flags & 0x80) != 0 &&
                                    WORLD->current_level != BONUS_GUNSHIPA_LDATA) {
                                    NewRumble(object->pad->bound_pad, 0.6f, 0);
                                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                    if (object->origin_scale_override != 1.0f && CannotKill(object) == 0) {
                                        ObjHitObj(NULL, object, 1, 0, 0, 1);
                                    }
                                    break;
                                }
                            }
                        } else if (api->field_281 != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) {
                            i32 surface;

                            surface = ((i8)(api->jump_animation_state));
                            if (((u32)(surface)) <= 31) {
                                TerrainKillPlayer(object, surface, &object->surface_normal);
                            }
                        }
                    }
                    if (CannotKill(object) == 0 && (object->portal_flags & 0x40000) == 0 && api->dead == 0 &&
                        api->water_height != 2000000.0f) {
                        i32 layer;

                        layer = ((i8)(api->field_283));
                        if ((layer == 3 ||
                             (layer == 9 && object->id == (u16)id_TRACTOR && WORLD->area != NULL &&
                              WORLD->area == DAGOBAH_ADATA) ||
                             (TerLayer[layer].flags & 0x20) != 0 ||
                             ((api->character_data->model_flags & 0x2000) == 0 &&
                              (api->character_data->game_character->flags_090 & 0x04000000) == 0)) &&
                            NoLayerKill(object) == 0 && object->character_context != 0x5d) {
                            u32 layer_flags;

                            layer_flags = TerLayer[((i8)(api->field_283))].flags;
                            if ((((layer_flags & 1) != 0 || (api->field_283 == 9 && object->id == (u16)id_TRACTOR &&
                                                             WORLD->area != NULL && WORLD->area == DAGOBAH_ADATA)) &&
                                 api->position.y < api->water_height &&
                                 !(VehicleArea != 0 && BonusArea != 0 &&
                                   api->character_data->game_character->field_028 > 0.0f) &&
                                 (((api->flags_1fc_word >> 7) & 1) != 0 ||
                                  (object->character_context != 0x47 && object->character_context != 0x46))) ||
                                ((layer_flags & 0x20) != 0 && api->collision_max.y > api->water_height &&
                                 (((api->flags_1fc_word >> 7) & 1) != 0 ||
                                  (object->character_context != 0x47 && object->character_context != 0x46)))) {
                                if (object->doomed_escape_locator != NULL && ((api->flags_1fc_word >> 7) & 1) == 0) {
                                    if ((object->flags_ef0 & 0x800) != 0) {
                                        ObjHitObj(NULL, object, 1, 0, 0, 1);
                                    }
                                    if (api->dead == 0) {
                                        StartBigJump(object, &object->doomed_escape_locator->position, 0, 1.0f, 1.0f,
                                                     0);
                                    }
                                } else {
                                    i32 saved_rescue_surface;

                                    saved_rescue_surface = 0;
                                    if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0 && api->field_283 == 6 &&
                                        (i8)api->jump_animation_state >= 0 && (i8)api->jump_animation_state < 32) {
                                        if ((TerSurface[(i8)api->jump_animation_state].flags & 1) != 0) {
                                            saved_rescue_surface = 1;
                                        } else {
                                            if (object->vehicle_hit_timer <= 0.0f) {
                                                ObjHitObj(NULL, object, 1, 0, 0, 1);
                                            }
                                            goto rescue_done;
                                        }
                                    }
                                    {
                                        ClearLastSafeTakeOver(object);
                                        Player_ClearContext(object, 1);
                                        object->field_7aa = 5;
                                        if (object->character_context != -1 &&
                                            api->character_model->model_data_b[api->anim_packet.requested_animation] !=
                                                NULL) {
                                            object->field_7aa = api->anim_packet.requested_animation;
                                        }
                                        object->character_context = 0x2b;
                                        object->jump_variant_state = 0;
                                        if ((TerLayer[((i8)(api->field_283))].flags & 0x20) != 0) {
                                            object->wall_run_state = 3;
                                        } else if (saved_rescue_surface || api->field_283 == 6) {
                                            api->velocity.y = -0.25f;
                                            object->wall_run_state = 2;
                                            NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                        } else if (WORLD->current_level == VADERC_LDATA) {
                                            api->velocity.y = -0.5f;
                                            object->wall_run_state = 4;
                                            NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                        } else {
                                            object->wall_run_state = 1;
                                        }
                                        object->turn_braking = 0.0f;
                                        PlayDieSfx(object);
                                        object->current_hp = 0;
                                        if (((api->flags_1fc_word & 0x80) != 0) &&
                                            ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet)) != NULL &&
                                            ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet))->coins > 0 &&
                                            BonusWinner == -1) {
                                            i32 coins;

                                            coins = LoseCoins(object, 2);
                                            if (coins > 0) {
                                                AddPickups(coins, 0, 0, 0, &api->collision_position, NULL, 2.0f,
                                                           ((i8)(api->field_280)), 1.0f,
                                                           api->water_height != 2000000.0f &&
                                                                   (TerLayer[((i8)(api->field_283))].flags & 1) != 0
                                                               ? api->water_height
                                                               : 2000000.0f,
                                                           NULL, 1, 0);
                                            }
                                            ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet))->scale = 1.5f;
                                        }
                                    }
                                rescue_done:;
                                }
                            } else if ((api->character_data->model_flags & 0x40) == 0 && object->id != (u16)id_SNAKE &&
                                       api->field_283 == 9 && api->collision_max.y < api->water_height) {
                                if (BonusWinner == -1) {
                                    i32 coins;

                                    coins = LoseCoins(object, 2);
                                    if (coins > 0) {
                                        AddPickups(coins, 0, 0, 0, &api->upper_position, NULL, 2.0f,
                                                   ((i8)(api->field_280)), 1.0f, 2000000.0f, NULL, 1, 0);
                                    }
                                }
                                InstantKillParts(object, 1, 2.0f);
                                KillPlayer(object, 3, 1, NULL);
                            }
                        }
                    }

                    api->is_underwater = ((u8)(UnderWater(object)));
                    api->water_intersects = ((u8)(IntersectWater(object)));
                }

                if (api->water_intersects != 0 && (entry_underwater | entry_intersects_water) == 0) {
                    i32 splash;

                    splash = object->character_context == 0x1f || object->character_context == 0 ||
                             (U64_PAIR(&CInfo[object->character_context].flags) & 4) != 0 ||
                             object->character_context == 0x0e || object->character_context == 0x0d;
                    if (!splash) {
                        i32 animation;

                        animation = CurrentAnim(&api->anim_packet);
                        splash = animation == 0x28 || animation == 5 || ((u32)(animation - 0x4b)) <= 1;
                    }
                    if (splash) {
                        AddWaterSplash(object, &api->collision_position);
                    }
                }
                if (api->model_draw_result != 0 && object->character_context != 0x40 &&
                    object->character_context != 0x2c) {
                    extern void GameObjectRotation(GAMEOBJECT *, i32);
                    GameObjectRotation(object, 2);
                }

                held_buttons = object->pad->buttons_held;
                pressed_buttons = object->pad->buttons_pressed;
                if ((api->flags_1fc_word & 0x180) == 0x80 && (entry_menu_id != -1 || MiniCutCam == 2)) {
                    object->pad->buttons_held = 0;
                    object->pad->buttons_pressed = 0;
                }
                Tag_Check(object);
                PreResetCode(object);
                ((TERRAIN_PLAYER_CHARACTER_PC *)(api->character_data))->move_callback(object);
                PostResetCode(object);
                if ((api->character_data->model_flags & 0x00200000) == 0) {
                    BigJumpCode(object);
                }
                object->pad->buttons_held = held_buttons;
                object->pad->buttons_pressed = pressed_buttons;
            }

            if ((object->portal_flags & 0x40000) == 0 && ((TERRAIN_PLAYER_SET_PC *)(CurrentTerrain)) != NULL &&
                api->dead == 0 && (WORLD->current_level->flags & 0xe0) == 0 &&
                (((((TERRAIN_PLAYER_SET_PC *)(CurrentTerrain))->minimum_height))) - 5.0f > api->position.y) {
                if (api->model_draw_result != 0) {
                    InstantKillParts(object, 0, 0.0f);
                }
                KillPlayer(object, 3, 1, NULL);
            }
        }
    }

    entry_pad->runtime_flags &= ~0x16u;
    object->field_d20 = 0;
    entry_pad->operator_data = NULL;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, object->field_0x276);
}

void __cdecl AnimatePlayer(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    i16 override_from;
    f32 direction, movement_speed, multiplier;
    f32 *time;
    object->anim_packet.previous_animation = object->anim_packet.animation_index;
    object->mini_previous_animation = object->mini_current_animation;
    if ((object->flags_1f8 & 0x40000) == 0) {
        object->character_data->animate_fn(object);
    }
    override_from = object->ai_packet.animation_override_from;
    if (override_from != -1 && ((override_from == 0xe9 && object->character_context != 0x1c) ||
                                object->anim_packet.requested_animation == override_from)) {
        object->anim_packet.requested_animation = object->ai_packet.animation_override_to;
    }
    if (model == NULL || (object->dead != 0 && object->death_delay == 0.0f) || object->dead == 1) {
        return;
    }
    direction = 1.0f;
    if ((object->flags_ef0 & 0x4000) != 0) {
        direction = -1.0f;
    }
    if (object->character_model->action_animations[1] == NULL) {
        if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
            movement_speed =
                object->velocity.x * object->facing_direction.x + object->velocity.z * object->facing_direction.z;
            if (movement_speed < 0.0f) {
                movement_speed = 0.0f;
            }
        } else {
            movement_speed = pad->input_magnitude;
        }
    } else {
        movement_speed = pad->animation_input_magnitude;
        if (movement_speed > 0.0f && (u16)object->id == id_GONKDROID && Cheat_IsOn(8) == 0) {
            movement_speed = object->character_data->game_character->walk_speed;
        }
    }
    multiplier = (object->character_context == 0x2d && object->grapple != NULL) ? GizBuildItMul(object) : 1.0f;
    UpdateAnimPacket(model, &object->anim_packet, (f32)((f64)FRAMETIME * 30.0 * (f64)multiplier),
                     movement_speed * direction, multiplier * FRAMETIME,
                     object->character_data->game_character->backwards_factor);
    if ((object->anim_packet.flags & 4) != 0) {
        time = NULL;
        if (object->anim_packet.blending != 0) {
            if (object->anim_packet.blend_target_animation == 0x5f) {
                time = &object->anim_packet.blend_target_time;
            }
        } else if (object->anim_packet.animation_index == 0x5f) {
            time = &object->anim_packet.current_time;
        }
        if (time != NULL) {
            f32 duration = AnimDuration((i16)object->id, 0x5f, 0.0f, 0.0f, 0);
            i32 count = (i32)((f64)duration / (f64)0.3f);
            i32 phase;
            f32 start;
            if (NuFmod(duration, 3.0f) > 0.15f) {
                ++count;
            }
            phase = qrand() / (0xffff / count + 1);
            model = object->character_model;
            start = (f32)((f64)phase * ((f64)0.3f * (f64)model->animation_configs[0x5f]->playback_rate) + 1.0);
            if (start < NuAnimEndFrame(model->action_animations[0x5f])) {
                *time = start;
            }
            object->flags_e20 = (object->flags_e20 & ~0x4000u) | ((phase & 1) << 14);
        }
    }
    AutoWeaponOnOff(object);
    AddFootSteps(object);
}

static void DrawWeapon_SetSabreObjects(GAMEOBJECT *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models,
                                       i32 *hilt) {
    if (red || green || blue || purple) {
        if (object->id == (u16)id_DARTHMAUL) {
            models[0] = *hilt = 0x12;
        } else if (object->id == (u16)id_DOOKU && WORLD->level_objects[0x13].active) {
            models[0] = *hilt = 0x13;
        } else {
            models[0] = *hilt = 0x11;
        }
    }
    if (red) {
        if (object->id == (u16)id_DARTHMAUL) {
            if (object->dead != 0) {
                return;
            }
            models[1] = 0x6d;
            models[2] = 0x6e;
            if (object->flags_e20 & 0x80000) {
                models[3] = 0x6e;
            }
        } else {
            if (object->dead != 0) {
                return;
            }
            models[1] = 0x65;
            models[2] = 0x66;
            if (object->flags_e20 & 0x80000) {
                models[3] = 0x66;
            }
        }
        return;
    }

    if (green) {
        if (object->dead != 0) {
            return;
        }
        models[1] = 0x67;
        models[2] = 0x68;
        if (object->flags_e20 & 0x80000) {
            models[3] = 0x68;
        }
        return;
    }

    if (blue) {
        if (object->dead == 0) {
            models[1] = 0x69;
            models[2] = 0x6a;
            if (object->flags_e20 & 0x80000) {
                models[3] = 0x6a;
            }
        }
    } else if (purple && object->dead == 0) {
        models[1] = 0x6b;
        models[2] = 0x6c;
        if (object->flags_e20 & 0x80000) {
            models[3] = 0x6c;
        }
    }
}

i32 __cdecl AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == (u16)id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

static f32 ParaphernaliaWeaponScalePc(GAMEOBJECT *object) {
    f32 *frame;
    if (object->character_data->game_character->hover_fall_mode == 9) {
        frame = AnimPlaying(&object->anim_packet, 0x5e, 1, 0);
        if (frame != NULL && *frame >= 18.0f && *frame <= CharacterWeaponScaleUpperBoundPc) {
            if (*frame <= 27.0) {
                return (f32)(((f64)*frame - 18.0) / 9.0);
            }
            if (55.0 > *frame) {
                return 1.0f;
            }
            return (f32)(1.0 - ((f64)*frame - 55.0) / 10.0);
        }
    }
    return 0.0f;
}

static void ParaphernaliaShieldPc(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    i32 locator = config->shield_locator;
    NUVEC scaling;
    NUMTX matrix, reflected;
    if (object->shield_hitpoints == 0 && config->shield_hitpoints > 0 && object->timer_d34 > 0.0f) {
        if ((GameTimer.update_count & 3u) >= 2) {
            return;
        }
        scaling.x = 1.0f;
    } else {
        if (object->origin_scale_override <= 0.0f) {
            return;
        }
        scaling.x = object->origin_scale_override;
    }
    scaling.y = scaling.z = scaling.x;
    NuMtxSetScale(&matrix, &scaling);
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        NuMtxTranslate(&matrix, (NUVEC *)&object->locator_matrices[locator].m30);
    } else {
        NuMtxTranslate(&matrix, &object->collision_position);
    }
    Draw3DObjectMtx(NULL, object->timer_d34 > 0.0f ? 0x70 : 0x6f, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, object->timer_d34 > 0.0f ? 0x93 : 0x92, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaThrustPc(GAMEOBJECT *object, i32 reflection) {
    i32 locator, effect_index;
    if (WORLD->level_objects[0x77].active == 0 || WORLD->level_objects[0x78].active == 0) {
        return;
    }
    effect_index = 0;
    for (locator = 0; locator < 16; ++locator) {
        f32 ratio;
        NUVEC scaling;
        NUMTX matrix, reflected;
        i32 first, second;
        if ((object->character_data->game_character->thrust_locators & (1u << locator)) == 0 ||
            object->character_model->points_of_interest[locator] == NULL) {
            continue;
        }
        ratio = (f32)((f64)((u8 *)object)[0xe28 + effect_index++] / 255.0);
        scaling.x = (f32)((((f64)ratio - 0.5) * 0.5) * object->thrust_effect_scale + object->thrust_effect_scale);
        if (scaling.x <= 0.0f) {
            continue;
        }
        scaling.y = scaling.z = scaling.x;
        matrix = object->locator_matrices[locator];
        NuMtxPreScale(&matrix, &scaling);
        Draw3DObjectMtx(NULL, 0x77, &matrix);
        Draw3DObjectMtx(NULL, 0x78, &matrix);
        if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                                WORLD->current_level->reflection_y, &reflected)) {
            first = LevelObject_GetReflection(0x77);
            if (WORLD->level_objects[first].active == 0) {
                first = 0x77;
            }
            second = LevelObject_GetReflection(0x78);
            if (WORLD->level_objects[second].active == 0) {
                second = 0x77;
            }
            NuRndrStartReflectionRender(0);
            /* Retail draws the original matrix in this reflection pass. */
            Draw3DObjectMtx(NULL, first, &matrix);
            Draw3DObjectMtx(NULL, second, &matrix);
            NuRndrEndReflectionRender();
        }
    }
}

void __cdecl ParaphernaliaCommunicatePc(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    i32 locator = config->thingy_locator;
    i32 model, reflection_model;
    NUMTX matrix, reflected;
    NUVEC scaling, blend_scaling;
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    model = -1;
    if ((object->character_data->model_flags & 0x40) != 0) {
        scaling.z = object->communicate_blend * object->origin_scale;
        scaling.x = scaling.y = scaling.z;
        NuMtxSetScale(&matrix, &scaling);
        GameDrawMtxRotateYPc(&matrix, (NUANG)(object->field_0x276 + 0x8000));
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        if (object->field_283 == 9 && matrix.m31 < object->water_height) {
            matrix.m31 = object->water_height;
        }
        model = 0x19;
    } else {
        if ((config->flags_094 & 0x80) != 0) {
            model = 0xe;
        }
        matrix = object->locator_matrices[locator];
        if (object->communicate_blend != 1.0f) {
            blend_scaling.z = object->communicate_blend;
            blend_scaling.x = blend_scaling.y = blend_scaling.z;
            NuMtxPreScale(&matrix, &blend_scaling);
        }
        if (model == -1) {
            return;
        }
    }
    if (WORLD->level_objects[model].active == 0) {
        return;
    }
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    reflection_model = LevelObject_GetReflection(model);
    if (reflection_model != -1 && WORLD->level_objects[reflection_model].active != 0 && reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                         WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&WORLD->level_objects[reflection_model].special, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaPlacePc(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 marker;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 ||
        object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
    if (frame == NULL) {
        return;
    }
    end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    if (start >= end) {
        return;
    }
    if (start < 1.0f) {
        start = 1.0f;
    }
    if (*frame < start) {
        return;
    }
    marker = AnimListFrame(object->character_model, object->field_7aa, 1);
    if (marker <= start) {
        return;
    }
    if (end < marker) {
        marker = end;
    }
    if (*frame < marker) {
        scaling.x = (*frame - start) / (marker - start);
    } else {
        scaling.x = 1.0f;
    }
    scaling.y = scaling.z = scaling.x;
    matrix = object->locator_matrices[locator];
    if (model == 0xec) {
        extra_model = 0xed;
        NuMtxPreRotateY(&matrix, object->takeover_start_angle);
    } else {
        extra_model = -1;
    }
    NuMtxPreScale(&matrix, &scaling);
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
        NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    }
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Retail draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
        }
    }
}

static void ParaphernaliaRemovePc(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 fade_start;
    f32 fade_end;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 ||
        object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
    if (frame == NULL) {
        return;
    }
    end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    if (start >= end) {
        return;
    }
    if (start < 1.0f) {
        start = 1.0f;
    }
    if (*frame < start) {
        return;
    }
    fade_start = AnimListFrame(object->character_model, object->field_7aa, 1);
    if (fade_start <= start) {
        return;
    }
    fade_end = AnimListFrame(object->character_model, object->field_7aa, 2);
    if (fade_end < fade_start) {
        fade_end = fade_start;
    }
    if (end < fade_start) {
        fade_start = end;
    }
    if (end < fade_end) {
        fade_end = end;
    }
    if (*frame <= fade_start) {
        scaling.x = 1.0f;
    } else {
        if (!(*frame < fade_end) || !(fade_start < fade_end)) {
            return;
        }
        scaling.x = 1.0f - (*frame - fade_start) / (fade_end - fade_start);
    }
    scaling.y = scaling.z = scaling.x;
    matrix = object->locator_matrices[locator];
    if (model == 0xec) {
        extra_model = 0xed;
        NuMtxPreRotateY(&matrix, object->takeover_start_angle);
    } else {
        extra_model = -1;
    }
    NuMtxPreScale(&matrix, &scaling);
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
        NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    }
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Retail draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
        }
    }
}

static void ParaphernaliaHandPc(GAMEOBJECT *object, i32 reflection) {
    NUMTX matrix;
    NUMTX reflected;
    i32 locator;
    if (object->character_context != 0x5d || WORLD->level_objects[0xf7].active == 0) {
        return;
    }
    locator = object->character_data->game_character->hand_locators[0];
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    NuMtxSetTranslation(&matrix, (NUVEC *)&object->locator_matrices[locator].m30);
    Draw3DObjectMtx(NULL, 0xf7, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, 0xf7, &reflected);
        NuRndrEndReflectionRender();
    }
}

void __cdecl ConfigureComplexShadow(GAMEOBJECT *object) {
    NUVEC direction;
    f32 distance;
    if (object != NULL) {
        direction.x = 65536.0f;
        direction.y = object->collision_position.y;
        direction.z = (f32)((f64)object->floor_height - (f64)0.1f);
    } else {
        direction.x = 0.0f;
        direction.y = -ComplexShadowBasePc * 0.25;
        direction.z = 0.0f;
    }
    distance = VehicleArea != 0 ? 150.0f : 30.0f;
    NuRndrShadowDirCol(&direction, 0x17000000u, distance, distance);
}

void __cdecl LoadSingleCharacter(BGPROCINFO *info) {
    APICHARACTERMODELLIST list[2] = {{(i16)waiting_for_character, 1}, {-1, 0}};
    NUGSCN *scene;
    i32 i;
    append = 1;
    nopakfile = CHARPAK == 0;
    APILoadCharacterModels(list, 0, &characterbuffer_ptr, characterbuffer_end, CharacterLoadAreaModelsPc);
    IconScenes_Load(list, 0, &characterbuffer_ptr, &characterbuffer_end);
    scene = IconScene_FindById(list[0].model_id);
    if (scene != NULL) {
        for (i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (ObjTab[i].kind != 3 || WORLD->level_objects[i].active != 0) {
                continue;
            }
            if (NuSpecialFind(scene, &WORLD->level_objects[i].special, ObjTab[i].name, 1) != 0) {
                WORLD->level_objects[i].active = 1;
            }
        }
    }
    Customiser_SaveModelTextureIDs(CharacterCustomiser, APICharacterLoaded(list[0].model_id));
    hub_character_ready = waiting_for_character;
    waiting_for_character = -1;
}

void __cdecl UpdateCharacterLoad(void) {
    i32 candidates[340];
    i32 candidate;
    i32 menu = GetMenuID();
    i32 required;
    i32 count;
    i32 id;
    if (menu == 13 || menu == 15 || menu == 16 || menu == 17 || LOADEROFF != 0 || BGLOAD == 0) {
        return;
    }
    if (waiting_for_character != -1 || hub_character_ready != -1 || bgGetProcActive() != NULL ||
        (i32)nudlist_field_9d8 > 0xfd) {
        return;
    }
    required = (0x3ff - (i32)(((f64)CHARACTERBUFFERSIZE / 7077888.0) * -1048576.0)) & ~0x3ff;
    if ((i32)(characterbuffer_end.addr - characterbuffer_ptr.addr) < required) {
        return;
    }
    count = 0;
    if (VehicleBoltId7f17d4Pc != -1 && APICharacterLoaded(VehicleBoltId7f17d4Pc) != NULL &&
        PunchRangeCompanionIdPc != -1 && APICharacterLoaded(PunchRangeCompanionIdPc) == NULL) {
        candidates[count++] = PunchRangeCompanionIdPc;
    } else if (id_BARMAN != -1 && APICharacterLoaded(id_BARMAN) == NULL) {
        candidates[count++] = id_BARMAN;
    } else if (id_CANTINABAND != -1 && APICharacterLoaded(id_CANTINABAND) == NULL) {
        candidates[count++] = id_CANTINABAND;
    } else if (PermanentWeirdo1IdPc != -1 && APICharacterLoaded(PermanentWeirdo1IdPc) == NULL) {
        candidates[count++] = PermanentWeirdo1IdPc;
    } else if (PermanentWeirdo2IdPc != -1 && APICharacterLoaded(PermanentWeirdo2IdPc) == NULL) {
        candidates[count++] = PermanentWeirdo2IdPc;
    } else if (DefaultAnimationJabbaIdPc != -1 && APICharacterLoaded(DefaultAnimationJabbaIdPc) == NULL) {
        candidates[count++] = DefaultAnimationJabbaIdPc;
    } else if (id_MOSEISLEYCITIZEN != -1 && APICharacterLoaded(id_MOSEISLEYCITIZEN) == NULL) {
        candidates[count++] = id_MOSEISLEYCITIZEN;
    } else if (id_CANTINAALIEN != -1 && APICharacterLoaded(id_CANTINAALIEN) == NULL) {
        candidates[count++] = id_CANTINAALIEN;
    } else if (CharacterFixupIdPc007f18c4 != -1 && APICharacterLoaded(CharacterFixupIdPc007f18c4) == NULL) {
        candidates[count++] = CharacterFixupIdPc007f18c4;
    } else {
        for (id = 0; id < CHARCOUNT; ++id) {
            if (id == id_DROIDEKA || (CDataList[id].model_flags & 0x04002000) != 0 || APICharacterLoaded(id) != NULL) {
                continue;
            }
            if ((i32)CDataList[id].animation_count + apicharsys->loaded_animation_count >
                apicharsys->animation_capacity) {
                continue;
            }
            if (InCollectList_Index(id, NULL, 0) == -1 || Collection_Got(id) == 0) {
                continue;
            }
            candidates[count++] = id;
        }
    }
    if (count <= 0) {
        return;
    }
    if (count > 1) {
        candidate = qrand() / ((0xffff / count) + 1);
    } else {
        candidate = 0;
    }
    waiting_for_character = candidates[candidate];
    WaitingForCharacterTime = 0.0f;
    bgPostRequest(LoadSingleCharacter, NULL, NULL, 0);
}

i32 __cdecl ActivePlayerInRange(NUVEC *position, f32 range_squared, f32 *distance_squared) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0) {
            f32 distance = NuVecDistSqr(&object->collision_position, position, NULL);
            if (distance < range_squared) {
                if (distance_squared != NULL) {
                    *distance_squared = distance;
                }
                return 1;
            }
        }
    }
    return 0;
}

i32 __cdecl AvailableToPlayer(u32 character_flags, i32 weapon_action, i32 helmet, i32 require_all) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object == NULL || object->character_data == NULL) {
            continue;
        }
        if (character_flags != 0) {
            if ((object->character_data->model_flags & character_flags) == character_flags) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (weapon_action != -1) {
            if (object->character_data->game_character->weapon_action == weapon_action) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (helmet != 0) {
            if (object->helmet != helmet) {
                continue;
            }
            if (require_all == 0) {
                return 1;
            }
        }
        return 1;
    }
    if (FreePlay != 0) {
        for (index = 0; index < apicharsys->character_count; ++index) {
            if (apicharsys->playermodelids[index] == -1 || FreePlay == 0 ||
                (GCDataList[index].flags_090 & 0x80000000) == 0 || Cheats_CheckFlags(0x100) == 0) {
                continue;
            }
            if (character_flags != 0) {
                if ((CDataList[index].model_flags & character_flags) == character_flags) {
                    if (require_all == 0) {
                        return 1;
                    }
                } else if (require_all != 0) {
                    continue;
                }
            }
            if (weapon_action != -1) {
                if (GCDataList[index].weapon_action != weapon_action) {
                    continue;
                }
                if (require_all == 0) {
                    return 1;
                }
            }
            return 1;
        }
    }
    return 0;
}

GAMEOBJECT *__cdecl GetOtherActivePlayer(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if (object == Player[0]) {
        other = (GAMEOBJECT *)Player[1];
    } else if (object == Player[1]) {
        other = (GAMEOBJECT *)Player[0];
    } else {
        return NULL;
    }
    if (other != NULL && (other->flags_1fc_word & 0x80) != 0) {
        return other;
    }
    return NULL;
}

#pragma data_seg(".data$SpecialLayer")
static SPECIAL_LAYER_PC SpecialLayer[42] = {
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_bombbackpack, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_sonargun, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_infrared_goggles, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_high, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_magnetic_boots, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_scuba_gear, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hack_pack, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_vacuum_gun, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_blue, 0},
};
#pragma data_seg()

u32 __cdecl AdjustLayerBits(u32 mask, GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
    i32 cape_layer = data->cape_layer;
    u32 cape = cape_layer != -1 ? 1u << cape_layer : 0;
    i32 hair_layer = data->hair_layer;
    u32 hair = hair_layer != -1 ? 1u << hair_layer : 0;
    SUIT_LAYERS_PC *suit;
    if (object->helmet != 0) {
        mask &= ~hair;
    }
    suit = (SUIT_LAYERS_PC *)object->suit;
    if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_A) {
        if ((suit->flags & 4) != 0) {
            mask = (mask & ~cape) | SpecialLayer[0].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[5].mask;
            if ((mask & SpecialLayer[20].mask) != 0) {
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[24].mask;
            } else if ((mask & SpecialLayer[21].mask) != 0) {
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[25].mask;
            }
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[19].mask;
        } else if ((suit->flags & 2) != 0) {
            mask &= ~cape;
        } else if ((suit->flags & 8) != 0) {
            mask = (mask & ~cape) | SpecialLayer[1].mask | SpecialLayer[7].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[4].mask;
            if ((mask & SpecialLayer[20].mask) != 0) {
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[22].mask;
            } else if ((mask & SpecialLayer[21].mask) != 0) {
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[23].mask;
            }
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[18].mask;
            if ((mask & SpecialLayer[8].mask) != 0) {
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[14].mask;
            } else if ((mask & SpecialLayer[9].mask) != 0) {
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[15].mask;
            } else if ((mask & SpecialLayer[10].mask) != 0) {
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[16].mask;
            }
        } else if ((suit->flags & 1) != 0) {
            mask |= SpecialLayer[2].mask;
            if ((mask & SpecialLayer[8].mask) != 0) {
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[11].mask;
            } else if ((mask & SpecialLayer[9].mask) != 0) {
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[12].mask;
            } else if ((mask & SpecialLayer[10].mask) != 0) {
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[13].mask;
            }
        }
    } else if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_B) {
        if ((suit->flags & 0x10) != 0) {
            mask = (mask & ~hair & ~cape) | SpecialLayer[27].mask;
            if ((mask & SpecialLayer[30].mask) != 0) {
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[39].mask;
            } else if ((mask & SpecialLayer[31].mask) != 0) {
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[40].mask;
            } else if ((mask & SpecialLayer[32].mask) != 0) {
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[41].mask;
            }
        } else if ((suit->flags & 0x40) != 0) {
            mask = (mask & ~cape) | SpecialLayer[26].mask;
            if ((mask & SpecialLayer[30].mask) != 0) {
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[36].mask;
            } else if ((mask & SpecialLayer[31].mask) != 0) {
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[37].mask;
            } else if ((mask & SpecialLayer[32].mask) != 0) {
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[38].mask;
            }
        } else if ((suit->flags & 0x20) != 0) {
            mask = (mask & ~cape) | SpecialLayer[28].mask;
            if ((mask & SpecialLayer[30].mask) != 0) {
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[33].mask;
            } else if ((mask & SpecialLayer[31].mask) != 0) {
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[34].mask;
            } else if ((mask & SpecialLayer[32].mask) != 0) {
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[35].mask;
            }
        } else if ((suit->flags & 0x80) != 0) {
            mask = (mask & ~cape) | SpecialLayer[29].mask;
        }
    }
    if (StartupGameFlag1 != 0) {
        if (object->id == (u16)id_BODYGUARD) {
            if (object->current_hp <= 1) {
                mask &= ~0x10u;
            }
        } else if (object->id == (u16)id_GEONOSIAN) {
            if ((object->flags_ef0 & 0x200) != 0) {
                mask |= 0x20;
            } else {
                mask |= 0x40;
            }
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[0]) {
            if ((CharacterCustomiser->pieces[Game.customizer_cape_piece].layer_flags & 0x40) == 0) {
                mask |= cape;
            }
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[1]) {
            if ((CharacterCustomiser->pieces[Game.customizer_secondary_cape_piece].layer_flags & 0x40) == 0) {
                mask |= cape;
            }
        } else if (object->id == (u16)id_CHEWBACCA) {
            if (Cheat_IsOn(4) != 0) {
                mask |= 0xc0;
            }
        }
    }
    if (object->character_data->game_character->ride_layers_off != 0 && object->takeover_source != NULL &&
        object->character_context == 0x3b) {
        mask &= ~object->character_data->game_character->ride_layers_off;
    }
    return mask;
}

static const u8 CreatureMosHatPc[5] = {0, 0, 0, 7, 14};
static const u8 CreatureMosHeadPc[5] = {20, 20, 8, 8, 4};
static const u8 CreatureMosBodyPc[3] = {3, 9, 19};
static const u8 CreatureMosArmsPc[3] = {1, 12, 15};
static const u8 CreatureMosHandsPc[3] = {2, 13, 16};
static const u8 CreatureMosWaistPc[3] = {6, 10, 17};
static const u8 CreatureMosLegsPc[3] = {5, 11, 18};

static void SetLayersMosPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureMosHatPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosHeadPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosArmsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosHandsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosWaistPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosLegsPc[qrand() / 0x5556];
}

static const u8 CreatureCantinaHeadPc[3] = {0, 3, 6};
static const u8 CreatureCantinaBodyPc[3] = {1, 4, 7};
static const u8 CreatureCantinaLegsPc[3] = {2, 5, 8};

static void SetLayersCantinaPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCantinaHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaLegsPc[qrand() / 0x5556];
}

static const u8 CreatureCloudHeadPc[3] = {1, 5, 7};
static const u8 CreatureCloudBodyPc[3] = {2, 3, 6};
static const u8 CreatureCloudLegsPc[3] = {0, 4, 8};

static void SetLayersCloudPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCloudHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudLegsPc[qrand() / 0x5556];
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((object->layer_mask << 4) & 0x800);
}

static void SetLayersBobPc(GAMEOBJECT *object) {
    i32 head[6] = {1, 2, 3, 21, 22, 23};
    i32 body[8] = {5, 6, 7, 8, 9, 10, 16, 17};
    i32 legs[3] = {11, 12, 15};
    i32 arms[2] = {13, 14};
    i32 hands[2] = {19, 20};
    ((PLAYER_LAYER_FLAGS_PC *)&object->flags_ef0)->bob = qrand() < 0x8000;
    if (qrand() < 0x8000) {
        object->layer_mask |= 0x10;
    }
    object->layer_mask = 1;
    object->layer_mask |= 1u << head[qrand() / 0x2aab];
    object->layer_mask |= 1u << body[qrand() / 0x2000];
    object->layer_mask |= 1u << legs[qrand() / 0x5556];
    object->layer_mask |= 1u << arms[qrand() / 0x8000];
    object->layer_mask |= 1u << hands[qrand() / 0x8000];
}

void __cdecl FixUpLayers(void) {
    i32 model_index;
    i32 id;
    for (model_index = 0; model_index < apicharsys->loaded_model_count; ++model_index) {
        CHARACTER_MODEL_LAYERS_PC *model = &apicharsys->models[model_index];
        PLAYERCHARACTERCONFIG_PC *character = &GCDataList[model->model_id];
        i32 layer_index;
        for (layer_index = 0; layer_index < character->layer_count; ++layer_index) {
            character->layers[layer_index].hierarchy_layer_index =
                NuHGobjGetLayerIndex(character->layers[layer_index].name, model->hierarchy);
        }
    }

    for (model_index = 0; model_index < 42; ++model_index) {
        SpecialLayer[model_index].mask = 0;
        if (SpecialLayer[model_index].character_id != NULL) {
            id = *SpecialLayer[model_index].character_id;
            if (id != -1) {
                SpecialLayer[model_index].mask = 1u << LayerFromName(&GCDataList[id], SpecialLayer[model_index].name);
            }
        }
    }
}

i32 __cdecl fun_0041aec0(GAMEOBJECT *object) {
    u8 context = (u8)object->character_context;
    if (context == 0x4d && object->field_7b3 == 0) {
        return 2;
    }
    return (context == 0x52) + 1;
}

i32 __cdecl SuperWeirdo(GAMEOBJECT *object) {
    if ((object->flags_1fc & 0x80) != 0 && (Game.customizer_weirdo_flags & 1) != 0 && CharacterCustomiser != NULL &&
        (object->id == (u16)CharacterCustomiser->character_ids[0] ||
         object->id == (u16)CharacterCustomiser->character_ids[1])) {
        return 1;
    }
    return 0;
}

i32 __cdecl Player_HasPurpleForce(GAMEOBJECT *object) {
    if (Cheat_IsOn(28) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

void __cdecl ResetPlayerMoves(GAMEOBJECT *object) {
    ResetPlayerPacket(((PLAYER_PACKET_STORAGE_PC *)object)->player_packet_storage, object->character_data);
    object->fall_acceleration_timer = 0.0f;
    object->movement_direction = v000;
    object->hud_icon_timer = 0.0f;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    if (object->character_model->action_animations[1] != NULL) {
        ResetAnimPacket(&object->anim_packet, 1);
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    } else {
        ResetAnimPacket(&object->anim_packet, -1);
    }
    object->field_282 = 0;
    object->field_281 = 0;
    SetGameObjectCharacterData(object);
    object->spawn_protection_timer = 0.0f;
    SetFlicker(object, 0.0f);
    ResetCoinPacket(object->coin_packet);
    object->respawn_timer = 0.0f;
    object->flags_1fc_word &= ~0x2000u;
    DrawOffsetCode(object, 1);
}

void __cdecl InitPlayerAI(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character;
    i32 can_pull_levers;
    u8 type;
    char *name;
    u64 route_mask;
    u32 route_mask_word;
    if (StartupGameFlag0 != 0) {
        PlayerInitCapabilitiesVariant0Pc(object);
    } else if (StartupGameFlag1 != 0) {
        PlayerInitCapabilitiesVariant1Pc(object);
    } else {
        PlayerInitCapabilitiesDefaultPc(object);
    }
    can_pull_levers = CanPullLevers((i16)object->id);
    object->ai_packet.flags &= ~0x10000u;
    object->flags_ef0 = (object->flags_ef0 & ~0x800000u) | (((u32)can_pull_levers << 23) & 0x800000u);
    object->ai_packet.route_mask_low = 0;
    object->ai_packet.route_mask_high = 0;
    if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0) {
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low),
               sizeof(route_mask_word));
        object->ai_packet.route_mask_low = route_mask_word;
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high),
               sizeof(route_mask_word));
        object->ai_packet.route_mask_high = route_mask_word;
    } else if (SpecialRouteCharacterTypeIDFn != NULL) {
        name = object->suit != NULL ? ((PLAYER_SUIT_NAME_PC *)object->suit)->name : object->character_data->name;
        type = SpecialRouteCharacterTypeIDFn(name);
        if (type != 0xff) {
            if (type < 64) {
                route_mask = (u64)1 << type;
                object->ai_packet.route_mask_low = (u32)route_mask;
                object->ai_packet.route_mask_high = (u32)(route_mask >> 32);
            } else {
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low),
                       sizeof(route_mask_word));
                object->ai_packet.route_mask_low = route_mask_word;
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high),
                       sizeof(route_mask_word));
                object->ai_packet.route_mask_high = route_mask_word;
            }
        }
    }
    object->run_speed_override = 1000000000.0f;
    object->walk_speed_override = 1000000000.0f;
    character = object->character_data;
    object->ai_packet.creature_set = 0;
    object->portal_flags |= 2;
    if ((character->model_flags & 0x80) != 0) {
        object->portal_flags |= 4;
    } else {
        object->portal_flags &= ~4u;
    }
    object->portal_flags = (object->portal_flags & 0x25fffc27) | 8;
    object->flags_ef0 &= 0xc7ffe6c0;
    object->hover_height_override = 1000000000.0f;
    object->flags_ef4 &= 0x31f7e95c;
    object->collision_scale_override = 1000000000.0f;
    object->flags_ef8_word &= 0xfffffffc;
    object->doomed_escape_locator = NULL;
    if ((object->flags_1f8 & 0x400) != 0) {
        object->flags_ef4 &= ~0x40000u;
    } else {
        object->flags_ef4 |= 0x40000u;
    }
    object->extra_layer_mask = 0;
    object->ai_packet.movement_target_radius = DEFAULT_MOVE_RANGE;
    object->ai_packet.flags &= ~0x10000000u;
    if (object->ai_packet.movement_target_radius > 0.0f) {
        object->ai_packet.flags = (object->ai_packet.flags & ~0x8000000u) | 0x4000000u;
    } else {
        object->ai_packet.flags &= ~0xc000000u;
    }
    object->movement_speed_scale_pc = 1.0f;
    object->flags_ef4 &= 0xfe5fffff;
    object->portal_flags &= 0xfff3ffff;
    if ((character->model_flags & 0x8000000) != 0) {
        object->portal_flags |= 0x100000;
    } else {
        object->portal_flags &= ~0x100000u;
    }
    object->tag_flags &= ~2u;
    object->field_ec4 = 0.0f;
    object->ai_packet.flags &= 0xffffafff;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    object->ai_packet.antinode_timer = 0.0f;
    object->portal_flags &= 0xfadff7fe;
    object->timer_ecc = 0.0f;
    object->flags_ef0 &= ~0x1000000u;
    object->alert_target = NULL;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.opponent = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->field_eb0 = 0;
    object->field_eb4 = 0;
    object->opponent = NULL;
    object->last_attacker = NULL;
    object->ai_opponent_exclusion_mask[0] = 0;
    object->ai_opponent_exclusion_mask[1] = 0;
    memset(object->joint_overrides, 0, 2 * sizeof(object->joint_overrides[0]));
    character = object->character_data;
    object->joint_override_count = 0;
    object->head_target = NULL;
    object->ai_packet.dont_avoid_character = NULL;
    object->collision_link = NULL;
    object->collision_exclusion_mask[0] = 0;
    object->collision_exclusion_mask[1] = 0;
    if ((character->model_flags & 0x4000) != 0 && (object->field_280 == 0xff || object->rider != NULL)) {
        object->flags_1fc_word |= 2;
    } else {
        object->flags_1fc_word &= ~2u;
    }
    if ((character->game_character->flags_090 & 0x2000) != 0) {
        object->flags_ef0 |= 0x80000000u;
    } else {
        object->flags_ef0 &= ~0x80000000u;
    }
    object->flags_1fc_word &= 0xffe7f7ff;
    object->flags_ef4 &= ~4u;
    object->animation_speed_multiplier = 1.0f;
    object->flags_ef0 &= 0xfbbfffff;
    object->flags_ef8_word &= 0xfffffff3;
    object->field_1038 = 0.0f;
    object->indy_climb = NULL;
    object->field_1083 = 0;
    object->gizforce_target = NULL;
    ResetPlayerAI(object);
}

void SetPlayerGroupPosition(f32 x, f32 y, f32 z) {
    i32 i;
    NUVEC offset = {0.0f, 0.0f, 0.2f};
    for (i = 0; i < PLAYERCOUNT; i++) {
        NUVEC position = {x, y, z};
        NuVecAdd(&position, &position, &offset);
        ResetPlayer(Player[i], 1, &position, 1);
        NuVecRotateY(&offset, &offset, 0x2000);
    }
}

i32 __cdecl NewPlayerCharacter(GAMEOBJECT *object, i32 id, i32 old_id, i32 unused) {
    PLAYERCHARACTERCONFIG_PC *config;
    NUVEC velocity;
    AIPATHINFO_PC path;
    AILOCATOR_PC *saved_locator;
    u32 saved_f08, saved_frame_state, saved_ai_flag, lean_enabled;
    i32 saved_flag, lean, context, health, hitpoints;
    u8 route, next_route;
    i32 on_surface;
    if (id == old_id || id < 0 || id >= CHARCOUNT || apicharsys->playermodelids[id] == -1) {
        return 0;
    }
    Shards_HandleLostObj(WORLD, object);
    if (VehicleArea == 0) {
        AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
    }
    context = object->character_context;
    lean = (i16)object->movement_lean_angle;
    lean_enabled = object->character_data->model_flags & 0x2000;
    LoseHelmet(object, 0, 1);
    DestroySnakeBody(object);
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->character_data = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    velocity = object->velocity;
    health = object->current_hp;
    hitpoints = object->field_107e;
    saved_flag = object->flags_1fc_bit_13;
    object->takeover_owner = NULL;
    object->suit = Suit_GetDefault(id);
    saved_locator = object->doomed_escape_locator;
    saved_ai_flag = ((NEW_PLAYER_FLAGS_PC *)object)->flags_ef0_bit_21;
    route = object->ai_packet.current_route;
    next_route = object->ai_packet.next_route;
    path = object->ai_packet.path_info;
    saved_f08 = object->route_frame_state;
    saved_frame_state = object->ai_packet.frame_state;
    InitPlayerAI(object);
    object->doomed_escape_locator = saved_locator;
    ((NEW_PLAYER_FLAGS_PC *)object)->flags_ef0_bit_21 = saved_ai_flag;
    object->ai_packet.current_route = route;
    object->ai_packet.next_route = next_route;
    object->ai_packet.path_info = path;
    object->route_frame_state = saved_f08;
    object->ai_packet.frame_state = saved_frame_state;
    object->collision_scale = 1.0f;
    object->layer_mask = object->character_data->game_character->layer_mask;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    Player_ClearContext(object, 1);
    ResetPlayerMoves(object);
    SetGameObjectCharacterData(object);
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ResetContexts(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet);
    GetTopBot(object);
    GameObjectDimensions(object);
    object->hud_icon_timer = 2.0f;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    InitSurfaceInfo(object);
    on_surface = SetObjOnSurface(object, 1);
    GizForce_ResetLOS(object);
    object->tag_reset_timer = 0.0f;
    object->pause_context_state = 0.0f;
    object->flags_ef0 &= ~0x80000u;
    object->current_hp = health;
    object->field_107e = hitpoints;
    object->flags_1fc_bit_13 = saved_flag;
    ResetMiniAnimPacket((MINIANIMPACKET_PC *)object->mini_anim_packet, -1);
    object->weapon_scale = ((object->flags_e20 >> 16) & 1) != 0 ? 1.0 : 0.0;
    object->field_e33 = 0;
    if (on_surface == 0) {
        object->velocity.y = velocity.y;
    } else {
        ((PLAYER_RESET_FIELDS_PC *)object)->ground_contact_grace_timer = 0.2f;
    }
    if (context == 0x33 && StartSlide(object, 0)) {
        object->velocity.x = velocity.x;
        object->velocity.z = velocity.z;
    }
    if (object->velocity.y == 0.0f) {
        object->velocity.y = -0.1f;
    }
    if (lean_enabled != 0 && (object->character_data->model_flags & 0x2000) != 0) {
        object->movement_lean_angle = (u16)lean;
    }
    object->view_distance = apicharsys->char_data[(i16)object->id].game_character->viewdistance;
    object->hear_distance = apicharsys->char_data[(i16)object->id].game_character->heardistance;
    object->max_view_height = apicharsys->char_data[(i16)object->id].game_character->maxviewheight;
    object->min_view_height = apicharsys->char_data[(i16)object->id].game_character->minviewheight;
    config = object->character_data->game_character;
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((config->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN) {
        SetLayersMosPc(object);
    } else if (object->id == (u16)id_CANTINAALIEN) {
        SetLayersCantinaPc(object);
    } else if (object->id == (u16)id_CLOUDCITYCITIZEN) {
        SetLayersCloudPc(object);
    } else if (object->id == (u16)id_BOB) {
        SetLayersBobPc(object);
    } else if (object->id == (u16)id_GEONOSIAN) {
        ((NEW_PLAYER_FLAGS_PC *)object)->geonosian_variant = qrand() < 0x8000;
    }
    InitCreatureModelFlagsPc(object);
    return 1;
}

void __cdecl ResetAICreature(GAMEOBJECT *object, AISYS_PC *system) {
    AICREATURE_PC *creature;
    AIPATHINFO_PC *path_info;
    AIROW_PC *row;
    NUVEC offset;
    i32 angle;
    if (object == NULL) {
        return;
    }
    PreResetCode(object);
    PostResetCode(object);
    if (system == NULL) {
        return;
    }
    InitPlayerAI(object);
    ResetPlayerMoves(object);
    Player_ClearContext(object, 1);
    Player_ResetContexts(object->unknown_06c4);
    object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    creature = &system->creatures[object->ai_packet.creature_index];
    object->ai_packet.creature_set = ((AI_CREATURE_RESET_VIEW_PC *)creature)->creature_set;
    if ((creature->flags & 1) != 0) {
        object->ai_packet.flags |= 0x80;
    } else {
        object->ai_packet.flags &= ~0x80u;
    }
    object->ai_packet.flags &= ~0x40000u;
    object->view_distance = creature->view_distance;
    object->hear_distance = creature->hear_distance;
    object->max_view_height = creature->max_view_height;
    object->min_view_height = creature->min_view_height;
    offset.x = (f32)((object->ai_packet.group_column + 1) / 2) * creature->x_spacing;
    if ((object->ai_packet.group_column & 1) != 0) {
        offset.x = -offset.x;
    }
    offset.y = 0.0f;
    offset.z = -creature->z_spacing * (f32)object->ai_packet.group_row;
    if (object->ai_packet.respawn_count != 0 && object->ai_packet.respawn_locator != NULL) {
        angle = object->ai_packet.respawn_locator->rotation;
        NuVecRotateY(&offset, &offset, angle);
        NuVecAdd(&object->position, &offset, &object->ai_packet.respawn_locator->position);
        path_info = &object->ai_packet.respawn_locator->path_info;
        object->ai_packet.respawn_locator = NULL;
    } else {
        angle = creature->y_rotation;
        NuVecRotateY(&offset, &offset, angle);
        NuVecAdd(&object->position, &offset, &creature->position);
        path_info = &creature->path_info;
    }
    object->collision_position = object->position;
    object->initial_position = object->position;
    plr_lastpos = object->collision_position;
    object->start_position = object->collision_position;
    object->reset_velocity = v000;
    object->velocity = object->reset_velocity;
    InitSurfaceInfo(object);
    SetObjOnSurface(object, 0);
    object->field_0x276 = (u16)angle;
    object->facing_angle = (u16)angle;
    object->movement_facing_angle = (u16)angle;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, (u16)angle);
    AISysCharacterSetPath(&object->ai_packet, path_info->path);
    AISysCharacterSetPathCnx(&object->ai_packet, &object->position, path_info->connection, path_info->direction);
    object->dead = 0;
    AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, creature,
                          ((AI_CREATURE_RESET_VIEW_PC *)creature)->script_name, NULL, 1, NULL, NULL);
    object->ai_packet.inside_path_node = -1;
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, (i8)object->field_281);
    ResetAnimPacket(&object->anim_packet, 1);
    SetAnimTimeRandom(object->character_model, &object->anim_packet);
    object->ai_packet.movement_stopping_distance = 0.0f;
    ((AI_RESET_EXTRA_PC *)object)->reset_timer = 0.0f;
    object->ai_packet.flags &= ~0x1000000u;
    object->ai_packet.movement_destination = object->position;
    object->ai_packet.path_target = NULL;
    ((AI_RESET_EXTRA_PC *)object)->search_state = 0;
    ((AI_RESET_EXTRA_PC *)object)->avoidance_state = 0;
    PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
    object->flags_1fc_word |= 0x1000;
    ++object->ai_packet.respawn_count;
    object->ai_packet.reset_mode = 2;
    if (object->ai_packet.group != NULL) {
        object->ai_packet.group->member_is_alive |= (u32)((u64)1 << object->ai_packet.group_member_index);
        row = &object->ai_packet.group->rows[object->ai_packet.group_row];
        row->is_alive |=
            (u8)((u64)1 << (object->ai_packet.group_member_index -
                            ((AI_CREATURE_RESET_VIEW_PC *)creature)->count_across * object->ai_packet.group_row));
        if (object->ai_packet.group_column == 0) {
            row->position = object->position;
            row->path_info = object->ai_packet.path_info;
            row->rotation = object->field_0x276;
            row->flags &= ~2u;
            row->next_connection = NULL;
        }
    }
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    if ((object->character_data->model_flags & 0x20000000) != 0 &&
        object->character_model->action_animations[0x41] != NULL) {
        object->context_timer = 1000000000.0f;
        object->character_context = 0x17;
        object->field_7aa = 0x41;
        ResetAnimPacket(&object->anim_packet, 0x41);
    }
}

void __cdecl ResetAICreatures(AISYS_PC *system) {
    GAMEOBJECT *object;
    AICREATURE_PC *creature;
    i32 index;
    if (system == NULL || (((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags & 1) != 0) {
        return;
    }
    ((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags |= 1;
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1f8 & 0x400) == 0) {
            continue;
        }
        object->flags_1fc_word &= ~0x1000u;
        if (WORLD->level_progress != NULL &&
            (((u64)1 << index) & ((LEVEL_AI_PROGRESS_PC *)WORLD->level_progress)->disabled_ai_objects) != 0) {
            object->dead = 1;
        } else {
            creature = &system->creatures[object->ai_packet.creature_index];
            object->ai_packet.respawn_count = 0;
            object->ai_packet.reset_mode = 0;
            AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, creature,
                                  ((AI_CREATURE_RESET_VIEW_PC *)creature)->script_name,
                                  (char *)GameObjectState_InActive, 1, NULL, NULL);
            if (object->ai_packet.script_processor.state != NULL &&
                object->ai_packet.script_processor.state->name != NULL &&
                NuStrICmp(object->ai_packet.script_processor.state->name, GameObjectState_InActive) == 0) {
                creature->activation_type = 2;
            }
            if (creature->activation_type != 0) {
                continue;
            }
            if (Game.difficulty >= creature->activation_difficulty) {
                if (creature->count > 1 && creature->start_stagger > 0.0f &&
                    object->ai_packet.group_member_index != 0) {
                    object->ai_packet.spawn_delay = (f32)object->ai_packet.group_member_index * creature->start_stagger;
                    object->ai_packet.reset_mode = 1;
                } else {
                    ResetAICreature(object, system);
                }
                continue;
            }
        }
        object->ai_packet.reset_mode = 4;
    }
}

i32 __cdecl InitCreature(GAMEOBJECT *object, i32 id, i32 parameter) {
    NUVEC start;
    void **animations;
    i32 animation;
    addcreature_override_id_check = 0;
    if (id < 0) {
        return 0;
    }
    if (id >= 0x154) {
        return 0;
    }
    if (apicharsys->playermodelids[id] == -1) {
        return 0;
    }
    object->character_data = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    object->layer_mask = object->character_data->game_character->layer_mask;
    object->flags_1f8 = 0;
    object->level_object_index = -1;
    memset(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, 0, 0x788);
    object->field_280 = -1;
    start = *Player_StartPos(object);
    object->flags_1f8 |= 2;
    if (parameter != 0) {
        object->flags_1f8 |= 0x4000;
    }
    if ((WORLD->current_level->flags & 0x40000) == 0) {
        object->flags_1f8 |= 0x40;
    }
    object->pad = GamePad_Allocate();
    object->pad->input_mode = 1;
    object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
    ResetPlayerPacket(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, object->character_data);
    object->movement_direction = v000;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->suit = Suit_GetDefault((i16)id);
    object->ai_packet.creature_index = 0xff;
    SetGameObjectCharacterData(object);
    object->death_delay = 0.5f;
    object->start_position = start;
    object->position = start;
    object->initial_position = start;
    object->collision_position = start;
    GetTopBot(object);
    GameObjectDimensions(object);
    object->animation_index = 1;
    if (object->character_model != NULL) {
        animations = object->character_model->action_animations;
        if (animations[1] == NULL) {
            for (animation = 0; animation < 0xe9; ++animation, ++animations) {
                if (*animations != NULL) {
                    break;
                }
            }
            if (animation < 0xe9) {
                object->animation_index = (i16)animation;
            }
        }
    }
    ResetAnimPacket(&object->anim_packet, object->animation_index);
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
    object->ai_packet.mover_height = object->bounds_radius + default_mover_extra;
    GameObjectOrigin(object);
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->flags_ef4 =
        (object->flags_ef4 & ~0x800u) | ((object->character_data->game_character->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN) {
        SetLayersMosPc(object);
    } else if (object->id == (u16)id_CANTINAALIEN) {
        SetLayersCantinaPc(object);
    } else if (object->id == (u16)id_CLOUDCITYCITIZEN) {
        SetLayersCloudPc(object);
    } else if (object->id == (u16)id_BOB) {
        SetLayersBobPc(object);
    } else if (object->id == (u16)id_GEONOSIAN) {
        ((PLAYER_LAYER_FLAGS_PC *)&object->flags_ef0)->bob = qrand() < 0x8000;
    }
    InitCreatureModelFlagsPc(object);
    return 1;
}

i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *animation, NUMTX *matrix,
                                   NUMTX *secondary_matrix, NUMTX *reflection_matrix, NUMTX *auxiliary_matrix,
                                   GAMEOBJECT *object, u32 flags) {
    NUMTX special_matrix;
    NUMTX scaled_matrix;
    NUMTX scaled_reflection;
    NUMTX output_matrices[256];
    i32 drag_bomb;
    i32 complex_shadow;
    i32 result;
    i32 draw_shadow;
    i32 i;
    drag_bomb = 0;
    if (object != NULL && object->id == id_DRAGBOMB && Cheat_IsOn(5) && WORLD->level_objects[0x135].active != 0) {
        NUVEC half_scale;
        ResetShadowMapRendering();
        special_matrix = *matrix;
        drag_bomb = 1;
        NuSpecialDrawAt(&WORLD->level_objects[0x135].special, &special_matrix);
        if (reflection_matrix != NULL) {
            special_matrix = *reflection_matrix;
            NuRndrStartReflectionRender(1);
            NuSpecialDrawAt(&WORLD->level_objects[0x135].special, &special_matrix);
            NuRndrEndReflectionRender();
        }
        half_scale.x = half_scale.y = half_scale.z = 0.5f;
        scaled_matrix = *matrix;
        matrix = &scaled_matrix;
        NuMtxPreScale(matrix, &half_scale);
        if (reflection_matrix != NULL) {
            scaled_reflection = *reflection_matrix;
            reflection_matrix = &scaled_reflection;
            NuMtxPreScale(reflection_matrix, &half_scale);
        }
    }
    complex_shadow = 0;
    if (object != NULL && (object->character_data->model_flags & 0x10000) != 0 &&
        (WORLD->area == NULL || WORLD->area != BATTLEOVERCORUSCANT_ADATA)) {
        ((CHARACTER_DRAW_HIERARCHY_PC *)model->hierarchy)->suppress_shadow_surface_points = 0;
        if (reflection_matrix != NULL) {
            ConfigureComplexShadow(object);
            complex_shadow = 1;
        }
    } else {
        ((CHARACTER_DRAW_HIERARCHY_PC *)model->hierarchy)->suppress_shadow_surface_points = 1;
    }
    drawcharactermodel_keepmergeaction = GameKeepMergeActionPc;
    MakeLayerList = GCDataList[model->model_id].make_layer_list;
    result =
        APIDrawCharacterModel(model, object != NULL ? (APICHARACTERDATA *)object->character_data : NULL, animation,
                              matrix, secondary_matrix, reflection_matrix, NULL, auxiliary_matrix, object, flags,
                              object != NULL && GameDrawCharacterModelFlagPc != 0 ? object->joint_overrides : NULL,
                              object != NULL && GameDrawCharacterModelFlagPc != 0 ? object->joint_override_count : 0,
                              Paused, FRAMETIME, output_matrices, NULL, WORLD->game_debris_sys);
    if (result == 0) {
        if (object != NULL && auxiliary_matrix != NULL) {
            for (i = 0; i < 16; ++i) {
                if (model->points_of_interest[i] != NULL) {
                    auxiliary_matrix[i] = object->transform;
                    memcpy(&auxiliary_matrix[i].m30, &object->collision_position, sizeof(NUVEC));
                    auxiliary_matrix[i].m31 = (f32)((f64)auxiliary_matrix[i].m31 -
                                                    ((f64)object->character_top + (f64)object->character_bottom) *
                                                        (f64)object->origin_scale * 0.5);
                }
            }
        }
    }
    draw_shadow = result != 0 && (object == NULL || ((object->flags_ef0 & 0x4000000) == 0 &&
                                                     (object->character_data->model_flags & 0x10000) == 0));
    if (PcWindowOptions.shadowmethod == 1 && draw_shadow && secondary_matrix != NULL && object != NULL) {
        u8 level_alpha = WORLD->current_level->blob_shadow_alpha;
        if (level_alpha > 0 && object->shadow_opacity > 0.0f && object->surface_normal.y > 0.0f &&
            (object->water_height == -1.0f || (TerLayer[object->field_283].flags & 1) == 0)) {
            if ((i16)object->id == id_MOUSEDROID && WORLD->level_objects[0xa2].active != 0) {
                f32 opacity;
                special_matrix = *secondary_matrix;
                opacity = object->shadow_opacity * object->surface_normal.y;
                NuSpecialDrawAtAlpha(&WORLD->level_objects[0xa2].special, &special_matrix, opacity);
            } else if (CharacterBlobShadowsOnPc != 0) {
                PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
                i32 alpha = config->blob_shadow_alpha < 255 ? config->blob_shadow_alpha : level_alpha;
                f32 radius;
                if (alpha > 0) {
                    radius = config->field_08c < 99.0f ? config->field_08c : object->character_data->collision_radius;
                    if (radius > 0.0f) {
                        radius = DropInOutScale(object) * radius;
                        NuRndrAddShadow(&secondary_matrix->translation, object->origin_scale * radius,
                                        alpha * object->shadow_opacity * object->surface_normal.y, object->field_1052,
                                        0, object->field_1054);
                    }
                }
            }
        }
    }
    if (complex_shadow != 0) {
        ConfigureComplexShadow(NULL);
    }
    if (drag_bomb != 0) {
        EnableShadowMapRendering(0);
    }
    return result;
}

i32 __cdecl DrawGameObjectsProcess(void) {
    GAMEOBJECT *object = Obj;
    i32 index;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        NUVEC position, scale;
        f32 drop_scale;
        NUMTX matrix, orientation, reflected, shadow;
        i32 angle;
        object->flags_1f8 |= 0x800;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }
        object->flags_e24 &= ~8u;
        ((DRAW_PROCESS_FLAGS_PC *)object)->origin_joint_was_ready = object->origin_joint_ready;
        object->origin_joint_ready = 0;
        if (object->dead != 0 && (object->field_1010 > 0.0f || object->death_delay == 0.0f)) {
            continue;
        }
        if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24) {
            continue;
        }
        if (object->character_context == 0xf && object->field_7b3 == 1 &&
            ((DRAW_PREPASS_TELEPORT_OBJECT_PC *)object)->teleport != NULL &&
            (((DRAW_PREPASS_TELEPORT_OBJECT_PC *)object)->teleport->flags & 4) == 0) {
            continue;
        }
        if (object->character_context == 0x3b || object->character_context == 0x39) {
            continue;
        }
        position.x = object->position.x;
        position.y = object->position.y;
        if ((object->flags_1f8 & 0x100) != 0) {
            position.y = object->character_bottom * object->origin_scale + position.y;
        }
        position.z = object->position.z;
        NuVecAdd(&position, &position, &object->render_offset);
        ((DRAW_PROCESS_FLAGS_PC *)object)->previous_model_draw_result = object->model_draw_result;
        object->model_draw_result = 1;
        scale.x = object->origin_scale;
        drop_scale = DropInOutScale(object);
        scale.x = drop_scale * scale.x;
        scale.y = scale.z = scale.x;
        NuMtxSetScale(&matrix, &scale);
        switch ((u8)object->movement_mode) {
            case 0:
                GameDrawMtxSetYRotationPc(&orientation, 0x8000);
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                }
                angle = object->field_0x276;
                if (angle != 0) {
                    GameDrawMtxRotateYPc(&orientation, angle);
                }
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 1:
                GameDrawMtxSetYRotationPc(&orientation, object->field_0x276 + 0x8000);
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                }
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 3:
                GameDrawMtxSetYRotationPc(&orientation, 0x8000);
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                }
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                }
                angle = object->field_0x276;
                if (angle != 0) {
                    GameDrawMtxRotateYPc(&orientation, angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 4:
                matrix = object->vehicle_orientation;
                NuMtxPreScale(&matrix, &scale);
                NuMtxPreRotateY(&matrix, 0x8000);
                NuMtxTranslate(&matrix, &position);
                break;
            case 5:
                matrix = object->vehicle_orientation;
                NuMtxPreScale(&matrix, &scale);
                NuMtxPreRotateY(&matrix, 0x8000);
                break;
            default:
            case 2:
                GameDrawMtxSetYRotationPc(&orientation, object->field_0x276 + 0x8000);
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                }
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
        }
        if (object->movement_lean_angle != 0) {
            angle = (i16)object->movement_lean_angle;
            if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
                angle = (i32)((f64)PodSprint_RollMul(object) * angle);
            }
            NuMtxPreRotateZ(&matrix, angle);
        }
        if (object->secondary_lean_angle != 0) {
            NuMtxPreRotateX(&matrix, (i16)object->secondary_lean_angle);
        }
        if (object->tertiary_lean_angle != 0) {
            NuMtxPreRotateY(&matrix, (i16)object->tertiary_lean_angle);
        }
        ApplyExtraRotation(object, &matrix);
        if ((u8)object->movement_mode != 4 &&
            (object->character_context == 0x23 || object->character_context == 0x24)) {
            NUVEC displacement;
            f32 offset;
            offset = object->character_data->collision_radius * object->character_data->model_scale * 7.5;
            if (object->character_context == 0x23) {
                offset = (1.0f - drop_scale) * offset;
            } else {
                offset = -((1.0f - drop_scale) * offset);
            }
            NuVecMtxTransformVU0(&displacement, &v001, &orientation);
            NuVecScale(&displacement, &displacement, offset);
            NuVecAdd(NUMTX_GET_ROW_VEC(&matrix, 3), NUMTX_GET_ROW_VEC(&matrix, 3), &displacement);
        }
        if (object->use_model_origin < 2 && (object->id == GameObjectRotationAtstId ||
                                             object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT)) {
            matrix.m31 = (f32)((f64)object->character_bottom * object->origin_scale + matrix.m31);
        }
        object->transform = matrix;
        if (scale.x == 0.0f && scale.y == 0.0f && scale.z == 0.0f) {
            object->model_draw_result = 0;
            continue;
        }
        object->flags_1f8 ^= 0x800;
        object->flags_ef0 &= ~0x20000u;
        *(u8 *)&object->field_107c = 0;
        if (object->reflection_enabled != 0 && (f64)object->reflection_height != 2000000.0 &&
            object->ai_update_distance < WORLD->current_level->reflection_range) {
            *(u8 *)&object->field_107c =
                (u8)MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                     WORLD->current_level->reflection_y, &reflected);
            object->draw_tertiary_mtx = reflected;
        }
        if (object->dead == 0 && (f64)object->floor_height != 2000000.0 && (object->flags_1f8 & 0x40) != 0 &&
            object->character_data->game_character->shadow_locators == 0) {
            GameDrawMtxCopyPc(&shadow, &ANI3_EVAL_IDENTITY_MATRIX);
            GameDrawMtxRotateYPc(&shadow, object->field_0x276 + 0x8000);
            if (object->field_1054 != 0) {
                GameDrawMtxRotateZPc(&shadow, object->field_1054);
            }
            if (object->field_1052 != 0) {
                GameDrawMtxRotateXPc(&shadow, object->field_1052);
            }
            shadow.m30 = object->position.x;
            object->flags_ef0 |= 0x20000;
            shadow.m31 = (f32)((f64)object->floor_height + (f64)0.005f);
            shadow.m32 = object->position.z;
            object->draw_secondary_mtx = shadow;
        }
    }
    return 0;
}

void DrawWeapons(GAMEOBJECT *object, i32 reflection, f32 weapon_scale) {
    PLAYERCHARACTERCONFIG_PC *data;
    NUMTX blade_matrix;
    NUMTX hilt_matrix;
    NUMTX reflected;
    NUMTX saved_matrix;
    NUVEC scale;
    NUVEC hilt_scale;
    NUMTX *matrix;
    i32 hand;
    i32 joint;
    i32 part;
    i32 count;
    i32 side;
    i32 passes;
    i32 model;
    i32 reflected_model;
    i32 red, green, blue, purple;
    u16 rotation = 0;
    i32 models[4] = {-1, -1, -1, -1};
    i32 hilt = -1;
    i32 reflected_models[4] = {-1, -1, -1, -1};
    i32 sabre = 0;
    if (weapon_scale <= 0.0f) {
        return;
    }
    if ((object->character_data->game_character->flags_094 & 0x200000) != 0) {
        return;
    }
    for (hand = 0; hand < 4; ++hand) {
        joint = object->character_data->game_character->weapon_joints[hand];
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
            red = 0;
            green = 0;
            blue = 0;
            purple = 0;
            if (Cheat_IsOn(15) && (object->character_data->game_character->weapon_action == 1 ||
                                   object->character_data->game_character->weapon_action == 8)) {
                models[0] = 0x59;
            } else {
                data = object->character_data->game_character;
                models[0] = data->weapon_model;
            }
            if (models[0] != -1) {
                if (object->id == (u16)id_JANGOFETT) {
                    rotation = 0xd1c8;
                } else if (models[0] == 0x65 || models[0] == 0x67 || models[0] == 0x69 || models[0] == 0x6b) {
                    if (object->field_280 != 0xff && Cheat_IsOn(25)) {
                        red = 1;
                    } else if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
                        purple = 1;
                    } else if (models[0] == 0x65) {
                        red = 1;
                    } else if (models[0] == 0x67) {
                        green = 1;
                    } else if (models[0] == 0x69) {
                        blue = 1;
                    } else {
                        purple = 1;
                    }
                    DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                    sabre = 1;
                }
            } else if (StartupGameFlag0 != 0) {
                if (object->weapon_model_override_pc != NULL) {
                    models[0] = *object->weapon_model_override_pc;
                }
                if (models[0] == -1) {
                    continue;
                }
            } else if (StartupGameFlag1 != 0) {
                if ((object->character_data->model_flags & 0x90) == 0x80) {
                    models[0] = 0xd;
                } else {
                    if (object->id == (u16)id_BOB) {
                        if ((object->flags_ef0 & 0x200) != 0) {
                            green = 1;
                        } else {
                            blue = 1;
                        }
                    } else if (AnakinGreenSabre(object)) {
                        green = 1;
                    } else if (data->streak_color == 0) {
                        red = 1;
                    } else if (data->streak_color == 1) {
                        green = 1;
                    } else if (data->streak_color == 2) {
                        blue = 1;
                    } else if (data->streak_color == 3) {
                        purple = 1;
                    }
                    if (object->field_280 != 0xff && Cheat_IsOn(25)) {
                        red = 1;
                        green = blue = purple = 0;
                    } else if (red || green || blue || purple) {
                        if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
                            purple = 1;
                            red = green = blue = 0;
                        } else if (green && object->id == (u16)id_GRIEVOUS && (hand == 0 || hand == 3)) {
                            blue = 1;
                            green = 0;
                        }
                    }
                    if (red || green || blue || purple) {
                        DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                        sabre = 1;
                    }
                }
            } else if (object->id == (u16)CharacterFixupIdPc007f1810 ||
                       (object->character_data->model_flags & 0x80) != 0) {
                models[0] = 0xc;
            }
            count = 0;
            for (part = 0; part < 4; ++part) {
                if (models[part] != -1) {
                    if (part != 3) {
                        ++count;
                    }
                    reflected_models[part] = LevelObject_GetReflection(models[part]);
                }
            }
            if (count > 0) {
                blade_matrix = object->locator_matrices[joint];
                scale.z = scale.y = scale.x = weapon_scale;
                if (hilt != -1) {
                    hilt_scale.x = weapon_scale + weapon_scale;
                    hilt_matrix = object->locator_matrices[joint];
                    if (hilt_scale.x > 1.0f) {
                        hilt_scale.x = 1.0f;
                    }
                    hilt_scale.z = hilt_scale.x;
                    hilt_scale.y = hilt_scale.x;
                    NuMtxPreScale(&hilt_matrix, &hilt_scale);
                }
                side = 0;
                if (rotation != 0) {
                    NuMtxPreRotateX(&blade_matrix, rotation);
                }
                NuMtxPreScale(&blade_matrix, &scale);
                saved_matrix = blade_matrix;
                passes = 1;
                do {
                    for (part = 0; part < 4; ++part) {
                        if (side && models[part] == hilt) {
                            continue;
                        }
                        model = models[part];
                        if (model == -1) {
                            continue;
                        }
                        if (part == 3) {
                            blade_matrix.m30 += object->weapon_trail_offset.x;
                            blade_matrix.m31 += object->weapon_trail_offset.y;
                            blade_matrix.m32 += object->weapon_trail_offset.z;
                        }
                        matrix = model == hilt ? &hilt_matrix : &blade_matrix;
                        Draw3DObjectMtx(NULL, model, matrix);
                        if (reflection) {
                            reflected_model = reflected_models[part];
                            if (reflected_model == -1 || !WORLD->level_objects[reflected_model].active) {
                                reflected_model = model;
                            }
                            if (MatrixReflection(matrix, object->reflection_enabled, object->reflection_height,
                                                 WORLD->current_level->reflection_y, &reflected)) {
                                NuRndrStartReflectionRender(0);
                                Draw3DObjectMtx(NULL, reflected_model, &reflected);
                                NuRndrEndReflectionRender();
                            }
                        }
                    }
                    if (!side && object->id == (u16)id_DARTHMAUL &&
                        ((object->field_280 != 0xff && Cheat_IsOn(25)) ||
                         (object->field_280 != 0xff && Player_HasPurpleForce(object)))) {
                        scale.z = scale.x = 1.0f;
                        scale.y = -1.0f;
                        blade_matrix = saved_matrix;
                        side = 1;
                        NuMtxPreScale(&blade_matrix, &scale);
                        ++passes;
                    }
                } while (--passes > 0);
            }
        }
        if (sabre && object->id != (u16)id_GRIEVOUS) {
            return;
        }
    }
}

static void __cdecl DrawCharacterAttachmentModelPc(GAMEOBJECT *object, i32 character_id, NUMTX *joint_matrices,
                                                   ANIMPACKET_PC *animation, i32 locator, NUMTX *override_matrix) {
    i16 model_index;
    CHARACTER_MODEL_LAYERS_PC *model;
    NUMTX matrix, reflected;
    NUMTX *reflection;
    if (override_matrix == NULL && (locator == -1 || object->character_model->points_of_interest[locator] == NULL)) {
        return;
    }
    model_index = apicharsys->playermodelids[character_id];
    if (model_index == -1) {
        return;
    }
    model = &apicharsys->models[model_index];
    if (override_matrix != NULL) {
        matrix = *override_matrix;
    } else {
        matrix = joint_matrices[locator];
    }
    if ((u8)object->field_107c != 0 && (f64)object->reflection_height != 2000000.0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                         WORLD->current_level->reflection_y, &reflected) != 0) {
        reflection = &reflected;
    } else {
        reflection = NULL;
    }
    GameDrawCharacterModel(model, animation, &matrix, NULL, reflection, NULL, NULL, 0xffffffffu);
}

void __cdecl DrawCharacterAttachments(GAMEOBJECT *object, NUMTX *joint_matrices) {
    i32 character_id;
    i32 current_animation;
    DRAW_ATTACHMENTS_PACKET_PC animation;
    NUMTX *override_matrix = NULL;
    if (object->suit != NULL && (object->suit->flags & 2) != 0) {
        if (object->id != (u16)ID_DEFAULTCHARACTER_A) {
            return;
        }
        character_id = AreaExtraTargetId7f17bcPc;
        AnimPacketMiniToFullPc((DRAW_ATTACHMENTS_MINI_PC *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)PunchRangeUnitIdPc && (AnimPlaying(&object->anim_packet, 0x51, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x55, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x56, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x94, 1, 1) != NULL)) {
        character_id = PunchRangeCompanionIdPc;
        memcpy(&animation, (u8 *)object + offsetof(GAMEOBJECT, anim_packet), sizeof(animation));
        if (animation.packet.blending != 0 && animation.packet.blend_target_animation != 0x51 &&
            animation.packet.blend_target_animation != 0x55 && animation.packet.blend_target_animation != 0x56 &&
            animation.packet.blend_target_animation != 0x94) {
            return;
        }
    } else if (object->id == (u16)AreaExtraSourceId7f17c8Pc) {
        character_id = AreaExtraTargetId7f17ccPc;
        AnimPacketMiniToFullPc((DRAW_ATTACHMENTS_MINI_PC *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)VehicleBoltId7f17d0Pc || object->id == (u16)VehicleBoltId7f17d4Pc) {
        if (AnimPacketModelsCompatiblePc(&object->anim_packet, (i16)object->id, PunchRangeCompanionIdPc) == 0) {
            return;
        }
        character_id = PunchRangeCompanionIdPc;
        memcpy(&animation, (u8 *)object + offsetof(GAMEOBJECT, anim_packet), sizeof(animation));
        override_matrix = &object->transform;
    } else {
        return;
    }
    if (character_id == -1) {
        return;
    }
    current_animation = CurrentAnim(&object->anim_packet);
    if (object->character_model->animations[current_animation] != NULL &&
        (object->character_model->animations[current_animation]->misc_flags & 8) != 0) {
        return;
    }
    DrawCharacterAttachmentModelPc(object, character_id, joint_matrices, &animation.packet,
                                   object->character_data->game_character->extra_character_locator, override_matrix);
}

void __cdecl DrawParaphernaliaPc(GAMEOBJECT *object) {
    i32 i;
    NUMTX matrix, reflected, special_reflected, attachments[16];
    PARAPHERNALIA_EXTRA_PC *extra;
    GAMEOBJECT *carried;
    PARAPHERNALIA_EXTRA_PC *carried_extra;
    i32 ride_locator;
    NUVEC carried_position;
    NUMTX *carried_reflection;
    u32 carried_layers;
    GAMEOBJECT *passenger;
    i32 passenger_locator;
    NUANGVEC passenger_rotation;
    PLAYERCHARACTERCONFIG_PC *config;
    i32 scene_locator;
    NUVEC glow_position;
    f32 glow_radius;
    f32 glow_alpha;
    f32 held_glow_alpha;
    i32 helmet_locator;
    f32 transform_alpha;
    f32 transform_radius;
    f32 *time;
    i32 special_locator;
    f32 distance;
    i32 chosen;
    f32 next;
    NUMTX *special_matrix;
    f32 start;
    f32 finish;
    f32 fraction;
    NUVEC special_scaling;
    i32 special_phase;

    extra = ((PARAPHERNALIA_EXTRA_PC *)(object));
    if (DrawParaphernaliaEnabledPc == 0) {
        return;
    }

    carried = object->takeover_source;
    if (carried != NULL && object->character_context != 0x3b &&
        (object->character_data->game_character->flags_098[0] & 0x40) == 0) {
        carried_extra = ((PARAPHERNALIA_EXTRA_PC *)(carried));
        carried_extra->draw_reflection = extra->draw_reflection;
        carried->reflection_enabled = object->reflection_enabled;
        carried->reflection_height = object->reflection_height;
        ride_locator = object->character_data->game_character->ride_locator;
        if (ride_locator != -1 && object->character_model->points_of_interest[ride_locator] != NULL) {
            matrix = object->locator_matrices[ride_locator];
        } else {
            carried_position.y = object->bounds_half_height;
            carried_position.x = 0.0f;
            NuVecMtxRotate(&carried_position, &carried_position, &object->transform);
            NuVecAdd(&carried_position, &carried_position, &object->collision_position);
            matrix = object->transform;
            matrix.translation = carried_position;
        }
        if (carried->reflection_enabled != 0 && carried->reflection_height != 2000000.0 &&
            object->ai_update_distance < ((u8)(WORLD->current_level->reflection_range)) &&
            MatrixReflection(&matrix, carried->reflection_enabled, carried->reflection_height,
                             WORLD->current_level->reflection_y, &reflected)) {
            carried_reflection = &reflected;
        } else {
            carried_reflection = NULL;
        }
        carried_layers = AdjustLayerBits(carried->character_data->game_character->layer_mask_medium, carried);
        if (GameDrawCharacterModel(carried->character_model, &carried->anim_packet, &matrix, NULL, carried_reflection,
                                   attachments, NULL, carried_layers)) {
            if (carried->helmet != 0) {
                DrawObjectOnCharacter(WORLD, carried, carried->helmet + 0xf9, NULL,
                                      carried->character_data->game_character->helmet_locator, -1, attachments,
                                      carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f, 1.0f);
            }
            if (Cheat_IsOn(2)) {
                DrawObjectOnCharacter(WORLD, carried, 0xe7, NULL, carried->character_data->game_character->head_locator,
                                      -1, attachments, carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f,
                                      1.0f);
            }
            DrawCharacterAttachments(carried, attachments);
            if (((i16)(carried->id)) == PermanentWeirdo1IdPc || ((i16)(carried->id)) == PermanentWeirdo2IdPc) {
                CustomiserDrawAccessoriesPc(CharacterCustomiser, carried, attachments);
            }
        }
    } else if ((object->flags_e24 & 1) != 0 && object->force_target != NULL) {
        passenger = object->force_target;
        passenger_locator = object->character_data->game_character->weapon_joints[0];
        if (passenger_locator != -1 && object->character_model->points_of_interest[passenger_locator] != NULL) {
            matrix = object->locator_matrices[passenger_locator];
        } else {
            passenger_rotation.x = -0x4000;
            passenger_rotation.y = object->field_0x276;
            NuMtxSetRotationXYVU0(&matrix, &passenger_rotation);
            matrix.m30 = object->position.x + NuTrigTable[object->field_0x276 >> 1] * object->bounds_radius * 1.75;
            matrix.m31 =
                object->collision_min.y + (object->collision_max.y - object->collision_min.y) * 0.60000002384185791;
            matrix.m32 = object->position.z +
                         NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff] * object->bounds_radius * 1.75;
        }
        GameDrawCharacterModel(passenger->character_model, &passenger->anim_packet, &matrix, NULL, NULL, NULL, NULL,
                               passenger->character_data->game_character->layer_mask_medium);
    }
    config = object->character_data->game_character;
    if ((config->flags_094 & 1) != 0) {
        if (BonusArea != 0 && VehicleArea != 0) {
            CharMiniKit_Draw(((i16)(object->id)), &object->transform, object->reflection_enabled,
                             object->reflection_height, WORLD->current_level->reflection_y);
        }
    } else if ((object->character_data->flags & 1) != 0) {
        scene_locator = config->thingy_locator;
        matrix = scene_locator != -1 && object->character_model->points_of_interest[scene_locator] != NULL &&
                         object->model_draw_result != 0
                     ? object->locator_matrices[scene_locator]
                     : object->transform;
        reflected = object->draw_tertiary_mtx;
        CharScene_Draw(WORLD, ((i16)(object->id)), &matrix, extra->draw_reflection ? &reflected : NULL);
    } else if (((i16)(object->id)) == id_CATAPULT &&
               (object->character_context != 0x0a || (object->context_variant_flags & 0x40) == 0 ||
                object->quick_shoot_bolt_id != -1 ||
                (object->context_timer < 0.5f && ((u32)(GameTimer.update_count)) % 6 < 3))) {
        dco_prerotatez = 0xc000;
        DrawObjectOnCharacter(WORLD, object, 0xf0, NULL, object->character_data->game_character->weapon_shoot_joints[0],
                              -1, object->locator_matrices, 0, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_data->game_character->weapon_action == 1 &&
        AnimPlaying(&object->anim_packet, 0x60, 1, 1) != NULL) {
        DrawObjectOnCharacter(WORLD, object, 9, NULL, object->character_data->game_character->weapon_joints[0], -1,
                              object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f,
                              1.0f);
    } else if (object->weapon_scale > 0.0f) {
        DrawWeapons(object, extra->draw_reflection, object->weapon_scale);
        if (object->field_d54 > 0.0f) {
            dco_locatorposonly = 1;
            DrawObjectOnCharacter(
                WORLD, object, 0x35, NULL, object->character_data->game_character->weapon_shoot_joints[0], -1,
                object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
        }
    } else {
        DrawWeapons(object, extra->draw_reflection, ParaphernaliaWeaponScalePc(object));
    }
    if (object->origin_scale_override > 0.0f || object->timer_d34 > 0.0f) {
        ResetShadowMapRendering();
        ParaphernaliaShieldPc(object, extra->draw_reflection);
        EnableShadowMapRendering(0);
    }
    if (object->communicate_blend > 0.0f) {
        ParaphernaliaCommunicatePc(object, extra->draw_reflection);
    }
    if (object->character_context == 0x22) {
        if ((WORLD->area != NULL && WORLD->area == HUB_ADATA) || WORLD->current_level == LevelFixupSlotPc0087b194 ||
            WORLD->current_level == DOOKUC_LDATA) {
            glow_position.x = object->collision_position.x;
            glow_position.y = object->character_bottom * object->origin_scale + object->position.y +
                              (object->character_top - object->character_bottom) * 0.5 * object->origin_scale;
            glow_position.z = object->collision_position.z;
            glow_radius =
                object->bounds_radius > object->bounds_half_height ? object->bounds_radius : object->bounds_half_height;
            glow_radius = glow_radius * 1.75;

            if (object->context_hover_target_y < 0.20000000298023224) {
                glow_alpha = object->context_hover_target_y / 0.20000000298023224;
            } else if (object->context_timer < 0.20000000298023224) {
                glow_alpha = object->context_timer / 0.20000000298023224;
            } else {
                glow_alpha = 1.0f;
            }
            DrawForceGlowSprite(&glow_position, glow_radius, 0xdf, glow_alpha, object);
        }
    } else if (extra->force_alpha > 0.0f && extra->force_radius > 0.0f &&
               WORLD->level_objects[object->field_e1e].active != 0 &&
               (object->character_data->game_character->flags_090 & 0x400) == 0) {
        held_glow_alpha = ((f32)(extra->force_alpha / FORCEGLOWTIME));
        DrawForceGlowSprite(&extra->force_position, extra->force_radius, object->field_e1e, held_glow_alpha, object);
    }
    if (Cheat_IsOn(2)) {
        DrawObjectOnCharacter(WORLD, object, 0xe7, NULL, object->character_data->game_character->head_locator, -1,
                              object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f,
                              1.0f);
    }
    if (object->helmet != 0) {
        if ((((i16)(object->id)) == id_CHEWBACCA || ((i16)(object->id)) == id_WOOKIEE) && object->helmet != 5 &&
            object->helmet != 6) {
            helmet_locator = 9;
        } else {
            helmet_locator = object->character_data->game_character->helmet_locator;
        }
        DrawObjectOnCharacter(WORLD, object, object->helmet + 0xf9, NULL, helmet_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->model_draw_result != 0 && object->character_data->game_character->thrust_locators != 0 &&
        object->thrust_effect_scale > 0.0f) {
        ParaphernaliaThrustPc(object, extra->draw_reflection);
    }
    if (object->character_context == 0x2d) {
        GizDrawBuildItPiece(object, extra->draw_reflection);
    }
    if (object->character_context == 0x48 && object->character_model->model_data_b[object->field_7aa] != NULL &&
        object->field_7b3 == 0) {
        ParaphernaliaPlacePc(object, 0xec, extra->draw_reflection,
                             object->character_data->game_character->place_locator);
    } else if (object->character_context == 0x49 && object->character_model->model_data_b[object->field_7aa] != NULL &&
               object->field_7b3 != 0) {
        ParaphernaliaRemovePc(object, 0xec, extra->draw_reflection,
                              object->character_data->game_character->place_locator);
    }
    if ((object->flags_e20 & 0x400000) != 0) {
        dco_prerotatez = 0x4000;
        DrawObjectOnCharacter(WORLD, object, BatarangGetObjectFromCharIdPc(((i16)(object->id))), NULL,
                              object->character_data->game_character->throw_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_context == 0x46) {
        Grapple_DrawLine(object);
    }
    if ((object->flags_e20 & 0x800000) != 0 || object->character_context == 0x58) {
        SuperCarry_DrawObject(object);
    }
    DrawCharacterAttachments(object, object->locator_matrices);
    if (object->character_context == 0x5d) {
        ParaphernaliaHandPc(object, extra->draw_reflection);
    }
    if (object->torpedo != NULL && object->torpedo->count > 0) {
        DrawTorpedos(object);
    }
    if (object->character_data->game_character->weapon_action == 0) {
        CustomiserDrawAccessoriesPc(CharacterCustomiser, object, NULL);
    }
    if ((object->character_data->game_character->flags_090 & 0x1000000) != 0 && extra->force_alpha > 0.0f &&
        extra->force_radius > 0.0f) {
        transform_alpha = ((f32)(extra->force_alpha * 0.60000002384185791 + 0.40000000596046448));
        transform_radius =
            ((f32)((extra->force_alpha * 0.20000004768371582 + 1.3999999761581421) * extra->force_radius));
        TransformDrawTargetPc(&extra->force_position, transform_radius, transform_alpha);
    }
    config = object->character_data->game_character;

    if (config->weapon_action == 1) {
        special_locator = config->thingy_locator;
        if (special_locator == -1 || object->character_model->points_of_interest[special_locator] == NULL ||
            (time = AnimPlaying(&object->anim_packet, 0x6b, 1, 1)) == NULL) {
            return;
        }
        if ((*time >= 70.0f && *time <= 210.0) || (*time >= 310.0f && *time <= 433.0)) {
            matrix = object->locator_matrices[special_locator];
            NuMtxPreRotateZ(&matrix, -0x4000);
            NuSpecialDrawAt(&WORLD->level_objects[10].special, &matrix);
            if (extra->draw_reflection != 0 &&
                MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                 WORLD->current_level->reflection_y, &special_reflected)) {
                NuSpecialDrawAt(&WORLD->level_objects[10].special, &special_reflected);
            }
        } else {
            distance = 1.0f;
            chosen = -1;
            for (i = 10; i < 18; ++i) {
                if (NuSpecialExistsFn(&LevHSpecial[i])) {
                    next = NuVecDistSqr(&object->collision_position, NuSpecialGetDrawPos(&LevHSpecial[i]), NULL);
                    if (next < distance) {
                        distance = next;
                        chosen = i;
                    }
                }
            }
            if (chosen != -1) {
                special_matrix = NuSpecialGetDrawMtx(&LevHSpecial[chosen]);
                NuSpecialDrawAt(&WORLD->level_objects[10].special, special_matrix);
            }
        }
    } else if (((i16)(object->id)) == CharacterFixupIdPc007f1820 &&
               (time = AnimPlaying(&object->anim_packet, 1, 0, 0)) != NULL) {
        start = AnimListFrame(object->character_model, 1, 0);
        if (start >= 1.0f) {
            finish = AnimListFrame(object->character_model, 1, 1);
            if (finish >= start && *time > start && *time < finish) {
                fraction = (*time - start) / (finish - start);

                if (fraction < 0.10000000149011612) {
                    special_scaling.x = fraction / 0.10000000149011612;
                } else if (fraction < 0.89999997615814209) {
                    special_scaling.x = 1.0f;
                } else {
                    special_scaling.x = 1.0 - (fraction - 0.89999997615814209) / 0.10000000149011612;
                }
                special_scaling.y = special_scaling.z = special_scaling.x;
                NuMtxSetScale(&matrix, &special_scaling);
                NuMtxRotateX(&matrix, ((u16)(((i32)(fraction * -65536.0)))));
                NuMtxRotateY(&matrix, object->field_0x276);
                NuMtxTranslate(&matrix, &object->locator_matrices[0].translation);
                special_phase = ((i32)(fraction * 32768.0));
                matrix.m31 = matrix.m31 + NuTrigTable[(special_phase >> 1) & 0x7fff] * 0.20000000298023224;
                DrawObjectOnCharacter(WORLD, object, 0xf9, NULL, 0, -1, &matrix, extra->draw_reflection,
                                      object->layer_mask, NULL, NULL, 1.0f, 1.0f);
            }
        }
    }
}

void DrawGameObjectsDraw(i32 pass) {
    GAMEOBJECT *object;
    i32 index;
    u8 drawn;

    EnableShadowMapRendering(0);
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; index++, object++) {
        if ((object->flags_1f8 & 0x800) != 0) {
            if (object->rider != NULL && object->character_context == 0x3b) {
                object->model_draw_result = object->rider->model_draw_result;
            }
            continue;
        }
        drawn = (u8)GameDrawCharacterModel(object->character_model, &object->anim_packet, &object->transform,
                                           (object->flags_ef0 & 0x20000) != 0 ? &object->draw_secondary_mtx : NULL,
                                           (u8)object->field_107c != 0 ? &object->draw_tertiary_mtx : NULL,
                                           object->locator_matrices, object, object->layer_mask);
        object->model_draw_result = drawn;
        ((DRAW_OBJECTS_FLAGS_PC *)object)->locators_updated = drawcharactermodel_locatorsupdated;
        if (drawn != 0) {
            DrawParaphernaliaPc(object);
        } else if (((object->flags_e20 & 0x400) != 0 || object->character_context == 8) &&
                   object->transform_alpha > 0.0f && object->transform_target_scale > 0.0f &&
                   WORLD->level_objects[object->field_e1e].active != 0 &&
                   (object->character_data->game_character->flags_090 & 0x400) == 0) {
            DrawForceGlowSprite(&((PARAPHERNALIA_EXTRA_PC *)object)->force_position, object->transform_target_scale,
                                object->field_e1e, (f32)(object->transform_alpha / FORCEGLOWTIME), object);
        }
        if (object->snake_body != NULL) {
            DrawSnakeBody(object);
        }
        object->origin_joint_ready = 1;
        if (object->model_draw_result != 0) {
            object->flags_ef0 |= 0x40000;
        }
        if (Paused == 0 && object->character_data->draw_fn != NULL) {
            ((void(__cdecl *)(GAMEOBJECT *))object->character_data->draw_fn)(object);
        }
    }
    ResetShadowMapRendering();
}

void DrawGameObjects() {
    const f32 saved_far_clip = global_camera.unknown_64;
    global_camera.unknown_64 = VehicleArea != 0 ? DrawGameObjectsVehicleFarClipPc : DrawGameObjectsFarClipPc;
    if (global_camera.unknown_64 > saved_far_clip) {
        global_camera.unknown_64 = saved_far_clip;
    }
    NuCameraSet(&global_camera);
    DrawGameObjectsDraw(0);
    global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
}

GAMEOBJECT *__cdecl ActivateCharacter(char *name, NUVEC *position, i32 angle) {
    GAMEOBJECT *object;
    if (Mission_Active(NULL) != NULL) {
        return NULL;
    }
    if (name != NULL && (object = GetNamedGameObject(WORLD->ai_sys, name)) != NULL &&
        (object->flags_1fc_word & 0x1000) == 0 && (FreePlay == 0 || (object->flags_1f8 & 0x400) != 0)) {
        if (object->ai_packet.creature_index == 0xff) {
            object->flags_1fc_word |= 0x1000;
            AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "Base");
            if (position != NULL) {
                object->position = *position;
                object->movement_facing_angle = object->facing_angle = object->field_0x276 = (u16)angle;
                AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
                object->initial_position = object->position;
                object->collision_position = object->position;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->respawn_position = object->position;
                object->last_safe_position = object->position;
                object->ai_update_position = object->position;
                object->reset_velocity = v000;
                object->velocity = object->reset_velocity;
                InitSurfaceInfo(object);
                SetObjOnSurface(object, 0);
            }
        } else {
            ResetAICreature(object, WORLD->ai_sys);
        }
        LightGameObject(object, WORLD->rtl_set);
        return object;
    }
    return NULL;
}

GAMEOBJECT *__cdecl AddCreature(i32 id, i32 parameter) {
    GAMEOBJECT *object;
    if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1) {
        return NULL;
    }
    object = AddGameObject();
    if (object == NULL) {
        return NULL;
    }
    InitCreature(object, id, parameter);
    return object;
}

void __cdecl Players_Init(void) {
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    PLAYER_INIT_PROGRESS_PC *progress = (PLAYER_INIT_PROGRESS_PC *)PlayerProgress;
    i16 *list;
    i32 index;
    for (index = 0; index < 8; ++index) {
        players[index] = NULL;
    }
    if (netclient != 0) {
        return;
    }
    PLAYERCOUNT = 0;
    if ((WORLD->current_level->flags & 2) != 0) {
        if (FreePlay == 0 && (Hub_UsePlayerList == 0 || HUB_ADATA == NULL || WORLD->area != HUB_ADATA) &&
            (UsePlayerList != 1 ||
             (PlayerList[0] != -1 && (apicharsys->playermodelids[PlayerList[0]] == -1 ||
                                      (PlayerList[1] != -1 && apicharsys->playermodelids[PlayerList[1]] == -1))))) {
            list = Area_PlayerIDList;
        } else {
            list = PlayerList;
        }
        if (LevelChangesInArea == 0 && UsePlayerList != 0 && progress[0].active == 0 && progress[1].active != 0 &&
            FreePlay == 0 && (WORLD->area->flags & 5) == 1 && list[0] != -1 &&
            list[0] == Area_StoryModelList[0].model_id && list[1] != -1 && list[1] == Area_StoryModelList[1].model_id &&
            list[0] != list[1]) {
            i16 first = list[0];
            list[0] = list[1];
            list[1] = first;
        }
        for (index = 0; list[index] != -1 && index < 8; ++index) {
            i32 id = list[index];
            GAMEOBJECT *object;
            PLAYER_INIT_EXTRA_PC *extra;
            APIOBJECT_PC *api;
            if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1) {
                continue;
            }
            object = AddGameObject();
            if (object == NULL) {
                continue;
            }
            InitCreature(object, id, 1);
            PreResetCode(object);
            PostResetCode(object);
            extra = (PLAYER_INIT_EXTRA_PC *)object;
            api = (APIOBJECT_PC *)object;
            players[PLAYERCOUNT] = object;
            object->previous_sock_position = &OldPlrSPos[PLAYERCOUNT];
            players[PLAYERCOUNT]->field_280 = (u8)PLAYERCOUNT;
            ((PLAYER_INIT_EXTRA_PC *)players[PLAYERCOUNT])->batarang = &BatarangPacketsPc[PLAYERCOUNT];
            players[PLAYERCOUNT]->torpedo = GetTorpedoPacket();
            ++PLAYERCOUNT;
            object->flags_1044 |= 1;
            object->field_107e = (u8)DEFAULT_PLAYERHITPOINTS;
            object->helmet = 0;
            if (UsePlayerList != 0) {
                i32 slot = (i8)object->field_280;
                object->player_controlled = PlayerProgress[slot].active;
                if (Area != last_area) {
                    object->flags_e20 &= ~0x10000u;
                    extra->previous_state = 0;
                } else if (UsePlayerList == 1) {
                    ((PLAYER_INIT_FLAGS_PC *)object)->stored_flag = PlayerProgress[slot].field_7;
                    object->current_hp = PlayerProgress[slot].hitpoints;
                    if (object->current_hp > object->field_107e) {
                        object->current_hp = (i8)object->field_107e;
                    }
                    object->helmet = PlayerProgress[slot].field_a;
                    extra->previous_state = PlayerProgress[slot].unknown_0004;
                    object->powerup_time = PlayerProgress[slot].field_c;
                    object->suit = PlayerSuit[slot];
                    if (object->torpedo != NULL) {
                        object->torpedo->count = PlayerTorpedoCount[slot];
                    }
                }
            } else {
                if (object->hostility_group == 0) {
                    object->flags_1fc_word |= 0x80;
                } else {
                    object->flags_1fc_word &= ~0x80u;
                }
                progress[object->hostility_group].hitpoints = object->current_hp;
                extra->previous_state = 0;
            }
            api->view_distance = apicharsys->char_data[(i16)object->id].game_character->viewdistance;
            api->hear_distance = apicharsys->char_data[(i16)object->id].game_character->heardistance;
            api->max_view_height = apicharsys->char_data[(i16)object->id].game_character->maxviewheight;
            api->min_view_height = apicharsys->char_data[(i16)object->id].game_character->minviewheight;
            object->ai_packet.flags &= ~0x5000u;
            object->portal_flags &= ~0x801u;
            object->flags_ef4 |= 0x40;
            object->ai_packet.target_metric_e0 = 1.0e9f;
            object->ai_packet.target_metric_f0 = 1.0e9f;
            object->ai_packet.nearest_opponent = NULL;
            object->ai_packet.target_dc = NULL;
            object->ai_packet.opponent = NULL;
            object->ai_packet.target_ec = NULL;
            object->field_ec4 = 0.0f;
            extra->field_454 = 0.0f;
            object->ai_awareness_mask = 0;
            object->ai_seen_mask = 0;
            object->field_eb0 = 0;
            object->field_eb4 = 0;
            object->opponent = NULL;
            object->last_attacker = NULL;
            object->alert_target = NULL;
            object->ai_opponent_exclusion_mask[0] = 0;
            object->ai_opponent_exclusion_mask[1] = 0;
            object->timer_ecc = 0.0f;
        }
    }
    UsePlayerList = 0;
    for (index = 0; index < 2; ++index) {
        if (players[index] != NULL) {
            players[index]->coin_packet = &CoinPacket[index];
            ((PLAYER_COIN_PACKET_PC *)players[index]->coin_packet)->last_coin = LEGOOBJ_DEFAULTLASTCOIN.low_word;
            if (Area == last_area) {
                ((PLAYER_COIN_PACKET_PC *)players[index]->coin_packet)->coins = progress[index].coins;
            } else {
                ((PLAYER_COIN_PACKET_PC *)players[index]->coin_packet)->coins = 0;
            }
            players[index]->gizforce_los_info = &GizForceLOSInfo[index];
            memcpy(&BackUpPlayers[index], players[index], 0x10d8);
        }
    }
    RememberPlayerIDs(0, players[0] != NULL ? (i16)players[0]->id : -1, players[1] != NULL ? (i16)players[1]->id : -1);
}

void __cdecl InitAICreatures(AISYS_PC *system) {
    i32 creature_index, member, type;
    AICREATURE_PC *creature;
    GAMEOBJECT *object;
    AIGROUP_PC *group;
    u32 model_flags;
    if (NOAICREATURES != 0 || Mission_Active(NULL) != NULL || netclient != 0 || system == NULL) {
        return;
    }
    for (creature_index = 0; creature_index < system->creature_count; ++creature_index) {
        creature = &system->creatures[creature_index];
        group = NULL;
        for (member = 0; member < creature->count; ++member) {
            if (((u64)creature->active_mask & ((u64)1 << member)) == 0) {
                continue;
            }
            type = creature->type;
            if ((u32)type > 339 || apicharsys->playermodelids[type] == -1) {
                continue;
            }
            object = AddGameObject();
            if (object == NULL) {
                continue;
            }
            InitCreature(object, type, 1);
            object->flags_1f8 |= 0x400;
            object->flags_1fc_word &= ~0x1000u;
            model_flags = apicharsys->char_data[creature->type].model_flags;
            if ((model_flags & 0x200) != 0) {
                object->flags_1f8 |= 4;
            } else if ((model_flags & 4) != 0) {
                object->flags_1f8 |= 1;
            }
            object->flags_1044 |= 1;
            if ((apicharsys->char_data[creature->type].model_flags & 0x1000) != 0) {
                object->flags_1044 |= 4;
            }
            object->ai_packet.creature_index = (u8)creature_index;
            if (member == 0 && creature->count > 1 && creature->start_stagger == 0.0f) {
                group = CreateAIGroup(system, creature->count_across, creature->x_spacing, creature->z_spacing,
                                      object->character_data->game_character->run_speed);
            }
            if (group != NULL) {
                AddToAIGroup(group, (APIOBJECT_PC *)object);
            }
            ((AI_CREATURE_INIT_PACKET_PC *)&object->ai_packet)->area = creature->area;
            ((AI_CREATURE_INIT_PACKET_PC *)&object->ai_packet)->locator = creature->locator;
            object->ai_packet.respawn_locator = (AILOCATOR_PC *)creature->respawn_locator;
            object->ai_packet.creature_set = creature->set;
        }
    }
    system->reset_flags &= ~1u;
}
