#include "saga.h"
#pragma data_seg(".data$ed_attr")
static eduiiattr_s ed_attr = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
#pragma bss_seg(".bss$ed_cfg_menu")
static struct eduimenu_s *ed_cfg_menu = 0;
#pragma bss_seg(".bss$ed_curr")
static struct ed_module_s *ed_curr = 0;
#pragma bss_seg(".bss$ed_list")
static struct ed_module_s *ed_list = 0;
#pragma bss_seg(".bss$ed_main_menu")
static struct eduimenu_s *ed_main_menu = 0;
#pragma data_seg(".data$ed_main_menu_h")
static i32 ed_main_menu_h = 320;
#pragma data_seg(".data$ed_main_menu_w")
static i32 ed_main_menu_w = 180;
#pragma data_seg(".data$ed_main_menu_x")
static i32 ed_main_menu_x = 220;
#pragma data_seg(".data$ed_main_menu_y")
static i32 ed_main_menu_y = 50;
#pragma bss_seg(".bss$ed_module_active")
static i32 ed_module_active = 0;
#pragma data_seg(".data$edmain_auto_speed")
static i32 edmain_auto_speed = 1;
#pragma data_seg(".data$edmain_default_camera_speed")
static f32 edmain_default_camera_speed = 1.0f;
#pragma data_seg(".data$edmain_menu_scale")
static f32 edmain_menu_scale = 1.0f;
#pragma bss_seg(".bss$processing_menu")
static struct eduimenu_s *processing_menu = 0;
#pragma bss_seg(".bss$submenu1")
static struct eduimenu_s *submenu1 = 0;
#pragma bss_seg(".bss$testmenu0")
static struct eduimenu_s *testmenu0 = 0;

#define EDUI_MIN(a, b) ((a) < (b) ? (a) : (b))
#define EDUI_MAX(a, b) ((a) > (b) ? (a) : (b))

static i32 edfile_media = 0;
static void *edfile_pakfile = 0;
i32 edfile_handle = -1;
i32 edfile_write_flag = 0;
char edfile_buffer[0x1000] = {0};
i32 edfile_buffer_pointer = 0;
i32 edfile_lock[2] = {0};
i32 edfile_readwrongendianess = 0;
i32 EduiUsingMenuFocusPc = 0;

static eduimenu_s *EduiDeepestChildPc(eduimenu_s *menu);

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
            case 0:
                return;
            case 1:
                NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                edfile_buffer_pointer = 0;
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

extern i32 editor_return;
extern void *dir_list;
extern i32 edui_filepick_listsize;
extern NUCAMERA *EdMainCurrentCameraPc;
extern NUVEC *EdMainLocationVectorPc;
extern i32 edmain_cursor_enabled;
void __cdecl PcEditorCursorEnablePc(i32 enabled);

typedef struct EDUI_PRIM_VERTEX_PC {
    f32 x;
    f32 y;
    f32 z;
    u32 colour;
} EDUI_PRIM_VERTEX_PC;
STATIC_ASSERT(offsetof(EDUI_PRIM_VERTEX_PC, colour) == 0xc);

static void EduiPrimSetColourPc(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0)
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    else
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    ((EDUI_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void __cdecl edmainSetReturn(i32 result) {
    editor_return = result;
}

ed_module_s *__cdecl edmainCurrent(void) {
    if (ed_module_active != 0 && ed_curr != NULL)
        return ed_curr;
    return NULL;
}

i32 __cdecl edmainActivate(ed_module_s *module, i32 notify) {
    ed_module_s *entry;
    void(__cdecl * callback)(void);
    if (module != NULL) {
        for (entry = ed_list; entry != NULL; entry = entry->next) {
            if (entry == module) {
                ed_curr = module;
                callback = module->activate;
                ed_module_active = 1;
                if (callback != NULL && notify != 0)
                    callback();
                return 1;
            }
        }
    } else if (ed_curr != NULL) {
        callback = ed_curr->deactivate;
        if (callback != NULL && notify != 0)
            callback();
        ed_curr = NULL;
        ed_module_active = 0;
    }
    return 0;
}

void __cdecl edmainSetCamera(NUMTX *matrix) {
    NUCAMERA *camera = EdMainCurrentCameraPc;
    camera->mtx = *matrix;
    NuCameraSet(camera);
}

NUCAMERA *__cdecl edmainGetCameraPc(void) {
    return EdMainCurrentCameraPc;
}

void __cdecl edmainExtCamera(NUCAMERA *camera) {
    EdMainCurrentCameraPc = camera != NULL ? camera : edinternalcam;
}

void __cdecl edmainSetCursorEnabled(i32 enabled) {
    edmain_cursor_enabled = enabled;
    PcEditorCursorEnablePc(enabled);
}

i32 __cdecl edmainGetCursorEnabled(void) {
    return edmain_cursor_enabled;
}

void __cdecl edmainReadConfig(void) {
    NUFILE file;
    i32 block;
    ed_module_s *module;
    if (ed_levelfile[0] != 0) {
        file = NuFileOpen(ed_levelfile, 0);
        if (file != 0) {
            NuFileBeginBlkRead(file, 0x3032554e);
            while ((block = NuFileBeginBlkRead(file, 0)) != 0) {
                for (module = ed_list; module != NULL; module = module->next) {
                    if (module->block_id == (u32)block) {
                        if (module->read != NULL)
                            module->read(file);
                        break;
                    }
                }
                NuFileEndBlkRead(file);
            }
            NuFileEndBlkRead(file);
            NuFileClose(file);
        }
    }
}

void __cdecl cbMMRegSel(eduimenu_s *menu, eduiitem_s *selected, u32 buttons) {
    ed_module_s *module = (ed_module_s *)selected->data_ptr;
    ed_curr = module;
    if (module->reserved == NULL) {
        void(__cdecl * activate)(void) = module->activate;
        ed_module_active = 1;
        if (activate != NULL)
            activate();
    }
}

void __cdecl cbMMCancel(eduimenu_s *menu, eduimenu_s *parent) {
    edmainSetReturn(1);
}

void __cdecl cbMMReturnToApp(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edmainSetReturn(1);
}

eduimenu_s *__cdecl edGetMainMenu(void) {
    return ed_main_menu;
}

void __cdecl edmainSetLocVec(NUVEC *location) {
    EdMainLocationVectorPc = location;
}

/* Native 617f80/6. The pointer is zero-initialized engine/editor storage.
 * The neighboring native setter 617f70 has no verified direct caller. */
NUVEC *__cdecl edmainQueryLocVec(void) {
    return EdMainLocationVectorPc;
}

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
    u32 bytes;
    eduimenu_s *menu;
    if (title != NULL)
        bytes = NuStrLen(title) + sizeof(eduimenu_s) + 1;
    else
        bytes = sizeof(eduimenu_s) + 1;
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
        if (menu == processing_menu)
            processing_menu = NULL;
        if (menu->parent != NULL) {
            if (menu == active_menu)
                active_menu = menu->parent;
            menu->parent->child = NULL;
        }
        if (menu == default_active_menu)
            default_active_menu = NULL;
        if (menu == active_menu)
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
    if (EduiUsingMenuFocusPc != 0) {
        if (menu == NULL)
            active_menu = EduiDeepestChildPc(default_active_menu);
        else
            active_menu = EduiDeepestChildPc(menu);
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

i32 __cdecl eduiMenuDetach(eduimenu_s *menu) {
    if (menu->parent != NULL) {
        if (menu->child != NULL)
            eduiMenuDetach(menu->child);
        if (menu == active_menu)
            active_menu = menu->parent;
        menu->parent->child = NULL;
        menu->parent = NULL;
        return 1;
    }
    eduiSetActiveMenu(NULL);
    return 0;
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

i32 __cdecl eduicbProcessGreyPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad);
i32 __cdecl eduicbRenderSelWithClipColour(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 selected);
i32 __cdecl eduicbRenderGreyPick(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 selected);

static void eduicbProcessGradPick(eduimenu_s *menu, eduiitem_s *item) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    eduiitem_s *pick;
    static eduimenu_s *gpcfg = 0;
    gpcfg = eduiMenuCreate(menu->x + 10, menu->y + 40, 180, 250, menu->font, eduicbPickCFGCancel, "GradPick Adjust");
    if (!gpcfg)
        return;
    if (item->type == 9) {
        pick = eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgCPPress, "Edit");
    } else {
        i32 grey = item->type == 8;
        pick = eduiItemColourPickCreate((u32)item, &ed_attr, cbgpcfgCPPress, "Colourpick");
        if (grey && pick) {
            pick->type = 11;
            pick->process = eduicbProcessGreyPick;
            pick->render = eduicbRenderGreyPick;
        }
        if (gradient->selected_stage) {
            f32 hue = gradient->selected_stage->hue;
            f32 saturation = gradient->selected_stage->saturation;
            f32 value = gradient->selected_stage->value;
            edui_colour_pick_s *colour = (edui_colour_pick_s *)pick;
            colour->hue = hue;
            colour->saturation = saturation;
            colour->value = value;
            colour->cursor_x = (f32)(hue / 360.0);
            colour->cursor_y = value;
        }
    }
    eduiMenuAddItem(gpcfg, pick);
    eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgAdd, "Add"));
    eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgDel, "Del"));
    eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgCopy, "Copy"));
    edui_last_item->type = item->type;
    pick = eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgPaste, "Paste");
    if (pick)
        pick->render = eduicbRenderSelWithClipColour;
    eduiMenuAddItem(gpcfg, pick);
    edui_last_item->type = item->type;
    eduiMenuAttach(menu, gpcfg);
}

void eduiCreateMessageMenu(eduimenu_s *parent, char *message, i32 highlighted) {
    u32 colours[4] = {0x800000c0u, 0x80ff0000, 0x80808080, 0x80404040};
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

    parent = NULL;
    if (menu->parent != NULL)
        parent = menu->parent;
    if (EduiUsingMenuFocusPc == 0 && parent)
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

void __cdecl cbSetAutoSpeed(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edcam_s *cam;
    edmain_auto_speed = item->highlighted;
    cam = gp_cam;
    if (edmain_auto_speed != 0) {
        cam->auto_move_base = 0.15f;
        cam->auto_move_dist_scale = 0.2f;
        cam->auto_zoom_base = 0.01f;
        cam->auto_zoom_dist_scale = 0.1f;
    } else {
        cam->auto_move_base = 0.0f;
        cam->auto_move_dist_scale = 0.0f;
        cam->auto_zoom_base = 0.0f;
        cam->auto_zoom_dist_scale = 0.0f;
    }
}

void __cdecl cbMMEditorConfig(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    eduimenu_s *config = ed_cfg_menu;
    config->x = menu->x + 10;
    config->y = menu->y + 10;
    eduiMenuAttach(menu, config);
}

void cbmcfgCancel(eduimenu_s *menu, eduimenu_s *unused1) {
    eduiMenuDetach(menu);
}

void __cdecl edmainCreateMenus(void) {
    ed_module_s *module;
    NuQFntPushCoordinateSystem(1);
    NuQFntSetScale(system_qfont, edmain_menu_scale, edmain_menu_scale);
    ed_cfg_menu = eduiMenuCreate(220, 50, 180, 320, ed_fnt, cbmcfgCancel, "Editor Config");
    if (ed_cfg_menu != NULL) {
        eduiMenuAddItem(ed_cfg_menu, eduiItemSliderCreate(0, &ed_attr, 0, cbmcfgCameraSpeed, 1.0f, 20.0f,
                                                          edmain_default_camera_speed, "Camera Speed"));
        eduiMenuAddItem(ed_cfg_menu, eduiItemToggleCreate(0, &ed_attr, edmain_auto_speed, 1, cbSetAutoSpeed,
                                                          "Proportional Cursor Speed"));
        eduiMenuFitWidth(ed_cfg_menu, 8);
    }
    ed_main_menu = eduiMenuCreate(ed_main_menu_x, ed_main_menu_y, ed_main_menu_w, ed_main_menu_h, ed_fnt, cbMMCancel,
                                  "Editor Options");
    if (ed_main_menu != NULL) {
        for (module = ed_list; module != NULL; module = module->next) {
            eduiitem_s *item =
                eduiMenuAddItem(ed_main_menu, eduiItemSelCreate((u32)module, &ed_attr, 0, 1, cbMMRegSel, module->name));
            if (module == ed_curr)
                eduiMenuHighlight(ed_main_menu, item);
        }
        eduiMenuAddItem(ed_main_menu, eduiItemSelCreate(0, &ed_attr, 0, 0, cbMMEditorConfig, "Editor Config"));
        eduiMenuAddItem(ed_main_menu, eduiItemSelCreate(0, &ed_attr, 0, 0, cbMMReturnToApp, "Return To App"));
        eduiMenuFitWidth(ed_main_menu, 8);
    }
    NuQFntPopCoordinateSystem();
}

void __cdecl edmainClose(void) {
    if (ed_init != 0) {
        NuCameraDestroy(edinternalcam);
        while (ed_list != NULL) {
            void(__cdecl * close)(void) = ed_list->close;
            if (close != NULL)
                close();
            ed_list = ed_list->next;
        }
        if (ed_main_menu != NULL) {
            eduiMenuDestroy(ed_main_menu);
            ed_main_menu = NULL;
        }
        if (ed_cfg_menu != NULL) {
            eduiMenuDestroy(ed_cfg_menu);
            ed_cfg_menu = NULL;
        }
        ed_list = NULL;
        ed_curr = NULL;
        ed_init = 0;
    }
}

void __cdecl edmainRender(void) {
    if (ed_main_menu != NULL) {
        NuQFntPushPrintMode(2);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        NuQFntSetScale(system_qfont, edmain_menu_scale, edmain_menu_scale);
        eduiMenuRender(ed_main_menu);
        NuQFntPopCoordinateSystem();
        NuQFntPopPrintMode();
    }
}

f32 __cdecl edmainSetMenuScale(f32 scale) {
    f32 previous = edmain_menu_scale;
    edmain_menu_scale = scale;
    if (ed_main_menu != NULL)
        eduiMenuFitWidth(ed_main_menu, 8);
    return previous;
}

void __cdecl edmainInit(void *font, char *configuration_file) {
    ed_module_s *module;
    if (ed_init != 0)
        edmainClose();
    ed_fnt = font;
    ed_module_active = 0;
    for (module = ed_list; module != NULL; module = module->next) {
        if (module->init != NULL)
            module->init();
    }
    edinternalcam = NuCameraCreate();
    EdMainCurrentCameraPc = edinternalcam;
    eduiInitMaterials();
    if (edui_filepick_listsize != 0 && dir_list == NULL)
        dir_list = NuAlloc(edui_filepick_listsize * 0x118, EduiSourcePathPc, 0x821);
    edmainCreateMenus();
    if (configuration_file != NULL)
        NuStrCpy(ed_levelfile, configuration_file);
    else
        ed_levelfile[0] = 0;
    edmainReadConfig();
    ed_init = 1;
}

void __cdecl edmainInitEx(void *font, char *configuration_file, eduiiattr_s *colours, i32 x, i32 y, i32 width,
                          i32 height) {
    ed_attr = *colours;
    ed_main_menu_x = x;
    ed_main_menu_y = y;
    ed_main_menu_w = width;
    ed_main_menu_h = height;
    edmainInit(font, configuration_file);
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

void __cdecl eduiFlushInteracts(void) {
    numInteracts = 0;
}

extern f32 edui_cursor_y;
extern struct PC_SCENE_MTL_VIEW *uimtls[5];

void __cdecl eduiRenderInteracts(void) {
    edui_interact_s *hovered;
    edui_interact_s *interact;
    i32 count;
    i32 i;
    u32 colour;
    f32 edge;

    if (numInteracts <= 0)
        return;

    hovered = NULL;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 5, (NUMTL *)uimtls[0]);
    interact = eduiInteractLocked;
    if (interact != NULL) {
        count = 1;
    } else {
        count = numInteracts;
        interact = eduiInteracts;
    }
    for (i = 0; i < count; ++i, ++interact) {
        colour = NuRandInt();
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x, interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x + interact->width, interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x + interact->width, interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x + interact->width, interact->height + interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x + interact->width, interact->height + interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x, interact->height + interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x, interact->height + interact->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(interact->x, interact->y, 0.0f);
        if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
            edge = interact->x + interact->width;
            if (edge > edui_cursor_x) {
                edge = interact->height + interact->y;
                if (edge > edui_cursor_y)
                    hovered = interact;
            }
        }
    }
    NuPrim2DEnd();
    if (hovered != NULL) {
        NuPrim2DBegin(16, 5, (NUMTL *)uimtls[3]);
        colour = (NuRandInt() & 0x00ffffff) | 0x40000000;
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(hovered->x, hovered->y, 0.0f);
        EduiPrimSetColourPc(colour);
        NuPrim2DAddXYZ(hovered->width + hovered->x, hovered->height + hovered->y, 0.0f);
        NuPrim2DEnd();
    }
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
}

