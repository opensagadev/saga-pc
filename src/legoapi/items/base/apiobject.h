#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/characters/motion/charshadows.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/edsplines.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct AIPATHCNX;
struct AIPATHNODE;
struct AITRIGGERSET_PARTIAL;
struct GAMEDEBRISENTRY;
struct GAMEPAD;
struct PLAYERCHARACTERCONFIG;
struct SUIT_LAYERS;
struct TUBE;
struct ai_helper_slot_s;
struct apicharactermodellist_s;
struct blowup_target_s;
struct bolt_s;
struct characterdata_s;
struct climb_slot_s;
struct gizforce_s;
struct guideline_record_s;
struct numtl_s;
struct player_force_los_slot_s;
struct techno_s;

struct gameobject_s;

struct gameobject_s;
typedef struct AIPACKET AIPACKET;

struct gameobject_s;

// Only fields with confirmed PC offsets are exposed.
typedef struct APIOBJECTSYS {     // size 0x218
    u32 object_size;              // 0x000
    struct gameobject_s *objects; // 0x004
    u64 line_of_sight[64];        // 0x008
    i32 los_cursor_208;           // 0x208
    i32 los_cursor_20c;           // 0x20c

    union { // size 0x8

        struct {                   // size 0x8
            u8 flags;              // 0x210
            u8 state_211[3];       // 0x211
            u8 unknown_214_b[0x4]; // 0x214
        };

        struct {               // size 0x8
            u32 flags_word;    // 0x210
            u8 unknown_214[4]; // 0x214
        };
    }; // 0x210
} APIOBJECTSYS;

typedef struct CHARACTER_ANIMATION_LOCATOR { // size 0x4c
    u8 unknown_00[0x14];                     // 0x00
    f32 blend_out_time;                      // 0x14
    f32 playback_rate;                       // 0x18
    f32 movement_speed;                      // 0x1c
    f32 movement_rate_cap;                   // 0x20
    f32 action_speed;                        // 0x24
    u8 unknown_28[0x20];                     // 0x28
    u16 misc_flags;                          // 0x48
    i8 locator;                              // 0x4a
} CHARACTER_ANIMATION_LOCATOR;

typedef struct API_ANIMATION_CACHE { // size 0x34
    char path[0x30];                 // 0x00
    void *animation;                 // 0x30
} API_ANIMATION_CACHE;

typedef struct TORPEDOPACKET { // size 0x88
    u8 count;                  // 0x00
    u8 flags;                  // 0x01
    u8 ricochet_count;         // 0x02
    u8 pickup_state;           // 0x03
    u8 target_type;            // 0x04
    u8 unknown_05[3];          // 0x05
    f32 steal_timer;           // 0x08
    f32 ricochet_time;         // 0x0c
    u8 unknown_10[4];          // 0x10
    u32 pickup_data[5];        // 0x14
    u32 pickup_flags[5];       // 0x28
    NUVEC pickup_positions[5]; // 0x3c
    NUVEC ricochet_position;   // 0x78
    void *target;              // 0x84
} TORPEDOPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 drawcharactermodel_nobsa;
    extern i32 drawcharactermodel_noani;
    extern i32 drawcharactermodel_restpose;
    extern i32 drawcharactermodel_keepmergeaction;
    extern i32 drawcharactermodel_locatorsupdated;
    extern u8 TempNumJoints;
    extern i32 notransparentchardraw;
    extern void (*APIObjResetShadowMapRenderingFn)(void);
    extern void (*APIObjEnableShadowMapRenderingFn)(void);

    typedef struct API_ACTION_INFO { // size 0x8
        const char *name;            // 0x0
        u32 flags;                   // 0x4
    } API_ACTION_INFO;

    typedef struct API_EXTRA_ACTION { // size 0x8
        const char *name;             // 0x0
        i32 action;                   // 0x4
    } API_EXTRA_ACTION;

    void PCCharacterModelDiagnosticNoOp();
    extern i32 nu_keyboard_no_key_state;
    void *LoadAnimFromPAK(char *path, i32 clip, void *pak, i32 flags);
    void *LoadAnim(char *path, i32 clip, VARIPTR *buffer, VARIPTR end);
    void APIObjectDestroyAll(APIOBJECTSYS *system);
    extern void (*APIObjPlaySfxByIdFn)(i32, NUVEC *);
    extern const char WorldGrassFormat[7];
    extern void *RedirectAnimList;
    extern char RedirectAnimDir[64];
    void PCCharacterSystemExistingNoOp();
    void SetAPIObjPlaySfxByIdFn(void (*)(i32, NUVEC *));
    void APIDumpCharacterModels(i32 mode);
#ifdef __cplusplus
}
#endif

/* gameobject_s.flags_1fc (and its _half/_word views): the bits whose setter or test says what they mean. */
#define APIOBJECT_FLAG_IN_USE 0x1         /* the object slot is live */
#define APIOBJECT_FLAG_DONT_PUSH 0x2      /* from CHARACTER_MODEL_DONT_PUSH and the GameDontPush AI action */
#define APIOBJECT_FLAG_SHADOW_TERRAIN 0x8 /* the ShadowTerrain AI action */
#define APIOBJECT_FLAG_PLAYER_ACTIVE 0x80 /* set when a player drops in, cleared when it drops out */
#define APIOBJECT_FLAG_CAN_FIRE 0x200     /* copy of CHARACTER_MODEL_CAN_FIRE for an active player */
#define APIOBJECT_FLAG_CHARACTER 0x1000

/* gameobject_s.flags_1f8: the SetSide AI action (neutral, baddie, goodiebaddie; goodie sets none; default takes the
 * creature's CHARACTER_MODEL_NEUTRAL or _BADDIE), the NoShadows action, and creatures made by AddCreature. */
#define APIOBJECT_SIDE_BADDIE 0x1
#define APIOBJECT_SIDE_NEUTRAL 0x4
#define APIOBJECT_CREATURE 0x400
#define APIOBJECT_NO_SHADOWS 0x2000
#define APIOBJECT_SIDE_GOODIE_BADDIE 0x10000

/* gameobject_s.flags_e20: the bits whose setter or test says what they mean. The force bits are set by
 * FindForcePushTarget from the character's CHARACTER_RUNTIME_CHOKE and _LIGHTNING and the target's take-over
 * animation (the mind trick sound plays for it); highlight-only is the UseForce AI action's forcepush_highlight_only
 * keyword. */
#define APIOBJECT_FORCE_CHOKE 0x80           /* ForceChokeCrunch, the target plays ACTION_CHOKED */
#define APIOBJECT_FORCE_LIGHTNING 0x100      /* the target plays ACTION_ZAPPED */
#define APIOBJECT_FORCE_TAKE_OVER 0x200      /* ForceMindTrick */
#define APIOBJECT_FORCE_HIGHLIGHT_ONLY 0x400 /* UseForce forcepush_highlight_only */
#define APIOBJECT_ATTACK_TARGET_SET 0x800    /* SetObjTarget, SetGizmoBlowUpTarget, SetPartTarget: aim at attack_target_position */
#define APIOBJECT_BOLT_ON_COURSE 0x2000      /* the incoming bolt's line passes through the object (dodge it) */
#define APIOBJECT_WEAPON_OUT 0x10000         /* SetWeaponOut/KeepWeaponOut set it, SetWeaponIn clears it */

/* gameobject_s.flags_e20, more runtime bits from their setters. */
#define APIOBJECT_LAUNCH_ROCKET 0x10            /* set at the JgRocket release frame; GameObjectStuffAfterAnimation fires the rocket part */
#define APIOBJECT_COMBO_IN_TIME 0x40            /* sabre combo: no mistimed press yet, the next hit takes the special branch */
#define APIOBJECT_DETONATOR_WAIT_RELEASE 0x8000 /* Detonator_MoveCode ignores the held special button until it is released */
#define APIOBJECT_SHOT_FIRED 0x40000            /* the shot of the current shoot animation has been fired */

/* gameobject_s.context_variant_flags: the bits whose meaning is the same in every character context; the others
 * mean something else per context. */
/* APIOBJECT_CONTEXT_EVENT_DONE: the context's action has happened (its animation's event frame or the timer); cleared
 * when the context starts. */
#define APIOBJECT_CONTEXT_EVENT_DONE 0x40
#define APIOBJECT_CONTEXT_INVULNERABLE 0x20000 /* hits do nothing while the context's info has the flag for it */

/* gameobject_s.context_variant_flags in the jump context (StartJump, StartEndOfJump, StartComboJump, StartLunge). */
#define APIOBJECT_JUMP_FLIP 0x2000    /* StartComboJump's flip; the landing plays LEGOACT_FLIPLAND */
#define APIOBJECT_JUMP_DOUBLE 0x4000  /* the second jump has started; the landing may play LEGOACT_LAND3 */
#define APIOBJECT_JUMP_FALLING 0x8000 /* StartEndOfJump, the combat roll's and the super carry jump's time is up */
#define APIOBJECT_JUMP_ATTACK 0x10000 /* StartJump in movement states 3 and 4, StartLunge */

/* gameobject_s.context_variant_flags in the punch context (PunchCode). */
#define APIOBJECT_PUNCH_COMBO 0x1          /* a combo style was chosen: queued_context_animation follows */
#define APIOBJECT_PUNCH_BEHIND 0x80        /* ACTION_PUNCH_BEHIND, the target is behind */
#define APIOBJECT_PUNCH_SPECIAL_MOVE 0x200 /* SpecialMove_Check found one: special_move */

/* gameobject_s.context_variant_flags in the sabre combo context (LightSabreComboCode). */
#define APIOBJECT_COMBO_ENDED 0x2   /* the combo context ended (animation end, blend-out or no press in time) */
#define APIOBJECT_COMBO_WAITING 0x4 /* between hits: a press within combo_input_timer continues the combo */

/* gameobject_s.context_variant_flags in the grapple and teleport contexts. */
#define APIOBJECT_GRAPPLE_CLIMB 0x100    /* hanging on a grapple: facing it and pulling up (the special button toggles it) */
#define APIOBJECT_TELEPORT_REVERSE 0x400 /* the teleport path is travelled from its other end */

/* gameobject_s.tag_flags, from the tagging code (legoapi_tag.c) and the SetTaggable AI action. */
#define APIOBJECT_TAG_DISABLED 0x2         /* SetTaggable FALSE: cannot be tagged to or dropped in on */
#define APIOBJECT_TAG_PENDING 0x4          /* a tag to pending_tag_target waits for tag_cooldown */
#define APIOBJECT_TAG_PENDING_TAKEOVER 0x8 /* the pending tag is a take-over (TagCode's takeover) */

