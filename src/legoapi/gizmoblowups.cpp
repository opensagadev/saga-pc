#include "gameapi/edtools/edfile.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/ai/game/gameantinode.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" void GizmoblowupsNuError(void) {
}

typedef struct {       // size 0x100
    u32 triggered[16]; // 0x00
    u32 enabled[16];   // 0x40
    u32 visible[16];   // 0x80
    u32 completed[16]; // 0xc0
} BLOWUP_PROGRESS;

typedef struct {              // size 0x100
    u32 blown_up[16];         // 0x00
    u32 activated[16];        // 0x40
    u32 visible[16];          // 0x80
    u32 secondary_output[16]; // 0xc0
} BLOWUP_RESET_PROGRESS;

char SpinnerOutputBuffer[14] = "100% Complete";
i32 blowup_gizmotype_id = -1;
i32 MAXBLOWUPRANDSPANG = 60;
char TurretOutputBuffer[32] = {0};
char *GizmoRegistryGizTurretsNamePtr = "GizTurret";

LEVEL_INST_ANIM_RESET LevInstAnimReset = {0};

void SetLevelExBlowupFlags(u32 flags) {
    EXBLOWUPFLAGS = flags;
}

void SetLevelBlowupCallback(LEVEL_BLOWUP_CALLBACK callback) {
    if (callback != NULL) {
        BlowupExFunc = callback;
    }
}

/* Pairs of (type flag, blowup flag) used to translate type flags into blowup flags. */
extern "C" u32 flagremaptab[32][2] = {
    {0x00000001, 0x00000002}, {0x00000002, 0x00000040}, {0x00000004, 0x00000001}, {0x00000008, 0x00000004}, {0x00000010, 0x00000008},
    {0x00000020, 0x00000010}, {0x00000040, 0x00000020}, {0x00000080, 0x00000080}, {0x00000100, 0x00000100}, {0x00000200, 0x00000200},
    {0x00000400, 0x00000400}, {0x00000800, 0x00000800}, {0x00001000, 0x00001000}, {0x00002000, 0x20000000}, {0x00004000, 0x00008000},
    {0x00008000, 0x00010000}, {0x00010000, 0x00020000}, {0x00020000, 0x00040000}, {0x00040000, 0x00080000}, {0x00080000, 0x00100000},
    {0x00100000, 0x00200000}, {0x00200000, 0x00400000}, {0x00400000, 0x00800000}, {0x00800000, 0x01000000}, {0x01000000, 0x02000000},
    {0x02000000, 0x04000000}, {0x04000000, 0x10000000}, {0x08000000, 0x40000000}, {0x10000000, 0x00004000}, {0x20000000, 0x08000000},
    {0x40000000, 0x00002000}, {0x80000000, 0x80000000}};
GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE GizmoBlowupHitMultiplier = {1};

extern "C" u32 RemapTypeFlagToBlowupFlag(u32 flag) {
    i32 i;
    for (i = 0; i < 32; ++i) {
        if (flagremaptab[i][0] == flag) {
            return flagremaptab[i][1];
        }
    }
    return 0;
}

extern "C" u32 RemapAllTypeFlagsToBlowupFlags(u32 flags) {
    u32 result = 0;
    i32 bit;
    for (bit = 0; bit < 32; ++bit) {
        u32 flag = flags & (1 << bit);
        if (flag != 0) {
            result |= RemapTypeFlagToBlowupFlag(flag);
        }
    }
    return result;
}

i32 InitGizmoBlowupTypes(WORLDINFO *world) {
    world->gizmo_blowup_types = NULL;
    world->gizmo_blowup_type_count = 0;
    if (world->current_level->max_gizmoblowuptypes == 0) {
        return 0;
    }
    world->gizmo_blowup_types = (GIZMOBLOWUPTYPE *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                   world->current_level->max_gizmoblowuptypes * sizeof(GIZMOBLOWUPTYPE));
    return world->gizmo_blowup_types != NULL;
}

i32 InitGizmoBlowups(WORLDINFO *world) {
    world->gizmo_blowups = NULL;
    world->gizmo_blowup_count = 0;
    if (world->current_level->max_gizmoblowups == 0) {
        return 0;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 0xf) & ~0xfu;
    world->gizmo_blowups = (GIZMOBLOWUP *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                          world->current_level->max_gizmoblowups * sizeof(GIZMOBLOWUP));
    return world->gizmo_blowups != NULL;
}

i32 InitGizmoBlowupsMtxBuffer(WORLDINFO *world) {
    world->gizmo_blowup_mtx_buffer = NULL;
    world->giz_buffer.addr = (world->giz_buffer.addr + 0x7f) & ~0x7fu;
    world->gizmo_blowup_mtx_buffer = (NUMTX *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 0x8000);
    return world->gizmo_blowup_mtx_buffer != NULL;
}

void *gizmoblowup_reservebuffers(WORLDINFO *world) {
    InitGizmoBlowupTypes(world);
    InitGizmoBlowups(world);
    InitGizmoBlowupsMtxBuffer(world);
    return world->gizmo_blowups;
}

extern "C" i32 Blowup_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world != NULL) {
        return world->current_level->max_gizmoblowups;
    }
    return 0;
}

extern "C" void Blowup_AddGizmos(GIZMOSYS *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    for (i = 0; i < w->gizmo_blowup_count; ++i) {
        if (NuStrLen(w->gizmo_blowups[i].name) != 0) {
            AddGizmo(giz, type, NULL, &w->gizmo_blowups[i]);
        }
    }
}

extern "C" char *Blowup_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? gizmo->blowup->name : NULL;
}

extern "C" i32 Blowup_GetOutput(GIZMO *gizmo, i32 output, i32 ignored) {
    GIZMOBLOWUP *blowup = (GIZMOBLOWUP *)gizmo->object;
    switch (output) {
        case BLOWUP_OUTPUT_BLOWN_UP:
            if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
                return 1;
            }
            break;
        case BLOWUP_OUTPUT_PUNCHED:
            if ((blowup->status_flags & BLOWUP_STATUS_HIT_PUNCH) != 0) {
                return 1;
            }
            break;
        case BLOWUP_OUTPUT_PLUGGING:
            if ((blowup->status_flags & BLOWUP_STATUS_PLUGGING) != 0) {
                return 1;
            }
            break;
    }
    return 0;
}

char *BlowupOutputNames[3] = {"Blownup", "Punched", "Plugging"};

extern "C" char *Blowup_GetOutputName(GIZMO *gizmo, i32 output) {
    if ((u32)output <= 2) {
        return BlowupOutputNames[output];
    }
    return NULL;
}

extern "C" i32 Blowup_GetNumOutputs(GIZMO *unused) {
    return 3;
}

extern "C" void Blowup_SetVisibility(GIZMO *gizmo, i32 visible) {
    GIZMOBLOWUP *blowup;
    if (gizmo == NULL) {
        return;
    }
    blowup = (GIZMOBLOWUP *)gizmo->object;
    blowup->visible = visible != 0;
    if (blowup->platform_id != -1) {
        PlatOnOff(blowup->platform_id, visible);
    }
    if (blowup->visible == 0 && blowup->anti_node != NULL) {
        GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, blowup->anti_node);
        blowup->anti_node = NULL;
    } else if (blowup->visible && blowup->anti_node == NULL) {
        blowup->status_flags |= 0x10000;
    }
}

extern "C" i32 Blowup_GetVisibility(GIZMO *gizmo) {
    return gizmo != NULL && (gizmo->blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0;
}

extern "C" NUVEC *GizmoBlowup_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &gizmo->blowup->position;
}

extern "C" void *Blowup_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x100);
}

extern "C" void Blowup_ClearProgress(void *owner, void *progress_ptr) {
    BLOWUP_PROGRESS *progress = (BLOWUP_PROGRESS *)progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->triggered, 0, sizeof(progress->triggered));
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
    memset(progress->completed, 0, sizeof(progress->completed));
}

extern "C" void Blowup_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    BLOWUP_PROGRESS *progress = (BLOWUP_PROGRESS *)progress_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUP *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    memset(progress->triggered, 0, sizeof(progress->triggered));
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
    memset(progress->completed, 0, sizeof(progress->completed));

    if (world == NULL || world->gizmo_blowups == NULL) {
        return;
    }
    record = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++record) {
        if (index >= 512) {
            break;
        }
        word = index / 32;
        bit = 1u << (index & 31);
        if (record->status_flags & BLOWUP_STATUS_BLOWN_UP) {
            progress->triggered[word] |= bit;
        }
        if (!(record->status_flags & BLOWUP_STATUS_VISIBLE)) {
            progress->visible[word] &= ~bit;
        }
        if (!(record->status_flags & BLOWUP_STATUS_ACTIVE)) {
            progress->enabled[word] &= ~bit;
        }
        if (record->status_flags & BLOWUP_STATUS_PLUGGING) {
            progress->completed[word] |= bit;
        }
    }
}

static void Blowup_Reset(GIZMOBLOWUP *blowup) {
    NUINSTANIM *animation;
    blowup->status_flags = (blowup->status_flags & 0xfeff7ffe) | BLOWUP_STATUS_VISIBLE | BLOWUP_STATUS_ACTIVE;
    blowup->remaining_health = blowup->initial_health;
    blowup->damage = blowup->initial_damage;
    animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL && (animation->flags & NUINSTANIM_FLAG_PLAYING)) {
        blowup->status_flags |= 0x100000;
        if (animation->flags & NUINSTANIM_FLAG_REPEATING) {
            blowup->status_flags |= 0x480000;
        }
    }
    blowup->status_flags |= 0x10000;
}

void GizBlowup_Respawn(GIZMOBLOWUP *blowup) {
    NUINSTANIM *animation;
    i32 i;
    i32 key;
    if (blowup == NULL) {
        return;
    }
    Blowup_Reset(blowup);
    if (BonusArea && VehicleArea && (blowup->draw_flags & BLOWUP_OPTION_TORPEDO_TARGET)) {
        if (NuSpecialExistsFn(&blowup->type->animated_special)) {
            NuSpecialSetVisibility(&blowup->type->animated_special, 0);
        }
    } else if (blowup->status_flags & BLOWUP_STATUS_VISIBLE) {
        NuSpecialSetVisibility(&blowup->type->animated_special, 1);
    }
    if (NuSpecialExistsFn(&blowup->type->decal_special)) {
        NuSpecialSetVisibility(&blowup->type->decal_special, 0);
    }
    blowup->animation_time = 0.0f;
    blowup->damage = blowup->initial_damage;
    blowup->remaining_health = blowup->initial_health;
    blowup->status_flags &= 0xfdf1e401;
    animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL) {
        if (BonusArea && VehicleArea && (blowup->draw_flags & BLOWUP_OPTION_TORPEDO_TARGET) && (blowup->status_flags & 0x100000)) {
            animation->flags |= NUINSTANIM_FLAG_PLAYING;
        }
        if ((animation->flags & NUINSTANIM_FLAG_PLAYING) && (animation->flags & NUINSTANIM_FLAG_REPEATING)) {
            blowup->status_flags |= 0x480000;
        }
    }
    if (blowup->platform_id != -1) {
        PlatOnOff(blowup->platform_id, 1);
    }
    for (i = 2; i < 4; ++i) {
        if (blowup->type->particle_types[i] != -1) {
            key = -1;
            AddFiniteShotDebrisEffect(&key, blowup->type->particle_types[i], &blowup->mid_position, 1);
        }
    }
}

