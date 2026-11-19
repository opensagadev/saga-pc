#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

void Faders_Configure(WORLDINFO *world, char *config) {
    NUFPAR *parser;
    FADER_s *fader;
    world->faders = NULL;
    world->fader_count = 0;
    if (world->current_gscn == NULL) {
        return;
    }
    parser = NuFParCreateMem("faders", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    world->faders = (FADER_s *)world->giz_buffer.addr;
    fader = world->faders;
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
        if (NuFParGetWord(parser) == 0 || NuSpecialFind(world->current_gscn, &fader->special, parser->word_buffer_ptr, 1) == 0) {
            continue;
        }
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "while_animating") == 0 && NuFParGetWord(parser) != 0) {
                NuSpecialFind(world->current_gscn, &fader->while_animating, parser->word_buffer_ptr, 1);
            }
        }
        ++fader;
        ++world->fader_count;
    }
    NuFParDestroy(parser);
    if (world->fader_count > 0) {
        world->giz_buffer.addr = ((u32)fader + 3) & ~3u;
    } else {
        world->faders = NULL;
    }
}

void Faders_Reset(WORLDINFO *world) {
    if (world->faders != NULL) {
        i32 i;
        for (i = 0; i < world->fader_count; ++i) {
            NuSpecialSetVisibility(&world->faders[i].special, 0);
        }
    }
}

void Faders_Draw(WORLDINFO *world) {
    FADER_s *fader = world->faders;
    i32 i;
    if (fader == NULL) {
        return;
    }
    for (i = 0; i < world->fader_count; ++i, ++fader) {
        NUMTX matrix = *NuSpecialGetDrawMtx(&fader->special);
        f32 alpha;
        if (NuSpecialExistsFn(&fader->while_animating) != 0) {
            alpha = 1.0f - NuSpecialGetAnimPos(&fader->while_animating);
        } else {
            f32 distance = NuVecDist(&matrix.translation, &pNuCam->mtx.translation, NULL);
            if (distance < 0.5f) {
                continue;
            }
            if (distance < 2.0f) {
                alpha = (f32)((distance - 0.5) / 1.5);
            } else {
                alpha = 1.0f;
            }
        }
        if (alpha > 0.0f) {
            NuSpecialDrawAtAlpha(&fader->special, &matrix, alpha);
        }
    }
}
