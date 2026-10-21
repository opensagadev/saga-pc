#include "saga.h"

#pragma const_seg(".rdata$NarrowSockLevelDeathStar2BPc")
const char NarrowSockLevelDeathStar2BPc[19] = "deathstar2battle_b";
#pragma const_seg(".rdata$NarrowSockLevelDeathStar2CPc")
const char NarrowSockLevelDeathStar2CPc[19] = "deathstar2battle_c";
#pragma const_seg()

#pragma data_seg(".data$NarrowSockExceptionsLswPc")
NARROWSOCKEXCEPTION_PC NarrowSockExceptionsLswPc[4] = {
    {"deathstarbattle_d", -1, 1, 67.5f, 999.0f},
    {NarrowSockLevelDeathStar2BPc, -1, 0, 67.5f, 999.0f},
    {NarrowSockLevelDeathStar2CPc, -1, 0, 30.0f, 999.0f},
    {NULL, 0, 0, 0.0f, 0.0f},
};
#pragma data_seg()
