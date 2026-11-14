#include "saga.h"
#include <stdio.h>

typedef struct SHOP_SAVE_VIEW {
    u8 unknown_0000[0x7bd4];
    u32 hint_purchased_bits[3];
    u32 character_purchased_bits[4];
    u32 extra_unlocked_bits[2];
    u32 gold_brick_purchased_bits[2];
    u32 extra_purchased_bits[2];
    u8 unknown_7c08[0x15];
    u8 bolt_unlocked;
} SHOP_SAVE_VIEW;

#define SHOP_FMIN(a, b) ((a) < (b) ? (a) : (b))
#define SHOP_FMAX(a, b) ((a) > (b) ? (a) : (b))
#define SHOP_CLAMP01(x) SHOP_FMAX(0.0f, SHOP_FMIN(x, 1.0f))
#define SHOP_LERP(a, b, t) ((a) + (t) * ((b) - (a)))
extern shopitem_s *CodeItems;
extern i32 SHOPEXTRACOUNT;
extern i32 SHOPCUTCOUNT;
extern i32 oldpicked;
extern i32 subpicked;
extern i32 charcheatix;
extern i32 extracheatix;
extern u32 codelist[144];
extern nuhspecial_s extrasils[44];
extern nuhspecial_s atoz0to9icon[36];
extern nuhspecial_s infoblank;
extern nuhspecial_s cutblank;
extern nuhspecial_s cutfilm_unlocked;
extern nuhspecial_s cutfilm_locked;
extern nuhspecial_s toolblank;
extern nuhspecial_s codeblank;
extern nuhspecial_s question;
extern nuhspecial_s arrow1;
extern nuhspecial_s arrow2;
extern nuhspecial_s arrow3;
extern nuhspecial_s arrow4;
extern struct nugspline_s *shopcamspline;
extern NUVEC *shopcampos;
extern NUVEC *shopcamlookat;
extern NUVEC CharCurPos[7];
extern NUVEC HintCurPos[7];
extern NUVEC ExtraCurPos[7];
extern NUVEC BrickCurPos[7];
extern NUVEC CutCurPos[7];
extern f32 pickedbing;
static f32 scalenorm;
static f32 scalepicked;
static f32 scale2;
static f32 scale3;
static f32 scale4;
extern f32 TopBigScale[6];
extern f32 TopShelfPush[6];
extern f32 TopBigPush[6];
static f32 SubBigCharPush;
static f32 SubNormCharPush;
static f32 subpush[3];
static u16 shelfang;
extern f32 codeshelfscale[6];
extern f32 codemenuscale[6];
extern f32 codebigscale[6];
extern struct CUT_PLAYER_ACTIVE *shopcutsceneplayer;
extern f32 ShopNativeScale[6];
extern f32 ShopNativeBigFactor[6];
extern f32 ShopNativePush[6];
extern f32 ShopNativeSubInitA;
extern f32 ShopNativeSubInitB;
extern f32 ShopSubInitA;
extern f32 ShopSubInitB;
extern f32 ShopCharTopOffset;
extern f32 ShopTopBrickOffset;
extern f32 ShopNativeSubNormCharPush;
extern f32 ShopCodeExitPush;
extern f32 ShopTopCodeOffset;
extern f32 ShopTopExtraOffset;
extern f32 ShopTopHintOffset;
extern i32 (*menuptr)(void *menu);
extern void (*ShopDrawPanelFn)(void);

/* Shop state machine. */
typedef struct SHOP_INPUT_PC {
    i32 left_pressed;
    i32 right_pressed;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_held;
    i32 right_held;
    i32 up_held;
    i32 down_held;
    i32 confirm;
    i32 cancel;
} SHOP_INPUT_PC;

typedef i32 (*SHOP_MENU_CALLBACK_PC)(void *);
typedef void (*SHOP_DRAW_CALLBACK_PC)(void);
extern SHOP_MENU_CALLBACK_PC ShopMenuParent[3];
extern SHOP_MENU_CALLBACK_PC ShopOldMenuParent[3];
extern SHOP_MENU_CALLBACK_PC ShopOldMenuFn;
extern SHOP_DRAW_CALLBACK_PC ShopDrawParent[3];
extern SHOP_DRAW_CALLBACK_PC ShopOldDrawParent[3];
extern SHOP_DRAW_CALLBACK_PC ShopOldDrawPanelFn;
extern SHOP_DRAW_CALLBACK_PC ShopOldDrawFn;
extern i32 ShopMenuLevel;
extern i32 ShopDrawLevel;
extern i32 ShopOldMenuLevel;
extern i32 ShopOldDrawLevel;
extern i32 ShopExit;
extern i32 ShopLastItem;
extern i32 ShopEntered;
extern i32 ShopItemChanged;
extern i32 ShopMoveSfxLock;
extern i32 ShopSubMenu;
extern i32 ShopEaseSubIn;
extern i32 ShopMenu;
extern i32 ShopColumn;
extern f32 ShopSlideTimer;
extern f32 ShopHintDrawWait;
extern f32 ShopOldPickedScale;
extern f32 ShopOldPickedPush;
extern f32 ShopScaleOverride[7];
extern f32 ShopInOutScale;
extern char *ShopCheatName;
extern char ShopUserCode[6];
/* The interaction batch shares SHOP_INPUT_PC and the Shop*Pc state above.
 * Movement mode is 1=left, 2=right. */
extern i32 ShopMoveItems;
extern i32 ShopSubMoveSfxLock;
extern i32 ShopScrollKeyHitPc;
void AddToCompletionPoints(u32 points);
extern f32 ShopPurchasePulse;
extern i32 HubForceShopSave;
extern f32 ShopPurchasePulseDuration;
#define SHOP_RESET_SHELF_POSITIONS()                                                                                                       \
    positions[0] = SubShelfPos[0];                                                                                                         \
    positions[1] = SubShelfPos[1];                                                                                                         \
    positions[2] = SubShelfPos[2];                                                                                                         \
    positions[3] = SubShelfPos[3];                                                                                                         \
    positions[4] = SubShelfPos[4];                                                                                                         \
    positions[5] = SubShelfPos[5];                                                                                                         \
    positions[6] = SubShelfPos[6];
#define SHOP_SLIDE_SLOT(i, j)                                                                                                              \
    t = SHOP_CLAMP01(t);                                                                                                                   \
    positions[i].x = SubShelfPos[i].x + (SubShelfPos[j].x - SubShelfPos[i].x) * t;                                                         \
    positions[i].y = SubShelfPos[i].y + (SubShelfPos[j].y - SubShelfPos[i].y) * t;                                                         \
    positions[i].z = SubShelfPos[i].z + (SubShelfPos[j].z - SubShelfPos[i].z) * t;
extern f32 ShopCheatTimer;
extern AREA_DATA_PC *ANEWHOPE_ADATA;
extern AREA_DATA_PC *ANAKINSFLIGHT_ADATA;
extern i32 ShopCodeCharacter;
extern i32 ShopCodeValid;
extern i32 ShopCodeMoveSfxLock;
extern i32 ShopCodeItemChanged;
extern f32 ShopCodeRepeatTimer;
extern f32 ShopCodeRepeatInterval;
extern f32 ShopCodeExitPush;
extern char ShopCodeCharacters[37];
extern i16 TextIdPc880708;
extern i16 TextIdPc88074c;
extern i16 TextIdPc880778;

