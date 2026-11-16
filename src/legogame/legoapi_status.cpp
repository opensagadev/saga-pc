#include <stdio.h>
#include "batman.h"
#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/save_values.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/missions.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dCore.h"
static i32 goldbrickmsgcount = 0;

/* Walks the two adjacent 0x14-byte rumble records. */

typedef struct status_world_minikit_reader_s {
    u8 unknown_0000[0x2968];
    MINIKIT minikit;
} STATUS_WORLD_MINIKIT_READER;

extern "C" i16 dat_00880bc0;

extern "C" {
    i32 AllMiniKitsDone(AREASAVE_s *save);
}
extern "C" {

    f32 StatusScoreRatio(f32 numerator, f32 denominator) {
        if (numerator != 0.0f && denominator != 0.0f) {
            f32 result = (f64)numerator / (f64)denominator;
            return result;
        }
        return 0.0f;
    }
}

static void StatusMiniKitRotateX(NUMTX *m, i32 a) {
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

static void StatusMiniKitRotateY(NUMTX *m, i32 a) {
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

static void StatusMiniKitRotateZ(NUMTX *m, i32 a) {
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
    i32 DrawGoldBrick_Stage = 0;
    i32 DrawGoldBrick_Phase = 0;

    void SetDrawGoldBrick(STATUSPACKET_s *packet, i32 unused) {
        i32 index = packet->current_gold_brick;
        if (packet->gold_brick_enabled[index] != 0) {
            STATUS_STAGE_s *stage = packet->stage;
            DrawGoldBrick_Stage = index;
            DrawGoldBrick_Phase = stage->step;
        }
    }

    static NUMTX RenderStatusMatrixPrivate = {0};
    AREA_DATA *GOTHAMSTREETS_ADATA = 0;
    AREA_DATA *NEGOTIATIONS_ADATA = 0;

    void DrawStatusText(char *text, u16 angle, f32 x, f32 y, f32 scale, u32 colour, i32 alignment) {
        NUVEC origin = {0.0f, 0.0f, 800.0f};
        NUVEC position = {x * DrawStatusScaleX, y * DrawStatusScaleY, 0.0f};
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
        NuQFntPushPrintMode(NUQFNT_MODE_3D);
        NuQFntSetCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, scale, scale);
        Text3DStringEncode(text, encoded);
        switch (alignment) {
            case TEXT_ALIGN_LEFT:
                NuQFntMove(QFont3DZ, 0.0f, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
            case TEXT_ALIGN_RIGHT:
                NuQFntMove(QFont3DZ, -NuQFntPrintLenW(QFont3DZ, encoded), (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
            default:
                NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5), (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
                break;
        }
        NuQFntPrintW(QFont3DZ, encoded);
        NuQFntPopPrintMode();
        NuQFntSetCoordinateSystem(NUQFNT_CS_640X224);
    }

    void DrawStatusTextFraction(i32 current, i32 total, f32 x, f32 y, u16 angle, f32 scale, u32 colour, f32 animation,
                                f32 animation_duration) {
        char text[16];
        u16 encoded[64];
        NUMTX matrix;
        NUVEC origin = {0.0f, 0.0f, 800.0f};
        NUVEC position = {(f32)((f64)x * 400.0), (f32)((f64)y * 250.0), 0.0f};
        f32 full_width;
        f32 height;
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
        NuQFntPushPrintMode(NUQFNT_MODE_3D);
        NuQFntSetCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
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
            f64 half_full_width;
            f64 current_center;
            f32 suffix_x;
            f32 current_width;
            f32 large_x;
            f32 small_x;
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
        NuQFntSetCoordinateSystem(NUQFNT_CS_640X224);
    }

    STATUS_STAGE_s *FindStatusStage(i32 type) {
        i32 index;
        for (index = 0; StatusStages[index].type != -1; ++index) {
            if (StatusStages[index].type == type) {
                return &StatusStages[index];
            }
        }
        return NULL;
    }

    void StatusStage_Reset(STATUS_STAGE_s *stage) {
        if (stage != NULL) {
            stage->step = STATUS_STEP_WAITING;
            stage->step_time = 0.0f;
            stage->shown = 0;
            stage->step_duration = 1.0f;
        }
    }

    /* The status-stage handlers call this shared routine. */
    void NewStatusRumbleBuzz(i32 player, float amount, float buzz, i32 priority) {
        for (i32 i = 0; i < 2; ++i) {
            if (StatusPacket.player_active[i] != 0 && (player == -1 || player == i)) {
                RUMBLE_PACKET_RESOURCE *rumble = &StatusPacket.rumble[i];
                if (amount > 0.0f && (rumble->amount <= 0.0f || amount > rumble->amount / rumble->decay_duration * rumble->intensity)) {
                    rumble->intensity = amount;
                    rumble->decay_duration = amount;
                    rumble->amount = amount;
                }
                if (buzz > rumble->duration) {
                    rumble->duration = buzz;
                }
                if (priority > 0 && ++priority > rumble->active) {
                    rumble->active = (u8)priority;
                }
            }
        }
    }

    void StatusRumbleUpdate(void) {
        RUMBLE_PACKET_RESOURCE *packet = StatusPacket.rumble;
        i32 player;
        for (player = 0; player < 2; ++player, ++packet) {
            i32 motor1;
            UpdateRumble(packet);
            if (Game.options_save.unknown_0000[player] != 0 && StatusPacket.player_active[player] != 0 &&
                ((u8)GamePadStorage[player].allocated & 0x20) != 0) {
                if (packet->amount > 0.0f) {
                    motor1 = (i32)((packet->intensity * packet->amount) * 255.0f / packet->decay_duration);
                } else {
                    motor1 = 0;
                }
                NuPadSetDeviceMotors(GamePadStorage[player].pad, packet->duration > 0.0f || packet->active > 0, motor1);
            } else {
                NuPadSetDeviceMotors(GamePadStorage[player].pad, 0, 0);
            }
        }
    }

    void NextStatusStage(STATUSPACKET_s *packet) {
        if (packet->stage != NULL && packet->stage->step == STATUS_STEP_WAITING && packet->stage->step_time < packet->stage_delay) {
            packet->flags_word |= 2;
            return;
        }
        if (packet->stage != NULL) {
            packet->stage->shown = 1;
        }
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
            if (packet->current_gold_brick + 1 >= packet->stage_count) {
                packet->next_stage = FindStatusStage(STATUS_STAGE_EXIT);
            } else {
                packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
            }
        }
        StatusStage_Reset(packet->stage);
        packet->flags_word &= ~2u;
        status_prompt = 0;
        if ((packet->stage != NULL && packet->stage->type == STATUS_STAGE_SAVE) || packet->stage->type == STATUS_STAGE_EXIT) {
            ResetRumble(&packet->rumble[0]);
            ResetRumble(&packet->rumble[1]);
        }
        if (packet->stage != NULL && packet->stage->type == STATUS_STAGE_SAVE) {
            ReCalculateCompletionPoints();
        }
    }

    i32 AddGoldBrickMessage(STATUSPACKET_s *packet, i16 brick) {
        if (goldbrickmsgcount < 16) {
            ((i16 *)(packet->unknown_12c))[goldbrickmsgcount] = brick;
            ++goldbrickmsgcount;
        }
        AddToGoldBricks();
        return 1;
    }

    // noinline: the original calls AddStatusStage out of line from InitStatusScreen; VC8 would inline it there.
    __declspec(noinline) void AddStatusStage(STATUSPACKET_s *packet, i32 type, i32 gold_brick_enabled) {
        packet->stage_types[packet->stage_count] = (i8)(type);
        packet->gold_brick_enabled[packet->stage_count] = (u8)(gold_brick_enabled);
        ++packet->stage_count;
    }

    i16 StatusNewCharacterIds[8] = {0};

    i32 newCharactersCollected(STATUSPACKET_s *packet) {
        i32 count = 0;
        if (StartupGameFlag1 == 0) {
            if ((((u8 *)&packet->lsw_packet->unknown_00)[3] & 1) != 0 && Game.initial_store_pack_flags != areaSuitBits) {
                return 1;
            }
        } else {
            i16 temple_ids[2];
            i16 *ids;
            temple_ids[0] = dat_007f17d4;
            temple_ids[1] = -1;
            ids = packet->area != NULL && packet->area == LOSTTEMPLE_ADATA ? temple_ids : Area_PlayerIDList;
            while (*ids != -1 && count < 7) {
                i32 id = *ids;
                i32 previous = 0;
                while (previous < count && StatusNewCharacterIds[previous] != id) {
                    ++previous;
                }
                if (previous == count && (Game_CharacterSave == NULL || (Game_CharacterSave[id] & 1) == 0)) {
                    i32 found = InCollectList_Index(id, MasterCollection.list, MasterCollection.count);
                    if (found != -1) {
                        COLLECTID *entry = &MasterCollection.list[found];
                        if (entry->can_buy == 0) {
                            StatusNewCharacterIds[count++] = (i16)id;
                            if (entry->field5_0x9 != 0) {
                                AddToCompletionPoints(POINTS_PER_CHARACTER);
                            }
                        }
                    }
                }
                ++ids;
            }
            StatusNewCharacterIds[count] = -1;
        }
        return count;
    }

    void StatusPacketReset(STATUSPACKET_s *packet) {
        struct STATUSPACKET_LSW_s *lsw = packet->lsw_packet;
        void (*reset)(STATUSPACKET_s *) = packet->reset_callback;
        i32 (*init)(WORLDINFO *, STATUSPACKET_s *) = packet->init_callback;
        i32 (*finish)(WORLDINFO *, STATUSPACKET_s *, i32) = packet->finish_callback;
        void (*draw)(STATUSPACKET_s *) = packet->draw_background_callback;
        f32 delay = packet->stage_delay;
        reset(packet);
        memset(packet, 0, sizeof(*packet));
        packet->lsw_packet = lsw;
        packet->finish_callback = finish;
        packet->reset_callback = reset;
        packet->draw_background_callback = draw;
        packet->stage_delay = delay;
        packet->init_callback = init;
    }

    i32 StatusNewMinikitCount = 0;
    i32 currentminikit = 0;
    u32 StatusCollectedSuitBits = 0;

    void InitStatusScreen(WORLDINFO *world) {
        STATUSPACKET_s *p = &StatusPacket;
        i32 i;
        i32 area;
        i32 episode;
        if (p->lsw_packet == NULL) {
            return;
        }
        episode = -1;
        for (i = 0; StatusStages[i].type != -1; ++i) {
            StatusStage_Reset(&StatusStages[i]);
        }
        StatusPacketReset(p);
        StatusIconY = STATSPOSY;
        p->flags_word &= ~1u;
        StatusNewMinikitCount = 0;
        currentminikit = 0;
        ResetRumble(&p->rumble[0]);
        ResetRumble(&p->rumble[1]);
        CoinTotalScale = 1.0f;
        goldbrickmsgcount = 0;
        if (netclient == 0) {
            StatusPlayerModelPair *models = (StatusPlayerModelPair *)&p->player0_model;
            for (i = 0; i < 2; ++i) {
                GAMEOBJECT *object = Player[i];
                if (object != NULL) {
                    models->model[i] = object->id;
                    p->player_active[i] = (u8)((object->flags_1fc_word >> 7) & 1);
                    if (object->player.coin_packet != NULL) {
                        p->coins_remaining[i] = object->player.coin_packet->coins;
                        ++p->status_flags;
                    } else {
                        p->coins_remaining[i] = 0;
                        ++p->status_flags;
                    }
                } else {
                    models->model[i] = 0xffff;
                }
            }
        }
        if (p->status_flags != 2) {
            p->status_flags = 0;
            return;
        }
        p->flags_word = (p->flags_word & ~STATUSPACKET_SUPER_STORY_MODE) | (((u32)SuperStory << 10) & 0x400u);
        p->flags_word = (p->flags_word & ~STATUSPACKET_FREE_PLAY) | (((u32)FreePlay << 6) & 0x40u);
        p->flags_word = (p->flags_word & ~STATUSPACKET_SAVED_AND_EXITED) | (((u32)PauseSaveAndExit << 11) & 0x800u);
        p->flags_word &= ~STATUSPACKET_CONTINUE_STORY;
        p->stage_count = 0;
        p->mission = Mission_Active(NULL);
        if (p->mission != NULL) {
            p->mission_state = (u8)Mission_CurrentState(NULL);
        } else {
            p->mission_state = MISSION_STATE_NONE;
        }
        p->challenge_state = (u8)ChallengeMode;
        PauseSaveAndExit = 0;
        area = (i8)world->area_index;
        p->flags_word &= ~STATUSPACKET_NEXT_AREA_UNLOCKED;
        p->previous_completion = (u8)Game.save_hash;
        p->flags_word &= ~(STATUSPACKET_VEHICLE_AREA | STATUSPACKET_BONUS_MODE | STATUSPACKET_SUPER_BONUS_MODE);
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
            if (episode != -1) {
                p->episode = &EDataList[p->area->episode_index];
            }
            p->chapter = p->area->area_index;
            if ((p->area->flags & AREAFLAG_VEHICLE_AREA) != 0) {
                p->flags_word |= STATUSPACKET_VEHICLE_AREA;
            }
            if ((p->area->flags & AREAFLAG_BONUS_AREA) != 0) {
                p->flags_word |= STATUSPACKET_BONUS_MODE;
            }
            if ((p->area->flags & AREAFLAG_SUPER_BONUS) != 0) {
                p->flags_word |= STATUSPACKET_SUPER_BONUS_MODE;
            }
        } else {
            p->area = NULL;
            p->episode = NULL;
            p->chapter = -1;
        }
        if (p->init_callback(world, p) != 0) {
            return;
        }
        if ((p->flags_word & STATUSPACKET_SUPER_STORY_MODE) != 0) {
            /* Tests the current level separately for loaded and unloaded worlds. */
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
                    } else {
                        p->new_best_time = 0.0f;
                    }
                    if (p->superstory_score > p->previous_best_score) {
                        p->new_best_score = p->superstory_score;
                        save->superstory_score_target = p->superstory_score;
                    } else {
                        p->new_best_score = 0;
                    }
                    if (save->superstory_complete == 0) {
                        save->superstory_complete = 1;
                        AddToCompletionPoints(POINTS_PER_SUPERSTORY);
                        if (EpisodeGoldBrickForSuperStory != 0) {
                            gold = AddGoldBrickMessage(p, dat_0088073c);
                        }
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
                    if (reward < 100) {
                        reward = 100;
                    }
                    IncreaseScore(&p->time_reward_score, reward, 0);
                }
                p->final_reward_score = p->time_reward_score;
                IncreaseScore(&p->final_reward_score, p->superstory_score, 0);
                p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_SUPER_STORY_COMPLETE);
                p->gold_brick_enabled[p->stage_count] = (u8)(gold);
                ++p->stage_count;
                p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_SUPER_STORY_TIME);
                p->gold_brick_enabled[p->stage_count] = (u8)(0);
                ++p->stage_count;
                p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_SUPER_STORY_SCORE);
                p->gold_brick_enabled[p->stage_count] = (u8)(0);
                ++p->stage_count;
                if (Game.gold_bricks > p->previous_gold_bricks) {
                    AddStatusStage(p, STATUS_STAGE_GOLD_BRICK, 0);
                }
                if (netclient == 0 && (p->new_best_time != 0.0f || p->new_best_score != 0 || p->superstory_score != 0)) {
                    AddStatusStage(p, STATUS_STAGE_SAVE, 0);
                }
                hub_from_superstory = p->area->episode_index;
            }
            p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_EXIT);
            p->gold_brick_enabled[p->stage_count] = (u8)(0);
            ++p->stage_count;
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            if (episode != -1 && p->chapter < EDataList[episode].regular_areas) {
                p->next_area = EDataList[episode].area_ids[p->chapter + 1];
                if (p->next_area != -1 && (ADataList[p->next_area].flags & AREAFLAG_ENDING_AREA) != 0) {
                    hub_from_superstory = episode;
                }
            }
            return;
        }
        if ((p->flags_word & STATUSPACKET_BONUS_MODE) != 0 && area != -1) {
            i32 gold = 0;
            p->elapsed_time = BonusTimer.time_elapsed;
            p->flags_word = (p->flags_word & ~STATUSPACKET_BONUS_WINNER) | (((u32)BonusWinner << 5) & 0x20u);
            p->previous_best_time = Game.area_save[area].challenge_trial_time;
            if (p->previous_best_time <= 0.0f || p->previous_best_time > p->elapsed_time) {
                if (WORLD->area->challenge_trial_time != 0 && p->previous_best_time >= WORLD->area->challenge_trial_time) {
                    AddToCompletionPoints(POINTS_PER_TIMETRIAL);
                    gold = AddGoldBrickMessage(p, dat_0088071c);
                }
                p->new_best_time = p->elapsed_time;
                Game.area_save[area].challenge_trial_time = p->elapsed_time;
            }
            if ((p->flags_word & STATUSPACKET_SUPER_BONUS_MODE) != 0) {
                if (Game.area_save[p->area_id].area_complete == 0) {
                    Game.area_save[p->area_id].area_complete = 1;
                    AddToCompletionPoints(POINTS_PER_SUPERBONUSCOMPLETE);
                    p->newly_completed = 1;
                    if (EpisodeGoldBrickForSuperBonus != 0) {
                        gold = AddGoldBrickMessage(p, dat_00880728);
                    }
                }
                AddStatusStage(p, STATUS_STAGE_BONUS_COMPLETE, gold);
                gold = 0;
            } else {
                AddStatusStage(p, STATUS_STAGE_BONUS_WIN, 0);
            }
            AddStatusStage(p, STATUS_STAGE_BONUS_TIME, gold);
            if (Game.gold_bricks > p->previous_gold_bricks) {
                AddStatusStage(p, STATUS_STAGE_GOLD_BRICK, 0);
            }
            AddStatusStage(p, STATUS_STAGE_PROMPT, 0);
            AddStatusStage(p, STATUS_STAGE_EXIT, 0);
            if (netclient == 0) {
                AddStatusStage(p, STATUS_STAGE_SAVE, 0);
            }
            AddStatusStage(p, STATUS_STAGE_FADE, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIcons = 1;
            return;
        }
        if (p->mission_state != MISSION_STATE_NONE) {
            i32 gold = 0;
            p->newly_completed = 0;
            p->original_score = *p->score;
            p->reward_score = p->original_score;
            p->elapsed_time = MissionSys->timer.time_elapsed;
            if (p->mission_state == MISSION_STATE_COMPLETE) {
                if (Game_MissionSave == NULL || Game_MissionSave->completed[MissionSys->active_mission->index] == 0) {
                    if (Game_MissionSave != NULL) {
                        Game_MissionSave->completed[MissionSys->active_mission->index] = 1;
                    }
                    AddToCompletionPoints(POINTS_PER_MISSION);
                    gold = AddGoldBrickMessage(p, dat_008806e4);
                }
                {
                    i32 whole = (i32)((f64)p->mission->time - (f64)p->elapsed_time);
                    i32 scaled = (i32)((u32)whole * 150u);
                    u64 reward = (u64)(i64)scaled;
                    if (reward < 150) {
                        reward = 150;
                    }
                    IncreaseScore(&p->reward_score, reward, 0);
                }
                if (Game_MissionSave != NULL) {
                    f32 *best = &Game_MissionSave->best_times[MissionSys->active_mission->index];
                    if (*best == 0.0f || *best > p->elapsed_time) {
                        *best = p->elapsed_time;
                    }
                }
            }
            AddStatusStage(p, STATUS_STAGE_MISSION, gold);
            if (Game.gold_bricks > p->previous_gold_bricks) {
                AddStatusStage(p, STATUS_STAGE_GOLD_BRICK, 0);
            }
            p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_PROMPT);
            p->gold_brick_enabled[p->stage_count] = (u8)(0);
            ++p->stage_count;
            p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_EXIT);
            p->gold_brick_enabled[p->stage_count] = (u8)(0);
            ++p->stage_count;
            if (netclient == 0) {
                AddStatusStage(p, STATUS_STAGE_SAVE, 0);
            }
            p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_FADE);
            p->gold_brick_enabled[p->stage_count] = (u8)(0);
            ++p->stage_count;
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIcons = 1;
            cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
            return;
        }
        if (p->challenge_state != CHALLENGE_OFF) {
            i32 first = 0;
            p->elapsed_time = ChallengeTimer.time_elapsed;
            p->original_score = *p->score;
            p->reward_score = p->original_score;
            if (p->challenge_state == CHALLENGE_WON) {
                if (Game.area_save[p->area_id].challenge_unlocked == 0) {
                    AddToCompletionPoints(POINTS_PER_CHALLENGE);
                    Game.area_save[p->area_id].challenge_unlocked = 1;
                    first = 1;
                }
                {
                    i32 whole = (i32)((f64)ADataList[p->area_id].challenge_trial_time - (f64)p->elapsed_time);
                    i32 scaled = (i32)((u32)whole * 500u);
                    u64 reward = (u64)(i64)scaled;
                    if (reward < 500) {
                        reward = 500;
                    }
                    IncreaseScore(&p->reward_score, reward, 0);
                }
            }
            AddStatusStage(p, STATUS_STAGE_CHALLENGE, 0);
            if (p->challenge_state == CHALLENGE_WON && first != 0) {
                AddStatusStage(p, STATUS_STAGE_CHALLENGE_CASH, 0);
            }
            AddStatusStage(p, STATUS_STAGE_PROMPT, 0);
            AddStatusStage(p, STATUS_STAGE_EXIT, 0);
            if (netclient == 0) {
                AddStatusStage(p, STATUS_STAGE_SAVE, 0);
            }
            AddStatusStage(p, STATUS_STAGE_FADE, 0);
            p->stage_types[p->stage_count] = -1;
            p->current_gold_brick = -1;
            p->next_stage = FindStatusStage(p->stage_types[0]);
            NextStatusStage(p);
            StatusDrawPlayerIcons = 1;
            cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
            return;
        }
        if (area != -1) {
            if (BuildUpDone != 0) {
                p->flags_word |= STATUSPACKET_TRUE_HERO_COMPLETE;
            }
            p->minikit_count = (u8)(Game.area_save[area].minikit_count + (u8)AreaGlobals.new_minikit_count);
            p->minikit_max = (p->area->flags & AREAFLAG_MINIKIT) != 0 ? 10 : 0;
            if (p->minikit_count > p->minikit_max) {
                p->minikit_count = p->minikit_max;
            }
            p->true_hero_target = (f32)p->area->true_hero_targets[(p->flags_word >> 6) & 1];
        }
        p->area_time = AreaTimer.time_elapsed;
        if ((u64)p->coins_remaining[0] + (u64)p->coins_remaining[1] >= 4000000000ULL) {
            p->collected_score = 4000000000.0f;
        } else {
            p->collected_score = (f32)(p->coins_remaining[0] + p->coins_remaining[1]);
        }
        p->newly_completed = 0;
        if ((p->unknown_b0 & STATUSPACKET_TRUE_HERO_COMPLETE) != 0) {
            p->true_hero_percent = 100.0f;
        } else {
            f32 target = p->true_hero_target;
            f32 numerator = (f64)p->collected_score * 100.0;
            p->true_hero_percent = StatusScoreRatio(numerator, target);
            if (p->true_hero_percent > 99.0f) {
                p->true_hero_percent = 99.0f;
            }
        }
        p->coins_collected[0] = p->coins_remaining[0];
        p->coins_collected[1] = p->coins_remaining[1];
        if (area != -1 && (p->unknown_b0 & STATUSPACKET_FREE_PLAY) == 0 && p->episode != NULL && p->chapter < p->episode->regular_areas) {
            p->next_area = p->episode->area_ids[p->chapter + 1];
            if (p->chapter < p->episode->regular_areas - 1) {
                p->flags_word |= STATUSPACKET_NEXT_AREA_UNLOCKED;
            }
        }
        StatusDrawPlayerIcons = 0;
        if (area != -1) {
            for (i = 0; i < AreaGlobals.new_minikit_count; ++i) {
                LEVELSAVE_s *save = &((LEVELSAVE_s *)Game.level_save)[NewMiniPiece[i].level];
                if (Game.area_save[area].minikit_count < 10) {
                    ++Game.area_save[area].minikit_count;
                }
                if (save->minikit_count < 10) {
                    NuStrCpy(save->minikit_names[save->minikit_count], NewMiniPiece[i].name);
                    ++save->minikit_count;
                }
            }
        }
        {
            i32 gold = 0;
            i32 completed_episode = 0;
            if (area != -1 && (p->unknown_b0 & STATUSPACKET_FREE_PLAY) == 0 && Game.area_save[area].area_complete == 0 &&
                (p->area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_BONUS_AREA | AREAFLAG_TEST_AREA)) == 0) {
                p->newly_completed = 1;
                Game.area_save[area].area_complete = 1;
                AddToCompletionPoints(POINTS_PER_STORY);
                if ((p->flags_word & STATUSPACKET_BONUS_MODE) == 0 && (p->area->flags & AREAFLAG_NO_GOLD_BRICK) == 0) {
                    gold = AddGoldBrickMessage(p, dat_00880728);
                }
                if (p->episode != NULL && Episode_IsComplete(p->episode, NULL) != 0) {
                    completed_episode = p->area->episode_index;
                }
                if (StartupGameFlag1 != 0 && p->area == NEGOTIATIONS_ADATA) {
                    for (i = 1; i < 6; ++i) {
                        Game.area_save[EDataList[i].area_ids[0]].complete = 1;
                    }
                }
                if ((p->unknown_b0 & STATUSPACKET_NEXT_AREA_UNLOCKED) != 0) {
                    Game.area_save[p->next_area].complete = 1;
                } else if (p->episode != NULL) {
                    i32 bonus = Episode_FindAreaFromFlags(p->episode, AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 4);
                    i32 bonus2 = Episode_FindAreaFromFlags(p->episode, AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
                    if (bonus != -1) {
                        Game.area_save[bonus].complete = 1;
                    }
                    if (bonus2 != -1) {
                        Game.area_save[bonus2].complete = 1;
                    }
                    hub_startoutsidebonusdoor_area = bonus2;
                }
            }
            if (p->newly_completed != 0) {
                AddStatusStage(p, STATUS_STAGE_LEVEL_COMPLETE, gold);
            }
            if (completed_episode != 0) {
                AddStatusStage(p, completed_episode + 29, 0);
            }
        }
        if (GOTHAMSTREETS_ADATA != NULL && p->newly_completed != 0 && p->area == GOTHAMSTREETS_ADATA) {
            AddStatusStage(p, STATUS_STAGE_GOTHAM_COMPLETE, 0);
        }
        if (p->area != NULL && (u8)p->area->cheat != 0xff && Game.area_save[area].red_brick_collected == 0 && AreaGlobals.unknown_08 != 0) {
            AddToCompletionPoints(POINTS_PER_REDBRICK);
            Game.area_save[area].red_brick_collected = 1;
            AddStatusStage(p, STATUS_STAGE_RED_BRICK, 0);
        }
        if (p->newly_completed != 0 && newCharactersCollected(p) > 0) {
            if (StartupGameFlag1 == 0) {
                if (((u32)p->lsw_packet->unknown_00 & 0x01000000u) != 0) {
                    AddStatusStage(p, 5, 0);
                } else {
                    AddStatusStage(p, 6, 0);
                    AddStatusStage(p, 7, 0);
                }
            } else {
                AddStatusStage(p, STATUS_STAGE_COLLECT_CHARACTERS, 0);
                AddStatusStage(p, STATUS_STAGE_COLLECT_CHARACTERS_OFF, 0);
            }
            RememberPlayerIDs(1, (i16)p->player0_model, (i16)p->player1_model);
        }
        if (area != -1 && (p->area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_BONUS_AREA | AREAFLAG_MINIKIT | AREAFLAG_TEST_AREA |
                                             AREAFLAG_SUPER_BONUS)) == 0x10) {
            if (HubBothTrueJediGoldBricks != 0) {
                u8 *complete = &Game.area_save[area].true_hero_complete[(p->flags_word >> 6) & 1];
                if (*complete == 0) {
                    if ((p->unknown_b0 & STATUSPACKET_TRUE_HERO_COMPLETE) != 0) {
                        *complete = 1;
                        AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                        AddStatusStage(p, STATUS_STAGE_TRUE_JEDI, AddGoldBrickMessage(p, dat_00880818));
                    } else if (p->collected_score > 0.0f) {
                        AddStatusStage(p, STATUS_STAGE_TRUE_JEDI_SCORE, 0);
                    }
                }
            } else if (Game.area_save[area].true_hero_complete[0] == 0 && Game.area_save[area].true_hero_complete[1] == 0) {
                if ((p->unknown_b0 & STATUSPACKET_TRUE_HERO_COMPLETE) != 0) {
                    Game.area_save[area].true_hero_complete[1] = 1;
                    Game.area_save[area].true_hero_complete[0] = 1;
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddStatusStage(p, STATUS_STAGE_TRUE_JEDI, AddGoldBrickMessage(p, dat_00880818));
                } else if (p->collected_score > 0.0f && (p->flags_word & STATUSPACKET_SAVED_AND_EXITED) == 0) {
                    AddStatusStage(p, STATUS_STAGE_TRUE_JEDI_SCORE, 0);
                }
            }
        }
        if (p->collected_score > 0.0f) {
            AddStatusStage(p, STATUS_STAGE_COINS, 0);
        }
        if (p->minikit_max > 0 &&
            (p->area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_BONUS_AREA | AREAFLAG_MINIKIT | AREAFLAG_TEST_AREA)) == 0x10 &&
            Game.area_save[area].minikit_complete == 0 && AreaGlobals.new_minikit_count > 0) {
            i32 gold = 0;
            p->new_minikits = AreaGlobals.new_minikit_count;
            if (Game.area_save[area].minikit_count >= p->minikit_max) {
                Game.area_save[area].minikit_complete = 1;
                p->flags_word |= STATUSPACKET_NEW_MINIKIT_COMPLETE;
                AddToCompletionPoints(POINTS_PER_MINIKIT);
                gold = AddGoldBrickMessage(p, dat_008808d0);
            }
            if (PauseSaveAndExit == 0 || p->new_minikits != 0) {
                AddStatusStage(p, STATUS_STAGE_MINIKIT, gold);
            }
            if ((p->unknown_b0 & STATUSPACKET_NEW_MINIKIT_COMPLETE) != 0 && AllMiniKitsDone((AREASAVE_s *)Game.area_save) != 0) {
                AddToCollection(dat_007f195c);
                AddStatusStage(p, STATUS_STAGE_ALL_MINIKITS, 0);
            }
        }
        if (Game.gold_bricks > p->previous_gold_bricks) {
            AddStatusStage(p, STATUS_STAGE_GOLD_BRICK, 0);
        }
        if ((p->flags_word & STATUSPACKET_SAVED_AND_EXITED) == 0) {
            AddStatusStage(p, STATUS_STAGE_PROMPT, 0);
        }
        p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_EXIT);
        p->gold_brick_enabled[p->stage_count] = (u8)(0);
        ++p->stage_count;
        if (netclient == 0) {
            AddStatusStage(p, STATUS_STAGE_SAVE, 0);
        }
        p->stage_types[p->stage_count] = (i8)(STATUS_STAGE_FADE);
        p->gold_brick_enabled[p->stage_count] = (u8)(0);
        ++p->stage_count;
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        StatusCollectedSuitBits = Game.initial_store_pack_flags;
        Game.initial_store_pack_flags = areaSuitBits;
    }

    void fun_00485010() {
        SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
    }

    void Status_DrawPlayerIcons(STATUSPACKET_s *packet, f32 y, f32 alpha) {
        i32 player;
        for (player = 0; player < 2; ++player) {
            f32 x;
            f32 weight;
            f32 icon_alpha;
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
            DrawCharIcon((i16)((StatusPlayerModelPair *)&packet->player0_model)->model[player], x, y, 0.0f, MissionPlayerIconSize, frame,
                         icon_alpha, icon_alpha, 1, NULL);
        }
    }

    /* StatusIconsOnOff lives with the shared status-screen code next to
       Status_DrawPlayerIcons, not with the LSW stage callbacks, so the stage callbacks call
       it instead of inlining it. */
    f32 StatusIconsOnOff(f32 blend) {
        i32 angle = (i32)((f64)blend * 16384.0);
        f32 position = (STATSPOSY - STATSPOS2Y) * NuTrigTable[(angle >> 1) & 0x7fff] + STATSPOS2Y;
        return position;
    }

    f32 slidetime = 0.25f;
    f32 pop_timer = 0.0f;
}

