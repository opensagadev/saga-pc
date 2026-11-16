#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/legoapi_signal.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

i32 Signals_Load(WORLDINFO *world, void *unused) {
    i32 version;
    i32 index;
    if (world->signal_count == 0) {
        version = EdFileReadInt();
        world->signal_count = EdFileReadInt();
        for (index = 0; index < world->signal_count; ++index) {
            EdFileRead(world->signals[index].name, 16);
            EdFileReadNuVec(&world->signals[index].position);
            world->signals[index].character_letter = EdFileReadChar();
            if (version >= 2) {
                world->signals[index].suit_letter = EdFileReadChar();
                world->signals[index].yaw = EdFileReadShort();
            } else {
                world->signals[index].suit_letter = 'b';
                world->signals[index].yaw = 0;
            }
        }
        return 1;
    }
    return 0;
}

void Signals_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->signal_count; ++index) {
        world->signals[index].platform_id = NewPlatPickupInst(&world->signals[index].matrix, 2);
    }
}

void *Signals_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA *level = world->current_level;
    world->signals = NULL;
    world->signal_count = 0;
    if (level->max_signals > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->signals = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_signals * sizeof(SIGNAL_s);
    }
    return world->signals;
}

void Signal_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        SIGNAL_s *signal = gizmo->object;
        signal->active = active != 0;
    }
}

i32 Signal_ActivateRev(GIZMO *gizmo, i32 active, i32 reverse) {
    if (gizmo != NULL) {
        SIGNAL_s *signal = gizmo->object;
        if (signal != NULL) {
            if ((reverse & 1) != 0) {
                return active == signal->active;
            }
            signal->active = active == 0;
            return 1;
        }
    }
    return 0;
}

static void SignalSetVisible(SIGNAL_s *signal, i32 visible) {
    u8 was_visible = signal->visible;
    signal->visible = visible != 0;
    if (signal->visible && !was_visible) {
        signal->platform_id = NewPlatPickupInst(&signal->matrix, 2);
    } else if (!signal->visible && was_visible) {
        DeletePlatinst(signal->platform_id);
    }
}

void Signal_SetGizmoVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SignalSetVisible(gizmo->object, visible);
    }
}

NUVEC *Signal_GetPos(GIZMO *gizmo) {
    SIGNAL_s *signal = gizmo != NULL ? (SIGNAL_s *)gizmo->object : NULL;
    return signal != NULL ? &signal->position : NULL;
}

static void SignalResetRecord(SIGNAL_s *signal) {
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
    if (signal->suit == NULL) {
        signal->suit = &SuitRecordInit[0];
    }
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
    signal->flags = (signal->flags & ~SIGNAL_IN_USE) | SIGNAL_VISIBLE | SIGNAL_ACTIVE;
    NuMtxSetRotationY(&signal->matrix, signal->yaw);
    NuMtxRotateZ(&signal->matrix, signal->terrain_roll);
    NuMtxRotateX(&signal->matrix, signal->terrain_pitch);
    NuMtxTranslate(&signal->matrix, &signal->position);
}

void Signals_Reset(WORLDINFO *world, void *unused, SIGNALPROGRESS_s *progress) {
    SIGNAL_s *signal;
    i32 index;
    if (world == NULL || world->signals == NULL) {
        return;
    }
    signal = world->signals;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        i32 word;
        u32 bit;
        SignalResetRecord(signal);
        if (progress == NULL || index >= 32) {
            continue;
        }
        word = index / 32;
        bit = 1 << (index & 31);
        if (progress->suit_letters[index] != 0) {
            signal->suit = Suit_FindFromLetter(progress->suit_letters[index]);
        }
        SignalSetVisible(signal, (progress->visible_mask[word] & bit) != 0);
        signal->active = (progress->active_mask[word] & bit) != 0;
    }
}

void Signals_Update(WORLDINFO *world, void *unused, f32 unused_time) {
    SIGNAL_s *signal = world->signals;
    i32 index;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        f32 target_scale;
        signal->radius = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.1f + (f64)0.8f);
        if (signal->visible && signal->active) {
            if (signal->in_use) {
                i32 player_index;
                for (player_index = 0; player_index < 8; ++player_index) {
                    GAMEOBJECT *player = Player[player_index];
                    if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                        (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && player->id == (u16)signal->suit->character_id &&
                        (f64)NuVecDistSqr(&player->collision_position, &signal->target_position, NULL) < 0.49000006914138794) {
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
                        if ((f64)lateral_squared < (f64)radius * radius) {
                            break;
                        }
                    }
                }
                if (player_index == 8) {
                    signal->in_use = 0;
                }
                target_scale = 0.0f;
            } else {
                ADD_GAME_MESSAGE message = AddGameMsg_Default;
                message.text = TTab[*signal->suit->text_id];
                message.position = &signal->target_position;
                message.scale = 1.0f;
                message.red = 0xff;
                message.green = 0xff;
                message.blue = 0;
                message.alpha = 0x30;
                message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_ONE_FRAME;
                AddGameMsg(&message);
                target_scale = 1.0f;
            }
            signal->scale = SeekLinearF(signal->scale, target_scale, (f32)(3.0 * FRAMETIME));
        } else {
            signal->scale = 0.0f;
            signal->in_use = 0;
        }
    }
}

