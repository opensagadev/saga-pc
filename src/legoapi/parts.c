#include <stdio.h>
#include "batman.h"
#include "gameapi/apisave.h"
#include "gameapi/edpart.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edpart_internal.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edstubs.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/part.h"
#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/nu3d/glutils.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nualloc.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nustencilshadows_PC.h"
#include <string.h>
#include <float.h>
#include "gameapi/edbits.h"

static i32 PartVectorSpreadDegrees = 30;
i32 MAXPARTS = 32;

PARTTYPE_LOOKUP part_types[128] = {0};
PART *Part = 0;
DEBRIS_INFO DebrisNullEffect = {
    "null",
    0,
    0,
    0,
    0,
    100,
    60,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0,
    0,
    0,
    0,
    40000.0f,
    0.0f,
    25.0f,
    0.0f,
    0.0f,
    0.5f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0},
    0.0f,
    1.0f,
    0,
    0,
    0,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    {{0.0f, 64, 64, 64, 0},
     {1.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0}},
    {{0.0f, 64.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0.125f,
    0.125f,
    0.0f,
    500.0f,
    {{0.0f, 500.0f}, {1.0f, 500.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 500.0f}, {1.0f, 500.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    -360.0f,
    360.0f,
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0.0f,
    0.0f,
    256.0f,
    256.0f,
    NULL,
    0.0f,
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0,
    0,
    3,
    0,
    4.0f,
    {1.0f, 1.0f, 1.0f},
    1.0f,
    0.1f,
    0.0f,
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0},
    0,
    0,
    0,
    0,
    0.0f,
    0.0f,
    0.5f,
    1.0f,
    0};
i32 DebrisSuspendDrawObjectSwitch = -1;
f32 debris_render_scale = 1.0f;

void InitPartTable(char **names) {
    i32 count = 0;
    part_debris_count = count;
    PDebNameList = names;
    if (names != NULL && *names != NULL) {
        while (*names != NULL) {
            names++;
            count++;
        }
        part_debris_count = count;
    }
}

PARTDEBRISSYS *InitPartDebris(VARIPTR *buffer, VARIPTR *end, i32 capacity, i32 named_count, char **names, i32 page) {
    PARTDEBRISSYS *system;
    i32 i;
    i32 type;
    system = (PARTDEBRISSYS *)((buffer->addr + 15) & ~(u32)15);
    buffer->addr = ((buffer->addr + 15) & ~(u32)15) + sizeof(PARTDEBRISSYS);
    if (system != NULL) {
        memset(system, 0, sizeof(*system));
        system->capacity = capacity;
        system->named_count = named_count;
        system->entries = (PARTDEBRISENTRY *)((buffer->addr + 15) & ~(u32)15);
        buffer->addr = ((buffer->addr + 15) & ~(u32)15) + capacity * sizeof(PARTDEBRISENTRY);
        if (system->entries != NULL) {
            memset(system->entries, -1, capacity * sizeof(PARTDEBRISENTRY));
            i = 0;
            if (names != NULL) {
                for (; i < system->named_count; ++i) {
                    NuStrCpy(system->entries[i].name, names[i]);
                    system->entries[i].id = -1;
                    system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
                }
            }
            for (type = 0; i < system->capacity && type < 128; ++type) {
                system->entries[i].id = -1;
                if (part_types[type].variant_count > 0) {
                    NuStrCpy(system->entries[i].name, part_types[type].name);
                    system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
                    ++i;
                }
            }
            for (; i < system->capacity; ++i) {
                system->entries[i].id = -1;
            }
            return system;
        }
    }
    return NULL;
}

i32 FindPartDebris(PARTDEBRISSYS *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = system->named_count; index < system->capacity; ++index) {
            if (NuStrICmp(name, system->entries[index].name) == 0) {
                return system->entries[index].id;
            }
        }
    }
    return -1;
}

void Parts_Start(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStartPage((u8)world->page_part);
    }
}

void Parts_Stop(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStopPage((u8)world->page_part);
    }
}

void AddPartDebris(PARTDEBRISSYS *sys, i32 index, NUVEC *position) {
    i32 effect;

    if (sys == NULL || index < 0 || index >= part_debris_count) {
        return;
    }
    effect = sys->entries[index].id;
    if (effect == -1) {
        return;
    }
    AddFiniteShotPART(effect, position, 1);
}

f32 brickimpactwait = 0;

void PartImpact_Brick(PART *part) {
    if (brickimpactwait <= 0.0f) {
        f32 speed = NuVecMag(&part->velocity);
        if (speed > 0.0f) {
            i32 angle;
            f32 volume;
            i32 sound_id;
            if (speed > 1.0f) {
                speed = 1.0f;
            }
            angle = ((i32)((1.0 - speed) * 16384.0 + 16384.0));
            volume = NuTrigTable[(angle >> 1) & 0x7fff];
            sound_id = GetSfxId("LegoSingle");
            PlaySfxByIdAndSetVolume(sound_id, &part->position, volume);
            brickimpactwait = 0.1f;
        }
    }
}

void PartStop_Flickerer(PART *part) {
    part->time_remaining = 5.0f;
    if ((part->flicker_flags & 1) != 0) {
        part->time_remaining = 2.5f;
    }
}

i32 PartDraw_Flickerer(PART *part) {
    i32 draw = 1;
    if (part->time_remaining > 0.0f && GameTimer.update_count % 6 >= 3) {
        f32 threshold = 2.0f;
        if ((part->flicker_flags & 1) != 0) {
            threshold = 1.0f;
        }
        if (part->time_remaining < threshold) {
            draw = 0;
        }
    }
    return draw;
}

PART *FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range) {
    f32 radius_squared = radius * radius;
    PART *nearest = NULL;
    PART *part = Part;
    f32 nearest_distance = range > 0.0f ? range : 100.0f;
    f32 distance;
    i32 i;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) == 0 || part->owner == owner || (part->flags & flags) != flags) {
            continue;
        }
        if (range > 0.0f) {
            distance = NuVecDistSqr(&part->position, position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = part;
            }
        } else {
            f32 speed = NuVecMag(&part->velocity);
            if (speed != 0.0f) {
                NUVEC direction;
                f32 reciprocal = 1.0f / speed;
                f32 half_speed;
                f32 limit;
                NuVecScale(&direction, &part->velocity, reciprocal);
                half_speed = (f32)((f64)speed * 0.5);
                distance = NuVecDistSqr(&part->position, position, NULL);
                limit = half_speed * half_speed;
                if (distance < limit && LineIntersectSphere(&part->position, &direction, position, radius_squared, NULL)) {
                    nearest_distance = distance;
                    nearest = part;
                }
            }
        }
    }
    return nearest;
}

void MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale) {
    if (direction != NULL) {
        i32 divisor;
        u16 z_random;
        u16 x_random;
        FindAnglesZX(direction, NULL, NULL);
        *velocity = v010;
        divisor = 65535 / (((PartVectorSpreadDegrees * 2 + 1) << 16) / 360) + 1;
        z_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees << 16) / 360));
        x_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees << 16) / 360));
        NuVecRotateZ(velocity, velocity, (u16)(z_random + temp_xrot));
        NuVecRotateX(velocity, velocity, (u16)(x_random + temp_zrot));
        NuVecScale(velocity, velocity, scale);
    } else {
        f64 random = (f64)qrand() * (f64)(1.0f / 65535.0f);
        velocity->x = (f32)(random + random - 1.0);
        velocity->y = AreaPickupGravity == 0.0f ? 0.0f : (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) + 1.0);
        random = (f64)qrand() * (f64)(1.0f / 65535.0f);
        velocity->z = (f32)(random + random - 1.0);
        NuVecScale(velocity, velocity, AreaPickupScale);
    }
}

PART *TargetPart(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius, f32 range_squared, i32 directional, i32 bolt_id) {
    NUVEC aim = *direction;
    NUVEC delta;
    NUVEC minimum;
    NUVEC maximum;
    PART *best = NULL;
    BOLTTYPE *bolt = BoltType_FindByID(bolt_id, WORLD);
    PART *part = Part;
    PART *previous = NULL;
    f32 nearest_distance = range_squared;
    f32 distance;
    f32 dot;
    u16 angle;
    i32 i;
    minimum.x = position->x - radius;
    minimum.z = position->z - radius;
    maximum.x = position->x + radius;
    maximum.z = position->z + radius;
    if (directional != 0) {
        aim = *direction;
        if ((bolt->flags_60 & 0x20000) != 0) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
    }
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) == 0 ||
            ((part->flags & 0x8000000) == 0 && (part->flags & (PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS)) !=
                                                   (PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS))) {
            continue;
        }
        if (part->position.x < minimum.x || part->position.x > maximum.x || part->position.z < minimum.z || part->position.z > maximum.z) {
            continue;
        }
        distance = NuVecDistSqr(&part->position, position, &delta);
        if (!(distance < range_squared)) {
            continue;
        }
        if (directional == 0) {
            NuVecRotateY(&aim, &v001, NuAtan2D(part->position.x - position->x, part->position.z - position->z));
        }
        if ((bolt->flags_60 & 0x20000) != 0) {
            delta.y = 0.0f;
        }
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (distance < TargetDist_Near2 && directional != 0) {
            angle = TargetDeg_Near;
        } else if (TargetDist_Mid2 > distance) {
            angle = TargetDeg_Mid;
        } else {
            angle = TargetDeg_Far;
        }
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance)) {
            continue;
        }
        if (part == object->player.collision_target) {
            previous = part;
        } else {
            nearest_distance = distance;
            best = part;
        }
    }
    if (best != NULL) {
        return best;
    }
    if (previous != NULL) {
        return previous;
    }
    return NULL;
}

PART *Part_FindFromHSpecial(nuhspecial_s *special) {
    PART *part;
    i32 index;
    if (special == NULL) {
        return NULL;
    }
    part = Part;
    for (index = 0; index < MAXPARTS; index++, part++) {
        if ((part->active & PART_IN_USE) != 0 && NuSpecialCompare(&part->special, special)) {
            return part;
        }
    }
    return NULL;
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) void DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].position.x = x;
        debkeydata[handle].position.y = y;
        debkeydata[handle].position.z = z;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void DebrisEmitterMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].emitter_momentum.x = x;
        debkeydata[handle].emitter_momentum.y = y;
        debkeydata[handle].emitter_momentum.z = z;
    }
}

void DebrisParticleMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].momentum.x = x;
        debkeydata[handle].momentum.y = y;
        debkeydata[handle].momentum.z = z;
    }
}

static void DebrisMtxCopyPrivate(NUMTX *destination, const NUMTX *source) {
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

static void DebrisMtxRotateXPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m01;
    f32 m11;
    f32 m21;
    f32 m31;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m01 = m->m01;
    m11 = m->m11;
    m21 = m->m21;
    m31 = m->m31;

    m->m01 = (m01 * cosx) - (m->m02 * sinx);
    m->m02 = (m01 * sinx) + (m->m02 * cosx);
    m->m11 = (m11 * cosx) - (m->m12 * sinx);
    m->m12 = (m11 * sinx) + (m->m12 * cosx);
    m->m21 = (m21 * cosx) - (m->m22 * sinx);
    m->m22 = (m21 * sinx) + (m->m22 * cosx);
    m->m31 = (m31 * cosx) - (m->m32 * sinx);
    m->m32 = (m31 * sinx) + (m->m32 * cosx);
}

static void DebrisMtxRotateYPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static void DebrisMtxRotateZPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = (m00 * cosx) - (m->m01 * sinx);
    m->m01 = (m00 * sinx) + (m->m01 * cosx);
    m->m10 = (m10 * cosx) - (m->m11 * sinx);
    m->m11 = (m10 * sinx) + (m->m11 * cosx);
    m->m20 = (m20 * cosx) - (m->m21 * sinx);
    m->m21 = (m20 * sinx) + (m->m21 * cosx);
    m->m30 = (m30 * cosx) - (m->m31 * sinx);
    m->m31 = (m30 * sinx) + (m->m31 * cosx);
}

i32 maxdebkeys = 0;

void DebFreeAllPanelEffects(void) {
    i32 j;
    i32 key;
    for (j = 0; j < maxdebkeys; ++j) {
        if (debkeydata[j].effect_index != 0 && debtab[debkeydata[j].effect_index]->time_group == DEBRIS_DRAW_PANEL) {
            key = j;
            DebFreeInstantly(&key);
        }
    }
}

void DebrisOrientation(i32 handle, i16 z, i16 y) {
    if (handle == -1) {
        return;
    }
    DebrisMtxCopyPrivate(&debkeydata[handle].effect_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].effect_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].effect_orientation, y);
    debkeydata[handle].orientation_dirty = 0.0f;
}

void DebrisEmitterOrientation(i32 handle, i16 z, i16 y, i16 x) {
    if (handle == -1) {
        return;
    }
    DebrisMtxCopyPrivate(&debkeydata[handle].emitter_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].emitter_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].emitter_orientation, y);
    DebrisMtxRotateXPrivate(&debkeydata[handle].emitter_orientation, x);
    debkeydata[handle].orientation_dirty = 0.0f;
}