/* gameobject_s.sabre_flags, per blade in the sabre collision code */
#define SABRE_PUSH 0x1   /* the blade pushes the characters it touches */
#define SABRE_STREAK 0x2 /* the blade draws its streak */
#define SABRE_DAMAGE 0x4 /* the blade hurts (ComboHitFrame) */

/* gameobject_s.helmet: the hat a hat machine put on (0 none); two of them work as disguises */
#define HELMET_NONE 0
#define HELMET_STORMTROOPER 5  /* opens stormtrooper panels, the baddies forget the party */
#define HELMET_BOUNTY_HUNTER 6 /* opens bounty hunter panels */

/* gameobject_s.unknown_281: the floor test of the character update (0 while airborne) */
#define GROUND_CONTACT 0x1 /* the collision box reaches the floor height */
#define GROUND_NEAR 0x2    /* GameObjectNearFloor */

/* gameobject_s.attack_override, the "attack_override=" keywords of the SetOpponent and EngageOpponent script actions */
#define ATTACK_OVERRIDE_NONE 0
#define ATTACK_OVERRIDE_PUNCH_1 1
#define ATTACK_OVERRIDE_PUNCH_2 2
#define ATTACK_OVERRIDE_PUNCH_3 3
#define ATTACK_OVERRIDE_PUNCH_BEHIND 4
#define ATTACK_OVERRIDE_PUNCH_SPECIAL 5
#define ATTACK_OVERRIDE_BLOCK 6
#define ATTACK_OVERRIDE_SHOOT 7

/* gameobject_s.portal_flags: the AI script actions that set them (keyword in the action table). */
#define APIOBJECT_CAN_DEFEND 0x2                    /* CanDefend */
#define APIOBJECT_CAN_USE_WEAPON 0x8                /* CanUseWeapon */
#define APIOBJECT_KEEP_WEAPON_OUT 0x10              /* KeepWeaponOut, SnapWeaponOut */
#define APIOBJECT_CAN_HIT_FORCE_OBJECTS 0x40        /* CanHitForceObjects */
#define APIOBJECT_SHOOT_OPPONENTS 0x80              /* SetShootOpponents */
#define APIOBJECT_ALWAYS_BACK_FLIP 0x100            /* AlwaysBackFlip */
#define APIOBJECT_CAN_ATTACK 0x200                  /* CanAttack */
#define APIOBJECT_UPDATE_SOCK_POS 0x4000            /* UpdateSockPos (also UseCurrentSpeed, RaceOpponent) */
#define APIOBJECT_IMMUNE_TO_KILL_TERRAIN 0x40000    /* ImmuneToKillTerrain */
#define APIOBJECT_IMMUNE_TO_BOLTS 0x80000           /* ImmuneToBolts */
#define APIOBJECT_RESPAWNABLE 0x100000              /* Respawnable, Kill respawn */
#define APIOBJECT_RESPAWN_AT_ORIGIN 0x200000        /* Respawnable origin, Kill respawn_at_origin */
#define APIOBJECT_CATCH_UP_FORBIDDEN 0x2000000      /* CatchUpForbidden */
#define APIOBJECT_CANNOT_DROP_IN 0x4000000          /* CannotDropIn */
#define APIOBJECT_BOSS 0x8000000                    /* SetBoss */
#define APIOBJECT_CAN_SHOOT_OBSTRUCTIONS 0x10000000 /* CanShootObstructions */
#define APIOBJECT_PREFERS_PLAYERS 0x40000000        /* PrefersPlayers */
#define APIOBJECT_NO_GRAVITY 0x80000000u            /* ApplyGravity FALSE */
/* gameobject_s.portal_flags: runtime bits, from their setters and readers. */
#define APIOBJECT_BEEN_HIT 0x1                /* hit this frame (ObjHitObj, a batarang); the BeenHit condition */
#define APIOBJECT_GOT_GUN 0x4                 /* a CHARACTER_MODEL_BLASTER character; the GotGun condition */
#define APIOBJECT_MADE_NOISE 0x800            /* shot or jumped: opponents within hear_distance acquire it */
#define APIOBJECT_AI_DOUBLE_JUMP 0x1000       /* an AI double jump: jump is pressed again once falling */
#define APIOBJECT_ENGAGE_OBJECT 0x8000        /* EngageObject: bolts aim at the engaged position */
#define APIOBJECT_ENGAGE_TARGET_NEW 0x10000   /* EngageObject found its target this frame: no velocity to lead it yet */
#define APIOBJECT_ENGAGE_PREDICTIVE 0x20000   /* EngageObject predictive: bolts lead the engaged position */
#define APIOBJECT_PORTAL 0x400000             /* PortalGameObject: the object has a portal room */
#define APIOBJECT_PORTAL_UPDATE_ROOM 0x800000 /* PortalGameObject update_room: the room follows the object */

/* gameobject_s.action_movement_state in the jump context: the kind of jump, from the starts that set it (the force
 * pushed and deactivated contexts use the field for other values). 0 is the plain jump; 5 (a jump with JumpCode's
 * animation set bit 4, the droids' hover) and 7 are not named yet. */
#define JUMP_KIND_FLIP 1        /* JumpCode's flip (LEGOACT_FLIP) */
#define JUMP_KIND_COMBO 2       /* StartComboJump */
#define JUMP_KIND_LUNGE 3       /* StartLunge */
#define JUMP_KIND_SLAM 4        /* Slam_Start */
#define JUMP_KIND_ZIPUP 6       /* the jump off a zip-up */
#define JUMP_KIND_COMBAT_ROLL 8 /* the combat roll jump (LEGOACT_COMBATROLL_JUMP) */
#define JUMP_KIND_PUSH_OFF 9    /* the jump out of pushing (pushing.c) */

/* gameobject_s.flags_e24: movement runtime bits, from their setters. */
#define APIOBJECT_GOT_VICTIM 0x1             /* a lunge ate its target (Condition_GotVictim); killed when the eating ends */
#define APIOBJECT_BOUNDARY_TURN 0x2          /* turned away from a collision normal or the turn-around splines; TurnCode waits */
#define APIOBJECT_LOOP_TURN 0x4              /* a turn asked for in a loop context: the turn starts from the lean */
#define APIOBJECT_LOCATORS_VALID 0x8         /* the locator matrices are this frame's (cleared before the draw) */
#define APIOBJECT_CABLE_TARGET_FOUND 0x10    /* CableTargetGameObject found a target for the cable in range */
#define APIOBJECT_TELEPORT_WAITING 0x20      /* an active player waits at a teleport for the other one */
#define APIOBJECT_ACTION_TAPPED 0x40         /* the action button was released within 0.25 s of its press */
#define APIOBJECT_GRAPPLE_AT_MAX_LENGTH 0x80 /* the grapple rope is let out to Grapple_MaximumLength */
#define APIOBJECT_PUNCH_LANDED 0x100         /* the last punch hit something (the hit debris is the next type) */
#define APIOBJECT_AT_INTERFACE 0x200         /* working a panel or the hat machine (no droid self-destruct) */
#define APIOBJECT_FALL_LAND 0x400            /* the next landing plays the fall landing (force push end, tightrope) */
#define APIOBJECT_FALL_LANDING 0x800         /* the landing started as a fall landing: stick input does not cut it */
#define APIOBJECT_NO_JUMP 0x1000         /* a ledge, climb, grapple, batarang, push or tightrope start this frame: JumpCode does nothing */
#define APIOBJECT_MAGNET_CLIMB 0x2000    /* climbing a magnet surface (context variant 5): the lean is kept */
#define APIOBJECT_DETONATOR_THROW 0x4000 /* the detonator's release frame passed: ThermalDetonator_Throw after the animation */
#define APIOBJECT_BOLTS_PASS 0x8000      /* bolts pass through it this frame (a stun death, a jump with context bit 0x1000) */

/* gameobject_s.flags_ef0: AI script actions. */
#define APIOBJECT_IGNORE_SHOVE_SYSTEM 0x1           /* IgnoreShoveSystem */
#define APIOBJECT_CANNOT_BE_SEEN 0x2                /* CannotBeSeen */
#define APIOBJECT_BOLTS_DONT_GET_DEFLECTED_BACK 0x8 /* SetBoltsDontGetDeflectedBack */
#define APIOBJECT_DONT_MOVE 0x10                    /* SetDontMove */
#define APIOBJECT_DONT_SET_STOPPED_FLAG 0x20        /* DontSetStoppedFlag */
#define APIOBJECT_DEFLECT_PLAYERS_PART 0x100        /* DeflectPlayersPart */
#define APIOBJECT_DOOMED_ESCAPE_TAKE_DAMAGE 0x800   /* SetDoomedEscapeLocator take_damage */
#define APIOBJECT_WALK_BACKWARDS 0x8000             /* WalkBackwards */
#define APIOBJECT_CAN_PULL_LEVERS 0x800000          /* CanPullLevers */
#define APIOBJECT_NOT_WITH_PARTY 0x1000000          /* NotWithParty */
#define APIOBJECT_CANNOT_BE_FORCED_BACK 0x10000000  /* CannotBeForcedBack */
#define APIOBJECT_CAN_TURN 0x20000000               /* CanTurn */
#define APIOBJECT_CAN_TAKE_OVER 0x40000000          /* SetCanTakeOver */

/* gameobject_s.flags_ef4: AI script actions, the AI update rate UpdateGameObjects keeps and the girl bit. */
#define APIOBJECT_DONT_AIM_AT 0x1                       /* DontAimAt */
#define APIOBJECT_CAN_BE_CARRIED 0x2                    /* CanBeCarried */
#define APIOBJECT_AI_UPDATE_NOW 0x8                     /* this frame is the object's AI update (every interval frames) */
#define APIOBJECT_AI_EVERY_FRAME 0x10                   /* the AI update interval is 1 */
#define APIOBJECT_NO_TIME_BASED_UPDATE 0x20             /* UseTimeBasedUpdate FALSE: always every frame */
#define APIOBJECT_AWKWARD_SHAPE_OVERRIDE 0x80           /* AwkwardShapeOverride */
#define APIOBJECT_IGNORE_SLIDE_TERRAIN 0x400            /* IgnoreSlideTerrain */
#define APIOBJECT_GIRL 0x800                            /* from CHARACTER_RUNTIME_IS_A_GIRL (CanPunchGirls, voices) */
#define APIOBJECT_SPLINE_FOLLOW_TERRAIN 0x1000          /* SplineFollowTerrain */
#define APIOBJECT_USE_ONE_AT_ONCE 0x2000                /* SetUseOneAtOnce */
#define APIOBJECT_HELPING_WITH_TRIGGERS 0x20000         /* set by HelpWithTriggers */
#define APIOBJECT_CAN_HELP_WITH_TRIGGERS 0x40000        /* CanHelpWithTriggers */
#define APIOBJECT_CAN_COLLIDE_WITH_OBJECTS 0x80000      /* CanCollideWithObjects TRUE */
#define APIOBJECT_USE_CURRENT_SPEED 0x200000            /* UseCurrentSpeed, RaceOpponent */
#define APIOBJECT_IGNORE_LAST_SAFE_PATH_POS 0x1000000   /* IgnoreLastSafePathPos */
#define APIOBJECT_USE_BIG_JUMP_TO_JUMP 0x40000000       /* UseBigJumpToJump */
#define APIOBJECT_IGNORE_TURN_AROUND_SPLINE 0x80000000u /* IgnoreTurnAroundSpline */

