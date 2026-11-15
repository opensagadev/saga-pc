#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gamelib/util/CRC16.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/snake.h"
#include "legoapi/actions/character/specialmoves.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/combat/shovesys.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/ai/core/aisysall.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/charplatforms.h"
#include "legoapi/characters/motion/charshadows.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/supportall.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/batman_gameai.h"
#include "legogame/game.h"
#include "legogame/pcbatman.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>

typedef struct PLAYER_PACKET_RESET { // size 0x788
    u8 unknown_000[0x101];           // 0x000
    u8 context_state_flags;          // 0x101
    u8 unknown_102[0x4d2];           // 0x102
    NUVEC reset_up_direction;        // 0x5d4
    GAMEPAD *gamepad;                // 0x5e0
    u8 unknown_5e4[0x20];            // 0x5e4
    void *unknown_604;               // 0x604
    u8 unknown_608[0xc];             // 0x608
    u32 unknown_614[8];              // 0x614
    u8 unknown_634[0x28];            // 0x634
    i32 input_state;                 // 0x65c
    f32 unknown_660;                 // 0x660
    u8 unknown_664[0x20];            // 0x664
    f32 delayed_turn_timer;          // 0x684
    f32 unknown_688;                 // 0x688
    u8 unknown_68c[0x4];             // 0x68c
    f32 unknown_690;                 // 0x690
    u8 unknown_694[0x24];            // 0x694
    f32 unknown_6b8;                 // 0x6b8
    u8 unknown_6bc[0x1c];            // 0x6bc
    f32 unknown_6d8;                 // 0x6d8
    f32 ground_height;               // 0x6dc
    u8 unknown_6e0[0xc];             // 0x6e0
    f32 unknown_6ec;                 // 0x6ec
    u8 unknown_6f0[0x18];            // 0x6f0
    f32 unknown_708;                 // 0x708
    f32 unknown_70c;                 // 0x70c
    f32 unknown_710;                 // 0x710
    u8 unknown_714[0x4];             // 0x714
    f32 unknown_718;                 // 0x718
    u8 unknown_71c[0x10];            // 0x71c
    f32 unknown_72c;                 // 0x72c
    u8 unknown_730[0x1c];            // 0x730
    i16 movement_angle_0;            // 0x74c
    i16 movement_angle_1;            // 0x74e
    u8 unknown_750[0x4];             // 0x750
    u16 movement_lean_angle;         // 0x754
    u16 secondary_lean_angle;        // 0x756
    u16 tertiary_lean_angle;         // 0x758
    i16 force_glow_index;            // 0x75a
    u32 secondary_flags;             // 0x75c
    u32 render_flags;                // 0x760
    u8 random_state[4];              // 0x764
    u8 unknown_768[0x17];            // 0x768
    i8 surface_type;                 // 0x77f
    u8 unknown_780[0x4];             // 0x780
    f32 unknown_784;                 // 0x784
} PLAYER_PACKET_RESET;

typedef struct GAMEPAD_RESET {     // size 0x5c
    void *bound_pad;               // 0x00
    u32 buttons_held;              // 0x04
    u32 buttons_pressed;           // 0x08
    u32 buttons_released;          // 0x0c
    u32 left_directions;           // 0x10
    u32 previous_left_directions;  // 0x14
    u32 right_directions;          // 0x18
    u32 previous_right_directions; // 0x1c
    u32 unknown_20;                // 0x20
    u8 unknown_024[0x1c];          // 0x24
    f32 rumble_field_40;           // 0x40
    f32 rumble_amount;             // 0x44
    f32 rumble_field_48;           // 0x48
    f32 rumble_time;               // 0x4c
    u8 unknown_050[0x4];           // 0x50
    void *operator_data;           // 0x54
    u8 unknown_058[0x2];           // 0x58
    u16 runtime_flags;             // 0x5a
} GAMEPAD_RESET;

typedef struct TERRAIN_PLAYER_IMPACT { // size 0x1c
    NUVEC position;                    // 0x00
    NUVEC normal;                      // 0x0c
    u8 material[2];                    // 0x18
    u8 flags;                          // 0x1a
    u8 normal_flags;                   // 0x1b
} TERRAIN_PLAYER_IMPACT;

typedef struct TERRAIN_PLAYER_PLATFORM_RECORD { // size 0x14
    u8 unknown_0[0xe];                          // 0x00
    i16 platform_id;                            // 0x0e
    GAMEOBJECT *object;                         // 0x10
} TERRAIN_PLAYER_PLATFORM_RECORD;

typedef struct TERRAIN_PLAYER_PLATFORM_SYS {   // size 0x1c
    u8 unknown_0[4];                           // 0x00
    i32 count;                                 // 0x04
    TERRAIN_PLAYER_PLATFORM_RECORD entries[1]; // 0x08
} TERRAIN_PLAYER_PLATFORM_SYS;

typedef struct TERRAIN_PLAYER_GRABBER { // size 0x564
    u8 unknown_0[0x554];                // 0x000
    f32 platform_contact_timer;         // 0x554
    u8 unknown_558[8];                  // 0x558
    i16 platform_id;                    // 0x560
} TERRAIN_PLAYER_GRABBER;

typedef struct TERRAIN_PLAYER_WORLD { // size 0x46f0
    u8 unknown_0[0x46ec];             // 0x0000
    TERRAIN_PLAYER_GRABBER *grabber;  // 0x46ec
} TERRAIN_PLAYER_WORLD;

typedef struct TERRAIN_PLAYER_BLOWUP { // size 0x104
    u8 unknown_0[0x102];               // 0x000
    i16 platform_id;                   // 0x102
} TERRAIN_PLAYER_BLOWUP;

typedef struct TERRAIN_PLAYER_COINS { // size 0x8
    u32 coins;                        // 0x0
    f32 scale;                        // 0x4
} TERRAIN_PLAYER_COINS;

typedef struct TERRAIN_PLAYER_SET { // size 0x1bb0
    u8 unknown_0[0x1bac];           // 0x0000
    i32 minimum_height;             // 0x1bac
} TERRAIN_PLAYER_SET;

typedef struct TERRAIN_PLAYER_CHARACTER { // size 0x1c
    u8 unknown_0[0x18];                   // 0x00
    void (*move_callback)(GAMEOBJECT *);  // 0x18
} TERRAIN_PLAYER_CHARACTER;

void TakeOverCode(GAMEOBJECT *, i32);
void ApplyGravity(GAMEOBJECT *, f32 *, f32, f32, f32 *);
void Tag_Check(GAMEOBJECT *);
void GameObjectRotation(GAMEOBJECT *, i32);
#define AIPATHCNX_FLAG_SET(connection, field, flag) ((connection) != NULL && ((connection)->field[0] & (u32)(flag)) != 0)

typedef struct level_ai_progress_s {
    u8 unknown_0000[0x2808];

    union {
        u32 disabled_ai_object_mask[2];
        u64 disabled_ai_objects;
    };
} LEVEL_AI_PROGRESS;

typedef struct AI_CREATURE_RESET_VIEW {
    u8 unknown_00[0x10];
    char script_name[0x10];
    u8 unknown_20[0x2c];
    u8 creature_set;
    u8 unknown_4d[4];
    u8 count_across;
} AI_CREATURE_RESET_VIEW;

typedef struct AI_RESET_EXTRA {
    u8 unknown_000[0x1086];
    u8 search_state;
    u8 avoidance_state;
    u8 unknown_1088[8];
    f32 reset_timer;
} AI_RESET_EXTRA;

typedef struct AI_CREATURE_RESET_FLAGS {
    u8 unknown_0000[0x1386];
    u16 reset_flags;
} AI_CREATURE_RESET_FLAGS;

void MovePlayer_VEHICLEDIRECTIONAL(GAMEOBJECT *object);
void MovePlayer_ROLLING(GAMEOBJECT *object);
i32 MovePlayer_TWIST(GAMEOBJECT *object);
i32 MovePlayer_CIRCLE(GAMEOBJECT *object);
i32 MovePlayer_GUNSHIPIN(GAMEOBJECT *object);
i32 MovePlayer_POD(GAMEOBJECT *object);
void MovePlayer_DIRECTIONAL(GAMEOBJECT *object);

typedef struct moveplayer_pad_bits_pc_s {
    u16 low : 3;
    u16 waggle : 1;
    u16 high : 12;
} MOVEPLAYER_PAD_BITS;

static __inline f32 PlayersFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

typedef struct CHARACTER_IDLE_STATE {
    u8 unknown_000[0x2b8];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;
} CHARACTER_IDLE_STATE;

typedef struct CHARACTER_IDLE_POLICY {
    u8 unknown_000[0x116];
    u8 use_standard_idle;
} CHARACTER_IDLE_POLICY;

f32 animduration_blendouttime;
i32 CharacterLoadAreaModels;
f32 DrawGameObjectsFarClip = 20.0f;
f32 DrawGameObjectsVehicleFarClip = 50.0f;
f32 TerrainPlayerOtherMovementScale = 2.5f;
f32 TerrainPlayerBikeMovementScale = 15.0f;
i16 ID_DEFAULTCHARACTER_A = -1;
i16 dat_007f17bc = -1;
i16 PunchRangeUnitId = -1;
i16 PunchRangeCompanionId = -1;
i16 dat_007f17c8 = -1;
i16 dat_007f17cc = -1;
i16 dat_007f17d4 = -1;
i16 dat_007f1810 = -1;
i16 dat_007f1820 = -1;
i16 id_MOUSEDROID = -1;
i32 waiting_for_character = -1;
i32 hub_character_ready = -1;
i16 dat_007f18c4;
i16 id_YODA = -1;
i16 id_YODAGHOST = -1;
i16 id_MOSEISLEYCITIZEN = -1;
i16 id_CANTINAALIEN = -1;
i16 id_CLOUDCITYCITIZEN = -1;
i16 id_GEONOSIAN = -1;
i16 id_CHEWBACCA = -1;
i16 id_WOOKIEE = -1;
i16 id_REPUBLICGUNSHIP = -1;
i16 id_REPUBLICGUNSHIP_GREEN = -1;
f32 default_mover_extra = 0.05f;
f32 TerrainPlayerPlatConnectTolerance = 0.01f;
f32 spline_seek_pos = 10.0f;
f32 spline_seek_ang = 10.0f;
f32 spline_xoffset_speed_seek = 4.0f;
f32 spline_xoffset_speed_roll = 16384.0f;
i32 TerrainPlayerShadowFix = 1;
i32 DrawParaphernaliaEnabled = 1;
f32 ComplexShadowBase = 25.0f;
i32 CharacterLoadAreaModels = 1;
i32 AnimPacketModelsCompatible(ANIMPACKET *packet, i32 source_id, i32 target_id);

typedef struct SPECIAL_LAYER_PC_s {
    i16 *character_id;
    const char *name;
    u32 mask;
} SPECIAL_LAYER;

typedef struct player_suit_name_s {
    u8 unknown_000[0x4];
    char *name;
} PLAYER_SUIT_NAME;

typedef struct player_packet_storage_s {
    u8 unknown_000[0x6c4];
    u8 player_packet_storage[0x788];
} PLAYER_PACKET_STORAGE;

typedef struct player_batarang_idle_s {
    u8 unknown_00[0x7d];
    u8 active;
} PLAYER_BATARANG_IDLE;

typedef struct player_idle_character_s {
    u8 unknown_00[0x116];
    u8 use_standard_idle;
} PLAYER_IDLE_CHARACTER;

/* Two-DWORD view of the default mask. */
typedef struct PLAYER_ROUTE_MASK_WORDS {
    u32 low;
    u32 high;
} PLAYER_ROUTE_MASK_WORDS;

extern u64 CharacterMaskAll;
extern i32 complexsockposition_forcesock;

typedef struct PLAYER_LAYER_FLAGS {
    u32 unknown_0 : 9;
    u32 bob : 1;
    u32 unknown_10 : 22;
} PLAYER_LAYER_FLAGS;

typedef struct PARAPHERNALIA_EXTRA {
    u8 prefix[0xc68];
    NUVEC force_position;
    u8 gap_c74[0x10c];
    f32 force_alpha;
    u8 gap_d84[8];
    f32 force_radius;
    u8 gap_d90[0x2ec];
    u8 draw_reflection;
} PARAPHERNALIA_EXTRA;

void DrawForceGlowSprite(NUVEC *, f32, i32, f32, GAMEOBJECT *);
void DrawSnakeBody(GAMEOBJECT *);

typedef struct draw_objects_flags_s {
    u8 unknown_000[0xe24];
    u32 flags_e24_bits_0_2 : 3;
    u32 locators_updated : 1;
} DRAW_OBJECTS_FLAGS;

typedef struct new_player_flags_s {
    u8 unknown_000[0xef0];
    u32 flags_ef0_bits_0_8 : 9;
    u32 geonosian_variant : 1;
    u32 flags_ef0_bits_10_20 : 11;
    u32 flags_ef0_bit_21 : 1;
} NEW_PLAYER_FLAGS;

typedef struct player_init_progress_s {
    u32 coins;
    i16 previous_state;
    u8 active;
    u8 stored_flag;
    i8 hitpoints;
    u8 unknown_09;
    u8 helmet;
    u8 unknown_0b;
    f32 powerup_time;
} PLAYER_INIT_PROGRESS;

typedef struct player_init_extra_s {
    u8 unknown_0000[0x454];
    f32 unknown_454;
    u8 unknown_0458[0xcc0 - 0x458];
    PLAYER_BATARANG_SLOT *batarang;
    u8 unknown_0cc4[0x1062 - 0xcc4];
    i16 previous_state;
} PLAYER_INIT_EXTRA;

typedef struct player_init_flags_s {
    u8 unknown_000[0xe20];
    u32 flags_e20_bits_0_15 : 16;
    u32 stored_flag : 1;
} PLAYER_INIT_FLAGS;

extern i32 movegamecamera_forcesock;

typedef union DRAW_ATTACHMENTS_PACKET {
    ANIMPACKET packet;
    u32 words[18];
} DRAW_ATTACHMENTS_PACKET;

/* Both calculated return paths end in an f32 store/reload. */
f32 DropInOutScale(GAMEOBJECT *object);

typedef struct CHARACTER_DRAW_HIERARCHY {
    u8 unknown_000[0x1a0];
    u8 suppress_shadow_surface_points;
} CHARACTER_DRAW_HIERARCHY;

extern i32 GameKeepMergeAction;
extern i32 CharacterBlobShadowsOn;
extern i32 GameDrawCharacterModelFlag;

typedef struct DRAW_PREPASS_TELEPORT {
    u8 unknown_00[0x4c];
    u8 flags;
} DRAW_PREPASS_TELEPORT;

typedef struct DRAW_PREPASS_TELEPORT_OBJECT {
    u8 unknown_000[0x798];
    DRAW_PREPASS_TELEPORT *teleport;
} DRAW_PREPASS_TELEPORT_OBJECT;

f32 PodSprint_RollMul(GAMEOBJECT *);
void ApplyExtraRotation(GAMEOBJECT *, NUMTX *);

typedef struct draw_process_flags_s {
    u8 unknown_000[0xe20];
    u32 flags_e20_bits_0_26 : 27;
    u32 origin_joint_was_ready : 1;
    u32 flags_e20_bits_28_31 : 4;
    u8 unknown_e24[0xef4 - 0xe24];
    u32 flags_ef4_bits_0_7 : 8;
    u32 previous_model_draw_result : 1;
} DRAW_PROCESS_FLAGS;

void CustomiserDrawAccessories(CUSTOMISER_LAYERS *, GAMEOBJECT *, NUMTX *);
void Grapple_DrawLine(GAMEOBJECT *);
void SuperCarry_DrawObject(GAMEOBJECT *);
void DrawTorpedos(GAMEOBJECT *);
void GizDrawBuildItPiece(GAMEOBJECT *, i32);
void TransformDrawTarget(NUVEC *, f32, f32);

typedef struct ai_creature_init_packet_s {
    u8 unknown_000[0xa0];
    void *area;
    void *locator;
} AI_CREATURE_INIT_PACKET;

