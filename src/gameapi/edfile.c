#include "saga.h"

static i32 edfile_media = 0;
static void *edfile_pakfile = 0;
i32 edfile_handle = -1;
i32 edfile_write_flag = 0;
char edfile_buffer[0x1000] = {0};
i32 edfile_buffer_pointer = 0;
i32 edfile_lock[2] = {0};
i32 edfile_readwrongendianess = 0;

extern i32 __cdecl NuFileWrite(NUFILE file, const void *data, i32 size);

void __cdecl EdFileSetMedia(i32 media) {
    edfile_media = media;
}

void __cdecl EdFileSetPakFile(void *pak) {
    edfile_pakfile = pak;
}

void __cdecl EdFileSetReadWrongEndianess(i32 value) {
    (void)value;
}

void __cdecl EdFileResetBuffers(void) {
    memset(edfile_buffer, 0, sizeof(edfile_buffer));
    edfile_buffer_pointer = 0;
    edfile_lock[1] = 0;
    edfile_lock[0] = 0;
}

__declspec(noinline) void __cdecl EdFileFillBuffer(void) {
    if (edfile_handle != -1) {
        memset(edfile_buffer, 0, sizeof(edfile_buffer));
        switch (edfile_media) {
            case 1:
                NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                edfile_buffer_pointer = 0;
                break;
            default:
                break;
        }
    }
}

i32 __cdecl EdFileOpen(char *filepath, i32 mode) {
    i32 item_handle;
    void *item;
    i32 item_size;

    if (edfile_handle != -1) {
        return 0;
    }
    EdFileResetBuffers();
    edfile_handle = -1;
    edfile_write_flag = 0;
    if (edfile_media != 1) {
        return 0;
    }
    if (mode != 0) {
        if (mode != 1) {
            return 0;
        }
        edfile_write_flag = 1;
        edfile_handle = NuFileOpen(filepath, 1);
        if (edfile_handle > 0) {
            return 1;
        }
        edfile_handle = -1;
        return 0;
    }
    if (edfile_pakfile != NULL) {
        item_handle = NuFilePakGetItem(edfile_pakfile, filepath);
        if (item_handle != 0) {
            NuFilePakGetItemInfo(edfile_pakfile, item_handle, &item, &item_size);
            edfile_handle = NuMemFileOpen(item, item_size, 0);
            if (edfile_handle > 0) {
                EdFileFillBuffer();
                return 1;
            }
            edfile_handle = -1;
        }
    }
    edfile_handle = NuFileOpen(filepath, 0);
    if (edfile_handle > 0) {
        EdFileFillBuffer();
        return 1;
    }
    edfile_handle = -1;
    return 0;
}

void __cdecl EdFileFlushBuffer(void) {
    if (edfile_handle != -1 && edfile_buffer_pointer != 0) {
        switch (edfile_media) {
            case 0:
                break;
            case 1:
                NuFileWrite(edfile_handle, edfile_buffer, edfile_buffer_pointer);
                edfile_buffer_pointer = 0;
                break;
        }
    }
}

i32 __cdecl EdFileClose(void) {
    edfile_pakfile = NULL;
    if (edfile_handle == -1)
        return 0;
    if (edfile_write_flag != 0)
        EdFileFlushBuffer();
    switch (edfile_media) {
        case 0:
            return 0;
        case 1:
            NuFileClose(edfile_handle);
            edfile_handle = -1;
            edfile_media = 0;
            return 1;
    }
    return 0;
}

void EdFileWrite(void *data, i32 len) {
    i32 to_write;

    while (len > 0) {
        to_write = MIN(len, 0x1000 - edfile_buffer_pointer);

        memcpy(edfile_buffer + edfile_buffer_pointer, data, to_write);

        edfile_buffer_pointer += to_write;
        len -= to_write;
        data = (void *)((size_t)data + to_write);

        if (edfile_buffer_pointer == 0x1000) {
            EdFileFlushBuffer();
        }
    }
}

void EdFileWriteFloat(f32 value) {
    f32 data = value;
    if (edfile_readwrongendianess)
        EdFileSwapEndianess32(&data);
    EdFileWrite(&data, 4);
}

void EdFileWriteInt(i32 value) {
    i32 data = value;
    if (edfile_readwrongendianess)
        EdFileSwapEndianess32(&data);
    EdFileWrite(&data, 4);
}

void EdFileWriteUnsignedInt(u32 value) {
    u32 data = value;
    if (edfile_readwrongendianess)
        EdFileSwapEndianess32(&data);
    EdFileWrite(&data, 4);
}

void EdFileWriteShort(i16 value) {
    i16 data = value;
    if (edfile_readwrongendianess)
        EdFileSwapEndianess16(&data);
    EdFileWrite(&data, 2);
}

void EdFileWriteUnsignedShort(u16 value) {
    u16 data = value;
    if (edfile_readwrongendianess)
        EdFileSwapEndianess16(&data);
    EdFileWrite(&data, 2);
}

void EdFileWriteChar(char value) {
    EdFileWrite(&value, 1);
}

void EdFileWriteUnsignedChar(u8 value) {
    EdFileWrite(&value, 1);
}

void EdFileWriteNuVec(NUVEC *value) {
    EdFileWriteFloat(value->x);
    EdFileWriteFloat(value->y);
    EdFileWriteFloat(value->z);
}

__declspec(noinline) void __cdecl EdFileRead(void *buffer, i32 length) {
    u8 *destination = (u8 *)buffer;
    i32 count;
    while (length > 0) {
        count = sizeof(edfile_buffer) - edfile_buffer_pointer;
        if (length < count) {
            count = length;
        }
        memcpy(destination, edfile_buffer + edfile_buffer_pointer, count);
        edfile_buffer_pointer += count;
        destination += count;
        length -= count;
        if (edfile_buffer_pointer == sizeof(edfile_buffer)) {
            if (edfile_handle != -1) {
                memset(edfile_buffer, 0, sizeof(edfile_buffer));
                if (edfile_media == 1) {
                    NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                    edfile_buffer_pointer = 0;
                }
            }
        }
    }
}

