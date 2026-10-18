#include "diagnostics.h"
#include "pc_fmv_native.h"
#include <string.h>
extern "C" {
    PC_FMV_MOVIE_PC FmvMoviePc = {0};              // 0x02913198
    i32 FmvInitializedPc = 0;                      // 0x029131e0
    i32 FmvState1Pc = 0, FmvState2Pc = 0;          // 0x029131e4, 0x029131e8
    RADFMV_CONTEXT_PC *FmvContextPc = NULL;        // 0x029131ec
    IDirectSound8 *FmvSoundPc = NULL;              // 0x029131f0
    u32 FmvFormatsPc[10] = {0};                    // 0x029131f4
    HWND FmvWindowPc = NULL;                       // 0x0291321c
    i32 FmvMutedPc = 0;                            // 0x02913220
    i32 FmvCopyFlagsPc = 1;                        // 0x0080fc60
    f32 FmvVolumePc = 1.0f;                        // 0x0080fc64
    i32 FmvSleepErrorPc = 0, FmvSleepCarryPc = 0;  // 0x02913240, 0x02913244
    i32 FmvFrequencyReadyPc = 0;                   // 0x02913248
    LARGE_INTEGER FmvFrequencyPc = {1000};         // 0x0080fcd0
    void(__cdecl *FmvDrawCallbackPc)(void) = NULL; // 0x02913170
    void __cdecl NuFmvStopSoundPc(i32);
    IDirectSound8 *__cdecl NuFmvGetSoundFactoryPc(void);
}
PC_EFFECT_SURFACE_PC FmvSurfacePc; // 0x02913224

extern "C" void __cdecl FmvInitializeFormatsPc(void) {
    FmvFormatsPc[8] = 1;
    FmvFormatsPc[9] = 2;
    FmvFormatsPc[0] = 3;
    FmvFormatsPc[6] = 4;
    FmvFormatsPc[1] = 5;
    FmvFormatsPc[7] = 6;
    FmvFormatsPc[2] = 9;
    FmvFormatsPc[3] = 10;
    FmvFormatsPc[4] = 8;
    FmvFormatsPc[5] = 7;
}

extern "C" void __cdecl FmvSleepPc(i32 duration) {
    LARGE_INTEGER before, after;
    if (FmvFrequencyReadyPc == 0) {
        FmvFrequencyReadyPc = 1;
        QueryPerformanceFrequency(&FmvFrequencyPc);
    }
    FmvSleepCarryPc += duration;
    i32 adjusted = FmvSleepCarryPc - FmvSleepErrorPc;
    if (adjusted > 1000) {
        FmvSleepCarryPc = adjusted;
        QueryPerformanceCounter(&before);
        Sleep(FmvSleepCarryPc / 1000);
        QueryPerformanceCounter(&after);
        u64 elapsed = (u64)((after.QuadPart - before.QuadPart) * 1000000) / FmvFrequencyPc.QuadPart;
        FmvSleepErrorPc = (i32)elapsed - FmvSleepCarryPc;
        FmvSleepCarryPc %= 1000;
    }
}

