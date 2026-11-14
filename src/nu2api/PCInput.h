#ifndef SAGA_NU2API_PCINPUT_H
#define SAGA_NU2API_PCINPUT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/edtools/edui.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufile/nufile.h"

#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#endif

typedef struct NuKeyboardState_PC {
    u8 unknown_0000_to_4e10[0x4e10];
    u8 keyboard_polled;
    u8 unknown_4e11_to_5d29[0x5d29 - 0x4e11];
    u8 unknown_5d29;
    u8 unknown_5d2a_to_6314[0x6314 - 0x5d2a];
    f32 mouse_delta[3];
    f32 mouse_smoothed[3];
    f32 cursor_x;
    f32 cursor_y;
    u8 unknown_6334_to_68b0[0x68b0 - 0x6334];
    u8 *keyboard_state;
} NuKeyboardState_PC;

STATIC_ASSERT(sizeof(NuKeyboardState_PC) == 0x68b4);
STATIC_ASSERT(offsetof(NuKeyboardState_PC, keyboard_polled) == 0x4e10);
STATIC_ASSERT(offsetof(NuKeyboardState_PC, keyboard_state) == 0x68b0);

typedef struct NuKeyboardDevice_PC NuKeyboardDevice_PC;
typedef struct PC_DI_FACTORY_PC PC_DI_FACTORY_PC;

typedef struct NuKeyboardOwner_PC {
    f32 unknown_0000;
    i32 unknown_0004;
    f32 unknown_0008;
    i32 device_count;
    u8 unknown_0010_to_0020[0x10];
    NuKeyboardDevice_PC *mouse;
    NuKeyboardDevice_PC *device;
    i32 poll_status;
    i32 key_count;
    PC_DI_FACTORY_PC *factory;
    i32 mouse_axes;
    i32 mouse_buttons;
    i32 mouse_cooperative;
    u8 mouse_enabled;
    u8 unknown_41[3];
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

STATIC_ASSERT(sizeof(NuKeyboardOwner_PC) == 0x44);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, device_count) == 0x0c);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse) == 0x20);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, device) == 0x24);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, poll_status) == 0x28);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, key_count) == 0x2c);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, factory) == 0x30);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_axes) == 0x34);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_buttons) == 0x38);

typedef struct GRAPHICS_CAPS_PC {
    i32 device_type;
    u32 adapter_ordinal;
    u32 unknown_008[3];
    u32 caps2;
    u32 unknown_018[1];
    u32 device_caps;
    u32 unknown_020[7];
    u32 texture_caps;
    u32 unknown_040[0x16];
    u32 sampler_count_pc;
    u32 unknown_09c[0xa];
    u32 vertex_shader_version;
    u32 unknown_0c8[1];
    u32 pixel_shader_version;
    u32 unknown_0d0[8];
    u32 simultaneous_render_targets;
    u32 unknown_0f4[0xf];
} GRAPHICS_CAPS_PC;

#pragma pack(push, 4)

typedef struct GRAPHICS_ADAPTER_IDENTIFIER_PC {
    char driver[512];
    char description[512];
    char device_name[32];
    LARGE_INTEGER driver_version;
    u32 vendor;
    u32 device_id;
    u32 subsystem_id;
    u32 revision;
    GUID device_identifier;
    u32 whql_level;
} GRAPHICS_ADAPTER_IDENTIFIER_PC;

#pragma pack(pop)
STATIC_ASSERT(sizeof(GRAPHICS_CAPS_PC) == 0x130);
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, caps2) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, device_caps) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, vertex_shader_version) == 0xc4);
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, simultaneous_render_targets) == 0xf0);
STATIC_ASSERT(sizeof(GRAPHICS_ADAPTER_IDENTIFIER_PC) == 0x44c);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, driver) == 0x0);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, description) == 0x200);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, device_name) == 0x400);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, driver_version) == 0x420);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, vendor) == 0x428);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, device_id) == 0x42c);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, subsystem_id) == 0x430);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, revision) == 0x434);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, device_identifier) == 0x438);
STATIC_ASSERT(offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, whql_level) == 0x448);

