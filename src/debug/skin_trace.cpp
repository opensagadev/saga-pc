#include "skin_trace.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    int skin_trace_gate = -1;
}
namespace {
    const unsigned int max_lines = 42, max_log_bytes = 100 * 1024;
    struct Register {
        TraceWord value[4];
        unsigned int valid;
    };
    struct Stream {
        TraceWord base;
        unsigned int size, epoch;
    };
    struct Owner {
        TraceWord skin, hierarchy, joints, world[16];
        unsigned int count, stream, epoch, ordinal;
    };
    struct Item {
        TraceWord item, palette, geometry, hierarchy, joints, world[16];
        unsigned int stream, epoch, ordinal;
    };
    struct Thread {
        unsigned int id, depth;
        TraceWord item[4], palette;
        int palette_count;
        unsigned int palette_stream, palette_epoch;
    };
    struct Instance {
        TraceWord hierarchy, joints;
        unsigned int ordinal;
    };
    struct State {
        int configured, enabled, stopped, resetting;
        unsigned int start, delay, selected, samples, last_tick, lines, bytes, next_owner, next_item, instances;
        TraceWord device, shader[2];
        unsigned int shader_valid[2], writer, multiple_writers;
        TraceWord selected_geometry[4], selected_material;
        unsigned int selected_draw;
        Stream stream[2];
        Owner owner[32];
        Item item[64];
        Thread thread[4];
        Instance instance[32];
        Register vertex[256], pixel[224];
    };
    State state = {0};
    typedef char TraceStateWithin24KiB[sizeof(State) <= 24 * 1024 ? 1 : -1];
    struct Guard {
        TraceSaved saved;
        Guard() : saved(skin_trace_lock()) {
            if (!state.configured) {
                state.configured = 1;
                state.enabled = skin_trace_configuration(&state.delay, &state.selected);
                skin_trace_gate = state.enabled;
                state.start = skin_trace_tick();
            }
        }
        ~Guard() {
            skin_trace_unlock(saved);
        }
    };
    bool active() {
        return state.enabled && !state.stopped;
    }
    unsigned int stream_of(TraceWord address) {
        for (unsigned int i = 0; i < 2; ++i)
            if (state.stream[i].size && address >= state.stream[i].base &&
                address - state.stream[i].base < state.stream[i].size)
                return i;
        return 2;
    }
    Thread *thread_state() {
        unsigned int id = skin_trace_thread();
        for (unsigned int i = 0; i < 4; ++i)
            if (state.thread[i].id == id)
                return &state.thread[i];
        for (unsigned int j = 0; j < 4; ++j)
            if (!state.thread[j].id) {
                state.thread[j].id = id;
                return &state.thread[j];
            }
        return 0;
    }
    bool device_writer() {
        unsigned int id = skin_trace_thread();
        if (!state.writer)
            state.writer = id;
        if (state.writer != id)
            state.multiple_writers = 1;
        return state.multiple_writers == 0;
    }
    void invalidate_registers() {
        for (unsigned int i = 0; i < 256; ++i)
            state.vertex[i].valid = 0;
        for (unsigned int j = 0; j < 224; ++j)
            state.pixel[j].valid = 0;
        state.shader_valid[0] = state.shader_valid[1] = 0;
    }
    void hex(char *out, unsigned int &used, unsigned int capacity, TraceWord word) {
        const char digits[] = "0123456789abcdef";
        if (used + 9 >= capacity)
            return;
        for (int n = 7; n >= 0; --n)
            out[used++] = digits[(word >> (n * 4)) & 15];
        out[used++] = ' ';
        out[used] = 0;
    }
    void text(char *out, unsigned int &used, unsigned int capacity, const char *word) {
        unsigned int n = (unsigned int)strlen(word);
        if (n >= capacity - used)
            return;
        memcpy(out + used, word, n + 1);
        used += n;
    }
} // namespace
extern "C" void skin_trace_stream(unsigned int index, TraceWord base, unsigned int size) {
    Guard guard;
    if (!active() || index >= 2)
        return;
    Stream &s = state.stream[index];
    s.base = base;
    s.size = size;
    ++s.epoch;
    for (unsigned int i = 0; i < 32; ++i)
        if (state.owner[i].stream == index)
            state.owner[i].skin = 0;
    for (unsigned int j = 0; j < 64; ++j)
        if (state.item[j].stream == index)
            state.item[j].item = 0;
    for (unsigned int k = 0; k < 4; ++k)
        if (state.thread[k].palette_stream == index) {
            state.thread[k].palette = 0;
            state.thread[k].palette_count = 0;
        }
}
extern "C" void skin_trace_skin(TraceWord hierarchy, TraceWord joints, const void *world64, TraceWord skin,
                                unsigned int count) {
    char record[512] = {0};
    bool emit = false;
    {
        Guard guard;
        if (!active() || !world64 || !skin || skin_trace_tick() - state.start < state.delay)
            return;
        unsigned int stream = stream_of(skin);
        if (stream >= 2)
            return;
        if (count > (state.stream[stream].size - (skin - state.stream[stream].base)) / 64)
            return;
        unsigned int ordinal = 0;
        for (unsigned int i = 0; i < state.instances; ++i)
            if (state.instance[i].hierarchy == hierarchy && state.instance[i].joints == joints)
                ordinal = state.instance[i].ordinal;
        if (!ordinal && state.instances < 32) {
            Instance &v = state.instance[state.instances++];
            v.hierarchy = hierarchy;
            v.joints = joints;
            v.ordinal = state.instances;
            ordinal = v.ordinal;
            if (ordinal <= 8) {
                unsigned int used = 0;
                text(record, used, sizeof(record),
                     "SKIN_TRACE inventory submission-instance ordinal/hierarchy/joints/world: ");
                hex(record, used, sizeof(record), ordinal);
                hex(record, used, sizeof(record), hierarchy);
                hex(record, used, sizeof(record), joints);
                for (unsigned int w = 0; w < 16; ++w) {
                    TraceWord bits;
                    memcpy(&bits, (const char *)world64 + w * 4, 4);
                    hex(record, used, sizeof(record), bits);
                }
                if (state.lines < max_lines && used + 160 <= max_log_bytes - state.bytes) {
                    ++state.lines;
                    state.bytes += used + 160;
                    emit = true;
                }
            }
        }
        if (!ordinal)
            return;
        for (unsigned int k = 0; k < 32; ++k) {
            Owner &prior = state.owner[k];
            if (prior.skin && prior.hierarchy == hierarchy && prior.joints == joints && prior.stream == stream &&
                prior.epoch == state.stream[stream].epoch && memcmp(prior.world, world64, 64)) {
                for (unsigned int a = 0; a < 32; ++a)
                    if (state.owner[a].hierarchy == hierarchy && state.owner[a].joints == joints)
                        state.owner[a].ordinal = 0;
                for (unsigned int b = 0; b < 64; ++b)
                    if (state.item[b].hierarchy == hierarchy && state.item[b].joints == joints)
                        state.item[b].ordinal = 0;
                ordinal = 0;
            }
        }
        Owner &o = state.owner[state.next_owner++ % 32];
        o.skin = skin;
        o.hierarchy = hierarchy;
        o.joints = joints;
        memcpy(o.world, world64, 64);
        o.count = count;
        o.stream = stream;
        o.epoch = state.stream[stream].epoch;
        o.ordinal = ordinal;
    }
    if (emit)
        skin_trace_emit(record);
}
extern "C" void skin_trace_item(TraceWord draw_item, TraceWord palette, TraceWord geometry, TraceWord skin) {
    Guard guard;
    if (!active())
        return;
    unsigned int stream = stream_of(draw_item);
    if (stream >= 2 || stream_of(palette) != stream)
        return;
    Owner *owner = 0;
    /* Newest registration wins; no borrowed native pointer is retained for later
     * reads. */
    unsigned int total = state.next_owner < 32 ? state.next_owner : 32;
    for (unsigned int n = 0; n < total; ++n) {
        Owner &o = state.owner[(state.next_owner - 1 - n) % 32];
        if (o.skin == skin && o.stream == stream && o.epoch == state.stream[stream].epoch) {
            owner = &o;
            break;
        }
    }
    if (!owner || owner->ordinal != state.selected)
        return;
    Item &i = state.item[state.next_item++ % 64];
    i.item = draw_item;
    i.palette = palette;
    i.geometry = geometry;
    i.hierarchy = owner->hierarchy;
    i.joints = owner->joints;
    memcpy(i.world, owner->world, 64);
    i.stream = stream;
    i.epoch = owner->epoch;
    i.ordinal = owner->ordinal;
}
extern "C" void skin_trace_enter_item(TraceWord item) {
    Guard guard;
    if (!active())
        return;
    Thread *t = thread_state();
    if (!t)
        return;
    if (t->depth < 4)
        t->item[t->depth] = item;
    ++t->depth;
}
extern "C" void skin_trace_leave_item() {
    Guard guard;
    if (!active())
        return;
    Thread *t = thread_state();
    if (t && t->depth)
        --t->depth;
}
extern "C" void skin_trace_palette(TraceWord packet, int count) {
    Guard guard;
    if (!active())
        return;
    Thread *t = thread_state();
    if (t) {
        t->palette = packet;
        t->palette_count = count;
        t->palette_stream = stream_of(packet);
        t->palette_epoch = t->palette_stream < 2 ? state.stream[t->palette_stream].epoch : 0;
    }
}
extern "C" void skin_trace_upload(int stage, unsigned int first, unsigned int count, const void *values,
                                  long result) { // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    Guard guard;
    if (!active() || stage < 0 || stage > 1)
        return;
    if (state.resetting || !device_writer()) {
        invalidate_registers();
        return;
    }
    unsigned int limit = stage ? 224 : 256;
    Register *r = stage ? state.pixel : state.vertex;
    if (first >= limit || count > limit - first || !values) {
        for (unsigned int i = 0; i < limit; ++i)
            r[i].valid = 0;
        return;
    }
    for (unsigned int j = 0; j < count; ++j) {
        r[first + j].valid = result >= 0;
        if (result >= 0)
            memcpy(r[first + j].value, (const char *)values + j * 16, 16);
    }
}
extern "C" void skin_trace_bind(int stage, TraceWord shader,
                                long result) { // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    Guard guard;
    if (!active() || stage < 0 || stage > 1)
        return;
    if (state.resetting || !device_writer()) {
        invalidate_registers();
        return;
    }
    state.shader_valid[stage] = result >= 0;
    state.shader[stage] = shader;
}
namespace {
    void invalidate_locked(TraceWord device) {
        state.device = device;
        invalidate_registers();
        state.writer = skin_trace_thread();
        state.multiple_writers = 0;
        for (unsigned int i = 0; i < 64; ++i)
            state.item[i].item = 0;
        for (unsigned int j = 0; j < 32; ++j)
            state.owner[j].skin = 0;
        for (unsigned int k = 0; k < 4; ++k) {
            state.thread[k].palette = 0;
            state.thread[k].palette_count = 0;
        }
    }
} // namespace
extern "C" void skin_trace_invalidate(TraceWord device) {
    Guard guard;
    if (!active())
        return;
    invalidate_locked(device);
}
extern "C" void skin_trace_reset_begin(TraceWord device) {
    Guard guard;
    if (!active())
        return;
    invalidate_locked(device);
    state.resetting = 1;
}
extern "C" void skin_trace_reset_end(TraceWord device) {
    Guard guard;
    if (!active())
        return;
    invalidate_locked(device);
    state.resetting = 0;
}
extern "C" void skin_trace_capture(const TraceDraw *draw) {
    char record[3000] = {0};
    bool emit = false;
    {
        Guard guard;
        if (!active() || state.resetting || !draw)
            return;
        unsigned int now = skin_trace_tick();
        if (now - state.start < state.delay || (state.samples && now - state.last_tick < 250))
            return;
        Thread *t = thread_state();
        if (!t || !t->depth || t->depth > 4)
            return;
        TraceWord current = t->item[t->depth - 1];
        Item *item = 0;
        unsigned int total = state.next_item < 64 ? state.next_item : 64;
        for (unsigned int n = 0; n < total; ++n) {
            Item &v = state.item[(state.next_item - 1 - n) % 64];
            if (v.item == current && v.geometry == draw->geometry && v.epoch == state.stream[v.stream].epoch) {
                item = &v;
                break;
            }
        }
        if (!item || item->ordinal != state.selected || item->palette != t->palette ||
            t->palette_stream != item->stream || t->palette_epoch != item->epoch || t->palette_count < 0 ||
            t->palette_count > 8)
            return;
        TraceWord identity[4] = {draw->vertex_buffer, draw->index_buffer, draw->base_vertex, draw->start_index};
        if (!state.selected_draw) {
            memcpy(state.selected_geometry, identity, sizeof(identity));
            state.selected_material = draw->material;
            state.selected_draw = 1;
        } else if (memcmp(state.selected_geometry, identity, sizeof(identity)) ||
                   state.selected_material != draw->material)
            return;
        unsigned int used = 0;
        text(record, used, sizeof(record),
             "SKIN_TRACE pre-draw-attempt submission-instance "
             "ordinal/key/slot/material/geometry/hierarchy/joints/epoch: ");
        hex(record, used, sizeof(record), item->ordinal);
        hex(record, used, sizeof(record), draw->shader_key);
        hex(record, used, sizeof(record), draw->shader_slot);
        hex(record, used, sizeof(record), draw->material);
        hex(record, used, sizeof(record), draw->geometry);
        hex(record, used, sizeof(record), item->hierarchy);
        hex(record, used, sizeof(record), item->joints);
        hex(record, used, sizeof(record), item->epoch);
        text(record, used, sizeof(record), " profile/tick/sample/item/palette/vb/ib/base/start: ");
        hex(record, used, sizeof(record), draw->profile);
        hex(record, used, sizeof(record), now);
        hex(record, used, sizeof(record), state.samples);
        hex(record, used, sizeof(record), current);
        hex(record, used, sizeof(record), item->palette);
        hex(record, used, sizeof(record), draw->vertex_buffer);
        hex(record, used, sizeof(record), draw->index_buffer);
        hex(record, used, sizeof(record), draw->base_vertex);
        hex(record, used, sizeof(record), draw->start_index);
        text(record, used, sizeof(record), " logical-vs/ps, actual-known-vs/ps: ");
        hex(record, used, sizeof(record), draw->expected_vertex_shader);
        hex(record, used, sizeof(record), draw->expected_pixel_shader);
        for (unsigned int stage = 0; stage < 2; ++stage) {
            if (state.shader_valid[stage])
                hex(record, used, sizeof(record), state.shader[stage]);
            else
                text(record, used, sizeof(record), "unknown ");
        }
        text(record, used, sizeof(record), " submission-world: ");
        for (unsigned int w = 0; w < 16; ++w)
            hex(record, used, sizeof(record), item->world[w]);
        const char *names[] = {" world11:", " light6:", " light7:", " light8:", " spec49:", " skin18:", " ambient1:"};
        for (unsigned int p = 0; p < 14; ++p) {
            unsigned int semantic = p / 2;
            const TraceParameter &v = draw->parameter[p];
            text(record, used, sizeof(record), names[semantic]);
            hex(record, used, sizeof(record), v.metadata);
            if (v.stage < 0 || v.stage > 1 || v.first < 0 || v.count <= 0) {
                text(record, used, sizeof(record), "absent/unknown ");
                continue;
            }
            unsigned int limit = v.stage ? 224 : 256;
            Register *r = v.stage ? state.pixel : state.vertex;
            unsigned int count = (unsigned int)v.count;
            if (semantic == 5 && count > (unsigned int)t->palette_count * 4)
                count = t->palette_count * 4;
            if (count > (semantic == 5 ? 32u : (semantic == 0 || semantic == 4 ? 4u : 1u)) ||
                (unsigned int)v.first >= limit || count > limit - (unsigned int)v.first) {
                text(record, used, sizeof(record), "range-unknown ");
                continue;
            }
            hex(record, used, sizeof(record), v.stage);
            hex(record, used, sizeof(record), v.first);
            hex(record, used, sizeof(record), count);
            if (count * 4 * 9 + 128 > sizeof(record) - used) {
                text(record, used, sizeof(record), "omitted-log-cap ");
                continue;
            }
            for (unsigned int n = 0; n < count; ++n) {
                if (!r[v.first + n].valid)
                    text(record, used, sizeof(record), "unknown ");
                else
                    for (unsigned int c = 0; c < 4; ++c)
                        hex(record, used, sizeof(record), r[v.first + n].value[c]);
            }
        }
        if (state.lines >= max_lines || used + 160 > max_log_bytes - state.bytes) {
            state.stopped = 1;
            skin_trace_gate = 0;
            return;
        }
        ++state.lines;
        state.bytes += used + 160;
        ++state.samples;
        state.last_tick = now;
        if (state.samples >= 32) {
            state.stopped = 1;
            skin_trace_gate = 0;
        }
        emit = true;
    }
    /* The state lock is released before entering the existing logger. */
    if (emit)
        skin_trace_emit(record);
}
