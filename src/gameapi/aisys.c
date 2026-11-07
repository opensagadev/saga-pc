#include "saga.h"

extern const char AIActionFalseName[6];

i32 __cdecl Action_PartyCanBeUnderCover(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time) {
        party_cant_be_under_cover = 0;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                party_cant_be_under_cover = 1;
        }
    }
    return 1;
}