/* Studs the level's blowups release (TotalLevelCoinTally). */
u32 GizmoBlowups_TotalScore(WORLDINFO *world) {
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    u32 total = 0;
    i32 i;
    if (blowup != NULL) {
        for (i = 0; i < world->gizmo_blowup_count; i++, blowup++) {
            total += blowup->score_multiplier * blowup->score;
        }
    }
    return total;
}

GIZMOBLOWUPTYPE *GizmoBlowupTypeAdd(WORLDINFO *world, nuhspecial_s *special, i32 flags, i32 *result) {
    i32 i;
    GIZMOBLOWUPTYPE *type;
    NUINSTANIM *animation;
    for (i = 0; i < world->gizmo_blowup_type_count; ++i) {
        if (NuSpecialCompare(&world->gizmo_blowup_types[i].animated_special, special) &&
            ((world->gizmo_blowup_types[i].animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) || flags)) {
            *result = 0;
            return NULL;
        }
    }
    if (world->gizmo_blowup_type_count >= world->current_level->max_gizmoblowuptypes) {
        return NULL;
    }
    type = &world->gizmo_blowup_types[world->gizmo_blowup_type_count];
    if (special && NuSpecialExistsFn(special)) {
        type->animated_special = *special;
        NuStrCpy(type->name, NuSpecialGetName(&type->animated_special));
    }
    for (i = 0; i < 7; ++i) {
        type->particle_types[i] = -1;
    }
    type->effect_flags = 100;
    type->collidable = 1;
    type->animation_flags = 0xc000;
    if (flags) {
        type->animation_flags = 0xe000;
    }
    type->animation_start_frame = 1.0f;
    animation = (NUINSTANIM *)NuSpecialGetInstAnim(&type->animated_special);
    if (animation) {
        type->animation_end_frame = NuAnimEndFrameOld(type->animated_special.scene->instance_animation_data[animation->animation_index]);
    } else {
        type->animation_end_frame = 1.0f;
    }
    type->animation_base_frame = 1.0f;
    type->spin = 0.0f;
    type->scale = 1.0f;
    GameAntiNodeData_Init(&type->anti_node_data, special);
    ++world->gizmo_blowup_type_count;
    return type;
}

void GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE *type, WORLDINFO *world) {
    GIZMOBLOWUPTYPE *types;
    GIZMOBLOWUPTYPE *last_allocated_type;
    GIZMOBLOWUPTYPE *moved_type;
    GIZMOBLOWUPTYPE *slot;
    i32 i;
    if (world == NULL || world->gizmo_blowup_type_count <= 0 || type == NULL) {
        return;
    }
    types = world->gizmo_blowup_types;
    if (type < types) {
        return;
    }
    if (type > types + world->current_level->max_gizmoblowuptypes) {
        return;
    }
    last_allocated_type = types + world->current_level->max_gizmoblowuptypes - 1;
    if (type < last_allocated_type) {
        for (moved_type = type + 1; moved_type <= last_allocated_type; ++moved_type) {
            for (i = 0; i < world->gizmo_blowup_count; ++i) {
                GIZMOBLOWUP *instance = &world->gizmo_blowups[i];
                GIZMOBLOWUPTYPE *assigned = instance->type;
                if (assigned == moved_type) {
                    instance->type = assigned - 1;
                }
            }
        }
    }
    slot = type;
    while (slot != world->gizmo_blowup_types + (world->gizmo_blowup_type_count - 1)) {
        *slot = *(slot + 1);
        ++slot;
    }
    memset(slot, 0, sizeof(*slot));
    --world->gizmo_blowup_type_count;
}

extern "C" f32 GizmoBlowup_SetAutoSetReflectY(GIZMOBLOWUP *blowup, NUVEC *position) {
    if (blowup != NULL) {
        f32 ground;
        if (blowup->platform_id != 0) {
            PlatOnOff(blowup->platform_id, 0);
        }
        ground = GameShadow(NULL, position, 5.0f, -1);
        if (blowup->platform_id != 0) {
            PlatOnOff(blowup->platform_id, 1);
        }
        if (ground != 2000000.0f) {
            return ground - 0.0;
        }
    }
    return 0.0f;
}

void GizmoSwapBlowups(GIZMOBLOWUP *first, GIZMOBLOWUP *second) {
    GIZMOBLOWUP temporary = *first;
    *first = *second;
    *second = temporary;
}

void GizmoSortBlowups(WORLDINFO *world) {
    i32 blowup_index = 0;
    i32 type_index;
    GIZMOBLOWUPTYPE *type = world->gizmo_blowup_types;
    GIZMOBLOWUP *blowup;
    GIZMOBLOWUP *candidate;
    GIZMOBLOWUP *end;
    for (type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index, ++type) {
        while (blowup_index < world->gizmo_blowup_count) {
            blowup = &world->gizmo_blowups[blowup_index];
            if (blowup->type != type) {
                candidate = blowup + 1;
                end = world->gizmo_blowups + world->gizmo_blowup_count;
                for (;;) {
                    if (candidate == end) {
                        goto next_type;
                    }
                    if (candidate->type == type) {
                        break;
                    }
                    ++candidate;
                }
                GizmoSwapBlowups(blowup, candidate);
            }
            ++blowup_index;
        }
    next_type:;
    }
}

void GizmoBlowUp_AddEffects(NUVEC *position, GIZMOBLOWUP *blowup, i32 offset, i32 flags, GAMEOBJECT *object) {
    GIZMOBLOWUPTYPE *type = blowup->type;
    NUVEC effect_position = *position;
    i32 i;
    i32 j;
    i32 count;
    i32 key;
    i32 hearts;
    i32 random;
    i32 explosion_flags;
    NUVEC direction;
    NUVEC velocity;
    NUMTX matrix;
    ADDPART part;
    if (offset) {
        effect_position.x += blowup->file_offset.x;
        effect_position.y += blowup->file_offset.y;
        effect_position.z += blowup->file_offset.z;
    }
    if (flags & BLOWUP_EFFECT_DEBRIS) {
        for (i = 4; i < 7; ++i) {
            if (type->particle_types[i] != -1) {
                key = -1;
                AddFiniteShotDebrisEffect(&key, type->particle_types[i], &effect_position, 1);
            }
        }
    }
    if (flags & BLOWUP_EFFECT_PARTICLES) {
        for (i = 0; i < 2; ++i) {
            if (type->particle_types[i] != -1) {
                AddFiniteShotPART(type->particle_types[i], &effect_position, 1);
            }
        }
    }
    if (flags & BLOWUP_EFFECT_PICKUPS) {
        count = blowup->score_multiplier * blowup->score;
        direction = v010;
        NuVecRotateX(&direction, &direction, blowup->pickup_x_rotation);
        NuVecRotateY(&direction, &direction, blowup->pickup_y_rotation);
        hearts = count > 0 ? ReleaseHearts() : 0;
        AddPickups(count, hearts, 0, (blowup->draw_flags >> 8) & 1, position, &direction, 0.0f, -1, 1.5f, 2000000.0f, NULL, 1, 1);
    }
    if (flags & BLOWUP_EFFECT_SHAKE) {
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
    }
    if (((blowup->draw_flags & BLOWUP_OPTION_FRAGMENTS_PER_HIT) != 0 && (i8)blowup->remaining_health > 0) ||
        (i8)blowup->remaining_health <= 0) {
        for (i = 0; i < 4; ++i) {
            if (!NuSpecialExistsFn(&type->alternate_specials[i])) {
                continue;
            }
            for (j = 0; j < type->fragment_count; ++j) {
                NuMtxSetIdentity(&matrix);
                matrix.translation = *position;
                velocity = v010;
                random =
                    (i16)(qrand() * PlayerRandomScale * ((MAXBLOWUPRANDSPANG << 16) / 360) - ((i16)(MAXBLOWUPRANDSPANG * 0.5) << 16) / 360);
                NuVecRotateX(&velocity, &velocity, blowup->pickup_x_rotation + random);
                random =
                    (i16)(qrand() * PlayerRandomScale * ((MAXBLOWUPRANDSPANG << 16) / 360) - ((i16)(MAXBLOWUPRANDSPANG * 0.5) << 16) / 360);
                NuVecRotateY(&velocity, &velocity, blowup->pickup_y_rotation + random);
                NuVecScale(&velocity, &velocity, type->fragment_speed);
                if (object != NULL) {
                    velocity.x += 0.75 * object->velocity.x;
                    velocity.y += 0.75 * object->velocity.y;
                    velocity.z += 0.75 * object->velocity.z;
                }
                part = Default_ADDPART;
                part.time_step = FRAMETIME;
                part.matrix = &matrix;
                part.velocity = &velocity;
                part.flicker_flags = 1;
                part.stop_fn = PartStop_Flickerer;
                part.draw_fn = PartDraw_Flickerer;
                part.flags |= PART_FLAG_ROTATE_RANDOMLY;
                part.gravity = type->fragment_gravity;
                part.special = &type->alternate_specials[i];
                AddPart(&part);
            }
        }
    }
    if (GizmoBlowUp_SfxFn != NULL) {
        GizmoBlowUp_SfxFn(blowup, position);
    } else {
        GameAudio_PlaySfx(0x33, position, 0, 0);
    }
    if (flags & BLOWUP_EFFECT_EXPLOSION) {
        explosion_flags = (blowup->draw_flags & BLOWUP_OPTION_CARRIED_ONLY) ? 0x122 : 0x27;
        if (blowup->draw_flags & BLOWUP_OPTION_KNOCKBACK) {
            explosion_flags |= 8;
        }
        AddExplosion(position, blowup->effect_delay, 0.5f, NULL, -1, explosion_flags);
    }
}

CHECK_LOST_DATA CheckLostDataFn = 0;

