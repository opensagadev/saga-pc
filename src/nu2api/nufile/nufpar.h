#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufile.h"

struct nufpcomjmp_s;
struct nufpcomjmpctx_s;

typedef struct nufparpos_s { // size 0x14
    i32 line_number;         // 0x00
    i32 line_position;       // 0x04
    i32 character_position;  // 0x08
    i32 buffer_start;        // 0x0c
    i32 buffer_end;          // 0x10
} NUFPARPOS;

#ifdef __cplusplus
extern "C" {
#endif
    void NuFParInit(i32 max_count, i32 buffer_size);
    extern const char LevelBeforeLoadName[16];
#ifdef __cplusplus
}
#endif

typedef struct nufpar_s {    // size 0x584
    char *file_buffer;       // 0x000
    NUFILE file_handle;      // 0x004
    char file_name[0x100];   // 0x008
    char line_buffer[0x202]; // 0x108
    char word_buffer[0x202]; // 0x30a
    char *line_buffer_ptr;   // 0x50c

    union { // size 0x4
        char *word_buffer_ptr;
        u16 *word_buffer_wide; // the word of a UTF-16 file
    }; // 0x510

    i32 line_buffer_size;   // 0x514
    i32 word_buffer_size;   // 0x518
    i32 line_number;        // 0x51c
    i32 line_position;      // 0x520
    i32 character_position; // 0x524
    i32 buffer_start;       // 0x528
    i32 buffer_end;         // 0x52c

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
} NUFPAR;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUFPAR NuFParRecords[4];
#ifdef __cplusplus
}
#endif

typedef struct nufpcomjmp_s {         // size 0x8
    char *keyword;                    // 0x0
    void (*callback)(NUFPAR *parser); // 0x4
} NUFPCOMJMP;

typedef struct nufpcomjmpctx_s {                     // size 0x8
    char *keyword;                                   // 0x0
    void (*callback)(NUFPAR *parser, void *context); // 0x4
} NUFPCOMJMPCTX;

#ifdef __cplusplus
extern "C" {
#endif
    NUFPAR *NuFParOpen(NUFILE file_handle);
    NUFPAR *NuFParCreate(char *file_name);
    NUFPAR *NuFParCreateMem(char *file_name, char *buffer, i32 size);
    NUFPAR *NuFParCreateGivenFH(char *file_name, NUFILE file_handle);
    void NuFParDestroy(NUFPAR *parser);
    i32 NuFParGetLine(NUFPAR *parser);
    i32 NuFParGetWord(NUFPAR *parser);
    void NuFParUnGetWord(NUFPAR *parser);
    i32 NuFParGetInt(NUFPAR *parser);
    f32 NuFParGetFloat(NUFPAR *parser);
    f32 NuFParGetFloatRDP(NUFPAR *parser);
    i32 NuFParGetIntRDP(NUFPAR *parser);
    void NuFParGetOptionalFloat(NUFPAR *parser, f32 *value);
    void NuFParGetOptionalInt(NUFPAR *parser, i32 *value);
    i32 NuFParPushCom2(NUFPAR *parser, NUFPCOMJMP *primary, NUFPCOMJMP *secondary);
    i32 NuFParPushCom(NUFPAR *parser, NUFPCOMJMP *commands);
    i32 NuFParPushComCTX(NUFPAR *parser, NUFPCOMJMPCTX *commands);
    i32 NuFParPushComCTX2(NUFPAR *parser, NUFPCOMJMPCTX *commands, NUFPCOMJMPCTX *commands2);
    void NuFParPopCom(NUFPAR *parser);
    i32 NuFParInterpretWord(NUFPAR *parser);
    i32 NuFParInterpretWordCTX(NUFPAR *parser, void *context);
    void NuFParGetPos(NUFPAR *parser, NUFPARPOS *position);
    void NuFParSetPos(NUFPAR *parser, NUFPARPOS *position);
    void NuFParSuspend(NUFPAR *parser);
    void NuFParResume(NUFPAR *parser);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef void (*NUFPCOMFN)(struct nufpar_s *parser);

    NUFPCOMFN NuFParSetInterpreterErrorHandler(NUFPCOMFN handler);
#ifdef __cplusplus
}
#endif
