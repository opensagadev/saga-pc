#include "saga.h"
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
    /* 0x63ef30: opens the per-modifier strength sliders (not yet ported). */
    void __cdecl cbRtlHelperModifiers(eduimenu_s *menu, eduiitem_s *item, u32 data);
    extern RTL_LIGHT_PC *rtl_locked;
    void __cdecl rtlFree(RTL_LIGHT_PC *light);
    void __cdecl fogFree(RTL_FOG_MIGRATION_PC *fog);
}

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

void RTLHelper::AddMenuItems(eduimenu_s *menu) {
    eduiMenuAddItem(menu, eduiItemSeparatorCreate(0, &EdLevelAttr));
    eduiMenuAddItem(menu, eduiItemSelCreate(0, &EdLevelAttr, 0, 1, cbRtlHelperModifiers, "Modifiers"));
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

/* Editor member references into a fog zone; colours are edited as 0..1
 * vectors, alphas and the low-quality density as 0..1 against 128. */
class EdRefFog : public EdRef {
  public:
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

EdControl::~EdControl() {
}

EdEnumControl::EdEnumControl(Item *items) : items(items) {
}

EdBitControl::EdBitControl(u32 bit_mask, Item *items) : EdEnumControl(items), bit_mask(bit_mask) {
}
