#include "gamelib/util/CRC16.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuerror_gen.h"
#include <stdarg.h>
#include <stdio.h>

void NuSevereWarning(const char *format, ...) {
    va_list args;
    char *message;
    sprintf(PCWarningOutputBuffer, " ");
    va_start(args, format);
    vsprintf(PCWarningFormatBuffer, format, args);
    va_end(args);
    NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
    NuStrCat(PCWarningOutputBuffer, "\n");
    if (PCWarningHandler != NULL) {
        PCWarningHandler(PCWarningOutputBuffer);
    }
    PCNativeDebugPrint(&PCNativeWarningDebugState, "%s", PCWarningOutputBuffer);
    message = PCWarningOutputBuffer;
    while (*message != '\0' && bHaveErr < 1023) {
        PCWarningMessages[bHaveErr++] = *message++;
    }
    PCWarningMessages[bHaveErr++] = '\0';
}

i32 NuHasError(void) {
    return bHaveErr;
}

void NuClearError(void) {
    bHaveErr = 0;
}

char *NuGetErrN(i32 entry) {
    i32 offset = 0;
    i32 current = 0;
    while (current < entry && offset < bHaveErr) {
        if (PCWarningMessages[offset++] == '\0') {
            ++current;
        }
    }
    return PCWarningMessages + offset;
}

char *NuErrorGetWarnings(void) {
    return bHaveErr ? PCWarningMessages : NULL;
}
