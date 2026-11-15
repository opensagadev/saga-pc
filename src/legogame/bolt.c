#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#include <math.h>

typedef struct VEHICLE_FIRE_STATE { // size 0xe3c
    u8 unknown_000[0x7f4];          // 0x000
    WEAPON_AMMO weapon_ammo;        // 0x7f4
    u8 unknown_7fc[0xe3a - 0x7fc];  // 0x7fc
    u8 bolt_fire_phase;             // 0xe3a
} VEHICLE_FIRE_STATE;

i16 dat_007f190c = -1;
i16 dat_007f1910 = -1;
i16 id_TIEBOMBER = -1;
i16 dat_007f196c = -1;
i16 id_XWING = -1;
i16 dat_007f1a24 = -1;
i16 dat_007f1a28 = -1;
i16 dat_007f1a2c = -1;
i16 id_SENTRYDROID = -1;
i16 dat_007f1a38 = -1;
i16 id_MINIDROIDEKA = -1;
i16 dat_007f1ad8 = -1;
i16 id_MINITIEBOMBER = -1;
i16 id_MINISTARDESTROYER = -1;
i16 id_MINIATAT = -1;
i16 dat_007f1aec = -1;
i16 dat_007f1af0 = -1;
i16 dat_007f1b00 = -1;
i16 dat_007f1b04 = -1;
i16 dat_007f1b08 = -1;
i16 dat_007f1b0c = -1;
i16 dat_007f1b34 = -1;
i16 dat_007f1b38 = -1;

i32 BoltType_FindIDByCreature(GAMEOBJECT *object, i32 fallback) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    VEHICLE_FIRE_STATE *state = (VEHICLE_FIRE_STATE *)object;
    i32 id;
    i32 result;
    if (object == NULL) {
        return fallback;
    }
    if (StartupGameFlag0 != 0 && state->weapon_ammo.type != NULL && state->weapon_ammo.type->bolt_override != -1) {
        return state->weapon_ammo.type->bolt_override;
    }
    id = (i16)object->id;
    if (id == VehicleSpecialShooterId) {
        return 43;
    }
    if (id == dat_007f19a4) {
        result = 7 + 2 * (state->bolt_fire_phase != 0);
        if (result == 7 && world->current_level == ASTEROIDCHASED_LDATA) {
            return 8;
        }
        return result;
    }
    if (id == dat_007f1a9c || id == dat_007f1aa0) {
        return 34;
    }
    if (id == dat_007f1b34 || id == dat_007f1b38 || id == id_ZAMSSPEEDER) {
        return 11;
    }
    if (id == id_FLASHSPEEDER) {
        return 20;
    }
    if (id == id_XWING || id == dat_007f195c) {
        return 6;
    }
    if (id == id_SPEEDERBIKE || id == dat_007f19a8) {
        return 10;
    }
    if (id == dat_007f18b4 || id == dat_007f1a24) {
        return 11;
    }
    if (id == id_TIEFIGHTER || id == dat_007f1a28 || id == dat_007f196c || id == id_TIEBOMBER || id == dat_007f1a2c) {
        if (world != NULL) {
            if (world->current_level == DEATHSTARBATTLED_LDATA) {
                return 13;
            }
            if (world->current_level == DOGFIGHTA_LDATA) {
                return 14;
            }
        }
        return 12;
    }
    if (id == dat_007f1b00 || id == dat_007f1ad8 || id == dat_007f1b04 || id == id_MINITIEBOMBER || id == dat_007f1aec ||
        id == dat_007f1b08 || id == dat_007f1b0c || id == id_MINIATAT || id == id_MINISTARDESTROYER) {
        return 23;
    }
    if (id == dat_007f1af0) {
        return 21 + (state->bolt_fire_phase != 0);
    }
    if (id == id_JEDISTARFIGHTERYELLOWEP3 || id == dat_007f19e8) {
        if (WORLD->area != NULL && WORLD->area == BATTLEOVERCORUSCANT_ADATA && WORLD->level_objects[0x12d].active != 0) {
            return 32;
        }
        return 33;
    }
    if (id == dat_007f17d0 || id == dat_007f17d4 || id == dat_007f1aac) {
        result = 42;
        if (WORLD->level_objects[GlobalBoltType[42].object_ids[0]].active == 0) {
            result = 0;
        }
        return result;
    }
    if (id == id_CATAPULT) {
        return 29;
    }
    if (object->character_data->move_fn == Move_CANNON) {
        return 20;
    }
    if (GCDataList[id].weapon_action == 8) {
        return 2;
    }
    if (GCDataList[id].weapon_model == 22) {
        return 2;
    }
    if (id == id_BIGGUN) {
        return 20;
    }
    if (id == id_BOBAFETT || id == dat_007f190c) {
        return 26;
    }
    if (id == dat_007f1910) {
        return 3;
    }
    if (id == id_ATAT) {
        return 24;
    }
    if (id == (i16)GameObjectRotationAtstId || id == id_CLONEWALKER) {
        return 4;
    }
    if (id == id_ATST_LOWRES) {
        return 25;
    }
    if (id == id_DROIDEKA || id == id_MINIDROIDEKA) {
        return 17;
    }
    if (id == id_STAP || id == id_STAP2) {
        return 0;
    }
    if (id == dat_007f1924) {
        return 27 + (Cheat_IsOn(0x29) != 0 ? 3 : 0);
    }
    if (id == dat_007f1928) {
        return 28 + (Cheat_IsOn(0x29) != 0 ? 3 : 0);
    }
    if (id == id_REPUBLICGUNSHIP || id == id_REPUBLICGUNSHIP_GREEN) {
        if (WORLD->current_level == BONUS_GUNSHIPA_LDATA) {
            return 35;
        }
        if (WORLD->current_level == BONUS_GUNSHIPB_LDATA) {
            return 36;
        }
        return fallback;
    }
    if (id == id_NEW_REPUBLIC_GUNSHIP || id == id_NEW_REPUBLIC_GUNSHIP_GREEN) {
        if (WORLD->current_level == GUNSHIPA_LDATA) {
            return 37;
        }
        if (WORLD->current_level == GUNSHIPB_LDATA) {
            return 38;
        }
        return fallback;
    }
    if (id == id_SENTRYDROID) {
        return 0;
    }
    if (id == dat_007f1a38) {
        return 0;
    }
    return fallback;
}

