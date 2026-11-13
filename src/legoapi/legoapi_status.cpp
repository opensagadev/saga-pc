#include <stdio.h>
#include "saga.h"
#pragma bss_seg(".bss$goldbrickmsgcount")
static i32 goldbrickmsgcount = 0;

/* Retail unit 0x482c00-0x486f20 (legoapi/misc/legoapi_status.cpp): definitions in retail address order. */

extern "C" {
    extern f32 DrawStatusScaleXPc, DrawStatusScaleYPc;
    void __cdecl NuQFntSetMtx(VUFNT *, NUMTX *);
    void __cdecl DrawCharIcon(i32 character_id, f32 x, f32 y, f32 z, f32 scale, i32 frame_object_id,
                              f32 character_alpha, f32 frame_alpha, i32 draw_character,
                              NUSPECIALHANDLE *override_special);
    i32 __cdecl AddToCollection(i32);
    extern f32 StatusIconYPc;
    extern i32 StatusDrawPlayerIconsPc;
}
/* Native walks the two adjacent 0x14-byte rumble records. */
typedef struct STATUS_RUMBLE_PAIR_PC {
    RUMBLE_PACKET_RESOURCE_PC players[2];
} STATUS_RUMBLE_PAIR_PC;
typedef struct StatusPlayerModelPairPc {
    u16 model[2];
} StatusPlayerModelPairPc;
#define STATUS_APPEND_STAGE_INLINE_PC(packet_arg, type_arg, gold_arg)                                                  \
    do {                                                                                                               \
        (packet_arg)->stage_types[(packet_arg)->stage_count] = (i8)(type_arg);                                         \
        (packet_arg)->gold_brick_enabled[(packet_arg)->stage_count] = (u8)(gold_arg);                                  \
        ++(packet_arg)->stage_count;                                                                                   \
    } while (0)
extern "C" void __cdecl GrabStillScreen(void);
extern "C" __declspec(noinline) void NeedScreenGrab(i32 needed);
typedef struct status_world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
} STATUS_WORLD_MINIKIT_READER_PC;
extern "C" {
    extern f32 StatusMiniKitProgressPc;
    extern f32 StatusMiniKitJibberLengthPc;
}
extern "C" i16 TextIdPc880bc0;
extern "C" {
    extern i16 TextIdPc8806f8, TextIdPc880704, TextIdPc880744, TextIdPc88078c, TextIdPc880790, TextIdPc88083c;
}

extern "C" {
    void __cdecl AddToCompletionPoints(u32 points);
    i32 AllMiniKitsDone(AREASAVE_s *save);
}
extern "C" {

    f32 __cdecl StatusScoreRatioPc(f32 numerator, f32 denominator) {
        if (numerator != 0.0f && denominator != 0.0f) {
            f32 result = (f64)numerator / (f64)denominator;
            return result;
        }
        return 0.0f;
    }
}

static void StatusMiniKitRotateXPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = (m01 * cosx) - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static void StatusMiniKitRotateYPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static void StatusMiniKitRotateZPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = (m00 * cosx) - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}
extern "C" {
    i32 StatusGoldBrickStagePc = 0;
    i32 StatusGoldBrickPhasePc = 0;

    __declspec(noinline) void __cdecl SetDrawGoldBrick(STATUSPACKET_s *packet, i32 unused) {
        i32 index = packet->current_gold_brick;
        (void)unused;
        if (packet->gold_brick_enabled[index] != 0) {
            STATUS_STAGE_s *stage = packet->stage;
            StatusGoldBrickStagePc = index;
            StatusGoldBrickPhasePc = stage->field_0x14;
        }
    }
    static NUMTX RenderStatusMatrixPrivate = {0};

    void __cdecl DrawStatusText(char *text, u16 angle, f32 x, f32 y, f32 scale, u32 colour, i32 alignment) {
        NUVEC origin = {0.0f, 0.0f, 800.0f};
        NUVEC position = {x * DrawStatusScaleXPc, y * DrawStatusScaleYPc, 0.0f};
        u16 encoded[512];
        NuQFntSetJustifiedTolerances(1.2f, 1.2f);
        NuMtxSetIdentity(&RenderStatusMatrixPrivate);
        NuMtxSetRotationX(&RenderStatusMatrixPrivate, 0);
        NuMtxRotateY(&RenderStatusMatrixPrivate, 0);
        NuMtxRotateZ(&RenderStatusMatrixPrivate, angle);
        NuMtxTranslate(&RenderStatusMatrixPrivate, &origin);
        NuMtxTranslate(&RenderStatusMatrixPrivate, &position);
        NuMtxMul(&RenderStatusMatrixPrivate, &RenderStatusMatrixPrivate, NuCameraGetMtx());
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &RenderStatusMatrixPrivate);
        NuQFntPushPrintMode(4);
        NuQFntSetCoordinateSystem(4);
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, scale, scale);
        Text3DStringEncode(text, encoded);
        switch (alignment) {
            case 2:
                NuQFntMove(QFont3DZ, 0.0f, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
            case 8:
                NuQFntMove(QFont3DZ, -NuQFntPrintLenW(QFont3DZ, encoded), (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
            default:
                NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5),
                           (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
        }
        NuQFntPrintW(QFont3DZ, encoded);
        NuQFntPopPrintMode();
        NuQFntSetCoordinateSystem(3);
    }

    void __cdecl DrawStatusTextFraction(i32 current, i32 total, f32 x, f32 y, u16 angle, f32 scale, u32 colour,
                                        f32 animation, f32 animation_duration) {
        char text[16];
        u16 encoded[64];
        NUMTX matrix;
        NUVEC origin = {0.0f, 0.0f, 800.0f};
        NUVEC position = {(f32)((f64)x * 400.0), (f32)((f64)y * 250.0), 0.0f};
        f32 full_width, height;
        NuQFntSetJustifiedTolerances(1.2f, 1.2f);
        NuMtxSetIdentity(&matrix);
        NuMtxSetRotationX(&matrix, 0);
        NuMtxRotateY(&matrix, 0);
        NuMtxRotateZ(&matrix, angle);
        NuMtxTranslate(&matrix, &origin);
        NuMtxTranslate(&matrix, &position);
        NuMtxMul(&matrix, &matrix, NuCameraGetMtx());
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &matrix);
        NuQFntPushPrintMode(4);
        NuQFntSetCoordinateSystem(4);
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, scale, scale);
        sprintf(text, "%i/%i", current, total);
        Text3DStringEncode(text, encoded);
        full_width = NuQFntPrintLenW(QFont3DZ, encoded);
        height = NuQFntHeight(QFont3DZ);
        NuQFntMove(QFont3DZ, (f32)(-full_width * 0.5), (f32)(-height * 0.5), 0.0f);
        if (animation > 0.0f) {
            i32 wave_angle = (i32)(((f64)animation / (f64)animation_duration) * 32768.0 + 16384.0);
            f64 blend = 1.0 - ((f64)NuTrigTable[(wave_angle >> 1) & 0x7fff] + 1.0) * 0.5;
            f64 wave = 1.0 - blend;
            f64 half_wave = wave * 0.5;
            f32 large_scale = (f32)(((f64)scale + 0.5) - half_wave);
            f32 small_scale = (f32)(((f64)scale - 0.5) + half_wave);
            f64 half_full_width, current_center;
            f32 suffix_x, current_width, large_x, small_x;
            sprintf(text, "/%i", total);
            Text3DStringEncode(text, encoded);
            half_full_width = full_width * 0.5;
            suffix_x = (f32)((full_width - NuQFntPrintLenW(QFont3DZ, encoded)) - half_full_width);
            NuQFntMove(QFont3DZ, suffix_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
            NuQFntPrintW(QFont3DZ, encoded);
            sprintf(text, "%i", current);
            Text3DStringEncode(text, encoded);
            current_width = NuQFntPrintLenW(QFont3DZ, encoded);
            NuQFntSetScale(QFont3DZ, large_scale, large_scale);
            current_center = (f64)current_width * 0.5 - half_full_width;
            large_x = (f32)(current_center - NuQFntPrintLenW(QFont3DZ, encoded) * 0.5);
            NuQFntMove(QFont3DZ, large_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
            NuQFntPrintW(QFont3DZ, encoded);
            NuQFntSetScale(QFont3DZ, small_scale, small_scale);
            small_x = (f32)(current_center - NuQFntPrintLenW(QFont3DZ, encoded) * 0.5);
            NuQFntMove(QFont3DZ, small_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
            NuQFntPrintW(QFont3DZ, encoded);
        } else {
            sprintf(text, "%i", current);
            Text3DStringEncode(text, encoded);
            NuQFntPrintW(QFont3DZ, encoded);
            sprintf(text, "/%i", total);
            Text3DStringEncode(text, encoded);
            NuQFntPrintW(QFont3DZ, encoded);
        }
        NuQFntPopPrintMode();
        NuQFntSetCoordinateSystem(3);
    }

    STATUS_STAGE_s *FindStatusStage(i32 type) {
        i32 index;
        for (index = 0; StatusStages[index].type != -1; ++index) {
            if (StatusStages[index].type == type)
                return &StatusStages[index];
        }
        return NULL;
    }

    /* Complete native status-control foundation. No registration/table activation. */
    void __cdecl StatusStage_Reset(STATUS_STAGE_s *stage) {
        if (stage != NULL) {
            stage->field_0x14 = -1;
            stage->field_0x18 = 0.0f;
            stage->field_0x12 = 0;
            stage->field_0x1c = 1.0f;
        }
    }

    /* Native status-stage handlers call this shared routine. */
    __declspec(noinline) void NewStatusRumbleBuzz(i32 player, float amount, float buzz, i32 priority) {
        if (StatusPacket.player0_active != 0 && (player == -1 || player == 0)) {
            if (amount > 0.0f && (StatusPacket.player0_rumble_time <= 0.0f ||
                                  amount > StatusPacket.player0_rumble_time / StatusPacket.player0_rumble_duration *
                                               StatusPacket.player0_rumble_amount)) {
                StatusPacket.player0_rumble_amount = amount;
                StatusPacket.player0_rumble_duration = amount;
                StatusPacket.player0_rumble_time = amount;
            }
            if (buzz > StatusPacket.player0_buzz_amount) {
                StatusPacket.player0_buzz_amount = buzz;
            }
            if (priority > 0) {
                const i32 old_priority = StatusPacket.player0_rumble_priority;
                priority = (i32)((u32)priority + 1u);
                if (priority > old_priority) {
                    StatusPacket.player0_rumble_priority = (u8)(priority);
                }
            }
        }

        if (StatusPacket.player1_active != 0 && (player == -1 || player == 1)) {
            if (amount > 0.0f && (StatusPacket.player1_rumble_time <= 0.0f ||
                                  amount > StatusPacket.player1_rumble_time / StatusPacket.player1_rumble_duration *
                                               StatusPacket.player1_rumble_amount)) {
                StatusPacket.player1_rumble_amount = amount;
                StatusPacket.player1_rumble_duration = amount;
                StatusPacket.player1_rumble_time = amount;
            }
            if (buzz > StatusPacket.player1_buzz_amount) {
                StatusPacket.player1_buzz_amount = buzz;
            }
            if (priority > 0) {
                const i32 old_priority = StatusPacket.player1_rumble_priority;
                priority = (i32)((u32)priority + 1u);
                if (priority > old_priority) {
                    StatusPacket.player1_rumble_priority = (u8)(priority);
                }
            }
        }
    }

    void __cdecl StatusRumbleUpdatePc(void) {
        RUMBLE_PACKET_RESOURCE_PC *packet = ((STATUS_RUMBLE_PAIR_PC *)&StatusPacket.player0_rumble_amount)->players;
        i32 player;
        for (player = 0; player < 2; ++player, ++packet) {
            i32 motor1;
            UpdateRumble(packet);
            if (Game.options_save.unknown_0000[player] != 0 && StatusPacket.player_active[player] != 0 &&
                ((u8)GamePadStorage[player].allocated & 0x20) != 0) {
                if (packet->amount > 0.0f)
                    motor1 = (i32)((packet->intensity * packet->amount) * 255.0f / packet->decay_duration);
                else
                    motor1 = 0;
                NuPadSetMotorsPc(GamePadStorage[player].pad, packet->duration > 0.0f || packet->active > 0, motor1);
            } else {
                NuPadSetMotorsPc(GamePadStorage[player].pad, 0, 0);
            }
        }
    }

    void __cdecl NextStatusStage(STATUSPACKET_s *packet) {
        if (packet->stage != NULL && packet->stage->field_0x14 == -1 &&
            packet->stage->field_0x18 < packet->field_0x68) {
            packet->flags_word |= 2;
            return;
        }
        if (packet->stage != NULL)
            packet->stage->field_0x12 = 1;
        packet->previous_stage_2 = packet->previous_stage;
        packet->current_gold_brick = (i8)((u8)packet->current_gold_brick + 1);
        packet->previous_stage = packet->stage;
        packet->stage = packet->next_stage;
        packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
        while (packet->stage == NULL || packet->next_stage == NULL) {
            if (packet->stage == NULL && packet->next_stage != NULL) {
                packet->stage = packet->next_stage;
                packet->next_stage = NULL;
            }
            packet->current_gold_brick = (i8)((u8)packet->current_gold_brick + 1);
            if (packet->current_gold_brick + 1 >= packet->stage_count)
                packet->next_stage = FindStatusStage(10);
            else
                packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
        }
        StatusStage_Reset(packet->stage);
        packet->flags_word &= ~2u;
        status_prompt = 0;
        if ((packet->stage != NULL && packet->stage->type == 11) || packet->stage->type == 10) {
            ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&packet->player0_rumble_amount);
            ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&packet->player1_rumble_amount);
        }
        if (packet->stage != NULL && packet->stage->type == 11)
            ReCalculateCompletionPoints();
    }

    i32 AddGoldBrickMessage(STATUSPACKET_s *packet, i16 brick) {
        if (goldbrickmsgcount < 16) {
            ((i16 *)(packet->field_0x12c))[goldbrickmsgcount] = brick;
            ++goldbrickmsgcount;
        }
        AddToGoldBricks();
        return 1;
    }

    __declspec(noinline) void AddStatusStage(STATUSPACKET_s *packet, i32 type, i32 gold_brick_enabled) {
        packet->stage_types[packet->stage_count] = (i8)(type);
        packet->gold_brick_enabled[packet->stage_count] = (u8)(gold_brick_enabled);
        ++packet->stage_count;
    }
    i16 StatusNewCharacterIdsPc[8] = {0};

    i32 __cdecl newCharactersCollected(STATUSPACKET_s *packet) {
        i32 count = 0;
        if (StartupGameFlag1 == 0) {
            if ((((u8 *)&packet->lsw_packet->field_0x00)[3] & 1) != 0 && Game.initial_store_pack_flags != areaSuitBits)
                return 1;
        } else {
            i16 temple_ids[2];
            i16 *ids;
            temple_ids[0] = VehicleBoltId7f17d4Pc;
            temple_ids[1] = -1;
            ids = packet->area != NULL && packet->area == LOSTTEMPLE_ADATA ? temple_ids : Area_PlayerIDList;
            while (*ids != -1 && count < 7) {
                i32 id = *ids, previous = 0;
                while (previous < count && StatusNewCharacterIdsPc[previous] != id)
                    ++previous;
                if (previous == count && (Game_CharacterSave == NULL || (Game_CharacterSave[id] & 1) == 0)) {
                    i32 found = InCollectList_Index(id, MasterCollectionPc.list, MasterCollectionPc.count);
                    if (found != -1) {
                        COLLECTID_PC *entry = &MasterCollectionPc.list[found];
                        if (entry->shop_enabled == 0) {
                            StatusNewCharacterIdsPc[count++] = (i16)id;
                            if (entry->unknown_price_tail[0] != 0)
                                AddToCompletionPoints(POINTS_PER_CHARACTER);
                        }
                    }
                }
                ++ids;
            }
            StatusNewCharacterIdsPc[count] = -1;
        }
        return count;
    }

    void __cdecl StatusPacketReset(STATUSPACKET_s *packet) {
        struct STATUSPACKET_LSW_s *lsw = packet->lsw_packet;
        void(__cdecl * reset)(STATUSPACKET_s *) = packet->reset_callback;
        i32(__cdecl * init)(WORLDINFO *, STATUSPACKET_s *) = packet->init_callback;
        i32(__cdecl * finish)(WORLDINFO *, STATUSPACKET_s *, i32) = packet->finish_callback;
        void(__cdecl * draw)(STATUSPACKET_s *) = packet->draw_background_callback;
        f32 delay = packet->field_0x68;
        reset(packet);
        memset(packet, 0, sizeof(*packet));
        packet->lsw_packet = lsw;
        packet->finish_callback = finish;
        packet->reset_callback = reset;
        packet->draw_background_callback = draw;
        packet->field_0x68 = delay;
        packet->init_callback = init;
    }
    i32 StatusNewMinikitCountPc = 0;
    i32 StatusCurrentMinikitPc = 0;
    u32 StatusCollectedSuitBitsPc = 0;

    void __cdecl InitStatusScreen(WORLDINFO *world) {
        STATUSPACKET_s *p = &StatusPacket;
        i32 i, area, episode;
        if (p->lsw_packet == NULL)
            return;
        episode = -1;
        for (i = 0; StatusStages[i].type != -1; ++i)
            StatusStage_Reset(&StatusStages[i]);
        StatusPacketReset(p);
        StatusIconYPc = STATSPOSY;
        p->flags_word &= ~1u;
        StatusNewMinikitCountPc = 0;
        StatusCurrentMinikitPc = 0;
        ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&p->player0_rumble_amount);
        ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&p->player1_rumble_amount);
        CoinTotalScale = 1.0f;
        goldbrickmsgcount = 0;
        if (netclient == 0) {
            StatusPlayerModelPairPc *models = (StatusPlayerModelPairPc *)&p->player0_model;
            for (i = 0; i < 2; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL) {
                    models->model[i] = (u16)object->id;
                    p->player_active[i] = (u8)((object->flags_1fc_word >> 7) & 1);
                    if (object->coin_packet != NULL) {
                        p->coins_remaining[i] = ((PLAYER_COIN_PACKET_PC *)object->coin_packet)->coins;
                        ++p->status_flags;
                    } else {
                        p->coins_remaining[i] = 0;
                        ++p->status_flags;
                    }
                } else
                    models->model[i] = 0xffff;
            }
        }
        if (p->status_flags != 2) {
            p->status_flags = 0;
            return;
        }
        p->flags_word = (p->flags_word & ~0x400u) | (((u32)SuperStory << 10) & 0x400u);
        p->flags_word = (p->flags_word & ~0x40u) | (((u32)FreePlay << 6) & 0x40u);
        p->flags_word = (p->flags_word & ~0x800u) | (((u32)PauseSaveAndExitPc << 11) & 0x800u);
        p->flags_word &= ~0x1000u;
        p->stage_count = 0;
        p->mission = Mission_Active(NULL);
        if (p->mission != NULL)
            p->mission_state = (u8)Mission_CurrentState(NULL);
        else
            p->mission_state = 0;
        p->challenge_state = (u8)ChallengeMode;
        PauseSaveAndExitPc = 0;
        area = (i8)world->area_index;
        p->flags_word &= ~8u;
        p->previous_completion = (u8)Game.save_hash;
        p->flags_word &= ~0x380u;
        p->next_area = -1;
        p->field_0xbe = -1;
        p->score = &Game.coins;
        p->previous_gold_bricks = Game.gold_bricks;
        p->displayed_gold_bricks = Game.gold_bricks;
        p->newly_completed = 0;
        p->field_0xbd = 0;
        p->area_id = (i16)area;
        if (area != -1) {
            p->area = &ADataList[area];
            p->episode_id = p->area->episode_index;
            episode = p->episode_id;
            if (episode != -1)
                p->episode = &EDataList[(i8)p->area->episode_index];
            p->chapter = p->area->area_index;
            if ((p->area->flags & 1) != 0)
                p->flags_word |= 0x80;
            if ((p->area->flags & 4) != 0)
                p->flags_word |= 0x100;
            if ((p->area->flags & 0x100) != 0)
                p->flags_word |= 0x200;
        } else {
            p->area = NULL;
            p->episode = NULL;
            p->chapter = -1;
        }
        if (p->init_callback(world, p) != 0)
            return;
        if ((p->flags_word & 0x400) != 0) {
            /* Retail tests the current level separately for loaded and unloaded worlds. */
            if (NewLData == STATUS_LDATA || (WORLD->loaded && WORLD->current_level == STATUS_LDATA) ||
                (!WORLD->loaded && WORLD->current_level == STATUS_LDATA)) {
                struct EPISODESAVE_s *save = &Game.episode_save[episode];
                i32 gold = 0;
                p->previous_best_time = save->superstory_time_limit;
                p->previous_best_score = save->superstory_score_target;
                p->superstory_time = SuperStoryTimer.time_elapsed;
                p->superstory_score = SuperStoryScore;
                if (episode != -1) {
                    if (p->previous_best_time > p->superstory_time) {
                        p->new_best_time = p->superstory_time;
                        save->superstory_time_limit = p->superstory_time;
                    } else
                        p->new_best_time = 0.0f;
                    if (p->superstory_score > p->previous_best_score) {
                        p->new_best_score = p->superstory_score;
                        save->superstory_score_target = p->superstory_score;
                    } else
                        p->new_best_score = 0;
                    if (save->superstory_complete == 0) {
                        save->superstory_complete = 1;
                        AddToCompletionPoints(POINTS_PER_SUPERSTORY);
                        if (EpisodeGoldBrickForSuperStoryPc != 0)
                            gold = AddGoldBrickMessage(p, TextIdPc88073c);
                    }
                } else {
                    p->new_best_score = 0;
                    p->new_best_time = 0.0f;
                }
                p->time_reward_score = *p->score;
                if (p->new_best_time != 0.0f) {
                    i32 whole = (i32)((f64)p->previous_best_time - (f64)p->new_best_time);
                    i32 scaled = (i32)((u32)whole * 100u);
                    u64 reward = (u64)(i64)scaled;
                    if (reward < 100)
                        reward = 100;
                    IncreaseScore(&p->time_reward_score, reward, 0);
                }
                p->final_reward_score = p->time_reward_score;
                IncreaseScore(&p->final_reward_score, (u64)p->superstory_score, 0);
                STATUS_APPEND_STAGE_INLINE_PC(p, 14, gold);
                STATUS_APPEND_STAGE_INLINE_PC(p, 15, 0);
                STATUS_APPEND_STAGE_INLINE_PC(p, 16, 0);
                if (Game.gold_bricks > p->previous_gold_bricks)
                    AddStatusStage(p, 19, 0);
                if (netclient == 0 && (p->new_best_time != 0.0f || p->new_best_score != 0 || p->superstory_score != 0))
                    AddStatusStage(p, 11, 0);
                hub_from_superstory = (i8)p->area->episode_index;
            }
            STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            if (episode != -1 && p->chapter < EDataList[episode].regular_areas) {
                p->next_area = EDataList[episode].area_ids[p->chapter + 1];
                if (p->next_area != -1 && (ADataList[p->next_area].flags & 2) != 0)
                    hub_from_superstory = episode;
            }
            return;
        }
        if ((p->flags_word & 0x100) != 0 && area != -1) {
            i32 gold = 0;
            p->elapsed_time = BonusTimer.time_elapsed;
            p->flags_word = (p->flags_word & ~0x20u) | (((u32)BonusWinner << 5) & 0x20u);
            p->previous_best_time = Game.area_save[area].challenge_trial_time;
            if (p->previous_best_time <= 0.0f || p->previous_best_time > p->elapsed_time) {
                if (WORLD->area->challenge_trial_time != 0 &&
                    p->previous_best_time >= WORLD->area->challenge_trial_time) {
                    AddToCompletionPoints(POINTS_PER_TIMETRIAL);
                    gold = AddGoldBrickMessage(p, TextIdPc88071c);
                }
                p->new_best_time = p->elapsed_time;
                Game.area_save[area].challenge_trial_time = p->elapsed_time;
            }
            if ((p->flags_word & 0x200) != 0) {
                if (Game.area_save[p->area_id].area_complete == 0) {
                    Game.area_save[p->area_id].area_complete = 1;
                    AddToCompletionPoints(POINTS_PER_SUPERBONUSCOMPLETE);
                    p->newly_completed = 1;
                    if (EpisodeGoldBrickForSuperBonusPc != 0)
                        gold = AddGoldBrickMessage(p, TextIdPc880728);
                }
                AddStatusStage(p, 22, gold);
                gold = 0;
            } else
                AddStatusStage(p, 20, 0);
            AddStatusStage(p, 21, gold);
            if (Game.gold_bricks > p->previous_gold_bricks)
                AddStatusStage(p, 19, 0);
            AddStatusStage(p, 9, 0);
            AddStatusStage(p, 10, 0);
            if (netclient == 0)
                AddStatusStage(p, 11, 0);
            AddStatusStage(p, 12, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIconsPc = 1;
            return;
        }
        if (p->mission_state != 0) {
            i32 gold = 0;
            p->newly_completed = 0;
            p->original_score = *p->score;
            p->reward_score = p->original_score;
            p->elapsed_time = MissionSys->timer.time_elapsed;
            if (p->mission_state == 2) {
                if (Game_MissionSave == NULL ||
                    Game_MissionSave->completed[(i8)MissionSys->active_mission->index] == 0) {
                    if (Game_MissionSave != NULL)
                        Game_MissionSave->completed[(i8)MissionSys->active_mission->index] = 1;
                    AddToCompletionPoints(POINTS_PER_MISSION);
                    gold = AddGoldBrickMessage(p, TextIdPc8806e4);
                }
                {
                    i32 whole = (i32)((f64)p->mission->time - (f64)p->elapsed_time);
                    i32 scaled = (i32)((u32)whole * 150u);
                    u64 reward = (u64)(i64)scaled;
                    if (reward < 150)
                        reward = 150;
                    IncreaseScore(&p->reward_score, reward, 0);
                }
                if (Game_MissionSave != NULL) {
                    f32 *best = &Game_MissionSave->best_times[(i8)MissionSys->active_mission->index];
                    if (*best == 0.0f || *best > p->elapsed_time)
                        *best = p->elapsed_time;
                }
            }
            AddStatusStage(p, 26, gold);
            if (Game.gold_bricks > p->previous_gold_bricks)
                AddStatusStage(p, 19, 0);
            STATUS_APPEND_STAGE_INLINE_PC(p, 9, 0);
            STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
            if (netclient == 0)
                AddStatusStage(p, 11, 0);
            STATUS_APPEND_STAGE_INLINE_PC(p, 12, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIconsPc = 1;
            cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
            return;
        }
        if (p->challenge_state != 0) {
            i32 first = 0;
            p->elapsed_time = ChallengeTimer.time_elapsed;
            p->original_score = *p->score;
            p->reward_score = p->original_score;
            if (p->challenge_state == 2) {
                if (Game.area_save[p->area_id].challenge_unlocked == 0) {
                    AddToCompletionPoints(POINTS_PER_CHALLENGE);
                    Game.area_save[p->area_id].challenge_unlocked = 1;
                    first = 1;
                }
                {
                    i32 whole = (i32)((f64)ADataList[p->area_id].challenge_trial_time - (f64)p->elapsed_time);
                    i32 scaled = (i32)((u32)whole * 500u);
                    u64 reward = (u64)(i64)scaled;
                    if (reward < 500)
                        reward = 500;
                    IncreaseScore(&p->reward_score, reward, 0);
                }
            }
            AddStatusStage(p, 23, 0);
            if (p->challenge_state == 2 && first != 0)
                AddStatusStage(p, 25, 0);
            AddStatusStage(p, 9, 0);
            AddStatusStage(p, 10, 0);
            if (netclient == 0)
                AddStatusStage(p, 11, 0);
            AddStatusStage(p, 12, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIconsPc = 1;
            cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
            return;
        }
        if (area != -1) {
            if (BuildUpDone != 0)
                p->flags_word |= 4;
            p->minikit_count = (u8)(Game.area_save[area].minikit_count + (u8)AreaGlobals_PC.new_minikit_count);
            p->minikit_max = (p->area->flags & 0x10) != 0 ? 10 : 0;
            if (p->minikit_count > p->minikit_max)
                p->minikit_count = p->minikit_max;
            p->true_hero_target = (f32)p->area->true_hero_targets[(p->flags_word >> 6) & 1];
        }
        p->area_time = AreaTimer.time_elapsed;
        if ((u64)p->coins_remaining[0] + (u64)p->coins_remaining[1] >= 4000000000ULL)
            p->collected_score = 4000000000.0f;
        else
            p->collected_score = (f32)(p->coins_remaining[0] + p->coins_remaining[1]);
        p->newly_completed = 0;
        if ((p->field_0xb0 & 4) != 0)
            p->true_hero_percent = 100.0f;
        else {
            f32 target = p->true_hero_target;
            f32 numerator = (f64)p->collected_score * 100.0;
            p->true_hero_percent = StatusScoreRatioPc(numerator, target);
            if (p->true_hero_percent > 99.0f)
                p->true_hero_percent = 99.0f;
        }
        p->coins_collected[0] = p->coins_remaining[0];
        p->coins_collected[1] = p->coins_remaining[1];
        if (area != -1 && (p->field_0xb0 & 0x40) == 0 && p->episode != NULL && p->chapter < p->episode->regular_areas) {
            p->next_area = p->episode->area_ids[p->chapter + 1];
            if (p->chapter < p->episode->regular_areas - 1)
                p->flags_word |= 8;
        }
        StatusDrawPlayerIconsPc = 0;
        if (area != -1) {
            for (i = 0; i < AreaGlobals_PC.new_minikit_count; ++i) {
                LEVELSAVE_s *save = &((LEVELSAVE_s *)Game.level_save)[NewMiniPiece[i].level];
                if (Game.area_save[area].minikit_count < 10)
                    ++Game.area_save[area].minikit_count;
                if (save->minikit_count < 10) {
                    NuStrCpy(save->minikit_names[save->minikit_count], NewMiniPiece[i].name);
                    ++save->minikit_count;
                }
            }
        }
        {
            i32 gold = 0, completed_episode = 0;
            if (area != -1 && (p->field_0xb0 & 0x40) == 0 && Game.area_save[area].area_complete == 0 &&
                (p->area->flags & 0x26) == 0) {
                p->newly_completed = 1;
                Game.area_save[area].area_complete = 1;
                AddToCompletionPoints(POINTS_PER_STORY);
                if ((p->flags_word & 0x100) == 0 && (p->area->flags & 0x800) == 0)
                    gold = AddGoldBrickMessage(p, TextIdPc880728);
                if (p->episode != NULL && Episode_IsComplete(p->episode, NULL) != 0)
                    completed_episode = (i8)p->area->episode_index;
                if (StartupGameFlag1 != 0 && p->area == NEGOTIATIONS_ADATA) {
                    for (i = 1; i < 6; ++i)
                        Game.area_save[EDataList[i].area_ids[0]].complete = 1;
                }
                if ((p->field_0xb0 & 8) != 0)
                    Game.area_save[p->next_area].complete = 1;
                else if (p->episode != NULL) {
                    i32 bonus = Episode_FindAreaFromFlags(p->episode, 5, 4);
                    i32 bonus2 = Episode_FindAreaFromFlags(p->episode, 5, 5);
                    if (bonus != -1)
                        Game.area_save[bonus].complete = 1;
                    if (bonus2 != -1)
                        Game.area_save[bonus2].complete = 1;
                    hub_startoutsidebonusdoor_area = bonus2;
                }
            }
            if (p->newly_completed != 0)
                AddStatusStage(p, 13, gold);
            if (completed_episode != 0)
                AddStatusStage(p, completed_episode + 29, 0);
        }
        if (GOTHAMSTREETS_ADATA != NULL && p->newly_completed != 0 && p->area == GOTHAMSTREETS_ADATA)
            AddStatusStage(p, 8, 0);
        if (p->area != NULL && (u8)p->area->cheat != 0xff && Game.area_save[area].red_brick_collected == 0 &&
            AreaGlobals_PC.field_08 != 0) {
            AddToCompletionPoints(POINTS_PER_REDBRICK);
            Game.area_save[area].red_brick_collected = 1;
            AddStatusStage(p, 18, 0);
        }
        if (p->newly_completed != 0 && newCharactersCollected(p) > 0) {
            if (StartupGameFlag1 == 0) {
                if (((u32)p->lsw_packet->field_0x00 & 0x01000000u) != 0)
                    AddStatusStage(p, 5, 0);
                else {
                    AddStatusStage(p, 6, 0);
                    AddStatusStage(p, 7, 0);
                }
            } else {
                AddStatusStage(p, 35, 0);
                AddStatusStage(p, 36, 0);
            }
            RememberPlayerIDs(1, (i16)p->player0_model, (i16)p->player1_model);
        }
        if (area != -1 && (p->area->flags & 0x136) == 0x10) {
            if (HubBothTrueJediGoldBricksPc != 0) {
                u8 *complete = &Game.area_save[area].true_hero_complete[(p->flags_word >> 6) & 1];
                if (*complete == 0) {
                    if ((p->field_0xb0 & 4) != 0) {
                        *complete = 1;
                        AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                        AddStatusStage(p, 1, AddGoldBrickMessage(p, TextIdPc880818));
                    } else if (p->collected_score > 0.0f)
                        AddStatusStage(p, 2, 0);
                }
            } else if (Game.area_save[area].true_hero_complete[0] == 0 &&
                       Game.area_save[area].true_hero_complete[1] == 0) {
                if ((p->field_0xb0 & 4) != 0) {
                    Game.area_save[area].true_hero_complete[1] = 1;
                    Game.area_save[area].true_hero_complete[0] = 1;
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddStatusStage(p, 1, AddGoldBrickMessage(p, TextIdPc880818));
                } else if (p->collected_score > 0.0f && (p->flags_word & 0x800) == 0)
                    AddStatusStage(p, 2, 0);
            }
        }
        if (p->collected_score > 0.0f)
            AddStatusStage(p, 3, 0);
        if (p->minikit_max > 0 && (p->area->flags & 0x36) == 0x10 && Game.area_save[area].minikit_complete == 0 &&
            AreaGlobals_PC.new_minikit_count > 0) {
            i32 gold = 0;
            p->new_minikits = AreaGlobals_PC.new_minikit_count;
            if (Game.area_save[area].minikit_count >= p->minikit_max) {
                Game.area_save[area].minikit_complete = 1;
                p->flags_word |= 0x10;
                AddToCompletionPoints(POINTS_PER_MINIKIT);
                gold = AddGoldBrickMessage(p, TextIdPc8808d0);
            }
            if (PauseSaveAndExitPc == 0 || p->new_minikits != 0)
                AddStatusStage(p, 4, gold);
            if ((p->field_0xb0 & 0x10) != 0 && AllMiniKitsDone((AREASAVE_s *)Game.area_save) != 0) {
                AddToCollection(VehicleBoltId7f195cPc);
                AddStatusStage(p, 17, 0);
            }
        }
        if (Game.gold_bricks > p->previous_gold_bricks)
            AddStatusStage(p, 19, 0);
        if ((p->flags_word & 0x800) == 0)
            AddStatusStage(p, 9, 0);
        STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
        if (netclient == 0)
            AddStatusStage(p, 11, 0);
        STATUS_APPEND_STAGE_INLINE_PC(p, 12, 0);
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        StatusCollectedSuitBitsPc = Game.initial_store_pack_flags;
        Game.initial_store_pack_flags = areaSuitBits;
    }

    void __cdecl Status_DrawPlayerIconsPc(STATUSPACKET_s *packet, f32 y, f32 alpha) {
        i32 player;
        for (player = 0; player < 2; ++player) {
            f32 x, weight, icon_alpha;
            i32 frame;
            if (player == 0) {
                frame = 0xa5;
                x = -ICONX;
            } else {
                x = ICONX;
                frame = 0xa6;
            }
            weight = packet->player_active[player] != 0 ? 1.0f : 0.25f;
            icon_alpha = weight * alpha;
            DrawCharIcon((i16)((StatusPlayerModelPairPc *)&packet->player0_model)->model[player], x, y, 0.0f,
                         MissionPlayerIconSizePc, frame, icon_alpha, icon_alpha, 1, NULL);
        }
    }

    /* Retail places StatusIconsOnOff (0x4850e0) with the shared status-screen code next to
       Status_DrawPlayerIconsPc, not with the LSW stage callbacks, so the stage callbacks call
       it instead of inlining it. */
    f32 __cdecl StatusIconsOnOff(f32 blend) {
        i32 angle = (i32)((f64)blend * 16384.0);
        f32 position = (STATSPOSY - STATSPOS2YPc) * NuTrigTable[(angle >> 1) & 0x7fff] + STATSPOS2YPc;
        return position;
    }
    f32 StatusMiniKitSlideTimePc = 0.25f;
    f32 StatusMiniKitPopTimerPc = 0.0f;
}

i32 __cdecl UpdateNewMiniKits(STATUSPACKET_s *packet, STATUS_STAGE_s *stage) {
    (void)packet;
    if (stage->field_0x18 > stage->field_0x1c) {
        stage->field_0x18 = 0.0f;
        StatusCurrentMinikitPc = (i32)((u32)StatusCurrentMinikitPc + 1u);
        StatusMiniKitPopTimerPc = 0.5f;
        StatusMiniKitProgressPc = 1.0f;
        return 1;
    }
    if (stage->field_0x18 > stage->field_0x1c - StatusMiniKitJibberLengthPc - StatusMiniKitSlideTimePc &&
        stage->field_0x18 <= stage->field_0x1c - StatusMiniKitJibberLengthPc) {
        StatusMiniKitProgressPc =
            (stage->field_0x1c - stage->field_0x18 - StatusMiniKitJibberLengthPc) / StatusMiniKitSlideTimePc;
    }
    return 0;
}
extern "C" {
    f32 StatusMiniKitDirectionOffsetPc = 0.035f;
    STATUS_MINIKIT_DRAW_CACHE_PC StatusMiniKitDrawCachePc[10] = {0};
}

void __cdecl DrawStatusMiniKit(f32 x, f32 y, f32 z, f32 scale, f32 collected_scale, i32 max_pieces,
                               STATUSPACKET_s *packet, f32 phase) {
    STATUS_WORLD_MINIKIT_READER_PC *view = (STATUS_WORLD_MINIKIT_READER_PC *)WORLD;
    if (view->minikit.scene == NULL || view->minikit.pieces == NULL)
        return;
    NUVEC offset = v000;
    NUVEC done;
    done.x = done.y = done.z = scale;
    NUVEC growing;
    growing.x = growing.y = growing.z = collected_scale;
    switch ((i8)view->minikit.pieces[StatusCurrentMinikitPc].direction) {
        case 0:
            offset.x += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
        case 1:
            offset.x -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
        case 2:
            offset.y += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
        case 3:
            offset.y -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
        case 4:
            offset.z -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
        case 5:
            offset.z += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffsetPc;
            break;
    }
    for (i32 index = 0; index < packet->minikit_max; index++) {
        view = (STATUS_WORLD_MINIKIT_READER_PC *)WORLD;
        if (index >= view->minikit.piece_count || index >= max_pieces)
            break;
        NUMTX matrix = view->minikit.pieces[index].matrix;
        if (index >= StatusCurrentMinikitPc) {
            NuMtxScale(&matrix, &growing);
            f32 reveal = index == StatusCurrentMinikitPc ? StatusMiniKitProgressPc : 1.0f;
            reveal = NuTrigTable[((i32)(reveal * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            MINIKIT_PIECE_PC *piece = &((STATUS_WORLD_MINIKIT_READER_PC *)WORLD)->minikit.pieces[index];
            switch ((i8)piece->direction) {
                case 0:
                    matrix.m30 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case 1:
                    matrix.m30 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case 2:
                    matrix.m31 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case 3:
                    matrix.m31 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case 4:
                    matrix.m32 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case 5:
                    matrix.m32 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
            }
        } else {
            NuMtxScale(&matrix, &done);
        }
        if (index <= StatusCurrentMinikitPc) {
            matrix.m30 += offset.x;
            matrix.m31 += offset.y;
            matrix.m32 += offset.z;
        }
        StatusMiniKitRotateYPc(&matrix, (u16)(NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5 * 65536.0));
        StatusMiniKitRotateZPc(
            &matrix, (u16)(NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 2.24f) / 2.24f * 65536.0) >> 1] * 2730.0));
        StatusMiniKitRotateXPc(
            &matrix,
            (u16)(NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 1.87f) / 1.87f * 65536.0) >> 1] * 2730.0 - 5461.0));
        WORLDINFO *world = WORLD;
        matrix.m30 += PANEL3DMULX * x;
        matrix.m31 += PANEL3DMULY * y;
        matrix.m32 += z;
        if (index < 10) {
            StatusMiniKitDrawCachePc[index].matrix = matrix;
            StatusMiniKitDrawCachePc[index].piece = &((STATUS_WORLD_MINIKIT_READER_PC *)world)->minikit.pieces[index];
            StatusMiniKitDrawCachePc[index].active = 1;
        }
        if (world->level_objects[206].active != 0)
            DrawPanel3DObjectMtxNoAlpha(
                (nuhspecial_s *)&((STATUS_WORLD_MINIKIT_READER_PC *)world)->minikit.pieces[index].special, &matrix);
    }
}
extern "C" {
    PART_LIGHTING_DATA_PC StatusMiniKitPartLightingPc = {0};
}

