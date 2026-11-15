/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufile/nufile.h"
#endif

struct FRAME_PAD_PC;

typedef struct NuKeyboardState_PC {           // size 0x68b4
    u8 unknown_0000_to_4e10[0x4e10];          // 0x0000
    u8 keyboard_polled;                       // 0x4e10
    u8 unknown_4e11_to_5d29[0x5d29 - 0x4e11]; // 0x4e11
    u8 unknown_5d29;                          // 0x5d29
    u8 unknown_5d2a_to_6314[0x6314 - 0x5d2a]; // 0x5d2a
    f32 mouse_delta[3];                       // 0x6314
    f32 mouse_smoothed[3];                    // 0x6320
    f32 cursor_x;                             // 0x632c
    f32 cursor_y;                             // 0x6330
    u8 unknown_6334_to_68b0[0x68b0 - 0x6334]; // 0x6334
    u8 *keyboard_state;                       // 0x68b0
} NuKeyboardState_PC;

typedef struct NuKeyboardDevice_PC NuKeyboardDevice_PC;
typedef struct PC_DI_FACTORY PC_DI_FACTORY;

typedef struct NuKeyboardOwner_PC { // size 0x44
    f32 unknown_0000;               // 0x00
    i32 unknown_0004;               // 0x04
    f32 unknown_0008;               // 0x08
    i32 device_count;               // 0x0c
    u8 unknown_0010_to_0020[0x10];  // 0x10
    NuKeyboardDevice_PC *mouse;     // 0x20
    NuKeyboardDevice_PC *device;    // 0x24
    i32 poll_status;                // 0x28
    i32 key_count;                  // 0x2c
    PC_DI_FACTORY *factory;         // 0x30
    i32 mouse_axes;                 // 0x34
    i32 mouse_buttons;              // 0x38
    i32 mouse_cooperative;          // 0x3c
    u8 mouse_enabled;               // 0x40
    u8 unknown_41[3];               // 0x41
#ifdef __cplusplus
    NuKeyboardOwner_PC();
    ~NuKeyboardOwner_PC();
    void SetMouseEnabledPc(u8 enabled);
    const struct PC_DI_MOUSE_STATE_PC *PollMousePc();
    i32 SetMouseCooperativePc(i32 value);
    void RescanDevicesPc();
    void Setup(HWND window, HINSTANCE instance);
    void ScheduleRescan(f32 interval, i32 repetitions);
    void Shutdown();
    i32 fun_006d5720(i32 *axes, i32 *buttons);
    i32 fun_006d5970();
    i32 fun_006d5980();
    u8 *PollKeyboardPc();
#endif
} NuKeyboardOwner_PC;

typedef struct GRAPHICS_CAPS {       // size 0x130
    i32 device_type;                 // 0x000
    u32 adapter_ordinal;             // 0x004
    u32 unknown_008[3];              // 0x008
    u32 caps2;                       // 0x014
    u32 unknown_018[1];              // 0x018
    u32 device_caps;                 // 0x01c
    u32 unknown_020[7];              // 0x020
    u32 texture_caps;                // 0x03c
    u32 unknown_040[0x16];           // 0x040
    u32 sampler_count;               // 0x098
    u32 unknown_09c[0xa];            // 0x09c
    u32 vertex_shader_version;       // 0x0c4
    u32 unknown_0c8[1];              // 0x0c8
    u32 pixel_shader_version;        // 0x0cc
    u32 unknown_0d0[8];              // 0x0d0
    u32 simultaneous_render_targets; // 0x0f0
    u32 unknown_0f4[0xf];            // 0x0f4
} GRAPHICS_CAPS;

#pragma pack(push, 4)

typedef struct GRAPHICS_ADAPTER_IDENTIFIER { // size 0x44c
    char driver[512];                        // 0x000
    char description[512];                   // 0x200
    char device_name[32];                    // 0x400
    LARGE_INTEGER driver_version;            // 0x420
    u32 vendor;                              // 0x428
    u32 device_id;                           // 0x42c
    u32 subsystem_id;                        // 0x430
    u32 revision;                            // 0x434
    GUID device_identifier;                  // 0x438
    u32 whql_level;                          // 0x448
} GRAPHICS_ADAPTER_IDENTIFIER;

