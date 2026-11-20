#include "gameapi/edptl.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/supportall.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" void TeleportNuError(void) {
}

typedef struct TELEPORT_KEYWORDS { // size 0x64
    char *name_prefix;             // 0x00
    NUFPCOMJMP keywords[12];       // 0x04
} TELEPORT_KEYWORDS;

extern "C" {
    i32 Teleport_GetMaxGizmos(WORLDINFO *);
    void Teleport_AddGizmos(GIZMOSYS *, i32, WORLDINFO *, void *);
    char *Teleport_GetGizmoName(GIZMO *);
    i32 Teleport_GetOutput(GIZMO *, i32, i32);
    char *Teleport_GetOutputName(GIZMO *, i32);
    i32 Teleport_GetNumOutputs(GIZMO *);
    void Teleport_Activate(GIZMO *, i32);
    i32 Teleport_ActivateRev(GIZMO *, i32, i32);
}

extern "C" {
    static void Tel_1_way(NUFPAR *parser);
    static void Tel_Hacky_Cam(NUFPAR *parser);
    static void Tel_crawl(NUFPAR *parser);
    static void Tel_duration(NUFPAR *parser);
    static void Tel_flap(NUFPAR *parser);
    static void Tel_flip_flap(NUFPAR *parser);
    static void Tel_name(NUFPAR *parser);
    static void Tel_obj(NUFPAR *parser);
    static void Tel_range(NUFPAR *parser);
    static void Tel_spline(NUFPAR *parser);
}
extern "C" {
    static TELEPORT_CONFIG *Tel_teleport = 0;
    static WORLDINFO *Tel_worldinfo = 0;
    static TELEPORT_KEYWORDS TeleportConfigData = {"TLT_",
                                                   {{"spline", Tel_spline},
                                                    {"duration", Tel_duration},
                                                    {"range", Tel_range},
                                                    {"1_way", Tel_1_way},
                                                    {"crawl", Tel_crawl},
                                                    {"flip_flap", Tel_flip_flap},
                                                    {"hackycam", Tel_Hacky_Cam},
                                                    {"obj", Tel_obj},
                                                    {"flap1", Tel_flap},
                                                    {"flap2", Tel_flap},
                                                    {"name", Tel_name},
                                                    {NULL, NULL}}};

    static void Tel_spline(NUFPAR *parser) {
        i32 index;
        TELEPORT_CONFIG *teleport;
        if (NuFParGetWord(parser) == 0) {
            return;
        }
        Tel_teleport->path = edSpline_SplineFind(Tel_worldinfo->current_gscn, parser->word_buffer_ptr);
        teleport = Tel_teleport;
        if (teleport->path == NULL || teleport->path->length != 4) {
            teleport->path = NULL;
        } else {
            for (index = 0; index < Tel_worldinfo->teleport_count; ++index) {
                if (Tel_worldinfo->teleports[index].path == teleport->path) {
                    break;
                }
            }
            if (index < Tel_worldinfo->teleport_count) {
                teleport->path = NULL;
            }
        }
        if (teleport->path != NULL) {
            NuStrCpy(teleport->name, TeleportConfigData.name_prefix);
            NuStrCat(Tel_teleport->name, Tel_teleport->path->name);
            GizmoGetUniqueName(Tel_worldinfo->gizmo_sys, TeleportConfigData.name_prefix, Tel_teleport->name, Tel_teleport->name, 0x40);
        }
    }

    static void Tel_duration(NUFPAR *parser) {
        Tel_teleport->duration = NuFParGetFloat(parser);
    }

    static void Tel_range(NUFPAR *parser) {
        f32 range = NuFParGetFloat(parser);
        Tel_teleport->flags |= TELEPORT_RANGE;
        Tel_teleport->range_squared = range * range;
    }

    static void Tel_1_way(NUFPAR *parser) {
        Tel_teleport->flags |= TELEPORT_ONE_WAY;
    }

    static void Tel_crawl(NUFPAR *parser) {
        Tel_teleport->flags |= TELEPORT_CRAWL;
    }

    static void Tel_flip_flap(NUFPAR *parser) {
        Tel_teleport->flags |= TELEPORT_FLIP_FLAP;
    }

    static void Tel_Hacky_Cam(NUFPAR *parser) {
        Tel_teleport->flags |= TELEPORT_HACKY_CAM;
    }

    static void Tel_obj(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            NuSpecialFind(Tel_worldinfo->current_gscn, &Tel_teleport->blocking_special, parser->word_buffer_ptr, 1);
        }
    }

    static void Tel_flap(NUFPAR *parser) {
        if (NuFParGetWord(parser) == 0) {
            return;
        }
        if (NuStrIStr(parser->line_buffer_ptr, "flap1") != NULL) {
            if (NuSpecialFind(Tel_worldinfo->current_gscn, &Tel_teleport->flap1_special, parser->word_buffer_ptr, 1) != 0) {
                Tel_teleport->flap1_matrix = *NuSpecialGetDrawMtx(&Tel_teleport->flap1_special);
            }
        } else {
            if (NuSpecialFind(Tel_worldinfo->current_gscn, &Tel_teleport->flap2_special, parser->word_buffer_ptr, 1) != 0) {
                Tel_teleport->flap2_matrix = *NuSpecialGetDrawMtx(&Tel_teleport->flap2_special);
            }
        }
    }

    static void Tel_name(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            GizmoGetUniqueName(WORLD->gizmo_sys, TeleportConfigData.name_prefix, parser->word_buffer_ptr, Tel_teleport->name, 0x40);
        }
    }

    void Teleports_Configure(WORLDINFO *world, char *config) {
        TELEPORT_CONFIG *teleport;
        NUFPAR *parser;
        i32 active;
        world->teleports = NULL;
        if (world->current_gscn == NULL) {
            return;
        }
        parser = NuFParCreateMem("teleports", config, 0xffff);
        if (parser == NULL) {
            return;
        }
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        teleport = static_cast<TELEPORT_CONFIG *>(world->giz_buffer.void_ptr);
        world->teleports = teleport;
        NuFParPushCom(parser, TeleportConfigData.keywords);
        active = 0;
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            if (active != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "teleport_end") == 0) {
                    active = 0;
                    if (NuStrLen(Tel_teleport->name) == 0) {
                        NuStrCpy(Tel_teleport->name, TeleportConfigData.name_prefix);
                        NuStrCat(Tel_teleport->name, "TeleportNoSpline!");
                        GizmoGetUniqueName(WORLD->gizmo_sys, TeleportConfigData.name_prefix, Tel_teleport->name, Tel_teleport->name, 0x40);
                    }
                    if (teleport->path != NULL) {
                        ++teleport;
                        ++world->teleport_count;
                    }
                } else {
                    active = 1;
                    NuFParInterpretWord(parser);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "teleport_start") == 0) {
                Tel_worldinfo = world;
                Tel_teleport = teleport;
                NuStrCpy(teleport->name, "");
                teleport->duration = 5.0f;
                teleport->enabled = 1;
                teleport->path = NULL;
                teleport->range_squared = 0;
                teleport->flags = 0;
                teleport->active = 0;
                teleport->blocking_special.special = NULL;
                teleport->blocking_special.scene = NULL;
                teleport->blocking_special.display_special = NULL;
                teleport->flap1_special.special = NULL;
                teleport->flap1_special.scene = NULL;
                teleport->flap1_special.display_special = NULL;
                teleport->flap2_special.special = NULL;
                teleport->flap2_special.scene = NULL;
                teleport->flap2_special.display_special = NULL;
                active = 1;
            }
        }
        NuFParDestroy(parser);
        if (world->teleport_count > 0) {
            world->giz_buffer.addr = ((u32)teleport + 15) & ~15u;
        } else {
            world->teleports = NULL;
        }
    }
}
static i32 FindTeleport_Direction = 0;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
extern "C" {
    static f32 TeleportFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }
}

