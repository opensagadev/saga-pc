#include "saga.h"

extern i32 NuMainSkipCutBordersPc;
extern i32 AutoSaveIconPendingPc;

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 6 * sizeof(void *) == 0x80);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if (HUB_ADATA != NULL && world->area == HUB_ADATA)
        return 1;
    if (SuperStory != 0)
        return 1;
    if (world->area != NULL && (world->area->flags & 0x100))
        return 1;
    if (world->current_level != NULL && (world->current_level->flags & 0x400))
        return 1;
    return 0;
}

STATIC_ASSERT(offsetof(HINT_PC, id) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(sizeof(((GAMEOBJECT *)0)->flags_1fc_word) == 4);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, target) == 0x84);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
