#include "saga.h"

STATIC_ASSERT(offsetof(HINTSYS_PC, active_hint) == 8);
STATIC_ASSERT(offsetof(HINT_PC, id) == 0);

void __cdecl RegisterWithHintSys(void(__cdecl *draw_fn)(HINT_PC *, i32), HINT_PC *hints, u32 *save_bits,
                                 i32 save_word_count) {
    hintsys.display_elapsed = 0.0f;
    hintsys.draw_fn = draw_fn;
    hintsys.hints = hints;
    hintsys.save_bits = save_bits;
    hintsys.save_word_count = save_word_count;
    hintsys.state = 2;
    hintsys.active_hint = NULL;
}

i32 __cdecl Hint_isComplete(HINT_PC *hint) {
    return hint->completed;
}

void __cdecl Hint_SetCompletePc(HINT_PC *hint) {
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

i32 __cdecl Hint_CurrentId(void) {
    return hintsys.active_hint != NULL ? hintsys.active_hint->id : -1;
}

HINT_PC *__cdecl Hint_FindHint(i32 hint_id) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return NULL;
    while (hint->id != -1) {
        if (hint->id == hint_id)
            return hint;
        ++hint;
    }
    return NULL;
}

void __cdecl Hint_SaveGameState(HINT_PC *hint) {
    HINT_PC *candidate = hintsys.hints;
    i32 index = 0;
    if (candidate == NULL || hintsys.save_bits == NULL)
        return;
    while (candidate->id != -1) {
        if ((u16)candidate->id == (u16)hint->id) {
            if (index / 32 < hintsys.save_word_count)
                hintsys.save_bits[index / 32] |= 1u << (index & 31);
            return;
        }
        ++candidate;
        ++index;
    }
}

void __cdecl Hint_SetHint(HINT_PC *hint, i32 force, i32 allow_completed) {
    i32 state;
    if (hintsys.active_hint != NULL && hintsys.active_hint == hint)
        return;
    if (hint == NULL) {
        hintsys.active_hint = NULL;
        return;
    }
    if (force == 0) {
        if (hint->completed != 0 && allow_completed == 0)
            return;
        if (hint->cooldown > 0.0f)
            return;
    }
    hintsys.active_hint = hint;
    hintsys.display_elapsed = 0.0f;
    hintsys.alpha = 0.0f;
    if (force == 0 && (hint->flags & 1) != 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
    do {
        state = qrand() / 16384;
    } while (state == hintsys.state);
    hintsys.state = (u8)state;
    hint->cooldown = hint->repeat_delay;
    hintsys.alpha = 0.0f;
}

void __cdecl Hint_SetHintFromId(i32 hint_id, i32 force, i32 allow_completed) {
    HINT_PC *hint;
    if (hintsys.active_hint != NULL && hintsys.active_hint->id == hint_id)
        return;
    if (hint_id < 0 || TTab[hint_id] == NULL) {
        hintsys.active_hint = NULL;
        return;
    }
    hint = Hint_FindHint(hint_id);
    if (hint != NULL)
        Hint_SetHint(hint, force, allow_completed);
}

void __cdecl Hint_SetComplete(i32 hint_id) {
    HINT_PC *hint;
    if (hint_id == -1)
        return;
    hint = Hint_FindHint(hint_id);
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

typedef struct HINT_CANCEL_PC {
    u8 unknown00[8];
    f32 display_duration;
    u8 unknown0c[0xc];
    f32 cooldown;
} HINT_CANCEL_PC;
STATIC_ASSERT(offsetof(HINT_CANCEL_PC, display_duration) == 8);
STATIC_ASSERT(offsetof(HINT_CANCEL_PC, cooldown) == 0x18);
void __cdecl Hint_CancelCurrent(void) {
    HINT_CANCEL_PC *hint = (HINT_CANCEL_PC *)hintsys.active_hint;
    if (hint != NULL) {
        if (hint->display_duration != 0.0f && (f64)hint->display_duration - 0.5 > hintsys.display_elapsed)
            hintsys.display_elapsed = (f32)((f64)hint->display_duration - 0.5);
        hint->cooldown = 0.0f;
    }
}

void __cdecl Hint_Reset(void) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return;
    while (hint->id != -1) {
        hint->cooldown = 0.0f;
        ++hint;
    }
    hintsys.display_elapsed = 0.0f;
    hintsys.state = 2;
    hintsys.alpha = 0.0f;
    hintsys.active_hint = NULL;
}

void __cdecl Hint_ClearHintsAndDoneFlags(void) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return;
    while (hint->id != -1) {
        hint->flags &= 0xfd;
        hint->completed = 0;
        ++hint;
    }
}