static void GameDrawMtxCopy(NUMTX *destination, const NUMTX *source) {
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

static void GameDrawMtxSetYRotation(NUMTX *matrix, NUANG angle) {
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

static void GameDrawMtxRotateX(NUMTX *matrix, NUANG angle) {
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

static void GameDrawMtxRotateY(NUMTX *matrix, NUANG angle) {
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

static void GameDrawMtxRotateZ(NUMTX *matrix, NUANG angle) {
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

PLAYERCHARACTERCONFIG GCDATA_DEFAULT = {
    PCBuildModelLayerList, /* 0x0 make_layer_list */
    NULL, /* 0x4 layers */
    NULL, /* 0x8 layer_lookup */
    0.1f, /* 0xc stop_speed */
    0.0f, /* 0x10 idle_speed */
    1.0f, /* 0x14 tiptoe_speed */
    2.0f, /* 0x18 walk_speed */
    3.0f, /* 0x1c run_speed */
    0.75f, /* 0x20 backwards_factor */
    -5.0f, /* 0x24 gravity */
    0.0f, /* 0x28 unknown_028 */
    2.0f, /* 0x2c jump_speed */
    1.0f, /* 0x30 jump_duration */
    1.0f, /* 0x34 jump_height */
    2.0f, /* 0x38 second_jump_speed */
    1.0f, /* 0x3c second_jump_duration */
    1.0f, /* 0x40 second_jump_height */
    {5.0f}, /* 0x44 velocity_seek_rate */
    0.0f, /* 0x48 jump_hold_duration */
    0.3375f, /* 0x4c collision_origin_radius */
    1.0471976f, /* 0x50 unknown_050 */
    0.34906587f, /* 0x54 unknown_054 */
    10.0f, /* 0x58 unknown_058 */
    -1.8325958f, /* 0x5c unknown_05c */
    0.0f, /* 0x60 unknown_060 */
    6.0f, /* 0x64 viewdistance */
    3.0f, /* 0x68 heardistance */
    0.5f, /* 0x6c maxviewheight */
    -0.5f, /* 0x70 minviewheight */
    1.0f, /* 0x74 turn_rate */
    {1.0f}, /* 0x78 high_speed_turn_rate */
    {1.0f}, /* 0x7c vehicle_acceleration_time */
    {1.0f}, /* 0x80 vehicle_deceleration_time */
    1.0f, /* 0x84 jump_move_speed_scale */
    0.0f, /* 0x88 unknown_088 */
    99.0f, /* 0x8c unknown_08c */
    0x0u, /* 0x90 flags_090 */
    {0x0u}, /* 0x94 flags_094 */
    {0, 0, 0, 0}, /* 0x98 flags_098 */
    0x1u, /* 0x9c layer_mask_special */
    0x1u, /* 0xa0 layer_mask */
    0x1u, /* 0xa4 layer_mask_medium */
    0x1u, /* 0xa8 layer_mask_low */
    0x1u, /* 0xac layer_mask_dead */
    0x0u, /* 0xb0 ride_layers_off */
    2.75f, /* 0xb4 layer_distance_medium */
    5.5f, /* 0xb8 layer_distance_low */
    {10.0f, 5.0f, 0.0f, 0.0f}, /* 0xbc ai_update_distances */
    {4, 2, 0, 0}, /* 0xcc ai_update_intervals */
    {-1, -1, -1, -1, -1, -1}, /* 0xd0 sfx_misc */
    -1, /* 0xdc sfx_die */
    -1, /* 0xde sfx_hurt */
    -1, /* 0xe0 sfx_grunt */
    -1, /* 0xe2 sfx_engine */
    {-1}, /* 0xe4 sfx_shoot */
    -1, /* 0xe6 sfx_footstep */
    -1, /* 0xe8 sfx_chatter */
    -1, /* 0xea sfx_sabre */
    -1, /* 0xec weapon_model */
    1110, /* 0xee coin_value */
    0, /* 0xf0 shadow_locators */
    0, /* 0xf2 thrust_locators */
    1, /* 0xf4 hitpoints */
    0, /* 0xf5 shield_hitpoints */
    255, /* 0xf6 blob_shadow_alpha */
    0, /* 0xf7 unknown_0f7 */
    {-1, -1, -1, -1}, /* 0xf8 weapon_joints */
    {{-1, -1, -1, -1}}, /* 0xfc weapon_shoot_joints */
    {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}, /* 0x100 streak_joints */
    {-1, -1}, /* 0x108 hand_locators */
    {-1, -1}, /* 0x10a grapple_locators */
    {-1}, /* 0x10c rocket_locator */
    {-1}, /* 0x10d shield_locator */
    {-1}, /* 0x10e head_locator */
    {-1}, /* 0x10f collision_locator */
    -1, /* 0x110 thingy_locator */
    -1, /* 0x111 helmet_locator */
    -1, /* 0x112 throw_locator */
    -1, /* 0x113 ride_locator */
    -1, /* 0x114 poo_locator */
    -1, /* 0x115 hair_layer */
    {255}, /* 0x116 hover_fall_mode */
    {255}, /* 0x117 streak_color */
    -1, /* 0x118 head_joint */
    -1, /* 0x119 cloak_joint */
    -1, /* 0x11a cloak_joint_2 */
    -1, /* 0x11b place_locator */
    -1, /* 0x11c cape_layer */
    -1, /* 0x11d extra_character_locator */
    0, /* 0x11e flags_11e */
    0, /* 0x11f layer_count */
};

void FixUpCharacters(CHARFIXUP *fixup) {
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

void ExtraCharacterFixUpAfterConfig(void) {
    i32 i;
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_JEDI, 8, 0, 0, -1, Move_JEDI, Animate_JEDI, NULL);
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER, 0x10, 0, 0, -1, Move_DROIDGENERIC, NULL, NULL);
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_DROID | CHARACTER_MODEL_PROTOCOL, 0x30, 0, 0, -1, Move_DROIDGENERIC, Animate_PROTOCOL, NULL);
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_DROID | CHARACTER_MODEL_ASTROMECH, 0x50, 0, 0, -1, Move_DROIDGENERIC, Animate_ASTROMECH,
                               PostAnimate_ASTROMECH);
    SetMoveAndAnimateFunctions(0, 0, CHARACTER_RUNTIME_CANNON, 0x800, -1, Move_CANNON, Animate_CANNON, NULL);
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_VEHICLE, 0x2000, 0, 0, -1, Move_VEHICLE, Animate_VEHICLE, NULL);
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_BEAST, 0x40000000, 0, 0, -1, Move_BEAST, Animate_BEAST, NULL);
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
        if ((GCDataList[i].flags_094 & CHARACTER_OPTION_HOVER_WINGS) != 0) {
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
    if (dat_007f18b4 != -1) {
        CDataList[dat_007f18b4].animate_fn = Animate_DEFAULT;
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

void ResetPlayerPacket(void *opaque_packet, CHARACTERDATA *character) {
    PLAYER_PACKET_RESET *packet = opaque_packet;
    i32 i;
    (void)character;
    packet->context_state_flags &= (u8)~1;
    packet->unknown_660 = 0.0f;
    packet->delayed_turn_timer = 0.0f;
    packet->movement_lean_angle = 0;
    packet->secondary_lean_angle = 0;
    packet->tertiary_lean_angle = 0;
    Player_ResetContexts(packet);
    ((GAMEPAD_RESET *)packet->gamepad)->buttons_held = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->buttons_pressed = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->buttons_released = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->left_directions = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->previous_left_directions = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->right_directions = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->previous_right_directions = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->unknown_20 = 0;
    ((GAMEPAD_RESET *)packet->gamepad)->rumble_field_40 = 0.0f;
    ((GAMEPAD_RESET *)packet->gamepad)->rumble_amount = 0.0f;
    ((GAMEPAD_RESET *)packet->gamepad)->rumble_field_48 = 0.0f;
    ((GAMEPAD_RESET *)packet->gamepad)->rumble_time = 0.0f;
    packet->gamepad->runtime_flags &= (u16)~GAMEPAD_AI_ATTACK;
    packet->gamepad->runtime_flags &= (u16)~GAMEPAD_AI_USE_FORCE;
    packet->gamepad->runtime_flags &= (u16)~GAMEPAD_AI_FORCE_PUSH;
    packet->secondary_flags &= ~0x400u;
    packet->input_state = 0;
    packet->gamepad->operator_data = NULL;
    CharShadows_Reset((CHARACTER_SHADOW_PACKET *)packet);
    for (i = 0; i < 8; ++i) {
        packet->unknown_614[i] = 0;
    }
    packet->unknown_6b8 = 0.0f;
    packet->unknown_6d8 = 0.0f;
    ResetForceGlow(packet);
    packet->ground_height = 2000000.0f;
    packet->secondary_flags &= ~1u;
    packet->unknown_72c = 0.0f;
    packet->force_glow_index = -1;
    packet->unknown_6ec = 0.0f;
    packet->movement_angle_0 = -1;
    packet->movement_angle_1 = qrand() < 0x8000 ? 0x26 : 0x4f;
    packet->unknown_708 = 0.0f;
    for (i = 0; i < 4; ++i) {
        packet->random_state[i] = (u8)(qrand() >> 8);
    }
    packet->render_flags &= 0xfffff3feu;
    packet->unknown_70c = 0.0f;
    packet->unknown_604 = NULL;
    packet->unknown_710 = 0.0f;
    packet->surface_type = -1;
    packet->unknown_718 = 0.0f;
    packet->reset_up_direction = v010;
    packet->unknown_690 = 0.0f;
    packet->unknown_688 = 0.0f;
    packet->unknown_784 = 0.0f;
}

void SetFlicker(GAMEOBJECT *object, f32 duration) {
    object->flags_e24 &= ~0x70000;
    object->vehicle_hit_timer = duration;
}

void ResetPlayerAI(GAMEOBJECT *object) {
    object->special_move_timer = 0.0f;
    object->ai_packet.animation_override_from = -1;
    object->ai_packet.animation_override_to = -1;
    object->flags_ef0 &= ~0x200000u;
    object->ai_packet.flags &= 0xfffffd9f;
    object->ai_packet.intersection_connection = NULL;
    object->ai_packet.intersection_target_connection = NULL;
    object->ai_packet.path_target = NULL;
    object->unknown_1086 = 0;
    object->unknown_1087 = 0;
    object->field_108c = 0;
    object->unknown_1088 = 0;
    object->can_use_object = NULL;
    object->use_action_word = 0;
    object->use_action_parameter_word = 0;
    object->ai_packet.frame_state = 0;
    object->route_frame_state = 0;
    memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
    object->ai_packet.inside_path_node = -1;
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, 1);
}

void DeactivateGameObject(GAMEOBJECT *object) {
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
    object->flags_1fc_word &= ~APIOBJECT_FLAG_CHARACTER;
    if (object->ai_packet.creature_index != 0xff &&
        AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "InActive") != 0) {
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
    if (object != NULL && (FreePlay == 0 || (object->flags_1f8 & APIOBJECT_CREATURE) != 0)) {
        DeactivateGameObject(object);
    }
}

void SetGameObjectCharacterData(GAMEOBJECT *object) {
    CHARACTERDATA *data = object->character_data;
    object->origin_scale = data->model_scale;
    object->collision_radius_unscaled = data->collision_radius;
    object->character_bottom = data->bottom;
    object->character_top = data->top;
    ScaleGameObject(object);
}

// noinline: the original calls InitCreatureModelFlags out of line from InitCreature; VC8 would inline it there.
static __declspec(noinline) void InitCreatureModelFlags(GAMEOBJECT *object) {
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) != 0) {
        object->flags_1f8 |= 0x20000;
    } else {
        object->flags_1f8 &= ~0x20000u;
    }
}

/* Placed with the game object code; PlayersDropInOut calls it rather than
   inlining it, so the definition lives in its own unit. */
void PlayerSetCurrentHp(GAMEOBJECT *object, i8 hitpoints) {
    object->current_hp = hitpoints;
    if (hitpoints > object->unknown_107e) {
        object->current_hp = object->unknown_107e;
    }
}

i32 KEEPONSCREEN_SIDESONLY = 0;
SUIT_LAYERS *PlayerSuit[8] = {0};
i32 makeplayerlist_freeplay = 0;
u8 PlayerTorpedoCount[8] = {0};
NUVEC GunshipANorm = {0};

i32 MakePlayerList(i32 count) {
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
        if (player->character_context != CHARACTER_CONTEXT_WEAPON_IN && player->unknown_e33 != 2 &&
            ((player->unknown_e22 & 1) != 0 || player->unknown_e33 != 0)) {
            progress->unknown_7 = 1;
        } else {
            progress->unknown_7 = 0;
        }
        if (player->coin_packet != NULL) {
            progress->coins = ((PLAYER_COIN_PACKET_PC *)player->coin_packet)->coins;
        } else {
            progress->coins = 0;
        }
        progress->field_b = player->jump_variant_state == 1;
        if (player->dead != 0) {
            progress->hitpoints = player->unknown_107e;
            progress->unknown_9 = 4;
        } else {
            progress->hitpoints = player->current_hp;
            progress->unknown_9 = player->saved_hitpoints;
        }
        progress->field_a = player->helmet;
        progress->unknown_0004 = player->unknown_1062;
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

void fun_00414ea0(GAMEOBJECT *source, GAMEOBJECT *target) {
    target->pad = source->pad;
    target->coin_packet = source->coin_packet;
    target->gizforce_los_info = source->gizforce_los_info;
    target->batarang = source->batarang;
    target->torpedo = source->torpedo;
    target->previous_sock_position = source->previous_sock_position;
    target->unknown_107e = source->unknown_107e;
    target->tag_object_callback = source->tag_object_callback;
    target->flags_1f8 = source->flags_1f8;
}

void ClearAICreatures() {
    i32 i;
    GAMEOBJECT *object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1f8 & APIOBJECT_CREATURE) != 0) {
            FreeTorpedoPacket(&object->torpedo);
            RemoveGameObject(object, 1);
        }
    }
}

void StoreProgressAICharacter(LEVEL_PROGRESS *progress) {
    LEVEL_AI_PROGRESS *saved = (LEVEL_AI_PROGRESS *)progress;
    GAMEOBJECT *object;
    i32 index;

    if (saved == NULL) {
        return;
    }
    object = Obj;
    saved->disabled_ai_object_mask[0] = 0;
    saved->disabled_ai_object_mask[1] = 0;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1f8 & APIOBJECT_CREATURE) != 0 &&
            object->ai_packet.reset_mode == 4) {
            u64 bit = (u64)1 << index;
            saved->disabled_ai_object_mask[0] |= (u32)bit;
            saved->disabled_ai_object_mask[1] |= (u32)(bit >> 32);
        }
    }
}

void AICreatureResumeScript(GAMEOBJECT *object) {
    AISCRIPT *script = object->ai_packet.script_processor.base_script;
    if (script != NULL) {
        AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, NULL, NULL, NULL, 0, script,
                              script->base_state);
        object->ai_packet.script_processor.active_ref_count = 0;
    }
}

i32 GetDefaultIdle(GAMEOBJECT *object) {
    i32 animation;
    PLAYER_BATARANG_IDLE *batarang;
    void **actions;
    void *idle;
    void *standard;
    if (((PLAYER_IDLE_CHARACTER *)object->character_data->game_character)->use_standard_idle == 0 &&
        (object->character_data->model_flags & CHARACTER_MODEL_BLASTER) != 0) {
        animation = 0x76;
    } else {
        animation = 0x19;
    }
    batarang = (PLAYER_BATARANG_IDLE *)object->batarang;
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
    if (((object->unknown_e22 & 1) != 0 || object->unknown_e33 == 1) && idle != NULL) {
        return animation;
    }
    return 1;
}

void ResetCharacterIdle(GAMEOBJECT *object, i32 mode, i32 animation) {
    CHARACTERANIM_CONFIG *info;
    i32 minimum;
    i32 maximum;
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
        object->idle_animation_limit = (f32)((f64)AnimDuration((i16)object->id, object->idle_animation, 0.0f, 0.0f, 0) * (f64)minimum -
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
    CHARACTER_IDLE_STATE *state = (CHARACTER_IDLE_STATE *)object;
    CHARACTERDATA *character = object->character_data;
    i32 alternate = ((CHARACTER_IDLE_POLICY *)character->game_character)->use_standard_idle == 0 &&
                            (character->model_flags & CHARACTER_MODEL_BLASTER) != 0
                        ? 118
                        : 25;
    i32 candidates[233];
    i32 count = 0;
    i32 animation;
    i32 repetitions;
    i32 minimum;
    i32 maximum;
    CHARACTERANIM_CONFIG *info;
    for (animation = 0; animation < 233; ++animation) {
        if (object->character_model->action_animations[animation] == NULL) {
            continue;
        }
        info = object->character_model->animation_configs[animation];
        if ((info->flags & CHARANIM_IDLE) == 0) {
            continue;
        }
        if (default_idle == alternate) {
            if ((info->flags & CHARANIM_GUN_ON) == 0) {
                continue;
            }
        } else if ((info->flags & CHARANIM_GUN_ON) != 0) {
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
        if (repetitions > 1 && (object->character_model->animation_configs[state->idle_animation]->flags & CHARANIM_CYCLE) == 0) {
            repetitions = 1;
        }
        state->idle_animation_time = 0.0f;
        state->idle_animation_limit =
            AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * repetitions - animduration_blendouttime;
    } else {
        ResetCharacterIdle(object, 0, default_idle);
    }
}

void UpdateCharacterIdle(GAMEOBJECT *object) {
    CHARACTER_IDLE_STATE *state = (CHARACTER_IDLE_STATE *)object;
    CHARACTERDATA *character;
    i32 alternate;
    i32 default_idle;
    i16 requested;
    if (object->character_model == NULL) {
        return;
    }
    character = object->character_data;
    alternate = ((CHARACTER_IDLE_POLICY *)character->game_character)->use_standard_idle == 0 &&
                        (character->model_flags & CHARACTER_MODEL_BLASTER) != 0
                    ? 118
                    : 25;
    requested = object->anim_packet.requested_animation;
    if (requested != 1 && requested != alternate) {
        default_idle = GetDefaultIdle(object);
    } else if (requested == 1 && object->anim_packet.previous_animation == alternate) {
        default_idle = 1;
    } else if (requested == alternate && object->anim_packet.previous_animation == ACTION_IDLE) {
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

static void GunshipKeepOnScreen(GAMEOBJECT *object) {
    f32 push_speed = (f32)((f64)object->character_data->game_character->run_speed * 5.0);
    f32 count = 0.0f;
    NUVEC normal;
    NUVEC response;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[3].point, &PlayPlane[3].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x6000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0xa000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x2000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[4].point, &PlayPlane[4].normal, NULL, 0.0f, NULL)) {
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

void KeepOnScreen(GAMEOBJECT *object) {
    f32 previous_keep_time = object->unknown_da8;
    i32 controlled = 0;
    i32 index;
    i32 two_players;
    i32 constrained;
    PLAYERCHARACTERCONFIG *character;
    f32 push_distance;
    NUVEC movement;
    object->unknown_da8 = 0.0f;
    if (newgamecam != 0 || (object->flags_ef4 & 0x10000000) != 0) {
        return;
    }
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *controller = Player[index];
        if (controller != NULL && controller->character_context == CHARACTER_CONTEXT_TECHNO && controller->techno != NULL &&
            controller->techno->controlled_object == object) {
            controlled = 1;
        }
    }
    if (!(object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && !controlled) {
        return;
    }
    if (object->dead != 0 || object->character_context == CHARACTER_CONTEXT_DOOMED) {
        return;
    }
    if (!(object->flags_ef0 & 0x40000)) {
        return;
    }
    if (object->character_context == CHARACTER_CONTEXT_TELEPORT || object->character_context == CHARACTER_CONTEXT_ZIP_UP ||
        object->character_context == CHARACTER_CONTEXT_BIG_JUMP || object->character_context == CHARACTER_CONTEXT_GRAPPLE ||
        object->character_context == CHARACTER_CONTEXT_TECHNO) {
        return;
    }
    if (MiniCutCam != 0 || GetMenuID() != -1) {
        return;
    }
    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && GameCam->mode == GAMECAM_MODE_GUNSHIP) {
        GunshipKeepOnScreen(object);
        return;
    }
    if (VehicleArea == 0) {
        if (object == Player[0] && Player[1] != NULL &&
            (!(Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) || Player[1]->character_context == CHARACTER_CONTEXT_DOOMED)) {
            return;
        }
        if (object == Player[1] && Player[0] != NULL &&
            (!(Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) || Player[0]->character_context == CHARACTER_CONTEXT_DOOMED)) {
            return;
        }
    }
    two_players = Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && Player[1] != NULL &&
                  (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE);
    character = object->character_data->game_character;
    push_distance = VehicleArea != 0 ? character->walk_speed : character->run_speed;
    movement = object->velocity;
    constrained = 0;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[0].point, &PlayPlane[0].normal, NULL, 0.0f, NULL)) {
        Surface_Deflect(&PlayPlane[0].normal, &movement, &movement, 0);
        movement.x += PlayPlane[0].normal.x * push_distance;
        movement.z += PlayPlane[0].normal.z * push_distance;
        constrained = 1;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, -object->bounds_radius, NULL)) {
        Surface_Deflect(&PlayPlane[2].normal, &object->velocity, &movement, 0);
        movement.x += PlayPlane[2].normal.x * push_distance;
        movement.z += PlayPlane[2].normal.z * push_distance;
        constrained = 1;
    } else if (OnOrInsidePlane(&object->collision_position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, -object->bounds_radius,
                               NULL)) {
        Surface_Deflect(&PlayPlane[1].normal, &object->velocity, &movement, 0);
        movement.x += PlayPlane[1].normal.x * push_distance;
        movement.z += PlayPlane[1].normal.z * push_distance;
        constrained = 1;
    }
    if ((VehicleArea != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) || KEEPONSCREEN_SIDESONLY == 0 ||
        (two_players && abs(RotDiff(0, ((KEEP_SCREEN_CAMERA_FIELDS *)GameCam)->pitch)) > 0x2000)) {
        if (OnOrInsidePlane(&object->collision_position, &PlayPlane[4].point, &PlayPlane[4].normal, NULL, -object->bounds_half_height,
                            NULL)) {
            index = !(((KEEP_SCREEN_CAMERA_FIELDS *)GameCam)->direction.y <= 0.0f) ? 5 : 0;
        } else if (OnOrInsidePlane(&object->collision_position, &PlayPlane[3].point, &PlayPlane[3].normal, NULL,
                                   -object->bounds_half_height, NULL)) {
            index = !(((KEEP_SCREEN_CAMERA_FIELDS *)GameCam)->direction.y <= 0.0f) ? 0 : 5;
        } else {
            index = -1;
        }
        if (index != -1) {
            Surface_Deflect(&PlayPlane[index].normal, &object->velocity, &movement, 0);
            movement.x += PlayPlane[index].normal.x * push_distance;
            movement.z += PlayPlane[index].normal.z * push_distance;
            constrained = 1;
        }
    }
    if (constrained) {
        object->movement_direction_x = movement.x;
        object->movement_direction_z = movement.z;
        object->unknown_da8 = previous_keep_time + FRAMETIME;
        SpecialMove_Cancel(object);
    } else {
        object->unknown_da8 = 0.0f;
    }
}

