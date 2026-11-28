#include "gameapi/edpart.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/specialsfx.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include <stdio.h>

extern "C" {

    i32 InitSpecialSfx(void *world_ptr) {
        SPECIAL_RESOURCE_WORLD *world = (SPECIAL_RESOURCE_WORLD *)world_ptr;
        if (!world) {
            return 0;
        }
        world->buffer.addr = (world->buffer.addr + 3) & ~3u;
        world->entries = (SPECIAL_RESOURCE_ENTRY *)world->buffer.addr;
        world->buffer.addr += 0xf00;
        world->entry_count = 0;
        memset(world->entries, 0, 0xf00);
        world->buffer.addr = (world->buffer.addr + 15) & ~15u;
        world->events = (SPECIAL_RESOURCE_EVENT *)world->buffer.addr;
        world->buffer.addr = (world->buffer.addr + 0xc03) & ~3u;
        world->event_count = 0;
        memset(world->events, 0, 0xc00);
        return 1;
    }

    void fun_005b1800(void) {
    }

    i32 SpecialSfxLoad(char *path, void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        i32 version;
        i32 count;
        i32 i;
        i32 j;
        char name[64];
        i32 length;
        SPECIAL_RESOURCE_ENTRY *entry;
        SPECIAL_RESOURCE_EVENT *event;
        SPECIAL_RESOURCE_EVENT *previous;
        if (!world) {
            return 0;
        }
        EdFileSetMedia(1);
        if (!EdFileOpen(path, 0)) {
            return 0;
        }
        version = EdFileReadInt();
        if (version > 2) {
            EdFileClose();
            return 0;
        }
        count = EdFileReadInt();
        if (count > 192) {
            count = 192;
        }
        EdFileReadInt();
        world->special_sfx_count = 0;
        for (i = 0; i < count; ++i) {
            entry = &world->special_sfx[i];
            length = EdFileReadShort();
            EdFileRead(name, length);
            name[length] = 0;
            NuSpecialFind(world->current_gscn, &entry->special, name, 1);
            entry->event_count = EdFileReadChar();
            if (version >= 2) {
                u8 type = EdFileReadChar();
                entry->flags = (entry->flags & 0xf0) | (type & 0xf);
            }
            ++world->special_sfx_count;
        }
        event = world->special_sfx_events;
        for (i = 0; i < count; ++i) {
            entry = &world->special_sfx[i];
            for (j = 0; j < entry->event_count; ++j, ++event) {
                length = EdFileReadShort();
                EdFileRead(name, length);
                name[length] = 0;
                event->sfx_id = (i16)GetSfxId(name);
                event->flags = EdFileReadChar();
                event->flags = (event->flags & ~SPECIALSFX_EVENT_LOOPING) | ((IsSfxLooping(event->sfx_id) & 1) << 1);
                event->trigger_frame = EdFileReadFloat();
                event->previous_frame = EdFileReadFloat();
                ++world->special_sfx_event_count;
            }
        }
        event = world->special_sfx_events;
        for (i = 0; i < world->special_sfx_count; ++i) {
            entry = &world->special_sfx[i];
            entry->events = event;
            previous = event;
            if (event == world->special_sfx_events + world->special_sfx_event_count) {
                event = NULL;
            } else {
                ++event;
            }
            for (j = 0; j < entry->event_count - 1; ++j) {
                previous->next = event;
                previous = event;
                ++event;
            }
            if (previous) {
                previous->next = NULL;
            }
        }
        EdFileClose();
        return 1;
    }

    i32 SpecialSfxAdd(i32 special_index) {
        nuhspecial_s special;
        SPECIAL_RESOURCE_ENTRY *entry;
        if (WORLD->special_sfx_count < 192) {
            NuGScnGetSpecial(&special, WORLD->current_gscn, special_index);
            if (NuSpecialExistsFn(&special) != 0) {
                entry = &WORLD->special_sfx[WORLD->special_sfx_count];
                entry->flags &= 0xf0;
                entry->special = special;
                entry->events = NULL;
                entry->event_count = 0;
                return ++WORLD->special_sfx_count;
            }
        }
        return -1;
    }

    void UpdateSpecialSfx(WORLDINFO *world_info) {
        WORLDINFO *world = world_info;
        i32 i;
        i32 j;
        i32 direction;
        i32 play;
        SPECIAL_RESOURCE_ENTRY *entry;
        SPECIAL_RESOURCE_EVENT *event;
        NUINSTANIM *animation;
        NUVEC *position;
        f32 end_frame;
        f32 frame;
        u8 flags;
        for (i = 0; i < world->special_sfx_count; ++i) {
            entry = &world->special_sfx[i];
            event = entry->events;
            if (NuSpecialExistsFn(&entry->special) == 0 || NuSpecialGetVisibilityFn(&entry->special) == 0) {
                continue;
            }
            animation = (NUINSTANIM *)NuSpecialGetInstAnim(&entry->special);
            if (animation == NULL) {
                continue;
            }
            if (entry->special.scene->instance_animation_data[animation->animation_index] == NULL) {
                continue;
            }
            end_frame = NuAnimEndFrameOld(entry->special.scene->instance_animation_data[animation->animation_index]);
            frame = animation->ltime;
            if ((animation->flags & NUINSTANIM_FLAG_PLAYING) == 0 && entry->animation_playing == 0) {
                for (j = 0; j < entry->event_count; ++j) {
                    if (event != NULL) {
                        event->flags &= ~1u;
                        if ((event->flags & SPECIALSFX_EVENT_LOOPING) == 0 && frame < end_frame) {
                            event->previous_frame = 0.0f;
                        }
                    }
                    event = event->next;
                }
                entry->animation_playing = 0;
                continue;
            }
            position = NuSpecialGetDrawPos(&entry->special);
            direction = GetAnimDirection(animation);
            for (j = 0; j < entry->event_count; ++j) {
                if (event != NULL) {
                    play = 0;
                    flags = event->flags;
                    if ((flags & SPECIALSFX_EVENT_LOOPING) != 0) {
                        if ((direction == 0 && (flags & SPECIALSFX_EVENT_FORWARD) != 0) ||
                            (flags & SPECIALSFX_EVENT_FORWARD) != 0 && (flags & SPECIALSFX_EVENT_BACKWARD) != 0) {
                            if (event->trigger_frame <= frame && frame <= event->previous_frame) {
                                play = 1;
                            }
                        } else if (direction == 1 && (flags & SPECIALSFX_EVENT_BACKWARD) != 0 && frame <= event->trigger_frame &&
                                   frame >= event->previous_frame) {
                            play = 1;
                        }
                        // MATCH-EXCEPTION: bit 2 is read as (flags >> 2) & 1, the code VC8 emits for a one-bit
                        // bitfield member; the event flags are a plain u8 in the header
                    } else if (((flags >> 2) & 1) != 0 && (flags & SPECIALSFX_EVENT_BACKWARD) != 0) {
                        if ((event->previous_frame < event->trigger_frame && event->trigger_frame <= frame) ||
                            (event->previous_frame > event->trigger_frame && frame <= event->trigger_frame)) {
                            play = 1;
                        }
                    } else if (((flags >> 2) & 1) != 0) {
                        if ((event->previous_frame < event->trigger_frame && event->trigger_frame <= frame) ||
                            (event->previous_frame == 0.0f && event->trigger_frame == 1.0f)) {
                            play = 1;
                        }
                    } else if ((flags & SPECIALSFX_EVENT_BACKWARD) != 0) {
                        if ((event->previous_frame > event->trigger_frame && frame <= event->trigger_frame) ||
                            (event->previous_frame == end_frame && event->trigger_frame == end_frame)) {
                            play = 1;
                        }
                    }
                    if (play != 0) {
                        GameAudio_PlaySfxById(event->sfx_id, position, 0, 0);
                    }
                    if ((event->flags & SPECIALSFX_EVENT_LOOPING) == 0) {
                        event->previous_frame = frame;
                    }
                }
                event = event->next;
            }
            entry->animation_playing = (i8)(animation->flags & NUINSTANIM_FLAG_PLAYING);
        }
    }

    void SetSpecialSfxBits(i32 *ids, i32 *count, WORLDINFO *world) {
        i32 i;
        if (world == NULL || world->special_sfx_event_count <= 0) {
            return;
        }
        for (i = 0; i < world->special_sfx_event_count; ++i) {
            i32 id = world->special_sfx_events[i].sfx_id;
            if (id != -1) {
                ids[(*count)++] = id;
            }
        }
    }

    void LoadSpecialSfxFile(void *world_ptr) {
        SPECIAL_RESOURCE_WORLD *world = (SPECIAL_RESOURCE_WORLD *)world_ptr;
        char path[256];
        sprintf(path, "%s.sfx", world->config_file);
        if (NuFileExists(path)) {
            SpecialSfxLoad(path, world);
        }
    }
}
