#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct NUGSCN_s;
struct cut_load_character_animation_s;
struct cut_load_locator_instance_system_s;

struct ANI3_HEADER;
struct CUT_LOOKAT_STATE;
union DWA_CURVE_CELL;
struct NUANIMDATA2_SKELETAL;
struct NUHGOBJ_EVAL;
struct cut_load_locator_instance_s;
struct instnugcutscene_s;
struct numtl_s;
struct nuhspecial_s;

typedef struct CUT_LOCATOR_ANIMATION { // size 0x54
    NUMTX base_matrix;                 // 0x00
    NUVEC pivot;                       // 0x40
    f32 locator_scale;                 // 0x4c
    struct ANI3_HEADER *animation;     // 0x50
} CUT_LOCATOR_ANIMATION;

typedef struct CUT_CHARACTER_ANIMATION { // size 0x64
    NUMTX base_matrix;                   // 0x00
    u8 reserved_40[4];                   // 0x40
    struct ANI3_HEADER *animation;       // 0x44
    struct NUANIMDATA2 *face_animation;  // 0x48
    struct NUANIMDATA2 *extra_animation; // 0x4c
    u8 reserved_50[0x4];                 // 0x50
    void *locator;                       // 0x54
    f32 animation_rate;                  // 0x58
    u8 flags;                            // 0x5c
    u8 animation_index;                  // 0x5d
    u8 has_locator;                      // 0x5e
    u8 reserved_5f;                      // 0x5f
    u8 locator_index;                    // 0x60
    u8 blend_time;                       // 0x61
    u8 reserved_62;                      // 0x62
    i8 start_frame;                      // 0x63
} CUT_CHARACTER_ANIMATION;

typedef struct cut_scene_load_s CUT_SCENE_LOAD;
typedef struct cut_load_character_s CUT_LOAD_CHARACTER;
typedef struct cut_load_character_instance_s CUT_LOAD_CHARACTER_INSTANCE;
typedef struct cut_locator_function_entry_s CUT_LOCATOR_FUNCTION_ENTRY;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CutSceneCameraFocalLength;

    typedef struct cut_load_locator_s CUT_LOAD_LOCATOR;
    typedef struct cut_load_locator_type_s CUT_LOAD_LOCATOR_TYPE;
    typedef struct cut_load_locator_system_s CUT_LOAD_LOCATOR_SYSTEM;
    extern void (*NuCutSceneResetCharacters)(struct instnugcutscene_s *);
    typedef void (*CUT_DRAW_RENDER_CALLBACK)(struct instnugcutscene_s *, void *, void *, void *, f32, i32);
    typedef void (*CUT_DRAW_EVAL_CALLBACK)(struct instnugcutscene_s *, void *, void *, void *, f32);
    typedef void (*CUT_DRAW_RIGID_CALLBACK)(void *, struct nuhspecial_s *, NUMTX *);
    void instGetLookAtLocatorInfo(struct instnugcutscene_s *, struct CUT_LOOKAT_STATE *);
    void instNuGCutLocatorUpdate(struct instnugcutscene_s *, CUT_LOAD_LOCATOR_SYSTEM *, struct cut_load_locator_instance_s *,
                                 CUT_LOAD_LOCATOR *, f32, NUMTX *, i32);
    extern i32 CutSceneSingleStreamBuffer;
    extern CUT_LOCATOR_FUNCTION_ENTRY *CutSceneLocatorFunctions;
    f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION *, NUMTX *, f32);
    void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
    i32 PCCutAni3CurveInteger(struct ANI3_HEADER *, f32 *, i32);
    extern const f64 AIPathAngularScale;

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

typedef struct CUT_LOCATOR_GENERIC {        // size 0x54
    NUMTX base_matrix;                      // 0x00
    u8 reserved_40[16];                     // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x50
} CUT_LOCATOR_GENERIC;

typedef struct CUT_RIGID_GENERIC {          // size 0x50
    NUMTX base_matrix;                      // 0x00
    u8 reserved_40[12];                     // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x4c
} CUT_RIGID_GENERIC;

/* CUT_CHARACTER_GENERIC.flags (CUT_DRAW_CHARACTER.flags in cutscenes.cpp) */
#define CUT_CHARACTER_MODEL_ONLY 0x2  /* drawn from a loaded character model (CutSceneFindModel's model_only), not a player */
#define CUT_CHARACTER_SHADOW 0x10     /* casts the blob shadow */
#define CUT_CHARACTER_REFLECTION 0x20 /* drawn reflected within CutReflectRange */

