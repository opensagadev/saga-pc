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
    header.third = header.first = header.second = -1;
    header.font_size = 12;
    strcat(header.label, "Arial");
    header.unknown_colour = 0xff;
    header.face_colour = 0xff4c5844;
    header.high_colour = 0xff889180;
    header.low_colour = 0xff2d3128;
    header.inner_colour = 0xff3e4637;
}
// Retail builds each position in one shared 16-byte temporary (UpdateGeometry's position) and copies it in.
struct PC_NATIVE_DEBUG_POSITION_PC {
    f32 x, y, z, w;
};
#define DEBUG_POINT_PC(index, px, py)                                                                                  \
    do {                                                                                                               \
        position.x = (px);                                                                                             \
        position.y = (py);                                                                                             \
        position.z = 0.5f;                                                                                             \
        position.w = 1.0f;                                                                                             \
        *(PC_NATIVE_DEBUG_POSITION_PC *)&vertices[index].x = position;                                                 \
    } while (0)
// Retail rereads each edge for every vertex.
#define DEBUG_EDGE_PC(edge) (*(volatile f32 *)&(edge))
#define DEBUG_RECT_PC(start, l, t, r, b)                                                                               \
    DEBUG_POINT_PC(start, l, t);                                                                                       \
    DEBUG_POINT_PC(start + 1, r, t);                                                                                   \
    DEBUG_POINT_PC(start + 2, r, b);                                                                                   \
    DEBUG_POINT_PC(start + 3, l, t);                                                                                   \
    DEBUG_POINT_PC(start + 4, r, b);                                                                                   \
    DEBUG_POINT_PC(start + 5, l, b)

void PC_NATIVE_DEBUG_STATE_PC::UpdateGeometry() {
    PC_NATIVE_DEBUG_POSITION_PC position;
    DEBUG_RECT_PC(0, DEBUG_EDGE_PC(left) + 1, DEBUG_EDGE_PC(top) + 1, DEBUG_EDGE_PC(right), DEBUG_EDGE_PC(bottom));
    DEBUG_RECT_PC(6, DEBUG_EDGE_PC(left), DEBUG_EDGE_PC(top), DEBUG_EDGE_PC(right) - 1, DEBUG_EDGE_PC(bottom) - 1);
    DEBUG_RECT_PC(12, DEBUG_EDGE_PC(left) + 1, DEBUG_EDGE_PC(top) + 1, DEBUG_EDGE_PC(right) - 1,
                  DEBUG_EDGE_PC(bottom) - 1);
    for (i32 i = 0; i < 6; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 6; i < 12; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 12; i < 18; i++)
        vertices[i].colour = current_header->face_colour;
    DEBUG_RECT_PC(18, DEBUG_EDGE_PC(left) + 5, DEBUG_EDGE_PC(top) + 15, (DEBUG_EDGE_PC(right) - 5) - 1,
                  (DEBUG_EDGE_PC(bottom) - 40) - 1);
    DEBUG_RECT_PC(24, (DEBUG_EDGE_PC(left) + 5) + 1, (DEBUG_EDGE_PC(top) + 15) + 1, DEBUG_EDGE_PC(right) - 5,
                  DEBUG_EDGE_PC(bottom) - 40);
    DEBUG_RECT_PC(30, (DEBUG_EDGE_PC(left) + 5) + 1, (DEBUG_EDGE_PC(top) + 15) + 1, (DEBUG_EDGE_PC(right) - 5) - 1,
                  (DEBUG_EDGE_PC(bottom) - 40) - 1);
    for (i32 i = 18; i < 24; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 24; i < 30; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 30; i < 36; i++)
        vertices[i].colour = current_header->inner_colour;
    DEBUG_RECT_PC(36, (DEBUG_EDGE_PC(left) + 5) + 1, (DEBUG_EDGE_PC(bottom) - 35) + 1, DEBUG_EDGE_PC(right) - 5,
                  DEBUG_EDGE_PC(bottom) - 5);
    DEBUG_RECT_PC(42, DEBUG_EDGE_PC(left) + 5, DEBUG_EDGE_PC(bottom) - 35, (DEBUG_EDGE_PC(right) - 5) - 1,
                  (DEBUG_EDGE_PC(bottom) - 5) - 1);
    DEBUG_RECT_PC(48, (DEBUG_EDGE_PC(left) + 5) + 1, (DEBUG_EDGE_PC(bottom) - 35) + 1, (DEBUG_EDGE_PC(right) - 5) - 1,
                  (DEBUG_EDGE_PC(bottom) - 5) - 1);
    for (i32 i = 36; i < 42; i++)
        vertices[i].colour = current_header->high_colour;
    for (i32 i = 42; i < 48; i++)
        vertices[i].colour = current_header->low_colour;
    for (i32 i = 48; i < 54; i++)
        vertices[i].colour = current_header->inner_colour;
}
#undef DEBUG_RECT_PC
#undef DEBUG_POINT_PC
#undef DEBUG_EDGE_PC
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
