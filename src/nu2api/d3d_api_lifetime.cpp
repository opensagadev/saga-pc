#include "saga.h"

extern "C" void __cdecl PcFreeApiCallBuffer(void) {
    if (PcApiCallBuffer != NULL)
        NuFree(PcApiCallBuffer, ".\\d3dApiCalls.cpp", 0x451);
    PcApiCallBufferCount = 0;
    PcApiCallBuffer = NULL;
}

extern "C" void __cdecl PcRendererCleanup(void) {
    STUBBED_NAMED("void action5fe450 at6df0c0");
    STUBBED_NAMED("void action5fa220 at6df0c5");
    STUBBED_NAMED("void action600bb0 at6df0ca");
    STUBBED_NAMED("void tail action5ffc10 at6df0cf");
}

extern "C" void __cdecl PcRendererShutdown(i32 unused) {
    if (PCDeviceIsAbsent() == 0) {
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTexture(1, NULL);
        PCDeviceSetTexture(2, NULL);
        PCDeviceSetTexture(3, NULL);
        STUBBED_NAMED("void action6e5590(0,0,0,0) at6df113");
        STUBBED_NAMED("void action6e5590(1,0,0,0) at6df120");
        STUBBED_NAMED("void action6e5590(2,0,0,0) at6df130");
        STUBBED_NAMED("void action6e5590(3,0,0,0) at6df13d");
    }
    STUBBED_NAMED("void action605bd0 at6df145");
    PCDeviceBackend.ReleaseDefaultPoolResources();
    STUBBED_NAMED("void action5fb960 at6df154");
    STUBBED_NAMED("void action5f85a0 at6df159");
    STUBBED_NAMED("void action401860 at6df15e");
    STUBBED_NAMED("void action5fa220 at6df163");
    STUBBED_NAMED("void action600bb0 at6df168");
    STUBBED_NAMED("void action5f9e50 at6df16d");
    STUBBED_NAMED("void action659140 at6df172");
    STUBBED_NAMED("void action5ffc10 at6df177");
    STUBBED_NAMED("void action5f9720 at6df17c");
    STUBBED_NAMED("void action602840 at6df181");
    STUBBED_NAMED("void action5f8fb0 at6df186");
    STUBBED_NAMED("void action631230 at6df18b");
    STUBBED_NAMED("void tail action74ed40 at6df190");
}

extern "C" void __cdecl PcDestroyPlatform(void) {
    PCNativeWarningDebugState.Shutdown();
    nu_keyboard_owner.Shutdown();
    PcRendererShutdown(0);
    PCDeviceBackend.Shutdown();
}
