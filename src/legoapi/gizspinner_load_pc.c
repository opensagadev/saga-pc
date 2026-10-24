#include "saga.h"

/* Private fragment, native2206B +real12B BSS owner. Same-TU bounds helper uses
 * optimizerESI in retail; compiled ABI/precision gate required before full
 * validation. */
extern const char LevelObjectNamePc00754364[20];
extern const char LevelObjectNamePc00754378[21];
extern const char GizmoRegistrySpinnerNamePc[8];
nuhspecial_s thingsSceneArm = {0};
STATIC_ASSERT(sizeof(thingsSceneArm) == 12);
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(GIZSPINNER_s, name) == 0x40);
STATIC_ASSERT(offsetof(GIZSPINNER_s, special) == 0x50);
STATIC_ASSERT(offsetof(GIZSPINNER_s, position) == 0x5c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, anim_set) == 0x68);
STATIC_ASSERT(offsetof(GIZSPINNER_s, primary_anim_obj) == 0x6c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, animation_speed) == 0x78);
STATIC_ASSERT(offsetof(GIZSPINNER_s, rotation) == 0x7c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, initial_rotation) == 0x82);
STATIC_ASSERT(offsetof(GIZSPINNER_s, state_flags) == 0x84);
STATIC_ASSERT(offsetof(GIZSPINNER_s, type) == 0x8b);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x08c) == 0x8c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x094) == 0x94);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x098) == 0x98);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x09c) == 0x9c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, output_count) == 0xa4);
STATIC_ASSERT(offsetof(GIZSPINNER_s, flags) == 0xac);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x2d8) == 0x2d8);
STATIC_ASSERT(offsetof(GIZSPINNER_s, animation_points) == 0x2dc);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, spinner_count) == 0x46e4);
STATIC_ASSERT(sizeof(nuhspecial_s) == 12);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, instance_animation) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, animation) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, start_frame) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, end_frame) == 0x1c);

typedef union spinner_duration_bits_pc_u {
    f32 value;
    u32 bits;
} SPINNER_DURATION_BITS_PC;

static __declspec(noinline) void SpinnerBoundsResetPc(GIZSPINNER_s *spinner) {
    NUVEC minimum, maximum, arm_minimum, arm_maximum;
    f32 x_extent, z_extent, extent;
    NuSpecialGetBounds((NUSPECIALHANDLE *)&spinner->special, &minimum, &maximum);
    if (NuSpecialCompare((NUSPECIALHANDLE *)&thingsSceneBase, (NUSPECIALHANDLE *)&spinner->special) != 0) {
        NuSpecialGetBounds((NUSPECIALHANDLE *)&thingsSceneArm, &arm_minimum, &arm_maximum);
        NuVecAdd(&minimum, &minimum, &arm_minimum);
        NuVecAdd(&maximum, &maximum, &arm_maximum);
    }
    x_extent = maximum.x - minimum.x;
    z_extent = maximum.z - minimum.z;
    extent = x_extent > z_extent ? x_extent : z_extent;
    spinner->field_0x094 = extent;
    spinner->state_flags |= 8;
    spinner->animation_speed = 1.0f;
    spinner->type = 4;
    spinner->animation_points[0] = 1.0f;
    spinner->field_0x08c = 0;
    spinner->rotation = spinner->initial_rotation;
    spinner->output_count = 1;
    spinner->field_0x09c = (f32)((f64)spinner->field_0x094 * 0.5);
    spinner->field_0x094 = (f32)((f64)spinner->field_0x094 * (f64)0.4f);
    spinner->field_0x098 = 0.0f;
}

/* Retail553710 is exactly one RET. Definition remains real; do not substitute
 * an unresolved ref. */
void __cdecl SpinnerMissingSpecialPc(char *, char *);

