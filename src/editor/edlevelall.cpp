#include "saga.h"
#include <new>
#include <string.h>
#include <stdio.h>

/* Level editor (Android editor/edlevelall.cpp). The retail unit is C++; the
 * EdDraw primitives that share it live in eddraw.c. */

extern "C" {
    extern eduimenu_s *edLevelActiveMenu;
    extern eduimenu_s *edLevelPinnedMenu;
    extern eduimenu_s *edLevelNextMenu;
    extern i32 edLevelDestroyActiveMenu;
    extern eduimenu_s *edLevelDestroyThisMenu;
    extern eduimenu_s *edLevelDestroyThisMenu2;
    extern i32 edlevel_mouseandkeyboard;
    void __cdecl eduiMenuFitOnScreen(eduimenu_s *menu, i32 padding);
    void __cdecl eduiSetDefaultActiveMenu(eduimenu_s *menu);
    void __cdecl eduiProcessCursor(f32 delta_time, FRAME_PAD_PC *pad);
    i32 __cdecl eduiGetCameraEnabled(void);
    void __cdecl eduiSetFontScale(f32 x, f32 y);
    void __cdecl edcamMove(FRAME_PAD_PC *pad);
    void __cdecl edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw);
    extern struct eduiiattr_s EdLevelAttr;
    extern i32 pad_disabled;
    extern i32 EdLevelFnt;
    extern i32 EdLevelFntScale;
    extern NUMTL *edLevel2dMtl;
    i32 __cdecl edmainGetCursorEnabled(void);
    NUCAMERA *__cdecl edmainGetCameraPc(void);
    f32 __cdecl edcamGetDist(void);
    void __cdecl EdDrawLineCross(NUVEC *position, f32 size, i32 colour);
    void __cdecl eduiFlushInteracts(void);
    void __cdecl NuRndrLine3dDbgFlush(void);
    void __cdecl eduiGetCursorCoords(f32 *x, f32 *y);
    void __cdecl NuFntSet(i32 font);
    void __cdecl NuFntScale(i32 x, i32 y);
    void __cdecl eduiSetCursorColour(u32 colour);
    void __cdecl eduiRenderCursor(void);
    extern i32 eduiPropTextPos;
    i32 __cdecl eduiUsedAlgPad(FRAME_PAD_PC *pad);
    void __cdecl eduiSetCursorCoords(f32 x, f32 y);
    void __cdecl PcGetMousePositionPc(f32 *x, f32 *y);
    void __cdecl NuCameraCalcRay(f32 screen_x, f32 screen_y, NUVEC *ray_start, NUVEC *ray_end, NUCAMERA *camera);
    eduimenu_s *__cdecl eduiGetActiveMenu(void);
    void __cdecl eduiSetCameraEnabled(i32 enabled);
    extern i32 NewPrim;
    void __cdecl EdDrawLineSphere(NUVEC *center, f32 radius, f32 scale, i32 colour);
    eduiitem_s *__cdecl eduiItemExpanderCreate(u32 data, const void *colours, void *callback, char *text);
    void __cdecl eduiItemExpanderAddChild(eduiitem_s *item, eduiitem_s *child);
    void __cdecl eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item);
    void __cdecl eduiMenuSetAttr(eduimenu_s *menu, const u32 *colours);
    eduimenu_s *__cdecl eduiGetActiveMenuParent(void);
    void __cdecl eduiSetActiveMenu(eduimenu_s *menu);
    void __cdecl EdDrawLineSegment(NUVEC *a, NUVEC *b, i32 colour);
    void __cdecl EdDrawLineCircleY(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void __cdecl EdDrawPolyArrow(NUVEC *from, NUVEC *to, i32 segments, i32 colour, f32 min_radius, f32 max_radius,
                                 f32 radius_scale, f32 radius_bias);
}

STATIC_ASSERT(sizeof(EdStream) == 0x20);

/* A bump allocator over a variptr range; blocks are 16-byte aligned and
 * cleared. */
struct MemoryBuffer {
    VARIPTR *position;
    VARIPTR *end;
    u32 used;
    u32 remaining;

    void *Allocate(u32 size) {
        if (end->addr - position->addr <= size)
            return NULL;
        position->addr = (position->addr + 15) & ~15;
        void *allocation = position->void_ptr;
        position->addr += size;
        memset(allocation, 0, size);
        used += size;
        remaining -= size;
        return allocation;
    }
};
STATIC_ASSERT(sizeof(MemoryBuffer) == 0x10);

template <class T> T *CreateObject(MemoryBuffer *buffer) {
    void *storage = buffer->Allocate(sizeof(T));
    if (storage == NULL)
        return NULL;
    return new (storage) T();
}

/* Writes the editor's block-structured .led format to an open NuFile; each
 * block starts with a size word that EndBlock patches once it is closed. */
class EdFileOutputStream : public EdOutputStream {
  public:
    EdFileOutputStream() {
        block_count = 0;
        file = 0;
    }
    virtual ~EdFileOutputStream() {
    }
    virtual i32 Eat(i32 size, i32 count);
    virtual i32 SerialiseBuffer(void *data, i32 size, i32 count);
    virtual char const *BeginBlock(char const *name);
    virtual void EndBlock();

    void Open(NUFILE handle, i32 stream_version) {
        file = handle;
        version = stream_version;
        if (BeginBlock("StreamInfo")) {
            SerialiseBuffer(&version, sizeof(version), 1);
            EndBlock();
        }
    }

    i32 block_positions[8];
    i32 block_count;
    NUFILE file;
};
STATIC_ASSERT(offsetof(EdFileOutputStream, block_positions) == 0x20);
STATIC_ASSERT(offsetof(EdFileOutputStream, block_count) == 0x40);
STATIC_ASSERT(offsetof(EdFileOutputStream, file) == 0x44);
STATIC_ASSERT(sizeof(EdFileOutputStream) == 0x48);

EdStream::EdStream() {
    memory_buffer = NULL;
    secondary_buffer = NULL;
    mode = 0;
    swap_endianness = 0;
    flags = 0;
}

i32 EdOutputStream::SerialiseString(char **text) {
    return SerialiseString(*text, 0);
}

i32 EdOutputStream::SerialiseString(char **text, i32) {
    return SerialiseString(*text, 0);
}

i32 EdFileOutputStream::Eat(i32, i32) {
    return 0;
}

i32 EdFileOutputStream::SerialiseBuffer(void *data, i32 size, i32 count) {
    i32 result;
    u8 *cursor;
    i32 i;
    if (swap_endianness && size > 1) {
        cursor = (u8 *)data;
        for (i = 0; i < count; i++) {
            switch (size) {
                case 2:
                    EdFileSwapEndianess16(cursor);
                    break;
                case 4:
                    EdFileSwapEndianess32(cursor);
                    break;
            }
            cursor += size;
        }
    }
    result = NuFileWrite(file, data, size * count);
    if (swap_endianness && size > 1) {
        cursor = (u8 *)data;
        for (i = 0; i < count; i++) {
            switch (size) {
                case 2:
                    EdFileSwapEndianess16(cursor);
                    break;
                case 4:
                    EdFileSwapEndianess32(cursor);
                    break;
            }
            cursor += size;
        }
    }
    return result;
}

i32 EdOutputStream::SerialiseString(char *text, i32) {
    i32 length;
    i32 written;
    if (text == NULL)
        text = (char *)"(NULL)";
    length = NuStrLen(text) + 1;
    written = SerialiseBuffer(&length, sizeof(length), 1);
    return written + SerialiseBuffer(text, 1, length);
}

/* Retail keeps an argument-less EdTrace call on the overflow paths of
 * BeginBlock/EndBlock; with EdTrace's empty body in this unit the compiler
 * drops it. */
char const *EdFileOutputStream::BeginBlock(char const *name) {
    i32 position = (i32)NuFilePos(file);
    if (block_count < 8)
        block_positions[block_count++] = position;
    else
        EdTrace(NULL);
    SerialiseBuffer(&position, sizeof(position), 1);
    SerialiseString((char *)name, 0);
    return name;
}

void EdFileOutputStream::EndBlock() {
    i32 position;
    i32 end;
    i32 size;
    if (block_count < 1)
        EdTrace(NULL);
    position = block_positions[--block_count];
    end = (i32)NuFilePos(file);
    size = end - position;
    NuFileSeek(file, position, 0);
    SerialiseBuffer(&size, sizeof(size), 1);
    NuFileSeek(file, end, 0);
}

/* Transform gizmo of the class editor (Android EdManipulator). */
class EdManipulator {
  public:
    virtual i32 Process(EdInputContext &input, ClassObjectList &objects);
    virtual void Render(ClassObjectList &objects);

    u32 unknown_04[3];
    i32 selected_attribute;

    static f32 Scale;
};

/* Property menu of the objects that share one editor class. */
class PropertyMenu : public CSListLink<PropertyMenu> {
  public:
    static void *operator new(size_t size) {
        return theMemoryManager.AllocPool(size, 1);
    }
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(PropertyMenu));
    }

    eduimenu_s *menu;
    EdControl *control;
    ClassObject objects[8];
    i32 object_count;
    i32 order;

    void SelectAttr(i32 selected);
};

class PropertyMenuList {
  public:
    PropertyMenuList() : first(NULL), last(NULL), count(0) {
    }

    PropertyMenu *first;
    PropertyMenu *last;
    i32 count;

    void InsertBefore(PropertyMenu *menu, PropertyMenu *position);
    void InsertSorted(PropertyMenu *menu);
};

struct PropertyMenuMetrics {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
};

/* Edits the properties of the class editor's selected objects through one
 * floating menu per object, thePropertyTool. */
class PropertyTool : public EdTool {
  public:
    PropertyTool() {
        show_type_names = 1;
        menu_attr.background = 0x80000000;
        menu_attr.text = 0x80ff0000;
        menu_attr.highlight = 0x80804040;
        menu_attr.disabled = 0x80404040;
        selected_attr.background = 0x80000000;
        selected_attr.text = 0x80ff0000;
        selected_attr.highlight = 0x80406080;
        selected_attr.disabled = 0x80404040;
        unselected_attr.background = 0x80000000;
        unselected_attr.text = 0x80ff0000;
        unselected_attr.highlight = 0x80707080;
        unselected_attr.disabled = 0x80808080;
    }
    virtual ~PropertyTool() {
    }
    virtual void Initialise(variptr_u &, variptr_u &, i32);
    virtual char const *GetName() {
        return "Property Tool";
    }
    virtual i32 Process(EdInputContext &input);
    virtual void Render();

    PropertyMenuList menus;
    i32 show_type_names;
    eduiiattr_s menu_attr;
    eduiiattr_s selected_attr;
    eduiiattr_s unselected_attr;

    void AddPropertyMenuItems(eduimenu_s *menu, EdClass *ed_class, void *object, eduiitem_s *parent);
    void AutoLocateMenu(PropertyMenu *property_menu);
    PropertyMenu *CreatePropertyMenu(ClassObject &object);
    PropertyMenu *FindItemMenu(PropertyMenu *menu, ClassObjectListEntry *entry);
    PropertyMenu *GetActiveMenu(PropertyMenu *menu);
    eduimenu_s *GetNextDefaultActiveMenu(eduimenu_s *menu);
    i32 ProcessControls(EdInputContext &input);
    i32 ProcessMenu(EdInputContext &input);
    void RefreshMenuControls(PropertyMenu *property_menu);
    void RenderMenu(PropertyMenu *property_menu);
    void SetDefaultActiveMenu(PropertyMenu *menu);
    void ToggleActiveMenu();
};
STATIC_ASSERT(sizeof(PropertyMenu) == 0x78);
STATIC_ASSERT(offsetof(PropertyMenu, objects) == 0x10);
STATIC_ASSERT(offsetof(PropertyMenu, order) == 0x74);
STATIC_ASSERT(sizeof(PropertyTool) == 0x4c);
STATIC_ASSERT(offsetof(PropertyTool, menus) == 0xc);
STATIC_ASSERT(offsetof(PropertyTool, selected_attr) == 0x2c);
STATIC_ASSERT(offsetof(EdManipulator, selected_attribute) == 0x10);

LevelEditor theLevelEditor;
PropertyTool thePropertyTool;
PropertyMenuMetrics menu_startmetrics = {20, 5, 200, 400};

void SerialiseChar(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 1, 1);
}

void SerialiseShort(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 1);
}

void SerialiseInt(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 1);
}

void SerialiseFloat(EdStream &stream, void *data, i32) {
    stream.SerialiseBuffer(data, 4, 1);
}

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

/* Writes a special's name, or reads one back and looks it up in every
 * loaded scene. */
