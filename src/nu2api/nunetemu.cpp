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

extern "C" i32 __cdecl refpack(u8 *source, i32 source_size, u8 *destination);
i32 unref(u8 *source, u8 *destination);

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

/* Payloads larger than one packet are split by size: each 16-bit length
 * prefixed payload becomes one segment, largest first. */
struct NuNetEmuSegment {
    u16 offset;
    u16 size;
};

static i32 cbSortSeg(const void *left, const void *right) {
    u16 left_size = ((const NuNetEmuSegment *)left)->size;
    u16 right_size = ((const NuNetEmuSegment *)right)->size;
    if (left_size > right_size)
        return -1;
    return left_size < right_size;
}

i32 NuNetEmu::RecvFrom(void *data, i32 size, u32 &address) {
    i32 received = 0;
    if (enabled) {
        if (read_offset >= unpacked_size) {
            received = theNetSessionManager->RecvFrom(packed_buffer, 0xbb8, &address);
            if (received > 0) {
                packet_stats.totals.values[1] += received;
                packet_stats.totals.values[3]++;
                read_offset = 0;
                packed_size = received;
                u32 start_time = UtilGetTime();
                unpacked_size = unref(packed_buffer, unpacked_buffer);
                u32 end_time = UtilGetTime();
                raw_stats.totals.values[1] += unpacked_size;
                packet_stats.pack_time[1] += end_time - start_time;
                raw_stats.totals.values[3]++;
            }
        }

        if (read_offset < unpacked_size && received >= 0) {
            i32 payload_size = unpacked_buffer[read_offset] | (unpacked_buffer[read_offset + 1] << 8);
            i32 copy_size = payload_size < size ? payload_size : size;
            PCByteCopy_candidate(data, &unpacked_buffer[read_offset + 2], copy_size);
            read_offset += payload_size + 2;
            return copy_size;
        }
    } else {
        return theNetSessionManager->RecvFrom(data, size, &address);
    }
    return received;
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

i32 NuNetEmu::SplitSendPacket(EmuPacket *packet) {
    u32 start_time = UtilGetTime();
    packed_size = refpack(packet->payload, packet->payload_size, packed_buffer);
    packet_stats.pack_time[0] += UtilGetTime() - start_time;

    i32 size = packed_size;
    if (size <= 0x4f0) {
        packet_stats.totals.values[0] += size;
        packet_stats.totals.values[2]++;
        theNetSessionManager->SendTo(packed_buffer, size, &packet->address);
        return size;
    }

    packet_stats.split_packets++;
    NuNetEmuSegment segments[256];
    u32 segment_count = 0;
    u32 offset = 0;
    while (offset < packet->payload_size && segment_count < 256) {
        i32 payload_size = packet->payload[offset] | (packet->payload[offset + 1] << 8);
        segments[segment_count].offset = (u16)offset;
        segments[segment_count].size = (u16)(payload_size + 2);
        segment_count++;
        offset += payload_size + 2;
    }
    qsort(segments, segment_count, sizeof(NuNetEmuSegment), cbSortSeg);

    EmuPacket *halves[2];
    halves[0] = new EmuPacket(&packet->address);
    halves[1] = new EmuPacket(&packet->address);
    for (u32 i = 0; i < segment_count; i++) {
        EmuPacket *half = halves[halves[1]->payload_size < halves[0]->payload_size];
        PCByteCopy_candidate(&half->payload[half->payload_size], &packet->payload[segments[i].offset],
                             segments[i].size);
        half->payload_size += segments[i].size;
    }

    i32 sent = SplitSendPacket(halves[0]);
    sent += SplitSendPacket(halves[1]);
    delete halves[0];
    delete halves[1];
    return sent;
}

i32 NuNetEmu::SendTo(void *data, i32 size, u32 *address, i32 flags) {
    if (enabled) {
        i32 sent = size;
        switch (loss_mode) {
            case 1:
                if (NuRandFloat() < loss_chance)
                    return sent;
                break;
            case 2:
                if (burst_remaining == 0 && NuRandFloat() < loss_chance)
                    burst_remaining = (i32)(NuRandFloat() * (burst_max - burst_min)) + burst_min;
                if (burst_remaining > 0) {
                    burst_remaining--;
                    return sent;
                }
                break;
        }

        if (packet_count < max_packets) {
            EmuPacket *packet = FindPacket(address, size);
            if (packet == NULL) {
                packet = new EmuPacket(address);
                u32 now = UtilGetFrameStartTime();
                if (max_latency > 0)
                    packet->send_time = (u32)(NuRandFloat() * (max_latency - min_latency)) + min_latency + now;
                else
                    packet->send_time = 0;
                packet->creation_time = now;
                packet->flush_time = now + flush_delay;
                packet->next = NULL;
                packet->previous = last_packet;
                if (last_packet != NULL)
                    last_packet->next = packet;
                last_packet = packet;
                if (first_packet == NULL)
                    first_packet = packet;
                packet_count++;
            }
            packet->AddPayload(data, size);
            queued_bytes += size;
        } else {
            sent = 0;
        }
        return sent;
    }
    return theNetSessionManager->SendTo(data, size, address);
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

/* Empty trace hook called by the network object system. */
void NetTrace(...) {
}
