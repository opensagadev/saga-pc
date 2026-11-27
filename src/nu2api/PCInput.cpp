#include "nu2api/PCInput.h"
#include <WbemIdl.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/world/levels.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/platform_files.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edtoolsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/light/lighting.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/nucore_plain_types.h"
#include "nu2api/nucore/numouse.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "legogame/pcbatman.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "pc_fmv_native.h"

/* C view of PC_INPUT_RUNTIME (a C++ class): devices[].action_keys_active and captured_port. */
typedef struct NuKeyboardActionRecord { // size 0x17c
    u8 unknown_00_to_27[0x28];          // 0x000
    u8 active;                          // 0x028
    u8 unknown_29_to_17b[0x17c - 0x29]; // 0x029
} NuKeyboardActionRecord;

typedef struct NuKeyboardActionState {        // size 0x660c
    u8 unknown_0000_to_4e33[0x4e34];          // 0x0000
    NuKeyboardActionRecord records[14];       // 0x4e34
    u8 unknown_62fc_to_6607[0x6608 - 0x62fc]; // 0x62fc
    i32 fallback_action;                      // 0x6608
} NuKeyboardActionState;

/* Append to nupad_text.cpp. Native6da3d0,51 bytes all stack cdecl. */
extern "C" char TextInputActionLabelBuffer[128]; /*2975970..29759ef*/
extern const GUID PcWbemClassId = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern const GUID PcWbemInterfaceId = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" {
    extern const u8 PcInputPovBits[8] = {1, 3, 2, 6, 4, 12, 8, 9};
}

struct PC_DI_JOYSTICK_STATE { // size 0x110
    i32 axes[8];              // 0x000
    i32 pov[4];               // 0x020
    u8 buttons[128];          // 0x030
    i32 extended_axes[24];    // 0x0b0
};

// Uncompiled drafts retaining native mouse accumulation and cursor behavior.
extern u32 mbuttons;
// Uncompiled PC control-flow reconstruction. All indexed records require native initialized domains.
extern u8 PcMouseAxisModes[8];
extern "C" const char *PcKeyboardLayoutNames[256];
extern "C" char PcKeyboardLayoutText[512];

// Append to input_runtime.cpp. Extend config unknown_44 with metadata named in B-layout.txt.
struct PC_INPUT_EXPORT_METADATA { // size 0xc
    i16 button_count;             // 0x0
    i16 pov_count;                // 0x2
    i16 axis_count;               // 0x4
    u16 deadzone_word;            // 0x6
    u16 alternate_deadzone_word;  // 0x8
    i16 key;                      // 0xa
};

extern "C" f32 FmvOverlayTimer;
extern "C" {
    extern const char PCKeyboardMouse2Name[16] = "KeyboardMouse2";
}

/* DIJOYSTATE2 (dinput.h): the joystick state whose axes ConfigureJoystickRanges gives a range, by offset. */
struct PC_DI_JOYSTATE2 {          // size 0x110
    LONG x;                       // 0x000
    LONG y;                       // 0x004
    LONG z;                       // 0x008
    LONG rx;                      // 0x00c
    LONG ry;                      // 0x010
    LONG rz;                      // 0x014
    LONG sliders[2];              // 0x018
    DWORD pov[4];                 // 0x020
    BYTE buttons[128];            // 0x030
    LONG velocity_x;              // 0x0b0
    LONG velocity_y;              // 0x0b4
    LONG velocity_z;              // 0x0b8
    LONG angular_velocity_x;      // 0x0bc
    LONG angular_velocity_y;      // 0x0c0
    LONG angular_velocity_z;      // 0x0c4
    LONG velocity_sliders[2];     // 0x0c8
    LONG acceleration_x;          // 0x0d0
    LONG acceleration_y;          // 0x0d4
    LONG acceleration_z;          // 0x0d8
    LONG angular_acceleration_x;  // 0x0dc
    LONG angular_acceleration_y;  // 0x0e0
    LONG angular_acceleration_z;  // 0x0e4
    LONG acceleration_sliders[2]; // 0x0e8
    LONG force_x;                 // 0x0f0
    LONG force_y;                 // 0x0f4
    LONG force_z;                 // 0x0f8
    LONG torque_x;                // 0x0fc
    LONG torque_y;                // 0x100
    LONG torque_z;                // 0x104
    LONG force_sliders[2];        // 0x108
};

/* DirectInput's C++ device interface, as far as ConfigureJoystickRanges calls it. */
struct PC_DI_DEVICE_INTERFACE {
    virtual HRESULT __stdcall QueryInterface(REFIID, void **) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
    virtual HRESULT __stdcall GetCapabilities(PC_DI_CAPS *) = 0;
    virtual HRESULT __stdcall EnumObjects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall GetProperty(REFGUID, PC_DI_PROPERTY_HEADER *) = 0;
    virtual HRESULT __stdcall SetProperty(REFGUID, const PC_DI_PROPERTY_HEADER *) = 0;
};

