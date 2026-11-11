#include "saga.h"
#include <new>
#include <stdio.h>

/* Real-time light editor class glue. The retail rtl unit is C++: these are the
 * EdClassInterface implementation for the light set (RTLHelper) that sits
 * beside the C rtl.c code. Editor-only; unreachable in retail. */

extern "C" {
    extern RTL_SET_PC *curr_set;
    extern f32 rtl_helper_radius;
    extern i32 rtl_helper_radius_mode;
    extern f32 min_r, def_fr;
    extern struct eduiiattr_s EdLevelAttr;
    eduiitem_s *__cdecl eduiItemSeparatorCreate(u32 data, const void *colours);
    void __cdecl cbRtlHelperModifiers(eduimenu_s *parent, eduiitem_s *item, u32 data);
    void __cdecl cbModifierAdjust(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    extern i32 EdLevelFnt;
    extern i32 modifier_cnt;
    extern char **modifier_names;
    void __cdecl eduiMenuFitOnScreen(eduimenu_s *menu, i32 padding);
    extern RTL_LIGHT_PC *rtl_locked;
    void __cdecl rtlFree(RTL_LIGHT_PC *light);
    void __cdecl fogFree(RTL_FOG_MIGRATION_PC *fog);
}

extern "C" {
    eduiitem_s *__cdecl eduiItemPropCreate(u32 data, void *colours, void *selected, void *changed, void *button,
                                           i32 button_type, char *text, char *value);
    i32 __cdecl eduiItemPropSetText(edui_prop_s *item, char *text);
    void __cdecl eduiGetCursorDelta(f32 *x, f32 *y);
}

/* Property controls of the editor unit that the light registration creates;
 * their constructors live there. */
class EdStringControl : public EdControl {
  public:
    EdStringControl();
};

class EdColourControl : public EdControl {
  public:
    EdColourControl();
};

class EdVectorControl : public EdControl {
  public:
    EdVectorControl();

    eduiitem_s *components[3];
};
STATIC_ASSERT(sizeof(EdVectorControl) == 0x1c);

class EdMatrixControl : public EdControl {
  public:
    EdMatrixControl();

    eduiitem_s *components[9];
};
STATIC_ASSERT(sizeof(EdMatrixControl) == 0x34);

/* Numeric property row clamped to [minimum, maximum]; the button drags the
 * value with the cursor or the analogue sticks. */
template <class T> class EdValueControl : public EdControl {
  public:
    static void *operator new(size_t size) {
        return theMemoryManager.AllocPool(size, 1);
    }
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdValueControl));
    }
    EdValueControl() : minimum(0), maximum(0) {
    }
    EdValueControl(T minimum, T maximum) : minimum(minimum), maximum(maximum) {
    }
    virtual void AddMenuItem(eduimenu_s *menu, EdRef *member, void *target);
    virtual void Refresh();
    void GetVal(T *value) {
        reference->GetMemberData(object, value_type, value, 0);
    }
    void SetVal(T *value) {
        reference->SetMemberData(object, value_type, value, 0, NULL);
    }
    /* Drag step per unit of stick or cursor travel; finer for narrow ranges. */
    static f32 GetSensitivity(eduiitem_s *item) {
        EdValueControl *control = static_cast<EdValueControl *>(item->data_ptr);
        f32 sensitivity = 0.01f;
        T range = control->maximum - control->minimum;
        if (range < 5)
            sensitivity = 0.001f;
        return sensitivity;
    }
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32 flags);

    static f32 MouseScale;

    i32 value_type;
    char *format;
    T minimum;
    T maximum;
};
STATIC_ASSERT(sizeof(EdValueControl<i32>) == 0x20);
STATIC_ASSERT(offsetof(EdValueControl<f32>, value_type) == 0x10);
STATIC_ASSERT(offsetof(EdValueControl<f32>, minimum) == 0x18);

template <> f32 EdValueControl<i32>::MouseScale = 100.0f;
template <> f32 EdValueControl<f32>::MouseScale = 100.0f;

template <class T> void EdValueControl<T>::Refresh() {
    T value;
    GetVal(&value);
    char text[128];
    NuSPrintf(text, format, value);
    eduiItemPropSetText(reinterpret_cast<edui_prop_s *>(item), text);
}

template <class T> void EdValueControl<T>::cbChanged(eduimenu_s *, eduiitem_s *item, u32) {
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    EdValueControl *control = static_cast<EdValueControl *>(item->data_ptr);
    T value = (T)NuAToF(property->property_text);
    if (value < control->minimum)
        value = control->minimum;
    if (value > control->maximum)
        value = control->maximum;
    control->SetVal(&value);
    char text[128];
    NuSPrintf(text, control->format, value);
    eduiItemPropSetText(property, text);
}

