#include "saga.h"

void NuMusic::SetFader(f32 target, f32 fade_seconds) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    if (fade_seconds != 0.0f) {
        fader_rate = 1.0f / fade_seconds;
        fader_target = target;
        return;
    }

    fader_target = target;
    fader_current = target;
    fader_rate = 0.0f;
}

void NuMusic::SetMasterVolume(f32 volume) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    if (volume > 1.0f) {
        volume = 1.0f;
    } else if (volume < 0.0f) {
        volume = 0.0f;
    }

    master_volume = volume;
}

i32 NuMusic::GetAlbumHandle(const char *name) {
    if (this != NULL && the_music_player != NULL) {
        for (i32 i = 0; i < album_count; i++) {
            if (NuStrICmp(albums[i].name, name) == 0) {
                return i + 0x12345678;
            }
        }
    }
    return 0;
}

void NuMusic::SetClassVolume(u32 class_mask, f32 volume) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    if (volume > 1.0f)
        volume = 1.0f;
    if (volume < 0.0)
        volume = 0.0f;
    if (volume > 1.0f)
        volume = 1.0f;

    if ((class_mask & 0x01) != 0)
        class_volumes[0] = volume;
    if ((class_mask & 0x02) != 0)
        class_volumes[1] = volume;
    if ((class_mask & 0x04) != 0)
        class_volumes[2] = volume;
    if ((class_mask & 0x08) != 0)
        class_volumes[3] = volume;
    if ((class_mask & 0x10) != 0)
        class_volumes[4] = volume;
    if ((class_mask & 0x20) != 0)
        class_volumes[5] = volume;
}

void NuMusic::GetSoundFiles(nusound_filename_info_s **files, i32 *count) {
    if (this != NULL) {
        if (files != NULL) {
            *files = fileinfo;
        }
    } else if (files != NULL) {
        *files = NULL;
    }

    if (count != NULL) {
        if (this != NULL) {
            *count = file_count;
        } else {
            *count = 0;
        }
    }
}

extern "C" f32 numusicGetDuckVolume(void) {
    f32 volume;

    if (the_music_player != NULL) {
        volume = the_music_player->duck_current;
    } else {
        volume = 1.0f;
    }
    return volume;
}

i32 NuMusic::ClassToIX(u32 class_mask) {
    for (i32 index = 0; index < 6; index++) {
        if (class_mask == (1u << index)) {
            return index;
        }
    }
    return -1;
}

NuMusic::Track *NuMusic::Album::GetTrack(u32 class_mask) {
    i32 index;
    for (index = 0; index < 6; index++) {
        if (class_mask == (1u << index)) {
            return tracks[index];
        }
    }
    return NULL;
}

void NuMusic::Track::ManageEntryTime(void) {
    if (entry_count != 0) {
        entry_index = (entry_index + 1) % entry_count;
        start_offset = entry_times[entry_index];
    } else {
        start_offset = 0.0f;
    }
}

NuMusic::Voice *NuMusic::FindVoiceByTrack(Track *track) {
    i32 index;

    if (track == NULL) {
        return NULL;
    }
    for (index = 0; index < 2; index++) {
        Voice *voice = &voices[index];
        if (voice->tracks[voice->track_index] == track) {
            return voice;
        }
    }
    return NULL;
}

NuMusic::Voice *NuMusic::FindVoiceByClass(u32 class_mask) {
    i32 index;

    for (index = 0; index < 2; index++) {
        Voice *voice = &voices[index];
        Track *track = voice->tracks[voice->track_index];
        if (track != NULL && track->clazz == class_mask) {
            return voice;
        }
    }
    return NULL;
}

NuMusic::Voice *NuMusic::FindVoiceByClassAndStatus(u32 class_mask, u32 status) {
    i32 index;

    for (index = 0; index < 2; index++) {
        Voice *voice = &voices[index];
        Track *track = voice->tracks[voice->track_index];
        if (track != NULL && track->clazz == class_mask && (u32)voice->status == status) {
            return voice;
        }
    }
    return NULL;
}

