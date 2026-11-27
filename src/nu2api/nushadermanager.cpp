#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/PCInput.h"
#include "nu2api/ShaderManagerHLSL.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/ShaderObjectKey.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>

template <> bool ShaderManagerTemplate<NuShaderObject>::CreateShader(const PC_SHADER_MATERIAL_FILTER *, bool, NuShaderObject *, i32);
template <> void ShaderManagerTemplate<NuShaderObject>::AdaptDescriptor(const NUSHADERMTLDESC *, const NUMTL *);

#include <new>

extern "C" void NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end) {
    VirtualStackAllocator allocator;
    allocator.setExternalMemoryPool(arena->void_ptr, arena_end.addr - arena->addr);
    void *memory = allocator.cursor;
    allocator.cursor += sizeof(ShaderManagerHLSL);
    PCShaderSlots = reinterpret_cast<PC_SHADER_SLOT_VIEW *>(new (memory) ShaderManagerHLSL(allocator));
    arena->addr += allocator.cursor - allocator.base;
}