void DebrisReflectionOrientation(i32 handle, i16 x, i16 y, f32 plane, f32 scale) {
    if (handle != -1) {
        debkeydata[handle].reflection_x = x;
        debkeydata[handle].reflection_y = y;
        debkeydata[handle].collision_plane = plane;
        debkeydata[handle].reflection_scale = scale;
    }
}

i32 DebrisGetParticleCount(i32 handle) {
    if (handle == -1) {
        return 0;
    }
    return debkeydata[handle].allocated_chunk_count;
}

void DebrisSetTrigger(i32 handle, i32 first, i32 second, f32 third) {
    if (handle != -1) {
        debkeydata[handle].trigger_first = first;
        debkeydata[handle].trigger_second = second;
        debkeydata[handle].switch_variable = third;
    }
}

void DebrisSetGroupID(i32 handle, i16 group) {
    if (handle != -1) {
        debkeydata[handle].render_group = group;
    }
}

void DebrisSetPriority(i32 handle, i16 priority, u8 automatic) {
    if (handle != -1) {
        debkeydata[handle].timed_flags = automatic;
        if (automatic == 0) {
            debkeydata[handle].render_priority = priority;
        }
    }
}

u16 DebrisQueryPriority(i32 effect_index) {
    if (effect_index >= 0 && effect_index < debinf_count && debtab[effect_index] != NULL) {
        switch (debtab[effect_index]->particle_type) {
            case DEBRIS_PARTICLE_ADDITIVE:
                return 20000;
            case DEBRIS_PARTICLE_MODULATIVE:
                return 40000;
            case DEBRIS_PARTICLE_SUBTRACTIVE:
                return 30000;
            case DEBRIS_PARTICLE_GLASS:
                return 10000;
        }
    }
    return 0;
}

void DebrisSetDrawFlag(i32 handle, i8 draw_flag) {
    if (handle != -1) {
        debkeydata[handle].draw_enabled = draw_flag;
    }
}

void DebrisStartOffset(i32 handle, f32 offset) {
    DebrisStartOffsetEx(debkeydata + handle, offset);
}

void AddDebrisEffect(i32 *handle, i32 effect_index, f32 x, f32 y, f32 z) {
    i32 i;
    DEBRIS_INFO *effect;
    i32 newly_allocated;
    i32 key_index;
    f32 now;
    newly_allocated = 0;
    if (effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL) {
        return;
    }
    if (effect->disabled != 0) {
        return;
    }

    if (*handle == -1) {
        *handle = DebAlloc();
        newly_allocated = 1;
    }
    key_index = *handle;
    if (key_index == -1) {
        return;
    }

    now = effect->time_group == DEBRIS_DRAW_PANEL ? panelglobaltime : globaltime;
    debkeydata[key_index].effect_index = (i16)(effect_index);
    debkeydata[key_index].finite = 0;
    DebrisStartOffset(key_index, effect->emission_start_offset);
    debkeydata[key_index].generator = gensorttab[effect->generator_type];
    debkeydata[key_index].momentum_adjuster = gencodetab[effect->momentum_adjustment_type];
    debkeydata[key_index].finite_count = 0;
    debkeydata[key_index].emitter_rotation_x = 0;
    debkeydata[key_index].emitter_rotation_y = 0;
    debkeydata[key_index].sphere_index = 0;
    debkeydata[key_index].sphere_next_time = 0.0f;
    for (i = 0; i < debtab[effect_index]->process_spheres; ++i) {
        debkeydata[key_index].process_spheres[i].time = -1.0f;
    }
    debkeydata[key_index].emission_position.x = 0.0f;
    debkeydata[key_index].emission_position.y = 0.0f;
    debkeydata[key_index].emission_position.z = 0.0f;
    debkeydata[key_index].momentum.x = 0.0f;
    debkeydata[key_index].momentum.y = 0.0f;
    debkeydata[key_index].momentum.z = 0.0f;
    debkeydata[key_index].emitter_momentum.x = 0.0f;
    debkeydata[key_index].emitter_momentum.y = 0.0f;
    debkeydata[key_index].emitter_momentum.z = 0.0f;
    debkeydata[key_index].portal_room = -1;
    debkeydata[key_index].orientation_flags = 7;
    debkeydata[key_index].gscene = NULL;
    debkeydata[key_index].orientation_dirty = 0.0f;
    debkeydata[key_index].cutoff_distance = 999999.0f;
    debkeydata[key_index].process_collision_sound = 0;
    debkeydata[key_index].last_update_time = now;
    debkeydata[key_index].runtime_created = 1;
    debkeydata[key_index].user_data = NULL;
    debkeydata[key_index].custom_facing = 0;
    if (debkeydata[key_index].emission_end_time < now) {
        debkeydata[key_index].emission_epoch = debkeydata[key_index].emission_end_time;
    } else {
        debkeydata[key_index].emission_epoch = now;
    }
    debkeydata[key_index].emission_disabled = 0;

    switch (effect->particle_type) {
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            debkeydata[key_index].render_priority = 40000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            debkeydata[key_index].render_priority = 20000;
            break;
        case DEBRIS_PARTICLE_MODULATIVE:
            debkeydata[key_index].render_priority = 50000;
            break;
        default:
            debkeydata[key_index].render_priority = 30000;
            break;
    }
    for (i = 0; i < 4; ++i) {
        debkeydata[key_index].collision_timers[i] = 9999;
        if (effect->sound_data[i * 3] != -1) {
            if (effect->sound_data[i * 3 + 1] == 4 || effect->sound_data[i * 3 + 1] == 3) {
                debkeydata[key_index].collision_timers[i] = 1;
            }
            debkeydata[key_index].process_collision_sound = 1;
        }
    }
    DebrisEmitterPos(key_index, x, y, z);
    DebrisEmitterOrientation(key_index, 0, 0, 0);
    DebrisOrientation(key_index, 0, 0);
    DebrisReflectionOrientation(key_index, 0, 0, 0, 0.9f);
    DebrisSetTrigger(key_index, 0, -1, 0);
    if (newly_allocated) {
        AddDebrisEffectToStack(debkeydata + key_index);
    }
}

void AddRotatedDebrisEffect(i32 *handle, i32 effect, f32 x, f32 y, f32 z, i16 rotation_x, i16 rotation_y) {
    AddDebrisEffect(handle, effect, x, y, z);
    DebrisEmitterOrientation(*handle, rotation_x, rotation_y, 0);
}