NuMusic::Voice *NuMusic::FindIdleVoice(void) {
    i32 index;

    for (index = 0; index < 2; index++) {
        if (voices[index].status == 1) {
            return &voices[index];
        }
    }
    for (index = 0; index < 2; index++) {
        if (voices[index].status == 3) {
            return &voices[index];
        }
    }
    return NULL;
}

i32 NuMusic::StopAll(i32 mode) {
    i32 voice_index;

    if ((mode & 2) == 0) {
        for (voice_index = 0; voice_index < 2; voice_index++) {
            Voice *voice = &voices[voice_index];
            NuSound3StopStereoStream(voice->stream_index * 2);
            if (voice->status != 1) {
                voice->play_time = 0.0f;
                voice->status = 1;
            }
            voice->fade_rate = 0.0f;
            voice->track_index = 0;
            voice->tracks[0] = NULL;
            voice->tracks[1] = NULL;
            voice->gain = 0.0f;
        }
        return 1;
    }

    {
        Track *track = voices[0].tracks[voices[0].track_index];
        if (track == NULL || track->clazz != 0x10) {
            Voice *voice = &voices[0];
            NuSound3StopStereoStream(voice->stream_index * 2);
            if (voice->status != 1) {
                voice->play_time = 0.0f;
                voice->status = 1;
            }
            voice->fade_rate = 0.0f;
            voice->track_index = 0;
            voice->tracks[0] = NULL;
            voice->tracks[1] = NULL;
            voice->gain = 0.0f;
        }
    }

    {
        Track *track = voices[1].tracks[voices[1].track_index];
        if (track != NULL && track->clazz == 0x10) {
            return 1;
        }
    }

    {
        Voice *voice = &voices[1];
        NuSound3StopStereoStream(voice->stream_index * 2);
        if (voice->status != 1) {
            voice->play_time = 0.0f;
            voice->status = 1;
        }
        voice->fade_rate = 0.0f;
        voice->track_index = 0;
        voice->tracks[0] = NULL;
        voice->tracks[1] = NULL;
        voice->gain = 0.0f;
    }
    return 1;
}

i32 NuMusic::Album::GetTracks(u32 class_mask, Track **out_tracks) {
    i32 count = 0;
    i32 index;

    for (index = 0; index < 6; index++) {
        if ((class_mask & (1u << index)) != 0) {
            out_tracks[index] = tracks[index];
            count++;
        } else {
            out_tracks[index] = NULL;
        }
    }
    return count;
}

void NuMusic::Album::Initialise(void) {
    i32 class_index;

    for (class_index = 0; class_index < 6; class_index++) {
        i32 track_index;
        Track *track;

        tracks[class_index] = NULL;
        track = tracks_source;
        for (track_index = 0; track_index < tracks_count; track_index++) {
            if (track->clazz == (1u << class_index)) {
                tracks[class_index] = track;
                break;
            }
            track++;
        }
    }
}

i32 NuMusic::PauseTrack(u32 class_mask) {
    Track *tracks_to_pause[6];
    i32 track_count;
    i32 track_index;
    i32 changed = 0;

    if (this == NULL || the_music_player == NULL || album == NULL) {
        return 0;
    }

    track_count = album->GetTracks(class_mask, tracks_to_pause);
    if (track_count == 0) {
        return 0;
    }
    for (track_index = 0; track_index < 6; track_index++) {
        if (tracks_to_pause[track_index] != NULL) {
            Voice *voice = FindVoiceByTrack(tracks_to_pause[track_index]);
            if (voice != NULL && (voice->status == 6 || voice->status == 7) && (voice->flags & 2) == 0) {
                NuSound3PauseStereoStream(voice->stream_index * 2);
                voice->flags |= 2;
                changed = 1;
            }
        }
    }
    return changed;
}

