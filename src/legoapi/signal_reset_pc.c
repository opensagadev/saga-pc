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
