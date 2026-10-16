#include "saga.h"
#include <string.h>
#include <stdio.h>

void __cdecl NewGame(void) {
    OPTIONSSAVE saved_options = Game.options_save;
    i32 i;
    u8 previous_group = 0;
    TempOptions = saved_options;
    memset(&Game, 0, sizeof(Game));
    Game.options_save = saved_options;
    adaptivedifficulty.level = 0;
    adaptivedifficulty.accumulator = 0.5f;
    adaptivedifficulty.time_elapsed = 0.0f;
    Game.save_version = 5;
    if (EDataList != NULL)
        Game.area_save[EDataList[0].area_ids[0]].complete = 1;
    else
        Game.area_save[0].complete = 1;
    if (SENATE_ADATA != NULL)
        Game.area_save[SENATE_ADATA->index].complete = 1;
    if (UTAPAU_ADATA != NULL)
        Game.area_save[UTAPAU_ADATA->index].complete = 1;
    if (HOTH_ADATA != NULL)
        Game.area_save[HOTH_ADATA->index].complete = 1;
    if (BONUSDAGOBAH_ADATA != NULL)
        Game.area_save[BONUSDAGOBAH_ADATA->index].complete = 1;
    if (BONUSKAMINO_ADATA != NULL)
        Game.area_save[BONUSKAMINO_ADATA->index].complete = 1;
    if (BONUSKASHYYYK_ADATA != NULL)
        Game.area_save[BONUSKASHYYYK_ADATA->index].complete = 1;
    Cheats_TurnOff(0);
    Game.purchased_extra_bits_high |= 0x1000;
    Cheat_SetOn(44, 1, 1);
    Hint_ClearHintsAndDoneFlags();
    LswForceUsedHintFlagsPc &= ~7u;
    GamePad_InitButtons();
    Tag_DoneFirst = 0;
    Tag_DoneAny = 0;
    for (i = 0; i < AREACOUNT; ++i)
        Game.area_save[i].challenge_trial_time = (f32)ADataList[i].challenge_trial_time;
    for (i = 0; i < 6; ++i) {
        Game.episode_start[i].superstory_time_limit = 3600.0f;
        Game.episode_start[i].superstory_score_target = 100000;
    }
    Customiser_CopyDefaultPiecesToSave(CharacterCustomiser, Game.customiser_start);
    if (TTab != NULL) {
        if (TTab[204] != NULL)
            NuStrCpy(Game.customiser_start[0].name, TTab[204]);
        if (TTab[205] != NULL)
            NuStrCpy(Game.customiser_start[1].name, TTab[205]);
        FinishWeirdoNames(-1);
    }
    if (id_DEFAULTCHARACTER[0] != -1)
        Game.character_save[id_DEFAULTCHARACTER[0]] |= 3;
    if (id_DEFAULTCHARACTER[1] != -1)
        Game.character_save[id_DEFAULTCHARACTER[1]] |= 3;
    if (PermanentWeirdo1IdPc != -1) {
        Game.character_save[PermanentWeirdo1IdPc] |= 3;
        if (CDataList != NULL)
            CDataList[PermanentWeirdo1IdPc].name_id = 204;
    }
    if (PermanentWeirdo2IdPc != -1) {
        Game.character_save[PermanentWeirdo2IdPc] |= 3;
        if (CDataList != NULL)
            CDataList[PermanentWeirdo2IdPc].name_id = 205;
    }
    Game.customiser_start[0].use_saved_name = 1;
    Game.customiser_start[1].use_saved_name = 1;
    for (i = 0; i < 10; ++i) {
        if (SuitRecordInitPc[i].group != previous_group)
            Game.initial_store_pack_flags |= 1u << i;
        previous_group = SuitRecordInitPc[i].group;
    }
}

/* Add to actual current game.cpp prefix only after root source/header/data grant.
 * Source composition, not a compiled/ABI/emitted-output acceptance.
 * Genuine subsystem suppliers are kept in their original owners.
 * All local callbacks are retained solely by the true publisher below. */
extern "C" i32 __cdecl Hub_PanelBusy(void);
extern "C" void __cdecl Hub_ResetPanel(void);

extern "C" i32 __cdecl ObjZappedBlue(GAMEOBJECT *);
extern "C" void __cdecl ResetShadowMapRenderingFn(void);
extern "C" void __cdecl EnableShadowMapRenderingFn(void);

#define GAME_PUBLISHER_ASSERT(expression)                                                                              \
    typedef char SAGA_JOIN(game_publisher_static_assert_, __LINE__)[(expression) ? 1 : -1]

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/sound-fade-callback.cppfrag
/* Uncompiled true original legogame/game.cpp static callback, published by
 * InitGameAfterConfig42e7fa to924dec. Full native429740..4299b5/629.
 * PC87afa4 is canonical BATTLEOVERCORUSCANT_ADATA, not an AA name guess.
 * Native never reads its passed WORLDINFO argument; it reads canonical WORLD.
 * No default state is written on the miss path. */
static i32 __cdecl SetSoundFadeDistCallBackFn_LSW(WORLDINFO_s *) {
    LEVELDATA_PC *level = WORLD->current_level;
    AREA_DATA_PC *area = WORLD->area;
    if (level == KAMINOE_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 22.5f;
    } else if (area != NULL && (area == GUNSHIP_ADATA || area == BONUS_GUNSHIP_ADATA)) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 125.0f;
    } else if (area != NULL && area == BATTLEOVERCORUSCANT_ADATA) {
        nusound_fade_start = 100.0f;
        nusound_fade_end = 250.0f;
    } else if (level == DEATHSTARRESCUEE_LDATA) {
        nusound_fade_start = 5.0f;
        nusound_fade_end = 50.0f;
    } else if (area != NULL && area == DEATHSTARESCAPE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 15.0f;
    } else if (area != NULL && area == ASTEROIDCHASE_ADATA) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 150.0f;
    } else if (area != NULL && area == JABBASPALACE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITA_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITC_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 55.0f;
    } else if (area != NULL && area == SPEEDERCHASE_ADATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 50.0f;
    } else if (level == ENDORBATTLEA_LDATA) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 12.0f;
    } else if (level == ENDORBATTLEB_LDATA || level == ENDORBATTLEC_LDATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 45.0f;
    } else if (level == ENDORBATTLED_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 12.0f;
    } else if (area != NULL && area == DEATHSTARBATTLE2_ADATA) {
        nusound_fade_start = 50.0f;
        nusound_fade_end = 200.0f;
    } else if (area != NULL && (area == E1CHARACTER_ADATA || area == E2CHARACTER_ADATA || area == E3CHARACTER_ADATA ||
                                area == BONUSKAMINO_ADATA || area == BONUSDAGOBAH_ADATA)) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 40.0f;
    } else {
        return 0;
    }
    return 1;
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/callback-contract-v1/disguise-adjust-source.cppfrag
static void DisguiseAdjust_LSW(i32 character_id, i32 hat, NUVEC *scale, NUVEC *offset) {
    *scale = GizmoBlowupUnitScalePc;
    *offset = v000;

    if (character_id == (i16)id_TAUNTAUN) {
        scale->x = 2.2f;
    } else if (character_id == (i16)id_DEWBACK) {
        scale->x = 4.6f;
    } else if (character_id == (i16)id_BANTHA) {
        scale->x = 3.8f;
    } else if (character_id == (i16)GameObjectRotationAtstId) {
        scale->x = 5.1f;
    } else if (character_id == (i16)id_SNOWMOB) {
        scale->x = 2.25f;
    } else if (character_id == (i16)id_MOONCAR || character_id == (i16)id_MAPCAR) {
        scale->x = 2.3f;
    } else if (character_id == (i16)id_ATAT) {
        scale->x = 7.0f;
    } else if (character_id == (i16)VehicleBoltId7f18b4Pc) {
        scale->x = 3.0f;
    } else if (character_id == (i16)id_XWING) {
        scale->x = 10.0f;
    } else if (character_id == (i16)id_YWING) {
        scale->x = 6.5f;
    } else if (character_id == (i16)VehicleBoltId7f1a24Pc) {
        scale->x = 4.0f;
    } else if (character_id == (i16)VehicleBoltId7f19a4Pc) {
        scale->x = 10.0f;
    } else if (character_id == (i16)id_TIEFIGHTER || character_id == (i16)VehicleBoltId7f1a28Pc ||
               character_id == (i16)VehicleBoltId7f196cPc || character_id == (i16)id_TIEBOMBER) {
        scale->x = 6.0f;
    } else if (character_id == (i16)VehicleBoltId7f1a2cPc || character_id == (i16)VehicleBoltId7f195cPc) {
        scale->x = 8.0f;
    } else if (character_id == (i16)id_WOOKIEFLYER) {
        scale->x = 6.0f;
    } else if (character_id == (i16)FreePlayId007f1a98Pc) {
        scale->x = 5.0f;
    } else if (character_id == (i16)FreePlayId007f1b40Pc || character_id == (i16)FreePlayId007f1b44Pc ||
               character_id == (i16)id_ANAKINSNEWPOD || character_id == (i16)id_ANAKINSNEWPODGREEN ||
               character_id == (i16)id_NEW_REPUBLIC_GUNSHIP || character_id == (i16)id_NEW_REPUBLIC_GUNSHIP_GREEN) {
        scale->x = 6.0f;
    } else if (character_id == (i16)VehicleBoltId7f1a9cPc || character_id == (i16)VehicleBoltId7f1aa0Pc) {
        scale->x = 7.0f;
    } else if (character_id == (i16)VehicleBoltId7f1b34Pc || character_id == (i16)VehicleBoltId7f1b38Pc) {
        scale->x = 5.0f;
    } else if (character_id == (i16)id_SEBULBASPOD) {
        scale->x = 8.0f;
    }

    if (hat == 5 || hat == 6) {
        offset->z = -0.03f;
    } else if (character_id == (i16)id_SNOWTROOPER || character_id == (i16)id_DEATHSTARTROOPER) {
        offset->z = -0.00999f;
    } else if (GCDataList[character_id].weapon_action == 1) {
        offset->z = -0.03f;
    } else if (character_id == (i16)id_GONKDROID) {
        offset->z = -0.0075f;
    } else if (character_id == (i16)id_IMPERIALSPY || character_id == (i16)FreePlayId007f199cPc ||
               character_id == (i16)id_GREEDO || character_id == (i16)id_BOSSK || character_id == (i16)id_CANTINABAND) {
        offset->z = -0.03f;
    } else if (character_id == (i16)id_IMPERIALGUARD) {
        offset->z = 0.015f;
    } else if (character_id == (i16)id_UGNAUGHT) {
        offset->z = -0.03f * 0.75f;
    }

    if (scale->x != 1.0f) {
        scale->z = scale->x;
        scale->y = scale->x;
    }
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/callback-contract-v1/object-policy-callbacks.cppfrag
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
GAME_PUBLISHER_ASSERT(offsetof(SUIT_LAYERS_PC, flags) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTERDATA_PC, model_flags) == 4);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_e33) == 0xe33);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);

static i32 CanMagnetClimb_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & 0x40) != 0;
}

static u32 CanPushObstacles_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0)
        return 1;
    return (object->character_data->game_character->flags_090 & 0x00800000) != 0;
}

static u32 CanSuperCarry_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0)
        return 1;
    return (object->character_data->game_character->flags_090 & 0x00800000) != 0;
}

static u32 CanPushBlocks_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0 || StartupGameFlag1 != 0)
        return 1;
    return (object->character_data->game_character->flags_090 & 0x00800000) != 0;
}

static i32 CanGlide_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & 2) != 0;
}

static i32 UsingExtraActions_Game(GAMEOBJECT *object) {
    return object->id == (u16)id_GRIEVOUS && ((object->flags_e20 & 0x00010000) == 0 || object->field_e33 == 2);
}

static i32 IsWearingBackPack_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & 0x100) != 0;
}

static i32 Fighting_WeaponInAction_Game(GAMEOBJECT *object) {
    CHARACTERDATA_PC *data = object->character_data;
    return data->game_character->weapon_action == 0 && (data->model_flags & 0x80) != 0 ? 126 : 16;
}

static i32 Fighting_WeaponOutAction_Game(GAMEOBJECT *object) {
    CHARACTERDATA_PC *data = object->character_data;
    return data->game_character->weapon_action == 0 && (data->model_flags & 0x80) != 0 ? 127 : 17;
}

static i32 FindSlamOrigin_UseCPos(GAMEOBJECT *object) {
    return object->id == (u16)id_GRIEVOUS;
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/callback-contract-v1/cutscene-accept-source.cppfrag
/* Reuse the canonical serialized clip declaration shared with gcutscn. */
GAME_PUBLISHER_ASSERT(sizeof(CUT_PLAYER_CLIP_CONFIG_PC) == 0x44);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, level_id) == 0);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, type) == 2);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, guest_episode) == 3);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, name) == 4);
GAME_PUBLISHER_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);
GAME_PUBLISHER_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);

static i32 CutScenePlayer_Accept(CUT_PLAYER_CLIP_CONFIG_PC *clip) {
    if (LOSTTEMPLE_ADATA != NULL && LDataList[clip->level_id].area_index == LOSTTEMPLE_ADATA->index)
        return 0;
    return 1;
}

// Source lease: build/analysis/startup-combat-hold-next/v1-contract/combat-game.cppfrag
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, translation) == 0x30);
typedef struct buck_rotation_reader_pc_s {
    u8 unknown_0000[0x27a];
    u16 rotation;
} BUCK_ROTATION_READER_PC;
GAME_PUBLISHER_ASSERT(offsetof(BUCK_ROTATION_READER_PC, rotation) == 0x27a);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
GAME_PUBLISHER_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);

static void ExtraHurtSfx_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_CHEWBACCA && CheatC3poBitsEnabledPc != 0)
        PlaySfx("C3_Death", &object->collision_position);
}

static void ExtraDieSfx_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_CHEWBACCA && CheatC3poBitsEnabledPc != 0)
        PlaySfx("C3_Death", &object->collision_position);
}

