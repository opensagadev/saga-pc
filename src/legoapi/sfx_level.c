#include "saga.h"

typedef struct level_sfx_world_view_s {
    u8 unknown_00[0x4684];
    i32 teleport_count;
    u8 unknown_4688[0x38];
    i32 push_block_count;
    u8 unknown_46c4[0x20];
    i32 spinner_count;
    u8 unknown_46e8[4];
    void *grabber;
    u8 unknown_46f0[0x960];
    LEVEL_SFX_PULSE_PC *pulses_sys;
    u8 unknown_5054[0x18];
    i32 nlevers;
    u8 unknown_5070[4];
    i32 *hat_machine_sys;
} LEVEL_SFX_WORLD_VIEW_PC;
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, teleport_count) == 0x4684);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, push_block_count) == 0x46c0);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, spinner_count) == 0x46e4);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, grabber) == 0x46ec);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, pulses_sys) == 0x5050);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, nlevers) == 0x506c);
STATIC_ASSERT(offsetof(LEVEL_SFX_WORLD_VIEW_PC, hat_machine_sys) == 0x5074);
typedef struct level_sfx_cut_slot_s {
    i16 id;
    u8 unknown_02[0x12];
} LEVEL_SFX_CUT_SLOT_PC;
STATIC_ASSERT(sizeof(LEVEL_SFX_CUT_SLOT_PC) == 0x14);
typedef struct level_sfx_cut_s {
    u8 unknown_00[0x70];
    LEVEL_SFX_CUT_SLOT_PC sfx[6];
} LEVEL_SFX_CUT_PC;
STATIC_ASSERT(offsetof(LEVEL_SFX_CUT_PC, sfx) == 0x70);