void SerialiseNuHSpecial(EdStream &stream, void *data, i32) {
    NUSPECIALHANDLE *special = static_cast<NUSPECIALHANDLE *>(data);
    if (stream.mode == 2) {
        char *name = NuSpecialGetName(special);
        stream.SerialiseString(name, 0);
    }
    if (stream.mode == 1) {
        char name[128];
        stream.SerialiseString(name, 128);
        NuSpecialFind(NULL, special, name, 0);
        for (i32 index = 0; index < theLevelEditor.reset_pending; index++) {
            if (NuSpecialFind(theLevelEditor.GetScene(index), special, name, 0))
                break;
        }
    }
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

/* Item callback that closes the menu it belongs to. */
void cbEdLevelDestroyOnSelect(eduimenu_s *menu, eduiitem_s *, u32) {
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    if (menu == edLevelActiveMenu)
        edLevelActiveMenu = NULL;
    if (menu == edLevelPinnedMenu)
        edLevelPinnedMenu = NULL;
}

/* Menu close callback for the level editor's menus: tears the menu down,
 * re-enables the camera and falls back to the pinned menu. */
void cbEdLevelDestroy(eduimenu_s *menu, eduimenu_s *) {
    if (menu->flags & 4)
        return;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    eduiSetCameraEnabled(1);
    if (menu == edLevelActiveMenu)
        edLevelActiveMenu = NULL;
    if (menu == edLevelPinnedMenu)
        edLevelPinnedMenu = NULL;
    else if (edLevelPinnedMenu != NULL && edLevelActiveMenu == NULL)
        edLevelActiveMenu = edLevelPinnedMenu;
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

STATIC_ASSERT(sizeof(EdTool) == 0xc);

EdTool::~EdTool() {
}

STATIC_ASSERT(sizeof(BaseEditor) == 0x10);
STATIC_ASSERT(offsetof(BaseEditor, id) == 0xc);

BaseEditor::~BaseEditor() {
}

STATIC_ASSERT(sizeof(EdMember) == 0x8);
STATIC_ASSERT(sizeof(ClassObject) == 0xc);
STATIC_ASSERT(offsetof(ClassObject, reference) == 0x8);
STATIC_ASSERT(sizeof(ClassObjectListEntry) == 0x14);
STATIC_ASSERT(sizeof(ClassObjectList) == 0xc);
STATIC_ASSERT(offsetof(ClassEditor, first_tool) == 0x10);
STATIC_ASSERT(offsetof(ClassEditor, manipulator) == 0x20);
STATIC_ASSERT(offsetof(ClassEditor, selected_objects) == 0x24);
STATIC_ASSERT(offsetof(ClassEditor, current_object) == 0x30);
STATIC_ASSERT(offsetof(ClassEditor, pending_object) == 0x40);
STATIC_ASSERT(offsetof(ClassEditor, menu) == 0x4c);
STATIC_ASSERT(offsetof(ClassEditor, snap_ray) == 0x50);
STATIC_ASSERT(offsetof(ClassEditor, class_filter) == 0x6c);
STATIC_ASSERT(sizeof(ClassEditor) == 0x70);

struct MemBufferAllocator {};

/* Editor subsystem; storage is owned by the level editor buffer, so the
 * deleting destructor never frees. */
class EdSubSystem : public MemBufferAllocator, public CSListLink<EdSubSystem> {
  public:
    static void operator delete(void *) {
    }
    virtual ~EdSubSystem();
    virtual void SubInitialise(variptr_u &, variptr_u &, i32) {
    }
    virtual void SubReset() {
    }
    virtual void SubProcess(f32) {
    }
    virtual void SubRender() {
    }
};
STATIC_ASSERT(sizeof(EdSubSystem) == 0xc);

EdSubSystem::~EdSubSystem() {
}

STATIC_ASSERT(sizeof(EditorSettings) == 0xc);

EditorSettings::~EditorSettings() {
}

EditorSettings::EditorSettings() {
    cursor_radius = 1.0f;
    snap_terrain = 1;
}

void EditorSettings::AddMenuItems(eduimenu_s *menu) {
    eduiMenuAddItem(menu, eduiItemSliderCreate((u32)&cursor_radius, &EdLevelAttr, 0, cbEdLevelSetSliderFloat, 1.0f,
                                               50.0f, cursor_radius, "Cursor Radius"));
    eduiMenuAddItem(menu, eduiItemCheckCreate((size_t)&snap_terrain, &EdLevelAttr, snap_terrain, 0, cbEdLevelToggleInt,
                                              "Snap Terrain"));
    edui_last_item->highlighted = snap_terrain & 1;
}

STATIC_ASSERT(offsetof(EdInputContext, pad) == 0x40);
STATIC_ASSERT(offsetof(EdInputContext, held) == 0x50);
STATIC_ASSERT(offsetof(EdInputContext, pressed) == 0x78);
STATIC_ASSERT(offsetof(EdInputContext, cleared) == 0xf0);
STATIC_ASSERT(offsetof(EdInputContext, values) == 0x118);
STATIC_ASSERT(offsetof(EdInputContext, repeat_times) == 0x1b8);
STATIC_ASSERT(sizeof(EdInputContext) == 0x260);

STATIC_ASSERT(offsetof(LevelEditor, background_colour) == 0x10);
STATIC_ASSERT(offsetof(LevelEditor, overlay_alpha) == 0x34);
STATIC_ASSERT(offsetof(LevelEditor, destroying_objects) == 0x2a1);
STATIC_ASSERT(offsetof(LevelEditor, reset_pending) == 0x2a8);
STATIC_ASSERT(offsetof(LevelEditor, scenes) == 0x2ac);
STATIC_ASSERT(offsetof(LevelEditor, first_editor) == 0x93c);
STATIC_ASSERT(offsetof(LevelEditor, active_editor) == 0x948);
STATIC_ASSERT(offsetof(LevelEditor, current_led_file) == 0xa54);
STATIC_ASSERT(offsetof(LevelEditor, info_x) == 0xee8);
STATIC_ASSERT(offsetof(LevelEditor, info_background) == 0xefc);
STATIC_ASSERT(offsetof(LevelEditor, pad_x) == 0x1000);
STATIC_ASSERT(offsetof(LevelEditor, pad_colour) == 0x1014);
STATIC_ASSERT(offsetof(LevelEditor, pad_text) == 0x101c);
STATIC_ASSERT(offsetof(LevelEditor, settings) == 0x109c);
STATIC_ASSERT(offsetof(LevelEditor, editors_entered) == 0x10a8);
STATIC_ASSERT(offsetof(LevelEditor, active) == 0x10ac);
STATIC_ASSERT(sizeof(LevelEditor) == 0x10b0);

void EdMessage(char const *, ...);

/* Writes the file header, the editor settings and every editor's block. */
i32 LevelEditor::WriteStream(EdFileOutputStream &stream) {
    i32 count;
    BaseEditor *editor;
    ++file_version;
    stream.BeginBlock("FileInfo");
    stream.SerialiseBuffer(&file_version, sizeof(file_version), 1);
    stream.EndBlock();
    stream.BeginBlock("Settings");
    stream.SerialiseBuffer(&settings.cursor_radius, sizeof(settings.cursor_radius), 1);
    stream.SerialiseBuffer(&settings.snap_terrain, sizeof(settings.snap_terrain), 1);
    stream.EndBlock();
    stream.BeginBlock("Editors");
    count = editor_count;
    stream.SerialiseBuffer(&count, sizeof(count), 1);
    for (editor = first_editor; editor != NULL; editor = editor->next) {
        stream.BeginBlock(editor->GetName());
        editor->Serialise(stream);
        stream.EndBlock();
    }
    stream.EndBlock();
    return 1;
}

/* Saves every editable scene to <directory>/<name>.led through a memory
 * file in the editor buffer; returns whether any scene was written. */
i32 LevelEditor::Save() {
    i32 saved_any;
    i32 i;
    if (!active)
        return 0;
    saved_any = 0;
    for (i = 0; i < 10; i++) {
        theClassEditor.PreSaveInitialisation();
        sprintf(save_filename, "%s/%s.led", scenes[i].directory, scenes[i].name);
        if (scenes[i].editable && save_filename) {
            VARIPTR buffer = editor_buffer_begin;
            NUFILE memory_file;
            editor_buffer_cursor = buffer;
            memory_file = NuMemFileOpen(buffer.void_ptr, editor_buffer_end.addr - buffer.addr, 1);
            if (memory_file) {
                EdFileOutputStream stream;
                i32 size;
                NUFILE output_file;
                stream.Open(memory_file, 4);
                stream.unknown_10 = i;
                WriteStream(stream);
                size = (i32)NuFilePos(memory_file);
                stream.file = 0;
                NuFileClose(memory_file);
                output_file = NuFileOpen(save_filename, 1);
                if (output_file) {
                    NuFileWrite(output_file, buffer.void_ptr, size);
                    NuFileClose(output_file);
                    scenes[i].saved = 1;
                    saved_any = 1;
                    continue;
                }
                scenes[i].saved = 0;
            }
            EdMessage(save_filename);
        }
    }
    theClassEditor.PostSaveInitialisation();
    return saved_any;
}

void LevelEditor::Flush() {
    for (BaseEditor *editor = first_editor; editor != NULL; editor = editor->next)
        editor->Flush();
    reset_pending = 0;
    memset(scenes, 0, sizeof(scenes));
}

NUGSCN *LevelEditor::GetScene(i32 index) {
    if ((u32)index <= 9)
        return scenes[index].scene;
    return NULL;
}

LevelEditorScene *LevelEditor::GetEdScene(i32 index) {
    if ((u32)index <= 9)
        return &scenes[index];
    return NULL;
}

/* Labels every pad button whose bit is set in the mask. */
LevelEditor::LevelEditor() : first_editor(NULL), last_editor(NULL), editor_count(0), active(0) {
    reset_pending = 0;
    background_colour[0] = 0.0f;
    background_colour[1] = 0.0f;
    background_colour[2] = 0.0f;
    background_colour[3] = 1.0f;
    overlay_alpha = 0.6f;
    field_0x20 = 0;
    field_0x24 = 0;
    field_0x28 = 1;
    field_0x30 = 1;
    field_0x38 = 1;
    info_x = 0;
    info_y = 0;
    info_width = 640;
    info_height = 448;
    info_colour = 0x80808080;
    info_background = 0x40000000;
    pad_x = 0;
    pad_y = 0;
    pad_width = 640;
    pad_height = 448;
    pad_background = 0x80000000;
    pad_colour = 0x80808080;
    SetPadText(0x1000, "D-Up");
    SetPadText(0x4000, "D-Down");
    SetPadText(0x8000, "D-Left");
    SetPadText(0x2000, "D-Right");
    SetPadText(0x10, "Triangle");
    SetPadText(0x40, "Cross");
    SetPadText(0x80, "Square");
    SetPadText(0x20, "Circle");
    SetPadText(4, "L1");
    SetPadText(1, "L2");
    SetPadText(8, "R1");
    SetPadText(2, "R2");
    SetPadText(0x200, "L3");
    SetPadText(0x400, "R1");
    SetPadText(0x800, "START");
    SetPadText(0x100, "SELECT");
}

/* Editor overlay: the current LED scene name, the camera target cross, the
 * active editor, the info box at the cursor, the menus and the cursor. */
void LevelEditor::Display(ThingRenderData *) {
    f32 cursor_x = 0.1f;
    f32 cursor_y = 0.9f;

    if (editors_entered) {
        if (theLevelEditor.current_led_file != 0xff) {
            LevelEditorScene *scene;
            i32 id = theLevelEditor.current_led_file;
            if (id >= 0 && id < 10)
                scene = &theLevelEditor.scenes[id];
            else
                scene = NULL;
            if (scene != NULL) {
                char *name = scene->name;
                DrawInfoText(&name, 1, static_cast<i32>(640.0f * cursor_x), static_cast<i32>(448.0f * cursor_y),
                             info_width, info_height, info_colour, info_background);
            }
        }
        if (edmainGetCursorEnabled())
            eduiFlushInteracts();
        if (field_0x28) {
            NUCAMERA *camera = edmainGetCameraPc();
            f32 distance = edcamGetDist();
            if (global_camera.far_clip < distance + distance)
                camera->far_clip = distance + distance;
            edcamSet();
            EdDrawBegin(0);
            EdDrawLineCross(reinterpret_cast<NUVEC *>(background_colour), 1.0f, 0xff000000);
            if (NewPrim != 0) {
                if (NewPrim == 2 || NewPrim == 3)
                    NuPrim3DEnd();
                NewPrim = 0;
            }
        }
        if (active_editor != NULL)
            active_editor->Render();
        NuRndrLine3dDbgFlush();
        eduiGetCursorCoords(&cursor_x, &cursor_y);
        DrawInfoText(info_text, 32, static_cast<i32>((1.0f + cursor_x) * 640.0),
                     static_cast<i32>((cursor_y + 1.0f) * 448.0), info_width, info_height, info_colour,
                     info_background);
        if (edLevelActiveMenu != NULL) {
            NuFntSet(EdLevelFnt);
            NuFntScale(EdLevelFntScale, EdLevelFntScale);
            eduiMenuRender(edLevelActiveMenu);
        }
        if (edLevelPinnedMenu != NULL) {
            NuFntSet(EdLevelFnt);
            NuFntScale(EdLevelFntScale, EdLevelFntScale);
            eduiMenuRender(edLevelPinnedMenu);
        }
        eduiSetCursorColour(field_0x2c);
        if (edmainGetCursorEnabled())
            eduiRenderCursor();
    }
}

/* Per-frame editor update: counts the editable scenes, handles the pending
 * menu swaps, updates the input context and camera, runs the active menu and
 * the active editor, and saves on request. */
void LevelEditor::ProcessEvenWhenPaused(ThingProcessData *data) {
    if (!editors_entered)
        return;
    f32 t = data->frame_time;
    FRAME_PAD_PC *pad = reinterpret_cast<FRAME_PAD_PC *>(data->pads[0]);
    text_length = 0;
    memset(info_text, 0, sizeof(info_text));
    memset(reserved_0xf00, 0, 0x80);
    memset(reserved_0xf00 + 0x80, 0, 0x80);
    field_0x2c = -1;
    editable_scene_count = 0;
    if (scenes[0].editable)
        editable_scene_count++;
    if (scenes[1].editable)
        editable_scene_count++;
    if (scenes[2].editable)
        editable_scene_count++;
    if (scenes[3].editable)
        editable_scene_count++;
    if (scenes[4].editable)
        editable_scene_count++;
    if (scenes[5].editable)
        editable_scene_count++;
    if (scenes[6].editable)
        editable_scene_count++;
    if (scenes[7].editable)
        editable_scene_count++;
    if (scenes[8].editable)
        editable_scene_count++;
    if (scenes[9].editable)
        editable_scene_count++;
    if (edLevelActiveMenu == NULL && field_0x38 != 0 && input.GetPress(4) != 0.0f) {
        eduiSetCameraEnabled(0);
        eduiSetDefaultActiveMenu(eduiGetActiveMenu());
        theLevelEditor.CreateMenu();
    }
    if (edLevelDestroyActiveMenu != 0) {
        eduiMenuDestroy(edLevelActiveMenu);
        edLevelActiveMenu = NULL;
        edLevelDestroyActiveMenu = 0;
    }
    if (edLevelDestroyThisMenu != NULL) {
        eduiMenuDestroy(edLevelDestroyThisMenu);
        edLevelDestroyThisMenu = NULL;
    }
    if (edLevelDestroyThisMenu2 != NULL) {
        eduiMenuDestroy(edLevelDestroyThisMenu2);
        edLevelDestroyThisMenu2 = NULL;
    }
    if (edLevelNextMenu != NULL) {
        if (edLevelActiveMenu != NULL)
            eduiMenuDestroy(edLevelActiveMenu);
        edLevelActiveMenu = edLevelNextMenu;
        edLevelNextMenu = NULL;
    }
    NUCAMERA *camera;
    if (field_0x28 != 0)
        camera = edmainGetCameraPc();
    else
        camera = &global_camera;
    input.Update(camera, pad, t, thePropertyTool.menus.first != NULL);
    if (edmainGetCursorEnabled())
        eduiProcessCursor(0.0f, pad);
    if (field_0x28 != 0 && eduiGetCameraEnabled()) {
        if (edlevel_mouseandkeyboard != 0)
            edcamMove(NULL);
        else
            edcamMove(pad);
        edcamGetPosAng(reinterpret_cast<NUVEC *>(background_colour), &field_0x20, &field_0x24);
    }
    if (input.GetPress(17) != 0.0f) {
        overlay_alpha += 0.1f;
        eduiSetFontScale(overlay_alpha, overlay_alpha);
    }
    if (input.GetPress(18) != 0.0f && (f32)EdLevelFntScale > 0.1f) {
        overlay_alpha -= 0.1f;
        eduiSetFontScale(overlay_alpha, overlay_alpha);
    }
    if (edLevelActiveMenu != NULL) {
        NuFntSet(EdLevelFnt);
        NuFntScale(EdLevelFntScale, EdLevelFntScale);
        i32 used = eduiMenuProcess(edLevelActiveMenu, t, pad);
        if (used != 0)
            input.Clear(3);
        if (edLevelActiveMenu != edLevelPinnedMenu && used == 0) {
            if (input.GetPress(3) != 0.0f) {
                eduiSetCameraEnabled(1);
                eduiMenuDestroy(edLevelActiveMenu);
                edLevelActiveMenu = NULL;
                eduiSetDefaultActiveMenu(NULL);
            }
            if (input.GetPress(4) != 0.0f) {
                eduiMenuDestroy(edLevelActiveMenu);
                edLevelActiveMenu = NULL;
                theLevelEditor.CreateMenu();
            }
        }
    } else if (edLevelPinnedMenu != NULL && !thePropertyTool.menus.first != NULL) {
        NuFntSet(EdLevelFnt);
        NuFntScale(EdLevelFntScale, EdLevelFntScale);
        if (eduiMenuProcess(edLevelPinnedMenu, t, pad) != 0)
            return;
    }
    if (active_editor != NULL)
        active_editor->Process(input);
    if (input.GetPress(23) != 0.0f)
        theLevelEditor.Save();
}

void cbEdLevelEditorList(eduimenu_s *menu, eduiitem_s *item, u32);
void cbEdLevelSettingsMenu(eduimenu_s *parent, eduiitem_s *item, u32);

/* Opens the top-level editor menu at the cursor: with Tab held the editor
 * list and options, otherwise the active editor's own items. */
void LevelEditor::CreateMenu() {
    f32 x;
    f32 y;
    eduimenu_s *menu;
    eduiGetCursorCoords(&x, &y);
    x *= 640.0f;
    y *= 448.0f;
    if (NuKeyboard(0x0f) != 0) {
        menu = eduiMenuCreate((i32)x, (i32)y, 100, 200, reinterpret_cast<void *>(EdLevelFnt), cbEdLevelDestroy,
                              "Level Editor");
        if (menu != NULL) {
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelEditorList, "Editors..."));
            eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdLevelSettingsMenu, "Options..."));
            eduiMenuFitWidth(menu, 5);
            eduiMenuFitOnScreen(menu, 1);
        }
    } else {
        menu = eduiMenuCreate((i32)x, (i32)y, 100, 200, reinterpret_cast<void *>(EdLevelFnt), cbEdLevelDestroy, NULL);
        if (menu != NULL) {
            if (active_editor != NULL)
                active_editor->AddMenuItems(menu);
            eduiMenuFitWidth(menu, 5);
            eduiMenuFitOnScreen(menu, 1);
        }
    }
    edLevelActiveMenu = menu;
}

