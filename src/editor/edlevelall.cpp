#include "saga.h"
#include <new>
#include <string.h>
#include <stdio.h>

/* Level editor (Android editor/edlevelall.cpp). The retail unit is C++; the
 * EdDraw primitives that share it live in eddraw.c. */

extern "C" {
    extern eduimenu_s *edLevelActiveMenu;
    void eduiAddPropTextPickEnt(eduimenu_s *menu, eduiitem_s *item);
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
    extern NUMTL *edLevel3dMtl;
    eduiitem_s *__cdecl eduiItemSeparatorCreate(u32 data, const void *colours);
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
    eduiitem_s *__cdecl eduiItemPropCreate(u32 data, void *colours, void *selected, void *changed, void *button,
                                           i32 button_type, char *text, char *value);
    eduiitem_s *__cdecl eduiItemPropCreateEx(u32 data, void *colours, void *selected, void *changed, void *button,
                                             i32 button_type, char *text, char *value, i32 extra_data);
    i32 __cdecl eduiItemPropSetText(edui_prop_s *item, char *text);
    void __cdecl eduiItemExpanderAddChild(eduiitem_s *item, eduiitem_s *child);
    void __cdecl eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item);
    void __cdecl eduiMenuSetAttr(eduimenu_s *menu, const u32 *colours);
    eduimenu_s *__cdecl eduiGetActiveMenuParent(void);
    void __cdecl eduiSetActiveMenu(eduimenu_s *menu);
    void __cdecl EdDrawLineSegment(NUVEC *a, NUVEC *b, i32 colour);
    void __cdecl EdDrawLineCircleY(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void __cdecl EdDrawLineCircleX(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void __cdecl EdDrawLineCircleZ(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void __cdecl EdDrawLineCube(NUMTX *transform, f32 size, i32 colour);
    void __cdecl EdDrawPolySector(NUVEC *center, f32 radius, i32 axis, i32 first_angle, i32 last_angle, i32 colour,
                                  i32 segments);
    f32 __cdecl eduiGetAnalougePadValue(FRAME_PAD_PC *pad);
    eduiitem_s *__cdecl eduiItemFilterCreate(u32 data, void *colours, char *text, char *value);
    void __cdecl eduiMenuSortItemsByTxt(eduimenu_s *menu);
    void __cdecl edcamSetDist(f32 distance);
    void __cdecl EdDrawPolyArrow(NUVEC *from, NUVEC *to, i32 segments, i32 colour, f32 min_radius, f32 max_radius,
                                 f32 radius_scale, f32 radius_bias);
}

STATIC_ASSERT(sizeof(EdStream) == 0x20);

/* A bump allocator over a variptr range; blocks are 16-byte aligned and
 * cleared. */
struct MemoryBuffer {
    MemoryBuffer() {
        position = NULL;
        end = NULL;
    }
    MemoryBuffer(VARIPTR *begin, VARIPTR *finish) {
        position = begin;
        end = finish;
        used = 0;
        remaining = finish->addr - begin->addr;
    }

    VARIPTR *position;
    VARIPTR *end;
    u32 used;
    u32 remaining;

    void *Allocate(u32 size) {
        void *allocation = NULL;
        if (size < end->addr - position->addr) {
            position->addr = (position->addr + 15) & ~15;
            allocation = position->void_ptr;
            position->addr += size;
            memset(allocation, 0, size);
            used += size;
            remaining -= size;
        }
        return allocation;
    }
};
STATIC_ASSERT(sizeof(MemoryBuffer) == 0x10);

template <class T> T *CreateObject(MemoryBuffer *buffer) {
    return new (buffer->Allocate(sizeof(T))) T();
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

/* Reads the .led block format back. Block names are kept in one pool;
 * BeginBlock reads a block header once and keeps it pending until a caller
 * asks for that block by name. */
class EdFileInputStream : public EdInputStream {
  public:
    struct Block {
        i32 position;
        i32 size;
        i32 name_offset;
    };

    EdFileInputStream(MemoryBuffer *buffer, MemoryBuffer *secondary) : EdInputStream(buffer, secondary) {
        block_count = 0;
        file = 0;
        name_length = 0;
        pending = 0;
    }
    virtual ~EdFileInputStream() {
    }
    virtual i32 Eat(i32 size, i32 count);
    virtual i32 SerialiseBuffer(void *data, i32 size, i32 count);
    virtual char const *BeginBlock(char const *name);
    virtual void EndBlock();
    void Open(NUFILE handle, i32 stream_version);

    Block blocks[8];
    i32 block_count;
    char block_names[256];
    i32 name_length;
    Block pending_block;
    i32 pending;
    NUFILE file;
};
STATIC_ASSERT(offsetof(EdFileInputStream, blocks) == 0x20);
STATIC_ASSERT(offsetof(EdFileInputStream, block_count) == 0x80);
STATIC_ASSERT(offsetof(EdFileInputStream, block_names) == 0x84);
STATIC_ASSERT(offsetof(EdFileInputStream, name_length) == 0x184);
STATIC_ASSERT(offsetof(EdFileInputStream, pending_block) == 0x188);
STATIC_ASSERT(offsetof(EdFileInputStream, pending) == 0x194);
STATIC_ASSERT(offsetof(EdFileInputStream, file) == 0x198);
STATIC_ASSERT(sizeof(EdFileInputStream) == 0x19c);

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

void EdMessage(char const *, ...);

i32 EdInputStream::SerialiseString(char **text, i32 count) {
    if (*text != NULL)
        return SerialiseString(*text, count);
    return SerialiseString(text);
}

/* A string longer than the caller's buffer is read through a scratch buffer,
 * truncated and reported. */
i32 EdInputStream::SerialiseString(char *text, i32 count) {
    i32 length;
    char buffer[256];
    SerialiseBuffer(&length, sizeof(length), 1);
    if (length > count) {
        SerialiseBuffer(buffer, 1, sizeof(buffer));
        NuStrNCpy(text, buffer, count);
        text[count - 1] = 0;
        EdMessage(buffer, text);
        return count;
    }
    return SerialiseBuffer(text, 1, length);
}

i32 EdInputStream::SerialiseString(char **text) {
    i32 length;
    SerialiseBuffer(&length, sizeof(length), 1);
    char *allocation = (char *)memory_buffer->Allocate(length);
    *text = allocation;
    return SerialiseBuffer(allocation, 1, length);
}

EdInputStream::EdInputStream(MemoryBuffer *buffer, MemoryBuffer *secondary) : EdStream(buffer, secondary) {
    mode = 1;
}

i32 EdFileInputStream::Eat(i32 size, i32 count) {
    return (i32)NuFileSeek(file, size * count, 1);
}

i32 EdFileInputStream::SerialiseBuffer(void *data, i32 size, i32 count) {
    i32 result;
    u8 *cursor;
    i32 i;
    result = NuFileRead(file, data, size * count);
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

/* Retail reports through EdMessage with the block names alone; the format
 * strings are not in the executable. */
char const *EdFileInputStream::BeginBlock(char const *name) {
    i32 position = (i32)NuFilePos(file);
    char *block_name;
    if (block_count > 0) {
        if (block_count < 1)
            EdTrace(NULL);
        Block &parent = blocks[block_count - 1];
        if (position >= parent.position + parent.size) {
            EdMessage(block_names + parent.name_offset);
            return NULL;
        }
    }
    if (!pending) {
        pending_block.position = position;
        pending_block.name_offset = name_length;
        pending = 1;
        SerialiseBuffer(&pending_block.size, sizeof(pending_block.size), 1);
        block_name = block_names + name_length;
        SerialiseString(block_name, sizeof(block_names) - name_length);
        name_length += NuStrLen(block_name) + 1;
    } else {
        block_name = block_names + pending_block.name_offset;
    }
    if (name != NULL && NuStrICmp(name, block_name)) {
        EdMessage(name, block_name);
        return NULL;
    }
    if (block_count < 8)
        blocks[block_count++] = pending_block;
    else
        EdTrace(NULL);
    pending = 0;
    return block_name;
}

void EdFileInputStream::EndBlock() {
    if (block_count < 1)
        EdTrace(NULL);
    Block block = blocks[--block_count];
    i32 end = block.position + block.size;
    if ((i32)NuFilePos(file) != end)
        NuFileSeek(file, end, 0);
    name_length = block.name_offset;
}

/* A version mismatch is reported with the two numbers only. */
void EdFileInputStream::Open(NUFILE handle, i32 stream_version) {
    file = handle;
    version = 0;
    if (BeginBlock("StreamInfo")) {
        SerialiseBuffer(&version, sizeof(version), 1);
        EndBlock();
    }
    if (version != stream_version)
        EdMessage((char const *)version, stream_version);
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
    i32 attribute_selected;
    i32 selected_axis;
    u32 unknown_1c;
    VuVec first_axis;
    VuVec second_axis;
    VuVec last_point;
    VuVec delta;
    VuVec cursor;
    i32 start_angle;
    i32 last_angle;
    i32 angle_delta;
    u32 unknown_7c;

    static f32 Scale;
    static i32 AxisColour[8];

    void GetAxisLocators(VuVec &origin, VuVec *points, VuMtx *matrix);
    void DrawAxis(VuVec &origin, VuMtx *matrix);
    void DrawRotator(VuVec &origin);
    i32 SelectAxis(EdInputContext &input, VuVec &origin, VuVec &first, VuVec &second, VuMtx *matrix);
    i32 SelectRotator(EdInputContext &input, VuVec &center, VuVec &plane);
};

/* The move, rotate and scale gizmos; each edits a different attribute. */
class EdManMove : public EdManipulator {
  public:
    EdManMove() {
        selected_attribute = 8;
    }
    virtual i32 Process(EdInputContext &input, ClassObjectList &objects);
    virtual void Render(ClassObjectList &objects);
};

class EdManRotate : public EdManipulator {
  public:
    EdManRotate() {
        selected_attribute = 0x10;
    }
    virtual i32 Process(EdInputContext &input, ClassObjectList &objects);
    virtual void Render(ClassObjectList &objects);
    i32 RotateItem(EdInputContext &input, ClassObjectList &objects, i32 angle, i32 axis);
};

class EdManScale : public EdManipulator {
  public:
    EdManScale() {
        selected_attribute = 0x20;
    }
    virtual i32 Process(EdInputContext &input, ClassObjectList &objects);
    virtual void Render(ClassObjectList &objects);
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
    void InsertAfter(PropertyMenu *menu, PropertyMenu *position);
    void Remove(PropertyMenu *menu);
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
    void SelectAttr(i32 selected);

    PropertyMenuList menus;
    i32 show_type_names;
    eduiiattr_s menu_attr;
    eduiiattr_s selected_attr;
    eduiiattr_s unselected_attr;

    void AddPropertyMenuItems(eduimenu_s *menu, EdClass *ed_class, void *object, eduiitem_s *parent);
    void AutoLocateMenu(PropertyMenu *property_menu);
    PropertyMenu *CreatePropertyMenu(ClassObject &object);
    PropertyMenu *FindItemMenu(PropertyMenu *menu, ClassObjectListEntry *entry);
    void BringToFront(PropertyMenu *menu);
    PropertyMenu *GetActiveMenu(PropertyMenu *menu);
    PropertyMenu *GetNextActiveMenu();
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
STATIC_ASSERT(offsetof(EdManipulator, last_point) == 0x40);
STATIC_ASSERT(offsetof(EdManipulator, cursor) == 0x60);
STATIC_ASSERT(offsetof(EdManipulator, selected_axis) == 0x18);
STATIC_ASSERT(offsetof(EdManipulator, first_axis) == 0x20);
STATIC_ASSERT(offsetof(EdManipulator, start_angle) == 0x70);
STATIC_ASSERT(sizeof(EdManipulator) == 0x80);
STATIC_ASSERT(sizeof(EdControl) == 0x10);
STATIC_ASSERT(offsetof(EdControl, item) == 0x4);
STATIC_ASSERT(offsetof(EdControl, object) == 0xc);
STATIC_ASSERT(sizeof(EdEnumControl) == 0x14);
STATIC_ASSERT(offsetof(EdEnumControl, items) == 0x10);
STATIC_ASSERT(sizeof(EdBitControl) == 0x18);
STATIC_ASSERT(offsetof(EdBitControl, bit_mask) == 0x14);

/* The class editor's default tool: picks, creates and deletes objects
 * under the cursor ray. */
class CursorTool : public EdTool {
  public:
    virtual ~CursorTool() {
    }
    virtual void Initialise(variptr_u &, variptr_u &, i32) {
    }
    virtual char const *GetName() {
        return "Cursor Tool";
    }
    virtual i32 Process(EdInputContext &input);
    virtual void Render();
};

/* Spline editing tool; its own processing is empty. */
class SplineTool : public PropertyTool {
  public:
    virtual ~SplineTool() {
    }
    virtual void Initialise(variptr_u &, variptr_u &, i32) {
    }
    virtual char const *GetName() {
        return "Spline Tool";
    }
    virtual i32 Process(EdInputContext &) {
        return 0;
    }
    virtual void Render() {
    }
};
STATIC_ASSERT(sizeof(CursorTool) == 0xc);
STATIC_ASSERT(sizeof(SplineTool) == 0x4c);

extern EdManipulator theDefaultManipulator;

inline ClassEditor::ClassEditor() : first_tool(NULL), last_tool(NULL), tool_count(0), selected_objects() {
    snap_distance = 10.0f;
    manipulator = &theDefaultManipulator;
    class_filter = -1;
}

ClassEditor theClassEditor;
CursorTool theCursorTool;
static i32 class_editor_create_pending = 0;
LevelEditor theLevelEditor;
PropertyTool thePropertyTool;
EdManipulator theDefaultManipulator;
EdManMove theMoveManipulator;
EdManRotate theRotateManipulator;
EdManScale theScaleManipulator;
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
    eduimenu_s *next = NULL;
    if (menu == NULL)
        next = edLevelActiveMenu;
    else if (menu == edLevelActiveMenu)
        next = edLevelPinnedMenu;
    return next;
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

char *EdLevelFileExtension = ".led";

void LevelEditor::SetSaveFilename(char *name) {
    if (name != NULL) {
        NuStrCpy(save_filename, name);
        NuStrCpy(editor_filename, save_filename);
        char *extension = NuStrRChr(editor_filename, '.');
        if (extension != NULL)
            *extension = 0;
        NuStrCat(editor_filename, EdLevelFileExtension);
    } else {
        save_filename[0] = 0;
        editor_filename[0] = 0;
    }
}

/* Scene -1 (no scene) is always editable; an inactive scene is too. */
i32 LevelEditor::IsEditable(i32 index) {
    if (index == -1)
        return 1;
    if ((u32)index <= 9 && (scenes[index].editable || !scenes[index].active))
        return 1;
    return 0;
}

i32 LevelEditor::FindSceneId(char *name) {
    for (i32 i = 0; i < 10; i++) {
        if (NuStrICmp(scenes[i].name, name) == 0)
            return i;
    }
    return -1;
}

/* Reads FileInfo, Settings and every editor block that names a registered
 * editor; unknown editor blocks are skipped. */
i32 LevelEditor::ReadStream(EdFileInputStream &stream) {
    if (stream.BeginBlock("FileInfo")) {
        stream.SerialiseBuffer(&file_version, sizeof(file_version), 1);
        stream.EndBlock();
    }
    if (stream.BeginBlock("Settings")) {
        stream.SerialiseBuffer(&settings.cursor_radius, sizeof(settings.cursor_radius), 1);
        stream.SerialiseBuffer(&settings.snap_terrain, sizeof(settings.snap_terrain), 1);
        stream.EndBlock();
    }
    if (stream.BeginBlock("Editors")) {
        i32 count;
        stream.SerialiseBuffer(&count, sizeof(count), 1);
        for (i32 index = 0; index < count; index++) {
            char const *name = stream.BeginBlock(NULL);
            for (BaseEditor *editor = first_editor; editor != NULL; editor = editor->next) {
                if (NuStrICmp(editor->GetName(), name) == 0) {
                    editor->Serialise(stream);
                    break;
                }
            }
            stream.EndBlock();
        }
        stream.EndBlock();
    }
    return 1;
}

/* Loads <name>.led into the scene of the same name: objects go to buffer,
 * the file image and scratch data to the editor buffer. Returns the scene
 * index, or -1. */
i32 LevelEditor::Load(char *filename, variptr_u *buffer, variptr_u *buffer_end, i32 flags) {
    MemoryBuffer source(buffer, buffer_end);
    MemoryBuffer scratch;
    i32 result = -1;
    if (editor_buffer_from_front == 0) {
        editor_buffer_end = *buffer_end;
        editor_buffer_begin.addr = buffer_end->addr - 0x20000;
    }
    char *basename = NuStrRChr(filename, '/') + 1;
    i32 basename_length = NuStrLen(basename);
    char scene_name[32];
    char directory[128];
    NuStrNCpy(scene_name, basename, basename_length - 3);
    NuStrNCpy(directory, filename, NuStrLen(filename) - basename_length);
    Placeable::CurrentLedFile = (i16)FindSceneId(scene_name);
    if (Placeable::CurrentLedFile != -1)
        NuStrCpy(scenes[Placeable::CurrentLedFile].directory, directory);
    current_led_file = Placeable::CurrentLedFile;
    editor_buffer_cursor = editor_buffer_begin;
    scratch = MemoryBuffer(&editor_buffer_cursor, &editor_buffer_end);
    SetSaveFilename(filename);
    if (!multi_load_active)
        theClassEditor.PreLoadInitialisation(&source, &scratch);
    void *file_data = scratch.position->void_ptr;
    i32 file_size = NuFileLoadBuffer(save_filename, file_data, scratch.remaining);
    if (file_size > 0) {
        if ((u32)file_size < scratch.end->addr - scratch.position->addr) {
            scratch.position->addr += file_size;
            scratch.used += file_size;
            scratch.remaining -= file_size;
        }
        NUFILE file = NuMemFileOpen(file_data, file_size, 0);
        if (file) {
            EdFileInputStream stream(&source, &scratch);
            if (flags)
                stream.flags = 0x200000;
            stream.Open(file, 4);
            i32 loaded = ReadStream(stream);
            stream.file = 0;
            NuFileClose(file);
            if (loaded) {
                scenes[Placeable::CurrentLedFile].editable = 1;
                result = Placeable::CurrentLedFile;
                Placeable::CurrentLedFile = -1;
            } else {
                result = -1;
            }
        }
    } else {
        EdMessage(save_filename);
    }
    if (!multi_load_active)
        theClassEditor.PostLoadInitialisation(&source, &scratch);
    return result;
}

/* Loads a .led image already in memory (file_begin..file_end). */
i32 LevelEditor::LoadState(variptr_u *buffer, variptr_u *buffer_end, variptr_u *scratch_begin, variptr_u *scratch_end,
                           variptr_u *file_begin, variptr_u *file_end) {
    i32 result;
    loading = 1;
    MemoryBuffer source;
    MemoryBuffer scratch(scratch_begin, scratch_end);
    result = 0;
    if (!multi_load_active)
        theClassEditor.PreLoadInitialisation(&source, &scratch);
    NUFILE file = NuMemFileOpen(file_begin->void_ptr, file_end->addr - file_begin->addr, 0);
    if (file) {
        EdFileInputStream stream(&source, &scratch);
        stream.Open(file, 4);
        stream.flags = 0x400000;
        result = ReadStream(stream);
        stream.file = 0;
        NuFileClose(file);
    }
    if (!multi_load_active)
        theClassEditor.PostLoadInitialisation(&source, &scratch);
    loading = 0;
    return result;
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

void ClassEditor::PreLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second) {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = &theRegistry.classes[i];
        if (object_class->object_interface != NULL)
            object_class->object_interface->PreLoadInitialisation(first, second);
    }
}

void ClassEditor::PostLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second) {
    i32 i;
    for (i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = &theRegistry.classes[i];
        if (object_class->object_interface != NULL)
            object_class->object_interface->PostLoadInitialisation(first, second);
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

EdControl::~EdControl() {
}

void EdControl::Refresh() {
}

void EdControl::Process(EdInputContext &) {
}

void EdControl::Render() {
}

char *EdEnumControl::GetEnumString(i32 value) {
    for (Item *entry = items; entry->name != NULL; entry++) {
        if (entry->value == value)
            return entry->name;
    }
    return "Unknown";
}

/* Accepts a typed choice by name or by number; anything else restores the
 * member's current value. */
void EdEnumControl::cbChanged(eduimenu_s *, eduiitem_s *item, u32) {
    EdEnumControl *control = static_cast<EdEnumControl *>(item->data_ptr);
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    Item *entry;
    for (entry = control->items; entry->name != NULL; entry++) {
        if (NuStrICmp(entry->name, property->property_text) == 0) {
            eduiItemPropSetText(property, entry->name);
            return;
        }
    }
    i32 value = NuAToI(property->property_text);
    for (entry = control->items; entry->name != NULL; entry++) {
        if (entry->value == value) {
            eduiItemPropSetText(property, entry->name);
            return;
        }
    }
    control->reference->GetMemberData(control->object, EdType_Int, &value, 0);
    char text[128];
    NuSPrintf(text, "%d", value);
    eduiItemPropSetText(property, text);
}

void EdEnumControl::Refresh() {
    i32 value;
    reference->GetMemberData(object, EdType_Int, &value, 0);
    eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(item), GetEnumString(value));
}

void EdBitControl::cbChanged(eduimenu_s *, eduiitem_s *item, u32) {
    EdBitControl *control = static_cast<EdBitControl *>(item->data_ptr);
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    Item *entry;
    for (entry = control->items; entry->name != NULL; entry++) {
        if (NuStrICmp(entry->name, property->property_text) == 0) {
            eduiItemPropSetText(property, entry->name);
            return;
        }
    }
    i32 value = NuAToI(property->property_text);
    for (entry = control->items; entry->name != NULL; entry++) {
        if (entry->value == value) {
            eduiItemPropSetText(property, entry->name);
            return;
        }
    }
    control->reference->GetMemberData(control->object, EdType_Int, &value, 0);
    char text[128];
    NuSPrintf(text, "%d", value);
    eduiItemPropSetText(property, text);
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

/* Property control for EdType_String members: the text is edited in place
 * and the button opens a text picker on it. */
class EdStringControl : public EdControl {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdStringControl));
    }
    EdStringControl();
    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    void GetVal(char *value, i32 capacity);
    void SetVal(char const *value);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbPress(eduimenu_s *menu, eduiitem_s *item, u32);
};
STATIC_ASSERT(sizeof(EdStringControl) == 0x10);

/* Property control for EdType_Colour3 members: "r g b" text, the property
 * button shows the colour and opens a colour picker. */
class EdColourControl : public EdControl {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdColourControl));
    }
    EdColourControl();
    virtual ~EdColourControl();
    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbColourSelected(eduimenu_s *menu, eduiitem_s *item, u32 flags);
};
STATIC_ASSERT(sizeof(EdColourControl) == 0x10);