/* gameobject_s.character_context: the index into CInfo (PCContextInfoTable[1 + context]), whose rows name them. */
#define CHARACTER_CONTEXT_NONE (-1)
#define CHARACTER_CONTEXT_JUMP 0
#define CHARACTER_CONTEXT_LAND_JUMP 1
#define CHARACTER_CONTEXT_LAND_JUMP_2 2
#define CHARACTER_CONTEXT_LAND_FLIP 3
#define CHARACTER_CONTEXT_LAND_COMBO_JUMP 4
#define CHARACTER_CONTEXT_COMBO 5
#define CHARACTER_CONTEXT_WEAPON_IN 6
#define CHARACTER_CONTEXT_WEAPON_OUT 7
#define CHARACTER_CONTEXT_FORCE 8
#define CHARACTER_CONTEXT_COMBO_ROTATE 9
#define CHARACTER_CONTEXT_SHOOT 10
#define CHARACTER_CONTEXT_INTERFACE 11
#define CHARACTER_CONTEXT_BLOCK 12
#define CHARACTER_CONTEXT_LAND_LUNGE 13
#define CHARACTER_CONTEXT_LAND_SLAM 14
#define CHARACTER_CONTEXT_TELEPORT 15
#define CHARACTER_CONTEXT_SWIPE 16
#define CHARACTER_CONTEXT_TUBE 17
#define CHARACTER_CONTEXT_FORCE_THROW 18
#define CHARACTER_CONTEXT_HOVER_UP 19
#define CHARACTER_CONTEXT_ROCKET 20
#define CHARACTER_CONTEXT_TAKE_HIT 21
#define CHARACTER_CONTEXT_ZAP 22
#define CHARACTER_CONTEXT_DEACTIVATED 23
#define CHARACTER_CONTEXT_HOLD 24
#define CHARACTER_CONTEXT_LAND_SPECIAL 25
#define CHARACTER_CONTEXT_COMMUNICATE 26
#define CHARACTER_CONTEXT_FORCE_PUSH 27
#define CHARACTER_CONTEXT_FORCE_PUSHED 28
#define CHARACTER_CONTEXT_FORCE_DEFLECT 29
#define CHARACTER_CONTEXT_FORCE_FROZEN 30
#define CHARACTER_CONTEXT_BIG_JUMP 31
#define CHARACTER_CONTEXT_BACK_FLIP 32
#define CHARACTER_CONTEXT_RECOIL 33
#define CHARACTER_CONTEXT_FORCED_BACK 34
#define CHARACTER_CONTEXT_DROP_IN 35
#define CHARACTER_CONTEXT_DROP_OUT 36
#define CHARACTER_CONTEXT_DODGE 37
#define CHARACTER_CONTEXT_PUNCH 38
#define CHARACTER_CONTEXT_PUSH 39
#define CHARACTER_CONTEXT_PUSH_SPINNER 40
#define CHARACTER_CONTEXT_LAND_COMBAT_ROLL 41
#define CHARACTER_CONTEXT_TURN 42
#define CHARACTER_CONTEXT_DOOMED 43
#define CHARACTER_CONTEXT_LAUNCH 44
#define CHARACTER_CONTEXT_BUILD_IT 45
#define CHARACTER_CONTEXT_THROW_DETONATOR 46
#define CHARACTER_CONTEXT_GRABBED 47
#define CHARACTER_CONTEXT_SPECIAL_MOVE_VICTIM 48
#define CHARACTER_CONTEXT_ROLL 49
#define CHARACTER_CONTEXT_UN_ROLL 50
#define CHARACTER_CONTEXT_SLIDE 51
#define CHARACTER_CONTEXT_BEEN_DRAGGED 52
#define CHARACTER_CONTEXT_ZIP_DOWN 53
#define CHARACTER_CONTEXT_LOOP 54
#define CHARACTER_CONTEXT_POO 55
#define CHARACTER_CONTEXT_GRAB 56
#define CHARACTER_CONTEXT_EATEN 57
#define CHARACTER_CONTEXT_BARREL_ROLL 58
#define CHARACTER_CONTEXT_BEEN_TAKEN_OVER 59
#define CHARACTER_CONTEXT_GET_IN 60
#define CHARACTER_CONTEXT_FLATTEN 61
#define CHARACTER_CONTEXT_BUCK 62
#define CHARACTER_CONTEXT_EAT 63
#define CHARACTER_CONTEXT_DISORIENTATE 64
#define CHARACTER_CONTEXT_ACTIVATE 65
#define CHARACTER_CONTEXT_ZAPPED_BY_FLOOR 66
#define CHARACTER_CONTEXT_CLIMB 67
#define CHARACTER_CONTEXT_TIGHTROPE 68
#define CHARACTER_CONTEXT_WALL_SHUFFLE 69
#define CHARACTER_CONTEXT_GRAPPLE 70
#define CHARACTER_CONTEXT_ZIP_UP 71
#define CHARACTER_CONTEXT_PLACE_DETONATOR 72
#define CHARACTER_CONTEXT_PICK_UP_DETONATOR 73
#define CHARACTER_CONTEXT_PULL_LEVER 74
#define CHARACTER_CONTEXT_FLOAT 75
#define CHARACTER_CONTEXT_SIGNAL 76
#define CHARACTER_CONTEXT_BATARANG 77
#define CHARACTER_CONTEXT_HANG 78
#define CHARACTER_CONTEXT_GLIDE 79
#define CHARACTER_CONTEXT_CATCH 80
#define CHARACTER_CONTEXT_TECHNO 81
#define CHARACTER_CONTEXT_ATTRACTO_TARGET 82
#define CHARACTER_CONTEXT_ATTRACTO_DEPOSIT 83
#define CHARACTER_CONTEXT_SONAR 84
#define CHARACTER_CONTEXT_LEDGE_TERRAIN 85
#define CHARACTER_CONTEXT_TRANSFORM 86
#define CHARACTER_CONTEXT_WALL_JUMP_WAIT 87
#define CHARACTER_CONTEXT_SUPER_CARRY 88
#define CHARACTER_CONTEXT_PUSH_OBSTACLE 89
#define CHARACTER_CONTEXT_STUNNED 90
#define CHARACTER_CONTEXT_LEDGE 91
#define CHARACTER_CONTEXT_SECURITY 92
#define CHARACTER_CONTEXT_BALLOONING 93
#define CHARACTER_CONTEXT_THROW_QUICK 94
#define CHARACTER_CONTEXT_DIE_AIR 95
#define CHARACTER_CONTEXT_DIE_GROUND 96
#define CHARACTER_CONTEXT_HAT_MACHINE 97
#define CHARACTER_CONTEXT_WHIP 98
#define CHARACTER_CONTEXT_NET_WAIT 99

typedef struct APIDEBRISSYS {        // size 0xc
    i32 named_count;                 // 0x0
    i32 capacity;                    // 0x4
    struct GAMEDEBRISENTRY *entries; // 0x8
} APIDEBRISSYS;

/* AIPACKET.flags bits that AI script actions and conditions set or test (their names are the script keywords). */
#define AIPACKET_IGNORE_WALL_SPLINES 0x80      /* IgnoreWallSplines; CheckWallSplines sets it too */
#define AIPACKET_CHECK_WALL_SPLINES 0x100      /* CheckWallSplines */
#define AIPACKET_CIRCLE_CLOCKWISE 0x200        /* SetCircleDirection Clockwise / AntiClockwise / Reverse / Random */
#define AIPACKET_PATH_BLOCKED 0x400000         /* the PathBlocked and CannotReachDestination conditions */
#define AIPACKET_FORMATION_COMMANDER 0x1000000 /* SetFormationCommander */
#define AIPACKET_ZERO_ACCELERATION 0x40000     /* SetZeroAcceleration: the pad input of the character is zero */
/* AIPACKET.flags bits the AI update sets (named from their setters and readers). */
#define AIPACKET_FOLLOWING_FORMATION 0x8             /* AIFormationFollow this frame: the goal speed mode is not applied */
#define AIPACKET_RIGHT_DIVERSION 0x20                /* CalculateIntersection found right_diversion */
#define AIPACKET_LEFT_DIVERSION 0x40                 /* CalculateIntersection found left_diversion */
#define AIPACKET_OPPONENT_IS_A_THREAT 0x800          /* the opponent has this packet's object as its opponent (GameOpponentIsAThreat) */
#define AIPACKET_PENDING_OPPONENT_IS_A_THREAT 0x1000 /* the same for the pending opponent; copied to 0x800 */
#define AIPACKET_INTERACTIVE 0x2000                  /* in this frame's interaction list: may be taken as an opponent */
#define AIPACKET_DRAWS_ATTENTION 0x4000              /* seen widens its visibility by draw_attention_distance; breaks party cover */
#define AIPACKET_NEEDS_LINE_OF_SIGHT 0x20000         /* an opponent is only taken in line of sight */
#define AIPACKET_INTERMEDIATE_TARGET 0x80000         /* the destination is a waypoint: slow pad input is kept */
#define AIPACKET_NO_ROUTE_PATH 0x100000              /* no way to the destination on the current route yet (the search tries the others) */
#define AIPACKET_KEEP_PATH_POS 0x2000000             /* a jump or context: the next update keeps the path position */
#define AIPACKET_RANGE_SHIFT 26                      /* bits 26-27: what SetMaxMovementRange measures from (AIRANGE_*) */
#define AIPACKET_RANGE_CREATURE 0x4000000            /* AIRANGE_CREATURE << AIPACKET_RANGE_SHIFT */
#define AIPACKET_RANGE_LOCATOR 0x8000000             /* AIRANGE_LOCATOR << AIPACKET_RANGE_SHIFT */
#define AIPACKET_RANGE_MASK 0xc000000
#define AIPACKET_OUT_OF_RANGE 0x10000000           /* farther than movement_target_radius: moves back to the range source */
#define AIPACKET_SPECIAL_MOVE_AHEAD 0x20000000     /* PreparingForSpecialMoveFn with the destination past this connection */
#define AIPACKET_PREPARING_SPECIAL_MOVE 0x40000000 /* PreparingForSpecialMoveFn took over the movement */
#define AIPACKET_MID_SPECIAL_MOVE 0x80000000u      /* MidSpecialMoveFn took over the movement */
/* the source of a movement range (AIPACKET_RANGE_SHIFT) */
#define AIRANGE_NONE 0
#define AIRANGE_CREATURE 1 /* the creature's position */
#define AIRANGE_LOCATOR 2  /* SetMaxMovementRange Locator: the packet's locator */
/* AIPACKET.navigation_flags (the code sets 0x1 and 0x8; 0x2 and 0x4 have no setter in the source) */
#define AIPACKET_NAV_SPECIAL_CONNECTION 0x1 /* the next connection has traversal flags the packet's capabilities allow */
#define AIPACKET_NAV_FIRST_PATH 0x2         /* follows path_sys->paths[0] instead of the active path, without position checks */
#define AIPACKET_NAV_SPECIAL_ROUTES 0x4     /* may take the path's special routes */
#define AIPACKET_NAV_ORIGIN_SET 0x8         /* creature_origin holds its place in the creature's formation */
/* AIPACKET.reset_mode: a creature's spawn state in UpdateGameObjects and ResetCreatures. */
#define AIPACKET_RESET_INACTIVE 0  /* waits for its activation (the "InActive" script state, an area, the difficulty) */
#define AIPACKET_RESET_STAGGERED 1 /* a group member waits spawn_delay before it appears */
#define AIPACKET_RESET_ACTIVE 2    /* spawned */
#define AIPACKET_RESET_IN_CRATE 3  /* hidden in a crate until the crate part breaks */
#define AIPACKET_RESET_DISABLED 4  /* not activated or dead for good; saved in disabled_ai_object_mask */
/* AIPACKET.path_connection_state in the jump connection handlers (StarWars_, Indy_ and Batman_PrepareJump); the
 * tube, door and other special connections count their own steps. */
