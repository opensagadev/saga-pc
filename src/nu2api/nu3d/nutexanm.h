#ifndef SAGA_NU2API_NU3D_NUTEXANM_H
#define SAGA_NU2API_NU3D_NUTEXANM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nurand.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern void *GHGTextureProgramsPC;
    void NuTexAnimAddList(void *list);
    extern const char NuTextureAnimationSlotsWarning[31];

    typedef struct PC_TEXTURE_ANIM_LIST {
        void *first;
        struct PC_TEXTURE_ANIM_LIST *next;
        struct PC_TEXTURE_ANIM_LIST *previous;
    } PC_TEXTURE_ANIM_LIST;

    STATIC_ASSERT(sizeof(PC_TEXTURE_ANIM_LIST) == 12);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_LIST, next) == 4);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_LIST, previous) == 8);

    typedef struct PC_TEXTURE_ANIM_ENV {
        void *program;
        i32 instruction_index;
        i32 loop_counts[16];
        i32 loop_starts[16];
        i32 loop_depth;
        i32 return_stack[16];
        i32 call_depth;
        i32 wait_base;
        i32 wait_random;
        i32 wait_remaining;
        NUMTL *material;
        u16 *texture_ids;
        i32 texture_index;
        u32 flags;
    } PC_TEXTURE_ANIM_ENV;

    STATIC_ASSERT(sizeof(PC_TEXTURE_ANIM_ENV) == 0xec);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, loop_depth) == 0x88);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, call_depth) == 0xcc);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, material) == 0xdc);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, texture_ids) == 0xe0);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, texture_index) == 0xe4);
    STATIC_ASSERT(offsetof(PC_TEXTURE_ANIM_ENV, flags) == 0xe8);
    extern PC_TEXTURE_ANIM_LIST *ntal_first;
    PC_TEXTURE_ANIM_ENV *NuTexAnimEnvCreate(VARIPTR *, NUMTL *, u16 *, void *);
    extern const char NuTextureAnimationSourcePathPC[50];

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
    TEXTURE_ANIMATION_PC *NuTexAnimCreate(VARIPTR *buffer, TEXTURE_ANIM_PROGRAM_PC *program, NUMTL *material,
                                          u16 *texture_ids, i32 texture_count);
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

#endif