static void BuckStartExtra_LSW(GAMEOBJECT *object) {
    if (WorldInfo_CurrentlyActive()->current_level != LevelFixupSlotPc0087b218 || object->id != (u16)id_SNOWMOB)
        return;
    for (i32 i = 0; i < 8; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player == NULL || (player->flags_1fc_word & 0x1001) != 0x1001 || player->dead != 0 || player->rider != NULL)
            continue;
        NUVEC delta;
        NuVecSub(&delta, &player->lower_position, &object->locator_matrices[1].translation);
        if (delta.y > BuckSnowmobMinYPc && delta.y < BuckSnowmobMaxYPc) {
            f32 radius = BuckSnowmobRadiusPc;
            if ((f64)delta.x * delta.x + (f64)delta.z * delta.z < (f64)radius * radius) {
                NUVEC destination;
                NuVecRotateY(&destination, &BuckSnowmobForwardPc, ((BUCK_ROTATION_READER_PC *)object)->rotation);
                NuVecAdd(&destination, &destination, &((GAMEOBJECT *)Player[i])->collision_position);
                f32 height = GameShadow(NULL, &destination, 5.0f, -1);
                if (height != 2000000.0f)
                    destination.y = height;
                StartBigJump((GAMEOBJECT *)Player[i], &destination, 0, BuckSnowmobJumpHeightPc,
                             BuckSnowmobJumpDurationPc, 1);
            }
        }
    }
    GameAudio_PlaySfx(4, &object->collision_position, 0, 0);
}

static i32 BoltInitSfx_LSW(GAMEOBJECT *object) {
    if (object != NULL && object->id == (u16)id_GEONOSIAN && (object->flags_ef0 & 0x200) == 0)
        return GameAudio_GetSfxId(0x49);
    return -1;
}

static void GameObjectDimensionsExtra_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_ATAT && object->character_context == 0x17 && object->field_7aa == 0x41)
        object->bounds_radius += object->bounds_radius;
}

static i32 Punch_GetDamage_LSW(GAMEOBJECT *attacker, GAMEOBJECT *target) {
    if (attacker->character_data->game_character->weapon_action == 7 && Cheat_IsOn(0x0c)) {
        GameAudio_PlaySfx(0x4a, target != NULL ? &target->collision_position : &attacker->collision_position, 0, 0);
        GameCam_NewShake(NULL, 0.75f, 0.75f, 1.0f);
        return -1;
    }
    if (target != NULL && (target->id == (u16)id_IMPERIALGUARD || target->id == (u16)id_GAMORREANGUARD) &&
        (target->character_context == 0x0c || target->character_context == 0x18))
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
    return 1;
}

static void Punch_HitHold(GAMEOBJECT *attacker, GAMEOBJECT *target) {
    if (attacker != NULL && target != NULL &&
        (target->id == (u16)id_IMPERIALGUARD || target->id == (u16)id_GAMORREANGUARD))
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
}

static void Punch_HitExtraCode_LSW(GAMEOBJECT *object, NUVEC *position) {
    if (object->id == (u16)id_GAMORREANGUARD &&
        ((object->character_context == 0x26 && object->field_7aa == 0x56) || object->character_context == 0x0d)) {
        NewRumbleAllPlayers(0.4f, 0.0f, 1, 0.0f);
        NewRumble(object->pad->bound_pad, 0.7f, 0);
        GameCam_Judder(GameCam, 0.25f, 0, &object->collision_position);
        PlaySfx("fs_gamorr_land", &object->lower_position);
    } else if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) != 0) {
        if (position != NULL)
            PlaySfx("WhipHit", position);
        else
            PlaySfx("WhipNowt", &object->collision_position);
    }
}

static i32 CanStartHold_Game(GAMEOBJECT *object) {
    if (StartupGameFlag1 != 0 && StartupGameFlag0 == 0)
        return 1;
    if ((object->character_data->game_character->flags_090 & 0x20000000) != 0 && Batarang_StartTargetting(object))
        return 0;
    return 2;
}

static i32 Slam_GetDebris_Game(GAMEOBJECT *object, i32 effect) {
    if (object->field_280 != 0xff) {
        if (Cheat_IsOn(0x19))
            return 1;
        if (object->field_280 != 0xff && Player_HasPurpleForce(object))
            return 4;
    }
    if (object->id == (u16)id_GRIEVOUS)
        return (qrand() >= 0x8000) + 2;
    return effect == -1 ? 2 : effect;
}

// Source lease: build/analysis/startup-combat-hold-next/v1-contract/player-clear-context-held.cppfrag
static void Player_ClearContext_Game(GAMEOBJECT *object, i32 release_takeover) {
    ReleaseForce(object, 0);
    ReleaseEat(object);
    ReleaseBuildIt(object, 0);
    ReleasePush(object);
    if (release_takeover != 0)
        ReleaseTakeOver(object, 1);
}

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/player-movement-callbacks-v1/closed7.cppfrag
typedef struct game_callback_weapon_flags_pc_s {
    u8 unknown_00[0xe22];
    u8 weapon_flags;
} GAME_CALLBACK_WEAPON_FLAGS_PC;
GAME_PUBLISHER_ASSERT(offsetof(GAME_CALLBACK_WEAPON_FLAGS_PC, weapon_flags) == 0xe22);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_e33) == 0xe33);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, surface_normal) + offsetof(NUVEC, y) == 0xf28);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
GAME_PUBLISHER_ASSERT(sizeof(SUIT_INITIALIZER_RECORD_PC) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, character_id) == 0x14);

static i32 __cdecl Game_IgnoreInput(void) {
    return newgamecam != 0;
}

typedef struct CHECK_RESET_WORLD_VIEW_PC {
    u8 unknown_000[0x118];
    i32 reset_flags;
    u8 unknown_11c[0x4678 - 0x11c];
    TRIGGER_SYSTEM_PC *ai_trigger_set_sys;
} CHECK_RESET_WORLD_VIEW_PC;
GAME_PUBLISHER_ASSERT(offsetof(CHECK_RESET_WORLD_VIEW_PC, reset_flags) == 0x118);
GAME_PUBLISHER_ASSERT(offsetof(CHECK_RESET_WORLD_VIEW_PC, ai_trigger_set_sys) == 0x4678);
typedef struct CHECK_RESET_PROGRESS_VIEW_PC {
    SCENEPROGRESS scene_progress;
    u32 flags;
    u8 unknown_2804[0x2c24 - 0x2804];
    u8 giz_flow_progress[1];
} CHECK_RESET_PROGRESS_VIEW_PC;
GAME_PUBLISHER_ASSERT(offsetof(CHECK_RESET_PROGRESS_VIEW_PC, flags) == 0x2800);
GAME_PUBLISHER_ASSERT(offsetof(CHECK_RESET_PROGRESS_VIEW_PC, giz_flow_progress) == 0x2c24);
typedef struct CHECK_RESET_MISSION_VIEW_PC {
    u8 unknown_00[8];
    TIMER timer;
} CHECK_RESET_MISSION_VIEW_PC;
GAME_PUBLISHER_ASSERT(offsetof(CHECK_RESET_MISSION_VIEW_PC, timer) == 8);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, trafficanim_sys) == 0x5168);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, pulses_sys) == 0x5050);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, progress_flag_5174) == 0x5170);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 2 * sizeof(void *) == 0x70);
GAME_PUBLISHER_ASSERT(offsetof(AREA_GLOBALS_PC, area_phase) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, spawn_protection_timer) == 0x1000);

extern "C" {
    void __cdecl SetTexAnimSignals(void);
    void __cdecl Bolts_Reset(void);
    void __cdecl Batarangs_Reset(void);
    void __cdecl Detonators_Reset(void);
    void __cdecl ResetParts(void);
    void __cdecl Panel_Clear(void);
    void __cdecl Hint_Reset(void);
    void __cdecl TrafficAnimSys_Reset(TRAFFICANIMSYS_s *system);
    void __cdecl Faders_Reset(WORLDINFO *world);
    void __cdecl GizmoSysReset(void *system, void *world, i32 progress_index);
    void __cdecl ClearAICreatures(void);
    void __cdecl CharPlatforms_Reset(CHARPLATFORMSYS_s *system);
    void __cdecl ResetRepeatSfx(void);
    void __cdecl SetSoundFadeDist(WORLDINFO *world, OPTIONSSAVE *options);
    void __cdecl CutScene_StartAudio(void);
    void __cdecl Tag_SetMode(i32 mode);
    void __cdecl ResetGizFlow(GIZFLOW_PC *system, GIZFLOWPROGRESS_PC *progress);
    extern const char ActionSnapPartyName[6];
    extern char AreaFixUpName_Jedi[8];
    extern i32 makebaddiesforgetinresetbits;
    extern i32 FalconDebKey[2];
    extern i32 reset_reimport;
    extern i32 come_from_an_editor;
    extern f32 VehicleAreaRememberSpeed;
}

extern "C" i32 __cdecl GetRandomCoinType(void);
extern "C" void __cdecl GameFog_Reset(void);

extern "C" void __cdecl InitGameMode(void) {
    i32 i;
    f32 speed;
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    if ((WORLD->current_level->flags & 2) != 0)
        MenuReset();
    for (i = 0; i < 8; ++i) {
        if (players[i] != NULL && (players[i]->flags_ef0 & 0x01000000u) == 0)
            ResetPlayer(players[i], 1, NULL, 1);
    }
    speed = GetVehicleAreaRememberSpeed();
    if (VehicleAreaRememberSpeed < speed)
        VehicleAreaRememberSpeed = speed;
    AveragePlayerCurrentSpeedMul();
    SetPlayer();
    AreaGlobals_PC.progress_flags = WORLD->area != NULL ? AreaGlobals_PC.area_phase : 0;
    AreaGlobals_PC.field_20 = AreaGlobals_PC.field_1c;
    NewLData = NULL;
    NewMode = 0;
    for (i = 0; i < 2; ++i) {
        if (players[i] != NULL && players[i]->coin_packet != NULL) {
            GIZMO_PICKUP_TYPE_PC *type = &GizmoPickupType[GetRandomCoinType()];
            PLAYER_COIN_PACKET_PC *coins = (PLAYER_COIN_PACKET_PC *)players[i]->coin_packet;
            coins->last_coin = type->first_model_id;
            if (type->random_model_count != 0)
                coins->last_coin += (i16)(qrand() / (0xffff / type->random_model_count + 1));
        }
    }
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        if ((Obj[i].flags_1fc_word & 0x1001) == 0x1001)
            Obj[i].flags_ef0 |= 0x80;
    }
    GameFog_Reset();
    ConfigureComplexShadow(NULL);
}

