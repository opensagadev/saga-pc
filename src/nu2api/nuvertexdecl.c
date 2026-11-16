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

i32 PCVertexDeclarationCountPC = 0;
PC_VERTEX_DECL_ENTRY PCVertexDeclarationCachePC[256] = {0};

PC_TEXTURE_RESOURCE *NuGetVertexDeclaration(u32 descriptor) {
    PC_VERTEX_DECL_ENTRY key;
    PC_VERTEX_DECL_ENTRY *record;
    i32 count = PCVertexDeclarationCountPC;
    u8 found = 0;
    key.key = descriptor;
    key.resource = NULL;
    record = PCVertexDeclarationFindSlot(&key, PCVertexDeclarationCachePC, count, &found);
    if (!found) {
        PC_VERTEX_ELEMENT elements[24];
        i32 normal;
        i32 tangent;
        i32 binormal;
        i32 diffuse;
        i32 specular;
        i32 texture_mode;
        i32 half_texture_mode;
        i32 weights;
        i32 colour2;
        i32 skinned;
        i32 instanced;
        i32 element_count;
        i32 offset;
        PCByteCopy(record + 1, record, (count - (record - PCVertexDeclarationCachePC)) * sizeof(*record));
        ++PCVertexDeclarationCountPC;
        if ((descriptor & 8) != 0 || (descriptor & 0x880000) != 0) {
            normal = 2;
        } else {
            normal = (descriptor >> 2) & 1;
        }
        if ((descriptor & 0x20) != 0 || (descriptor & 0x1000000) != 0) {
            tangent = 2;
        } else {
            tangent = (descriptor >> 4) & 1;
        }
        if ((descriptor & 0x80) != 0 || (descriptor & 0x2000000) != 0) {
            binormal = 2;
        } else {
            binormal = (descriptor >> 6) & 1;
        }
        diffuse = (descriptor >> 8) & 1;
        specular = (descriptor & 0x600) != 0;
        if ((descriptor >> 27) & 1) {
            texture_mode = 0;
        } else {
            texture_mode = (descriptor >> 11) & 7;
        }
        if ((descriptor >> 27) & 1) {
            half_texture_mode = (descriptor >> 11) & 7;
        } else {
            half_texture_mode = 0;
        }
        weights = (descriptor & 0x8000) ? 2 : (descriptor >> 14) & 1;
        colour2 = (descriptor & 0x20000) ? 2 : (descriptor >> 16) & 1;
        skinned = (descriptor >> 26) & 1;
        instanced = (descriptor >> 22) & 1;
        offset = 0;
        element_count = 0;
#define VERTEX_ELEMENT(type_value, usage_value, index_value, byte_count)                                                                   \
    elements[element_count].stream = 0;                                                                                                    \
    elements[element_count].offset = (u16)offset;                                                                                          \
    elements[element_count].type = type_value;                                                                                             \
    elements[element_count].method = 0;                                                                                                    \
    elements[element_count].usage = usage_value;                                                                                           \
    elements[element_count].usage_index = index_value;                                                                                     \
    ++element_count;                                                                                                                       \
    offset += byte_count
        VERTEX_ELEMENT(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0, 12);
        switch (normal) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0, 12);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4N, D3DDECLUSAGE_NORMAL, 0, 4);
                if (PCDeviceHardwareVertexProcessingPC <= 1) {
                    elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
                }
                break;
        }
        switch (tangent) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_TANGENT, 0, 12);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4N, D3DDECLUSAGE_TANGENT, 0, 4);
                if (PCDeviceHardwareVertexProcessingPC <= 1) {
                    elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
                }
                break;
        }
        switch (binormal) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_TANGENT, 1, 12);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4N, D3DDECLUSAGE_TANGENT, 1, 4);
                if (PCDeviceHardwareVertexProcessingPC <= 1) {
                    elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
                }
                break;
        }
        if (diffuse) {
            VERTEX_ELEMENT(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 0, 4);
        }
        if (specular) {
            VERTEX_ELEMENT(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 1, 4);
        }
        switch (texture_mode) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 0, 8);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 0, 16);
                break;
            case 3:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 0, 16);
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 1, 8);
                break;
            case 4:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 0, 16);
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 1, 16);
                break;
        }
        switch (half_texture_mode) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_2, D3DDECLUSAGE_TEXCOORD, 0, 4);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_4, D3DDECLUSAGE_TEXCOORD, 0, 8);
                break;
            case 3:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_4, D3DDECLUSAGE_TEXCOORD, 0, 8);
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_2, D3DDECLUSAGE_TEXCOORD, 1, 4);
                break;
            case 4:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_4, D3DDECLUSAGE_TEXCOORD, 0, 8);
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT16_4, D3DDECLUSAGE_TEXCOORD, 1, 8);
                break;
        }
        switch (weights) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_BLENDWEIGHT, 0, 8);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4N, D3DDECLUSAGE_BLENDWEIGHT, 0, 4);
                if (PCDeviceHardwareVertexProcessingPC <= 1) {
                    elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
                }
                break;
        }
        switch (colour2) {
            case 1:
                VERTEX_ELEMENT(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_BLENDINDICES, 0, 12);
                break;
            case 2:
                VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4, D3DDECLUSAGE_BLENDINDICES, 0, 4);
                if (PCDeviceHardwareVertexProcessingPC <= 1) {
                    elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
                }
                break;
        }
        if (skinned) {
            VERTEX_ELEMENT(D3DDECLTYPE_UBYTE4N, D3DDECLUSAGE_TEXCOORD, 6, 4);
            if (PCDeviceHardwareVertexProcessingPC <= 1) {
                elements[element_count - 1].type = D3DDECLTYPE_D3DCOLOR;
            }
            VERTEX_ELEMENT(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_TEXCOORD, 7, 0);
        }
        if (instanced) {
            elements[element_count].stream = 1;
            elements[element_count].offset = 0;
            elements[element_count].type = D3DDECLTYPE_FLOAT3;
            elements[element_count].method = 0;
            elements[element_count].usage = D3DDECLUSAGE_POSITION;
            elements[element_count].usage_index = 1;
            ++element_count;
        }
        elements[element_count].stream = 0xff;
        elements[element_count].offset = 0;
        elements[element_count].type = D3DDECLTYPE_UNUSED;
        elements[element_count].method = 0;
        elements[element_count].usage = 0;
        elements[element_count].usage_index = 0;
        record->key = descriptor;
        PCDeviceCreateVertexDeclaration(elements, &record->resource);
    }
    return record->resource;
#undef VERTEX_ELEMENT
}