DEBRISPARTICLE *GenDebIndexSpheroid(DEBRISKEY *key, DEBRIS_INFO *effect, float time) {
    i32 trail;
    DEBRISPARTICLE *particle;
    NUVEC vector;
    i32 angle_y;
    f64 random;
    f32 sine;
    i32 angle_z;
    NUMTX scale;
    DEBRISPARTICLE *trail_particle;
    if (key->particle_cursor >= key->particle_count) {
        key->particle_cursor = 0;
    }
    if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
        particle = &key->particle_chunks[key->particle_cursor / 12]->particles[key->particle_cursor % 12];
    } else {
        particle = &key->particle_chunks[key->particle_cursor / 32]->particles[key->particle_cursor % 32];
    }
    ++key->particle_cursor;
    particle->start_time = time;
    key->emission_epoch = time;
    particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

    vector.x = NuFsqrt(NuRandFloatSeeded(&debrisseed));
    vector.y = vector.z = 0.0f;
    angle_y = (i32)(NuRandFloatSeeded(&debrisseed) * 65536.0f);
    random = NuRandFloatSeeded(&debrisseed);
    sine = random + random - 1.0f;
    angle_z = (i16)DebrisNuASin(sine);
    NuVecRotateZ(&vector, &vector, angle_z);
    NuVecRotateY(&vector, &vector, angle_y);

    DebrisMtxCopyPrivate(&scale, &ANI3_EVAL_IDENTITY_MATRIX);
    scale.m00 = effect->emitter_random_extent.x;
    scale.m11 = effect->emitter_random_extent.y;
    scale.m22 = effect->emitter_random_extent.z;
    NuVecMtxTransformVU0(&vector, &vector, &scale);
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->position.x = vector.x;
    particle->position.y = vector.y;
    particle->position.z = vector.z;
    random = NuRandFloatSeeded(&debrisseed);
    vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
    random = NuRandFloatSeeded(&debrisseed);
    vector.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
    random = NuRandFloatSeeded(&debrisseed);
    vector.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->momentum.x = vector.x;
    particle->momentum.y = vector.y;
    particle->momentum.z = vector.z;
    if (key->momentum_adjuster != NULL) {
        key->momentum_adjuster(key, effect, particle);
    }
    particle->position.x = key->emission_position.x + particle->position.x;
    particle->position.y = key->emission_position.y + particle->position.y;
    particle->position.z = key->emission_position.z + particle->position.z;
    particle->momentum.x = key->momentum.x + particle->momentum.x;
    particle->momentum.y = key->momentum.y + particle->momentum.y;
    particle->momentum.z = key->momentum.z + particle->momentum.z;

    for (trail = 0; trail < effect->trail_count; ++trail) {
        if (key->particle_cursor >= key->particle_count) {
            key->particle_cursor = 0;
        }
        if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
            trail_particle = &key->particle_chunks[key->particle_cursor / 12]->particles[key->particle_cursor % 12];
        } else {
            trail_particle = &key->particle_chunks[key->particle_cursor / 32]->particles[key->particle_cursor % 32];
        }
        ++key->particle_cursor;
        *trail_particle = *particle;
        trail_particle->start_time += (f32)(trail + 1) * effect->trail_time;
    }

    if (effect->native_data == NULL) {
        GenericDebinfoDmaTypeUpdate(effect);
    }
    return particle;
}

