#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef union AI_ACTION_FLAGS_EF4_PC {
    u32 word;
    struct {
        u32 unused0 : 25;
        u32 no_idle_speed : 1;
        u32 unused26 : 6;
    } bits;
} AI_ACTION_FLAGS_EF4_PC;

STATIC_ASSERT(sizeof(AI_ACTION_FLAGS_EF4_PC) == 4);

extern void __cdecl AiRndrLine3d(NURND_VERTEX3D_PC *, NUMTL *, NUMTX *);

void AIRetreatFromDestination2(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);

typedef struct GIZSPECIAL_PARTIAL_PC {
    u8 unknown_00[0x20];
    GAMEANIMSET *anim_set;
} GIZSPECIAL_PARTIAL_PC;

STATIC_ASSERT(offsetof(GIZSPECIAL_PARTIAL_PC, anim_set) == 0x20);

extern const char AITransferLiteral75782c[5];

extern const char AITransferLiteral7583e4[12];

extern const char AITransferLiteral7583f0[8];

extern const char AIActionFalseName[6];

extern const char AIScriptActionNameArgPrefix[6];

extern const char AreaConfigKeyword_character[10];

extern const char AITransferLiteral757a38[12];

extern const char AIGameCharacterName[11];

extern const char AIScriptFalse[6];

extern const char AITransferLiteral757a70[11];

extern const char GizmoDefaultOutputName[8];

typedef struct AITURRET_PARTIAL_PC {
    u8 unknown_00[0xe8];
    GAMEOBJECT *controller;
} AITURRET_PARTIAL_PC;

typedef struct AIOBSTACLE_PARTIAL_PC {
    u8 unknown_00[0x9c];
    u32 runtime_flags;
} AIOBSTACLE_PARTIAL_PC;

STATIC_ASSERT(offsetof(AITURRET_PARTIAL_PC, controller) == 0xe8);

STATIC_ASSERT(offsetof(AIOBSTACLE_PARTIAL_PC, runtime_flags) == 0x9c);

extern i32 turret_gizmotype_id;

extern i32 obstacle_gizmotype_id;

extern i32 blowup_gizmotype_id;

void __cdecl ReleaseTakeOver(GAMEOBJECT *, i32);

void __cdecl PartKill_ForceThrow(PART_PC *, i32);

void __cdecl FindForcePushTarget(GAMEOBJECT *object, i32 activate, i32 target_filter);

STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);

STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_d20) == 0xd20);

STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_1) == 0x68);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_3) == 0x6c);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_4) == 0x70);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_timer) == 0x9c);

extern const char AIGameSetName[5];

extern const char AILocatorIndexedFormat[6];

extern const char AIGameNearestName[8];

extern const char AIActionPersonalName[9];

extern const char AITransferLiteral7577dc[8];

extern const char AIActionIndexedName[8];

extern const char AILocatorPersonalFormat[6];

extern const char ActionSnapDontCheckTerrainName[19];

extern const char AITransferLiteral757824[7];

extern const char AITransferLiteral75781c[8];

extern const char AITransferLiteral757834[13];

extern const char AITransferLiteral757884[12];

extern const char AITransferLiteral757a68[6];

extern const char AITransferLiteral757c10[13];

extern const char AITransferLiteral757c20[12];

extern const char AIPathLocatorName[9];

extern const char AITransferLiteral758194[11];

extern const char AITransferLiteral758188[11];

extern const char AITransferLiteral7581a0[8];

extern const char AITransferLiteral758174[19];

extern const char AITransferLiteral7581d4[10];

extern const char AITransferLiteral7581ec[9];

extern const char AITransferLiteral7581e0[9];

extern const char AITransferLiteral757cd0[6];

extern const char AITransferLiteral757a8c[7];

extern const char AITransferLiteral757a94[9];

extern const char AITransferLiteral757a84[8];

extern const char ZArgPrefix[3];

extern const char YArgPrefix[3];

extern const char AITransferLiteral757c5c[4];

extern const char XArgPrefix[3];

extern const char AITransferLiteral757c64[4];

extern const char AITransferLiteral757c60[4];

extern const char AITransferLiteral757f28[8];

extern const char AIGameOpponentName[9];

extern const char AITransferLiteral7583f8[8];

extern const char RaceOpponentDistancePrefix[9];

STATIC_ASSERT(offsetof(AIPACKET_PC, antinode_timer) == 0x18c);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_collision_priority_pc) == 0x292);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_resolved_collision_priority_pc) == 0x294);

STATIC_ASSERT(offsetof(APIOBJECT_PC, collision_link) == 0x298);

typedef void(__cdecl *CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);

STATIC_ASSERT(offsetof(AIPATH_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AIPATH_PC, node_count) == 0x10);

STATIC_ASSERT(offsetof(AIPATH_PC, flags) == 0x11);

STATIC_ASSERT(sizeof(AIPATH_PC) == 0xa8);

STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);

STATIC_ASSERT(offsetof(AIPATH_PC, index) == 0x15);

STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);

STATIC_ASSERT(offsetof(AIPATH_PC, route_matrix) == 0x84);

STATIC_ASSERT(offsetof(AIPATH_PC, bounds_min) == 0x90);

STATIC_ASSERT(offsetof(AIPATH_PC, bounds_max) == 0x9c);

/* Original AISYS PC callbacks; bounded native evidence in ../native. */

typedef struct ai_context_reset_view_pc_s {
    u8 unknown_000[0x6c4];
    u8 reset_contexts;
} AI_CONTEXT_RESET_VIEW_PC;

STATIC_ASSERT(offsetof(AI_CONTEXT_RESET_VIEW_PC, reset_contexts) == 0x6c4);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);

STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_set) == 0x378);

STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);

STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

void __cdecl GizObstacle_JumpToStart(GIZOBSTACLE *);

void __cdecl GizObstacle_JumpToEnd(GIZOBSTACLE *);

void __cdecl GizObstacle_PlayForwards(GIZOBSTACLE *);

void __cdecl GizObstacle_PlayBackwards(GIZOBSTACLE *);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);

typedef struct ai_force_fields_pc_s {
    u8 unknown000[0x1c];
    NUVEC position;
    u8 unknown028[0x74 - 0x28];
    u32 config_flags;
    u8 unknown078[0xa0 - 0x78];
    u32 runtime_flags;
} AI_FORCE_FIELDS_PC;

typedef struct ai_force_object_pc_s {
    u8 unknown000[0x10cc];
    GIZFORCE_PC *force;
} AI_FORCE_OBJECT_PC;

STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, position) == 0x1c);

STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, config_flags) == 0x74);

STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, runtime_flags) == 0xa0);

STATIC_ASSERT(offsetof(AI_FORCE_OBJECT_PC, force) == 0x10cc);

/* Original AISYS callback family; PC-native seven-argument callback ABI. */
extern i32 __cdecl TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);

extern void __cdecl RegisterTakeOverObject(GAMEOBJECT *);

STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);

STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);

extern const char AIActionBluePrefixPc[6];

extern const char AIActionGreenPrefixPc[7];

extern const char AIActionRedPrefixPc[5];

extern const char AIActionCountPrefixPc[7];

extern const char AIActionTotalPrefixPc[7];

extern const char AIActionMyPosNamePc[6];

extern const char AIActionCanBeStolenNamePc[12];

extern const char AIActionTorpedoNamePc[8];

typedef struct torpedo_action_packet_pc_s {
    u8 count;
    u8 flags;
    u8 unknown_02;
    u8 state;
    u8 unknown_04[4];
    f32 timer;
    u8 unknown_0c[0x30];
    NUVEC pickup_positions[5];
    u8 unknown_78[0x10];
} TORPEDO_ACTION_PACKET_PC;

STATIC_ASSERT(sizeof(TORPEDO_ACTION_PACKET_PC) == 0x88);

STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, count) == 0);

STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, flags) == 1);

STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, state) == 3);

STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, timer) == 8);

STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, pickup_positions) == 0x3c);

STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);

STATIC_ASSERT(offsetof(AISYS_PC, player_2) == 0x1390);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);

STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);

STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);

STATIC_ASSERT(offsetof(AISYS_PC, areas) == 0x38);

STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);

STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);

STATIC_ASSERT(offsetof(AISYS_PC, scripts) == 0x1c);

/* Prospective src/gameapi/aisys.c append; existing typed state/shape contracts reused. */
void __cdecl NuRndrBoundingBox(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, i32 colour);

STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);

STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);

void AiRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *material, NUMTX *matrix);

i32 __cdecl NewRayCastScaleY(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags);

STATIC_ASSERT(offsetof(AIPATH_PC, nodes) == 0x7c);

STATIC_ASSERT(sizeof(NUVEC) == 12);

void __cdecl routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);

void __cdecl areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);

static const char GizActionLevelPrefixPc[7] = "level=";

void __cdecl FollowAPIObject(APIOBJECT_PC *, APIOBJECT_PC *, i32, f32);

#pragma bss_seg(".bss$prev_droid_ix")
static i32 prev_droid_ix = 0;
#pragma bss_seg()

static const char CompleteLevelCutscenePrefix[10] = "cutscene=";

static const char NewLevelPrefix[10] = "newlevel=";

STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);

STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);

STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);

STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);

STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);

STATIC_ASSERT(offsetof(WORLDINFO, script_processor_count) == 0x2aec);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator_set) == 0xa8);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, myspline) == 0xac);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);

void __cdecl LevelScriptReStoreProgress(WORLDINFO *, LEVEL_SCRIPT_PROCESS_PC *);

extern const char ActionSnapXName[2];

extern const char ActionSnapZName[2];

GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *, char *);

void __cdecl GizmoPickup_TurnOnPickup(GIZMOPICKUP_PC *);

f32 guided_speed = 30.0f;

f32 guided_life = 8.0f;

f32 guided_start_time = 0.5f;

i32 guided_rotate_speed = 240000;

extern i32 __cdecl Action_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);

extern i32 __cdecl GizmoGetTypeIDByName(void *, char *);

STATIC_ASSERT(offsetof(TECHNO_s, flags) == 0x3c);

typedef struct ai_cable_head_pc_s {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
} AI_CABLE_HEAD_PC;

typedef struct ai_cable_object_pc_s {
    u8 unknown000[0x1040];
    CABLE_RECORD_PC *cable;
} AI_CABLE_OBJECT_PC;

STATIC_ASSERT(offsetof(AI_CABLE_HEAD_PC, source) == 0);

STATIC_ASSERT(offsetof(AI_CABLE_HEAD_PC, target) == 4);

STATIC_ASSERT(offsetof(AI_CABLE_OBJECT_PC, cable) == 0x1040);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, locator) == 0x36c);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, mover_height) == 0x3e8);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, last_path_position) == 0x438);

STATIC_ASSERT(offsetof(AILOCATOR_PC, path_info) == 0x20);

i32 __cdecl Action_EngageOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);

i32 __cdecl CanFightLikeAJedi(GAMEOBJECT *);

i32 __cdecl CharCategory_FindByName(char *);

extern const char AITransferLiteral7576c0[6];

extern const char AITransferLiteral7576b8[7];

extern const char AITransferLiteral7576b0[6];

extern const char AITransferLiteral7576a8[7];

extern const char AITransferLiteral75769c[11];

extern const char AITransferLiteral75768c[13];

STATIC_ASSERT(offsetof(AIPACKET_PC, terrain_origin) == 0x14c);

STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);

STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);

STATIC_ASSERT(offsetof(AIPACKET_PC, creature_index) == 0x138);

struct GIZSPINNER_s *__cdecl GizSpinner_FindBySpecialName(void *world_ptr, char *name);

STATIC_ASSERT(offsetof(GAMEOBJECT, animation_speed_multiplier) == 0x1034);

STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) + offsetof(AIPATHINFO_PC, flags) == 0x166);

STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_ef2) == 0xef2);

STATIC_ASSERT(offsetof(AIPACKET_PC, respawn_count) == 0x1fc);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_ef0) == 0xef0);

i32 __cdecl GizmoPickup_BeenTurnedOn(GIZMOPICKUP_PC *);

extern i32 gizbuildit_gizmotype_id;

STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);

STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);

STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) + offsetof(SOCKPOSITION_PC, distance) == 0x69c);

STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_position) + offsetof(SPLINEPOS, finished) == 0xe6b);

extern f32 debug_condition;

STATIC_ASSERT(offsetof(AIPACKET_PC, reset_mode) == 0x13e);

typedef struct AI_CONDITION_TURRET_PC {
    u8 unknown_000[0x13a];
    u16 flags;
} AI_CONDITION_TURRET_PC;

STATIC_ASSERT(offsetof(AI_CONDITION_TURRET_PC, flags) == 0x13a);

STATIC_ASSERT(sizeof(AI_CONDITION_TURRET_PC) == 0x13c);

STATIC_ASSERT(sizeof(FLOWBOX_PC) == 0x20);

STATIC_ASSERT(offsetof(FLOWBOX_PC, state_flags_low) == 0xa);

STATIC_ASSERT(offsetof(FLOWBOX_PC, name) == 0x1c);

STATIC_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai) == 4);

i32 __cdecl Hint_isAvailable(i32 hint_id);

i32 __cdecl Hint_isCompleteByIdPc(i32 hint_id);

typedef struct ai_global_trigger_world_pc_s {
    u8 unknown_0000[0x4678];
    AITRIGGERSETSYS_s *trigger_system;
} AI_GLOBAL_TRIGGER_WORLD_PC;

STATIC_ASSERT(offsetof(AI_GLOBAL_TRIGGER_WORLD_PC, trigger_system) == 0x4678);

STATIC_ASSERT(offsetof(AITRIGGERSETSYS_s, field_0x4280) == 0x4280);

STATIC_ASSERT(offsetof(AITRIGGERSETSYS_s, field_0x42c0) == 0x42c0);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent_metric) == 0x3b0);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, flags) == 0x4b0);

STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);

STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_f10) == 0xf10);

i32 __cdecl ForceAnimationDonePc(GIZFORCE_PC *force);

typedef struct condition_trigger_process_reader_pc_s {
    u8 unknown_00[0xa0];
    AIAREA_PC *trigger_area;
} CONDITION_TRIGGER_PROCESS_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PROCESS_READER_PC, trigger_area) == 0xa0);

STATIC_ASSERT(sizeof(AIAREA_PC) == 0x40);

STATIC_ASSERT(offsetof(AIAREA_PC, system) == 0x38);

typedef struct condition_trigger_player_reader_pc_s {
    u8 unknown_00[0x2b0];
    u64 membership;
} CONDITION_TRIGGER_PLAYER_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PLAYER_READER_PC, membership) == 0x2b0);

STATIC_ASSERT(offsetof(AIPACKET_PC, group) == 0x144);

STATIC_ASSERT(offsetof(AIGROUP_PC, member_count) == 7);

STATIC_ASSERT(offsetof(AIGROUP_PC, members) == 0xc);

STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);

STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent) == 0xd4);

STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent_metric) == 0xd8);

STATIC_ASSERT(offsetof(GAMEOBJECT, last_attacker) == 0x10a8);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);

typedef struct ai_been_to_level_progress_pc_s {
    u8 unknown_0000[0x2800];
    u32 visited_flags;
    u8 unknown_2804[0x624];
} AI_BEEN_TO_LEVEL_PROGRESS_PC;

STATIC_ASSERT(sizeof(AI_BEEN_TO_LEVEL_PROGRESS_PC) == sizeof(LEVEL_PROGRESS));

STATIC_ASSERT(offsetof(AI_BEEN_TO_LEVEL_PROGRESS_PC, visited_flags) == 0x2800);

STATIC_ASSERT(offsetof(AREA_DATA_PC, levels) == 0x60);

STATIC_ASSERT(offsetof(AREA_DATA_PC, level_count) == 0x7d);

typedef struct ai_on_object_platform_pc_s {
    u8 unknown_00[0x40];
    NUMTX *scene_object;
    u8 unknown_44[0x28];
} AI_ON_OBJECT_PLATFORM_PC;

typedef struct ai_on_object_terrain_pc_s {
    u8 unknown_00[0x68];
    AI_ON_OBJECT_PLATFORM_PC *platforms;
} AI_ON_OBJECT_TERRAIN_PC;

STATIC_ASSERT(sizeof(AI_ON_OBJECT_PLATFORM_PC) == 0x6c);

STATIC_ASSERT(offsetof(AI_ON_OBJECT_PLATFORM_PC, scene_object) == 0x40);

STATIC_ASSERT(offsetof(AI_ON_OBJECT_TERRAIN_PC, platforms) == 0x68);

STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);

STATIC_ASSERT(offsetof(GAMEOBJECT, supporting_platform_id) == 0x27e);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_282) == 0x282);

STATIC_ASSERT(offsetof(GAMEOBJECT, position) + offsetof(NUVEC, y) == 0x60);

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_target) == 0xea4);

STATIC_ASSERT(sizeof(NUSPECIALHANDLE) == 0xc);

STATIC_ASSERT(offsetof(NUINSTANIM, ltime) == 0x4c);

STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);

STATIC_ASSERT(offsetof(APIOBJECT_PC, owner) == 0);

STATIC_ASSERT(offsetof(CUTINFO, instance) == 4);

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

STATIC_ASSERT(offsetof(GIZOBSTACLE, triggering_object) == 0x38);

STATIC_ASSERT(offsetof(AIAREA_PC, runtime_flags) == 0x2a);

STATIC_ASSERT(offsetof(GIZOBSTACLE, runtime_flags) == 0x9c);

/* Native instance reader: only the condition's flags field is consumed. */
typedef struct condition_cut_instance_reader_pc_s {
    u8 opaque_00_88[0x88];
    u32 flags;
} CONDITION_CUT_INSTANCE_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_CUT_INSTANCE_READER_PC, flags) == 0x88);

typedef struct condition_hat_name_reader_pc_s {
    u8 unknown_00[0x40];
    char name[16];
} CONDITION_HAT_NAME_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_HAT_NAME_READER_PC, name) == 0x40);

STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, name) == 0x58);

STATIC_ASSERT(offsetof(GIZPANEL_s, name) == 0x40);

STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);

SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);

void AINetReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet);

/* Candidate source for src/gameapi/aiscript.c. Readonly access-view bindings
 * retain their separately reviewed original owner and extent policies. */
extern const char ActionSnapPlayer1Name[8];

extern const char ActionSnapPlayer2Name[8];

extern const char ActionSnapRotationYName[5];

extern const char ActionSnapPartyName[6];

extern const char ActionSnapDistanceFromEndName[18];

extern const char ActionSnapSockName[5];

extern const char ActionSnapDxName[3];

extern const char ActionSnapDyName[3];

extern const f64 ActionSnapRotationFactorPc;

extern const f64 ActionSnapWeightedRandomZeroPc;

typedef NUVEC *(__cdecl *AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);

extern const char AISetLayerKeywordPc[11];

extern const char AIClearLayerKeywordPc[13];

typedef struct ai_force_platform_record_pc_s {
    u8 unknown_00[0x40];
    NUMTX *scene_matrix;
    u8 unknown_44[0x28];
} AI_FORCE_PLATFORM_RECORD_PC;
STATIC_ASSERT(sizeof(AI_FORCE_PLATFORM_RECORD_PC) == 0x6c);
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_RECORD_PC, scene_matrix) == 0x40);
typedef struct ai_force_platform_terrain_pc_s {
    u8 unknown_00[0x68];
    AI_FORCE_PLATFORM_RECORD_PC *platforms;
} AI_FORCE_PLATFORM_TERRAIN_PC;
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_TERRAIN_PC, platforms) == 0x68);
typedef struct ai_force_platform_object_data_pc_s {
    u8 unknown_00[2];
    i16 platform_id;
} AI_FORCE_PLATFORM_OBJECT_DATA_PC;
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_OBJECT_DATA_PC, platform_id) == 2);

typedef struct ai_trigger_world_pc_s {
    u8 unknown_0000[0x4678];
    AITRIGGERSETSYS_s *trigger_system;
} AI_TRIGGER_WORLD_PC;
typedef struct ai_trigger_grapple_pc_s {
    u8 unknown_00[0x24];
    NUVEC ground_position;
} AI_TRIGGER_GRAPPLE_PC;
typedef struct ai_trigger_object_pc_s {
    u8 unknown_0000[0x1072];
    u8 grapple_direction;
} AI_TRIGGER_OBJECT_PC;
STATIC_ASSERT(offsetof(AI_TRIGGER_WORLD_PC, trigger_system) == 0x4678);
STATIC_ASSERT(offsetof(AI_TRIGGER_GRAPPLE_PC, ground_position) == 0x24);
STATIC_ASSERT(offsetof(AI_TRIGGER_OBJECT_PC, grapple_direction) == 0x1072);
typedef struct ai_trigger_force_pc_s {
    u8 unknown_00[0x1c];
    NUVEC position;
    u8 unknown_28[0x4c];
    u32 config_flags;
} AI_TRIGGER_FORCE_PC;
STATIC_ASSERT(offsetof(AI_TRIGGER_FORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(AI_TRIGGER_FORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1088) == 0x1088);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_target) == 0x10cc);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, trigger_count) == 0x20c);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, targets) == 0x24);
STATIC_ASSERT(offsetof(AITRIGGERSET_TARGET, position) == 0x10);
STATIC_ASSERT(offsetof(AITRIGGERSET_TARGET, path) == 0x20);
void __cdecl ClearSpecialMove(GAMEOBJECT *object);
f32 __cdecl GizSpinner_GetNearestTargetPoint(GIZSPINNER_s *, NUVEC *, NUVEC *, NUVEC *, i32);

void __cdecl SockOn(SOCKSYS_PC *sys, i32 index);
void __cdecl SockOff(SOCKSYS_PC *sys, i32 index);
u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION, LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
u32 LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
typedef struct bulk_ai_lever_pc_s {
    u8 unknown_00[0x40];
    NUVEC floor_position;
    u8 unknown_4c[0x1c];
    NUVEC position;
    u8 unknown_74[4];
    f32 pull_progress;
    u8 unknown_7c[0x12];
    u16 animation_frame;
    u8 unknown_90[4];
    u16 flags;
} BULK_AI_LEVER_PC;
typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
typedef struct bulk_ai_techno_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    NUVEC ground_position;
    u8 unknown_28[0x14];
    u8 flags;
} BULK_AI_TECHNO_PC;
i32 __cdecl GizTechno_CanUseTechno(GAMEOBJECT *object, BULK_AI_TECHNO_PC *techno);
typedef struct ai_action_controller_world_pc_s {
    u8 unknown_0000[0x4670];
    AI_CONTROL_SYSTEM_PC *control_system;
} AI_ACTION_CONTROLLER_WORLD_PC;
struct AIPATHCNXCONTROLLER_s *__cdecl AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *, AISYS_PC *, AIPATH_PC *, char *,
                                                                char *, i32, char *, i32, i32);
void __cdecl AIPathCnxControllerSetOnRange(struct AIPATHCNXCONTROLLER_s *, i32, i32);
const char AIControllerLiteralPc00757e34[10];
const char AIControllerLiteralPc00757d98[10];
const char AIControllerLiteralPc00757d70[16];
const char AIControllerLiteralPc00757db4[9];
const char AIControllerLiteralPc00757de4[9];
const char AIControllerLiteralPc00757e18[9];
const char AIControllerLiteralPc00757e40[10];
const char AIControllerLiteralPc00757dc8[14];
const char AIControllerLiteralPc00757da4[7];
const char AIControllerLiteralPc00757e10[8];
const char AIControllerLiteralPc00757d60[12];
const char AIControllerLiteralPc00757dac[8];
const char AIControllerLiteralPc00757dc0[7];
const char AIControllerLiteralPc00757dfc[5];
const char AIControllerLiteralPc00757d6c[3];
const char AIControllerLiteralPc00757d80[14];
const char AIControllerLiteralPc00757e04[9];
const char AIControllerLiteralPc00757d54[10];
const char AIControllerLiteralPc00757dd8[12];
const char AIControllerLiteralPc00757d90[7];
const char AIControllerLiteralPc00757e24[13];
const char AIControllerLiteralPc00757df0[10];
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, pull_progress) == 0x78);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, animation_frame) == 0x8e);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, ground_position) == 0x1c);
STATIC_ASSERT(offsetof(AI_ACTION_CONTROLLER_WORLD_PC, control_system) == 0x4670);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, position) == 0x68);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, position) == 0x10);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, flags) == 0x3c);
i32 __cdecl GizmoGetTypeIDByName(void *system, char *name);
SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);
void __cdecl SockOn(SOCKSYS_PC *system, i32 index);
void __cdecl SockOff(SOCKSYS_PC *system, i32 index);
i32 turret_gizmotype_id;
i32 obstacle_gizmotype_id;
const char GizmoSnapOptionNamePc[5];
GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *, char *);
const char GizActionLiteral758274[];
const char GizActionLiteral75827c[];
const char GizActionLiteral758280[];
const char GizActionLiteral758288[];
const char GizActionLiteral75829c[];
const char GizActionLiteral7582b4[];
const char GizActionLiteral7582d0[];
const char GizActionLiteral7582e8[];
const char GizActionLiteral758300[];
const char GizActionLiteral758318[];
const char GizActionLiteral758328[];
const char GizActionLiteral758330[];
const char GizActionLiteral758338[];
const char GizActionLiteral75834c[];
const char GizActionLiteral758360[];
const char GizActionLiteral758374[];
const char AITransferLiteral757824[], AITransferLiteral757a7c[];
extern const char AIConditionMySetNamePc[6];
extern const char AIConditionTakeoverTargetNamePc[15];

GAMEOBJECT *__cdecl GetNamedGameObject(void *sys, char *name) {
    APIOBJECT_PC *result;
    if (GetNamedGameObjectCallback != NULL) {
        result = GetNamedGameObjectCallback(sys, name);
        if (result != NULL) {
            return (GAMEOBJECT *)result->owner;
        }
    }
    return NULL;
}

/* Forward declarations for functions used before their retail position. */

static i32 __cdecl PartyMemberInWay(GAMEOBJECT *object, GAMEOBJECT *opponent) {
    NUVEC difference;
    NUVEC direction;
    f32 target_distance;
    f32 length;
    f32 inverse;
    f32 radius;
    f32 radius_squared;
    i32 index;
    GAMEOBJECT *member;
    if ((object->flags_1f8 & 5u) != 0)
        return 0;
    target_distance = NuVecDistSqr(&opponent->collision_position, &object->collision_position, &difference);
    length = NuFsqrt(target_distance);
    inverse = length != 0.0f ? 1.0f / length : 0.0f;
    NuVecScale(&direction, &difference, inverse);
    for (index = 0; index < 8; index++) {
        member = (GAMEOBJECT *)Player[index];
        if (member == NULL || (member->flags_1fc_word & 0x1001u) != 0x1001u || member == object || member == opponent)
            continue;
        if (NuVecDistSqr(&object->collision_position, &member->collision_position, &difference) < target_distance) {
            member = (GAMEOBJECT *)Player[index];
            radius = (f32)(member->bounds_radius + 0.125);
            radius_squared = radius * radius;
            if (LineIntersectSphere(&object->collision_position, &direction, &member->collision_position,
                                    radius_squared, NULL) != 0)
                return 1;
        }
    }
    return 0;
}

AILOCATOR_PC *LocalGetRandomLocator(AILOCATOR_PC **locators, i32 count, f32 clip_radius, NUVEC *position,
                                    f32 max_distance, i32 outside_camera, f32 max_delta_y, f32 min_delta_y) {
    i32 candidate_count;
    i32 i;
    i32 selected;
    struct AILOCATOR_PC *result;
    i32 candidates[64];
    candidate_count = 0;
    result = NULL;
    if (count > 64) {
        count = 64;
    }
    for (i = 0; i < count; ++i) {
        u8 valid;
        if (locators[i] == NULL) {
            continue;
        }
        valid = 0;
        if (outside_camera != 0) {
            if (NuCameraClipTestSphere(&locators[i]->position, 0.0f, &NuMtxSetIdentityTemplate) == 0) {
                valid = 1;
            }
        } else {
            valid = 1;
            if (clip_radius > 0.0f) {
                if (NuCameraClipTestSphere(&locators[i]->position, clip_radius, &NuMtxSetIdentityTemplate) == 0) {
                    const i16 room = (i16)(NuPortalWhichRoom(WORLD->current_gscn, &locators[i]->position));
                    valid = 0;
                    if (room >= 0 && WORLD->rooms_visible_ptr[room] == 0) {
                        valid = 1;
                    }
                }
            }
        }
        if (valid != 0) {
            if (max_distance == 1000000000.0f) {
                candidates[candidate_count++] = i;
            } else {
                NUVEC delta;
                const f32 distance = NuVecDistSqr(position, &locators[i]->position, &delta);
                if (min_delta_y != 1000000000.0f && delta.y < min_delta_y) {
                    continue;
                }
                if (max_delta_y != 1000000000.0f && max_delta_y < delta.y) {
                    continue;
                }
                if (distance < max_distance * max_distance) {
                    candidates[candidate_count++] = i;
                }
            }
        }
    }
    if (candidate_count != 0) {
        selected = candidates[qrand() / (65535 / candidate_count + 1)];
        result = locators[selected];
        locators[selected] = NULL;
    }
    return result;
}

AILOCATOR_PC *__cdecl LocalGetNearestLocator(void *locator_array, i32 count, f32 clip_radius, NUVEC *position,
                                             f32 max_distance, i32 outside_camera, f32 max_delta_y, f32 min_delta_y) {
    f32 nearest_distance;
    i32 candidates[64];
    i32 candidate_count;
    i32 i;
    i32 nearest;
    AILOCATOR_PC *result;
    AILOCATOR_PC **locators = (AILOCATOR_PC **)locator_array;
    nearest_distance = max_distance == 1000000000.0f ? max_distance : max_distance * max_distance;
    candidate_count = 0;
    if (count > 64) {
        count = 64;
    }
    for (i = 0; i < count; ++i) {
        if (locators[i] == NULL) {
            continue;
        }
        if (outside_camera != 0) {
            if (NuCameraClipTestSphere(&locators[i]->position, 0.0f, &numtx_identity) != 0) {
                continue;
            }
        } else if (clip_radius > 0.0f &&
                   NuCameraClipTestSphere(&locators[i]->position, clip_radius, &numtx_identity) == 0) {
            continue;
        }
        candidates[candidate_count++] = i;
    }
    nearest = -1;
    for (i = 0; i < candidate_count; ++i) {
        NUVEC delta;
        const f32 distance = NuVecDistSqr(position, &locators[candidates[i]]->position, &delta);
        if (min_delta_y != 1000000000.0f && min_delta_y > delta.y) {
            continue;
        }
        if (max_delta_y != 1000000000.0f && delta.y > max_delta_y) {
            continue;
        }
        if (!(nearest_distance > distance)) {
            continue;
        }
        nearest_distance = distance;
        nearest = i;
    }
    if (nearest == -1) {
        return NULL;
    }
    result = locators[candidates[nearest]];
    locators[candidates[nearest]] = NULL;
    return result;
}

static __declspec(noinline) AILOCATOR_PC *FirstSpawnSetLocator(AILOCATORSET_PC *set, AISYS_PC *sys) {
    AILOCATOR_PC *locator = NULL;
    if (set != NULL && set->locator_count != 0)
        locator = &sys->locators[set->locator_entries[0]];
    return locator;
}

static void GizActionNextLocator(AISYS_PC *sys, AILOCATORSET_PC *set, i32 looping, i32 finish_at_end,
                                 i32 ignore_assigned, APIOBJECT_PC *object) {
    i32 passed_start = 0;
    i32 current;
    i32 direction;
    i32 candidate;
    AILOCATOR_PC *locator;
    if (set == NULL || set->locator_count <= 1 || object == NULL)
        return;
    if (ignore_assigned != 0)
        AILocatorSet_CheckLocatorsStillAssigned(sys, set);
    locator = object->ai->locator;
    direction = (object->flags_low & 0x100000) == 0 ? 1 : -1;
    current = set->locator_count;
    if (locator != NULL) {
        i32 index;
        for (index = 0; index < set->locator_count; ++index) {
            if (locator == &sys->locators[set->locator_entries[index]]) {
                set->assigned[index] = 0xff;
                current = index;
                break;
            }
        }
    }
    if (current == set->locator_count)
        current = qrand() / (0xffff / set->locator_count + 1);
    candidate = current;
    for (;;) {
        candidate += direction;
        if (candidate >= set->locator_count) {
            if (finish_at_end != 0) {
                object->ai->locator = NULL;
                return;
            }
            if (looping != 0)
                candidate = 0;
            else {
                direction = -1;
                object->flags_low |= 0x100000;
                candidate = set->locator_count - 2;
            }
        } else if (candidate < 0) {
            if (looping != 0)
                candidate = set->locator_count - 1;
            else {
                object->flags_low &= ~0x100000;
                direction = 1;
                candidate = 1;
            }
        }
        if (candidate == current) {
            if (looping != 0 || ++passed_start > 1)
                break;
        } else if (ignore_assigned == 0 || set->assigned[candidate] == 0xff)
            break;
    }
    object->ai->locator = &sys->locators[set->locator_entries[candidate]];
    set->assigned[candidate] = object->object_id;
}

f32 __cdecl Condition_GlynTestPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return 1.0f;
}

f32 __cdecl Condition_DebugPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                              void *argument) {
    return debug_condition;
}

f32 __cdecl Condition_ActivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                               void *argument) {
    if (packet != NULL && packet->reset_mode == 2)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_GotGunPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                               void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        if ((packet->owner->objptr->portal_flags & 4) != 0)
            return 1.0f;
        return 0.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_PrefersBrawlingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        if ((packet->owner->character_data->model_flags & 0x80000000u) != 0)
            return 1.0f;
        return 0.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_ObstacleAtEndInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, name);
}

void *__cdecl Condition_SpecialAtStartInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, name);
}

f32 __cdecl Condition_ObstacleAtStartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ObstacleAtEnd(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ObstacleLockedShutPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if ((((GIZOBSTACLE *)argument)->runtime_flags & 8) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ObstacleLockedOpenPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if ((((GIZOBSTACLE *)argument)->runtime_flags & 4) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_ObstacleLockedOpenInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, name);
    if (gizmo != NULL)
        return gizmo->object;
    return NULL;
}

f32 __cdecl Condition_ObstacleOpenedByPlayerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)argument;
    if (obstacle != NULL && player != NULL && obstacle->triggering_object == player)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ObstacleOpenedByEitherPlayerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                     char *name, void *argument) {
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)argument;
    if (obstacle != NULL) {
        if (player != NULL && obstacle->triggering_object == player)
            return 1.0f;
        if (player2 != NULL && obstacle->triggering_object == player2)
            return 1.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_ForceAtStartInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
}

f32 __cdecl Condition_ForceAtStartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ForceAtEndPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_IsVisibleInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name;
}

f32 __cdecl Condition_IsVisible(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char *unused3,
                                void *argument) {
    NUSPECIALHANDLE special = {NULL, NULL, NULL};
    NuSpecialFind(WORLD->current_gscn, &special, (char *)argument, 1);
    if (NuSpecialExistsFn(&special) != 0)
        return (f32)(NuSpecialGetVisibilityFn(&special));
    return 0.0f;
}

f32 __cdecl Condition_MySetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                              void *argument) {
    return (f32)process->creature_set;
}

void *__cdecl Condition_ForcePushingInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_ForcePushingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
            return 0.0f;
        object = packet->owner->objptr;
    }
    if (object != NULL && object->character_context == 0x1b)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_TurretAliveInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo;
    void *turret;
    gizmo = GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, name);
    if (gizmo != NULL) {
        turret = gizmo->object;
        if (turret != NULL)
            return turret;
    }
    return NULL;
}

f32 __cdecl Condition_TurretAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    AI_CONDITION_TURRET_PC *turret = (AI_CONDITION_TURRET_PC *)argument;
    u16 flags;
    if (turret != NULL) {
        memcpy(&flags, &turret->flags, sizeof(flags));
        if ((flags & 0x30) == 0)
            return 1.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_CategoryIsInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && system != NULL && CharCategory != NULL)
        return (void *)CharCategory_FindByName(name);
    return (void *)-1;
}

