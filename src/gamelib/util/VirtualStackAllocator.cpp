#include "saga.h"

VirtualStackAllocator::VirtualStackAllocator() {
    owns_memory = 0;
    cursor = NULL;
    base = NULL;
    end = NULL;
}

VirtualStackAllocator::VirtualStackAllocator(i32 size) {
    owns_memory = 0;
    cursor = new u8[size];
    base = cursor;
    end = cursor + size;
}

VirtualStackAllocator::VirtualStackAllocator(void *memory, u32 size) {
    owns_memory = 0;
    cursor = static_cast<u8 *>(memory);
    base = cursor;
    end = cursor + size;
}

VirtualStackAllocator::VirtualStackAllocator(VirtualStackAllocator &parent, u32 size) {
    owns_memory = 0;
    cursor = NULL;
    base = NULL;
    end = NULL;
    u8 *memory = parent.cursor;
    parent.cursor += size;
    setExternalMemoryPool(memory, size);
}

void VirtualStackAllocator::setExternalMemoryPool(void *memory, u32 size) {
    if (owns_memory != 0 && base != NULL) {
        delete[] base;
    }
    cursor = static_cast<u8 *>(memory);
    base = cursor;
    owns_memory = 0;
    end = cursor + size;
}

void VirtualStackAllocator::fun_00740fa0(VirtualStackAllocator &parent, u32 size) {
    u8 *memory = parent.cursor;
    parent.cursor += size;
    setExternalMemoryPool(memory, size);
}

/* A shader program key (Android ShaderObjectKey). */
struct ShaderObjectKey {
    ShaderObjectKey(const PC_SHADER_MATERIAL_FILTER_VIEW &filter, bool pixel_stage);
    bool operator==(const ShaderObjectKey &other) const;

    u32 key;
};

PC_SHADER_MATERIAL_FILTER_VIEW::PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                                               const PC_SHADER_FILTER_MATERIAL_VIEW *material,
                                                               i32 flags, i32 variant) {
    Initialize(desc, material, flags, variant);
}

ShaderObjectKey::ShaderObjectKey(const PC_SHADER_MATERIAL_FILTER_VIEW &filter, bool pixel_stage) {
    key = 0;
    NuShaderObjectKeyGenerate3(&key, &filter, pixel_stage != 0);
}

bool ShaderObjectKey::operator==(const ShaderObjectKey &other) const {
    return key == other.key;
}

i32 VirtualStackAllocator::fun_00741100() const {
    return cursor - base;
}

extern "C" void fun_00741170() {
}

extern "C" void fun_00741180() {
}

extern "C" void fun_00741200() {
}

void NuShaderObject::fun_00741250() {
    PCShaderNativeUnInit(this);
}

bool NuShaderObject::fun_00741260() const {
    return marker >= 0;
}

void NuShaderObject::fun_00741270() {
    PCDeviceSetVertexShaderPc(vertex_shader);
    PCDeviceSetPixelShaderPc(pixel_shader);
}

extern "C" void fun_00741290() {
}

VirtualStackAllocator::~VirtualStackAllocator() {
    if (owns_memory != 0 && base != NULL) {
        delete[] base;
    }
}

void NuShaderObject::fun_007412a0() {
    ref_count++;
}

void NuShaderObject::fun_007412b0() {
    ref_count--;
}

i32 NuShaderObject::fun_007412c0() const {
    return ref_count;
}

u32 *NuShaderObject::fun_007412d0() {
    return &key;
}
