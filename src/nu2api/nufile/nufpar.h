/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NUFILE_NUFPAR_H_PART
#define SAGA_NU2API_NUFILE_NUFPAR_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUFILE_NUFPAR_H_PART == 1 && !defined(SAGA_NU2API_NUFILE_NUFPAR_H_STOP)

struct nufpcomjmp_s;
struct nufpcomjmpctx_s;

typedef struct nufparpos_s { // size 0x14
    i32 line_number;         // 0x00
    i32 line_position;       // 0x04
    i32 character_position;  // 0x08
    i32 buffer_start;        // 0x0c
    i32 buffer_end;          // 0x10
} NUFPARPOS_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void NuFParInit(i32 max_count, i32 buffer_size);
    extern const char LevelBeforeLoadName[16];
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUFILE_NUFPAR_H_PART
#define SAGA_NU2API_NUFILE_NUFPAR_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_NUFPAR_H_STOP
#endif
#endif

#if SAGA_NU2API_NUFILE_NUFPAR_H_PART == 2 && !defined(SAGA_NU2API_NUFILE_NUFPAR_H_STOP)

typedef struct nufpar_s {    // size 0x584
    char *file_buffer;       // 0x000
    NUFILE file_handle;      // 0x004
    char file_name[0x100];   // 0x008
    char line_buffer[0x202]; // 0x108
    char word_buffer[0x202]; // 0x30a
    char *line_buffer_ptr;   // 0x50c
    char *word_buffer_ptr;   // 0x510
    i32 line_buffer_size;    // 0x514
    i32 word_buffer_size;    // 0x518
    i32 line_number;         // 0x51c
    i32 line_position;       // 0x520
    i32 character_position;  // 0x524
    i32 buffer_start;        // 0x528
    i32 buffer_end;          // 0x52c

    union { // size 0x20
        struct nufpcomjmp_s *command_stack[8];
        struct nufpcomjmpctx_s *command_stack_ctx[8];
    }; // 0x530

    union { // size 0x20
        struct nufpcomjmp_s *command_stack2[8];
        struct nufpcomjmpctx_s *command_stack2_ctx[8];
    }; // 0x550

    i32 command_position;   // 0x570
    i32 file_size;          // 0x574
    u8 is_utf16;            // 0x578
    u8 is_utf8;             // 0x579
    u8 unknown_057a[2];     // 0x57a
    char *separator_list;   // 0x57c
    char *separator_tokens; // 0x580
} NUFPAR_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUFPAR_PC NuFParRecords[4];
#ifdef __cplusplus
}
#endif

typedef struct nufpcomjmp_s {            // size 0x8
    char *keyword;                       // 0x0
    void (*callback)(NUFPAR_PC *parser); // 0x4
} NUFPCOMJMP;

typedef struct nufpcomjmpctx_s {                        // size 0x8
    char *keyword;                                      // 0x0
    void (*callback)(NUFPAR_PC *parser, void *context); // 0x4
} NUFPCOMJMPCTX;

#ifdef __cplusplus
extern "C" {
#endif
    NUFPAR_PC *NuFParOpen(NUFILE file_handle);
    NUFPAR_PC *NuFParCreate(char *file_name);
    NUFPAR_PC *NuFParCreateMem(char *file_name, char *buffer, i32 size);
    NUFPAR_PC *NuFParCreateGivenFH(char *file_name, NUFILE file_handle);
    void NuFParDestroy(NUFPAR_PC *parser);
    i32 NuFParGetLine(NUFPAR_PC *parser);
    i32 NuFParGetWord(NUFPAR_PC *parser);
    void NuFParUnGetWord(NUFPAR_PC *parser);
    i32 NuFParGetInt(NUFPAR_PC *parser);
    f32 NuFParGetFloat(NUFPAR_PC *parser);
    f32 NuFParGetFloatRDP(NUFPAR_PC *parser);
    i32 NuFParGetIntRDP(NUFPAR_PC *parser);
    void NuFParGetOptionalFloat(NUFPAR_PC *parser, f32 *value);
    void NuFParGetOptionalInt(NUFPAR_PC *parser, i32 *value);
    i32 NuFParPushCom2(NUFPAR_PC *parser, NUFPCOMJMP *primary, NUFPCOMJMP *secondary);
    i32 NuFParPushCom(NUFPAR_PC *parser, NUFPCOMJMP *commands);
    i32 NuFParPushComCTX(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands);
    i32 NuFParPushComCTX2(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands, NUFPCOMJMPCTX *commands2);
    void NuFParPopCom(NUFPAR_PC *parser);
    i32 NuFParInterpretWord(NUFPAR_PC *parser);
    i32 NuFParInterpretWordCTX(NUFPAR_PC *parser, void *context);
    void NuFParGetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position);
    void NuFParSetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position);
    void NuFParSuspend(NUFPAR_PC *parser);
    void NuFParResume(NUFPAR_PC *parser);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUFILE_NUFPAR_H_PART
#define SAGA_NU2API_NUFILE_NUFPAR_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_NUFPAR_H_STOP
#endif
#endif

#if SAGA_NU2API_NUFILE_NUFPAR_H_PART == 3 && !defined(SAGA_NU2API_NUFILE_NUFPAR_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    typedef void (*NUFPCOMFN)(struct nufpar_s *parser);

    NUFPCOMFN NuFParSetInterpreterErrorHandler(NUFPCOMFN handler);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUFILE_NUFPAR_H_PART
#define SAGA_NU2API_NUFILE_NUFPAR_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_NUFPAR_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUFILE_NUFPAR_H_STOP