i32 GizmoBlowupBlowup(GIZMOBLOWUP *blowup, i32 effects, i32 hit_type, i32 damage, GAMEOBJECT *object, i32 hit_context) {
    NUINSTANIM *animation;
    NUINSTANIM *burst;
    GIZMOBLOWUPTYPE *type;
    i32 has_burst;
    i32 destroyed = 0;
    i32 effect_flags = 0;
    i32 pending = 0;
    i32 player;
    GAMEOBJECT *target;
    f32 radius_squared;
    f32 x;
    f32 y;
    f32 z;
    if (blowup == NULL) {
        return 0;
    }
    if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) == 0) {
        return 0;
    }
    if ((blowup->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0) {
        return 0;
    }
    animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL) {
        type = blowup->type;
        if (type->animation_start_frame != type->animation_end_frame) {
            if ((type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) == 0 && (blowup->status_flags & 0x400000) == 0) {
                if ((blowup->draw_flags & (BLOWUP_OPTION_CHARACTER_SMASH | BLOWUP_OPTION_VEHICLE_SMASH)) == 0) {
                    if (blowup->animation_time < type->animation_end_frame) {
                        blowup->status_flags |= 0x80000;
                        return 0;
                    }
                    blowup->status_flags &= ~0x80000;
                }
            } else if ((blowup->draw_flags & BLOWUP_OPTION_SPECIAL_ANIMATION) != 0) {
                if (animation->ltime < type->animation_start_frame || animation->ltime > type->animation_end_frame) {
                    return 0;
                }
            } else {
                if (blowup->animation_time < type->animation_start_frame || blowup->animation_time > type->animation_end_frame) {
                    return 0;
                }
            }
        }
    }
    type = blowup->type;
    switch (hit_type) {
        case BLOWUP_HIT_PUNCH:
            blowup->status_flags |= BLOWUP_STATUS_HIT_PUNCH;
            break;
        case BLOWUP_HIT_BATARANG:
            blowup->status_flags |= BLOWUP_STATUS_HIT_BATARANG;
            break;
        case BLOWUP_HIT_BOLT:
            blowup->status_flags |= BLOWUP_STATUS_HIT_BOLT;
            break;
        case BLOWUP_HIT_PROXIMITY:
            blowup->status_flags |= BLOWUP_STATUS_HIT_PROXIMITY;
            break;
        case BLOWUP_HIT_PROXIMITY_VEHICLE:
            blowup->status_flags |= BLOWUP_STATUS_HIT_PROXIMITY_VEHICLE;
            break;
        case BLOWUP_HIT_SIGNAL:
            blowup->status_flags |= BLOWUP_STATUS_HIT_SIGNAL;
            break;
        case BLOWUP_HIT_CARRIED:
            blowup->status_flags |= BLOWUP_STATUS_HIT_CARRIED;
            break;
        case BLOWUP_HIT_TRANSFORM:
            blowup->status_flags |= BLOWUP_STATUS_HIT_TRANSFORM;
            break;
        case BLOWUP_HIT_EXPLOSION:
            blowup->status_flags |= BLOWUP_STATUS_HIT_EXPLOSION;
            break;
        case 10:
            blowup->status_flags |= 0x400;
            break;
        case BLOWUP_HIT_TORPEDO:
            blowup->status_flags |= BLOWUP_STATUS_HIT_TORPEDO;
            break;
        case 12:
            blowup->status_flags |= 0x1000;
            break;
        case BLOWUP_HIT_SABRE:
            blowup->status_flags |= BLOWUP_STATUS_HIT_SABRE;
            break;
    }
    has_burst = NuSpecialExistsFn(&type->burst_special);
    if (has_burst) {
        burst = (NUINSTANIM *)NuSpecialGetInstAnim(&type->burst_special);
        if (burst != NULL) {
            burst->flags |= NUINSTANIM_FLAG_PLAYING;
        }
    }
    if (damage != -1) {
        blowup->remaining_health = (u8)(blowup->remaining_health - (u8)damage);
    }
    if (damage == -1 || (i8)blowup->remaining_health <= 0) {
        destroyed = 1;
        blowup->remaining_health = 0;
        if ((type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) != 0 && (blowup->draw_flags & BLOWUP_OPTION_SPECIAL_ANIMATION) != 0) {
            NuSpecialSetVisibility(&type->animated_special, 0);
        }
        blowup->status_flags = (blowup->status_flags & 0xfe733fff) | BLOWUP_STATUS_BLOWN_UP;
        blowup->animation_time = 1.0f;
        if (object != NULL) {
            if (blowup->damage != 0) {
                ObjHitObj(NULL, object, blowup->damage, 1, 0, hit_context);
            }
        } else if (((blowup->draw_flags & BLOWUP_OPTION_CHARACTER_SMASH) != 0 || (blowup->draw_flags & BLOWUP_OPTION_VEHICLE_SMASH) != 0) &&
                   blowup->damage > 0 && (!(blowup->proximity_radius <= 0.0f) || !(blowup->effect_delay > 0.0f))) {
            radius_squared = blowup->proximity_radius * blowup->proximity_radius;
            for (player = 0; player < 8; ++player) {
                target = Player[player];
                if (target != NULL && (target->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                    (target->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && target->dead == 0 && !(target->invulnerable_timer > 0.0f)) {
                    x = blowup->mid_position.x - target->collision_position.x;
                    y = blowup->mid_position.y - target->collision_position.y;
                    z = blowup->mid_position.z - target->collision_position.z;
                    if (x * x + y * y + z * z < radius_squared) {
                        ObjHitObj(NULL, target, blowup->damage, 1, 0, hit_context);
                    }
                }
            }
        }
        if (blowup->platform_id != -1) {
            if (!has_burst) {
                PlatOnOff(blowup->platform_id, 0);
            } else {
                PlatOnOff(blowup->platform_id, 0);
                if (blowup->burst_platform_id != -1) {
                    PlatOnOff(blowup->burst_platform_id, 1);
                }
            }
            if ((type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) != 0) {
                NuSpecialSetVisibility(&type->animated_special, 0);
            }
        }
        if ((blowup->draw_flags & BLOWUP_OPTION_NO_DECAL) == 0 && NuSpecialExistsFn(&blowup->type->decal_special)) {
            blowup->animation_time = 0.1f;
            blowup->status_flags |= 0x20000;
        }
        if ((blowup->draw_flags & BLOWUP_OPTION_CREATE_STUFF) != 0) {
            GizmoBlowupCreateStuff(blowup);
        }
        if (GameBlowUpBlownUpFn != NULL) {
            GameBlowUpBlownUpFn(blowup);
        }
        if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 && CheckLostDataFn != NULL) {
            CheckLostDataFn(blowup);
        }
        if (blowup->respawn_period > 0.0f) {
            blowup->respawn_timer = blowup->respawn_period;
        }
        effect_flags = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES;
        if ((blowup->draw_flags & BLOWUP_OPTION_SHAKE) != 0) {
            effect_flags = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES | BLOWUP_EFFECT_SHAKE;
        }
        if (blowup->anti_node != NULL) {
            GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, blowup->anti_node);
            blowup->anti_node = NULL;
        }
    } else {
        pending = (blowup->status_flags >> 21) & 1;
    }
    if (!destroyed && !pending) {
        effect_flags = BLOWUP_EFFECT_DEBRIS;
    }
    if (destroyed || pending) {
        effect_flags |= BLOWUP_EFFECT_PICKUPS;
    }
    if (blowup->effect_delay > 0.0f && destroyed) {
        effect_flags |= BLOWUP_EFFECT_EXPLOSION;
    }
    if (effects) {
        GizmoBlowUp_AddEffects(&blowup->mid_position, blowup, 1, effect_flags, object);
    }
    return 1;
}

void UpdateMidPos(GIZMOBLOWUP *blowup) {
    NUVEC minimum;
    NUVEC maximum;
    NUVEC corners[8];
    NUVEC delta;
    i32 i;
    if (blowup->draw_flags & BLOWUP_OPTION_MID_AT_ORIGIN) {
        blowup->mid_position = blowup->transform.translation;
    }
    if (blowup->anti_node == NULL || (blowup->draw_flags & BLOWUP_OPTION_MID_AT_ORIGIN) == 0) {
        NuSpecialGetBounds(&blowup->type->animated_special, &minimum, &maximum);
        corners[0].x = minimum.x;
        corners[0].y = minimum.y;
        corners[0].z = minimum.z;
        corners[1].x = maximum.x;
        corners[1].y = minimum.y;
        corners[1].z = minimum.z;
        corners[2].x = maximum.x;
        corners[2].y = minimum.y;
        corners[2].z = maximum.z;
        corners[3].x = minimum.x;
        corners[3].y = minimum.y;
        corners[3].z = maximum.z;
        corners[4].x = minimum.x;
        corners[4].y = maximum.y;
        corners[4].z = minimum.z;
        corners[5].x = maximum.x;
        corners[5].y = maximum.y;
        corners[5].z = minimum.z;
        corners[6].x = maximum.x;
        corners[6].y = maximum.y;
        corners[6].z = maximum.z;
        corners[7].x = minimum.x;
        corners[7].y = maximum.y;
        corners[7].z = maximum.z;
        NuVecMtxTransformVU0(&minimum, &minimum, &blowup->transform);
        NuVecMtxTransformVU0(&maximum, &maximum, &blowup->transform);
        for (i = 0; i < 8; ++i) {
            NuVecMtxTransformVU0(&corners[i], &corners[i], &blowup->transform);
        }
        if ((blowup->draw_flags & BLOWUP_OPTION_MID_AT_ORIGIN) == 0) {
            blowup->mid_position.x = (maximum.x - minimum.x) * 0.5 + minimum.x;
            blowup->mid_position.y = (maximum.y - minimum.y) * 0.5 + minimum.y;
            blowup->mid_position.z = (maximum.z - minimum.z) * 0.5 + minimum.z;
        }
    }
    if (blowup->anti_node == NULL && (blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0) {
        blowup->anti_node = GameAntinode_RegisterAntiNodeUsingData(WORLD->game_antinode_sys, &blowup->mid_position,
                                                                   (u16)(blowup->model_rotation_y + blowup->model_rotation_z),
                                                                   &blowup->type->anti_node_data, 0.0f, blowup->draw_flags & 0x4000);
    }
    delta.x = maximum.x - blowup->mid_position.x;
    delta.y = maximum.y - blowup->mid_position.y;
    delta.z = maximum.z - blowup->mid_position.z;
    if ((blowup->draw_flags & BLOWUP_OPTION_MID_AT_ORIGIN) == 0) {
        blowup->target_scale = NuFsqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    }
    blowup->status_flags &= ~0x10000;
}

void GizmoBlowupEarlyUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUP *blowup;
    NUINSTANIM *animation;
    i32 index;
    i32 particle;
    f32 x;
    f32 z;
    f32 radius;
    f32 end_frame;
    NUMTX matrix;
    if (!world) {
        return;
    }
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if (blowup->respawn_timer > 0.0f) {
            blowup->respawn_timer -= FRAMETIME;
            if (blowup->respawn_timer <= 2.0f) {
                blowup->status_flags |= BLOWUP_STATUS_VISIBLE;
                if (blowup->respawn_timer <= 0.0f) {
                    GizBlowup_Respawn(blowup);
                } else if (GameTimer.update_count % 6 >= 3) {
                    blowup->status_flags &= ~BLOWUP_STATUS_VISIBLE;
                }
                x = (blowup->bounds_max.x - blowup->bounds_min.x) * 0.5f;
                z = (blowup->bounds_max.z - blowup->bounds_min.z) * 0.5f;
                radius = x < z ? z : x;
                PushAway(&blowup->mid_position, radius, &blowup->bounds_min, &blowup->bounds_max, NULL, NULL, 1.0f, 4);
            }
        }
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
        if ((blowup->active && !(blowup->status_flags & BLOWUP_STATUS_BLOWN_UP)) || (blowup->status_flags & 0x20000)) {
            if (blowup->draw_flags & BLOWUP_OPTION_FOLLOW_ANIMATION) {
                blowup->transform = *NuSpecialGetInstanceMtx(&blowup->type->animated_special);
                blowup->status_flags |= 0x10000;
            }
            if (blowup->status_flags & 0x10000) {
                UpdateMidPos(blowup);
            }
            if (animation) {
                end_frame = NuAnimEndFrameOld(blowup->type->animated_special.scene->instance_animation_data[animation->animation_index]);
                if (blowup->status_flags & 0x80000) {
                    if (!(blowup->type->animation_runtime_flags & 1) &&
                        ((blowup->type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) || (blowup->status_flags & 0x400000))) {
                        blowup->type->animation_base_frame += FRAMETIME * 60.0f * animation->tfactor;
                        blowup->status_flags |= 0x10000;
                        if (blowup->type->animation_base_frame >= end_frame) {
                            if (animation->flags & NUINSTANIM_FLAG_REPEATING) {
                                blowup->type->animation_base_frame = 1.0f;
                            } else {
                                blowup->status_flags |= 0x1000000;
                            }
                        }
                        blowup->type->animation_runtime_flags |= 1;
                    }
                    blowup->status_flags |= 0x10000;
                    blowup->animation_time += FRAMETIME * 60.0f * animation->tfactor;
                    EvalAnim(&blowup->type->animated_special, blowup->animation_time, &matrix, 0);
                    blowup->mid_position.x = blowup->position.x + matrix.translation.x;
                    blowup->mid_position.y = blowup->position.y + matrix.translation.y;
                    blowup->mid_position.z = blowup->position.z + matrix.translation.z;
                    if (blowup->animation_time >= end_frame) {
                        if (animation->flags & NUINSTANIM_FLAG_REPEATING) {
                            blowup->animation_time = 0.0f;
                        } else {
                            blowup->animation_time = blowup->type->animation_start_frame;
                            blowup->status_flags |= 0x1000000;
                        }
                    }
                }
            }
            if (blowup->status_flags & 0x40000) {
                if (blowup->activation_delay > 0.0f) {
                    blowup->activation_delay -= FRAMETIME;
                    if (blowup->activation_delay <= 0.0f) {
                        if (!animation || (blowup->status_flags & 0x80000)) {
                            blowup->status_flags |= 0x1000000;
                        } else {
                            blowup->status_flags |= 0x80000;
                        }
                        blowup->status_flags &= ~0x40000;
                    }
                }
            } else if (blowup->status_flags & 0x20000) {
                if (blowup->animation_time > 0.0f) {
                    blowup->animation_time -= FRAMETIME;
                } else {
                    blowup->status_flags &= ~0x20000;
                }
            }
            if (blowup->status_flags & 0x1000000) {
                GizmoBlowupBlowup(blowup, 1, -1, 1, NULL, 1);
            }
            for (particle = 7; particle < 9; ++particle) {
                if (blowup->type->particle_types[particle] != -1) {
                    AddVariableShotDebrisEffectTimed1(blowup->type->particle_types[particle], &blowup->mid_position, 60, FRAMETIME, 0, 0,
                                                      NULL);
                }
            }
        } else if (!blowup->active && animation && (animation->flags & NUINSTANIM_FLAG_PLAYING)) {
            animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
        }
    }
}

