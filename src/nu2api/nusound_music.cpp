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
    i32 index = NuMusic::ClassToIX(class_mask);
    if (index == -1) {
        return NULL;
    }
    return tracks[index];
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
