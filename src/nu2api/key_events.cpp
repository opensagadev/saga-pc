#include "saga.h"

extern "C" i32 QueueKeyPressPc(i32 scan_code, i32 modifiers) {
    if ((u8)(PcKeyQueueTail + 1) == PcKeyQueueHead)
        return 0;
    PcKeyQueueScanCodes[PcKeyQueueTail] = (u8)scan_code;
    PcKeyQueueModifiers[PcKeyQueueTail] = (u8)modifiers;
    ++PcKeyQueueTail;
    return 1;
}
extern "C" void PcKeyDown(i32 scan_code) {
    PcPressedScanCodes[scan_code] = 1;
    i32 modifiers = 0;
    if (PcPressedScanCodes[42] || PcPressedScanCodes[54])
        modifiers = 1;
    if (PcPressedScanCodes[29] || PcPressedScanCodes[157])
        modifiers |= 2;
    if (PcPressedScanCodes[184])
        modifiers |= 4;
    switch (scan_code) {
        case 29:
        case 42:
        case 54:
        case 56:
        case 157:
        case 184:
            return;
        default:
            QueueKeyPressPc(scan_code, modifiers);
            break;
    }
}
extern "C" void PcKeyUp(i32 scan_code) {
    PcPressedScanCodes[scan_code] = 0;
}