typedef struct CUT_CHARACTER_GENERIC {      // size 0x64
    NUMTX base_matrix;                      // 0x00
    char *name;                             // 0x40
    struct NUANIMDATA2_SKELETAL *animation; // 0x44
    struct NUANIMDATA2 *face_animation;     // 0x48
    struct NUANIMDATA2 *extra_animation;    // 0x4c
    u8 reserved_50[0x4];                    // 0x50
    void *locator;                          // 0x54
    f32 animation_rate;                     // 0x58
    u8 flags;                               // 0x5c
    u8 animation_index;                     // 0x5d
    u8 has_locator;                         // 0x5e
    u8 reserved_5f;                         // 0x5f
    u8 locator_index;                       // 0x60
    u8 blend_time;                          // 0x61
    u8 reserved_62;                         // 0x62
    i8 start_frame;                         // 0x63
} CUT_CHARACTER_GENERIC;

#ifdef __cplusplus
extern "C" {
#endif
    void NuGCutRigidCalcMtx(CUT_RIGID_GENERIC *, f32, NUMTX *);
    extern i32 DefragCutSceneListSize;
    extern void (*DefragInstDestroyedFn)(void *);
    extern void *DefragCutSceneBaseMem;
    extern void *DefragInstBaseMem;
#ifdef __cplusplus
}
#endif

/* cut_load_locator_s.flags */
#define CUTLOC_PIVOT 0x4        /* the matrix is moved by the locator's pivot */
#define CUTLOC_FINITE_SHOT 0x10 /* its debris effect fires once (AddScaledFiniteShotDebrisEffect) */
#define CUTLOC_PERSISTENT 0x20  /* its debris effect is one instance that follows the locator (AddDebrisEffect) */

typedef struct cut_load_locator_s { // size 0x64
    NUMTX matrix;                   // 0x00
    NUVEC pivot;                    // 0x40
    f32 scale;                      // 0x4c
    void *animation;                // 0x50
    u32 reserved_54;                // 0x54
    u8 flags;                       // 0x58
    u8 type_index;                  // 0x59
    u8 reserved_5a;                 // 0x5a
    u8 tracks_self;                 // 0x5b
    u8 reserved_5c[8];              // 0x5c
} CUT_LOAD_LOCATOR;

/* cut_load_locator_type_s.flags: what drives the locators of the type, from CutSceneLocatorFixUp and the locator
 * update, which take the first bit set in this order: DEBRIS, FUNCTION, VFX, SFX, LOOK_AT. */
#define CUTLOCATOR_DEBRIS 0x1   /* a debris effect by name (LookupDebrisEffectPage) */
#define CUTLOCATOR_FUNCTION 0x2 /* a locator function by name (CutFindLocatorFunction) */
#define CUTLOCATOR_SFX 0x4      /* a sound (NuCutSceneRequestSFX, NuCutSceneSFXUpdate) */
#define CUTLOCATOR_LOOK_AT 0x8  /* CutSceneLocatorLookAtUpdate */
#define CUTLOCATOR_VFX 0x10     /* a visual effect (LookupLocatorVfxFn) */

typedef struct cut_load_locator_type_s { // size 0xc
    char *name;                          // 0x0
    u8 flags;                            // 0x4
    u8 reserved_05;                      // 0x5
    u16 function_index;                  // 0x6
    u32 reserved_08;                     // 0x8
} CUT_LOAD_LOCATOR_TYPE;

typedef struct cut_load_locator_system_s { // size 0xc
    CUT_LOAD_LOCATOR *locators;            // 0x0
    CUT_LOAD_LOCATOR_TYPE *types;          // 0x4
    u8 count;                              // 0x8
    u8 type_count;                         // 0x9
    u8 reserved_0a[2];                     // 0xa
} CUT_LOAD_LOCATOR_SYSTEM;

typedef struct cut_load_rigid_s { // size 0x5c
    NUMTX matrix;                 // 0x00
    char *name;                   // 0x40
    struct NUGSCN_s *scene;       // 0x44
    void *special;                // 0x48
    void *animation;              // 0x4c
    STATE_ANIM *state_animation;  // 0x50

    union { // size 0x4
        i32 index;
        CUT_LOAD_LOCATOR *pointer;
    } locator; // 0x54

    u8 flags;         // 0x58
    u8 uses_locator;  // 0x59
    u8 locator_index; // 0x5a
    u8 reserved_5b;   // 0x5b
} CUT_LOAD_RIGID;