SIGNAL_s *Signal_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    SIGNAL_s *nearest = NULL;
    f32 nearest_distance;
    SIGNAL_s *signal;
    i32 index;
    signal = world->signals;
    nearest_distance = 1000000000.0f;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        f32 signal_distance;
        if (object != NULL) {
            if (!signal->visible || !signal->active || signal->in_use || (u16)signal->suit->character_id != object->id) {
                continue;
            }
            signal_distance = NuVecDistSqr(position, &signal->target_position, NULL);
        } else {
            signal_distance = NuVecDistSqr(position, &signal->position, NULL);
        }
        if (signal_distance < nearest_distance) {
            nearest_distance = signal_distance;
            nearest = signal;
        }
    }
    if (distance != NULL) {
        *distance = nearest_distance;
    }
    return nearest;
}

void Signals_Draw(WORLDINFO *world, void *unused, f32 unused_time) {
    SIGNAL_s *signal = world->signals;
    i32 index;
    for (index = 0; index < world->signal_count; ++index, ++signal) {
        NUMTX matrix;
        if ((signal->flags & SIGNAL_VISIBLE) == 0) {
            continue;
        }
        matrix = signal->matrix;
        if (world->level_objects[0x36].active != 0) {
            NuSpecialDrawAt(&world->level_objects[0x36].special, &matrix);
        }
        if ((signal->flags & SIGNAL_ACTIVE) != 0) {
            if (world->level_objects[signal->animation_1].active != 0) {
                NuSpecialDrawAt(&world->level_objects[signal->animation_1].special, &matrix);
            }
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
                    if (lateral_squared <= 0.5625f) {
                        fade = 0.0f;
                    } else {
                        fade = (f32)(((f64)NuFsqrt(lateral_squared) - 0.75) / 0.75);
                    }
                    if (along_normal < 0.0f || along_normal > 2.0f) {
                        f32 overshoot;
                        if (along_normal >= 0.0f) {
                            overshoot = (f32)((f64)along_normal - 2.0);
                        } else {
                            overshoot = -along_normal;
                        }
                        if (overshoot < 0.75f) {
                            fade = (f32)((f64)fade + (1.0 - (f64)fade) * ((f64)overshoot / 0.75));
                        } else {
                            fade = 1.0f;
                        }
                    }
                    alpha *= fade;
                }
                alpha *= signal->scale;
                if (alpha > 0.0f) {
                    NuSpecialDrawAtAlpha(&world->level_objects[signal->animation_2].special, &matrix, alpha);
                }
            }
        } else if (world->level_objects[signal->animation_0].active != 0) {
            NuSpecialDrawAt(&world->level_objects[signal->animation_0].special, &matrix);
        }
    }
}

i32 Signals_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_signals : 0;
}

void Signals_AddGizmos(void *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->signal_count; ++index) {
        if (NuStrLen(world->signals[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &world->signals[index]);
        }
    }
}

char *Signal_GetGizmoName(GIZMO *gizmo) {
    SIGNAL_s *signal = gizmo != NULL ? (SIGNAL_s *)gizmo->object : NULL;
    return signal != NULL ? signal->name : NULL;
}

i32 Signal_GetOutput(GIZMO *gizmo, i32 unused_index, i32 unused_mode) {
    SIGNAL_s *signal = gizmo->object;
    if ((signal->flags & SIGNAL_VISIBLE) != 0 && (signal->flags & SIGNAL_ACTIVE) != 0) {
        return 1;
    }
    return 0;
}

char *Signal_GetOutputName(GIZMO *unused, i32 unused_index) {
    return "Active";
}

i32 Signal_GetNumOutputs(GIZMO *unused) {
    return 1;
}

void *Signals_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(SIGNALPROGRESS_s));
}

void Signals_ClearProgress(void *unused, SIGNALPROGRESS_s *progress) {
    if (progress != NULL) {
        memset(progress->suit_letters, 0, sizeof(progress->suit_letters));
        progress->active_mask[0] = 0xffffffffu;
        progress->visible_mask[0] = 0xffffffffu;
    }
}