/* The SDK NuFabs; this unit keeps its own out-of-line copy. */
static f32 DebrisFabs(f32 f) {
    f32 rv;
    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

static f32 DebrisDivide(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void DebrisEmitterOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].emitter_orientation = *source;
        debkeydata[handle].emitter_orientation.m30 = 0.0f;
        debkeydata[handle].emitter_orientation.m31 = 0.0f;
        debkeydata[handle].emitter_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void AddVariableShotDebrisEffectTimed5(i32 effect_index, NUVEC *position, NUVEC *momentum, NUVEC *position_delta, i32 count, f32 duration,
                                       NUMTX *emitter_orientation, NUMTX *particle_orientation, u16 render_priority, i8 timed_flags) {
    DEBRIS_INFO *effect;
    NUVEC clipped_position;

    f32 thinning;
    f32 emission_interval;
    f32 rate;
    f32 now;
    f32 end_time;
    f32 elapsed_intervals;
    f32 emission_time;
    f32 emission_epoch;
    f32 next_emission_time;
    i32 panel_time;
    i32 emission_count;
    i32 i;
    i32 j;
    i32 particle;
    i32 particle_count;
    i32 particle_key_slot;
    i32 new_key_index;
    DEBRISKEY *key;
    i32 required_particles;
    i32 required_chunks;
    i32 new_chunk_count;
    i32 total_chunk_count;
    DEBRISPARTICLE *generated;
    i32 chunk_index;
    DEBRIS_CHUNK_CONTROL *control;
    DEBRIS_CHUNK_CONTROL **stack;
    DEBRIS_PARTICLE_CHUNK *last_chunk;

    if (debris_suspended != 0 || update_debris_enabled == 0 || effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL || effect->disabled != 0 || count <= 0) {
        return;
    }
    if (DEBRIS_DRAW_PANEL != effect->time_group) {
        NUVEC extent;
        clipped_position.x = position->x;
        clipped_position.y = position->y;
        clipped_position.z = position->z;
        extent.x = extent.y = extent.z = 1.0f;
        if (NuCameraClipTestExtentsAxisAligned(&clipped_position, &extent, effect->clip_extent) == 0) {
            return;
        }
    }
    thinning = 0 != forced_debris_thinning ? debris_thinning_level
                                           : (effect->thinning < debris_thinning_level ? effect->thinning : debris_thinning_level);
    rate = DebrisDivide((f32)count, thinning);
    emission_interval = DebrisDivide(1.0f, rate);
    if (emitter_orientation == NULL) {
        emitter_orientation = &NuMtxSetIdentityTemplate;
    }
    if (particle_orientation == NULL) {
        particle_orientation = &NuMtxSetIdentityTemplate;
    }
    if (effect->time_group == DEBRIS_DRAW_PANEL) {
        now = panelglobaltime;
        panel_time = 1;
    } else {
        now = globaltime;
        panel_time = 0;
    }
    end_time = now + duration;
    emission_epoch = ((f32)(i32)DebrisDivide(now, emission_interval)) * emission_interval;
    emission_time = emission_epoch + emission_interval;
    emission_count = 0;
    next_emission_time = emission_time;
    i = 0;
    while (next_emission_time <= end_time) {
        if (++i >= 100) {
            break;
        }
        ++emission_count;
        next_emission_time += emission_interval;
    }
    if (emission_count == 0) {
        return;
    }
    particle_count = emission_count * (effect->trail_count + 1);
    particle_key_slot = -1;
    key = NULL;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] != -1) {
            key = &debkeydata[effect->particle_keys[i]];
            if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
                if (key->particle_count + particle_count <= 0x180) {
                    particle_key_slot = i;
                    break;
                }
            } else if (key->particle_count + particle_count <= 0x400) {
                particle_key_slot = i;
                break;
            }
        } else if (freedebkeyptr < maxdebkeys) {
            new_key_index = DebAlloc();
            effect->particle_keys[i] = (i16)new_key_index;
            key = &debkeydata[(i16)new_key_index];
            key->effect_index = (i16)effect_index;
            key->finite_count = 0;
            key->generator = gensorttab[effect->generator_type];
            key->momentum_adjuster = gencodetab[effect->momentum_adjustment_type];
            key->effect_orientation = *particle_orientation;
            key->effect_orientation.m30 = 0.0f;
            key->effect_orientation.m31 = 0.0f;
            key->effect_orientation.m32 = 0.0f;
            DebrisEmitterPos(effect->particle_keys[i], 0.0f, 0.0f, 0.0f);
            key->render_priority = render_priority;
            key->timed_flags = timed_flags;
            particle_key_slot = i;
            break;
        }
    }
    if (key == NULL) {
        return;
    }
    required_particles = particle_count + key->particle_count;
    if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
        if (required_particles > key->allocated_chunk_count * 12) {
            if (key->allocated_chunk_count < 0) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 12 - 1) / 12;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (freedebchkptrg + new_chunk_count >= debrischunksglass || required_chunks > 32) {
                return;
            }
            for (i = 0; i < new_chunk_count; ++i) {
                key->particle_chunks[key->allocated_chunk_count] = freedebchunksglass[freedebchkptrg++];
                for (particle = 0; particle < 12; ++particle) {
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].inverse_lifetime = 32768.0f;
                }
                ++key->allocated_chunk_count;
            }
            key->allocated_chunk_count = (i16)required_chunks;
            LinkDmaParticalSets(key->particle_chunks, required_chunks);
            if (required_chunks == new_chunk_count) {
                total_chunk_count = debrischunks + debrischunksglass;
                for (j = 0; j < total_chunk_count; ++j) {
                    if (NULL == ParticleChunkToRender[j].particle_chunk) {
                        ParticleChunkToRender[j].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[j].effect = effect;
                        ParticleChunkToRender[j].key = key;
                        ParticleChunkToRender[j].render_priority = render_priority;
                        AddChunkToRenderStack(&ParticleChunkToRender[j], &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
        }
    } else {
        if (required_particles > key->allocated_chunk_count * 32) {
            if (key->allocated_chunk_count < 0) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 32 - 1) / 32;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (freedebchkptr + new_chunk_count >= debrischunks || required_chunks > 32) {
                return;
            }
            for (i = 0; i < new_chunk_count; ++i) {
                key->particle_chunks[key->allocated_chunk_count] = freedebchunks[freedebchkptr++];
                for (particle = 0; particle < 32; ++particle) {
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].inverse_lifetime = 32768.0f;
                }
                ++key->allocated_chunk_count;
            }
            key->allocated_chunk_count = (i16)required_chunks;
            LinkDmaParticalSets(key->particle_chunks, required_chunks);
            if (required_chunks == new_chunk_count) {
                total_chunk_count = debrischunks + debrischunksglass;
                for (j = 0; j < total_chunk_count; ++j) {
                    if (NULL == ParticleChunkToRender[j].particle_chunk) {
                        ParticleChunkToRender[j].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[j].effect = effect;
                        ParticleChunkToRender[j].key = key;
                        ParticleChunkToRender[j].render_priority = render_priority;
                        AddChunkToRenderStack(&ParticleChunkToRender[j], &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
        }
    }
    key->particle_count += (i16)particle_count;
    if (momentum != NULL) {
        key->momentum.x = momentum->x;
        key->momentum.y = momentum->y;
        key->momentum.z = momentum->z;
    } else {
        key->momentum.x = 0.0f;
        key->momentum.y = 0.0f;
        key->momentum.z = 0.0f;
    }
    DebrisEmitterOrientationMtx(effect->particle_keys[particle_key_slot], emitter_orientation);
    key->emission_epoch = emission_epoch;
    i = 0;
    while (emission_time <= end_time) {
        if (++i >= 100) {
            break;
        }
        if (position_delta != NULL) {
            NuVecAddScale(&key->emission_position, position, position_delta, emission_time - end_time);
        } else {
            key->emission_position.x = position->x;
            key->emission_position.y = position->y;
            key->emission_position.z = position->z;
        }
        key->emission_time = emission_time;
        generated = key->generator(key, effect, emission_time);
        if (effect->process_spheres != 0 && i == 1 && generated != NULL) {
            DebrisProcessSpheres(generated, emission_time, effect, key, 1);
        }
        emission_time = emission_interval + key->emission_epoch;
    }
    DebrisGetControlStackLock();
    // MATCH-EXCEPTION: the original tests the 16-bit counts once before entering the loop.
    if (key->controlled_chunk_count < key->allocated_chunk_count) {
        for (chunk_index = key->controlled_chunk_count; chunk_index < key->allocated_chunk_count; ++chunk_index) {
            if (freechunkcontrolsptr < (debrischunks + debrischunksglass) * 2) {
                freechunkcontrols[freechunkcontrolsptr]->particle_chunk = key->particle_chunks[chunk_index];
                freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                    now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
                freechunkcontrols[freechunkcontrolsptr]->active = 1;
                freechunkcontrols[freechunkcontrolsptr]->owner = key;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[panel_time]);
                ++freechunkcontrolsptr;
                ++key->controlled_chunk_count;
            }
        }
    }
    if (key->allocated_chunk_count == key->controlled_chunk_count) {
        last_chunk = key->particle_chunks[key->controlled_chunk_count - 1];
        stack = &debris_chunk_control_stack[panel_time];
        for (control = *stack; control != NULL; control = control->next) {
            if (control->particle_chunk == last_chunk) {
                RemoveChunkControlFromStack(control, stack);
                control->expiry_time = now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
                AddChunkControlToStack(control, stack);
                break;
            }
        }
    }
    key->previous_particle_count = key->particle_count;
    key->previous_allocated_chunk_count = key->allocated_chunk_count;
    debris_control_stack_lock = 0;
}

void AddVariableShotDebrisEffectMtx4(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, NUMTX *emitter_orientation,
                                     NUMTX *particle_orientation, u16 priority, i8 flags) {
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment, emitter_orientation,
                                      particle_orientation, priority, flags);
}

void AddVariableShotDebrisEffectMtx3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, NUMTX *emitter_orientation,
                                     NUMTX *particle_orientation) {
    u16 priority;
    if (effect < 0 || debtab[effect] == NULL) {
        return;
    }
    switch (debtab[effect]->particle_type) {
        case DEBRIS_PARTICLE_MODULATIVE:
            priority = 40000;
            break;
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            priority = 30000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            priority = 10000;
            break;
        default:
            priority = 20000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment, emitter_orientation,
                                      particle_orientation, priority, 0);
}

