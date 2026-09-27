#include "saga.h"

extern f32 FRAMETIME;
extern f32 __cdecl NuFmod(f32 value, f32 divisor);

void ResetFrameCounters(void) {
    MainFrameCounters.warmup_frame = 0;
    MainFrameCounters.every_frame = 0xff;
    MainFrameCounters.alternate_frame = 0xff;
    MainFrameCounters.third_frame = 0xff;
    MainFrameCounters.fourth_frame = 0xff;
}

void UpdateFrameCounters(void) {
    if (do_multiframe_update == 0) {
        ResetFrameCounters();
        return;
    }

    if ((i8)MainFrameCounters.warmup_frame <= 0) {
        ++MainFrameCounters.warmup_frame;
        return;
    }

    MainFrameCounters.every_frame = 1;
    MainFrameCounters.alternate_frame = (MainFrameCounters.alternate_frame + 1) & 1;
    if (++MainFrameCounters.third_frame == 3) {
        MainFrameCounters.third_frame = 0;
    }
    MainFrameCounters.fourth_frame = (MainFrameCounters.fourth_frame + 1) & 3;
}

void ResetTimer(TIMER *timer, f32 reset_time) {
    timer->last_time_elapsed = reset_time;
    timer->time_elapsed = reset_time;
    timer->time_elapsed_mod_seconds = NuFmod(reset_time, 1.0f);
    timer->update_count = 0;
}

void UpdateTimer(TIMER *timer) {
    timer->last_time_elapsed = timer->time_elapsed;
    timer->time_elapsed += FRAMETIME;
    timer->time_elapsed_mod_seconds += FRAMETIME;

    while (timer->time_elapsed_mod_seconds >= 1.0f) {
        timer->time_elapsed_mod_seconds -= 1.0f;
    }

    if (timer->update_count == -1)
        return;

    timer->update_count++;
}
