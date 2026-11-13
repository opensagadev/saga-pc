#include "saga.h"
#include <string.h>

/* Retail unit 0x413f00-0x42009e: definitions in retail address order. */

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
typedef struct PUSH_SUPPORT_BLOCK_VIEW_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
} PUSH_SUPPORT_BLOCK_VIEW_PC;
typedef struct PUSH_SUPPORT_WORLD_VIEW_PC {
    u8 unknown_000[0x46bc];
    PUSH_SUPPORT_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_SUPPORT_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_SUPPORT_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, count) == 0x46c0);
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
i32 __cdecl KillGameObject(GAMEOBJECT *object, i32 reason, i32 unused);
void __cdecl ReleaseTakeOver(GAMEOBJECT *target, i32 unused);
__declspec(noinline) i32 __cdecl SetObjOnSurface(GAMEOBJECT *object, i32 mode);
void __cdecl InitSurfaceInfo(GAMEOBJECT *object);
f32 __cdecl VehicleTurnOrLoopOffset(GAMEOBJECT *object);
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
void AICreatureResumeScript(GAMEOBJECT *object);

void __cdecl FixUpCharacters(CHARFIXUP_PC *fixup) {
    i32 i;
    for (i = 0; i < CHARCOUNT; ++i) {
        GCDataList[i] = GCDATA_DEFAULT;
        if (CDataList[i].name_id == -1)
            CDataList[i].name_id = tUNKNOWN;
        if (CDataList[i].move_fn == NULL)
            CDataList[i].move_fn = Move_CHARACTER;
        if (CDataList[i].animate_fn == NULL)
            CDataList[i].animate_fn = Animate_CHARACTER;
    }
    if (fixup != NULL) {
        while (fixup->name != NULL) {
            if (fixup->id != NULL)
                *fixup->id = (i16)CharIDFromName(fixup->name);
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
    for (i = 0; i < 8; ++i)
        packet->field_614[i] = 0;
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
    for (i = 0; i < 4; ++i)
        packet->random_state[i] = (u8)(qrand() >> 8);
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

void __cdecl DeactivateGameObject(GAMEOBJECT *object) {
    if (object == NULL)
        return;
    if (object->rider != NULL) {
        if ((object->flags_1f8 & 0x4000) != 0)
            ReleaseTakeOver(object, 1);
        else
            KillGameObject(object->rider, 4, 0);
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
    if (Mission_Active(NULL) != NULL || name == NULL)
        return;
    object = GetNamedGameObject(WORLD->ai_sys, name);
    if (object != NULL && (FreePlay == 0 || (object->flags_1f8 & 0x400) != 0)) {
        DeactivateGameObject(object);
    }
}

/* Original source name unknown. Retail places this with the game object code
   at 0x414d10, and PlayersDropInOut calls it rather than inlining it, so the
   definition lives in its own unit. */
void __cdecl PlayerSetCurrentHpPc(GAMEOBJECT *object, i8 hitpoints) {
    object->current_hp = hitpoints;
    if ((i32)hitpoints > (i32)object->field_107e)
        object->current_hp = object->field_107e;
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
    if (newgamecam != 0 || (object->flags_ef4 & 0x10000000) != 0)
        return;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *controller = Player[index];
        if (controller != NULL && controller->character_context == 0x51 && controller->techno != NULL &&
            controller->techno->controlled_object == object)
            controlled = 1;
    }
    if (!(object->flags_1fc & 0x80) && !controlled)
        return;
    if (object->dead != 0 || object->character_context == 0x2b)
        return;
    if (!(object->flags_ef0 & 0x40000))
        return;
    if (object->character_context == 0x0f || object->character_context == 0x47 || object->character_context == 0x1f ||
        object->character_context == 0x46 || object->character_context == 0x51)
        return;
    if (MiniCutCam != 0 || GetMenuID() != -1)
        return;
    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && GameCam->mode == 0x0b) {
        GunshipKeepOnScreenPc(object);
        return;
    }
    if (VehicleArea == 0) {
        if (object == Player[0] && Player[1] != NULL &&
            (!(((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) || ((GAMEOBJECT *)Player[1])->character_context == 0x2b))
            return;
        if (object == Player[1] && Player[0] != NULL &&
            (!(((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) || ((GAMEOBJECT *)Player[0])->character_context == 0x2b))
            return;
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
                            -object->bounds_half_height, NULL))
            index = !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? 5 : 0;
        else if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL,
                                 -object->bounds_half_height, NULL))
            index = !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? 0 : 5;
        else
            index = -1;
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

    if (object->model_draw_result != 0)
        object->field_f10 = 0.0f;
    else
        object->field_f10 += FRAMETIME;
    if (object->character_context == 0x2b)
        object->turn_braking += FRAMETIME;

    if ((f64)object->run_speed_override != MoveSplineRunSpeedSentinel)
        speed = object->run_speed_override;
    else
        speed = object->character_data->game_character->run_speed;
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
        if (WORLD->current_level == PODSPRINTA_LDATA)
            NewTerrPlatformsOff();
        object->floor_height = GameShadow(object, &position, 5.0f, 0x1f);
        if (object->floor_height == 2000000.0) {
            position.y = SeekValF(object->position.y, hover, 5.0f);
        } else {
            hover += object->floor_height;
            position.y = SeekValF(object->position.y, hover, 5.0f);
            if (position.y < object->floor_height)
                position.y = object->floor_height;
        }
        if (WORLD->current_level == PODSPRINTA_LDATA)
            position.y += object->movement_spline_offset.y;
    }
    if (object->delayed_turn_timer > 0.0f && object->character_context != 0x2a && object->character_context != 0x36 &&
        object->character_context != 0x3a)
        object->delayed_turn_timer -= FRAMETIME;
    if (WORLD->current_level == SPEEDERCHASEA_LDATA)
        position.y += VehicleTurnOrLoopOffset(object);

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
            if (roll < -0x10000)
                roll = -0x10000;
            else if (roll > 0x10000)
                roll = 0x10000;
        }
        target_roll = roll / 4;
        if (target_roll < -0x2000)
            target_roll = -0x2000;
        else if (target_roll > 0x2000)
            target_roll = 0x2000;
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
    if (pad->input_magnitude != pad->previous_input_magnitude)
        pad->runtime_flags |= 0x40;
    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->flags_ef0 &= ~0x4000;
    object->previous_movement_angle = object->field_0x276;
    if (object->model_draw_result != 0)
        object->field_f10 = 0.0f;
    else
        object->field_f10 += FRAMETIME;
    if (object->character_context == 0x2b)
        object->turn_braking += FRAMETIME;
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
                } else if (object->pad == ViewCamGetGamePad())
                    pad->input_magnitude = 0.0f;
                else {
                    buttons = pad->buttons_held;
                    if ((buttons & (GAMEPAD_DUP | GAMEPAD_DDOWN | GAMEPAD_DLEFT | GAMEPAD_DRIGHT)) != 0) {
                        x = (buttons & GAMEPAD_DLEFT) != 0 ? -127.5f : (buttons & GAMEPAD_DRIGHT) != 0 ? 127.5f : 0.0f;
                        z = (buttons & GAMEPAD_DDOWN) != 0 ? -127.5f : (buttons & GAMEPAD_DUP) != 0 ? 127.5f : 0.0f;
                        if (x != 0.0f || z != 0.0f)
                            pad->input_mode = 1;
                    } else {
                        x = (f32)((f64)analog_x - 127.5);
                        z = (f32) - ((f64)analog_y - 127.5);
                        if (x * x + z * z < 1806.25)
                            x = z = 0.0f;
                        else if (x != 0.0f || z != 0.0f)
                            pad->input_mode = 2;
                    }
                    x = (f32)(x * (f64)0.007843137718737125f);
                    z = (f32)(z * (f64)0.007843137718737125f);
                    angle = NuAtan2D(x, z);
                    axis.x = axis.y = 0.0f;
                    axis.z = MovePlayerFabs(x) > MovePlayerFabs(z) ? MovePlayerFabs(x) : MovePlayerFabs(z);
                    NuVecRotateY(&input, &axis, angle);
                    magnitude = NuFsqrt(input.x * input.x + input.z * input.z);
                    if (object == Player[0])
                        index = 0;
                    else if (object == Player[1])
                        index = 1;
                    else
                        goto skip_pad_history;
                    {
                        PlayerPadPreviousSpeed[index] = PlayerPadCurrentSpeed[index];
                        PlayerPadPreviousAngle[index] = PlayerPadCurrentAngle[index];
                        PlayerPadCurrentSpeed[index] = pad->input_magnitude;
                        PlayerPadCurrentAngle[index] = pad->input_angle;
                    }
                skip_pad_history:
                    if ((f64)0.2f > magnitude)
                        pad->input_magnitude = 0.0f;
                    else {
                        if ((object->character_data->model_flags & 0x20) != 0 &&
                            ((object->player_index != -1 && object->vehicle_secondary_hp < 3) ||
                             (object->player_index == -1 && object->current_hp < 3)))
                            pad->input_magnitude = object->character_data->game_character->walk_speed;
                        else if (magnitude < 0.5f && (object->character_data->game_character->flags_090 & 8) == 0)
                            pad->input_magnitude = object->character_data->game_character->tiptoe_speed;
                        else if (magnitude < (f64)0.8f)
                            pad->input_magnitude = object->character_data->game_character->walk_speed;
                        else
                            pad->input_magnitude = object->character_data->game_character->run_speed;
                    }
                    pad->input_direction_z = input.x;
                    pad->input_direction_x = input.z;
                    if (pad->input_magnitude > 0.0f)
                        pad->input_angle = NuAtan2D(pad->input_direction_z, input.z);
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
                } else if (object->character_context == 0x17 && (object->field_ef8 & 1) == 0)
                    pad->input_magnitude = 0.0f;
                else {
                    axis.x = object->ai_packet.movement_position.x - object->position.x;
                    axis.z = object->ai_packet.movement_position.z - object->position.z;
                    if (axis.x == 0.0f && axis.z == 0.0f) {
                        pad->input_angle = 0;
                        pad->input_direction_x = pad->input_direction_z = pad->input_magnitude = 0.0f;
                    } else {
                        magnitude = NuFsqrt(axis.x * axis.x + axis.z * axis.z);
                        pad->input_magnitude = magnitude / ai_moveradius;
                        if (pad->input_magnitude > 1.0f)
                            pad->input_magnitude = 1.0f;
                        inverse = 1.0f / magnitude;
                        pad->input_direction_x = inverse * axis.x;
                        pad->input_direction_z = inverse * axis.z;
                        pad->input_angle = NuAtan2D(pad->input_direction_x, pad->input_direction_z);
                        pad->input_angle = NuAngSub(pad->input_angle, GameCam->input_yaw);
                    }
                    pad->input_mode = 1;
                }
                if (object->context_target_position != NULL)
                    pad->input_magnitude = 0.0f;
                else {
                    run_speed = object->run_speed_override != 1000000000.0
                                    ? object->run_speed_override
                                    : object->character_data->game_character->run_speed;
                    walk_speed = object->walk_speed_override != 1000000000.0
                                     ? object->walk_speed_override
                                     : object->character_data->game_character->walk_speed;
                    if ((object->ai_packet.flags & 0x40000) != 0)
                        pad->input_magnitude = 0.0f;
                    else
                        pad->input_magnitude *= run_speed;
                    if ((object->ai_packet.flags & 0x80000) == 0 &&
                        pad->input_magnitude < object->character_data->game_character->tiptoe_speed * 0.5)
                        pad->input_magnitude = 0.0f;
                    if ((object->ai_packet.flags & 8) == 0) {
                        if (object->ai_packet.goal_speed_mode != 0) {
                            if (object->ai_packet.goal_speed_mode == 1) {
                                if (pad->input_magnitude > walk_speed)
                                    pad->input_magnitude = walk_speed;
                            } else if (object->ai_packet.goal_speed_mode == 2) {
                                if (pad->input_magnitude > object->character_data->game_character->tiptoe_speed)
                                    pad->input_magnitude = object->character_data->game_character->tiptoe_speed;
                            }
                        }
                    }
                    if ((object->flags_1f8 & 0x400) == 0 && (object->portal_flags & 0x2000000) == 0 &&
                        (object->field_f10 >= OFFSCREEN_CATCHUP_TIME ||
                         (object->model_draw_result == 0 && (object->tag_flags & 2) == 0 &&
                          drop_back_in_timer > 0.0f)) &&
                        (pad->input_magnitude + (f64)0.001f >= walk_speed || object->id == (u16)id_SPEEDERBIKE))
                        object->flags_ef0 |= 0x400;
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
            if (pad->input_magnitude > 0.0f)
                pad->animation_input_magnitude = pad->input_magnitude;
            if (object->character_context == 0x3b || object->character_context == 0x39)
                return;
            object->movement_mode = 2;
            ShoveSystemCheckGameObject(object);
            if (object->id == (u16)id_DRAGBOMB)
                MovePlayer_ROLLING(object);
            else if (!MovePlayer_TWIST(object) && !MovePlayer_CIRCLE(object) && !MovePlayer_GUNSHIPIN(object) &&
                     !MovePlayer_POD(object)) {
                if ((object->character_data->model_flags & 0x2000) != 0)
                    MovePlayer_VEHICLEDIRECTIONAL(object);
                else
                    MovePlayer_DIRECTIONAL(object);
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
                    if (object->techno->controlled_player->character_context == 0x33)
                        object->techno->controlled_player->character_context = -1;
                } else {
                    object->position.y += FRAMETIME * 1.5;
                    object->velocity.y = 0.0f;
                    object->field_281 = 0;
                    object->lift_clear_state = 0;
                    if (object->character_context == 0x33)
                        object->character_context = -1;
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
                    0x82)
                    special_path_endpoints = 1;
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
                object->character_context != 0x1e)
                api->flags_1fc_word |= 4;
            else
                api->flags_1fc_word &= ~4u;
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
                                            if (object->vehicle_hit_timer <= 0.0f)
                                                ObjHitObj(NULL, object, 1, 0, 0, 1);
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
    if ((object->flags_1f8 & 0x40000) == 0)
        object->character_data->animate_fn(object);
    override_from = object->ai_packet.animation_override_from;
    if (override_from != -1 && ((override_from == 0xe9 && object->character_context != 0x1c) ||
                                object->anim_packet.requested_animation == override_from))
        object->anim_packet.requested_animation = object->ai_packet.animation_override_to;
    if (model == NULL || (object->dead != 0 && object->death_delay == 0.0f) || object->dead == 1)
        return;
    direction = 1.0f;
    if ((object->flags_ef0 & 0x4000) != 0)
        direction = -1.0f;
    if (object->character_model->action_animations[1] == NULL) {
        if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
            movement_speed =
                object->velocity.x * object->facing_direction.x + object->velocity.z * object->facing_direction.z;
            if (movement_speed < 0.0f)
                movement_speed = 0.0f;
        } else {
            movement_speed = pad->input_magnitude;
        }
    } else {
        movement_speed = pad->animation_input_magnitude;
        if (movement_speed > 0.0f && (u16)object->id == id_GONKDROID && Cheat_IsOn(8) == 0)
            movement_speed = object->character_data->game_character->walk_speed;
    }
    multiplier = (object->character_context == 0x2d && object->grapple != NULL) ? GizBuildItMul(object) : 1.0f;
    UpdateAnimPacket(model, &object->anim_packet, (f32)((f64)FRAMETIME * 30.0 * (f64)multiplier),
                     movement_speed * direction, multiplier * FRAMETIME,
                     object->character_data->game_character->backwards_factor);
    if ((object->anim_packet.flags & 4) != 0) {
        time = NULL;
        if (object->anim_packet.blending != 0) {
            if (object->anim_packet.blend_target_animation == 0x5f)
                time = &object->anim_packet.blend_target_time;
        } else if (object->anim_packet.animation_index == 0x5f) {
            time = &object->anim_packet.current_time;
        }
        if (time != NULL) {
            f32 duration = AnimDuration((i16)object->id, 0x5f, 0.0f, 0.0f, 0);
            i32 count = (i32)((f64)duration / (f64)0.3f);
            i32 phase;
            f32 start;
            if (NuFmod(duration, 3.0f) > 0.15f)
                ++count;
            phase = qrand() / (0xffff / count + 1);
            model = object->character_model;
            start = (f32)((f64)phase * ((f64)0.3f * (f64)model->animation_configs[0x5f]->playback_rate) + 1.0);
            if (start < NuAnimEndFrame(model->action_animations[0x5f]))
                *time = start;
            object->flags_e20 = (object->flags_e20 & ~0x4000u) | ((phase & 1) << 14);
        }
    }
    AutoWeaponOnOff(object);
    AddFootSteps(object);
}