typedef struct TEXT_INPUT_LABELS_PC TEXT_INPUT_LABELS_PC;

struct TEXT_INPUT_LABELS_PC {
    union {
        char *initializer_names[37];
        char *indexed_names[34];

        struct {
            char *fixed_names[4];
            char *button_prefix;
            char *axis_negative;
            char *axis_positive;
            char *axis_format;
            char *axis_names[6];
            u8 unknown_0038[0x5c];
        };
    };

    char *direction_positive[3];
    char *direction_negative[3];
    char *hat_names[3];
    char *hat_format;
    char *unknown;
};

STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, button_prefix) == 0x10);
STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, axis_names) == 0x20);
STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, direction_positive) == 0x94);
STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, direction_negative) == 0xa0);
STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, hat_names) == 0xac);
STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, unknown) == 0xbc);
STATIC_ASSERT(sizeof(TEXT_INPUT_LABELS_PC) == 0xc0);

STATIC_ASSERT(offsetof(TEXT_INPUT_LABELS_PC, initializer_names) == 0);
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
    extern PC_NATIVE_DEBUG_STATE_PC PCNativeWarningDebugState;
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
    void PCGraphicsFatalDiagnostic(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct PC_DI_CAPS_PC {
    DWORD size;
    DWORD flags;
    DWORD device_type;
    DWORD axes;
    DWORD buttons;
    DWORD povs;
    DWORD ff_sample_period;
    DWORD ff_min_time_resolution;
    DWORD firmware_revision;
    DWORD hardware_revision;
    DWORD ff_driver_version;
};

struct PC_DI_PROPERTY_PC {
    DWORD size;
    DWORD header_size;
    DWORD object;
    DWORD how;
    DWORD data;
};

struct PC_DI_OBJECT_FORMAT_PC {
    const GUID *guid;
    DWORD offset;
    DWORD type;
    DWORD flags;
};

struct PC_DI_DATA_FORMAT_PC {
    DWORD size;
    DWORD object_size;
    DWORD flags;
    DWORD data_size;
    DWORD object_count;
    const PC_DI_OBJECT_FORMAT_PC *objects;
};

struct PC_DI_INSTANCE_PC {
    DWORD size;
    GUID instance;
    GUID product;
    DWORD device_type;
    char instance_name[260];
    char product_name[260];
    u8 unknown_230[0x14];
};

struct PC_DI_DEVICE_METHODS_PC {
    void *unknown_00;
    void *unknown_04;
    ULONG(__stdcall *release)(NuKeyboardDevice_PC *);
    HRESULT(__stdcall *get_caps)(NuKeyboardDevice_PC *, PC_DI_CAPS_PC *);
    void *unknown_10;
    void *unknown_14;
    HRESULT(__stdcall *set_property)(NuKeyboardDevice_PC *, REFGUID, const PC_DI_PROPERTY_PC *);
    HRESULT(__stdcall *acquire)(NuKeyboardDevice_PC *);
    HRESULT(__stdcall *unacquire)(NuKeyboardDevice_PC *);
    HRESULT(__stdcall *get_state)(NuKeyboardDevice_PC *, DWORD, void *);
    void *unknown_28;
    HRESULT(__stdcall *set_format)(NuKeyboardDevice_PC *, const PC_DI_DATA_FORMAT_PC *);
    void *unknown_30;
    HRESULT(__stdcall *set_cooperative_level)(NuKeyboardDevice_PC *, HWND, DWORD);
    void *unknown_38_60[11];
    HRESULT(__stdcall *poll)(NuKeyboardDevice_PC *);
};

struct NuKeyboardDevice_PC {
    PC_DI_DEVICE_METHODS_PC *methods;
};

struct PC_DI_FACTORY_METHODS_PC {
    void *unknown_00;
    void *unknown_04;
    ULONG(__stdcall *release)(PC_DI_FACTORY_PC *);
    HRESULT(__stdcall *create_device)(PC_DI_FACTORY_PC *, REFGUID, NuKeyboardDevice_PC **, IUnknown *);
    HRESULT(__stdcall *enum_devices)
    (PC_DI_FACTORY_PC *, DWORD, BOOL(__stdcall *)(const PC_DI_INSTANCE_PC *, void *), void *, DWORD);
};

struct PC_DI_FACTORY_PC {
    PC_DI_FACTORY_METHODS_PC *methods;
};

extern const GUID PcDirectInput8AId;
extern const GUID PcSystemMouseId;
extern const GUID PcSystemKeyboardId;
extern const PC_DI_DATA_FORMAT_PC PcMouseDataFormat;
extern const PC_DI_DATA_FORMAT_PC PcKeyboardDataFormat;
extern const PC_DI_DATA_FORMAT_PC PcJoystickDataFormat;
extern i32 PcInputSelectedPort;
extern "C" BOOL __stdcall InputEnumDevicesPc(const PC_DI_INSTANCE_PC *, void *);
STATIC_ASSERT(sizeof(PC_DI_CAPS_PC) == 44);
STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, flags) == 4);
STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, axes) == 12);
STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, buttons) == 16);
STATIC_ASSERT(sizeof(PC_DI_PROPERTY_PC) == 20);
STATIC_ASSERT(sizeof(PC_DI_DATA_FORMAT_PC) == 24);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, objects) == 20);
STATIC_ASSERT(sizeof(PC_DI_OBJECT_FORMAT_PC) == 16);
STATIC_ASSERT(offsetof(PC_DI_INSTANCE_PC, instance) == 4);
STATIC_ASSERT(offsetof(PC_DI_INSTANCE_PC, product) == 0x14);
STATIC_ASSERT(offsetof(PC_DI_INSTANCE_PC, device_type) == 0x24);
STATIC_ASSERT(offsetof(PC_DI_INSTANCE_PC, product_name) == 0x12c);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, get_caps) == 0xc);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, set_property) == 0x18);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, acquire) == 0x1c);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, set_format) == 0x2c);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, set_cooperative_level) == 0x34);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, poll) == 0x64);
STATIC_ASSERT(offsetof(PC_DI_FACTORY_METHODS_PC, create_device) == 0xc);
STATIC_ASSERT(offsetof(PC_DI_FACTORY_METHODS_PC, enum_devices) == 0x10);
STATIC_ASSERT(sizeof(NuKeyboardOwner_PC) == 0x44);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse) == 0x20);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, key_count) == 0x2c);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, factory) == 0x30);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_axes) == 0x34);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_buttons) == 0x38);

STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, size) == 0x0);
STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, device_type) == 0x8);
STATIC_ASSERT(offsetof(PC_DI_CAPS_PC, povs) == 0x14);
STATIC_ASSERT(offsetof(PC_DI_PROPERTY_PC, size) == 0x0);
STATIC_ASSERT(offsetof(PC_DI_PROPERTY_PC, header_size) == 0x4);
STATIC_ASSERT(offsetof(PC_DI_PROPERTY_PC, object) == 0x8);
STATIC_ASSERT(offsetof(PC_DI_PROPERTY_PC, how) == 0xc);
STATIC_ASSERT(offsetof(PC_DI_PROPERTY_PC, data) == 0x10);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, size) == 0x0);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, object_size) == 0x4);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, flags) == 0x8);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, data_size) == 0xc);
STATIC_ASSERT(offsetof(PC_DI_DATA_FORMAT_PC, object_count) == 0x10);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, release) == 0x8);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, unacquire) == 0x20);
STATIC_ASSERT(offsetof(PC_DI_DEVICE_METHODS_PC, get_state) == 0x24);

struct PC_INPUT_SAMPLE_PC {
    u8 analog[8];
    u32 buttons;
};

struct PC_INPUT_RAW_SAMPLE_PC {
    i16 axes[32];
    u8 buttons[132];
};

struct PC_DI_MOUSE_STATE_PC {
    i32 axes[3];
    u8 buttons[8];
};

struct PC_INPUT_CAPTURE_PC {
    u8 kind[3];
    u8 unknown_03;
    i16 value[3];
};

