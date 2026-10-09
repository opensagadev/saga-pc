#define SAGA_SHADER_PARSER_TYPES
#include "saga.h"
#include <string.h>
void ShaderCompilerParserPc::FlushText() {
    i32 count = text.length;
    if (count > 0) {
        char *bytes = reinterpret_cast<char *>(arena->cursor);
        arena->cursor += count;
        memcpy(bytes, text.data, count);
        ShaderParserTextNodePc *node = reinterpret_cast<ShaderParserTextNodePc *>(arena->cursor);
        arena->cursor += sizeof(ShaderParserTextNodePc);
        if (node != NULL) {
            node->kind = 7;
            node->next = NULL;
            node->bytes = bytes;
            node->count = count;
        }
        if (tail == NULL)
            head = node;
        else
            *tail = node;
        tail = &node->next;
        text.length = 0;
    }
}
void ShaderCompilerParserPc::DefineMacro(const char *name, const char *value) {
    FlushText();
    i32 bytes = (i32)strlen(name) + 1;
    char *copy_name = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += bytes;
    memcpy(copy_name, name, bytes);
    char *copy_value = NULL;
    if (value != NULL) {
        bytes = (i32)strlen(value) + 1;
        copy_value = reinterpret_cast<char *>(arena->cursor);
        arena->cursor += bytes;
        memcpy(copy_value, value, bytes);
    }
    ShaderParserMacroNodePc *node = reinterpret_cast<ShaderParserMacroNodePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserMacroNodePc);
    if (node != NULL) {
        node->kind = 1;
        node->next = NULL;
        node->name = copy_name;
        node->value = copy_value;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}
void ShaderCompilerParserPc::UndefineMacro(const char *name) {
    FlushText();
    i32 bytes = (i32)strlen(name) + 1;
    char *copy = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += bytes;
    memcpy(copy, name, bytes);
    ShaderParserUndefNodePc *node = reinterpret_cast<ShaderParserUndefNodePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserUndefNodePc);
    if (node != NULL) {
        node->kind = 2;
        node->next = NULL;
        node->name = copy;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}
void ShaderCompilerParserPc::BeginIf(ShaderConditionNodePc *condition) {
    FlushText();
    ShaderParserBranchNodePc *node = reinterpret_cast<ShaderParserBranchNodePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserBranchNodePc);
    if (node != NULL) {
        node->kind = 0;
        node->next = NULL;
        node->if_true = NULL;
        node->if_false = NULL;
        node->condition = condition;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
    branches[branch_count++] = node;
    scopes[scope_count++] = node;
    tail = &node->if_true;
}
void ShaderCompilerParserPc::BeginElse() {
    FlushText();
    tail = &branches[branch_count - 1]->if_false;
}
void ShaderCompilerParserPc::BeginElseIf(ShaderConditionNodePc *condition) {
    FlushText();
    tail = &branches[branch_count - 1]->if_false;
    ShaderParserBranchNodePc *node = reinterpret_cast<ShaderParserBranchNodePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserBranchNodePc);
    if (node != NULL) {
        node->kind = 0;
        node->next = NULL;
        node->if_true = NULL;
        node->if_false = NULL;
        node->condition = condition;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
    branches[branch_count++] = node;
    tail = &node->if_true;
}
void ShaderCompilerParserPc::EndIf() {
    FlushText();
    while (branches[branch_count - 1] != scopes[scope_count - 1])
        --branch_count;
    --branch_count;
    tail = &branches[branch_count]->next;
    --scope_count;
}

