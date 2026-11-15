#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameobject_s;

typedef void (*GAME_MESSAGE_CALLBACK)(struct game_message_s *);

/* add_game_message_s.flags, game_message_s.flags: what UpdateGameMessages, TransformGameMessages and
 * DrawGameMessages do with a message. The easing bits pick the curve of the move and scale progress. */
#define GAMEMSG_WORLD_POSITION 0x1      /* the position is in the world: projected to the screen */
#define GAMEMSG_FOLLOW 0x2              /* projected every frame, not only the first */
#define GAMEMSG_WORLD_SCALE 0x4         /* the scale is a world width, projected with the position */
#define GAMEMSG_DRAW_MOVE 0x8           /* drawn on the way from the position to target_position */
#define GAMEMSG_MOVE 0x10               /* the position moves to target_position over the duration */
#define GAMEMSG_DRAW_SCALE 0x20         /* drawn scaled on the way to target_scale */
#define GAMEMSG_SCALE 0x40              /* the scale moves to target_scale over the duration */
#define GAMEMSG_ONE_FRAME 0x80          /* no duration: expires at its first update */
#define GAMEMSG_EASE_IN 0x100           /* progress 1 - cos(t * 90) */
#define GAMEMSG_EASE_OUT 0x200          /* progress sin(t * 90) */
#define GAMEMSG_ARC 0x400               /* progress sin(t * 180): out and back */
#define GAMEMSG_DIP 0x800               /* progress 1 - sin(t * 180) */
#define GAMEMSG_KEEP 0x1000             /* never taken over when all 128 slots are in use */
#define GAMEMSG_TEXT_POINTER 0x2000     /* the text is kept as a pointer, not copied */
#define GAMEMSG_FADE 0x4000             /* the alpha fades out linearly */
#define GAMEMSG_FADE_SMOOTH 0x8000      /* the alpha fades out along a cosine */
#define GAMEMSG_OBJECT_ALPHA 0x10000    /* an object message is drawn with the message's alpha */
#define GAMEMSG_OBJECT_NO_ALPHA 0x20000 /* an object message is drawn with DrawPanel3DObjectNoAlpha */
#define GAMEMSG_BEHIND_CAMERA 0x40000   /* still drawn when the position is behind the camera */

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct add_game_message_s {  // size 0x50
        const char *text;                // 0x00
        NUVEC *position;                 // 0x04
        NUVEC *target_position;          // 0x08
        f32 scale;                       // 0x0c
        f32 target_scale;                // 0x10
        u8 red;                          // 0x14
        u8 green;                        // 0x15
        u8 blue;                         // 0x16
        u8 alpha;                        // 0x17
        u32 flags;                       // 0x18
        f32 duration;                    // 0x1c
        f32 delay;                       // 0x20
        u16 unknown_24;                  // 0x24
        i16 icon;                        // 0x26
        void *special;                   // 0x28
        u32 score;                       // 0x2c
        f32 unknown_30;                  // 0x30
        f32 unknown_34;                  // 0x34
        GAME_MESSAGE_CALLBACK delay_fn;  // 0x38
        GAME_MESSAGE_CALLBACK tick_fn;   // 0x3c
        GAME_MESSAGE_CALLBACK update_fn; // 0x40
        void *unknown_44;                // 0x44
        GAME_MESSAGE_CALLBACK end_fn;    // 0x48
        i8 player_index;                 // 0x4c
        u8 allow_temporary;              // 0x4d
        u8 target_is_coin_total;         // 0x4e
        u8 unknown_4f;                   // 0x4f
    } ADD_GAME_MESSAGE;

    extern ADD_GAME_MESSAGE AddGameMsg_Default;
#ifdef __cplusplus
}
#endif

