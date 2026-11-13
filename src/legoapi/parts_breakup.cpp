#include "saga.h"

struct KILLPARTS_MINIKIT_PIECE_PC {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1c];
};
STATIC_ASSERT(sizeof(KILLPARTS_MINIKIT_PIECE_PC) == 0x68);
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECE_PC, matrix) == 0xc);

struct KILLPARTS_MINIKIT_PIECES_PC {
    u8 unknown_0[4];
    KILLPARTS_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
};
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECES_PC, pieces) == 4);
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECES_PC, piece_count) == 8);

struct KILLPARTS_RENDER_PC {
    const char *name;
    NUSPECIALHANDLE **rigid_specials;
    u8 unknown_8[0xc];
};
STATIC_ASSERT(sizeof(KILLPARTS_RENDER_PC) == 0x14);
STATIC_ASSERT(offsetof(KILLPARTS_RENDER_PC, rigid_specials) == 4);

struct KILLPARTS_OBJECT_LIGHT_PC {
    u8 unknown_0[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
};
STATIC_ASSERT(offsetof(KILLPARTS_OBJECT_LIGHT_PC, lighting) == 0x4d8);

struct KILLPARTS_WORLD_SCENES_PC {
    u8 unknown_0[0x2980];
    KILLPARTS_CHARSCENE_PC *character_scenes;
};
STATIC_ASSERT(offsetof(KILLPARTS_WORLD_SCENES_PC, character_scenes) == 0x2980);

struct KILLPARTS_SAVED_PIECES_PC {
    i16 pieces[9];
    u8 unknown_12[0x26];
};
STATIC_ASSERT(sizeof(KILLPARTS_SAVED_PIECES_PC) == 0x38);

struct KILLPARTS_CUSTOMISER_PC {
    u8 unknown_0[0x24];
    CUSTOMPIECE_LAYERS_PC *head_pieces;
    CUSTOMPIECE_LAYERS_PC *helmet_pieces;
    u8 unknown_2c[0x40];
    i16 character_ids[2];
    u8 unknown_70[0x104];
    KILLPARTS_SAVED_PIECES_PC *save;
};
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, head_pieces) == 0x24);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, helmet_pieces) == 0x28);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, character_ids) == 0x6c);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, save) == 0x174);

struct KILLPARTS_PART_PC {
    u8 unknown_0[0x20c];
    f32 speeder_index;
};
STATIC_ASSERT(offsetof(KILLPARTS_PART_PC, speeder_index) == 0x20c);

struct KILLPARTS_RIDER_LIGHT_PC {
    u8 unknown_0[0x628];
    NUVEC intensity[2];
};
STATIC_ASSERT(offsetof(KILLPARTS_RIDER_LIGHT_PC, intensity) == 0x628);

extern "C" {

    void __cdecl PartImpact_Brick(PART_PC *part) {
        if (KillPartsBrickImpactWaitPc <= 0.0f) {
            f32 speed = NuVecMag(&part->velocity);
            if (speed > 0.0f) {
                if (speed > 1.0f)
                    speed = 1.0f;
                const i32 angle = static_cast<i32>((1.0 - speed) * 16384.0 + 16384.0);
                const f32 volume = NuTrigTable[(angle >> 1) & 0x7fff];
                const i32 sound_id = GetSfxId(KillPartsLegoSingleTextPc);
                PlaySfxByIdAndSetVolume(sound_id, &part->position, volume);
                KillPartsBrickImpactWaitPc = 0.1f;
            }
        }
    }
}
