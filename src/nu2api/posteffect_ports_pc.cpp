#include "saga.h"

extern "C" void __cdecl PCByteCopy_candidate(void *, const void *, u32);
extern "C" PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void);
extern "C" PC_POST_DATA_PORT_MANAGER PostResourceManagerPc;
extern "C" PC_POST_DATA_PORT PostResourcePortsPc[6];
extern "C" PC_FRAMEBUFFER_PC *PostBlurFramebufferPc;
extern "C" PC_FRAMEBUFFER_PC *PostCopyFramebufferPc;

i32 __thiscall PC_POST_DATA_PORT_MANAGER::Register(const char *name, void *data) {
    PC_POST_DATA_PORT_ENTRY *entry;
    for (entry = entries; entry < entries + 256; ++entry) {
        if (NuStrCmp(entry->name, name) == 0) {
            entry->data = data;
            return entry - entries;
        }
    }
    for (entry = entries; entry < entries + 256; ++entry) {
        if (entry->references == 0) {
            PCByteCopy_candidate(entry->name, name, NuStrLen(name) + 1);
            entry->data = data;
            ++entry->references;
            return entry - entries;
        }
    }
    return -1;
}

extern "C" void __cdecl PostFilterInitSharedResourcesPc(void) {
    if (PostResourcePortsPc[0].index >= 0)
        --PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].references;
    PostResourcePortsPc[0].manager = &PostResourceManagerPc;
    PostResourcePortsPc[0].index = PostResourceManagerPc.Register("postEffect.outFramebuffer", NULL);
    if (PostResourcePortsPc[1].index >= 0)
        --PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].references;
    PostResourcePortsPc[1].manager = &PostResourceManagerPc;
    PostResourcePortsPc[1].index = PostResourceManagerPc.Register("postEffect.colorBuffer", NULL);
    if (PostResourcePortsPc[2].index >= 0)
        --PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].references;
    PostResourcePortsPc[2].manager = &PostResourceManagerPc;
    PostResourcePortsPc[2].index = PostResourceManagerPc.Register("postEffect.normalBuffer", NULL);
    if (PostResourcePortsPc[3].index >= 0)
        --PostResourcePortsPc[3].manager->entries[PostResourcePortsPc[3].index].references;
    PostResourcePortsPc[3].manager = &PostResourceManagerPc;
    PostResourcePortsPc[3].index = PostResourceManagerPc.Register("postEffect.velocityBuffer", NULL);
    if (PostResourcePortsPc[4].index >= 0)
        --PostResourcePortsPc[4].manager->entries[PostResourcePortsPc[4].index].references;
    PostResourcePortsPc[4].manager = &PostResourceManagerPc;
    PostResourcePortsPc[4].index = PostResourceManagerPc.Register("postEffect.depthRTBuffer", NULL);
    if (PostResourcePortsPc[5].index >= 0)
        --PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].references;
    PostResourcePortsPc[5].manager = &PostResourceManagerPc;
    PostResourcePortsPc[5].index = PostResourceManagerPc.Register("postEffect.depthBuffer", NULL);
    PostBlurFramebufferPc = PCGraphicsCreateFramebuffer();
    PostCopyFramebufferPc = PCGraphicsCreateFramebuffer();
}

extern "C" {
#pragma data_seg(".data$PostResourcePortsPc")
    PC_POST_DATA_PORT PostResourcePortsPc[6] = {{-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL}};
#pragma data_seg()
#pragma bss_seg(".bss$PostResourceManagerPc")
    PC_POST_DATA_PORT_MANAGER PostResourceManagerPc = {0};
#pragma bss_seg(".bss$PostBlurFramebufferPc")
    PC_FRAMEBUFFER_PC *PostBlurFramebufferPc = 0;
#pragma bss_seg(".bss$PostCopyFramebufferPc")
    PC_FRAMEBUFFER_PC *PostCopyFramebufferPc = 0;
#pragma bss_seg()
}