extern "C" void PlayAnim(NUINSTANIM *animation, f32 *playback, f32 speed, i32 backwards) {
    animation->flags |= NUINSTANIM_FLAG_PLAYING;
    if (speed >= 0.0f) {
        *playback = SeekLinearF(*playback, backwards != 0 ? 1.0f : -1.0f, speed);
    }
    animation->tfactor = animation->fparam1 * *playback;
}

void GizmoBlowupLateUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUPTYPE *type;
    i32 index;
    if (!world) {
        return;
    }
    type = world->gizmo_blowup_types;
    for (index = 0; index < world->gizmo_blowup_type_count; ++index, ++type) {
        if (NuSpecialExistsFn(&type->animated_special) && NuSpecialGetInstAnim(&type->animated_special) &&
            (type->animation_runtime_flags & 1)) {
            type->animation_runtime_flags &= ~1;
        }
    }
}

extern "C" i32 GizmoBlowupTypeNameBlank(char *name) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        if (((i32 *)name)[i] != 0) {
            return 0;
        }
    }
    return 1;
}

i32 GizBlowup_InitSingleTerrain(GIZMOBLOWUP *blowup) {
    i32 instance;
    if (!blowup || (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) || !(blowup->draw_flags & BLOWUP_OPTION_TERRAIN)) {
        return 0;
    }
    if (blowup->platform_id == -1) {
        instance = NuSpecialGetInstanceix(&blowup->type->animated_special);
        if (((blowup->draw_flags & 0x2000) == 0 || (blowup->draw_flags & BLOWUP_OPTION_SPECIAL_ANIMATION) != 0) &&
            (blowup->draw_flags & BLOWUP_OPTION_SHARED_PLATFORM) != 0) {
            blowup->platform_id = (i16)FindPlatInst(instance);
        } else {
            blowup->platform_id = (i16)NewPlatInst(blowup, instance);
        }
        if (blowup->platform_id == -1) {
            return 0;
        }
        if (!(blowup->status_flags & BLOWUP_STATUS_VISIBLE)) {
            PlatOnOff(blowup->platform_id, 0);
        }
    }
    PlatInstRotate(blowup->platform_id, 1);
    if (blowup->burst_platform_id == -1 && NuSpecialExistsFn(&blowup->type->burst_special)) {
        blowup->burst_platform_id = (i16)NewPlatInst(blowup, NuSpecialGetInstanceix(&blowup->type->burst_special));
        if (blowup->burst_platform_id == -1) {
            return -1;
        }
    }
    PlatInstRotate(blowup->burst_platform_id, 1);
    return 1;
}

void GizBlowup_InitTerrain(void) {
    GIZMOBLOWUP *blowup;
    i32 index;
    if (WORLD->gizmo_blowups && WORLD->gizmo_blowup_count > 0) {
        blowup = WORLD->gizmo_blowups;
        for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup) {
            blowup->platform_id = -1;
            blowup->burst_platform_id = -1;
            if (blowup->draw_flags & BLOWUP_OPTION_TERRAIN) {
                GizBlowup_InitSingleTerrain(blowup);
            }
        }
    }
}

void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP *blowup) {
    if (!blowup) {
        return;
    }
    if (blowup->platform_id != -1) {
        DeletePlatinst(blowup->platform_id);
        blowup->platform_id = -1;
    }
    if (blowup->burst_platform_id != -1) {
        DeletePlatinst(blowup->burst_platform_id);
        blowup->burst_platform_id = -1;
    }
}

void GizBlowup_DeleteTerrain(void) {
    GIZMOBLOWUP *blowup;
    i32 index;
    if (WORLD->gizmo_blowups && WORLD->gizmo_blowup_count > 0) {
        blowup = WORLD->gizmo_blowups;
        for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup) {
            GizBlowup_DeleteSingleTerrain(blowup);
        }
    }
}

void GizmoBlowupUpdateMatrix(GIZMOBLOWUP *blowup) {
    NUVEC position;
    if (!blowup) {
        return;
    }
    position = blowup->position;
    blowup->transform = *NuSpecialGetDrawMtx(&blowup->type->animated_special);
    blowup->position = v000;
    NuMtxRotateY(&blowup->transform, blowup->model_rotation_y);
    NuMtxPreRotateX(&blowup->transform, blowup->model_rotation_x);
    NuMtxPreRotateY(&blowup->transform, blowup->model_rotation_z);
    NuMtxTranslate(&blowup->transform, &position);
}

void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP *blowup, NUMTX *matrix, i32 alternate) {
    NUVEC position;
    if (!blowup) {
        return;
    }
    NuMtxSetIdentity(matrix);
    if (alternate) {
        NuMtxScaleU(matrix, blowup->decal_scale);
    } else {
        NuMtxScaleU(matrix, (1.0f - 10.0f * blowup->animation_time) * blowup->decal_scale);
    }
    matrix->translation.x = 0.0f;
    matrix->translation.y = 0.0f;
    matrix->translation.z = 0.0f;
    NuMtxRotateY(matrix, blowup->decal_rotation_y);
    NuMtxPreRotateX(matrix, blowup->decal_rotation_x);
    NuMtxPreRotateY(matrix, blowup->decal_rotation_z);
    position.x = blowup->decal_offset.x + blowup->position.x;
    position.y = blowup->decal_offset.y + blowup->position.y;
    position.z = blowup->decal_offset.z + blowup->position.z;
    NuMtxTranslate(matrix, &position);
}

void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP *blowup, NUMTX *matrix) {
    NUVEC position;
    if (!blowup) {
        return;
    }
    NuMtxSetIdentity(matrix);
    NuMtxSetScale(matrix, &GizmoBlowupUnitScale);
    matrix->translation.x = 0.0f;
    matrix->translation.y = 0.0f;
    matrix->translation.z = 0.0f;
    NuMtxRotateY(matrix, blowup->shadow_rotation_y);
    NuMtxPreRotateX(matrix, blowup->shadow_rotation_x);
    NuMtxPreRotateY(matrix, blowup->shadow_rotation_z);
    position.x = blowup->shadow_offset.x + blowup->position.x;
    position.y = blowup->shadow_offset.y + blowup->position.y;
    position.z = blowup->shadow_offset.z + blowup->position.z;
    NuMtxTranslate(matrix, &position);
}

GIZMOBLOWUPTYPE *GizmoBlowup_FindType(char *name, WORLDINFO *world) {
    i32 i;
    if (world == NULL) {
        return NULL;
    }
    if (world->gizmo_blowup_types == NULL || world->gizmo_blowup_type_count == 0) {
        return NULL;
    }
    if (name) {
        for (i = 0; i < world->gizmo_blowup_type_count; ++i) {
            if (!NuStrCmp(world->gizmo_blowup_types[i].name, name)) {
                return &world->gizmo_blowup_types[i];
            }
        }
    }
    return NULL;
}

