#include "saga.h"
typedef struct lose_coins_packet_pc_s {
    u32 coins;
} LOSE_COINS_PACKET_PC;
STATIC_ASSERT(offsetof(LOSE_COINS_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, coin_packet) == 0x7fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
i32 __cdecl LoseCoins(GAMEOBJECT *object, i32 cause) {
    LOSE_COINS_PACKET_PC *packet;
    u32 lost = 0;
    i32 adjustment;
    if (Player_HasInvincibility(object) || (object->character_data->game_character->flags_090 & 0x8000) ||
        object->coin_packet == NULL)
        return 0;
    if (cause == 1)
        lost = BonusArea != 0 ? 25000 : 2000;
    else if (cause == 2)
        lost = BonusArea != 0 ? 12500 : 1000;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][0];
        if (adjustment == 1)
            lost += lost;
        else if (adjustment == -1)
            lost >>= 1;
    }
    packet = (LOSE_COINS_PACKET_PC *)object->coin_packet;
    if (lost > packet->coins) {
        lost = packet->coins;
        packet->coins = 0;
    } else if (lost > 0)
        packet->coins -= lost;
    if (Cheats_CheckFlags(0x7c) || DoubleScoreTime > 0.0f)
        return 0;
    return (i32)lost;
}