i32 UpdateNewMiniKits(STATUSPACKET_s *packet, STATUS_STAGE_s *stage) {
    if (stage->step_time > stage->step_duration) {
        stage->step_time = 0.0f;
        currentminikit = (i32)((u32)currentminikit + 1u);
        pop_timer = 0.5f;
        slideseek = 1.0f;
        return 1;
    }
    if (stage->step_time > stage->step_duration - jibberlen - slidetime && stage->step_time <= stage->step_duration - jibberlen) {
        slideseek = (stage->step_duration - stage->step_time - jibberlen) / slidetime;
    }
    return 0;
}

extern "C" {
    f32 StatusMiniKitDirectionOffset = 0.035f;
    STATUS_MINIKIT_DRAW_CACHE StatusMiniKitDrawCache[10] = {0};
    struct STATUS_STAGE_s *StatusStages = 0;
    STATUSPACKET_s StatusPacket = {0};
}

void DrawStatusMiniKit(f32 x, f32 y, f32 z, f32 scale, f32 collected_scale, i32 max_pieces, STATUSPACKET_s *packet, f32 phase) {
    STATUS_WORLD_MINIKIT_READER *view = (STATUS_WORLD_MINIKIT_READER *)WORLD;
    if (view->minikit.scene == NULL || view->minikit.pieces == NULL) {
        return;
    }
    NUVEC offset = v000;
    NUVEC done;
    done.x = done.y = done.z = scale;
    NUVEC growing;
    growing.x = growing.y = growing.z = collected_scale;
    switch ((i8)view->minikit.pieces[currentminikit].direction) {
        case MINIKIT_PIECE_NEG_X:
            offset.x += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
        case MINIKIT_PIECE_POS_X:
            offset.x -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
        case MINIKIT_PIECE_NEG_Y:
            offset.y += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
        case MINIKIT_PIECE_POS_Y:
            offset.y -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
        case MINIKIT_PIECE_NEG_Z:
            offset.z -= NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
        case MINIKIT_PIECE_POS_Z:
            offset.z += NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff] * StatusMiniKitDirectionOffset;
            break;
    }
    for (i32 index = 0; index < packet->minikit_max; index++) {
        view = (STATUS_WORLD_MINIKIT_READER *)WORLD;
        if (index >= view->minikit.piece_count || index >= max_pieces) {
            break;
        }
        NUMTX matrix = view->minikit.pieces[index].matrix;
        if (index >= currentminikit) {
            NuMtxScale(&matrix, &growing);
            f32 reveal = index == currentminikit ? slideseek : 1.0f;
            reveal = NuTrigTable[((i32)(reveal * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            MINIKIT_PIECE *piece = &((STATUS_WORLD_MINIKIT_READER *)WORLD)->minikit.pieces[index];
            switch ((i8)piece->direction) {
                case MINIKIT_PIECE_NEG_X:
                    matrix.m30 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case MINIKIT_PIECE_POS_X:
                    matrix.m30 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case MINIKIT_PIECE_NEG_Y:
                    matrix.m31 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case MINIKIT_PIECE_POS_Y:
                    matrix.m31 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case MINIKIT_PIECE_NEG_Z:
                    matrix.m32 += (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
                case MINIKIT_PIECE_POS_Z:
                    matrix.m32 -= (piece->direction_index * 0.025f + 0.25) * reveal;
                    break;
            }
        } else {
            NuMtxScale(&matrix, &done);
        }
        if (index <= currentminikit) {
            matrix.m30 += offset.x;
            matrix.m31 += offset.y;
            matrix.m32 += offset.z;
        }
        StatusMiniKitRotateY(&matrix, (u16)(NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5 * 65536.0));
        StatusMiniKitRotateZ(&matrix, (u16)(NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 2.24f) / 2.24f * 65536.0) >> 1] * 2730.0));
        StatusMiniKitRotateX(&matrix,
                             (u16)(NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 1.87f) / 1.87f * 65536.0) >> 1] * 2730.0 - 5461.0));
        WORLDINFO *world = WORLD;
        matrix.m30 += PANEL3DMULX * x;
        matrix.m31 += PANEL3DMULY * y;
        matrix.m32 += z;
        if (index < 10) {
            StatusMiniKitDrawCache[index].matrix = matrix;
            StatusMiniKitDrawCache[index].piece = &((STATUS_WORLD_MINIKIT_READER *)world)->minikit.pieces[index];
            StatusMiniKitDrawCache[index].active = 1;
        }
        if (world->level_objects[206].active != 0) {
            DrawPanel3DObjectMtxNoAlpha(&((STATUS_WORLD_MINIKIT_READER *)world)->minikit.pieces[index].special, &matrix);
        }
    }
}