extern const GUID PcDiGuidPov = {0xa36d02f2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidSlider = {0xa36d02e4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidRzAxis = {0xa36d02e3, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidRyAxis = {0xa36d02f5, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidRxAxis = {0xa36d02f4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidZAxis = {0xa36d02e2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidYAxis = {0xa36d02e1, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidXAxis = {0xa36d02e0, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDiGuidKey = {0x55728220, 0xd33c, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" {
    static const PC_DI_OBJECT_FORMAT PcKeyboardDataFormatObjects[256] = {
        {&PcDiGuidKey, 0x0, 0x8000000c, 0x0},  {&PcDiGuidKey, 0x1, 0x8000010c, 0x0},  {&PcDiGuidKey, 0x2, 0x8000020c, 0x0},
        {&PcDiGuidKey, 0x3, 0x8000030c, 0x0},  {&PcDiGuidKey, 0x4, 0x8000040c, 0x0},  {&PcDiGuidKey, 0x5, 0x8000050c, 0x0},
        {&PcDiGuidKey, 0x6, 0x8000060c, 0x0},  {&PcDiGuidKey, 0x7, 0x8000070c, 0x0},  {&PcDiGuidKey, 0x8, 0x8000080c, 0x0},
        {&PcDiGuidKey, 0x9, 0x8000090c, 0x0},  {&PcDiGuidKey, 0xa, 0x80000a0c, 0x0},  {&PcDiGuidKey, 0xb, 0x80000b0c, 0x0},
        {&PcDiGuidKey, 0xc, 0x80000c0c, 0x0},  {&PcDiGuidKey, 0xd, 0x80000d0c, 0x0},  {&PcDiGuidKey, 0xe, 0x80000e0c, 0x0},
        {&PcDiGuidKey, 0xf, 0x80000f0c, 0x0},  {&PcDiGuidKey, 0x10, 0x8000100c, 0x0}, {&PcDiGuidKey, 0x11, 0x8000110c, 0x0},
        {&PcDiGuidKey, 0x12, 0x8000120c, 0x0}, {&PcDiGuidKey, 0x13, 0x8000130c, 0x0}, {&PcDiGuidKey, 0x14, 0x8000140c, 0x0},
        {&PcDiGuidKey, 0x15, 0x8000150c, 0x0}, {&PcDiGuidKey, 0x16, 0x8000160c, 0x0}, {&PcDiGuidKey, 0x17, 0x8000170c, 0x0},
        {&PcDiGuidKey, 0x18, 0x8000180c, 0x0}, {&PcDiGuidKey, 0x19, 0x8000190c, 0x0}, {&PcDiGuidKey, 0x1a, 0x80001a0c, 0x0},
        {&PcDiGuidKey, 0x1b, 0x80001b0c, 0x0}, {&PcDiGuidKey, 0x1c, 0x80001c0c, 0x0}, {&PcDiGuidKey, 0x1d, 0x80001d0c, 0x0},
        {&PcDiGuidKey, 0x1e, 0x80001e0c, 0x0}, {&PcDiGuidKey, 0x1f, 0x80001f0c, 0x0}, {&PcDiGuidKey, 0x20, 0x8000200c, 0x0},
        {&PcDiGuidKey, 0x21, 0x8000210c, 0x0}, {&PcDiGuidKey, 0x22, 0x8000220c, 0x0}, {&PcDiGuidKey, 0x23, 0x8000230c, 0x0},
        {&PcDiGuidKey, 0x24, 0x8000240c, 0x0}, {&PcDiGuidKey, 0x25, 0x8000250c, 0x0}, {&PcDiGuidKey, 0x26, 0x8000260c, 0x0},
        {&PcDiGuidKey, 0x27, 0x8000270c, 0x0}, {&PcDiGuidKey, 0x28, 0x8000280c, 0x0}, {&PcDiGuidKey, 0x29, 0x8000290c, 0x0},
        {&PcDiGuidKey, 0x2a, 0x80002a0c, 0x0}, {&PcDiGuidKey, 0x2b, 0x80002b0c, 0x0}, {&PcDiGuidKey, 0x2c, 0x80002c0c, 0x0},
        {&PcDiGuidKey, 0x2d, 0x80002d0c, 0x0}, {&PcDiGuidKey, 0x2e, 0x80002e0c, 0x0}, {&PcDiGuidKey, 0x2f, 0x80002f0c, 0x0},
        {&PcDiGuidKey, 0x30, 0x8000300c, 0x0}, {&PcDiGuidKey, 0x31, 0x8000310c, 0x0}, {&PcDiGuidKey, 0x32, 0x8000320c, 0x0},
        {&PcDiGuidKey, 0x33, 0x8000330c, 0x0}, {&PcDiGuidKey, 0x34, 0x8000340c, 0x0}, {&PcDiGuidKey, 0x35, 0x8000350c, 0x0},
        {&PcDiGuidKey, 0x36, 0x8000360c, 0x0}, {&PcDiGuidKey, 0x37, 0x8000370c, 0x0}, {&PcDiGuidKey, 0x38, 0x8000380c, 0x0},
        {&PcDiGuidKey, 0x39, 0x8000390c, 0x0}, {&PcDiGuidKey, 0x3a, 0x80003a0c, 0x0}, {&PcDiGuidKey, 0x3b, 0x80003b0c, 0x0},
        {&PcDiGuidKey, 0x3c, 0x80003c0c, 0x0}, {&PcDiGuidKey, 0x3d, 0x80003d0c, 0x0}, {&PcDiGuidKey, 0x3e, 0x80003e0c, 0x0},
        {&PcDiGuidKey, 0x3f, 0x80003f0c, 0x0}, {&PcDiGuidKey, 0x40, 0x8000400c, 0x0}, {&PcDiGuidKey, 0x41, 0x8000410c, 0x0},
        {&PcDiGuidKey, 0x42, 0x8000420c, 0x0}, {&PcDiGuidKey, 0x43, 0x8000430c, 0x0}, {&PcDiGuidKey, 0x44, 0x8000440c, 0x0},
        {&PcDiGuidKey, 0x45, 0x8000450c, 0x0}, {&PcDiGuidKey, 0x46, 0x8000460c, 0x0}, {&PcDiGuidKey, 0x47, 0x8000470c, 0x0},
        {&PcDiGuidKey, 0x48, 0x8000480c, 0x0}, {&PcDiGuidKey, 0x49, 0x8000490c, 0x0}, {&PcDiGuidKey, 0x4a, 0x80004a0c, 0x0},
        {&PcDiGuidKey, 0x4b, 0x80004b0c, 0x0}, {&PcDiGuidKey, 0x4c, 0x80004c0c, 0x0}, {&PcDiGuidKey, 0x4d, 0x80004d0c, 0x0},
        {&PcDiGuidKey, 0x4e, 0x80004e0c, 0x0}, {&PcDiGuidKey, 0x4f, 0x80004f0c, 0x0}, {&PcDiGuidKey, 0x50, 0x8000500c, 0x0},
        {&PcDiGuidKey, 0x51, 0x8000510c, 0x0}, {&PcDiGuidKey, 0x52, 0x8000520c, 0x0}, {&PcDiGuidKey, 0x53, 0x8000530c, 0x0},
        {&PcDiGuidKey, 0x54, 0x8000540c, 0x0}, {&PcDiGuidKey, 0x55, 0x8000550c, 0x0}, {&PcDiGuidKey, 0x56, 0x8000560c, 0x0},
        {&PcDiGuidKey, 0x57, 0x8000570c, 0x0}, {&PcDiGuidKey, 0x58, 0x8000580c, 0x0}, {&PcDiGuidKey, 0x59, 0x8000590c, 0x0},
        {&PcDiGuidKey, 0x5a, 0x80005a0c, 0x0}, {&PcDiGuidKey, 0x5b, 0x80005b0c, 0x0}, {&PcDiGuidKey, 0x5c, 0x80005c0c, 0x0},
        {&PcDiGuidKey, 0x5d, 0x80005d0c, 0x0}, {&PcDiGuidKey, 0x5e, 0x80005e0c, 0x0}, {&PcDiGuidKey, 0x5f, 0x80005f0c, 0x0},
        {&PcDiGuidKey, 0x60, 0x8000600c, 0x0}, {&PcDiGuidKey, 0x61, 0x8000610c, 0x0}, {&PcDiGuidKey, 0x62, 0x8000620c, 0x0},
        {&PcDiGuidKey, 0x63, 0x8000630c, 0x0}, {&PcDiGuidKey, 0x64, 0x8000640c, 0x0}, {&PcDiGuidKey, 0x65, 0x8000650c, 0x0},
        {&PcDiGuidKey, 0x66, 0x8000660c, 0x0}, {&PcDiGuidKey, 0x67, 0x8000670c, 0x0}, {&PcDiGuidKey, 0x68, 0x8000680c, 0x0},
        {&PcDiGuidKey, 0x69, 0x8000690c, 0x0}, {&PcDiGuidKey, 0x6a, 0x80006a0c, 0x0}, {&PcDiGuidKey, 0x6b, 0x80006b0c, 0x0},
        {&PcDiGuidKey, 0x6c, 0x80006c0c, 0x0}, {&PcDiGuidKey, 0x6d, 0x80006d0c, 0x0}, {&PcDiGuidKey, 0x6e, 0x80006e0c, 0x0},
        {&PcDiGuidKey, 0x6f, 0x80006f0c, 0x0}, {&PcDiGuidKey, 0x70, 0x8000700c, 0x0}, {&PcDiGuidKey, 0x71, 0x8000710c, 0x0},
        {&PcDiGuidKey, 0x72, 0x8000720c, 0x0}, {&PcDiGuidKey, 0x73, 0x8000730c, 0x0}, {&PcDiGuidKey, 0x74, 0x8000740c, 0x0},
        {&PcDiGuidKey, 0x75, 0x8000750c, 0x0}, {&PcDiGuidKey, 0x76, 0x8000760c, 0x0}, {&PcDiGuidKey, 0x77, 0x8000770c, 0x0},
        {&PcDiGuidKey, 0x78, 0x8000780c, 0x0}, {&PcDiGuidKey, 0x79, 0x8000790c, 0x0}, {&PcDiGuidKey, 0x7a, 0x80007a0c, 0x0},
        {&PcDiGuidKey, 0x7b, 0x80007b0c, 0x0}, {&PcDiGuidKey, 0x7c, 0x80007c0c, 0x0}, {&PcDiGuidKey, 0x7d, 0x80007d0c, 0x0},
        {&PcDiGuidKey, 0x7e, 0x80007e0c, 0x0}, {&PcDiGuidKey, 0x7f, 0x80007f0c, 0x0}, {&PcDiGuidKey, 0x80, 0x8000800c, 0x0},
        {&PcDiGuidKey, 0x81, 0x8000810c, 0x0}, {&PcDiGuidKey, 0x82, 0x8000820c, 0x0}, {&PcDiGuidKey, 0x83, 0x8000830c, 0x0},
        {&PcDiGuidKey, 0x84, 0x8000840c, 0x0}, {&PcDiGuidKey, 0x85, 0x8000850c, 0x0}, {&PcDiGuidKey, 0x86, 0x8000860c, 0x0},
        {&PcDiGuidKey, 0x87, 0x8000870c, 0x0}, {&PcDiGuidKey, 0x88, 0x8000880c, 0x0}, {&PcDiGuidKey, 0x89, 0x8000890c, 0x0},
        {&PcDiGuidKey, 0x8a, 0x80008a0c, 0x0}, {&PcDiGuidKey, 0x8b, 0x80008b0c, 0x0}, {&PcDiGuidKey, 0x8c, 0x80008c0c, 0x0},
        {&PcDiGuidKey, 0x8d, 0x80008d0c, 0x0}, {&PcDiGuidKey, 0x8e, 0x80008e0c, 0x0}, {&PcDiGuidKey, 0x8f, 0x80008f0c, 0x0},
        {&PcDiGuidKey, 0x90, 0x8000900c, 0x0}, {&PcDiGuidKey, 0x91, 0x8000910c, 0x0}, {&PcDiGuidKey, 0x92, 0x8000920c, 0x0},
        {&PcDiGuidKey, 0x93, 0x8000930c, 0x0}, {&PcDiGuidKey, 0x94, 0x8000940c, 0x0}, {&PcDiGuidKey, 0x95, 0x8000950c, 0x0},
        {&PcDiGuidKey, 0x96, 0x8000960c, 0x0}, {&PcDiGuidKey, 0x97, 0x8000970c, 0x0}, {&PcDiGuidKey, 0x98, 0x8000980c, 0x0},
        {&PcDiGuidKey, 0x99, 0x8000990c, 0x0}, {&PcDiGuidKey, 0x9a, 0x80009a0c, 0x0}, {&PcDiGuidKey, 0x9b, 0x80009b0c, 0x0},
        {&PcDiGuidKey, 0x9c, 0x80009c0c, 0x0}, {&PcDiGuidKey, 0x9d, 0x80009d0c, 0x0}, {&PcDiGuidKey, 0x9e, 0x80009e0c, 0x0},
        {&PcDiGuidKey, 0x9f, 0x80009f0c, 0x0}, {&PcDiGuidKey, 0xa0, 0x8000a00c, 0x0}, {&PcDiGuidKey, 0xa1, 0x8000a10c, 0x0},
        {&PcDiGuidKey, 0xa2, 0x8000a20c, 0x0}, {&PcDiGuidKey, 0xa3, 0x8000a30c, 0x0}, {&PcDiGuidKey, 0xa4, 0x8000a40c, 0x0},
        {&PcDiGuidKey, 0xa5, 0x8000a50c, 0x0}, {&PcDiGuidKey, 0xa6, 0x8000a60c, 0x0}, {&PcDiGuidKey, 0xa7, 0x8000a70c, 0x0},
        {&PcDiGuidKey, 0xa8, 0x8000a80c, 0x0}, {&PcDiGuidKey, 0xa9, 0x8000a90c, 0x0}, {&PcDiGuidKey, 0xaa, 0x8000aa0c, 0x0},
        {&PcDiGuidKey, 0xab, 0x8000ab0c, 0x0}, {&PcDiGuidKey, 0xac, 0x8000ac0c, 0x0}, {&PcDiGuidKey, 0xad, 0x8000ad0c, 0x0},
        {&PcDiGuidKey, 0xae, 0x8000ae0c, 0x0}, {&PcDiGuidKey, 0xaf, 0x8000af0c, 0x0}, {&PcDiGuidKey, 0xb0, 0x8000b00c, 0x0},
        {&PcDiGuidKey, 0xb1, 0x8000b10c, 0x0}, {&PcDiGuidKey, 0xb2, 0x8000b20c, 0x0}, {&PcDiGuidKey, 0xb3, 0x8000b30c, 0x0},
        {&PcDiGuidKey, 0xb4, 0x8000b40c, 0x0}, {&PcDiGuidKey, 0xb5, 0x8000b50c, 0x0}, {&PcDiGuidKey, 0xb6, 0x8000b60c, 0x0},
        {&PcDiGuidKey, 0xb7, 0x8000b70c, 0x0}, {&PcDiGuidKey, 0xb8, 0x8000b80c, 0x0}, {&PcDiGuidKey, 0xb9, 0x8000b90c, 0x0},
        {&PcDiGuidKey, 0xba, 0x8000ba0c, 0x0}, {&PcDiGuidKey, 0xbb, 0x8000bb0c, 0x0}, {&PcDiGuidKey, 0xbc, 0x8000bc0c, 0x0},
        {&PcDiGuidKey, 0xbd, 0x8000bd0c, 0x0}, {&PcDiGuidKey, 0xbe, 0x8000be0c, 0x0}, {&PcDiGuidKey, 0xbf, 0x8000bf0c, 0x0},
        {&PcDiGuidKey, 0xc0, 0x8000c00c, 0x0}, {&PcDiGuidKey, 0xc1, 0x8000c10c, 0x0}, {&PcDiGuidKey, 0xc2, 0x8000c20c, 0x0},
        {&PcDiGuidKey, 0xc3, 0x8000c30c, 0x0}, {&PcDiGuidKey, 0xc4, 0x8000c40c, 0x0}, {&PcDiGuidKey, 0xc5, 0x8000c50c, 0x0},
        {&PcDiGuidKey, 0xc6, 0x8000c60c, 0x0}, {&PcDiGuidKey, 0xc7, 0x8000c70c, 0x0}, {&PcDiGuidKey, 0xc8, 0x8000c80c, 0x0},
        {&PcDiGuidKey, 0xc9, 0x8000c90c, 0x0}, {&PcDiGuidKey, 0xca, 0x8000ca0c, 0x0}, {&PcDiGuidKey, 0xcb, 0x8000cb0c, 0x0},
        {&PcDiGuidKey, 0xcc, 0x8000cc0c, 0x0}, {&PcDiGuidKey, 0xcd, 0x8000cd0c, 0x0}, {&PcDiGuidKey, 0xce, 0x8000ce0c, 0x0},
        {&PcDiGuidKey, 0xcf, 0x8000cf0c, 0x0}, {&PcDiGuidKey, 0xd0, 0x8000d00c, 0x0}, {&PcDiGuidKey, 0xd1, 0x8000d10c, 0x0},
        {&PcDiGuidKey, 0xd2, 0x8000d20c, 0x0}, {&PcDiGuidKey, 0xd3, 0x8000d30c, 0x0}, {&PcDiGuidKey, 0xd4, 0x8000d40c, 0x0},
        {&PcDiGuidKey, 0xd5, 0x8000d50c, 0x0}, {&PcDiGuidKey, 0xd6, 0x8000d60c, 0x0}, {&PcDiGuidKey, 0xd7, 0x8000d70c, 0x0},
        {&PcDiGuidKey, 0xd8, 0x8000d80c, 0x0}, {&PcDiGuidKey, 0xd9, 0x8000d90c, 0x0}, {&PcDiGuidKey, 0xda, 0x8000da0c, 0x0},
        {&PcDiGuidKey, 0xdb, 0x8000db0c, 0x0}, {&PcDiGuidKey, 0xdc, 0x8000dc0c, 0x0}, {&PcDiGuidKey, 0xdd, 0x8000dd0c, 0x0},
        {&PcDiGuidKey, 0xde, 0x8000de0c, 0x0}, {&PcDiGuidKey, 0xdf, 0x8000df0c, 0x0}, {&PcDiGuidKey, 0xe0, 0x8000e00c, 0x0},
        {&PcDiGuidKey, 0xe1, 0x8000e10c, 0x0}, {&PcDiGuidKey, 0xe2, 0x8000e20c, 0x0}, {&PcDiGuidKey, 0xe3, 0x8000e30c, 0x0},
        {&PcDiGuidKey, 0xe4, 0x8000e40c, 0x0}, {&PcDiGuidKey, 0xe5, 0x8000e50c, 0x0}, {&PcDiGuidKey, 0xe6, 0x8000e60c, 0x0},
        {&PcDiGuidKey, 0xe7, 0x8000e70c, 0x0}, {&PcDiGuidKey, 0xe8, 0x8000e80c, 0x0}, {&PcDiGuidKey, 0xe9, 0x8000e90c, 0x0},
        {&PcDiGuidKey, 0xea, 0x8000ea0c, 0x0}, {&PcDiGuidKey, 0xeb, 0x8000eb0c, 0x0}, {&PcDiGuidKey, 0xec, 0x8000ec0c, 0x0},
        {&PcDiGuidKey, 0xed, 0x8000ed0c, 0x0}, {&PcDiGuidKey, 0xee, 0x8000ee0c, 0x0}, {&PcDiGuidKey, 0xef, 0x8000ef0c, 0x0},
        {&PcDiGuidKey, 0xf0, 0x8000f00c, 0x0}, {&PcDiGuidKey, 0xf1, 0x8000f10c, 0x0}, {&PcDiGuidKey, 0xf2, 0x8000f20c, 0x0},
        {&PcDiGuidKey, 0xf3, 0x8000f30c, 0x0}, {&PcDiGuidKey, 0xf4, 0x8000f40c, 0x0}, {&PcDiGuidKey, 0xf5, 0x8000f50c, 0x0},
        {&PcDiGuidKey, 0xf6, 0x8000f60c, 0x0}, {&PcDiGuidKey, 0xf7, 0x8000f70c, 0x0}, {&PcDiGuidKey, 0xf8, 0x8000f80c, 0x0},
        {&PcDiGuidKey, 0xf9, 0x8000f90c, 0x0}, {&PcDiGuidKey, 0xfa, 0x8000fa0c, 0x0}, {&PcDiGuidKey, 0xfb, 0x8000fb0c, 0x0},
        {&PcDiGuidKey, 0xfc, 0x8000fc0c, 0x0}, {&PcDiGuidKey, 0xfd, 0x8000fd0c, 0x0}, {&PcDiGuidKey, 0xfe, 0x8000fe0c, 0x0},
        {&PcDiGuidKey, 0xff, 0x8000ff0c, 0x0},
    };
}
extern "C" {
    static const PC_DI_OBJECT_FORMAT PcJoystickDataFormatObjects[164] = {
        {&PcDiGuidXAxis, 0x0, 0x80ffff03, 0x100},
        {&PcDiGuidYAxis, 0x4, 0x80ffff03, 0x100},
        {&PcDiGuidZAxis, 0x8, 0x80ffff03, 0x100},
        {&PcDiGuidRxAxis, 0xc, 0x80ffff03, 0x100},
        {&PcDiGuidRyAxis, 0x10, 0x80ffff03, 0x100},
        {&PcDiGuidRzAxis, 0x14, 0x80ffff03, 0x100},
        {&PcDiGuidSlider, 0x18, 0x80ffff03, 0x100},
        {&PcDiGuidSlider, 0x1c, 0x80ffff03, 0x100},
        {&PcDiGuidPov, 0x20, 0x80ffff10, 0x0},
        {&PcDiGuidPov, 0x24, 0x80ffff10, 0x0},
        {&PcDiGuidPov, 0x28, 0x80ffff10, 0x0},
        {&PcDiGuidPov, 0x2c, 0x80ffff10, 0x0},
        {NULL, 0x30, 0x80ffff0c, 0x0},
        {NULL, 0x31, 0x80ffff0c, 0x0},
        {NULL, 0x32, 0x80ffff0c, 0x0},
        {NULL, 0x33, 0x80ffff0c, 0x0},
        {NULL, 0x34, 0x80ffff0c, 0x0},
        {NULL, 0x35, 0x80ffff0c, 0x0},
        {NULL, 0x36, 0x80ffff0c, 0x0},
        {NULL, 0x37, 0x80ffff0c, 0x0},
        {NULL, 0x38, 0x80ffff0c, 0x0},
        {NULL, 0x39, 0x80ffff0c, 0x0},
        {NULL, 0x3a, 0x80ffff0c, 0x0},
        {NULL, 0x3b, 0x80ffff0c, 0x0},
        {NULL, 0x3c, 0x80ffff0c, 0x0},
        {NULL, 0x3d, 0x80ffff0c, 0x0},
        {NULL, 0x3e, 0x80ffff0c, 0x0},
        {NULL, 0x3f, 0x80ffff0c, 0x0},
        {NULL, 0x40, 0x80ffff0c, 0x0},
        {NULL, 0x41, 0x80ffff0c, 0x0},
        {NULL, 0x42, 0x80ffff0c, 0x0},
        {NULL, 0x43, 0x80ffff0c, 0x0},
        {NULL, 0x44, 0x80ffff0c, 0x0},
        {NULL, 0x45, 0x80ffff0c, 0x0},
        {NULL, 0x46, 0x80ffff0c, 0x0},
        {NULL, 0x47, 0x80ffff0c, 0x0},
        {NULL, 0x48, 0x80ffff0c, 0x0},
        {NULL, 0x49, 0x80ffff0c, 0x0},
        {NULL, 0x4a, 0x80ffff0c, 0x0},
        {NULL, 0x4b, 0x80ffff0c, 0x0},
        {NULL, 0x4c, 0x80ffff0c, 0x0},
        {NULL, 0x4d, 0x80ffff0c, 0x0},
        {NULL, 0x4e, 0x80ffff0c, 0x0},
        {NULL, 0x4f, 0x80ffff0c, 0x0},
        {NULL, 0x50, 0x80ffff0c, 0x0},
        {NULL, 0x51, 0x80ffff0c, 0x0},
        {NULL, 0x52, 0x80ffff0c, 0x0},
        {NULL, 0x53, 0x80ffff0c, 0x0},
        {NULL, 0x54, 0x80ffff0c, 0x0},
        {NULL, 0x55, 0x80ffff0c, 0x0},
        {NULL, 0x56, 0x80ffff0c, 0x0},
        {NULL, 0x57, 0x80ffff0c, 0x0},
        {NULL, 0x58, 0x80ffff0c, 0x0},
        {NULL, 0x59, 0x80ffff0c, 0x0},
        {NULL, 0x5a, 0x80ffff0c, 0x0},
        {NULL, 0x5b, 0x80ffff0c, 0x0},
        {NULL, 0x5c, 0x80ffff0c, 0x0},
        {NULL, 0x5d, 0x80ffff0c, 0x0},
        {NULL, 0x5e, 0x80ffff0c, 0x0},
        {NULL, 0x5f, 0x80ffff0c, 0x0},
        {NULL, 0x60, 0x80ffff0c, 0x0},
        {NULL, 0x61, 0x80ffff0c, 0x0},
        {NULL, 0x62, 0x80ffff0c, 0x0},
        {NULL, 0x63, 0x80ffff0c, 0x0},
        {NULL, 0x64, 0x80ffff0c, 0x0},
        {NULL, 0x65, 0x80ffff0c, 0x0},
        {NULL, 0x66, 0x80ffff0c, 0x0},
        {NULL, 0x67, 0x80ffff0c, 0x0},
        {NULL, 0x68, 0x80ffff0c, 0x0},
        {NULL, 0x69, 0x80ffff0c, 0x0},
        {NULL, 0x6a, 0x80ffff0c, 0x0},
        {NULL, 0x6b, 0x80ffff0c, 0x0},
        {NULL, 0x6c, 0x80ffff0c, 0x0},
        {NULL, 0x6d, 0x80ffff0c, 0x0},
        {NULL, 0x6e, 0x80ffff0c, 0x0},
        {NULL, 0x6f, 0x80ffff0c, 0x0},
        {NULL, 0x70, 0x80ffff0c, 0x0},
        {NULL, 0x71, 0x80ffff0c, 0x0},
        {NULL, 0x72, 0x80ffff0c, 0x0},
        {NULL, 0x73, 0x80ffff0c, 0x0},
        {NULL, 0x74, 0x80ffff0c, 0x0},
        {NULL, 0x75, 0x80ffff0c, 0x0},
        {NULL, 0x76, 0x80ffff0c, 0x0},
        {NULL, 0x77, 0x80ffff0c, 0x0},
        {NULL, 0x78, 0x80ffff0c, 0x0},
        {NULL, 0x79, 0x80ffff0c, 0x0},
        {NULL, 0x7a, 0x80ffff0c, 0x0},
        {NULL, 0x7b, 0x80ffff0c, 0x0},
        {NULL, 0x7c, 0x80ffff0c, 0x0},
        {NULL, 0x7d, 0x80ffff0c, 0x0},
        {NULL, 0x7e, 0x80ffff0c, 0x0},
        {NULL, 0x7f, 0x80ffff0c, 0x0},
        {NULL, 0x80, 0x80ffff0c, 0x0},
        {NULL, 0x81, 0x80ffff0c, 0x0},
        {NULL, 0x82, 0x80ffff0c, 0x0},
        {NULL, 0x83, 0x80ffff0c, 0x0},
        {NULL, 0x84, 0x80ffff0c, 0x0},
        {NULL, 0x85, 0x80ffff0c, 0x0},
        {NULL, 0x86, 0x80ffff0c, 0x0},
        {NULL, 0x87, 0x80ffff0c, 0x0},
        {NULL, 0x88, 0x80ffff0c, 0x0},
        {NULL, 0x89, 0x80ffff0c, 0x0},
        {NULL, 0x8a, 0x80ffff0c, 0x0},
        {NULL, 0x8b, 0x80ffff0c, 0x0},
        {NULL, 0x8c, 0x80ffff0c, 0x0},
        {NULL, 0x8d, 0x80ffff0c, 0x0},
        {NULL, 0x8e, 0x80ffff0c, 0x0},
        {NULL, 0x8f, 0x80ffff0c, 0x0},
        {NULL, 0x90, 0x80ffff0c, 0x0},
        {NULL, 0x91, 0x80ffff0c, 0x0},
        {NULL, 0x92, 0x80ffff0c, 0x0},
        {NULL, 0x93, 0x80ffff0c, 0x0},
        {NULL, 0x94, 0x80ffff0c, 0x0},
        {NULL, 0x95, 0x80ffff0c, 0x0},
        {NULL, 0x96, 0x80ffff0c, 0x0},
        {NULL, 0x97, 0x80ffff0c, 0x0},
        {NULL, 0x98, 0x80ffff0c, 0x0},
        {NULL, 0x99, 0x80ffff0c, 0x0},
        {NULL, 0x9a, 0x80ffff0c, 0x0},
        {NULL, 0x9b, 0x80ffff0c, 0x0},
        {NULL, 0x9c, 0x80ffff0c, 0x0},
        {NULL, 0x9d, 0x80ffff0c, 0x0},
        {NULL, 0x9e, 0x80ffff0c, 0x0},
        {NULL, 0x9f, 0x80ffff0c, 0x0},
        {NULL, 0xa0, 0x80ffff0c, 0x0},
        {NULL, 0xa1, 0x80ffff0c, 0x0},
        {NULL, 0xa2, 0x80ffff0c, 0x0},
        {NULL, 0xa3, 0x80ffff0c, 0x0},
        {NULL, 0xa4, 0x80ffff0c, 0x0},
        {NULL, 0xa5, 0x80ffff0c, 0x0},
        {NULL, 0xa6, 0x80ffff0c, 0x0},
        {NULL, 0xa7, 0x80ffff0c, 0x0},
        {NULL, 0xa8, 0x80ffff0c, 0x0},
        {NULL, 0xa9, 0x80ffff0c, 0x0},
        {NULL, 0xaa, 0x80ffff0c, 0x0},
        {NULL, 0xab, 0x80ffff0c, 0x0},
        {NULL, 0xac, 0x80ffff0c, 0x0},
        {NULL, 0xad, 0x80ffff0c, 0x0},
        {NULL, 0xae, 0x80ffff0c, 0x0},
        {NULL, 0xaf, 0x80ffff0c, 0x0},
        {&PcDiGuidXAxis, 0xb0, 0x80ffff03, 0x200},
        {&PcDiGuidYAxis, 0xb4, 0x80ffff03, 0x200},
        {&PcDiGuidZAxis, 0xb8, 0x80ffff03, 0x200},
        {&PcDiGuidRxAxis, 0xbc, 0x80ffff03, 0x200},
        {&PcDiGuidRyAxis, 0xc0, 0x80ffff03, 0x200},
        {&PcDiGuidRzAxis, 0xc4, 0x80ffff03, 0x200},
        {&PcDiGuidSlider, 0x18, 0x80ffff03, 0x200},
        {&PcDiGuidSlider, 0x1c, 0x80ffff03, 0x200},
        {&PcDiGuidXAxis, 0xd0, 0x80ffff03, 0x300},
        {&PcDiGuidYAxis, 0xd4, 0x80ffff03, 0x300},
        {&PcDiGuidZAxis, 0xd8, 0x80ffff03, 0x300},
        {&PcDiGuidRxAxis, 0xdc, 0x80ffff03, 0x300},
        {&PcDiGuidRyAxis, 0xe0, 0x80ffff03, 0x300},
        {&PcDiGuidRzAxis, 0xe4, 0x80ffff03, 0x300},
        {&PcDiGuidSlider, 0x18, 0x80ffff03, 0x300},
        {&PcDiGuidSlider, 0x1c, 0x80ffff03, 0x300},
        {&PcDiGuidXAxis, 0xf0, 0x80ffff03, 0x400},
        {&PcDiGuidYAxis, 0xf4, 0x80ffff03, 0x400},
        {&PcDiGuidZAxis, 0xf8, 0x80ffff03, 0x400},
        {&PcDiGuidRxAxis, 0xfc, 0x80ffff03, 0x400},
        {&PcDiGuidRyAxis, 0x100, 0x80ffff03, 0x400},
        {&PcDiGuidRzAxis, 0x104, 0x80ffff03, 0x400},
        {&PcDiGuidSlider, 0x18, 0x80ffff03, 0x400},
        {&PcDiGuidSlider, 0x1c, 0x80ffff03, 0x400},
    };
}

extern "C" {
    static const PC_DI_OBJECT_FORMAT PcMouseDataFormatObjects[11] = {
        {&PcDiGuidXAxis, 0x0, 0xffff03, 0x0}, {&PcDiGuidYAxis, 0x4, 0xffff03, 0x0}, {&PcDiGuidZAxis, 0x8, 0x80ffff03, 0x0},
        {NULL, 0xc, 0xffff0c, 0x0},           {NULL, 0xd, 0xffff0c, 0x0},           {NULL, 0xe, 0x80ffff0c, 0x0},
        {NULL, 0xf, 0x80ffff0c, 0x0},         {NULL, 0x10, 0x80ffff0c, 0x0},        {NULL, 0x11, 0x80ffff0c, 0x0},
        {NULL, 0x12, 0x80ffff0c, 0x0},        {NULL, 0x13, 0x80ffff0c, 0x0},
    };
}

extern const GUID PcSystemKeyboardId = {0x6f1d2b61, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const PC_DI_DATA_FORMAT PcKeyboardDataFormat = {24, 16, 2, 256, 256, PcKeyboardDataFormatObjects};
extern const PC_DI_DATA_FORMAT PcJoystickDataFormat = {24, 16, 1, 272, 164, PcJoystickDataFormatObjects};
extern const GUID PcSystemMouseId = {0x6f1d2b60, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDirectInput8AId = {0xbf798030, 0x483a, 0x4da2, {0xaa, 0x99, 0x5d, 0x64, 0xed, 0x36, 0x97, 0x00}};
extern const PC_DI_DATA_FORMAT PcMouseDataFormat = {24, 16, 2, 20, 11, PcMouseDataFormatObjects};

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 PCInputFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

void PC_INPUT_RUNTIME::SetSuppressKey(u8 value) {
    suppress_key_1 = value;
}

void PC_INPUT_RUNTIME::SetActionKeysActive(i32 port, u8 value) {
    devices[port].action_keys_active = value;
}

void PC_INPUT_RUNTIME::SetCapturedPort(i32 port) {
    captured_port = port;
}

i32 PC_INPUT_RUNTIME::GetCapturedPort() {
    return captured_port;
}

void PC_INPUT_RUNTIME::ResetShutdownState() {
    shutdown_state = 0;
}

void PC_INPUT_RUNTIME::SetThresholds(f32 threshold, i32 motion, f32 ramp) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        axis_threshold = threshold * 32767.0;
    }
    if (motion > 0) {
        motion_threshold = motion;
    }
    if (ramp >= 0.0f) {
        ramp_seconds = ramp;
    }
}

extern "C" {
    u8 PcMouseAxisModes[8] = {0};
}

void PC_INPUT_RUNTIME::SetMouseAxisModes(u8 x_mode, u8 y_mode, u8 wheel_mode) {
    memset(PcMouseAxisModes, 0, sizeof(PcMouseAxisModes));
    PcMouseAxisModes[0] = x_mode;
    PcMouseAxisModes[1] = y_mode;
    PcMouseAxisModes[2] = wheel_mode;
}

extern "C" {
    PC_INPUT_RUNTIME *nu_input_state = 0;
}

extern "C" void fun_006d3560(void) {
}

extern "C" void PCInputNuError(void) {
}

extern "C" PC_INPUT_RUNTIME *PcInputGetRuntime(void) {
    return nu_input_state;
}

void PC_INPUT_RUNTIME::ClearResetPairs() {
    for (i32 port = 0; port < 14; ++port) {
        for (i32 action = 0; action < 32; ++action) {
            devices[port].reset_pairs[action].first = 0;
            devices[port].reset_pairs[action].second = 0;
        }
    }
}

void PC_INPUT_RUNTIME::GetDeviceCounts(i32 port, i32 *buttons, i32 *axes, i32 *povs) {
    *buttons = devices[port].button_count;
    *axes = devices[port].axis_count;
    *povs = devices[port].pov_count;
}

i32 PC_INPUT_RUNTIME::FindConfigRecord(PC_INPUT_CONFIG *records, i32 count, PC_INPUT_CONFIG *config) {
    for (i32 index = 0; index < count; ++index) {
        if (config->product_id == records[index].product_id && config->key == records[index].key) {
            return index;
        }
    }
    return -1;
}

i32 PC_INPUT_RUNTIME::ConfigChecksumStep(i32 previous, i32 value) {
    if (previous & 0x80000000) {
        return (previous * 2 + 1) ^ value ^ 0x12345678;
    }
    return (previous * 2) ^ value ^ 0x12345678;
}

i32 PC_INPUT_RUNTIME::HashAuxiliaryBindings() {
    i32 hash = 0;
    for (i32 bank = 0; bank < 4; ++bank) {
        for (i32 action = 0; action < 32; ++action) {
            hash = ((hash & 0x80000000) ? hash * 2 + 1 : hash * 2) ^ auxiliary_bindings[bank][action].value ^ 0x12345678;
            hash = ((hash & 0x80000000) ? hash * 2 + 1 : hash * 2) ^ auxiliary_bindings[bank][action].kind ^ 0x12345678;
        }
    }
    return hash;
}

i32 PC_INPUT_RUNTIME::FindPrimaryConfig(PC_INPUT_CONFIG *config) {
    for (i32 index = 0; index < 64; ++index) {
        if (config->product_id == primary[index].product_id && config->key == primary[index].key) {
            return index;
        }
    }
    return -1;
}

void PC_INPUT_RUNTIME::PromoteConfig(PC_INPUT_CONFIG *records, i32 count, PC_INPUT_CONFIG *config) {
    i32 index = 0;
    for (; index < count; ++index) {
        if (config->product_id == records[index].product_id && records[index].key == config->key) {
            break;
        }
    }
    if (index == count) {
        index = count - 1;
    }
    if (index > 0) {
        PCByteCopy(records + 1, records, index * sizeof(*records));
    }
    PCByteCopy(records, config, sizeof(*records));
}

void PC_INPUT_RUNTIME::PromotePrimaryConfig(PC_INPUT_CONFIG *config) {
    PromoteConfig(primary, 64, config);
}

void PC_INPUT_RUNTIME::fun_006d3980(i32) {
}

void PC_INPUT_RUNTIME::ReadConfiguration(PC_INPUT_CONFIG *records, const char *directory) {
    char path[1024];
    u32 version = 0;
    u32 magic;
    NuStrCpy(path, directory);
    NuStrCat(path, "Mappings.dat");
    memset(records, 0, 0x2700);
    FILE *file = PcOpenPlatformFile(path, "", 0, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        i32 bytes = ftell(file);
        fseek(file, 0, SEEK_SET);
        fread(&magic, 4, 1, file);
        if (magic == 0x4d415056) {
            fread(&version, 4, 1, file);
            bytes -= 8;
        } else {
            fseek(file, 0, SEEK_SET);
        }
        if (version == 19) {
            fread(records, bytes, 1, file);
        }
        fclose(file);
    }
}

void PC_INPUT_RUNTIME::InitConfiguration() {
    char directory[1024];
    /* Retail passes only the folder; the two missing %s read the company and product arguments that
       PcGetUserFolder left on the stack (cdecl cleanup is deferred). */
    sprintf(directory, "%s\\%s\\%s\\", PcGetUserFolder(PcCompanyName, PcProductName));
    memset(primary, 0, sizeof(primary));
    ReadConfiguration(primary, directory);
}

void PC_INPUT_RUNTIME::fun_006d3b10(i32) {
}

void PC_INPUT_RUNTIME::SanitizeConfigName(const char *name, char *destination) {
    PCByteCopy(destination, name, 63);
    destination[63] = 0;
    char *out = destination;
    for (char *in = destination; *in; ++in) {
        char c = *in;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
            *out++ = c;
        }
    }
    *out = 0;
}

const char *PcInputBindingKindNames[14] = {"NOTSET",          "PADDIGITAL",      "PADANALOGUEPOS", "PADANALOGUENEG", "MOUSEDIGITAL",
                                           "MOUSEMOVEPOS",    "MOUSEMOVENEG",    "KEYDIGITAL",     "KEYDIGITALPOS",  "KEYDIGITALNEG",
                                           "KEYANALOGUEPAIR", "PADANALOGUEPAIR", "FORCEDMOUSEX",   "FORCEDMOUSEY"};

void PC_INPUT_RUNTIME::FormatConfigText(char *destination, PC_INPUT_CONFIG *config) {
    char text[256];
    char part[256];
    char character[4];
    sprintf(text, "\t\"%s\",\t0x%x,%d,%d, {", config->name, config->product_id, config->guid_data2, config->guid_data3);
    for (i32 i = 0; i < 8; ++i) {
        u32 c = config->guid_data4[i];
        if (c < 32) {
            sprintf(part, "%d", c);
        } else {
            character[0] = (char)c;
            character[1] = 0;
            sprintf(part, "'%s'", character);
        }
        strcat(text, part);
        strcat(text, i < 7 ? "," : "}, //Guid stuff\n");
    }
    NuStrCpy(destination, text);
    sprintf(text, "\t%d,%d,%d,%d,%d,%d,0,0,\t//NumButtons etc.\n", config->button_count, config->pov_count, config->axis_count,
            (i16)((u16)config->deadzone | ((u16)config->deadzone_high << 8)),
            (i16)((u16)config->alternate_deadzone | ((u16)config->alternate_deadzone_high << 8)), config->key);
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d,%d, ", PcInputBindingKindNames[config->axes[i][0]], config->axes[i][1], config->axes[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//analogue thumb sticks\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d,%d, ", PcInputBindingKindNames[config->axis_bindings[i][0]], config->axis_bindings[i][1],
                config->axis_bindings[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//analogue shoulder buttons\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//l2,r2,l1,r1\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 4; i < 8; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//rup,rright,rdown,rleft\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 8; i < 12; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//select,l3,r3,start\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 12; i < 16; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//lup,lright,ldown,lleft\n");
    NuStrCat(destination, text);
}

void PC_INPUT_RUNTIME::ExportConfig(i32 port, PC_INPUT_CONFIG *config, i32 auxiliary, i32 bank, i32 key) {
    PC_INPUT_BINDING *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    for (i32 i = 0; i < 8; ++i) {
        config->all_axes[i][0] = 0;
        config->all_axes[i][1] = 255;
        config->all_axes[i][2] = 255;
    }
    for (i32 i = 0; i < 16; ++i) {
        config->buttons[i][0] = 0;
        config->buttons[i][1] = 0;
    }
    for (i32 action = 0; action < shutdown_state; ++action) {
        i32 button = commands[action].button;
        i32 active = commands[action].active;
        i32 axis = commands[action].axis;
        i32 kind = bindings[action].kind;
        if (!kind) {
            continue;
        }
        if (axis >= 0) {
            if (kind == 12 || kind == 13) {
                config->all_axes[axis][0] = (u8)kind;
                u8 value = (u8)bindings[action].value;
                config->all_axes[axis][1] = value;
                config->all_axes[axis][2] = value;
            } else if (kind == PC_BINDING_JOYSTICK_AXIS || kind == PC_BINDING_JOYSTICK_AXIS_REVERSED) {
                config->all_axes[axis][1] = (u8)bindings[action].value;
                config->all_axes[axis][2] = (u8)bindings[action].value;
                if (active == 3) {
                    config->all_axes[axis][0] = (u8)kind;
                } else if ((active == 2 && kind == PC_BINDING_JOYSTICK_AXIS) ||
                           (active == 1 && kind == PC_BINDING_JOYSTICK_AXIS_REVERSED)) {
                    config->all_axes[axis][0] = 2;
                } else {
                    config->all_axes[axis][0] = 3;
                }
            } else if (kind == PC_BINDING_MOUSE_AXIS || kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                config->all_axes[axis][1] = (u8)bindings[action].value;
                config->all_axes[axis][2] = (u8)bindings[action].value;
                if (active == 3) {
                    config->all_axes[axis][0] = (u8)kind;
                } else if ((active == 2 && kind == PC_BINDING_MOUSE_AXIS) || (active == 1 && kind == PC_BINDING_MOUSE_AXIS_REVERSED)) {
                    config->all_axes[axis][0] = 5;
                } else {
                    config->all_axes[axis][0] = 6;
                }
            } else if (kind == PC_BINDING_JOYSTICK_BUTTON) {
                config->all_axes[axis][0] = 11;
                if (active == 2 || active == 3) {
                    config->all_axes[axis][2] = (u8)bindings[action].value;
                } else {
                    config->all_axes[axis][1] = (u8)bindings[action].value;
                }
            } else if (kind == PC_BINDING_KEY) {
                config->all_axes[axis][0] = 10;
                if (active == 2 || active == 3) {
                    config->all_axes[axis][2] = (u8)bindings[action].value;
                } else {
                    config->all_axes[axis][1] = (u8)bindings[action].value;
                }
            }
        }
        if (button >= 0 && (kind >= 1 && kind <= 7 || kind == 12 || kind == 13)) {
            config->buttons[button][0] = (u8)kind;
            config->buttons[button][1] = (u8)bindings[action].value;
        }
    }
    for (i32 i = 0; i < 8; ++i) {
        if (config->all_axes[i][1] == 255 || config->all_axes[i][2] == 255) {
            config->all_axes[i][0] = 0;
            config->all_axes[i][2] = 0;
            config->all_axes[i][1] = 0;
        }
    }
    memset(config->name, 0, sizeof(config->name));
    PCByteCopy(config->name, devices[port].name, NuStrLen(devices[port].name));
    config->product_id = devices[port].product.Data1;
    config->guid_data2 = devices[port].product.Data2;
    config->guid_data3 = devices[port].product.Data3;
    for (i32 i = 0; i < 8; ++i) {
        config->guid_data4[i] = devices[port].product.Data4[i];
    }
    PC_INPUT_EXPORT_METADATA *metadata = (PC_INPUT_EXPORT_METADATA *)&config->button_count;
    metadata->button_count = devices[port].button_count;
    metadata->pov_count = devices[port].pov_count;
    metadata->axis_count = devices[port].axis_count;
    u16 deadzone = devices[port].deadzone;
    i16 saved_key = (i16)key;
    metadata->deadzone_word = deadzone;
    metadata->alternate_deadzone_word = devices[port].alternate_deadzone;
    metadata->key = saved_key;
}

PC_INPUT_CONFIG PcStaticInputConfigs[24] = {
    {"USB Joystick     ",
     0x30e8f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"XBOX 360 For Windows (Controller)",
     0x28e045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Controller (Xbox 360 Wireless Receiver for Windows)",
     0x719045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"PS Vibration Feedback Converter ",
     0x50c12,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Rockfire PSX\\USB Bridge",
     0x20500583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logic3 12 button vibration",
     0x31207b5,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Interact Gaming Device",
     0x262a05fd,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x01},
      {0x01, 0x04},
      {0x01, 0x03},
      {0x01, 0x00},
      {0x01, 0x0a},
      {0x01, 0x02},
      {0x01, 0x05},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Keyboard",
     0x1,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x2,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x12},
      {0x07, 0x02},
      {0x07, 0x04},
      {0x07, 0x48},
      {0x07, 0x4d},
      {0x07, 0x50},
      {0x07, 0x4b},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0x14},
      {0x07, 0x23},
      {0x07, 0x22},
      {0x07, 0x21}}},
    {"Keyboard",
     0x4,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"PLAYSTATION(R)3 Controller",
     0x268054c,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x02, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x00, 0x05, 0x05}, {0x00, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07}}},
    {"Saitek P2900 Wireless Pad",
     0x40c06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0e, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4 axis 16 button joystick",
     0x6676666,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f}}},
    {"MP-8888 USB Joypad",
     0x88880925,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"2 in 1 DT",
     0xb320044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech RumblePad 2 USB",
     0xc218046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     10,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech Dual Action",
     0xc216046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     8,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Rumble Pad",
     0x1100f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Thrustmaster vibrating gamepad",
     0xb312044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4-Axis,12-Button with POV",
     0xa0090583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Saitek P2600 Rumble Force Pad",
     0x5f0d06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"T Mini",
     0xd001044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Pad",
     0x1120f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}}};

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::FindConfig(u8 mask, i32 port, i32 key) {
    if (port < 0) {
        return NULL;
    }
    if (mask & 4) {
        for (i32 i = 0; i < 64; ++i) {
            if (secondary[i].product_id == devices[port].product.Data1 && (key < 0 || secondary[i].key == key)) {
                return &secondary[i];
            }
        }
    }
    if (mask & 2) {
        for (i32 i = 0; i < 64; ++i) {
            if (primary[i].product_id == devices[port].product.Data1 && (key < 0 || primary[i].key == key)) {
                return &primary[i];
            }
        }
    }
    if (mask & 1) {
        for (u32 i = 0; i < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++i) {
            if (PcStaticInputConfigs[i].product_id == devices[port].product.Data1 && (key < 0 || PcStaticInputConfigs[i].key == key)) {
                return &PcStaticInputConfigs[i];
            }
        }
    }
    return NULL;
}

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::FindConfigByName(u8 mask, const char *name) {
    if (mask & 4) {
        for (i32 index = 0; index < 64; ++index) {
            if (NuStrICmp(secondary[index].name, name) == 0) {
                return &secondary[index];
            }
        }
    }
    if (mask & 2) {
        for (i32 index = 0; index < 64; ++index) {
            if (NuStrICmp(primary[index].name, name) == 0) {
                return &primary[index];
            }
        }
    }
    if (mask & 1) {
        for (u32 index = 0; index < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++index) {
            if (NuStrICmp(PcStaticInputConfigs[index].name, name) == 0) {
                return &PcStaticInputConfigs[index];
            }
        }
    }
    return NULL;
}

