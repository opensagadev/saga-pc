#include "saga.h"

typedef struct SHARD_LOST_OWNER_PC {
    u8 unknown_00[0x3c];
    u8 state_flags;
    u8 unknown_3d[3];
    f32 collection_time;
    GAMEOBJECT *collector;
    u8 unknown_48[0xc];
} SHARD_LOST_OWNER_PC;
STATIC_ASSERT(sizeof(SHARD_LOST_OWNER_PC) == 0x54);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, state_flags) == 0x3c);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, collection_time) == 0x40);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, collector) == 0x44);
typedef struct WORLD_SHARDS_READER_PC {
    u8 unknown_0000[0x5090];
    SHARD_LOST_OWNER_PC *shards;
    i32 shard_count;
} WORLD_SHARDS_READER_PC;
STATIC_ASSERT(offsetof(WORLD_SHARDS_READER_PC, shards) == 0x5090);
STATIC_ASSERT(offsetof(WORLD_SHARDS_READER_PC, shard_count) == 0x5094);
void __cdecl Shards_HandleLostObj(WORLDINFO *opaque_world, GAMEOBJECT *object) {
    WORLD_SHARDS_READER_PC *world = (WORLD_SHARDS_READER_PC *)opaque_world;
    SHARD_LOST_OWNER_PC *shard = world->shards;
    i32 i;
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        if ((shard->state_flags & 0xc) == 4 && shard->collector == object) {
            shard->state_flags &= (u8)~4;
            shard->collection_time = 0.0f;
            shard->collector = NULL;
        }
    }
}

static void Shop_DrawCharacter(shopitem_s *item, NUVEC *position, f32 scale_value, f32 ypush, u16 xrot, u16 yrot,
                               u16 zrot) {
    if (!(!NuSpecialExistsFn(&iconback))) {
        i32 character_id;
        u8 unlocked;
        i32 unavailable;
        f32 alpha;
        NUVEC scale;
        NUMTX matrix;
        i32 icon_object_id;
        i32 top_shelf_character = 0;
        if (item == &TopShelf[1]) {
            top_shelf_character = 1;
            item = &CharItems[(i32)(NuFmod(GameTimer.time_elapsed, (f32)(SHOPCHARCOUNT) * 0.2f) / 0.2f)];
        }

        character_id = (u16)(item->item_id);
        unlocked = CollectIDUnlocked(character_id) != NULL;
        unavailable = !unlocked || item->unlocked != 1;
        alpha = 1.0f;
        if (!CollectIDUnlocked(character_id))
            alpha = 0.5f;

        scale.x = scale.y = scale.z = scale_value;
        NuMtxSetScale(&matrix, &scale);
        NuMtxRotateXInline(&matrix, xrot);
        NuMtxRotateYInline(&matrix, yrot);
        NuMtxRotateZInline(&matrix, zrot);
        NuMtxTranslate(&matrix, position);
        matrix.m31 += ypush;

        NuSpecialDrawAtAlpha(&iconback, &matrix, top_shelf_character ? 1.0f : alpha);

        icon_object_id = CDataList[character_id].icon_model;
        if (icon_object_id != -1) {
            struct WORLDINFO_s *world;
            LEVEL_OBJECT_RUNTIME_PC *icon;
            if (unavailable != 0)
                ++icon_object_id;
            world = WORLD;
            icon = &world->level_objects[icon_object_id];
            if (icon->active != 0) {
                NuSpecialDrawAtAlpha(&icon->special, &matrix, alpha);
            }
        }
    }
}

void DrawItem(nuhspecial_s *special, NUVEC *position, float scale_value, float unused3, float y_push, u16 x_rot,
              u16 y_rot, u16 z_rot) {
    NUVEC scale;
    NUANGVEC rotation;
    NUMTX matrix;
    if (position != NULL) {
        if (NuSpecialExistsFn(special) != 0) {
            rotation.y = y_rot;
            rotation.x = x_rot;
            rotation.z = z_rot;
            NuMtxSetRotateXYZVU0(&matrix, &rotation);

            scale.x = scale.y = scale.z = scale_value;
            NuMtxScaleVU0(&matrix, &scale);
            *(NUVEC *)(&matrix.m30) = *position;
            matrix.m31 += y_push;
            NuSpecialDrawAt(special, &matrix);
        }
    }
}

void DrawArrow(nuhspecial_s *special, float scale_value) {
    if (NuSpecialExistsFn(special)) {
        NUVEC scale = {scale_value, scale_value, scale_value};
        NUMTX matrix = *NuSpecialGetDrawMtx(special);
        NuMtxPreScale(&matrix, &scale);
        NuSpecialDrawAt(special, &matrix);
    }
}

