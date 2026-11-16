#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aipath.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"

struct AIACTIONDEF;
struct AICONDITIONDEF;
struct AIAREA;

typedef struct AISCRIPT AISCRIPT;
typedef struct AISTATE AISTATE;
typedef struct AIACTION AIACTION;
typedef struct AICONDITION AICONDITION;
typedef struct AIREFSCRIPT AIREFSCRIPT;
typedef struct AISCRIPTPROCESS AISCRIPTPROCESS;

typedef struct AISCRIPTPROCESSSTACK { // size 0x14
    f32 complex_params[4];            // 0x00
    u8 is_first_time_state;           // 0x10
    u8 force_complex_eval;            // 0x11
    u8 unknown_12[2];                 // 0x12
} AISCRIPTPROCESSSTACK;

/* Not converted: these asserts balance VC8 symbol ids for fragile functions (docs/cleanup-audit.md, section 6). */

struct AISCRIPTPROCESS {                 // size 0xc8
    AISCRIPT *base_script;               // 0x00
    AISCRIPT *script;                    // 0x04
    AISTATE *state;                      // 0x08
    AIACTION *action_node;               // 0x0c
    AISTATE *next_state;                 // 0x10
    f32 params[4];                       // 0x14
    f32 script_timer;                    // 0x24
    AISCRIPTPROCESSSTACK param_stack[2]; // 0x28
    u32 flags;                           // 0x50
    AIREFSCRIPT *active_refs[4];         // 0x54
    i32 active_ref_count;                // 0x64
    u8 action_data_1;                    // 0x68
    u8 action_data_2;                    // 0x69
    u16 action_data_6;                   // 0x6a
    void *action_data_3;                 // 0x6c
    f32 action_data_4;                   // 0x70
    f32 action_data_5;                   // 0x74
    NUVEC action_pos;                    // 0x78
    AIPATHINFO path_info;                // 0x84
    f32 action_timer;                    // 0x9c
    struct AIAREA *trigger_area;         // 0xa0
    struct AILOCATOR *locator;           // 0xa4
    struct AILOCATORSET *locator_set;    // 0xa8

    union { // size 0x4
        u32 field_ac;
        struct nugspline_s *myspline;
    }; // 0xac

    u8 creature_set;          // 0xb0
    u8 unknown_b1;            // 0xb1
    u16 unknown_b2;           // 0xb2
    u8 interrupt_priority;    // 0xb4
    u8 interrupt_id;          // 0xb5
    u16 action_data_7;        // 0xb6
    f32 interrupt_timer;      // 0xb8
    AISTATE *interrupt_state; // 0xbc
    AISTATE *return_to_state; // 0xc0
    void *local_messages;     // 0xc4
};

/* AISCRIPTPROCESS.action_data_1 holds each action's own options; the bits below are set by the keyword in
 * the comment. The follow actions (FollowOpponent, FollowPlayer, FollowCharacter, GameFollowPlayer) pass them
 * on as FollowAPIObject's flags. */
#define AIFOLLOW_CAN_GO_OFF_PATH 0x1 /* can_go_off_path: leave the path to reach a target on it */
#define AIFOLLOW_IGNORE_RADIUS 0x2   /* ignore_radius: no stopping distance around the target */
/* Circle, CirclePlayer and CircleOpponent. */
#define AICIRCLE_FACING 0x1           /* facing: look at the centre */
#define AICIRCLE_TARGET_ORIGIN 0x2    /* can_go_off_path: circle the target's terrain origin, not its path position */
#define AICIRCLE_CURRENT_DIST 0x4     /* currentdist: the radius is the distance when the action starts */
#define AICIRCLE_CURRENT_POSITION 0x8 /* current_position (Circle's default): circle the start position */
#define AICIRCLE_LOCATOR 0x10         /* locator=: circle a locator */
#define AICIRCLE_ORIGIN 0x20          /* origin: circle the creature's origin */
/* GoToLocator. */
#define AIGOTO_ON_GROUND 0x1               /* on_ground */
#define AIGOTO_XZ_RANGECHECK 0x2           /* xz_rangecheck */
#define AIGOTO_FACE_OPPONENT 0x4           /* face_opponent */
#define AIGOTO_IGNORE_PATH 0x8             /* ignore_path */
#define AIGOTO_MUST_REACH_DESTINATION 0x10 /* must_reach_destination: jump when the path is blocked */
/* AttackOpponent, EngageOpponent and ShootAtOpponent. */
#define AIENGAGE_OFFSCREEN 0x1          /* offscreen: also fire while not drawn */
#define AIENGAGE_KEEP_RANGE 0x2         /* minrange=, or held back by the one-at-once rule: keep the goal range */
#define AIENGAGE_NO_FIRE_IN_MINICUT 0x4 /* no_fire_in_minicut */
#define AIENGAGE_CIRCLE 0x8             /* circle: circle the opponent inside the goal range */
#define AIENGAGE_BACKING_OFF 0x10       /* walking backwards inside the goal range (turning circle characters) */
#define AIENGAGE_BACK_OFF 0x20          /* set in LSW1: back off inside the goal range */

