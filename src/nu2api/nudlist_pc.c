#include "saga.h"
#include <string.h>
#include <stdio.h>

/* nu3d display-list debug printers; the rest of the PC display-list backend is in
 * nu3d_platform_pc.cpp. */

void __cdecl PCSpecialEmptyHandler_candidate(void);

void __cdecl DisplayListPrintItem(NUDISPLAYLISTITEM *item, i32 index, i32 type_count, i32 *types, i32 file_handle) {
    char text[256];
    char detail[256];
    i32 selected;
    i32 type_index;
    text[0] = '\0';
    if (file_handle != 0) {
        if (item->id == 0)
            sprintf(text, "<font color = \"#00a000\">");
        else
            sprintf(text, "<font color = \"#0000a0\">");
    }
    sprintf(detail, "%d: ", index);
    NuStrCat(text, detail);
    switch (item->type) {
        case 0x80:
            sprintf(detail, "MTL              ");
            break;
        case 0x81:
            sprintf(detail, "TXTRLOAD         ");
            break;
        case 0x82:
            sprintf(detail, "GEOMCALL         ");
            break;
        case 0x83:
            sprintf(detail, "MTXLOAD          ");
            break;
        case 0x84:
            sprintf(detail, "TERMINATE        ");
            break;
        case 0x85:
            sprintf(detail, "MTL_CLIP         ");
            break;
        case 0x86:
            sprintf(detail, "CLUT             ");
            break;
        case 0x87:
            sprintf(detail, "DUMMY            ");
            break;
        case 0x88:
            sprintf(detail, "DLIST            ");
            break;
        case 0x8b:
            sprintf(detail, "DYNAMIC GEOMETRY ");
            break;
        case 0x8c:
            sprintf(detail, "TRANSFORM_REF    ");
            break;
        case 0x8d:
            sprintf(detail, "NEXT             ");
            break;
        case 0x8e:
            sprintf(detail, "END ITEM         ");
            break;
        case 0x8f:
            sprintf(detail, "FACEON           ");
            break;
        case 0x90:
            sprintf(detail, "FACEON_TRANSFORM ");
            break;
        case 0x93:
            sprintf(detail, "GEOMCALL2D       ");
            break;
        case 0x94:
            sprintf(detail, "LIGHTS           ");
            break;
        case 0x95:
            sprintf(detail, "DEBUG GEOMCALL   ");
            break;
        case 0x97:
            sprintf(detail, "LINECALL2D       ");
            break;
        case 0x9a:
            sprintf(detail, "CAMERA           ");
            break;
        case 0x9b:
            sprintf(detail, "FX SCREEN COPY   ");
            break;
        case 0x9c:
            sprintf(detail, "FX SUN FLARE     ");
            break;
        case 0x9d:
            sprintf(detail, "FX SCREEN FILTER ");
            break;
        case 0x9e:
            sprintf(detail, "FX SPEED BLUR    ");
            break;
        case 0x9f:
            sprintf(detail, "LINEBUFFER       ");
            break;
        case 0xa0:
            sprintf(detail, "GEOM CLIPPED     ");
            break;
        case 0xad:
            sprintf(detail, "INST_SURF_GEOM ITEM ");
            break;
        case 0xb0:
            sprintf(detail, "LIGHTMAP ITEM    ");
            break;
        default:
            if (DisplayListDebugPS(item, detail) != 0)
                sprintf(detail, "ERROR: Unknown item.");
            break;
    }
    NuStrCat(text, detail);
    sprintf(detail, "(%x)", (u32)item->next);
    NuStrCat(text, detail);
    selected = type_count == 0;
    for (type_index = 0; type_index < type_count; ++type_index) {
        if (types[type_index] == item->type) {
            selected = 1;
            break;
        }
    }
    if (nudlist_frame_active >= 1 && selected) {
        if (file_handle == 0) {
            PCSpecialEmptyHandler_candidate();
            return;
        }
        NuHtmlWrite(text);
        NuHtmlWrite("</br>\n");
        if (item->id == 0)
            NuHtmlWrite("<font color = \"#000000\">");
    }
    if (file_handle != 0)
        DisplayListPrintItemPS(item, file_handle);
}

/* Writes every item of a display list to the HTML capture. Bit 0 of flags follows
 * jump items (id 1); bit 1 selects the 0x84 terminator instead of 0x8e. Retail passes
 * items in EAX (a whole-program-optimised private ABI); this port uses cdecl. */
void __cdecl DisplayListPrintItemsPc(NUDISPLAYLISTITEM *item, u32 flags, i32 type_count, i32 *types, i32 file_handle) {
    i32 index = 0;
    u32 end;
    if (item == NULL)
        return;
    end = (flags & 2) ? 0x84 : 0x8e;
    do {
        DisplayListPrintItem(item, index, type_count, types, file_handle);
        if ((flags & 1) && item->id == 1)
            item = (NUDISPLAYLISTITEM *)item->next;
        else
            ++item;
        ++index;
    } while (item->type != end);
    DisplayListPrintItem(item, index, type_count, types, file_handle);
}
