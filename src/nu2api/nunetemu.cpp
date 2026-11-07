#include "saga.h"

/* Network condition emulator (retail nunetemu.cpp, 0x610a50-0x611889). */

STATIC_ASSERT(sizeof(NuNetEmu::EmuPacket) == 0x24);
STATIC_ASSERT(offsetof(NuNetEmu::EmuPacket, address) == 0x8);
STATIC_ASSERT(offsetof(NuNetEmu::EmuPacket, payload) == 0xc);
STATIC_ASSERT(offsetof(NuNetEmu::EmuPacket, flush_time) == 0x18);
STATIC_ASSERT(offsetof(NuNetEmu::EmuPacket, creation_time) == 0x20);
STATIC_ASSERT(sizeof(NuNetEmu::PackStats) == 0x238);
STATIC_ASSERT(offsetof(NuNetEmu, enabled) == 0x10);
STATIC_ASSERT(offsetof(NuNetEmu, loss_chance) == 0x34);
STATIC_ASSERT(offsetof(NuNetEmu, packed_buffer) == 0x40);
STATIC_ASSERT(offsetof(NuNetEmu, unpacked_buffer) == 0xbf8);
STATIC_ASSERT(offsetof(NuNetEmu, read_offset) == 0x17b0);
STATIC_ASSERT(offsetof(NuNetEmu, max_packets) == 0x17c8);
STATIC_ASSERT(offsetof(NuNetEmu, raw_stats) == 0x17cc);
STATIC_ASSERT(offsetof(NuNetEmu, packet_stats) == 0x19ec);
STATIC_ASSERT(sizeof(NuNetEmu) == 0x1c24);

NuNetEmu theNuNetEmu;

NuNetEmu::PackStats::PackStats(const char *name) : NetStats(name) {
}

void *NuNetEmu::EmuPacket::operator new(u32 size) {
    return theMemoryManager.AllocPool(size, 1);
}

void NuNetEmu::EmuPacket::operator delete(void *memory) {
    theMemoryManager.FreePool(memory, sizeof(EmuPacket));
}

i32 NuNetEmu::EmuPacket::GetFreeBytes() {
    return 0xbb8 - payload_size;
}

NuNetEmu::EmuPacket::~EmuPacket() {
    theMemoryManager.FreePool(payload, 0xbb8);
}

void NuNetEmu::EmuPacket::AddPayload(void *data, i32 size) {
    payload[payload_size] = (u8)size;
    payload[payload_size + 1] = (u8)(size >> 8);
    PCByteCopy_candidate(payload + payload_size + 2, data, size);
    payload_count++;
    payload_size += size + 2;
}

void NuNetEmu::SetConditions(eConditions new_conditions) {
    conditions = new_conditions;
    switch (new_conditions) {
        case CONDITIONS_NORMAL:
            loss_mode = 0;
            min_latency = 0;
            max_latency = 0;
            break;
        case CONDITIONS_LIGHT:
            loss_mode = 1;
            loss_chance = 0.005f;
            min_latency = 20;
            max_latency = 40;
            break;
        case CONDITIONS_HEAVY:
            loss_mode = 1;
            loss_chance = 0.02f;
            min_latency = 50;
            max_latency = 70;
            break;
        case CONDITIONS_BURSTY:
            loss_mode = 2;
            loss_chance = 0.01f;
            burst_min = 5;
            burst_max = 15;
            min_latency = 100;
            max_latency = 120;
            break;
    }
}