void cbEdLevelEditorSelect(eduimenu_s *, eduiitem_s *item, u32) {
    theLevelEditor.active_editor = static_cast<BaseEditor *>(item->data_ptr);
    edLevelDestroyActiveMenu = 1;
}

/* Lists every registered editor in a submenu next to the selected item. */
void LevelEditor::CreateEditorList(eduimenu_s *parent, eduiitem_s *item) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, "Editor List");
    if (menu == NULL)
        return;
    for (BaseEditor *editor = first_editor; editor != NULL; editor = editor->next)
        eduiMenuAddItem(menu, eduiItemSelCreate((u32)editor, &EdLevelAttr, 0, 0, cbEdLevelEditorSelect,
                                                const_cast<char *>(editor->GetName())));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void cbEdLevelEditorList(eduimenu_s *menu, eduiitem_s *item, u32) {
    theLevelEditor.CreateEditorList(menu, item);
}

/* Saves every editable scene and reports each result in a submenu. */
void cbEdLevelSave(eduimenu_s *parent, eduiitem_s *item, u32) {
    theLevelEditor.Save();
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, "Save File");
    char message[128];
    if (theLevelEditor.active) {
        for (i32 index = 0; index < theLevelEditor.reset_pending; index++) {
            LevelEditorScene *scene = theLevelEditor.GetEdScene(index);
            if (scene != NULL && scene->active && scene->editable && scene->saved) {
                scene->saved = 0;
                if (menu != NULL) {
                    sprintf(message, "%s successfully saved", scene->name);
                    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, message));
                    eduiMenuFitWidth(menu, 5);
                    eduiMenuFitOnScreen(menu, 1);
                }
            } else if (menu != NULL && scene != NULL && scene->active && scene->editable) {
                sprintf(message, "%s : Save Failed, have you got the lock?", scene->name);
                eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, message));
                eduiMenuFitWidth(menu, 5);
                eduiMenuFitOnScreen(menu, 1);
            }
        }
    } else if (menu != NULL) {
        strcpy(message, "Saving not allowed in Debug Mode - change to Edit Mode");
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, message));
        eduiMenuFitWidth(menu, 5);
        eduiMenuFitOnScreen(menu, 1);
    }
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void cbEdLevelSettingsMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, "Options");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelSave, "Save Data"));
    theLevelEditor.settings.AddMenuItems(menu);
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void LevelEditor::SetPadText(i32 buttons, char *text) {
    i32 i;
    for (i = 0; i < 32; i++) {
        if (buttons & (1 << i))
            pad_text[i] = text;
    }
}

/* Draws the non-NULL lines in a box; a x of -1 right-aligns it and a y
 * of -1 puts it at the top. */
void LevelEditor::DrawInfoText(char **lines, i32 count, i32 x, i32 y, i32 available_width, i32, i32 text_colour,
                               i32 background) {
    i32 height = 0;
    i32 width = 0;
    i32 line_height;
    i32 text_offset;
    i32 i;

    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    line_height = static_cast<i32>(NuQFntHeight(system_qfont) * 0.15625);
    text_offset = static_cast<i32>(NuQFntHeight(system_qfont) * 0.125 + NuQFntBaseline(system_qfont));
    for (i = 0; i < count; i++) {
        if (lines[i] != NULL) {
            i32 line_width = static_cast<i32>(NuQFntPrintLenU(system_qfont, lines[i])) >> 4;
            if (line_width > width)
                width = line_width;
            height += line_height;
        }
    }
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
    if (height > 0) {
        if (x <= -1)
            x = available_width - width - 5;
        if (y <= -1)
            y = 5;
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, background, edLevel2dMtl);
        NuQFntPushPrintMode(2);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        NuQFntSetColour(system_qfont, text_colour);
        for (i = 0; i < count; i++) {
            if (lines[i] != NULL) {
                NuQFntPrintEx(system_qfont, x << 4, text_offset + y * 8, 0x10, lines[i]);
                y += line_height;
            }
        }
        NuQFntPopCoordinateSystem();
        NuQFntPopPrintMode();
    }
}

char const *ClassEditor::GetName() {
    return "Class Editor";
}

i32 ClassEditor::ReadBlock(DATAPTR *) {
    return 0;
}

void ClassEditor::WriteBlock(i32) {
}

void ClassEditor::ClearLevel(i32 level) {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        if (theRegistry.classes[i].object_interface != NULL)
            theRegistry.classes[i].object_interface->ClearLevel(level);
    }
}

void ClassEditor::Flush() {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        if (theRegistry.classes[i].object_interface != NULL)
            theRegistry.classes[i].object_interface->Flush();
    }
}

void ClassEditor::Enter() {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        if (theRegistry.classes[i].object_interface != NULL)
            theRegistry.classes[i].object_interface->EnterEditor();
    }
}

void ClassEditor::Exit() {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        if (theRegistry.classes[i].object_interface != NULL)
            theRegistry.classes[i].object_interface->ExitEditor();
    }
}

void ClassEditor::PreSaveInitialisation() {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = &theRegistry.classes[i];
        if (object_class->object_interface != NULL)
            object_class->object_interface->PreSaveInitialisation();
    }
}

void ClassEditor::PostSaveInitialisation() {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = &theRegistry.classes[i];
        if (object_class->object_interface != NULL)
            object_class->object_interface->PostSaveInitialisation();
    }
}

void EdInputContext::Set(i32 input, f32 value, f32 repeat_delay) {
    if (value != 0.0f) {
        pressed[input] = held[input] == 0;
        held[input] = 1;
        values[input] = value;
        if (repeat_times[input] >= repeat_window + current_time) {
            repeat_times[input] = current_time;
            repeated[input] = 1;
        } else if (repeat_times[input] == 0.0f) {
            repeated[input] = 1;
        }
        repeat_times[input] += repeat_delay;
    } else {
        released[input] = held[input] != 0;
        held[input] = 0;
        values[input] = value;
        repeat_times[input] = 0.0f;
    }
}

void EdInputContext::Clear(i32 input) {
    if (input >= 0 && input <= 39) {
        cleared[input] = 1;
        values[input] = 0.0f;
    }
}

f32 EdInputContext::Get(i32 input) {
    if (input >= 0 && input <= 39)
        return values[input];
    return 0.0f;
}

f32 EdInputContext::GetPress(i32 input) {
    if (input >= 0 && input <= 39 && pressed[input] != 0)
        return values[input];
    return 0.0f;
}

f32 EdInputContext::GetRelease(i32 input) {
    if (input >= 0 && input <= 39 && released[input] != 0)
        return values[input];
    return 0.0f;
}

f32 EdInputContext::GetHold(i32 input) {
    if (input >= 0 && input <= 39 && held[input] != 0)
        return values[input];
    return 0.0f;
}

f32 EdInputContext::GetRepeat(i32 input) {
    if (input >= 0 && input <= 39 && repeated[input] != 0)
        return values[input];
    return 0.0f;
}

u8 EdInputUseMouse;
f32 EdInputMouseY = 0.5f;
f32 EdInputMouseX = 0.5f;

/* Samples the camera, the cursor ray and every logical input for this frame.
 * While a property text field is being edited every input is released. */
