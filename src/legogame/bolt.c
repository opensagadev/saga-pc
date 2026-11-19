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

i16 id_FOURLOM = -1;
i16 id_TRAININGREMOTE = -1;
i16 id_TIEBOMBER = -1;
i16 id_TIEFIGHTERDARTH = -1;
i16 id_XWING = -1;
i16 id_SNOWSPEEDER = -1;
i16 id_TIEINTERCEPTOR = -1;
i16 id_IMPERIALSHUTTLE = -1;
i16 id_SENTRYDROID = -1;
i16 id_KAMINOANDROID = -1;
i16 id_MINIDROIDEKA = -1;
i16 id_MINITIEINTERCEPTOR = -1;
i16 id_MINITIEBOMBER = -1;
i16 id_MINISTARDESTROYER = -1;
i16 id_MINIATAT = -1;
i16 id_MINIIMPERIALSHUTTLE = -1;
i16 id_MINIMILLENNIUMFALCON = -1;
i16 id_MINITIEFIGHTER = -1;
i16 id_MINITIEADVANCED = -1;
i16 id_MINISITHINFILTRATOR = -1;
i16 id_MINISOLARSAILOR = -1;
i16 id_ANAKINSSPEEDER = -1;
i16 id_ANAKINSSPEEDERGREEN = -1;

i32 BoltType_FindIDByCreature(GAMEOBJECT *object, i32 fallback) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 id;
    i32 result;
    if (object == NULL) {
        return fallback;
    }
    if (StartupGameFlag0 != 0 && object->player.weapon.type != NULL && object->player.weapon.type->bolt_override != -1) {
        return object->player.weapon.type->bolt_override;
    }
    id = (i16)object->id;
    if (id == VehicleSpecialShooterId) {
        return 43;
    }
    if (id == id_MILLENNIUMFALCON) {
        result = 7 + 2 * (object->player.bolt_fire_phase != 0);
        if (result == 7 && world->current_level == ASTEROIDCHASED_LDATA) {
            return 8;
        }
        return result;
    }
    if (id == id_NABOOSTARFIGHTER || id == id_NABOOSTARFIGHTERLIME) {
        return 34;
    }
    if (id == id_ANAKINSSPEEDER || id == id_ANAKINSSPEEDERGREEN || id == id_ZAMSSPEEDER) {
        return 11;
    }
    if (id == id_FLASHSPEEDER) {
        return 20;
    }
    if (id == id_XWING || id == id_SLAVE1) {
        return 6;
    }
    if (id == id_SPEEDERBIKE || id == id_SPEEDERBIKESNOW) {
        return 10;
    }
    if (id == id_PROBEDROID || id == id_SNOWSPEEDER) {
        return 11;
    }
    if (id == id_TIEFIGHTER || id == id_TIEINTERCEPTOR || id == id_TIEFIGHTERDARTH || id == id_TIEBOMBER || id == id_IMPERIALSHUTTLE) {
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
    if (id == id_MINITIEFIGHTER || id == id_MINITIEINTERCEPTOR || id == id_MINITIEADVANCED || id == id_MINITIEBOMBER ||
        id == id_MINIIMPERIALSHUTTLE || id == id_MINISITHINFILTRATOR || id == id_MINISOLARSAILOR || id == id_MINIATAT ||
        id == id_MINISTARDESTROYER) {
        return 23;
    }
    if (id == id_MINIMILLENNIUMFALCON) {
        return 21 + (object->player.bolt_fire_phase != 0);
    }
    if (id == id_JEDISTARFIGHTERYELLOWEP3 || id == id_JEDISTARFIGHTERREDEP3) {
        if (WORLD->area != NULL && WORLD->area == BATTLEOVERCORUSCANT_ADATA && WORLD->level_objects[0x12d].active != 0) {
            return 32;
        }
        return 33;
    }
    if (id == id_INDIANAJONES || id == id_HANSOLOINDY || id == id_SATIPO) {
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
    if (GCDataList[id].variant == CHARVARIANT_CLONE) {
        return 2;
    }
    if (GCDataList[id].weapon_model == 22) {
        return 2;
    }
    if (id == id_BIGGUN) {
        return 20;
    }
    if (id == id_BOBAFETT || id == id_FOURLOM) {
        return 26;
    }
    if (id == id_TRAININGREMOTE) {
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
    if (id == id_EWOK) {
        return 27 + (Cheat_IsOn(0x29) != 0 ? 3 : 0);
    }
    if (id == id_WICKET) {
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
    if (id == id_KAMINOANDROID) {
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
        if (bolt->impact_surface != -1) {
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
        NewRumble(bolt->owner->player.pad->bound_pad, 0.3f, 0);
        PlaySfx("exp_thermalDet", points);
        GameCam_HitJudder();
        explosion = AddExplosion(&bolt->position, (f32)(AreaPickupScale * 0.5), 0.25f, NULL, -1,
                                 EXPLOSION_HITS_CHARACTERS | EXPLOSION_HITS_BLOWUPS | EXPLOSION_HITS_PARTS | EXPLOSION_KILLS |
                                     EXPLOSION_SHAKE | EXPLOSION_HITS_FLICKERING | EXPLOSION_BLOWUP_EXPLOSION);
        if (explosion != NULL && Arcade != 0) {
            GAMEOBJECT *owner = bolt->owner;
            if (owner != NULL && (owner == Player[0] || owner == Player[1]) && (owner->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                (owner->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && (owner->player_slot == 0 || owner->player_slot == 1)) {
                explosion->flags |= 0x10000;
                explosion->owner = bolt->owner;
            }
        }
    }
}

i32 Bolt_HitPart_LSW(BOLT *bolt, PART *part) {
    if (part->player_mask == 1 && part->scale_time < 2.0f) {
        return 1;
    }
    if (part->player_mask == 2 && part->scale_time < 1.0f) {
        return 1;
    }
    return 0;
}

i16 id_MINIXWING = -1;
i16 id_MINIROYALSTARSHIP = -1;

i32 Bolt_AlternateFire_LSW(GAMEOBJECT *object, i32 index) {
    i16 id = object->id;
    if (id == id_XWING || id == id_MINIXWING || id == id_MINITIEINTERCEPTOR || id == id_MINIATAT || id == id_MINIROYALSTARSHIP ||
        id == id_MINIIMPERIALSHUTTLE) {
        if (object->player.bolt_fire_phase == 0 && (index == 0 || index == 2)) {
            return 0;
        }
        if (object->player.bolt_fire_phase == 1 && (index == 1 || index == 3)) {
            return 0;
        }
    } else if (id == id_MILLENNIUMFALCON || id == id_MINIMILLENNIUMFALCON) {
        if (index != object->player.bolt_fire_phase) {
            return 0;
        }
    } else if (id == (i16)GameObjectRotationAtstId || id == id_JEDISTARFIGHTERREDEP3 || id == id_JEDISTARFIGHTERYELLOWEP3) {
        if (index != object->player.bolt_fire_phase) {
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