template <class T> void EdValueControl<T>::cbButton(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
    EdValueControl *control = static_cast<EdValueControl *>(item->data_ptr);
    FRAME_PAD_PC *pad = EdControl::Input->pad;
    f32 sensitivity = GetSensitivity(item);
    property->button_latched = 1;
    T value = (T)NuAToF(property->property_text);
    f32 dx = 0.0f;
    f32 change = 0.0f;
    if (pad != NULL && (pad->digital_buttons & 0x40)) {
        if (pad->analog_left_y > 128.0)
            change = sensitivity * 10.0 * (pad->analog_left_y - 128.0);
        if (pad->analog_left_y < 0x80)
            change = sensitivity * -10.0 * (128.0 - pad->analog_left_y);
        if (pad->analog_right_y > 128.0)
            change = sensitivity * 0.1 * (pad->analog_right_y - 128.0);
        if (pad->analog_right_y < 0x80)
            change = sensitivity * -0.1 * (128.0 - pad->analog_right_y);
        value -= (T)change;
    } else {
        eduiGetCursorDelta(&dx, &change);
        value -= (T)(MouseScale * change);
    }
    char text[128];
    NuSPrintf(text, control->format, value);
    eduiItemPropSetText(property, text);
    cbChanged(menu, item, flags);
}

template <class T> void EdValueControl<T>::AddMenuItem(eduimenu_s *menu, EdRef *member, void *target) {
    EdValueControl *control = new EdValueControl;
    if (control != NULL) {
        control->value_type = value_type;
        control->format = format;
        control->minimum = minimum;
        control->maximum = maximum;
        control->reference = member;
        control->object = target;
        T value;
        control->GetVal(&value);
        char text[128];
        NuSPrintf(text, format, value);
        control->item = eduiItemPropCreate((u32)control, &EdLevelAttr, EdControl::cbSelected, cbChanged, cbButton, 2,
                                           member->name, text);
        eduiMenuAddItem(menu, control->item);
    }
}

class EdIntControl : public EdValueControl<i32> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdIntControl));
    }
    EdIntControl(i32 minimum, i32 maximum) : EdValueControl<i32>(minimum, maximum) {
        value_type = EdType_Int;
        format = "%d";
    }
};

class EdFloatControl : public EdValueControl<f32> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdFloatControl));
    }
    EdFloatControl() : EdValueControl<f32>(-1000000000.0f, 1000000000.0f) {
        value_type = EdType_Float;
        format = "%.2f";
    }
    EdFloatControl(f32 minimum, f32 maximum) : EdValueControl<f32>(minimum, maximum) {
        value_type = EdType_Float;
        format = "%.2f";
    }
};

void cbEdLevelDestroy(eduimenu_s *menu, eduimenu_s *);
i32 LineToPlaneIntersecion(VuVec &origin, VuVec &direction, VuVec &plane, VuVec *intersection);

STATIC_ASSERT(RTL_EDITOR_SET_SIZE_PC == 0x6fac);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, led_file) == 0x80);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, led_file) == 0x38);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, density_wii) == 0x3c);

class RTLHelper : public EdClassInterface {
  public:
    RTLHelper();
    virtual void Flush() {
        set = NULL;
    }
    virtual i32 GetNumObjects();
    virtual void *GetNextObject(void *object);
    virtual void *CreateObject(void *data, i32 guid, i32 flags);
    virtual void DestroyObject(void *object, i32) {
        rtlFree(static_cast<RTL_LIGHT_PC *>(object));
    }
    virtual void Process(void *object, EdInputContext &input);
    virtual void Render(void *object, i32 selected);
    virtual void EnterEditor();
    virtual void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    virtual void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
        curr_set = NULL;
    }
    virtual void PreSaveInitialisation() {
        curr_set = set;
    }
    virtual void PostSaveInitialisation() {
        curr_set = NULL;
    }
    virtual void AddMenuItems(eduimenu_s *menu);
    RTL_SET_PC *GetIndexedSet();
    void Initialise();

    RTL_SET_PC *set;
};
STATIC_ASSERT(sizeof(RTLHelper) == 0xc);
STATIC_ASSERT(offsetof(RTLHelper, set) == 0x8);

RTLHelper::RTLHelper() : set(NULL) {
}

i32 RTLHelper::GetNumObjects() {
    i32 count = 0;
    i32 i;
    for (i = 0; i < 128; i++) {
        if (curr_set->lights[i].type != 0)
            count++;
    }
    return count;
}

void *RTLHelper::GetNextObject(void *object) {
    RTL_SET_PC *rtl_set = curr_set;
    i32 i;
    if (rtl_set != NULL) {
        if (object == NULL)
            return &rtl_set->lights[0];
        for (i = static_cast<RTL_LIGHT_PC *>(object) - rtl_set->lights + 1; i < 128; i++) {
            if (rtl_set->lights[i].type != 0)
                return &rtl_set->lights[i];
        }
    }
    return NULL;
}

/* A new light: the first free slot of the current set with the editor's
 * default radii, colours and direction, tagged with the current LED file. */
