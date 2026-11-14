#ifndef SAGA_NU2API_NU3D_NUTEXANM_H
#define SAGA_NU2API_NU3D_NUTEXANM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
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

#ifndef TYPES_OFF_37
struct TEXTURE_ANIM_PROGRAM_PC {
    TEXTURE_ANIM_PROGRAM_PC *next;     // 0x000
    TEXTURE_ANIM_PROGRAM_PC *previous; // 0x004
    char name[32];                     // 0x008
    i32 on_signal[32];                 // 0x028
    i32 off_signal[32];                // 0x0a8
    u32 on_mask;                       // 0x128
    u32 off_mask;                      // 0x12c
    i16 label_ids[32];                 // 0x130
    i16 label_offsets[32];             // 0x170
    i32 label_count;                   // 0x1b0
    i16 instruction_count;             // 0x1b4
    u16 flags;                         // 0x1b6
    u16 mask;                          // 0x1b8
    i16 instructions[1];               // 0x1ba
}; // size 0x1bc

struct TEXTURE_ANIMATION_PC {
    TEXTURE_ANIMATION_PC *next;       // 0x00
    TEXTURE_ANIMATION_PC *previous;   // 0x04
    u16 *texture_ids;                 // 0x08
    i16 texture_count;                // 0x0c
    u8 flags;                         // 0x0e
    u8 reserved_0f;                   // 0x0f
    NUMTL *material;                  // 0x10
    PC_TEXTURE_ANIM_ENV *environment; // 0x14
    char *name;                       // 0x18
    char *program_name;               // 0x1c
}; // size 0x20

#endif /* TYPES_OFF_37 */
#endif
