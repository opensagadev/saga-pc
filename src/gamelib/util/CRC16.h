#pragma once

#include "nu2api/nucore/common.h"

#include "gamelib/util/gamelib_util_types.h"

#ifdef __cplusplus
class CRC16 {
  public:
    CRC16();
    static u32 hash(const unsigned char *, i32);
    static u32 hashInverse(const unsigned char *, i32);
};
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 append;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
struct ShaderLexicalSlice;

#endif
/* ShaderSourceNode.semantic: the semantic_* keywords of a declaration (ShaderDeclarationSemantic) and the
 * names ShaderSourceNode::SemanticName prints; a custom semantic keeps its own text. */
#define SHADER_SEMANTIC_CUSTOM 0
#define SHADER_SEMANTIC_DEFAULT 1
#define SHADER_SEMANTIC_POSITION 2
#define SHADER_SEMANTIC_POSITION1 3
#define SHADER_SEMANTIC_VPOS 4
#define SHADER_SEMANTIC_NORMAL 5
#define SHADER_SEMANTIC_TANGENT 6
#define SHADER_SEMANTIC_BINORMAL 7
#define SHADER_SEMANTIC_TANGENT1 8
#define SHADER_SEMANTIC_COLORX 9
#define SHADER_SEMANTIC_COLOR0 10
#define SHADER_SEMANTIC_COLOR1 11
#define SHADER_SEMANTIC_FOG 12
#define SHADER_SEMANTIC_TEXCOORDX 13
#define SHADER_SEMANTIC_TEXCOORD0 14
#define SHADER_SEMANTIC_TEXCOORD1 15
#define SHADER_SEMANTIC_TEXCOORD2 16
#define SHADER_SEMANTIC_TEXCOORD3 17
#define SHADER_SEMANTIC_TEXCOORD4 18
#define SHADER_SEMANTIC_TEXCOORD5 19
#define SHADER_SEMANTIC_TEXCOORD6 20
#define SHADER_SEMANTIC_TEXCOORD7 21
#define SHADER_SEMANTIC_BLENDINDICES0 22
#define SHADER_SEMANTIC_BLENDWEIGHT0 23
#define SHADER_SEMANTIC_INDEX 24

#ifdef __cplusplus
struct ShaderConditionNode {    // size 0xc
    ShaderConditionNode *left;  // 0x0
    ShaderConditionNode *right; // 0x4
    const char *token;          // 0x8
    ShaderConditionNode(const char *token, ShaderConditionNode *left, ShaderConditionNode *right);
    ShaderConditionNode *AppendOperand(ShaderConditionNode *operand);
};

struct ShaderSourceNode {        // size 0x24
    u32 flags;                   // 0x00
    i32 type;                    // 0x04
    i32 array_count;             // 0x08
    const char *name;            // 0x0c
    i32 semantic;                // 0x10
    const char *custom_semantic; // 0x14
    i32 vertex_register;         // 0x18
    i32 pixel_register;          // 0x1c
    i32 expansion_state;         // 0x20
    ShaderSourceNode(u32, i32, i32, const char *, i32, const char *, i32, i32);
    const char *SemanticName();
    i32 fun_00746620();
    const char *fun_00746630();
};

struct ShaderRegisterBlock { // size 0x14
    char name[16];           // 0x00
    i32 register_count;      // 0x10

    ShaderRegisterBlock() {
        name[0] = 0;
        register_count = 0;
    }

    void fun_00746760(const char *block_name, i32 count);
};

struct ShaderMacroDefinition { // size 0xc
    const char *name;          // 0x0
    const char *value;         // 0x4
    u8 emit;                   // 0x8
    u8 unknown_9[3];           // 0x9
    ShaderMacroDefinition(const char *name, const char *value, u8 emit);
};

extern "C" void ShaderLexicalClassesInit(void);

/* ShaderLexicalClasses: the character classes of the shader preprocessor's lexer. */
#define SHADER_CHAR_IDENTIFIER 0x1 /* letters and '_' */
#define SHADER_CHAR_DIGIT 0x2
#define SHADER_CHAR_OPERATOR 0x4 /* | & ( > < = ! - + and 0x80 */
#define SHADER_CHAR_BLANK 0x8    /* space and tab */
#define SHADER_CHAR_NEWLINE 0x10 /* '\n' and '\r' */