f32 __cdecl Condition_CategoryIs(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    if ((i32)argument != -1 && packet != NULL && packet->owner != NULL &&
        CharCategory_IsCategory(packet->owner->objptr, (i32)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 Condition_PlayerCategoryIs(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char *unused3,
                               void *argument) {
    const i32 category = (i32)argument;
    if (category != -1 && player != NULL && CharCategory_IsCategory(player, (i32)category) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmAPartyCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->field_280 != 0xff)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_IAmAInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL) {
        for (index = 0; index < CHARCOUNT; ++index) {
            if (NuStrICmp(CDataList[index].name, name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}

f32 __cdecl Condition_IAmAPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        (i32)(i16)packet->owner->objptr->id == (i32)argument)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_OpponentIsAInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL) {
        for (index = 0; index < CHARCOUNT; ++index) {
            if (NuStrICmp(CDataList[index].name, name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}

f32 __cdecl Condition_OpponentIsAPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->opponent != NULL && packet->opponent->objptr != NULL &&
        (i16)packet->opponent->objptr->id == (i32)argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_GameOpponentIsAThreatPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    if (packet != NULL)
        return (f32)((packet->flags >> 11) & 1u);
    return 0.0f;
}

f32 __cdecl Condition_CanFightLikeAJedi(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        CanFightLikeAJedi(packet->owner) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmAGoodyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 5) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmABaddyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 0x10001) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmANeutralPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 4) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmAGoodyBaddyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 0x10000) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_EitherPlayerIsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_EitherPlayerIsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (system != NULL && argument != NULL && (system->player_1 == argument || system->player_2 == argument))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_Player1IsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (system != NULL && argument != NULL && system->player_1 == argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_Player2IsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (system != NULL && argument != NULL && system->player_2 == argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmPlayer2Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *current = player;
        GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
        if (current == first) {
            if (packet->owner->objptr == (GAMEOBJECT *)Player[1])
                return 1.0f;
        } else if (current == (GAMEOBJECT *)Player[1] && packet->owner->objptr == first) {
            return 1.0f;
        }
    }
    return 0.0f;
}

void *__cdecl Condition_PlayerUsingForceInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? gizmo->object : NULL;
}

f32 __cdecl Condition_UsingForcePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (GizForce_GameObjUsingForce(packet->owner->objptr, (GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerUsingForcePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (GizForce_GameObjUsingForce(player, (GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerUsingForce(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2,
                                             char *unused3, void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    if (GizForce_GameObjUsingForce(player, force) != 0)
        return 1.0f;
    if (GizForce_GameObjUsingForce(player2, force) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_OnForcePlatformInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    GIZFORCE_PC *force;
    if (gizmo != NULL) {
        force = (GIZFORCE_PC *)gizmo->object;
        if (force != NULL && (force->runtime_flags & 0x100) != 0)
            return force;
    }
    return NULL;
}

f32 __cdecl Condition_OnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *object;
    i16 platform_id;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    GAMEANIMOBJ *animation;
    if (force == NULL || packet == NULL || packet->owner == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    if (object->field_281 == 0 && object->field_282 == 0)
        return 0.0f;
    platform_id = object->supporting_platform_id;
    if (platform_id == -1)
        return 0.0f;
    terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
    if (!((f64)object->position.y >= (f64)terrain->platforms[platform_id].scene_matrix->m31))
        return 0.0f;
    animation = force->anim_set->objects;
    while (animation != NULL) {
        if (platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        animation = animation->next;
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerOnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *object;
    i16 platform_id;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    GAMEANIMOBJ *animation;
    if (force == NULL)
        return 0.0f;
    object = player;
    if (object == NULL || (object->field_281 == 0 && object->field_282 == 0))
        return 0.0f;
    platform_id = object->supporting_platform_id;
    if (platform_id == -1)
        return 0.0f;
    terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
    if (!((f64)object->position.y >= (f64)terrain->platforms[platform_id].scene_matrix->m31))
        return 0.0f;
    animation = force->anim_set->objects;
    while (animation != NULL) {
        if (platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        animation = animation->next;
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerOnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *first;
    GAMEOBJECT *second;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    i32 eligible;
    i16 platform_id;
    GAMEANIMOBJ *animation;
    if (force != NULL) {
        first = player;
        if (first != NULL) {
            terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
            eligible = 0;
            if (first->field_281 != 0 || first->field_282 != 0) {
                platform_id = first->supporting_platform_id;
                if (platform_id != -1 &&
                    (f64)first->position.y >= (f64)terrain->platforms[platform_id].scene_matrix->m31)
                    eligible = 1;
            }
            second = player2;
            if (second != NULL && (second->field_281 != 0 || second->field_282 != 0)) {
                platform_id = second->supporting_platform_id;
                if (platform_id != -1 &&
                    (f64)second->position.y >= (f64)terrain->platforms[platform_id].scene_matrix->m31)
                    eligible |= 2;
            }
            if (eligible != 0) {
                animation = force->anim_set->objects;
                while (animation != NULL) {
                    if ((eligible & 1) != 0 &&
                        first->supporting_platform_id ==
                            ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
                        return 1.0f;
                    if ((eligible & 2) != 0 &&
                        second->supporting_platform_id ==
                            ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
                        return 1.0f;
                    animation = animation->next;
                }
            }
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_ForceBeingUsedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    if (force != NULL && ((force->runtime_flags & 0x200000) != 0 || force->reset_word != 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerDeflectingPartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    if (player != NULL && player->force_part != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerPullingLeverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                 char *name, void *argument) {
    GIZMO_LEVER_PC *target;
    if (player != NULL && player->character_context == 0x4a && (target = (GIZMO_LEVER_PC *)player->grapple) != NULL &&
        (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    if (player2 != NULL && player2->character_context == 0x4a &&
        (target = (GIZMO_LEVER_PC *)player2->grapple) != NULL && (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerUsingHatMachinePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    CONDITION_HAT_NAME_READER_PC *target;
    if (player != NULL && player->character_context == 0x61 &&
        (target = (CONDITION_HAT_NAME_READER_PC *)player->grapple) != NULL &&
        (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    if (player2 != NULL && player2->character_context == 0x61 &&
        (target = (CONDITION_HAT_NAME_READER_PC *)player2->grapple) != NULL &&
        (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerUsingPanelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    GIZPANEL_s *target;
    if (player != NULL && player->character_context == 0x0b && (target = (GIZPANEL_s *)player->grapple) != NULL &&
        (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    if (player2 != NULL && player2->character_context == 0x0b && (target = (GIZPANEL_s *)player2->grapple) != NULL &&
        (name == NULL || NuStrICmp(target->name, name) == 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerWearingHelmetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    if (player->helmet == 5)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_ForceCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? gizmo->object : NULL;
}

f32 __cdecl Condition_ForceCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && GizForce_Complete((GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ForceFinishedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && ForceAnimationDonePc((GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_BuildItCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, gizbuildit_gizmotype_id, name);
}

f32 __cdecl Condition_BuildItCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    GIZMO_PC *gizmo = (GIZMO_PC *)argument;
    GIZBUILDIT_PC *buildit;
    if (gizmo != NULL) {
        buildit = (GIZBUILDIT_PC *)gizmo->object;
        if (buildit != NULL && buildit->build_state == 2)
            return 1.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_BlowupBlownupInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, name);
}

f32 __cdecl Condition_BlowupBlownupPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_IsAliveInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    (void)script;
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_IsAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    APIOBJECT_PC *object = (APIOBJECT_PC *)argument;
    AIGROUP_PC *group;
    GAMEOBJECT *member;
    i32 index, count;
    (void)system;
    (void)process;
    (void)packet;
    (void)name;
    if (object == NULL)
        return 0.0f;
    group = object->ai->group;
    if (group != NULL) {
        count = group->member_count;
        for (index = 0; index < count; ++index) {
            member = (GAMEOBJECT *)group->members[index];
            if (member != NULL && (member->flags_1fc_word & 1u) != 0 && (member->flags_1fc_word & 0x1000u) != 0 &&
                member->dead == 0)
                return 1.0f;
        }
        return 0.0f;
    }
    member = (GAMEOBJECT *)object;
    if ((member->flags_1fc_word & 0x1000u) != 0 && member->dead == 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_AIOverrideControlInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_AIOverrideControlPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner;
    }
    if ((object->flags_1fc_word & 0x100) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_IsOnScreenInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_IsOnScreenPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner;
    }
    if (object->model_draw_result != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_OffScreenTimerInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    if (name != NULL && GetNamedGameObjectCallback != NULL) {
        object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}

f32 __cdecl Condition_OffScreenTimerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL)
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object != NULL)
        return object->field_f10;
    return 0.0f;
}

f32 __cdecl Condition_PlayerOnGroundPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (player != NULL && (player->field_281 != 0 || player->field_282 != 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OnGroundPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (object != NULL && (object->field_281 != 0 || object->field_282 != 0))
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_BeenAlertedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        if (packet->owner->objptr->alert_target != NULL)
            return 1.0f;
        return 0.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_OnObjectInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    NUSPECIALHANDLE special;
    if (CurrentTerrain != NULL) {
        if (NuSpecialFind(WORLD->current_gscn, &special, name, 1) != 0)
            return (void *)FindPlatInst(NuSpecialGetInstanceix(&special));
    }
    return (void *)-1;
}

f32 __cdecl Condition_PlayerOnObjectPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    i32 platform = (i32)argument;
    if (player != NULL && platform != -1 && (player->field_281 != 0 || player->field_282 != 0) &&
        player->supporting_platform_id == platform) {
        NUMTX *transform =
            ((AI_ON_OBJECT_TERRAIN_PC *)CurrentTerrain)->platforms[player->supporting_platform_id].scene_object;
        if (player->position.y >= transform->m31)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerOnObjectPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    i32 platform = (i32)argument;

    if (player != NULL && platform != -1 && (player->field_281 != 0 || player->field_282 != 0) &&
        player->supporting_platform_id == platform) {
        NUMTX *transform =
            ((AI_ON_OBJECT_TERRAIN_PC *)CurrentTerrain)->platforms[player->supporting_platform_id].scene_object;
        if (player->position.y >= transform->m31)
            return 1.0f;
    }
    if (player2 != NULL && platform != -1 && (player2->field_281 != 0 || player2->field_282 != 0) &&
        player2->supporting_platform_id == platform) {
        NUMTX *transform =
            ((AI_ON_OBJECT_TERRAIN_PC *)CurrentTerrain)->platforms[player2->supporting_platform_id].scene_object;
        if (player2->position.y >= transform->m31)
            return 1.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_EitherPlayerLocatorRangeXZInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AIPathFindLocator(system, name) : NULL;
}

f32 __cdecl Condition_EitherPlayerLocatorRangeXZ(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                 char *unused3, void *argument) {
    NUVEC difference;
    if (packet != NULL && packet->owner != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL) {
            locator = processor->locator;
        }
        if (locator != NULL) {
            f32 first_distance = 1.0e9f;
            f32 second_distance = 1.0e9f;
            if (player != NULL) {
                first_distance = NuVecXZDist(&player->position, &locator->position, &difference);
            }
            if (player2 != NULL) {
                second_distance = NuVecXZDist(&player2->position, &locator->position, &difference);
            }
            return first_distance < second_distance ? first_distance : second_distance;
        }
    }
    return 1.0e9f;
}

f32 __cdecl Condition_OnObjectPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        i32 platform = (i32)argument;
        if (object != NULL && platform != -1 && (object->field_281 != 0 || object->field_282 != 0) &&
            object->supporting_platform_id == platform) {
            NUMTX *transform =
                ((AI_ON_OBJECT_TERRAIN_PC *)CurrentTerrain)->platforms[object->supporting_platform_id].scene_object;
            if (object->position.y >= transform->m31)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_OnSameObjectAsPlayerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && player != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        if ((object->field_281 != 0 || object->field_282 != 0) && (player->field_281 != 0 || player->field_282 != 0) &&
            object->supporting_platform_id != -1 && object->supporting_platform_id == player->supporting_platform_id)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_SpawnCountPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL)
        return (f32)packet->respawn_count;
    return 0.0f;
}

f32 __cdecl Condition_BehindCameraPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        if (object != NULL) {
            NUVEC difference;
            NuVecSub(&difference, &object->collision_position, &GameCam->position);
            if (GameCam->dir.x * difference.x + GameCam->dir.y * difference.y + GameCam->dir.z * difference.z < 0.0)
                return 1.0f;
        }
    }
    return 0.0f;
}

void *__cdecl Condition_LocatorOnScreenInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return AIPathFindLocator(system, name);
}

f32 __cdecl Condition_LocatorOnScreenPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    if (locator == NULL)
        locator = process->locator;
    if (locator != NULL && NuCameraClipTestSphere(&locator->position, 0.0f, &NuMtxSetIdentityTemplate) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_BlockingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    GAMEOBJECT *object;
    u64 flags;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (object != NULL) {
            flags = U64_PAIR(&CInfo[(i8)object->character_context].flags);
            if ((flags & 0x04000000u) != 0 ||
                ((flags & 0x08000000u) != 0 && (object->context_variant_flags & 0x20000u) != 0))
                return 1.0f;
        }
    }
    return 0.0f;
}

void *__cdecl Condition_BeenHitInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    if (name != NULL && GetNamedGameObjectCallback != NULL) {
        object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}

f32 __cdecl Condition_BeenHitPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL && packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (object != NULL) {
        if (object->vehicle_hit_timer > 0.0f || (object->portal_flags & 1) != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_HoverPhasePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return (f32)packet->owner->objptr->jump_variant_state;
    return 0.0f;
}

void *__cdecl Condition_HitPointsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    (void)script;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_HitPointsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    (void)system;
    (void)process;
    (void)name;
    if (object == NULL) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
    }
    if (object != NULL)
        return (f32)(i32)(i8)object->current_hp;
    return 0.0f;
}

f32 __cdecl Condition_CollidingWithOpponentPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    GAMEOBJECT *opponent, *owner;
    if (packet != NULL && (opponent = packet->opponent) != NULL && (owner = packet->owner) != NULL &&
        (U64_PAIR(opponent->collision_identity_mask) & U64_PAIR(owner->collision_contact_mask)) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_CollidingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && U64_PAIR(packet->owner->collision_contact_mask) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_XPosInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_XPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL && packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (object != NULL)
        return object->collision_position.x;
    return 0.0f;
}

f32 __cdecl Condition_YPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL && packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (object != NULL)
        return object->collision_position.y;
    return 0.0f;
}

f32 __cdecl Condition_ZPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL && packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (object != NULL)
        return object->collision_position.z;
    return 0.0f;
}

void *__cdecl Condition_IsSetAliveInit(AISYS_PC *unused0, char *arg, AISCRIPT_PC *unused2) {
    u32 set;
    if (arg == NULL)
        return NULL;
    if (NuStrICmp(arg, AIConditionMySetNamePc) == 0)
        return (void *)((intptr_t)-1);
    set = NuAToI(arg);
    if (set - 1 > 15)
        set = 0;
    return (void *)((intptr_t)set);
}

f32 __cdecl Condition_IsSetAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    i32 set = (i32)argument;
    if (set == -1)
        set = process->creature_set;
    if (set != 0 && aicreature_sets_alive[set - 1] != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_NumInSetAlive(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    i32 set = (i32)argument;
    if (set == -1)
        set = process->creature_set;
    if (set != 0)
        return (f32)aicreature_sets_alive[set - 1];
    return 0.0f;
}

f32 __cdecl Condition_ContextPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return (f32)(i32)packet->owner->objptr->character_context;
    return -1.0f;
}

void *__cdecl Condition_InContextInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (NuStrICmp(name, "DEACTIVATED") == 0)
        return (void *)23;
    if (NuStrICmp(name, "FORCEDBACK") == 0)
        return (void *)34;
    if (NuStrICmp(name, "GRAB") == 0)
        return (void *)56;
    if (NuStrICmp(name, "EAT") == 0)
        return (void *)63;
    if (NuStrICmp(name, "FORCEPUSHED") == 0)
        return (void *)28;
    if (NuStrICmp(name, "FORCEPUSH") == 0)
        return (void *)27;
    if (NuStrICmp(name, "GETIN") == 0)
        return (void *)60;
    if (NuStrICmp(name, "BALLOONING") == 0)
        return (void *)93;
    if (NuStrICmp(name, "STUNNED") == 0)
        return (void *)90;
    if (NuStrICmp(name, "FLOAT") == 0)
        return (void *)75;
    return (void *)(NuStrICmp(name, "GRAPPLE") == 0 ? 70 : 100);
}

f32 __cdecl Condition_InContextPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        packet->owner->objptr->character_context == (i32)argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentContextPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->opponent != NULL && packet->opponent->objptr->character_context == (i32)argument)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_OnDynamicGrappleInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_OnDynamicGrapplePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner->objptr;
    }
    if (object != NULL && object->character_context == 0x46) {
        GRAPPLE_MOTION_PC *grapple = (GRAPPLE_MOTION_PC *)object->grapple;
        if (grapple != NULL && grapple->has_terrain_platform != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_Player2ActivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (player2 != NULL)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_NumBaddiesInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return NULL;
}

f32 __cdecl Condition_NumBaddiesPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    i32 count = 0;
    i32 remaining = HIGHGAMEOBJECT;
    GAMEOBJECT *object = Obj;
    while (remaining > 0) {
        u32 flags = object->flags_1fc_word;
        if ((flags & 1) != 0 && (flags & 0x1000) != 0 && (object->flags_1f8 & 1) != 0)
            count++;
        object++;
        remaining--;
    }
    return (f32)count;
}

void *__cdecl Condition_NumForceObjectsInit(AISYS_PC *system, char *name, AISCRIPT_PC *unused2) {
    i32 flags = 0;
    if (name != NULL && system != NULL) {
        if (NuStrIStr(name, "throwable") != NULL)
            flags = 1;
        if (NuStrIStr(name, "inrange") != NULL)
            flags |= 2;
    }
    return (void *)((intptr_t)flags);
}

/* Retail returns zero unconditionally. */
f32 __cdecl Condition_NumForceObjectsPc(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2,
                                        char *unused3, void *unused4) {
    return 0.0f;
}

void *__cdecl Condition_BeenToLevelInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL && WORLD->area != NULL) {
        for (index = 0; index < WORLD->area->level_count; ++index) {
            if (NuStrICmp(name, LDataList[WORLD->area->levels[index]].name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}

f32 __cdecl Condition_BeenToLevelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    i32 index = (i32)argument;
    if (index != -1)
        return (f32)(((AI_BEEN_TO_LEVEL_PROGRESS_PC *)LevelProgressData)[index].visited_flags & 1u);
    return 0.0f;
}

void *__cdecl Condition_LastLevelInit(AISYS_PC *system, char *name, AISCRIPT_PC *unused2) {
    if (name != NULL && system != NULL && WORLD->area != NULL) {
        i32 index;
        for (index = 0; index < LEVELCOUNT; ++index) {
            if (NuStrICmp(name, LDataList[index].name) == 0) {
                return (void *)((i32)index);
            }
        }
    }
    return (void *)((i32)-1);
}

f32 __cdecl Condition_LastLevelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (LastLData != NULL && (i32)LastLData->index == (i32)argument)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_MessageInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    void *message = NULL;
    if (name != NULL && system != NULL && gizaimessagesys != NULL)
        message = CheckGizAIMessage(gizaimessagesys, name, NULL);
    return message;
}

f32 __cdecl Condition_MessagePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    GIZAIMESSAGE *message = (GIZAIMESSAGE *)argument;
    if (message != NULL)
        return message->value;
    return 0.0f;
}

void *__cdecl Condition_ScriptParamInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL) {
        for (index = 0; index < 4; ++index) {
            if (NuStrICmp(script->params[index].name, name) == 0)
                return (void *)index;
        }
        return (void *)NuAToI(name);
    }
    return (void *)-1;
}

f32 __cdecl Condition_ScriptParamPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    i32 index = (i32)argument;
    if (index >= 0)
        return process->params[index];
    return 0.0f;
}

void *__cdecl Condition_AnimationFinishedInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    NUSPECIALHANDLE special;
    NuSpecialFind(WORLD->current_gscn, &special, name, 1);
    if (NuSpecialExistsFn(&special) != 0)
        return NuSpecialGetInstAnim(&special);
    return NULL;
}

f32 __cdecl Condition_AnimationFinishedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    NUINSTANIM *instance = (NUINSTANIM *)argument;
    void *animation;
    f64 time;
    if (instance != NULL && (instance->flags & 1) == 0) {
        animation = WORLD->current_gscn->instance_animation_data[instance->animation_index];
        if (animation != NULL) {
            time = (f64)instance->ltime;
            {
                if ((f64)NuAnimEndFrameOld(animation) <= time)
                    return 1.0f;
                return 0.0f;
            }
        }
    }
    return 0.0f;
}

void *__cdecl Condition_RigidAnimFrameInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    NUSPECIALHANDLE special;
    NuSpecialFind(WORLD->current_gscn, &special, name, 1);
    if (NuSpecialExistsFn(&special))
        return NuSpecialGetInstAnim(&special);
    return NULL;
}

f32 __cdecl Condition_RigidAnimFramePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (argument != NULL)
        return ((NUINSTANIM *)argument)->ltime;
    return 1.0f;
}

void *__cdecl Condition_CutSceneStartedInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return CutScene_Find(WORLD->cutscene_sys, name);
}

f32 __cdecl Condition_CutSceneStartedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    CUTINFO *cutscene = (CUTINFO *)argument;
    if (cutscene != NULL && cutscene->instance != NULL &&
        (((CONDITION_CUT_INSTANCE_READER_PC *)cutscene->instance)->flags & 2) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CutSceneFinishedInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return CutScene_Find(WORLD->cutscene_sys, name);
}

f32 __cdecl Condition_CutSceneFinishedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    CUTINFO *cutscene = (CUTINFO *)argument;
    if (cutscene != NULL && cutscene->instance != NULL &&
        instNuGCutSceneIsFinished((INSTNUGCUTSCENE *)cutscene->instance) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CutSceneExistsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return CutScene_Find(WORLD->cutscene_sys, name);
}

f32 __cdecl Condition_CutSceneExistsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (argument != NULL)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CutScenePlayingInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return CutScene_Find(WORLD->cutscene_sys, name);
}

f32 __cdecl Condition_CutScenePlayingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    return 0.0f;
}

void *__cdecl Condition_PlayerInSockInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return FindSock(WORLD->sock_sys, name);
}

f32 __cdecl Condition_PlayerInSockPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL && WORLD->sock_sys != NULL && &WORLD->sock_sys->socks[(i8)player->sock_index] == argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_SockDistanceToPlayer(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                           char *unused3, void *unused4) {
    f32 result = 0.0f;
    if (packet != NULL && packet->owner != NULL && player != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        GAMEOBJECT *target = player;
        if (target->sock_position.sock_index != -1 &&
            target->sock_position.sock_index == object->sock_position.sock_index) {
            f64 distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
            result = (f32)(distance - MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position));
        }
    }
    return result;
}

f32 __cdecl Condition_SockDistanceToOpponent(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                             char *unused3, void *unused4) {
    f32 result = 0.0f;
    if (packet != NULL && packet->owner != NULL && packet->opponent != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        GAMEOBJECT *target = packet->opponent->objptr;
        if (target->sock_position.sock_index != -1 &&
            target->sock_position.sock_index == object->sock_position.sock_index) {
            f64 distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
            result = (f32)(distance - MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position));
        }
    }
    return result;
}

f32 __cdecl Condition_SockXDistanceToPlayer(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                            char *unused3, void *unused4) {
    f32 result = 0.0f;
    NUVEC player_offset, object_offset;
    if (packet != NULL && packet->owner != NULL && player != NULL) {
        GAMEOBJECT *object = packet->owner->objptr;
        if (player->sock_position.sock_index != -1 &&
            player->sock_position.sock_index == object->sock_position.sock_index) {
            NuVecSub(&player_offset, &player->position, &player->sock_position.midpoint);
            NuVecRotateY(&player_offset, &player_offset, -player->sock_position.heading);
            NuVecSub(&object_offset, &object->position, &object->sock_position.midpoint);
            NuVecRotateY(&object_offset, &object_offset, -object->sock_position.heading);
            result = player_offset.x - object_offset.x;
        }
    }
    return result;
}

f32 __cdecl Condition_PlayerDistanceAlongSockPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                char *name, void *argument) {
    if (player != NULL)
        return player->sock_position.distance;
    return 0.0f;
}

f32 __cdecl Condition_FurthestPlayerDistanceAlongSockPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process,
                                                        AIPACKET_PC *packet, char *name, void *argument) {
    if (player != NULL) {
        if (player2 != NULL && player2->sock_position.distance > player->sock_position.distance)
            return player2->sock_position.distance;
        return player->sock_position.distance;
    }
    return 0.0f;
}

f32 __cdecl Condition_FinishedSplinePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        if (object->movement_spline_position.spline != NULL) {
            if (object->movement_spline_position.finished != 0)
                return 1.0f;
            return 0.0f;
        }
        return 1.0f;
    }
    return -1.0f;
}

f32 __cdecl Condition_CurrentHintIdPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    return (f32)Hint_CurrentId();
}

void *__cdecl Condition_HintAvailableInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return (void *)NuAToI(name);
    return NULL;
}

f32 __cdecl Condition_HintAvailablePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && Hint_isAvailable((i32)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_HintCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL && Hint_isCompleteByIdPc((i32)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_FreeplayPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return (f32)FreePlay;
}

f32 __cdecl Condition_IndyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    return (f32)StartupGameFlag0;
}

f32 __cdecl Condition_MissionModePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (Mission_Active(NULL) != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_MissionWonPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (MissionSys != NULL && MissionSys->state == 2)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ChallengeModePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (ChallengeMode != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PSPPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                            void *argument) {
    return 0.0f;
}

f32 __cdecl Condition_BoltsDontGetDeflectedBackPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->objptr->field_ef0 & 8) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_CheatProgressPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    return 0.0f;
}

f32 __cdecl Condition_BigJumpCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr->character_context == 0x1f)
        return 0.0f;
    return 1.0f;
}

void *__cdecl Condition_RespawnLocatorIsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

f32 __cdecl Condition_RespawnLocatorIsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (argument != NULL && packet != NULL && packet->respawn_locator == argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_InMiniCutPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (MiniCutCam != 0 || (name != NULL && ObstacleCamSpl != NULL))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_MaulShouldRunAway(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    f32 dx;
    f32 dz;
    NUANG owner_angle;
    GAMEOBJECT *player;
    i32 i;
    if (packet->owner != NULL) {
        dx = (f32)((f64)packet->owner->collision_position.x - 5.5);
        dz = (f32)((f64)packet->owner->collision_position.z - (f64)3.65f);
        owner_angle = NuAtan2D(dx, dz);
        for (i = 0; i < 2; i++) {
            player = Player[i];
            if (player != NULL && (player->flags_1fc_word & 0x1001) == 0x1001) {
                dx = (f32)((f64)player->collision_position.x - 5.5);
                dz = (f32)((f64)player->collision_position.z - (f64)3.65f);
                if ((f64)dx * dx + (f64)dz * dz < 25.0) {
                    if (NuAngSub(NuAtan2D(dx, dz), owner_angle) < 0xe38)
                        return 1.0f;
                }
            }
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_DropBackInTimerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    return drop_back_in_timer;
}

f32 __cdecl Condition_PartyUnderCoverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    (void)system;
    (void)process;
    (void)packet;
    (void)name;
    (void)argument;
    if (party_under_cover != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_NumBaddiesThatCanSeePlayersPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    return (f32)nbaddies_can_see_players;
}

f32 __cdecl Condition_HelpWithTriggersPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    AITRIGGERSETSYS_s *triggers = ((AI_GLOBAL_TRIGGER_WORLD_PC *)WORLD)->trigger_system;
    u8 index;
    if (triggers != NULL && packet != NULL && packet->owner != NULL) {
        index = packet->owner->hostility_group;
        if (triggers->field_0x42c0[index] != -1 && triggers->sets[triggers->field_0x4280[index]].field_0x20e != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_PartyContainsDroidsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->flags_ef3 & 1) == 0 && (object->character_data->model_flags & 0x10) != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_CannotReachDestinationPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    if (packet != NULL && (packet->flags & 0x400000) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_EitherPlayerPushingSpinnerInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizSpinner_FindBySpecialName(WORLD, name);
}

f32 __cdecl Condition_EitherPlayerPushingSpinnerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                   char *name, void *argument) {
    if (argument != NULL) {
        if (player != NULL && player->character_context == 0x28 && player->grapple == argument)
            return 1.0f;
        if (player2 != NULL && player2->character_context == 0x28 && player2->grapple == argument)
            return 1.0f;
    }
    return 0.0f;
}

void *__cdecl Condition_CharacterRangeInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_CharacterRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (packet != NULL && packet->owner != NULL && object != NULL) {
        NUVEC difference;
        return NuVecDist(&object->position, &packet->owner->position, &difference);
    }
    return 1000000000.0f;
}

f32 __cdecl Condition_BeenSpawnedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    // MATCH-EXCEPTION: field_280 is a signed char in the original (compared with -1); saga.h declares u8
    if (packet != NULL && packet->owner != NULL && (i8)packet->owner->field_280 == -1 && packet->creature_index == 0xff)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_TakeOverTargetInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}

f32 __cdecl Condition_TakeOverTargetInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        AIAREA_PC *area = (AIAREA_PC *)argument;
        GAMEOBJECT *target = NULL;
        i32 index;
        u64 mask;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_READER_PC *)process)->trigger_area;
        index = (i32)((u32)area - (u32)WORLD->ai_sys->areas) >> 6;
        if (packet->owner->objptr != NULL)
            target = packet->owner->objptr->takeover_target;
        if (system->player_1 != NULL && target != NULL) {
            /* The retail compiler helper consumes CL, including its out-of-range zero cases. */
            mask = (u64)1 << index;
            if ((((CONDITION_TRIGGER_PLAYER_READER_PC *)target)->membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerInMyTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    AIAREA_PC *area;
    GAMEOBJECT *object;
    i32 index;
    i64 mask;
    if (system != NULL) {
        area = ((CONDITION_TRIGGER_PROCESS_READER_PC *)process)->trigger_area;
        if (area != NULL) {
            if (system->player_1 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_1;
                index = (i32)((u32)area - (u32)area->system->areas) >> 6;
                /* Retail wraps the 32-bit shift and sign-extends bit 31. */
                mask = (i32)(1u << ((u32)index & 31));
                if ((((CONDITION_TRIGGER_PLAYER_READER_PC *)object)->membership & mask) != 0)
                    return 1.0f;
            }
            if (system->player_2 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_2;
                index = (i32)((u32)area - (u32)area->system->areas) >> 6;
                mask = (i32)(1u << ((u32)index & 31));
                if ((((CONDITION_TRIGGER_PLAYER_READER_PC *)object)->membership & mask) != 0)
                    return 1.0f;
            }
        }
    }
    return 0.0f;
}

void *__cdecl Condition_TakenOverInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_TakenOver(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char *name,
                                void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (name != NULL) {
            if (NuStrICmp(name, AIGameOpponentName) == 0) {
                GAMEOBJECT *opponent = packet->owner->objptr->ai_packet.opponent;
                if (opponent != NULL)
                    object = opponent->objptr;
            } else if (NuStrICmp(name, AIConditionTakeoverTargetNamePc) == 0) {
                object = packet->owner->objptr;
                if (object != NULL)
                    object = object->takeover_target;
            }
        } else if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
    }
    if (object != NULL && object->takeover_source != NULL && object->character_context != 0x3b)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (player != NULL && player->takeover_source != NULL && player->character_context != 0x3b)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    GAMEOBJECT *object = player;
    if (object != NULL && object->takeover_source != NULL && object->character_context != 0x3b)
        return 1.0f;
    object = player2;
    if (object != NULL && object->takeover_source != NULL && object->character_context != 0x3b)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_BeenTakenOverInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_BeenTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        GAMEOBJECT *owner = packet->owner;
        if (owner == NULL)
            return 0.0f;
        object = owner->objptr;
        if (object == NULL)
            return 0.0f;
    }
    if (object->takeover_source != NULL && object->character_context == 0x3b)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_OnSpeederBikeInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_OnSpeederBikePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (object != NULL && object->rider != NULL && object->character_context == 0x3b &&
        object->rider->id == (u16)id_SPEEDERBIKE)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_UnderPlayerControlInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_UnderPlayerControlPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object != NULL && (object->flags_1fc_word & 0x80) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CharacterExistsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GAMEOBJECT *object = NULL;
    if (name != NULL && system != NULL)
        object = GetNamedGameObject(system, name);
    return object;
}

f32 __cdecl Condition_CharacterExistsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (argument != NULL)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CharacterTypeExistsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL) {
        for (index = 0; index < CHARCOUNT; ++index) {
            if (NuStrICmp(CDataList[index].name, name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}

f32 __cdecl Condition_CharacterTypeExistsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    i32 index;
    if (argument != NULL) {
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            if ((i16)Obj[index].id == (i32)argument)
                return 1.0f;
        }
        return 0.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_LastAttackerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    GAMEOBJECT *object;
    NUVEC difference;
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet == NULL || packet->owner == NULL)
        return AIPositionUnsetPc;
    object = packet->owner->objptr;
    if (object->last_attacker == NULL)
        return AIPositionUnsetPc;
    return NuVecDist(&object->position, &object->last_attacker->position, &difference);
}

f32 __cdecl Condition_LastAttackerIsActivePlayerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                   char *name, void *argument) {
    GAMEOBJECT *object;
    GAMEOBJECT *attacker;
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet == NULL || packet->owner == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    attacker = object->last_attacker;
    if (attacker != NULL && (attacker->flags_1fc & 0x80) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_GotLocatorInSetInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return AIPathFindLocatorSet(system, name);
}

f32 __cdecl Condition_GotLocatorInSetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    AILOCATORSET_PC *set = (AILOCATORSET_PC *)argument;
    if (set != NULL) {
        AILOCATOR_PC *locator = process->locator;
        if (locator != NULL) {
            u8 locator_index = (u8)(locator - system->locators);
            i32 count = set->locator_count;
            i32 index;
            for (index = 0; index < count; ++index)
                if (locator_index == set->locator_entries[index])
                    return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_GotOpponentLOSPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object, *opponent;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    if (object->character_context == 0x1b)
        opponent = object->force_target;
    else
        opponent = packet->opponent;
    /* hostility_group is below 64 for every live object; __allshl yields 0 otherwise, as in retail. */
    if (opponent != NULL && (WORLD->object_sys->hostility_masks[packet->owner->hostility_group] &
                             ((u64)1 << opponent->hostility_group)) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EmptyTakeOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    i32 character = -1;
    i32 index;
    GAMEOBJECT *object;
    if (name != NULL && system != NULL) {
        for (index = 0; index < CHARCOUNT && character == -1; ++index) {
            if (NuStrICmp(CDataList[index].name, name) == 0)
                character = index;
        }
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            object = &Obj[index];
            if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
                (object->flags_1f8 & 0x400) != 0 && (i16)object->id == character) {
                if (object->takeover_target == NULL || object->takeover_source == NULL ||
                    object->takeover_source->character_context != 0x3b)
                    return 1.0f;
            }
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_HasTakeOverTargetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        packet->owner->objptr->takeover_target != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_TakeOverRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        GAMEOBJECT *target = packet->owner->objptr->takeover_target;
        if (target != NULL)
            return NuVecDist(&target->position, &packet->owner->position, NULL);
    }
    return 1000000000.0f;
}

f32 __cdecl Condition_GotVictimPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_e24 & 1) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ScreenWipePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (FadeSys.fade > 0.0f)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_HeadTurnRestrictedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return (f32)(packet->owner->objptr->flags_ef0 >> 16 & 1);
    return 0.0f;
}

f32 __cdecl Condition_ShopActive(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    if (GetMenuID() == 13)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_SideInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL) {
        if (NuStrICmp(name, AITransferLiteral7576c0) == 0 || NuStrICmp(name, AITransferLiteral7576b8) == 0)
            return (void *)-1;
        if (NuStrICmp(name, AITransferLiteral7576b0) == 0 || NuStrICmp(name, AITransferLiteral7576a8) == 0)
            return (void *)1;
        if (NuStrICmp(name, AITransferLiteral75769c) == 0 || NuStrICmp(name, AITransferLiteral75768c) == 0)
            return (void *)2;
    }
    return NULL;
}

f32 __cdecl Condition_Side(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                           void *argument) {
    i32 side;
    if (packet == NULL || packet->owner == NULL)
        return 0.0f;
    side = (i32)argument;
    if (side == 2) {
        if ((packet->owner->flags_1f8 & 0x10000) != 0)
            return 1.0f;
    } else if (side == 1) {
        if ((packet->owner->flags_1f8 & 5) == 0)
            return 1.0f;
    } else if (side == -1) {
        if ((packet->owner->flags_1f8 & 1) != 0)
            return 1.0f;
    } else if (side == 0) {
        if ((packet->owner->flags_1f8 & 4) != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_NearestPartyRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    f32 nearest = 1000000000.0f;
    if (packet != NULL && packet->owner != NULL && system != NULL) {
        NUVEC difference;
        i32 index;
        for (index = 0; index < 8; ++index) {
            GAMEOBJECT *member = (GAMEOBJECT *)Player[index];
            if (member != NULL && (member->flags_1fc_word & 1u) != 0 && (member->flags_1fc_word & 0x1000u) != 0) {
                f32 distance = NuVecDistSqr(&member->position, &packet->owner->position, &difference);
                if (distance < nearest)
                    nearest = distance;
            }
        }
        if ((f64)nearest != 1000000000.0)
            nearest = NuFsqrt(nearest);
    }
    return nearest;
}

f32 __cdecl Condition_NearestPartyXZRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    f32 nearest = 1000000000.0f;
    if (packet != NULL && packet->owner != NULL && system != NULL) {
        NUVEC difference;
        i32 index;
        for (index = 0; index < 8; ++index) {
            GAMEOBJECT *member = (GAMEOBJECT *)Player[index];
            if (member != NULL && (member->flags_1fc_word & 1u) != 0 && (member->flags_1fc_word & 0x1000u) != 0) {
                f32 distance = NuVecXZDistSqr(&member->position, &packet->owner->position, &difference);
                if (distance < nearest)
                    nearest = distance;
            }
        }
        if ((f64)nearest != 1000000000.0)
            nearest = NuFsqrt(nearest);
    }
    return nearest;
}

f32 __cdecl Condition_OpponentToPlayerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL && packet->opponent != NULL && system != NULL &&
        system->player_1 != NULL)
        return NuVecDist(&system->player_1->position, &packet->opponent->position, &delta);
    return 1000000000.0f;
}