extern "C" {
    PART_LIGHTING_DATA StatusMiniKitPartLighting = {0};
}

void AddStatusMiniKitParts(void) {
    f32 delay = 0.0f;
    NUVEC target_position = {0.0f, STATSPOSY, 1.0f};
    StatusMiniKitPartLighting.values[0] = PanelLightingColours[0];
    StatusMiniKitPartLighting.values[3] = PanelLightingDirections[0];
    StatusMiniKitPartLighting.values[1] = PanelLightingColours[1];
    StatusMiniKitPartLighting.values[4] = PanelLightingDirections[1];
    StatusMiniKitPartLighting.values[2] = PanelLightingColours[2];
    StatusMiniKitPartLighting.values[5] = PanelLightingDirections[2];
    StatusMiniKitPartLighting.values[6] = PanelLightingAmbient;
    for (i32 i = 0; i < 10; ++i) {
        if (StatusMiniKitDrawCache[i].active == 0) {
            continue;
        }
        NUVEC velocity;
        velocity.x = velocity.z = 0.0f;
        velocity.y = (f32)qrand() * (1.0f / 65535.0f) * 0.25f + 0.25f;
        NuVecRotateZ(&velocity, &velocity, qrand());
        ADDPART part = Default_ADDPART;
        part.matrix = &StatusMiniKitDrawCache[i].matrix;
        part.velocity = &velocity;
        part.lighting_data = &StatusMiniKitPartLighting;
        part.special = &StatusMiniKitDrawCache[i].piece->special;
        part.flags = PART_FLAG_ROTATE_RANDOMLY | PART_FLAG_IGNORE_TERRAIN;
        part.gravity = 0.0f;
        part.time_step = FRAMETIME;
        AddPart(&part);
        for (i32 coin = 0; coin < 5; ++coin) {
            NUVEC position = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.1f), 1.0f};
            NuVecRotateZ(&position, &position, qrand());
            ADD_GAME_MESSAGE message = AddGameMsg_Default;
            message.position = &position;
            message.target_scale = 0.75f;
            message.player_index = (i8)(qrand() / 0x8000);
            target_position.x = cointotal_x[message.player_index];
            message.target_position = &target_position;
            message.target_scale = COINTOTAL_COINSIZE;
            message.icon = GizmoPickupType[2].first_model_id;
            message.special = &WORLD->level_objects[message.icon].special;
            message.flags =
                GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN | GAMEMSG_KEEP;
            message.duration = COINMSGTIME;
            message.score = GizmoPickupType[2].score;
            message.end_fn = EndScoreMessage;
            message.delay = delay;
            message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
            message.target_is_coin_total = 1;
            AddGameMsg(&message);
            if (delay == 0.0f) {
                NewStatusRumbleBuzz(-1, 0.0f, 0.0f, 1);
                AddGameDebris(WORLD->game_debris_sys, 0x38, &position);
            }
            delay = (f32)((f64)delay + (f64)0.1f);
        }
    }
    SetPanelLights(1.0f);
}

