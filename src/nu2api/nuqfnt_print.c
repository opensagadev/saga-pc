#include "saga.h"
void __cdecl NuQFntPrintW(VUFNT *font, u16 *text) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        NuQFntPrintCharW(font, text, NuQFntMode);
    }
}