f32 __cdecl Condition_OpponentPathPosRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->opponent != NULL) {
        AIPACKET_PC *opponent_packet = ((APIOBJECT_PC *)packet->opponent)->ai;
        if (opponent_packet != NULL) {
            NUVEC difference;
            return NuVecDist(&packet->owner->position, &opponent_packet->last_path_position, &difference);
        }
    }
    return 1000000000.0f;
}

void *__cdecl Condition_GizmoOutput0InitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return GizmoFindByName(WORLD->gizmo_sys, -1, name);
    return NULL;
}

f32 __cdecl Condition_GizmoOutput0Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL)
        return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1);
    return 0.0f;
}

f32 __cdecl Condition_GizmoOutput1Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL)
        return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1);
    return 0.0f;
}

f32 __cdecl Condition_GizmoOutput2Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL)
        return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 2, 1);
    return 0.0f;
}

f32 __cdecl Condition_GizmoOutput3Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL)
        return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 3, 1);
    return 0.0f;
}

void *__cdecl Condition_GizmoVisibilityInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return GizmoFindByName(WORLD->gizmo_sys, -1, name);
    return NULL;
}

f32 __cdecl Condition_GizmoVisibilityPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (argument != NULL)
        return (f32)GizmoGetVisibility(WORLD->gizmo_sys, argument);
    return 0.0f;
}

void *__cdecl Condition_FlowBoxCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return FlowBoxFindByName(WORLD->giz_flow, name);
    return NULL;
}

f32 __cdecl Condition_FlowBoxCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    FLOWBOX_PC *box = (FLOWBOX_PC *)argument;
    if (box != NULL)
        return (box->state_flags_low & 2) != 0 ? 1.0 : 0.0;
    return 0.0f;
}

void *Condition_AngleAboutMyLocatorToPlayerInit(AISYS_PC *unused0, char *name, AISCRIPT_PC *unused2) {
    return (void *)(intptr_t)(name != NULL && NuStrICmp(name, "player1") == 0 ? 1 : -1);
}

f32 __cdecl Condition_AngleAboutMyLocatorToPlayer(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                  char *unused3, void *argument) {
    f32 result = 0.0f;
    if (argument != NULL && processor->locator != NULL && packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->owner;
        GAMEOBJECT *target = NULL;
        if ((intptr_t)argument == -1)
            target = player;
        else if ((intptr_t)argument == 1)
            target = (GAMEOBJECT *)Player[1];
        if (target != NULL) {
            NUANG object_angle = NuAtan2D(object->position.x - processor->locator->position.x,
                                          object->position.z - processor->locator->position.z);
            NUANG target_angle = NuAtan2D(target->position.x - processor->locator->position.x,
                                          target->position.z - processor->locator->position.z);
            result = (f32)(NuAngSub(target_angle, object_angle));
        }
    }
    return result;
}

f32 __cdecl Condition_AnimSpeedMulPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return packet->owner->objptr->animation_speed_multiplier;
    return 1.0f;
}

void *__cdecl Condition_PickupBeenTurnedOnInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return GizmoPickup_FindByName(WORLD, name);
    return NULL;
}

f32 __cdecl Condition_PickupBeenTurnedOnPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (argument != NULL)
        return (f32)GizmoPickup_BeenTurnedOn((GIZMOPICKUP_PC *)argument);
    return 0.0f;
}

f32 __cdecl Condition_CanHearRadioPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (radios_playing != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_BeingTowedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && (packet->owner->objptr->flags_ef4 & 0x200) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_RaceLapPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    return (f32)Lap;
}

f32 __cdecl Condition_MusicOn(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                              void *argument) {
    if (Game.options_save.music_enabled != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_CharacterLoadedInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return (void *)CharIDFromName(name);
    return NULL;
}

f32 __cdecl Condition_CharacterLoaded(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && APICharacterLoaded((i32)argument) != NULL)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_AreaCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? Area_FindByName(name, NULL) : NULL;
}

f32 __cdecl Condition_AreaCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    AREA_DATA_PC *area = (AREA_DATA_PC *)argument;
    if (area != NULL && Game.area_save[area->index].area_complete == 1)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ShouldAttackOpponentPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    GAMEOBJECT *object;
    GAMEOBJECT *opponent;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    opponent = object->ai_packet.opponent;
    if (opponent != NULL && opponent->objptr != NULL && (object->ai_packet.flags & 0x800) != 0 &&
        object->ai_packet.opponent_metric < 1.0f &&
        (opponent->objptr->id == (u16)id_BAT || opponent->objptr->id == (u16)id_SNAKE))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_InSwampPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        packet->owner->objptr->field_283 == 9)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_InSameTriggerAreaAsNearestPlayer(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1,
                                                       AIPACKET_PC *packet, char *unused3, void *unused4) {
    GAMEOBJECT *nearest = NULL;
    f32 distance = 1.0e9f;
    if (packet != NULL && packet->owner != NULL) {
        if (player != NULL) {
            distance = NuVecXZDistSqr(&player->position, &packet->owner->position, NULL);
            nearest = player;
        }
        if (player2 != NULL) {
            f32 second_distance = NuVecXZDistSqr(&player2->position, &packet->owner->position, NULL);
            if (distance > second_distance)
                nearest = player2;
        }
        if (nearest != NULL && ((packet->owner->ai_area_mask[0] & nearest->ai_area_mask[0]) |
                                (packet->owner->ai_area_mask[1] & nearest->ai_area_mask[1])) != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_NetworkGameOnGoingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    return 0.0f;
}

i32 __cdecl Action_SetTakeOverTarget(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    char *type_name;
    GAMEOBJECT *target;
    f32 nearest;
    i32 check_takeover;
    char *value;
    i32 type;
    GAMEOBJECT *candidate;
    NUVEC difference;
    f32 distance;
    GAMEOBJECT *previous;

    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    type_name = NULL;
    target = NULL;
    nearest = 1000000000.0f;
    check_takeover = 0;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7576f8);
        if (value != NULL)
            type_name = value + 5;
        else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL)
            target = GetNamedGameObject(system, value + 9);
        else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7576e0)) != NULL) {
            nearest = AIParamToFloat(processor, value + 9);
            nearest *= nearest;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576d0) != 0)
            check_takeover = 1;
    }
    if (target == NULL && type_name == NULL)
        return 1;
    type = -1;
    for (index = 0; index < CHARCOUNT && type == -1; ++index) {
        if (NuStrICmp(CDataList[index].name, type_name) == 0)
            type = index;
    }
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || (i16)candidate->id != type)
            continue;
        if (candidate->takeover_target != NULL && candidate->takeover_target != object)
            continue;
        if (candidate->takeover_source != NULL &&
            (candidate->takeover_source->character_context == 0x3b || candidate->character_context != 0x3b))
            continue;

        NuVecSub(&difference, &candidate->collision_position, &packet->owner->collision_position);
        distance = NuVecMagSqr(&difference);
        if (!(distance < nearest))
            continue;
        nearest = distance;
        target = candidate;
    }
    if (target == NULL) {
        object->takeover_target = NULL;
        return 1;
    }
    if (check_takeover &&
        (target->takeover_source != NULL || !(target->character_data->game_character->flags_090 & 0x40) ||
         (object->character_data->model_flags & 0x10)))
        return 1;
    previous = object->takeover_target;
    if (previous != NULL && previous->takeover_target != NULL && previous->takeover_target == object)
        previous->takeover_target = NULL;
    object->takeover_target = target;
    target->takeover_target = object;
    return 1;
}

i32 __cdecl Action_ClearTakeOverTarget(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    if (first_time) {
        object = packet->owner->objptr;
        if (object != NULL) {
            target = object->takeover_target;
            if (target != NULL) {
                object->takeover_target = NULL;
                target->takeover_target = NULL;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_Activate(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate;
    APIOBJECT_PC *found;
    i32 index, creature_set = 0;
    char *value;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) && candidate->ai_packet.creature_set == creature_set) {
                if (candidate->ai_packet.creature_index == 0xff) {
                    candidate->flags_1fc_word |= 0x1000;
                    AIScriptSetBaseScriptStateByName(&candidate->ai_packet.script_processor, "Base");
                } else
                    ResetAICreature(candidate, system);
                ++aicreature_sets_alive[creature_set - 1];
            }
        }
    } else if (object != NULL) {
        if (object->ai_packet.creature_index == 0xff) {
            object->flags_1fc_word |= 0x1000;
            AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "Base");
        } else
            ResetAICreature(object, system);
        if (object->ai_packet.creature_set != 0 && object->ai_packet.creature_set <= 16)
            ++aicreature_sets_alive[object->ai_packet.creature_set - 1];
    }
    return 1;
}

i32 __cdecl Action_DeActivate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 creature_set;
    char *value;
    i32 parsed_set;
    GAMEOBJECT *candidate;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    creature_set = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameSetName);
        if (value != NULL) {
            parsed_set = (i32)(AIParamToFloat(processor, value + 4));
            creature_set = (u32)(parsed_set) < 17 ? parsed_set : 0;
        }
    }

    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) == (1u | 0x1000u) &&
                candidate->ai_packet.creature_set == creature_set) {
                DeactivateGameObject(candidate);
            }
        }
    } else if (object != NULL) {
        DeactivateGameObject(object);
    }
    return 1;
}

i32 __cdecl Action_CanDefend(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 2u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAimAt(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef4 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->flags_ef4 &= (u32)(~1u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToKillTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->portal_flags |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToBolts(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x80000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x80000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_Respawnable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags = (object->portal_flags & ~0x200000u) | 0x100000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIPathOriginName) == 0) {
                object->portal_flags |= 0x200000u;
            } else if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x100000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanUseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 8;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoss(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x8000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x8000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetSide(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    char *value;
    GAMEOBJECT *object;
    i32 side;
    f32 range_squared;
    i32 type_ids[10];
    i32 type_count;
    i32 local_type;
    i32 global_type;
    u32 old_side;
    u32 model_flags;
    AICREATURE_PC *creature;
    f32 distance_squared;
    GAMEOBJECT *candidate;
    i32 type_matches;
    NUVEC offset;
    i32 index, type_index;

    (void)param_6;
    range_squared = 0.0f;
    distance_squared = 0.0f;
    side = 0;
    type_count = 0;
    object = NULL;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1fc & 1u) != 0 &&
            packet->owner->objptr != NULL) {
            object = packet->owner->objptr;
        }

        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral75773c) == 0) {
                side = 0;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576a8) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral7576b0) == 0) {
                side = -1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576b8) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral7576c0) == 0) {
                side = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral75768c) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral75769c) == 0) {
                side = 2;
            } else if (NuStrICmp(params[index], (char *)GizmoDefaultOutputName) == 0) {
                side = 3;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                    if (local_type != 0xff) {
                        global_type = LevelCharacterGlobalIDFn(local_type);
                        if (global_type != -1 && type_count < 10) {
                            type_ids[type_count++] = global_type;
                        }
                    }
                }
            } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                range_squared = AIParamToFloat(processor, value + 6);
                range_squared *= range_squared;
            }
        }

        if (type_count <= 0 && !(range_squared > 0.0f)) {
            if (object != NULL) {
                old_side = object->flags_1f8 & 0x10005;
                object->flags_1f8 &= 0xfffefffau;
                switch (side) {
                    case 0:
                        object->flags_1f8 |= 4;
                        break;
                    case 1:
                        object->flags_1f8 |= 1;
                        break;
                    case 2:
                        object->flags_1f8 |= 0x10000;
                        break;
                    case 3:
                        if ((object->flags_1f8 & 0x400u) != 0 && object->ai_packet.creature_index != 0xff) {
                            creature = sys->creatures + object->ai_packet.creature_index;
                            model_flags = apicharsys->char_data[creature->type].model_flags;
                            if ((model_flags & 0x200) != 0) {
                                object->flags_1f8 |= 4;
                            } else if ((model_flags & 4) != 0) {
                                object->flags_1f8 |= 1;
                            }
                        }
                        break;
                }

                object->ai_packet.capabilities &= ~(LEGO_AIPATHCNX_FORBADDIES | LEGO_AIPATHCNX_FORGOODIES);
                if ((object->flags_1f8 & 1) != 0) {
                    object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
                } else if ((object->flags_1f8 & 4) == 0) {
                    object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
                }

                if ((old_side & object->flags_1f8) == 0) {
                    object->ai_packet.flags &= ~0x800u;
                    object->ai_packet.opponent = NULL;
                    object->alert_target = NULL;
                    object->field_ec4 = 0.0f;
                }
            }
            return 1;
        }

        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            type_matches = 0;
            if ((candidate->flags_1fc_word & 1u) == 0 || (candidate->flags_1fc_word & 0x1000u) == 0 ||
                (candidate->flags_1f8 & 0x400u) == 0) {
                continue;
            }

            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == type_ids[type_index]) {
                    type_matches = 1;
                }
            }
            if (range_squared > 0.0f && object != NULL) {
                NuVecSub(&offset, &object->position, &candidate->position);
                distance_squared = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
            }
            if ((type_matches == 1 && (distance_squared < range_squared || range_squared == 0.0f)) ||
                (type_count == 0 && distance_squared < range_squared)) {
                old_side = candidate->flags_1f8 & 0x10005;
                candidate->flags_1f8 &= 0xfffefffau;
                switch (side) {
                    case 0:
                        candidate->flags_1f8 |= 4;
                        break;
                    case 1:
                        candidate->flags_1f8 |= 1;
                        break;
                    case 2:
                        candidate->flags_1f8 |= 0x10000;
                        break;
                    case 3:
                        if ((candidate->flags_1f8 & 0x400u) != 0 && candidate->ai_packet.creature_index != 0xff) {
                            creature = sys->creatures + candidate->ai_packet.creature_index;
                            model_flags = apicharsys->char_data[creature->type].model_flags;
                            if ((model_flags & 0x200) != 0) {
                                candidate->flags_1f8 |= 4;
                            } else if ((model_flags & 4) != 0) {
                                candidate->flags_1f8 |= 1;
                            }
                        }
                        break;
                }

                candidate->ai_packet.capabilities &= ~(LEGO_AIPATHCNX_FORBADDIES | LEGO_AIPATHCNX_FORGOODIES);
                if ((candidate->flags_1f8 & 1) != 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
                } else if ((candidate->flags_1f8 & 4) == 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
                }

                if ((old_side & candidate->flags_1f8) == 0) {
                    candidate->ai_packet.flags &= ~0x800u;
                    candidate->ai_packet.opponent = NULL;
                    candidate->alert_target = NULL;
                    candidate->field_ec4 = 0.0f;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetStateArea(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    char *state_name;
    i32 types[10];
    i32 type_count;
    f32 range_squared;
    char *value;
    f32 range;
    i32 local_type;
    i32 global_type;
    NUVEC origin;
    GAMEOBJECT *object;
    NUVEC difference;
    f32 distance_squared;
    i32 matching_type;
    AIPACKET_PC *object_packet;
    i32 index, object_index, type_index;
    (void)sys;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    state_name = NULL;

    type_count = 0;
    range_squared = 0.0f;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757724);
        if (value != NULL) {
            range = AIParamToFloat(processor, value + 6);
            range_squared = range * range;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757744)) != NULL) {
            state_name = value + 6;
        }
    }

    if (state_name == NULL || (type_count == 0 && !(range_squared > 0.0f))) {
        processor->next_state = AIStateFind(state_name, processor->script);
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        origin = packet->owner->objptr->collision_position;
    }

    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
        object = &Obj[object_index];
        if ((object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (object->flags_1f8 & 0x400) == 0) {
            continue;
        }

        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;

        matching_type = type_count == 0;
        for (type_index = 0; type_index < type_count; ++type_index) {
            if ((i16)object->id == types[type_index]) {
                matching_type = 1;
            }
        }
        if (matching_type != 0 && range_squared > distance_squared) {
            object_packet = ((APIOBJECT_PC *)object)->ai;
            object_packet->script_processor.next_state =
                AIStateFind(state_name, object_packet->script_processor.script);
        }
    }
    return 1;
}

i32 __cdecl Action_UpdateSockPos(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x4000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x4000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_Kill(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                        i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate, *excluded = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, respawn = 0, origin = 0, all_ai = 0, debris = 0, parts = 0, creature_set = 0, check_dead = 0;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if (NuStrIStr(params[index], "opponent") != NULL) {
            if (packet->opponent != NULL)
                object = ((APIOBJECT_PC *)packet->opponent)->owner;
        } else if (NuStrICmp(params[index], "respawn") == 0) {
            respawn = 1;
        } else if (NuStrICmp(params[index], "respawn_at_origin") == 0) {
            respawn = 1;
            origin = 1;
        } else if ((value = NuStrIStr(params[index], "all_ai_except")) != NULL) {
            excluded = GetNamedGameObject(system, value + 14);
            all_ai = 1;
        } else if (NuStrICmp(params[index], "all_ai") == 0) {
            all_ai = 1;
        } else if (NuStrICmp(params[index], "check_if_dead") == 0) {
            check_dead = 1;
        } else if (NuStrICmp(params[index], "debris") == 0) {
            debris = 1;
        } else if (NuStrICmp(params[index], "parts_on") == 0) {
            parts = 1;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (all_ai) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
                (candidate->flags_1f8 & 0x400) && candidate != excluded &&
                (!check_dead || (candidate->dead == 0 && candidate->field_1010 <= 0.0f))) {
                candidate->portal_flags =
                    (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
                if (parts)
                    KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
                if (debris)
                    KillGameObject(candidate, 2, 0);
                else
                    KillGameObject(candidate, 4, 0);
            }
        }
    } else if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
                candidate->ai_packet.creature_set == creature_set &&
                (!check_dead || (candidate->dead == 0 && candidate->field_1010 <= 0.0f))) {
                candidate->portal_flags =
                    (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
                if (parts)
                    KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
                if (debris)
                    KillGameObject(candidate, 2, 0);
                else
                    KillGameObject(candidate, 4, 0);
            }
        }
    } else if (object != NULL && (!check_dead || (object->dead == 0 && object->field_1010 <= 0.0f))) {
        object->portal_flags =
            (object->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
        if (parts)
            KillParts(object, -1, -1, 1, 0.0f, 0, NULL);
        if (!debris)
            KillGameObject(object, 4, 0);
        else
            KillGameObject(object, 2, 0);
    }
    return 1;
}

i32 __cdecl Action_Explode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL) {
        Detonate(&packet->owner->position, 0);
    }
    return 1;
}

i32 __cdecl Action_SetScriptState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *target;
    char *state_name;
    char *value;
    i32 creature_set;
    i32 index;
    AISTATE_PC *state;
    GAMEOBJECT *object;
    target = NULL;
    creature_set = 0;
    state_name = NULL;
    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL)
        target = (APIOBJECT_PC *)packet->owner;
    for (index = 0; index < count; index++) {
        if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL) {
            if (GetNamedGameObjectCallback != NULL)
                target = GetNamedGameObjectCallback(system, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AIGameSetName)) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        } else if ((value = NuStrIStr(params[index], (char *)AIGameStateName)) != NULL)
            state_name = value + 6;
    }
    if (state_name != NULL) {
        if (creature_set != 0) {
            object = Obj;
            for (index = 0; index < HIGHGAMEOBJECT; index++, object++) {
                if ((((APIOBJECT_PC *)object)->flags_low & 1) != 0 &&
                    (((APIOBJECT_PC *)object)->flags_low & 0x1000) != 0 &&
                    object->ai_packet.creature_set == creature_set) {
                    state = AIStateFind(state_name, ((APIOBJECT_PC *)object)->ai->script_processor.base_script);
                    if (state != NULL) {
                        ((APIOBJECT_PC *)object)->ai->script_processor.active_ref_count = 0;
                        AIScriptProcessorInit(system, ((APIOBJECT_PC *)object)->ai,
                                              &((APIOBJECT_PC *)object)->ai->script_processor, NULL, NULL, NULL, 0,
                                              ((APIOBJECT_PC *)object)->ai->script_processor.base_script, state);
                    }
                }
            }
        } else if (target != NULL && target->ai != NULL && target->ai->script_processor.base_script != NULL) {
            state = AIStateFind(state_name, target->ai->script_processor.base_script);
            if (state != NULL) {
                target->ai->script_processor.active_ref_count = 0;
                AIScriptProcessorInit(system, target->ai, &target->ai->script_processor, NULL, NULL, NULL, 0,
                                      target->ai->script_processor.base_script, state);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetAIOverrideControl(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    i32 enabled;
    char *value;
    GAMEOBJECT *character;

    enabled = 1;
    if (!first_time) {
        object = processor->action_data_3;
    } else {
        object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL)
                    object = GetNamedGameObjectCallback(system, value + 10);
            } else if (NuStrICmp((char *)AIActionFalseName, params[index]) == 0) {
                enabled = 0;
            }
        }
        processor->action_data_3 = object;
    }
    if (object != NULL) {
        if (enabled) {
            character = (GAMEOBJECT *)object->owner;
            if (character != NULL) {
                {
                    character->field_1010 = 0.0f;
                    character->death_timer = 0;
                    if (character->dead != 0) {
                        character->dead = 0;
                        character->current_hp = character->field_107e;
                        character->shield_hitpoints = character->character_data->game_character->shield_hitpoints;
                        character->saved_hitpoints = 4;
                    }
                }
                if (character->takeover_source != NULL) {
                    character->takeover_source->field_1010 = 0.0f;
                    character->takeover_source->death_timer = 0;
                    if (character->takeover_source->dead != 0) {
                        character->takeover_source->dead = 0;
                        character->takeover_source->current_hp = character->takeover_source->field_107e;
                        character->takeover_source->shield_hitpoints =
                            character->takeover_source->character_data->game_character->shield_hitpoints;
                        character->takeover_source->saved_hitpoints = 4;
                    }
                }
            }
        }
        object->flags_low = (object->flags_low & ~0x100u) | ((enabled & 1) << 8);
    }
    return 1;
}

i32 __cdecl Action_SetLastSafePathPos(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    char *value;
    if (first_time == 0) {
        return 1;
    }
    object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL)
                object = GetNamedGameObjectCallback(system, value + 10);
        } else {
            NuStrICmp((char *)AIActionFalseName, params[index]);
        }
    }
    if (object != NULL) {
        object->respawn_position = object->position;
        object->last_safe_position = object->position;
    }
    return 1;
}

i32 __cdecl Action_SetDontMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0) {
                // This action's FALSE toggle uses the first parameter, not the current one.
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (object->flags_ef0 & ~0x10u) | ((enabled & 1) << 4);
        }
    }
    return 1;
}

i32 __cdecl Action_DontSetStoppedFlag(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *value;

    if (first_time) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0)
                enabled = 0;
        }
        if (object != NULL)
            object->flags_ef0 = (object->flags_ef0 & ~0x20u) | ((enabled & 1) << 5);
    }
    return 1;
}

i32 __cdecl Action_GameSetLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *target;
    char *locator_name;
    i32 personal;
    i32 indexed;
    i32 nearest;
    i32 random_count;
    char *value;
    NUVEC difference;
    AILOCATOR_PC *locator;
    f32 best_distance;
    char numbered_name[64];
    AILOCATOR_PC *candidate;
    f32 distance;
    AILOCATOR_PC *nearest_locator;
    (void)param_6;
    best_distance = 1.0e9f;
    locator_name = NULL;
    personal = 0;
    indexed = 0;
    nearest = 0;
    random_count = 0;
    target = NULL;
    locator = NULL;
    nearest_locator = NULL;
    if (param_5 == 0) {
        return 1;
    }
    if (packet != NULL && packet->owner != NULL) {
        target = (APIOBJECT_PC *)packet->owner;
    }
    if (param_4 == 0) {
        return 1;
    }
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            locator_name = value + 5;
        } else if (NuStrICmp(params[index], (char *)AIActionPersonalName) == 0) {
            personal = 1;
        } else if (NuStrICmp(params[index], (char *)AIActionIndexedName) == 0) {
            indexed = 1;
        } else if (NuStrICmp(params[index], (char *)AIGameNearestName) == 0) {
            nearest = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7577dc)) != NULL) {
            random_count = (i32)(AIParamToFloat(processor, value + 7));
        } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL &&
                   GetNamedGameObjectCallback != NULL) {
            target = GetNamedGameObjectCallback(sys, value + 10);
        }
    }

    if (locator_name != NULL) {
        if (nearest) {
            index = 0;
            do {
                sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name, index++);
                candidate = AIPathFindLocator(sys, numbered_name);
                if (candidate != NULL) {
                    distance = NuVecDistSqr(&packet->terrain_origin, &candidate->position, &difference);
                    if (distance < best_distance) {
                        best_distance = distance;
                        nearest_locator = candidate;
                    }
                }
            } while (candidate != NULL);
            locator = nearest_locator;
        } else if (packet != NULL) {
            if (indexed && ((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
                sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name,
                        ((APIOBJECT_PC *)packet->owner)->locator_name_index);
            } else if (personal && ((APIOBJECT_PC *)packet->owner)->character_data != NULL) {
                sprintf(numbered_name, (char *)AILocatorPersonalFormat, locator_name,
                        ((APIOBJECT_PC *)packet->owner)->character_data->name);
            } else if (random_count != 0) {
                sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name, NuRand(0) % random_count);
            } else {
                sprintf(numbered_name, locator_name);
            }
            locator = AIPathFindLocator(sys, numbered_name);
        }
    }
    if (target != NULL) {
        target->ai->locator = locator;
    }
    return 1;
}

i32 __cdecl Action_SetLocatorSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *value;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                processor->locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value + 5);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SnapToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 check_terrain;
    char *value;
    i32 player;
    AILOCATOR_PC *locator;
    char locator_name[64];

    if (first_time != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        name = NULL;
        personal = 0;
        indexed = 0;
        random_count = 0;
        check_terrain = 1;
        for (i = 0; i < param_count; ++i) {

            if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75782c)) != NULL)
                name = value + 5;
            else if (NuStrIStr(params[i], (char *)AIActionPersonalName) != NULL)
                personal = 1;
            else if (NuStrIStr(params[i], (char *)AIActionIndexedName) != NULL)
                indexed = 1;
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757824)) != NULL)
                random_count = (i32)AIParamToFloat(processor, value + 9);
            else if ((value = NuStrIStr(params[i], (char *)AreaConfigKeyword_character)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75781c)) != NULL) {
                player = (i32)AIParamToFloat(processor, value + 7) - 1;
                if ((u32)player <= 1)
                    object = Player[player];
            } else if (NuStrIStr(params[i], (char *)ActionSnapDontCheckTerrainName) != NULL)
                check_terrain = 0;
        }
        if (object == NULL && packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (object != NULL) {
            locator = object->ai_packet.locator;
            if (name != NULL) {

                if (indexed && (i8)object->field_280 != -1)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)object->field_280);
                else if (personal && object->character_data != NULL)
                    sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                else if (random_count != 0)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                else
                    sprintf(locator_name, name);
                locator = AIPathFindLocator(sys, locator_name);
            }
            if (locator != NULL) {
                object->position = locator->position;
                object->field_0x276 = locator->rotation;
                object->facing_angle = locator->rotation;
                object->movement_facing_angle = locator->rotation;
                object->ai_packet.path_info = locator->path_info;
                object->collision_position = object->position;
                object->initial_position = object->position;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->respawn_position = object->position;
                object->last_safe_position = object->position;
                object->ai_update_position = object->position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                if (check_terrain != 0)
                    SetObjOnSurface(object, 0);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_BigJumpToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 personal;
    i32 indexed;
    i32 random_count;
    char *name;
    f32 jump_factor;
    char *value;
    char locator_name[64];
    AILOCATOR_PC *locator;
    (void)param_6;

    jump_factor = 1.0f;
    personal = 0;
    indexed = 0;
    random_count = 0;
    name = NULL;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        processor->action_data_3 = processor->locator;

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
            if (value != NULL) {
                name = value + 5;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionPersonalName)) != NULL) {
                personal = 1;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionIndexedName)) != NULL) {
                indexed = 1;
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757824))) != NULL) {
                random_count = (i32)(AIParamToFloat(processor, value + 9));
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757834))) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            }
        }

        if (name != NULL) {

            if (indexed != 0 && object != NULL && (i8)(object->field_280) != -1) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
            } else if (personal != 0 && object != NULL && object->character_data != NULL) {
                sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
            } else if (random_count != 0) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
            } else {
                sprintf(locator_name, name);
            }
            processor->action_data_3 = AIPathFindLocator(sys, locator_name);
        }

        locator = (AILOCATOR_PC *)(processor->action_data_3);
        if (locator != NULL) {
            StartBigJump(object, &locator->position, 0, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = locator->path_info;
            object->ai_packet.last_path_position = locator->position;
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_BigJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    NUVEC destination;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 to_origin;
    i32 check_terrain;
    f32 jump_factor;
    i32 slam;
    char *value;
    AICREATURE_PC *creature;
    APIOBJECT_PC *lookup;

    (void)param_6;
    jump_factor = 1.0f;
    destination.x = 1.0e9f;
    destination.y = 1.0e9f;
    destination.z = 1.0e9f;
    target = NULL;
    slam = 0;
    check_terrain = 0;
    to_origin = 0;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    if (param_5 != 0) {

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757874);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL &&
                    (lookup = GetNamedGameObjectCallback(sys, value + 14)) != NULL)
                    target = (GAMEOBJECT *)lookup->owner;
                else
                    target = NULL;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral75786c) == 0) {
                slam = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757860) == 0) {
                to_origin = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757834)) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                destination.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                destination.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                destination.z = AIParamToFloat(processor, value + 2);
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757844) == 0) {
                check_terrain = 1;
            }
        }

        if (to_origin != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.creature_index != 0xff) {
            creature = &sys->creatures[object->ai_packet.creature_index];
            StartBigJump(object, &creature->position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = creature->path_info;
            object->ai_packet.last_path_position = creature->position;
        } else if (target != NULL) {
            StartBigJump(object, &target->last_safe_position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = target->ai_packet.path_info;
            object->ai_packet.last_path_position = target->ai_packet.last_path_position;
        } else {
            if (destination.x != 1.0e9 && destination.y != 1.0e9 && destination.z != 1.0e9)
                StartBigJump(object, &destination, slam, jump_factor, 1.0f, check_terrain);
            else
                StartBigJump(object, &object->collision_position, slam, jump_factor, 1.0f, check_terrain);
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_SetDoomedEscapeLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 take_damage;
    char *value;
    char locator_name[64];

    if (first_time) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        if (param_count == 0)
            return 1;
        name = NULL;
        personal = 0;
        indexed = 0;
        random_count = 0;
        take_damage = 0;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75782c)) != NULL)
                name = value + 5;
            else if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL)
                personal = 1;
            else if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL)
                indexed = 1;
            else if (NuStrIStr(params[index], (char *)AITransferLiteral757884) != NULL)
                take_damage = 1;
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757824)) != NULL)
                random_count = (i32)(AIParamToFloat(processor, value + 9));
        }
        if (object != NULL) {
            object->flags_ef0 &= ~0x800u;
            object->doomed_escape_locator = NULL;
            if (name != NULL) {

                if (indexed && (i8)(object->field_280) != -1)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
                else if (personal && object->character_data != NULL)
                    sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                else if (random_count != 0)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                else
                    sprintf(locator_name, name);
                object->doomed_escape_locator = AIPathFindLocator(system, locator_name);
                if (object->doomed_escape_locator != NULL)
                    object->flags_ef0 = (object->flags_ef0 & ~0x800u) | (take_damage << 11);
            }
        }
    }
    return 1;
}

/* Source conventions expose the seven supplied words; historical aggregate
 * types and formal signatures are a separate gate. */
i32 __cdecl Action_SnapToPosition(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    NUVEC position = {0.0f, 0.0f, 0.0f};
    i32 check_terrain = 1;
    GAMEOBJECT *object = NULL;
    i32 facing = 0;
    i32 index;
    char *value;

    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
            NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL) {
            object = player;
        } else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL) {
            object = player2;
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapRotationYName)) != NULL) {
            facing = (i32)((f64)(i32)AIParamToFloat(processor, value + 5) * ActionSnapRotationFactorPc);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapXName)) != NULL) {
            position.x = AIParamToFloat(processor, value + 2);
        } else if ((value = NuStrIStr(params[index], "y")) != NULL) {
            position.y = AIParamToFloat(processor, value + 2);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapZName)) != NULL) {
            position.z = AIParamToFloat(processor, value + 2);
        } else if (NuStrIStr(params[index], (char *)ActionSnapDontCheckTerrainName) != NULL) {
            check_terrain = 0;
        }
    }
    if (object != NULL) {
        object->position = position;
        object->initial_position = position;
        object->collision_position = position;
        object->field_0x276 = (u16)facing;
        object->facing_angle = (u16)facing;
        object->movement_facing_angle = (u16)facing;
        plr_lastpos = position;
        object->start_position = position;
        object->respawn_position = object->position;
        object->last_safe_position = object->position;
        object->ai_update_position = object->position;
        object->velocity = v000;
        InitSurfaceInfo(object);
        if (check_terrain != 0)
            SetObjOnSurface(object, 0);
    }
    return 1;
}

i32 __cdecl Action_SnapToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 check_terrain;
    char *value;
    AICREATURE_PC *creature;

    (void)processor;
    (void)param_6;
    object = NULL;
    check_terrain = 1;
    if (param_5 == 0) {
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrIStr(params[index], (char *)ActionSnapDontCheckTerrainName) != NULL) {
            check_terrain = 0;
        }
    }

    if (object == NULL || (object->flags_1f8 & 0x400) == 0 || object->ai_packet.creature_index == 0xff) {
        return 1;
    }

    creature = &sys->creatures[object->ai_packet.creature_index];
    object->position = creature->position;
    object->field_0x276 = (u16)creature->y_rotation;
    object->facing_angle = (u16)creature->y_rotation;
    object->movement_facing_angle = (u16)creature->y_rotation;
    object->ai_packet.path_info.flags_word &= ~1u;
    object->collision_position = object->position;
    object->initial_position = object->position;
    plr_lastpos = object->collision_position;
    object->start_position = object->collision_position;
    object->respawn_position = object->position;
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->velocity = v000;
    InitSurfaceInfo(object);
    if (check_terrain != 0) {
        SetObjOnSurface(object, 0);
    }
    return 1;
}

i32 __cdecl Action_SnapToSockPosition(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *owner;
    SOCKPOSITION_PC sock_position;
    NUVEC offset;
    NUVEC ring_offset;
    i32 sock_index = 0;
    i32 party = 0;
    i32 from_end = 0;
    i32 index;
    i32 spread;
    i32 random_angle;
    u16 heading;
    f32 distance = 0.0f;
    f64 weighted_term;
    f32 base_dx;
    f32 base_dy;
    f32 dx;
    f32 dy;
    char *value;

    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL) {
            owner = (APIOBJECT_PC *)packet->owner;
            if (owner->owner != NULL)
                object = (GAMEOBJECT *)owner->owner;
        }
        for (index = 0; index < count; ++index) {
            if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
                NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL) {
                object = player;
            } else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL) {
                object = player2;
            } else if (NuStrIStr(params[index], (char *)ActionSnapPartyName) != NULL) {
                party = 1;
            } else if ((value = NuStrIStr(params[index], (char *)ActionSnapRotationYName)) != NULL) {
                AIParamToFloat(processor, value + 5);
            } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDistanceFromEndName)) != NULL) {
                distance = AIParamToFloat(processor, value + 18);
                from_end = 1;
            } else if ((value = NuStrIStr(params[index], (char *)RaceOpponentDistancePrefix)) != NULL) {
                distance = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)ActionSnapSockName)) != NULL) {
                sock_index = (i32)AIParamToFloat(processor, value + 5);
            } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDxName)) != NULL) {
                AIParamToFloat(processor, value + 3);
            } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDyName)) != NULL) {
                AIParamToFloat(processor, value + 3);
            }
        }
        if (from_end != 0) {
            if (WORLD->sock_sys == NULL || (u32)sock_index > 63u)
                return 1;
            SetSockPostion(WORLD->sock_sys, &sock_position, sock_index,
                           (i32)WORLD->sock_sys->socks[sock_index].length - 1, 1.0f);
        } else {
            SetSockPostion(WORLD->sock_sys, &sock_position, sock_index, 0, 0.0f);
        }
        if (sock_position.sock_index == -1)
            return 1;
        MoveSockPosition(WORLD->sock_sys, &sock_position, distance, &sock_position);
        weighted_term = (1.0 - (f64)NuRandFloat()) * ActionSnapWeightedRandomZeroPc;
        base_dx = (f32)((f64)NuRandFloat() * ActionSnapWeightedRandomZeroPc + weighted_term);
        weighted_term = (1.0 - (f64)NuRandFloat()) * ActionSnapWeightedRandomZeroPc;
        base_dy = (f32)((f64)NuRandFloat() * ActionSnapWeightedRandomZeroPc + weighted_term);
        heading = sock_position.heading;
        random_angle = NuRand(NULL) % 65536;
        spread = 0;
        for (index = 0; index < 8; ++index, spread += 65536) {
            if (party != 0)
                object = (GAMEOBJECT *)Player[index];
            if (object != NULL && (object->flags_1fc_word & 1u) != 0 && (object->flags_1fc_word & 0x1000u) != 0) {
                dx = base_dx;
                dy = base_dy;
                if (index > 1) {
                    ring_offset.x = 1.0f;
                    ring_offset.y = 0.0f;
                    ring_offset.z = 0.0f;
                    NuVecRotateZ(&ring_offset, &ring_offset, NuAngAdd(random_angle, spread / 3));
                    dx = (f32)((f64)ring_offset.x + (f64)base_dx);
                    dy = (f32)((f64)ring_offset.y + (f64)base_dy);
                }
                if (dx != 0.0f || dy != 0.0f) {
                    offset.x = dx;
                    offset.y = dy;
                    offset.z = 0.0f;
                    NuVecRotateX(&offset, &offset, sock_position.midpoint_pitch);
                    NuVecRotateY(&offset, &offset, NuAngAdd(heading, 0x8000));
                } else {
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = 0.0f;
                }
                NuVecAdd(&offset, &offset, &sock_position.midpoint);
                object->position = offset;
                object->collision_position = object->position;
                object->initial_position = object->position;
                object->field_0x276 = heading;
                object->facing_angle = heading;
                object->movement_facing_angle = heading;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->respawn_position = object->position;
                object->last_safe_position = object->position;
                object->ai_update_position = object->position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                SetObjOnSurface(object, 0);
            }
            if (party == 0)
                break;
        }
    }
    return 1;
}

