#include "saga.h"
#include <string.h>

/* Level editor (Android editor/edlevelall.cpp). The retail unit is C++; the
 * EdDraw primitives that share it live in eddraw.c. */

extern "C" {
    extern eduimenu_s *edLevelActiveMenu;
    extern eduimenu_s *edLevelPinnedMenu;
    extern i32 pad_disabled;
}

struct EdStream {
    virtual ~EdStream() {
    }
    virtual i32 Eat(i32, i32) = 0;
    virtual i32 SerialiseBuffer(void *data, i32 size, i32 count) = 0;
    /* MSVC lays overloaded virtuals out in reverse declaration order, so
     * SerialiseString(char *, i32) lands in slot 5 (+0x14). */
    virtual i32 SerialiseString(char *text, i32 count) = 0;
    virtual i32 SerialiseString(char **text) = 0;
    virtual i32 SerialiseString(char **text, i32 count) = 0;
    virtual char const *BeginBlock(char const *name) = 0;
    virtual void EndBlock() = 0;

    i32 version;
    i32 mode;
    i32 swap_endianness;
    i32 unknown_10;
    void *memory_buffer;
    void *secondary_buffer;
    i32 flags;
};
STATIC_ASSERT(sizeof(EdStream) == 0x20);

struct PropertyTool {
    eduimenu_s *GetNextDefaultActiveMenu(eduimenu_s *menu);
};

void SerialiseVuVec(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 3);
}

void SerialiseVuMtx(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 16);
}

void SerialiseNuVec(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 3);
}

void SerialiseNuMtx(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 16);
}

void SerialiseString(EdStream &stream, void *data, i32 count) {
    stream.SerialiseString(static_cast<char *>(data), count);
}

void SerialiseStringAddr(EdStream &stream, void *data, i32 count) {
    stream.SerialiseString(static_cast<char **>(data), count);
}

void SerialiseColour3(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 3);
}

void eduiSetPinnedMenu(eduimenu_s *menu) {
    edLevelPinnedMenu = menu;
}

void edSetPadDisabled(i32 disabled) {
    pad_disabled = disabled;
}

i32 edGetPadDisabled() {
    return pad_disabled;
}

void cbEdLevelSetSliderFloat(eduimenu_s *, eduiitem_s *item, u32) {
    *static_cast<f32 *>(item->data_ptr) = reinterpret_cast<edui_slider_s *>(item)->value;
}

void cbEdLevelSetSliderInt(eduimenu_s *, eduiitem_s *item, u32) {
    *static_cast<i32 *>(item->data_ptr) = static_cast<i32>(reinterpret_cast<edui_slider_s *>(item)->value);
}

void cbEdLevelSetText(eduimenu_s *, eduiitem_s *item, u32) {
    strcpy(static_cast<char *>(item->data_ptr), reinterpret_cast<edui_textpicker_s *>(item)->value);
}

void cbEdLevelToggleInt(eduimenu_s *, eduiitem_s *item, u32) {
    i32 *value = static_cast<i32 *>(item->data_ptr);
    *value ^= 1;
    item->highlighted = *value;
}

eduimenu_s *PropertyTool::GetNextDefaultActiveMenu(eduimenu_s *menu) {
    if (menu == NULL)
        return edLevelActiveMenu;
    if (menu != edLevelActiveMenu)
        return NULL;
    return edLevelPinnedMenu;
}

eduimenu_s *GetMenuActiveChild(eduimenu_s *menu) {
    if (menu == NULL)
        return NULL;
    if (menu->child != NULL)
        return GetMenuActiveChild(menu->child);
    return menu;
}