void Signals_StoreProgress(WORLDINFO *world, void *unused, SIGNALPROGRESS_s *progress) {
    SIGNAL_s *signal;
    i32 index;
    Signals_ClearProgress(world, progress);
    if (progress == NULL || world == NULL || world->signals == NULL) {
        return;
    }
    signal = world->signals;
    for (index = 0; index < world->signal_count && index < 32; ++index, ++signal) {
        i32 word = index / 32;
        u32 bit = 1 << (index & 31);
        progress->suit_letters[index] = world->signals[index].suit->letter;
        if (!signal->visible) {
            progress->visible_mask[word] &= ~bit;
        }
        if (!signal->active) {
            progress->active_mask[word] &= ~bit;
        }
    }
}

char *SignalRegistryName = "Signal";
ADDGIZMOTYPE SignalAddType = {0};

ADDGIZMOTYPE *Signals_RegisterGizmo(i32 unused_type_id) {
    char *name = SignalRegistryName;
    SignalAddType = Default_ADDGIZMOTYPE;
    SignalAddType.name = name;
    SignalAddType.prefix = dat_007516c0;
    SignalAddType.fns.options = sizeof(SIGNALPROGRESS_s);
    SignalAddType.fns.get_max_gizmos_fn = (i32(*)(void *))Signals_GetMaxGizmos;
    SignalAddType.fns.add_gizmos_fn = (void *)Signals_AddGizmos;
    SignalAddType.fns.early_update_fn = NULL;
    SignalAddType.fns.late_update_fn = (void (*)(void *, void *, f32))Signals_Update;
    SignalAddType.fns.draw_fn = (void (*)(void *, void *, f32))Signals_Draw;
    SignalAddType.fns.panel_draw_fn = NULL;
    SignalAddType.fns.get_gizmo_name_fn = Signal_GetGizmoName;
    SignalAddType.fns.get_output_fn = Signal_GetOutput;
    SignalAddType.fns.get_output_name_fn = Signal_GetOutputName;
    SignalAddType.fns.get_num_outputs_fn = (void *)Signal_GetNumOutputs;
    SignalAddType.fns.activate_fn = Signal_Activate;
    SignalAddType.fns.activate_rev_fn = Signal_ActivateRev;
    SignalAddType.fns.set_visibility_fn = Signal_SetGizmoVisibility;
    SignalAddType.fns.get_visibility_fn = NULL;
    SignalAddType.fns.get_pos_fn = (void *)Signal_GetPos;
    SignalAddType.fns.using_special_fn = NULL;
    SignalAddType.fns.bolt_hit_plat_fn = NULL;
    SignalAddType.fns.get_best_bolt_target_fn = NULL;
    SignalAddType.fns.bolt_hit_fn = NULL;
    SignalAddType.fns.allocate_progress_data_fn = Signals_AllocateProgressData;
    SignalAddType.fns.clear_progress_fn = (void (*)(void *, void *))Signals_ClearProgress;
    SignalAddType.fns.store_progress_fn = (void (*)(void *, void *, void *))Signals_StoreProgress;
    SignalAddType.fns.reset_fn = (void (*)(void *, void *, void *))Signals_Reset;
    SignalAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))Signals_ReserveBufferSpace;
    SignalAddType.fns.load_fn = (i32(*)(void *, void *))Signals_Load;
    SignalAddType.fns.post_load_fn = NULL;
    SignalAddType.fns.add_level_sfx_fn = NULL;
    return &SignalAddType;
}