EdStringControl::EdStringControl() {
}

void EdStringControl::GetVal(char *value, i32 capacity) {
    if (reference->type_id == EdType_String)
        reference->GetMemberData(object, EdType_String, value, capacity);
}

void EdStringControl::SetVal(char const *value) {
    if (reference->type_id == EdType_String) {
        char copy[128];
        NuStrNCpy(copy, value, reference->size);
        reference->SetMemberData(object, EdType_String, copy, 0, NULL);
    }
}

void EdStringControl::Refresh() {
    char value[128];
    GetVal(value, sizeof(value));
    eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(item), value);
}

void EdStringControl::cbChanged(eduimenu_s *, eduiitem_s *item, u32) {
    static_cast<EdStringControl *>(item->data_ptr)->SetVal(reinterpret_cast<edui_prop_s *>(item)->property_text);
}

void EdStringControl::cbPress(eduimenu_s *menu, eduiitem_s *item, u32) {
    eduiAddPropTextPickEnt(menu, item);
}

EdColourControl::EdColourControl() {
}

EdColourControl::~EdColourControl() {
}

void EdColourControl::Refresh() {
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    NUVEC colour;
    reference->GetMemberData(object, EdType_Colour3, &colour, 0);
    char value[128];
    NuSPrintf(value, "%.2f %.2f %.2f", colour.x, colour.y, colour.z);
    eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(item), value);
    i32 packed = (i32)(colour.z * 255.0);
    packed |= 0xffffff00;
    packed <<= 8;
    packed |= (i32)(colour.y * 255.0) & 0xff;
    packed <<= 8;
    packed |= (i32)(colour.x * 255.0) & 0xff;
    property->button_colour = packed;
}

void EdColourControl::cbChanged(eduimenu_s *, eduiitem_s *item, u32) {
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    EdColourControl *control = static_cast<EdColourControl *>(item->data_ptr);
    NUVEC colour;
    control->reference->GetMemberData(control->object, EdType_Colour3, &colour, 0);
    sscanf(property->property_text, "%f %f %f", &colour.x, &colour.y, &colour.z);
    control->reference->SetMemberData(control->object, EdType_Colour3, &colour, 0, NULL);
    char value[128];
    NuSPrintf(value, "%.2f %.2f %.2f", colour.x, colour.y, colour.z);
    eduiItemPropSetText(property, value);
    i32 packed = (i32)(colour.z * 255.0);
    packed |= 0xffffff00;
    packed <<= 8;
    packed |= (i32)(colour.y * 255.0) & 0xff;
    packed <<= 8;
    packed |= (i32)(colour.x * 255.0) & 0xff;
    property->button_colour = packed;
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

/* Average position of the objects that have one; returns how many had. */
i32 ClassObjectList::GetAveragePosition(VuVec &average) {
    average.x = 0.0f;
    average.y = 0.0f;
    average.z = 0.0f;
    average.w = 1.0f;
    i32 count = 0;
    for (ClassObjectListEntry *entry = first; entry != NULL; entry = entry->next) {
        __declspec(align(16)) VuVec position;
        if (reinterpret_cast<ClassObject *>(&entry->ed_class)->GetAttributeData(8, EdType_VuVec, &position, 0)) {
            average.x += position.x;
            average.y += position.y;
            average.z += position.z;
            count++;
        }
    }
    if (count > 0) {
        f32 scale = 1.0f / count;
        average.x *= scale;
        average.y *= scale;
        average.z *= scale;
    }
    return count;
}

void ClassObjectList::Remove(ClassObjectListEntry *entry) {
    if (entry->next != NULL)
        entry->next->previous = entry->previous;
    else
        last = entry->previous;
    if (entry->previous != NULL)
        entry->previous->next = entry->next;
    else
        first = entry->next;
    entry->next = NULL;
    entry->previous = NULL;
    count--;
}

/* mode 1 toggles the object (and its picked member) in the selection, mode 2
 * replaces every entry for the same object, anything else replaces the whole
 * selection. Returns whether the object ends up selected. */
i32 ClassEditor::SelectObject(ClassObject &object, i32 mode) {
    i32 selected = 0;
    ClassObjectListEntry *entry;
    if (mode == 1) {
        if (object.object == NULL)
            goto done;
        for (entry = selected_objects.first; entry != NULL; entry = entry->next) {
            if (entry->object == object.object && entry->reference == object.reference) {
                selected_objects.Remove(entry);
                theMemoryManager.FreePool(entry, sizeof(ClassObjectListEntry));
                goto done;
            }
        }
        entry = (ClassObjectListEntry *)theMemoryManager.AllocPool(sizeof(ClassObjectListEntry), 1);
        if (entry != NULL) {
            memset(entry, 0, sizeof(ClassObjectListEntry));
            entry->ed_class = object.ed_class;
            entry->object = object.object;
            entry->reference = object.reference;
            entry->next = NULL;
            entry->previous = selected_objects.last;
            if (selected_objects.last != NULL)
                selected_objects.last->next = entry;
            selected_objects.last = entry;
            if (selected_objects.first == NULL)
                selected_objects.first = entry;
            selected_objects.count++;
        }
    } else {
        if (mode == 2) {
            ClassObjectListEntry *next;
            for (entry = selected_objects.first; entry != NULL; entry = next) {
                next = entry->next;
                if (entry->object == object.object) {
                    selected_objects.Remove(entry);
                    theMemoryManager.FreePool(entry, sizeof(ClassObjectListEntry));
                }
            }
        } else {
            while ((entry = selected_objects.first) != NULL) {
                selected_objects.Remove(entry);
                theMemoryManager.FreePool(entry, sizeof(ClassObjectListEntry));
            }
        }
        if (object.object != NULL) {
            entry = (ClassObjectListEntry *)theMemoryManager.AllocPool(sizeof(ClassObjectListEntry), 1);
            if (entry != NULL) {
                memset(entry, 0, sizeof(ClassObjectListEntry));
                entry->ed_class = object.ed_class;
                entry->object = object.object;
                entry->reference = object.reference;
                entry->next = NULL;
                entry->previous = selected_objects.last;
                if (selected_objects.last != NULL)
                    selected_objects.last->next = entry;
                selected_objects.last = entry;
                if (selected_objects.first == NULL)
                    selected_objects.first = entry;
                selected_objects.count++;
            }
        }
    }
    selected = 1;
done:
    thePropertyTool.SelectAttr(manipulator->selected_attribute);
    return selected;
}

i32 ClassEditor::IsSelectedObject(ClassObject &object) {
    ClassObject selected = object;
    for (ClassObjectListEntry *entry = selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->object == selected.object && entry->reference == selected.reference)
            return 1;
    }
    return 0;
}

/* A class is editable when its filter bit is set; an object when it belongs
 * to no scene or to an editable (or inactive) scene. */
i32 ClassEditor::Editable(void *object, EdClass *object_class, i32 index) {
    i16 scene = 0;
    if (object != NULL) {
        EdMember member;
        if (object_class->FindMember(&member, object, 0x100, 1))
            member.reference->GetAttributeData(member.object, 0x100, EdType_Short, &scene, 0);
        return theLevelEditor.IsEditable(scene);
    }
    if (class_filter & (1 << index))
        return 1;
    return 0;
}

/* Draws the tools, the selection (through the manipulator when there is one)
 * and every editable object of the classes that render or show spheres. */
void ClassEditor::Render() {
    ClassObject selected;
    thePropertyTool.Render();
    if (active_tool != NULL)
        active_tool->Render();
    if (current_object.object != NULL && !IsSelectedObject(current_object))
        DrawObjectSphere(current_object, field_3c);
    if (selected_objects.count > 0) {
        ClassObjectListEntry *entry;
        if (manipulator != NULL) {
            manipulator->Render(selected_objects);
        } else {
            for (entry = selected_objects.first; entry != NULL; entry = entry->next)
                DrawObjectSphere(*(ClassObject *)&entry->ed_class, 0xff800000);
        }
        for (entry = selected_objects.first; entry != NULL; entry = entry->next)
            theRegistry.ClassIFaceRender(entry->ed_class, entry->object, 1);
    }
    for (i32 index = 0; index < theRegistry.class_count; index++) {
        selected.ed_class = theRegistry.GetClass(index);
        if (!Editable(NULL, selected.ed_class, index))
            continue;
        if (selected.ed_class->object_interface == NULL)
            continue;
        if (!(selected.ed_class->flags & 0x8000080))
            continue;
        for (selected.object = selected.ed_class->object_interface->GetNextObject(NULL); selected.object != NULL;
             selected.object = selected.ed_class->object_interface->GetNextObject(selected.object)) {
            if (!Editable(selected.object, selected.ed_class, -1))
                continue;
            if ((selected.ed_class->flags & 0x8000000) && !IsSelectedObject(selected))
                theRegistry.ClassIFaceRender(selected.ed_class, selected.object, 0);
            if ((selected.ed_class->flags & 0x80) && IsSelectedObject(selected) &&
                selected.object != current_object.object)
                DrawObjectSphere(selected, 0xff000080);
        }
    }
    if (menu != NULL)
        eduiMenuRender(menu);
}

/* Whether no named object of any named class uses this name. */
i32 ClassEditor::IsUniqueName(char *name) {
    EdMember member;
    char object_name[128];
    for (i32 i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = &theRegistry.classes[i];
        if (object_class->object_interface == NULL || !(object_class->flags & 2))
            continue;
        for (void *object = object_class->object_interface->GetNextObject(NULL); object != NULL;
             object = object_class->object_interface->GetNextObject(object)) {
            if (object_class->FindMember(&member, object, 2, 1)) {
                member.reference->GetAttributeData(member.object, 2, EdType_String, object_name, sizeof(object_name));
                if (NuStrICmp(name, object_name) == 0)
                    return 0;
            }
        }
    }
    return 1;
}

/* Writes name with its trailing number (or "00") bumped until the result is
 * unique. */
void ClassEditor::MakeUniqueName(char const *name, char *destination, i32 size) {
    i32 digits = 0;
    i32 number = 0;
    i32 length = NuStrLen((char *)name);
    char const *cursor = name + length - 1;
    while (*cursor >= '0' && *cursor <= '9') {
        cursor--;
        digits++;
    }
    if (digits > 0) {
        length -= digits;
        number = NuAToI((char *)name + length);
    } else {
        digits = 2;
    }
    if (length + digits >= size)
        length = size - digits - 1;
    NuStrNCpy(destination, name, length + 1);
    char format[16];
    NuSPrintf(format, "%%.%dd", digits);
    do {
        number++;
        NuSPrintf(destination + length, format, number);
    } while (!IsUniqueName(destination));
}

class SplineObject;

/* Bezier subdivision tolerances and the step below which splines use
 * straight segments. */
f32 BezierLineTolerance = 0.01f;
i32 BezierLineMaxDepth = 2;
f32 SplineMinStep = 0.1f;

/* One spline control point: its position and the two tangent handles. */
struct SplineKnot {
    SplineKnot *next;
    SplineKnot *previous;
    __declspec(align(16)) VuVec position;
    VuVec in_tangent;
    VuVec out_tangent;
    SplineObject *spline;
    i16 led_file;

    void Smooth();
};
STATIC_ASSERT(offsetof(SplineKnot, position) == 0x10);
STATIC_ASSERT(offsetof(SplineKnot, spline) == 0x40);
STATIC_ASSERT(sizeof(SplineKnot) == 0x50);

struct SplineKnotList {
    SplineKnot *first;
    SplineKnot *last;
    i32 count;

    ~SplineKnotList();
    void InsertAfter(SplineKnot *knot, SplineKnot *after);
};

/* A block of generated spline points. */
class SplinePointBlock : public CSListLink<SplinePointBlock> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(SplinePointBlock));
    }
    SplinePointBlock();
    virtual ~SplinePointBlock() {
        if (points != NULL)
            theMemoryManager.FreePool(points, capacity * sizeof(VuVec));
    }

    i32 capacity;
    i32 point_count;
    VuVec *points;
};
STATIC_ASSERT(offsetof(SplinePointBlock, capacity) == 0xc);
STATIC_ASSERT(offsetof(SplinePointBlock, points) == 0x14);
STATIC_ASSERT(sizeof(SplinePointBlock) == 0x18);

struct SplinePointList {
    SplinePointBlock *first;
    SplinePointBlock *last;
    i32 block_count;

    ~SplinePointList();
    void AddPoint(VuVec &point);
    void Clear();
    void Draw();
};

/* A named editor spline; the knot list is followed by its generated points. */
class SplineObject : public CSListLink<SplineObject> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(SplineObject));
    }
    SplineObject() {
        knots.first = NULL;
        knots.last = NULL;
        knots.count = 0;
        points.first = NULL;
        points.last = NULL;
        points.block_count = 0;
        led_file = Placeable::CurrentLedFile;
    }
    virtual ~SplineObject();

    char name[32];
    SplineKnotList knots;
    SplinePointList points;
    i16 led_file;
    u16 reserved_46;
    f32 step;
    f32 height;
    i32 drop;
    i32 closed;

    /* Retail never casts the terrain ray here; a dropping spline is just
     * lowered by 10. */
    void DropPoint(VuVec &point) {
        if (drop != 0) {
            __declspec(align(16)) VuVec start;
            start.x = point.x;
            start.y = point.y;
            start.z = point.z;
            point.y = start.y - 10.0;
        }
    }
    void Draw(i32 selected, i32 straight, i32 show_points, f32);
    void GenBezierPoints();
    void GenLinearPoints();
    void GenPoints() {
        if (step < SplineMinStep)
            GenLinearPoints();
        else
            GenBezierPoints();
    }
    void ReverseKnots();
};
STATIC_ASSERT(offsetof(SplineObject, knots) == 0x2c);
STATIC_ASSERT(offsetof(SplineObject, points) == 0x38);
STATIC_ASSERT(offsetof(SplineObject, led_file) == 0x44);
STATIC_ASSERT(offsetof(SplineObject, drop) == 0x50);
STATIC_ASSERT(sizeof(SplineObject) == 0x58);