extern "C" TELEPORT_CONFIG *Teleport_Find(GAMEOBJECT *object, f32 range_squared) {
    TELEPORT_CONFIG *teleport = WORLD->teleports;
    if (teleport == NULL) {
        return NULL;
    }
    f32 radius_squared = 2.0f * object->collision_radius;
    radius_squared *= radius_squared;
    i32 nearest = -1;
    f32 nearest_distance = 100000000.0f;
    for (i32 i = 0; i < WORLD->teleport_count; ++i, ++teleport) {
        if (teleport->active != 0 || teleport->enabled == 0) {
            continue;
        }
        if (NuSpecialExistsFn(&teleport->blocking_special)) {
            NUINSTANIM *animation = (NUINSTANIM *)NuSpecialGetInstAnim(&teleport->blocking_special);
            if (animation != NULL) {
                if ((animation->flags & NUINSTANIM_FLAG_PLAYING) || animation->ltime == 1.0f) {
                    continue;
                }
            } else if (NuSpecialGetVisibilityFn(&teleport->blocking_special)) {
                continue;
            }
        }
        i32 range_flag = teleport->flags & TELEPORT_RANGE;
        f32 distance = NuVecDistSqr(&object->collision_position, teleport->path->pts, NULL);
        if (((range_squared != 0.0f && distance < range_squared) ||
             (range_flag != 0 ? distance < teleport->range_squared : distance < radius_squared)) &&
            (nearest == -1 || distance < nearest_distance)) {
            nearest = i;
            FindTeleport_Direction = 0;
            nearest_distance = distance;
        }
        if ((teleport->flags & TELEPORT_ONE_WAY) != 0) {
            continue;
        }
        distance = NuVecDistSqr(&object->collision_position, &teleport->path->pts[teleport->path->length - 1], NULL);
        if (((range_squared != 0.0f && distance < range_squared) ||
             (range_flag != 0 ? distance < teleport->range_squared : distance < radius_squared)) &&
            (nearest == -1 || distance < nearest_distance)) {
            nearest = i;
            FindTeleport_Direction = 1;
            nearest_distance = distance;
        }
    }
    if (nearest != -1) {
        return &WORLD->teleports[nearest];
    }
    return NULL;
}

