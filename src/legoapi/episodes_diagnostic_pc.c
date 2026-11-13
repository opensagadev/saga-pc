#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). Episodes_ConfigureList
   calls it with the config path when the file cannot be opened and when it holds
   no episode; VC8 drops calls to an empty body it can see, so the definition
   lives outside episodes.cpp. */
void __cdecl EpisodesConfigDiagnosticPc(char *path) {
}