/* ShaderParserNode.kind and ShaderExpansionNode.kind: the node type each ShaderParser*Pc constructor stores. */
#define SHADER_NODE_BRANCH 0
#define SHADER_NODE_MACRO 1
#define SHADER_NODE_UNDEF 2
#define SHADER_NODE_DECLARATION 3
#define SHADER_NODE_REFERENCE 4
#define SHADER_NODE_TOGGLE 5
#define SHADER_NODE_EMIT 6
#define SHADER_NODE_TEXT 7

/* Shader declaration flags: the storage keyword of nu2_use/nu2_declare (ShaderCompilerParser::ParseDeclaration),
 * the stages a uniform is bound to (vreg, freg) and shared (ShaderDeclarationFlags). */
#define SHADER_DECL_VS_LOCAL 0x2
#define SHADER_DECL_FS_LOCAL 0x4
#define SHADER_DECL_VARYING 0x8
#define SHADER_DECL_ATTRIBUTE 0x10
#define SHADER_DECL_UNIFORM 0x20
#define SHADER_DECL_PIXEL 0x1000
#define SHADER_DECL_VERTEX 0x2000
#define SHADER_DECL_SHARED 0x4000

/* ShaderParserDeclaration.type: the type keyword of an nu2_declare (ShaderCompilerParser::ParseDeclaration); a
 * bare 1-4 is float to float4. */
#define SHADER_TYPE_FLOAT 1
#define SHADER_TYPE_FLOAT1 2
#define SHADER_TYPE_FLOAT2 3
#define SHADER_TYPE_FLOAT3 4
#define SHADER_TYPE_FLOAT4 5
#define SHADER_TYPE_FLOAT3X3 6
#define SHADER_TYPE_FLOAT4X4 7
#define SHADER_TYPE_HALF 8
#define SHADER_TYPE_HALF1 9
#define SHADER_TYPE_HALF2 10
#define SHADER_TYPE_HALF3 11
#define SHADER_TYPE_HALF4 12
#define SHADER_TYPE_HALF3X3 13
#define SHADER_TYPE_HALF4X4 14
#define SHADER_TYPE_BOOL 15
#define SHADER_TYPE_INT 16
#define SHADER_TYPE_SAMPLER1D 17
#define SHADER_TYPE_SAMPLER2D 18 /* also sampler */
#define SHADER_TYPE_SAMPLER3D 19
#define SHADER_TYPE_SAMPLERCUBE 20
#define SHADER_TYPE_HOOK 21

struct ShaderExpansionNode {   // size 0x28
    i32 kind;                  // 0x00
    ShaderExpansionNode *next; // 0x04

    union { // size 0x20

        struct {                            // size 0xc
            ShaderExpansionNode *if_true;   // 0x08
            ShaderExpansionNode *if_false;  // 0x0c
            ShaderConditionNode *condition; // 0x10
        } branch;

        struct {               // size 0x8
            const char *name;  // 0x08
            const char *value; // 0x0c
        } definition;

        struct {                         // size 0x20
            u32 flags;                   // 0x08
            i32 type;                    // 0x0c
            i32 array_count;             // 0x10
            const char *name;            // 0x14
            i32 semantic;                // 0x18
            const char *custom_semantic; // 0x1c
            i32 vertex_register;         // 0x20
            i32 pixel_register;          // 0x24
        } declaration;

        struct {              // size 0xc
            u32 flags;        // 0x08
            i32 alias_depth;  // 0x0c
            const char *name; // 0x10
        } reference;

        struct {        // size 0x1
            u8 enabled; // 0x08
        } toggle;

        struct {       // size 0x4
            u32 flags; // 0x08
        } emit;

        struct {               // size 0x8
            const char *bytes; // 0x08
            i32 count;         // 0x0c
        } text;
    } data; // 0x08
};

#endif
#ifdef __cplusplus

extern "C" u32 ShaderDeclarationFlags(u8, u8, u8);
#endif

#ifdef __cplusplus

struct ShaderCompilerBufferAllocator { // size 0x10
    u8 *data;                          // 0x0
    i32 length;                        // 0x4
    i32 capacity;                      // 0x8
    u8 owns_memory;                    // 0xc
    ShaderCompilerBufferAllocator(i32, bool);
    ShaderCompilerBufferAllocator(VirtualStackAllocator &, i32);
    ~ShaderCompilerBufferAllocator();
    char *CopyText();
    void Resize(i32);
    void AppendBytes(const char *, i32);
    i32 Find(const char *text, i32 start);
    bool MatchesAt(i32 start, const char *text);
    void OverwriteText(i32 start, const char *text);
    void WriteBytes(i32 offset, const void *source, i32 count);
    void AppendText(const char *);
    ShaderCompilerBufferAllocator &operator<<(const char *);
    ShaderCompilerBufferAllocator &operator<<(i32);
    void AppendInt(i32);
    void AppendSlice(const ShaderLexicalSlice &slice);
    ShaderCompilerBufferAllocator &operator<<(const ShaderLexicalSlice &slice);
    void fun_00746450(char value);
    u8 *fun_00746490();
};