void EdInputContext::Update(NUCAMERA *camera, FRAME_PAD_PC *new_pad, f32 elapsed, bool) {
    f32 cursor_y = 0.5f;
    f32 cursor_x = 0.5f;
    f32 mouse_y;
    f32 mouse_x;
    __declspec(align(16)) VuVec ray_end;
    VuVec direction;
    i32 i;

    pad = new_pad;
    view_position.x = camera->mtx.m30;
    view_position.y = camera->mtx.m31;
    view_position.z = camera->mtx.m32;
    view_direction.x = camera->mtx.m10;
    view_direction.y = camera->mtx.m11;
    view_direction.z = camera->mtx.m12;
    view_direction.x *= 1000.0;
    view_direction.y *= 1000.0;
    view_direction.z *= 1000.0;
    if (eduiUsedAlgPad(new_pad) > 0) {
        eduiSetCursorCoords(0.5f, 0.5f);
        EdInputUseMouse = 0;
        cursor_x = cursor_y = 0.5f;
    } else {
        PcGetMousePositionPc(&mouse_x, &mouse_y);
        if (mouse_x != EdInputMouseX || mouse_y != EdInputMouseY) {
            EdInputUseMouse = 1;
            EdInputMouseX = mouse_x;
            EdInputMouseY = mouse_y;
        }
        if (EdInputUseMouse) {
            cursor_x = EdInputMouseX;
            cursor_y = EdInputMouseY;
        }
    }
    {
        VuVec *start = &ray_start;
        NuCameraCalcRay(cursor_x, cursor_y, &start->xyz, &ray_end.xyz, camera);
        direction.x = ray_end.x - start->x;
        direction.y = ray_end.y - start->y;
        direction.z = ray_end.z - start->z;
    }
    ray_direction.x = direction.x;
    ray_direction.y = direction.y;
    ray_direction.z = direction.z;
    ray_direction.w = direction.w;
    delta_time = elapsed;

    if (eduiPropTextPos < 0) {
        i32 shift = NuKeyboard(0x2a) | NuKeyboard(0x36) | NuKeyboard(0x1f);
        i32 alt = NuKeyboard(0x38) | NuKeyboard(0xb8) | NuKeyboard(0x39);
        i32 control = NuKeyboard(0x1d) | NuKeyboard(0x9d) | NuKeyboard(0x2e);
        i32 left_click = NuMouseReadButtons() == 1 || (pad->digital_buttons & 0x800);
        i32 square = 0;
        i32 left_select = left_click && !alt && !control;
        i32 right_click;
        i32 key;
        f32 mouse_dx = NuMouseReadXVel();
        f32 mouse_dy = NuMouseReadYVel();
        f32 mouse_dz = NuMouseReadZVel();
        if (!edGetPadDisabled()) {
            if (eduiGetActiveMenu() == NULL) {
                u32 buttons = pad->digital_buttons;
                square = buttons & 0x80;
                Set(38, static_cast<f32>(static_cast<i32>(buttons & 0x40)), elapsed);
            }
            right_click = NuMouseReadButtons() == 2 || (pad->digital_buttons & 0x20);
        } else {
            right_click = NuMouseReadButtons() == 2;
        }
        Set(0, mouse_dx, elapsed);
        Set(1, mouse_dy, elapsed);
        Set(2, mouse_dz, elapsed);
        Set(3, static_cast<f32>(left_select), elapsed);
        Set(4, static_cast<f32>(right_click && !alt && !control), elapsed);
        key = NuKeyboard(0x10);
        Set(5, key, elapsed);
        key = NuKeyboard(0x11);
        Set(6, key, elapsed);
        key = NuKeyboard(0x12);
        Set(7, key, elapsed);
        key = NuKeyboard(0x13);
        Set(8, key, elapsed);
        key = NuKeyboard(0x21);
        Set(9, key, elapsed);
        key = NuKeyboard(0x22);
        Set(10, key, elapsed);
        key = NuKeyboard(0xd2);
        Set(11, key, elapsed);
        key = NuKeyboard(0xd3);
        Set(12, key, elapsed);
        Set(13, static_cast<f32>(alt), elapsed);
        key = NuKeyboard(0x0d);
        Set(14, key, elapsed);
        key = NuKeyboard(0x0c);
        Set(15, key, elapsed);
        Set(16, static_cast<f32>(shift), elapsed);
        key = NuKeyboard(0x1b);
        Set(17, key, elapsed);
        key = NuKeyboard(0x1a);
        Set(18, key, elapsed);
        key = NuKeyboard(0xcd);
        Set(19, key, elapsed);
        key = NuKeyboard(0xcb);
        Set(20, key, elapsed);
        Set(21, static_cast<f32>(shift), elapsed);
        Set(22, static_cast<f32>(control), elapsed);
        Set(23, static_cast<f32>(NuKeyboard(0x1f) && control), elapsed);
        key = NuKeyboard(0x01);
        Set(24, key, elapsed);
        Set(25, static_cast<f32>(left_click && !alt && control), elapsed);
        key = NuKeyboard(2);
        Set(26, key, elapsed);
        key = NuKeyboard(3);
        Set(27, key, elapsed);
        key = NuKeyboard(4);
        Set(28, key, elapsed);
        key = NuKeyboard(5);
        Set(29, key, elapsed);
        key = NuKeyboard(6);
        Set(30, key, elapsed);
        key = NuKeyboard(7);
        Set(31, key, elapsed);
        key = NuKeyboard(8);
        Set(32, key, elapsed);
        key = NuKeyboard(9);
        Set(33, key, elapsed);
        key = NuKeyboard(10);
        Set(34, key, elapsed);
        key = NuKeyboard(11);
        Set(35, key, elapsed);
        Set(37, static_cast<f32>(square), elapsed);
        Set(39, static_cast<f32>(static_cast<i32>(pad->digital_buttons & 0x800)), elapsed);
    } else {
        for (i = 0; i < 40; i++)
            Set(i, 0.0f, elapsed);
    }
    memset(cleared, 0, sizeof(cleared));
}

void EdControl::Refresh() {
}

void EdControl::Process(EdInputContext &) {
}

void EdControl::Render() {
}

void EdBitControl::Refresh() {
}

/* Base interface for placeable scene objects; scene_id is the object-type ID
 * PlaceableHelper::RegisterObjectType assigns (-1 until registered). */
class PlaceableInterface : public EdClassInterface {
  public:
    PlaceableInterface();

    i32 scene_id;
};
STATIC_ASSERT(sizeof(PlaceableInterface) == 0xc);
STATIC_ASSERT(offsetof(PlaceableInterface, scene_id) == 0x8);

PlaceableInterface::PlaceableInterface() : scene_id(-1) {
}

EdString::~EdString() {
    if (data != NULL)
        theMemoryManager.FreePool(data, data[0]);
}

void EdString::Set(const char *value) {
    if (value != NULL) {
        i32 size = NuStrLen(value) + 2;
        if (data != NULL && size > data[0]) {
            theMemoryManager.FreePool(data, data[0]);
            data = NULL;
        }
        if (data == NULL) {
            data = static_cast<char *>(theMemoryManager.AllocPool(size, 1));
            data[0] = static_cast<char>(((size - 1) / 32 + 1) * 32);
        }
        NuStrNCpy(data + 1, value, 250);
    } else if (data != NULL) {
        theMemoryManager.FreePool(data, data[0]);
        data = NULL;
    }
}

i16 Placeable::CurrentLedFile;

Placeable::~Placeable() {
}

/* Layouts of the editor classes declared in saga.h, checked here rather than
 * in the header (assert typedefs there shift codegen in unrelated units). */
STATIC_ASSERT(sizeof(EdString) == 4);
STATIC_ASSERT(sizeof(Placeable) == 0x18);
STATIC_ASSERT(offsetof(Placeable, scene_id) == 0x4);
STATIC_ASSERT(offsetof(Placeable, led_file) == 0x6);
STATIC_ASSERT(offsetof(Placeable, attributes) == 0x8);
STATIC_ASSERT(offsetof(Placeable, name) == 0xc);
STATIC_ASSERT(offsetof(Placeable, params) == 0x10);
STATIC_ASSERT(sizeof(EdType) == 0xc);
STATIC_ASSERT(offsetof(EdType, serialise) == 0x8);
STATIC_ASSERT(sizeof(EdDefunctList) == 0xc);
STATIC_ASSERT(sizeof(EdRegistry) == 0x44);
STATIC_ASSERT(offsetof(EdRegistry, types) == 0x4);
STATIC_ASSERT(offsetof(EdRegistry, type_count) == 0x1c);
STATIC_ASSERT(offsetof(EdRegistry, defunct_objects) == 0x38);
STATIC_ASSERT(sizeof(EdRef) == 0x28);
STATIC_ASSERT(offsetof(EdRef, type_id) == 0xc);
STATIC_ASSERT(offsetof(EdRef, member_offset) == 0x14);
STATIC_ASSERT(offsetof(EdRef, size) == 0x18);
STATIC_ASSERT(offsetof(EdRef, attributes) == 0x1c);
STATIC_ASSERT(offsetof(EdRef, replication_group) == 0x24);

STATIC_ASSERT(sizeof(EdClass) == 0x18);
STATIC_ASSERT(offsetof(EdClass, object_interface) == 0x14);

EdRegistry theRegistry;

void EdRegistry::Initialise(VARIPTR &buffer, VARIPTR &, i32 max_classes, i32 max_types, i32 max_mappings,
                            i32 max_notifiers) {
    type_capacity = max_types;
    class_capacity = max_classes;
    mapping_capacity = max_mappings;
    notifier_capacity = max_notifiers;
    type_count = 0;
    class_count = 0;
    object_count = 0;
    notifier_count = 0;
    types = reinterpret_cast<EdType *>((buffer.addr + 15) & ~15);
    buffer.addr = (buffer.addr + 15) & ~15;
    buffer.addr += sizeof(EdType) * type_capacity;
    memset(types, 0, sizeof(EdType) * type_capacity);
    classes = reinterpret_cast<EdClass *>((buffer.addr + 15) & ~15);
    buffer.addr = (buffer.addr + 15) & ~15;
    buffer.addr += sizeof(EdClass) * class_capacity;
    memset(classes, 0, sizeof(EdClass) * class_capacity);
    mappings = reinterpret_cast<NameMapping *>((buffer.addr + 15) & ~15);
    buffer.addr = (buffer.addr + 15) & ~15;
    buffer.addr += sizeof(NameMapping) * mapping_capacity;
    memset(mappings, 0, sizeof(NameMapping) * mapping_capacity);
    notifiers = reinterpret_cast<EdObjectNotifier **>((buffer.addr + 15) & ~15);
    buffer.addr = (buffer.addr + 15) & ~15;
    buffer.addr += sizeof(EdObjectNotifier *) * notifier_capacity;
    memset(notifiers, 0, sizeof(EdObjectNotifier *) * notifier_capacity);
    initialised = 1;
}

void EdRegistry::Flush() {
    type_count = 0;
    class_count = 0;
    object_count = 0;
}

i32 EdRegistry::RegisterType(char *name, i32 size, void (*serialise)(EdStream &, void *, i32)) {
    i32 index = type_count++;
    EdType *type = &types[index];
    type->name = name;
    type->size = size;
    type->serialise = serialise;
    return index;
}

i32 EdType_Char = 0;
i32 EdType_Short;
i32 EdType_Int = 0;
i32 EdType_Float;
i32 EdType_VuVec;
i32 EdType_VuMtx;
i32 EdType_Enumeration = 0;
i32 EdType_String;
i32 EdType_Colour3 = 0;
i32 EdType_NuHSpecial = 0;
i32 EdType_NuVec;
i32 EdType_NuMtx;

void EdRegistry::RegisterBaseTypes() {
    EdType_Char = RegisterType("Char", 1, SerialiseChar);
    EdType_Short = RegisterType("Short", 2, SerialiseShort);
    EdType_Int = RegisterType("Int", 4, SerialiseInt);
    EdType_Float = RegisterType("Float", 4, SerialiseFloat);
    EdType_VuVec = RegisterType("VuVec", 16, SerialiseVuVec);
    EdType_VuMtx = RegisterType("VuMtx", 64, SerialiseVuMtx);
    EdType_Enumeration = RegisterType("Enum", 4, SerialiseInt);
    EdType_String = RegisterType("String", -1, SerialiseString);
    EdType_Colour3 = RegisterType("Colour3", 12, SerialiseColour3);
    EdType_NuHSpecial = RegisterType("NuHSpecial", 12, SerialiseNuHSpecial);
    EdType_NuVec = RegisterType("NuVec", 12, SerialiseNuVec);
    EdType_NuMtx = RegisterType("NuMtx", 64, SerialiseNuMtx);
}

#pragma push_macro("RegisterClass")
#undef RegisterClass
EdClass *EdRegistry::RegisterClass(char *name, EdClassInterface *object_interface, i32 flags) {
    i32 index = class_count++;
    EdClass *object_class = &classes[index];
    object_class->name = name;
    object_class->object_interface = object_interface;
    object_class->flags = flags;
    if (object_interface != NULL)
        object_interface->object_class = object_class;
    return object_class;
}
#pragma pop_macro("RegisterClass")

i32 EdRegistry::AddMapping(char *source, char *destination) {
    NameMapping *mapping = &mappings[object_count++];
    mapping->source = source;
    mapping->destination = destination;
    return 1;
}

char *EdRegistry::MapName(char *name) {
    for (i32 index = 0; index < object_count; index++) {
        if (NuStrICmp(mappings[index].source, name) == 0)
            return mappings[index].destination;
    }
    return name;
}

i32 EdRegistry::GetTypeId(char *name) {
    for (i32 index = 0; index < type_count; index++) {
        if (NuStrICmp(types[index].name, name) == 0)
            return index;
    }
    return -1;
}

EdType *EdRegistry::GetType(char *name) {
    for (i32 index = 0; index < type_count; index++) {
        if (NuStrICmp(types[index].name, name) == 0)
            return &types[index];
    }
    return NULL;
}

i32 EdRegistry::GetClassId(char *name) {
    for (i32 index = 0; index < class_count; index++) {
        if (NuStrICmp(classes[index].name, name) == 0)
            return index;
    }
    return -1;
}

i32 EdRegistry::GetClassId(EdClass *object_class) {
    return object_class - classes;
}

EdClass *EdRegistry::GetClass(char *name) {
    for (i32 index = 0; index < class_count; index++) {
        if (NuStrICmp(classes[index].name, name) == 0)
            return &classes[index];
    }
    return NULL;
}