struct PC_INPUT_MOUSE_RUNTIME_PC {
    i16 current[3];
    i16 previous[3];
    f32 accumulated[3];
    f32 delta[3];
    u8 unknown_24[12];
    f32 position[2];
    u8 buttons[16];
    u8 unknown_48;
    u8 port_activity[14];
    u8 unknown_57;
    i32 field_58;
};

STATIC_ASSERT(sizeof(PC_INPUT_SAMPLE_PC) == 12);
STATIC_ASSERT(offsetof(PC_INPUT_SAMPLE_PC, buttons) == 8);
STATIC_ASSERT(sizeof(PC_INPUT_RAW_SAMPLE_PC) == 196);
STATIC_ASSERT(offsetof(PC_INPUT_RAW_SAMPLE_PC, buttons) == 0x40);
STATIC_ASSERT(sizeof(PC_DI_MOUSE_STATE_PC) == 20);
STATIC_ASSERT(offsetof(PC_DI_MOUSE_STATE_PC, buttons) == 12);
STATIC_ASSERT(sizeof(PC_INPUT_CAPTURE_PC) == 10);
STATIC_ASSERT(offsetof(PC_INPUT_CAPTURE_PC, value) == 4);
STATIC_ASSERT(sizeof(PC_INPUT_MOUSE_RUNTIME_PC) == 0x5c);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, previous) == 6);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, accumulated) == 0xc);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, delta) == 0x18);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, position) == 0x30);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, buttons) == 0x38);
STATIC_ASSERT(offsetof(PC_INPUT_MOUSE_RUNTIME_PC, port_activity) == 0x49);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_cooperative) == 0x3c);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, mouse_enabled) == 0x40);

#pragma pack(push, 2)

struct PC_INPUT_CONFIG_PC {
    char name[64];
    u32 product_id;

    union {
        u8 unknown_44[0x16];

        struct {
            u32 guid_data2;
            u32 guid_data3;
            u8 guid_data4[8];
            i16 button_count;
            i16 pov_count;
            i16 axis_count;
        };
    };

    u8 deadzone;
    u8 unknown_5b;
    u8 alternate_deadzone;
    u8 unknown_5d;
    i16 key;
    u8 unknown_60[4];

    union {
        struct {
            u8 axes[4][3];
            u8 unknown_axes[4][3];
        };

        u8 all_axes[8][3];
    };

    u8 buttons[16][2];
};

#pragma pack(pop)

struct PC_INPUT_COMMAND_PC {
    i8 active;
    i8 axis;
    i8 button;
    u8 unknown_03;
    u16 unknown_04;
};

struct PC_INPUT_BINDING_PC {
    u8 kind;
    u8 unknown_01;
    i16 value;
};

struct PC_INPUT_DEVICE_PC {
    i32 kind;
    NuKeyboardDevice_PC *device;
    GUID product;
    i16 button_count;
    i8 axis_count;
    i8 pov_count;
    u8 deadzone;
    u8 alternate_deadzone;
    u8 unknown_1e[2];
    i32 left_trigger;
    i32 right_trigger;

    union {
        u8 unknown_28[2];

        struct {
            u8 action_keys_active;
            u8 unknown_29;
        };
    };

    u8 config_dirty;
    u8 config_installed;
    u8 config_present;
    u8 capabilities_valid;
    char name[64];
    PC_INPUT_BINDING_PC bindings[32];

    struct {
        u16 first;
        u16 second;
    } reset_pairs[32];

    u8 unknown_16e[2];
    PC_INPUT_SAMPLE_PC sample;
};

struct PC_XINPUT_GAMEPAD_PC {
    u16 buttons;
    u8 left_trigger;
    u8 right_trigger;
    i16 thumb_lx;
    i16 thumb_ly;
    i16 thumb_rx;
    i16 thumb_ry;
};

struct PC_XINPUT_STATE_PC {
    u32 packet_number;
    PC_XINPUT_GAMEPAD_PC gamepad;
};

STATIC_ASSERT(sizeof(PC_XINPUT_STATE_PC) == 16);
STATIC_ASSERT(offsetof(PC_XINPUT_STATE_PC, gamepad) == 4);
STATIC_ASSERT(sizeof(PC_XINPUT_GAMEPAD_PC) == 12);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, left_trigger) == 2);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, right_trigger) == 3);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, thumb_lx) == 4);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, thumb_ly) == 6);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, thumb_rx) == 8);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, thumb_ry) == 10);

