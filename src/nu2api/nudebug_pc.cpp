#include "saga.h"
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <process.h>
#include <string.h>

extern "C" void NuFileWriteString(NUFILE file, const char *text);

/* Empty hook that receives each runtime error report. PCNativeErrorPrint
 * still calls it, which suggests it was compiled in another unit; here the
 * compiler drops the call to the empty body. */
extern "C" void PCNativeErrorHookPc(char *text) {
    (void)text;
}

PC_NATIVE_DEBUG_STATE_PC *PC_NATIVE_DEBUG_STATE_PC::fun_006dff70() {
    return this;
}

void PCNativeDebugPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list args;
    if (format != NULL) {
        va_start(args, format);
        vsprintf(PCNativeDebugFormatBuffer, format, args);
        va_end(args);
    }
    sprintf(PCNativeDebugOutputBuffer, "NUDEBUG: %s", PCNativeDebugFormatBuffer);
    if (state->enabled != 0) {
        OutputDebugStringA(PCNativeDebugOutputBuffer);
    }
}

void PCGraphicsFatalDiagnostic(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer, "\n%s\n", PCNativeDebugFormatBuffer);
    MessageBoxA(PCInputInstance, PCNativeDebugOutputBuffer, "Fatal Error!", 0x10);
    raise(22);
    _exit(3);
}

extern "C" void PCNativeMessagePrintPc(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    (void)state;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer, "\n%s\n", PCNativeDebugFormatBuffer);
    MessageBoxA(PCInputInstance, PCNativeDebugOutputBuffer, "", 0x30);
}

extern "C" void PCNativeErrorPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer,
            "Important. Do not ignore!\n\n%s\n\n(Your application has generated an error message)\n\n",
            PCNativeDebugFormatBuffer);
    PCNativeErrorHookPc(PCNativeDebugOutputBuffer);
    switch (MessageBoxA(PCInputInstance, PCNativeDebugOutputBuffer, "Nu2Api Runtime Error!", 0x12)) {
        case IDABORT:
            raise(22);
            _exit(3);
        case IDRETRY:
            DebugBreak();
            break;
    }
    sprintf(PCNativeDebugOutputBuffer, "NUERROR: %s\r\n", PCNativeDebugFormatBuffer);
    OutputDebugStringA(PCNativeDebugOutputBuffer);
    if (state->write_log && state->log_file != 0) {
        NuFileWriteString(state->log_file, PCNativeDebugOutputBuffer);
    }
}

extern "C" void PCNativeWarningPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer, "NUWARNING: %s\r\n", PCNativeDebugFormatBuffer);
    OutputDebugStringA(PCNativeDebugOutputBuffer);
    if (state->write_log && state->log_file != 0) {
        NuFileWriteString(state->log_file, PCNativeDebugOutputBuffer);
    }
}

/* Only format the text; the PC build draws nothing. */
extern "C" void PCNativeDebugPrintAtPc(PC_NATIVE_DEBUG_STATE_PC *state, i32 x, i32 y, u32 colour, const char *format,
                                       ...) {
    va_list arguments;
    (void)state;
    (void)x;
    (void)y;
    (void)colour;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
}

extern "C" void PCNativeDebugFormatPc(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...) {
    va_list arguments;
    (void)state;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
}

/* DirectX SDK DxErr9.lib (dxerr9.h). */
extern "C" const char *WINAPI DXGetErrorString9A(HRESULT result);
extern "C" const char *WINAPI DXGetErrorDescription9A(HRESULT result);

extern "C" void PCNativeD3DWarningPrint(PC_NATIVE_DEBUG_STATE_PC *state, HRESULT result, const char *format, ...) {
    va_list arguments;
    if (format != NULL) {
        va_start(arguments, format);
        vsprintf(PCNativeDebugFormatBuffer, format, arguments);
        va_end(arguments);
    }
    sprintf(PCNativeDebugOutputBuffer, "NUD3DWARNING: %s, DirectDescription: %s, %s\r\n", PCNativeDebugFormatBuffer,
            DXGetErrorString9A(result), DXGetErrorDescription9A(result));
    OutputDebugStringA(PCNativeDebugOutputBuffer);
    if (state->write_log && state->log_file != 0) {
        NuFileWriteString(state->log_file, PCNativeDebugOutputBuffer);
    }
}

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

