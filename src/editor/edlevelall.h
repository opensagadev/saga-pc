#ifndef SAGA_EDITOR_EDLEVELALL_H_PART
#define SAGA_EDITOR_EDLEVELALL_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/EdObjectNotifier.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_EDITOR_EDLEVELALL_H_PART == 1 && !defined(SAGA_EDITOR_EDLEVELALL_H_STOP)

struct FRAME_PAD_PC;
struct eduiiattr_s;
struct eduiitem_s;
struct eduimenu_s;
struct nucamera_s;
struct numtx_s;

#ifdef __cplusplus
extern "C" {
#endif
    void EdDrawBegin(i32 index);
    void EdDrawEnd(void);
    void EdDrawPolyCylinder(struct numtx_s *matrix, f32 half_height, f32 radius, f32 top_radius, i32 segments,
                            i32 colour, i32 cap_bottom, i32 cap_top);
#ifdef __cplusplus
}
#endif
typedef struct VuVec VuVec;

struct VuVec {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
        };

        NUVEC xyz;
    };

    f32 w;
#ifdef __cplusplus
    f32 Length();
    void fun_006b83b0();
    void fun_006b83c0();
    void fun_006b83d0();
    void fun_006b83e0();
    VuVec fun_006b8400(f32 value) const;
    f32 fun_006b8440(const VuVec &other) const;
#endif
};

STATIC_ASSERT(sizeof(struct VuVec) == 16);

#ifdef __cplusplus
struct DATAPTR;
struct EdClass;
struct EdInputContext;
struct EdMember;
class EdRef;
class EdStream;
class EdFileOutputStream;
class EdFileInputStream;
struct MemoryBuffer;

void EdTrace(...);
struct EdDefunctListEntry;
extern i32 EdType_Char;
extern i32 EdType_Short;
extern i32 EdType_Int;
extern i32 EdType_Float;
extern i32 EdType_VuVec;
extern i32 EdType_VuMtx;
extern i32 EdType_NuVec;
extern i32 EdType_NuMtx;

struct EdManipulator;
#endif
#undef SAGA_EDITOR_EDLEVELALL_H_PART
#define SAGA_EDITOR_EDLEVELALL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDLEVELALL_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDLEVELALL_H_PART == 2 && !defined(SAGA_EDITOR_EDLEVELALL_H_STOP)
#ifdef __cplusplus

/* Editor object-class interface (Android gameapi_edtools_types.h). MSVC lays
 * overloaded virtuals out in reverse declaration order, so the filtered
 * GetNextObject sits in slot 4 and the two-vector DistanceToObject in slot 27. */
class EdClassInterface {
  public:
    EdClassInterface() : object_class(NULL) {
    }

    virtual ~EdClassInterface() {
    }

    virtual void ClearLevel(i32) {
    }

    virtual void Flush() = 0;
    virtual i32 GetNumObjects() = 0;
    virtual void *GetNextObject(void *object) = 0;
    virtual void *GetNextObject(void *object, i32 (*filter)(void *));
    virtual void *CreateObject(void *data, i32 guid, i32 flags) = 0;
    virtual void DestroyObject(void *object, i32 flags) = 0;

    virtual void DefunctObject(void *) {
    }

    virtual void ReviveObject(void *) {
    }

    virtual void SetObjectGuid(void *, i32) {
    }

    virtual i32 GetObjectGuid(void *) {
        return 0;
    }

    virtual i32 GetConstructorData(void *, void *, i32) {
        return 0;
    }

    virtual void Construct(void *, void *) {
    }

    virtual void Process(void *, EdInputContext &) {
    }

    virtual void Render(void *, i32) {
    }

    virtual void EnterEditor() {
    }

    virtual void ExitEditor() {
    }

    virtual void EnterLevel() {
    }

    virtual void ExitLevel() {
    }

    virtual void UpdateLists(MemoryBuffer *, MemoryBuffer *) {
    }