#define AIJUMP_NONE 0
#define AIJUMP_APPROACH 1 /* to the take-off point on the start node's radius */
#define AIJUMP_BACK_UP 2  /* until farther than the jump distance from the landing node */
#define AIJUMP_RUN_UP 3   /* towards the landing node until within the jump distance */
#define AIJUMP_TAKE_OFF 4 /* a double jump waits for the jump re-entry timer */
#define AIJUMP_JUMP 5     /* SetSpecialMove and the jump button, unless someone stands on the landing */

/* AIPACKET.creature_index and .current_route when the packet has none (creature.c sets the index of a creature; the route
 * follower clears the route). */
#define AIPACKET_NO_CREATURE 0xff
#define AIPACKET_NO_ROUTE 0xff

struct AIPACKET { // size 0x20c

    union { // size 0xd0
        AISCRIPTPROCESS script_processor;

        struct {                                     // size 0xd0
            struct AISCRIPT *base_script;            // 0x000
            struct AISCRIPT *script;                 // 0x004
            struct AISTATE *state;                   // 0x008
            struct nulistlnk_s *action_node;         // 0x00c
            struct AISTATE *next_state;              // 0x010
            f32 params[4];                           // 0x014
            f32 script_timer;                        // 0x024
            AISCRIPTPROCESSSTACK param_stack[2];     // 0x028
            u8 unknown_50[0x4];                      // 0x050
            struct AIREFSCRIPT *active_refs[4];      // 0x054
            i32 active_ref_count;                    // 0x064
            u8 action_data_1;                        // 0x068
            u8 action_data_2;                        // 0x069
            u16 action_data_6;                       // 0x06a
            void *action_data_3;                     // 0x06c
            f32 action_data_4;                       // 0x070
            f32 action_data_5;                       // 0x074
            NUVEC script_action_position;            // 0x078
            AIPATHINFO script_destination_path_info; // 0x084
            f32 action_timer;                        // 0x09c
            u8 unknown_a0[0x4];                      // 0x0a0
            struct AILOCATOR *locator;               // 0x0a4
            struct AILOCATORSET *locator_set;        // 0x0a8
            u8 unknown_ac[0x4];                      // 0x0ac
            u8 creature_set;                         // 0x0b0
            u8 unknown_0b1[0x17];                    // 0x0b1

            union { // size 0x4
                u8 unknown_0c8[4];
                void *takeover_script_link;
            }; // 0x0c8

            AISCRIPTPROCESS *alternate_processor; // 0x0cc
        };
    }; // 0x000
    struct gameobject_s *owner;                 // 0x0d0
    struct APIOBJECT *nearest_opponent;         // 0x0d4
    f32 nearest_opponent_metric;                // 0x0d8
    struct APIOBJECT *pending_nearest_opponent; // 0x0dc
    f32 pending_nearest_metric;                 // 0x0e0
    struct APIOBJECT *opponent;                 // 0x0e4
    f32 opponent_metric;                        // 0x0e8
    struct APIOBJECT *pending_opponent;         // 0x0ec
    f32 pending_opponent_metric;                // 0x0f0
    struct gameobject_s *dont_avoid_character;  // 0x0f4
    NUVEC creature_origin;                      // 0x0f8
    NUVEC movement_destination;                 // 0x104
    f32 movement_stopping_distance;             // 0x110
    NUVEC movement_position;                    // 0x114
    f32 mover_height;                           // 0x120

    union { // size 0x2
        u16 unknown_124;
        i16 inside_path_node;
    }; // 0x124

    i16 animation_override_from; // 0x126
    i16 animation_override_to;   // 0x128
    u8 unknown_12a[0x6];         // 0x12a
    u32 route_mask_low;          // 0x130
    u32 route_mask_high;         // 0x134

    union { // size 0x1
        u8 unknown_138;
        u8 creature_index;
    }; // 0x138

    union { // size 0x1
        u8 path_update_flag;
        u8 path_connection_state;
    }; // 0x139

    u16 available_routes;         // 0x13a
    u8 current_route;             // 0x13c
    u8 next_route;                // 0x13d
    u8 reset_mode;                // 0x13e
    u8 goal_speed_mode;           // 0x13f
    u8 movement_stopped;          // 0x140
    u8 unknown_141;               // 0x141
    u8 divert_search_index;       // 0x142
    u8 divert_node_index;         // 0x143
    struct AIGROUP *group;        // 0x144
    u8 group_member_index;        // 0x148
    u8 group_column;              // 0x149
    u8 group_row;                 // 0x14a
    u8 movement_target_direction; // 0x14b
    NUVEC terrain_origin;         // 0x14c
    AIPATHINFO path_info;         // 0x158
    NUVEC last_path_position;     // 0x170

    union { // size 0x4
        void *current_path_node;
        struct AIPATHNODE *goal_path_node;
    }; // 0x17c

    f32 movement_instruction_parameter; // 0x180

    union { // size 0x4
        void *path_target;
        struct AIPATHNODE *special_move_node;
    }; // 0x184

    struct AIPATHCNX *movement_target;                // 0x188
    f32 antinode_timer;                               // 0x18c
    struct AIPATHCNX *intersection_connection;        // 0x190
    struct AIPATHCNX *intersection_target_connection; // 0x194
    NUVEC right_diversion;                            // 0x198
    NUVEC left_diversion;                             // 0x1a4

    union { // size 0x4
        u8 unknown_1b0[4];
        struct AILOCATOR *respawn_locator;
    }; // 0x1b0

    NUVEC fallback_destination;     // 0x1b4
    f32 fallback_stopping_distance; // 0x1c0
    f32 movement_parameter;         // 0x1c4
    u8 unknown_1c8[4];              // 0x1c8
    AIPATHINFO fallback_path_info;  // 0x1cc
    NUVEC *movement_look_target;    // 0x1e4

    union { // size 0x4
        u32 flags;

        struct {                // size 0x4
            u8 flags_low;       // 0x1e8
            u8 selection_flags; // 0x1e9
            u8 route_flags;     // 0x1ea
            u8 priority_flags;  // 0x1eb
        };

        struct {                        // size 0x4
            u32 : 15;                   // 0x1e8:0
            u32 antinode_clockwise : 1; // 0x1e8:15
            u32 : 16;                   // 0x1e8:16
        };
    }; // 0x1e8

    u32 navigation_flags;       // 0x1ec
    f32 movement_target_radius; // 0x1f0
    u32 capabilities;           // 0x1f4
    i32 frame_state;            // 0x1f8
    u32 respawn_count;          // 0x1fc
    f32 spawn_delay;            // 0x200
    u8 unknown_204[4];          // 0x204
    f32 time_off_path;          // 0x208
};

/* The AI library's object: the first 0x2b8 bytes of every GAMEOBJECT (AISysGetPathPos2 builds one of this size on
   the stack). owner points back at the GAMEOBJECT itself. */
