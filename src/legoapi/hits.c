#include "saga.h"

i32 __cdecl CannotKill(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x02000000) != 0)
        return 1;
    return (object->character_data->game_character->flags_094 & 0x800) != 0 && object->field_280 == 0xff;
}

i32 __cdecl ObjHitObj_Flags(GAMEOBJECT *object) {
    i32 flags = 0;
    if (object != NULL) {
        if ((object->flags_1fc_word & 0x80) == 0) {
            flags = 2;
        } else {
            flags |= 0x804;
        }
        if ((object->flags_1f8 & 0x10001) != 0) {
            flags |= 0x10;
        } else if ((object->flags_1f8 & 4) != 0) {
            flags |= 0x20;
        } else {
            flags |= 8;
        }
    }
    return flags;
}

void __cdecl KillRumble(GAMEOBJECT *object) {
    PAD_INPUT_PC *pad;
    if (object != NULL && (object->flags_1fc & 0x80) != 0) {
        NewRumble(object->pad->bound_pad, 0.7f, 0);
        pad = object->pad->bound_pad;
        if (pad != NULL)
            NuSound3AddRumble(pad, 0.1f, 0, 0, 0.0f);
    }
}
void __cdecl HitRumble(GAMEOBJECT *object) {
    PAD_INPUT_PC *pad;
    if (object != NULL && (object->flags_1fc & 0x80) != 0) {
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        pad = object->pad->bound_pad;
        if (pad != NULL)
            NuSound3AddRumble(pad, 2.0f / DEFAULTFPS, 0, 0, 0.0f);
    }
}

#include <stdio.h>
typedef struct combat_coinpacket_pc_s {
    u32 coins;
} COMBAT_COINPACKET_PC;
STATIC_ASSERT(offsetof(COMBAT_COINPACKET_PC, coins) == 0);

typedef struct shield_state_pc_s {
    u8 unknown_000[0xd30];
    f32 shield_enabled;
    f32 shield_timer;
} SHIELD_STATE_PC;
STATIC_ASSERT(offsetof(SHIELD_STATE_PC, shield_enabled) == 0xd30);
STATIC_ASSERT(offsetof(SHIELD_STATE_PC, shield_timer) == 0xd34);
void __cdecl AlertSurroundingCreatures(GAMEOBJECT *, NUVEC *);
void __cdecl TakeHitRumble(GAMEOBJECT *, f32);
i32 __cdecl Player_HasInvincibility(GAMEOBJECT *);
i32 __cdecl GameAudio_GetPlrSfxBits(void *);
extern const char ShieldHitSfxPc[9], ShieldFlickerSfxPc[13];
extern f32 SHIELDFLICKERTIME;
void __cdecl ObjHitShield(GAMEOBJECT *attacker, GAMEOBJECT *target, i32 damage, void *unused_bolt) {
    SHIELD_STATE_PC *shield = (SHIELD_STATE_PC *)target;
    i32 health;
    f32 first_rumble, second_rumble;
    (void)unused_bolt;
    if (target == NULL)
        return;
    if (attacker != NULL && !(attacker->flags_1fc_word & 1))
        attacker = NULL;
    if (shield->shield_enabled == 1.0f && shield->shield_timer > 0.0f) {
        if (attacker != NULL && !(attacker->flags_1fc_word & 0x80) && attacker->character_context != 0x39)
            damage = 0;
    }
    if (attacker != NULL && (attacker->flags_1fc_word & 0x80))
        AlertSurroundingCreatures(attacker, &attacker->collision_position);
    GameAudio_PlaySfx(0x41, &target->collision_position, GameAudio_GetPlrSfxBits(attacker), 0);
    if (target->flags_1fc_word & 0x80)
        TakeHitRumble(target, 0.6f);
    if (MiniCutCam != 0 && (target->flags_1fc_word & 0x80))
        damage = 0;
    if (target->shield_hitpoints != 0 && damage > 0) {
        if ((target->flags_1fc_word & 0x80) && Player_HasInvincibility(target))
            damage = 0;
        health = target->shield_hitpoints - damage;
        if (health > 0) {
            PlaySfx((char *)ShieldHitSfxPc, &target->collision_position);
            target->shield_hitpoints = (u8)health;
            shield->shield_timer = SHIELDFLICKERTIME;
            first_rumble = 0.5f;
            second_rumble = 0.0f;
        } else {
            PlaySfx((char *)ShieldFlickerSfxPc, &target->collision_position);
            shield->shield_timer = 0.5f;
            target->shield_hitpoints = 0;
            shield->shield_enabled = 0.0f;
            first_rumble = 0.8f;
            second_rumble = 0.3f;
        }
        if (attacker != NULL) {
            if (first_rumble > 0.0f)
                NewRumble(attacker->pad->bound_pad, first_rumble, 0);
            if (second_rumble > 0.0f)
                NewRumble(attacker->pad->bound_pad, second_rumble, 0);
        }
        return;
    }
    if (attacker != NULL)
        NewBuzz(attacker->pad->bound_pad, 0.1f, 0);
}

