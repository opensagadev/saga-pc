#pragma once

#include "nu2api/nucore/common.h"

struct TRAFFICANIMSYS_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 traffic_portalling;
    void TrafficAnimSys_Reset(struct TRAFFICANIMSYS_s *system);
#ifdef __cplusplus
}
#endif