void NuNetEmu::PackStats::Draw(f32 x, f32 y, f32 width, f32 height, eInfo info) {
    NetStats::Draw(x, y, width, height, info);

    NuQFntPushCoordinateSystem(3);
    NuQFntPushPrintMode(2);
    VUFNT *font = system_qfont;
    f32 font_height = NuQFntHeight(font);
    NuQFntSetColour(font, 0x80808080);
    NuQFntSetScale(font, 0.75f, 0.75f);

    f32 text_y = y + font_height;
    text_y += font_height;
    text_y += font_height;
    text_y += font_height;
    text_y += font_height;
    text_y += font_height;
    char text[128];
    NuQFntMove(font, x, text_y, 0.0f);
    NuSPrintf(text, "Pack Ratio %.3f%%", pack_ratio);
    NuQFntPrintU(font, text);

    text_y += font_height;
    f32 send_time = 0.0f;
    f32 receive_time = 0.0f;
    if (totals.values[2] > 0) {
        send_time = (f32)pack_time[0] / (f32)totals.values[2];
    }
    if (totals.values[3] > 0) {
        receive_time = (f32)pack_time[1] / (f32)totals.values[3];
    }
    NuQFntMove(font, x, text_y, 0.0f);
    NuSPrintf(text, "Pack Time %.2f : %.2f", send_time, receive_time);
    NuQFntPrintU(font, text);

    text_y += font_height;
    NuQFntMove(font, x, text_y, 0.0f);
    NuSPrintf(text, "Ave Packet %.1f", average_packet_size);
    NuQFntPrintU(font, text);

    text_y += font_height;
    NuQFntMove(font, x, text_y, 0.0f);
    NuSPrintf(text, "Split Packets %d", split_packets);
    NuQFntPrintU(font, text);

    text_y += font_height;
    NuQFntMove(font, x, text_y, 0.0f);
    NuSPrintf(text, "Held Packets %d", held_packets);
    NuQFntPrintU(font, text);

    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

NuNetEmu::EmuPacket *NuNetEmu::FindPacket(u32 *, i32 size) {
    EmuPacket *packet = first_packet;
    u32 now = UtilGetFrameStartTime();
    size += 2;
    while (packet != NULL) {
        if (packet->GetFreeBytes() >= size && (packet->flush_time >= packet->send_time || packet->flush_time >= now)) {
            return packet;
        }
        packet = packet->next;
    }
    return NULL;
}

NuNetEmu::EmuPacket::EmuPacket(u32 *net_address) {
    payload = (u8 *)theMemoryManager.AllocPool(0xbb8, 1);
    payload_size = 0;
    payload_count = 0;
    address = *net_address;
}

NuNetEmu::NuNetEmu()
    : first_packet(NULL), last_packet(NULL), packet_count(0), raw_stats("EmuRaw"), packet_stats("EmuPack") {
    enabled = 0;
    min_latency = 0;
    max_latency = 0;
    conditions = 0;
    loss_mode = 0;
    burst_min = 0;
    burst_max = 0;
    burst_remaining = 0;
    loss_chance = 0.0f;
    flush_size = 0x320;
    flush_delay = 0x32;
    next_send_time = 0;
    bandwidth_low = 0x1800;
    bandwidth_high = 0x2800;
    max_packets = 0x80;
    read_offset = 0;
    unpacked_size = 0;
    packed_size = 0;
    SetConditions(CONDITIONS_NORMAL);
    field_00 = 0x200;
}

void NuNetEmu::Update() {
    u32 now = UtilGetFrameStartTime();
    if (now > next_send_time) {
        u32 sent = 0;
        u32 bandwidth = packet_count > 2 ? bandwidth_high : bandwidth_low;
        u32 budget = bandwidth / 30;
        EmuPacket *packet = first_packet;
        while (packet != NULL && sent < budget) {
            if (now >= packet->send_time && (now >= packet->flush_time || packet->payload_size >= flush_size)) {
                EmuPacket *next = packet->next;
                raw_stats.totals.values[0] += packet->payload_size;
                raw_stats.totals.values[2]++;
                sent += SplitSendPacket(packet);

                if (packet->next != NULL) {
                    packet->next->previous = packet->previous;
                } else {
                    last_packet = packet->previous;
                }
                if (packet->previous != NULL) {
                    packet->previous->next = packet->next;
                } else {
                    first_packet = packet->next;
                }
                packet->next = NULL;
                packet->previous = NULL;
                packet_count--;
                delete packet;
                packet = next;
            } else {
                packet = packet->next;
            }
        }
        next_send_time = now + (u32)((f32)sent / ((f32)bandwidth / 1000.0));
    }

    queued_bytes = 0;
    for (EmuPacket *packet = first_packet; packet != NULL; packet = packet->next) {
        if (packet->flush_time >= packet->send_time || packet->flush_time >= now) {
            queued_bytes += packet->payload_size;
        }
    }

    raw_stats.Update();
    packet_stats.Update();

    f32 raw_bytes = (f32)raw_stats.totals.values[0];
    if (raw_bytes > 0.0f) {
        packet_stats.pack_ratio = (f32)packet_stats.totals.values[0] / raw_bytes;
    } else {
        packet_stats.pack_ratio = 0.0f;
    }
    if (packet_stats.totals.values[2] != 0) {
        packet_stats.average_packet_size = (f32)packet_stats.totals.values[0] / (f32)packet_stats.totals.values[2];
    } else {
        packet_stats.average_packet_size = 0.0f;
    }
    packet_stats.held_packets = packet_count;
}
