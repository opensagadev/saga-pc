/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_EDITOR_EDLEVELALL_H_PART
#define SAGA_EDITOR_EDLEVELALL_H_PART 1
#endif

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
    void EdDrawPolyCylinder(struct numtx_s *matrix, f32 half_height, f32 radius, f32 top_radius, i32 segments, i32 colour, i32 cap_bottom,
                            i32 cap_top);
#ifdef __cplusplus
}
#endif
typedef struct VuVec VuVec;

struct VuVec { // size 0x10

    union { // size 0xc

        struct {   // size 0xc
            f32 x; // 0x0
            f32 y; // 0x4
            f32 z; // 0x8
        };

        NUVEC xyz;
    }; // 0x0

    f32 w; // 0xc
#ifdef __cplusplus
    f32 Length();
    void fun_006b83b0();
    void fun_006b83c0();
    void fun_006b83d0();
    void fun_006b83e0();
    f32 &fun_006b83f0(i32 index);
    VuVec fun_006b8400(f32 value) const;
    f32 fun_006b8440(const VuVec &other) const;
#endif
};

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

/* Editor object-class interface. MSVC lays
 * overloaded virtuals out in reverse declaration order, so the filtered
 * GetNextObject sits in slot 4 and the two-vector DistanceToObject in slot 27. */
class EdClassInterface { // size 0x8
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

    EdClass *object_class; // 0x4
};

/* Intrusive list link; non-polymorphic, so it sits
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
class EdControl { // size 0x10
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
    virtual void SetMenuItemAttr(i32 state, struct eduiitem_s *item, struct eduiiattr_s *normal, struct eduiiattr_s *highlighted);
    i32 SelectSubObject();
    static void cbSelected(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    struct eduiitem_s *item; // 0x4
    EdRef *reference;        // 0x8
    void *object;            // 0xc
    static EdInputContext *Input;
};

class EdEnumControl : public EdControl { // size 0x14
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
    void GetVal(i32 &value);
    void SetVal(i32 &value);
    static void cbChanged(struct eduimenu_s *, struct eduiitem_s *item, u32);
    static void cbButton(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static void cbSelectItem(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static Item YesNoItems[];
    Item *items; // 0x10
};

class EdBitControl : public EdEnumControl { // size 0x18
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdBitControl));
    }

    EdBitControl(u32 bit_mask, Item *items) : EdEnumControl(items), bit_mask(bit_mask) {
    }

    void GetVal(i32 &value);
    void SetVal(i32 &value);
    virtual void AddMenuItem(struct eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    static void cbChanged(struct eduimenu_s *, struct eduiitem_s *item, u32);
    static void cbButton(struct eduimenu_s *menu, struct eduiitem_s *item, u32);
    static void cbSelectItem(struct eduimenu_s *menu, struct eduiitem_s *item, u32);

    u32 bit_mask; // 0x14
};

/* Editor string: data[0] holds the pool block size, the text starts at
 * data + 1. */
struct EdString { // size 0x4
    char *data;   // 0x0

    EdString() : data(NULL) {
    }

    void Set(const char *value);
    ~EdString();
    i32 fun_006b85c0(i32 size);
    char *fun_006b86a0();
};

/* Editor serialisation stream; mode is 1 reading, 2 writing. */
class EdStream { // size 0x20
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

    i32 version;                    // 0x04
    i32 mode;                       // 0x08
    i32 swap_endianness;            // 0x0c
    i32 unknown_10;                 // 0x10
    MemoryBuffer *memory_buffer;    // 0x14
    MemoryBuffer *secondary_buffer; // 0x18
    i32 flags;                      // 0x1c
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

/* A registered editor value type. */
struct EdType {                                 // size 0xc
    char *name;                                 // 0x0
    i32 size;                                   // 0x4
    void (*serialise)(EdStream &, void *, i32); // 0x8

    void Serialise(EdStream &stream);
};

/* A registered editor class: its member list and object interface. */
struct EdClass {                                                                // size 0x18
    char *name;                                                                 // 0x00
    i32 flags;                                                                  // 0x04
    EdRef *members;                                                             // 0x08
    EdRef *last_member;                                                         // 0x0c
    i32 member_count;                                                           // 0x10
    EdClassInterface *object_interface; /* "interface" is an objbase.h macro */ // 0x14

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

struct EdDefunctList { // size 0xc

    EdDefunctList() : first(NULL), last(NULL), count(0) {
    }