void Signal_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    if (object->character_context == CHARACTER_CONTEXT_SIGNAL) {
        f32 *animation_time = NULL;
        i32 exchange;
        f32 elapsed;
        if (object->character_model->action_animations[object->unknown_7aa] != NULL) {
            animation_time = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
            if (animation_time == NULL) {
                return;
            }
        }
        exchange = 0;
        elapsed = (f32)((f64)object->context_timer + (f64)FRAMETIME);
        object->context_timer = elapsed;
        if (elapsed >= object->context_duration) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                exchange = 1;
            }
            StartEndOfJump(object);
        } else {
            if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                return;
            }
            if (animation_time != NULL) {
                f32 frame = AnimListFrame(object->character_model, object->unknown_7aa, 0);
                if (frame >= 1.0f && *animation_time >= frame) {
                    exchange = 1;
                }
            }
        }
        if (exchange != 0) {
            SIGNAL_s *signal;
            SUIT_INITIALIZER_RECORD *old_suit;
            SUIT_INITIALIZER_RECORD *new_suit;
            i32 index;
            /* Context 0x4c uses the attachment union and the full 24-byte suit record. */
            ((SIGNAL_s *)object->grapple)->flags |= SIGNAL_IN_USE;
            object->context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
            old_suit = (SUIT_INITIALIZER_RECORD *)object->suit;
            object->ai_packet.capabilities &= ~old_suit->character_flags;
            signal = object->grapple;
            new_suit = signal->suit;
            object->suit = (SUIT_LAYERS *)new_suit;
            signal->suit = old_suit;
            new_suit = (SUIT_INITIALIZER_RECORD *)object->suit;
            object->ai_packet.capabilities |= new_suit->character_flags;
            for (index = 0; index < 10; ++index) {
                if (new_suit == &SuitRecordInit[index]) {
                    areaSuitBits |= 1u << index;
                    break;
                }
            }
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            NewRumble(object->pad->bound_pad, 0.6f, 0);
        }
        if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
            NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f), 0);
        }
        return;
    }
    if (object->unknown_281 != 0 || object->character_context == CHARACTER_CONTEXT_BIG_JUMP ||
        object->character_context == CHARACTER_CONTEXT_DOOMED || object->character_context == CHARACTER_CONTEXT_FLOAT ||
        (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || (U64_PAIR(&CInfo[object->character_context].flags) & 0x800) != 0) {
        return;
    }
    {
        f32 distance;
        SIGNAL_s *signal = Signal_FindNearest(world, &object->collision_position, object, &distance);
        if (signal != NULL && distance < 0.0625f) {
            object->context_timer = 0.0f;
            object->character_context = CHARACTER_CONTEXT_SIGNAL;
            object->grapple = signal;
            object->unknown_7aa = ACTION_CHANGE;
            object->context_duration = AnimDuration((i16)object->id, 0x96, 0.0f, 0.0f, 1);
            if (object->context_duration <= 0.0f) {
                object->context_duration = 1.0f;
            }
            object->context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
        }
    }
}

void SuitBlowupCode(WORLDINFO *world, GAMEOBJECT *object) {
    GIZMOBLOWUP *candidates[256];
    GIZMOBLOWUP *blowup;
    NUVEC minimum;
    NUVEC maximum;
    i32 count;
    i32 index;
    f32 remaining;
    if (object->character_context == CHARACTER_CONTEXT_SONAR) {
        if (object->character_model->model_data_b[object->unknown_7aa] != NULL &&
            AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0) == NULL) {
            return;
        }
        object->context_timer += FRAMETIME;
        if ((f64)object->context_duration < (f64)0.2f && (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            object->context_duration = 0.2f;
        }
        remaining = object->context_duration - FRAMETIME;
        object->context_duration = remaining;
        if (remaining <= 0.0f) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        if (!(object->context_timer >= 0.5f)) {
            return;
        }
        blowup = world->gizmo_blowups;
        if (blowup == NULL) {
            return;
        }
        count = 0;
        minimum.x = object->collision_position.x - 3.0;
        minimum.y = object->collision_position.y - 3.0;
        minimum.z = object->collision_position.z - 3.0;
        maximum.x = object->collision_position.x + 3.0;
        maximum.y = object->collision_position.y + 3.0;
        maximum.z = object->collision_position.z + 3.0;
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
            const u32 status = blowup->status_flags;
            u32 draw;
            if ((status & BLOWUP_STATUS_BLOWN_UP) != 0 || (status & BLOWUP_STATUS_ACTIVE) == 0 || (status & BLOWUP_STATUS_VISIBLE) == 0) {
                continue;
            }
            draw = blowup->draw_flags;
            if ((draw & BLOWUP_OPTION_CARRIED_ONLY) == 0 || ((draw & BLOWUP_OPTION_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0)) {
                continue;
            }
            if (blowup->mid_position.x < minimum.x || blowup->mid_position.x > maximum.x || blowup->mid_position.z < minimum.z ||
                blowup->mid_position.z > maximum.z || blowup->mid_position.y < minimum.y || blowup->mid_position.y > maximum.y) {
                continue;
            }
            if (!(NuVecDistSqr(&blowup->mid_position, &object->collision_position, NULL) > 9.0f) && count < 256) {
                candidates[count++] = blowup;
            }
        }
        if (count > 0 && ParticlesPerSecond(5.0f, FRAMETIME) > 0) {
            GizmoBlowupBlowup(candidates[qrand() / (65535 / count + 1)], 1, BLOWUP_HIT_SIGNAL, 1, NULL, 1);
        }
    } else {
        if (object->unknown_281 != 0 && ObjLandReady(object) != 0 && (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            object->character_context = CHARACTER_CONTEXT_SONAR;
            object->context_timer = 0.0f;
            object->unknown_7aa = ACTION_SONAR;
            object->context_duration = 0.3f;
        }
    }
}