/* CUT_LOAD_RIGID.flags */
#define CUTRIGID_VISIBLE 0x1   /* the state animation's visibility when it is instanced and found */
#define CUTRIGID_INSTANCED 0x2 /* NuGCutRigidForceInstanced */
#define CUTRIGID_FOUND 0x4     /* the fix-up found its special in the scene */

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
    u32 reserved_58;           // 0x58
    u8 flags;                  // 0x5c
    u8 reserved_5d;            // 0x5d
    u8 uses_locator;           // 0x5e
    u8 reserved_5f;            // 0x5f
    u8 locator_index;          // 0x60
    u8 reserved_61[3];         // 0x61
};

typedef struct cut_load_character_system_s {
    CUT_LOAD_CHARACTER *characters;
    u16 count;
} CUT_LOAD_CHARACTER_SYSTEM;

typedef struct cut_load_camera_s { // size 0x50
    NUMTX matrix;                  // 0x00
    u8 flags;                      // 0x40
    u8 reserved_41;                // 0x41
    u8 animation_node;             // 0x42
    u8 focus_flags;                // 0x43
    u8 reserved_44[12];            // 0x44
} CUT_LOAD_CAMERA;

typedef struct cut_load_camera_system_s { // size 0x24
    i32 count;                            // 0x00
    CUT_LOAD_CAMERA *cameras;             // 0x04
    void *animation;                      // 0x08
    STATE_ANIM *state_animation;          // 0x0c
    i8 starting_camera;                   // 0x10
    i8 target_camera_map[11];             // 0x11
    void *focus_animation;                // 0x1c
    STATE_ANIM *focus_state_animation;    // 0x20
} CUT_LOAD_CAMERA_SYSTEM;

typedef struct cut_load_trigger_event_s { // size 0xc
    i16 index;                            // 0x0
    u16 reserved_02;                      // 0x2
    char *name;                           // 0x4
    STATE_ANIM *state_animation;          // 0x8
} CUT_LOAD_TRIGGER_EVENT;

typedef struct cut_load_trigger_system_s {
    i32 count;
    CUT_LOAD_TRIGGER_EVENT *events;
} CUT_LOAD_TRIGGER_SYSTEM;

/* cut_scene_load_s.flags, from the cut scene loader and the stream code; 0x2 (kept over a reload) is not named yet. */
#define CUTSCENE_STREAMED 0x1       /* the animation streams through two stream buffers */
#define CUTSCENE_HAS_SFX 0x4        /* a locator type requested a sound */
#define CUTSCENE_STREAM_PENDING 0x8 /* a stream part not yet swapped in: its pointers are fixed up then */

struct cut_scene_load_s {                                        // size 0x5c
    i32 version;                                                 // 0x00
    i32 string_delta;                                            // 0x04
    f32 duration;                                                // 0x08
    char *strings;                                               // 0x0c
    CUT_LOAD_CAMERA_SYSTEM *camera_system;                       // 0x10
    CUT_LOAD_RIGID_SYSTEM *rigid_system;                         // 0x14
    CUT_LOAD_CHARACTER_SYSTEM *character_system;                 // 0x18
    CUT_LOAD_LOCATOR_SYSTEM *locator_system;                     // 0x1c
    CUT_LOAD_BOUNDS *bounds;                                     // 0x20
    CUT_LOAD_TRIGGER_SYSTEM *trigger_system;                     // 0x24
    i32 relocation_delta;                                        // 0x28
    char *filename;                                              // 0x2c
    void *stream_buffer_0;                                       // 0x30
    void *stream_buffer_1;                                       // 0x34
    u32 flags;                                                   // 0x38
    i32 stream_buffer_size;                                      // 0x3c
    u8 last_stream;                                              // 0x40
    u8 reserved_41;                                              // 0x41
    u16 total_stream_frames;                                     // 0x42
    struct NUGSCN_s *scene;                                      // 0x44
    void *extra_scene;                                           // 0x48
    u32 stream_buffer_offset;                                    // 0x4c
    struct cut_load_character_animation_s *character_animations; // 0x50
    u16 *focus_indices;                                          // 0x54
    i32 loaded_size;                                             // 0x58
};

typedef struct cut_load_camera_target_s { // size 0x10
    NUVEC *target;                        // 0x0
    f32 start_frame;                      // 0x4
    f32 duration;                         // 0x8
    i8 target_index;                      // 0xc
    u8 reserved_0d[3];                    // 0xd
} CUT_LOAD_CAMERA_TARGET;

