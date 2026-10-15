#include "saga.h"
#include <string.h>

void __cdecl ResetRadios(void) {
    memset(RadioEntriesPc, 0, sizeof(RadioEntriesPc));
    radios_playing = 0;
}