typedef struct reaction_fields_pc_s {
    u8 unknown_00[0x6c4];
    u8 reset_contexts;
    u8 unknown_6c5[0x750 - 0x6c5];
    f32 animation_speed;
    u8 unknown_754[0x778 - 0x754];
    f32 reaction_duration;
} REACTION_FIELDS_PC;
STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, reset_contexts) == 0x6c4);
STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, animation_speed) == 0x750);
STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, reaction_duration) == 0x778);
void __cdecl Player_ClearContext(GAMEOBJECT *, i32);
void __cdecl Player_ResetContexts(void *);
i32 __cdecl PlayerTakeHitReactionPc(GAMEOBJECT *target, GAMEOBJECT *attacker, f32 duration, u16 flags) {
    REACTION_FIELDS_PC *view = (REACTION_FIELDS_PC *)target;
    void **animations;
    i32 choices[16], count = 0, animation, index;
    u8 saved_jump_state;
    f32 speed, timer, dx, dz;
    u16 angle;
    if (target->field_280 != 0xff)
        return 0;
    animations = target->character_model->model_data_b;
    if (animations[0xa7] != NULL && animations[0xaa] != NULL)
        choices[count++] = 0xa7;
    if (animations[0xa8] != NULL && animations[0xab] != NULL)
        choices[count++] = 0xa8;
    if (animations[0xa9] != NULL && animations[0xac] != NULL)
        choices[count++] = 0xa9;
    if (!(flags & 0x200)) {
        if (animations[0x3d] != NULL)
            choices[count++] = 0x3d;
        if (attacker != NULL && (attacker->flags_1fc_word & 0x80) && animations[0xb9] != NULL)
            choices[count++] = 0xb9;
    }
    animation = count ? choices[qrand() / (65535 / count + 1)] : 1;
    if (animation == -1)
        return 0;
    saved_jump_state = target->jump_variant_state;
    target->field_7aa = (i16)animation;
    speed = AnimSpeed(target->character_model, (i16)animation);
    view->animation_speed = speed;
    if (target->field_7aa != 0xb9 && target->field_7aa != 0x3d) {
        if (speed <= 0.0f)
            view->animation_speed = 1.0f;
        else if (speed <= (f64)0.8f)
            view->animation_speed = 0.8f;
    }
    ResetAnimPacket(&target->anim_packet, -1);
    Player_ClearContext(target, 1);
    Player_ResetContexts(&view->reset_contexts);
    view->reaction_duration = duration;
    target->character_context = 0x5a;
    target->field_7b3 = 0;
    if (attacker != NULL) {
        dz = attacker->collision_position.z - target->collision_position.z;
        dx = attacker->collision_position.x - target->collision_position.x;
        angle = NuAtan2D(dx, dz);
        target->field_0x276 = target->movement_facing_angle = target->facing_angle = angle;
    }
    index = (i16)target->field_7aa;
    if (target->character_model->model_data_b[index] != NULL &&
        (target->character_model->animation_configs[index]->flags & 2)) {
        target->context_timer = (f32)(((f64)qrand() * 1.5259021893143654e-05) * 0.5 + 1.0);
    } else {
        timer = AnimDuration((i16)target->id, (i16)index, 0.0f, 0.0f, target->field_7aa != 0xb9);
        target->context_timer = timer;
        if (timer <= 0.0f)
            target->context_timer = 0.6f;
    }
    if (attacker != NULL) {
        dx = target->collision_position.x - attacker->collision_position.x;
        dz = target->collision_position.z - attacker->collision_position.z;
        target->movement_facing_angle = NuAtan2D(-dx, -dz);
    }
    target->jump_variant_state = saved_jump_state;
    return target->field_7aa == 0xb9 ? 2 : 1;
}