extern "C" void __cdecl FmvDrawPc(PC_FMV_MOVIE_PC *movie) {
    f32 opacity = 1.0f;
    NUMTX world, view, projection;
    if (SoundVoiceEnabledGate == 0)
        return;
    PC_D3D9_DEVICE_PC *device = FmvContextPc->device;
    if (movie->target != NULL)
        PCDeviceBackend.SetSurfaceRenderTarget(&FmvSurfacePc, 0);
    else
        PCDeviceBackend.ResetTargets();
    if (movie->fading != 0)
        opacity = movie->fade_remaining / movie->fade_duration;
    RadFmvBeginScenePc(FmvContextPc);
    SAGA_SKIN_CALL(skin_trace_invalidate, (unsigned)device);
    device->methods->set_vertex_shader(device, NULL);
    device->methods->set_pixel_shader(device, NULL);
    device->methods->set_render_state(device, 22, 1);
    device->methods->set_render_state(device, 14, 0);
    device->methods->set_render_state(device, 7, 0);
    device->methods->set_render_state(device, 137, 0);
    device->methods->set_render_state(device, 27, 0);
    device->methods->set_render_state(device, 19, 5);
    device->methods->set_render_state(device, 20, 6);
    device->methods->set_render_state(device, 15, 0);
    device->methods->set_render_state(device, 24, 16);
    device->methods->set_render_state(device, 25, 5);
    device->methods->set_sampler_state(device, 0, 1, 3);
    device->methods->set_sampler_state(device, 0, 2, 3);
    device->methods->set_sampler_state(device, 0, 5, 2);
    device->methods->set_sampler_state(device, 0, 6, 2);
    device->methods->set_sampler_state(device, 0, 7, 2);
    device->methods->set_texture_stage_state(device, 0, 4, 4);
    device->methods->set_texture_stage_state(device, 0, 5, 2);
    device->methods->set_texture_stage_state(device, 0, 6, 0);
    device->methods->set_texture_stage_state(device, 0, 1, 5);
    device->methods->set_texture_stage_state(device, 0, 2, 2);
    device->methods->set_texture_stage_state(device, 0, 3, 0);
    memset(&world, 0, sizeof(world));
    world.m00 = world.m11 = world.m22 = world.m33 = 1.0f;
    device->methods->set_transform(device, 256, &world);
    D3DXMatrixOrthoLH((D3DXMATRIX *)&projection, 2.0f, 2.0f, 1.0f, 10.0f);
    device->methods->set_transform(device, 3, &projection);
    memset(&view, 0, sizeof(view));
    view.m00 = view.m11 = view.m22 = view.m33 = 1.0f;
    device->methods->set_transform(device, 2, &view);
    RadFmvBlitImagePc(movie->image, movie->x, movie->y, movie->x_scale, movie->y_scale, opacity);
    if (movie->target == NULL)
        RadFmvEndScenePc(FmvContextPc);
    else
        ((HRESULT(__stdcall *)(PC_D3D9_DEVICE_PC *))device->methods->unknown_0070[14])(device);
    if (movie->fading != 0) {
        movie->fade_remaining = (f32)(movie->fade_remaining - (f64)33.333335876464844);
        if (movie->fade_remaining < 0.0f) {
            movie->playing = 0;
            movie->fading = 0;
        }
    }
}

extern "C" i32 __cdecl FmvDecodePc(PC_FMV_MOVIE_PC *movie, RADFMV_IMAGE_PC *image, u32 unused) {
    void *pixels;
    u32 pitch, format, x, y, width, height;
    BinkDoFrame(movie->bink);
    if (RadFmvLockImagePc(image, &pixels, &pitch, &format, &x, &y, &width, &height, 1)) {
        do {
            BinkCopyToBufferRect(movie->bink, pixels, pitch, movie->bink->height, 0, 0, x, y, width, height,
                                 FmvFormatsPc[format] | (FmvCopyFlagsPc != 0 ? 0x80000000u : 0));
            RadFmvUnlockImagePc(image);
        } while (RadFmvLockImagePc(image, &pixels, &pitch, &format, &x, &y, &width, &height, 1));
    }
    return 0;
}

extern "C" i32 __cdecl FmvInitializePc(HWND window, GRAPHICS_FACTORY_PC *factory, PC_D3D9_DEVICE_PC *device,
                                       IDirectSound8 *sound) {
    GRAPHICS_DISPLAY_MODE_PC mode;
    FmvInitializeFormatsPc();
    factory->methods->get_adapter_display_mode(factory, 0, &mode);
    FmvContextPc = (RADFMV_CONTEXT_PC *)NuAlloc(sizeof(RADFMV_CONTEXT_PC), ".\\pc\\Bink\\dsplay_bink.cpp", 0x173);
    if (FmvContextPc == NULL)
        return 0;
    RadFmvInitializeFormatsPc(factory, mode.format, FmvContextPc);
    // GRAPHICS_CAPS_PC.texture_caps is being exposed at its verified PC offset by
    // integration.
    FmvContextPc->permits_nonpow2 = (PCDeviceBackend.caps.texture_caps >> 8) & 1;
    FmvContextPc->factory = factory;
    FmvContextPc->device = device;
    FmvContextPc->window = window;
    FmvSoundPc = sound;
    BinkSetSoundSystem(BinkOpenDirectSound, sound);
    FmvWindowPc = window;
    return 1;
}