i32 __cdecl Action_SetOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *opponent;
    GAMEOBJECT *candidate;
    GAMEOBJECT *droids[8];
    i32 droid_count;
    i32 index;
    i32 player_index;
    i32 object_index;
    i32 type;
    char *value;
    opponent = NULL;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        droid_count = 0;
        for (index = 0; index < count; index++) {
            if (NuStrIStr(params[index], (char *)AICombatDroidName) != NULL) {
                for (player_index = 0; player_index < 8; player_index++) {
                    candidate = (GAMEOBJECT *)Player[player_index];
                    if (candidate != NULL && (candidate->flags_1fc_word & 1) != 0 &&
                        (candidate->flags_1fc_word & 0x1000) != 0 &&
                        (candidate->dead == 0 || candidate->field_1010 > 0.0f) &&
                        (candidate->flags_ef0 & 0x01000000u) == 0 &&
                        (candidate->character_data->model_flags & 0x10u) != 0)
                        droids[droid_count++] = candidate;
                }
                if (droid_count != 0) {
                    prev_droid_ix = (prev_droid_ix + 1) % droid_count;
                    opponent = droids[prev_droid_ix];
                }
            } else if (NuStrIStr(params[index], (char *)AICombatNearestEnemyName) != NULL)
                object->opponent = NULL;
            else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL)
                opponent = GetNamedGameObject(system, value + 9);
            else if (NuStrICmp(params[index], (char *)AICombatLastAttackerName) == 0)
                opponent = object->last_attacker;
            else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentTypeName)) != NULL) {
                type = 255;
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    type = (u8)LevelCharacterTypeIDFn(value + 13);
                    if (type != 255)
                        type = LevelCharacterGlobalIDFn(type);
                }
                for (object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
                    if ((i16)Obj[object_index].id == type) {
                        opponent = &Obj[object_index];
                        break;
                    }
                }
            }
        }
        object->opponent = opponent;
    }
    return 1;
}

i32 __cdecl Action_AttackOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    APIOBJECT_PC *opponent;
    APIOBJECT_PC *reloaded_opponent;
    i32 index;
    i32 fire;
    char *value;
    f32 distance_squared;
    f32 goal_range;
    f32 attack_range;
    f32 attack_squared;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    ai_fighting = 1;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            value = NuStrIStr(params[index], (char *)AIPathGoalRangeName);
            if (value != NULL)
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 10);
            else if ((value = NuStrIStr(params[index], (char *)AIGameRangeName)) != NULL)
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 6);
            else if (NuStrICmp(params[index], (char *)AIGameJediGoodieAttackName) == 0)
                processor->action_data_2 = 1;
            else if ((value = NuStrIStr(params[index], (char *)AIGameAttackOverrideName)) != NULL) {
                value += 16;
                if (NuStrICmp(value, (char *)AIGamePunch1Name) == 0)
                    processor->action_data_6 = 1;
                else if (NuStrICmp(value, (char *)AIGamePunch2Name) == 0)
                    processor->action_data_6 = 2;
                else if (NuStrICmp(value, (char *)AIGamePunch3Name) == 0)
                    processor->action_data_6 = 3;
                else if (NuStrICmp(value, (char *)AIGamePunchBehindName) == 0)
                    processor->action_data_6 = 4;
                else if (NuStrICmp(value, (char *)AIGamePunchSpecialName) == 0)
                    processor->action_data_6 = 5;
                else if (NuStrICmp(value, (char *)AIGameBlockName) == 0)
                    processor->action_data_6 = 6;
                else if (NuStrICmp(value, (char *)AIGameShootName) == 0)
                    processor->action_data_6 = 7;
                else
                    processor->action_data_6 = 0;
            }
        }
        processor->action_data_4 = 0.2f;
    }
    if (processor->action_data_6 != 0)
        object->attack_override = (u8)processor->action_data_6;
    opponent = (APIOBJECT_PC *)packet->opponent;
    if (opponent == NULL || opponent->ai == NULL)
        return 0;
    target = (GAMEOBJECT *)opponent->owner;
    object->portal_flags |= 0x20u;
    if ((((APIOBJECT_PC *)target)->flags_low & 0x1001u) != 0x1001u || target->dead != 0 ||
        target->character_context == 0x2b ||
        ((((APIOBJECT_PC *)target)->flags_low & 0x80u) != 0 && !(target->spawn_protection_timer > 1.5f)))
        return 0;
    distance_squared = NuVecDistSqr(&target->position, &((APIOBJECT_PC *)packet->owner)->position, NULL);
    if ((object->flags_ef4 & 0x2000u) != 0) {
        reloaded_opponent = (APIOBJECT_PC *)object->ai_packet.opponent;
        if (oneAtOnce_CanAttack(object, (GAMEOBJECT *)reloaded_opponent->owner) != 0) {
            packet->movement_instruction_parameter = 0.0f;
            processor->action_data_1 &= ~2u;
        } else {
            packet->movement_instruction_parameter = oneAtOnce_GetHoldRange(object);
            processor->action_data_1 |= 2;
        }
    }
    if ((processor->action_data_1 & 2) == 0) {
        goal_range = target->ai_packet.mover_height + packet->movement_instruction_parameter + packet->mover_height;
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, 0.0f, &target->ai_packet.path_info, 1,
                          goal_range);
        packet->goal_speed_mode = 0;
    } else if (distance_squared <
               (packet->movement_instruction_parameter - aitol) * (packet->movement_instruction_parameter - aitol)) {
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, packet->mover_height,
                          &target->ai_packet.path_info, 2, packet->movement_instruction_parameter);
        packet->goal_speed_mode = 0;
    } else if (distance_squared >
               (packet->movement_instruction_parameter + aitol) * (packet->movement_instruction_parameter + aitol)) {
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, packet->mover_height,
                          &target->ai_packet.path_info, 1, packet->movement_instruction_parameter);
        packet->goal_speed_mode = 0;
    } else
        packet->movement_look_target = &target->position;
    fire = 0;
    if (processor->action_data_4 > 0.0f) {
        if (target->character_context == 0x5a)
            return 0;
        distance_squared = NuVecDistSqr(&target->position, &((APIOBJECT_PC *)packet->owner)->position, NULL);
        attack_range = target->ai_packet.mover_height + processor->action_data_4 + object->ai_packet.mover_height;
        attack_squared = attack_range * attack_range;
        if (distance_squared < attack_squared)
            fire = 1;
    } else {
        object->pad->runtime_flags |= 2;
        fire = 1;
    }
    if (fire != 0) {
        object->pad->buttons_pressed |= GAMEPAD_ACTION;
        if (processor->action_data_2 != 0)
            object->portal_flags |= 0x400u;
    }
    return 0;
}

i32 __cdecl Action_EngageOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    f32 initial_fire_fraction;
    i32 explicit_fire_range = 0;
    i32 index;
    char *value;
    GAMEOBJECT *opponent = NULL;
    APIOBJECT_PC *api_opponent;
    NUVEC difference;
    f32 distance_squared;
    f32 goal_range;
    f32 stopping_distance;
    f32 retreat_parameter;
    u32 character_flags_2;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }

    object = ((APIOBJECT_PC *)packet->owner)->owner;
    ai_fighting = 1;
    if (param_5 != 0) {
        processor->action_data_5 = engagefiretime;
        packet->movement_instruction_parameter = idealgoalrange;
        initial_fire_fraction = NuRandFloat();

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameFireRangeName);
            if (value != NULL) {
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
                explicit_fire_range = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatMinimumRangeName)) != NULL) {
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 9);
                processor->action_data_1 |= 2;
            } else if (NuStrIStr(params[index], (char *)AICombatStaticName) != NULL) {
                packet->movement_instruction_parameter = 0.0f;
            } else if (NuStrIStr(params[index], (char *)AICombatOffscreenName) != NULL) {
                processor->action_data_1 |= 1;
            } else if (NuStrIStr(params[index], (char *)AICombatNoFireInMiniCutName) != NULL) {
                processor->action_data_1 |= 4;
            } else if (NuStrIStr(params[index], (char *)AICombatCircleName) != NULL) {
                typedef union AI_ENGAGE_CIRCLE_FLAGS_PC {
                    u32 raw;
                    struct {
                        u32 reserved_before : 9;
                        u32 circle_clockwise : 1;
                        u32 reserved_after : 22;
                    } bits;
                } AI_ENGAGE_CIRCLE_FLAGS_PC;
                AI_ENGAGE_CIRCLE_FLAGS_PC *flags = (AI_ENGAGE_CIRCLE_FLAGS_PC *)&packet->flags;
                flags->bits.circle_clockwise = !flags->bits.circle_clockwise;
                processor->action_data_1 |= 8;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL) {
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 9);
            } else if (NuStrIStr(params[index], (char *)AICombatInstantName) != NULL) {
                initial_fire_fraction = 0.0f;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameAttackOverrideName)) != NULL) {
                value += 16;
                if (NuStrICmp(value, (char *)AIGamePunch1Name) == 0)
                    processor->action_data_6 = 1;
                else if (NuStrICmp(value, (char *)AIGamePunch2Name) == 0)
                    processor->action_data_6 = 2;
                else if (NuStrICmp(value, (char *)AIGamePunch3Name) == 0)
                    processor->action_data_6 = 3;
                else if (NuStrICmp(value, (char *)AIGamePunchBehindName) == 0)
                    processor->action_data_6 = 4;
                else if (NuStrICmp(value, (char *)AIGamePunchSpecialName) == 0)
                    processor->action_data_6 = 5;
                else if (NuStrICmp(value, (char *)AIGameBlockName) == 0)
                    processor->action_data_6 = 6;
                else if (NuStrICmp(value, (char *)AIGameShootName) == 0)
                    processor->action_data_6 = 7;
                else
                    processor->action_data_6 = 0;
            }
        }
        if (LSW1 != 0) {
            processor->action_data_1 |= 0x20;
        }
        processor->action_timer = initial_fire_fraction * processor->action_data_5;
        if (!explicit_fire_range) {
            if (packet->movement_instruction_parameter == 0.0f)
                processor->action_data_4 = 9999.9f;
            else
                processor->action_data_4 = packet->movement_instruction_parameter + aitol;
        }
    }

    if (processor->action_data_3 != NULL) {
        opponent = (GAMEOBJECT *)(processor->action_data_3);
        if (!(opponent->flags_1fc_word & 1) || !(opponent->flags_1fc_word & 0x1000) ||
            (opponent->dead != 0 && !(opponent->field_1010 > 0.0f)))
            opponent = NULL;
    }
    if (processor->action_data_6 != 0) {
        object->attack_override = (u8)(processor->action_data_6);
    }
    if (opponent == NULL) {
        api_opponent = (APIOBJECT_PC *)(packet->opponent);
        if (api_opponent == NULL || api_opponent->ai == NULL || (opponent = api_opponent->owner) == NULL)
            return 0;
    }
    object->portal_flags |= 0x20;
    if (!(opponent->flags_1fc_word & 1) || !(opponent->flags_1fc_word & 0x1000) || opponent->dead != 0 ||
        opponent->character_context == 0x2b ||
        ((opponent->flags_1fc_word & 0x80u) != 0 && opponent->spawn_protection_timer > 1.5f))
        return 0;

    distance_squared = NuVecDistSqr(&opponent->position, &packet->owner->position, &difference);

    if ((object->flags_ef4 & 0x2000u) != 0) {
        if (oneAtOnce_CanAttack(object, (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner)) {
            goal_range = 0.1f;
            processor->action_data_1 &= (u8)(~2u);
        } else {
            goal_range = oneAtOnce_GetHoldRange(object);
            processor->action_data_1 |= 2;
        }
    } else if (packet->movement_instruction_parameter != 0.0f &&
               (((opponent->flags_1fc_word & 0x80u) != 0 && object->model_draw_result == 0) ||
                (((u64)1 << opponent->hostility_group) &
                 WORLD->object_sys->hostility_masks[((APIOBJECT_PC *)packet->owner)->object_id]) == 0)) {
        goal_range = 0.1f;
    } else {
        goal_range = packet->movement_instruction_parameter;
    }

    if (goal_range == 0.0f) {
        if ((object->character_data->game_character->flags_090 & 0x100) == 0)
            packet->movement_look_target = &opponent->position;
    } else {
        stopping_distance =
            (goal_range < opponent->ai_packet.mover_height ? goal_range : opponent->ai_packet.mover_height);
        if ((object->field_ef2 & 1) == 0)
            processor->action_data_1 &= (u8)(~0x10u);
        if ((object->field_ef2 & 1) != 0 && (object->character_data->game_character->flags_090 & 0x100) != 0) {
            if ((processor->action_data_1 & 0x10) != 0) {
                if (distance_squared > (goal_range + aitol) * (goal_range + aitol))
                    processor->action_data_1 &= (u8)(~0x10u);
            } else if (distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
                processor->action_data_1 |= 0x10;
            }
            if ((processor->action_data_1 & 0x10) != 0) {
                AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                                  &opponent->ai_packet.path_info, 2, goal_range);
                packet->goal_speed_mode = 1;
                object->flags_ef0 |= 0x8000u;
            } else {
                AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, 0.0f, &opponent->ai_packet.path_info,
                                  1, 0.01f);
                packet->goal_speed_mode = 0;
            }
        } else if ((object->character_data->game_character->flags_090 & 0x100) != 0 &&
                   distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, goal_range);
            packet->goal_speed_mode = 1;
            if ((object->character_data->game_character->flags_090 & 0x100) != 0)
                object->flags_ef0 |= 0x8000u;
        } else if ((processor->action_data_1 & 0x20) != 0 &&
                   distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, goal_range);
            packet->goal_speed_mode = 1;
        } else if (StartupGameFlag1 == 0 &&
                   distance_squared + distance_squared / 10.0 < (goal_range + aitol) * (goal_range + aitol) &&
                   (processor->action_data_1 & 2) != 0) {
            retreat_parameter = aitol + packet->movement_instruction_parameter;
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, retreat_parameter);
            packet->goal_speed_mode = 0;
        } else if (distance_squared > (goal_range + aitol) * (goal_range + aitol) &&
                   (StartupGameFlag1 == 0 || (processor->action_data_1 & 2) == 0)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 1, goal_range);
            packet->goal_speed_mode = 0;
        } else if ((processor->action_data_1 & 8) != 0) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 3, goal_range);
            packet->goal_speed_mode = 0;
        } else if ((object->character_data->game_character->flags_090 & 0x100) == 0) {
            packet->movement_look_target = &opponent->position;
        }
    }

    if ((((u64)1 << opponent->hostility_group) & WORLD->object_sys->hostility_masks[object->hostility_group]) == 0)
        return 0;
    character_flags_2 = object->character_data->game_character->flags_090 & 0x100;
    if (character_flags_2 != 0 && (object->field_ef2 & 1) != 0)
        return 0;
    if (object->model_draw_result == 0 && (processor->action_data_1 & 1) == 0)
        return 0;
    if (!(distance_squared < processor->action_data_4 * processor->action_data_4))
        return 0;
    if (character_flags_2 == 0)
        packet->movement_look_target = &opponent->position;
    if (MiniCutCam != 0 && (processor->action_data_1 & 4) != 0) {
        processor->action_timer = (f32)(NuRandFloat() * processor->action_data_5 + 2.0);
    } else {
        processor->action_timer -= param_6;
    }
    if (opponent->dead == 0) {
        object->script_fire_target = opponent;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = (f32)(NuRandFloat() * processor->action_data_5 + processor->action_data_5 * 0.5);
            if (PartyMemberInWay(object, opponent) == 0) {
                if (oneAtOnce_CanAttack(object, opponent))
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
            }
        }
    }
    return 0;
}

i32 __cdecl Action_ShootAtOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    f32 fraction;
    i32 explicit_range;
    i32 index;
    char *value;
    GAMEOBJECT *opponent;
    NUVEC difference;
    f32 distance;
    i32 outside_arc;
    NUVEC relative;
    f32 interval;
    f32 interval_2;
    f32 random;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (param_5 != 0) {
        processor->action_data_5 = engagefiretime;
        fraction = NuRandFloat();
        explicit_range = 0;
        for (index = 0; index < param_4; ++index) {

            if ((value = NuStrIStr(params[index], (char *)AIGameFireRangeName)) != NULL) {
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
                explicit_range = 1;
            } else if (NuStrIStr(params[index], (char *)AICombatOffscreenName) != NULL) {
                processor->action_data_1 |= 1;
            } else if (NuStrIStr(params[index], (char *)AICombatNoFireInMiniCutName) != NULL) {
                processor->action_data_1 |= 4;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL) {
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 9);
            } else if (NuStrIStr(params[index], (char *)AICombatInstantName) != NULL) {
                fraction = 0.0f;
            } else if (NuStrICmp(params[index], (char *)AICombatFrontArcOnlyName) == 0) {
                processor->action_data_2 = 1;
            }
        }
        processor->action_timer = fraction * processor->action_data_5;
        if (!explicit_range)
            if (packet->movement_instruction_parameter == 0.0f)
                processor->action_data_4 = 9999.9f;
            else
                processor->action_data_4 = packet->movement_instruction_parameter + aitol;
    }
    opponent = (GAMEOBJECT *)(processor->action_data_3);
    if (opponent == NULL || (opponent->flags_1fc_word & 0x1001) != 0x1001 ||
        (opponent->dead != 0 && !(opponent->field_1010 > 0.0f))) {
        if (packet->opponent == NULL || ((APIOBJECT_PC *)packet->opponent)->ai == NULL ||
            ((APIOBJECT_PC *)packet->opponent)->owner == NULL)
            return 0;
        opponent = ((APIOBJECT_PC *)packet->opponent)->owner;
    }

    distance = NuVecDistSqr(&packet->owner->position, &opponent->position, &difference);
    object->portal_flags |= 0x20;
    outside_arc = processor->action_data_2;
    if (outside_arc != 0) {

        NuVecSub(&relative, &opponent->position, &packet->owner->position);
        NuVecRotateY(&relative, &relative, -(i32)(((GAMEOBJECT *)packet->owner)->field_0x276));
        if (relative.z > 0.0f)
            outside_arc = 0;
    }
    if ((((u64)1 << opponent->hostility_group) & WORLD->object_sys->hostility_masks[object->hostility_group]) == 0)
        return 0;
    if (object->model_draw_result == 0 && (processor->action_data_1 & 1) == 0)
        return 0;
    if (!(distance < processor->action_data_4 * processor->action_data_4))
        return 0;
    if (opponent->dead != 0 && !(opponent->field_1010 > 0.0f))
        return 0;
    if (outside_arc != 0)
        return 0;
    packet->movement_look_target = &opponent->position;
    if (MiniCutCam != 0 && (processor->action_data_1 & 4) != 0) {
        interval = processor->action_data_5;
        processor->action_timer = (f32)(NuRandFloat() * interval + 2.0);
    } else {
        processor->action_timer -= param_6;
    }
    if (opponent->dead == 0 && processor->action_timer <= 0.0f) {
        interval_2 = processor->action_data_5;
        random = NuRandFloat();
        processor->action_timer = (f32)(random * interval_2 + 0.5 * interval_2);
        if (PartyMemberInWay(object, opponent) == 0) {
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
            object->script_fire_target = opponent;
        }
    }
    return 0;
}

i32 __cdecl Action_EngageObject(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    NUVEC *target;
    (void)sys;
    if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }

    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (param_5 != 0) {
        char *name;
        i32 nearest;
        i32 index;
        packet->movement_instruction_parameter = idealgoalrange;
        processor->action_data_5 = engagefiretime;

        name = NULL;
        nearest = 0;
        for (index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], (char *)"name");
            if (value != NULL) {
                name = value + 5;
            } else if ((value = NuStrIStr(params[index], (char *)"fireinterval")) != NULL) {
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
            } else if (NuStrIStr(params[index], (char *)"nearest") != NULL) {
                nearest = 1;
            } else if (NuStrIStr(params[index], (char *)"predictive") != NULL) {
                processor->action_data_1 = 1;
            }
        }

        processor->action_timer = NuRandFloat() * processor->action_data_5;
        if (name != NULL) {
            if (nearest != 0) {
                NUSPECIALHANDLE specials[50];
                const i32 special_count = NuSpecialFindMulti(WORLD->current_gscn, specials, name, 50, 0);
                if (special_count >= 1) {
                    i32 index;
                    f32 nearest_distance = 1.0e9f;
                    NUVEC difference;
                    for (index = 0; index < special_count; ++index) {
                        NUVEC *position = NuSpecialGetDrawPos(&specials[index]);
                        const f32 distance =
                            NuVecDistSqr(&((APIOBJECT_PC *)packet->owner)->position, position, &difference);
                        if (distance < nearest_distance) {
                            nearest_distance = distance;
                            processor->action_data_3 = position;
                        }
                    }
                }
            } else {
                NUSPECIALHANDLE special;
                if (NuSpecialFind(WORLD->current_gscn, &special, name, 1) != 0) {
                    processor->action_data_3 = NuSpecialGetDrawPos(&special);
                }
            }
        }

        if (processor->action_data_3 != NULL) {
            object->portal_flags |= 0x10000;
            object->quick_shoot_timer = NuRandFloat() * processor->action_data_5;
        }
    }

    target = (NUVEC *)processor->action_data_3;
    if (target == NULL) {
        return 0;
    }

    packet->movement_look_target = target;
    processor->action_timer -= param_6;
    object->field_e4c = *target;
    object->portal_flags |= 0x8000;
    object->portal_flags |= 0x20;
    if (processor->action_timer > 0.0f) {
        return 0;
    }

    object->portal_flags = (object->portal_flags & ~0x20000u) | ((processor->action_data_1 & 1) << 17);
    processor->action_timer = (0.5f + NuRandFloat()) * processor->action_data_5;
    object->pad->buttons_pressed |= GAMEPAD_ACTION;
    return 0;
}

i32 __cdecl Action_GrabVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *victim = NULL;
    i32 index;
    char *value;
    APIOBJECT_PC *named;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time == 0)
        return 1;
    if (object->ai_packet.opponent != NULL)
        victim = ((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a20);
        if (value != NULL) {
            named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            victim = named != NULL ? named->owner : NULL;
        }
    }
    if (victim != NULL)
        GrabVictim(object, victim);
    return 1;
}

i32 __cdecl Action_EatVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        EatVictim(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_ReleaseVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        ReleaseEat(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_FollowDirection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    APIOBJECT_PC *opponent;
    AILOCATOR_PC *start;
    AILOCATOR_PC *end;
    f32 random;
    f32 distance_squared;
    f32 interval;
    i32 index;
    char *value;
    NUVEC destination;
    NUVEC difference;
    start = NULL;
    end = NULL;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        processor->action_pos.x = 0.0f;
        processor->action_pos.y = 0.0f;
        processor->action_pos.z = 1.0f;
        random = NuRandFloat();
        for (index = 0; index < count; index++) {
            value = NuStrIStr(params[index], (char *)AIGameStartName);
            if (value != NULL)
                start = AIPathFindLocator(system, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIGameEndName)) != NULL)
                end = AIPathFindLocator(system, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIGameFireRangeName)) != NULL)
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
            else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL)
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
        }
        if (start != NULL && end != NULL) {
            NuVecSub(&processor->action_pos, &end->position, &start->position);
            NuVecNorm(&processor->action_pos, &processor->action_pos);
        }
        processor->action_timer = random * processor->action_data_5;
        NuVecScale(&processor->action_pos, &processor->action_pos, direction_scale);
    }
    NuVecAdd(&destination, &object->position, &processor->action_pos);
    AIMoveInstruction(packet, &destination, 0.0f, NULL, 1, 0.0f);
    opponent = (APIOBJECT_PC *)packet->opponent;
    if (processor->action_data_4 > 0.0f && opponent != NULL && opponent->ai != NULL) {
        distance_squared = NuVecDistSqr(&((APIOBJECT_PC *)packet->owner)->position, &opponent->position, &difference);
        if ((((u64)1 << opponent->object_id) & WORLD->object_sys->hostility_masks[object->hostility_group]) != 0 &&
            object->model_draw_result != 0 && distance_squared < processor->action_data_4 * processor->action_data_4) {
            target = (GAMEOBJECT *)opponent->owner;
            if (opponent->leader_selection_state == 0 || target->field_1010 > 0.0f) {
                processor->action_timer -= elapsed;
                if (((APIOBJECT_PC *)packet->opponent)->leader_selection_state == 0 &&
                    processor->action_timer <= 0.0f) {
                    interval = processor->action_data_5;
                    processor->action_timer = (f32)(NuRandFloat() * interval + 0.5 * interval);
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
                    object->script_fire_target = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
                }
            }
        }
    }
    return 0;
}

i32 __cdecl Action_PressSpecialButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 result;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    object = packet->owner->objptr;
    if (object == NULL)
        return 1;
    if (first_time) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a38) == 0)
                processor->action_data_1 = 1;
        }
    }
    object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
    result = 1;
    if (processor->action_data_1 == 0)
        return result;
    object->pad->buttons_held |= GAMEPAD_SPECIAL;
    result = 0;
    return result;
}

i32 __cdecl Action_PressTagButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_TAG;
    }
    return 1;
}

i32 __cdecl Action_PressActionButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_ACTION;
        object->portal_flags |= 0x400u;
        object->portal_flags |= 0x20;
    }
    return 1;
}

i32 __cdecl Action_UseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (object != NULL) {
        object->portal_flags |= 0x20000000u;
    }
    return 1;
}

i32 __cdecl Action_SetInvulnerable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 types[10];
    i32 type_count;
    i32 still_take_hit;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;
    i32 local_type;
    i32 global_type;
    GAMEOBJECT *candidate;
    i32 index, object_index, type_index;
    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    type_count = 0;
    still_take_hit = 0;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    enabled = 1;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
            if (value != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                    if (local_type != 0xff) {
                        global_type = LevelCharacterGlobalIDFn(local_type);
                        if (global_type != -1 && type_count < 10) {
                            types[type_count++] = global_type;
                        }
                    }
                }
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a44) == 0) {
                still_take_hit = 1;
            }
        }
    }

    if (type_count != 0) {
        candidate = Obj;

        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (candidate->flags_1f8 & 0x400) == 0) {
                continue;
            }
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == types[type_index]) {
                    if (enabled) {
                        candidate->flags_ef0 =
                            (candidate->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
                    } else {
                        candidate->flags_ef0 &= ~0x400000u;
                    }
                }
            }
        }
    } else if (object != NULL && (object->flags_1f8 & 0x400) != 0) {
        if (enabled) {

            object->flags_ef0 = (object->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
        } else {
            object->flags_ef0 &= ~0x400000u;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAvoidCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    GAMEOBJECT *dont_avoid;
    i32 enabled;
    char *value;

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        if (param_count > 0) {
            dont_avoid = NULL;
            enabled = 1;
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (value != NULL) {
                    object = GetNamedGameObject(system, value + 10);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AITransferLiteral757a5c);
                if (value != NULL) {
                    dont_avoid = GetNamedGameObject(system, value + 11);
                    continue;
                }
                if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
            if (object != NULL && dont_avoid != NULL)
                object->ai_packet.dont_avoid_character = enabled ? dont_avoid : NULL;
        }
    }
    return 1;
}

i32 __cdecl Action_GameDontPush(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 dont_push;
    i32 types[10];
    i32 type_count;
    i32 type_index;
    i32 object_index;
    GAMEOBJECT *candidate;
    char *value;
    i32 local_type;
    i32 global_type;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    dont_push = 1;

    type_count = 0;

    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
            if (value != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    local_type = (u32)LevelCharacterTypeIDFn(value + 5);
                    if (local_type != 0xff) {
                        global_type = LevelCharacterGlobalIDFn(local_type);
                        if (global_type != -1 && type_count < 10)
                            types[type_count++] = global_type;
                    }
                }
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                dont_push = 0;
            }
        }
    }
    if (type_count != 0) {
        candidate = Obj;
        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
            if ((candidate->flags_1fc_word & 0x1001u) != 0x1001u || (candidate->flags_1f8 & 0x400u) == 0)
                continue;
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == types[type_index])
                    candidate->flags_1fc_word = (candidate->flags_1fc_word & ~2u) | ((dont_push & 1) << 1);
            }
        }
    } else if (object != NULL) {
        object->flags_1fc_word = (object->flags_1fc_word & ~2u) | ((dont_push & 1) << 1);
    }
    return 1;
}

i32 __cdecl Action_PressJumpButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    return 1;
}

i32 __cdecl Action_AddToSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 creature_set;
    (void)sys;
    (void)param_6;
    if (param_5 == 0)
        return 1;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object == NULL)
        return 1;
    for (index = 0; index < param_4; ++index) {
        if (NuStrICmp(params[index], (char *)AITransferLiteral757a68) == 0) {
            object->ai_packet.creature_set = 0;
            continue;
        }
        creature_set = (i32)(AIParamToFloat(processor, params[index]));
        if ((u32)(creature_set - 1) <= 15) {
            object->ai_packet.creature_set = (u8)(creature_set);
            ++aicreature_sets_alive[creature_set - 1];
        }
    }
    return 1;
}

i32 __cdecl Action_SetControlSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->ai_packet.movement_stopped = 0;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a70) == 0) {
                object->ai_packet.movement_stopped = 1;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    NUGSPLINE *spline;
    i32 looping;
    char *value;
    SPLINEPOS *position;

    (void)sys;
    (void)processor;
    (void)param_6;
    spline = NULL;
    looping = 0;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757a84);
            if (value != NULL) {
                spline = NuSplineFind(WORLD->current_gscn, value + 7);
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a7c) == 0) {
                looping = 1;
            }
        }

        position = &object->movement_spline_position;
        memset(position, 0, sizeof(*position));
        if (spline != NULL) {
            InitSplinePosition(position, spline, 0.0f, looping);
        }
    }
    return 1;
}

i32 __cdecl Action_SetZeroAcceleration(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;

    if (first_time) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        enabled = 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
                NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL)
                object = player;
            else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL)
                object = player2;
            else if (NuStrIStr(params[index], (char *)AIActionFalseName) != NULL)
                enabled = 0;
        }
        if (object != NULL)
            object->ai_packet.flags = (object->ai_packet.flags & ~0x40000u) | ((enabled & 1) << 18);
    }
    return 1;
}

i32 __cdecl Action_BreakFormation(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {

    if (packet != NULL && packet->group != NULL) {
        packet->group->flags &= ~8u;
    }
    return 1;
}

i32 __cdecl Action_FormationMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    if (packet != NULL && packet->group != NULL) {
        packet->group->flags |= 8u;
    }
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 1;
}

i32 __cdecl Action_SetFormationCommander(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                         char **unused3, i32 unused4, i32 unused5, f32 unused6) {
    GAMEOBJECT *object;
    AIGROUP_PC *group;
    i32 commander_id;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || packet->group == NULL)
        return 1;
    object = packet->owner->objptr;
    group = packet->group;
    if (packet->group_member_index != group->member_count - 1)
        return 1;
    if (group->count_across != 1 && group->member_count % group->count_across != 1)
        return 1;
    packet->flags |= 0x1000000;
    if ((i16)object->id == id_BATTLEDROID || (i16)object->id == id_BATTLEDROIDSECURITY ||
        (i16)object->id == id_BATTLEDROIDGEONOSIAN)
        commander_id = id_BATTLEDROIDCOMMANDER;
    else if ((i16)object->id == id_CLONEEP3)
        commander_id = id_CLONEEP3SAND;
    else
        return 1;
    NewPlayerCharacter(object, commander_id, (i16)object->id, 1);
    object->current_hp = object->character_data->game_character->hitpoints;
    object->field_107e = object->character_data->game_character->hitpoints;
    return 1;
}

i32 __cdecl Action_RemoveThrownForceObjects(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                            char **params, i32 count, i32 first_time, f32 delta) {
    PART_PC *part = Part;
    i32 index;
    (void)system;
    (void)processor;
    (void)packet;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    for (index = 0; index < MAXPARTS; ++index, ++part)
        if ((part->active & 1) && part->kill_fn == PartKill_ForceThrow)
            KillPart(part, 0);
    return 1;
}

static __declspec(noinline) ZIPUP_s *LocalGetRandomSpawnZipUp(ZIPUP_s **zipups, i32 count) {
    i32 index;
    i32 selection;
    ZIPUP_s *result;
    i32 eligible = 0;
    if (count > 32)
        count = 32;
    for (index = 0; index < count; ++index) {
        if (zipups[index] != NULL && (zipups[index]->flags & 0x40) != 0)
            ++eligible;
    }
    if (eligible == 0)
        return NULL;
    selection = qrand() / (65535 / eligible + 1);
    result = zipups[selection];
    zipups[selection] = NULL;
    return result;
}

static __declspec(noinline) ZIPUP_s *FindSpawnZipUp(char *name) {
    i32 index;
    ZIPUP_s *zipup = (ZIPUP_s *)WORLD->zipups;
    for (index = 0; index < WORLD->zipup_count; ++index, ++zipup) {
        if ((zipup->flags & 0xc0) == 0xc0 && NuStrICmp(name, zipup->name) == 0)
            return zipup;
    }
    return NULL;
}

i32 __cdecl Action_Launch(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                          i32 count, i32 first_time, f32 elapsed) {
    if (first_time != 0 && packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        StartLaunch(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_SelectRandomSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    NUGSPLINE *splines[32];
    NUGSPLINE *unused_splines[32];
    i32 count;
    i32 unused;
    char *value;
    NUGSPLINE *spline;
    GAMEOBJECT *object;
    i32 unused_count;
    if (first_time) {

        count = 0;
        unused = 0;
        script_spline_selected = NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757a94);
            if (value != NULL) {
                count += NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[count], 32 - count);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757a84)) != NULL) {
                spline = NuSplineFind(WORLD->current_gscn, value + 7);
                if (spline != NULL && count < 32)
                    splines[count++] = spline;
            } else if (NuStrIStr(params[index], (char *)AITransferLiteral757a8c) != NULL) {
                unused = 1;
            }
        }
        if (count != 0) {
            if (unused) {
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length |= 0x8000;
                }
                unused_count = 0;
                for (index = 0; index < count; ++index) {
                    if (splines[index]->length >= 0)
                        unused_splines[unused_count++] = splines[index];
                }
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length &= 0x7fff;
                }
                if (unused_count != 0)
                    script_spline_selected = unused_splines[qrand() / (0xffff / unused_count + 1)];
            } else {
                script_spline_selected = splines[qrand() / (0xffff / count + 1)];
            }
        }
    }
    return 1;
}