void *RTLHelper::CreateObject(void *, i32, i32) {
    RTL_LIGHT_PC *light;
    i32 i;
    /* rtlAlloc, inlined here in retail's single C++ rtl unit. */
    if (curr_set != NULL) {
        for (i = 0; i < 128; i++) {
            if (curr_set->lights[i].type == 0)
                goto found;
        }
    }
    return NULL;
found:
    light = &curr_set->lights[i];
    if (light != NULL) {
        rtl_locked = NULL;
        light->position = APIObjectZeroMomentumPC;
        light->inner_radius = min_r;
        light->outer_radius = def_fr;
        light->colour.x = 1.0f;
        light->colour.y = 1.0f;
        light->colour.z = 1.0f;
        light->secondary_colour.x = 0.5f;
        light->secondary_colour.y = 0.5f;
        light->secondary_colour.z = 0.5f;
        light->type = 2;
        light->flags &= static_cast<u8>(~1u);
        light->field_5e = 0;
        light->field_60 = 0;
        light->parameters[0] = 0.1f;
        light->parameters[1] = 0.1f;
        light->parameters[2] = 0.1f;
        light->parameters[3] = 0.1f;
        light->parameter_54 = 0.0f;
        light->pitch = 0;
        light->yaw = 0;
        light->direction = RtlUnitDirectionZPc;
        light->field_64 = 0.0f;
        NuVecRotateX(&light->direction, &light->direction, light->pitch);
        NuVecRotateY(&light->direction, &light->direction, light->yaw);
        light->field_79 = -1;
        light->field_7a = -1;
        light->field_7b = 0;
        light->field_7c = curr_set->lights;
        light->led_file = Placeable::CurrentLedFile;
    }
    return light;
}

/* Opens a submenu with one 0..1 strength slider per light modifier. */
void cbRtlHelperModifiers(eduimenu_s *parent, eduiitem_s *item, u32) {
    eduimenu_s *menu =
        eduiMenuCreate(item->x + parent->width, item->y, 180, 250, (void *)EdLevelFnt, cbEdLevelDestroy, NULL);
    i32 i;
    if (menu == NULL)
        return;
    for (i = 0; i < modifier_cnt; i++) {
        edui_slider_s *slider =
            (edui_slider_s *)eduiMenuAddItem(menu, eduiItemSliderCreate(i, item->colours, 0, cbModifierAdjust, 0.0f,
                                                                        1.0f, modifiers[i], modifier_names[i]));
        eduiItemSliderSetGranularity(slider, 0.1f);
    }
    eduiMenuFitWidth(menu, 5);
    eduiMenuFitOnScreen(menu, 1);
    eduiMenuAttach(parent, menu);
}

void RTLHelper::AddMenuItems(eduimenu_s *menu) {
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 1, cbRtlHelperModifiers, "Modifiers"));
}

f32 VuVec::Length() {
    f32 length_squared = x * x + y * y + z * z;
    return NuFsqrt(length_squared);
}

/* Screen-space distance below which a radius ring can be grabbed. */
static f32 rtl_helper_grab_distance = 0.5f;

/* Drags the light's inner or outer radius ring in the camera-facing plane
 * through the light; the ring nearest the cursor on screen is grabbed when
 * the drag starts. */
