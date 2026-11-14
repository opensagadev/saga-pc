#include "saga.h"

extern char TextInputKeyNameTextPc7516c0[1];

const char *TextInputButtonGlyphsPc[12] = {
    "\xd4\xb1",
    "\xd4\xb2",
    "\xd4\xb3",
    "\xd4\xb4",
    "\xd4\xb5",
    "\xd4\xb6",
    "\xd4\xbd",
    "\xd4\xbe",
    TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0,
    "\xd4\xb8",
    "\xd4\xb7",
};

const char *TextInputAxisGlyphsPc[4] = {
    "\xd4\xbc",
    "\xd4\xba",
    "\xd4\xb9",
    "\xd4\xbb",
};

void TextInputGlyphNamePc(i32 code, char *destination) {
    if ((u32)code <= 11U) {
        NuStrCpy(destination, (char *)TextInputButtonGlyphsPc[code]);
    } else if ((u32)code - 128U <= 3U) {
        NuStrCpy(destination, (char *)TextInputAxisGlyphsPc[(u32)code - 128U]);
    } else {
        *destination = 0;
    }
}

void PcInput_SetStringUCGfxRemapFn(void (*callback)(i32, char *)) {
    TextInputNameCallbackPc = callback;
}
