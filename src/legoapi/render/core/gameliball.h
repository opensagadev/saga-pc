/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

/* 11 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 1 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)

struct APIDEBRISSYS_PC;
struct FRAME_PAD_PC;
struct NUGSCN_s;
struct PC_SCENE_MTL_VIEW;
struct debris_collision_entry_s;
struct debris_info_s;
struct debris_key_s;
struct debris_particle_chunk_s;
struct debris_render_chunk_s;
struct debris_scale_cache_s;
struct eduiitem_s;
struct eduimenu_s;
struct nufpar_pc_s;
struct numtl_s;

typedef struct SFX_GROUP_PC { // size 0x8
    i16 first_sample;         // 0x0
    i16 sample_count;         // 0x2
    i16 cursor;               // 0x4
    i16 unknown_0006;         // 0x6
} SFX_GROUP_PC;

typedef struct TERRAIN_POLYGON_INFO_PC TERRAIN_POLYGON_INFO_PC;

struct TERRAIN_POLYGON_INFO_PC { // size 0x64
    f32 min_x;                   // 0x00
    f32 max_x;                   // 0x04
    f32 min_y;                   // 0x08
    f32 max_y;                   // 0x0c
    f32 min_z;                   // 0x10
    f32 max_z;                   // 0x14
    NUVEC vertices[4];           // 0x18
    NUVEC normals[2];            // 0x48

    union { // size 0x4
        u32 packed_metadata;

        struct {             // size 0x4
            u8 material[2];  // 0x60
            u8 flags;        // 0x62
            u8 normal_flags; // 0x63
        };
    }; // 0x60
};

/* Terrain set, group, platform and query layouts shared by nuterrain.c and the terrain_*.cpp units. */
typedef struct TERRAIN_DISPLAY_SCENE TERRAIN_DISPLAY_SCENE;
typedef struct TERRAIN_DISPLAY_OBJECT TERRAIN_DISPLAY_OBJECT;
typedef struct TERRAIN_GSCENE TERRAIN_GSCENE;
typedef struct TERRAIN_SCENE_OBJECT TERRAIN_SCENE_OBJECT;
typedef struct TERRAIN_FILE_CHUNK TERRAIN_FILE_CHUNK;
typedef struct TERRAIN_SPATIAL_CHUNK TERRAIN_SPATIAL_CHUNK;
typedef struct TERRAIN_SKIN_STREAM TERRAIN_SKIN_STREAM;

typedef union TERRAIN_SCAN_WORD { // size 0x4

    struct {               // size 0x4
        i16 shape_count;   // 0x0
        i16 terrain_group; // 0x2
    } header;

    TERRAIN_POLYGON_INFO_PC *shape;
    u32 opaque_bits;
} TERRAIN_SCAN_WORD;

struct TERRAIN_FILE_CHUNK { // size 0x34
    i16 chunk_count;        // 0x00
    u16 version;            // 0x02
    u32 data_word_count;    // 0x04
    NUVEC origin;           // 0x08
    i16 chunk_type;         // 0x14
    u16 field_16;           // 0x16
    u8 unknown_18[0x14];    // 0x18
    u32 platform_flags;     // 0x2c
    u16 field_30;           // 0x30
    u16 group_info;         // 0x32
};

struct TERRAIN_SCENE_OBJECT { // size 0x50
    u8 pad_0x00[0x44];        // 0x00
    u32 flags;                // 0x44
    void *object;             // 0x48
    u8 pad_0x4c[4];           // 0x4c
};

struct TERRAIN_DISPLAY_OBJECT { // size 0xd0
    u8 pad_0x00[0x40];          // 0x00
    u8 embedded_object[0x78];   // 0x40
    u32 flags;                  // 0xb8
    u32 unknown_bc;             // 0xbc
    i32 scene_object_index;     // 0xc0
    void *object;               // 0xc4
    u8 pad_0xc8[8];             // 0xc8
};

struct TERRAIN_DISPLAY_SCENE {       // size 0x74
    u8 pad_0x00[0x6c];               // 0x00
    i32 object_count;                // 0x6c
    TERRAIN_DISPLAY_OBJECT *objects; // 0x70
};

struct TERRAIN_GSCENE {                   // size 0x114
    u8 pad_0x00[0x1c];                    // 0x000
    i32 object_count;                     // 0x01c
    TERRAIN_SCENE_OBJECT *objects;        // 0x020
    u8 pad_0x24[0x34];                    // 0x024
    void *object_map_present;             // 0x058
    u16 *object_map;                      // 0x05c
    u8 pad_0x60[0xb0];                    // 0x060
    TERRAIN_DISPLAY_SCENE *display_scene; // 0x110
};