void RTLHelper::Process(void *object, EdInputContext &input) {
    RTL_LIGHT_PC *light = static_cast<RTL_LIGHT_PC *>(object);
    if (input.GetHold(22) != 0.0f) {
        __declspec(align(16)) VuVec difference;
        __declspec(align(16)) VuVec offset;
        __declspec(align(16)) VuVec position;
        __declspec(align(16)) VuVec direction;
        __declspec(align(16)) VuVec plane;
        __declspec(align(16)) VuVec delta;
        __declspec(align(16)) VuVec outer_point;
        __declspec(align(16)) VuVec inner_point;
        __declspec(align(16)) VuVec intersection;
        __declspec(align(16)) VuVec screen_cursor;
        __declspec(align(16)) VuVec screen_inner;
        __declspec(align(16)) VuVec screen_outer;
        position.xyz = light->position;
        plane.xyz = *reinterpret_cast<NUVEC *>(&global_camera.mtx.m20);
        plane.w = -(plane.x * position.x + plane.y * position.y + plane.z * position.z);
        if (LineToPlaneIntersecion(input.ray_start, input.ray_direction, plane, &intersection)) {
            difference.x = intersection.x - position.x;
            difference.y = intersection.y - position.y;
            difference.z = intersection.z - position.z;
            delta.x = difference.x;
            delta.y = difference.y;
            delta.z = difference.z;
            delta.w = difference.w;
            if (input.GetPress(25) != 0.0f || input.GetHold(25) == 0.0f) {
                direction.x = difference.x;
                direction.y = difference.y;
                direction.z = difference.z;
                direction.w = difference.w;
                NuVecNorm(&direction.xyz, &direction.xyz);
                direction.w = 1.0f;
                f32 radius = light->inner_radius;
                offset.x = direction.x * radius;
                offset.y = direction.y * radius;
                offset.z = direction.z * radius;
                difference.x = position.x + offset.x;
                difference.y = position.y + offset.y;
                difference.z = position.z + offset.z;
                inner_point.x = difference.x;
                inner_point.y = difference.y;
                inner_point.z = difference.z;
                inner_point.w = difference.w;
                radius = light->outer_radius;
                offset.x = direction.x * radius;
                offset.y = direction.y * radius;
                offset.z = direction.z * radius;
                difference.x = position.x + offset.x;
                difference.y = position.y + offset.y;
                difference.z = position.z + offset.z;
                outer_point.x = difference.x;
                outer_point.y = difference.y;
                outer_point.z = difference.z;
                outer_point.w = difference.w;
                NuCameraTransformScreenClip(&screen_cursor.xyz, &intersection.xyz, 1, NULL);
                NuCameraTransformScreenClip(&screen_inner.xyz, &inner_point.xyz, 1, NULL);
                NuCameraTransformScreenClip(&screen_outer.xyz, &outer_point.xyz, 1, NULL);
                screen_outer.z = 0.0f;
                screen_inner.z = 0.0f;
                screen_cursor.z = 0.0f;
                offset.x = screen_cursor.x - screen_inner.x;
                offset.y = screen_cursor.y - screen_inner.y;
                offset.z = screen_cursor.z - screen_inner.z;
                f32 inner_distance = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
                offset.x = screen_cursor.x - screen_outer.x;
                offset.y = screen_cursor.y - screen_outer.y;
                offset.z = screen_cursor.z - screen_outer.z;
                f32 outer_distance = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
                f32 nearest_distance;
                i32 nearest_mode;
                if (outer_distance > inner_distance) {
                    nearest_distance = inner_distance;
                    nearest_mode = 1;
                } else {
                    nearest_distance = outer_distance;
                    nearest_mode = 2;
                }
                rtl_helper_radius_mode = 0;
                if (nearest_distance < rtl_helper_grab_distance * rtl_helper_grab_distance)
                    rtl_helper_radius_mode = nearest_mode;
                if (input.GetHold(25) != 0.0f) {
                    if (rtl_helper_radius_mode == 1) {
                        f32 radius = delta.Length();
                        light->inner_radius = radius;
                        if (light->outer_radius < radius)
                            light->outer_radius = radius;
                    }
                    if (rtl_helper_radius_mode == 2) {
                        f32 radius = delta.Length();
                        light->outer_radius = radius;
                        if (light->inner_radius > radius)
                            light->inner_radius = radius;
                    }
                }
            } else {
                /* The same drag, kept apart from the grab test as in retail. */
                if (input.GetHold(25) != 0.0f) {
                    if (rtl_helper_radius_mode == 1) {
                        f32 radius = delta.Length();
                        light->inner_radius = radius;
                        if (light->outer_radius < radius)
                            light->outer_radius = radius;
                    }
                    if (rtl_helper_radius_mode == 2) {
                        f32 radius = delta.Length();
                        light->outer_radius = radius;
                        if (light->inner_radius > radius)
                            light->inner_radius = radius;
                    }
                }
            }
        }
    } else {
        rtl_helper_radius_mode = 0;
    }
}

void RTLHelper::EnterEditor() {
    curr_set = rtlGetEnvSet();
    if (curr_set == NULL)
        curr_set = set;
    RTL_SET_INDEX_PC(curr_set)->enabled = 0;
}

/* The 16-byte-aligned permanent-buffer carve is the inline allocator retail
 * also uses in refpack_init. */
void RTLHelper::PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
    if (set == NULL) {
        MEMORYMANAGER *manager = &theMemoryManager;
        RTL_SET_PC *fresh = NULL;
        if (RTL_EDITOR_SET_SIZE_PC < *manager->end_anchor - *manager->start_anchor) {
            *manager->start_anchor = (*manager->start_anchor + 15) & ~15;
            fresh = reinterpret_cast<RTL_SET_PC *>(*manager->start_anchor);
            *manager->start_anchor += RTL_EDITOR_SET_SIZE_PC;
            memset(fresh, 0, RTL_EDITOR_SET_SIZE_PC);
            manager->unknown_14 += RTL_EDITOR_SET_SIZE_PC;
            manager->size -= RTL_EDITOR_SET_SIZE_PC;
            manager->unknown_10 = *manager->start_anchor;
        }
        set = fresh;
        RTL_SET_INDEX_PC(set)->enabled = 0;
    }
    curr_set = set;
}

static __declspec(noinline) void RtlHelperCopyMtx(NUMTX *dst, const NUMTX *src) {
    dst->m00 = src->m00;
    dst->m01 = src->m01;
    dst->m02 = src->m02;
    dst->m03 = src->m03;
    dst->m10 = src->m10;
    dst->m11 = src->m11;
    dst->m12 = src->m12;
    dst->m13 = src->m13;
    dst->m20 = src->m20;
    dst->m21 = src->m21;
    dst->m22 = src->m22;
    dst->m23 = src->m23;
    dst->m30 = src->m30;
    dst->m31 = src->m31;
    dst->m32 = src->m32;
    dst->m33 = src->m33;
}

#define RTLHELPER_COLOUR(c, scale)                                                                                     \
    (((static_cast<i32>((c).z * (scale)) & 0xff) << 16 | static_cast<i32>(0xff000000)) |                               \
     ((static_cast<i32>((c).y * (scale)) & 0xff) << 8) | (static_cast<i32>((c).x * (scale)) & 0xff))

