#ifndef SAGA_LEGOAPI_WORLD_WORLD_SHARED_H
#define SAGA_LEGOAPI_WORLD_WORLD_SHARED_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Pulses_Configure(WORLDINFO *, char *);
    void EquivalentObjects_Configure(WORLDINFO *, char *);
    void CharPlatforms_Configure(WORLDINFO *, char *);
    void SetLevelSfxBits(WORLDINFO *);
    CUT_LOAD_SYS_PC *CutScenes_Load(char *, NUGSCN *, NUGSCN *, i32, VARIPTR *, VARIPTR *, i32, i32, WORLDINFO *);

    void LevelSplines_InitForLevel(WORLDINFO *);
    void NuGScnUpdate(NUGSCN *scene, f32 delta);
    extern NUGSCN *things_scene;

    void TrafficAnimSys_Configure(WORLDINFO *world, char *config);
    void Faders_Configure(WORLDINFO *world, char *config);
    extern NUVEC TerrImpactPos;
    extern i32 terrhitflags;
    extern i16 TerrImpact;
    extern NUVEC TerrImpactNormal;
    GAMEANIMSYS *GameAnimSys_Create(VARIPTR *buf, VARIPTR *end);
    extern NUVEC ShadNorm;
    void ResetLevSfx(WORLDINFO *world);
    void SockSys_Configure(SOCKSYS_PC *, char *, i32, VARIPTR *, VARIPTR *, NUGSCN *);
    void LoadGizmoSys(void *system, void *world, char *config_file);
    void ClearGameObjects(APIOBJECTSYS_PC *system);
    i32 rtlResetDynamic(void);
    void *GameAntnode_CreateSys(WORLDINFO *, VARIPTR *, VARIPTR *, i32);
    extern i32 CUTCAM;
    extern const char APIObjectCharsDirectory[7];
    void GameAIScriptAddLevelSfx(WORLDINFO *, NULISTHDR *);
    i32 LoadEditorSplines(char *, VARIPTR *, VARIPTR *);
    void *NuGCutSceneLoad(char *path, VARIPTR *buffer, VARIPTR *end, i32 no_stream_buffers);
    void NuGCutSceneFixUp(void *scene, NUGSCN *gscene, void *extra_scene, i32 page);
    void NuGCutSceneFixUpExtra(void *scene, NUGSCN *gscene);
    INSTNUGCUTSCENE *instNuGCutSceneCreate(void *scene, NUGSCN *gscene, void *extra_scene, char *name, VARIPTR *buffer,
                                           i32 background);
    PARTDEBRISSYS *InitPartDebris(VARIPTR *, VARIPTR *, i32, i32, char **, i32);
    GAMEOBJECT *AddGameObject(void);
    i32 InitCreature(GAMEOBJECT *, i32, i32);
    void LevObj_FixUpPlatIDs(WORLDINFO *);
    void NuRndrInitWorld(void);

    void PortalDoors_Configure(WORLDINFO *, char *);
    void GizForceSFX_Configure(WORLDINFO *, char *);
    void Doors_Configure(WORLDINFO *, char *);
    void Particles_Load(WORLDINFO *, char **, i32, i32);
    void *LoadGizFlow(void *, void *, char *, VARIPTR *, VARIPTR *);
    void GizmoSysAddGizmos(void *, GIZFLOW_PC *, void *);
    void LoadLights(void *, char *);

    i32 InitSpecialSfx(void *);
    void LoadSpecialSfxFile(void *);
    void BoltTypes_Configure(WORLDINFO *, char *);
    void BoltTypes_Init(WORLDINFO *);
    void LevelObjects_InitForLevel(WORLDINFO *);

    extern char *debris_name[147];
    extern void *things_scene_terrain;
    AISYS_PC *AISysLoad(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *);
    void *AIPathCnxControlSysCreate(VARIPTR *, VARIPTR *, i32);
    void *AIPathCnxHelperSysCreate(VARIPTR *, VARIPTR *, i32);
    void *AITriggerSetSysCreate(VARIPTR *, VARIPTR *);
    void *CreateClimbObjectSys(VARIPTR *, VARIPTR *, i32);
    APIOBJECTSYS_PC *APIObjectSysInit(i32, VARIPTR *, VARIPTR *);
    void MiniKit_Load(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *, void *);
    void MiniKit_InitPieces(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *);
    void Hub_LoadAndFixUpMiniKits(WORLDINFO *, VARIPTR *, VARIPTR *);
    void CharacterMiniKits_Load(COLLECTION_BUILD_PC *, WORLDINFO *, VARIPTR *, VARIPTR *);
    void RippleEffects_Configure(WORLDINFO *, char *);
    extern COLLECTION_BUILD_PC MiniKitCollection;
    void CharScenes_LevelLoad(WORLDINFO *);
#ifdef __cplusplus
}
#endif

#endif