void __cdecl AddStatusMiniKitParts(void) {
    NUVEC target_position = {0.0f, STATSPOSY, 1.0f};
    for (i32 i = 0; i < 3; ++i) {
        StatusMiniKitPartLightingPc.values[i] = PanelLightingColoursPc[i];
        StatusMiniKitPartLightingPc.values[i + 3] = PanelLightingDirectionsPc[i];
    }
    StatusMiniKitPartLightingPc.values[6] = PanelLightingAmbientPc;
    f32 delay = 0.0f;
    for (i32 i = 0; i < 10; ++i) {
        if (StatusMiniKitDrawCachePc[i].active == 0)
            continue;
        NUVEC velocity = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * 0.25 + 0.25), 0.0f};
        NuVecRotateZ(&velocity, &velocity, qrand());
        ADDPART_PC part = Default_ADDPART;
        part.matrix = &StatusMiniKitDrawCachePc[i].matrix;
        part.velocity = &velocity;
        part.flags = 0x480;
        part.gravity = 0.0f;
        part.special = &StatusMiniKitDrawCachePc[i].piece->special;
        part.time_step = FRAMETIME;
        part.lighting_data = &StatusMiniKitPartLightingPc;
        AddPart(&part);
        for (i32 coin = 0; coin < 5; ++coin) {
            NUVEC position = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.1f), 1.0f};
            NuVecRotateZ(&position, &position, qrand());
            ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
            message.position = &position;
            message.target_scale = 0.75f;
            message.player_index = (i8)(qrand() / 0x8000);
            target_position.x = cointotal_x[(i32)message.player_index];
            message.target_position = &target_position;
            message.target_scale = COINTOTAL_COINSIZE;
            message.icon = GizmoPickupType[2].first_model_id;
            message.special = &WORLD->level_objects[message.icon].special;
            message.flags = 0x112d;
            message.duration = COINMSGTIME;
            message.score = GizmoPickupType[2].score;
            message.end_fn = EndScoreMessage;
            message.delay = delay;
            message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
            message.target_is_coin_total = 1;
            AddGameMsg(&message);
            if (delay == 0.0f) {
                i32 priority = 1;
                if (StatusPacket.player_active[0] != 0) {
                    if (StatusPacket.player0_buzz_amount < 0.0f)
                        StatusPacket.player0_buzz_amount = 0.0f;
                    if (priority > 0) {
                        i32 previous_priority = StatusPacket.player0_rumble_priority;
                        ++priority;
                        if (priority > previous_priority)
                            StatusPacket.player0_rumble_priority = (u8)priority;
                    }
                }
                if (StatusPacket.player_active[1] != 0) {
                    if (StatusPacket.player1_buzz_amount < 0.0f)
                        StatusPacket.player1_buzz_amount = 0.0f;
                    if (priority > 0) {
                        i32 previous_priority = StatusPacket.player1_rumble_priority;
                        ++priority;
                        if (priority > previous_priority)
                            StatusPacket.player1_rumble_priority = (u8)priority;
                    }
                }
                AddGameDebris(WORLD->game_debris_sys, 0x38, &position);
            }
            delay = (f32)((f64)delay + (f64)0.1f);
        }
    }
    SetPanelLights(1.0f);
}
extern "C" {
    f32 StatusPromptSingleYPc = 0.125f;
    f32 StatusPromptHintYPc = 0.39f;
    f32 StatusPromptStepYPc = -0.2f;

    void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha) {
        AREA_DATA_PC *areas = ADataList;
        i32 labels[2];
        i32 locked = 0;
        i32 count;
        i32 row;
        f32 title;
        f32 step;
        f32 y;
        labels[1] = -1;
        if ((packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 || packet->mission_state != 0) {
            labels[0] = TextIdPc88083c;
            labels[1] = TextIdPc8806f8;
        } else if ((packet->flags_word & 0x40u) != 0 ||
                   (packet->area_id != -1 && (u8)areas[packet->area_id].episode_index == 0xff)) {
            labels[0] = TextIdPc8806f8;
        } else if ((packet->flags_word & 8u) != 0 && packet->next_area != -1 &&
                   (areas[packet->next_area].flags & 2u) == 0 && Game.area_save[packet->next_area].complete == 0) {
            labels[0] = TextIdPc8806f8;
            locked = 1;
        } else {
            if (packet->next_area != -1 && (areas[packet->next_area].flags & 2u) != 0)
                labels[0] = TextIdPc880744;
            else
                labels[0] = TextIdPc880704;
            if ((packet->flags_word & 8u) != 0)
                labels[1] = TextIdPc8806f8;
        }
        title = StatusCoinsTitleYPc;
        if (StartupGameFlag1 != 0)
            step = StatusPromptStepYPc;
        else
            step = -((f64)title + (f64)title);
        if (labels[1] == -1) {
            i16 label;
            count = 1;
            y = StartupGameFlag1 != 0 ? StatusPromptSingleYPc : title;
            if (locked != 0) {
                label = areas[packet->next_area].area_index == 5 ? TextIdPc88078c : TextIdPc880790;
                SmartTextEx(TTab[label], 0.0f, 0.325f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 63, 0, 1.7f, 1, NULL, 0,
                            (i32)((f64)alpha * 128.0));
            }
        } else {
            count = 2;
            y = 0.0f;
            if (StartupGameFlag1 != 0 && packet->newly_completed != 0 && packet->area != NULL &&
                packet->area == NEGOTIATIONS_ADATA) {
                Hint_SetHintFromId(0x26c, 0, 1);
                Hint_SetComplete(0x26c);
                y = StatusPromptHintYPc;
                Hint_Draw(-1);
            }
            y = (f64)y - (f64)step * 0.5;
        }
        if ((packet->flags_word & 0x100u) != 0)
            y = (f64)y + (f64)0.075f;
        for (row = 0; row < count; ++row) {
            i32 red;
            i32 green;
            i32 blue;
            if (selected != 0 && status_prompt == row) {
                if (menu_flash != 0) {
                    red = MENUFLASH0R;
                    green = MENUFLASH0G;
                    blue = MENUFLASH0B;
                } else {
                    red = MENUFLASH1R;
                    green = MENUFLASH1G;
                    blue = MENUFLASH1B;
                }
                if (menu_pulsate > 0.0f) {
                    f64 inverse = 1.0 - (f64)menu_pulsate;
                    red = (i32)((f64)(u32)MENUFLASH0R * (f64)menu_pulsate + (f64)(u32)MENUFLASH1R * inverse);
                    green = (i32)((f64)(u32)MENUFLASH0G * (f64)menu_pulsate + (f64)(u32)MENUFLASH1G * inverse);
                    blue = (i32)((f64)(u32)MENUFLASH0B * (f64)menu_pulsate + (f64)(u32)MENUFLASH1B * inverse);
                }
            } else {
                red = MENUENTRYR;
                green = MENUENTRYG;
                blue = MENUENTRYB;
            }
            SmartTextEx(TTab[labels[row]], 0.0f, y, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, blue, 1.7f, 1, NULL, 0,
                        (u8)(i32)((f64)alpha * 128.0));
            y = (f64)step + (f64)y;
        }
    }
    f32 StatusBonusTimeYPc = -0.5f;

    void __cdecl DrawBonusTime(STATUSPACKET_s *packet, f32 position, i32 alpha) {
        f32 y = StatusBonusTimeYPc * position;
        char time[256];
        char previous[256];
        char *title;
        i32 red, green;
        if (packet->new_best_time != 0.0f) {
            title = TTab[TextIdPc88071c];
            red = 0;
            green = 255;
        } else {
            title = TTab[TextIdPc880734];
            red = 255;
            green = 0;
        }
        {
            f32 title_y = (f64)y + (f64)0.15f;
            SmartTextEx(title, 0.0f, title_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, 0, 1.7f, 1, NULL, 0, (u8)alpha);
        }
        {
            f32 value = packet->new_best_time != 0.0f ? packet->new_best_time : packet->elapsed_time;
            Text_MakeTime(value, 1, 1, 1, time);
        }
        {
            f32 time_y = (f64)y + 0.0;
            Text3DEx(time, 0.0f, time_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, alpha);
        }
        Text_MakeTime(packet->previous_best_time, 1, 1, 1, time);
        NuStrCpy(previous, "(");
        NuStrCat(previous, time);
        NuStrCat(previous, ")");
        {
            f32 previous_y = (f64)y - (f64)0.15f;
            Text3DEx(previous, 0.0f, previous_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, (u8)alpha >> 1);
        }
    }
    i32 StatusDemoOptionPc = 0;
#pragma data_seg(".data$StatusCountdownLimitPc")
    extern "C" {
        f32 StatusCountdownLimitPc = 6.0f;
    }
#pragma data_seg()
#pragma data_seg(".data$StatusGoldBrickXPc")
    extern "C" {
        f32 StatusGoldBrickXPc = 0.715f;
    }
#pragma data_seg()
#pragma data_seg(".data$StatusGoldBrickYPc")
    extern "C" {
        f32 StatusGoldBrickYPc = -0.666f;
    }
#pragma data_seg()
#pragma data_seg(".data$StatusGoldBrickScalePc")
    extern "C" {
        f32 StatusGoldBrickScalePc = 0.6f;
    }
#pragma data_seg()
}