EdClass *EdRegistry::GetClass(i32 index) {
    if (index < 0 || index >= class_count)
        return NULL;
    return &classes[index];
}

void EdRegistry::ClassIFaceProcess(i32 class_id, void *object, EdInputContext &context) {
    EdClass *object_class = GetClass(class_id);
    if (object_class != NULL && object_class->object_interface != NULL)
        object_class->object_interface->Process(object, context);
}

void EdRegistry::ClassIFaceProcess(EdClass *object_class, void *object, EdInputContext &context) {
    if (object_class != NULL && object_class->object_interface != NULL)
        object_class->object_interface->Process(object, context);
}

void EdRegistry::ClassIFaceRender(i32 class_id, void *object, i32 flags) {
    EdClass *object_class = GetClass(class_id);
    if (object_class != NULL && object_class->object_interface != NULL)
        object_class->object_interface->Render(object, flags);
}

void EdRegistry::ClassIFaceRender(EdClass *object_class, void *object, i32 flags) {
    if (object_class != NULL && object_class->object_interface != NULL)
        object_class->object_interface->Render(object, flags);
}

void EdRegistry::AddObjectNotifier(EdObjectNotifier *notifier) {
    if (notifier_count < notifier_capacity)
        notifiers[notifier_count++] = notifier;
}

void EdRegistry::NotifyCreateObject(void *object, EdClass *object_class, void *source, i32 index, i32 context,
                                    i32 flags) {
    for (i32 i = 0; i < notifier_count; i++)
        notifiers[i]->NotifyCreateObject(object, object_class, source, index, context, flags);
}

void EdRegistry::NotifyDestroyObject(void *object, EdClass *object_class, i32 index, i32 flags) {
    for (i32 i = 0; i < notifier_count; i++)
        notifiers[i]->NotifyDestroyObject(object, object_class, index, flags);
}

void EdRegistry::NotifyDefunctObject(void *object, EdClass *object_class, i32 flags) {
    for (i32 i = 0; i < notifier_count; i++)
        notifiers[i]->NotifyDefunctObject(object, object_class, flags);
}

void EdRegistry::NotifyReviveObject(void *object, EdClass *object_class, i32 flags) {
    for (i32 i = 0; i < notifier_count; i++)
        notifiers[i]->NotifyReviveObject(object, object_class, flags);
}

void *EdRegistry::CreateObject(EdClassInterface *object_interface, void *source, i32 index, i32 guid, i32 flags) {
    if (guid == 0 && create_object_guid != NULL)
        guid = create_object_guid();
    void *object = object_interface->CreateObject(source, index, flags);
    if (object != NULL) {
        object_interface->SetObjectGuid(object, guid);
        if (!(flags & 2))
            NotifyCreateObject(object, object_interface->object_class, source, index, guid, flags);
    }
    return object;
}

/* Writes (or reads) the byte saying whether an object follows. */
i32 EdClass::SerialiseObjectHeader(EdStream &stream, void *object) {
    u8 present = 0;
    if (stream.mode == 2 && object != NULL)
        present = 1;
    stream.SerialiseBuffer(&present, 1, 1);
    return present;
}

void EdRegistry::DestroyObject(EdClassInterface *object_interface, void *object, i32 index, i32 flags) {
    theLevelEditor.destroying_objects = 1;
    if (object != NULL) {
        if (!(flags & 2))
            NotifyDestroyObject(object, object_interface->object_class, index, flags);
        object_interface->DestroyObject(object, flags);
    }
    theLevelEditor.destroying_objects = 0;
}

struct EdDefunctListEntry {
    EdDefunctListEntry *next;
    EdDefunctListEntry *previous;
    EdClass *object_class;
    void *object;

    EdDefunctListEntry() : next(NULL), previous(NULL) {
    }
};
STATIC_ASSERT(sizeof(EdDefunctListEntry) == 0x10);

void EdRegistry::DefunctObject(EdClassInterface *object_interface, void *object, i32, i32 flags) {
    if (object != NULL) {
        if (!(flags & 2))
            NotifyDefunctObject(object, object_interface->object_class, flags);
        object_interface->DefunctObject(object);
        EdDefunctListEntry *entry = new EdDefunctListEntry;
        entry->object_class = object_interface->object_class;
        entry->object = object;
        entry->next = NULL;
        entry->previous = defunct_objects.last;
        if (defunct_objects.last != NULL)
            defunct_objects.last->next = entry;
        defunct_objects.last = entry;
        if (defunct_objects.first == NULL)
            defunct_objects.first = entry;
        defunct_objects.count++;
    }
}

void *EdRef::GetMemberObject(void *object) {
    void *member = static_cast<u8 *>(object) + member_offset;
    if (member != NULL) {
        if (attributes & 0x40000000)
            return *static_cast<void **>(member);
        return member;
    }
    return NULL;
}

void EdRef::CheckType(i32 requested_type) {
    if (requested_type == type_id || (type_id == EdType_NuVec && requested_type == EdType_VuVec) ||
        (type_id == EdType_NuMtx && requested_type == EdType_VuMtx))
        return;
    EdType *requested = theRegistry.GetType(requested_type);
    EdType *actual = theRegistry.GetType(type_id);
    EdTrace(requested->name, actual->name);
}

i32 EdRef::GetTypeSize(i32 requested_type, i32 data_size) {
    CheckType(requested_type);
    i32 type_size = size > 0 ? size : theRegistry.GetType(type_id)->size;
    if (data_size > 0 && type_size > data_size)
        EdTrace(theRegistry.GetType(type_id)->name, type_size);
    return type_size;
}

void EdRef::GetMemberData(void *object, i32 requested_type, void *data, i32 data_size) {
    void *member = static_cast<u8 *>(object) + member_offset;
    i32 type_size = GetTypeSize(requested_type, data_size);
    if (member != NULL) {
        if (attributes & 0x40000000)
            member = *static_cast<void **>(member);
        PCByteCopy_candidate(data, member, type_size);
    }
}

void EdRef::SetMemberData(void *object, i32 requested_type, void *data, i32 data_size, i16 *) {
    void *member = static_cast<u8 *>(object) + member_offset;
    i32 type_size = GetTypeSize(requested_type, data_size);
    if (member != NULL) {
        if (attributes & 0x40000000)
            member = *static_cast<void **>(member);
        PCByteCopy_candidate(member, data, type_size);
    }
}

/* Attribute-tagged access: 8 asks a matrix member for its translation and 0x100
 * reads a short-sized member. */
i32 EdRef::GetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size) {
    if (attributes & attribute) {
        if ((attribute & 8) && type_id == EdType_VuMtx) {
            __declspec(align(16)) NUMTX matrix;
            GetMemberData(object, EdType_VuMtx, &matrix, 0);
            PCByteCopy_candidate(data, &matrix.m30, sizeof(VuVec));
            return 1;
        }
        if (attribute & 0x100) {
            i16 value;
            GetMemberData(object, EdType_Short, &value, 0);
            PCByteCopy_candidate(data, &value, sizeof(i16));
            return 1;
        }
        GetMemberData(object, requested_type, data, data_size);
        return 1;
    }
    return 0;
}

i32 EdRef::SetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size) {
    if (!(attributes & attribute))
        return 0;
    if ((attribute & 8) && type_id == EdType_VuMtx) {
        __declspec(align(16)) NUMTX matrix;
        GetMemberData(object, EdType_VuMtx, &matrix, 0);
        PCByteCopy_candidate(&matrix.m30, data, sizeof(VuVec));
        SetMemberData(object, EdType_VuMtx, &matrix, 0, NULL);
        return 1;
    }
    if (attribute & 0x100) {
        SetMemberData(object, EdType_Short, &data, sizeof(i16), NULL);
        return 0;
    }
    SetMemberData(object, requested_type, data, data_size, NULL);
    return 1;
}

const VuVec *Placeable::GetInitialPosition() const {
    const VuMtx *matrix = GetInitialTransform();
    return matrix ? reinterpret_cast<const VuVec *>(&matrix->matrix.m30) : NULL;
}

/* Retail only translates a scratch copy; the object is left unchanged. */
void Placeable::SetInitialPosition(const VuVec *position) {
    NUMTX matrix;
    const VuMtx *initial = GetInitialTransform();
    if (initial) {
        matrix = initial->matrix;
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&position->xyz));
    } else {
        NuMtxSetTranslation(&matrix, const_cast<NUVEC *>(&position->xyz));
    }
}

const VuVec *Placeable::GetCurrentPosition() const {
    const VuMtx *matrix = GetCurrentTransform();
    return matrix ? reinterpret_cast<const VuVec *>(&matrix->matrix.m30) : NULL;
}

void Placeable::SetCurrentPosition(const VuVec *position) {
    NUMTX matrix;
    const VuMtx *current = GetCurrentTransform();
    if (current) {
        matrix = current->matrix;
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&position->xyz));
    } else {
        NuMtxSetTranslation(&matrix, const_cast<NUVEC *>(&position->xyz));
    }
}

/* Spline knot editing: position and tangent references of the knot class. */
class KnotHelper : public EdClassInterface {
  public:
    KnotHelper();
    virtual void Flush() {
    }
    virtual i32 GetNumObjects();
    virtual void *GetNextObject(void *object);
    virtual void *CreateObject(void *data, i32 guid, i32 flags);
    virtual void DestroyObject(void *object, i32 flags);
    virtual void Process(void *object, EdInputContext &input);
    virtual void Render(void *object, i32 selected);
    virtual f32 DistanceToObject(VuVec &start, VuVec &end, void *object, EdRef **reference);
    using EdClassInterface::DistanceToObject;

    EdRef *position_ref;
    EdRef *in_tangent_ref;
    EdRef *out_tangent_ref;
};
STATIC_ASSERT(sizeof(KnotHelper) == 0x14);

KnotHelper::KnotHelper() {
}

class SplineObject;

/* One spline control point: its position and the two tangent handles. */
struct SplineKnot {
    SplineKnot *next;
    SplineKnot *previous;
    __declspec(align(16)) VuVec position;
    VuVec in_tangent;
    VuVec out_tangent;
    SplineObject *spline;
    i16 led_file;
};
STATIC_ASSERT(offsetof(SplineKnot, position) == 0x10);
STATIC_ASSERT(offsetof(SplineKnot, spline) == 0x40);
STATIC_ASSERT(sizeof(SplineKnot) == 0x50);

struct SplineKnotList {
    SplineKnot *first;
    SplineKnot *last;
    i32 count;
};

/* A named editor spline; the knot list is followed by its generated points. */
class SplineObject : public CSListLink<SplineObject> {
  public:
    virtual ~SplineObject();

    char name[32];
    SplineKnotList knots;
};
STATIC_ASSERT(offsetof(SplineObject, knots) == 0x2c);

/* The spline object class (Android SplineHelper), theSplineHelper. */
class SplineHelper : public EdClassInterface {
  public:
    virtual void Flush();
    virtual i32 GetNumObjects();
    virtual void *GetNextObject(void *object);
    virtual void *CreateObject(void *data, i32 guid, i32 flags);
    virtual void DestroyObject(void *object, i32 flags);
    using EdClassInterface::GetNextObject;

    SplineObject *first_object;
    SplineObject *last_object;
    i32 object_count;
};
STATIC_ASSERT(offsetof(SplineHelper, first_object) == 0x8);
extern SplineHelper theSplineHelper;

f32 EdManipulator::Scale = 2.0f;
f32 KnotHelperRadius = 0.25f;
f32 LineToPointDistance(VuVec &origin, VuVec &direction, VuVec &point, VuVec *closest);

i32 KnotHelper::GetNumObjects() {
    i32 count = 0;
    for (SplineObject *spline = theSplineHelper.first_object; spline != NULL; spline = spline->next)
        count += spline->knots.count;
    return count;
}

/* Walks the knots of every spline in order. */
void *KnotHelper::GetNextObject(void *current) {
    if (current == NULL) {
        for (SplineObject *spline = theSplineHelper.first_object; spline != NULL; spline = spline->next) {
            if (spline->knots.count != 0)
                return spline->knots.first;
        }
        return NULL;
    }
    SplineKnot *next = static_cast<SplineKnot *>(current)->next;
    if (next == NULL) {
        for (SplineObject *spline = static_cast<SplineKnot *>(current)->spline->next; spline != NULL;
             spline = spline->next) {
            if (spline->knots.count != 0)
                return spline->knots.first;
        }
    }
    return next;
}

/* Unlinks and frees a knot; a spline left without knots is destroyed too. */
void KnotHelper::DestroyObject(void *object, i32) {
    SplineKnot *knot = static_cast<SplineKnot *>(object);
    theLevelEditor.destroying_objects = 1;
    SplineObject *spline = knot->spline;
    if (knot->next != NULL)
        knot->next->previous = knot->previous;
    else
        spline->knots.last = knot->previous;
    if (knot->previous != NULL)
        knot->previous->next = knot->next;
    else
        spline->knots.first = knot->next;
    knot->next = NULL;
    knot->previous = NULL;
    spline->knots.count--;
    theMemoryManager.FreePool(knot, sizeof(SplineKnot));
    if (spline->knots.count == 0) {
        if (spline->next != NULL)
            spline->next->previous = spline->previous;
        else
            theSplineHelper.last_object = spline->previous;
        if (spline->previous != NULL)
            spline->previous->next = spline->next;
        else
            theSplineHelper.first_object = spline->next;
        spline->next = NULL;
        spline->previous = NULL;
        theSplineHelper.object_count--;
        delete spline;
    }
}

