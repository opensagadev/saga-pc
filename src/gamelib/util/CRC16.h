#ifndef SAGA_GAMELIB_UTIL_CRC16_H_PART
#define SAGA_GAMELIB_UTIL_CRC16_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: gamelib/util/gamelib_util_types.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gamelib/util/gamelib_util_types.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMELIB_UTIL_CRC16_H_PART == 1 && !defined(SAGA_GAMELIB_UTIL_CRC16_H_STOP)

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
    extern const char NuBlankStringValue[2];
    extern i32 append;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
struct ShaderLexicalSlicePc;

#endif
#ifdef __cplusplus
struct ShaderConditionNodePc {
    ShaderConditionNodePc *left;
    ShaderConditionNodePc *right;
    const char *token;
    ShaderConditionNodePc(const char *token, ShaderConditionNodePc *left, ShaderConditionNodePc *right);
    ShaderConditionNodePc *AppendOperand(ShaderConditionNodePc *operand);
};

struct ShaderSourceNodePc {
    u32 flags;
    i32 type;
    i32 array_count;
    const char *name;
    i32 semantic;
    const char *custom_semantic;
    i32 vertex_register;
    i32 pixel_register;
    i32 expansion_state;
    ShaderSourceNodePc(u32, i32, i32, const char *, i32, const char *, i32, i32);
    const char *SemanticName();
    i32 fun_00746620();
    const char *fun_00746630();
};

STATIC_ASSERT(offsetof(ShaderSourceNodePc, expansion_state) == 0x20);

struct ShaderRegisterBlockPc {
    char name[16];
    i32 register_count;

    ShaderRegisterBlockPc() {
        name[0] = 0;
        register_count = 0;
    }

    void fun_00746760(const char *block_name, i32 count);
};

STATIC_ASSERT(sizeof(ShaderRegisterBlockPc) == 20);
STATIC_ASSERT(offsetof(ShaderRegisterBlockPc, register_count) == 16);

struct ShaderMacroDefinitionPc {
    const char *name;
    const char *value;
    u8 emit;
    u8 unknown_9[3];
    ShaderMacroDefinitionPc(const char *name, const char *value, u8 emit);
};

STATIC_ASSERT(sizeof(ShaderMacroDefinitionPc) == 12);
STATIC_ASSERT(offsetof(ShaderMacroDefinitionPc, name) == 0);
STATIC_ASSERT(offsetof(ShaderMacroDefinitionPc, value) == 4);
STATIC_ASSERT(offsetof(ShaderMacroDefinitionPc, emit) == 8);

STATIC_ASSERT(sizeof(ShaderConditionNodePc) == 12);
STATIC_ASSERT(offsetof(ShaderConditionNodePc, left) == 0);
STATIC_ASSERT(offsetof(ShaderConditionNodePc, right) == 4);
STATIC_ASSERT(offsetof(ShaderConditionNodePc, token) == 8);
STATIC_ASSERT(sizeof(ShaderSourceNodePc) == 36);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, flags) == 0);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, type) == 4);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, array_count) == 8);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, name) == 12);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, semantic) == 16);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, custom_semantic) == 20);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, vertex_register) == 24);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, pixel_register) == 28);
STATIC_ASSERT(offsetof(ShaderSourceNodePc, expansion_state) == 32);

STATIC_ASSERT(offsetof(ShaderRegisterBlockPc, name) == 0);
extern "C" void ShaderLexicalClassesInitPc(void);

struct ShaderExpansionNodePc {
    i32 kind;
    ShaderExpansionNodePc *next;

    union {
        struct {
            ShaderExpansionNodePc *if_true;
            ShaderExpansionNodePc *if_false;
            ShaderConditionNodePc *condition;
        } branch;

        struct {
            const char *name;
            const char *value;
        } definition;

        struct {
            u32 flags;
            i32 type;
            i32 array_count;
            const char *name;
            i32 semantic;
            const char *custom_semantic;
            i32 vertex_register;
            i32 pixel_register;
        } declaration;

        struct {
            u32 flags;
            i32 alias_depth;
            const char *name;
        } reference;

        struct {
            u8 enabled;
        } toggle;

        struct {
            u32 flags;
        } emit;

        struct {
            const char *bytes;
            i32 count;
        } text;
    } data;
};

