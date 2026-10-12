#include "saga.h"
typedef struct FADER_PC {
    NUSPECIALHANDLE special, while_animating;
} FADER_PC;
STATIC_ASSERT(sizeof(FADER_PC) == 0x18);
STATIC_ASSERT(offsetof(FADER_PC, while_animating) == 0xc);
typedef struct FADERS_WORLD_VIEW_PC {
    u8 unknown_000[0x5034];
    FADER_PC *faders;
    i32 fader_count;
} FADERS_WORLD_VIEW_PC;
STATIC_ASSERT(offsetof(FADERS_WORLD_VIEW_PC, faders) == 0x5034);
STATIC_ASSERT(offsetof(FADERS_WORLD_VIEW_PC, fader_count) == 0x5038);
void __cdecl Faders_Configure(WORLDINFO *world, char *config) {
    FADERS_WORLD_VIEW_PC *view = (FADERS_WORLD_VIEW_PC *)world;
    NUFPAR_PC *parser;
    FADER_PC *fader;
    view->faders = NULL;
    view->fader_count = 0;
    if (world->current_gscn == NULL) {
        return;
    }
    parser = NuFParCreateMem("faders", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    view->faders = (FADER_PC *)world->giz_buffer.addr;
    fader = view->faders;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "fader") != 0) {
            continue;
        }
        fader->special.scene = NULL;
        fader->special.special = NULL;
        fader->special.display_special = NULL;
        fader->while_animating.scene = NULL;
        fader->while_animating.special = NULL;
        fader->while_animating.display_special = NULL;
        if (NuFParGetWord(parser) == 0 ||
            NuSpecialFind(world->current_gscn, &fader->special, parser->word_buffer_ptr, 1) == 0) {
            continue;
        }
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "while_animating") == 0 && NuFParGetWord(parser) != 0) {
                NuSpecialFind(world->current_gscn, &fader->while_animating, parser->word_buffer_ptr, 1);
            }
        }
        ++view->fader_count;
        ++fader;
    }
    NuFParDestroy(parser);
    if (view->fader_count <= 0) {
        view->faders = NULL;
    } else {
        world->giz_buffer.addr = ((u32)fader + 3) & ~3u;
    }
}

void Faders_Reset(WORLDINFO *world) {
    if (world->faders != NULL && world->fader_count > 0) {
        i32 i;
        for (i = 0; i < world->fader_count; ++i) {
            NuSpecialSetVisibility(&world->faders[i].special, 0);
        }
    }
}

void Faders_Draw(WORLDINFO *world) {
    i32 i;
    for (i = 0; world->faders != NULL && i < world->fader_count; ++i) {
        FADER_s *fader = &(world->faders[i]);
        NUMTX matrix = *NuSpecialGetDrawMtx(&(*fader).special);
        f32 alpha = 0.0f;
        if (NuSpecialExistsFn(&(*fader).while_animating) == 0) {
            f32 distance = NuVecDist(((NUVEC *)(&matrix.m30)), ((NUVEC *)(&pNuCam->mtx.m30)), NULL);
            if (distance >= 2.0f) {
                alpha = 1.0f;
            } else if (distance >= 0.5f) {
                alpha = (distance - 0.5f) / 1.5f;
            }
        } else {
            alpha = 1.0f - NuSpecialGetAnimPos(&(*fader).while_animating);
        }
        if (alpha > 0.0f) {
            NuSpecialDrawAtAlpha(&(*fader).special, &matrix, alpha);
        }
    }
}