void PC_INPUT_RUNTIME::SetPreferredKey(i32 key) {
    preferred_key = key;
}

void PC_INPUT_RUNTIME::InstallConfig(PC_INPUT_CONFIG *config, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port) {
    PC_INPUT_BINDING *bindings;
    if (auxiliary) {
        auxiliary_ports[auxiliary_port] = port;
        bindings = auxiliary_bindings[auxiliary_port];
    } else {
        bindings = devices[port].bindings;
    }
    devices[port].config_present = config != NULL;
    devices[port].config_dirty = 0;
    for (i32 i = 0; i < 32; ++i) {
        bindings[i].kind = PC_BINDING_NONE;
        bindings[i].value = 0;
    }
    if (config != NULL) {
        devices[port].config_installed = 1;
        if (!auxiliary) {
            devices[port].deadzone = config->deadzone;
            devices[port].alternate_deadzone = config->alternate_deadzone;
        }
        PC_INPUT_BINDING *binding = bindings;
        for (i32 i = 0; i < 32; ++i, ++binding) {
            if (commands[i].active) {
                i32 axis = commands[i].axis;
                i32 button = commands[i].button;
                if (axis >= 0) {
                    i32 kind = config->axes[axis][0];
                    i32 primary = config->axes[axis][1];
                    i32 secondary = config->axes[axis][2];
                    switch (kind) {
                        case 10:
                            binding->kind = PC_BINDING_KEY;
                            if (commands[i].active == 2) {
                                binding->value = secondary;
                            } else {
                                binding->value = primary;
                            }
                            break;
                        case 11:
                            binding->kind = PC_BINDING_JOYSTICK_BUTTON;
                            if (commands[i].active == 2) {
                                binding->value = secondary;
                            } else {
                                binding->value = primary;
                            }
                            break;
                        case PC_BINDING_JOYSTICK_AXIS:
                        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                        case PC_BINDING_MOUSE_AXIS:
                        case PC_BINDING_MOUSE_AXIS_REVERSED:
                        case 12:
                        case 13:
                            binding->kind = kind;
                            binding->value = secondary;
                            break;
                    }
                }
                if ((u32)button <= 15 && binding->kind == PC_BINDING_NONE) {
                    u8 kind = config->buttons[button][0];
                    u8 value = config->buttons[button][1];
                    switch (kind) {
                        case PC_BINDING_JOYSTICK_BUTTON:
                        case PC_BINDING_JOYSTICK_AXIS:
                        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                        case PC_BINDING_MOUSE_BUTTON:
                        case PC_BINDING_MOUSE_AXIS:
                        case PC_BINDING_MOUSE_AXIS_REVERSED:
                        case PC_BINDING_KEY:
                            binding->kind = kind;
                            binding->value = value;
                            break;
                    }
                }
            }
        }
    }
}

i32 PC_INPUT_RUNTIME::fun_006d4790(i32) {
    return 0;
}

void PC_INPUT_RUNTIME::InitXInputCaps(const PC_XINPUT_CAPABILITIES *caps, PC_INPUT_DEVICE *record) {
    record->pov_count = 0;
    record->button_count = 0;
    record->axis_count = 5;
    if (record->product.Data1 == 0x028e045e || record->product.Data1 == 0x0719045e) {
        record->button_count = 10;
        record->pov_count = 1;
        record->left_trigger = 0xffff;
        record->right_trigger = 0xffff;
    } else {
        record->left_trigger = caps->motor_left;
        record->right_trigger = caps->motor_right;
        for (i32 bit = 0; bit < 32; ++bit) {
            if (caps->gamepad.buttons & (1 << bit)) {
                ++record->button_count;
            }
        }
        if ((caps->gamepad.buttons & 15) == 15) {
            record->button_count -= 4;
            record->pov_count = 1;
        }
    }
}

// Native input_runtime.cpp member.
void PC_INPUT_RUNTIME::NormalizeAxisPair(i16 *x, i16 *y) {
    f32 vx = (f32)*x;
    f32 vy = (f32)*y;
    f32 ax;
    f32 ay;
    NU_FABS_STORE(ax, vx);
    NU_FABS_STORE(ay, vy);
    f32 square = ax * ax + ay * ay;
    f32 magnitude = NuFsqrt(square);
    f32 ratio;
    if (ax > ay) {
        ratio = (f32)(32767.0 / ax);
    } else if (ay != 0.0f) {
        ratio = (f32)(32767.0 / ay);
    } else {
        ratio = 1.0f;
    }
    f32 scale = ratio * magnitude;
    scale = (f32)(scale * 0.000030517578125);
    vx = scale * vx;
    vy = scale * vy;
    if (vx < -32767.0) {
        vx = -32767.0f;
    }
    if (vx > 32767.0) {
        vx = 32767.0f;
    }
    if (vy < -32767.0) {
        vy = -32767.0f;
    }
    if (vy > 32767.0) {
        vy = 32767.0f;
    }
    *x = (i16)(i32)vx;
    *y = (i16)(i32)vy;
}

i32 PC_INPUT_RUNTIME::FindButtonCommand(i32 button) {
    for (i32 index = 0; index < shutdown_state; ++index) {
        if (commands[index].active == 3 && commands[index].button == button) {
            return index;
        }
    }
    return -1;
}

i32 PC_INPUT_RUNTIME::FindAxisCommand(i32 axis, i32 active) {
    for (i32 index = 0; index < shutdown_state; ++index) {
        if (commands[index].active == active && commands[index].axis == axis) {
            return index;
        }
    }
    return -1;
}

/* Clears the pad direction bits that the first four bindings map to joystick buttons 0x80-0x83. */
u32 PC_INPUT_RUNTIME::fun_006d4a80(i32 port, u32 buttons) {
    for (i32 i = 0; i < 4; ++i) {
        PC_INPUT_BINDING *binding = &devices[port].bindings[i];
        if (binding->kind == PC_BINDING_JOYSTICK_BUTTON) {
            i32 button = binding->value;
            if (button == 0x80) {
                buttons &= ~0x1000;
            }
            if (button == 0x82) {
                buttons &= ~0x4000;
            }
            if (button == 0x83) {
                buttons &= ~0x8000;
            }
            if (button == 0x81) {
                buttons &= ~0x2000;
            }
        }
    }
    return buttons;
}

void PC_INPUT_RUNTIME::CopyBindings(i32 port, i32 to_auxiliary, i32 bank) {
    if (port < 0) {
        return;
    }
    if (to_auxiliary) {
        auxiliary_ports[bank] = port;
        PCByteCopy(auxiliary_bindings[bank], devices[port].bindings, 0x80);
    } else {
        PCByteCopy(devices[port].bindings, auxiliary_bindings[bank], 0x80);
    }
}

void PC_INPUT_RUNTIME::AdvanceCaptureRecords() {
    for (i32 port = 0; port < 14; ++port) {
        staged_capture[port] = captures[port];
        captures[port].kind[2] = PC_BINDING_NONE;
        captures[port].kind[0] = PC_BINDING_NONE;
        captures[port].kind[1] = PC_BINDING_NONE;
        if (mouse.port_activity[port]) {
            --mouse.port_activity[port];
            if (staged_capture[port].kind[2] || staged_capture[port].kind[0] || staged_capture[port].kind[1]) {
                mouse.port_activity[port] = 5;
            }
        }
    }
    completed_mask = 0;
}

extern "C" {
    const char *PcKeyboardLayoutNames[256] = {0};
    char PcKeyboardLayoutText[512] = {0};
}

void PC_INPUT_RUNTIME::BuildKeyboardLayoutNames(u32 layout_id) {
    char *text = PcKeyboardLayoutText;
    char name[16];
    sprintf(name, "%08.8i32x", layout_id);
    HKL layout = LoadKeyboardLayoutA(name, 1);
    for (i32 key = 0; key < 255; ++key) {
        PcKeyboardLayoutNames[key] = "?";
        u32 character = MapVirtualKeyExA(key, 3, layout);
        if (character >= 0x20 && character <= 0x5a) {
            PcKeyboardLayoutNames[key] = text;
            text[0] = character;
            text[1] = 0;
            text += 2;
        }
    }
}

extern "C" {
    i32 nu_keyboard_repeat_countdown = 0;
}

void PC_INPUT_RUNTIME::SetKeyboardUiOverride(i32 value) {
    i32 previous = keyboard_ui_override;
    keyboard_ui_override = value;
    if (previous != value) {
        nu_keyboard_repeat_countdown = 6;
    }
}

i32 PC_INPUT_RUNTIME::GetKeyboardUiOverride() {
    return keyboard_ui_override;
}

void PC_INPUT_RUNTIME::ClearMouseAccumulation() {
    mouse.accumulated[0] = mouse.accumulated[1] = mouse.accumulated[2] = 0.0f;
}

void PC_INPUT_RUNTIME::ResetInputRecords() {
    for (i32 i = 0; i < 32; ++i) {
        commands[i].active = 0;
        commands[i].button = -1;
        commands[i].axis = -1;
        commands[i].value = 0;
    }
    shutdown_state = 0;
}

extern "C" {
    void (*TextInputNameCallback)(i32, char *) = 0;
}

extern "C" void fun_006d3b20(void) {
}

extern "C" void fun_006d4d60(void) {
}

extern "C" void PcInput_SetStringUCGfxRemapFn(void (*callback)(i32, char *)) {
    TextInputNameCallback = callback;
}

i32 PC_INPUT_RUNTIME::IsActionBound(i32 port, i32 action, i32 auxiliary, i32 bank) {
    if (auxiliary) {
        return auxiliary_bindings[bank][action].kind != PC_BINDING_NONE;
    }
    return devices[port].bindings[action].kind != PC_BINDING_NONE;
}

void PC_INPUT_RUNTIME::SetCustomBindingsActive(i32 value) {
    mouse.custom_bindings_active = value;
}

i32 PC_INPUT_RUNTIME::GetInputRecordCount() {
    return shutdown_state;
}

i32 PC_INPUT_RUNTIME::GetReplayFrame() {
    return replay_frame;
}

i32 PC_INPUT_RUNTIME::GetReplayLength() {
    return replay_length;
}