void RTLHelper::Render(void *object, i32) {
    RTL_LIGHT_PC *light = static_cast<RTL_LIGHT_PC *>(object);
    i32 colour = RTLHELPER_COLOUR(light->colour, 255.0);
    i32 dark_colour = RTLHELPER_COLOUR(light->colour, 128.0);
    __declspec(align(16)) VuVec position;
    __declspec(align(16)) VuVec direction;
    __declspec(align(16)) VuVec centre;
    __declspec(align(16)) VuVec half;
    f32 scale;
    __declspec(align(16)) NUMTX matrix;

    edrtlDrawLightEx(light - curr_set->lights, rtl_helper_radius_mode);
    position.xyz = light->position;
    direction.xyz = light->direction;
    scale = -rtl_helper_radius;
    direction.x *= scale;
    direction.y *= scale;
    direction.z *= scale;
    RtlHelperCopyMtx(&matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    NuMtxAlignZ(&matrix, &direction.xyz);
    EdDrawBegin(0);
    half.x = direction.x * -0.5;
    half.y = direction.y * -0.5;
    half.z = direction.z * -0.5;
    centre.x = position.x + half.x;
    centre.y = position.y + half.y;
    centre.z = position.z + half.z;
    matrix.m30 = centre.x;
    matrix.m31 = centre.y;
    matrix.m32 = centre.z;
    matrix.m33 = 1.0f;
    EdDrawPolyCylinder(&matrix, rtl_helper_radius, rtl_helper_radius, rtl_helper_radius, 4, colour, 1, 0);
    centre.x = position.x + direction.x;
    centre.y = position.y + direction.y;
    centre.z = position.z + direction.z;
    matrix.m30 = centre.x;
    matrix.m31 = centre.y;
    matrix.m32 = centre.z;
    matrix.m33 = 1.0f;
    EdDrawPolyCylinder(&matrix, rtl_helper_radius * 0.75, rtl_helper_radius, rtl_helper_radius * 2.0f, 4, dark_colour,
                       0, 0);
    EdDrawEnd();
}

/* Makes this helper's set current and builds its light index on first use;
 * returns NULL while the set is missing or empty. */
RTL_SET_PC *RTLHelper::GetIndexedSet() {
    if (set != NULL) {
        curr_set = set;
        if (GetNumObjects() > 0) {
            RTL_INDEX_PC *index = RTL_SET_INDEX_PC(set);
            if (index->enabled == 0) {
                VARIPTR buffer;
                buffer.void_ptr = index;
                IndexLights(set, &buffer, buffer.addr + RTL_SET_INDEX_AREA_SIZE_PC);
            }
            return set;
        }
    }
    return NULL;
}

#define RTLHELPER_FOGS(set) ((RTL_FOG_MIGRATION_PC *)(set)->fog)

/* EdClassInterface for the fog volumes of the current light set. */
class FogHelper : public EdClassInterface {
  public:
    FogHelper();
    virtual void Flush() {
    }
    virtual i32 GetNumObjects();
    virtual void *GetNextObject(void *object);
    virtual void *CreateObject(void *data, i32 guid, i32 flags);
    virtual void DestroyObject(void *object, i32) {
        fogFree(static_cast<RTL_FOG_MIGRATION_PC *>(object));
    }
    virtual void Render(void *object, i32 selected);
    void Initialise();
};
STATIC_ASSERT(sizeof(FogHelper) == 0x8);

FogHelper::FogHelper() {
}

i32 FogHelper::GetNumObjects() {
    i32 count = 0;
    i32 i;
    for (i = 0; i < 32; i++) {
        if (RTLHELPER_FOGS(curr_set)[i].type != 0)
            count++;
    }
    return count;
}

void *FogHelper::GetNextObject(void *object) {
    RTL_SET_PC *rtl_set = curr_set;
    i32 i;
    if (rtl_set != NULL) {
        if (object == NULL)
            return &RTLHELPER_FOGS(rtl_set)[0];
        for (i = static_cast<RTL_FOG_MIGRATION_PC *>(object) - RTLHELPER_FOGS(rtl_set) + 1; i < 32; i++) {
            if (RTLHELPER_FOGS(rtl_set)[i].type != 0)
                return &RTLHELPER_FOGS(rtl_set)[i];
        }
    }
    return NULL;
}

/* The fog's editor sphere; a spherical fog also shows its extent, boxed when
 * selected. */
void FogHelper::Render(void *object, i32 selected) {
    RTL_FOG_MIGRATION_PC *fog = static_cast<RTL_FOG_MIGRATION_PC *>(object);
    ClassObject fog_object(object_class, fog);
    theClassEditor.DrawObjectSphere(fog_object, fog->colour);
    switch (fog->type) {
        case 1:
            RndrOSphere(&fog->position, fog->radius, (fog->colour & 0xffffff) | 0x80000000, 16, NULL);
            if (selected)
                RndrOSquare(&fog->position, fog->radius, -1, NULL);
            break;
    }
}

/* A new fog zone in the first free slot of the current set. */
void *FogHelper::CreateObject(void *, i32, i32) {
    RTL_FOG_MIGRATION_PC *fog;
    i32 i;
    /* fogAlloc, inlined here in retail's single C++ rtl unit. */
    if (curr_set != NULL) {
        for (i = 0; i < 32; i++) {
            if (RTLHELPER_FOGS(curr_set)[i].type == 0)
                goto found;
        }
    }
    return NULL;
found:
    fog = &RTLHELPER_FOGS(curr_set)[i];
    if (fog != NULL) {
        fog->type = 1;
        fog->position = APIObjectZeroMomentumPC;
        fog->radius = min_r;
        fog->start = 10.0f;
        fog->end = 100.0f;
        fog->start_psp = 10.0f;
        fog->end_psp = 100.0f;
        fog->colour = 0x80808080;
        fog->low_quality_density = 0;
        fog->low_quality_colour = 0;
        fog->density = 0.0f;
        fog->density_wii = 0.0f;
        fog->depth_of_field_fstop = 1;
        fog->led_file = Placeable::CurrentLedFile;
    }
    return fog;
}

/* Editor member references into a light. member_offset carries a pseudo id
 * (0x80000002 up) instead of a byte offset. */
class EdRefRTL : public EdRef {
  public:
    EdRefRTL(char *type_name, char *member_name, i32 offset, i32 member_size, i32 member_attributes,
             EdControl *member_control)
        : EdRef(type_name, member_name, offset, member_size, member_attributes, member_control, 0) {
    }
    virtual void GetMemberData(void *object, i32 requested_type, void *data, i32 data_size);
    virtual void SetMemberData(void *object, i32 requested_type, void *data, i32 data_size, i16 *);
};

void EdRefRTL::GetMemberData(void *object, i32 requested_type, void *data, i32) {
    RTL_LIGHT_PC *light = static_cast<RTL_LIGHT_PC *>(object);
    CheckType(requested_type);
    switch (member_offset) {
        case 0x80000002:
            sprintf(static_cast<char *>(data), "Light%d", light - curr_set->lights);
            break;
        case 0x80000003: {
            NUMTX *matrix = static_cast<NUMTX *>(data);
            RtlHelperCopyMtx(matrix, &ANI3_EVAL_IDENTITY_MATRIX);
            matrix->translation = light->position;
            NuMtxAlignZ(matrix, &light->direction);
            break;
        }
        case 0x80000004:
            *static_cast<i32 *>(data) = light->type;
            break;
        case 0x80000005:
            *static_cast<i32 *>(data) = light->disabled;
            break;
        case 0x80000006:
            *static_cast<i32 *>(data) = light->cast_shadow;
            break;
        case 0x80000007:
            *static_cast<i32 *>(data) = light->has_specular;
            break;
        case 0x80000008:
            *static_cast<f32 *>(data) = light->inner_radius;
            break;
        case 0x80000009:
            *static_cast<f32 *>(data) = light->outer_radius;
            break;
        case 0x8000000a:
            *static_cast<i32 *>(data) = static_cast<u16>(light->field_5e);
            break;
        case 0x8000000b:
            *static_cast<i32 *>(data) = static_cast<u16>(light->field_60);
            break;
        case 0x8000000c:
            *static_cast<i32 *>(data) = light->field_68;
            break;
        case 0x8000000d:
            *static_cast<i32 *>(data) = light->group_id;
            break;
        case 0x8000000e:
            *static_cast<i32 *>(data) = light->field_7b;
            break;
    }
}

void EdRefRTL::SetMemberData(void *object, i32 requested_type, void *data, i32, i16 *) {
    RTL_LIGHT_PC *light = static_cast<RTL_LIGHT_PC *>(object);
    f32 radius;
    CheckType(requested_type);
    switch (member_offset) {
        case 0x80000003: {
            NUMTX *matrix = static_cast<NUMTX *>(data);
            light->position = matrix->translation;
            light->direction = *reinterpret_cast<NUVEC *>(&matrix->m20);
            break;
        }
        case 0x80000004:
            light->type = *static_cast<u8 *>(data);
            break;
        case 0x80000005:
            light->disabled = *static_cast<u8 *>(data);
            break;
        case 0x80000006:
            light->cast_shadow = *static_cast<u8 *>(data);
            break;
        case 0x80000007:
            light->has_specular = *static_cast<u8 *>(data);
            break;
        case 0x80000008:
            if (*static_cast<f32 *>(data) < static_cast<f64>(0.1f))
                *static_cast<f32 *>(data) = 0.1f;
            radius = *static_cast<f32 *>(data);
            light->inner_radius = radius;
            if (light->outer_radius < radius)
                light->outer_radius = radius;
            break;
        case 0x80000009:
            if (*static_cast<f32 *>(data) < static_cast<f64>(0.1f))
                *static_cast<f32 *>(data) = 0.1f;
            radius = *static_cast<f32 *>(data);
            light->outer_radius = radius;
            if (light->inner_radius > radius)
                light->inner_radius = radius;
            break;
        case 0x8000000a:
            light->field_5e = *static_cast<i16 *>(data);
            light->field_60 &= ~light->field_5e;
            break;
        case 0x8000000b:
            light->field_60 = *static_cast<i16 *>(data);
            light->field_5e &= ~light->field_60;
            break;
        case 0x8000000c:
            light->field_68 = *static_cast<u16 *>(data);
            break;
        case 0x8000000d:
            light->group_id = *static_cast<u8 *>(data);
            break;
        case 0x8000000e:
            light->field_7b = *static_cast<i8 *>(data);
            break;
    }
}

/* Light types offered by the "Type" row. */
static EdEnumControl::Item rtl_helper_type_items[] = {
    {"AMBIENT", 1},    {"POINT", 2},     {"POINTFLICKER", 3}, {"DIRECTIONAL", 4}, {"CAMDIR", 5},
    {"POINTBLEND", 6}, {"ANTILIGHT", 7}, {"JONFLICKER", 8},   {NULL, 0},
};

#define RTLHELPER_REF(ref_class, type, name, offset, size, attributes, control)                                        \
    new (theMemoryManager.AllocPool(sizeof(ref_class), 1)) ref_class(type, name, offset, size, attributes, control)
#define RTLHELPER_NEW(control_class) new (theMemoryManager.AllocPool(sizeof(control_class), 1)) control_class

/* Registers the "Light" class: its name, LED file, type, colours, flicker
 * timing, transform, radii, id masks and flags. */
void RTLHelper::Initialise() {
#pragma push_macro("RegisterClass")
#undef RegisterClass
    EdClass *object_class = theRegistry.RegisterClass("Light", this, 0x8400000);
#pragma pop_macro("RegisterClass")
    object_class->AddType(
        RTLHELPER_REF(EdRefRTL, "String", "Name", 0x80000002, 0x40, 0x10000002, RTLHELPER_NEW(EdStringControl)));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Short", "LEDFile", 0x80, 0, 0x10000100, NULL, 0));
    object_class->AddType(
        RTLHELPER_REF(EdRefRTL, "Int", "Type", 0x80000004, 0, 0, RTLHELPER_NEW(EdEnumControl)(rtl_helper_type_items)));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Colour3", "Hi Col", 0x24, 0, 0, RTLHELPER_NEW(EdColourControl), 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Colour3", "Lo Col", 0x30, 0, 0, RTLHELPER_NEW(EdColourControl), 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Multiplier", 0x6c, 0, 0, new EdFloatControl(0.1f, 127.0f), 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "High Time", 0x44, 0, 0, new EdFloatControl, 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "High Rand", 0x48, 0, 0, new EdFloatControl, 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Low Time", 0x4c, 0, 0, new EdFloatControl, 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Low Rand", 0x50, 0, 0, new EdFloatControl, 0));
    object_class->AddType(
        RTLHELPER_REF(EdRefRTL, "VuMtx", "Pos/Rot", 0x80000003, 0, 0x18, RTLHELPER_NEW(EdMatrixControl)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Float", "Inner", 0x80000008, 0, 0, new EdFloatControl));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Float", "Outer", 0x80000009, 0, 0, new EdFloatControl));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Associate IDs", 0x8000000a, 0, 0,
                                        RTLHELPER_NEW(EdBitControl)(0xffff, EdEnumControl::YesNoItems)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Exclude IDs", 0x8000000b, 0, 0,
                                        RTLHELPER_NEW(EdBitControl)(0xffff, EdEnumControl::YesNoItems)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "User ID", 0x8000000c, 0, 0, new EdIntControl(0, 16)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Group ID", 0x8000000d, 0, 0, new EdIntControl(0, 255)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Modifier Id", 0x8000000e, 0, 0, new EdIntControl(0, 32)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Cast Shadows", 0x80000006, 0, 0,
                                        RTLHELPER_NEW(EdEnumControl)(EdEnumControl::YesNoItems)));
    object_class->AddType(RTLHELPER_REF(EdRefRTL, "Int", "Specular", 0x80000007, 0, 0,
                                        RTLHELPER_NEW(EdEnumControl)(EdEnumControl::YesNoItems)));
}

