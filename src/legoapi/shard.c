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

/* The retail shop code clamps through nested min/max macros: the inner min is evaluated twice. */
#define SHOP_FMIN(a, b) ((a) < (b) ? (a) : (b))
#define SHOP_FMAX(a, b) ((a) > (b) ? (a) : (b))
#define SHOP_CLAMP01(x) SHOP_FMAX(0.0f, SHOP_FMIN(x, 1.0f))
#define SHOP_LERP(a, b, t) ((a) + (t) * ((b) - (a)))
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

i32 __cdecl SpeederBlowupHack(GIZMOBLOWUP_PC *blowup, i32 hit_type) {
    (void)hit_type;
    if (blowup != NULL) {
        if (NuStrCmp(blowup->name, "thermocrate_011") == 0 || NuStrCmp(blowup->name, "thermocrate_021") == 0 ||
            NuStrCmp(blowup->name, "thermocrate_031") == 0 || NuStrCmp(blowup->name, "minikit101") == 0)
            return 0;
    }
    return 1;
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
    i32 i, code_count, price;
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
            if (CDataList[character_id].name_id != -1)
                NuStrCpy(item->name, TTab[CDataList[character_id].name_id]);
            else
                NuStrCpy(item->name, CDataList[character_id].name);
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
    for (i = 0; i < 7; ++i)
        CharCurPos[i] = SubShelfPos[i];
    for (i = 0; i < 7; ++i)
        CharShelfIds[i] = (SHOPCHARCOUNT + i - 3) % SHOPCHARCOUNT;
    if (SHOPHINTCOUNT > 0) {
        for (i = 0; i < 7; ++i)
            HintCurPos[i] = SubShelfPos[i];
        for (i = 0; i < 7; ++i)
            HintShelfIds[i] = (SHOPHINTCOUNT + i - 3) % SHOPHINTCOUNT;
    }
    if (SHOPEXTRACOUNT > 0) {
        for (i = 0; i < 7; ++i)
            ExtraCurPos[i] = SubShelfPos[i];
        for (i = 0; i < 7; ++i)
            ExtraShelfIds[i] = (SHOPEXTRACOUNT + i - 3) % SHOPEXTRACOUNT;
    }
    price = 10000;
    for (i = 0; i < SHOPGOLDBRICKS; ++i) {
        BrickItems[i].type = 4;
        BrickItems[i].item_id = (i16)i;
        BrickItems[i].price = price;
        BrickItems[i].special = TopShelf[4].special;
        BrickItems[i].unlocked = 0;
        if (((u64)save->gold_brick_purchased_bits[i / 32] & ((u64)1 << (i % 32))) != 0)
            BrickItems[i].unlocked = 1;
        price += 5000;
    }
    for (i = 0; i < 7; ++i)
        BrickCurPos[i] = SubShelfPos[i];
    for (i = 0; i < 7; ++i)
        BrickShelfIds[i] = (i + 11) % 14;
    SHOPCUTCOUNT = CutScenePlayCountPc;
    for (i = 0; i < SHOPCUTCOUNT && i < 128; ++i) {
        shopitem_s *item = &CutItems[i];
        item->special = TopShelf[5].special;
        item->item_id = (i16)i;
        item->type = 5;
        item->price = 0;
        item->unlocked = 0;
    }
    if (shop_from_cutsceneplayer == 0) {
        if (SHOPCUTCOUNT > 0) {
            for (i = 0; i < 7; ++i)
                CutCurPos[i] = SubShelfPos[i];
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
    TopShelfScale[0] = ShopNativeScalePc[0];
    TopShelfScale[1] = ShopNativeScalePc[2];
    TopShelfScale[2] = ShopNativeScalePc[1];
    TopShelfScale[3] = ShopNativeScalePc[3];
    TopShelfScale[4] = ShopNativeScalePc[4];
    TopShelfScale[5] = ShopNativeScalePc[5];
    for (i = 0; i < 6; ++i)
        topscale[i] = TopShelfScale[i];
    TopBigScale[0] = TopShelfScale[0] * ShopNativeBigFactorPc[0];
    TopBigScale[1] = TopShelfScale[1] * ShopNativeBigFactorPc[1];
    TopBigScale[2] = TopShelfScale[2] * ShopNativeBigFactorPc[2];
    TopBigScale[3] = TopShelfScale[3] * ShopNativeBigFactorPc[3];
    TopBigScale[4] = TopShelfScale[4] * ShopNativeBigFactorPc[4];
    TopBigScale[5] = TopShelfScale[5] * ShopNativeBigFactorPc[5];
    TopShelfPush[0] = ShopNativePushPc[0];
    TopShelfPush[1] = ShopNativePushPc[1];
    TopShelfPush[2] = ShopNativePushPc[2];
    TopShelfPush[3] = ShopNativePushPc[3];
    TopShelfPush[4] = ShopNativePushPc[4];
    TopShelfPush[5] = ShopNativePushPc[5];
    TopBigPush[0] = 0.0f;
    TopBigPush[1] = 0.0f;
    TopBigPush[2] = -0.005f;
    TopBigPush[3] = 0.0f;
    TopBigPush[4] = -0.01f;
    TopBigPush[5] = 0.0f;
    for (i = 0; i < 6; ++i)
        toppush[i] = TopBigPush[i];
    ShopSubInitAPc = ShopNativeSubInitAPc;
    ShopSubInitBPc = ShopNativeSubInitBPc;
    SubBigCharPush = 0.05f;
    SubNormCharPush = 0.02f;
    subpush[0] = subpush[2] = SubNormCharPush;
    subpush[1] = SubBigCharPush;
    for (i = 0; i < 6; ++i) {
        codebigscale[i] = (f32)((TopShelfScale[3] * (f64)ShopNativeBigFactorPc[3]) * (f64)0.8f);
        codeshelfscale[i] = codemenuscale[i] = (f32)(TopShelfScale[3] * (f64)0.8f);
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

/* Native PC shop state machine. Child callbacks remain real unresolved native
 * references until their complete functions are ported; menu ID 13 stays unwired. */
typedef struct SHOP_INPUT_PC {
    i32 left_pressed, right_pressed, up_pressed, down_pressed;
    i32 left_held, right_held, up_held, down_held;
    i32 confirm, cancel;
} SHOP_INPUT_PC;
STATIC_ASSERT(sizeof(SHOP_INPUT_PC) == 0x28);
STATIC_ASSERT(offsetof(SHOP_INPUT_PC, confirm) == 0x20);
STATIC_ASSERT(offsetof(SHOP_INPUT_PC, cancel) == 0x24);

typedef i32(__cdecl *SHOP_MENU_CALLBACK_PC)(void *);
typedef void(__cdecl *SHOP_DRAW_CALLBACK_PC)(void);
extern SHOP_MENU_CALLBACK_PC ShopMenuParentPc[3], ShopOldMenuParentPc[3], ShopOldMenuFnPc;
extern SHOP_DRAW_CALLBACK_PC ShopDrawParentPc[3], ShopOldDrawParentPc[3];
extern SHOP_DRAW_CALLBACK_PC ShopOldDrawPanelFnPc, ShopOldDrawFnPc;
extern i32 ShopMenuLevelPc, ShopDrawLevelPc, ShopOldMenuLevelPc, ShopOldDrawLevelPc;
extern i32 ShopExitPc, ShopLastItemPc, ShopEnteredPc, ShopItemChangedPc, ShopMoveSfxLockPc, ShopSubMenuPc;
extern i32 ShopEaseSubInPc, ShopMenuPc, ShopColumnPc;
extern f32 ShopSlideTimerPc, ShopHintDrawWaitPc, ShopOldPickedScalePc, ShopOldPickedPushPc;
extern f32 ShopScaleOverridePc[7], ShopInOutScalePc;
extern char *ShopCheatNamePc;
extern char ShopUserCodePc[6];
extern u32 GAMEPAD_MENUCANCEL;
i32 __cdecl SubItemMenu(void *menu);
i32 __cdecl CodeMenu(void *menu);
void __cdecl DrawItemMenu2D(void);
void __cdecl DrawCodeMenu(void);
void __cdecl DrawCodeMenu3D(void);
void __cdecl DrawSubItemMenu2D(void);
void __cdecl DrawSubItemMenu3D(void);

/* 004c1080..004c11ba: input argument is EAX in the original same-TU helper. */
static void Shop_GetInputPc(SHOP_INPUT_PC *input) {
    i32 player;
    GAMEPAD_RUNTIME_INPUT_PC *pad;
    u32 held, pressed;
    memset(input, 0, sizeof(*input));
    for (player = 0; player < 2; ++player) {
        if (MissionMenuPacketPc.active_player[player] == 0)
            continue;
        pad = (GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player];
        if ((pad->pressed & GAMEPAD_MENUSELECT) != 0 && netclient == 0) {
            input->confirm = 1;
            return;
        }
        if ((pad->pressed & GAMEPAD_MENUCANCEL) != 0) {
            input->cancel = 1;
            return;
        }
        if (netclient != 0)
            continue;
        held = pad->held | pad->left_held;
        pressed = pad->pressed | pad->left_pressed;
        if ((held & GAMEPAD_DLEFT) != 0)
            input->left_held = 1;
        if ((held & GAMEPAD_DRIGHT) != 0)
            input->right_held = 1;
        if (input->left_held != 0 && input->right_held != 0)
            input->left_held = input->right_held = 0;
        if ((held & GAMEPAD_DUP) != 0)
            input->up_held = 1;
        if ((held & GAMEPAD_DDOWN) != 0)
            input->down_held = 1;
        if (input->up_held != 0 && input->down_held != 0)
            input->up_held = input->down_held = 0;
        if ((pressed & GAMEPAD_DLEFT) != 0)
            input->left_pressed = 1;
        if ((pressed & GAMEPAD_DRIGHT) != 0)
            input->right_pressed = 1;
        if (input->left_pressed != 0 && input->right_pressed != 0)
            input->left_pressed = input->right_pressed = 0;
        if ((pressed & GAMEPAD_DUP) != 0)
            input->up_pressed = 1;
        if ((pressed & GAMEPAD_DDOWN) != 0)
            input->down_pressed = 1;
        if (input->up_pressed != 0 && input->down_pressed != 0)
            input->up_pressed = input->down_pressed = 0;
    }
}

/* 004c4110..004c4193. PC cancellation belongs to the active child callback. */
i32 __cdecl DoShopMenu(void *menu) {
    i32 result;
    if (menuptr != NULL) {
        result = menuptr(menu);
        if (result == 5)
            return 1;
        if (result == 6) {
            menuptr = ShopMenuParentPc[ShopMenuLevelPc];
            ShopMenuParentPc[ShopMenuLevelPc] = NULL;
            --ShopMenuLevelPc;
            if (ShopMenuLevelPc < 0)
                ShopMenuLevelPc = 0;
            ShopDrawPanelFnPc = ShopDrawParentPc[ShopDrawLevelPc];
            ShopDrawParentPc[ShopDrawLevelPc] = NULL;
            --ShopDrawLevelPc;
            drawptr = NULL;
            if (ShopDrawLevelPc < 0)
                ShopDrawLevelPc = 0;
        }
    }
    return 0;
}

/* 004c6e90..004c74dd. The PC version deliberately ignores the generic MENU
 * confirm/cursor fields and permits selection of all six original shelves. */
i32 __cdecl ItemMenu(void *menu) {
    SHOP_INPUT_PC input;
    i32 entry_picked = picked;
    i32 candidate = picked;
    i32 selection = -1;
    i32 cancel = 0;
    f32 reverse_phase, old_factor, forward_factor;
    i32 old_angle, forward_angle;
    (void)menu;
    Shop_GetInputPc(&input);
    if (ShopSlideTimerPc >= 0.0f) {
        ShopSlideTimerPc -= FRAMETIME;
        ShopNameAlpha = 0.0f;
    }
    if (ShopExitPc != 0)
        ShopNameAlpha = 0.0f;
    ShopItemChangedPc = 0;
    Hint_CancelCurrent();
    ShopHintDrawWaitPc = 0.2f;
    if (ShopSlideTimerPc < 0.0f) {
        ShopMoveSfxLockPc = 0;
        if (input.left_pressed != 0 && picked > 0)
            candidate = picked - 1;
        else if (input.right_pressed != 0 && picked < 5)
            candidate = picked + 1;
        else if (input.confirm != 0)
            selection = picked;
        else if (input.cancel != 0)
            cancel = 1;
    }
    if (candidate != entry_picked) {
        ShopSubMenuPc = 0;
        ShopLastItemPc = picked;
        picked = candidate;
        if (picked < 0)
            picked = 0;
        else if (picked > 5)
            picked = 5;
        ShopSlideTimerPc = 0.125f;
        if (ShopMoveSfxLockPc == 0) {
            ShopMoveSfxLockPc = 1;
            GameAudio_PlaySfx(0x2f, &SubShelfPos[5], 0, 0);
        }
        ShopNameAlpha = 0.0f;
        GameCam_Blend(GameCam, 0.3f, 0.0f, 1);
    }
    if (selection != -1) {
        if (selection != picked) {
            picked = selection;
            if (ShopMoveSfxLockPc == 0) {
                ShopMoveSfxLockPc = 1;
                GameAudio_PlaySfx(0x2f, &SubShelfPos[5], 0, 0);
            }
            ShopNameAlpha = 0.0f;
        }
        ShopSlideTimerPc = 0.125f;
        ShopLastItemPc = picked;
        picked = -1;
        ShopSubMenuPc = 1;
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        GameCam_Blend(GameCam, 0.6f, 0.0f, 1);
        ShopNameAlpha = 0.0f;
    }
    if (cancel != 0) {
        ShopSlideTimerPc = 0.125f;
        ShopExitPc = 1;
        ShopLastItemPc = picked;
        picked = -1;
        ShopSubMenuPc = 0;
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
    }
    if (ShopSlideTimerPc < 0.0f) {
        ShopEnteredPc = 0;
    } else if ((ShopLastItemPc != picked || ShopEnteredPc != 0) && ShopSubMenuPc == 0) {
        reverse_phase = (f32)((f64)ShopSlideTimerPc * 8.0);
        old_angle = (i32)((f64)(1.0f - reverse_phase) * 32768.0 + 16384.0);
        forward_angle = (i32)((f64)reverse_phase * 32768.0 + 16384.0);
        old_factor = (f32)(1.0 - ((f64)NuTrigTable[(old_angle >> 1) & 0x7fff] + 1.0) * 0.5);
        forward_factor = (f32)(1.0 - ((f64)NuTrigTable[(forward_angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopLastItemPc != -1) {
            old_factor = old_factor < 1.0f ? old_factor : 1.0f;
            old_factor = old_factor > 0.0f ? old_factor : 0.0f;
            topscale[ShopLastItemPc] = TopBigScale[ShopLastItemPc] +
                                       (TopShelfScale[ShopLastItemPc] - TopBigScale[ShopLastItemPc]) * old_factor;
            forward_factor = forward_factor < 1.0f ? forward_factor : 1.0f;
            forward_factor = forward_factor > 0.0f ? forward_factor : 0.0f;
            toppush[ShopLastItemPc] = TopBigPush[ShopLastItemPc] +
                                      (TopShelfPush[ShopLastItemPc] - TopBigPush[ShopLastItemPc]) * forward_factor;
        }
        if (picked != -1 && ShopLastItemPc != -1) {
            old_factor = old_factor < 1.0f ? old_factor : 1.0f;
            old_factor = old_factor > 0.0f ? old_factor : 0.0f;
            topscale[picked] = TopShelfScale[picked] + (TopBigScale[picked] - TopShelfScale[picked]) * old_factor;
            forward_factor = forward_factor < 1.0f ? forward_factor : 1.0f;
            forward_factor = forward_factor > 0.0f ? forward_factor : 0.0f;
            toppush[picked] = TopShelfPush[picked] + (TopBigPush[picked] - TopShelfPush[picked]) * forward_factor;
        }
    }
    if (ShopSubMenuPc != 0 && ShopSlideTimerPc <= 0.0f) {
        ShopInOutScalePc = 0.0f;
        picked = ShopLastItemPc;
        ShopSlideTimerPc = 0.125f;
        ++ShopMenuLevelPc;
        ShopMenuParentPc[ShopMenuLevelPc] = menuptr;
        ShopSubMenuPc = 0;
        ShopEaseSubInPc = 1;
        ++ShopDrawLevelPc;
        ShopDrawParentPc[ShopDrawLevelPc] = ShopDrawPanelFnPc;
        if (TopShelf[ShopLastItemPc].type == 3) {
            ShopUserCodePc[0] = 'A';
            ShopUserCodePc[1] = 'A';
            ShopUserCodePc[2] = 'A';
            ShopUserCodePc[3] = 'A';
            ShopUserCodePc[4] = 'A';
            ShopUserCodePc[5] = 'A';
            ShopMenuPc = 11;
            ShopColumnPc = 0;
            ShopLastItemPc = -1;
            menuptr = CodeMenu;
            ShopCheatNamePc = NULL;
            ShopDrawPanelFnPc = DrawCodeMenu;
            drawptr = DrawCodeMenu3D;
        } else {
            menuptr = SubItemMenu;
            ShopDrawPanelFnPc = DrawSubItemMenu2D;
            drawptr = DrawSubItemMenu3D;
        }
    }
    if (ShopExitPc != 0 && ShopSlideTimerPc < 0.0f) {
        menuptr = NULL;
        return 5;
    }
    return 7;
}

/* 004c74e0..004c76c4. Preserve PC initialization to shelf zero, and the
 * complete cutscene-player return state rather than Android's defaults. */
i32 __cdecl UpdateShop(void *menu) {
    UpdateCharacterIDs();
    ShopScaleOverridePc[0] = ShopScaleOverridePc[6] = 0.0f;
    ShopScaleOverridePc[1] = ShopScaleOverridePc[5] = 0.4f;
    ShopScaleOverridePc[2] = ShopScaleOverridePc[4] = 0.8f;
    if (ShopNameAlpha < 1.0f) {
        ShopNameAlpha += FRAMETIME + FRAMETIME;
        if (ShopNameAlpha > 1.0f)
            ShopNameAlpha = 1.0f;
    }
    ShopLockedScalePc = SeekLinearF(ShopLockedScalePc, 1.0f, (f32)((f64)FRAMETIME * 3.0));
    SubNormCharPush = 0.02f;
    SubBigCharPush = 0.05f;
    if (menuptr == NULL) {
        if (shop_from_cutsceneplayer == 0) {
            menuptr = ItemMenu;
            ShopSlideTimerPc = 0.125f;
            ShopExitPc = 0;
            ShopEnteredPc = 1;
            picked = 0;
            ShopLastItemPc = 0;
            ShopDrawPanelFnPc = DrawItemMenu2D;
            ShopDrawLevelPc = 0;
            ShopDrawParentPc[0] = NULL;
            ShopMenuLevelPc = 0;
            ShopMenuParentPc[0] = NULL;
        } else {
            ShopDrawParentPc[0] = ShopOldDrawParentPc[0];
            topscale[5] = ShopOldPickedScalePc;
            shop_from_cutsceneplayer = 0;
            menuptr = ShopOldMenuFnPc;
            toppush[5] = ShopOldPickedPushPc;
            ShopMenuParentPc[0] = ShopOldMenuParentPc[0];
            ShopMenuParentPc[1] = ShopOldMenuParentPc[1];
            ShopDrawParentPc[2] = ShopOldDrawParentPc[2];
            ShopDrawParentPc[1] = ShopOldDrawParentPc[1];
            ShopDrawPanelFnPc = ShopOldDrawPanelFnPc;
            drawptr = ShopOldDrawFnPc;
            ShopMenuParentPc[2] = ShopOldMenuParentPc[2];
            ShopDrawLevelPc = ShopOldDrawLevelPc;
            picked = oldpicked;
            ShopMenuLevelPc = ShopOldMenuLevelPc;
        }
    }
    return DoShopMenu(menu) != 0;
}

/* The interaction batch shares SHOP_INPUT_PC and the existing Shop*Pc state
 * contract above. Native movement mode is 1=left, 2=right (not Android's sign). */
extern i32 ShopMoveItemsPc, ShopSubMoveSfxLockPc, ShopScrollKeyHitPc;
i32 __cdecl MoveSubItemsLeft(i32 *ids, NUVEC *positions, i32 count);
i32 __cdecl MoveSubItemsRight(i32 *ids, NUVEC *positions, i32 count);
void __cdecl Hint_ResetHint(i32 hint_id, i32 reset_completed);

/* 004c3900..004c3988 instructions; 004c3988..004c39a0 six-entry switch table. */
void __cdecl SelectSubItem(void) {
    shopitem_s *items;
    i32 index, result;
    switch (picked) {
        case 0:
            items = HintItems;
            index = HintShelfIds[3];
            break;
        case 1:
            items = CharItems;
            index = CharShelfIds[3];
            break;
        case 2:
            items = ExtraItems;
            index = ExtraShelfIds[3];
            break;
        case 4:
            items = BrickItems;
            index = BrickShelfIds[3];
            break;
        case 5:
            items = CutItems;
            index = CutShelfIds[3];
            break;
        default:
            return;
    }
    if (items == NULL || index == -1)
        return;
    result = SelectShopItem(items, index);
    if (result == -1)
        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
    else if (result == 1)
        ShopNameAlpha = 0.0f;
}

/* 004c39a0..004c410a. The PC callback uses direct shop-pad input, buys on one
 * confirm, and has no Android touch rows, item-selection mode, or cursor input. */
i32 __cdecl SubItemMenu(void *menu) {
    SHOP_INPUT_PC input;
    shopitem_s *items = NULL;
    shopitem_s *selected;
    i32 *ids = NULL, *count = NULL;
    NUVEC *positions = NULL;
    NUVEC position;
    i32 left = 0, right = 0, cancel = 0, scrolling = 0;
    f32 ypush = 0.0f, phase, factor;
    i32 angle, moved;
    (void)menu;
    Shop_GetInputPc(&input);
    if (ShopHintDrawWaitPc > 0.0f) {
        ShopHintDrawWaitPc -= FRAMETIME;
        ShopNameAlpha = 0.0f;
    }
    if (ShopExitPc != 0)
        ShopNameAlpha = 0.0f;
    if (picked == 0) {
        items = HintItems;
        ids = HintShelfIds;
        positions = HintCurPos;
        count = &SHOPHINTCOUNT;
        if (ShopHintDrawWaitPc <= 0.0f && (items[ids[3]].unlocked == 1 || items[ids[3]].price == 0)) {
            Hint_ResetHint(ShopHintIdsPc[(u16)items[ids[3]].item_id], 0);
            Hint_SetHintFromId(ShopHintIdsPc[(u16)items[ids[3]].item_id], 1, 0);
        } else {
            Hint_CancelCurrent();
        }
        ypush = SubNormCharPush;
    } else if (picked == 1) {
        items = CharItems;
        ids = CharShelfIds;
        positions = CharCurPos;
        count = &SHOPCHARCOUNT;
        ypush = SubNormCharPush;
    } else if (picked == 2) {
        items = ExtraItems;
        ids = ExtraShelfIds;
        positions = ExtraCurPos;
        count = &SHOPEXTRACOUNT;
        ypush = SubNormCharPush;
    } else if (picked == 4) {
        items = BrickItems;
        ids = BrickShelfIds;
        positions = BrickCurPos;
        count = &SHOPGOLDBRICKS;
        ypush = SubNormCharPush;
    } else if (picked == 5) {
        items = CutItems;
        ids = CutShelfIds;
        positions = CutCurPos;
        count = &SHOPCUTCOUNT;
        ypush = SubNormCharPush;
    }
    if (ShopSlideTimerPc >= 0.0f) {
        ShopSlideTimerPc -= FRAMETIME;
        if (ShopSlideTimerPc < 0.0f)
            ShopNameAlpha = 0.0f;
    }
    if (ShopEaseSubInPc != 0) {
        phase = (f32)(1.0 - (f64)ShopSlideTimerPc * 8.0);
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        factor = factor < 1.0f ? factor : 1.0f;
        factor = 0.0f > factor ? 0.0f : factor;
        if (ShopEaseSubInPc == 1)
            ShopInOutScalePc = 0.0f + factor;
        else if (ShopEaseSubInPc == -1)
            ShopInOutScalePc = (f32)(1.0 - (f64)factor);
    }
    if (ShopMoveItemsPc != 0) {
        if (ShopSubMoveSfxLockPc == 0) {
            ShopSubMoveSfxLockPc = 1;
            GameAudio_PlaySfx(0x2f, positions, 0, 0);
        }
        moved = 0;
        if (ShopMoveItemsPc == 1)
            moved = MoveSubItemsLeft(ids, positions, *count);
        else if (ShopMoveItemsPc == 2)
            moved = MoveSubItemsRight(ids, positions, *count);
        if (moved != 0 && items != NULL) {
            selected = &items[ids[3]];
            if (selected->type == 0 && selected->unlocked == 1)
                SelectSubItem();
        }
    } else {
        ShopSubMoveSfxLockPc = 0;
    }
    if (ShopSlideTimerPc < 0.0f) {
        ShopEaseSubInPc = 0;
        if (input.left_held != 0)
            left = 1;
        else if (input.right_held != 0)
            right = 1;
        else if (input.confirm != 0 && ShopExitPc == 0) {
            SelectSubItem();
            goto finish;
        } else if (input.cancel != 0)
            cancel = 1;
    }
    if (ShopExitPc == 0) {
        if (left != 0 || right != 0) {
            ShopScaleOverridePc[0] = left != 0 ? 0.0f : 1.0f;
            ShopScaleOverridePc[1] = ShopScaleOverridePc[5] = 0.4f;
            ShopScaleOverridePc[2] = ShopScaleOverridePc[4] = 0.8f;
            ShopScaleOverridePc[6] = left != 0 ? 1.0f : 0.0f;
            position = SubShelfPos[left != 0 ? 0 : 6];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            position = SubShelfPos[left != 0 ? 6 : 0];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            ShopMoveItemsPc = left != 0 ? 1 : 2;
            ShopSlideTimerPc = 0.125f;
            ShopHintDrawWaitPc = 0.2f;
            scrolling = 1;
        }
        if (cancel != 0) {
            ShopSlideTimerPc = 0.125f;
            ShopEaseSubInPc = -1;
            ShopExitPc = 1;
            GameAudio_PlaySfx(0x31, NULL, 0, 0);
        }
    }
finish:
    if (ShopExitPc != 0 && ShopSlideTimerPc < 0.0f) {
        ShopSlideTimerPc = 0.125f;
        ShopExitPc = 0;
        ShopEaseSubInPc = -1;
        ShopLastItemPc = -1;
        GameCam_Blend(GameCam, 0.6f, 0.0f, 1);
        ShopNameAlpha = 0.0f;
        return 6;
    }
    ShopScrollKeyHitPc = scrolling;
    return 0;
}

i32 __cdecl AddToCollection(i32 id);
void __cdecl AddToCompletionPoints(u32 points);
extern f32 ShopPurchasePulsePc;
extern i32 HubForceShopSavePc;
i32 __cdecl BuyShopItem(shopitem_s *items, i32 index, i32 charge);
i32 __cdecl CutScenePlayer_CanStart(i32 index);
void __cdecl CutScenePlayer_Start(i32 index);

/* 004c1050..004c1077: original same-TU EAX helper includes the vehicle exception. */
static i32 Shop_CharacterUnlockedPc(i32 character_id) {
    if (character_id == VehicleBoltId7f17d4Pc)
        return ((SHOP_SAVE_VIEW_PC *)&Game)->bolt_unlocked != 0;
    return CollectIDUnlocked(character_id) != 0;
}

/* 004c2730..004c2ae8 instructions; 004c2ae8..004c2b00 six-entry switch table. */
i32 __cdecl SelectShopItem(shopitem_s *items, i32 index) {
    shopitem_s *item = &items[index];
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    HINT_PC *hint;
    i32 id, area, i;
    switch (item->type) {
        case 0:
            id = (u16)item->item_id;
            if (item->unlocked == 1 || item->price == 0)
                return 1;
            if (Game.coins < (u32)item->price)
                goto no_cash;
            AddToCompletionPoints(POINTS_PER_HINT);
            hint = Hint_FindHint(ShopHintIdsPc[id]);
            if (hint != NULL)
                hint->flags |= 2;
            BuyShopItem(items, index, 1);
            return 1;
        case 1:
            if (item->unlocked == 1)
                return 0;
            id = (u16)item->item_id;
            if (id < 0 || id >= CHARCOUNT)
                return 0;
            if (Shop_CharacterUnlockedPc(id) == 0)
                goto locked;
            if (Game.coins < (u32)item->price)
                goto no_cash;
            if (AddToCollection(id) != 0) {
                AddToCompletionPoints(POINTS_PER_CHARACTER);
                BuyShopItem(items, index, 1);
            }
            return 1;
        case 2:
            id = (u16)item->item_id;
            if (item->unlocked == 1 || (save->extra_purchased_bits[id / 32] & (1u << (id & 31))) != 0)
                return 0;
            area = ExtrasCheatsPc[id].area_index;
            if (area != -1 && Game.area_save[area].red_brick_collected == 0)
                goto locked;
            if (Game.coins < (u32)item->price)
                goto no_cash;
            save->extra_purchased_bits[id / 32] |= 1u << (id & 31);
            AddToCompletionPoints(POINTS_PER_CHEAT);
            BuyShopItem(items, index, 1);
            return 1;
        case 4:
            if (item->unlocked == 1 || item->price == 0)
                return 0;
            if ((f64)(i32)((u32)index * 3600u) > (f64)Game.game_time)
                goto locked;
            if (Game.coins < (u32)item->price)
                goto no_cash;
            AddToCompletionPoints(POINTS_PER_GOLDBRICK);
            AddToGoldBricks();
            BuyShopItem(items, index, 1);
            return 1;
        case 5:
            if (CutScenePlayer_CanStart(index) == 0) {
                PlaySfx((char *)GameAudioName0075c354Pc, NULL);
                ShopLockedScalePc = 1.5f;
                return 0;
            }
            ShopOldMenuFnPc = menuptr;
            ShopOldPickedScalePc = topscale[5];
            ShopOldPickedPushPc = toppush[5];
            for (i = 0; i < 3; ++i) {
                ShopOldDrawParentPc[i] = ShopDrawParentPc[i];
                ShopOldMenuParentPc[i] = ShopMenuParentPc[i];
            }
            ShopOldDrawPanelFnPc = ShopDrawPanelFnPc;
            ShopOldDrawFnPc = drawptr;
            ShopOldMenuLevelPc = ShopMenuLevelPc;
            ShopOldDrawLevelPc = ShopDrawLevelPc;
            oldpicked = picked;
            menuptr = NULL;
            for (i = 0; i < 3; ++i) {
                ShopDrawParentPc[i] = NULL;
                ShopMenuParentPc[i] = NULL;
            }
            ShopMenuLevelPc = 0;
            ShopDrawLevelPc = 0;
            SHOPACTIVE = 0;
            BuyShopItem(items, index, 0);
            ShopPurchasePulsePc = 0.0f;
            TempGame.game_time = Game.game_time;
            if (memcmp(&TempGame, &Game, sizeof(Game)) != 0)
                HubForceShopSavePc = 1;
            CutScenePlayer_Start(index);
            return 1;
        default:
            return -1;
    }
no_cash:
    CoinTotalScale = 1.5f;
    return -1;
locked:
    GameAudio_PlaySfx(0x32, NULL, 0, 0);
    ShopLockedScalePc = 1.5f;
    return 0;
}

extern f32 ShopPurchasePulseDurationPc;

/* 004c2660..004c2725. Preserve the PC purchase pulse owner separately from the
 * existing shop initialization value at 0087bbd0. */
i32 __cdecl BuyShopItem(shopitem_s *items, i32 index, i32 charge) {
    shopitem_s *item = &items[index];
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    u32 *bits = NULL;
    if (item->type == 0)
        bits = save->hint_purchased_bits;
    else if (item->type == 1)
        bits = save->character_purchased_bits;
    else if (item->type == 2)
        bits = save->extra_unlocked_bits;
    else if (item->type == 4)
        bits = save->gold_brick_purchased_bits;
    else if (item->type == 5) {
        PlaySfx((char *)GameAudioName0075c36cPc, NULL);
        return 1;
    }
    if (bits != NULL)
        bits[index / 32] |= (u32)((u64)1 << (index % 32));
    if (charge != 0)
        Game.coins -= item->price;
    ShopPurchasePulsePc = ShopPurchasePulseDurationPc;
    item->unlocked = 1;
    if (charge != 0)
        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
    return 1;
}

/* The retail move code is written out per shelf slot: any compiler-unrolled
 * loop in the function makes VC8 schedule the x87 operands differently. */
#define SHOP_RESET_SHELF_POSITIONS()                                                                                   \
    positions[0] = SubShelfPos[0];                                                                                     \
    positions[1] = SubShelfPos[1];                                                                                     \
    positions[2] = SubShelfPos[2];                                                                                     \
    positions[3] = SubShelfPos[3];                                                                                     \
    positions[4] = SubShelfPos[4];                                                                                     \
    positions[5] = SubShelfPos[5];                                                                                     \
    positions[6] = SubShelfPos[6];
#define SHOP_SLIDE_SLOT(i, j)                                                                                          \
    t = SHOP_CLAMP01(t);                                                                                               \
    positions[i].x = SubShelfPos[i].x + (SubShelfPos[j].x - SubShelfPos[i].x) * t;                                     \
    positions[i].y = SubShelfPos[i].y + (SubShelfPos[j].y - SubShelfPos[i].y) * t;                                     \
    positions[i].z = SubShelfPos[i].z + (SubShelfPos[j].z - SubShelfPos[i].z) * t;

/* 004c2b00..004c31fd. Right moves IDs toward index zero, interpolates slots
 * 1..6 toward their previous shelf slot, and uses the PC scale2 owner. */
i32 __cdecl MoveSubItemsRight(i32 *ids, NUVEC *positions, i32 count) {
    i32 angle;
    f32 phase, reverse_phase, t;
    if (ShopSlideTimerPc <= 0.0f) {
        ids[0] = ids[1];
        ids[1] = ids[2];
        ids[2] = ids[3];
        ids[3] = ids[4];
        ids[4] = ids[5];
        ids[5] = ids[6];
        ++ids[6];
        if (ids[6] >= count)
            ids[6] = 0;
        ShopSlideTimerPc = 0.0f;
        SHOP_RESET_SHELF_POSITIONS()
        scale4 = scalenorm;
        scale3 = scalepicked;
        scale2 = scalenorm;
        subpush[0] = SubNormCharPush;
        subpush[1] = SubBigCharPush;
        subpush[2] = SubNormCharPush;
        ShopMoveItemsPc = 0;
        ShopScaleOverridePc[0] = 0.0f;
        ShopScaleOverridePc[1] = 0.4f;
        ShopScaleOverridePc[2] = 0.8f;
        ShopScaleOverridePc[4] = 0.8f;
        ShopScaleOverridePc[5] = 0.4f;
        ShopScaleOverridePc[6] = 0.0f;
        return 1;
    }
    t = 1.0f - ShopSlideTimerPc * 8.0f;
    phase = t;
    reverse_phase = 1.0f - t;
    angle = (i32)(t * 32768.0f + 16384.0f);
    t = 1.0f - (NuTrigTable[(angle >> 1) & 0x7fff] + 1.0f) * 0.5f;
    SHOP_SLIDE_SLOT(1, 0)
    SHOP_SLIDE_SLOT(2, 1)
    SHOP_SLIDE_SLOT(3, 2)
    SHOP_SLIDE_SLOT(4, 3)
    SHOP_SLIDE_SLOT(5, 4)
    SHOP_SLIDE_SLOT(6, 5)
    t = SHOP_CLAMP01(t);
    scale3 = scalepicked + (scalenorm - scalepicked) * t;
    t = SHOP_CLAMP01(t);
    scale2 = scalenorm + (scalepicked - scalenorm) * t;
    t = SHOP_CLAMP01(t);
    subpush[1] = SubBigCharPush + (SubNormCharPush - SubBigCharPush) * t;
    t = SHOP_CLAMP01(t);
    subpush[2] = SubNormCharPush + (SubBigCharPush - SubNormCharPush) * t;
    ShopScaleOverridePc[0] = reverse_phase;
    ShopScaleOverridePc[6] = phase;
    return 0;
}

/* 004c3200..004c38fc. Preserve the descending PC interpolation order and
 * the distinct scale4 owner, including when positions aliases SubShelfPos. */
i32 __cdecl MoveSubItemsLeft(i32 *ids, NUVEC *positions, i32 count) {
    i32 angle;
    f32 phase, reverse_phase, t;
    if (ShopSlideTimerPc <= 0.0f) {
        ids[6] = ids[5];
        ids[5] = ids[4];
        ids[4] = ids[3];
        ids[3] = ids[2];
        ids[2] = ids[1];
        ids[1] = ids[0];
        --ids[0];
        if (ids[0] < 0)
            ids[0] = count - 1;
        ShopSlideTimerPc = 0.0f;
        SHOP_RESET_SHELF_POSITIONS()
        scale4 = scalenorm;
        scale3 = scalepicked;
        scale2 = scalenorm;
        subpush[0] = SubNormCharPush;
        subpush[1] = SubBigCharPush;
        subpush[2] = SubNormCharPush;
        ShopMoveItemsPc = 0;
        ShopScaleOverridePc[0] = 0.0f;
        ShopScaleOverridePc[1] = 0.4f;
        ShopScaleOverridePc[2] = 0.8f;
        ShopScaleOverridePc[4] = 0.8f;
        ShopScaleOverridePc[5] = 0.4f;
        ShopScaleOverridePc[6] = 0.0f;
        return 1;
    }
    t = 1.0f - ShopSlideTimerPc * 8.0f;
    phase = t;
    reverse_phase = 1.0f - t;
    angle = (i32)(t * 32768.0f + 16384.0f);
    t = 1.0f - (NuTrigTable[(angle >> 1) & 0x7fff] + 1.0f) * 0.5f;
    SHOP_SLIDE_SLOT(5, 6)
    SHOP_SLIDE_SLOT(4, 5)
    SHOP_SLIDE_SLOT(3, 4)
    SHOP_SLIDE_SLOT(2, 3)
    SHOP_SLIDE_SLOT(1, 2)
    SHOP_SLIDE_SLOT(0, 1)
    t = SHOP_CLAMP01(t);
    scale4 = scalenorm + (scalepicked - scalenorm) * t;
    t = SHOP_CLAMP01(t);
    scale3 = scalepicked + (scalenorm - scalepicked) * t;
    t = SHOP_CLAMP01(t);
    subpush[0] = SubNormCharPush + (SubBigCharPush - SubNormCharPush) * t;
    t = SHOP_CLAMP01(t);
    subpush[1] = SubBigCharPush + (SubNormCharPush - SubBigCharPush) * t;
    ShopScaleOverridePc[0] = phase;
    ShopScaleOverridePc[6] = reverse_phase;
    return 0;
}

extern f32 ShopCheatTimerPc;
extern const f64 ShopCheatBlinkCutoffPc;
void __cdecl DrawSubItems(void);

/* 004c6400..004c64e9. The PC top-level label follows all six native shelves. */
void __cdecl DrawItemMenu2D(void) {
    i32 text_id;
    if (ShopNameAlpha > 0.0f) {
        if (picked == 0)
            text_id = TextIdPc880904;
        else if (picked == 1)
            text_id = TextIdPc88070c;
        else if (picked == 2)
            text_id = TextIdPc88090c;
        else if (picked == 4)
            text_id = TextIdPc88077c;
        else if (picked == 5)
            text_id = TextIdPc880800;
        else
            text_id = TextIdPc880908;
        SmartTextEx(TTab[text_id], 0.0f, (f32)(((f64)HubEpisodeTitleYPc + HubEpisodeSubtitleYPc) * 0.5), 1.0f, 0.8f,
                    0.8f, 0.8f, 0, 255, 255, 255, 1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
    }
}

/* 004c4f40..004c501f. ShopCheatNamePc is a text pointer at00880008; the
 * separate integer code column is at0087c564. */
void __cdecl DrawCodeMenu(void) {
    if (ShopCheatTimerPc > 0.0f) {
        if (ShopCheatNamePc != NULL && (f64)ShopCheatTimerPc < ShopCheatBlinkCutoffPc &&
            NuFmod(ShopCheatTimerPc, 0.35f) < 0.175f) {
            SmartTextEx(ShopCheatNamePc, 0.0f, (f32)(((f64)HubEpisodeTitleYPc + HubEpisodeSubtitleYPc) * 0.5), 1.0f,
                        0.8f, 0.8f, 0.8f, 0, 0, 255, 0, 1.7f, 1, NULL, 0, 128);
        }
        ShopCheatTimerPc -= FRAMETIME;
    }
}

/* 004c6c70..004c6dd8 instructions;004c6dd8..004c6df0 six-entry table.
 * This native predicate was previously misidentified as DrawShopPrompts. */
static i32 Shop_CanSelectPc(void) {
    shopitem_s *item;
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    i32 id, area, index;
    switch (picked) {
        case 0:
            item = &HintItems[HintShelfIds[3]];
            return Game.coins >= (u32)item->price && item->unlocked != 1;
        case 1:
            item = &CharItems[CharShelfIds[3]];
            return Game.coins >= (u32)item->price && Shop_CharacterUnlockedPc((u16)item->item_id) != 0 &&
                   item->unlocked != 1;
        case 2:
            item = &ExtraItems[ExtraShelfIds[3]];
            if (Game.coins < (u32)item->price)
                return 0;
            id = (u16)item->item_id;
            if (id >= 8) {
                area = ExtrasCheatsPc[id].area_index;
                if (area != -1 && Game.area_save[area].red_brick_collected == 0)
                    return 0;
            }
            return (save->extra_purchased_bits[id / 32] & (1u << (id & 31))) == 0 && item->unlocked != 1;
        case 4:
            index = BrickShelfIds[3];
            if (Game.coins < (u32)BrickItems[index].price)
                return 0;
            if ((f64)(i32)((u32)index * 3600u) > (f64)Game.game_time)
                return 0;
            return 1;
        case 5:
            return CutScenePlayer_CanStart(CutShelfIds[3]) != 0;
        default:
            return 0;
    }
}

/* 004c7750..004c7819. Keep the affordability predicate separate from actual
 * prompt drawing and preserve both players' native prompt arguments. */
void __cdecl DrawShopPrompts(void) {
    i32 select = -1, back = -1;
    f32 alpha = 1.0f;
    if (menuptr == ItemMenu) {
        back = TextIdPc880844;
        select = TextIdPc880888;
    } else if (menuptr == SubItemMenu) {
        select = picked == 5 ? TextIdPc8808c0 : TextIdPc8808c4;
        if (Shop_CanSelectPc() == 0)
            alpha = 0.25f;
        back = tBACK;
    } else if (menuptr == CodeMenu) {
        back = tBACK;
        select = TextIdPc880888;
    }
    if (netclient != 0)
        select = -1;
    if (select != -1 || back != -1)
        DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], select, alpha, -1, back, -1, TextIdPc880eac,
                              MissionMenuPacketPc.active_player[1], select, alpha, -1, back, -1, TextIdPc880eac);
}

/* 004c6c10..004c6c15: retain the real native target, currently unresolved. */
void __cdecl DrawSubItemMenu3D(void) {
    DrawSubItems();
}

extern f32 ShopCodePulseScalePc, ShopCodeArrowBingPc, ShopCodeArrowAmplitudePc;

/* 004c5650..004c58dd. PC renders exactly six code slots, uses codeshelfscale,
 * and updates the original arrow phase owner without Android's extra slot. */
void __cdecl DrawCodeMenu3D(void) {
    i32 letters[6];
    i32 i, angle;
    u16 rotation = shelfang;
    f32 bing = 0.0f, scale, push;
    for (i = 0; i < 6; ++i) {
        if (ShopUserCodePc[i] >= 'A' && ShopUserCodePc[i] <= 'Z')
            letters[i] = ShopUserCodePc[i] - 'A';
        else if (ShopUserCodePc[i] >= '0' && ShopUserCodePc[i] <= '9')
            letters[i] = ShopUserCodePc[i] - '0' + 26;
    }
    if (ShopPurchasePulsePc > 0.0f) {
        bing = ShopPurchasePulsePc / ShopPurchasePulseDurationPc;
        bing = (f32)((f64)bing * 0.5);
        ShopPurchasePulsePc -= FRAMETIME;
    } else {
        ShopPurchasePulsePc = 0.0f;
    }
    for (i = 0; i < 6; ++i) {
        if (ShopColumnPc == i) {
            scale = codeshelfscale[i] * ShopNativeScalePc[3] * ShopInOutScalePc + bing * ShopCodePulseScalePc;
            push = subpush[1];
        } else if (ShopLastItemPc == i) {
            scale = codeshelfscale[i] * ShopNativeScalePc[3] * ShopInOutScalePc + bing * ShopCodePulseScalePc;
            push = subpush[2];
        } else {
            scale = codeshelfscale[i] * ShopNativeScalePc[3] * ShopInOutScalePc;
            push = SubNormCharPush;
        }
        DrawItem(&codeblank, &CodePos[i], scale, 1.0f, push, 0, rotation, 0);
        DrawItem(&atoz0to9icon[letters[i]], &CodePos[i], scale, 1.0f, push, 0, rotation, 0);
    }
    angle = (i32)((f64)ShopInOutScalePc * 32768.0);
    ShopCodeArrowBingPc = NuTrigTable[(angle >> 1) & 0x7fff] * ShopCodeArrowAmplitudePc;
}

extern i32 ShopBrickRotationDegreesPc;
extern f32 ShopCharacterPeriodPc;
extern const f64 ShopCharacterAmplitudePc;
extern AREA_DATA_PC *ANEWHOPE_ADATA, *ANAKINSFLIGHT_ADATA;
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(LEVELDATA_PC, episode) == 0xa2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);

/* 004c58e0..004c63fc, including two six-entry native switch tables. */
void __cdecl DrawSubItems(void) {
    f32 bing = 0.0f;
    f32 bing_scale, item_scale, normal_push, scale, ypush;
    shopitem_s *items, *item;
    NUVEC *positions;
    i32 *shelf_ids;
    i32 slot, phase_angle, id, area, episode;
    u16 rotation;
    i32 angle;
    nuhspecial_s *blank, *film;
    LEVELDATA_PC *level;
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    if (ShopPurchasePulsePc > 0.0f) {
        bing = ShopPurchasePulsePc / ShopPurchasePulseDurationPc;
        ShopPurchasePulsePc -= FRAMETIME;
    } else {
        ShopPurchasePulsePc = 0.0f;
    }
    phase_angle = (i32)((f64)ShopPurchasePulsePc * 32768.0 + 16384.0);
    bing_scale = (f32)(1.0 - ((f64)NuTrigTable[(phase_angle >> 1) & 0x7fff] + 1.0) * 0.5);
    switch (picked) {
        case 0:
            items = HintItems;
            shelf_ids = HintShelfIds;
            positions = HintCurPos;
            item_scale = ShopNativeScalePc[0];
            break;
        case 1:
            items = CharItems;
            shelf_ids = CharShelfIds;
            positions = CharCurPos;
            item_scale = ShopNativeScalePc[2];
            break;
        case 2:
            items = ExtraItems;
            shelf_ids = ExtraShelfIds;
            positions = ExtraCurPos;
            item_scale = ShopNativeScalePc[1];
            break;
        case 4:
            items = BrickItems;
            shelf_ids = BrickShelfIds;
            positions = BrickCurPos;
            item_scale = ShopNativeScalePc[4];
            break;
        case 5:
            items = CutItems;
            shelf_ids = CutShelfIds;
            positions = CutCurPos;
            item_scale = ShopNativeScalePc[5];
            break;
        default:
            return;
    }
    normal_push = SubNormCharPush;
    for (slot = 0; slot < 7; ++slot) {
        if ((ShopMoveItemsPc == 1 && slot == 6) || (ShopMoveItemsPc == 2 && slot == 0))
            continue;
        rotation = 0;
        if (slot == 0) {
            scale = item_scale * ShopInOutScalePc * ShopScaleOverridePc[0];
            ypush = normal_push * ShopScaleOverridePc[0];
        } else if (slot == 6) {
            scale = item_scale * ShopInOutScalePc * ShopScaleOverridePc[6];
            ypush = normal_push * ShopScaleOverridePc[6];
        } else if (ShopMoveItemsPc == 1 && slot == 5) {
            scale = item_scale * ShopInOutScalePc * ShopScaleOverridePc[6];
            ypush = normal_push * ShopScaleOverridePc[6];
        } else if (ShopMoveItemsPc == 2 && slot == 1) {
            scale = item_scale * ShopInOutScalePc * ShopScaleOverridePc[0];
            ypush = normal_push * ShopScaleOverridePc[0];
        } else if (slot == 2) {
            scale = item_scale * scale4 * ShopInOutScalePc;
            ypush = subpush[0];
        } else if (slot == 3) {
            if (ShopPurchasePulsePc >= 0.0f)
                rotation = (u16)(i32)((f64)(-bing) * 65536.0);
            scale = item_scale * scale3 * ShopInOutScalePc + ShopCodePulseScalePc * bing_scale;
            ypush = subpush[1];
        } else if (slot == 4) {
            scale = item_scale * scale2 * ShopInOutScalePc;
            ypush = subpush[2];
        } else {
            scale = item_scale * ShopInOutScalePc;
            ypush = normal_push;
        }
        item = &items[shelf_ids[slot]];
        switch (picked) {
            case 0:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulsePc > 0.0f) {
                        if (slot == 3)
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        else
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    } else {
                        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    }
                }
                angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                if (items[shelf_ids[slot]].unlocked == 1) {
                    DrawItem(&TopShelf[0].special, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                } else {
                    DrawItem(&infoblank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    DrawItem(&TopShelf[3].special, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                }
                break;
            case 1:
                rotation =
                    (u16)(i32)((f64)
                                   NuTrigTable[(((u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, ShopCharacterPeriodPc) /
                                                            (f64)ShopCharacterPeriodPc * 65536.0) +
                                                 0x4000) >>
                                                1) &
                                               0x7fff] *
                               ShopCharacterAmplitudePc);
                if (item->unlocked != 1)
                    rotation = 0;
                angle = shelfang + rotation;
                Shop_DrawCharacter(&items[shelf_ids[slot]], &positions[slot], scale, ypush, 0, angle, 0);
                break;
            case 2:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulsePc > 0.0f) {
                        if (slot == 3)
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        else
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    } else {
                        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    }
                    angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                    DrawItem(&toolblank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    Shop_DrawItemPc(&items[shelf_ids[slot]], &positions[slot], scale, 1.0f, 1, ypush, 0, angle, 0);
                } else {
                    angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                    DrawItem(&toolblank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    item = &items[shelf_ids[slot]];
                    id = (u16)item->item_id;
                    if ((save->extra_purchased_bits[id / 32] & (1u << (id & 31))) != 0 || id < 8 ||
                        ExtrasCheatsPc[id].area_index == -1 ||
                        Game.area_save[ExtrasCheatsPc[id].area_index].red_brick_collected != 0) {
                        Shop_DrawItemPc(item, &positions[slot], scale, 1.0f, 1, ypush, 0, angle, 0);
                    } else if (NuSpecialExistsFn(&extrasils[shelf_ids[slot]]) != 0) {
                        DrawItem(&extrasils[shelf_ids[slot]], &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    }
                }
                break;
            case 4:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulsePc > 0.0f) {
                        if (slot == 3)
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        else
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    } else {
                        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    }
                    rotation = (u16)(rotation + (u16)(items[shelf_ids[slot]].item_id * 0x1249) +
                                     (i32)((u32)ShopBrickRotationDegreesPc << 16) / 360);
                }
                angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                Shop_DrawBrickPc(&items[shelf_ids[slot]], &positions[slot], scale, ypush, 0, angle, 0);
                break;
            case 5:
                blank = &cutblank;
                film = &cutfilm_locked;
                id = (u16)item->item_id;
                if (shopcutsceneplayer != NULL && CutScenePlayer_CanStart(id) != 0) {
                    blank = &toolblank;
                    level = &LDataList[((CUT_PLAYER_CLIP_CONFIG_PC *)shopcutsceneplayer->clips)[id].level_id];
                    area = level->area_index;
                    episode = (i8)level->episode;
                    if (episode != -1) {
                        if (episode & 1)
                            blank = &codeblank;
                    } else if (area != -1) {
                        if (ANEWHOPE_ADATA != NULL && area == ANEWHOPE_ADATA->index)
                            blank = &codeblank;
                        else if (ANAKINSFLIGHT_ADATA != NULL && area == ANAKINSFLIGHT_ADATA->index)
                            blank = &toolblank;
                    }
                    rotation =
                        (u16)(id * 0x1555 - (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * -65536.0));
                    film = &cutfilm_unlocked;
                }
                angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                DrawItem(blank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                DrawItem(film, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                break;
        }
    }
}

extern i32 ShopCodeCharacterPc, ShopCodeValidPc, ShopCodeMoveSfxLockPc, ShopCodeItemChangedPc;
extern f32 ShopCodeRepeatTimerPc, ShopCodeRepeatIntervalPc, ShopCodeExitPushPc;
extern const f32 ShopCodeCheatDisplayTimePc;
extern char ShopCodeCharactersPc[37];

/* 004c41a0..004c4f38. Same TU as the native EAX-argument input helper. */
i32 __cdecl CodeMenu(void *menu) {
    i32 down = 0, up = 0, left = 0, right = 0, confirm = 0, cancel = 0;
    SHOP_INPUT_PC input;
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    f32 factor, phase;
    i32 i, angle;
    i8 area;
    char code[7];
    u32 crc;
    (void)menu;
    Shop_GetInputPc(&input);
    if (ShopSlideTimerPc >= 0.0f)
        ShopSlideTimerPc -= FRAMETIME;
    if (ShopCodeRepeatTimerPc >= 0.0f)
        ShopCodeRepeatTimerPc -= FRAMETIME;
    ShopCodeItemChangedPc = 0;
    if (ShopEaseSubInPc) {
        phase = (f32)(1.0 - (f64)ShopSlideTimerPc * 8.0);
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopEaseSubInPc == 1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScalePc = SHOP_LERP(0.0f, 1.0f, factor);
        } else if (ShopEaseSubInPc == -1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScalePc = SHOP_LERP(1.0f, 0.0f, factor);
        }
    }
    if (ShopSlideTimerPc <= 0.0f) {
        ShopCodeValidPc = 0;
        ShopEaseSubInPc = 0;
        if (input.left_held)
            left = 1;
        else if (input.right_held)
            right = 1;
        else if (input.down_held) {
            if (ShopCodeRepeatTimerPc <= 0.0f)
                down = 1;
            else if (input.down_pressed)
                down = 1;
        } else if (input.up_held) {
            if (ShopCodeRepeatTimerPc <= 0.0f)
                up = 1;
            else if (input.up_pressed)
                up = 1;
        }
        if (input.confirm)
            confirm = 1;
        else if (input.cancel)
            cancel = 1;
        if (down) {
            ShopCodeRepeatTimerPc = ShopCodeRepeatIntervalPc;
            --ShopCodeCharacterPc;
            if (ShopCodeCharacterPc < 0)
                ShopCodeCharacterPc = 35;
            ShopCodeValidPc = 0;
            ShopUserCodePc[ShopColumnPc] = ShopCodeCharactersPc[ShopCodeCharacterPc];
            ShopCodeMoveSfxLockPc = 1;
            GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
        }
        if (up) {
            ShopCodeRepeatTimerPc = ShopCodeRepeatIntervalPc;
            ++ShopCodeCharacterPc;
            if (ShopCodeCharacterPc >= 36)
                ShopCodeCharacterPc = 0;
            ShopCodeValidPc = 0;
            ShopUserCodePc[ShopColumnPc] = ShopCodeCharactersPc[ShopCodeCharacterPc];
            ShopCodeMoveSfxLockPc = 1;
            GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
        }
        if (left) {
            ShopLastItemPc = ShopColumnPc;
            --ShopColumnPc;
            if (ShopColumnPc < 0)
                ShopColumnPc = 0;
            ShopCodeValidPc = 0;
            if (!ShopUserCodePc[ShopColumnPc])
                ShopCodeCharacterPc = 0;
            else
                for (i = 0; i < 36; ++i) {
                    if (ShopUserCodePc[ShopColumnPc] == ShopCodeCharactersPc[i]) {
                        ShopCodeCharacterPc = i;
                        break;
                    }
                }
            if (ShopLastItemPc != ShopColumnPc) {
                ShopSlideTimerPc = 0.125f;
                if (!ShopCodeMoveSfxLockPc) {
                    ShopCodeMoveSfxLockPc = 1;
                    GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
                }
            }
        }
        if (right) {
            ShopLastItemPc = ShopColumnPc;
            ++ShopColumnPc;
            if (ShopColumnPc >= 6)
                ShopColumnPc = 5;
            ShopCodeValidPc = 0;
            if (!ShopUserCodePc[ShopColumnPc])
                ShopCodeCharacterPc = 0;
            else
                for (i = 0; i < 36; ++i) {
                    if (ShopUserCodePc[ShopColumnPc] == ShopCodeCharactersPc[i]) {
                        ShopCodeCharacterPc = i;
                        break;
                    }
                }
            if (ShopLastItemPc != ShopColumnPc) {
                ShopSlideTimerPc = 0.125f;
                if (!ShopCodeMoveSfxLockPc) {
                    ShopCodeMoveSfxLockPc = 1;
                    GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
                }
            }
        }
        if (cancel) {
            ShopSlideTimerPc = 0.125f;
            ShopCodeValidPc = 0;
            ShopCodeCharacterPc = 0;
            ShopEaseSubInPc = -1;
            ShopExitPc = 1;
            GameAudio_PlaySfx(0x31, NULL, 0, 0);
        }
        if (confirm) {
            ShopUserCodePc[ShopColumnPc] = ShopCodeCharactersPc[ShopCodeCharacterPc];
            for (i = 0; i < 6; ++i)
                code[i] = ShopUserCodePc[i];
            code[6] = 0;
            crc = SfxNameHash(code);
            for (i = 0; i < 144; ++i) {
                if (codelist[i] && crc == codelist[i]) {
                    ShopCodeValidPc = 1;
                    break;
                }
            }
            if (i >= 144) {
                PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                ShopCheatNamePc = NULL;
            } else {
                if (i < charcheatix) {
                    if (Game_CharacterSave &&
                        !(Game_CharacterSave[(u16)CharItems[i].item_id] & SAVE_CHARACTER_UNLOCKED)) {
                        Game_CharacterSave[(u16)CharItems[i].item_id] |= SAVE_CHARACTER_UNLOCKED;
                        AddToCompletionPoints(POINTS_PER_CHARACTER);
                        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
                    } else
                        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                    ShopCheatNamePc = TTab[CDataList[(u16)CharItems[i].item_id].name_id];
                } else if (i < extracheatix) {
                    i -= charcheatix;
                    if (!(save->extra_purchased_bits[i / 32] & (1U << (i & 31)))) {
                        save->extra_purchased_bits[i / 32] |= 1U << (i & 31);
                        ExtraItems[i].unlocked = 1;
                        AddToCompletionPoints(POINTS_PER_CHEAT);
                        area = ExtrasCheatsPc[i].area_index;
                        if (area != -1 && !Game.area_save[area].red_brick_collected) {
                            Game.area_save[area].red_brick_collected = 1;
                            AddToCompletionPoints(POINTS_PER_REDBRICK);
                        }
                        save->extra_unlocked_bits[i / 32] |= (u32)(1ULL << (i % 32));
                        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
                    } else
                        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                    ShopCheatNamePc = TTab[ExtrasCheatsPc[i].text_id ? *ExtrasCheatsPc[i].text_id : tUNKNOWN];
                }
                ShopPurchasePulsePc = ShopPurchasePulseDurationPc;
                ShopCheatTimerPc = ShopCodeCheatDisplayTimePc;
            }
        }
    } else
        ShopCodeMoveSfxLockPc = 0;

    if (ShopSlideTimerPc >= 0.0f) {
        phase = (f32)(1.0 - (f32)((f64)ShopSlideTimerPc * 8.0));
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopLastItemPc != -1) {
            if (ShopEaseSubInPc == 1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItemPc] =
                    SHOP_LERP(codemenuscale[ShopLastItemPc], codebigscale[ShopLastItemPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            } else if (ShopEaseSubInPc == -1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItemPc] =
                    SHOP_LERP(codebigscale[ShopLastItemPc], codemenuscale[ShopLastItemPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            } else {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItemPc] =
                    SHOP_LERP(codebigscale[ShopLastItemPc], codemenuscale[ShopLastItemPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            }
        }
        if (ShopColumnPc != -1) {
            if (ShopEaseSubInPc == 1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumnPc] =
                    SHOP_LERP(codemenuscale[ShopColumnPc], codebigscale[ShopColumnPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            } else if (ShopEaseSubInPc == -1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumnPc] =
                    SHOP_LERP(codebigscale[ShopColumnPc], codemenuscale[ShopColumnPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            } else {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumnPc] =
                    SHOP_LERP(codemenuscale[ShopColumnPc], codebigscale[ShopColumnPc], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            }
        }
    } else
        ShopLastItemPc = -1;
    if (ShopExitPc && ShopSlideTimerPc < 0.0f) {
        ShopExitPc = 0;
        ShopEaseSubInPc = -1;
        ShopLastItemPc = -1;
        subpush[1] = ShopCodeExitPushPc;
        ShopSlideTimerPc = 0.125f;
        ShopNameAlpha = 0.0f;
        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        return 6;
    }
    return 7;
}

extern i16 TextIdPc880708, TextIdPc88074c, TextIdPc880778;
void __cdecl CutScenePlayer_GetText(i32 id, char *title, char *subtitle, i32 mode);
void __cdecl CoinTotal_Draw(i32, f32, f32, i32, f32, i32, i32, i32);
STATIC_ASSERT(offsetof(GAMESAVE, game_time) == 0x7c20);
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
STATIC_ASSERT(sizeof(AREA_SAVE_PICKUP_PC) == 0xc);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, red_brick_collected) == 6);

/* 004c64f0..004c6bec instructions;004c6bec..004c6c04 six-entry table. */
void __cdecl DrawSubItemMenu2D(void) {
    shopitem_s *items, *item;
    i32 *ids;
    char title[128], subtitle[128];
    char *name;
    i32 buyable = 0, show_name = 0, id, available, price;
    i8 area;
    f32 scale, y;
    if (picked == 0) {
        items = HintItems;
        ids = HintShelfIds;
    } else if (picked == 1) {
        items = CharItems;
        ids = CharShelfIds;
    } else if (picked == 2) {
        items = ExtraItems;
        ids = ExtraShelfIds;
    } else if (picked == 4) {
        items = BrickItems;
        ids = BrickShelfIds;
    } else if (picked == 5) {
        items = CutItems;
        ids = CutShelfIds;
    } else
        return;
    if (items == NULL)
        return;
    item = &items[ids[3]];
    name = item->name;
    if (item->type == 1) {
        if (item->unlocked == 1)
            show_name = 1;
        else if ((u16)item->item_id == VehicleBoltId7f17d4Pc ? ((SHOP_SAVE_VIEW_PC *)&Game)->bolt_unlocked != 0
                                                             : CollectIDUnlocked((u16)item->item_id) != 0)
            buyable = show_name = 1;
        else {
            scale = (f32)((f64)ShopLockedScalePc * (f64)0.7f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, (f32)(((f64)HubEpisodeTitleYPc + HubEpisodeSubtitleYPc) * 0.5),
                        1.0f, scale, scale, scale, 0, 255, 0, 0, 1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
        }
    } else if (item->type == 0) {
        buyable = item->unlocked != 1 && item->price != 0;
    } else if (item->type == 2) {
        id = (u16)item->item_id;
        NuStrCpy(subtitle, TTab[ExtrasCheatsPc[id].text_id != NULL ? *ExtrasCheatsPc[id].text_id : tUNKNOWN]);
        name = subtitle;
        if (items[ids[3]].unlocked == 1)
            show_name = 1;
        else {
            area = ExtrasCheatsPc[id].area_index;
            if (id < 8 || area == -1 || Game.area_save[area].red_brick_collected != 0)
                buyable = show_name = 1;
            else {
                sprintf(title, "%s %i", TTab[TextIdPc88074c], id - 7);
                scale = (f32)((f64)ShopLockedScalePc * (f64)0.6f);
                SmartTextEx(title, 0.0f, HubEpisodeTitleYPc, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                            (i32)((f64)ShopNameAlpha * 128.0));
                SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, 255, 0, 0,
                            1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
            }
        }
    } else if (item->type == 4) {
        id = (u16)item->item_id;
        sprintf(title, "%s %i", TTab[TextIdPc880778], id + 1);
        name = title;
        if (items[ids[3]].unlocked == 1)
            show_name = 1;
        else if (!((f64)(id * 3600) > (f64)Game.game_time))
            buyable = show_name = 1;
        else {
            SmartTextEx(title, 0.0f, HubEpisodeTitleYPc, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
            scale = (f32)((f64)ShopLockedScalePc * (f64)0.6f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, 255, 0, 0,
                        1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
        }
    } else if (item->type == 5) {
        id = (u16)item->item_id;
        available = CutScenePlayer_CanStart(id);
        CutScenePlayer_GetText(id, title, subtitle, available);
        SmartTextEx(title, 0.0f, HubEpisodeTitleYPc, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                    (i32)((f64)ShopNameAlpha * 128.0));
        if (available != 0 && shopcutsceneplayer != NULL)
            SmartTextEx(subtitle, 0.0f, HubEpisodeSubtitleYPc, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
        else {
            scale = (f32)((f64)ShopLockedScalePc * (f64)0.6f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, 255, 0, 0,
                        1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
        }
        buyable = show_name = 0;
    }
    if (!(ShopNameAlpha > 0.0f))
        return;
    if (show_name != 0) {
        y = buyable != 0 ? HubEpisodeTitleYPc : (f32)(((f64)HubEpisodeTitleYPc + HubEpisodeSubtitleYPc) * 0.5);
        scale = buyable != 0 ? 0.6f : 0.8f;
        SmartTextEx(name, 0.0f, y, 1.0f, scale, scale, scale, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                    (i32)((f64)ShopNameAlpha * 128.0));
    }
    if (buyable != 0) {
        switch (items[ids[3]].type) {
            case 0:
                price = items[HintShelfIds[3]].price;
                break;
            case 2:
                price = items[ExtraShelfIds[3]].price;
                break;
            case 4:
                price = items[BrickShelfIds[3]].price;
                break;
            case 5:
                return;
            case 1:
            default:
                price = items[CharShelfIds[3]].price;
                break;
        }
        CoinTotal_Draw(price, HubEpisodeSubtitleYPc, 1.2f, 0, ShopNameAlpha, 255, 191, 0);
    }
}

void __cdecl BuyAllShopExtras(void) {
    SHOP_SAVE_VIEW_PC *save = (SHOP_SAVE_VIEW_PC *)&Game;
    i32 index, area;
    for (index = 0; index < 44; ++index) {
        save->extra_purchased_bits[(u32)index >> 5] |= 1u << (index & 31);
        if (WORLD->current_level == HUB_LDATA)
            ExtraItems[index].unlocked = 1;
        area = ExtrasCheatsPc[index].area_index;
        if (area != -1 && Game.area_save[area].red_brick_collected == 0)
            Game.area_save[area].red_brick_collected = 1;
    }
    save->extra_unlocked_bits[0] = 0xffffffffu;
    save->extra_unlocked_bits[1] = 0xffffffffu;
}
