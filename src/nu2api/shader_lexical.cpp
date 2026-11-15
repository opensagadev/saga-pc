#define SAGA_SHADER_PARSER_TYPES
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/shader_parser.h"
static u8 ShaderLexicalClasses[256] = {0};

extern "C" void ShaderLexicalClassesInit(void) {
    if (ShaderLexicalClasses['A'] != 0) {
        return;
    }
    ShaderLexicalClasses['_'] = 1;
    ShaderLexicalClasses['Z'] = 1;
    ShaderLexicalClasses['z'] = 1;
    ShaderLexicalClasses['Y'] = 1;
    ShaderLexicalClasses['y'] = 1;
    ShaderLexicalClasses['X'] = 1;
    ShaderLexicalClasses['x'] = 1;
    ShaderLexicalClasses['W'] = 1;
    ShaderLexicalClasses['w'] = 1;
    ShaderLexicalClasses['V'] = 1;
    ShaderLexicalClasses['v'] = 1;
    ShaderLexicalClasses['U'] = 1;
    ShaderLexicalClasses['u'] = 1;
    ShaderLexicalClasses['T'] = 1;
    ShaderLexicalClasses['t'] = 1;
    ShaderLexicalClasses['S'] = 1;
    ShaderLexicalClasses['s'] = 1;
    ShaderLexicalClasses['R'] = 1;
    ShaderLexicalClasses['r'] = 1;
    ShaderLexicalClasses['Q'] = 1;
    ShaderLexicalClasses['q'] = 1;
    ShaderLexicalClasses['P'] = 1;
    ShaderLexicalClasses['p'] = 1;
    ShaderLexicalClasses['O'] = 1;
    ShaderLexicalClasses['o'] = 1;
    ShaderLexicalClasses['N'] = 1;
    ShaderLexicalClasses['n'] = 1;
    ShaderLexicalClasses['M'] = 1;
    ShaderLexicalClasses['m'] = 1;
    ShaderLexicalClasses['L'] = 1;
    ShaderLexicalClasses['l'] = 1;
    ShaderLexicalClasses['K'] = 1;
    ShaderLexicalClasses['k'] = 1;
    ShaderLexicalClasses['J'] = 1;
    ShaderLexicalClasses['j'] = 1;
    ShaderLexicalClasses['I'] = 1;
    ShaderLexicalClasses['i'] = 1;
    ShaderLexicalClasses['H'] = 1;
    ShaderLexicalClasses['h'] = 1;
    ShaderLexicalClasses['G'] = 1;
    ShaderLexicalClasses['g'] = 1;
    ShaderLexicalClasses['F'] = 1;
    ShaderLexicalClasses['f'] = 1;
    ShaderLexicalClasses['E'] = 1;
    ShaderLexicalClasses['e'] = 1;
    ShaderLexicalClasses['D'] = 1;
    ShaderLexicalClasses['d'] = 1;
    ShaderLexicalClasses['C'] = 1;
    ShaderLexicalClasses['c'] = 1;
    ShaderLexicalClasses['B'] = 1;
    ShaderLexicalClasses['b'] = 1;
    ShaderLexicalClasses['A'] = 1;
    ShaderLexicalClasses['a'] = 1;
    ShaderLexicalClasses['9'] = 2;
    ShaderLexicalClasses['8'] = 2;
    ShaderLexicalClasses['7'] = 2;
    ShaderLexicalClasses['6'] = 2;
    ShaderLexicalClasses['5'] = 2;
    ShaderLexicalClasses['4'] = 2;
    ShaderLexicalClasses['3'] = 2;
    ShaderLexicalClasses['2'] = 2;
    ShaderLexicalClasses['1'] = 2;
    ShaderLexicalClasses['0'] = 2;
    ShaderLexicalClasses['|'] = 4;
    ShaderLexicalClasses['&'] = 4;
    ShaderLexicalClasses['('] = 4;
    ShaderLexicalClasses['>'] = 4;
    ShaderLexicalClasses['<'] = 4;
    ShaderLexicalClasses['='] = 4;
    ShaderLexicalClasses['!'] = 4;
    ShaderLexicalClasses['-'] = 4;
    ShaderLexicalClasses['+'] = 4;
    ShaderLexicalClasses[0x80] = 4;
    ShaderLexicalClasses['\t'] = 8;
    ShaderLexicalClasses[' '] = 8;
    ShaderLexicalClasses['\n'] = 16;
    ShaderLexicalClasses['\r'] = 16;
}

