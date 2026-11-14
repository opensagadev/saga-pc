#ifndef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/cutscenes/cutscenes.h, nu2api/nu3d/nu3d_includes.h, nu2api/nu3d/nugscn.h,
 * nu2api/nu3d/numtl.h, nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/nucore/nugcutscene.h,
 * nu2api/numath/numtx.h, nu2api/numath/nuvec.h, nu2api/trigger.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/cutscenes/cutscenes.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/trigger.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 1 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef union DWA_FRAME_HEADER_PC {
    f32 duration;
    u32 magic;
} DWA_FRAME_HEADER_PC;

typedef union DWA_NODE_COUNT_PC {
    i16 legacy;
    u16 modern;
} DWA_NODE_COUNT_PC;

typedef struct NUANIMDATA2_PC {
    DWA_FRAME_HEADER_PC header;
    DWA_NODE_COUNT_PC node_count;
    i16 curve_count;
    i16 chunk_count;
    u16 modern_frame_count;
    DWA_CURVE_CELL_PC *curves;
    i8 *curve_types;
} NUANIMDATA2_PC;

typedef struct CUT_LOCATOR_ANIMATION_PC {
    NUMTX base_matrix;
    NUVEC pivot;
    f32 locator_scale;
    ANI3_HEADER_PC *animation;
} CUT_LOCATOR_ANIMATION_PC;

typedef struct CUT_CHARACTER_ANIMATION_PC {
    NUMTX base_matrix;
    u8 unknown_40[4];
    ANI3_HEADER_PC *animation;
    struct NUANIMDATA2_PC *face_animation;
    struct NUANIMDATA2_PC *extra_animation;
    u8 unknown_50[0x4];
    void *locator;
    f32 animation_rate;
    u8 flags;
    u8 animation_index;
    u8 has_locator;
    u8 field_5f;
    u8 locator_index;
    u8 blend_time;
    u8 unknown_62;
    i8 start_frame;
} CUT_CHARACTER_ANIMATION_PC;

STATIC_ASSERT(offsetof(CUT_LOCATOR_ANIMATION_PC, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation) == 0x44);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation_rate) == 0x58);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, flags) == 0x5c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, blend_time) == 0x61);
STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, start_frame) == 0x63);

STATIC_ASSERT(offsetof(CUT_CHARACTER_ANIMATION_PC, animation_index) == 0x5d);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_count) == 6);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, chunk_count) == 8);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, modern_frame_count) == 0x0a);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curves) == 0x0c);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_types) == 0x10);

STATIC_ASSERT(offsetof(NUANIMDATA2_PC, header) == 0);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, node_count) == 4);
STATIC_ASSERT(offsetof(NUANIMDATA2_PC, curve_types) == 0x10);
STATIC_ASSERT(sizeof(NUANIMDATA2_PC) == 0x14);

