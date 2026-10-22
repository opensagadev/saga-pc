#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

extern char NuHtmlUnknownTextPc[8];

void __cdecl NuHtmlWrite(char *text, ...) {
    va_list args;
    i32 length;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownTextPc;
    }
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursorPc, text, args);
    length = NuStrLen(PCDisplayCaptureCursorPc);
    PCDisplayCaptureCursorPc += length;
    if (PCDisplayCaptureCursorPc > nudlist_field_18b0) {
        NuFileWriteString(nudlist_field_988, PCDisplayCaptureBufferPc);
        PCDisplayCaptureCursorPc = PCDisplayCaptureBufferPc;
        nudlist_field_18b0 = PCDisplayCaptureFlushPointPc;
    }
}

void __cdecl NuHtmlFlushPc(void) {
    if (nudlist_field_18b0 != PCDisplayCaptureCursorPc) {
        NuFileWriteString(nudlist_field_988, PCDisplayCaptureBufferPc);
        PCDisplayCaptureCursorPc = PCDisplayCaptureBufferPc;
        nudlist_field_18b0 = PCDisplayCaptureFlushPointPc;
    }
    nudlist_field_988 = 0;
}