static void TeleportGetSegment(i32 *start, i32 *end, GAMEOBJECT *object) {
    TELEPORT_CONFIG *teleport = object->player.teleport;
    if ((teleport->flags & TELEPORT_CRAWL) != 0) {
        if ((object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0) {
            *start = 0;
            *end = 3;
        } else {
            *start = 3;
            *end = 0;
        }
    } else if ((object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0) {
        if (object->player.big_jump_animation > 0) {
            *start = 2;
            *end = 3;
        } else {
            *start = 0;
            *end = 1;
        }
    } else {
        if (object->player.big_jump_animation > 0) {
            *start = 1;
            *end = 0;
        } else {
            *start = 3;
            *end = 2;
        }
    }
}

static void Teleport_SetCrawlTime(GAMEOBJECT *object, i32 start, i32 end) {
    if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
        f32 animation_speed = AnimSpeed(object->character_model, object->player.context_animation);
        if (animation_speed != 0.0f) {
            TELEPORT_CONFIG *teleport = object->player.teleport;

            f32 speed_abs = TeleportFabs(animation_speed);
            NUVEC *points = teleport->path->pts;
            object->player.context_duration = NuVecXZDist(&points[start], &points[end], NULL) / speed_abs;
            return;
        }
    }
    if ((object->player.teleport->flags & TELEPORT_CRAWL) != 0) {
        object->player.context_duration = 3.0f;
    } else {
        object->player.context_duration = 1.0f;
    }
}

extern "C" void Teleports_Reset(WORLDINFO *world) {
    TELEPORT_CONFIG *teleport = world->teleports;
    if (teleport == NULL) {
        return;
    }
    for (i32 index = 0; index < WORLD->teleport_count; ++index, ++teleport) {
        teleport->active = 0;
        teleport->enabled = 1;
        for (i32 flap = 0; flap < 2; ++flap) {
            teleport->flap_target[flap] = 0;
            teleport->flap_angle[flap] = 0;
        }
    }
}

extern "C" void Teleports_UpdateBeforeGameObjects(WORLDINFO *world) {
    TELEPORT_CONFIG *teleport = world->teleports;
    if (teleport == NULL) {
        return;
    }
    for (i32 index = 0; index < WORLD->teleport_count; ++index, ++teleport) {
        memset(teleport->flap_target, 0, sizeof(teleport->flap_target));
    }
}

extern "C" void Teleports_UpdateAfterGameObjects(WORLDINFO *world) {
    TELEPORT_CONFIG *teleport = world->teleports;
    NUMTX matrix;
    if (teleport == NULL) {
        return;
    }
    for (i32 index = 0; index < WORLD->teleport_count; ++index, ++teleport) {
        for (i32 flap = 0; flap < 2; ++flap) {
            teleport->flap_angle[flap] = SeekRot(teleport->flap_angle[flap], teleport->flap_target[flap], 5.0f);
            if (NuSpecialExistsFn(&(&teleport->flap1_special)[flap])) {
                matrix = (&teleport->flap1_matrix)[flap];
                NuMtxPreRotateX(&matrix,
                                (teleport->flags & TELEPORT_FLIP_FLAP) != 0 ? -teleport->flap_angle[flap] : teleport->flap_angle[flap]);
                NuSpecialSetDrawMtx(&(&teleport->flap1_special)[flap], &matrix);
                NuSpecialUpdate(&(&teleport->flap1_special)[flap]);
            }
        }
    }
}

extern "C" void Teleport_NetMoveCode(GAMEOBJECT *object) {
    if (object->player.character_context != CHARACTER_CONTEXT_TELEPORT || object->player.context_phase != 0) {
        return;
    }
    i32 rotation = (object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0 ? 0x4000 : 0xc000;
    for (i32 flap = 0; flap < 2; ++flap) {
        if (NuSpecialExistsFn(&(&object->player.teleport->flap1_special)[flap])) {
            if (!(NuVecDistSqr(&object->collision_position, NuSpecialGetDrawPos(&(&object->player.teleport->flap1_special)[flap]), NULL) <
                  0.36f)) {
                continue;
            }
            object->player.teleport->flap_target[flap] = (u16)(flap == 0 ? rotation : -rotation);
            if ((object->player.saved_pitch & (1 << flap)) == 0) {
                object->player.saved_pitch |= (u16)(1 << flap);
                PlaySfx("env_door_flap", &object->collision_position);
            }
        }
    }
}

extern "C" void Teleport_MoveCode(GAMEOBJECT *object, i32 start_immediately) {
    TELEPORT_CONFIG *teleport;
    i32 start_index;
    i32 end_index;
    if ((object->character_data->model_flags & CHARACTER_MODEL_TELEPORT) == 0 && SuperWeirdo(object) == 0) {
        return;
    }
    if (object->player.character_context == CHARACTER_CONTEXT_TELEPORT) {
        if (object->player.context_phase == 0) {
            i32 first_rotation = (object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0 ? 0x4000 : 0xc000;
            for (i32 i = 0; i < 2; ++i) {
                if (NuSpecialExistsFn(&(&object->player.teleport->flap1_special)[i])) {
                    NUVEC *position = &object->collision_position;
                    if (!(NuVecDistSqr(position, NuSpecialGetDrawPos(&(&object->player.teleport->flap1_special)[i]), NULL) < 0.36f)) {
                        continue;
                    }
                    object->player.teleport->flap_target[i] = (u16)(i == 0 ? (u16)first_rotation : -(u16)first_rotation);
                    if ((object->player.saved_pitch & (1 << i)) == 0) {
                        object->player.saved_pitch |= (1 << i);
                        PlaySfx("env_door_flap", position);
                    }
                }
            }
        }
        switch (object->player.context_phase) {
            case 0: {
                i32 completed = 0;
                if (object->character_model->model_data_b[object->player.context_animation] == NULL ||
                    CurrentAnim(&object->anim_packet) == object->player.context_animation) {
                    object->player.context_timer -= FRAMETIME;
                    if (object->player.context_timer <= 0.0f) {
                        object->player.character_context = CHARACTER_CONTEXT_NONE;
                        object->player.context_timer = 0.0f;
                        object->player.teleport->active = 0;
                        completed = 1;
                        if ((object->player.teleport->flags & TELEPORT_HACKY_CAM) != 0) {
                            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
                        }
                        AlertSurroundingCreatures(object, &object->collision_position);
                    }
                }
                f32 progress = 1.0f - object->player.context_timer / object->player.context_duration;
                TeleportGetSegment(&start_index, &end_index, object);
                teleport = object->player.teleport;
                NUVEC *segment_start = &teleport->path->pts[start_index];
                NUVEC *segment_end = &teleport->path->pts[end_index];
                NUVEC target;
                target.x = (segment_end->x - segment_start->x) * progress + segment_start->x;
                target.y = (segment_end->y - segment_start->y) * progress + segment_start->y;
                target.z = (segment_end->z - segment_start->z) * progress + segment_start->z;
                SeekVec(&object->position, &object->position, &target, 10.0f);
                object->velocity = v000;
                f32 shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
                object->floor_height = shadow_height;
                if (shadow_height != 2000000.0) {
                    object->position.y = shadow_height - object->character_bottom * object->origin_scale;
                    GetSurfaceInfo(object, 1, shadow_height);
                } else {
                    GetSurfaceInfo(object, 0, shadow_height);
                }
                object->position.y += (f64)progress * (f64)0.0025f;
                if (completed == 0) {
                    return;
                }
                teleport = object->player.teleport;
                if ((teleport->flags & TELEPORT_CRAWL) != 0 || object->player.big_jump_animation != 0) {
                    object->velocity.y = -0.1f;
                    object->ground_flags = 0;
                    return;
                }
                object->player.big_jump_animation = 1;
                object->player.character_context = CHARACTER_CONTEXT_TELEPORT;
                object->player.context_phase = 1;
                object->player.context_timer = 2.0f;
                object->start_position =
                    teleport->path->pts[(object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0 ? 1 : 2];
                object->position = object->start_position;
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
                return;
            }
            case 1: {
                i32 landed = 0;
                object->player.context_timer -= FRAMETIME;
                f32 shadow_height = 2000000.0f;
                f32 progress;
                if (object->player.context_timer <= 0.0f) {
                    progress = 1.0f;
                    object->player.context_phase = 0;
                    TeleportGetSegment(&start_index, &end_index, object);
                    Teleport_SetCrawlTime(object, start_index, end_index);
                    object->player.context_timer = object->player.context_duration;
                    teleport = object->player.teleport;
                    NUVEC *segment_start = &teleport->path->pts[start_index];
                    NUVEC *segment_end = &teleport->path->pts[end_index];
                    object->movement_facing_angle = object->facing_angle = object->movement_angle =
                        NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
                    shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
                    object->floor_height = shadow_height;
                    landed = 1;
                    GetSurfaceInfo(object, shadow_height != 2000000.0, shadow_height);
                } else {
                    progress = 1.0f - object->player.context_timer * 0.5;
                }
                if ((object->player.context_variant_flags & APIOBJECT_TELEPORT_REVERSE) != 0) {
                    start_index = 1;
                    end_index = 2;
                } else {
                    start_index = 2;
                    end_index = 1;
                }
                NUVEC *points = object->player.teleport->path->pts;
                NUVEC *start = &points[start_index];
                NUVEC *end = &points[end_index];
                object->position.x = (end->x - start->x) * progress + start->x;
                object->position.y = (end->y - start->y) * progress + start->y;
                object->position.z = (end->z - start->z) * progress + start->z;
                object->velocity = v000;
                if (landed != 0 && shadow_height != 2000000.0) {
                    object->position.y = shadow_height - object->character_bottom * object->origin_scale;
                }
                return;
            }
            default:
                return;
        }
    } else {
        if (object->ground_flags == 0 || object->dead != 0) {
            return;
        }
        if (object->player.character_context != CHARACTER_CONTEXT_NONE && object->player.character_context != CHARACTER_CONTEXT_LAND_JUMP &&
            object->player.character_context != CHARACTER_CONTEXT_LAND_JUMP_2 &&
            object->player.character_context != CHARACTER_CONTEXT_LAND_FLIP &&
            object->player.character_context != CHARACTER_CONTEXT_LAND_COMBO_JUMP && objInNetWaitContext(object, 15) == 0) {
            return;
        }
        if (!(object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && object->use_action != 3) {
            return;
        }
        teleport = Teleport_Find(object, 0.0f);
        if (teleport == NULL) {
            return;
        }
        if (objInNetWaitContext(object, 15) != 0) {
            object->player.context_timer -= FRAMETIME;
            if (object->player.context_timer <= 0.0f) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                object->player.panel_request_data = NULL;
            }
        }
        if (start_immediately == 0 && (objInNetWaitContext(object, 15) == 0 || object->player.panel_request_data == NULL)) {
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                object->player.flags_e24 |= APIOBJECT_TELEPORT_WAITING;
            }
            return;
        }
        object->player.character_context = CHARACTER_CONTEXT_TELEPORT;
        object->player.grapple = teleport;
        teleport->active = 1;
        object->player.teleport_reverse = FindTeleport_Direction == 0;
        PlayJumpSfx(object, 0);
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
            Hint_SetComplete(0x268);
        }
        object->player.context_animation = ACTION_CRAWL;
        object->player.context_phase = 0;
        object->player.big_jump_animation = 0;
        TeleportGetSegment(&start_index, &end_index, object);
        Teleport_SetCrawlTime(object, start_index, end_index);
        object->player.context_timer = object->player.context_duration;
        teleport = object->player.teleport;
        NUVEC *segment_start = &teleport->path->pts[start_index];
        NUVEC *segment_end = &teleport->path->pts[end_index];
        object->movement_facing_angle = NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
        object->velocity = v000;
        object->player.saved_pitch = 0;
    }
}

extern "C" {
    i32 Teleport_UpdateHints(HINT *hint) {
        if (WORLD->area != NULL && WORLD->area == HUB_ADATA) {
            return 0;
        }
        if (WORLD->teleport_count <= 0 || AvailableToPlayer(CHARACTER_MODEL_TELEPORT, -1, -1, 0) == 0) {
            return 0;
        }
        if ((Player[0] != NULL &&
             ((Player[0]->player.flags_e24 & APIOBJECT_TELEPORT_WAITING) != 0 || Teleport_Find(Player[0], 1.5625f) != NULL)) ||
            (Player[1] != NULL &&
             ((Player[1]->player.flags_e24 & APIOBJECT_TELEPORT_WAITING) != 0 || Teleport_Find(Player[1], 1.5625f) != NULL))) {
            return 1;
        }
        return 0;
    }

    i32 Teleport_ActivateRev(GIZMO *gizmo, i32 reverse, i32 mode) {
        TELEPORT_CONFIG *teleport;
        if (gizmo == NULL) {
            return 0;
        }
        if (gizmo->object == NULL) {
            return 0;
        }
        teleport = gizmo->teleport;
        if ((mode & 1) != 0 && teleport->enabled != reverse) {
            return 0;
        }
        if (reverse != 0) {
            teleport->enabled = 0;
        } else if (teleport->enabled == 0) {
            teleport->enabled = 1;
        }
        return 1;
    }

    void Teleport_Activate(GIZMO *gizmo, i32 active) {
        TELEPORT_CONFIG *teleport = gizmo->teleport;
        if (active != 0) {
            teleport->enabled = 1;
            teleport->active = 0;
        } else {
            teleport->enabled = 0;
        }
    }

    i32 Teleport_GetMaxGizmos(WORLDINFO *world_ptr) {
        WORLDINFO *world = world_ptr;
        if (world != NULL) {
            return world->current_level->max_teleports;
        }
        return 0;
    }

    void Teleport_AddGizmos(GIZMOSYS *giz, i32 type, WORLDINFO *world_ptr, void *data) {
        WORLDINFO *world = world_ptr;
        i32 i;
        if (world == NULL || world->teleports == NULL) {
            return;
        }
        for (i = 0; i < world->teleport_count; ++i) {
            AddGizmo(giz, type, NULL, &world->teleports[i]);
        }
    }

    char *Teleport_GetGizmoName(GIZMO *gizmo) {
        if (gizmo == NULL || gizmo->object == NULL) {
            return NULL;
        }
        return (char *)gizmo->object;
    }

    i32 Teleport_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
        if (gizmo != NULL && gizmo->object != NULL) {
            return gizmo->teleport->active;
        }
        return 0;
    }

    char *Teleport_GetOutputName(GIZMO *gizmo, i32 output) {
        return "Occupied";
    }

    i32 Teleport_GetNumOutputs(GIZMO *gizmo) {
        return 1;
    }

    static ADDGIZMOTYPE GizmoRegistryTeleportAddType = {0};
    i32 teleport_gizmotype_id = -1;

    ADDGIZMOTYPE *Teleport_RegisterGizmo(i32 type_id) {
        ADDGIZMOTYPE *addtype = &GizmoRegistryTeleportAddType;
        *addtype = Default_ADDGIZMOTYPE;
        addtype->prefix = TeleportConfigData.name_prefix;
        addtype->fns.options = 0x0;
        addtype->fns.early_update_fn = NULL;
        addtype->fns.late_update_fn = NULL;
        addtype->fns.draw_fn = NULL;
        addtype->fns.panel_draw_fn = NULL;
        addtype->fns.set_visibility_fn = NULL;
        addtype->fns.get_visibility_fn = NULL;
        addtype->fns.get_pos_fn = NULL;
        addtype->fns.using_special_fn = NULL;
        addtype->fns.allocate_progress_data_fn = NULL;
        addtype->fns.clear_progress_fn = NULL;
        addtype->fns.store_progress_fn = NULL;
        addtype->fns.reset_fn = NULL;
        addtype->fns.reserve_buffer_space_fn = NULL;
        addtype->fns.load_fn = NULL;
        addtype->fns.post_load_fn = NULL;
        addtype->fns.add_level_sfx_fn = NULL;
        addtype->name = "Teleport";
        addtype->fns.get_max_gizmos_fn = (i32(*)(void *))Teleport_GetMaxGizmos;
        addtype->fns.add_gizmos_fn = (void *)Teleport_AddGizmos;
        addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO *))Teleport_GetGizmoName;
        addtype->fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))Teleport_GetOutput;
        addtype->fns.get_output_name_fn = (char *(*)(GIZMO *, i32))Teleport_GetOutputName;
        addtype->fns.get_num_outputs_fn = (void *)Teleport_GetNumOutputs;
        addtype->fns.activate_fn = (void (*)(GIZMO *, i32))Teleport_Activate;
        addtype->fns.activate_rev_fn = (i32(*)(GIZMO *, i32, i32))Teleport_ActivateRev;
        teleport_gizmotype_id = type_id;
        return addtype;
    }
}
