#include "saga.h"

extern "C" {
    i32 __cdecl PCFrameBeginScenePc(void);
    void __cdecl PCFrameResetTargetsPc(void);
    void __cdecl PCFrameConfigureTargetsPc(void);
}

/* Full native 60aff0 control reconstruction, with genuinely missing VOID
 * actions retained at their actual call sites. No consumed value is stubbed. */
extern "C" i32 __cdecl NuRndrSwapScreen(i32 mode) {
    i32 last_effect_scene = -1;
    RenderFrameShaderTimePc += nuapi.frame_time;
    DwaWeightCursorPc = 0xef00;
    DwaPointerCursorPc = 0x800;
    PCFrameBeginScenePc();
    PCFrameCriticalSectionEnter_candidate();
    for (i32 i = 0; i != 4; ++i)
        PCDeviceSetTexture((u32)i, NULL);
    PCDeviceSetStreamSourcePc(0, NULL, 0, 0);
    PCDeviceSetStreamSourcePc(1, NULL, 0, 0);
    PCDeviceSetStreamSourcePc(2, NULL, 0, 0);
    PCDeviceSetStreamSourcePc(3, NULL, 0, 0);
    NuShaderManagerBindShaderPc(NULL);
    NuDebrisRendererFlushBuffers();
    NuDisplayListSwapBuffersEndFrame();
    NuRndrSwapStreamBuffers();
    _NuTimeBarSlotEnd(-1, 0);
    NuDisplayListPurge_candidate();
    _NuTimeBarSlotBegin(-1, 4, "CPU_QUEUE_DRAW");
    for (i32 i = 0; i < RenderFrameShaderArrayCountPc && i * 16 < 0x80; ++i) {
        NuShaderManagerSetElementfv(0x4e, i, RenderFrameShaderArrayPc[i]);
    }
    NuShaderManagerSetfv(0x3f, &RenderFrameShaderTimePc);
    PCDeviceResetSamplerDefaultsPc();
    NuPostEffectReset();
    for (i32 i = 0; i < sceneParametersCount; ++i) {
        NURNDRLIBSCENE *scene = &sceneParameters[i];
        if (!PcWindowOptions.bloomenabled)
            scene->unknown_58 = 0;
        if (!PcWindowOptions.shadowsenabled)
            scene->unknown_48 = 0;
        if (!PcWindowOptions.speedblurenabled)
            scene->unknown_188 = 0;
        if ((scene->unknown_58 != 0 && NuPostEffectIsInitialised(4)) ||
            (scene->unknown_ac != 0 && NuPostEffectIsInitialised(8)) ||
            (scene->unknown_48 != 0 && NuPostEffectIsInitialised(0x20)) ||
            (scene->unknown_188 != 0 && NuPostEffectIsInitialised(0x80)))
            last_effect_scene = i;
    }
    if (last_effect_scene != -1)
        PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
    for (i32 i = 0; i < sceneParametersCount; ++i) {
        NURNDRLIBSCENE *scene = &sceneParameters[i];
        bool rendered_effect = false;
        RenderFrameSceneFlagsPc = scene->unknown_24;
        PC_FRAMEBUFFER_PC *destination = i >= last_effect_scene ? PcBootSecondaryFramebuffer : PcBootDefaultFramebuffer;
        f32 dimensions[4];
        dimensions[0] = (f32)PS2_REZ_W;
        dimensions[1] = (f32)PS2_REZ_H;
        dimensions[2] = 1.0f / dimensions[0];
        dimensions[3] = 1.0f / dimensions[1];
        if (scene->dyn_light_count != 0) {
            NURNDRLIGHTRECORD *light = static_cast<NURNDRLIGHTRECORD *>(scene->dyn_light_ptr);
            if (NuDynamicLightIsEnabled(light)) {
                if (scene->scene_id != -1)
                    NuDynamicLightAddRenderScene(light, scene->unknown_40, scene->scene_id);
                if (scene->unknown_40 == 0) {
                    NuPostEffectAddDynamicLight(reinterpret_cast<NuDynamicLight *>(light));
                }
            }
            scene->scene_id = -1;
        }
        if (scene->unknown_ac != 0)
            NuPostEffectIsInitialised(8); /* Native60b270 result unused. */
        if (scene->unknown_58 != 0 && NuPostEffectIsInitialised(4)) {
            NuPostEffectEnable(4);
            NuPostEffectBloom(reinterpret_cast<const NU_POST_BLOOM_PARAMETERS_PC *>(&scene->burnout_parameters_pc));
            scene->unknown_58 = 0;
            rendered_effect = true;
        }
        if (scene->unknown_48 != 0) {
            NuPostEffectEnable(0x20);
            NuPostEffectDeferredShading(reinterpret_cast<const f32 *>(&scene->unknown_48));
            scene->unknown_48 = 0;
            rendered_effect = true;
        }
        if (scene->unknown_188 != 0) {
            STUBBED_NAMED(0x005c32d0); /* Activate(0x80),60b2e2 */
            STUBBED_NAMED(0x005c37d0); /* (&scene->unknown_188),60b2e8 */
            rendered_effect = true;
        }
        if (i <= last_effect_scene) {
            PCFrameConfigureTargetsPc();
        } else {
            PCFrameResetTargetsPc();
        }
        NuShaderManagerSetfv(0x3e, dimensions);
        if (scene->clear_flags_08 != 0) {
            u32 flags = scene->clear_flags_08;
            u32 clear = 0;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(14, 1);
            PCDeviceSetRenderState(52, 1);
            if (flags & 0x100)
                clear = 1;
            if (flags & 0x200)
                clear |= 2;
            if (flags & 0x1800)
                clear |= 4;
            u32 colour = (u32)scene->bg_colour | 0xff000000u;
            scene->bg_colour = (i32)colour;
            colour = (colour & 0xff000000u) | ((colour & 0xffu) << 16) | (colour & 0xff00u) | ((colour >> 16) & 0xffu);
            PCDeviceClear(0, NULL, clear, colour, scene->clear_alpha, 0x60);
            PCDeviceSetRenderState(52, 0);
        }
        if (scene->scene_id != -1) {
            D3DPERF_BeginEvent((D3DCOLOR)-1, L"3D render");
            NuDisplayListDrawRenderScene_candidate(scene->scene_id);
            D3DPERF_EndEvent();
        }
        if (rendered_effect) {
            PCDevicePostEffectBarrierPc();
            NuShaderManagerBindShaderPc(NULL);
            NuPostEffectRender(destination);
            NuShaderManagerBindShaderPc(NULL);
            PCDeviceResetSamplerDefaultsPc();
        }
        if (scene->unknown_214 != 0) {
            FRAME_NATIVE_TEXTURE_PC *texture =
                reinterpret_cast<FRAME_NATIVE_TEXTURE_PC *>(NuTextureGetDimensions((i32)scene->unknown_214));
            i32 index =
                (i32)((u32)texture - (u32)&FrameNativeTexturePoolPc.records[0]) / (i32)sizeof(FRAME_NATIVE_TEXTURE_PC);
            if ((u32)index <= 15u && texture->render_texture != NULL) {
                i32 width = NuTexGetWidth((i32)scene->unknown_214);
                i32 height = NuTexGetHeight((i32)scene->unknown_214);
                bool resize = width != PS2_REZ_W || height != PS2_REZ_H;
                PC_EFFECT_SURFACE_PC target;
                target.Attach(texture->render_texture, 0, false);
                PCDeviceBackend.CopySurfaceTargetPc(&target, resize ? 2 : 1, 0);
            }
        }
    }
    sceneParametersCount = 0; /* Actual native60b50d, after both live-count loops. */
    NuRndrEndFrameActionPc();
    NuPostEffectEnd();
    _NuTimeBarSlotEnd(-1, 4);
    STUBBED_NAMED(0x005ded40); /* native60b529 */
    NuDisplayListBeginFrame();
    NuDisplayListStreamCursor = &nudlist_stream_cursor;
    nudlist_field_1598 = nuprim_field_ba8;
    if (RenderFramePausedPc == 0)
        ++PCWindFrameCount;
    if (RenderFrameCallbackPc != NULL)
        RenderFrameCallbackPc();
    NuRndrEndSceneEx(0);
    PCDeviceEndScenePc(); /* Genuine native10-byte wrapper6e4a10; result unused. */
    PcInputEndFramePc();
    PCFramePresentNativePc(mode, 1);
    PCFrameCriticalSectionLeave_candidate();
    NuTimeBarSlotReset(-1, 0);
    _NuTimeBarSlotBegin(-1, 0, "CPU");
    return 1;
}
extern "C" i32 __cdecl NuRndrSwapScreenEx(i32 mode, void(__cdecl *callback)(void)) {
    if (callback != NULL)
        callback();
    return NuRndrSwapScreen(mode);
}

extern "C" {
    extern i32 PCFramePresentationStatePc;
    void __cdecl PCPresentationPlatformLeafPc(void);
}
STATIC_ASSERT(sizeof(PCFramePresentationStatePc) == 4);

extern "C" i32 __cdecl PCFramePresentNativePc(i32 mode_unused, i32 flag_unused) {
    PCFramePresentationStatePc = 0;
    PCDeviceBackend.Present(NULL, NULL, NULL, NULL);
    PCPresentationPlatformLeafPc();
    return 0;
}
