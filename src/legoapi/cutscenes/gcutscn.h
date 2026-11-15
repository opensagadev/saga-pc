/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART == 1 && !defined(SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP)

struct ANI3_HEADER;
struct CUT_LOOKAT_STATE;
union DWA_CURVE_CELL;
struct NUANIMDATA2_SKELETAL;
struct NUHGOBJ_EVAL;
struct cut_load_locator_instance_s;
struct instnugcutscene_s;
struct numtl_s;
struct nuspecial_handle_s;

typedef union DWA_FRAME_HEADER {
    f32 duration;
    u32 magic;
} DWA_FRAME_HEADER;

typedef union DWA_NODE_COUNT {
    i16 legacy;
    u16 modern;
} DWA_NODE_COUNT;

typedef struct NUANIMDATA2 {      // size 0x14
    DWA_FRAME_HEADER header;      // 0x00
    DWA_NODE_COUNT node_count;    // 0x04
    i16 curve_count;              // 0x06
    i16 chunk_count;              // 0x08
    u16 modern_frame_count;       // 0x0a
    union DWA_CURVE_CELL *curves; // 0x0c
    i8 *curve_types;              // 0x10
} NUANIMDATA2;

typedef struct CUT_LOCATOR_ANIMATION { // size 0x54
    NUMTX base_matrix;                 // 0x00
    NUVEC pivot;                       // 0x40
    f32 locator_scale;                 // 0x4c
    struct ANI3_HEADER *animation;     // 0x50
} CUT_LOCATOR_ANIMATION;

typedef struct CUT_CHARACTER_ANIMATION { // size 0x64
    NUMTX base_matrix;                   // 0x00
    u8 unknown_40[4];                    // 0x40
    struct ANI3_HEADER *animation;       // 0x44
    struct NUANIMDATA2 *face_animation;  // 0x48
    struct NUANIMDATA2 *extra_animation; // 0x4c
    u8 unknown_50[0x4];                  // 0x50
    void *locator;                       // 0x54
    f32 animation_rate;                  // 0x58
    u8 flags;                            // 0x5c
    u8 animation_index;                  // 0x5d
    u8 has_locator;                      // 0x5e
    u8 unknown_5f;                       // 0x5f
    u8 locator_index;                    // 0x60
    u8 blend_time;                       // 0x61
    u8 unknown_62;                       // 0x62
    i8 start_frame;                      // 0x63
} CUT_CHARACTER_ANIMATION;

typedef struct cut_scene_load_s CUT_SCENE_LOAD_PC;
typedef struct cut_load_character_s CUT_LOAD_CHARACTER;
typedef struct cut_load_character_instance_s CUT_LOAD_CHARACTER_INSTANCE;
typedef struct cut_locator_function_entry_s CUT_LOCATOR_FUNCTION_ENTRY_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CutSceneCameraFocalLength;

    typedef struct cut_load_locator_s CUT_LOAD_LOCATOR;
    typedef struct cut_load_locator_type_s CUT_LOAD_LOCATOR_TYPE;
    typedef struct cut_load_locator_system_s CUT_LOAD_LOCATOR_SYSTEM;
    extern void (*NuCutSceneResetCharacters)(struct instnugcutscene_s *);
    typedef void (*CUT_DRAW_RENDER_CALLBACK_PC)(struct instnugcutscene_s *, void *, void *, void *, f32, i32);
    typedef void (*CUT_DRAW_EVAL_CALLBACK_PC)(struct instnugcutscene_s *, void *, void *, void *, f32);
    typedef void (*CUT_DRAW_RIGID_CALLBACK_PC)(void *, struct nuspecial_handle_s *, NUMTX *);
    void instGetLookAtLocatorInfo(struct instnugcutscene_s *, struct CUT_LOOKAT_STATE *);
    void instNuGCutLocatorUpdate(struct instnugcutscene_s *, CUT_LOAD_LOCATOR_SYSTEM *, struct cut_load_locator_instance_s *,
                                 CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    extern i32 CutSceneSingleStreamBuffer;
    extern CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctions;
    f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION *, NUMTX *, f32);
    void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
    i32 PCCutAni3CurveInteger(struct ANI3_HEADER *, f32 *, i32);
    extern const f64 AIPathAngularScale;

    typedef struct state_anim_s { // size 0xc
        u16 count;                // 0x0
        u16 unknown_0002;         // 0x2
        f32 *times;               // 0x4
        u8 *values;               // 0x8
    } STATE_ANIM_PC;

    void instNuGCutSceneDestroy(struct instnugcutscene_s *);
    void instNuGCutSceneEndButNotSystems(struct instnugcutscene_s *);

    void instNuGCutSceneEndFirstFrame(struct instnugcutscene_s *);
    void instNuGCutSceneStart(struct instnugcutscene_s *);
    void instNuGCutSceneReset(struct instnugcutscene_s *);
    struct instnugcutscene_s *instNuGCutSceneFindActive(char *name);
    void NuGCutSceneSysRender(i32);
    struct cut_load_rigid_s;
    extern i32 CutSceneRenderRigids;
    extern i32 CutSceneRenderCharacters;
    extern i32 CutSceneRenderLocators;

    void NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed);
    extern NUMTX CutSceneCameraMatrix;
    extern NUVEC CutSceneFocusLocation;
    extern u8 CutSceneUseFocusLocation;
    extern f32 CutSceneFocusDistance;
    extern f32 CutSceneFStop;
    extern i32 CutSceneCameraChanged;
    extern i32 CutSceneCameraDofMode;
    extern struct instnugcutscene_s *CutSceneLoadingInstance;
    extern i32 CutSceneUpdateCameras;
    i32 instNuGCutSceneIsFinished(struct instnugcutscene_s *);
    void instNuGCutScenePause(struct instnugcutscene_s *, i32 paused);
    void instNuGCutSceneStop(struct instnugcutscene_s *);
    void NuGCutSceneSysPostBackgroundLoad(void);
    i32 instNuGCutSceneSwapBuffers(struct instnugcutscene_s *, i32 force);
    void instNuGCutSceneServiceLoad(void);
    extern i32 CutSceneStreamLoadGuard;
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