void DrawShop3D(WORLDINFO *world) {
    if (drawptr != NULL) {
        drawptr();
    }
    DrawTopShelf(picked);
    SetLevelLights(world->rtl_set, 1.0f);
}

extern i32(__cdecl *menuptr)(void *menu);
extern NUVEC *shopcamlookat;
i32 __cdecl ItemMenu(void *menu);

void __cdecl GetShopCamLookPos(NUVEC *position) {
    if (menuptr == ItemMenu && splshelf != NULL && picked != -1)
        *position = splshelf->pts[picked];
    else
        *position = *shopcamlookat;
}

static i32 Signals_Load(void *world_info, void *unused1) {
    i32 version;
    i32 index;
    WORLDINFO *world = ((WORLDINFO *)(world_info));
    if (world->signal_count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    world->signal_count = EdFileReadInt();
    for (index = 0; index < world->signal_count; ++index) {
        EdFileRead(world->signals[index].name, sizeof(world->signals[index].name));
        EdFileReadNuVec(&world->signals[index].position);
        world->signals[index].character_letter = EdFileReadChar();
        if (version > 1) {
            world->signals[index].suit_letter = EdFileReadChar();
            world->signals[index].yaw = EdFileReadShort();
        } else {
            world->signals[index].suit_letter = 'b';
            world->signals[index].yaw = 0;
        }
    }
    return 1;
}

static void Signals_Draw(void *world_info, void *unused1, float unused2) {
    i32 i;
    WORLDINFO *world = ((WORLDINFO *)(world_info));
    SIGNAL_s *signal = world->signals;
    for (i = 0; i < world->signal_count; i++, signal++) {
        NUMTX matrix;
        if (!signal->visible) {
            continue;
        }
        matrix = signal->matrix;
        if (world->level_objects[0x36].active) {
            NuSpecialDrawAt(&world->level_objects[0x36].special, &matrix);
        }
        if (signal->active) {
            if (world->level_objects[signal->animation_1].active) {
                NuSpecialDrawAt(&world->level_objects[signal->animation_1].special, &matrix);
            }
            if (world->level_objects[signal->animation_2].active) {
                f32 along_normal;
                f32 distance;
                f32 distance_squared;
                f32 alpha = signal->radius;
                NUVEC camera_offset;
                NuVecSub(&camera_offset, &GameCam->position, &signal->position);
                along_normal = NuVecDot(&camera_offset, &signal->normal);
                distance = NuVecMag(&camera_offset);
                distance_squared = distance * distance - along_normal * along_normal;
                if (distance_squared < 2.25f) {
                    f32 fade;
                    if (distance_squared <= 0.5625f) {
                        fade = 0.0f;
                    } else {
                        fade = (NuFsqrt(distance_squared) - 0.75f) / 0.75f;
                    }
                    if (along_normal < 0.0f || along_normal > 2.0f) {
                        if (along_normal >= 0.0f) {
                            along_normal -= 2.0f;
                        } else {
                            along_normal = -along_normal;
                        }
                        if (along_normal < 0.75f) {
                            fade += (1.0f - fade) * (along_normal / 0.75f);
                        } else {
                            fade = 1.0f;
                        }
                    }
                    alpha *= fade;
                }
                alpha *= signal->scale;
                if (alpha > 0.0f) {
                    NuSpecialDrawAtAlpha(&world->level_objects[signal->animation_2].special, &matrix, alpha);
                }
            }
        } else if (world->level_objects[signal->animation_0].active) {
            NuSpecialDrawAt(&world->level_objects[signal->animation_0].special, &matrix);
        }
    }
}

static void Signal_SetGizmoVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        Signal_SetVisibility(((SIGNAL_s *)(gizmo->object)), visible);
    }
}

i32 SpeederBlowupHack(GIZMOBLOWUP_PC *blowup, i32 unused1) {
    if (blowup == NULL) {
        return 1;
    }
    return NuStrCmp(blowup->name, "thermocrate_011") != 0 && NuStrCmp(blowup->name, "thermocrate_021") != 0 &&
           NuStrCmp(blowup->name, "thermocrate_031") != 0 && NuStrCmp(blowup->name, "minikit101") != 0;
}

void SpeederChaseA_Reset(WORLDINFO *unused0) {
    troopercannons_beenReset = 0;
    InitBikeParts();
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "SpeedersToKill", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "SpeedersKilled", NULL);
    LevAIMessage[2] = CheckGizAIMessage(gizaimessagesys, "Stage", NULL);
    LevAIMessage[3] = CheckGizAIMessage(gizaimessagesys, "Phase", NULL);
    LevAIMessage[4] = CheckGizAIMessage(gizaimessagesys, "KeepPlayersOnSpeeders", NULL);
    LevAIMessage[5] = CheckGizAIMessage(gizaimessagesys, "SpeederMode", NULL);
    players_going_forward = 1;
}