extern "C" {
    f32 StatusPromptSingleY = 0.125f;
    f32 StatusPromptHintY = 0.39f;
    f32 StatusPromptStepY = -0.2f;

    void Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha) {
        AREA_DATA *areas = ADataList;
        i32 labels[2];
        i32 locked = 0;
        i32 count;
        i32 row;
        f32 title;
        f32 step;
        f32 y;
        labels[1] = -1;
        if ((packet->flags_word & STATUSPACKET_BONUS_MODE) != 0) {
            labels[0] = dat_0088083c;
            labels[1] = dat_008806f8;
        } else if (packet->challenge_state != CHALLENGE_OFF) {
            labels[0] = dat_0088083c;
            labels[1] = dat_008806f8;
        } else if (packet->mission_state != MISSION_STATE_NONE) {
            labels[0] = dat_0088083c;
            labels[1] = dat_008806f8;
        } else if ((packet->flags_word & STATUSPACKET_FREE_PLAY) != 0) {
            labels[0] = dat_008806f8;
        } else if (packet->area_id != -1 && (u8)areas[packet->area_id].episode_index == 0xff) {
            labels[0] = dat_008806f8;
        } else {
            u32 chapter = (packet->flags_word >> 3) & 1;
            if (chapter != 0 && packet->next_area != -1 && (areas[packet->next_area].flags & AREAFLAG_ENDING_AREA) == 0 &&
                Game.area_save[packet->next_area].complete == 0) {
                labels[0] = dat_008806f8;
                locked = 1;
            } else {
                if (packet->next_area != -1 && (areas[packet->next_area].flags & AREAFLAG_ENDING_AREA) != 0) {
                    labels[0] = dat_00880744;
                } else {
                    labels[0] = dat_00880704;
                }
                if (chapter != 0) {
                    labels[1] = dat_008806f8;
                }
            }
        }
        title = StatusCoinsTitleY;
        if (StartupGameFlag1 != 0) {
            step = StatusPromptStepY;
        } else {
            step = -(title + title);
        }
        if (labels[1] == -1) {
            i16 label;
            count = 1;
            y = StartupGameFlag1 != 0 ? StatusPromptSingleY : title;
            if (locked != 0) {
                label = areas[packet->next_area].area_index == 5 ? dat_0088078c : dat_00880790;
                SmartTextEx(TTab[label], 0.0f, 0.325f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 63, 0, 1.7f, 1, NULL, 0, (i32)((f64)alpha * 128.0));
            }
        } else {
            if (StartupGameFlag1 != 0 && packet->newly_completed != 0 && packet->area != NULL && packet->area == NEGOTIATIONS_ADATA) {
                Hint_SetHintFromId(0x26c, 0, 1);
                Hint_SetComplete(0x26c);
                y = StatusPromptHintY;
                Hint_Draw(-1);
            } else {
                y = 0.0f;
            }
            count = 2;
            y = (f64)y - (f64)step * 0.5;
        }
        if ((packet->flags_word & STATUSPACKET_BONUS_MODE) != 0) {
            y = (f64)y + (f64)0.075f;
        }
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

    f32 StatusBonusTimeY = -0.5f;
    NUVEC PanelLightingColours[3] = {{1.0f, 0.5f, 0.5f}, {0.5f, 1.0f, 0.5f}, {0.5f, 0.5f, 1.0f}};
    NUVEC PanelLightingDirections[3] = {{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}};
    NUVEC PanelLightingAmbient = {1.0f, 1.0f, 1.0f};

    void DrawBonusTime(STATUSPACKET_s *packet, f32 position, i32 alpha) {
        f32 y = StatusBonusTimeY * position;
        char time[256];
        char previous[256];
        char *title;
        i32 red;
        i32 green;
        if (packet->new_best_time != 0.0f) {
            title = TTab[dat_0088071c];
            red = 0;
            green = 255;
        } else {
            title = TTab[dat_00880734];
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

    i32 StatusDemoOption = 0;
    extern "C" {
        f32 StatusCountdownLimit = 6.0f;
    }
    extern "C" {
        f32 StatusGoldBrickX = 0.715f;
    }
    extern "C" {
        f32 StatusGoldBrickY = -0.666f;
    }
    extern "C" {
        f32 StatusGoldBrickScale = 0.6f;
        f32 DrawStatusScaleX = 350.0f;
        f32 DrawStatusScaleY = 300.0f;
    }
}

extern "C" void DrawStatusScreen(WORLDINFO *) {
    iconalphaoverride = -1.0f;
    memset(StatusMiniKitDrawCache, 0, sizeof(StatusMiniKitDrawCache));
    if (GAMEDEMO != 0) {
        if (GAMEDEMO != 1 || FadeSys.fade != 0.0f) {
            return;
        }
        char *labels[2];
        labels[0] = TTab[dat_008808a8];
        labels[1] = TTab[dat_00880bc0];
        f32 y = -0.75 - MENUDY * 0.5;
        for (i32 option = 0; option < 2; ++option) {
            i32 red;
            i32 green;
            i32 blue;
            if (option == StatusDemoOption) {
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
            Text3D(labels[option], 0.0f, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue);
            y += MENUDY;
        }
        return;
    }
    if (FadeSys.fade > 0.0f) {
        return;
    }
    if (StatusPacket.status_flags != 0) {
        if (StatusPacket.draw_background_callback != NULL) {
            StatusPacket.draw_background_callback(&StatusPacket);
        }
        if (DrawGoldBrick_Phase != -1 && DrawGoldBrick_Stage != -1 && StatusPacket.stage->step == DrawGoldBrick_Phase &&
            StatusPacket.current_gold_brick == DrawGoldBrick_Stage) {
            f32 duration;
            if (DrawGoldBrick_Phase == 4) {
                duration = 5.5f;
                if (StatusPacket.stage->step_time >= duration) {
                    goto draw_stages;
                }
            } else {
                duration = StatusPacket.stage->step_duration;
            }
            u16 rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
            u16 tilt = (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f);
            f32 quarter = duration * 0.25f;
            f32 three_quarters = duration * 0.75f;
            f32 alpha;
            if (StatusPacket.stage->step_time < quarter) {
                alpha = NuTrigTable[((i32)(StatusPacket.stage->step_time / quarter * 16384.0f) >> 1) & 0x7fff];
            } else if (StatusPacket.stage->step_time < three_quarters) {
                alpha = 1.0f;
            } else {
                alpha = NuTrigTable
                    [((i32)((1.0f - (StatusPacket.stage->step_time - three_quarters) / (duration - three_quarters)) * 16384.0f) >> 1) &
                     0x7fff];
            }
            f32 scale = StatusGoldBrickScale * alpha;
            DrawPanel3DObjectNoAlpha(StatusGoldBrickX, StatusGoldBrickY, 1.0f, scale, scale, scale, tilt, rotation, 0,
                                     &WORLD->level_objects[211].special, 2);
            if (Text_Language != NULANGUAGE_JAPANESE) {
                Text3DEx((char *)"+1", StatusGoldBrickX, StatusGoldBrickY - 0.15f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 0, 255,
                         (u8)(alpha * 128.0f));
            }
        }
    draw_stages:
        for (i32 i = 0; StatusStages[i].type != -1; i++) {
            STATUS_STAGE_s *stage = &StatusStages[i];
            if (stage->draw_callback != NULL) {
                if (stage == StatusPacket.stage) {
                    StatusPacket.stage->draw_callback(StatusPacket.stage, &StatusPacket, 1);
                } else {
                    stage->draw_callback(stage, &StatusPacket, 0);
                }
            }
        }
        f32 alpha;
        if (StatusPacket.stage->type == STATUS_STAGE_SAVE) {
            return;
        }
        if (StatusPacket.stage->type == STATUS_STAGE_FADE) {
            alpha = 0.0f;
        } else if (StatusPacket.stage->type == STATUS_STAGE_EXIT) {
            alpha = StatusPacket.stage->step_time < 1.0f ? 1.0f - StatusPacket.stage->step_time : 0.0f;
        } else if (StatusPacket.stage->type == STATUS_STAGE_GOLD_BRICK && StatusPacket.stage->step != 0) {
            if (StatusPacket.stage->step_time < 1.0f) {
                alpha = 1.0f - StatusPacket.stage->step_time;
            } else {
                f32 fade_start = StatusPacket.stage->step_duration - 1.0;
                if (StatusPacket.stage->step_time >= fade_start) {
                    alpha = (StatusPacket.stage->step_time - fade_start) / (StatusPacket.stage->step_duration - fade_start);
                } else {
                    alpha = 0.0f;
                }
            }
        } else {
            alpha = 1.0f;
        }
        if (StatusDrawPlayerIcons != 0) {
            Status_DrawPlayerIcons(&StatusPacket, StatusIconY, iconalphaoverride >= 0.0f ? iconalphaoverride : alpha);
        }
    } else {
        char time[256];
        f32 remaining = StatusCountdownLimit - GameTimer.time_elapsed;
        if (remaining < 0.0f) {
            remaining = 0.0f;
        }
        Text_MakeTime(remaining, 0, 0, 0, time);
        Text3D(time, NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 0.432f) / 0.432f * 65536.0f) >> 1] * 0.01f,
               NuTrigTable[(u16)(NuFmod(GameTimer.time_elapsed, 0.5f) * 2.0f * 65536.0f) >> 1] * 0.01f, 1.0f, 1.0f, 1.0f, 1.0f, 0, 255, 191,
               0);
    }
}