/* Same-unit matrix helpers, also kept as standalone copies (source in EAX, destination in ECX). */
static void ShopMtxCopy(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static void NuMtxRotateXInline(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = (m01 * cosx) - (m->m02 * sinx);
    m->m02 = (m01 * sinx) + (m->m02 * cosx);
    m->m11 = (m11 * cosx) - (m->m12 * sinx);
    m->m12 = (m11 * sinx) + (m->m12 * cosx);
    m->m21 = (m21 * cosx) - (m->m22 * sinx);
    m->m22 = (m21 * sinx) + (m->m22 * cosx);
    m->m31 = (m31 * cosx) - (m->m32 * sinx);
    m->m32 = (m31 * sinx) + (m->m32 * cosx);
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

    m->m00 = (m00 * cosx) - (m->m01 * sinx);
    m->m01 = (m00 * sinx) + (m->m01 * cosx);
    m->m10 = (m10 * cosx) - (m->m11 * sinx);
    m->m11 = (m10 * sinx) + (m->m11 * cosx);
    m->m20 = (m20 * cosx) - (m->m21 * sinx);
    m->m21 = (m20 * sinx) + (m->m21 * cosx);
    m->m30 = (m30 * cosx) - (m->m31 * sinx);
    m->m31 = (m30 * sinx) + (m->m31 * cosx);
}

/* Same-unit helper taking EAX; includes the vehicle exception. */
static i32 Shop_CharacterUnlocked(i32 character_id) {
    if (character_id == VehicleBoltId7f17d4) {
        return ((SHOP_SAVE_VIEW *)&Game)->bolt_unlocked != 0;
    }
    return CollectIDUnlocked(character_id) != 0;
}

/* Same-unit helper taking its input in EAX. */
static void Shop_GetInput(SHOP_INPUT_PC *input) {
    i32 player;
    GAMEPAD_RUNTIME_INPUT_PC *pad;
    u32 held;
    u32 pressed;
    memset(input, 0, sizeof(*input));
    for (player = 0; player < 2; ++player) {
        if (MissionMenuPacket.active_player[player] == 0) {
            continue;
        }
        pad = (GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player];
        if ((pad->pressed & GAMEPAD_MENUSELECT) != 0 && netclient == 0) {
            input->confirm = 1;
            return;
        }
        if ((pad->pressed & GAMEPAD_MENUCANCEL) != 0) {
            input->cancel = 1;
            return;
        }
        if (netclient != 0) {
            continue;
        }
        held = pad->held | pad->left_held;
        pressed = pad->pressed | pad->left_pressed;
        if ((held & GAMEPAD_DLEFT) != 0) {
            input->left_held = 1;
        }
        if ((held & GAMEPAD_DRIGHT) != 0) {
            input->right_held = 1;
        }
        if (input->left_held != 0 && input->right_held != 0) {
            input->left_held = input->right_held = 0;
        }
        if ((held & GAMEPAD_DUP) != 0) {
            input->up_held = 1;
        }
        if ((held & GAMEPAD_DDOWN) != 0) {
            input->down_held = 1;
        }
        if (input->up_held != 0 && input->down_held != 0) {
            input->up_held = input->down_held = 0;
        }
        if ((pressed & GAMEPAD_DLEFT) != 0) {
            input->left_pressed = 1;
        }
        if ((pressed & GAMEPAD_DRIGHT) != 0) {
            input->right_pressed = 1;
        }
        if (input->left_pressed != 0 && input->right_pressed != 0) {
            input->left_pressed = input->right_pressed = 0;
        }
        if ((pressed & GAMEPAD_DUP) != 0) {
            input->up_pressed = 1;
        }
        if ((pressed & GAMEPAD_DDOWN) != 0) {
            input->down_pressed = 1;
        }
        if (input->up_pressed != 0 && input->down_pressed != 0) {
            input->up_pressed = input->down_pressed = 0;
        }
    }
}

#pragma bss_seg(".bss$CodePos")
static NUVEC CodePos[6] = {0};
#pragma bss_seg(".bss$ShelfPos")
static NUVEC ShelfPos[6] = {0};
#pragma bss_seg(".bss$splcharshelf")
static struct nugspline_s *splcharshelf = NULL;
#pragma bss_seg()
static struct nugspline_s *splcodes = 0;
static struct nugspline_s *splshelf = 0;

void LoadShelfSplines(void) {
    i32 i;
    i32 shelf_count;
    i32 character_count;
    i32 code_count;
    NUVEC start;
    NUVEC end;
    NUVEC diff;
    splshelf = NuSplineFind(WORLD->current_gscn, "shelf_top1");
    splcharshelf = NuSplineFind(WORLD->current_gscn, "shelf_bottom");
    splcodes = NuSplineFind(WORLD->current_gscn, "shelf_6");
    if (splshelf == NULL || splcharshelf == NULL || splcodes == NULL) {
        return;
    }
    shelf_count = splshelf->length;
    character_count = splcharshelf->length;
    code_count = splcodes->length;
    memset(SubShelfPos, 0, sizeof(SubShelfPos));
    memset(ShelfPos, 0, sizeof(ShelfPos));
    memset(CodePos, 0, 6 * sizeof(*CodePos));
    for (i = 0; i < shelf_count; ++i) {
        ShelfPos[i] = splshelf->pts[i];
    }
    for (i = 0; i < character_count; ++i) {
        SubShelfPos[i] = splcharshelf->pts[i];
    }
    for (i = 0; i < code_count; ++i) {
        CodePos[i] = splcodes->pts[i];
    }
    start = splshelf->pts[0];
    end = splshelf->pts[shelf_count];
    NuVecSub(&diff, &end, &start);
    shelfang = (u16)NuAtan2D(diff.x, diff.z);
}

void InitExtraList(void) {
    i32 i;
    i32 word;
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    for (i = 0; i < 44; ++i) {
        NuStrCpy(ExtraItems[i].special_name, ExtrasCheats[i].extra_name);
        NuSpecialFind(WORLD->current_gscn, &ExtraItems[i].special, ExtraItems[i].special_name, 1);
        ExtraItems[i].item_id = (i16)i;
        ExtraItems[i].type = 2;
        ExtraItems[i].unlocked = 0;
        // MATCH-EXCEPTION: the original keeps the word's byte offset in a register (add r, r twice)
        // instead of scaling the index, which only a byte-offset form reproduces.
        word = i / 32 * 4;
        if (((u64) * (u32 *)((u8 *)save->extra_unlocked_bits + word) & ((u64)1 << (i % 32))) != 0) {
            ExtraItems[i].unlocked = 1;
            *(u32 *)((u8 *)save->extra_purchased_bits + word) |= 1u << (i & 31);
        }
        ExtraItems[i].price = ExtrasCheats[i].extra_price;
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
    u8 i;
    char name[16];
    for (i = 'a'; i <= 'z'; ++i) {
        memset(name, 0, sizeof(name));
        sprintf(name, "shop_%c", i);
        NuSpecialFind(WORLD->current_gscn, &atoz0to9icon[i - 'a'], name, 1);
    }
    for (i = '0'; i <= '9'; ++i) {
        memset(name, 0, sizeof(name));
        sprintf(name, "shop_%c", i);
        NuSpecialFind(WORLD->current_gscn, &atoz0to9icon[i - '0' + 26], name, 1);
    }
}

void UpdateCharacterIDs(void) {
    i32 i;
    for (i = 0; i < ShopCollection.count; ++i) {
        COLLECTID *collect = (COLLECTID *)&ShopCollection.list[i];
        i32 price = collect->field3_0x4;
        CharItems[i].item_id = collect->id;
        CharItems[i].price = price;
    }
}

void InitShop(WORLDINFO *world) {
    i32 i;
    i32 code_count;
    i32 price;
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    CharItems = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 100 * sizeof(shopitem_s));
    HintItems = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    ExtraItems = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    CodeItems = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 64 * sizeof(shopitem_s));
    BrickItems = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 14 * sizeof(shopitem_s));
    if (CharItems == NULL || HintItems == NULL || ExtraItems == NULL || CodeItems == NULL || BrickItems == NULL) {
        return;
    }
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
    for (i = 0; i < ShopCollection.count && i < 100; ++i) {
        COLLECTID *collect = (COLLECTID *)&ShopCollection.list[i];
        if (collect != NULL) {
            i32 character_id = collect->id;
            shopitem_s *item;
            CharItems[SHOPCHARCOUNT].type = 1;
            CharItems[SHOPCHARCOUNT].unlocked = 0;
            if (((u64)save->character_purchased_bits[SHOPCHARCOUNT / 32] & ((u64)1 << (SHOPCHARCOUNT % 32))) != 0) {
                CharItems[SHOPCHARCOUNT].unlocked = 1;
            }
            item = &CharItems[SHOPCHARCOUNT];
            if (CDataList[character_id].name_id != -1) {
                NuStrCpy(item->name, TTab[CDataList[character_id].name_id]);
            } else {
                NuStrCpy(item->name, CDataList[character_id].name);
            }
            memset(&CharItems[SHOPCHARCOUNT].special, 0, sizeof(CharItems[SHOPCHARCOUNT].special));
            if (code_count < 144) {
                codelist[code_count++] = SfxNameHash(collect->cheat_code);
            }
            ++SHOPCHARCOUNT;
        }
    }
    UpdateCharacterIDs();
    charcheatix = code_count;
    for (i = 0; i < 44; ++i) {
        if (code_count < 144) {
            codelist[code_count++] = SfxNameHash(ExtrasCheats[i].code);
        }
    }
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
        HINT *hint = Hint_FindHint(ShopHintIds[i]);
        HintItems[i].type = 0;
        HintItems[i].item_id = (i16)i;
        HintItems[i].price = hint != NULL ? hint->price : 0;
        HintItems[i].unlocked = 0;
        if (((u64)save->hint_purchased_bits[i / 32] & ((u64)1 << (i % 32))) != 0) {
            HintItems[i].unlocked = 1;
        }
        if (HintItems[i].price == 0) {
            HintItems[i].unlocked = 1;
        }
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
    for (i = 0; i < 7; ++i) {
        CharCurPos[i] = SubShelfPos[i];
    }
    for (i = 0; i < 7; ++i) {
        CharShelfIds[i] = (SHOPCHARCOUNT + i - 3) % SHOPCHARCOUNT;
    }
    if (SHOPHINTCOUNT > 0) {
        for (i = 0; i < 7; ++i) {
            HintCurPos[i] = SubShelfPos[i];
        }
        for (i = 0; i < 7; ++i) {
            HintShelfIds[i] = (SHOPHINTCOUNT + i - 3) % SHOPHINTCOUNT;
        }
    }
    if (SHOPEXTRACOUNT > 0) {
        for (i = 0; i < 7; ++i) {
            ExtraCurPos[i] = SubShelfPos[i];
        }
        for (i = 0; i < 7; ++i) {
            ExtraShelfIds[i] = (SHOPEXTRACOUNT + i - 3) % SHOPEXTRACOUNT;
        }
    }
    price = 10000;
    for (i = 0; i < SHOPGOLDBRICKS; ++i) {
        BrickItems[i].type = 4;
        BrickItems[i].item_id = (i16)i;
        BrickItems[i].price = price;
        BrickItems[i].special = TopShelf[4].special;
        BrickItems[i].unlocked = 0;
        if (((u64)save->gold_brick_purchased_bits[i / 32] & ((u64)1 << (i % 32))) != 0) {
            BrickItems[i].unlocked = 1;
        }
        price += 5000;
    }
    for (i = 0; i < 7; ++i) {
        BrickCurPos[i] = SubShelfPos[i];
    }
    for (i = 0; i < 7; ++i) {
        BrickShelfIds[i] = (i + 11) % 14;
    }
    SHOPCUTCOUNT = CutScenePlayCount;
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
            for (i = 0; i < 7; ++i) {
                CutCurPos[i] = SubShelfPos[i];
            }
            for (i = 0; i < 7; ++i) {
                CutShelfIds[i] = (SHOPCUTCOUNT + i - 3) % SHOPCUTCOUNT;
            }
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
    TopShelfScale[0] = ShopNativeScale[0];
    TopShelfScale[1] = ShopNativeScale[2];
    TopShelfScale[2] = ShopNativeScale[1];
    TopShelfScale[3] = ShopNativeScale[3];
    TopShelfScale[4] = ShopNativeScale[4];
    TopShelfScale[5] = ShopNativeScale[5];
    for (i = 0; i < 6; ++i) {
        topscale[i] = TopShelfScale[i];
    }
    TopBigScale[0] = TopShelfScale[0] * ShopNativeBigFactor[0];
    TopBigScale[1] = TopShelfScale[1] * ShopNativeBigFactor[1];
    TopBigScale[2] = TopShelfScale[2] * ShopNativeBigFactor[2];
    TopBigScale[3] = TopShelfScale[3] * ShopNativeBigFactor[3];
    TopBigScale[4] = TopShelfScale[4] * ShopNativeBigFactor[4];
    TopBigScale[5] = TopShelfScale[5] * ShopNativeBigFactor[5];
    TopShelfPush[0] = ShopNativePush[0];
    TopShelfPush[1] = ShopNativePush[1];
    TopShelfPush[2] = ShopNativePush[2];
    TopShelfPush[3] = ShopNativePush[3];
    TopShelfPush[4] = ShopNativePush[4];
    TopShelfPush[5] = ShopNativePush[5];
    TopBigPush[0] = 0.0f;
    TopBigPush[1] = 0.0f;
    TopBigPush[2] = -0.005f;
    TopBigPush[3] = 0.0f;
    TopBigPush[4] = -0.01f;
    TopBigPush[5] = 0.0f;
    for (i = 0; i < 6; ++i) {
        toppush[i] = TopBigPush[i];
    }
    ShopSubInitA = ShopNativeSubInitA;
    ShopSubInitB = ShopNativeSubInitB;
    SubBigCharPush = 0.05f;
    SubNormCharPush = 0.02f;
    subpush[0] = subpush[2] = SubNormCharPush;
    subpush[1] = SubBigCharPush;
    for (i = 0; i < 6; ++i) {
        codebigscale[i] = (f32)((TopShelfScale[3] * (f64)ShopNativeBigFactor[3]) * (f64)0.8f);
        codeshelfscale[i] = codemenuscale[i] = (f32)(TopShelfScale[3] * (f64)0.8f);
    }
    shopcutsceneplayer = CutScenePlayer_Available();
}

