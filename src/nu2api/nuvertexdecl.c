#include "legoapi/items/collect/bolts.h"
#include "nu2api/PCInput.h"
#include "nu2api/VertexDeclarations.h"
#include "nu2api/nu3d/nuvertexformat.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"

PC_VERTEX_DECL_ENTRY *PCVertexDeclarationFindSlot(const PC_VERTEX_DECL_ENTRY *key, PC_VERTEX_DECL_ENTRY *first, i32 count, u8 *found) {
    PC_VERTEX_DECL_ENTRY *last = first + count - 1;
    PC_VERTEX_DECL_ENTRY *middle;
    while (first <= last) {
        middle = first + (last - first) / 2;
        if (middle->key < key->key) {
            first = middle + 1;
        } else if (middle->key > key->key) {
            last = middle - 1;
        } else {
            *found = 1;
            return middle;
        }
    }
    *found = 0;
    return first;
}