i32 gizmoblowup_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUPTYPE type;
    GIZMOBLOWUP blowup;
    GIZMOBLOWUPTYPE *output_type;
    GIZMOBLOWUP *output_blowup;
    char text[256];
    char lookup_text[256];
    char particle_text[256];
    char part_text[256];
    i32 version;
    i32 file_types = 0;
    i32 file_instances;
    i32 index;
    i32 particle;
    i32 alternate;
    i8 length;
    i32 missing_type;
    i32 added_types = 0;
    NUINSTANIM *animation;
    NUVEC position;
    GIZMOBLOWUPTYPE *found_type;
    if (world->gizmo_blowup_types == NULL || world->gizmo_blowups == NULL) {
        return 0;
    }
    memset(world->gizmo_blowup_types, 0, world->current_level->max_gizmoblowuptypes * sizeof(GIZMOBLOWUPTYPE));
    if (world->gizmo_blowup_count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    if (version >= 2) {
        file_types = EdFileReadInt();
    }
    file_instances = EdFileReadInt();
    if (version >= 2) {
        output_type = world->gizmo_blowup_types;
        for (index = 0; index < file_types; ++index) {
            missing_type = 0;
            memset(&type, 0, sizeof(type));
            length = EdFileReadChar();
            EdFileRead(text, length);
            NuStrCpy(type.name, text);
            length = EdFileReadChar();
            EdFileRead(text, length);
            Gizmo_FindNuSpecial(world->current_gscn, &type.animated_special, text, 1, world->gizmo_sys, "BlowUp", type.name);
            if (!NuSpecialExistsFn(&type.animated_special)) {
                missing_type = 1;
            }
            type.particle_types[0] = -1;
            type.particle_types[1] = -1;
            if (version >= 17) {
                for (particle = 0; particle < 2; ++particle) {
                    length = EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(part_text, length);
                        type.particle_types[particle] = -1;
                        type.particle_types[particle] = (i16)PARTLookupTypePageOnly(part_text, world->page_part);
                    }
                }
            } else if (version >= 5) {
                length = EdFileReadChar();
                if (length > 0) {
                    EdFileRead(part_text, length);
                    type.particle_types[0] = (i16)PARTLookupType(part_text);
                }
            } else {
                type.particle_types[0] = EdFileReadShort();
            }
            for (particle = 2; particle < 9; ++particle) {
                type.particle_types[particle] = -1;
            }
            if (version >= 4) {
                for (particle = 0; particle < 3; ++particle) {
                    length = EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle + 4] = -1;
                        type.particle_types[particle + 4] = (i16)LookupDebrisEffectPageOnly(particle_text, (i8)world->page_pp);
                    }
                }
            }
            if (version >= 26) {
                for (particle = 0; particle < 2; ++particle) {
                    length = EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle + 2] = -1;
                        type.particle_types[particle + 2] = (i16)LookupDebrisEffect(particle_text);
                    }
                }
            }
            if (version >= 27) {
                for (particle = 0; particle < 2; ++particle) {
                    length = EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle + 7] = -1;
                        type.particle_types[particle + 7] = (i16)LookupDebrisEffect(particle_text);
                    }
                }
            }
            type.animation_flags = (u32)EdFileReadInt();
            if (version >= 7) {
                type.effect_flags = (u32)EdFileReadInt();
                type.collidable = (u8)EdFileReadChar();
            } else {
                type.effect_flags = 100;
                type.collidable = 1;
            }
            type.animation_time_scale = version >= 8 ? EdFileReadFloat() : 0.0f;
            if (version >= 9) {
                memset(text, 0, sizeof(text));
                length = EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.decal_special, text, 1, world->gizmo_sys, "BlowUpDecal", type.name);
                }
            }
            if (version >= 14) {
                type.animation_start_frame = EdFileReadFloat();
                type.animation_end_frame = EdFileReadFloat();
            } else {
                type.animation_start_frame = 1.0f;
                animation = (NUINSTANIM *)NuSpecialGetInstAnim(&type.animated_special);
                type.animation_end_frame =
                    animation != NULL ? NuAnimEndFrameOld(type.animated_special.scene->instance_animation_data[animation->animation_index])
                                      : 1.0f;
            }
            if (version >= 15) {
                type.debris_count = (u8)EdFileReadChar();
                type.debris_type = (u8)EdFileReadChar();
            }
            if (version >= 16) {
                GameAntiNodeData_Read(&type.anti_node_data);
            } else if (NuSpecialExistsFn(&type.animated_special)) {
                GameAntiNodeData_Init(&type.anti_node_data, &type.animated_special);
            }
            if (version >= 22) {
                for (alternate = 0; alternate < 4; ++alternate) {
                    memset(text, 0, sizeof(text));
                    length = EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(text, length);
                        Gizmo_FindNuSpecial(world->current_gscn, &type.alternate_specials[alternate], text, 1, world->gizmo_sys,
                                            "BlowUpEmitObj", type.name);
                    }
                }
                type.fragment_count = (u8)EdFileReadChar();
                type.fragment_gravity = EdFileReadFloat();
                type.fragment_speed = EdFileReadFloat();
            } else if (version >= 18) {
                memset(text, 0, sizeof(text));
                length = EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.alternate_specials[0], text, 1, world->gizmo_sys, "BlowUpEmitObj",
                                        type.name);
                }
                type.fragment_count = (u8)EdFileReadChar();
                type.fragment_gravity = EdFileReadFloat();
                type.fragment_speed = EdFileReadFloat();
            } else {
                type.fragment_count = 0;
                type.fragment_gravity = -10.0f;
                type.fragment_speed = 1.0f;
            }
            if (version >= 19) {
                memset(text, 0, 16);
                length = EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    text[16] = 0;
                    Gizmo_FindNuSpecial(world->current_gscn, &type.shadow_special, text, 1, world->gizmo_sys, "BlowUpShadow", type.name);
                }
            }
            if (version >= 20) {
                memset(text, 0, 16);
                memset(&type.burst_special, 0, sizeof(type.burst_special));
                length = EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.burst_special, text, 1, world->gizmo_sys, "BlowUpSwap", type.name);
                }
            }
            if (version >= 23) {
                type.spin = EdFileReadFloat();
            }
            if (version >= 24) {
                type.scale = EdFileReadFloat();
            }
            if (!missing_type) {
                *output_type++ = type;
                ++added_types;
            }
        }
        world->gizmo_blowup_type_count = added_types;
    }
    for (index = 0; index < file_instances; ++index) {
        memset(&blowup, 0, sizeof(blowup));
        length = EdFileReadChar();
        EdFileRead(text, length);
        if (version >= 2) {
            length = EdFileReadChar();
            EdFileRead(blowup.name, length);
            blowup.name_terminator = 0;
        }
        found_type = GizmoBlowup_FindType(text, world);
        missing_type = found_type == NULL;
        if (!missing_type) {
            output_blowup = &world->gizmo_blowups[world->gizmo_blowup_count++];
            blowup.type = found_type;
        }
        EdFileReadNuVec(&position);
        blowup.model_rotation_x = (u16)EdFileReadShort();
        blowup.model_rotation_y = (u16)EdFileReadShort();
        blowup.model_rotation_z = (u16)EdFileReadShort();
        blowup.score = 0;
        blowup.score_multiplier = 1;
        blowup.file_flags = 0;
        if (version >= 2) {
            if (version < 20) {
                blowup.draw_flags = (u32)(i32)EdFileReadShort();
            } else if (version >= 28 && version < 29) {
                blowup.draw_flags = (u32)EdFileReadInt();
                EdFileReadInt();
            } else {
                blowup.draw_flags = (u32)EdFileReadInt();
            }
            blowup.secondary_flags = 0;
            if (version < 30) {
                if (blowup.draw_flags & BLOWUP_OPTION_POWERUP) {
                    blowup.draw_flags &= ~BLOWUP_OPTION_POWERUP;
                    blowup.secondary_flags = 1;
                }
                if (blowup.draw_flags & BLOWUP_OPTION_REFLECTION) {
                    blowup.secondary_flags |= 4;
                    blowup.draw_flags &= ~BLOWUP_OPTION_REFLECTION;
                }
            } else {
                blowup.secondary_flags = (u32)EdFileReadInt();
            }
            if (version == 2) {
                blowup.draw_flags |= BLOWUP_OPTION_BATARANG_TARGET | BLOWUP_OPTION_TERRAIN;
            }
            blowup.score = (u32)EdFileReadInt();
            blowup.score_multiplier = (u8)EdFileReadChar();
            blowup.file_flags = (u8)EdFileReadChar();
        }
        blowup.initial_damage = version >= 4 ? (u8)EdFileReadChar() : 0;
        blowup.proximity_radius = version >= 6 ? EdFileReadFloat() : 0.0f;
        if (version >= 8) {
            blowup.effect_delay = EdFileReadFloat();
            blowup.proximity_delay = EdFileReadFloat();
        } else {
            blowup.effect_delay = 0.0f;
            blowup.proximity_delay = 0.0f;
        }
        if (version >= 9) {
            blowup.decal_rotation_x = (u16)EdFileReadShort();
            blowup.decal_rotation_y = (u16)EdFileReadShort();
            blowup.decal_rotation_z = (u16)EdFileReadShort();
            blowup.decal_offset.x = EdFileReadFloat();
            blowup.decal_offset.y = EdFileReadFloat();
            blowup.decal_offset.z = EdFileReadFloat();
        }
        blowup.decal_scale = version >= 10 ? EdFileReadFloat() : 1.0f;
        if (version >= 11) {
            blowup.file_offset.x = EdFileReadFloat();
            blowup.file_offset.y = EdFileReadFloat();
            blowup.file_offset.z = EdFileReadFloat();
        }
        if (version >= 12) {
            blowup.initial_health = blowup.remaining_health = (u8)EdFileReadChar();
        } else {
            blowup.initial_health = blowup.remaining_health = 1;
        }
        if (version >= 13) {
            blowup.pickup_x_rotation = (u16)EdFileReadShort();
            blowup.pickup_y_rotation = (u16)EdFileReadShort();
        }
        if (version >= 19) {
            blowup.shadow_rotation_x = (u16)EdFileReadShort();
            blowup.shadow_rotation_y = (u16)EdFileReadShort();
            blowup.shadow_rotation_z = (u16)EdFileReadShort();
            blowup.shadow_offset.x = EdFileReadFloat();
            blowup.shadow_offset.y = EdFileReadFloat();
            blowup.shadow_offset.z = EdFileReadFloat();
            blowup.file_scale = EdFileReadFloat();
        }
        blowup.animation_offset = version >= 21 ? EdFileReadFloat() : 0.0f;
        if (version >= 23) {
            blowup.respawn_period = EdFileReadFloat();
        }
        if (version >= 31) {
            blowup.reflection_height = EdFileReadFloat();
        }
        if (!missing_type) {
            *output_blowup = blowup;
            output_blowup->position = position;
        }
    }
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        found_type = &world->gizmo_blowup_types[index];
        found_type->instance_count = 0;
        for (particle = 0; particle < world->gizmo_blowup_count; ++particle) {
            if (world->gizmo_blowups[particle].type == found_type) {
                ++found_type->instance_count;
            }
        }
    }
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        output_blowup = &world->gizmo_blowups[index];
        position = *NuSpecialGetDrawPos(&output_blowup->type->animated_special);
        if (position.x == output_blowup->position.x && position.y == output_blowup->position.y && position.z == output_blowup->position.z) {
            output_blowup->type->animation_flags |= GIZMOBLOWUPTYPE_AT_SPECIAL;
        }
        output_blowup->platform_id = -1;
        output_blowup->burst_platform_id = -1;
        if ((output_blowup->draw_flags & BLOWUP_OPTION_REFLECTION) == 0) {
            output_blowup->reflection_height = 0.0f;
        }
    }
    return 1;
}

