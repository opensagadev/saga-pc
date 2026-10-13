#include "saga.h"

extern "C" {
    void __cdecl PCGraphicsReleaseFramebuffers(void);
    void __cdecl PCGraphicsRestoreFramebuffers(void);
}

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
        PCDeviceSetStreamSourcePc(0, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(1, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(2, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(3, NULL, 0, 0);
    }
    PCGraphicsReleaseFramebuffers();
    PCDeviceBackend.ReleaseDefaultPoolResources();
    PC_EFFECT_SURFACE_PC::ReleaseResources();
    PCGraphicsReleaseEffectTextures();
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

extern "C" void __cdecl PCGraphicsRecreateResources(i32 reset_available) {
    PCDeviceBackend.RestoreDefaultPoolResources();
    if (reset_available != 0) {
        STUBBED_NAMED("void action5c3900 at6dfe33");
        STUBBED_NAMED("void action5658b0(PS2_REZ_W,PS2_REZ_H) at6dfe45");
    }
    PCGraphicsRestoreEffectTextures(reset_available == 1);
    PC_EFFECT_SURFACE_PC::RestoreResources();
    PCGraphicsRestoreFramebuffers();
    STUBBED_NAMED("void action600bc0 at6dfe68");
    STUBBED_NAMED("void action5f9e60 at6dfe6d");
    STUBBED_NAMED("NuDebrisCreateVertexBuffersPc at6dfe72 (private particle helper)");
    STUBBED_NAMED("void action659150 at6dfe77");
    STUBBED_NAMED("void action401850 at6dfe7c");
    STUBBED_NAMED("void action5f8fc0 at6dfe81");
    STUBBED_NAMED("void action6028b0 at6dfe86");
    PCDeviceResetState();
    STUBBED_NAMED("void action631250 at6dfe90");
    STUBBED_NAMED("void action74ed60 at6dfe95");
    STUBBED_NAMED("void tail action60a410 at6dfe9b");
}

extern "C" void __cdecl PcDestroyPlatform(void) {
    PCNativeWarningDebugState.Shutdown();
    nu_keyboard_owner.Shutdown();
    PcRendererShutdown(0);
    PCDeviceBackend.Shutdown();
}

extern "C" void __cdecl PCFrameResetTargetsPc(void) {
    PCDeviceBackend.ResetTargets();
}

extern "C" void __cdecl PCFrameConfigureTargetsPc(void) {
    PCDeviceBackend.ConfigureCurrentTargets();
}