/* Full PC ObjHitObj source contract: complete 5764-byte original interval,
   physical calling convention and finite reader cohorts independently reviewed.
   PC-only reaction and adaptive branches and PC DWORD masks are preserved.
   Existing partial part/object providers retain their explicit runtime gaps. */
typedef struct combat_reaction_owner_pc_s {
    u8 unknown_000[0x7bb];
    i8 attacker_slot;
} COMBAT_REACTION_OWNER_PC;
STATIC_ASSERT(offsetof(COMBAT_REACTION_OWNER_PC, attacker_slot) == 0x7bb);
extern f32 CombatReactionHorizontalScalePc, CombatReactionVerticalScalePc;
typedef struct combat_attack_counter_pc_s {
    u8 unknown_000[0x1083];
    u8 attack_count;
} COMBAT_ATTACK_COUNTER_PC;
STATIC_ASSERT(offsetof(COMBAT_ATTACK_COUNTER_PC, attack_count) == 0x1083);
extern i32 CombatHitReactionIntervalPc;
i32 __cdecl ObjHitObj(GAMEOBJECT *attacker, GAMEOBJECT *target, i32 damage, u16 flags, i32 probe, i32 ignored) {
    i32 reaction_trigger = 0;

    i32 no_hurt, no_impact, no_rumble, throw_up, random, hit, instant, result, health, coins, hearts;
    i16 *parts_angle;
    BOLT *bolt;
    u16 computed_angle;
    f32 random_unit;

    if (target == NULL || (target->flags_1fc_word & 0x1001) != 0x1001 || target->dead != 0 || target->field_1010 > 0.0f)
        return 0;
    no_hurt = objhitobj_nohurtsfx;
    no_impact = objhitobj_noimpactsfx;
    no_rumble = objhitobj_noattackerrumble;
    throw_up = objhitobj_throwkillpartsup;
    parts_angle = objhitobj_killparts_yrot;
    random = qrand();
    random_unit = (f32)((f64)random * 1.5259021893143654e-05);
    objhitobj_nohurtsfx = 0;
    objhitobj_noimpactsfx = 0;
    objhitobj_noattackerrumble = 0;
    objhitobj_throwkillpartsup = 0;
    objhitobj_killparts_yrot = NULL;
    bolt = objhitobj_bolt;
    objhitobj_bolt = NULL;
    if (Hub_InMenu() && ((target->flags_1fc_word & 0x80)))
        return 0;
    if (target->character_data->game_character->flags_090 & 0x8000)
        return 0;
    if (target->character_context == 95 || target->character_context == 96)
        return 0;
    if (target->character_context == 90 && (!target->field_7b3 || (attacker && !(attacker->flags_1fc_word & 0x80))))
        return 0;
    if (!(damage == -1 && (flags & 0x200))) {
        if (target->character_context == 0 &&
            (target->action_movement_state == 3 || target->action_movement_state == 4))
            return 0;
        if (target->character_context == 13 || target->character_context == 14)
            return 0;
    }
    if (CInfo[target->character_context].parameter & 4)
        return 0;
    if (attacker && !(attacker->flags_1fc_word & 1))
        attacker = NULL;
    if (target->field_280 != 0xff)
        ObstacleCamHoldUntilPlayersMove = 0;
    hit = damage;
    instant = damage == -1;
    if (target->character_context == 76 || target->character_context == 81) {
        hit = 0;
        instant = 0;
    }
    if (attacker && ((attacker->flags_1fc_word & 0x80)))
        AlertSurroundingCreatures(attacker, &target->collision_position);
    if (hit > 0 || instant)
        target->portal_flags |= 1;
    if (!(flags & 0x100)) {
        damage = 0;
    } else {
        if ((CInfo[target->character_context].flags & 0x4000000) ||
            (target->character_context != -1 && (target->character_context == LEGOCONTEXT_LAND_SLAM ||
                                                 target->character_context == LEGOCONTEXT_LAND_LUNGE))) {
            i32 rotation_difference = RotDiff(attacker->movement_facing_angle, target->movement_facing_angle);
            if (rotation_difference < 0)
                rotation_difference = -rotation_difference;
            if (rotation_difference > 0x4000 &&
                !(LEGOCONTEXT_COMBO != -1 && attacker->character_context == LEGOCONTEXT_COMBO &&
                  attacker->combo_branch == 6)) {
                if ((target->flags_1fc_word & 0x80))
                    NewRumble(target->pad->bound_pad, 0.75f, 0);
                if (LEGOCONTEXT_HOLD != -1 && target->character_context == LEGOCONTEXT_HOLD) {
                    NewBlockAction(target);
                    if ((target->id == id_IMPERIALGUARD || target->id == id_GAMORREANGUARD) &&
                        (target->character_context == 24 || target->character_context == 12))
                        GameAudio_PlaySfx(74, &target->collision_position, 0, 0);
                }
                return 0;
            }
        }
    }
    if (target->origin_scale_override >= 1.0f) {
        if ((flags & 0x100) &&
            (instant || (LEGOCONTEXT_COMBO != -1 && attacker->character_context == LEGOCONTEXT_COMBO &&
                         attacker->combo_branch == 6)))
            damage = target->shield_hitpoints;
        ObjHitShield(attacker, target, damage, bolt);
        return 0;
    }
    if (flags == 0)
        flags = ObjHitObj_Flags(attacker);
    if ((target->portal_flags & 0x08000000) || WORLD->current_level == VADERC_LDATA) {
        if ((u32)(hit) >= 2)
            hit = 1;
    } else if (attacker && (flags & 0x100) && attacker->character_context == 5 && attacker->combo_branch == 6 &&
               target->field_280 == 0xff) {
        hit = -1;
    }
    if ((target->portal_flags & 0x00080000) && (flags & 0x80)) {
        hit = 0;
    } else {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
            if (!disable_narrow_socks && (flags & 4) && ((target->flags_1fc_word & 0x80))) {
                hit = 0;
                goto attributed_hit;
            }
            if (target->id == id_SPEEDERBIKE && !(target->flags_1fc_word & 0x80)) {
                if (attacker && !(attacker->character_data->model_flags & 0x2000))
                    hit = 0;
                else if (target->ai_packet.creature_set != 2)
                    hit = 0;
                goto attributed_hit;
            }
        }
        if (WORLD->area == HOTHBATTLE_ADATA &&
            ((target->id == id_ATAT && !(target->flags_1fc_word & 0x80) &&
              (!attacker || !(attacker->flags_1fc_word & 0x80))) ||
             (attacker && attacker->id == id_ATAT && !(target->flags_1fc_word & 0x80) &&
              !(attacker->flags_1fc_word & 0x80)))) {
            if (hit == -1 && target->id == id_ATAT) {
                if (target->character_context != 23)
                    hit = 0;
            } else
                hit = attacker && attacker->id == id_ATAT && target->id == id_DRAGBOMB ? -1 : 0;
            goto attributed_hit;
        }
        if (flags & 1)
            goto attributed_hit;
        if ((flags & 2) && !(target->flags_1fc_word & 0x80) && WORLD->current_level != HUB_LDATA) {
            if (!StartupGameFlag1 && !(target->portal_flags & 0x08000000) && CombatHitReactionIntervalPc != 0 &&
                attacker != NULL && attacker->field_280 != 0xff && target->field_280 == 0xff &&
                !(target->flags_1fc_word & 2)) {
                COMBAT_ATTACK_COUNTER_PC *counter = (COMBAT_ATTACK_COUNTER_PC *)attacker;
                counter->attack_count = (u8)((counter->attack_count + 1) % CombatHitReactionIntervalPc);
                if (counter->attack_count == 0) {
                    reaction_trigger = 1;
                    goto attributed_hit;
                }
            }
            hit = 0;
            goto attributed_hit;
        }
        if ((flags & 4) && (target->flags_1f8 & 0x10405) == 0x400 && WORLD->current_level != HUB_LDATA) {
            hit = 0;
            goto attributed_hit;
        }
        if ((flags & 12) == 8 && ((target->flags_1fc_word & 0x80)) && WORLD->current_level != HUB_LDATA &&
            WORLD->current_level != VADERC_LDATA) {
            hit = 0;
            goto attributed_hit;
        }
        if ((MiniCutCam && ((target->flags_1fc_word & 0x80))) || target->pad == ViewCamGetGamePad() ||
            ((target->character_data->model_flags & 0x20000000) && target->rider == NULL))
            hit = 0;
        else if ((flags & 4) && !(target->flags_1fc_word & 0x80) && !(target->portal_flags & 0x08000000) &&
                 !StartupGameFlag1 && CombatHitReactionIntervalPc != 0 && attacker != NULL &&
                 !(target->flags_1fc_word & 2)) {
            COMBAT_ATTACK_COUNTER_PC *counter = (COMBAT_ATTACK_COUNTER_PC *)attacker;
            counter->attack_count = (u8)((counter->attack_count + 1) % CombatHitReactionIntervalPc);
            if (counter->attack_count == 0)
                reaction_trigger = 1;
        }
    }