void GizmoBlowupsFinalSetup(WORLDINFO *world) {
    GIZMOBLOWUPTYPE *type;
    GIZMOBLOWUP *blowup;
    NUINSTANIM *animation;
    NUVEC centre;
    i32 index;
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        type = &world->gizmo_blowup_types[index];
        type->animation_runtime_flags &= ~1;
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(&type->animated_special);
        if (animation) {
            type->animation_base_frame = animation->ltime;
        } else {
            type->animation_base_frame = 1.0f;
        }
        if (NuSpecialExistsFn(&type->decal_special)) {
            NuSpecialSetVisibility(&type->decal_special, 0);
        }
    }
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        blowup = &world->gizmo_blowups[index];
        blowup->animation_time = 1.0f;
        blowup->status_flags = (blowup->status_flags & 0xf7b7fc00) | 0x810000;
        if (NuSpecialExistsFn(&blowup->type->animated_special)) {
            if (NuSpecialExistsFn(&blowup->type->burst_special)) {
                NuSpecialSetVisibility(&blowup->type->burst_special, 0);
            }
            if (blowup->type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) {
                blowup->position = *NuSpecialGetDrawPos(&blowup->type->animated_special);
            }
            NuSpecialGetRadius(&blowup->type->animated_special, &centre, &blowup->target_scale);
            centre.x = blowup->position.x + centre.x;
            centre.y = blowup->position.y + centre.y;
            centre.z = blowup->position.z + centre.z;
            blowup->bounds_min.x = centre.x - blowup->target_scale;
            blowup->bounds_min.y = centre.y - blowup->target_scale;
            blowup->bounds_min.z = centre.z - blowup->target_scale;
            blowup->bounds_max.x = centre.x + blowup->target_scale;
            blowup->bounds_max.y = centre.y + blowup->target_scale;
            blowup->bounds_max.z = centre.z + blowup->target_scale;
            animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
            if (animation && (animation->flags & NUINSTANIM_FLAG_PLAYING)) {
                blowup->status_flags |= 0x100000;
                if (animation->flags & NUINSTANIM_FLAG_REPEATING) {
                    blowup->status_flags |= 0x480000;
                }
            }
        } else {
            centre = v000;
            blowup->bounds_max = v000;
            blowup->bounds_min = blowup->bounds_max;
        }
        GizmoBlowupUpdateMatrix(blowup);
        blowup->initial_health = (u8)(blowup->initial_health * GizmoBlowupHitMultiplier.low_byte);
    }
    GizBlowup_DeleteTerrain();
    GizBlowup_InitTerrain();
}

GIZMOBLOWUP *GizmoBlowUpOpponent(GAMEOBJECT *object, f32 range, f32 extra_radius, f32 minimum_radius, i32 mode, u32 mask, u32 value,
                                 u32 secondary_mask) {
    GIZMOBLOWUP *target = WORLD->gizmo_blowups;
    GIZMOBLOWUP *nearest = NULL;
    f32 nearest_distance;
    f32 distance;
    f32 limit;
    u32 flags;
    NUVEC delta;
    i32 i;
    i32 behind;
    if (target == NULL) {
        return NULL;
    }
    nearest_distance = range * range;
    GizmoBlowUpOpponent_Behind = 0;
    for (i = 0; i < WORLD->gizmo_blowup_count; ++i, ++target) {
        if ((target->status_flags & BLOWUP_STATUS_BLOWN_UP) || !(target->status_flags & BLOWUP_STATUS_VISIBLE) ||
            !(target->status_flags & BLOWUP_STATUS_ACTIVE) || !(target->status_flags & BLOWUP_STATUS_DRAWN) ||
            ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && target->platform_id != -1 &&
             target->platform_id == object->supporting_platform_id)) {
            continue;
        }
        flags = target->draw_flags;
        if ((flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) && ShadowMode == 0) {
            continue;
        }
        if (mask != 0) {
            if (value != 0) {
                if ((flags & mask) != value) {
                    continue;
                }
            } else if (!(flags & mask)) {
                continue;
            }
        }
        if (secondary_mask != 0 && !(target->secondary_flags & secondary_mask)) {
            continue;
        }
        if (mode == 4 && !(flags & 0x80)) {
            continue;
        }
        if (mode == 5) {
            if ((object->collision_max.y - object->collision_min.y) * 1.5 + object->collision_max.y < target->bounds_min.y ||
                object->collision_min.y > target->bounds_max.y) {
                continue;
            }
            distance = NuVecXZDistSqr(&object->collision_position, &target->mid_position, &delta);
        } else {
            if (object->collision_max.y < target->bounds_min.y) {
                continue;
            }
            distance = NuVecDistSqr(&object->collision_position, &target->mid_position, &delta);
        }
        if (!(distance < nearest_distance)) {
            continue;
        }
        if (mode == 2) {
            nearest = target;
            GizmoBlowUpOpponent_Behind = 0;
            nearest_distance = distance;
            continue;
        }
        if (extra_radius > 0.0f) {
            limit = object->bounds_radius + target->target_scale + extra_radius;
            if (distance >= limit * limit) {
                continue;
            }
            if (minimum_radius > 0.0f) {
                limit = object->bounds_radius + target->target_scale + minimum_radius;
                if (distance < limit * limit) {
                    continue;
                }
            }
        }
        NuVecRotateY(&delta, &delta, -object->movement_facing_angle);
        if (delta.z >= 0.0f) {
            behind = 1;
            if (mode != 4 || LegoActPunchBehind == -1 || object->character_model->model_data_b[LegoActPunchBehind] == NULL) {
                continue;
            }
        } else {
            behind = 0;
        }
        if (distance < nearest_distance) {
            nearest = target;
            nearest_distance = distance;
            GizmoBlowUpOpponent_Behind = behind;
        }
    }
    GizmoBlowUpOpponent_Range2 = nearest_distance;
    return nearest;
}

u32 GetLevelExBlowupFlags(void) {
    return EXBLOWUPFLAGS;
}

GIZMOBLOWUP *GizmoBlowUp_Hit(GAMEOBJECT *object, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum, NUVEC *maximum, BOLT *bolt,
                             u32 hit_type, u8 *flags) {
    i32 airborne_damage = 0;
    u32 exclude_flag_2 = EXBLOWUPFLAGS & 2;
    u32 exclude_flag_1 = EXBLOWUPFLAGS & 1;
    GIZMOBLOWUP *nearest;
    f32 nearest_distance;
    GIZMOBLOWUP *blowup;
    i32 index;
    i32 damage;
    i32 cause;
    if (object != NULL) {
        airborne_damage = object->character_data->game_character->hover_height > 0.0f;
    }
    blowup = WORLD->gizmo_blowups;
    if (blowup == NULL) {
        return NULL;
    }
    nearest_distance = 1000000.0f;
    nearest = NULL;
    for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup) {
        i32 point;
        if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) || !(blowup->status_flags & BLOWUP_STATUS_ACTIVE) ||
            !(blowup->status_flags & BLOWUP_STATUS_VISIBLE) ||
            (exclude_flag_1 && (blowup->draw_flags & BLOWUP_OPTION_RIDER_BOLTS_ONLY) && airborne_damage)) {
            continue;
        }
        if (exclude_flag_2 && (blowup->draw_flags & BLOWUP_OPTION_EX_FUNC) &&
            (hit_type == BLOWUP_HIT_TRANSFORM || hit_type == BLOWUP_HIT_EXPLOSION) && BlowupExFunc != NULL &&
            BlowupExFunc(blowup, hit_type)) {
            continue;
        }
        if (bolt != NULL && (blowup->platform_id != -1 || (blowup->draw_flags & BLOWUP_OPTION_SHOOTABLE) == 0 ||
                             ((blowup->draw_flags & BLOWUP_OPTION_RIDER_BOLTS_ONLY) && (object == NULL || object->player.rider == NULL)))) {
            continue;
        }
        if (hit_type == 0) {
            if ((blowup->draw_flags & 0x10000) == 0) {
                continue;
            }
        } else if (hit_type == BLOWUP_HIT_BOLT && (blowup->draw_flags & BLOWUP_OPTION_EX_FUNC)) {
            continue;
        }
        if (((blowup->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) && ShadowMode == 0) ||
            (hit_type == BLOWUP_HIT_CARRIED) != ((blowup->draw_flags & BLOWUP_OPTION_CARRIED_ONLY) != 0)) {
            continue;
        }
        if (maximum->x < blowup->mid_position.x - blowup->target_scale || minimum->x > blowup->target_scale + blowup->mid_position.x ||
            maximum->z < blowup->mid_position.z - blowup->target_scale || minimum->z > blowup->mid_position.z + blowup->target_scale ||
            maximum->y < blowup->mid_position.y - blowup->target_scale || minimum->y > blowup->mid_position.y + blowup->target_scale) {
            continue;
        }
        for (point = point_count - 1; point >= 0; --point) {
            if (SphereSphereOverlap(&blowup->mid_position, blowup->target_scale, &points[point], radius)) {
                NUVEC *origin = object != NULL ? &object->collision_position : &points[point];
                f32 distance = NuVecDistSqr(origin, &blowup->mid_position, NULL);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = blowup;
                }
                break;
            }
        }
    }
    if (nearest != NULL) {
        if (hit_type == BLOWUP_HIT_BATARANG || hit_type == BLOWUP_HIT_CARRIED) {
            damage = -1;
        } else {
            damage = 1;
            if (bolt != NULL) {
                damage = BoltType_FindByID(bolt->type_id, WORLD)->damage;
                if (Cheats_CheckFlags(CHEAT_DOUBLE_BOLT_DAMAGE) && (bolt->flags & (BOLT_PLAYER_1 | BOLT_PLAYER_2))) {
                    damage *= 2;
                }
            }
        }
        if (hit_type == BLOWUP_HIT_BOLT) {
            cause = 10;
        } else if (hit_type == BLOWUP_HIT_BATARANG || hit_type == BLOWUP_HIT_CARRIED) {
            cause = 9;
        } else {
            cause = hit_type == 0 ? 13 : 3;
        }
        if (GizmoBlowupBlowup(nearest, 1, cause, damage, NULL, 1)) {
            if (object != NULL) {
                NewRumble(object->player.pad->bound_pad, 0.4f, 0);
                GameCam_HitJudder();
            }
        } else if (bolt != NULL) {
            NUVEC normal;
            NuVecSub(&normal, &nearest->position, &bolt->position);
            NuVecNorm(&normal, &normal);
            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, flags);
        }
        if (BoltSys->stop_targeting != NULL) {
            BoltSys->stop_targeting(object, points);
        }
    }
    return nearest;
}

extern "C" void GizmoBlowup_HitBlowup(GAMEOBJECT *object, GIZMOBLOWUP *blowup, i32 hit_type, BOLT *bolt, NUVEC *position, u8 *hit_data,
                                      u32 mode, i32 unused) {
    if (GizmoBlowupBlowup(blowup, 1, hit_type, 1, NULL, 1)) {
        if (object != NULL) {
            NewRumble(object->player.pad->bound_pad, 0.4f, 0);
            GameCam_HitJudder();
        }
    } else if (bolt != NULL) {
        NUVEC direction;
        NuVecSub(&direction, &blowup->position, &bolt->position);
        NuVecNorm(&direction, &direction);
        Bolt_AddDeflectedBolt(bolt, &bolt->direction, &direction, hit_data);
    }
    if (BoltSys->stop_targeting != NULL) {
        BoltSys->stop_targeting(object, position);
    }
    switch (mode) {
        case 1:
            BoltSys->debris(bolt, position, -1, NULL, 0);
            if (bolt->owner != NULL) {
                NewRumble(bolt->owner->player.pad->bound_pad, 0.6f, 0);
            }
            Bolt_End(bolt, 1);
            Bolt_PlayHitSfx(bolt);
            break;
    }
}