STATIC_ASSERT(sizeof(ShaderExpansionNodePc) == 0x28);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, kind) == 0);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, next) == 4);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.branch.if_true) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.branch.if_false) == 12);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.branch.condition) == 16);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.definition.name) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.definition.value) == 12);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.flags) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.type) == 12);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.array_count) == 16);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.name) == 20);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.semantic) == 24);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.custom_semantic) == 28);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.vertex_register) == 32);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.declaration.pixel_register) == 36);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.reference.flags) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.reference.alias_depth) == 12);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.reference.name) == 16);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.toggle.enabled) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.emit.flags) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.text.bytes) == 8);
STATIC_ASSERT(offsetof(ShaderExpansionNodePc, data.text.count) == 12);
#endif
#ifdef __cplusplus

extern "C" u32 ShaderDeclarationFlagsPc(u8, u8, u8);
#endif
#undef SAGA_GAMELIB_UTIL_CRC16_H_PART
#define SAGA_GAMELIB_UTIL_CRC16_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMELIB_UTIL_CRC16_H_STOP
#endif
#endif

#if SAGA_GAMELIB_UTIL_CRC16_H_PART == 2 && !defined(SAGA_GAMELIB_UTIL_CRC16_H_STOP)
#ifdef __cplusplus

struct ShaderCompilerBufferAllocatorPc {
    u8 *data;
    i32 length;
    i32 capacity;
    u8 owns_memory;
    ShaderCompilerBufferAllocatorPc(i32, bool);
    ShaderCompilerBufferAllocatorPc(VirtualStackAllocator &, i32);
    ~ShaderCompilerBufferAllocatorPc();
    char *CopyText();
    void Resize(i32);
    void AppendBytes(const char *, i32);
    i32 Find(const char *text, i32 start);
    bool MatchesAt(i32 start, const char *text);
    void OverwriteText(i32 start, const char *text);
    void WriteBytes(i32 offset, const void *source, i32 count);
    void AppendText(const char *);
    ShaderCompilerBufferAllocatorPc &operator<<(const char *);
    ShaderCompilerBufferAllocatorPc &operator<<(i32);
    void AppendInt(i32);
    void AppendSlice(const ShaderLexicalSlicePc &slice);
    ShaderCompilerBufferAllocatorPc &operator<<(const ShaderLexicalSlicePc &slice);
    void fun_00746450(char value);
    u8 *fun_00746490();
};

STATIC_ASSERT(sizeof(ShaderCompilerBufferAllocatorPc) == 16);
STATIC_ASSERT(offsetof(ShaderCompilerBufferAllocatorPc, data) == 0);
STATIC_ASSERT(offsetof(ShaderCompilerBufferAllocatorPc, length) == 4);
STATIC_ASSERT(offsetof(ShaderCompilerBufferAllocatorPc, capacity) == 8);
STATIC_ASSERT(offsetof(ShaderCompilerBufferAllocatorPc, owns_memory) == 12);
#endif
#ifdef __cplusplus

struct ShaderCompilerDictionaryPc {
    ShaderMacroDefinitionPc *programs[300];
    i32 program_count;
    ShaderSourceNodePc *definitions[250];
    i32 definition_count;
    ShaderRegisterBlockPc register_blocks[8];
    i32 register_block_count;
    VirtualStackAllocator scratch;
    VirtualStackAllocator output_memory;
    u8 emit_definitions;
    u8 unknown_965;
    u8 unknown_966;
    u8 emit_semantics;
    u8 emit_registers;
    u8 emit_centroid;
    u8 unknown_96a[2];
    const char *register_format;
    bool force_mode;
    u8 unknown_971[3];
    i32 profile;
    i32 mode;
    i32 output_stage;
    i32 first_version;
    i32 second_version;
    const char *profile_name;
    ShaderCompilerBufferAllocatorPc register_buffer;

    union {
        i32 unknown_99c;
        ShaderCompilerBufferAllocatorPc *active_output;
    };