void PC_INPUT_RUNTIME::InsertInputRecord(i32 index, i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04) {
    if ((u32)index > 31) {
        return;
    }
    for (i32 i = 0; i < 32; ++i) {
        if (unknown_00 == commands[i].active) {
            if (field_01 >= 0 && field_01 == commands[i].axis) {
                return;
            }
            if (unknown_02 >= 0 && unknown_02 == commands[i].button) {
                return;
            }
        }
    }
    commands[index].active = (i8)unknown_00;
    commands[index].axis = (i8)field_01;
    commands[index].button = (i8)unknown_02;
    commands[index].value = unknown_04;
}

i32 PC_INPUT_RUNTIME::GetCommand(i32 action, i32 *active, i32 *axis, i32 *button, i32 *value) {
    if (action < 0 || action >= 32) {
        return 0;
    }
    if (active != NULL) {
        *active = commands[action].active;
    }
    if (axis != NULL) {
        *axis = commands[action].axis;
    }
    if (button != NULL) {
        *button = commands[action].button;
    }
    if (value != NULL) {
        *value = commands[action].value;
    }
    return 1;
}

i32 PC_INPUT_RUNTIME::FindCommand(i32 active, i32 code) {
    i32 button;
    i32 axis;
    i32 mode;
    for (i32 action = 0; action < 32; ++action) {
        GetCommand(action, &mode, &axis, &button, NULL);
        if (mode == active) {
            if (active == 3) {
                if (button == code) {
                    return action;
                }
            } else if ((active == 1 || active == 2) && axis == code) {
                return action;
            }
        }
    }
    return -1;
}

void PC_INPUT_RUNTIME::SetBlockedKeys(const char *keys) {
    blocked_keys[0] = 0;
    if (keys != NULL) {
        NuStrCpy(blocked_keys, keys);
    }
}

i32 PC_INPUT_RUNTIME::HasBinding(i32 port, i32 kind, i32 value) {
    for (i32 action = 0; action < 32; ++action) {
        if (devices[port].bindings[action].kind == kind && devices[port].bindings[action].value == value) {
            return 1;
        }
    }
    return 0;
}

/* Original source name unknown. PlayersDropInOut calls it rather than folding the constant. */
extern "C" i32 PcInput_GetFirstKeyboardPort(void) {
    return 10;
}

extern "C" {
    i8 PcInputActionOrder[20] = {0};
}

extern "C" void PcInputSetActionOrder(const i8 *order) {
    i8 *destination;
    i32 value;
    PcInputActionOrder[0] = -1;
    if (order != NULL) {
        destination = PcInputActionOrder;
        do {
            value = *order++;
            *destination++ = value;
        } while (value >= 0);
    }
}

// The DirectX SDK "XInput and DirectInput" IsXInputDevice sample.
extern "C" BOOL IsXInputProduct(const GUID *product) {
    IWbemLocator *locator = NULL;
    IEnumWbemClassObject *enumerator = NULL;
    IWbemClassObject *devices[20] = {NULL};
    IWbemServices *services = NULL;
    BSTR namespace_name = NULL;
    BSTR device_id = NULL;
    BSTR class_name = NULL;
    DWORD returned = 0;
    bool is_xinput = false;
    UINT index = 0;
    VARIANT value;
    HRESULT hr;

    hr = CoInitialize(NULL);
    bool cleanup_com = SUCCEEDED(hr);

    hr = CoCreateInstance(PcWbemClassId, NULL, CLSCTX_INPROC_SERVER, PcWbemInterfaceId, (LPVOID *)&locator);
    if (FAILED(hr) || locator == NULL) {
        goto cleanup;
    }

    namespace_name = SysAllocString(L"\\\\.\\root\\cimv2");
    if (namespace_name == NULL) {
        goto cleanup;
    }
    class_name = SysAllocString(L"Win32_PNPEntity");
    if (class_name == NULL) {
        goto cleanup;
    }
    device_id = SysAllocString(L"DeviceID");
    if (device_id == NULL) {
        goto cleanup;
    }

    hr = locator->ConnectServer(namespace_name, NULL, NULL, 0L, 0L, NULL, NULL, &services);
    if (FAILED(hr) || services == NULL) {
        goto cleanup;
    }

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                      EOAC_NONE);

    hr = services->CreateInstanceEnum(class_name, 0, NULL, &enumerator);
    if (FAILED(hr) || enumerator == NULL) {
        goto cleanup;
    }

    for (;;) {
        hr = enumerator->Next(10000, 20, devices, &returned);
        if (FAILED(hr)) {
            goto cleanup;
        }
        if (returned == 0) {
            break;
        }

        for (index = 0; index < returned; index++) {
            hr = devices[index]->Get(device_id, 0L, &value, NULL, NULL);
            if (SUCCEEDED(hr) && value.vt == VT_BSTR && value.bstrVal != NULL) {
                if (wcsstr(value.bstrVal, L"IG_")) {
                    DWORD pid = 0;
                    DWORD vid = 0;
                    WCHAR *vid_text = wcsstr(value.bstrVal, L"VID_");
                    if (vid_text && swscanf(vid_text, L"VID_%4X", &vid) != 1) {
                        vid = 0;
                    }
                    WCHAR *pid_text = wcsstr(value.bstrVal, L"PID_");
                    if (pid_text && swscanf(pid_text, L"PID_%4X", &pid) != 1) {
                        pid = 0;
                    }

                    DWORD vid_pid = MAKELONG(vid, pid);
                    if (vid_pid == product->Data1) {
                        is_xinput = true;
                        goto cleanup;
                    }
                }
            }
            if (devices[index]) {
                devices[index]->Release();
                devices[index] = NULL;
            }
        }
    }

cleanup:
    if (namespace_name) {
        SysFreeString(namespace_name);
    }
    if (device_id) {
        SysFreeString(device_id);
    }
    if (class_name) {
        SysFreeString(class_name);
    }
    for (index = 0; index < sizeof(devices) / sizeof(devices[0]); index++) {
        if (devices[index]) {
            devices[index]->Release();
            devices[index] = NULL;
        }
    }
    if (enumerator) {
        enumerator->Release();
        enumerator = NULL;
    }
    if (locator) {
        locator->Release();
        locator = NULL;
    }
    if (services) {
        services->Release();
        services = NULL;
    }

    if (cleanup_com) {
        CoUninitialize();
    }

    return is_xinput;
}

extern "C" void ConfigureJoystickRanges(NuKeyboardDevice *device) {
    if (device == NULL) {
        return;
    }
    PC_DI_RANGE range;
    // MATCH-EXCEPTION: retail calls SetProperty through the C++ interface (method loaded before the arguments).
    PC_DI_DEVICE_INTERFACE *input = (PC_DI_DEVICE_INTERFACE *)device;
    range.header.size = 24;
    range.header.header_size = 16;
    range.header.how = 1;
    range.minimum = -32768;
    range.maximum = 32767;
    range.header.object = offsetof(PC_DI_JOYSTATE2, x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, rx);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, ry);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, rz);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, sliders[0]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, sliders[1]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, velocity_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, velocity_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, velocity_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, velocity_sliders[0]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, velocity_sliders[1]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, acceleration_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, acceleration_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, acceleration_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, acceleration_sliders[0]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, acceleration_sliders[1]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, force_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, force_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, force_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, torque_x);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, torque_y);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, torque_z);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, force_sliders[0]);
    input->SetProperty(*(const GUID *)4, &range.header);
    range.header.object = offsetof(PC_DI_JOYSTATE2, force_sliders[1]);
    input->SetProperty(*(const GUID *)4, &range.header);
}

extern "C" {
    u8 nu_keyboard_pages[0x200] = {0};
    i8 nu_keyboard_page_index = 0;
}

NuKeyboardOwner::NuKeyboardOwner() {
    rescan_interval = 0.0f;
    rescan_timer = 0.0f;
    mouse_enabled = 1;
    rescan_count = 0;
    device_count = 0;
    mouse_cooperative = 1;
    poll_status = 0;
    device = NULL;
    nu_keyboard_page_index = 0;
    memset(nu_keyboard_pages, 0, 256);
    memset(nu_keyboard_pages + 256, 0, 256);
}

NuKeyboardOwner::~NuKeyboardOwner() {
    rescan_interval = 0.0f;
    rescan_count = 0;
    rescan_timer = 0.0f;
    device_count = 0;
}

i32 NuKeyboardOwner::GetMouseCaps(i32 *axes, i32 *buttons) {
    if (mouse_axes != 0 && mouse_buttons != 0) {
        *axes = mouse_axes;
        *buttons = mouse_buttons;
        return 1;
    }
    *axes = 0;
    *buttons = 0;
    return 0;
}

// Native NuKeyboardOwner cohort, append to pc_input.cpp.
i32 NuKeyboardOwner::SetMouseCooperative(i32 value) {
    i32 previous = mouse_cooperative;
    if (previous == value) {
        return value;
    }
    mouse_cooperative = value;
    if (mouse != NULL) {
        mouse->methods->unacquire(mouse);
        if (value) {
            mouse->methods->set_cooperative_level(mouse, PCInputInstance, 5);
        } else {
            mouse->methods->set_cooperative_level(mouse, PCInputInstance, 6);
        }
        mouse->methods->acquire(mouse);
    }
    return previous;
}

void NuKeyboardOwner::SetMouseEnabled(u8 enabled) {
    mouse_enabled = enabled;
}

extern "C" {
    PC_DI_MOUSE_STATE PcMouseSamples[2] = {0};
    i32 nu_keyboard_device_flags = 0;
    i8 PcMouseSampleIndex = 0;
}

const PC_DI_MOUSE_STATE *NuKeyboardOwner::PollMouse() {
    if ((nu_keyboard_device_flags & 1) || mouse == NULL || !mouse_enabled) {
        return NULL;
    }
    if (mouse->methods->poll(mouse) < 0) {
        mouse->methods->acquire(mouse);
        i32 old = mouse_cooperative;
        mouse_cooperative = old == 0;
        SetMouseCooperative(old);
        return NULL;
    }
    PcMouseSampleIndex ^= 1;
    mouse->methods->get_state(mouse, 20, &PcMouseSamples[PcMouseSampleIndex]);
    return &PcMouseSamples[PcMouseSampleIndex];
}

extern "C" {
    i32 dat_00824fb8 = 20;
}

extern "C" void fun_006d5860(void) {
}

extern "C" i32 fun_006d5870(void) {
    return dat_00824fb8;
}

extern "C" i32 fun_006d5880(void) {
    return dat_00824fb8;
}

extern "C" {
    u32 mbuttons = 0;
}

extern "C" void fun_006d5890(void) {
}

void NuMouseRead(void) {
}

u32 NuMouseReadButtons(void) {
    return mbuttons;
}

static u32 PcMousePressedMask = 0;

extern "C" u32 PcMouseGetPressedMask(void) {
    return PcMousePressedMask;
}

extern "C" u32 NuMouseButtonDown(i32 button) {
    if (button >= 0 && button < 4u) {
        return (1 << button) & mbuttons;
    }
    return 0;
}

extern "C" u32 PcMouseButtonPressed(i32 button) {
    if (button >= 0 && button < 4u) {
        return (1 << button) & PcMousePressedMask;
    }
    return 0;
}

/* Add out-of-line definitions to existing pc_input.cpp. The actual global
   nu_keyboard_owner in data.cpp then uses genuine VC8 CRT construction/destruction. */
void NuKeyboardOwner::ScheduleRescan(f32 interval, i32 repetitions) {
    rescan_interval = interval;
    rescan_count = repetitions;
    rescan_timer = interval;
}

i32 NuKeyboardOwner::HasMouse() {
    return mouse != NULL;
}

i32 NuKeyboardOwner::HasDevice() {
    return device != NULL;
}

extern "C" {
    i32 nu_keyboard_key_index = 0;
    i32 nu_keyboard_previous_key_index = 0;
    i32 nu_keyboard_no_key_state = 0;
}

u8 *NuKeyboardOwner::PollKeyboard() {
    u8 *page;
    i8 page_index;
    i32 i;
    i32 next_key_index;
    i32 key_index;
    i32 key_count;

    poll_status = 0;
    if ((nu_keyboard_device_flags & 2) == 0 && device != 0) {
        poll_status = 1;
        if (device->methods->poll(device) < 0) {
            if (device->methods->acquire(device) < 0) {
                page = nu_keyboard_pages + nu_keyboard_page_index * 0x100;
                memset(page, 0, nu_keyboard_owner.key_count);
                return page;
            }
            device->methods->poll(device);
        }

        nu_keyboard_page_index ^= 1;
        if (device->methods->get_state(device, 0x100, nu_keyboard_pages + nu_keyboard_page_index * 0x100) == 0) {
            page_index = nu_keyboard_page_index;
            key_count = nu_keyboard_owner.key_count;
            key_index = nu_keyboard_key_index;
            nu_keyboard_previous_key_index = key_index;
            next_key_index = 0;
            for (i = 0; i < key_count; ++i) {
                if (nu_keyboard_pages[page_index * 0x100 + i] != 0 && i != key_index) {
                    next_key_index = i;
                    break;
                }
            }

            if (nu_keyboard_repeat_countdown != 0) {
                --nu_keyboard_repeat_countdown;
                if (next_key_index != 0) {
                    nu_keyboard_repeat_countdown = 6;
                }
                memset(nu_keyboard_pages + page_index * 0x100, 0, key_count);
            } else if (next_key_index != 0) {
                nu_keyboard_key_index = key_index = next_key_index;
                goto check_none;
            }
            if (key_index != 0 && nu_keyboard_pages[page_index * 0x100 + key_index] == 0) {
                nu_keyboard_key_index = key_index = 0;
            }
        check_none:
            if (key_index == 0) {
                nu_keyboard_no_key_state = 0;
            }
            return nu_keyboard_pages + page_index * 0x100;
        }
    }

    page = nu_keyboard_pages + nu_keyboard_page_index * 0x100;
    memset(page, 0, nu_keyboard_owner.key_count);
    return page;
}

extern "C" void fun_006d5940(void) {
}

extern "C" f32 PcMouseGetAccumulatedX(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[0];
}

extern "C" f32 PcMouseGetAccumulatedY(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[1];
}

extern "C" f32 PcMouseGetAccumulatedZ(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[2];
}

/* Eases the smoothed mouse axis towards the latest delta and returns it scaled, with a small dead zone. */
extern "C" f32 NuMouseSmoothAxis(i32 axis) {
    f32 value = nu_input_state->mouse.delta[axis] - nu_input_state->mouse.smoothed[axis];
    value = value * 0.125 + nu_input_state->mouse.smoothed[axis];
    nu_input_state->mouse.smoothed[axis] = value;
    value = value * 4.0;
    if (PCInputFabs(value) < 0.01f) {
        value = 0.0f;
    }
    return value;
}

extern "C" f32 NuMouseReadXRel(void) {
    return NuMouseSmoothAxis(0);
}

extern "C" f32 NuMouseReadYRel(void) {
    return NuMouseSmoothAxis(1);
}

extern "C" f32 NuMouseReadZRel(void) {
    return (f32)(nu_input_state->mouse.delta[2] * -4.0);
}

extern "C" f32 NuMouseReadXVel(void) {
    return (f32)(nu_input_state->mouse.delta[0] * -4.0);
}

extern "C" f32 NuMouseReadYVel(void) {
    return (f32)(nu_input_state->mouse.delta[1] * -4.0);
}

extern "C" f32 NuMouseReadZVel(void) {
    return (f32)(nu_input_state->mouse.delta[2] * -4.0);
}

extern "C" f32 PcInputMouseX(void) {
    return nu_input_state->mouse.position[0];
}

extern "C" f32 PcInputMouseY(void) {
    return nu_input_state->mouse.position[1];
}

extern "C" void PcInputSetThresholds(f32 threshold, i32 motion, f32 ramp) {
    nu_input_state->SetThresholds(threshold, motion, ramp);
}

extern "C" i32 PcInputBindingKind(i32 device, i32 binding, i32 auxiliary, i32 auxiliary_port) {
    if (auxiliary != 0) {
        return nu_input_state->auxiliary_bindings[auxiliary_port][binding].kind;
    }
    return nu_input_state->devices[device].bindings[binding].kind;
}

extern "C" void PcInputClearBindings(i32 device, i32 auxiliary, i32 auxiliary_port) {
    PC_INPUT_BINDING *bindings;
    i32 i;
    if (auxiliary != 0) {
        bindings = nu_input_state->auxiliary_bindings[auxiliary_port];
    } else {
        if (device < 0) {
            return;
        }
        bindings = nu_input_state->devices[device].bindings;
    }
    for (i = 0; i < 32; ++i) {
        bindings[i].kind = PC_BINDING_NONE;
        bindings[i].value = 0;
    }
}

extern "C" i32 PcInputHasBinding(i32 port, i32 kind, i32 value) {
    return nu_input_state->HasBinding(port, kind, value);
}

extern "C" i32 PcInputDevicePresent(i32 device) {
    if (device >= 0) {
        return nu_input_state->devices[device].kind != PC_INPUT_DEVICE_NONE;
    }
    return 0;
}

extern "C" void PcInputCopyBindings(i32 port, i32 to_auxiliary, i32 bank) {
    nu_input_state->CopyBindings(port, to_auxiliary, bank);
}

extern "C" void PcInputResetMouseMotion() {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->mouse.accumulated[2] = 0.0f;
    runtime->mouse.accumulated[1] = 0.0f;
    runtime->mouse.accumulated[0] = 0.0f;
}

extern "C" i32 PcInputPlayerOnKeyboardMouse(i32 player) {
    i32 port = NuPadGetPort(player);
    if (port == 10 || port == 12) {
        return 1;
    }
    return 0;
}

extern "C" i32 PcInputPlayerOnKeyboardMouse2(i32 player) {
    i32 port = NuPadGetPort(player);
    if (port == 10 || port == 12) {
        return 1;
    }
    return 0;
}

extern "C" i32 PcInputPlayerOnKeyboardPort(i32 player) {
    i32 port = NuPadGetPort(player);
    return port >= 10 && port <= 13;
}

void (*PcInputStateInitializedCallback)(PC_INPUT_RUNTIME *) = NULL;

extern "C" void PcInputSetStateInitializedCallback(void (*callback)(PC_INPUT_RUNTIME *)) {
    PcInputStateInitializedCallback = callback;
}

extern "C" void PcInputAuxiliaryMasks(u32 *buttons, u32 *axes, i32 bank) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    u32 button_mask = 0;
    u32 axis_mask = 0;
    for (i32 action = 0; action < 32; ++action) {
        i32 active;
        i32 axis;
        i32 button;
        if (runtime->GetCommand(action, &active, &axis, &button, NULL) && active != 0 &&
            runtime->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE) {
            if (button >= 0) {
                button_mask |= 1 << button;
            }
            if (axis >= 0) {
                axis_mask |= 1 << axis;
            }
        }
    }
    *buttons = button_mask;
    *axes = axis_mask;
}

extern "C" u32 PcInputAuxiliaryActionMask(i32 bank) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    u32 mask = 0;
    for (i32 action = 0; action < 32; ++action) {
        i32 active;
        if (runtime->GetCommand(action, &active, NULL, NULL, NULL) && active != 0 &&
            runtime->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE) {
            mask |= 1 << action;
        }
    }
    return mask;
}

extern "C" i32 PcInputFindActionForMasks(u32 buttons, u32 axes) {
    i32 action;
    i32 active;
    i32 axis;
    i32 button;
    i32 value;
    if (buttons == 0 && axes == 0) {
        return -2;
    }
    if (buttons != 0 && axes != 0) {
        return -3;
    }
    if (buttons != 0) {
        for (action = 0; action < 32; ++action) {
            if (nu_input_state->GetCommand(action, &active, &axis, &button, &value) && button >= 0 && (buttons & (1 << button))) {
                if (buttons & ~(1 << button)) {
                    return -4;
                }
                return action;
            }
        }
    } else {
        for (action = 0; action < 32; ++action) {
            if (nu_input_state->GetCommand(action, NULL, &axis, NULL, NULL) && axis >= 0 && (axes & (1 << axis))) {
                if (axes & ~(1 << axis)) {
                    return -5;
                }
                return action;
            }
        }
    }
    return -1;
}

extern "C" i32 GamePadReadBackend(FRAME_PAD *pad, i32 ignored) {
    return NuPadRead(pad);
}

extern "C" void fun_006d6030(void) {
}

extern "C" i32 fun_006d6040(void) {
    return 0;
}

extern "C" i32 fun_006d6050(void) {
    return 0;
}

extern "C" PC_DI_MOUSE_STATE *PcMouseCurrentSample(void) {
    return &PcMouseSamples[PcMouseSampleIndex];
}

extern "C" PC_DI_MOUSE_STATE *PcMousePreviousSample(void) {
    return &PcMouseSamples[PcMouseSampleIndex ^ 1];
}

extern "C" i32 NuPad_Interface_GetMaxDevices(void) {
    return 14;
}

extern "C" void NuPadOpenPS(FRAME_PAD *pad) {
    pad->flags |= 4;
}

extern "C" void fun_006d60c0(void) {
}

extern "C" void NuPadClosePS(FRAME_PAD *pad) {
    pad->flags &= ~4;
}

extern "C" void NuPadInitPS(FRAME_SCANNED_PAD *pad) {
}

extern "C" i32 PcInputKeyboardDeadzone(void) {
    return nu_input_state->devices[10].alternate_deadzone;
}

extern "C" void PcInputSetKeyboardDeadzone(i32 value) {
    if (value < 1) {
        value = 1;
    } else if (value > 10) {
        value = 10;
    }
    nu_input_state->devices[10].alternate_deadzone = value;
}

extern "C" void NuKeyboardEnableAllDevices(void) {
    nu_keyboard_device_flags = 7;
}

extern "C" void NuKeyboardEnableKeyboardMouse(void) {
    nu_keyboard_device_flags = 3;
}

extern "C" void NuKeyboardDisableDevices(void) {
    nu_keyboard_device_flags = 0;
}

