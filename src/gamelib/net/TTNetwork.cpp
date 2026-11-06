#include "saga.h"

/* TTNetwork object (0x60dee0-0x60ffcb): network statistics and the TTNetwork
 * singleton. Only the statistics helpers are ported so far. */

STATIC_ASSERT(offsetof(NetStats, history_index) == 0x18);
STATIC_ASSERT(offsetof(NetStats, last_time) == 0x1c);
STATIC_ASSERT(offsetof(NetStats, peak) == 0x20);
STATIC_ASSERT(offsetof(NetStats, previous) == 0x30);

NetStatCounters::NetStatCounters() {
    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
}

NetSmallStats::NetSmallStats(const char *stats_name) : name(stats_name) {
}

void NetSmallStats::Update() {
}

NetStats::NetStats(const char *stats_name) : NetSmallStats(stats_name), history_index(0), last_time(0) {
}

void NetStatCounters::Clear() {
    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
}

NetStatCounters &NetStatCounters::operator+=(const NetStatCounters &other) {
    values[0] += other.values[0];
    values[1] += other.values[1];
    values[2] += other.values[2];
    values[3] += other.values[3];
    return *this;
}

NetStatCounters &NetStatCounters::operator-=(const NetStatCounters &other) {
    values[0] -= other.values[0];
    values[1] -= other.values[1];
    values[2] -= other.values[2];
    values[3] -= other.values[3];
    return *this;
}

NetStatCounters &NetStatCounters::KeepMax(const NetStatCounters &other) {
    values[0] = values[0] > other.values[0] ? values[0] : other.values[0];
    values[1] = values[1] > other.values[1] ? values[1] : other.values[1];
    values[2] = values[2] > other.values[2] ? values[2] : other.values[2];
    values[3] = values[3] > other.values[3] ? values[3] : other.values[3];
    return *this;
}

void NetStats::Update() {
    i32 index;
    if (NetTimeGetMs() - last_time > 1000) {
        index = history_index + 1;
        if (index >= 30)
            index = 0;
        history[index] = totals;
        history[index] -= previous;
        peak.KeepMax(history[index]);
        previous = totals;
        history_index = index;
        last_time = NetTimeGetMs();
    }
}