#pragma pack(pop)

typedef struct TEXT_INPUT_LABELS_PC TEXT_INPUT_LABELS_PC;

struct TEXT_INPUT_LABELS_PC { // size 0xc0

    union { // size 0x94
        char *initializer_names[37];
        char *indexed_names[34];

        struct {                   // size 0x94
            char *fixed_names[4];  // 0x00
            char *button_prefix;   // 0x10
            char *axis_negative;   // 0x14
            char *axis_positive;   // 0x18
            char *axis_format;     // 0x1c
            char *axis_names[6];   // 0x20
            u8 unknown_0038[0x5c]; // 0x38
        };
    }; // 0x00

    char *direction_positive[3]; // 0x94
    char *direction_negative[3]; // 0xa0
    char *hat_names[3];          // 0xac
    char *hat_format;            // 0xb8
    char *unknown;               // 0xbc
};

#ifdef __cplusplus
extern "C" {
#endif
    i32 PcInput_GetCtrlStringPlayer(void);
    i32 PcInput_GetCtrlInputString(i32 device, i32 button, char *destination);
    extern char *TextInputKeyNameTablePc[238];
    extern char **TextInputKeyNamesPc;
    extern u8 TextInputKeyboardOverridePc[24];
    extern i8 TextInputAxisLabelIndexPc[16];
    extern i8 TextInputButtonLabelIndexPc[11];
    extern TEXT_INPUT_LABELS_PC *TextInputAlternateLabelsPc;
    extern TEXT_INPUT_LABELS_PC *TextInputDefaultLabelsPc;
    extern void (*TextInputNameCallbackPc)(i32, char *);
    extern i8 TextInputLastDeviceIndexPc;

    void TextInputSetKeyboardOverridePc(i32);
    void PcInput_SetCtrlStringPlayer(i32);
    i32 PcInput_PortIsKeyboard(i32);
    extern i32 (*PcInputCommandFilterPc)(i32, i32);
    void PcInput_SetIsUsedCallBack(i32 (*filter)(i32, i32));
    i32 PcInput_ScanForAnyInput(u32 excluded, u8 kinds, i32 *kind, i32 *value, i32 skip_active);
    i32 PcInput_RemapVirtualKeyboard(i32 port, i32 player);
    u8 PcInput_SafeGetMapping(i32 port, i32 key, i32 auxiliary_index);
    char **PcInput_GetKeyNameTable(void);
    i32 PcInput_PortIsUC(i32);
    void MenuSetPlatformState(i32 state);
    extern PC_NATIVE_DEBUG_STATE PCNativeWarningDebugState;
    extern NUFILE_DEVICE devices[16];
    extern NuKeyboardState_PC *nu_input_state;
    extern NuKeyboardOwner_PC nu_keyboard_owner;
    extern u8 nu_keyboard_pages[0x200];
    extern u8 nu_keyboard_page_index;
    extern i32 nu_keyboard_device_flags;
    extern i32 nu_keyboard_key_index;
    extern i32 nu_keyboard_repeat_countdown;
    void PcInput_EnableKeyboards(const i16 *keys, i32 count);
    extern i32 PCDeviceHardwareVertexProcessingPC;
    void NuPadAutoConfigurePc(i32 count);
    void PcInput_DisableAllKeyboards(void);
    void PCGraphicsFatalDiagnostic(PC_NATIVE_DEBUG_STATE *state, const char *format, ...);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct PC_DI_CAPS {               // size 0x2c
    DWORD size;                   // 0x00
    DWORD flags;                  // 0x04
    DWORD device_type;            // 0x08
    DWORD axes;                   // 0x0c
    DWORD buttons;                // 0x10
    DWORD povs;                   // 0x14
    DWORD ff_sample_period;       // 0x18
    DWORD ff_min_time_resolution; // 0x1c
    DWORD firmware_revision;      // 0x20
    DWORD hardware_revision;      // 0x24
    DWORD ff_driver_version;      // 0x28
};

struct PC_DI_PROPERTY { // size 0x14
    DWORD size;         // 0x00
    DWORD header_size;  // 0x04
    DWORD object;       // 0x08
    DWORD how;          // 0x0c
    DWORD data;         // 0x10
};

struct PC_DI_OBJECT_FORMAT { // size 0x10
    const GUID *guid;        // 0x0
    DWORD offset;            // 0x4
    DWORD type;              // 0x8
    DWORD flags;             // 0xc
};

struct PC_DI_DATA_FORMAT {              // size 0x18
    DWORD size;                         // 0x00
    DWORD object_size;                  // 0x04
    DWORD flags;                        // 0x08
    DWORD data_size;                    // 0x0c
    DWORD object_count;                 // 0x10
    const PC_DI_OBJECT_FORMAT *objects; // 0x14
};

struct PC_DI_INSTANCE {      // size 0x244
    DWORD size;              // 0x000
    GUID instance;           // 0x004
    GUID product;            // 0x014
    DWORD device_type;       // 0x024
    char instance_name[260]; // 0x028
    char product_name[260];  // 0x12c
    u8 unknown_230[0x14];    // 0x230
};

struct PC_DI_DEVICE_METHODS_PC {                                                              // size 0x68
    void *unknown_00;                                                                         // 0x00
    void *unknown_04;                                                                         // 0x04
    ULONG(__stdcall *release)(NuKeyboardDevice_PC *);                                         // 0x08
    HRESULT(__stdcall *get_caps)(NuKeyboardDevice_PC *, PC_DI_CAPS *);                        // 0x0c
    void *unknown_10;                                                                         // 0x10
    void *unknown_14;                                                                         // 0x14
    HRESULT(__stdcall *set_property)(NuKeyboardDevice_PC *, REFGUID, const PC_DI_PROPERTY *); // 0x18
    HRESULT(__stdcall *acquire)(NuKeyboardDevice_PC *);                                       // 0x1c
    HRESULT(__stdcall *unacquire)(NuKeyboardDevice_PC *);                                     // 0x20
    HRESULT(__stdcall *get_state)(NuKeyboardDevice_PC *, DWORD, void *);                      // 0x24
    void *unknown_28;                                                                         // 0x28
    HRESULT(__stdcall *set_format)(NuKeyboardDevice_PC *, const PC_DI_DATA_FORMAT *);         // 0x2c
    void *unknown_30;                                                                         // 0x30
    HRESULT(__stdcall *set_cooperative_level)(NuKeyboardDevice_PC *, HWND, DWORD);            // 0x34
    void *unknown_38_60[11];                                                                  // 0x38
    HRESULT(__stdcall *poll)(NuKeyboardDevice_PC *);                                          // 0x64
};

struct NuKeyboardDevice_PC {
    PC_DI_DEVICE_METHODS_PC *methods;
};

struct PC_DI_FACTORY_METHODS {                                                                       // size 0x14
    void *unknown_00;                                                                                // 0x00
    void *unknown_04;                                                                                // 0x04
    ULONG(__stdcall *release)(PC_DI_FACTORY *);                                                      // 0x08
    HRESULT(__stdcall *create_device)(PC_DI_FACTORY *, REFGUID, NuKeyboardDevice_PC **, IUnknown *); // 0x0c
    HRESULT(__stdcall *enum_devices)                                                                 // 0x10
    (PC_DI_FACTORY *, DWORD, BOOL(__stdcall *)(const PC_DI_INSTANCE *, void *), void *, DWORD);
};

struct PC_DI_FACTORY {
    PC_DI_FACTORY_METHODS *methods;
};

extern const GUID PcDirectInput8AId;
extern const GUID PcSystemMouseId;
extern const GUID PcSystemKeyboardId;
extern const PC_DI_DATA_FORMAT PcMouseDataFormat;
extern const PC_DI_DATA_FORMAT PcKeyboardDataFormat;
extern const PC_DI_DATA_FORMAT PcJoystickDataFormat;
extern i32 PcInputSelectedPort;
extern "C" BOOL __stdcall InputEnumDevicesPc(const PC_DI_INSTANCE *, void *);

struct PC_INPUT_SAMPLE_PC { // size 0xc
    u8 analog[8];           // 0x0
    u32 buttons;            // 0x8
};

struct PC_INPUT_RAW_SAMPLE_PC { // size 0xc4
    i16 axes[32];               // 0x00
    u8 buttons[132];            // 0x40
};

struct PC_DI_MOUSE_STATE_PC { // size 0x14
    i32 axes[3];              // 0x00
    u8 buttons[8];            // 0x0c
};

struct PC_INPUT_CAPTURE_PC { // size 0xa
    u8 kind[3];              // 0x0
    u8 unknown_03;           // 0x3
    i16 value[3];            // 0x4
};

struct PC_INPUT_MOUSE_RUNTIME_PC { // size 0x5c
    i16 current[3];                // 0x00
    i16 previous[3];               // 0x06
    f32 accumulated[3];            // 0x0c
    f32 delta[3];                  // 0x18
    u8 unknown_24[12];             // 0x24
    f32 position[2];               // 0x30
    u8 buttons[16];                // 0x38
    u8 unknown_48;                 // 0x48
    u8 port_activity[14];          // 0x49
    u8 unknown_57;                 // 0x57
    i32 unknown_58;                // 0x58
};

#pragma pack(push, 2)

struct PC_INPUT_CONFIG_PC { // size 0x9c
    char name[64];          // 0x00
    u32 product_id;         // 0x40

    union { // size 0x16
        u8 unknown_44[0x16];

        struct {              // size 0x16
            u32 guid_data2;   // 0x44
            u32 guid_data3;   // 0x48
            u8 guid_data4[8]; // 0x4c
            i16 button_count; // 0x54
            i16 pov_count;    // 0x56
            i16 axis_count;   // 0x58
        };
    }; // 0x44

    u8 deadzone;           // 0x5a
    u8 unknown_5b;         // 0x5b
    u8 alternate_deadzone; // 0x5c
    u8 unknown_5d;         // 0x5d
    i16 key;               // 0x5e
    u8 unknown_60[4];      // 0x60

    union { // size 0x18

        struct {                   // size 0x18
            u8 axes[4][3];         // 0x64
            u8 unknown_axes[4][3]; // 0x70
        };

        u8 all_axes[8][3];
    }; // 0x64

    u8 buttons[16][2]; // 0x7c
};

#pragma pack(pop)

struct PC_INPUT_COMMAND_PC { // size 0x6
    i8 active;               // 0x0
    i8 axis;                 // 0x1
    i8 button;               // 0x2
    u8 unknown_03;           // 0x3
    u16 unknown_04;          // 0x4
};

struct PC_INPUT_BINDING_PC { // size 0x4
    u8 kind;                 // 0x0
    u8 unknown_01;           // 0x1
    i16 value;               // 0x2
};

struct PC_INPUT_DEVICE_PC {      // size 0x17c
    i32 kind;                    // 0x000
    NuKeyboardDevice_PC *device; // 0x004
    GUID product;                // 0x008
    i16 button_count;            // 0x018
    i8 axis_count;               // 0x01a
    i8 pov_count;                // 0x01b
    u8 deadzone;                 // 0x01c
    u8 alternate_deadzone;       // 0x01d
    u8 unknown_1e[2];            // 0x01e
    i32 left_trigger;            // 0x020
    i32 right_trigger;           // 0x024

    union { // size 0x2
        u8 unknown_28[2];

        struct {                   // size 0x2
            u8 action_keys_active; // 0x028
            u8 unknown_29;         // 0x029
        };
    }; // 0x028

    u8 config_dirty;                  // 0x02a
    u8 config_installed;              // 0x02b
    u8 config_present;                // 0x02c
    u8 capabilities_valid;            // 0x02d
    char name[64];                    // 0x02e
    PC_INPUT_BINDING_PC bindings[32]; // 0x06e

    struct {           // size 0x4
        u16 first;     // 0x0ee
        u16 second;    // 0x0f0
    } reset_pairs[32]; // 0x0ee

    u8 unknown_16e[2];         // 0x16e
    PC_INPUT_SAMPLE_PC sample; // 0x170
};

struct PC_XINPUT_GAMEPAD_PC { // size 0xc
    u16 buttons;              // 0x0
    u8 left_trigger;          // 0x2
    u8 right_trigger;         // 0x3
    i16 thumb_lx;             // 0x4
    i16 thumb_ly;             // 0x6
    i16 thumb_rx;             // 0x8
    i16 thumb_ry;             // 0xa
};

struct PC_XINPUT_STATE_PC {       // size 0x10
    u32 packet_number;            // 0x0
    PC_XINPUT_GAMEPAD_PC gamepad; // 0x4
};

struct PC_XINPUT_CAPABILITIES_PC { // size 0x14
    u8 type;                       // 0x00
    u8 subtype;                    // 0x01
    u16 flags;                     // 0x02
    PC_XINPUT_GAMEPAD_PC gamepad;  // 0x04
    u16 motor_left;                // 0x10
    u16 motor_right;               // 0x12
};

struct PC_INPUT_RUNTIME_PC {          // size 0x68b4
    PC_INPUT_CONFIG_PC primary[64];   // 0x0000
    PC_INPUT_CONFIG_PC secondary[64]; // 0x2700

    union { // size 0x34
        u8 unknown_4e00[0x34];

        struct {                // size 0x34
            i32 field_4e00;     // 0x4e00
            u32 read_mask;      // 0x4e04
            u32 action_mask;    // 0x4e08
            u32 completed_mask; // 0x4e0c
            u8 keyboard_polled; // 0x4e10
            u8 mouse_polled;    // 0x4e11
            u8 suppress_key_1;  // 0x4e12

            union { // size 0x21

                struct {                   // size 0x21
                    u8 unknown_4e13;       // 0x4e13
                    u8 unknown_4e14[0x20]; // 0x4e14
                };

                char blocked_keys[33];
            }; // 0x4e13
        };
    }; // 0x4e00

    PC_INPUT_DEVICE_PC devices[14]; // 0x4e34

    union { // size 0x5c
        u8 unknown_62fc[0x5c];
        PC_INPUT_MOUSE_RUNTIME_PC mouse;
    }; // 0x62fc

    PC_INPUT_BINDING_PC auxiliary_bindings[4][32]; // 0x6358
    i32 auxiliary_ports[4];                        // 0x6558

    union { // size 0x9c
        u8 unknown_6568[0x9c];

        struct {                                    // size 0x9c
            i32 axis_threshold;                     // 0x6568
            i32 motion_threshold;                   // 0x656c
            f32 ramp_seconds;                       // 0x6570
            u8 capture_available;                   // 0x6574
            u8 unknown_6575;                        // 0x6575
            PC_INPUT_CAPTURE_PC staged_capture[14]; // 0x6576
            u8 unknown_6602[2];                     // 0x6602
        };
    }; // 0x6568

    i32 preferred_key; // 0x6604

    union { // size 0x1dc
        u8 unknown_6608[0x1dc];

        struct {                               // size 0x1dc
            i32 captured_port;                 // 0x6608
            PC_INPUT_CAPTURE_PC captures[14];  // 0x660c
            u8 unknown_6698[0x82];             // 0x6698
            u8 first_mouse;                    // 0x671a
            u8 unknown_671b;                   // 0x671b
            PC_INPUT_RAW_SAMPLE_PC raw_sample; // 0x671c
            i8 keyboard_ui_override;           // 0x67e0
            u8 unknown_67e1;                   // 0x67e1
            u8 unknown_67e2[2];                // 0x67e2
        };
    }; // 0x6608

    i32 shutdown_state;               // 0x67e4
    i32 unknown_67e8;                 // 0x67e8
    i32 unknown_67ec;                 // 0x67ec
    PC_INPUT_COMMAND_PC commands[32]; // 0x67f0
    u8 *keyboard_state;               // 0x68b0
    PC_INPUT_RUNTIME_PC();

    i32 PollKeyboardPc() {
        if (!keyboard_polled) {
            keyboard_polled = 1;
            keyboard_state = nu_keyboard_owner.PollKeyboardPc();
        }
        return 1;
    }

    void InitConfiguration();
    void ReadConfiguration(PC_INPUT_CONFIG_PC *, const char *);
    PC_INPUT_CONFIG_PC *FindConfig(u8 mask, i32 port, i32 key);
    void InstallConfig(PC_INPUT_CONFIG_PC *, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port);
    void BindPreferredConfig(i32 mask, i32 port);
    i32 ResolveAuxiliaryConflicts(i32 auxiliary_index, i32 bank_count);
    i32 SelectConfiguration(i32 port, i32 key, i32 auxiliary_index);
    u8 InstallDropInConfiguration(i32 port, i32 player_index);
    void InitKeyboardProfiles();
    void AutoConfigurePadsPc(i32 count);
    void ServicePendingCapture();
    void AdvanceCaptureRecords();

    void EndFramePc() {
        ServicePendingCapture();
        AdvanceCaptureRecords();
        read_mask = 0;
        keyboard_polled = 0;
        mouse_polled = 0;
        capture_available = 0;
    }

    i32 ReadSample(i32 port, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u32 *, u8 *, i32 *);
    i32 SampleDevicePc(i32 port, PC_INPUT_RAW_SAMPLE_PC *sample);
    i32 UpdateMousePc();
    void CaptureBinding(i32 port);
    i32 TranslateBindings(i32 port, PC_INPUT_SAMPLE_PC *, i32 valid, PC_INPUT_RAW_SAMPLE_PC *, PC_INPUT_MOUSE_RUNTIME_PC *, u8 **keys);
    void ApplyMouseAxisPc(PC_INPUT_SAMPLE_PC *sample);
    u32 CombineUiButtons(i32 port, u32 buttons);
    void NormalizeAxisPair(i16 *x, i16 *y);
    void InitXInputCaps(const PC_XINPUT_CAPABILITIES_PC *, PC_INPUT_DEVICE_PC *);
    void CopyBindings(i32, i32, i32);
    i32 HashAuxiliaryBindings();
    i32 BindingAvailable(i32, i32, i32, i32);
    void ClearOtherBankConflicts(i32, i32);
    void AssignBinding(i32, i32, i32, i32, i32, i32, i32, u32);
    void ExportConfig(i32, PC_INPUT_CONFIG_PC *, i32, i32, i32);
    void PromoteConfig(PC_INPUT_CONFIG_PC *, i32, PC_INPUT_CONFIG_PC *);
    void SanitizeConfigName(const char *, char *);
    void FormatConfigText(char *, PC_INPUT_CONFIG_PC *);
    void WriteConfigText(const char *, PC_INPUT_CONFIG_PC *);
    void WriteConfiguration(PC_INPUT_CONFIG_PC *, i32, const char *);
    void SaveConfigurationPc();
    i32 fun_006d38b0(PC_INPUT_CONFIG_PC *config);
    void fun_006d3970(PC_INPUT_CONFIG_PC *config);
    void fun_006d3980(i32);
    void fun_006d3b10(i32);
    i32 fun_006d49f0(i32 button);
    i32 fun_006d4a30(i32 axis, i32 active);
    void fun_006d4d20(i32 value);
    i32 fun_006d4d50();
    void fun_006d4d70();
    i32 fun_006d4ef0(i32 action, i32 *active, i32 *axis, i32 *button, i32 *value);
    i32 fun_006d4f60(i32 active, i32 code);
    void fun_006d4fc0(const char *keys);
    i32 fun_006d4fe0(i32 port, i32 kind, i32 value);
    i32 fun_006d6db0(i32 port, i32 index);
    void fun_006d6eb0(i32 port, PC_INPUT_CONFIG_PC *config);
    PC_INPUT_CONFIG_PC *fun_006d6ed0(u8 mask, i32 port);
    void fun_006d6ef0(u8 mask, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port, i32 fallback);
    i32 fun_006d7410(i32 key, i32 port);
    i32 fun_006d99e0(i32 port, u8 mask);
    u32 fun_006d82d0(i32, u32 buttons, i32 key, i32 mouse_button, i32 axis, i32 direction, u32 bit);
    void fun_006d35d0(u8 value);
    void fun_006d35e0(i32 port, u8 value);
    void fun_006d3600(i32 port);
    i32 fun_006d3610();
    void fun_006d3620();
    void fun_006d3630(f32 threshold, i32 motion, f32 ramp);
    void fun_006d36d0();
    void fun_006d3710(i32 port, i32 *buttons, i32 *axes, i32 *povs);
    i32 fun_006d3750(PC_INPUT_CONFIG_PC *records, i32 count, PC_INPUT_CONFIG_PC *config);
    i32 fun_006d37a0(i32 previous, i32 value);
    PC_INPUT_CONFIG_PC *fun_006d4500(u8 mask, const char *name);
    void fun_006d45e0(i32 key);
    i32 fun_006d4790(i32);
    i32 fun_006d4de0(i32 port, i32 action, i32 auxiliary, i32 bank);
    void fun_006d4c90(u32 layout_id);
    void ResetInputRecords();
    void fun_006d4e20(i32 value);
    i32 GetInputRecordCount();
    i32 fun_006d4e40();
    i32 fun_006d4e50();
    void InsertInputRecord(i32 index, i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04);
    void AddInputRecord(i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04);
    void InitializeDefaultRecords();
};

// Add public declarations in C++ context (PC_INPUT_CONFIG_PC type is C++ only).
typedef i32 (*PC_INPUT_COMMAND_FILTER_PC)(i32, i32);
extern u8 PcInputAxisRemap[16];
extern u32 PcInputAxisRemapInitialized;
extern "C" const char PCKeyboardMouse2Name[16];
extern "C" i32 dat_00824fb8;
extern "C" u8 PcInput_MappingIsGood(i32 port, i32 player_index);
extern "C" void PcInputCopyBindingsPc(i32 port, i32 to_auxiliary, i32 bank);
extern "C" void PcInputResetMouseMotionPc();
extern "C" char *PcInputDeviceNamePc(i32 port);
extern "C" void PcInputSuppressKeyOnePc(i32 value);
extern "C" void PcInputSetMouseFieldPc(i32 value);
extern "C" void PcInputSetCapturedPortPc(i32 port);
extern "C" i32 PcInputCapturedPortPc();
extern "C" void PcInputSetBlockedKeysPc(const char *keys);
extern "C" i32 PcInputIsMouseDevicePc(i32 port);
extern "C" i32 PcInputIsJoystickDevicePc(i32 port);
extern "C" i32 PcInputMixConfigurationHashPc(i32 previous, i32 value);
extern "C" i32 PcInputHashAuxiliaryBindingsPc();
extern "C" void PcInputInstallConfigPc(PC_INPUT_CONFIG_PC *config, i32 port, i32 key, i32 auxiliary, i32 bank);
extern "C" i32 PcInputBindingPresentPc(i32 port, i32 action, i32 auxiliary, i32 bank);
extern "C" PC_INPUT_CONFIG_PC *PcInputFindConfigPc(u8 mask, i32 port, i32 key);
extern "C" i32 PcInputBindingAvailablePc(i32 bank, i32 bank_count, i32 kind, i32 value);
extern "C" void PcInputClearOtherBankConflictsPc(i32 bank, i32 bank_count);
extern "C" i32 PcInputResolveAuxiliaryConflictsPc(i32 bank, i32 bank_count);
extern "C" i32 PcInputAlternateDeadzonePc(i32 port);
extern "C" void PcInputSetDeadzonePc(i32 port, i32 value);
extern "C" void PcInputSetAlternateDeadzonePc(i32 port, i32 value);
extern "C" PC_INPUT_COMMAND_FILTER_PC PcInputGetCommandFilterPc();
extern "C" void PcInputAssignBindingPc(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded);
extern "C" i32 PcInputBindingButtonNamePc(i32 action, i32 port, char *destination, i32 auxiliary, i32 bank, i32 a, i32 b, char *c);
extern "C" void PcInputStageAuxiliaryConfigPc(i32 port, i32 bank, i32 key);
extern "C" void PcInputSaveConfigurationPc();

extern PC_INPUT_CONFIG_PC PcStaticInputConfigs[24];
extern PC_DI_CAPS PcKeyboardCapabilities;
extern "C" BOOL IsXInputProductPc(const GUID *);
extern "C" void ConfigureJoystickRangesPc(NuKeyboardDevice_PC *);

extern const GUID PcWbemClassId;
extern const GUID PcWbemInterfaceId;
extern "C" char *PcGetUserFolder(const char *, const char *);
struct _iobuf;
extern "C" _iobuf *PcOpenPlatformFile(const char *, const char *, i32, const char *);

#endif
#ifdef __cplusplus
struct PC_INPUT_RUNTIME_PC;
extern const char *PcCompanyName;
extern void (*PcInputStateInitializedCallback)(PC_INPUT_RUNTIME_PC *);
extern TEXT_INPUT_LABELS_PC dat_00820560;
extern TEXT_INPUT_LABELS_PC dat_00820480;
extern TEXT_INPUT_LABELS_PC dat_00820aa0;
extern TEXT_INPUT_LABELS_PC dat_00820d40;
extern TEXT_INPUT_LABELS_PC dat_00820c60;
extern TEXT_INPUT_LABELS_PC dat_00820b80;
extern TEXT_INPUT_LABELS_PC dat_00820640;
extern TEXT_INPUT_LABELS_PC dat_008209c0;
extern TEXT_INPUT_LABELS_PC dat_00820e20;
extern TEXT_INPUT_LABELS_PC dat_008210c0;
extern TEXT_INPUT_LABELS_PC dat_00820f00;
extern TEXT_INPUT_LABELS_PC dat_00820fe0;
extern TEXT_INPUT_LABELS_PC dat_00820800;
extern TEXT_INPUT_LABELS_PC dat_00820720;
extern TEXT_INPUT_LABELS_PC dat_008208e0;
extern char *dat_00821910[238];
extern char *dat_00822080[222];
extern char *dat_00822ae8[222];
extern char *dat_00822770[222];
extern char *dat_008223f8[222];
extern char *dat_008231d8[238];
extern char *dat_008240b8[238];
extern char *dat_00822e60[222];
extern char *dat_00821cc8[238];
extern char *dat_00823590[238];
extern char *dat_00823948[238];
extern char *dat_00824828[238];
extern char *dat_00824470[238];
extern char *dat_00823d00[238];
extern char *dat_00824be0[238];
extern char *dat_00821558[238];
#endif
#ifdef __cplusplus
extern "C" void PcKeyDown(i32 scan_code);
extern "C" void PcKeyUp(i32 scan_code);
extern "C" void PcPrepareTiming(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void SetPadDescriptorPc(i32, const u16 *);
    void InitPadDescriptorsPc(const u16 *, const u16 *, const u16 *, const u16 *, const i8 *);
    i32 QueueKeyPress(i32, i32);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PcIntelShaderFallback;
    extern u8 PcCapsConfigured;
    // Volatile in retail: filled from the window proc; NuKeyGet re-reads Head with no intervening store or call.
    extern volatile u8 PcKeyQueueHead;
    extern volatile u8 PcKeyQueueTail;
    extern volatile u8 PcKeyQueueScanCodes[256];
    extern volatile u8 PcKeyQueueModifiers[256];
    extern u8 PcPressedScanCodes[256];

    void InitPadDescriptorsPc(const u16 *, const u16 *, const u16 *, const u16 *, const i8 *);
    extern i8 PcInputActionOrder[20];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 AreaGetKeyboardOverridePc(void);
    void PCPresentationPlatformLeafPc(void);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 GamePadReadBackendPc(struct FRAME_PAD_PC *pad, i32 ignored);
    i32 NuPadReadOutputsPc(i32 port, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u32 *, u8 *, i32 *);
    void PcInputEndFramePc(void);
    extern const u8 PcInputPovBitsPc[8];
    extern u8 PcMouseAxisModesPc[3];
    extern u32 mbuttons;
    extern f32 PcMouseAxisIdlePc[3];
    extern i32 PcMouseCurrentAnglePc;
    extern i32 PcMouseTargetAnglePc;
    extern f32 PcMouseRadiusPc;
    extern f32 PcMouseInterpolationPc;
    extern i8 PcMouseSampleIndexPc;
#ifdef __cplusplus
    extern PC_DI_MOUSE_STATE_PC PcMouseSamplesPc[2];
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern char TextInputActionLabelBufferPc[128];
    char *TextInputActionLabelPc(i32, i32, i32, i32);
    f32 fun_006d5c50(void);
    f32 fun_006d5c60(void);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern const char *PcInputBindingKindNames[14];
extern u8 PcInputWriteTextProfiles;
#endif