extern "C" void FinishStatusPacket(i32 choice) {
    if (LOSTTEMPLE_ADATA != NULL && StatusPacket.area_id == (i16)LOSTTEMPLE_ADATA->index) {
        if (StatusPacket.player0_model == (u16)dat_007f17d0) {
            StatusPacket.player0_model = (u16)dat_007f17d4;
        } else if (StatusPacket.player1_model == (u16)dat_007f17d0) {
            StatusPacket.player1_model = (u16)dat_007f17d4;
        }
    }
    RememberPlayerIDs(1, (i16)StatusPacket.player0_model, (i16)StatusPacket.player1_model);
    if (StatusPacket.finish_callback(WORLD, &StatusPacket, choice) != 0) {
        return;
    }
    NewLData = HUB_LDATA;
    if (choice == 0 &&
        ((StatusPacket.flags_word & STATUSPACKET_BONUS_MODE) != 0 ||
         (StartupGameFlag1 != 0 && (StatusPacket.challenge_state != CHALLENGE_OFF || StatusPacket.mission_state != MISSION_STATE_NONE)))) {
        NewLData = Area_FindNextPlayLevel(ADataList[StatusPacket.area->index].levels[0]);
        ResetBits = 0xffffffbfu;
        reset_area = 1;
        FADETYPE fade = {FADE_TYPE_STILL_WIPE};
        FadeSys.SetFade(fade, 0);
        NeedScreenGrab(1);
        GrabStillScreen();
        if (StatusPacket.challenge_state != CHALLENGE_OFF) {
            InitChallenge(StatusPacket.area->index);
        }
        if (StatusPacket.mission_state != MISSION_STATE_NONE) {
            InitMission(MissionSys, MissionSys->active_mission->index);
            NewLData = &LDataList[MissionSys->active_mission->level];
        }
    } else if (choice == 0 && StatusPacket.next_area != -1) {
        NewLData = &LDataList[ADataList[StatusPacket.next_area].levels[0]];
        if ((ADataList[StatusPacket.next_area].flags & AREAFLAG_ENDING_AREA) != 0) {
            LevelCompletionFallbackFlagPC = 1;
        } else {
            loadareacharacters_no_backdrop_reset = 1;
            if ((StatusPacket.flags_word & STATUSPACKET_SUPER_STORY_MODE) != 0) {
                finishloop_backdroponly = 1;
            }
        }
    } else if (StartupGameFlag1 != 0 && ((StatusPacket.flags_word & STATUSPACKET_SUPER_STORY_MODE) != 0 ||
                                         ((StatusPacket.area->flags & AREAFLAG_BONUS_AREA) != 0 && StatusPacket.free_play == 0))) {
        NewLData = CREDITS_LDATA;
    }
    if ((StatusPacket.flags_word & STATUSPACKET_SAVED_AND_EXITED) != 0) {
        NewLData = HUB_LDATA;
    }
    if (NewLData->area_index != -1 && (ADataList[NewLData->area_index].flags & AREAFLAG_HUB_AREA) != 0 &&
        StatusPacket.mission_state != MISSION_STATE_NONE && MissionSys->active_mission != NULL) {
        hub_from_mission = MissionSys->active_mission->index;
    }
    if (NewLData == HUB_LDATA) {
        OldBonusScore[0] = 0;
        OldBonusScore[1] = 0;
    } else {
        OldBonusScore[0] = BonusScore[0];
        OldBonusScore[1] = BonusScore[1];
    }
    NuSound3StopRumble();
}