typedef struct AIACTIONMACRO AIACTIONMACRO;
typedef struct AICONDITIONMACRO AICONDITIONMACRO;

struct AIACTIONMACRO { // size 0x14
    NULISTLNK link;    // 0x00
    char *name;        // 0x08
    NULISTHDR actions; // 0x0c
};

struct AICONDITIONMACRO { // size 0x14
    NULISTLNK link;       // 0x00
    char *name;           // 0x08
    NULISTHDR conditions; // 0x0c
};

typedef struct AISCRIPTPARAM { // size 0x8
    char *name;                // 0x0
    f32 default_value;         // 0x4
} AISCRIPTPARAM;

struct AIACTION {                   // size 0x14
    NULISTLNK link;                 // 0x00
    char **params;                  // 0x08
    i32 param_count;                // 0x0c
    struct AIACTIONDEF *definition; // 0x10
};

struct AICONDITION {                   // size 0x2c
    NULISTLNK link;                    // 0x00
    f32 param_value;                   // 0x08
    i8 comparison;                     // 0x0c
    i8 param_index;                    // 0x0d
    u16 flags;                         // 0x0e
    char *complex_expression;          // 0x10
    char *argument;                    // 0x14
    void *initialized_argument;        // 0x18
    struct AICONDITIONDEF *definition; // 0x1c
    char *next_state_name;             // 0x20
    AISTATE *next_state;               // 0x24
    AICONDITION *parameter_condition;  // 0x28
};

/* AICONDITION.flags, set by the script parser */
#define AICONDITION_AND 0x1                   /* "and": the next condition must hold too */
#define AICONDITION_KEEP_BLOCKED_MESSAGES 0x2 /* "goto ... KeepBlockedMessages" */
#define AICONDITION_PARAM 0x4                 /* compares with a script parameter ("param" or its name) */
#define AICONDITION_COMPLEX 0x8               /* compares with an expression evaluated at run time */

/* AISCRIPTPROCESS.flags */
#define AISCRIPTPROCESS_NEW_ACTION 0x1            /* the current action has not run yet: its first_time call is due */
#define AISCRIPTPROCESS_INACTIVE 0x2              /* a level script switched off by its AI gizmo */
#define AISCRIPTPROCESS_KEEP_BLOCKED_MESSAGES 0x4 /* the state change keeps the blocked message count */

struct AIREFSCRIPT {              // size 0x24
    NULISTLNK link;               // 0x00
    char *name;                   // 0x08
    AISCRIPT *script;             // 0x0c
    char *return_state_name;      // 0x10
    AISTATE *return_state;        // 0x14
    u32 check_global_scripts : 1; // 0x18:0
    u32 check_level_scripts : 1;  // 0x18:1
    NULISTHDR conditions;         // 0x1c
};

struct AISTATE {                 // size 0x24
    NULISTLNK link;              // 0x00
    NULISTHDR conditions;        // 0x08
    NULISTHDR actions;           // 0x10
    char *name;                  // 0x18
    NULISTHDR reference_scripts; // 0x1c
};

struct AISCRIPT {                // size 0x58
    NULISTLNK link;              // 0x00
    char *name;                  // 0x08
    char *derived_from;          // 0x0c
    NULISTHDR states;            // 0x10
    AISCRIPTPARAM params[4];     // 0x18
    AISTATE *base_state;         // 0x38
    u32 flags_3c;                // 0x3c
    NULISTHDR reference_scripts; // 0x40
    NULISTHDR condition_macros;  // 0x48
    NULISTHDR action_macros;     // 0x50
};
