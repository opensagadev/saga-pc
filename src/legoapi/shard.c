#include "saga.h"
extern CHEAT_PC ExtrasCheatsPc[46];
/* Integrate these declarations once. Keep all existing canonical data owners. */
typedef struct SHOP_SAVE_VIEW_PC {
    u8 unknown_0000[0x7bd4];
    u32 hint_purchased_bits[3];
    u32 character_purchased_bits[4];
    u32 extra_unlocked_bits[2];
    u32 gold_brick_purchased_bits[2];
    u32 extra_purchased_bits[2];
    u8 unknown_7c08[0x15];
    u8 bolt_unlocked;
} SHOP_SAVE_VIEW_PC;
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, hint_purchased_bits) == 0x7bd4);
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, character_purchased_bits) == 0x7be0);
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, extra_unlocked_bits) == 0x7bf0);
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, gold_brick_purchased_bits) == 0x7bf8);
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, extra_purchased_bits) == 0x7c00);
STATIC_ASSERT(offsetof(SHOP_SAVE_VIEW_PC, bolt_unlocked) == 0x7c1d);
STATIC_ASSERT(offsetof(shopitem_s, item_id) == 0x60);
STATIC_ASSERT(offsetof(shopitem_s, type) == 0x62);
STATIC_ASSERT(offsetof(shopitem_s, unlocked) == 0x63);
STATIC_ASSERT(offsetof(shopitem_s, price) == 0x64);
STATIC_ASSERT(offsetof(shopitem_s, special) == 0x68);
STATIC_ASSERT(offsetof(COLLECTID, cheat_code) == 0xc);
STATIC_ASSERT(offsetof(COLLECTID, field3_0x4) == 4);
extern shopitem_s *CodeItems;
extern i32 SHOPEXTRACOUNT, SHOPCUTCOUNT;
extern i32 oldpicked, subpicked, charcheatix, extracheatix;
extern u32 codelist[144];
extern nuhspecial_s extrasils[44], atoz0to9icon[36];
extern nuhspecial_s infoblank, cutblank, cutfilm_unlocked, cutfilm_locked;
extern nuhspecial_s toolblank, codeblank, question, arrow1, arrow2, arrow3, arrow4;
extern struct nugspline_s *shopcamspline;
extern NUVEC *shopcampos, *shopcamlookat;
extern NUVEC CharCurPos[7], HintCurPos[7], ExtraCurPos[7], BrickCurPos[7], CutCurPos[7];
extern i32 shop_from_cutsceneplayer;
extern f32 pickedbing, scalenorm, scalepicked, scale2, scale3, scale4;
extern f32 TopBigScale[6], TopShelfPush[6], TopBigPush[6];
extern f32 SubBigCharPush, SubNormCharPush, subpush[3];
extern f32 codeshelfscale[6], codemenuscale[6], codebigscale[6];
extern struct CUT_PLAYER_ACTIVE_PC *shopcutsceneplayer;
extern i32 CutScenePlayCountPc;
extern f32 ShopNativeScalePc[6], ShopNativeBigFactorPc[6], ShopNativePushPc[6];
extern f32 ShopNativeSubInitAPc, ShopNativeSubInitBPc;
extern f32 ShopSubInitAPc, ShopSubInitBPc;
void LoadShelfSplines(void);
void InitExtraList(void);
void InitAlphaList(void);
void UpdateCharacterIDs(void);
void InitShop(WORLDINFO *);
extern f32 ShopCharTopOffsetPc;
extern f32 ShopTopBrickOffsetPc;

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

/* Place before Shop_DrawCharacter in shard.c; static native same-TU helpers. */