i32 CheckCash(shopitem_s *items, i32 item) {
    return Game.coins >= (u32)items[item].price;
}

i32 BuyShopItem(shopitem_s *items, i32 index, i32 charge) {
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    u32 *bits = NULL;
    if (items[index].type == 0) {
        bits = save->hint_purchased_bits;
    } else if (items[index].type == 1) {
        bits = save->character_purchased_bits;
    } else if (items[index].type == 2) {
        bits = save->extra_unlocked_bits;
    } else if (items[index].type == 4) {
        bits = save->gold_brick_purchased_bits;
    } else if (items[index].type == 5) {
        PlaySfx("MenuSelect", NULL);
        return 1;
    }
    if (bits != NULL) {
        bits[index / 32] |= (u32)((u64)1 << (index % 32));
    }
    if (charge != 0) {
        Game.coins -= items[index].price;
    }
    ShopPurchasePulse = ShopPurchasePulseDuration;
    items[index].unlocked = 1;
    if (charge != 0) {
        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
    }
    return 1;
}

i32 SelectShopItem(shopitem_s *items, i32 index) {
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    HINT *hint;
    i32 id;
    i32 i;
    switch (items[index].type) {
        case 1:
            if (items[index].unlocked == 1) {
                return 0;
            }
            id = (u16)items[index].item_id;
            if (id < 0 || id >= CHARCOUNT) {
                return 0;
            }
            if (Shop_CharacterUnlocked(id) == 0) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                ShopLockedScale = 1.5f;
                return 0;
            }
            if (Game.coins < (u32)items[index].price) {
                CoinTotalScale = 1.5f;
                return -1;
            }
            if (AddToCollection(id) != 0) {
                AddToCompletionPoints(POINTS_PER_CHARACTER);
            buy:
                BuyShopItem(items, index, 1);
            }
            return 1;
        case 0:
            id = (u16)items[index].item_id;
            if (items[index].unlocked == 1 || items[index].price == 0) {
                return 1;
            }
            if (Game.coins < (u32)items[index].price) {
                CoinTotalScale = 1.5f;
                return -1;
            }
            AddToCompletionPoints(POINTS_PER_HINT);
            hint = Hint_FindHint(ShopHintIds[id]);
            if (hint != NULL) {
                hint->flags |= 2;
            }
            goto buy;
        case 2:
            id = (u16)items[index].item_id;
            if (items[index].unlocked == 1 || (save->extra_purchased_bits[id / 32] & (1u << (id & 31))) != 0) {
                return 0;
            }
            if (ExtrasCheats[id].area_index != -1 && Game.area_save[ExtrasCheats[id].area_index].red_brick_collected == 0) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                ShopLockedScale = 1.5f;
                return 0;
            }
            if (Game.coins < (u32)items[index].price) {
                CoinTotalScale = 1.5f;
                return -1;
            }
            save->extra_purchased_bits[id / 32] |= 1u << (id & 31);
            AddToCompletionPoints(POINTS_PER_CHEAT);
            goto buy;
        case 4:
            if (items[index].unlocked == 1 || items[index].price == 0) {
                return 0;
            }
            if (Game.game_time < index * 3600) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                ShopLockedScale = 1.5f;
                return 0;
            }
            if (Game.coins < (u32)items[index].price) {
                CoinTotalScale = 1.5f;
                return -1;
            }
            AddToCompletionPoints(POINTS_PER_GOLDBRICK);
            AddToGoldBricks();
            goto buy;
        case 5:
            if (CutScenePlayer_CanStart(index) == 0) {
                PlaySfx("MenuNoEntry", NULL);
                ShopLockedScale = 1.5f;
                return 0;
            }
            ShopOldMenuFn = menuptr;
            ShopOldPickedScale = topscale[5];
            ShopOldPickedPush = toppush[5];
            for (i = 0; i < 3; ++i) {
                ShopOldDrawParent[i] = ShopDrawParent[i];
                ShopOldMenuParent[i] = ShopMenuParent[i];
            }
            ShopOldDrawPanelFn = ShopDrawPanelFn;
            ShopOldDrawFn = drawptr;
            ShopOldMenuLevel = ShopMenuLevel;
            ShopOldDrawLevel = ShopDrawLevel;
            oldpicked = picked;
            menuptr = NULL;
            for (i = 0; i < 3; ++i) {
                ShopDrawParent[i] = NULL;
                ShopMenuParent[i] = NULL;
            }
            ShopMenuLevel = 0;
            ShopDrawLevel = 0;
            SHOPACTIVE = 0;
            BuyShopItem(items, index, 0);
            ShopPurchasePulse = 0.0f;
            TempGame.game_time = Game.game_time;
            if (memcmp(&TempGame, &Game, sizeof(Game)) != 0) {
                HubForceShopSave = 1;
            }
            CutScenePlayer_Start(index);
            return 1;
        default:
            return -1;
    }
}

/* Right moves IDs toward index zero, interpolates slots 1..6 toward their
 * previous shelf slot, and uses scale2. */
i32 MoveSubItemsRight(i32 *ids, NUVEC *positions, i32 count) {
    i32 angle;
    i32 i;
    f32 phase;
    f32 reverse_phase;
    f32 t;
    if (ShopSlideTimer <= 0.0f) {
        for (i = 0; i < 6; ++i) {
            ids[i] = ids[i + 1];
        }
        ++ids[6];
        if (ids[6] >= count) {
            ids[6] = 0;
        }
        ShopSlideTimer = 0.0f;
        scale4 = scalenorm;
        scale3 = scalepicked;
        scale2 = scalenorm;
        SHOP_RESET_SHELF_POSITIONS()
        subpush[0] = SubNormCharPush;
        subpush[1] = SubBigCharPush;
        subpush[2] = SubNormCharPush;
        ShopMoveItems = 0;
        ShopScaleOverride[0] = 0.0f;
        ShopScaleOverride[1] = 0.4f;
        ShopScaleOverride[2] = 0.8f;
        ShopScaleOverride[4] = 0.8f;
        ShopScaleOverride[5] = 0.4f;
        ShopScaleOverride[6] = 0.0f;
        return 1;
    }
    t = 1.0f - ShopSlideTimer * 8.0f;
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
    ShopScaleOverride[0] = reverse_phase;
    ShopScaleOverride[6] = phase;
    return 0;
}

/* Interpolates in descending order with the distinct scale4, including when
 * positions aliases SubShelfPos. */