struct ShaderParserTogglePc : ShaderParserNodePc {
    u8 enabled;
    u8 unknown_9[3];
};
struct ShaderParserReferencePc : ShaderParserNodePc {
    u32 flags;
    i32 alias_depth;
    char *name;
};
struct ShaderParserDeclarationPc : ShaderParserNodePc {
    u32 flags;
    i32 type;
    i32 array_count;
    char *name;
    i32 semantic;
    char *custom_semantic;
    i32 vertex_register;
    i32 pixel_register;
};
struct ShaderParserEmitPc : ShaderParserNodePc {
    u32 flags;
};
STATIC_ASSERT(sizeof(ShaderParserTogglePc) == 12);
STATIC_ASSERT(offsetof(ShaderParserTogglePc, enabled) == 8);
STATIC_ASSERT(sizeof(ShaderParserReferencePc) == 20);
STATIC_ASSERT(offsetof(ShaderParserReferencePc, flags) == 8);
STATIC_ASSERT(offsetof(ShaderParserReferencePc, alias_depth) == 12);
STATIC_ASSERT(offsetof(ShaderParserReferencePc, name) == 16);
STATIC_ASSERT(sizeof(ShaderParserDeclarationPc) == 40);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, flags) == 8);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, type) == 12);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, array_count) == 16);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, name) == 20);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, semantic) == 24);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, custom_semantic) == 28);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, vertex_register) == 32);
STATIC_ASSERT(offsetof(ShaderParserDeclarationPc, pixel_register) == 36);
STATIC_ASSERT(sizeof(ShaderParserEmitPc) == 12);
STATIC_ASSERT(offsetof(ShaderParserEmitPc, flags) == 8);
STATIC_ASSERT(sizeof(ShaderCompilerParserPc) == 0x45c);

void ShaderCompilerParserPc::SetShowDefinitions(u8 enabled) {
    FlushText();
    ShaderParserTogglePc *node = reinterpret_cast<ShaderParserTogglePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserTogglePc);
    if (node != NULL) {
        node->kind = 5;
        node->next = NULL;
        node->enabled = enabled;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}
void ShaderCompilerParserPc::UseDeclaration(u32 flags, i32 depth, const char *name) {
    FlushText();
    i32 bytes = (i32)strlen(name) + 1;
    char *copy = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += bytes;
    memcpy(copy, name, bytes);
    ShaderParserReferencePc *node = reinterpret_cast<ShaderParserReferencePc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserReferencePc);
    if (node != NULL) {
        node->kind = 4;
        node->next = NULL;
        node->flags = flags;
        node->alias_depth = depth;
        node->name = copy;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}
void ShaderCompilerParserPc::AddDeclaration(u32 flags, i32 type, i32 count, const char *name, i32 semantic,
                                            const char *custom, i32 vertex, i32 pixel) {
    FlushText();
    i32 bytes = (i32)strlen(name) + 1;
    char *copy = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += bytes;
    memcpy(copy, name, bytes);
    char *custom_copy = NULL;
    if (custom != NULL) {
        bytes = (i32)strlen(custom) + 1;
        custom_copy = reinterpret_cast<char *>(arena->cursor);
        arena->cursor += bytes;
        memcpy(custom_copy, custom, bytes);
    }
    ShaderParserDeclarationPc *node = reinterpret_cast<ShaderParserDeclarationPc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserDeclarationPc);
    if (node != NULL) {
        node->kind = 3;
        node->next = NULL;
        node->flags = flags;
        node->type = type;
        node->array_count = count;
        node->name = copy;
        node->semantic = semantic;
        node->custom_semantic = custom_copy;
        node->vertex_register = vertex;
        node->pixel_register = pixel;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}
void ShaderCompilerParserPc::EmitDeclarations(u32 flags) {
    FlushText();
    ShaderParserEmitPc *node = reinterpret_cast<ShaderParserEmitPc *>(arena->cursor);
    arena->cursor += sizeof(ShaderParserEmitPc);
    if (node != NULL) {
        node->kind = 6;
        node->next = NULL;
        node->flags = flags;
    }
    if (tail == NULL)
        head = node;
    else
        *tail = node;
    tail = &node->next;
}