i32 Action_CreateSplineCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 unused6) {
    u16 angle;
    u16 pitch;
    i32 model_count;
    i32 spline_count;
    i32 use_selected;
    i32 min_group_size;
    i32 max_group_size;
    i32 group_size;
    f32 min_distance;
    f32 max_distance;
    f32 distance;
    i32 relative_to_player;
    i32 creature_set;
    i32 looping;
    struct AILOCATOR_PC *relative_locator;
    struct gameobject_s *rider;
    i32 i;
    struct nugspline_s *spline;
    i32 model;
    i16 models[10];
    struct nugspline_s *splines[32];
    char script_name[64] = "default";
    NUVEC spline_offset = {0.0f, 0.0f, 0.0f};
    if (!first_time || num_params <= 0)
        return 1;

    model_count = 0, spline_count = 0, use_selected = 0;
    min_group_size = -1, max_group_size = -1, group_size = 1;
    min_distance = 1000000000.0f, max_distance = 1000000000.0f, distance = 0.0f;
    relative_to_player = 0, creature_set = 0, looping = 0;
    relative_locator = NULL;
    rider = NULL;
    for (i = 0; i < num_params; i++) {
        char *value = NuStrIStr(params[i], "mingroupsize");
        if (value != NULL)
            min_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "maxgroupsize")) != NULL)
            max_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "groupsize")) != NULL)
            group_size = (i32)AIParamToFloat(processor, value + 10);
        else if ((value = NuStrIStr(params[i], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                u8 type = LevelCharacterTypeIDFn(value + 5);
                if (type != 0xff) {
                    i16 model = LevelCharacterGlobalIDFn(type);
                    if (model != -1 && model_count < 10)
                        models[model_count++] = model;
                }
            }
        } else if ((value = NuStrIStr(params[i], "script")) != NULL)
            NuStrCpy(script_name, value + 7);
        else if ((value = NuStrIStr(params[i], "mindistance")) != NULL)
            min_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], "maxdistance")) != NULL)
            max_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], (char *)RaceOpponentDistancePrefix)) != NULL)
            distance = AIParamToFloat(processor, value + 9);
        else if (script_spline_selected != NULL && NuStrICmp("use_selected_spline", params[i]) == 0)
            use_selected = 1;
        else if ((value = NuStrIStr(params[i], "splines=")) != NULL)
            spline_count +=
                NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[spline_count], 32 - spline_count);
        else if (NuStrIStr(params[i], "spline=myspline") != NULL) {
            if (processor->myspline != NULL)
                splines[spline_count++] = processor->myspline;
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757a84)) != NULL) {
            NUGSPLINE *spline = NuSplineFind(WORLD->current_gscn, value + 7);
            if (spline != NULL && spline_count < 32)
                splines[spline_count++] = spline;
        } else if ((value = NuStrIStr(params[i], "x_offset=")) != NULL)
            spline_offset.x = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "y_offset=")) != NULL)
            spline_offset.y = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "z_offset=")) != NULL)
            spline_offset.z = AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "looping") == 0)
            looping = 1;
        else if (NuStrIStr(params[i], "addtoset=myset") != NULL)
            creature_set = processor->creature_set;
        else if ((value = NuStrIStr(params[i], "addtoset=")) != NULL)
            creature_set = (i32)AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "relative_to_player") == 0)
            relative_to_player = 1;
        else if ((value = NuStrIStr(params[i], "relative_to_locator=")) != NULL)
            relative_locator = AIPathFindLocator(system, value + 20);
        else if (NuStrIStr(params[i], "ridden_by=myself") != NULL) {
            if (packet != NULL && packet->owner != NULL)
                rider = ((GAMEOBJECT *)packet->owner)->objptr;
        } else if ((value = NuStrIStr(params[i], "ridden_by=")) != NULL)
            rider = GetNamedGameObject(system, value + 10);
    }
    if (min_group_size < max_group_size && min_group_size >= 0)
        group_size = NuRand(NULL) % (max_group_size + 1 - min_group_size) + min_group_size;
    if (min_distance != 1000000000.0f && max_distance != 1000000000.0f) {
        f32 fraction = NuRandFloat();
        distance = max_distance * fraction + min_distance * (1.0f - fraction);
    }
    if ((spline_count | use_selected) == 0 || group_size <= 0 || model_count == 0)
        return 1;
    spline = use_selected ? script_spline_selected : splines[qrand() / (65535 / spline_count + 1)];
    model = models[NuRand(NULL) % model_count];
    for (i = 0; i < group_size; i++) {
        GAMEOBJECT *object = AddDynamicCreature(model, NULL, 0, script_name, NULL, NULL, 0, spline, &spline_offset,
                                                looping, creature_set);
        if (object == NULL)
            continue;
        if (relative_to_player || relative_locator != NULL) {
            SPLINEPOS position;
            i16 start = -1, end = -1;
            if (WORLD->current_level == PODSPRINTA_LDATA)
                PodSprint_GetIAlongVals(spline, &start, &end);
            if (relative_to_player) {
                GetNearestSplinePos(&player->collision_position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            } else if (relative_locator != NULL) {
                GetNearestSplinePos(&relative_locator->position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            }
        }
        if (distance != 0.0f) {
            NUVEC position;
            NUVEC offset;
            MoveSplinePosition(&object->movement_spline_position, distance);
            position = position, offset = offset;
            PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position,
                             &angle, &pitch, object->movement_spline_position.looping);
            object->movement_facing_angle = angle;
            object->facing_angle = angle;
            object->field_0x276 = angle;
            object->pitch_angle = -pitch;
            if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
                object->movement_spline_offset.z != 0.0f) {
                NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
                NuVecRotateY(&offset, &offset, object->field_0x276);
                NuVecAdd(&position, &position, &offset);
            }
            object->position = position;
            object->initial_position = position;
            object->collision_position = position;
            plr_lastpos = position;
            object->start_position = position;
            GameObjectOrigin(object);
            object->last_safe_position = object->position;
            object->ai_update_position = object->position;
        }
        if (rider != NULL)
            TakeOverGameObject(rider, object, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_SetCurrentSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    GAMEOBJECT *object;
    i32 speed_mode;
    f32 speed;
    char *value;
    APIOBJECT_PC *named;

    speed = 0.0f;
    speed_mode = -1;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (!first_time)
        return 1;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL) {
            named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
            object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
        } else if (NuStrICmp((char *)AITransferLiteral757bb4, params[i]) == 0)
            speed_mode = 2;
        else if (NuStrICmp((char *)AITransferLiteral757ba8, params[i]) == 0)
            speed_mode = 1;
        else if (NuStrICmp((char *)AITransferLiteral757b9c, params[i]) == 0)
            speed_mode = 0;
        else
            speed = AIParamToFloat(processor, params[i]);
    }
    if (object != NULL) {
        if (speed_mode == 2)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->tiptoe_speed;
        else if (speed_mode == 1)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->walk_speed;
        else if (speed_mode == 0) {
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->run_speed;
            object->field_dc8 = 1.0f;
        }
        object->velocity.x = 0.0f;
        object->velocity.y = 0.0f;
        object->velocity.z = speed;
        NuVecRotateY(&object->velocity, &object->velocity, object->facing_angle);
    }
    return 1;
}

i32 __cdecl Action_SetRunSpeed(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 num_params, i32 first_time, f32 elapsed) {
    union {
        f32 value;
        u32 bits;
    } absolute;

    GAMEOBJECT *object;
    i32 i;
    char *value;
    f32 speed = 1000000000.0f;
    f32 minimum = 0.0f;
    f32 maximum = 1000000000.0f;
    f32 multiplier = 1.0f;
    i32 multiply = 0;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time) {
        if (num_params != 0) {
            for (i = 0; i < num_params; i++) {
                value = NuStrIStr(params[i], (char *)AIControlMultiplyName);
                if (value != NULL) {
                    multiplier = AIParamToFloat(processor, value + 9);
                    multiply = 1;
                } else if (NuStrICmp(params[i], (char *)AIControlMaximumDefaultName) == 0)
                    maximum = object->character_data->game_character->run_speed;
                else if (NuStrICmp(params[i], (char *)GizmoDefaultOutputName) == 0)
                    speed = object->character_data->game_character->run_speed;
                else if (NuStrICmp(params[i], (char *)AIControlClearName) == 0) {
                } else if ((value = NuStrIStr(params[i], (char *)AIControlMaximumName)) != NULL)
                    maximum = AIParamToFloat(processor, value + 4);
                else if ((value = NuStrIStr(params[i], (char *)AIControlMinimumName)) != NULL)
                    minimum = AIParamToFloat(processor, value + 4);
                else if ((value = NuStrIStr(params[i], (char *)AIControlSeekName)) != NULL)
                    processor->action_data_4 = AIParamToFloat(processor, value + 5);
                else if (NuStrIStr(params[i], (char *)AIControlPlayerRunSpeedName) != NULL)
                    speed = player->character_data->game_character->run_speed;
                else
                    speed = AIParamToFloat(processor, params[i]);
            }
        }
        absolute.value = minimum;
        absolute.bits &= 0x7fffffffu;
        minimum = absolute.value;
        absolute.value = maximum;
        absolute.bits &= 0x7fffffffu;
        maximum = absolute.value;
        if (processor->action_data_4 == 0.0f) {
            if (multiply) {
                if (object->run_speed_override == MoveSplineRunSpeedSentinel)
                    object->run_speed_override = object->character_data->game_character->run_speed;
                object->run_speed_override = multiplier * object->run_speed_override;
                if (object->run_speed_override < 0.0f) {
                    if (object->run_speed_override < -maximum)
                        object->run_speed_override = -maximum;
                    else if (maximum > minimum && object->run_speed_override > -minimum)
                        object->run_speed_override = -minimum;
                } else {
                    if (object->run_speed_override > maximum)
                        object->run_speed_override = maximum;
                    else if (maximum > minimum && object->run_speed_override < minimum)
                        object->run_speed_override = minimum;
                }
            } else
                object->run_speed_override = speed;
            return 1;
        }
        if (multiply) {
            if (object->run_speed_override == MoveSplineRunSpeedSentinel)
                processor->action_data_5 = object->character_data->game_character->run_speed;
            processor->action_data_5 *= multiplier;
            if (processor->action_data_5 < 0.0f) {
                if (processor->action_data_5 < -maximum)
                    processor->action_data_5 = -maximum;
                else if (maximum > minimum && processor->action_data_5 > -minimum)
                    processor->action_data_5 = -minimum;
            } else {
                if (processor->action_data_5 > maximum)
                    processor->action_data_5 = maximum;
                else if (maximum > minimum && processor->action_data_5 < minimum)
                    processor->action_data_5 = minimum;
            }
        } else
            processor->action_data_5 = speed;
    }
    if (processor->action_data_4 > 0.0f && object->run_speed_override < MoveSplineRunSpeedSentinel) {
        object->run_speed_override =
            SeekValF(object->run_speed_override, processor->action_data_5, processor->action_data_4);
        absolute.value = object->run_speed_override - processor->action_data_5;
        absolute.bits &= 0x7fffffffu;
        if (absolute.value < 0.01f)
            object->run_speed_override = processor->action_data_5;
        else
            return 0;
    }
    return 1;
}

i32 __cdecl Action_SetWalkSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->walk_speed_override = 1000000000.0f;
        if (num_params != 0 && NuStrICmp(params[0], (char *)GizmoDefaultOutputName) != 0)
            object->walk_speed_override = AIParamToFloat(processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetHitPoints(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hitpoints;
    i32 set_max;
    char *value;
    GIZAIMESSAGE *message;

    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    hitpoints = -1;
    set_max = 1;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c20)) != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
            if (message != NULL)
                hitpoints = (i32)message->value;
        } else if (NuStrICmp(params[i], (char *)AITransferLiteral757c10) == 0) {
            set_max = 0;
        } else if (NuStrICmp(params[i], (char *)GizmoDefaultOutputName) != 0) {
            hitpoints = (i32)AIParamToFloat(processor, params[i]);
        }
    }
    if (object != NULL) {
        if (hitpoints == -1)
            hitpoints = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->hitpoints;
        if (set_max)
            object->field_107e = (u8)hitpoints;
        object->current_hp = (i8)hitpoints;
    }
    return 1;
}

i32 __cdecl Action_SetShieldHitPoints(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hit_points;
    char *value;

    if (first_time == 0)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    hit_points = -1;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
        } else
            hit_points = (i32)(AIParamToFloat(processor, params[index]));
    }
    if (object != NULL) {
        if (hit_points == -1)
            hit_points = object->character_data->game_character->shield_hitpoints;
        object->shield_hitpoints = (u8)(hit_points);
    }
    return 1;
}

i32 __cdecl Action_SetMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                              i32 count, i32 first_time, f32 unused6) {
    char *name = NULL;
    i32 operation = 0;
    f32 amount = 0.0f;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                name = value + 5;
            else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (name != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, name, NULL);
            if (message != NULL) {
                switch (operation) {
                    case -1:
                        message->value -= amount;
                        break;
                    case 0:
                        message->value = amount;
                        break;
                    case 1:
                        message->value = message->value + amount;
                        break;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetScriptParam(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    f32 amount = 0.0f;
    i32 operation = 0;
    i32 param_index = -1;
    i32 index;
    i32 candidate;
    char *value;
    if (first_time != 0 && count != 0 && processor->script != NULL) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += 5;
                for (candidate = 0; candidate < 4; ++candidate) {
                    if (NuStrICmp(processor->script->params[candidate].name, value) == 0) {
                        param_index = candidate;
                        break;
                    }
                }
            } else if ((value = NuStrIStr(params[index], "ix=")) != NULL) {
                param_index = (i32)AIParamToFloat(processor, value + 3);
                if (param_index >= 4)
                    param_index = -1;
            } else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (param_index >= 0) {
            switch (operation) {
                case -1:
                    processor->params[param_index] -= amount;
                    break;
                case 0:
                    processor->params[param_index] = amount;
                    break;
                case 1:
                    processor->params[param_index] = processor->params[param_index] + amount;
                    break;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CopyMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    char *from = NULL;
    char *to = NULL;
    i32 index;
    char *value;
    GIZAIMESSAGE *source;
    GIZAIMESSAGE *destination;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "from=");
            if (value != NULL)
                from = value + 5;
            else if ((value = NuStrIStr(params[index], "to=")) != NULL)
                to = value + 3;
        }
        if (from != NULL && to != NULL) {
            source = CheckGizAIMessage(gizaimessagesys, from, NULL);
            destination = CheckGizAIMessage(gizaimessagesys, to, NULL);
            if (source != NULL && destination != NULL)
                destination->value = source->value;
        }
    }
    return 1;
}

i32 __cdecl Action_AddPartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 elapsed) {
    if (first_time != 0) {
        NUSPECIALHANDLE special = {0};
        NUVEC position;
        i32 index;
        char *value;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75782c);
            if (value != NULL)
                NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
            else if ((value = NuStrIStr(params[index], (char *)ActionSnapXName)) != NULL)
                position.x = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[index], "y")) != NULL)
                position.y = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[index], (char *)ActionSnapZName)) != NULL)
                position.z = AIParamToFloat(processor, value + 2);
        }
        if (NuSpecialExistsFn(&special)) {
            NUVEC velocity;
            NUMTX matrix;
            ADDPART_PC part;
            MakeThrowVector(&velocity, &position, &player->collision_position, &player->velocity, GameDeflectPartSpeed,
                            GameDeflectPartGravity);
            NuMtxSetTranslation(&matrix, &position);
            part = Default_ADDPART;
            part.field_14 = part.field_18 = 0.1f;
            part.gravity = GameDeflectPartGravity;
            part.time_step = FRAMETIME;
            part.matrix = &matrix;
            part.velocity = &velocity;
            part.special = &special;
            part.flags = 0x29b;
            part.collision_fn = PartCollide_3D;
            AddPart(&part);
            NewRumble(player->pad->bound_pad, 0.5f, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_AddPartDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    NUVEC offset;
    NUVEC position;
    i32 type;
    char *value;
    GAMEOBJECT *object;
    type = -1;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    if (first_time) {
        // The original requires coordinates or a character parameter to supply this position.

        for (i = 0; i < num_params; i++) {
            value = NuStrIStr(params[i], (char *)AITransferLiteral75782c);
            if (value != NULL)
                type = PARTLookupType(value + 5);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c64)) != NULL)
                offset.x = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c60)) != NULL)
                offset.y = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c5c)) != NULL)
                offset.z = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)XArgPrefix)) != NULL)
                position.x = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)YArgPrefix)) != NULL)
                position.y = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)ZArgPrefix)) != NULL)
                position.z = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)AIGameCharacterName)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
                if (object != NULL)
                    position = object->collision_position;
            }
        }
        if (type != -1) {
            NuVecAdd(&position, &position, &offset);
            AddFiniteShotPART(type, &position, 1);
        }
    }
    return 1;
}

void GuidedMissile_Move(PART_PC *part, f32 time) {
    NUVEC position;
    NUVEC scale;
    GAMEOBJECT *recipient = part->recipient;
    if (part->scale_time > guided_life) {
        part->field_124[3] = (i32)SeekValF((f32)part->field_124[3], 0.0f, 2.0f);
    } else if (recipient != NULL && part->scale_time > guided_start_time) {
        NUVEC direction;
        i32 yaw, pitch;
        NuVecSub(&direction, &recipient->collision_position, &part->position);
        yaw = NuAtan2D(direction.x, direction.z);
        NuVecRotateY(&direction, &direction, -yaw);
        direction.y = recipient->collision_position.y + 0.5 - part->position.y;
        pitch = -NuAtan2D(direction.y, direction.z);
        part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 1.0f);
        part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 3.0f);
        part->field_124[3] = guided_rotate_speed;
    } else {
        part->field_124[3] = guided_rotate_speed;
    }
    part->rotation_z += (i32)((f64)part->field_124[3] * FRAMETIME);
    part->velocity.x = 0.0f;
    part->velocity.y = 0.0f;
    part->velocity.z = guided_speed;
    NuVecRotateX(&part->velocity, &part->velocity, part->rotation_x);
    NuVecRotateY(&part->velocity, &part->velocity, part->rotation_y);
    {
        position.x = part->position.x + part->velocity.x * time;
        position.y = part->position.y + part->velocity.y * time + part->gravity * time;
        position.z = part->position.z + part->velocity.z * time;
        NuMtxSetRotationX(&part->transform, NuAngAdd(part->rotation_x, 0x4000));
        NuMtxRotateY(&part->transform, part->rotation_y);
        NuMtxPreRotateY(&part->transform, part->rotation_z);
        NuMtxTranslate(&part->transform, &position);
        if (part->scale_time < 0.2f) {
            scale.x = scale.y = scale.z = part->scale_time / 0.2f;
            NuMtxPreScale(&part->transform, &scale);
        }
        if ((part->active & 0x80) != 0) {
            i32 count = ParticlesPerFrame(1.0f, FRAMETIME);
            NUVEC momentum;
            momentum.x = -part->velocity.x;
            momentum.y = -part->velocity.y;
            momentum.z = -part->velocity.z;
            AddGameDebrisMom(WORLD->game_debris_sys, 0xb, &position, count, &momentum);
        }
    }
}

void GuidedMissile_Kill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x7d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x21, &part->position);
    GameCam_Judder(GameCam, 0.2f, 0, NULL);
    Game_KillPart(part, reason);
    NewRumbleAllPlayers(0.7f, 0.1f, 0, 0);
}

void GuidedMissile_Deflect(PART_PC *part) {
    part->flags &= ~0x4000;
    part->flags |= 0x80;
    NewPartRotation(part);
}

i32 __cdecl Action_LaunchGuidedMissile(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    NUVEC offset = {0.0f, 0.0f, 0.0f};
    if (first_time != 0) {
        NUSPECIALHANDLE source = {0};
        i32 index;
        char *value;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "from=");
            if (value != NULL)
                NuSpecialFind(WORLD->current_gscn, &source, value + 5, 1);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c64)) != NULL)
                offset.x = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c60)) != NULL)
                offset.y = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c5c)) != NULL)
                offset.z = AIParamToFloat(processor, value + 3);
        }
        if (NuSpecialExistsFn(&source)) {
            NUMTX matrix = *NuSpecialGetDrawMtx(&source);
            NUVEC scale;
            NUVEC direction;
            NUVEC returned_scale;
            NUVEC centre;
            ADDPART_PC part;
            scale = *NuMtxGetScale(&returned_scale, &matrix);
            scale.x = 1.0f / scale.x;
            scale.y = 1.0f / scale.y;
            scale.z = 1.0f / scale.z;
            NuMtxPreScale(&matrix, &scale);
            if (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f)
                NuMtxTranslate(&matrix, &offset);
            direction.y = direction.x = 0.0f;
            direction.z = 1.0f;
            NuVecMtxRotate(&direction, &direction, &matrix);
            part = Default_ADDPART;
            part.matrix = &matrix;
            part.velocity = &direction;
            NuSpecialGetRadius(&WORLD->level_objects[232].special, &centre, &part.field_14);
            part.part_id = 232;
            part.field_14 *= test_missile_scale;
            part.field_18 = part.field_14;
            part.special = &WORLD->level_objects[232].special;
            part.time_step = FRAMETIME;
            part.gravity = 0.0f;
            part.flags = 0x831b;
            part.kill_fn = GuidedMissile_Kill;
            part.field_c0 = test_missile_scale;
            part.move_fn = GuidedMissile_Move;
            part.deflect_fn = GuidedMissile_Deflect;
            part.collision_fn = PartCollide_3D;
            part.recipient = player;
            AddPart(&part);
        }
    }
    return 1;
}

i32 __cdecl Action_SetHoverPhase(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if ((object->character_data->game_character)->hover_fall_mode != 2) {
        return 1;
    }

    if (param_5 != 0) {
        processor->action_data_1 = 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                processor->action_data_1 = 0;
            }
        }
    }

    if (processor->action_data_1 != 0) {
        if (object->jump_variant_state != 1) {
            object->pad->buttons_pressed |= GAMEPAD_JUMP;
            return 0;
        }
    } else if (object->jump_variant_state == 1) {
        object->pad->buttons_pressed |= GAMEPAD_JUMP;
        return 0;
    }
    return 1;
}

i32 __cdecl Action_UseCurrentSpeed(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    char *value;
    i32 snap;
    i32 index;

    snap = 0;
    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL) {
        object = ((APIOBJECT_PC *)packet->owner)->owner;
        if (!first_time)
            return 1;
        object->flags_ef4 |= 0x200000u;
        object->movement_speed_scale_pc = 1.0f;
        for (index = 0; index < num_params; index++) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x200000u;
            else if ((value = NuStrIStr(params[index], (char *)AIUseCurrentSpeedMultiplierName)) != NULL)
                object->movement_speed_scale_pc = AIParamToFloat(processor, value + 11);
            else if (NuStrICmp(params[index], (char *)AIUseCurrentSpeedSnapName) == 0)
                snap = 1;
        }
        if ((object->flags_ef4 & 0x200000u) != 0 && (object->portal_flags & 0x4000u) == 0) {
            object->portal_flags |= 0x4000u;
            if (!snap)
                return 1;
            ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        }
        if (snap)
            CurrentStart(object, 1, 1);
    }
    return 1;
}

i32 __cdecl Action_SetMaxMovementRange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    i32 index;
    i32 range_type;
    f32 range;
    i32 all_non_party;
    GAMEOBJECT *object;
    range = 0.0f;
    index = 0;
    all_non_party = 0;
    range_type = 1;
    if (first_time != 0) {
        for (; index < count; index++) {
            if (NuStrICmp((char *)AIControlDefaultTitleName, params[index]) == 0)
                range = DEFAULT_MOVE_RANGE;
            else if (NuStrICmp((char *)AIControlAllNonPartyName, params[index]) == 0)
                all_non_party = 1;
            else if (NuStrICmp((char *)AIControlLocatorName, params[index]) == 0)
                range_type = 2;
            else
                range = AIParamToFloat(processor, params[index]);
        }
        if (all_non_party != 0) {
            object = Obj;
            for (index = 0; index < HIGHGAMEOBJECT; index++, object++) {
                if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
                    (object->flags_1f8 & 0x400u) != 0) {
                    object->ai_packet.movement_target_radius = range;
                    if (range > 0.0f)
                        object->ai_packet.flags = (object->ai_packet.flags & ~0x08000000u) | 0x04000000u;
                    else
                        object->ai_packet.flags &= ~0x0c000000u;
                }
            }
        } else if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL) {
            object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
            object->ai_packet.movement_target_radius = range;
            if (range > 0.0f)
                object->ai_packet.flags = (object->ai_packet.flags & ~0x0c000000u) | ((range_type & 3) << 26);
            else
                object->ai_packet.flags &= ~0x0c000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_SetDefaultMovementRange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                           char **params, i32 count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time != 0) {
        for (index = 0; index < count; index++)
            DEFAULT_MOVE_RANGE = AIParamToFloat(processor, params[index]);
    }
    return 1;
}

i32 __cdecl Action_SetGravityHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    f32 minimum;
    f32 maximum;
    char *value;
    f32 random;

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || first_time == 0) {
        return 1;
    }
    object->portal_flags &= 0x7fffffffu;
    object->hover_height_override = 1.0e9f;

    minimum = 1.0e9f;
    maximum = 1.0e9f;
    for (index = 0; index < param_count; ++index) {
        if (NuStrICmp(params[index], (char *)AIScaleOverrideResetName) == 0) {
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMinimumName);
        if (value != NULL) {
            minimum = AIParamToFloat(processor, value + 4);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMaximumName);
        if (value != NULL) {
            maximum = AIParamToFloat(processor, value + 4);
        } else {
            object->hover_height_override = AIParamToFloat(processor, params[index]);
        }
    }
    if (minimum != 1.0e9 && maximum != 1.0e9) {
        random = NuRandFloat();
        object->hover_height_override = maximum * random + (1.0f - random) * minimum;
    }
    return 1;
}

i32 __cdecl Action_SetScaleOverride(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    f32 minimum;
    f32 maximum;
    f32 target;
    f32 time;
    f32 random;
    char *value;
    i32 index;

    minimum = 1000000000.0f;
    maximum = 1000000000.0f;
    target = 1000000000.0f;
    time = 0.0f;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time) {
        for (index = 0; index < num_params; index++) {
            if (NuStrICmp(params[index], (char *)AIScaleOverrideResetName) == 0)
                target = 1000000000.0f;
            else if ((value = NuStrIStr(params[index], (char *)AIControlMinimumName)) != NULL)
                minimum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIControlMaximumName)) != NULL)
                maximum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIActionTimeName)) != NULL)
                time = AIParamToFloat(processor, value + NuStrLen(AIActionTimeName) + 1);
            else
                target = AIParamToFloat(processor, params[index]);
        }
        if (time != 0.0f) {
            if (minimum != MoveSplineRunSpeedSentinel && maximum != MoveSplineRunSpeedSentinel) {
                random = NuRandFloat();
                target = (1.0f - random) * minimum + random * maximum;
            }
            processor->action_data_4 = time;
            processor->action_data_5 = (target - object->collision_scale_override) / time;
        } else {
            if (minimum != MoveSplineRunSpeedSentinel && maximum != MoveSplineRunSpeedSentinel) {
                random = NuRandFloat();
                object->collision_scale_override = (1.0f - random) * minimum + random * maximum;
            } else {
                object->collision_scale_override = target;
            }
            return 1;
        }
    }
    if (processor->action_data_4 > 0.0f) {
        if (FRAMETIME < processor->action_data_4) {
            object->collision_scale_override += FRAMETIME * processor->action_data_5;
            processor->action_data_4 -= FRAMETIME;
            return 0;
        }
        object->collision_scale_override += processor->action_data_5 * processor->action_data_4;
        processor->action_data_4 = 0.0f;
    }
    return 1;
}

i32 __cdecl Action_ApplyGravity(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags &= (u32)(~0x80000000u);
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags |= 0x80000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreShoveSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeForcedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 0x10000000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanTurn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 0x20000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~0x20000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_NoIdleSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            ((AI_ACTION_FLAGS_EF4_PC *)&object->flags_ef4)->bits.no_idle_speed = enabled;
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeSeen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 2;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~2u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetVisibility(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                                 i32 param_count, i32 first_time, float unused6) {
    i32 visible = 1;
    if (first_time != 0) {
        i32 index;
        NUSPECIALHANDLE special = {0};
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], (char *)"name");
            if (value != NULL) {
                NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
            } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                visible = 0;
            }
        }
        if (NuSpecialExistsFn(&special) != 0) {
            NuSpecialSetVisibility(&special, visible);
        }
    }
    return 1;
}

i32 __cdecl Action_EnableSock(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 sock_index;
    i32 enabled;
    (void)sys;
    (void)packet;
    (void)param_6;
    enabled = 1;
    sock_index = -1;
    if (param_5 != 0) {
        i32 index;
        for (index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], (char *)"ix=");
            if (value != NULL) {
                sock_index = (i32)(AIParamToFloat(processor, value + 3));
            } else if ((value = NuStrIStr(params[index], (char *)"name=")) != NULL) {
                SOCK_PC *sock = FindSock(WORLD->sock_sys, value + 5);
                if (sock != NULL) {
                    sock_index = sock - WORLD->sock_sys->socks;
                }
            } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                enabled = 0;
            }
        }
        if (enabled != 0) {
            SockOn(WORLD->sock_sys, sock_index);
        } else {
            SockOff(WORLD->sock_sys, sock_index);
        }
    }
    return 1;
}

i32 __cdecl Action_AddDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    i32 handle;
    NUVEC position;
    GAMEOBJECT *object;
    i32 debris_type;
    char *value;
    NUVEC *debris_position;

    (void)param_6;

    handle = -1;
    position.x = AIPositionUnsetPc;
    position.y = AIPositionUnsetPc;
    position.z = AIPositionUnsetPc;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;

    if (param_5 != 0) {
        debris_type = 33;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                position.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                position.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                position.z = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
                debris_type = FindGameDebris((APIDEBRISSYS_PC *)(perm_debrissys), value + 5);
            }
        }

        if (position.x == AIPositionUnsetPc && position.y == AIPositionUnsetPc && position.z == AIPositionUnsetPc) {
            if (object == NULL)
                return 1;
            debris_position = &object->collision_position;
        } else {
            debris_position = &position;
        }
        AddFiniteShotDebrisEffect(&handle, WORLD->game_debris_sys->entries[debris_type].effect, debris_position, 1);
    }
    return 1;
}

i32 Action_JudderGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    f32 time = 0.1f;
    i32 axis = 0;
    (void)sys;
    (void)packet;
    (void)param_6;
    if (param_5 != 0) {
        i32 index;
        for (index = 0; index < param_4; ++index) {
            if (NuStrIStr(params[index], "axis=x") != NULL) {
                axis = 0;
            } else if (NuStrIStr(params[index], "axis=y") != NULL) {
                axis = 1;
            } else if (NuStrIStr(params[index], "axis=z") != NULL) {
                axis = 2;
            } else {
                char *parameter = NuStrIStr(params[index], "time");
                if (parameter != NULL) {
                    time = AIParamToFloat(processor, parameter + 5);
                }
            }
        }
        if (axis == 2 && qrand() < 0x8000) {
            time = -time;
        }
        GameCam_Judder(GameCam, time, axis, NULL);
    }
    return 1;
}

i32 __cdecl Action_CameraShake(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    f32 amount;
    f32 duration;
    f32 speed;
    char *value;

    (void)sys;
    (void)packet;
    (void)param_6;
    amount = 1.0f;
    duration = 0.0f;
    speed = 1.0f;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757cd8);
            if (value != NULL) {
                amount = AIParamToFloat(processor, value + 4);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0)) != NULL) {
                duration = AIParamToFloat(processor, value + 5);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cc8)) != NULL) {
                speed = AIParamToFloat(processor, value + 6);
            }
        }
        GameCam_NewShake(GameCam, amount, duration, speed);
    }
    return 1;
}

i32 Action_ResetGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    if (param_5 != 0) {
        GameCam_Reset(GameCam);
    }
    return 1;
}

i32 Action_PlayCutScene(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                        i32 param_count, i32 first_time, float unused6) {
    char *name = NULL;
    if (first_time != 0) {
        i32 i;
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], "name");
            if (value != NULL) {
                name = value + 5;
            }
        }
        if (name != NULL) {
            NewCutScene(NULL, WORLD->cutscene_sys, name, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_SetLevelPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *path_name;
    char *value;
    GAMEOBJECT *object;
    AIPACKET_PC *object_packet;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 == 0 || sys->path_sys == NULL) {
        return 1;
    }

    path_name = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
        if (value != NULL) {
            path_name = value + 5;
        }
    }

    if (AISysSetLevelPath(sys, path_name) == 0) {
        return 1;
    }

    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object == NULL || (object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u)) {
            continue;
        }
        object_packet = &object->ai_packet;
        AISysCharacterSetPath(object_packet, sys->path_sys->active_path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, object_packet, 0xff, (i8)(object->field_281));
    }
    return 1;
}

i32 Action_SetHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                   i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 ignore_done = 0;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
            else if (NuStrIStr(params[index], (char *)AIActionIgnoreDoneFlagNamePc) != NULL)
                ignore_done = 1;
        }
        Hint_SetHintFromId(hint_id, 0, ignore_done);
    }
    return 1;
}

i32 Action_SetHintComplete(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
        }
        Hint_SetComplete(hint_id);
    }
    return 1;
}

i32 Action_CancelHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                      i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    Hint_CancelCurrent();
    return 1;
}

i32 __cdecl Action_DisableNarrowSocks(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    if (params != NULL && param_count != 0 && params[0] != NULL &&
        NuStrICmp((char *)AIActionFalseName, params[0]) == 0) {
        disable_narrow_socks = 0;
        return 1;
    }
    disable_narrow_socks = 1;
    return 1;
}

i32 __cdecl Action_CycleCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    i32 single_switch;
    i32 first_category;
    (void)sys;
    if (packet != NULL && packet->owner != NULL) {
        object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (object == NULL || FreePlay == 0 || (i8)object->field_280 == -1)
            return 1;

        single_switch = 0;
        if (param_5 != 0) {
            i32 index;
            processor->action_data_1 = (u8)CHARCATEGORYCOUNT;
            processor->action_data_2 = (u8)CHARCATEGORYCOUNT;

            for (index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"time");
                if (value != NULL) {
                    processor->action_data_4 = AIParamToFloat(processor, value + 5);
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"Category");
                if (value != NULL) {
                    const i32 category = CharCategory_FindByName(value + 9);
                    if (category != -1) {
                        if (processor->action_data_1 >= CHARCATEGORYCOUNT)
                            processor->action_data_1 = (u8)category;
                        else
                            processor->action_data_2 = (u8)category;
                        break;
                    }
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"min_speed=");
                if (value != NULL) {
                    processor->action_data_5 = AIParamToFloat(processor, value + 10);
                    continue;
                }

                if (NuStrICmp(params[index], "SingleSwitch") == 0)
                    single_switch = 1;
            }
        }

        if (object->character_context == 0xb)
            return 0;

        first_category = processor->action_data_1;
        if (first_category < CHARCATEGORYCOUNT) {
            if (processor->action_data_5 == 0.0f ||
                ((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >=
                    processor->action_data_5) {
                i32 second_category;
                if (CharCategory_IsCategory(object, first_category) != 0)
                    return 1;

                second_category = processor->action_data_2;
                if (second_category < CHARCATEGORYCOUNT && CharCategory_IsCategory(object, second_category) != 0)
                    return 1;
            }

            processor->action_timer += param_6;
            if (processor->action_timer > processor->action_data_4) {
                processor->action_timer = 0.0f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
            return 0;
        }

        if (processor->action_data_5 != 0.0f) {
            if (((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >= processor->action_data_5)
                return 1;
        } else {
            if (single_switch != 0)
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            return 1;
        }

        processor->action_timer += param_6;
        if (processor->action_timer > processor->action_data_4) {
            processor->action_timer = 0.0f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return 1;
}

static u32 ParseAIPathCnxFlagPc(char *name) {
    u32 flag;
    if (StartupGameFlag0 != 0)
        flag = AIPathParseIndyFlagPc(name);
    else if (StartupGameFlag1 != 0)
        flag = AIPathParseStarWarsFlagPc(name);
    else
        flag = AIPathParseBatmanFlagPc(name);
    if (flag != 0)
        return flag;
    if (NuStrICmp(name, "BLOCK") == 0)
        return 0x80000000;
    if (NuStrICmp(name, "BIGJUMP") == 0)
        return LEGO_AIPATHCNX_BIGJUMP;
    if (NuStrICmp(name, "REQUIRESPERMISSION") == 0)
        return LEGO_AIPATHCNX_REQUIRESPERMISSION;
    if (NuStrICmp(name, "NO_DESTINATION_CHECK") == 0)
        return LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
    return 0;
}

i32 __cdecl Action_CnxController(struct AISYS_PC *sys, struct AISCRIPTPROCESS_PC *processor, struct AIPACKET_PC *packet,
                                 char **params, i32 param_count, i32 first_time, f32 param_6) {
    struct AIPATH_PC *path;
    char *from;
    char *to;
    char *target_name;
    i32 target_type;
    i32 fake_animation_id;
    i32 gizmo_output;
    u32 on_flags;
    u32 off_flags;
    i32 both_ways;
    i32 check_visible;
    i32 on_obstacle_open;
    i32 off_obstacle_open;
    i32 index;
    WORLDINFO *world;
    struct AIPATHCNXCONTROLLER_s *controller;
    (void)packet;
    (void)param_6;
    path = NULL;
    from = NULL;
    to = NULL;
    target_name = NULL;
    target_type = 0;
    fake_animation_id = -1;
    gizmo_output = 0;
    on_flags = 0;
    off_flags = 0;
    both_ways = 0;
    check_visible = 0;
    on_obstacle_open = 0;
    off_obstacle_open = 0;
    if (first_time == 0) {
        return 1;
    }

    for (index = 0; index < param_count; ++index) {
        char *match;
        u32 *flag_target;
        i32 is_on;
        char *value;
        if (NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e40) != NULL) {
            continue;
        }
        match = NuStrIStr(params[index], (char *)("from="));
        if (match != NULL) {
            from = match + 5;
            continue;
        }
        match = NuStrIStr(params[index], (char *)("to="));
        if (match != NULL) {
            to = match + 3;
            continue;
        }
        match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e34);
        if (match != NULL) {
            path = AISysFindPath(sys, match + 9);
            continue;
        }
        if (NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e24) != NULL) {
            check_visible = 1;
            continue;
        }
        if (NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e18) != NULL) {
            both_ways = 1;
            continue;
        }

        match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e10);
        if (match != NULL) {
            flag_target = &on_flags;
            is_on = 1;
            value = match + 8;
        } else {
            match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e04);
            if (match == NULL) {
                if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757dfc)) != NULL) {
                    target_name = match + 4;
                    target_type = 0;
                    continue;
                }
                if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757df0)) != NULL) {
                    target_name = match + 9;
                    target_type = 1;
                    continue;
                }
                if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757de4)) != NULL) {
                    target_name = match + 8;
                    target_type = 2;
                    continue;
                }
                match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757dd8);
                if (match != NULL) {
                    fake_animation_id = (i32)(AIParamToFloat(processor, match + 11));
                    target_type = 5;
                    continue;
                }
                match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757dc8);
                if (match != NULL) {
                    gizmo_output = (i32)(AIParamToFloat(processor, match + 13));
                    continue;
                }
                if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757dc0)) != NULL) {
                    target_name = match + 6;
                    target_type = 3;
                } else if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757db4)) != NULL) {
                    target_name = match + NuStrLen(AIControllerLiteralPc00757db4);
                    target_type = 6;
                } else if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757dac)) != NULL) {
                    target_name = match + 7;
                    target_type = 4;
                } else if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757da4)) != NULL) {
                    target_name = match + 6;
                    target_type = 7;
                } else if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757d98)) != NULL) {
                    target_name = match + 9;
                    target_type = 8;
                } else if ((match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757d90)) != NULL) {
                    target_name = match + 6;
                    target_type = 9;
                }
                continue;
            }
            flag_target = &off_flags;
            is_on = 0;
            value = match + 9;
        }
        if (NuStrIStr(params[index], (char *)AIControllerLiteralPc00757d80) != NULL) {
            if (is_on)
                on_obstacle_open = 1;
            else
                off_obstacle_open = 1;
        } else if (NuStrIStr(params[index], (char *)AIControllerLiteralPc00757d70) != NULL) {
            if (is_on)
                on_obstacle_open = 0;
            else
                off_obstacle_open = 0;
        } else {
            *flag_target |= ParseAIPathCnxFlagPc(value);
        }
    }

    world = WORLD;
    controller = AIPathCnxControllerCreate(((AI_ACTION_CONTROLLER_WORLD_PC *)world)->control_system, world->ai_sys,
                                           path, from, to, target_type, target_name, fake_animation_id, gizmo_output);
    if (controller == NULL) {
        return 1;
    }
    controller->on_flags = on_flags;
    controller->off_flags = off_flags;
    controller->flags_word =
        (u16)((((((off_obstacle_open & 1) << 1 | (on_obstacle_open & 1)) << 1 | (check_visible & 1)) << 1 |
                (both_ways & 1))
               << 1) |
              (controller->flags_word & 0xffe1));

    for (index = 0; index < param_count; ++index) {
        char range[64];
        char *separator;
        char *end_text;
        i32 start_frame;
        i32 end_frame;
        char *match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757e40);
        if (match == NULL) {
            continue;
        }
        NuStrCpy(range, match + 10);
        separator = NuStrIStr(range, (char *)AIControllerLiteralPc00757d6c);
        if (separator == NULL) {
            continue;
        }
        *separator = '\0';
        end_text = separator + 2;
        start_frame = NuStrICmp(range, AIControllerLiteralPc00757d60) == 0   ? -2
                      : NuStrICmp(range, AIControllerLiteralPc00757d54) == 0 ? -1
                                                                             : (i32)(AIParamToFloat(processor, range));
        end_frame = NuStrICmp(end_text, AIControllerLiteralPc00757d60) == 0 ? -2
                    : NuStrICmp(end_text, AIControllerLiteralPc00757d54) == 0
                        ? -1
                        : (i32)(AIParamToFloat(processor, end_text));
        if (end_frame != -1 && end_frame != -2 && end_frame < start_frame) {
            continue;
        }
        AIPathCnxControllerSetOnRange(controller, start_frame, end_frame);
    }
    return 1;
}