static i32 bShowCursor = 1;

void __cdecl eduiShowCursor(i32 show) {
    bShowCursor = show;
}

f32 edui_cursor_dx;
f32 edui_cursor_dy;
u32 edui_cursor_colour = 0xff000000;

void __cdecl eduiGetCursorCoords(f32 *x, f32 *y) {
    *x = edui_cursor_x / 640.0;
    *y = edui_cursor_y / 224.0;
}

void __cdecl eduiSetCursorCoords(f32 x, f32 y) {
    edui_cursor_x = x * 640.0;
    edui_cursor_y = y * 224.0;
}

void __cdecl eduiGetCursorDelta(f32 *x, f32 *y) {
    *x = edui_cursor_dx / 640.0;
    *y = edui_cursor_dy / 224.0;
}

/* 1 when the left stick is off centre, 2 for the right stick. */
i32 __cdecl eduiUsedAlgPad(FRAME_PAD_PC *pad) {
    if (pad != NULL) {
        if (pad->analog_left_x < 0x40 || pad->analog_left_x > 0xc0 || pad->analog_left_y < 0x40 ||
            pad->analog_left_y > 0xc0)
            return 1;
        if (pad->analog_right_x < 0x40 || pad->analog_right_x > 0xc0 || pad->analog_right_y < 0x40 ||
            pad->analog_right_y > 0xc0)
            return 2;
    }
    return 0;
}

/* The PC cursor follows the mouse elsewhere; this only keeps it on screen. */
void __cdecl EduiClampCursorPc(void) {
    if (edui_cursor_x < 0.0f)
        edui_cursor_x = 0.0f;
    else if (edui_cursor_x > 640.0f)
        edui_cursor_x = 640.0f;
    if (edui_cursor_y < 0.0f)
        edui_cursor_y = 0.0f;
    if (edui_cursor_y > 224.0)
        edui_cursor_y = 224.0f;
}

void __cdecl eduiSetCursorColour(u32 colour) {
    edui_cursor_colour = colour;
}

void __cdecl eduiRenderCursor(void) {
}

void __cdecl eduiInit(void) {
    eduiInitMaterials();
    if (edui_filepick_listsize != 0 && dir_list == NULL)
        dir_list = NuAlloc(edui_filepick_listsize * 0x118, EduiSourcePathPc, 0x821);
}

void __cdecl eduiSetFont(void *font) {
    edui_font = font;
}

i32 edui_camera_enabled = 1;
i32 eduiPropTextPos = -1;

i32 __cdecl eduiGetCameraEnabled(void) {
    return edui_camera_enabled;
}

void __cdecl eduiSetCameraEnabled(i32 enabled) {
    edui_camera_enabled = enabled;
}

void __cdecl eduiSetDefaultActiveMenu(eduimenu_s *menu) {
    default_active_menu = menu;
}

eduimenu_s *__cdecl eduiGetActiveMenu(void) {
    return active_menu;
}

void __cdecl PcGetMousePositionPc(f32 *x, f32 *y);
void __cdecl PcSetMousePositionPc(f32 x, f32 y);
u32 edui_cursor_buttons_old;
i32 edui_cursor_locked;

/* A stick nudge recentres the cursor, otherwise it follows the mouse; holding
 * key 0x38 locks it, parking the OS cursor where the editor cursor is. */
void __cdecl eduiProcessCursorDefault(f32 delta_time, FRAME_PAD_PC *pad) {
    u8 centred = 0;
    (void)delta_time;
    if (pad != NULL && eduiUsedAlgPad(pad) && !(pad->digital_buttons & 0x40)) {
        centred = 1;
        edui_cursor_x = 320.0f;
        edui_cursor_y = 112.0f;
    } else {
        edui_cursor_dx = -NuMouseReadXVel() * 0.1f;
        edui_cursor_dy = -NuMouseReadYVel() * 0.1f;
    }
    if (NuKeyboard(0x38)) {
        if (edui_cursor_locked == 0) {
            PcEditorCursorEnablePc(0);
            edui_cursor_locked = 1;
        }
    } else if (edui_cursor_locked == 1) {
        PcEditorCursorEnablePc(1);
        edui_cursor_locked = 0;
    }
    if (edui_cursor_locked) {
        PcSetMousePositionPc(edui_cursor_x / 640.0, edui_cursor_y / 224.0);
    } else if (!centred) {
        PcGetMousePositionPc(&edui_cursor_x, &edui_cursor_y);
        edui_cursor_x *= 640.0;
        edui_cursor_y *= 224.0;
    }
    EduiClampCursorPc();
    edui_cursor_buttons_old = edui_cursor_buttons;
    edui_cursor_buttons = 0;
    edui_cursor_buttons |= NuMouseReadButtons() == 1 ? 0x40 : 0;
    edui_cursor_buttons |= NuMouseReadButtons() == 2 ? 0x10 : 0;
    edui_cursor_buttons_db = (edui_cursor_buttons ^ edui_cursor_buttons_old) & edui_cursor_buttons;
}

void __cdecl eduiProcessCursor(f32 delta_time, FRAME_PAD_PC *pad) {
    eduiProcessCursorDefault(delta_time, pad);
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
    u32 hover : 1;
    u32 unknown_flags : 30;
    f32 button_size;
    f32 button_x;
    f32 button_y;
    i32 depth;
    void(__cdecl *changed)(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
} EDUI_EXPANDER_PC;
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, first_child) == 0x4c);
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, last_child) == 0x50);
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, button_size) == 0x58);
STATIC_ASSERT(offsetof(EDUI_EXPANDER_PC, depth) == 0x64);
STATIC_ASSERT(sizeof(EDUI_EXPANDER_PC) == 0x6c);

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

extern struct PC_SCENE_MTL_VIEW *uimtls[5];
extern i32 EduiSelectionMaterialPc;
extern f32 EduiScaleXPc;
extern f32 EduiScaleYPc;
i32 __cdecl NuVSPrintf(char *dest, const char *format, va_list args);
void __cdecl NuRndrLineRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material);

static void EduiExpanderOpenPc(EDUI_EXPANDER_PC *expander) {
    if (expander->first_child && !expander->open) {
        expander->last_child->next = expander->base.next;
        if (expander->base.next)
            expander->base.next->previous = expander->last_child;
        expander->base.next = expander->first_child;
        expander->first_child->previous = &expander->base;
        expander->open = 1;
    }
}

/* Is the editor cursor over the expander's open/close button? */
static i32 EduiExpanderOverButtonPc(EDUI_EXPANDER_PC *expander) {
    // MATCH-EXCEPTION: parameter copy, reversed declarations, a local for button_x and mirrored
    // compares; only this form gives retail's x87 operand order (U9/U20).
    EDUI_EXPANDER_PC *e = expander;
    f32 bottom = (e->button_y + e->button_size) * 0.5;
    f32 right = e->button_size + e->button_x;
    f32 top = e->button_y * 0.5;
    f32 x = e->button_x;
    if (edui_cursor_x >= x && edui_cursor_y >= top && right > edui_cursor_x && bottom > edui_cursor_y)
        return 1;
    return 0;
}

static i32 __cdecl eduicbProcessExpander(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    (void)delta_time;
    if (pad->digital_buttons_pressed & 0x40) {
        if (expander->changed)
            expander->changed(menu, item, 0);
        if (expander->first_child) {
            if (expander->open)
                eduicbItemExpanderClose(item);
            else
                EduiExpanderOpenPc(expander);
        }
    }
    return 0;
}

static i32 __cdecl eduicbInteractExpander(edui_interact_s *interact) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)interact->item;
    if (edui_cursor_buttons_db & 0x40) {
        if (expander->changed)
            expander->changed(interact->menu, &expander->base, 0);
        if (EduiExpanderOverButtonPc(expander) && expander->first_child) {
            if (expander->open)
                eduicbItemExpanderClose(&expander->base);
            else
                EduiExpanderOpenPc(expander);
        }
    }
    return 0;
}

static void __cdecl eduicbItemDestroyExpander(eduimenu_s *menu, eduiitem_s *item) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    if (expander->last_child)
        expander->last_child->next = NULL;
    while (expander->first_child) {
        eduiitem_s *next = expander->first_child->next;
        expander->first_child->destroy(menu, expander->first_child);
        expander->first_child = next;
    }
}

void __cdecl eduiIitemExpanderSetDepth(eduiitem_s *item, i32 depth) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    eduiitem_s *child;
    expander->depth = depth;
    for (child = expander->first_child; child; child = child->next) {
        if (child->type == 19)
            eduiIitemExpanderSetDepth(child, depth + 1);
        if (child->type == 17)
            ((edui_prop_s *)child)->depth = depth + 1;
        if (child == expander->last_child)
            break;
    }
}

void __cdecl eduiItemExpanderAddChild(eduiitem_s *item, eduiitem_s *child) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    child->previous = expander->last_child;
    if (expander->last_child)
        expander->last_child->next = child;
    expander->last_child = child;
    if (!expander->first_child)
        expander->first_child = child;
    child->next = NULL;
    if (child->type == 19)
        eduiIitemExpanderSetDepth(child, expander->depth + 1);
    if (child->type == 17)
        ((edui_prop_s *)child)->depth = expander->depth + 1;
}

eduimenu_s *__cdecl eduiGetTopLevelParent(eduimenu_s *menu) {
    if (menu != NULL) {
        eduimenu_s *parent = menu->parent;
        if (parent != NULL)
            return eduiGetTopLevelParent(parent);
    }
    return menu;
}

eduimenu_s *__cdecl eduiGetActiveMenuParent(void) {
    return eduiGetTopLevelParent(active_menu);
}

/* The menu itself or one of its open children when it holds the focus. */
static eduimenu_s *EduiMenuChainActivePc(eduimenu_s *menu) {
    if (menu == NULL)
        return NULL;
    if (menu == active_menu)
        return menu;
    return EduiMenuChainActivePc(menu->child);
}

static eduimenu_s *EduiDeepestChildPc(eduimenu_s *menu) {
    if (menu != NULL) {
        if (menu->child != NULL)
            return EduiDeepestChildPc(menu->child);
        return menu;
    }
    return NULL;
}

