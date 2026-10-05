#include "saga.h"
ShaderCompilerBufferAllocatorPc::ShaderCompilerBufferAllocatorPc(i32 size, bool owns) {
    data = new u8[size];
    length = 0;
    capacity = size;
    owns_memory = owns;
}
ShaderCompilerBufferAllocatorPc::ShaderCompilerBufferAllocatorPc(VirtualStackAllocator &parent, i32 size) {
    data = parent.cursor;
    parent.cursor += size;
    length = 0;
    capacity = size;
    owns_memory = 0;
}
ShaderCompilerBufferAllocatorPc::~ShaderCompilerBufferAllocatorPc() {
    if (data != NULL && owns_memory != 0) {
        delete[] data;
        data = NULL;
    }
}
void ShaderCompilerBufferAllocatorPc::Resize(i32 size) {
    u8 *memory = new u8[size];
    i32 copied = length < size ? length : size;
    memcpy(memory, data, copied);
    delete[] data;
    data = memory;
    capacity = size;
    if (length > size)
        length = size;
}