extern "C" void __cdecl CheckResetBits(void) {
    i32 i;
    GAMEOBJECT **player_slot;
    char script_name[32];
    if ((ResetBits & 0x20) != 0 && WORLD->current_level->area_level_index != -1)
        ClearLevelProgress(WORLD->current_level->area_level_index, WORLD);
    Cheats_Reset();
    if (WORLD->level_progress != NULL) {
        ResetScene(WORLD->current_gscn, &WORLD->level_progress->scene_progress);
        ((CHECK_RESET_PROGRESS_VIEW_PC *)WORLD->level_progress)->flags |= 1;
    }
    GizmoBlowupVisibilityOverrides(WORLD);
    texanimbits = 0;
    SetTexAnimSignals();
    SetTexAnimSignals();
    CutScenes_Reset(WORLD);
    if (NOSOUND == 0)
        NuSound3Update();
    if ((ResetBits & 0x40) == 0)
        InitGameMode();
    if (NOSOUND == 0)
        NuSound3Update();
    STUBBED_NAMED("CheckResetBits: Customiser_SetUpCharacterData(CharacterCustomiser) at 0x00424500");
    Surfaces_Reset();
    ResetStreaks();
    Bolts_Reset();
    Batarangs_Reset();
    Detonators_Reset();
    ResetParts();
    ResetExplosions();
    ShoveObjectSysReset();
    Panel_Clear();
    GameCam_Reset(GameCam);
    ResetGameMessages();
    STUBBED_NAMED("CheckResetBits: Tag_ResetTransfers at 0x0059fe10");
    Hint_Reset();
    Hint_CancelCurrent();
    Teleports_Reset(WORLD);
    TrafficAnimSys_Reset(WORLD->trafficanim_sys);
    Pulses_Reset((LEVEL_SFX_PULSE_PC *)WORLD->pulses_sys);
    NuSound3StopRumble();
    ResetTimer(&GameTimer, 0.0f);
    ResetTimer(&GamePlayTimer, 0.0f);
    ResetTimer(&PauseTimer, 0.0f);
    ResetTimer(&JoinInTimer, 0.0f);
    if ((ResetBits & 0x20) != 0 && MissionSys != NULL)
        ResetTimer(&((CHECK_RESET_MISSION_VIEW_PC *)MissionSys)->timer, 0.0f);
    Hint_SetHintFromId(-1, 0, 0);
    ResetRepeatSfx();
    DEFAULT_MOVE_RANGE = 0.0f;
    alert_obj = NULL;
    alert_timer = 0.0f;
    drop_back_in_timer = 0.0f;
    ResetRippleSet(RippleSetPc);
    STUBBED_NAMED("CheckResetBits: Grabber_Reset(WORLD) at 0x004710d0");
    Faders_Reset(WORLD);
    TerrainChatterWaitPc = 3.0f;
    TerrainLastChatterSfxPc = -1;
    tieonsfxwait = 1.0f;
    tieoffsfxwait = 1.0f;
    FalconDebKey[0] = -1;
    FalconDebKey[1] = -1;
    LevelNameTime = 0.0f;
    LevelNameMul = LevelChange != 0 ? 3.0f : 0.0f;
    memset(&LevelLightingDataPc, 0, sizeof(LevelLightingDataPc));
    if (VehicleArea != 0) {
        if (WORLD->current_level == ASTEROIDCHASED_LDATA || WORLD->current_level == DEATHSTAR2BATTLEA_LDATA) {
            TargetDist_Near2 = 6.25f;
            TargetDist_Mid2 = 100.0f;
            TargetDeg_Near = 0x38e3;
            TargetDeg_Mid = 0x31c7;
            TargetDeg_Far = 0x2aaa;
        } else {
            TargetDist_Near2 = 6.25f;
            TargetDist_Mid2 = 100.0f;
            TargetDeg_Near = 0x2aaa;
            TargetDeg_Mid = 0x1c71;
            TargetDeg_Far = 0x0e38;
        }
    } else {
        TargetDist_Near2 = 0.25f;
        TargetDist_Mid2 = 1.0f;
        TargetDeg_Near = 0x2aaa;
        TargetDeg_Mid = 0x2000;
        TargetDeg_Far = 0x1555;
    }
    party_under_cover = 0;
    nbaddies_can_see_players = 0;
    if (reset_restart != 0) {
        LevTime[0] = 0.0f;
        LevTime[1] = 0.0f;
        BonusCoinTotal = 0;
    }
    if (WORLD->object_sys != NULL)
        WORLD->object_sys->flags &= ~1;
    if ((ResetBits & 0x20) != 0)
        GizmoSysClearLevelProgress(WORLD, WORLD->current_level->area_level_index);
    GameAnimSys_ReStoreProgress(WORLD->game_anim_sys, WORLD->current_level->area_level_index);
    GizmoSysReset(WORLD->gizmo_sys, WORLD, WORLD->current_level->area_level_index);
    if ((ResetBits & 1) != 0) {
        DrawBossHitPoints(NULL);
        ResetForceBack();
        ClearAICreatures();
        GameAISysReset(WORLD->ai_sys);
        ReStoreStatusTakeOverObjectSys(1);
        AIScriptInitConditions(WORLD->ai_sys);
        for (player_slot = (GAMEOBJECT **)Player; player_slot < (GAMEOBJECT **)Player + 8; ++player_slot) {
            if (*player_slot == NULL)
                continue;
            InitPlayerAI(*player_slot);
            if (FreePlay != 0) {
                if (AIScriptFind(WORLD->ai_sys, "Freeplay", 0, 1, 0) != NULL)
                    sprintf(script_name, "Freeplay");
                else if (AIScriptFind(WORLD->ai_sys, ActionSnapPartyName, 0, 1, 0) != NULL)
                    sprintf(script_name, ActionSnapPartyName);
                else
                    sprintf(script_name, "GeneralParty");
            } else if (Mission_Active(NULL) != NULL) {
                if (AIScriptFind(WORLD->ai_sys, "Mission", 0, 1, 0) != NULL)
                    sprintf(script_name, "Mission");
                else
                    goto party_script;
            } else if (AIScriptFind(WORLD->ai_sys, (*player_slot)->character_data->name, 0, 1, 0) != NULL) {
                sprintf(script_name, (*player_slot)->character_data->name);
            } else {
                if (((*player_slot)->character_data->model_flags & 8) != 0)
                    sprintf(script_name, AreaFixUpName_Jedi);
                else if (((*player_slot)->character_data->model_flags & 0x80) != 0)
                    sprintf(script_name, "Blaster");
                else
                    sprintf(script_name, "NoWeapon");
                if (AIScriptFind(WORLD->ai_sys, script_name, 0, 1, 0) == NULL) {
                party_script:
                    if (AIScriptFind(WORLD->ai_sys, ActionSnapPartyName, 0, 1, 0) != NULL)
                        sprintf(script_name, ActionSnapPartyName);
                    else
                        sprintf(script_name, "GeneralParty");
                }
            }
            AIScriptProcessorInit(WORLD->ai_sys, &(*player_slot)->ai_packet,
                                  &(*player_slot)->ai_packet.script_processor, NULL, script_name, NULL, 1, NULL, NULL);
            if (FreePlay == 0 && Mission_Active(NULL) == NULL && (*player_slot)->rider == NULL &&
                AIScriptSetBaseScriptStateByName(&(*player_slot)->ai_packet.script_processor,
                                                 (char *)GameObjectState_InActive) != 0)
                (*player_slot)->flags_1fc_word &= ~0x1000;
        }
    } else {
        ReStoreStatusTakeOverObjectSys(0);
    }
    if (makebaddiesforgetinresetbits != 0)
        MakeBaddiesForgetAboutParty(0);
    if ((ResetBits & 2) != 0) {
        for (i = 0; i < 8; ++i) {
            if (Player[i] != NULL)
                ((GAMEOBJECT *)Player[i])->saved_hitpoints = 4;
        }
    }
    if ((ResetBits & 4) != 0) {
        for (i = 0; i < 8; ++i) {
            if (Player[i] != NULL) {
                if ((((GAMEOBJECT *)Player[i])->flags_1fc & 0x80) != 0)
                    ((GAMEOBJECT *)Player[i])->spawn_protection_timer = 2.5f;
                else
                    ((GAMEOBJECT *)Player[i])->spawn_protection_timer = 0.0f;
            }
        }
    }
    ResetRadios();
    SpecialMiniKits_Reset(WORLD);
    STUBBED_NAMED("CheckResetBits: SuperCounters_FixUpGizmos(WORLD) at 0x00573690");
    AITriggerSetSysReset(((CHECK_RESET_WORLD_VIEW_PC *)WORLD)->ai_trigger_set_sys);
    AITriggerSysAutoSetUp(WORLD, (AITRIGGERSETSYS_s *)((CHECK_RESET_WORLD_VIEW_PC *)WORLD)->ai_trigger_set_sys);
    if ((ResetBits & 0x10) != 0) {
        for (player_slot = (GAMEOBJECT **)Player; player_slot < (GAMEOBJECT **)Player + 8; ++player_slot) {
            if (*player_slot != NULL)
                STUBBED_NAMED("CheckResetBits: ResetPlayer(player, 1, NULL, 0) at 0x0041b420");
        }
        VehicleAreaRememberSpeed = GetVehicleAreaRememberSpeed();
    }
    LevHSpecialExists = 0;
    for (i = 0; i < 88; ++i) {
        if (NuSpecialExistsFn(&LevHSpecial[i]) != 0)
            LevHSpecialExists |= (u64)1 << i;
    }
    WORLD->blowup_target_candidate_count = 0;
    if (WORLD->current_level->callbacks[2] != NULL)
        ((void(__cdecl *)(WORLDINFO *))WORLD->current_level->callbacks[2])(WORLD);
    CharPlatforms_Reset((CHARPLATFORMSYS_s *)WORLD->shadow_platform_list_pc);
    SetSoundFadeDist(WORLD, &Game.options_save);
    TempOptions.master_volume = Game.options_save.master_volume;
    TempOptions.sound_volume = Game.options_save.sound_volume;
    TempOptions.music_volume = Game.options_save.music_volume;
    if (WORLD->level_progress != NULL && (((CHECK_RESET_PROGRESS_VIEW_PC *)WORLD->level_progress)->flags & 2) == 0 &&
        WORLD->camera_splines[1] != NULL && gone_through_door_to_new_mode == 0 && gone_through_door_to_new_level == 0 &&
        come_from_an_editor == 0)
        GameCameraMakeMiniCut(WORLD->camera_splines[1], 0.0f, 2.0f, 0.0f, 1.0f, 0, 1);
    gone_through_door_to_new_mode = 0;
    come_from_an_editor = 0;
    if (newmode_cutinfo != NULL) {
        NewCutScene((CUTINFO *)newmode_cutinfo, WORLD->cutscene_sys, NULL, 0);
        newmode_cutinfo = NULL;
    }
    if (BonusArea != 0)
        Cheats_TurnOff(1);
    STUBBED_NAMED("CheckResetBits: BonusCoinTarget = TotalLevelCoinTally(WORLD, NULL x7) at 0x0046c350");
    if (BonusCoinTarget > 1000000)
        BonusCoinTarget = 1000000;
    if (netclient != 0)
        BonusCoinTarget = 1000000;
    ResetBits = 0;
    reset_restart = 0;
    reset_reimport = 0;
    CutScene_StartAudio();
    if (NOSOUND == 0)
        NuSound3Update();
    ((CHECK_RESET_WORLD_VIEW_PC *)WORLD)->reset_flags = 1;
    LookAtBoth = 0;
    oneAtOnce_SetNumAttackers(1);
    if (WORLD->level_progress != NULL) {
        if ((((CHECK_RESET_PROGRESS_VIEW_PC *)WORLD->level_progress)->flags & 2) != 0)
            WORLD->progress_flag_5174 = (((CHECK_RESET_PROGRESS_VIEW_PC *)WORLD->level_progress)->flags >> 2) & 1;
    }
    if (WORLD->level_progress != NULL)
        ResetGizFlow(WORLD->giz_flow,
                     (GIZFLOWPROGRESS_PC *)((CHECK_RESET_PROGRESS_VIEW_PC *)WORLD->level_progress)->giz_flow_progress);
    else
        ResetGizFlow(WORLD->giz_flow, NULL);
    EffectOffProgress_Reset(WORLD->level_progress);
    if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && WORLD->area == HUB_ADATA)
        Tag_SetMode(3);
    else if (StartupGameFlag1 != 0 && StartupGameFlag0 == 0)
        Tag_SetMode(1);
    else
        Tag_SetMode(2);
    if (WORLD->area != NULL && WORLD->area == JEDI_ADATA)
        WeaponInOut_NoAIJediSfx = 1;
    else
        WeaponInOut_NoAIJediSfx = 0;
    bonusmodearcade = 0;
    if (WORLD->area != NULL && (WORLD->area->flags & 0x10) != 0 &&
        AreaGlobals_PC.progress_flags < AreaGlobals_PC.area_phase) {
        AreaGlobals_PC.progress_flags = AreaGlobals_PC.area_phase;
        if (AreaGlobals_PC.area_phase > 10)
            AreaGlobals_PC.progress_flags = 10;
    }
}

static i32 __cdecl LastSafePosExtra(GAMEOBJECT *object) {
    LEVELDATA_PC *level = WORLD->current_level;
    if (level == LevelFixupSlotPc0087b1a8 && object->jump_animation_state == 0x0e)
        return 1;
    if (level == LevelFixupSlotPc0087b264 && object->jump_animation_state == 0x18 && (object->flags_1fc & 0x80) != 0 &&
        object->field_283 == -1)
        return 1;
    if (level == DEATHSTARESCAPEC_LDATA && (i16)object->id == id_SERVICECAR)
        return 1;
    return 0;
}

static i32 __cdecl Jump_PreventJump(GAMEOBJECT *object) {
    return WORLD->current_level == LevelFixupSlotPc0087b154 && object->floor_height != 2000000.0 &&
           object->surface_normal.y < NuTrigTable[0x371c];
}

static void __cdecl Jump_EndOfLandContext(GAMEOBJECT *object) {
    GAME_CALLBACK_WEAPON_FLAGS_PC *weapon = (GAME_CALLBACK_WEAPON_FLAGS_PC *)object;
    if ((i16)object->id == id_THEEMPEROR && object->character_context == LEGOCONTEXT_LAND_COMBOJUMP &&
        BigJumpLandPreferredPc != -1 && object->field_7aa == BigJumpLandPreferredPc &&
        ((weapon->weapon_flags & 1) == 0 || object->field_e33 == 2))
        ResetAnimPacket(&object->anim_packet, -1);
}

static void __cdecl BigJump_EndOfLand(GAMEOBJECT *object) {
    GAME_CALLBACK_WEAPON_FLAGS_PC *weapon = (GAME_CALLBACK_WEAPON_FLAGS_PC *)object;
    if ((i16)object->id == id_THEEMPEROR && BigJumpLandPreferredPc != -1 &&
        object->field_7aa == BigJumpLandPreferredPc && ((weapon->weapon_flags & 1) == 0 || object->field_e33 == 2))
        ResetAnimPacket(&object->anim_packet, -1);
}

static i32 __cdecl Player_ToggleSubCharacter_Game(GAMEOBJECT *object, i32 left, i32 sound) {
    i32 previous, selected;
    u32 available;
    SUIT_INITIALIZER_RECORD_PC *record;
    if (object->suit == NULL)
        return 0;
    previous = Suit_GetIndex(object->suit);
    available = areaSuitBits;
    selected = previous;
    record = &SuitRecordInitPc[selected];
    for (;;) {
        if (left != 0) {
            --selected;
            --record;
        } else {
            ++selected;
            ++record;
        }
        if (selected < 0 || selected == 10 || record->character_id != (i16)object->id || selected == previous)
            return 0;
        if ((available & (1u << ((u32)selected & 31))) != 0)
            break;
    }
    if (selected == -1)
        return 0;
    NewPlayerCharacter(object, (i16)object->id, -1, 1);
    if (sound != 0)
        GameAudio_PlaySfx(left != 0 ? 0x23 : 0x24, &object->collision_position, 0, 0);
    object->suit = (SUIT_LAYERS_PC *)&SuitRecordInitPc[selected];
    InitPlayerAI(object);
    return 1;
}

static void __cdecl Punch_HitExtraCode_alternate_PC(GAMEOBJECT *object, NUVEC *position) {
    if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) != 0) {
        if (position != NULL)
            PlaySfx("WhipHit", position);
        else
            PlaySfx("WhipNowt", &object->collision_position);
    }
}

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/player-movement-callbacks-v1/held2.cppfrag
/* Genuine proposed supplier definitions remain in their original speederchase/suit owners. */
extern "C" f32 __cdecl GetVehicleAreaRememberSpeed(void);
extern "C" SUIT_LAYERS_PC *__cdecl Suit_GetLast(i32, i32);

extern "C" {
    void __cdecl SetAnimBlendMode(i32 mode);
    void __cdecl GameRegisterGizActions(void);
    void __cdecl DrawAutoSaveIcon(void);
    void __cdecl InitPartTable(char **names);
    void __cdecl GameAudio_Reset(void);
    u8 __cdecl NuAnimSetForceEulerToQuat(u8 force);
    i32 __cdecl NuWindowsGetConfigValuePc(i32 index);

    u8 *Game_LevelSave = 0;
    MISSION_SAVE_READER_PC *Game_MissionSave = 0;
    TIMER OverallGamePlayTimer = {0};
    i32 GameDrawCharacterModelFlagPc = 1;
    char *partdebris_name[] = {"TRAINING_P",   "PART_MOUSE", "PART_BLACK_3", "DRAG_PART_1", "SMALL_PART_2", "RAT_PART",
                               "BOULDER_PART", "POPCORN",    "PART_THROW",   "ENG_POP",     "DOOKU_PART",   "MAUL_P_1",
                               "MAUL_P_2",     "MAUL_P_3",   "SPEAR_HIT",    "RED_BRICK",   "POD_ROCK",     NULL};
}

static void InitOptionsSaveDefaults(OPTIONSSAVE *options) {
    i32 aspect = NuVideoGetAspect();

    options->unknown_0000[0] = 0;
    options->unknown_0000[1] = 0;
    options->unknown_0000[2] = 0;
    options->music_volume = 10;
    options->sound_volume = 10;
    options->unknown_0007[0] = 0;
    options->unknown_0007[1] = 0;
    options->master_volume = 10;
    options->music_enabled = 1;
    options->widescreen = (aspect == 0 || aspect == 3) ? 0 : 1;
    options->brightness = 10;
    options->sound_volume = (u8)NuWindowsGetConfigValuePc(6);
    options->music_volume = (u8)NuWindowsGetConfigValuePc(5);
}