/* The spline object class (Android SplineHelper), theSplineHelper. */
class SplineHelper : public EdClassInterface {
  public:
    SplineHelper() : first_object(NULL), last_object(NULL), object_count(0), auto_generate_points(1) {
    }
    virtual void ClearLevel(i32 level);
    virtual void Flush() {
        last_object = NULL;
        first_object = NULL;
        object_count = 0;
    }
    virtual i32 GetNumObjects() {
        return object_count;
    }
    virtual void *GetNextObject(void *object);
    virtual void *CreateObject(void *data, i32 guid, i32 flags);
    virtual void DestroyObject(void *object, i32 flags);
    virtual void Process(void *object, EdInputContext &input);
    virtual void Render(void *object, i32 selected);
    virtual void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
    }
    virtual void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    virtual void SerialiseObject(EdStream &stream, void *object);
    virtual void AddMenuItems(eduimenu_s *menu);
    using EdClassInterface::GetNextObject;

    SplineObject *first_object;
    SplineObject *last_object;
    i32 object_count;
    i32 auto_generate_points;

    static void cbEdSplineAutoGenPoints(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    static void cbEdSplineReGenPoints(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    static void cbEdSplineReverseSpline(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    static void cbEdSplineSmoothKnot(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    static void cbEdSplineSmoothSpline(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
};
STATIC_ASSERT(offsetof(SplineHelper, first_object) == 0x8);
STATIC_ASSERT(offsetof(SplineHelper, auto_generate_points) == 0x14);
STATIC_ASSERT(sizeof(SplineHelper) == 0x18);
extern SplineHelper theSplineHelper;

/* Picking a knot selects its spline unless the spline is already selected. */
void KnotHelper::Process(void *object, EdInputContext &) {
    SplineKnot *knot = (SplineKnot *)object;
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->object == knot->spline)
            return;
    }
    ClassObject spline_object;
    spline_object.ed_class = theSplineHelper.object_class;
    spline_object.object = knot->spline;
    theClassEditor.SelectObject(spline_object, 1);
}

KnotHelper theKnotHelper;

SplineKnotList::~SplineKnotList() {
    SplineKnot *knot;
    while ((knot = first) != NULL) {
        if (knot->next != NULL)
            knot->next->previous = knot->previous;
        else
            last = knot->previous;
        if (knot->previous != NULL)
            knot->previous->next = knot->next;
        else
            first = knot->next;
        knot->next = NULL;
        knot->previous = NULL;
        count--;
        theMemoryManager.FreePool(knot, sizeof(SplineKnot));
    }
}

SplinePointList::~SplinePointList() {
    SplinePointBlock *block;
    while ((block = first) != NULL) {
        if (block->next != NULL)
            block->next->previous = block->previous;
        else
            last = block->previous;
        if (block->previous != NULL)
            block->previous->next = block->next;
        else
            first = block->next;
        block->next = NULL;
        block->previous = NULL;
        block_count--;
        delete block;
    }
}

SplineObject::~SplineObject() {
}

/* Sets both tangents to a tenth of the neighbour-to-neighbour vector; a
 * spline that drops onto the terrain lowers them by 10. */
void SplineKnot::Smooth() {
    __declspec(align(16)) VuVec before;
    __declspec(align(16)) VuVec after;
    __declspec(align(16)) VuVec centre;
    __declspec(align(16)) VuVec delta;
    before.x = 0.0f;
    before.y = 0.0f;
    before.z = 0.0f;
    after.x = 0.0f;
    after.y = 0.0f;
    after.z = 0.0f;
    centre.x = position.x;
    centre.y = position.y;
    centre.z = position.z;
    if (previous != NULL && next != NULL) {
        before.x = next->position.x;
        before.y = next->position.y;
        before.z = next->position.z;
        after.x = previous->position.x;
        after.y = previous->position.y;
        after.z = previous->position.z;
    } else if (next != NULL) {
        before.x = next->position.x;
        before.y = next->position.y;
        before.z = next->position.z;
        after.x = position.x;
        after.y = position.y;
        after.z = position.z;
    } else if (previous != NULL) {
        before.x = position.x;
        before.y = position.y;
        before.z = position.z;
        after.x = previous->position.x;
        after.y = previous->position.y;
        after.z = previous->position.z;
    }
    delta.x = before.x - after.x;
    delta.y = before.y - after.y;
    delta.z = before.z - after.z;
    before.x = delta.x * 0.1;
    before.y = delta.y * 0.1;
    before.z = delta.z * 0.1;
    after.x = before.x + centre.x;
    after.y = before.y + centre.y;
    after.z = before.z + centre.z;
    before.x = centre.x - before.x;
    before.y = centre.y - before.y;
    before.z = centre.z - before.z;
    if (spline != NULL) {
        if (spline->drop)
            after.y = after.y - 10.0;
        if (spline->drop)
            before.y = before.y - 10.0;
    }
    out_tangent.x = after.x;
    out_tangent.y = after.y;
    out_tangent.z = after.z;
    out_tangent.w = after.w;
    in_tangent.x = before.x;
    in_tangent.y = before.y;
    in_tangent.z = before.z;
    in_tangent.w = before.w;
}

/* Adds a knot after the selected knot (or at the end) of the selected
 * spline, creating a new spline when none is selected. */
void *KnotHelper::CreateObject(void *, i32, i32) {
    SplineObject *spline = NULL;
    SplineKnot *selected_knot = NULL;
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (spline == NULL && entry->ed_class == theSplineHelper.object_class)
            spline = (SplineObject *)entry->object;
        if (selected_knot == NULL && entry->ed_class == theKnotHelper.object_class)
            selected_knot = (SplineKnot *)entry->object;
        if (selected_knot != NULL && spline != NULL)
            break;
    }
    if (spline == NULL) {
        spline = new (theMemoryManager.AllocPool(sizeof(SplineObject), 1)) SplineObject;
        if (spline == NULL)
            return NULL;
        theClassEditor.MakeUniqueName("NewSpline", spline->name, sizeof(spline->name));
        spline->next = NULL;
        spline->previous = theSplineHelper.last_object;
        if (theSplineHelper.last_object != NULL)
            theSplineHelper.last_object->next = spline;
        theSplineHelper.last_object = spline;
        if (theSplineHelper.first_object == NULL)
            theSplineHelper.first_object = spline;
        theSplineHelper.object_count++;
    }
    SplineKnot *knot = (SplineKnot *)theMemoryManager.AllocPool(sizeof(SplineKnot), 1);
    if (knot == NULL)
        return NULL;
    knot->next = NULL;
    knot->previous = NULL;
    knot->led_file = Placeable::CurrentLedFile;
    knot->spline = spline;
    spline->knots.InsertAfter(knot, selected_knot);
    knot->Smooth();
    return knot;
}

/* Retail keeps this out of line: KnotHelper::CreateObject calls it (0x6cf159). */
__declspec(noinline) void SplineKnotList::InsertAfter(SplineKnot *knot, SplineKnot *after) {
    if (after != NULL) {
        knot->previous = after;
        knot->next = after->next;
        if (after->next != NULL)
            after->next->previous = knot;
        else
            last = knot;
        after->next = knot;
    } else {
        knot->next = NULL;
        knot->previous = last;
        if (last != NULL)
            last->next = knot;
        last = knot;
        if (first == NULL)
            first = knot;
    }
    count++;
}

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
    SplineKnot *next = NULL;
    if (current == NULL) {
        for (SplineObject *spline = theSplineHelper.first_object; spline != NULL; spline = spline->next) {
            if (spline->knots.count != 0)
                return spline->knots.first;
        }
    } else {
        next = static_cast<SplineKnot *>(current)->next;
        if (next == NULL) {
            for (SplineObject *spline = static_cast<SplineKnot *>(current)->spline->next; spline != NULL;
                 spline = spline->next) {
                if (spline->knots.count != 0)
                    return spline->knots.first;
            }
            return NULL;
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

/* Absolute value by clearing the sign bit. */
static inline f32 VuFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

/* Signed difference of two 16-bit angles, in -0x8000..0x7fff. */
static inline i32 VuAngleDifference(i32 from, i32 to) {
    i32 difference = (from - to) & 0xffff;
    if (difference >= 0x8000)
        difference -= 0x10000;
    return difference;
}

static inline void VuVecSet(VuVec &v, f32 x, f32 y, f32 z, f32 w) {
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
}

/* The origin as a point: 0 0 0 1. */
static inline void VuVecSetOrigin(VuVec &v) {
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    v.w = 1.0f;
}

static inline VuVec operator+(const VuVec &a, const VuVec &b) {
    VuVec sum;
    sum.x = a.x + b.x;
    sum.y = a.y + b.y;
    sum.z = a.z + b.z;
    return sum;
}

static inline VuVec operator-(const VuVec &a, const VuVec &b) {
    VuVec difference;
    difference.x = a.x - b.x;
    difference.y = a.y - b.y;
    difference.z = a.z - b.z;
    return difference;
}

static inline VuVec operator*(const VuVec &a, f32 scale) {
    VuVec product;
    product.x = a.x * scale;
    product.y = a.y * scale;
    product.z = a.z * scale;
    return product;
}

/* Unit-length copy of a direction, as a point (w = 1). */
static inline void VuVecNormalise(VuVec &destination, VuVec &source) {
    NuVecNorm(&destination.xyz, &source.xyz);
    destination.w = 1.0f;
}

static inline f32 VuVecDot(const VuVec &a, const VuVec &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* VuVec assignment copies the four floats one by one. */
static inline f32 VuVecLengthSq(const VuVec &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline f32 VuVecLength(const VuVec &v) {
    f32 length_squared = VuVecLengthSq(v);
    return NuFsqrt(length_squared);
}

static inline void VuVecCopy(VuVec &destination, const VuVec &source) {
    destination.x = source.x;
    destination.y = source.y;
    destination.z = source.z;
    destination.w = source.w;
}

static i32 bezierline_depth = 0;

/* Length of a cubic Bezier segment: the chord once the de Casteljau midpoint
 * lies within BezierLineTolerance of it, otherwise the sum of both halves. */
f32 BezierLineLength(VuVec &start, VuVec &first_control, VuVec &end, VuVec &second_control) {
    __declspec(align(16)) VuVec points[6];
    VuVecCopy(points[0], (first_control + second_control) * 0.5);
    VuVecCopy(points[1], (start + first_control) * 0.5);
    VuVecCopy(points[2], (end + second_control) * 0.5);
    VuVecCopy(points[3], (points[1] + points[0]) * 0.5);
    VuVecCopy(points[4], (points[0] + points[2]) * 0.5);
    VuVecCopy(points[5], (points[3] + points[4]) * 0.5);
    VuVec error = (start + end) * 0.5 - points[5];
    f32 length;
    if (!(NuFsqrt(error.x * error.x + error.y * error.y + error.z * error.z) < BezierLineTolerance) &&
        bezierline_depth < BezierLineMaxDepth) {
        bezierline_depth++;
        length = BezierLineLength(start, points[1], points[5], points[3]) +
                 BezierLineLength(points[5], points[4], end, points[2]);
        bezierline_depth--;
        return length;
    }
    VuVec chord = start - end;
    length = NuFsqrt(chord.x * chord.x + chord.y * chord.y + chord.z * chord.z);
    return length;
}

/* Binary search along the segment for the point distance along it. */
i32 BezierLinePos(VuVec &result, VuVec &start, VuVec &first_control, VuVec &end, VuVec &second_control, f32 distance) {
    f32 t = 0.5f;
    f32 step = 0.25f;
    __declspec(align(16)) VuVec points[6];
    for (i32 count = 0; count < 256; count++) {
        f32 complement = 1.0f - t;
        VuVecCopy(points[0], first_control * t + second_control * complement);
        VuVecCopy(points[1], start * t + first_control * complement);
        VuVecCopy(points[2], second_control * t + end * complement);
        VuVecCopy(points[3], points[1] * t + points[0] * complement);
        VuVecCopy(points[4], points[0] * t + points[2] * complement);
        VuVecCopy(points[5], points[3] * t + points[4] * complement);
        points[5].w = 1.0f;
        f32 length = BezierLineLength(start, points[1], points[5], points[3]);
        f32 difference = distance - length;
        if (difference < 0.0f)
            difference = -difference;
        if (!(difference > BezierLineTolerance))
            break;
        if (distance < length)
            t += step;
        else
            t -= step;
        step *= 0.5;
    }
    result.x = points[5].x;
    result.y = points[5].y;
    result.z = points[5].z;
    result.w = points[5].w;
    return 1;
}

f32 BezierDrawTolerance = 0.001f;
static i32 bezierdraw_depth = 0;

/* Draws a Bezier segment, subdividing until a piece is short relative to
 * its distance from the camera. */
void DrawBezierLine(VuVec &start, VuVec &start_control, VuVec &end, VuVec &end_control, NUMTL *material, i32 colour) {
    VuVec camera;
    camera.x = global_camera.mtx.m30;
    camera.y = global_camera.mtx.m31;
    camera.z = global_camera.mtx.m32;
    VuVec offset = start - camera;
    f32 distance = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
    offset = end - camera;
    f32 end_distance = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
    if (distance > end_distance)
        distance = end_distance;
    if (5.5f > distance)
        distance = 5.5f;
    VuVec chord = end - start;
    if (BezierDrawTolerance * distance < chord.x * chord.x + chord.y * chord.y + chord.z * chord.z &&
        bezierdraw_depth < 16) {
        bezierdraw_depth++;
        __declspec(align(16)) VuVec points[6];
        VuVecCopy(points[0], (start_control + end_control) * 0.5);
        VuVecCopy(points[1], (start + start_control) * 0.5);
        VuVecCopy(points[2], (end + end_control) * 0.5);
        VuVecCopy(points[3], (points[1] + points[0]) * 0.5);
        VuVecCopy(points[4], (points[0] + points[2]) * 0.5);
        VuVecCopy(points[5], (points[3] + points[4]) * 0.5);
        DrawBezierLine(start, points[1], points[5], points[3], material, colour);
        DrawBezierLine(points[5], points[4], end, points[2], material, colour);
        bezierdraw_depth--;
        return;
    }
    EdDrawLineSegment(&start.xyz, &end.xyz, colour);
}

SplinePointBlock::SplinePointBlock() {
    capacity = 16;
    points = (VuVec *)theMemoryManager.AllocPool(capacity * sizeof(VuVec), 1);
}

/* Appends to the first block with room, or to a new block. */
void SplinePointList::AddPoint(VuVec &point) {
    SplinePointBlock *block;
    VuVec *slot;
    for (block = first; block != NULL; block = block->next) {
        if (block->point_count < block->capacity) {
            slot = &block->points[block->point_count++];
            slot->x = point.x;
            slot->y = point.y;
            slot->z = point.z;
            slot->w = point.w;
            return;
        }
    }
    block = new (theMemoryManager.AllocPool(sizeof(SplinePointBlock), 1)) SplinePointBlock;
    if (block != NULL) {
        slot = &block->points[block->point_count++];
        slot->x = point.x;
        slot->y = point.y;
        slot->z = point.z;
        slot->w = point.w;
        block->next = NULL;
        block->previous = last;
        if (last != NULL)
            last->next = block;
        last = block;
        if (first == NULL)
            first = block;
        block_count++;
    }
}

/* Frees the default-sized blocks and empties the others. */
void SplinePointList::Clear() {
    SplinePointBlock *next;
    for (SplinePointBlock *block = first; block != NULL; block = next) {
        next = block->next;
        if (block->capacity == 16) {
            if (block->next != NULL)
                block->next->previous = block->previous;
            else
                last = block->previous;
            if (block->previous != NULL)
                block->previous->next = block->next;
            else
                first = block->next;
            block->next = NULL;
            block->previous = NULL;
            block_count--;
            delete block;
        } else {
            block->point_count = 0;
        }
    }
}

void SplinePointList::Draw() {
    for (SplinePointBlock *block = first; block != NULL; block = block->next) {
        for (i32 index = 0; index < block->point_count; index++)
            EdDrawLineCross(&block->points[index].xyz, 0.5f, -1);
    }
}

void SplineObject::ReverseKnots() {
    SplineKnotList reversed = knots;
    knots.first = NULL;
    knots.last = NULL;
    knots.count = 0;
    SplineKnot *knot;
    while ((knot = reversed.last) != NULL) {
        if (knot->next != NULL)
            knot->next->previous = knot->previous;
        else
            reversed.last = knot->previous;
        if (knot->previous != NULL)
            knot->previous->next = knot->next;
        else
            reversed.first = knot->next;
        knot->next = NULL;
        knot->previous = NULL;
        reversed.count--;
        knot->previous = knots.last;
        if (knots.last != NULL)
            knots.last->next = knot;
        knots.last = knot;
        if (knots.first == NULL)
            knots.first = knot;
        knots.count++;
    }
}

/* One point per knot (and the first again for a closed spline). */
void SplineObject::GenLinearPoints() {
    points.Clear();
    __declspec(align(16)) VuVec point;
    for (SplineKnot *knot = knots.first; knot != NULL; knot = knot->next) {
        point.x = knot->position.x;
        point.y = knot->position.y;
        point.z = knot->position.z;
        point.w = knot->position.w;
        DropPoint(point);
        point.y += height;
        points.AddPoint(point);
    }
    if (closed != 0 && knots.first != NULL) {
        point.x = knots.first->position.x;
        point.y = knots.first->position.y;
        point.z = knots.first->position.z;
        point.w = knots.first->position.w;
        DropPoint(point);
        point.y += height;
        points.AddPoint(point);
    }
}

/* Points every step along the Bezier segments between the knots. */
void SplineObject::GenBezierPoints() {
    i32 closing = 0;
    points.Clear();
    f32 distance = 0.0f;
    SplineKnot *start = knots.first;
    if (start == NULL)
        return;
    SplineKnot *end = start->next;
    while (end != NULL) {
        f32 length = BezierLineLength(start->position, start->out_tangent, end->position, end->in_tangent);
        for (; distance < length; distance += step) {
            __declspec(align(16)) VuVec point;
            BezierLinePos(point, start->position, start->out_tangent, end->position, end->in_tangent, distance);
            DropPoint(point);
            point.y += height;
            points.AddPoint(point);
        }
        distance -= length;
        start = end;
        end = end->next;
        if (closing)
            return;
        if (end == NULL && closed != 0) {
            end = knots.first;
            closing = 1;
        }
        if (start == NULL)
            return;
    }
}

void SplineObject::Draw(i32 selected, i32 straight, i32 show_points, f32) {
    i32 colour = selected ? 0x80008080 : 0x80808080;
    EdDrawBegin(0);
    for (SplineKnot *knot = knots.first; knot != NULL && knot->next != NULL; knot = knot->next) {
        SplineKnot *next = knot->next;
        if (straight != 0 || step < SplineMinStep)
            EdDrawLineSegment(&knot->position.xyz, &next->position.xyz, colour);
        else
            DrawBezierLine(knot->position, knot->out_tangent, next->position, next->in_tangent, edLevel3dMtl, colour);
    }
    if (show_points != 0 && step > SplineMinStep)
        points.Draw();
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
}

void SplineHelper::ClearLevel(i32 level) {
    if (level == -1)
        return;
    SplineObject *next;
    for (SplineObject *spline = (SplineObject *)GetNextObject(NULL); spline != NULL; spline = next) {
        next = (SplineObject *)GetNextObject(spline);
        if (spline->led_file == level)
            DestroyObject(spline, 0);
    }
}

void *SplineHelper::GetNextObject(void *object) {
    if (object == NULL)
        return first_object;
    return static_cast<SplineObject *>(object)->next;
}

void *SplineHelper::CreateObject(void *, i32, i32) {
    void *memory = theMemoryManager.AllocPool(sizeof(SplineObject), 1);
    if (memory == NULL)
        return NULL;
    SplineObject *spline = new (memory) SplineObject;
    spline->next = NULL;
    spline->previous = last_object;
    if (last_object != NULL)
        last_object->next = spline;
    last_object = spline;
    if (first_object == NULL)
        first_object = spline;
    object_count++;
    return spline;
}

/* Only a spline whose knots are all gone is destroyed. */
void SplineHelper::DestroyObject(void *object, i32) {
    theLevelEditor.destroying_objects = 1;
    for (SplineObject *spline = first_object; spline != NULL; spline = spline->next) {
        if (spline == object && spline->knots.count == 0) {
            if (spline->next != NULL)
                spline->next->previous = spline->previous;
            else
                last_object = spline->previous;
            if (spline->previous != NULL)
                spline->previous->next = spline->next;
            else
                first_object = spline->next;
            spline->next = NULL;
            spline->previous = NULL;
            object_count--;
            delete spline;
            return;
        }
    }
}

/* Regenerates a spline's points once its knots stop being dragged. */
void SplineHelper::Process(void *object, EdInputContext &input) {
    SplineObject *spline = static_cast<SplineObject *>(object);
    if (input.GetHold(3) != 0.0f)
        return;
    if (spline->points.block_count != 0)
        return;
    spline->GenPoints();
}

void SplineHelper::Render(void *object, i32 selected) {
    static_cast<SplineObject *>(object)->Draw(selected, selected == 0, selected, 0.0f);
}

void SplineHelper::PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
    for (SplineObject *spline = first_object; spline != NULL; spline = spline->next)
        spline->GenPoints();
}

/* Writes the knot count and knots; reading recreates the knots. */
void SplineHelper::SerialiseObject(EdStream &stream, void *object) {
    SplineObject *spline = static_cast<SplineObject *>(object);
    i32 knot_count;
    if (stream.mode == 2) {
        knot_count = spline->knots.count;
        stream.SerialiseBuffer(&knot_count, sizeof(knot_count), 1);
        for (SplineKnot *knot = spline->knots.first; knot != NULL; knot = knot->next)
            theKnotHelper.object_class->SerialiseObject(stream, knot);
    }
    if (stream.mode == 1) {
        stream.SerialiseBuffer(&knot_count, sizeof(knot_count), 1);
        for (i32 index = 0; index < knot_count; index++) {
            SplineKnot *knot = (SplineKnot *)theMemoryManager.AllocPool(sizeof(SplineKnot), 1);
            if (knot != NULL) {
                knot->next = NULL;
                knot->previous = NULL;
                knot->led_file = Placeable::CurrentLedFile;
            }
            theKnotHelper.object_class->SerialiseObject(stream, knot);
            if (knot != NULL) {
                knot->spline = spline;
                knot->next = NULL;
                knot->previous = spline->knots.last;
                if (spline->knots.last != NULL)
                    spline->knots.last->next = knot;
                spline->knots.last = knot;
                if (spline->knots.first == NULL)
                    spline->knots.first = knot;
                spline->knots.count++;
            }
        }
    }
}

void SplineHelper::AddMenuItems(eduimenu_s *menu) {
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdSplineSmoothKnot, "Smooth Knot"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdSplineSmoothSpline, "Smooth Spline"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdSplineReverseSpline, "Reverse Spline"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdSplineReGenPoints, "Re-Gen Points"));
    eduiMenuAddItem(menu, eduiItemCheckCreate(0, &EdLevelAttr, auto_generate_points, 0, cbEdSplineAutoGenPoints,
                                              "Auto-Gen Points"));
}

void SplineHelper::cbEdSplineAutoGenPoints(eduimenu_s *, eduiitem_s *item, u32) {
    theSplineHelper.auto_generate_points ^= 1;
    item->highlighted = theSplineHelper.auto_generate_points & 1;
}

void SplineHelper::cbEdSplineReGenPoints(eduimenu_s *, eduiitem_s *, u32) {
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class == theSplineHelper.object_class)
            static_cast<SplineObject *>(entry->object)->GenPoints();
    }
}