typedef union TERRAIN_FLOAT_BITS {
    f32 value;
    u32 bits;
} TERRAIN_FLOAT_BITS;
typedef struct TerrainScanBounds TerrainScanBounds;
typedef struct TerrainScanWriter TerrainScanWriter;

struct TerrainScanBounds {
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
};

typedef struct music_playback_s { // size 0x24
    i16 state;                    // 0x00
    i16 requested_track;          // 0x02
    i16 primary_stream;           // 0x04
    i16 current_track;            // 0x06
    i16 transition_frames;        // 0x08
    i16 queued_track;             // 0x0a
    i16 secondary_stream;         // 0x0c
    i16 resume_frames;            // 0x0e
    u8 pause_requested;           // 0x10
    u8 restore_requested;         // 0x11
    u8 field_0012;                // 0x12
    u8 field_0013;                // 0x13
    f32 seek_offset;              // 0x14
    f32 transition;               // 0x18
    u16 update_delay;             // 0x1c
    i16 resume_track;             // 0x1e
    void *track_data;             // 0x20
} MUSICPLAYBACK_PC;

typedef struct sfx_record_s { // size 0x40
    char *name;               // 0x00
    u32 sample_id_flags;      // 0x04
    i32 pitch;                // 0x08
    i8 priority;              // 0x0c
    u8 unknown_000d[3];       // 0x0d
    i16 next;                 // 0x10
    i16 volume;               // 0x12
    i16 group;                // 0x14
    u16 category;             // 0x16
    f32 pan;                  // 0x18
    f32 pitch_rnd;            // 0x1c
    f32 volume_rnd;           // 0x20
    f32 falloff_near;         // 0x24
    f32 falloff_far;          // 0x28
    f32 buzz_timer;           // 0x2c
    i32 rumble_strength;      // 0x30
    f32 rumble_sustain;       // 0x34
    f32 rumble_release;       // 0x38
    const char *filename;     // 0x3c
} SFX_RECORD_PC;

typedef struct sfx_file_info_s { // size 0x1c
    const char *filename;        // 0x00
    i32 pitch;                   // 0x04
    i32 static_sample;           // 0x08
    i32 index;                   // 0x0c
    u32 unknown_10[3];           // 0x10
} SFX_FILE_INFO_PC;

typedef struct gameanimprogress_s { // size 0xc
    i32 count;                      // 0x0
    i32 entry_size;                 // 0x4
    u8 **entries;                   // 0x8
} GAMEANIMPROGRESS;

