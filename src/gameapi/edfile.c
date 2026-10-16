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

__declspec(noinline) void __cdecl EdFileFlushBuffer(void) {
    if (edfile_handle != -1 && edfile_buffer_pointer != 0 && edfile_media == 1) {
        NuFileWrite(edfile_handle, edfile_buffer, edfile_buffer_pointer);
        edfile_buffer_pointer = 0;
    }
}

i32 __cdecl EdFileClose(void) {
    edfile_pakfile = NULL;
    if (edfile_handle == -1) {
        return 0;
    }
    if (edfile_write_flag != 0) {
        EdFileFlushBuffer();
    }
    if (edfile_media != 1) {
        return 0;
    }
    NuFileClose(edfile_handle);
    edfile_handle = -1;
    edfile_media = 0;
    return 1;
}

void EdFileWrite(void *data, i32 len) {
    i32 to_write;

    while (len > 0) {
        to_write = MIN(0x1000 - edfile_buffer_pointer, len);

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
    if (remaining > 0) {
        NUFILE output = NuFileOpen(destination, NUFILE_WRITE);
        NUFILE input = NuFileOpen(source, NUFILE_READ);
        if (output > 0 && input > 0) {
            while (remaining > 0) {
                i32 size = MIN(((i32)(remaining)), 0x1000);
                NuFileRead(input, edfile_buffer, size);
                NuFileWrite(output, edfile_buffer, size);
                remaining -= 0x1000;
            }
            NuFileClose(input);
            NuFileClose(output);
            return 1;
        }
        if (output > 0)
            NuFileClose(output);
        if (input > 0)
            NuFileClose(input);
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

void eduiSetActiveMenu(eduimenu_s *menu) {
    if (eduiGetUsingMenuFocus()) {
        if (menu) {
            while (menu->child)
                menu = menu->child;
            active_menu = menu;
        } else {
            menu = default_active_menu;
            if (menu)
                while (menu->child)
                    menu = menu->child;
            active_menu = menu;
        }
    }
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

static void cbgpcfgAdd(eduimenu_s *menu, eduiitem_s *item, u32 value) {
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

i32 eduiCheckForPadMenuCancel(eduimenu_s *menu, FRAME_PAD_PC *pad) {
    struct eduimenu_s *parent;
    if (!pad || !(pad->digital_buttons_pressed & 0x10))
        return 0;

    parent = menu->parent;
    if (!eduiGetUsingMenuFocus() && parent)
        eduiMenuDetach(menu);
    if (menu->callback)
        menu->callback(menu, parent);
    return 1;
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

static void cbSubMenu(eduimenu_s *menu, eduiitem_s *item) {
    eduiMenuAttach(menu, ((eduimenu_s *)(item->data_ptr)));
}

static void cbmcfgCancel(eduimenu_s *menu, eduimenu_s *unused1) {
    eduiMenuDetach(menu);
}

eduiitem_s *eduiItemGreyGradPickCreate(size_t data, const void *colours, EdUiItemCallback callback, char *text) {
    eduiitem_s *item = eduiItemGradPickCreate(data, colours, callback, text);
    if (item)
        item->type = 8;
    return item;
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