i32 NuMusic::ResumeTrack(u32 class_mask) {
    Track *tracks_to_resume[6];
    i32 track_count;
    i32 track_index;
    i32 changed = 0;

    if (this == NULL || the_music_player == NULL || album == NULL) {
        return 0;
    }

    track_count = album->GetTracks(class_mask, tracks_to_resume);
    if (track_count == 0) {
        return 0;
    }
    for (track_index = 0; track_index < 6; track_index++) {
        if (tracks_to_resume[track_index] != NULL) {
            Voice *voice = FindVoiceByTrack(tracks_to_resume[track_index]);
            if (voice != NULL && (voice->status == 6 || voice->status == 7) && (voice->flags & 2) != 0) {
                NuSound3ResumeStereoStream(voice->stream_index * 2);
                voice->flags &= ~2;
                changed = 1;
            }
        }
    }
    return changed;
}

bool NuMusic::SelectTrack(u32 class_mask, const char *name) {
    i32 handle;

    if (this == NULL || the_music_player == NULL) {
        return false;
    }
    handle = GetTrackHandle(class_mask, name);
    return SelectTrackByHandle(class_mask, handle);
}

bool NuMusic::SelectTrackByHandle(u32 class_mask, i32 handle) {
    i32 class_index;
    i32 current_album_index;
    i32 handle_album_index;
    i32 track_index;

    if (this == NULL || the_music_player == NULL || album == NULL) {
        return false;
    }
    class_index = ClassToIX(class_mask);
    if (class_index < 0) {
        return false;
    }
    if (handle == -1) {
        album->tracks[class_index] = NULL;
        return true;
    }

    handle_album_index = handle >> 11;
    track_index = handle & 0x7ff;
    current_album_index = (i32)(album - albums);
    if (handle_album_index == current_album_index) {
        if (track_index < 0 || track_index >= album->tracks_count) {
            return false;
        }
        album->tracks[class_index] = &album->tracks_source[track_index];
        return true;
    }
    if (handle_album_index == album_count) {
        if (track_index < 0 || track_index >= extra_track_count) {
            return false;
        }
        album->tracks[class_index] = &extra_tracks[track_index];
        return true;
    }
    return false;
}

i32 NuMusic::GetTrackHandle(u32 class_mask, const char *name) {
    Album *current_album;
    i32 track_index;

    if (this == NULL) {
        return -1;
    }
    if (the_music_player == NULL) {
        return -1;
    }

    current_album = album;
    if (name == NULL) {
        i32 class_index;
        i32 current_album_index;
        Track *track;

        if (current_album == NULL) {
            return -1;
        }
        class_index = ClassToIX(class_mask);
        if (class_index < 0 || current_album->tracks[class_index] == NULL) {
            return -1;
        }
        track = current_album->tracks[class_index];
        track_index = (i32)(track - current_album->tracks_source);
        current_album_index = (i32)(current_album - albums);
        return (current_album_index << 11) | track_index;
    }

    if (current_album != NULL) {
        for (track_index = 0; track_index < current_album->tracks_count; track_index++) {
            Track *track = &current_album->tracks_source[track_index];
            if (track->clazz == class_mask && track->ident != NULL && NuStrICmp(track->ident, name) == 0) {
                return ((i32)(current_album - albums) << 11) | track_index;
            }
        }
    }

    for (track_index = 0; track_index < extra_track_count; track_index++) {
        Track *track = &extra_tracks[track_index];
        if (track->clazz == class_mask && track->ident != NULL && NuStrICmp(track->ident, name) == 0) {
            return (album_count << 11) | track_index;
        }
    }
    return -1;
}

