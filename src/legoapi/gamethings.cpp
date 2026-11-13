#include "saga.h"

/* The game's ThingManager (retail 0x5b15e0-0x5b1750). LoadPermData creates it
 * with four slots; the PC build registers no things in it. */

extern "C" {
    GameThingManager *theGameThings = NULL;
}

GameThingManager::GameThingManager(i32 max_things) : ThingManager(max_things) {
    theGameThings = this;
}

GameThingManager::~GameThingManager() {
    theGameThings = NULL;
}

void GameThingManager::AddOnceOnlyThings() {
}

void CreateThingManager() {
    theGameThings = new GameThingManager(4);
}