void __cdecl Hint_LoadAllGameState(void) {
    HINT_PC *hint = hintsys.hints;
    i32 index = 0;
    if (hint == NULL || hintsys.save_bits == NULL)
        return;
    while (hint->id != -1) {
        i32 word = index / 32;
        if (word >= hintsys.save_word_count)
            break;
        hint->completed = (hintsys.save_bits[word] & (1U << (index % 32))) != 0;
        ++hint;
        ++index;
    }
}

/* PC576840/31; append to hint.c; update_fn alias below names native+0. */
void __cdecl Hint_Draw(i32 viewport) {
    if (hintsys.active_hint != NULL && hintsys.draw_fn != NULL)
        hintsys.draw_fn(hintsys.active_hint, viewport);
}

/* Append to src/legoapi/hint.c. */

f32 __cdecl CurrentHintAlpha(void) {
    f32 alpha;
    f32 fade_start;
    if (hintsys.active_hint == NULL)
        return 0.0f;
    alpha = 1.0f;
    if (hintsys.active_hint->display_duration > 0.0f) {
        fade_start = (f32)((f64)hintsys.active_hint->display_duration - 0.5);
        if (hintsys.display_elapsed >= fade_start) {
            alpha =
                1.0f - (hintsys.display_elapsed - fade_start) / (hintsys.active_hint->display_duration - fade_start);
            if (alpha <= 0.0f)
                alpha = 0.0f;
        }
    }
    return alpha * hintsys.alpha;
}

/* 00576630..00576668. Both arguments are ordinary cdecl stack arguments. */
void __cdecl Hint_ResetHint(i32 hint_id, i32 reset_completed) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return;
    while (hint->id != -1) {
        if (hint->id == hint_id) {
            if (reset_completed == 1)
                hint->completed = 0;
            hintsys.display_elapsed = 0.0f;
            return;
        }
        ++hint;
    }
}
typedef struct HINT_COMPLETION_PC {
    u16 points;
    u8 reserved02, flags;
} HINT_COMPLETION_PC;
STATIC_ASSERT(sizeof(HINT_COMPLETION_PC) == sizeof(CUSTOMISER_COMPLETION_READER_PC));
STATIC_ASSERT(offsetof(HINT_COMPLETION_PC, points) == 0);
STATIC_ASSERT(offsetof(HINT_COMPLETION_PC, flags) == 3);

typedef struct HINT_WORLD_TELEPORT_PC {
    u8 reserved00[0x4684];
    i32 teleport_count;
} HINT_WORLD_TELEPORT_PC;
STATIC_ASSERT(offsetof(HINT_WORLD_TELEPORT_PC, teleport_count) == 0x4684);

/* Native PC hint updater records retain their original packed field offsets. */
typedef struct HINT_PANEL_PC {
    u8 reserved00[0x61];
    u8 model_variant, flags;
    u8 reserved63[0x29];
    f32 target_scale;
} HINT_PANEL_PC;
STATIC_ASSERT(sizeof(HINT_PANEL_PC) == 0x90);
STATIC_ASSERT(offsetof(HINT_PANEL_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(HINT_PANEL_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(HINT_PANEL_PC, target_scale) == 0x8c);
struct panel_movement_pc_s;
struct panel_movement_pc_s *__cdecl GizPanel_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *, i32);

typedef struct HINT_ZIPUP_PC {
    u8 name[16];
    NUVEC lower_position;
    u8 reserved1c[0x4c];
    u8 flags;
    u8 reserved69[0x13];
} HINT_ZIPUP_PC;
STATIC_ASSERT(sizeof(HINT_ZIPUP_PC) == 0x7c);
STATIC_ASSERT(offsetof(HINT_ZIPUP_PC, lower_position) == 0x10);
STATIC_ASSERT(offsetof(HINT_ZIPUP_PC, flags) == 0x68);

typedef struct HINT_TURRET_PC {
    u8 reserved00[0x24];
    NUVEC position;
    u8 reserved30[0xb4];
    GAMEOBJECT *opponent;
    u8 reservede8[0x20];
    u32 behavior_flags;
    u8 reserved10c[0x2e];
    u16 flags;
} HINT_TURRET_PC;
STATIC_ASSERT(sizeof(HINT_TURRET_PC) == 0x13c);
STATIC_ASSERT(offsetof(HINT_TURRET_PC, position) == 0x24);
STATIC_ASSERT(offsetof(HINT_TURRET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(HINT_TURRET_PC, behavior_flags) == 0x108);
STATIC_ASSERT(offsetof(HINT_TURRET_PC, flags) == 0x13a);
STATIC_ASSERT(offsetof(GAMEOBJECT, unknown_0e28) + 4 == 0xe2c);
STATIC_ASSERT(offsetof(GAMESAVE, losttemple_trailer_watched) == 0x7c1d);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, area_complete) == 1);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, builders_active) == 0x6e);
STATIC_ASSERT(offsetof(struct GIZBUILDITSYS_s, count) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_turret_sys) == 0x46b8);
STATIC_ASSERT(offsetof(struct GIZTURRETSYS_s, count) == 8);

