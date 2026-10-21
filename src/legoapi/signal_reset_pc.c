#include "saga.h"

STATIC_ASSERT(sizeof(SIGNAL_s) == 0x94);
STATIC_ASSERT(offsetof(SIGNAL_s, matrix) == 0x10);
STATIC_ASSERT(offsetof(SIGNAL_s, position) == 0x50);
STATIC_ASSERT(offsetof(SIGNAL_s, yaw) == 0x5c);
STATIC_ASSERT(offsetof(SIGNAL_s, terrain_pitch) == 0x5e);
STATIC_ASSERT(offsetof(SIGNAL_s, terrain_roll) == 0x60);
STATIC_ASSERT(offsetof(SIGNAL_s, flags) == 0x6a);
STATIC_ASSERT(offsetof(SIGNAL_s, radius) == 0x6c);
STATIC_ASSERT(offsetof(SIGNAL_s, scale) == 0x70);
STATIC_ASSERT(offsetof(SIGNAL_s, normal) == 0x74);
STATIC_ASSERT(offsetof(SIGNAL_s, suit) == 0x80);
STATIC_ASSERT(offsetof(SIGNAL_s, target_position) == 0x84);
STATIC_ASSERT(offsetof(SIGNAL_s, platform_id) == 0x90);
STATIC_ASSERT(offsetof(WORLDINFO, signals) == 0x5060);
STATIC_ASSERT(offsetof(WORLDINFO, signal_count) == 0x5064);
STATIC_ASSERT(sizeof(SIGNALPROGRESS_s) == 0x28);
STATIC_ASSERT(offsetof(SIGNALPROGRESS_s, active_mask) == 0x20);
STATIC_ASSERT(offsetof(SIGNALPROGRESS_s, visible_mask) == 0x24);

SUIT_INITIALIZER_RECORD_PC *__cdecl Suit_FindFromLetter(char letter) {
    i32 index;
    for (index = 0; index < 10; ++index) {
        if (SuitRecordInitPc[index].letter == letter)
            return &SuitRecordInitPc[index];
    }
    return NULL;
}

static void SignalSetVisiblePc(SIGNAL_s *signal, i32 visible) {
    u8 was_visible = signal->flags & 1;
    signal->flags = (signal->flags & ~1) | (visible != 0);
    if (visible != 0) {
        if (was_visible == 0)
            signal->platform_id = NewPlatPickupInst(&signal->matrix, 2);
    } else if (was_visible != 0) {
        DeletePlatinst(signal->platform_id);
    }
}

void __cdecl Signal_SetGizmoVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL)
        SignalSetVisiblePc((SIGNAL_s *)gizmo->object, visible);
}

static void SignalResetRecordPc(SIGNAL_s *signal) {
    f32 height;
    NewTerrPlatformsOff();
    height = GameShadow(NULL, &signal->position, 5.0f, -1);
    if (height != 2000000.0f) {
        signal->position.y = height;
        FindAnglesZX(&ShadNorm, &signal->terrain_pitch, &signal->terrain_roll);
    }
    signal->radius = 0.85f;
    signal->scale = 1.0f;
    NuVecRotateZ(&signal->normal, &v010, signal->terrain_roll);
    NuVecRotateX(&signal->normal, &signal->normal, signal->terrain_pitch);
    NuVecScale(&signal->target_position, &signal->normal, 0.6f);
    NuVecAdd(&signal->target_position, &signal->target_position, &signal->position);
    signal->suit = Suit_FindFromLetter(signal->suit_letter);
    if (signal->suit == NULL)
        signal->suit = &SuitRecordInitPc[0];
    signal->character_letter = signal->suit->group;
    signal->suit_letter = signal->suit->letter;
    if (signal->character_letter == 'r') {
        signal->animation_0 = 0x3a;
        signal->animation_1 = 0x3b;
        signal->animation_2 = 0x3c;
    } else {
        signal->animation_0 = 0x37;
        signal->animation_1 = 0x38;
        signal->animation_2 = 0x39;
    }
    signal->flags = (signal->flags & ~4) | 3;
    NuMtxSetRotationY(&signal->matrix, signal->yaw);
    NuMtxRotateZ(&signal->matrix, signal->terrain_roll);
    NuMtxRotateX(&signal->matrix, signal->terrain_pitch);
    NuMtxTranslate(&signal->matrix, &signal->position);
}

