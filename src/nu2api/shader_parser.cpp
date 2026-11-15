#define SAGA_SHADER_PARSER_TYPES
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/shader_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Copies a NUL-terminated string into the parse arena.
#define ARENA_COPY_STRING(copy, string)                                                                                                    \
    do {                                                                                                                                   \
        VirtualStackAllocator *stack = arena;                                                                                              \
        i32 length = (i32)strlen(string);                                                                                                  \
        copy = reinterpret_cast<char *>(stack->cursor);                                                                                    \
        stack->cursor += length + 1;                                                                                                       \
        memcpy(copy, string, length + 1);                                                                                                  \
    } while (0)
// Links a new node at the current insertion point.
#define APPEND_NODE(node)                                                                                                                  \
    do {                                                                                                                                   \
        if (tail == NULL) {                                                                                                                \
            head = node;                                                                                                                   \
            tail = &node->next;                                                                                                            \
        } else {                                                                                                                           \
            *tail = node;                                                                                                                  \
            tail = &node->next;                                                                                                            \
        }                                                                                                                                  \
    } while (0)

void ShaderCompilerParser::FlushText() {
    i32 count = text.length;
    if (count > 0) {
        char *bytes = reinterpret_cast<char *>(arena->cursor);
        arena->cursor += count;
        memcpy(bytes, text.data, count);
        ShaderParserTextNode *node = reinterpret_cast<ShaderParserTextNode *>(arena->cursor);
        arena->cursor += sizeof(ShaderParserTextNode);
        if (node != NULL) {
            node->kind = SHADER_NODE_TEXT;
            node->next = NULL;
            node->bytes = bytes;
            node->count = count;
        } else {
            // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
            // the real placement new would emit unmapped inline COMDATs.
            node = NULL;
        }
        APPEND_NODE(node);
        text.length = 0;
    }
}

extern "C" void fun_0074bef0(void) {
}