static i32 __cdecl MakeSaveHash(void) {
    return Game.save_hash;
}

extern "C" void __cdecl InitGameBeforeConfig(void) {
    if (PAL != 0) {
        NuStrCpy(SaveProductCodePc, "BESLES-54221");
        DEFAULTFRAMETIME = 0.02f;
    } else {
        NuStrCpy(SaveProductCodePc, "BASLUS-21409");
        DEFAULTFRAMETIME = ApplyGravityMinimumFrameTime;
    }
    FRAMETIME = DEFAULTFRAMETIME;
    DEFAULTFPS = 1.0f / DEFAULTFRAMETIME;
    DEFAULTFRAMETIME = ApplyGravityMinimumFrameTime;
    MAXFRAMETIME = 0.1f;
    SaveSystemInitialise(6, MakeSaveHash, &Game, sizeof(Game), 1, DrawAutoSaveIcon);
    Game_OptionsSave = &Game.options_save;
    Game_LevelSave = Game.unknown_0011;
    Game_AreaSave = (COLLECTION_AREA_SAVE_PC *)Game.area_save;
    EpisodeSavePc = (EPISODE_SAVE_READER_PC *)Game.episode_start;
    Game_CharacterSave = Game.character_save;
    CustomiserCompletionSavePc = (CUSTOMISER_COMPLETION_READER_PC *)&Game.save_hash;
    Game_MissionSave = &Game.mission_save;
    ResetSeeds();
    ResetTimer(&GlobalTimer, 0.0f);
    ResetTimer(&OverallGamePlayTimer, 0.0f);
    SetAnimBlendMode(2);
    SetAPIObjPlaySfxByIdFn(PlaySfxById);
    PCAnimationEffectCheckpoint();
    InitOptionsSaveDefaults(&Game.options_save);
    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    Text_GameSetLanguageFn = Text_SetLanguage_Game;
    if (Text_Language != 0 && Text_Language != 13 && PAL != 0) {
        Text_SetLanguage(NuLanguageGet());
    }
    GameRegisterGizActions();
    WorldInfo_Reset(&WorldInfo[0], -1);
    WorldInfo_Reset(&WorldInfo[1], -1);
    Game.purchased_extra_bits_high |= 0x1000;
    memset(&KillPartsZeroLightingPc, 0, sizeof(KillPartsZeroLightingPc));
    memset(GameCam, 0, sizeof(*GameCam));
    InitPartTable(partdebris_name);
    GameAudio_Reset();
    if (StartupGameFlag0 != 0) {
        NuAnimSetForceEulerToQuat(1);
    }
    GameDrawCharacterModelFlagPc = 0;
}

typedef struct game_door_callback_pc_s {
    u8 unknown_00[0xf0];
    u16 level_id;
} GAME_DOOR_CALLBACK_PC;
GAME_PUBLISHER_ASSERT(offsetof(GAME_DOOR_CALLBACK_PC, level_id) == 0xf0);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA_PC, index) == 0x62);

static void __cdecl GoThroughDoor_ExtraCode(WORLDINFO *world, GAME_DOOR_CALLBACK_PC *door) {
    VehicleAreaRememberSpeed = GetVehicleAreaRememberSpeed();
    if (world->current_level == VADERA_LDATA)
        STUBBED_NAMED("428d95: VaderA_GoneThroughDoor(world,door) native4e4220 VOID action unready");
    if (PODRACEB_LDATA != NULL && door->level_id == PODRACEB_LDATA->index && world->current_level == PODRACEA_LDATA) {
        if (Lap != 3) {
            STUBBED_NAMED("428e1b: tail PodRace_IncreaseLap() native4d9f90 VOID action unready");
        } else if (FreePlay != 0) {
            CompleteLevel(world);
        } else {
            NewLData = LevelFixupSlotPc0087b0b8;
            LevelCompletionFallbackFlagPC = 1;
            if (waiting_for_level != -1)
                waiting_for_new_level = 1;
        }
    }
}

typedef struct game_callback_height_bits_pc_s {
    u8 unknown_00[0xf08];
    u32 saved_state_f08_bits;
} GAME_CALLBACK_HEIGHT_BITS_PC;
GAME_PUBLISHER_ASSERT(offsetof(GAME_CALLBACK_HEIGHT_BITS_PC, saved_state_f08_bits) == 0xf08);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, doomed_escape_locator) == 0xf14);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, current_route) == 0x404);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, next_route) == 0x405);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
GAME_PUBLISHER_ASSERT(sizeof(AIPATHINFO_PC) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, frame_state) == 0x4c0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

static void __cdecl Player_ToggledCharacter_Game(GAMEOBJECT *object, i32 owned) {
    SUIT_LAYERS_PC *previous = object->suit;
    if ((i16)object->id != ID_DEFAULTCHARACTER_A && (i16)object->id != ID_DEFAULTCHARACTER_B) {
        object->suit = NULL;
    } else if (owned != 0) {
        object->suit = Suit_GetLast((i16)object->id, 1);
        if (object->suit == NULL)
            object->suit = Suit_GetDefault((i16)object->id);
    } else {
        object->suit = Suit_GetDefault((i16)object->id);
    }
    if (previous != object->suit) {
        GAME_CALLBACK_HEIGHT_BITS_PC *height = (GAME_CALLBACK_HEIGHT_BITS_PC *)object;
        u8 current_route = object->ai_packet.current_route;
        u8 next_route = object->ai_packet.next_route;
        AIPATHINFO_PC path_info = object->ai_packet.path_info;
        AILOCATOR_PC *escape_locator = object->doomed_escape_locator;
        u32 height_bits = height->saved_state_f08_bits;
        i32 frame_state = object->ai_packet.frame_state;
        u32 preserve_flag = object->flags_ef0 & 0x200000;
        InitPlayerAI(object);
        object->ai_packet.path_info = path_info;
        object->ai_packet.next_route = next_route;
        object->doomed_escape_locator = escape_locator;
        object->flags_ef0 = (object->flags_ef0 & ~0x200000u) | preserve_flag;
        object->ai_packet.current_route = current_route;
        height->saved_state_f08_bits = height_bits;
        object->ai_packet.frame_state = frame_state;
    }
}

// Source lease: build/analysis/publication-render-audio-next/source-contract-v2/private-type-proposal.hfrag
// Working reader views only; never establishes native allocation/capacity.
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, flags_word) == 0xb0);
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, status_flags) == 0xb4);
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, stage) == 0x124);
GAME_PUBLISHER_ASSERT(offsetof(STATUS_STAGE_s, type) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(STATUS_STAGE_s, field_0x18) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
GAME_PUBLISHER_ASSERT(offsetof(GAMECAMERA, sock_position.sock_index) == 1);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, nearest_opponent) == 0x39c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, nearest_opponent_metric) == 0x3a0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
// Declare only after sole-writer canonical audit. No backing supplied here.
extern i32 selectmodemode;
extern f32 selectmodetime;
extern i32 CruiserD_LiftChase;
// Original Android static game.cpp state; PC private visibility/TU unproved.
extern i32 deathstar_hold_count;
// External declarations only; final shared integration must use canonical C linkage.
extern i32(__cdecl *CheckMusicOtherFn)(void);
extern i32(__cdecl *GameAudio_ActionMusicFn)(void);
extern i32(__cdecl *PauseRenderOffFn)(void);
// Genuine unresolved value suppliers: cannot activate music family with substitutes.
i32 __cdecl Hub_Outside(void);
i32 __cdecl KaminoInside(void);
i32 __cdecl KaminoDiscoOn(void);
i32 __cdecl DeathStarShieldDown(void);
i32 __cdecl SarlaccPitDiscoActive(WORLDINFO *world);

// Source lease: build/analysis/publication-render-audio-next/source-contract-v2/render-bodies.cppfrag
// Append inside the real publisher/private cohort only after owner review.
static void __cdecl CutScene_OverrideConfigFileName_LSW(char *filename, i32 unused0, i32 unused1) {
    char *prefix = "episodei\\ep1_podrace_";
    const char *suffix;
    if (PODSPRINT_ADATA == NULL || WorldInfo_CurrentlyActive()->area != PODSPRINT_ADATA)
        return;
    if (NuStrIStr(filename, prefix) != filename)
        return;
    suffix = filename + NuStrLen(prefix);
    if (NuStrICmp(suffix, "arrival1") == 0 || NuStrICmp(suffix, "arrival2") == 0 ||
        NuStrICmp(suffix, "arrival3") == 0 || NuStrICmp(suffix, "arrival4") == 0 || NuStrICmp(suffix, "intro") == 0 ||
        NuStrICmp(suffix, "tuskenraiders") == 0 || NuStrICmp(suffix, "outro1") == 0 || NuStrICmp(suffix, "outro2") == 0)
        NuStrCat(filename, "_sprint");
}

static void __cdecl BackDrop_Alpha(f32 *alpha) {
    if (WORLD != NULL) {
        LEVELDATA_PC *level = WORLD->current_level;
        if (level == TITLES_LDATA) {
            *alpha *= newgamealpha;
            return;
        }
        if (level == CREDITS_LDATA && CreditsFlag == 3) {
            *alpha *= CreditsAlpha;
            return;
        }
        if ((level->flags & 0x400) != 0 && StatusPacket.status_flags != 0 && StatusPacket.stage->type == 12 &&
            (StatusPacket.flags_word & 0x1000) == 0) {
            const f32 time = StatusPacket.stage->field_0x18;
            *alpha = time < 0.5f ? 1.0f - (time + time) : 0.0f;
            return;
        }
    }
    if (GetMenuID() == 15 && selectmodemode == 4) {
        const f32 time = selectmodetime;
        *alpha = time < 0.5f ? 1.0f - (time + time) : 0.0f;
    }
}

static i32 __cdecl PauseRenderOff(void) {
    GetMenuID();
    return 0;
}

static i32 __cdecl MatrixReflection_CanOverride(void) {
    if (WORLD->current_level == DEATHSTARESCAPEA_LDATA && GameCam->sock_position.sock_index != 0 &&
        GameCam->sock_position.sock_index != 3)
        return 0;
    return 1;
}

static void __cdecl SurfaceInfo_ExtraReflect(GAMEOBJECT *object) {
    if (WORLD->current_level == LevelFixupSlotPc0087b170 && object->reflection_height == 2000000.0 &&
        object->position.x < 11.0f) {
        object->reflection_enabled = 3;
        object->reflection_height = -39.2f;
    }
    if (WORLD->current_level == LevelFixupSlotPc0087b1d0) {
        if (object->position.z > 20.75f) {
            object->reflection_enabled = 3;
            object->reflection_height = 22.4f;
        }
        if (object->position.x < -20.75f) {
            object->reflection_enabled = 1;
            object->reflection_height = -22.4f;
        }
    }
}

// Source lease: build/analysis/publication-render-audio-next/source-contract-v2/music-bodies.cppfrag
// Genuine private callback/helper group: retain within real game publisher TU.
// The five named value suppliers below are genuine unresolved providers, not stubs.
static i32 __cdecl ActionMusicSockChoicePc(void) {
    NUVEC position;
    SOCKPOSITION_PC socket_position;
    if (Players_AveragePos(&position, &socket_position) != 0) {
        const i8 socket = socket_position.sock_index;
        if (socket == 6 || socket == 2 || socket == 3 || socket == 4 || socket == 9)
            return 0;
        if (socket == 7 || socket == 8)
            return 1;
    }
    return -1;
}

static i32 __cdecl CheckMusicOther(void) {
    NUVEC position;
    SOCKPOSITION_PC socket_position;
    LEVELDATA_PC *level;
    if (Players_AveragePos(&position, &socket_position) == 0)
        return 0;
    level = WORLD->current_level;
    if (level == HUB_LDATA)
        return Hub_Outside() != 0;
    if (level == LevelFixupSlotPc0087b12c)
        return KaminoInside() != 0;
    if (level == KAMINOC_LDATA)
        return KaminoDiscoOn() != 0;
    if (level == KAMINOE_LDATA)
        return KaminoInside() == 0;
    if (level == LevelFixupSlotPc0087b1bc)
        return socket_position.sock_index == 3;
    if (level == DEATHSTARBATTLED_LDATA) {
        if (DeathStarShieldDown() != 0) {
            deathstar_hold_count = 30;
            return 1;
        }
        if (deathstar_hold_count > 0) {
            --deathstar_hold_count;
            return 1;
        }
    } else if (level == LevelFixupSlotPc0087b228) {
        return GameCam->sock_position.sock_index == 4;
    } else if (level == SARLACCPITB_LDATA) {
        return SarlaccPitDiscoActive(WORLD) != 0;
    }
    return 0;
}

static i32 __cdecl ActionMusicFn(void) {
    i32 i;
    if (StartupGameFlag0 != 0) {
        if (WORLD->area != NULL && WORLD->area == LOSTTEMPLE_ADATA && WORLD->current_level != LevelFixupSlotPc0087b2c4)
            return 1;
        return 0;
    }
    if (Arcade != 0 || DoubleScore != 0 || Cheat_PowerUpActive(-1) != 0 ||
        (WORLD->current_level == LevelFixupSlotPc0087b164 && MiniCutCam != 0) ||
        (WORLD->current_level == CRUISERD_LDATA && CruiserD_LiftChase != 0) ||
        WORLD->current_level == DEATHSTARRESCUEE_LDATA)
        return 1;
    if (WORLD->current_level == LevelFixupSlotPc0087b1bc && CheckMusicOther() != 0)
        return 1;
    if (WORLD->current_level == CLOUDCITYESCAPEA_LDATA)
        return 1;
    if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
        const i32 choice = ActionMusicSockChoicePc();
        if (choice != -1)
            return choice;
    }
    if (WORLD->current_level == HUB_LDATA)
        return ai_fighting != 0;
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL &&
            (player->ai_packet.opponent != NULL ||
             (player->ai_packet.nearest_opponent != NULL && player->ai_packet.nearest_opponent->dead == 0 &&
              player->ai_packet.nearest_opponent_metric < 3.0f)))
            return 1;
    }
    return 0;
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v1/game-private-three.cppfrag Same
// original game.cpp private callback/publisher group. No retention references.
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
GAME_PUBLISHER_ASSERT(offsetof(GAMECAMERA, sock_position.sock_index) == 0x1);