i32 __cdecl Action_CnxHelper(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    f32 jump_off_dy;
    AIPATH_PC *path;
    char *from;
    char *to;
    void *grapple;
    i32 both_ways;
    i32 index;
    (void)packet;
    (void)param_6;
    jump_off_dy = 0.0f;
    path = NULL;
    from = NULL;
    to = NULL;
    grapple = NULL;
    both_ways = 0;
    if (param_5 == 0 || param_4 <= 0) {
        return 1;
    }

    for (index = 0; index < param_4; ++index) {
        char *value = NuStrIStr(params[index], (char *)"pathname=");
        if (value != NULL) {
            path = AISysFindPath(sys, value + strlen("pathname="));
            continue;
        }
        if (NuStrIStr(params[index], (char *)"bothways") != NULL) {
            both_ways = 1;
            continue;
        }
        value = NuStrIStr(params[index], (char *)"from=");
        if (value != NULL) {
            from = value + strlen("from=");
            continue;
        }
        value = NuStrIStr(params[index], (char *)"to=");
        if (value != NULL) {
            to = value + strlen("to=");
            continue;
        }
        value = NuStrIStr(params[index], (char *)"jump_off_dy=");
        if (value != NULL) {
            jump_off_dy = AIParamToFloat(processor, value + strlen("jump_off_dy="));
            continue;
        }
        value = NuStrIStr(params[index], (char *)"grapple=");
        if (value != NULL) {
            GIZMO_PC *gizmo =
                (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, grapple_gizmotype_id, value + strlen("grapple="));
            if (gizmo != NULL) {
                grapple = gizmo->object;
            }
        }
    }

    if (from != NULL && to != NULL) {
        i32 direction = 0;
        AIPATHCNX_PC *connection = (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, path, from, to, &direction));
        if (connection != NULL) {
            if (both_ways) {
                direction = 0xff;
            }
            if (grapple != NULL) {
                AI_HELPER_SLOT_PC *helper =
                    AIPathCnxHelperSys_AddHelper(WORLD->ai_connection_helpers, connection, (u8)direction, grapple, 1);
                if (helper != NULL) {
                    helper->jump_off_dy = jump_off_dy;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    AIPATHSYS_PC *path_system;
    GAMEOBJECT *object;
    AIPATH_PC *path;
    char *value;
    APIOBJECT_PC *named;
    i32 index, path_index;

    (void)param_6;
    path = NULL;
    object = NULL;
    if (param_5 == 0) {
        return 1;
    }
    if (sys == NULL) {
        return 1;
    }
    if (sys->path_sys == NULL) {
        return 1;
    }

    path_system = sys->path_sys;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
            } else if (NuStrIStr(params[index], (char *)AITransferLiteral757e70) != NULL) {
                path = sys->path_sys->active_path;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757e68)) != NULL) {
                value += 5;
                for (path_index = 0; path_index < path_system->path_count; ++path_index) {
                    if (NuStrICmp(path_system->paths[path_index]->name, value) == 0) {
                        path = path_system->paths[path_index];
                        break;
                    }
                }
            }
        }
    }
    if (object != NULL && path != NULL) {
        AISysCharacterSetPath(&object->ai_packet, path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    return 1;
}

i32 __cdecl Action_GoToLevelPath(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    AIPATHNODE_PC *node;
    NUVEC difference;
    f32 distance_squared;
    u16 connection_index;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    if (first_time != 0) {
        if (system != NULL && packet->path_info.path != NULL && packet->path_info.connection != NULL) {
            if (packet->path_info.path != system->path_sys->active_path) {
                AISysCharacterSetPath(packet, system->path_sys->active_path);
                AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)packet->owner, packet, 0xff, 1);
            }
            return 1;
        }
    } else {
        node = (AIPATHNODE_PC *)processor->action_data_3;
        if (node != NULL) {
            distance_squared = NuVecXZDistSqr(&packet->owner->position, &node->position, &difference);
            if (distance_squared < node->radius_squared) {
                memset(&packet->path_info, 0, sizeof(packet->path_info));
                AISysCharacterSetPath(packet, system->path_sys->active_path);
                if ((node->runtime_flags & 1) != 0) {
                    connection_index = (u16)node->path_flags;
                    AISysCharacterSetPathCnx(packet, &packet->owner->position,
                                             &packet->path_info.path->connections[connection_index], 0);
                }
                return 1;
            }
            AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                              packet->movement_instruction_parameter);
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromLastAttacker(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                            char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    GAMEOBJECT *attacker;

    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], (char *)AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
            }
        }
    }

    attacker = object->last_attacker;
    if (attacker != NULL) {
        AIMoveInstruction(packet, &attacker->ai_packet.last_path_position, attacker->ai_packet.mover_height,
                          &attacker->ai_packet.path_info, 2, packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &object->last_attacker->position;
        }
    }
    return 0;
}

i32 __cdecl Action_GoToOriginalPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    GAMEOBJECT *object;
    AIPATH_PC *original_path;
    AIPATH_PC *current_path;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *target_node;
    AIPATHCNX_PC *connection;
    AIPATHINFO_PC *move_path;
    i32 index;
    i32 node_index;
    f32 distance;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;
    if (sys == NULL || sys->path_sys == NULL || (object->flags_1f8 & 0x400) == 0 ||
        object->ai_packet.creature_index == 0xff) {
        return 0;
    }
    original_path = sys->creatures[packet->creature_index].path_info.path;
    current_path = packet->path_info.path;
    if (first_time != 0) {
        if (current_path == original_path) {
            return 1;
        }
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL) {
                AIActionParseSpeedFn(params[index], &packet->goal_speed_mode);
            }
        }
        current_path = packet->path_info.path;
        packet->goal_path_node = NULL;
        if (current_path == NULL) {
            return 0;
        }
        if (current_path != sys->path_sys->active_path || packet->path_info.connection == NULL ||
            (original_path->flags & 2) == 0) {
            return 0;
        }
        node = original_path->nodes;
        node_index = 0;
        while (node_index < original_path->node_count && (node->runtime_flags & 1) == 0) {
            ++node_index;
            ++node;
        }
        if (node_index >= original_path->node_count) {
            return 1;
        }
        processor->action_data_3 = node;
        AISysCharacterSetPath(packet, current_path);
        connection = &sys->path_sys->active_path->connections[(u16)node->path_flags];
        move_path = &processor->path_info;
        target_node = &move_path->path->nodes[connection->node_indices[0]];
        move_path->connection = connection;
        move_path->direction = 0;
        move_path->on_path = 1;
        difference.x = node->position.x - target_node->position.x;
        difference.z = node->position.z - target_node->position.z;
        NuVecRotateY(&difference, &difference, -connection->rotation);
        move_path->distance = difference.z / connection->max_horizontal_distance;
        move_path->width = difference.x;
        AIMoveInstruction(packet, &node->position, 0.0f, move_path, 1, packet->movement_instruction_parameter);
        return 0;
    }
    node = (AIPATHNODE_PC *)processor->action_data_3;
    if (node == NULL) {
        return 0;
    }
    distance = NuVecXZDistSqr(&packet->terrain_origin, &node->position, &difference);
    if (distance < node->radius_squared) {
        memset(&packet->path_info, 0, sizeof(packet->path_info));
        AISysCharacterSetPath(packet, original_path);
        AISysCharacterSetPathCnx(packet, &packet->owner->position, packet->fallback_path_info.connection, 0);
        return 1;
    }
    AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1, packet->movement_instruction_parameter);
    return 0;
}

i32 __cdecl Action_SetAnimation(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    i16 animation;
    (void)system;
    (void)processor;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0 && count == 1) {
        animation = FindAnimIX(object->character_data, params[0]);
        if (animation != -1)
            ResetAnimPacket(&object->anim_packet, animation);
    }
    return 1;
}

i32 __cdecl Action_AnimTimeRandom(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    }
    return 1;
}

i32 __cdecl Action_AlwaysTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 enabled;
    char *value;
    GIZMO_PC *gizmo;
    obstacle = NULL;
    enabled = 1;
    if (first_time != 0) {
        if (param_count != 0) {
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
                if (value != NULL) {
                    gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
                    if (gizmo != NULL) {
                        obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
                    }
                } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    enabled = 0;
                }
            }
            if (obstacle != NULL) {
                obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~4u) | ((enabled & 1) << 2));
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 blocked;
    char *value;
    GIZMO_PC *gizmo;
    obstacle = NULL;
    blocked = 0;
    if (first_time != 0) {
        if (param_count != 0) {
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
                if (value != NULL) {
                    gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
                    if (gizmo != NULL) {
                        obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
                    }
                } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    blocked = 1;
                }
            }
            if (obstacle != NULL) {
                obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~8u) | ((blocked & 1) << 3));
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PlayGizObstacle(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 delta) {
    GIZMO_PC *gizmo;
    char *value;
    i32 index, backwards = 0, stay_open = 0, stay_shut = 0, snap = 0;
    GIZOBSTACLE *obstacle = NULL;
    (void)system;
    (void)processor;
    (void)packet;
    (void)delta;
    if (!first_time || count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL)
                obstacle = (GIZOBSTACLE *)gizmo->object;
        } else if (NuStrICmp(params[index], "backwards") == 0)
            backwards = 1;
        else if (NuStrICmp(params[index], "stayopen") == 0)
            stay_open = 1;
        else if (NuStrICmp(params[index], "stayshut") == 0)
            stay_shut = 1;
        else if (NuStrICmp(params[index], "SNAP") == 0)
            snap = 1;
    }
    if (obstacle != NULL) {
        if (backwards || stay_shut) {
            if (snap)
                GizObstacle_JumpToStart(obstacle);
            else
                GizObstacle_PlayBackwards(obstacle);
        } else if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
        obstacle->runtime_flags =
            (obstacle->runtime_flags & ~0xcu) | ((u32)(stay_open & 1) << 2) | ((u32)(stay_shut & 1) << 3);
    }
    return 1;
}

i32 __cdecl Action_PlayGizSpecial(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZSPECIAL_PARTIAL_PC *special;
    i32 backwards;
    i32 snap;
    i32 restart;
    char *value;
    GIZMO_PC *gizmo;

    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && param_4 > 0) {
        special = NULL;
        backwards = 0;
        snap = 0;
        restart = 0;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, value + 5);
                if (gizmo != NULL) {
                    special = (GIZSPECIAL_PARTIAL_PC *)(gizmo->object);
                }
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757ea8) == 0) {
                backwards = 1;
            } else if (NuStrICmp(params[index], (char *)GizmoSnapOptionNamePc) == 0) {
                snap = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757eb4) == 0) {
                restart = 1;
            }
        }

        if (special == NULL) {
            return 1;
        }
        if (backwards != 0) {
            if (snap != 0) {
                GameAnimSet_JumpToStart(special->anim_set);
            } else {
                if (restart != 0) {
                    GameAnimSet_JumpToEnd(special->anim_set);
                }
                GameAnimSet_Play(special->anim_set, -1.0f, 1);
            }
        } else if (snap != 0) {
            GameAnimSet_JumpToEnd(special->anim_set);
        } else {
            if (restart != 0) {
                GameAnimSet_JumpToStart(special->anim_set);
            }
            GameAnimSet_Play(special->anim_set, 1.0f, 1);
        }
    }
    return 1;
}

i32 Action_SetObstacleToEnd(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    GIZOBSTACLE *obstacle = NULL;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && param_4 != 0) {
        i32 index;
        for (index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], "name=");
            if (value != NULL) {
                obstacle = GizObstacle_FindByName(WORLD->giz_obstacle_sys, value + 5);
            }
        }
        if (obstacle != NULL) {
            GizObstacle_JumpToEnd(obstacle);
        }
    }
    return 1;
}

i32 __cdecl Action_CanOpenDoors(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_1044 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 1u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootOffScreen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        object->flags_1044 |= 4u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 4u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~4u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_KeepWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 keep_out = 1;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (param_4 != 0) {
            for (index = 0; index < param_4; ++index) {
                if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    keep_out = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        lookup =
                            GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                        object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                }
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x10u) | ((keep_out & 1) << 4);
        }
    }
    return 1;
}

i32 __cdecl Action_SnapWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    i32 keep_out;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    enabled = 1;
    keep_out = -1;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ec4)) != NULL) {
                keep_out = NuStrICmp(value + 9, (char *)AITransferLiteral757ebc) == 0;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
    }

    if (object != NULL) {
        if (enabled != 0) {
            object->flags_e20 |= 0x10000u;
            object->weapon_scale = 1.0f;
            if (keep_out != -1) {
                object->portal_flags = (u32)((object->portal_flags & ~0x10u) | (keep_out ? 0x10u : 0));
            }
        } else {
            object->weapon_scale = 0.0f;
            object->flags_e20 &= (u32)(~0x10000u);
            object->portal_flags &= (u32)(~0x10u);
        }
    }
    return 1;
}

i32 __cdecl Action_ResetContext(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (count != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "character=");
            if (value != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = found != NULL ? found->owner : NULL;
            }
        }
    }
    if (object != NULL) {
        Player_ClearContext(object, 1);
        Player_ResetContexts(&((AI_CONTEXT_RESET_VIEW_PC *)object)->reset_contexts);
    }
    return 1;
}

i32 __cdecl Action_PrefersPlayers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x40000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoltsDontGetDeflectedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~8u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootObstructions(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x10000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_UseBigJumpToJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef4 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreTurnAroundSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x80000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x80000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanMoveWhenDeactivated(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 1;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 2u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanPullLevers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x800000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~0x800000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanHitForceObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x40;
        }
    }
    return 1;
}

i32 __cdecl Action_AlwaysBackFlip(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x100u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x100u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PlayerSpeederHack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef8_word &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetAnimSpeedMul(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    f32 multiplier;
    f32 minimum;
    f32 maximum;
    f32 product;
    char *value;
    i32 index;

    multiplier = 1.0f;
    minimum = 0.0f;
    maximum = 1000000000.0f;
    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL) {
        object = ((APIOBJECT_PC *)packet->owner)->owner;
        if (num_params == 0)
            return 1;
        for (index = 0; index < num_params; index++) {
            value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName);
            if (value != NULL)
                object->animation_speed_multiplier = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedMultiplyByName)) != NULL)
                multiplier = AIParamToFloat(processor, value + 12);
            else if ((value = NuStrIStr(params[index], (char *)AIControlMaximumName)) != NULL)
                maximum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIControlMinimumName)) != NULL)
                minimum = AIParamToFloat(processor, value + 4);
        }
        if (multiplier != 1.0f) {
            product = object->animation_speed_multiplier * multiplier;
            object->animation_speed_multiplier = product;
            if (maximum < product)
                object->animation_speed_multiplier = maximum;
            else if (minimum > product)
                object->animation_speed_multiplier = minimum;
        }
    }
    return 1;
}

i32 __cdecl Action_CatchUpForbidden(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 forbidden;
    char *parameter;

    (void)processor;
    (void)param_6;
    object = NULL;
    forbidden = 1;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        if (param_4 != 0) {
            for (index = 0; index < param_4; ++index) {
                parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (parameter != NULL) {
                    {
                        lookup =
                            GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, parameter + 10) : NULL;
                        object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    forbidden = 0;
                }
            }
        }
        if (object != NULL) {
            object->portal_flags = (u32)((object->portal_flags & ~0x2000000u) | ((forbidden & 1) << 25));
        }
    }
    return 1;
}

i32 __cdecl Action_SetTaggable(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, disabled = 0;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (count != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = found != NULL ? found->owner : NULL;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
                target = found != NULL ? found->owner : NULL;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                disabled = 1;
            }
        }
    }
    if (object != NULL) {
        if (disabled && (object->flags_1fc_word & 0x80) != 0) {
            if (object->takeover_source != NULL)
                ReleaseTakeOver(object, 0);
            else if (TagCharacter(object, target, 0) == 0)
                disabled = 0;
            SetPlayer();
        }
        object->tag_flags = (object->tag_flags & ~2U) | ((u32)disabled << 1);
    }
    return 1;
}

i32 __cdecl Action_CannotDropIn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
                GetNamedGameObject(sys, value + 7);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x4000000u) | ((enabled & 1) << 26);
        }
    }
    return 1;
}

i32 __cdecl Action_NotWithParty(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (object->flags_ef0 & ~0x1000000u) | ((enabled & 1) << 24);
        }
    }
    return 1;
}

i32 __cdecl Action_CanAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x200u) | ((enabled & 1) << 9);
        }
    }
    return 1;
}

i32 __cdecl Action_TakeDamage(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *hitter, *candidate;
    APIOBJECT_PC *found;
    char *value;
    i32 index, damage, creature_set;
    (void)delta;
    object = NULL;
    hitter = NULL;
    damage = 1;
    creature_set = 0;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (count != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "character=");
            if (value != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = found != NULL ? found->owner : NULL;
            } else if ((value = NuStrIStr(params[index], "hitter=")) != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
                hitter = found != NULL ? found->owner : NULL;
            } else if ((value = NuStrIStr(params[index], "damage=")) != NULL) {
                damage = (i32)AIParamToFloat(processor, value + 7);
            } else if (NuStrIStr(params[index], "opponent") != NULL) {
                if (packet != NULL && packet->opponent != NULL) {
                    if (packet->opponent->objptr != NULL) {
                        object = packet->opponent->objptr;
                        if (packet->owner != NULL && packet->owner->objptr != NULL)
                            hitter = packet->owner->objptr;
                    }
                }
            } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
                creature_set = (i32)AIParamToFloat(processor, value + 4);
                if (creature_set < 0 || creature_set > 16)
                    creature_set = 0;
            }
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc & 1) != 0 && candidate->ai_packet.creature_set == creature_set)
                ObjHitObj(hitter, candidate, damage, 0, 0, 1);
        }
    } else if (object != NULL) {
        ObjHitObj(hitter, object, damage, 0, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_TagCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            target = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        TagCharacter(object, target, 0);
        SetPlayer();
    }
    return 1;
}

i32 __cdecl Action_UseForce(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    GIZFORCE_PC *force, *candidates[16];
    AI_FORCE_FIELDS_PC *view;
    GIZMO_PC *gizmo;
    char *name;
    i32 index, candidate_count = 0, triggered_by_hit = 0, valid_category;
    (void)system;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        processor->action_data_3 = NULL;
        for (index = 0; index < count; ++index) {
            if (NuStrICmp(params[index], "throwable") == 0)
                processor->action_data_1 = 1;
            else if (NuStrICmp(params[index], "inrange") == 0)
                processor->action_data_2 = 1;
            else if (NuStrICmp(params[index], "triggered_by_hit") == 0)
                triggered_by_hit = 1;
            else {
                name = NuStrIStr(params[index], "name");
                name = name != NULL ? name + 5 : params[index];
                if (name != NULL && candidate_count < 16) {
                    gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
                    if (gizmo != NULL && gizmo->object != NULL) {
                        force = (GIZFORCE_PC *)gizmo->object;
                        candidates[candidate_count] = force;
                        view = (AI_FORCE_FIELDS_PC *)force;
                        if ((view->runtime_flags & 2) && !(view->runtime_flags & 0x10000))
                            ++candidate_count;
                    }
                }
            }
        }
        if (candidate_count != 0)
            processor->action_data_3 = candidates[qrand() / (0xffff / candidate_count + 1)];
    }
    force = (GIZFORCE_PC *)processor->action_data_3;
    if (force == NULL)
        return 1;
    view = (AI_FORCE_FIELDS_PC *)force;
    if (!(view->runtime_flags & 1))
        return 0;
    valid_category = CharCategory_IsCategory(object, (view->config_flags & 0x10) ? 1 : 0) != 0;
    if (triggered_by_hit) {
        view->runtime_flags |= 0x8000;
        return 0;
    }
    if (valid_category) {
        packet->movement_look_target = &view->position;
        object->pad->runtime_flags |= 4;
        ((AI_FORCE_OBJECT_PC *)object)->force = force;
        if (processor->action_data_1 != 0) {
            if (packet->nearest_opponent == NULL) {
                object->pad->runtime_flags &= ~4;
                ((AI_FORCE_OBJECT_PC *)object)->force = NULL;
            }
            return 1;
        }
        return GizForce_Complete(force) != 0;
    }
    if (FreePlay != 0) {
        processor->action_timer -= delta;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return 0;
}

i32 __cdecl Action_TriggerBlowUp(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZMOBLOWUP_PC *blowup;
    char *value;
    GIZMO_PC *gizmo;
    blowup = NULL;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, value + 5);
                if (gizmo != NULL) {
                    blowup = (GIZMOBLOWUP_PC *)(gizmo->object);
                }
            }
        }
        if (blowup != NULL && (blowup->status_flags & 1) == 0) {
            GizmoBlowupBlowup(blowup, 1, -1, 1, NULL, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_ForcePush(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *action_object;
    i32 index;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;
    if (first_time != 0) {
        processor->action_data_3 = NULL;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], "target=opponent") == 0) {
                processor->action_data_3 = packet->opponent;
                if (processor->action_data_3 == NULL) {
                    return 1;
                }
            } else {
                char *value = NuStrIStr(params[index], "target=");
                if (value != NULL) {
                    processor->action_data_3 = GetNamedGameObject(system, value + 7);
                    if (processor->action_data_3 == NULL) {
                        return 1;
                    }
                } else if (NuStrICmp(params[index], "forcepush_highlight_only") == 0) {
                    processor->action_data_1 = 1;
                }
            }
        }
        if (object->character_context == 0x1b && processor->action_data_1 == 0 && object->force_target != NULL &&
            processor->action_data_3 == NULL) {
            object->pad->runtime_flags |= 0x10;
            object->field_d20 = (u32)object->force_target;
            object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
            object->flags_e20 &= ~0x20000u;
            object->character_context = -1;
            FindForcePushTarget(object, 0, 0);
        }
    }
    if (CharCategory_IsCategory(object, 0) == 0) {
        if (FreePlay != 0) {
            f32 timer = processor->action_timer - elapsed;
            processor->action_timer = timer;
            if (timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return 0;
    }
    object->pad->runtime_flags |= 0x10;
    object->field_d20 = (u32)processor->action_data_3;
    object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
    memcpy(&action_object, &processor->action_data_4, sizeof(action_object));
    if (action_object != NULL) {
        return action_object != object->force_target;
    }
    if (object->character_context == 0x1b && object->force_target != NULL) {
        memcpy(&processor->action_data_4, &object->force_target, sizeof(object->force_target));
    }
    return 0;
}

i32 __cdecl Action_DeflectPlayersPart(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || !first_time)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x100u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x100u;
        }
    }
    return 1;
}

i32 __cdecl Action_SetPathCnxFlag(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 param_6) {
    char *from;
    char *to;
    u32 add_flags;
    u32 remove_flags;
    i32 set;
    i32 both_ways;
    i32 index;
    i32 direction;
    (void)processor;
    (void)packet;
    (void)param_6;
    from = NULL;
    to = NULL;
    add_flags = 0;
    remove_flags = 0;
    set = 1;
    both_ways = 0;
    direction = 0;
    if (sys == NULL || sys->path_sys == NULL || sys->path_sys->path_count == 0 || first_time == 0) {
        return 1;
    }

    for (index = 0; index < param_count; ++index) {
        u32 flag;
        char *match = NuStrIStr(params[index], (char *)"from");
        if (match != NULL) {
            from = match + 5;
            continue;
        }
        match = NuStrIStr(params[index], (char *)AIControllerLiteralPc00757d6c);
        if (match != NULL) {
            to = match + 3;
            continue;
        }
        flag = ParseAIPathCnxFlagPc(params[index]);
        if (flag != 0) {
            add_flags |= flag;
            if (flag == (u32)LEGO_AIPATHCNX_JUMP_NOW) {
                remove_flags |= (u32)LEGO_AIPATHCNX_DONT_JUMP_NOW;
            } else if (flag == (u32)LEGO_AIPATHCNX_DONT_JUMP_NOW) {
                remove_flags |= (u32)LEGO_AIPATHCNX_JUMP_NOW;
            } else if (flag == 0x20000000u) {
                both_ways = 1;
            }
            continue;
        }
        if (NuStrICmp(params[index], "bothways") == 0) {
            both_ways = 1;
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            set = 0;
        }
    }

    if (from != NULL && to != NULL) {
        AIPATHCNX_PC *connection =
            (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, sys->path_sys->active_path, from, to, &direction));
        if (connection != NULL) {
            if (set) {
                connection->traversal_flags[direction] |= add_flags;
                connection->traversal_flags[direction] &= ~remove_flags;
            } else {
                connection->traversal_flags[direction] &= ~add_flags;
            }
            if (both_ways) {
                if (set) {
                    connection->traversal_flags[!direction] |= add_flags;
                    connection->traversal_flags[!direction] &= ~remove_flags;
                } else {
                    connection->traversal_flags[!direction] &= ~add_flags;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PlaySfx(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_count, i32 first_time, f32 delta_time) {
    NUVEC position;
    NUVEC *position_ptr;
    char *name;
    char play_count;
    f32 repeat_delay;
    f32 start_delay;
    i32 index;

    (void)packet;
    (void)delta_time;
    if (first_time == 0 || param_count <= 0)
        return 1;

    position.x = 1.0e9f;
    position.y = 1.0e9f;
    position.z = 1.0e9f;
    position_ptr = NULL;
    name = NULL;
    play_count = 1;
    repeat_delay = 0.0f;
    start_delay = 0.0f;

    for (index = 0; index < param_count; ++index) {
        char *value;

        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            name = value + 5;
            continue;
        }
        value = NuStrIStr(params[index], (char *)PlayCountArgPrefix);
        if (value != NULL) {
            play_count = (char)AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 10);
            continue;
        }
        value = NuStrIStr(params[index], (char *)RepeatDelayArgPrefix);
        if (value != NULL) {
            repeat_delay = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 9);
            continue;
        }
        value = NuStrIStr(params[index], (char *)StartDelayArgPrefix);
        if (value != NULL) {
            start_delay = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 11);
            continue;
        }
        value = NuStrIStr(params[index], (char *)XArgPrefix);
        if (value != NULL) {
            position.x = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)YArgPrefix);
        if (value != NULL) {
            position.y = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)ZArgPrefix);
        if (value != NULL) {
            position.z = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)CharacterPosArgPrefix);
        if (value != NULL) {
            GAMEOBJECT *object;
            object = GetNamedGameObject(sys, value + 14);
            position_ptr = &object->collision_position;
        }
    }

    if (position_ptr == NULL && (position.x != 1.0e9f || position.y != 1.0e9f || position.z != 1.0e9f)) {
        position_ptr = &position;
    }
    if (name != NULL) {
        PlayRepeatSfx(name, -1, start_delay, (i32)play_count, repeat_delay, position_ptr);
    }
    return 1;
}

i32 Action_CameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                     i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_delta;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    i32 follow_target;
    i32 borders;
    i32 use_current_camera;
    f32 start;
    f32 end;
    f32 blend_in;
    f32 blend_out;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_delta.x = 0.0f;
    camera_delta.y = 0.0f;
    camera_delta.z = 0.0f;
    range = 1.0f;
    rotation_x = 0;
    rotation_y = 0;
    target_locator = NULL;
    target_character = NULL;
    target_object = NULL;
    camera_locator = NULL;
    follow_target = 0;
    borders = 1;
    use_current_camera = 0;
    start = 0.0f;
    end = 1000000000.0f;
    blend_in = 0.0f;
    blend_out = 0.0f;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp((char *)AITransferLiteral757a68, params[index]) == 0) {
                GameCameraMakeMiniCut2(NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0);
                return 1;
            } else if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
                use_current_camera = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
                start = AIParamToFloat(processor, value + 11);
            } else if (NuStrICmp((char *)AICameraLiteral758070, params[index]) == 0) {
                end = 1000000000.0f;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
                end = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
                blend_in = AIParamToFloat(processor, value + 14);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
                blend_out = AIParamToFloat(processor, value + 15);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
                target_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
                target_character = value + 14;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
                target_object = value + 8;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
                camera_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                range = AIParamToFloat(processor, value + 6);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
                rotation_y = (i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
                rotation_x = -(i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
            } else if (NuStrICmp(params[index], (char *)AICameraLiteral757ffc) == 0) {
                follow_target = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
                camera_delta.x = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
                camera_delta.y = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
                camera_delta.z = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
                camera_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
                camera_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
                camera_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
                target_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
                target_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
                target_position.z = AIParamToFloat(processor, value + 9);
            } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
                borders = 0;
            }
        }

        target = NULL;
        if (target_locator != NULL) {
            locator = AIPathFindLocator(system, target_locator);
            if (locator == NULL)
                return 1;
            target = &locator->position;
        } else if (target_character != NULL) {
            if (packet != NULL && packet->owner != NULL &&
                NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0) {
                object = packet->owner->objptr;
            } else {
                APIOBJECT_PC *named;
                if (GetNamedGameObjectCallback == NULL)
                    return 1;
                named = GetNamedGameObjectCallback(system, target_character);
                if (named == NULL)
                    return 1;
                object = (GAMEOBJECT *)named->owner;
            }
            if (object == NULL)
                return 1;
            target = &object->collision_position;
        } else if (target_object != NULL) {
            if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) == 0)
                return 1;
            target = NuSpecialGetDrawPos(&special);
        } else {
            if (target_position.x == 1000000000.0f || target_position.y == 1000000000.0f ||
                target_position.z == 1000000000.0f)
                return 1;
            target = &target_position;
        }
        if (target == NULL)
            return 1;

        if (use_current_camera != 0) {
            matrix = NuCameraGetMtx();
            if (matrix != NULL) {
                camera_position = matrix->translation;
                NuVecAdd(&camera_position, &camera_position, &camera_delta);
                goto camera_ready;
            }
        } else {
            if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
                camera_position.z != 1000000000.0f)
                goto camera_ready;
            if (camera_locator != NULL) {
                locator = AIPathFindLocator(system, camera_locator);
                if (locator != NULL) {
                    camera_position = locator->position;
                    goto camera_ready;
                }
            }
        }
        camera_position.x = 0.0f;
        camera_position.y = 0.0f;
        camera_position.z = range;
        NuVecRotateX(&camera_position, &camera_position, rotation_x);
        NuVecRotateY(&camera_position, &camera_position, rotation_y);
        NuVecAdd(&camera_position, &camera_position, target);
    camera_ready:
        GameCameraMakeMiniCut2(&camera_position, target, 0, start, end, blend_in, blend_out, follow_target, 0, borders);
    }
    return 1;
}

i32 Action_DynamicCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_offset;
    u32 flags;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    i32 rotation_z;
    f32 start_time;
    f32 blend_in_time;
    f32 end_time;
    f32 blend_out_time;
    f32 blend_time;
    f32 hold_time;
    f32 max_time;
    i32 borders;
    i32 use_current_position;
    i8 mode;
    i8 interpolation;
    NUGSPLINE *spline;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    i32 have_camera_position;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_offset.x = 0.0f;
    camera_offset.y = 0.0f;
    camera_offset.z = 0.0f;
    flags = 0;
    range = 1.0f;
    rotation_x = 0;
    rotation_y = 0;
    rotation_z = 0;
    start_time = 0.0f;
    blend_in_time = 0.0f;
    end_time = 1000000000.0f;
    blend_out_time = 0.0f;
    blend_time = 0.0f;
    hold_time = 0.0f;
    max_time = 0.0f;
    borders = 1;
    use_current_position = 0;
    mode = -1;
    interpolation = -1;
    spline = NULL;
    target_locator = NULL;
    target_character = NULL;
    target_object = NULL;
    camera_locator = NULL;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
                flags |= 0x100;
                use_current_position = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758160)) != NULL) {
                flags |= 0x2000;
                spline = NuSplineFind(WORLD->current_gscn, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75814c)) != NULL) {
                flags |= 0x4000;
                range = AIParamToFloat(processor, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
                flags |= 0x200;
                start_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758140)) != NULL) {
                flags |= 0x800;
                blend_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
                blend_in_time = AIParamToFloat(processor, value + 14);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
                blend_out_time = AIParamToFloat(processor, value + 15);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
                flags |= 0x400;
                end_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758134)) != NULL) {
                max_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758128)) != NULL) {
                flags |= 0x1000;
                hold_time = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
                target_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
                target_character = value + 14;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
                target_object = value + 8;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
                camera_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                flags |= 1;
                range = AIParamToFloat(processor, value + 6);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75811c)) != NULL) {
                flags |= 0x20;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
                flags |= 0x4;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758110)) != NULL) {
                flags |= 0x10;
                rotation_x = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
                flags |= 0x2;
                rotation_x = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758104)) != NULL) {
                flags |= 0x40;
                rotation_z = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral7580fc)) != NULL) {
                flags |= 0x8;
                rotation_z = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
                camera_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
                camera_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
                camera_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
                target_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
                target_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
                target_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
                camera_offset.x = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
                camera_offset.y = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
                camera_offset.z = AIParamToFloat(processor, value + 10);
            } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
                borders = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580f0, params[index]) == 0) {
                mode = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580e4, params[index]) == 0) {
                mode = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580dc, params[index]) == 0) {
                interpolation = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580cc, params[index]) == 0) {
                interpolation = 2;
            } else if (NuStrICmp((char *)AICameraLiteral7580c0, params[index]) == 0) {
                interpolation = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580b0, params[index]) == 0) {
                interpolation = 4;
            } else if (NuStrICmp((char *)AICameraLiteral7580a4, params[index]) == 0) {
                interpolation = 3;
            }
        }
        target = NULL;
        if (target_locator != NULL) {
            locator = AIPathFindLocator(system, target_locator);
            if (locator != NULL)
                target = &locator->position;
        } else if (target_character != NULL) {
            if (packet != NULL && packet->owner != NULL &&
                NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0)
                object = packet->owner->objptr;
            else {
                APIOBJECT_PC *named =
                    GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, target_character) : NULL;
                object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
            }
            if (object != NULL)
                target = &object->collision_position;
        } else if (target_object != NULL) {
            if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) != 0)
                target = NuSpecialGetDrawPos(&special);
        } else if (target_position.x != 1000000000.0f && target_position.y != 1000000000.0f &&
                   target_position.z != 1000000000.0f) {
            target = &target_position;
        }
        if (target != NULL)
            flags |= 0x80;

        have_camera_position = 0;
        if (use_current_position != 0) {
            matrix = NuCameraGetMtx();
            if (matrix != NULL) {
                camera_position = matrix->translation;
                have_camera_position = 1;
            }
        } else if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
                   camera_position.z != 1000000000.0f) {
            have_camera_position = 1;
        } else if (camera_locator != NULL) {
            locator = AIPathFindLocator(system, camera_locator);
            if (locator != NULL) {
                camera_position = locator->position;
                have_camera_position = 1;
            }
        }
        if (spline != NULL) {
            PointAlongSpline(spline, 0.0f, &camera_position, NULL, NULL, 0);
            if (mode == -1)
                mode = 2;
        } else if (have_camera_position != 0) {
            flags |= 0x100;
            NuVecAdd(&camera_position, &camera_position, &camera_offset);
            if (mode == -1)
                mode = 1;
        } else {
            if (target != NULL) {
                camera_position.x = 0.0f;
                camera_position.y = 0.0f;
                camera_position.z = range;
                NuVecRotateX(&camera_position, &camera_position, rotation_x);
                NuVecRotateY(&camera_position, &camera_position, rotation_y);
                NuVecAdd(&camera_position, &camera_position, target);
            }
            if (mode == -1)
                mode = 0;
        }
        GameCameraMakeMiniCut3(flags, range, rotation_x, rotation_y, rotation_z, target, 0, &camera_position,
                               start_time, blend_in_time, end_time, blend_out_time, blend_time, hold_time, max_time,
                               borders, spline, mode, interpolation);
    }
    return 1;
}