#endif
#ifdef __cplusplus

struct ShaderCompilerDictionary {                  // size 0x9a4
    ShaderMacroDefinition *programs[300];          // 0x000
    i32 program_count;                             // 0x4b0
    ShaderSourceNode *definitions[250];            // 0x4b4
    i32 definition_count;                          // 0x89c
    ShaderRegisterBlock register_blocks[8];        // 0x8a0
    i32 register_block_count;                      // 0x940
    VirtualStackAllocator scratch;                 // 0x944
    VirtualStackAllocator output_memory;           // 0x954
    u8 emit_definitions;                           // 0x964
    u8 output_suppressed;                          // 0x965
    u8 emit_comments;                              // 0x966
    u8 emit_semantics;                             // 0x967
    u8 emit_registers;                             // 0x968
    u8 emit_centroid;                              // 0x969
    u8 unknown_96a[2];                             // 0x96a
    const char *register_format;                   // 0x96c
    bool force_mode;                               // 0x970
    u8 unknown_971[3];                             // 0x971
    i32 profile;                                   // 0x974
    i32 mode;                                      // 0x978
    i32 output_stage;                              // 0x97c
    i32 first_version;                             // 0x980
    i32 second_version;                            // 0x984
    const char *profile_name;                      // 0x988
    ShaderCompilerBufferAllocator register_buffer; // 0x98c

    ShaderCompilerBufferAllocator *active_output; // 0x99c

    u32 unknown_9a0; // 0x9a0
    ShaderCompilerDictionary(VirtualStackAllocator &, i32);
    ~ShaderCompilerDictionary();
    void SetProfile(i32, i32, i32);
    bool GetForceMode() const;
    i32 GetProfile() const;
    void AddRegisterBlock(const char *, i32);
    i32 FindRegisterBlock(const char *);
    ShaderMacroDefinition *FindDefinition(const char *);
    void Expand(void *, ShaderCompilerBufferAllocator *);
    void AddDefinition(const char *, const char *, i32);
    void BindOutputBuffer(ShaderCompilerBufferAllocator *);
    bool IsNumericToken(const char *);
    bool EvaluateCondition(ShaderConditionNode *, i32 *);
    i32 EvaluateValue(ShaderConditionNode *);
    const char *RenderASTName(ShaderSourceNode *);
    ShaderSourceNode *FindASTExact(const char *, u32);
    ShaderSourceNode *FindASTUniform(const char *);
    void fun_00748810(i32 stage);
    void fun_00748820();
    void fun_00748840();
    void EmitType(ShaderCompilerBufferAllocator *);
    void EmitRegister(ShaderCompilerBufferAllocator *);
    void EmitVertexInput(ShaderCompilerBufferAllocator *);
    void EmitAttribute(ShaderCompilerBufferAllocator *);
    void EmitProgram(ShaderCompilerBufferAllocator *);
    void AddASTDefinition(ShaderSourceNode *);
    void fun_007451c0(bool force);
    void fun_00746940(ShaderConditionNode *node, char *text);
    void fun_00746bf0(const char *name);
};

#endif

#ifdef __cplusplus

struct ShaderLexicalSlice { // size 0x8
    const char *begin;      // 0x0
    i32 length;             // 0x4
    bool Equals(const char *text);
    bool StartsWith(const char *text);
    char *HeapCopy();
    char *ArenaCopy(VirtualStackAllocator *arena);
    i32 AsInteger(i32 skipped_prefix);
    const char *GetBegin();
    const char *At(i32 offset);
    bool fun_0074b080(const char *text);
};

struct ShaderLexicalReader {
    bool Next(ShaderCompilerBufferAllocator *input, i32 *cursor, ShaderLexicalSlice *slice);
    i32 IsWordChar(u8 c);
    i32 IsTokenChar(u8 c);
};

#endif
#ifdef __cplusplus
extern "C" i32 ShaderDeclarationSemantic(ShaderLexicalSlice *);
#endif