i32 __cdecl EdFileReadInt(void) {
    union {
        i32 value;
        struct {
            u8 first;
            u8 second;
            u8 third;
            u8 fourth;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.fourth;
        data.bytes.fourth = temporary;
        temporary = data.bytes.second;
        data.bytes.second = data.bytes.third;
        data.bytes.third = temporary;
    }
    return data.value;
}

f32 __cdecl EdFileReadFloat(void) {
    union {
        f32 value;
        struct {
            u8 first;
            u8 second;
            u8 third;
            u8 fourth;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.fourth;
        data.bytes.fourth = temporary;
        temporary = data.bytes.second;
        data.bytes.second = data.bytes.third;
        data.bytes.third = temporary;
    }
    return data.value;
}

i16 __cdecl EdFileReadShort(void) {
    union {
        i16 value;
        struct {
            u8 first;
            u8 second;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.second;
        data.bytes.second = temporary;
    }
    return data.value;
}

void __cdecl EdFileReadNuVec(NUVEC *out) {
    out->x = EdFileReadFloat();
    out->y = EdFileReadFloat();
    out->z = EdFileReadFloat();
}

i32 EdFileBackup(char *source, char *destination) {
    i64 remaining = NuFileSize(source);
    if (remaining <= 0)
        return 0;
    {
        NUFILE output = NuFileOpen(destination, NUFILE_WRITE);
        NUFILE input = NuFileOpen(source, NUFILE_READ);
        if (input > 0 && output > 0) {
            while (remaining > 0) {
                NuFileRead(input, edfile_buffer, MIN((i32)remaining, 0x1000));
                NuFileWrite(output, edfile_buffer, MIN((i32)remaining, 0x1000));
                remaining -= 0x1000;
            }
            NuFileClose(input);
            NuFileClose(output);
            return 1;
        }
        if (input > 0)
            NuFileClose(input);
        if (output > 0)
            NuFileClose(output);
    }
    return 0;
}

i32 FS_GetPadWithRepeat(FRAME_PAD_PC *pad, float repeat, float elapsed) {
    static i32 LastPad = 0;
    static float PadRepeat = 0;

    if (pad->digital_buttons != LastPad) {
        LastPad = pad->digital_buttons;
        PadRepeat = 0.5f;
        return LastPad;
    }

    PadRepeat -= elapsed;
    if (PadRepeat < 0.0f) {
        PadRepeat += repeat;
        return LastPad;
    }
    return 0;
}

void FS_SetFileSelPathFromName(char *name) {
    char file_name[256] = "";
    NuStrCpy(FS_Path, name);
    if (NuStrRChr(FS_Path, '.') != NULL) {
        char *separator = NuStrRChr(FS_Path, '\\');
        if (separator != NULL) {
            NuStrCpy(file_name, separator + 1);
            separator[1] = '\0';
        } else {
            NuStrCpy(file_name, name);
            FS_Path[0] = '\0';
        }
    }
    if (file_name[0] != '\0') {
        ((i32(*)(char *, char *, i32))(&NuFileExtConvert))(FS_LastFileName, file_name, 255);
    } else {
        FS_LastFileName[0] = '\0';
    }
}

extern const char EduiSourcePathPc[52];
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);
STATIC_ASSERT(offsetof(eduiitem_s, selection_group) == 0x18);
STATIC_ASSERT(offsetof(eduiitem_s, text) == 0x24);
STATIC_ASSERT(offsetof(eduimenu_s, callback) == 0x30);
STATIC_ASSERT(offsetof(eduimenu_s, title) == 0x34);

eduiitem_s *__cdecl eduiMenuAddItem(eduimenu_s *menu, eduiitem_s *item) {
    if (menu->last != NULL)
        menu->last->next = item;
    else
        menu->first = item;
    item->previous = menu->last;
    item->next = NULL;
    menu->last = item;
    menu->field_0c = menu->first;
    edui_last_item = item;
    menu->field_10 = NULL;
    return item;
}

eduimenu_s *__cdecl eduiMenuCreate(i32 x, i32 y, i32 width, i32 height, void *font, void *callback, char *title) {
    u32 bytes = sizeof(eduimenu_s) + 1;
    eduimenu_s *menu;
    if (title != NULL)
        bytes += NuStrLen(title);
    menu = (eduimenu_s *)NuAlloc(bytes, EduiSourcePathPc, 0x3dd);
    if (menu != NULL) {
        memset(menu, 0, bytes);
        menu->x = x;
        menu->y = y;
        menu->width = width;
        menu->height = height;
        menu->field_24 = -1;
        menu->field_28 = -1;
        menu->font = font;
        menu->callback = (EdUiMenuCallback)callback;
        menu->flags = 0;
        if (title != NULL) {
            menu->title = (char *)(menu + 1);
            NuStrCpy(menu->title, title);
        }
        active_menu = menu;
    }
    edui_last_item = NULL;
    return menu;
}

void __cdecl eduiMenuHighlight(eduimenu_s *menu, eduiitem_s *item) {
    eduiitem_s *entry;
    if (item->selection_group != 0) {
        for (entry = menu->first; entry != NULL; entry = entry->next) {
            if (entry == item) {
                if (entry->type == 3)
                    entry->highlighted = 1 - entry->highlighted;
                else
                    entry->highlighted = 1;
            } else if (entry->selection_group == item->selection_group) {
                entry->highlighted = 0;
            }
        }
    }
}

i32 __cdecl eduiItemSetText(eduiitem_s *item, char *text) {
    if (item->text != NULL && NuStrLen(item->text) < NuStrLen(text)) {
        NuFree(item->text, EduiSourcePathPc, 0x516);
        item->text = NULL;
    }
    if (item->text == NULL)
        item->text = (char *)NuAlloc(NuStrLen(text) + 1, EduiSourcePathPc, 0x51b);
    if (item->text != NULL) {
        NuStrCpy(item->text, text);
        return 1;
    }
    return 0;
}

void eduiMenuDestroyItems(eduimenu_s *menu) {
    if (menu) {
        eduicbMenuCloseAllexpanders(menu);
        while (menu->first) {
            eduiitem_s *next = menu->first->next;
            menu->first->destroy(menu, menu->first);
            menu->first = next;
        }
        menu->last = NULL;
        menu->selected = NULL;
        menu->field_0c = NULL;
        menu->field_10 = NULL;
    }
}

STATIC_ASSERT(offsetof(edui_interact_s, menu) == 0x10);
STATIC_ASSERT(offsetof(eduimenu_s, child) == 0x3c);
STATIC_ASSERT(offsetof(eduimenu_s, parent) == 0x40);

void __cdecl eduiMenuDestroy(eduimenu_s *menu) {
    if (menu != NULL) {
        if (eduiInteractLocked != NULL && eduiInteractLocked->menu == menu)
            eduiInteractLocked = NULL;
        if (processing_menu == menu)
            processing_menu = NULL;
        if (menu->parent != NULL) {
            if (active_menu == menu)
                active_menu = menu->parent;
            menu->parent->child = NULL;
        }
        if (default_active_menu == menu)
            default_active_menu = NULL;
        if (active_menu == menu)
            active_menu = NULL;
        if (menu->child != NULL)
            menu->child->parent = NULL;
        eduiMenuDestroyItems(menu);
        NuFree(menu, EduiSourcePathPc, 0x3d0);
    }
}

void __cdecl eduiMenuSortItemsByTxt(eduimenu_s *menu) {
    eduiitem_s *first = menu->first;
    if (first != NULL) {
        i32 sorted;
        do {
            eduiitem_s *item = first;
            sorted = 1;
            while (item->next != NULL && item->text != NULL && item->next->text != NULL) {
                if (item->type != 20 && (item->next->type == 20 || NuStrICmp(item->text, item->next->text) > 0)) {
                    if (item != menu->last) {
                        eduiitem_s *next = item->next;
                        eduiitem_s *previous = item->previous;
                        item->previous = next;
                        item->next = next->next;
                        if (item->next != NULL)
                            item->next->previous = item;
                        else
                            menu->last = item;
                        if (previous != NULL)
                            previous->next = next;
                        else
                            menu->first = next;
                        next->previous = previous;
                        next->next = item;
                        menu->field_0c = menu->first;
                        menu->field_10 = NULL;
                    }
                    if (item == first)
                        first = item->previous;
                    sorted = 0;
                } else {
                    item = item->next;
                }
            }
        } while (sorted == 0);
    }
}

void __cdecl cbInteractMenuScrollTo(eduimenu_s *menu, char *text) {
    i32 comparison;
    if (menu == NULL || text == NULL || text[0] == 0)
        return;
    comparison = menu->first != NULL && menu->first->text != NULL ? NuStrICmp(menu->first->text, text) : -1;
    menu->field_0c = menu->first;
    menu->selected = menu->first;
    while (menu->selected->next != NULL && comparison < 0) {
        menu->selected = menu->selected->next;
        if (menu->selected->text != NULL)
            comparison = NuStrICmp(menu->selected->text, text);
        else
            comparison = -1;
    }
    menu->field_10 = menu->selected;
}

static i32 eduicbProcessSel(eduimenu_s *menu, eduiitem_s *item, f32 unused2, FRAME_PAD_PC *pad) {
    edui_sel_s *selection = ((edui_sel_s *)(item));
    if (pad->digital_buttons_pressed & EDUI_CURSOR_PRIMARY) {
        eduiMenuHighlight(menu, item);
        if (selection->selected)
            selection->selected(menu, item, pad->digital_buttons);
    } else if ((pad->digital_buttons & EDUI_CURSOR_PRIMARY) && selection->held) {
        selection->held(menu, item, pad->digital_buttons);
    }
    return 0;
}

void eduicbCancelMessageMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edui_messagemenu);
    edui_messagemenu = NULL;
}