void __cdecl SetLevelSfxBits(WORLDINFO *world) {
    LEVEL_SFX_WORLD_VIEW_PC *view = (LEVEL_SFX_WORLD_VIEW_PC *)world;
    i32 i, j, character_id, portal;
    LEVELDATA_PC *level;
    i16 model_id;
    CHARACTERDATA_PC *character;
    PLAYERCHARACTERCONFIG_PC *game_character;
    CHARACTER_EFFECT_CONFIG_PC *effect;
    i32 double_score_sfx;
    i32 sfx_ids[1024];
    i32 sfx_count = 0;

#define ADD_SFX(name) AddLevelSfxFromName(name, sfx_ids, &sfx_count, 0x400)
#define ADD_GAME_SFX(id) GameAudio_AddSfx(id, sfx_ids, &sfx_count, 0x400)

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

    ADD_GAME_SFX(0x50);
    ADD_GAME_SFX(0x51);
    ADD_GAME_SFX(0x52);
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
                LEVEL_SFX_CUT_PC *cut = (LEVEL_SFX_CUT_PC *)world->cutscene_sys->records[i];
                if (cut->sfx[j].id != -1)
                    sfx_ids[sfx_count++] = cut->sfx[j].id;
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

    if (world->area == NULL || (world->area->flags & 1) == 0) {
        ADD_SFX("Char_Slide_Lp");
        if (view->push_block_count > 0) {
            ADD_SFX("Block_Shove");
            ADD_SFX("Block_Push_Lp");
        }
        ADD_SFX("exp_thermalDet");
        ADD_SFX("imp_thermalDet_attach");
        if (world->area != NULL && world->area == SPEEDERCHASE_ADATA) {
            ADD_SFX("XWing_LoopDeLoop");
        }
    } else {
        ADD_SFX("XWing_Torpedo");
        ADD_SFX("imp_proton_torp");
        ADD_SFX("XWing_LoopDeLoop");
    }
    ADD_SFX("Explode1");

    level = world->current_level;
    {
        if (level->sfx_ambient != -1) {
            sfx_ids[sfx_count++] = level->sfx_ambient;
        }
        if (level == STATUS_LDATA || (level->flags & 0x400) != 0) {
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
        } else if (world->area != NULL && (world->area->flags & 4) != 0) {
            ADD_SFX("Victory");
            ADD_SFX("exp_debris");
        } else if (level == CREDITS_LDATA) {
            ADD_SFX("StatusAward");
        }
    }

    if (world->area != NULL) {
        if (world->area->super_counter_count != 0) {
            ADD_GAME_SFX(0x53);
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
        } else if (level == DEATHSTARESCAPEA_LDATA) {
            ADD_SFX("Dianoga_Groan");
            ADD_SFX("Dianoga_Roar");
        } else if (level == DEATHSTARESCAPED_LDATA) {
            ADD_SFX("Dianoga_Groan");
            ADD_SFX("Dianoga_Roar");
        } else if (level == DEATHSTARESCAPEB_LDATA) {
            ADD_SFX("SqueakWash");
        } else if (level == DEATHSTARESCAPEC_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == DEATHSTARBATTLEA_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == DEATHSTARBATTLEB_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == DEATHSTARBATTLEC_LDATA) {
            ADD_SFX("FField");
            ADD_SFX("FFieldOff");
        } else if (level == DEATHSTARBATTLED_LDATA) {
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
        } else if (level == JABBASPALACEA_LDATA) {
            ADD_SFX("SwDisco");
        } else if (level == JABBASPALACEB_LDATA) {
            ADD_SFX("SwDisco");
        } else if (level == JABBASPALACED_LDATA) {
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
    if (apicharsys->loaded_model_count > 0 && (level->flags & 2) != 0 && VehicleArea == 0) {
        ADD_GAME_SFX(0x14);
        ADD_GAME_SFX(0x15);
    }

    for (character_id = 0; character_id < apicharsys->character_count; ++character_id) {
        model_id = apicharsys->playermodelids[character_id];
        if (model_id == -1 || (apicharsys->models[model_id].flags & 1) == 0) {
            continue;
        }

        character = &((CHARACTERDATA_PC *)CDataList)[character_id];
        game_character = character->game_character;

        if (game_character->sfx_die != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_die;
        } else if ((character->model_flags & 0x44002010) == 0) {
            ADD_GAME_SFX(((i32)(game_character->flags_090 << 14) >> 31) + 0x1c);
        }
        if (game_character->sfx_hurt != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_hurt;
        } else if ((character->model_flags & 0x44002010) == 0) {
            ADD_GAME_SFX(((i32)(game_character->flags_090 << 14) >> 31) + 0x17);
        }
        if (game_character->sfx_grunt != -1) {
            sfx_ids[sfx_count++] = game_character->sfx_grunt;
        } else if ((character->model_flags & 0x44002010) == 0) {
            ADD_GAME_SFX(((i32)(game_character->flags_090 << 14) >> 31) + 0x15);
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

        if ((character->model_flags & 0x40) != 0) {
            ADD_SFX("drd_r2_scope_up");
            ADD_SFX("drd_r2_scope_down");
            ADD_SFX("drd_r2_mvt_water_lp");
        }
        if ((game_character->flags_090 & 0x400) != 0) {
            ADD_SFX("TowCable_Fire");
            ADD_SFX("TowCable_Latch");
            ADD_SFX("TowCable_Detach");
            ADD_SFX("TowCable_Snap");
        }
        if ((character->model_flags & 0x2000) != 0) {
            ADD_SFX("XWing_Torpedo");
            ADD_SFX("env_tractorbeam_lp");
            if ((character->model_flags & 0x04000000) == 0) {
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
        if (game_character->weapon_action == 10) {
            ADD_SFX("veh_tie_by");
            ADD_SFX("Tie_Spins");
        }
        if (character_id == VehicleBoltId7f1924Pc || character_id == VehicleBoltId7f1928Pc) {
            ADD_SFX("wpn_bomb_drop");
            ADD_SFX("exp_bomb");
        }
        if ((game_character->flags_090 & 4) != 0) {
            ADD_SFX("ForceLightningLp");
        } else if ((game_character->flags_090 & 2) != 0) {
            ADD_SFX("ForceChokeCrunch");
        } else if ((character->model_flags & 8) != 0) {
            ADD_SFX("ForceMindTrick");
        }
        if (game_character->weapon_action == 12 && (character->model_flags & 8) != 0 && id_GAMORREANGUARD != -1 &&
            apicharsys->playermodelids[id_GAMORREANGUARD] != -1) {
            ADD_SFX("ForceChokeCrunch");
        }
        if ((game_character->flags_094 & 0x20000000) != 0) {
            ADD_GAME_SFX(0x4c);
            ADD_GAME_SFX(0x4d);
            ADD_GAME_SFX(0x4e);
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
        if ((character->model_flags & 0x20) != 0) {
            ADD_SFX("TC14_VLA");
            ADD_SFX("TC14_VLN");
        }
        if ((character->model_flags & 0x40) != 0) {
            ADD_SFX("R2D2_VLA");
        }
    }

    level = world->current_level;
    double_score_sfx = Arcade != 0 || ((level->flags & 0x800) != 0);
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

    AddLevelSfxGizmoSys((GIZMOSYS_PREFIX_PC *)world->gizmo_sys, world, sfx_ids, &sfx_count, 0x400);
    SetSpecialSfxBits(sfx_ids, &sfx_count, world);
    sfx_ids[sfx_count] = -1;
    if (sfx_count > 0) {
        SetSoundBitsById(sfx_ids, SetSfxBit_On);
    }
    SfxBitsStore(&CurrentSFXTAB);

#undef ADD_GAME_SFX
#undef ADD_SFX
}

/* PC 0x00547d70. */
void __cdecl LevChatterSfx(char *name, NUVEC *position) {
    if (TerrainChatterWaitPc <= 0.0f && qrand() < 0x400) {
        PlaySfx(name, position);
        TerrainChatterWaitPc = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + NuTimeBarColumnStep);
    }
}
