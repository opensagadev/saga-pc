#include "saga.h"

i32 __cdecl Hub_Outside(void) {
    return WORLD->current_level == HUB_LDATA && HubAiMusicStatePc.area == 7;
}
