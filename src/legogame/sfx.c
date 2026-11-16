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

typedef struct level_sfx_world_view_s { // size 0x5078
    u8 unknown_00[0x4684];              // 0x0000
    i32 teleport_count;                 // 0x4684
    u8 unknown_4688[0x38];              // 0x4688
    i32 push_block_count;               // 0x46c0
    u8 unknown_46c4[0x20];              // 0x46c4
    i32 spinner_count;                  // 0x46e4
    u8 unknown_46e8[4];                 // 0x46e8
    void *grabber;                      // 0x46ec
    u8 unknown_46f0[0x960];             // 0x46f0
    LEVEL_SFX_PULSE *pulses_sys;        // 0x5050
    u8 unknown_5054[0x18];              // 0x5054
    i32 nlevers;                        // 0x506c
    u8 unknown_5070[4];                 // 0x5070
    i32 *hat_machine_sys;               // 0x5074
} LEVEL_SFX_WORLD_VIEW;

typedef struct level_sfx_cut_slot_s { // size 0x14
    i16 id;                           // 0x00
    u8 unknown_02[0x12];              // 0x02
} LEVEL_SFX_CUT_SLOT;

typedef struct level_sfx_cut_s { // size 0xe8
    u8 unknown_00[0x70];         // 0x00
    LEVEL_SFX_CUT_SLOT sfx[6];   // 0x70
} LEVEL_SFX_CUT;

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
    LEVEL_SFX_WORLD_VIEW *view = (LEVEL_SFX_WORLD_VIEW *)world;
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

