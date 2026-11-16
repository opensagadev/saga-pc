#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct GAMEPAD;
struct addgizmotype_s;
struct gameobject_s;
struct nuhspecial_s;

/* The techno target_mode: what kind of object target_name names (Technos_FindTgt looks it up by its kind, or
 * any kind first and records the one it found). */
#define TECHNO_TARGET_ANY 0
#define TECHNO_TARGET_CHARACTER 1 /* GetNamedGameObject */
#define TECHNO_TARGET_SPECIAL 2   /* NuSpecialFind */
#define TECHNO_TARGET_GIZMO 3     /* GizmoFindByName */

typedef struct techno_s { // size 0x70
    u8 unknown_00[0x37];  // 0x00
    u8 target_mode;       // 0x37
    u8 unknown_38[0x34];  // 0x38

    union { // size 0x4
        void *controlled_object;
        struct gameobject_s *controlled_player;
        struct nuhspecial_s *controlled_special;
    }; // 0x6c
} TECHNO;

typedef struct techno_config_s { // size 0x4c
    f32 interaction_time;        // 0x00
    i16 idle_object_ids[4];      // 0x04
    NUVEC idle_offsets[3];       // 0x0c
    i16 hand_object_ids[2];      // 0x30
    NUVEC hand_offsets[2];       // 0x34
} TECHNO_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif
    extern const char *TechnoRegistrationName;
    extern TECHNO_CONFIG TechnoSys;
    extern i32 techno_gizmotype_id;
    struct addgizmotype_s *Technos_RegisterGizmo(i32 type_id);
    extern TECHNO_CONFIG TechnoSys;
    extern const char *TechnoRegistrationName;
    extern i32 techno_gizmotype_id;
#ifdef __cplusplus
}
#endif
typedef struct TECHNO_s TECHNO_s;

struct TECHNO_s {          // size 0x70
    char name[16];         // 0x00
    NUVEC position;        // 0x10
    NUVEC ground_position; // 0x1c
    f32 ground_offset;     // 0x28
    f32 scale;             // 0x2c
    u16 y_rotation;        // 0x30
    u16 ground_x_rotation; // 0x32
    u16 ground_z_rotation; // 0x34
    u8 enabled;            // 0x36
    u8 target_mode;        // 0x37
    i32 output;            // 0x38

    union { // size 0x1
        u8 flags;

        struct {                    // size 0x1
            u8 active : 1;          // 0x3c:0
            u8 visible : 1;         // 0x3c:1
            u8 used_this_frame : 1; // 0x3c:2
            u8 complete : 1;        // 0x3c:3
            u8 reserved_flags : 4;  // 0x3c:4
        };
    }; // 0x3c

    char target_name[16]; // 0x3d

    union { // size 0x1f
        char target_object_name[31];

        struct {                           // size 0x1f
            u8 target_special_padding[19]; // 0x4d
            u8 target_special_storage[12]; // 0x60
        };
    }; // 0x4d

    void *controlled_object; // 0x6c
};

#ifdef __cplusplus
extern "C" {
#endif
    TECHNO *Technos_FindControllingTechno(struct gameobject_s *);

    i32 Techno_FindOperator(void *, struct GAMEPAD **, struct gameobject_s **);
#ifdef __cplusplus
}
#endif