__declspec(noinline) void cbInteractMenuKeySelect(eduimenu_s *menu) {
    u32 modifiers;
    char text[2];
    i32 key = NuKeyGet(&modifiers);
    if (key >= 0) {
        text[0] = NuKeyToAscii(key, modifiers & 1);
        text[1] = 0;
        cbInteractMenuScrollTo(menu, text);
    }
}

void eduiMenuFitOnScreen(eduimenu_s *menu, i32 padding) {
    if (menu->field_24 < 0) {
        i32 saved = edui_donotdraw;
        edui_donotdraw = 1;
        eduiMenuRender(menu);
        edui_donotdraw = saved;
    }
    if (menu->x + menu->field_24 > 640 - padding)
        menu->x -= menu->field_24;
    if (menu->x < padding)
        menu->x = padding;
    if (menu->y + menu->field_28 > 448 - padding)
        menu->y -= menu->field_28;
    if (menu->y < padding)
        menu->y = padding;
}

i32 eduiMenuAttach(eduimenu_s *menu, eduimenu_s *child) {
    if (child->parent)
        return 0;
    while (menu->child)
        menu = menu->child;
    child->parent = menu;
    menu->child = child;
    eduiSetActiveMenu(child);
    return 1;
}

eduiitem_s *eduiItemCheckCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback,
                                char *text) {
    eduiitem_s *item = eduiItemSelCreate(data, colours, selected, group, callback, text);
    if (item) {
        item->type = 2;
        item->process = eduicbProcessSel;
        item->render = eduicbRenderCheck;
    }
    return item;
}

eduiitem_s *eduiItemToggleCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback,
                                 char *text) {
    eduiitem_s *item = eduiItemSelCreate(data, colours, selected, group, callback, text);
    if (item) {
        item->type = 3;
        item->process = eduicbProcessSel;
        item->render = eduicbRenderCheck;
    }
    return item;
}

edui_textpicker_s *eduiAddTextPickEntEx(eduimenu_s *menu, eduiitem_s *source, EdUiItemCallback callback) {
    struct edui_textpicker_s *picker;
    static eduimenu_s *tpent = 0;
    tpent = eduiMenuCreate(source->x + 10, source->y + 10, 180, 50, menu->font, NULL, ((char *)("Enter Text")));
    tpent->flags |= 2;
    picker = ((edui_textpicker_s *)(eduiItemTextPickCreate(((size_t)(source)), &ed_attr, callback, ((char *)("")))));
    eduiMenuAddItem(tpent, picker);
    eduiMenuAttach(menu, tpent);
    picker->editing = 1;
    eduiMenuFitWidth(tpent, 8);
    picker->cursor = NuStrLen(picker->value);
    return picker;
}

void cbgpcfgAdd(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = ((edui_gradient_pick_s *)(item->data_ptr));
    edui_gradient_node_s *stage = picker->selected_stage;
    f32 time = 0.0f;
    f32 hue = 0.0f;
    f32 saturation = 0.0f;
    f32 intensity = 1.0f;
    if (stage != NULL) {
        time = ((stage->next != NULL ? stage->next->time : 1.0f) + stage->time) * 0.5f;
        hue = stage->hue;
        saturation = stage->saturation;
        intensity = stage->value;
    }
    picker->selected_stage = eduiGradStageAdd(picker, time, hue, saturation, intensity);
    if (picker->add != NULL)
        picker->add(menu, item, value);
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

static i32 eduicbProcessGradPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    struct edui_gradient_pick_s *gradient;
    struct edui_gradient_node_s *stage;
    f32 lower;
    f32 upper;
    u8 right2;
    u8 left2;
    u8 left1;
    u8 right1;
    i32 decrease;
    i32 increase;
    (void)delta_time;
    gradient = ((edui_gradient_pick_s *)(item));
    if (gradient->changed)
        gradient->changed(menu, item, pad->digital_buttons);
    if (pad->digital_buttons_pressed & 0x40) {
        struct eduiitem_s *pick;
        static eduimenu_s *gpcfg = 0;
        gpcfg =
            eduiMenuCreate(menu->x + 10, menu->y + 40, 180, 250, menu->font, eduicbPickCFGCancel, "GradPick Adjust");
        if (!gpcfg)
            return 0;

        if (item->type == 8) {
            pick = eduiItemGreyPickCreate(((size_t)(item)), &ed_attr, cbgpcfgCPPress, "Colourpick");
            if (gradient->selected_stage)
                eduiItemColourPickSetHSV(((edui_colour_pick_s *)(pick)), gradient->selected_stage->hue,
                                         gradient->selected_stage->saturation, gradient->selected_stage->value);
        } else if (item->type == 9)
            pick = eduiItemSelCreate(((size_t)(item)), &ed_attr, 0, 0, cbgpcfgCPPress, "Edit");
        else {
            pick = eduiItemColourPickCreate(((size_t)(item)), &ed_attr, cbgpcfgCPPress, "Colourpick");
            if (gradient->selected_stage)
                eduiItemColourPickSetHSV(((edui_colour_pick_s *)(pick)), gradient->selected_stage->hue,
                                         gradient->selected_stage->saturation, gradient->selected_stage->value);
        }
        eduiMenuAddItem(gpcfg, pick);
        eduiMenuAddItem(gpcfg, eduiItemSelCreate(((size_t)(item)), &ed_attr, 0, 0, cbgpcfgAdd, "Add"));
        eduiMenuAddItem(gpcfg, eduiItemSelCreate(((size_t)(item)), &ed_attr, 0, 0, cbgpcfgDel, "Del"));
        eduiMenuAddItem(gpcfg, eduiItemSelCreate(((size_t)(item)), &ed_attr, 0, 0, cbgpcfgCopy, "Copy"));
        edui_last_item->type = item->type;
        eduiMenuAddItem(gpcfg,
                        eduiItemSelWithClipColourCreate(((size_t)(item)), &ed_attr, 0, 0, cbgpcfgPaste, "Paste"));
        edui_last_item->type = item->type;
        eduiMenuAttach(menu, gpcfg);
        return 0;
    }
    if (!gradient->selected_stage)
        gradient->selected_stage = gradient->first_stage;
    stage = gradient->selected_stage;
    if (!stage)
        return 0;
    if (pad->digital_buttons_pressed & 0x8000) {
        if (stage->previous)
            gradient->selected_stage = stage->previous;
        return 1;
    }
    if (pad->digital_buttons_pressed & 0x2000) {
        if (stage->next)
            gradient->selected_stage = stage->next;
        return 1;
    }
    if (pad->digital_buttons & 0xa000)
        return 1;
    lower = stage->previous ? stage->previous->time : 0.0f;
    upper = stage->next ? stage->next->time : 1.0f;
    right2 = pad->analog_r2;
    left2 = pad->analog_l2;
    left1 = pad->analog_l1;
    right1 = pad->analog_r1;
    decrease = left1 ? left1 : left2;
    increase = right1 ? right1 : right2;
    if (!(increase && !decrease)) {
        if (decrease) {
            f32 time = stage->time - ((f32)(decrease)) / 15360.0f;
            stage->time = lower > time ? lower : time;
            gradient->change_timer = 60;
        }
    } else {
        f32 time = ((f32)(increase)) / 15360.0f + stage->time;
        stage->time = upper < time ? upper : time;
        gradient->change_timer = 60;
    }
    return 0;
}