i32 NuMusic::GetStatus(u32 class_mask, i32 *per_class_status) {
    i32 result = 0;
    i32 voice_index;

    if (this == NULL || the_music_player == NULL) {
        return 0;
    }

    if (per_class_status != NULL) {
        i32 index;
        for (index = 0; index < 6; index++) {
            per_class_status[index] = 0;
        }
    }

    for (voice_index = 0; voice_index < 2; voice_index++) {
        Voice *voice = &voices[voice_index];
        Track *track = voice->tracks[voice->track_index];
        i32 voice_status = 0;
        i32 class_index;

        if (track == NULL || (track->clazz & class_mask) == 0) {
            continue;
        }

        if (voice->status == 1) {
            voice_status = 0;
        } else if (voice->status == 3) {
            voice_status = 2;
        } else if (voice->status == 7) {
            voice_status = 4;
        } else {
            voice_status = 1;
        }
        result |= voice_status;

        for (class_index = 0; class_index < 6; class_index++) {
            if (track->clazz == (1u << class_index)) {
                if (per_class_status != NULL) {
                    per_class_status[class_index] |= voice_status;
                }
                break;
            }
        }
    }
    return result;
}

void NuMusic::Debug(i32 unused, i32 y) {
    (void)unused;
    const char *class_names[] = {NuMusicDebugClassInvalid,   NuMusicDebugClassQuiet,   NuMusicDebugClassAction,
                                 NuMusicDebugClassSignature, NuMusicDebugClassOverlay, NuMusicDebugClassCutscene,
                                 NuMusicDebugClassNoMusic};
    struct StreamInfoPc {
        u8 unknown_0000[12];
        f32 playback_position;
    } stream_info;
    STATIC_ASSERT(sizeof(StreamInfoPc) == 0x10);
    STATIC_ASSERT(offsetof(StreamInfoPc, playback_position) == 0x0c);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
    NuQFntSetScale(system_qfont, 0.8f, 0.8f);
    NuQFntSetColour(system_qfont, NuMusicDebugColour);

    for (i32 i = 0; i < 2; i++) {
        Voice *voice = &voices[i];
        NuSound3GetStreamInfo(voice->stream_index * 2, &stream_info);
        NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugVoiceFormat, i);
        y += (i32)NuQFntHeight(system_qfont);
        if (voice->tracks[voice->track_index] == NULL) {
            NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugTrackNone);
            y += (i32)NuQFntHeight(system_qfont);
            NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugClassNone);
        } else {
            NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugTrackFormat,
                          voice->tracks[voice->track_index]->filenames[voice->track_sub[voice->track_index]]);
            y += (i32)NuQFntHeight(system_qfont);
            NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugClassFormat,
                          class_names[ClassToIX(voice->tracks[voice->track_index]->clazz) + 1]);
        }
        y += (i32)NuQFntHeight(system_qfont);
        NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugStatusFormat, NuMusicVoiceStatusText[voice->status]);
        y += (i32)NuQFntHeight(system_qfont);
        NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugVolumeFormat, voice->gain, voice->last_volume);
        y += (i32)NuQFntHeight(system_qfont);
        NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugFadeFormat, voice->fade_rate);
        y += (i32)NuQFntHeight(system_qfont);
        NuQFntPrintEx(system_qfont, 4800, y, 0x10, NuMusicDebugPositionFormat, stream_info.playback_position);
        y += (i32)NuQFntHeight(system_qfont);
    }
    NuQFntPopPrintMode();
}

#include <stdarg.h>

struct PC_STEREO_PLAY_PACKET {
    i32 option11;
    i32 reserved;
    i32 stream_index;
    i32 first_sample;
    i32 last_sample;
    i32 volume_left;
    i32 volume_right;
    i32 option9_mode;
    f32 start_offset;
};
STATIC_ASSERT(sizeof(PC_STEREO_PLAY_PACKET) == 36);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, stream_index) == 8);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, first_sample) == 12);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, last_sample) == 16);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, option9_mode) == 28);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, start_offset) == 32);
extern "C" void PCStereoStartPacket(PC_STEREO_PLAY_PACKET *packet);
extern "C" f32 PCMusicDuckOverride;

extern "C" __declspec(noinline) void PCStereoUnknownToken(void) {
}