typedef struct cut_scene_load_pc_s CUT_SCENE_LOAD_PC;
typedef struct cut_load_character_s CUT_LOAD_CHARACTER;
typedef struct cut_load_character_instance_s CUT_LOAD_CHARACTER_INSTANCE;
typedef struct cut_locator_function_entry_pc_s CUT_LOCATOR_FUNCTION_ENTRY_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CutSceneCameraFocalLengthPc;

    typedef struct cut_load_locator_s CUT_LOAD_LOCATOR;
    typedef struct cut_load_locator_type_s CUT_LOAD_LOCATOR_TYPE;
    typedef struct cut_load_locator_system_s CUT_LOAD_LOCATOR_SYSTEM;
    extern void (*NuCutSceneResetCharacters)(INSTNUGCUTSCENE *);
    typedef void (*CUT_DRAW_RENDER_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    typedef void (*CUT_DRAW_EVAL_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
    typedef void (*CUT_DRAW_RIGID_CALLBACK_PC)(void *, NUSPECIALHANDLE *, NUMTX *);
    void instGetLookAtLocatorInfo(INSTNUGCUTSCENE *, CUT_LOOKAT_STATE_PC *);
    void instNuGCutLocatorUpdate(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                                 CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    extern i32 CutSceneSingleStreamBufferPc;
    extern CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctionsPc;
    f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION_PC *, NUMTX *, f32);
    void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION_PC *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
    i32 PCCutAni3CurveInteger(ANI3_HEADER_PC *, f32 *, i32);
    extern const f64 AIPathAngularScale;

    typedef struct state_anim_pc_s {
        u16 count;
        u16 unknown_0002;
        f32 *times;
        u8 *values;
    } STATE_ANIM_PC;

    STATIC_ASSERT(sizeof(STATE_ANIM_PC) == 12);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, count) == 0);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, times) == 4);
    STATIC_ASSERT(offsetof(STATE_ANIM_PC, values) == 8);
    void instNuGCutSceneDestroy(INSTNUGCUTSCENE *);
    void instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *);

    void instNuGCutSceneEndFirstFrame(INSTNUGCUTSCENE *);
    void instNuGCutSceneStart(INSTNUGCUTSCENE *);
    void instNuGCutSceneReset(INSTNUGCUTSCENE *);
    INSTNUGCUTSCENE *instNuGCutSceneFindActivePc(char *name);
    void NuGCutSceneSysRender(i32);
    struct cut_load_rigid_s;
    extern i32 CutSceneRenderRigidsPc;
    extern i32 CutSceneRenderCharactersPc;
    extern i32 CutSceneRenderLocatorsPc;

    void NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed);
    extern NUMTX CutSceneCameraMatrixPc;
    extern NUVEC CutSceneFocusLocationPc;
    extern u8 CutSceneUseFocusLocationPc;
    extern f32 CutSceneFocusDistancePc;
    extern f32 CutSceneFStopPc;
    extern i32 CutSceneCameraChangedPc;
    extern i32 CutSceneCameraDofModePc;
    extern INSTNUGCUTSCENE *CutSceneLoadingInstancePc;
    extern i32 CutSceneUpdateCamerasPc;
    i32 instNuGCutSceneIsFinished(INSTNUGCUTSCENE *);
    void instNuGCutScenePause(INSTNUGCUTSCENE *, i32 paused);
    void instNuGCutSceneStop(INSTNUGCUTSCENE *);
    void NuGCutSceneSysPostBackgroundLoad(void);
    i32 instNuGCutSceneSwapBuffers(INSTNUGCUTSCENE *, i32 force);
    void instNuGCutSceneServiceLoad(void);
    extern i32 CutSceneStreamLoadGuardPc;
    extern void *cutscene_load_instance;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 2 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef struct CUT_RIGID_ANIMATION_PC {
    NUMTX base_matrix;
    char *name;
    struct NUGSCN_s *scene;
    void *special_object;
    ANI3_HEADER_PC *animation;
} CUT_RIGID_ANIMATION_PC;

STATIC_ASSERT(offsetof(CUT_RIGID_ANIMATION_PC, animation) == 0x4c);
#ifdef __cplusplus
extern "C" {
#endif
    i32 FindTexAnimFromMtl(NUHGOBJ_EVAL_PC *, NUMTL *);
    void CutGetJointOverrideMatrixPc(NUHGOBJ_EVAL_PC *, i32, NUMTX *);
    void PCCutRigidAni3(CUT_RIGID_ANIMATION_PC *, f32, NUMTX *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 3 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

typedef struct CUT_LOCATOR_GENERIC_PC {
    NUMTX base_matrix;
    u8 unknown_40_4f[16];
    CUT_ANIMATION_PC *animation;
} CUT_LOCATOR_GENERIC_PC;

typedef struct CUT_RIGID_GENERIC_PC {
    NUMTX base_matrix;
    u8 unknown_40_4b[12];
    CUT_ANIMATION_PC *animation;
} CUT_RIGID_GENERIC_PC;

typedef struct CUT_CHARACTER_GENERIC_PC {
    NUMTX base_matrix;
    char *name;
    CUT_ANIMATION_PC *animation;
    struct NUANIMDATA2_PC *face_animation;
    struct NUANIMDATA2_PC *extra_animation;
    u8 unknown_50[0x4];
    void *locator;
    f32 animation_rate;
    u8 flags;
    u8 animation_index;
    u8 has_locator;
    u8 field_5f;
    u8 locator_index;
    u8 blend_time;
    u8 unknown_62;
    i8 start_frame;
} CUT_CHARACTER_GENERIC_PC;

STATIC_ASSERT(offsetof(CUT_LOCATOR_GENERIC_PC, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_RIGID_GENERIC_PC, animation) == 0x4c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation) == 0x44);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation_rate) == 0x58);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, flags) == 0x5c);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, animation_index) == 0x5d);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, blend_time) == 0x61);
STATIC_ASSERT(offsetof(CUT_CHARACTER_GENERIC_PC, start_frame) == 0x63);
#ifndef TYPES_OFF_11

