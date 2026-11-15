#include "legoapi/characters/core/character.h"
#include "legoapi/lib/text.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nuwin32/d3dCore.h"

// Globals whose owning unit cannot define them yet without changing code (docs/data-status.md,
// "What data.cpp still holds"). Each names the reason.

extern "C" {
    // nu2api/nucore_includes.c: with the tables in its unit VC8 compiles NuStringIsFluff and
    // NuStringBadSubString differently.
    extern const u16 bad_fluff[26] = {
        '!', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{', '}', '[', ']', '@', '~', '#', '?', '<', '>', ',', '.', ' ', 0,
    };
}

// PC reads pairs of 16-bit code points, ending with a zero pair.
u16 bad_equiv[8][2] = {
    {'a', '@'},
    {'a', '4'},
    {'e', '3'},
    {'i', '1'},
    {'l', '1'},
    {'o', '0'},
    {'t', '7'},
    {0,   0  }
};

extern "C" {
    // C++ objects with a constructor and destructor; in their units (pc_device.cpp, pc_input.cpp) the
    // initialiser and atexit functions come out differently.
    PC_DEVICE_BACKEND PCDeviceBackend;
    NuKeyboardOwner nu_keyboard_owner;
    PC_DEVICE_STATE_CACHE PCDeviceStateCache;

    // legoapi/text.c is C; this initialiser copies string pointers at startup, which only C++ does.
    LANGUAGEDATA Text_LanguageList_Default[7] = {
        {1,  apitxt_ENGLISH},
        {2,  apitxt_FRENCH },
        {4,  apitxt_GERMAN },
        {5,  apitxt_ITALIAN},
        {3,  apitxt_SPANISH},
        {8,  apitxt_DANISH },
        {-1, NULL          }
    };

    // nu2api/nuparticle_state.c: with the table in its unit VC8 compiles NuRndrSetDebBox differently.
    extern const NUVEC debris_box_corners[8] = {
        {-0.2f, -0.2f, -0.05f},
        {0.2f,  -0.2f, -0.05f},
        {-0.2f, 0.2f,  -0.05f},
        {0.2f,  0.2f,  -0.05f},
        {-0.5f, -0.5f, 0.8f  },
        {0.5f,  -0.5f, 0.8f  },
        {-0.5f, 0.5f,  0.8f  },
        {0.5f,  0.5f,  0.8f  }
    };

    // legogame/game_obj.c: with either id in its unit VC8 compiles Tag_UpdateHint differently.
    i16 id_GRABCONTROL = -1;
    i16 id_GRABR2CONTROL = -1;
}