/* Screen-space helpers in 640x448 units; EduiScaleXPc/EduiScaleYPc convert to
 * the 16x/8x sub-pixel coordinates of the 2D renderer. */
static void EduiDrawRectPc(f32 x, f32 y, f32 width, f32 height, u32 colour, NUMTL *material) {
    if (edui_donotdraw == 0)
        NuRndrRect2di((i32)(x * EduiScaleXPc), (i32)(y * EduiScaleYPc), (i32)(width * EduiScaleXPc),
                      (i32)(height * EduiScaleYPc), colour, material);
}

/* Line primitives in 640x448 units; the coordinates are rounded to f32 first. */
#define EDUI_DRAW_LINE(x0, y0, x1, y1, colour, material)                                                               \
    do {                                                                                                               \
        f32 line_x0 = (x0), line_y0 = (y0), line_x1 = (x1), line_y1 = (y1);                                            \
        if (edui_donotdraw == 0)                                                                                       \
            NuRndrLine2di((i32)(line_x0 * EduiScaleXPc), (i32)(line_y0 * EduiScaleYPc), (i32)(line_x1 * EduiScaleXPc), \
                          (i32)(line_y1 * EduiScaleYPc), colour, material);                                            \
    } while (0)

#define EDUI_DRAW_LINE_RECT(x, y, width, height, colour, material)                                                     \
    do {                                                                                                               \
        f32 rect_x = (x), rect_y = (y), rect_width = (width), rect_height = (height);                                  \
        if (edui_donotdraw == 0)                                                                                       \
            NuRndrLineRect2di((i32)(rect_x * EduiScaleXPc), (i32)(rect_y * EduiScaleYPc),                              \
                              (i32)(rect_width * EduiScaleXPc), (i32)(rect_height * EduiScaleYPc), colour, material);  \
    } while (0)

static void eduiFntPrintClipEx(void *font, f32 x, f32 y, i32 alignment, f32 clip_x, f32 clip_width, char *format, ...) {
    if (edui_donotdraw == 0) {
        char text[1024];
        va_list args;
        i32 left = (i32)(x * EduiScaleXPc);
        i32 top = (i32)(y * EduiScaleYPc);
        i32 clip_left = (i32)(clip_x * EduiScaleXPc);
        i32 clip_right = (i32)(clip_width * EduiScaleXPc);
        i32 width;
        i32 text_x;
        char *end;
        NuQFntPushPrintMode(2);
        va_start(args, format);
        NuVSPrintf(text, format, args);
        va_end(args);
        width = (i32)NuQFntPrintLenU((VUFNT *)font, text);
        switch (alignment) {
            case 16:
                break;
            case 32:
                left -= width;
                break;
            case 64:
                left -= width / 2;
                break;
        }
        text_x = left;
        if (left < clip_left)
            text_x = clip_left;
        end = text + NuStrLen(text) - 1;
        while (width > clip_right && end > text) {
            *end-- = 0;
            width = (i32)NuQFntPrintLenU((VUFNT *)font, text);
        }
        NuQFntMove((VUFNT *)font, (f32)text_x, (f32)top, 0.0f);
        NuQFntPrintU((VUFNT *)font, text);
        NuQFntPopPrintMode();
    }
}

static i32 __cdecl eduicbRenderExpander(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
    EDUI_EXPANDER_PC *expander = (EDUI_EXPANDER_PC *)item;
    u32 button_colours[2];
    f32 item_x = (f32)x;
    f32 item_y = (f32)y;
    f32 item_width = (f32)width;
    f32 row_height;
    f32 baseline;
    f32 text_x;
    i32 depth;
    (void)height;
    button_colours[0] = 0xff000000;
    button_colours[1] = 0xffffffff;
    item->x = x;
    item->y = y;
    expander->hover = EduiExpanderOverButtonPc(expander);
    if (edui_donotdraw == 0) {
        NuQFntSet((VUFNT *)edui_font);
        if (edui_donotdraw == 0)
            NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
    }
    row_height = NuQFntHeight((VUFNT *)edui_font) * 1.25 * (1.0f / EduiScaleYPc);
    baseline = (NuQFntHeight((VUFNT *)edui_font) * 0.125 + NuQFntBaseline((VUFNT *)edui_font)) * (1.0f / EduiScaleYPc);
    if (menu != NULL && EduiMenuChainActivePc(menu))
        EduiDrawRectPc(item_x, item_y, item_width, row_height, item->colours[2 + item->highlighted],
                       (NUMTL *)uimtls[ui_bgmtl]);
    for (depth = 0; depth < expander->depth; depth++) {
        f32 guide_x = item_x + ((f32)depth + 0.5) * (row_height - 2.0);
        EDUI_DRAW_LINE(guide_x, item_y, guide_x, item_y + row_height, 0xff000000, (NUMTL *)uimtls[0]);
    }
    expander->button_size = row_height - 2.0f;
    expander->button_x = item_x + expander->depth * expander->button_size + 1.0f;
    expander->button_y = item_y + 1.0f;
    if (expander->first_child) {
        EDUI_DRAW_LINE_RECT(expander->button_x, expander->button_y, expander->button_size, expander->button_size,
                            0xff000000, (NUMTL *)uimtls[EduiSelectionMaterialPc]);
        EDUI_DRAW_LINE(expander->button_x + 2.0f, expander->button_y + expander->button_size * 0.5,
                       expander->button_x + expander->button_size - 2.0f,
                       expander->button_y + expander->button_size * 0.5, button_colours[expander->hover],
                       (NUMTL *)uimtls[0]);
        if (!expander->open)
            EDUI_DRAW_LINE(expander->button_x + expander->button_size * 0.5, expander->button_y + 2.0f,
                           expander->button_x + expander->button_size * 0.5,
                           expander->button_y + expander->button_size - 2.0f, button_colours[expander->hover],
                           (NUMTL *)uimtls[0]);
    }
    text_x = expander->button_x + expander->button_size + 2.0f;
    eduiFntPrintClipEx(edui_font, text_x, item_y + baseline, 16, text_x, item_width - (expander->button_size - 2.0f),
                       item->text);
    return (i32)row_height;
}

eduiitem_s *__cdecl eduiItemExpanderCreate(u32 data, const void *colours, void *callback, char *text) {
    EDUI_EXPANDER_PC *item = (EDUI_EXPANDER_PC *)NuAlloc(sizeof(EDUI_EXPANDER_PC), EduiSourcePathPc, 0x1473);
    if (item != NULL) {
        memset(item, 0, sizeof(EDUI_EXPANDER_PC));
        item->base.data = data;
        item->base.type = 19;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.highlighted = 0;
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessExpander;
        item->base.render = eduicbRenderExpander;
        item->base.destroy = eduicbItemDestroyExpander;
        item->interact = eduicbInteractExpander;
        eduiItemSetText(&item->base, text);
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        return &item->base;
    }
    return NULL;
}

/* Property items: a label, a divider the cursor can drag, an editable value
 * and an optional button (1 arrow, 2 up/down arrows, 3 colour swatch). */
static char eduiPropTextStore[256] = {0};
static char eduiPropTextEdit[256] = {0};
f32 EduiPropLabelWidthPc = 50.0f;
extern i32 eduiPropTextPos;
extern i32 edui_camera_enabled;
extern f32 edui_cursor_x;
extern void __cdecl NuKeyFlush(void);
extern void __cdecl EduiFntPrintScaledPc(void *font, f32 x, f32 y, i32 alignment, char *text);
extern void __cdecl NuRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);

i32 __cdecl eduiItemPropSetText(edui_prop_s *item, char *text) {
    if (item->property_text && NuStrLen(item->property_text) < NuStrLen(text)) {
        NuFree(item->property_text, EduiSourcePathPc, 0x1108);
        item->property_text = NULL;
    }
    if (item->property_text == NULL)
        item->property_text = (char *)NuAlloc(NuStrLen(text) + 1, EduiSourcePathPc, 0x110d);
    if (item->property_text) {
        NuStrCpy(item->property_text, text);
        return 1;
    }
    return 0;
}

/* Dragging the divider resizes the label column of every property item. */
static i32 EduiPropDragLabelPc(edui_prop_s *property, eduimenu_s *menu) {
    eduiitem_s *other;
    if (property->dragging_label) {
        property->label_width = edui_cursor_x - property->base.x;
        if (property->label_width < 5.0f)
            property->label_width = 5.0f;
        for (other = menu->first; other != NULL; other = other->next) {
            if (other->type == 17 && other != &property->base)
                ((edui_prop_s *)other)->label_width = property->label_width;
        }
        if (!(edui_cursor_buttons & 0x40))
            property->dragging_label = 0;
    }
    return 0;
}

/* Keyboard editing of the value text in eduiPropTextEdit. */
static i32 EduiPropKeyboardPc(edui_prop_s *property, eduimenu_s *menu) {
    u32 modifiers;
    i32 length;
    i32 key;
    char *cursor;
    char *insertion;
    if (!property->editing)
        return 0;
    length = NuStrLen(eduiPropTextEdit);
    if (eduiPropTextPos < 0)
        eduiPropTextPos = 0;
    if (eduiPropTextPos > length)
        eduiPropTextPos = length;
    key = NuKeyGet(&modifiers);
    if (key == -1)
        return 0;
    switch (key) {
        case 0x1c:
            eduiItemPropSetText(property, eduiPropTextEdit);
            if (property->changed)
                property->changed(menu, &property->base, 0);
        case 1:
            property->editing = 0;
            eduiPropTextPos = -1;
            return 0;
        case 0xcb:
            if (eduiPropTextPos > 0)
                eduiPropTextPos--;
            return 0;
        case 0xcd:
            if (eduiPropTextPos < length)
                eduiPropTextPos++;
            return 0;
        case 0x0e:
            if (eduiPropTextPos > 0) {
                eduiPropTextPos--;
                for (cursor = &eduiPropTextEdit[eduiPropTextPos]; *cursor; cursor++)
                    *cursor = cursor[1];
            }
            return 0;
        case 0xd3:
            if (eduiPropTextPos < length) {
                for (cursor = &eduiPropTextEdit[eduiPropTextPos]; *cursor; cursor++)
                    *cursor = cursor[1];
            }
            return 0;
        default: {
            char letter = NuKeyToAscii(key, modifiers & 1);
            if (letter) {
                insertion = &eduiPropTextEdit[eduiPropTextPos];
                eduiPropTextEdit[length + 1] = '\0';
                for (cursor = &eduiPropTextEdit[length]; cursor > insertion; cursor--)
                    *cursor = cursor[-1];
                eduiPropTextPos++;
                *cursor = letter;
            }
        }
    }
    return 0;
}

/* Runs the button callback while it is held (or the item is activated). */
static i32 EduiPropButtonPc(edui_prop_s *property, eduimenu_s *menu, FRAME_PAD_PC *pad) {
    edui_camera_enabled = 1;
    if (property->button_held || (property->base.type == 17 && (pad->digital_buttons & 0x40))) {
        if (property->button) {
            edui_camera_enabled = 0;
            property->button(menu, &property->base, edui_cursor_buttons);
            property->button_held = 0;
        }
        if (!(edui_cursor_buttons & 0x40))
            property->button_held = 0;
    }
    return 0;
}

static i32 __cdecl eduicbProcessProp(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    (void)delta_time;
    EduiPropDragLabelPc((edui_prop_s *)item, menu);
    EduiPropKeyboardPc((edui_prop_s *)item, menu);
    EduiPropButtonPc((edui_prop_s *)item, menu, pad);
    return 0;
}

static i32 EduiPropOverLabelPc(edui_interact_s *interact) {
    f32 edge;
    if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
        edge = ((edui_prop_s *)interact->item)->label_width + interact->x - 1.0;
        if (edui_cursor_x < edge) {
            edge = interact->height + interact->y;
            if (edui_cursor_y < edge)
                return 1;
        }
    }
    return 0;
}

static i32 EduiPropOverDividerPc(edui_interact_s *interact) {
    f32 edge = ((edui_prop_s *)interact->item)->label_width + interact->x - 1.0f;
    if (edui_cursor_x >= edge && edui_cursor_y >= interact->y) {
        edge = ((edui_prop_s *)interact->item)->label_width + interact->x + 1.0f;
        if (edui_cursor_x < edge) {
            edge = interact->height + interact->y;
            if (edui_cursor_y < edge)
                return 1;
        }
    }
    return 0;
}

static i32 EduiPropOverValuePc(edui_interact_s *interact) {
    edui_prop_s *property = (edui_prop_s *)interact->item;
    f32 edge = property->label_width + interact->x + 1.0;
    if (edui_cursor_x >= edge && edui_cursor_y >= interact->y) {
        if (edui_cursor_x < property->button_x) {
            edge = interact->height + interact->y;
            if (edui_cursor_y < edge)
                return 1;
        }
    }
    return 0;
}

static i32 EduiPropOverButtonPc(edui_prop_s *property) {
    return edui_cursor_x >= property->button_x && edui_cursor_y >= property->button_y * 0.5 &&
           edui_cursor_x < property->button_x + property->button_size &&
           edui_cursor_y < (property->button_y + property->button_size) * 0.5;
}