struct cut_load_locator_s {
    NUMTX matrix;     // 0x00
    NUVEC pivot;      // 0x40
    f32 scale;        // 0x4c
    void *animation;  // 0x50
    u32 unknown_54;   // 0x54
    u8 flags;         // 0x58
    u8 type_index;    // 0x59
    u8 unknown_5a;    // 0x5a
    u8 tracks_self;   // 0x5b
    u8 unknown_5c[8]; // 0x5c
}; // size 0x64

struct cut_load_locator_type_s {
    char *name;         // 0x0
    u8 flags;           // 0x4
    u8 unknown_05;      // 0x5
    u16 function_index; // 0x6
    u32 unknown_08;     // 0x8
}; // size 0xc

struct cut_load_locator_system_s {
    CUT_LOAD_LOCATOR *locators;   // 0x0
    CUT_LOAD_LOCATOR_TYPE *types; // 0x4
    u8 count;                     // 0x8
    u8 type_count;                // 0x9
    u8 unknown_0a[2];             // 0xa
}; // size 0xc

typedef struct cut_load_rigid_s {
    NUMTX matrix;                   // 0x00
    char *name;                     // 0x40
    NUGSCN *scene;                  // 0x44
    void *special;                  // 0x48
    void *animation;                // 0x4c
    STATE_ANIM_PC *state_animation; // 0x50

    union {                        // 0x54
        i32 index;                 // 0x54
        CUT_LOAD_LOCATOR *pointer; // 0x54
    } locator;

    u8 flags;         // 0x58
    u8 uses_locator;  // 0x59
    u8 locator_index; // 0x5a
    u8 unknown_5b;    // 0x5b
} CUT_LOAD_RIGID;     // size 0x5c

typedef struct cut_load_rigid_system_s {
    CUT_LOAD_RIGID *rigids;
    u16 count;
    u16 unused;
} CUT_LOAD_RIGID_SYSTEM;

struct cut_load_character_s {
    NUMTX matrix;              // 0x00
    char *name;                // 0x40
    void *animation;           // 0x44
    void *face_animation;      // 0x48
    void *extra_animation;     // 0x4c
    void *model;               // 0x50
    CUT_LOAD_LOCATOR *locator; // 0x54
    u32 unknown_58;            // 0x58
    u8 flags;                  // 0x5c
    u8 unknown_5d;             // 0x5d
    u8 uses_locator;           // 0x5e
    u8 unknown_5f;             // 0x5f
    u8 locator_index;          // 0x60
    u8 unknown_61[3];          // 0x61
}; // size 0x64

typedef struct cut_load_character_system_s {
    CUT_LOAD_CHARACTER *characters;
    u16 count;
} CUT_LOAD_CHARACTER_SYSTEM;

typedef struct cut_load_character_animation_s {
    u32 unknown_00;             // 0x0
    void *animation;            // 0x4
    u8 unknown_08[8];           // 0x8
} CUT_LOAD_CHARACTER_ANIMATION; // size 0x10