void ShaderCompilerParserPc::ParseDeclaration(ShaderCompilerBufferAllocatorPc *input, i32 *cursor) {
    char temporary[200];
    VirtualStackAllocator memory(temporary, sizeof(temporary));
    i32 state = 0, type = 0, count = -1, vertex = -1, pixel = -1, depth = 0, semantic = 1;
    u32 flags = 1;
    u8 shared = 0;
    char *name = NULL, *custom = NULL;
    ShaderLexicalSlicePc token = {NULL, 0};
    ShaderLexicalReaderPc *reader = reinterpret_cast<ShaderLexicalReaderPc *>(&lexical_state);
    while (reader->Next(input, cursor, &token)) {
        switch (state) {
            case 0:
                if (token.Equals("nu2_use"))
                    state = 2;
                else if (token.Equals("nu2_declare"))
                    state = 7;
                else if (token.Equals("nu2_anchorPoint"))
                    state = 22;
                else if (token.Equals("nu2_showDefine"))
                    state = 1;
                else {
                    token.Equals("nu2_debug");
                    return;
                }
                break;
            case 1:
                if (token.Equals("true")) {
                    SetShowDefinitions(1);
                    return;
                }
                if (token.Equals("false"))
                    count = (count & ~255);
                SetShowDefinitions((u8)count);
                return;
            case 2:
                if (!token.Equals("("))
                    return;
                state = 3;
                break;
            case 3:
                if (token.Equals("vs_local"))
                    flags = 0x2002;
                else if (token.Equals("fs_local"))
                    flags = 0x1004;
                else if (token.Equals("varying"))
                    flags = 0x3008;
                else if (token.Equals("attribute"))
                    flags = 0x2010;
                else if (token.Equals("uniform"))
                    flags = 0x3020;
                else
                    return;
                state = 4;
                break;
            case 4:
                token.Equals(",");
                state = 5;
                break;
            case 5:
                if (token.Equals("*"))
                    ++depth;
                else {
                    name = token.ArenaCopy(&memory);
                    state = 6;
                }
                break;
            case 6:
                if (token.Equals(")"))
                    UseDeclaration(flags, depth, name);
                return;
            case 7:
                token.Equals("(");
                state = 8;
                break;
            case 8:
                if (token.Equals("vs_local")) {
                    flags = 0x2002;
                    state = 12;
                } else if (token.Equals("fs_local")) {
                    flags = 0x1004;
                    state = 12;
                } else if (token.Equals("varying")) {
                    flags = 0x3008;
                    state = 12;
                } else if (token.Equals("attribute")) {
                    flags = 0x2010;
                    state = 12;
                } else if (token.Equals("uniform")) {
                    flags = 0x3020;
                    state = 10;
                } else if (token.Equals("shared")) {
                    shared = 1;
                    state = 9;
                } else
                    return;
                break;
            case 9:
                if (!token.Equals("uniform"))
                    return;
                flags = 0x3020;
                state = 10;
                break;
            case 10:
                if (token.Equals(",")) {
                    flags = 0x3020 | (shared ? 0x4000 : 0);
                    state = 13;
                } else {
                    if (token.StartsWith("vreg"))
                        vertex = token.AsInteger(4);
                    else if (token.StartsWith("freg"))
                        pixel = token.AsInteger(4);
                    state = 11;
                }
                break;
            case 11:
                if (token.Equals(",")) {
                    flags = ShaderDeclarationFlagsPc(shared, vertex >= 0, pixel >= 0);
                    state = 13;
                } else {
                    if (token.StartsWith("vreg"))
                        vertex = token.AsInteger(4);
                    else if (token.StartsWith("freg"))
                        pixel = token.AsInteger(4);
                    state = 12;
                }
                break;
            case 12:
                token.Equals(",");
                if ((flags & 0x20) != 0)
                    flags = ShaderDeclarationFlagsPc(shared, vertex >= 0, pixel >= 0);
                state = 13;
                break;
            case 13:
                if (token.Equals("1"))
                    type = 1;
                else if (token.Equals("2"))
                    type = 3;
                else if (token.Equals("3"))
                    type = 4;
                else if (token.Equals("4"))
                    type = 5;
                else if (token.Equals("float"))
                    type = 1;
                else if (token.Equals("float1"))
                    type = 2;
                else if (token.Equals("float2"))
                    type = 3;
                else if (token.Equals("float3"))
                    type = 4;
                else if (token.Equals("float4"))
                    type = 5;
                else if (token.Equals("float3x3"))
                    type = 6;
                else if (token.Equals("float4x4"))
                    type = 7;
                else if (token.Equals("half"))
                    type = 8;
                else if (token.Equals("half1"))
                    type = 9;
                else if (token.Equals("half2"))
                    type = 10;
                else if (token.Equals("half3"))
                    type = 11;
                else if (token.Equals("half4"))
                    type = 12;
                else if (token.Equals("half3x3"))
                    type = 13;
                else if (token.Equals("half4x4"))
                    type = 14;
                else if (token.Equals("bool"))
                    type = 15;
                else if (token.Equals("hook"))
                    type = 21;
                else if (token.Equals("int"))
                    type = 16;
                else if (token.Equals("sampler"))
                    type = 18;
                else if (token.Equals("sampler1D"))
                    type = 17;
                else if (token.Equals("sampler2D"))
                    type = 18;
                else if (token.Equals("sampler3D"))
                    type = 19;
                else if (token.Equals("samplerCUBE"))
                    type = 20;
                state = type != 0 ? 14 : 26;
                if (state == 26)
                    return;
                break;
            case 14:
                if (token.Equals("["))
                    state = 15;
                else if (token.Equals(","))
                    state = 18;
                else
                    return;
                break;
            case 15:
                if (token.Equals("]")) {
                    count = -2;
                    state = 17;
                } else {
                    count = token.AsInteger(0);
                    state = 16;
                }
                break;
            case 16:
                if (!token.Equals("]"))
                    return;
                state = 17;
                break;
            case 17:
                if (!token.Equals(","))
                    return;
                state = 18;
                break;
            case 18:
                name = token.ArenaCopy(&memory);
                state = 19;
                break;
            case 19:
                token.Equals(",");
                state = 20;
                break;
            case 20:
                semantic = ShaderDeclarationSemanticPc(&token);
                if (semantic == 0)
                    custom = token.ArenaCopy(&memory);
                state = 21;
                break;
            case 21:
                if (token.Equals(")"))
                    AddDeclaration(flags, type, count, name, semantic, custom, vertex, pixel);
                return;
            case 22:
                token.Equals("(");
                state = 23;
                break;
            case 23:
                if (token.Equals("uniform"))
                    flags = 0x3020;
                else if (token.Equals("attribute"))
                    flags = 0x2010;
                else if (token.Equals("varying"))
                    flags = 0x3008;
                else if (token.Equals("vs_local"))
                    flags = 0x2002;
                else if (token.Equals("fs_local"))
                    flags = 0x1004;
                else
                    return;
                state = 24;
                break;
            case 24:
                if (token.Equals(")"))
                    EmitDeclarations(flags);
                return;
            default:
                break;
        }
    }
}