extern "C" {

    void TextInputSetKeyboardOverride(i32 mode) {
        PC_INPUT_RUNTIME *manager = nu_input_state;
        i32 previous = manager->keyboard_ui_override;
        manager->keyboard_ui_override = (u8)mode;
        if (mode != previous) {
            nu_keyboard_repeat_countdown = 6;
        }
    }
}

extern "C" i32 AreaGetKeyboardOverride(void) {
    return nu_input_state->keyboard_ui_override;
}

extern "C" void SetPadDescriptor(i32 product_id, const u16 *descriptor) {
    if (descriptor == NULL) {
        return;
    }
    PC_INPUT_CONFIG *config = NULL;
    /* Scans 24 table slots although the static table holds 15 configs. */
    for (u32 index = 0; index < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++index) {
        if (PcStaticInputConfigs[index].product_id == (u32)product_id) {
            config = &PcStaticInputConfigs[index];
            break;
        }
    }
    if (config == NULL) {
        return;
    }
    for (i32 index = 0; index < 4; ++index) {
        config->axes[index][0] = 10;
        config->axes[index][1] = (u8)descriptor[0];
        config->axes[index][2] = (u8)descriptor[1];
        descriptor += 2;
    }
    for (i32 index = 0; index < 4; ++index) {
        config->axis_bindings[index][0] = 0;
        config->axis_bindings[index][1] = 0;
        config->axis_bindings[index][2] = 0;
    }
    for (i32 index = 0; index < 16; ++index) {
        if ((i16)*descriptor != -1) {
            config->buttons[index][0] = 7;
            config->buttons[index][1] = (u8)*descriptor;
        } else {
            config->buttons[index][0] = 0;
            config->buttons[index][1] = 0;
        }
        ++descriptor;
    }
    while ((i16)*descriptor != -1) {
        config->buttons[(i16)descriptor[0]][0] = (u8)descriptor[1];
        config->buttons[(i16)descriptor[0]][1] = (u8)descriptor[2];
        descriptor += 3;
    }
}

extern "C" void InitPadDescriptors(const u16 *first, const u16 *second, const u16 *third, const u16 *fourth, const i8 *order) {
    SetPadDescriptor(3, first);
    SetPadDescriptor(1, second);
    SetPadDescriptor(4, third);
    SetPadDescriptor(2, fourth);
    PcInputActionOrder[0] = -1;
    if (order != NULL) {
        i32 index = 0;
        i32 value;
        do {
            value = *order++;
            PcInputActionOrder[index++] = value;
        } while (value >= 0);
    }
}

extern "C" char *PcInputDeviceName(i32 port) {
    return nu_input_state->devices[port].name;
}

extern "C" void PcInputSuppressKeyOne(i32 value) {
    nu_input_state->suppress_key_1 = (u8)value;
}

extern "C" {
    i8 TextInputLastDeviceIndex = 0;

    void PcInput_SetCtrlStringPlayer(i32 device) {
        TextInputLastDeviceIndex = (i8)device;
    }

    i32 PcInput_GetCtrlStringPlayer(void) {
        return TextInputLastDeviceIndex;
    }
}

extern "C" f32 PcInputRampCurve(f32 period, f32 time) {
    time /= period;
    return 1.0f - 1.0f / NuPow(2.0f, time);
}

extern "C" void PcInputSeekInt(i32 *value, i32 target, f32 rate) {
    i32 current = *value;
    f32 step = (target - current) * rate;
    step += current;
    *value = step;
}

extern "C" void PcInputSeekFloat(f32 *value, f32 target, f32 rate) {
    target -= *value;
    target *= rate;
    *value += target;
}

extern "C" {
    f32 PcMouseInterpolation = 0;
}

static void PcMouseUpdateInterpolation(void) {
    f32 elapsed = NuTimeGetFrameTime();
    f32 exponent = (f32)(elapsed * 4.0);
    PcMouseInterpolation = 1.0f - 1.0f / NuPow(2.0f, exponent);
}

extern "C" {
    i32 PcMouseCurrentAngle = 0;
    i32 PcMouseTargetAngle = 0;
    f32 PcMouseRadius = 0;
}

void PC_INPUT_RUNTIME::ApplyMouseAxis(PC_INPUT_SAMPLE *sample) {
    (void)NuTimeGetFrameTime();
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    f32 goal = 0.0f;
    f32 x = (f32)(runtime->mouse.delta[0] * -4.0);
    f32 y = (f32)(runtime->mouse.delta[1] * -4.0);
    f32 square = x * x + y * y;
    if (square > 100000.0f) {
        PcMouseTargetAngle = NuAtan2D(y, x);
        i32 difference = PcMouseTargetAngle - PcMouseCurrentAngle;
        if (difference > 32768) {
            PcMouseCurrentAngle += 65536;
        } else if (difference < -32768) {
            PcMouseCurrentAngle -= 65536;
        }
    }
    PcMouseUpdateInterpolation();
    i32 difference = PcMouseTargetAngle - PcMouseCurrentAngle;
    f32 movement = PcMouseInterpolation * (f32)difference;
    f32 angle = movement + (f32)PcMouseCurrentAngle;
    PcMouseCurrentAngle = (i32)angle;
    difference = PcMouseTargetAngle - PcMouseCurrentAngle;
    if (difference > 32768) {
        difference -= 65536;
    } else if (difference < -32768) {
        difference += 65536;
    }
    if (abs(difference) > 100) {
        goal = 0.3f;
    }
    if (sample->analog[1] < 112) {
        goal = 0.7f;
    }
    if (sample->analog[1] > 144) {
        goal = 1.0f;
    }
    f32 radius_delta = goal - PcMouseRadius;
    radius_delta *= PcMouseInterpolation;
    PcMouseRadius += radius_delta;
    i32 cosine = ((PcMouseCurrentAngle + 16384) >> 1) & 32767;
    i32 sine = (PcMouseCurrentAngle >> 1) & 32767;
    f32 fx = -NuTrigTable[cosine] * PcMouseRadius;
    fx += 1.0f;
    f32 fy = -NuTrigTable[sine] * PcMouseRadius;
    fy += 1.0f;
    sample->analog[0] = (u8)(i32)(fy * 127.5);
    f32 scaled_x = (f32)(fx * 127.5);
    sample->analog[1] = (u8)(i32)scaled_x;
}

extern "C" void PcInputSetMouseField(i32 value) {
    nu_input_state->mouse.custom_bindings_active = value;
}

extern "C" void PcInputSetCapturedPort(i32 port) {
    nu_input_state->captured_port = port;
}

extern "C" i32 PcInputCapturedPort() {
    return nu_input_state->captured_port;
}

extern "C" void PcInput_DisableAllKeyboards(void) {
    PC_INPUT_RUNTIME *state = nu_input_state;
    for (i32 device = 10; device < 14; ++device) {
        state->devices[device].action_keys_active = 0;
    }
}

extern "C" void PcInputSetBlockedKeys(const char *keys) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->blocked_keys[0] = 0;
    if (keys != NULL) {
        NuStrCpy(runtime->blocked_keys, keys);
    }
}

extern "C" i32 PcInputXInputBackPressed(void) {
    PC_INPUT_RUNTIME *state = nu_input_state;
    i32 i;
    for (i = 0; i <= 3; ++i) {
        if (state->devices[i].kind == PC_INPUT_DEVICE_XINPUT && state->devices[i].back_pressed != 0) {
            return 1;
        }
    }
    return 0;
}

extern "C" {

    i32 PcInput_PortIsKeyboard(i32 device) {
        return (u32)device - 10u <= 3u;
    }
}

extern "C" i32 PcInputIsMouseDevice(i32 port) {
    return port == 10 || port == 12;
}

extern "C" i32 PcInputIsJoystickDevice(i32 port) {
    return (u32)port <= 9;
}

extern "C" {

    i32 PcInput_PortIsUC(i32 device) {
        return (u32)device <= 3u;
    }
}

extern "C" i32 PcInputHashAuxiliaryBindings() {
    return nu_input_state->HashAuxiliaryBindings();
}

extern "C" i32 PcInputMixConfigurationHash(i32 previous, i32 value) {
    if (previous & 0x80000000) {
        return (previous * 2 + 1) ^ value ^ 0x12345678;
    }
    return (previous * 2) ^ value ^ 0x12345678;
}

extern "C" void PcInputInstallConfig(PC_INPUT_CONFIG *config, i32 port, i32 key, i32 auxiliary, i32 bank) {
    nu_input_state->InstallConfig(config, port, key, auxiliary, bank);
}

extern "C" void PcInputStageAuxiliaryConfig(i32 port, i32 bank, i32 key) {
    PC_INPUT_CONFIG config;
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->ExportConfig(port, &config, 1, bank, key);
    runtime->PromoteConfig(runtime->primary, 64, &config);
}

extern "C" i32 PcInputBindingPresent(i32 port, i32 action, i32 auxiliary, i32 bank) {
    return auxiliary ? nu_input_state->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE
                     : nu_input_state->devices[port].bindings[action].kind != PC_BINDING_NONE;
}

extern "C" PC_INPUT_CONFIG *PcInputFindConfig(u8 mask, i32 port, i32 key) {
    return nu_input_state->FindConfig(mask, port, key);
}

extern "C" {
    volatile u8 PcKeyQueueHead = 0;
    volatile u8 PcKeyQueueTail = 0;
    volatile u8 PcKeyQueueScanCodes[256] = {0};
    volatile u8 PcKeyQueueModifiers[256] = {0};
}

extern "C" i32 NuKeyGet(u32 *modifiers) {
    i32 scan_code;
    if (PcKeyQueueTail != PcKeyQueueHead) {
        scan_code = PcKeyQueueScanCodes[PcKeyQueueHead];
        if (modifiers != NULL) {
            *modifiers = PcKeyQueueModifiers[PcKeyQueueHead];
        }
        PcKeyQueueHead = PcKeyQueueHead + 1;
        return scan_code;
    }
    return -1;
}

extern "C" void NuKeyFlush(void) {
    PcKeyQueueHead = PcKeyQueueTail;
}

extern "C" i32 QueueKeyPress(i32 scan_code, i32 modifiers) {
    if ((u8)(PcKeyQueueTail + 1) != PcKeyQueueHead) {
        PcKeyQueueScanCodes[PcKeyQueueTail] = (u8)scan_code;
        PcKeyQueueModifiers[PcKeyQueueTail] = (u8)modifiers;
        PcKeyQueueTail = PcKeyQueueTail + 1;
        return 1;
    }
    return 0;
}

extern "C" {
    u8 PcPressedScanCodes[256] = {0};
}

extern "C" void PcKeyDown(i32 scan_code) {
    PcPressedScanCodes[scan_code] = 1;
    i32 modifiers = 0;
    if (PcPressedScanCodes[42] || PcPressedScanCodes[54]) {
        modifiers = 1;
    }
    if (PcPressedScanCodes[29] || PcPressedScanCodes[157]) {
        modifiers |= 2;
    }
    if (PcPressedScanCodes[184]) {
        modifiers |= 4;
    }
    switch (scan_code) {
        case 29:
        case 42:
        case 54:
        case 56:
        case 157:
        case 184:
            return;
        default:
            QueueKeyPress(scan_code, modifiers);
            break;
    }
}

extern "C" {
    f32 FmvOverlayTimer = 0.0f;
}

extern "C" void MenuSetPlatformState(i32 state) {
}

/* Empty; its presentation caller consumes no return value. */
extern "C" void PCPresentationPlatformLeaf(void) {
}

extern "C" void FmvPlatformFinish(void) {
}

extern "C" void fun_006d6b60(void) {
}

extern "C" i32 PcKeyboardKeyAutoRepeat(i32 key) {
    if (nu_keyboard_key_index == 0 || key != nu_keyboard_key_index) {
        return 0;
    }
    if (nu_keyboard_no_key_state == 0) {
        FmvOverlayTimer = 0.5f;
        nu_keyboard_no_key_state = 1;
        return 1;
    }
    FmvOverlayTimer = FmvOverlayTimer - NuTimeGetFrameTime();
    if (FmvOverlayTimer <= 0.0f) {
        FmvOverlayTimer += 0.125;
        if (FmvOverlayTimer <= 0.0f) {
            FmvOverlayTimer = 0.0f;
        }
        return 1;
    }
    return 0;
}

extern "C" {

    i32 NuKeyboard(i32 key) {
        return (i8)nu_keyboard_pages[nu_keyboard_page_index * 0x100 + key];
    }

    i32 NuKeyboard_db(i32 key) {
        i32 page = nu_keyboard_page_index;
        u8 current = nu_keyboard_pages[(page << 8) + key];
        if (current != 0 && (current ^ nu_keyboard_pages[((page ^ 1) << 8) + key]) != 0) {
            return 1;
        }
        return 0;
    }

    i32 NuKeyboardGetPreviousKeyIndex(void) {
        return nu_keyboard_previous_key_index;
    }

    i32 NuKeyboardGetKeyIndex(void) {
        return nu_keyboard_key_index;
    }

    i32 PcKeyboardPressedKey = 0;
    i32 PcKeyboardPreviousKey = 0;
    i32 PcKeyboardHeldKey = 0;

    void PcKeyboardReadKey(i32 *key, i32 *pressed, i32 *shift, i32 *repeat) {
        i32 page = nu_keyboard_page_index << 8;
        i32 i;
        PcKeyboardPreviousKey = PcKeyboardHeldKey;
        if (nu_keyboard_pages[page + PcKeyboardHeldKey] == 0) {
            PcKeyboardHeldKey = 0;
        }
        for (i = 0; i < nu_keyboard_owner.key_count; ++i) {
            if (i != 0x2a && i != 0x36 && nu_keyboard_pages[page + i] != 0 && i != PcKeyboardPreviousKey) {
                PcKeyboardHeldKey = i;
                break;
            }
        }
        PcKeyboardPressedKey = PcKeyboardPreviousKey != PcKeyboardHeldKey ? PcKeyboardHeldKey : 0;
        if (shift != NULL) {
            *shift = nu_keyboard_pages[page + 0x2a] != 0 || nu_keyboard_pages[page + 0x36] != 0;
        }
        if (key != NULL) {
            *key = PcKeyboardHeldKey;
        }
        if (pressed != NULL) {
            *pressed = PcKeyboardPressedKey;
        }
        *repeat = PcKeyboardKeyAutoRepeat(PcKeyboardHeldKey) ? PcKeyboardHeldKey : 0;
    }
}

extern "C" void PcKeyUp(i32 scan_code) {
    PcPressedScanCodes[scan_code] = 0;
}

extern "C" {
    char **TextInputKeyNames = TextInputKeyNameTable;

    char **PcInput_GetKeyNameTable(void) {
        return TextInputKeyNames;
    }

    i32 (*PcInputCommandFilter)(i32, i32) = NULL;
}

extern "C" void PcInput_SetIsUsedCallBack(i32 (*filter)(i32, i32)) {
    PcInputCommandFilter = filter;
}

extern "C" PC_INPUT_COMMAND_FILTER PcInputGetCommandFilter() {
    return PcInputCommandFilter;
}

i32 PC_INPUT_RUNTIME::KeyboardPortFor(i32 port, i32 index) {
    if (port >= 10 && port <= 13) {
        if (index != captured_port) {
            ++port;
        }
        if (index >= 0) {
            port += index * 2;
        }
    }
    return port;
}

void PC_INPUT_RUNTIME::WriteConfigText(const char *directory, PC_INPUT_CONFIG *config) {
    char name[64];
    char path[1024];
    char text[2036];
    if (!config->name[0]) {
        return;
    }
    SanitizeConfigName(config->name, name);
    sprintf(path, "%s%s_v%d.txt", directory, name, config->key);
    FILE *file = PcOpenPlatformFile(path, "", 0, "w");
    if (file != NULL) {
        FormatConfigText(text, config);
        fwrite(text, NuStrLen(text), 1, file);
        fclose(file);
    }
}

void PC_INPUT_RUNTIME::ExportPortConfig(i32 port, PC_INPUT_CONFIG *config) {
    ExportConfig(port, config, 0, 0, 0);
}

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::FindPortConfig(u8 mask, i32 port) {
    return FindConfig(mask, port, 0);
}

void PC_INPUT_RUNTIME::InstallConfigFor(u8 mask, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port, i32 fallback) {
    PC_INPUT_CONFIG *config = FindConfig(mask, port, key);
    if (config == NULL) {
        if (!fallback) {
            return;
        }
        config = FindConfig(mask, port, -1);
        if (config == NULL) {
            return;
        }
    }
    InstallConfig(config, port, config->key, auxiliary, auxiliary_port);
}

