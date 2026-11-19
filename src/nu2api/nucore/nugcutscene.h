#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/numath/numtx.h"

struct NUANIM_TIME2;
struct NUHGOBJ_EVAL;
struct cut_load_locator_instance_s;
struct nuhspecial_s;

/* instnugcutscene_s.flags_88 (and the cut scene instance views): what instNuGCutSceneStart, End, the
 * instNuGCutSceneSet* transforms and instNuGCutSceneClipTest set and test. */
#define NUGCUTSCENE_INST_PLAYING 0x2           /* set by Start, cleared by End */
#define NUGCUTSCENE_INST_OWN_MATRIX 0x80       /* set by every SetTranslation/Rotate call: draw and clip with the instance matrix */
#define NUGCUTSCENE_INST_CLIP_BOUNDS 0x100     /* ClipTest tests the scene bounds */
#define NUGCUTSCENE_INST_CLIP_RADIUS 0x200     /* ClipTest tests the camera distance */
#define NUGCUTSCENE_INST_VISIBLE 0x400         /* ClipTest's result */
#define NUGCUTSCENE_INST_ENDED 0x1000          /* set by End, cleared by Start and Reset */
#define NUGCUTSCENE_INST_ADVANCING 0x1         /* cleared by Start; the first update after it sets it, the frame advances from the next */
#define NUGCUTSCENE_INST_PAUSED 0x4            /* the cut script's pause command: the frame does not advance */
#define NUGCUTSCENE_INST_LOOP 0x8              /* CUTINFO_LOOPING: Start again at the end */
#define NUGCUTSCENE_INST_PING_PONG 0x10        /* the rate turns round at either end */
#define NUGCUTSCENE_INST_HOLD_FIRST_FRAME 0x20 /* played backwards, it stops on the first frame */
#define NUGCUTSCENE_INST_AUTO_START 0x40       /* the update starts it while it is not playing */
#define NUGCUTSCENE_INST_SUSPENDED 0x800       /* the update leaves it alone */
#define NUGCUTSCENE_INST_NEXT_STREAM_READY 0x8000000 /* a streamed scene's next part is loaded; without it the last frame is held */
#define NUGCUTSCENE_INST_STREAM_BUFFER_1 0x10000000  /* active_stream_buffer: the part plays from stream buffer 1 */
#define NUGCUTSCENE_INST_STREAMED 0x20000000         /* set at creation for a streamed scene: the update queues its loading */
#define NUGCUTSCENE_INST_SKIPPING 0x40000000         /* a frame of the skip countdown, played as paused */
/* The cut scene instance flags_8c; its low four bits are skip_frames. */
#define NUGCUTSCENE_PLAY_SKIPPABLE 0x10        /* skip ends it (or skips the stream); otherwise the quit prompt */
#define NUGCUTSCENE_PLAY_SFX_FIXUP 0x20        /* NuCutSceneSFXFixUp runs while the camera is inside the bounds */
#define NUGCUTSCENE_PLAY_HOLD_LAST_FRAME 0x40  /* at the end it holds the last frame (ENDED) instead of ending */
#define NUGCUTSCENE_PLAY_STREAM_LOADING 0x80   /* its next stream part is loading */
#define NUGCUTSCENE_PLAY_LOCK_PLAYER 0x200     /* sets CutSceneLockPlayer while it plays */
#define NUGCUTSCENE_PLAY_STREAM_SKIP 0x400     /* skipping a streamed scene: CutSceneStreamSkip runs */
#define NUGCUTSCENE_PLAY_CAMERA_CHANGED 0x1000 /* the camera state animation switched camera this update */

