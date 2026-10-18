#include "saga.h"
extern i16 CharacterFixupIdPc007f1a3c;

STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(NUGSCN, instance_animation_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, ltime) == 0x4c);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, flags) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, animation_index) == 0x5c);

SPEEDERCHASE_NETPACKET_PC *SpeederChaseNetPacketPc = NULL;
i32 SpeederChaseState0Pc = 0;
i32 SpeederChaseState1Pc = 0;
f32 SpeederChaseTime0Pc = 0.0f;
f32 SpeederChaseTime1Pc = 0.0f;

void __cdecl SpeederChaseA_Init(WORLDINFO *world) {
    SpeederChaseNetPacketPc = (SPEEDERCHASE_NETPACKET_PC *)SetLevelHack(7);
    SetLevelExBlowupFlags(3);
    SetLevelBlowupCallbackPc(SpeederBlowupHack);
    InitTrooperCannons(world);
    players_cannot_exit_speeder = 0;
    InitBikeParts();
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "clear1_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "clear1_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "clear2_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "clear2_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "clear1_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "clear3_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], "clear3_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], "clear4_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[8], "clear4_ffield2", 1);
    SpeederChaseTime1Pc = SpeederChaseTime0Pc = 0.0f;
    LevSfxFlag[0] = LevSfxFlag[1] = 0;
    ((u8 *)LevSfxFlag)[8] = 0;
    SpeederChaseState1Pc = SpeederChaseState0Pc = 0;
}

void __cdecl SpeederChaseA_Update(WORLDINFO *world) {
    GAMEOBJECT *object;
    NUINSTANIM_TIME_VIEW *animation;
    NUVEC *position;
    void *animation_data;
    f64 animation_time;
    i32 i;
    ResetTrooperCannons(world, CharacterFixupIdPc007f1a3c);
    UpdateTrooperCannons(world);
    if (netclient == 0) {
        if (LevAIMessage[4] != NULL) {
            players_cannot_exit_speeder = 1;
            if (LevAIMessage[4]->value != 1.0f)
                players_cannot_exit_speeder = 0;
        } else {
            players_cannot_exit_speeder = 0;
        }
    }
    for (i = 0; i < 2; ++i) {
        object = (GAMEOBJECT *)Player[i];
        if (object != NULL) {
            if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
                object->id == (u16)id_SPEEDERBIKE && object->rider != NULL && (object->rider->flags_1fc & 0x80) != 0)
                object->flags_ef4 |= 0x1000000;
            else
                object->flags_ef4 &= ~0x1000000;
        }
    }
    players_going_forward = 1;
    for (i = 0; i < 2; ++i) {
        object = (GAMEOBJECT *)Player[i];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 && (object->flags_1f8 & 0x40000) == 0) {
            if (GoingForwardsAlongNarrowSock(object) == 0 || ((GAMEOBJECT *)Player[i])->character_context == 0x2a) {
                players_going_forward = 0;
                break;
            }
        }
    }
    for (i = 0; i < 9; ++i) {
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
        position = NuSpecialGetDrawPos(&LevHSpecial[i]);
        if (animation != NULL) {
            animation_data = WORLD->current_gscn->instance_animation_data[animation->animation_index];
            if (((u8 *)LevSfxFlag)[i] == 0) {
                if (animation->ltime > 0.0f) {
                    animation_time = animation->ltime;
                    if (animation_time != NuAnimEndFrameOld(animation_data)) {
                        PlaySfx("env_laser_gate_on", position);
                        ((u8 *)LevSfxFlag)[i] = 1;
                    }
                }
            } else {
                if ((animation->flags & 1) == 0) {
                    animation_time = animation->ltime;
                    if (animation_time == NuAnimEndFrameOld(animation_data)) {
                        PlaySfx("env_laser_gate_off", position);
                        ((u8 *)LevSfxFlag)[i] = 0;
                        continue;
                    }
                }
                PlaySfx("env_laser_gate_lp", position);
            }
        }
    }
}

void __cdecl SpeederChaseA_Panel(WORLDINFO *world) {
    i16 targets[10];
    u8 defeated[10] = {0};
    i32 count, killed, i;
    (void)world;
    if (netclient != 0) {
        if (SpeederChaseNetPacketPc == NULL || SpeederChaseNetPacketPc->phase != 1 ||
            SpeederChaseNetPacketPc->target_count == 0)
            return;
        for (i = 0; i < SpeederChaseNetPacketPc->target_count; ++i) {
            targets[i] = id_SPEEDERBIKE;
            if (i >= SpeederChaseNetPacketPc->target_count - SpeederChaseNetPacketPc->killed_count)
                defeated[i] = 1;
        }
        count = SpeederChaseNetPacketPc->target_count;
    } else {
        if (LevAIMessage[0] == NULL || LevAIMessage[1] == NULL || LevAIMessage[3] == NULL ||
            LevAIMessage[3]->value != 1.0f || !(LevAIMessage[0]->value > 0.0f))
            return;
        count = (i32)LevAIMessage[0]->value;
        if (count > 10)
            count = 10;
        killed = (i32)LevAIMessage[1]->value;
        for (i = 0; i < count; ++i) {
            targets[i] = id_SPEEDERBIKE;
            if (i >= count - killed)
                defeated[i] = 1;
        }
    }
    DrawMeleeTargetsPc(targets, defeated, count);
}