/* Editor member references into a fog zone; colours are edited as 0..1
 * vectors, alphas and the low-quality density as 0..1 against 128. */
class EdRefFog : public EdRef {
  public:
    EdRefFog(char *type_name, char *member_name, i32 offset, i32 member_size, i32 member_attributes,
             EdControl *member_control)
        : EdRef(type_name, member_name, offset, member_size, member_attributes, member_control, 0) {
    }
    virtual void GetMemberData(void *object, i32 requested_type, void *data, i32 data_size);
    virtual void SetMemberData(void *object, i32 requested_type, void *data, i32 data_size, i16 *);
};

void EdRefFog::GetMemberData(void *object, i32 requested_type, void *data, i32) {
    RTL_FOG_MIGRATION_PC *fog = static_cast<RTL_FOG_MIGRATION_PC *>(object);
    CheckType(requested_type);
    switch (member_offset) {
        case 0x80000002:
            sprintf(static_cast<char *>(data), "Fog%d", fog - RTLHELPER_FOGS(curr_set));
            break;
        case 0x80000003: {
            NUVEC *colour = static_cast<NUVEC *>(data);
            u8 *bytes = reinterpret_cast<u8 *>(&fog->colour);
            colour->x = bytes[0] * (1.0f / 255.0f);
            colour->y = bytes[1] * (1.0f / 255.0f);
            colour->z = bytes[2] * (1.0f / 255.0f);
            break;
        }
        case 0x80000004:
            *static_cast<f32 *>(data) = reinterpret_cast<u8 *>(&fog->colour)[3] * (1.0f / 128.0f);
            break;
        case 0x80000005: {
            NUVEC *colour = static_cast<NUVEC *>(data);
            u8 *bytes = reinterpret_cast<u8 *>(&fog->low_quality_colour);
            colour->x = bytes[0] * (1.0f / 255.0f);
            colour->y = bytes[1] * (1.0f / 255.0f);
            colour->z = (1.0f / 255.0f) * bytes[2];
            break;
        }
        case 0x80000006:
            *static_cast<f32 *>(data) = reinterpret_cast<u8 *>(&fog->low_quality_colour)[3] * (1.0f / 128.0f);
            break;
        case 0x80000007:
            *static_cast<f32 *>(data) = fog->low_quality_density * (1.0f / 128.0f);
            break;
        case 0x80000008:
            *static_cast<f32 *>(data) = fog->depth_of_field_fstop * 0.1f;
            break;
        case 0x80000009:
            *static_cast<NUVEC *>(data) = fog->position;
            break;
    }
}