// Genuine mutable source-owned state, pending sole-writer canonical overlap review.
extern f32 minikittime;

static f32 __cdecl Hint_AlphaTarget(void) {
    if (minikittime > 0.0f && ChallengeMode == 0)
        return 0.0f;
    return 1.0f;
}

static i32 __cdecl GizmoBlowUp_NoTarget(WORLDINFO *world, GAMEOBJECT *object) {
    if (world->current_level == DEATHSTARRESCUEA_LDATA)
        return GameCam->sock_position.sock_index == 2;
    return 0;
}

static i32 __cdecl Tag_NoHiddenIcon(GAMEOBJECT *object) {
    if (WorldInfo_CurrentlyActive()->current_level == DEATHSTARESCAPEB_LDATA)
        return GameCam->sock_position.sock_index == 5;
    return 0;
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v2/message-and-pickup.cppfrag Source
// fragments only; the genuine private callback/publisher TU must be resolved.
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, score) == 0xdc);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, target_is_coin_total) == 0xfe);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, position_a) == 0x7c);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, position) == 0x94);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, has_start_position) == 0xf9);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE_PC, icon) == 0xe6);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
GAME_PUBLISHER_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
GAME_PUBLISHER_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);

static void __cdecl GameMsg_EndDelay_Game(GAME_MESSAGE_PC *message) {
    i32 random;
    if (message->score <= 0)
        return;
    if (VehicleArea != 0) {
        DrawBuildUpTime = (f32)((f64)COINMSGTIME + 1.0);
        return;
    }
    if (message->target_is_coin_total != 1)
        return;
    AddGameDebris(WORLD->game_debris_sys, 0x38, &message->position_a);
    if (WORLD->area != NULL && (WORLD->area->flags & 0x100) != 0) {
        random = qrand();
        message->position.x =
            (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.x);
        random = qrand();
        message->position.y =
            (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.y);
        random = qrand();
        message->has_start_position = 0;
        message->position.z =
            (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.z);
    }
}

static NUSPECIALHANDLE *__cdecl GameMsg_GetExtraObj(GAME_MESSAGE_PC *message) {
    if (message->icon == 0xd0)
        return &WORLD->level_objects[0xd1].special;
    return NULL;
}

static f32 __cdecl GizmoPickups_Collide2D(GAMEOBJECT *object) {
    if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == (u16)id_SPEEDERBIKE)
        return 2.0f;
    return 0.0f;
}

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v3/game-buildit.cppfrag
// Private callbacks stay with their genuine original Init publisher.
struct GIZBUILDIT_PC;
struct GAME_BUILDIT_READER_PC {
    u8 opaque_00[0x34];
    NUVEC position;
    u8 opaque_40[0x2c];
    u8 anim_object_count;
};
GAME_PUBLISHER_ASSERT(offsetof(GAME_BUILDIT_READER_PC, position) == 0x34);
GAME_PUBLISHER_ASSERT(offsetof(GAME_BUILDIT_READER_PC, anim_object_count) == 0x6c);
GAME_PUBLISHER_ASSERT(offsetof(GAMESAVE, gold_bricks) == 0x7c1a);
GAME_PUBLISHER_ASSERT(offsetof(GAMESAVE, purchased_extra_bits_high) == 0x7c04);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, part_debris_sys) == 0x138);

extern "C" i32 __cdecl Hub_BonusBuildIt(GIZBUILDIT_PC *buildit);

static i32 __cdecl GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT_PC *input, GAMEOBJECT *object) {
    GAME_BUILDIT_READER_PC *buildit = (GAME_BUILDIT_READER_PC *)input;
    if (WORLD->area != NULL && (WORLD->area->flags & 0x40) != 0) {
        if (HubBuildItPc == input)
            return Game.gold_bricks >= 2 * buildit->anim_object_count;
        if (Game.gold_bricks < buildit->anim_object_count && Hub_BonusBuildIt(input) != 0)
            return 0;
    }
    return 1;
}

extern "C" void __cdecl BuyAllShopExtras(void);
extern "C" i16 TextIdPc880ff0;

static void __cdecl GizBuildIt_FinishFn_Game(GIZBUILDIT_PC *input) {
    if (WORLD->area != NULL && (WORLD->area->flags & 0x40) != 0 && HubBuildItPc == input &&
        (Game.purchased_extra_bits_high & 0x80) == 0) {
        AddFancyMessageRGB(TTab[TextIdPc880ff0], 0.0f, 0.0f, 0.6f, 1.25f, 1, 255, 0, 0);
        BuyAllShopExtras();
        ReCalculateCompletionPoints();
        AddPartDebris(WORLD->part_debris_sys, 15, &((GAME_BUILDIT_READER_PC *)HubBuildItPc)->position);
    }
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v4/collection-selecting.cppfrag Keep
// in the coherent original private callback publication group; no wrappers.
extern i32 freeplaymode;
extern i32 freeplay_selected[2];
GAME_PUBLISHER_ASSERT(sizeof(MISSION_MENU_PACKET_PC) == 0xa);
GAME_PUBLISHER_ASSERT(offsetof(MISSION_MENU_PACKET_PC, player_model) == 0);
GAME_PUBLISHER_ASSERT(offsetof(MISSION_MENU_PACKET_PC, active_player) == 4);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, area) == 0x12c);

static void __cdecl Collection_GetSelectingPlayerIDs(i16 *ids) {
    i32 count;
    if (WORLD->area == NULL || WORLD->area != HUB_ADATA || GetMenuID() != 17 || (u32)freeplaymode > 3)
        return;
    count = 0;
    if (MissionMenuPacketPc.active_player[0] != 0 && freeplay_selected[0] <= 2) {
        ids[0] = MissionMenuPacketPc.player_model[0];
        count = 1;
    }
    if (MissionMenuPacketPc.active_player[1] != 0 && freeplay_selected[1] <= 2)
        ids[count] = MissionMenuPacketPc.player_model[1];
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/publisher-source-v1/world-and-hub-corrected.cppfrag
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);

extern "C" void __cdecl Game_WorldInfo_InitMenu(WORLDINFO *world, i32 *menu_id, i32 *menu_y) {
    if (world->current_level == TITLES_LDATA)
        *menu_id = 0;
    else if (world->current_level == CREDITS_LDATA)
        *menu_id = 18;
}

extern "C" void __cdecl Game_WorldInfo_InitLast(WORLDINFO *world) {
    STUBBED_NAMED("Game_WorldInfo_InitLast: GizSpinners_InitTerrain 48f6f6 -> 554350");
    STUBBED_NAMED("Game_WorldInfo_InitLast: Signals_InitTerrain 48f6fc -> 4c7980");
    STUBBED_NAMED("Game_WorldInfo_InitLast: Attractos_InitTerrain 48f702 -> 4029e0");
    STUBBED_NAMED("Game_WorldInfo_InitLast: SecurityDoors_InitTerrain 48f708 -> 4bcf70");
    Levers_InitTerrain(world);
    GizPanel_InitTerrain(world);
    HatMachines_InitTerrain(world);
    GizmoBlowupsFinalSetup(world);
    STUBBED_NAMED("Game_WorldInfo_InitLast: InitClimbObjectSys 48f726 -> 56cc00");
    STUBBED_NAMED("Game_WorldInfo_InitLast: GizmoPushBlockInitAndReset 48f72e -> 5a7a00 (world,NULL)");
}

// These two genuine Hub functions belong in the Hub owner, not game.cpp.

// Keep this private callback with the real publisher; native is a tail call.
extern "C" void __cdecl Hub_ResetPanel(void);
static void __cdecl PauseGame_ExtraCode(void) {
    Hub_ResetPanel();
}

GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
static i32 __cdecl AlternateGame_callback_925180_PC(GAMEOBJECT *object) {
    u8 context = (u8)object->character_context;
    if (context == 0x4d && object->field_7b3 == 0)
        return 2;
    return (context == 0x52) + 1;
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v7/game-gizmo-sfx.cppfrag
/* Local minimum reader; no GAMECAMERA size or backing claim. */
typedef struct GAME_GIZMO_SFX_CAMERA_READER_PC {
    u8 opaque_00;
    u8 sock;
    u8 opaque_02[0x2a];
    f32 sock_distance;
} GAME_GIZMO_SFX_CAMERA_READER_PC;
GAME_PUBLISHER_ASSERT(offsetof(GAME_GIZMO_SFX_CAMERA_READER_PC, sock) == 1);
GAME_PUBLISHER_ASSERT(offsetof(GAME_GIZMO_SFX_CAMERA_READER_PC, sock_distance) == 0x2c);

void __cdecl GizmoBlowUp_Sfx(GIZMOBLOWUP_PC *unused, NUVEC *position) {
    LEVELDATA_PC *level = WORLD->current_level;
    GAME_GIZMO_SFX_CAMERA_READER_PC *camera = (GAME_GIZMO_SFX_CAMERA_READER_PC *)GameCam;
    (void)unused;
    if (level == PODSPRINTA_LDATA && (camera->sock == 10 || camera->sock == 11 || camera->sock == 12) &&
        camera->sock_distance > 145.0f && camera->sock_distance < 195.0f) {
        PlaySfx("PodX_PurpCrysHit", position);
    } else if (level == PODRACEB_LDATA && camera->sock == 0 && camera->sock_distance > 145.0f) {
        PlaySfx("PodX_PurpCrysHit", position);
    } else {
        GameAudio_PlaySfx(0x33, position, 0, 0);
    }
}

extern "C" NUMTL *FadeMtl2;
extern "C" void __cdecl DrawPauseFade(void);

typedef struct PANEL_PRIM_VERTEX_PC {
    f32 x, y, z;
    u32 colour;
} PANEL_PRIM_VERTEX_PC;
STATIC_ASSERT(offsetof(PANEL_PRIM_VERTEX_PC, colour) == 0xc);

static void NuRndrPrimSetColour(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0)
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    else
        converted =
            ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    ((PANEL_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

extern "C" void __cdecl PanelRender(WORLDINFO *world) {
    i32 colour;
    NuRndrBeginScene(1);
    SetQFont2D();
    SetPanelLights(1.0f);
    NuRndrClear(0xe00, 0xff000000, 1.0f);
    DrawPauseFade();
    DrawPanel();
    FadeSys.Update();
    STUBBED_NAMED("PanelRender: theGameThings->DisplayThings(NULL) (vtable slot 0x28) at 0x0095e438");
    NuRndrEndScene();
    FadeSys.Draw();
    if (WORLD != NULL && WORLD->current_level == TITLES_LDATA) {
        colour = (i32)((1.0f - newgamealpha) * 255.0);
        NuRndrBeginScene(1);
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(2);
        NuPrim2DBegin(0x10, 5, FadeMtl2);
        colour <<= 24;
        NuRndrPrimSetColour(colour);
        NuPrim2DAddXYZ(0.0f, 0.0f, 0.0f);
        NuRndrPrimSetColour(colour);
        NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
        NuRndrEndScene();
    }
}

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v8/tag-lsw.cppfrag
typedef struct TAG_CHARACTER_DATA_READER_PC {
    u8 opaque_00[0x42];
    i16 icon_model;
    u8 opaque_44[8];
} TAG_CHARACTER_DATA_READER_PC;
GAME_PUBLISHER_ASSERT(sizeof(TAG_CHARACTER_DATA_READER_PC) == 0x4c);
GAME_PUBLISHER_ASSERT(offsetof(TAG_CHARACTER_DATA_READER_PC, icon_model) == 0x42);
GAME_PUBLISHER_ASSERT(offsetof(APICHARSYS, char_data) == 0x20);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, hud_icon_timer) == 0xd60);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);

extern i32(__cdecl *Tag_NoHiddenIconFn)(GAMEOBJECT *);

void __cdecl Tag_DrawIcon_LSW(GAMEOBJECT *object) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC position;
    i32 id;
    i16 icon;
    if (VehicleArea != 0 || FadeSys.fade != 0.0f || (object->flags_1fc & 0x80) == 0 ||
        (WORLD->current_level->flags & 0x800000) != 0) {
        return;
    }
    if (Tag_NoHiddenIconFn != NULL && Tag_NoHiddenIconFn(object) != 0) {
        return;
    }
    if ((object->flags_ef0 & 0x100000) == 0) {
        f32 timer = object->hud_icon_timer;
        if (!(timer > 0.0f)) {
            return;
        }
        if (timer < 2.0f && !(NuFmod(timer, 0.4f) >= (f64)0.2f)) {
            return;
        }
    }
    message = AddGameMsg_Default;
    position.x = object->collision_position.x;
    position.y = (f32)((f64)object->character_top * object->origin_scale + object->position.y + (f64)0.1f);
    position.z = object->collision_position.z;
    message.position = &position;
    message.scale = MissionPlayerIconSizePc + MissionPlayerIconSizePc;
    message.target_is_coin_total = 2;
    message.flags = 0x87;
    message.unknown_34 = 0.0f;
    if (object->field_280 == 1) {
        message.unknown_34 += 0.002f;
    }
    id = (i16)object->id;
    icon = ((TAG_CHARACTER_DATA_READER_PC *)apicharsys->char_data)[id].icon_model;
    if (icon != -1 && GCDataList[id].hover_fall_mode == 0 && (i32)icon != PCArcadeUnpairedIconModel) {
        icon = (i16)((u16)icon + 1);
    }
    message.icon = icon;
    message.special = &WORLD->level_objects[(i32)icon];
    AddGameMsg(&message);
    message.icon = object->field_280 != 0 ? 0xa5 : 0xa6;
    message.special = &WORLD->level_objects[(i32)message.icon];
    AddGameMsg(&message);
}

// Source lease: build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v9/tag-alternate.cppfrag
/* Reuse TAG_CHARACTER_DATA_READER_PC from the LSW callback contract. */
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, pause_context_state) == 0xee8);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) == 0x14);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m30) == 0x30);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m31) == 0x34);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m32) == 0x38);

extern f32 game_pulse;