struct game_message_s {              // size 0x114
    const char *text;                // 0x000
    char text_buffer[0x78];          // 0x004
    NUVEC position_a;                // 0x07c
    NUVEC target_position;           // 0x088
    NUVEC position;                  // 0x094
    NUVEC start_position;            // 0x0a0
    f32 scale_start;                 // 0x0ac
    f32 target_scale;                // 0x0b0
    f32 scale_initial;               // 0x0b4
    f32 scale_current;               // 0x0b8
    f32 elapsed;                     // 0x0bc
    f32 duration;                    // 0x0c0
    f32 parameter_c4;                // 0x0c4
    f32 parameter_c8;                // 0x0c8
    f32 scale_multiplier;            // 0x0cc
    f32 delay;                       // 0x0d0
    f32 parameter_d4;                // 0x0d4
    u32 flags;                       // 0x0d8
    u32 score;                       // 0x0dc
    u16 parameter_e0;                // 0x0e0
    u16 parameter_e2;                // 0x0e2
    u16 parameter_e4;                // 0x0e4
    u16 icon;                        // 0x0e6
    nuhspecial_s special;            // 0x0e8
    u8 red;                          // 0x0f4
    u8 green;                        // 0x0f5
    u8 blue;                         // 0x0f6
    u8 alpha;                        // 0x0f7
    u8 active;                       // 0x0f8
    u8 has_start_position;           // 0x0f9
    u8 expired;                      // 0x0fa
    u8 parameter_fb;                 // 0x0fb
    u8 parameter_fc;                 // 0x0fc
    i8 player_index;                 // 0x0fd
    u8 target_is_coin_total;         // 0x0fe
    u8 allow_temporary;              // 0x0ff
    GAME_MESSAGE_CALLBACK delay_fn;  // 0x100
    GAME_MESSAGE_CALLBACK tick_fn;   // 0x104
    GAME_MESSAGE_CALLBACK update_fn; // 0x108

    union { // size 0x4
        void *parameter_10c;
        void (*draw_fn)(struct game_message_s *, NUVEC *, f32);
    }; // 0x10c

    GAME_MESSAGE_CALLBACK end_fn; // 0x110
};

#ifdef __cplusplus
extern "C" {
#endif
    extern GAME_MESSAGE GameMessage[128];
    void Pup_CollectCoin(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_UpdateBlueCoin(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_UpdatePurpleCoin(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_CollectMinikit(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectHeart(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectRedBrick(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectCharKit(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_UpdatePowerUp(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_CollectPowerUp(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void CollectRedBrick(NUVEC *);
    void SetCoinType(i32, GIZMOPICKUP *);
    extern char dat_00758c00[9];
    extern char dat_00758c0c[8];
    extern char dat_00758c14[10];
    extern char dat_00758c20[8];
    extern char dat_00758c28[10];
    extern char dat_00758c34[6];
    extern char dat_00758c3c[10];
    extern char dat_00758c48[8];
    extern char dat_00758c50[12];
    extern char dat_00758c5c[12];
    extern char dat_00758c68[10];
    extern char dat_00758c74[10];
    extern char dat_00758c80[12];
    extern GIZMO_PICKUP_TYPE GizmoPickupType[10];

    void AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction, f32 unused, i32 player_index,
                    f32 speed, f32 lifetime, struct gameobject_s *owner, i32 random_types, i32 panel);
    void AddCoinsAsParts(i32 coins, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddHeartAsPart(struct gameobject_s *owner, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddTorpedoAsPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void PowerUp_AddPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    extern f32 PANEL_COINSCALE_START;
    extern f32 PartExtraCoinRate;
    extern f32 PartVehicleHeartLifetime;
    extern f32 PartVehicleHeartPodraceHeight;
    extern f32 PartVehicleHeartHeight;
    extern f32 PartVehicleHeartPodraceScale;
    extern f32 PartVehicleHeartGunshipScale;
    extern f32 PartVehicleHeartScale;
    extern f32 coinimpactwait;
    extern const char PowerUpBounceSfx[21];
    extern const char CoinDropGoldSfx[10];
    void AddTorpedoAsPart(NUVEC *, NUVEC *, f32, f32);
    void AddHeartAsPart(struct gameobject_s *, NUVEC *, NUVEC *, f32, f32);
    void PowerUp_AddPart(NUVEC *, NUVEC *, f32, f32);
    void AddCoinsAsParts(i32, NUVEC *, NUVEC *, f32, f32);
#define PickupCoinName PickupCoinSfx // NOLINT(readability-identifier-naming)
#define PickupCoinBName dat_00758c5c // NOLINT(readability-identifier-naming)
    void AddCoinsToPanel(i32 coins, NUVEC *position, i32 player_index, f32 scale, struct gameobject_s *object, i32 mode);
    i32 LoseCoins(struct gameobject_s *object, i32 cause);
    extern char ScorePanelName[9];
    extern char ScoreCounterName[8];
    extern char PickupCoinsLandName[10];
    extern u8 CoinTab[4];
    extern f32 AddCoinDelay[2];
    extern const f64 PlayerSlideSpeed;
    extern const f64 PlayerTightRopePhase;
    void PowerUp_Update(struct gameobject_s *);
    extern char ScorePanelName[9];
    extern char dat_00758c14[10];
    void GizmoPickups_SetOnOff(void);
#ifdef __cplusplus
}
#endif