void EdRefFog::SetMemberData(void *object, i32 requested_type, void *data, i32, i16 *) {
    RTL_FOG_MIGRATION_PC *fog = static_cast<RTL_FOG_MIGRATION_PC *>(object);
    CheckType(requested_type);
    switch (member_offset) {
        case 0x80000003: {
            NUVEC *colour = static_cast<NUVEC *>(data);
            fog->colour =
                (fog->colour & 0xff000000) |
                (((static_cast<u32>(colour->z * 255.0f) & 0xff) << 8 | (static_cast<u32>(colour->y * 255.0f) & 0xff))
                 << 8) |
                (static_cast<u32>(colour->x * 255.0f) & 0xff);
            break;
        }
        case 0x80000004:
            reinterpret_cast<u8 *>(&fog->colour)[3] = 0;
            fog->colour |= static_cast<u32>(*static_cast<f32 *>(data) * 128.0f) << 24;
            break;
        case 0x80000005: {
            NUVEC *colour = static_cast<NUVEC *>(data);
            fog->low_quality_colour =
                (fog->low_quality_colour & 0xff000000) |
                (((static_cast<u32>(colour->z * 255.0f) & 0xff) << 8 | (static_cast<u32>(colour->y * 255.0f) & 0xff))
                 << 8) |
                (static_cast<u32>(colour->x * 255.0f) & 0xff);

            break;
        }
        case 0x80000006:
            reinterpret_cast<u8 *>(&fog->low_quality_colour)[3] = 0;
            fog->low_quality_colour |= static_cast<u32>(*static_cast<f32 *>(data) * 128.0f) << 24;
            break;
        case 0x80000007:
            fog->low_quality_density = static_cast<i32>(*static_cast<f32 *>(data) * 128.0f);
            break;
        case 0x80000008:
            fog->depth_of_field_fstop = static_cast<u8>(*static_cast<f32 *>(data) * 10.0f);
            break;
        case 0x80000009:
            fog->position = *static_cast<NUVEC *>(data);
            break;
    }
}