i32 Action_EndCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                        i32 param_count, i32 first_time, f32 elapsed) {
    char *value;
    i32 i;

    if (first_time != 0 && MiniCutCam != 0) {
        for (i = 0; i < param_count; ++i) {
            value = NuStrIStr(params[i], (char *)AICameraLiteral758064);
            if (value != NULL) {
                ObstacleCamEnd = AIParamToFloat(processor, value + 9);
                ObstacleCamEnd += ObstacleCamTime;
            } else {
                value = NuStrIStr(params[i], (char *)AICameraLiteral758044);
                if (value != NULL) {
                    ObstacleCamBlendOutTime = AIParamToFloat(processor, value + 15);
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_DontRaycastLOS(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && WORLD->object_sys != NULL) {

        WORLD->object_sys->flags_word |= 1u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                WORLD->object_sys->flags_word &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetForceBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    NUVEC *position;
    f32 radius;
    i32 type;
    i32 enabled;
    char *value;
    AILOCATOR_PC *locator;

    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    position = NULL;
    radius = 1.5f;
    type = 0;
    enabled = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[index], (char *)AIPathLocatorName)) != NULL) {
            locator = AIPathFindLocator(sys, value + 8);
            position = locator != NULL ? &locator->position : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7581a0)) != NULL) {
            radius = AIParamToFloat(processor, value + 7);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758194) == 0) {
            type = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758188) == 0) {
            type = 2;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758174) == 0) {
            type = 3;
        }
    }

    if (enabled != 0) {
        if (object != NULL || position != NULL) {
            SetForceBack(object, position, radius, type);
        }
    } else {
        ResetForceBack();
    }
    return 1;
}

i32 __cdecl Action_FaceCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    f32 completion_time;
    NUMTX *camera;
    (void)sys;
    if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }

    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    completion_time = 0.0f;
    if (param_5 != 0) {
        i32 index;
        f32 minimum_time = 0.0f;
        f32 maximum_time = 0.0f;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp("look_at_camera", params[index]) == 0) {
                processor->action_data_1 = 1;
            } else {
                char *value = NuStrIStr(params[index], (char *)"mintime");
                if (value != NULL) {
                    minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                } else if ((value = NuStrIStr(params[index], (char *)"maxtime")) != NULL) {
                    maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                } else {
                    processor->action_timer = AIParamToFloat(processor, params[index]);
                }
            }
        }
        if (processor->action_timer == 0.0f && maximum_time > minimum_time) {
            processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
        }
    }

    camera = NuCameraGetMtx();
    if (camera != NULL) {
        packet->movement_look_target = (NUVEC *)&camera->m30;
        if (processor->action_data_1 != 0) {
            SetHeadTarget(object, object->ai_packet.movement_look_target, 6, 1.0f, 0.0f, 0.0f);
        }
    }
    if (processor->action_timer > completion_time) {
        processor->action_timer -= param_6;
        if (completion_time >= processor->action_timer) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_FaceCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 10);
            } else {
                processor->action_timer = AIParamToFloat(processor, params[index]);
            }
        }
    }
    if (packet != NULL && processor->action_data_3 != NULL)
        packet->movement_look_target = &((GAMEOBJECT *)processor->action_data_3)->position;
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_SpinOnSpot(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    f32 zero;
    u16 angle;
    f32 min_time;
    f32 max_time;
    char *value;
    (void)sys;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    zero = 0.0f;

    if (param_5 != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral7581ec);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral7581e0);
                if (value != NULL) {
                    max_time = AIParamToFloatEx(packet, processor, value + 8);
                } else {
                    value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0);
                    if (value != NULL) {
                        processor->action_timer = AIParamToFloatEx(packet, processor, value + 5);
                    } else {
                        value = NuStrIStr(params[index], (char *)AITransferLiteral7581d4);
                        if (value != NULL) {
                            processor->action_data_4 =
                                (f32)((i32)(AIParamToFloatEx(packet, processor, value + 9) * 182.04444885253906));
                        }
                    }
                }
            }
        }
        if (min_time != max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
        angle = object->field_0x276;
    } else {
        angle = processor->action_data_6;
    }

    angle += (i32)(param_6 * processor->action_data_4);
    processor->action_data_6 = angle;
    processor->action_pos.x = 0.0f;
    processor->action_pos.y = 0.0f;
    processor->action_pos.z = 1.0f;
    NuVecRotateY(&processor->action_pos, &processor->action_pos, angle);
    NuVecAdd(&processor->action_pos, &processor->action_pos, &object->collision_position);
    packet->movement_look_target = &processor->action_pos;

    if (processor->action_timer > zero) {
        processor->action_timer -= param_6;
        if (zero >= processor->action_timer) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_FollowCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    APIOBJECT_PC *named;
    GAMEOBJECT *target;
    GAMEOBJECT *owner;
    if (packet == NULL)
        return 1;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                processor->action_data_3 = named != NULL ? named->owner : NULL;
            } else if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0)
                processor->action_data_1 |= 2;
            else if (NuStrICmp(params[index], (char *)AIPathCanGoOffPathName) == 0)
                processor->action_data_1 |= 1;
            else if (NuStrICmp(params[index], (char *)AIGameOpponentName) == 0)
                processor->action_data_3 = packet->opponent;
            else if (NuStrICmp(params[index], (char *)AIGameTakeOverTargetName) == 0) {
                owner = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
                processor->action_data_3 = owner->takeover_target;
            } else
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
        }
    }
    target = (GAMEOBJECT *)processor->action_data_3;
    if (target != NULL)
        FollowAPIObject((APIOBJECT_PC *)packet->owner, (APIOBJECT_PC *)target, processor->action_data_1,
                        packet->movement_instruction_parameter);
    return 0;
}

i32 __cdecl Action_GameFollowPlayer(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    i32 index;
    f32 first_distance;
    f64 first_saved;
    f32 second_distance;
    f32 distance;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL || system == NULL ||
        system->player_1 == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0)
                processor->action_data_1 |= 2;
            else if (NuStrICmp(params[index], (char *)AIPathCanGoOffPathName) == 0)
                processor->action_data_1 |= 1;
            else if (NuStrICmp(params[index], (char *)AIGameHoldSpecialButtonName) == 0)
                processor->action_data_2 = 1;
            else if (NuStrICmp(params[index], (char *)AIGameNearestName) == 0) {
                if (player != NULL && player2 != NULL) {
                    first_distance = NuVecDist(&((APIOBJECT_PC *)packet->owner)->position, &player->position, NULL);
                    first_saved = first_distance;
                    second_distance = NuVecDist(&((APIOBJECT_PC *)packet->owner)->position, &player2->position, NULL);
                    if (second_distance < first_saved)
                        processor->action_data_3 = player2;
                    else
                        processor->action_data_3 = player;
                } else
                    processor->action_data_3 = player;
            } else if (packet->movement_instruction_parameter == 0.0f)
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
        }
    }
    if (processor->action_data_3 == NULL && player == NULL)
        return 0;
    if (((APIOBJECT_PC *)packet->owner)->locator_name_index != -1 && drop_back_in_timer > 0.0f)
        FollowAPIObject((APIOBJECT_PC *)packet->owner, (APIOBJECT_PC *)player, processor->action_data_1, 0.01f);
    else if (((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
        distance = packet->movement_instruction_parameter +
                   party_follow_offsets[((APIOBJECT_PC *)packet->owner)->locator_name_index];
        FollowAPIObject((APIOBJECT_PC *)packet->owner, (APIOBJECT_PC *)player, processor->action_data_1, distance);
    } else
        FollowAPIObject((APIOBJECT_PC *)packet->owner, (APIOBJECT_PC *)player, processor->action_data_1,
                        packet->movement_instruction_parameter);
    if (processor->action_data_2 != 0) {
        object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        object->pad->buttons_held |= GAMEPAD_SPECIAL;
    }
    return 0;
}

i32 __cdecl Action_MoveForward(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    i32 minimum_turn;
    i32 maximum_turn;
    i32 turn;
    i32 random_direction;
    i32 index;
    char *value;
    NUVEC destination;
    if (player == NULL || packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 0;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        processor->action_data_4 = (f32)object->field_0x276;
        minimum_turn = 0;
        maximum_turn = 0;
        turn = 0;
        random_direction = 0;
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            value = NuStrIStr(params[index], (char *)AIGameMinimumTurnName);
            if (value != NULL) {
                minimum_turn = (i32)((i32)AIParamToFloat(processor, value + 9) * (f64)(65536.0f / 360.0f));
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameMaximumTurnName);
            if (value != NULL) {
                maximum_turn = (i32)((i32)AIParamToFloat(processor, value + 9) * (f64)(65536.0f / 360.0f));
                continue;
            }
            if (NuStrIStr(params[index], (char *)AIGameRandomTurnDirectionName) != NULL) {
                random_direction = 1;
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameTurnName);
            if (value != NULL)
                turn = (i32)((i32)AIParamToFloat(processor, value + 5) * (f64)(65536.0f / 360.0f));
        }
        if (maximum_turn != 0)
            turn = (i32)((f32)(maximum_turn - minimum_turn) * NuRandFloat()) + minimum_turn;
        if (turn != 0) {
            if (random_direction != 0 && (NuRand(NULL) & 1) != 0)
                turn = -turn;
            processor->action_data_4 = (f32)NuAngAdd((NUANG)processor->action_data_4, turn);
        }
    }
    if ((packet->path_info.flags & 1) == 0)
        return 0;
    destination.x = 0.0f;
    destination.y = 0.0f;
    destination.z = 10.0f;
    NuVecRotateY(&destination, &destination, (NUANG)processor->action_data_4);
    NuVecAdd(&destination, &destination, &object->position);
    AIMoveInstruction(packet, &destination, 0.0f, &packet->path_info, 1, 0.01f);
    return 0;
}

i32 Action_HelpWithTriggers(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **unused3,
                            i32 unused4, i32 unused5, f32 elapsed) {
    AITRIGGERSETSYS_s *system;
    AITRIGGERSET_s *set;
    i32 trigger_index;
    GAMEOBJECT *object;
    AITRIGGERSET_TARGET *target;
    GIZMO_PC *gizmo;
    NUVEC *spinner_origin;
    NUVEC offset;
    f32 dist;
    NUVEC spinner_position, spinner_direction;
    if (((AI_TRIGGER_WORLD_PC *)WORLD)->trigger_system != NULL && packet != NULL && packet->owner != NULL &&
        packet->owner->objptr != NULL) {
        system = ((AI_TRIGGER_WORLD_PC *)WORLD)->trigger_system;
        if (system->field_0x42c0[packet->owner->hostility_group] == -1)
            return 0;
        set = &system->sets[system->field_0x4280[packet->owner->hostility_group]];
        trigger_index = system->field_0x42c0[packet->owner->hostility_group];
        object = packet->owner->objptr;
        target = &set->targets[trigger_index];
        gizmo = set->triggers[trigger_index];
        spinner_origin = NULL;
        if (gizmo->type_id == spinner_gizmotype_id) {
            GIZSPINNER_s *spinner;
            spinner_origin = &packet->owner->collision_position;
            spinner = (GIZSPINNER_s *)gizmo->object;
            if (GizSpinner_GetNearestTargetPoint(spinner, spinner_origin, &spinner_position, &spinner_direction, 1) !=
                -1.0f) {
                NuVecAddScale(&spinner_position, &spinner_position, &spinner_direction, 0.5f);
                AIMoveInstruction(packet, &spinner_position, 0.0f, &target->path, 1, 0.0f);
            } else if (GizSpinner_GetNearestTargetPoint(spinner, spinner_origin, &spinner_position, &spinner_direction,
                                                        0) != -1.0f) {
                NuVecAddScale(&spinner_position, &spinner_position, &spinner_direction, 0.5f);
                AIMoveInstruction(packet, &spinner_position, 0.0f, &target->path, 1, 0.0f);
            }
        } else if (gizmo->type_id == grapple_gizmotype_id) {
            AI_TRIGGER_GRAPPLE_PC *grapple = (AI_TRIGGER_GRAPPLE_PC *)gizmo->object;
            if (object->character_context == LEGOCONTEXT_GRAPPLE && object->grapple == grapple) {
                GAMEOBJECT *found;
                i32 i;
                object->field_1088 = 5;
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                ClearSpecialMove(object);
                found = NULL;
                for (i = 0; i < 2; ++i) {
                    if (found == NULL) {
                        GAMEOBJECT *other = (GAMEOBJECT *)Player[i];
                        if (other != NULL && other->character_context == 0x46) {
                            i32 j;
                            for (j = 0; j < set->trigger_count; ++j) {
                                if (set->triggers[j] != NULL && set->triggers[j]->object == other->grapple) {
                                    found = other;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (found != NULL) {
                    if (player->position.y > object->position.y + (double)0.05f)
                        ((AI_TRIGGER_OBJECT_PC *)object)->grapple_direction = 1;
                    else if (object->position.y - (double)0.05f > player->position.y)
                        ((AI_TRIGGER_OBJECT_PC *)object)->grapple_direction = 2;
                }
                return 0;
            }
            GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
            AIMoveInstruction(packet, &target->position, 0.0f, &target->path, 1, 0.0f);
        } else {
            AIMoveInstruction(packet, &target->position, 0.0f, &target->path, 1, 0.0f);
        }
        gizmo = set->triggers[trigger_index];
        if (gizmo == NULL)
            return 0;
        if (gizmo->type_id == lever_gizmotype_id) {
            if (object->character_model->action_animations[0x5d] != NULL) {
                GIZMO_LEVER_PC *lever = (GIZMO_LEVER_PC *)gizmo->object;
                dist = NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset);
                if (dist < ai_moveradius * ai_moveradius) {
                    packet->movement_look_target = &lever->position;
                    object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
                }
            } else if (FreePlay != 0) {
                processor->action_timer -= elapsed;
                if (processor->action_timer < 0.0f) {
                    processor->action_timer = 0.5f;
                    object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
                }
            }
        } else if (gizmo->type_id == spinner_gizmotype_id) {
            if (object->field_281 != 0 &&
                NuVecXZDistSqr(spinner_origin, &spinner_position, NULL) < packet->mover_height) {
                NuVecAddScale(&spinner_position, &spinner_position, &spinner_direction, -0.5f);
                AIMoveInstruction(packet, &spinner_position, 0.0f, &target->path, 1, 0.0f);
                object->flags_ef4 |= 0x20000;
                object->respawn_timer = 0.0f;
            }
        } else if (gizmo->type_id == force_gizmotype_id) {
            AI_TRIGGER_FORCE_PC *force = (AI_TRIGGER_FORCE_PC *)gizmo->object;
            i32 is_category;
            if ((force->config_flags & 0x10) != 0)
                is_category = CharCategory_IsCategory(object, 1);
            else
                is_category = CharCategory_IsCategory(object, 0);
            if (is_category == 0) {
                if (FreePlay != 0) {
                    processor->action_timer -= elapsed;
                    if (processor->action_timer < 0.0f) {
                        processor->action_timer = 0.5f;
                        object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
                    }
                }
                return 0;
            }
            dist = NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset);
            if (dist < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &force->position;
                object->pad->runtime_flags |= 4;
                object->gizforce_target = force;
            }
        } else if (gizmo->type_id == grapple_gizmotype_id) {
            AI_TRIGGER_GRAPPLE_PC *grapple = (AI_TRIGGER_GRAPPLE_PC *)gizmo->object;
            if (processor->action_data_1 != 0) {
                if (object->character_context != 0) {
                    processor->action_data_1 = 1;
                    return 0;
                }
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                AIMoveInstruction(packet, &grapple->ground_position, 0.0f, &target->path, 7,
                                  packet->movement_instruction_parameter);
                if (processor->action_data_1 != 0)
                    return 0;
            }
            dist = NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset);
            if (dist < ai_moveradius * ai_moveradius) {
                processor->action_data_1 = 1;
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                object->pad->buttons_pressed |= GAMEPAD_JUMP;
            }
        }
    }
    return 0;
}

i32 __cdecl Action_UseTriggerSet(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    AITRIGGERSET_s *set;
    i32 index;
    char *value;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || system == NULL ||
        system->player_1 == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time && WORLD->ai_trigger_set != NULL) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameSetName);
            if (value != NULL) {
                i32 set_number = (i32)AIParamToFloat(processor, value + 4);
                if ((u32)set_number - 1u <= 31u)
                    processor->action_data_3 = &((AITRIGGERSETSYS_s *)WORLD->ai_trigger_set)->sets[set_number - 1];
            }
        }
    }
    set = (AITRIGGERSET_s *)processor->action_data_3;
    if (set != NULL) {
        object->active_trigger_set = (AITRIGGERSET_PARTIAL_PC *)set;
        set->flags_word |= 2;
        Action_HelpWithTriggers(system, processor, packet, params, count, first_time, elapsed);
    }
    return 0;
}

i32 __cdecl Action_PullLever(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 elapsed) {
    BULK_AI_LEVER_PC *lever;
    GAMEOBJECT *object;
    i32 instant = 0;
    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            char *name = NuStrIStr(params[index], (char *)"lever=");
            if (name != NULL) {
                BULK_AI_LEVER_PC *lever;
                f32 height;
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, lever_gizmotype_id, name + 6);
                if (gizmo == NULL || gizmo->object == NULL)
                    continue;
                lever = (BULK_AI_LEVER_PC *)gizmo->object;
                processor->action_data_3 = lever;
                if ((lever->flags & 2))
                    continue;
                processor->action_pos = lever->floor_position;
                height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                if (height != 2000000.0f)
                    processor->action_pos.y = height;
                AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL,
                                 0xff);
            } else if (NuStrICmp(params[index], "instant") == 0) {
                instant = 1;
            }
        }
    }
    lever = (BULK_AI_LEVER_PC *)processor->action_data_3;
    if (lever == NULL)
        return 1;
    if (instant != 0) {
        lever->flags |= 2;
        lever->animation_frame = 0x8000;
        lever->pull_progress = 1.0f;
        return 1;
    }
    if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
    if ((object->flags_ef0 & 0x800000) != 0) {
        f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
        if (distance < ai_moveradius * ai_moveradius) {
            packet->movement_look_target = &lever->position;
            object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        }
    } else if (FreePlay != 0) {
        processor->action_timer -= elapsed;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return object->character_context == 0x4a && object->grapple == lever;
}

i32 __cdecl Action_UsePanel(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    BULK_AI_PANEL_PC *panel;
    if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            GIZMO_PC *gizmo;
            BULK_AI_PANEL_PC *panel;
            f32 height;
            char *name = NuStrIStr(params[index], (char *)"name=");
            if (name == NULL)
                continue;
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizpanel_gizmotype_id, name + 5);
            if (gizmo == NULL || gizmo->object == NULL)
                continue;
            panel = (BULK_AI_PANEL_PC *)gizmo->object;
            processor->action_data_3 = panel;
            if ((panel->flags & 2) != 0)
                continue;
            processor->action_pos = panel->floor_position;
            height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
            if (height != 2000000.0f)
                processor->action_pos.y = height;
            AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL, 0xff);
        }
    }
    panel = (BULK_AI_PANEL_PC *)processor->action_data_3;
    if (panel == NULL || (panel->flags & 2) != 0)
        return 1;
    AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
    if (GizPanel_CanUsePanel(object, panel) != 0) {
        f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
        if (distance < ai_moveradius * ai_moveradius) {
            packet->movement_look_target = &panel->position;
            object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        }
    } else if (FreePlay != 0) {
        processor->action_timer -= elapsed;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return object->character_context == 0x0b && object->grapple == panel;
}

i32 __cdecl Action_UseTechno(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    BULK_AI_TECHNO_PC *techno;
    if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            GIZMO_PC *gizmo;
            BULK_AI_TECHNO_PC *techno;
            f32 height;
            char *name = NuStrIStr(params[index], (char *)"name=");
            if (name == NULL)
                continue;
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, techno_gizmotype_id, name + 5);
            if (gizmo == NULL || gizmo->object == NULL)
                continue;
            techno = (BULK_AI_TECHNO_PC *)gizmo->object;
            processor->action_data_3 = techno;
            processor->action_pos = techno->ground_position;
            height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
            if (height != 2000000.0f)
                processor->action_pos.y = height;
            AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL, 0xff);
        }
    }
    techno = (BULK_AI_TECHNO_PC *)processor->action_data_3;
    if (techno == NULL || (techno->flags & 1) == 0 || (techno->flags & 2) == 0 || (techno->flags & 8) != 0)
        return 1;
    AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
    if (GizTechno_CanUseTechno(object, techno) == 0) {
        if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
    } else {
        f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
        if (distance < ai_moveradius * ai_moveradius) {
            packet->movement_look_target = &techno->position;
            GameObjectSetCanUse(object, techno, 2, 1, 0.0f);
        }
    }
    return 0;
}

i32 Action_ReleaseLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                          i32 param_count, i32 first_time, float unused6) {
    i32 index;
    GAMEOBJECT *object = NULL;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
            object = packet->owner->objptr;
        }
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], "character=");
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + sizeof("character=") - 1);
            }
        }
        if (object != NULL) {
            object->ai_packet.locator = NULL;
        }
    }
    return 1;
}

i32 __cdecl Action_AssignLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, float unused6) {
    APIOBJECT_PC *named;
    AILOCATOR_PC *locator;
    AILOCATORSET_PC *locator_set;
    i32 assignment;
    i32 index;
    GAMEOBJECT *object;
    if (first_time == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner : NULL;
    locator = NULL;
    locator_set = NULL;
    assignment = packet != NULL && packet->owner != NULL ? ((APIOBJECT_PC *)object)->object_id : -1;

    for (index = 0; index < param_count; ++index) {
        char *value;
        char *param = params[index];
        if (NuStrICmp(param, GizActionLiteral758288) == 0) {
            locator = object != NULL ? object->ai_packet.locator : NULL;
            continue;
        }
        value = NuStrIStr(param, (char *)GizActionLiteral758280);
        if (value != NULL) {
            locator = AIPathFindLocator(sys, value + NuStrLen(GizActionLiteral758280) + 1);
            continue;
        }
        value = NuStrIStr(param, "character");
        if (value != NULL) {
            object = (GetNamedGameObjectCallback != NULL &&
                              (named = GetNamedGameObjectCallback(sys, value + NuStrLen("character") + 1)) != NULL
                          ? (GAMEOBJECT *)named->owner
                          : NULL);
            assignment = ((APIOBJECT_PC *)object)->object_id;
            continue;
        }
        value = NuStrIStr(param, (char *)GizActionLiteral75827c);
        if (value != NULL) {
            locator_set = AIPathFindLocatorSet(sys, value + NuStrLen(GizActionLiteral75827c) + 1);
            continue;
        }
        if (NuStrICmp(param, GizActionLiteral758274) == 0) {
            assignment = 0x80;
        }
    }

    if (locator != NULL && locator_set != NULL) {
        for (index = 0; index < locator_set->locator_count; ++index) {
            if (&sys->locators[locator_set->locator_entries[index]] != locator) {
                continue;
            }
            break;
        }
        locator_set->assigned[index] = assignment;
    }
    return 1;
}

i32 __cdecl Action_GetLocatorFromSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, float unused6) {
    APIOBJECT_PC *named;
    AILOCATORSET_PC *locator_set;
    i32 ignore_assigned;
    i32 next;
    char *set_name;
    i32 use_player;
    i32 use_opponent;
    i32 use_second_player;
    i32 first;
    i32 looping;
    i32 finish_at_end;
    i32 random;
    i32 furthest;
    f32 max_range;
    f32 off_screen_radius;
    i32 index;
    APIOBJECT_PC *target;
    NUVEC *reference_position;
    NUVEC *second_position;
    GAMEOBJECT *object;
    object = NULL;
    second_position = NULL;
    ignore_assigned = -1;
    next = 0;
    set_name = NULL;
    use_player = 0;
    use_opponent = 0;
    use_second_player = 0;
    first = 0;
    looping = 0;
    finish_at_end = 0;
    random = 0;
    furthest = 0;
    max_range = 0.0f;
    off_screen_radius = 0.0f;
    if (first_time == 0) {
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL)
        object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    locator_set = processor->locator_set;

    for (index = 0; index < param_count; ++index) {
        char *value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object =
                (GetNamedGameObjectCallback != NULL && (named = GetNamedGameObjectCallback(sys, value + 10)) != NULL
                     ? (GAMEOBJECT *)named->owner
                     : NULL);
            continue;
        }
        value = NuStrIStr(params[index], (char *)GizActionLiteral758374);
        if (value != NULL) {
            max_range = AIParamToFloat(processor, value + 10);
            continue;
        }
        value = NuStrIStr(params[index], (char *)GizActionLiteral758360);
        if (value != NULL) {
            max_range = AIParamToFloat(processor, value + 17);
            use_player = 1;
            continue;
        }
        value = NuStrIStr(params[index], (char *)GizActionLiteral75834c);
        if (value != NULL) {
            max_range = AIParamToFloat(processor, value + 19);
            use_opponent = 1;
            continue;
        }
        value = NuStrIStr(params[index], (char *)GizActionLiteral758338);
        if (value != NULL) {
            off_screen_radius = AIParamToFloat(processor, value + 18);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            set_name = value + 5;
            continue;
        }

        if (NuStrICmp(params[index], AITransferLiteral757824) == 0) {
            random = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral758330) == 0) {
            next = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral758328) == 0) {
            first = 1;
        } else if (NuStrICmp(params[index], AITransferLiteral757a7c) == 0) {
            looping = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral758318) == 0) {
            finish_at_end = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral758300) == 0) {
            ignore_assigned = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral7582e8) == 0) {
            ignore_assigned = 0;
        } else if (NuStrICmp(params[index], GizActionLiteral7582d0) == 0) {
            furthest = 1;
            use_opponent = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral7582b4) == 0) {
            furthest = 1;
            use_player = 1;
            use_second_player = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral75829c) == 0) {
            use_player = 1;
            use_second_player = 1;
        }
    }

    if (ignore_assigned == -1) {
        ignore_assigned = next ^ 1;
    }
    if (object == NULL) {
        return 1;
    }
    if (set_name != NULL) {
        locator_set = AIPathFindLocatorSet(WORLD->ai_sys, set_name);
    }
    if (locator_set == NULL) {
        return 1;
    }

    target = (APIOBJECT_PC *)object;
    if (use_player) {
        reference_position = &player->position;
    } else if (use_opponent && object->ai_packet.opponent != NULL) {
        reference_position = &((APIOBJECT_PC *)object->ai_packet.opponent)->position;
    } else {
        reference_position = &target->position;
    }
    if (use_second_player && player2 != NULL)
        second_position = &player2->position;

    if (first) {
        if (locator_set->locator_count != 0) {
            ((APIOBJECT_PC *)object)->ai->locator = &sys->locators[locator_set->locator_entries[0]];
            locator_set->assigned[0] = target->object_id;
        }
        return 1;
    }

    if (next) {
        GizActionNextLocator(sys, locator_set, looping, finish_at_end, ignore_assigned, target);
    } else {
        if (!random) {
            if (furthest) {
                AILocatorSet_AssignFurthestLocator(sys, locator_set, target, max_range, reference_position,
                                                   second_position, off_screen_radius, ignore_assigned);
            } else {
                AILocatorSet_AssignNearestLocator(sys, locator_set, target, max_range, reference_position,
                                                  second_position, off_screen_radius, ignore_assigned);
            }
        } else {
            AILocatorSet_AssignRandomLocator(sys, locator_set, target, max_range, reference_position, off_screen_radius,
                                             ignore_assigned);
        }
    }
    return 1;
}

i32 __cdecl Action_ProbeDroid(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *towing_player, *bomb;
    AI_CABLE_OBJECT_PC *view, *player_view;
    AI_CABLE_HEAD_PC *cable;
    AILOCATOR_PC *locator;
    NUVEC difference;
    f32 distance;
    char *engage_params[3];
    (void)params;
    (void)count;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    bomb = NULL;
    towing_player = NULL;
    if (((AI_CABLE_OBJECT_PC *)player)->cable != NULL &&
        (bomb = ((AI_CABLE_HEAD_PC *)((AI_CABLE_OBJECT_PC *)player)->cable)->target) != NULL && bomb->id == id_DRAGBOMB)
        towing_player = player;
    else if (player2 != NULL && ((AI_CABLE_OBJECT_PC *)player2)->cable != NULL &&
             (bomb = ((AI_CABLE_HEAD_PC *)((AI_CABLE_OBJECT_PC *)player2)->cable)->target) != NULL &&
             bomb->id == id_DRAGBOMB)
        towing_player = player2;
    view = (AI_CABLE_OBJECT_PC *)object;
    if (bomb != NULL && towing_player != NULL) {
        if (view->cable != NULL && ((AI_CABLE_HEAD_PC *)view->cable)->target != bomb) {
            ((AI_CABLE_HEAD_PC *)view->cable)->source = NULL;
            view->cable = NULL;
        }
        if (view->cable == NULL) {
            distance = NuVecXZDistSqr(&object->collision_position, &bomb->collision_position, &difference);
            if (distance < (f64)max_cable_length * max_cable_length * 0.5)
                view->cable = CreateCable(object, bomb, 1);
        }
        if (view->cable != NULL) {
            locator = object->ai_packet.locator;
            if (locator != NULL)
                AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 1, 0.0f);
            else
                AIMoveInstruction(packet, &towing_player->ai_packet.last_path_position,
                                  towing_player->ai_packet.mover_height, &towing_player->ai_packet.path_info, 2, 99.0f);
        } else {
            AIMoveInstruction(packet, &bomb->ai_packet.last_path_position, bomb->ai_packet.mover_height,
                              &bomb->ai_packet.path_info, 1, 0.0f);
        }
    } else {
        if (view->cable != NULL) {
            ((AI_CABLE_HEAD_PC *)view->cable)->source = NULL;
            view->cable = NULL;
        }
        AIMoveInstruction(packet, NULL, 0.0f, NULL, 4, packet->movement_instruction_parameter);
    }
    engage_params[0] = "goalrange 10";
    engage_params[1] = "firerange 15";
    engage_params[2] = "fireinterval 1.5";
    Action_EngageOpponent(system, processor, packet, engage_params, 3, first_time, delta);
    return 0;
}

i32 __cdecl Action_AlertCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        AlertSurroundingCreatures(object, &object->collision_position);
    return 1;
}

i32 __cdecl Action_SetLastAttacker(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *victim;
    GAMEOBJECT *attacker;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    victim = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    attacker = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a20);
        if (value != NULL) {
            victim = GetNamedGameObject(sys, value + 7);
            continue;
        }
        if (NuStrICmp(params[index], (char *)AITransferLiteral7583d0) == 0) {
            if (packet != NULL && packet->opponent != NULL) {
                attacker = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583c4);
        if (value != NULL) {
            attacker = GetNamedGameObject(sys, value + 9);
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7583b4) == 0) {
            attacker = (GAMEOBJECT *)((APIOBJECT_PC *)sys->player_1)->owner;
        }
    }
    if (victim != NULL && attacker != NULL) {
        victim->last_attacker = attacker;
    }
    return 1;
}

i32 __cdecl Action_LinkTurretToController(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    AITURRET_PARTIAL_PC *turret;
    GAMEOBJECT *controller;
    char *value;
    GIZMO_PC *gizmo;

    turret = NULL;
    controller = NULL;
    if (first_time == 0) {
        return 1;
    }
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583f0);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, value + 7);
            if (gizmo != NULL && gizmo->object != NULL) {
                turret = (AITURRET_PARTIAL_PC *)(gizmo->object);
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7583e4)) != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 11) : NULL;
                controller = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        }
    }
    if (turret != NULL) {
        turret->controller = controller;
    }
    return 1;
}

i32 __cdecl Action_TakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 release;
    char *value;
    i32 player_index;
    PLAYERCHARACTERCONFIG_PC *target_config;

    (void)processor;
    (void)param_6;
    release = 0;
    object = NULL;
    target = NULL;
    if (param_5 == 0) {
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        target = object->takeover_target;
    }

    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75781c);
            if (value != NULL) {
                player_index = (i32)(AIParamToFloat(processor, value + 7)) - 1;
                if (player_index >= 0 && player_index <= 1) {
                    object = Player[player_index];
                }
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral757f28);
                if (value != NULL) {
                    {
                        lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 7) : NULL;
                        target = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                } else if (NuStrIStr(params[index], (char *)AIGameOpponentName) != NULL) {
                    if (packet->opponent != NULL) {
                        target = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
                    }
                } else if (NuStrIStr(params[index], (char *)AITransferLiteral7583f8) != NULL) {
                    release = 1;
                }
            }
        }
    }

    if (object == NULL) {
        return 1;
    }
    if (target != NULL && release == 0) {
        if ((target->character_data->model_flags & 0x40000000) != 0 &&
            (target->character_context == 0x17 || target->character_context == 0x3e)) {
            return 0;
        }
        target_config = target->character_data->game_character;
        if (target->takeover_source == NULL && (target_config->flags_090 & 0x40) != 0 &&
            (object->character_data->model_flags & 0x10) == 0) {
            TakeOverGameObject(object, target, 1, 0);
        }
    } else if (release != 0) {
        ReleaseTakeOver(object, 0);
    }
    return 1;
}

i32 __cdecl Action_ReleaseTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *value;

    object = NULL;
    if (param_5 == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        }
    }
    if (object != NULL && object->takeover_source != NULL) {
        ReleaseTakeOver(object, 0);
    }
    return 1;
}

i32 __cdecl Action_RegisterTakeOverObject(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        RegisterTakeOverObject(object);
    return 1;
}

i32 __cdecl Action_AddGameMsgCount(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object;
    NUVEC *position;
    i32 count = 0;
    i32 total = 0;
    u8 red = 0;
    u8 green = 0;
    u8 blue = 0;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    position = NULL;
    object = NULL;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(AIActionMyPosNamePc, params[index]) == 0 && object != NULL)
                position = &object->collision_position;
            else if ((value = NuStrIStr(params[index], (char *)AIActionTotalPrefixPc)) != NULL)
                total = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionCountPrefixPc)) != NULL)
                count = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c20)) != NULL) {
                message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
                if (message != NULL)
                    count = (i32)message->value;
            } else if ((value = NuStrIStr(params[index], (char *)AIActionRedPrefixPc)) != NULL)
                red = (u8)AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIActionGreenPrefixPc)) != NULL)
                green = (u8)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionBluePrefixPc)) != NULL)
                blue = (u8)AIParamToFloat(processor, value + 5);
        }
        if (count != 0 && count <= total)
            AddGameMsgCount(position, count, total, red, green, blue, 0.75f);
    }
    return 1;
}

i32 __cdecl Action_SetCanTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    object = NULL;
    if (param_5 == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (object != NULL) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp((char *)AITransferLiteral758430, params[index]) == 0) {
                object->flags_ef0 |= 0x40000000u;
            } else if (NuStrIStr(params[index], (char *)AIScriptFalse) != NULL) {
                object->flags_ef0 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanBeCarried(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index, enabled = 1;
    (void)processor;
    (void)delta;
    object = NULL;
    if (!first_time)
        return 1;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (count != 0) {
        for (index = 0; index < count; ++index) {
            if (NuStrICmp(params[index], "FALSE") == 0)
                enabled = 0;
            else if ((value = NuStrIStr(params[index], "character=")) != NULL) {
                found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = found != NULL ? found->owner : NULL;
            }
        }
    }
    if (object != NULL) {
        if (object->character_context == 0x3c || object->takeover_source != NULL)
            Player_ClearContext(object, 1);
        object->flags_ef4 = (object->flags_ef4 & ~2u) | ((u32)(enabled & 1) << 1);
    }
    return 1;
}

i32 __cdecl Action_IgnoreLastSafePathPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                           char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    i32 ignore = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                    ignore = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        if (GetNamedGameObjectCallback != NULL) {
                            named = GetNamedGameObjectCallback(system, value + 10);
                            object = named != NULL ? named->owner : NULL;
                        } else {
                            object = NULL;
                        }
                    }
                }
            }
        }
        if (object != NULL) {
            if (object->character_context == 0x3c || object->rider != NULL)
                Player_ClearContext(object, 1);
            object->flags_ef4 = (object->flags_ef4 & ~0x01000000u) | ((ignore & 1) << 24);
        }
    }
    return 1;
}

