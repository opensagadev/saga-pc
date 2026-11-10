#include "saga.h"
#include <math.h>
#include <string.h>
extern "C" {
    void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    extern f32 PushRumblePeriodPc;
    extern f32 PushRumbleWindowPc;
    extern f32 PushBlockOverlapShrinkPc;
    PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
}
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
