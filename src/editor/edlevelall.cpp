#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Level editor (Android editor/edlevelall.cpp). The retail unit is C++; the
 * EdDraw primitives that share it live in eddraw.c. */

extern "C" {
    extern eduimenu_s *edLevelActiveMenu;
    extern eduimenu_s *edLevelPinnedMenu;
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
    void __cdecl EdDrawPolyArrow(NUVEC *from, NUVEC *to, i32 segments, i32 colour, f32 min_radius, f32 max_radius,
                                 f32 radius_scale, f32 radius_bias);
}

class EdStream {
  public:
    EdStream();
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

/* Writes strings as a length-prefixed buffer through SerialiseBuffer. */
class EdOutputStream : public EdStream {
  public:
    EdOutputStream() {
        mode = 2;
    }
    virtual ~EdOutputStream() {
    }
    virtual i32 SerialiseString(char *text, i32 count);
    virtual i32 SerialiseString(char **text);
    virtual i32 SerialiseString(char **text, i32 count);
};

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

class EditorSettings {
  public:
    EditorSettings();
    virtual ~EditorSettings();
    void AddMenuItems(eduimenu_s *menu);
    f32 cursor_radius;
    i32 snap_terrain;
};
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

/* The editor's per-frame input state (Android EdInputContext): the camera
 * view, the cursor ray and 40 logical inputs with press/release/repeat
 * edges. The PC layout has 8 more bytes at the end than Android's. */
struct EdInputContext {
    VuVec view_position;
    VuVec view_direction;
    VuVec ray_start;
    VuVec ray_direction;
    FRAME_PAD_PC *pad;
    f32 delta_time;
    f32 current_time;
    f32 repeat_window;
    u8 held[40];
    u8 pressed[40];
    u8 released[40];
    u8 repeated[40];
    u8 cleared[40];
    f32 values[40];
    f32 repeat_times[40];
    u8 reserved_258[8];

    void Set(i32 input, f32 value, f32 repeat_delay);
    void Clear(i32 input);
    f32 Get(i32 input);
    f32 GetPress(i32 input);
    f32 GetRelease(i32 input);
    f32 GetHold(i32 input);
    f32 GetRepeat(i32 input);
    void Update(NUCAMERA *camera, FRAME_PAD_PC *new_pad, f32 elapsed, bool menu_active);
};
STATIC_ASSERT(offsetof(EdInputContext, pad) == 0x40);
STATIC_ASSERT(offsetof(EdInputContext, held) == 0x50);
STATIC_ASSERT(offsetof(EdInputContext, pressed) == 0x78);
STATIC_ASSERT(offsetof(EdInputContext, cleared) == 0xf0);
STATIC_ASSERT(offsetof(EdInputContext, values) == 0x118);
STATIC_ASSERT(offsetof(EdInputContext, repeat_times) == 0x1b8);
STATIC_ASSERT(sizeof(EdInputContext) == 0x260);

/* The level editor thing (Android LevelEditor), theLevelEditor. The PC input
 * context is 8 bytes larger than Android's, so every field after it sits 8
 * bytes later. */
class LevelEditor : public BaseThing {
  public:
    virtual ~LevelEditor() {
    }
    virtual char const *GetName() {
        return "LevelEditor";
    }
    virtual void ProcessEvenWhenPaused(ThingProcessData *);
    virtual void Display(ThingRenderData *);

    void SetPadText(i32 buttons, char *text);
    NUGSCN *GetScene(i32 index);
    LevelEditorScene *GetEdScene(i32 index);
    i32 WriteStream(EdFileOutputStream &stream);
    i32 Save();
    void DrawInfoText(char **lines, i32 count, i32 x, i32 y, i32 available_width, i32 available_height, i32 text_colour,
                      i32 background);

    f32 background_colour[4];
    i32 field_0x20;
    i32 field_0x24;
    i32 field_0x28;
    i32 field_0x2c;
    i32 field_0x30;
    f32 overlay_alpha;
    i32 field_0x38;
    i32 editor_buffer_from_front;
    EdInputContext input;
    u8 reserved_0x2a0;
    u8 destroying_objects;
    u8 reserved_0x2a2[2];
    i32 editable_scene_count;
    i32 reset_pending;
    LevelEditorScene scenes[10];
    BaseEditor *first_editor;
    BaseEditor *last_editor;
    i32 editor_count;
    BaseEditor *active_editor;
    char save_filename[0x80];
    char editor_filename[0x80];
    i32 file_version;
    i32 multi_load_active;
    i16 current_led_file;
    u8 reserved_0xa56[2];
    VARIPTR editor_buffer_cursor;
    VARIPTR editor_buffer_begin;
    VARIPTR editor_buffer_end;
    char text_buffer[0x400];
    i32 text_length;
    char *info_text[32];
    i32 info_x;
    i32 info_y;
    i32 info_width;
    i32 info_height;
    u32 info_colour;
    u32 info_background;
    u8 reserved_0xf00[0x100];
    i32 pad_x;
    i32 pad_y;
    i32 pad_width;
    i32 pad_height;
    u32 pad_background;
    u32 pad_colour;
    u8 reserved_0x1018[4];
    char *pad_text[32];
    EditorSettings settings;
    i32 editors_entered;
    i32 active;
};
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
i32 EdType_Short;
i32 EdType_Float;
i32 EdType_VuVec;
i32 EdType_VuMtx;
i32 EdType_NuVec;
i32 EdType_NuMtx;

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