void __cdecl Signals_Reset(WORLDINFO *world, void *unused, SIGNALPROGRESS_s *progress) {
    SIGNAL_s *signal;
    i32 index;
    if (world == NULL || world->signals == NULL)
        return;
    signal = world->signals;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        u32 mask;
        SignalResetRecordPc(signal);
        if (progress == NULL || index >= 32)
            continue;
        mask = 1u << index;
        if (progress->suit_letters[index] != 0)
            signal->suit = Suit_FindFromLetter(progress->suit_letters[index]);
        SignalSetVisiblePc(signal, (progress->visible_mask & mask) != 0);
        signal->flags = (signal->flags & ~2) | (((progress->active_mask & mask) != 0) << 1);
    }
}

extern const char AIRegistryTextPc007575bc[7];

STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_signals) == 0xea);
STATIC_ASSERT(offsetof(SIGNAL_s, name) == 0);
STATIC_ASSERT(sizeof(((SIGNAL_s *)0)->name) == 16);
STATIC_ASSERT(offsetof(SIGNAL_s, character_letter) == 0x68);
STATIC_ASSERT(offsetof(SIGNAL_s, suit_letter) == 0x69);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

void *__cdecl Signals_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->signals = NULL;
    world->signal_count = 0;
    if (level->max_signals != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->signals = (SIGNAL_s *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_signals * sizeof(SIGNAL_s);
    }
    return world->signals;
}

i32 __cdecl Signals_Load(WORLDINFO *world, void *unused) {
    i32 version;
    i32 index;
    if (world->signal_count != 0)
        return 0;
    version = EdFileReadInt();
    world->signal_count = EdFileReadInt();
    for (index = 0; index < world->signal_count; ++index) {
        EdFileRead(world->signals[index].name, 16);
        EdFileReadNuVec(&world->signals[index].position);
        world->signals[index].character_letter = EdFileReadChar();
        if (version > 1) {
            world->signals[index].suit_letter = EdFileReadChar();
            world->signals[index].yaw = EdFileReadShort();
        } else {
            world->signals[index].suit_letter = 'b';
            world->signals[index].yaw = 0;
        }
    }
    return 1;
}

void *__cdecl Signals_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(SIGNALPROGRESS_s));
}

void __cdecl Signals_ClearProgress(void *unused, SIGNALPROGRESS_s *progress) {
    if (progress != NULL) {
        memset(progress->suit_letters, 0, sizeof(progress->suit_letters));
        progress->active_mask = 0xffffffffu;
        progress->visible_mask = 0xffffffffu;
    }
}

void __cdecl Signals_StoreProgress(WORLDINFO *world, void *unused, SIGNALPROGRESS_s *progress) {
    SIGNAL_s *signal;
    i32 index;
    Signals_ClearProgress(world, progress);
    if (progress == NULL || world == NULL || world->signals == NULL)
        return;
    signal = world->signals;
    for (index = 0; index < world->signal_count && index < 32; ++index, ++signal) {
        u32 mask = 1u << index;
        progress->suit_letters[index] = world->signals[index].suit->letter;
        if ((signal->flags & 1) == 0)
            progress->visible_mask &= ~mask;
        if ((signal->flags & 2) == 0)
            progress->active_mask &= ~mask;
    }
}

void __cdecl Signals_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->signal_count; ++index) {
        i16 platform_id = NewPlatPickupInst(&world->signals[index].matrix, 2);
        world->signals[index].platform_id = platform_id;
    }
}

i32 __cdecl Signals_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_signals : 0;
}

void __cdecl Signals_AddGizmos(void *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->signal_count; ++index) {
        if (NuStrLen(world->signals[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->signals[index]);
    }
}

char *__cdecl Signal_GetGizmoName(GIZMO_PC *gizmo) {
    SIGNAL_s *signal = gizmo != NULL ? (SIGNAL_s *)gizmo->object : NULL;
    return signal != NULL ? signal->name : NULL;
}

NUVEC *__cdecl Signal_GetPos(GIZMO_PC *gizmo) {
    SIGNAL_s *signal = gizmo != NULL ? (SIGNAL_s *)gizmo->object : NULL;
    return signal != NULL ? &signal->position : NULL;
}

void __cdecl Signal_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        SIGNAL_s *signal = (SIGNAL_s *)gizmo->object;
        signal->flags = (signal->flags & ~2) | ((active != 0) << 1);
    }
}