#ifdef __cplusplus
extern "C" {
#endif
    void PlaySfxByIdAndSetVolumeAndPitch(i32, NUVEC *, f32, f32);
    void SetSoundBitsById(const i32 *, void (*)(i32));
    void DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z);
    void PlaySfxByIdAndSetPitch(i32, NUVEC *, f32);
    void DebFreeAllCreatedEffects(void);

    typedef struct TERRAIN_CALLBACK_LIFECYCLE_PC { // size 0x8
        void (*function)(void *argument);          // 0x0
        void *argument;                            // 0x4
    } TERRAIN_CALLBACK_LIFECYCLE_PC;

    extern f32 TerrPlatScanDist;
    extern i32 PlatCodeCallback;
    extern i32 cntrots;
    extern TERRAIN_CALLBACK_LIFECYCLE_PC PlatCallback[8];
    extern i32 DEBPAGE_GENERAL;
    extern i32 DEBPAGE_CHARACTER;
    extern i32 DEBPAGE_AREA;
    void DebrisSetFacing(i32, u8, i16, i16);
    void edppStartSingleEffect(i32);
    extern i32 EdppUseMemcardPc;
    void DebrisPopulateInstance(i32, f32);
    i32 CreateScaledEffect(i32, f32);
    void DebrisReScale(i32, f32);
    void AddScaledFiniteShotDebrisEffect(i32 *, i32, NUVEC *, NUVEC *, NUVEC *, i32, f32);
    extern i32 EDPP_SCALE_TYPES;
    void ScanTerrainPlatform(i32 group_index, i32 terrain_mask);
    extern i32 TerrPlatDis;
    void SetSoundVolume(f32 volume);
    f32 GetSoundVolume(void);
    extern f32 debris_render_global_time;
    extern f32 debris_render_panel_time;
    extern u8 TerrWallTab[4];
    void RotateTerrain(TERRAIN_POLYGON_INFO_PC *);
    void FullDeflectSize(NUVEC *, NUVEC *, NUVEC *);
    void FullReflectTest(NUVEC *, NUVEC *, NUVEC *);
    void TerrainWallSideSlide(NUVEC *, void *, f32, f32, NUANG);
    void TerrainPolyEdge(NUVEC *, NUVEC *);
    i16 InsideLineF(f32, f32, f32, f32, f32, f32);
    i16 InsidePolLines(f32, f32, f32, f32, f32, f32, f32, f32, f32, NUVEC *);
    i32 HitWallSpline(void);
    extern i32 TerrPolyObj;
    i32 TerrainPlatformEmbedded(NUVEC *);
    void TerrainImpact(NUVEC *, NUVEC *, u8 *);
    i32 PlatformChecks(i32, NUVEC *);
    i32 FullDeflectTest(NUVEC *, NUVEC *, NUVEC *);
    void TerrainSideClamp(NUVEC *, NUVEC *);
    void TerrainImpactNorm(void);
    void StorePlatImpact(void);
    i32 TerrShapeSideStep(NUVEC *, NUVEC *, u8 *);
    void TerrainMoveImpactData(void);
    i32 TerrainImpactPlatform(u8 *);
    i32 TerrainPlatformMoveCheck(NUVEC *, NUVEC *, i32, i32, i32);
    void AddCollisionSphere(NUVEC *, f32);
    extern const f64 TerrainOpposingContactThresholdPc;
    extern f32 TerConTol;
    extern f32 TerrWallDeflectYScale;
    extern i32 PlatCrush;
    extern NUVEC TerrWallNorm;
    extern i32 plathitid;
    extern TERRAIN_POLYGON_INFO_PC PlatImpactTer;
    extern TERRAIN_POLYGON_INFO_PC TerrPolyInfo;
    extern f32 wallover;
    extern i32 TerImpactDataMax;
    extern i32 *TerImpactDataCount;
    extern NUVEC PlatImpactNorm;
    extern void *TerImpactData;
    extern i32 WallSplCount;
    extern i32 curPickInst;
    extern void *TerrOverRideScan;
    void ScanTerrain(i32, i32, i32);
    void ScanWallSplineTerrain(i32, i32, i32);
    f32 TerrainDivideNonzeroPC(f32, f32);
    void DebrisTimeSlip(i32);
    void DebrisProcessTimeSlip(void);
    void TerrainDiagnosticNoOpPC(const void *context, ...);
    extern const f32 TerrainUnusedSpatialBlockPc;
    extern const f32 TerrainBoundsMinimumPc;
    extern const f32 TerrainBoundsMaximumPc;
    extern const f32 TerrainSteepNormalYPc;
    extern void *ScaleTerrainT2;
    extern void *TempScanStack;
    extern void *WallSplList;
    extern TERRAIN_POLYGON_INFO_PC *ScaleTerrainT1;
    extern i32 terraincnt;
    extern i32 curSphereter;
    extern i32 platinrange;
    extern i32 PlatImpactId;
    extern i32 TerrShapeAdjCnt;
    extern TERRAIN_POLYGON_INFO_PC *TerrPoly;
    extern u8 TerrWallInfo;
    extern GAMEANIMPROGRESS gameanimsysprogress;
    i32 ShadowRoofInfo(void);
    i32 NewShadowOnPlatform(void);
    extern TERRAIN_POLYGON_INFO_PC *ShadPoly;
    extern TERRAIN_POLYGON_INFO_PC *EShadPoly;
    extern TERRAIN_POLYGON_INFO_PC *ShadRoofPoly;
    extern i16 castnum;
    extern f32 GetSurfaceInfoRoofHeightPc;
    extern NUVEC GetSurfaceInfoWaterNormalPc;
    void edanimStartPage(i32 page);
    void edanimStopPage(i32 page);
    void edanimClearPage(i32 page);
    void SoundStopMusic(void);
    void edbriDetermineNearest(f32 distance);
    void edgraSortVectorBuffer(i32 index);
    void edgraInitAllClumps(void);
    typedef struct debris_particle_s DEBRISPARTICLE_PC;

    struct debris_particle_s { // size 0x20
        NUVEC position;        // 0x00
        f32 start_time;        // 0x0c
        NUVEC momentum;        // 0x10
        f32 inverse_lifetime;  // 0x1c
    };

    extern i32 debris_suspended;
    extern i32 update_debris_enabled;
    extern f32 globaltime;
    extern f32 panelglobaltime;
    extern i16 *freedebkeys;
    extern i32 freedebkeyptr;
    extern i32 maxdebkeys;
    i32 DebAlloc(void);

    void DebrisStartOffset(i32, f32);
    void DebrisOrientation(i32, i16, i16);
    void DebrisEmitterOrientation(i32, i16, i16, i16);
    void DebrisReflectionOrientation(i32, i16, i16, f32, f32);
    void DebrisSetTrigger(i32, i32, i32, f32);
    void edgraCalculatePage(i8, i32);
    extern volatile i32 debris_control_stack_lock;
    i32 SolveQuadratic(f32, f32, f32, f32 *, f32 *);
    void DebrisGetControlStackLock(void);
    void DebrisReleaseControlStackLock(void);
    i32 DebrisNuASin(f32);
    extern u32 debris_trash_space;
    extern u32 debris_trash_size;
    extern i32 debris_setup_called;
    extern i32 freedebchkptr;
    extern i32 freedebchkptrg;

    void edbriStopPage(i8 page);
    extern SFX_GROUP_PC NuSound3Groups[128];
    extern i16 NuSound3GroupSampleIds[512];
    extern i32 NuSound3GroupSampleCount;
    extern i32 NuSound3GroupCount;
    void DebrisRegisterCutoffCameraVec(void *position);
    void DebrisSetRenderGroup(i32 group);
    void NewTerrHitInfo(u8 *);
    extern const char LevelConfigGenericKeyword_reflect_y[10];
    extern const char LevelConfigGenericKeyword_metal[6];
    extern const char LevelConfigGenericKeyword_in_space[9];
    extern const char LevelConfigGenericKeyword_override_nopickupgravity[25];
    extern const char LevelConfigGenericKeyword_hidden_icons[13];
    extern const char LevelConfigGenericKeyword_pickups_to_panel[17];
    extern const char LevelConfigGenericKeyword_forget_takeovers[17];
    extern const char LevelConfigGenericKeyword_narrow_socks[13];
    extern const char LevelConfigGenericKeyword_camera_rain[12];
    extern const char LevelConfigGenericKeyword_terrain_rain[13];
    extern const char LevelConfigGenericKeyword_double_score[13];
    extern const char LevelConfigGenericKeyword_flat_terrain[13];
    extern const char LevelConfigGenericKeyword_cam_tilt[9];
    extern const char LevelConfigGenericKeyword_raycaststep[12];
    extern const char LevelConfigGenericKeyword_plat_scan_dist[15];
    extern const char LevelConfigGenericKeyword_waterripple_startcol_r[23];
    extern const char LevelConfigGenericKeyword_waterripple_startcol_g[23];
    extern const char LevelConfigGenericKeyword_waterripple_startcol_b[23];
    extern const char LevelConfigGenericKeyword_waterripple_startcol_a[23];
    extern const char LevelConfigGenericKeyword_waterripple_endcol_r[21];
    extern const char LevelConfigGenericKeyword_waterripple_endcol_g[21];
    extern const char LevelConfigGenericKeyword_waterripple_endcol_b[21];
    extern const char LevelConfigGenericKeyword_waterripple_endcol_a[21];
    extern const char LevelConfigGenericKeyword_waterripple_life[17];
    extern const char LevelConfigGenericKeyword_cam_pullback_dist[18];
    extern const char LevelConfigGenericKeyword_cam_lateral_dist[17];
    extern const char LevelConfigGenericKeyword_campos_seek[12];
    extern const char LevelConfigGenericKeyword_camang_seek[12];
    extern const char LevelConfigGenericKeyword_shadowtype[11];
    extern const char LevelConfigGenericKeyword_music[6];
    extern const char LevelConfigGenericKeyword_music_other[12];
    extern const char LevelConfigGenericKeyword_sfx_ambient[12];
    extern const char LevelConfigGenericKeyword_conveyor[9];
    extern const f32 LevelConfigFarDistance;
    u32 SfxNameHash(const char *name);
    extern i16 *SfxHashHeads;
    void PlaySfxAndSetVolume(char *, NUVEC *, f32);
    void PlaySfxAndSetPitch(char *, NUVEC *, f32);
    void PlaySfxById(i32, NUVEC *);
    void PlaySfxByIdAndSetVolume(i32, NUVEC *, f32);
    extern const f64 BoltTargetRandomDenominator;
    extern void *CurrentTerrain;
    i32 PlatInstGetHit(i32);
    void ScanTerrIDRemovePlat(i32);
    extern NUVEC PCSoundListenerLoopPositionPc;
    extern u32 nusound_seed;
    extern u32 *SfxCRCTable;
    extern i32 SfxCRCTableInitialized;
    u32 SfxNameHashN(const char *name, u32 limit);
    extern SFX_FILE_INFO_PC *SfxFileTable;
    extern i32 SfxFileCount;
    void SetSfxBit_OnEx(i32 id);
    void SetSfxBit_On(i32 id);
    void SetSfxBit_OffEx(i32 id);
    i32 SfxBitEx(i32 id);
    extern i32 DebrisCutSceneMode;
    extern i32 edbri_page_on[8];
    void edbriBridgesReset(void);
    void DebFreeChunksInstantly(i32 *);
    void edppPtlDestroy(i32);
    void edppPtlShelve(i32);
    void DebFree(i32 *);

    void DebrisProcessControlChunks(i32);

    typedef struct CHARACTER_SHADOW_RESET_SLOT { // size 0x14
        NUVEC position;                          // 0x00
        u16 x_rotation;                          // 0x0c
        u16 z_rotation;                          // 0x0e
        f32 opacity;                             // 0x10
    } CHARACTER_SHADOW_RESET_SLOT;

    /* PodRacePc: see episodeI.c. */

    i32 DebrisSingleCollisionCheckScaleYFlag(i32, NUVEC *, f32, f32, u8);
    i32 DebrisSingleTorusCollisionCheckScaleYFlag(i32, NUVEC *, f32, f32, u8);
    i32 DebrisCollisionCheckScaleYFlag(NUVEC *, f32, f32, u8);
    i32 DebrisTorusCollisionCheckScaleYFlag(NUVEC *, f32, f32, u8);
    i32 DebrisPreCheckCollisions(NUVEC *, f32);
    i32 DebrisTorusCollisionCheckScaleY(NUVEC *, f32, f32);
    i32 DebrisCollisionCheckScaleY(NUVEC *, f32, f32);
    i32 DebrisCollisionCheckFlag(NUVEC *, f32, u8);
    i32 DebrisCollisionCheck(NUVEC *, f32);
    i32 DebrisAllCollisionCheckScaleYFlag(NUVEC *, f32, f32, u8);
    void SortDebrisRenderStack(void);
    void UpdateDebrisRenderStackPriority(void);
    i32 DebrisFindAllOfType(i32, NUVEC *, i32 *, i32, i32, i32);
    i32 DebrisGetRingProperties(i32, NUVEC *, NUVEC *, f32 *);
    i32 DebrisGetConeProperties(i32, NUVEC *, NUVEC *, f32 *, f32 *, NUVEC *, f32 *, f32 *);
    void edppDetermineNearest(f32);
    void edppDeleteEffect(i32);
    i32 IsSfxLooping(i32);
    extern i32 numspherecolldata;
    extern i32 numtoruscolldata;
    extern NUVEC debris_collide_pt;
    extern i32 globalframes;
    i32 GetSfxIdN(char *, i32);
    void DebrisStatusAlwaysOff(i32 *handle);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