static i32 __cdecl eduicbInteractProp(edui_interact_s *interact) {
    eduimenu_s *menu = interact->menu;
    edui_prop_s *property = (edui_prop_s *)interact->item;
    i32 handled = 0;
    if (edui_cursor_buttons & 0x40) {
        if (!property->dragging_label && !property->button_held) {
            if (!property->editing) {
                if (EduiPropOverLabelPc(interact) && property->selected)
                    property->selected(menu, &property->base, 0);
                if (EduiPropOverDividerPc(interact)) {
                    property->dragging_label = 1;
                    property->divider_active = 1;
                }
                if (EduiPropOverValuePc(interact) && !property->editing) {
                    if (property->selected)
                        property->selected(menu, &property->base, 0);
                    NuKeyFlush();
                    property->editing = 1;
                    NuStrCpy(eduiPropTextStore, property->property_text);
                    NuStrCpy(eduiPropTextEdit, property->property_text);
                    eduiPropTextPos = NuStrLen(property->property_text);
                }
                if ((EduiPropOverButtonPc(property) || eduiInteractLocked) && property->button) {
                    property->button(menu, &property->base, 0);
                    if (property->button_latched)
                        handled = 1;
                }
            } else if (EduiPropOverValuePc(interact)) {
                property->editing = 1;
                eduiPropTextPos = -1;
                handled = 1;
            }
        }
    }
    return property->editing || property->dragging_label || property->button_held || handled;
}

static void __cdecl eduicbItemDestroyProp(eduimenu_s *menu, eduiitem_s *item) {
    edui_prop_s *property = (edui_prop_s *)item;
    if (item->text) {
        NuFree(item->text, EduiSourcePathPc, 0x1313);
        item->text = NULL;
    }
    if (property->property_text) {
        NuFree(property->property_text, EduiSourcePathPc, 0x1318);
        property->property_text = NULL;
    }
    NuFree(item, EduiSourcePathPc, 0x131c);
    menu->field_0c = menu->first;
}

static void EduiDrawLinePc(f32 x0, f32 y0, f32 x1, f32 y1, u32 colour, NUMTL *material) {
    if (edui_donotdraw == 0)
        NuRndrLine2di((i32)(x0 * EduiScaleXPc), (i32)(y0 * EduiScaleYPc), (i32)(x1 * EduiScaleXPc),
                      (i32)(y1 * EduiScaleYPc), colour, material);
}

static i32 __cdecl eduicbRenderProp(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
    edui_prop_s *property = (edui_prop_s *)item;
    char prefix[256];
    u32 divider_colours[2];
    u32 button_colours[2];
    i32 points[6];
    f32 draw_x = (f32)x;
    f32 draw_y = (f32)y;
    f32 draw_width = (f32)width;
    f32 row_height;
    f32 baseline;
    f64 indent_width;
    f32 indentation;
    f32 value_x;
    f32 clip_x;
    i32 depth;
    (void)height;
    item->x = x;
    item->y = y;
    divider_colours[0] = 0x0ff00000;
    divider_colours[1] = 0xffffffff;
    button_colours[0] = 0xff000000;
    button_colours[1] = 0xffffffff;
    if ((EduiPropOverButtonPc(property) && eduiInteractLocked == NULL) || property->button_held)
        property->button_hover = 1;
    else
        property->button_hover = 0;
    if (edui_donotdraw == 0)
        NuQFntSet((VUFNT *)edui_font);
    switch (item->unknown_10) {
        case 1:
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, 0xff000060);
            break;
        case 2:
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, 0xff006000);
            break;
        case 3:
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, 0xff600000);
            break;
        default:
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
            break;
    }
    row_height = NuQFntHeight((VUFNT *)edui_font) * 1.25 * (1.0f / EduiScaleYPc);
    baseline = (NuQFntHeight((VUFNT *)edui_font) * 0.125 + NuQFntBaseline((VUFNT *)edui_font)) * (1.0f / EduiScaleYPc);
    indent_width = row_height - 2.0;
    indentation = property->depth * indent_width;
    if (property->editing) {
        f32 text_width = NuQFntPrintLenU((VUFNT *)edui_font, eduiPropTextEdit) * (1.0f / EduiScaleXPc);
        f32 edit_width = draw_width - property->label_width - 4.0;
        f32 caret_x;
        text_width = EDUI_MAX(text_width, edit_width);
        EduiDrawRectPc(draw_x, draw_y, draw_width, row_height, item->colours[2 + item->highlighted],
                       (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0)
            NuRndrRect2di((i32)((draw_x + property->label_width + 2.0) * EduiScaleXPc),
                          (i32)((draw_y + 2.0) * EduiScaleYPc), (i32)(text_width * EduiScaleXPc),
                          (i32)((row_height - 4.0) * EduiScaleYPc), 0xffffffff, (NUMTL *)uimtls[0]);
        for (depth = 0; depth < property->depth; depth++) {
            f32 guide_x = draw_x + (depth + 0.5) * indent_width;
            if (edui_donotdraw == 0)
                NuRndrLine2di((i32)(guide_x * EduiScaleXPc), (i32)(draw_y * EduiScaleYPc),
                              (i32)(guide_x * EduiScaleXPc), (i32)((draw_y + row_height) * EduiScaleYPc), 0xff000000,
                              (NUMTL *)uimtls[0]);
        }
        if (edui_donotdraw == 0)
            NuRndrLine2di((i32)((draw_x + property->label_width) * EduiScaleXPc), (i32)(draw_y * EduiScaleYPc),
                          (i32)((draw_x + property->label_width) * EduiScaleXPc),
                          (i32)((draw_y + row_height) * EduiScaleYPc), divider_colours[property->divider_active],
                          (NUMTL *)uimtls[0]);
        eduiFntPrintClipEx(edui_font, draw_x + indentation, draw_y + baseline, 16, draw_x - indentation,
                           property->label_width, item->text);
        EduiFntPrintScaledPc(edui_font, draw_x + property->label_width + 2.0f, draw_y + baseline, 16, eduiPropTextEdit);
        NuStrNCpy(prefix, eduiPropTextEdit, eduiPropTextPos + 1);
        prefix[eduiPropTextPos + 1] = '\0';
        caret_x =
            draw_x + property->label_width + 2.0f + NuQFntPrintLenU((VUFNT *)edui_font, prefix) * (1.0f / EduiScaleXPc);
        if (edui_donotdraw == 0)
            NuRndrLine2di((i32)(caret_x * EduiScaleXPc), (i32)(draw_y * EduiScaleYPc), (i32)(caret_x * EduiScaleXPc),
                          (i32)((draw_y + row_height) * EduiScaleYPc), 0xff000000, (NUMTL *)uimtls[0]);
    } else {
        property->button_size = indent_width;
        property->button_x = draw_x + draw_width - (property->button_size + 1.0f);
        property->button_y = draw_y + 1.0f;
        if (menu != NULL && EduiMenuChainActivePc(menu))
            EduiDrawRectPc(draw_x, draw_y, draw_width, row_height, item->colours[2 + item->highlighted],
                           (NUMTL *)uimtls[ui_bgmtl]);
        EduiDrawLinePc(draw_x + property->label_width, draw_y, draw_x + property->label_width, draw_y + row_height,
                       divider_colours[property->divider_active], (NUMTL *)uimtls[0]);
        for (depth = 0; depth < property->depth; depth++) {
            f32 guide_x = draw_x + (depth + 0.5) * indent_width;
            if (edui_donotdraw == 0)
                NuRndrLine2di((i32)(guide_x * EduiScaleXPc), (i32)(draw_y * EduiScaleYPc),
                              (i32)(guide_x * EduiScaleXPc), (i32)((draw_y + row_height) * EduiScaleYPc), 0xff000000,
                              (NUMTL *)uimtls[0]);
        }
        eduiFntPrintClipEx(edui_font, draw_x + indentation, draw_y + baseline, 16, draw_x - indentation,
                           property->label_width, item->text);
        value_x = property->button_x - 1.0;
        clip_x = draw_x + property->label_width;
        eduiFntPrintClipEx(edui_font, value_x, draw_y + baseline, 32, clip_x, value_x - clip_x,
                           property->property_text);
        if (property->button_type == 3 && edui_donotdraw == 0)
            NuRndrRect2di((i32)(property->button_x * EduiScaleXPc), (i32)(property->button_y * EduiScaleYPc),
                          (i32)(property->button_size * EduiScaleXPc), (i32)(property->button_size * EduiScaleYPc),
                          property->button_colour, (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0)
            NuRndrLineRect2di((i32)(property->button_x * EduiScaleXPc), (i32)(property->button_y * EduiScaleYPc),
                              (i32)(property->button_size * EduiScaleXPc), (i32)(property->button_size * EduiScaleYPc),
                              0xff000000, (NUMTL *)uimtls[EduiSelectionMaterialPc]);
        if (property->button_type == 1) {
            points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleXPc);
            points[1] = (i32)((property->button_y + property->button_size * 0.1f) * EduiScaleYPc);
            points[2] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleXPc);
            points[3] = (i32)((property->button_y + property->button_size * 0.5) * EduiScaleYPc);
            points[4] = points[0];
            points[5] = (i32)((property->button_y + property->button_size * 0.9f) * EduiScaleYPc);
            if (edui_donotdraw == 0)
                NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], (NUMTL *)uimtls[0]);
        }
        if (property->button_type == 2) {
            points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleXPc);
            points[1] = (i32)((property->button_y + property->button_size * 0.45f) * EduiScaleYPc);
            points[2] = (i32)((property->button_x + property->button_size * 0.5) * EduiScaleXPc);
            points[3] = (i32)((property->button_y + property->button_size * 0.1f) * EduiScaleYPc);
            points[4] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleXPc);
            points[5] = points[1];
            if (edui_donotdraw == 0)
                NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], (NUMTL *)uimtls[0]);
            points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleXPc);
            points[1] = (i32)((property->button_y + property->button_size * 0.55f) * EduiScaleYPc);
            points[2] = (i32)((property->button_x + property->button_size * 0.5) * EduiScaleXPc);
            points[3] = (i32)((property->button_y + property->button_size * 0.9f) * EduiScaleYPc);
            points[4] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleXPc);
            points[5] = points[1];
            if (edui_donotdraw == 0)
                NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], (NUMTL *)uimtls[0]);
        }
    }
    return (i32)row_height;
}

eduiitem_s *__cdecl eduiItemPropCreateEx(u32 data, void *colours, void *selected, void *changed, void *button,
                                         i32 button_type, char *text, char *value, i32 extra_data) {
    edui_prop_s *item = (edui_prop_s *)NuAlloc(sizeof(edui_prop_s), EduiSourcePathPc, 0x1327);
    if (item) {
        memset(item, 0, sizeof(edui_prop_s));
        item->base.data = data;
        item->base.type = 17;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.highlighted = 0;
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessProp;
        item->base.render = eduicbRenderProp;
        item->base.destroy = eduicbItemDestroyProp;
        item->interact = eduicbInteractProp;
        eduiItemSetText(&item->base, text);
        eduiItemPropSetText(item, value);
        item->label_width = EduiPropLabelWidthPc;
        item->selected = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))selected;
        item->button_type = button_type;
        item->extra_data = extra_data;
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))changed;
        item->button = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))button;
        return &item->base;
    }
    return NULL;
}

eduiitem_s *__cdecl eduiItemPropCreate(u32 data, void *colours, void *selected, void *changed, void *button,
                                       i32 button_type, char *text, char *value) {
    return eduiItemPropCreateEx(data, colours, selected, changed, button, button_type, text, value, 0);
}

/* A filter item: typing into its value hides every menu item whose text
 * does not contain it; hidden items wait on the filter's own child list. */
typedef struct edui_filter_s {
    edui_prop_s prop;
    eduiitem_s *first_child;
    i32 reserved_84;
    u32 sorted;
} edui_filter_s;
STATIC_ASSERT(sizeof(edui_filter_s) == 0x8c);

void __cdecl eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item);
STATIC_ASSERT(offsetof(edui_filter_s, first_child) == 0x80);
STATIC_ASSERT(offsetof(edui_filter_s, sorted) == 0x88);

static i32 __cdecl eduicbInteractFilter(edui_interact_s *interact) {
    return eduicbInteractProp(interact);
}

static void __cdecl eduicbItemDestroyFilter(eduimenu_s *menu, eduiitem_s *item) {
    edui_filter_s *filter = (edui_filter_s *)item;
    eduiitem_s *child;
    eduiitem_s *next;
    if (filter->first_child != NULL) {
        do {
            child = filter->first_child;
            next = child->next;
            child->destroy(menu, child);
            filter->first_child = next;
        } while (next != NULL);
    }
    eduicbItemDestroyProp(menu, item);
}

static i32 __cdecl eduicbProcessFilter(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_filter_s *filter = (edui_filter_s *)item;
    i32 changed = 0;
    char *query;
    eduiitem_s *candidate;
    eduiitem_s *next;
    (void)delta_time;
    EduiPropDragLabelPc(&filter->prop, menu);
    EduiPropKeyboardPc(&filter->prop, menu);
    EduiPropButtonPc(&filter->prop, menu, pad);
    if (filter->prop.editing)
        query = eduiPropTextEdit;
    else
        query = filter->prop.property_text;
    if (*query) {
        for (candidate = menu->first; candidate != NULL; candidate = next) {
            next = candidate->next;
            if (candidate->type != 20 && candidate->type != 18 && !NuStrIStr(candidate->text, query)) {
                eduiMenuRemoveItem(menu, candidate);
                candidate->previous = NULL;
                candidate->next = filter->first_child;
                if (filter->first_child != NULL)
                    filter->first_child->previous = candidate;
                filter->first_child = candidate;
                changed = 1;
            }
        }
        for (candidate = filter->first_child; candidate != NULL; candidate = next) {
            next = candidate->next;
            if (NuStrIStr(candidate->text, query)) {
                if (filter->first_child == candidate)
                    filter->first_child = candidate->next;
                if (candidate->next != NULL)
                    candidate->next->previous = candidate->previous;
                if (candidate->previous != NULL)
                    candidate->previous->next = candidate->next;
                candidate->next = NULL;
                candidate->previous = NULL;
                eduiMenuAddItem(menu, candidate);
                changed = 1;
            }
        }
    } else {
        for (candidate = filter->first_child; candidate != NULL; candidate = next) {
            next = candidate->next;
            if (filter->first_child == candidate)
                filter->first_child = candidate->next;
            if (candidate->next != NULL)
                candidate->next->previous = candidate->previous;
            if (candidate->previous != NULL)
                candidate->previous->next = candidate->next;
            candidate->next = NULL;
            candidate->previous = NULL;
            eduiMenuAddItem(menu, candidate);
            changed = 1;
        }
    }
    if (changed || !(filter->sorted & 1)) {
        eduiMenuSortItemsByTxt(menu);
        filter->sorted |= 1;
    }
    return 0;
}

