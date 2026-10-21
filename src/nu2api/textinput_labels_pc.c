#include "saga.h"

extern char TextInputKeyNameTextPc7516c0[1];

#pragma const_seg(".rdata$TextInputGlyphBBPc")
const char TextInputGlyphBBPc[3] = "\xd4\xbb";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB9Pc")
const char TextInputGlyphB9Pc[3] = "\xd4\xb9";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphBAPc")
const char TextInputGlyphBAPc[3] = "\xd4\xba";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphBCPc")
const char TextInputGlyphBCPc[3] = "\xd4\xbc";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB7Pc")
const char TextInputGlyphB7Pc[3] = "\xd4\xb7";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB8Pc")
const char TextInputGlyphB8Pc[3] = "\xd4\xb8";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphBEPc")
const char TextInputGlyphBEPc[3] = "\xd4\xbe";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphBDPc")
const char TextInputGlyphBDPc[3] = "\xd4\xbd";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB6Pc")
const char TextInputGlyphB6Pc[3] = "\xd4\xb6";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB5Pc")
const char TextInputGlyphB5Pc[3] = "\xd4\xb5";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB4Pc")
const char TextInputGlyphB4Pc[3] = "\xd4\xb4";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB3Pc")
const char TextInputGlyphB3Pc[3] = "\xd4\xb3";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB2Pc")
const char TextInputGlyphB2Pc[3] = "\xd4\xb2";
#pragma const_seg()
#pragma const_seg(".rdata$TextInputGlyphB1Pc")
const char TextInputGlyphB1Pc[3] = "\xd4\xb1";
#pragma const_seg()

const char *TextInputButtonGlyphsPc[12] = {
    TextInputGlyphB1Pc,           TextInputGlyphB2Pc,           TextInputGlyphB3Pc, TextInputGlyphB4Pc,
    TextInputGlyphB5Pc,           TextInputGlyphB6Pc,           TextInputGlyphBDPc, TextInputGlyphBEPc,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputGlyphB8Pc, TextInputGlyphB7Pc,
};

const char *TextInputAxisGlyphsPc[4] = {
    TextInputGlyphBCPc,
    TextInputGlyphBAPc,
    TextInputGlyphB9Pc,
    TextInputGlyphBBPc,
};

void __cdecl TextInputGlyphNamePc(i32 code, char *destination) {
    if ((u32)code <= 11U) {
        NuStrCpy(destination, (char *)TextInputButtonGlyphsPc[code]);
    } else if ((u32)code - 128U <= 3U) {
        NuStrCpy(destination, (char *)TextInputAxisGlyphsPc[(u32)code - 128U]);
    } else {
        *destination = 0;
    }
}

void __cdecl TextInputSetNameCallbackPc(void(__cdecl *callback)(i32, char *)) {
    TextInputNameCallbackPc = callback;
}