i32 __cdecl Action_AwkwardShapeOverridePc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                    enabled = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        if (GetNamedGameObjectCallback != NULL) {
                            named = GetNamedGameObjectCallback(system, value + 10);
                            object = named != NULL ? named->owner : NULL;
                        } else {
                            object = NULL;
                        }
                    }
                }
            }
        }
        if (object != NULL) {
            if (object->character_context == 0x3c || object->rider != NULL)
                Player_ClearContext(object, 1);
            object->flags_ef4 = (object->flags_ef4 & ~0x80u) | ((enabled & 1) << 7);
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreSlideTerrainPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                    enabled = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        if (GetNamedGameObjectCallback != NULL) {
                            named = GetNamedGameObjectCallback(system, value + 10);
                            object = named != NULL ? named->owner : NULL;
                        } else {
                            object = NULL;
                        }
                    }
                }
            }
        }
        if (object != NULL) {
            object->flags_ef4 = (object->flags_ef4 & ~0x400u) | ((enabled & 1) << 10);
        }
    }
    return 1;
}

i32 __cdecl Action_SplineFollowTerrainPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                    enabled = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        if (GetNamedGameObjectCallback != NULL) {
                            named = GetNamedGameObjectCallback(system, value + 10);
                            object = named != NULL ? named->owner : NULL;
                        } else {
                            object = NULL;
                        }
                    }
                }
            }
        }
        if (object != NULL) {
            object->flags_ef4 = (object->flags_ef4 & ~0x1000u) | ((enabled & 1) << 12);
        }
    }
    return 1;
}

i32 __cdecl Action_AddMiscPickups(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    i32 value = 0;
    i32 torpedo = 0;
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    NUVEC *position;
    i32 index;
    char *parameter;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < count; ++index) {
            parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (parameter != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, parameter + 10) : NULL;
                object = named != NULL ? named->owner : NULL;
            } else if ((parameter = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                value = (i32)AIParamToFloat(processor, parameter + 6);
            else if ((parameter = NuStrIStr(params[index], (char *)AIActionTorpedoPrefixPc)) != NULL)
                torpedo = (i32)AIParamToFloat(processor, parameter + 8);
        }
        if (object != NULL) {
            position = &object->collision_position;
            if (position != NULL && (value != 0 || torpedo != 0))
                AddMiscPickups(position, -1, value, torpedo);
        }
    }
    return 1;
}

i32 __cdecl Action_SetLayer(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta_time) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *api_object;
    i32 index, layer;
    char *value;
    u32 set_layers = 0;
    u32 clear_layers = 0;
    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL) {
                api_object = GetNamedGameObjectCallback(system, value + 10);
                object = api_object != NULL ? api_object->owner : NULL;
            } else {
                object = NULL;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AISetLayerKeywordPc);
        if (value != NULL) {
            layer = (i32)AIParamToFloat(process, value + 10);
            if (layer >= 1 && layer <= 32)
                set_layers |= 1u << (layer - 1);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIClearLayerKeywordPc);
        if (value != NULL) {
            layer = (i32)AIParamToFloat(process, value + 12);
            if (layer >= 1 && layer <= 32)
                clear_layers |= 1u << (layer - 1);
        }
    }
    if (object != NULL)
        object->extra_layer_mask = (object->extra_layer_mask | set_layers) & ~clear_layers;
    return 1;
}

i32 __cdecl Action_CreateRider(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    GAMEOBJECT *vehicle = NULL;
    i32 model = -1;
    char *script = NULL;
    i32 index;
    char *value;
    GAMEOBJECT *rider;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        vehicle = packet->owner->objptr;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
            if (value != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    model = (u8)LevelCharacterTypeIDFn(value + 5);
                    if (model != 0xff)
                        model = LevelCharacterGlobalIDFn(model);
                }
            } else if ((value = NuStrIStr(params[index], "script")) != NULL)
                script = value + 7;
        }
        if (model != -1 && vehicle->rider == NULL) {
            rider =
                AddDynamicCreature(model, &vehicle->collision_position, 0,
                                   script != NULL ? script : GizmoDefaultOutputName, NULL, NULL, 0, NULL, NULL, 0, 0);
            if (rider != NULL)
                TakeOverGameObject(rider, vehicle, 0, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_ThrowDetonatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    CHARACTER_MODEL_LAYERS_PC *model;
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    object->character_context = 0x2e;
    if (object->jump_variant_state == 1)
        object->field_7aa = 0x6f;
    else if ((object->field_e22 & 1) != 0)
        object->field_7aa = 0x6e;
    else
        object->field_7aa = 0x65;
    model = object->character_model;
    if (model->model_data_b[(i16)object->field_7aa] == NULL)
        object->field_7aa = 0x65;
    object->context_variant_flags &= ~0x40u;
    if (model->model_data_b[(i16)object->field_7aa] != NULL)
        object->context_timer = AnimDuration((i16)object->id, (i16)object->field_7aa, 0.0f, 0.0f, 1);
    else
        object->context_timer = 1.0f;
    return 1;
}

i32 __cdecl Action_UseTimeBasedUpdate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled = 0;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (first_time != 0) {
            for (index = 0; index < param_count; ++index) {
                if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                    enabled = 1;
                else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
        }
        object->flags_ef4 = (object->flags_ef4 & ~0x20u) | (((enabled == 0) & 1) << 5);
    }
    return 1;
}

i32 __cdecl Action_WalkBackwards(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *opponent;
    GAMEOBJECT *owner;
    (void)sys;
    (void)param_6;
    if (packet == NULL) {
        return 1;
    }
    if (param_5 != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (index = 0; index < param_4; ++index) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    opponent = (APIOBJECT_PC *)packet->opponent;
    if (opponent != NULL && opponent->ai != NULL) {
        owner = packet->owner->objptr;
        AIMoveInstruction(packet, &opponent->ai->last_path_position, opponent->ai->mover_height,
                          &opponent->ai->path_info, 2, packet->movement_instruction_parameter);
        owner->flags_ef0 |= 0x8000u;
        owner->ai_packet.goal_speed_mode = 1;
    }
    return 0;
}

i32 __cdecl Action_AddScriptProcessor(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 count, i32 first_time, f32 elapsed) {
    char *script_name = NULL;
    char *name = NULL;
    AIAREA_PC *area = NULL;
    AILOCATORSET_PC *locator_set = NULL;
    AILOCATOR_PC *locator = NULL;
    i32 set = 0;
    NUGSPLINE *spline = NULL;
    i32 override_count = 0;
    char override_names[4][32];
    f32 override_values[4];
    i32 index, slot;
    char *value, *separator;
    AISCRIPTPROCESS_PC *created;
    WORLDINFO *world;
    (void)packet;
    (void)elapsed;
    if (first_time != 0 && 0 < count) {
        for (index = 0; index < count; ++index) {
            if ((value = NuStrIStr(params[index], "script=")) != NULL) {
                value += 7;
                if (AIScriptFind(WORLD->ai_sys, value, 0, 1, 1) != NULL)
                    script_name = value;
            } else if ((value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix)) != NULL) {
                name = value + 5;
            } else if ((value = NuStrIStr(params[index], "area=")) != NULL) {
                area = AISysFindArea(system, value + 5);
            } else if ((value = NuStrIStr(params[index], "locator_set=")) != NULL) {
                locator_set = AIPathFindLocatorSet(system, value + 12);
            } else if ((value = NuStrIStr(params[index], (char *)AIPathLocatorName)) != NULL) {
                locator = AIPathFindLocator(system, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIGameSetName)) != NULL) {
                set = (i32)AIParamToFloat(processor, value + 4);
                if (set < 0 || set > 16)
                    set = 0;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757a84)) != NULL) {
                spline = NuSplineFind(WORLD->current_gscn, value + 7);
            } else if ((value = NuStrIStr(params[index], (char *)AIScriptParamPrefix)) != NULL && override_count < 4) {
                NuStrNCpy(override_names[override_count], value + 6, 32);
                separator = NuStrIStr(override_names[override_count], "=");
                if (separator != NULL) {
                    override_values[override_count] = AIParamToFloat(processor, separator + 1);
                    *separator = '\0';
                    ++override_count;
                }
            }
        }
        world = WORLD;
        if (script_name != NULL && world->script_processor_count < 32) {
            created = &world->script_processors[world->script_processor_count].processor;
            AIScriptProcessorInit(system, NULL, created, NULL, script_name, NULL, 0, NULL, NULL);
            created->locator_set = locator_set;
            created->locator = locator;
            created->unknown_a0 = (u32)area;
            created->creature_set = (u8)set;
            created->myspline = spline;
            if (name != NULL) {
                world = WORLD;
                NuStrCpy(world->script_processors[world->script_processor_count].name, name);
            }
            world = WORLD;
            LevelScriptReStoreProgress(WORLD, &world->script_processors[world->script_processor_count]);
            if (override_count != 0 && created->script != NULL) {
                for (index = 0; index < override_count; ++index) {
                    for (slot = 0; slot < 4; ++slot) {
                        if (NuStrICmp(override_names[index], created->script->params[slot].name) == 0) {
                            created->params[slot] = override_values[index];
                            break;
                        }
                    }
                }
            }
            ++WORLD->script_processor_count;
            GizmoSysAddGizmos(WORLD->gizmo_sys, WORLD->giz_flow, WORLD);
        }
    }
    return 1;
}

i32 __cdecl Action_ForceLightning(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    struct gameobject_s *object;
    NUVEC target;
    NUVEC direction;
    NUVEC origins[2];
    i32 origin_index;
    (void)param_6;
    object = NULL;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }

    if (param_5 != 0) {
        if (param_4 > 0) {
            i32 index;
            for (index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], "locator=");
                if (value != NULL) {
                    AILOCATOR_PC *locator = AIPathFindLocator(sys, value + 8);
                    if (locator != NULL) {
                        processor->action_pos = locator->position;
                        processor->action_data_1 = 1;
                    }
                } else {
                    value = NuStrIStr(params[index], "randxz=");
                    if (value != NULL) {
                        processor->action_data_4 = AIParamToFloat(processor, value + 7);
                    }
                }
            }
        }
    }

    if (object == NULL || processor->action_data_1 == 0) {
        return 0;
    }

    ForceLightning_Origin(object, &origins[0], &origins[1]);

    target = processor->action_pos;
    if (processor->action_data_4 > 0.0f) {
        f64 range = processor->action_data_4;
        target.x += range - NuRandFloat() * (range + range);
        range = processor->action_data_4;
        target.z += range - NuRandFloat() * (range + range);
    }

    for (origin_index = 0; origin_index < 2; ++origin_index) {
        NUVEC *origin = &origins[origin_index];
        if (origin->y != 1000000000.0) {
            f32 distance = NuVecDist(&target, origin, &direction);
            NuLgtLaser(lightning_type, lightning_sizew[0], lightning_sizel[0], lightning_sizewab[0], origin, &direction,
                       lightning_col[0], lightning_endw[0], distance);
            PlaySfx("ForceLightningLp", &processor->action_pos);
        }
    }
    packet->movement_look_target = &processor->action_pos;
    return 0;
}

i32 __cdecl Action_SetUseOneAtOnce(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 enabled;
    GAMEOBJECT *object;
    i32 index;
    char *name;
    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        enabled = 1;
        object = packet != NULL ? packet->owner->objptr : NULL;

        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                name += 10;
                if (NuStrICmp(name, (char *)AITransferLiteral757f7c) == 0 && packet != NULL)
                    object = packet->owner->objptr;
                else
                    object = GetNamedGameObject(WORLD->ai_sys, name);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                enabled = 0;
        }
        if (object != NULL) {
            object->flags_ef4 = (object->flags_ef4 & ~0x2000u) | ((enabled & 1) << 13);
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_MaxAttackers(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                value = NuStrIStr(params[index], (char *)AIExpressionMaxName);
                if (value != NULL)
                    oneAtOnce_SetNumAttackers((i32)AIParamToFloat(processor, value + 4));
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_AttackersPerRow(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                         char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count != 0) {
            for (index = 0; index < count; ++index) {
                value = NuStrIStr(params[index], (char *)AIActionNumName);
                if (value != NULL)
                    oneAtOnce_SetAttackersPerRow((i32)AIParamToFloat(processor, value + 4));
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_RowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_InitRowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                     char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetInitDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

i32 __cdecl Action_SetTechnoCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    TECHNO_PC *techno = NULL;
    i32 complete = 1;
    i32 index;
    char *value;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    if (first_time) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "techno");
            if (value != NULL) {
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(
                    WORLD->gizmo_sys, GizmoGetTypeIDByName(WORLD->gizmo_sys, (char *)PCContextName_Techno),
                    value + NuStrLen("techno") + 1);
                if (gizmo != NULL && gizmo->object != NULL)
                    techno = (TECHNO_PC *)gizmo->object;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                complete = 0;
            } else if ((value = NuStrIStr(params[index], "controlling")) != NULL) {
                char *name = value + NuStrLen("controlling") + 1;
                GAMEOBJECT_LOOKUP_CALLBACK_PC lookup = GetNamedGameObjectCallback;
                void *ai_system = WORLD->ai_sys;
                APIOBJECT_PC *named = lookup != NULL ? lookup(ai_system, name) : NULL;
                object = named != NULL ? named->owner : NULL;
            }
        }
        if (object != NULL)
            techno = Technos_FindControllingTechno(object);
        if (techno != NULL)
            ((TECHNO_s *)techno)->complete = complete;
    }
    return 1;
}

i32 __cdecl Action_LetGoOfBalloonPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    if (packet != NULL && packet->owner != NULL)
        LetGoOfBalloon(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_DrawBossHitPointsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    if (packet != NULL && packet->owner != NULL)
        DrawBossHitPoints(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_CompleteLevel(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    LEVELDATA_PC *level;
    char *cutscene_name;
    char *value;
    i32 index;

    level = NULL;
    cutscene_name = NULL;
    if (first_time == 0)
        return 1;

    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)CompleteLevelCutscenePrefix);
        if (value != NULL)
            cutscene_name = value + NuStrLen(CompleteLevelCutscenePrefix);

        value = NuStrIStr(params[index], (char *)NewLevelPrefix);
        if (value != NULL) {
            value += NuStrLen(NewLevelPrefix);
            level = Level_FindByName(value, NULL);
        }
    }

    if (FreePlay == 0) {
        if (cutscene_name == NULL)
            goto choose_level;
        if (NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL)
            return 1;
        if (FreePlay == 0)
            goto choose_level;
    }

    if (level != NULL && (level->flags & 0xe0u) != 0)
        level = Area_FindStatusLevel(WORLD->area, NULL);

choose_level:
    if (level != NULL)
        GoToNewLevel(level->index);
    else
        CompleteLevel(WORLD);
    return 1;
}

i32 __cdecl Action_GoToNewLevelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    LEVELDATA_PC *level = NULL;
    char *cutscene_name = NULL;
    char *value;
    i32 index;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)GizActionLevelPrefixPc);
            if (value != NULL) {
                value += NuStrLen(GizActionLevelPrefixPc);
                level = Level_FindByName(value, NULL);
            } else {
                value = NuStrIStr(params[index], (char *)CompleteLevelCutscenePrefix);
                if (value != NULL)
                    cutscene_name = value + NuStrLen(CompleteLevelCutscenePrefix);
            }
        }
    }
    if (FreePlay == 0 && cutscene_name != NULL && NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL)
        return 1;
    if (level != NULL)
        GoToNewLevel(level->index);
    return 1;
}

i32 __cdecl Action_CircleLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    AILOCATOR_PC *locator;
    i32 index;
    char *value;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        processor->action_data_1 = 0;
        processor->action_data_3 = processor->locator;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value == NULL) {
                value = NuStrIStr(params[index], (char *)AIActionCircleTeleportName);
            }
            if (value != NULL) {
                processor->action_data_3 =
                    AIPathFindLocator(sys, value + NuStrLen((char *)AIScriptActionNameArgPrefix));
            } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                packet->movement_instruction_parameter =
                    AIParamToFloatEx(packet, processor, value + NuStrLen((char *)AIPathGoalRangeName) + 1);
            } else if (NuStrICmp(params[index], (char *)AIActionCircleReverseName) == 0) {
                packet->flags ^= 0x200;
            } else if (NuStrICmp(params[index], (char *)AIActionCircleAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[index], (char *)AIActionCircleClockwiseName) == 0) {
                packet->flags |= 0x200;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }
    locator = (AILOCATOR_PC *)processor->action_data_3;
    if (locator != NULL) {
        AIMoveInstruction(packet, &locator->position, packet->movement_instruction_parameter, &locator->path_info, 3,
                          packet->movement_instruction_parameter);
    }
    if (locator == NULL)
        return 1;
    return 0;
}

i32 __cdecl Action_GizmoActivate(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += NuStrLen(AIScriptActionNameArgPrefix);
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value);
            } else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoActivate(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

i32 __cdecl Action_GizmoSetVisibility(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += NuStrLen(AIScriptActionNameArgPrefix);
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value);
            } else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoSetVisibility(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

i32 __cdecl Action_TurnOnPickupPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    GIZMOPICKUP_PC *pickup = NULL;
    if (first_time != 0) {
        i32 index;
        char *value;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += NuStrLen(AIScriptActionNameArgPrefix);
                pickup = GizmoPickup_FindByName(WORLD, value);
            }
        }
        if (pickup != NULL)
            GizmoPickup_TurnOnPickup(pickup);
    }
    return 1;
}

i32 __cdecl Action_CanHelpWithTriggers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanCollideWithObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 &= ~0x80000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                object->flags_ef4 |= 0x80000u;
        }
    }
    return 1;
}

i32 __cdecl Action_AddTorpedoPacket(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object = NULL;
    i32 count = 0;
    u8 flags = 0;
    i32 index;
    char *value;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionTorpedoNamePc);
            if (value != NULL) {
                count = (i32)AIParamToFloat(&packet->script_processor, value + 8);
                if (count < 0)
                    count = 0;
                else if (count > getMaxTorpedos(NULL))
                    count = getMaxTorpedos(NULL);
            } else if (NuStrICmp(params[index], AIActionCanBeStolenNamePc) == 0)
                flags |= 0x20;
        }
    }
    if (object != NULL) {
        if (object->torpedo != NULL)
            FreeTorpedoPacket(&object->torpedo);
        object->torpedo = GetTorpedoPacket();
        if (object->torpedo != NULL && count != 0) {
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->count = (u8)count;
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->flags |= flags;
            for (index = 0; index < count; ++index) {
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->pickup_positions[index] = object->collision_position;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->timer = 0.8f;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->state = 0;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetShootOpponentsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    i32 index;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        object->portal_flags |= 0x80;
        for (index = 0; index < count; ++index) {
            if (NuStrICmp(params[index], AIScriptFalse) == 0)
                object->portal_flags &= ~0x80u;
        }
    }
    return 1;
}

i32 __cdecl Action_PartyCanBeUnderCover(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time) {
        party_cant_be_under_cover = 0;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                party_cant_be_under_cover = 1;
        }
    }
    return 1;
}

void __cdecl GameAIScriptAddLevelSfx(WORLDINFO *world, NULISTHDR *scripts) {
    NULISTLNK *script_link;

    if (scripts == NULL) {
        return;
    }

    script_link = NuLinkedListGetHead(scripts);
    while (script_link != NULL) {
        AISCRIPT_PC *script = (AISCRIPT_PC *)script_link;
        NULISTLNK *state_link = NuLinkedListGetHead(&script->states);

        while (state_link != NULL) {
            AISTATE_PC *state = (AISTATE_PC *)state_link;
            NULISTLNK *action_link = NuLinkedListGetHead(&state->actions);

            while (action_link != NULL) {
                AIACTION_PC *action = (AIACTION_PC *)action_link;
                AIACTIONDEF_PC *definition = action->definition;

                if (definition != NULL && definition->eval_fn == Action_PlaySfx) {
                    i32 index;
                    for (index = 0; index < action->param_count; ++index) {
                        char *name = NuStrIStr(action->params[index], (char *)AIScriptActionNameArgPrefix);
                        if (name != NULL) {
                            AddLevSfx(world, NULL, name + 5, -1);
                        }
                    }
                }

                action_link = NuLinkedListGetNext(&state->actions, action_link);
            }
            state_link = NuLinkedListGetNext(&script->states, state_link);
        }
        script_link = NuLinkedListGetNext(scripts, script_link);
    }
}

i32 __cdecl Action_RaceOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *actor;
    GAMEOBJECT *opponent;
    APIOBJECT_PC *owner;
    SOCKPOSITION_PC projected_position;
    NUVEC opponent_offset;
    NUVEC actor_offset;
    NUVEC destination;
    f64 actor_distance;
    f32 distance_delta;
    char *value;
    i32 index;

    if (packet == NULL || packet->owner == NULL)
        return 1;
    owner = (APIOBJECT_PC *)packet->owner;
    if (owner->owner == NULL)
        return 1;
    actor = (GAMEOBJECT *)owner->owner;

    if (first_time != 0) {
        actor->portal_flags |= 0x4000u;
        actor->flags_ef4 |= 0x200000u;
        actor->movement_speed_scale_pc = 1.0f;
        actor->ai_packet.movement_stopped = 1;
        processor->action_data_4 = 10.0f;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)RaceOpponentDistancePrefix);
            if (value != NULL)
                processor->action_data_4 = AIParamToFloat(&packet->script_processor, value + 9);
        }
    }

    MoveSockPosition(WORLD->sock_sys, &actor->sock_position, 10.0f, &projected_position);
    opponent = packet->opponent;
    if (opponent != NULL && ((APIOBJECT_PC *)opponent)->ai != NULL) {
        actor_distance = MidDistanceFromSockStart(WORLD->sock_sys, &actor->sock_position);
        distance_delta = (f32)(actor_distance - MidDistanceFromSockStart(WORLD->sock_sys, &opponent->sock_position));
        distance_delta = distance_delta - processor->action_data_4;
        if ((f64)distance_delta > (f64)processor->action_data_4 * 0.5)
            actor->movement_speed_scale_pc = 0.9f;
        else if ((f64)distance_delta < -((f64)processor->action_data_4 * 0.5))
            actor->movement_speed_scale_pc = 1.1f;
        else
            actor->movement_speed_scale_pc = 1.0f;

        NuVecSub(&opponent_offset, &opponent->position, &opponent->sock_position.midpoint);
        NuVecRotateY(&opponent_offset, &opponent_offset, -(i32)opponent->sock_position.heading);
        NuVecSub(&actor_offset, &actor->position, &actor->sock_position.midpoint);
        NuVecRotateY(&actor_offset, &actor_offset, -(i32)actor->sock_position.heading);
        destination.z = 0.0f;
        destination.y = 0.0f;
        destination.x = (f32)((f64)opponent_offset.x * 0.5 + (f64)actor_offset.x * 0.5);
        NuVecAdd(&destination, &destination, &projected_position.midpoint);
    } else {
        actor->movement_speed_scale_pc = 1.0f;
        destination = projected_position.midpoint;
    }

    AIMoveInstruction(packet, &destination, 0.0f, NULL, 1, 0.0f);
    return 0;
}

i32 __cdecl Action_CreateCreatures(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_count, i32 first_time, f32 unused_delta) {
    NUVEC position;
    AIPATHINFO_PC *path;
    i32 yaw;
    i32 index;
    f32 delay;
    i32 offset_applied;
    i32 minimum = -1, maximum = -1, count = 1;
    i32 start_locator = 0, inherit = 1;
    i32 offscreen = 0, nearest = 0, onscreen = 0;
    i32 surface = 1, creature_set = 0;
    i32 no_gun = 0, cheap = 0, ballooning = 0;
    AILOCATOR_PC *locators[64] = {NULL};
    ZIPUP_s *zipups[32];
    i16 models[10];
    char script[64] = "default";
    f32 xspacing = 0.8f, zspacing = 0.8f;
    NUVEC offset = {0.0f, 0.0f, 0.0f};
    AILOCATORSET_PC *locator_set = NULL;
    i32 zipdown = 0;
    i32 across = 2;
    i32 locator_count = 0, zipup_count = 0, model_count = 0;
    f32 fall_chance = 0.0f;
    f32 crate_height = 0.0f, crate_delay = 0.0f;
    GAMEOBJECT *rider = NULL;
    char *state = NULL;
    f32 max_range = 1000000000.0f, min_dy = 1000000000.0f, max_dy = 1000000000.0f;
    i32 grouped = 1;
    i64 add_capabilities = 0, remove_capabilities = 0;
    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL) {
        path = &packet->path_info;
        position = packet->owner->position;
        yaw = packet->owner->field_0x276;
    } else {
        yaw = 0;
        path = NULL;
    }
    for (index = 0; index < param_count; ++index) {
        char *value;
        if ((value = NuStrIStr(params[index], "mingroupsize")) != NULL)
            minimum = (i32)(AIParamToFloat(processor, value + 13));
        else if ((value = NuStrIStr(params[index], "maxgroupsize")) != NULL)
            maximum = (i32)(AIParamToFloat(processor, value + 13));
        else if ((value = NuStrIStr(params[index], "groupsize")) != NULL)
            count = (i32)(AIParamToFloat(processor, value + 10));
        else if ((value = NuStrIStr(params[index], "start_locator_set=")) != NULL) {
            value += 18;
            if (NuStrICmp("myset", value) == 0)
                locator_set = processor->locator_set;
            else
                locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value);
            if (locator_set != NULL)
                start_locator = 1;
        } else if (NuStrICmp("dont_inherit_locator_set", params[index]) == 0)
            inherit = 0;
        else if ((value = NuStrIStr(params[index], "locator_set=")) != NULL) {
            value += 12;
            if (NuStrICmp("myset", value) == 0)
                locator_set = processor->locator_set;
            else
                locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value);
            if (locator_set != NULL) {
                i32 i;
                AILocatorSet_CheckLocatorsStillAssigned(sys, locator_set);
                for (i = 0; i < locator_set->locator_count && locator_count < 64; ++i) {
                    if (locator_set->assigned[i] == 0xff)
                        locators[locator_count++] = &sys->locators[locator_set->locator_entries[i]];
                }
            }
        } else if ((value = NuStrIStr(params[index], "locator=")) != NULL) {
            if (locator_count < 64) {
                AILOCATOR_PC *locator;
                value += 8;
                if (NuStrICmp("mylocator", value) == 0)
                    locator = processor->locator;
                else
                    locator = AIPathFindLocator(sys, value);
                if (locator != NULL)
                    locators[locator_count++] = locator;
            }
        } else if ((value = NuStrIStr(params[index], "max_range_to_player=")) != NULL)
            max_range = AIParamToFloat(processor, value + 20);
        else if ((value = NuStrIStr(params[index], "mindy=")) != NULL)
            min_dy = AIParamToFloat(processor, value + 6);
        else if ((value = NuStrIStr(params[index], "maxdy=")) != NULL)
            max_dy = AIParamToFloat(processor, value + 6);
        else if ((value = NuStrIStr(params[index], "zipup")) != NULL) {
            if (locator_count < 32) {
                ZIPUP_s *zipup;
                value += 6;
                zipup = FindSpawnZipUp(value);
                if (zipup != NULL)
                    zipups[zipup_count++] = zipup;
            }
        } else if (NuStrIStr(params[index], "type=randommap") != NULL) {
            if (WORLD->current_level == HUB_LDATA) {
                i16 model = Hub_GetRandomCharType();
                if (model != -1 && model_count < 10)
                    models[model_count++] = model;
            }
        } else if (NuStrIStr(params[index], "type=generic_goon") != NULL) {
            if (model_count < 10) {
                i16 model = GetGenericGoon(0);
                if (model != -1)
                    models[model_count++] = model;
            }
        } else if ((value = NuStrIStr(params[index], "type")) != NULL) {
            value += 5;
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                if (WORLD->current_level != HUB_LDATA) {
                    u8 type = LevelCharacterTypeIDFn(value);
                    if (type != 0xff) {
                        i16 model = LevelCharacterGlobalIDFn(type);
                        if (model != -1 && model_count < 10)
                            models[model_count++] = model;
                    }
                } else {
                    i16 model;
                    if (NuStrICmp("Barman", value) == 0)
                        model = id_BARMAN;
                    else if (NuStrICmp("JABBA", value) == 0)
                        model = DefaultAnimationJabbaIdPc;
                    else if (NuStrICmp("CANTINABAND", value) == 0)
                        model = id_CANTINABAND;
                    else
                        continue;
                    if (apicharsys->playermodelids[model] != -1 && model_count < 10)
                        models[model_count++] = model;
                }
            }
        } else if ((value = NuStrIStr(params[index], "state=")) != NULL)
            state = value + 6;
        else if ((value = NuStrIStr(params[index], "script")) != NULL)
            NuStrCpy(script, value + 7);
        else if ((value = NuStrIStr(params[index], "xspacing")) != NULL)
            xspacing = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[index], "zspacing")) != NULL)
            zspacing = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[index], "xoffset")) != NULL)
            offset.x = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "yoffset")) != NULL)
            offset.y = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "zoffset")) != NULL)
            offset.z = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "nacross")) != NULL)
            across = (i32)(AIParamToFloat(processor, value + 8));
        else if (NuStrIStr(params[index], "addtoset=myset") != NULL)
            creature_set = processor->creature_set;
        else if ((value = NuStrIStr(params[index], "addtoset=")) != NULL)
            creature_set = (i32)(AIParamToFloat(processor, value + 9));
        else if ((value = NuStrIStr(params[index], "crate_height=")) != NULL)
            crate_height = AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[index], "crate_delay=")) != NULL)
            crate_delay = AIParamToFloat(processor, value + 12);
        else if (NuStrICmp("not_grouped", params[index]) == 0)
            grouped = 0;
        else if (NuStrICmp("dont_set_on_surface", params[index]) == 0)
            surface = 0;
        else if (NuStrICmp("zipdown", params[index]) == 0) {
            zipdown = 1;
            surface = 0;
        } else if (NuStrICmp("nearest_player", params[index]) == 0)
            nearest = 1;
        else if (NuStrICmp("off_screen", params[index]) == 0 || NuStrICmp("offscreen", params[index]) == 0)
            offscreen = 1;
        else if (NuStrICmp("on_screen", params[index]) == 0)
            onscreen = 1;
        else if ((value = NuStrIStr(params[index], "fall_chance=")) != NULL)
            fall_chance = AIParamToFloat(processor, value + 12);
        else if (NuStrICmp("CheapAsChips", params[index]) == 0)
            cheap = 1;
        else if (NuStrIStr(params[index], "ridden_by=myself") != NULL) {
            if (packet != NULL && packet->owner != NULL)
                rider = packet->owner->objptr;
        } else if ((value = NuStrIStr(params[index], "ridden_by=")) != NULL)
            rider = GetNamedGameObject(sys, value + 10);
        else if (NuStrICmp("no_gun", params[index]) == 0)
            no_gun = 1;
        else if (NuStrICmp("ballooning", params[index]) == 0) {
            ballooning = 1;
            surface = 0;
        } else if ((value = NuStrIStr(params[index], "lose_capability=")) != NULL)
            remove_capabilities |= (i32)(ParseAIPathCnxFlagPc(value + NuStrLen("lose_capability=")));
        else if ((value = NuStrIStr(params[index], "capability=")) != NULL)
            add_capabilities |= (i32)(ParseAIPathCnxFlagPc(value + NuStrLen("capability=")));
    }
    if (minimum >= 0) {
        if (maximum > minimum) {
            count = minimum;
            count += NuRand(NULL) % (maximum + 1 - minimum);
        } else if (maximum == minimum)
            count = maximum;
    }
    delay = 0.0f;
    offset_applied = 0;
    while (count != 0) {
        i32 model;
        AILOCATOR_PC *locator;
        ZIPUP_s *zipup;
        if ((locator_count | zipup_count) == 0 && rider == NULL && start_locator == 0)
            break;
        if (model_count == 0)
            break;
        model = models[NuRand(NULL) % model_count];
        locator = NULL;
        zipup = NULL;
        if ((locator_count | start_locator) != 0) {
            if (start_locator != 0 && locator_set != NULL) {
                locator = FirstSpawnSetLocator(locator_set, sys);
            } else {
                f32 clip_radius = 0.0f;
                if (offscreen != 0)
                    clip_radius = 0.5f + apicharsys->char_data[(i16)model].collision_radius;
                if (nearest != 0)
                    locator = LocalGetNearestLocator(locators, locator_count, clip_radius, &player->position, max_range,
                                                     onscreen, max_dy, min_dy);
                else if (max_range == 1000000000.0f)
                    locator = LocalGetRandomLocator(locators, locator_count, clip_radius, NULL, 1000000000.0f, onscreen,
                                                    max_dy, min_dy);
                else
                    locator = LocalGetRandomLocator(locators, locator_count, clip_radius, &player->collision_position,
                                                    max_range, onscreen, max_dy, min_dy);
            }
            if (locator == NULL)
                break;
            path = &locator->path_info;
            position = locator->position;
            yaw = locator->flags;
            offset_applied = 0;
        } else if (zipup_count != 0) {
            zipup = LocalGetRandomSpawnZipUp(zipups, zipup_count);
            if (zipup == NULL)
                break;
            position = zipup->lower_position;
            path = NULL;
            surface = 0;
        } else if (rider != NULL) {
            path = &rider->ai_packet.path_info;
            position = rider->ai_packet.last_path_position;
            yaw = rider->field_0x276;
        }
        if (offset_applied == 0 && (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f)) {
            NuVecRotateY(&offset, &offset, yaw);
            NuVecAdd(&position, &position, &offset);
            offset_applied = 1;
        }
        if (count > 0) {
            AIGROUP_PC *group;
            i32 member;
            i32 members = 1;
            i32 make_group = 0;
            if (grouped == 0)
                --count;
            else {
                members = count;
                make_group = count > 1;
                count = 0;
            }
            group = NULL;
            for (member = 0; member < members; ++member) {
                GAMEOBJECT *object;
                if (member == 0 && make_group != 0) {
                    PLAYERCHARACTERCONFIG_PC *data = apicharsys->char_data[model].game_character;
                    group = CreateAIGroup(sys, across, xspacing, zspacing, data->run_speed);
                }
                object = AddDynamicCreature(model, &position, yaw, script, path, group, surface, NULL, NULL, 0,
                                            creature_set);
                if (object == NULL)
                    continue;
                if (state != NULL)
                    AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, state);
                object->ai_packet.locator = locator;
                if (inherit != 0)
                    object->ai_packet.locator_set = locator_set;
                if (no_gun != 0)
                    object->portal_flags &= ~4;
                if (crate_height != 0.0f)
                    SpawnCreatureFromCrate(object, crate_height, delay);
                delay += crate_delay;
                if (locator_set != NULL && object->ai_packet.locator != NULL) {
                    i32 i;
                    for (i = 0; i < locator_set->locator_count; ++i) {
                        if (locator_set->locator_entries[i] == object->ai_packet.locator - sys->locators) {
                            locator_set->assigned[i] = object->hostility_group;
                            break;
                        }
                    }
                }
                if (zipup != NULL) {
                    ZIPUP_s *attached_zipup;
                    ZIPUP_s *current_zipup;
                    f32 dx;
                    f32 dz;
                    i32 rotation;
                    i32 angle;
                    object->attached_zipup = zipup;
                    zipup->runtime_flags |= 1;
                    attached_zipup = (ZIPUP_s *)object->attached_zipup;
                    object->context_variant_flags |= 0x20;
                    object->character_context = 0x47;
                    object->field_7aa = 0x2a;
                    object->movement_facing_angle =
                        NuAtan2D(attached_zipup->upper_position.x - attached_zipup->lower_position.x,
                                 attached_zipup->upper_position.z - attached_zipup->lower_position.z);
                    object->field_e22 |= 1;
                    object->velocity.y = 0.0f;
                    object->context_timer = 0.0f;
                    object->field_e33 = (u8)(object->weapon_scale < 1.0f);
                    PlaySfx("ZipUp", &object->collision_position);
                    PlayJumpSfx(object, 0);
                    object->jump_variant_state = 0;
                    current_zipup = (ZIPUP_s *)object->attached_zipup;
                    dx = current_zipup->hook_origin.x - attached_zipup->lower_position.x;
                    dz = current_zipup->hook_origin.z - attached_zipup->lower_position.z;
                    angle = NuAtan2D(current_zipup->hook_origin.y - attached_zipup->lower_position.y,
                                     NuFsqrt(dx * dx + dz * dz));
                    rotation = 0x4000 - (angle < 0 ? -angle : angle);
                    if (angle < 0)
                        rotation = -rotation;
                    object->field_7a4 = rotation;
                } else if (zipdown != 0 && !(fall_chance > 0.0f && fall_chance > NuRandFloat())) {
                    object->character_context = 0x35;
                    object->field_7aa = 0x2a;
                    object->external_force = object->upper_position;
                    SetWeaponOut(object);
                }
                if (cheap != 0) {
                    object->flags_1fc_word |= 0x420;
                }
                if (rider != NULL)
                    TakeOverGameObject(rider, object, 0, 1);
                else if (ballooning != 0) {
                    StartBallooning(object, 0);
                    if (locator != NULL)
                        SetBallooningHeight(object, locator->position.y);
                }
                if (add_capabilities != 0)
                    object->ai_packet.capabilities |= (u32)add_capabilities;
                if (remove_capabilities != 0)
                    object->ai_packet.capabilities &= ~(u32)remove_capabilities;
            }
        }
    }
    return 1;
}
