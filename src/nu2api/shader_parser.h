/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gamelib/util/CRC16.h"
#endif

#ifdef __cplusplus

struct ShaderParserNodePc {   // size 0x8
    i32 kind;                 // 0x0
    ShaderParserNodePc *next; // 0x4
    ShaderParserNodePc(i32 kind);
};

struct ShaderParserTextNodePc : ShaderParserNodePc { // size 0x10
    char *bytes;                                     // 0x8
    i32 count;                                       // 0xc
    ShaderParserTextNodePc(char *bytes, i32 count);
};

struct ShaderParserMacroNodePc : ShaderParserNodePc { // size 0x10
    char *name;                                       // 0x8
    char *value;                                      // 0xc
    ShaderParserMacroNodePc(char *name, char *value);
};

struct ShaderParserUndefNodePc : ShaderParserNodePc { // size 0xc
    char *name;                                       // 0x8
    ShaderParserUndefNodePc(char *name);
};

struct ShaderParserBranchNodePc : ShaderParserNodePc { // size 0x14
    ShaderParserNodePc *if_true;                       // 0x08
    ShaderParserNodePc *if_false;                      // 0x0c
    ShaderConditionNodePc *condition;                  // 0x10
    ShaderParserBranchNodePc(ShaderConditionNodePc *condition);
};
class ShaderCompilerParserPc;
#ifdef SAGA_SHADER_PARSER_TYPES
#include <vector>
struct ShaderParserIncludePc;

class ShaderCompilerParserPc {
  public:
    char include_directories[0x100];
    std::vector<ShaderParserIncludePc *> includes;
    u8 lexical_state;
    u8 reserved_111[3];
    ShaderParserNodePc *head;
    ShaderParserNodePc **tail;
    VirtualStackAllocator *arena;
    u8 emit_text;
    u8 reserved_121[3];
    ShaderCompilerBufferAllocatorPc text;
    ShaderParserBranchNodePc *branches[100];
    i32 branch_count;
    ShaderParserBranchNodePc *scopes[100];
    i32 scope_count;
    ShaderCompilerParserPc();
    ~ShaderCompilerParserPc();
    void SetIncludeDirectories(const char *);
    bool NextIncludeDirectory(const char *, i32 *, char *);
    void ClearIncludes();
    ShaderCompilerBufferAllocatorPc *LoadInclude(const char *);
    void ProcessDirective(ShaderCompilerBufferAllocatorPc *, i32 *);
    void LoadSourceLines(ShaderCompilerBufferAllocatorPc *, i32);
    ShaderParserNodePc *Parse(ShaderCompilerBufferAllocatorPc *, i32, u8, VirtualStackAllocator *);
    void FlushText();
    void DefineMacro(const char *name, const char *value);
    void UndefineMacro(const char *name);
    void BeginIf(ShaderConditionNodePc *condition);
    void BeginElse();
    void BeginElseIf(ShaderConditionNodePc *condition);
    void EndIf();
    void ParseDeclaration(ShaderCompilerBufferAllocatorPc *, i32 *);
    void SetShowDefinitions(u8);
    void UseDeclaration(u32, i32, const char *);
    void AddDeclaration(u32, i32, i32, const char *, i32, const char *, i32, i32);
    void EmitDeclarations(u32);
    ShaderConditionNodePc *ParseExpression(ShaderCompilerBufferAllocatorPc *input, i32 *cursor);
    void fun_0074b330(const ShaderLexicalSlicePc &slice);
    void fun_0074b6b0(ShaderParserNodePc *node);
};

STATIC_ASSERT(sizeof(ShaderCompilerParserPc) == 0x45c);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, include_directories) == 0);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, includes) == 0x100);
#endif
#ifdef SAGA_SHADER_PARSER_TYPES
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, lexical_state) == 0x110);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, head) == 0x114);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, tail) == 0x118);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, arena) == 0x11c);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, emit_text) == 0x120);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, text) == 0x124);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, branches) == 0x134);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, branch_count) == 0x2c4);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, scopes) == 0x2c8);
STATIC_ASSERT(offsetof(ShaderCompilerParserPc, scope_count) == 0x458);
#endif
#endif
