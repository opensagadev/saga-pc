#include "saga.h"

struct zipup_pc_s {
    u8 unknown_00[0x10];
    NUVEC endpoints[3];
    u8 unknown_34[0x34];
    union {
        u16 flags_word;
        struct {
            u8 flags, runtime_flags;
        };
    };
    u8 unknown_6a[0x12];
};
STATIC_ASSERT(sizeof(ZIPUP_PC) == 0x7c);
STATIC_ASSERT(offsetof(ZIPUP_PC, endpoints) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_PC, flags_word) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_PC, flags) == 0x68);
ZIPUP_PC *__cdecl ZipUp_FindNearest(WORLDINFO *world, NUVEC *position, f32 radius, f32 *distance, i32 *endpoint,
                                    GAMEOBJECT *object) {
    ZIPUP_PC *zipup = world->zipups;
    ZIPUP_PC *nearest = NULL;
    i32 index, end;
    f32 square_distance, best_distance;
    if (zipup == NULL)
        return NULL;
    best_distance = radius + 0.25;
    best_distance *= best_distance;
    for (index = 0; index < world->zipup_count; ++index, ++zipup) {
        if (object != NULL) {
            if ((zipup->flags & 0xc0) != 0xc0)
                continue;
            if ((zipup->flags & 0x20) != 0) {
                if ((object->character_data->game_character->flags_094 & 0x20000000) == 0)
                    continue;
            } else {
                if ((object->character_data->game_character->flags_094 & 0x20000000) != 0)
                    continue;
                if ((zipup->flags & 2) == 0 && object->field_280 != 0xff)
                    continue;
            }
        }
        for (end = 0; end < 3; ++end) {
            if (object != NULL && (end == 1 || (end == 2 && (zipup->flags & 8) == 0)))
                continue;
            square_distance = NuVecDistSqr(position, &zipup->endpoints[end], NULL);
            if (best_distance > square_distance) {
                best_distance = square_distance;
                nearest = zipup;
                if (endpoint != NULL)
                    *endpoint = end;
            }
        }
    }
    if (distance != NULL)
        *distance = best_distance;
    return nearest;
}