void eduiCreateMessageMenu(eduimenu_s *parent, char *message, i32 highlighted) {
    static const u32 initial_colours[4] = {0x800000c0u, 0x80ff0000, 0x80808080, 0x80404040};
    u32 colours[4];
    memcpy(colours, initial_colours, sizeof(colours));
    if (highlighted == 1)
        colours[0] = 0x8000c000u;
    edui_messagemenu = eduiMenuCreate(70, 70, 180, 250, parent->font, eduicbCancelMessageMenu, ((char *)("Message")));
    if (!edui_messagemenu)
        return;
    eduiMenuAddItem(edui_messagemenu, eduiItemSelCreate(1, colours, 0, 0, NULL, message));
    eduiMenuAttach(parent, edui_messagemenu);
    edui_messagemenu->x = parent->x + 10;
    edui_messagemenu->y = parent->y + 40;
}

void eduiCreate3LineMessageMenu(eduimenu_s *parent, char *first, char *second, char *third, i32 first_highlight,
                                i32 second_highlight, i32 third_highlight) {
    eduiiattr_s normal = {0x800000c0, 0x80ff0000, 0x80808080, 0x80404040};
    eduiiattr_s highlighted = {0x8000c000, 0x80ff0000, 0x80808080, 0x80404040};
    edui_messagemenu = eduiMenuCreate(70, 70, 300, 250, parent->font, eduicbCancelMessageMenu, ((char *)("Message")));
    if (edui_messagemenu) {
        if (first)
            eduiMenuAddItem(edui_messagemenu,
                            eduiItemSelCreate(1, first_highlight ? &highlighted : &normal, 0, 0, NULL, first));
        if (second)
            eduiMenuAddItem(edui_messagemenu,
                            eduiItemSelCreate(1, second_highlight ? &highlighted : &normal, 0, 0, NULL, second));
        if (third)
            eduiMenuAddItem(edui_messagemenu,
                            eduiItemSelCreate(1, third_highlight ? &highlighted : &normal, 0, 0, NULL, third));
        eduiMenuAttach(parent, edui_messagemenu);
        edui_messagemenu->x = parent->x + 10;
        edui_messagemenu->y = parent->y + 40;
    }
}

i32 eduiMenuProcessInput(eduimenu_s *menu, f32 delta_time, FRAME_PAD_PC *pad, i32 item_result) {
    u32 buttons;
    u32 pressed;
    u32 up_button;
    u32 down_button;
    i32 up;
    i32 down;
    i32 to_first;
    i32 page_up;
    i32 page_down;
    i32 to_last;
    static f32 up_rept = 0;
    static f32 down_rept = 0;
    cbInteractMenuKeySelect(menu);
    if (item_result || !pad)
        return 0;
    if (menu && menu->selected && menu->selected->input && !menu->selected->disabled && pad->digital_buttons) {
        if (menu->selected->input(menu, menu->selected, pad->digital_buttons, pad->digital_buttons_pressed))
            return 0;
    }
    buttons = pad->digital_buttons;
    pressed = pad->digital_buttons_pressed;
    up_button = (menu->flags & 2) ? 0x8000 : 0x1000;
    down_button = (menu->flags & 2) ? 0x2000 : 0x4000;
    up = 0;
    if (pressed & up_button) {
        up_rept = 1.0f / 3.0f;
        up = 1;
    }
    if ((buttons & up_button) && up_rept > 0.0f) {
        up_rept -= delta_time;
        if (up_rept <= 0.0f) {
            up_rept = 1.0f / 6.0f;
            up = 1;
        }
    }
    if (!(menu->flags & 6)) {
        if ((buttons & 0x8000) && !(menu->flags & 1)) {
            i32 distance = ((i32)(delta_time * 1500.0f));
            if (distance < 1)
                distance = 1;
            menu->x -= distance;
        }
        if ((buttons & 0x2000) && !(menu->flags & 1)) {
            i32 distance = ((i32)(delta_time * 1500.0f));
            if (distance < 1)
                distance = 1;
            menu->x += distance;
        }
    }
    down = 0;
    if (pressed & down_button) {
        down_rept = 1.0f / 3.0f;
        down = 1;
    }
    if ((buttons & down_button) && down_rept > 0.0f) {
        down_rept -= delta_time;
        if (down_rept <= 0.0f) {
            down_rept = 1.0f / 6.0f;
            down = 1;
        }
    }
    to_first = up && (buttons & 1);
    page_up = up && (buttons & 2);
    page_down = down && (buttons & 2);
    to_last = down && (buttons & 1);
    if (page_down && menu->field_10) {
        struct eduiitem_s *item;
        i32 count = 1;
        for (item = menu->field_0c; item && item != menu->field_10; item = item->next)
            ++count;
        while (menu->selected->next && count--) {
            menu->selected = menu->selected->next;
        }
        return 0;
    }
    if (page_up && menu->field_10) {
        struct eduiitem_s *item;
        i32 count = 1;
        for (item = menu->field_0c; item && item != menu->field_10; item = item->next)
            ++count;
        menu->selected = menu->field_0c;
        while (menu->selected->previous && count--) {
            menu->selected = menu->selected->previous;
            menu->field_0c = menu->selected;
        }
        return 0;
    }
    if (up && menu->selected && menu->selected->previous) {
        do {
            struct eduiitem_s *item;
            if (menu->selected == menu->field_0c)
                menu->field_0c = menu->selected->previous;
            item = menu->selected->previous;
            while (item->type == 18)
                item = item->previous;
            menu->selected = item;
        } while (menu->selected->previous && to_first);
        return 0;
    }
    if (down && menu->selected && menu->selected->next) {
        do {
            eduiitem_s *item = menu->selected->next;
            while (item->type == 18)
                item = item->next;
            menu->selected = item;
        } while (menu->selected->next && to_last);
        if (to_last) {
            struct eduiitem_s *item;
            i32 count = 1;
            for (item = menu->field_0c; item && item != menu->field_10; item = item->next)
                ++count;
            menu->field_0c = menu->selected;
            while (menu->field_0c->previous && count--)
                menu->field_0c = menu->field_0c->previous;
        }
        return 0;
    }
    eduiCheckForPadMenuCancel(menu, pad);
    return 0;
}

void cbSubMenu(eduimenu_s *menu, eduiitem_s *item) {
    eduiMenuAttach(menu, ((eduimenu_s *)(item->data_ptr)));
}

void cbmcfgCameraSpeed(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    f32 divisor = 512.0f / ((edui_slider_s *)item)->value;
    edcam_s *cam = gp_cam;
    f32 speed = 0.2f / divisor;
    cam->position_speed.x = speed;
    cam->position_speed.y = speed;
    cam->position_speed.z = speed;
    cam->distance_speed = 1.0f / divisor;
}

void cbmcfgCancel(eduimenu_s *menu, eduimenu_s *unused1) {
    eduiMenuDetach(menu);
}

void __cdecl eduiGradStageSetHSV(edui_gradient_node_s *stage, f32 hue, f32 saturation, f32 value);

edui_gradient_node_s *__cdecl eduiGradStageAdd(edui_gradient_pick_s *item, f32 time, f32 hue, f32 saturation,
                                               f32 value) {
    edui_gradient_node_s *stage =
        (edui_gradient_node_s *)NuAlloc(sizeof(edui_gradient_node_s), EduiSourcePathPc, 0xa37);
    edui_gradient_node_s *previous;
    edui_gradient_node_s *next;
    if (stage) {
        memset(stage, 0, sizeof(edui_gradient_node_s));
        previous = NULL;
        for (next = item->first_stage; next; next = next->next) {
            if (next->time > time)
                break;
            previous = next;
        }
        if (previous)
            previous->next = stage;
        else
            item->first_stage = stage;
        if (next)
            next->previous = stage;
        stage->time = time;
        stage->previous = previous;
        stage->next = next;
        eduiGradStageSetHSV(stage, hue, saturation, value);
        item->selected_stage = stage;
    }
    return stage;
}

