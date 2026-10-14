#include "saga.h"

struct PC_POST_DATA_PORT_ENTRY {
    void *data;
    char name[32];
    i32 references;
};
struct PC_POST_DATA_PORT_MANAGER {
    PC_POST_DATA_PORT_ENTRY entries[256];
    i32 __thiscall Register(const char *name, void *data);
};
struct PC_POST_DATA_PORT {
    i32 index;
    PC_POST_DATA_PORT_MANAGER *manager;
};
STATIC_ASSERT(sizeof(PC_POST_DATA_PORT_ENTRY) == 40);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT_ENTRY, name) == 4);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT_ENTRY, references) == 36);
STATIC_ASSERT(sizeof(PC_POST_DATA_PORT_MANAGER) == 0x2800);
STATIC_ASSERT(sizeof(PC_POST_DATA_PORT) == 8);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT, manager) == 4);
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
    PC_POST_DATA_PORT_MANAGER PostResourceManagerPc;
#pragma bss_seg(".bss$PostBlurFramebufferPc")
    PC_FRAMEBUFFER_PC *PostBlurFramebufferPc;
#pragma bss_seg(".bss$PostCopyFramebufferPc")
    PC_FRAMEBUFFER_PC *PostCopyFramebufferPc;
#pragma bss_seg()
}
