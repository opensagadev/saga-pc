#include "saga.h"

i32 __cdecl PCDeviceSetCursorPositionPc(i32 x, i32 y);

void __cdecl NuMouseRead(void) {
}

u32 NuMouseReadButtons(void) {
    return PcMouseButtonsMaskPc;
}

f32 __cdecl NuMouseReadX(void) {
    return PcMouseX;
}

f32 __cdecl NuMouseReadY(void) {
    return PcMouseY;
}

void __cdecl PcEditorCursorEnablePc(i32 enabled) {
}

void __cdecl PcGetMousePositionPc(f32 *x, f32 *y) {
    if (x != NULL)
        *x = PcMouseX;
    if (y != NULL)
        *y = PcMouseY;
}

/* Moves the OS cursor to a normalised client position. */
void __cdecl PcSetMousePositionPc(f32 x, f32 y) {
    i32 client_x;
    i32 client_y;
    PcMouseX = x;
    PcMouseY = y;
    client_x = (i32)(x * (f32)PcClientWidth);
    client_y = (i32)(y * (f32)PcClientHeight);
    SetCursorPos(client_x, client_y);
    PCDeviceSetCursorPositionPc(client_x, client_y);
}
