#include "saga.h"

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

i16 __cdecl GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback) {
    (void)name;
    (void)table;
    (void)fallback;
    return -1;
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