void ShaderCompilerParser::DefineMacro(const char *name, const char *value) {
    FlushText();
    char *copy_name;
    ARENA_COPY_STRING(copy_name, name);
    char *copy_value;
    if (value != NULL) {
        ARENA_COPY_STRING(copy_value, value);
    } else {
        copy_value = NULL;
    }
    ShaderParserMacroNode *node = reinterpret_cast<ShaderParserMacroNode *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserMacroNode);
    if (node != NULL) {
        node->kind = SHADER_NODE_MACRO;
        node->next = NULL;
        node->name = copy_name;
        node->value = copy_value;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::UndefineMacro(const char *name) {
    FlushText();
    char *copy;
    ARENA_COPY_STRING(copy, name);
    ShaderParserUndefNode *node = reinterpret_cast<ShaderParserUndefNode *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserUndefNode);
    if (node != NULL) {
        node->kind = SHADER_NODE_UNDEF;
        node->next = NULL;
        node->name = copy;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::BeginIf(ShaderConditionNode *condition) {
    FlushText();
    ShaderParserBranchNode *node = reinterpret_cast<ShaderParserBranchNode *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserBranchNode);
    if (node != NULL) {
        node->kind = SHADER_NODE_BRANCH;
        node->next = NULL;
        node->if_true = NULL;
        node->if_false = NULL;
        node->condition = condition;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
    branches[branch_count++] = node;
    scopes[scope_count++] = node;
    tail = &node->if_true;
}

void ShaderCompilerParser::BeginElse() {
    FlushText();
    tail = &branches[branch_count - 1]->if_false;
}

void ShaderCompilerParser::BeginElseIf(ShaderConditionNode *condition) {
    FlushText();
    tail = &branches[branch_count - 1]->if_false;
    ShaderParserBranchNode *node = reinterpret_cast<ShaderParserBranchNode *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserBranchNode);
    if (node != NULL) {
        node->kind = SHADER_NODE_BRANCH;
        node->next = NULL;
        node->if_true = NULL;
        node->if_false = NULL;
        node->condition = condition;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
    branches[branch_count++] = node;
    tail = &node->if_true;
}

void ShaderCompilerParser::EndIf() {
    FlushText();
    while (branches[branch_count - 1] != scopes[scope_count - 1]) {
        --branch_count;
    }
    --branch_count;
    tail = &branches[branch_count]->next;
    --scope_count;
}

struct ShaderParserToggle : ShaderParserNode { // size 0xc
    u8 enabled;                                // 0x8
    u8 unknown_9[3];                           // 0x9
    ShaderParserToggle(u8 enabled);
};

struct ShaderParserReference : ShaderParserNode { // size 0x14
    u32 flags;                                    // 0x08
    i32 alias_depth;                              // 0x0c
    char *name;                                   // 0x10
    ShaderParserReference(u32 flags, i32 alias_depth, char *name);
};

struct ShaderParserDeclaration : ShaderParserNode { // size 0x28
    u32 flags;                                      // 0x08
    i32 type;                                       // 0x0c
    i32 array_count;                                // 0x10
    char *name;                                     // 0x14
    i32 semantic;                                   // 0x18
    char *custom_semantic;                          // 0x1c
    i32 vertex_register;                            // 0x20
    i32 pixel_register;                             // 0x24
    ShaderParserDeclaration(u32 flags, i32 type, i32 array_count, char *name, i32 semantic, char *custom_semantic, i32 vertex_register,
                            i32 pixel_register);
};

struct ShaderParserEmit : ShaderParserNode { // size 0xc
    u32 flags;                               // 0x8
    ShaderParserEmit(u32 flags);
};

ShaderParserNode::ShaderParserNode(i32 kind) : kind(kind), next(NULL) {
}

ShaderParserBranchNode::ShaderParserBranchNode(ShaderConditionNode *condition)
    : ShaderParserNode(0), if_true(NULL), if_false(NULL), condition(condition) {
}

ShaderParserMacroNode::ShaderParserMacroNode(char *name, char *value) : ShaderParserNode(1), name(name), value(value) {
}

ShaderParserUndefNode::ShaderParserUndefNode(char *name) : ShaderParserNode(2), name(name) {
}

ShaderParserDeclaration::ShaderParserDeclaration(u32 flags, i32 type, i32 array_count, char *name, i32 semantic, char *custom_semantic,
                                                 i32 vertex_register, i32 pixel_register)
    : ShaderParserNode(3), flags(flags), type(type), array_count(array_count), name(name), semantic(semantic),
      custom_semantic(custom_semantic), vertex_register(vertex_register), pixel_register(pixel_register) {
}

ShaderParserReference::ShaderParserReference(u32 flags, i32 alias_depth, char *name)
    : ShaderParserNode(4), flags(flags), alias_depth(alias_depth), name(name) {
}

ShaderParserEmit::ShaderParserEmit(u32 flags) : ShaderParserNode(6), flags(flags) {
}

ShaderParserToggle::ShaderParserToggle(u8 enabled) : ShaderParserNode(5), enabled(enabled) {
}

ShaderParserTextNode::ShaderParserTextNode(char *bytes, i32 count) : ShaderParserNode(7), bytes(bytes), count(count) {
}

void ShaderCompilerParser::fun_0074b330(const ShaderLexicalSlice &slice) {
    text.AppendBytes(slice.begin, slice.length);
}

void ShaderCompilerParser::fun_0074b6b0(ShaderParserNode *node) {
    APPEND_NODE(node);
}

void ShaderCompilerParser::SetShowDefinitions(u8 enabled) {
    FlushText();
    ShaderParserToggle *node = reinterpret_cast<ShaderParserToggle *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserToggle);
    if (node != NULL) {
        node->kind = SHADER_NODE_TOGGLE;
        node->next = NULL;
        node->enabled = enabled;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::UseDeclaration(u32 flags, i32 depth, const char *name) {
    FlushText();
    char *copy;
    ARENA_COPY_STRING(copy, name);
    ShaderParserReference *node = reinterpret_cast<ShaderParserReference *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserReference);
    if (node != NULL) {
        node->kind = SHADER_NODE_REFERENCE;
        node->next = NULL;
        node->flags = flags;
        node->alias_depth = depth;
        node->name = copy;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::AddDeclaration(u32 flags, i32 type, i32 count, const char *name, i32 semantic, const char *custom, i32 vertex,
                                          i32 pixel) {
    FlushText();
    char *copy;
    ARENA_COPY_STRING(copy, name);
    char *custom_copy;
    if (custom != NULL) {
        ARENA_COPY_STRING(custom_copy, custom);
    } else {
        custom_copy = NULL;
    }
    ShaderParserDeclaration *node = reinterpret_cast<ShaderParserDeclaration *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserDeclaration);
    if (node != NULL) {
        node->kind = SHADER_NODE_DECLARATION;
        node->next = NULL;
        node->flags = flags;
        node->type = type;
        node->array_count = count;
        node->name = copy;
        node->semantic = semantic;
        node->custom_semantic = custom_copy;
        node->vertex_register = vertex;
        node->pixel_register = pixel;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::EmitDeclarations(u32 flags) {
    FlushText();
    ShaderParserEmit *node = reinterpret_cast<ShaderParserEmit *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserEmit);
    if (node != NULL) {
        node->kind = SHADER_NODE_EMIT;
        node->next = NULL;
        node->flags = flags;
    } else {
        // MATCH-EXCEPTION: redundant reset is the expanded form of the original placement new;
        // the real placement new would emit unmapped inline COMDATs.
        node = NULL;
    }
    APPEND_NODE(node);
}

void ShaderCompilerParser::ParseDeclaration(ShaderCompilerBufferAllocator *input, i32 *cursor) {
    char temporary[200];
    VirtualStackAllocator memory(temporary, sizeof(temporary));
    i32 count = -1;
    i32 pixel = -1;
    char *custom = NULL;
    i32 vertex = -1;
    char *name = NULL;
    i32 state = SHADER_PARSE_KEYWORD;
    i32 type = 0;
    u8 shared = 0;
    u32 flags = 1;
    i32 depth = 0;
    ShaderLexicalSlice token = {NULL, 0};
    i32 semantic = SHADER_SEMANTIC_DEFAULT;
    ShaderLexicalReader *reader = reinterpret_cast<ShaderLexicalReader *>(&lexical_state);
    while (state != SHADER_PARSE_DONE && reader->Next(input, cursor, &token)) {
        switch (state) {
            case SHADER_PARSE_KEYWORD:
                if (token.Equals("nu2_use")) {
                    state = SHADER_PARSE_USE_OPEN;
                } else if (token.Equals("nu2_declare")) {
                    state = SHADER_PARSE_DECLARE_OPEN;
                } else if (token.Equals("nu2_anchorPoint")) {
                    state = SHADER_PARSE_ANCHOR_OPEN;
                } else if (token.Equals("nu2_showDefine")) {
                    state = SHADER_PARSE_SHOW_DEFINE_VALUE;
                } else {
                    token.Equals("nu2_debug");
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_SHOW_DEFINE_VALUE:
                if (token.Equals("true")) {
                    SetShowDefinitions(1);
                } else {
                    if (token.Equals("false")) {
                        count = (count & ~255);
                    }
                    SetShowDefinitions((u8)count);
                }
                state = SHADER_PARSE_DONE;
                break;
            case SHADER_PARSE_USE_OPEN:
                if (token.Equals("(")) {
                    state = SHADER_PARSE_USE_STORAGE;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_USE_STORAGE:
                if (token.Equals("vs_local")) {
                    flags = SHADER_DECL_VS_LOCAL | SHADER_DECL_VERTEX;
                } else if (token.Equals("fs_local")) {
                    flags = SHADER_DECL_FS_LOCAL | SHADER_DECL_PIXEL;
                } else if (token.Equals("varying")) {
                    flags = SHADER_DECL_VARYING | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                } else if (token.Equals("attribute")) {
                    flags = SHADER_DECL_ATTRIBUTE | SHADER_DECL_VERTEX;
                } else if (token.Equals("uniform")) {
                    flags = SHADER_DECL_UNIFORM | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                } else {
                    state = SHADER_PARSE_DONE;
                    break;
                }
                state = SHADER_PARSE_USE_COMMA;
                break;
            case SHADER_PARSE_USE_COMMA:
                token.Equals(",");
                state = SHADER_PARSE_USE_NAME;
                break;
            case SHADER_PARSE_USE_NAME:
                if (token.Equals("*")) {
                    ++depth;
                } else {
                    name = token.ArenaCopy(&memory);
                    state = SHADER_PARSE_USE_CLOSE;
                }
                break;
            case SHADER_PARSE_USE_CLOSE:
                if (token.Equals(")")) {
                    UseDeclaration(flags, depth, name);
                }
                state = SHADER_PARSE_DONE;
                break;
            case SHADER_PARSE_DECLARE_OPEN:
                token.Equals("(");
                state = SHADER_PARSE_DECLARE_STORAGE;
                break;
            case SHADER_PARSE_DECLARE_STORAGE:
                if (token.Equals("vs_local")) {
                    flags = SHADER_DECL_VS_LOCAL | SHADER_DECL_VERTEX;
                    state = SHADER_PARSE_DECLARE_STORAGE_COMMA;
                } else if (token.Equals("fs_local")) {
                    flags = SHADER_DECL_FS_LOCAL | SHADER_DECL_PIXEL;
                    state = SHADER_PARSE_DECLARE_STORAGE_COMMA;
                } else if (token.Equals("varying")) {
                    flags = SHADER_DECL_VARYING | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                    state = SHADER_PARSE_DECLARE_STORAGE_COMMA;
                } else if (token.Equals("attribute")) {
                    flags = SHADER_DECL_ATTRIBUTE | SHADER_DECL_VERTEX;
                    state = SHADER_PARSE_DECLARE_STORAGE_COMMA;
                } else if (token.Equals("uniform")) {
                    flags = SHADER_DECL_UNIFORM | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                    state = SHADER_PARSE_DECLARE_REGISTER;
                } else if (token.Equals("shared")) {
                    state = SHADER_PARSE_DECLARE_SHARED_UNIFORM;
                    shared = 1;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_DECLARE_SHARED_UNIFORM:
                if (token.Equals("uniform")) {
                    flags = SHADER_DECL_UNIFORM | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                    state = SHADER_PARSE_DECLARE_REGISTER;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_DECLARE_REGISTER:
                if (token.Equals(",")) {
                    flags |= shared ? 0x4000 : 0;
                    state = SHADER_PARSE_DECLARE_TYPE;
                } else {
                    if (token.StartsWith("vreg")) {
                        vertex = token.AsInteger(4);
                    } else if (token.StartsWith("freg")) {
                        pixel = token.AsInteger(4);
                    }
                    state = SHADER_PARSE_DECLARE_REGISTER2;
                }
                break;
            case SHADER_PARSE_DECLARE_REGISTER2:
                if (token.Equals(",")) {
                    flags = ShaderDeclarationFlags(shared, vertex >= 0, pixel >= 0);
                    state = SHADER_PARSE_DECLARE_TYPE;
                } else {
                    if (token.StartsWith("vreg")) {
                        vertex = token.AsInteger(4);
                    } else if (token.StartsWith("freg")) {
                        pixel = token.AsInteger(4);
                    }
                    state = SHADER_PARSE_DECLARE_STORAGE_COMMA;
                }
                break;
            case SHADER_PARSE_DECLARE_STORAGE_COMMA:
                token.Equals(",");
                if ((flags & SHADER_DECL_UNIFORM) != 0) {
                    flags = ShaderDeclarationFlags(shared, vertex >= 0, pixel >= 0);
                }
                state = SHADER_PARSE_DECLARE_TYPE;
                break;
            case SHADER_PARSE_DECLARE_TYPE:
                if (token.Equals("1")) {
                    type = SHADER_TYPE_FLOAT;
                } else if (token.Equals("2")) {
                    type = SHADER_TYPE_FLOAT2;
                } else if (token.Equals("3")) {
                    type = SHADER_TYPE_FLOAT3;
                } else if (token.Equals("4")) {
                    type = SHADER_TYPE_FLOAT4;
                } else if (token.Equals("float")) {
                    type = SHADER_TYPE_FLOAT;
                } else if (token.Equals("float1")) {
                    type = SHADER_TYPE_FLOAT1;
                } else if (token.Equals("float2")) {
                    type = SHADER_TYPE_FLOAT2;
                } else if (token.Equals("float3")) {
                    type = SHADER_TYPE_FLOAT3;
                } else if (token.Equals("float4")) {
                    type = SHADER_TYPE_FLOAT4;
                } else if (token.Equals("float3x3")) {
                    type = SHADER_TYPE_FLOAT3X3;
                } else if (token.Equals("float4x4")) {
                    type = SHADER_TYPE_FLOAT4X4;
                } else if (token.Equals("half")) {
                    type = SHADER_TYPE_HALF;
                } else if (token.Equals("half1")) {
                    type = SHADER_TYPE_HALF1;
                } else if (token.Equals("half2")) {
                    type = SHADER_TYPE_HALF2;
                } else if (token.Equals("half3")) {
                    type = SHADER_TYPE_HALF3;
                } else if (token.Equals("half4")) {
                    type = SHADER_TYPE_HALF4;
                } else if (token.Equals("half3x3")) {
                    type = SHADER_TYPE_HALF3X3;
                } else if (token.Equals("half4x4")) {
                    type = SHADER_TYPE_HALF4X4;
                } else if (token.Equals("bool")) {
                    type = SHADER_TYPE_BOOL;
                } else if (token.Equals("hook")) {
                    type = SHADER_TYPE_HOOK;
                } else if (token.Equals("int")) {
                    type = SHADER_TYPE_INT;
                } else if (token.Equals("sampler")) {
                    type = SHADER_TYPE_SAMPLER2D;
                } else if (token.Equals("sampler1D")) {
                    type = SHADER_TYPE_SAMPLER1D;
                } else if (token.Equals("sampler2D")) {
                    type = SHADER_TYPE_SAMPLER2D;
                } else if (token.Equals("sampler3D")) {
                    type = SHADER_TYPE_SAMPLER3D;
                } else if (token.Equals("samplerCUBE")) {
                    type = SHADER_TYPE_SAMPLERCUBE;
                }
                state = type != 0 ? 14 : 26;
                break;
            case SHADER_PARSE_DECLARE_ARRAY:
                if (token.Equals("[")) {
                    state = SHADER_PARSE_DECLARE_ARRAY_COUNT;
                } else if (token.Equals(",")) {
                    state = SHADER_PARSE_DECLARE_NAME;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_DECLARE_ARRAY_COUNT:
                if (token.Equals("]")) {
                    count = -2;
                    state = SHADER_PARSE_DECLARE_ARRAY_COMMA;
                } else {
                    count = token.AsInteger(0);
                    state = SHADER_PARSE_DECLARE_ARRAY_CLOSE;
                }
                break;
            case SHADER_PARSE_DECLARE_ARRAY_CLOSE:
                if (token.Equals("]")) {
                    state = SHADER_PARSE_DECLARE_ARRAY_COMMA;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_DECLARE_ARRAY_COMMA:
                if (token.Equals(",")) {
                    state = SHADER_PARSE_DECLARE_NAME;
                } else {
                    state = SHADER_PARSE_DONE;
                }
                break;
            case SHADER_PARSE_DECLARE_NAME:
                name = token.ArenaCopy(&memory);
                state = SHADER_PARSE_DECLARE_NAME_COMMA;
                break;
            case SHADER_PARSE_DECLARE_NAME_COMMA:
                token.Equals(",");
                state = SHADER_PARSE_DECLARE_SEMANTIC;
                break;
            case SHADER_PARSE_DECLARE_SEMANTIC:
                semantic = ShaderDeclarationSemantic(&token);
                if (semantic == SHADER_SEMANTIC_CUSTOM) {
                    custom = token.ArenaCopy(&memory);
                }
                state = SHADER_PARSE_DECLARE_CLOSE;
                break;
            case SHADER_PARSE_DECLARE_CLOSE:
                if (token.Equals(")")) {
                    AddDeclaration(flags, type, count, name, semantic, custom, vertex, pixel);
                }
                state = SHADER_PARSE_DONE;
                break;
            case SHADER_PARSE_ANCHOR_OPEN:
                token.Equals("(");
                state = SHADER_PARSE_ANCHOR_STORAGE;
                break;
            case SHADER_PARSE_ANCHOR_STORAGE:
                if (token.Equals("uniform")) {
                    flags = SHADER_DECL_UNIFORM | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                } else if (token.Equals("attribute")) {
                    flags = SHADER_DECL_ATTRIBUTE | SHADER_DECL_VERTEX;
                } else if (token.Equals("varying")) {
                    flags = SHADER_DECL_VARYING | SHADER_DECL_PIXEL | SHADER_DECL_VERTEX;
                } else if (token.Equals("vs_local")) {
                    flags = SHADER_DECL_VS_LOCAL | SHADER_DECL_VERTEX;
                } else if (token.Equals("fs_local")) {
                    flags = SHADER_DECL_FS_LOCAL | SHADER_DECL_PIXEL;
                } else {
                    state = SHADER_PARSE_DONE;
                    break;
                }
                state = SHADER_PARSE_ANCHOR_CLOSE;
                break;
            case SHADER_PARSE_ANCHOR_CLOSE:
                if (token.Equals(")")) {
                    EmitDeclarations(flags);
                }
                state = SHADER_PARSE_DONE;
                break;
            default:
                break;
        }
    }
}

template <class T> inline void SafeDeleteArray(T *&pointer) {
    if (pointer != NULL) {
        delete[] pointer;
        pointer = NULL;
    }
}

struct ShaderParserInclude {              // size 0x14
    const char *name;                     // 0x00
    ShaderCompilerBufferAllocator buffer; // 0x04

  private:
    friend class ShaderCompilerParser;

    ShaderParserInclude(const char *name, i32 size) : name(name), buffer(size, true) {
    }

    ~ShaderParserInclude() {
        SafeDeleteArray(name);
    }
};

ShaderCompilerParser::ShaderCompilerParser() : text((ShaderLexicalClassesInit(), 0x1000), true) {
    include_directories[0] = 0;
    head = NULL;
    tail = NULL;
    emit_text = 0;
    arena = NULL;
    memset(branches, 0, sizeof(branches));
    branch_count = 0;
    memset(scopes, 0, sizeof(scopes));
    scope_count = 0;
}

void ShaderCompilerParser::ClearIncludes() {
    for (std::vector<ShaderParserInclude *>::iterator i = includes.begin(); i != includes.end(); ++i) {
        delete *i;
    }
    includes.erase(includes.begin(), includes.end());
}

/* Out-of-line copies of the include-list members the parser's loops inline. */
template void std::vector<ShaderParserInclude *>::clear();
template bool
std::vector<ShaderParserInclude *>::const_iterator::operator<(const std::vector<ShaderParserInclude *>::const_iterator &) const;

ShaderCompilerParser::~ShaderCompilerParser() {
    ClearIncludes();
}

void ShaderCompilerParser::SetIncludeDirectories(const char *paths) {
    strcpy(include_directories, paths);
}

extern "C" bool fun_0074b2f0(ShaderCompilerBufferAllocator *line, i32 *position) {
    for (i32 i = 0; i < line->length; ++i) {
        u8 c = line->data[i];
        if (c != ' ' && c != '\t') {
            if (c == '#') {
                *position = i;
                return true;
            }
            break;
        }
    }
    return false;
}

bool ShaderCompilerParser::NextIncludeDirectory(const char *paths, i32 *cursor, char *directory) {
    if (*cursor >= (i32)strlen(paths)) {
        return false;
    }
    paths += *cursor;
    while (*paths != 0 && *paths != ';') {
        *directory++ = *paths++;
        ++*cursor;
    }
    *directory = 0;
    if (*paths == ';') {
        ++*cursor;
    }
    return true;
}

extern "C" void fun_0074b9f0(void) {
}

extern "C" void fun_0074bae0(void) {
}

ShaderCompilerBufferAllocator *ShaderCompilerParser::LoadInclude(const char *name) {
    for (std::vector<ShaderParserInclude *>::iterator i = includes.begin(); i != includes.end(); ++i) {
        if (strcmp((*i)->name, name) == 0) {
            return &(*i)->buffer;
        }
    }
    char directory[256];
    char path[256];
    i32 cursor = 0;
    NUFILE file = 0;

    bool more = NextIncludeDirectory(include_directories, &cursor, directory);
    while (more) {
        if (file != 0) {
            break;
        }
        sprintf(path, "%s\\%s", directory, name);
        file = NuFileOpen(path, 0);
        more = NextIncludeDirectory(include_directories, &cursor, directory);
    }
    i32 size = (i32)NuFileOpenSize(file);
    if (size <= 0) {
        return NULL;
    }
    ShaderParserInclude *include = new ShaderParserInclude(_strdup(name), size);
    if (include->buffer.capacity < size) {
        include->buffer.Resize(size);
    }
    include->buffer.length = size;
    includes.push_back(include);
    NuFileRead(file, include->buffer.data, include->buffer.length);
    NuFileClose(file);
    return &include->buffer;
}

void ShaderCompilerParser::ProcessDirective(ShaderCompilerBufferAllocator *input, i32 *cursor) {
    char temporary[200];
    VirtualStackAllocator memory(temporary, sizeof(temporary));
    char *name = NULL;
    ShaderLexicalSlice token = {NULL, 0};
    i32 state = SHADER_DIRECTIVE_KEYWORD;
    ShaderLexicalReader *reader = reinterpret_cast<ShaderLexicalReader *>(&lexical_state);
    while (state != SHADER_DIRECTIVE_DONE && reader->Next(input, cursor, &token)) {
        switch (state) {
            case SHADER_DIRECTIVE_KEYWORD:
                if (token.Equals("define")) {
                    state = SHADER_DIRECTIVE_DEFINE_NAME;
                } else if (token.Equals("ifdef")) {
                    // Not handled; the following tokens are still scanned.
                } else if (token.Equals("ifndef")) {
                    // Not handled; the following tokens are still scanned.
                } else if (token.Equals("undef")) {
                    state = SHADER_DIRECTIVE_UNDEF_NAME;
                } else if (token.Equals("include")) {
                    state = SHADER_DIRECTIVE_INCLUDE_PATH;
                } else if (token.Equals("pragma")) {
                    ParseDeclaration(input, cursor);
                    state = SHADER_DIRECTIVE_DONE;
                } else if (token.Equals("if")) {
                    BeginIf(ParseExpression(input, cursor));
                    state = SHADER_DIRECTIVE_DONE;
                } else if (token.Equals("else")) {
                    BeginElse();
                    state = SHADER_DIRECTIVE_DONE;
                } else if (token.Equals("elif")) {
                    BeginElseIf(ParseExpression(input, cursor));
                    state = SHADER_DIRECTIVE_DONE;
                } else if (token.Equals("endif")) {
                    EndIf();
                    state = SHADER_DIRECTIVE_DONE;
                } else {
                    state = SHADER_DIRECTIVE_DONE;
                }
                break;
            case SHADER_DIRECTIVE_DEFINE_NAME:
                name = token.ArenaCopy(&memory);
                state = SHADER_DIRECTIVE_DEFINE_VALUE;
                break;
            case SHADER_DIRECTIVE_DEFINE_VALUE:
                DefineMacro(name, token.ArenaCopy(&memory));
                state = SHADER_DIRECTIVE_DONE;
                break;
            case SHADER_DIRECTIVE_UNDEF_NAME:
                UndefineMacro(token.ArenaCopy(&memory));
                state = SHADER_DIRECTIVE_DONE;
                break;
            case SHADER_DIRECTIVE_INCLUDE_PATH: {
                char *path = token.HeapCopy();
                ShaderCompilerBufferAllocator *included = LoadInclude(path + 1);
                if (included != NULL) {
                    LoadSourceLines(included, 0);
                }
                delete path;
                state = SHADER_DIRECTIVE_DONE;
                break;
            }
        }
    }
    if (state == SHADER_DIRECTIVE_DEFINE_VALUE) {
        DefineMacro(name, NULL);
    }
}

void ShaderCompilerParser::LoadSourceLines(ShaderCompilerBufferAllocator *input, i32 offset) {
    char line_memory[512];
    VirtualStackAllocator memory(line_memory, sizeof(line_memory));
    ShaderCompilerBufferAllocator line(memory, sizeof(line_memory));
    while (offset < input->length) {
        const char *first = reinterpret_cast<const char *>(input->data) + offset;
        const char *end = reinterpret_cast<const char *>(input->data) + input->length;
        const char *p = first;
        i32 count = 0;
        while (p < end) {
            ++count;
            if (*p == '\n') {
                break;
            }
            ++p;
        }
        line.AppendBytes(first, count);
        offset += count;
        i32 cursor = 0;
        i32 position = 0;
        while (position < line.length && (line.data[position] == ' ' || line.data[position] == '\t')) {
            ++position;
        }
        if (position < line.length && line.data[position] == '#') {
            cursor = position + 1;
            ProcessDirective(&line, &cursor);
        } else if (emit_text != 0) {
            text.AppendBytes(reinterpret_cast<const char *>(line.data), line.length);
        }
        line.length = 0;
    }
}

ShaderParserNode *ShaderCompilerParser::Parse(ShaderCompilerBufferAllocator *input, i32 offset, u8 emit, VirtualStackAllocator *memory) {
    arena = memory;
    emit_text = emit;
    head = NULL;
    tail = NULL;
    branch_count = 0;
    scope_count = 0;
    LoadSourceLines(input, offset);
    FlushText();
    ShaderParserNode *result = head;
    arena = NULL;
    emit_text = 0;
    return result;
}