void __cdecl Tag_DrawIcon_Batman(GAMEOBJECT *object) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC position;
    i32 id;
    i16 icon;
    i32 first_green, first_blue;
    u8 first_red;
    u8 alpha;
    if (VehicleArea != 0 || FadeSys.fade != 0.0f || (object->flags_1fc & 0x80) == 0 ||
        !(object->pause_context_state > 0.0f)) {
        return;
    }
    message = AddGameMsg_Default;
    if (object->field_280 == 0) {
        first_red = 0;
        first_green = 127;
        first_blue = 255;
    } else {
        first_red = 63;
        first_green = 255;
        first_blue = 0;
    }
    alpha = (u8)(i32)(((f64)game_pulse * (f64)0.2f + (f64)0.8f) * 128.0 * object->pause_context_state);
    position.x = object->upper_position.x;
    position.y = (f32)((f64)object->character_top * object->origin_scale + object->position.y + (f64)0.05f);
    position.z = object->upper_position.z;
    if ((object->flags_e24 & 8) != 0) {
        f32 max_y = object->collision_position.y;
        f32 x_sum = 0.0f;
        f32 z_sum = 0.0f;
        i32 count = 0;
        i32 index;
        for (index = 0; index < 16; ++index) {
            if (object->character_model->points_of_interest[index] != NULL) {
                NUMTX *matrix = &object->locator_matrices[index];
                ++count;
                x_sum += matrix->m30;
                z_sum += matrix->m32;
                if (max_y < matrix->m31) {
                    max_y = matrix->m31;
                }
            }
        }
        if (count > 0) {
            f32 inverse_count = 1.0f / (f32)count;
            position.x = (f32)(((f64)inverse_count * x_sum + position.x) * 0.5);
            position.z = (f32)(((f64)inverse_count * z_sum + position.z) * 0.5);
            position.y = (f32)(((f64)max_y + (f64)0.05f + position.y) * 0.5);
        }
    }
    message = AddGameMsg_Default;
    message.position = &position;
    message.red = first_red;
    message.green = (u8)first_green;
    message.blue = (u8)first_blue;
    message.alpha = alpha;
    message.target_is_coin_total = 2;
    if (object->field_280 == 1) {
        message.unknown_34 = 0.0f;
        message.unknown_34 += 0.002f;
    }
    message.text = TagAsciiDownPc;
    message.scale = 2.0f;
    message.flags = 0x1087;
    AddGameMsg(&message);
    position.y = (f32)((f64)position.y + (f64)0.1175f);
    id = (i16)object->id;
    icon = ((TAG_CHARACTER_DATA_READER_PC *)apicharsys->char_data)[id].icon_model;
    message.scale = MissionPlayerIconSizePc + MissionPlayerIconSizePc;
    message.text = NULL;
    message.flags = 0x11087;
    message.icon = icon;
    message.special = &WORLD->level_objects[(i32)icon];
    AddGameMsg(&message);
    message.icon = object->field_280 != 0 ? 0xa5 : 0xa6;
    message.special = &WORLD->level_objects[(i32)message.icon];
    AddGameMsg(&message);
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v10/transform-and-allgold.cppfrag

GAME_PUBLISHER_ASSERT(offsetof(GIZMOBLOWUP_PC, mid_position) == 0x4c);
GAME_PUBLISHER_ASSERT(offsetof(GIZMOBLOWUP_PC, target_scale) == 0xa8);

void __cdecl GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP_PC *target) {
    if (Transform_TargettedByObj(target) == NULL) {
        f32 radius = (f32)((f64)target->target_scale * (f64)1.4f);
        NUVEC *position = &target->mid_position;
        f32 alpha = 0.4f;
        (void)radius;
        (void)position;
        (void)alpha;
        STUBBED_NAMED("GizmoBlowup_TransformDraw_Game: 4d5588 Transform_DrawTarget(position, radius, alpha)");
    }
}

static void __cdecl LEGO_AllGoldBricksFn(void) {
}

// Source lease:
// build/analysis/next-world-loading-wave/game-init-after-MM/remaining22-contract-v12/autobuild-and-reward-typed.cppfrag
typedef struct ATTRACTO_READER_PC {
    u8 opaque_00[0x5c];
    u16 angle;
} ATTRACTO_READER_PC;
GAME_PUBLISHER_ASSERT(offsetof(ATTRACTO_READER_PC, angle) == 0x5c);

static i32 __cdecl GizBuildit_AutoBuildPos_Game(void *context, NUVEC *position, NUVEC *result, u16 *angle) {
    WORLDINFO *world = (WORLDINFO *)context;
    ATTRACTO_PC *attracto;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
    }
    attracto = Attracto_FindNearest(world, position, NULL, NULL);
    if (attracto == NULL) {
        return 0;
    }
    Attracto_GetPos_Top(attracto, result);
    if (angle != NULL) {
        *angle = ((ATTRACTO_READER_PC *)attracto)->angle;
    }
    return 1;
}

typedef struct GAME_CUSTOMISER_REWARD_READER_PC {
    u8 opaque_00[0x174];
    u16 *save_pieces;
} GAME_CUSTOMISER_REWARD_READER_PC;
GAME_PUBLISHER_ASSERT(offsetof(GAME_CUSTOMISER_REWARD_READER_PC, save_pieces) == 0x174);

static void __cdecl LEGO_100PercentFn(void) {
    if (CharacterCustomiser != NULL) {
        i32 category;
        i32 piece;
        Customiser_Set100PercentPieces(CharacterCustomiser);
        if (Customiser_FindPieceByName(CharacterCustomiser, "hat_hair_00", &category, &piece) != NULL &&
            category != -1 && piece != -1) {
            GAME_CUSTOMISER_REWARD_READER_PC *customiser = (GAME_CUSTOMISER_REWARD_READER_PC *)CharacterCustomiser;
            customiser->save_pieces[category] = (u16)piece;
        }
    }
}

// Genuine original static configuration definitions.
/* Original static game.cpp settings/items. Proposal only, no production data. */
extern "C" AREA_DATA_PC *DEATHSTARRESCUE_ADATA;

i32 __cdecl GameAudio_OverrideFootStep_LSW(GAMEOBJECT *object, i32 on_platform) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    AREA_DATA_PC *area = world->area;
    if ((area == HOTHESCAPE_ADATA || world->current_level == JABBASPALACEE_LDATA) && on_platform == 0)
        return GetSfxId("fs_ice");
    if (area == DAGOBAH_ADATA && on_platform == 0) {
        if (world->current_level != LevelFixupSlotPc0087b258 || object->jump_animation_state != 0x14) {
            if (world->current_level != LevelFixupSlotPc0087b264) {
                if (world->current_level != LevelFixupSlotPc0087b268 ||
                    (GameCam->sock_position.sock_index != 1 && GameCam->sock_position.sock_index != 4))
                    return GetSfxId("fs_swamp");
            }
        }
    }
    if (world->current_level == JABBASPALACEA_LDATA &&
        (object->jump_animation_state == 0x18 || object->jump_animation_state == 9))
        return GetSfxId("fs_ice");
    if ((area == DEATHSTARRESCUE_ADATA || area == DEATHSTARESCAPE_ADATA || WORLD->area == JABBASPALACE_ADATA) &&
        object->jump_animation_state == 0x14)
        return GetSfxId((char *)GameAudioName0075c478Pc);
    return -1;
}

i32 __cdecl GameAudio_CheckReverb_LSW(void) {
    LEVELDATA_PC *level = WorldInfo_CurrentlyActive()->current_level;
    if (level == LevelFixupSlotPc0087b200 || level == LevelFixupSlotPc0087b264)
        return 1;
    if (level == LevelFixupSlotPc0087b228 && GameCam->sock_position.sock_index == 4)
        return 1;
    return 0;
}

static CUT_DRAW_SETTINGS_PC InitCutSettingsLswPc = {0x5b, 0x5c, 0xe7, 2, 0};
static CUT_DRAW_SETTINGS_PC InitCutSettingsAlternatePc = {0x5b, 0x5c, 0xe7, 2, 1};
static PLAYER_ITEM_TYPE_RESET_PC InitPlayerItemTypesPc[2] = {{15, {6, 0}, "blue", 2, 0, {0, 0}},
                                                             {-1, {0, 0}, NULL, 0, 0, {0, 0}}};

// Actual publication parent; unresolved VOID actions remain at native sites.
/* Source-only candidate. Native complete parent 42e160..42f393/4659.
 * Shared member refinements and real config storage in the adjacent recipe
 * must close before compilation. No substitute backing is supplied here. */