/* Empty. */
extern "C" void NuMainStatusHelper(STATUSPACKET_s *) {
}

extern "C" {

    void RegisterStatusScreen(STATUS_STAGE_s *stages, i32 *unused, const REGISTER_STATUS_PACKET *registration) {
        StatusStages = stages;
        StatusPacket.lsw_packet = registration->lsw_packet;
        StatusPacket.init_callback = registration->init_callback;
        StatusPacket.finish_callback = registration->finish_callback;
        StatusPacket.reset_callback = registration->reset_callback;
        StatusPacket.draw_background_callback = registration->draw_background_callback;
        StatusPacket.stage_delay = registration->stage_delay;
    }

    void UpdateStatusScreen(WORLDINFO *world) {
        const f32 elapsed = FRAMETIME;
        f32 seek_step = (f64)elapsed * 3.0;
        i32 player;
        i32 select = 0;
        i32 up = 0;
        i32 down = 0;
        CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, seek_step);
        DrawGoldBrick_Phase = -1;
        DrawGoldBrick_Stage = -1;
        switch (GAMEDEMO) {
            case 0:
                break;
            case 1:
                if (FadeSys.fade != 0.0f || NewLData != NULL) {
                    return;
                }
                {
                    for (player = 0; player < 2; ++player) {
                        u32 pressed;
                        u32 directions;
                        if (select != 0 || up != 0 || down != 0) {
                            break;
                        }
                        if (StatusPacket.player_active[player] == 0) {
                            continue;
                        }
                        pressed = GamePadStorage[player].menu_buttons_pressed;
                        if ((pressed & GAMEPAD_MENUSELECT) != 0) {
                            select = 1;
                        } else {
                            directions = GamePadStorage[player].menu_directions;
                            if ((directions & GAMEPAD_DUP) != 0 || (pressed & GAMEPAD_DUP) != 0) {
                                up = 1;
                            } else if ((directions & GAMEPAD_DDOWN) != 0 || (pressed & GAMEPAD_DDOWN) != 0) {
                                down = 1;
                            }
                        }
                    }
                    if (select != 0) {
                        GameAudio_PlaySfx(0x30, NULL, 0, 0);
                        switch (StatusDemoOption) {
                            case 0:
                                GAMEDEMO = 2;
                                NewLData = HUB_LDATA;
                                new_level_from_menu = 1;
                                break;
                            case 1:
                                SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
                                break;
                        }
                    } else if (up != 0) {
                        if (StatusDemoOption > 0) {
                            --StatusDemoOption;
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        }
                    } else if (down != 0) {
                        if (StatusDemoOption < 1) {
                            ++StatusDemoOption;
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        }
                    }
                }
                return;
            case 2:
                if (GameTimer.time_elapsed >= 8.0f) {
                    SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
                }
                return;
            default:
                return;
        }
        if (StatusPacket.status_flags == 0) {
            return;
        }
        if (!(StatusPacket.newly_completed != 0 && StatusPacket.area != NULL && StatusPacket.area == NEGOTIATIONS_ADATA)) {
            u32 button = Text_Language != NULANGUAGE_JAPANESE ? GAMEPAD_JUMP : GAMEPAD_MENUSELECT;
            if ((StatusPacket.player0_active != 0 && (GamePadStorage[0].menu_buttons_pressed & button) != 0) ||
                (StatusPacket.player1_active != 0 && (GamePadStorage[1].menu_buttons_pressed & button) != 0) ||
                (StatusPacket.unknown_b0 & 2) != 0) {
                STATUS_STAGE_s *stage = StatusPacket.stage;
                if (stage == NULL) {
                    goto status_done;
                }
                if (stage->skip_callback != NULL) {
                    stage->skip_callback(stage, &StatusPacket);
                }
            }
        }
        if (StatusPacket.stage != NULL && StatusPacket.stage->update_callback != NULL) {
            StatusPacket.stage->update_callback(StatusPacket.stage, &StatusPacket, elapsed);
        }
    status_done:
        StatusRumbleUpdate();
        if (StatusPacket.status_flags == 0 && GameTimer.time_elapsed >= 5.0f) {
            FinishStatusPacket(0);
        }
    }
}