extern "C" i32 NuSound3PlayStereoV(i32 token, ...) {
    va_list arguments;
    PC_STEREO_PLAY_PACKET packet;
    if (NuSound3Initialized == 0)
        return 0;
    packet.reserved = -1;
    packet.stream_index = 0;
    packet.first_sample = -1;
    packet.last_sample = -1;
    packet.volume_left = 0x3fff;
    packet.volume_right = 0x3fff;
    packet.option9_mode = -1;
    packet.start_offset = 0.0f;
    va_start(arguments, token);
    while (token != 1) {
        switch (token) {
            case 0:
                break;
            case 2:
                packet.stream_index = va_arg(arguments, i32);
                break;
            case 3:
                packet.first_sample = va_arg(arguments, i32);
                packet.last_sample = packet.first_sample + 1;
                break;
            case 4:
                packet.first_sample = va_arg(arguments, i32);
                break;
            case 5:
                packet.last_sample = va_arg(arguments, i32);
                break;
            case 6:
                packet.volume_left = packet.volume_right = va_arg(arguments, i32);
                break;
            case 7:
                packet.volume_left = va_arg(arguments, i32);
                break;
            case 8:
                packet.volume_right = va_arg(arguments, i32);
                break;
            case 9:
                packet.option9_mode = va_arg(arguments, i32);
                break;
            case 10:
                packet.start_offset = (f32)va_arg(arguments, f64);
                break;
            case 11:
                packet.option11 = va_arg(arguments, i32);
                break;
            case 12:
                packet.option9_mode = 0;
                break;
            default:
                PCStereoUnknownToken();
                break;
        }
        token = va_arg(arguments, i32);
    }
    if (packet.last_sample == -1)
        packet.last_sample = packet.first_sample + 1;
    va_end(arguments);
    PCStereoStartPacket(&packet);
    return 1;
}

i32 NuMusic::Voice::Unload(void) {
    i32 index = track_index;
    if (tracks[index] == NULL || index == 0)
        return 0;
    if (tracks[index]->clazz == 8) {
        --track_index;
        if (tracks[track_index] != NULL) {
            gain = 0.0f;
            NuSound3StopStereoStream(stream_index * 2);
            Track *track = tracks[track_index];
            i32 result = NuSound3PlayStereoV(2, stream_index, 3, track->file_indexes[track_sub[track_index]], 9, 0, 6,
                                             0, 10, (f64)track->start_offset, 1);
            last_volume = 0.0f;
            track->ManageEntryTime();
            if (result != 0 && status != 4) {
                status = 4;
                play_time = 0.0f;
            }
        } else {
            if (status != 0) {
                status = 0;
                play_time = 0.0f;
            }
            tracks[track_index] = NULL;
        }
    }
    return 1;
}