i32 __cdecl Signal_ActivateRev(GIZMO_PC *gizmo, i32 active, i32 reverse) {
    SIGNAL_s *signal;
    if (gizmo == NULL)
        return 0;
    signal = (SIGNAL_s *)gizmo->object;
    if (signal == NULL)
        return 0;
    if ((reverse & 1) == 0) {
        signal->flags = (signal->flags & ~2) | ((active == 0) << 1);
        return 1;
    }
    return ((signal->flags & 2) != 0) == active;
}

i32 __cdecl Signal_GetOutput(GIZMO_PC *gizmo, i32 unused_index, i32 unused_mode) {
    SIGNAL_s *signal = (SIGNAL_s *)gizmo->object;
    return (signal->flags & 3) == 3;
}

char *__cdecl Signal_GetOutputName(GIZMO_PC *unused, i32 unused_index) {
    return (char *)AIRegistryTextPc007575bc;
}

i32 __cdecl Signal_GetNumOutputs(GIZMO_PC *unused) {
    return 1;
}

STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, character_id) == 0x14);
STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, text_id) == 8);
STATIC_ASSERT(offsetof(GAMECAMERA, position) == 0x11c);

void __cdecl Signals_Update(WORLDINFO *world, void *unused, f32 unused_time) {
    SIGNAL_s *signal = world->signals;
    i32 index;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        f32 target_scale = 0.0f;
        signal->radius = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.1f + (f64)0.8f);
        if ((signal->flags & 3) != 3) {
            signal->scale = 0.0f;
            signal->flags &= ~4;
            continue;
        }
        if ((signal->flags & 4) != 0) {
            i32 player_index;
            for (player_index = 0; player_index < 8; ++player_index) {
                GAMEOBJECT *player = (GAMEOBJECT *)Player[player_index];
                if (player != NULL && (player->flags_1fc_word & 0x1001) == 0x1001 &&
                    player->id == (u16)signal->suit->character_id &&
                    (f64)NuVecDistSqr(&player->collision_position, &signal->target_position, NULL) <
                        0.49000006914138794) {
                    NUVEC offset;
                    f32 along_normal;
                    f32 distance;
                    f32 radius;
                    f32 lateral_squared;
                    NuVecSub(&offset, &player->collision_position, &signal->position);
                    along_normal = NuVecDot(&offset, &signal->normal);
                    distance = NuVecMag(&offset);
                    radius = (f32)((f64)player->bounds_radius + (f64)0.3f);
                    lateral_squared = (f32)((f64)distance * distance - (f64)along_normal * along_normal);
                    if ((f64)lateral_squared < (f64)radius * radius)
                        break;
                }
            }
            if (player_index == 8)
                signal->flags &= ~4;
        } else {
            ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
            message.text = TTab[*signal->suit->text_id];
            message.position = &signal->target_position;
            message.scale = 1.0f;
            message.red = 0xff;
            message.green = 0xff;
            message.blue = 0;
            message.alpha = 0x30;
            message.flags = 0x87;
            AddGameMsg(&message);
            target_scale = 1.0f;
        }
        signal->scale = SeekLinearF(signal->scale, target_scale, (f32)(3.0 * FRAMETIME));
    }
}