// Builds each position in one shared 16-byte temporary (UpdateGeometry's position) and copies it in.
struct PC_NATIVE_DEBUG_POSITION_PC {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

#define DEBUG_POINT_PC(index, px, py)                                                                                  \
    do {                                                                                                               \
        position.x = (px);                                                                                             \
        position.y = (py);                                                                                             \
        position.z = 0.5f;                                                                                             \
        position.w = 1.0f;                                                                                             \
        *(PC_NATIVE_DEBUG_POSITION_PC *)&vertex[index].x = position;                                                   \
    } while (0)
#define DEBUG_RECT_PC(start, l, t, r, b)                                                                               \
    DEBUG_POINT_PC(start, l, t);                                                                                       \
    DEBUG_POINT_PC(start + 1, r, t);                                                                                   \
    DEBUG_POINT_PC(start + 2, r, b);                                                                                   \
    DEBUG_POINT_PC(start + 3, l, t);                                                                                   \
    DEBUG_POINT_PC(start + 4, r, b);                                                                                   \
    DEBUG_POINT_PC(start + 5, l, b)

void PC_NATIVE_DEBUG_STATE_PC::UpdateGeometry() {
    PC_NATIVE_DEBUG_POSITION_PC position;
    PC_NATIVE_DEBUG_VERTEX_PC *vertex = vertices;
    DEBUG_RECT_PC(0, left + 1, top + 1, right, bottom);
    DEBUG_RECT_PC(6, left, top, right - 1, bottom - 1);
    DEBUG_RECT_PC(12, left + 1, top + 1, right - 1, bottom - 1);
    for (i32 i = 0; i < 6; i++) {
        vertices[i].colour = current_header->low_colour;
    }
    for (i32 i = 6; i < 12; i++) {
        vertices[i].colour = current_header->high_colour;
    }
    for (i32 i = 12; i < 18; i++) {
        vertices[i].colour = current_header->face_colour;
    }
    DEBUG_RECT_PC(18, left + 5, top + 15, (right - 5) - 1, (bottom - 40) - 1);
    DEBUG_RECT_PC(24, (left + 5) + 1, (top + 15) + 1, right - 5, bottom - 40);
    DEBUG_RECT_PC(30, (left + 5) + 1, (top + 15) + 1, (right - 5) - 1, (bottom - 40) - 1);
    for (i32 i = 18; i < 24; i++) {
        vertices[i].colour = current_header->low_colour;
    }
    for (i32 i = 24; i < 30; i++) {
        vertices[i].colour = current_header->high_colour;
    }
    for (i32 i = 30; i < 36; i++) {
        vertices[i].colour = current_header->inner_colour;
    }
    DEBUG_RECT_PC(36, (left + 5) + 1, (bottom - 35) + 1, right - 5, bottom - 5);
    DEBUG_RECT_PC(42, left + 5, bottom - 35, (right - 5) - 1, (bottom - 5) - 1);
    DEBUG_RECT_PC(48, (left + 5) + 1, (bottom - 35) + 1, (right - 5) - 1, (bottom - 5) - 1);
    for (i32 i = 36; i < 42; i++) {
        vertices[i].colour = current_header->high_colour;
    }
    for (i32 i = 42; i < 48; i++) {
        vertices[i].colour = current_header->low_colour;
    }
    for (i32 i = 48; i < 54; i++) {
        vertices[i].colour = current_header->inner_colour;
    }
}

#undef DEBUG_RECT_PC
#undef DEBUG_POINT_PC

PC_NATIVE_DEBUG_STATE_PC::PC_NATIVE_DEBUG_STATE_PC() {
    enabled = 1;
    write_log = 0;
    unknown_004 = 0;
    unknown_008 = 0;
    unknown_00c = 0;
    log_file = 0;
}

PC_NATIVE_DEBUG_STATE_PC::~PC_NATIVE_DEBUG_STATE_PC() {
    Shutdown();
}

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
        if (log_file) {
            NuFileWriteString(log_file, "-- Log Started --\n\n");
        }
    }
}

void PC_NATIVE_DEBUG_STATE_PC::Shutdown() {
    if (unknown_0) {
        unknown_0 = 0;
        if (write_log) {
            NuFileClose(log_file);
        }
    }
}

void PC_NATIVE_DEBUG_STATE_PC::fun_006dffa0(u8 value) {
    enabled = value;
}

extern "C" void fun_006dffb0(void) {
}
