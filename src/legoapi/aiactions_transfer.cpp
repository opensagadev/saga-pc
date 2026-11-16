#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/gizmos/transport/ledges.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"

extern "C" i32 special_debug;
extern "C" void PCSpecialEmptyHandler(...);
extern "C" {
    SOCK *FindSock(SOCKSYS *sys, char *name) {
        if (sys != NULL) {
            SOCK *sock = sys->socks;
            for (i32 index = 0; index < 64; ++index, ++sock) {
                if (sock->valid && NuStrICmp(name, sock->name) == 0) {
                    return sock;
                }
            }
        }
        return NULL;
    }
}

static i32 NuSpecialFindMultiDisplay(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 capacity, i32 flags) {
    i32 count = 0;
    NUDISPLAY_SPECIAL *special = scene->display_list->specials;
    for (i32 index = 0; index < scene->display_list->special_count; ++index, ++special) {
        if (NuStrIStr(special->name, name) != NULL) {
            dest->scene = scene;
            dest->display_special = special;
            dest->special = NULL;
            ++dest;
            if (++count >= capacity) {
                return count;
            }
        }
    }
    if (flags != 0 && special_debug != 0) {
        PCSpecialEmptyHandler(name);
    }
    return count;
}

extern "C" i32 NuSpecialFindMulti(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 capacity, i32 flags) {
    NUDISPLAY_SPECIAL *display_special = NULL;
    i32 count = 0;
    if (scene != NULL) {
        if (scene->display_list != NULL) {
            return NuSpecialFindMultiDisplay(scene, dest, name, capacity, flags);
        }
        NUSPECIAL_LEGACY *special = scene->specials;
        for (i32 index = 0; index < scene->special_count; ++index, ++special, ++display_special) {
            if (NuStrIStr(special->name, name) != NULL) {
                dest->scene = scene;
                dest->special = (NUMTX *)special;
                dest->display_special = scene->display_list != NULL ? display_special : NULL;
                ++dest;
                if (++count >= capacity) {
                    return count;
                }
            }
        }
        if (flags != 0 && special_debug != 0) {
            PCSpecialEmptyHandler(name);
        }
        return count;
    }
    PCSpecialEmptyHandler();
    return 0;
}

extern "C" {
    static i32 Ledges_Load(void *world_info, void *) {
        WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
        if (world->ledge_count != 0) {
            return 0;
        }
        i32 version = EdFileReadInt();
        world->ledge_count = EdFileReadInt();
        for (i32 i = 0; i < world->ledge_count; ++i) {
            LEDGE_s *ledge = &world->ledges[i];
            EdFileRead(ledge->name, 8);
            EdFileReadNuVec(&ledge->position);
            ledge->y_rotation = EdFileReadShort();
            ledge->type_code = EdFileReadChar();
            if (version > 1) {
                ledge->unknown_1c = EdFileReadShort();
                ledge->unknown_1e = EdFileReadShort();
                if (version == 2) {
                    ledge->flags = 0;
                } else {
                    ledge->flags = EdFileReadUnsignedChar();
                }
            } else {
                ledge->unknown_1c = -1;
                ledge->unknown_1e = -1;
                ledge->flags = 0;
            }
        }
        return 1;
    }
}
extern "C" void StartBallooning(GAMEOBJECT *, i32);

extern "C" struct AIPATHCNXCONTROLLER_s *AIPathCnxControllerCreate(AI_CONTROL_SYSTEM *, AISYS *, AIPATH *, char *, char *, i32, char *, i32,
                                                                   i32);