static void NuMtxRotateXInline(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static void NuMtxRotateYInline(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
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

static void NuMtxRotateZInline(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cosx - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}

/* Replace the existing sole static owner in shard.c; native same 450B function. */
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
            item =
                CharItems - (i32)(NuFmod(GameTimer.time_elapsed, (f32)(SHOPCHARCOUNT * (double)0.2f)) / (double)-0.2f);
        }

        character_id = (u16)(item->item_id);
        unlocked = character_id == VehicleBoltId7f17d4Pc ? ((SHOP_SAVE_VIEW_PC *)&Game)->bolt_unlocked != 0
                                                         : CollectIDUnlocked(character_id) != NULL;
        unavailable = !unlocked || item->unlocked != 1;
        alpha = 1.0f;
        if (!(character_id == VehicleBoltId7f17d4Pc ? ((SHOP_SAVE_VIEW_PC *)&Game)->bolt_unlocked != 0
                                                    : CollectIDUnlocked(character_id) != NULL))
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
            rotation.x = x_rot;
            rotation.y = y_rot;
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

/* PC4c6c50/21, append to shard.c (or retain C linkage in C++).
 * 880100 native callback BSS0, single owner; SHOPACTIVE existing880494. */
extern void(__cdecl *ShopDrawPanelFnPc)(void);
void __cdecl DrawShopPanel(void) {
    if (SHOPACTIVE != 0 && ShopDrawPanelFnPc != NULL)
        ShopDrawPanelFnPc();
}

void LoadShelfSplines(void) {
    i32 i, shelf_count, character_count, code_count;
    NUVEC start, end, diff;
    splshelf = NuSplineFind(WORLD->current_gscn, "shelf_top1");
    splcharshelf = NuSplineFind(WORLD->current_gscn, "shelf_bottom");
    splcodes = NuSplineFind(WORLD->current_gscn, "shelf_6");
    if (splshelf == NULL || splcharshelf == NULL || splcodes == NULL)
        return;
    shelf_count = (i16)splshelf->length;
    character_count = (i16)splcharshelf->length;
    code_count = (i16)splcodes->length;
    memset(SubShelfPos, 0, sizeof(SubShelfPos));
    memset(ShelfPos, 0, sizeof(ShelfPos));
    memset(CodePos, 0, 6 * sizeof(*CodePos));
    for (i = 0; i < shelf_count; ++i)
        ShelfPos[i] = splshelf->pts[i];
    for (i = 0; i < character_count; ++i)
        SubShelfPos[i] = splcharshelf->pts[i];
    for (i = 0; i < code_count; ++i)
        CodePos[i] = splcodes->pts[i];
    start = splshelf->pts[0];
    end = splshelf->pts[shelf_count];
    NuVecSub(&diff, &end, &start);
    shelfang = (u16)NuAtan2D(diff.x, diff.z);
}

void InitExtraList(void) {
    i32 i;
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    for (i = 0; i < 44; ++i) {
        NuStrCpy(ExtraItems[i].special_name, ExtrasCheatsPc[i].extra_name);
        NuSpecialFind(WORLD->current_gscn, &ExtraItems[i].special, ExtraItems[i].special_name, 1);
        ExtraItems[i].item_id = (i16)i;
        ExtraItems[i].type = 2;
        ExtraItems[i].unlocked = 0;
        if (((u64)save->extra_unlocked_bits[i / 32] & ((u64)1 << (i % 32))) != 0) {
            ExtraItems[i].unlocked = 1;
            save->extra_purchased_bits[i / 32] |= 1u << (i & 31);
        }
        ExtraItems[i].price = ExtrasCheatsPc[i].extra_price;
    }
    SHOPEXTRACOUNT = 44;
    for (i = 0; i < SHOPEXTRACOUNT; ++i) {
        char name[32];
        NuStrCpy(name, ExtraItems[i].special_name);
        NuStrCat(name, "b");
        NuSpecialFind(WORLD->current_gscn, &extrasils[i], name, 1);
    }
}

void InitAlphaList(void) {
    i32 i;
    for (i = 0; i < 26; ++i) {
        char name[16] = {0};
        sprintf(name, "shop_%c", 'a' + i);
        NuSpecialFind(WORLD->current_gscn, &atoz0to9icon[i], name, 1);
    }
    for (i = 0; i < 10; ++i) {
        char name[16] = {0};
        sprintf(name, "shop_%c", '0' + i);
        NuSpecialFind(WORLD->current_gscn, &atoz0to9icon[i + 26], name, 1);
    }
}

void UpdateCharacterIDs(void) {
    i32 i;
    for (i = 0; i < ShopCollectionPc.count; ++i) {
        COLLECTID *collect = (COLLECTID *)&ShopCollectionPc.list[i];
        CharItems[i].item_id = collect->id;
        CharItems[i].price = collect->field3_0x4;
    }
}

void InitShop(WORLDINFO *world) {
    i32 i, code_count;
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    CharItems = (shopitem_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 100 * sizeof(shopitem_s));
    HintItems = (shopitem_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    ExtraItems = (shopitem_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    CodeItems = (shopitem_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    BrickItems = (shopitem_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 14 * sizeof(shopitem_s));
    if (CharItems == NULL || HintItems == NULL || ExtraItems == NULL || CodeItems == NULL || BrickItems == NULL)
        return;
    TopShelf[0].type = 0;
    NuStrCpy(TopShelf[0].name, "Hint");
    NuStrCpy(TopShelf[0].special_name, "info");
    NuSpecialFind(things_scene, &TopShelf[0].special, "info", 1);
    TopShelf[1].type = 1;
    TopShelf[1].name[0] = 0;
    TopShelf[1].special_name[0] = 0;
    memset(&TopShelf[1].special, 0, sizeof(TopShelf[1].special));
    TopShelf[2].type = 2;
    NuStrCpy(TopShelf[2].name, "Extra");
    NuStrCpy(TopShelf[2].special_name, "tool_box");
    NuSpecialFind(WORLD->current_gscn, &TopShelf[2].special, "tool_box", 1);
    TopShelf[3].type = 3;
    NuStrCpy(TopShelf[3].name, "Code");
    NuStrCpy(TopShelf[3].special_name, "shop_question");
    NuSpecialFind(WORLD->current_gscn, &TopShelf[3].special, "shop_question", 1);
    TopShelf[4].type = 4;
    NuStrCpy(TopShelf[4].name, "Gold Bricks");
    NuStrCpy(TopShelf[4].special_name, "gold_brick");
    NuSpecialFind(things_scene, &TopShelf[4].special, "gold_brick", 1);
    TopShelf[5].type = 5;
    NuStrCpy(TopShelf[5].name, "Cut Scenes");
    NuStrCpy(TopShelf[5].special_name, "FMV");
    NuSpecialFind(world->current_gscn, &TopShelf[5].special, "fmv", 1);
    charcheatix = 0;
    extracheatix = 0;
    memset(codelist, 0, sizeof(codelist));
    SHOPCHARCOUNT = 0;
    code_count = 0;
    for (i = 0; i < ShopCollectionPc.count && i < 100; ++i) {
        COLLECTID *collect = (COLLECTID *)&ShopCollectionPc.list[i];
        if (collect != NULL) {
            i32 character_id = collect->id;
            shopitem_s *item;
            CharItems[SHOPCHARCOUNT].type = 1;
            CharItems[SHOPCHARCOUNT].unlocked = 0;
            if (((u64)save->character_purchased_bits[SHOPCHARCOUNT / 32] & ((u64)1 << (SHOPCHARCOUNT % 32))) != 0)
                CharItems[SHOPCHARCOUNT].unlocked = 1;
            item = &CharItems[SHOPCHARCOUNT];
            if (CDataList[character_id].name_id == -1)
                NuStrCpy(item->name, CDataList[character_id].dir);
            else
                NuStrCpy(item->name, TTab[CDataList[character_id].name_id]);
            memset(&CharItems[SHOPCHARCOUNT].special, 0, sizeof(CharItems[SHOPCHARCOUNT].special));
            if (code_count < 144)
                codelist[code_count++] = SfxNameHash(collect->cheat_code);
            ++SHOPCHARCOUNT;
        }
    }
    UpdateCharacterIDs();
    charcheatix = code_count;
    for (i = 0; i < 44; ++i)
        if (code_count < 144)
            codelist[code_count++] = SfxNameHash(ExtrasCheatsPc[i].code);
    extracheatix = code_count;
    NuSpecialFind(things_scene, &iconback, "icon_back_neutral", 1);
    NuSpecialFind(WORLD->current_gscn, &infoblank, "info_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &cutblank, "fmv_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &cutfilm_unlocked, "shop_film1", 1);
    NuSpecialFind(WORLD->current_gscn, &cutfilm_locked, "shop_film1b", 1);
    NuSpecialFind(WORLD->current_gscn, &toolblank, "tool_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &codeblank, "code_blank", 1);
    NuSpecialFind(things_scene, &question, "question_icon", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow1, "shop_arrow1", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow2, "shop_arrow2", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow3, "shop_arrow3", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow4, "shop_arrow4", 1);
    NuSpecialSetVisibility(&arrow1, 0);
    NuSpecialSetVisibility(&arrow2, 0);
    NuSpecialSetVisibility(&arrow3, 0);
    NuSpecialSetVisibility(&arrow4, 0);
    InitExtraList();
    InitAlphaList();
    for (i = 0; i < SHOPHINTCOUNT; ++i) {
        HINT_PC *hint = Hint_FindHint(ShopHintIdsPc[i]);
        HintItems[i].type = 0;
        HintItems[i].item_id = (i16)i;
        HintItems[i].price = hint != NULL ? hint->price : 0;
        HintItems[i].unlocked = 0;
        if (((u64)save->hint_purchased_bits[i / 32] & ((u64)1 << (i % 32))) != 0)
            HintItems[i].unlocked = 1;
        if (HintItems[i].price == 0)
            HintItems[i].unlocked = 1;
        HintItems[i].special = atoz0to9icon[i];
    }
    shopcamspline = NuSplineFind(WORLD->current_gscn, "shop_cam");
    if (shopcamspline != NULL) {
        shopcampos = shopcamspline->pts;
        shopcamlookat = shopcamspline->pts + 1;
    } else {
        shopcampos = NULL;
        shopcamlookat = NULL;
    }
    LoadShelfSplines();
    memcpy(CharCurPos, SubShelfPos, sizeof(CharCurPos));
    for (i = 0; i < 7; ++i)
        CharShelfIds[i] = (SHOPCHARCOUNT + i - 3) % SHOPCHARCOUNT;
    if (SHOPHINTCOUNT > 0) {
        memcpy(HintCurPos, SubShelfPos, sizeof(HintCurPos));
        for (i = 0; i < 7; ++i)
            HintShelfIds[i] = (SHOPHINTCOUNT + i - 3) % SHOPHINTCOUNT;
    }
    if (SHOPEXTRACOUNT > 0) {
        memcpy(ExtraCurPos, SubShelfPos, sizeof(ExtraCurPos));
        for (i = 0; i < 7; ++i)
            ExtraShelfIds[i] = (SHOPEXTRACOUNT + i - 3) % SHOPEXTRACOUNT;
    }
    for (i = 0; i < SHOPGOLDBRICKS; ++i) {
        BrickItems[i].type = 4;
        BrickItems[i].item_id = (i16)i;
        BrickItems[i].price = 10000 + i * 5000;
        BrickItems[i].special = TopShelf[4].special;
        BrickItems[i].unlocked = 0;
        if (((u64)save->gold_brick_purchased_bits[i / 32] & ((u64)1 << (i % 32))) != 0)
            BrickItems[i].unlocked = 1;
    }
    memcpy(BrickCurPos, SubShelfPos, sizeof(BrickCurPos));
    for (i = 0; i < 7; ++i)
        BrickShelfIds[i] = (i + 11) % 14;
    SHOPCUTCOUNT = CutScenePlayCountPc;
    for (i = 0; i < SHOPCUTCOUNT && i < 128; ++i) {
        CutItems[i].special = TopShelf[5].special;
        CutItems[i].item_id = (i16)i;
        CutItems[i].type = 5;
        CutItems[i].price = 0;
        CutItems[i].unlocked = 0;
    }
    if (shop_from_cutsceneplayer == 0) {
        if (SHOPCUTCOUNT > 0) {
            memcpy(CutCurPos, SubShelfPos, sizeof(CutCurPos));
            for (i = 0; i < 7; ++i)
                CutShelfIds[i] = (SHOPCUTCOUNT + i - 3) % SHOPCUTCOUNT;
        }
        picked = 0;
    } else {
        picked = oldpicked;
    }
    subpicked = 0;
    pickedbing = 0.4f;
    scalenorm = 1.0f;
    scalepicked = 1.4f;
    scale2 = scale4 = scalenorm;
    scale3 = scalepicked;
    for (i = 0; i < 6; ++i) {
        TopShelfScale[i] = ShopNativeScalePc[i == 1 ? 2 : (i == 2 ? 1 : i)];
        topscale[i] = TopShelfScale[i];
        TopBigScale[i] = TopShelfScale[i] * ShopNativeBigFactorPc[i];
        TopShelfPush[i] = ShopNativePushPc[i];
    }
    TopBigPush[0] = TopBigPush[1] = TopBigPush[3] = TopBigPush[5] = 0.0f;
    TopBigPush[2] = -0.005f;
    TopBigPush[4] = -0.01f;
    for (i = 0; i < 6; ++i)
        toppush[i] = TopBigPush[i];
    ShopSubInitAPc = ShopNativeSubInitAPc;
    ShopSubInitBPc = ShopNativeSubInitBPc;
    SubBigCharPush = 0.05f;
    SubNormCharPush = 0.02f;
    subpush[0] = subpush[2] = SubNormCharPush;
    subpush[1] = SubBigCharPush;
    for (i = 0; i < 6; ++i) {
        codeshelfscale[i] = codemenuscale[i] = (f32)(TopShelfScale[3] * (double)0.8f);
        codebigscale[i] = (f32)((TopShelfScale[3] * (double)ShopNativeBigFactorPc[3]) * (double)0.8f);
    }
    shopcutsceneplayer = CutScenePlayer_Available();
}

/* Append in shard.c after Shop_DrawCharacter and DrawItem; retain those owners. */
static void Shop_DrawItemPc(shopitem_s *item, NUVEC *position, f32 scale_value, f32 alpha, i32 opaque, f32 ypush,
                            u16 xrot, u16 yrot, u16 zrot) {
    NUVEC scale;
    NUANGVEC rotation;
    NUMTX matrix;
    if (item == NULL || position == NULL)
        return;
    if (NuSpecialExistsFn(&item->special) == 0)
        return;
    rotation.x = xrot;
    rotation.y = yrot;
    rotation.z = zrot;
    NuMtxSetRotateXYZVU0(&matrix, &rotation);
    scale.x = scale.y = scale.z = scale_value;
    NuMtxScaleVU0(&matrix, &scale);
    *(NUVEC *)&matrix.m30 = *position;
    matrix.m31 += ypush;
    if (opaque != 0)
        NuSpecialDrawAt(&item->special, &matrix);
    else
        NuSpecialDrawAtAlpha(&item->special, &matrix, alpha);
}

static void Shop_DrawBrickPc(shopitem_s *item, NUVEC *position, f32 scale_value, f32 ypush, u16 xrot, u16 yrot,
                             u16 zrot) {
    NUVEC scale;
    NUMTX matrix;
    if (NuSpecialExistsFn(&item->special) != 0) {
        scale.x = scale.y = scale.z = scale_value;
        NuMtxSetScale(&matrix, &scale);
        NuMtxRotateXInline(&matrix, xrot);
        NuMtxRotateYInline(&matrix, yrot);
        NuMtxRotateZInline(&matrix, zrot);
        NuMtxTranslate(&matrix, position);
        matrix.m31 += ypush;
        NuSpecialDrawAt(&item->special, &matrix);
    }
}

void DrawTopShelf(i32 unused) {
    Shop_DrawItemPc(&TopShelf[0], &ShelfPos[0], topscale[0], 1.0f, 1, toppush[0] + -0.006f, 0, shelfang, 0);
    Shop_DrawCharacter(&TopShelf[1], &ShelfPos[1], topscale[1], toppush[1] + ShopCharTopOffsetPc, 0, shelfang, 0);
    Shop_DrawItemPc(&TopShelf[2], &ShelfPos[2], topscale[2], 1.0f, 1, toppush[2] + 0.005f, 0, shelfang, 0);
    DrawItem(&codeblank, &ShelfPos[3], topscale[3], 1.0f, toppush[3] + -0.005f, 0, shelfang, 0);
    Shop_DrawItemPc(&TopShelf[3], &ShelfPos[3], topscale[3], 1.0f, 0, toppush[3] + -0.005f, 0, shelfang, 0);
    Shop_DrawBrickPc(&TopShelf[4], &ShelfPos[4], topscale[4], toppush[4] + ShopTopBrickOffsetPc, 0,
                     (u16)(shelfang + 0x2000), 0);
    Shop_DrawItemPc(&TopShelf[5], &ShelfPos[5], topscale[5], 1.0f, 1, toppush[5], 0, shelfang, 0);
}
