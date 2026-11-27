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

i32 PC_SHADER_MATERIAL_FILTER::HasSurfaceLayerAbove() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2) != 0 && descriptor->lightmap_tids[0] > parameter;
}

i32 PC_SHADER_MATERIAL_FILTER::HasSurfaceLayerAtOrBelow() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2) != 0 && descriptor->lightmap_tids[0] <= parameter;
}

extern "C" i32 NuShaderManagerGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

void NuShaderObject::InitHLSL(const u32 *key, i32 index, const char *vertex_source, i32 vertex_length, const char *pixel_source,
                              i32 pixel_length, i32 version) {
    PCShaderObjectInitHLSLPc(this, key, index, vertex_source, vertex_length, pixel_source, pixel_length, version);
}

i32 NuShaderObject::GetMarker() {
    return marker;
}

void ShaderManagerHLSLPc::AdaptDescriptor(const NUSHADERMTLDESC *input, const NUMTL *input_material) {
    NUSHADERMTLDESC *desc = const_cast<NUSHADERMTLDESC *>(input);
    NUMTL *material = const_cast<NUMTL *>(input_material);
    if ((desc->vertex_format & 0x4000000) != 0) {
        desc->layer_flags |= PC_SHADER_LAYER_PRELIGHT_FX;
        desc->flags |= 0x200000;
    }
    i32 version = compiler->dictionary.profile;
    if (PcWindowOptions.usehires == 0) {
        version = PC_SHADER_MODEL_1_1;
    }
    switch (version) {
        case PC_SHADER_MODEL_2_X:
            desc->flags &= 0xff77dffb;
            desc->layer_flags &= 0xe7fadffb;
            break;
        case PC_SHADER_MODEL_2_0:
            desc->flags &= 0xff7efffb;
            desc->layer_flags &= 0xbffbfffb;
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~0x20000U;
                desc->layer_flags &= ~PC_SHADER_LAYER_SHINEMAP;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~3U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_NORMAL_MAP | PC_SHADER_LAYER_PARALLAX_MAP);
            }
            if ((desc->flags & 0x20000) != 0 || (desc->layer_flags & PC_SHADER_LAYER_SHINEMAP) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= 0xbfffffef;
                desc->layer_flags &= 0xfdffffef;
            }
            if ((desc->flags & 2) != 0) {
                desc->flags = (desc->flags & ~2U) | 1;
                desc->layer_flags = (desc->layer_flags & ~PC_SHADER_LAYER_PARALLAX_MAP) | PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x60) == 0x60) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x80) != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x25) == 0x25) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 1) != 0 && desc->lightmap_tids[1] != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            desc->flags &= 0xffff9bff;
            desc->layer_flags &= 0xffff9bff;
            if ((desc->layer_flags & 0x4000000) != 0 && (desc->layer_flags & PC_SHADER_LAYER_PHONG) != 0 &&
                (desc->layer_flags & PC_SHADER_LAYER_PRELIGHT_FX) != 0 && (desc->layer_flags & PC_SHADER_LAYER_NORMAL_MAP) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            desc->flags &= 0xfdffffef;
            desc->layer_flags &= 0xffefffef;
            if ((desc->flags & 0x49) == 0x49) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x100) != 0 && (desc->flags & 8) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 0x80201000) == 0x80201000) {
                desc->flags &= ~9U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_NORMAL_MAP | PC_SHADER_LAYER_PHONG);
            }
            desc->flags &= 0xfff7ffff;
            desc->layer_flags &= 0xe7feffff;
            break;
        case PC_SHADER_MODEL_1_1: {
            bool layer = (desc->layer_flags & PC_SHADER_LAYER_REFRACTION) != 0;
            if ((desc->flags & 0x40200) == 0) {
                desc->flags &= ~0x400U;
            }
            desc->layer_flags &= ~PC_SHADER_LAYER_REFRACTION;
            if (layer && (desc->layer_flags & PC_SHADER_LAYER_REFRACTION) == 0 && (material->attribs.word & 0x700000) != 0x100000) {
                material->attribs.word &= 0x807fffff;
            }
            if ((desc->flags & 0x4000) != 0) {
                desc->shininess = (f32)(desc->shininess * (f64)0.4f);
            }
            desc->flags &= 0xffff9ffc;
            desc->layer_flags &= 0xffff9ffc;
            if ((desc->flags & 0x10000) == 0) {
                if ((desc->flags & 0x20) != 0) {
                    desc->flags &= ~8U;
                    desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
                }
            } else {
                desc->flags &= ~0x20U;
                desc->layer_flags &= ~PC_SHADER_LAYER_ENVMAP_CUBE;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            desc->flags &= 0xf916fffb;
            desc->layer_flags &= 0x27c8fffb;
            if ((desc->layer_flags & 0x4000000) != 0) {
                desc->flags &= ~0x28U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_PHONG | PC_SHADER_LAYER_ENVMAP_CUBE);
            }
            break;
        }
    }
    if (compiler->dictionary.force_mode) {
        desc->flags &= 0xfffffbd7;
        desc->layer_flags &= 0xfffffbd7;
    }
}

#include <new>

// for the header: NuShaderObject::stage_flags is a bitfield (each path bit is stored with xor/and/xor).
struct NuShaderObjectPaths {
    u32 base : 1;
    u32 first : 1;
    u32 layer : 1;
    u32 second : 1;
    u32 unknown : 28;
};

bool ShaderManagerHLSL::CreateShader(const PC_SHADER_MATERIAL_FILTER *filter, bool pixel_stage, NuShaderObject *shader, i32 index) {
    u32 key = 0;
    NuShaderObjectKeyGenerate3(&key, filter, pixel_stage != 0);
    vertex_source.length = 0;
    pixel_source.length = 0;
    compiler->Generate(const_cast<PC_SHADER_MATERIAL_FILTER *>(filter), &vertex_source, &pixel_source);
    PCShaderObjectInitHLSLPc(shader, &key, index, vertex_source.data, vertex_source.length, pixel_source.data, pixel_source.length,
                             compiler->dictionary.profile);
    NuShaderObjectPaths *paths = reinterpret_cast<NuShaderObjectPaths *>(&shader->stage_flags);
    paths->base = filter->variant == 0;
    paths->first = filter->first_path != 0;
    paths->layer = filter->variant != 0 ? false : (bool)((filter->descriptor->layer_flags >> 16) & 1);
    paths->second = filter->second_path != 0;
    return true;
}

extern "C" void NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end) {
    VirtualStackAllocator allocator;
    allocator.setExternalMemoryPool(arena->void_ptr, arena_end.addr - arena->addr);
    void *memory = allocator.cursor;
    allocator.cursor += sizeof(ShaderManagerHLSL);
    PCShaderSlots = reinterpret_cast<PC_SHADER_SLOT_VIEW *>(new (memory) ShaderManagerHLSL(allocator));
    arena->addr += allocator.cursor - allocator.base;
}
