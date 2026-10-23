#include "saga.h"
#include <string.h>

extern "C" {
    extern PC_EFFECT_SURFACE_PC *PcSurfaceRegistry[256];
    extern u8 PcSurfaceRegistryInitialized;
    extern PC_FRAMEBUFFER_PC PcFramebufferPool[32];
    extern u8 PcFramebufferActive[32];
    extern PC_FRAMEBUFFER_PC *PcBoundFramebuffer;
    extern PC_FRAMEBUFFER_PC *PcDefaultFramebuffer;
    extern PC_FRAMEBUFFER_PC *PcSecondaryFramebuffer;
}

PC_EFFECT_SURFACE_PC::PC_EFFECT_SURFACE_PC() {
    texture = NULL;
    owns_texture = 0;
    surface = NULL;
    level = 0;
    field_10 = 0;
    width = height = 0;
    registry_index = -1;
    Reset();
    if (!PcSurfaceRegistryInitialized) {
        memset(PcSurfaceRegistry, 0, sizeof(PcSurfaceRegistry));
        PcSurfaceRegistryInitialized = 1;
    }
    for (i32 index = 0; index != 256; ++index)
        if (PcSurfaceRegistry[index] == NULL) {
            PcSurfaceRegistry[index] = this;
            registry_index = (i16)index;
            return;
        }
}
inline PC_EFFECT_SURFACE_PC::~PC_EFFECT_SURFACE_PC() {
    Reset();
    PcSurfaceRegistry[registry_index] = NULL;
}
void PC_EFFECT_SURFACE_PC::Reset() {
    if (texture != NULL && texture->surface != NULL)
        surface = NULL;
    else if (surface != NULL) {
        surface->methods->release(surface);
        surface = NULL;
    }
    if (texture != NULL && owns_texture)
        PCGraphicsDestroyRenderTexture(texture);
    texture = NULL;
    owns_texture = 0;
    level = 0;
    field_10 = 0;
    width = height = 0;
}
void PC_EFFECT_SURFACE_PC::Attach(GRAPHICS_RENDER_TEXTURE_PC *new_texture, i32 new_level, bool own) {
    if (texture == new_texture && (i32)level == new_level)
        return;
    Reset();
    texture = new_texture;
    owns_texture = (u8)own;
    surface = NULL;
    level = (u16)new_level;
    field_10 = 0;
    height = width = 0;
    if (new_texture == NULL)
        return;
    if (new_texture->surface != NULL) {
        surface = new_texture->surface;
        width = new_texture->width;
        height = new_texture->height;
    } else {
        PC_TEXTURE_LEVEL_DESC_PC description;
        new_texture->resource->methods->get_surface_level(new_texture->resource, level, &surface);
        texture->resource->methods->get_level_desc(texture->resource, level, &description);
        width = (u16)description.width;
        height = (u16)description.height;
    }
}
void PC_EFFECT_SURFACE_PC::ReleaseResources() {
    for (i32 index = 0; index != 256; ++index) {
        PC_EFFECT_SURFACE_PC *item = PcSurfaceRegistry[index];
        if (item != NULL && item->surface != NULL) {
            if (item->texture->surface == NULL)
                item->surface->methods->release(item->surface);
            PcSurfaceRegistry[index]->surface = NULL;
        }
    }
}
void PC_EFFECT_SURFACE_PC::RestoreResources() {
    for (i32 index = 0; index != 256; ++index) {
        PC_EFFECT_SURFACE_PC *item = PcSurfaceRegistry[index];
        if (item == NULL)
            continue;
        if (item->texture == NULL)
            item->surface = NULL;
        else if (item->texture->surface != NULL) {
            item->surface = item->texture->surface;
            PcSurfaceRegistry[index]->width = PcSurfaceRegistry[index]->texture->width;
            PcSurfaceRegistry[index]->height = PcSurfaceRegistry[index]->texture->height;
        } else {
            PC_TEXTURE_LEVEL_DESC_PC description;
            item->texture->resource->methods->get_surface_level(item->texture->resource, item->level, &item->surface);
            item = PcSurfaceRegistry[index];
            item->texture->resource->methods->get_level_desc(item->texture->resource, item->level, &description);
            PcSurfaceRegistry[index]->width = (u16)description.width;
            PcSurfaceRegistry[index]->height = (u16)description.height;
        }
    }
}
PC_FRAMEBUFFER_PC::PC_FRAMEBUFFER_PC() {
}
PC_FRAMEBUFFER_PC::~PC_FRAMEBUFFER_PC() {
}