/* Nearest of the knot's three handles to the ray, less the handle radius. */
f32 KnotHelper::DistanceToObject(VuVec &start, VuVec &end, void *object, EdRef **reference) {
    SplineKnot *knot = static_cast<SplineKnot *>(object);
    f32 distance = LineToPointDistance(start, end, knot->position, NULL);
    EdRef *nearest = position_ref;
    f32 in_distance = LineToPointDistance(start, end, knot->in_tangent, NULL);
    if (in_distance < distance) {
        distance = in_distance;
        nearest = in_tangent_ref;
    }
    f32 out_distance = LineToPointDistance(start, end, knot->out_tangent, NULL);
    if (out_distance < distance) {
        distance = out_distance;
        nearest = out_tangent_ref;
    }
    distance -= KnotHelperRadius * EdManipulator::Scale;
    distance = distance < 0.0f ? 0.0f : distance;
    if (reference != NULL)
        *reference = nearest;
    return distance;
}

/* Draws the knot's three handles and the tangent lines. */
void KnotHelper::Render(void *object, i32) {
    SplineKnot *knot = static_cast<SplineKnot *>(object);
    i32 colours[3];
    colours[0] = 0x80808080;
    colours[1] = 0x80800000;
    colours[2] = 0x80000080;
    EdDrawBegin(0);
    VuVec *handles = &knot->position;
    for (i32 index = 0; index < 3; index++)
        EdDrawLineCircleY(&handles[index].xyz, KnotHelperRadius * EdManipulator::Scale, colours[index], 8);
    EdDrawLineSegment(&knot->position.xyz, &knot->in_tangent.xyz, -1);
    EdDrawLineSegment(&knot->position.xyz, &knot->out_tangent.xyz, -1);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
}

/* Returns the next object after current that the filter accepts. */
void *EdClassInterface::GetNextObject(void *current, i32 (*filter)(void *)) {
    void *next = GetNextObject(current);
    while (next != NULL && filter(next) == 0)
        next = GetNextObject(next);
    return next;
}

/* Distance from point to the segment origin..origin+direction; optionally
 * returns the closest point on the segment. */
f32 LineToPointDistance(VuVec &origin, VuVec &direction, VuVec &point, VuVec *closest) {
    f32 length = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    f32 inverse_length = 1.0f / length;
    VuVec normal;
    __declspec(align(16)) VuVec offset;
    VuVec on_segment;
    VuVec difference;
    f32 projection;
    f32 distance;
    normal.x = direction.x * inverse_length;
    normal.y = direction.y * inverse_length;
    normal.z = direction.z * inverse_length;
    offset.x = point.x - origin.x;
    offset.y = point.y - origin.y;
    offset.z = point.z - origin.z;
    projection = offset.x * normal.x + offset.y * normal.y + offset.z * normal.z;
    if (projection > 0.0f) {
        if (projection < length) {
            normal.x *= projection;
            normal.y *= projection;
            normal.z *= projection;
            offset.x = origin.x + normal.x;
            offset.y = origin.y + normal.y;
            offset.z = origin.z + normal.z;
        } else {
            offset.x = origin.x + direction.x;
            offset.y = origin.y + direction.y;
            offset.z = origin.z + direction.z;
        }
        on_segment.x = offset.x;
        on_segment.y = offset.y;
        on_segment.z = offset.z;
        on_segment.w = offset.w;
    } else {
        on_segment.x = origin.x;
        on_segment.y = origin.y;
        on_segment.z = origin.z;
        on_segment.w = origin.w;
    }
    difference.x = on_segment.x - point.x;
    difference.y = on_segment.y - point.y;
    difference.z = on_segment.z - point.z;
    distance = NuFsqrt(difference.x * difference.x + difference.y * difference.y + difference.z * difference.z);
    if (closest != NULL)
        *closest = on_segment;
    return distance;
}

/* Intersects the segment origin..origin+direction with plane (xyz normal,
 * w offset); returns 0 when both ends lie on the same side. */
i32 LineToPlaneIntersecion(VuVec &origin, VuVec &direction, VuVec &plane, VuVec *intersection) {
    __declspec(align(16)) VuVec end;
    f32 dot;
    f32 start_distance;
    f32 end_distance;
    end.x = origin.x + direction.x;
    end.y = origin.y + direction.y;
    end.z = origin.z + direction.z;
    dot = origin.x * plane.x + origin.y * plane.y + origin.z * plane.z;
    start_distance = dot + plane.w;
    dot = plane.x * end.x + end.y * plane.y + plane.z * end.z;
    end_distance = dot + plane.w;
    if (start_distance * end_distance < 0.0) {
        if (intersection != NULL) {
            f32 t = -start_distance / (end_distance - start_distance);
            end.x = direction.x * t;
            end.y = direction.y * t;
            end.z = direction.z * t;
            intersection->x = end.x;
            intersection->y = end.y;
            intersection->z = end.z;
            intersection->w = end.w;
            intersection->x += origin.x;
            intersection->y += origin.y;
            intersection->z += origin.z;
        }
        return 1;
    }
    return 0;
}

/* Distance from the cursor ray to the object's position sphere (attribute 8
 * position, attribute 0x40 radius, default radius 1). */
f32 EdClassInterface::DistanceToObject(VuVec &origin, VuVec &direction, void *object, EdRef **reference) {
    __declspec(align(16)) VuVec position;
    f32 radius = 1.0f;
    EdMember member;
    f32 distance = 3.402823466e38f;
    if (object_class->FindMember(&member, object, 8, 1)) {
        member.reference->GetAttributeData(member.object, 8, EdType_VuVec, &position, 0);
        if (object_class->FindMember(&member, object, 0x40, 1))
            member.reference->GetAttributeData(member.object, 0x40, EdType_Float, &radius, 0);
        f32 line_distance = LineToPointDistance(origin, direction, position, NULL);
        distance = line_distance - radius;
        if (distance < 0.0f)
            distance = 0.0f;
    }
    if (reference != NULL)
        *reference = NULL;
    return distance;
}

/* Distance from a point to the object's position sphere (default radius 0). */
f32 EdClassInterface::DistanceToObject(VuVec &point, void *object, EdRef **reference) {
    __declspec(align(16)) VuVec position;
    f32 radius = 0.0f;
    EdMember member;
    f32 distance = 3.402823466e38f;
    if (object_class->FindMember(&member, object, 8, 1)) {
        VuVec delta;
        member.reference->GetAttributeData(member.object, 8, EdType_VuVec, &position, 0);
        if (object_class->FindMember(&member, object, 0x40, 1))
            member.reference->GetAttributeData(member.object, 0x40, EdType_Float, &radius, 0);
        delta.x = point.x - position.x;
        delta.y = point.y - position.y;
        delta.z = point.z - position.z;
        f32 length = NuFsqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        distance = length - radius;
        if (distance < 0.0f)
            distance = 0.0f;
    }
    if (reference != NULL)
        *reference = NULL;
    return distance;
}

/* Writes (or reads) one member description; version 0 streams carry no
 * size. Class-typed members are written with their stream class index. */
void EdRef::Serialise(EdStream &stream, i32 *class_mapping) {
    if (stream.version == 0) {
        stream.SerialiseBuffer(&type_id, sizeof(i32), 1);
        stream.SerialiseString(&name);
        stream.SerialiseBuffer(&member_offset, sizeof(i32), 1);
        stream.SerialiseBuffer(&attributes, sizeof(i32), 1);
        return;
    }
    if ((attributes & 0x80000000) && class_mapping != NULL)
        stream.SerialiseBuffer(&class_mapping[type_id], sizeof(i32), 1);
    else
        stream.SerialiseBuffer(&type_id, sizeof(i32), 1);
    stream.SerialiseString(&name);
    stream.SerialiseBuffer(&member_offset, sizeof(i32), 1);
    stream.SerialiseBuffer(&size, sizeof(i32), 1);
    stream.SerialiseBuffer(&attributes, sizeof(i32), 1);
}

/* Serialises one object's members as described by its own class. */
void EdClass::SerialiseObject(EdStream &stream, void *object) {
    if (stream.BeginBlock("Object") == NULL)
        return;
    if (object != NULL) {
        u8 data[256];
        for (EdRef *member = members; member != NULL; member = member->next) {
            if (stream.flags & 0x400000) {
                if (member->attributes & 0x400000)
                    continue;
            } else if (member->attributes & 0x10000000) {
                continue;
            }
            if (member->attributes & 0x80000000) {
                EdClass *member_class = theRegistry.GetClass(member->type_id);
                member_class->SerialiseObject(stream, member->GetMemberObject(object));
            } else {
                EdType *type = theRegistry.GetType(member->type_id);
                i32 type_size = member->size > 0 ? member->size : type->size;
                if (stream.mode == 2)
                    member->GetMemberData(object, member->type_id, data, sizeof(data));
                type->serialise(stream, data, type_size);
                if (stream.mode == 1)
                    member->SetMemberData(object, member->type_id, data, sizeof(data), NULL);
            }
        }
        if (object_interface != NULL)
            object_interface->SerialiseObject(stream, object);
    }
    stream.EndBlock();
}

/* Appends a member and folds its attribute flags into the class's. */
void EdClass::AddType(EdRef *member) {
    member->next = NULL;
    member->previous = last_member;
    if (last_member != NULL)
        last_member->next = member;
    EdRef *first = members;
    last_member = member;
    if (first == NULL)
        members = member;
    member_count++;
    flags |= member->attributes & 0x17a;
    if (member->attributes & 0x80000000)
        flags |= theRegistry.GetClass(member->type_id)->flags & 0x17a;
}

/* Collects this class and every class-typed member class the stream keeps. */
i32 EdClass::GetStreamClasses(EdStream &stream, i32 *stream_classes, i32 &count, i32 capacity) {
    i32 result = 0;
    if (count < capacity) {
        stream_classes[count] = theRegistry.GetClassId(this);
        count++;
    }
    for (EdRef *member = members; member != NULL; member = member->next) {
        if (!(member->attributes & 0x80000000))
            continue;
        if (stream.flags & 0x400000) {
            if (member->attributes & 0x400000)
                continue;
        } else if (member->attributes & 0x10000000) {
            continue;
        }
        theRegistry.GetClass(member->type_id)->GetStreamClasses(stream, stream_classes, count, capacity);
        result = 1;
    }
    return result;
}

/* Finds a member by name, descending into class-typed members when
 * recursive. */
EdRef *EdClass::FindTypeRef(char *member_name, i32 recursive) {
    for (EdRef *member = members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            if (recursive) {
                EdRef *reference = theRegistry.GetClass(member->type_id)->FindTypeRef(member_name, 1);
                if (reference != NULL)
                    return reference;
                continue;
            }
        }
        if (NuStrICmp(member->name, member_name) == 0)
            return member;
    }
    return NULL;
}

/* Numbers the classes that the stream will write; skipped classes map to -1. */
void EdRegistry::GetStreamClassMapping(EdStream &stream, i32 *mapping, i32 &count, i32) {
    i32 used_classes[64];
    memset(used_classes, 0, sizeof(used_classes));
    for (i32 index = 0; index < class_count; index++) {
        EdClass *object_class = &classes[index];
        if (stream.flags & 0x400000) {
            if (object_class->flags & 0x400000)
                continue;
        } else if (object_class->flags & 0x10000000) {
            continue;
        }
        i32 stream_classes[64];
        i32 stream_class_count = 0;
        object_class->GetStreamClasses(stream, stream_classes, stream_class_count, 64);
        for (i32 stream_class = 0; stream_class < stream_class_count; stream_class++)
            used_classes[stream_classes[stream_class]] = 1;
    }
    count = 0;
    for (i32 index = 0; index < class_count; index++) {
        if (used_classes[index]) {
            mapping[index] = count;
            count++;
        } else {
            mapping[index] = -1;
        }
    }
}

/* Writes the class description (or reads it into members allocated from the
 * stream's secondary buffer). */
void EdClass::Serialise(EdStream &stream, i32 *class_mapping) {
    i32 count;
    if (stream.BeginBlock("Class") == NULL)
        return;
    stream.SerialiseString(&name);
    if (stream.version == 0)
        stream.SerialiseBuffer(&count, sizeof(count), 1);
    if (stream.mode == 2) {
        count = 0;
        for (EdRef *member = members; member != NULL; member = member->next) {
            if (stream.flags & 0x400000) {
                if (member->attributes & 0x400000)
                    continue;
            } else {
                i32 keep = !(member->attributes & 0x10000000);
                if ((member->attributes & 0x80000000) && class_mapping != NULL && class_mapping[member->type_id] == -1)
                    continue;
                if (!keep)
                    continue;
            }
            count++;
        }
        stream.SerialiseBuffer(&count, sizeof(count), 1);
        for (EdRef *member = members; member != NULL; member = member->next) {
            if (stream.flags & 0x400000) {
                if (member->attributes & 0x400000)
                    continue;
            } else {
                i32 keep = !(member->attributes & 0x10000000);
                if ((member->attributes & 0x80000000) && class_mapping != NULL && class_mapping[member->type_id] == -1)
                    continue;
                if (!keep)
                    continue;
            }
            member->Serialise(stream, class_mapping);
        }
    }
    if (stream.mode == 1) {
        if (stream.version < 3) {
            stream.SerialiseBuffer(&count, sizeof(count), 1);
            for (i32 index = 0; index < count; index++) {
                EdRef *member = new (stream.secondary_buffer->Allocate(sizeof(EdRef))) EdRef;
                member->Serialise(stream, NULL);
                AddType(member);
            }
        }
        stream.SerialiseBuffer(&count, sizeof(count), 1);
        for (i32 index = 0; index < count; index++) {
            EdRef *member = new (stream.secondary_buffer->Allocate(sizeof(EdRef))) EdRef;
            if (member != NULL) {
                member->Serialise(stream, NULL);
                AddType(member);
            } else {
                EdTrace("");
            }
        }
    }
    stream.EndBlock();
}