attributed_hit:
    if (attacker) {
        target->last_attacker = attacker;
        if ((attacker->flags_1fc_word & 0x80)) {
            if (WORLD->current_level == HUB_LDATA &&
                AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "TakenHitFromPlayer")) {
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                                &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
                goto impact;
            }
        }
    }
    if (AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "TakenHit"))
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                        &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
impact:
    if (!no_impact) {
        if (attacker && attacker->id == id_BODYGUARD)
            PlaySfx("Grv_GuardImpact", &target->collision_position);
        else if (attacker && attacker->id == id_IMPERIALGUARD)
            PlaySfx("wpn_bib_stab", &target->collision_position);
        else if (bolt) {
            if (!(bolt->type_id >= 27 && bolt->type_id <= 29) && WORLD->area &&
                (WORLD->area == PODRACE_ADATA || WORLD->area == PODSPRINT_ADATA)) {
                PlaySfx("Pod_TuskHit", &bolt->position);
                no_hurt = 1;
            }
        } else if (VehicleArea || (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks)) {
            i32 bits = attacker && (u8)(attacker->field_280) <= 1 ? 1 << attacker->field_280 : 0;
            GameAudio_PlaySfx(40, &target->collision_position, bits, 0);
        } else if (flags & 0x40) {
            if (attacker && attacker->character_context == 38 &&
                (AnimMiscFlags(attacker->character_model, attacker->field_7aa) & 4))
                PlaySfx("WhipHit", &target->collision_position);
            else {
                i32 bits = attacker && (u8)(attacker->field_280) <= 1 ? 1 << attacker->field_280 : 0;
                GameAudio_PlaySfx(74, &target->collision_position, bits, 0);
            }
        } else if (attacker) {
            if (attacker->character_data->model_flags & 8)
                GameAudio_PlaySfx(65, &target->collision_position, GameAudio_GetPlrSfxBits(attacker), 0);
            else
                GameAudio_PlaySfx(74, &target->collision_position, 0, 0);
        }
    }
    result = 0;
    if (target->spawn_protection_timer > 0.0f || (target->flags_ef0 & 0x00400000)) {
        result = 0;
        if (target->flags_ef0 & 0x00001000)
            PlayerTakeHit(target, attacker);
        goto finish;
    }
    if (target->character_context == 21 && (target->portal_flags & 0x08000000)) {
        target->spawn_protection_timer = 2.5f;
        result = 0;
        goto finish;
    }
    if ((target->character_data->model_flags & 0x10) && !target->current_hp && (flags & 0x40) && random_unit > 0.75f) {
        DeactivatePlayer(target, 5.0f, NULL);
        result = 0;
        goto finish;
    }
    if (hit != -1 && !(target->vehicle_hit_timer <= 0.0f) && (!attacker || !(attacker->flags_1fc_word & 0x80)))
        return 0;
    if (target->character_context == 45 && !Player_HasFastBuild(target))
        GizBuildIt_SetToStart(target->grapple, 1, 1);
    Player_ClearContext(target, 0);
    ReleaseForce(target, 1);
    if (target->helmet && 1)
        LoseHelmet(target, 0, 0);
    if (hit != -1) {
        if (!target->current_hp || (((target->flags_1fc_word & 0x80)) && Player_HasInvincibility(target))) {
            if (target->id == id_ROYALGUARD || target->id == id_WOOKIEE)
                SetFlicker(target, 0.4f);
            PlayerTakeHit(target, attacker);
            result = 0;
            if ((target->flags_1fc_word & 0x80))
                TakeHitRumble(target, 0.666f);
            goto hurt;
        }
        health = target->current_hp - hit;
        if (health > 0) {
            if ((target->character_data->model_flags & 0x40000000) && target->character_context == 23)
                goto refill;
            goto surviving_hit;
        }
    } else
        health = 0;
    no_impact =
        players_cannot_exit_speeder && target->id == id_SPEEDERBIKE && target->rider && target->field_280 != 0xff;
    if ((target->character_data->model_flags & 0x20000000) && !ObjIsTargetSpeeder(target) && !no_impact) {
        coins = 0;
        if (((target->flags_1fc_word & 0x80)) && target->coin_packet &&
            ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
            coins = LoseCoins(target, 1);
        AddPickups(coins, 0, 0, 0, &target->collision_position, NULL, 2.0f, attacker ? (i8)attacker->field_280 : -1,
                   1.0f, 2000000.0f, attacker, 1, 0);
        DeactivatePlayer(target, 1000000000.0f, NULL);
        target->current_hp = target->field_107e;
        SetFlicker(target, 0.4f);
        result = 0;
        goto hurt;
    }
    if (target->character_data->model_flags & 0x40000000)
        goto refill;
    if ((target->portal_flags & 0x08000000) && (!FreePlay || WORLD->current_level != CRUISERC_LDATA))
        goto surviving_hit;
    if (probe) {
        result = 2;
        goto finish;
    }
    if (target->character_context == 93) {
        PopBalloon(target);
        target->current_hp = 1;
        result = 0;
        goto hurt;
    }
    if (!StartupGameFlag1) {
        u16 angle;
        f32 speed;
        COMBAT_REACTION_OWNER_PC *reaction = (COMBAT_REACTION_OWNER_PC *)target;
        if (reaction_trigger) {
            i32 reaction_result = PlayerTakeHitReactionPc(target, attacker, CombatReactionDurationPc, flags);
            if (reaction_result != 0) {
                target->current_hp = 1;
                SetFlicker(target, 0.4f);
                if (reaction_result == 2 && attacker != NULL && (attacker->flags_1fc_word & 0x80) && !no_rumble)
                    KillRumble(attacker);
                goto hurt;
            }
        }
        if (StartupGameFlag1 || (flags & 0x200) || attacker == NULL || target->field_280 != 0xff ||
            (target->portal_flags & 0x08000000) || qrand() >= 0x8000 || (target->flags_1fc_word & 2))
            goto ordinary_death;
        target->character_context = 95;
        target->field_7aa = 0xb8;
        if (target->character_model->model_data_b[0xb8] == NULL)
            target->field_7aa = 5;
        target->current_hp = 1;
        target->context_timer = 0.0f;
        target->context_duration = 2.0f;
        angle = NuAtan2D(target->collision_position.x - attacker->collision_position.x,
                         target->collision_position.z - attacker->collision_position.z);
        speed = (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.4000000059604645 + 0.800000011920929) *
                      CombatReactionHorizontalScalePc);
        target->external_force.x = NuTrigTable[angle >> 1] * speed;
        target->external_force.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
        target->movement_facing_angle = angle + 0x8000;
        reaction->attacker_slot = attacker->field_280 <= 1 ? (i8)attacker->field_280 : -1;
        if (qrand() < 0x5555) {
            target->context_variant_flags |= 0x100;
            target->velocity.y =
                (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.30000001192092896 + 0.20000000298023224) *
                      CombatReactionVerticalScalePc);
            target->external_force.x *= 2.0f;
            target->external_force.z *= 2.0f;
        } else {
            target->context_variant_flags &= ~0x100;
            target->velocity.y =
                (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.4000000059604645 + 0.800000011920929) *
                      CombatReactionVerticalScalePc);
        }
        ResetAnimPacket(&target->anim_packet, -1);
        SetFlicker(target, 0.4f);
        if ((attacker->flags_1fc_word & 0x80) && !no_rumble)
            KillRumble(attacker);
        goto hurt;
    }