NUVEC PercentHint_TriggerPos = {-28.421f, 0.0f, -52.462f};

i32 __cdecl ActivePlayerInRange(NUVEC *position, f32 range_squared, f32 *distance_squared) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0) {
            volatile f32 distance = NuVecDistSqr(&object->collision_position, position, NULL);
            if (distance < range_squared) {
                if (distance_squared != NULL)
                    *distance_squared = distance;
                return 1;
            }
        }
    }
    return 0;
}

GIZBUILDIT_PC *__cdecl GizBuildIt_AnyReacting(WORLDINFO *world) {
    struct GIZBUILDITSYS_s *system = world->giz_buildit_sys;
    if (system != NULL) {
        GIZBUILDIT_PC *buildit = (GIZBUILDIT_PC *)system->buildits;
        i32 count = system->count;
        i32 index;
        for (index = 0; index < count; ++index, ++buildit) {
            u16 flags = buildit->availability_flags;
            if ((flags & 2) != 0 && (flags & 4) != 0 && buildit->builders_active == 0 && (flags & 1) != 0)
                return buildit;
        }
    }
    return NULL;
}

i32 __cdecl GizForce_UpdateHint(HINT_PC *hint) {
    i32 index;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    for (index = 0; index < 2; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->transform_alpha > 0.0f &&
            object->transform_target != NULL && object->unknown_0e28[4] == 0)
            return 1;
    }
    return 0;
}

i32 __cdecl GizPanel_UpdateHint(HINT_PC *hint) {
    u8 storm = 0, bountyhunter = 0, droid = 0;
    i32 index;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL) {
            f32 distance;
            HINT_PANEL_PC *panel =
                (HINT_PANEL_PC *)GizPanel_FindNearest(WORLD, &object->position, object, &distance, 0);
            if (panel != NULL && (panel->flags & 8) != 0 && (panel->flags & 3) == 0) {
                volatile f32 radius =
                    (f32)(((f64)((GAMEOBJECT *)Player[index])->bounds_radius + 1.0) * panel->target_scale);
                volatile f32 radius_squared = radius * radius;
                if (radius_squared > distance) {
                    switch (panel->model_variant) {
                        case 0:
                        case 1:
                            droid = 1;
                            break;
                        case 2:
                            bountyhunter = 1;
                            break;
                        case 3:
                            storm = 1;
                            break;
                    }
                }
            }
        }
    }
    if (storm != 0 && ((u16)hint->id == 0x260 || (u16)hint->id == 0x269)) {
        if (AvailableToPlayer(0, 1, 5, 0) != 0)
            return (u16)hint->id == 0x260;
        return (u16)hint->id == 0x269;
    }
    if (bountyhunter != 0 && ((u16)hint->id == 0x261 || (u16)hint->id == 0x26a)) {
        if (AvailableToPlayer(0x1000000, -1, 6, 0) != 0)
            return (u16)hint->id == 0x261;
        return (u16)hint->id == 0x26a;
    }
    if (droid != 0 && (u16)hint->id == 0x25f)
        return (LswForceUsedHintFlagsPc & 2) == 2;
    return 0;
}

i32 __cdecl Percent_UpdateHint(HINT_PC *hint) {
    i32 active = 0, nearby = 0, index;
    if (hint->completed != 0 || WORLD->current_level != HUB_LDATA || FadeSys.fade != 0.0f ||
        CustomiserCompletionSavePc == NULL ||
        ((HINT_COMPLETION_PC *)CustomiserCompletionSavePc)->points < HubCompletionPointsPc)
        return 0;
    for (index = 0; index < 2; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0) {
            ++active;
            if (NuVecXZDistSqr(&object->position, &PercentHint_TriggerPos, NULL) < 4.0f)
                ++nearby;
        }
    }
    if (active > 0 && nearby == active) {
        hint->colour_index = 4;
        return 1;
    }
    return 0;
}

