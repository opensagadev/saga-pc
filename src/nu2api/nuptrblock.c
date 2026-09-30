#include "saga.h"

void *__cdecl NuPtrBlockFix(void *block) {
    VARIPTR table;
    VARIPTR return_pointer;
    VARIPTR entry;
    i32 count;
    i32 index;

    return_pointer.void_ptr = block;
    return_pointer.addr += 4;
    table.void_ptr = block;
    table.addr = *(u32 *)table.void_ptr + table.addr;
    count = *(i32 *)table.void_ptr;
    table.addr += 4;
    for (index = 0; index < count; ++index) {
        entry.addr = *(u32 *)table.void_ptr + table.addr;
        if (*(u32 *)entry.void_ptr != 0)
            *(u32 *)entry.void_ptr += entry.addr;
        table.addr += 4;
    }
    return return_pointer.void_ptr;
}