typedef struct instnugcutscene_s {      // size 0xf8
    struct instnugcutscene_s *next;     // 0x00
    struct instnugcutscene_s *previous; // 0x04
    char name[16];                      // 0x08
    NUMTX matrix;                       // 0x18
    CUT_SCENE_LOAD *scene;              // 0x58
    CUT_SCENE_LOAD *saved_scene;        // 0x5c
    NUVEC center;                       // 0x60
    NUVEC bounds_min;                   // 0x6c
    NUVEC bounds_max;                   // 0x78
    f32 radius_squared;                 // 0x84

    union { // size 0x4
        u32 flags_88;

        struct {                          // size 0x4
            u32 flags_88_bits_0_1 : 2;    // 0x88:0
            u32 paused : 1;               // 0x88:2
            u32 flags_88_bits_3_12 : 10;  // 0x88:3
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

    f32 current_frame;                                           // 0x90
    f32 render_frame;                                            // 0x94
    f32 rate;                                                    // 0x98
    CUT_LOAD_CAMERA_INSTANCE *camera_instance;                   // 0x9c
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *rigid_instance;              // 0xa0
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *character_instance;      // 0xa4
    struct cut_load_locator_instance_system_s *locator_instance; // 0xa8
    CUT_LOAD_TRIGGER_INSTANCE *trigger_instance;                 // 0xac
    struct instnugcutscene_s *chained_instance;                  // 0xb0
    void (*end_callback)(struct instnugcutscene_s *);            // 0xb4
    u32 unknown_b8;                                              // 0xb8

    union { // size 0x8

        struct {                   // size 0x8
            void *stream_buffer_0; // 0xbc
            void *stream_buffer_1; // 0xc0
        };

        void *stream_buffers[2];
    }; // 0xbc

    struct NUGSCN_s *scene_owner;                     // 0xc4
    CUT_LOAD_TRIGGER_OWNER *extra_scene_owner;        // 0xc8
    u8 unknown_cc[12];                                // 0xcc
    u16 skip_countdown;                               // 0xd8
    u8 unknown_da[5];                                 // 0xda
    u8 stream_index;                                  // 0xdf
    f32 accumulated_duration;                         // 0xe0
    f32 elapsed;                                      // 0xe4
    f32 alpha;                                        // 0xe8
    void *pending_stream;                             // 0xec
    u32 allocation_size;                              // 0xf0
    struct instnugcutscene_s *queued_stream_instance; // 0xf4
} INSTNUGCUTSCENE;

/* A cut scene the defragmenter tracks (instNuGCutSceneAddCleanUpItem). */
typedef struct cut_cleanup_entry_s { // size 0x10
    void *handle;                    // 0x0
    CUT_SCENE_LOAD *scene;           // 0x4
    f32 accumulated_duration;        // 0x8
    u8 flags;                        // 0xc
    u8 unknown_0d[3];                // 0xd
} CUT_CLEANUP_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*UpdateLocatorVfxFn)(i32, NUMTX *);
    extern CUT_CLEANUP_ENTRY *DefragCutSceneListBase;
    extern INSTNUGCUTSCENE *(*DefragGetInstFn)(void *);
    extern INSTNUGCUTSCENE *(*DefragCreateInstFn)(void *, CUT_SCENE_LOAD *, VARIPTR *);
    extern i32 (*TriggerLocatorVfxFn)(i32, NUMTX *);
    extern void (*NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    extern void (*NuCutSceneRigidPostRender)(void *, struct nuhspecial_s *, NUMTX *);
    void NuSetCutSceneResetCharactersFn(void (*)(INSTNUGCUTSCENE *));
    extern i32 (*LookupLocatorVfxFn)(char *);
    extern i32 (*NuCutSceneRequestSFX)(char *);
    void instNuGCutSceneSetMtx(INSTNUGCUTSCENE *inst, NUMTX *mtx);

    extern void (*NuCutSceneDestroyCharacters)(void *);
    extern void (*NuCutSceneCharacterDestroyData)(void *, void *);
    extern void (*NuCutSceneCharacterEval)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
    extern void (*NuCutSceneCharacterRelease)(void *, void *);
    extern void (*ReleaseLocatorVfxFn)(i32);
    void NuGCutSceneDestroy(void *);
    extern void (*NuCutSceneCharacterProcess)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
    extern void (*NuCutSceneSFXFixUp)(INSTNUGCUTSCENE *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct NUHGOBJ_EVAL *(*NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);
    extern void (*NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, struct cut_load_locator_instance_s *, CUT_LOAD_LOCATOR *, f32, NUMTX *,
                                       i32);
    void NuGCutSceneSysInit(CUT_LOCATOR_FUNCTION_ENTRY *);
    void NuSetCutSceneCharacterRenderFn(CUT_DRAW_RENDER_CALLBACK);
    void NuSetCutSceneCharacterEvalFn(CUT_DRAW_EVAL_CALLBACK);
    void NuSetCutSceneFindCharactersFn(void (*)(CUT_SCENE_LOAD *));
    void NuSetCutSceneCharacterCreateDataFn(void (*)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *));
    void NuSetCutSceneRigidPostRenderFn(CUT_DRAW_RIGID_CALLBACK);
    i32 FindMtlInHGObj(struct NUHGOBJ_EVAL *, i32);
    extern void (*NuCutSceneFindCharacters)(CUT_SCENE_LOAD *);
    extern void (*NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *);
    extern void (*NuCutSceneRigidCollisionCheck)(struct cut_load_rigid_s *, NUMTX *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuGCutLocatorIsVisble_3(struct cut_load_locator_s *, f32, f32 *, f32 *);
    i32 NuGCutLocatorIsVisble(struct cut_load_locator_s *, f32, struct NUANIM_TIME2 *, f32 *, f32 *);
    f32 *NuGCutLocatorCalcMtx(struct cut_load_locator_s *, f32, NUMTX *, struct NUANIM_TIME2 *);
    void NuGCutCharAnimProcess(struct cut_load_character_s *, f32, NUMTX *, i32 *, u32 *, f32 *, f32 *, f32 *, i32 *);
#ifdef __cplusplus
}
#endif