typedef struct cut_load_camera_state_s { // size 0x4
    u8 flags;                            // 0x0
    u8 event_index;                      // 0x1
    u8 reserved_02[2];                   // 0x2
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
    u8 reserved_0f;                         // 0xf
} CUT_LOAD_CAMERA_INSTANCE;

typedef struct cut_load_rigid_instance_s { // size 0x10
    nuhspecial_s special;                  // 0x0
    u8 state_index;                        // 0xc
    u8 visible;                            // 0xd
    u8 reserved_0e[2];                     // 0xe
} CUT_LOAD_RIGID_INSTANCE;

typedef struct cut_load_rigid_instance_system_s {
    CUT_LOAD_RIGID_INSTANCE *rigids;
} CUT_LOAD_RIGID_INSTANCE_SYSTEM;

typedef struct cut_load_character_instance_system_s {
    CUT_LOAD_CHARACTER_INSTANCE *characters;
} CUT_LOAD_CHARACTER_INSTANCE_SYSTEM;

typedef struct cut_load_focus_target_s { // size 0x18
    u32 reserved_00[5];                  // 0x00
    CUT_LOAD_LOCATOR *self;              // 0x14
} CUT_LOAD_FOCUS_TARGET;

typedef struct cut_load_sound_state_s {
    u32 reserved_00[4];
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
    u8 reserved_04[0x30];                 // 0x04
} CUT_LOAD_TRIGGER_ENTRY;

typedef struct cut_load_trigger_flag_s { // size 0x4
    u16 reserved_00;                     // 0x0
    u8 flags;                            // 0x2
    u8 reserved_03;                      // 0x3
} CUT_LOAD_TRIGGER_FLAG;

typedef struct cut_load_trigger_owner_s { // size 0x10
    u8 reserved_00[8];                    // 0x0
    i32 count;                            // 0x8

    union { // size 0x4
        CUT_LOAD_TRIGGER_ENTRY *entries;
        CUT_LOAD_TRIGGER_FLAG *trigger_states;
    }; // 0xc
} CUT_LOAD_TRIGGER_OWNER;

typedef struct cut_load_trigger_state_s { // size 0x4
    u8 state_index;                       // 0x0
    u8 reserved_01[3];                    // 0x1
} CUT_LOAD_TRIGGER_STATE;

typedef struct cut_load_trigger_instance_s {
    CUT_LOAD_TRIGGER_OWNER *owner;
    CUT_LOAD_TRIGGER_STATE *states;
} CUT_LOAD_TRIGGER_INSTANCE;

struct cut_locator_function_entry_s { // size 0x14
    char *name;                       // 0x00
    u32 reserved_04;                  // 0x04
    void (*reset)(struct instnugcutscene_s *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                  CUT_LOAD_LOCATOR *); // 0x08
    u8 reserved_0c[8];                 // 0x0c
};

/* cut_script_node_s.type: the cut scene script command each node holds (CutScriptParseBlock keywords). */
#define CUTSCRIPT_START 0              /* STARTCUTSCENEINST */
#define CUTSCRIPT_PLAY 1               /* PLAYCUTSCENEINST forward|backward */
#define CUTSCRIPT_CHAIN 2              /* CHAINCUTSCENEINST */
#define CUTSCRIPT_REPEAT 3             /* REPEATCUTSCENEINST */
#define CUTSCRIPT_RESET 4              /* RESETCUTSCENEINST */
#define CUTSCRIPT_SET_TRIGGER_FLAG 5   /* SETTRIGGERFLAG */
#define CUTSCRIPT_RESET_TRIGGER 6      /* RESETTRIGGER */
#define CUTSCRIPT_SET_VISIBILITY 7     /* SETVISIBILITY */
#define CUTSCRIPT_IF_PARAM 8           /* IFPARAM ... ELSE ... ENDIF */
#define CUTSCRIPT_SET_PARAM 9          /* SETPARAM */
#define CUTSCRIPT_IF_TRIGGER_PULLED 10 /* IFTRIGGERPULLED, IFTRIGGERNOTPULLED (branches swapped) */
#define CUTSCRIPT_ENABLE_TRIGGER 11    /* ENABLETRIGGER */
#define CUTSCRIPT_PAUSE 12             /* PAUSECUTSCENEINST */
#define CUTSCRIPT_UNPAUSE 13           /* UNPAUSECUTSCENEINST */
#define CUTSCRIPT_APP_COMMAND 14       /* APPCOMMAND */