i32 __cdecl IndyUnlocked_UpdateHint(HINT_PC *hint) {
    if (hint->completed == 0 && WORLD->current_level == HUB_LDATA && FadeSys.fade == 0.0f &&
        Game.losttemple_trailer_watched != 0) {
        hint->colour_index = 4;
        return 1;
    }
    return 0;
}

i32 __cdecl Dodge_UpdateHint(HINT_PC *hint) {
    i32 index;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL) {
            if ((object->character_data->model_flags & 0x2000) != 0)
                return 0;
            if ((object->flags_1fc & 0x80) != 0 && (object->portal_flags & 1) != 0 &&
                (object->character_model->model_data_b[0x4f] != NULL ||
                 object->character_model->model_data_b[0x26] != NULL))
                return 1;
        }
    }
    return 0;
}

i32 __cdecl Shop_UpdateHint(HINT_PC *hint) {
    (void)hint;
    return 0;
}

i32 __cdecl Teleport_UpdateHints(HINT_PC *hint) {
    GAMEOBJECT *object;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    if (((HINT_WORLD_TELEPORT_PC *)WORLD)->teleport_count <= 0 || AvailableToPlayer(0x40000, -1, -1, 0) == 0)
        return 0;
    object = (GAMEOBJECT *)Player[0];
    if (object != NULL && ((object->flags_e24 & 0x20) != 0 || Teleport_Find(object, 1.5625f) != NULL))
        return 1;
    object = (GAMEOBJECT *)Player[1];
    return object != NULL && ((object->flags_e24 & 0x20) != 0 || Teleport_Find(object, 1.5625f) != NULL);
}

i32 __cdecl ShinyMetal_UpdateHint(HINT_PC *hint) {
    GAMEOBJECT *object;
    (void)hint;
    if (JABBASPALACE_ADATA == NULL || WORLD->current_level != JABBASPALACEB_LDATA ||
        Game.area_save[JABBASPALACE_ADATA->index].area_complete != 0 || Mission_Active(NULL) != NULL)
        return 0;
    if (LevGizObst[0] != NULL && LevGizObst[0]->anim_set->state == 0)
        return 0;
    if (LevBlowUp[0] == NULL || (LevBlowUp[0]->status_flags & 1) != 0)
        return 0;
    object = player;
    if (object != NULL && (f64)NuVecDistSqr(&object->collision_position, &LevBlowUp[0]->mid_position, NULL) < 25.0)
        return 1;
    object = player2;
    return object != NULL && (f64)NuVecDistSqr(&object->collision_position, &LevBlowUp[0]->mid_position, NULL) < 25.0;
}

i32 __cdecl ZipUps_UpdateHint(HINT_PC *hint) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    HINT_ZIPUP_PC *zipups;
    i32 index;
    if (world->area != NULL && world->area == HUB_ADATA)
        return 0;
    zipups = (HINT_ZIPUP_PC *)world->zipups;
    if (zipups == NULL)
        return 0;
    for (index = 0; index < world->zipup_count; ++index) {
        if (hint->completed == 0 && CanDrawZipUpSwirls != 0 && (zipups[index].flags & 0x40) != 0 &&
            ActivePlayerInRange(&zipups[index].lower_position, 2.0f, NULL) != 0)
            return 1;
    }
    return 0;
}

i32 __cdecl GizBuildIts_UpdateHint(HINT_PC *hint) {
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    if (hint->completed != 0)
        return 0;
    return GizBuildIt_AnyReacting(WorldInfo_CurrentlyActive()) != NULL;
}

i32 __cdecl Push_UpdateHints(HINT_PC *hint) {
    i32 index;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    if (LEGOACT_PUSH == -1)
        return 0;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 &&
            object->character_model->model_data_b[LEGOACT_PUSH] != NULL &&
            NearestPushBlock(WORLD, &object->collision_position, 2.0f) != NULL)
            return 1;
    }
    return 0;
}

i32 __cdecl GizTurrets_UpdateHint(HINT_PC *hint) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    struct GIZTURRETSYS_s *system = world->giz_turret_sys;
    HINT_TURRET_PC *turret;
    i32 index;
    (void)hint;
    if ((world->area != NULL && world->area == HUB_ADATA) || VehicleArea != 0 || system == NULL)
        return 0;
    turret = (HINT_TURRET_PC *)system->turrets;
    for (index = 0; index < system->count; ++index, ++turret) {
        if ((turret->flags & 2) != 0 && (turret->flags & 4) != 0 && (turret->behavior_flags & 0x4000) != 0 &&
            (turret->behavior_flags & 0x10) == 0 && turret->opponent != NULL &&
            NuVecDistSqr(&GameCam->position, &turret->position, NULL) < 36.0f)
            return 1;
    }
    return 0;
}