static i32 __cdecl eduicbRenderFilter(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
    return eduicbRenderProp(menu, item, x, y, width, height);
}

eduiitem_s *__cdecl eduiItemFilterCreate(u32 data, void *colours, char *text, char *value) {
    edui_filter_s *item = (edui_filter_s *)NuAlloc(sizeof(edui_filter_s), EduiSourcePathPc, 0x151a);
    if (item) {
        memset(item, 0, sizeof(edui_filter_s));
        item->prop.base.data = data;
        item->prop.base.type = 20;
        memcpy(item->prop.base.colours, colours, sizeof(item->prop.base.colours));
        item->prop.base.highlighted = 0;
        item->prop.base.text_alignment = 0x40;
        item->prop.base.selection_group = 0;
        item->prop.base.process = eduicbProcessFilter;
        item->prop.base.render = eduicbRenderFilter;
        item->prop.base.destroy = eduicbItemDestroyFilter;
        item->prop.interact = eduicbInteractFilter;
        eduiItemSetText(&item->prop.base, text);
        eduiItemPropSetText(&item->prop, value);
        item->prop.label_width = EduiPropLabelWidthPc;
        return &item->prop.base;
    }
    return NULL;
}

/* Analog pad rate for the editor: the left stick moves coarsely, the right
 * stick finely, while the cursor button (bit 6) is held. */
f32 __cdecl eduiGetAnalougePadValue(FRAME_PAD_PC *pad) {
    f32 value = 0.0f;
    if (pad != NULL && (pad->digital_buttons & 0x40)) {
        if (pad->analog_left_x > 192)
            value = (pad->analog_left_y - 128.0) * 0.01f;
        if (pad->analog_left_x < 64)
            value = (128.0 - pad->analog_left_y) * -0.01f;
        if (pad->analog_right_x > 192)
            value = (pad->analog_right_y - 128.0) * 0.001f;
        if (pad->analog_right_x < 64)
            value = (128.0 - pad->analog_right_y) * -0.001f;
    }
    return value;
}

void __cdecl eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item) {
    if (menu->first == item)
        menu->first = item->next;
    if (menu->last == item)
        menu->last = item->previous;
    if (menu->selected == item)
        menu->selected = item->next ? item->next : item->previous;
    menu->field_0c = menu->first;
    menu->field_10 = NULL;
    if (item->next)
        item->next->previous = item->previous;
    if (item->previous)
        item->previous->next = item->next;
    item->next = NULL;
    item->previous = NULL;
}

void __cdecl eduiMenuSetAttr(eduimenu_s *menu, const u32 *colours) {
    eduiitem_s *item;
    for (item = menu->first; item; item = item->next) {
        item->colours[0] = colours[0];
        item->colours[1] = colours[1];
        item->colours[2] = colours[2];
        item->colours[3] = colours[3];
    }
    if (menu->child)
        eduiMenuSetAttr(menu->child, colours);
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

i32 __cdecl eduicbRenderTexturePick(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height);

/* The left stick pans the selected texture corner (scaled by the zoom), the
 * shoulder buttons zoom, and the face buttons snap, switch corners or apply. */
static i32 __cdecl eduicbProcessTexturePick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_texture_pick_s *pick = (edui_texture_pick_s *)item;
    i32 stick_x = pad->analog_left_x - 128;
    i32 stick_y = pad->analog_left_y - 128;
    i32 left;
    i32 right;
    (void)delta_time;
    if (stick_x > 0) {
        if (stick_x < 32)
            stick_x = 0;
        else
            stick_x = (stick_x - 32) * 255 / 223;
    } else if (stick_x > -32) {
        stick_x = 0;
    } else {
        stick_x = (stick_x + 32) * 255 / 223;
    }
    if (stick_y > 0) {
        if (stick_y < 32)
            stick_y = 0;
        else
            stick_y = (stick_y - 32) * 255 / 223;
    } else if (stick_y > -32) {
        stick_y = 0;
    } else {
        stick_y = (stick_y + 32) * 255 / 223;
    }
    pick->uv_x[pick->selected_corner] += stick_x / (pick->zoom * 8192.0);
    pick->uv_y[pick->selected_corner] += stick_y / (pick->zoom * 8192.0);
    pick->uv_x[pick->selected_corner] = EDUI_MIN(1.0f, EDUI_MAX(0.0f, pick->uv_x[pick->selected_corner]));
    pick->uv_y[pick->selected_corner] = EDUI_MIN(1.0f, EDUI_MAX(0.0f, pick->uv_y[pick->selected_corner]));
    left = pad->analog_l2;
    right = pad->analog_r2;
    if (pad->analog_l1)
        left = pad->analog_l1;
    if (pad->analog_r1)
        right = pad->analog_r1;
    if (left) {
        pick->zoom = EDUI_MAX(1.0f, pick->zoom - left / 4192.0);
        return 1;
    }
    if (right) {
        pick->zoom = EDUI_MIN(16.0f, right / 4192.0 + pick->zoom);
        return 1;
    }
    if (pad->digital_buttons_pressed & 0x100) {
        pick->uv_x[pick->selected_corner] = (i32)(pick->uv_x[pick->selected_corner] * 256.0 + 0.5) * (1.0 / 256.0);
        pick->uv_y[pick->selected_corner] = (i32)(pick->uv_y[pick->selected_corner] * 256.0 + 0.5) * (1.0 / 256.0);
    }
    if (pad->digital_buttons_pressed & 0x80)
        pick->selected_corner = pick->selected_corner != 1;
    if ((pad->digital_buttons_pressed & 0x40) && pick->changed)
        pick->changed(menu, item, pad->digital_buttons);
    return 0;
}