extern "C" PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void) {
    i32 index;
    for (index = 0; index != 32; ++index)
        if (PcFramebufferActive[index] == 0)
            break;
    if (index == 32) {
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }
    PcFramebufferActive[index] = 1;
    PC_FRAMEBUFFER_PC *frame = &PcFramebufferPool[index];
    if (frame == NULL) {
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }
    for (index = 0; index != 5; ++index) {
        frame->targets[index].Reset();
        frame->clear_colour[index] = 0xffffffff;
    }
    frame->changed = 1;
    return frame;
}
extern "C" void __cdecl PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER_PC *frame) {
    for (i32 index = 0; index != 5; ++index)
        frame->targets[index].Reset();
    for (i32 index = 0; index != 32; ++index)
        if (frame == &PcFramebufferPool[index])
            PcFramebufferActive[index] = 0;
}
extern "C" void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *frame, i32 index,
                                                    GRAPHICS_RENDER_TEXTURE_PC *texture, i32 level) {
    frame->targets[index].Attach(texture, level, false);
    frame->changed = 1;
}
extern "C" void __cdecl PCGraphicsAttachOwnedFramebuffer(PC_FRAMEBUFFER_PC *frame, i32 index,
                                                         GRAPHICS_RENDER_TEXTURE_PC *texture, i32 level) {
    frame->targets[index].Attach(texture, level, true);
}
extern "C" void __cdecl PCGraphicsReleaseFramebuffers(void) {
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        PC_EFFECT_SURFACE_PC *target = &frame->targets[0];
        if (target->texture != NULL || target->level != 0) {
            target->Reset();
            target->texture = NULL;
            target->owns_texture = 0;
            target->surface = NULL;
            target->level = 0;
            target->field_10 = 0;
            target->width = target->height = 0;
        }
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        PC_EFFECT_SURFACE_PC *target = &frame->targets[1];
        if (target->texture != NULL || target->level != 0) {
            target->Reset();
            target->texture = NULL;
            target->owns_texture = 0;
            target->surface = NULL;
            target->level = 0;
            target->field_10 = 0;
            target->width = target->height = 0;
        }
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        PC_EFFECT_SURFACE_PC *target = &frame->targets[2];
        if (target->texture != NULL || target->level != 0) {
            target->Reset();
            target->texture = NULL;
            target->owns_texture = 0;
            target->surface = NULL;
            target->level = 0;
            target->field_10 = 0;
            target->width = target->height = 0;
        }
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcSecondaryFramebuffer;
        PC_EFFECT_SURFACE_PC *target = &frame->targets[0];
        if (target->texture != NULL || target->level != 0) {
            target->Reset();
            target->texture = NULL;
            target->owns_texture = 0;
            target->surface = NULL;
            target->level = 0;
            target->field_10 = 0;
            target->width = target->height = 0;
        }
        frame->changed = 1;
    }
}
extern "C" void __cdecl PCGraphicsRestoreFramebuffers(void) {
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        frame->targets[0].Attach(PCDeviceBackend.textures[0], 0, false);
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        frame->targets[1].Attach(PCDeviceBackend.textures[1], 0, false);
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcDefaultFramebuffer;
        frame->targets[2].Attach(PCDeviceBackend.textures[2], 0, false);
        frame->changed = 1;
    }
    {
        PC_FRAMEBUFFER_PC *frame = PcSecondaryFramebuffer;
        frame->targets[0].Attach(PCDeviceBackend.textures[0], 0, false);
        frame->changed = 1;
    }
}
extern "C" void __cdecl PCGraphicsBindFramebuffer(PC_FRAMEBUFFER_PC *frame) {
    if (PcBoundFramebuffer == frame && (frame == NULL || frame->changed == 0))
        return;
    if (frame == PcDefaultFramebuffer)
        PCDeviceBackend.ConfigureCurrentTargets();
    else if (frame == PcSecondaryFramebuffer)
        PCDeviceBackend.ResetTargets();
    else if (frame != NULL) {
        PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[0], 0);
        PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[1], 1);
        PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[2], 2);
        PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[3], 3);
        PCDeviceBackend.SetDepthStencilSurface(&frame->targets[4]);
    }
    PcBoundFramebuffer = frame;
    if (frame != NULL)
        frame->changed = 1;
}
extern "C" void __cdecl PCGraphicsBindDefaultFramebuffer(void) {
    PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
}
extern "C" void __cdecl PCGraphicsClearFramebuffer(u32 mask, u32 colour) {
    u32 flags = 0;
    if (PcBoundFramebuffer->targets[0].texture != NULL && (mask & 0x100))
        flags = 1;
    if (PcBoundFramebuffer->targets[4].texture != NULL && (mask & 0x200))
        flags |= 6;
    if (flags != 0) {
        u32 converted =
            (colour & 0xff000000u) | ((colour & 0xffu) << 16) | (colour & 0xff00u) | ((colour >> 16) & 0xffu);
        PCDeviceClear(0, NULL, flags, converted, 1.0f, 0);
    }
}
extern "C" PC_EFFECT_SURFACE_PC *__cdecl PCGraphicsFramebufferTarget(PC_FRAMEBUFFER_PC *frame, i32 index) {
    return &frame->targets[index];
}
extern "C" void __cdecl PCGraphicsInitializeFramebuffers(u8 flags, i32 unused, VARIPTR *buffer, VARIPTR end) {
    (void)unused;
    (void)buffer;
    (void)end;
    memset(PcFramebufferPool, 0, sizeof(PcFramebufferPool));
    memset(PcFramebufferActive, 0, sizeof(PcFramebufferActive));
    PcBoundFramebuffer = PcDefaultFramebuffer = PcSecondaryFramebuffer = NULL;
    bool multi = PCDeviceBackend.caps.simultaneous_render_targets > 1;
    bool second = multi && (flags & 2);
    bool third = multi && (flags & 4);
    PcDefaultFramebuffer = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, PCDeviceBackend.textures[0], 0);
    PcDefaultFramebuffer->clear_colour[0] = 0;
    PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
    PcDefaultFramebuffer->clear_colour[2] = 0xffffffff;
    if (second && PCDeviceBackend.target_count > 1) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, PCDeviceBackend.textures[1], 0);
        PcDefaultFramebuffer->clear_colour[1] = 0x808080;
    }
    if (third && PCDeviceBackend.target_count > 2) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
        PcDefaultFramebuffer->clear_colour[2] = 0x8080;
    }
    PcSecondaryFramebuffer = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, PCDeviceBackend.textures[0], 0);
    PcSecondaryFramebuffer->clear_colour[0] = 0;
    PcBootDefaultFramebuffer = PcBootSecondaryFramebuffer = NULL;
    PcBootDefaultFramebuffer = PcDefaultFramebuffer;
    PcBootSecondaryFramebuffer = PcSecondaryFramebuffer;
    PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
}

