#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/objects/game_obj.h"
#include "legoapi/lib/hub.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct REGISTER_STATUS_PACKET;
struct STATUSPACKET_s;
struct STATUS_STAGE_s;
struct WORLDINFO_s;

typedef struct PAD_INPUT {
    u8 unknown_00[0x92];
    u8 analog_left_x;
    u8 analog_left_y;
} PAD_INPUT;

typedef struct RUMBLE_PACKET_RESOURCE { // size 0x14

    union { // size 0x4
        u8 unknown_00[4];
        f32 intensity;
    }; // 0x00

    f32 amount; // 0x04

    union { // size 0x4
        u8 unknown_08[4];
        f32 decay_duration;
    }; // 0x08

    f32 duration; // 0x0c
    u8 active;    // 0x10
} RUMBLE_PACKET_RESOURCE;

typedef struct STATUS_RUMBLE_PAIR {
    RUMBLE_PACKET_RESOURCE players[2];
} STATUS_RUMBLE_PAIR;
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f17d0;
/* Windows SDK commctrl.h aliases DrawStatusText to DrawStatusTextA. */
#ifdef DrawStatusText
#undef DrawStatusText
#endif
    void DrawStatusText(char *, u16, f32, f32, f32, u32, i32);
    extern const char RenderSplineBountyStart[13];
    extern f32 DrawStatusScaleX;
    extern f32 DrawStatusScaleY;

    /* Additive shared contracts only; existing RTL/flight/game/player types reused. */
    extern NUVEC PanelLightingDirections[3];

    extern i32 StatusNewMinikitCount;
    extern i32 currentminikit;
    extern i16 StatusNewCharacterIds[8];
    extern u32 StatusCollectedSuitBits;
    extern f32 StatusMiniKitDirectionOffset;
    extern f32 slideseek;
    void AddStatusMiniKitParts(void);
    extern f32 jibberlen;
    extern f32 slidetime;
    extern f32 pop_timer;
    extern i32 PauseSaveAndExit;
    f32 StatusScoreRatio(f32, f32);
    extern i32 StatusDemoOption;
    void StatusRumbleUpdate(void);
    void FinishStatusPacket(i32);
    extern i32 DrawGoldBrick_Stage;
    extern i32 DrawGoldBrick_Phase;
    f32 StatusIconsOnOff(f32);
    void NewStatusRumbleBuzz(i32, f32, f32, i32);
#ifdef __cplusplus
}
#endif

typedef struct minikit_piece_s { // size 0x68
    nuhspecial_s special;        // 0x00
    NUMTX matrix;                // 0x0c
    u8 unknown_4c[0x18];         // 0x4c
    u8 direction;                // 0x64
    u8 unknown_65_b[0x1];        // 0x65
    u8 enabled;                  // 0x66
    u8 direction_index;          // 0x67
} MINIKIT_PIECE;

typedef struct status_minikit_draw_cache_s { // size 0x48
    NUMTX matrix;                            // 0x00
    MINIKIT_PIECE *piece;                    // 0x40
    i32 active;                              // 0x44
} STATUS_MINIKIT_DRAW_CACHE;

#ifdef __cplusplus
extern "C" {
#endif
    extern COLLECTION_BUILD MasterCollection;
    extern STATUS_MINIKIT_DRAW_CACHE StatusMiniKitDrawCache[10];
    extern PART_LIGHTING_DATA StatusMiniKitPartLighting;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern AREA_DATA *GOTHAMSTREETS_ADATA;
    void StatusStage_Reset(struct STATUS_STAGE_s *stage);
    void NextStatusStage(struct STATUSPACKET_s *);
    void RegisterStatusScreen(struct STATUS_STAGE_s *, i32 *, const struct REGISTER_STATUS_PACKET *);
    void DrawStatusMiniKit(f32, f32, f32, f32, f32, i32, struct STATUSPACKET_s *, f32);
    i32 UpdateNewMiniKits(struct STATUSPACKET_s *, struct STATUS_STAGE_s *);
    void StatusPacketReset(struct STATUSPACKET_s *);
    i32 newCharactersCollected(struct STATUSPACKET_s *);
    void InitStatusScreen(struct WORLDINFO_s *);
    extern AREA_DATA *NEGOTIATIONS_ADATA;
    void SetDrawGoldBrick(struct STATUSPACKET_s *, i32);
    void DrawBonusTime(struct STATUSPACKET_s *, f32, i32);
    extern struct STATUS_STAGE_s *StatusStages;
    void DrawStatusTextFraction(i32, i32, f32, f32, u16, f32, u32, f32, f32);
    void Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);
#ifdef __cplusplus
}
#endif