typedef struct APIOBJECT {                          // size 0x2b8
    struct gameobject_s *owner;                     // 0x000
    AIPACKET *ai;                                   // 0x004
    u8 unknown_008[0x48];                           // 0x008
    struct CHARACTER_MODEL_LAYERS *character_model; // 0x050
    struct characterdata_s *character_data;         // 0x054
    u16 facing_angle;                               // 0x058
    u16 movement_facing_angle;                      // 0x05a
    NUVEC position;                                 // 0x05c
    NUVEC velocity;                                 // 0x068
    NUVEC previous_velocity;                        // 0x074
    NUVEC collision_position;                       // 0x080
    NUVEC start_position;                           // 0x08c
    NUVEC initial_position;                         // 0x098
    f32 scaled_radius;                              // 0x0a4
    u8 unknown_a8[0x4];                             // 0x0a8
    f32 collision_radius;                           // 0x0ac

    union { // size 0x148
        u8 unknown_0b0[0x148];

        struct {                            // size 0x148
            u8 unknown_b0_to_1b4[0x104];    // 0x0b0
            NUVEC last_safe_position;       // 0x1b4
            NUVEC previous_position;        // 0x1c0
            NUVEC respawn_position;         // 0x1cc
            u8 unknown_1d8[0x4];            // 0x1d8
            f32 bounds_radius;              // 0x1dc
            f32 bounds_half_height;         // 0x1e0
            u8 unknown_1e4[0x4];            // 0x1e4
            u32 collision_identity_mask[2]; // 0x1e8
            u32 collision_contact_mask[2];  // 0x1f0
        };
    }; // 0x0b0

    u32 motion_flags; // 0x1f8

    union { // size 0x4
        u32 flags_low;

        struct {                      // size 0x4
            u32 : 17;                 // 0x1fc:0
            u32 ignore_antinodes : 1; // 0x1fc:17
            u32 : 14;                 // 0x1fc:18
        };
    }; // 0x1fc

    u8 unknown_200[0xc];                                          // 0x200
    nuhspecial_s collision_special;                               // 0x20c
    u8 unknown_218_b[0x8];                                        // 0x218
    f32 water_height;                                             // 0x220
    u8 unknown_224[0x4];                                          // 0x224
    f32 velocity_magnitude;                                       // 0x228
    f32 horizontal_velocity_magnitude;                            // 0x22c
    f32 view_distance;                                            // 0x230
    f32 hear_distance;                                            // 0x234
    f32 max_view_height;                                          // 0x238
    f32 min_view_height;                                          // 0x23c
    f32 ai_visibility_extension;                                  // 0x240
    u8 unknown_244[0x18];                                         // 0x244
    f32 previous_animation_root_time;                             // 0x25c
    f32 previous_blend_target_root_time;                          // 0x260
    struct CHARACTERANIM_CONFIG *previous_animation_root_info;    // 0x264
    struct CHARACTERANIM_CONFIG *previous_blend_target_root_info; // 0x268
    u8 unknown_26c[0xc];                                          // 0x26c
    u16 pitch_angle;                                              // 0x278
    u8 unknown_27a[0x2];                                          // 0x27a
    u16 roll_angle;                                               // 0x27c
    i16 supporting_platform_id;                                   // 0x27e
    i8 locator_name_index;                                        // 0x280
    u8 grounded;                                                  // 0x281
    u8 unknown_282[9];                                            // 0x282
    u8 dead;                                                      // 0x28b
    u8 unknown_28c;                                               // 0x28c
    u8 object_id;                                                 // 0x28d
    u8 unknown_28e[0x4];                                          // 0x28e
    u16 ai_collision_priority;                                    // 0x292
    u16 ai_resolved_collision_priority;                           // 0x294
    u8 unknown_296[0x2];                                          // 0x296
    struct APIOBJECT *collision_link;                             // 0x298
    u8 unknown_29c[0x4];                                          // 0x29c
    u32 collision_exclusion_mask[2];                              // 0x2a0
    u64 awareness;                                                // 0x2a8
    u32 ai_area_mask[2];                                          // 0x2b0
} APIOBJECT;

/* CHARACTERANIM_CONFIG.flags: the keywords of an anim_start block of a character config (CC_anim_start). */
#define CHARANIM_AREA 0x1                  /* loaded with the area's animations, not the character's */
#define CHARANIM_CYCLE 0x2                 /* cycle on */
#define CHARANIM_LOAD 0x4                  /* the animation file is loaded (set for every anim_start) */
#define CHARANIM_BSA 0x8                   /* bsa on */
#define CHARANIM_IDLE 0x10                 /* type idle */
#define CHARANIM_TRANSLATE 0x20            /* translate */
#define CHARANIM_NO_HEADTURN 0x40          /* no_headturn, headturn_off, headturn off */
#define CHARANIM_CAN_PLAY_BACKWARDS 0x80   /* can_play_backwards */
#define CHARANIM_FOOTSTEPS 0x100           /* footsteps */
#define CHARANIM_GUN_OFF 0x400             /* gun_off, gun off */
#define CHARANIM_GUN_ON 0x800              /* gun_on, gun on */
#define CHARANIM_NO_LOAD 0x8000            /* AnimList_NoLoad: not loaded at all */
#define CHARANIM_FOOTSTEPS_4FRAMES 0x10000 /* footsteps 4frames */
#define CHARANIM_FOOTSTEPS_JUDDER 0x20000  /* footsteps judder */
#define CHARANIM_FOOTSTEPS_SHAKE 0x40000   /* footsteps shake */

/* CHARACTERANIM_CONFIG.misc_flags (CC_SetAnimMiscFlags). */
#define CHARANIM_MISC_IS_PUNCH 0x1            /* is_punch */
#define CHARANIM_MISC_IS_KICK 0x2             /* is_kick */
#define CHARANIM_MISC_IS_WHIP 0x4             /* is_whip */
#define CHARANIM_MISC_EXTRA_CHARACTER_OFF 0x8 /* extra_character_off */

typedef struct CHARACTERANIM_CONFIG { // size 0x4c
    char *name;                       // 0x00
    u32 flags;                        // 0x04
    i16 action_id;                    // 0x08
    u8 unknown_0a;                    // 0x0a
    u8 priority;                      // 0x0b
    u8 minimum_repetitions;           // 0x0c
    u8 maximum_repetitions;           // 0x0d
    u8 unknown_0e[2];                 // 0x0e
    f32 blend_in_time;                // 0x10
    f32 blend_out_time;               // 0x14
    f32 playback_rate;                // 0x18
    f32 movement_speed;               // 0x1c
    f32 movement_rate_cap;            // 0x20
    f32 action_speed;                 // 0x24

    union { // size 0x10

        struct {               // size 0x10
            f32 event_frame_1; // 0x28
            f32 event_frame_2; // 0x2c
            f32 event_frame_3; // 0x30
            f32 event_frame_4; // 0x34
        };

        f32 event_frames[4];
    }; // 0x28

    f32 stop_frame;         // 0x38
    NUVEC root_translation; // 0x3c
    u16 misc_flags;         // 0x48
    u8 locator;             // 0x4a
    u8 unknown_4b;          // 0x4b
} CHARACTERANIM_CONFIG;

typedef struct CHARACTER_MODEL_LAYERS { // size 0x54
    i16 model_id;                       // 0x00
    u8 flags;                           // 0x02
    u8 state;                           // 0x03
    struct NUHGOBJ_EVAL *hierarchy;     // 0x04

    union { // size 0x4
        struct CHARACTER_ANIMATION_LOCATOR **animations;
        CHARACTERANIM_CONFIG **animation_configs;
    }; // 0x08

    union { // size 0x4
        void **action_animations;
        void **model_data_b;
    }; // 0x0c
    struct NUANIMDATA2 **model_data_c; // 0x10
    void *points_of_interest[16];      // 0x14
} CHARACTER_MODEL_LAYERS;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*RedirectAnimFn)(char *, void *, CHARACTERANIM_CONFIG *, char *);
    i32 PCBuildModelLayerList(CHARACTER_MODEL_LAYERS *, i16 *, u32);
    void PCStoreModelLocatorCoordinates(CHARACTER_MODEL_LAYERS *, NUMTX *, NUMTX *, NUVEC *, NUMTX *);

    void APIObjectLOSChecks(APIOBJECTSYS *, i32, i32, APIOBJECT **, i32, APIOBJECT **, f32);

    i32 APIObjectCollision(APIOBJECT *, APIOBJECT *);
    i32 APIObjectCollision2D(APIOBJECT *, APIOBJECT *);
    void APIObjectCollisions(i32, APIOBJECT **, NUVEC *, NUVEC *, i32 (*)(APIOBJECT *, APIOBJECT *));
#ifdef __cplusplus
}
#endif

typedef struct apicharacterdata_s {               // size 0x4c
    i32 field0_0x0;                               // 0x00
    u32 model_flags;                              // 0x04
    char *dir;                                    // 0x08
    char *name;                                   // 0x0c
    CHARACTERANIM_CONFIG *animations;             // 0x10
    u8 opaque_14_24[0x10];                        // 0x14
    struct PLAYERCHARACTERCONFIG *game_character; // 0x24
    struct CHARACTER_EFFECT_CONFIG *effects;      // 0x28
    f32 field13_0x2c;                             // 0x2c
    f32 collision_radius;                         // 0x30
    f32 bounds_min_y;                             // 0x34
    f32 bounds_max_y;                             // 0x38
    f32 field17_0x3c;                             // 0x3c
    u8 flags;                                     // 0x40
    u8 field19_0x41;                              // 0x41
    i16 field20_0x42;                             // 0x42
    u32 ai_path_capabilities;                     // 0x44
    u32 field22_0x48;                             // 0x48
} APICHARACTERDATA;

typedef struct apicharsys_s {                 // size 0x38
    i32 character_count;                      // 0x00
    i32 model_capacity;                       // 0x04
    i32 model_id_capacity;                    // 0x08
    i32 permanent_model_count;                // 0x0c
    i32 loaded_model_count;                   // 0x10
    i32 animation_capacity;                   // 0x14
    CHARACTER_MODEL_LAYERS *models;           // 0x18
    i16 *playermodelids;                      // 0x1c
    APICHARACTERDATA *char_data;              // 0x20
    API_ANIMATION_CACHE *animations;          // 0x24
    i32 area_animation_count;                 // 0x28
    i32 loaded_animation_count;               // 0x2c
    i32 animation_load_attempts;              // 0x30
    void (*set_creature_lights)(APIOBJECT *); // 0x34
} APICHARSYS;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NUHGOBJ_EVAL *Temphgobj;
    extern struct numtl_s *APITrans_Mtl[2];
#ifdef __cplusplus
}
#endif

#pragma pack(push, 1)

typedef struct AI_SPECIAL_PROGRESS { // size 0x6
    u8 reserved_1072;                // 0x0
    u8 flags;                        // 0x1
    f32 progress;                    // 0x2
} AI_SPECIAL_PROGRESS;

typedef struct INDY_GUIDELINE_STATE {     // size 0x6
    u8 movement_direction;                // 0x0
    i8 endpoint;                          // 0x1
    struct guideline_record_s *guideline; // 0x2
} INDY_GUIDELINE_STATE;

#pragma pack(pop)

/* PLAYERPACKET.combo_style: which punch animation a punch combo continues with (0 or 1), or none. */
#define COMBO_STYLE_NONE 0xff