extern "C" void __cdecl DrawStatusScreen(WORLDINFO *) {
    iconalphaoverride = -1.0f;
    memset(StatusMiniKitDrawCachePc, 0, sizeof(StatusMiniKitDrawCachePc));
    if (GAMEDEMO != 0) {
        if (GAMEDEMO != 1 || FadeSys.fade != 0.0f)
            return;
        char *labels[2];
        labels[0] = TTab[TextIdPc8808a8];
        labels[1] = TTab[TextIdPc880bc0];
        f32 y = -0.75 - MENUDY * 0.5;
        for (i32 option = 0; option < 2; ++option) {
            i32 red;
            i32 green;
            i32 blue;
            if (option == StatusDemoOptionPc) {
                if (menu_flash != 0) {
                    red = MENUFLASH0R;
                    green = MENUFLASH0G;
                    blue = MENUFLASH0B;
                } else {
                    red = MENUFLASH1R;
                    green = MENUFLASH1G;
                    blue = MENUFLASH1B;
                }
                if (menu_pulsate > 0.0f) {
                    red = (i32)(MENUFLASH0R * menu_pulsate + MENUFLASH1R * (1.0f - menu_pulsate));
                    green = (i32)(MENUFLASH0G * menu_pulsate + MENUFLASH1G * (1.0f - menu_pulsate));
                    blue = (i32)(MENUFLASH0B * menu_pulsate + MENUFLASH1B * (1.0f - menu_pulsate));
                }
            } else {
                red = MENUENTRYR;
                green = MENUENTRYG;
                blue = MENUENTRYB;
            }
            Text3D(labels[option], 0.0f, y, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0, red, green,
                   blue);
            y += MENUDY;
        }
        return;
    }
    if (FadeSys.fade > 0.0f)
        return;
    if (StatusPacket.status_flags != 0) {
        if (StatusPacket.draw_background_callback != NULL)
            StatusPacket.draw_background_callback(&StatusPacket);
        if (StatusGoldBrickPhasePc != -1 && StatusGoldBrickStagePc != -1 &&
            StatusPacket.stage->field_0x14 == StatusGoldBrickPhasePc &&
            StatusPacket.current_gold_brick == StatusGoldBrickStagePc) {
            f32 duration;
            if (StatusGoldBrickPhasePc == 4) {
                duration = 5.5f;
                if (StatusPacket.stage->field_0x18 >= duration)
                    goto draw_stages;
            } else {
                duration = StatusPacket.stage->field_0x1c;
            }
            u16 rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
            u16 tilt = (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f);
            f32 quarter = duration * 0.25f;
            f32 three_quarters = duration * 0.75f;
            f32 alpha;
            if (StatusPacket.stage->field_0x18 < quarter)
                alpha = NuTrigTable[((i32)(StatusPacket.stage->field_0x18 / quarter * 16384.0f) >> 1) & 0x7fff];
            else if (StatusPacket.stage->field_0x18 < three_quarters)
                alpha = 1.0f;
            else
                alpha = NuTrigTable[((i32)((1.0f - (StatusPacket.stage->field_0x18 - three_quarters) /
                                                       (duration - three_quarters)) *
                                           16384.0f) >>
                                     1) &
                                    0x7fff];
            f32 scale = StatusGoldBrickScalePc * alpha;
            DrawPanel3DObjectNoAlpha(StatusGoldBrickXPc, StatusGoldBrickYPc, 1.0f, scale, scale, scale, tilt, rotation,
                                     0, &WORLD->level_objects[211].special, 2);
            if (Text_Language != 0)
                Text3DEx((char *)"+1", StatusGoldBrickXPc, StatusGoldBrickYPc - 0.15f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255,
                         0, 255, (u8)(alpha * 128.0f));
        }
    draw_stages:
        for (i32 i = 0; StatusStages[i].type != -1; i++) {
            STATUS_STAGE_s *stage = &StatusStages[i];
            if (stage->draw_callback != NULL) {
                if (stage == StatusPacket.stage)
                    StatusPacket.stage->draw_callback(StatusPacket.stage, &StatusPacket, 1);
                else
                    stage->draw_callback(stage, &StatusPacket, 0);
            }
        }
        f32 alpha;
        if (StatusPacket.stage->type == 11)
            return;
        if (StatusPacket.stage->type == 12) {
            alpha = 0.0f;
        } else if (StatusPacket.stage->type == 10) {
            alpha = StatusPacket.stage->field_0x18 < 1.0f ? 1.0f - StatusPacket.stage->field_0x18 : 0.0f;
        } else if (StatusPacket.stage->type == 19 && StatusPacket.stage->field_0x14 != 0) {
            if (StatusPacket.stage->field_0x18 < 1.0f) {
                alpha = 1.0f - StatusPacket.stage->field_0x18;
            } else {
                f32 fade_start = StatusPacket.stage->field_0x1c - 1.0;
                if (StatusPacket.stage->field_0x18 >= fade_start)
                    alpha =
                        (StatusPacket.stage->field_0x18 - fade_start) / (StatusPacket.stage->field_0x1c - fade_start);
                else
                    alpha = 0.0f;
            }
        } else {
            alpha = 1.0f;
        }
        if (StatusDrawPlayerIconsPc != 0)
            Status_DrawPlayerIconsPc(&StatusPacket, StatusIconYPc,
                                     iconalphaoverride >= 0.0f ? iconalphaoverride : alpha);
    } else {
        char time[256];
        f32 remaining = StatusCountdownLimitPc - GameTimer.time_elapsed;
        if (remaining < 0.0f)
            remaining = 0.0f;
        Text_MakeTime(remaining, 0, 0, 0, time);
        Text3D(time, NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 0.432f) / 0.432f * 65536.0f) >> 1] * 0.01f,
               NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 0.5f) * 2.0f * 65536.0f) >> 1] * 0.01f, 1.0f, 1.0f,
               1.0f, 1.0f, 0, 255, 191, 0);
    }
}