i32 __cdecl AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == (u16)id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

i32 __cdecl ActivePlayerInRange(NUVEC *position, f32 range_squared, f32 *distance_squared) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0) {
            f32 distance = NuVecDistSqr(&object->collision_position, position, NULL);
            if (distance < range_squared) {
                if (distance_squared != NULL)
                    *distance_squared = distance;
                return 1;
            }
        }
    }
    return 0;
}

i32 __cdecl SuperWeirdo(GAMEOBJECT *object) {
    if ((object->flags_1fc & 0x80) != 0 && (Game.customizer_weirdo_flags & 1) != 0 && CharacterCustomiser != NULL &&
        (object->id == (u16)CharacterCustomiser->character_ids[0] ||
         object->id == (u16)CharacterCustomiser->character_ids[1]))
        return 1;
    return 0;
}

void __cdecl ResetAICreature(GAMEOBJECT *object, AISYS_PC *system) {
    AICREATURE_PC *creature;
    AIPATHINFO_PC *path_info;
    AIROW_PC *row;
    NUVEC offset;
    i32 angle;
    if (object == NULL)
        return;
    PreResetCode(object);
    PostResetCode(object);
    if (system == NULL)
        return;
    InitPlayerAI(object);
    ResetPlayerMoves(object);
    Player_ClearContext(object, 1);
    Player_ResetContexts(object->unknown_06c4);
    object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    creature = &system->creatures[object->ai_packet.creature_index];
    object->ai_packet.creature_set = ((AI_CREATURE_RESET_VIEW_PC *)creature)->creature_set;
    if ((creature->flags & 1) != 0)
        object->ai_packet.flags |= 0x80;
    else
        object->ai_packet.flags &= ~0x80u;
    object->ai_packet.flags &= ~0x40000u;
    object->view_distance = creature->view_distance;
    object->hear_distance = creature->hear_distance;
    object->max_view_height = creature->max_view_height;
    object->min_view_height = creature->min_view_height;
    offset.x = (f32)((object->ai_packet.group_column + 1) / 2) * creature->x_spacing;
    if ((object->ai_packet.group_column & 1) != 0)
        offset.x = -offset.x;
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
    if (system == NULL || (((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags & 1) != 0)
        return;
    ((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags |= 1;
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1f8 & 0x400) == 0)
            continue;
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
                NuStrICmp(object->ai_packet.script_processor.state->name, GameObjectState_InActive) == 0)
                creature->activation_type = 2;
            if (creature->activation_type != 0)
                continue;
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

GAMEOBJECT *__cdecl ActivateCharacter(char *name, NUVEC *position, i32 angle) {
    GAMEOBJECT *object;
    if (Mission_Active(NULL) != NULL)
        return NULL;
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
