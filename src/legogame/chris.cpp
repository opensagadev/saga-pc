// Dogfight translation unit: the space-battle levels, their flight splines, the Anakin C doors and
// the dogfight music points, compiled as one C++ unit.

#include "editor/edlevelall.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/chris.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/collect/spacelevel.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numusic/numusic.h"
#include <stdio.h>
#include "legoapi/ai/game/creature.h"
#include "nu2api/nusound/nusound.h"
static f32 MaxRadialCamY = 0;
static NUVEC4 RadialMoveCentre = {0};
static f32 RadialPlayerRadius[2];
static NUVEC4 dat_00855e40;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 ChrisFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

static void ChrisVuVecSet(NUVEC4 *out, f32 x, f32 y, f32 z, f32 w) {
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

static void ChrisVuVecSetOrigin(NUVEC4 &v) {
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    v.w = 1.0f;
}

static void ChrisVuVecAdd(NUVEC4 &destination, const NUVEC4 &a, const NUVEC4 &b) {
    destination.x = a.x + b.x;
    destination.y = a.y + b.y;
    destination.z = a.z + b.z;
}

static void ChrisVuVecSub(NUVEC4 &destination, const NUVEC4 &a, const NUVEC4 &b) {
    destination.x = a.x - b.x;
    destination.y = a.y - b.y;
    destination.z = a.z - b.z;
}

static void ChrisVuVecScale(NUVEC4 &destination, const NUVEC4 &v, f32 scale) {
    destination.x = v.x * scale;
    destination.y = v.y * scale;
    destination.z = v.z * scale;
}

/* Unit-length copy of a direction, as a point (w = 1). */
static void ChrisVuVecNormalise(NUVEC4 &destination, NUVEC4 &source) {
    NuVecNorm(&destination.xyz, &source.xyz);
    destination.w = 1.0f;
}

static void ChrisVuVecCross(NUVEC4 &destination, NUVEC4 &a, NUVEC4 &b) {
    NuVecCross(&destination.xyz, &a.xyz, &b.xyz);
}

static f32 ChrisVuVecDot(const NUVEC4 &a, const NUVEC4 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static f32 ChrisVuVecLengthSq(const NUVEC4 &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static void ChrisVuVecInvMtxRotate(NUVEC4 *destination, NUVEC4 *v, NUMTX *matrix) {
    NuVecInvMtxRotate(&destination->xyz, &v->xyz, matrix);
}

static void ChrisVuVecMtxTransform(NUVEC4 *destination, NUVEC4 *v, NUMTX *matrix) {
    NuVecMtxTransform(&destination->xyz, &v->xyz, matrix);
}

static f32 ChrisVuVecLength(const NUVEC4 &v) {
    f32 length_squared = ChrisVuVecLengthSq(v);
    return NuFsqrt(length_squared);
}

extern "C" {

    /* The PC keeps eight additional bytes at the end of each fighter record. */
    typedef struct DOGFIGHT_FIGHTER { // size 0x130
        starfighter_s fields;         // 0x000
        u8 unknown_128[8];            // 0x128
    } DOGFIGHT_FIGHTER;

    typedef struct DOGFIGHT_GROUP {   // size 0x680
        NUMTX matrix;                 // 0x000
        DOGFIGHT_FIGHTER fighters[5]; // 0x040
        NUVEC4 velocity;              // 0x630
        NUVEC4 camera_position;       // 0x640
        NUVEC4 target;                // 0x650
        u8 unknown_660[8];            // 0x660
        i32 active;                   // 0x668
        i32 state;                    // 0x66c
        u32 reset_colour;             // 0x670
        i32 draw_target;              // 0x674
        f32 duration;                 // 0x678
        f32 speed;                    // 0x67c
    } DOGFIGHT_GROUP;

    typedef struct DOGFIGHT_CROSS { // size 0x50
        NUMTX matrix;               // 0x00
        u16 flags;                  // 0x40
        u16 enabled;                // 0x42
        u8 unknown_44[4];           // 0x44
        u32 colour;                 // 0x48
        f32 scale;                  // 0x4c
    } DOGFIGHT_CROSS;

    typedef struct DOGFIGHT_ACTION { // size 0x8
        i32 command;                 // 0x0
        f32 value;                   // 0x4
    } DOGFIGHT_ACTION;

    typedef struct DOGFIGHT_DOOR { // size 0x14
        char name[12];             // 0x00
        f32 distance;              // 0x0c
        f32 timer;                 // 0x10
    } DOGFIGHT_DOOR;

    typedef struct DOGFIGHT_RESET_RECORD { // size 0x60
        u8 unknown[0x60];                  // 0x00
    } DOGFIGHT_RESET_RECORD;

    typedef struct DOGFIGHT_RESET_BUFFER { // size 0xc
        DOGFIGHT_RESET_RECORD *records;    // 0x0
        u32 capacity;                      // 0x4
        u32 used;                          // 0x8
    } DOGFIGHT_RESET_BUFFER;

    typedef struct DOGFIGHT_COIN_RECORD { // size 0x10
        i32 spline_id;                    // 0x0
        f32 spawn_time;                   // 0x4
        u8 unknown_08[8];                 // 0x8
    } DOGFIGHT_COIN_RECORD;

    typedef struct DOGFIGHT_ACTION_STATE { // size 0x30
        NUVEC4 draw_scale;                 // 0x00
        DOGFIGHT_ACTION *actions;          // 0x10
        DOGFIGHT_ACTION *current;          // 0x14
        i32 next_index;                    // 0x18
        i32 command;                       // 0x1c
        u32 unknown_20;                    // 0x20
        f32 timer;                         // 0x24
        u32 unknown_28;                    // 0x28
        struct DOGFIGHT_SPACE *owner;      // 0x2c
    } DOGFIGHT_ACTION_STATE;

    typedef struct DOGFIGHT_QUICK_BOLT { // size 0x60
        NUMTX matrix;                    // 0x00
        NUVEC4 velocity;                 // 0x40
        f32 duration;                    // 0x50
        u32 unknown_54;                  // 0x54
        i32 type;                        // 0x58
        u32 unknown_5c;                  // 0x5c
    } DOGFIGHT_QUICK_BOLT;

    typedef struct DOGFIGHT_QUICK_BUFFER { // size 0xc
        DOGFIGHT_QUICK_BOLT *records;      // 0x0
        u32 capacity;                      // 0x4
        u32 used;                          // 0x8
    } DOGFIGHT_QUICK_BUFFER;

    typedef struct DOGFIGHT_SPACE {               // size 0x64740
        DOGFIGHT_CROSS crosses[2];                // 0x00000
        DOGFIGHT_GROUP groups[8];                 // 0x000a0
        DOGFIGHT_ACTION_STATE formation;          // 0x034a0
        FLIGHT_SPLINE_RESOURCE splines[256];      // 0x034d0
        DOGFIGHT_FIGHTER queued_fighters[96];     // 0x564d0
        DOGFIGHT_RESET_RECORD reset_records[256]; // 0x5d6d0

        union { // size 0xc
            DOGFIGHT_RESET_BUFFER reset_buffer;
            DOGFIGHT_QUICK_BUFFER quick_buffer; /* the same buffer holding the quick bolts (DrawQuickBolts) */
        }; // 0x636d0

        u8 unknown_636dc[4];                    // 0x636dc
        NUVEC4 origin_start;                    // 0x636e0
        NUVEC4 origin_end;                      // 0x636f0
        NUVEC4 direction;                       // 0x63700
        f32 direction_length;                   // 0x63710
        f32 inverse_direction_length;           // 0x63714
        u8 unknown_63718[4];                    // 0x63718
        f32 door_time;                          // 0x6371c
        f32 door_countdown;                     // 0x63720
        f32 door_elapsed;                       // 0x63724
        f32 normalized_speed;                   // 0x63728
        f32 value_one_a;                        // 0x6372c
        f32 value_one_b;                        // 0x63730
        i32 coin_history_count;                 // 0x63734
        DOGFIGHT_COIN_RECORD coin_history[256]; // 0x63738
        u8 unknown_64738[8];                    // 0x64738
    } DOGFIGHT_SPACE;

    NUVEC4 DogfightDrawScale = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 DogfightResetTimer = 0.0f;
    DOGFIGHT_ACTION DogfightAnakinActions[10] = {
        {0, 1.0f}, {2, 7.5f}, {0, 1.5f}, {2, 7.5f}, {0, 7.5f}, {1, 8.0f}, {0, 4.0f}, {1, 8.0f}, {0, 8.0f}, {3, 0.0f},
    };
    DOGFIGHT_DOOR DogfightDoors[7] = {
        {"door_to_a2", 33.0359993f, 11.5f},      {"door_to_a3", 121.769997f, 31.2399998f}, {"door_to_b", 196.729996f, 60.3320007f},
        {"door_to_b2", 334.369995f, 97.558197f}, {"door_to_c", 435.119995f, 131.429993f},  {"door_to_c1", 508.679993f, 147.050003f},
        {"door_to_c2", 583.72998f, 167.5f},
    };

    typedef struct DOGFIGHT_SPEEDUP { // size 0x8
        f32 distance;                 // 0x0
        f32 speed;                    // 0x4
    } DOGFIGHT_SPEEDUP;

    f32 DogfightSpeedRate = 0.25f;
    DOGFIGHT_SPEEDUP DogfightSpeedList[9] = {
        {58.0f, 0.5f},  {72.0f, 1.0f},  {174.0f, 0.5f}, {183.0f, 1.0f}, {207.0f, 0.5f},
        {220.0f, 1.0f}, {313.0f, 0.5f}, {335.0f, 1.0f}, {0.0f, 0.0f},
    };

    f32 DogfightWingSpeed = 35.0f;
    static f32 DogfightPassingSpeed = 200.0f;

    f32 SpeedDist = 5.0f;
    f32 SpeedAdd = 1.0f;
    f32 MulDist = 0.5f;
    f32 MulAdd = 0.0f;

    extern f32 FlightSplineDefaultIncrement;

    /* These records retain the PC-only 0x130 fighter and 0x60 quick-bolt strides. */
    f32 DogfightDeathImpulseScale = 10.0f;
    f32 DogfightAimReset = 2.0f;
    f32 DogfightAimDecay = 2.0f;
    f32 DogfightFireNear = 5.0f;
    f32 DogfightFireFar = 100.0f;
    f32 MissileDist = -3.0f;
    NUVEC DogfightCachedPlayerPosition = {0.0f, 0.0f, 0.0f};
    NUVEC DogfightCachedPlayerVelocity = {0.0f, 0.0f, 0.0f};

    extern void SpaceResetAudioPoint(void);

    /* Drawing controls, each in its own section; AnakinC occupies the intervening slot. */
    SOCKPOSITION *DogfightSelectedSock = NULL;
    i32 DogfightDrawSelectedSock = 0;
    i32 DogfightHideGeometry = 0;
    struct anakin_door_s *AnakinC = 0;
    i32 DogfightDrawSpline = 0;
    i32 DogfightDrawFighterSpline = 0;
    NUVEC DogfightJetOffset = {0.15f, 0.08f, 0.32f};
    i32 DogfightBoltModelsFirst[4] = {303, 305, 301, 301};
    i32 DogfightBoltModelsSecond[4] = {304, 306, 302, 302};
    extern i32 DogfightDebrisKeys[4];

    f32 DogfightCollisionRadiusSquared = 2.0f;

    typedef __declspec(align(16)) struct DOGFIGHT_COLLISION_VECTOR { // size 0x10
        NUVEC xyz;                                                   // 0x0
    } DOGFIGHT_COLLISION_VECTOR;

    /* Anakin C door pairs; the table ends with an empty name. */
    anakin_door_setup_s DoorSetupList[15] = {
        {"door1", "door1r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door2", "door2r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door3", "door3r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door4", "door4r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door5", "door5r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door6", "door6r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door7", "door7r", 15.0f, 1.0f, 0, {0}, {1.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
        {"door8", "door8r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
        {"door9", "door9r", 15.0f, 1.0f, 0, {0}, {0.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
        {"door10", "door10r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
        {"door11", "door11r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door12", "door12r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door15", "door15r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door16", "door16r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"", NULL, 0.0f, 0.0f, 0, {0}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f},
    };

    char *FlightSplineExtension = ".FSP";
    f32 FlightSplineDefaultIncrement = 0.01f;
    f32 PodRaceSplineIncrement = 0.001f;

    void FlightSplineCapacityDiagnostic(char *filename);

    i32 DogfightDebrisKeys[4] = {-1, -1, -1, -1};

    static void CopyFlightMatrix(NUMTX *destination, const NUMTX *source);
    static void TransformFlightVector(NUVEC *output, NUVEC *input, NUMTX *matrix);
    static i32 DrawFlightCharacter(NUMTX *matrix, i32 model_id, i32 flags, f32 scale);
    void ChrisAnakinCUpdate();
    static void ProcessQuickBolts(DOGFIGHT_QUICK_BUFFER *buffer);
    static void DrawQuickBolts(DOGFIGHT_QUICK_BUFFER *buffer);
    DOGFIGHT_QUICK_BOLT *QuickBoltRecordAllocate(DOGFIGHT_QUICK_BUFFER *buffer);
    static i32 QuickBoltCreate(DOGFIGHT_QUICK_BUFFER *buffer, NUMTX *matrix, NUVEC4 *velocity, f32 duration, i32 type, i32 model_id);
    static void SampleFlightSpline(FLIGHT_SPLINE_RESOURCE *spline, NUVEC4 *points, i32 count);
    static f32 FlightSplineLegacyLength(FLIGHT_SPLINE_RESOURCE *spline);
    static f32 FlightSplineCumulativeLength(FLIGHT_SPLINE_RESOURCE *spline);
    static void FlightSplineLoadRecord(FLIGHT_SPLINE_RESOURCE *spline, i32 version, i32 index);
    static void DrawFlightSplineDebug(FLIGHT_SPLINE_RESOURCE *spline, i32 mode);
    static void DogfightResetCrosses(DOGFIGHT_CROSS *crosses, i32 count);
    static DOGFIGHT_FIGHTER *QueuedFighterSelector(DOGFIGHT_SPACE *space);
    static void DogfightResetSplineHistory(FLIGHT_SPLINE_RESOURCE *records, i32 count);
    static void DogfightResetWorldRecords(WORLDINFO *world, DOGFIGHT_SPACE *space);
    static __declspec(noinline) void DogfightResetGroup(DOGFIGHT_GROUP *group);
    static void StarFighterAlign(starfighter_s *fighter, NUVEC4 *direction, f32 distance, i32 mode);
    static i32 StarFighterFire(starfighter_s *fighter, NUVEC4 *velocity, NUMTX *aim_matrix, f32 shot_speed);
    static void StarFighterTarget(starfighter_s *fighter);
    i32 ProcessStarFighter(DOGFIGHT_FIGHTER *record, DOGFIGHT_QUICK_BUFFER *info);
    static void DrawStarFighter(DOGFIGHT_FIGHTER *record);
    static DOGFIGHT_GROUP *SelectFreeFlightGroup(void);
    static void ProcessFlightGroup(DOGFIGHT_GROUP *group, DOGFIGHT_QUICK_BUFFER *buffer);
    void SpaceAudioPoint(void);
    static void DogfightResetSpace(WORLDINFO *world, DOGFIGHT_SPACE *space);
    void SpaceRumbleProcess(void);
    static void DrawSpaceLevel(DOGFIGHT_SPACE *space);
    void ProcessCurrentSpeed(WORLDINFO *world, DOGFIGHT_SPEEDUP *entry);
    void ResetSpaceFromWorld(WORLDINFO *world);
    void ChrisAnakinBInit();
    static i32 CollideBoltStarFighter(BOLT *bolt, starfighter_s *fighter, NUVEC *position, NUVEC *velocity);
    static void DrawSelectedSock(SOCKSYS *system, i32 radius, SOCKPOSITION *position, i32 mode, i32 count);
    void ChrisAnakinCInit();
    static void SpawnFlightFighter(FLIGHT_SPLINE_RESOURCE *spline, DOGFIGHT_FIGHTER *record);
    static void ProcessFlightSpline(FLIGHT_SPLINE_RESOURCE *spline, DOGFIGHT_SPACE *space);
    void MakeWingFormation(NUVEC4 *start, NUVEC4 *target, f32 duration, i32 mode);
    static void ProcessFormationActions(DOGFIGHT_ACTION_STATE *state);
    static void ProcessSpaceLevel(DOGFIGHT_SPACE *space);

    static void CopyFlightMatrix(NUMTX *destination, const NUMTX *source) {
        destination->m00 = source->m00;
        destination->m01 = source->m01;
        destination->m02 = source->m02;
        destination->m03 = source->m03;
        destination->m10 = source->m10;
        destination->m11 = source->m11;
        destination->m12 = source->m12;
        destination->m13 = source->m13;
        destination->m20 = source->m20;
        destination->m21 = source->m21;
        destination->m22 = source->m22;
        destination->m23 = source->m23;
        destination->m30 = source->m30;
        destination->m31 = source->m31;
        destination->m32 = source->m32;
        destination->m33 = source->m33;
    }

    static void SetFlightMatrixIdentity(NUMTX *matrix) {
        CopyFlightMatrix(matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    }

    static void TransformFlightVector(NUVEC *output, NUVEC *input, NUMTX *matrix) {
        f32 x = (f64)matrix->m10 * input->y + (f64)input->x * matrix->m00 + (f64)matrix->m20 * input->z;
        f32 y = (f64)matrix->m01 * input->x + (f64)matrix->m11 * input->y + (f64)matrix->m21 * input->z;
        output->z = (f64)matrix->m02 * input->x + (f64)matrix->m12 * input->y + (f64)matrix->m22 * input->z;
        output->x = x;
        output->y = y;
    }

    void fun_0040e950() {
    }

    void fun_0040e960() {
    }

    void fun_0040e970() {
    }

    void fun_0040e980() {
    }

    void fun_0040e990() {
    }

    static i32 DrawFlightCharacter(NUMTX *matrix, i32 model_id, i32 flags, f32 scale) {
        __declspec(align(16)) NUMTX scaled;
        i32 model_index = apicharsys->playermodelids[model_id];
        if (model_index != -1) {
            if (scale != 1.0f) {
                scaled = *matrix;
                matrix = &scaled;
                NuMtxPreScaleUVU0(matrix, scale);
            }
            return GameDrawCharacterModel(&apicharsys->models[model_index], NULL, matrix, NULL, NULL, NULL, NULL, flags);
        }
        return 0;
    }

    void ChrisAnakinCUpdate() {
        anakin_door_s *door = AnakinC;
        i32 i;
        for (i = 0; i < 12; ++i, ++door) {
            NUVEC *direction;
            if (door->active == 0) {
                continue;
            }
            direction = &door->direction;
            {
                f32 minimum = door->minimum_offset;
                f32 offset = door->offset - door->speed * FRAMETIME;
                door->offset = offset;
                if (offset <= minimum) {
                    door->offset = minimum;
                }
            }
            // The transform temporaries require a 16-byte-aligned stack.
            {
                f32 offset = door->offset;
                __declspec(align(16)) NUVEC translation;
                __declspec(align(16)) NUVEC position;
                translation.x = direction->x * offset;
                translation.y = direction->y * offset;
                translation.z = direction->z * offset;
                NuVecMtxTransform(&position, &translation, &door->original_matrix);
                memcpy(&door->matrix.m30, &position, sizeof(position));
                door->matrix.m33 = 1.0f;
                NuSpecialSetDrawMtx(&door->special, &door->matrix);
                if (door->has_secondary != 0) {
                    NuVecMtxTransform(&position, &translation, &door->original_secondary_matrix);
                    memcpy(&door->secondary_matrix.m30, &position, sizeof(position));
                    door->secondary_matrix.m33 = 1.0f;
                    NuSpecialSetDrawMtx(&door->secondary_special, &door->secondary_matrix);
                }
            }
        }
    }

    i32 fun_0040eb70() {
        return 0;
    }

    void fun_0040eb80(GAMEOBJECT *object, NUMTX *matrix) {
        *matrix = object->transform;
        NuMtxPreRotateY(matrix, 0x8000);
    }

    static void InitResetBuffer(DOGFIGHT_RESET_BUFFER *buffer, DOGFIGHT_RESET_RECORD *records, u32 capacity) {
        buffer->records = records;
        buffer->capacity = capacity;
    }

    static void ClearResetBuffer(DOGFIGHT_RESET_BUFFER *buffer) {
        if (buffer != NULL && buffer->capacity != 0) {
            memset(buffer->records, 0, buffer->capacity * sizeof(*buffer->records));
            buffer->used = 0;
        }
    }

    static void ProcessQuickBolts(DOGFIGHT_QUICK_BUFFER *buffer) {
        __declspec(align(16)) NUVEC player0;
        __declspec(align(16)) NUVEC player1;
        __declspec(align(16)) NUVEC step;
        DOGFIGHT_QUICK_BOLT *bolt;
        DOGFIGHT_QUICK_BOLT *end;
        NUVEC *position;
        NUVEC *velocity;
        i32 valid0;
        i32 valid1;
        f64 dx;
        f64 dy;
        f64 dz;
        if (buffer == NULL || buffer->capacity == 0) {
            return;
        }
        if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            valid0 = 1;
            player0.x = Player[0]->transform.m30;
            player0.y = Player[0]->transform.m31;
            player0.z = Player[0]->transform.m32;
        } else {
            valid0 = 0;
        }
        if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            valid1 = 1;
            player1.x = Player[1]->transform.m30;
            player1.y = Player[1]->transform.m31;
            player1.z = Player[1]->transform.m32;
        } else {
            valid1 = 0;
        }
        bolt = buffer->records;
        end = bolt + buffer->capacity;
        for (; bolt < end; ++bolt) {
            if (bolt->duration == 0.0f) {
                continue;
            }
            velocity = &bolt->velocity.xyz;
            position = &bolt->matrix.translation;
            step.x = velocity->x * FRAMETIME;
            step.y = velocity->y * FRAMETIME;
            step.z = velocity->z * FRAMETIME;
            position->x += step.x;
            position->y += step.y;
            position->z += step.z;
            if (Player[0] != NULL && valid0 != 0) {
                dy = bolt->matrix.m31 - player0.y;
                dx = bolt->matrix.m30 - player0.x;
                dz = bolt->matrix.m32 - player0.z;
                if (dy * dy + dx * dx + dz * dz < 1.0) {
                    bolt->duration = 0.0f;
                    ObjHitObj(NULL, Player[0], 1, 0, 0, 1);
                }
            }
            if (valid1 != 0 && Player[1] != NULL) {
                dy = bolt->matrix.m31 - player1.y;
                dx = bolt->matrix.m30 - player1.x;
                dz = bolt->matrix.m32 - player1.z;
                if (dy * dy + dx * dx + dz * dz < 1.0) {
                    bolt->duration = 0.0f;
                    ObjHitObj(NULL, Player[1], 1, 0, 0, 1);
                }
            }
            bolt->duration -= FRAMETIME;
            if (bolt->duration < 0.0f) {
                bolt->duration = 0.0f;
            }
        }
    }

    static void DrawQuickBolts(DOGFIGHT_QUICK_BUFFER *buffer) {
        DOGFIGHT_QUICK_BOLT *bolt;
        DOGFIGHT_QUICK_BOLT *end;
        if (buffer == NULL || buffer->capacity == 0) {
            return;
        }
        bolt = buffer->records;
        end = bolt + buffer->capacity;
        for (; bolt < end; ++bolt) {
            if (bolt->duration != 0.0f) {
                Draw3DObjectMtx(WORLD, DogfightBoltModelsFirst[bolt->type], &bolt->matrix);
                Draw3DObjectMtx(WORLD, DogfightBoltModelsSecond[bolt->type], &bolt->matrix);
            }
        }
    }

    DOGFIGHT_QUICK_BOLT *QuickBoltRecordAllocate(DOGFIGHT_QUICK_BUFFER *buffer) {
        DOGFIGHT_QUICK_BOLT *start = buffer->records + buffer->used;
        DOGFIGHT_QUICK_BOLT *end = buffer->records + buffer->capacity;
        DOGFIGHT_QUICK_BOLT *record;
        for (record = start; record < end; ++record) {
            if (record->duration == 0.0f) {
                goto found;
            }
        }
        for (record = buffer->records; record < start; ++record) {
            if (record->duration == 0.0f) {
                goto found;
            }
        }
        return NULL;
    found:
        buffer->used = record - buffer->records;
        memset(record, 0, sizeof(*record));
        return record;
    }

    static i32 QuickBoltCreate(DOGFIGHT_QUICK_BUFFER *buffer, NUMTX *matrix, NUVEC4 *velocity, f32 duration, i32 type, i32 model_id) {
        DOGFIGHT_QUICK_BOLT *record = QuickBoltRecordAllocate(buffer);
        f64 dx;
        f64 dy;
        f64 dz;
        if (record != NULL) {
            dy = (f64)matrix->m31 - global_camera.mtx.m31;
            dx = (f64)matrix->m30 - global_camera.mtx.m30;
            dz = (f64)matrix->m32 - global_camera.mtx.m32;
            if (dx * dx + dy * dy + dz * dz < 40000.0) {
                switch (model_id) {
                    case -300:
                        PlaySfx("Dog_CloneARC170Gun", &matrix->translation);
                        break;
                    case -298:
                    case -297:
                        PlaySfx("Dog_DroidFighterBlast", &matrix->translation);
                        break;
                    case -299:
                        PlaySfx("Dog_TriFighterGuns", &matrix->translation);
                        break;
                }
            }
            record->duration = duration;
            record->matrix = *matrix;
            NuMtxPreRotateX(&record->matrix, 16384);
            record->velocity = *velocity;
            record->type = type;
            return 1;
        }
        return 0;
    }

    void CalcSplinePoint(FLIGHT_SPLINE_RESOURCE *spline, NUVEC4 *result, f32 along) {
        f32 scaled = (spline->point_count - 1) * along;
        i32 index = (i32)scaled;
        f32 fraction = scaled - index;
        NUVEC4 direction;
        NUVEC4 first;
        NUVEC4 last;
        NUVEC4 constant;
        NUVEC4 linear;
        NUVEC4 quadratic;
        NUVEC4 coefficient;
        __declspec(align(16)) NUVEC4 cubic;
        NUVEC4 *previous;
        NUVEC4 *current;
        NUVEC4 *next;
        NUVEC4 *following;
        f32 squared;
        f32 cubed;
        f32 sum;
        if (index <= 0) {
            direction.x = spline->points[0].x - spline->points[1].x;
            direction.y = spline->points[0].y - spline->points[1].y;
            direction.z = spline->points[0].z - spline->points[1].z;
            index = 0;
            ChrisVuVecNormalise(direction, direction);
            direction.x *= 10.0;
            direction.y *= 10.0;
            direction.z *= 10.0;
            first.x = spline->points[0].x + direction.x;
            first.y = spline->points[0].y + direction.y;
            first.z = spline->points[0].z + direction.z;
            first.w = spline->points[0].w;
            previous = &first;
        } else {
            if (index > spline->point_count - 1) {
                index = spline->point_count;
            }
            previous = &spline->points[index - 1];
        }
        current = &spline->points[index];
        next = &spline->points[index + 1];
        if (index >= spline->point_count - 2) {
            NUVEC4 *before_end = &spline->points[spline->point_count - 2];
            NUVEC4 *end_point = &spline->points[spline->point_count - 1];
            direction.x = end_point->x - before_end->x;
            direction.y = end_point->y - before_end->y;
            direction.z = end_point->z - before_end->z;
            NuVecNorm(&direction.xyz, &direction.xyz);
            direction.w = 1.0f;
            direction.x *= 10.0;
            direction.y *= 10.0;
            direction.z *= 10.0;
            last.x = spline->points[spline->point_count - 1].x + direction.x;
            last.y = spline->points[spline->point_count - 1].y + direction.y;
            last.z = spline->points[spline->point_count - 1].z + direction.z;
            last.w = spline->points[spline->point_count - 1].w;
            following = &last;
        } else {
            following = &spline->points[index + 2];
        }
        constant.x = current->x + current->x;
        constant.y = current->y + current->y;
        constant.z = current->z + current->z;
        linear.x = next->x - previous->x;
        linear.y = next->y - previous->y;
        linear.z = next->z - previous->z;
        linear.x *= fraction;
        linear.y *= fraction;
        linear.z *= fraction;
        quadratic.x = previous->x + previous->x;
        quadratic.y = previous->y + previous->y;
        quadratic.z = previous->z + previous->z;
        coefficient.x = current->x * 5.0;
        coefficient.y = current->y * 5.0;
        coefficient.z = current->z * 5.0;
        quadratic.x -= coefficient.x;
        quadratic.y -= coefficient.y;
        quadratic.z -= coefficient.z;
        coefficient.x = next->x * 4.0;
        coefficient.y = next->y * 4.0;
        coefficient.z = next->z * 4.0;
        quadratic.x += coefficient.x;
        quadratic.y += coefficient.y;
        quadratic.z += coefficient.z;
        quadratic.x -= following->x;
        quadratic.y -= following->y;
        quadratic.z -= following->z;
        sum = fraction * fraction;
        quadratic.x *= sum;
        quadratic.y *= sum;
        quadratic.z *= sum;
        cubic.x = current->x - next->x;
        cubic.y = current->y - next->y;
        cubic.z = current->z - next->z;
        cubic.x *= 3.0;
        cubic.y *= 3.0;
        cubic.z *= 3.0;
        cubic.x -= previous->x;
        cubic.y -= previous->y;
        cubic.z -= previous->z;
        cubic.x = cubic.x + following->x;
        cubic.y = following->y + cubic.y;
        cubic.z += following->z;
        sum *= fraction;
        cubic.x *= sum;
        cubic.y *= sum;
        cubic.z *= sum;
        sum = linear.x + constant.x;
        sum += quadratic.x;
        sum += cubic.x;
        result->x = sum * 0.5;
        sum = linear.y + constant.y;
        sum += quadratic.y;
        sum += cubic.y;
        result->y = sum * 0.5;
        sum = linear.z + constant.z;
        sum += quadratic.z;
        sum += cubic.z;
        result->z = sum * 0.5;
        result->w = (next->w - current->w) * fraction + current->w;
    }

    void CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE *spline, NUVEC4 *result, f32 distance) {
        i32 index;
        if (distance >= spline->length) {
            distance = 1.0f;
        } else {
            for (index = 0; index < spline->point_count; ++index) {
                if (spline->cumulative_distances[index] > distance) {
                    f32 start = index == 0 ? 0.0f : spline->cumulative_distances[index - 1];
                    f32 segment_fraction = (distance - start) / (spline->cumulative_distances[index] - start);
                    distance = (segment_fraction + index) / spline->point_count;
                    break;
                }
            }
        }
        CalcSplinePoint(spline, result, distance);
    }

    static void SampleFlightSpline(FLIGHT_SPLINE_RESOURCE *spline, NUVEC4 *points, i32 count) {
        f32 step;
        f32 along;
        i32 i;
        step = 1.0 / (f64)(count - 1);
        points[0] = spline->points[0];
        along = step;
        for (i = 1; i < count - 1; ++i) {
            CalcSplinePoint(spline, &points[i], along);
            along += step;
        }
        points[count - 1] = spline->points[spline->point_count - 1];
    }

    static f32 FlightSplineLegacyLength(FLIGHT_SPLINE_RESOURCE *spline) {
        f32 distance = 0.0f;
        f32 along = 1.0f;
        NUVEC4 current;
        NUVEC4 previous;
        __declspec(align(16)) NUVEC4 difference;
        f32 squared;
        if (spline->point_count != 0) {
            CalcSplinePoint(spline, &current, along);
            do {
                previous = current;
                if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
                    along -= PodRaceSplineIncrement;
                } else {
                    along -= FlightSplineDefaultIncrement;
                }
                if (along < 0.0f) {
                    along = 0.0f;
                }
                CalcSplinePoint(spline, &current, along);
                difference.x = current.x - previous.x;
                difference.y = current.y - previous.y;
                difference.z = current.z - previous.z;
                squared = ChrisVuVecLengthSq(difference);
                distance = (f32)((f64)NuFsqrt(squared) + distance);
            } while (along > 0.0f);
        }
        return distance;
    }

    static f32 FlightSplineCumulativeLength(FLIGHT_SPLINE_RESOURCE *spline) {
        f32 distance = 0.0f;
        i32 point;
        i32 sample;
        f32 point_value;
        f32 along;
        f32 squared;
        NUVEC4 previous;
        NUVEC4 current;
        __declspec(align(16)) NUVEC4 difference;
        for (point = 0; point < spline->point_count; ++point) {
            point_value = (f32)point;
            along = (f32)((f64)point_value / spline->point_count);
            CalcSplinePoint(spline, &previous, along);
            for (sample = 0; sample < 10; ++sample) {
                along = (f32)(((f64)(sample + 1) / 10.0 + point_value) / spline->point_count);
                CalcSplinePoint(spline, &current, along);
                difference.x = current.x - previous.x;
                difference.y = current.y - previous.y;
                difference.z = current.z - previous.z;
                squared = (f32)((f64)difference.x * difference.x + (f64)difference.y * difference.y + (f64)difference.z * difference.z);
                distance = (f32)((f64)NuFsqrt(squared) + distance);
                previous = current;
            }
            spline->cumulative_distances[point] = distance;
        }
        return distance;
    }

    static void FlightSplineLoadRecord(FLIGHT_SPLINE_RESOURCE *spline, i32 version, i32 index) {
        i32 point;
        spline->point_count = EdFileReadInt();
        spline->reset_spawn = EdFileReadFloat();
        spline->id = EdFileReadInt();
        spline->enabled = 1;
        if (version >= 2) {
            spline->spawn_interval = EdFileReadFloat();
            spline->repeat_limit = EdFileReadInt();
        } else {
            spline->repeat_limit = 0;
            spline->spawn_interval = 0.0f;
        }
        if (version >= 3) {
            spline->target_id = EdFileReadInt();
            spline->object_id = EdFileReadInt();
        } else {
            spline->target_id = -1;
            spline->object_id = index;
        }
        for (point = 0; point < spline->point_count; ++point) {
            spline->points[point].x = EdFileReadFloat();
            spline->points[point].y = EdFileReadFloat();
            spline->points[point].z = EdFileReadFloat();
            spline->points[point].w = EdFileReadFloat();
        }
    }

    void FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE *splines, i32 capacity) {
        char filename[256];
        i32 version;
        i32 count;
        i32 i;
        FLIGHT_SPLINE_RESOURCE *spline;
        sprintf(filename, "%s%s", world->config_file, FlightSplineExtension);
        EdFileSetMedia(1);
        if (!EdFileOpen(filename, 0)) {
            return;
        }
        version = EdFileReadInt();
        count = EdFileReadInt();
        if (count > capacity) {
            FlightSplineCapacityDiagnostic(filename);
        }
        for (i = 0; i < count; ++i) {
            FlightSplineLoadRecord(&splines[i], version, i);
        }
        if (version > 3) {
            for (i = 0; i < count; ++i) {
                spline = &splines[i];
                spline->length = EdFileReadFloat();
                if (version > 4) {
                    i32 point;
                    for (point = 0; point < spline->point_count; ++point) {
                        spline->cumulative_distances[point] = EdFileReadFloat();
                    }
                } else {
                    spline->length = FlightSplineCumulativeLength(&splines[i]);
                }
            }
        } else {
            for (i = 0; i < count; ++i) {
                splines[i].length = FlightSplineLegacyLength(&splines[i]);
            }
        }
        for (; i < capacity; ++i) {
            splines[i].point_count = 0;
        }
        EdFileClose();
    }

    /* The debug paths keep these vector evaluations without line submission calls. */
    static void DrawFlightSplineDebug(FLIGHT_SPLINE_RESOURCE *spline, i32 mode) {
        NUVEC4 points[512];
        __declspec(align(16)) NUVEC4 direction;
        __declspec(align(16)) NUVEC4 axis;
        i32 count;
        i32 i;
        i32 from;
        i16 yaw;
        i16 pitch;
        if (mode == 1 || (mode != 2 && mode != 3)) {
            return;
        }
        count = 20;
        if (mode == 3) {
            count = spline->point_count * 8;
        }
        SampleFlightSpline(spline, points, count);
        for (i = 1; i < count; ++i) {
            if (mode == 3) {
                {
                    NUVEC4 *next = &points[i];
                    NUVEC4 *previous = &points[i - 1];
                    direction.x = next->x - (f64)previous->x;
                    direction.y = next->y - (f64)previous->y;
                    direction.z = next->z - (f64)previous->z;
                }
                yaw = (i16)NuAtan2D(direction.x, direction.z);
                NuVecRotateY(&axis.xyz, &direction.xyz, -yaw);
                pitch = (i16)NuAtan2D(axis.y, axis.z);
                axis.x = 1.0f;
                axis.y = 0.0f;
                axis.z = 0.0f;
                axis.w = 0.0f;
                NuVecRotateZ(&axis.xyz, &axis.xyz, (i16)(i32)points[i].w);
                NuVecRotateX(&axis.xyz, &axis.xyz, -pitch);
                NuVecRotateY(&axis.xyz, &axis.xyz, yaw);
            }
        }
        for (i = 0; i < spline->point_count; ++i) {
            from = (i32)((f32)(i * count) / (spline->point_count - 1) - 0.5);
            if (from < 0) {
                from = 0;
            } else if (from > count - 2) {
                from = count - 2;
            }
            {
                NUVEC4 *previous = &points[from];
                NUVEC4 *next = &points[from + 1];
                direction.x = next->x - (f64)previous->x;
                direction.y = next->y - (f64)previous->y;
                direction.z = next->z - (f64)previous->z;
            }
            yaw = (i16)NuAtan2D(direction.x, direction.z);
            NuVecRotateY(&axis.xyz, &direction.xyz, -yaw);
            pitch = (i16)NuAtan2D(axis.y, axis.z);
            axis.x = 2.0f;
            axis.y = 0.0f;
            axis.z = 0.0f;
            axis.w = 0.0f;
            NuVecRotateZ(&axis.xyz, &axis.xyz, (i16)(i32)spline->points[i].w);
            NuVecRotateX(&axis.xyz, &axis.xyz, -pitch);
            NuVecRotateY(&axis.xyz, &axis.xyz, yaw);
        }
    }

    void fun_0040fd50(void *unused, NUVEC *position) {
        memcpy(position, &WORLD->dogfight_space->origin_end, sizeof(NUVEC));
    }

    f32 dat_007f15e4 = 120.0f;
    f32 dat_007f15e8 = 150.0f;
    f32 dat_007f15ec = 1.0f;
    f32 dat_007f15f0 = 1.0f;
    f32 dat_007f15f4 = 0.6f;

    void ChrisRadialCam(NUVEC *position, NUVEC *target) {
        __declspec(align(16)) NUVEC new_position = *position;
        __declspec(align(16)) NUVEC new_target = *target;
        __declspec(align(16)) NUVEC position_delta;
        __declspec(align(16)) NUVEC origin_delta;
        f32 position_radius;
        f32 base_radius;
        f32 target_radius;

        position_delta.y = 0.0f;
        position_delta.x = new_position.x - RadialMoveCentre.x;
        position_delta.z = new_position.z - RadialMoveCentre.z;
        position_radius =
            NuFsqrt(position_delta.x * position_delta.x + position_delta.y * position_delta.y + position_delta.z * position_delta.z);
        origin_delta.x = dat_00855e40.x - RadialMoveCentre.x;
        origin_delta.y = 0.0f;
        origin_delta.z = dat_00855e40.z - RadialMoveCentre.z;
        RadialPlayerRadius[0] =
            NuFsqrt(origin_delta.x * origin_delta.x + origin_delta.y * origin_delta.y + origin_delta.z * origin_delta.z);

        if (RadialPlayerRadius[0] < dat_007f15e4) {
            base_radius = dat_007f15e4;
        } else {
            base_radius = MIN(dat_007f15e8, RadialPlayerRadius[0]);
        }
        if (position_radius != 0.0f) {
            target_radius = base_radius + (RadialPlayerRadius[0] - base_radius) * dat_007f15ec;
            f32 scale = target_radius / position_radius;
            position_delta.x *= scale;
            position_delta.z *= scale;
        }
        new_position.x = RadialMoveCentre.x + position_delta.x;
        new_position.z = RadialMoveCentre.z + position_delta.z;
        *position = new_position;

        if (RadialPlayerRadius[0] < base_radius) {
            target_radius = base_radius + (RadialPlayerRadius[0] - base_radius) * dat_007f15f0;
        } else {
            target_radius = base_radius + (RadialPlayerRadius[0] - base_radius) * dat_007f15f4;
        }
        if (RadialPlayerRadius[0] != 0.0f) {
            f32 scale = target_radius / RadialPlayerRadius[0];
            origin_delta.x *= scale;
            origin_delta.z *= scale;
        }
        new_target.x = RadialMoveCentre.x + origin_delta.x;
        new_target.z = RadialMoveCentre.z + origin_delta.z;
        *target = new_target;
    }

    static void DogfightResetCrosses(DOGFIGHT_CROSS *crosses, i32 count) {
        i32 i;
        DOGFIGHT_CROSS *cross = crosses;
        for (i = 0; i < count; ++i, ++cross) {
            cross->enabled = 0;
            cross->matrix.m00 = 80.0f;
            cross->matrix.m01 = 55.0f;
            cross->matrix.m02 = 200.0f;
            cross->matrix.m03 = 1.0f;
            cross->matrix.m33 = 1.0f;
            cross->matrix.m30 = 0.0f;
            cross->matrix.m31 = 0.0f;
            cross->matrix.m32 = 0.0f;
            cross->matrix.m10 = 0.0f;
            cross->matrix.m11 = 0.0f;
            cross->matrix.m12 = 0.0f;
            cross->matrix.m13 = 1.0f;
            cross->matrix.m23 = 1.0f;
            cross->matrix.m20 = 0.0f;
            cross->matrix.m21 = 0.0f;
            cross->matrix.m22 = 0.0f;
            cross->colour = 0xffffff;
            cross->matrix.m12 = cross->matrix.m02;
            cross->flags = 0;
            cross->scale = 5.0f;
        }
    }

    static DOGFIGHT_FIGHTER *QueuedFighterSelector(DOGFIGHT_SPACE *space) {
        i32 i;
        for (i = 0; i < 96; ++i) {
            if (space->queued_fighters[i].fields.active == 0) {
                return &space->queued_fighters[i];
            }
        }
        return NULL;
    }

    static void DogfightResetSplineHistory(FLIGHT_SPLINE_RESOURCE *records, i32 count) {
        i32 i;
        for (i = 0; i < count; ++i, ++records) {
            records->next_spawn = records->reset_spawn;
            records->remaining = records->repeat_limit;
        }
    }

    static void DogfightResetWorldRecords(WORLDINFO *world, DOGFIGHT_SPACE *space) {
        ChrisVuVecSet(&DogfightDrawScale, 4.0f, 4.0f, 4.0f, 1.0f);
        space->formation.draw_scale.x = DogfightOriginEndXPc;
        space->formation.draw_scale.y = DogfightOriginEndY;
        space->formation.draw_scale.z = DogfightOriginEndZ;
        space->formation.draw_scale.w = 1.0f;
        space->formation.actions = NULL;
        if (world->current_level != DOGFIGHTA_LDATA) {
            space->formation.actions = DogfightAnakinActions;
        }
        ClearResetBuffer(&space->reset_buffer);
        space->formation.current = NULL;
        space->formation.next_index = 0;
        space->formation.command = 4;
        space->formation.owner = space;
        space->value_one_a = 1.0f;
        space->value_one_b = 1.0f;
    }

    static void DogfightResetFighters(DOGFIGHT_FIGHTER *fighters, i32 count) {
        i32 i;
        for (i = 0; i < count; ++i) {
            fighters[i].fields.active = 0;
            fighters[i].fields.parent = NULL;
            fighters[i].fields.spline = NULL;
        }
    }

    static void DogfightResetGroup(DOGFIGHT_GROUP *group) {
        DogfightResetFighters(group->fighters, 5);
        group->reset_colour = 0xff00;
        group->active = 0;
        group->draw_target = 0;
    }

    static void StarFighterAlign(starfighter_s *fighter, NUVEC4 *direction, f32 distance, i32 mode) {
        NUVEC4 local;
        __declspec(align(16)) NUVEC4 up;
        i16 yaw;
        i16 pitch;
        i16 world_yaw;
        i16 world_pitch;
        i16 roll;
        f64 limit;
        f32 adjustment;
        ChrisVuVecInvMtxRotate(&local, direction, &fighter->matrix);
        yaw = (i16)NuAtan2D(local.x, local.z);
        fighter->yaw = yaw;
        NuVecRotateY(&local.xyz, &local.xyz, -yaw);
        pitch = (i16)NuAtan2D(local.y, local.z);
        fighter->pitch = pitch;
        NuMtxPreRotateY(&fighter->matrix, yaw);
        NuMtxPreRotateX(&fighter->matrix, -pitch);
        if (mode != 0) {
            NuMtxPreRotateZ(&fighter->matrix, 512);
            return;
        }
        world_yaw = (i16)NuAtan2D(direction->x, direction->z);
        NuVecRotateY(&up.xyz, &direction->xyz, -world_yaw);
        world_pitch = (i16)NuAtan2D(up.y, up.z);
        up.x = 1.0f;
        up.w = up.z = up.y = 0.0f;
        NuVecRotateZ(&up.xyz, &up.xyz, (i16)(i32)fighter->target_position.w + 16384);
        NuVecRotateX(&up.xyz, &up.xyz, -world_pitch);
        NuVecRotateY(&up.xyz, &up.xyz, world_yaw);
        ChrisVuVecInvMtxRotate(&up, &up, &fighter->matrix);
        roll = (i16)NuAtan2D(up.x, up.y);
        limit = (f64)FRAMETIME * 24000.0;
        if (roll > limit) {
            roll = (i16)(i32)limit;
        }
        limit = (f64)FRAMETIME * -24000.0;
        if (roll < limit) {
            roll = (i16)(i32)limit;
        }
        adjustment = roll;
        if (distance > 2.0f) {
            adjustment = 0.0f;
        } else if (distance > 1.0f) {
            adjustment = (2.0 - (f64)distance) * adjustment;
        }
        NuMtxPreRotateZ(&fighter->matrix, -(i16)(i32)adjustment);
    }

    /* Return 0: prepared shot; 1: lost target; 2: target retained without a shot. */
    static i32 StarFighterFire(starfighter_s *fighter, NUVEC4 *velocity, NUMTX *aim_matrix, f32 shot_speed) {
        __declspec(align(16)) NUVEC4 offset;
        __declspec(align(16)) NUVEC4 local;
        NUVEC *target_position;
        NUVEC *target_velocity;
        NUVEC4 *axis;
        NUVEC4 *front;
        NUVEC4 *right;
        NUVEC4 *up;
        GAMEOBJECT *player;
        f32 forward;
        f32 square;
        f32 ratio;
        f32 closing_speed;
        f32 travel_time;
        f32 least;
        f32 up_dot;
        f32 front_dot;
        f32 bias;
        i16 yaw;
        i16 pitch;

        union {
            f32 value;
            u32 bits;
        } absolute;

        if (fighter->target_id == -1) {
            return 1;
        }
        if (fighter->target_id == -2) {
            if ((NuRandFloat() < 0.5f && (player = Player[0]) != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) ||
                ((player = Player[1]) != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0)) {
                DogfightCachedPlayerPosition = player->collision_position;
                DogfightCachedPlayerVelocity = player->velocity;
            }
            target_position = &DogfightCachedPlayerPosition;
            target_velocity = &DogfightCachedPlayerVelocity;
        } else if (fighter->target_id == -3) {
            *aim_matrix = fighter->matrix;
            velocity->y = velocity->x = 0.0f;
            velocity->z = shot_speed;
            velocity->w = 0.0f;
            TransformFlightVector(&velocity->xyz, &velocity->xyz, aim_matrix);
            return 0;
        } else {
            if (fighter->target == NULL || fighter->target->active == 0) {
                return 1;
            }
            target_position = &fighter->target->matrix.translation;
            target_velocity = &fighter->velocity.xyz;
        }
        front = &fighter->matrix.rows[2];
        offset.x = target_position->x - fighter->matrix.m30;
        offset.y = target_position->y - fighter->matrix.m31;
        offset.z = target_position->z - fighter->matrix.m32;
        forward = (f64)front->y * offset.y + (f64)front->x * offset.x + (f64)front->z * offset.z;
        if (forward < DogfightFireNear || forward > DogfightFireFar) {
            return 1;
        }
        square = (f64)offset.y * offset.y + (f64)offset.x * offset.x + (f64)offset.z * offset.z;
        ratio = (f64)forward * forward / square;
        if (ratio < (f64)0.969f) {
            return 2;
        }
        closing_speed = (f64)front->y * target_velocity->y + (f64)target_velocity->x * front->x + (f64)front->z * target_velocity->z;
        closing_speed = 50.0 - (f64)closing_speed;
        if (closing_speed < 1.0f) {
            return 2;
        }
        travel_time = forward / closing_speed;
        local.x = (f64)target_velocity->x * travel_time;
        local.y = (f64)target_velocity->y * travel_time;
        local.z = (f64)target_velocity->z * travel_time;
        ChrisVuVecAdd(offset, offset, local);
        if (fighter->fired == 0) {
            right = &fighter->matrix.rows[0];
            up = &fighter->matrix.rows[1];
            least = ChrisFabs((f64)right->y * offset.y + (f64)right->x * offset.x + (f64)right->z * offset.z);
            absolute.value = (f64)up->y * offset.y + (f64)up->x * offset.x + (f64)up->z * offset.z;
            absolute.bits &= 0x7fffffff;
            up_dot = absolute.value;
            absolute.value = ChrisVuVecDot(*front, offset);
            absolute.bits &= 0x7fffffff;
            front_dot = absolute.value;
            axis = front;
            if (up_dot > least) {
                if (front_dot > least) {
                    axis = right;
                }
            } else if (front_dot > up_dot) {
                axis = up;
            }
            ChrisVuVecCross(fighter->aim_direction, offset, *axis);
            ChrisVuVecNormalise(fighter->aim_direction, fighter->aim_direction);
        }
        if (fighter->aim_bias != 0.0f) {
            bias = fighter->aim_bias;
            ChrisVuVecScale(local, fighter->aim_direction, bias);
            ChrisVuVecAdd(offset, offset, local);
        }
        *aim_matrix = fighter->matrix;
        NuVecInvMtxRotate(&local.xyz, &offset.xyz, aim_matrix);
        yaw = (i16)NuAtan2D(local.x, local.z);
        NuVecRotateY(&local.xyz, &local.xyz, -yaw);
        pitch = (i16)NuAtan2D(local.y, local.z);
        NuMtxPreRotateY(aim_matrix, yaw);
        NuMtxPreRotateX(aim_matrix, -pitch);
        velocity->y = velocity->x = 0.0f;
        velocity->z = shot_speed;
        velocity->w = 0.0f;
        TransformFlightVector(&velocity->xyz, &velocity->xyz, aim_matrix);
        return 0;
    }

    static void StarFighterTarget(starfighter_s *fighter) {
        DOGFIGHT_SPACE *space = WORLD->dogfight_space;
        FLIGHT_SPLINE_RESOURCE *spline = fighter->spline;
        DOGFIGHT_FIGHTER *record;
        i32 target_id;
        i32 i;
        if (spline == NULL) {
            return;
        }
        target_id = spline->target_id;
        if (target_id == -1) {
            fighter->target_id = -1;
            fighter->target_ready = 0;
            fighter->target = NULL;
            return;
        }
        if (target_id == -2) {
            fighter->target_id = (i16)target_id;
            fighter->target = NULL;
            fighter->target_ready = 1;
            return;
        }
        if (target_id == -3) {
            fighter->target_id = (i16)target_id;
            fighter->target = NULL;
            fighter->target_ready = 1;
            return;
        }
        fighter->target_ready = 0;
        fighter->target_id = -1;
        record = space->queued_fighters;
        for (i = 0; i < 96; ++i, ++record) {
            if (record->fields.active != 0 && record->fields.object_id == target_id) {
                fighter->target_id = (i16)target_id;
                fighter->target = &record->fields;
                fighter->target_ready = 1;
                return;
            }
        }
    }

    i32 ProcessStarFighter(DOGFIGHT_FIGHTER *record, DOGFIGHT_QUICK_BUFFER *info) {
        starfighter_s *fighter = &record->fields;
        DOGFIGHT_GROUP *parent;
        DOGFIGHT_FIGHTER *missile;
        NUVEC4 shot_velocity;
        __declspec(align(16)) NUVEC4 direction;
        NUMTX aim_matrix;
        f32 magnitude;
        f32 scale;
        f32 distance_squared;
        f32 camera_distance;
        f32 pitch;
        f32 distance;
        f32 speed;
        f64 dx;
        f64 dy;
        f64 dz;
        i32 finished;
        i32 fire_result;
        if (Player[0] != NULL && Player[0]->sock_position.distance > 435.0) {
            MissileDist = 2.0f;
        }
        if (fighter->hit_count != 0) {
            if (fighter->spline != NULL && (fighter->spline->id == 85 || fighter->spline->id == 84)) {
                fighter->health = 0;
            } else {
                --fighter->health;
            }
            --fighter->hit_count;
            if (fighter->health == 0) {
                if (fighter->spline != NULL && (fighter->spline->id == 85 || fighter->spline->id == 84)) {
                    fighter->death_timer = 1.0f;
                } else {
                    fighter->death_timer = 2.0f;
                }
                parent = (DOGFIGHT_GROUP *)fighter->parent;
                if (parent != NULL) {
                    direction.x = fighter->matrix.m30 - parent->matrix.m30;
                    direction.y = fighter->matrix.m31 - parent->matrix.m31;
                    direction.z = fighter->matrix.m32 - parent->matrix.m32;
                    parent->target = *(NUVEC4 *)&fighter->matrix.m30;
                } else {
                    *&direction.xyz = v010;
                }
                magnitude = (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
                magnitude = NuFsqrt(magnitude);
                if (magnitude != 0.0f) {
                    scale = qrand() * (((f64)DogfightDeathImpulseScale + 1.0) / 65535.0) / magnitude;
                    direction.x *= scale;
                    direction.y *= scale;
                    direction.z *= scale;
                }
                fighter->velocity.x += direction.x;
                fighter->velocity.y += direction.y;
                fighter->velocity.z += direction.z;
                fighter->death_rotate_x = qrand() * 1.0000152587890625 - 32768.0;
                fighter->death_rotate_z = qrand() * 1.0000152587890625 - 32768.0;
                if (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84)) {
                    AddGameDebris(WORLD->game_debris_sys, 23, &fighter->matrix.translation);
                }
            }
            if (fighter->health == -2) {
                fighter->death_timer = 0.0f;
            }
        }
        if (fighter->health <= 0 && (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84))) {
            fighter->death_timer -= FRAMETIME;
            if (fighter->death_timer <= 0.0f) {
                AddGameDebris(WORLD->game_debris_sys, 23, &fighter->matrix.translation);
                fighter->active = 0;
            } else {
                NUVEC step;
                NuMtxPreRotateX(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_x * FRAMETIME * 4.0));
                NuMtxPreRotateZ(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_z * FRAMETIME * 4.0));
                step.x = (f64)fighter->velocity.x * FRAMETIME;
                step.y = (f64)fighter->velocity.y * FRAMETIME;
                step.z = (f64)fighter->velocity.z * FRAMETIME;
                fighter->matrix.m30 += step.x;
                fighter->matrix.m31 += step.y;
                fighter->matrix.m32 += step.z;
                AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[48].effect, &fighter->matrix.translation, 1, 0, 0);
            }
            return 0;
        }
        parent = (DOGFIGHT_GROUP *)fighter->parent;
        if (parent != NULL) {
            fighter->matrix = parent->matrix;
            fighter->velocity = parent->velocity;
            NuVecMtxTransform(&fighter->matrix.translation, &fighter->parent_offset.xyz, &parent->matrix);
            fighter->shoot_timer -= FRAMETIME;
            if (fighter->shoot_timer <= 0.0f) {
                NUVEC4 parent_position;
                parent_position.x = parent_position.y = 0.0f;
                parent_position.z = -10.0f;
                parent_position.w = 1.0f;
                ChrisVuVecMtxTransform(&parent_position, &parent_position, &fighter->matrix);
                fighter->shoot_timer = 1.0f;
            }
            return 1;
        }
        if (fighter->spline == NULL) {
            return 0;
        }
        finished = 0;
        direction.x = fighter->target_position.x - fighter->matrix.m30;
        direction.y = fighter->target_position.y - fighter->matrix.m31;
        direction.z = fighter->target_position.z - fighter->matrix.m32;
        distance_squared = (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
        if (fighter->death_timer >= 0.0f) {
            fighter->death_timer -= FRAMETIME;
        }
        while (distance_squared < 25.0) {
            fighter->spline_progress += FlightSplineDefaultIncrement;
            if (fighter->spline_progress > 1.0f) {
                finished = 1;
                fighter->spline_progress -= 1.0;
                distance_squared = 1000000.0f;
                break;
            }
            CalcSplinePoint(fighter->spline, &fighter->target_position, fighter->spline_progress);
            if (fighter->model_id == -307) {
                fighter->target_position.x += (f64)fighter->matrix.m10 * MissileDist;
                fighter->target_position.y += (f64)fighter->matrix.m11 * MissileDist;
                fighter->target_position.z += (f64)fighter->matrix.m12 * MissileDist;
            }
            direction.x = fighter->target_position.x - fighter->matrix.m30;
            direction.y = fighter->target_position.y - fighter->matrix.m31;
            direction.z = fighter->target_position.z - fighter->matrix.m32;
            distance_squared = (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
        }
        camera_distance = ((f64)fighter->matrix.m31 - global_camera.mtx.m31) * global_camera.mtx.m21 +
                          ((f64)fighter->matrix.m30 - global_camera.mtx.m30) * global_camera.mtx.m20 +
                          ((f64)fighter->matrix.m32 - global_camera.mtx.m32) * global_camera.mtx.m22;
        if (camera_distance >= 0.0f) {
            camera_distance = 1.0f;
        }
        if (camera_distance < -2.0f) {
            pitch = 0.5f;
        } else {
            pitch = (f64)camera_distance * 0.25 + 1.0;
        }
        switch (fighter->model_id) {
            case -300:
                PlaySfxAndSetPitch("Dog_CloneARC170EngLp", &fighter->matrix.translation, pitch);
                break;
            case -299:
                if (fighter->health > 0) {
                    PlaySfxAndSetPitch("Dog_TriFighterEngLp", &fighter->matrix.translation, pitch);
                }
                break;
            case -298:
            case -297:
                if (fighter->health > 0) {
                    PlaySfxAndSetPitch("Dog_DroidFighterEngLp", &fighter->matrix.translation, pitch);
                }
                break;
        }
        if (finished != 0) {
            fighter->active = 0;
        } else {
            distance = NuFsqrt(distance_squared);
            if (distance != 0.0f) {
                speed = fighter->speed / distance;
                fighter->velocity.x = (f64)direction.x * speed;
                fighter->velocity.y = (f64)direction.y * speed;
                fighter->velocity.z = (f64)direction.z * speed;
                direction.x = (f64)fighter->velocity.x * FRAMETIME;
                direction.y = (f64)fighter->velocity.y * FRAMETIME;
                direction.z = (f64)fighter->velocity.z * FRAMETIME;
                fighter->matrix.m30 += direction.x;
                fighter->matrix.m31 += direction.y;
                fighter->matrix.m32 += direction.z;
                StarFighterAlign(fighter, &direction, (f64)distance / 5.0, fighter->death_timer > 0.0f);
                fighter->movement = direction;
            }
        }
        fighter->matrix.m33 = 1.0f;
        if (fighter->delete_when_done == 0) {
            if (fighter->initial_delay != 0.0f) {
                fighter->initial_delay -= FRAMETIME;
                if (fighter->initial_delay <= 0.0f) {
                    fighter->initial_delay = 0.0f;
                    fighter->aim_bias = DogfightAimReset;
                }
                return 1;
            }
            if (fighter->target_ready == 0) {
                StarFighterTarget(fighter);
                if (fighter->target_ready == 0) {
                    return 1;
                }
            }
            if (fighter->model_id == -307) {
                return 1;
            }
            if (fighter->fired != 0 && fighter->aim_bias > 0.0f) {
                fighter->aim_bias -= (f64)DogfightAimDecay * FRAMETIME;
            }
            if (fighter->shoot_timer > 0.0f) {
                fighter->shoot_timer -= FRAMETIME;
                return 1;
            }
            fighter->shoot_timer = NuRandFloat() * (f64)0.2f + (f64)0.2f;
            if (info == NULL) {
                return 1;
            }
            fire_result = StarFighterFire(fighter, &shot_velocity, &aim_matrix, 75.0f);
            if (fire_result != 0) {
                if (fire_result == 1 && fighter->fired != 0) {
                    fighter->delete_when_done = 1;
                }
                return 1;
            }
            if (fighter->missile_count != 0 && fighter->spline->id != 85 && fighter->spline->id != 84 && NuRandFloat() < (f64)0.2f) {
                missile = QueuedFighterSelector(WORLD->dogfight_space);
                if (missile != NULL) {
                    --fighter->missile_count;
                    *missile = *record;
                    missile->fields.speed += 10.0;
                    missile->fields.model_id = -307;
                    missile->fields.matrix.m30 += (f64)missile->fields.matrix.m10 * MissileDist;
                    missile->fields.matrix.m31 += (f64)missile->fields.matrix.m11 * MissileDist;
                    missile->fields.matrix.m32 += (f64)missile->fields.matrix.m12 * MissileDist;
                    dx = (f64)missile->fields.matrix.m30 - global_camera.mtx.m30;
                    dy = (f64)missile->fields.matrix.m31 - global_camera.mtx.m31;
                    dz = (f64)missile->fields.matrix.m32 - global_camera.mtx.m32;
                    if (dy * dy + dx * dx + dz * dz < 40000.0) {
                        PlaySfx("Ep3_1_ProtoXWingMissile", &missile->fields.matrix.translation);
                    }
                }
            }
            QuickBoltCreate(info, &aim_matrix, &shot_velocity, 2.5f, fighter->model_id == -300, fighter->model_id);
            fighter->fired = 1;
        }
        return 1;
    }

    static void DrawStarFighter(DOGFIGHT_FIGHTER *record) {
        starfighter_s *fighter = &record->fields;
        __declspec(align(16)) NUMTX scaled;
        if (fighter->model_id < 0) {
            if (fighter->model_id == -297 || fighter->model_id == -299 || fighter->model_id == -298 || fighter->model_id == -307) {
                scaled = fighter->matrix;
                scaled.m00 *= (f64)1.15f;
                scaled.m01 *= (f64)1.15f;
                scaled.m02 *= (f64)1.15f;
                scaled.m10 *= (f64)1.15f;
                scaled.m11 *= (f64)1.15f;
                scaled.m12 *= (f64)1.15f;
                scaled.m20 *= (f64)1.15f;
                scaled.m21 *= (f64)1.15f;
                scaled.m22 *= (f64)1.15f;
                NuSpecialDrawAt(&WORLD->level_objects[-fighter->model_id].special, &scaled);
            } else {
                NuSpecialDrawAt(&WORLD->level_objects[-fighter->model_id].special, &fighter->matrix);
            }
            if (fighter->model_id == -307) {
                if (fighter->model_id == -307) {
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[49].effect, &fighter->matrix.translation, 1, 0, 0);
                }
            }
        } else {
            DrawFlightCharacter(&fighter->matrix, fighter->model_id, fighter->draw_flags, fighter->scale);
        }
        if (DogfightDrawFighterSpline != 0 && fighter->health > 0 && fighter->parent == NULL && DogfightDrawSpline != 0 &&
            fighter->spline != NULL && fighter->spline->point_count != 0) {
            DrawFlightSplineDebug(fighter->spline, 3);
        }
    }

    static DOGFIGHT_GROUP *SelectFreeFlightGroup(void) {
        i32 i;
        if (WORLD->level_specific_data != NULL) {
            for (i = 0; i < 8; ++i) {
                if (WORLD->dogfight_space->groups[i].active == 0) {
                    return &WORLD->dogfight_space->groups[i];
                }
            }
        }
        return NULL;
    }

    static void ProcessFlightGroup(DOGFIGHT_GROUP *group, DOGFIGHT_QUICK_BUFFER *buffer) {
        __declspec(align(16)) NUVEC step;
        f32 speed;
        i32 i;
        i32 active = 0;
        if (group->active == 0) {
            return;
        }
        if ((u32)group->state <= 1) {
            if (group->state == 0) {
                NUVEC4 *velocity = &group->velocity;
                NuVecSub(&velocity->xyz, &GameCam->render_mtx.translation, &group->matrix.translation);
                velocity->w = 1.0f;
                NuVecNorm(&velocity->xyz, &velocity->xyz);
                velocity->w = 1.0f;
                speed = group->speed;
                velocity->x *= speed;
                velocity->y *= speed;
                velocity->z *= speed;
            }
            step.x = (f64)group->velocity.x * FRAMETIME;
            step.y = (f64)group->velocity.y * FRAMETIME;
            step.z = (f64)group->velocity.z * FRAMETIME;
            {
                NUVEC *position = &group->matrix.translation;
                position->x += step.x;
                position->y += step.y;
                position->z += step.z;
            }
            NuVecInvMtxTransform(&group->camera_position.xyz, &group->matrix.translation, &GameCam->render_mtx);
            group->duration -= FRAMETIME;
            if (group->duration < 0.0f || (group->camera_position.z > 800.0 && group->state == 1) ||
                (group->camera_position.z < 0.0f && group->state == 0)) {
                group->active = 0;
            }
        }
        for (i = 0; i < 5; ++i) {
            if (group->fighters[i].fields.active != 0) {
                active |= ProcessStarFighter(&group->fighters[i], buffer);
            }
        }
        if (active == 0) {
            group->draw_target = 1;
        }
        if (group->draw_target != 0) {
            step.x = (f64)group->velocity.x * FRAMETIME;
            step.y = (f64)group->velocity.y * FRAMETIME;
            step.z = (f64)group->velocity.z * FRAMETIME;
            group->target.x = group->matrix.m30 + (f64)step.x;
            group->target.y = group->matrix.m31 + (f64)step.y;
            group->target.z = group->matrix.m32 + (f64)step.z;
        }
    }

    void SpaceAudioPoint(void) {
        music_man.SetTrackEntryTimeByClass(2, WORLD->dogfight_space->door_time);
        music_man.SetTrackEntryTimeByClass(0x20, WORLD->dogfight_space->door_time);
    }

    void SpaceResetAudioPoint(void) {
        GAMEOBJECT *second = Player[1];
        GAMEOBJECT *first = Player[0];
        i32 i;
        for (i = 6; i != -1; --i) {
            if ((first != NULL && first->sock_position.distance > DogfightDoors[i].distance) ||
                (second != NULL && second->sock_position.distance > DogfightDoors[i].distance)) {
                music_man.SetTrackEntryTimeByClass(2, DogfightDoors[i].timer);
                music_man.SetTrackEntryTimeByClass(0x20, DogfightDoors[i].timer);
                break;
            }
        }
        if (i == -1) {
            SetPreSeekStartPoint(0.0f);
        }
    }

    static void DogfightResetSpace(WORLDINFO *world, DOGFIGHT_SPACE *space) {
        NUVEC4 *direction;
        f32 value;
        i32 i = 6;
        GAMEOBJECT *first;
        GAMEOBJECT *second;
        space->origin_end.x = DogfightOriginEndXPc;
        space->origin_end.y = DogfightOriginEndY;
        space->origin_end.z = DogfightOriginEndZ;
        space->origin_end.w = 0.0f;
        space->origin_start.x = DogfightOriginStartX;
        space->origin_start.y = 326.5f;
        space->origin_start.z = -394.0f;
        space->origin_start.w = 0.0f;
        direction = &space->direction;
        direction->x = space->origin_end.x - space->origin_start.x;
        direction->y = space->origin_end.y - space->origin_start.y;
        direction->z = space->origin_end.z - space->origin_start.z;
        direction->x *= (f64)1.1f;
        direction->y *= (f64)1.1f;
        direction->z *= (f64)1.1f;
        space->origin_end.x = space->direction.x + space->origin_start.x;
        space->origin_end.y = space->origin_start.y + space->direction.y;
        space->origin_end.z = space->origin_start.z + space->direction.z;
        value = (f32)((f64)space->direction.x * space->direction.x + (f64)space->direction.y * space->direction.y +
                      (f64)space->direction.z * space->direction.z);
        value = NuFsqrt(value);
        space->direction_length = value;
        space->inverse_direction_length = (f32)(1.0 / value);
        second = Player[1];
        first = Player[0];
        for (; i != -1; --i) {
            if (first != NULL && first->sock_position.distance > DogfightDoors[i].distance) {
                break;
            }
            if (second != NULL && second->sock_position.distance > DogfightDoors[i].distance) {
                break;
            }
        }
        // Computes the door times first and clears them when no door was found.
        value = DogfightDoors[i].timer;
        space->door_time = value;
        value = (f32)((f64)value * (f64)94.977417f / (f64)59.449684f);
        space->door_countdown = value;
        space->door_elapsed = (f32)(((f64)value - FRAMETIME) * (f64)94.977417f / (f64)59.449684f);
        if (i == -1) {
            space->door_countdown = 0.0f;
            space->door_time = 0.0f;
            space->door_elapsed = 0.0f;
        }
        for (i = 0; i < 8; ++i) {
            DogfightResetGroup(&space->groups[i]);
        }
        DogfightResetWorldRecords(world, space);
        DogfightResetCrosses(space->crosses, 2);
        DogfightResetFighters(space->queued_fighters, 96);
        DogfightResetSplineHistory(space->splines, 256);
        DogfightResetTimer = (f32)((f64)NuRandFloat() * 10.0 + 3.0);
    }

    void SpaceRumbleProcess(void) {
        NUVEC origin;
        NUMTX matrix;
        NUVEC4 ray;
        GAMEOBJECT *player;
        f32 nearest = 35.0f;
        i32 i;

        player = Player[0];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            origin = player->position;
        } else {
            player = Player[1];
            if (player != NULL) {
                origin = player->position;
            }
        }
        NuMtxSetRotationY(&matrix, 0x2000);
        NuMtxRotateZ(&matrix, 0x2000);
        ray.w = 1.0f;
        for (i = 0; i < 12; ++i) {
            switch (i) {
                case 0:
                case 6:
                    ray.x = -35.0f;
                    ray.y = 0.0f;
                    ray.z = 0.0f;
                    break;
                case 1:
                case 7:
                    ray.x = 35.0f;
                    ray.y = 0.0f;
                    ray.z = 0.0f;
                    break;
                case 2:
                case 8:
                    ray.y = -35.0f;
                    ray.x = 0.0f;
                    ray.z = 0.0f;
                    break;
                case 3:
                case 9:
                    ray.y = 35.0f;
                    ray.x = 0.0f;
                    ray.z = 0.0f;
                    break;
                case 4:
                case 10:
                    ray.z = -35.0f;
                    ray.x = 0.0f;
                    ray.y = 0.0f;
                    break;
                case 5:
                case 11:
                    ray.z = 35.0f;
                    ray.x = 0.0f;
                    ray.y = 0.0f;
                    break;
            }
            if (i > 5) {
                NuVec4MtxTransformVU0(&ray, &ray, &matrix);
            }
            if (NewRayCastScaleYMask(&origin, &ray.xyz, 0.0f, 1.0f, 0, 0) != 0) {
                f32 squared = (f32)((f64)ray.x * ray.x + (f64)ray.y * ray.y + (f64)ray.z * ray.z);
                ray.x = NuFsqrt(squared);
                if (ray.x < nearest) {
                    nearest = ray.x;
                }
            }
        }
        if (nearest < 35.0f) {
            f64 amount = 1.0 - (f64)nearest / 35.0;
            f32 strength;
            GameCam_NewShake(GameCam, (f32)((f64)MulDist * amount + MulAdd), 0.2f, (f32)((f64)SpeedDist * amount + SpeedAdd));
            strength = (f32)(amount * 0.5);
            if (strength > 0.35f) {
                strength = 0.35f;
            }
            NewRumbleAllPlayers(strength, 0.0f, 0, 0);
        }
        DogfightResetTimer -= FRAMETIME;
        if (DogfightResetTimer < 0.0f) {
            GameCam_Judder(GameCam, 0.2f, 2, NULL);
            NewRumbleAllPlayers(0.3f, 0.0f, 0, 0);
            DogfightResetTimer = (f32)((f64)NuRandFloat() * 10.0 + 3.0);
        }
    }

    void fun_00411f00() {
        DogfightDebrisKeys[0] = -1;
        DogfightDebrisKeys[1] = -1;
        DogfightDebrisKeys[2] = -1;
        DogfightDebrisKeys[3] = -1;
    }

    static void DrawStarFighterGroup(DOGFIGHT_GROUP *group) {
        i32 i;
        if (group->active != 0) {
            for (i = 0; i < 5; ++i) {
                if (group->fighters[i].fields.active != 0) {
                    DrawStarFighter(&group->fighters[i]);
                }
            }
        }
    }

    static void DrawSpaceLevel(DOGFIGHT_SPACE *space) {
        GAMEOBJECT *player;
        NUMTX matrix;
        i32 i;
        i32 j;
        if (DogfightHideGeometry == 0) {
            for (i = 0; i < 2; ++i) {
                if (space->crosses[i].enabled != 0) {
                    NuVecMtxTransform(&space->crosses[i].matrix.translation, (NUVEC *)&space->crosses[i].matrix.m10, &GameCam->render_mtx);
                }
            }
            for (i = 0; i < 8; ++i) {
                DrawStarFighterGroup(&space->groups[i]);
            }
            for (i = 0; i < 96; ++i) {
                if (space->queued_fighters[i].fields.active != 0) {
                    DrawStarFighter(&space->queued_fighters[i]);
                }
            }
            DrawQuickBolts(&space->quick_buffer);
        }
        for (i = 0; i < 4; ++i) {
            player = Player[i >> 1];
            if (player == NULL) {
                continue;
            }
            if (player->id != (u16)id_JEDISTARFIGHTERYELLOWEP3 && player->id != (u16)id_JEDISTARFIGHTERREDEP3) {
                if (DogfightDebrisKeys[i] != -1) {
                    DebFreeInstantly(&DogfightDebrisKeys[i]);
                }
            } else if (DogfightDebrisKeys[i] == -1) {
                AddDebrisEffect(&DogfightDebrisKeys[i], WORLD->game_debris_sys->entries[47].effect, 0.0f, 0.0f, 0.0f);
            } else {
                matrix.m00 = player->transform.m00;
                matrix.m01 = player->transform.m01;
                matrix.m02 = player->transform.m02;
                matrix.m03 = player->transform.m03;
                matrix.m10 = player->transform.m20;
                matrix.m11 = player->transform.m21;
                matrix.m12 = player->transform.m22;
                matrix.m13 = player->transform.m23;
                matrix.m20 = -player->transform.m10;
                matrix.m21 = -player->transform.m11;
                matrix.m22 = -player->transform.m12;
                matrix.m23 = -player->transform.m13;
                matrix.m30 = player->transform.m30;
                matrix.m31 = player->transform.m31;
                matrix.m32 = player->transform.m32;
                matrix.m33 = player->transform.m33;
                if ((i & 1) != 0) {
                    matrix.m30 += ((f64)matrix.m20 * DogfightJetOffset.y - (f64)matrix.m00 * DogfightJetOffset.x) +
                                  (f64)matrix.m10 * DogfightJetOffset.z;
                    matrix.m31 += ((f64)matrix.m21 * DogfightJetOffset.y - (f64)matrix.m01 * DogfightJetOffset.x) +
                                  (f64)matrix.m11 * DogfightJetOffset.z;
                    matrix.m32 += ((f64)matrix.m22 * DogfightJetOffset.y - (f64)matrix.m02 * DogfightJetOffset.x) +
                                  (f64)matrix.m12 * DogfightJetOffset.z;
                } else {
                    matrix.m30 += ((f64)matrix.m20 * DogfightJetOffset.y + (f64)matrix.m00 * DogfightJetOffset.x) +
                                  (f64)matrix.m10 * DogfightJetOffset.z;
                    matrix.m31 += ((f64)matrix.m21 * DogfightJetOffset.y + (f64)matrix.m01 * DogfightJetOffset.x) +
                                  (f64)matrix.m11 * DogfightJetOffset.z;
                    matrix.m32 += ((f64)matrix.m22 * DogfightJetOffset.y + (f64)matrix.m02 * DogfightJetOffset.x) +
                                  (f64)matrix.m12 * DogfightJetOffset.z;
                }
                DebrisPosOrientationMtx(DogfightDebrisKeys[i], &matrix);
            }
        }
    }

    void fun_00412200() {
    }

    void ProcessCurrentSpeed(WORLDINFO *world, DOGFIGHT_SPEEDUP *entry) {
        DOGFIGHT_SPACE *space = world->dogfight_space;
        while (entry->distance != 0.0f) {
            if (space->door_elapsed < entry->distance && space->door_countdown >= entry->distance) {
                space->value_one_a = entry->speed;
            }
            ++entry;
        }
        if (space->value_one_a > space->value_one_b) {
            space->value_one_b += FRAMETIME * DogfightSpeedRate;
            if (space->value_one_b > space->value_one_a) {
                space->value_one_b = space->value_one_a;
            }
        } else if (space->value_one_a < space->value_one_b) {
            space->value_one_b -= FRAMETIME * DogfightSpeedRate;
            if (space->value_one_b < space->value_one_a) {
                space->value_one_b = space->value_one_a;
            }
        }
        world->sock_sys->socks[0].current_speed = space->value_one_b * 20.0;
        if (world->current_level == DOGFIGHTA_LDATA && world->sock_sys->socks[0].current_speed != 0.0f) {
            space->normalized_speed = world->sock_sys->socks[0].current_speed / 11.0;
        }
    }

    /* One of four identical WORLD adapters. */
    void ResetSpaceFromWorld(WORLDINFO *world) {
        DogfightResetSpace(world, world->dogfight_space);
    }

    void fun_00412370() {
        DrawSpaceLevel(WORLD->dogfight_space);
    }

    void fun_00412390(WORLDINFO *world) {
        DogfightResetSpace(world, world->dogfight_space);
    }

    void ChrisAnakinBInit() {
        nuhspecial_s centre;
        ChrisVuVecSetOrigin(RadialMoveCentre);
        MaxRadialCamY = 8.36f;
        NuSpecialList(WORLD->current_gscn);

        if (NuSpecialFind(WORLD->current_gscn, &centre, "Centre", 1) != 0 && NuSpecialExistsFn(&centre) != 0) {
            NUVEC *position = NuSpecialGetPos(&centre);
            memcpy(&RadialMoveCentre, position, sizeof(NUVEC));
        }
    }

    void fun_00412450() {
    }

    void fun_00412460() {
    }

    void fun_00412470() {
    }

    void fun_00412480() {
        DrawSpaceLevel(WORLD->dogfight_space);
    }

    void fun_004124a0(WORLDINFO *world) {
        DogfightResetSpace(world, world->dogfight_space);
    }

    void fun_004124c0(WORLDINFO *world) {
        DogfightResetSpace(world, world->dogfight_space);
    }

    void fun_004124e0() {
    }

    void ChrisDogFightAReset(WORLDINFO *world) {
        SpaceResetAudioPoint();
        ResetSpaceFromWorld(world);
        BOLT_OVERRIDE_PLAYERBOLTSPEED = 150.0f;
        BOLT_OVERRIDE_PLAYERBOLTDURATION = 1.5f;
        DogfightDebrisKeys[0] = -1;
        DogfightDebrisKeys[1] = -1;
        DogfightDebrisKeys[2] = -1;
        DogfightDebrisKeys[3] = -1;
        music_man.StopTrack(2, 0);
        music_man.StopTrack(0x20, 0);
    }

    void fun_00412560() {
    }

    void fun_00412570() {
    }

    void ChrisAllocLevelStuff(WORLDINFO *world) {
        world->has_level_specific_data = 1;
        if (world->current_level == DOGFIGHTA_LDATA) {
            DOGFIGHT_SPACE *space;
            world->level_specific_data = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(DOGFIGHT_SPACE));
            space = world->dogfight_space;
            InitResetBuffer(&space->reset_buffer, space->reset_records, 256);
            world->dogfight_space->normalized_speed = 1.0f;
            if (world->current_level == DOGFIGHTA_LDATA && world->sock_sys->socks[0].current_speed != 0.0f) {
                world->dogfight_space->normalized_speed = (f32)((f64)world->sock_sys->socks[0].current_speed / 11.0);
            }
            world->dogfight_space->coin_history_count = 0;
        } else if (world->current_level == PODRACEA_LDATA || world->current_level == PODRACEB_LDATA ||
                   world->current_level == dat_0087b0b4) {
            world->level_specific_data = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 0xb030);
        } else {
            world->has_level_specific_data = 0;
        }
    }

    void fun_00412680(WORLDINFO *world) {
        world->level_specific_data = NULL;
        if (AnakinC == NULL) {
            AnakinC = NULL;
        }
        world->level_specific_data = NULL;
    }

    i32 ShipDropCoins(starfighter_s *fighter) {
        DOGFIGHT_SPACE *space = WORLD->dogfight_space;
        i32 index;
        for (index = 0; index < space->coin_history_count; ++index) {
            if (space->coin_history[index].spline_id == fighter->spline->id &&
                space->coin_history[index].spawn_time == fighter->spawn_time) {
                return 0;
            }
        }
        if (index >= 256) {
            return 0;
        }
        space->coin_history[index].spline_id = fighter->spline->id;
        WORLD->dogfight_space->coin_history[index].spawn_time = fighter->spawn_time;
        ++WORLD->dogfight_space->coin_history_count;
        return 1;
    }

    static i32 CollideBoltStarFighter(BOLT *bolt, starfighter_s *fighter, NUVEC *position, NUVEC *velocity) {
        NUVEC relative_position;
        __declspec(align(16)) NUVEC relative_velocity;
        __declspec(align(16)) NUVEC step;
        NUVEC *ship_position = &fighter->matrix.translation;
        f32 a;
        f32 dot;
        f32 b;
        f32 c;
        f32 root;
        f32 enter;
        f32 leave;
        f32 time;
        f64 denominator;
        f64 dx;
        f64 dy;
        f64 dz;
        i32 coins;
        i32 player_index;
        relative_position.x = position->x - ship_position->x;
        relative_position.y = position->y - ship_position->y;
        relative_position.z = position->z - ship_position->z;
        relative_velocity.x = velocity->x - fighter->velocity.x;
        relative_velocity.y = velocity->y - fighter->velocity.y;
        relative_velocity.z = velocity->z - fighter->velocity.z;
        a = (f64)relative_velocity.y * relative_velocity.y + (f64)relative_velocity.x * relative_velocity.x +
            (f64)relative_velocity.z * relative_velocity.z;
        dot = (f64)relative_position.y * relative_velocity.y + (f64)relative_position.x * relative_velocity.x +
              (f64)relative_position.z * relative_velocity.z;
        b = dot + dot;
        c = (f64)relative_position.y * relative_position.y + (f64)relative_position.x * relative_position.x +
            (f64)relative_position.z * relative_position.z;
        c -= DogfightCollisionRadiusSquared;
        if (a > 0.0f) {
            c = (f64)b * b - ((f64)a * 4.0) * c;
            if (!(c >= 0.0f)) {
                return 0;
            }
            root = NuFsqrt(c);
            denominator = (f64)a + a;
            enter = (-(f64)b - root) / denominator;
            time = -FRAMETIME;
            leave = ((f64)root - b) / denominator;
            if (!(leave >= time) || !(enter <= 0.0f)) {
                return 0;
            }
            if (enter < time) {
                enter = time;
            }
            step.x = (f64)relative_velocity.x * enter;
            step.y = (f64)relative_velocity.y * enter;
            step.z = (f64)relative_velocity.z * enter;
            position->x += step.x;
            position->y += step.y;
            position->z += step.z;
        } else if (!(c <= 0.0f)) {
            return 0;
        }
        if (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84)) {
            BoltSys->debris(bolt, position, 0, &fighter->velocity.xyz, 0);
            bolt->active = 0;
            if (ShipDropCoins(fighter) != 0) {
                coins = fighter->model_id == -299 ? 500 : 1000;
            } else {
                coins = 0;
            }
            player_index = bolt->owner != NULL ? bolt->owner->player_index : -1;
            AddPickups(coins, ReleaseHearts(), 0, 0, ship_position, NULL, 2.0f, player_index, 1.0f, 2000000.0f, NULL, 1, 1);
            AddFiniteShotPART(PARTLookupType("DogBits"), ship_position, 1);
            dy = (f64)fighter->matrix.m31 - global_camera.mtx.m31;
            dx = (f64)fighter->matrix.m30 - global_camera.mtx.m30;
            dz = (f64)fighter->matrix.m32 - global_camera.mtx.m32;
            if (dy * dy + dx * dx + dz * dz < 40000.0) {
                if (fighter->health > 0) {
                    switch (fighter->model_id) {
                        case -299:
                            PlaySfx("Dog_TriFighterHit", ship_position);
                            break;
                        case -298:
                        case -297:
                            PlaySfx("Dog_DroidFighterHit", ship_position);
                            break;
                    }
                } else {
                    PlaySfx("Ep3_1_ExplosionXXL", ship_position);
                }
            }
        }
        ++fighter->hit_count;
        return 1;
    }

    i32 ChrisExtraBoltCollision(BOLT *bolt, NUVEC *points) {
        DOGFIGHT_SPACE *space;
        DOGFIGHT_COLLISION_VECTOR position;
        DOGFIGHT_COLLISION_VECTOR velocity;
        i32 group;
        i32 i;
        if (WORLD->has_level_specific_data == 0) {
            return 0;
        }
        space = WORLD->dogfight_space;
        if (space == NULL || (bolt->flags & (BOLT_PLAYER_1 | BOLT_PLAYER_2)) == 0) {
            return 0;
        }
        position.xyz = points[1];
        velocity.xyz = bolt->velocity;
        for (group = 0; group < 8; ++group) {
            if (space->groups[group].active != 0) {
                for (i = 0; i < 5; ++i) {
                    if (space->groups[group].fighters[i].fields.active != 0 &&
                        CollideBoltStarFighter(bolt, &space->groups[group].fighters[i].fields, &position.xyz, &velocity.xyz) != 0) {
                        return 1;
                    }
                }
            }
        }
        for (i = 0; i < 96; ++i) {
            if (space->queued_fighters[i].fields.active != 0 &&
                CollideBoltStarFighter(bolt, &space->queued_fighters[i].fields, &position.xyz, &velocity.xyz) != 0) {
                return 1;
            }
        }
        return 0;
    }

    static void DrawSelectedSock(SOCKSYS *system, i32 radius, SOCKPOSITION *position, i32 mode, i32 count) {
        SOCK *sock;
        __declspec(align(16)) NUVEC a;
        __declspec(align(16)) NUVEC b;
        __declspec(align(16)) NUVEC c;
        __declspec(align(16)) NUVEC d;
        __declspec(align(16)) NUVEC difference;
        i32 i;
        i32 end;
        f32 square;
        if (position->sock_index == -1 || mode != 5) {
            return;
        }
        sock = &system->socks[position->sock_index];
        a = sock->a->pts[0];
        b = sock->b->pts[0];
        c = sock->c->pts[0];
        d = sock->d->pts[0];
        difference.x = (f64)c.x + b.x;
        difference.y = (f64)c.y + b.y;
        difference.z = (f64)c.z + b.z;
        difference.x -= a.x;
        difference.y -= a.y;
        difference.z -= a.z;
        difference.x -= d.x;
        difference.y -= d.y;
        difference.z -= d.z;
        ChrisVuVecLength(*(NUVEC4 *)&difference);
        difference.x = (f64)b.x + a.x;
        difference.y = (f64)b.y + a.y;
        difference.z = (f64)b.z + a.z;
        difference.x -= c.x;
        difference.y -= c.y;
        difference.z -= c.z;
        difference.x -= d.x;
        difference.y -= d.y;
        difference.z -= d.z;
        square = (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
        (void)NuFsqrt(square);
        i = position->segment - (count >> 1) * radius;
        end = i + radius * count;
        for (; i < end; ++i) {
            a = sock->a->pts[i];
            b = sock->b->pts[i];
            c = sock->c->pts[i];
            d = sock->d->pts[i];
            difference.x = (f64)c.x + b.x;
            difference.y = (f64)c.y + b.y;
            difference.z = (f64)c.z + b.z;
            difference.x -= a.x;
            difference.y -= a.y;
            difference.z -= a.z;
            difference.x -= d.x;
            difference.y -= d.y;
            difference.z -= d.z;
            square = (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
            (void)NuFsqrt(square);
            difference.x = (f64)b.x + a.x;
            difference.y = (f64)b.y + a.y;
            difference.z = (f64)b.z + a.z;
            difference.x -= c.x;
            difference.y -= c.y;
            difference.z -= c.z;
            difference.x -= d.x;
            difference.y -= d.y;
            difference.z -= d.z;
            square = (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
            (void)NuFsqrt(square);
        }
    }

    i32 fun_00412f90() {
        return 0;
    }

    void fun_00412fa0() {
    }

    void ChrisAnakinCReset() {
        struct anakin_door_setup_s *setup;
        anakin_door_s *door = AnakinC;
        i32 count = 0;
        for (setup = DoorSetupList; count < 12; ++setup) {
            if (setup->name == NULL) {
                for (; count < 12; ++count, ++door) {
                    door->active = 0;
                }
                return;
            }
            if (!NuSpecialFind(WORLD->current_gscn, &door->special, setup->name, 1) || !NuSpecialExistsFn(&door->special)) {
                continue;
            }

            if (NuSpecialFind(WORLD->current_gscn, &door->secondary_special, setup->secondary_name, 1)) {
                door->has_secondary = ((i16)(NuSpecialExistsFn(&door->secondary_special)));
            }

            SetFlightMatrixIdentity(&door->original_matrix);
            door->matrix = door->original_matrix = *NuSpecialGetMtx(&door->special);
            if (door->has_secondary != 0) {
                SetFlightMatrixIdentity(&door->original_secondary_matrix);
                door->secondary_matrix = door->original_secondary_matrix = *NuSpecialGetMtx(&door->secondary_special);
            }
            door->platform_id = ((i16)(FindPlatInst(NuSpecialGetInstanceix(&door->special))));
            door->active = 1;
            door->flags = ((u8)(setup->flags));
            door->direction = setup->direction;
            door->offset = setup->initial_offset;
            door->speed = setup->speed;
            door->minimum_offset = setup->minimum_offset;
            door->open_delay = setup->open_delay;
            ++door;
            ++count;
        }
    }

    void ChrisAnakinCInit() {
        NuSpecialList(WORLD->current_gscn);
        ChrisAnakinCReset();
    }

    void fun_00413230(GAMEOBJECT *object, NUMTX *matrix) {
        __declspec(align(16)) NUMTX rotated = object->transform;
        *matrix = rotated;
        NuMtxPreRotateY(matrix, 0x8000);
    }

    static void SpawnFlightFighter(FLIGHT_SPLINE_RESOURCE *spline, DOGFIGHT_FIGHTER *record) {
        starfighter_s *fighter = &record->fields;
        __declspec(align(16)) NUVEC4 direction;
        i32 model;
        if (spline->point_count == 0) {
            return;
        }
        SetFlightMatrixIdentity(&fighter->matrix);
        *(NUVEC4 *)&fighter->matrix.m30 = spline->points[0];
        direction.x = spline->points[1].x - (f64)spline->points[0].x;
        direction.y = spline->points[1].y - (f64)spline->points[0].y;
        direction.z = spline->points[1].z - (f64)spline->points[0].z;
        StarFighterAlign(fighter, &direction, 1.0f, 0);
        ChrisVuVecSetOrigin(fighter->velocity);
        fighter->target_position = spline->points[0];
        fighter->spline_progress = 0.0f;
        fighter->initial_delay = 0.0f;
        fighter->shoot_timer = 0.0f;
        fighter->active = 1;
        fighter->parent = NULL;
        fighter->spline = spline;
        fighter->health = 1;
        fighter->hit_count = 0;
        fighter->fired = 0;
        fighter->delete_when_done = 0;
        model = spline->id;
        if (model == 87) {
            fighter->model_id = -297;
        } else if (model == 86) {
            fighter->model_id = -299;
        } else if (model == 54) {
            fighter->model_id = -298;
        } else if (model == 85) {
            fighter->model_id = -300;
        } else if (model == 84) {
            fighter->model_id = -300;
        } else {
            fighter->model_id = (i16)model;
        }
        fighter->scale = 1.0f;
        fighter->draw_flags = 1;
        fighter->speed = 50.0f;
        fighter->object_id = spline->object_id;
        fighter->target_id = (i16)spline->target_id;
        fighter->missile_count = 1;
        model = spline->id;
        if (model == 85 || model == 84 || model == 86) {
            fighter->scale = 1.0f;
            fighter->speed = 22.5f;
        } else if (model == 54) {
            fighter->scale = 2.0f;
            fighter->speed = 50.0f;
        } else if (model == 87) {
            fighter->scale = 1.0f;
            fighter->speed = 22.5f;
        } else if (model == -307) {
            fighter->scale = 1.0f;
            fighter->speed = 33.0f;
        }
    }

    static void ProcessFlightSpline(FLIGHT_SPLINE_RESOURCE *spline, DOGFIGHT_SPACE *space) {
        DOGFIGHT_FIGHTER *fighter;
        if (spline->point_count == 0) {
            return;
        }
        if (!(space->door_elapsed < spline->next_spawn && space->door_countdown >= spline->next_spawn)) {
            return;
        }
        fighter = QueuedFighterSelector(space);
        fighter->fields.spawn_time = spline->next_spawn;
        SpawnFlightFighter(spline, fighter);
        if (spline->spawn_interval != 0.0f) {
            if (spline->repeat_limit != 0) {
                --spline->remaining;
                if (spline->remaining == 0) {
                    spline->next_spawn = -1.0f;
                    return;
                }
            }
            spline->next_spawn += spline->spawn_interval;
        } else {
            spline->next_spawn = -1.0f;
        }
    }

    void MakeWingFormation(NUVEC4 *start, NUVEC4 *target, f32 duration, i32 mode) {
        DOGFIGHT_GROUP *group = SelectFreeFlightGroup();
        __declspec(align(16)) NUVEC4 offset;
        u16 angle;
        u16 step;
        i32 i;
        f32 speed;
        NUVEC4 *velocity;
        if (group == NULL) {
            return;
        }
        group->active = 1;
        group->draw_target = 0;
        SetFlightMatrixIdentity(&group->matrix);
        group->matrix = GameCam->render_mtx;
        *(NUVEC4 *)&group->matrix.m30 = *start;
        if (mode != 0) {
            group->state = 0;
            NuMtxPreRotateY(&group->matrix, 0x8000);
            group->speed = DogfightWingSpeed;
        } else {
            group->state = 1;
            group->speed = DogfightPassingSpeed;
        }
        velocity = &group->velocity;
        ChrisVuVecSub(*velocity, *target, *start);
        ChrisVuVecNormalise(*velocity, *velocity);
        speed = group->speed;
        ChrisVuVecScale(*velocity, *velocity, speed);
        group->duration = duration;
        offset.x = -5.0f;
        offset.y = 0.0f;
        offset.z = 0.0f;
        offset.w = 0.0f;
        angle = (u16)qrand();
        step = (u16)(qrand() / 21 + 0x2666);
        for (i = 0; i < 5; ++i) {
            starfighter_s *fighter = &group->fighters[i].fields;
            NuVecRotateZ(&fighter->parent_offset.xyz, &offset.xyz, (i16)angle);
            angle = (u16)(angle + step);
            fighter->active = 1;
            fighter->scale = 2.0f;
            fighter->parent = (struct spacelevel_flight_group_s *)group;
            fighter->health = 1;
            fighter->hit_count = 0;
            fighter->spline = NULL;
            fighter->model_id = 0x36;
            fighter->draw_flags = 1;
        }
    }

    static void ProcessFormationActions(DOGFIGHT_ACTION_STATE *state) {
        NUVEC4 start;
        __declspec(align(16)) NUVEC4 target;
        if (state->actions == NULL) {
            return;
        }
        if (state->command == 4) {
            state->current = &state->actions[state->next_index];
            ++state->next_index;
            state->command = state->current->command;
            if (state->command == 3) {
                state->current = state->actions;
                state->next_index = 1;
                state->command = state->actions[0].command;
            }
            switch (state->command) {
                case 2:
                    start.x = -10.0f;
                    start.y = -5.0f;
                    start.z = -10.0f;
                    start.w = 1.0f;
                    NuVecRotateZ(&start.xyz, &start.xyz, qrand());
                    ChrisVuVecMtxTransform(&start, &start, &GameCam->render_mtx);
                    target.x = 0.0f;
                    target.y = 0.0f;
                    target.z = 400.0f;
                    target.w = 1.0f;
                    ChrisVuVecMtxTransform(&target, &target, &GameCam->render_mtx);
                    MakeWingFormation(&start, &target, state->current->value, 0);
                    state->command = 4;
                    break;
                case 1:
                    *&target.xyz = *&GameCam->render_mtx.translation;
                    start.x = 0.0f;
                    start.y = 0.0f;
                    start.z = 300.0f;
                    start.w = 1.0f;
                    ChrisVuVecMtxTransform(&start, &start, &GameCam->render_mtx);
                    MakeWingFormation(&start, &target, state->current->value, 1);
                    state->command = 4;
                    break;
                case 0:
                    state->timer = state->current->value;
                    break;
            }
        }
        if (state->command == 0) {
            state->timer -= FRAMETIME;
            if (state->timer <= 0.0f) {
                state->command = 4;
            }
        }
    }

    static void ProcessSpaceLevel(DOGFIGHT_SPACE *space) {
        DOGFIGHT_QUICK_BUFFER *buffer;
        GAMEOBJECT *player0;
        GAMEOBJECT *player1;
        DOOR_s *door;
        i32 i;
        i32 j;
        space->door_elapsed = space->door_countdown;
        space->door_time += FRAMETIME;
        space->door_countdown += (f64)space->normalized_speed * FRAMETIME;
        ProcessFormationActions(&space->formation);
        buffer = &space->quick_buffer;
        for (i = 0; i < 8; ++i) {
            ProcessFlightGroup(&space->groups[i], buffer);
        }
        ProcessQuickBolts(buffer);
        for (i = 0; i < 256; ++i) {
            ProcessFlightSpline(&space->splines[i], space);
        }
        for (i = 0; i < 96; ++i) {
            if (space->queued_fighters[i].fields.active != 0) {
                ProcessStarFighter(&space->queued_fighters[i], buffer);
            }
        }
        player1 = Player[1];
        player0 = Player[0];
        for (i = 6; i != -1; --i) {
            if ((player0 != NULL && player0->sock_position.distance > DogfightDoors[i].distance) ||
                (player1 != NULL && player1->sock_position.distance > DogfightDoors[i].distance)) {
                door = WORLD->doors;
                for (j = 0; j < WORLD->door_count; ++j, ++door) {
                    if (NuStrICmp((char *)door, DogfightDoors[i].name) == 0) {
                        Doors_SetLastDoor(door);
                        break;
                    }
                }
                break;
            }
        }
        if (WORLD->area == BATTLEOVERCORUSCANT_ADATA) {
            SpaceRumbleProcess();
        }
    }

    void fun_00413a10() {
        ProcessSpaceLevel(WORLD->dogfight_space);
    }

    /* This entry is implemented without installing its original level callback. */
    void ChrisDogFightAInit(WORLDINFO *world) {
        i32 i;
        ChrisAllocLevelStuff(world);
        DogfightResetSpace(world, world->dogfight_space);
        for (i = 0; i < 256; ++i) {
            ((FLIGHT_SPLINE_RESOURCE *)world->dogfight_space->splines)[i].point_count = 0;
        }
        if (world->current_level == DOGFIGHTA_LDATA) {
            FlightSpline_Init(world, (FLIGHT_SPLINE_RESOURCE *)world->dogfight_space->splines, 256);
        }
        DogfightResetSplineHistory(WORLD->dogfight_space->splines, 256);
    }

    void ChrisDogFightAUpdate(WORLDINFO *world) {
        u8 *flags = LevFlag.bytes;
        SpaceResetAudioPoint();
        ProcessCurrentSpeed(world, DogfightSpeedList);
        ProcessSpaceLevel(world->dogfight_space);
        if (AreaGlobals.player_deaths != 0 || flags[0] != 0 || BATTLEOVERCORUSCANT_ADATA == NULL) {
            return;
        }
        if (Game.area_save[BATTLEOVERCORUSCANT_ADATA->index].area_complete != 0) {
            return;
        }
        if (GamePlayTimer.time_elapsed >= 3.0f) {
            flags[0] = 1;
        }
    }

    void ChrisDogFightADraw(WORLDINFO *world) {
        DrawSpaceLevel(world->dogfight_space);
        if (DogfightDrawSelectedSock != 0 && DogfightSelectedSock != NULL) {
            DrawSelectedSock(world->sock_sys, 4, DogfightSelectedSock, 5, 8);
        }
    }

} // extern "C"
