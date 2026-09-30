#include "saga.h"

MISSIONDATA_PC *__cdecl Mission_Active(MISSIONSYS_PC *system) {
    if (system == NULL) {
        system = MissionSys;
        if (system == NULL)
            return NULL;
    }
    if (system->state == 0)
        return NULL;
    return system->active_mission;
}