i32 MoveSubItemsLeft(i32 *ids, NUVEC *positions, i32 count) {
    i32 angle;
    i32 i;
    f32 phase;
    f32 reverse_phase;
    f32 t;
    if (ShopSlideTimer <= 0.0f) {
        for (i = 6; i > 0; --i) {
            ids[i] = ids[i - 1];
        }
        --ids[0];
        if (ids[0] < 0) {
            ids[0] = count - 1;
        }
        ShopSlideTimer = 0.0f;
        scale4 = scalenorm;
        scale3 = scalepicked;
        scale2 = scalenorm;
        SHOP_RESET_SHELF_POSITIONS()
        subpush[0] = SubNormCharPush;
        subpush[1] = SubBigCharPush;
        subpush[2] = SubNormCharPush;
        ShopMoveItems = 0;
        ShopScaleOverride[0] = 0.0f;
        ShopScaleOverride[1] = 0.4f;
        ShopScaleOverride[2] = 0.8f;
        ShopScaleOverride[4] = 0.8f;
        ShopScaleOverride[5] = 0.4f;
        ShopScaleOverride[6] = 0.0f;
        return 1;
    }
    t = 1.0f - ShopSlideTimer * 8.0f;
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
    ShopScaleOverride[0] = phase;
    ShopScaleOverride[6] = reverse_phase;
    return 0;
}

void SelectSubItem(void) {
    shopitem_s *items;
    i32 index;
    i32 result;
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
    if (items == NULL || index == -1) {
        return;
    }
    result = SelectShopItem(items, index);
    if (result == -1) {
        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
    } else if (result == 1) {
        ShopNameAlpha = 0.0f;
    }
}

/* Uses direct shop-pad input, buys on one confirm, and has no touch rows,
 * item-selection mode, or cursor input. */
i32 SubItemMenu(void *menu) {
    SHOP_INPUT_PC input;
    shopitem_s *items = NULL;
    i32 *ids;
    i32 *count;
    NUVEC *positions;
    NUVEC position;
    i32 left = 0;
    i32 right = 0;
    i32 cancel = 0;
    i32 scrolling = 0;
    f32 ypush = 0.0f;
    f32 phase;
    f32 factor;
    i32 angle;
    (void)menu;
    Shop_GetInput(&input);
    if (ShopHintDrawWait > 0.0f) {
        ShopHintDrawWait -= FRAMETIME;
        ShopNameAlpha = 0.0f;
    }
    if (ShopExit != 0) {
        ShopNameAlpha = 0.0f;
    }
    if (picked == 0) {
        items = HintItems;
        ids = HintShelfIds;
        positions = HintCurPos;
        count = &SHOPHINTCOUNT;
        if (ShopHintDrawWait <= 0.0f && (items[ids[3]].unlocked == 1 || items[ids[3]].price == 0)) {
            Hint_ResetHint(ShopHintIds[(u16)items[ids[3]].item_id], 0);
            Hint_SetHintFromId(ShopHintIds[(u16)items[ids[3]].item_id], 1, 0);
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
    } else {
        ids = NULL;
        positions = NULL;
        count = NULL;
    }
    if (ShopSlideTimer >= 0.0f) {
        ShopSlideTimer -= FRAMETIME;
        if (ShopSlideTimer < 0.0f) {
            ShopNameAlpha = 0.0f;
        }
    }
    if (ShopEaseSubIn != 0) {
        phase = (f32)(1.0 - (f64)ShopSlideTimer * 8.0);
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopEaseSubIn == 1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScale = 0.0f + factor;
        } else if (ShopEaseSubIn == -1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScale = (f32)(1.0f + (factor * -1.0));
        }
    }
    if (ShopMoveItems != 0) {
        if (ShopSubMoveSfxLock == 0) {
            ShopSubMoveSfxLock = 1;
            GameAudio_PlaySfx(0x2f, positions, 0, 0);
        }
        if (ShopMoveItems == 1) {
            if (MoveSubItemsLeft(ids, positions, *count) != 0 && items != NULL) {
                if (items[ids[3]].type == 0 && items[ids[3]].unlocked == 1) {
                    SelectSubItem();
                }
            }
        } else if (ShopMoveItems == 2) {
            if (MoveSubItemsRight(ids, positions, *count) != 0 && items != NULL) {
                if (items[ids[3]].type == 0 && items[ids[3]].unlocked == 1) {
                    SelectSubItem();
                }
            }
        }
    } else {
        ShopSubMoveSfxLock = 0;
    }
    if (ShopSlideTimer < 0.0f) {
        ShopEaseSubIn = 0;
        if (input.left_held != 0) {
            left = 1;
        } else if (input.right_held != 0) {
            right = 1;
        } else if (input.confirm != 0 && ShopExit == 0) {
            SelectSubItem();
            goto finish;
        } else if (input.cancel != 0) {
            cancel = 1;
        }
    }
    if (ShopExit == 0) {
        if (left != 0) {
            ShopScaleOverride[0] = 0.0f;
            ShopScaleOverride[6] = 1.0f;
            ShopScaleOverride[1] = 0.4f;
            ShopScaleOverride[2] = 0.8f;
            ShopScaleOverride[4] = 0.8f;
            ShopScaleOverride[5] = 0.4f;
            position = SubShelfPos[0];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            position = SubShelfPos[6];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            ShopSlideTimer = 0.125f;
            ShopMoveItems = 1;
            scrolling = 1;
            ShopHintDrawWait = 0.2f;
        } else if (right != 0) {
            ShopScaleOverride[0] = 1.0f;
            ShopScaleOverride[6] = 0.0f;
            ShopScaleOverride[1] = 0.4f;
            ShopScaleOverride[2] = 0.8f;
            ShopScaleOverride[4] = 0.8f;
            ShopScaleOverride[5] = 0.4f;
            position = SubShelfPos[6];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            position = SubShelfPos[0];
            position.y += ypush;
            AddGameDebris(WORLD->game_debris_sys, qrand() / 10923 + 76, &position);
            ShopMoveItems = 2;
            ShopSlideTimer = 0.125f;
            scrolling = 1;
            ShopHintDrawWait = 0.2f;
        }
        if (cancel != 0) {
            ShopSlideTimer = 0.125f;
            ShopEaseSubIn = -1;
            ShopExit = 1;
            GameAudio_PlaySfx(0x31, NULL, 0, 0);
        }
    }
finish:
    if (ShopExit != 0 && ShopSlideTimer < 0.0f) {
        ShopSlideTimer = 0.125f;
        ShopExit = 0;
        ShopEaseSubIn = -1;
        ShopLastItem = -1;
        GameCam_Blend(GameCam, 0.6f, 0.0f, 1);
        ShopNameAlpha = 0.0f;
        return 6;
    }
    ShopScrollKeyHitPc = scrolling;
    return 0;
}

/* Cancellation belongs to the active child callback. */
i32 DoShopMenu(void *menu) {
    i32 result;
    if (menuptr != NULL) {
        result = menuptr(menu);
        switch (result) {
            case 5:
                return 1;
            case 6:
                menuptr = ShopMenuParent[ShopMenuLevel];
                ShopMenuParent[ShopMenuLevel] = NULL;
                --ShopMenuLevel;
                if (ShopMenuLevel < 0) {
                    ShopMenuLevel = 0;
                }
                ShopDrawPanelFn = ShopDrawParent[ShopDrawLevel];
                ShopDrawParent[ShopDrawLevel] = NULL;
                drawptr = NULL;
                --ShopDrawLevel;
                if (ShopDrawLevel < 0) {
                    ShopDrawLevel = 0;
                }
                break;
        }
    }
    return 0;
}