    virtual void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
    }

    virtual void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
    }

    virtual void PreSaveInitialisation() {
    }

    virtual void PostSaveInitialisation() {
    }

    virtual void SerialiseObject(EdStream &, void *) {
    }

    virtual f32 DistanceToObject(VuVec &start, VuVec &end, void *object, EdRef **reference);
    virtual f32 DistanceToObject(VuVec &position, void *object, EdRef **reference);

    virtual void AddMenuItems(struct eduimenu_s *) {
    }

    virtual void Import() {
    }

    EdClass *object_class;
};

STATIC_ASSERT(sizeof(EdClassInterface) == 0x8);
STATIC_ASSERT(offsetof(EdClassInterface, object_class) == 0x4);

/* Intrusive list link (Android CSListLink<T>); non-polymorphic, so it sits
 * after the vtable pointer of the classes that derive from it. */
template <class T> class CSListLink {
  public:
    /* Retail's standalone copies of this constructor (0x6c1260, 0x6c6400, ...)
     * test this against NULL before clearing the links. */
    CSListLink() {
        CSListLink *link = this;
        if (link != NULL) {
            link->next = NULL;
            link->previous = NULL;
        }
    }

    T *next;
    T *previous;
};

/* Property-menu control bound to one EdRef of an edited object. Controls live
 * in theMemoryManager's size-class pools. */
class EdControl {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdControl));
    }

    EdControl() {
    }

    virtual ~EdControl() {
    }

    virtual void Refresh();
    virtual void Process(EdInputContext &input);
    virtual void Render();
    virtual void AddMenuItem(struct eduimenu_s *menu, EdRef *reference, void *object);
    virtual void SetMenuItemAttr(i32 state, struct eduiitem_s *item, struct eduiiattr_s *normal,
                                 struct eduiiattr_s *highlighted);
    i32 SelectSubObject();
    static void cbSelected(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    struct eduiitem_s *item;
    EdRef *reference;
    void *object;
    static EdInputContext *Input;
};