typedef struct CUT_RIGID_ANIMATION { // size 0x50
    NUMTX base_matrix;               // 0x00
    char *name;                      // 0x40
    struct NUGSCN_s *scene;          // 0x44
    void *special_object;            // 0x48
    struct ANI3_HEADER *animation;   // 0x4c
} CUT_RIGID_ANIMATION;

#ifdef __cplusplus
extern "C" {
#endif
    i32 FindTexAnimFromMtl(struct NUHGOBJ_EVAL *, struct numtl_s *);
    void CutGetJointOverrideMatrix(struct NUHGOBJ_EVAL *, i32, NUMTX *);
    void PCCutRigidAni3(CUT_RIGID_ANIMATION *, f32, NUMTX *);
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

typedef struct CUT_LOCATOR_GENERIC {        // size 0x54
    NUMTX base_matrix;                      // 0x00
    u8 unknown_40_4f[16];                   // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x50
} CUT_LOCATOR_GENERIC;

typedef struct CUT_RIGID_GENERIC {          // size 0x50
    NUMTX base_matrix;                      // 0x00
    u8 unknown_40_4b[12];                   // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x4c
} CUT_RIGID_GENERIC;

typedef struct CUT_CHARACTER_GENERIC {      // size 0x64
    NUMTX base_matrix;                      // 0x00
    char *name;                             // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x44
    struct NUANIMDATA2 *face_animation;     // 0x48
    struct NUANIMDATA2 *extra_animation;    // 0x4c
    u8 unknown_50[0x4];                     // 0x50
    void *locator;                          // 0x54
    f32 animation_rate;                     // 0x58
    u8 flags;                               // 0x5c
    u8 animation_index;                     // 0x5d
    u8 has_locator;                         // 0x5e
    u8 unknown_5f;                          // 0x5f
    u8 locator_index;                       // 0x60
    u8 blend_time;                          // 0x61
    u8 unknown_62;                          // 0x62
    i8 start_frame;                         // 0x63
} CUT_CHARACTER_GENERIC;

#ifdef __cplusplus
extern "C" {
#endif
    void NuGCutRigidCalcMtx(CUT_RIGID_GENERIC *, f32, NUMTX *);
#ifdef __cplusplus
}
#endif

typedef struct cut_load_locator_s { // size 0x64
    NUMTX matrix;                   // 0x00
    NUVEC pivot;                    // 0x40
    f32 scale;                      // 0x4c
    void *animation;                // 0x50
    u32 unknown_54;                 // 0x54
    u8 flags;                       // 0x58
    u8 type_index;                  // 0x59
    u8 unknown_5a;                  // 0x5a
    u8 tracks_self;                 // 0x5b
    u8 unknown_5c[8];               // 0x5c
} CUT_LOAD_LOCATOR;

typedef struct cut_load_locator_type_s { // size 0xc
    char *name;                          // 0x0
    u8 flags;                            // 0x4
    u8 unknown_05;                       // 0x5
    u16 function_index;                  // 0x6
    u32 unknown_08;                      // 0x8
} CUT_LOAD_LOCATOR_TYPE;

typedef struct cut_load_locator_system_s { // size 0xc
    CUT_LOAD_LOCATOR *locators;            // 0x0
    CUT_LOAD_LOCATOR_TYPE *types;          // 0x4
    u8 count;                              // 0x8
    u8 type_count;                         // 0x9
    u8 unknown_0a[2];                      // 0xa
} CUT_LOAD_LOCATOR_SYSTEM;

typedef struct cut_load_rigid_s {   // size 0x5c
    NUMTX matrix;                   // 0x00
    char *name;                     // 0x40
    NUGSCN *scene;                  // 0x44
    void *special;                  // 0x48
    void *animation;                // 0x4c
    STATE_ANIM_PC *state_animation; // 0x50

    union { // size 0x4
        i32 index;
        CUT_LOAD_LOCATOR *pointer;
    } locator; // 0x54

    u8 flags;         // 0x58
    u8 uses_locator;  // 0x59
    u8 locator_index; // 0x5a
    u8 unknown_5b;    // 0x5b
} CUT_LOAD_RIGID;

typedef struct cut_load_rigid_system_s {
    CUT_LOAD_RIGID *rigids;
    u16 count;
    u16 unused;
} CUT_LOAD_RIGID_SYSTEM;

struct cut_load_character_s {  // size 0x64
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
};

typedef struct cut_load_character_system_s {
    CUT_LOAD_CHARACTER *characters;
    u16 count;
} CUT_LOAD_CHARACTER_SYSTEM;

typedef struct cut_load_camera_s { // size 0x50
    NUMTX matrix;                  // 0x00
    u8 flags;                      // 0x40
    u8 unknown_41;                 // 0x41
    u8 animation_node;             // 0x42
    u8 focus_flags;                // 0x43
    u8 unknown_44[12];             // 0x44
} CUT_LOAD_CAMERA;

typedef struct cut_load_camera_system_s { // size 0x24
    i32 count;                            // 0x00
    CUT_LOAD_CAMERA *cameras;             // 0x04
    void *animation;                      // 0x08
    STATE_ANIM_PC *state_animation;       // 0x0c
    i8 starting_camera;                   // 0x10
    i8 target_camera_map[11];             // 0x11
    void *focus_animation;                // 0x1c
    STATE_ANIM_PC *focus_state_animation; // 0x20
} CUT_LOAD_CAMERA_SYSTEM;

typedef struct cut_load_trigger_event_s { // size 0xc
    i16 index;                            // 0x0
    u16 unknown_02;                       // 0x2
    char *name;                           // 0x4
    STATE_ANIM_PC *state_animation;       // 0x8
} CUT_LOAD_TRIGGER_EVENT;

typedef struct cut_load_trigger_system_s {
    i32 count;
    CUT_LOAD_TRIGGER_EVENT *events;
} CUT_LOAD_TRIGGER_SYSTEM;

struct cut_scene_load_s {                               // size 0x5c
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
};

typedef struct cut_load_camera_target_s { // size 0x10
    NUVEC *target;                        // 0x0
    f32 start_frame;                      // 0x4
    f32 duration;                         // 0x8
    i8 target_index;                      // 0xc
    u8 unknown_0d[3];                     // 0xd
} CUT_LOAD_CAMERA_TARGET;

typedef struct cut_load_camera_state_s { // size 0x4
    u8 flags;                            // 0x0
    u8 event_index;                      // 0x1
    u8 unknown_02[2];                    // 0x2
} CUT_LOAD_CAMERA_STATE;

typedef struct cut_load_camera_instance_s { // size 0x10
    CUT_LOAD_CAMERA_TARGET *targets;        // 0x0
    CUT_LOAD_CAMERA_STATE *states;          // 0x4
    u8 state_index;                         // 0x8
    i8 camera_index;                        // 0x9
    u8 next_target_index;                   // 0xa
    u8 target_capacity;                     // 0xb
    u8 target_count;                        // 0xc
    u8 focus_state_index;                   // 0xd
    i8 focus_index;                         // 0xe
    u8 unknown_0f;                          // 0xf
} CUT_LOAD_CAMERA_INSTANCE;

typedef struct cut_load_rigid_instance_s { // size 0x10
    NUSPECIALHANDLE special;               // 0x0
    u8 state_index;                        // 0xc
    u8 visible;                            // 0xd
    u8 unknown_0e[2];                      // 0xe
} CUT_LOAD_RIGID_INSTANCE;

typedef struct cut_load_rigid_instance_system_s {
    CUT_LOAD_RIGID_INSTANCE *rigids;
} CUT_LOAD_RIGID_INSTANCE_SYSTEM;

typedef struct cut_load_character_instance_system_s {
    CUT_LOAD_CHARACTER_INSTANCE *characters;
} CUT_LOAD_CHARACTER_INSTANCE_SYSTEM;

typedef struct cut_load_focus_target_s { // size 0x18
    u32 unknown_00[5];                   // 0x00
    CUT_LOAD_LOCATOR *self;              // 0x14
} CUT_LOAD_FOCUS_TARGET;

typedef struct cut_load_sound_state_s {
    u32 unknown_00[4];
} CUT_LOAD_SOUND_STATE;

typedef struct cut_load_locator_instance_s { // size 0x8
    f32 frame;                               // 0x0

    union { // size 0x4
        i32 index;
        CUT_LOAD_FOCUS_TARGET *focus;
        CUT_LOAD_SOUND_STATE *sound;
    } effect; // 0x4
} CUT_LOAD_LOCATOR_INSTANCE;

typedef struct cut_load_trigger_entry_s { // size 0x34
    char *name;                           // 0x00
    u8 unknown_04[0x30];                  // 0x04
} CUT_LOAD_TRIGGER_ENTRY;

typedef struct cut_load_trigger_flag_s { // size 0x4
    u16 unknown_00;                      // 0x0
    u8 flags;                            // 0x2
    u8 unknown_03;                       // 0x3
} CUT_LOAD_TRIGGER_FLAG;

typedef struct cut_load_trigger_owner_s { // size 0x10
    u8 unknown_00[8];                     // 0x0
    i32 count;                            // 0x8

    union { // size 0x4
        CUT_LOAD_TRIGGER_ENTRY *entries;
        CUT_LOAD_TRIGGER_FLAG *trigger_states;
    }; // 0xc
} CUT_LOAD_TRIGGER_OWNER;

typedef struct cut_load_trigger_state_s { // size 0x4
    u8 state_index;                       // 0x0
    u8 unknown_01[3];                     // 0x1
} CUT_LOAD_TRIGGER_STATE;

typedef struct cut_load_trigger_instance_s {
    CUT_LOAD_TRIGGER_OWNER *owner;
    CUT_LOAD_TRIGGER_STATE *states;
} CUT_LOAD_TRIGGER_INSTANCE;

typedef struct cut_instance_load_s { // size 0xf8
    INSTNUGCUTSCENE *next;           // 0x00
    INSTNUGCUTSCENE *previous;       // 0x04
    char name[16];                   // 0x08
    NUMTX matrix;                    // 0x18
    CUT_SCENE_LOAD_PC *scene;        // 0x58
    CUT_SCENE_LOAD_PC *saved_scene;  // 0x5c
    NUVEC center;                    // 0x60
    NUVEC bounds_min;                // 0x6c
    NUVEC bounds_max;                // 0x78
    f32 radius_squared;              // 0x84

    union { // size 0x4
        u32 flags_88;

        struct {                          // size 0x4
            u32 flags_88_bits_0_12 : 13;  // 0x88:0
            i32 repeat_count : 5;         // 0x88:13
            u32 flags_88_bit_18 : 1;      // 0x88:18
            u32 synchro : 8;              // 0x88:19
            u32 flags_88_bit_27 : 1;      // 0x88:27
            i32 active_stream_buffer : 1; // 0x88:28
        };
    }; // 0x88

    union { // size 0x4
        u32 flags_8c;

        struct {                 // size 0x4
            i32 skip_frames : 4; // 0x8c:0
        };
    }; // 0x8c

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

    union { // size 0x8

        struct {                   // size 0x8
            void *stream_buffer_0; // 0xbc
            void *stream_buffer_1; // 0xc0
        };

        void *stream_buffers[2];
    }; // 0xbc

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
} CUT_INSTANCE_LOAD_PC;

struct cut_locator_function_entry_s { // size 0x14
    char *name;                       // 0x00
    u32 unknown_04;                   // 0x04
    void (*reset)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                  CUT_LOAD_LOCATOR *); // 0x08
    u8 unknown_0c[8];                  // 0x0c
};

#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CUTSCENES_GCUTSCN_H_STOP