i32 PC_INPUT_RUNTIME::BindingAvailable(i32 bank, i32 bank_count, i32 kind, i32 value) {
    i32 port;
    const char *key;
    i32 blocked;
    const i8 *order;
    PC_INPUT_BINDING *bindings;
    i32 other;
    i32 action;

    port = auxiliary_ports[bank];
    if (port < 10 || port > 13) {
        return 1;
    }
    if (kind == PC_BINDING_KEY) {
        key = blocked_keys;
        while ((blocked = *key++) != 0) {
            if (value == blocked) {
                return 0;
            }
        }
    }
    bindings = auxiliary_bindings[0];
    for (other = 0; other < bank_count; ++other, bindings += 32) {
        if (other != bank) {
            port = auxiliary_ports[other];
            if (port != -1 && port >= 10 && port <= 13) {
                if (PcInputActionOrder[0] >= 0) {
                    order = PcInputActionOrder;
                    while ((action = *order++) >= 0) {
                        if (value == bindings[action].value && kind == bindings[action].kind) {
                            return 0;
                        }
                    }
                } else {
                    for (action = 0; action < 32; ++action) {
                        if (value == bindings[action].value && kind == bindings[action].kind) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

i32 PC_INPUT_RUNTIME::ResolveAuxiliaryConflicts(i32 auxiliary_index, i32 bank_count) {
    if ((u32)(auxiliary_ports[auxiliary_index] - 10) > 3) {
        return 0;
    }
    PC_INPUT_BINDING *bindings = auxiliary_bindings[auxiliary_index];
    i32 missing = 0;
    PC_INPUT_BINDING *peer_bindings = auxiliary_bindings[0];
    i32 *peer_port = auxiliary_ports;
    for (i32 bank = 0; bank < bank_count; ++bank, peer_bindings += 32, ++peer_port) {
        if (bank == auxiliary_index || (u32)(*peer_port - 10) > 3) {
            continue;
        }
        if (PcInputActionOrder[0] >= 0) {
            i8 *first_order = PcInputActionOrder;
            for (i32 first = *first_order; first >= 0; first = *++first_order) {
                PC_INPUT_BINDING *first_binding = &bindings[first];
                if (!first_binding->kind) {
                    continue;
                }
                i8 *second_order = PcInputActionOrder;
                for (i32 second = *second_order; second >= 0; second = *++second_order) {
                    PC_INPUT_BINDING *selected_binding = &bindings[second];
                    PC_INPUT_BINDING *peer_binding = &peer_bindings[second];
                    if (selected_binding->kind && selected_binding->value != 255 && first_binding->value == peer_binding->value &&
                        first_binding->kind == peer_binding->kind) {
                        first_binding->value = 0;
                        first_binding->kind = PC_BINDING_NONE;
                    }
                }
            }
        } else {
            PC_INPUT_BINDING *first_binding = bindings;
            for (i32 first = 0; first < 32; ++first, ++first_binding) {
                if (!first_binding->kind) {
                    continue;
                }
                PC_INPUT_BINDING *selected_binding = bindings;
                PC_INPUT_BINDING *peer_binding = peer_bindings;
                for (i32 second = 0; second < 32; ++second, ++selected_binding, ++peer_binding) {
                    if (selected_binding->kind && first_binding->value == peer_binding->value &&
                        first_binding->kind == peer_binding->kind) {
                        first_binding->value = 0;
                        first_binding->kind = PC_BINDING_NONE;
                    }
                }
            }
        }
    }
    for (i32 index = 0; PcInputActionOrder[index] >= 0; ++index) {
        i32 command = PcInputActionOrder[index];
        if ((PcInputCommandFilter == NULL || PcInputCommandFilter(auxiliary_ports[auxiliary_index], command)) &&
            bindings[command].kind == PC_BINDING_NONE) {
            missing = 1;
            break;
        }
    }
    return missing;
}

i32 PC_INPUT_RUNTIME::SelectConfiguration(i32 port, i32 key, i32 auxiliary_index) {
    PC_INPUT_CONFIG *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL) {
            break;
        }
        if (++key == 3) {
            key = 0;
        }
    }
    InstallConfig(config, port, key, auxiliary_index >= 0, auxiliary_index);
    if (auxiliary_index >= 0 && (u32)(port - 10) <= 3) {
        for (i32 device = 0; device < 3; ++device) {
            (void)NuPadGetPort(device);
        }
        if (ResolveAuxiliaryConflicts(auxiliary_index, 4)) {
            config = NULL;
        }
    }
    return config != NULL;
}

u8 PC_INPUT_RUNTIME::InstallDropInConfiguration(i32 port, i32 player_index) {
    for (i32 bank = 0; bank < 3; ++bank) {
        if (bank == player_index) {
            continue;
        }
        i32 linked_port = NuPadGetPort(bank);
        if (linked_port < 0) {
            continue;
        }
        auxiliary_ports[bank] = linked_port;
        PCByteCopy(auxiliary_bindings[bank], devices[linked_port].bindings, 0x80);
    }
    i32 key = player_index;
    PC_INPUT_CONFIG *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL) {
            break;
        }
        if (++key == 3) {
            key = 0;
        }
    }
    if (config == NULL) {
        return 0;
    }
    InstallConfig(config, port, player_index, 1, player_index);
    return ResolveAuxiliaryConflicts(player_index, 3) == 0;
}

i32 PC_INPUT_RUNTIME::SelectConfigurationKey(i32 key, i32 port) {
    return SelectConfiguration(port, key, key);
}

void PC_INPUT_RUNTIME::ClearOtherBankConflicts(i32 bank, i32 bank_count) {
    PC_INPUT_BINDING *source;
    PC_INPUT_BINDING *target;
    i32 port;
    i32 other;
    i32 action;

    port = auxiliary_ports[bank];
    if (port < 10 || port > 13) {
        return;
    }
    source = auxiliary_bindings[bank];
    for (other = 0; other < bank_count; ++other) {
        if (other != bank) {
            port = auxiliary_ports[other];
            if (port >= 10 && port <= 13) {
                target = auxiliary_bindings[other];
                for (action = 0; action < 32; ++action) {
                    if (source[action].value == target[action].value && source[action].kind == target[action].kind) {
                        target[action].value = 0;
                        target[action].kind = PC_BINDING_NONE;
                    }
                }
            }
        }
    }
}

i32 PC_INPUT_RUNTIME::SampleDevice(i32 port, PC_INPUT_RAW_SAMPLE *sample) {
    memset(sample->axes, 0, sizeof(sample->axes));
    memset(sample->buttons, 0, sizeof(sample->buttons));
    if (port < 0) {
        return 0;
    }
    if ((u32)(port - 10) <= 3) {
        if (nu_keyboard_owner.poll_status == 0) {
            return 0;
        }
        return (i8)devices[port].action_keys_active;
    }
    if (port < 4) {
        devices[port].back_pressed = 0;
        if (devices[port].kind != PC_INPUT_DEVICE_XINPUT) {
            return 0;
        }
        PC_XINPUT_STATE state;
        if (XInputGetState(port, (XINPUT_STATE *)&state) != 0) {
            return 0;
        }
        if (!devices[port].capabilities_valid) {
            PC_XINPUT_CAPABILITIES caps;
            devices[port].capabilities_valid = XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0;
            if (devices[port].capabilities_valid) {
                InitXInputCaps(&caps, &devices[port]);
            }
        }
        sample->buttons[7] = (u8)((state.gamepad.buttons >> 4) & 1);
        sample->buttons[6] = (u8)((state.gamepad.buttons >> 5) & 1);
        devices[port].back_pressed = sample->buttons[6];
        sample->buttons[128] = (u8)(state.gamepad.buttons & 1);
        sample->buttons[130] = (u8)((state.gamepad.buttons >> 1) & 1);
        sample->buttons[131] = (u8)((state.gamepad.buttons >> 2) & 1);
        sample->buttons[129] = (u8)((state.gamepad.buttons >> 3) & 1);
        sample->buttons[3] = (u8)((state.gamepad.buttons >> 15) & 1);
        sample->buttons[1] = (u8)((state.gamepad.buttons >> 13) & 1);
        sample->buttons[0] = (u8)((state.gamepad.buttons >> 12) & 1);
        sample->buttons[5] = (u8)((state.gamepad.buttons >> 9) & 1);
        sample->buttons[2] = (u8)((state.gamepad.buttons >> 14) & 1);
        sample->buttons[4] = (u8)((state.gamepad.buttons >> 8) & 1);
        sample->buttons[8] = (u8)((state.gamepad.buttons >> 6) & 1);
        sample->buttons[9] = (u8)((state.gamepad.buttons >> 7) & 1);
        sample->axes[0] = state.gamepad.thumb_lx;
        if (state.gamepad.thumb_ly == -32768) {
            state.gamepad.thumb_ly = -32767;
        }
        sample->axes[1] = (i16)-state.gamepad.thumb_ly;
        sample->axes[3] = state.gamepad.thumb_rx;
        if (state.gamepad.thumb_ry == -32768) {
            state.gamepad.thumb_ry = -32767;
        }
        sample->axes[4] = (i16)-state.gamepad.thumb_ry;
        sample->axes[6] = (i16)((u16)state.gamepad.left_trigger << 7);
        sample->axes[7] = (i16)((u16)state.gamepad.right_trigger << 7);
        NormalizeAxisPair(&sample->axes[0], &sample->axes[1]);
        NormalizeAxisPair(&sample->axes[3], &sample->axes[4]);
    } else {
        NuKeyboardDevice *device = devices[port].device;
        if (device == NULL) {
            return 0;
        }
        if (device->methods->poll(device) < 0) {
            device->methods->acquire(device);
            return 0;
        }
        PC_DI_JOYSTICK_STATE state;
        if (device->methods->get_state(device, 0x110, &state) != 0) {
            return 0;
        }
        /* Each DIJOYSTATE2 axis field is copied by name. */
        sample->axes[0] = state.axes[0];
        sample->axes[1] = state.axes[1];
        sample->axes[2] = state.axes[2];
        sample->axes[3] = state.axes[3];
        sample->axes[4] = state.axes[4];
        sample->axes[5] = state.axes[5];
        sample->axes[6] = state.axes[6];
        sample->axes[7] = state.axes[7];
        sample->axes[8] = state.extended_axes[0];
        sample->axes[9] = state.extended_axes[1];
        sample->axes[10] = state.extended_axes[2];
        sample->axes[11] = state.extended_axes[3];
        sample->axes[12] = state.extended_axes[4];
        sample->axes[13] = state.extended_axes[5];
        sample->axes[14] = state.extended_axes[6];
        sample->axes[15] = state.extended_axes[7];
        sample->axes[16] = state.extended_axes[8];
        sample->axes[17] = state.extended_axes[9];
        sample->axes[18] = state.extended_axes[10];
        sample->axes[19] = state.extended_axes[11];
        sample->axes[20] = state.extended_axes[12];
        sample->axes[21] = state.extended_axes[13];
        sample->axes[22] = state.extended_axes[14];
        sample->axes[23] = state.extended_axes[15];
        sample->axes[24] = state.extended_axes[16];
        sample->axes[25] = state.extended_axes[17];
        sample->axes[26] = state.extended_axes[18];
        sample->axes[27] = state.extended_axes[19];
        sample->axes[28] = state.extended_axes[20];
        sample->axes[29] = state.extended_axes[21];
        sample->axes[30] = state.extended_axes[22];
        sample->axes[31] = state.extended_axes[23];
        PCByteCopy(sample->buttons, state.buttons, devices[port].button_count);
        if (state.pov[3] >= 0) {
            sample->buttons[128] = PcInputPovBits[state.pov[3] / 4500] & 1;
            sample->buttons[129] = (PcInputPovBits[state.pov[3] / 4500] >> 1) & 1;
            sample->buttons[130] = (PcInputPovBits[state.pov[3] / 4500] >> 2) & 1;
            sample->buttons[131] = (PcInputPovBits[state.pov[3] / 4500] >> 3) & 1;
        } else {
            sample->buttons[131] = 0;
            sample->buttons[130] = 0;
            sample->buttons[129] = 0;
            sample->buttons[128] = 0;
        }
    }
    return 1;
}

i32 PC_INPUT_RUNTIME::UpdateMouse() {
    if (!(nu_keyboard_device_flags & 1) && !mouse_polled) {
        i32 button_count = nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons ? nu_keyboard_owner.mouse_buttons : 0;
        mouse_polled = 1;
        const PC_DI_MOUSE_STATE *state = nu_keyboard_owner.PollMouse();
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.previous[axis] = mouse.current[axis];
        }
        if (state != NULL) {
            f32 multiplier = (f32)devices[10].alternate_deadzone * 2.0f;
            multiplier *= multiplier;
            for (i32 axis = 0; axis < 3; ++axis) {
                mouse.current[axis] = (i16)state->axes[axis];
            }
            if (first_mouse) {
                first_mouse = 0;
                for (i32 axis = 0; axis < 3; ++axis) {
                    mouse.accumulated[axis] = 0;
                    mouse.previous[axis] = mouse.current[axis];
                }
                mouse.position[0] = 0.5f;
                mouse.position[1] = 0.5f;
            }
            for (i32 axis = 0; axis < 3; ++axis) {
                f32 change = (f32)(mouse.current[axis] - mouse.previous[axis]);
                if (change > 32767.0) {
                    change = (f32)(change - 65536.0);
                }
                if (change < -32768.0f) {
                    change = (f32)(change + 65536.0);
                }
                mouse.delta[axis] = axis < 2 ? multiplier * change : change;
            }
            mouse.accumulated[0] += mouse.delta[0] * 0.00390625;
            mouse.accumulated[1] += mouse.delta[1] * 0.00390625;
            mouse.accumulated[2] += mouse.delta[2];
            f32 reciprocal = 1.0f / PcInputAspectRatio();
            mouse.position[0] += mouse.delta[0] * reciprocal * 7.62939453125e-6;
            mouse.position[1] += mouse.delta[1] * 7.62939453125e-6;
            if (mouse.position[0] < 0.0f) {
                mouse.position[0] = 0.0f;
            } else if (mouse.position[0] > 1.0f) {
                mouse.position[0] = 1.0f;
            }
            if (mouse.position[1] < 0.0f) {
                mouse.position[1] = 0.0f;
            } else if (mouse.position[1] > 1.0f) {
                mouse.position[1] = 1.0f;
            }
            u32 previous = mbuttons;
            mbuttons = 0;
            u32 mask = 0;
            for (i32 button = 0; button < button_count; ++button) {
                if (mouse.buttons[button]) {
                    mask |= 1u << button;
                }
            }
            mbuttons = mask;
            PcMousePressedMask = (mask ^ previous) & mask;
            memset(mouse.buttons, 0, 8);
            PCByteCopy(mouse.buttons, state->buttons, button_count);
            return 1;
        }
        memset(mouse.buttons, 0, sizeof(mouse.buttons));
        mouse.accumulated[0] = mouse.accumulated[1] = mouse.accumulated[2] = mouse.delta[0] = mouse.delta[1] = mouse.delta[2] = 0.0f;
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.previous[axis] = mouse.current[axis];
        }
    }
    return 0;
}

extern "C" {
    f32 PcMouseAxisIdle[3] = {0};
}

