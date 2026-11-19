#include "globals.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/audio/specialsfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/gizmos/object/hatmachine.h"
#include "legoapi/items/fx/pulses.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/numusic/sfx.h"
#include <string.h>
#include <stdio.h>

AREA_DATA *DEATHSTARRESCUE_ADATA;
AREA_DATA *DEATHSTARRESCUE_ADATA = 0;
AREA_DATA *DEATHSTARESCAPE_ADATA = 0;
AREA_DATA *HOTHESCAPE_ADATA = 0;
AREA_DATA *NEWTOWN_ADATA = 0;
i32 ShadowMode = 0;

i32 GameAudio_OverrideFootStep_LSW(GAMEOBJECT *object, i32 on_platform) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    AREA_DATA *area = world->area;
    if ((area == HOTHESCAPE_ADATA || world->current_level == JABBASPALACEE_LDATA) && on_platform == 0) {
        return GetSfxId("fs_ice");
    }
    if (area == DAGOBAH_ADATA && on_platform == 0) {
        if (world->current_level != dat_0087b258 || object->jump_animation_state != 0x14) {
            if (world->current_level != dat_0087b264) {
                if (world->current_level != dat_0087b268 ||
                    (GameCam->sock_position.sock_index != 1 && GameCam->sock_position.sock_index != 4)) {
                    return GetSfxId("fs_swamp");
                }
            }
        }
    }
    if (world->current_level == JABBASPALACEA_LDATA && (object->jump_animation_state == 0x18 || object->jump_animation_state == 9)) {
        return GetSfxId("fs_ice");
    }
    if ((area == DEATHSTARRESCUE_ADATA || area == DEATHSTARESCAPE_ADATA || WORLD->area == JABBASPALACE_ADATA) &&
        object->jump_animation_state == 0x14) {
        return GetSfxId("FS_JWalkM");
    }
    return -1;
}

i32 GameAudio_CheckReverb_LSW(void) {
    LEVELDATA *level = WorldInfo_CurrentlyActive()->current_level;
    if (level == dat_0087b200) {
        return 1;
    }
    if (level == dat_0087b264) {
        return 1;
    }
    if (level == dat_0087b228 && GameCam->sock_position.sock_index == 4) {
        return 1;
    }
    return 0;
}

