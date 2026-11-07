#include "saga.h"

/* The game's ThingManager (retail 0x5b15e0-0x5b1750). LoadPermData creates it
 * with four slots; the PC build registers no things in it. */

extern "C" {
    GameThingManager *GameThingManagerPc = NULL;
}

GameThingManager::GameThingManager(i32 max_things) : ThingManager(max_things) {
    GameThingManagerPc = this;
}

GameThingManager::~GameThingManager() {
    GameThingManagerPc = NULL;
}

void GameThingManager::AddOnceOnlyThings() {
}

void CreateThingManager() {
    GameThingManagerPc = new GameThingManager(4);
}
