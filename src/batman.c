#include "saga.h"

int __cdecl NuMain(int argc, char **argv) {
    NuCommandLine(&argc, &argv);
    WorldInfo_InitOnce();
    // TODO: Reconstruct the next PC startup call, InitOnce, and the remaining game loop.
    return 0;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show) {
    // TODO: Reconstruct the original command-line setup and application startup.
    return NuMain(0, NULL);
}