void AddVariableShotDebrisEffectMtx(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation, NUMTX *particle_orientation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectMtx3(effect, position, &APIObjectZeroMomentumPC, count, &orientation, particle_orientation);
}

void AddVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation) {
    AddVariableShotDebrisEffectMtx(effect, position, count, z_rotation, y_rotation, NULL);
}

i32 AddScaledVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation, i16 y_rotation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, NULL);
    }
    return scaled;
}

void DebrisDrawCalculateClipBoxes(DEBRIS_INFO *effect, DEBRISKEY *key) {
    NUMTX matrix;
    f32 padding;
    f32 radius;
    NUVEC extent;
    NUVEC minimum;
    DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
    NuMtxTranslate(&matrix, &key->position);
    extent.x = (DebrisFabs(effect->emitter_velocity.x) * (effect->emission_period_random + effect->emission_period) +
                effect->momentum_random_x * effect->particle_lifetime) +
               effect->emitter_random_extent.x;
    extent.y = (DebrisFabs(effect->emitter_velocity.y) * (effect->emission_period_random + effect->emission_period) +
                (DebrisFabs(effect->momentum_base) + effect->rotor_speed_x) * effect->particle_lifetime) +
               effect->emitter_random_extent.y;
    extent.z = (DebrisFabs(effect->emitter_velocity.z) * (effect->emission_period_random + effect->emission_period) +
                effect->rotor_speed_y * effect->particle_lifetime) +
               effect->emitter_random_extent.z;
    NuVecMtxRotate(&extent, &extent, &key->emitter_orientation);
    if (extent.x < 0.0f) {
        extent.x = -extent.x;
    }
    if (extent.y < 0.0f) {
        extent.y = -extent.y;
    }
    if (extent.z < 0.0f) {
        extent.z = -extent.z;
    }
    padding = (f32)((f64)0.0001f * effect->max_size);
    /* Rounds after each padding pass. */
    extent.x += padding;
    extent.y += padding;
    extent.z += padding;
    extent.x = (f32)(extent.x + (f64)0.2f);
    extent.y = (f32)(extent.y + (f64)0.2f);
    extent.z = (f32)(extent.z + (f64)0.2f);
    minimum.x = -extent.x;
    minimum.y = -extent.y;
    minimum.z = -extent.z;
    if (effect->gravity > 0.0f) {
        extent.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    } else if (effect->gravity < 0.0f) {
        minimum.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    }
    radius = MAX(MAX(extent.x, extent.z), MAX(extent.y, -minimum.y));
    key->clip_radius = radius;
    key->clip_min = minimum;
    key->clip_max = extent;
    DebrisMtxCopyPrivate(&key->clip_matrix, &matrix);
}

__declspec(align(16)) NUMTX xzfacingmtx = {0};
i32 g_renderingDebris = 0;
i32 edbits_editmode = 0;

void DebrisDraw(i32 unused, i32 pass) {
    DEBRIS_RENDER_CHUNK *chunk;
    i32 drawn = 0;
    if (debris_suspended != 0) {
        return;
    }
    _NuTimeBarSlotBegin(0, 12, "deb");
    DebMat[7]->texture_id = (i16)debris_rt;
    for (chunk = ParticleChunkRenderStack[pass]; chunk != NULL; chunk = chunk->next) {
        DEBRIS_INFO *effect;
        DEBRISKEY *key;
        NUMTX matrix;
        NUVEC position;
        f32 render_time;
        if (chunk->particle_chunk == NULL) {
            continue;
        }
        effect = chunk->effect;
        key = chunk->key;
        if (DebrisCutSceneMode != 0 && effect->cutscene_only != 0) {
            continue;
        }
        if (DebrisSuspendDrawObjectSwitch != -1 && key != NULL && key->trigger_second == DebrisSuspendDrawObjectSwitch) {
            continue;
        }
        if (effect->native_data == NULL) {
            if (freeDmaDebType == EDPP_MAX_DMADEBTYPES) {
                DebrisFreeOldestDmaDebTypeTable();
            }
            GenericDebinfoDmaTypeUpdate(effect);
            if (effect->native_data == NULL) {
                continue;
            }
        }
        if (key != NULL) {
            i32 assigned = 0;
            i32 visible = 1;
            i32 i;
            for (i = 0; i < 8; ++i) {
                if (effect->particle_keys[i] != -1 && &debkeydata[effect->particle_keys[i]] == key) {
                    assigned = 1;
                    break;
                }
            }
            if (!assigned && effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                key->cutoff_distance > effect->sound_range) {
                visible = 0;
            }
            if (key->draw_enabled == 0) {
                visible = 0;
            }
            if (pass != 4 && visible && effect->generator_type == DEBRIS_GENERATOR_NORMAL && effect->use_explicit_clip_box == 0 &&
                !assigned) {
                NUVEC minimum;
                NUVEC maximum;
                if (key->clip_radius == 0.0f || *edbits_editor_enabled != 0) {
                    DebrisDrawCalculateClipBoxes(effect, key);
                }
                if (edbits_editmode == 0 && key->gscene != NULL && key->portal_room != -1) {
                    if (NuPortalClipTest(key->gscene, &key->position, key->clip_radius, key->portal_room) == 0) {
                        visible = 0;
                    }
                }
                minimum = key->clip_min;
                maximum = key->clip_max;
                if (NuCameraClipTestExtents(&minimum, &maximum, &key->clip_matrix, 0.0f, 0) == 0) {
                    continue;
                }
            }
            if (!visible) {
                continue;
            }
            DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
            position.x = key->position.x;
            position.y = key->position.y;
            position.z = key->position.z;
        } else {
            if (effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                CameraEmitterDistance(&chunk->position) > effect->sound_range) {
                continue;
            }
            matrix = chunk->effect_orientation;
            position.x = chunk->position.x;
            position.y = chunk->position.y;
            position.z = chunk->position.z;
        }
        drawn = 1;
        NuMtxTranslate(&matrix, &position);
        if (pass == 4) {
            NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
            render_time = debris_render_panel_time;
            effect->last_render_time = panelglobaltime;
        } else {
            render_time = debris_render_global_time;
            effect->last_render_time = globaltime;
        }
        if (key != NULL && key->custom_facing != 0) {
            NuRndrSetParticleRotation(&key->particle_orientation);
        } else if (effect->camera_facing != 0) {
            NuRndrSetParticleRotation(&xzfacingmtx);
        } else {
            NuRndrSetParticleRotation(NULL);
        }
        NuMtxPreScaleX(&matrix, debris_render_scale);
        if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 4, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->camera_cutoff_radius);
        } else if (effect->use_explicit_clip_box != 0) {
            NuRndrSetDebBox(&effect->particle_box);
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 6, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->camera_cutoff_radius);
        } else {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 0, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->camera_cutoff_radius);
        }
    }
    if (drawn != 0) {
        g_renderingDebris = g_renderingDebris == 0;
    }
    _NuTimeBarSlotEnd(0, 12);
}