#include <stdio.h>
#include <stdlib.h>
struct ShaderParserIncludePc {
    char *name;
    ShaderCompilerBufferAllocatorPc buffer;

  private:
    friend class ShaderCompilerParserPc;
    ShaderParserIncludePc(const char *path, i32 size) : name(_strdup(path)), buffer(size, true) {
    }
    ~ShaderParserIncludePc() {
        if (name != NULL) {
            delete[] name;
            name = NULL;
        }
    }
};
STATIC_ASSERT(sizeof(ShaderParserIncludePc) == 20);
STATIC_ASSERT(offsetof(ShaderParserIncludePc, name) == 0);
STATIC_ASSERT(offsetof(ShaderParserIncludePc, buffer) == 4);

ShaderCompilerParserPc::ShaderCompilerParserPc() : text((ShaderLexicalClassesInitPc(), 0x1000), true) {
    include_directories[0] = 0;
    head = NULL;
    tail = NULL;
    arena = NULL;
    emit_text = 0;
    memset(branches, 0, sizeof(branches));
    branch_count = 0;
    memset(scopes, 0, sizeof(scopes));
    scope_count = 0;
}
void ShaderCompilerParserPc::ClearIncludes() {
    for (std::vector<ShaderParserIncludePc *>::iterator i = includes.begin(); i != includes.end(); ++i)
        if (*i != NULL)
            delete *i;
    includes.erase(includes.begin(), includes.end());
}
ShaderCompilerParserPc::~ShaderCompilerParserPc() {
    ClearIncludes();
}
void ShaderCompilerParserPc::SetIncludeDirectories(const char *paths) {
    strcpy(include_directories, paths);
}

bool ShaderCompilerParserPc::NextIncludeDirectory(const char *paths, i32 *cursor, char *directory) {
    if (*cursor >= (i32)strlen(paths))
        return false;
    const char *p = paths + *cursor;
    while (*p != 0 && *p != ';') {
        *directory++ = *p++;
        ++*cursor;
    }
    *directory = 0;
    if (*p == ';')
        ++*cursor;
    return true;
}
ShaderCompilerBufferAllocatorPc *ShaderCompilerParserPc::LoadInclude(const char *name) {
    for (std::vector<ShaderParserIncludePc *>::iterator i = includes.begin(); i != includes.end(); ++i)
        if (strcmp((*i)->name, name) == 0)
            return &(*i)->buffer;
    char directory[256], path[256];
    i32 cursor = 0;
    NUFILE file = 0;

    bool more = NextIncludeDirectory(include_directories, &cursor, directory);
    while (more) {
        if (file != 0)
            break;
        sprintf(path, "%s\\%s", directory, name);
        file = NuFileOpen(path, 0);
        more = NextIncludeDirectory(include_directories, &cursor, directory);
    }
    i32 size = (i32)NuFileOpenSize(file);
    if (size <= 0)
        return NULL;
    ShaderParserIncludePc *include = new ShaderParserIncludePc(name, size);
    if (include->buffer.capacity < size)
        include->buffer.Resize(size);
    include->buffer.length = size;
    includes.push_back(include);
    NuFileRead(file, include->buffer.data, include->buffer.length);
    NuFileClose(file);
    return &include->buffer;
}