void SplineHelper::cbEdSplineReverseSpline(eduimenu_s *, eduiitem_s *, u32) {
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class == theSplineHelper.object_class) {
            SplineObject *spline = static_cast<SplineObject *>(entry->object);
            spline->ReverseKnots();
            if (theSplineHelper.auto_generate_points)
                spline->GenPoints();
        }
    }
}

void SplineHelper::cbEdSplineSmoothSpline(eduimenu_s *, eduiitem_s *, u32) {
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class == theSplineHelper.object_class) {
            SplineObject *spline = static_cast<SplineObject *>(entry->object);
            for (SplineKnot *knot = spline->knots.first; knot != NULL; knot = knot->next)
                knot->Smooth();
            if (theSplineHelper.auto_generate_points)
                spline->GenPoints();
        }
    }
}

/* Smooths the selected knots, then regenerates each touched spline once. */
void SplineHelper::cbEdSplineSmoothKnot(eduimenu_s *, eduiitem_s *, u32) {
    SplineObject *changed[64];
    i32 changed_count = 0;
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class != theKnotHelper.object_class)
            continue;
        SplineKnot *knot = static_cast<SplineKnot *>(entry->object);
        knot->Smooth();
        if (!theSplineHelper.auto_generate_points || knot->spline == NULL)
            continue;
        i32 index;
        for (index = 0; index < changed_count; index++) {
            if (changed[index] == knot->spline)
                break;
        }
        if (index == changed_count)
            changed[changed_count++] = knot->spline;
    }
    for (i32 index = 0; index < changed_count; index++)
        changed[index]->GenPoints();
}

SplineHelper theSplineHelper;
SplineTool theSplineTool;

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
/* First member carrying one of the attribute bits, searching embedded
 * classes when recursive. */
EdRef *EdClass::FindTypeRef(i32 attributes, i32 recursive) {
    for (EdRef *member = members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            if (recursive) {
                EdRef *reference = theRegistry.GetClass(member->type_id)->FindTypeRef(attributes, 1);
                if (reference != NULL)
                    return reference;
            }
        } else if (member->attributes & attributes) {
            return member;
        }
    }
    return NULL;
}

/* Retail keeps this out of line: SerialiseObject calls it (0x6cda5e). */
__declspec(noinline) EdRef *EdClass::FindTypeRef(char *member_name, i32 recursive) {
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
                if (!(member->attributes & 0x80000000)) {
                    EdType *type = theRegistry.GetType(member->type_id);
                    i32 type_size = member->size > 0 ? member->size : type->size;
                    if (stream.mode == 2)
                        member->GetMemberData(object, member->type_id, data, sizeof(data));
                    type->serialise(stream, data, type_size);
                    if (stream.mode == 1)
                        member->SetMemberData(object, member->type_id, data, sizeof(data), NULL);
                } else {
                    EdClass *source_class = registry->GetClass(source->type_id);
                    EdClass *member_class = theRegistry.GetClass(member->type_id);
                    member_class->SerialiseObject(stream, member->GetMemberObject(object), source_class, registry);
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

/* Saving writes the live registry; loading reads the file's schema into a
 * scratch registry in the secondary buffer and maps the objects through it. */
void ClassEditor::Serialise(EdStream &stream) {
    if (stream.mode == 2) {
        theRegistry.Serialise(stream);
        theRegistry.SerialiseObjects(stream, NULL);
    }
    if (stream.mode == 1) {
        MemoryBuffer *saved = stream.memory_buffer;
        stream.memory_buffer = stream.secondary_buffer;
        EdRegistry *source = ::CreateObject<EdRegistry>(stream.secondary_buffer);
        source->Initialise(*stream.secondary_buffer->position, *stream.secondary_buffer->end, 50, 50, 10, 1);
        source->Serialise(stream);
        stream.memory_buffer = saved;
        theRegistry.SerialiseObjects(stream, source);
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

/* The placeable object types (Android PlaceableHelper): every placeable
 * helper registers its interface here so tools can walk all placeables. */
class PlaceableHelper {
  public:
    struct ObjectType {
        char *name;
        PlaceableInterface *object_interface;
    };

    i32 object_type_count;
    i32 iteration_object_type;
    ObjectType object_types[16];

    __forceinline void *GetNextObject(void *object) {
        if (object == NULL)
            iteration_object_type = 0;
        while (iteration_object_type < object_type_count) {
            void *next = object_types[iteration_object_type].object_interface->GetNextObject(object);
            if (next != NULL)
                return next;
            iteration_object_type++;
            object = NULL;
        }
        return NULL;
    }
};
STATIC_ASSERT(sizeof(PlaceableHelper) == 0x88);

PlaceableHelper thePlaceableHelper;

void ClassEditor::cbEdClassSetSnap(eduimenu_s *menu, eduiitem_s *item, u32) {
    theClassEditor.snap_mode = item->data;
    eduiMenuHighlight(menu, item);
}

void ClassEditor::SetViewMenuHilight(eduimenu_s *menu) {
    for (eduiitem_s *item = menu->first; item != NULL; item = item->next) {
        i32 index = item->data;
        if ((u32)(index - 3) <= 60)
            item->highlighted = (theClassEditor.class_filter & 1 << (index - 3)) != 0;
    }
}

void ClassEditor::cbEdClassSetView(eduimenu_s *menu, eduiitem_s *item, u32) {
    i32 index = item->data;
    switch (index) {
        case 0:
            theClassEditor.class_filter = -1;
            break;
        case 1:
            theClassEditor.class_filter = 0;
            break;
        case 2:
            theClassEditor.class_filter = ~theClassEditor.class_filter;
            break;
        default:
            index -= 3;
            if (index >= 0)
                theClassEditor.class_filter ^= 1 << index;
            break;
    }
    SetViewMenuHilight(menu);
}

/* Number keys 1-0 toggle the first ten classes' visibility (with the modifier
 * held, show only that class); the invert key inverts or resets the filter. */
void ClassEditor::UpdateClassFilter(EdInputContext &input) {
    i32 count = theRegistry.class_count;
    if (count > 10)
        count = 10;
    for (i32 index = 0; index < count; index++) {
        if (input.GetPress(index + 26) != 0.0f) {
            if (input.GetHold(22) != 0.0f)
                class_filter = 1 << index;
            else
                class_filter ^= 1 << index;
        }
    }
    if (input.GetPress(36) != 0.0f) {
        if (input.GetHold(22) != 0.0f)
            class_filter = -1;
        else
            class_filter = ~class_filter;
    }
}

void ClassEditor::cbDestroyMenu(eduimenu_s *menu, eduimenu_s *) {
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    if (menu == theClassEditor.menu)
        theClassEditor.menu = NULL;
}

/* Removes placeables that share a type, a position within 0.1 and a name
 * that contains the other's. */
void ClassEditor::cbEdClassRemoveDuplicates(eduimenu_s *, eduiitem_s *, u32) {
    Placeable *objects[256];
    i32 count = 0;
    for (Placeable *object = (Placeable *)thePlaceableHelper.GetNextObject(NULL); object != NULL && count < 256;
         object = (Placeable *)thePlaceableHelper.GetNextObject(object))
        objects[count++] = object;
    for (i32 first = 0; first < count; first++) {
        Placeable *object = objects[first];
        if (object == NULL)
            continue;
        const char *name = object->GetName();
        __declspec(align(16)) VuVec first_position;
        first_position.xyz = object->GetInitialPosition()->xyz;
        i32 object_type;
        for (object_type = 0; object_type < thePlaceableHelper.object_type_count; object_type++) {
            if (thePlaceableHelper.object_types[object_type].object_interface->scene_id == object->scene_id)
                break;
        }
        if (!(VuVecLength(first_position) > 0.1))
            continue;
        for (i32 second = first + 1; second < count; second++) {
            Placeable *other = objects[second];
            if (other == NULL)
                continue;
            const char *other_name = other->GetName();
            __declspec(align(16)) VuVec second_position;
            second_position.xyz = other->GetInitialPosition()->xyz;
            i32 other_type;
            for (other_type = 0; other_type < thePlaceableHelper.object_type_count; other_type++) {
                if (thePlaceableHelper.object_types[other_type].object_interface->scene_id == other->scene_id)
                    break;
            }
            if (!(VuVecLength(first_position) > 0.1))
                continue;
            if (VuVecLength(first_position - second_position) < 0.1 && object_type == other_type &&
                (NuStrStr((char *)name, other_name) != NULL || NuStrStr((char *)other_name, name) != NULL)) {
                thePlaceableHelper.object_types[other_type].object_interface->DestroyObject(other, 0);
                objects[second] = NULL;
            }
        }
    }
}

void ClassEditor::cbEdClassToolsMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdClassRemoveDuplicates, "Remove Duplicates"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void ClassEditor::cbEdClassSnapMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    eduiMenuAddItem(
        menu, eduiItemToggleCreate(1, &EdLevelAttr, theClassEditor.snap_mode == 1, 1, cbEdClassSetSnap, "Height"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(2, &EdLevelAttr, theClassEditor.snap_mode == 2, 1, cbEdClassSetSnap, "Ray"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(0, &EdLevelAttr, theClassEditor.snap_mode == 0, 1, cbEdClassSetSnap, "Off"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

/* Pinning keeps the menu open when it loses the focus; unpinning closes it. */
void ClassEditor::cbEdClassSetPinned(eduimenu_s *menu, eduiitem_s *item, u32) {
    menu->flags ^= 4;
    item->highlighted = !item->highlighted;
    if (menu->flags & 4) {
        eduiMenuDetach(menu);
        edLevelPinnedMenu = menu;
    } else {
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
        if (menu == edLevelActiveMenu)
            edLevelActiveMenu = NULL;
        edLevelPinnedMenu = NULL;
    }
}

/* Shows or hides the LED scenes and, per class, the objects in the view. */
void ClassEditor::cbEdClassViewMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    i32 shortcuts[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    char label[128];
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemCheckCreate(0, &EdLevelAttr, 0, 0, cbEdClassSetPinned, "Pinned"));
    menu->flags &= ~4;
    for (i32 index = 0; index < 10; index++) {
        LevelEditorScene *scene = theLevelEditor.GetEdScene(index);
        if (scene != NULL && scene->active)
            eduiMenuAddItem(
                menu, eduiItemToggleCreate(index << 6, &EdLevelAttr, scene->editable, 1, cbEdFilterLED, scene->name));
    }
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassSetView, "All"));
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdClassSetView, "None"));
    eduiMenuAddItem(menu, eduiItemSelCreate(2, &EdLevelAttr, 0, 0, cbEdClassSetView, "Invert"));
    if (theRegistry.class_count > 0) {
        for (i32 index = 0; index < theRegistry.class_count; index++) {
            EdClass *ed_class = &theRegistry.classes[index];
            if (ed_class->flags & 0x20000000)
                continue;
            if (index < 10)
                NuSPrintf(label, "%d %s", shortcuts[index], ed_class->name);
            else
                NuSPrintf(label, "   %s", ed_class->name);
            eduiMenuAddItem(menu, eduiItemCheckCreate(index + 3, &EdLevelAttr,
                                                      (theClassEditor.class_filter & 1 << index) != 0, 0,
                                                      cbEdClassSetView, label));
        }
    } else {
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "No Registered Classes"));
    }
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

/* Removes the selected objects: each leaves the selection and is destroyed
 * through the registry. */
void ClassEditor::DestroySelectedObjectsNow() {
    ClassObjectListEntry *entry = selected_objects.first;
    while (entry != NULL) {
        if (current_object.object == entry->object && current_object.reference == entry->reference) {
            current_object.ed_class = NULL;
            current_object.object = NULL;
        }
        theRegistry.DestroyObject(entry->ed_class->object_interface, entry->object, 0, 0);
        selected_objects.Remove(entry);
        theMemoryManager.FreePool(entry, sizeof(ClassObjectListEntry));
        entry = selected_objects.first;
    }
}

void ClassEditor::cbDestroyObject(eduimenu_s *, eduiitem_s *item, u32) {
    if (item->data != 0)
        theClassEditor.DestroySelectedObjectsNow();
    eduiSetCameraEnabled(1);
    if (edLevelActiveMenu != NULL) {
        if (edLevelActiveMenu->parent != NULL)
            edLevelActiveMenu->parent->child = NULL;
        if (edLevelActiveMenu->child != NULL)
            edLevelActiveMenu->child->parent = NULL;
    }
    edLevelDestroyActiveMenu = 1;
}

/* Asks for confirmation in a menu at the cursor before destroying. */
void ClassEditor::DestroySelectedObjects() {
    char title[136];
    f32 x;
    f32 y;
    NuStrCpy(title, "Destroy Selected Objects");
    eduiGetCursorCoords(&x, &y);
    x *= 640.0;
    y *= 448.0;
    eduimenu_s *menu =
        eduiMenuCreate((i32)x, (i32)y, 300, 50, reinterpret_cast<void *>(EdLevelFnt), cbDestroyMenu, title);
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbDestroyObject, "Yes"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbDestroyObject, "No"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    edLevelNextMenu = menu;
}

void ClassEditor::cbEdClassDeleteObject(eduimenu_s *, eduiitem_s *, u32) {
    theClassEditor.DestroySelectedObjects();
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

/* Selects the member this control edits as the class editor's sub-object;
 * the select button held toggles it instead. */
i32 EdControl::SelectSubObject() {
    ClassObjectListEntry *entry;
    for (entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->object == object)
            break;
    }
    if (entry == NULL)
        return 1;
    ClassObject selection(entry->ed_class, entry->object);
    selection.reference = entry->reference;
    if (selection.object != NULL) {
        i32 mode = static_cast<i32>(Input->GetHold(16)) != 0 ? 1 : 2;
        selection.reference = reference;
        theClassEditor.SelectObject(selection, mode);
        for (entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
            if (entry->object == object)
                return 1;
        }
        theClassEditor.SelectObject(selection, 1);
    }
    return 1;
}

/* Scales the gizmo with the zoom inputs and tracks the cursor ray at the
 * selection's depth. */
i32 EdManipulator::Process(EdInputContext &input, ClassObjectList &objects) {
    f32 step = 1.0f;
    if (input.GetHold(21) != 0.0f)
        step = 3.0f;
    if (input.GetHold(14) != 0.0f)
        Scale += step * input.delta_time;
    if (input.GetHold(15) != 0.0f) {
        if (Scale > 0.1)
            Scale -= step * input.delta_time;
        else
            Scale = 0.1f;
    }
    __declspec(align(16)) VuVec point;
    f32 distance = 0.0f;
    if (objects.GetAveragePosition(point)) {
        VuVec offset;
        offset.x = point.x - input.ray_start.x;
        offset.y = point.y - input.ray_start.y;
        offset.z = point.z - input.ray_start.z;
        f32 length_squared = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
        distance = NuFsqrt(length_squared);
    }
    VuVec direction = input.ray_direction;
    NuVecNorm(&direction.xyz, &direction.xyz);
    direction.w = 1.0f;
    {
        VuVec scaled;
        scaled.x = direction.x * distance;
        scaled.y = direction.y * distance;
        scaled.z = direction.z * distance;
        point.x = input.ray_start.x + scaled.x;
        point.y = input.ray_start.y + scaled.y;
        point.z = input.ray_start.z + scaled.z;
    }
    if (input.GetPress(3) != 0.0f) {
        delta.x = 0.0f;
        delta.y = 0.0f;
        delta.z = 0.0f;
        delta.w = 1.0f;
        last_point.x = point.x;
        last_point.y = point.y;
        last_point.z = point.z;
        last_point.w = point.w;
    } else {
        VuVec difference;
        difference.x = point.x - last_point.x;
        difference.y = point.y - last_point.y;
        difference.z = point.z - last_point.z;
        delta.x = difference.x;
        delta.y = difference.y;
        delta.z = difference.z;
        delta.w = difference.w;
        last_point.x = point.x;
        last_point.y = point.y;
        last_point.z = point.z;
        last_point.w = point.w;
    }
    VuVec *target = &cursor;
    target->x = input.view_position.x;
    target->y = input.view_position.y;
    target->z = input.view_position.z;
    target->w = input.view_position.w;
    target->x = last_point.x;
    target->y = last_point.y;
    target->z = last_point.z;
    target->w = last_point.w;
    attribute_selected = 0;
    last_point.w = 1.0f;
    delta.w = 1.0f;
    cursor.w = 1.0f;
    for (ClassObjectListEntry *entry = objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class->FindTypeRef(selected_attribute, 1) != NULL) {
            attribute_selected = 1;
            break;
        }
    }
    return 0;
}

void EdManipulator::Render(ClassObjectList &objects) {
    for (ClassObjectListEntry *entry = objects.first; entry != NULL; entry = entry->next)
        theClassEditor.DrawObjectSphere(*reinterpret_cast<ClassObject *>(&entry->ed_class), 0xff800000);
}

/* Gizmo proportions, scaled by EdManipulator::Scale. */
f32 ManipulatorAxisLength = 1.0f;
f32 ManipulatorHandleRadius = 0.25f;
f32 ManipulatorBoxSize = 0.1f;
f32 ManipulatorRotatorPickDistance = 0.05f;

i32 EdManipulator::AxisColour[8] = {(i32)0xffffffff, (i32)0xff0000ff, (i32)0xff00ff00, (i32)0xffff0000,
                                    (i32)0xff00ffff, (i32)0xffff00ff, (i32)0xffffff00, (i32)0xffffffff};

/* Where the rotate gizmo's sphere was last hit (debug output). */
VuVec SpherePos1;
VuVec SpherePos2;

static i32 rotation_axis = 1;

static inline void VuMtxCopy(VuMtx &destination, const VuMtx &source) {
    destination.matrix.m00 = source.matrix.m00;
    destination.matrix.m01 = source.matrix.m01;
    destination.matrix.m02 = source.matrix.m02;
    destination.matrix.m03 = source.matrix.m03;
    destination.matrix.m10 = source.matrix.m10;
    destination.matrix.m11 = source.matrix.m11;
    destination.matrix.m12 = source.matrix.m12;
    destination.matrix.m13 = source.matrix.m13;
    destination.matrix.m20 = source.matrix.m20;
    destination.matrix.m21 = source.matrix.m21;
    destination.matrix.m22 = source.matrix.m22;
    destination.matrix.m23 = source.matrix.m23;
    destination.matrix.m30 = source.matrix.m30;
    destination.matrix.m31 = source.matrix.m31;
    destination.matrix.m32 = source.matrix.m32;
    destination.matrix.m33 = source.matrix.m33;
}

/* Rotates a direction by the matrix's 3x3 part. */
static inline void VuMtxRotateVec(const VuMtx &matrix, const VuVec &v, VuVec &result) {
    f32 x = v.x * matrix.matrix.m00 + v.y * matrix.matrix.m10 + v.z * matrix.matrix.m20;
    f32 y = v.x * matrix.matrix.m01 + v.y * matrix.matrix.m11 + v.z * matrix.matrix.m21;
    result.z = v.x * matrix.matrix.m02 + v.y * matrix.matrix.m12 + v.z * matrix.matrix.m22;
    result.x = x;
    result.y = y;
}

/* result = a * b over the 3x4 affine part; the result's last column is 0 0 0 1. */
static inline void VuMtxMul(const VuMtx &a, const VuMtx &b, VuMtx &result) {
    __declspec(align(16)) VuMtx product;
    product.matrix.m00 = a.matrix.m00 * b.matrix.m00 + a.matrix.m01 * b.matrix.m10 + a.matrix.m02 * b.matrix.m20 +
                         a.matrix.m03 * b.matrix.m30;
    product.matrix.m01 = a.matrix.m00 * b.matrix.m01 + a.matrix.m01 * b.matrix.m11 + a.matrix.m02 * b.matrix.m21 +
                         a.matrix.m03 * b.matrix.m31;
    product.matrix.m02 = a.matrix.m00 * b.matrix.m02 + a.matrix.m01 * b.matrix.m12 + a.matrix.m02 * b.matrix.m22 +
                         a.matrix.m03 * b.matrix.m32;
    product.matrix.m03 = 0.0f;
    product.matrix.m10 = a.matrix.m10 * b.matrix.m00 + a.matrix.m11 * b.matrix.m10 + a.matrix.m12 * b.matrix.m20 +
                         a.matrix.m13 * b.matrix.m30;
    product.matrix.m11 = a.matrix.m10 * b.matrix.m01 + a.matrix.m11 * b.matrix.m11 + a.matrix.m12 * b.matrix.m21 +
                         a.matrix.m13 * b.matrix.m31;
    product.matrix.m12 = a.matrix.m10 * b.matrix.m02 + a.matrix.m11 * b.matrix.m12 + a.matrix.m12 * b.matrix.m22 +
                         a.matrix.m13 * b.matrix.m32;
    product.matrix.m13 = 0.0f;
    product.matrix.m20 = a.matrix.m20 * b.matrix.m00 + a.matrix.m21 * b.matrix.m10 + a.matrix.m22 * b.matrix.m20 +
                         a.matrix.m23 * b.matrix.m30;
    product.matrix.m21 = a.matrix.m20 * b.matrix.m01 + a.matrix.m21 * b.matrix.m11 + a.matrix.m22 * b.matrix.m21 +
                         a.matrix.m23 * b.matrix.m31;
    product.matrix.m22 = a.matrix.m20 * b.matrix.m02 + a.matrix.m21 * b.matrix.m12 + a.matrix.m22 * b.matrix.m22 +
                         a.matrix.m23 * b.matrix.m32;
    product.matrix.m23 = 0.0f;
    product.matrix.m30 = a.matrix.m30 * b.matrix.m00 + a.matrix.m31 * b.matrix.m10 + a.matrix.m32 * b.matrix.m20 +
                         a.matrix.m33 * b.matrix.m30;
    product.matrix.m31 = a.matrix.m30 * b.matrix.m01 + a.matrix.m31 * b.matrix.m11 + a.matrix.m32 * b.matrix.m21 +
                         a.matrix.m33 * b.matrix.m31;
    product.matrix.m32 = a.matrix.m30 * b.matrix.m02 + a.matrix.m31 * b.matrix.m12 + a.matrix.m32 * b.matrix.m22 +
                         a.matrix.m33 * b.matrix.m32;
    product.matrix.m33 = 1.0f;
    VuMtxCopy(result, product);
}

/* A scaling matrix with the vector's components on the diagonal. */
static inline void VuMtxSetScale(VuMtx &matrix, const VuVec &scale) {
    matrix.matrix.m00 = scale.x;
    matrix.matrix.m01 = 0.0f;
    matrix.matrix.m02 = 0.0f;
    matrix.matrix.m03 = 0.0f;
    matrix.matrix.m10 = 0.0f;
    matrix.matrix.m11 = scale.y;
    matrix.matrix.m12 = 0.0f;
    matrix.matrix.m13 = 0.0f;
    matrix.matrix.m20 = 0.0f;
    matrix.matrix.m21 = 0.0f;
    matrix.matrix.m22 = scale.z;
    matrix.matrix.m23 = 0.0f;
    matrix.matrix.m30 = 0.0f;
    matrix.matrix.m31 = 0.0f;
    matrix.matrix.m32 = 0.0f;
    matrix.matrix.m33 = scale.w;
}

/* Rotate every row of the matrix about one axis by a 16-bit angle. */
static inline void VuMtxRotateX(VuMtx &matrix, i32 angle) {
    f32 cosine = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff];
    f32 sine = NuTrigTable[angle >> 1 & 0x7fff];
    f32 y0 = matrix.matrix.m01;
    f32 y1 = matrix.matrix.m11;
    f32 y2 = matrix.matrix.m21;
    f32 y3 = matrix.matrix.m31;
    matrix.matrix.m01 = y0 * cosine - matrix.matrix.m02 * sine;
    matrix.matrix.m02 = matrix.matrix.m02 * cosine + y0 * sine;
    matrix.matrix.m11 = y1 * cosine - matrix.matrix.m12 * sine;
    matrix.matrix.m12 = matrix.matrix.m12 * cosine + y1 * sine;
    matrix.matrix.m21 = y2 * cosine - matrix.matrix.m22 * sine;
    matrix.matrix.m22 = matrix.matrix.m22 * cosine + y2 * sine;
    matrix.matrix.m31 = y3 * cosine - matrix.matrix.m32 * sine;
    matrix.matrix.m32 = matrix.matrix.m32 * cosine + y3 * sine;
}