class EdEnumControl : public EdControl {
  public:
    struct Item {
        char *name;
        i32 value;
    };

    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdEnumControl));
    }

    EdEnumControl(Item *items) : items(items) {
    }

    virtual void AddMenuItem(struct eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    char *GetEnumString(i32 value);
    i32 fun_006ba010(char *name);
    static void cbChanged(struct eduimenu_s *, struct eduiitem_s *item, u32);
    static void cbButton(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static void cbSelectItem(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static Item YesNoItems[];
    Item *items;
};

class EdBitControl : public EdEnumControl {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdBitControl));
    }

    EdBitControl(u32 bit_mask, Item *items) : EdEnumControl(items), bit_mask(bit_mask) {
    }

    virtual void AddMenuItem(struct eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    static void cbChanged(struct eduimenu_s *, struct eduiitem_s *item, u32);
    static void cbButton(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static void cbSelectItem(struct eduimenu_s *menu, struct eduiitem_s *item, u32);

    u32 bit_mask;
};

/* Editor string: data[0] holds the pool block size, the text starts at
 * data + 1. */
struct EdString {
    char *data;

    EdString() : data(NULL) {
    }

    void Set(const char *value);
    ~EdString();
    i32 fun_006b85c0(i32 size);
    char *fun_006b86a0();
};

/* Editor serialisation stream (Android EdStream); mode is 1 reading, 2 writing. */
class EdStream {
  public:
    EdStream();

    EdStream(MemoryBuffer *buffer, MemoryBuffer *secondary) {
        memory_buffer = buffer;
        secondary_buffer = secondary;
        mode = 0;
        swap_endianness = 0;
        flags = 0;
    }

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
    i32 fun_00613030(u8 &value);
    i32 fun_00613050(i32 &value);

    EdStream(MemoryBuffer *buffer);
    i32 fun_00611910();
    i32 fun_006119a0(i32 mask);
    void fun_006119b0(MemoryBuffer *buffer);
    i32 fun_006c1e00(i32 &value);
    i32 fun_006c1e20(u8 &value);

    i32 version;
    i32 mode;
    i32 swap_endianness;
    i32 unknown_10;
    MemoryBuffer *memory_buffer;
    MemoryBuffer *secondary_buffer;
    i32 flags;
};

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

/* Reads strings back; SerialiseString(char **) allocates from memory_buffer. */
class EdInputStream : public EdStream {
  public:
    EdInputStream() {
        mode = 1;
    }

    EdInputStream(MemoryBuffer *buffer, MemoryBuffer *secondary);

    virtual ~EdInputStream() {
    }

    virtual i32 SerialiseString(char *text, i32 count);
    virtual i32 SerialiseString(char **text);
    virtual i32 SerialiseString(char **text, i32 count);
};

/* A registered editor value type (Android EdType). */
struct EdType {
    char *name;
    i32 size;
    void (*serialise)(EdStream &, void *, i32);

    void Serialise(EdStream &stream);
};

/* A registered editor class: its member list and object interface. */
struct EdClass {
    char *name;
    i32 flags;
    EdRef *members;
    EdRef *last_member;
    i32 member_count;
    EdClassInterface *object_interface; /* Android "interface" (an objbase.h macro) */

    i32 FindMember(EdMember *result, void *object, i32 attributes, i32 recursive);
    i32 GetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SerialiseObjectHeader(EdStream &stream, void *object);
    void CopyObject(void *destination, void *source);
    void AddType(EdRef *member);
    EdRef *FindTypeRef(char *member_name, i32 recursive);
    EdRef *FindTypeRef(i32 attributes, i32 recursive);
    void *FindObject(char *object_name);
    i32 GetStreamClasses(EdStream &stream, i32 *stream_classes, i32 &count, i32 capacity);
    void Serialise(EdStream &stream, i32 *class_mapping);
    void SerialiseObject(EdStream &stream, void *object);
    void SerialiseObject(EdStream &stream, void *object, EdClass *schema, struct EdRegistry *registry);
};

struct EdDefunctList {
    EdDefunctList() : first(NULL), last(NULL), count(0) {
    }

    void ReviveAll(i32 flags);
    EdDefunctListEntry *first;
    EdDefunctListEntry *last;
    i32 count;
};

/* One member of an editor class: where it sits in the object, its type and
 * the control that edits it. Attribute bit 30 marks a pointer member. */
class EdRef : public CSListLink<EdRef> {
  public:
    EdRef(char *type_name, char *member_name, i32 offset, i32 member_size, i32 member_attributes,
          EdControl *member_control, i32 group);

    EdRef() {
    }

    virtual void *GetMemberObject(void *object);
    virtual void GetMemberData(void *object, i32 requested_type, void *data, i32 data_size);
    virtual void SetMemberData(void *object, i32 requested_type, void *data, i32 data_size, i16 *);

    i32 type_id;
    char *name;
    i32 member_offset;
    i32 size;
    i32 attributes;
    EdControl *control;
    i32 replication_group;

    i32 GetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    void CheckType(i32 requested_type);
    i32 GetTypeSize(i32 requested_type, i32 data_size);
    void Serialise(EdStream &stream, i32 *class_mapping);
};

/* A member reference bound to the object (or sub-object) that holds it. */
struct EdMember {
    void *object;
    EdRef *reference;

    i32 fun_006c1f60(i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 fun_006c1f90(i32 attribute, i32 requested_type, void *data, i32 data_size);
};

/* An editor object together with its class and the member it was picked by. */
struct ClassObject {
    EdClass *ed_class;
    void *object;
    EdRef *reference;

    ClassObject(EdClass *object_class = NULL, void *object_data = NULL)
        : ed_class(object_class), object(object_data), reference(NULL) {
    }

    i32 GetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size);
    void CopyObject(ClassObject &source);

    bool fun_006b89f0(ClassObject &other) {
        return object == other.object && reference == other.reference;
    }

    void Set(char *name);
    void GetName(char *destination, i32 size);
};

class EditorSettings {
  public:
    EditorSettings();
    virtual ~EditorSettings();
    void AddMenuItems(struct eduimenu_s *menu);
    void Serialise(EdStream &stream);
    f32 cursor_radius;
    i32 snap_terrain;
};
#endif
#undef SAGA_EDITOR_EDLEVELALL_H_PART
#define SAGA_EDITOR_EDLEVELALL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDLEVELALL_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDLEVELALL_H_PART == 3 && !defined(SAGA_EDITOR_EDLEVELALL_H_STOP)
#ifdef __cplusplus

/* The editor's type/class registry (Android EdRegistry), theRegistry. */
struct EdRegistry {
    struct NameMapping {
        char *source;
        char *destination;
    };

    i32 initialised;
    EdType *types;
    EdClass *classes;
    NameMapping *mappings;
    EdObjectNotifier **notifiers;
    i32 (*create_object_guid)();
    i32 type_capacity;
    i32 type_count;
    i32 class_capacity;
    i32 class_count;
    i32 mapping_capacity;
    i32 object_count;
    i32 notifier_capacity;
    i32 notifier_count;
    EdDefunctList defunct_objects;

    void Initialise(VARIPTR &buffer, VARIPTR &, i32 max_classes, i32 max_types, i32 max_mappings, i32 max_notifiers);
    void Flush();
    i32 RegisterType(char *name, i32 size, void (*serialise)(EdStream &, void *, i32));
    void RegisterBaseTypes();
#pragma push_macro("RegisterClass")
#undef RegisterClass
    EdClass *RegisterClass(char *name, EdClassInterface *object_interface, i32 flags);
#pragma pop_macro("RegisterClass")
    i32 AddMapping(char *source, char *destination);
    char *MapName(char *name);
    i32 GetTypeId(char *name);
    EdType *GetType(char *name);
    EdType *GetType(i32 index);
    i32 GetClassId(char *name);
    i32 GetClassId(EdClass *object_class);
    EdClass *GetClass(char *name);
    EdClass *GetClass(i32 index);
    void ClassIFaceProcess(i32 class_id, void *object, EdInputContext &context);
    void ClassIFaceProcess(EdClass *object_class, void *object, EdInputContext &context);
    void ClassIFaceRender(i32 class_id, void *object, i32 flags);
    void ClassIFaceRender(EdClass *object_class, void *object, i32 flags);
    void AddObjectNotifier(EdObjectNotifier *notifier);
    void NotifyCreateObject(void *object, EdClass *object_class, void *source, i32 index, i32 context, i32 flags);
    void NotifyDestroyObject(void *object, EdClass *object_class, i32 index, i32 flags);
    void NotifyDefunctObject(void *object, EdClass *object_class, i32 flags);
    void NotifyReviveObject(void *object, EdClass *object_class, i32 flags);
    void *CreateObject(EdClassInterface *object_interface, void *source, i32 index, i32 guid, i32 flags);
    void *CreateObject(EdClassInterface *object_interface, void *source, i32 flags);
    void DestroyObject(EdClassInterface *object_interface, void *object, i32 index, i32 flags);
    void DefunctObject(EdClassInterface *object_interface, void *object, i32 index, i32 flags);
    void GetStreamClassMapping(EdStream &stream, i32 *mapping, i32 &count, i32 capacity);
    void Serialise(EdStream &stream);
    void SerialiseObjects(EdStream &stream, EdRegistry *source_registry);
};

extern EdRegistry theRegistry;

extern ClassEditor theClassEditor;

/* The editor's per-frame input state (Android EdInputContext): the camera
 * view, the cursor ray and 40 logical inputs with press/release/repeat
 * edges. The PC layout has 8 more bytes at the end than Android's. */
struct EdInputContext {
    VuVec view_position;
    VuVec view_direction;
    VuVec ray_start;
    VuVec ray_direction;
    struct FRAME_PAD_PC *pad;
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
    void fun_006c1fc0(i32 input, i32 value, f32 repeat_delay);
    void Clear(i32 input);
    f32 Get(i32 input);
    f32 GetPress(i32 input);
    f32 GetRelease(i32 input);
    f32 GetHold(i32 input);
    f32 GetRepeat(i32 input);
    void Update(struct nucamera_s *camera, struct FRAME_PAD_PC *new_pad, f32 elapsed, bool menu_active);
};
#endif
#undef SAGA_EDITOR_EDLEVELALL_H_PART
#define SAGA_EDITOR_EDLEVELALL_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDLEVELALL_H_STOP
#endif
#endif

#undef SAGA_EDITOR_EDLEVELALL_H_STOP