void ShaderCompilerParserPc::ProcessDirective(ShaderCompilerBufferAllocatorPc *input, i32 *cursor) {
    char temporary[200];
    VirtualStackAllocator memory(temporary, sizeof(temporary));
    ShaderLexicalSlicePc token = {NULL, 0};
    char *name = NULL;
    i32 state = 0;
    ShaderLexicalReaderPc *reader = reinterpret_cast<ShaderLexicalReaderPc *>(&lexical_state);
    while (reader->Next(input, cursor, &token)) {
        switch (state) {
            case 0:
                if (token.Equals("define"))
                    state = 1;
                else if (token.Equals("ifdef"))
                    continue;
                else if (token.Equals("ifndef"))
                    continue;
                else if (token.Equals("undef"))
                    state = 3;
                else if (token.Equals("include"))
                    state = 4;
                else if (token.Equals("pragma")) {
                    ParseDeclaration(input, cursor);
                    return;
                } else if (token.Equals("if")) {
                    BeginIf(ParseExpression(input, cursor));
                    return;
                } else if (token.Equals("else")) {
                    BeginElse();
                    return;
                } else if (token.Equals("elif")) {
                    BeginElseIf(ParseExpression(input, cursor));
                    return;
                } else if (token.Equals("endif")) {
                    EndIf();
                    return;
                } else
                    return;
                break;
            case 1:
                name = token.ArenaCopy(&memory);
                state = 2;
                break;
            case 2:
                DefineMacro(name, token.ArenaCopy(&memory));
                return;
            case 3:
                UndefineMacro(token.ArenaCopy(&memory));
                return;
            case 4: {
                char *path = token.HeapCopy();
                ShaderCompilerBufferAllocatorPc *included = LoadInclude(path + 1);
                if (included != NULL)
                    LoadSourceLines(included, 0);
                free(path);
                return;
            }
            default:
                if (state == 5)
                    break;
                continue;
        }
    }
    if (state == 2)
        DefineMacro(name, NULL);
}
void ShaderCompilerParserPc::LoadSourceLines(ShaderCompilerBufferAllocatorPc *input, i32 offset) {
    char line_memory[512];
    VirtualStackAllocator memory(line_memory, sizeof(line_memory));
    ShaderCompilerBufferAllocatorPc line(memory, sizeof(line_memory));
    while (offset < input->length) {
        const char *first = reinterpret_cast<const char *>(input->data) + offset;
        const char *end = reinterpret_cast<const char *>(input->data) + input->length;
        const char *p = first;
        i32 count = 0;
        while (p < end) {
            ++count;
            if (*p == '\n')
                break;
            ++p;
        }
        line.AppendBytes(first, count);
        offset += count;
        i32 cursor = 0;
        while (cursor < line.length && (line.data[cursor] == ' ' || line.data[cursor] == '\t'))
            ++cursor;
        if (cursor < line.length && line.data[cursor] == '#') {
            ++cursor;
            ProcessDirective(&line, &cursor);
        } else if (emit_text != 0)
            text.AppendBytes(reinterpret_cast<const char *>(line.data), line.length);
        line.length = 0;
    }
}
ShaderParserNodePc *ShaderCompilerParserPc::Parse(ShaderCompilerBufferAllocatorPc *input, i32 offset, u8 emit,
                                                  VirtualStackAllocator *memory) {
    arena = memory;
    emit_text = emit;
    head = NULL;
    tail = NULL;
    branch_count = 0;
    scope_count = 0;
    LoadSourceLines(input, offset);
    FlushText();
    ShaderParserNodePc *result = head;
    arena = NULL;
    emit_text = 0;
    return result;
}