void AddVariableShotDebrisEffectTimed3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation) {
    u16 priority;
    if (effect < 0 || debtab[effect] == NULL) {
        return;
    }
    switch (debtab[effect]->particle_type) {
        case DEBRIS_PARTICLE_MODULATIVE:
            priority = 40000;
            break;
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            priority = 30000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            priority = 10000;
            break;
        default:
            priority = 20000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count, time, emitter_orientation, particle_orientation, priority,
                                      0);
}

void AddVariableShotDebrisEffectTimed1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation, i16 y_rotation,
                                       NUMTX *particle_orientation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectTimed3(effect, position, &APIObjectZeroMomentumPC, count, time, &orientation, particle_orientation);
}

void DebrisSetFacing(i32 handle, u8 enabled, i16 x_angle, i16 y_angle) {
    if (handle != -1) {
        debkeydata[handle].custom_facing = enabled;
        DebrisMtxCopyPrivate(&debkeydata[handle].particle_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
        DebrisMtxRotateXPrivate(&debkeydata[handle].particle_orientation, x_angle);
        DebrisMtxRotateYPrivate(&debkeydata[handle].particle_orientation, y_angle);
    }
}

void DebrisSetRoomID(i32 handle, NUGSCN *scene) {
    if (scene != NULL) {
        debkeydata[handle].portal_room = NuPortalWhichRoom(scene, &debkeydata[handle].position);
        debkeydata[handle].gscene = debkeydata[handle].portal_room != -1 ? scene : NULL;
    } else {
        debkeydata[handle].portal_room = -1;
        debkeydata[handle].gscene = NULL;
    }
}

void DebrisSetDetailLevels(i32 handle, i32 detail_levels) {
    debkeydata[handle].orientation_flags = (u8)detail_levels;
}

void AddDebrisEffectToStack(DEBRISKEY *key) {
    if (debris_keystack != NULL) {
        debris_keystack->next = key;
    }
    key->previous = debris_keystack;
    debris_keystack = key;
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) void DebrisOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].effect_orientation = *source;
        debkeydata[handle].effect_orientation.m30 = 0.0f;
        debkeydata[handle].effect_orientation.m31 = 0.0f;
        debkeydata[handle].effect_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void DebrisPosOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].effect_orientation = *source;
        debkeydata[handle].position.x = source->m30;
        debkeydata[handle].position.y = source->m31;
        debkeydata[handle].position.z = source->m32;
        debkeydata[handle].effect_orientation.m30 = 0.0f;
        debkeydata[handle].effect_orientation.m31 = 0.0f;
        debkeydata[handle].effect_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