void NuMusic::Process(f32 delta) {
    if (this == NULL || the_music_player == NULL)
        return;
    if (fader_current != fader_target) {
        if (fader_target > fader_current) {
            fader_current = delta * fader_rate + fader_current;
            if (fader_target <= fader_current) {
                fader_rate = 0.0f;
                fader_current = fader_target;
            }
        } else {
            fader_current = fader_current - delta * fader_rate;
            if (fader_current <= fader_target) {
                fader_rate = 0.0f;
                fader_current = fader_target;
            }
        }
    }
    duck_gain = 1.0f;
    duck_rate = 1.0f;
    Track *track = voices[0].tracks[voices[0].track_index];
    if (track != NULL && voices[0].status == 7) {
        duck_gain = 1.0f < track->duck_volume ? 1.0f : track->duck_volume;
        f32 rate = 1.0f / track->duck_fade;
        duck_rate = rate > 1.0f ? rate : 1.0f;
    }
    track = voices[1].tracks[voices[1].track_index];
    if (track != NULL && voices[1].status == 7) {
        duck_gain = duck_gain < track->duck_volume ? duck_gain : track->duck_volume;
        f32 rate = 1.0f / track->duck_fade;
        duck_rate = rate > duck_rate ? rate : duck_rate;
    }
    if (duck_current > duck_gain) {
        f64 current = duck_current - (f64)duck_rate * delta;
        duck_current = duck_gain > current ? duck_gain : (f32)current;
    } else if (duck_current < duck_gain) {
        f64 current = (f64)duck_rate * delta + duck_current;
        duck_current = duck_gain < current ? duck_gain : (f32)current;
    }
    for (i32 index = 0; index < 2; ++index) {
        Voice *voice = &voices[index];
        Track *current_track = voice->tracks[voice->track_index];
        if (delta != 0.0f) {
            i32 stream_status = NuSound3GetStereoStreamStatus(voice->stream_index * 2);
            if (voice->status == 7 && voice->fade_rate != 0.0f) {
                f32 gain = voice->fade_rate * delta + voice->gain;
                voice->gain = gain > 1.0f ? 1.0f : gain;
                if (voice->gain < 0.0f)
                    voice->gain = 0.0f;
                if (voice->gain == 0.0f) {
                    voice->fade_rate = 0.0f;
                    if ((voice->flags & 1) != 0) {
                        NuSound3StopStereoStream(voice->stream_index * 2);
                        if (voice->status != 0) {
                            voice->status = 0;
                            voice->play_time = 0.0f;
                        }
                        voice->flags &= ~1;
                    } else if (current_track->entry_count != 0) {
                        i32 result =
                            NuSound3PlayStereoV(2, voice->stream_index, 3,
                                                current_track->file_indexes[voice->track_sub[voice->track_index]], 9, 0,
                                                6, 0, 10, (f64)current_track->start_offset, 1);
                        voice->last_volume = 0.0f;
                        current_track->ManageEntryTime();
                        if (result != 0 && voice->status != 4) {
                            voice->status = 4;
                            voice->play_time = 0.0f;
                        }
                    } else {
                        NuSound3ResumeStereoStream(voice->stream_index * 2);
                        if (voice->status != 3) {
                            voice->status = 3;
                            voice->play_time = 0.0f;
                        }
                    }
                } else if (voice->gain == 1.0f) {
                    voice->fade_rate = 0.0f;
                }
            }
            switch (voice->status) {
                case 0:
                    if (stream_status == 0) {
                        voice->play_time = 0.0f;
                        voice->status = 1;
                        voice->fade_rate = 0.0f;
                        voice->track_index = 0;
                        voice->gain = 0.0f;
                        voice->tracks[0] = NULL;
                    }
                    break;
                case 4:
                    if (stream_status == 2 || (stream_status == 0 && voice->play_time > 0.25f)) {
                        voice->play_time = 0.0f;
                        voice->status = 5;
                    }
                    break;
                case 5:
                    if (stream_status != 2) {
                        voice->play_time = 0.0f;
                        voice->status = 3;
                    }
                    break;
                case 6:
                    if (stream_status == 1) {
                        voice->play_time = 0.0f;
                        voice->status = 7;
                    }
                    break;
                case 7:
                    if (stream_status == 0) {
                        voice->play_time = 0.0f;
                        voice->status = 3;
                        if (voice->tracks[voice->track_index] != NULL) {
                            NuSound3StopStereoStream(voice->stream_index * 2);
                            if (voice->tracks[voice->track_index]->clazz == 8)
                                voice->Unload();
                        }
                    }
                    break;
            }
            f32 duck = duck_current;
            f32 attenuation = 1.0f;
            f32 class_volume = 1.0f;
            if (current_track != NULL) {
                if ((current_track->flags & 1) != 0 || current_track->duck_volume != 1.0f)
                    duck = 1.0f;
                class_volume = class_volumes[ClassToIX(current_track->clazz)];
                attenuation = current_track->attenuation;
            }
            f32 volume =
                master_volume * duck * voice->gain * class_volume * fader_current * attenuation * global_attenuation;
            voice->volume = volume;
            if (volume != voice->last_volume) {
                NuSound3SetStreamVolume(voice->stream_index * 2, (i32)((f64)volume * 16383.0));
                voice->last_volume = voice->volume;
            }
        }
        voice->play_time += delta;
    }
    if (PCMusicDuckOverride != 1.0f)
        duck_current = PCMusicDuckOverride;
}
