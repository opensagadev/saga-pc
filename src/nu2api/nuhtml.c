#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

extern char NuHtmlUnknownText[8];

void NuHtmlWrite(char *text, ...) {
    va_list args;
    i32 length;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursorPc, text, args);
    length = NuStrLen(PCDisplayCaptureCursorPc);
    PCDisplayCaptureCursorPc += length;
    if (PCDisplayCaptureCursorPc > nudlist_field_18b0) {
        NuFileWriteString(nudlist_field_988, PCDisplayCaptureBuffer);
        PCDisplayCaptureCursorPc = PCDisplayCaptureBuffer;
        nudlist_field_18b0 = PCDisplayCaptureBuffer + 0xc00;
    }
}

void NuHtmlFlushPc(void) {
    if (nudlist_field_18b0 != PCDisplayCaptureCursorPc) {
        NuFileWriteString(nudlist_field_988, PCDisplayCaptureBuffer);
        PCDisplayCaptureCursorPc = PCDisplayCaptureBuffer;
        nudlist_field_18b0 = PCDisplayCaptureBuffer + 0xc00;
    }
    nudlist_field_988 = 0;
}

/* Writes a banner row whose caption is formatted like printf. */
void NuHtmlHeading(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#CFCFE5><tr><td><font face=arial size=+3> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursorPc, text, args);
    PCDisplayCaptureCursorPc += NuStrLen(PCDisplayCaptureCursorPc);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlHeading2(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#DFDFE5><tr><td><font face=arial size=+2> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursorPc, text, args);
    PCDisplayCaptureCursorPc += NuStrLen(PCDisplayCaptureCursorPc);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlHeading3(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#FFDFE5><tr><td><font face=arial size=+0> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursorPc, text, args);
    PCDisplayCaptureCursorPc += NuStrLen(PCDisplayCaptureCursorPc);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlBanner(void) {
    NuHtmlWrite("<table   width=100%c height=20 bgcolor=#FF0000><tr><td><font face=arial size=+2></font></table>", 0x25);
}