GIZMOBLOWUP *GizmoBlowUp_FindFromPlatID(WORLDINFO *world, i32 platform) {
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    i32 index;
    if (blowup) {
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
            if (blowup->platform_id == platform) {
                return blowup;
            }
        }
    }
    return NULL;
}

GIZMOBLOWUP *GizmoBlowUp_FindByName(WORLDINFO *world, char *name) {
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    i32 index;
    if (blowup) {
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
            if (!NuStrICmp(blowup->name, name)) {
                return blowup;
            }
        }
    }
    return NULL;
}

f32 TargetDist_Near2 = 0;

GIZMOBLOWUP *GizmoBlowUp_Target(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius, f32 range_squared, i32 directional,
                                i32 planar, i32 bolt_id) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    BOLTTYPE *bolt = BoltType_FindByID(bolt_id, world);
    u16 near_angle = TargetDeg_Near;
    u16 mid_angle = TargetDeg_Mid;
    u16 far_angle = TargetDeg_Far;
    u16 angle;
    f32 near_distance = TargetDist_Near2;
    f32 mid_distance = TargetDist_Mid2;
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    GIZMOBLOWUP *nearest;
    GIZMOBLOWUP *fallback;
    f32 nearest_distance;
    f32 distance;
    f32 dot;
    NUVEC aim;
    NUVEC delta;
    NUVEC min;
    NUVEC max;
    i32 count;
    i32 i;
    if (blowup == NULL) {
        return NULL;
    }
    if (GizmoBlowUp_NoTargetFn != NULL && GizmoBlowUp_NoTargetFn(world, object) != 0) {
        return NULL;
    }
    nearest_distance = 100000000.0f;
    nearest = NULL;
    fallback = NULL;
    aim = *direction;
    min.x = position->x - radius;
    min.z = position->z - radius;
    max.x = position->x + radius;
    max.z = position->z + radius;
    if (directional != 0 && (bolt->flags_60 & 0x20000) != 0) {
        aim.y = 0.0f;
        NuVecNorm(&aim, &aim);
    }
    if (world->blowup_target_count > 0) {
        count = world->blowup_target_count;
    } else {
        count = world->gizmo_blowup_count;
    }
    if (world->blowup_target_count == 0) {
        blowup = world->gizmo_blowups;
    }
    for (i = 0; i < count; ++i, ++blowup) {
        if (world->blowup_target_count != 0) {
            blowup = world->blowup_targets[i];
        }
        if ((blowup->draw_flags & BLOWUP_OPTION_SHOOTABLE) == 0 || (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 ||
            (blowup->status_flags & 0x08800000) == 0 || (blowup->status_flags & BLOWUP_STATUS_VISIBLE) == 0) {
            continue;
        }
        if ((blowup->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0) {
            continue;
        }
        if (blowup->mid_position.x < min.x || blowup->mid_position.x > max.x || blowup->mid_position.z < min.z ||
            blowup->mid_position.z > max.z) {
            continue;
        }
        distance =
            planar == 0 ? NuVecDistSqr(&blowup->mid_position, position, &delta) : NuVecXZDistSqr(&blowup->mid_position, position, &delta);
        if (!(distance < range_squared)) {
            continue;
        }
        if (directional == 0) {
            NuVecRotateY(&aim, &v001, NuAtan2D(blowup->mid_position.x - position->x, blowup->mid_position.z - position->z));
        }
        if ((bolt->flags_60 & 0x20000) != 0) {
            delta.y = 0.0f;
        }
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (distance < near_distance && directional != 0) {
            angle = near_angle;
        } else if (distance < mid_distance) {
            angle = mid_angle;
        } else {
            angle = far_angle;
        }
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance)) {
            continue;
        }
        if (object != NULL && (GAMEOBJECT *)blowup == object->player.collision_target) {
            fallback = blowup;
        } else {
            nearest_distance = distance;
            nearest = blowup;
        }
    }
    if (nearest == NULL && fallback != NULL) {
        nearest = fallback;
    }
    return nearest;
}

i32 GizmoBlowupCheckProximity(WORLDINFO *world, GAMEOBJECT *object) {
    i32 count = 0;
    i32 special = 0;
    i32 normal = 0;
    i32 destroy = 0;
    i32 index;
    i32 hit_type;
    GIZMOBLOWUP *blowup;
    void *animation;
    f32 x;
    f32 y;
    f32 z;
    if (object == NULL || (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
        return 0;
    }
    if ((object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0 || object->player.takeover_source != NULL) {
        special = 1;
    } else {
        normal = 1;
    }
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 || (blowup->status_flags & BLOWUP_STATUS_ACTIVE) == 0 ||
            (blowup->status_flags & BLOWUP_STATUS_VISIBLE) == 0 || (special && (blowup->draw_flags & BLOWUP_OPTION_VEHICLE_SMASH) == 0) ||
            (normal && (blowup->draw_flags & BLOWUP_OPTION_CHARACTER_SMASH) == 0) || (blowup->status_flags & 0x2040000) != 0) {
            continue;
        }
        x = object->collision_position.x - blowup->mid_position.x;
        y = object->collision_position.y - blowup->mid_position.y;
        z = object->collision_position.z - blowup->mid_position.z;
        if (blowup->proximity_radius != 0.0f && !(x * x + y * y + z * z < blowup->proximity_radius * blowup->proximity_radius)) {
            continue;
        }
        if (special) {
            hit_type = BLOWUP_HIT_PROXIMITY_VEHICLE;
            blowup->status_flags |= BLOWUP_STATUS_HIT_PROXIMITY_VEHICLE;
        } else {
            hit_type = BLOWUP_HIT_PROXIMITY;
            blowup->status_flags |= BLOWUP_STATUS_HIT_PROXIMITY;
        }
        animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        if (blowup->proximity_delay > 0.0f) {
            blowup->activation_delay = blowup->proximity_delay;
            blowup->status_flags |= 0x40000;
        } else if (animation == NULL || (blowup->status_flags & 0x80000) != 0) {
            destroy = 1;
        } else {
            blowup->status_flags |= 0x2080000;
            blowup->animation_time = 0.0f;
        }
        if (destroy) {
            GizmoBlowupBlowup(blowup, 1, hit_type, 1, object, 1);
            ++count;
        }
    }
    return count;
}

void GizmoBlowupVisibilityOverrides(WORLDINFO *world) {
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    GIZMOBLOWUPTYPE *type;
    i32 index;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) && !(blowup->draw_flags & BLOWUP_OPTION_SPECIAL_ANIMATION)) {
            NuSpecialSetVisibility(&blowup->type->animated_special, 0);
        }
    }
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        type = &world->gizmo_blowup_types[index];
        if (NuSpecialExistsFn(&type->decal_special)) {
            NuSpecialSetVisibility(&type->decal_special, 0);
        }
    }
}

extern "C" i32 GizmoBlowupTypeGetCount(WORLDINFO *world) {
    if (world == NULL) {
        return -1;
    }
    return world->gizmo_blowup_type_count;
}

extern "C" char *GizmoBlowupTypeGetNameFromIndex(WORLDINFO *world, i32 index) {
    GIZMOBLOWUPTYPE *type;
    if (world == NULL || index == -1 || index > world->gizmo_blowup_type_count) {
        return NULL;
    }
    type = &world->gizmo_blowup_types[index];
    return NuStrLen(type->name) != 0 ? type->name : NULL;
}

void GizmoBlowUpTypeBlowUp(WORLDINFO *world, i32 index, NUVEC *position) {
    GIZMOBLOWUPTYPE *type;
    GIZMOBLOWUP temporary;
    i32 flags;
    if (!world || index == -1 || index > world->gizmo_blowup_type_count) {
        return;
    }
    type = &world->gizmo_blowup_types[index];
    if (!NuStrLen(type->name) || !type->name) {
        return;
    }
    type = GizmoBlowup_FindType(type->name, world);
    if (!type) {
        return;
    }
    memset(&temporary, 0, sizeof(temporary));
    temporary.type = type;
    flags = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES | BLOWUP_EFFECT_PICKUPS;
    if (temporary.effect_delay > 0.0f) {
        flags = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES | BLOWUP_EFFECT_PICKUPS | BLOWUP_EFFECT_EXPLOSION;
    }
    GizmoBlowUp_AddEffects(position, &temporary, 0, flags, NULL);
    GameAudio_PlaySfx(0x33, position, 0, 0);
}

void GizmoBlowupResetNameTable(void) {
    memset(GizmoBlowupNameTable, 0, sizeof(GizmoBlowupNameTable));
    GizmoBlowupNameCount = 0;
}

i32 GizmoBlowupGetNameTableId(char *name) {
    i32 i;
    for (i = 0; i < GizmoBlowupNameCount; ++i) {
        if (!NuStrICmp(GizmoBlowupNameTable[i], name)) {
            return i;
        }
    }
    if (GizmoBlowupNameCount < 32) {
        NuStrNCpy(GizmoBlowupNameTable[GizmoBlowupNameCount], name, sizeof(GizmoBlowupNameTable[GizmoBlowupNameCount]));
        GizmoBlowupNameCount++;
        return GizmoBlowupNameCount - 1;
    }
    return -1;
}

i32 GizmoBlowupGetTypeFromNameTableId(WORLDINFO *world, i32 id) {
    char *name;
    GIZMOBLOWUPTYPE *type;
    i32 i;
    if (id >= 0 && id < GizmoBlowupNameCount && (name = GizmoBlowupNameTable[id]) != NULL) {
        type = world->gizmo_blowup_types;
        for (i = 0; i < world->gizmo_blowup_type_count; ++i, ++type) {
            if (!NuStrICmp(type->name, name)) {
                return i;
            }
        }
    }
    return -1;
}

GIZMOBLOWUP *FindNearestGizmoBlowUp(WORLDINFO *world, NUVEC *position, f32 max_distance_squared) {
    GIZMOBLOWUP *nearest;
    f32 nearest_distance;
    i32 index;
    i32 count;
    if (world == NULL) {
        return NULL;
    }
    count = world->gizmo_blowup_count;
    if (count <= 0) {
        return NULL;
    }
    if (position == NULL) {
        return NULL;
    }
    nearest_distance = max_distance_squared;
    nearest = NULL;
    for (index = 0; index < count; ++index) {
        GIZMOBLOWUP *blowup = &world->gizmo_blowups[index];
        if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) != 0 && (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) == 0 &&
            (blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0) {
            f32 dx = blowup->position.x - position->x;
            f32 dy = blowup->position.y - position->y;
            f32 dz = blowup->position.z - position->z;
            f32 distance = dx * dx + dy * dy + dz * dz;
            if (distance < max_distance_squared && distance < nearest_distance) {
                nearest_distance = distance;
                nearest = blowup;
            }
        }
    }
    return nearest;
}