void __cdecl eduicbItemGradPickDestroy(eduimenu_s *menu, eduiitem_s *item);
i32 __cdecl eduicbProcessGradPickItem(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad);

eduiitem_s *__cdecl eduiItemGradPickCreate(u32 data, void *colours, void *callback, char *text) {
    edui_gradient_pick_s *item = (edui_gradient_pick_s *)NuAlloc(sizeof(edui_gradient_pick_s), EduiSourcePathPc, 0xaa2);
    if (item) {
        memset(item, 0, sizeof(edui_gradient_pick_s));
        item->base.data = data;
        item->base.type = 7;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessGradPickItem;
        item->base.render = eduicbRenderGradPick;
        item->base.destroy = eduicbItemGradPickDestroy;
        eduiItemSetText(&item->base, text);
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->change_timer = 0;
        return &item->base;
    }
    return NULL;
}

eduiitem_s *eduiItemGreyGradPickCreate(size_t data, const void *colours, EdUiItemCallback callback, char *text) {
    eduiitem_s *item = eduiItemGradPickCreate(data, colours, callback, text);
    if (item)
        item->type = 8;
    return item;
}

void __cdecl eduiMenuAddItemFirst(eduimenu_s *menu, eduiitem_s *item) {
    if (menu->first)
        menu->first->previous = item;
    else
        menu->last = item;
    item->previous = NULL;
    item->next = menu->first;
    menu->first = item;
    menu->field_0c = item;
    edui_last_item = item;
    menu->field_10 = NULL;
}

i32 __cdecl eduicbInteractSel(edui_interact_s *interact) {
    edui_sel_s *item = (edui_sel_s *)interact->item;
    eduimenu_s *menu = interact->menu;
    i32(__cdecl * callback)(struct edui_interact_s *) = interact->callback;
    if (callback && (edui_cursor_buttons_db & 0x40))
        item->selected(menu, &item->base, edui_cursor_buttons);
    else if (callback && (edui_cursor_buttons & 0x40) && item->held)
        item->held(menu, &item->base, edui_cursor_buttons);
    return 0;
}

i32 numInteracts = 0;
edui_interact_s eduiInteracts[64] = {0};

STATIC_ASSERT(offsetof(edui_interact_s, item) == 0x14);

static void EduiClearInteractionsPc(eduimenu_s *menu, eduiitem_s *item);

void __cdecl eduicbItemDestroy(eduimenu_s *menu, eduiitem_s *item) {
    EduiClearInteractionsPc(menu, item);
    if (item->text != NULL) {
        NuFree(item->text, EduiSourcePathPc, 0x29e);
        item->text = NULL;
    }
    NuFree(item, EduiSourcePathPc, 0x2a2);
    menu->field_0c = menu->first;
}

void __cdecl eduicbItemSliderDestroy(eduimenu_s *menu, eduiitem_s *item) {
    edui_slider_s *slider = (edui_slider_s *)item;
    if (slider->format != NULL) {
        NuFree(slider->format, EduiSourcePathPc, 0xfbc);
        slider->format = NULL;
    }
    eduicbItemDestroy(menu, item);
}

static void EduiClearInteractionsPc(eduimenu_s *menu, eduiitem_s *item) {
    edui_interact_s *interact = eduiInteracts;
    i32 remaining = numInteracts;
    if (remaining > 0) {
        do {
            if (interact->menu == menu && interact->item == item) {
                interact->menu = NULL;
                interact->item = NULL;
            }
            ++interact;
        } while (--remaining != 0);
    }
}

eduiitem_s *__cdecl eduiItemSelCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text) {
    edui_sel_s *item = (edui_sel_s *)NuAlloc(sizeof(edui_sel_s), EduiSourcePathPc, 0x53b);
    if (item) {
        memset(item, 0, sizeof(edui_sel_s));
        item->base.data = data;
        item->base.type = 0;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.selection_group = group;
        item->base.highlighted = selected;
        item->base.text_alignment = 0x40;
        item->base.process = eduicbProcessSel;
        item->base.render = eduicbRenderSel;
        item->base.destroy = eduicbItemDestroy;
        item->interact = eduicbInteractSel;
        item->base.text = NULL;
        eduiItemSetText(&item->base, text ? text : "<no name>");
        item->selected = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->held = NULL;
        return &item->base;
    }
    return NULL;
}

void __cdecl eduiItemSliderSetValEx(edui_slider_s *item, f32 value, i32 reset_timer, i32 notify) {
    item->normalized_value = (value - item->minimum) / item->range;
    item->value = value;
    if (reset_timer)
        item->change_timer = 60;
    if (notify && item->changed)
        item->changed(NULL, &item->base, 0);
}

void __cdecl eduiItemSliderSetVal(edui_slider_s *item, f32 value) {
    eduiItemSliderSetValEx(item, value, 1, 1);
}

void __cdecl eduiItemSliderSetFmt(edui_slider_s *item, char *format) {
    if (item->format) {
        if (NuStrLen(item->format) >= NuStrLen(format))
            goto copy;
        NuFree(item->format, EduiSourcePathPc, 0xfa6);
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0xfa7);
    } else {
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0xfab);
    }
copy:
    NuStrCpy(item->format, format);
}

void __cdecl eduiItemSliderSetGranularity(edui_slider_s *item, f32 granularity) {
    item->granularity = granularity;
}

i32 __cdecl eduicbInteractSlider(edui_interact_s *interact) {
    edui_slider_s *slider = (edui_slider_s *)interact->item;
    f32 value = (edui_cursor_x - interact->x) / interact->width;
    value = value > 0.0f ? (value < 1.0f ? value : 1.0f) : 0.0f;
    eduiItemSliderSetVal(slider, value * slider->range);
    return (edui_cursor_buttons >> 6) & 1;
}

extern f32 edui_font_scale_x;
extern f32 edui_font_scale_y;

void __cdecl eduiSetFontScale(f32 x, f32 y) {
    edui_font_scale_x = x;
    edui_font_scale_y = y;
}

typedef struct EDUI_EXPANDER_PC {
    eduiitem_s base;
    i32(__cdecl *interact)(edui_interact_s *interact);
    eduiitem_s *first_child;
    eduiitem_s *last_child;
    u32 open : 1;
    u32 unknown_flags : 31;
} EDUI_EXPANDER_PC;
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, first_child) == 0x4c);
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, last_child) == 0x50);

void __cdecl eduicbItemExpanderClose(eduiitem_s *item) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    eduiitem_s *child;
    for (child = expander->first_child; child; child = child->next) {
        if (child->type == 19)
            eduicbItemExpanderClose(child);
        if (child == expander->last_child)
            break;
    }
    if (expander->first_child && expander->open) {
        expander->base.next = expander->last_child->next;
        if (expander->last_child->next)
            expander->last_child->next->previous = &expander->base;
        expander->first_child->previous = NULL;
        expander->open = 0;
    }
}

void __cdecl eduicbMenuCloseAllexpanders(eduimenu_s *menu) {
    eduiitem_s *item;
    for (item = menu->last; item; item = item->previous) {
        if (item->type == 19)
            eduicbItemExpanderClose(item);
    }
}

i32 __cdecl eduicbProcessColourPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad);
i32 __cdecl eduicbRenderColourPick(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height);
i32 __cdecl eduicbInteractColourPick(edui_interact_s *interact);