static inline void VuMtxRotateY(VuMtx &matrix, i32 angle) {
    f32 cosine = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff];
    f32 sine = NuTrigTable[angle >> 1 & 0x7fff];
    f32 x0 = matrix.matrix.m00;
    f32 x1 = matrix.matrix.m10;
    f32 x2 = matrix.matrix.m20;
    f32 x3 = matrix.matrix.m30;
    matrix.matrix.m00 = x0 * cosine + matrix.matrix.m02 * sine;
    matrix.matrix.m02 = matrix.matrix.m02 * cosine - x0 * sine;
    matrix.matrix.m10 = x1 * cosine + matrix.matrix.m12 * sine;
    matrix.matrix.m12 = matrix.matrix.m12 * cosine - x1 * sine;
    matrix.matrix.m20 = x2 * cosine + matrix.matrix.m22 * sine;
    matrix.matrix.m22 = matrix.matrix.m22 * cosine - x2 * sine;
    matrix.matrix.m30 = x3 * cosine + matrix.matrix.m32 * sine;
    matrix.matrix.m32 = matrix.matrix.m32 * cosine - x3 * sine;
}

static inline void VuMtxRotateZ(VuMtx &matrix, i32 angle) {
    f32 cosine = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff];
    f32 sine = NuTrigTable[angle >> 1 & 0x7fff];
    f32 x0 = matrix.matrix.m00;
    f32 x1 = matrix.matrix.m10;
    f32 x2 = matrix.matrix.m20;
    f32 x3 = matrix.matrix.m30;
    matrix.matrix.m00 = x0 * cosine - matrix.matrix.m01 * sine;
    matrix.matrix.m01 = x0 * sine + matrix.matrix.m01 * cosine;
    matrix.matrix.m10 = x1 * cosine - matrix.matrix.m11 * sine;
    matrix.matrix.m11 = x1 * sine + matrix.matrix.m11 * cosine;
    matrix.matrix.m20 = x2 * cosine - matrix.matrix.m21 * sine;
    matrix.matrix.m21 = x2 * sine + matrix.matrix.m21 * cosine;
    matrix.matrix.m30 = x3 * cosine - matrix.matrix.m31 * sine;
    matrix.matrix.m31 = x3 * sine + matrix.matrix.m31 * cosine;
}

/* Handle positions: 0 origin, 1-3 axis tips, 4-6 plane boxes, 7 centre. */
void EdManipulator::GetAxisLocators(VuVec &origin, VuVec *points, VuMtx *matrix) {
    f32 length = ManipulatorAxisLength * Scale;
    VuVecSetOrigin(points[3]);
    VuVecCopy(points[2], points[3]);
    VuVecCopy(points[1], points[2]);
    VuVecCopy(points[7], points[1]);
    VuVecCopy(points[0], points[7]);
    points[1].x += length;
    points[2].y += length;
    points[3].z += length;
    VuVecCopy(points[4], (points[1] + points[2]) * 0.5f);
    VuVecCopy(points[5], (points[3] + points[1]) * 0.5f);
    VuVecCopy(points[6], (points[2] + points[3]) * 0.5f);
    if (matrix != NULL) {
        for (i32 index = 0; index < 8; index++)
            VuMtxRotateVec(*matrix, points[index], points[index]);
    }
    for (i32 index = 0; index < 8; index++) {
        points[index].x += origin.x;
        points[index].y += origin.y;
        points[index].z += origin.z;
    }
}

/* The move/scale gizmo: spheres at the axis tips, boxes on the planes and
 * the centre, and arrows along the axes; grey when nothing editable is
 * selected. */
void EdManipulator::DrawAxis(VuVec &origin, VuMtx *matrix) {
    __declspec(align(16)) VuVec points[8];
    __declspec(align(16)) VuMtx box;
    GetAxisLocators(origin, points, matrix);
    EdDrawBegin(1);
    EdDrawLineSphere(&points[1].xyz, ManipulatorHandleRadius * Scale, 1.0f,
                     attribute_selected ? AxisColour[1] : 0xff808080);
    EdDrawLineSphere(&points[2].xyz, ManipulatorHandleRadius * Scale, 1.0f,
                     attribute_selected ? AxisColour[2] : 0xff808080);
    EdDrawLineSphere(&points[3].xyz, ManipulatorHandleRadius * Scale, 1.0f,
                     attribute_selected ? AxisColour[3] : 0xff808080);
    VuMtxCopy(box, *(const VuMtx *)&ANI3_EVAL_IDENTITY_MATRIX);
    box.matrix.m30 = points[4].x;
    box.matrix.m31 = points[4].y;
    box.matrix.m32 = points[4].z;
    box.matrix.m33 = 1.0f;
    EdDrawLineCube(&box.matrix, ManipulatorBoxSize * Scale, attribute_selected ? AxisColour[4] : 0xff808080);
    box.matrix.m30 = points[5].x;
    box.matrix.m31 = points[5].y;
    box.matrix.m32 = points[5].z;
    box.matrix.m33 = 1.0f;
    EdDrawLineCube(&box.matrix, ManipulatorBoxSize * Scale, attribute_selected ? AxisColour[5] : 0xff808080);
    box.matrix.m30 = points[6].x;
    box.matrix.m31 = points[6].y;
    box.matrix.m32 = points[6].z;
    box.matrix.m33 = 1.0f;
    EdDrawLineCube(&box.matrix, ManipulatorBoxSize * Scale, attribute_selected ? AxisColour[6] : 0xff808080);
    box.matrix.m30 = points[7].x;
    box.matrix.m31 = points[7].y;
    box.matrix.m32 = points[7].z;
    box.matrix.m33 = 1.0f;
    EdDrawLineCube(&box.matrix, ManipulatorBoxSize * Scale, attribute_selected ? AxisColour[7] : 0xff808080);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
    EdDrawBegin(0);
    f32 size = ManipulatorHandleRadius * Scale;
    __declspec(align(16)) VuVec offset;
    __declspec(align(16)) VuVec start;
    __declspec(align(16)) VuVec end;
    VuVecCopy(offset, (points[1] - points[7]) * size);
    VuVecCopy(start, points[1] - offset * 0.5f);
    VuVecCopy(end, points[1] + offset * 0.5f);
    f32 radius = size * 0.2f;
    EdDrawPolyArrow(&start.xyz, &end.xyz, 8, attribute_selected ? AxisColour[1] : 0xff808080, radius, radius, 0.5f,
                    0.0f);
    VuVecCopy(offset, (points[2] - points[7]) * size);
    VuVecCopy(start, points[2] - offset * 0.5f);
    VuVecCopy(end, points[2] + offset * 0.5f);
    EdDrawPolyArrow(&start.xyz, &end.xyz, 8, attribute_selected ? AxisColour[2] : 0xff808080, radius, radius, 0.2f,
                    0.0f);
    VuVecCopy(offset, (points[3] - points[7]) * size);
    VuVecCopy(start, points[3] - offset * 0.5f);
    VuVecCopy(end, points[3] + offset * 0.5f);
    EdDrawPolyArrow(&start.xyz, &end.xyz, 8, attribute_selected ? AxisColour[3] : 0xff808080, radius, radius, 0.2f,
                    0.0f);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
}

/* The rotate gizmo: one circle per axis and the swept sector of a drag. */
void EdManipulator::DrawRotator(VuVec &origin) {
    f32 radius = Scale * ManipulatorAxisLength;
    EdDrawBegin(1);
    EdDrawLineCircleX(&origin.xyz, radius, attribute_selected ? AxisColour[1] : 0xff808080, 32);
    EdDrawLineCircleY(&origin.xyz, radius, attribute_selected ? AxisColour[2] : 0xff808080, 32);
    EdDrawLineCircleZ(&origin.xyz, radius, attribute_selected ? AxisColour[3] : 0xff808080, 32);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
    if (start_angle != last_angle && selected_axis != 0) {
        EdDrawBegin(1);
        EdDrawPolySector(&origin.xyz, radius, selected_axis - 1, start_angle, last_angle, 0x80808080, 32);
        if (NewPrim != 0) {
            if (NewPrim == 2 || NewPrim == 3)
                NuPrim3DEnd();
            NewPrim = 0;
        }
    }
}

/* Picks the handle nearest the cursor ray and returns its index (0 for
 * none) with the axes it moves along; a drag keeps the pressed handle. */