/* Serialises an object against the class description it was written with,
 * skipping members this build no longer has. */
void EdClass::SerialiseObject(EdStream &stream, void *object, EdClass *schema, EdRegistry *registry) {
    if (stream.BeginBlock("Object") == NULL)
        return;
    if (object != NULL) {
        u8 data[256];
        for (EdRef *source = schema->members; source != NULL; source = source->next) {
            EdRef *member = FindTypeRef(source->name, 0);
            if (member != NULL) {
                if (stream.mode != 1) {
                    if (stream.mode != 2)
                        continue;
                    if (stream.flags & 0x400000) {
                        if (member->attributes & 0x400000)
                            continue;
                    } else if (member->attributes & 0x10000000) {
                        continue;
                    }
                }
                if (member->attributes & 0x80000000) {
                    EdClass *source_class = registry->GetClass(source->type_id);
                    EdClass *member_class = theRegistry.GetClass(member->type_id);
                    member_class->SerialiseObject(stream, member->GetMemberObject(object), source_class, registry);
                } else {
                    EdType *type = theRegistry.GetType(member->type_id);
                    i32 type_size = member->size > 0 ? member->size : type->size;
                    if (stream.mode == 2)
                        member->GetMemberData(object, member->type_id, data, sizeof(data));
                    type->serialise(stream, data, type_size);
                    if (stream.mode == 1)
                        member->SetMemberData(object, member->type_id, data, sizeof(data), NULL);
                }
            } else {
                EdType *type = registry->GetType(source->type_id);
                if (source->size > 0) {
                    if (NuStrICmp(type->name, "String") == 0) {
                        i32 length;
                        stream.SerialiseBuffer(&length, sizeof(length), 1);
                        stream.Eat(length, 1);
                    } else {
                        stream.Eat(source->size, 1);
                    }
                } else {
                    stream.Eat(type->size, 1);
                }
            }
        }
        if (object_interface != NULL)
            object_interface->SerialiseObject(stream, object);
    }
    stream.EndBlock();
}

/* Finds an object of this class by its name attribute. */
void *EdClass::FindObject(char *object_name) {
    for (void *object = object_interface->GetNextObject(NULL); object != NULL;
         object = object_interface->GetNextObject(object)) {
        char name[256];
        EdMember member;
        if (FindMember(&member, object, 2, 1) &&
            member.reference->GetAttributeData(member.object, 2, EdType_String, name, sizeof(name)) &&
            NuStrICmp(object_name, name) == 0)
            return object;
    }
    return NULL;
}

/* Whether an object belongs in the stream: its replication group must match
 * and its flags must not exclude it. */
static i32 IsStreamObject(EdStream &stream, EdClass *object_class, void *object) {
    EdMember member;
    i32 object_flags = 0;
    i16 group = 0;
    i32 result = 1;
    if (object_class->FindMember(&member, object, 1, 1))
        member.reference->GetAttributeData(member.object, 1, EdType_Int, &object_flags, 0);
    if (object_class->FindMember(&member, object, 0x100, 1))
        member.reference->GetAttributeData(member.object, 0x100, EdType_Short, &group, 0);
    if (stream.unknown_10 != group)
        result = 0;
    if (stream.flags & 0x400000) {
        if (object_flags & 0x400000)
            result = 0;
    } else if (object_flags & 0x10000000) {
        result = 0;
    }
    return result;
}

/* Writes every streamed object of every class, or reads them back against
 * the class descriptions in source_registry. */
void EdRegistry::SerialiseObjects(EdStream &stream, EdRegistry *source_registry) {
    char object_name[256];
    EdMember member;
    i32 count;
    i32 object_count;
    i32 object_flags;

    if (stream.mode == 2) {
        count = 0;
        for (i32 index = 0; index < class_count; index++) {
            EdClass *object_class = &classes[index];
            if (object_class->object_interface == NULL)
                continue;
            if (stream.flags & 0x400000) {
                if (object_class->flags & 0x400000)
                    continue;
            } else if (object_class->flags & 0x10000000) {
                continue;
            }
            count++;
        }
        stream.SerialiseBuffer(&count, sizeof(count), 1);
        for (i32 index = 0; index < class_count; index++) {
            EdClass *object_class = &classes[index];
            if (object_class->object_interface == NULL)
                continue;
            if (stream.flags & 0x400000) {
                if (object_class->flags & 0x400000)
                    continue;
            } else if (object_class->flags & 0x10000000) {
                continue;
            }
            stream.BeginBlock("ObjectList");
            stream.SerialiseString(&object_class->name);
            object_count = 0;
            void *object;
            for (object = object_class->object_interface->GetNextObject(NULL); object != NULL;
                 object = object_class->object_interface->GetNextObject(object)) {
                if (IsStreamObject(stream, object_class, object))
                    object_count++;
            }
            stream.SerialiseBuffer(&object_count, sizeof(object_count), 1);
            for (object = object_class->object_interface->GetNextObject(NULL); object != NULL;
                 object = object_class->object_interface->GetNextObject(object)) {
                if (!IsStreamObject(stream, object_class, object))
                    continue;
                object_flags = 0;
                if (object_class->FindMember(&member, object, 1, 1))
                    member.reference->GetAttributeData(member.object, 1, EdType_Int, &object_flags, 0);
                stream.SerialiseBuffer(&object_flags, sizeof(object_flags), 1);
                if ((object_flags & 0x2000000) || (object_class->flags & 0x4000000)) {
                    if (object_class->FindMember(&member, object, 4, 1))
                        member.reference->GetAttributeData(member.object, 4, EdType_String, object_name,
                                                           sizeof(object_name));
                    stream.SerialiseString(object_name, sizeof(object_name));
                }
                object_class->SerialiseObject(stream, object);
            }
            stream.EndBlock();
        }
    }

    if (stream.mode == 1) {
        if (stream.version > 3) {
            stream.SerialiseBuffer(&count, sizeof(count), 1);
            for (i32 index = 0; index < count; index++) {
                if (stream.BeginBlock("ObjectList") != NULL) {
                    char class_name[128];
                    stream.SerialiseString(class_name, sizeof(class_name));
                    EdClass *source_class = source_registry->GetClass(class_name);
                    EdClass *object_class = GetClass(MapName(source_class->name));
                    if (object_class != NULL && object_class->object_interface != NULL) {
                        stream.SerialiseBuffer(&object_count, sizeof(object_count), 1);
                        for (i32 object_index = 0; object_index < object_count; object_index++) {
                            void *object;
                            stream.SerialiseBuffer(&object_flags, sizeof(object_flags), 1);
                            if (object_flags & 0x2000000) {
                                stream.SerialiseString(object_name, sizeof(object_name));
                                object = object_class->FindObject(object_name);
                            } else if (object_class->flags & 0x4000000) {
                                stream.SerialiseString(object_name, sizeof(object_name));
                                object = object_class->FindObject(object_name);
                                if (object != NULL)
                                    object = theRegistry.CreateObject(object_class->object_interface, object, 4, 0, 2);
                            } else {
                                object = theRegistry.CreateObject(object_class->object_interface, NULL, 4, 0, 2);
                            }
                            object_class->SerialiseObject(stream, object, source_class, source_registry);
                            if (object != NULL)
                                theRegistry.NotifyCreateObject(object, object_class, NULL, 0, 0, 0);
                        }
                    }
                }
                stream.EndBlock();
            }
        } else {
            for (i32 index = 0; index < source_registry->class_count; index++) {
                EdClass *source_class = source_registry->GetClass(index);
                EdClass *object_class = GetClass(MapName(source_class->name));
                if (object_class != NULL && object_class->object_interface != NULL) {
                    if (stream.BeginBlock("ObjectList") != NULL) {
                        stream.SerialiseBuffer(&object_count, sizeof(object_count), 1);
                        for (i32 object_index = 0; object_index < object_count; object_index++) {
                            void *object;
                            stream.SerialiseBuffer(&object_flags, sizeof(object_flags), 1);
                            if (object_flags & 0x2000000) {
                                stream.SerialiseString(object_name, sizeof(object_name));
                                object = object_class->FindObject(object_name);
                            } else if (object_class->flags & 0x4000000) {
                                stream.SerialiseString(object_name, sizeof(object_name));
                                object = object_class->FindObject(object_name);
                                if (object != NULL)
                                    object = theRegistry.CreateObject(object_class->object_interface, object, 4, 0, 2);
                            } else {
                                object = theRegistry.CreateObject(object_class->object_interface, NULL, 4, 0, 2);
                            }
                            object_class->SerialiseObject(stream, object, source_class, source_registry);
                            if (object != NULL)
                                theRegistry.NotifyCreateObject(object, object_class, NULL, 0, 0, 0);
                        }
                    }
                    stream.EndBlock();
                }
            }
        }
    }
}

/* Writes the type and class descriptions. */
void EdRegistry::Serialise(EdStream &stream) {
    if (stream.BeginBlock("TypeList")) {
        stream.SerialiseBuffer(&type_count, sizeof(type_count), 1);
        for (i32 index = 0; index < type_count; index++) {
            EdType *type = &types[index];
            if (stream.BeginBlock("Type")) {
                stream.SerialiseString(&type->name);
                stream.SerialiseBuffer(&type->size, sizeof(type->size), 1);
                stream.EndBlock();
            }
        }
        stream.EndBlock();
    }
    if (stream.BeginBlock("ClassList")) {
        if (stream.mode == 2) {
            i32 count = 0;
            i32 mapping[64];
            GetStreamClassMapping(stream, mapping, count, 64);
            stream.SerialiseBuffer(&count, sizeof(count), 1);
            for (i32 index = 0; index < class_count; index++) {
                if (mapping[index] != -1)
                    classes[index].Serialise(stream, mapping);
            }
        }
        if (stream.mode == 1) {
            stream.SerialiseBuffer(&class_count, sizeof(class_count), 1);
            for (i32 index = 0; index < class_count; index++)
                classes[index].Serialise(stream, NULL);
        }
        stream.EndBlock();
    }
}

/* Finds the first member carrying any of the attributes, descending into
 * class-typed members when recursive. */
i32 EdClass::FindMember(EdMember *result, void *object, i32 attributes, i32 recursive) {
    EdRef *member;
    for (member = members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            if (recursive) {
                EdClass *member_class;
                void *member_object;
                i32 id = member->type_id;
                if (id >= 0 && id < theRegistry.class_count)
                    member_class = &theRegistry.classes[id];
                else
                    member_class = NULL;
                member_object = member->GetMemberObject(object);
                if (member_class->FindMember(result, member_object, attributes, 1))
                    return 1;
            }
        } else if (member->attributes & attributes) {
            result->object = object;
            result->reference = member;
            return 1;
        }
    }
    return 0;
}

/* Reads an attribute through the picked member first, then through the
 * first class member that carries it. */
i32 ClassObject::GetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size) {
    EdMember member;
    if (reference != NULL && reference->GetAttributeData(object, attribute, requested_type, data, data_size))
        return 1;
    if (ed_class->FindMember(&member, object, attribute, 1))
        return member.reference->GetAttributeData(member.object, attribute, requested_type, data, data_size);
    return 0;
}

i32 ClassObject::SetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size) {
    EdMember member;
    if (reference != NULL && reference->SetAttributeData(object, attribute, requested_type, data, data_size))
        return 1;
    if (ed_class->FindMember(&member, object, attribute, 1))
        return member.reference->SetAttributeData(member.object, attribute, requested_type, data, data_size);
    return 0;
}

/* Toggles whether the LED scene behind a filter menu item is editable. */
void ClassEditor::cbEdFilterLED(eduimenu_s *, eduiitem_s *item, u32) {
    if (item == NULL)
        return;
    LevelEditorScene *scene = theLevelEditor.GetEdScene((u32)item->data >> 6);
    if (scene == NULL)
        return;
    theLevelEditor.current_led_file = -1;
    scene->editable = scene->editable + 1;
    item->highlighted = scene->editable;
}

/* Arrow-head shape of the facing arrow DrawObjectSphere draws. */
f32 ClassEditorArrowMinRadius = 0.1f;
f32 ClassEditorArrowMaxRadius = 0.1f;
f32 ClassEditorArrowRadiusScale = 0.1f;
f32 ClassEditorArrowRadiusBias = 0.1f;

/* Outlines an object with a sphere of its radius attribute (1 by default) and,
 * when it has a transform, an arrow down its facing axis. */