typedef struct cut_load_camera_pc_s {
    NUMTX matrix;      // 0x00
    u8 flags;          // 0x40
    u8 unknown_41;     // 0x41
    u8 animation_node; // 0x42
    u8 focus_flags;    // 0x43
    u8 unknown_44[12]; // 0x44
} CUT_LOAD_CAMERA_PC;  // size 0x50

typedef struct cut_load_camera_system_s {
    i32 count;                            // 0x00
    CUT_LOAD_CAMERA_PC *cameras;          // 0x04
    void *animation;                      // 0x08
    STATE_ANIM_PC *state_animation;       // 0x0c
    i8 starting_camera;                   // 0x10
    i8 target_camera_map[11];             // 0x11
    void *focus_animation;                // 0x1c
    STATE_ANIM_PC *focus_state_animation; // 0x20
} CUT_LOAD_CAMERA_SYSTEM;                 // size 0x24

typedef struct cut_load_trigger_event_s {
    i16 index;                      // 0x0
    u16 unknown_02;                 // 0x2
    char *name;                     // 0x4
    STATE_ANIM_PC *state_animation; // 0x8
} CUT_LOAD_TRIGGER_EVENT;           // size 0xc

typedef struct cut_load_trigger_system_s {
    i32 count;
    CUT_LOAD_TRIGGER_EVENT *events;
} CUT_LOAD_TRIGGER_SYSTEM;

typedef struct cut_load_bounds_s {
    NUVEC min;     // 0x00
    NUVEC max;     // 0x0c
} CUT_LOAD_BOUNDS; // size 0x18

struct cut_scene_load_pc_s {
    i32 version;                                        // 0x00
    i32 string_delta;                                   // 0x04
    f32 duration;                                       // 0x08
    char *strings;                                      // 0x0c
    CUT_LOAD_CAMERA_SYSTEM *camera_system;              // 0x10
    CUT_LOAD_RIGID_SYSTEM *rigid_system;                // 0x14
    CUT_LOAD_CHARACTER_SYSTEM *character_system;        // 0x18
    CUT_LOAD_LOCATOR_SYSTEM *locator_system;            // 0x1c
    CUT_LOAD_BOUNDS *bounds;                            // 0x20
    CUT_LOAD_TRIGGER_SYSTEM *trigger_system;            // 0x24
    i32 relocation_delta;                               // 0x28
    char *filename;                                     // 0x2c
    void *stream_buffer_0;                              // 0x30
    void *stream_buffer_1;                              // 0x34
    u32 flags;                                          // 0x38
    i32 stream_buffer_size;                             // 0x3c
    u32 unknown_40;                                     // 0x40
    NUGSCN *scene;                                      // 0x44
    void *extra_scene;                                  // 0x48
    u32 stream_buffer_offset;                           // 0x4c
    CUT_LOAD_CHARACTER_ANIMATION *character_animations; // 0x50
    u16 *focus_indices;                                 // 0x54
    i32 loaded_size;                                    // 0x58
}; // size 0x5c

typedef struct cut_load_camera_target_pc_s {
    NUVEC *target;           // 0x0
    f32 start_frame;         // 0x4
    f32 duration;            // 0x8
    i8 target_index;         // 0xc
    u8 unknown_0d[3];        // 0xd
} CUT_LOAD_CAMERA_TARGET_PC; // size 0x10

typedef struct cut_load_camera_state_pc_s {
    u8 flags;               // 0x0
    u8 event_index;         // 0x1
    u8 unknown_02[2];       // 0x2
} CUT_LOAD_CAMERA_STATE_PC; // size 0x4

typedef struct cut_load_camera_instance_s {
    CUT_LOAD_CAMERA_TARGET_PC *targets; // 0x0
    CUT_LOAD_CAMERA_STATE_PC *states;   // 0x4
    u8 state_index;                     // 0x8
    i8 camera_index;                    // 0x9
    u8 next_target_index;               // 0xa
    u8 target_capacity;                 // 0xb
    u8 target_count;                    // 0xc
    u8 focus_state_index;               // 0xd
    i8 focus_index;                     // 0xe
    u8 unknown_0f;                      // 0xf
} CUT_LOAD_CAMERA_INSTANCE;             // size 0x10