i32 EdManipulator::SelectAxis(EdInputContext &input, VuVec &origin, VuVec &first, VuVec &second, VuMtx *matrix) {
    __declspec(align(16)) VuVec points[8];
    VuVecSetOrigin(first);
    VuVecSetOrigin(second);
    i32 nearest = 0;
    GetAxisLocators(origin, points, matrix);
    f32 nearest_distance = 3.402823466e38f;
    for (i32 index = 1; index < 8; index++) {
        f32 distance = LineToPointDistance(input.ray_start, input.ray_direction, points[index], NULL);
        f32 radius = index < 4 ? ManipulatorHandleRadius * Scale : ManipulatorBoxSize * Scale;
        if (distance < radius && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = index;
        }
    }
    switch (nearest) {
        case 1:
            VuVecSet(first, 1.0f, 0.0f, 0.0f, 1.0f);
            break;
        case 2:
            VuVecSet(first, 0.0f, 1.0f, 0.0f, 1.0f);
            break;
        case 3:
            VuVecSet(first, 0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case 4:
            VuVecSet(first, 1.0f, 0.0f, 0.0f, 1.0f);
            VuVecSet(second, 0.0f, 1.0f, 0.0f, 1.0f);
            break;
        case 5:
            VuVecSet(first, 1.0f, 0.0f, 0.0f, 1.0f);
            VuVecSet(second, 0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case 6:
        case 7:
            VuVecSet(first, 0.0f, 1.0f, 0.0f, 1.0f);
            VuVecSet(second, 0.0f, 0.0f, 1.0f, 1.0f);
            break;
    }
    if (matrix != NULL) {
        VuMtxRotateVec(*matrix, first, first);
        VuMtxRotateVec(*matrix, second, second);
        VuVecNormalise(first, first);
        VuVecNormalise(second, second);
    }
    if (input.GetPress(3) != 0.0f) {
        selected_axis = nearest;
        VuVecCopy(first_axis, first);
        VuVecCopy(second_axis, second);
        return nearest;
    }
    if (input.GetHold(3) != 0.0f) {
        VuVecCopy(first, first_axis);
        VuVecCopy(second, second_axis);
        return selected_axis;
    }
    selected_axis = 0;
    VuVecSetOrigin(first_axis);
    VuVecSetOrigin(second_axis);
    return nearest;
}

/* Intersects the line through origin along direction with a sphere. */
i32 LineToSphereIntersection(VuVec &origin, VuVec &direction, VuVec &center, f32 radius, VuVec *far_intersection,
                             VuVec *near_intersection) {
    __declspec(align(16)) VuVec end = origin + direction;
    __declspec(align(16)) VuVec delta = end - origin;
    f32 a = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    __declspec(align(16)) VuVec offset = origin - center;
    f32 b = 2 * VuVecDot(delta, offset);
    f32 c = VuVecLengthSq(center);
    c += VuVecLengthSq(origin);
    c -= 2.0 * VuVecDot(center, origin);
    c -= radius * radius;
    f32 discriminant = b * b - 4.0 * a * c;
    if (VuFabs(a) < 1.1920929e-7f || discriminant < 0.0f)
        return 0;
    f32 far_t = (-b + NuFsqrt(discriminant)) / (2.0 * a);
    f32 near_t = (-b - NuFsqrt(discriminant)) / (2.0 * a);
    if (far_intersection != NULL)
        VuVecCopy(*far_intersection, origin + direction * far_t);
    if (near_intersection != NULL)
        VuVecCopy(*near_intersection, origin + direction * near_t);
    return 1;
}

/* Picks the rotation circle under the cursor; while dragging, tracks the
 * angle swept around the picked axis in angle_delta. */
i32 EdManipulator::SelectRotator(EdInputContext &input, VuVec &center, VuVec &plane) {
    __declspec(align(16)) VuVec hits[2];
    __declspec(align(16)) VuVec normals[3];
    __declspec(align(16)) VuVec chosen;
    f32 radius = ManipulatorAxisLength * Scale;
    f32 nearest_distance = 3.402823466e38f;
    i32 axis = 0;
    i32 angle = 0;
    if (input.GetPress(3) == 0.0f && input.GetHold(3) != 0.0f) {
        VuVecCopy(plane, first_axis);
        axis = selected_axis;
        if (LineToPlaneIntersecion(input.ray_start, input.ray_direction, plane, &chosen)) {
            f32 x = chosen.x - center.x;
            f32 y = chosen.y - center.y;
            f32 z = chosen.z - center.z;
            switch (axis) {
                case 1:
                    angle = NuAtan2DA(y, z);
                    break;
                case 2:
                    angle = NuAtan2DA(x, -z);
                    break;
                case 3:
                    angle = NuAtan2DA(x, y);
                    break;
            }
            angle_delta = VuAngleDifference(last_angle, angle);
            last_angle = angle;
            return axis;
        }
        angle_delta = 0;
        return axis;
    }
    VuVecSet(normals[0], 1.0f, 0.0f, 0.0f, -center.x);
    VuVecSet(normals[1], 0.0f, 1.0f, 0.0f, -center.y);
    VuVecSet(normals[2], 0.0f, 0.0f, 1.0f, -center.z);
    if (LineToSphereIntersection(input.ray_start, input.ray_direction, center, radius + 0.01f, &hits[0], &hits[1])) {
        VuVecCopy(SpherePos1, hits[0]);
        VuVecCopy(SpherePos2, hits[1]);
        for (i32 candidate = 0; candidate < 3; candidate++) {
            for (i32 hit = 0; hit < 2; hit++) {
                VuVec &point = hits[hit];
                VuVec &normal = normals[candidate];
                __declspec(align(16)) VuVec projection;
                __declspec(align(16)) VuVec screen_point;
                f32 distance_to_plane = VuVecDot(normal, point) + normal.w;
                VuVecCopy(projection, point - normal * distance_to_plane);
                NuCameraTransformScreenClip(&projection.xyz, &projection.xyz, 1, NULL);
                NuCameraTransformScreenClip(&screen_point.xyz, &point.xyz, 1, NULL);
                f32 distance = VuVecLength(screen_point - projection);
                if (distance < ManipulatorRotatorPickDistance && distance < nearest_distance) {
                    nearest_distance = distance;
                    chosen.x = point.x;
                    chosen.y = point.y;
                    chosen.z = point.z;
                    axis = candidate + 1;
                }
            }
        }
    }
    f32 x = chosen.x - center.x;
    f32 y = chosen.y - center.y;
    f32 z = chosen.z - center.z;
    switch (axis) {
        case 1:
            VuVecSet(plane, 1.0f, 0.0f, 0.0f, 1.0f);
            plane.w = -center.x;
            angle = NuAtan2DA(y, z);
            break;
        case 2:
            VuVecSet(plane, 0.0f, 1.0f, 0.0f, 1.0f);
            plane.w = -center.y;
            angle = NuAtan2DA(x, -z);
            break;
        case 3:
            VuVecSet(plane, 0.0f, 0.0f, 1.0f, 1.0f);
            plane.w = -center.z;
            angle = NuAtan2DA(x, y);
            break;
    }
    if (input.GetPress(3) != 0.0f) {
        selected_axis = axis;
        VuVecCopy(first_axis, plane);
        start_angle = angle;
        last_angle = angle;
        angle_delta = 0;
        return axis;
    }
    selected_axis = 0;
    VuVecSetOrigin(first_axis);
    VuVecSetOrigin(second_axis);
    start_angle = 0;
    last_angle = 0;
    angle_delta = 0;
    return axis;
}

/* Moves the selected objects along the picked axis or plane; with the snap
 * input held, the centre handle drops them at the cursor position. */
i32 EdManMove::Process(EdInputContext &input, ClassObjectList &objects) {
    EdManipulator::Process(input, objects);
    __declspec(align(16)) VuVec position;
    if (!objects.GetAveragePosition(position))
        return 0;
    __declspec(align(16)) VuVec first;
    __declspec(align(16)) VuVec second;
    i32 axis = SelectAxis(input, position, first, second, NULL);
    theLevelEditor.field_0x2c = AxisColour[axis];
    if (input.GetHold(3) == 0.0f && input.GetHold(38) == 0.0f)
        return axis != 0;
    for (ClassObjectListEntry *entry = objects.first; entry != NULL; entry = entry->next) {
        ClassObject *object = reinterpret_cast<ClassObject *>(&entry->ed_class);
        VuVecSetOrigin(position);
        object->GetAttributeData(8, EdType_VuVec, &position, 0);
        switch (axis) {
            case 1:
            case 2:
            case 3: {
                f32 amount = delta.x * first.x + delta.y * first.y + delta.z * first.z;
                if (amount == 0.0f)
                    continue;
                __declspec(align(16)) VuVec offset = first * amount;
                position.x += offset.x;
                position.y += offset.y;
                position.z += offset.z;
                break;
            }
            case 4:
            case 5:
            case 6: {
                i32 moved = 0;
                f32 amount = delta.x * first.x + delta.y * first.y + delta.z * first.z;
                if (amount != 0.0f) {
                    __declspec(align(16)) VuVec offset = first * amount;
                    position.x += offset.x;
                    position.y += offset.y;
                    position.z += offset.z;
                    moved = 1;
                }
                amount = delta.x * second.x + delta.y * second.y + delta.z * second.z;
                if (amount != 0.0f) {
                    __declspec(align(16)) VuVec offset = second * amount;
                    position.x += offset.x;
                    position.y += offset.y;
                    position.z += offset.z;
                } else if (!moved) {
                    continue;
                }
                break;
            }
            case 7:
                if (input.GetHold(38) == 0.0f)
                    continue;
                position.x = theLevelEditor.background_colour[0];
                position.y = theLevelEditor.background_colour[1];
                position.z = theLevelEditor.background_colour[2];
                position.w = theLevelEditor.background_colour[3];
                break;
            default:
                continue;
        }
        if (theClassEditor.snap_mode == 1 || theClassEditor.snap_mode == 2) {
            /* The terrain snap of the other builds is compiled out; only
             * the ray normalisation is left. */
            VuVec ray = theClassEditor.snap_ray;
            NuVecNorm(&ray.xyz, &ray.xyz);
        }
        object->SetAttributeData(8, EdType_VuVec, &position, 0);
    }
    return axis != 0;
}

void EdManMove::Render(ClassObjectList &objects) {
    __declspec(align(16)) VuVec position;
    if (objects.count > 2)
        EdManipulator::Render(objects);
    if (objects.GetAveragePosition(position))
        DrawAxis(position, NULL);
}

/* Turns every selected object's transform about one axis, keeping its
 * position. */
i32 EdManRotate::RotateItem(EdInputContext &, ClassObjectList &objects, i32 angle, i32 axis) {
    ClassObjectListEntry *entry = objects.first;
    __declspec(align(16)) VuVec position;
    objects.GetAveragePosition(position);
    if (angle == 0 || entry == NULL)
        return axis;
    for (; entry != NULL; entry = entry->next) {
        ClassObject *object = reinterpret_cast<ClassObject *>(&entry->ed_class);
        __declspec(align(16)) VuMtx matrix;
        VuMtxCopy(matrix, *(const VuMtx *)&ANI3_EVAL_IDENTITY_MATRIX);
        if (!object->GetAttributeData(0x10, EdType_VuMtx, &matrix, 0))
            continue;
        switch (axis) {
            case 1:
                position.x = matrix.matrix.m30;
                position.y = matrix.matrix.m31;
                position.z = matrix.matrix.m32;
                position.w = matrix.matrix.m33;
                VuMtxRotateX(matrix, angle);
                break;
            case 2:
                position.x = matrix.matrix.m30;
                position.y = matrix.matrix.m31;
                position.z = matrix.matrix.m32;
                position.w = matrix.matrix.m33;
                VuMtxRotateY(matrix, angle);
                break;
            case 3:
                position.x = matrix.matrix.m30;
                position.y = matrix.matrix.m31;
                position.z = matrix.matrix.m32;
                position.w = matrix.matrix.m33;
                VuMtxRotateZ(matrix, angle);
                break;
            default:
                continue;
        }
        matrix.matrix.m30 = position.x;
        matrix.matrix.m31 = position.y;
        matrix.matrix.m32 = position.z;
        matrix.matrix.m33 = position.w;
        object->SetAttributeData(0x10, EdType_VuMtx, &matrix, 0);
    }
    return axis;
}

/* Drags rotate about the picked circle; the analog pad (with its button
 * cycling the axis) rotates about rotation_axis. */
i32 EdManRotate::Process(EdInputContext &input, ClassObjectList &objects) {
    EdManipulator::Process(input, objects);
    __declspec(align(16)) VuVec position;
    if (!objects.GetAveragePosition(position))
        return 0;
    eduiSetCameraEnabled(1);
    if (input.pad->digital_buttons_pressed & 0x10) {
        if (++rotation_axis == 4)
            rotation_axis = 1;
    }
    if (input.GetHold(38) != 0.0f) {
        eduiSetCameraEnabled(0);
        return RotateItem(input, objects, (i32)(eduiGetAnalougePadValue(input.pad) * 400.0), rotation_axis);
    }
    if (input.GetHold(3) == 0.0f)
        return 0;
    __declspec(align(16)) VuVec plane;
    i32 axis = SelectRotator(input, position, plane);
    theLevelEditor.field_0x2c = AxisColour[axis];
    return RotateItem(input, objects, angle_delta, axis);
}

void EdManRotate::Render(ClassObjectList &objects) {
    __declspec(align(16)) VuVec position;
    if (objects.count > 2)
        EdManipulator::Render(objects);
    if (objects.GetAveragePosition(position))
        DrawRotator(position);
}

/* Scales the selected objects' scale matrices along the picked axis or
 * plane, or uniformly with the analog inputs on the centre handle. */
i32 EdManScale::Process(EdInputContext &input, ClassObjectList &objects) {
    EdManipulator::Process(input, objects);
    __declspec(align(16)) VuVec position;
    if (!objects.GetAveragePosition(position))
        return 0;
    __declspec(align(16)) VuMtx transform;
    reinterpret_cast<ClassObject *>(&objects.first->ed_class)->GetAttributeData(0x10, EdType_VuMtx, &transform, 0);
    __declspec(align(16)) VuVec first;
    __declspec(align(16)) VuVec second;
    i32 axis = SelectAxis(input, position, first, second, &transform);
    theLevelEditor.field_0x2c = AxisColour[axis];
    if (input.GetHold(3) == 0.0f)
        return axis != 0;
    for (ClassObjectListEntry *entry = objects.first; entry != NULL; entry = entry->next) {
        ClassObject *object = reinterpret_cast<ClassObject *>(&entry->ed_class);
        __declspec(align(16)) VuMtx scaling;
        __declspec(align(16)) VuMtx result;
        __declspec(align(16)) VuVec scale;
        __declspec(align(16)) VuVec projected;
        __declspec(align(16)) VuVec unit;
        VuVecSet(unit, 1.0f, 1.0f, 1.0f, 1.0f);
        object->GetAttributeData(0x20, EdType_VuMtx, &transform, 0);
        switch (axis) {
            case 1:
            case 2:
            case 3: {
                VuMtxRotateVec(transform, first, projected);
                f32 length = VuVecLength(projected);
                length = Scale * length;
                f32 movement = VuVecDot(delta, first);
                if (movement == 0.0f)
                    continue;
                NuVecInvMtxRotate(&first.xyz, &first.xyz, &transform.matrix);
                VuVecNormalise(first, first);
                f32 change = (movement + length) / length;
                change -= 1.0f;
                VuVecCopy(scale, first * change + unit);
                VuMtxSetScale(scaling, scale);
                scaling.matrix.m33 = 1.0f;
                position.x = transform.matrix.m30;
                position.y = transform.matrix.m31;
                position.z = transform.matrix.m32;
                position.w = transform.matrix.m33;
                VuMtxMul(scaling, transform, result);
                break;
            }
            case 4:
            case 5:
            case 6: {
                VuMtxRotateVec(transform, first, projected);
                f32 length = Scale * VuVecLength(projected);
                f32 movement = VuVecDot(delta, second) + VuVecDot(delta, first);
                if (movement == 0.0f)
                    continue;
                NuVecInvMtxRotate(&first.xyz, &first.xyz, &transform.matrix);
                VuVecNormalise(first, first);
                f32 change = (movement + length) / length;
                change -= 1.0f;
                VuVecCopy(scale, second * change + first * change + unit);
                VuMtxSetScale(scaling, scale);
                scaling.matrix.m33 = 1.0f;
                position.x = transform.matrix.m30;
                position.y = transform.matrix.m31;
                position.z = transform.matrix.m32;
                position.w = transform.matrix.m33;
                VuMtxMul(scaling, transform, result);
                break;
            }
            case 7: {
                f32 movement = input.Get(1) - input.Get(0) + input.Get(2);
                if (movement == 0.0f)
                    continue;
                movement = movement * 0.005;
                f32 factor = movement + 1.0;
                VuVecCopy(scale, unit * factor);
                VuMtxSetScale(scaling, scale);
                scaling.matrix.m33 = 1.0f;
                position.x = transform.matrix.m30;
                position.y = transform.matrix.m31;
                position.z = transform.matrix.m32;
                position.w = transform.matrix.m33;
                VuMtxMul(scaling, transform, result);
                break;
            }
            default:
                continue;
        }
        VuMtxCopy(transform, result);
        transform.matrix.m30 = position.x;
        transform.matrix.m31 = position.y;
        transform.matrix.m32 = position.z;
        transform.matrix.m33 = position.w;
        object->SetAttributeData(0x20, EdType_VuMtx, &transform, 0);
    }
    return axis != 0;
}

void EdManScale::Render(ClassObjectList &objects) {
    __declspec(align(16)) VuVec position;
    __declspec(align(16)) VuMtx transform;
    if (objects.count > 2)
        EdManipulator::Render(objects);
    if (objects.GetAveragePosition(position)) {
        reinterpret_cast<ClassObject *>(&objects.first->ed_class)->GetAttributeData(0x20, EdType_VuMtx, &transform, 0);
        DrawAxis(position, &transform);
    }
}

void EdControl::cbSelected(eduimenu_s *menu, eduiitem_s *item, u32) {
    EdControl *control = static_cast<EdControl *>(item->data_ptr);
    i32 selected = control->SelectSubObject();
    theClassEditor.mode = 0;
    theClassEditor.manipulator = &theDefaultManipulator;
    thePropertyTool.SelectAttr(theClassEditor.manipulator->selected_attribute);
    if (selected != 0)
        *reinterpret_cast<eduiiattr_s *>(item->colours) = thePropertyTool.unselected_attr;
    for (PropertyMenu *property_menu = thePropertyTool.menus.first; property_menu != NULL;
         property_menu = property_menu->next) {
        if (property_menu->menu == menu) {
            property_menu->control = control;
            return;
        }
    }
}

void EdControl::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdControl *control = new (theMemoryManager.AllocPool(sizeof(EdControl), 1)) EdControl;
    if (control != NULL) {
        control->reference = member;
        control->object = target;
        control->item = eduiItemPropCreate((u32)control, &EdLevelAttr, cbSelected, NULL, NULL, 0, member->name, NULL);
        eduiMenuAddItem(menu, control->item);
    }
}

void EdEnumControl::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdEnumControl *control = new (theMemoryManager.AllocPool(sizeof(EdEnumControl), 1)) EdEnumControl(items);
    if (control != NULL) {
        control->reference = member;
        control->object = target;
        i32 value;
        member->GetMemberData(target, EdType_Int, &value, 0);
        control->item = eduiItemPropCreate((u32)control, &EdLevelAttr, cbSelected, cbChanged, cbButton, 1, member->name,
                                           GetEnumString(value));
        eduiMenuAddItem(menu, control->item);
    }
}

/* One expander with a child property per bit of the mask. */
void EdBitControl::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdBitControl *control = new (theMemoryManager.AllocPool(sizeof(EdBitControl), 1)) EdBitControl(bit_mask, items);
    if (control != NULL) {
        control->reference = member;
        control->object = target;
        i32 value;
        member->GetMemberData(control->object, EdType_Int, &value, 0);
        control->item = eduiItemExpanderCreate((u32)control, &EdLevelAttr, cbSelected, member->name);
        eduiMenuAddItem(menu, control->item);
        for (i32 bit = 0; bit < 32; bit++) {
            u32 mask = 1 << bit;
            if (control->bit_mask & mask) {
                char bit_name[32];
                NuSPrintf(bit_name, "%d", bit);
                eduiItemExpanderAddChild(control->item, eduiItemPropCreateEx((u32)control, &EdLevelAttr, cbSelected,
                                                                             cbChanged, cbButton, 1, bit_name,
                                                                             GetEnumString((value & mask) != 0), bit));
            }
        }
    }
}

