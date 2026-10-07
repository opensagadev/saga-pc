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
