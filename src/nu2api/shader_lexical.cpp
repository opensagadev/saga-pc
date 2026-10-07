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