void EdStringControl::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdStringControl *control = new (theMemoryManager.AllocPool(sizeof(EdStringControl), 1)) EdStringControl;
    if (control != NULL) {
        control->reference = member;
        control->object = target;
        char value[128];
        control->GetVal(value, sizeof(value));
        control->item =
            eduiItemPropCreate((u32)control, &EdLevelAttr, cbSelected, cbChanged, cbPress, 1, member->name, value);
        eduiMenuAddItem(menu, control->item);
    }
}

void EdColourControl::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdColourControl *control = new (theMemoryManager.AllocPool(sizeof(EdColourControl), 1)) EdColourControl;
    if (control != NULL) {
        control->reference = member;
        control->object = target;
        NUVEC colour;
        member->GetMemberData(control->object, EdType_Colour3, &colour, 0);
        char name[128];
        NuSPrintf(name, "%s", member->name);
        char value[128];
        NuSPrintf(value, "%.2f %.2f %.2f", colour.x, colour.y, colour.z);
        edui_prop_s *property = reinterpret_cast<edui_prop_s *>(
            eduiItemPropCreate((u32)control, &EdLevelAttr, cbSelected, cbChanged, cbButton, 3, name, value));
        control->item = &property->base;
        i32 packed = (i32)(colour.z * 255.0);
        packed |= 0xffffff00;
        packed <<= 8;
        packed |= (i32)(colour.y * 255.0) & 0xff;
        packed <<= 8;
        packed |= (i32)(colour.x * 255.0) & 0xff;
        property->button_colour = packed;
        eduiMenuAddItem(menu, control->item);
    }
}

void PropertyTool::Initialise(variptr_u &, variptr_u &, i32) {
}

void PropertyMenu::SelectAttr(i32 selected) {
    for (eduiitem_s *item = menu->first; item != NULL; item = item->next) {
        EdControl *control = static_cast<EdControl *>(item->data_ptr);
        if (control != NULL)
            control->SetMenuItemAttr(selected, item, &thePropertyTool.selected_attr, &thePropertyTool.unselected_attr);
    }
}

void PropertyTool::SelectAttr(i32 selected) {
    for (PropertyMenu *property_menu = menus.first; property_menu != NULL; property_menu = property_menu->next)
        property_menu->SelectAttr(selected);
}

/* Members the active manipulator edits (mask) are drawn unselected unless
 * another member of the same object is the selected sub-object. */
void EdControl::SetMenuItemAttr(i32 mask, eduiitem_s *menu_item, eduiiattr_s *selected, eduiiattr_s *unselected) {
    if (reference->attributes & mask) {
        ClassObjectListEntry *entry = theClassEditor.selected_objects.first;
        i32 subobject_count = 0;
        if (entry != NULL) {
            for (; entry != NULL; entry = entry->next) {
                if (entry->object == object && entry->reference != NULL) {
                    subobject_count++;
                    if (entry->reference == reference)
                        break;
                }
            }
            if (entry == NULL && subobject_count != 0) {
                memcpy(menu_item->colours, selected, sizeof(eduiiattr_s));
                return;
            }
        }
        memcpy(menu_item->colours, unselected, sizeof(eduiiattr_s));
        return;
    }
    memcpy(menu_item->colours, selected, sizeof(eduiiattr_s));
}

static EdEnumControl *active_enum_control;
static EdBitControl *edBitControl;
static edui_prop_s *edBitItem;
static i32 edBitIndex;

void EdEnumControl::cbSelectItem(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    if (active_enum_control != NULL) {
        Item *choice = static_cast<Item *>(item->data_ptr);
        eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(active_enum_control->item),
                            active_enum_control->GetEnumString(choice->value));
        active_enum_control->reference->SetMemberData(active_enum_control->object, EdType_Int, &choice->value, 0, NULL);
        cbEdLevelDestroyOnSelect(menu, item, flags);
    }
}

/* Pops up the list of choices next to the item. */
void EdEnumControl::cbButton(eduimenu_s *menu, eduiitem_s *item, u32) {
    active_enum_control = static_cast<EdEnumControl *>(item->data_ptr);
    eduimenu_s *choices = eduiMenuCreate(item->x + menu->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                         cbEdLevelDestroy, NULL);
    if (choices == NULL)
        return;
    for (Item *entry = active_enum_control->items; entry->name != NULL; entry++)
        eduiMenuAddItem(choices, eduiItemSelCreate((u32)entry, item->colours, 0, 0, cbSelectItem, entry->name));
    choices->flags |= 1;
    eduiMenuAttach(menu, choices);
    eduiMenuFitWidth(choices, 5);
    eduiMenuFitOnScreen(choices, 30);
    reinterpret_cast<edui_prop_s *>(item)->button_held = 0;
}

void EdBitControl::cbSelectItem(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    if (edBitControl != NULL) {
        Item *choice = static_cast<Item *>(item->data_ptr);
        eduiItemPropSetText(edBitItem, edBitControl->GetEnumString(choice->value));
        u32 value;
        edBitControl->reference->GetMemberData(edBitControl->object, EdType_Int, &value, 0);
        if (choice->value != 0)
            value |= 1 << edBitIndex;
        else
            value &= ~(1 << edBitIndex);
        edBitControl->reference->SetMemberData(edBitControl->object, EdType_Int, &value, 0, NULL);
        cbEdLevelDestroyOnSelect(menu, item, flags);
    }
}

void EdBitControl::cbButton(eduimenu_s *menu, eduiitem_s *item, u32) {
    edBitItem = reinterpret_cast<edui_prop_s *>(item);
    edBitControl = static_cast<EdBitControl *>(item->data_ptr);
    edBitIndex = edBitItem->extra_data;
    eduimenu_s *choices = eduiMenuCreate(item->x + menu->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                         cbEdLevelDestroy, NULL);
    if (choices == NULL)
        return;
    for (Item *entry = edBitControl->items; entry->name != NULL; entry++)
        eduiMenuAddItem(choices, eduiItemSelCreate((u32)entry, item->colours, 0, 0, cbSelectItem, entry->name));
    choices->flags |= 1;
    eduiMenuAttach(menu, choices);
    eduiMenuFitWidth(choices, 5);
    reinterpret_cast<edui_prop_s *>(item)->button_held = 0;
}

static EdColourControl *edColourControl;

void EdColourControl::cbColourSelected(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    NUVEC colour;
    edui_colour_pick_s *picker = reinterpret_cast<edui_colour_pick_s *>(item);
    colour.x = picker->red;
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(edColourControl->item);
    colour.y = picker->green;
    colour.z = picker->blue;
    edColourControl->reference->SetMemberData(edColourControl->object, EdType_Colour3, &colour, 0, NULL);
    char value[128];
    NuSPrintf(value, "%.2f %.2f %.2f", colour.x, colour.y, colour.z);
    eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(edColourControl->item), value);
    i32 packed = (i32)(colour.z * 255.0);
    packed |= 0xffffff00;
    packed <<= 8;
    packed |= (i32)(colour.y * 255.0) & 0xff;
    packed <<= 8;
    packed |= (i32)(colour.x * 255.0) & 0xff;
    property->button_colour = packed;
    cbEdLevelDestroyOnSelect(menu, item, flags);
}

void EdColourControl::cbButton(eduimenu_s *menu, eduiitem_s *item, u32) {
    edColourControl = static_cast<EdColourControl *>(item->data_ptr);
    eduimenu_s *picker_menu = eduiMenuCreate(item->x + menu->width, item->y, 180, 250,
                                             reinterpret_cast<void *>(EdLevelFnt), cbEdLevelDestroy, NULL);
    if (picker_menu == NULL)
        return;
    NUVEC colour;
    edColourControl->reference->GetMemberData(edColourControl->object, EdType_Colour3, &colour, 0);
    eduiitem_s *picker = eduiItemColourPickCreate(0, item->colours, cbColourSelected, "Colour");
    eduiItemColourPickSetRGB(reinterpret_cast<edui_colour_pick_s *>(picker), colour.x, colour.y, colour.z);
    eduiMenuAddItem(picker_menu, picker);
    eduiMenuAttach(menu, picker_menu);
    reinterpret_cast<edui_prop_s *>(item)->button_held = 0;
}

/* Keeps the list ordered by PropertyMenu::order. */
void PropertyMenuList::InsertSorted(PropertyMenu *menu) {
    for (PropertyMenu *position = first; position != NULL; position = position->next) {
        if (menu->order < position->order) {
            InsertBefore(menu, position);
            return;
        }
    }
    InsertAfter(menu, NULL);
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

/* A NULL position appends the menu. */
void PropertyMenuList::InsertAfter(PropertyMenu *menu, PropertyMenu *position) {
    if (position != NULL) {
        menu->previous = position;
        menu->next = position->next;
        if (position->next != NULL)
            position->next->previous = menu;
        else
            last = menu;
        position->next = menu;
        count++;
    } else {
        menu->next = NULL;
        menu->previous = last;
        if (last != NULL)
            last->next = menu;
        last = menu;
        if (first == NULL)
            first = menu;
        count++;
    }
}

void PropertyMenuList::Remove(PropertyMenu *menu) {
    if (menu->next != NULL)
        menu->next->previous = menu->previous;
    else
        last = menu->previous;
    if (menu->previous != NULL)
        menu->previous->next = menu->next;
    else
        first = menu->next;
    menu->next = NULL;
    menu->previous = NULL;
    count--;
}

PropertyMenu *PropertyTool::FindItemMenu(PropertyMenu *menu, ClassObjectListEntry *entry) {
    for (; menu != NULL; menu = menu->next) {
        for (i32 i = 0; i < menu->object_count; i++) {
            if (entry->object == menu->objects[i].object)
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

/* Moves a menu ahead of every ordered and new (-1) menu. */
void PropertyTool::BringToFront(PropertyMenu *menu) {
    menus.Remove(menu);
    menu->order = -2;
    menus.InsertSorted(menu);
}

PropertyMenu *PropertyTool::GetActiveMenu(PropertyMenu *menu) {
    if (menu == NULL)
        return NULL;
    if (menu->menu == eduiGetActiveMenuParent())
        return menu;
    menu = menu->next;
    return GetActiveMenu(menu);
}

PropertyMenu *PropertyTool::GetNextActiveMenu() {
    PropertyMenu *next = NULL;
    PropertyMenu *menu = GetActiveMenu(menus.first);
    if (menu != NULL)
        next = menu->next;
    return next;
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
        SetDefaultActiveMenu(GetNextActiveMenu());
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
            menus.Remove(menu);
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
        menus.Remove(menu);
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

void LevelEditor::AddInfoText(char *text) {
    for (i32 i = 0; i < 32; i++) {
        if (info_text[i] == NULL) {
            i32 size = NuStrLen(text) + 1;
            char *line = text_buffer + text_length;
            NuStrCpy(line, text);
            text_length += size;
            info_text[i] = line;
            return;
        }
    }
}

void LevelEditor::SetCameraPosition(VuVec &position) {
    background_colour[0] = position.x;
    background_colour[1] = position.y;
    background_colour[2] = position.z;
    background_colour[3] = position.w;
    edcamSetPos(reinterpret_cast<NUVEC *>(background_colour));
}

/* Copies every plain member from source to destination, recursing into
 * embedded objects. */
void EdClass::CopyObject(void *destination, void *source) {
    for (EdRef *member = members; member != NULL; member = member->next) {
        if (member->attributes & 0x1800000)
            continue;
        if (member->attributes < 0) {
            EdClass *member_class = theRegistry.GetClass(member->type_id);
            void *source_member = member->GetMemberObject(source);
            void *destination_member = member->GetMemberObject(destination);
            member_class->CopyObject(destination_member, source_member);
        } else {
            u32 data[64];
            member->GetMemberData(source, member->type_id, data, sizeof(data));
            member->SetMemberData(destination, member->type_id, data, sizeof(data), NULL);
        }
    }
}

/* The average position of up to 64 objects and the radius of the sphere
 * around it that holds every object's own sphere. */
i32 ClassObjectList::GetAveragePosition(VuVec &average, f32 &radius) {
    __declspec(align(16)) VuVec positions[64];
    __declspec(align(16)) f32 radii[64];
    i32 count = 0;
    average.x = 0.0f;
    average.y = 0.0f;
    average.z = 0.0f;
    average.w = 1.0f;
    for (ClassObjectListEntry *entry = first; entry != NULL && count < 64; entry = entry->next) {
        ClassObject &object = *reinterpret_cast<ClassObject *>(&entry->ed_class);
        if (object.GetAttributeData(8, EdType_VuVec, &positions[count], 0)) {
            average.x += positions[count].x;
            average.y += positions[count].y;
            average.z += positions[count].z;
            radii[count] = 1.0f;
            object.GetAttributeData(0x40, EdType_Float, &radii[count], 0);
            count++;
        }
    }
    if (count > 0) {
        f32 scale = 1.0f / count;
        average.x *= scale;
        average.y *= scale;
        average.z *= scale;
        radius = 0.0f;
        for (i32 i = 0; i < count; i++) {
            NUVEC difference;
            difference.x = average.x - positions[i].x;
            difference.y = average.y - positions[i].y;
            difference.z = average.z - positions[i].z;
            f32 extent =
                NuFsqrt(difference.x * difference.x + difference.y * difference.y + difference.z * difference.z) +
                radii[i];
            if (radius < extent)
                radius = extent;
        }
    }
    return count;
}

void ClassEditor::SetMode(i32 new_mode) {
    mode = new_mode;
    switch (new_mode) {
        case 0:
        case 1:
        case 2:
            manipulator = &theDefaultManipulator;
            break;
        case 3:
            manipulator = &theMoveManipulator;
            break;
        case 4:
            manipulator = &theRotateManipulator;
            break;
        case 5:
            manipulator = &theScaleManipulator;
            break;
    }
    thePropertyTool.SelectAttr(manipulator->selected_attribute);
}

void ClassEditor::cbEdClassSetMode(eduimenu_s *menu, eduiitem_s *item, u32) {
    theClassEditor.SetMode(item->data);
    eduiMenuHighlight(menu, item);
}

void ClassEditor::cbEdClassModeMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(0, &EdLevelAttr, theClassEditor.mode == 0, 1, cbEdClassSetMode, "Select"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(1, &EdLevelAttr, theClassEditor.mode == 1, 1, cbEdClassSetMode, "Create"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(2, &EdLevelAttr, theClassEditor.mode == 2, 1, cbEdClassSetMode, "Delete"));
    eduiMenuAddItem(menu, eduiItemToggleCreate(3, &EdLevelAttr, theClassEditor.mode == 3, 1, cbEdClassSetMode, "Move"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(4, &EdLevelAttr, theClassEditor.mode == 4, 1, cbEdClassSetMode, "Rotate"));
    eduiMenuAddItem(menu,
                    eduiItemToggleCreate(5, &EdLevelAttr, theClassEditor.mode == 5, 1, cbEdClassSetMode, "Scale"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

/* Asks which LED file a new object goes in; cbFileSelected then creates it. */
void ClassEditor::SelectLED(i32 class_id) {
    f32 x;
    f32 y;
    eduiGetCursorCoords(&x, &y);
    x *= 640.0;
    y *= 448.0;
    class_editor_create_pending = class_id;
    eduimenu_s *menu = eduiMenuCreate(static_cast<i32>(x), static_cast<i32>(y), 300, 250,
                                      reinterpret_cast<void *>(EdLevelFnt), cbEdLevelDestroy, "Select LED File");
    if (menu == NULL)
        return;
    for (i32 index = 0; index < theLevelEditor.reset_pending; index++) {
        LevelEditorScene *scene = theLevelEditor.GetEdScene(index);
        if (scene != NULL && scene->editable)
            eduiMenuAddItem(menu, eduiItemSelCreate(index, &EdLevelAttr, 0, 0, cbFileSelected, scene->name));
    }
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    edLevelNextMenu = menu;
}

/* Gives a new object a unique name and puts it at the manipulator cursor. */
void ClassEditor::InitialiseObject(ClassObject &object) {
    __declspec(align(16)) EdMember member;
    char name[128];
    char unique_name[128];
    if (object.ed_class->FindMember(&member, object.object, 2, 1)) {
        member.reference->GetAttributeData(member.object, 2, EdType_String, name, sizeof(name));
        MakeUniqueName(name, unique_name, sizeof(unique_name));
        member.reference->SetAttributeData(member.object, 2, EdType_String, unique_name, 0);
    }
    if (object.ed_class->FindMember(&member, object.object, 8, 1))
        member.reference->SetAttributeData(member.object, 8, EdType_VuVec, &manipulator->cursor, 0);
}

i32 ClassEditor::CreateObject(EdClass *object_class) {
    if (object_class == NULL || object_class->object_interface == NULL)
        return 0;
    void *object = theRegistry.CreateObject(object_class->object_interface, NULL, 4, 0, 2);
    if (object == NULL)
        return 0;
    ClassObject created(object_class, object);
    InitialiseObject(created);
    theRegistry.NotifyCreateObject(created.object, created.ed_class, NULL, 0, 0, 0);
    SelectObject(created, 0);
    return 1;
}

/* Clones a selected object of the class if there is one, otherwise creates
 * a fresh object (singleton classes can only be cloned). */
i32 ClassEditor::cbEdCreateClassNewObject(i32 class_id) {
    EdClass *object_class = theRegistry.GetClass(class_id);
    i32 created = 0;
    for (ClassObjectListEntry *entry = theClassEditor.selected_objects.first; entry != NULL; entry = entry->next) {
        if (entry->ed_class == object_class)
            return theClassEditor.CreateObject(*reinterpret_cast<ClassObject *>(&entry->ed_class));
    }
    if (!(object_class->flags & 0x4000000))
        created = theClassEditor.CreateObject(class_id);
    return created;
}

/* Retail calls this out of line from both callers (0x6cf88b, 0x6d25fa). */
__declspec(noinline) i32 ClassEditor::CreateObject(i32 class_id) {
    if (theLevelEditor.current_led_file == -1) {
        SelectLED(class_id);
        return 1;
    }
    Placeable::CurrentLedFile = theLevelEditor.current_led_file;
    EdClass *object_class = theRegistry.GetClass(class_id);
    if (object_class == NULL || (object_class->flags & 0x4000000))
        return 0;
    return CreateObject(object_class);
}

/* Creates a copy of source (or, for a singleton class, re-creates it). */
i32 ClassEditor::CreateObject(ClassObject &source) {
    ClassObject created(source.ed_class);
    if (theLevelEditor.current_led_file == -1) {
        SelectLED(-1);
        return 1;
    }
    Placeable::CurrentLedFile = theLevelEditor.current_led_file;
    EdClassInterface *object_interface = source.ed_class->object_interface;
    if (source.ed_class->flags & 0x4000000)
        created.object = theRegistry.CreateObject(object_interface, source.object, 4, 0, 2);
    else
        created.object = theRegistry.CreateObject(object_interface, NULL, 4, 0, 2);
    if (created.object == NULL)
        return 0;
    if (source.ed_class->flags & 0x4000000) {
        i32 flags = 0x4000000;
        created.SetAttributeData(1, EdType_Int, &flags, 0);
    } else {
        created.ed_class->CopyObject(created.object, source.object);
        created.ed_class->object_interface->Construct(created.object, source.object);
    }
    InitialiseObject(created);
    theRegistry.NotifyCreateObject(created.object, created.ed_class, NULL, 0, 0, 0);
    SelectObject(created, 0);
    return 1;
}

void ClassEditor::cbFileSelected(eduimenu_s *, eduiitem_s *item, u32) {
    if (item == NULL)
        return;
    theLevelEditor.current_led_file = static_cast<i16>(item->data);
    if (class_editor_create_pending == -1) {
        if (theClassEditor.selected_objects.first != NULL)
            theClassEditor.CreateObject(
                *reinterpret_cast<ClassObject *>(&theClassEditor.selected_objects.first->ed_class));
    } else {
        theClassEditor.CreateObject(class_editor_create_pending);
    }
    eduiSetCameraEnabled(1);
    if (edLevelActiveMenu != NULL) {
        if (edLevelActiveMenu->parent != NULL)
            edLevelActiveMenu->parent->child = NULL;
        if (edLevelActiveMenu->child != NULL)
            edLevelActiveMenu->child->parent = NULL;
    }
    edLevelDestroyActiveMenu = 1;
}

void ClassEditor::cbEdClassFileMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    for (i32 index = 0; index < 10; index++) {
        LevelEditorScene *scene = theLevelEditor.GetEdScene(index);
        if (scene != NULL && scene->active)
            eduiMenuAddItem(
                menu, eduiItemToggleCreate(index << 6, &EdLevelAttr, scene->editable, 1, cbEdFilterLED, scene->name));
    }
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelSave, "Save"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void ClassEditor::cbEdClassNewObject(eduimenu_s *parent, eduiitem_s *item, u32) {
    EdClass *object_class = theRegistry.GetClass(item->data);
    if (cbEdCreateClassNewObject(item->data)) {
        eduiSetCameraEnabled(1);
        if (edLevelActiveMenu != NULL) {
            if (edLevelActiveMenu->parent != NULL)
                edLevelActiveMenu->parent->child = NULL;
            if (edLevelActiveMenu->child != NULL)
                edLevelActiveMenu->child->parent = NULL;
        }
        edLevelDestroyActiveMenu = 1;
        return;
    }
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, "Error!");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "Failed to create new object"));
    if (object_class == NULL)
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "Unknown class id"));
    else if (object_class->flags & 0x4000000)
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "No selected objects to clone"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void ClassEditor::cbEdClassNewMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    for (i32 index = 0; index < theRegistry.class_count; index++) {
        if (!(theRegistry.classes[index].flags & 0x20000000))
            eduiMenuAddItem(menu, eduiItemSelCreate(index, &EdLevelAttr, 0, 0, cbEdClassNewObject,
                                                    theRegistry.classes[index].name));
    }
    if (menu->first == NULL)
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "No Registered Classes"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

i32 ClassEditor::cbEdCopySelectedObject(EdInputContext &) {
    if (theClassEditor.selected_objects.first != NULL)
        return theClassEditor.CreateObject(
            *reinterpret_cast<ClassObject *>(&theClassEditor.selected_objects.first->ed_class));
    return 0;
}

/* Moves the camera to the selection and backs off until it all fits. */
void ClassEditor::FocusSelected() {
    __declspec(align(16)) VuVec position;
    f32 radius;
    if (selected_objects.GetAveragePosition(position, radius)) {
        theLevelEditor.SetCameraPosition(position);
        NUCAMERA *camera = edmainGetCameraPc();
        f32 half_fov = camera->fov * 0.5;
        i32 angle = static_cast<i32>(half_fov * 10430.3779296875);
        radius = radius * static_cast<f64>(1.2f) *
                 (NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] / NuTrigTable[angle >> 1 & 0x7fff]);
        edcamSetDist(-radius);
    }
}

void ClassEditor::ViewSelected() {
    __declspec(align(16)) VuVec position;
    if (selected_objects.GetAveragePosition(position))
        theLevelEditor.SetCameraPosition(position);
}

void ClassEditor::cbEdClassSelectObject(eduimenu_s *, eduiitem_s *item, u32) {
    theClassEditor.pending_object.object = item->data_ptr;
    if (theClassEditor.pending_object.object == NULL)
        return;
    theClassEditor.SelectObject(theClassEditor.pending_object, 0);
    theClassEditor.FocusSelected();
    eduiSetCameraEnabled(1);
    if (edLevelActiveMenu != NULL) {
        if (edLevelActiveMenu->parent != NULL)
            edLevelActiveMenu->parent->child = NULL;
        if (edLevelActiveMenu->child != NULL)
            edLevelActiveMenu->child->parent = NULL;
    }
    edLevelDestroyActiveMenu = 1;
}

void ClassEditor::cbEdClassSelectObjectMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    char name[128];
    EdClass *object_class = theRegistry.GetClass(item->data);
    theClassEditor.pending_object.ed_class = object_class;
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    EdRef *name_reference = object_class->FindTypeRef(2, 1);
    for (void *object = object_class->object_interface->GetNextObject(NULL); object != NULL;
         object = object_class->object_interface->GetNextObject(object)) {
        if (name_reference == NULL || !name_reference->GetAttributeData(object, 2, EdType_String, name, sizeof(name)))
            NuStrCpy(name, object_class->name);
        if (theClassEditor.Editable(object, object_class, -1))
            eduiMenuAddItem(menu, eduiItemSelCreate(reinterpret_cast<i32>(object), &EdLevelAttr, 0, 0,
                                                    cbEdClassSelectObject, name));
    }
    if (menu->first == NULL)
        eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "No Object"));
    else
        eduiMenuAddItemFirst(menu, eduiItemFilterCreate(0, &EdLevelAttr, "FILTER", ""));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
    eduiMenuSortItemsByTxt(menu);
}

void ClassEditor::cbEdClassSelectClassMenu(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu = eduiMenuCreate(item->x + parent->width, item->y, 180, 250, reinterpret_cast<void *>(EdLevelFnt),
                                      cbEdLevelDestroy, NULL);
    if (menu == NULL)
        return;
    for (i32 index = 0; index < 10; index++) {
        LevelEditorScene *scene = theLevelEditor.GetEdScene(index);
        if (scene != NULL && scene->active)
            eduiMenuAddItem(
                menu, eduiItemToggleCreate(index << 6, &EdLevelAttr, scene->editable, 1, cbEdFilterLED, scene->name));
    }
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    for (i32 index = 0; index < theRegistry.class_count; index++) {
        EdClass *object_class = &theRegistry.classes[index];
        if (!(object_class->flags & 0x20000000) && object_class->object_interface != NULL)
            eduiMenuAddItem(
                menu, eduiItemSelCreate(index, &EdLevelAttr, 0, 0, cbEdClassSelectObjectMenu, object_class->name));
    }
    if (menu->first == NULL)
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdLevelDestroyOnSelect, "No Registered Classes"));
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
    eduiMenuSortItemsByTxt(menu);
}

