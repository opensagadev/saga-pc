#ifndef SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART == 1 && !defined(SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP)

struct AIACTIONDEF_PC;
struct AICONDITIONDEF_PC;

typedef struct AISCRIPT_PC AISCRIPT_PC;
typedef struct AISTATE_PC AISTATE_PC;
typedef struct AIACTION_PC AIACTION_PC;
typedef struct AICONDITION_PC AICONDITION_PC;
typedef struct AIREFSCRIPT_PC AIREFSCRIPT_PC;
typedef struct AISCRIPTPROCESS_PC AISCRIPTPROCESS_PC;

typedef struct AISCRIPTPROCESSSTACK_PC {
    f32 complex_params[4];
    u8 is_first_time_state;
    u8 force_complex_eval;
    u8 unknown_12[2];
} AISCRIPTPROCESSSTACK_PC;

/* Not converted: these asserts balance VC8 symbol ids for fragile functions (docs/cleanup-audit.md, section 6). */
STATIC_ASSERT(sizeof(AISCRIPTPROCESSSTACK_PC) == 0x14);

struct AISCRIPTPROCESS_PC {
    AISCRIPT_PC *base_script;               // 0x00
    AISCRIPT_PC *script;                    // 0x04
    AISTATE_PC *state;                      // 0x08
    AIACTION_PC *action_node;               // 0x0c
    AISTATE_PC *next_state;                 // 0x10
    f32 params[4];                          // 0x14
    f32 script_timer;                       // 0x24
    AISCRIPTPROCESSSTACK_PC param_stack[2]; // 0x28
    u32 flags;                              // 0x50
    AIREFSCRIPT_PC *active_refs[4];         // 0x54
    i32 active_ref_count;                   // 0x64
    u8 action_data_1;                       // 0x68
    u8 action_data_2;                       // 0x69
    u16 action_data_6;                      // 0x6a
    void *action_data_3;                    // 0x6c
    f32 action_data_4;                      // 0x70
    f32 action_data_5;                      // 0x74
    NUVEC action_pos;                       // 0x78
    AIPATHINFO_PC path_info;                // 0x84
    f32 action_timer;                       // 0x9c
    u32 unknown_a0;                         // 0xa0
    struct AILOCATOR_PC *locator;           // 0xa4
    struct AILOCATORSET_PC *locator_set;    // 0xa8

    union {                           // 0xac
        u32 field_ac;                 // 0xac
        struct nugspline_s *myspline; // 0xac
    };

    u8 creature_set;             // 0xb0
    u8 unknown_b1;               // 0xb1
    u16 unknown_b2;              // 0xb2
    u8 interrupt_priority;       // 0xb4
    u8 interrupt_id;             // 0xb5
    u16 action_data_7;           // 0xb6
    f32 interrupt_timer;         // 0xb8
    AISTATE_PC *interrupt_state; // 0xbc
    AISTATE_PC *return_to_state; // 0xc0
    void *local_messages;        // 0xc4
}; // size 0xc8

typedef struct AIACTIONMACRO_PC AIACTIONMACRO_PC;
typedef struct AICONDITIONMACRO_PC AICONDITIONMACRO_PC;

struct AIACTIONMACRO_PC {
    NULISTLNK link;    // 0x00
    char *name;        // 0x08
    NULISTHDR actions; // 0x0c
}; // size 0x14

struct AICONDITIONMACRO_PC {
    NULISTLNK link;       // 0x00
    char *name;           // 0x08
    NULISTHDR conditions; // 0x0c
}; // size 0x14

typedef struct AISCRIPTPARAM_PC {
    char *name;        // 0x0
    f32 default_value; // 0x4
} AISCRIPTPARAM_PC;    // size 0x8

#undef SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART == 2 && !defined(SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP)

struct AIACTION_PC {
    NULISTLNK link;                    // 0x00
    char **params;                     // 0x08
    i32 param_count;                   // 0x0c
    struct AIACTIONDEF_PC *definition; // 0x10
}; // size 0x14

struct AICONDITION_PC {
    NULISTLNK link;                       // 0x00
    f32 param_value;                      // 0x08
    i8 comparison;                        // 0x0c
    i8 param_index;                       // 0x0d
    u16 flags;                            // 0x0e
    char *complex_expression;             // 0x10
    char *argument;                       // 0x14
    void *initialized_argument;           // 0x18
    struct AICONDITIONDEF_PC *definition; // 0x1c
    char *next_state_name;                // 0x20
    AISTATE_PC *next_state;               // 0x24
    AICONDITION_PC *parameter_condition;  // 0x28
}; // size 0x2c

#undef SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART == 3 && !defined(SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP)

struct AIREFSCRIPT_PC {
    NULISTLNK link;               // 0x00
    char *name;                   // 0x08
    AISCRIPT_PC *script;          // 0x0c
    char *return_state_name;      // 0x10
    AISTATE_PC *return_state;     // 0x14
    u32 check_global_scripts : 1; // 0x18:0
    u32 check_level_scripts : 1;  // 0x18:1
    NULISTHDR conditions;         // 0x1c
}; // size 0x24

struct AISTATE_PC {
    NULISTLNK link;              // 0x00
    NULISTHDR conditions;        // 0x08
    NULISTHDR actions;           // 0x10
    char *name;                  // 0x18
    NULISTHDR reference_scripts; // 0x1c
}; // size 0x24

struct AISCRIPT_PC {
    NULISTLNK link;              // 0x00
    char *name;                  // 0x08
    char *derived_from;          // 0x0c
    NULISTHDR states;            // 0x10
    AISCRIPTPARAM_PC params[4];  // 0x18
    AISTATE_PC *base_state;      // 0x38
    u32 flags_3c;                // 0x3c
    NULISTHDR reference_scripts; // 0x40
    NULISTHDR condition_macros;  // 0x48
    NULISTHDR action_macros;     // 0x50
}; // size 0x58

#undef SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_AI_AISYS_AISCRIPT_TYPES_H_STOP