void SetAudioFadeLevel(f32 level);
f32 GetAudioFadeLevel(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif

    void NewScanInit(void);
    f32 NewShadowHandelEx(NUVEC *, f32, f32, f32, i32, i16 *);
    i32 TerrainTrackBack(NUVEC *, NUVEC *, f32, f32, NUVEC *);
    void TerrDraw(TERRAIN_POLYGON_INFO_PC *, i16);
    void TerrDrawSitu(TERRAIN_POLYGON_INFO_PC *, NUVEC *);
    void TerrDrawPlat(TERRAIN_POLYGON_INFO_PC *, i16);
    void TerrDrawPlatCol(TERRAIN_POLYGON_INFO_PC *, i16, i32);
    void TerrDrawImpactPol(void);
    void DrawMPlatPc(i32);
    void DrawMSitu(i32);
    void SphereDrawEx(NUVEC *, f32, f32, i32);
    void SphereDraw(NUVEC *, f32, f32);

    struct TERRAIN_SKIN_STREAM;
    struct TERRAIN_SKIN_INFO;
    struct TERRAIN_SKIN_BLEND;
    void TerrainSkinPrepareGroupPc(i32 group_index, struct TERRAIN_SKIN_STREAM *output,
                                   const struct TERRAIN_SKIN_INFO *info);
    void TerrainSkinInitArenaPc(struct TERRAIN_SKIN_INFO *base, u8 *end, i32 capacity);
    void TerrainSkinResetArenaPc(void);
    u8 *TerrainSkinRebuildIndexPc(i32 pool_slots);

    void terrainpickupinit(char *, void **);
    void AddPickupTerr(i32, NUVEC *);
    void ReassignPickupInst(i32, i32);

    extern SFX_FILE_INFO_PC *SfxFileInfoPc;
    extern SFX_RECORD_PC *SfxRevertInfoPc;
    extern i32 NumSfx;
    extern i32 NumSfxNames;
    extern char SfxNameSlotsPc[1601][32];
    extern char SfxFilenameSlotsPc[1601][64];
    extern i32 SfxRefCountsPc[1601];
    extern char SfxConfigFilenamePc[256];
    extern f32 SfxAudioVersionPc;
    extern i32 edSfxAllCount;
    void edgraDetermineNearestInstance(f32 distance);
    extern i32 edgra_mode;
    extern i32 edpp_showAllPlaced;
    void DebrisSetDetailLevels(i32 arg0, i32 arg1);
    void DebrisSetGroupID(i32 handle, i16 group);
    void edppDrawCursor(void);
    void edppHighlightNearest(void);
    void edppMultipleCopyClear(void);
    void edppMultipleCopyCopy(void);
    void edppMultipleCopyPaste(void);
    i32 edppPtlCreate(struct nuvec_s *arg0, i32 arg1);
    extern i32 edpp_readout;
    void DebrisSetPriority(i32 arg0, i16 arg1, u8 arg2);
    extern i32 edbits_particle_level_page;
    i32 edgraClumpCreate(struct nuvec_s *position);
    void edgraClumpDestroy(i32 index);
    void edgraClumpPlace(i32 index, struct nuvec_s *position);
    void edgraInstanceCreate(struct nuvec_s *position);
    void edgraInstanceDestroy(i32 index);
    void edgraInstancePlace(i32 index, struct nuvec_s *position);
    extern i32 edgra_copy_source;
    extern i32 edbri_pageid;
    void edbriDrawCursor(void);
    void edgraDrawCursor(void);
    extern char edbits_datapath[256];
    extern i32 edbits_override_backups;
    i32 edppSaveEffects(char *filename, u8 page);
    extern i32 edpp_usememcard;
    void edbriBridgeDestroy(i32 arg0);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 2 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)