#define ADD_SFX(name) AddLevelSfxFromName(name, sfx_ids, &sfx_count, 0x400)

    for (i = 0; i < world->level_sfx_count; ++i) {
        sfx_ids[sfx_count++] = world->level_sfx[i].id;
    }

    if (view->spinner_count > 0) {
        ADD_SFX("fly_paddle_rotate_lp");
        ADD_SFX("fly_paddle_stuck");
    }
    if (view->teleport_count > 0) {
        ADD_SFX("env_door_flap");
    }

    GameAudio_AddSfx(0x50, sfx_ids, &sfx_count, 0x400);
    GameAudio_AddSfx(0x51, sfx_ids, &sfx_count, 0x400);
    GameAudio_AddSfx(0x52, sfx_ids, &sfx_count, 0x400);
    ADD_SFX("Grv_GuardWeaponLp");

    if (view->grabber != NULL) {
        if (world->current_level == CLOUDCITYTRAPA_LDATA) {
            ADD_SFX("CarbonFreezeCraneLp");
            ADD_SFX("CarbonFreezeCrane");
        } else if (world->current_level == JABBASPALACEB_LDATA) {
            ADD_SFX("env_hover_box_lp");
        } else {
            ADD_SFX("env_crane_mvt_lp");
        }
        ADD_SFX("env_crane_in");
        ADD_SFX("env_grabber_down");
        ADD_SFX("env_grabber_pickup");
        ADD_SFX("env_grabber_up");
        ADD_SFX("Explode2");
        if (world->current_level == CLOUDCITYESCAPEC_LDATA) {
            ADD_SFX("imp_C3PO_magnet");
            ADD_SFX("imp_C3PO_magnet_drop");
        }
    }

    if (world->cutscene_sys != NULL) {
        for (i = 0; i < world->cutscene_sys->count; ++i) {
            for (j = 0; j < 6; ++j) {
                LEVEL_SFX_CUT *cut = (LEVEL_SFX_CUT *)world->cutscene_sys->records[i];
                i32 id = cut->sfx[j].id;
                if (id != -1) {
                    sfx_ids[sfx_count++] = id;
                }
            }
        }
    }

    Pulses_AddSfx(view->pulses_sys, sfx_ids, &sfx_count, 0x400);
    if (view->hat_machine_sys != NULL && *view->hat_machine_sys > 0) {
        ADD_SFX("SwLever");
        ADD_SFX("HatOn");
    }
    if (view->nlevers > 0) {
        ADD_SFX("SwLever");
    }

    if (world->area == NULL || (world->area->flags & AREAFLAG_VEHICLE_AREA) == 0) {
        ADD_SFX("Char_Slide_Lp");
        if (view->push_block_count > 0) {
            ADD_SFX("Block_Shove");
            ADD_SFX("Block_Push_Lp");
        }
        ADD_SFX("exp_thermalDet");
        ADD_SFX("imp_thermalDet_attach");
        if (world->area == SPEEDERCHASE_ADATA) {
            ADD_SFX("XWing_LoopDeLoop");
        }
    } else {
        ADD_SFX("XWing_Torpedo");
        ADD_SFX("imp_proton_torp");
        ADD_SFX("XWing_LoopDeLoop");
    }
    ADD_SFX("Explode1");

    level = world->current_level;
    if (level->sfx_ambient != -1) {
        sfx_ids[sfx_count++] = level->sfx_ambient;
    }
    if (level == STATUS_LDATA || (level->flags & LEVEL_STATUS) != 0) {
        ADD_SFX("StatusAward");
        ADD_SFX("Status_GoldBarDec");
        ADD_SFX("TrueJedi_100pc");
        ADD_SFX("TrueJedi_NOT");
        ADD_SFX("MK-Panel");
        ADD_SFX("Char_Icon_App");
        ADD_SFX("Char_Icon_Slide");
        ADD_SFX("LegoClicks");
        ADD_SFX("Shop_BuyCheat");
        ADD_SFX("Explode1");
        ADD_SFX("Jp_Ana_Jump");
        ADD_SFX("PickupCoin");
    } else if (world->area != NULL && (world->area->flags & AREAFLAG_BONUS_AREA) != 0) {
        ADD_SFX("Victory");
        ADD_SFX("exp_debris");
    } else if (level == CREDITS_LDATA) {
        ADD_SFX("StatusAward");
    }

    if (world->area != NULL) {
        if (world->area->super_counter_count != 0) {
            GameAudio_AddSfx(0x53, sfx_ids, &sfx_count, 0x400);
        }
        if (world->area == PODRACE_ADATA || world->area == PODSPRINT_ADATA) {
            ADD_SFX("PodX_TuskenBlast");
            ADD_SFX("Pod_TuskHit");
            ADD_SFX("PodX_EngSebulba_Lp");
            ADD_SFX("PodX_EngAnakin_Lp");
            ADD_SFX("PodX_EngGeneric1_Lp");
            ADD_SFX("PodX_EngGeneric2_Lp");
            ADD_SFX("PodX_EngGeneric3_Lp");
            ADD_SFX("PodX_EngStartup1");
            ADD_SFX("PodX_EngStartup2");
            ADD_SFX("Pod_Race_Go");
            ADD_SFX("Pod_Race_Light");
            ADD_SFX("PodX_EngGeneric_Lp");
            ADD_SFX("PodX_Collide");
            ADD_SFX("PodX_Crash");
            ADD_SFX("PodX_Booster");
            ADD_SFX("CountdownTimerTick");
            ADD_SFX("CountdownTimerTock");
            ADD_SFX("PodX_PurpCrysHit");
        } else if (world->area == GUNSHIP_ADATA || world->area == BONUS_GUNSHIP_ADATA) {
            ADD_SFX("GC_GunshipBlasterFire");
            ADD_SFX("GC_GunshipEngineLp");
            ADD_SFX("GC_GunshipDeath");
            ADD_SFX("GC_LaserBeam");
            ADD_SFX("Explode1");
        } else if (world->area == HUB_ADATA) {
            ADD_SFX("Shop_BuyCheat");
            ADD_SFX("Shop_NotEnufMuny");
            ADD_SFX("ui_ZoomIn");
            ADD_SFX("ui_ZoomOut");
            ADD_SFX("ui_hover_lp");
        } else if (world->area == JABBASPALACE_ADATA) {
            ADD_SFX("swdisco");
        } else if (world->area == NEWTOWN_ADATA) {
            ADD_SFX("BBounce");
        }
    }

    level = world->current_level;
    {
        if (level == NEGOTIATIONSA_LDATA) {
            ADD_SFX("NegA_CoinMDoorUp");
            ADD_SFX("NegA_CoinMDoorDn");
        } else if (level == NEGOTIATIONSB_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("NegC_Crate");
        } else if (level == NEGOTIATIONSC_LDATA) {
            ADD_SFX("NegC_MagnetCoil");
            ADD_SFX("NegC_MagnetCoilH");
            ADD_SFX("NegC_MagnetCoilS");
            ADD_SFX("FField");
            ADD_SFX("NegC_Crate");
        } else if (level == GUNGAN_A_LDATA) {
            ADD_SFX("GunA_TreeFall");
            ADD_SFX("FS_CreaRun");
            ADD_SFX("IkopiGrowl");
            ADD_SFX("KaaduBark");
            ADD_SFX("KaaduGrowl");
        } else if (level == RESCUEC_LDATA) {
            ADD_SFX("ResX_MarbColLp");
            ADD_SFX("ResX_PoleLift_Rise");
            ADD_SFX("ResX_PoleLift_Lower");
        } else if (level == RETAKEB_LDATA) {
            ADD_SFX("ResX_MarbColLp");
        } else if (level == RETAKED_LDATA) {
            ADD_SFX("ResX_PortCulLp");
            ADD_SFX("ResX_PortCulEnd");
            ADD_SFX("Ep1_5_OutdoorPlatform");
        } else if (level == RETAKEE_LDATA) {
            ADD_SFX("Ep1_5_OutdoorPlatform");
            ADD_SFX("ResX_PortCulLp");
            ADD_SFX("ResX_PortCulEnd");
        } else if (level == MAULA_LDATA) {
            ADD_SFX("Ep1_7_BDroidPlatf");
            ADD_SFX("Ep1_7_SFPowerUp");
            ADD_SFX("Ep1_7_SFHoverLp");
            ADD_SFX("Ep1_7_SFTakeOff");
        } else if (level == MAULD_LDATA) {
            ADD_SFX("Ep1_7_FloatPlatRise");
            ADD_SFX("NegC_Crate");
        } else if (level == KAMINOE_LDATA) {
            ADD_SFX("Kam_Slave1BlasterFire");
            ADD_SFX("Slave1_EngineLp");
        } else if (level == KAMINOC_LDATA) {
            ADD_SFX("Kam_DiscoFloorPanelOn");
            ADD_SFX("Kam_DiscoFloorPanelDone");
            ADD_SFX("Kam_SparkLp");
            ADD_SFX("Kam_SparkLp");
            ADD_SFX("Kam_ForceFieldLp");
            ADD_SFX("Kam_ForceFieldOff");
        } else if (level == FACTORYB_LDATA) {
            ADD_SFX("FacB_BeltLp");
            ADD_SFX("FacB_ConvStop");
            ADD_SFX("Fac_StampHydros");
            ADD_SFX("Fac_StampImpacts");
            ADD_SFX("Fac_BonusCylUp");
            ADD_SFX("Fac_BonusBeep");
        } else if (level == FACTORYD_LDATA) {
            ADD_SFX("Fac_BucketGearUD");
            ADD_SFX("Fac_BucketAwayFB");
            ADD_SFX("Fac_BucketLp");
            ADD_SFX("Fac_BucketArriveLock");
            ADD_SFX("Fac_BucketOpens");
        } else if (level == FACTORYF_LDATA) {
            ADD_SFX("Fac_LaserGateLoop");
        } else if (level == FACTORYG_LDATA) {
            ADD_SFX("Fac_ObiRestraintLoop");
        } else if (level == GUNSHIPB_LDATA) {
            ADD_SFX("CountdownTimerTick");
            ADD_SFX("CountdownTimerTock");
        } else if (level == DOOKUC_LDATA) {
            ADD_SFX("Dooku_LightningLp");
        } else if (level == DOGFIGHTA_LDATA) {
            ADD_SFX("Ep3_1_StarDestEngLp");
            ADD_SFX("Ep3_1_ProtoXWingMissile");
            ADD_SFX("Ep3_1_ExplosionM");
            ADD_SFX("Ep3_1_ExplosionXXL");
            ADD_SFX("Dog_TurretFire");
            ADD_SFX("Dog_CloneARC170Gun");
            ADD_SFX("Dog_JediStFighterGun");
            ADD_SFX("Dog_JediStFighterGun2");
            ADD_SFX("Dog_JediStFighterGun3");
            ADD_SFX("Dog_JediStFighterEngLp");
            ADD_SFX("Dog_CloneARC170EngLp");
            ADD_SFX("Dog_DroidFighterBlast");
            ADD_SFX("Dog_TriFighterGuns");
            ADD_SFX("Dog_TriFighterGuns2");
            ADD_SFX("Dog_DroidFighterEngLp");
            ADD_SFX("Dog_TriFighterEngLp");
            ADD_SFX("Dog_DroidFighterHit");
            ADD_SFX("Dog_TriFighterHit");
            ADD_SFX("Dog_HugeBeamGunFire");
            ADD_SFX("Dog_HugeBeamGunLp");
            ADD_SFX("Dog_SepShieldPwrDown");
            ADD_SFX("Dog_ShipBreakExplo");
            ADD_SFX("Dog_ShipDoorOpen");
            ADD_SFX("Dog_StDestTurretSpins");
            ADD_SFX("Dog_TowerBreakExplo");
        } else if (level == CRUISERD_LDATA) {
            ADD_SFX("Cru_HugeWallMoveLp");
        } else if (level == CRUISERG_LDATA) {
            ADD_SFX("NegA_CoinMDoorUp");
            ADD_SFX("NegA_CoinMDoorDn");
        } else if (level == KASHYYYKD_LDATA) {
            ADD_SFX("Kas_BoulderLoop");
            ADD_SFX("Kas_BoulderExplo");
        } else if (level == VADERA_LDATA) {
            ADD_SFX("CountdownTimerTick");
            ADD_SFX("CountdownTimerTock");
        } else if (level == TATOOINEA_LDATA) {
            ADD_SFX("env_shower_lp");
        } else if (level == TATOOINEB_LDATA) {
            ADD_SFX("greenlighton");
            ADD_SFX("env_suckerspit_3PO");
            ADD_SFX("env_suckerspit_gonk");
            ADD_SFX("conveyorlp");
        } else if (level == MOSEISLEYA_LDATA) {
            ADD_SFX("env_jacuzzi_lp");
        } else if (level == MOSEISLEYC_LDATA) {
            ADD_SFX("Droid_BeamLp");
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
            ADD_SFX("SwPPad");
        } else if (level == DEATHSTARRESCUEB_LDATA) {
            ADD_SFX("env_tractorbeam_off");
            ADD_SFX("env_tractorbeam_lp");
        } else if (level == DEATHSTARRESCUEC_LDATA) {
            ADD_SFX("ffield");
        } else if (level == DEATHSTARRESCUEE_LDATA) {
            ADD_SFX("Turret_PitchLp");
            ADD_SFX("Turret_YawLp");
            ADD_SFX("Turbo_Fire");
        } else if (level == DEATHSTARESCAPEA_LDATA || level == DEATHSTARESCAPED_LDATA) {
            ADD_SFX("Dianoga_Groan");
            ADD_SFX("Dianoga_Roar");
        } else if (level == DEATHSTARESCAPEB_LDATA) {
            ADD_SFX("SqueakWash");
        } else if (level == DEATHSTARESCAPEC_LDATA || level == DEATHSTARBATTLEA_LDATA || level == DEATHSTARBATTLEB_LDATA ||
                   level == DEATHSTARBATTLEC_LDATA || level == DEATHSTARBATTLED_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == HOTHESCAPEA_LDATA) {
            ADD_SFX("ThermalDet_Beep");
            ADD_SFX("env_padLight_on");
        } else if (level == HOTHESCAPEC_LDATA) {
            ADD_SFX("env_padLight_on");
        } else if (level == ASTEROIDCHASED_LDATA) {
            ADD_SFX("exp_asteroid");
        } else if (level == CLOUDCITYTRAPA_LDATA) {
            ADD_SFX("Env_Steam_Lp");
        } else if (level == CLOUDCITYESCAPEA_LDATA) {
            ADD_SFX("env_steam_lp");
            ADD_SFX("Env_ctrl_desk_on");
        } else if (level == CLOUDCITYESCAPEC_LDATA) {
            ADD_SFX("env_steam_lp");
        } else if (level == DEATHSTAR2BATTLEB_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == DEATHSTAR2BATTLED_LDATA) {
            ADD_SFX("ForceLightningLp");
        } else if (level == JABBASPALACEA_LDATA || level == JABBASPALACEB_LDATA || level == JABBASPALACED_LDATA) {
            ADD_SFX("SwDisco");
        } else if (level == JABBASPALACEE_LDATA) {
            ADD_SFX("exp_minecart");
        } else if (level == SARLACCPITB_LDATA) {
            ADD_SFX("env_curtain_lp");
            ADD_SFX("Kam_DiscoFloorPanelOn");
            ADD_SFX("Kam_DiscoFloorPanelDone");
        } else if (level == SARLACCPITC_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
            ADD_SFX("imp_c3po_magnet_drop");
            ADD_SFX("env_magnet_on");
        } else if (level == SPEEDERCHASEA_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == ENDORBATTLEA_LDATA) {
            ADD_SFX("env_lantern_lp");
        } else if (level == ENDORBATTLEB_LDATA) {
            ADD_SFX("waterfall");
            ADD_SFX("drd_r2_mvt_water_lp");
            ADD_SFX("env_lantern_lp");
        } else if (level == ENDORBATTLED_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == EMPERORFIGHTA_LDATA) {
            ADD_SFX("env_block_light_on");
            ADD_SFX("env_padlight_on");
        }
    }

    if (VehicleArea != 0) {
        ADD_SFX("TieDoorsOpen");
    }
    if (Mission_Active(NULL) != NULL) {
        ADD_SFX("Victory");
    }
    if (Arcade != 0) {
        ADD_SFX("env_padLight_on");
    }
    if (world->area == HOTHESCAPE_ADATA || world->area == JABBASPALACE_ADATA) {
        ADD_SFX("fs_ice");
    } else if (world->area == DAGOBAH_ADATA) {
        ADD_SFX("fs_swamp");
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
            ADD_SFX("drd_r2_scope_up");
            ADD_SFX("drd_r2_scope_down");
            ADD_SFX("drd_r2_mvt_water_lp");
        }
        if ((game_character->flags_090 & CHARACTER_RUNTIME_CAN_DRAG_BOMBS) != 0) {
            ADD_SFX("TowCable_Fire");
            ADD_SFX("TowCable_Latch");
            ADD_SFX("TowCable_Detach");
            ADD_SFX("TowCable_Snap");
        }
        if ((character->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
            ADD_SFX("XWing_Torpedo");
            ADD_SFX("env_tractorbeam_lp");
            if ((character->model_flags & CHARACTER_MODEL_MINIKIT) == 0) {
                ADD_SFX("Explode1");
            }
        }
        if (character_id == id_CHEWBACCA) {
            ADD_SFX("C3_Hurt");
            ADD_SFX("C3_Death");
        }
        if (character->move_fn == Move_BEAST) {
            ADD_SFX("Lego_Poo");
            ADD_SFX("Lego_PLOP");
            ADD_SFX("FliesLp");
        }
        if (game_character->variant == CHARVARIANT_TIE) {
            ADD_SFX("veh_tie_by");
            ADD_SFX("Tie_Spins");
        }
        if (character_id == dat_007f1924 || character_id == dat_007f1928) {
            ADD_SFX("wpn_bomb_drop");
            ADD_SFX("exp_bomb");
        }
        if ((game_character->flags_090 & CHARACTER_RUNTIME_LIGHTNING) != 0) {
            ADD_SFX("ForceLightningLp");
        } else if ((game_character->flags_090 & CHARACTER_RUNTIME_CHOKE) != 0) {
            ADD_SFX("ForceChokeCrunch");
        } else if ((character->model_flags & CHARACTER_MODEL_JEDI) != 0) {
            ADD_SFX("ForceMindTrick");
        }
        if (game_character->variant == CHARVARIANT_LUKE && (character->model_flags & CHARACTER_MODEL_JEDI) != 0 &&
            id_GAMORREANGUARD != -1 && apicharsys->playermodelids[id_GAMORREANGUARD] != -1) {
            ADD_SFX("ForceChokeCrunch");
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
            ADD_SFX("TC14_VLA");
            ADD_SFX("TC14_VLN");
        }
        if ((character->model_flags & CHARACTER_MODEL_ASTROMECH) != 0) {
            ADD_SFX("R2D2_VLA");
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
        ADD_SFX("ui_DoubleScoreEntry");
        ADD_SFX("ui_DoubleScoreText");
    }

    AddLevelSfxGizmoSys(world->gizmo_sys, world, sfx_ids, &sfx_count, 0x400);
    SetSpecialSfxBits(sfx_ids, &sfx_count, world);
    sfx_ids[sfx_count] = -1;
    if (sfx_count > 0) {
        SetSoundBitsById(sfx_ids, SetSfxBit_On);
    }
    SfxBitsStore(&CurrentSFXTAB);

#undef ADD_SFX
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