eduiitem_s *__cdecl eduiItemTexturePickCreate(u32 data, void *colours, void *callback, char *text) {
    edui_texture_pick_s *item = (edui_texture_pick_s *)NuAlloc(sizeof(edui_texture_pick_s), EduiSourcePathPc, 0xce1);
    if (item) {
        memset(item, 0, sizeof(edui_texture_pick_s));
        item->base.data = data;
        item->base.type = 12;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessTexturePick;
        item->base.render = eduicbRenderTexturePick;
        item->base.destroy = eduicbItemDestroy;
        eduiItemSetText(&item->base, text);
        item->uv_x[0] = 0.4f;
        item->uv_y[0] = 0.4f;
        item->changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->uv_x[1] = 0.5f;
        item->selected_corner = 0;
        item->uv_y[1] = 0.5f;
        item->zoom = 1.0f;
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

void __cdecl eduicbItemTextPickDestroy(eduimenu_s *menu, eduiitem_s *item);
extern char *edui_textrow_default[4];
extern char *edui_textrow_lower[4];
extern char EduiTextCopyBufferPc[64];

/* On-screen keyboard of the text picker: rows 0-3 are the letter rows, rows
 * 4 and 5 the copy/paste row when the picker allows pasting (flag 2). */
static void EduiTextPickPreviousRowPc(edui_textpicker_s *picker) {
    if (--picker->keyboard_row < 0)
        picker->keyboard_row = (picker->keyboard_flags & 2) ? 5 : 3;
}

static void EduiTextPickNextRowPc(edui_textpicker_s *picker) {
    picker->keyboard_row++;
    if (picker->keyboard_flags & 2) {
        if (picker->keyboard_row > 5)
            picker->keyboard_row = 0;
    } else if (picker->keyboard_row > 3) {
        picker->keyboard_row = 0;
    }
}

static void EduiTextPickPreviousColumnPc(edui_textpicker_s *picker) {
    if (picker->keyboard_row >= 4)
        return;
    if (--picker->keyboard_column < 0)
        picker->keyboard_column = NuStrLen(textrow[picker->keyboard_row]) - 1;
}

static void EduiTextPickNextColumnPc(edui_textpicker_s *picker) {
    if (picker->keyboard_row >= 4)
        return;
    if (++picker->keyboard_column >= NuStrLen(textrow[picker->keyboard_row]))
        picker->keyboard_column = 0;
}

/* Clears the text and, when pasting is allowed, replaces it with the copy
 * buffer. */
static void EduiTextPickPastePc(edui_textpicker_s *picker) {
    picker->cursor = 0;
    picker->value[0] = '\0';
    if ((picker->keyboard_flags & 2) && EduiTextCopyBufferPc[0]) {
        NuStrNCpy(picker->value, EduiTextCopyBufferPc, picker->max_length - 1);
        picker->value[picker->max_length - 1] = '\0';
        picker->cursor = NuStrLen(picker->value);
    }
}

static void EduiTextPickInsertPc(edui_textpicker_s *picker, char character) {
    i32 index;
    if (NuStrLen(picker->value) >= picker->max_length)
        return;
    for (index = sizeof(picker->value); index > picker->cursor; index--)
        picker->value[index - 1] = picker->value[index - 2];
    picker->value[picker->cursor++] = character;
}

static void EduiTextPickCursorRightPc(edui_textpicker_s *picker) {
    picker->cursor = EDUI_MIN(NuStrLen(picker->value), picker->cursor + 1);
}

static void EduiTextPickDeletePc(edui_textpicker_s *picker) {
    i32 index;
    if (picker->cursor == 0)
        return;
    if (picker->value[picker->cursor]) {
        picker->cursor--;
        for (index = picker->cursor; index < 0xff; index++)
            picker->value[index] = picker->value[index + 1];
    } else {
        picker->value[--picker->cursor] = '\0';
    }
}

/* Swaps the keyboard letter rows between upper and lower case. */
static void EduiTextPickToggleCasePc(void) {
    if (textrow[1] == edui_textrow_default[1])
        memcpy(textrow, edui_textrow_lower, sizeof(textrow));
    else
        memcpy(textrow, edui_textrow_default, sizeof(textrow));
}

static i32 EduiTextPickKeyboardPc(edui_textpicker_s *picker, eduimenu_s *menu, FRAME_PAD_PC *pad) {
    u32 modifiers;
    i32 key = NuKeyGet(&modifiers);
    NuStrLen(picker->value);
    if (key != -1) {
        switch (key) {
            case 1:
            case 0x1c:
                if (picker->callback)
                    picker->callback(menu, &picker->base, pad->digital_buttons);
                break;
            case 0xcb:
                EduiTextPickPreviousColumnPc(picker);
                break;
            case 0xcd:
                EduiTextPickNextColumnPc(picker);
                break;
            case 0xc8:
                EduiTextPickPreviousRowPc(picker);
                break;
            case 0xd0:
                EduiTextPickNextRowPc(picker);
                break;
            case 0x0e:
            case 0xd3:
                EduiTextPickDeletePc(picker);
                break;
            default: {
                char character = NuKeyToAscii(key, modifiers & 1);
                if (character)
                    EduiTextPickInsertPc(picker, character);
            } break;
        }
    }
    return 0;
}

static void tpentOnEnter(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_textpicker_s *source = (edui_textpicker_s *)item->data_ptr;
    NuStrCpy(source->value, ((edui_textpicker_s *)item)->value);
    if (source->callback)
        source->callback(menu->parent, &source->base, buttons);
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

/* Opens an entry menu that edits a copy of the picker's text. */
void eduiAddTextPickEnt(eduimenu_s *menu, eduiitem_s *item) {
    edui_textpicker_s *source = (edui_textpicker_s *)item;
    edui_textpicker_s *picker = eduiAddTextPickEntEx(menu, item, tpentOnEnter);
    if (picker) {
        NuStrCpy(picker->value, source->value);
        picker->cursor = NuStrLen(picker->value);
        memcpy(picker->base.colours, item->colours, sizeof(picker->base.colours));
        picker->max_length = source->max_length;
        picker->keyboard_flags = (picker->keyboard_flags & ~2u) | (source->keyboard_flags & 2);
    }
}

static void tpentPropOnEnter(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_prop_s *property = (edui_prop_s *)item->data_ptr;
    eduimenu_s *parent;
    NuStrCpy(property->property_text, ((edui_textpicker_s *)item)->value);
    if (property->changed)
        property->changed(menu->parent, &property->base, buttons);
    parent = menu->parent;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    if (parent)
        eduiSetActiveMenu(parent);
}

/* Opens an entry menu that edits a copy of the property's text. */
void eduiAddPropTextPickEnt(eduimenu_s *menu, eduiitem_s *item) {
    edui_prop_s *source = (edui_prop_s *)item;
    edui_textpicker_s *picker = eduiAddTextPickEntEx(menu, item, tpentPropOnEnter);
    if (picker) {
        NuStrCpy(picker->value, source->property_text);
        picker->cursor = NuStrLen(picker->value);
        memcpy(picker->base.colours, item->colours, sizeof(picker->base.colours));
    }
}

static i32 EduiTextPickPadPc(edui_textpicker_s *picker, eduimenu_s *menu, FRAME_PAD_PC *pad) {
    if (picker == NULL || !picker->editing) {
        if (pad->digital_buttons_pressed & 0x40)
            eduiAddTextPickEnt(menu, &picker->base);
        return 0;
    }
    if (pad->digital_buttons_pressed & 0x4000)
        EduiTextPickNextRowPc(picker);
    if (pad->digital_buttons_pressed & 0x1000)
        EduiTextPickPreviousRowPc(picker);
    if (pad->digital_buttons_pressed & 0x8000)
        EduiTextPickPreviousColumnPc(picker);
    if (pad->digital_buttons_pressed & 0x2000)
        EduiTextPickNextColumnPc(picker);
    if ((pad->digital_buttons_pressed & 0x10) && picker->callback)
        picker->callback(menu, &picker->base, pad->digital_buttons);
    if ((pad->digital_buttons & 0xc) == 0xc)
        EduiTextPickPastePc(picker);
    if ((pad->digital_buttons & 3) == 3 && (picker->keyboard_flags & 2)) {
        NuStrNCpy(EduiTextCopyBufferPc, picker->value, sizeof(EduiTextCopyBufferPc));
        picker->value[sizeof(EduiTextCopyBufferPc) - 1] = '\0';
    }
    if (pad->digital_buttons_pressed & 4)
        picker->cursor = EDUI_MAX(picker->cursor - 1, 0);
    if (pad->digital_buttons_pressed & 8)
        EduiTextPickCursorRightPc(picker);
    if (pad->digital_buttons_pressed & 0x40)
        EduiTextPickInsertPc(picker, textrow[picker->keyboard_row][picker->keyboard_column]);
    if (pad->digital_buttons_pressed & 0x80)
        EduiTextPickDeletePc(picker);
    if (pad->digital_buttons_pressed & 0x20)
        EduiTextPickToggleCasePc();
    return 1;
}

i32 __cdecl eduicbProcessTextPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    (void)delta_time;
    EduiTextPickKeyboardPc((edui_textpicker_s *)item, menu, pad);
    EduiTextPickPadPc((edui_textpicker_s *)item, menu, pad);
    return 0;
}

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
    i32 result;
    processing_menu = menu;
    if (menu->child == NULL)
        return eduiMenuProcessAux(menu, delta_time, pad);
    result = eduiMenuProcess(menu->child, delta_time, pad);
    if (processing_menu) {
        if (result)
            eduiSetActiveMenu(menu->child ? menu->child : menu);
        else
            result = eduiProcessInteracts(menu, pad);
    }
    return result;
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
    i32 height;
    EduiRenderWidthPc = width;
    height = item->render(menu, item, x, y, width, selected);
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

#define EDUI_DRAW_LINE_I(x0, y0, x1, y1, colour, material)                                                             \
    do {                                                                                                               \
        if (!edui_donotdraw)                                                                                           \
            NuRndrLine2di(x0, y0, x1, y1, colour, material);                                                           \
    } while (0)

static void EduiDrawDisabledItemPc(eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
    if (item->disabled) {
        i32 left = x << 4;
        i32 top = y << 3;
        i32 span = width << 4;
        i32 bottom = top + (height << 3);
        EDUI_DRAW_LINE_I(left, top, left + span / 4, bottom, item->colours[0], (NUMTL *)uimtls[ui_bgmtl]);
        EDUI_DRAW_LINE_I(left + span / 4, top, left + span / 2, bottom, item->colours[0], (NUMTL *)uimtls[ui_bgmtl]);
        EDUI_DRAW_LINE_I(left + span / 2, top, left + span * 3 / 4, bottom, item->colours[0],
                         (NUMTL *)uimtls[ui_bgmtl]);
        EDUI_DRAW_LINE_I(left + span * 3 / 4, top, left + span, bottom, item->colours[0], (NUMTL *)uimtls[ui_bgmtl]);
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

void __cdecl eduicbPickCFGCancel(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

void __cdecl FS_SortStrings(char *start, char *end, i32 mode) {
    char temporary[256];
    char *current;
    char *candidate;
    i32 direction;
    i32 current_length;
    i32 candidate_length;
    if ((u32)mode > 1) {
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
    } else {
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
    }
}

char FS_Filter[256] = "*.nup | *.hgp   ";
char FS_FilterOut[256] = "_PC. | _360. | _PS3.";
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

extern void __cdecl NuStrToLower(char *text);
char FileSelFilterBlocksPc[8][96] = {0};
char FileSelFilterOutBlocksPc[8][12] = {0};
i32 FileSelFilterBlockCountPc = 0;
i32 FileSelFilterOutBlockCountPc = 0;
char FileSelVolumeNamesPc[1024] = {0};
const char FileSelParentNamePc[3] = "..";
char FS_ParentEntry[8] = "D######";
char *FS_FileListEnd = FS_FileList;
extern char FileSelFilterBlocksPc[8][96];
extern char FileSelFilterOutBlocksPc[8][12];
extern i32 FileSelFilterBlockCountPc;
extern i32 FileSelFilterOutBlockCountPc;

char *__cdecl FS_GetFilterString(char *input, char *output) {
    char *end;
    char *trimmed;
    while (*input == '*' || *input == ' ')
        ++input;
    end = input;
    while (*end != 0 && *end != '|' && *end != '*')
        ++end;
    trimmed = end;
    while (trimmed != input && trimmed[-1] == ' ')
        --trimmed;
    while (input != trimmed)
        *output++ = *input++;
    *output = 0;
    return end;
}

void __cdecl FS_BuildFilterBlocks(char *input) {
    i32 slot = 0;
    FileSelFilterBlockCountPc = 0;
    NuStrToLower(input);
    while (*input != 0) {
        input = FS_GetFilterString(input, &FileSelFilterBlocksPc[FileSelFilterBlockCountPc][slot * 12]);
        if (*input == '|') {
            FileSelFilterBlocksPc[FileSelFilterBlockCountPc][(slot + 1) * 12] = 0;
            ++FileSelFilterBlockCountPc;
            slot = 0;
            ++input;
        } else
            ++slot;
    }
    if (slot != 0)
        ++FileSelFilterBlockCountPc;
}

void __cdecl FS_BuildFilterOutBlocks(char *input) {
    FileSelFilterOutBlockCountPc = 0;
    NuStrToLower(input);
    while (*input != 0) {
        input = FS_GetFilterString(input, FileSelFilterOutBlocksPc[FileSelFilterOutBlockCountPc]);
        if (FileSelFilterOutBlocksPc[FileSelFilterOutBlockCountPc][0] != 0)
            ++FileSelFilterOutBlockCountPc;
        if (*input == '|')
            ++input;
    }
}

i32 __cdecl FS_FileNameFilter(char *name) {
    char lower[256];
    i32 i, slot, dots;
    char *retry, *match, *scan;
    NuStrCpy(lower, name);
    NuStrToLower(lower);
    for (i = 0; i < FileSelFilterOutBlockCountPc; ++i)
        if (NuStrStr(lower, FileSelFilterOutBlocksPc[i]) != NULL)
            return 0;
    for (i = 0; i < FileSelFilterBlockCountPc; ++i) {
        retry = lower;
        do {
            match = retry;
            retry = NULL;
            for (slot = 0; slot < 7; ++slot) {
                if (FileSelFilterBlocksPc[i][slot * 12] == 0)
                    break;
                match = NuStrStr(match, &FileSelFilterBlocksPc[i][slot * 12]);
                if (match == NULL)
                    break;
                if (slot == 0)
                    retry = match + 1;
            }
            if (match != NULL && slot != 7) {
                dots = 0;
                for (scan = match; *scan != 0; ++scan) {
                    if (*scan == '.' && ++dots > 1)
                        break;
                }
                if (dots <= 1)
                    return 1;
            }
        } while (retry != NULL);
    }
    return 0;
}

typedef struct FILE_DIRECTORY_TIME_PC {
    i8 second, minute, hour, day, month, reserved;
    i16 year;
} FILE_DIRECTORY_TIME_PC;
typedef struct FILE_DIRECTORY_ENTRY_PC {
    u32 flags, size;
    FILE_DIRECTORY_TIME_PC created, modified;
    char name[256];
} FILE_DIRECTORY_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_ENTRY_PC, name) == 0x18);

/* File picker directory menu: the picker and menu it was opened for. */
static eduimenu_s *filepick_menu = 0;
static edui_file_pick_s *filepick_item = 0;
static i32 dir_size = 0;

static void eduicbDestroyDirectoryList(eduimenu_s *menu, eduimenu_s *parent) {
    (void)parent;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

static void eduicbSelectDirectoryExit(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)item;
    (void)value;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

/* Steps into a directory (or up for the "<..>" entry 9999), or picks a file. */
static void eduicbSelectDirectoryEntry(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    if (dir_list == NULL)
        return;
    if (item->data == 9999) {
        char *separator = NuStrRChr(filepick_item->filename, '/');
        if (separator)
            *separator = '\0';
        else
            filepick_item->filename[0] = '\0';
        filepick_item->reopen_directory = 1;
    }
    if (((FILE_DIRECTORY_ENTRY_PC *)dir_list)[item->data].flags & 8) {
        if (filepick_item->filename[0])
            NuStrCat(filepick_item->filename, "/");
        NuStrCat(filepick_item->filename, ((FILE_DIRECTORY_ENTRY_PC *)dir_list)[item->data].name);
        filepick_item->reopen_directory = 1;
    }
    if (!(((FILE_DIRECTORY_ENTRY_PC *)dir_list)[item->data].flags & 8)) {
        NuStrCpy(filepick_item->name, ((FILE_DIRECTORY_ENTRY_PC *)dir_list)[item->data].name);
        if (filepick_item->changed)
            filepick_item->changed(filepick_menu, &filepick_item->base, value);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

/* Lists the picker's directory (filtered by its filter text) in a menu. */
static void eduicbAttachDirectoryList(eduimenu_s *menu, edui_file_pick_s *picker) {
    eduimenu_s *directory_menu;
    eduiitem_s *item;
    FILE_DIRECTORY_ENTRY_PC *entry;
    i32 directory;
    i32 index;
    char text[1024];
    if (dir_list == NULL)
        return;
    picker->reopen_directory = 0;
    filepick_menu = menu;
    filepick_item = picker;
    directory_menu =
        eduiMenuCreate(menu->x + 20, menu->y + 20, 250, 300, menu->font, eduicbDestroyDirectoryList, "Directory");
    if (directory_menu == NULL)
        return;
    NuSPrintf(text, "%s%s", picker->directory, picker->filename);
    directory = NuFileOpenDir(text);
    if (directory) {
        dir_size = 0;
        while (NuFileReadDir(directory, &((FILE_DIRECTORY_ENTRY_PC *)dir_list)[dir_size]) > 0 &&
               dir_size < edui_filepick_listsize)
            dir_size++;
        if (picker->compare_entries)
            qsort(dir_list, dir_size, sizeof(FILE_DIRECTORY_ENTRY_PC),
                  (i32(__cdecl *)(const void *, const void *))picker->compare_entries);
        if (picker->filename[0]) {
            item = eduiItemSelCreate(9999, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, "<..>");
            item->text_alignment = 0x10;
            eduiMenuAddItem(directory_menu, item);
        }
        for (index = 0; index < dir_size; index++) {
            entry = &((FILE_DIRECTORY_ENTRY_PC *)dir_list)[index];
            if (entry->flags & 8) {
                NuSPrintf(text, "<%s>", entry->name);
                item = eduiItemSelCreate(index, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, text);
                item->text_alignment = 0x10;
                eduiMenuAddItem(directory_menu, item);
            }
        }
        for (index = 0; index < dir_size; index++) {
            entry = &((FILE_DIRECTORY_ENTRY_PC *)dir_list)[index];
            if (picker->filter[0] && !NuStrIStr(entry->name, picker->filter))
                continue;
            if (entry->flags & 8)
                continue;
            NuSPrintf(text, "%s  ( size=%d  date=%d/%d/%d %d:%d )", entry->name, entry->size, entry->modified.day,
                      entry->modified.month, entry->modified.year, entry->modified.hour, entry->modified.minute);
            item = eduiItemSelCreate(index, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, text);
            item->text_alignment = 0x10;
            eduiMenuAddItem(directory_menu, item);
        }
        if (directory_menu->first == NULL)
            eduiMenuAddItem(directory_menu, eduiItemSelCreate(dir_size, picker->base.colours, 0, 1,
                                                              eduicbSelectDirectoryExit, "No Files"));
        NuFileCloseDir(directory);
    }
    eduiMenuFitWidth(directory_menu, 8);
    eduiMenuAttach(menu, directory_menu);
}

i32 __cdecl eduicbProcessFilePick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_file_pick_s *picker = (edui_file_pick_s *)item;
    (void)delta_time;
    if ((pad->digital_buttons_pressed & 0x40) || picker->reopen_directory) {
        eduiMenuHighlight(menu, item);
        eduicbAttachDirectoryList(menu, picker);
    }
    return 0;
}

typedef struct FILE_DIRECTORY_RAW_TIME_PC {
    u8 second, minute, hour, day, month, year_low, year_high;
} FILE_DIRECTORY_RAW_TIME_PC;
typedef struct FILE_DIRECTORY_HOST_ENTRY_PC {
    u32 flags;
    u8 reserved_04[4];
    u32 size;
    u8 reserved_0c;
    FILE_DIRECTORY_RAW_TIME_PC created;
    u8 reserved_14;
    FILE_DIRECTORY_RAW_TIME_PC accessed;
    u8 reserved_1c;
    FILE_DIRECTORY_RAW_TIME_PC modified;
    u32 unused_24;
    u8 reserved_28[0x18];
    char name[MAX_PATH];
} FILE_DIRECTORY_HOST_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, created) == 0xd);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, modified) == 0x1d);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, name) == 0x40);
extern char FileSelVolumeNamesPc[1024];
extern i32 __cdecl NuFileReadDir(i32 file, FILE_DIRECTORY_ENTRY_PC *entry);
extern void __cdecl FS_SortStrings(char *start, char *end, i32 mode);

