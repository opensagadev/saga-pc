#include "saga.h"

static void ZeroLightSetupPrivatePc(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1,
                                    NUVEC *direction2, NUVEC *colour2) {
    PCGlobalLightingState.direction[0] = *direction0;
    PCGlobalLightingState.direction[1] = *direction1;
    PCGlobalLightingState.direction[2] = *direction2;
    PCGlobalLightingState.intensity[0] = *colour0;
    PCGlobalLightingState.intensity[1] = *colour1;
    PCGlobalLightingState.intensity[2] = *colour2;
    NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
}
void __cdecl SetZeroLights(void) {
    NUVEC black = {0, 0, 0};
    ZeroLightSetupPrivatePc(&LightingUnitDirectionPc, &black, &LightingUnitDirectionPc, &black,
                            &LightingUnitDirectionPc, &black);
    PCGlobalLightingState.ambient = APIObjectZeroMomentumPC;
    NuRndrSetAmbientLightPS(&APIObjectZeroMomentumPC);
}

void __cdecl FindAndSetLights(NUVEC *position, f32 scale, void *set) {
    RTL_DATA_PC lights;
    rtlResetEx(&lights, 1);
    rtlDynamicMasterEnable(0);
    rtlApplySetScale(set, &lights, position, NULL, -1, scale);
    rtlDynamicMasterEnable(1);
    rtlSetLights(&lights);
}
void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set) {
    rtlResetEx(packet, 1);
    if (position != NULL)
        rtlApplySetScale(set, packet, position, NULL, -1, 1.0f);
}

typedef struct startup_object_light_pc_s {
    u8 unknown_000[0x1fc];
    u32 flags;
    u8 unknown_200[0xc04];
    i32 dynamic_light_id;
    u8 unknown_e08[0x2d0];
} STARTUP_OBJECT_LIGHT_PC;
STATIC_ASSERT(sizeof(STARTUP_OBJECT_LIGHT_PC) == sizeof(GAMEOBJECT));
STATIC_ASSERT(offsetof(STARTUP_OBJECT_LIGHT_PC, flags) == 0x1fc);
STATIC_ASSERT(offsetof(STARTUP_OBJECT_LIGHT_PC, dynamic_light_id) == 0xe04);
STATIC_ASSERT(offsetof(NULNKHDR, id) == 0xc);
i32 __cdecl rtlDynamicAlloc(void);
i32 __cdecl rtlDynamicSetType(i32 id, i32 type);
void __cdecl InitGameObjectLights(void) {
    STARTUP_OBJECT_LIGHT_PC *objects = (STARTUP_OBJECT_LIGHT_PC *)Obj;
    i32 index;
    for (index = 0; index < 64; ++index)
        objects[index].dynamic_light_id = -1;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        if ((objects[index].flags & 0x1001) == 0x1001) {
            objects[index].dynamic_light_id = rtlDynamicAlloc();
            if (objects[index].dynamic_light_id != -1) {
                rtlDynamicSetType(objects[index].dynamic_light_id, 2);
                rtlDynamicEnable(objects[index].dynamic_light_id, 0);
            }
        }
    }
}
