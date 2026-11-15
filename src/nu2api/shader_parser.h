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

/* ShaderCompilerParser::ParseDeclaration: the token each state of the #pragma parser expects (nu2_use,
 * nu2_declare, nu2_anchorPoint, nu2_showDefine). */
#define SHADER_PARSE_KEYWORD 0
#define SHADER_PARSE_SHOW_DEFINE_VALUE 1
#define SHADER_PARSE_USE_OPEN 2
#define SHADER_PARSE_USE_STORAGE 3
#define SHADER_PARSE_USE_COMMA 4
#define SHADER_PARSE_USE_NAME 5 /* after any number of '*' */
#define SHADER_PARSE_USE_CLOSE 6
#define SHADER_PARSE_DECLARE_OPEN 7
#define SHADER_PARSE_DECLARE_STORAGE 8
#define SHADER_PARSE_DECLARE_SHARED_UNIFORM 9
#define SHADER_PARSE_DECLARE_REGISTER 10 /* vreg or freg of a uniform, or the comma */
#define SHADER_PARSE_DECLARE_REGISTER2 11
#define SHADER_PARSE_DECLARE_STORAGE_COMMA 12
#define SHADER_PARSE_DECLARE_TYPE 13
#define SHADER_PARSE_DECLARE_ARRAY 14 /* '[' or the comma */
#define SHADER_PARSE_DECLARE_ARRAY_COUNT 15
#define SHADER_PARSE_DECLARE_ARRAY_CLOSE 16
#define SHADER_PARSE_DECLARE_ARRAY_COMMA 17
#define SHADER_PARSE_DECLARE_NAME 18
#define SHADER_PARSE_DECLARE_NAME_COMMA 19
#define SHADER_PARSE_DECLARE_SEMANTIC 20
#define SHADER_PARSE_DECLARE_CLOSE 21
#define SHADER_PARSE_ANCHOR_OPEN 22
#define SHADER_PARSE_ANCHOR_STORAGE 23
#define SHADER_PARSE_ANCHOR_CLOSE 24
#define SHADER_PARSE_DONE 26

/* ShaderCompilerParser::ProcessDirective: the token each state of the directive parser expects. */
#define SHADER_DIRECTIVE_KEYWORD 0
#define SHADER_DIRECTIVE_DEFINE_NAME 1
#define SHADER_DIRECTIVE_DEFINE_VALUE 2
#define SHADER_DIRECTIVE_UNDEF_NAME 3
#define SHADER_DIRECTIVE_INCLUDE_PATH 4
#define SHADER_DIRECTIVE_DONE 5

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