// This method never reads its receiver. No receiver storage or extent is inferred here.
struct ShaderLexicalTokenPredicate {
    i32 IsSpecialToken(const char *);
};

i32 ShaderLexicalTokenPredicate::IsSpecialToken(const char *token) {
    return (ShaderLexicalClasses[(u8)token[0]] & 4) != 0 || strcmp(token, "defined") == 0;
}

bool ShaderCompilerDictionary::IsNumericToken(const char *text) {
    if ((ShaderLexicalClasses[(u8)*text] & 2) == 0) {
        return false;
    }
    while (*++text != 0) {
        if ((ShaderLexicalClasses[(u8)*text] & 2) == 0) {
            return false;
        }
    }
    return true;
}

// MATCH-EXCEPTION: the original calls Equals everywhere; the pointer-walking body is small enough
// that VC8 would otherwise inline it into ShaderDeclarationSemantic.
__declspec(noinline) bool ShaderLexicalSlice::Equals(const char *text) {
    const char *source = begin;
    for (i32 i = 0; i < length; ++i) {
        if (*source != *text) {
            return false;
        }
        ++source;
        ++text;
    }
    return *text == 0;
}

bool ShaderLexicalSlice::StartsWith(const char *text) {
    const char *source = begin;
    while (*text != 0) {
        if (*text++ != *source++) {
            return false;
        }
    }
    return true;
}

char *ShaderLexicalSlice::HeapCopy() {
    char *result = reinterpret_cast<char *>(::operator new(length + 1));
    memcpy(result, begin, length);
    result[length] = 0;
    return result;
}

// noinline: the original calls ArenaCopy out of line from ShaderCompilerParser::ParseExpression; VC8 would inline it
// there.
__declspec(noinline) char *ShaderLexicalSlice::ArenaCopy(VirtualStackAllocator *arena) {
    char *result = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += length + 1;
    memcpy(result, begin, length);
    result[length] = 0;
    return result;
}

i32 ShaderLexicalSlice::AsInteger(i32 skipped_prefix) {
    i32 result = 0;
    const char *p = begin + length - 1;
    i32 multiplier = 1;
    for (i32 i = 0; i < length - skipped_prefix; ++i) {
        result += (*p-- - '0') * multiplier;
        multiplier *= 10;
    }
    return result;
}

bool ShaderLexicalReader::Next(ShaderCompilerBufferAllocator *input, i32 *cursor, ShaderLexicalSlice *slice) {
    const char *p = reinterpret_cast<const char *>(input->data) + *cursor;
    while (*cursor < input->length && ShaderLexicalClasses[(u8)*p] == 8) {
        ++*cursor;
        ++p;
    }
    if (*cursor >= input->length) {
        return false;
    }
    slice->begin = p;
    slice->length = 0;
    i32 first = *cursor;
    if ((ShaderLexicalClasses[(u8) reinterpret_cast<const char *>(input->data)[first]] & 1) != 0) {
        do {
            ++*cursor;
            ++p;
        } while (*cursor < input->length && (ShaderLexicalClasses[(u8)*p] & 3) != 0);
        slice->length = *cursor - first;
        return true;
    }
    u8 token = (u8)*p;
    if ((ShaderLexicalClasses[token] & 2) != 0) {
        do {
            ++*cursor;
            ++p;
        } while (*cursor < input->length && (ShaderLexicalClasses[(u8)*p] & 2) != 0);
        slice->length = *cursor - first;
        return true;
    }
    if (token == '(') {
        *cursor = first + 1;
        slice->length = 1;
        return true;
    }
    if (token == ')') {
        *cursor = first + 1;
        slice->length = 1;
        return true;
    }
    if (token == '&') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == token) {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == '|') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == token) {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == '<') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == '=') {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == '>') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == '=') {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == '=') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == '=') {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == ',') {
        *cursor = first + 1;
        slice->length = 1;
        return true;
    }
    if (token == '!') {
        *cursor = first + 1;
        if (*cursor < input->length && (u8)p[1] == '=') {
            ++*cursor;
        }
        slice->length = *cursor - first;
        return true;
    }
    if (token == '"') {
        do {
            ++*cursor;
            ++p;
        } while (*cursor < input->length && *p != '"');
        slice->length = *cursor - first;
        return true;
    }
    if (token == '*' || token == '[' || token == ']') {
        *cursor = first + 1;
        slice->length = 1;
        return true;
    }
    return false;
}

bool ShaderLexicalSlice::fun_0074b080(const char *text) {
    return !Equals(text);
}

