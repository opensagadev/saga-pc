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
    if (owns_memory != 0 && base != NULL)
        delete[] base;
    cursor = static_cast<u8 *>(memory);
    base = cursor;
    owns_memory = 0;
    end = cursor + size;
}
VirtualStackAllocator::~VirtualStackAllocator() {
    if (owns_memory != 0 && base != NULL)
        delete[] base;
}