struct TerrainScanWriter {
    u8 *group_header;
    TERRAIN_SHAPE **cursor;
    u8 *limit;
    i32 group_shape_count;
    i32 scaled_shape_count;
};
#ifdef __cplusplus
extern "C" {
#endif
    void SfxBitsStore(SoundTable *);
    void NewTerrAxisFreedom(TERRAIN_AXIS_FREEDOM_SHAPE_PC *, NUVEC *);
    extern TERRAIN_SPHERE_PC SphereData[16];
    extern TERRAIN_AXIS_FREEDOM_SHAPE_PC *TerrShape;
    extern TERRAIN_TRACK_SLOT_PC *CurTrackInfo;
    void DebrisReserveTrashableSpace(VARIPTR *, VARIPTR, i32, i32);
    void DebrisTrashableSetup(VARIPTR *, VARIPTR *);
    void SfxBitsRestore(SoundTable *table);
    void SfxBuildCRCTable(VARIPTR *buffer_cursor, VARIPTR end);
    void edgraSetup(VARIPTR *, VARIPTR, i32, i32, i32);
    void DebrisSetup2(VARIPTR *, VARIPTR, VARIPTR *, VARIPTR, char *, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 3 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)

typedef struct TERRAIN_SET_PC {                 // size 0xa490
    TERRAIN_GROUP_PC *groups;                   // 0x0000
    u8 unknown_004[0x60];                       // 0x0004
    void *field_064;                            // 0x0064
    TERRAIN_PLATFORM_PC *platforms;             // 0x0068
    i16 active_platform_groups[96];             // 0x006c
    i32 active_platform_count;                  // 0x012c
    f32 platform_query_min_x;                   // 0x0130
    u8 unknown_134[4];                          // 0x0134
    f32 platform_query_min_z;                   // 0x0138
    f32 platform_query_max_x;                   // 0x013c
    u8 unknown_140[4];                          // 0x0140
    f32 platform_query_max_z;                   // 0x0144
    struct TERRAIN_SPATIAL_NODE *spatial_nodes; // 0x0148
    i16 group_count;                            // 0x014c
    u16 file_version;                           // 0x014e
    i32 group_index_count;                      // 0x0150
    i32 removed_platform_count;                 // 0x0154
    i16 removed_platforms[32];                  // 0x0158
    TERRAIN_TRACK_SLOT_PC track_slots[64];      // 0x0198
    TERRAIN_CELL_PC cells[256];                 // 0x0798
    TERRAIN_CELL_PC platform_cell;              // 0x1b98
    i32 minimum_height;                         // 0x1bac
    i32 used_cell_count;                        // 0x1bb0
    i16 *group_indices;                         // 0x1bb4
    TERRAIN_INDEX_LEVEL index_levels[17];       // 0x1bb8
    i32 max_group_indices;                      // 0xa484
    i32 max_groups;                             // 0xa488
    i32 max_platforms;                          // 0xa48c
} TERRAIN_SET_PC;

#ifdef __cplusplus
extern "C" {
#endif

    void DebrisOrientationMtx(i32, NUMTX *);
    i32 AddScaledVariableShotDebrisEffect2(i32, NUVEC *, i32, f32, NUMTX *, NUMTX *, f32);
    extern struct debris_scale_cache_s *debscale;
    void AddVariableShotDebrisEffectTimed3(i32, NUVEC *, NUVEC *, i32, f32, NUMTX *, NUMTX *);
    void AddVariableShotDebrisEffectTimed3(i32, NUVEC *, NUVEC *, i32, f32, NUMTX *, NUMTX *);
    struct TERRAIN_SET_PC;
    extern NUVEC4 tertempvec4;
    extern NUMTX tertempmat;
    extern struct APIDEBRISSYS_PC *perm_debrissys;
    void DebrisEmitterOrientationMtx(i32, NUMTX *);
    void DebrisShift(NUMTX *);
    extern struct debris_collision_entry_s *toruscolldata;
    i32 AddScaledVariableShotDebrisEffect1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z, i16 y,
                                           NUMTX *orientation, f32 scale);
    struct TERRAIN_SET_PC;
    i32 TerrainSkinCreateInfoPc(NUMTX *matrix, const struct TERRAIN_SKIN_BLEND *blend, const NUVEC4 *sample_rows,
                                f32 scale, i32 source_scene_index, i16 mode, struct TERRAIN_SET_PC *source_terrain);
    i32 AddPickupTerrRot(i32, NUMTX *, NUMTX *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 4 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)

struct TERRAIN_SPATIAL_CHUNK {      // size 0x14
    TERRAIN_SPATIAL_NODE *previous; // 0x00
    TERRAIN_SPATIAL_NODE node;      // 0x04
};
#ifdef __cplusplus
extern "C" {
#endif
    void TerrainScanWallSpline(struct TERRAIN_SPATIAL_NODE *);
    void TerrainAddWallSpline(struct TERRAIN_SPATIAL_NODE *, struct TERRAIN_SET_PC *);
    void TerrainRemoveWallSpline(struct TERRAIN_SPATIAL_NODE *, struct TERRAIN_SET_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 5 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void DebrisSetRoomID(i32, struct NUGSCN_s *);
    extern struct NUGSCN_s *EdppPageScenesPc[8];
    extern struct numtl_s *DebMat[9];
    extern struct NUGSCN_s *edbri_page_scene[8];

    i32 edbitsLookupInstance(char *, struct NUGSCN_s *);
    void edanimRegisterBaseScene(struct NUGSCN_s *scene);
    struct numtl_s *CreateCopyMat(struct numtl_s *, i32, i32, i32, i32);

    void edbriBridgeUpdate(i32 index, struct NUGSCN_s *scene);
    i32 edppLoadPage(char *, i8, struct NUGSCN_s *);
    extern struct NUGSCN_s *edbits_things_scene;
    void edgraDoInput(struct FRAME_PAD_PC *pad);
    void edppDoInput(struct FRAME_PAD_PC *pad);
    void edbriDoInput(struct FRAME_PAD_PC *pad);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 6 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_key_s *debkeydata;
    void AudioGroup_Parse(struct nufpar_pc_s *);
    void edbitsDrawCircleTilted(struct nuvec_s *arg0, f32 arg1, i32 arg2, struct PC_SCENE_MTL_VIEW *arg3, i32 arg4,
                                i32 arg5);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 7 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct SPECIAL_RESOURCE_EVENT SPECIAL_RESOURCE_EVENT;

    struct SPECIAL_RESOURCE_EVENT {   // size 0x10
        f32 previous_frame;           // 0x0
        u8 flags;                     // 0x4
        u8 unknown_05;                // 0x5
        i16 sfx_id;                   // 0x6
        f32 trigger_frame;            // 0x8
        SPECIAL_RESOURCE_EVENT *next; // 0xc
    };

    typedef struct SPECIAL_RESOURCE_ENTRY { // size 0x14
        u8 flags;                           // 0x00
        u8 unknown_01;                      // 0x01
        i8 event_count;                     // 0x02
        i8 animation_playing;               // 0x03
        NUSPECIALHANDLE special;            // 0x04
        SPECIAL_RESOURCE_EVENT *events;     // 0x10
    } SPECIAL_RESOURCE_ENTRY;

    typedef struct SPECIAL_RESOURCE_WORLD { // size 0x46dc
        u8 unknown_000[0x80];               // 0x0000
        char config_file[0x80];             // 0x0080
        u8 unknown_100[4];                  // 0x0100
        VARIPTR buffer;                     // 0x0104
        u8 unknown_108[0x13c - 0x108];      // 0x0108
        struct NUGSCN_s *scene;             // 0x013c
        u8 unknown_140[0x46cc - 0x140];     // 0x0140
        i32 entry_count;                    // 0x46cc
        SPECIAL_RESOURCE_ENTRY *entries;    // 0x46d0
        i32 event_count;                    // 0x46d4
        SPECIAL_RESOURCE_EVENT *events;     // 0x46d8
    } SPECIAL_RESOURCE_WORLD;

#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 8 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_info_s *effecttypes;
    void GenDebMomAdjFromPos(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void GenDebMomAdjFromPosAll(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void GenDebMomAdjFromPosRev(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void GenDebMomAdjFromSplash(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void GenDebMomAdjFromAshRock(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void GenDebMomAdjFromPosRevTree(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
    void DebFreeOrphansInstantly(struct debris_info_s *);
    void FileLoadSingleEffectType(struct debris_info_s *, i32, i8);
    void edanimcbCancelBouncyMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelLocalSoundTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelSoundMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelSoundTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbSetSoundType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void cbPtlCancelDataMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelEmitMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlShowAll(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *edptl_drawflag_menu;
    extern struct eduimenu_s *edptl_group_menu;
    extern struct eduimenu_s *ptldatamenu;
    extern struct eduimenu_s *ptlemitmenu;
    extern struct eduimenu_s *dpadmodemenu;
    extern struct eduimenu_s *edpp_active_menu;
    extern struct eduimenu_s *edptl_bounce_menu;
    extern struct eduimenu_s *edptl_damage_menu;
    extern struct eduimenu_s *edptl_damageflag_menu;
    extern struct eduimenu_s *edptl_orphanlist_menu;
    extern struct eduimenu_s *edptl_testdetail_menu;
    extern struct eduimenu_s *effectlistmenu;
    extern struct eduimenu_s *namemenu;
    extern struct eduimenu_s *ptlcutoffmenu;
    extern struct eduimenu_s *edptl_quickdel_menu;
    extern struct eduimenu_s *texturemenu;
    extern struct eduimenu_s *textureselectmenu;
    void edanimcbCancelLocalParticleMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelLocalParticleTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelMCTBMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelParticleMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbCancelParticleTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbSetLocalParticleType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void edanimcbSetParticleRate(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edanimcbSetParticleType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void edanimcbToggleParticleSwitch(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void cbPtlCancelGSortMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelReadoutMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlSelReadout(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *ptlgsortmenu;
    extern struct eduimenu_s *ptlreadoutmenu;
    void edanimcbCancelSwitchTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edanimcbSetSwitchType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void edanimcbLocalParticleMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *ptltypemenu;
    extern struct eduimenu_s *ptlgcodemenu;
    void edptlcbCancelClipboardMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    extern struct eduimenu_s *messagemenu;
    extern struct eduimenu_s *ptlclipmenu;
    extern struct eduimenu_s *edptl_instancesettings_menu;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 9 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_key_s *debris_keystack;
    void DebrisDrawCalculateClipBoxes(struct debris_info_s *, struct debris_key_s *);
    DEBRISPARTICLE_PC *GenDebIndex(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebDummy(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexRadial(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexRadialRotor(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexSpheroid(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexBounceY(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexBounceXZ(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexImprovedRadial(struct debris_key_s *, struct debris_info_s *, f32);
    DEBRISPARTICLE_PC *GenDebIndexRadialStar(struct debris_key_s *, struct debris_info_s *, f32);
    void DebReAlloc2(struct debris_key_s *);
    void RemoveDebrisEffectFromStack(struct debris_key_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 10 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern DEBRIS_CHUNK_CONTROL_PC **freechunkcontrols;
    extern DEBRIS_CHUNK_CONTROL_PC *debris_chunk_control_stack[2];
    void AddChunkControlToStack(DEBRIS_CHUNK_CONTROL_PC *, DEBRIS_CHUNK_CONTROL_PC **);
    extern DEBRIS_CHUNK_CONTROL_PC *debris_chunk_controls;
    extern struct debris_particle_chunk_s **freedebchunks;
    extern struct debris_particle_chunk_s **freedebchunksglass;
    void RemoveChunkControlFromStack(DEBRIS_CHUNK_CONTROL_PC *, DEBRIS_CHUNK_CONTROL_PC **);
    void RemoveAnyChunkControls(struct debris_particle_chunk_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 11
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART == 11 && !defined(SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_render_chunk_s *ParticleChunkToRender;
    extern struct debris_render_chunk_s *ParticleChunkRenderStack[5];
    void AddChunkToRenderStack(struct debris_render_chunk_s *, struct debris_render_chunk_s **);
    void RemoveChunkFromRenderStack(struct debris_render_chunk_s *, struct debris_render_chunk_s **);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_PART 12
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_RENDER_CORE_GAMELIBALL_H_STOP
