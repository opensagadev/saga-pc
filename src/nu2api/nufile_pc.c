#include "saga.h"

i32 __cdecl NuFileUseHiresPC(void) {
    return PcWindowOptions.usehires;
}

void __cdecl NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject) {
    (void)device_id;
    (void)reboot_iop;
    (void)eject;
}
