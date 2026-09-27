#include "saga.h"

enum FADETYPE_VALUE {
    FADE_TYPE_NONE = -1,
    FADE_TYPE_SCREEN = 0,
    FADE_TYPE_WIPE = 1,
    FADE_TYPE_STILL_WIPE = 2,
    FADE_TYPE_STILL = 3,
};

struct FADEINFO_s;

struct FadeBase {
    virtual ~FadeBase();
    virtual void Init(FADEINFO_s *info) = 0;
    virtual void InitFade() = 0;
    virtual void UpdateFade() = 0;
    virtual void DrawFade() = 0;
    virtual FADETYPE_VALUE GetFadeType() const = 0;
};

struct FADEINFO_s {
    u32 direction;
    f32 fade;
    f32 rate;
    i32 busy;
    i32 stage;
    FadeBase *fades[4];
    FADETYPE_VALUE pending_type;
    i32 field_28;
};

STATIC_ASSERT(sizeof(FADEINFO_s) == 0x2c);
STATIC_ASSERT(offsetof(FADEINFO_s, direction) == 0x00);
STATIC_ASSERT(offsetof(FADEINFO_s, fade) == 0x04);
STATIC_ASSERT(offsetof(FADEINFO_s, rate) == 0x08);
STATIC_ASSERT(offsetof(FADEINFO_s, busy) == 0x0c);
STATIC_ASSERT(offsetof(FADEINFO_s, stage) == 0x10);
STATIC_ASSERT(offsetof(FADEINFO_s, fades) == 0x14);
STATIC_ASSERT(offsetof(FADEINFO_s, pending_type) == 0x24);
STATIC_ASSERT(offsetof(FADEINFO_s, field_28) == 0x28);

struct FADETYPE {
    FADETYPE_VALUE type;
};

class FadeSystem : public FADEINFO_s {
  public:
    void Init();
    void Update();
    void Draw();
    void SetStage(char stage);
    i32 SetFade(FADETYPE const &type, u32 frames);
};

STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);

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
