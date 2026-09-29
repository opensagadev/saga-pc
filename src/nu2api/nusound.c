#include "saga.h"

void __cdecl SfxSetFileTable(SFX_FILE_INFO_PC *records) {
    i32 index = 0;

    SfxFileTable = records;
    SfxFileCount = 0;
    if (records == NULL) {
        return;
    }

    while (records[index].filename != NULL) {
        NuStrLen(records[index].filename);
        index++;
        SfxFileCount++;
    }
}

void __cdecl SfxBuildCRCTable(VARIPTR *buffer_cursor) {
    u32 *table;
    i32 index;

    if (SfxCRCTableInitialized != 0) {
        return;
    }

    buffer_cursor->addr = (buffer_cursor->addr + 3) & ~3u;
    table = (u32 *)buffer_cursor->void_ptr;
    SfxCRCTable = table;
    buffer_cursor->addr += 0x400;

    for (index = 0; index < 256; index++) {
        u32 value = (u32)index << 24;
        i32 bit;

        for (bit = 0; bit < 8; bit++) {
            if ((value & 0x80000000) != 0) {
                value = (value << 1) ^ 0x04c11db7;
            } else {
                value <<= 1;
            }
        }
        table[index] = value;
    }
    SfxCRCTableInitialized = 1;
}

u32 __cdecl SfxNameHash(const char *name) {
    u32 hash = 0;
    i32 index = 0;

    while (name[index] != '\0') {
        u8 character = (u8)name[index];
        i32 table_index;

        if (character >= 'a' && character <= 'z') {
            character = (u8)(character - ('a' - 'A'));
        }
        table_index = (i8)character ^ (i32)(hash >> 24);
        hash = (hash << 8) ^ SfxCRCTable[table_index];
        index++;
    }
    return hash;
}

u32 __cdecl SfxNameHashN(const char *name, u32 max_length) {
    u32 hash = 0;
    u32 index = 0;

    while (name[index] != '\0' && index < max_length) {
        u8 character = (u8)name[index];
        i32 table_index;

        if (character >= 'a' && character <= 'z') {
            character = (u8)(character - ('a' - 'A'));
        }
        table_index = (i8)character ^ (i32)(hash >> 24);
        hash = (hash << 8) ^ SfxCRCTable[table_index];
        index++;
    }
    return hash;
}

i32 __cdecl GetSfxId(char *name) {
    i32 index;

    if (name == NULL || SfxHashHeads == NULL) {
        return -1;
    }

    index = SfxHashHeads[SfxNameHash(name) & 0xff];
    while (index != -1) {
        SFX_RECORD_PC *record = &SfxInfo[index];
        if (NuStrNICmp(name, record->name, 0x20) == 0) {
            return index;
        }
        index = record->next;
    }
    return -1;
}

void __cdecl SetSfxBit_OnEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return;
    }
    SfxBits[index >> 4] |= (u16)(1u << (index & 0xf));
}

void __cdecl SetSfxBit_OffEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return;
    }
    SfxBits[index >> 4] &= (u16) ~(1u << (index & 0xf));
}

i32 __cdecl SfxBitEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return -1;
    }
    return (SfxBits[index >> 4] & (u16)(1u << (index & 0xf))) != 0;
}

void __cdecl SetSfxBit_On(i32 sound_id) {
    i32 sample_id;

    if (sound_id < 0) {
        return;
    }
    sample_id = (i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17;
    if ((u32)sample_id <= 0x63f) {
        SetSfxBit_OnEx(sample_id);
    }
}

i32 __cdecl AmbientFromQuiet(i32 quiet_index) {
    i32 pair_index;

    if (quiet_index == -1) {
        return -1;
    }

    for (pair_index = 0; AmbientPairTab[pair_index * 2] != -1; pair_index++) {
        if (AmbientPairTab[pair_index * 2] == quiet_index) {
            return AmbientPairTab[pair_index * 2 + 1];
        }
    }
    return -1;
}

void __cdecl SfxBitsStore(void *sound_table) {
    PCByteCopy_candidate(sound_table, SfxBits, 0xc8);
}

void __cdecl SfxBitsRestore(void *sound_table) {
    PCByteCopy_candidate(SfxBits, sound_table, 0xc8);
}

void __cdecl SoundStopMusic(void) {
    if (NOSOUND == 0 && NOMUSIC == 0) {
        NuSound3StopStereoStream(0);
        NuSound3StopStereoStream(1);

        Music.transition = 0.0f;
        Music.state = 0;
        Music.requested_track = -1;
        Music.current_track = -1;
        Music.queued_track = -1;
        Music.primary_stream = 0;
        Music.transition_frames = 0;
        Music.pause_requested = 0;
        Music.field_0012 = 0;
        Music.field_0013 = 0;
        Music.update_delay = 0;
        Music.restore_requested = 0;
        Music.resume_track = -1;
    }
}

void __cdecl ResetLevSfx(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        world->level_sfx[index].id = -1;
    }
    world->level_sfx_count = 0;
}

void __cdecl ClearLinkedCutSceneMusic(void *context) {
    if (context == NULL || Music.track_data == context) {
        Music.track_data = NULL;
    }
}

void __cdecl ResetSounds(void) {
    PCByteCopy_candidate(SfxBits, GlobalSfxBits, 0xc8);
}

i16 __cdecl GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback) {
    (void)name;
    (void)table;
    (void)fallback;
    return -1;
}

void __cdecl UpdateBackgroundMusic(void) {
}

void SetSoundVolume(f32 volume) {
    MASTERVOLUME = volume;
}

f32 GetSoundVolume(void) {
    return MASTERVOLUME;
}

void __cdecl NuSound3AddRumble(void *pad, f32 strength, i32 duration, i32 priority, f32 buzz) {
    (void)pad;
    (void)strength;
    (void)duration;
    (void)priority;
    (void)buzz;
}

void __cdecl NuSound3SetRumblePads(void *pad0, void *pad1) {
    (void)pad0;
    (void)pad1;
}

void __cdecl NuSound3SetDat(NUDATHDR *header) {
    NuSoundDat = header;
}

i32 __cdecl NuSound3SetReverb(i32 enable) {
    i32 previous = NuSound3Reverb;
    NuSound3Reverb = enable != 0;
    return previous;
}

void NuSound3SetLoopHoldTime(f32 time) {
    NuSound3LoopHoldTime = (i32)(time * NuScale1000);
}

void __cdecl NuSound3FlushLoops(void) {
}

void __cdecl NuSound3StopRumble(void) {
}