i32 PC_INPUT_RUNTIME::TranslateBindings(i32 port, PC_INPUT_SAMPLE *out, i32 valid, PC_INPUT_RAW_SAMPLE *raw, PC_INPUT_MOUSE_RUNTIME *motion,
                                        u8 **keys) {
    i32 step;
    if (ramp_seconds == 0.0f) {
        step = 32768;
    } else {
        step = (i32)(NuTimeGetFrameTime() * (32768.0 / ramp_seconds));
        if (step == 0) {
            step = 1;
        }
    }
    if (port >= 10 && port < 14 && keyboard_ui_override) {
        step = 32768;
    }
    i16 sums[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u32 buttons = 0;
    for (i32 n = 0; n < shutdown_state; ++n) {
        PC_INPUT_BINDING *binding = &devices[port].bindings[n];
        PC_INPUT_COMMAND *command = &commands[n];
        i16 *retained = (i16 *)&devices[port].reset_pairs[n].first;
        i32 index = binding->value;
        u8 pressed = 0;
        u8 digital = 1;
        if (index >= 0) {
            i32 value;
            switch (binding->kind) {
                case PC_BINDING_MOUSE_BUTTON:
                    pressed = motion->buttons[index];
                    break;
                case PC_BINDING_MOUSE_AXIS:
                case PC_BINDING_MOUSE_AXIS_REVERSED:
                    if (valid) {
                        if (PcMouseAxisModes[index] == 0) {
                            value = (i32)motion->delta[index];
                            if (binding->kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                                value = -value;
                            }
                            pressed = value > 0;
                            value *= 8;
                        } else {
                            if (PcMouseAxisModes[index] == 2) {
                                if (motion->accumulated[index] > 256.0f) {
                                    motion->accumulated[index] = 256.0f;
                                }
                                if (motion->accumulated[index] < -256.0f) {
                                    motion->accumulated[index] = -256.0f;
                                }
                                i32 moved = (i32)motion->delta[index];
                                if (abs(moved) < 5) {
                                    PcMouseAxisIdle[index] += NuTimeGetFrameTime();
                                } else {
                                    PcMouseAxisIdle[index] = 0.0f;
                                }
                                if (PcMouseAxisIdle[index] > 0.5f) {
                                    f32 accumulated = motion->accumulated[index];
                                    if (accumulated != 0.0f) {
                                        if (accumulated > 0.0f && accumulated < 128.0f) {
                                            accumulated -= 1.0f;
                                        } else if (accumulated < 0.0f && accumulated > -128.0f) {
                                            accumulated += 1.0f;
                                        } else {
                                            accumulated = (f32)(accumulated - accumulated * 0.0078125);
                                        }
                                        motion->accumulated[index] = accumulated;
                                    }
                                }
                            }
                            value = (i32)motion->accumulated[index];
                            if (binding->kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                                value = -value;
                            }
                            if (value >= 32) {
                                value = ((value - 31) * 32512) / 96;
                            } else if (value < -32) {
                                value = ((value + 32) * 32512) / 96;
                            } else {
                                value = 0;
                            }
                        }
                        if (value > 32767) {
                            value = 32767;
                        }
                        if (value < -32767) {
                            value = -32767;
                        }
                        if (command->active == 2 || command->active == 3) {
                            *retained = value > 0 ? (i16)value : 0;
                        } else {
                            *retained = value < 0 ? (i16)value : 0;
                        }
                        digital = 0;
                    }
                    break;
                case PC_BINDING_KEY:
                    if (!suppress_key_1 || index != 1) {
                        pressed = (*keys)[index];
                    }
                    break;
                case PC_BINDING_JOYSTICK_BUTTON:
                    if (valid) {
                        pressed = raw->buttons[index];
                    }
                    break;
                case PC_BINDING_JOYSTICK_AXIS:
                case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                    if (valid) {
                        value = raw->axes[index];
                        if (binding->kind == PC_BINDING_JOYSTICK_AXIS_REVERSED) {
                            value = -value;
                        }
                        if (value > 32767) {
                            value = 32767;
                        }
                        if (command->active == 2 || command->active == 3) {
                            *retained = value > 0 ? (i16)value : 0;
                        } else {
                            *retained = value < 0 ? (i16)value : 0;
                        }
                        i32 magnitude = abs(*retained);
                        pressed = magnitude > axis_threshold;
                        digital = 0;
                    }
                    break;
            }
            if (command->button >= 0) {
                if (pressed || (!digital && (*retained > axis_threshold || *retained < -axis_threshold))) {
                    buttons |= 1u << command->button;
                }
            }
        }
        if (command->axis >= 0) {
            if (digital) {
                i32 value = *retained;
                if (pressed) {
                    if (command->active == 2 || command->active == 3) {
                        value += step;
                        if (value > 32767) {
                            value = 32767;
                        }
                    } else {
                        value -= step;
                        if (value < -32768) {
                            value = -32768;
                        }
                    }
                } else if (value < 0) {
                    value += step;
                    if (value > 0) {
                        value = 0;
                    }
                } else {
                    value -= step;
                    if (value < 0) {
                        value = 0;
                    }
                }
                *retained = (i16)value;
            }
            sums[command->axis] = (i16)((u16)sums[command->axis] + (u16)*retained);
        }
    }
    for (i32 axis = 0; axis < 4; ++axis) {
        out->analog[axis] = (u8)((sums[axis] + 32768) >> 8);
    }
    for (i32 axis = 4; axis < 8; ++axis) {
        out->analog[axis] = (u8)(sums[axis] >> 7);
    }
    out->buttons = buttons;
    return 1;
}

u32 PC_INPUT_RUNTIME::MergeKeyMouseButton(i32, u32 buttons, i32 key, i32 mouse_button, i32 axis, i32 direction, u32 bit) {
    if (key >= 0 && nu_keyboard_pages[nu_keyboard_page_index * 256 + key]) {
        buttons |= bit;
    }
    if (mouse_button >= 0 && mouse.buttons[mouse_button]) {
        buttons |= bit;
    }
    if (axis >= 0) {
        if ((direction > 0 && mouse.delta[axis] > 0.0f) || (direction < 0 && mouse.delta[axis] < 0.0f)) {
            buttons |= bit;
        }
    }
    return buttons;
}

u32 PC_INPUT_RUNTIME::CombineUiButtons(i32 port, u32 buttons) {
    if ((u32)(port - 10) <= 3) {
        buttons = 0;
    }
    const u8 *keys = nu_keyboard_pages + nu_keyboard_page_index * 256;
    if (keys[0x1c]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (mouse.buttons[0]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (keys[0x9c]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (keys[0x39]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (mouse.buttons[2]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (!suppress_key_1) {
        if ((u32)(port - 10) <= 3) {
            buttons &= ~PAD_BUTTON_START;
        }
        if (keys[1]) {
            buttons |= PAD_BUTTON_CIRCLE;
        }
    }
    if (keys[0xe]) {
        buttons |= PAD_BUTTON_CIRCLE;
    }
    if (mouse.buttons[1]) {
        buttons |= PAD_BUTTON_CIRCLE;
    }
    if (keys[0x3b]) {
        buttons |= PAD_BUTTON_SQUARE;
    }
    if (keys[0x3c]) {
        buttons |= PAD_BUTTON_TRIANGLE;
    }
    if (keys[0xcd]) {
        buttons |= PAD_BUTTON_RIGHT;
    }
    if (keys[0xcb]) {
        buttons |= PAD_BUTTON_LEFT;
    }
    if (keys[0xd0]) {
        buttons |= PAD_BUTTON_DOWN;
    }
    if (mouse.delta[2] < 0.0f) {
        buttons |= PAD_BUTTON_DOWN;
    }
    if (keys[0xc8]) {
        buttons |= PAD_BUTTON_UP;
    }
    if (mouse.delta[2] > 0.0f) {
        buttons |= PAD_BUTTON_UP;
    }
    return buttons;
}

void PC_INPUT_RUNTIME::CaptureBinding(i32 port) {
    u32 mask = 1u << port;
    if (!(action_mask & mask) || (completed_mask & mask)) {
        return;
    }
    completed_mask |= mask;
    action_mask &= ~mask;
    if ((u32)(port - 10) <= 3) {
        if (capture_available) {
            captures[port] = captures[10];
            return;
        }
        capture_available = 1;
        i32 key_count = nu_keyboard_owner.key_count;
        i32 groups = (key_count + 3) / 4;
        const u32 *words = (const u32 *)keyboard_state;
        for (i32 group = 0; group < groups; ++group) {
            if (words[group] != 0) {
                i32 key = group * 4;
                for (i32 lane = 0; lane < 4; ++lane, ++key) {
                    if (key >= key_count) {
                        break;
                    }
                    if (keyboard_state[key]) {
                        captures[10].kind[0] = PC_BINDING_KEY;
                        captures[10].value[0] = (i16)key;
                        break;
                    }
                }
                break;
            }
        }
        if (nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons && nu_keyboard_owner.mouse_axes >= 2 &&
            nu_keyboard_owner.mouse_buttons >= 2) {
            for (i32 button = 0; button < 8; ++button) {
                if (mouse.buttons[button]) {
                    captures[10].kind[1] = PC_BINDING_MOUSE_BUTTON;
                    captures[10].value[1] = (i16)button;
                    break;
                }
            }
            i32 x = (i32)mouse.accumulated[0];
            i32 y = (i32)mouse.accumulated[1];
            i32 z = (i32)mouse.accumulated[2];
            if (abs(x) > abs(y)) {
                if (abs(x) > motion_threshold) {
                    if (x > 0) {
                        captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                        captures[10].value[2] = 0;
                    } else {
                        captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                        captures[10].value[2] = 0;
                    }
                }
            } else if (abs(y) > motion_threshold) {
                if (y > 0) {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                    captures[10].value[2] = 1;
                } else {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                    captures[10].value[2] = 1;
                }
            }
            if (z) {
                if (z > 0) {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                    captures[10].value[2] = 2;
                } else {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                    captures[10].value[2] = 2;
                }
            }
        }
        return;
    }
    for (i32 axis = 0; axis < 32; ++axis) {
        if (raw_sample.axes[axis] > 16384) {
            captures[port].kind[2] = PC_BINDING_JOYSTICK_AXIS;
            captures[port].value[2] = (i16)axis;
            break;
        }
        if (raw_sample.axes[axis] < -16384) {
            captures[port].kind[2] = PC_BINDING_JOYSTICK_AXIS_REVERSED;
            captures[port].value[2] = (i16)axis;
            break;
        }
    }
    for (i32 button = 0; button < devices[port].button_count; ++button) {
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = PC_BINDING_JOYSTICK_BUTTON;
            captures[port].value[0] = (i16)button;
            break;
        }
    }
    for (i32 button = 128; button <= 131; ++button) {
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = PC_BINDING_JOYSTICK_BUTTON;
            captures[port].value[0] = (i16)button;
            break;
        }
    }
}

// Uncompiled native input_runtime.cpp lifecycle cohort.
void PC_INPUT_RUNTIME::ServicePendingCapture() {
    if (action_mask == 0) {
        return;
    }
    if (!keyboard_polled) {
        keyboard_polled = 1;
        keyboard_state = nu_keyboard_owner.PollKeyboard();
    }
    UpdateMouse();
    for (i32 port = 0; port < 14; ++port) {
        u32 mask = 1u << port;
        // Native 6d87a7 JNE skips SET bits: preserve this condition, even though
        // CaptureBinding then sees a clear bit and returns without recording.
        if (!(action_mask & mask)) {
            read_mask |= mask;
            action_mask &= ~mask;
            i32 valid = SampleDevice(port, &raw_sample);
            if (valid || (u32)(port - 10) <= 3) {
                CaptureBinding(port);
            }
        }
    }
}

extern "C" {
    TEXT_INPUT_LABELS *TextInputAlternateLabels = 0;
    TEXT_INPUT_LABELS *TextInputDefaultLabels = 0;
    u8 TextInputKeyboardOverride[24] = {0, 0, 0, 0, 0, 0, 0, 0, 200, 205, 208, 203, 60, 1, 28, 59, 0, 0, 0, 0, 0, 0, 0, 0};
    i8 TextInputAxisLabelIndex[16] = {14, 15, 16, 17, 0, 0, 18, 19, 20, 21, 0, 0, 28, 28, 29, 29};
    i8 TextInputButtonLabelIndex[11] = {24, 23, 25, 22, 26, 27, 33, 32, 30, 31, 0};
}

i32 PC_INPUT_RUNTIME::ButtonName(i32 device, i32 button, char *destination, i32 custom, i32 custom_index, i32 use_callback,
                                 i32 alternate_labels, char *delimiters) {
    i32 result = 0;
    i32 open = 0;
    i32 close = 0;
    if (button >= 256) {
        if (TextInputNameCallback != NULL) {
            TextInputNameCallback(button, destination);
        } else {
            NuStrCpy(destination, "GFX?");
        }
        return 1;
    }
    TEXT_INPUT_LABELS *labels = TextInputDefaultLabels;
    if (alternate_labels != 0) {
        labels = TextInputAlternateLabels;
    }
    i32 simple = device < 4;
    if (delimiters != NULL && delimiters[0] != 0 && delimiters[1] != 0) {
        open = delimiters[0];
        close = delimiters[1];
        if (open != 0) {
            *destination++ = (char)open;
        }
    }
    i32 type;
    i32 value;
    if (custom != 0) {
        type = auxiliary_bindings[custom_index][button].kind;
        value = auxiliary_bindings[custom_index][button].value;
    } else {
        type = devices[device].bindings[button].kind;
        value = devices[device].bindings[button].value;
    }
    if (keyboard_ui_override != 0 && mouse.custom_bindings_active == 0 && ((u32)(device - 10) <= 3 || device < 0) &&
        TextInputKeyboardOverride[button] > 0) {
        type = PC_BINDING_KEY;
        value = TextInputKeyboardOverride[button];
    }
    i32 expected = 2;
    switch (type) {
        case PC_BINDING_JOYSTICK_BUTTON: {
            char text[5];
            *destination = 0;
            if (simple != 0) {
                if (TextInputNameCallback != NULL && use_callback != 0) {
                    TextInputNameCallback(value, text);
                    if (text[0] != 0) {
                        if (open != 0) {
                            --destination;
                        }
                        NuStrCpy(destination, text);
                        result = 1;
                        break;
                    }
                }
                if (value <= 10) {
                    NuStrCpy(destination, labels->indexed_names[TextInputButtonLabelIndex[value]]);
                    break;
                }
            }
            if ((u32)(value - 128) <= 3) {
                NuStrCpy(destination, labels->indexed_names[value - 128]);
            } else {
                sprintf(destination, "%s %d", labels->button_prefix, value + 1);
            }
            break;
        }
        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
            expected = 1;
        case PC_BINDING_JOYSTICK_AXIS: {
            if (value < 0) {
                break;
            }
            PC_INPUT_COMMAND *command = &commands[button];
            if (command->active == 3 && type == PC_BINDING_JOYSTICK_AXIS) {
                expected = 3;
            }
            if (simple != 0 && value < 8) {
                char text[5] = "";
                value = value * 2 + (command->active == expected);
                *destination = 0;
                if (TextInputNameCallback != NULL) {
                    if (value == 13 || value == 14) {
                        TextInputNameCallback(11, text);
                    } else if (value == 15 || value == 12) {
                        TextInputNameCallback(10, text);
                    }
                    if (text[0] != 0) {
                        if (open != 0) {
                            --destination;
                        }
                        NuStrCpy(destination, text);
                        result = 1;
                        break;
                    }
                }
                NuStrCpy(destination, labels->indexed_names[TextInputAxisLabelIndex[value]]);
            } else if (value < 6) {
                if (command->active == expected) {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_positive);
                } else {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_negative);
                }
            } else {
                i32 reversed = NuLanguageGet() == NULANGUAGE_POLISH;
                if (command->active == expected) {
                    if (reversed == 0) {
                        sprintf(destination, labels->axis_format, value, labels->axis_positive);
                    } else {
                        sprintf(destination, labels->axis_format, labels->axis_positive, value);
                    }
                } else {
                    if (reversed == 0) {
                        sprintf(destination, labels->axis_format, value, labels->axis_negative);
                    } else {
                        sprintf(destination, labels->axis_format, labels->axis_negative, value);
                    }
                }
            }
            break;
        }
        case 12:
            if (commands[button].active == 1) {
                NuStrCpy(destination, labels->direction_positive[0]);
            } else {
                NuStrCpy(destination, labels->direction_negative[0]);
            }
            break;
        case 13:
            if (commands[button].active == 1) {
                NuStrCpy(destination, labels->direction_positive[1]);
            } else {
                NuStrCpy(destination, labels->direction_negative[1]);
            }
            break;
        case PC_BINDING_MOUSE_BUTTON:
            if ((u32)value <= 2) {
                NuStrCpy(destination, labels->hat_names[value]);
            } else {
                sprintf(destination, labels->hat_format, value + 1);
            }
            break;
        case PC_BINDING_MOUSE_AXIS_REVERSED:
            if ((u32)value <= 2) {
                if (commands[button].active == 2 || commands[button].active == 3) {
                    NuStrCpy(destination, labels->direction_positive[value]);
                } else {
                    NuStrCpy(destination, labels->direction_negative[value]);
                }
            }
            break;
        case PC_BINDING_MOUSE_AXIS:
            if ((u32)value <= 2) {
                if (commands[button].active == 2 || commands[button].active == 3) {
                    NuStrCpy(destination, labels->direction_negative[value]);
                } else {
                    NuStrCpy(destination, labels->direction_positive[value]);
                }
            }
            break;
        case PC_BINDING_KEY:
        case 8:
        case 9:
            if (value >= 1) {
                NuStrCpy(destination, TextInputKeyNames[value]);
            }
            break;
        default:
            NuStrCpy(destination, labels->fallback_name);
            break;
    }
    if (close != 0) {
        i32 length = NuStrLen(destination);
        destination[length] = (char)close;
        destination[length + 1] = 0;
    }
    return result;
}

void PC_INPUT_RUNTIME::AddInputRecord(i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04) {
    if (shutdown_state < 32) {
        InsertInputRecord(shutdown_state, unknown_00, field_01, unknown_02, unknown_04);
        ++shutdown_state;
    }
}

u8 PcInputAxisRemap[16] = {0};
u32 PcInputAxisRemapInitialized = 0;

void PC_INPUT_RUNTIME::AssignBinding(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    PC_INPUT_BINDING *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    i32 simple = (u32)port <= 3;
    i32 active = commands[action].active;
    i32 positive;
    i32 negative;
    if (kind == PC_BINDING_MOUSE_AXIS || kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
        positive = PC_BINDING_MOUSE_AXIS;
        negative = PC_BINDING_MOUSE_AXIS_REVERSED;
    } else {
        positive = PC_BINDING_JOYSTICK_AXIS;
        negative = PC_BINDING_JOYSTICK_AXIS_REVERSED;
    }
    if (!(PcInputAxisRemapInitialized & 1)) {
        PcInputAxisRemapInitialized |= 1;
        PcInputAxisRemap[10] = (u8)positive;
        PcInputAxisRemap[11] = (u8)positive;
        PcInputAxisRemap[12] = (u8)negative;
        PcInputAxisRemap[13] = (u8)negative;
        PcInputAxisRemap[14] = 0;
        PcInputAxisRemap[15] = 0;
    }
    if (kind != positive && kind != negative) {
        for (i32 i = 0; i < 32; ++i) {
            if (bindings[i].kind == kind && bindings[i].value == value && !(excluded & (1u << i))) {
                bindings[i].kind = PC_BINDING_NONE;
                bindings[i].value = 0;
            }
        }
        bindings[action].value = (u16)value;
        bindings[action].kind = (u8)kind;
        return;
    }
    if (active == 3) {
        bindings[action].kind = (u8)kind;
        bindings[action].value = (u16)value;
        for (i32 i = 0; i < 32; ++i) {
            i32 other_kind = bindings[i].kind;
            if (i != action && (other_kind == positive || other_kind == negative) && bindings[i].value == value) {
                i32 index = 0;
                if (kind == positive) {
                    index = 4;
                }
                if (other_kind == positive) {
                    index += 2;
                }
                if (commands[i].active == 2) {
                    ++index;
                }
                if (commands[i].active == 3) {
                    index += 8;
                }
                u8 remapped_kind = PcInputAxisRemap[index];
                bindings[i].kind = remapped_kind;
                bindings[i].value = (u16)(remapped_kind ? value : 0);
            }
        }
        return;
    }
    if (kind == positive && active == 2) {
        kind = positive;
    } else if (kind == negative && active == 1) {
        kind = positive;
    } else {
        kind = negative;
    }
    for (i32 i = 0; i < 32; ++i) {
        if ((bindings[i].kind == positive || bindings[i].kind == negative) && bindings[i].value == value && !(excluded & (1u << i))) {
            bindings[i].kind = PC_BINDING_NONE;
            bindings[i].value = 0;
        }
    }
    i32 axis = commands[action].axis;
    if (axis < 0) {
        return;
    }
    if (simple && (value == 6 || value == 7)) {
        bindings[action].value = (u16)value;
        bindings[action].kind = (u8)kind;
        return;
    }
    if (active == 2 || active == 1) {
        for (i32 i = 0; i < 32; ++i) {
            if ((commands[i].active == 2 || commands[i].active == 1) && commands[i].axis == axis) {
                bindings[i].kind = (u8)kind;
                bindings[i].value = (u16)value;
            }
        }
    }
}

void NuKeyboardOwner::Shutdown() {
    if (nu_input_state == NULL) {
        return;
    }
    PC_INPUT_DEVICE *devices = nu_input_state->devices;
    if (device != NULL) {
        device->methods->unacquire(device);
        if (device != NULL) {
            device->methods->release(device);
            device = NULL;
        }
    }
    for (u16 index = 10; index < 14; ++index) {
        devices[index].device = NULL;
    }
    for (i32 index = 4; index < 14; ++index) {
        if (devices[index].device != NULL) {
            devices[index].device->methods->unacquire(devices[index].device);
            if (devices[index].device != NULL) {
                devices[index].device->methods->release(devices[index].device);
                devices[index].device = NULL;
            }
        }
        devices[index].kind = PC_INPUT_DEVICE_NONE;
        devices[index].capabilities_valid = 0;
    }
    if (mouse != NULL) {
        mouse->methods->unacquire(mouse);
        mouse->methods->release(mouse);
        mouse = NULL;
    }
    if (factory != NULL) {
        factory->methods->release(factory);
        factory = NULL;
    }
    PC_INPUT_RUNTIME *released = nu_input_state;
    if (released != NULL) {
        released->shutdown_state = 0;
        ::operator delete(released);
    }
}

extern "C" i32 PcInput_ScanForAnyInput(u32 excluded, u8 kinds, i32 *kind, i32 *value, i32 skip_active) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->action_mask = (u32)-1;
    for (i32 port = 0; port < 14; ++port) {
        if ((skip_active && runtime->mouse.port_activity[port]) || (excluded & (1u << port))) {
            continue;
        }
        PC_INPUT_CAPTURE *capture = &runtime->staged_capture[port];
        i8 selected_kind;
        i32 selected_value;
        if ((u32)(port - 10) <= 3) {
            if ((kinds & 16) && capture->kind[0]) {
                selected_kind = capture->kind[0];
                selected_value = capture->value[0];
            } else if ((kinds & 8) && capture->kind[1]) {
                selected_kind = capture->kind[1];
                selected_value = capture->value[1];
            } else if ((kinds & 4) && capture->kind[2]) {
                selected_kind = capture->kind[2];
                selected_value = capture->value[2];
            } else {
                continue;
            }
        } else {
            if ((kinds & 2) && capture->kind[0]) {
                selected_kind = capture->kind[0];
                selected_value = capture->value[0];
            } else if ((kinds & 1) && capture->kind[2]) {
                selected_kind = capture->kind[2];
                selected_value = capture->value[2];
            } else {
                continue;
            }
        }
        if (!selected_kind) {
            return -1;
        }
        if (kind != NULL) {
            *kind = selected_kind;
        }
        if (value != NULL) {
            *value = selected_value;
        }
        return port;
    }
    return -1;
}

extern "C" void PcInputAssignBinding(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    nu_input_state->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
}

extern "C" void PcInputAssignButtonBinding(i32 button, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    i32 command_button;
    for (i32 action = 0; action < 32; ++action) {
        runtime->GetCommand(action, NULL, NULL, &command_button, NULL);
        if (command_button == button) {
            runtime->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
            return;
        }
    }
}

extern "C" void PcInputAssignAxisBinding(i32 axis, i32 active, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank,
                                         u32 excluded) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    i32 command_active;
    i32 command_axis;
    for (i32 action = 0; action < 32; ++action) {
        runtime->GetCommand(action, &command_active, &command_axis, NULL, NULL);
        if (command_axis == axis && command_active == active) {
            runtime->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
            return;
        }
    }
}

extern "C" void NuKeyboardRead(void) {
    nu_input_state->PollKeyboard();
}

// Uncompiled PC reconstruction draft. Append to input_runtime.cpp only after shared contract review.
extern "C" i32 NuPadGetDeadzoneByPortPS(i32 port) {
    if ((u32)(port - 10) <= 3) {
        return 0;
    }
    return nu_input_state->devices[port].deadzone;
}

extern "C" i32 PcInputAlternateDeadzone(i32 port) {
    if ((u32)(port - 10) > 3) {
        return 0;
    }
    return nu_input_state->devices[port].alternate_deadzone;
}

extern "C" void PcInputSetDeadzone(i32 port, i32 value) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if ((u32)(port - 10) > 3) {
        device->deadzone = value;
    } else {
        device->deadzone = 0;
    }
}

extern "C" void PcInputSetAlternateDeadzone(i32 port, i32 value) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if ((u32)(port - 10) <= 3) {
        device->alternate_deadzone = value;
    } else {
        device->alternate_deadzone = 0;
    }
}

extern "C" void NuPadSetMotorsPS(i32 port, i32 motor0, i32 motor1) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if (port >= 0 && port < 4) {
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = device->left_trigger * motor1 / 256;
        if (motor0 != 0) {
            motor0 = 1;
        }
        vibration.wRightMotorSpeed = device->right_trigger * motor0;
        XInputSetState(port, &vibration);
    }
}

extern "C" i32 PcInputPlayerOnPadPort(i32 player) {
    i32 port = NuPadGetPort(player);
    return port >= 0 && port <= 3;
}

extern "C" i32 NuPadButtonName(i32 player, i32 button, char *destination, i32, i32, i32 use_callback, i32 alternate_labels,
                               char *delimiters) {
    PC_INPUT_RUNTIME *manager = nu_input_state;
    i32 port = NuPadGetPort(player);
    return manager->ButtonName(port, button, destination, 0, 0, use_callback, alternate_labels, delimiters);
}

extern "C" void PcInputClearOtherBankConflicts(i32 bank, i32 bank_count) {
    nu_input_state->ClearOtherBankConflicts(bank, bank_count);
}

extern "C" i32 PcInput_RemapVirtualKeyboard(i32 port, i32 player) {
    if (port < 10) {
        return port;
    }
    if (port >= 14) {
        return port;
    }
    if (player != nu_input_state->captured_port) {
        ++port;
    }
    if (player >= 0) {
        port += player * 2;
    }
    return port;
}

void PcInput_EnableKeyboards(const i16 *keys, i32 count) {
    NuKeyboardActionState *state;
    i32 i;
    i32 recognized;

    state = (NuKeyboardActionState *)nu_input_state;
    recognized = 0;
    for (i = 0; i < count; ++i) {
        i32 key;
        key = keys[i];
        if ((u32)(key - 10) <= 3) {
            recognized = 1;
            state->records[key].active = 1;
        }
    }
    if (!recognized) {
        state->records[state->fallback_action != 0 ? 11 : 10].active = 1;
    }
}

extern "C" u8 PcInput_SafeGetMapping(i32 port, i32 key, i32 auxiliary_index) {
    return nu_input_state->SelectConfiguration(port, key, auxiliary_index);
}

extern "C" u8 PcInput_MappingIsGood(i32 port, i32 player_index) {
    return nu_input_state->InstallDropInConfiguration(port, player_index);
}

extern "C" i32 PcInputBindingAvailable(i32 bank, i32 bank_count, i32 kind, i32 value) {
    return nu_input_state->BindingAvailable(bank, bank_count, kind, value);
}

extern "C" void PcInputEndFrame(void) {
    nu_input_state->EndFrame();
}

extern "C" i32 PcInputResolveAuxiliaryConflicts(i32 bank, i32 bank_count) {
    return nu_input_state->ResolveAuxiliaryConflicts(bank, bank_count);
}

extern "C" i32 PcInputBindingButtonName(i32 action, i32 port, char *destination, i32 auxiliary, i32 bank, i32 a, i32 b, char *c) {
    return nu_input_state->ButtonName(action, port, destination, auxiliary, bank, a, b, c);
}

void PC_INPUT_RUNTIME::InitializeDefaultRecords() {
    ResetInputRecords();
    AddInputRecord(1, 0, -1, 0);
    AddInputRecord(2, 0, -1, 0);
    AddInputRecord(1, 1, -1, 0);
    AddInputRecord(2, 1, -1, 0);
    AddInputRecord(1, 2, -1, 0);
    AddInputRecord(2, 2, -1, 0);
    AddInputRecord(1, 3, -1, 0);
    AddInputRecord(2, 3, -1, 0);
    AddInputRecord(3, -1, 12, 0);
    AddInputRecord(3, -1, 13, 0);
    AddInputRecord(3, -1, 14, 0);
    AddInputRecord(3, -1, 15, 0);
    AddInputRecord(3, -1, 4, 0);
    AddInputRecord(3, -1, 5, 0);
    AddInputRecord(3, -1, 6, 0);
    AddInputRecord(3, -1, 7, 0);
    AddInputRecord(2, 4, 2, 0);
    AddInputRecord(2, 5, 0, 0);
    AddInputRecord(2, 6, 3, 0);
    AddInputRecord(2, 7, 1, 0);
    AddInputRecord(3, -1, 9, 0);
    AddInputRecord(3, -1, 10, 0);
    AddInputRecord(3, -1, 8, 0);
    AddInputRecord(3, -1, 11, 0);
}

i32 PC_INPUT_RUNTIME::HasConfig(i32 port, u8 mask) {
    return FindConfig(mask, port, 0) != NULL;
}

u8 PcInputWriteTextProfiles = 0;

void PC_INPUT_RUNTIME::WriteConfiguration(PC_INPUT_CONFIG *records, i32 count, const char *directory) {
    char path[1024];
    NuStrCpy(path, directory);
    NuStrCat(path, "Mappings.dat");
    FILE *file = PcOpenPlatformFile(path, "", 0, "wb");
    if (file != NULL) {
        u32 word = 0x4d415056;
        fwrite(&word, 4, 1, file);
        word = 19;
        fwrite(&word, 4, 1, file);
        fwrite(records, count * sizeof(*records), 1, file);
        fclose(file);
    }
    for (i32 i = 0; i < count; ++i) {
        if (records[i].name[0] && PcInputWriteTextProfiles) {
            WriteConfigText(directory, &records[i]);
        }
    }
}

void PC_INPUT_RUNTIME::SaveConfiguration() {
    char directory[1024];
    /* Retail passes only the folder; the two missing %s read the company and product arguments that
       PcGetUserFolder left on the stack (cdecl cleanup is deferred). */
    sprintf(directory, "%s\\%s\\%s\\", PcGetUserFolder(PcCompanyName, PcProductName));
    WriteConfiguration(primary, 64, directory);
}

void PC_INPUT_RUNTIME::BindPreferredConfig(i32 mask, i32 port) {
    PC_INPUT_CONFIG *config = FindConfig((u8)mask, port, preferred_key);
    if (!config) {
        config = FindConfig((u8)mask, port, -1);
    }
    if (config) {
        InstallConfig(config, port, config->key, 0, 0);
    }
}

void PC_INPUT_RUNTIME::InitKeyboardProfiles() {
    PC_INPUT_CONFIG *config;
    NuStrCpy(devices[10].name, "KeyboardMouse1");
    devices[10].kind = PC_INPUT_DEVICE_KEYBOARD_MOUSE1;
    devices[10].product.Data1 = 1;
    devices[10].config_dirty = 1;
    devices[10].capabilities_valid = 1;
    devices[10].button_count = 255;
    config = FindConfig(3, 10, preferred_key);
    if (!config) {
        config = FindConfig(3, 10, -1);
    }
    if (config) {
        InstallConfig(config, 10, config->key, 0, 0);
    }
    devices[11].device = nu_keyboard_owner.device;
    NuStrCpy(devices[11].name, "Keyboard1");
    devices[11].kind = PC_INPUT_DEVICE_KEYBOARD1;
    devices[11].product.Data1 = 3;
    devices[11].config_dirty = 1;
    devices[11].capabilities_valid = 1;
    devices[11].button_count = 255;
    config = FindConfig(3, 11, preferred_key);
    if (!config) {
        config = FindConfig(3, 11, -1);
    }
    if (config) {
        InstallConfig(config, 11, config->key, 0, 0);
    }
    NuStrCpy(devices[12].name, PCKeyboardMouse2Name);
    devices[12].kind = PC_INPUT_DEVICE_KEYBOARD_MOUSE2;
    devices[12].product.Data1 = 2;
    devices[12].config_dirty = 1;
    devices[12].capabilities_valid = 1;
    devices[12].button_count = 255;
    config = FindConfig(3, 12, preferred_key);
    if (!config) {
        config = FindConfig(3, 12, -1);
    }
    if (config) {
        InstallConfig(config, 12, config->key, 0, 0);
    }
    devices[13].device = nu_keyboard_owner.device;
    NuStrCpy(devices[13].name, "Keyboard2");
    devices[13].kind = PC_INPUT_DEVICE_KEYBOARD2;
    devices[13].product.Data1 = 4;
    devices[13].config_dirty = 1;
    devices[13].capabilities_valid = 1;
    devices[13].button_count = 255;
    config = FindConfig(3, 13, preferred_key);
    if (!config) {
        config = FindConfig(3, 13, -1);
    }
    if (config) {
        InstallConfig(config, 13, config->key, 0, 0);
    }
}