void SetLevelSfxBits(WORLDINFO *world) {
    i32 i;
    i32 j;
    i32 character_id;
    i32 portal;
    LEVELDATA *level;
    i16 model_id;
    CHARACTERDATA *character;
    PLAYERCHARACTERCONFIG *game_character;
    CHARACTER_EFFECT_CONFIG *effect;
    i32 double_score_sfx;
    i32 sfx_ids[1024];
    i32 sfx_count = 0;

    for (i = 0; i < world->level_sfx_count; ++i) {
        sfx_ids[sfx_count++] = world->level_sfx[i].id;
    }

    if (world->spinner_count > 0) {
        AddLevelSfxFromName("fly_paddle_rotate_lp", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("fly_paddle_stuck", sfx_ids, &sfx_count, 0x400);
    }
    if (world->teleport_count > 0) {
        AddLevelSfxFromName("env_door_flap", sfx_ids, &sfx_count, 0x400);
    }

    GameAudio_AddSfx(0x50, sfx_ids, &sfx_count, 0x400);
    GameAudio_AddSfx(0x51, sfx_ids, &sfx_count, 0x400);
    GameAudio_AddSfx(0x52, sfx_ids, &sfx_count, 0x400);
    AddLevelSfxFromName("Grv_GuardWeaponLp", sfx_ids, &sfx_count, 0x400);

    if (world->grabber != NULL) {
        if (world->current_level == CLOUDCITYTRAPA_LDATA) {
            AddLevelSfxFromName("CarbonFreezeCraneLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("CarbonFreezeCrane", sfx_ids, &sfx_count, 0x400);
        } else if (world->current_level == JABBASPALACEB_LDATA) {
            AddLevelSfxFromName("env_hover_box_lp", sfx_ids, &sfx_count, 0x400);
        } else {
            AddLevelSfxFromName("env_crane_mvt_lp", sfx_ids, &sfx_count, 0x400);
        }
        AddLevelSfxFromName("env_crane_in", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("env_grabber_down", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("env_grabber_pickup", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("env_grabber_up", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Explode2", sfx_ids, &sfx_count, 0x400);
        if (world->current_level == CLOUDCITYESCAPEC_LDATA) {
            AddLevelSfxFromName("imp_C3PO_magnet", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("imp_C3PO_magnet_drop", sfx_ids, &sfx_count, 0x400);
        }
    }

    if (world->cutscene_sys != NULL) {
        for (i = 0; i < world->cutscene_sys->count; ++i) {
            for (j = 0; j < 6; ++j) {
                CUTINFO *cut = world->cutscene_sys->records[i];
                i32 id = cut->sfx[j].id;
                if (id != -1) {
                    sfx_ids[sfx_count++] = id;
                }
            }
        }
    }

    Pulses_AddSfx(world->pulses_sys, sfx_ids, &sfx_count, 0x400);
    if (world->hat_machine_sys != NULL && world->hat_machine_sys->count > 0) {
        AddLevelSfxFromName("SwLever", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("HatOn", sfx_ids, &sfx_count, 0x400);
    }
    if (world->nlevers > 0) {
        AddLevelSfxFromName("SwLever", sfx_ids, &sfx_count, 0x400);
    }

    if (world->area == NULL || (world->area->flags & AREAFLAG_VEHICLE_AREA) == 0) {
        AddLevelSfxFromName("Char_Slide_Lp", sfx_ids, &sfx_count, 0x400);
        if (world->push_block_count > 0) {
            AddLevelSfxFromName("Block_Shove", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Block_Push_Lp", sfx_ids, &sfx_count, 0x400);
        }
        AddLevelSfxFromName("exp_thermalDet", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("imp_thermalDet_attach", sfx_ids, &sfx_count, 0x400);
        if (world->area == SPEEDERCHASE_ADATA) {
            AddLevelSfxFromName("XWing_LoopDeLoop", sfx_ids, &sfx_count, 0x400);
        }
    } else {
        AddLevelSfxFromName("XWing_Torpedo", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("imp_proton_torp", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("XWing_LoopDeLoop", sfx_ids, &sfx_count, 0x400);
    }
    AddLevelSfxFromName("Explode1", sfx_ids, &sfx_count, 0x400);

    level = world->current_level;
    if (level->sfx_ambient != -1) {
        sfx_ids[sfx_count++] = level->sfx_ambient;
    }
    if (level == STATUS_LDATA || (level->flags & LEVEL_STATUS) != 0) {
        AddLevelSfxFromName("StatusAward", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Status_GoldBarDec", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("TrueJedi_100pc", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("TrueJedi_NOT", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("MK-Panel", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Char_Icon_App", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Char_Icon_Slide", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("LegoClicks", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Shop_BuyCheat", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Explode1", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("Jp_Ana_Jump", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("PickupCoin", sfx_ids, &sfx_count, 0x400);
    } else if (world->area != NULL && (world->area->flags & AREAFLAG_BONUS_AREA) != 0) {
        AddLevelSfxFromName("Victory", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("exp_debris", sfx_ids, &sfx_count, 0x400);
    } else if (level == CREDITS_LDATA) {
        AddLevelSfxFromName("StatusAward", sfx_ids, &sfx_count, 0x400);
    }

    if (world->area != NULL) {
        if (world->area->super_counter_count != 0) {
            GameAudio_AddSfx(0x53, sfx_ids, &sfx_count, 0x400);
        }
        if (world->area == PODRACE_ADATA || world->area == PODSPRINT_ADATA) {
            AddLevelSfxFromName("PodX_TuskenBlast", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Pod_TuskHit", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngSebulba_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngAnakin_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngGeneric1_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngGeneric2_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngGeneric3_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngStartup1", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngStartup2", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Pod_Race_Go", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Pod_Race_Light", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_EngGeneric_Lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_Collide", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_Crash", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_Booster", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("CountdownTimerTick", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("CountdownTimerTock", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("PodX_PurpCrysHit", sfx_ids, &sfx_count, 0x400);
        } else if (world->area == GUNSHIP_ADATA || world->area == BONUS_GUNSHIP_ADATA) {
            AddLevelSfxFromName("GC_GunshipBlasterFire", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("GC_GunshipEngineLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("GC_GunshipDeath", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("GC_LaserBeam", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Explode1", sfx_ids, &sfx_count, 0x400);
        } else if (world->area == HUB_ADATA) {
            AddLevelSfxFromName("Shop_BuyCheat", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Shop_NotEnufMuny", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ui_ZoomIn", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ui_ZoomOut", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ui_hover_lp", sfx_ids, &sfx_count, 0x400);
        } else if (world->area == JABBASPALACE_ADATA) {
            AddLevelSfxFromName("swdisco", sfx_ids, &sfx_count, 0x400);
        } else if (world->area == NEWTOWN_ADATA) {
            AddLevelSfxFromName("BBounce", sfx_ids, &sfx_count, 0x400);
        }
    }

    level = world->current_level;
    {
        if (level == NEGOTIATIONSA_LDATA) {
            AddLevelSfxFromName("NegA_CoinMDoorUp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegA_CoinMDoorDn", sfx_ids, &sfx_count, 0x400);
        } else if (level == NEGOTIATIONSB_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegC_Crate", sfx_ids, &sfx_count, 0x400);
        } else if (level == NEGOTIATIONSC_LDATA) {
            AddLevelSfxFromName("NegC_MagnetCoil", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegC_MagnetCoilH", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegC_MagnetCoilS", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegC_Crate", sfx_ids, &sfx_count, 0x400);
        } else if (level == GUNGAN_A_LDATA) {
            AddLevelSfxFromName("GunA_TreeFall", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FS_CreaRun", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("IkopiGrowl", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("KaaduBark", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("KaaduGrowl", sfx_ids, &sfx_count, 0x400);
        } else if (level == RESCUEC_LDATA) {
            AddLevelSfxFromName("ResX_MarbColLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ResX_PoleLift_Rise", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ResX_PoleLift_Lower", sfx_ids, &sfx_count, 0x400);
        } else if (level == RETAKEB_LDATA) {
            AddLevelSfxFromName("ResX_MarbColLp", sfx_ids, &sfx_count, 0x400);
        } else if (level == RETAKED_LDATA) {
            AddLevelSfxFromName("ResX_PortCulLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ResX_PortCulEnd", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep1_5_OutdoorPlatform", sfx_ids, &sfx_count, 0x400);
        } else if (level == RETAKEE_LDATA) {
            AddLevelSfxFromName("Ep1_5_OutdoorPlatform", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ResX_PortCulLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("ResX_PortCulEnd", sfx_ids, &sfx_count, 0x400);
        } else if (level == MAULA_LDATA) {
            AddLevelSfxFromName("Ep1_7_BDroidPlatf", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep1_7_SFPowerUp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep1_7_SFHoverLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep1_7_SFTakeOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == MAULD_LDATA) {
            AddLevelSfxFromName("Ep1_7_FloatPlatRise", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegC_Crate", sfx_ids, &sfx_count, 0x400);
        } else if (level == KAMINOE_LDATA) {
            AddLevelSfxFromName("Kam_Slave1BlasterFire", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Slave1_EngineLp", sfx_ids, &sfx_count, 0x400);
        } else if (level == KAMINOC_LDATA) {
            AddLevelSfxFromName("Kam_DiscoFloorPanelOn", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_DiscoFloorPanelDone", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_SparkLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_SparkLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_ForceFieldLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_ForceFieldOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == FACTORYB_LDATA) {
            AddLevelSfxFromName("FacB_BeltLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FacB_ConvStop", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_StampHydros", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_StampImpacts", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BonusCylUp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BonusBeep", sfx_ids, &sfx_count, 0x400);
        } else if (level == FACTORYD_LDATA) {
            AddLevelSfxFromName("Fac_BucketGearUD", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BucketAwayFB", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BucketLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BucketArriveLock", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Fac_BucketOpens", sfx_ids, &sfx_count, 0x400);
        } else if (level == FACTORYF_LDATA) {
            AddLevelSfxFromName("Fac_LaserGateLoop", sfx_ids, &sfx_count, 0x400);
        } else if (level == FACTORYG_LDATA) {
            AddLevelSfxFromName("Fac_ObiRestraintLoop", sfx_ids, &sfx_count, 0x400);
        } else if (level == GUNSHIPB_LDATA) {
            AddLevelSfxFromName("CountdownTimerTick", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("CountdownTimerTock", sfx_ids, &sfx_count, 0x400);
        } else if (level == DOOKUC_LDATA) {
            AddLevelSfxFromName("Dooku_LightningLp", sfx_ids, &sfx_count, 0x400);
        } else if (level == DOGFIGHTA_LDATA) {
            AddLevelSfxFromName("Ep3_1_StarDestEngLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep3_1_ProtoXWingMissile", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep3_1_ExplosionM", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Ep3_1_ExplosionXXL", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TurretFire", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_CloneARC170Gun", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_JediStFighterGun", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_JediStFighterGun2", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_JediStFighterGun3", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_JediStFighterEngLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_CloneARC170EngLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_DroidFighterBlast", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TriFighterGuns", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TriFighterGuns2", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_DroidFighterEngLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TriFighterEngLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_DroidFighterHit", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TriFighterHit", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_HugeBeamGunFire", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_HugeBeamGunLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_SepShieldPwrDown", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_ShipBreakExplo", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_ShipDoorOpen", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_StDestTurretSpins", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dog_TowerBreakExplo", sfx_ids, &sfx_count, 0x400);
        } else if (level == CRUISERD_LDATA) {
            AddLevelSfxFromName("Cru_HugeWallMoveLp", sfx_ids, &sfx_count, 0x400);
        } else if (level == CRUISERG_LDATA) {
            AddLevelSfxFromName("NegA_CoinMDoorUp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("NegA_CoinMDoorDn", sfx_ids, &sfx_count, 0x400);
        } else if (level == KASHYYYKD_LDATA) {
            AddLevelSfxFromName("Kas_BoulderLoop", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kas_BoulderExplo", sfx_ids, &sfx_count, 0x400);
        } else if (level == VADERA_LDATA) {
            AddLevelSfxFromName("CountdownTimerTick", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("CountdownTimerTock", sfx_ids, &sfx_count, 0x400);
        } else if (level == TATOOINEA_LDATA) {
            AddLevelSfxFromName("env_shower_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == TATOOINEB_LDATA) {
            AddLevelSfxFromName("greenlighton", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_suckerspit_3PO", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_suckerspit_gonk", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("conveyorlp", sfx_ids, &sfx_count, 0x400);
        } else if (level == MOSEISLEYA_LDATA) {
            AddLevelSfxFromName("env_jacuzzi_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == MOSEISLEYC_LDATA) {
            AddLevelSfxFromName("Droid_BeamLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("SwPPad", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARRESCUEB_LDATA) {
            AddLevelSfxFromName("env_tractorbeam_off", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_tractorbeam_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARRESCUEC_LDATA) {
            AddLevelSfxFromName("ffield", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARRESCUEE_LDATA) {
            AddLevelSfxFromName("Turret_PitchLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Turret_YawLp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Turbo_Fire", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARESCAPEA_LDATA || level == DEATHSTARESCAPED_LDATA) {
            AddLevelSfxFromName("Dianoga_Groan", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Dianoga_Roar", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARESCAPEB_LDATA) {
            AddLevelSfxFromName("SqueakWash", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTARESCAPEC_LDATA || level == DEATHSTARBATTLEA_LDATA || level == DEATHSTARBATTLEB_LDATA ||
                   level == DEATHSTARBATTLEC_LDATA || level == DEATHSTARBATTLED_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == HOTHESCAPEA_LDATA) {
            AddLevelSfxFromName("ThermalDet_Beep", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_padLight_on", sfx_ids, &sfx_count, 0x400);
        } else if (level == HOTHESCAPEC_LDATA) {
            AddLevelSfxFromName("env_padLight_on", sfx_ids, &sfx_count, 0x400);
        } else if (level == ASTEROIDCHASED_LDATA) {
            AddLevelSfxFromName("exp_asteroid", sfx_ids, &sfx_count, 0x400);
        } else if (level == CLOUDCITYTRAPA_LDATA) {
            AddLevelSfxFromName("Env_Steam_Lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == CLOUDCITYESCAPEA_LDATA) {
            AddLevelSfxFromName("env_steam_lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Env_ctrl_desk_on", sfx_ids, &sfx_count, 0x400);
        } else if (level == CLOUDCITYESCAPEC_LDATA) {
            AddLevelSfxFromName("env_steam_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTAR2BATTLEB_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == DEATHSTAR2BATTLED_LDATA) {
            AddLevelSfxFromName("ForceLightningLp", sfx_ids, &sfx_count, 0x400);
        } else if (level == JABBASPALACEA_LDATA || level == JABBASPALACEB_LDATA || level == JABBASPALACED_LDATA) {
            AddLevelSfxFromName("SwDisco", sfx_ids, &sfx_count, 0x400);
        } else if (level == JABBASPALACEE_LDATA) {
            AddLevelSfxFromName("exp_minecart", sfx_ids, &sfx_count, 0x400);
        } else if (level == SARLACCPITB_LDATA) {
            AddLevelSfxFromName("env_curtain_lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_DiscoFloorPanelOn", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Kam_DiscoFloorPanelDone", sfx_ids, &sfx_count, 0x400);
        } else if (level == SARLACCPITC_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("imp_c3po_magnet_drop", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_magnet_on", sfx_ids, &sfx_count, 0x400);
        } else if (level == SPEEDERCHASEA_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == ENDORBATTLEA_LDATA) {
            AddLevelSfxFromName("env_lantern_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == ENDORBATTLEB_LDATA) {
            AddLevelSfxFromName("waterfall", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("drd_r2_mvt_water_lp", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_lantern_lp", sfx_ids, &sfx_count, 0x400);
        } else if (level == ENDORBATTLED_LDATA) {
            AddLevelSfxFromName("FField", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FFieldOff", sfx_ids, &sfx_count, 0x400);
        } else if (level == EMPERORFIGHTA_LDATA) {
            AddLevelSfxFromName("env_block_light_on", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_padlight_on", sfx_ids, &sfx_count, 0x400);
        }
    }

    if (VehicleArea != 0) {
        AddLevelSfxFromName("TieDoorsOpen", sfx_ids, &sfx_count, 0x400);
    }
    if (Mission_Active(NULL) != NULL) {
        AddLevelSfxFromName("Victory", sfx_ids, &sfx_count, 0x400);
    }
    if (Arcade != 0) {
        AddLevelSfxFromName("env_padLight_on", sfx_ids, &sfx_count, 0x400);
    }
    if (world->area == HOTHESCAPE_ADATA || world->area == JABBASPALACE_ADATA) {
        AddLevelSfxFromName("fs_ice", sfx_ids, &sfx_count, 0x400);
    } else if (world->area == DAGOBAH_ADATA) {
        AddLevelSfxFromName("fs_swamp", sfx_ids, &sfx_count, 0x400);
    }

    level = world->current_level;
    if (apicharsys->loaded_model_count > 0 && (level->flags & LEVEL_GAMEPLAY) != 0 && VehicleArea == 0) {
        GameAudio_AddSfx(0x14, sfx_ids, &sfx_count, 0x400);
        GameAudio_AddSfx(0x15, sfx_ids, &sfx_count, 0x400);
    }

    for (character_id = 0; character_id < apicharsys->character_count; ++character_id) {
        model_id = apicharsys->playermodelids[character_id];
        if (model_id == -1 || (apicharsys->models[model_id].flags & 1) == 0) {
            continue;
        }

        character = &CDataList[character_id];
        game_character = character->game_character;

        if (game_character->sfx_die != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_die;
        } else if ((character->model_flags &
                    (CHARACTER_MODEL_DROID | CHARACTER_MODEL_VEHICLE | CHARACTER_MODEL_MINIKIT | CHARACTER_MODEL_BEAST)) == 0) {
            GameAudio_AddSfx(((i32)(game_character->flags_090 << 14) >> 31) + 0x1c, sfx_ids, &sfx_count, 0x400);
        }
        if (game_character->sfx_hurt != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_hurt;
        } else if ((character->model_flags &
                    (CHARACTER_MODEL_DROID | CHARACTER_MODEL_VEHICLE | CHARACTER_MODEL_MINIKIT | CHARACTER_MODEL_BEAST)) == 0) {
            GameAudio_AddSfx(((i32)(game_character->flags_090 << 14) >> 31) + 0x17, sfx_ids, &sfx_count, 0x400);
        }
        if (game_character->sfx_grunt != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_grunt;
        } else if ((character->model_flags &
                    (CHARACTER_MODEL_DROID | CHARACTER_MODEL_VEHICLE | CHARACTER_MODEL_MINIKIT | CHARACTER_MODEL_BEAST)) == 0) {
            GameAudio_AddSfx(((i32)(game_character->flags_090 << 14) >> 31) + 0x15, sfx_ids, &sfx_count, 0x400);
        }

        if (game_character->sfx_engine != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_engine;
        }
        if (game_character->sfx_shoot != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_shoot;
        }
        if (game_character->sfx_footstep != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_footstep;
        }
        if (game_character->sfx_chatter != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_chatter;
        }
        if (game_character->sfx_sabre != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_sabre;
        }
        for (i = 0; i < 6 && game_character->sfx_misc[i] != -1; ++i) {
            sfx_ids[sfx_count++] = game_character->sfx_misc[i];
        }

        if ((character->model_flags & CHARACTER_MODEL_ASTROMECH) != 0) {
            AddLevelSfxFromName("drd_r2_scope_up", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("drd_r2_scope_down", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("drd_r2_mvt_water_lp", sfx_ids, &sfx_count, 0x400);
        }
        if ((game_character->flags_090 & CHARACTER_RUNTIME_CAN_DRAG_BOMBS) != 0) {
            AddLevelSfxFromName("TowCable_Fire", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("TowCable_Latch", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("TowCable_Detach", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("TowCable_Snap", sfx_ids, &sfx_count, 0x400);
        }
        if ((character->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
            AddLevelSfxFromName("XWing_Torpedo", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("env_tractorbeam_lp", sfx_ids, &sfx_count, 0x400);
            if ((character->model_flags & CHARACTER_MODEL_MINIKIT) == 0) {
                AddLevelSfxFromName("Explode1", sfx_ids, &sfx_count, 0x400);
            }
        }
        if (character_id == id_CHEWBACCA) {
            AddLevelSfxFromName("C3_Hurt", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("C3_Death", sfx_ids, &sfx_count, 0x400);
        }
        if (character->move_fn == Move_BEAST) {
            AddLevelSfxFromName("Lego_Poo", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Lego_PLOP", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("FliesLp", sfx_ids, &sfx_count, 0x400);
        }
        if (game_character->variant == CHARVARIANT_TIE) {
            AddLevelSfxFromName("veh_tie_by", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("Tie_Spins", sfx_ids, &sfx_count, 0x400);
        }
        if (character_id == id_EWOK || character_id == id_WICKET) {
            AddLevelSfxFromName("wpn_bomb_drop", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("exp_bomb", sfx_ids, &sfx_count, 0x400);
        }
        if ((game_character->flags_090 & CHARACTER_RUNTIME_LIGHTNING) != 0) {
            AddLevelSfxFromName("ForceLightningLp", sfx_ids, &sfx_count, 0x400);
        } else if ((game_character->flags_090 & CHARACTER_RUNTIME_CHOKE) != 0) {
            AddLevelSfxFromName("ForceChokeCrunch", sfx_ids, &sfx_count, 0x400);
        } else if ((character->model_flags & CHARACTER_MODEL_JEDI) != 0) {
            AddLevelSfxFromName("ForceMindTrick", sfx_ids, &sfx_count, 0x400);
        }
        if (game_character->variant == CHARVARIANT_LUKE && (character->model_flags & CHARACTER_MODEL_JEDI) != 0 &&
            id_GAMORREANGUARD != -1 && apicharsys->playermodelids[id_GAMORREANGUARD] != -1) {
            AddLevelSfxFromName("ForceChokeCrunch", sfx_ids, &sfx_count, 0x400);
        }
        if ((game_character->flags_094 & CHARACTER_OPTION_HAS_WHIP) != 0) {
            GameAudio_AddSfx(0x4c, sfx_ids, &sfx_count, 0x400);
            GameAudio_AddSfx(0x4d, sfx_ids, &sfx_count, 0x400);
            GameAudio_AddSfx(0x4e, sfx_ids, &sfx_count, 0x400);
        }

        effect = character->effects;
        if (effect != NULL) {
            while (effect->character_id != -1) {
                if (effect->sound_id != -1) {
                    sfx_ids[sfx_count++] = effect->sound_id;
                }
                ++effect;
            }
        }
        if ((character->model_flags & CHARACTER_MODEL_PROTOCOL) != 0) {
            AddLevelSfxFromName("TC14_VLA", sfx_ids, &sfx_count, 0x400);
            AddLevelSfxFromName("TC14_VLN", sfx_ids, &sfx_count, 0x400);
        }
        if ((character->model_flags & CHARACTER_MODEL_ASTROMECH) != 0) {
            AddLevelSfxFromName("R2D2_VLA", sfx_ids, &sfx_count, 0x400);
        }
    }

    level = world->current_level;
    double_score_sfx = Arcade != 0 || ((level->flags & LEVEL_DOUBLE_SCORE) != 0);
    if (!double_score_sfx) {
        for (portal = 0x13; portal <= 0x17; ++portal) {
            if (world->camera_splines[portal] != NULL) {
                double_score_sfx = 1;
                break;
            }
        }
    }
    if (double_score_sfx) {
        AddLevelSfxFromName("ui_DoubleScoreEntry", sfx_ids, &sfx_count, 0x400);
        AddLevelSfxFromName("ui_DoubleScoreText", sfx_ids, &sfx_count, 0x400);
    }

    AddLevelSfxGizmoSys(world->gizmo_sys, world, sfx_ids, &sfx_count, 0x400);
    SetSpecialSfxBits(sfx_ids, &sfx_count, world);
    sfx_ids[sfx_count] = -1;
    if (sfx_count > 0) {
        SetSoundBitsById(sfx_ids, SetSfxBit_On);
    }
    SfxBitsStore(&CurrentSFXTAB);
}

void Shadow_SetMode(void) {
    i32 i;
    ShadowMode = 0;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *player = Player[i];
        if (player != NULL && player->player.suit != NULL && (player->player.suit->flags & SUIT_SHADOW) != 0) {
            ShadowMode = 1;
            return;
        }
    }
}