/* The editable object nearest along the ray; with filter set, only one the
 * ray actually hits. */
i32 ClassEditor::FindNearestObject(VuVec &origin, VuVec &direction, ClassObject &nearest, i32 filter) {
    ClassObject best;
    f32 best_distance = 3.402823466e38f;
    for (i32 class_index = 0; class_index < theRegistry.class_count; class_index++) {
        EdClass *object_class = &theRegistry.classes[class_index];
        if (!(class_filter & 1 << class_index) || object_class->object_interface == NULL || !(object_class->flags & 8))
            continue;
        for (void *object = object_class->object_interface->GetNextObject(NULL); object != NULL;
             object = object_class->object_interface->GetNextObject(object)) {
            if (!Editable(object, object_class, -1))
                continue;
            __declspec(align(16)) EdRef *reference;
            f32 distance = object_class->object_interface->DistanceToObject(origin, direction, object, &reference);
            if (distance < best_distance) {
                best_distance = distance;
                best.ed_class = object_class;
                best.object = object;
                best.reference = reference;
            }
        }
    }
    if (filter != 0 && best_distance > 0.0f) {
        best.ed_class = NULL;
        best.object = NULL;
    }
    nearest = best;
    return nearest.object != NULL;
}

/* Like the above, but cycles through every object the ray hits: returns the
 * one after `after`, or the first. */
i32 ClassEditor::FindNearestObject(VuVec &origin, VuVec &direction, ClassObject &nearest, ClassObject &after,
                                   i32 filter) {
    __declspec(align(16)) ClassObject candidates[16];
    i32 count = 0;
    for (i32 class_index = 0; class_index < theRegistry.class_count; class_index++) {
        EdClass *object_class = &theRegistry.classes[class_index];
        if (!(class_filter & 1 << class_index) || object_class->object_interface == NULL || !(object_class->flags & 8))
            continue;
        for (void *object = object_class->object_interface->GetNextObject(NULL); object != NULL;
             object = object_class->object_interface->GetNextObject(object)) {
            if (!Editable(object, object_class, -1))
                continue;
            EdRef *reference;
            f32 distance = object_class->object_interface->DistanceToObject(origin, direction, object, &reference);
            if (distance < 0.01f && count < 16) {
                candidates[count].ed_class = object_class;
                candidates[count].object = object;
                candidates[count].reference = reference;
                count++;
            }
        }
    }
    if (count == 0)
        return 0;
    for (i32 i = 0; i <= count; i++) {
        if (candidates[i].object == after.object) {
            if (i < count - 1) {
                nearest = candidates[i + 1];
                return 1;
            }
            break;
        }
    }
    nearest = candidates[0];
    return 1;
}

/* Drops selected objects that no longer exist and steps the selection
 * forwards or backwards through the class's objects. */
void ClassEditor::UpdateSelectedObjects(EdInputContext &input) {
    ClassObjectListEntry *entry;
    ClassObjectListEntry *next;
    for (entry = selected_objects.first; entry != NULL; entry = next) {
        EdClassInterface *object_interface = entry->ed_class->object_interface;
        void *selected = entry->object;
        next = entry->next;
        void *object = object_interface->GetNextObject(NULL);
        while (object != NULL && object != selected)
            object = object_interface->GetNextObject(object);
        if (object != NULL)
            continue;
        if (entry->next != NULL)
            entry->next->previous = entry->previous;
        else
            selected_objects.last = entry->previous;
        if (entry->previous != NULL)
            entry->previous->next = entry->next;
        else
            selected_objects.first = entry->next;
        entry->next = NULL;
        entry->previous = NULL;
        selected_objects.count--;
        theMemoryManager.FreePool(entry, sizeof(ClassObjectListEntry));
    }
    entry = selected_objects.first;
    if (entry == NULL)
        return;
    void *object = NULL;
    if (input.GetPress(19) != 0.0f) {
        object = entry->object;
        i32 steps = 0;
        do {
            steps++;
            object = entry->ed_class->object_interface->GetNextObject(object);
            if (object == NULL)
                object = entry->ed_class->object_interface->GetNextObject(NULL);
        } while (!Editable(object, entry->ed_class, -1) && steps < 0x1000);
    }
    if (input.GetPress(20) != 0.0f) {
        void *current = entry->object;
        void *previous = NULL;
        i32 steps = 0;
        do {
            if (Editable(current, entry->ed_class, -1))
                previous = current;
            current = entry->ed_class->object_interface->GetNextObject(current);
            if (current == NULL)
                current = entry->ed_class->object_interface->GetNextObject(NULL);
            steps++;
        } while (current != entry->object && steps < 0x1000);
        if (previous == NULL)
            return;
        object = previous;
    }
    if (object != NULL) {
        ClassObject selected(entry->ed_class, object);
        theClassEditor.SelectObject(selected, 0);
        theClassEditor.ViewSelected();
    }
}

/* Picks objects under the cursor ray, creates copies and deletes. */
i32 CursorTool::Process(EdInputContext &input) {
    char name[128];
    char text[128];
    ClassObject hover;
    i32 extend = static_cast<i32>(input.GetHold(16)) != 0;
    __declspec(align(16)) VuVec end;
    end.x = input.ray_direction.x + input.ray_start.x;
    end.y = input.ray_direction.y + input.ray_start.y;
    end.z = input.ray_direction.z + input.ray_start.z;
    i32 colour = NuRandInt();
    EdDrawBegin(0);
    EdDrawLineSegment(&input.ray_start.xyz, &end.xyz, colour);
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
    if (theLevelEditor.field_0x30 == 0)
        return 0;
    ClassObject after;
    if (theClassEditor.selected_objects.count > 0)
        after = *reinterpret_cast<ClassObject *>(&theClassEditor.selected_objects.first->ed_class);
    if (theClassEditor.FindNearestObject(input.ray_start, input.ray_direction, hover, 1)) {
        theClassEditor.current_object = hover;
        if (input.GetPress(3) != 0.0f) {
            theClassEditor.FindNearestObject(input.ray_start, input.ray_direction, hover, after, 1);
            theClassEditor.SelectObject(hover, extend);
        }
        if (hover.GetAttributeData(2, EdType_String, name, sizeof(name))) {
            sprintf(text, "object : %s", name);
            theLevelEditor.AddInfoText(text);
        } else {
            theLevelEditor.AddInfoText("object : no name");
        }
    } else {
        hover.ed_class = NULL;
        hover.object = NULL;
        theClassEditor.current_object = hover;
        if (input.GetPress(11) != 0.0f && after.object != NULL)
            theClassEditor.CreateObject(after);
    }
    if (input.GetPress(12) != 0.0f)
        theClassEditor.DestroySelectedObjects();
    if (hover.object == NULL && input.GetPress(3) != 0.0f) {
        hover.ed_class = NULL;
        hover.object = NULL;
        theClassEditor.SelectObject(hover, 0);
    }
    return 0;
}

void CursorTool::Render() {
    thePropertyTool.Render();
}

ClassEditor::~ClassEditor() {
}

void ClassEditor::Initialise(variptr_u &first, variptr_u &second, i32) {
    theRegistry.Initialise(first, second, 32, 32, 10, 1);
    theRegistry.RegisterBaseTypes();
    theCursorTool.next = NULL;
    theCursorTool.previous = last_tool;
    if (last_tool != NULL)
        last_tool->next = &theCursorTool;
    last_tool = &theCursorTool;
    if (first_tool == NULL)
        first_tool = &theCursorTool;
    tool_count++;
    theSplineTool.next = NULL;
    theSplineTool.previous = last_tool;
    if (last_tool != NULL)
        last_tool->next = &theSplineTool;
    last_tool = &theSplineTool;
    if (first_tool == NULL)
        first_tool = &theSplineTool;
    tool_count++;
    active_tool = &theCursorTool;
}

void ClassEditor::AddMenuItems(eduimenu_s *menu) {
    EdClass *classes[32];
    i32 class_count;
    i32 index;
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassFileMenu, "File"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassToolsMenu, "Tools"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassSnapMenu, "Snap"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassModeMenu, "Mode"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassViewMenu, "Show/Hide"));
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassSelectClassMenu, "Select"));
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    if (selected_objects.count == 0) {
        eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassNewMenu, "New"));
        return;
    }
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassNewMenu, "New/Copy"));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 0, cbEdClassDeleteObject, "Delete"));
    class_count = 0;
    for (ClassObjectListEntry *entry = selected_objects.first; entry != NULL; entry = entry->next) {
        for (index = 0; index < class_count; index++) {
            if (classes[index] == entry->ed_class)
                break;
        }
        if (index == class_count)
            classes[class_count++] = entry->ed_class;
    }
    for (index = 0; index < class_count; index++)
        classes[index]->object_interface->AddMenuItems(menu);
}

void ClassEditor::Process(EdInputContext &input) {
    if (snap_mode == 1) {
        snap_ray.x = 0.0f;
        snap_ray.y = -1000.0f;
        snap_ray.z = 0.0f;
    }
    if (snap_mode == 2) {
        snap_ray.x = input.ray_direction.x;
        snap_ray.y = input.ray_direction.y;
        snap_ray.z = input.ray_direction.z;
        snap_ray.w = input.ray_direction.w;
    }
    UpdateSelectedObjects(input);
    UpdateClassFilter(input);
    if (menu != NULL) {
        theLevelEditor.field_0x30 = 0;
        theLevelEditor.field_0x38 = 0;
        NuFntSet(EdLevelFnt);
        NuFntScale(EdLevelFntScale, EdLevelFntScale);
        eduiMenuProcess(menu, input.delta_time, input.pad);
        if (menu != NULL)
            return;
    }
    if (input.GetHold(13) != 0.0f)
        return;
    theLevelEditor.field_0x30 = 1;
    theLevelEditor.field_0x38 = 1;
    field_3c = 0xff808080;
    i32 handled = thePropertyTool.Process(input);
    if (!handled) {
        if (input.GetPress(5) != 0.0f)
            SetMode(0);
        if (input.GetPress(6) != 0.0f)
            SetMode(3);
        if (input.GetPress(7) != 0.0f)
            SetMode(4);
        if (input.GetPress(8) != 0.0f)
            SetMode(5);
        if (input.GetPress(37) != 0.0f) {
            ClassObject nearest;
            if (FindNearestObject(input.ray_start, input.ray_direction, nearest, 1))
                DestroySelectedObjects();
            else
                cbEdCopySelectedObject(input);
        }
        if (manipulator == NULL)
            manipulator = &theDefaultManipulator;
        handled = manipulator->Process(input, selected_objects);
    }
    if (active_tool != NULL && !handled)
        active_tool->Process(input);
    if (selected_objects.count <= 0)
        return;
    if (input.GetPress(9) != 0.0f)
        FocusSelected();
    if (input.GetPress(10) != 0.0f)
        ViewSelected();
    for (ClassObjectListEntry *entry = selected_objects.first; entry != NULL; entry = entry->next)
        theRegistry.ClassIFaceProcess(entry->ed_class, entry->object, input);
    if (input.GetPress(39) != 0.0f) {
        __declspec(align(16)) VuVec position;
        reinterpret_cast<ClassObject *>(&selected_objects.first->ed_class)
            ->GetAttributeData(8, EdType_VuVec, &position, 0);
        theLevelEditor.SetCameraPosition(position);
        thePropertyTool.Process(input);
        if (mode == 0)
            mode = 3;
        SetMode(mode);
        eduiSetActiveMenu(edLevelActiveMenu != NULL ? edLevelActiveMenu : edLevelPinnedMenu);
    }
}
