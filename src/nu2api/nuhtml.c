#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nucore/nustring.h"
#include <stdarg.h>
#include <stdio.h>

char NuHtmlUnknownText[8] = "unknown";

void NuHtmlWrite(char *text, ...) {
    va_list args;
    i32 length;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursor, text, args);
    length = NuStrLen(PCDisplayCaptureCursor);
    PCDisplayCaptureCursor += length;
    if (PCDisplayCaptureCursor > PCDisplayCaptureFlushLimit) {
        NuFileWriteString(NuHtmlFile, PCDisplayCaptureBuffer);
        PCDisplayCaptureCursor = PCDisplayCaptureBuffer;
        PCDisplayCaptureFlushLimit = PCDisplayCaptureBuffer + 0xc00;
    }
}

void NuHtmlEnd(void) {
    if (PCDisplayCaptureFlushLimit != PCDisplayCaptureCursor) {
        NuFileWriteString(NuHtmlFile, PCDisplayCaptureBuffer);
        PCDisplayCaptureCursor = PCDisplayCaptureBuffer;
        PCDisplayCaptureFlushLimit = PCDisplayCaptureBuffer + 0xc00;
    }
    NuHtmlFile = 0;
}

/* Writes a banner row whose caption is formatted like printf. */
void NuHtmlHeading(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#CFCFE5><tr><td><font face=arial size=+3> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursor, text, args);
    PCDisplayCaptureCursor += NuStrLen(PCDisplayCaptureCursor);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlHeading2(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#DFDFE5><tr><td><font face=arial size=+2> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursor, text, args);
    PCDisplayCaptureCursor += NuStrLen(PCDisplayCaptureCursor);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlHeading3(char *text, ...) {
    va_list args;
    if (text == NULL || *text == '\0') {
        text = NuHtmlUnknownText;
    }
    NuHtmlWrite("<table width=100%c bgcolor=#FFDFE5><tr><td><font face=arial size=+0> ", 0x25);
    va_start(args, text);
    vsprintf(PCDisplayCaptureCursor, text, args);
    PCDisplayCaptureCursor += NuStrLen(PCDisplayCaptureCursor);
    NuHtmlWrite("</font></table>\n");
}

void NuHtmlBanner(void) {
    NuHtmlWrite("<table   width=100%c height=20 bgcolor=#FF0000><tr><td><font face=arial size=+2></font></table>", 0x25);
}