/* Registers the "Fog" class: name, LED file, fog and haze colours and
 * densities, blur, depth of field, range and position. */
void FogHelper::Initialise() {
#pragma push_macro("RegisterClass")
#undef RegisterClass
    EdClass *object_class = theRegistry.RegisterClass("Fog", this, 0x8400000);
#pragma pop_macro("RegisterClass")
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "String", "Name", 0x80000002, 0x40, 0x10000002, RTLHELPER_NEW(EdStringControl)));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Short", "LEDFile", 0x38, 0, 0x10000100, NULL, 0));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Colour3", "Fog Colour", 0x80000003, 0, 0, RTLHELPER_NEW(EdColourControl)));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Float", "Fog Alpha", 0x80000004, 0, 0x10000000, new EdFloatControl(0.0f, 1.0f)));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Fog Density", 0x2c, 0, 0, new EdFloatControl(0.0f, 4.0f), 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Fog Start", 0x0, 0, 0, new EdFloatControl, 0));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Fog End", 0x4, 0, 0, new EdFloatControl, 0));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Colour3", "Haze Colour", 0x80000005, 0, 0, RTLHELPER_NEW(EdColourControl)));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Float", "Haze Density", 0x80000006, 0, 0x10000000, new EdFloatControl(0.0f, 1.0f)));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Float", "Blur Density", 0x80000007, 0, 0, new EdFloatControl(0.0f, 1.0f)));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "Float", "DOF F-Stop", 0x80000008, 0, 0, new EdFloatControl(1.0f, 21.0f)));
    object_class->AddType(new (theMemoryManager.AllocPool(sizeof(EdRef), 1))
                              EdRef("Float", "Range", 0x1c, 0, 0, new EdFloatControl(1.0f, 1000.0f), 0));
    object_class->AddType(
        RTLHELPER_REF(EdRefFog, "VuVec", "Position", 0x80000009, 0, 8, RTLHELPER_NEW(EdVectorControl)));
}
#undef RTLHELPER_NEW
#undef RTLHELPER_REF