i32 __cdecl GizSpinner_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZSPINNER_s *spinner;
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ *object;
    nuhspecial_s special;
    SPINNER_DURATION_BITS_PC duration;
    i32 version, index, name_length, output_points, object_count, object_index, point;
    i32 special_missing, spinner_type, spinner_flags;
    u32 spinner_state_flags;
    char special_name[32];
    f32 longest_duration = 0.0f;
    f32 animation_speed, initial_animation_point, spinner_field_98;
    f32 start_frame, end_frame, frame_duration, rounded_point;
    f64 interval;
    (void)unused;
    if (world->spinners == NULL)
        return 0;
    NuSpecialFind(things_scene, (NUSPECIALHANDLE *)&thingsSceneBase, (char *)LevelObjectNamePc00754378, 0);
    NuSpecialFind(things_scene, (NUSPECIALHANDLE *)&thingsSceneArm, (char *)LevelObjectNamePc00754364, 0);
    if (world->spinner_count != 0)
        return 0;
    version = EdFileReadInt();
    world->spinner_count = EdFileReadInt();
    for (index = 0; index < world->spinner_count; ++index) {
        animation_speed = 1.0f;
        spinner_state_flags = 0;
        spinner_flags = 0;
        special_missing = 0;
        spinner_type = 4;
        anim_set = world->spinners[index].anim_set;
        memset(&world->spinners[index], 0, sizeof(GIZSPINNER_s));
        world->spinners[index].anim_set = anim_set;
        name_length = (i8)EdFileReadChar();
        EdFileRead(world->spinners[index].name, name_length);
        EdFileReadNuVec(&world->spinners[index].position);
        {
            u16 rotation = (u16)EdFileReadShort();
            world->spinners[index].initial_rotation = rotation;
        }
        name_length = (i8)EdFileReadChar();
        EdFileRead(special_name, name_length);
        NuSpecialFind(world->current_gscn, (NUSPECIALHANDLE *)&special, special_name, 0);
        if (NuSpecialExistsFn((NUSPECIALHANDLE *)&special) == 0) {
            NuSpecialFind(things_scene, (NUSPECIALHANDLE *)&special, special_name, 0);
            if (NuSpecialExistsFn((NUSPECIALHANDLE *)&special) == 0)
                special_missing = 1;
        }
        world->spinners[index].special = special;
        output_points = (i8)EdFileReadChar();
        if (version >= 2) {
            spinner_type = (i8)EdFileReadChar();
            if (version >= 3) {
                spinner_state_flags = (u32)EdFileReadInt() & ~0x360u;
                world->spinners[index].field_0x2d8 = EdFileReadFloat();
                if (version >= 4) {
                    animation_speed = EdFileReadFloat();
                    if (version >= 6)
                        spinner_flags = (i8)EdFileReadChar();
                }
            }
        }
        GameAnimSet_RemoveAllObjects(world->spinners[index].anim_set);
        if (version >= 5) {
            GizmoFileReadGameAnimSet(world->spinners[index].anim_set, world, NULL, (u8)version,
                                     (char *)GizmoRegistrySpinnerNamePc, world->spinners[index].name);
            for (object = world->spinners[index].anim_set->objects; object != NULL; object = object->next) {
                if (world->spinners[index].primary_anim_obj == NULL) {
                    if (object->animation != NULL) {
                        world->spinners[index].primary_anim_obj = object;
                        duration.value = object->end_frame - object->start_frame;
                        duration.bits &= 0x7fffffffu;
                        longest_duration = duration.value;
                    }
                } else if (object->animation != NULL) {
                    duration.value = object->end_frame - object->start_frame;
                    duration.bits &= 0x7fffffffu;
                    if (duration.value > longest_duration) {
                        longest_duration = duration.value;
                        world->spinners[index].primary_anim_obj = object;
                    }
                }
            }
        } else {
            object_count = (i8)EdFileReadChar();
            for (object_index = 0; object_index < object_count; ++object_index) {
                start_frame = 1.0f;
                end_frame = AIPositionUnsetPc;
                name_length = (i8)EdFileReadChar();
                EdFileRead(special_name, name_length);
                NuSpecialFind(world->current_gscn, (NUSPECIALHANDLE *)&special, special_name, 0);
                if (NuSpecialExistsFn((NUSPECIALHANDLE *)&special) == 0) {
                    NuSpecialFind(things_scene, (NUSPECIALHANDLE *)&special, special_name, 0);
                    NuSpecialExistsFn((NUSPECIALHANDLE *)&special);
                }
                if (version >= 3) {
                    start_frame = EdFileReadFloat();
                    end_frame = EdFileReadFloat();
                }
                object = GameAnimSet_AddObject(world->spinners[index].anim_set, (NUSPECIALHANDLE *)&special,
                                               start_frame, end_frame, 0);
                if (world->spinners[index].primary_anim_obj == NULL) {
                    if (object != NULL && object->animation != NULL) {
                        world->spinners[index].primary_anim_obj = object;
                        duration.value = object->end_frame - object->start_frame;
                        duration.bits &= 0x7fffffffu;
                        longest_duration = duration.value;
                    }
                } else if (object != NULL && object->animation != NULL) {
                    duration.value = object->end_frame - object->start_frame;
                    duration.bits &= 0x7fffffffu;
                    if (duration.value > longest_duration) {
                        longest_duration = duration.value;
                        world->spinners[index].primary_anim_obj = object;
                    }
                }
            }
        }
        if (version >= 7) {
            for (point = 1; point <= output_points; ++point) {
                f32 value = EdFileReadFloat();
                world->spinners[index].animation_points[point] = value;
            }
            for (point = output_points + 1; point < 9; ++point)
                world->spinners[index].animation_points[point] = -1.0f;
        } else if (world->spinners[index].anim_set != NULL && world->spinners[index].primary_anim_obj != NULL) {
            object = world->spinners[index].primary_anim_obj;
            frame_duration = object->end_frame - object->start_frame;
            if (output_points > 0) {
                interval = (f64)frame_duration / (output_points - 1);
                for (point = 0; point < output_points; ++point) {
                    i32 frame_offset = (i32)((f64)point * interval);
                    rounded_point = (f32)((f64)frame_offset + object->start_frame);
                    world->spinners[index].animation_points[point + 1] = rounded_point;
                }
            }
            for (point = output_points + 1; point < 9; ++point)
                world->spinners[index].animation_points[point] = -1.0f;
        }
        spinner_field_98 = version >= 8 ? EdFileReadFloat() : 0.0f;
        initial_animation_point = version >= 9 ? EdFileReadFloat() : animation_speed;
        if (special_missing) {
            world->spinners[index].flags &= ~1u;
            SpinnerMissingSpecialPc(world->spinners[index].name, special_name);
            --world->spinner_count;
            --index;
            continue;
        }
        spinner = &world->spinners[index];
        SpinnerBoundsResetPc(spinner);
        world->spinners[index].type = (u8)spinner_type;
        world->spinners[index].animation_speed = animation_speed;
        world->spinners[index].animation_points[0] = initial_animation_point;
        world->spinners[index].output_count = (u8)output_points;
        world->spinners[index].state_flags = spinner_state_flags;
        world->spinners[index].flags = spinner_flags;
        world->spinners[index].field_0x098 = spinner_field_98;
        world->spinners[index].flags |= 1;
        world->spinners[index].flags &= 0xddu;
    }
    return 1;
}
