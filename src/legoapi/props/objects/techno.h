#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct GAMEPAD;
struct WORLDINFO_s;
struct addgizmotype_s;
struct gameobject_s;
struct gizmo_s;
struct nuhspecial_s;

/* The techno target_mode: what kind of object target_name names (Technos_FindTgt looks it up by its kind, or
 * any kind first and records the one it found). */
#define TECHNO_TARGET_ANY 0
#define TECHNO_TARGET_CHARACTER 1 /* GetNamedGameObject */
#define TECHNO_TARGET_SPECIAL 2   /* NuSpecialFind */
#define TECHNO_TARGET_GIZMO 3     /* GizmoFindByName */

/* TECHNO.flags, the bitfields */
#define TECHNO_FLAG_ACTIVE 0x1
#define TECHNO_FLAG_VISIBLE 0x2
#define TECHNO_FLAG_USED_THIS_FRAME 0x4 /* a character worked it this frame: the ground marker rises */
#define TECHNO_FLAG_COMPLETE 0x8        /* the gizmo output */

/* TECHNO.movement: the pad input that drives the target (the level file's byte, or an AI script's Move_*). */
#define TECHNO_MOVE_WAGGLE 0x1
#define TECHNO_MOVE_SPIN 0x2
#define TECHNO_MOVE_VERTICAL 0x4
#define TECHNO_MOVE_HORIZONTAL 0x8

typedef struct techno_s {  // size 0x70
    char name[16];         // 0x00
    NUVEC position;        // 0x10
    NUVEC ground_position; // 0x1c
    f32 ground_offset;     // 0x28
    f32 scale;             // 0x2c
    u16 y_rotation;        // 0x30
    u16 ground_x_rotation; // 0x32
    u16 ground_z_rotation; // 0x34
    u8 movement;           // 0x36
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

    union { // size 0x4
        void *controlled_object;
        struct gameobject_s *controlled_player;
        struct nuhspecial_s *controlled_special;
        struct gizmo_s *controlled_gizmo;
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

#ifdef __cplusplus
extern "C" {
#endif
    TECHNO *Technos_FindControllingTechno(struct gameobject_s *);

    i32 Techno_FindOperator(void *, struct GAMEPAD **, struct gameobject_s **);
    void *Technos_FindTgt(TECHNO *techno);
    NUVEC *Technos_TgtPos(TECHNO *techno);
    i32 GizTechno_CanUseTechno(struct gameobject_s *object, TECHNO *techno);
    void Techno_MoveCode(struct WORLDINFO_s *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