void ClassEditor::DrawObjectSphere(ClassObject &selected, i32 colour) {
    f32 radius;
    __declspec(align(16)) VuMtx transform;
    VuVec position;

    if (selected.object == NULL)
        return;
    if (!selected.GetAttributeData(0x40, EdType_Float, &radius, 0))
        radius = 1.0f;
    if (selected.GetAttributeData(0x10, EdType_VuMtx, &transform, 0)) {
        VuVec difference;
        EdDrawBegin(0);
        EdDrawLineSphere(&transform.matrix.translation, radius, 1.0f, colour);
        {
            VuVec scaled;
            scaled.x = transform.matrix.m20 * radius;
            scaled.y = transform.matrix.m21 * radius;
            scaled.z = transform.matrix.m22 * radius;
            difference.x = transform.matrix.m30 - scaled.x;
            difference.y = transform.matrix.m31 - scaled.y;
            difference.z = transform.matrix.m32 - scaled.z;
        }
        /* Retail copies the difference's unset w along with it. */
        {
            VuVec tip;
            tip.x = difference.x;
            tip.y = difference.y;
            tip.z = difference.z;
            tip.w = difference.w;
            EdDrawPolyArrow(&transform.matrix.translation, &tip.xyz, 4, colour, ClassEditorArrowMinRadius,
                            ClassEditorArrowMaxRadius, ClassEditorArrowRadiusScale, ClassEditorArrowRadiusBias);
        }
    } else {
        if (!selected.GetAttributeData(8, EdType_VuVec, &position, 0))
            return;
        EdDrawBegin(0);
        EdDrawLineSphere(&position.xyz, radius, 1.0f, colour);
    }
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
}

/* Retail's editor trace hook: the format strings were compiled out, leaving an
 * empty variadic call where the registry reported type mismatches. Defined
 * last so the calls above keep retail's codegen; EdRef's constructor lives in
 * edref.cpp because this body would let the compiler drop its one-argument
 * call. */
void EdTrace(const char *, ...) {
}

EdInputContext *EdControl::Input = NULL;

void PropertyTool::Initialise(variptr_u &, variptr_u &, i32) {
}

void PropertyMenu::SelectAttr(i32 selected) {
    for (eduiitem_s *item = menu->first; item != NULL; item = item->next) {
        EdControl *control = static_cast<EdControl *>(item->data_ptr);
        if (control != NULL)
            control->SetMenuItemAttr(selected, item, &thePropertyTool.selected_attr, &thePropertyTool.unselected_attr);
    }
}

/* Keeps the list ordered by PropertyMenu::order. */
void PropertyMenuList::InsertSorted(PropertyMenu *menu) {
    for (PropertyMenu *position = first; position != NULL; position = position->next) {
        if (menu->order < position->order) {
            InsertBefore(menu, position);
            return;
        }
    }
    menu->next = NULL;
    menu->previous = last;
    if (last != NULL)
        last->next = menu;
    last = menu;
    if (first == NULL)
        first = menu;
    count++;
}

void PropertyMenuList::InsertBefore(PropertyMenu *menu, PropertyMenu *position) {
    if (position != NULL) {
        menu->next = position;
        menu->previous = position->previous;
        if (position->previous != NULL)
            position->previous->next = menu;
        else
            first = menu;
        position->previous = menu;
        count++;
    } else {
        menu->next = first;
        menu->previous = NULL;
        if (first != NULL)
            first->previous = menu;
        if (last == NULL)
            last = menu;
        count++;
        first = menu;
    }
}

PropertyMenu *PropertyTool::FindItemMenu(PropertyMenu *menu, ClassObjectListEntry *entry) {
    for (; menu != NULL; menu = menu->next) {
        for (i32 i = 0; i < menu->object_count; i++) {
            if (menu->objects[i].object == entry->object)
                return menu;
        }
    }
    return NULL;
}

void PropertyTool::RefreshMenuControls(PropertyMenu *property_menu) {
    eduiitem_s *item = property_menu->menu->field_0c;
    NuFntSet(EdLevelFnt);
    NuFntScale(EdLevelFntScale, EdLevelFntScale);
    for (; item != NULL; item = item->next) {
        EdControl *control = static_cast<EdControl *>(item->data_ptr);
        if (control != NULL)
            control->Refresh();
        if (item == property_menu->menu->field_10)
            break;
    }
}

PropertyMenu *PropertyTool::GetActiveMenu(PropertyMenu *menu) {
    if (menu == NULL)
        return NULL;
    if (menu->menu == eduiGetActiveMenuParent())
        return menu;
    return GetActiveMenu(menu->next);
}

void PropertyTool::SetDefaultActiveMenu(PropertyMenu *menu) {
    if (menu != NULL && menu->menu != NULL)
        eduiSetActiveMenu(GetMenuActiveChild(menu->menu));
    else
        eduiSetActiveMenu(edLevelActiveMenu != NULL ? edLevelActiveMenu : edLevelPinnedMenu);
}

void PropertyTool::ToggleActiveMenu() {
    PropertyMenu *menu = GetActiveMenu(menus.first);
    if (menu == NULL) {
        if (GetNextDefaultActiveMenu(eduiGetActiveMenu()) == NULL)
            SetDefaultActiveMenu(menus.first);
    } else if (menu == menus.last) {
        eduiSetActiveMenu(NULL);
    } else {
        menu = GetActiveMenu(menus.first);
        SetDefaultActiveMenu(menu != NULL ? menu->next : NULL);
    }
}

i32 PropertyTool::ProcessControls(EdInputContext &input) {
    for (PropertyMenu *menu = menus.first; menu != NULL; menu = menu->next) {
        if (menu->control != NULL)
            menu->control->Process(input);
    }
    return 0;
}

void PropertyTool::AddPropertyMenuItems(eduimenu_s *menu, EdClass *ed_class, void *object, eduiitem_s *parent) {
    for (EdRef *member = ed_class->members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            eduiitem_s *expander = eduiItemExpanderCreate(0, &EdLevelAttr, NULL, member->name);
            if (parent != NULL)
                eduiItemExpanderAddChild(parent, expander);
            else
                eduiMenuAddItem(menu, expander);
            i32 class_id = member->type_id;
            EdClass *member_class =
                class_id >= 0 && class_id < theRegistry.class_count ? &theRegistry.classes[class_id] : NULL;
            AddPropertyMenuItems(menu, member_class, member->GetMemberObject(object), expander);
        } else if (member->control != NULL) {
            eduiitem_s *last = menu->last;
            member->control->AddMenuItem(menu, member, object);
            if (parent != NULL) {
                eduiitem_s *item = last != NULL ? last->next : menu->first;
                while (item != NULL) {
                    eduiitem_s *next = item->next;
                    eduiMenuRemoveItem(menu, item);
                    eduiItemExpanderAddChild(parent, item);
                    item = next;
                }
            }
        }
    }
}

/* Places a new menu right of the others, wrapping back to the left edge. */
void PropertyTool::AutoLocateMenu(PropertyMenu *property_menu) {
    eduimenu_s *menu = property_menu->menu;
    bool use_stored_metrics = true;
    bool placed;
    menu->x = 20;
    menu->y = 5;
    do {
        placed = true;
        for (PropertyMenu *other = menus.first; other != NULL; other = other->next) {
            eduimenu_s *other_menu = other->menu;
            if (other_menu != menu) {
                use_stored_metrics = false;
                if (menu->x + menu->width >= other_menu->x && menu->x <= other_menu->x + other_menu->width) {
                    placed = false;
                    menu->x += 10;
                    break;
                }
            }
        }
        if (use_stored_metrics) {
            menu->x = menu_startmetrics.x;
            menu->y = menu_startmetrics.y;
            menu->width = menu_startmetrics.width;
            menu->height = menu_startmetrics.height;
        }
        if (menu->x + menu->width > 640.0 - 0.0) {
            menu->x = 20;
            menu->y = 5;
            return;
        }
    } while (!placed);
}

PropertyMenu *PropertyTool::CreatePropertyMenu(ClassObject &object) {
    PropertyMenu *property_menu = new PropertyMenu;
    if (property_menu == NULL)
        return NULL;
    PropertyMenuMetrics metrics = menu_startmetrics;
    char name[64];
    char title[128];
    if (!object.GetAttributeData(2, EdType_String, name, sizeof(name)))
        NuStrCpy(name, "no name");
    NuSPrintf(title, "%s - %s", object.ed_class->name, name);
    eduimenu_s *menu = eduiMenuCreate(metrics.x, metrics.y, metrics.width, metrics.height,
                                      reinterpret_cast<void *>(EdLevelFnt), NULL, title);
    if (menu != NULL) {
        AddPropertyMenuItems(menu, object.ed_class, object.object, NULL);
        menu->selected = menu->first;
        eduiMenuFitWidth(menu, 5);
    }
    eduiMenuSetAttr(menu, &selected_attr.background);
    property_menu->menu = menu;
    property_menu->objects[0] = object;
    property_menu->object_count = 1;
    property_menu->control = NULL;
    property_menu->SelectAttr(theClassEditor.manipulator != NULL ? theClassEditor.manipulator->selected_attribute : 0);
    return property_menu;
}

void PropertyTool::RenderMenu(PropertyMenu *property_menu) {
    eduimenu_s *menu = property_menu->menu;
    VuVec start;
    VuVec end;
    if (property_menu->order == -1)
        AutoLocateMenu(property_menu);
    NuCameraCalcRay((f32)((menu->x + menu->width * 0.5) / 640.0), (f32)((menu->y + menu->height * 0.125) / 448.0),
                    (NUVEC *)&start, (NUVEC *)&end, NULL);
    property_menu->objects[0].GetAttributeData(8, EdType_VuVec, &end, 0);
    EdDrawBegin(0);
    EdDrawLineSegment((NUVEC *)&start, (NUVEC *)&end, (i32)&selected_attr);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
    if (property_menu->menu != NULL)
        eduiMenuRender(menu);
    if (menu->selected != NULL) {
        EdControl *control = static_cast<EdControl *>(menu->selected->data_ptr);
        if (control != NULL)
            control->Render();
    }
}

void PropertyTool::Render() {
    PropertyMenu *selected = NULL;
    for (PropertyMenu *menu = menus.last; menu != NULL; menu = menu->previous) {
        if (menu == GetActiveMenu(menus.first))
            selected = menu;
        else
            RenderMenu(menu);
    }
    if (selected != NULL)
        RenderMenu(selected);
}

/* Rebuilds the menu list from the class editor's selection, keeping the
 * menus of objects that stay selected, then lets the menus take the input. */
i32 PropertyTool::ProcessMenu(EdInputContext &input) {
    PropertyMenuList list;
    i32 order = 0;
    PropertyMenu *menu;
    for (menu = menus.first; menu != NULL; menu = menu->next)
        menu->order = order++;
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        ClassObject &object = *reinterpret_cast<ClassObject *>(&entry->ed_class);
        for (menu = menus.first; menu != NULL; menu = menu->next) {
            i32 i;
            for (i = 0; i < menu->object_count; i++) {
                if (menu->objects[i].object == entry->object)
                    break;
            }
            if (i < menu->object_count)
                break;
        }
        if (menu != NULL) {
            menu->object_count = 0;
            menu->objects[menu->object_count++] = object;
            if (menu->next != NULL)
                menu->next->previous = menu->previous;
            else
                menus.last = menu->previous;
            if (menu->previous != NULL)
                menu->previous->next = menu->next;
            else
                menus.first = menu->next;
            menu->next = NULL;
            menu->previous = NULL;
            menus.count--;
            list.InsertSorted(menu);
        } else {
            menu = FindItemMenu(list.first, entry);
            if (menu != NULL) {
                if (menu->object_count < 8)
                    menu->objects[menu->object_count++] = object;
            } else {
                for (PropertyMenu *other = menus.first; other != NULL; other = other->next) {
                    eduimenu_s *other_menu = other->menu;
                    menu_startmetrics.x = other_menu->x > 0 ? other_menu->x : 20;
                    menu_startmetrics.y = other_menu->y > 0 ? other_menu->y : 5;
                    menu_startmetrics.width = other_menu->width;
                    menu_startmetrics.height = other_menu->height;
                }
                menu = CreatePropertyMenu(object);
                menu->menu->x = menu_startmetrics.x;
                menu->menu->y = menu_startmetrics.y;
                menu->menu->width = menu_startmetrics.width;
                menu->menu->height = menu_startmetrics.height;
                menu->order = -1;
                list.InsertSorted(menu);
            }
        }
    }
    while (menus.first != NULL) {
        menu = menus.first;
        if (menu->next != NULL)
            menu->next->previous = menu->previous;
        else
            menus.last = menu->previous;
        if (menu->previous != NULL)
            menu->previous->next = menu->next;
        else
            menus.first = menu->next;
        menu->next = NULL;
        menu->previous = NULL;
        menus.count--;
        eduicbMenuCloseAllexpanders(menu->menu);
        if (menu->menu != NULL) {
            for (eduiitem_s *item = menu->menu->first; item != NULL; item = item->next)
                delete static_cast<EdControl *>(item->data_ptr);
        }
        eduiMenuDestroy(menu->menu);
        delete menu;
    }
    menus = list;
    if (edLevelActiveMenu != NULL)
        return 0;
    EdControl::Input = &input;
    PropertyMenu *active = GetActiveMenu(menus.first);
    if (active != NULL) {
        RefreshMenuControls(active);
        if (eduiMenuProcess(active->menu, input.delta_time, input.pad) != 0)
            return 1;
    }
    for (menu = menus.first; menu != NULL;) {
        PropertyMenu *next = menu->next;
        if (menu != active) {
            RefreshMenuControls(menu);
            if (eduiMenuProcess(menu->menu, input.delta_time, input.pad) != 0)
                return 1;
        }
        menu = next;
    }
    if (input.pad->digital_buttons_pressed & 0x100)
        ToggleActiveMenu();
    EdControl::Input = NULL;
    return 0;
}

i32 PropertyTool::Process(EdInputContext &input) {
    i32 result = ProcessMenu(input);
    ProcessControls(input);
    return result;
}