ordinary_death:
    target->current_hp = 0;
    coins = 0;
    hearts = 0;
    if (target->field_280 == 0xff) {
        if (!(target->flags_1fc_word & 0x80)) {
            coins = BonusArea ? (u16)(target->character_data->game_character->coin_value) : (Cheat_IsOn(16) ? 350 : 0);
            hearts = ReleaseHearts();
        }
    } else if ((target->flags_1fc_word & 0x80)) {
        if (target->coin_packet && ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
            coins = LoseCoins(target, 1);
        if (!BuildUpTotal)
            BuildUpScale = 1.5f;
        DrawBuildUpTime = 1.0f;
        builduptime = 1.0f;
    }
    if (hearts || coins > 0)
        AddPickups(coins, hearts, 0, 0, &target->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, attacker,
                   !BonusArea && coins < 2500, BonusArea ? 1 : 0);
    if (target->torpedo && WORLD->area != SPEEDERCHASE_ADATA && (!WORLD->area || !(WORLD->area->flags & 4)))
        DropTorpedoPickups(target->torpedo, target->torpedo->count);
    if (!parts_angle) {
        if (!attacker)
            parts_angle = NULL;
        else if (flags & 0x240) {
            computed_angle =
                NuAtan2D(target->position.x - attacker->position.x, target->position.z - attacker->position.z);
            parts_angle = (i16 *)(&computed_angle);
        }
    }
    if (WORLD->current_level == VADERC_LDATA && !netclient) {
        GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, "FinalFight", NULL);
        if (message && message->value == 1.0f && (!attacker || ((attacker->flags_1fc_word & 0x80)))) {
            LevelCompletionFallbackFlagPC = 1;
            if (FreePlay)
                CompleteLevel(WORLD);
            else {
                char name[16];
                NUSPECIALHANDLE special;
                i32 i;
                for (i = 1; i != 12; ++i) {
                    sprintf(name, "rock%d", i);
                    if (NuSpecialFind(WORLD->current_gscn, &special, name, 1))
                        NuSpecialSetVisibility(&special, 0);
                }
                NewCutScene(NULL, WORLD->cutscene_sys,
                            attacker && attacker->id == id_OBIWANKENOBIEP3 && target->id == id_ANAKINJEDI
                                ? (char *)("ep3_darthvader_outro2")
                                : (char *)("ep3_darthvader_outro1"),
                            1);
            }
            return 0;
        }
    }
    if (no_impact && target->rider) {
        KillParts(target->rider, -1, target->rider->id == id_BODYGUARD ? 4 : -1, 1, 0.0f, 0, (u16 *)(parts_angle));
        KillGameObject(target->rider, 2, 0);
    }
    KillParts(target, -1, target->id == id_BODYGUARD ? 4 : -1, 1, throw_up ? 1.0f : 0.0f, 0, (u16 *)(parts_angle));
    KillGameObject(target, 2, 0);
    if ((target->flags_1fc_word & 0x80))
        GameCam_Judder(GameCam, 0.2f, 0, NULL);
    if (attacker != NULL && (attacker->flags_1fc_word & 0x80) && (target->flags_1f8 & 1) &&
        !(target->character_data->model_flags & 0x80000) && (flags & 4))
        AdaptiveDifficultyAdjustPc(&adaptivedifficulty, CombatNpcKillDifficultyAmountPc);
    else if ((target->flags_1fc_word & 0x80) && (flags & 2))
        AdaptiveDifficultyAdjustPc(&adaptivedifficulty, CombatPlayerDeathDifficultyAmountPc);
    result = 2;
    goto finish;
