#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nurand.h"

struct numtl_s;

#ifdef __cplusplus
extern "C" {
#endif

    extern void *GHGTextureProgramsPC;
    void NuTexAnimAddList(void *list);
    extern const char NuTextureAnimationSlotsWarning[31];

    typedef struct PC_TEXTURE_ANIM_LIST {      // size 0xc
        void *first;                           // 0x0
        struct PC_TEXTURE_ANIM_LIST *next;     // 0x4
        struct PC_TEXTURE_ANIM_LIST *previous; // 0x8
    } PC_TEXTURE_ANIM_LIST;

    typedef struct PC_TEXTURE_ANIM_ENV { // size 0xec
        void *program;                   // 0x00
        i32 instruction_index;           // 0x04
        i32 loop_counts[16];             // 0x08
        i32 loop_starts[16];             // 0x48
        i32 loop_depth;                  // 0x88
        i32 return_stack[16];            // 0x8c
        i32 call_depth;                  // 0xcc
        i32 wait_base;                   // 0xd0
        i32 wait_random;                 // 0xd4
        i32 wait_remaining;              // 0xd8
        struct numtl_s *material;        // 0xdc
        u16 *texture_ids;                // 0xe0
        i32 texture_index;               // 0xe4
        u32 flags;                       // 0xe8
    } PC_TEXTURE_ANIM_ENV;

    extern PC_TEXTURE_ANIM_LIST *ntal_first;
    PC_TEXTURE_ANIM_ENV *NuTexAnimEnvCreate(VARIPTR *, struct numtl_s *, u16 *, void *);

    typedef struct TEXTURE_ANIM_PROGRAM TEXTURE_ANIM_PROGRAM;
    typedef struct TEXTURE_ANIMATION TEXTURE_ANIMATION;

    struct TEXTURE_ANIM_PROGRAM {       // size 0x1bc
        TEXTURE_ANIM_PROGRAM *next;     // 0x000
        TEXTURE_ANIM_PROGRAM *previous; // 0x004
        char name[32];                  // 0x008
        i32 on_signal[32];              // 0x028
        i32 off_signal[32];             // 0x0a8
        u32 on_mask;                    // 0x128
        u32 off_mask;                   // 0x12c
        i16 label_ids[32];              // 0x130
        i16 label_offsets[32];          // 0x170
        i32 label_count;                // 0x1b0
        i16 instruction_count;          // 0x1b4
        i16 flags;                      // 0x1b6
        u16 mask;                       // 0x1b8
        i16 instructions[1];            // 0x1ba
    };

    struct TEXTURE_ANIMATION {            // size 0x20
        TEXTURE_ANIMATION *next;          // 0x00
        TEXTURE_ANIMATION *previous;      // 0x04
        u16 *texture_ids;                 // 0x08
        i16 texture_count;                // 0x0c
        u8 flags;                         // 0x0e
        u8 reserved_0f;                   // 0x0f
        struct numtl_s *material;         // 0x10
        PC_TEXTURE_ANIM_ENV *environment; // 0x14
        char *name;                       // 0x18
        char *program_name;               // 0x1c
    };

    void NuTexAnimSetMask(i32 mask);
    void NuTexAnimEnvProc(PC_TEXTURE_ANIM_ENV *environment);
    void NuTexAnimEnvReset(PC_TEXTURE_ANIM_ENV *environment);
    void NuTexAnimProcessList(TEXTURE_ANIMATION *animation);
    void NuTexAnimInitMutex(void);
    void NuTexAnimRestart(void);
    TEXTURE_ANIMATION *NuTexAnimFind(TEXTURE_ANIMATION *animation, const char *name);
    void NuTexAnimDestroy(TEXTURE_ANIMATION *animation);
    TEXTURE_ANIMATION *NuTexAnimCreate(VARIPTR *buffer, TEXTURE_ANIM_PROGRAM *program, struct numtl_s *material, u16 *texture_ids,
                                       i32 texture_count);
    TEXTURE_ANIM_PROGRAM *NuTexAnimProgFind(const char *name);
    void NuTexAnimProgDestroy(TEXTURE_ANIM_PROGRAM *program);
    void NuTexAnimProgRelease(void);
    void NuTexAnimProgInit(TEXTURE_ANIM_PROGRAM *program);
    TEXTURE_ANIM_PROGRAM *NuTexAnimProgCreate(VARIPTR *buffer, i32 extra_bytes, const char *name);
    void NuTexAnimProgWrite(char *path, TEXTURE_ANIM_PROGRAM *program);
    TEXTURE_ANIM_PROGRAM *NuTexAnimProgRead(VARIPTR *buffer, char *path);
    void NuTexAnimProgAssembleEnd(TEXTURE_ANIM_PROGRAM *program);
    TEXTURE_ANIM_PROGRAM *NuTexAnimProgParseFile(NUFILE file, VARIPTR *buffer, VARIPTR end, i32 fps_unused);
    TEXTURE_ANIM_PROGRAM *NuTexAnimProgReadScript(char *path, VARIPTR *buffer, VARIPTR end, i32 fps_unused);

    extern NURAND texanim_rand;
    extern u16 script_mask;
    extern const char NuTextureEnvironmentCubePath[];
#ifdef __cplusplus
}
#endif

/* TEXTURE_ANIM_PROGRAM.instructions: the opcodes, each written by the handler of its script keyword
 * (TextureAnimationCommandsPrivate). */
#define TEXANIM_OP_TEX 0
#define TEXANIM_OP_TEXR 1
#define TEXANIM_OP_TEXADJ 2
#define TEXANIM_OP_TEXADJR 3
#define TEXANIM_OP_WAIT 4
#define TEXANIM_OP_RATE 7
#define TEXANIM_OP_GOTO 9
#define TEXANIM_OP_GOSUB 10
#define TEXANIM_OP_BTEX 11
#define TEXANIM_OP_RET 12
#define TEXANIM_OP_REPEAT 13
#define TEXANIM_OP_REPEND 14
#define TEXANIM_OP_UNTILTEX 15
#define TEXANIM_OP_END 16
#define TEXANIM_OP_XREF 17

/* The comparisons of btex and untiltex (TextureAnimationReadComparisonPrivate). */
#define TEXANIM_CMP_EQ 0
#define TEXANIM_CMP_LT 1
#define TEXANIM_CMP_GT 2
#define TEXANIM_CMP_LE 3
#define TEXANIM_CMP_GE 4
#define TEXANIM_CMP_NE 5