void __cdecl Signals_Draw(WORLDINFO *world, void *unused, f32 unused_time) {
    SIGNAL_s *signal = world->signals;
    i32 index;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        NUMTX matrix;
        if ((signal->flags & 1) == 0)
            continue;
        matrix = signal->matrix;
        if (world->level_objects[0x36].active != 0)
            NuSpecialDrawAt(&world->level_objects[0x36].special, &matrix);
        if ((signal->flags & 2) != 0) {
            if (world->level_objects[signal->animation_1].active != 0)
                NuSpecialDrawAt(&world->level_objects[signal->animation_1].special, &matrix);
            if (world->level_objects[signal->animation_2].active != 0) {
                f32 alpha = signal->radius;
                NUVEC offset;
                f32 along_normal;
                f32 distance;
                f32 lateral_squared;
                NuVecSub(&offset, &GameCam->position, &signal->position);
                along_normal = NuVecDot(&offset, &signal->normal);
                distance = NuVecMag(&offset);
                lateral_squared = (f32)((f64)distance * distance - (f64)along_normal * along_normal);
                if (lateral_squared < 2.25f) {
                    f32 fade;
                    if (lateral_squared <= 0.5625f)
                        fade = 0.0f;
                    else
                        fade = (f32)(((f64)NuFsqrt(lateral_squared) - 0.75) / 0.75);
                    if (along_normal < 0.0f || along_normal > 2.0f) {
                        if (along_normal >= 0.0f)
                            along_normal = (f32)((f64)along_normal - 2.0);
                        else
                            along_normal = -along_normal;
                        if (along_normal < 0.75f)
                            fade = (f32)((f64)fade + (1.0 - (f64)fade) * ((f64)along_normal / 0.75));
                        else
                            fade = 1.0f;
                    }
                    alpha *= fade;
                }
                alpha *= signal->scale;
                if (alpha > 0.0f)
                    NuSpecialDrawAtAlpha(&world->level_objects[signal->animation_2].special, &matrix, alpha);
            }
        } else if (world->level_objects[signal->animation_0].active != 0) {
            NuSpecialDrawAt(&world->level_objects[signal->animation_0].special, &matrix);
        }
    }
}

extern const char PCContextName_Signal[7];
extern char TextInputKeyNameTextPc7516c0[1];

#pragma data_seg(".data$SignalRegistryNamePc")
char *SignalRegistryNamePc = (char *)PCContextName_Signal;
#pragma data_seg()
#pragma data_seg(".data$SignalAddTypePc")
ADDGIZMOTYPE_PC SignalAddTypePc = {0};
#pragma data_seg()

STATIC_ASSERT(sizeof(ADDGIZMOTYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADDGIZMOTYPE_PC, fns) == 8);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, options) == 0);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, get_max_gizmos_fn) == 4);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, load_fn) == 0x64);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, add_level_sfx_fn) == 0x6c);

ADDGIZMOTYPE_PC *__cdecl Signals_RegisterGizmo(i32 unused_type_id) {
    char *name = SignalRegistryNamePc;
    SignalAddTypePc = Default_ADDGIZMOTYPE;
    SignalAddTypePc.name = name;
    SignalAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    SignalAddTypePc.fns.options = 0x28;
    SignalAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Signals_GetMaxGizmos;
    SignalAddTypePc.fns.add_gizmos_fn = (void *)Signals_AddGizmos;
    SignalAddTypePc.fns.early_update_fn = NULL;
    SignalAddTypePc.fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))Signals_Update;
    SignalAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))Signals_Draw;
    SignalAddTypePc.fns.panel_draw_fn = NULL;
    SignalAddTypePc.fns.get_gizmo_name_fn = Signal_GetGizmoName;
    SignalAddTypePc.fns.get_output_fn = Signal_GetOutput;
    SignalAddTypePc.fns.get_output_name_fn = Signal_GetOutputName;
    SignalAddTypePc.fns.get_num_outputs_fn = (void *)Signal_GetNumOutputs;
    SignalAddTypePc.fns.activate_fn = Signal_Activate;
    SignalAddTypePc.fns.activate_rev_fn = Signal_ActivateRev;
    SignalAddTypePc.fns.set_visibility_fn = Signal_SetGizmoVisibility;
    SignalAddTypePc.fns.get_visibility_fn = NULL;
    SignalAddTypePc.fns.get_pos_fn = (void *)Signal_GetPos;
    SignalAddTypePc.fns.using_special_fn = NULL;
    SignalAddTypePc.fns.bolt_hit_plat_fn = NULL;
    SignalAddTypePc.fns.get_best_bolt_target_fn = NULL;
    SignalAddTypePc.fns.bolt_hit_fn = NULL;
    SignalAddTypePc.fns.allocate_progress_data_fn = Signals_AllocateProgressData;
    SignalAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))Signals_ClearProgress;
    SignalAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))Signals_StoreProgress;
    SignalAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))Signals_Reset;
    SignalAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))Signals_ReserveBufferSpace;
    SignalAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))Signals_Load;
    SignalAddTypePc.fns.post_load_fn = NULL;
    SignalAddTypePc.fns.add_level_sfx_fn = NULL;
    return &SignalAddTypePc;
}
