#include "saga.h"
#include <string.h>

static VARIPTR RndrStreamFirstPc = {0};
static VARIPTR RndrStreamSecondPc = {0};
static i32 RndrStreamSizePc = 0;
static i32 RndrStreamIndexPc = 0;
static u32 RndrStreamUsedPc = 0;
static u32 RndrStreamMaxUsedPc = 0;

static void __cdecl NuRndrResetStreamBufferPc(void) {
    if (RndrStreamIndexPc == 0) {
        nudlist_stream_cursor = RndrStreamFirstPc;
        nuprim_field_ba8 = RndrStreamFirstPc.addr + RndrStreamSizePc;
    } else if (RndrStreamIndexPc == 1) {
        nudlist_stream_cursor = RndrStreamSecondPc;
        nuprim_field_ba8 = RndrStreamSecondPc.addr + RndrStreamSizePc;
    }
    if (nudlist_stream_cursor.addr & 15)
        nudlist_stream_cursor.addr = (nudlist_stream_cursor.addr + 15) & ~15u;
    SAGA_SKIN_CALL(skin_trace_stream, (unsigned)RndrStreamIndexPc, nudlist_stream_cursor.addr,
                   nuprim_field_ba8 - nudlist_stream_cursor.addr);
}

void __cdecl NuRndrStreamInit(i32 size, VARIPTR *buffer) {
    if (buffer == NULL) {
        RndrStreamFirstPc.void_ptr = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 126);
        RndrStreamSecondPc.void_ptr = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 127);
    } else {
        RndrStreamFirstPc.addr = (buffer->addr + 31) & ~31u;
        RndrStreamSecondPc.addr = (RndrStreamFirstPc.addr + size + 31) & ~31u;
        buffer->addr = RndrStreamSecondPc.addr + size;
    }
    RndrStreamSizePc = size;
    RndrStreamIndexPc = 0;
    memset(RndrStreamFirstPc.void_ptr, 0, size);
    memset(RndrStreamSecondPc.void_ptr, 0, size);
    NuRndrResetStreamBufferPc();
}

void __cdecl NuRndrSwapStreamBuffers(void) {
    if (RndrStreamIndexPc == 0)
        RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamFirstPc.addr;
    else if (RndrStreamIndexPc == 1)
        RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamSecondPc.addr;
    if (RndrStreamUsedPc > RndrStreamMaxUsedPc)
        RndrStreamMaxUsedPc = RndrStreamUsedPc;
    if (RndrStreamUsedPc > (u32)RndrStreamSizePc) {
        PCGraphicsRenderTextureAllocationError();
        DebugBreak();
    }
    RndrStreamIndexPc = (RndrStreamIndexPc + 1) % 2;
    NuRndrResetStreamBufferPc();
}