extern "C" void Blowup_Activate(GIZMO *gizmo, i32 active) {
    GIZMOBLOWUP *blowup;
    NUINSTANIM *animation;
    if (gizmo == NULL) {
        return;
    }
    blowup = (GIZMOBLOWUP *)gizmo->object;
    blowup->active = active != 0;
    animation = (NUINSTANIM *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL) {
        if (active == 0) {
            animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
            return;
        }
        if ((blowup->status_flags & 0x100000) != 0) {
            animation->flags |= NUINSTANIM_FLAG_PLAYING;
        }
    }
    if (active != 0) {
        Blowup_Reset(blowup);
    }
}

extern "C" void Blowups_Reset(void *world_ptr, void *unused, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    BLOWUP_RESET_PROGRESS *progress = (BLOWUP_RESET_PROGRESS *)progress_ptr;
    GIZMOBLOWUP *blowup;
    i32 index;
    i32 word;
    i32 old_visible;
    u32 bit;
    if (world == NULL) {
        return;
    }
    blowup = world->gizmo_blowups;
    if (blowup != NULL) {
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
            Blowup_Reset(blowup);
            if (progress != NULL && index < 512) {
                word = index / 32;
                bit = 1 << (index & 31);
                blowup->blown_up = (progress->blown_up[word] & bit) != 0;
                old_visible = blowup->visible;
                blowup->visible = (progress->visible[word] & bit) != 0;
                if (!old_visible) {
                    if (blowup->visible) {
                        GizBlowup_InitSingleTerrain(blowup);
                    }
                } else if (!blowup->visible) {
                    GizBlowup_DeleteSingleTerrain(blowup);
                }
                blowup->active = (progress->activated[word] & bit) != 0;
                blowup->secondary_output = (progress->secondary_output[word] & bit) != 0;
            }
            if (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) {
                blowup->animation_time = 0.0f;
            }
        }
    }
    GizBlowup_DeleteTerrain();
    GizBlowup_InitTerrain();
}

extern "C" void GizmoBlowupDraw(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    NUMTX matrix;
    i32 index;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0) {
            blowup->drawn = NuSpecialDrawAt(&blowup->type->animated_special, &blowup->transform);
            if ((blowup->status_flags & BLOWUP_STATUS_DRAWN) != 0) {
                NuCameraTransformScreenClip(&blowup->screen_position, &blowup->position, 1, NULL);
            }
            if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) != 0 && (blowup->status_flags & BLOWUP_STATUS_DRAWN) != 0 &&
                (blowup->draw_flags & BLOWUP_OPTION_TRANSFORMABLE) != 0 && GizmoBlowup_TransformDrawFn != NULL) {
                GizmoBlowup_TransformDrawFn(blowup);
            }
        } else {
            blowup->status_flags &= ~BLOWUP_STATUS_DRAWN;
            if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
                if ((blowup->draw_flags & BLOWUP_OPTION_NO_DECAL) == 0) {
                    GizmoBlowupGenDecalMatrix(blowup, &matrix, 0);
                    NuSpecialDrawAt(&blowup->type->decal_special, &matrix);
                }
                if (NuSpecialExistsFn(&blowup->type->burst_special) != 0) {
                    NuSpecialDrawAt(&blowup->type->burst_special, &blowup->transform);
                }
            }
        }
        if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) != 0 && (blowup->draw_flags & BLOWUP_OPTION_NO_SHADOW) == 0 &&
            NuSpecialExistsFn(&blowup->type->shadow_special) != 0) {
            GizmoBlowupGenShadowMatrix(blowup, &matrix);
            NuSpecialDrawAt(&blowup->type->shadow_special, &blowup->transform);
        }
    }
}

extern "C" void GizmoBlowupBurstDraw(void *world_pointer, void *data, f32 frame_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZMOBLOWUP *blowup = world->gizmo_blowups;
    GIZMOBLOWUPTYPE *type;
    nuhspecial_s *special;
    NUINSTANIM *animation;
    NUMTX animated_matrix;
    NUMTX draw_matrix;
    f32 end_frame;
    f32 frame;
    f32 reflection_height;
    i32 type_index;
    i32 instance_index;
    for (type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
        type = blowup->type;
        if (type != NULL) {
            GIZMOBLOWUP *decal_blowup = blowup;
            GIZMOBLOWUP *shadow_blowup = blowup;
            GIZMOBLOWUP *burst_blowup = blowup;
            for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup) {
                if (blowup->draw_flags & BLOWUP_OPTION_SPECIAL_ANIMATION) {
                    blowup->status_flags |= BLOWUP_STATUS_DRAWN;
                    NuCameraTransformScreenClip(&blowup->screen_position, &blowup->position, 1, NULL);
                    continue;
                }
                if (!(blowup->status_flags & BLOWUP_STATUS_VISIBLE)) {
                    continue;
                }
                if (blowup->draw_flags & BLOWUP_OPTION_NO_SHADOW) {
                    ResetShadowMapRendering();
                } else {
                    EnableShadowMapRendering(0);
                }
                special = &type->animated_special;
                animation = (NUINSTANIM *)NuSpecialGetInstAnim(special);
                if (animation != NULL && (blowup->status_flags & 0x80000)) {
                    end_frame =
                        NuAnimEndFrameOld(blowup->type->animated_special.scene->instance_animation_data[animation->animation_index]);
                    frame = blowup->type->animation_base_frame;
                    if (blowup->animation_offset + frame > end_frame) {
                        end_frame = blowup->animation_offset + frame - end_frame;
                        frame = end_frame + animation->first_frame;
                    } else {
                        frame = blowup->animation_offset + frame;
                    }
                    if (blowup->type->animation_flags & GIZMOBLOWUPTYPE_AT_SPECIAL) {
                        EvalAnim(special, frame, &animated_matrix, 1);
                    } else {
                        EvalAnim(special, (blowup->status_flags & 0x400000) ? frame : blowup->animation_time, &animated_matrix, 0);
                        NuMtxMulVU0(&animated_matrix, &animated_matrix, &blowup->transform);
                    }
                    blowup->drawn = NuSpecialDrawAt(special, &animated_matrix);
                    draw_matrix = animated_matrix;
                    if (blowup->status_flags & BLOWUP_STATUS_DRAWN) {
                        NuCameraTransformScreenClip(&blowup->screen_position, NUMTX_GET_ROW_VEC(&animated_matrix, 3), 1, NULL);
                    }
                } else {
                    blowup->drawn = NuSpecialDrawAt(special, &blowup->transform);
                    draw_matrix = blowup->transform;
                    if (blowup->status_flags & BLOWUP_STATUS_DRAWN) {
                        NuCameraTransformScreenClip(&blowup->screen_position, &blowup->position, 1, NULL);
                    }
                }
                ResetShadowMapRendering();
                if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) && (blowup->status_flags & BLOWUP_STATUS_DRAWN) &&
                    (blowup->draw_flags & BLOWUP_OPTION_TRANSFORMABLE) && GizmoBlowup_TransformDrawFn != NULL) {
                    GizmoBlowup_TransformDrawFn(blowup);
                }
                if (blowup->draw_flags & BLOWUP_OPTION_REFLECTION) {
                    reflection_height = draw_matrix.m31 + blowup->reflection_height;
                    if (MatrixReflection(&draw_matrix, 2, reflection_height, blowup->reflection_height, &draw_matrix)) {
                        NuRndrStartReflectionRender(0);
                        NuSpecialDrawAt(special, &draw_matrix);
                        NuRndrEndReflectionRender();
                    }
                }
            }
            if (NuSpecialExistsFn(&type->decal_special)) {
                for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++decal_blowup) {
                    if (!(decal_blowup->draw_flags & BLOWUP_OPTION_NO_DECAL) && (decal_blowup->status_flags & BLOWUP_STATUS_BLOWN_UP)) {
                        GizmoBlowupGenDecalMatrix(decal_blowup, &animated_matrix, 0);
                        NuSpecialDrawAt(&type->decal_special, &animated_matrix);
                    }
                }
            }
            if (NuSpecialExistsFn(&type->shadow_special)) {
                for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++shadow_blowup) {
                    if ((shadow_blowup->status_flags & BLOWUP_STATUS_ACTIVE) && !(shadow_blowup->draw_flags & BLOWUP_OPTION_NO_SHADOW)) {
                        GizmoBlowupGenShadowMatrix(shadow_blowup, &animated_matrix);
                        NuSpecialDrawAt(&type->decal_special, &animated_matrix);
                    }
                }
            }
            if (NuSpecialExistsFn(&type->burst_special)) {
                for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++burst_blowup) {
                    if (burst_blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) {
                        NuSpecialDrawAt(&type->burst_special, &burst_blowup->transform);
                    }
                }
            }
        }
    }
    ResetShadowMapRendering();
}

extern "C" char *GizmoRegistryNewBlowupNamePtr = "blowup";
i32 GizmoBlowupNameCount = 0;
u32 EXBLOWUPFLAGS = 0;
char GizmoBlowupNameTable[32][32] = {0};
i32 GizmoBlowUpOpponent_Behind = 0;
f32 GizmoBlowUpOpponent_Range2 = 0;
void (*GameBlowUpBlownUpFn)(void *blowup) = 0;
i32 (*BlowupExFunc)(GIZMOBLOWUP *, i32) = 0;
void (*GizmoBlowup_TransformDrawFn)(GIZMOBLOWUP *target) = NULL;
void (*GizmoBlowUp_SfxFn)(GIZMOBLOWUP *, NUVEC *) = 0;
i32 (*GizmoBlowUp_NoTargetFn)(WORLDINFO *, GAMEOBJECT *) = 0;
static ADDGIZMOTYPE GizmoRegistryNewBlowupAddType = {0};

ADDGIZMOTYPE *NewBlowup_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE *addtype = &GizmoRegistryNewBlowupAddType;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryNewBlowupNamePtr;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x100;
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))Blowup_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)Blowup_AddGizmos;
    addtype->fns.early_update_fn = (void (*)(void *, void *, f32))GizmoBlowupEarlyUpdate;
    addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizmoBlowupLateUpdate;
    addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizmoBlowupBurstDraw;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO *))Blowup_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))Blowup_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO *, i32))Blowup_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)Blowup_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO *, i32))Blowup_Activate;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO *, i32))Blowup_SetVisibility;
    addtype->fns.get_visibility_fn = (i32(*)(GIZMO *))Blowup_GetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBlowup_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))Blowup_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void (*)(void *, void *))Blowup_ClearProgress;
    addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))Blowup_StoreProgress;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))Blowups_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))gizmoblowup_reservebuffers;
    addtype->fns.load_fn = (i32(*)(void *, void *))gizmoblowup_Load;
    blowup_gizmotype_id = type_id;
    return addtype;
}

extern "C" void BlowupObjEmit_Stop(struct PART *part) {
}