extern "C" const WCHAR PostEffectRenderLabelPc[];
extern "C" void __cdecl NuPostEffectRender(PC_FRAMEBUFFER_PC *output) {
    PC_FRAMEBUFFER_PC *bound = PCGraphicsGetBoundFramebufferPc();
    if (output == NULL)
        output = NuFramebufferGetObject(1);
    PC_FRAMEBUFFER_PC *intermediate = NuFramebufferGetObject(0);
    GRAPHICS_RENDER_TEXTURE_PC *color = PCGraphicsGetFramebufferAttachmentPc(bound, 0, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *normal = PCGraphicsGetFramebufferAttachmentPc(bound, 1, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *velocity = PCGraphicsGetFramebufferAttachmentPc(bound, 2, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *depth_rt = PCGraphicsGetFramebufferAttachmentPc(bound, 2, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *depth = PCGraphicsGetFramebufferAttachmentPc(bound, 4, NULL, NULL);
    PCGraphicsFramebufferActionPc(0);
    PostProxyBuffersPc[0].texture = color;
    PostProxyBuffersPc[0].kind = 0;
    PostProxyBuffersPc[0].enabled = PostProxyBuffersPc[0].resolved = true;
    PostProxyBuffersPc[1].texture = normal;
    PostProxyBuffersPc[1].kind = 1;
    PostProxyBuffersPc[1].enabled = PostProxyBuffersPc[1].resolved = true;
    PostProxyBuffersPc[2].texture = velocity;
    PostProxyBuffersPc[2].kind = 2;
    PostProxyBuffersPc[2].enabled = PostProxyBuffersPc[2].resolved = true;
    PostProxyBuffersPc[3].texture = depth_rt;
    PostProxyBuffersPc[3].kind = 2;
    PostProxyBuffersPc[3].enabled = PostProxyBuffersPc[3].resolved = true;
    PostProxyBuffersPc[4].texture = depth;
    PostProxyBuffersPc[4].kind = 4;
    PostProxyBuffersPc[4].enabled = PostProxyBuffersPc[4].resolved = true;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = &PostProxyBuffersPc[0];
    PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].data = &PostProxyBuffersPc[1];
    PostResourcePortsPc[3].manager->entries[PostResourcePortsPc[3].index].data = &PostProxyBuffersPc[2];
    PostResourcePortsPc[4].manager->entries[PostResourcePortsPc[4].index].data = &PostProxyBuffersPc[3];
    PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data = &PostProxyBuffersPc[4];
    D3DPERF_BeginEvent(0xffffffff, PostEffectRenderLabelPc);
    NuPostFilterGen *filters[5];
    i32 count = 0;
    filters[0] = NULL;
    if (NuPostSpeedFilterPc != NULL && ((NuPostFilterGen *)NuPostSpeedFilterPc)->isEnabled())
        filters[count++] = (NuPostFilterGen *)NuPostSpeedFilterPc;
    if (NuPostDeferredFilterPc != NULL && ((NuPostFilterGen *)NuPostDeferredFilterPc)->isEnabled())
        filters[count++] = (NuPostFilterGen *)NuPostDeferredFilterPc;
    if (NuPostMainFilterPc != NULL && ((NuPostFilterGen *)NuPostMainFilterPc)->isEnabled())
        filters[count++] = (NuPostFilterGen *)NuPostMainFilterPc;
    if (NuPostMotionFilterPc != NULL && ((NuPostFilterGen *)NuPostMotionFilterPc)->isEnabled())
        filters[count++] = (NuPostFilterGen *)NuPostMotionFilterPc;
    if (NuPostAccumFilterPc != NULL && ((NuPostFilterGen *)NuPostAccumFilterPc)->isEnabled())
        filters[count++] = (NuPostFilterGen *)NuPostAccumFilterPc;
    NuDisplayHandlerVertexShaderCachePc = NULL;
    NuDisplayHandlerPixelShaderCachePc = NULL;
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    for (i32 i = 0; i < count; ++i) {
        PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data =
            i + 1 < count ? intermediate : output;
        filters[i]->render();
        filters[i]->reset();
        for (u32 stage = 0; stage < 6; ++stage)
            PCGraphicsBindEffectTexture(NULL, stage);
    }
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 1);
    PCDeviceSetRenderState(27, 1);
    PCDeviceSetRenderState(7, 1);
    PCDeviceSetRenderState(23, 4);
    ++NuPostEffectsRanPc;
    D3DPERF_EndEvent();
}

extern "C" {
#pragma data_seg(".data$PostProxyBuffersPc")
    PC_POST_BUFFER_PROXY_PC PostProxyBuffersPc[5] = {{NULL, 0, true, true, {0, 0}},
                                                     {NULL, 0, true, true, {0, 0}},
                                                     {NULL, 0, true, true, {0, 0}},
                                                     {NULL, 0, true, true, {0, 0}},
                                                     {NULL, 0, true, true, {0, 0}}};
#pragma data_seg(".data$PostBloomAttachPendingPc")
    u8 PostBloomAttachPendingPc = 1;
#pragma data_seg()
#pragma const_seg(".rdata$PostEffectRenderLabelPc")
    const WCHAR PostEffectRenderLabelPc[] = L"Post-processing-effect render";
#pragma const_seg()
}