/* The player packet: the per-character control state ResetPlayerPacket clears (GAMEOBJECT.player). */
typedef struct PLAYERPACKET {        // size 0x788
    CHARACTER_SHADOW_PACKET shadows; // 0x000

    struct {          // size 0x8
        f32 height;   // 0x064
        f32 velocity; // 0x068
    } suspension[4];  // 0x064

    NUVEC external_force;                  // 0x084
    NUVEC launch_origin;                   // 0x090
    NUVEC context_position_offset;         // 0x09c
    f32 context_animation_timer;           // 0x0a8
    f32 unknown_770;                       // 0x0ac
    u8 unknown_774[0x4];                   // 0x0b0
    f32 context_hover_target_y;            // 0x0b4
    f32 context_timer;                     // 0x0b8
    f32 unknown_780;                       // 0x0bc
    f32 context_duration;                  // 0x0c0
    f32 airborne_action_timer;             // 0x0c4
    f32 jump_start_height;                 // 0x0c8
    struct gameobject_s *force_target;     // 0x0cc
    struct blowup_target_s *blowup_target; // 0x0d0

    union { // size 0x4
        void *grapple;
        struct PART *force_throw_part;
        struct techno_s *techno;
        struct zipup_s *attached_zipup;
        struct TUBE *attached_tube;
        struct GIZMO_LEVER *lever;
        struct panel_movement_s *panel;
        struct security_door_record_s *security_door;
        struct attracto_s *attracto;
        struct SIGNAL_s *signal;
        struct TELEPORT_MOTION *teleport;
        struct TIGHTROPE_MOTION *tightrope;
        struct HATMACHINE *hat_machine;
        struct DETONATOR_s *detonator;
    }; // 0x0d4

    i32 panel_use_request; // 0x0d8

    union { // size 0x4
        void *panel_request_data;

        struct {                 // size 0x4
            u8 unknown_7a0[0x3]; // 0x0dc
            u8 unknown_7a3;      // 0x0df
        };
    }; // 0x0dc

    u16 unknown_7a4;              // 0x0e0
    u16 takeover_start_angle;     // 0x0e2
    u16 unknown_7a8;              // 0x0e4
    i16 unknown_7aa;              // 0x0e6
    i16 queued_context_animation; // 0x0e8
    u8 combo_branch;              // 0x0ea
    u8 unknown_07af;              // 0x0eb
    u8 combo_input_latched;       // 0x0ec

    union { // size 0x1
        u8 unknown_07b1;
        u8 landing_followup;
    }; // 0x0ed

    u8 combo_stage;           // 0x0ee
    u8 unknown_7b3;           // 0x0ef
    u8 big_jump_animation;    // 0x0f0
    i8 character_context;     // 0x0f1
    u8 context_variant;       // 0x0f2
    i8 special_move;          // 0x0f3
    u8 action_movement_state; // 0x0f4
    u8 jump_sequence;         // 0x0f5
    i8 whip_action;           // 0x0f6
    i8 hit_variant;           // 0x0f7

    union { // size 0x4
        u32 context_variant_flags;

        struct {                     // size 0x4
            u8 context_flags;        // 0x0f8
            i8 context_variant_bits; // 0x0f9
            u8 jump_flags;           // 0x0fa
            u8 unknown_7bf[0x1];     // 0x0fb
        };
    }; // 0x0f8

    f32 tag_cooldown;                        // 0x0fc
    i8 tag_player_index;                     // 0x100
    u8 tag_flags;                            // 0x101
    u8 unknown_07c6[2];                      // 0x102
    struct gameobject_s *pending_tag_target; // 0x104

    union { // size 0x4
        SOCKROT sock_angles;

        struct {                   // size 0x4
            u16 sock_body_pitch;   // 0x108
            u16 sock_body_heading; // 0x10a
        };
    }; // 0x108

    MINIANIMPACKET mini_anim_packet; // 0x10c

    i16 *weapon_model_override;                        // 0x130
    u8 unknown_7f8[0x4];                               // 0x134
    struct player_coin_packet_s *coin_packet;          // 0x138
    struct player_force_los_slot_s *gizforce_los_info; // 0x13c
    NUMTX locator_matrices[16];                        // 0x140
    NUMTX vehicle_orientation;                         // 0x540
    f32 current_speed_mul;                             // 0x580
    f32 current_speed_bias;                            // 0x584
    f32 socket_current_speed;                          // 0x588

    union { // size 0x24
        u8 unknown_c50[0x4];
        f32 unknown_c54;
        u8 unknown_c58[0x4];
        f32 head_target_facing;
        u8 unknown_c60[0x14];
        NUVEC weapon_trail_offset;

        struct {                           // size 0x24
            u8 targeting_reserved_c50[12]; // 0x58c
            NUVEC camera_screen_position;  // 0x598
            NUVEC glow_position;           // 0x5a4
        };
    }; // 0x58c

    NUVEC camera_saved_position; // 0x5b0

    union { // size 0xc

        struct {               // size 0xc
            void *unknown_c80; // 0x5bc
            void *unknown_c84; // 0x5c0
            void *unknown_c88; // 0x5c4
        };

        NUVEC attack_target_position;
    }; // 0x5bc

    union { // size 0xc

        struct {             // size 0xc
            f32 unknown_c8c; // 0x5c8
            f32 unknown_c90; // 0x5cc
            f32 unknown_c94; // 0x5d0
        };

        NUVEC attack_target_velocity;
    }; // 0x5c8

    NUVEC suspension_normal;              // 0x5d4
    struct GAMEPAD *pad;                  // 0x5e0
    SOCKPOSITION *previous_sock_position; // 0x5e4
    u32 directional_input_state;          // 0x5e8
    struct PART *incoming_part;           // 0x5ec
    struct PART *blocked_part;            // 0x5f0
    struct PART *force_part;              // 0x5f4
    struct SUIT_LAYERS *suit;             // 0x5f8

    union { // size 0x4
        struct gameobject_s *unknown_cc0;
        struct player_batarang_slot_s *batarang;
    }; // 0x5fc
    struct TORPEDOPACKET *torpedo;         // 0x600
    struct gameobject_s *collision_target; // 0x604
    u8 unknown_0ccc[4];                    // 0x608

    union { // size 0x4
        struct gameobject_s *takeover_source;
        struct gameobject_s *rider;
    }; // 0x60c
    struct gameobject_s *takeover_owner;   // 0x610
    void *sabre_streaks[4][2];             // 0x614
    struct gameobject_s *block_attacker;   // 0x634
    struct gameobject_s *incoming_melee;   // 0x638
    struct gameobject_s *incoming_special; // 0x63c
    struct bolt_s *incoming_bolt;          // 0x640
    struct bolt_s *blocked_bolt;           // 0x644

    i32 unknown_d0c;                         // 0x648
    struct gameobject_s *force_throw_target; // 0x64c

    void *glow_object;                              // 0x650
    void *glow_pending;                             // 0x654
    struct gameobject_s *airborne_collision_target; // 0x658
    u32 unknown_d20;                                // 0x65c
    f32 ground_contact_grace_timer;                 // 0x660
    f32 jump_reentry_timer;                         // 0x664
    f32 airborne_input_timer;                       // 0x668
    f32 origin_scale_override;                      // 0x66c
    f32 timer_d34;                                  // 0x670
    f32 communicate_blend;                          // 0x674
    f32 weapon_scale;                               // 0x678
    f32 weapon_scale_rate;                          // 0x67c
    f32 sabre_collision_radius;                     // 0x680
    f32 delayed_turn_timer;                         // 0x684
    f32 combo_input_timer;                          // 0x688
    f32 ai_combo_cooldown;                          // 0x68c
    f32 unknown_d54;                                // 0x690
    f32 quick_shoot_timer;                          // 0x694
    f32 torpedo_fire_cooldown;                      // 0x698
    f32 hud_icon_timer;                             // 0x69c
    f32 jump_variant_timer;                         // 0x6a0
    f32 jump_chain_timer;                           // 0x6a4
    f32 surface_flag8_scale;                        // 0x6a8
    f32 movement_animation_hold_timer;              // 0x6ac
    f32 movement_animation_release_timer;           // 0x6b0
    f32 directional_seek_multiplier;                // 0x6b4
    f32 terrain_origin_floor_offset;                // 0x6b8

    f32 glow_alpha;    // 0x6bc
    f32 glow_target;   // 0x6c0
    f32 glow_recovery; // 0x6c4
    f32 glow_radius;   // 0x6c8
    f32 glow_hold;     // 0x6cc

    union { // size 0x4
        f32 engine_level;
        f32 force_use_volume;
    }; // 0x6d0

    f32 turn_braking;                // 0x6d4
    f32 fall_recovery_timer;         // 0x6d8
    f32 nearby_floor_distance;       // 0x6dc
    f32 input_toggle_hold_time;      // 0x6e0
    f32 unknown_da8;                 // 0x6e4
    f32 fall_animation_timer;        // 0x6e8
    f32 unknown_db0;                 // 0x6ec
    f32 fall_hover_height;           // 0x6f0
    f32 pre_reset_hover_height;      // 0x6f4
    f32 field_dbc;                   // 0x6f8
    f32 sabre_contact_sfx_timer;     // 0x6fc
    f32 unknown_dc4;                 // 0x700
    f32 unknown_dc8;                 // 0x704
    f32 thrust_effect_scale;         // 0x708
    f32 takeover_timer;              // 0x70c
    f32 arrow_blend;                 // 0x710
    f32 block_cooldown;              // 0x714
    f32 field_ddc;                   // 0x718
    f32 unknown_de0;                 // 0x71c
    f32 hold_timer;                  // 0x720
    f32 ai_hop_timer;                // 0x724
    f32 powerup_time;                // 0x728
    f32 pre_terrain_speed;           // 0x72c
    f32 post_terrain_speed;          // 0x730
    NUVEC reset_velocity;            // 0x734
    i32 dynamic_light_id;            // 0x740
    u16 delayed_turn_target_angle;   // 0x744
    u16 force_heading;               // 0x746
    u16 current_input_angle;         // 0x748
    u16 previous_boundary_angle;     // 0x74a
    i16 previous_block_animation;    // 0x74c
    i16 unknown_e12;                 // 0x74e
    i16 held_movement_animation;     // 0x750
    i16 released_movement_animation; // 0x752
    i16 movement_lean_angle;         // 0x754
    i16 secondary_lean_angle;        // 0x756
    i16 tertiary_lean_angle;         // 0x758
    i16 glow_model;                  // 0x75a

    union { // size 0x4
        u32 flags_e20;

        struct {            // size 0x4
            u8 unknown_e20; // 0x75c
            u8 unknown_e21; // 0x75d
            u8 unknown_e22; // 0x75e
            u8 unknown_e23; // 0x75f
        };

        struct {                      // size 0x4
            u32 : 16;                 // 0x75c:0
            u32 weapon_animation : 1; // 0x75c:16
        };
    }; // 0x75c

    union { // size 0x4
        u32 flags_e24;
        u8 unknown_e24;

        struct {                                                                            // size 0x4
            u32 : 3;                                                                        // 0x760:0
            u32 origin_hold : 1; /* tightrope: hold position taken from the model origin */ // 0x760:3
            u32 : 12;                                                                       // 0x760:4
            u32 hit_side : 3; /* 3 = hit from behind, 4 = from the front */                 // 0x760:16
            u32 : 13;                                                                       // 0x760:19
        };
    }; // 0x760

    u8 effect_noise[4];    // 0x764
    u8 glow_kind;          // 0x768
    u8 glow_pending_kind;  // 0x769
    u8 unknown_0e2e;       // 0x76a
    u8 block_latch;        // 0x76b
    u8 quick_shoot_flags;  // 0x76c
    u8 unknown_0e31;       // 0x76d
    u8 jump_variant_state; // 0x76e
    u8 unknown_e33;        // 0x76f
    u8 sabre_flags;        // 0x770
    u8 sabre_damage;       // 0x771
    u8 unknown_0e36;       // 0x772
    u8 wall_run_state;     // 0x773
    u8 shield_hitpoints;   // 0x774

    union { // size 0x1
        u8 saved_hitpoints;
        u8 vehicle_secondary_hp;
        u8 protocol_interface_action;
    }; // 0x775

    u8 bolt_fire_phase;     // 0x776
    u8 action_suppressed;   // 0x777
    u8 in_narrow_socket;    // 0x778
    u8 batarang_release;    // 0x779
    i8 debris_joint;        // 0x77a
    i8 quick_shoot_bolt_id; // 0x77b
    i8 slam_effect;         // 0x77c
    u8 combo_style;         // 0x77d
    u8 raw_surface_id;      // 0x77e
    i8 blade_index;         // 0x77f
    i8 blade_states[4];     // 0x780
    f32 unknown_e48;        // 0x784
} PLAYERPACKET;