ShaderConditionNode::ShaderConditionNode(const char *token, ShaderConditionNode *left, ShaderConditionNode *right)
    : left(left), right(right), token(token) {
}

// noinline: the original calls AppendOperand out of line from ShaderCompilerParser::ParseExpression; VC8 would inline
// it there.
__declspec(noinline) ShaderConditionNode *ShaderConditionNode::AppendOperand(ShaderConditionNode *operand) {
    if (left != NULL) {
        if (right != NULL) {
            return NULL;
        }
        right = operand;
        return operand;
    }
    left = operand;
    return operand;
}

i32 ShaderConditionPrecedence(const char *token) {
    if (strcmp(token, "&&") == 0) {
        return 1;
    }
    if (strcmp(token, "||") == 0) {
        return 1;
    }
    if (strcmp(token, "==") == 0) {
        return 2;
    }
    if (strcmp(token, "<=") == 0) {
        return 2;
    }
    if (strcmp(token, ">=") == 0) {
        return 2;
    }
    if (strcmp(token, "<") == 0) {
        return 2;
    }
    if (strcmp(token, ">") == 0) {
        return 2;
    }
    if (strcmp(token, "!=") == 0) {
        return 2;
    }
    if (strcmp(token, "!") == 0) {
        return 3;
    }
    if (strcmp(token, "defined") == 0) {
        return 4;
    }
    if (strcmp(token, "\x80") == 0) {
        return 10;
    }
    return 1000;
}

ShaderConditionNode *ShaderCompilerParser::ParseExpression(ShaderCompilerBufferAllocator *input, i32 *cursor) {
    ShaderConditionNode *current = NULL;
    ShaderConditionNode *stack[100];
    i32 count = 0;
    ShaderLexicalSlice slice = {NULL, 0};
    ShaderLexicalReader *reader = reinterpret_cast<ShaderLexicalReader *>(&lexical_state);
    while (reader->Next(input, cursor, &slice)) {
        char *token = slice.ArenaCopy(arena);
        ShaderConditionNode *memory = reinterpret_cast<ShaderConditionNode *>(arena->cursor);
        ShaderConditionNode *node;
        arena->cursor += sizeof(ShaderConditionNode);
        if (memory != NULL) {
            memory->left = NULL;
            memory->right = NULL;
            memory->token = token;
            node = memory;
        } else {
            node = NULL;
        }
        if (current == NULL) {
            current = node;
            continue;
        }
        bool append = false;
        if (strcmp(token, "(") == 0) {
            append = true;
        } else if (strcmp(token, ")") == 0) {
            do {
                if (strcmp(current->token, "(") == 0) {
                    break;
                }
                if (count == 0) {
                    current = NULL;
                    break;
                }
                current = stack[--count];
            } while (current != NULL);
            const_cast<char *>(current->token)[0] = '\x80';
        } else if ((ShaderLexicalClasses[(u8)token[0]] & 4) != 0 || strcmp(token, "defined") == 0) {
            if (strcmp(current->token, "(") == 0) {
                stack[count++] = current;
            } else if (((ShaderLexicalClasses[(u8)current->token[0]] & 4) != 0 || strcmp(current->token, "defined") == 0) &&
                       ShaderConditionPrecedence(token) > ShaderConditionPrecedence(current->token)) {
                stack[count++] = current;
                current = current->AppendOperand(node);
            } else {
                while (count != 0) {
                    ShaderConditionNode *parent = stack[count - 1];
                    if (strcmp(parent->token, "(") == 0) {
                        node->left = current;
                        parent->left = node;
                        current = node;
                        goto next;
                    }
                    if (ShaderConditionPrecedence(token) > ShaderConditionPrecedence(parent->token)) {
                        node->left = current;
                        parent->right = node;
                        current = node;
                        goto next;
                    }
                    current = stack[--count];
                }
                node->left = current;
                current = node;
            }
        } else if (strcmp(current->token, "(") == 0 || (ShaderLexicalClasses[(u8)current->token[0]] & 4) != 0 ||
                   strcmp(current->token, "defined") == 0) {
            append = true;
        }
        if (append) {
            stack[count++] = current;
            if (current->left == NULL) {
                current->left = node;
                current = node;
            } else if (current->right == NULL) {
                current->right = node;
                current = node;
            } else {
                current = NULL;
            }
        }
    next:;
    }
    return stack[0];
}

extern "C" u32 ShaderDeclarationFlags(u8 shared, u8 vertex, u8 pixel) {
    return (shared ? 0x4000u : 0u) | (vertex ? 0x2000u : 0u) | (pixel ? 0x1000u : 0u) | 0x20u;
}

