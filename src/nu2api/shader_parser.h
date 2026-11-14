#ifndef SAGA_NU2API_SHADER_PARSER_H
#define SAGA_NU2API_SHADER_PARSER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gamelib/util/CRC16.h, gamelib/util/gamelib_util_types.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#endif

#ifdef __cplusplus

struct ShaderParserNodePc {
    i32 kind;
    ShaderParserNodePc *next;
    ShaderParserNodePc(i32 kind);
};

struct ShaderParserTextNodePc : ShaderParserNodePc {
    char *bytes;
    i32 count;
    ShaderParserTextNodePc(char *bytes, i32 count);
};

struct ShaderParserMacroNodePc : ShaderParserNodePc {
    char *name;
    char *value;
    ShaderParserMacroNodePc(char *name, char *value);
};

struct ShaderParserUndefNodePc : ShaderParserNodePc {
    char *name;
    ShaderParserUndefNodePc(char *name);
};

struct ShaderParserBranchNodePc : ShaderParserNodePc {
    ShaderParserNodePc *if_true;
    ShaderParserNodePc *if_false;
    ShaderConditionNodePc *condition;
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
STATIC_ASSERT(sizeof(ShaderParserNodePc) == 8);
STATIC_ASSERT(offsetof(ShaderParserNodePc, kind) == 0);
STATIC_ASSERT(offsetof(ShaderParserNodePc, next) == 4);
STATIC_ASSERT(sizeof(ShaderParserTextNodePc) == 16);
STATIC_ASSERT(offsetof(ShaderParserTextNodePc, bytes) == 8);
STATIC_ASSERT(offsetof(ShaderParserTextNodePc, count) == 12);
STATIC_ASSERT(sizeof(ShaderParserMacroNodePc) == 16);
STATIC_ASSERT(offsetof(ShaderParserMacroNodePc, name) == 8);
STATIC_ASSERT(offsetof(ShaderParserMacroNodePc, value) == 12);
STATIC_ASSERT(sizeof(ShaderParserUndefNodePc) == 12);
STATIC_ASSERT(offsetof(ShaderParserUndefNodePc, name) == 8);
STATIC_ASSERT(sizeof(ShaderParserBranchNodePc) == 20);
STATIC_ASSERT(offsetof(ShaderParserBranchNodePc, if_true) == 8);
STATIC_ASSERT(offsetof(ShaderParserBranchNodePc, if_false) == 12);
STATIC_ASSERT(offsetof(ShaderParserBranchNodePc, condition) == 16);
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

#endif
