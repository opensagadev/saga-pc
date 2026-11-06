#include "saga.h"

/* Real-time light editor class glue. The retail rtl unit is C++: these are the
 * EdClassInterface implementation for the light set (RTLHelper) that sits
 * beside the C rtl.c code. Editor-only; unreachable in retail. */

extern "C" {
    extern RTL_SET_PC *curr_set;
    extern f32 rtl_helper_radius;
    extern i32 rtl_helper_radius_mode;
    void __cdecl rtlFree(RTL_LIGHT_PC *light);
}

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
