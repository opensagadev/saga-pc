/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nurand.h"
#endif

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

    typedef struct TEXTURE_ANIM_PROGRAM_PC TEXTURE_ANIM_PROGRAM_PC;
    typedef struct TEXTURE_ANIMATION_PC TEXTURE_ANIMATION_PC;
    void NuTexAnimSetMask(i32 mask);
    void NuTexAnimEnvProc(PC_TEXTURE_ANIM_ENV *environment);
    void NuTexAnimEnvReset(PC_TEXTURE_ANIM_ENV *environment);
    void NuTexAnimProcessList(TEXTURE_ANIMATION_PC *animation);
    void NuTexAnimInitMutex(void);
    void NuTexAnimRestart(void);
    TEXTURE_ANIMATION_PC *NuTexAnimFind(TEXTURE_ANIMATION_PC *animation, const char *name);
    void NuTexAnimDestroy(TEXTURE_ANIMATION_PC *animation);
    TEXTURE_ANIMATION_PC *NuTexAnimCreate(VARIPTR *buffer, TEXTURE_ANIM_PROGRAM_PC *program, struct numtl_s *material, u16 *texture_ids,
                                          i32 texture_count);
    TEXTURE_ANIM_PROGRAM_PC *NuTexAnimProgFind(const char *name);
    void NuTexAnimProgDestroy(TEXTURE_ANIM_PROGRAM_PC *program);
    void NuTexAnimProgRelease(void);
    void NuTexAnimProgInit(TEXTURE_ANIM_PROGRAM_PC *program);
    TEXTURE_ANIM_PROGRAM_PC *NuTexAnimProgCreate(VARIPTR *buffer, i32 extra_bytes, const char *name);
    void NuTexAnimProgWrite(char *path, TEXTURE_ANIM_PROGRAM_PC *program);
    TEXTURE_ANIM_PROGRAM_PC *NuTexAnimProgRead(VARIPTR *buffer, char *path);
    void NuTexAnimProgAssembleEnd(TEXTURE_ANIM_PROGRAM_PC *program);
    TEXTURE_ANIM_PROGRAM_PC *NuTexAnimProgParseFile(NUFILE file, VARIPTR *buffer, VARIPTR end, i32 fps_unused);
    TEXTURE_ANIM_PROGRAM_PC *NuTexAnimProgReadScript(char *path, VARIPTR *buffer, VARIPTR end, i32 fps_unused);

    extern NURAND texanim_rand;
    extern u16 script_mask;
    extern const char NuTextureEnvironmentCubePathPc[];
#ifdef __cplusplus
}
#endif