typedef struct gameobject_s { // size 0x10d8

    union { // size 0x4c

        struct {                         // size 0x4c
            struct gameobject_s *objptr; // 0x0000
            AIPACKET *ai;                // 0x0004
            ANIMPACKET anim_packet;      // 0x0008
        };

        struct {                             // size 0x40
            u8 unknown_0000_animation[0x39]; // 0x0000
            u8 anim_blending;                // 0x0039
            u8 unknown_003a[4];              // 0x003a
            i16 animation_index;             // 0x003e
        };
    }; // 0x0000

    u8 unknown_004c[4];                      // 0x004c
    CHARACTER_MODEL_LAYERS *character_model; // 0x0050
    struct characterdata_s *character_data;  // 0x0054
    u16 facing_angle;                        // 0x0058
    u16 movement_facing_angle;               // 0x005a
    NUVEC position;                          // 0x005c
    NUVEC velocity;                          // 0x0068
    NUVEC previous_velocity;                 // 0x0074
    NUVEC collision_position;                // 0x0080
    NUVEC start_position;                    // 0x008c
    NUVEC initial_position;                  // 0x0098
    f32 scaled_radius;                       // 0x00a4
    f32 origin_scale;                        // 0x00a8
    f32 collision_radius;                    // 0x00ac
    f32 unknown_b0;                          // 0x00b0
    f32 scaled_height;                       // 0x00b4
    NUMTX transform;                         // 0x00b8
    NUMTX draw_secondary_mtx;                // 0x00f8
    NUMTX draw_tertiary_mtx;                 // 0x0138
    NUVEC collision_min;                     // 0x0178
    NUVEC collision_max;                     // 0x0184
    NUVEC upper_position;                    // 0x0190
    NUVEC lower_position;                    // 0x019c
    NUVEC collision_origin;                  // 0x01a8
    NUVEC last_safe_position;                // 0x01b4
    NUVEC previous_position;                 // 0x01c0
    NUVEC respawn_position;                  // 0x01cc
    f32 respawn_timer;                       // 0x01d8
    f32 bounds_radius;                       // 0x01dc
    f32 bounds_half_height;                  // 0x01e0
    u8 unknown_01e4[4];                      // 0x01e4
    u32 collision_identity_mask[2];          // 0x01e8
    u32 collision_contact_mask[2];           // 0x01f0
    u32 flags_1f8;                           // 0x01f8

    union { // size 0x4
        u8 flags_1fc;
        u16 flags_1fc_half;
        u32 flags_1fc_word;

        struct {              // size 0x2
            u8 flags_1fc_low; // 0x01fc
            u8 unknown_1fd;   // 0x01fd
        };

        struct {                           // size 0x4
            u32 flags_1fc_bits_0_12 : 13;  // 0x01fc:0
            u32 flags_1fc_bit_13 : 1;      // 0x01fc:13
            u32 flags_1fc_bits_14_31 : 18; // 0x01fc:14
        };

        struct {                        // size 0x4
            u32 flags_1fc_bit_0 : 1;    // 0x01fc:0
            u32 flags_1fc_bit_1 : 1;    // 0x01fc:1
            u32 flags_1fc_bits_2_6 : 5; // 0x01fc:2
            u32 player_controlled : 1;  // 0x01fc:7
        };
    }; // 0x01fc

    union { // size 0xc
        NUVEC movement_direction;

        struct {                      // size 0xc
            f32 movement_direction_x; // 0x0200
            u8 unknown_0204[4];       // 0x0204
            f32 movement_direction_z; // 0x0208
        };
    }; // 0x0200

    u8 unknown_020c[0xc];      // 0x020c
    f32 previous_floor_height; // 0x0218
    f32 floor_height;          // 0x021c
    f32 water_height;          // 0x0220

    union { // size 0x4
        u8 unknown_0224[4];
        f32 roof_height;
    }; // 0x0224

    f32 velocity_magnitude;                                       // 0x0228
    f32 horizontal_velocity_magnitude;                            // 0x022c
    f32 view_distance;                                            // 0x0230
    f32 hear_distance;                                            // 0x0234
    f32 max_view_height;                                          // 0x0238
    f32 min_view_height;                                          // 0x023c
    f32 ai_visibility_extension;                                  // 0x0240
    NUVEC previous_animation_root;                                // 0x0244
    NUVEC previous_blend_target_root;                             // 0x0250
    f32 previous_animation_root_time;                             // 0x025c
    f32 previous_blend_target_root_time;                          // 0x0260
    struct CHARACTERANIM_CONFIG *previous_animation_root_info;    // 0x0264
    struct CHARACTERANIM_CONFIG *previous_blend_target_root_info; // 0x0268
    NUVEC animation_root_delta;                                   // 0x026c
    u16 pitch_angle;                                              // 0x0278
    u16 unknown_276;                                              // 0x027a
    u16 roll_angle;                                               // 0x027c
    i16 supporting_platform_id;                                   // 0x027e

    union { // size 0x1
        u8 unknown_280;
        i8 player_index; /* -1 when no player controls the object */
    }; // 0x0280

    u8 unknown_281; // 0x0281
    u8 unknown_282; // 0x0282
    i8 unknown_283; // 0x0283

    union { // size 0x1
        u8 unknown_0284;
        i8 roof_surface_id;
    }; // 0x0284

    u8 jump_animation_state; // 0x0285

    union { // size 0x1
        u8 unknown_0286;
        u8 is_underwater;
    }; // 0x0286

    u8 water_intersects;  // 0x0287
    u8 model_draw_result; // 0x0288

    union { // size 0x2
        u8 unknown_0289[2];

        struct {             // size 0x2
            u8 effect_flags; // 0x0289
            u8 unknown_028a; // 0x028a
        };
    }; // 0x0289

    u8 dead;               // 0x028b
    u8 origin_joint_ready; // 0x028c
    u8 object_id;          // 0x028d: slot index in the object system (APIObjectCreate)

    union { // size 0xa
        u8 unknown_028e[0xa];

        struct {                                // size 0xa
            u16 surface_effect_count;           // 0x028e
            u16 surface_effect_id;              // 0x0290
            u16 ai_collision_priority;          // 0x0292
            u16 ai_resolved_collision_priority; // 0x0294
            u8 ai_priority_reserved_296[0x2];   // 0x0296
        };
    }; // 0x028e
    struct APIOBJECT *collision_link; // 0x0298
    u8 unknown_29c[0x4];              // 0x029c
    u32 collision_exclusion_mask[2];  // 0x02a0

    u64 awareness; // 0x02a8

    u32 ai_area_mask[2];         // 0x02b0
    f32 idle_total_time;         // 0x02b8
    f32 idle_animation_time;     // 0x02bc
    f32 idle_animation_limit;    // 0x02c0
    i16 idle_animation;          // 0x02c4
    i16 previous_idle_animation; // 0x02c6

    union { // size 0x20c
        AIPACKET ai_packet;

        struct {                     // size 0x158
            u8 unknown_02c8[0x14c];  // 0x02c8
            NUVEC ai_terrain_origin; // 0x0414
        };
    }; // 0x02c8

    u8 unknown_4d4[0x4];      // 0x04d4
    RTL_DATA lighting_packet; // 0x04d8
    u8 unknown_61c[0x45];     // 0x061c
    u8 unknown_661;           // 0x0661
    i16 sock_segment;         // 0x0662
    u8 unknown_664[0xc];      // 0x0664

    union { // size 0x38
        SOCKPOSITION sock_position;

        struct {                  // size 0x30
            u8 unknown_0670;      // 0x0670
            u8 sock_index;        // 0x0671
            u8 unknown_672[0x14]; // 0x0672
            u16 yrot;             // 0x0686
            u8 unknown_688[0x4];  // 0x0688
            f32 unknown_68c;      // 0x068c
            u8 unknown_690[0x6];  // 0x0690
            u16 sock_heading;     // 0x0696
            u8 unknown_0698[8];   // 0x0698
        };
    }; // 0x0670

    NUVEC contact_position; // 0x06a8
    NUVEC contact_normal;   // 0x06b4

    union { // size 0x4
        u32 contact_material_word;
        u8 contact_material[4];
    }; // 0x06c0

    PLAYERPACKET player;                // 0x06c4
    NUVEC unknown_e4c;                  // 0x0e4c
    NUVEC unknown_e58;                  // 0x0e58
    SPLINEPOS movement_spline_position; // 0x0e64
    NUVEC movement_spline_offset;       // 0x0e84
    f32 movement_spline_lateral_speed;  // 0x0e90
    NUVEC render_offset;                // 0x0e94

    union { // size 0xc
        NUVEC unknown_ea0;
        struct gameobject_s *field_ea0_target;

        struct {                                     // size 0xc
            struct gameobject_s *script_fire_target; // 0x0ea0

            union { // size 0x4
                struct gameobject_s *reciprocal_target;
                struct gameobject_s *takeover_target;
            }; // 0x0ea4

            void (*tag_object_callback)(struct gameobject_s *); // 0x0ea8
        };
    }; // 0x0ea0

    NUVEC *context_target_position;    // 0x0eac
    u64 seen_opponents;                // 0x0eb0
    u64 excluded_opponents;            // 0x0eb8
    struct gameobject_s *alert_target; // 0x0ec0
    f32 alert_timer;                   // 0x0ec4
    f32 movement_speed_scale;          // 0x0ec8
    f32 timer_ecc;                     // 0x0ecc

    union { // size 0x4
        f32 alert_target_timer;
        f32 ai_jump_destination_distance;
    }; // 0x0ed0

    f32 run_speed_override;        // 0x0ed4
    f32 walk_speed_override;       // 0x0ed8
    f32 jump_destination_distance; // 0x0edc
    f32 unknown_ee0;               // 0x0ee0
    f32 tag_reset_timer;           // 0x0ee4
    f32 pause_context_state;       // 0x0ee8

    union { // size 0x4
        u32 portal_flags;

        struct {                        // size 0x4
            u32 portal_bits_0_19 : 20;  // 0x0eec:0
            u32 respawnable : 1;        // 0x0eec:20
            u32 respawn_at_origin : 1;  // 0x0eec:21
            u32 portal_bits_22_31 : 10; // 0x0eec:22
        };

        struct {              // size 0x4
            u8 combat_flags;  // 0x0eec
            u8 combat_flags2; // 0x0eed
            u8 combat_flags3; // 0x0eee
            u8 combat_flags4; // 0x0eef
        };
    }; // 0x0eec

    union { // size 0x4
        u32 flags_ef0;

        struct {            // size 0x4
            u8 unknown_ef0; // 0x0ef0
            u8 unknown_ef1; // 0x0ef1
            u8 unknown_ef2; // 0x0ef2
            u8 flags_ef3;   // 0x0ef3
        };

        struct {                               // size 0x4
            u32 : 11;                          // 0x0ef0:0
            u32 doomed_escape_take_damage : 1; // 0x0ef0:11
            u32 still_take_hit : 1;            // 0x0ef0:12
            u32 : 8;                           // 0x0ef0:13
            u32 character_toggled : 1;         // 0x0ef0:21
            u32 invulnerable : 1;              // 0x0ef0:22
            u32 : 9;                           // 0x0ef0:23
        };
    }; // 0x0ef0

    union { // size 0x4
        u32 flags_ef4;
        u8 ai_update_flags;
    }; // 0x0ef4

    union { // size 0x4
        u32 flags_ef8_word;

        struct {            // size 0x4
            u8 unknown_ef8; // 0x0ef8
            u8 unknown_ef9; // 0x0ef9
            u8 field_efa;   // 0x0efa
            u8 field_efb;   // 0x0efb
        };
    }; // 0x0ef8

    void *can_use_object; // 0x0efc

    union { // size 0x4

        struct {                  // size 0x4
            u8 unknown_f00;       // 0x0f00
            u8 use_action_frames; // 0x0f01
            u8 unknown_f02[0x2];  // 0x0f02
        };

        u32 use_action_word;
    }; // 0x0f00

    union { // size 0x4
        f32 use_action_parameter;
        u32 use_action_parameter_word;
    }; // 0x0f04

    i32 route_frame_state; // 0x0f08
    f32 big_jump_height;   // 0x0f0c
    f32 unknown_f10;       // 0x0f10

    union { // size 0x4
        u32 unknown_f14;
        AILOCATOR *doomed_escape_locator;
    }; // 0x0f14

    NUVEC target_velocity;                     // 0x0f18
    NUVEC surface_normal;                      // 0x0f24
    NUVEC facing_direction;                    // 0x0f30
    NUHGOBJ_JOINT_OVERRIDE joint_overrides[3]; // 0x0f3c
    NUVEC *head_target;                        // 0x0fd8
    NUVEC head_target_position;                // 0x0fdc
    f32 head_target_timer;                     // 0x0fe8
    f32 head_target_delay;                     // 0x0fec
    f32 character_bottom;                      // 0x0ff0
    f32 character_top;                         // 0x0ff4
    f32 collision_scale;                       // 0x0ff8
    f32 collision_radius_unscaled;             // 0x0ffc
    f32 spawn_protection_timer;                // 0x1000
    f32 collision_y_scale;                     // 0x1004
    f32 death_timer;                           // 0x1008
    f32 death_delay;                           // 0x100c
    f32 unknown_1010;                          // 0x1010
    f32 reflection_height;                     // 0x1014
    f32 vehicle_hit_timer;                     // 0x1018
    f32 ai_update_distance;                    // 0x101c
    f32 shadow_opacity;                        // 0x1020
    f32 shadow_radius;                         // 0x1024
    f32 hover_height_override;                 // 0x1028
    f32 collision_scale_override;              // 0x102c
    f32 terrain_impact_speed;                  // 0x1030
    f32 animation_speed_multiplier;            // 0x1034
    f32 pod_boost_timer;                       // 0x1038
    f32 fall_acceleration_timer;               // 0x103c
    struct CABLE_s *cable;                     // 0x1040
    u32 flags_1044;                            // 0x1044
    u32 layer_mask;                            // 0x1048
    u32 extra_layer_mask;                      // 0x104c

    union { // size 0x2
        i16 unknown_1050;
        u16 lift_clear_state;
    }; // 0x1050

    u16 unknown_1052;            // 0x1052
    u16 unknown_1054;            // 0x1054
    u16 unknown_1056;            // 0x1056
    u16 unknown_1058;            // 0x1058
    u16 shadow_joint_mask;       // 0x105a
    u16 unknown_105c;            // 0x105c
    u16 unknown_105e;            // 0x105e
    u16 previous_movement_angle; // 0x1060
    u16 unknown_1062;            // 0x1062
    u16 id;                      // 0x1064
    i16 route_character_id;      // 0x1066

    union { // size 0x2
        u16 reflection_x;
        i16 route_suit_index;
    }; // 0x1068

    union { // size 0x2
        i16 portal_room;
        u16 reflection_z;
    }; // 0x106a

    i16 terrain_platform_id; // 0x106c
    i16 unknown_106e;        // 0x106e
    i16 level_object_index;  // 0x1070

    union { // size 0x6
        u8 unknown_1072[6];
        u8 ai_frame_event;
        AI_SPECIAL_PROGRESS special_move_progress_view;
        INDY_GUIDELINE_STATE indy_guideline_state;
    }; // 0x1072

    u8 contact_count;    // 0x1078
    u8 use_model_origin; // 0x1079

    union { // size 0x1
        u8 flags_107a;
        i8 movement_mode;
    }; // 0x107a

    u8 reflection_enabled; // 0x107b

    union { // size 0x2
        i16 unknown_107c;

        struct {                     // size 0x2
            u8 reflection_active;    // 0x107c
            u8 joint_override_count; // 0x107d
        };
    }; // 0x107c

    u8 unknown_107e;         // 0x107e
    i8 current_hp;           // 0x107f
    i8 head_target_priority; // 0x1080
    u8 route_start_index;    // 0x1081
    u8 helmet;               // 0x1082
    u8 unknown_1083;         // 0x1083
    u8 one_at_once_player;   // 0x1084
    u8 attack_override;      // 0x1085
    u8 unknown_1086;         // 0x1086
    u8 unknown_1087;         // 0x1087
    u8 unknown_1088;         // 0x1088
    u8 route_search_index;   // 0x1089
    u8 unknown_108a[2];      // 0x108a

    union { // size 0x4

        struct {                // size 0x4
            u8 unknown_108c[2]; // 0x108c
            u8 unknown_108e;    // 0x108e
            u8 unknown_108f;    // 0x108f
        };

        u32 field_108c;
        struct ai_helper_slot_s *indy_connection_helper;
    }; // 0x108c

    union { // size 0x4

        struct {                // size 0x4
            u8 unknown_1090[2]; // 0x1090
            u8 unknown_1092;    // 0x1092
            u8 unknown_1093;    // 0x1093
        };

        f32 special_move_timer;
    }; // 0x1090

    union { // size 0x4

        struct {                // size 0x4
            u8 unknown_1094;    // 0x1094
            u8 unknown_1095;    // 0x1095
            u8 unknown_1096[2]; // 0x1096
        };

        struct AIPATHNODE *special_move_source;
    }; // 0x1094

    union { // size 0xc

        struct {                  // size 0xc
            u32 unknown_1098;     // 0x1098
            u32 unknown_109c;     // 0x109c
            u8 unknown_10a0[0x4]; // 0x10a0
        };

        NUVEC special_move_look_position;
    }; // 0x1098
    struct gameobject_s *opponent;                // 0x10a4
    struct gameobject_s *last_attacker;           // 0x10a8
    void *snake_body;                             // 0x10ac
    void (*move_override)(struct gameobject_s *); // 0x10b0

    union { // size 0x4
        u8 unknown_10b4[4];
        struct climb_slot_s *indy_climb;
    }; // 0x10b4

    f32 ai_elapsed_time;                             // 0x10b8
    NUVEC ai_update_position;                        // 0x10bc
    f32 ai_vertical_velocity;                        // 0x10c8
    struct gizforce_s *gizforce_target;              // 0x10cc
    struct gameanimobj_s *gizforce_target_object;    // 0x10d0
    struct AITRIGGERSET_PARTIAL *active_trigger_set; // 0x10d4
} GAMEOBJECT;

#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectVelocities(GAMEOBJECT *);
    GAMEOBJECT *APIObjectCreate(APIOBJECTSYS *);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectRemoveFromLOSTable(APIOBJECTSYS *system, GAMEOBJECT *source, GAMEOBJECT *target);
    void APIObjectDestroy(APIOBJECTSYS *system, GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 InModelList(struct apicharactermodellist_s *, i32, i32 *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 animduration_blendouttime;
    i32 AnimPacketModelsCompatible(ANIMPACKET *packet, i32 source_id, i32 target_id);
    i32 QuickNewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags, f32 max_distance, f32 step);
    void SetAnimBlendMode(i32 mode);
#ifdef __cplusplus
}
#endif
