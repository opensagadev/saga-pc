#include "saga.h"

void __cdecl NuTexAnimAddList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    if (animation == NULL)
        return;
    PCThreadCriticalSectionEnter_candidate(g_texAnimCriticalSection);
    node = ntal_free;
    if (node != NULL) {
        if (node->next == NULL)
            NuSevereWarning(NuTextureAnimationSlotsWarning);
        ntal_free = ntal_free->next;
        node->next = ntal_first;
        node->previous = NULL;
        node->first = animation;
        if (ntal_first != NULL)
            ntal_first->previous = node;
        ntal_first = node;
    }
    PCThreadCriticalSectionLeave_candidate(g_texAnimCriticalSection);
}

void __cdecl NuTexAnimRemoveList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    PCThreadCriticalSectionEnter_candidate(g_texAnimCriticalSection);
    for (node = ntal_first; node != NULL; node = node->next) {
        if (node->first == animation) {
            if (node->next != NULL)
                node->next->previous = node->previous;
            if (node->previous != NULL)
                node->previous->next = node->next;
            else
                ntal_first = node->next;
            node->next = ntal_free;
            ntal_free = node;
            NuThreadEnableThreadSwap();
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(g_texAnimCriticalSection);
}

PC_TEXTURE_ANIM_ENV *__cdecl NuTexAnimEnvCreate(VARIPTR *buffer, NUMTL *material, u16 *texture_ids, void *program) {
    PC_TEXTURE_ANIM_ENV *environment;
    if (buffer != NULL) {
        environment = (PC_TEXTURE_ANIM_ENV *)((buffer->addr + 3) & ~3u);
        buffer->void_ptr = environment + 1;
    } else {
        environment = (PC_TEXTURE_ANIM_ENV *)NuAlloc(sizeof(PC_TEXTURE_ANIM_ENV), NuTextureAnimationSourcePathPC, 548);
    }
    if (environment != NULL) {
        environment->program = program;
        environment->instruction_index = 0;
        environment->loop_depth = 0;
        environment->call_depth = 0;
        environment->wait_base = 0;
        environment->wait_random = 0;
        environment->wait_remaining = 0;
        environment->material = material;
        environment->texture_ids = texture_ids;
        environment->texture_index = 0;
        if (buffer != NULL)
            environment->flags &= ~1u;
        else
            environment->flags |= 1;
    }
    return environment;
}