surviving_hit:
    if ((target->flags_1fc_word & 0x80))
        TakeHitRumble(target, 0.666f);
    target->current_hp = health;
    if (hit != 0) {
        if (AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "LostHitPoints"))
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                            &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
        SetFlicker(target, 0.4f);
        if (target->id == id_SNAKE)
            SnakeBeenHit(target);
        PlayerTakeHit(target, attacker);
        if (target->field_280 != 0xff && hit > 0) {
            u8 value = target->saved_hitpoints - hit;
            target->saved_hitpoints = value ? value : 1;
            if ((target->character_data->model_flags & 0x20) && target->use_model_origin) {
                i32 index = target->saved_hitpoints - 1;
                if (index > 2)
                    index = 2;
                index = objhit_damage_joints[2 - index];
                if (target->character_model->points_of_interest[index])
                    AddGameDebris(WORLD->game_debris_sys, 113, (NUVEC *)(&target->locator_matrices[index].m30));
            }
        }
    } else
        PlayerTakeHit(target, attacker);
    result = 1;
    if (target->id == id_BODYGUARD && target->current_hp == 1)
        KillParts(target, 4, -1, 1, 0.0f, 0, NULL);
    goto hurt;
hurt:
    if (!no_hurt && result != 2 && target->character_context != 23)
        PlayHurtSfx(target);
    goto finish;
finish:
    if (attacker && ((attacker->flags_1fc_word & 0x80))) {
        if (!no_rumble) {
            if (result == 2)
                KillRumble(attacker);
            else
                HitRumble(attacker);
        }
        if (!(flags & 0x4000) && result == 2 && (u8)(attacker->field_280) <= 1 && Arcade)
            Arcade_Kill((i8)attacker->field_280, (i8)target->field_280);
    }
    return result;
refill:
    coins = 0;
    if (((target->flags_1fc_word & 0x80)) && target->coin_packet &&
        ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
        coins = LoseCoins(target, 1);
    AddPickups(coins, 0, 0, 0, &target->collision_position, NULL, 2.0f, attacker ? (i8)attacker->field_280 : -1, 1.0f,
               2000000.0f, attacker, 1, 0);
    if (target->character_context != 23)
        SetFlicker(target, 0.4f);
    Player_ClearContext(target, 1);
    target->current_hp = target->field_107e;
    result = 0;
    goto hurt;
}