extern "C" {
    PC_EFFECT_SURFACE_PC *PcSurfaceRegistry[256] = {NULL};
    u8 PcSurfaceRegistryInitialized = 0;
    PC_FRAMEBUFFER_PC PcFramebufferPool[32];
    u8 PcFramebufferActive[32] = {0};
    PC_FRAMEBUFFER_PC *PcBoundFramebuffer = NULL;
    PC_FRAMEBUFFER_PC *PcDefaultFramebuffer = NULL;
    PC_FRAMEBUFFER_PC *PcSecondaryFramebuffer = NULL;
    PC_BOOT_FRAMEBUFFER_VIEW_PC PcBootFramebuffersPc = {NULL, NULL};
}

extern "C" void __cdecl NuEffectTexLockVP(VARIPTR *buffer, VARIPTR end) {
    (void)buffer;
    (void)end;
}
extern "C" i32 __cdecl NuEffectTexUnlockVP(void) {
    return 0;
}
extern "C" PC_FRAMEBUFFER_PC *__cdecl NuFramebufferGetObject(i32 index) {
    return PcBootFramebuffersPc.slots[index];
}
extern "C" i32 __cdecl NuFramebufferGetWidth(PC_FRAMEBUFFER_PC *framebuffer) {
    if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer)
        return PCDeviceBackend.parameters.width;
    return framebuffer != NULL ? framebuffer->targets[0].width : 0;
}
extern "C" i32 __cdecl NuFramebufferGetHeight(PC_FRAMEBUFFER_PC *framebuffer) {
    if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer)
        return PCDeviceBackend.parameters.height;
    return framebuffer != NULL ? framebuffer->targets[0].height : 0;
}

/* Two additional genuine original leaves, proposed in existing effect_surface.cpp.
 * Complete native entries are6B and1B; neither replaces the held copy operation. */
extern "C" PC_FRAMEBUFFER_PC *__cdecl PCGraphicsGetBoundFramebufferPc(void) {
    return PcBoundFramebuffer;
}
extern "C" void __cdecl PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *frame, i32 enabled) {
    (void)frame;
    (void)enabled;
}

extern "C" void __cdecl PCGraphicsCopyFramebufferTex2DPc(i32 attachment, GRAPHICS_RENDER_TEXTURE_PC *texture, i32 level,
                                                         i32 x, i32 y, i32 width, i32 height) {
    (void)x;
    (void)y;
    if (attachment != 0) {
        /* Native tail5f7490 is the existing genuine RET-only operation; no arg is read. */
        PCGraphicsRenderTextureAllocationError();
        return;
    }
    {
        bool resize = width != PS2_REZ_W || height != PS2_REZ_H;
        PC_EFFECT_SURFACE_PC target;
        target.Attach(texture, level, false);
        PCDeviceBackend.CopySurfaceTargetPc(&target, resize ? 2 : 1, 0);
    }
}
