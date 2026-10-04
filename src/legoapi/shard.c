#include "saga.h"

typedef struct SHARD_LOST_OWNER_PC {
    u8 unknown_00[0x3c];
    u8 state_flags;
    u8 unknown_3d[3];
    f32 collection_time;
    GAMEOBJECT *collector;
    u8 unknown_48[0xc];
} SHARD_LOST_OWNER_PC;
STATIC_ASSERT(sizeof(SHARD_LOST_OWNER_PC) == 0x54);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, state_flags) == 0x3c);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, collection_time) == 0x40);
STATIC_ASSERT(offsetof(SHARD_LOST_OWNER_PC, collector) == 0x44);
typedef struct WORLD_SHARDS_READER_PC {
    u8 unknown_0000[0x5090];
    SHARD_LOST_OWNER_PC *shards;
    i32 shard_count;
} WORLD_SHARDS_READER_PC;
STATIC_ASSERT(offsetof(WORLD_SHARDS_READER_PC, shards) == 0x5090);
STATIC_ASSERT(offsetof(WORLD_SHARDS_READER_PC, shard_count) == 0x5094);
void __cdecl Shards_HandleLostObj(WORLDINFO *opaque_world, GAMEOBJECT *object) {
    WORLD_SHARDS_READER_PC *world = (WORLD_SHARDS_READER_PC *)opaque_world;
    SHARD_LOST_OWNER_PC *shard = world->shards;
    i32 i;
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        if ((shard->state_flags & 0xc) == 4 && shard->collector == object) {
            shard->state_flags &= (u8)~4;
            shard->collection_time = 0.0f;
            shard->collector = NULL;
        }
    }
}