void __cdecl FS_GetDirList(char *path, char *filter, char *filter_out) {
    FILE_DIRECTORY_ENTRY_PC entry;
    char header[7];
    char *next = FS_FileList;
    char *drive;
    char *file_begin;
    i32 directory, length;
    FS_BuildFilterBlocks(filter);
    FS_BuildFilterOutBlocks(filter_out);
    FS_CurrentCursorPos = FS_FileList;
    FS_CurrentPos = FS_FileList;
    FS_CursorLine = 0;
    FS_CurrentPosFileNum = 0;
    FS_NumFiles = 0;
    if (FS_ShowVolumes != 0) {
        GetLogicalDriveStringsA(1024, FileSelVolumeNamesPc);
        for (drive = FileSelVolumeNamesPc; *drive != 0; drive += length + 1) {
            length = NuStrLen(drive);
            header[0] = 'V';
            header[1] = header[2] = header[3] = header[4] = header[5] = 'A';
            header[6] = '#';
            PCByteCopy_candidate(next, header, 7);
            next += 7;
            NuStrCpy(next, drive);
            next += length;
            *next++ = 0;
            ++FS_NumFiles;
        }
        FS_SortStrings(FS_FileList, next, 0);
    } else {
        directory = NuFileOpenDir(path);
        PCByteCopy_candidate(FS_FileList, FS_ParentEntry, 7);
        NuStrCpy(FS_FileList + 7, FileSelParentNamePc);
        ++FS_NumFiles;
        next = FS_FileList + 10;
        if (directory != 0) {
            while ((length = NuFileReadDir(directory, &entry)) != 0) {
                if ((entry.name[0] == '.' && (entry.name[1] == 0 || (entry.name[1] == '.' && entry.name[2] == 0))) ||
                    !(entry.flags & 8))
                    continue;
                if (next + length + 8 >= FS_FileList + sizeof(FS_FileList))
                    break;
                header[0] = 'D';
                header[4] = entry.created.day + 'A';
                header[3] = entry.created.hour + 'A';
                header[2] = entry.created.minute + 'A';
                header[1] = entry.created.second + 'A';
                header[5] = entry.created.month + 'A';
                header[6] = (u8)(entry.created.year - 123);
                PCByteCopy_candidate(next, header, 7);
                next += 7;
                NuStrCpy(next, entry.name);
                next += length;
                *next++ = 0;
                ++FS_NumFiles;
            }
        }
        FS_SortStrings(FS_FileList + 10, next, FS_SortMode);
        if (directory != 0)
            NuFileCloseDir(directory);
        file_begin = next;
        directory = NuFileOpenDir(path);
        if (directory != 0) {
            while ((length = NuFileReadDir(directory, &entry)) != 0) {
                if ((entry.flags & 8) || !FS_FileNameFilter(entry.name))
                    continue;
                if (next + length + 8 >= FS_FileList + sizeof(FS_FileList))
                    break;
                header[0] = 'F';
                header[4] = entry.modified.day + 'A';
                header[3] = entry.modified.hour + 'A';
                header[2] = entry.modified.minute + 'A';
                header[1] = entry.modified.second + 'A';
                header[5] = entry.modified.month + 'A';
                header[6] = (u8)(entry.modified.year - 123);
                PCByteCopy_candidate(next, header, 7);
                next += 7;
                NuStrCpy(next, entry.name);
                next += length;
                *next++ = 0;
                ++FS_NumFiles;
            }
        }
        FS_SortStrings(file_begin, next, FS_SortMode);
        if (directory != 0)
            NuFileCloseDir(directory);
    }
    FS_FileListEnd = next;
}

extern i32 edmain_cursor_enabled;
f32 edui_cursor_y = 50.0f;

STATIC_ASSERT(offsetof(eduimenu_s, selected) == 8);
STATIC_ASSERT(offsetof(eduimenu_s, parent) == 0x40);
STATIC_ASSERT(offsetof(eduiitem_s, process) == 0x3c);
STATIC_ASSERT(offsetof(edui_interact_s, callback) == 0x18);
STATIC_ASSERT(offsetof(edui_interact_s, field_20) == 0x20);

static i32 EduiInteractionContainsCursorPc(edui_interact_s *interact) {
    f32 edge;
    if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
        edge = interact->width + interact->x;
        if (edui_cursor_x < edge) {
            edge = interact->height + interact->y;
            if (edui_cursor_y < edge)
                return 1;
        }
    }
    return 0;
}

static i32 EduiProcessLockedInteractionPc(eduimenu_s *menu) {
    eduimenu_s *locked_menu = eduiInteractLocked->menu;
    if (menu && (menu == locked_menu || menu->parent == locked_menu)) {
        eduiInteractLocked = eduiInteractLocked->callback(eduiInteractLocked) ? eduiInteractLocked : NULL;
    }
    return eduiInteractLocked != NULL;
}

static i32 EduiInteractHelperPc6272a0(eduimenu_s *menu, edui_interact_s *interact) {
    i32 result = 0;
    u32 pressed;
    if (interact->menu == menu) {
        pressed = edui_cursor_buttons_db;
        if ((interact->buttons & pressed) != 0 && EduiInteractionContainsCursorPc(interact)) {
            result = 1;
            if (interact->menu != NULL && interact->item != NULL && interact->item->type != 18)
                interact->menu->selected = interact->item;
            if (interact->field_20 != 0) {
                if ((interact->buttons & pressed) == 0)
                    return result;
            } else {
                if ((interact->buttons & edui_cursor_buttons) == 0)
                    return result;
                if (menu != active_menu)
                    eduiSetActiveMenu(menu);
            }
            if (interact->callback != NULL && interact->callback(interact))
                eduiInteractLocked = interact;
        }
    }
    return result;
}

i32 eduiProcessInteracts(eduimenu_s *menu, FRAME_PAD_PC *pad) {
    i32 result = 0;
    i32 index;
    edui_interact_s *interact;
    (void)pad;
    if (edmain_cursor_enabled) {
        if (eduiInteractLocked != NULL) {
            if ((eduiInteractLocked->menu == NULL || eduiInteractLocked->item == NULL) && eduiInteractLocked->field_20)
                eduiInteractLocked = NULL;
        }
        if (eduiInteractLocked != NULL) {
            result = EduiProcessLockedInteractionPc(menu);
        } else {
            interact = &eduiInteracts[numInteracts - 1];
            for (index = 0; index < numInteracts; index++, interact--) {
                if (EduiInteractHelperPc6272a0(menu, interact))
                    return 1;
            }
        }
    }
    return result;
}

i32 eduiMenuProcessAux(eduimenu_s *menu, f32 delta_time, FRAME_PAD_PC *pad) {
    if (!EduiUsingMenuFocusPc || menu == active_menu) {
        i32 result = 0;
        eduiitem_s *item;
        if (!menu->selected)
            menu->selected = menu->first;
        item = menu->selected;
        if (item && !item->disabled && item->process)
            result = item->process(menu, item, delta_time, pad);
        if ((!EduiUsingMenuFocusPc || menu == active_menu) && menu == processing_menu) {
            if (!menu->selected)
                menu->selected = menu->first;
            if (!EduiUsingMenuFocusPc || menu == active_menu)
                eduiMenuProcessInput(menu, delta_time, pad, result);
        }
    }
    return eduiProcessInteracts(menu, pad);
}

/* SOURCE ONLY: independent file-selector helpers appended to src/gameapi/edfile.c. */

typedef struct FS_PACKED_ENTRY_HEADER_PC {
    u8 kind;
    u8 encoded_second;
    u8 encoded_minute;
    u8 encoded_hour;
    u8 encoded_day;
    u8 encoded_month;
    u8 encoded_year;
} FS_PACKED_ENTRY_HEADER_PC;
STATIC_ASSERT(sizeof(FS_PACKED_ENTRY_HEADER_PC) == 7);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, kind) == 0);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_second) == 1);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_minute) == 2);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_hour) == 3);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_day) == 4);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_month) == 5);
STATIC_ASSERT(offsetof(FS_PACKED_ENTRY_HEADER_PC, encoded_year) == 6);

void __cdecl FS_MakeDateTimeString(const FS_PACKED_ENTRY_HEADER_PC *entry, char *output) {
    NuSPrintf(output, " %.2d/%.2d/%d %.2d:%.2d:%.2d", entry->encoded_day - 'A', entry->encoded_month - 'A',
              entry->encoded_year + 1915, entry->encoded_hour - 'A', entry->encoded_minute - 'A',
              entry->encoded_second - 'A');
}

void __cdecl FS_MoveCursorDown(i32 steps) {
    while (steps > 0) {
        i32 top_length = NuStrLen(FS_CurrentPos);
        i32 cursor_length = NuStrLen(FS_CurrentCursorPos);
        char *next = FS_CurrentCursorPos + cursor_length + 1;
        char *end = FS_FileListEnd;
        if (next < end) {
            if (FS_CursorLine == 13) {
                char *next_top = FS_CurrentPos + top_length + 1;
                if (next_top < end) {
                    ++FS_CurrentPosFileNum;
                    FS_CurrentPos = next_top;
                }
            } else {
                ++FS_CursorLine;
            }
            FS_CurrentCursorPos = next;
        }
        --steps;
    }
}

f32 __cdecl FS_GetDirTextWidth(void) {
    char date_time[64];
    f32 max_width = 0.0f;
    char *entry;
    for (entry = FS_FileList; entry < FS_FileListEnd;) {
        FS_PACKED_ENTRY_HEADER_PC *header = (FS_PACKED_ENTRY_HEADER_PC *)entry;
        char *name = (char *)(header + 1);
        f32 width;
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        width = NuQFntPrintLenU(system_qfont, name);
        if (header->encoded_year != '#') {
            NuQFntSetPointSize(system_qfont, 0.8f, 1.0f);
            FS_MakeDateTimeString(header, date_time);
            width += NuQFntPrintLenU(system_qfont, date_time);
        }
        if (width > max_width)
            max_width = width;
        entry = name + NuStrLen(name) + 1;
    }
    return max_width;
}

/* PC file-selector rendering. Data mapping extents remain separately reviewed. */
#pragma section(".data$FS_Title", read, write)
__declspec(allocate(".data$FS_Title")) char FS_Title[64] = "Title";
#pragma section(".data$FS_X", read, write)
__declspec(allocate(".data$FS_X")) f32 FS_X = 50.0f;
#pragma section(".data$FS_Y", read, write)
__declspec(allocate(".data$FS_Y")) f32 FS_Y = 40.0f;
#pragma section(".data$FS_W", read, write)
__declspec(allocate(".data$FS_W")) f32 FS_W = 0.0f;
#pragma section(".data$FS_H", read, write)
__declspec(allocate(".data$FS_H")) f32 FS_H = 0.0f;
#pragma section(".data$SortStrings", read, write)
__declspec(allocate(".data$SortStrings")) char *SortStrings[4] = {"A to Z", "Z to A", "Old to New", "New To Old"};
extern void __cdecl NuRndrRect2d(f32 x, f32 y, f32 z, f32 width, f32 height, i32 colour, NUMTL *material);