extern "C" void __cdecl FinishStatusPacket(i32 choice) {
    if (LOSTTEMPLE_ADATA != NULL && StatusPacket.area_id == (i16)LOSTTEMPLE_ADATA->index) {
        if (StatusPacket.player0_model == (u16)VehicleBoltId7f17d0Pc)
            StatusPacket.player0_model = (u16)VehicleBoltId7f17d4Pc;
        else if (StatusPacket.player1_model == (u16)VehicleBoltId7f17d0Pc)
            StatusPacket.player1_model = (u16)VehicleBoltId7f17d4Pc;
    }
    RememberPlayerIDs(1, (i16)StatusPacket.player0_model, (i16)StatusPacket.player1_model);
    if (StatusPacket.finish_callback(WORLD, &StatusPacket, choice) != 0)
        return;
    NewLData = HUB_LDATA;
    if (choice == 0 &&
        ((StatusPacket.flags_word & 0x100) != 0 ||
         (StartupGameFlag1 != 0 && (StatusPacket.challenge_state != 0 || StatusPacket.mission_state != 0)))) {
        NewLData = Area_FindNextPlayLevel(ADataList[StatusPacket.area->index].levels[0]);
        ResetBits = 0xffffffbfu;
        reset_area = 1;
        FADETYPE fade = {FADE_TYPE_STILL_WIPE};
        FadeSys.SetFade(fade, 0);
        NeedScreenGrab(1);
        GrabStillScreen();
        if (StatusPacket.challenge_state != 0)
            InitChallenge(StatusPacket.area->index);
        if (StatusPacket.mission_state != 0) {
            InitMission(MissionSys, (i8)MissionSys->active_mission->index);
            NewLData = &LDataList[MissionSys->active_mission->level];
        }
    } else if (choice == 0 && StatusPacket.next_area != -1) {
        NewLData = &LDataList[ADataList[StatusPacket.next_area].levels[0]];
        if ((ADataList[StatusPacket.next_area].flags & 2) != 0) {
            LevelCompletionFallbackFlagPC = 1;
        } else {
            loadareacharacters_no_backdrop_reset = 1;
            if ((StatusPacket.flags_word & 0x400) != 0)
                finishloop_backdroponly = 1;
        }
    } else if (StartupGameFlag1 != 0 && ((StatusPacket.flags_word & 0x400) != 0 ||
                                         ((StatusPacket.area->flags & 4) != 0 && StatusPacket.free_play == 0))) {
        NewLData = CREDITS_LDATA;
    }
    if ((StatusPacket.flags_word & 0x800) != 0)
        NewLData = HUB_LDATA;
    if (NewLData->area_index != -1 && (ADataList[NewLData->area_index].flags & 0x40) != 0 &&
        StatusPacket.mission_state != 0 && MissionSys->active_mission != NULL)
        hub_from_mission = (i8)MissionSys->active_mission->index;
    if (NewLData == HUB_LDATA) {
        OldBonusScore[0] = 0;
        OldBonusScore[1] = 0;
    } else {
        OldBonusScore[0] = BonusScore[0];
        OldBonusScore[1] = BonusScore[1];
    }
    NuSound3StopRumble();
}

