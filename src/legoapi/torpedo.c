#include "saga.h"
typedef struct torpedo_pickup_positions_pc_s {
    u8 count;
    u8 unknown_01[0x3b];
    NUVEC pickup_positions[5];
} TORPEDO_PICKUP_POSITIONS_PC;
STATIC_ASSERT(offsetof(TORPEDO_PICKUP_POSITIONS_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_PICKUP_POSITIONS_PC, pickup_positions) == 0x3c);
void __cdecl AddPickups(i32, i32, i32, i32, NUVEC *, NUVEC *, f32, i32, f32, f32, GAMEOBJECT *, i32, i32);
void __cdecl DropTorpedoPickups(TORPEDOPACKET_PC *packet, i32 count) {
    TORPEDO_PICKUP_POSITIONS_PC *view = (TORPEDO_PICKUP_POSITIONS_PC *)packet;
    i32 index;
    if (packet == NULL || count == 0)
        return;
    if (count > packet->count)
        count = packet->count;
    else if (count < 0)
        return;
    for (index = 1; index <= count; ++index)
        AddPickups(0, 0, 1, 0, &view->pickup_positions[packet->count - index], &v010, 5.0f, -1, 1.0f, 2000000.0f, NULL,
                   1, 0);
    packet->count = (u8)(packet->count - count);
}

i32 __cdecl getMaxTorpedos(GAMEOBJECT *object) {
    if (object != NULL &&
        (object->id == (u16)id_YWING || object->id == (u16)id_MINIYWING || object->id == (u16)id_TIEBOMBER ||
         object->id == (u16)id_MINITIEBOMBER || object->id == (u16)id_MINISTARDESTROYER))
        return 5;
    return 3;
}
