#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/text_tables.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 StatusCollectionIconY;
    extern f32 StatusCollectionTextY;
    extern f32 StatusCollectionY;
    extern i32 StatusCollectionAppearSoundPlayed;
    extern i32 StatusCollectionMoveSoundPlayed;
    extern f32 StatusMiniKitEntranceHeight;
    extern f32 StatusMiniKitScale;
    extern f32 StatusMiniKitX;
    extern f32 StatusMiniKitY;
    extern i16 dat_00880728;
    extern i16 dat_00880f9c;
    extern i16 dat_00880720;
    extern i16 dat_00880738;
    extern i16 dat_00880734;
    extern f32 StatusIconY;
    extern f32 STATSPOS2Y;
    extern i32 StatusDrawPlayerIcons;
    extern f32 StatusTrueHeroBarScale;
    extern f32 StatusTrueHeroBarY;
    extern f32 StatusTrueHeroTextScale;
    extern f32 StatusTrueHeroTextY;
    extern f32 StatusTrueHeroNextSoundPercent;
    extern f32 StatusTrueHeroSpDelay;
    extern f32 StatusTitleY;
    extern f32 StatusSecondaryTitleY;
    extern i16 dat_008806e8;
    extern i16 dat_00880710;
    extern i16 dat_00880730;
    extern i16 dat_0088100c;
    extern i16 dat_00881014;
    extern i16 dat_0088101c;
    extern i16 dat_00881024;
    extern i16 dat_0088102c;
    extern i16 dat_00881034;
    extern i16 dat_0088103c;
    extern i16 dat_008810cc;
    extern i16 dat_008810d0;
    extern f32 HudScoreScale;
    extern f32 StatusCoinsTotalY;
    extern f32 StatusCoinsScale;
    extern f32 StatusCoinsTitleScale;
    extern f32 StatusCoinsTitleY;
    extern f32 StatusTextScale;
    extern i32 StatusRed;
    extern i32 StatusGreen;
    extern i32 StatusBlue;
    extern f32 StatusAllMiniKitsTitleScale;
    extern f32 StatusAllMiniKitsCharacterTextScale;
    extern f32 StatusAllMiniKitsX;
    extern f32 StatusAllMiniKitsY;
    extern f32 StatusAllMiniKitsScale;
    extern f32 StatusAllMiniKitsZ;
    extern f32 StatusAllMiniKitsCountYOffset;
    extern f32 StatusAllMiniKitsCountScale;
    extern f32 StatusAllMiniKitsQuestionScale;
    extern f32 StatusAllMiniKitsQuestionYOffset;
    extern f32 StatusAllMiniKitsCompleteScale;
    extern f32 StatusAllMiniKitsCompleteYOffset;
#ifdef __cplusplus
}
#endif

typedef struct BONUS_GUNSHIP_FLAGS { // size 0x10
    u8 progress;                     // 0x0
    u8 secondary;                    // 0x1
    u8 reserved[14];                 // 0x2
} BONUS_GUNSHIP_FLAGS;

typedef struct KAMINO_SHIP_ZERO_ANGLES {
    u16 pitch;
    i32 yaw;
} KAMINO_SHIP_ZERO_ANGLES;

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

typedef struct TAG_CHARACTER_DATA_READER {
    u8 opaque_00[0x42];
    i16 icon_model;
    u8 opaque_44[8];
} TAG_CHARACTER_DATA_READER;

typedef struct cutinfo_reader_s {
    u8 opaque_00_04[0x4];
    void *instance;
    u8 opaque_08_54[0x4c];
    f32 previous_frame;
} CUTINFO_READER;

typedef struct kaminoc_packet_s {
    i16 off_mask;
    i16 pending_mask;
    i16 occupied_mask;
    i16 complete_mask;
    i16 final_mask;
    i16 sound_mask;
    u8 complete;
    u8 reserved_0d;
} KAMINOC_PACKET;

typedef struct {                   // size 0x8
    BOMB_PROGRESS_RECORD *records; // 0x0
    u16 count;                     // 0x4
} BOMB_PROGRESS_SYSTEM;

typedef struct {                    // size 0x8
    i32 count;                      // 0x0
    PANEL_PROGRESS_RECORD *records; // 0x4
} PANEL_PROGRESS_SYSTEM;
