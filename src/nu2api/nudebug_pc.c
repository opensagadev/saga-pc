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
