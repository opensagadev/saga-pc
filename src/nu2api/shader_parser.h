#pragma once

#include "nu2api/nucore/common.h"

struct ShaderConditionNode;

#ifdef __cplusplus

struct ShaderParserNode {   // size 0x8
    i32 kind;               // 0x0
    ShaderParserNode *next; // 0x4
    ShaderParserNode(i32 kind);
};

struct ShaderParserTextNode : ShaderParserNode { // size 0x10
    char *bytes;                                 // 0x8
    i32 count;                                   // 0xc
    ShaderParserTextNode(char *bytes, i32 count);
};

struct ShaderParserMacroNode : ShaderParserNode { // size 0x10
    char *name;                                   // 0x8
    char *value;                                  // 0xc
    ShaderParserMacroNode(char *name, char *value);
};

struct ShaderParserUndefNode : ShaderParserNode { // size 0xc
    char *name;                                   // 0x8
    ShaderParserUndefNode(char *name);
};

struct ShaderParserBranchNode : ShaderParserNode { // size 0x14
    ShaderParserNode *if_true;                     // 0x08
    ShaderParserNode *if_false;                    // 0x0c
    struct ShaderConditionNode *condition;         // 0x10
    ShaderParserBranchNode(struct ShaderConditionNode *condition);
};
class ShaderCompilerParser;
#ifdef SAGA_SHADER_PARSER_TYPES
#include <vector>
struct ShaderParserInclude;

class ShaderCompilerParser { // size 0x45c
  public:
    char include_directories[0x100];             // 0x000
    std::vector<ShaderParserInclude *> includes; // 0x100
    u8 lexical_state;                            // 0x110
    u8 reserved_111[3];                          // 0x111
    ShaderParserNode *head;                      // 0x114
    ShaderParserNode **tail;                     // 0x118
    VirtualStackAllocator *arena;                // 0x11c
    u8 emit_text;                                // 0x120
    u8 reserved_121[3];                          // 0x121
    ShaderCompilerBufferAllocator text;          // 0x124
    ShaderParserBranchNode *branches[100];       // 0x134
    i32 branch_count;                            // 0x2c4
    ShaderParserBranchNode *scopes[100];         // 0x2c8
    i32 scope_count;                             // 0x458
    ShaderCompilerParser();
    ~ShaderCompilerParser();
    void SetIncludeDirectories(const char *);
    bool NextIncludeDirectory(const char *, i32 *, char *);
    void ClearIncludes();
    ShaderCompilerBufferAllocator *LoadInclude(const char *);
    void ProcessDirective(ShaderCompilerBufferAllocator *, i32 *);
    void LoadSourceLines(ShaderCompilerBufferAllocator *, i32);
    ShaderParserNode *Parse(ShaderCompilerBufferAllocator *, i32, u8, VirtualStackAllocator *);
    void FlushText();
    void DefineMacro(const char *name, const char *value);
    void UndefineMacro(const char *name);
    void BeginIf(ShaderConditionNode *condition);
    void BeginElse();
    void BeginElseIf(ShaderConditionNode *condition);
    void EndIf();
    void ParseDeclaration(ShaderCompilerBufferAllocator *, i32 *);
    void SetShowDefinitions(u8);
    void UseDeclaration(u32, i32, const char *);
    void AddDeclaration(u32, i32, i32, const char *, i32, const char *, i32, i32);
    void EmitDeclarations(u32);
    ShaderConditionNode *ParseExpression(ShaderCompilerBufferAllocator *input, i32 *cursor);
    void fun_0074b330(const ShaderLexicalSlice &slice);
    void fun_0074b6b0(ShaderParserNode *node);
};

#endif
#endif