typedef struct cut_load_rigid_instance_s {
    NUSPECIALHANDLE special; // 0x0
    u8 state_index;          // 0xc
    u8 visible;              // 0xd
    u8 unknown_0e[2];        // 0xe
} CUT_LOAD_RIGID_INSTANCE;   // size 0x10

typedef struct cut_load_rigid_instance_system_s {
    CUT_LOAD_RIGID_INSTANCE *rigids;
} CUT_LOAD_RIGID_INSTANCE_SYSTEM;

struct cut_load_character_instance_s {
    void *model;      // 0x00
    u8 unknown_04[8]; // 0x04
    f32 frame_a;      // 0x0c
    u8 unknown_10[5]; // 0x10
    u8 animation_a;   // 0x15
    u8 animation_b;   // 0x16
    u8 unknown_17;    // 0x17
}; // size 0x18

typedef struct cut_load_character_instance_system_s {
    CUT_LOAD_CHARACTER_INSTANCE *characters;
} CUT_LOAD_CHARACTER_INSTANCE_SYSTEM;

typedef struct cut_load_focus_target_s {
    u32 unknown_00[5];      // 0x00
    CUT_LOAD_LOCATOR *self; // 0x14
} CUT_LOAD_FOCUS_TARGET;    // size 0x18

typedef struct cut_load_sound_state_s {
    u32 unknown_00[4];
} CUT_LOAD_SOUND_STATE;

struct cut_load_locator_instance_s {
    f32 frame; // 0x0

    union {                           // 0x4
        i32 index;                    // 0x4
        CUT_LOAD_FOCUS_TARGET *focus; // 0x4
        CUT_LOAD_SOUND_STATE *sound;  // 0x4
    } effect;
}; // size 0x8

typedef struct cut_load_locator_instance_system_s {
    CUT_LOAD_LOCATOR_INSTANCE *locators;
} CUT_LOAD_LOCATOR_INSTANCE_SYSTEM;

typedef struct cut_load_trigger_entry_s {
    char *name;           // 0x00
    u8 unknown_04[0x30];  // 0x04
} CUT_LOAD_TRIGGER_ENTRY; // size 0x34

typedef struct cut_load_trigger_flag_pc_s {
    u16 unknown_00;         // 0x0
    u8 flags;               // 0x2
    u8 unknown_03;          // 0x3
} CUT_LOAD_TRIGGER_FLAG_PC; // size 0x4

typedef struct cut_load_trigger_owner_s {
    u8 unknown_00[8]; // 0x0
    i32 count;        // 0x8

    union {                                       // 0xc
        CUT_LOAD_TRIGGER_ENTRY *entries;          // 0xc
        CUT_LOAD_TRIGGER_FLAG_PC *trigger_states; // 0xc
    };
} CUT_LOAD_TRIGGER_OWNER; // size 0x10

typedef struct cut_load_trigger_state_pc_s {
    u8 state_index;          // 0x0
    u8 unknown_01[3];        // 0x1
} CUT_LOAD_TRIGGER_STATE_PC; // size 0x4

typedef struct cut_load_trigger_instance_s {
    CUT_LOAD_TRIGGER_OWNER *owner;
    CUT_LOAD_TRIGGER_STATE_PC *states;
} CUT_LOAD_TRIGGER_INSTANCE;

