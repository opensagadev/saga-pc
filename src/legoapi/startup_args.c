#include "saga.h"

void __cdecl StartupParseArguments(i32 count, char **arguments) {
    while (count > 0) {
        StartupParseArgument(count, arguments);
        count--;
        arguments++;
    }

    StartupGameFlag0 = 0;
    StartupGameFlag1 = 1;
}
