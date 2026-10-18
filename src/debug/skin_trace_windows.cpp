/* Debug-only adapter. Compile alongside src/debug/log.c; never link into Matching. */
#include "saga.h"
#include "skin_trace.h"
#include <errno.h>
#include <windows.h>

static volatile LONG skin_trace_state_lock = 0;
extern "C" TraceSaved skin_trace_lock(void) {
    TraceSaved saved;
    saved.error = GetLastError();
    saved.crt_errno = errno;
    while (InterlockedCompareExchange(&skin_trace_state_lock, 1, 0) != 0)
        Sleep(0);
    return saved;
}
extern "C" void skin_trace_unlock(TraceSaved saved) {
    InterlockedExchange(&skin_trace_state_lock, 0);
    errno = saved.crt_errno;
    SetLastError(saved.error);
}
extern "C" unsigned int skin_trace_thread(void) {
    return GetCurrentThreadId();
}
extern "C" unsigned int skin_trace_tick(void) {
    return GetTickCount();
}
extern "C" int skin_trace_configuration(unsigned int *delay, unsigned int *ordinal) {
    char text[64];
    DWORD count = GetEnvironmentVariableA("SAGA_SKIN_TRACE", text, sizeof(text));
    if (!count || count >= sizeof(text))
        return 0;
    unsigned int values[2] = {0, 0}, part = 0, digits = 0;
    for (unsigned int i = 0; i < count; ++i) {
        if (text[i] == ',' && part == 0 && digits) {
            part = 1;
            digits = 0;
            continue;
        }
        if (text[i] < '0' || text[i] > '9' || values[part] > 3600000 / 10)
            return 0;
        values[part] = values[part] * 10 + (text[i] - '0');
        ++digits;
    }
    if (part != 1 || !digits || values[0] > 3600000 || values[1] < 1 || values[1] > 32)
        return 0;
    *delay = values[0];
    *ordinal = values[1];
    return 1;
}
extern "C" void skin_trace_emit(const char *record) {
    LOG_INFO("%s", record);
}

extern "C" void skin_trace_capture_geometry(TraceWord geometry, TraceWord material, void *manager_pointer,
                                            TraceWord vertex_buffer, TraceWord index_buffer, TraceWord base_vertex,
                                            TraceWord start_index) {
    /* All native objects are read synchronously while valid at this draw. */
    PC_SHADER_MANAGER_VIEW *manager = (PC_SHADER_MANAGER_VIEW *)manager_pointer;
    PC_SHADER_OBJECT_FULL_VIEW *shader = manager ? manager->bound_slot : NULL;
    if (!shader)
        return;
    TraceDraw draw = {0};
    draw.geometry = geometry;
    draw.material = material;
    draw.shader_slot = (TraceWord)shader;
    draw.shader_key = shader->key;
    draw.profile = shader->version;
    draw.expected_vertex_shader = (TraceWord)shader->vertex_shader;
    draw.expected_pixel_shader = (TraceWord)shader->pixel_shader;
    draw.vertex_buffer = vertex_buffer;
    draw.index_buffer = index_buffer;
    draw.base_vertex = base_vertex;
    draw.start_index = start_index;
    const unsigned int semantics[7] = {0x11, 6, 7, 8, 0x49, 0x18, 1};
    for (unsigned int i = 0; i < 7; ++i) {
        for (unsigned int stage = 0; stage < 2; ++stage) {
            TraceParameter &out = draw.parameter[i * 2 + stage];
            out.stage = -1;
            out.first = -1;
            out.count = 0;
        }
        if (shader->version == 4) {
            ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantics[i]];
            for (unsigned int stage = 0; stage < 2; ++stage)
                if (uniform->stage_mask & (1u << stage)) {
                    TraceParameter &out = draw.parameter[i * 2 + stage];
                    out.stage = stage;
                    out.first = uniform->register_index;
                    out.count = uniform->value_count;
                    out.metadata = uniform->stage_mask;
                }
        } else {
            PC_SHADER_PARAMETER_LOCAL_PC *parameter = &shader->parameters[semantics[i]];
            unsigned int stage = (parameter->metadata & 0xf0000000U) == 0x10000000U ? 0 : 1;
            TraceParameter &out = draw.parameter[i * 2 + stage];
            out.stage = stage;
            out.first = parameter->register_index;
            out.count = (signed char)parameter->register_count;
            out.metadata = parameter->metadata;
        }
    }
    skin_trace_capture(&draw);
}