eduiitem_s *__cdecl eduiItemColourPickCreate(u32 data, void *colours, void *callback, char *text) {
    edui_colour_pick_s *item = (edui_colour_pick_s *)NuAlloc(sizeof(edui_colour_pick_s), EduiSourcePathPc, 0xb6c);
    if (item) {
        memset(item, 0, sizeof(edui_colour_pick_s));
        item->base.data = data;
        item->base.type = 10;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessColourPick;
        item->base.render = eduicbRenderColourPick;
        item->base.destroy = eduicbItemDestroy;
        item->interact = eduicbInteractColourPick;
        eduiItemSetText(&item->base, text);
        item->cursor_x = 0.5f;
        item->cursor_y = 0.5f;
        item->hue = 180.0f;
        item->value = 0.5f;
        item->saturation = 1.0f;
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        return &item->base;
    }
    return NULL;
}

eduiitem_s *__cdecl eduiItemSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 range,
                                         f32 value, char *text) {
    edui_slider_s *item = (edui_slider_s *)NuAlloc(sizeof(edui_slider_s), EduiSourcePathPc, 0xfd4);
    if (item) {
        memset(item, 0, sizeof(edui_slider_s));
        item->base.data = data;
        item->base.type = 6;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = group;
        item->base.process = eduicbProcessSlider;
        item->base.render = eduicbRenderSlider;
        item->base.destroy = eduicbItemSliderDestroy;
        item->interact = eduicbInteractSlider;
        eduiItemSetText(&item->base, text);
        item->minimum = minimum;
        item->range = range;
        item->granularity = 0.001f;
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->format = NULL;
        eduiItemSliderSetFmt(item, "(%1.03f)");
        eduiItemSliderSetVal(item, value);
        return &item->base;
    }
    return NULL;
}

eduiitem_s *__cdecl eduiItemSliderCreateInt(u32 data, void *colours, i32 group, void *callback, i32 minimum,
                                            i32 maximum, i32 value, char *text) {
    edui_slider_s *item =
        (edui_slider_s *)eduiItemSliderCreate(data, colours, group, callback, minimum, maximum, value, text);
    if (item) {
        eduiItemSliderSetFmt(item, "(%d)");
        item->granularity = 1.0f;
        item->base.render = eduicbRenderSliderInt;
    }
    return &item->base;
}

eduiitem_s *__cdecl eduiItemColourSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum,
                                               f32 range, f32 value, u8 red, u8 green, u8 blue, char *text) {
    edui_colour_slider_s *item =
        (edui_colour_slider_s *)NuAlloc(sizeof(edui_colour_slider_s), EduiSourcePathPc, 0x1a53);
    if (item) {
        memset(item, 0, sizeof(edui_colour_slider_s));
        item->base.base.data = data;
        item->base.base.type = 13;
        memcpy(item->base.base.colours, colours, sizeof(item->base.base.colours));
        item->base.base.text_alignment = 0x40;
        item->base.base.selection_group = group;
        item->base.base.process = eduicbProcessColourSlider;
        item->base.base.render = eduicbRenderColourSlider;
        item->base.base.destroy = eduicbItemSliderDestroy;
        eduiItemSetText(&item->base.base, text);
        item->base.minimum = minimum;
        item->base.range = range;
        item->base.granularity = 1.0f;
        item->base.changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->base.format = NULL;
        item->red = red;
        item->green = green;
        item->blue = blue;
        eduiItemSliderSetFmt(&item->base, "(%d)");
        eduiItemSliderSetVal(&item->base, value);
        return &item->base.base;
    }
    return NULL;
}

i32 __cdecl eduicbProcessTextPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad);
void __cdecl eduicbItemTextPickDestroy(eduimenu_s *menu, eduiitem_s *item);
extern char *edui_textrow_default[4];

void __cdecl eduiItemTextPickSetFmt(edui_textpicker_s *item, char *format) {
    if (item->format) {
        if (NuStrLen(item->format) >= NuStrLen(format))
            goto copy;
        NuFree(item->format, EduiSourcePathPc, 0x630);
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0x631);
    } else {
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0x635);
    }
copy:
    NuStrCpy(item->format, format);
}

eduiitem_s *__cdecl eduiItemTextPickCreate(u32 data, void *colours, void *callback, char *text) {
    edui_textpicker_s *item = (edui_textpicker_s *)NuAlloc(sizeof(edui_textpicker_s), EduiSourcePathPc, 0x63d);
    if (item) {
        memset(item, 0, sizeof(edui_textpicker_s));
        item->base.data = data;
        item->base.type = 1;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessTextPick;
        item->base.render = eduicbRenderTextPick;
        item->base.destroy = eduicbItemTextPickDestroy;
        eduiItemSetText(&item->base, text);
        item->max_length = 255;
        item->callback = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->format = NULL;
        item->cursor = 0;
        eduiItemTextPickSetFmt(item, "\"%s\"");
        memcpy(textrow, edui_textrow_default, sizeof(textrow));
        return &item->base;
    }
    return NULL;
}

void __cdecl eduiItemFilePickSetFmt(edui_file_pick_s *item, char *format) {
    if (item->format) {
        if (NuStrLen(item->format) >= NuStrLen(format))
            goto copy;
        NuFree(item->format, EduiSourcePathPc, 0x881);
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0x882);
    } else {
        item->format = (char *)NuAlloc(NuStrLen(format) + 1, EduiSourcePathPc, 0x886);
    }
copy:
    NuStrCpy(item->format, format);
}

eduiitem_s *__cdecl eduiItemFilePickCreate(u32 data, void *colours, void *callback, char *text) {
    edui_file_pick_s *item = (edui_file_pick_s *)NuAlloc(sizeof(edui_file_pick_s), EduiSourcePathPc, 0x950);
    if (item) {
        memset(item, 0, sizeof(edui_file_pick_s));
        item->base.data = data;
        item->base.type = 1;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessFilePick;
        item->base.render = eduicbRenderFilePick;
        item->base.destroy = eduicbItemFilePickDestroy;
        eduiItemSetText(&item->base, text);
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->format = NULL;
        item->name[0] = 0;
        item->directory[0] = 0;
        item->filename[0] = 0;
        item->compare_entries = NULL;
        eduiItemFilePickSetFmt(item, "\"%s\"");
        return &item->base;
    }
    return NULL;
}

i32 eduiMenuProcess(eduimenu_s *menu, f32 delta_time, FRAME_PAD_PC *pad) {
    processing_menu = menu;
    if (menu->child) {
        i32 result = eduiMenuProcess(menu->child, delta_time, pad);
        if (processing_menu) {
            if (result)
                eduiSetActiveMenu(menu->child ? menu->child : menu);
            else
                result = eduiProcessInteracts(menu, pad);
        }
        return result;
    }
    return eduiMenuProcessAux(menu, delta_time, pad);
}

char __cdecl EdFileReadChar(void) {
    char character;
    EdFileRead(&character, sizeof(character));
    return character;
}