void __cdecl InitGameAfterConfig(void) {
    POINTS_PER_STORY = 6;
    POINTS_PER_CHARACTER = 1;
    POINTS_PER_SUPERSTORY = 3;
    POINTS_PER_SUPERBONUSCOMPLETE = 3;
    POINTS_PER_TIMETRIAL = 1;
    POINTS_PER_CHALLENGE = 2;
    POINTS_PER_MINIKIT = 2;
    POINTS_PER_TRUEJEDI = 2;
    POINTS_PER_REDBRICK = 1;
    POINTS_PER_MISSION = 2;
    POINTS_PER_CHEAT = 1;
    POINTS_PER_HINT = 1;
    POINTS_PER_GOLDBRICK = 1;
    if (StartupGameFlag1 != 0)
        SHOPGOLDBRICKS = 14;

    SHOPHINTCOUNT = 0;
    for (i16 *text_id = ShopHintTextIdsPc; *text_id != -1 && SHOPHINTCOUNT < 24; ++text_id) {
        HINT_PC *hint = Hint_FindHint(*text_id);
        if (hint != NULL) {
            ShopHintIdsPc[SHOPHINTCOUNT++] = hint->id;
            if (hint->price > 0) {
                HubCompletionPointsPc += POINTS_PER_HINT;
                CompletionPointInfoPc[5] += POINTS_PER_HINT;
            }
        }
    }
    HubBothTrueJediGoldBricksPc = 0;
    EpisodeGoldBrickForSuperBonusPc = 1;
    EpisodeGoldBrickForSuperStoryPc = 0;
    EpisodeGoldBrickForChallengePc = 0;
    if (EPISODECOUNT > 0) {
        i32 points = EPISODECOUNT * POINTS_PER_SUPERSTORY;
        HubCompletionPointsPc += points;
        CompletionPointInfoPc[0] += points;
    }

    for (i32 area_id = 0; area_id < AREACOUNT; ++area_id) {
        AREA_DATA_PC *area = &ADataList[area_id];
        i32 area_index;
        area->episode_index = (u8)Episode_ContainsArea(area_id, &area_index);
        area->area_index = (u8)area_index;
        if (area == HUB_ADATA || (area->flags & 0x2022) != 0)
            continue;
        if ((area->flags & 0x100) != 0) {
            HubCompletionPointsPc += POINTS_PER_SUPERBONUSCOMPLETE;
            CompletionPointInfoPc[1] += POINTS_PER_SUPERBONUSCOMPLETE;
            if (EpisodeGoldBrickForSuperBonusPc != 0)
                ++GOLDBRICKPOINTS;
        } else if ((area->flags & 4) != 0) {
            HubCompletionPointsPc += POINTS_PER_TIMETRIAL;
            CompletionPointInfoPc[1] += POINTS_PER_TIMETRIAL;
            ++GOLDBRICKPOINTS;
        } else {
            i32 total = HubCompletionPointsPc + POINTS_PER_STORY;
            i32 bucket = CompletionPointInfoPc[1] + POINTS_PER_STORY;
            HubCompletionPointsPc = total;
            CompletionPointInfoPc[1] = bucket;
            if ((area->flags & 0x800) == 0)
                ++GOLDBRICKPOINTS;
            if ((ADataList[area_id].flags & 0x10) != 0) {
                GOLDBRICKPOINTS += 2;
                total += POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                bucket += POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                if (HubBothTrueJediGoldBricksPc != 0) {
                    total += POINTS_PER_TRUEJEDI;
                    bucket += POINTS_PER_TRUEJEDI;
                    ++GOLDBRICKPOINTS;
                }
                total += POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE;
                bucket += POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE;
                HubCompletionPointsPc = total;
                CompletionPointInfoPc[1] = bucket;
                if (EpisodeGoldBrickForChallengePc != 0)
                    ++GOLDBRICKPOINTS;
            }
        }
        if ((u8)area->cheat != 0xff)
            Cheat_SetArea((i8)area->cheat, area_id);
    }

    for (i32 level_id = 0; level_id < LEVELCOUNT; ++level_id) {
        LEVELDATA_PC *level = &LDataList[level_id];
        if ((i16)level->title_text_id < 0 || (i16)level->title_text_id >= 800)
            level->title_text_id = tUNKNOWN;
        level->area_index = -1;
        level->area_level_index = 0xff;
        for (i32 area_id = 0; level->area_index == -1 && area_id < AREACOUNT; ++area_id) {
            for (i32 child = 0; level->area_index == -1 && child < ADataList[area_id].level_count; ++child) {
                if ((i16)ADataList[area_id].levels[child] == level_id) {
                    level->area_index = (u8)area_id;
                    level->area_level_index = (u8)child;
                }
            }
        }
        if (level->area_index != -1) {
            i32 area_id = (i8)level->area_index;
            level->episode = ADataList[area_id].episode_index;
            if ((ADataList[area_id].flags & 1) != 0) {
                level->reflection_range = 50;
                level->raycast_step = 20;
            }
            if (level->episode != 0xff && (ADataList[area_id].flags & 0x106) == 0 && (level->flags & 0x4e0) == 0)
                level->flags |= 0x2000;
        }
    }

    Level_RegisterGameConfigKeywords(LevelConfigKeywords_BeforeLoad, WorldLoadAfterKeywordsPc);
    Suits_Init();
    Collection_Configure("chars\\collection.txt", &permbuffer_ptr, &permbuffer_end);
    if ((u16)VehicleBoltId7f17d4Pc != 0xffff && InCollectList_Index((i16)VehicleBoltId7f17d4Pc, NULL, 0) != -1 &&
        TempCollectID != NULL) {
        TempCollectID->selection_type = 0;
        TempCollectID->shop_enabled = 1;
        TempCollectID->price = 50000;
    }
    i32 character_points = COLLECTION_COMPLETIONCOUNT * POINTS_PER_CHARACTER;
    HubCompletionPointsPc += character_points;
    CompletionPointInfoPc[2] += character_points;

    if (StartupGameFlag1 != 0) {
        MissionSys = Missions_Configure("levels\\missions.txt", &permbuffer_ptr, &permbuffer_end, &Game.mission_save);
        if (MissionSys != NULL) {
            HubCompletionPointsPc += MissionSys->count * POINTS_PER_MISSION;
            CompletionPointInfoPc[3] += MissionSys->count * POINTS_PER_MISSION;
            GOLDBRICKPOINTS += MissionSys->count;
        }
        Tag_DrawIconFn = Tag_DrawIcon_LSW;
        GlobalBoltType[0].object_ids[0] = 0x5d;
        GlobalBoltType[0].object_ids[1] = 0x5e;
        GlobalBoltType[1].object_ids[0] = 0x5f;
        GlobalBoltType[1].object_ids[1] = 0x60;
        GlobalBoltType[2].object_ids[0] = 0x61;
        GlobalBoltType[2].object_ids[1] = 0x62;
        DEFAULT_PLAYERHITPOINTS = 4;
        PlayerHitPointsTwoHeartsPc = 0;
        CutScenes_InitSystem(&InitCutSettingsLswPc);
        CutSceneLookupAllDebrisPagesPc = 1;
        STUBBED_NAMED("InitGameAfterConfig: NarrowSockExceptions_Init 42e6ac -> 54a180, config7f1158");
        APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_LSW, "chars\\commonanims");
        STUBBED_NAMED("InitGameAfterConfig: SurfaceMaskOff 42e6ca -> 572370, mask8034c4");
        STUBBED_NAMED("InitGameAfterConfig: SurfaceMaskOn 42e6d4 -> 572360, mask95102c");
        Hub_UsePlayerList = 1;
        BoltSys_Init(&BoltSys_LSW);
        GameAudio_Init(&GameAudioLswPc);
        KITPOSX = 0.0f;
        KITPOS2X = 0.0f;
        ConvertOldPickupsPc = 1;
        GrabScreenWhenFadingPc = 1;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        TrooperGameDebrisPc[0] = 0x83;
        TrooperGameDebrisPc[1] = 0x49;
        TrooperGameDebrisPc[2] = 0x4a;
        TrooperGameDebrisPc[3] = 0x4b;
        REDBRICKPOSX = 0.0f;
        LEGOHINT_SHOOTCAMERAS = 0x266;
        REDBRICKPOSY = -0.5f;
        PushBlockHintIdPc = 0x267;
        LEGOHINT_BUILD = 0x25c;
        REDBRICKPOS2X = 1.25f;
        LEGOHINT_FREEPLAYTOGGLE = 0x258;
        REDBRICKPOS2Y = 0.0f;
        DoubleJump_AlwaysReachJump2Height = 1;
        DoubleJump_JediSlam = 1;
        CanPunchGirls = 0;
        ExtraHurtSfxFn = ExtraHurtSfx_LSW;
        ExtraDieSfxFn = ExtraDieSfx_LSW;
        BuckStartExtraFn = BuckStartExtra_LSW;
        BoltInitSfxFn = BoltInitSfx_LSW;
        GameObjectDimensionsExtraFn = GameObjectDimensionsExtra_LSW;
        Punch_GetDamageFn = Punch_GetDamage_LSW;
        Punch_HitHoldFn = Punch_HitHold;
        Punch_HitExtraCodeFn = Punch_HitExtraCode_LSW;
        SetSoundFadeDistCallBackFn = SetSoundFadeDistCallBackFn_LSW;
        PlayerItemTypes_Init(InitPlayerItemTypesPc);
        SUPERCARRY_THROWSPEED_XZ = 2.0f;
        DisguiseAdjustFn = DisguiseAdjust_LSW;
        SUPERCARRY_THROWSPEED_Y = 1.5f;
        SuperCarry_UseActionButton = 1;
        CutScene_OverrideConfigFileNameFn = CutScene_OverrideConfigFileName_LSW;
        SUPERCARRY_RELEASESPEED_XZ = 1.0f;
        CharPivot_Init(CharPivot_LSW);
        CombatReactionHorizontalScalePc = 3.5f;
        ForcePush_Waft = 1;
        CombatReactionVerticalScalePc = 2.5f;
        ForcePush_SuperPush = 1;
        ForcePush_SuperMindTrick = 1;
        Lighting_HighlightFlash = 1;
    } else {
        Tag_DrawIconFn = Tag_DrawIcon_Batman;
        DEFAULT_PLAYERHITPOINTS = 8;
        PlayerHitPointsTwoHeartsPc = 1;
        STUBBED_NAMED("InitGameAfterConfig: alternate colours 42e8cc -> 509660 (18 native scalar arguments)");
        AlternateHudColourBytesPc[0] = 0;
        AlternateHudColourBytesPc[1] = 255;
        AlternateHudRgbPc[0] = 0;
        AlternateHudColourBytesPc[2] = 255;
        AlternateHudColourBytesPc[3] = 0;
        AlternateHudRgbPc[1] = 0;
        AlternateHudColourBytesPc[4] = 128;
        AlternateHudColourBytesPc[5] = 255;
        AlternateHudColourBytesPc[6] = 0;
        AlternateHudRgbPc[2] = 0;
        AlternateHudColourBytesPc[7] = 80;
        RenderBackdropBlackPc = 1;
        Lighting_FlashRedOnLastHeart = 1;
        CutScenes_InitSystem(&InitCutSettingsAlternatePc);
        AlternateGameCallbackPc = AlternateGame_callback_925180_PC;
        APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_Batman, "chars\\commonanims");
        STUBBED_NAMED("InitGameAfterConfig: SurfaceMaskOn 42e957 -> 572360, mask8034c4");
        STUBBED_NAMED("InitGameAfterConfig: SurfaceMaskOff 42e961 -> 572370, mask95102c");
        Hub_UsePlayerList = 0;
        BoltSys_Init(&BoltSys_Batman);
        GameAudio_Init(&GameAudioAlternatePc);
        MAGNETOFFSET = 0.0305f;
        GizmoBlowupHitMultiplierPc.value = 2;
        PUNCHGAP = 0.3f;
        CheckLostDataFn = Batarangs_CheckLostData;
        LEGOHINT_BUILD = 3;
        PUNCHCHARGAP = 0.6f;
    }

    LEGOACT_IDLE = 1;
    LEGOACT_BUILD = 0x5f;
    Lighting_BlueFlickerFn = ObjZappedBlue;
    AddGameMsg_Default.delay_fn = GameMsg_EndDelay_Game;
    WorldInfo_InitMenuFn = Game_WorldInfo_InitMenu;
    WorldInfo_InitLastFn = Game_WorldInfo_InitLast;
    TerSurface[12].object_id = 0x15;
    TerSurface[13].object_id = 0x16;
    LEGOACT_WALK = 0;
    LEGOACT_JUMP = 6;
    BigJumpLandFallbackPc = 7;
    LEGOACT_JUMP2 = 9;
    LEGOACT_JUMP3 = 0xe;
    BigJumpLandSecondaryPc = 10;
    LEGOACT_LAND3 = 0x15;
    LEGOACT_COMBOJUMP = 0x12;
    BigJumpLandPreferredPc = 0x13;
    LEGOACT_FALL = 5;
    LEGOACT_FALLLAND = 0x59;
    LEGOACT_BACKPACKFALLLAND = 0xb3;
    BoltDeactivatedActionPc = 0x41;
    LegoActPunchBehindPc = 0x94;
    BoltShootClassId0 = 0x5b;
    BoltShootClassId1 = 0x5a;
    BoltShootClassId2 = 0x5c;
    LEGOACT_LEDGE_GRAB = 0xda;
    LEGOACT_LEDGE_IDLE = 0x9d;
    LEGOACT_LEDGE_LEFT = 0x9e;
    LEGOACT_LEDGE_RIGHT = 0x9f;
    LEGOACT_HANG_IDLE = 0x91;
    LEGOACT_HANG_MOVE = 0x92;
    LEGOACT_CLIMB_IDLE = 0x87;
    LEGOACT_CLIMB_UP = 0xcd;
    LEGOACT_CLIMB_DOWN = 0xce;
    LEGOACT_CLIMB_LEFT = 0xcf;
    LEGOACT_CLIMB_RIGHT = 0xd0;
    LEGOACT_WALLSHUFFLE_IDLE = 0x8a;
    LEGOACT_WALLSHUFFLE_LEFT = 0x8b;
    LEGOACT_WALLSHUFFLE_RIGHT = 0x8c;
    LEGOACT_PUSH = 0x52;
    LEGOACT_SUPERPUSH_IDLE = 0xae;
    LEGOACT_SUPERPUSH_PUSH = 0xaf;
    LEGOACT_SUPERPUSH_PULL = 0xb0;
    LEGOACT_FLIP = 0xc;
    LEGOACT_FLIPLAND = 0xd;
    JumpBackFlipActionPc = 0x77;
    LEGOACT_WALLJUMP_WAIT = 0xa1;
    LEGOACT_GRAPPLE_IDLE = 0xba;
    LEGOACT_GRAPPLE_UP = 0xbb;
    LEGOACT_GRAPPLE_DOWN = 0xbc;
    LEGOACT_GRAPPLE_HANG = 0xbd;
    LEGOACT_GLIDE = 0x93;
    LEGOACT_SUPERCARRY_PICKUP = 0xa3;
    LEGOACT_SUPERCARRY_IDLE = 0xa4;
    LEGOACT_SUPERCARRY_WALK = 0xa5;
    LEGOACT_SUPERCARRY_THROW = 0xa6;
    LEGOACT_SUPERCARRY_PUTDOWN = 0xd5;
    LEGOACT_SUPERCARRY_BASH = 0xd6;
    LEGOACT_SUPERCARRY_JUMP = 0xd7;
    LEGOACT_SUPERCARRY_LAND = 0xd8;
    LEGOACT_SUPERCARRY_FALLLAND = 0xd9;
    LEGOACT_MAGNET_WALK_METAL = 0xc5;
    LEGOACT_MAGNET_TIPTOE = 0xc6;
    LEGOACT_MAGNET_WALK = 0xc7;
    LEGOACT_MAGNET_RUN = 0xc8;
    LEGOACT_MAGNET_JUMP = 0xc9;
    LEGOACT_MAGNET_LAND = 0xca;
    LEGOACT_LUNGE = 0x1f;
    LEGOACT_LUNGELAND = 0x20;
    JumpSlamActionPc = 0x21;
    LEGOACT_SLAMLAND = 0x22;
    LEGOACT_COMBATROLL_JUMP = 0xb4;
    LEGOACT_COMBATROLL_FALL = 0xb5;
    LEGOACT_COMBATROLL_LAND = 0xb6;
    LEGOACT_COMBATROLL_FIRE = 0x57;
    LEGOACT_EXTRA_JUMP = 0x78;
    LEGOACT_WHIP_CRACK = 0xdd;
    LEGOACT_WHIP_GRAB = 0xdd;
    LEGOACT_WHIP_BREAK = 0xdd;
    LEGOCONTEXT_BIGJUMP = 0x1f;
    BoltBlockContextPc = 0xc;
    LEGOCONTEXT_COMBO = 5;
    LEGOCONTEXT_PUNCH = 0x26;
    LEGOCONTEXT_SPECIALMOVE_ATTACKER = 0x26;
    LEGOSPL_SPLIT = 5;
    LEGOACT_EXTRA_JUMP2 = 0x79;
    LEGOACT_EXTRA_LAND = 0x7a;
    LEGOACT_EXTRA_LAND2 = 0x7b;
    LEGOACT_BUCK = 0x2a;
    LEGOACT_TEETER = 0xdb;
    LEGOACT_WHIP_START = 0xdc;
    ZipUpSwingStartAnimationPc = 0xe0;
    ZipUpSwingSwingAnimationPc = 0xe1;
    ZipUpSwingJumpAnimationPc = 0xe2;
    LEGOCONTEXT_TUBE = 0x11;
    LEGOCONTEXT_GLIDE = 0x4f;
    LEGOCONTEXT_HOLD = 0x18;
    LEGOCONTEXT_DROPIN = 0x23;
    LEGOCONTEXT_DOOMED = 0x2b;
    LEGOCONTEXT_LEDGETERRAIN = 0x55;
    LEGOCONTEXT_CLIMB = 0x43;
    LEGOCONTEXT_HANG = 0x4e;
    LEGOCONTEXT_JUMP = 0;
    ZipUpWallShuffleContextPc = 0x45;
    LEGOCONTEXT_PUSH = 0x27;
    LEGOCONTEXT_PUSHSPINNER = 0x28;
    LEGOCONTEXT_PUSHOBSTACLE = 0x59;
    LEGOCONTEXT_BEENTAKENOVER = 0x3b;
    LEGOCONTEXT_GETIN = 0x3c;
    LEGOCONTEXT_BUILDIT = 0x2d;
    LEGOCONTEXT_WEAPONIN = 6;
    LEGOCONTEXT_WEAPONOUT = 7;
    JumpBackFlipContextPc = 0x20;
    LEGOCONTEXT_WALLJUMPWAIT = 0x57;
    LEGOCONTEXT_GRAPPLE = 0x46;
    LEGOCONTEXT_SUPERCARRY = 0x58;
    LEGOCONTEXT_LAND_JUMP = 1;
    LEGOCONTEXT_LAND_JUMP2 = 2;
    LEGOCONTEXT_LAND_FLIP = 3;
    LEGOCONTEXT_LAND_COMBOJUMP = 4;
    LEGOCONTEXT_LAND_LUNGE = 0xd;
    LEGOCONTEXT_LAND_SLAM = 0xe;
    LEGOCONTEXT_LAND_SPECIAL = 0x19;
    ZipUpLandCombatRollContextPc = 0x29;
    LEGOCONTEXT_EATEN = 0x39;
    LEGOCONTEXT_SPECIALMOVE_VICTIM = 0x30;
    LEGOCONTEXT_BUCK = 0x3e;
    LEGOCONTEXT_WHIP = 0x62;
    LegoContextNetWaitPc = 0x63;
    LEGOMENU_TITLES = 0;
    LEGOMENU_NEWGAME = 1;
    LEGOMENU_PAUSEMAIN = 2;
    LEGOMENU_CREDITS = 18;
    LEGOCAMMODE_DOORCUT = 4;
    LEGOCAMMODE_OBSTACLE = 2;
    LEGOOBJ_ICON_FRAME_NEUTRAL = 0xa7;
    LEGOOBJ_ICON_FRAME_GREEN = 0xa5;
    LEGOOBJ_ICON_FRAME_BLUE = 0xa6;
    LegoObjectIconQuestionPc = 0xa1;
    PCArcadeUnpairedIconModel = 0xa3;
    CharKitModelId_PC = 0xcf;
    MiniKitModelId_PC = 0xce;
    PlayerStartSplineIndexPc = 0;
    LegoGameDebrisSplashPc = 0xd;
    GizBuilditGDeb[0] = 0x4c;
    GizBuilditGDeb[1] = 0x4d;
    GizBuilditGDeb[2] = 0x4e;
    GizBuilditGDeb[3] = 0x4f;
    GizBuilditGDeb[4] = 0x50;
    GizBuilditGDeb[5] = 0x51;
    GizSpinnerGDeb_Fail[0] = 0x57;
    GizSpinnerGDeb_Fail[2] = 0x58;
    GizSpinnerGDeb_Fail[1] = 1;
    LEGOASCII_UP = InitAsciiUpPc;
    LEGOASCII_DOWN = TagAsciiDownPc;
    LEGOASCII_LEFT = InitAsciiLeftPc;
    LEGOASCII_RIGHT = InitAsciiRightPc;
    LEGOASCII_BIGARROW = InitAsciiBigArrowPc;
    GamePads_IgnoreInputFn = Game_IgnoreInput;
    Door_GoThrough_ExtraCodeFn = GoThroughDoor_ExtraCode;
    GizmoBlowup_TransformDrawFn = GizmoBlowup_TransformDraw_Game;
    KEEPONSCREEN_SIDESONLY = 1;
    CutScenePlayer_AcceptFn = CutScenePlayer_Accept;
    CutScenePlayer_Configure("cut\\clips.txt", &permbuffer_ptr, &permbuffer_end, &tCLIPi, &tINTRO, &tMIDTRO, &tOUTRO,
                             &tENDING);
    i32 powerup_text = (i16)tPOWERUP;
    i32 startup_game = StartupGameFlag1;
    CanMagnetClimbFn = CanMagnetClimb_Game;
    CanPushObstaclesFn = CanPushObstacles_Game;
    CanSuperCarryFn = CanSuperCarry_Game;
    CanPushBlocksFn = CanPushBlocks_Game;
    CanGlideFn = CanGlide_Game;
    UsingExtraActionsFn = UsingExtraActions_Game;
    CanStartHoldFn = CanStartHold_Game;
    Player_ClearContextFn = Player_ClearContext_Game;
    LEGOTHINGSSCENE_TER_SPINBASE = 0;
    LEGOTHINGSSCENE_TER_SPINARM = 1;
    GizBuildit_AutoBuildPosFn = GizBuildit_AutoBuildPos_Game;
    Fighting_WeaponInActionFn = Fighting_WeaponInAction_Game;
    Fighting_WeaponOutActionFn = Fighting_WeaponOutAction_Game;
    LEGOOBJ_FLOORTARGET = 0x55;
    LEGOOBJ_GRAPPLE_HOOK = 0x3d;
    Slam_GetDebrisFn = Slam_GetDebris_Game;
    IsWearingBackPackFn = IsWearingBackPack_Game;
    Grass_Available = 1;
    PauseGame_ExtraCodeFn = PauseGame_ExtraCode;
    Hub_PanelBusyFn = Hub_PanelBusy;
    CheckMusicOtherFn = CheckMusicOther;
    GizBuildIt_CanStartBuildingFn = GizBuildIt_CanStartBuildingFn_Game;
    GizBuildIt_FinishFn = GizBuildIt_FinishFn_Game;
    POWERUP_TEXTID = powerup_text;
    RenderBackdropAlphaFnPc = BackDrop_Alpha;
    Game_100PercentFn = LEGO_100PercentFn;
    Game_AllGoldBricksFn = LEGO_AllGoldBricksFn;
    LastSafePosExtraFn = LastSafePosExtra;
    GameAudio_ActionMusicFn = ActionMusicFn;
    PauseRenderOffFn = PauseRenderOff;
    FindSlamOrigin_UseCPosFn = FindSlamOrigin_UseCPos;
    GizmoBlowUp_NoTargetFn = GizmoBlowUp_NoTarget;
    MatrixReflection_CanOverrideFn = MatrixReflection_CanOverride;
    Jump_PreventJumpFn = Jump_PreventJump;
    SurfaceInfo_ExtraReflectFn = SurfaceInfo_ExtraReflect;
    Hint_AlphaTargetFn = Hint_AlphaTarget;
    Arcade_TextCrawlID = 0x1f1;
    Arcade_TextCrawlParagraphs = 2;
    GizmoPickups_Collide2DFn = GizmoPickups_Collide2D;
    LEGOOBJ_DEFAULTLASTCOIN.value = 0xb7;
    Tag_NoHiddenIconFn = Tag_NoHiddenIcon;
    Collection_GetSelectingPlayerIDsFn = Collection_GetSelectingPlayerIDs;
    GizmoBlowUp_SfxFn = GizmoBlowUp_Sfx;
    APIObjResetShadowMapRenderingFn = ResetShadowMapRenderingFn;
    APIObjEnableShadowMapRenderingFn = EnableShadowMapRenderingFn;
    GameMsg_GetExtraObjFn = GameMsg_GetExtraObj;
    Jump_EndOfLandContextFn = Jump_EndOfLandContext;
    BigJump_EndOfLandFn = BigJump_EndOfLand;

    if (StartupGameFlag0 != 0) {
        LEDGETERRAINLOOKAHEAD = 0.075f;
        NUVEC zero = v000;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        TechnoSysPc.idle_offsets[0] = zero;
        TechnoSysPc.idle_offsets[1] = zero;
        TechnoSysPc.interaction_time = -0.2f;
        TechnoSysPc.idle_offsets[2].x = 0.0f;
        TechnoSysPc.idle_offsets[2].y = 0.0f;
        DefaultMotionLedgeTerrainEnabledPc = 1;
        LedgeTerrain_CheckAnims = 0;
        TechnoSysPc.idle_offsets[2].z = -0.1295f;
        Grapples_Available = 1;
        Grapple_RopeSwingsAvailable = 1;
        SuperCarryEnablePc = 1;
        CombatRollEnablePc = 1;
        CUTSUBTITLEDEFAULT_Y = -0.5f;
        LEGOHINT_PUNCH = 0;
        CUTSUBTITLEDEFAULT_SCALEX = 0.6f;
        CUTSUBTITLEDEFAULT_SCALEY = 0.6f;
        TechnoSysPc.idle_object_ids[1] = 0x2f;
        TechnoSysPc.idle_object_ids[2] = 0x30;
        TechnoSysPc.idle_object_ids[3] = 0x31;
        SUPERCARRY_THROWSPEED_XZ = 2.0f;
        SUPERCARRY_THROWSPEED_Y = 1.5f;
        TechnoSysPc.idle_object_ids[0] = 0x56;
        TechnoSysPc.hand_object_ids[0] = -1;
        TechnoSysPc.hand_object_ids[1] = -1;
        TechnoDefaultObjectPc = 0x56;
        SuperCarry_UseActionButton = 1;
    } else if (startup_game == 0) {
        Player_ToggleSubCharacterFn = Player_ToggleSubCharacter_Game;
        Player_ToggledCharacterFn = Player_ToggledCharacter_Game;
        Punch_HitExtraCodeFn = Punch_HitExtraCode_alternate_PC;
        CharPivot_Init(&CharPivot_Batman);
        startup_game = StartupGameFlag1;
    }
    HubCompletionPointsPc += 44 * POINTS_PER_CHEAT;
    for (i32 i = 0; i < 44; ++i)
        CompletionPointInfoPc[4] += POINTS_PER_CHEAT;
    if (SHOPGOLDBRICKS > 0) {
        i32 count = SHOPGOLDBRICKS;
        HubCompletionPointsPc += count * POINTS_PER_GOLDBRICK;
        GOLDBRICKPOINTS += count;
        for (i32 i = 0; i < count; ++i)
            CompletionPointInfoPc[6] += POINTS_PER_GOLDBRICK;
    }
    if (startup_game != 0) {
        CUT_PLAYER_ACTIVE_PC *player = CutScenePlayer_Available();
        CutScenePlayCountPc = 0;
        if (player != NULL)
            CutScenePlayCountPc = player->clip_count;
    }
}