extern "C" i32 ShaderDeclarationSemantic(ShaderLexicalSlice *slice) {
    if (slice->Equals("default")) {
        return SHADER_SEMANTIC_DEFAULT;
    }
    if (slice->Equals("strict")) {
        return SHADER_SEMANTIC_TEXCOORDX;
    }
    if (slice->Equals("semantic_default")) {
        return SHADER_SEMANTIC_DEFAULT;
    }
    if (slice->Equals("semantic_position")) {
        return SHADER_SEMANTIC_POSITION;
    }
    if (slice->Equals("semantic_position1")) {
        return SHADER_SEMANTIC_POSITION1;
    }
    if (slice->Equals("semantic_vpos")) {
        return SHADER_SEMANTIC_VPOS;
    }
    if (slice->Equals("semantic_normal")) {
        return SHADER_SEMANTIC_NORMAL;
    }
    if (slice->Equals("semantic_tangent")) {
        return SHADER_SEMANTIC_TANGENT;
    }
    if (slice->Equals("semantic_binormal")) {
        return SHADER_SEMANTIC_BINORMAL;
    }
    if (slice->Equals("semantic_bitangent")) {
        return SHADER_SEMANTIC_BINORMAL;
    }
    if (slice->Equals("semantic_tangent1")) {
        return SHADER_SEMANTIC_TANGENT1;
    }
    if (slice->Equals("semantic_colorx")) {
        return SHADER_SEMANTIC_COLORX;
    }
    if (slice->Equals("semantic_color0")) {
        return SHADER_SEMANTIC_COLOR0;
    }
    if (slice->Equals("semantic_color1")) {
        return SHADER_SEMANTIC_COLOR1;
    }
    if (slice->Equals("semantic_fog")) {
        return SHADER_SEMANTIC_FOG;
    }
    if (slice->Equals("semantic_texcoordx")) {
        return SHADER_SEMANTIC_TEXCOORDX;
    }
    if (slice->Equals("semantic_texcoord0")) {
        return SHADER_SEMANTIC_TEXCOORD0;
    }
    if (slice->Equals("semantic_texcoord1")) {
        return SHADER_SEMANTIC_TEXCOORD1;
    }
    if (slice->Equals("semantic_texcoord2")) {
        return SHADER_SEMANTIC_TEXCOORD2;
    }
    if (slice->Equals("semantic_texcoord3")) {
        return SHADER_SEMANTIC_TEXCOORD3;
    }
    if (slice->Equals("semantic_texcoord4")) {
        return SHADER_SEMANTIC_TEXCOORD4;
    }
    if (slice->Equals("semantic_texcoord5")) {
        return SHADER_SEMANTIC_TEXCOORD5;
    }
    if (slice->Equals("semantic_texcoord6")) {
        return SHADER_SEMANTIC_TEXCOORD6;
    }
    if (slice->Equals("semantic_texcoord7")) {
        return SHADER_SEMANTIC_TEXCOORD7;
    }
    if (slice->Equals("semantic_blendIndices0")) {
        return SHADER_SEMANTIC_BLENDINDICES0;
    }
    if (slice->Equals("semantic_blendWeight0")) {
        return SHADER_SEMANTIC_BLENDWEIGHT0;
    }
    if (slice->Equals("semantic_index")) {
        return SHADER_SEMANTIC_INDEX;
    }
    return SHADER_SEMANTIC_CUSTOM;
}

extern "C" bool fun_0074bd00(const char *left, const char *right) {
    return ShaderConditionPrecedence(left) > ShaderConditionPrecedence(right);
}

/* The accessors below have no caller. */
i32 ShaderLexicalReader::IsWordChar(u8 c) {
    return ShaderLexicalClasses[c] & 1;
}

i32 ShaderLexicalReader::IsTokenChar(u8 c) {
    return (ShaderLexicalClasses[c] & 3) != 0;
}

const char *ShaderLexicalSlice::GetBegin() {
    return begin;
}

const char *ShaderLexicalSlice::At(i32 offset) {
    return begin + offset;
}

void ShaderCompilerBufferAllocator::AppendSlice(const ShaderLexicalSlice &slice) {
    AppendBytes(slice.begin, slice.length);
}

ShaderCompilerBufferAllocator &ShaderCompilerBufferAllocator::operator<<(const ShaderLexicalSlice &slice) {
    AppendBytes(slice.begin, slice.length);
    return *this;
}
