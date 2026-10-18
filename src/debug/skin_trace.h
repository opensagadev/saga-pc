#ifndef SAGA_SKIN_TRACE_CORE_H
#define SAGA_SKIN_TRACE_CORE_H
/* Debug-only diagnostic API. Integration macros belong in diagnostics.h.
 * Opt in before launch: SAGA_SKIN_TRACE=60000,1 make BUILD_KIND=Debug run
 * Normal stdout and automatic build/log files receive the trace; no redirection is needed.
 * Delay is milliseconds from the first diagnostic hook. The ordinal selects the first
 * distinct (hierarchy,joints) token observed after that delay, not a named GAMEOBJECT.
 * The first eight tokens are inventoried. Shared/ambiguous or expired associations
 * yield no sample rather than an invented actor identity.
 * One geometry-resource/material pair is sampled at most 32 times, at least 250 ms
 * apart. State is capped at 24 KiB and trace output at 100 KiB including prefix reserve.
 * Raw register bits describe observed successful uploads; absent, failed, reset or
 * unobserved state is unknown. Large sections may be explicitly omitted for log limits.
 * Matching removes every hook and its argument evaluation. This observes inputs only;
 * animated specular variation alone does not establish a rendering defect.
 */
typedef unsigned int TraceWord;
struct TraceSaved {
    unsigned long error; // NOLINT(google-runtime-int): Win32 DWORD ABI; portable fixture backing.
    int crt_errno;
};
struct TraceParameter {
    int stage, first, count;
    TraceWord metadata;
};
struct TraceDraw {
    TraceWord geometry, material, shader_slot, shader_key, profile;
    TraceWord expected_vertex_shader, expected_pixel_shader;
    TraceWord vertex_buffer, index_buffer, base_vertex, start_index;
    TraceParameter parameter[14]; /* world11, lights6/7/8, specular49, palette18, ambient1 */
};
extern "C" {
    extern int skin_trace_gate;
    TraceSaved skin_trace_lock(void);
    void skin_trace_unlock(TraceSaved);
    unsigned int skin_trace_thread(void);
    unsigned int skin_trace_tick(void);
    int skin_trace_configuration(unsigned int *delay, unsigned int *ordinal);
    void skin_trace_emit(const char *record);
    void skin_trace_stream(unsigned int index, TraceWord base, unsigned int size);
    void skin_trace_skin(TraceWord hierarchy, TraceWord joints, const void *world64, TraceWord skin,
                         unsigned int count);
    void skin_trace_item(TraceWord draw_item, TraceWord palette, TraceWord geometry, TraceWord skin);
    void skin_trace_enter_item(TraceWord item);
    void skin_trace_leave_item(void);
    void skin_trace_palette(TraceWord packet, int count);
    void skin_trace_upload(int stage, unsigned int first, unsigned int count, const void *values,
                           long result);                            // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    void skin_trace_bind(int stage, TraceWord shader, long result); // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    void skin_trace_invalidate(TraceWord device);
    void skin_trace_reset_begin(TraceWord device);
    void skin_trace_reset_end(TraceWord device);
    void skin_trace_capture(const TraceDraw *draw);
}
#endif
