#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

void __cdecl PCNativeDebugPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list args;
    if (format != NULL) {
        va_start(args, format);
        vsprintf(PCNativeDebugFormatBuffer, format, args);
        va_end(args);
    }
    sprintf(PCNativeDebugOutputBuffer, PCNativeDebugPrefix, PCNativeDebugFormatBuffer);
    if (state->enabled != 0)
        OutputDebugStringA(PCNativeDebugOutputBuffer);
}

#include <signal.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
void __cdecl PCGraphicsFatalDiagnostic(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer, "\n%s\n", PCNativeDebugFormatBuffer);
    MessageBoxA((HWND)PCInputInstance_candidate, PCNativeDebugOutputBuffer, "Fatal Error!", 0x10);
    raise(22);
    _exit(3);
}

#include "saga.h"
#include <string.h>
#include <stdio.h>
extern "C" void __cdecl NuFileWriteString(NUFILE file, const char *text);

void PC_NATIVE_DEBUG_STATE_PC::InitializeHeader() {
    header.first = header.second = header.third = -1;
    header.font_size = 12;
    strcat(header.label, "Arial");
    header.unknown_colour = 0xff;
    header.face_colour = 0xff4c5844;
    header.high_colour = 0xff889180;
    header.low_colour = 0xff2d3128;
    header.inner_colour = 0xff3e4637;
}
#define DEBUG_RECT_PC(start, l, t, r, b)                                                                               \
    vertices[start].x = (f32)(l);                                                                                      \
    vertices[start].y = (f32)(t);                                                                                      \
    vertices[start].z = 0.5f;                                                                                          \
    vertices[start].w = 1.0f;                                                                                          \
    vertices[start + 1].x = (f32)(r);                                                                                  \
    vertices[start + 1].y = (f32)(t);                                                                                  \
    vertices[start + 1].z = 0.5f;                                                                                      \
    vertices[start + 1].w = 1.0f;                                                                                      \
    vertices[start + 2].x = (f32)(r);                                                                                  \
    vertices[start + 2].y = (f32)(b);                                                                                  \
    vertices[start + 2].z = 0.5f;                                                                                      \
    vertices[start + 2].w = 1.0f;                                                                                      \
    vertices[start + 3].x = (f32)(l);                                                                                  \
    vertices[start + 3].y = (f32)(t);                                                                                  \
    vertices[start + 3].z = 0.5f;                                                                                      \
    vertices[start + 3].w = 1.0f;                                                                                      \
    vertices[start + 4].x = (f32)(r);                                                                                  \
    vertices[start + 4].y = (f32)(b);                                                                                  \
    vertices[start + 4].z = 0.5f;                                                                                      \
    vertices[start + 4].w = 1.0f;                                                                                      \
    vertices[start + 5].x = (f32)(l);                                                                                  \
    vertices[start + 5].y = (f32)(b);                                                                                  \
    vertices[start + 5].z = 0.5f;                                                                                      \
    vertices[start + 5].w = 1.0f

void PC_NATIVE_DEBUG_STATE_PC::UpdateGeometry() {
    DEBUG_RECT_PC(0, (f64)left + 1, (f64)top + 1, right, bottom);
    DEBUG_RECT_PC(6, left, top, (f64)right - 1, (f64)bottom - 1);
    DEBUG_RECT_PC(12, (f64)left + 1, (f64)top + 1, (f64)right - 1, (f64)bottom - 1);
    for (i32 i = 0; i < 6; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 6; i < 12; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 12; i < 18; i++)
        vertices[i].colour = current_header->face_colour;
    DEBUG_RECT_PC(18, (f64)left + 5, (f64)top + 15, ((f64)right - 5) - 1, ((f64)bottom - 40) - 1);
    DEBUG_RECT_PC(24, ((f64)left + 5) + 1, ((f64)top + 15) + 1, (f64)right - 5, (f64)bottom - 40);
    DEBUG_RECT_PC(30, ((f64)left + 5) + 1, ((f64)top + 15) + 1, ((f64)right - 5) - 1, ((f64)bottom - 40) - 1);
    for (i32 i = 18; i < 24; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 24; i < 30; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 30; i < 36; i++)
        vertices[i].colour = current_header->inner_colour;
    DEBUG_RECT_PC(36, ((f64)left + 5) + 1, ((f64)bottom - 35) + 1, (f64)right - 5, (f64)bottom - 5);
    DEBUG_RECT_PC(42, (f64)left + 5, (f64)bottom - 35, ((f64)right - 5) - 1, ((f64)bottom - 5) - 1);
    DEBUG_RECT_PC(48, ((f64)left + 5) + 1, ((f64)bottom - 35) + 1, ((f64)right - 5) - 1, ((f64)bottom - 5) - 1);
    for (i32 i = 36; i < 42; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 42; i < 48; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 48; i < 54; i++)
        vertices[i].colour = current_header->inner_colour;
}
#undef DEBUG_RECT_PC
void PC_NATIVE_DEBUG_STATE_PC::Initialize() {
    unknown_0 = 1;
    left = top = 100.0f;
    right = 600.0f;
    bottom = 500.0f;
    InitializeHeader();
    current_header = &header;
    UpdateGeometry();
    if (write_log) {
        char path[1024];
        sprintf(path, "Log.txt");
        log_file = NuFileOpen(path, 1);
        if (log_file)
            NuFileWriteString(log_file, "-- Log Started --\n\n");
    }
}
void PC_NATIVE_DEBUG_STATE_PC::Shutdown() {
    if (unknown_0) {
        unknown_0 = 0;
        if (write_log)
            NuFileClose(log_file);
    }
}
