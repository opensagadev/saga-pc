#include "saga.h"

extern "C" {
#pragma bss_seg(".bss$FadeSys")
    FadeSystem FadeSys;
#pragma bss_seg()
}

class Fade : public FadeBase {
  public:
    void Init(FADEINFO_s *state);
    void InitFade();
    void UpdateFade();
    void DrawFade();
    FADETYPE_VALUE GetFadeType() const;
};

class FadeWipe : public FadeBase {
  public:
    void Init(FADEINFO_s *state);
    void InitFade();
    void UpdateFade();
    void DrawFade();
    FADETYPE_VALUE GetFadeType() const;
};

class FadeStillWipe : public FadeBase {
  public:
    void Init(FADEINFO_s *state);
    void InitFade();
    void UpdateFade();
    void DrawFade();
    FADETYPE_VALUE GetFadeType() const;
};

class FadeStill : public FadeBase {
  public:
    void Init(FADEINFO_s *state);
    void InitFade();
    void UpdateFade();
    void DrawFade();
    FADETYPE_VALUE GetFadeType() const;
};

extern "C" {
    void __cdecl NeedScreenGrab(i32 needed);
    void __cdecl DrawStillScreen(i32 clear);
    void __cdecl DrawPauseScreenWipe(void);
    void __cdecl DrawFadeScreenWipe(void);

    Fade fade;
    FadeWipe fadeWipe;
    FadeStillWipe fadeStillWipe;
    FadeStill fadeStill;
    FADEINFO_s *pFadeInfo = 0;
    NUMTL *FadeMtl = 0;
    NUMTL *ScreenFadeMtl = 0;
    i32 wait_till_next_frame = 0;
    i32 FRAMES_TO_WAIT = 1;
}

FadeBase::~FadeBase() {
}

void Fade::Init(FADEINFO_s *state) {
    info = state;
}

void Fade::InitFade() {
    if ((info->stage & 1) != 0) {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
    } else {
        info->fade = 0.0f;
        info->rate = 2.0f;
    }
}

void Fade::UpdateFade() {
}

void Fade::DrawFade() {
    i32 index;

    if (info->fade != 0.0f && FadeMtl != NULL) {
        index = (i32)(((f64)info->fade - 0.0) * 16384.0 + 49152.0) >> 1 & 0x7fff;
        NuRndrRect2di(0, 0, 0x2800, 0xe00, (i32)(NuTrigTable[index] * 128.0) << 24, ScreenFadeMtl);
    }
}

FADETYPE_VALUE Fade::GetFadeType() const {
    return FADE_TYPE_SCREEN;
}

void FadeWipe::Init(FADEINFO_s *state) {
    info = state;
}

void FadeWipe::InitFade() {
    u32 old_direction = info->direction;

    if ((info->stage & 1) != 0) {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
    } else {
        info->fade = 0.0f;
        info->rate = 2.0f;
    }
    do {
        info->direction = 1u << (qrand() / 0x4000);
    } while (info->direction == old_direction);
}

void FadeWipe::UpdateFade() {
}

void FadeWipe::DrawFade() {
    DrawFadeScreenWipe();
}

FADETYPE_VALUE FadeWipe::GetFadeType() const {
    return FADE_TYPE_WIPE;
}

void FadeStillWipe::Init(FADEINFO_s *state) {
    info = state;
}

void FadeStillWipe::InitFade() {
    u32 old_direction = info->direction;

    if ((info->stage & 1) != 0) {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
        if (CUTSTOPGAME == 0 && WORLD->current_level != NULL && (WORLD->current_level->flags & 0xe0) == 0 &&
            WORLD->current_level != TITLES_LDATA) {
            PlaySfx("WipeScreen", NULL);
        }
    } else {
        pause_rndr_on = 0;
        info->fade = 1.0f;
        info->rate = 2.0f;
        if (info->field_28 != 0) {
            NeedScreenGrab(1);
        } else {
            info->field_28 = 1;
        }
        wait_till_next_frame = FRAMES_TO_WAIT;
    }
    do {
        info->direction = 1u << (qrand() / 0x4000);
    } while (info->direction == old_direction);
}