struct PC_XINPUT_CAPABILITIES_PC {
    u8 type;
    u8 subtype;
    u16 flags;
    PC_XINPUT_GAMEPAD_PC gamepad;
    u16 motor_left;
    u16 motor_right;
};

struct PC_INPUT_RUNTIME_PC {
    PC_INPUT_CONFIG_PC primary[64];
    PC_INPUT_CONFIG_PC secondary[64];

    union {
        u8 unknown_4e00[0x34];

        struct {
            i32 field_4e00;
            u32 read_mask;
            u32 action_mask;
            u32 completed_mask;
            u8 keyboard_polled;
            u8 mouse_polled;
            u8 suppress_key_1;

            union {
                struct {
                    u8 field_4e13;
                    u8 unknown_4e14[0x20];
                };

                char blocked_keys[33];
            };
        };
    };

    PC_INPUT_DEVICE_PC devices[14];

    union {
        u8 unknown_62fc[0x5c];
        PC_INPUT_MOUSE_RUNTIME_PC mouse;
    };

    PC_INPUT_BINDING_PC auxiliary_bindings[4][32];
    i32 auxiliary_ports[4];

    union {
        u8 unknown_6568[0x9c];

        struct {
            i32 axis_threshold;
            i32 motion_threshold;
            f32 ramp_seconds;
            u8 capture_available;
            u8 unknown_6575;
            PC_INPUT_CAPTURE_PC staged_capture[14];
            u8 unknown_6602[2];
        };
    };

    i32 preferred_key;

    union {
        u8 unknown_6608[0x1dc];

        struct {
            i32 captured_port;
            PC_INPUT_CAPTURE_PC captures[14];
            u8 unknown_6698[0x82];
            u8 first_mouse;
            u8 unknown_671b;
            PC_INPUT_RAW_SAMPLE_PC raw_sample;
            i8 keyboard_ui_override;
            u8 unknown_67e1;
            u8 unknown_67e2[2];
        };
    };

    i32 shutdown_state;
    i32 unknown_67e8;
    i32 unknown_67ec;
    PC_INPUT_COMMAND_PC commands[32];
    u8 *keyboard_state;
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
    i32 ResolveAuxiliaryConflictsPc(i32 auxiliary_index, i32 bank_count);
    i32 SelectConfigurationPc(i32 port, i32 key, i32 auxiliary_index);
    u8 InstallDropInConfigurationPc(i32 port, i32 player_index);
    void InitKeyboardProfiles();
    void AutoConfigurePadsPc(i32 count);
    void ServicePendingCapturePc();
    void AdvanceCaptureRecordsPc();

    void EndFramePc() {
        ServicePendingCapturePc();
        AdvanceCaptureRecordsPc();
        read_mask = 0;
        keyboard_polled = 0;
        mouse_polled = 0;
        capture_available = 0;
    }

    i32 ReadSamplePc(i32 port, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u8 *, u32 *, u8 *, i32 *);
    i32 SampleDevicePc(i32 port, PC_INPUT_RAW_SAMPLE_PC *sample);
    i32 UpdateMousePc();
    void CaptureBindingPc(i32 port);
    i32 TranslateBindingsPc(i32 port, PC_INPUT_SAMPLE_PC *, i32 valid, PC_INPUT_RAW_SAMPLE_PC *,
                            PC_INPUT_MOUSE_RUNTIME_PC *, u8 **keys);
    void ApplyMouseAxisPc(PC_INPUT_SAMPLE_PC *sample);
    u32 CombineUiButtonsPc(i32 port, u32 buttons);
    void NormalizeAxisPairPc(i16 *x, i16 *y);
    void InitXInputCaps(const PC_XINPUT_CAPABILITIES_PC *, PC_INPUT_DEVICE_PC *);
    void CopyBindingsPc(i32, i32, i32);
    i32 HashAuxiliaryBindingsPc();
    i32 BindingAvailablePc(i32, i32, i32, i32);
    void ClearOtherBankConflictsPc(i32, i32);
    void AssignBindingPc(i32, i32, i32, i32, i32, i32, i32, u32);
    void ExportConfigPc(i32, PC_INPUT_CONFIG_PC *, i32, i32, i32);
    void PromoteConfigPc(PC_INPUT_CONFIG_PC *, i32, PC_INPUT_CONFIG_PC *);
    void SanitizeConfigNamePc(const char *, char *);
    void FormatConfigTextPc(char *, PC_INPUT_CONFIG_PC *);
    void WriteConfigTextPc(const char *, PC_INPUT_CONFIG_PC *);
    void WriteConfigurationPc(PC_INPUT_CONFIG_PC *, i32, const char *);
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
    void InsertInputRecord(i32 index, i32 field_00, i32 field_01, i32 field_02, u16 field_04);
    void AddInputRecord(i32 field_00, i32 field_01, i32 field_02, u16 field_04);
    void InitializeDefaultRecords();
};