extern "C" i32 __cdecl FmvOpenPc(PC_FMV_MOVIE_PC *movie, const char *path, i32 render_to_texture, f32 volume,
                                 i32 option) {
    u32 language;
    memset(movie, 0, sizeof(*movie));
    movie->fade_duration = 1.0f;
    movie->fade_remaining = 0.0f;
    movie->fading = 0;
    movie->option = option;
    language = NuLanguageGet();
    BinkSetSoundTrack(1, &language);
    LOG_INFO("FMV opening %s", path);
    movie->bink = BinkOpen(path, 0x4000);
    if (movie->bink == NULL) {
        LOG_WARN("FMV open failed: %s", path);
        return 0;
    }
    u32 index;
    for (index = 0; index < movie->bink->track_count; ++index)
        if (movie->bink->track_ids[index] == language)
            break;
    if (index == movie->bink->track_count && movie->bink->track_count != 0) {
        language = movie->bink->track_ids[0];
        BinkClose(movie->bink);
        BinkSetSoundTrack(1, &language);
        movie->bink = BinkOpen(path, 0x4000);
    }
    if (movie->bink == NULL) {
        LOG_WARN("FMV reopen failed: %s", path);
        return 0;
    }
    LOG_INFO("FMV ready: %s (%u x %u, %u frames)", path, movie->bink->width, movie->bink->height, movie->bink->frames);
    FmvVolumePc = volume;
    BinkSetVolume(movie->bink, language, (i32)(volume * 32768.0));
    movie->image = RadFmvCreateImagePc(FmvContextPc, movie->bink->width, movie->bink->height, 0, 256);
    if (render_to_texture != 0) {
        movie->width = movie->bink->width;
        movie->height = movie->bink->height;
        movie->target = PCGraphicsCreateEffectTexture(movie->width, movie->height, 1, 1, 2);
        FmvSurfacePc.Attach(movie->target, 0, false);
        movie->x = movie->y = 0.5f;
        movie->x_scale = (f32)movie->width / movie->bink->width;
        movie->y_scale = (f32)movie->height / movie->bink->height;
    } else {
        movie->width = PCDeviceBackend.parameters.width;
        movie->height = PCDeviceBackend.parameters.height;
        FmvSurfacePc.Reset();
        movie->target = NULL;
        f32 ratio = (f32)movie->bink->width / movie->bink->height;
        f32 display_ratio;
        switch (PcWindowOptions.aspect_mode) {
            case 1:
                display_ratio = 1.7777777910232544f;
                break;
            case 2:
                display_ratio = 1.600000023841858f;
                break;
            case 3:
                display_ratio = PCDeviceBackend.active_aspect;
                break;
            default:
                display_ratio = 1.3333333730697632f;
                break;
        }
        f32 x_ratio = ratio / display_ratio;
        if (x_ratio > 1.0f)
            x_ratio = 1.0f;
        f32 y_ratio = display_ratio / ratio;
        if (y_ratio > 1.0f)
            y_ratio = 1.0f;
        movie->x_scale = (f32)movie->width / movie->bink->width * x_ratio;
        movie->y_scale = (f32)movie->height / movie->bink->height * y_ratio;
        movie->x = (f32)(((f32)movie->width - movie->bink->width * movie->x_scale) * 0.5 + 0.5);
        movie->y = (f32)(((f32)movie->height - movie->bink->height * movie->y_scale) * 0.5 + 0.5);
    }
    return 1;
}

extern "C" void __cdecl FmvFramePc(PC_FMV_MOVIE_PC *movie) {
    if (SoundVoiceEnabledGate != 0) {
        if (FmvMutedPc != 0) {
            BinkSetVolume(movie->bink, 0, (i32)(FmvVolumePc * 32768.0));
            FmvMutedPc = 0;
        }
        if (BinkWait(movie->bink) == 0) {
            FmvDecodePc(movie, movie->image, 3);
            FmvDrawPc(movie);
            if (movie->bink->frame_number == movie->bink->frames)
                movie->playing = 0;
            else
                BinkNextFrame(movie->bink);
        } else
            FmvSleepPc(500);
    } else {
        BinkSetVolume(movie->bink, 0, 0);
        FmvMutedPc = 1;
    }
}

extern "C" void __cdecl FmvClosePc(PC_FMV_MOVIE_PC *movie) {
    if (FmvContextPc != NULL) {
        if (FmvContextPc->device != NULL)
            ((HRESULT(__stdcall *)(PC_D3D9_DEVICE_PC *, u32, void *))FmvContextPc->device->methods->set_texture)(
                FmvContextPc->device, 0, NULL);
        PCDeviceBackend.ResetTargets();
    }
    if (movie->image != NULL) {
        RadFmvDestroyImagePc(movie->image);
        movie->image = NULL;
    }
    if (movie->bink != NULL) {
        BinkClose(movie->bink);
        movie->bink = NULL;
    }
    if (movie->target != NULL) {
        FmvSurfacePc.Reset();
        PCGraphicsDestroyRenderTexture(movie->target);
    }
}

extern "C" void __cdecl FmvFadePc(PC_FMV_MOVIE_PC *movie, f32 duration) {
    if (movie->fading == 0) {
        movie->fading = 1;
        movie->fade_duration = duration;
        movie->fade_remaining = duration;
    }
}