u16 __cdecl EdFileReadUnsignedShort(void) {
    union {
        u16 value;
        struct {
            u8 first;
            u8 second;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.second;
        data.bytes.second = temporary;
    }
    return data.value;
}

u8 __cdecl EdFileReadUnsignedChar(void) {
    u8 character;
    EdFileRead(&character, sizeof(character));
    return character;
}

void __cdecl EdFileSwapEndianess16(void *value) {
    u8 *bytes = (u8 *)value;
    u8 temp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = temp;
}
void __cdecl EdFileSwapEndianess32(void *value) {
    u8 *bytes = (u8 *)value;
    u8 temp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = temp;
    temp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = temp;
}

u32 __cdecl EdFileReadUnsignedInt(void) {
    union {
        u32 value;
        struct {
            u8 first;
            u8 second;
            u8 third;
            u8 fourth;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.fourth;
        data.bytes.fourth = temporary;
        temporary = data.bytes.second;
        data.bytes.second = data.bytes.third;
        data.bytes.third = temporary;
    }
    return data.value;
}

extern struct PC_SCENE_MTL_VIEW *uimtls[5];

void __cdecl eduiInitMaterials(void) {
    uimtls[0] = (struct PC_SCENE_MTL_VIEW *)NuMtlCreate(1);
    ((NUMTL *)uimtls[0])->attribs.cull_mode = 2;
    ((NUMTL *)uimtls[0])->attribs.z_mode = 3;
    ((NUMTL *)uimtls[0])->opacity = 1.0f;
    ((NUMTL *)uimtls[0])->attribs.alpha_mode = 0;
    NuMtlUpdate(((NUMTL *)uimtls[0]));
    uimtls[1] = (struct PC_SCENE_MTL_VIEW *)NuMtlCreate(1);
    ((NUMTL *)uimtls[1])->diffuse_r = 0.2f;
    ((NUMTL *)uimtls[1])->diffuse_g = 0.2f;
    ((NUMTL *)uimtls[1])->diffuse_b = 0.2f;
    ((NUMTL *)uimtls[1])->attribs.cull_mode = 2;
    ((NUMTL *)uimtls[1])->attribs.z_mode = 3;
    ((NUMTL *)uimtls[1])->opacity = 0.999f;
    ((NUMTL *)uimtls[1])->attribs.alpha_mode = 3;
    NuMtlUpdate(((NUMTL *)uimtls[1]));
    uimtls[2] = (struct PC_SCENE_MTL_VIEW *)NuMtlCreate(1);
    ((NUMTL *)uimtls[2])->diffuse_r = 0.2f;
    ((NUMTL *)uimtls[2])->diffuse_g = 0.2f;
    ((NUMTL *)uimtls[2])->diffuse_b = 0.2f;
    ((NUMTL *)uimtls[2])->attribs.cull_mode = 2;
    ((NUMTL *)uimtls[2])->attribs.z_mode = 3;
    ((NUMTL *)uimtls[2])->opacity = 0.999f;
    ((NUMTL *)uimtls[2])->attribs.alpha_mode = 2;
    NuMtlUpdate(((NUMTL *)uimtls[2]));
    uimtls[3] = (struct PC_SCENE_MTL_VIEW *)NuMtlCreate(1);
    ((NUMTL *)uimtls[3])->diffuse_r = 0.2f;
    ((NUMTL *)uimtls[3])->diffuse_g = 0.2f;
    ((NUMTL *)uimtls[3])->diffuse_b = 0.2f;
    ((NUMTL *)uimtls[3])->attribs.cull_mode = 2;
    ((NUMTL *)uimtls[3])->attribs.z_mode = 3;
    ((NUMTL *)uimtls[3])->opacity = 0.999f;
    ((NUMTL *)uimtls[3])->attribs.alpha_mode = 1;
    NuMtlUpdate(((NUMTL *)uimtls[3]));
}

void __cdecl eduiMenuFitWidth(eduimenu_s *menu, i32 padding) {
    eduiitem_s *first = menu->first;
    eduiitem_s *item;
    i32 width = 0;
    i32 property_label_width = 0;
    i32 property_value_width = 0;
    i32 item_text_width = 0;
    i32 property_width;
    eduicbMenuOpenAllexpanders(menu);
    NuQFntPushCoordinateSystem(1);
    NuQFntSetScale((VUFNT *)edui_font, edui_font_scale_x, edui_font_scale_y);
    item_text_width = menu->title != NULL ? (i32)NuQFntPrintLenU((VUFNT *)edui_font, menu->title) >> 4 : 0;
    if (item_text_width > width)
        width = item_text_width;
    for (item = first; item != NULL; item = item->next) {
        switch (item->type) {
            case 1:
                item_text_width = ((i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3) * 10;
                break;
            case 2:
            case 3:
                if (item->text != NULL) {
                    i32 icon_width = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
                    i32 text_width = (i32)NuQFntPrintLenU((VUFNT *)edui_font, item->text) >> 4;
                    item_text_width = icon_width + text_width;
                }
                break;
            case 17: {
                edui_prop_s *property = (edui_prop_s *)item;
                i32 label_width = item->text != NULL ? (i32)NuQFntPrintLenU((VUFNT *)edui_font, item->text) >> 4 : 30;
                i32 value_width;
                if (label_width > property_label_width)
                    property_label_width = label_width;
                value_width = property->property_text != NULL
                                  ? (i32)NuQFntPrintLenU((VUFNT *)edui_font, property->property_text) >> 4
                                  : 30;
                if (value_width > property_value_width)
                    property_value_width = value_width;
                item_text_width = value_width;
                break;
            }
            case 6: {
                edui_slider_s *slider = (edui_slider_s *)item;
                char format[512];
                char text[512];
                NuStrCpy(format, item->text);
                NuStrCat(format, slider->format);
                NuSPrintf(text, format, -0.0);
                item_text_width = (i32)NuQFntPrintLenU((VUFNT *)edui_font, text) >> 4;
                break;
            }
            default:
                if (item->text != NULL)
                    item_text_width = (i32)NuQFntPrintLenU((VUFNT *)edui_font, item->text) >> 4;
                break;
        }
        if (item_text_width > width)
            width = item_text_width;
    }
    property_width = property_label_width + property_value_width + ((i32)NuQFntHeight((VUFNT *)edui_font) >> 3);
    if (property_width > width)
        width = property_width;
    for (item = menu->first; item != NULL; item = item->next) {
        if (item->type == 17)
            ((edui_prop_s *)item)->label_width = (f32)(property_label_width + 1);
    }
    menu->width = width + padding;
    NuQFntPopCoordinateSystem();
    eduicbMenuCloseAllexpanders(menu);
}

void __cdecl eduicbMenuOpenAllexpanders(eduimenu_s *menu) {
    eduiitem_s *item;
    for (item = menu->first; item != NULL; item = item->next) {
        if (item->type == 19) {
            EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
            if (expander->first_child != NULL && !expander->open) {
                expander->last_child->next = expander->base.next;
                if (expander->base.next != NULL)
                    expander->base.next->previous = expander->last_child;
                expander->base.next = expander->first_child;
                expander->first_child->previous = &expander->base;
                expander->open = 1;
            }
        }
    }
}

i32 EduiRenderWidthPc = 0;
i32 EduiSelectionMaterialPc = 0;
u32 EduiItemOutlineColourPc = 0x80ffffff;
u32 EduiMarkedItemOutlineColourPc = 0x0000ff00;

static void EduiDrawDisabledItemPc(eduiitem_s *item, i32 x, i32 y, i32 width, i32 height);

i32 __cdecl EduiRenderItemPc(eduiitem_s *item, eduimenu_s *menu, i32 x, i32 y, i32 width, i32 selected) {
    i32(__cdecl * render)(eduimenu_s *, eduiitem_s *, i32, i32, i32, i32) = item->render;
    i32 height;
    EduiRenderWidthPc = width;
    height = render(menu, item, x, y, width, selected);
    if (selected != 0) {
        i32 marked = (item->flags & 4) != 0;
        u32 colour = EduiMarkedItemOutlineColourPc;
        if (!marked)
            colour = EduiItemOutlineColourPc;
        if (!edui_donotdraw) {
            NuRndrLineRect2di(x << 4, (y << 3) + 8, EduiRenderWidthPc << 4, (height << 3) - 16, colour,
                              (NUMTL *)uimtls[EduiSelectionMaterialPc]);
        }
    }
    EduiDrawDisabledItemPc(item, x, y, width, height);
    return height;
}

static void EduiDrawDisabledItemPc(eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
    if (item->disabled && !edui_donotdraw) {
        i32 left = x << 4;
        i32 top = y << 3;
        i32 pixel_width = width << 4;
        i32 bottom = top + (height << 3);
        i32 quarter = left + pixel_width / 4;
        i32 half;
        i32 three_quarters;
        u32 colour = item->colours[0];
        NUMTL *material = (NUMTL *)uimtls[ui_bgmtl];
        NuRndrLine2di(left, top, quarter, bottom, colour, material);
        if (!edui_donotdraw) {
            material = (NUMTL *)uimtls[ui_bgmtl];
            colour = item->colours[0];
            half = left + pixel_width / 2;
            NuRndrLine2di(quarter, top, half, bottom, colour, material);
            if (!edui_donotdraw) {
                material = (NUMTL *)uimtls[ui_bgmtl];
                colour = item->colours[0];
                three_quarters = left + (pixel_width * 3) / 4;
                NuRndrLine2di(half, top, three_quarters, bottom, colour, material);
                if (!edui_donotdraw) {
                    material = (NUMTL *)uimtls[ui_bgmtl];
                    colour = item->colours[0];
                    NuRndrLine2di(three_quarters, top, left + pixel_width, bottom, colour, material);
                }
            }
        }
    }
}

void __cdecl eduicbItemTextPickDestroy(eduimenu_s *menu, eduiitem_s *item) {
    edui_textpicker_s *picker = (edui_textpicker_s *)item;
    if (picker->format != NULL) {
        NuFree(picker->format, EduiSourcePathPc, 0x623);
        picker->format = NULL;
    }
    eduicbItemDestroy(menu, item);
}

void __cdecl eduicbItemFilePickDestroy(eduimenu_s *menu, eduiitem_s *item) {
    edui_file_pick_s *picker = (edui_file_pick_s *)item;
    if (picker->format != NULL) {
        NuFree(picker->format, EduiSourcePathPc, 0x946);
        picker->format = NULL;
    }
    eduicbItemDestroy(menu, item);
}

void __cdecl FS_SortStrings(char *start, char *end, i32 mode) {
    char temporary[256];
    char *current;
    char *candidate;
    i32 direction;
    i32 current_length;
    i32 candidate_length;
    if ((u32)mode <= 1) {
        direction = mode == 0 ? 1 : -1;
        for (current = start; current != end;) {
            current_length = NuStrLen(current);
            candidate = current + NuStrLen(current) + 1;
            while (candidate != end) {
                candidate_length = NuStrLen(candidate);
                if (NuStrICmp(current + 7, candidate + 7) == direction) {
                    NuStrCpy(temporary, candidate);
                    PCByteCopy_candidate(current + candidate_length + 1, current, candidate - current);
                    PCByteCopy_candidate(current, temporary, candidate_length + 1);
                    current_length = candidate_length;
                }
                candidate += candidate_length + 1;
            }
            current += current_length + 1;
        }
    } else {
        u32 current_key;
        u32 candidate_key;
        i32 compare;
        direction = mode == 2 ? 1 : -1;
        for (current = start; current != end;) {
            current_key = (u32)(u8)current[6] - 'A';
            current_key = current_key * 13 + (u8)current[5] - 'A';
            current_key = current_key * 33 + (u8)current[4] - 'A';
            current_key = current_key * 25 + (u8)current[3] - 'A';
            current_key = current_key * 61 + (u8)current[2] - 'A';
            current_key = current_key * 61 + (u8)current[1] - 'A';
            current_length = NuStrLen(current);
            candidate = current + NuStrLen(current) + 1;
            while (candidate != end) {
                candidate_key = (u32)(u8)candidate[6] - 'A';
                candidate_key = candidate_key * 13 + (u8)candidate[5] - 'A';
                candidate_key = candidate_key * 33 + (u8)candidate[4] - 'A';
                candidate_key = candidate_key * 25 + (u8)candidate[3] - 'A';
                candidate_key = candidate_key * 61 + (u8)candidate[2] - 'A';
                candidate_key = candidate_key * 61 + (u8)candidate[1] - 'A';
                candidate_length = NuStrLen(candidate);
                compare = candidate_key < current_key ? 1 : (candidate_key > current_key ? -1 : 0);
                if (compare == direction) {
                    NuStrCpy(temporary, candidate);
                    PCByteCopy_candidate(current + candidate_length + 1, current, candidate - current);
                    PCByteCopy_candidate(current, temporary, candidate_length + 1);
                    current_key = candidate_key;
                    current_length = candidate_length;
                }
                candidate += candidate_length + 1;
            }
            current += current_length + 1;
        }
    }
}

char *FS_CurrentPos = FS_FileList;
char *FS_CurrentCursorPos = FS_FileList;
i32 FS_CursorLine = 0;

void __cdecl FS_MoveCursorUp(i32 steps) {
    if (steps > 0) {
        i32 top_number = FS_CurrentPosFileNum;
        char *cursor = FS_CurrentCursorPos;
        char *top = FS_CurrentPos;
        do {
            i32 top_length = 0;
            i32 cursor_length = 0;
            if (top != FS_FileList) {
                char *previous = top - 2;
                i32 length = 0;
                if (*previous == '\0') {
                    top_length = 1;
                } else {
                    for (;;) {
                        if (previous == FS_FileList) {
                            top_length = length + 2;
                            break;
                        }
                        --previous;
                        ++length;
                        if (*previous == '\0') {
                            top_length = length + 1;
                            break;
                        }
                    }
                }
            }
            if (cursor != FS_FileList) {
                char *previous = cursor - 2;
                i32 length = 0;
                if (*previous == '\0') {
                    cursor_length = 1;
                } else {
                    for (;;) {
                        if (previous == FS_FileList) {
                            cursor_length = length + 2;
                            break;
                        }
                        --previous;
                        ++length;
                        if (*previous == '\0') {
                            cursor_length = length + 1;
                            break;
                        }
                    }
                }
            }
            if (cursor == top) {
                top -= top_length;
                FS_CurrentPos = top;
                if (top_number != 0) {
                    --top_number;
                    FS_CurrentPosFileNum = top_number;
                }
            } else {
                --FS_CursorLine;
            }
            cursor -= cursor_length;
            --steps;
            FS_CurrentCursorPos = cursor;
        } while (steps > 0);
    }
}

void __cdecl FS_SetCursorToLastFileName(void) {
    char *entry = FS_FileList;
    i32 index;
    if (FS_LastFileName[0] == '\0')
        return;
    for (index = 0; index < FS_NumFiles; ++index) {
        if (NuStrICmp(entry + 7, FS_LastFileName) == 0) {
            FS_CurrentCursorPos = entry;
            FS_CurrentPosFileNum = index;
            FS_CurrentPos = entry;
            FS_CursorLine = 0;
            if (index > 0) {
                i32 file_count = FS_NumFiles;
                i32 end_index = (i32)((u32)index + 14);
                while (end_index > file_count) {
                    char *previous;
                    i32 previous_length;
                    i32 length = 0;
                    if (entry == FS_FileList)
                        break;
                    previous = entry - 2;
                    if (*previous == '\0') {
                        previous_length = 1;
                    } else {
                        for (;;) {
                            if (previous == FS_FileList) {
                                previous_length = length + 2;
                                break;
                            }
                            --previous;
                            ++length;
                            if (*previous == '\0') {
                                previous_length = length + 1;
                                break;
                            }
                        }
                    }
                    if (previous_length == 0)
                        break;
                    --FS_CurrentPosFileNum;
                    ++FS_CursorLine;
                    entry -= previous_length;
                    --index;
                    --end_index;
                    FS_CurrentPos = entry;
                    if (index <= 0)
                        break;
                }
            }
            return;
        }
        entry += NuStrLen(entry) + 1;
    }
}
