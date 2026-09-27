#include "saga.h"

#include <string.h>

void *__cdecl AISysBufferAlloc(VARIPTR *cursor, VARIPTR *end, u32 size) {
    u32 aligned;

    if (cursor == NULL || end == NULL || cursor->addr + size >= end->addr) {
        return NULL;
    }
    aligned = (cursor->addr + 15) & ~15u;
    cursor->addr = aligned + size;
    memset((void *)aligned, 0, size);
    return (void *)aligned;
}

void GameAISysSetGame(void) {
    /* PC selects one of three game-specific AI initializers from global state. */
    STUBBED_NAMED("GameAISysSetGame: PC game selection and dispatch");
}

void GameAISysInit(void) {
    /*
     * PC registers the AI callback tables, initializes their defaults, and
     * finishes by dispatching through GameAISysSetGame. Those callback setters
     * and globals are not reconstructed yet.
     */
    STUBBED_NAMED("GameAISysInit: AI callback registration and defaults");
    GameAISysSetGame();
}