i32 PC_INPUT_RUNTIME::ButtonNameDefault(i32 device, i32 button, char *destination, i32 custom, i32 custom_index) {
    return ButtonName(device, button, destination, custom, custom_index, 1, 1, "()");
}

static i32 PcXInputDeviceCount; // XInput pads found by the current device enumeration
PC_DI_CAPS PcKeyboardCapabilities = {0};
i32 PcInputSelectedPort = -1;

extern "C" BOOL __stdcall InputEnumDevices(const PC_DI_INSTANCE *instance, void *unused) {
    PC_INPUT_RUNTIME *state = nu_input_state;
    PC_INPUT_DEVICE *devices = state->devices;
    if (IsXInputProduct(&instance->product)) {
        i32 port = PcXInputDeviceCount;
        PcInputSelectedPort = port;
        devices[port].product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63) {
            length = 63;
        }
        memset(devices[port].name, 0, 63);
        PCByteCopy(devices[port].name, instance->product_name, length);
        devices[port].kind = PC_INPUT_DEVICE_XINPUT;
        devices[port].capabilities_valid = 0;
        devices[port].deadzone = 64;
        PC_XINPUT_CAPABILITIES capabilities;
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&capabilities) == 0) {
            state->InitXInputCaps(&capabilities, &devices[port]);
            if (devices[port].button_count == 10 && devices[port].axis_count == 5 && devices[port].pov_count == 1) {
                devices[port].product.Data1 = 0x028e045e;
            }
        }
        PC_INPUT_CONFIG *config = state->FindConfig(7, port, state->preferred_key);
        if (!config) {
            config = state->FindConfig(7, port, -1);
        }
        if (config) {
            state->InstallConfig(config, port, config->key, 0, 0);
        }
        ++PcXInputDeviceCount;
        return TRUE;
    }
    u32 type = (u8)instance->device_type;
    if (type != 19) {
        if (type - 20 > 1) {
            return TRUE;
        }
        NuKeyboardDevice *device;
        if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, instance->instance, &device, NULL) < 0) {
            return TRUE;
        }
        i32 port;
        for (port = 4; port < 10; ++port) {
            if (devices[port].kind == PC_INPUT_DEVICE_NONE) {
                break;
            }
        }
        if (port >= 10) {
            return TRUE;
        }
        PC_INPUT_DEVICE *record = &devices[port];
        if (PcInputSelectedPort == -1) {
            PcInputSelectedPort = port;
        }
        PC_DI_CAPS caps;
        caps.size = sizeof(caps);
        HRESULT result = device->methods->get_caps(device, &caps);
        record->button_count = (i8)caps.buttons;
        record->capabilities_valid = result == 0;
        record->pov_count = (u8)caps.povs;
        record->device = device;
        record->axis_count = (u8)caps.axes;
        record->deadzone = 32;
        record->kind = PC_INPUT_DEVICE_DIRECTINPUT;
        record->config_dirty = 1;
        record->product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63) {
            length = 63;
        }
        memset(record->name, 0, 63);
        PCByteCopy(record->name, instance->product_name, length);
        if (NuStrIStr((char *)instance->product_name, "Saitek") != NULL &&
            (NuStrIStr((char *)instance->product_name, "P2600") != NULL || NuStrIStr((char *)instance->product_name, "P2900") != NULL)) {
            record->button_count -= 2;
        }
        device->methods->set_format(device, &PcJoystickDataFormat);
        device->methods->set_cooperative_level(device, PCInputInstance, 9);
        device->methods->acquire(device);
        ConfigureJoystickRanges(device);
        state->BindPreferredConfig(7, port);
        ++nu_keyboard_owner.device_count;
        return TRUE;
    }
    if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, PcSystemKeyboardId, &nu_keyboard_owner.device, NULL) !=
        0) {
        return TRUE;
    }
    PcKeyboardCapabilities.size = sizeof(PcKeyboardCapabilities);
    nu_keyboard_owner.device->methods->set_cooperative_level(nu_keyboard_owner.device, PCInputInstance, 22);
    nu_keyboard_owner.device->methods->set_format(nu_keyboard_owner.device, &PcKeyboardDataFormat);
    nu_keyboard_owner.device->methods->acquire(nu_keyboard_owner.device);
    nu_keyboard_owner.device->methods->get_caps(nu_keyboard_owner.device, &PcKeyboardCapabilities);
    state->InitKeyboardProfiles();
    devices[10].device = nu_keyboard_owner.device;
    devices[10].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[12].device = nu_keyboard_owner.device;
    devices[12].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[11].device = nu_keyboard_owner.device;
    devices[11].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[13].device = nu_keyboard_owner.device;
    devices[13].button_count = (i16)PcKeyboardCapabilities.buttons;
    nu_keyboard_owner.key_count = 256;
    return TRUE;
}

void NuKeyboardOwner::RescanDevices() {
    PC_INPUT_DEVICE *devices = nu_input_state->devices;
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    device_count = 0;
    PcXInputDeviceCount = 0;
    for (i32 port = 0; port < 4; ++port) {
        PC_XINPUT_CAPABILITIES caps;
        PC_INPUT_DEVICE *record = &devices[port];
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0) {
            record->capabilities_valid = 1;
            runtime->InitXInputCaps(&caps, record);
            record->config_dirty = 1;
            ++device_count;
        } else {
            record->kind = PC_INPUT_DEVICE_NONE;
            record->capabilities_valid = 0;
            record->device = NULL;
        }
    }
    for (i32 port = 4; port < 10; ++port) {
        PC_INPUT_DEVICE *record = &devices[port];
        if (record->device != NULL) {
            record->device->methods->unacquire(record->device);
            if (record->device != NULL) {
                record->device->methods->release(record->device);
                record->device = NULL;
            }
        }
        record->kind = PC_INPUT_DEVICE_NONE;
        record->capabilities_valid = 0;
    }
    if (nu_keyboard_owner.device != NULL) {
        nu_keyboard_owner.device->methods->unacquire(nu_keyboard_owner.device);
        if (nu_keyboard_owner.device != NULL) {
            nu_keyboard_owner.device->methods->release(nu_keyboard_owner.device);
            nu_keyboard_owner.device = NULL;
        }
        for (i32 port = 10; port < 14; ++port) {
            devices[port].device = NULL;
            devices[port].kind = PC_INPUT_DEVICE_NONE;
            devices[port].capabilities_valid = 0;
        }
    }
    device_count = 0;
    PcXInputDeviceCount = 0;
    factory->methods->enum_devices(factory, 3, InputEnumDevices, NULL, 1);
    factory->methods->enum_devices(factory, 2, InputEnumDevices, NULL, 1);
    factory->methods->enum_devices(factory, 4, InputEnumDevices, NULL, 1);
}

extern "C" void PcInputSavePlayerConfig(i32 player) {
    PC_INPUT_RUNTIME *state = nu_input_state;
    PC_INPUT_CONFIG config;
    i32 port = NuPadGetPort(player);
    state->ExportConfig(port, &config, 0, 0, 0);
    state->PromoteConfig(state->primary, 64, &config);
    state->SaveConfiguration();
}

extern "C" {
    char TextInputActionLabelBuffer[128] = {0};
}

extern "C" char *TextInputActionLabel(i32 device, i32 action, i32 custom, i32 custom_index) {
    nu_input_state->ButtonName(device, action, TextInputActionLabelBuffer, custom, custom_index, 1, 1, "()");
    return TextInputActionLabelBuffer;
}

extern "C" i32 PcInput_GetCtrlInputString(i32 device, i32 button, char *destination) {
    PC_INPUT_RUNTIME *manager = nu_input_state;
    if (device < 0) {
        device = 0;
    }
    i32 linked_device = NuPadGetPort(device);
    if (button >= 0) {
        return manager->ButtonName(linked_device, button, destination, 0, 0, 1, 1, "()");
    }
    *destination = 0;
    return 0;
}

extern "C" void PcInputLoadPlayerConfigs(i32 player) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    PC_INPUT_CONFIG config;
    for (i32 port = 0; port < 14; ++port) {
        i32 owner = NuPadGetPlayer(port);
        if (runtime->devices[port].kind != PC_INPUT_DEVICE_NONE && (owner == -1 || owner == player)) {
            PC_INPUT_CONFIG *found = runtime->FindConfig(3, port, player);
            if (found == NULL) {
                found = runtime->FindConfig(3, port, -1);
                if (found == NULL) {
                    continue;
                }
                runtime->InstallConfig(found, port, player, 1, player);
                runtime->ExportConfig(port, &config, 1, player, player);
                runtime->PromoteConfig(runtime->primary, 64, &config);
                runtime->SaveConfiguration();
                runtime->InstallConfig(&config, port, player, 0, -1);
            }
            NuPadMapPlayerToPort(player, port);
            NuPadSetValid(port, 1);
            return;
        }
    }
}

extern "C" void PcInputSaveConfiguration() {
    nu_input_state->SaveConfiguration();
}

PC_INPUT_RUNTIME::PC_INPUT_RUNTIME() {
    memset(devices, 0, sizeof(devices));
    memset(primary, 0, sizeof(primary));
    memset(secondary, 0, sizeof(secondary));
    memset(&mouse, 0, offsetof(PC_INPUT_MOUSE_RUNTIME, clear_end));
    keyboard_state = NULL;
    memset(&raw_sample, 0, sizeof(raw_sample));
    memset(auxiliary_bindings, 0, sizeof(auxiliary_bindings));
    for (i32 i = 0; i < 32; ++i) {
        commands[i].active = 0;
        commands[i].axis = -1;
        commands[i].button = -1;
        commands[i].value = 0;
    }
    shutdown_state = 0;
    InitConfiguration();
    pending_binding = -1;
    first_mouse = 1;
    action_mask = 0;
    completed_mask = 0;
    for (i32 i = 0; i < 14; ++i) {
        mouse.port_activity[i] = 0;
        captures[i].kind[0] = 0;
        captures[i].kind[1] = 0;
        captures[i].kind[2] = 0;
        staged_capture[i].kind[0] = 0;
        staged_capture[i].kind[1] = 0;
        staged_capture[i].kind[2] = 0;
    }
    captured_port = -1;
    keyboard_ui_override = 0;
    preferred_key = 0;
    mouse.custom_bindings_active = 0;
    suppress_key_1 = 0;
    blocked_keys[0] = '\xff';
    for (i32 i = 0; i < 14; ++i) {
        for (i32 j = 0; j < 32; ++j) {
            devices[i].reset_pairs[j].first = 0;
            devices[i].reset_pairs[j].second = 0;
        }
    }
    ramp_seconds = PCKeyboardDefaultValue;
    axis_threshold = 0x1fff;
    motion_threshold = 32;
    read_mask = 0;
    keyboard_polled = 0;
    mouse_polled = 0;
    mouse_ui_override = 0;
    InitializeDefaultRecords();
    if (PcInputStateInitializedCallback) {
        PcInputStateInitializedCallback(this);
    }
    InitKeyboardProfiles();
    switch (NuLanguageGet()) {
        case NULANGUAGE_JAPANESE:
            TextInputKeyNames = TextInputKeyNamesJapanese;
            TextInputDefaultLabels = &TextInputLabelsJapanese;
            TextInputAlternateLabels = &TextInputLabelsJapanese;
            break;
        case NULANGUAGE_ENGLISH:
            TextInputKeyNames = TextInputKeyNameTable;
            TextInputDefaultLabels = &TextInputLabelsEnglish;
            TextInputAlternateLabels = &TextInputLabelsEnglish;
            break;
        case NULANGUAGE_FRENCH:
            TextInputKeyNames = TextInputKeyNamesFrench;
            TextInputDefaultLabels = &TextInputLabelsFrench;
            TextInputAlternateLabels = &TextInputLabelsFrench;
            break;
        case NULANGUAGE_SPANISH:
            TextInputKeyNames = TextInputKeyNamesSpanish;
            TextInputDefaultLabels = &TextInputLabelsSpanish;
            TextInputAlternateLabels = &TextInputLabelsSpanish;
            break;
        case NULANGUAGE_GERMAN:
            TextInputKeyNames = TextInputKeyNamesGerman;
            TextInputDefaultLabels = &TextInputLabelsGerman;
            TextInputAlternateLabels = &TextInputLabelsGerman;
            break;
        case NULANGUAGE_ITALIAN:
            TextInputKeyNames = TextInputKeyNamesItalian;
            TextInputDefaultLabels = &TextInputLabelsItalian;
            TextInputAlternateLabels = &TextInputLabelsItalian;
            break;
        case NULANGUAGE_DUTCH:
            TextInputKeyNames = TextInputKeyNamesDutch;
            TextInputDefaultLabels = &TextInputLabelsDutch;
            TextInputAlternateLabels = &TextInputLabelsDutch;
            break;
        case NULANGUAGE_PORTUGUESE:
            TextInputKeyNames = TextInputKeyNamesPortuguese;
            TextInputDefaultLabels = &TextInputLabelsPortuguese;
            TextInputAlternateLabels = &TextInputLabelsPortuguese;
            break;
        case NULANGUAGE_DANISH:
            TextInputKeyNames = TextInputKeyNamesDanish;
            TextInputDefaultLabels = &TextInputLabelsDanish;
            TextInputAlternateLabels = &TextInputLabelsDanish;
            break;
        case NULANGUAGE_CZECH:
            TextInputKeyNames = TextInputKeyNamesCzech;
            TextInputDefaultLabels = &TextInputLabelsCzech;
            TextInputAlternateLabels = &TextInputLabelsCzech;
            break;
        case NULANGUAGE_POLISH:
            TextInputKeyNames = TextInputKeyNamesPolish;
            TextInputDefaultLabels = &TextInputLabelsPolish;
            TextInputAlternateLabels = &TextInputLabelsPolish;
            break;
        case NULANGUAGE_RUSSIAN:
            TextInputKeyNames = TextInputKeyNamesRussian;
            TextInputDefaultLabels = &TextInputLabelsRussian;
            TextInputAlternateLabels = &TextInputLabelsRussian;
            break;
        case NULANGUAGE_SWEDISH:
            TextInputKeyNames = TextInputKeyNamesSwedish;
            TextInputDefaultLabels = &TextInputLabelsSwedish;
            TextInputAlternateLabels = &TextInputLabelsSwedish;
            break;
        case NULANGUAGE_NORWEGIAN:
            TextInputKeyNames = TextInputKeyNamesNorwegian;
            TextInputDefaultLabels = &TextInputLabelsNorwegian;
            TextInputAlternateLabels = &TextInputLabelsNorwegian;
            break;
        case NULANGUAGE_BRAZILIAN_PORTUGUESE:
            TextInputKeyNames = TextInputKeyNamesBrazilianPortuguese;
            TextInputDefaultLabels = &TextInputLabelsPortuguese;
            TextInputAlternateLabels = &TextInputLabelsPortuguese;
            break;
        case NULANGUAGE_FINNISH:
            TextInputKeyNames = TextInputKeyNamesFinnish;
            TextInputDefaultLabels = &TextInputLabelsFinnish;
            TextInputAlternateLabels = &TextInputLabelsFinnish;
            break;
        case NULANGUAGE_US_ENGLISH:
            TextInputKeyNames = TextInputKeyNamesDefault;
            TextInputDefaultLabels = &TextInputLabelsEnglish;
            TextInputAlternateLabels = &TextInputLabelsEnglish;
            break;
        default:
            TextInputKeyNames = TextInputKeyNamesDefault;
            TextInputDefaultLabels = &TextInputLabelsEnglish;
            TextInputAlternateLabels = &TextInputLabelsEnglish;
            break;
    }
}

void PC_INPUT_RUNTIME::AutoConfigurePads(i32 count) {
    i32 device = 0;
    for (i32 pad = 0; pad < count; ++pad) {
        i32 linked = NuPadGetPort(pad);
        for (; device < 14; ++device) {
            if (devices[device].kind != PC_INPUT_DEVICE_NONE && (device < 10 || device > 13 || devices[device].action_keys_active)) {
                if (device != linked) {
                    NuPadMapPlayerToPort(pad, device);
                    NuPadSetValid(device, 1);
                    PC_INPUT_CONFIG *config = FindConfig(7, device, preferred_key);
                    if (!config) {
                        config = FindConfig(7, device, -1);
                    }
                    if (config) {
                        InstallConfig(config, device, config->key, 0, 0);
                    }
                }
                ++device;
                break;
            }
        }
    }
}

i32 PC_INPUT_RUNTIME::ReadSample(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2, u32 *buttons, u8 *unknown,
                                 i32 *flags) {
    i32 valid = 0;
    if ((u32)(port - 10) <= 3 && !devices[port].action_keys_active) {
        return 0;
    }
    if (!keyboard_polled) {
        if (nu_keyboard_owner.rescan_count > 0) {
            nu_keyboard_owner.rescan_timer = (f32)((f64)nu_keyboard_owner.rescan_timer - NuTimeGetFrameTime());
            if (nu_keyboard_owner.rescan_timer <= 0.0f) {
                nu_keyboard_owner.rescan_timer += nu_keyboard_owner.rescan_interval;
                --nu_keyboard_owner.rescan_count;
                nu_keyboard_owner.RescanDevices();
            }
        }
        if (!keyboard_polled) {
            keyboard_polled = 1;
            keyboard_state = nu_keyboard_owner.PollKeyboard();
        }
    }
    UpdateMouse();
    u32 mask = 1u << port;
    PC_INPUT_SAMPLE *sample = &devices[port].sample;
    if (!(read_mask & mask)) {
        read_mask |= mask;
        valid = SampleDevice(port, &raw_sample);
        if ((action_mask & mask) && valid) {
            CaptureBinding(port);
        }
        TranslateBindings(port, sample, valid, &raw_sample, &mouse, &keyboard_state);
        if ((port == 10 || port == 12) && !keyboard_ui_override) {
            ApplyMouseAxis(sample);
        }
        if (keyboard_ui_override && valid) {
            if ((u32)(port - 10) <= 3) {
                sample->buttons = 0;
            }
            i32 linked = NuPadGetPort(TextInputLastDeviceIndex);
            if (linked == port || (linked == -1 && (u32)(port - 10) <= 3)) {
                sample->buttons = CombineUiButtons(port, sample->buttons);
            }
        }
    }
    *buttons = sample->buttons;
    if (!(nu_keyboard_device_flags & 4) && SoundVoiceEnabledGate) {
        *lx = sample->analog[0];
        *ly = sample->analog[1];
        *rx = sample->analog[2];
        *ry = sample->analog[3];
        *l1 = sample->analog[4];
        *l2 = sample->analog[5];
        *r1 = sample->analog[6];
        *r2 = sample->analog[7];
    } else {
        *lx = *ly = *rx = *ry = 128;
        *l1 = *l2 = *r1 = *r2 = 0;
        *buttons = 0;
    }
    *unknown = 0;
    *flags = 0;
    return valid;
}

void NuKeyboardOwner::Setup(HWND window, HINSTANCE instance) {
    DirectInput8Create(instance, 0x800, PcDirectInput8AId, (void **)&factory, NULL);
    factory->methods->create_device(factory, PcSystemMouseId, &mouse, NULL);
    PC_DI_CAPS capabilities;
    capabilities.size = sizeof(capabilities);
    mouse->methods->get_caps(mouse, &capabilities);
    if (capabilities.flags & 1) {
        mouse_axes = capabilities.axes;
        mouse_buttons = capabilities.buttons;
        mouse->methods->set_cooperative_level(mouse, window, 5);
        mouse->methods->set_format(mouse, &PcMouseDataFormat);
        PC_DI_PROPERTY property;
        property.header.header_size = 16;
        property.header.size = 20;
        property.header.how = 0;
        property.header.object = 0;
        property.data = 0;
        mouse->methods->set_property(mouse, *(const GUID *)2, &property.header);
        property.header.size = 20;
        property.header.header_size = 16;
        property.header.object = 0;
        property.header.how = 0;
        property.data = 16;
        mouse->methods->set_property(mouse, *(const GUID *)1, &property.header);
        mouse->methods->acquire(mouse);
    } else {
        mouse_axes = 0;
        mouse_buttons = 0;
    }
    nu_input_state = new PC_INPUT_RUNTIME;
    PcInput_EnableKeyboards(NULL, 0);
    device_count = 0;
    PcXInputDeviceCount = 0;
    factory->methods->enum_devices(factory, 3, InputEnumDevices, NULL, 1);
    factory->methods->enum_devices(factory, 2, InputEnumDevices, NULL, 1);
    factory->methods->enum_devices(factory, 4, InputEnumDevices, NULL, 1);
    nu_keyboard_owner.rescan_interval = 0.2f;
    nu_keyboard_owner.rescan_timer = 0.2f;
    PcInputSelectedPort = -1;
    nu_keyboard_owner.rescan_count = 3;
}

extern "C" i32 NuPadReadOutputs(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2, u32 *buttons, u8 *unknown,
                                i32 *flags) {
    if (nu_input_state != NULL) {
        return nu_input_state->ReadSample(port, lx, ly, rx, ry, l1, l2, r1, r2, buttons, unknown, flags);
    }
    return 0;
}

extern "C" void NuPadAutoConfigure(i32 count) {
    nu_input_state->AutoConfigurePads(count);
}
