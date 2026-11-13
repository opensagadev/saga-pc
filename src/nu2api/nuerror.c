#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

void __cdecl NuSevereWarning(const char *format, ...) {
    va_list args;
    char *message;
    sprintf(PCWarningOutputBuffer, PCWarningPrefix);
    va_start(args, format);
    vsprintf(PCWarningFormatBuffer, format, args);
    va_end(args);
    NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
    NuStrCat(PCWarningOutputBuffer, PCWarningSuffix);
    if (PCWarningHandler != NULL)
        PCWarningHandler(PCWarningOutputBuffer);
    PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, PCWarningOutputBuffer);
    message = PCWarningOutputBuffer;
    while (*message != '\0' && bHaveErr < 1023)
        PCWarningMessages[bHaveErr++] = *message++;
    PCWarningMessages[bHaveErr++] = '\0';
}

i32 __cdecl NuHasError(void) {
    return bHaveErr;
}

void __cdecl NuClearError(void) {
    bHaveErr = 0;
}

char *NuGetErrN(i32 entry) {
    i32 offset = 0, current = 0;
    while (current < entry && offset < bHaveErr) {
        if (PCWarningMessages[offset++] == '\0')
            ++current;
    }
    return PCWarningMessages + offset;
}