void __cdecl RenderFileSel3(i32 draw_title) {
    char buffer[256];
    char date_time[64];
    char *text, *entry;
    f32 x, y, width, height, baseline, full_width, box_height, box_width;
    f32 text_x, line_height, path_y, list_box_height, line_y, footer_base, footer_y, sort_length;
    f64 list_height;
    i32 line;
    if (FS_Active == 0)
        return;
    x = FS_X;
    y = FS_Y;
    width = FS_Width;
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
    height = NuQFntHeight(system_qfont);
    baseline = NuQFntBaseline(system_qfont);
    full_width = width;
    if (FS_NumFiles > 14) {
        full_width = width + 8.0;
    }

    if (draw_title)
        box_height = height * 17.0 * 0.0625 + 6.0;
    else
        box_height = height * 16.0 * 0.0625 + 4.0;
    box_width = full_width + 4.0;
    NuRndrRect2d(x, y, 0.0f, box_width, box_height, 0xff808080, NULL);
    x += 2.0;
    y += 1.0;
    FS_W = box_width;
    FS_H = box_height;
    text_x = (x + 4.0) * 16.0;
    if (draw_title) {
        f32 title_height = height * 0.0625;
        f32 title_y;
        NuRndrRect2d(x, y, 0.0f, full_width, title_height, 0xff404040, NULL);
        NuRndrLine3dDbg(x, y, 0.0f, width + x, y, 0.0f, 0xff808080);
        NuQFntSet(system_qfont);
        NuQFntSetColour(system_qfont, 0x80808080);
        title_y = y * 16.0 + baseline;
        NuQFntMove(system_qfont, text_x, title_y, 0.0f);
        NuQFntPrintU(system_qfont, FS_Title);
        y += title_height + 2.0;
    }

    line_height = height * 0.0625;
    path_y = y * 16.0 + baseline;
    NuRndrRect2d(x, y, 0.0f, full_width, line_height, 0x20202020, NULL);
    if (FS_ShowVolumes) {
        NuStrCpy(buffer, "Volumes:-");
        text = buffer;
    } else {
        f32 available = full_width - 8.0;
        f32 length;
        text = FS_Path;
        length = NuQFntPrintLenU(system_qfont, text) * 0.0625;
        if (length > available) {
            f32 scale = available / length;
            if (scale < 0.5f) {
                // Too long even at half width: keep the drive and first
                // directory, then "\....", then the last directory.
                char *first = NuStrChr(text, '\\');
                if (first != NULL) {
                    char *last;
                    i32 tail_length;
                    char *second = NuStrChr(first + 1, '\\');
                    if (second == NULL) {
                        second = first;
                    }
                    *second = '\0';
                    NuStrCpy(buffer, text);
                    *second = '\\';
                    NuStrCat(buffer, "\\....");
                    tail_length = NuStrLen(second);
                    if (tail_length != 0 && second[tail_length - 1] == '\\') {
                        second[tail_length - 1] = '\0';
                        last = NuStrRChr(second, '\\');
                        second[tail_length - 1] = '\\';
                    } else {
                        last = NuStrRChr(second, '\\');
                    }
                    if ((u32)last > (u32)second) {
                        NuStrCat(buffer, last);
                        text = buffer;
                    }
                }
                length = NuQFntPrintLenU(system_qfont, text) * 0.0625;
                if (!(length > available)) {
                    goto path_done;
                }
            }
            NuQFntSetPointSize(system_qfont, available / length, 1.0f);
        }
    path_done:;
    }
    NuQFntSet(system_qfont);
    NuQFntMove(system_qfont, text_x, path_y, 0.0f);
    NuQFntSetColour(system_qfont, 0x80408080);
    NuQFntPrintU(system_qfont, text);
    NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);

    y += line_height + 2.0;
    list_height = height * 14.0;
    list_box_height = list_height * 0.0625;
    NuRndrRect2d(x, y, 0.0f, width, list_box_height, 0x20202020, NULL);
    if (FS_NumFiles > 14) {
        f32 thumb, thumb_offset;
        NuRndrRect2d(width + x, y, 0.0f, 8.0f, list_box_height, 0xff646464, NULL);
        thumb = list_box_height / FS_NumFiles * 14.0;
        if (thumb < 5.0f) {
            thumb = 5.0f;
        }
        thumb_offset = (list_box_height - thumb) * FS_CurrentPosFileNum / (FS_NumFiles - 14);
        NuRndrRect2d(width + x, thumb_offset + y, 0.0f, 8.0f, thumb, 0xffc8c8c8, NULL);
    }

    line_y = path_y + 32.0;
    entry = FS_CurrentPos;
    for (line = 0; line < 14; ++line) {
        FS_PACKED_ENTRY_HEADER_PC *header;
        char *name;
        if (entry >= FS_FileListEnd) {
            break;
        }
        line_y += height;
        if (entry == FS_CurrentCursorPos) {
            NuRndrRect2d(x, (line_y - baseline) * 0.0625, 0.0f, width, line_height, 0xff202060, NULL);
        }
        NuQFntSet(system_qfont);
        NuQFntMove(system_qfont, text_x, line_y, 0.0f);
        header = (FS_PACKED_ENTRY_HEADER_PC *)entry;
        if (header->kind == 'V') {
            NuQFntSetColour(system_qfont, 0x80204080);
        } else if (header->kind == 'D') {
            NuQFntSetColour(system_qfont, 0x80804020);
        } else {
            NuQFntSetColour(system_qfont, 0x80408080);
        }
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        name = (char *)(header + 1);
        NuQFntPrintU(system_qfont, name);
        if (header->encoded_year != '#') {
            f32 length;
            f64 time_x;
            NuQFntSetPointSize(system_qfont, 0.8f, 1.0f);
            NuSPrintf(date_time, " %.2d:%.2d:%.2d", header->encoded_hour - 'A', header->encoded_minute - 'A',
                      header->encoded_second - 'A');
            length = NuQFntPrintLenU(system_qfont, date_time);
            NuQFntSetColour(system_qfont, 0x80208080);
            time_x = (width + x) * 16.0 - length;
            NuQFntMove(system_qfont, time_x, line_y, 0.0f);
            NuQFntPrintU(system_qfont, date_time);
            NuSPrintf(date_time, " %.2d/%.2d/%d", header->encoded_day - 'A', header->encoded_month - 'A',
                      header->encoded_year + 1915);
            length = NuQFntPrintLenU(system_qfont, date_time);
            NuQFntSetColour(system_qfont, 0x80808020);
            NuQFntMove(system_qfont, time_x - length, line_y, 0.0f);
            NuQFntPrintU(system_qfont, date_time);
        }
        entry = name + NuStrLen(name) + 1;
    }

    footer_base = y * 16.0 + baseline;
    footer_y = footer_base + list_height;
    NuQFntSet(system_qfont);
    NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
    NuQFntSetColour(system_qfont, 0x80808080);
    NuQFntMove(system_qfont, x * 16.0, footer_y, 0.0f);
    NuQFntPrintU(system_qfont, FS_Filter);
    NuQFntSetColour(system_qfont, 0x80000040);
    sort_length = NuQFntPrintLenU(system_qfont, SortStrings[FS_SortMode]);
    NuQFntMove(system_qfont, (full_width + x) * 16.0 - sort_length, footer_y, 0.0f);
    NuQFntPrintU(system_qfont, SortStrings[FS_SortMode]);
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
}

STATIC_ASSERT(offsetof(edui_gradient_pick_s, first_stage) == 0x4c);
STATIC_ASSERT(offsetof(edui_gradient_pick_s, selected_stage) == 0x50);
STATIC_ASSERT(offsetof(edui_gradient_pick_s, changed) == 0x54);
STATIC_ASSERT(offsetof(edui_gradient_pick_s, change_timer) == 0x6c);
STATIC_ASSERT(offsetof(edui_gradient_node_s, next) == 0);
STATIC_ASSERT(offsetof(edui_gradient_node_s, previous) == 4);
STATIC_ASSERT(offsetof(edui_gradient_node_s, time) == 8);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons) == 0x70);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons_pressed) == 0x78);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_l1) == 0x9c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_r1) == 0x9d);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_l2) == 0x9e);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_r2) == 0x9f);

i32 __cdecl eduicbProcessGradPickItem(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    edui_gradient_node_s *stage;
    f32 lower, upper;
    i32 decrease, increase;
    (void)delta_time;
    if (gradient->changed)
        gradient->changed(menu, item, pad->digital_buttons);
    if (pad->digital_buttons_pressed & 0x40) {
        eduicbProcessGradPick(menu, item);
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
    decrease = pad->analog_l1 ? pad->analog_l1 : pad->analog_l2;
    increase = pad->analog_r1 ? pad->analog_r1 : pad->analog_r2;
    if (decrease) {
        f32 step = (f32)(decrease / 15360.0);
        f64 time = (f64)stage->time - step;
        stage->time = lower > time ? lower : time;
        gradient->change_timer = 60;
    } else if (increase) {
        f32 step = (f32)(increase / 15360.0);
        f64 time = (f64)step + stage->time;
        stage->time = upper < time ? upper : time;
        gradient->change_timer = 60;
    }
    return 0;
}

extern u8 FS_RefreshDir;

i32 __cdecl ProcessFileSel3(f32 elapsed, FRAME_PAD_PC *pad) {
    i32 buttons;
    i32 length;
    char *separator;
    f32 width;
    if (FS_Active == 0)
        return 0;
    if (FS_RefreshDir != 0) {
        FS_GetDirList(FS_Path, FS_Filter, FS_FilterOut);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        width = FS_GetDirTextWidth() * 0.0625;
        NuQFntPopCoordinateSystem();
        if (width < 240.0)
            width = 240.0f;
        FS_Width = width + 8.0;
        FS_GetPadWithRepeat(pad, 0.05f, elapsed);
        if (FS_LastFileName[0] != '\0')
            FS_SetCursorToLastFileName();
        FS_RefreshDir = 0;
    }
    buttons = FS_GetPadWithRepeat(pad, 0.05f, elapsed);
    if (buttons & 0x4000)
        FS_MoveCursorDown(1);
    if (buttons & 0x1000)
        FS_MoveCursorUp(1);
    if (buttons & 1)
        FS_MoveCursorDown(14);
    if (buttons & 4)
        FS_MoveCursorUp(14);
    if (buttons & 2)
        FS_MoveCursorDown(FS_NumFiles);
    if (buttons & 8)
        FS_MoveCursorUp(FS_NumFiles);
    if (buttons & 0x80) {
        if (++FS_SortMode == 4)
            FS_SortMode = 0;
        FS_RefreshDir = 2;
        NuStrCpy(FS_LastFileName, FS_CurrentCursorPos + 7);
    }
    if (buttons & 0x20) {
        length = NuStrLen(FS_Path);
        if (length != 0 && FS_Path[length - 1] == '\\')
            FS_Path[length - 1] = '\0';
        separator = NuStrRChr(FS_Path, '\\');
        if (separator != NULL) {
            separator[1] = '\0';
        } else {
            NuStrCat(FS_Path, WorldPathSeparator);
            FS_ShowVolumes = 1;
        }
        FS_RefreshDir = 1;
        FS_LastFileName[0] = '\0';
    }
    if (buttons & 0x40) {
        if (FS_CurrentCursorPos[0] == 'V') {
            FS_ShowVolumes = 0;
            NuStrCpy(FS_Path, FS_CurrentCursorPos + 7);
            FS_LastFileName[0] = '\0';
            FS_RefreshDir = 1;
        } else if (FS_CurrentCursorPos[0] == 'D') {
            if (FS_CurrentCursorPos[7] == '.' && FS_CurrentCursorPos[8] == '.' && FS_CurrentCursorPos[9] == '\0') {
                length = NuStrLen(FS_Path);
                if (length != 0 && FS_Path[length - 1] == '\\')
                    FS_Path[length - 1] = '\0';
                separator = NuStrRChr(FS_Path, '\\');
                if (separator != NULL) {
                    separator[1] = '\0';
                } else {
                    NuStrCat(FS_Path, WorldPathSeparator);
                    FS_ShowVolumes = 1;
                }
            } else {
                length = NuStrLen(FS_Path);
                if (length != 0 && FS_Path[length - 1] != '\\')
                    NuStrCat(FS_Path, WorldPathSeparator);
                NuStrCat(FS_Path, FS_CurrentCursorPos + 7);
                NuStrCat(FS_Path, WorldPathSeparator);
            }
            FS_LastFileName[0] = '\0';
            FS_RefreshDir = 1;
        } else {
            FS_Active = 0;
            NuStrCpy(FS_LastFileName, FS_CurrentCursorPos + 7);
            if (FS_Callback != NULL)
                FS_Callback(FS_Path, FS_CurrentCursorPos + 7);
            return 1;
        }
    }
    return 0;
}

void CreateTestMenu(void *font) {
    u32 colours[4] = {0x80000000u, 0x80ff0000u, 0x80808080u, 0x80404040u};
    eduiInitMaterials();

    submenu1 = eduiMenuCreate(90, 90, 180, 50, font, NULL, ("SubMenu1"));
    if (submenu1 != NULL) {
        eduiMenuAddItem(submenu1, eduiItemSelCreate(0, colours, 0, 1, (void *)cbSel, ("Sub Sel 0")));
        eduiMenuAddItem(submenu1, eduiItemSelCreate(1, colours, 0, 1, (void *)cbSel, ("Sub Sel 1")));
        eduiMenuAddItem(submenu1, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sub Sel 2")));
    }

    submenu0 = eduiMenuCreate(70, 70, 180, 50, font, NULL, ("SubMenu0"));
    if (submenu0 != NULL) {
        eduiMenuAddItem(submenu0, eduiItemSelCreate(0, colours, 0, 1, (void *)cbSel, ("Sub Sel")));
        eduiMenuAddItem(submenu0,
                        eduiItemSelCreate((u32)(submenu1), colours, 0, 0, (void *)cbSubMenu, ("Sub Menu 1..")));
        eduiMenuAddItem(submenu0, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sub Sel")));
    }

    testmenu0 = eduiMenuCreate(50, 50, 180, 50, font, NULL, ("Main Menu"));
    if (testmenu0 != NULL) {
        edui_gradient_pick_s *gradient;
        edui_gradient_pick_s *grey_gradient;
        eduiMenuAddItem(testmenu0,
                        eduiItemSelCreate((u32)(submenu0), colours, 0, 0, (void *)cbSubMenu, ("Sub Menu 0..")));

        gradient = (edui_gradient_pick_s *)(eduiItemGradPickCreate(1, colours, (void *)cbGradChange, ("GradPick")));
        if (gradient != NULL) {
            eduiGradStageAdd(gradient, 0.0f, 0.0f, 0.0f, 0.0f);
            eduiGradStageAdd(gradient, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        eduiMenuAddItem(testmenu0, (eduiitem_s *)gradient);

        grey_gradient = (edui_gradient_pick_s *)(eduiItemGreyGradPickCreate(1, colours, (EdUiItemCallback)cbGradChange,
                                                                            ("GreyGradPick")));
        if (grey_gradient != NULL) {
            eduiGradStageAdd(grey_gradient, 0.0f, 0.0f, 0.0f, 0.0f);
            eduiGradStageAdd(grey_gradient, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        eduiMenuAddItem(testmenu0, (eduiitem_s *)grey_gradient);

        eduiMenuAddItem(testmenu0, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sel")));
    }
}

void TestMenu(FRAME_PAD_PC *pad) {
    eduiMenuProcess(testmenu0, 0.016666668f, pad);
    eduiMenuRender(testmenu0);
}