void MovePlayerSpline(GAMEOBJECT *object) {
    GAMEPAD *pad = object->pad;
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
    object->unknown_282 = object->unknown_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->previous_movement_angle = object->unknown_276;
    object->flags_ef0 &= ~0x4000;

    if (object->model_draw_result != 0) {
        object->unknown_f10 = 0.0f;
    } else {
        object->unknown_f10 += FRAMETIME;
    }
    if (object->character_context == CHARACTER_CONTEXT_DOOMED) {
        object->turn_braking += FRAMETIME;
    }

    if ((f64)object->run_speed_override != MoveSplineRunSpeedSentinel) {
        speed = object->run_speed_override;
    } else {
        speed = object->character_data->game_character->run_speed;
    }
    distance = FRAMETIME * speed;
    MoveSplinePosition(&object->movement_spline_position, distance);
    PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position, (u16 *)&angle,
                     (u16 *)&pitch, object->movement_spline_position.looping);
    if (distance < 0.0f) {
        angle += 0x8000;
        pitch = (u16)-pitch;
    }
    object->movement_facing_angle = angle;
    object->unknown_276 = object->facing_angle = SeekRot(object->facing_angle, angle, spline_seek_ang);
    object->pitch_angle = SeekRot(object->pitch_angle, -pitch, spline_seek_ang);

    if (WORLD->current_level != PODSPRINTA_LDATA || speed > 0.0f) {
        object->movement_spline_lateral_speed =
            SeekValF(object->movement_spline_lateral_speed, -object->movement_spline_offset.x, spline_xoffset_speed_seek);
    }
    object->movement_spline_offset.x += object->movement_spline_lateral_speed * FRAMETIME;
    if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f || object->movement_spline_offset.z != 0.0f) {
        NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
        NuVecRotateY(&offset, &offset, object->unknown_276);
        NuVecAdd(&position, &position, &offset);
    }

    if ((object->flags_ef4 & APIOBJECT_SPLINE_FOLLOW_TERRAIN) != 0) {
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
    if (object->delayed_turn_timer > 0.0f && object->character_context != CHARACTER_CONTEXT_TURN &&
        object->character_context != CHARACTER_CONTEXT_LOOP && object->character_context != CHARACTER_CONTEXT_BARREL_ROLL) {
        object->delayed_turn_timer -= FRAMETIME;
    }
    if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
        position.y += VehicleTurnOrLoopOffset(object);
    }

    SeekVec(&object->position, &object->position, &position, spline_seek_pos);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    object->flags_107a = 0;

    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_BANKING) != 0) {
        roll = 0;
        if (object->character_context != CHARACTER_CONTEXT_TURN && object->character_context != CHARACTER_CONTEXT_LOOP &&
            object->character_context != CHARACTER_CONTEXT_BARREL_ROLL) {
            diff = RotDiff(object->previous_movement_angle, object->unknown_276);
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
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, target_roll, 8.0f);
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

void MovePlayer(GAMEOBJECT *object) {
    GAMEPAD *pad = object->pad;
    i32 analog_x;
    i32 analog_y;
    i32 menu_id;
    i32 index;
    i32 waggle;
    u32 buttons;
    f32 x;
    f32 z;
    f32 magnitude;
    f32 inverse;
    f32 run_speed;
    f32 walk_speed;
    NUVEC input;
    NUVEC axis;
    NUANG angle;

    object->flags_e20 &= ~0x200000;
    if (pad->bound_pad != NULL) {
        analog_x = pad->bound_pad->analog_left_x;
        analog_y = pad->bound_pad->analog_left_y;
    } else {
        analog_x = analog_y = 0;
    }
    if (pad->input_magnitude != pad->previous_input_magnitude) {
        pad->runtime_flags |= GAMEPAD_MAGNITUDE_CHANGED;
    }
    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->unknown_282 = object->unknown_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->flags_ef0 &= ~0x4000;
    object->previous_movement_angle = object->unknown_276;
    if (object->model_draw_result != 0) {
        object->unknown_f10 = 0.0f;
    } else {
        object->unknown_f10 += FRAMETIME;
    }
    if (object->character_context == CHARACTER_CONTEXT_DOOMED) {
        object->turn_braking += FRAMETIME;
    }
    if ((object->flags_ef4 & 0x4000000) == 0) {
        if (!object->dead) {
            pad->runtime_flags &= ~GAMEPAD_WAGGLE;
            pad->waggle_magnitude = 0.0f;
            object->unknown_106e = -1;
            object->flags_ef0 &= ~0x400;
            menu_id = GetMenuID();
            if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && (object->flags_1fc_word & 0x100) == 0) {
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
                    x = (f32)(x * (f64)0.007843138f);
                    z = (f32)(z * (f64)0.007843138f);
                    angle = NuAtan2D(x, z);
                    axis.x = axis.y = 0.0f;
                    axis.z = PlayersFabs(x) > PlayersFabs(z) ? PlayersFabs(x) : PlayersFabs(z);
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
                        if ((object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0 &&
                            ((object->player_index != -1 && object->vehicle_secondary_hp < 3) ||
                             (object->player_index == -1 && object->current_hp < 3))) {
                            pad->input_magnitude = object->character_data->game_character->walk_speed;
                        } else if (magnitude < 0.5f &&
                                   (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_NO_TIPTOE) == 0) {
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
                    ((MOVEPLAYER_PAD_BITS *)&pad->runtime_flags)->waggle = waggle;
                    pad->waggle_magnitude = object == Player[0] || object == Player[1] ? GamePad_Rotate(object) : 0.0f;
                }
            } else {
                if ((object->flags_ef0 & APIOBJECT_DONT_MOVE) != 0) {
                    pad->input_magnitude = 0.0f;
                    object->pad->buttons_pressed = object->pad->buttons_held = 0;
                } else if (object->ai_packet.movement_stopped == 1) {
                    pad->input_direction_x = pad->input_direction_z = pad->input_magnitude = 0.0f;
                    pad->input_mode = 1;
                } else if (object->character_context == CHARACTER_CONTEXT_DEACTIVATED && (object->unknown_ef8 & 1) == 0) {
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
                    run_speed = object->run_speed_override != 1000000000.0 ? object->run_speed_override
                                                                           : object->character_data->game_character->run_speed;
                    walk_speed = object->walk_speed_override != 1000000000.0 ? object->walk_speed_override
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
                    if ((object->flags_1f8 & APIOBJECT_CREATURE) == 0 && (object->portal_flags & APIOBJECT_CATCH_UP_FORBIDDEN) == 0 &&
                        (object->unknown_f10 >= OFFSCREEN_CATCHUP_TIME ||
                         (object->model_draw_result == 0 && (object->tag_flags & APIOBJECT_TAG_DISABLED) == 0 &&
                          drop_back_in_timer > 0.0f)) &&
                        (pad->input_magnitude + (f64)0.001f >= walk_speed || object->id == (u16)id_SPEEDERBIKE)) {
                        object->flags_ef0 |= 0x400;
                    }
                }
                object->pad->operator_data = object->ai_packet.movement_look_target;
            }
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && menu_id == 0x0e &&
                (object == Player[0] || object == Player[1])) {
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
            if (object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER || object->character_context == CHARACTER_CONTEXT_EATEN) {
                return;
            }
            object->movement_mode = 2;
            ShoveSystemCheckGameObject(object);
            if (object->id == id_DRAGBOMB) {
                MovePlayer_ROLLING(object);
            } else if (!MovePlayer_TWIST(object) && !MovePlayer_CIRCLE(object) && !MovePlayer_GUNSHIPIN(object) &&
                       !MovePlayer_POD(object)) {
                if ((object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
                    MovePlayer_VEHICLEDIRECTIONAL(object);
                } else {
                    MovePlayer_DIRECTIONAL(object);
                }
            }
            if (LIFTPLAYER != 0 && menu_id == -1 && !object->dead && object->character_context != CHARACTER_CONTEXT_DOOMED &&
                (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_DONT_MOVE) == 0 &&
                (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && (pad->buttons_held & GAMEPAD_LIFT) != 0 &&
                pad != ViewCamGetGamePad()) {
                if (object->character_context == CHARACTER_CONTEXT_TECHNO && object->techno != NULL && object->techno->target_mode == 1 &&
                    object->techno->controlled_player != NULL) {
                    object->techno->controlled_player->position.y += FRAMETIME * 1.5;
                    object->techno->controlled_player->velocity.y = 0.0f;
                    object->techno->controlled_player->unknown_281 = 0;
                    object->techno->controlled_player->lift_clear_state = 0;
                    if (object->techno->controlled_player->character_context == CHARACTER_CONTEXT_SLIDE) {
                        object->techno->controlled_player->character_context = CHARACTER_CONTEXT_NONE;
                    }
                } else {
                    object->position.y += FRAMETIME * 1.5;
                    object->velocity.y = 0.0f;
                    object->unknown_281 = 0;
                    object->lift_clear_state = 0;
                    if (object->character_context == CHARACTER_CONTEXT_SLIDE) {
                        object->character_context = CHARACTER_CONTEXT_NONE;
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

static void TerrainPlayerResetContact(GAMEOBJECT *object) {
    object->jump_animation_state = 0;
    object->raw_surface_id = 0;
    object->surface_normal = v010;
    object->water_height = 2000000.0f;
    object->roof_height_pc = 2000000.0f;
    object->unknown_283 = -1;
    object->roof_surface_id = (u8)-1;
    object->terrain_platform_id = -1;
}

static i32 TerrainKillPlayer(GAMEOBJECT *object, i32 surface, NUVEC *normal) {
    u32 flags;

    flags = TerSurface[surface].flags;
    if ((flags & 1) == 0 &&
        ((flags & 0x4000) == 0 || (object->character_data->model_flags & CHARACTER_MODEL_DROID) != 0 ||
         (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) != 0) &&
        ((flags & 0x8000) == 0 || (object->player_index != -1 && !(normal->y > 0.0f)))) {
        return 0;
    }
    InstantKillParts(object, 1, 0.0f);
    if (((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) && BonusWinner == -1) {
        i32 coins;

        coins = LoseCoins(object, 1);
        AddPickups(coins, 0, 0, 0, &object->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, object, 1, 0);
    }
    KillPlayer(object, 2, 1, NULL);
    return 1;
}

void TerrainPlayer(GAMEOBJECT *object) {
    NUVEC forward;
    GAMEPAD *entry_pad;
    i32 entry_underwater;
    i32 entry_intersects_water;
    i32 entry_menu_id;
    i32 entry_platform;
    TERRAIN_PLAYER_IMPACT impact_records[8];
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
        if (object->takeover_source != NULL && object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER) {
            TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
        } else {
            GAMEOBJECT *api;
            i32 terrain_mask;
            f32 entry_vertical_velocity;
            const AIPATHINFO *path_info;
            i32 ai_controlled;
            i32 special_path_endpoints;
            i32 shadow_grounding;
            i32 retain_floor;
            NUVEC incoming_velocity;
            i32 player_edge_probe;
            f32 movement_threshold;
            f32 speed_squared;

            api = object;
            terrain_mask = GetSurfaceInfoDefaultMask.word;
            if ((api->character_data->model_flags & CHARACTER_MODEL_DROID) == 0 &&
                (api->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) == 0) {
                terrain_mask |= GetSurfaceInfoRuntimeMask.word;
            }
            ai_controlled = api->flags_1f8 & APIOBJECT_CREATURE;
            if (ai_controlled && object->id != id_DRAGBOMB &&
                (WORLD->area != HOTHBATTLE_ADATA || api->character_data->game_character->unknown_028 != 0.0f)) {
                terrain_mask |= 0x40;
            }
            entry_vertical_velocity = api->velocity.y;
            object->flags_e20 &= ~8u;
            path_info = &object->ai_packet.path_info;
            special_path_endpoints = 0;
            if (path_info->path != NULL && path_info->connection != NULL) {
                const AIPATHNODE *nodes;

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
            } else if ((api->character_data->game_character->flags_090 & CHARACTER_RUNTIME_DONT_MOVE) != 0) {
                api->velocity.x = 0.0f;
                api->velocity.z = 0.0f;
                shadow_grounding = 1;
                retain_floor = 0;
            } else if ((api->flags_1fc_word & 0x40) == 0 && (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000) == 0) {
                i32 ordinary_ai_path;

                ordinary_ai_path = ai_controlled && object->movement_spline_position.spline == NULL && (path_info->flags & 1) != 0 &&
                                   (path_info->flags & 8) == 0 && object->character_context != CHARACTER_CONTEXT_FORCE_PUSHED &&
                                   object->context_target_position == NULL && !special_path_endpoints &&
                                   Technos_FindControllingTechno(object) == NULL && (api->flags_1fc_word & 0x40000) == 0 &&
                                   !AIPATHCNX_FLAG_SET(path_info->connection, original_traversal_flags, LEGO_AIPATHCNX_BLOCKAGE) &&
                                   !AIPATHCNX_FLAG_SET(path_info->connection, traversal_flags, LEGO_AIPATHCNX_WALLSHUFFLE) &&
                                   !AIPATHCNX_FLAG_SET(path_info->connection, traversal_flags, LEGO_AIPATHCNX_FULLTERRAIN) &&
                                   (object->character_context != CHARACTER_CONTEXT_STUNNED || object->unknown_7b3 != 0);
                if (ordinary_ai_path) {
                    retain_floor = (api->flags_1fc_word & 0x10) != 0;
                    shadow_grounding = 1;
                } else {
                    retain_floor = ai_controlled ? 0 : (0x10 & api->flags_1fc_word) != 0;
                    if (0 != (object->flags_ef4 & 0x40)) {
                        object->flags_ef4 &= ~0x40u;
                        shadow_grounding = 1;
                    } else {
                        shadow_grounding = (api->flags_1fc_word & APIOBJECT_FLAG_SHADOW_TERRAIN) != 0;
                    }
                }
            } else {
                shadow_grounding = 0;
                retain_floor = 0;
            }

            player_edge_probe = ((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) && object->spawn_protection_timer > 1.25f;

            if (VehicleArea == 0 && api->dead == 0 &&
                (api->unknown_281 != 0 ||
                 (player_edge_probe && object->character_context == CHARACTER_CONTEXT_NONE && object->jump_variant_state != 1))) {
                i32 walkable_floor;
                i32 probe_edge;

                walkable_floor = api->floor_height != 2000000.0f && object->surface_normal.y > NuTrigTable[0x238e];
                probe_edge =
                    walkable_floor ? (player_edge_probe || 0 != ((U64_PAIR(&CInfo[object->character_context].flags) & 0x100000000ULL))) : 0;
                if (probe_edge && api->movement_direction_x == 0.0f && api->movement_direction_z == 0.0f &&
                    (api->velocity.x != 0.0f || api->velocity.z != 0.0f)) {
                    NUVEC next_position;
                    f32 next_floor;

                    next_position.x = api->position.x + api->velocity.x * FRAMETIME;
                    next_position.y = api->position.y;
                    next_position.z = api->position.z + api->velocity.z * FRAMETIME;
                    next_floor = GameShadow(object, &next_position, 5.0f, terrain_mask | 0x1f);
                    if (next_floor != 2000000.0f && ShadNorm.y > NuTrigTable[0x238e] &&
                        (player_edge_probe ? api->collision_min.y - next_floor : api->floor_height - next_floor) > 0.2f) {
                        api->velocity.z = 0.0f;
                        api->velocity.x = 0.0f;
                    }
                }
            }
            if (!(object->pad->input_magnitude > 0.0f) && object->character_context != CHARACTER_CONTEXT_SLIDE &&
                !(api->unknown_281 != 0 && (i8)api->jump_animation_state >= 0 && (i8)api->jump_animation_state < 32 &&
                  TerSurface[(i8)api->jump_animation_state].movement_scale < 1.0f) &&
                AnimSpeed(api->character_model, CurrentAnim(&api->anim_packet)) == 0.0f &&
                (api->character_data->model_flags & CHARACTER_MODEL_NON_STICK) == 0) {
                movement_threshold = api->character_data->game_character->stop_speed * api->origin_scale;
            } else {
                movement_threshold = 0.0f;
            }

            speed_squared = (api->velocity.x * api->velocity.x + api->velocity.y * api->velocity.y) + api->velocity.z * api->velocity.z;
            if ((object->flags_ef0 & APIOBJECT_DONT_SET_STOPPED_FLAG) == 0 && (object->flags_ef4 & 0x400000) != 0 &&
                !special_path_endpoints && object->ai_packet.path_target == NULL &&
                !((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) && (object->flags_e20 & 0x10000000) == 0 &&
                object->jump_variant_state == 0 && object->character_context != CHARACTER_CONTEXT_JUMP &&
                object->character_context != CHARACTER_CONTEXT_FORCE_PUSHED && api->jump_animation_state != 8 &&
                (GameTimer.update_count >= 2 || (api->flags_1f8 & (APIOBJECT_SIDE_BADDIE | APIOBJECT_SIDE_NEUTRAL)) != 0) &&
                (api->unknown_281 & 3) != 0 && api->supporting_platform_id == -1 &&
                movement_threshold * movement_threshold >= speed_squared && object->character_context != CHARACTER_CONTEXT_TELEPORT &&
                object->character_context != CHARACTER_CONTEXT_INTERFACE && object->character_context != CHARACTER_CONTEXT_FORCE_FROZEN) {
                api->flags_1fc_word |= 4;
            } else {
                api->flags_1fc_word &= ~4u;
            }
            incoming_velocity = api->velocity;
            object->contact_count = 0;
            if ((api->flags_1fc_word & 0x20) != 0 || (object->flags_e20 & 0x20) != 0 || object->movement_spline_position.spline != NULL ||
                object->move_override != NULL ||
                (object->character_context == CHARACTER_CONTEXT_TELEPORT && (object->unknown_7b3 == 1 || object->unknown_7b3 == 0)) ||
                (object->character_context == CHARACTER_CONTEXT_LAUNCH && object->unknown_7b3 == 0) ||
                (object->character_context == CHARACTER_CONTEXT_ZIP_UP && object->action_movement_state == 1)) {
                api->respawn_timer = 0.0f;
                object->flags_e20 |= 2;
                api->unknown_281 = 0;
                api->supporting_platform_id = -1;
                object->contact_material_word = 0;
                api->position.x += api->velocity.x * FRAMETIME;
                api->position.y += api->velocity.y * FRAMETIME;
                api->position.z += api->velocity.z * FRAMETIME;
            } else if (shadow_grounding || retain_floor || (WORLD->current_level->flags & LEVEL_FLAT_TERRAIN) != 0) {
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
                        if (TerrainPlayerShadowFix != 0) {
                            NUVEC shadow_position;

                            shadow_position = api->collision_position;
                            shadow_position.y =
                                api->initial_position.y > api->collision_position.y ? api->initial_position.y : api->collision_position.y;
                            api->floor_height = GameShadow(object, &shadow_position, 5.0f, terrain_mask | 0x1f);
                        } else {
                            api->floor_height = GameShadow(object, &api->collision_position, 5.0f, terrain_mask | 0x1f);
                        }
                        api->supporting_platform_id = ((i16)(NewShadowOnPlatform()));
                        if (api->floor_height != 2000000.0f) {
                            GetSurfaceInfo(object, 1, api->floor_height);
                        } else {
                            GetSurfaceInfo(object, 0, api->floor_height);
                            TerrainPlayerResetContact(object);
                        }
                        api->is_underwater = ((u8)(UnderWater(object)));
                        api->water_intersects = ((u8)(IntersectWater(object)));
                        if (api->supporting_platform_id == -1 && api->unknown_281 != 0 && object->terrain_platform_id != -1) {
                            api->supporting_platform_id = object->terrain_platform_id;
                        }
                    }
                    grounded_lower_bound = object->character_bottom * api->origin_scale;
                    if (api->floor_height != 2000000.0f && api->position.y + grounded_lower_bound < api->floor_height) {
                        api->velocity.y = 0.0f;
                        api->position.y = api->floor_height - grounded_lower_bound;
                    }

                    api->unknown_281 = 0;
                    if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                        api->unknown_281 |= 2;
                    }
                    if (api->collision_min.y <= api->floor_height) {
                        api->unknown_281 |= 1;
                    }
                    if ((api->unknown_281 & 1) != 0) {
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
                    TERRAIN_PLAYER_PLATFORM_SYS *platforms;
                    i32 i;

                    collision_position.x = api->position.x;
                    collision_position.y = api->origin_scale * object->character_bottom + api->position.y;
                    collision_position.z = api->position.z;
                    object_index = (((u32)(object)) - ((u32)(Obj))) / sizeof(GAMEOBJECT);

                    NuVecScale(&movement, &api->velocity, FRAMETIME);

                    object->contact_material_word = 0;

                    if (object->use_model_origin > 0 && api->unknown_281 != 0 && (movement.x != 0.0f || movement.z != 0.0f) &&
                        !(WORLD->current_level == ENDORBATTLEC_LDATA && object->id == GameObjectRotationAtstId)) {
                        i32 deflection_mode;

                        deflection_mode = 0;
                        if (VehicleArea != 0 && (api->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0 &&
                            api->character_data->game_character->unknown_028 != 0.0f) {
                            deflection_mode = 2;
                        } else if (object->character_context == CHARACTER_CONTEXT_NONE && object->pad->input_magnitude > 0.0f) {
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
                        (api->character_data->model_flags & CHARACTER_MODEL_NON_STICK) != 0 ? 0.0f : TerrainPlayerPlatConnectTolerance);
                    if ((object->ai_packet.flags & AIPACKET_IGNORE_WALL_SPLINES) != 0) {
                        IgnoreWallSplines = 1;
                    }
                    TerrainSetImpactData(impact_records, &impact_count, 8);
                    if ((object->flags_ef0 & 0x400) != 0) {
                        if (object->id == (u16)id_SPEEDERBIKE) {
                            if (object->ai_packet.goal_speed_mode == 1) {
                                movement_scale = TerrainPlayerBikeMovementScale / api->character_data->game_character->walk_speed;
                            } else if (object->ai_packet.goal_speed_mode == 2) {
                                movement_scale = TerrainPlayerBikeMovementScale / api->character_data->game_character->tiptoe_speed;
                            } else {
                                movement_scale = TerrainPlayerBikeMovementScale / api->character_data->game_character->run_speed;
                            }
                        } else {
                            movement_scale = TerrainPlayerOtherMovementScale / api->character_data->game_character->run_speed;
                        }
                        movement.x *= movement_scale;
                        movement.z *= movement_scale;
                    }
                    excluded_count = 0;
                    if (object->unknown_106e != -1) {
                        excluded_platforms[excluded_count++] = object->unknown_106e;
                    }
                    if (object->level_object_index != -1) {
                        excluded_platforms[excluded_count++] = object->level_object_index;
                    }
                    if (object->character_context == CHARACTER_CONTEXT_JUMP) {
                        GAMEOBJECT *target;

                        target = object->airborne_collision_target;
                        if (target != NULL && target->level_object_index != -1) {
                            excluded_platforms[excluded_count++] = target->level_object_index;
                        }
                    } else if (object->character_context == CHARACTER_CONTEXT_SUPER_CARRY && object->unknown_7b3 == 1) {
                        TERRAIN_PLAYER_BLOWUP *blowup;

                        blowup = object->grapple;
                        if (blowup->platform_id != -1) {
                            excluded_platforms[excluded_count++] = blowup->platform_id;
                        }
                    }
                    platforms = ((TERRAIN_PLAYER_PLATFORM_SYS *)(WORLD->shadow_platform_list_pc));
                    if (platforms != NULL && VehicleArea == 0) {
                        i32 player;

                        player = ((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0);
                        if (!player || object->character_context == CHARACTER_CONTEXT_FLATTEN || object->takeover_source != NULL ||
                            object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER ||
                            (api->character_data->game_character->flags_090 &
                             (CHARACTER_RUNTIME_CAN_TAKE_OVER | CHARACTER_RUNTIME_GHOST)) != 0) {
                            i32 i;

                            for (i = 0; platforms->count > i; ++i) {
                                GAMEOBJECT *platform_object;

                                platform_object = platforms->entries[i].object;
                                if (platform_object != NULL && ((object->flags_ef4 & APIOBJECT_CAN_COLLIDE_WITH_OBJECTS) == 0 || player)) {
                                    excluded_platforms[excluded_count++] = platform_object->level_object_index;
                                }
                            }
                        }
                    }
                    for (i = 0; i < excluded_count; ++i) {
                        PlatOnOff(excluded_platforms[i], 0);
                    }

                    NewTerrainScaleYMask(&collision_position, &movement, ((u8 *)(&object->unknown_1050)), object_index,
                                         entry_pad->input_magnitude > 0.0f ? 0.0f : movement_threshold * FRAMETIME, api->collision_radius,
                                         object->collision_y_scale, 0, 0, terrain_mask);
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
                        memcpy(&object->contact_position, &impact_records[impact_count - 1], sizeof(TERRAIN_PLAYER_IMPACT));
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
                            NUVEC previous_forward;
                            NUVEC current_forward;
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
                                api->unknown_276 = NuAngAdd(rotation, api->unknown_276);
                                api->facing_angle = NuAngAdd(rotation, api->facing_angle);
                                api->movement_facing_angle = NuAngAdd(rotation, api->movement_facing_angle);
                            }
                        }
                    }
                    if (api->dead == 0) {
                        i32 wall_impact;

                        wall_impact = 0;
                        if (object->contact_count != 0 && 0 == (api->flags_1fc_word & 0x84) &&
                            object->character_context != CHARACTER_CONTEXT_JUMP && object->character_context != CHARACTER_CONTEXT_CLIMB &&
                            object->character_context != CHARACTER_CONTEXT_WALL_SHUFFLE && (object->flags_ef4 & 0x10000) == 0) {
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
                floor_height =
                    (api->flags_1fc_word & 4) == 0 ? GameShadow(object, &api->position, 5.0f, terrain_mask | 0x1f) : api->floor_height;
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
                    if (api->supporting_platform_id == -1 && api->unknown_281 != 0 && object->terrain_platform_id != -1) {
                        api->supporting_platform_id = object->terrain_platform_id;
                    }
                    if ((CHARACTER_RUNTIME_GHOST & api->character_data->game_character->flags_090) == 0 &&
                        object->reflection_height == 2000000.0f && object->terrain_platform_id != -1) {
                        f32 reflection_height;

                        reflection_height = FindReflectionNoPlatforms(&api->position);
                        if (reflection_height != 2000000.0f) {
                            object->reflection_height = reflection_height;
                            object->reflection_enabled = 2;
                        }
                    }
                }

                api->unknown_281 = object->unknown_1050 != 0 ? 1 : 0;
                if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                    api->unknown_281 |= 2;
                }
                if (api->unknown_281 != 0) {
                    object->flags_ef0 |= 0x2000000;
                }
            }

            if (api->unknown_281 != 0 && entry_platform == -1 && api->supporting_platform_id != -1 &&
                api->supporting_platform_id == object->terrain_platform_id && entry_vertical_velocity < 0.0f) {
                TERRAIN_PLAYER_GRABBER *grabber;

                grabber = ((TERRAIN_PLAYER_WORLD *)(WORLD))->grabber;
                if (grabber != NULL && grabber->platform_id == api->supporting_platform_id) {
                    if (grabber->platform_contact_timer <= 0.0f) {
                        grabber->platform_contact_timer = 0.25f;
                    }
                } else {
                    GAMEOBJECT *platform_owner;

                    platform_owner = CharPlatform_FindObjFromPlatID(((TERRAIN_PLAYER_PLATFORM_SYS *)(WORLD->shadow_platform_list_pc)),
                                                                    api->supporting_platform_id);
                    if (platform_owner != NULL && platform_owner->character_data->game_character->unknown_028 > 0.0f) {
                        platform_owner->velocity.y = entry_vertical_velocity * 0.5f + platform_owner->velocity.y;
                    }
                }
            }
            if (api->unknown_281 == 0 && entry_platform != -1 && api->supporting_platform_id == -1 &&
                object->terrain_platform_id == entry_platform) {
                TERRAIN_PLAYER_GRABBER *grabber;

                grabber = ((TERRAIN_PLAYER_WORLD *)(WORLD))->grabber;
                if (grabber != NULL && grabber->platform_id == entry_platform) {
                    if (grabber->platform_contact_timer <= 0.0f) {
                        grabber->platform_contact_timer = 0.25f;
                    }
                } else {
                    GAMEOBJECT *platform_owner;

                    platform_owner =
                        CharPlatform_FindObjFromPlatID(((TERRAIN_PLAYER_PLATFORM_SYS *)(WORLD->shadow_platform_list_pc)), entry_platform);
                    if (platform_owner != NULL && platform_owner->character_data->game_character->unknown_028 > 0.0f) {
                        platform_owner->velocity.y -= entry_vertical_velocity * 0.5f;
                    }
                }
            }

            if (VehicleArea == 0 && ((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) && api->supporting_platform_id != -1) {
                GAMEOBJECT *platform_owner;

                platform_owner = CharPlatform_FindObjFromPlatID(((TERRAIN_PLAYER_PLATFORM_SYS *)(WORLD->shadow_platform_list_pc)),
                                                                api->supporting_platform_id);
                if (platform_owner != NULL) {
                    const PLAYERCHARACTERCONFIG *data;

                    data = platform_owner->character_data->game_character;
                    if ((data->flags_090 & CHARACTER_RUNTIME_CAN_FLATTEN) != 0 && api->position.y < platform_owner->position.y &&
                        platform_owner->horizontal_velocity_magnitude > 0.5f * data->run_speed) {
                        StartFlatten(platform_owner, object);
                    }
                }
            }

            if (api->dead != 0) {
                if (api->unknown_281 != 0) {
                    api->velocity.y = 0.0f;
                    api->flags_1fc_word |= 4;
                }
                if ((api->flags_1fc_word & 4) == 0) {
                    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
                }
            } else {
                u32 held_buttons;
                u32 pressed_buttons;

                if (object->character_context == CHARACTER_CONTEXT_DOOMED) {
                    if (api->unknown_281 != 0 || api->model_draw_result == 0 || object->turn_braking >= 5.0f ||
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
                    TERRAIN_PLAYER_IMPACT *impacts;
                    i32 count;

                    if (impact_count == -1) {
                        count = 1;
                        impacts = (TERRAIN_PLAYER_IMPACT *)&object->contact_position;
                    } else {
                        impacts = impact_records;
                        count = impact_count;
                    }
                    if (CannotKill(object) == 0 && api->dead == 0 &&
                        (VehicleArea == 0 || ((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0)) &&
                        (object->flags_ef0 & 0x40000) != 0 && (object->portal_flags & APIOBJECT_IMMUNE_TO_KILL_TERRAIN) == 0 &&
                        object->character_context != CHARACTER_CONTEXT_BALLOONING) {
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
                                    (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA && GameTimer.time_elapsed >= 1.0f)) {
                                    NewRumble(object->pad->bound_pad, (((f32)(qrand())) * (1.0f / 65535.0f)) * 0.5f, 0);
                                    if (api->model_draw_result == 0 && object->character_context != CHARACTER_CONTEXT_DROP_IN) {
                                        if (((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0)) {
                                            LoseCoins(object, 2);
                                        }
                                        KillPlayer(object, 2, 1, NULL);
                                        break;
                                    }
                                } else if (((api->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0 ||
                                            (api->character_data->game_character->flags_090 & CHARACTER_RUNTIME_HAZARD_PROTECTION) != 0) &&
                                           api->unknown_283 == 6) {
                                    break;
                                }
                                if ((WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA) &&
                                    TerrainKillPlayer(object, surface, normal) != 0) {
                                    if (WORLD->current_level == ASTEROIDCHASEC_LDATA) {
                                        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
                                    }
                                    break;
                                }
                                if ((TerSurface[surface].flags & 0x80) != 0 && WORLD->current_level != BONUS_GUNSHIPA_LDATA) {
                                    NewRumble(object->pad->bound_pad, 0.6f, 0);
                                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                    if (object->origin_scale_override != 1.0f && CannotKill(object) == 0) {
                                        ObjHitObj(NULL, object, 1, 0, 0, 1);
                                    }
                                    break;
                                }
                            }
                        } else if (api->unknown_281 != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) {
                            i32 surface;

                            surface = ((i8)(api->jump_animation_state));
                            if (((u32)(surface)) <= 31) {
                                TerrainKillPlayer(object, surface, &object->surface_normal);
                            }
                        }
                    }
                    if (CannotKill(object) == 0 && (object->portal_flags & APIOBJECT_IMMUNE_TO_KILL_TERRAIN) == 0 && api->dead == 0 &&
                        api->water_height != 2000000.0f) {
                        i32 layer;

                        layer = api->unknown_283;
                        if ((layer == 3 ||
                             (layer == 9 && object->id == (u16)id_TRACTOR && WORLD->area != NULL && WORLD->area == DAGOBAH_ADATA) ||
                             (TerLayer[layer].flags & 0x20) != 0 ||
                             ((api->character_data->model_flags & CHARACTER_MODEL_VEHICLE) == 0 &&
                              (api->character_data->game_character->flags_090 & CHARACTER_RUNTIME_HAZARD_PROTECTION) == 0)) &&
                            NoLayerKill(object) == 0 && object->character_context != CHARACTER_CONTEXT_BALLOONING) {
                            u32 layer_flags;

                            layer_flags = TerLayer[api->unknown_283].flags;
                            if ((((layer_flags & 1) != 0 || (api->unknown_283 == 9 && object->id == (u16)id_TRACTOR &&
                                                             WORLD->area != NULL && WORLD->area == DAGOBAH_ADATA)) &&
                                 api->position.y < api->water_height &&
                                 !(VehicleArea != 0 && BonusArea != 0 && api->character_data->game_character->unknown_028 > 0.0f) &&
                                 (((api->flags_1fc_word >> 7) & 1) != 0 || (object->character_context != CHARACTER_CONTEXT_ZIP_UP &&
                                                                            object->character_context != CHARACTER_CONTEXT_GRAPPLE))) ||
                                ((layer_flags & 0x20) != 0 && api->collision_max.y > api->water_height &&
                                 (((api->flags_1fc_word >> 7) & 1) != 0 || (object->character_context != CHARACTER_CONTEXT_ZIP_UP &&
                                                                            object->character_context != CHARACTER_CONTEXT_GRAPPLE)))) {
                                if (object->doomed_escape_locator != NULL && ((api->flags_1fc_word >> 7) & 1) == 0) {
                                    if ((object->flags_ef0 & APIOBJECT_DOOMED_ESCAPE_TAKE_DAMAGE) != 0) {
                                        ObjHitObj(NULL, object, 1, 0, 0, 1);
                                    }
                                    if (api->dead == 0) {
                                        StartBigJump(object, &object->doomed_escape_locator->position, 0, 1.0f, 1.0f, 0);
                                    }
                                } else {
                                    i32 saved_rescue_surface;

                                    saved_rescue_surface = 0;
                                    if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0 && api->unknown_283 == 6 &&
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
                                        object->unknown_7aa = ACTION_FALL;
                                        if (object->character_context != CHARACTER_CONTEXT_NONE &&
                                            api->character_model->model_data_b[api->anim_packet.requested_animation] != NULL) {
                                            object->unknown_7aa = api->anim_packet.requested_animation;
                                        }
                                        object->character_context = CHARACTER_CONTEXT_DOOMED;
                                        object->jump_variant_state = 0;
                                        if ((TerLayer[api->unknown_283].flags & 0x20) != 0) {
                                            object->wall_run_state = 3;
                                        } else if (saved_rescue_surface || api->unknown_283 == 6) {
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
                                        if (((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) &&
                                            ((TERRAIN_PLAYER_COINS *)(object->coin_packet)) != NULL &&
                                            ((TERRAIN_PLAYER_COINS *)(object->coin_packet))->coins > 0 && BonusWinner == -1) {
                                            i32 coins;

                                            coins = LoseCoins(object, 2);
                                            if (coins > 0) {
                                                AddPickups(coins, 0, 0, 0, &api->collision_position, NULL, 2.0f, ((i8)(api->unknown_280)),
                                                           1.0f,
                                                           api->water_height != 2000000.0f && (TerLayer[api->unknown_283].flags & 1) != 0
                                                               ? api->water_height
                                                               : 2000000.0f,
                                                           NULL, 1, 0);
                                            }
                                            ((TERRAIN_PLAYER_COINS *)(object->coin_packet))->scale = 1.5f;
                                        }
                                    }
                                rescue_done:;
                                }
                            } else if ((api->character_data->model_flags & CHARACTER_MODEL_ASTROMECH) == 0 && object->id != (u16)id_SNAKE &&
                                       api->unknown_283 == 9 && api->collision_max.y < api->water_height) {
                                if (BonusWinner == -1) {
                                    i32 coins;

                                    coins = LoseCoins(object, 2);
                                    if (coins > 0) {
                                        AddPickups(coins, 0, 0, 0, &api->upper_position, NULL, 2.0f, ((i8)(api->unknown_280)), 1.0f,
                                                   2000000.0f, NULL, 1, 0);
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

                    splash = object->character_context == CHARACTER_CONTEXT_BIG_JUMP ||
                             object->character_context == CHARACTER_CONTEXT_JUMP ||
                             (U64_PAIR(&CInfo[object->character_context].flags) & 4) != 0 ||
                             object->character_context == CHARACTER_CONTEXT_LAND_SLAM ||
                             object->character_context == CHARACTER_CONTEXT_LAND_LUNGE;
                    if (!splash) {
                        i32 animation;

                        animation = CurrentAnim(&api->anim_packet);
                        splash = animation == 0x28 || animation == 5 || ((u32)(animation - 0x4b)) <= 1;
                    }
                    if (splash) {
                        AddWaterSplash(object, &api->collision_position);
                    }
                }
                if (api->model_draw_result != 0 && object->character_context != CHARACTER_CONTEXT_DISORIENTATE &&
                    object->character_context != CHARACTER_CONTEXT_LAUNCH) {
                    extern void GameObjectRotation(GAMEOBJECT *, i32);
                    GameObjectRotation(object, 2);
                }

                held_buttons = object->pad->buttons_held;
                pressed_buttons = object->pad->buttons_pressed;
                if ((api->flags_1fc_word & 0x180) == APIOBJECT_FLAG_PLAYER_ACTIVE && (entry_menu_id != -1 || MiniCutCam == 2)) {
                    object->pad->buttons_held = 0;
                    object->pad->buttons_pressed = 0;
                }
                Tag_Check(object);
                PreResetCode(object);
                ((TERRAIN_PLAYER_CHARACTER *)(api->character_data))->move_callback(object);
                PostResetCode(object);
                if ((api->character_data->model_flags & CHARACTER_MODEL_CANNOT_BIG_JUMP) == 0) {
                    BigJumpCode(object);
                }
                object->pad->buttons_held = held_buttons;
                object->pad->buttons_pressed = pressed_buttons;
            }

            if ((object->portal_flags & APIOBJECT_IMMUNE_TO_KILL_TERRAIN) == 0 && ((TERRAIN_PLAYER_SET *)(CurrentTerrain)) != NULL &&
                api->dead == 0 && (WORLD->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0 &&
                (((((TERRAIN_PLAYER_SET *)(CurrentTerrain))->minimum_height))) - 5.0f > api->position.y) {
                if (api->model_draw_result != 0) {
                    InstantKillParts(object, 0, 0.0f);
                }
                KillPlayer(object, 3, 1, NULL);
            }
        }
    }

    entry_pad->runtime_flags &= ~(GAMEPAD_AI_ATTACK | GAMEPAD_AI_USE_FORCE | GAMEPAD_AI_FORCE_PUSH);
    object->unknown_d20 = 0;
    entry_pad->operator_data = NULL;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, object->unknown_276);
}

void AnimatePlayer(GAMEOBJECT *object) {
    GAMEPAD *pad = object->pad;
    CHARACTER_MODEL_LAYERS *model = object->character_model;
    i16 override_from;
    f32 direction;
    f32 movement_speed;
    f32 multiplier;
    f32 *time;
    object->anim_packet.previous_animation = object->anim_packet.animation_index;
    object->mini_previous_animation = object->mini_current_animation;
    if ((object->flags_1f8 & 0x40000) == 0) {
        object->character_data->animate_fn(object);
    }
    override_from = object->ai_packet.animation_override_from;
    if (override_from != -1 && ((override_from == 0xe9 && object->character_context != CHARACTER_CONTEXT_FORCE_PUSHED) ||
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
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAR_WHEELS) != 0) {
            movement_speed = object->velocity.x * object->facing_direction.x + object->velocity.z * object->facing_direction.z;
            if (movement_speed < 0.0f) {
                movement_speed = 0.0f;
            }
        } else {
            movement_speed = pad->input_magnitude;
        }
    } else {
        movement_speed = pad->animation_input_magnitude;
        if (movement_speed > 0.0f && object->id == id_GONKDROID && Cheat_IsOn(8) == 0) {
            movement_speed = object->character_data->game_character->walk_speed;
        }
    }
    multiplier = (object->character_context == CHARACTER_CONTEXT_BUILD_IT && object->grapple != NULL) ? GizBuildItMul(object) : 1.0f;
    UpdateAnimPacket(model, &object->anim_packet, (f32)((f64)FRAMETIME * 30.0 * (f64)multiplier), movement_speed * direction,
                     multiplier * FRAMETIME, object->character_data->game_character->backwards_factor);
    if ((object->anim_packet.flags & 4) != 0) {
        time = NULL;
        if (object->anim_packet.blending != 0) {
            if (object->anim_packet.blend_target_animation == ACTION_BUILD) {
                time = &object->anim_packet.blend_target_time;
            }
        } else if (object->anim_packet.animation_index == ACTION_BUILD) {
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

static void DrawWeapon_SetSabreObjects(GAMEOBJECT *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models, i32 *hilt) {
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

AREA_DATA *JEDI_ADATA = 0;
AREA_DATA *DOOKU_ADATA = 0;

i32 AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == (u16)id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

static f32 ParaphernaliaWeaponScale(GAMEOBJECT *object) {
    f32 *frame;
    if (object->character_data->game_character->hover_fall_mode == 9) {
        frame = AnimPlaying(&object->anim_packet, ACTION_HELMETON, 1, 0);
        if (frame != NULL && *frame >= 18.0f && *frame <= 65.0f) {
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

static void ParaphernaliaShield(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG *config = object->character_data->game_character;
    i32 locator = config->shield_locator;
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
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
    if (reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, object->timer_d34 > 0.0f ? 0x93 : 0x92, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaThrust(GAMEOBJECT *object, i32 reflection) {
    i32 locator;
    i32 effect_index;
    if (WORLD->level_objects[0x77].active == 0 || WORLD->level_objects[0x78].active == 0) {
        return;
    }
    effect_index = 0;
    for (locator = 0; locator < 16; ++locator) {
        f32 ratio;
        NUVEC scaling;
        NUMTX matrix;
        NUMTX reflected;
        i32 first;
        i32 second;
        if ((object->character_data->game_character->thrust_locators & (1u << locator)) == 0 ||
            object->character_model->points_of_interest[locator] == NULL) {
            continue;
        }
        ratio = (f32)((f64)object->effect_noise[effect_index++] / 255.0);
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
            /* Draws the original matrix in this reflection pass. */
            Draw3DObjectMtx(NULL, first, &matrix);
            Draw3DObjectMtx(NULL, second, &matrix);
            NuRndrEndReflectionRender();
        }
    }
}

void ParaphernaliaCommunicate(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG *config = object->character_data->game_character;
    i32 locator = config->thingy_locator;
    i32 model;
    i32 reflection_model;
    NUMTX matrix;
    NUMTX reflected;
    NUVEC scaling;
    NUVEC blend_scaling;
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    model = -1;
    if ((object->character_data->model_flags & CHARACTER_MODEL_ASTROMECH) != 0) {
        scaling.z = object->communicate_blend * object->origin_scale;
        scaling.x = scaling.y = scaling.z;
        NuMtxSetScale(&matrix, &scaling);
        GameDrawMtxRotateY(&matrix, object->unknown_276 + 0x8000);
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        if (object->unknown_283 == 9 && matrix.m31 < object->water_height) {
            matrix.m31 = object->water_height;
        }
        model = 0x19;
    } else {
        if ((config->flags_094 & CHARACTER_OPTION_CAN_COMMUNICATE) != 0) {
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
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&WORLD->level_objects[reflection_model].special, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaPlace(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 marker;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    frame = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
    if (frame == NULL) {
        return;
    }
    end = NuAnimEndFrame(object->character_model->model_data_b[object->unknown_7aa]);
    start = AnimListFrame(object->character_model, object->unknown_7aa, 0);
    if (start >= end) {
        return;
    }
    if (start < 1.0f) {
        start = 1.0f;
    }
    if (*frame < start) {
        return;
    }
    marker = AnimListFrame(object->character_model, object->unknown_7aa, 1);
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
    if (reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
        }
    }
}

static void ParaphernaliaRemove(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 fade_start;
    f32 fade_end;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    frame = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
    if (frame == NULL) {
        return;
    }
    end = NuAnimEndFrame(object->character_model->model_data_b[object->unknown_7aa]);
    start = AnimListFrame(object->character_model, object->unknown_7aa, 0);
    if (start >= end) {
        return;
    }
    if (start < 1.0f) {
        start = 1.0f;
    }
    if (*frame < start) {
        return;
    }
    fade_start = AnimListFrame(object->character_model, object->unknown_7aa, 1);
    if (fade_start <= start) {
        return;
    }
    fade_end = AnimListFrame(object->character_model, object->unknown_7aa, 2);
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
    if (reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0) {
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
        }
    }
}

static void ParaphernaliaHand(GAMEOBJECT *object, i32 reflection) {
    NUMTX matrix;
    NUMTX reflected;
    i32 locator;
    if (object->character_context != CHARACTER_CONTEXT_BALLOONING || WORLD->level_objects[0xf7].active == 0) {
        return;
    }
    locator = object->character_data->game_character->hand_locators[0];
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    NuMtxSetTranslation(&matrix, (NUVEC *)&object->locator_matrices[locator].m30);
    Draw3DObjectMtx(NULL, 0xf7, &matrix);
    if (reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, 0xf7, &reflected);
        NuRndrEndReflectionRender();
    }
}

void ConfigureComplexShadow(GAMEOBJECT *object) {
    NUVEC direction;
    f32 distance;
    if (object != NULL) {
        direction.x = 65536.0f;
        direction.y = object->collision_position.y;
        direction.z = (f32)((f64)object->floor_height - (f64)0.1f);
    } else {
        direction.x = 0.0f;
        direction.y = -ComplexShadowBase * 0.25;
        direction.z = 0.0f;
    }
    distance = VehicleArea != 0 ? 150.0f : 30.0f;
    NuRndrShadowDirCol(&direction, 0x17000000u, distance, distance);
}

void LoadSingleCharacter(BGPROCINFO *info) {
    APICHARACTERMODELLIST list[2] = {
        {(i16)waiting_for_character, 1},
        {-1,                         0}
    };
    NUGSCN *scene;
    i32 i;
    append = 1;
    nopakfile = CHARPAK == 0;
    APILoadCharacterModels(list, 0, &characterbuffer_ptr, characterbuffer_end, CharacterLoadAreaModels);
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

void UpdateCharacterLoad(void) {
    i32 candidates[340];
    i32 candidate;
    i32 menu = GetMenuID();
    i32 required;
    i32 count;
    i32 id;
    if (menu == 13 || menu == 15 || menu == 16 || menu == 17 || LOADEROFF != 0 || BGLOAD == 0) {
        return;
    }
    if (waiting_for_character != -1 || hub_character_ready != -1 || bgGetProcActive() != NULL || (i32)nudlist_field_9d8 > 0xfd) {
        return;
    }
    required = (0x3ff - (i32)(((f64)CHARACTERBUFFERSIZE / 7077888.0) * -1048576.0)) & ~0x3ff;
    if ((i32)(characterbuffer_end.addr - characterbuffer_ptr.addr) < required) {
        return;
    }
    count = 0;
    if (dat_007f17d4 != -1 && APICharacterLoaded(dat_007f17d4) != NULL && PunchRangeCompanionId != -1 &&
        APICharacterLoaded(PunchRangeCompanionId) == NULL) {
        candidates[count++] = PunchRangeCompanionId;
    } else if (id_BARMAN != -1 && APICharacterLoaded(id_BARMAN) == NULL) {
        candidates[count++] = id_BARMAN;
    } else if (id_CANTINABAND != -1 && APICharacterLoaded(id_CANTINABAND) == NULL) {
        candidates[count++] = id_CANTINABAND;
    } else if (PermanentWeirdo1Id != -1 && APICharacterLoaded(PermanentWeirdo1Id) == NULL) {
        candidates[count++] = PermanentWeirdo1Id;
    } else if (PermanentWeirdo2Id != -1 && APICharacterLoaded(PermanentWeirdo2Id) == NULL) {
        candidates[count++] = PermanentWeirdo2Id;
    } else if (DefaultAnimationJabbaId != -1 && APICharacterLoaded(DefaultAnimationJabbaId) == NULL) {
        candidates[count++] = DefaultAnimationJabbaId;
    } else if (id_MOSEISLEYCITIZEN != -1 && APICharacterLoaded(id_MOSEISLEYCITIZEN) == NULL) {
        candidates[count++] = id_MOSEISLEYCITIZEN;
    } else if (id_CANTINAALIEN != -1 && APICharacterLoaded(id_CANTINAALIEN) == NULL) {
        candidates[count++] = id_CANTINAALIEN;
    } else if (dat_007f18c4 != -1 && APICharacterLoaded(dat_007f18c4) == NULL) {
        candidates[count++] = dat_007f18c4;
    } else {
        for (id = 0; id < CHARCOUNT; ++id) {
            if (id == id_DROIDEKA || (CDataList[id].model_flags & (CHARACTER_MODEL_VEHICLE | CHARACTER_MODEL_MINIKIT)) != 0 ||
                APICharacterLoaded(id) != NULL) {
                continue;
            }
            if (CDataList[id].animation_count + apicharsys->loaded_animation_count > apicharsys->animation_capacity) {
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

i32 ActivePlayerInRange(NUVEC *position, f32 range_squared, f32 *distance_squared) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = Player[index];
        if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
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

i32 AvailableToPlayer(u32 character_flags, i32 weapon_action, i32 helmet, i32 require_all) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = Player[index];
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
                (GCDataList[index].flags_090 & CHARACTER_RUNTIME_EXTRA_TOGGLE) == 0 || Cheats_CheckFlags(0x100) == 0) {
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

GAMEOBJECT *GetOtherActivePlayer(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if (object == Player[0]) {
        other = Player[1];
    } else if (object == Player[1]) {
        other = Player[0];
    } else {
        return NULL;
    }
    if (other != NULL && (other->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        return other;
    }
    return NULL;
}

static SPECIAL_LAYER SpecialLayer[42] = {
    {&ID_DEFAULTCHARACTER_A, "bombbackpack",        0},
    {&ID_DEFAULTCHARACTER_A, "sonargun",            0},
    {&ID_DEFAULTCHARACTER_A, "infrared_goggles",    0},
    {&ID_DEFAULTCHARACTER_A, "mask_black",          0},
    {&ID_DEFAULTCHARACTER_A, "mask_blue",           0},
    {&ID_DEFAULTCHARACTER_A, "mask_red",            0},
    {&ID_DEFAULTCHARACTER_A, "cape_black",          0},
    {&ID_DEFAULTCHARACTER_A, "cape_blue",           0},
    {&ID_DEFAULTCHARACTER_A, "body_grey_nextgen",   0},
    {&ID_DEFAULTCHARACTER_A, "body_grey_high",      0},
    {&ID_DEFAULTCHARACTER_A, "body_grey_low",       0},
    {&ID_DEFAULTCHARACTER_A, "body_black_nextgen",  0},
    {&ID_DEFAULTCHARACTER_A, "body_black_high",     0},
    {&ID_DEFAULTCHARACTER_A, "body_black_low",      0},
    {&ID_DEFAULTCHARACTER_A, "body_blue_nextgen",   0},
    {&ID_DEFAULTCHARACTER_A, "body_blue_high",      0},
    {&ID_DEFAULTCHARACTER_A, "body_blue_low",       0},
    {&ID_DEFAULTCHARACTER_A, "face_hands_black",    0},
    {&ID_DEFAULTCHARACTER_A, "face_hands_blue",     0},
    {&ID_DEFAULTCHARACTER_A, "face_hands_red",      0},
    {&ID_DEFAULTCHARACTER_A, "hips_black_nextgen",  0},
    {&ID_DEFAULTCHARACTER_A, "hips_black_high",     0},
    {&ID_DEFAULTCHARACTER_A, "hips_blue_nextgen",   0},
    {&ID_DEFAULTCHARACTER_A, "hips_blue_high",      0},
    {&ID_DEFAULTCHARACTER_A, "hips_red_nextgen",    0},
    {&ID_DEFAULTCHARACTER_A, "hips_red_high",       0},
    {&ID_DEFAULTCHARACTER_B, "magnetic_boots",      0},
    {&ID_DEFAULTCHARACTER_B, "scuba_gear",          0},
    {&ID_DEFAULTCHARACTER_B, "hack_pack",           0},
    {&ID_DEFAULTCHARACTER_B, "vacuum_gun",          0},
    {&ID_DEFAULTCHARACTER_B, "nextgen_limbs_green", 0},
    {&ID_DEFAULTCHARACTER_B, "hires_limbs_green",   0},
    {&ID_DEFAULTCHARACTER_B, "lowres_limbs_green",  0},
    {&ID_DEFAULTCHARACTER_B, "nextgen_limbs_white", 0},
    {&ID_DEFAULTCHARACTER_B, "hires_limbs_white",   0},
    {&ID_DEFAULTCHARACTER_B, "lowres_limbs_white",  0},
    {&ID_DEFAULTCHARACTER_B, "nextgen_limbs_grey",  0},
    {&ID_DEFAULTCHARACTER_B, "hires_limbs_grey",    0},
    {&ID_DEFAULTCHARACTER_B, "lowres_limbs_grey",   0},
    {&ID_DEFAULTCHARACTER_B, "nextgen_limbs_blue",  0},
    {&ID_DEFAULTCHARACTER_B, "hires_limbs_blue",    0},
    {&ID_DEFAULTCHARACTER_B, "lowres_limbs_blue",   0},
};

u32 AdjustLayerBits(u32 mask, GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG *data = object->character_data->game_character;
    i32 cape_layer = data->cape_layer;
    u32 cape = cape_layer != -1 ? 1u << cape_layer : 0;
    i32 hair_layer = data->hair_layer;
    u32 hair = hair_layer != -1 ? 1u << hair_layer : 0;
    SUIT_LAYERS *suit;
    if (object->helmet != 0) {
        mask &= ~hair;
    }
    suit = object->suit;
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
        object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER) {
        mask &= ~object->character_data->game_character->ride_layers_off;
    }
    return mask;
}

static const u8 CreatureMosHat[5] = {0, 0, 0, 7, 14};
static const u8 CreatureMosHead[5] = {20, 20, 8, 8, 4};
static const u8 CreatureMosBody[3] = {3, 9, 19};
static const u8 CreatureMosArms[3] = {1, 12, 15};
static const u8 CreatureMosHands[3] = {2, 13, 16};
static const u8 CreatureMosWaist[3] = {6, 10, 17};
static const u8 CreatureMosLegs[3] = {5, 11, 18};

static void SetLayersMos(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureMosHat[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosHead[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosBody[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosArms[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosHands[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosWaist[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosLegs[qrand() / 0x5556];
}

static const u8 CreatureCantinaHead[3] = {0, 3, 6};
static const u8 CreatureCantinaBody[3] = {1, 4, 7};
static const u8 CreatureCantinaLegs[3] = {2, 5, 8};

static void SetLayersCantina(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCantinaHead[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaBody[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaLegs[qrand() / 0x5556];
}

static const u8 CreatureCloudHead[3] = {1, 5, 7};
static const u8 CreatureCloudBody[3] = {2, 3, 6};
static const u8 CreatureCloudLegs[3] = {0, 4, 8};

static void SetLayersCloud(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCloudHead[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudBody[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudLegs[qrand() / 0x5556];
    object->flags_ef4 = (object->flags_ef4 & ~APIOBJECT_GIRL) | ((object->layer_mask << 4) & 0x800);
}

static void SetLayersBob(GAMEOBJECT *object) {
    i32 head[6] = {1, 2, 3, 21, 22, 23};
    i32 body[8] = {5, 6, 7, 8, 9, 10, 16, 17};
    i32 legs[3] = {11, 12, 15};
    i32 arms[2] = {13, 14};
    i32 hands[2] = {19, 20};
    ((PLAYER_LAYER_FLAGS *)&object->flags_ef0)->bob = qrand() < 0x8000;
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

void FixUpLayers(void) {
    i32 model_index;
    i32 id;
    for (model_index = 0; model_index < apicharsys->loaded_model_count; ++model_index) {
        CHARACTER_MODEL_LAYERS *model = &apicharsys->models[model_index];
        PLAYERCHARACTERCONFIG *character = &GCDataList[model->model_id];
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

i32 fun_0041aec0(GAMEOBJECT *object) {
    u8 context = (u8)object->character_context;
    if (context == CHARACTER_CONTEXT_BATARANG && object->unknown_7b3 == 0) {
        return 2;
    }
    return (context == CHARACTER_CONTEXT_ATTRACTO_TARGET) + 1;
}

i32 SuperWeirdo(GAMEOBJECT *object) {
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && (Game.customizer_weirdo_flags & 1) != 0 && CharacterCustomiser != NULL &&
        (object->id == (u16)CharacterCustomiser->character_ids[0] || object->id == (u16)CharacterCustomiser->character_ids[1])) {
        return 1;
    }
    return 0;
}

i32 Player_HasPurpleForce(GAMEOBJECT *object) {
    if (Cheat_IsOn(28) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

void ResetPlayerMoves(GAMEOBJECT *object) {
    ResetPlayerPacket(((PLAYER_PACKET_STORAGE *)object)->player_packet_storage, object->character_data);
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
    object->unknown_282 = 0;
    object->unknown_281 = 0;
    SetGameObjectCharacterData(object);
    object->spawn_protection_timer = 0.0f;
    SetFlicker(object, 0.0f);
    ResetCoinPacket(object->coin_packet);
    object->respawn_timer = 0.0f;
    object->flags_1fc_word &= ~0x2000u;
    DrawOffsetCode(object, 1);
}

void InitPlayerAI(GAMEOBJECT *object) {
    CHARACTERDATA *character;
    i32 can_pull_levers;
    u8 type;
    char *name;
    u64 route_mask;
    u32 route_mask_word;
    if (StartupGameFlag0 != 0) {
        PlayerInitCapabilitiesVariant0(object);
    } else if (StartupGameFlag1 != 0) {
        PlayerInitCapabilitiesVariant1(object);
    } else {
        PlayerInitCapabilitiesDefault(object);
    }
    can_pull_levers = CanPullLevers((i16)object->id);
    object->ai_packet.flags &= ~0x10000u;
    object->flags_ef0 = (object->flags_ef0 & ~APIOBJECT_CAN_PULL_LEVERS) | (((u32)can_pull_levers << 23) & 0x800000u);
    object->ai_packet.route_mask_low = 0;
    object->ai_packet.route_mask_high = 0;
    if (FreePlay != 0 && (object->flags_1f8 & APIOBJECT_CREATURE) == 0) {
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAll + offsetof(PLAYER_ROUTE_MASK_WORDS, low), sizeof(route_mask_word));
        object->ai_packet.route_mask_low = route_mask_word;
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAll + offsetof(PLAYER_ROUTE_MASK_WORDS, high), sizeof(route_mask_word));
        object->ai_packet.route_mask_high = route_mask_word;
    } else if (SpecialRouteCharacterTypeIDFn != NULL) {
        name = object->suit != NULL ? ((PLAYER_SUIT_NAME *)object->suit)->name : object->character_data->name;
        type = SpecialRouteCharacterTypeIDFn(name);
        if (type != 0xff) {
            if (type < 64) {
                route_mask = (u64)1 << type;
                object->ai_packet.route_mask_low = (u32)route_mask;
                object->ai_packet.route_mask_high = (u32)(route_mask >> 32);
            } else {
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAll + offsetof(PLAYER_ROUTE_MASK_WORDS, low), sizeof(route_mask_word));
                object->ai_packet.route_mask_low = route_mask_word;
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAll + offsetof(PLAYER_ROUTE_MASK_WORDS, high), sizeof(route_mask_word));
                object->ai_packet.route_mask_high = route_mask_word;
            }
        }
    }
    object->run_speed_override = 1000000000.0f;
    object->walk_speed_override = 1000000000.0f;
    character = object->character_data;
    object->ai_packet.creature_set = 0;
    object->portal_flags |= APIOBJECT_CAN_DEFEND;
    if ((character->model_flags & CHARACTER_MODEL_BLASTER) != 0) {
        object->portal_flags |= 4;
    } else {
        object->portal_flags &= ~4u;
    }
    object->portal_flags = (object->portal_flags & 0x25fffc27) | APIOBJECT_CAN_USE_WEAPON;
    object->flags_ef0 &= 0xc7ffe6c0;
    object->hover_height_override = 1000000000.0f;
    object->flags_ef4 &= 0x31f7e95c;
    object->collision_scale_override = 1000000000.0f;
    object->flags_ef8_word &= 0xfffffffc;
    object->doomed_escape_locator = NULL;
    if ((object->flags_1f8 & APIOBJECT_CREATURE) != 0) {
        object->flags_ef4 &= ~APIOBJECT_CAN_HELP_WITH_TRIGGERS;
    } else {
        object->flags_ef4 |= APIOBJECT_CAN_HELP_WITH_TRIGGERS;
    }
    object->extra_layer_mask = 0;
    object->ai_packet.movement_target_radius = DEFAULT_MOVE_RANGE;
    object->ai_packet.flags &= ~0x10000000u;
    if (object->ai_packet.movement_target_radius > 0.0f) {
        object->ai_packet.flags = (object->ai_packet.flags & ~0x8000000u) | 0x4000000u;
    } else {
        object->ai_packet.flags &= ~0xc000000u;
    }
    object->movement_speed_scale = 1.0f;
    object->flags_ef4 &= 0xfe5fffff;
    object->portal_flags &= 0xfff3ffff;
    if ((character->model_flags & CHARACTER_MODEL_RESPAWN) != 0) {
        object->portal_flags |= APIOBJECT_RESPAWNABLE;
    } else {
        object->portal_flags &= ~APIOBJECT_RESPAWNABLE;
    }
    object->tag_flags &= ~APIOBJECT_TAG_DISABLED;
    object->unknown_ec4 = 0.0f;
    object->ai_packet.flags &= 0xffffafff;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    object->ai_packet.antinode_timer = 0.0f;
    object->portal_flags &= 0xfadff7fe;
    object->timer_ecc = 0.0f;
    object->flags_ef0 &= ~APIOBJECT_NOT_WITH_PARTY;
    object->alert_target = NULL;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.opponent = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->unknown_eb0 = 0;
    object->unknown_eb4 = 0;
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
    if ((character->model_flags & CHARACTER_MODEL_DONT_PUSH) != 0 && (object->player_index == -1 || object->rider != NULL)) {
        object->flags_1fc_word |= APIOBJECT_FLAG_DONT_PUSH;
    } else {
        object->flags_1fc_word &= ~APIOBJECT_FLAG_DONT_PUSH;
    }
    if ((character->game_character->flags_090 & CHARACTER_RUNTIME_DONT_MOVE_OUT_OF_WAY) != 0) {
        object->flags_ef0 |= 0x80000000u;
    } else {
        object->flags_ef0 &= ~0x80000000u;
    }
    object->flags_1fc_word &= 0xffe7f7ff;
    object->flags_ef4 &= ~4u;
    object->animation_speed_multiplier = 1.0f;
    object->flags_ef0 &= 0xfbbfffff;
    object->flags_ef8_word &= 0xfffffff3;
    object->unknown_1038 = 0.0f;
    object->indy_climb = NULL;
    object->unknown_1083 = 0;
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

i32 NewPlayerCharacter(GAMEOBJECT *object, i32 id, i32 old_id, i32 unused) {
    PLAYERCHARACTERCONFIG *config;
    NUVEC velocity;
    AIPATHINFO path;
    AILOCATOR *saved_locator;
    u32 saved_f08;
    u32 saved_frame_state;
    u32 saved_ai_flag;
    u32 lean_enabled;
    i32 saved_flag;
    i32 lean;
    i32 context;
    i32 health;
    i32 hitpoints;
    u8 route;
    u8 next_route;
    i32 on_surface;
    if (id == old_id || id < 0 || id >= CHARCOUNT || apicharsys->playermodelids[id] == -1) {
        return 0;
    }
    Shards_HandleLostObj(WORLD, object);
    if (VehicleArea == 0) {
        AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
    }
    context = object->character_context;
    lean = object->movement_lean_angle;
    lean_enabled = object->character_data->model_flags & CHARACTER_MODEL_VEHICLE;
    LoseHelmet(object, 0, 1);
    DestroySnakeBody(object);
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->character_data = (CHARACTERDATA *)&apicharsys->char_data[id];
    velocity = object->velocity;
    health = object->current_hp;
    hitpoints = object->unknown_107e;
    saved_flag = object->flags_1fc_bit_13;
    object->takeover_owner = NULL;
    object->suit = Suit_GetDefault(id);
    saved_locator = object->doomed_escape_locator;
    saved_ai_flag = ((NEW_PLAYER_FLAGS *)object)->flags_ef0_bit_21;
    route = object->ai_packet.current_route;
    next_route = object->ai_packet.next_route;
    path = object->ai_packet.path_info;
    saved_f08 = object->route_frame_state;
    saved_frame_state = object->ai_packet.frame_state;
    InitPlayerAI(object);
    object->doomed_escape_locator = saved_locator;
    ((NEW_PLAYER_FLAGS *)object)->flags_ef0_bit_21 = saved_ai_flag;
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
    Player_ResetContexts(((CREATURE_RESET_PACKET_VIEW *)object)->packet);
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
    object->unknown_107e = hitpoints;
    object->flags_1fc_bit_13 = saved_flag;
    ResetMiniAnimPacket((MINIANIMPACKET *)object->mini_anim_packet, -1);
    object->weapon_scale = ((object->flags_e20 >> 16) & 1) != 0 ? 1.0 : 0.0;
    object->unknown_e33 = 0;
    if (on_surface == 0) {
        object->velocity.y = velocity.y;
    } else {
        ((PLAYER_RESET_FIELDS *)object)->ground_contact_grace_timer = 0.2f;
    }
    if (context == CHARACTER_CONTEXT_SLIDE && StartSlide(object, 0)) {
        object->velocity.x = velocity.x;
        object->velocity.z = velocity.z;
    }
    if (object->velocity.y == 0.0f) {
        object->velocity.y = -0.1f;
    }
    if (lean_enabled != 0 && (object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
        object->movement_lean_angle = (u16)lean;
    }
    object->view_distance = apicharsys->char_data[(i16)object->id].game_character->viewdistance;
    object->hear_distance = apicharsys->char_data[(i16)object->id].game_character->heardistance;
    object->max_view_height = apicharsys->char_data[(i16)object->id].game_character->maxviewheight;
    object->min_view_height = apicharsys->char_data[(i16)object->id].game_character->minviewheight;
    config = object->character_data->game_character;
    object->flags_ef4 = (object->flags_ef4 & ~APIOBJECT_GIRL) | ((config->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN) {
        SetLayersMos(object);
    } else if (object->id == (u16)id_CANTINAALIEN) {
        SetLayersCantina(object);
    } else if (object->id == (u16)id_CLOUDCITYCITIZEN) {
        SetLayersCloud(object);
    } else if (object->id == (u16)id_BOB) {
        SetLayersBob(object);
    } else if (object->id == (u16)id_GEONOSIAN) {
        ((NEW_PLAYER_FLAGS *)object)->geonosian_variant = qrand() < 0x8000;
    }
    InitCreatureModelFlags(object);
    return 1;
}

void ResetAICreature(GAMEOBJECT *object, AISYS_PC *system) {
    AICREATURE *creature;
    AIPATHINFO *path_info;
    AIROW *row;
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
    object->current_hp = object->unknown_107e = object->character_data->game_character->hitpoints;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    creature = &system->creatures[object->ai_packet.creature_index];
    object->ai_packet.creature_set = ((AI_CREATURE_RESET_VIEW *)creature)->creature_set;
    if ((creature->flags & 1) != 0) {
        object->ai_packet.flags |= AIPACKET_IGNORE_WALL_SPLINES;
    } else {
        object->ai_packet.flags &= ~AIPACKET_IGNORE_WALL_SPLINES;
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
    object->unknown_276 = (u16)angle;
    object->facing_angle = (u16)angle;
    object->movement_facing_angle = (u16)angle;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, (u16)angle);
    AISysCharacterSetPath(&object->ai_packet, path_info->path);
    AISysCharacterSetPathCnx(&object->ai_packet, &object->position, path_info->connection, path_info->direction);
    object->dead = 0;
    AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, creature,
                          ((AI_CREATURE_RESET_VIEW *)creature)->script_name, NULL, 1, NULL, NULL);
    object->ai_packet.inside_path_node = -1;
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, (i8)object->unknown_281);
    ResetAnimPacket(&object->anim_packet, 1);
    SetAnimTimeRandom(object->character_model, &object->anim_packet);
    object->ai_packet.movement_stopping_distance = 0.0f;
    ((AI_RESET_EXTRA *)object)->reset_timer = 0.0f;
    object->ai_packet.flags &= ~AIPACKET_FORMATION_COMMANDER;
    object->ai_packet.movement_destination = object->position;
    object->ai_packet.path_target = NULL;
    ((AI_RESET_EXTRA *)object)->search_state = 0;
    ((AI_RESET_EXTRA *)object)->avoidance_state = 0;
    PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
    object->flags_1fc_word |= APIOBJECT_FLAG_CHARACTER;
    ++object->ai_packet.respawn_count;
    object->ai_packet.reset_mode = 2;
    if (object->ai_packet.group != NULL) {
        object->ai_packet.group->member_is_alive |= (u32)((u64)1 << object->ai_packet.group_member_index);
        row = &object->ai_packet.group->rows[object->ai_packet.group_row];
        row->is_alive |= (u8)((u64)1 << (object->ai_packet.group_member_index -
                                         ((AI_CREATURE_RESET_VIEW *)creature)->count_across * object->ai_packet.group_row));
        if (object->ai_packet.group_column == 0) {
            row->position = object->position;
            row->path_info = object->ai_packet.path_info;
            row->rotation = object->unknown_276;
            row->flags &= ~2u;
            row->next_connection = NULL;
        }
    }
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    if ((object->character_data->model_flags & CHARACTER_MODEL_ONLY_ACTIVE_WHEN_TAKEN_OVER) != 0 &&
        object->character_model->action_animations[0x41] != NULL) {
        object->context_timer = 1000000000.0f;
        object->character_context = CHARACTER_CONTEXT_DEACTIVATED;
        object->unknown_7aa = ACTION_DEACTIVATED;
        ResetAnimPacket(&object->anim_packet, 0x41);
    }
}

void ResetAICreatures(AISYS_PC *system) {
    GAMEOBJECT *object;
    AICREATURE *creature;
    i32 index;
    if (system == NULL || (((AI_CREATURE_RESET_FLAGS *)system)->reset_flags & 1) != 0) {
        return;
    }
    ((AI_CREATURE_RESET_FLAGS *)system)->reset_flags |= 1;
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1f8 & APIOBJECT_CREATURE) == 0) {
            continue;
        }
        object->flags_1fc_word &= ~APIOBJECT_FLAG_CHARACTER;
        if (WORLD->level_progress != NULL && (((u64)1 << index) & ((LEVEL_AI_PROGRESS *)WORLD->level_progress)->disabled_ai_objects) != 0) {
            object->dead = 1;
        } else {
            creature = &system->creatures[object->ai_packet.creature_index];
            object->ai_packet.respawn_count = 0;
            object->ai_packet.reset_mode = 0;
            AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, creature,
                                  ((AI_CREATURE_RESET_VIEW *)creature)->script_name, "InActive", 1, NULL, NULL);
            if (object->ai_packet.script_processor.state != NULL && object->ai_packet.script_processor.state->name != NULL &&
                NuStrICmp(object->ai_packet.script_processor.state->name, "InActive") == 0) {
                creature->activation_type = 2;
            }
            if (creature->activation_type != 0) {
                continue;
            }
            if (Game.difficulty >= creature->activation_difficulty) {
                if (creature->count > 1 && creature->start_stagger > 0.0f && object->ai_packet.group_member_index != 0) {
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

i32 addcreature_override_id_check = 0;

i32 InitCreature(GAMEOBJECT *object, i32 id, i32 parameter) {
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
    object->character_data = (CHARACTERDATA *)&apicharsys->char_data[id];
    object->layer_mask = object->character_data->game_character->layer_mask;
    object->flags_1f8 = 0;
    object->level_object_index = -1;
    memset(((CREATURE_RESET_PACKET_VIEW *)object)->packet, 0, 0x788);
    object->unknown_280 = -1;
    start = *Player_StartPos(object);
    object->flags_1f8 |= 2;
    if (parameter != 0) {
        object->flags_1f8 |= 0x4000;
    }
    if ((WORLD->current_level->flags & LEVEL_IN_SPACE) == 0) {
        object->flags_1f8 |= 0x40;
    }
    object->pad = GamePad_Allocate();
    object->pad->input_mode = 1;
    object->current_hp = object->unknown_107e = object->character_data->game_character->hitpoints;
    ResetPlayerPacket(((CREATURE_RESET_PACKET_VIEW *)object)->packet, object->character_data);
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
    object->flags_ef4 = (object->flags_ef4 & ~APIOBJECT_GIRL) | ((object->character_data->game_character->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN) {
        SetLayersMos(object);
    } else if (object->id == (u16)id_CANTINAALIEN) {
        SetLayersCantina(object);
    } else if (object->id == (u16)id_CLOUDCITYCITIZEN) {
        SetLayersCloud(object);
    } else if (object->id == (u16)id_BOB) {
        SetLayersBob(object);
    } else if (object->id == (u16)id_GEONOSIAN) {
        ((PLAYER_LAYER_FLAGS *)&object->flags_ef0)->bob = qrand() < 0x8000;
    }
    InitCreatureModelFlags(object);
    return 1;
}

/* Place actual owners in canonical data.cpp extern-C cohort. */
i32 GameKeepMergeAction = 0;

i32 GameDrawCharacterModel(CHARACTER_MODEL_LAYERS *model, ANIMPACKET *animation, NUMTX *matrix, NUMTX *secondary_matrix,
                           NUMTX *reflection_matrix, NUMTX *auxiliary_matrix, GAMEOBJECT *object, u32 flags) {
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
    if (object != NULL && (object->character_data->model_flags & CHARACTER_MODEL_COMPLEX_SHADOW) != 0 &&
        (WORLD->area == NULL || WORLD->area != BATTLEOVERCORUSCANT_ADATA)) {
        ((CHARACTER_DRAW_HIERARCHY *)model->hierarchy)->suppress_shadow_surface_points = 0;
        if (reflection_matrix != NULL) {
            ConfigureComplexShadow(object);
            complex_shadow = 1;
        }
    } else {
        ((CHARACTER_DRAW_HIERARCHY *)model->hierarchy)->suppress_shadow_surface_points = 1;
    }
    drawcharactermodel_keepmergeaction = GameKeepMergeAction;
    MakeLayerList = GCDataList[model->model_id].make_layer_list;
    result = APIDrawCharacterModel(model, object != NULL ? (APICHARACTERDATA *)object->character_data : NULL, animation, matrix,
                                   secondary_matrix, reflection_matrix, NULL, auxiliary_matrix, object, flags,
                                   object != NULL && GameDrawCharacterModelFlag != 0 ? object->joint_overrides : NULL,
                                   object != NULL && GameDrawCharacterModelFlag != 0 ? object->joint_override_count : 0, Paused, FRAMETIME,
                                   output_matrices, NULL, WORLD->game_debris_sys);
    if (result == 0) {
        if (object != NULL && auxiliary_matrix != NULL) {
            for (i = 0; i < 16; ++i) {
                if (model->points_of_interest[i] != NULL) {
                    auxiliary_matrix[i] = object->transform;
                    memcpy(&auxiliary_matrix[i].m30, &object->collision_position, sizeof(NUVEC));
                    auxiliary_matrix[i].m31 =
                        (f32)((f64)auxiliary_matrix[i].m31 -
                              ((f64)object->character_top + (f64)object->character_bottom) * (f64)object->origin_scale * 0.5);
                }
            }
        }
    }
    draw_shadow = result != 0 && (object == NULL || ((object->flags_ef0 & 0x4000000) == 0 &&
                                                     (object->character_data->model_flags & CHARACTER_MODEL_COMPLEX_SHADOW) == 0));
    if (PcWindowOptions.shadowmethod == 1 && draw_shadow && secondary_matrix != NULL && object != NULL) {
        u8 level_alpha = WORLD->current_level->blob_shadow_alpha;
        if (level_alpha > 0 && object->shadow_opacity > 0.0f && object->surface_normal.y > 0.0f &&
            (object->water_height == -1.0f || (TerLayer[object->unknown_283].flags & 1) == 0)) {
            if ((i16)object->id == id_MOUSEDROID && WORLD->level_objects[0xa2].active != 0) {
                f32 opacity;
                special_matrix = *secondary_matrix;
                opacity = object->shadow_opacity * object->surface_normal.y;
                NuSpecialDrawAtAlpha(&WORLD->level_objects[0xa2].special, &special_matrix, opacity);
            } else if (CharacterBlobShadowsOn != 0) {
                PLAYERCHARACTERCONFIG *config = object->character_data->game_character;
                i32 alpha = config->blob_shadow_alpha < 255 ? config->blob_shadow_alpha : level_alpha;
                f32 radius;
                if (alpha > 0) {
                    radius = config->unknown_08c < 99.0f ? config->unknown_08c : object->character_data->collision_radius;
                    if (radius > 0.0f) {
                        radius = DropInOutScale(object) * radius;
                        NuRndrAddShadow(&secondary_matrix->translation, object->origin_scale * radius,
                                        alpha * object->shadow_opacity * object->surface_normal.y, object->unknown_1052, 0,
                                        object->unknown_1054);
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

i32 DrawGameObjectsProcess(void) {
    GAMEOBJECT *object = Obj;
    i32 index;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        NUVEC position;
        NUVEC scale;
        f32 drop_scale;
        NUMTX matrix;
        NUMTX orientation;
        NUMTX reflected;
        NUMTX shadow;
        i32 angle;
        object->flags_1f8 |= 0x800;
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0) {
            continue;
        }
        object->flags_e24 &= ~8u;
        ((DRAW_PROCESS_FLAGS *)object)->origin_joint_was_ready = object->origin_joint_ready;
        object->origin_joint_ready = 0;
        if (object->dead != 0 && (object->unknown_1010 > 0.0f || object->death_delay == 0.0f)) {
            continue;
        }
        if ((object->flags_e20 & 0x20) != 0 && object->character_context != CHARACTER_CONTEXT_DROP_IN &&
            object->character_context != CHARACTER_CONTEXT_DROP_OUT) {
            continue;
        }
        if (object->character_context == CHARACTER_CONTEXT_TELEPORT && object->unknown_7b3 == 1 &&
            ((DRAW_PREPASS_TELEPORT_OBJECT *)object)->teleport != NULL &&
            (((DRAW_PREPASS_TELEPORT_OBJECT *)object)->teleport->flags & 4) == 0) {
            continue;
        }
        if (object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER || object->character_context == CHARACTER_CONTEXT_EATEN) {
            continue;
        }
        position.x = object->position.x;
        position.y = object->position.y;
        if ((object->flags_1f8 & 0x100) != 0) {
            position.y = object->character_bottom * object->origin_scale + position.y;
        }
        position.z = object->position.z;
        NuVecAdd(&position, &position, &object->render_offset);
        ((DRAW_PROCESS_FLAGS *)object)->previous_model_draw_result = object->model_draw_result;
        object->model_draw_result = 1;
        scale.x = object->origin_scale;
        drop_scale = DropInOutScale(object);
        scale.x = drop_scale * scale.x;
        scale.y = scale.z = scale.x;
        NuMtxSetScale(&matrix, &scale);
        switch ((u8)object->movement_mode) {
            case 0:
                GameDrawMtxSetYRotation(&orientation, 0x8000);
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateX(&orientation, object->pitch_angle);
                }
                angle = object->unknown_276;
                if (angle != 0) {
                    GameDrawMtxRotateY(&orientation, angle);
                }
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZ(&orientation, object->roll_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 1:
                GameDrawMtxSetYRotation(&orientation, object->unknown_276 + 0x8000);
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateX(&orientation, object->pitch_angle);
                }
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZ(&orientation, object->roll_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 3:
                GameDrawMtxSetYRotation(&orientation, 0x8000);
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZ(&orientation, object->roll_angle);
                }
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateX(&orientation, object->pitch_angle);
                }
                angle = object->unknown_276;
                if (angle != 0) {
                    GameDrawMtxRotateY(&orientation, angle);
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
                GameDrawMtxSetYRotation(&orientation, object->unknown_276 + 0x8000);
                if (object->roll_angle != 0) {
                    GameDrawMtxRotateZ(&orientation, object->roll_angle);
                }
                if (object->pitch_angle != 0) {
                    GameDrawMtxRotateX(&orientation, object->pitch_angle);
                }
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
        }
        if (object->movement_lean_angle != 0) {
            angle = object->movement_lean_angle;
            if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
                angle = (i32)((f64)PodSprint_RollMul(object) * angle);
            }
            NuMtxPreRotateZ(&matrix, angle);
        }
        if (object->secondary_lean_angle != 0) {
            NuMtxPreRotateX(&matrix, object->secondary_lean_angle);
        }
        if (object->tertiary_lean_angle != 0) {
            NuMtxPreRotateY(&matrix, object->tertiary_lean_angle);
        }
        ApplyExtraRotation(object, &matrix);
        if ((u8)object->movement_mode != 4 &&
            (object->character_context == CHARACTER_CONTEXT_DROP_IN || object->character_context == CHARACTER_CONTEXT_DROP_OUT)) {
            NUVEC displacement;
            f32 offset;
            offset = object->character_data->collision_radius * object->character_data->model_scale * 7.5;
            if (object->character_context == CHARACTER_CONTEXT_DROP_IN) {
                offset = (1.0f - drop_scale) * offset;
            } else {
                offset = -((1.0f - drop_scale) * offset);
            }
            NuVecMtxTransformVU0(&displacement, &v001, &orientation);
            NuVecScale(&displacement, &displacement, offset);
            NuVecAdd(NUMTX_GET_ROW_VEC(&matrix, 3), NUMTX_GET_ROW_VEC(&matrix, 3), &displacement);
        }
        if (object->use_model_origin < 2 &&
            (object->id == GameObjectRotationAtstId || object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT)) {
            matrix.m31 = (f32)((f64)object->character_bottom * object->origin_scale + matrix.m31);
        }
        object->transform = matrix;
        if (scale.x == 0.0f && scale.y == 0.0f && scale.z == 0.0f) {
            object->model_draw_result = 0;
            continue;
        }
        object->flags_1f8 ^= 0x800;
        object->flags_ef0 &= ~0x20000u;
        *(u8 *)&object->unknown_107c = 0;
        if (object->reflection_enabled != 0 && (f64)object->reflection_height != 2000000.0 &&
            object->ai_update_distance < WORLD->current_level->reflection_range) {
            *(u8 *)&object->unknown_107c = (u8)MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                                                WORLD->current_level->reflection_y, &reflected);
            object->draw_tertiary_mtx = reflected;
        }
        if (object->dead == 0 && (f64)object->floor_height != 2000000.0 && (object->flags_1f8 & 0x40) != 0 &&
            object->character_data->game_character->shadow_locators == 0) {
            GameDrawMtxCopy(&shadow, &ANI3_EVAL_IDENTITY_MATRIX);
            GameDrawMtxRotateY(&shadow, object->unknown_276 + 0x8000);
            if (object->unknown_1054 != 0) {
                GameDrawMtxRotateZ(&shadow, object->unknown_1054);
            }
            if (object->unknown_1052 != 0) {
                GameDrawMtxRotateX(&shadow, object->unknown_1052);
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
    PLAYERCHARACTERCONFIG *data;
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
    i32 red;
    i32 green;
    i32 blue;
    i32 purple;
    u16 rotation = 0;
    i32 models[4] = {-1, -1, -1, -1};
    i32 hilt = -1;
    i32 reflected_models[4] = {-1, -1, -1, -1};
    i32 sabre = 0;
    if (weapon_scale <= 0.0f) {
        return;
    }
    if ((object->character_data->game_character->flags_094 & CHARACTER_OPTION_NO_WEAPON_DRAW) != 0) {
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
                    if (object->player_index != -1 && Cheat_IsOn(25)) {
                        red = 1;
                    } else if (object->player_index != -1 && Player_HasPurpleForce(object)) {
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
                if (object->weapon_model_override != NULL) {
                    models[0] = *object->weapon_model_override;
                }
                if (models[0] == -1) {
                    continue;
                }
            } else if (StartupGameFlag1 != 0) {
                if ((object->character_data->model_flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BLASTER)) == 0x80) {
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
                    if (object->player_index != -1 && Cheat_IsOn(25)) {
                        red = 1;
                        green = blue = purple = 0;
                    } else if (red || green || blue || purple) {
                        if (object->player_index != -1 && Player_HasPurpleForce(object)) {
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
            } else if (object->id == (u16)dat_007f1810 || (object->character_data->model_flags & CHARACTER_MODEL_BLASTER) != 0) {
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
                        ((object->player_index != -1 && Cheat_IsOn(25)) || (object->player_index != -1 && Player_HasPurpleForce(object)))) {
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

static void DrawExtraCharacterLayerBits(GAMEOBJECT *object, i32 character_id, NUMTX *joint_matrices, ANIMPACKET *animation, i32 locator,
                                        NUMTX *override_matrix) {
    i16 model_index;
    CHARACTER_MODEL_LAYERS *model;
    NUMTX matrix;
    NUMTX reflected;
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
    if ((u8)object->unknown_107c != 0 && (f64)object->reflection_height != 2000000.0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y, &reflected) !=
            0) {
        reflection = &reflected;
    } else {
        reflection = NULL;
    }
    GameDrawCharacterModel(model, animation, &matrix, NULL, reflection, NULL, NULL, 0xffffffffu);
}

void DrawCharacterAttachments(GAMEOBJECT *object, NUMTX *joint_matrices) {
    i32 character_id;
    i32 current_animation;
    DRAW_ATTACHMENTS_PACKET animation;
    NUMTX *override_matrix = NULL;
    if (object->suit != NULL && (object->suit->flags & 2) != 0) {
        if (object->id != (u16)ID_DEFAULTCHARACTER_A) {
            return;
        }
        character_id = dat_007f17bc;
        AnimPacketMiniToFull((DRAW_ATTACHMENTS_MINI *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)PunchRangeUnitId && (AnimPlaying(&object->anim_packet, ACTION_PUNCH, 1, 1) != NULL ||
                                                       AnimPlaying(&object->anim_packet, ACTION_PUNCH2, 1, 1) != NULL ||
                                                       AnimPlaying(&object->anim_packet, ACTION_PUNCH3, 1, 1) != NULL ||
                                                       AnimPlaying(&object->anim_packet, ACTION_PUNCH_BEHIND, 1, 1) != NULL)) {
        character_id = PunchRangeCompanionId;
        memcpy(&animation, (u8 *)object + offsetof(GAMEOBJECT, anim_packet), sizeof(animation));
        if (animation.packet.blending != 0 && animation.packet.blend_target_animation != ACTION_PUNCH &&
            animation.packet.blend_target_animation != ACTION_PUNCH2 && animation.packet.blend_target_animation != ACTION_PUNCH3 &&
            animation.packet.blend_target_animation != ACTION_PUNCH_BEHIND) {
            return;
        }
    } else if (object->id == (u16)dat_007f17c8) {
        character_id = dat_007f17cc;
        AnimPacketMiniToFull((DRAW_ATTACHMENTS_MINI *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)dat_007f17d0 || object->id == (u16)dat_007f17d4) {
        if (AnimPacketModelsCompatible(&object->anim_packet, (i16)object->id, PunchRangeCompanionId) == 0) {
            return;
        }
        character_id = PunchRangeCompanionId;
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
    DrawExtraCharacterLayerBits(object, character_id, joint_matrices, &animation.packet,
                                object->character_data->game_character->extra_character_locator, override_matrix);
}

void DrawParaphernalia(GAMEOBJECT *object) {
    i32 i;
    NUMTX matrix;
    NUMTX reflected;
    NUMTX special_reflected;
    NUMTX attachments[16];
    PARAPHERNALIA_EXTRA *extra;
    GAMEOBJECT *carried;
    PARAPHERNALIA_EXTRA *carried_extra;
    i32 ride_locator;
    NUVEC carried_position;
    NUMTX *carried_reflection;
    u32 carried_layers;
    GAMEOBJECT *passenger;
    i32 passenger_locator;
    NUANGVEC passenger_rotation;
    PLAYERCHARACTERCONFIG *config;
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

    extra = ((PARAPHERNALIA_EXTRA *)(object));
    if (DrawParaphernaliaEnabled == 0) {
        return;
    }

    carried = object->takeover_source;
    if (carried != NULL && object->character_context != CHARACTER_CONTEXT_BEEN_TAKEN_OVER &&
        !object->character_data->game_character->dont_draw_rider) {
        carried_extra = ((PARAPHERNALIA_EXTRA *)(carried));
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
            object->ai_update_distance < WORLD->current_level->reflection_range &&
            MatrixReflection(&matrix, carried->reflection_enabled, carried->reflection_height, WORLD->current_level->reflection_y,
                             &reflected)) {
            carried_reflection = &reflected;
        } else {
            carried_reflection = NULL;
        }
        carried_layers = AdjustLayerBits(carried->character_data->game_character->layer_mask_medium, carried);
        if (GameDrawCharacterModel(carried->character_model, &carried->anim_packet, &matrix, NULL, carried_reflection, attachments, NULL,
                                   carried_layers)) {
            if (carried->helmet != 0) {
                DrawObjectOnCharacter(WORLD, carried, carried->helmet + 0xf9, NULL, carried->character_data->game_character->helmet_locator,
                                      -1, attachments, carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f, 1.0f);
            }
            if (Cheat_IsOn(2)) {
                DrawObjectOnCharacter(WORLD, carried, 0xe7, NULL, carried->character_data->game_character->head_locator, -1, attachments,
                                      carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f, 1.0f);
            }
            DrawCharacterAttachments(carried, attachments);
            if (((i16)(carried->id)) == PermanentWeirdo1Id || ((i16)(carried->id)) == PermanentWeirdo2Id) {
                CustomiserDrawAccessories(CharacterCustomiser, carried, attachments);
            }
        }
    } else if ((object->flags_e24 & 1) != 0 && object->force_target != NULL) {
        passenger = object->force_target;
        passenger_locator = object->character_data->game_character->weapon_joints[0];
        if (passenger_locator != -1 && object->character_model->points_of_interest[passenger_locator] != NULL) {
            matrix = object->locator_matrices[passenger_locator];
        } else {
            passenger_rotation.x = -0x4000;
            passenger_rotation.y = object->unknown_276;
            NuMtxSetRotationXYVU0(&matrix, &passenger_rotation);
            matrix.m30 = object->position.x + NuTrigTable[object->unknown_276 >> 1] * object->bounds_radius * 1.75;
            matrix.m31 = object->collision_min.y + (object->collision_max.y - object->collision_min.y) * 0.6000000238418579;
            matrix.m32 = object->position.z + NuTrigTable[((object->unknown_276 + 0x4000) >> 1) & 0x7fff] * object->bounds_radius * 1.75;
        }
        GameDrawCharacterModel(passenger->character_model, &passenger->anim_packet, &matrix, NULL, NULL, NULL, NULL,
                               passenger->character_data->game_character->layer_mask_medium);
    }
    config = object->character_data->game_character;
    if ((config->flags_094 & CHARACTER_OPTION_MINIKIT_WITH_SCENE) != 0) {
        if (BonusArea != 0 && VehicleArea != 0) {
            CharMiniKit_Draw(((i16)(object->id)), &object->transform, object->reflection_enabled, object->reflection_height,
                             WORLD->current_level->reflection_y);
        }
    } else if ((object->character_data->flags & 1) != 0) {
        scene_locator = config->thingy_locator;
        matrix = scene_locator != -1 && object->character_model->points_of_interest[scene_locator] != NULL && object->model_draw_result != 0
                     ? object->locator_matrices[scene_locator]
                     : object->transform;
        reflected = object->draw_tertiary_mtx;
        CharScene_Draw(WORLD, ((i16)(object->id)), &matrix, extra->draw_reflection ? &reflected : NULL);
    } else if (((i16)(object->id)) == id_CATAPULT &&
               (object->character_context != CHARACTER_CONTEXT_SHOOT ||
                (object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0 || object->quick_shoot_bolt_id != -1 ||
                (object->context_timer < 0.5f && GameTimer.update_count % 6 < 3))) {
        dco_prerotatez = 0xc000;
        DrawObjectOnCharacter(WORLD, object, 0xf0, NULL, object->character_data->game_character->weapon_shoot_joints[0], -1,
                              object->locator_matrices, 0, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_data->game_character->weapon_action == 1 && AnimPlaying(&object->anim_packet, ACTION_IDLE5, 1, 1) != NULL) {
        DrawObjectOnCharacter(WORLD, object, 9, NULL, object->character_data->game_character->weapon_joints[0], -1,
                              object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    } else if (object->weapon_scale > 0.0f) {
        DrawWeapons(object, extra->draw_reflection, object->weapon_scale);
        if (object->unknown_d54 > 0.0f) {
            dco_locatorposonly = 1;
            DrawObjectOnCharacter(WORLD, object, 0x35, NULL, object->character_data->game_character->weapon_shoot_joints[0], -1,
                                  object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
        }
    } else {
        DrawWeapons(object, extra->draw_reflection, ParaphernaliaWeaponScale(object));
    }
    if (object->origin_scale_override > 0.0f || object->timer_d34 > 0.0f) {
        ResetShadowMapRendering();
        ParaphernaliaShield(object, extra->draw_reflection);
        EnableShadowMapRendering(0);
    }
    if (object->communicate_blend > 0.0f) {
        ParaphernaliaCommunicate(object, extra->draw_reflection);
    }
    if (object->character_context == CHARACTER_CONTEXT_FORCED_BACK) {
        if ((WORLD->area != NULL && WORLD->area == HUB_ADATA) || WORLD->current_level == dat_0087b194 ||
            WORLD->current_level == DOOKUC_LDATA) {
            glow_position.x = object->collision_position.x;
            glow_position.y = object->character_bottom * object->origin_scale + object->position.y +
                              (object->character_top - object->character_bottom) * 0.5 * object->origin_scale;
            glow_position.z = object->collision_position.z;
            glow_radius = object->bounds_radius > object->bounds_half_height ? object->bounds_radius : object->bounds_half_height;
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
    } else if (extra->force_alpha > 0.0f && extra->force_radius > 0.0f && WORLD->level_objects[object->unknown_e1e].active != 0 &&
               (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_DRAG_BOMBS) == 0) {
        held_glow_alpha = (extra->force_alpha / FORCEGLOWTIME);
        DrawForceGlowSprite(&extra->force_position, extra->force_radius, object->unknown_e1e, held_glow_alpha, object);
    }
    if (Cheat_IsOn(2)) {
        DrawObjectOnCharacter(WORLD, object, 0xe7, NULL, object->character_data->game_character->head_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->helmet != 0) {
        if ((((i16)(object->id)) == id_CHEWBACCA || ((i16)(object->id)) == id_WOOKIEE) && object->helmet != 5 && object->helmet != 6) {
            helmet_locator = 9;
        } else {
            helmet_locator = object->character_data->game_character->helmet_locator;
        }
        DrawObjectOnCharacter(WORLD, object, object->helmet + 0xf9, NULL, helmet_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->model_draw_result != 0 && object->character_data->game_character->thrust_locators != 0 &&
        object->thrust_effect_scale > 0.0f) {
        ParaphernaliaThrust(object, extra->draw_reflection);
    }
    if (object->character_context == CHARACTER_CONTEXT_BUILD_IT) {
        GizDrawBuildItPiece(object, extra->draw_reflection);
    }
    if (object->character_context == CHARACTER_CONTEXT_PLACE_DETONATOR &&
        object->character_model->model_data_b[object->unknown_7aa] != NULL && object->unknown_7b3 == 0) {
        ParaphernaliaPlace(object, 0xec, extra->draw_reflection, object->character_data->game_character->place_locator);
    } else if (object->character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR &&
               object->character_model->model_data_b[object->unknown_7aa] != NULL && object->unknown_7b3 != 0) {
        ParaphernaliaRemove(object, 0xec, extra->draw_reflection, object->character_data->game_character->place_locator);
    }
    if ((object->flags_e20 & 0x400000) != 0) {
        dco_prerotatez = 0x4000;
        DrawObjectOnCharacter(WORLD, object, BatarangGetObjectFromCharId(((i16)(object->id))), NULL,
                              object->character_data->game_character->throw_locator, -1, object->locator_matrices, extra->draw_reflection,
                              object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_context == CHARACTER_CONTEXT_GRAPPLE) {
        Grapple_DrawLine(object);
    }
    if ((object->flags_e20 & 0x800000) != 0 || object->character_context == CHARACTER_CONTEXT_SUPER_CARRY) {
        SuperCarry_DrawObject(object);
    }
    DrawCharacterAttachments(object, object->locator_matrices);
    if (object->character_context == CHARACTER_CONTEXT_BALLOONING) {
        ParaphernaliaHand(object, extra->draw_reflection);
    }
    if (object->torpedo != NULL && object->torpedo->count > 0) {
        DrawTorpedos(object);
    }
    if (object->character_data->game_character->weapon_action == 0) {
        CustomiserDrawAccessories(CharacterCustomiser, object, NULL);
    }
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TRANSFORMATRON) != 0 && extra->force_alpha > 0.0f &&
        extra->force_radius > 0.0f) {
        transform_alpha = ((f32)(extra->force_alpha * 0.6000000238418579 + 0.4000000059604645));
        transform_radius = ((f32)((extra->force_alpha * 0.20000004768371582 + 1.399999976158142) * extra->force_radius));
        TransformDrawTarget(&extra->force_position, transform_radius, transform_alpha);
    }
    config = object->character_data->game_character;

    if (config->weapon_action == 1) {
        special_locator = config->thingy_locator;
        if (special_locator == -1 || object->character_model->points_of_interest[special_locator] == NULL ||
            (time = AnimPlaying(&object->anim_packet, ACTION_COMMUNICATE, 1, 1)) == NULL) {
            return;
        }
        if ((*time >= 70.0f && *time <= 210.0) || (*time >= 310.0f && *time <= 433.0)) {
            matrix = object->locator_matrices[special_locator];
            NuMtxPreRotateZ(&matrix, -0x4000);
            NuSpecialDrawAt(&WORLD->level_objects[10].special, &matrix);
            if (extra->draw_reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
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
    } else if (((i16)(object->id)) == dat_007f1820 && (time = AnimPlaying(&object->anim_packet, ACTION_IDLE, 0, 0)) != NULL) {
        start = AnimListFrame(object->character_model, ACTION_IDLE, 0);
        if (start >= 1.0f) {
            finish = AnimListFrame(object->character_model, ACTION_IDLE, 1);
            if (finish >= start && *time > start && *time < finish) {
                fraction = (*time - start) / (finish - start);

                if (fraction < 0.10000000149011612) {
                    special_scaling.x = fraction / 0.10000000149011612;
                } else if (fraction < 0.8999999761581421) {
                    special_scaling.x = 1.0f;
                } else {
                    special_scaling.x = 1.0 - (fraction - 0.8999999761581421) / 0.10000000149011612;
                }
                special_scaling.y = special_scaling.z = special_scaling.x;
                NuMtxSetScale(&matrix, &special_scaling);
                NuMtxRotateX(&matrix, ((u16)(((i32)(fraction * -65536.0)))));
                NuMtxRotateY(&matrix, object->unknown_276);
                NuMtxTranslate(&matrix, &object->locator_matrices[0].translation);
                special_phase = ((i32)(fraction * 32768.0));
                matrix.m31 = matrix.m31 + NuTrigTable[(special_phase >> 1) & 0x7fff] * 0.20000000298023224;
                DrawObjectOnCharacter(WORLD, object, 0xf9, NULL, 0, -1, &matrix, extra->draw_reflection, object->layer_mask, NULL, NULL,
                                      1.0f, 1.0f);
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
            if (object->rider != NULL && object->character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER) {
                object->model_draw_result = object->rider->model_draw_result;
            }
            continue;
        }
        drawn = (u8)GameDrawCharacterModel(object->character_model, &object->anim_packet, &object->transform,
                                           (object->flags_ef0 & 0x20000) != 0 ? &object->draw_secondary_mtx : NULL,
                                           (u8)object->unknown_107c != 0 ? &object->draw_tertiary_mtx : NULL, object->locator_matrices,
                                           object, object->layer_mask);
        object->model_draw_result = drawn;
        ((DRAW_OBJECTS_FLAGS *)object)->locators_updated = drawcharactermodel_locatorsupdated;
        if (drawn != 0) {
            DrawParaphernalia(object);
        } else if (((object->flags_e20 & APIOBJECT_FORCE_HIGHLIGHT_ONLY) != 0 || object->character_context == CHARACTER_CONTEXT_FORCE) &&
                   object->transform_alpha > 0.0f && object->transform_target_scale > 0.0f &&
                   WORLD->level_objects[object->unknown_e1e].active != 0 &&
                   (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_DRAG_BOMBS) == 0) {
            DrawForceGlowSprite(&((PARAPHERNALIA_EXTRA *)object)->force_position, object->transform_target_scale, object->unknown_e1e,
                                object->transform_alpha / FORCEGLOWTIME, object);
        }
        if (object->snake_body != NULL) {
            DrawSnakeBody(object);
        }
        object->origin_joint_ready = 1;
        if (object->model_draw_result != 0) {
            object->flags_ef0 |= 0x40000;
        }
        if (Paused == 0 && object->character_data->draw_fn != NULL) {
            ((void (*)(GAMEOBJECT *))object->character_data->draw_fn)(object);
        }
    }
    ResetShadowMapRendering();
}

void DrawGameObjects() {
    const f32 saved_far_clip = global_camera.unknown_64;
    global_camera.unknown_64 = VehicleArea != 0 ? DrawGameObjectsVehicleFarClip : DrawGameObjectsFarClip;
    if (global_camera.unknown_64 > saved_far_clip) {
        global_camera.unknown_64 = saved_far_clip;
    }
    NuCameraSet(&global_camera);
    DrawGameObjectsDraw(0);
    global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
}

GAMEOBJECT *ActivateCharacter(char *name, NUVEC *position, i32 angle) {
    GAMEOBJECT *object;
    if (Mission_Active(NULL) != NULL) {
        return NULL;
    }
    if (name != NULL && (object = GetNamedGameObject(WORLD->ai_sys, name)) != NULL &&
        (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 && (FreePlay == 0 || (object->flags_1f8 & APIOBJECT_CREATURE) != 0)) {
        if (object->ai_packet.creature_index == 0xff) {
            object->flags_1fc_word |= APIOBJECT_FLAG_CHARACTER;
            AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "Base");
            if (position != NULL) {
                object->position = *position;
                object->movement_facing_angle = object->facing_angle = object->unknown_276 = (u16)angle;
                AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, 1);
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

GAMEOBJECT *AddCreature(i32 id, i32 parameter) {
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

PLAYER_COIN_PACKET_PC CoinPacket[2] = {0};
PLAYER_FORCE_LOS_SLOT GizForceLOSInfo[2] = {0};
GAMEOBJECT BackUpPlayers[2] = {0};
SOCKPOSITION OldPlrSPos[8] = {0};
i32 UsePlayerList = 0;

void Players_Init(void) {
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    PLAYER_INIT_PROGRESS *progress = (PLAYER_INIT_PROGRESS *)PlayerProgress;
    i16 *list;
    i32 index;
    for (index = 0; index < 8; ++index) {
        players[index] = NULL;
    }
    if (netclient != 0) {
        return;
    }
    PLAYERCOUNT = 0;
    if ((WORLD->current_level->flags & LEVEL_GAMEPLAY) != 0) {
        if (FreePlay == 0 && (Hub_UsePlayerList == 0 || HUB_ADATA == NULL || WORLD->area != HUB_ADATA) &&
            (UsePlayerList != 1 || (PlayerList[0] != -1 && (apicharsys->playermodelids[PlayerList[0]] == -1 ||
                                                            (PlayerList[1] != -1 && apicharsys->playermodelids[PlayerList[1]] == -1))))) {
            list = Area_PlayerIDList;
        } else {
            list = PlayerList;
        }
        if (LevelChangesInArea == 0 && UsePlayerList != 0 && progress[0].active == 0 && progress[1].active != 0 && FreePlay == 0 &&
            (WORLD->area->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == 1 && list[0] != -1 &&
            list[0] == Area_StoryModelList[0].model_id && list[1] != -1 && list[1] == Area_StoryModelList[1].model_id &&
            list[0] != list[1]) {
            i16 first = list[0];
            list[0] = list[1];
            list[1] = first;
        }
        for (index = 0; list[index] != -1 && index < 8; ++index) {
            i32 id = list[index];
            GAMEOBJECT *object;
            PLAYER_INIT_EXTRA *extra;
            APIOBJECT *api;
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
            extra = (PLAYER_INIT_EXTRA *)object;
            api = (APIOBJECT *)object;
            players[PLAYERCOUNT] = object;
            object->previous_sock_position = &OldPlrSPos[PLAYERCOUNT];
            players[PLAYERCOUNT]->unknown_280 = (u8)PLAYERCOUNT;
            ((PLAYER_INIT_EXTRA *)players[PLAYERCOUNT])->batarang = &BatarangPackets[PLAYERCOUNT];
            players[PLAYERCOUNT]->torpedo = GetTorpedoPacket();
            ++PLAYERCOUNT;
            object->flags_1044 |= 1;
            object->unknown_107e = (u8)DEFAULT_PLAYERHITPOINTS;
            object->helmet = 0;
            if (UsePlayerList != 0) {
                i32 slot = object->player_index;
                object->player_controlled = PlayerProgress[slot].active;
                if (Area != last_area) {
                    object->flags_e20 &= ~APIOBJECT_WEAPON_OUT;
                    extra->previous_state = 0;
                } else if (UsePlayerList == 1) {
                    ((PLAYER_INIT_FLAGS *)object)->stored_flag = PlayerProgress[slot].unknown_7;
                    object->current_hp = PlayerProgress[slot].hitpoints;
                    if (object->current_hp > object->unknown_107e) {
                        object->current_hp = (i8)object->unknown_107e;
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
                    object->flags_1fc_word |= APIOBJECT_FLAG_PLAYER_ACTIVE;
                } else {
                    object->flags_1fc_word &= ~APIOBJECT_FLAG_PLAYER_ACTIVE;
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
            object->unknown_ec4 = 0.0f;
            extra->unknown_454 = 0.0f;
            object->ai_awareness_mask = 0;
            object->ai_seen_mask = 0;
            object->unknown_eb0 = 0;
            object->unknown_eb4 = 0;
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
            memcpy(&BackUpPlayers[index], players[index], sizeof(BackUpPlayers[index]));
        }
    }
    RememberPlayerIDs(0, players[0] != NULL ? (i16)players[0]->id : -1, players[1] != NULL ? (i16)players[1]->id : -1);
}

void InitAICreatures(AISYS_PC *system) {
    i32 creature_index;
    i32 member;
    i32 type;
    AICREATURE *creature;
    GAMEOBJECT *object;
    AIGROUP *group;
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
            object->flags_1f8 |= APIOBJECT_CREATURE;
            object->flags_1fc_word &= ~APIOBJECT_FLAG_CHARACTER;
            model_flags = apicharsys->char_data[creature->type].model_flags;
            if ((model_flags & 0x200) != 0) {
                object->flags_1f8 |= APIOBJECT_SIDE_NEUTRAL;
            } else if ((model_flags & 4) != 0) {
                object->flags_1f8 |= APIOBJECT_SIDE_BADDIE;
            }
            object->flags_1044 |= 1;
            if ((apicharsys->char_data[creature->type].model_flags & CHARACTER_MODEL_CAN_SHOOT_OFFSCREEN) != 0) {
                object->flags_1044 |= 4;
            }
            object->ai_packet.creature_index = (u8)creature_index;
            if (member == 0 && creature->count > 1 && creature->start_stagger == 0.0f) {
                group = CreateAIGroup(system, creature->count_across, creature->x_spacing, creature->z_spacing,
                                      object->character_data->game_character->run_speed);
            }
            if (group != NULL) {
                AddToAIGroup(group, (APIOBJECT *)object);
            }
            ((AI_CREATURE_INIT_PACKET *)&object->ai_packet)->area = creature->area;
            ((AI_CREATURE_INIT_PACKET *)&object->ai_packet)->locator = creature->locator;
            object->ai_packet.respawn_locator = creature->respawn_locator;
            object->ai_packet.creature_set = creature->set;
        }
    }
    system->reset_flags &= ~1u;
}
