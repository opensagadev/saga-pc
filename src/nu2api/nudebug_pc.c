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
