#include "saga.h"
static u8 ShaderLexicalClassesPc[256];
extern "C" void __cdecl ShaderLexicalClassesInitPc(void) {
    if (ShaderLexicalClassesPc['A'] != 0)
        return;
    ShaderLexicalClassesPc['_'] = 1;
    for (i32 i = 25; i >= 0; --i) {
        ShaderLexicalClassesPc['A' + i] = 1;
        ShaderLexicalClassesPc['a' + i] = 1;
    }
    for (i32 i = 9; i >= 0; --i)
        ShaderLexicalClassesPc['0' + i] = 2;
    ShaderLexicalClassesPc['|'] = 4;
    ShaderLexicalClassesPc['&'] = 4;
    ShaderLexicalClassesPc['('] = 4;
    ShaderLexicalClassesPc['>'] = 4;
    ShaderLexicalClassesPc['<'] = 4;
    ShaderLexicalClassesPc['='] = 4;
    ShaderLexicalClassesPc['!'] = 4;
    ShaderLexicalClassesPc['-'] = 4;
    ShaderLexicalClassesPc['+'] = 4;
    ShaderLexicalClassesPc[0x80] = 4;
    ShaderLexicalClassesPc['\t'] = 8;
    ShaderLexicalClassesPc[' '] = 8;
    ShaderLexicalClassesPc['\n'] = 16;
    ShaderLexicalClassesPc['\r'] = 16;
}
// This method never reads its receiver. No receiver storage or extent is inferred here.
struct ShaderLexicalTokenPredicatePc {
    i32 IsSpecialToken(const char *);
};
i32 ShaderLexicalTokenPredicatePc::IsSpecialToken(const char *token) {
    return (ShaderLexicalClassesPc[(u8)token[0]] & 4) != 0 || strcmp(token, "defined") == 0;
}

bool ShaderCompilerDictionaryPc::IsNumericToken(const char *text) {
    if ((ShaderLexicalClassesPc[(u8)*text] & 2) == 0)
        return false;
    while (*++text != 0)
        if ((ShaderLexicalClassesPc[(u8)*text] & 2) == 0)
            return false;
    return true;
}

void *__cdecl operator new(size_t bytes);
bool ShaderLexicalSlicePc::Equals(const char *text) {
    for (i32 i = 0; i < length; ++i)
        if (begin[i] != text[i])
            return false;
    return text[length > 0 ? length : 0] == 0;
}
bool ShaderLexicalSlicePc::StartsWith(const char *text) {
    const char *source = begin;
    while (*text != 0)
        if (*source++ != *text++)
            return false;
    return true;
}
char *ShaderLexicalSlicePc::HeapCopy() {
    char *result = reinterpret_cast<char *>(::operator new(length + 1));
    memcpy(result, begin, length);
    result[length] = 0;
    return result;
}
char *ShaderLexicalSlicePc::ArenaCopy(VirtualStackAllocator *arena) {
    char *result = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += length + 1;
    memcpy(result, begin, length);
    result[length] = 0;
    return result;
}
i32 ShaderLexicalSlicePc::AsInteger(i32 skipped_prefix) {
    u32 result = 0;
    u32 multiplier = 1;
    const char *p = begin + length - 1;
    for (i32 count = length - skipped_prefix; count > 0; --count) {
        result += (u32)((i32)(signed char)*p-- - '0') * multiplier;
        multiplier *= 10;
    }
    return (i32)result;
}
bool ShaderLexicalReaderPc::Next(ShaderCompilerBufferAllocatorPc *input, i32 *cursor, ShaderLexicalSlicePc *slice) {
    const char *source = reinterpret_cast<const char *>(input->data);
    while (*cursor < input->length && ShaderLexicalClassesPc[(u8)source[*cursor]] == 8)
        ++*cursor;
    if (*cursor >= input->length)
        return false;
    i32 first = *cursor;
    slice->begin = source + first;
    slice->length = 0;
    u8 token = (u8)source[first];
    if ((ShaderLexicalClassesPc[token] & 1) != 0) {
        do {
            ++*cursor;
        } while (*cursor < input->length && (ShaderLexicalClassesPc[(u8)source[*cursor]] & 3) != 0);
    } else if ((ShaderLexicalClassesPc[token] & 2) != 0) {
        do {
            ++*cursor;
        } while (*cursor < input->length && (ShaderLexicalClassesPc[(u8)source[*cursor]] & 2) != 0);
    } else if (token == '(' || token == ')' || token == ',' || token == '*' || token == '[' || token == ']') {
        ++*cursor;
    } else if (token == '&' || token == '|') {
        ++*cursor;
        if (*cursor < input->length && (u8)source[*cursor] == token)
            ++*cursor;
    } else if (token == '<' || token == '>' || token == '=' || token == '!') {
        ++*cursor;
        if (*cursor < input->length && source[*cursor] == '=')
            ++*cursor;
    } else if (token == '"') {
        do {
            ++*cursor;
        } while (*cursor < input->length && source[*cursor] != '"');
    } else {
        return false;
    }
    slice->length = *cursor - first;
    return true;
}
