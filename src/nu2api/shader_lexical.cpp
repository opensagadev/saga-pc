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

ShaderConditionNodePc *ShaderConditionNodePc::AppendOperand(ShaderConditionNodePc *operand) {
    if (left == NULL) {
        left = operand;
        return operand;
    }
    if (right == NULL) {
        right = operand;
        return operand;
    }
    return NULL;
}

static i32 ShaderConditionPrecedencePc(const char *token) {
    if (strcmp(token, "&&") == 0 || strcmp(token, "||") == 0)
        return 1;
    if (strcmp(token, "==") == 0 || strcmp(token, "<=") == 0 || strcmp(token, ">=") == 0 || strcmp(token, "<") == 0 ||
        strcmp(token, ">") == 0 || strcmp(token, "!=") == 0)
        return 2;
    if (strcmp(token, "!") == 0)
        return 3;
    if (strcmp(token, "defined") == 0)
        return 4;
    return strcmp(token, "\x80") == 0 ? 10 : 1000;
}

ShaderConditionNodePc *ShaderCompilerParserPc::ParseExpression(ShaderCompilerBufferAllocatorPc *input, i32 *cursor) {
    ShaderConditionNodePc *current = NULL;
    ShaderConditionNodePc *stack[100];
    i32 count = 0;
    ShaderLexicalSlicePc slice;
    ShaderLexicalReaderPc *reader = reinterpret_cast<ShaderLexicalReaderPc *>(&lexical_state);
    while (reader->Next(input, cursor, &slice)) {
        char *token = slice.ArenaCopy(arena);
        ShaderConditionNodePc *node = reinterpret_cast<ShaderConditionNodePc *>(arena->cursor);
        arena->cursor += sizeof(ShaderConditionNodePc);
        if (node != NULL) {
            node->left = NULL;
            node->right = NULL;
            node->token = token;
        }
        if (current == NULL) {
            current = node;
        } else if (strcmp(token, "(") == 0) {
            stack[count++] = current;
            current = current->AppendOperand(node);
        } else if (strcmp(token, ")") == 0) {
            while (strcmp(current->token, "(") != 0) {
                if (count == 0) {
                    current = NULL;
                    break;
                }
                current = stack[--count];
                if (current == NULL)
                    break;
            }
            const_cast<char *>(current->token)[0] = '\x80';
        } else if ((ShaderLexicalClassesPc[(u8)token[0]] & 4) != 0 || strcmp(token, "defined") == 0) {
            if (strcmp(current->token, "(") == 0) {
                stack[count++] = current;
            } else if (((ShaderLexicalClassesPc[(u8)current->token[0]] & 4) != 0 ||
                        strcmp(current->token, "defined") == 0) &&
                       ShaderConditionPrecedencePc(token) > ShaderConditionPrecedencePc(current->token)) {
                stack[count++] = current;
                current = current->AppendOperand(node);
            } else {
                ShaderConditionNodePc *parent = NULL;
                while (count != 0) {
                    parent = stack[count - 1];
                    if (strcmp(parent->token, "(") == 0 ||
                        ShaderConditionPrecedencePc(token) > ShaderConditionPrecedencePc(parent->token))
                        break;
                    current = stack[--count];
                }
                node->left = current;
                if (count != 0) {
                    if (strcmp(parent->token, "(") == 0)
                        parent->left = node;
                    else
                        parent->right = node;
                }
                current = node;
            }
        } else if (strcmp(current->token, "(") == 0 || (ShaderLexicalClassesPc[(u8)current->token[0]] & 4) != 0 ||
                   strcmp(current->token, "defined") == 0) {
            stack[count++] = current;
            current = current->AppendOperand(node);
        }
    }
    return stack[0];
}

extern "C" u32 __cdecl ShaderDeclarationFlagsPc(u8 shared, u8 vertex, u8 pixel) {
    return (shared ? 0x4000u : 0u) | (vertex ? 0x2000u : 0u) | (pixel ? 0x1000u : 0u) | 0x20u;
}

extern "C" i32 __cdecl ShaderDeclarationSemanticPc(ShaderLexicalSlicePc *slice) {
    if (slice->Equals("default"))
        return 1;
    if (slice->Equals("strict"))
        return 13;
    if (slice->Equals("semantic_default"))
        return 1;
    if (slice->Equals("semantic_position"))
        return 2;
    if (slice->Equals("semantic_position1"))
        return 3;
    if (slice->Equals("semantic_vpos"))
        return 4;
    if (slice->Equals("semantic_normal"))
        return 5;
    if (slice->Equals("semantic_tangent"))
        return 6;
    if (slice->Equals("semantic_binormal"))
        return 7;
    if (slice->Equals("semantic_bitangent"))
        return 7;
    if (slice->Equals("semantic_tangent1"))
        return 8;
    if (slice->Equals("semantic_colorx"))
        return 9;
    if (slice->Equals("semantic_color0"))
        return 10;
    if (slice->Equals("semantic_color1"))
        return 11;
    if (slice->Equals("semantic_fog"))
        return 12;
    if (slice->Equals("semantic_texcoordx"))
        return 13;
    if (slice->Equals("semantic_texcoord0"))
        return 14;
    if (slice->Equals("semantic_texcoord1"))
        return 15;
    if (slice->Equals("semantic_texcoord2"))
        return 16;
    if (slice->Equals("semantic_texcoord3"))
        return 17;
    if (slice->Equals("semantic_texcoord4"))
        return 18;
    if (slice->Equals("semantic_texcoord5"))
        return 19;
    if (slice->Equals("semantic_texcoord6"))
        return 20;
    if (slice->Equals("semantic_texcoord7"))
        return 21;
    if (slice->Equals("semantic_blendIndices0"))
        return 22;
    if (slice->Equals("semantic_blendWeight0"))
        return 23;
    if (slice->Equals("semantic_index"))
        return 24;
    return 0;
}