#undef GAME_PUBLISHER_ASSERT

struct MUSIC_PAIR_ARENA_PC {
    i32 prefix_word;
    u8 unknown_004[0x18];
    i16 action_first;
    u8 unknown_01e[0x1a];
    i16 ambient_first;
    u8 unknown_03a[2];
};
STATIC_ASSERT(sizeof(MUSIC_PAIR_ARENA_PC) == 0x3c);
STATIC_ASSERT(offsetof(MUSIC_PAIR_ARENA_PC, action_first) == 0x1c);
STATIC_ASSERT(offsetof(MUSIC_PAIR_ARENA_PC, ambient_first) == 0x38);

extern "C" nusound_filename_info_s *__cdecl ConfigureMusic(char *ignored_file, VARIPTR *cursor, VARIPTR *end) {
    nusound_filename_info_s *files;
    MUSIC_PAIR_ARENA_PC *arena;
    music_man.Initialise("audio\\music.cfg", NULL, cursor, *end);
    music_man.GetSoundFiles(&files, NULL);
    SfxMusicExtensionPc = ".mib";
    arena = (MUSIC_PAIR_ARENA_PC *)((cursor->addr + 3) & ~3u);
    arena->prefix_word = 0;
    arena->action_first = -1;
    ActionPairTab = (PC_MUSIC_ACTION_PAIR_PC *)&arena->action_first;
    arena->ambient_first = -1;
    AmbientPairTab = &arena->ambient_first;
    cursor->void_ptr = arena + 1;
    return files;
}

extern "C" {
    extern i32 last_currentmusicpair_quiet;
    extern i32 MusicOther;
    extern i32 sticky_attack;
}

extern "C" void __cdecl GamePlayMusic(LEVELDATA_PC *level, i32 check, OPTIONSSAVE *options) {
    i32 other = MusicOther;
    i32 music_other;

    last_currentmusicpair_quiet = CurrentMusicPair_Quiet;
    MusicOther = 0;
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return;
    }
    if (CheckMusicOtherFn != NULL) {
        music_other = CheckMusicOtherFn();
    } else {
        music_other = 0;
    }
    MusicOther = music_other;
    if (check != 0) {
        if (music_other == other) {
            return;
        }
    } else {
        sticky_attack_time = 0.0f;
        sticky_attack = MusicActiveReset;
    }

    music_man.SelectTrackByHandle(1, level->music_tracks[music_other]);
    music_man.SelectTrackByHandle(2, level->music_tracks[MusicOther + 2]);
    music_man.SelectTrackByHandle(0x20, level->music_tracks[MusicOther + 4]);

    if (options->music_enabled != 0) {
        if (sticky_attack != 0) {
            if (music_man.GetTrackHandle(2, NULL) != -1) {
                music_man.PlayTrack(2);
                return;
            }
            if (music_man.GetTrackHandle(1, NULL) != -1) {
                music_man.PlayTrack(1);
                return;
            }
        } else {
            if (music_man.GetTrackHandle(1, NULL) != -1) {
                music_man.PlayTrack(1);
                return;
            }
            if (music_man.GetTrackHandle(2, NULL) != -1) {
                music_man.PlayTrack(2);
                return;
            }
        }
    }
    music_man.PlayTrack(0x20);
}

extern "C" void MusicClearAll() {
    MusicPlrsUnderAttack = 0;
    MusicActive = -1;
    sticky_attack_time = 0.0f;
    MusicActiveReset = 0;
    MusicSwapDelay = 0.0f;
    MusicSeekIndex = 0;
    PlayersUnderAttack = 0;
    MusicInside = 0;
    MusicFindCut = 0;
    MusicRestoreTrack = 0;
    MusicOnFlag = 1;
    MusicPlrsHoldAttack = 0;
    CurrentMusicPair_Quiet = -1;
}

struct MUSIC_CUT_STOP_VIEW_PC {
    u8 unknown_0000[0xec];
    i16 skip_level;
};
STATIC_ASSERT(offsetof(MUSIC_CUT_STOP_VIEW_PC, skip_level) == 0xec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, nearest_opponent) == 0x39c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, nearest_opponent_metric) == 0x3a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(offsetof(OPTIONSSAVE, music_enabled) == 6);

extern "C" void __cdecl ProcessMusicChanges(LEVELDATA_PC *level, OPTIONSSAVE *options) {
    if (GameAudio_ActionMusicFn != NULL) {
        MusicActiveReset = GameAudio_ActionMusicFn();
    } else {
        MusicActiveReset =
            DoubleScore != 0 || Cheat_PowerUpActive(-1) != 0 ||
            (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->ai_packet.opponent != NULL ||
                                   (((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent != NULL &&
                                    ((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent->dead == 0 &&
                                    ((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent_metric < 3.0f))) ||
            (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->ai_packet.opponent != NULL ||
                                   (((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent != NULL &&
                                    ((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent->dead == 0 &&
                                    ((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent_metric < 3.0f)));
    }
    if (CheckMusicOtherFn != NULL)
        MusicOther = CheckMusicOtherFn();
    else
        MusicOther = 0;
    if (Paused == 0 && sticky_attack_time > 0.0f)
        sticky_attack_time -= FRAMETIME;
    if (sticky_attack != MusicActiveReset && sticky_attack_time <= 0.0f) {
        sticky_attack = MusicActiveReset;
        sticky_attack_time = ActionMusicStickyTimeoutPc[MusicActiveReset];
    }
    if (CutStopInfo != NULL) {
        MUSIC_CUT_STOP_VIEW_PC *cut = (MUSIC_CUT_STOP_VIEW_PC *)CutStopInfo;
        if (cut->skip_level != -1 && (LDataList[cut->skip_level].flags & 1) != 0)
            level = &LDataList[cut->skip_level];
    }
    music_man.SelectTrackByHandle(1, level->music_tracks[MusicOther]);
    music_man.SelectTrackByHandle(2, level->music_tracks[MusicOther + 2]);
    music_man.SelectTrackByHandle(0x20, level->music_tracks[MusicOther + 4]);
    if (options->music_enabled != 0) {
        if (sticky_attack != 0) {
            if (music_man.GetTrackHandle(2, NULL) != -1) {
                music_man.PlayTrack(2);
                goto process_music;
            }
            if (music_man.GetTrackHandle(1, NULL) != -1) {
                music_man.PlayTrack(1);
                goto process_music;
            }
        } else {
            if (music_man.GetTrackHandle(1, NULL) != -1) {
                music_man.PlayTrack(1);
                goto process_music;
            }
            if (music_man.GetTrackHandle(2, NULL) != -1) {
                music_man.PlayTrack(2);
                goto process_music;
            }
        }
    }
    music_man.PlayTrack(0x20);
process_music:
    music_man.Process(FRAMETIME);
}