/* Retail 486c00 consists solely of RET. */
extern "C" void __cdecl NuMainStatusHelperPc(STATUSPACKET_s *) {
}
extern "C" {

    void __cdecl RegisterStatusScreen(STATUS_STAGE_s *stages, i32 *unused,
                                      const REGISTER_STATUS_PACKET_PC *registration) {
        (void)unused;
        StatusStages = stages;
        StatusPacket.lsw_packet = registration->lsw_packet;
        StatusPacket.init_callback = registration->init_callback;
        StatusPacket.finish_callback = registration->finish_callback;
        StatusPacket.reset_callback = registration->reset_callback;
        StatusPacket.draw_background_callback = registration->draw_background_callback;
        StatusPacket.field_0x68 = registration->stage_delay;
    }

    void __cdecl UpdateStatusScreen(WORLDINFO *world) {
        const f32 elapsed = FRAMETIME;
        f32 seek_step = (f64)elapsed * 3.0;
        i32 player;
        i32 select = 0;
        i32 up = 0;
        i32 down = 0;
        (void)world;
        CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, seek_step);
        StatusGoldBrickPhasePc = -1;
        StatusGoldBrickStagePc = -1;
        switch (GAMEDEMO) {
            case 0:
                break;
            case 1:
                if (FadeSys.fade != 0.0f || NewLData != NULL)
                    return;
                {
                    for (player = 0; player < 2; ++player) {
                        u32 pressed, directions;
                        if (select != 0 || up != 0 || down != 0)
                            break;
                        if (StatusPacket.player_active[player] == 0)
                            continue;
                        pressed = GamePadStorage[player].menu_buttons_pressed;
                        if ((pressed & GAMEPAD_MENUSELECT) != 0)
                            select = 1;
                        else {
                            directions = GamePadStorage[player].menu_directions;
                            if ((directions & GAMEPAD_DUP) != 0 || (pressed & GAMEPAD_DUP) != 0)
                                up = 1;
                            else if ((directions & GAMEPAD_DDOWN) != 0 || (pressed & GAMEPAD_DDOWN) != 0)
                                down = 1;
                        }
                    }
                    if (select != 0) {
                        GameAudio_PlaySfx(0x30, NULL, 0, 0);
                        switch (StatusDemoOptionPc) {
                            case 0:
                                GAMEDEMO = 2;
                                NewLData = HUB_LDATA;
                                new_level_from_menu = 1;
                                break;
                            case 1:
                                SendMessageA(PCInputInstance_candidate, WM_CLOSE, 0, 0);
                                break;
                        }
                    } else if (up != 0) {
                        if (StatusDemoOptionPc > 0) {
                            --StatusDemoOptionPc;
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        }
                    } else if (down != 0) {
                        if (StatusDemoOptionPc < 1) {
                            ++StatusDemoOptionPc;
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        }
                    }
                }
                return;
            case 2:
                if (GameTimer.time_elapsed >= 8.0f)
                    SendMessageA(PCInputInstance_candidate, WM_CLOSE, 0, 0);
                return;
            default:
                return;
        }
        if (StatusPacket.status_flags == 0)
            return;
        if (!(StatusPacket.newly_completed != 0 && StatusPacket.area != NULL &&
              StatusPacket.area == NEGOTIATIONS_ADATA)) {
            u32 button = Text_Language != 0 ? GAMEPAD_JUMP : GAMEPAD_MENUSELECT;
            if ((StatusPacket.player0_active != 0 && (GamePadStorage[0].menu_buttons_pressed & button) != 0) ||
                (StatusPacket.player1_active != 0 && (GamePadStorage[1].menu_buttons_pressed & button) != 0) ||
                (StatusPacket.field_0xb0 & 2) != 0) {
                STATUS_STAGE_s *stage = StatusPacket.stage;
                if (stage == NULL)
                    goto status_done;
                if (stage->skip_callback != NULL)
                    stage->skip_callback(stage, &StatusPacket);
            }
        }
        if (StatusPacket.stage != NULL && StatusPacket.stage->update_callback != NULL)
            StatusPacket.stage->update_callback(StatusPacket.stage, &StatusPacket, elapsed);
    status_done:
        StatusRumbleUpdatePc();
        if (StatusPacket.status_flags == 0 && GameTimer.time_elapsed >= 5.0f)
            FinishStatusPacket(0);
    }
}
