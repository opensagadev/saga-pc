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

/* The SDK NuFabs; this unit keeps its own out-of-line copy. */
void fun_0064fd90(void) {
}

void fun_0064fda0(void) {
}