void FadeStillWipe::UpdateFade() {
    if (wait_till_next_frame > 0) {
        wait_till_next_frame--;
    }
    if ((info->stage & 2) != 0 && wait_till_next_frame == 0) {
        pause_rndr_on = 1;
    }
}

void FadeStillWipe::DrawFade() {
    if (wait_till_next_frame != 0) {
        return;
    }
    if ((info->stage & 2) != 0) {
        DrawStillScreen(1);
    } else {
        DrawPauseScreenWipe();
    }
}

FADETYPE_VALUE FadeStillWipe::GetFadeType() const {
    return FADE_TYPE_STILL_WIPE;
}

void FadeStill::Init(FADEINFO_s *state) {
    info = state;
}

void FadeStill::InitFade() {
    u32 old_direction = info->direction;

    if ((info->stage & 1) != 0) {
        info->fade = 0.0f;
        info->rate = 0.0f;
    } else {
        info->fade = 1.0f;
        info->rate = 2.0f;
        NeedScreenGrab(1);
        wait_till_next_frame = FRAMES_TO_WAIT;
    }
    do {
        info->direction = 1u << (qrand() / 0x4000);
    } while (info->direction == old_direction);
}

void FadeStill::UpdateFade() {
    if ((info->stage & 2) != 0) {
        pause_rndr_on = 1;
    }
}

void FadeStill::DrawFade() {
    if (wait_till_next_frame != 0) {
        wait_till_next_frame--;
        return;
    }
    if ((info->stage & 2) != 0) {
        DrawStillScreen(1);
    } else {
        DrawPauseScreenWipe();
    }
}

FADETYPE_VALUE FadeStill::GetFadeType() const {
    return FADE_TYPE_STILL;
}

i32 FadeSystem::AddFade(FadeBase *fade_object) {
    FADETYPE_VALUE type;

    if (fade_object == NULL) {
        return 0;
    }
    type = fade_object->GetFadeType();
    fade_object->Init(this);
    fades[type] = fade_object;
    return 1;
}

extern i32 fade_draw_suppressed;
extern f32 FRAMETIME;

void FadeSystem::Init() {
    fades[0] = 0;
    fades[1] = 0;
    fades[2] = 0;
    fades[3] = 0;
    field_28 = 1;
    pending_type = FADE_TYPE_NONE;
}

void FadeSystem::Update() {
    f32 old_fade = fade;
    FADETYPE_VALUE type = pending_type;
    if (type == FADE_TYPE_NONE)
        return;

    fade = fade + rate * FRAMETIME;
    if (fade > 1.0f)
        fade = 1.0f;
    else if (fade < 0.0f)
        fade = 0.0f;

    if (old_fade < 1.0f && fade == 1.0f)
        busy = 1;
    else if (busy != 0)
        --busy;

    if (fades[type] != 0)
        fades[type]->UpdateFade();
    if (fade == 0.0f || fade == 1.0f)
        rate = 0.0f;
    if (fade == 0.0f)
        pending_type = FADE_TYPE_NONE;
}

void FadeSystem::Draw() {
    if (pending_type != FADE_TYPE_NONE && fade_draw_suppressed == 0 && fades[pending_type] != 0)
        fades[pending_type]->DrawFade();
}

void FadeSystem::SetStage(char new_stage) {
    stage = new_stage;
    busy = 0;
    if (pending_type != FADE_TYPE_NONE && fades[pending_type] != 0)
        fades[pending_type]->InitFade();
}

i32 FadeSystem::SetFade(FADETYPE const &type, u32 frames) {
    FADETYPE_VALUE fade_type = type.type;
    if (fade_type != FADE_TYPE_NONE && fades[fade_type] != 0) {
        pending_type = fade_type;
        direction = frames;
        return 1;
    }
    pending_type = FADE_TYPE_NONE;
    return 0;
}