// Add public declarations in C++ context (PC_INPUT_CONFIG_PC type is C++ only).
typedef i32 (*PC_INPUT_COMMAND_FILTER_PC)(i32, i32);
extern u8 PcInputAxisRemap[16];
extern u32 PcInputAxisRemapInitialized;
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
extern "C" void PcInputAssignBindingPc(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank,
                                       u32 excluded);
extern "C" i32 PcInputBindingButtonNamePc(i32 action, i32 port, char *destination, i32 auxiliary, i32 bank, i32 a,
                                          i32 b, char *c);
extern "C" void PcInputStageAuxiliaryConfigPc(i32 port, i32 bank, i32 key);
extern "C" void PcInputSaveConfigurationPc();

extern PC_INPUT_CONFIG_PC PcStaticInputConfigs[24];
extern PC_DI_CAPS_PC PcKeyboardCapabilities;
extern "C" BOOL IsXInputProductPc(const GUID *);
extern "C" void ConfigureJoystickRangesPc(NuKeyboardDevice_PC *);
STATIC_ASSERT(sizeof(PC_INPUT_CONFIG_PC) == 0x9c);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, guid_data2) == 0x44);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, guid_data3) == 0x48);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, guid_data4) == 0x4c);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, button_count) == 0x54);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, pov_count) == 0x56);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, axis_count) == 0x58);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, all_axes) == 0x64);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, blocked_keys) == 0x4e13);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, product_id) == 0x40);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, deadzone) == 0x5a);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, alternate_deadzone) == 0x5c);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, key) == 0x5e);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, axes) == 0x64);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, buttons) == 0x7c);
STATIC_ASSERT(sizeof(PC_INPUT_DEVICE_PC) == 0x17c);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, device) == 4);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, product) == 8);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, button_count) == 0x18);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, axis_count) == 0x1a);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, pov_count) == 0x1b);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, deadzone) == 0x1c);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, left_trigger) == 0x20);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, right_trigger) == 0x24);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, action_keys_active) == 0x28);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, unknown_28) == 0x28);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, config_dirty) == 0x2a);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, config_installed) == 0x2b);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, config_present) == 0x2c);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, capabilities_valid) == 0x2d);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, name) == 0x2e);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, bindings) == 0x6e);
STATIC_ASSERT(sizeof(PC_XINPUT_CAPABILITIES_PC) == 20);
STATIC_ASSERT(offsetof(PC_XINPUT_CAPABILITIES_PC, gamepad) == 4);
STATIC_ASSERT(offsetof(PC_XINPUT_CAPABILITIES_PC, motor_left) == 16);
STATIC_ASSERT(sizeof(PC_INPUT_RUNTIME_PC) == 0x68b4);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, secondary) == 0x2700);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, devices) == 0x4e34);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, auxiliary_bindings) == 0x6358);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, auxiliary_ports) == 0x6558);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, preferred_key) == 0x6604);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, commands) == 0x67f0);

STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, name) == 0x0);
STATIC_ASSERT(offsetof(PC_INPUT_CONFIG_PC, unknown_axes) == 0x70);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, kind) == 0x0);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, alternate_deadzone) == 0x1d);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, active) == 0x0);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, axis) == 0x1);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, button) == 0x2);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, unknown_04) == 0x4);
STATIC_ASSERT(offsetof(PC_INPUT_BINDING_PC, kind) == 0x0);
STATIC_ASSERT(offsetof(PC_INPUT_BINDING_PC, value) == 0x2);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, primary) == 0x0);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, keyboard_state) == 0x68b0);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, sample) == 0x170);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, read_mask) == 0x4e04);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, action_mask) == 0x4e08);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, completed_mask) == 0x4e0c);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, keyboard_polled) == 0x4e10);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, mouse_polled) == 0x4e11);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, suppress_key_1) == 0x4e12);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, mouse) == 0x62fc);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, mouse) + offsetof(PC_INPUT_MOUSE_RUNTIME_PC, port_activity) == 0x6345);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, axis_threshold) == 0x6568);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, motion_threshold) == 0x656c);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, ramp_seconds) == 0x6570);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, capture_available) == 0x6574);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, staged_capture) == 0x6576);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, captured_port) == 0x6608);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, captures) == 0x660c);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, first_mouse) == 0x671a);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, raw_sample) == 0x671c);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, keyboard_ui_override) == 0x67e0);

STATIC_ASSERT(offsetof(PC_XINPUT_CAPABILITIES_PC, motor_right) == 0x12);
STATIC_ASSERT(offsetof(PC_XINPUT_GAMEPAD_PC, buttons) == 0x0);

STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, reset_pairs) == 0xee);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, active) == 0);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, axis) == 1);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, button) == 2);
STATIC_ASSERT(offsetof(PC_INPUT_COMMAND_PC, unknown_04) == 4);

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
extern TEXT_INPUT_LABELS_PC PcInputLabels820560;
extern TEXT_INPUT_LABELS_PC PcInputLabels820480;
extern TEXT_INPUT_LABELS_PC PcInputLabels820aa0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820d40;
extern TEXT_INPUT_LABELS_PC PcInputLabels820c60;
extern TEXT_INPUT_LABELS_PC PcInputLabels820b80;
extern TEXT_INPUT_LABELS_PC PcInputLabels820640;
extern TEXT_INPUT_LABELS_PC PcInputLabels8209c0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820e20;
extern TEXT_INPUT_LABELS_PC PcInputLabels8210c0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820f00;
extern TEXT_INPUT_LABELS_PC PcInputLabels820fe0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820800;
extern TEXT_INPUT_LABELS_PC PcInputLabels820720;
extern TEXT_INPUT_LABELS_PC PcInputLabels8208e0;
extern char *PcInputKeyNames821910[238];
extern char *PcInputKeyNames822080[222];
extern char *PcInputKeyNames822ae8[222];
extern char *PcInputKeyNames822770[222];
extern char *PcInputKeyNames8223f8[222];
extern char *PcInputKeyNames8231d8[238];
extern char *PcInputKeyNames8240b8[238];
extern char *PcInputKeyNames822e60[222];
extern char *PcInputKeyNames821cc8[238];
extern char *PcInputKeyNames823590[238];
extern char *PcInputKeyNames823948[238];
extern char *PcInputKeyNames824828[238];
extern char *PcInputKeyNames824470[238];
extern char *PcInputKeyNames823d00[238];
extern char *PcInputKeyNames824be0[238];
extern char *PcInputKeyNames821558[238];
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
    i32 QueueKeyPressPc(i32, i32);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
STATIC_ASSERT(offsetof(PC_DI_FACTORY_METHODS_PC, release) == 8);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, shutdown_state) == 0x67e4);
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
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, pixel_shader_version) == 0xcc);
#ifdef __cplusplus
extern "C" {
#endif
    i32 AreaGetKeyboardOverridePc(void);
    void PCPresentationPlatformLeafPc(void);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, sampler_count_pc) == 0x98);
#ifdef __cplusplus
extern "C" {
#endif
    i32 GamePadReadBackendPc(FRAME_PAD_PC *pad, i32 ignored);
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
#ifdef __cplusplus
}
#endif

#endif