    void ReviveAll(i32 flags);
    EdDefunctListEntry *first; // 0x0
    EdDefunctListEntry *last;  // 0x4
    i32 count;                 // 0x8
};

/* One member of an editor class: where it sits in the object, its type and
 * the control that edits it. Attribute bit 30 marks a pointer member. */
class EdRef : public CSListLink<EdRef> { // size 0x28
  public:
    EdRef(char *type_name, char *member_name, i32 offset, i32 member_size, i32 member_attributes, EdControl *member_control, i32 group);

    EdRef() {
    }

    virtual void *GetMemberObject(void *object);
    virtual void GetMemberData(void *object, i32 requested_type, void *data, i32 data_size);
    virtual void SetMemberData(void *object, i32 requested_type, void *data, i32 data_size, i16 *);

    i32 type_id;           // 0x0c
    char *name;            // 0x10
    i32 member_offset;     // 0x14
    i32 size;              // 0x18
    i32 attributes;        // 0x1c
    EdControl *control;    // 0x20
    i32 replication_group; // 0x24

    i32 GetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SetAttributeData(void *object, i32 attribute, i32 requested_type, void *data, i32 data_size);
    void CheckType(i32 requested_type);
    i32 GetTypeSize(i32 requested_type, i32 data_size);
    void Serialise(EdStream &stream, i32 *class_mapping);
};

/* A member reference bound to the object (or sub-object) that holds it. */
struct EdMember {     // size 0x8
    void *object;     // 0x0
    EdRef *reference; // 0x4

    i32 fun_006c1f60(i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 fun_006c1f90(i32 attribute, i32 requested_type, void *data, i32 data_size);
};

/* An editor object together with its class and the member it was picked by. */
struct ClassObject {   // size 0xc
    EdClass *ed_class; // 0x0
    void *object;      // 0x4
    EdRef *reference;  // 0x8

    ClassObject(EdClass *object_class = NULL, void *object_data = NULL) : ed_class(object_class), object(object_data), reference(NULL) {
    }

    i32 GetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size);
    i32 SetAttributeData(i32 attribute, i32 requested_type, void *data, i32 data_size);
    void CopyObject(ClassObject &source);

    void fun_006b89c0(EdClass *object_class, void *object_data);
    void fun_006b89d0();
    i32 fun_006b89e0();

    bool fun_006b89f0(ClassObject &other) {
        return object == other.object && reference == other.reference;
    }

    bool fun_006b8a20(ClassObject &other);

    void Set(char *name);
    void GetName(char *destination, i32 size);
};

class EditorSettings { // size 0xc
  public:
    EditorSettings();
    virtual ~EditorSettings();
    void AddMenuItems(struct eduimenu_s *menu);
    void Serialise(EdStream &stream);
    f32 cursor_radius; // 0x4
    i32 snap_terrain;  // 0x8
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

/* The editor's type/class registry, theRegistry. */
struct EdRegistry { // size 0x44

    struct NameMapping {
        char *source;
        char *destination;
    };

    i32 initialised;               // 0x00
    EdType *types;                 // 0x04
    EdClass *classes;              // 0x08
    NameMapping *mappings;         // 0x0c
    EdObjectNotifier **notifiers;  // 0x10
    i32 (*create_object_guid)();   // 0x14
    i32 type_capacity;             // 0x18
    i32 type_count;                // 0x1c
    i32 class_capacity;            // 0x20
    i32 class_count;               // 0x24
    i32 mapping_capacity;          // 0x28
    i32 object_count;              // 0x2c
    i32 notifier_capacity;         // 0x30
    i32 notifier_count;            // 0x34
    EdDefunctList defunct_objects; // 0x38

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

/* The editor's per-frame input state: the camera
 * view, the cursor ray and 40 logical inputs with press/release/repeat
 * edges. */
struct EdInputContext {       // size 0x260
    VuVec view_position;      // 0x000
    VuVec view_direction;     // 0x010
    VuVec ray_start;          // 0x020
    VuVec ray_direction;      // 0x030
    struct FRAME_PAD_PC *pad; // 0x040
    f32 delta_time;           // 0x044
    f32 current_time;         // 0x048
    f32 repeat_window;        // 0x04c
    u8 held[40];              // 0x050
    u8 pressed[40];           // 0x078
    u8 released[40];          // 0x0a0
    u8 repeated[40];          // 0x0c8
    u8 cleared[40];           // 0x0f0
    f32 values[40];           // 0x118
    f32 repeat_times[40];     // 0x1b8
    u8 reserved_258[8];       // 0x258

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