void Bolt_Debris_LSW(BOLT *bolt, NUVEC *points, i32 point, NUVEC *unused, i32 no_explosion) {
    i32 i;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 effect;
    if (world->current_level == DOGFIGHTA_LDATA) {
        effect = 0x2e;
        if (bolt->unknown_104 != -1) {
            points = &bolt->dogfight_hit_position;
        }
    } else {
        effect = bolt->hit_debris;
    }
    for (i = 0; i < 3; ++i) {
        if ((bolt->flags & 0x40000200) != 0 && point == -1) {
            if (i != 1) {
                continue;
            }
        } else if (point != -1 && point != i) {
            continue;
        }
        AddGameDebris(world->game_debris_sys, effect, &points[i]);
        if (!(bolt->flags & BOLT_DEFLECTED) && Player_HasDoubleBoltDamage_FromBolt(bolt)) {
            AddGameDebris(world->game_debris_sys, effect, &points[i]);
        }
    }
    if (bolt->hit_part_debris != -1) {
        AddPartDebris(world->part_debris_sys, bolt->hit_part_debris, &bolt->position);
    }
    if (bolt->type_id == 0x1d) {
        AddGameDebris(world->game_debris_sys, 0x7f, &bolt->position);
        AddPartDebris(world->part_debris_sys, 6, &bolt->position);
        GameCam_NewShake(GameCam, 0.75f, 0.75f, 1.0f);
        NewRumbleAllPlayers(0.5f, 0.0f, 0, 0);
    }
    if (no_explosion == 0 && bolt->owner != NULL && (bolt->owner->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Cheat_IsOn(0x12) &&
        (world->area == NULL || !(world->area->flags & AREAFLAG_VEHICLE_AREA))) {
        EXPLOSION *explosion;
        AddGameDebris(WORLD->game_debris_sys, 0x49, &bolt->position);
        NewRumble(bolt->owner->pad->bound_pad, 0.3f, 0);
        PlaySfx("exp_thermalDet", points);
        GameCam_HitJudder();
        explosion = AddExplosion(&bolt->position, (f32)(AreaPickupScale * 0.5), 0.25f, NULL, -1,
                                 EXPLOSION_HITS_CHARACTERS | EXPLOSION_HITS_BLOWUPS | EXPLOSION_HITS_PARTS | EXPLOSION_KILLS |
                                     EXPLOSION_SHAKE | EXPLOSION_HITS_FLICKERING | EXPLOSION_BLOWUP_EXPLOSION);
        if (explosion != NULL && Arcade != 0) {
            GAMEOBJECT *owner = bolt->owner;
            if (owner != NULL && (owner == Player[0] || owner == Player[1]) && (owner->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                (owner->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && (owner->unknown_280 == 0 || owner->unknown_280 == 1)) {
                explosion->flags |= 0x10000;
                explosion->owner = bolt->owner;
            }
        }
    }
}

i32 Bolt_HitPart_LSW(BOLT *bolt, PART *part) {
    if (part->unknown_206 == 1 && part->scale_time < 2.0f) {
        return 1;
    }
    if (part->unknown_206 == 2 && part->scale_time < 1.0f) {
        return 1;
    }
    return 0;
}

i16 id_MINIXWING = -1;
i16 id_MINIROYALSTARSHIP = -1;

i32 Bolt_AlternateFire_LSW(GAMEOBJECT *object, i32 index) {
    i16 id = object->id;
    if (id == id_XWING || id == id_MINIXWING || id == dat_007f1ad8 || id == id_MINIATAT || id == id_MINIROYALSTARSHIP ||
        id == dat_007f1aec) {
        if (object->bolt_fire_phase == 0 && (index == 0 || index == 2)) {
            return 0;
        }
        if (object->bolt_fire_phase == 1 && (index == 1 || index == 3)) {
            return 0;
        }
    } else if (id == dat_007f19a4 || id == dat_007f1af0) {
        if (index != object->bolt_fire_phase) {
            return 0;
        }
    } else if (id == (i16)GameObjectRotationAtstId || id == dat_007f19e8 || id == id_JEDISTARFIGHTERYELLOWEP3) {
        if (index != object->bolt_fire_phase) {
            return 0;
        }
    }
    return 1;
}

void EndBolt_EwokTorpedo(BOLT *bolt) {
    Detonate(&bolt->position, 0);
}

void UpdateBolt_Geonosian(BOLT *bolt) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 effect = world->game_debris_sys->entries[85].effect;
    if (effect != -1) {
        AddVariableShotDebrisEffectTimed1(effect, &bolt->position, 60, FRAMETIME, 0, 0, NULL);
    }
}

f32 fun_00408760(f32 value) {
    return ceilf(value);
}

f32 fun_00408780(f32 value) {
    return ceilf(value);
}