    u32 unknown_9a0;
    ShaderCompilerDictionaryPc(VirtualStackAllocator &, i32);
    ~ShaderCompilerDictionaryPc();
    void SetProfile(i32, i32, i32);
    bool GetForceMode() const;
    i32 GetProfile() const;
    void AddRegisterBlock(const char *, i32);
    i32 FindRegisterBlock(const char *);
    ShaderMacroDefinitionPc *FindDefinition(const char *);
    void Expand(void *, ShaderCompilerBufferAllocatorPc *);
    void AddDefinition(const char *, const char *, i32);
    void BindOutputBuffer(ShaderCompilerBufferAllocatorPc *);
    bool IsNumericToken(const char *);
    bool EvaluateCondition(ShaderConditionNodePc *, i32 *);
    i32 EvaluateValue(ShaderConditionNodePc *);
    const char *RenderASTName(ShaderSourceNodePc *);
    ShaderSourceNodePc *FindASTExact(const char *, u32);
    ShaderSourceNodePc *FindASTUniform(const char *);
    void EmitType(ShaderCompilerBufferAllocatorPc *);
    void EmitRegister(ShaderCompilerBufferAllocatorPc *);
    void EmitVertexInput(ShaderCompilerBufferAllocatorPc *);
    void EmitAttribute(ShaderCompilerBufferAllocatorPc *);
    void EmitProgram(ShaderCompilerBufferAllocatorPc *);
    void AddASTDefinition(ShaderSourceNodePc *);
    void fun_007451c0(bool force);
    void fun_00746940(ShaderConditionNodePc *node, char *text);
    void fun_00746bf0(const char *name);
};

STATIC_ASSERT(sizeof(ShaderCompilerDictionaryPc) == 0x9a4);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, program_count) == 0x4b0);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, definitions) == 0x4b4);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, definition_count) == 0x89c);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, register_blocks) == 0x8a0);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, register_block_count) == 0x940);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, scratch) == 0x944);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, output_memory) == 0x954);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, force_mode) == 0x970);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, profile) == 0x974);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, output_stage) == 0x97c);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, first_version) == 0x980);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, second_version) == 0x984);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, profile_name) == 0x988);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, register_buffer) == 0x98c);

STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, emit_definitions) == 0x964);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, emit_semantics) == 0x967);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, emit_registers) == 0x968);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, emit_centroid) == 0x969);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, register_format) == 0x96c);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, mode) == 0x978);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, unknown_99c) == 0x99c);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, programs) == 0);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, unknown_965) == 0x965);
STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, unknown_966) == 0x966);

STATIC_ASSERT(offsetof(ShaderCompilerDictionaryPc, active_output) == 0x99c);
#endif
#undef SAGA_GAMELIB_UTIL_CRC16_H_PART
#define SAGA_GAMELIB_UTIL_CRC16_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMELIB_UTIL_CRC16_H_STOP
#endif
#endif

#if SAGA_GAMELIB_UTIL_CRC16_H_PART == 3 && !defined(SAGA_GAMELIB_UTIL_CRC16_H_STOP)
#ifdef __cplusplus

struct ShaderLexicalSlicePc {
    const char *begin;
    i32 length;
    bool Equals(const char *text);
    bool StartsWith(const char *text);
    char *HeapCopy();
    char *ArenaCopy(VirtualStackAllocator *arena);
    i32 AsInteger(i32 skipped_prefix);
    const char *GetBegin();
    const char *At(i32 offset);
    bool fun_0074b080(const char *text);
};

struct ShaderLexicalReaderPc {
    bool Next(ShaderCompilerBufferAllocatorPc *input, i32 *cursor, ShaderLexicalSlicePc *slice);
    i32 IsWordChar(u8 c);
    i32 IsTokenChar(u8 c);
};

STATIC_ASSERT(sizeof(ShaderLexicalSlicePc) == 8);
STATIC_ASSERT(offsetof(ShaderLexicalSlicePc, begin) == 0);
STATIC_ASSERT(offsetof(ShaderLexicalSlicePc, length) == 4);
#endif
#ifdef __cplusplus
extern "C" i32 ShaderDeclarationSemanticPc(ShaderLexicalSlicePc *);
#endif
#undef SAGA_GAMELIB_UTIL_CRC16_H_PART
#define SAGA_GAMELIB_UTIL_CRC16_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMELIB_UTIL_CRC16_H_STOP
#endif
#endif

#undef SAGA_GAMELIB_UTIL_CRC16_H_STOP