i32 CodeMenu(void *menu) {
    i32 down = 0;
    i32 up = 0;
    i32 left = 0;
    i32 right = 0;
    i32 confirm = 0;
    i32 cancel = 0;
    SHOP_INPUT_PC input;
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    f32 factor;
    f32 phase;
    i32 i;
    i32 angle;
    i8 area;
    char code[7];
    u32 crc;
    (void)menu;
    Shop_GetInput(&input);
    if (ShopSlideTimer >= 0.0f) {
        ShopSlideTimer -= FRAMETIME;
    }
    if (ShopCodeRepeatTimer >= 0.0f) {
        ShopCodeRepeatTimer -= FRAMETIME;
    }
    ShopCodeItemChanged = 0;
    if (ShopEaseSubIn) {
        phase = (f32)(1.0 - (f64)ShopSlideTimer * 8.0);
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopEaseSubIn == 1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScale = SHOP_LERP(0.0f, 1.0f, factor);
        } else if (ShopEaseSubIn == -1) {
            factor = SHOP_CLAMP01(factor);
            ShopInOutScale = (f32)(1.0f + (factor * -1.0));
        }
    }
    if (ShopSlideTimer <= 0.0f) {
        ShopCodeValid = 0;
        ShopEaseSubIn = 0;
        if (input.left_held) {
            left = 1;
        } else if (input.right_held) {
            right = 1;
        } else if (input.down_held) {
            if (ShopCodeRepeatTimer <= 0.0f || input.down_pressed) {
                down = 1;
            }
        } else if (input.up_held) {
            if (ShopCodeRepeatTimer <= 0.0f || input.up_pressed) {
                up = 1;
            }
        }
        if (input.confirm) {
            confirm = 1;
        } else if (input.cancel) {
            cancel = 1;
        }
        if (down) {
            ShopCodeRepeatTimer = ShopCodeRepeatInterval;
            --ShopCodeCharacter;
            if (ShopCodeCharacter < 0) {
                ShopCodeCharacter = 35;
            }
            ShopCodeValid = 0;
            ShopUserCode[ShopColumn] = ShopCodeCharacters[ShopCodeCharacter];
            ShopCodeMoveSfxLock = 1;
            GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
        }
        if (up) {
            ShopCodeRepeatTimer = ShopCodeRepeatInterval;
            ++ShopCodeCharacter;
            if (ShopCodeCharacter >= 36) {
                ShopCodeCharacter = 0;
            }
            ShopCodeValid = 0;
            ShopUserCode[ShopColumn] = ShopCodeCharacters[ShopCodeCharacter];
            ShopCodeMoveSfxLock = 1;
            GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
        }
        if (left) {
            ShopLastItem = ShopColumn;
            --ShopColumn;
            if (ShopColumn < 0) {
                ShopColumn = 0;
            }
            ShopCodeValid = 0;
            if (!ShopUserCode[ShopColumn]) {
                ShopCodeCharacter = 0;
            } else {
                for (i = 0; i < 36; ++i) {
                    if (ShopUserCode[ShopColumn] == ShopCodeCharacters[i]) {
                        ShopCodeCharacter = i;
                        break;
                    }
                }
            }
            if (ShopLastItem != ShopColumn) {
                ShopSlideTimer = 0.125f;
                if (!ShopCodeMoveSfxLock) {
                    ShopCodeMoveSfxLock = 1;
                    GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
                }
            }
        }
        if (right) {
            ShopLastItem = ShopColumn;
            ++ShopColumn;
            if (ShopColumn >= 6) {
                ShopColumn = 5;
            }
            ShopCodeValid = 0;
            if (!ShopUserCode[ShopColumn]) {
                ShopCodeCharacter = 0;
            } else {
                for (i = 0; i < 36; ++i) {
                    if (ShopUserCode[ShopColumn] == ShopCodeCharacters[i]) {
                        ShopCodeCharacter = i;
                        break;
                    }
                }
            }
            if (ShopLastItem != ShopColumn) {
                ShopSlideTimer = 0.125f;
                if (!ShopCodeMoveSfxLock) {
                    ShopCodeMoveSfxLock = 1;
                    GameAudio_PlaySfx(0x2f, &CodePos[2], 0, 0);
                }
            }
        }
        if (cancel) {
            ShopSlideTimer = 0.125f;
            ShopCodeValid = 0;
            ShopCodeCharacter = 0;
            ShopEaseSubIn = -1;
            ShopExit = 1;
            GameAudio_PlaySfx(0x31, NULL, 0, 0);
        }
        if (confirm) {
            ShopUserCode[ShopColumn] = ShopCodeCharacters[ShopCodeCharacter];
            for (i = 0; i < 6; ++i) {
                code[i] = ShopUserCode[i];
            }
            code[6] = 0;
            crc = SfxNameHash(code);
            for (i = 0; i < 144; ++i) {
                if (codelist[i] && crc == codelist[i]) {
                    ShopCodeValid = 1;
                    break;
                }
            }
            if (i >= 144) {
                PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                ShopCheatName = NULL;
            } else {
                if (i < charcheatix) {
                    if (Game_CharacterSave && !(Game_CharacterSave[(u16)CharItems[i].item_id] & SAVE_CHARACTER_UNLOCKED)) {
                        Game_CharacterSave[(u16)CharItems[i].item_id] |= SAVE_CHARACTER_UNLOCKED;
                        AddToCompletionPoints(POINTS_PER_CHARACTER);
                        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
                    } else {
                        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                    }
                    ShopCheatName = TTab[CDataList[(u16)CharItems[i].item_id].name_id];
                } else if (i < extracheatix) {
                    i -= charcheatix;
                    if (!(save->extra_purchased_bits[i / 32] & (1U << (i & 31)))) {
                        save->extra_purchased_bits[i / 32] |= 1U << (i & 31);
                        ExtraItems[i].unlocked = 1;
                        AddToCompletionPoints(POINTS_PER_CHEAT);
                        area = ExtrasCheats[i].area_index;
                        if (area != -1 && !Game.area_save[area].red_brick_collected) {
                            Game.area_save[area].red_brick_collected = 1;
                            AddToCompletionPoints(POINTS_PER_REDBRICK);
                        }
                        save->extra_unlocked_bits[i / 32] |= (u32)(1ULL << (i % 32));
                        PlaySfx("Shop_BuyCheat", &SubShelfPos[3]);
                    } else {
                        PlaySfx("Shop_NotEnufMuny", &SubShelfPos[3]);
                    }
                    ShopCheatName = ExtrasCheats[i].text_id ? TTab[*ExtrasCheats[i].text_id] : TTab[tUNKNOWN];
                }
                ShopPurchasePulse = ShopPurchasePulseDuration;
                ShopCheatTimer = 3.1499999f;
            }
        }
    } else {
        ShopCodeMoveSfxLock = 0;
    }

    if (ShopSlideTimer >= 0.0f) {
        phase = (f32)(1.0 - (f32)((f64)ShopSlideTimer * 8.0));
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        factor = (f32)(1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
        if (ShopLastItem != -1) {
            if (ShopEaseSubIn == 1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItem] = SHOP_LERP(codemenuscale[ShopLastItem], codebigscale[ShopLastItem], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            } else if (ShopEaseSubIn == -1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItem] = SHOP_LERP(codebigscale[ShopLastItem], codemenuscale[ShopLastItem], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            } else {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopLastItem] = SHOP_LERP(codebigscale[ShopLastItem], codemenuscale[ShopLastItem], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[2] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            }
        }
        if (ShopColumn != -1) {
            if (ShopEaseSubIn == 1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumn] = SHOP_LERP(codemenuscale[ShopColumn], codebigscale[ShopColumn], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            } else if (ShopEaseSubIn == -1) {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumn] = SHOP_LERP(codebigscale[ShopColumn], codemenuscale[ShopColumn], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubBigCharPush, SubNormCharPush, factor);
            } else {
                factor = SHOP_CLAMP01(factor);
                codeshelfscale[ShopColumn] = SHOP_LERP(codemenuscale[ShopColumn], codebigscale[ShopColumn], factor);
                factor = SHOP_CLAMP01(factor);
                subpush[1] = SHOP_LERP(SubNormCharPush, SubBigCharPush, factor);
            }
        }
    } else {
        ShopLastItem = -1;
    }
    if (ShopExit && ShopSlideTimer < 0.0f) {
        ShopExit = 0;
        ShopEaseSubIn = -1;
        ShopLastItem = -1;
        subpush[1] = ShopCodeExitPush;
        ShopSlideTimer = 0.125f;
        ShopNameAlpha = 0.0f;
        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        return 6;
    }
    return 7;
}

/* ShopCheatName is a text pointer; the integer code column is separate. */
void DrawCodeMenu(void) {
    if (ShopCheatTimer > 0.0f) {
        if (ShopCheatName != NULL && (f64)ShopCheatTimer < 3.1499998569488525 && NuFmod(ShopCheatTimer, 0.35f) < 0.175f) {
            SmartTextEx(ShopCheatName, 0.0f, (f32)(((f64)HubEpisodeTitleY + HubEpisodeSubtitleY) * 0.5), 1.0f, 0.8f, 0.8f, 0.8f, 0, 0, 255,
                        0, 1.7f, 1, NULL, 0, 128);
        }
        ShopCheatTimer -= FRAMETIME;
    }
}

static void Shop_DrawCharacter(shopitem_s *item, NUVEC *position, f32 scale_value, f32 ypush, u16 xrot, u16 yrot, u16 zrot) {
    if (!(!NuSpecialExistsFn(&iconback))) {
        i32 character_id;
        i32 unavailable;
        f32 alpha;
        NUVEC scale;
        NUMTX matrix;
        i32 icon_object_id;
        i32 top_shelf_character;
        if (item == &TopShelf[1]) {
            item = CharItems - (i32)(NuFmod(GameTimer.time_elapsed, (f32)(SHOPCHARCOUNT * (double)0.2f)) / (double)-0.2f);
            top_shelf_character = 1;
        } else {
            top_shelf_character = 0;
        }

        character_id = (u16)(item->item_id);
        unavailable = 1;
        if ((character_id == VehicleBoltId7f17d4 ? ((SHOP_SAVE_VIEW *)&Game)->bolt_unlocked != 0
                                                 : CollectIDUnlocked(character_id) != NULL) &&
            item->unlocked == 1) {
            unavailable = 0;
        }
        alpha =
            (character_id == VehicleBoltId7f17d4 ? ((SHOP_SAVE_VIEW *)&Game)->bolt_unlocked != 0 : CollectIDUnlocked(character_id) != NULL)
                ? 1.0f
                : 0.5f;

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
            if (unavailable != 0) {
                ++icon_object_id;
            }
            if (WORLD->level_objects[icon_object_id].active != 0) {
                NuSpecialDrawAtAlpha(&WORLD->level_objects[icon_object_id].special, &matrix, alpha);
            }
        }
    }
}