typedef struct cut_instance_load_pc_s {
    INSTNUGCUTSCENE *next;          // 0x00
    INSTNUGCUTSCENE *previous;      // 0x04
    char name[16];                  // 0x08
    NUMTX matrix;                   // 0x18
    CUT_SCENE_LOAD_PC *scene;       // 0x58
    CUT_SCENE_LOAD_PC *saved_scene; // 0x5c
    NUVEC center;                   // 0x60
    NUVEC bounds_min;               // 0x6c
    NUVEC bounds_max;               // 0x78
    f32 radius_squared;             // 0x84

    union {           // 0x88
        u32 flags_88; // 0x88

        struct {                          // 0x88
            u32 flags_88_bits_0_12 : 13;  // 0x88:0
            i32 repeat_count : 5;         // 0x88:13
            u32 flags_88_bit_18 : 1;      // 0x88:18
            u32 synchro : 8;              // 0x88:19
            u32 flags_88_bit_27 : 1;      // 0x88:27
            i32 active_stream_buffer : 1; // 0x88:28
        };
    };

    union {           // 0x8c
        u32 flags_8c; // 0x8c

        struct {                 // 0x8c
            i32 skip_frames : 4; // 0x8c:0
        };
    };

    f32 current_frame;                                      // 0x90
    f32 render_frame;                                       // 0x94
    f32 rate;                                               // 0x98
    CUT_LOAD_CAMERA_INSTANCE *camera_instance;              // 0x9c
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *rigid_instance;         // 0xa0
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *character_instance; // 0xa4
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *locator_instance;     // 0xa8
    CUT_LOAD_TRIGGER_INSTANCE *trigger_instance;            // 0xac
    INSTNUGCUTSCENE *chained_instance;                      // 0xb0
    void (*end_callback)(INSTNUGCUTSCENE *);                // 0xb4
    u32 unknown_b8;                                         // 0xb8

    union { // 0xbc

        struct {                   // 0xbc
            void *stream_buffer_0; // 0xbc
            void *stream_buffer_1; // 0xc0
        };

        void *stream_buffers[2]; // 0xbc
    };

    NUGSCN *scene_owner;                       // 0xc4
    CUT_LOAD_TRIGGER_OWNER *extra_scene_owner; // 0xc8
    u8 unknown_cc[12];                         // 0xcc
    u16 skip_countdown;                        // 0xd8
    u8 unknown_da[5];                          // 0xda
    u8 stream_index;                           // 0xdf
    f32 accumulated_duration;                  // 0xe0
    f32 elapsed;                               // 0xe4
    f32 alpha;                                 // 0xe8
    void *pending_stream;                      // 0xec
    u32 allocation_size;                       // 0xf0
    INSTNUGCUTSCENE *queued_stream_instance;   // 0xf4
} CUT_INSTANCE_LOAD_PC;                        // size 0xf8

struct cut_locator_function_entry_pc_s {
    char *name;     // 0x00
    u32 unknown_04; // 0x04
    void (*reset)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                  CUT_LOAD_LOCATOR *); // 0x08
    u32 unknown_0c;                    // 0x0c
    void (*update)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32,
                   NUMTX *, i32); // 0x10
}; // size 0x14

typedef struct CUT_LOOKAT_LOCATOR_INFO_PC {
    u8 unknown_00[0x5b];      // 0x00
    u8 character_id;          // 0x5b
    u8 joint;                 // 0x5c
    u8 material;              // 0x5d
    u8 secondary_material;    // 0x5e
    u8 tertiary_material;     // 0x5f
} CUT_LOOKAT_LOCATOR_INFO_PC; // size 0x60

struct CUT_LOOKAT_STATE_PC {
    NUHGOBJ_EVAL_PC *hierarchy;          // 0x00
    f32 blend;                           // 0x04
    u16 character;                       // 0x08
    u16 material;                        // 0x0a
    u16 texture;                         // 0x0c
    u16 secondary_material;              // 0x0e
    u16 tertiary_material;               // 0x10
    u16 joint;                           // 0x12
    CUT_LOOKAT_LOCATOR_INFO_PC *locator; // 0x14
}; // size 0x18

#endif /* TYPES_OFF_11 */
#ifdef __cplusplus
extern "C" {
#endif
    void NuGCutRigidCalcMtx(CUT_RIGID_GENERIC_PC *, f32, NUMTX *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
