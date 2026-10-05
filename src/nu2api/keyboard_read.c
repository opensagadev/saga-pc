#include "saga.h"
#include <string.h>

typedef struct NuKeyboardDeviceVTable_PC {
    u8 unknown_0000_to_001c[0x1c];
    i32(__stdcall *acquire)(NuKeyboardDevice_PC *device);
    u8 unknown_0020_to_0024[4];
    i32(__stdcall *get_state)(NuKeyboardDevice_PC *device, u32 size, void *state);
    u8 unknown_0028_to_0064[0x64 - 0x28];
    i32(__stdcall *poll)(NuKeyboardDevice_PC *device);
} NuKeyboardDeviceVTable_PC;

struct NuKeyboardDevice_PC {
    NuKeyboardDeviceVTable_PC *vtable;
};

STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, acquire) == 0x1c);
STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, get_state) == 0x24);
STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, poll) == 0x64);

u8 *__fastcall NuKeyboardPoll(NuKeyboardOwner_PC *owner) {
    NuKeyboardDevice_PC *device;
    u8 *page;
    u8 page_index;
    i32 i;
    i32 next_key_index;
    i32 previous_key_index;

    owner->poll_status = 0;
    if ((nu_keyboard_device_flags & 2) != 0 || owner->device == 0) {
        page_index = nu_keyboard_page_index;
        page = nu_keyboard_pages + (i8)page_index * 0x100;
        memset(page, 0, nu_keyboard_owner.key_count);
        return page;
    }

    owner->poll_status = 1;
    device = owner->device;
    if (device->vtable->poll(device) < 0 && (device->vtable->acquire(device) < 0 || device->vtable->poll(device) < 0)) {
        page_index = nu_keyboard_page_index;
        page = nu_keyboard_pages + (i8)page_index * 0x100;
        memset(page, 0, nu_keyboard_owner.key_count);
        return page;
    }

    page_index = (u8)(nu_keyboard_page_index ^ 1);
    nu_keyboard_page_index = page_index;
    page = nu_keyboard_pages + (i8)page_index * 0x100;
    if (device->vtable->get_state(device, 0x100, page) != 0) {
        memset(page, 0, nu_keyboard_owner.key_count);
        return page;
    }

    previous_key_index = nu_keyboard_key_index;
    nu_keyboard_previous_key_index = previous_key_index;
    next_key_index = 0;
    for (i = 0; i < nu_keyboard_owner.key_count; ++i) {
        if (page[i] != 0 && i != previous_key_index) {
            next_key_index = i;
        }
    }

    if (nu_keyboard_repeat_countdown != 0) {
        --nu_keyboard_repeat_countdown;
        if (next_key_index != 0) {
            nu_keyboard_repeat_countdown = 6;
        }
        memset(page, 0, nu_keyboard_owner.key_count);
        if (previous_key_index != 0 && page[previous_key_index] == 0) {
            nu_keyboard_key_index = 0;
        }
    } else if (next_key_index != 0) {
        nu_keyboard_key_index = next_key_index;
    }

    if (nu_keyboard_repeat_countdown == 0 && next_key_index == 0) {
        /* Preserve the last key while it remains down; otherwise clear it. */
        if (nu_keyboard_key_index != 0 && page[nu_keyboard_key_index] == 0) {
            nu_keyboard_key_index = 0;
        }
    } else if (nu_keyboard_key_index != 0 && page[nu_keyboard_key_index] == 0) {
        nu_keyboard_key_index = 0;
    }

    if (nu_keyboard_key_index == 0) {
        nu_keyboard_no_key_state = 0;
    }
    return page;
}

void __cdecl NuKeyboardRead(void) {
    NuKeyboardState_PC *state;

    state = nu_input_state;
    if (!state->keyboard_polled) {
        state->keyboard_polled = 1;
        state->keyboard_state = NuKeyboardPoll(&nu_keyboard_owner);
    }
}

i32 NuKeyToAscii(u32 key, i32 shifted) {
    NUKEYCHAR *entry;

    for (entry = NuKeyChar; entry->key != 0; ++entry) {
        if (entry->key == key) {
            return shifted ? entry->shifted : entry->normal;
        }
    }
    return 0;
}

/* PC input startup remains structurally incomplete at the marked platform
 * dependencies; these candidates preserve the verified wrapper ABI/order. */
extern HINSTANCE PCInputWindow_candidate;
extern HWND PCInputInstance_candidate;

i32 __cdecl NuKeyboardPlatformStartup_candidate(void) {
    STUBBED_NAMED(0x006d3150); /* PC input subsystem pre-configuration */
    NuKeyboardCreateWindowState_candidate(PCInputInstance_candidate, PCInputWindow_candidate);
    SetCursor(NULL);
    STUBBED_NAMED(0x006e57d0);
    STUBBED_NAMED(0x006e5340);
    return 1;
}