void DrawItem(nuhspecial_s *special, NUVEC *position, float scale_value, float unused3, float y_push, u16 x_rot, u16 y_rot, u16 z_rot) {
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

static void Shop_DrawItem(shopitem_s *item, NUVEC *position, f32 scale_value, f32 alpha, i32 opaque, f32 ypush, u16 xrot, u16 yrot,
                          u16 zrot) {
    NUVEC scale;
    NUANGVEC rotation;
    NUMTX matrix;
    if (item == NULL || position == NULL) {
        return;
    }
    if (NuSpecialExistsFn(&item->special) == 0) {
        return;
    }
    rotation.x = xrot;
    rotation.y = yrot;
    rotation.z = zrot;
    NuMtxSetRotateXYZVU0(&matrix, &rotation);
    scale.x = scale.y = scale.z = scale_value;
    NuMtxScaleVU0(&matrix, &scale);
    *(NUVEC *)&matrix.m30 = *position;
    matrix.m31 += ypush;
    if (opaque != 0) {
        NuSpecialDrawAt(&item->special, &matrix);
    } else {
        NuSpecialDrawAtAlpha(&item->special, &matrix, alpha);
    }
}

static void Shop_DrawBrick(shopitem_s *item, NUVEC *position, f32 scale_value, f32 ypush, u16 xrot, u16 yrot, u16 zrot) {
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
    Shop_DrawItem(&TopShelf[0], &ShelfPos[0], topscale[0], 1.0f, 1, toppush[0] + ShopTopHintOffset, 0, shelfang, 0);
    Shop_DrawCharacter(&TopShelf[1], &ShelfPos[1], topscale[1], toppush[1] + ShopCharTopOffset, 0, shelfang, 0);
    Shop_DrawItem(&TopShelf[2], &ShelfPos[2], topscale[2], 1.0f, 1, toppush[2] + ShopTopExtraOffset, 0, shelfang, 0);
    DrawItem(&codeblank, &ShelfPos[3], topscale[3], 1.0f, toppush[3] + ShopTopCodeOffset, 0, shelfang, 0);
    Shop_DrawItem(&TopShelf[3], &ShelfPos[3], topscale[3], 1.0f, 0, ShopTopCodeOffset + toppush[3], 0, shelfang, 0);
    Shop_DrawBrick(&TopShelf[4], &ShelfPos[4], topscale[4], toppush[4] + ShopTopBrickOffset, 0, (u16)(shelfang + 0x2000), 0);
    Shop_DrawItem(&TopShelf[5], &ShelfPos[5], topscale[5], 1.0f, 1, toppush[5], 0, shelfang, 0);
}

/* Renders exactly six code slots, uses codeshelfscale, and updates the
 * arrow phase. */
void DrawCodeMenu3D(void) {
    i32 letters[6];
    i32 i;
    i32 angle;
    i32 rotation = shelfang;

    f32 bing = 0.0f;
    f32 scale;
    f32 push;
    for (i = 0; i < 6; ++i) {
        if (ShopUserCode[i] <= 'Z' && ShopUserCode[i] >= 'A') {
            letters[i] = ShopUserCode[i] - 'A';
        } else if (ShopUserCode[i] <= '9' && ShopUserCode[i] >= '0') {
            letters[i] = ShopUserCode[i] - '0' + 26;
        }
    }
    if (ShopPurchasePulse > 0.0f) {
        bing = ShopPurchasePulse;
        bing /= ShopPurchasePulseDuration;
        bing = (f32)((f64)bing * 0.5);
        ShopPurchasePulse -= FRAMETIME;
    } else {
        ShopPurchasePulse = 0.0f;
    }
    for (i = 0; i < 6; ++i) {
        if (i == ShopColumn) {
            scale = codeshelfscale[i] * ShopNativeScale[3] * ShopInOutScale + bing * ShopCodePulseScale;
            push = subpush[1];
        } else if (i == ShopLastItem) {
            scale = codeshelfscale[i] * ShopNativeScale[3] * ShopInOutScale + bing * ShopCodePulseScale;
            push = subpush[2];
        } else {
            scale = codeshelfscale[i] * ShopNativeScale[3] * ShopInOutScale;
            push = SubNormCharPush;
        }
        DrawItem(&codeblank, &CodePos[i], scale, 1.0f, push, 0, rotation, 0);
        DrawItem(&atoz0to9icon[letters[i]], &CodePos[i], scale, 1.0f, push, 0, rotation, 0);
    }
    angle = (i32)((f64)ShopInOutScale * 32768.0);
    ShopCodeArrowBing = NuTrigTable[(angle >> 1) & 0x7fff] * ShopCodeArrowAmplitude;
}

void DrawSubItems(void) {
    f32 bing = 0.0f;
    f32 bing_scale;
    f32 item_scale;
    f32 normal_push;
    f32 scale;
    f32 ypush;
    shopitem_s *items;
    shopitem_s *item;
    NUVEC *positions;
    i32 *shelf_ids;
    i32 slot;
    i32 phase_angle;
    i32 id;
    i32 area;
    i32 episode;
    u16 rotation;
    i32 angle;
    nuhspecial_s *blank;
    nuhspecial_s *film;
    LEVELDATA_PC *level;
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    if (ShopPurchasePulse > 0.0f) {
        bing = ShopPurchasePulse / ShopPurchasePulseDuration;
        ShopPurchasePulse -= FRAMETIME;
    } else {
        ShopPurchasePulse = 0.0f;
    }
    phase_angle = (i32)((f64)ShopPurchasePulse * 32768.0 + 16384.0);
    bing_scale = (f32)(1.0 - ((f64)NuTrigTable[(phase_angle >> 1) & 0x7fff] + 1.0) * 0.5);
    switch (picked) {
        case 0:
            items = HintItems;
            shelf_ids = HintShelfIds;
            positions = HintCurPos;
            item_scale = ShopNativeScale[0];
            break;
        case 1:
            items = CharItems;
            shelf_ids = CharShelfIds;
            positions = CharCurPos;
            item_scale = ShopNativeScale[2];
            break;
        case 2:
            items = ExtraItems;
            shelf_ids = ExtraShelfIds;
            positions = ExtraCurPos;
            item_scale = ShopNativeScale[1];
            break;
        case 4:
            items = BrickItems;
            shelf_ids = BrickShelfIds;
            positions = BrickCurPos;
            item_scale = ShopNativeScale[4];
            break;
        case 5:
            items = CutItems;
            shelf_ids = CutShelfIds;
            positions = CutCurPos;
            item_scale = ShopNativeScale[5];
            break;
        default:
            return;
    }
    normal_push = SubNormCharPush;
    for (slot = 0; slot < 7; ++slot) {
        if ((ShopMoveItems == 1 && slot == 6) || (ShopMoveItems == 2 && slot == 0)) {
            continue;
        }
        rotation = 0;
        if (slot == 0) {
            scale = item_scale * ShopInOutScale * ShopScaleOverride[0];
            ypush = normal_push * ShopScaleOverride[0];
        } else if (slot == 6) {
            scale = item_scale * ShopInOutScale * ShopScaleOverride[6];
            ypush = normal_push * ShopScaleOverride[6];
        } else if (ShopMoveItems == 1 && slot == 5) {
            scale = item_scale * ShopInOutScale * ShopScaleOverride[6];
            ypush = normal_push * ShopScaleOverride[6];
        } else if (ShopMoveItems == 2 && slot == 1) {
            scale = item_scale * ShopInOutScale * ShopScaleOverride[0];
            ypush = normal_push * ShopScaleOverride[0];
        } else if (slot == 2) {
            scale = item_scale * scale4 * ShopInOutScale;
            ypush = subpush[0];
        } else if (slot == 3) {
            if (ShopPurchasePulse >= 0.0f) {
                rotation = (u16)(i32)((f64)(-bing) * 65536.0);
            }
            scale = item_scale * scale3 * ShopInOutScale + ShopCodePulseScale * bing_scale;
            ypush = subpush[1];
        } else if (slot == 4) {
            scale = item_scale * scale2 * ShopInOutScale;
            ypush = subpush[2];
        } else {
            scale = item_scale * ShopInOutScale;
            ypush = normal_push;
        }
        item = &items[shelf_ids[slot]];
        switch (picked) {
            case 0:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulse > 0.0f) {
                        if (slot == 3) {
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        } else {
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        }
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
                rotation = (u16)(i32)((f64)NuTrigTable[(((u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, ShopCharacterPeriod) /
                                                                    (f64)ShopCharacterPeriod * 65536.0) +
                                                         0x4000) >>
                                                        1) &
                                                       0x7fff] *
                                      4369.0);
                if (item->unlocked != 1) {
                    rotation = 0;
                }
                angle = shelfang + rotation;
                Shop_DrawCharacter(&items[shelf_ids[slot]], &positions[slot], scale, ypush, 0, angle, 0);
                break;
            case 2:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulse > 0.0f) {
                        if (slot == 3) {
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        } else {
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        }
                    } else {
                        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    }
                    angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                    DrawItem(&toolblank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    Shop_DrawItem(&items[shelf_ids[slot]], &positions[slot], scale, 1.0f, 1, ypush, 0, angle, 0);
                } else {
                    angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                    DrawItem(&toolblank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    item = &items[shelf_ids[slot]];
                    id = (u16)item->item_id;
                    if ((save->extra_purchased_bits[id / 32] & (1u << (id & 31))) != 0 || id < 8 || ExtrasCheats[id].area_index == -1 ||
                        Game.area_save[ExtrasCheats[id].area_index].red_brick_collected != 0) {
                        Shop_DrawItem(item, &positions[slot], scale, 1.0f, 1, ypush, 0, angle, 0);
                    } else if (NuSpecialExistsFn(&extrasils[shelf_ids[slot]]) != 0) {
                        DrawItem(&extrasils[shelf_ids[slot]], &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                    }
                }
                break;
            case 4:
                if (item->unlocked == 1) {
                    if (ShopPurchasePulse > 0.0f) {
                        if (slot == 3) {
                            rotation += (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        } else {
                            rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                        }
                    } else {
                        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
                    }
                    rotation =
                        (u16)(rotation + (u16)(items[shelf_ids[slot]].item_id * 0x1249) + (i32)((u32)ShopBrickRotationDegrees << 16) / 360);
                }
                angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                Shop_DrawBrick(&items[shelf_ids[slot]], &positions[slot], scale, ypush, 0, angle, 0);
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
                        if (episode & 1) {
                            blank = &codeblank;
                        }
                    } else if (area != -1) {
                        if (ANEWHOPE_ADATA != NULL && area == ANEWHOPE_ADATA->index) {
                            blank = &codeblank;
                        } else if (ANAKINSFLIGHT_ADATA != NULL && area == ANAKINSFLIGHT_ADATA->index) {
                            blank = &toolblank;
                        }
                    }
                    rotation = (u16)(id * 0x1555 - (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * -65536.0));
                    film = &cutfilm_unlocked;
                }
                angle = shelfang + (item->unlocked == 1 ? rotation : 0);
                DrawItem(blank, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                DrawItem(film, &positions[slot], scale, 1.0f, ypush, 0, angle, 0);
                break;
        }
    }
}

/* The top-level label follows all six shelves. */
void DrawItemMenu2D(void) {
    i32 text_id;
    if (ShopNameAlpha > 0.0f) {
        if (picked == 0) {
            text_id = TextIdPc880904;
        } else if (picked == 1) {
            text_id = TextIdPc88070c;
        } else if (picked == 2) {
            text_id = TextIdPc88090c;
        } else if (picked == 4) {
            text_id = TextIdPc88077c;
        } else if (picked == 5) {
            text_id = TextIdPc880800;
        } else {
            text_id = TextIdPc880908;
        }
        SmartTextEx(TTab[text_id], 0.0f, (f32)(((f64)HubEpisodeTitleY + HubEpisodeSubtitleY) * 0.5), 1.0f, 0.8f, 0.8f, 0.8f, 0, 255, 255,
                    255, 1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
    }
}

void DrawSubItemMenu2D(void) {
    shopitem_s *items;
    i32 *ids;
    char title[128];
    char subtitle[128];
    char *name;
    i32 buyable = 0;
    i32 show_name = 0;
    i32 id;
    i32 available;
    i32 price;
    f32 scale;
    f32 y;
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
    } else {
        return;
    }
    if (items == NULL) {
        return;
    }
    name = items[ids[3]].name;
    if (items[ids[3]].type == 1) {
        if (items[ids[3]].unlocked == 1) {
            show_name = 1;
            buyable = 0;
        } else if ((u16)items[ids[3]].item_id == VehicleBoltId7f17d4 ? ((SHOP_SAVE_VIEW *)&Game)->bolt_unlocked != 0
                                                                     : CollectIDUnlocked((u16)items[ids[3]].item_id) != 0) {
            buyable = show_name = 1;
        } else {
            scale = (f32)((f64)ShopLockedScale * (f64)0.7f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, (f32)((HubEpisodeTitleY + (f64)HubEpisodeSubtitleY) * 0.5), 1.0f, scale, scale, scale,
                        0, 255, 0, 0, 1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
        }
    } else if (items[ids[3]].type == 0) {
        show_name = 0;
        buyable = items[ids[3]].unlocked == 1 ? 0 : (u32)items[ids[3]].price > 0;
    } else if (items[ids[3]].type == 2) {
        id = (u16)items[ids[3]].item_id;
        // MATCH-EXCEPTION: the original selects the 16-bit text id before sign-extending it once, which the
        // casts reproduce; the plain ternary widens each arm separately.
        NuStrCpy(subtitle, TTab[(i16)(ExtrasCheats[id].text_id != NULL ? (u16)*ExtrasCheats[id].text_id : tUNKNOWN)]);
        name = subtitle;
        if (items[ids[3]].unlocked == 1) {
            show_name = 1;
            buyable = 0;
        } else {
            if (id < 8 || ExtrasCheats[id].area_index == -1 || Game.area_save[ExtrasCheats[id].area_index].red_brick_collected != 0) {
                buyable = show_name = 1;
            } else {
                sprintf(title, "%s %i", TTab[TextIdPc88074c], id - 7);
                scale = (f32)((f64)ShopLockedScale * (f64)0.6f);
                SmartTextEx(title, 0.0f, HubEpisodeTitleY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                            (i32)((f64)ShopNameAlpha * 128.0));
                SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleY, 1.0f, scale, scale, scale, 0, 255, 0, 0, 1.7f, 1, NULL, 0,
                            (i32)((f64)ShopNameAlpha * 128.0));
                buyable = show_name = 0;
            }
        }
    } else if (items[ids[3]].type == 4) {
        id = (u16)items[ids[3]].item_id;
        sprintf(title, "%s %i", TTab[TextIdPc880778], id + 1);
        name = title;
        if (items[ids[3]].unlocked == 1) {
            show_name = 1;
            buyable = 0;
        } else if (Game.game_time < id * 3600) {
            SmartTextEx(title, 0.0f, HubEpisodeTitleY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
            scale = (f32)((f64)ShopLockedScale * (f64)0.6f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleY, 1.0f, scale, scale, scale, 0, 255, 0, 0, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
            buyable = show_name = 0;
        } else {
            buyable = show_name = 1;
        }
    } else if (items[ids[3]].type == 5) {
        id = (u16)items[ids[3]].item_id;
        available = CutScenePlayer_CanStart(id);
        CutScenePlayer_GetText(id, title, subtitle, available);
        SmartTextEx(title, 0.0f, HubEpisodeTitleY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                    (i32)((f64)ShopNameAlpha * 128.0));
        if (available != 0 && shopcutsceneplayer != NULL) {
            SmartTextEx(subtitle, 0.0f, HubEpisodeSubtitleY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
        } else {
            scale = (f32)((f64)ShopLockedScale * (f64)0.6f);
            SmartTextEx(TTab[TextIdPc880708], 0.0f, HubEpisodeSubtitleY, 1.0f, scale, scale, scale, 0, 255, 0, 0, 1.7f, 1, NULL, 0,
                        (i32)((f64)ShopNameAlpha * 128.0));
        }
        buyable = show_name = 0;
    }
    if (ShopNameAlpha > 0.0f) {
        if (show_name != 0) {
            y = buyable != 0 ? HubEpisodeTitleY : (f32)(((f64)HubEpisodeSubtitleY + HubEpisodeTitleY) * 0.5);
            scale = buyable != 0 ? 0.6f : 0.8f;
            SmartTextEx(name, 0.0f, y, 1.0f, scale, scale, scale, 0, 255, 255, 255, 1.7f, 1, NULL, 0, (i32)((f64)ShopNameAlpha * 128.0));
        }
        if (buyable != 0) {
            switch (items[ids[3]].type) {
                case 1:
                    price = items[CharShelfIds[3]].price;
                    break;
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
                default:
                    price = items[CharShelfIds[3]].price;
                    break;
            }
            CoinTotal_Draw(price, HubEpisodeSubtitleY, 1.2f, 0, ShopNameAlpha, 255, 191, 0);
        }
    }
}

void DrawSubItemMenu3D(void) {
    DrawSubItems();
}

void DrawShop3D(WORLDINFO *world) {
    if (drawptr != NULL) {
        drawptr();
    }
    DrawTopShelf(picked);
    SetLevelLights(world->rtl_set, 1.0f);
}

void DrawShopPanel(void) {
    if (SHOPACTIVE != 0 && ShopDrawPanelFn != NULL) {
        ShopDrawPanelFn();
    }
}

static i32 Shop_CanSelect(void) {
    shopitem_s *item;
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    i32 id;
    i32 index;
    switch (picked) {
        case 1:
            if (Game.coins < (u32)CharItems[CharShelfIds[3]].price) {
                return 0;
            }
            item = &CharItems[CharShelfIds[3]];
            return Shop_CharacterUnlocked((u16)item->item_id) != 0 && item->unlocked != 1;
        case 0:
            index = HintShelfIds[3];
            return Game.coins >= (u32)HintItems[index].price && HintItems[index].unlocked != 1;
        case 2:
            if (Game.coins < (u32)ExtraItems[ExtraShelfIds[3]].price) {
                return 0;
            }
            item = &ExtraItems[ExtraShelfIds[3]];
            id = (u16)item->item_id;
            if (id >= 8 && ExtrasCheats[id].area_index != -1 && Game.area_save[ExtrasCheats[id].area_index].red_brick_collected == 0) {
                return 0;
            }
            return (save->extra_purchased_bits[id / 32] & (1u << (id & 31))) == 0 && item->unlocked != 1;
        case 4:
            index = BrickShelfIds[3];
            return Game.coins >= (u32)BrickItems[index].price && !(Game.game_time < index * 3600);
        case 5:
            return CutScenePlayer_CanStart(CutShelfIds[3]) != 0;
    }
    return 0;
}

void Shop_CollectAllCharacters(i32 mode) {
    SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
    i32 i;
    if (mode != 0) {
        return;
    }
    for (i = 0; i < ShopCollection.count && i < 100; ++i) {
        save->character_purchased_bits[i / 32] |= (u32)((u64)1 << (i % 32));
        if (WORLD->current_level == HUB_LDATA) {
            CharItems[i].unlocked = 1;
        }
    }
}

static void ShopMtxSetIdentity(NUMTX *matrix) {
    ShopMtxCopy(matrix, &ANI3_EVAL_IDENTITY_MATRIX);
}

/* Ignores the generic MENU confirm/cursor fields and permits selection of all
 * six shelves. */
i32 ItemMenu(void *menu) {
    SHOP_INPUT_PC input;
    i32 entry_picked = picked;
    i32 candidate = picked;
    i32 selection = -1;
    i32 cancel = 0;
    f32 reverse_phase;
    f32 old_factor;
    f32 forward_factor;
    i32 old_angle;
    i32 forward_angle;
    (void)menu;
    Shop_GetInput(&input);
    if (ShopSlideTimer >= 0.0f) {
        ShopSlideTimer -= FRAMETIME;
        ShopNameAlpha = 0.0f;
    }
    if (ShopExit != 0) {
        ShopNameAlpha = 0.0f;
    }
    ShopItemChanged = 0;
    Hint_CancelCurrent();
    ShopHintDrawWait = 0.2f;
    if (ShopSlideTimer < 0.0f) {
        ShopMoveSfxLock = 0;
        if (input.left_pressed != 0 && picked > 0) {
            candidate = picked - 1;
        } else if (input.right_pressed != 0 && picked < 5) {
            candidate = picked + 1;
        } else if (input.confirm != 0) {
            selection = picked;
        } else if (input.cancel != 0) {
            cancel = 1;
        }
    }
    if (candidate != entry_picked) {
        ShopSubMenu = 0;
        ShopLastItem = picked;
        picked = candidate;
        if (candidate < 0) {
            picked = 0;
        } else if (candidate > 5) {
            picked = 5;
        }
        ShopSlideTimer = 0.125f;
        if (ShopMoveSfxLock == 0) {
            ShopMoveSfxLock = 1;
            GameAudio_PlaySfx(0x2f, &SubShelfPos[5], 0, 0);
        }
        ShopNameAlpha = 0.0f;
        GameCam_Blend(GameCam, 0.3f, 0.0f, 1);
    }
    if (selection != -1) {
        if (selection != picked) {
            picked = selection;
            if (ShopMoveSfxLock == 0) {
                ShopMoveSfxLock = 1;
                GameAudio_PlaySfx(0x2f, &SubShelfPos[5], 0, 0);
            }
            ShopNameAlpha = 0.0f;
        }
        ShopSlideTimer = 0.125f;
        ShopLastItem = picked;
        picked = -1;
        ShopSubMenu = 1;
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        GameCam_Blend(GameCam, 0.6f, 0.0f, 1);
        ShopNameAlpha = 0.0f;
    }
    if (cancel != 0) {
        ShopSlideTimer = 0.125f;
        ShopExit = 1;
        ShopLastItem = picked;
        picked = -1;
        ShopSubMenu = 0;
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
    }
    if (ShopSlideTimer >= 0.0f) {
        if ((ShopLastItem != picked || ShopEntered != 0) && ShopSubMenu == 0) {
            reverse_phase = (f32)((f64)ShopSlideTimer * 8.0);
            old_factor = 1.0f - reverse_phase;
            old_angle = (i32)((f64)old_factor * 32768.0 + 16384.0);
            old_factor = (f32)(1.0 - ((f64)NuTrigTable[(old_angle >> 1) & 0x7fff] + 1.0) * 0.5);
            forward_angle = (i32)((f64)reverse_phase * 32768.0 + 16384.0);
            forward_factor = (f32)(1.0 - ((f64)NuTrigTable[(forward_angle >> 1) & 0x7fff] + 1.0) * 0.5);
            if (ShopLastItem != -1) {
                old_factor = SHOP_CLAMP01(old_factor);
                topscale[ShopLastItem] = TopBigScale[ShopLastItem] + (TopShelfScale[ShopLastItem] - TopBigScale[ShopLastItem]) * old_factor;
                forward_factor = SHOP_CLAMP01(forward_factor);
                toppush[ShopLastItem] = TopBigPush[ShopLastItem] + (TopShelfPush[ShopLastItem] - TopBigPush[ShopLastItem]) * forward_factor;
            }
            if (picked != -1 && ShopLastItem != -1) {
                old_factor = SHOP_CLAMP01(old_factor);
                topscale[picked] = TopShelfScale[picked] + (TopBigScale[picked] - TopShelfScale[picked]) * old_factor;
                forward_factor = SHOP_CLAMP01(forward_factor);
                toppush[picked] = TopShelfPush[picked] + (TopBigPush[picked] - TopShelfPush[picked]) * forward_factor;
            }
        }
    } else {
        ShopEntered = 0;
    }
    if (ShopSubMenu != 0 && ShopSlideTimer <= 0.0f) {
        ShopInOutScale = 0.0f;
        picked = ShopLastItem;
        ShopSlideTimer = 0.125f;
        ++ShopMenuLevel;
        ShopMenuParent[ShopMenuLevel] = menuptr;
        ShopSubMenu = 0;
        ShopEaseSubIn = 1;
        if (TopShelf[ShopLastItem].type == 3) {
            memset(ShopUserCode, 'A', 6);
            ShopMenu = 11;
            ShopColumn = 0;
            ShopLastItem = -1;
            menuptr = CodeMenu;
            ShopCheatName = NULL;
            ++ShopDrawLevel;
            ShopDrawParent[ShopDrawLevel] = ShopDrawPanelFn;
            ShopDrawPanelFn = DrawCodeMenu;
            drawptr = DrawCodeMenu3D;
        } else {
            menuptr = SubItemMenu;
            ++ShopDrawLevel;
            ShopDrawParent[ShopDrawLevel] = ShopDrawPanelFn;
            ShopDrawPanelFn = DrawSubItemMenu2D;
            drawptr = DrawSubItemMenu3D;
        }
    }
    if (ShopExit != 0 && ShopSlideTimer < 0.0f) {
        menuptr = NULL;
        return 5;
    }
    return 7;
}

/* Initializes to shelf zero and keeps the complete cutscene-player
 * return state. */
i32 UpdateShop(void *menu) {
    i32 i;
    UpdateCharacterIDs();
    ShopScaleOverride[0] = 0.0f;
    ShopScaleOverride[1] = 0.4f;
    ShopScaleOverride[2] = 0.8f;
    ShopScaleOverride[4] = 0.8f;
    ShopScaleOverride[5] = 0.4f;
    ShopScaleOverride[6] = 0.0f;
    if (ShopNameAlpha < 1.0f) {
        ShopNameAlpha += FRAMETIME * 2.0f;
        if (ShopNameAlpha > 1.0f) {
            ShopNameAlpha = 1.0f;
        }
    }
    ShopLockedScale = SeekLinearF(ShopLockedScale, 1.0f, (f32)((f64)FRAMETIME * 3.0));
    SubNormCharPush = ShopNativeSubNormCharPush;
    SubBigCharPush = ShopCodeExitPush;
    if (menuptr == NULL) {
        if (shop_from_cutsceneplayer != 0) {
            shop_from_cutsceneplayer = 0;
            topscale[5] = ShopOldPickedScale;
            menuptr = ShopOldMenuFn;
            toppush[5] = ShopOldPickedPush;
            for (i = 0; i < 3; ++i) {
                ShopDrawParent[i] = ShopOldDrawParent[i];
                ShopMenuParent[i] = ShopOldMenuParent[i];
            }
            ShopDrawPanelFn = ShopOldDrawPanelFn;
            drawptr = ShopOldDrawFn;
            ShopMenuLevel = ShopOldMenuLevel;
            ShopDrawLevel = ShopOldDrawLevel;
            picked = oldpicked;
        } else {
            ShopSlideTimer = 0.125f;
            menuptr = ItemMenu;
            ShopExit = 0;
            ShopEntered = 1;
            picked = 0;
            ShopLastItem = 0;
            ShopDrawPanelFn = DrawItemMenu2D;
            ShopDrawLevel = 0;
            ShopMenuParent[0] = NULL;
            ShopDrawParent[0] = NULL;
            ShopMenuLevel = 0;
        }
    }
    return DoShopMenu(menu) != 0;
}

void DrawQuestion(NUVEC *position, f32 scale_value, f32 y_push) {
    NUMTX matrix;
    NUVEC scale;
    if (position != NULL && NuSpecialExistsFn(&question)) {
        ShopMtxSetIdentity(&matrix);
        scale.x = scale.y = scale.z = scale_value;
        NuMtxSetScale(&matrix, &scale);
        *(NUVEC *)&matrix.m30 = *position;
        matrix.m31 += y_push;
        NuSpecialDrawAt(&question, &matrix);
    }
}

/* The affordability predicate, separate from the prompt drawing; it takes
 * both players' prompt arguments. */
void DrawShopPrompts(void) {
    i32 select = -1;
    i32 back = -1;
    f32 alpha = 1.0f;
    if (menuptr == ItemMenu) {
        back = TextIdPc880844;
        select = TextIdPc880888;
    } else if (menuptr == SubItemMenu) {
        select = picked == 5 ? TextIdPc8808c0 : TextIdPc8808c4;
        if (Shop_CanSelect() == 0) {
            alpha = 0.25f;
        }
        back = tBACK;
    } else if (menuptr == CodeMenu) {
        back = tBACK;
        select = TextIdPc880888;
    }
    if (netclient != 0) {
        select = -1;
    }
    if (select != -1 || back != -1) {
        DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], select, alpha, -1, back, -1, TextIdPc880eac,
                              MissionMenuPacket.active_player[1], select, alpha, -1, back, -1, TextIdPc880eac);
    }
}

void GetShopCamLookPos(NUVEC *position) {
    if (menuptr == ItemMenu && splshelf != NULL && picked != -1) {
        *position = splshelf->pts[picked];
    } else {
        *position = *shopcamlookat;
    }
}
