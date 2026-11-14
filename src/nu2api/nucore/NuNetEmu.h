/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edlevelall.h"
#include "gamelib/util/gamelib_util_types.h"
#endif

#ifdef __cplusplus

/* Network condition emulator (nunetemu, 0x610a50-0x611889): batches outgoing
 * payloads into packets, applies latency/loss and keeps statistics. */
struct NuNetEmu {
    class EmuPacket : public CSListLink<EmuPacket> {
      public:
        EmuPacket(u32 *net_address);
        ~EmuPacket();
        static void *operator new(u32 size);
        static void operator delete(void *memory);
        i32 GetFreeBytes();
        void AddPayload(void *data, i32 size);

        u32 address;
        u8 *payload;
        u32 payload_size;
        u32 payload_count;
        u32 flush_time;
        u32 send_time;
        u32 creation_time;
    };

    class PackStats : public NetStats {
      public:
        PackStats(const char *name);
        virtual void Draw(f32 x, f32 y, f32 width, f32 height, eInfo info);

        u32 pack_time[2];
        u32 split_packets;
        u32 held_packets;
        f32 pack_ratio;
        f32 average_packet_size;
    };

    enum eConditions { CONDITIONS_NORMAL, CONDITIONS_LIGHT, CONDITIONS_HEAVY, CONDITIONS_BURSTY };

    NuNetEmu();
    EmuPacket *FindPacket(u32 *net_address, i32 size);
    i32 RecvFrom(void *data, i32 size, u32 &address);
    i32 SendTo(void *data, i32 size, u32 *address, i32 flags);
    i32 SplitSendPacket(EmuPacket *packet);
    void SetConditions(eConditions new_conditions);
    void Update();
    void fun_0060e750(u8 new_enabled);
    NetStats *fun_0060e760();
    PackStats *fun_0060e770();
    i32 fun_00611dd0() const;

    i32 field_00;
    EmuPacket *first_packet;
    EmuPacket *last_packet;
    i32 packet_count;
    u8 enabled;
    i32 min_latency;
    i32 max_latency;
    i32 queued_bytes;
    i32 conditions;
    i32 loss_mode;
    i32 burst_min;
    i32 burst_max;
    i32 burst_remaining;
    f32 loss_chance;
    u32 flush_size;
    u32 flush_delay;
    u8 packed_buffer[0xbb8];
    u8 unpacked_buffer[0xbb8];
    i32 read_offset;
    i32 unpacked_size;
    i32 packed_size;
    u32 next_send_time;
    u32 bandwidth_low;
    u32 bandwidth_high;
    i32 max_packets;
    NetStats raw_stats;
    PackStats packet_stats;
};

extern NuNetEmu theNuNetEmu;
#endif