i32 AddScaledVariableShotDebrisEffect2(i32 effect, NUVEC *position, i32 count, f32 time, NUMTX *emitter, NUMTX *particle, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed3(scaled, position, &APIObjectZeroMomentumPC, count, time, emitter, particle);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed3(scaled, position, momentum, count, time, emitter_orientation, particle_orientation);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect4(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, i32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation, u16 priority, i8 flags, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed5(scaled, position, momentum, NULL, count, time, emitter_orientation, particle_orientation,
                                          priority, flags);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect5(i32 effect, NUVEC *position, NUVEC *momentum, NUVEC *position_delta, i32 count, f32 time,
                                       NUMTX *emitter_orientation, NUMTX *particle_orientation, u16 priority, i8 flags, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed5(scaled, position, momentum, position_delta, count, time, emitter_orientation,
                                          particle_orientation, priority, flags);
    }
    return scaled;
}

NUMTL *CreateCopyMat(NUMTL *source, i32 enable_uv_mode, i32 alpha_mode, i32 depth_mode, i32 filter_mode) {
    NUMTL *material;
    NUMTL *original = source;
    if (source == NULL) {
        return NULL;
    }
    material = NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = 0.999f;
    material->attribs.cull_mode = NUMTL_CULL_NONE;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.filter_mode = (u32)filter_mode & 3;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;
    if (enable_uv_mode != 0) {
        material->disable_depth_write = 1;
    }
    material->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    material->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    material->texture_id = original->texture_id;
    material->sort_priority = original->sort_priority;
    material->alpha_fail = 1;
    NuMtlUpdate(material);
    return material;
}

i32 DebrisArenaInitialized = 0;
i16 *freedebkeys = 0;
DEBRIS_PART_HEADER **DmaDebTypes = 0;

void DebrisSetup2(VARIPTR *buffer, VARIPTR buffer_end, VARIPTR *trash_buffer, VARIPTR trash_end, char *texture_name, i32 chunk_count,
                  i32 glass_chunk_count, i32 key_count, i32 effect_count) {
    i32 i;
    i32 texture_id;
    if (debris_trash_space == 0) {
        debrischunks = chunk_count;
        debrischunksglass = glass_chunk_count;
    }
    debris_setup_called = 1;
    maxdebkeys = key_count;
    debinf_count = effect_count;
    DebrisArenaInitialized = 1;
    NuRegisterEndFrameCallBackFn(DebrisProcessTimeSlip);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debris_chunk_controls = (DEBRIS_CHUNK_CONTROL *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL);
    freechunkcontrols = (DEBRIS_CHUNK_CONTROL **)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL *);
    freedebchunks = (DEBRIS_PARTICLE_CHUNK **)buffer->addr;
    buffer->addr += debrischunks * sizeof(DEBRIS_PARTICLE_CHUNK *);
    freedebchunksglass = (DEBRIS_PARTICLE_CHUNK **)buffer->addr;
    buffer->addr += debrischunksglass * sizeof(DEBRIS_PARTICLE_CHUNK *);
    ParticleChunkToRender = (DEBRIS_RENDER_CHUNK *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * sizeof(DEBRIS_RENDER_CHUNK);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debkeydata = (DEBRISKEY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRISKEY);
    freedebkeys = (i16 *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(i16);
    effecttypes = (DEBRIS_INFO *)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO);
    debtab = (DEBRIS_INFO **)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO *);
    DmaDebTypes = (DEBRIS_PART_HEADER **)buffer->addr;
    buffer->addr += EDPP_MAX_DMADEBTYPES * sizeof(DEBRIS_PART_HEADER *);
    spherecolldata = (DEBRIS_COLLISION_ENTRY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY);
    toruscolldata = (DEBRIS_COLLISION_ENTRY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY);
    debscale = (DEBRIS_SCALE_CACHE *)buffer->addr;
    buffer->addr += EDPP_SCALE_TYPES * sizeof(DEBRIS_SCALE_CACHE);
    memset(freedebkeys, 0, maxdebkeys * sizeof(i16));
    memset(debkeydata, 0, maxdebkeys * sizeof(DEBRISKEY));
    memset(effecttypes, 0, debinf_count * sizeof(DEBRIS_INFO));
    memset(debtab, 0, debinf_count * sizeof(DEBRIS_INFO *));
    memset(debscale, 0, EDPP_SCALE_TYPES * sizeof(u32));
    for (i = 0; i < maxdebkeys; ++i) {
        spherecolldata[i].key_index = i;
        spherecolldata[i].flags = 3;
        toruscolldata[i].key_index = i;
        toruscolldata[i].flags = 3;
    }
    effecttypes[0] = DebrisNullEffect;
    debtab[0] = effecttypes;
    edpp_types_used = 1;
    numspherecolldata = maxdebkeys;
    numtoruscolldata = maxdebkeys;
    DebMat[0] = CreateAlphaBlendTexture(buffer, buffer_end, texture_name, 1, 2, 256, 0);
    texture_id = DebMat[0]->texture_id;
    DebMat[0]->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
    DebMat[0]->attribs.alpha_test = 1;
    NuMtlUpdate(DebMat[0]);
    DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
    DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
    DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
    DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
    DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
    DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
    DebMat[0]->shader.vertex_format |= 0x100000;
    DebMat[1]->shader.vertex_format |= 0x100000;
    DebMat[2]->shader.vertex_format |= 0x100000;
    DebMat[3]->shader.vertex_format |= 0x100000;
    DebMat[4]->shader.vertex_format |= 0x100000;
    DebMat[5]->shader.vertex_format |= 0x100000;
    DebMat[6]->shader.vertex_format |= 0x100000;
    DebMat[6]->texture_id = texture_id;
    DebMat[5]->texture_id = texture_id;
    DebMat[4]->texture_id = texture_id;
    DebMat[3]->texture_id = texture_id;
    DebMat[2]->texture_id = texture_id;
    DebMat[1]->texture_id = texture_id;
    DebMat[0]->texture_id = texture_id;
    DebMat[7] = NuMtlCreateEx3D(1, 2);
    DebMat[7]->diffuse.x = 1.0f;
    DebMat[7]->diffuse.y = 1.0f;
    DebMat[7]->diffuse.z = 1.0f;
    DebMat[7]->attribs.cull_mode = NUMTL_CULL_NONE;
    DebMat[7]->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
    DebMat[7]->attribs.filter_mode = 0;
    DebMat[7]->attribs.lighting = NUMTL_LIGHTING_NONE;
    DebMat[7]->attribs.vertex_colour = 1;
    DebMat[7]->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    DebMat[7]->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    DebMat[7]->opacity = 0.999f;
    DebMat[7]->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
    DebMat[7]->alpha_fail = 1;
    DebMat[7]->texture_id = 1;
    DebMat[7]->sort_priority = 128;
    DebMat[7]->shader.vertex_format |= 0x100000;
    DebMat[7]->transform_scale = 0.25f;
    DebMat[7]->material_kind = 0x97;
    NuMtlUpdate(DebMat[7]);
    DebMat[8] = CreateCopyMat(DebMat[0], 0, 6, 0, 1);
    for (i = 0; debtab[i] != NULL; ++i) {
        if (debtab[i]->frame_divisor == 0) {
            debtab[i]->frame_divisor = 1;
        }
        debtab[i]->animation_rate.rate = (f32)((debtab[i]->texture_frames * 60) / debtab[i]->frame_divisor);
        debtab[i]->animation_rate.packed_rate = (debtab[i]->animation_rate.packed_rate & 0xfffff800) +
                                                (((u16)debtab[i]->texture_frames * (debtab[i]->texture_rows - 1)) & 0x3ff);
        debtab[i]->alpha_keys[0].value /= debtab[i]->particle_lifetime;
        debtab[i]->texture_u0 += 524288.0;
        debtab[i]->texture_u1 += 524288.0;
        debtab[i]->texture_v0 += 524288.0;
        debtab[i]->texture_v1 += 524288.0;
    }
    for (i = 0; i < debinf_count; ++i) {
        effecttypes[i].native_data = NULL;
    }
    freeDmaDebType = 0;
    if (debris_suspended == 0) {
        if (trash_buffer != NULL) {
            DebrisTrashableSetup(trash_buffer, &trash_end);
        } else {
            DebrisTrashableSetup(buffer, &buffer_end);
        }
    }
    for (i = 0; i < maxdebkeys; ++i) {
        freedebkeys[i] = (i16)i;
    }
    for (i = 0; i < maxdebkeys; ++i) {
        debkeydata[i].finite = 0;
        debkeydata[i].particle_cursor = 0;
        debkeydata[i].emitter_rotation_x = 0;
        debkeydata[i].emitter_rotation_y = 0;
        debkeydata[i].finite_count = 0;
        debkeydata[i].effect_index = 0;
        debkeydata[i].allocation_index = -1;
    }
    xzfacingmtx.m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    xzfacingmtx.m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    xzfacingmtx.m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    xzfacingmtx.m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    xzfacingmtx.m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    xzfacingmtx.m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    xzfacingmtx.m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    xzfacingmtx.m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    xzfacingmtx.m20 = ANI3_EVAL_IDENTITY_MATRIX.m20;
    xzfacingmtx.m21 = ANI3_EVAL_IDENTITY_MATRIX.m21;
    xzfacingmtx.m22 = ANI3_EVAL_IDENTITY_MATRIX.m22;
    xzfacingmtx.m23 = ANI3_EVAL_IDENTITY_MATRIX.m23;
    xzfacingmtx.m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    xzfacingmtx.m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    xzfacingmtx.m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    xzfacingmtx.m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    DebrisMtxRotateXPrivate(&xzfacingmtx, -0x4000);
}

void fun_0064fd90(void) {
}

void fun_0064fda0(void) {
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) i32 AddScaledVariableShotDebrisEffect1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation,
                                                            i16 y_rotation, NUMTX *orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, orientation);
    }
    return scaled;
}

void DebrisSetup(VARIPTR *buffer, VARIPTR buffer_end, char *texture_name, i32 chunk_count, i32 key_count, i32 effect_count) {
    DebrisSetup2(buffer, buffer_end, buffer, buffer_end, texture_name, chunk_count, 0x20, key_count, effect_count);
}
