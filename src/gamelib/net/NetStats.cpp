#include "saga.h"

/* Network statistics overlays (0x60f3a0-0x60ffcb): NetSmallStats::Draw and
 * NetStats::Draw (Android gamelib/util/Stats.cpp). */

typedef struct NET_PRIM_VERTEX_PC {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} NET_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(NET_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(NET_PRIM_VERTEX_PC, colour) == 0xc);

static void NetPrimSetColour(i32 colour) {
    if (NuPrimNeedsOverbrightening != 0)
        ((NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour =
            ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00u);
    else
        ((NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour =
            (((colour & 0xff) >> 1) << 16) + ((((colour >> 8) & 0xff) >> 1) << 8) + (((colour >> 16) & 0xff) >> 1) +
            (colour & 0xff000000);
}

static void NetPrimPosition(f32 x, f32 y, f32 z) {
    ((NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = x;
    ((NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = y;
    ((NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z;
    NuPrimStreamBufferPointer->void_ptr = (NET_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr + 1;
    ++NuPrimVertexCount;
}

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
    if (UtilGetFrameStartTime() - last_time > 1000) {
        index = history_index + 1;
        if (index >= 30)
            index = 0;
        history[index] = totals;
        history[index] -= previous;
        peak.KeepMax(history[index]);
        previous = totals;
        history_index = index;
        last_time = UtilGetFrameStartTime();
    }
}

void NetSmallStats::Draw(f32 x, f32 y, f32 width, f32 height, eInfo) {
    f32 half_width = width * 0.5;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    NuPrim2DBegin(0xf, 5, NULL);

    NetPrimSetColour(0x80000080);
    NetPrimPosition(x, y, 0.0f);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(x, y - height, 0.0f);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(x + half_width, y - height, 0.0f);

    NuPrim2DEnd();
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);

    NuQFntPushCoordinateSystem(3);
    NuQFntPushPrintMode(2);
    VUFNT *font = system_qfont;
    f32 font_height = NuQFntHeight(font);
    f32 right = x + half_width;
    NuQFntSetColour(font, 0x80808080);
    NuQFntSetScale(font, 0.75f, 0.75f);

    f32 text_y = y + font_height;
    NuQFntMove(font, right, text_y, 0.0f);
    NuQFntPrintU(font, (char *)name);

    char text[128];
    text_y += font_height;
    NuQFntMove(font, right, text_y, 0.0f);
    NuSPrintf(text, "total b %d : %d", totals.values[0], totals.values[1]);
    NuQFntPrintU(font, text);

    text_y += font_height;
    NuQFntMove(font, right, text_y, 0.0f);
    NuSPrintf(text, "total p%d : %d", totals.values[2], totals.values[3]);
    NuQFntPrintU(font, text);

    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

void NetStats::Draw(f32 x, f32 y, f32 width, f32 height, eInfo info) {
    NetStatCounters visible;
    for (i32 i = 0; i < 30; ++i) {
        visible.KeepMax(history[i]);
    }

    f32 half_width = width * 0.5;
    f32 x_step = half_width / 30.0;
    f32 byte_maximum = MAX((f32)MAX(visible.values[0], visible.values[1]), 4352.0f);
    f32 byte_scale = height / byte_maximum;
    f32 packet_scale = height / (f32)MAX(visible.values[2], visible.values[3]);

    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    NuPrim2DBegin(0xf, 5, NULL);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(x, y, 0.0f);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(x, y - height, 0.0f);
    NetPrimSetColour(0x80000080);
    f32 right = x + half_width;
    NetPrimPosition(right, y - height, 0.0f);
    NuPrim2DEnd();

    NuPrim2DBegin(0xf, 5, NULL);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(x, y - height + byte_scale * 4352.0, 0.0f);
    NetPrimSetColour(0x80000080);
    NetPrimPosition(right, y - height + byte_scale * 4352.0, 0.0f);
    NuPrim2DEnd();

    f32 sample_y;
    NuPrim2DBegin(0xf, 5, NULL);
    i32 sample = history_index + 1;
    for (i32 i = 0; i < 30; ++i) {
        if (sample >= 30) {
            sample = 0;
        }
        f32 sample_x = x + (f32)i * x_step;
        switch (info) {
            case INFO_BYTES:
                sample_y = y - height + (f32)history[sample].values[0] * byte_scale;
                break;
            case INFO_PACKETS:
                sample_y = y - height + (f32)history[sample].values[2] * packet_scale;
                break;
        }
        NetPrimSetColour(0x80008000);
        NetPrimPosition(sample_x, sample_y, 0.0f);
        ++sample;
    }
    NuPrim2DEnd();

    NuPrim2DBegin(0xf, 5, NULL);
    sample = history_index + 1;
    for (i32 i = 0; i < 30; ++i) {
        if (sample >= 30) {
            sample = 0;
        }
        f32 sample_x = x + (f32)i * x_step;
        switch (info) {
            case INFO_BYTES:
                sample_y = y - height + (f32)history[sample].values[1] * byte_scale;
                break;
            case INFO_PACKETS:
                sample_y = y - height + (f32)history[sample].values[3] * packet_scale;
                break;
        }
        NetPrimSetColour(0x80008080);
        NetPrimPosition(sample_x, sample_y, 0.0f);
        ++sample;
    }
    NuPrim2DEnd();

    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);

    NuQFntPushCoordinateSystem(3);
    NuQFntPushPrintMode(2);
    VUFNT *font = system_qfont;
    f32 font_height = NuQFntHeight(font);
    NuQFntSetColour(font, 0x80808080);
    NuQFntSetScale(font, 0.75f, 0.75f);

    y += font_height;
    NuQFntMove(font, right, y, 0.0f);
    NuQFntPrintU(font, (char *)name);

    char text[128];
    y += font_height;
    NuQFntMove(font, right, y, 0.0f);
    NuSPrintf(text, "bps %d : %d", history[history_index].values[0], history[history_index].values[1]);
    NuQFntPrintU(font, text);

    y += font_height;
    NuQFntMove(font, right, y, 0.0f);
    NuSPrintf(text, "pps %d : %d", history[history_index].values[2], history[history_index].values[3]);
    NuQFntPrintU(font, text);

    y += font_height;
    NuQFntMove(font, right, y, 0.0f);
    NuSPrintf(text, "max bps %d : %d", peak.values[0], peak.values[1]);
    NuQFntPrintU(font, text);

    y += font_height;
    NuQFntMove(font, right, y, 0.0f);
    NuSPrintf(text, "max pps %d : %d", peak.values[2], peak.values[3]);
    NuQFntPrintU(font, text);

    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}
