#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

void __cdecl NuSevereWarning(const char *format, ...) {
    va_list args;
    char *message;
    i32 index;
    sprintf(PCWarningOutputBuffer, PCWarningPrefix);
    va_start(args, format);
    vsprintf(PCWarningFormatBuffer, format, args);
    va_end(args);
    NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
    NuStrCat(PCWarningOutputBuffer, PCWarningSuffix);
    if (PCWarningHandler != NULL)
        PCWarningHandler(PCWarningOutputBuffer);
    PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, PCWarningOutputBuffer);
    index = bHaveErr;
    message = PCWarningOutputBuffer;
    while (*message != '\0' && index < 1023) {
        PCWarningMessages[index] = *message;
        ++message;
        ++index;
    }
    PCWarningMessages[index++] = '\0';
    bHaveErr = index;
}

i32 __cdecl NuHasError(void) {
    return bHaveErr;
}

void __cdecl NuClearError(void) {
    bHaveErr = 0;
}
