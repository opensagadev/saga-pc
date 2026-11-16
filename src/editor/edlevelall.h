#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/edtools/EdObjectNotifier.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

struct LevelEditorScene;
struct numtl_s;

struct FRAME_PAD;
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
    extern struct numtl_s *edLevel2dMtl;
    extern struct numtl_s *edLevel3dMtl;
    extern struct numtl_s *edLevelTexMtl;
    extern NUMTX *NewMtx;
    extern struct eduimenu_s *edLevelActiveMenu;
    extern struct eduimenu_s *edLevelDestroyThisMenu2;
    extern struct eduimenu_s *edLevelDestroyThisMenu;
    extern struct eduimenu_s *edLevelNextMenu;
    extern struct eduimenu_s *edLevelPinnedMenu;
    extern i32 EdLevelFnt;
    extern i32 EdLevelFntScale;
    extern i32 NewMtl;
    extern i32 NewPrim;
    extern i32 edLevelDestroyActiveMenu;
    extern i32 edlevel_mouseandkeyboard;
    extern i32 pad_disabled;
    extern struct eduiiattr_s EdLevelAttr;
    void EdDrawLineArrow(NUMTX *transform, f32 size, i32 colour);
    void EdDrawLineCircleX(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void EdDrawLineCircleY(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void EdDrawLineCircleZ(NUVEC *centre, f32 radius, i32 colour, i32 segments);
    void EdDrawLineCross(NUVEC *position, f32 size, i32 colour);
    void EdDrawLineCube(NUMTX *transform, f32 size, i32 colour);
    void EdDrawLineSegment(NUVEC *a, NUVEC *b, i32 colour);
    void EdDrawLineSphere(NUVEC *center, f32 radius, f32 scale, i32 colour);
    void EdDrawMtx(NUMTX *matrix);
    void EdDrawPolyArrow(NUVEC *from, NUVEC *to, i32 segments, i32 colour, f32 min_radius, f32 max_radius, f32 radius_scale,
                         f32 radius_bias);
    void EdDrawPolyAxis(NUMTX *transform, f32 size, i32 opacity);
    void EdDrawPolyCylinderEx(NUVEC *start, NUVEC *end, i32 sides, i32 colour, i32 cap_colour, f32 radius, f32 limit, f32 offset);
    void EdDrawPolySector(NUVEC *center, f32 radius, i32 axis, i32 first_angle, i32 last_angle, i32 colour, i32 segments);
    void EdDrawPolyTri(NUVEC *a, NUVEC *b, NUVEC *c, i32 colour);
    extern f32 rtl_helper_radius;
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
/* EdRef.member_offset pseudo ids (0x80000000 | n) of the editor classes whose members are not plain offsets; each
 * is named after the label its class registers ("Light" for EdRefRTL). */
#define EDREF_FOG_NAME 0x80000002
#define EDREF_FOG_FOG_COLOUR 0x80000003
#define EDREF_FOG_FOG_ALPHA 0x80000004
#define EDREF_FOG_HAZE_COLOUR 0x80000005
#define EDREF_FOG_HAZE_DENSITY 0x80000006
#define EDREF_FOG_BLUR_DENSITY 0x80000007
#define EDREF_FOG_DOF_F_STOP 0x80000008
#define EDREF_FOG_POSITION 0x80000009
#define EDREF_KNOT_SPLINE 0x80000001
#define EDREF_KNOT_RADIUS 0x80000002
#define EDREF_KNOT_POS 0x80000003
#define EDREF_KNOT_IN 0x80000004
#define EDREF_KNOT_OUT 0x80000005
#define EDREF_LIGHT_NAME 0x80000002
#define EDREF_LIGHT_POS_ROT 0x80000003
#define EDREF_LIGHT_TYPE 0x80000004
#define EDREF_LIGHT_CAST_SHADOWS 0x80000006
#define EDREF_LIGHT_SPECULAR 0x80000007
#define EDREF_LIGHT_INNER 0x80000008
#define EDREF_LIGHT_OUTER 0x80000009
#define EDREF_LIGHT_ASSOCIATE_IDS 0x8000000a
#define EDREF_LIGHT_EXCLUDE_IDS 0x8000000b
#define EDREF_LIGHT_USER_ID 0x8000000c
#define EDREF_LIGHT_GROUP_ID 0x8000000d
#define EDREF_LIGHT_MODIFIER_ID 0x8000000e
#define EDREF_PLACEABLE_LEDFILE 0x80000001
#define EDREF_PLACEABLE_ATTRIBUTES 0x80000002
#define EDREF_PLACEABLE_NAME 0x80000003
#define EDREF_PLACEABLE_PARAMS 0x80000005
#define EDREF_SPECIAL_OBJECT_OBJECT 0x80000008
#define EDREF_SPECIAL_OBJECT_VISIBLE 0x80000009
#define EDREF_SPECIAL_OBJECT_COLLISION 0x8000000a
#define EDREF_SPLINE_STEP 0x80000001
#define EDREF_SPLINE_HEIGHT 0x80000002
#define EDREF_SPLINE_DROP 0x80000003
#define EDREF_SPLINE_CLOSED 0x80000004

/* EdRef.attributes, the bits the EdRef constructor sets from the member's type name. */
#define EDREF_ATTR_POINTER 0x40000000 /* the type name ends in '*' */
#define EDREF_ATTR_CLASS 0x80000000   /* not a registered type: a registered class */
/* The attributes a class registers a member with; GetAttributeData and FindMember look a member up by one. */
#define EDREF_ATTR_FLAGS 0x1              /* "Attributes": the object's flag word */
#define EDREF_ATTR_NAME 0x2               /* the object's name (the knot's spline name) */
#define EDREF_ATTR_POSITION 0x8           /* "pos x/y/z" in the property editor */
#define EDREF_ATTR_ROTATION 0x10          /* "rot x/y/z" */
#define EDREF_ATTR_SCALE 0x20             /* "scale x/y/z" */
#define EDREF_ATTR_RADIUS 0x40            /* the object's radius (picking, the selection sphere) */
#define EDREF_ATTR_LED_FILE 0x100         /* "LEDFile": the scene the object belongs to */
#define EDREF_ATTR_NOT_IN_STATE 0x400000  /* left out of the state streams (SaveState, LoadState) */
#define EDREF_ATTR_NOT_IN_FILE 0x10000000 /* left out of the .led file streams */
/* EdClass.flags: the RegisterClass flags; AddType folds the members' EDREF_ATTR_CLASS_MEMBER bits in, so a class
 * flag EDREF_ATTR_POSITION means it has a position member. The NOT_IN_STATE and NOT_IN_FILE bits apply to the
 * whole class and also to an object whose flag word has them. */
#define EDREF_ATTR_CLASS_MEMBER 0x17a
#define EDCLASS_SELECTED_SPHERE 0x80 /* the editor draws a sphere around its selected objects */
#define EDCLASS_CLONE_ONLY 0x4000000 /* new objects are clones of the selected one, matched by name in a stream */
#define EDCLASS_RENDER_ALL 0x8000000 /* the editor renders its objects while they are not selected */
#define EDCLASS_HIDDEN 0x20000000    /* left out of the class menus */
/* EdStream.flags. */
#define EDSTREAM_STATE 0x400000 /* a state stream (SaveState, LoadState), not a .led file */

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

#ifdef __cplusplus

/* Base of every placeable level-editor object. */
class Placeable { // size 0x18
  public:
    i16 scene_id;    // 0x04
    i16 led_file;    // 0x06
    i32 attributes;  // 0x08
    EdString name;   // 0x0c
    EdString params; // 0x10
    u32 reserved_14; // 0x14

    static i16 CurrentLedFile;

    Placeable() : scene_id(0), led_file(CurrentLedFile), attributes(0), reserved_14(0) {
    }

    virtual ~Placeable();

    virtual Placeable *Clone(i32) const {
        return NULL;
    }

    virtual void Reset() {
    }

    virtual const char *GetName() const {
        const char *value = name.data ? name.data + 1 : NULL;
        return value ? value : "No Name";
    }

    virtual void SetName(const char *value) {
        name.Set(value);
    }

    virtual f32 GetRadius() const {
        return 1.0f;
    }

    virtual const struct VuMtx *GetInitialTransform() const {
        return NULL;
    }

    virtual void SetInitialTransform(const struct VuMtx *) {
    }

    virtual const struct VuMtx *GetCurrentTransform() const {
        return NULL;
    }

    virtual void SetCurrentTransform(const struct VuMtx *) {
    }

    virtual const struct VuVec *GetInitialPosition() const;
    virtual void SetInitialPosition(const struct VuVec *position);
    virtual const struct VuVec *GetCurrentPosition() const;
    virtual void SetCurrentPosition(const struct VuVec *position);

    virtual void Render(const struct VuMtx *) const {
    }

    i32 fun_006b8ce0();
    i32 fun_006b8cf0();
    void fun_006b8d00(i32 value);
    char *fun_006b8d10();
    i32 fun_006b8d20();
    void fun_006b8d30(i16 value);
    void fun_006c22b0(const char *value);
};

struct ClassObjectListEntry : public CSListLink<ClassObjectListEntry> { // size 0x14
    static void *operator new(size_t size);
    static void operator delete(void *memory);

    ClassObjectListEntry();

    struct EdClass *ed_class; // 0x08
    void *object;             // 0x0c
    EdRef *reference;         // 0x10
};

struct ClassObjectList {         // size 0xc
    ClassObjectListEntry *first; // 0x0
    ClassObjectListEntry *last;  // 0x4
    i32 count;                   // 0x8

    ClassObjectListEntry *fun_006c1440();
    void fun_006c1450(ClassObjectListEntry *entry, ClassObjectListEntry *position);
    void Remove(ClassObjectListEntry *entry);
    i32 fun_006c14f0();
    i32 GetAveragePosition(struct VuVec &average);
    i32 GetAveragePosition(struct VuVec &average, f32 &radius);
    i32 IsInList(ClassObject object);
    i32 IsInList(void *object, EdRef *reference);
    i32 IsInList(struct EdClass *object_class);
};

class EdTool : public CSListLink<EdTool> { // size 0xc
  public:
    virtual ~EdTool();

    virtual void Initialise(variptr_u &, variptr_u &, i32) {
    }

    virtual char const *GetName() {
        return "";
    }

    virtual i32 Process(struct EdInputContext &) {
        return 0;
    }

    virtual void Render() {
    }
};

class BaseEditor : public CSListLink<BaseEditor> { // size 0x10
  public:
    virtual ~BaseEditor();

    virtual void Initialise(variptr_u &, variptr_u &, i32 id) {
        this->id = id;
    }

    virtual char const *GetName() {
        return "";
    }

    virtual i32 ReadBlock(DATAPTR *) {
        return 0;
    }

    virtual void WriteBlock(i32) {
    }

    virtual void Serialise(EdStream &) {
    }

    virtual void ClearLevel(i32) {
    }

    virtual void Flush() {
    }

    virtual void Enter() {
    }

    virtual void Exit() {
    }

    virtual void Process(struct EdInputContext &) {
    }

    virtual void Render() {
    }

    virtual void AddMenuItems(struct eduimenu_s *) {
    }

    static void WriteBeginBlock(NUFILE file, i32 type);
    static void WriteEndBlock(NUFILE file);
    static void WriteMetaData(NUFILE file, i32 type, i32 version, i32 count);
    void ReadBuffer(void **destination, void *source, i32 size);

    i32 id; // 0xc
    static i32 blockStart[8];
    static i32 blockDepth;
};

/* The class/object editor, theClassEditor. */
class ClassEditor : public BaseEditor { // size 0x70
  public:
    ClassEditor();
    virtual ~ClassEditor();
    virtual void Initialise(variptr_u &, variptr_u &, i32);
    virtual char const *GetName();
    virtual i32 ReadBlock(DATAPTR *);
    virtual void WriteBlock(i32);
    virtual void Serialise(EdStream &);
    virtual void ClearLevel(i32);
    virtual void Flush();
    virtual void Enter();
    virtual void Exit();
    virtual void Process(struct EdInputContext &);
    virtual void Render();
    virtual void AddMenuItems(struct eduimenu_s *);

    EdTool *first_tool;               // 0x10
    EdTool *last_tool;                // 0x14
    i32 tool_count;                   // 0x18
    EdTool *active_tool;              // 0x1c
    EdManipulator *manipulator;       // 0x20
    ClassObjectList selected_objects; // 0x24
    ClassObject current_object;       // 0x30
    i32 unknown_3c;                   // 0x3c
    ClassObject pending_object;       // 0x40
    struct eduimenu_s *menu;          // 0x4c
    VuVec snap_ray;                   // 0x50
    f32 snap_distance;                // 0x60
    i32 snap_mode;                    // 0x64
    i32 mode;                         // 0x68
    i32 class_filter;                 // 0x6c

    void DrawObjectSphere(struct ClassObject &selected, i32 colour);
    static void cbEdFilterLED(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelSelectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelDeselectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    void PreSaveInitialisation();
    void PostSaveInitialisation();
    void UpdateLists(MemoryBuffer *first, MemoryBuffer *second);
    void UpdateSnapRay(struct VuVec &position);
    void PreLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second);
    void PostLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second);
    i32 SelectObject(struct ClassObject &object, i32 mode);
    i32 IsUniqueName(char *name);
    void MakeUniqueName(char const *name, char *destination, i32 size);
    i32 IsSelectedObject(struct ClassObject &object);
    i32 IsSelectedObject(void *object, EdRef *reference);
    i32 IsSelectedClass(struct EdClass *object_class);
    void SnapPoint(struct VuVec &point);
    i32 Editable(void *object, struct EdClass *object_class, i32 index);
    void SetMode(i32 new_mode);
    void UpdateClassFilter(struct EdInputContext &input);
    void UpdateSelectedObjects(struct EdInputContext &input);
    void DestroySelectedObjects();
    void DestroySelectedObjectsNow();
    void SelectLED(i32 class_id);
    void ViewSelected();
    void FocusSelected();
    void InitialiseObject(struct ClassObject &object);
    i32 CreateObject(struct ClassObject &object);
    i32 CreateObject(struct EdClass *object_class);
    i32 CreateObject(i32 class_id);
    i32 FindNearestObject(struct VuVec &point, struct ClassObject &nearest, i32 filter);
    i32 FindNearestObject(struct VuVec &point, struct ClassObject &nearest, ClassObject &after, i32 filter);
    i32 FindNearestObject(struct VuVec &origin, VuVec &direction, struct ClassObject &nearest, i32 filter);
    i32 FindNearestObject(struct VuVec &origin, VuVec &direction, struct ClassObject &nearest, ClassObject &after, i32 filter);
    static i32 cbEdCreateClassNewObject(i32 class_id);
    static i32 cbEdCopySelectedObject(struct EdInputContext &input);
    static void SetViewMenuHilight(struct eduimenu_s *menu);
    static void cbDestroyMenu(struct eduimenu_s *menu, struct eduimenu_s *);
    static void cbDestroyObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassDeleteObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassFileMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassModeMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassNewMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassNewObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassRemoveDuplicates(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectClassMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectObjectMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetMode(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetPinned(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetSnap(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetView(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSnapMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassToolsMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassViewMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbFileSelected(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    EdTool *fun_006b8be0();
    f32 &fun_006b8bf0();
    i32 fun_006b8c10();
    void fun_006b8c20(EdTool *tool);
    void fun_006b8c30(struct ClassObject &object);
    void fun_006b8c50(i32 value);
    i32 fun_006b8c60(struct ClassObject &object);
    ClassObjectList &fun_006b8c90();
    void RegisterTool(EdTool &tool);
    struct ClassObject &fun_006b8ca0();
};
#endif

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
struct EdInputContext {    // size 0x260
    VuVec view_position;   // 0x000
    VuVec view_direction;  // 0x010
    VuVec ray_start;       // 0x020
    VuVec ray_direction;   // 0x030
    struct FRAME_PAD *pad; // 0x040
    f32 delta_time;        // 0x044
    f32 current_time;      // 0x048
    f32 repeat_window;     // 0x04c
    u8 held[40];           // 0x050
    u8 pressed[40];        // 0x078
    u8 released[40];       // 0x0a0
    u8 repeated[40];       // 0x0c8
    u8 cleared[40];        // 0x0f0
    f32 values[40];        // 0x118
    f32 repeat_times[40];  // 0x1b8
    u8 reserved_258[8];    // 0x258

    void Set(i32 input, f32 value, f32 repeat_delay);
    void fun_006c1fc0(i32 input, i32 value, f32 repeat_delay);
    void Clear(i32 input);
    f32 Get(i32 input);
    f32 GetPress(i32 input);
    f32 GetRelease(i32 input);
    f32 GetHold(i32 input);
    f32 GetRepeat(i32 input);
    void Update(struct nucamera_s *camera, struct FRAME_PAD *new_pad, f32 elapsed, bool menu_active);
};
#endif

#ifdef __cplusplus

/* The level editor thing, theLevelEditor. */
class LevelEditor : public BaseThing { // size 0x10b0
  public:
    LevelEditor();

    virtual char const *GetName() {
        return "LevelEditor";
    }

    virtual void ProcessEvenWhenPaused(struct ThingProcessData *);
    virtual void Display(ThingRenderData *);

    void SetPadText(i32 buttons, char *text);
    void RegisterEditor(BaseEditor &editor);
    void ClearLevel(i32 index);
    void Initalise(variptr_u &buffer, variptr_u &buffer_end, i32 from_front);
    void AddInfoText(char *text);
    void SetCameraPosition(struct VuVec &position);
    void CreateMenu();
    void CreateEditorList(struct eduimenu_s *parent, struct eduiitem_s *item);
    void Enter();
    void Exit();
    struct NUGSCN_s *GetScene(i32 index);
    struct LevelEditorScene *GetEdScene(i32 index);
    void Flush();
    i32 WriteStream(EdFileOutputStream &stream);
    i32 ReadStream(EdFileInputStream &stream);
    i32 Save();
    void BeginMultiLoad(variptr_u *buffer, variptr_u *buffer_end);
    void EndMultiLoad(variptr_u *buffer, variptr_u *buffer_end);
    i32 SaveState(i32 scene_index, variptr_u *buffer, variptr_u *buffer_end);
    i32 SaveState(variptr_u *buffer, variptr_u *buffer_end);
    void SetSaveFilename(char *name);
    void Reset();
    i32 AddScene(char *name, struct NUGSCN_s *scene, i32 active);
    i32 FindSceneId(char *name);
    i32 IsEditable(i32 index);
    i32 Load(char *filename, variptr_u *buffer, variptr_u *buffer_end, i32 flags);
    i32 LoadState(variptr_u *buffer, variptr_u *buffer_end, variptr_u *scratch_begin, variptr_u *scratch_end, variptr_u *file_begin,
                  variptr_u *file_end);
    void DrawInfoText(char **lines, i32 count, i32 x, i32 y, i32 available_width, i32 available_height, i32 text_colour, i32 background);
    void fun_006b8ff0(bool value);
    void fun_006b9000(u8 value);
    void fun_006b9010(u8 value);
    i16 fun_006b9020();
    void fun_006b9030(i16 value);
    i32 fun_006b9040();
    f32 *fun_006b9080();
    void fun_006b9090(i32 value);
    i32 fun_006b90a0();
    void fun_006b90b0(i32 value);
    void fun_006b90c0(i32 value);
    i32 fun_006b90e0();
    void fun_006b90f0(BaseEditor *editor);
    struct NUGSCN_s *fun_006bc740(char *name);
    i32 fun_006bc7e0(struct NUGSCN_s *scene);
    void fun_006bc9d0(struct eduimenu_s *menu);
    char *fun_006bc9e0(char *text);

    f32 background_colour[4];     // 0x0010
    i32 unknown_20;               // 0x0020
    i32 unknown_24;               // 0x0024
    i32 unknown_28;               // 0x0028
    i32 unknown_2c;               // 0x002c
    i32 unknown_30;               // 0x0030
    f32 overlay_alpha;            // 0x0034
    i32 unknown_38;               // 0x0038
    i32 editor_buffer_from_front; // 0x003c
    EdInputContext input;         // 0x0040
    u8 loading;                   // 0x02a0
    u8 destroying_objects;        // 0x02a1
    u8 reserved_0x2a2[2];         // 0x02a2
    i32 editable_scene_count;     // 0x02a4
    i32 reset_pending;            // 0x02a8
    LevelEditorScene scenes[10];  // 0x02ac
    BaseEditor *first_editor;     // 0x093c
    BaseEditor *last_editor;      // 0x0940
    i32 editor_count;             // 0x0944
    BaseEditor *active_editor;    // 0x0948
    char save_filename[0x80];     // 0x094c
    char editor_filename[0x80];   // 0x09cc
    i32 file_version;             // 0x0a4c
    i32 multi_load_active;        // 0x0a50
    i16 current_led_file;         // 0x0a54
    u8 reserved_0xa56[2];         // 0x0a56
    VARIPTR editor_buffer_cursor; // 0x0a58
    VARIPTR editor_buffer_begin;  // 0x0a5c
    VARIPTR editor_buffer_end;    // 0x0a60
    char text_buffer[0x400];      // 0x0a64
    i32 text_length;              // 0x0e64
    char *info_text[32];          // 0x0e68
    i32 info_x;                   // 0x0ee8
    i32 info_y;                   // 0x0eec
    i32 info_width;               // 0x0ef0
    i32 info_height;              // 0x0ef4
    u32 info_colour;              // 0x0ef8
    u32 info_background;          // 0x0efc
    u8 reserved_0xf00[0x100];     // 0x0f00
    i32 pad_x;                    // 0x1000
    i32 pad_y;                    // 0x1004
    i32 pad_width;                // 0x1008
    i32 pad_height;               // 0x100c
    u32 pad_background;           // 0x1010
    u32 pad_colour;               // 0x1014
    u8 reserved_0x1018[4];        // 0x1018
    char *pad_text[32];           // 0x101c
    EditorSettings settings;      // 0x109c
    i32 editors_entered;          // 0x10a8
    i32 active;                   // 0x10ac
};

extern LevelEditor theLevelEditor;
#endif

#ifdef __cplusplus
extern EdManipulator theDefaultManipulator;
#endif

#ifdef __cplusplus
i32 LineToPlaneIntersecion(VuVec &origin, VuVec &direction, VuVec &plane, VuVec *intersection);
#endif
