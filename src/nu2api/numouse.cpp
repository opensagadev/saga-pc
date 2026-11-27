#include "nu2api/PCInput.h"
#include "nu2api/nucore/numouse.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/nu2api_windows.h"

void NuMouseRead(void) {
}

u32 NuMouseReadButtons(void) {
    return mbuttons;
}

extern "C" u32 NuMouseButtonDown(i32 button) {
    if (button >= 0 && button < 4u) {
        return (1 << button) & mbuttons;
    }
    return 0;
}

extern "C" void fun_006d5940(void) {
}

f32 PcMouseX = 0.0f;

f32 NuMouseReadX(void) {
    return PcMouseX;
}

f32 PcMouseY = 0.0f;

f32 NuMouseReadY(void) {
    return PcMouseY;
}

void PcEditorCursorEnable(i32 enabled) {
}

void PcGetMousePosition(f32 *x, f32 *y) {
    if (x != NULL) {
        *x = PcMouseX;
    }
    if (y != NULL) {
        *y = PcMouseY;
    }
}

/* Moves the OS cursor to a normalised client position. */
void PcSetMousePosition(f32 x, f32 y) {
    i32 client_x;
    i32 client_y;
    PcMouseX = x;
    PcMouseY = y;
    client_x = (i32)(x * (f32)PcClientWidth);
    client_y = (i32)(y * (f32)PcClientHeight);
    SetCursorPos(client_x, client_y);
    PCDeviceSetCursorPosition(client_x, client_y);
}
