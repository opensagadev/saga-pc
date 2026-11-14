#include "saga.h"

f32 Condition_GlynTest(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Debug(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Active(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotGun(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PrefersBrawling(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ObstacleAtEndInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_SpecialAtStartInitPc(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ObstacleAtStart(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ObstacleAtEnd(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ObstacleLockedShut(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ObstacleLockedOpen(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ObstacleOpenedByPlayerInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ObstacleOpenedByPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ObstacleOpenedByEitherPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ForceInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ForceAtStart(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ForceAtEnd(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IsVisibleInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IsVisible(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_MySet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ForcePushingInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ForcePushing(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_TurretAliveInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_TurretAlive(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CategoryIsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CategoryIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerCategoryIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmAPartyCharacter(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IAmAInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IAmA(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OpponentIsAInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_OpponentIsA(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GameOpponentIsAThreatPc(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CanFightLikeAJedi(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmAGoody(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmABaddy(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmANeutral(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmAGoodieBaddie(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_EitherPlayerIsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_EitherPlayerIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Player1Is(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Player2Is(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IAmPlayer2(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_UsingForceInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_UsingForce(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerUsingForce(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerUsingForce(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OnForcePlatformInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_OnForcePlatform(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerOnForcePlatform(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerOnForcePlatform(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ForceBeingUsed(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerDeflectingPart(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerPullingLever(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerUsingHatMachinePc(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerUsingPanel(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerWearingHelmet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ForceCompleteInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ForceComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ForceFinished(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_BuildItCompleteInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_BuildItComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_BlowupBlownupInitPc(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_BlowupBlownup(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IsAliveInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IsAlive(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_AIOverrideControlInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_AIOverrideControl(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IsOnScreenInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IsOnScreen(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OffScreenTimerInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_OffScreenTimer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerOnGround(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OnGround(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BeenAlerted(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OnObjectInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_PlayerOnObject(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerOnObject(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_EitherPlayerLocatorRangeXZInitPc(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_EitherPlayerLocatorRangeXZ(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OnObject(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OnSameObjectAsPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_SpawnCount(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BehindCamera(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_LocatorOnScreenInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_LocatorOnScreen(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Blocking(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_BeenHitInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_BeenHit(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_HoverPhase(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_HitPointsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_HitPoints(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CollidingWithOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Colliding(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_XPosInitPc(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_XPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_YPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ZPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IsSetAliveInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IsSetAlive(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NumInSetAlive(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Context(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_InContextInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_InContext(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentContext(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OnDynamicGrappleInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_OnDynamicGrapple(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Player2Active(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_NumBaddiesInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_NumBaddies(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_NumForceObjectsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_NumForceObjects(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_BeenToLevelInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_BeenToLevel(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_LastLevelInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_LastLevel(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_MessageInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_Message(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_ScriptParamInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_ScriptParam(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_AnimationFinishedInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_AnimationFinished(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_RigidAnimFrameInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_RigidAnimFrame(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CutSceneStartedInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CutSceneStarted(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CutSceneFinishedInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CutSceneFinished(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CutSceneExistsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CutSceneExists(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CutScenePlayingInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CutScenePlaying(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_PlayerInSockInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_PlayerInSock(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_SockDistanceToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_SockDistanceToOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_SockXDistanceToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerDistanceAlongSock(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_FurthestPlayerDistanceAlongSock(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_FinishedSpline(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CurrentHintId(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_HintAvailableInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_HintAvailable(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_HintComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Freeplay(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_IndyPc(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_MissionMode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_MissionWon(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ChallengeMode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PSP(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BoltsDontGetDeflectedBack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CheatProgress(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BigJumpComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_RespawnLocatorIsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_RespawnLocatorIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InMiniCut(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_MaulShouldRunAway(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_DropBackInTimer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PartyUnderCover(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NumBaddiesThatCanSeePlayers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PartyContainsDroids(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CannotReachDestination(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_EitherPlayerPushingSpinnerInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_EitherPlayerPushingSpinner(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CharacterRangeInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CharacterRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BeenSpawned(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_TakeOverTargetInTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_TakeOverTargetInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerInMyTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_TakenOverInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_TakenOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerTakenOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerTakenOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_BeenTakenOverInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_BeenTakenOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_OnSpeederBikeInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_OnSpeederBike(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_UnderPlayerControlInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_UnderPlayerControl(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CharacterExistsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CharacterExists(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CharacterTypeExistsInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CharacterTypeExists(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_LastAttackerRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_LastAttackerIsActivePlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_GotLocatorInSetInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_GotLocatorInSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotOpponentLOS(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EmptyTakeOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_HasTakeOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_TakeOverRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotVictim(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ScreenWipe(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_HeadTurnRestricted(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ShopActive(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_SideInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_Side(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestPartyRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestPartyXZRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentToPlayerRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentPathPosRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_GizmoOutput0InitPc(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_GizmoOutput0(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GizmoOutput1(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GizmoOutput2(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GizmoOutput3(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_GizmoVisibilityInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_GizmoVisibility(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_FlowBoxCompleteInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_FlowBoxComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_AngleAboutMyLocatorToPlayerInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_AngleAboutMyLocatorToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_AnimSpeedMul(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_PickupBeenTurnedOnInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_PickupBeenTurnedOn(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CanHearRadio(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BeingTowed(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_RaceLapPc(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_MusicOn(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_CharacterLoadedInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_CharacterLoaded(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_AreaCompleteInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_AreaComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_ShouldAttackOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InSwamp(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InSameTriggerAreaAsNearestPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NetworkGameOnGoing(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
i32 Action_ClearTakeOverTarget(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DeActivate(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanDefend(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DontAimAt(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ImmuneToKillTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ImmuneToBolts(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Respawnable(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanUseWeapon(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetBoss(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UpdateSockPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Explode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetLastSafePathPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetDontMove(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DontSetStoppedFlag(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GameSetLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetLocatorSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SnapToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_BigJumpToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetDoomedEscapeLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ShootAtOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_EngageObject(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GrabVictim(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_EatVictim(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ReleaseVictim(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PressSpecialButton(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PressTagButton(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PressActionButton(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UseWeapon(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PressJumpButton(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddToSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetControlSystem(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_BreakFormation(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FormationMove(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetFormationCommander(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Launch(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SelectRandomSpline(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CreateSplineCreatures(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetRunSpeed(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetWalkSpeed(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetHitPoints(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetShieldHitPoints(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetMessage(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetScriptParam(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CopyMessage(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddPart(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddPartDebris(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_LaunchGuidedMissile(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetHoverPhase(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetMaxMovementRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetDefaultMovementRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ApplyGravity(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_IgnoreShoveSystem(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CannotBeForcedBack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanTurn(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_NoIdleSpeed(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CannotBeSeen(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetVisibility(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_EnableSock(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddDebris(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_JudderGameCamera(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ResetGameCamera(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PlayCutScene(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetLevelPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetHint(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetHintComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CancelHint(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DisableNarrowSocks(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CycleCharacter(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CnxController(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CnxHelper(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToLevelPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetAnimation(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AlwaysTriggerObstacle(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanTriggerObstacle(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetObstacleToEnd(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanOpenDoors(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanShootOffScreen(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_KeepWeaponOut(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PrefersPlayers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetBoltsDontGetDeflectedBack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanShootObstructions(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UseBigJumpToJump(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_IgnoreTurnAroundSpline(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanMoveWhenDeactivated(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DontAttack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanPullLevers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanHitForceObjects(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AlwaysBackFlip(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PlayerSpeederHack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CatchUpForbidden(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_NotWithParty(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanAttack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_TriggerBlowUp(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ForcePush(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DeflectPlayersPart(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetPathCnxFlag(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DontRaycastLOS(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetForceBack(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FaceCamera(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SpinOnSpot(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UseTriggerSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PullLever(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UsePanel(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UseTechno(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ReleaseLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AssignLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GetLocatorFromSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_LinkTurretToController(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_TakeOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ReleaseTakeOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddGameMsgCount(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetCanTakeOver(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_IgnoreLastSafePathPos(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AwkwardShapeOverride(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_IgnoreSlideTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SplineFollowTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddMiscPickups(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetLayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CreateRider(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ThrowDetonator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_UseTimeBasedUpdate(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_WalkBackwards(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddScriptProcessor(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ForceLightning(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetAO_MaxAttackers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetAO_AttackersPerRow(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetAO_RowDist(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetAO_InitRowDist(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetTechnoComplete(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_LetGoOfBalloon(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_DrawBossHitPoints(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToNewLevel(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GizmoActivate(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GizmoSetVisibility(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_TurnOnPickup(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanHelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CanCollideWithObjects(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_AddTorpedoPacket(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetShootOpponents(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PartyCanBeUnderCover(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CreateCreatures(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SpeederBeingChased(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_MushroomCollapse(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Sebulba(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_NewSebulba(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_BoulderSection(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
void *Condition_AlwaysTrueInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_LocatorRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_LocatorRangeXZ(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_LocatorRangeY(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotLocatorSet(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OnPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerOnPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentOnPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_TimeOffPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_CurrentLocatorIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestPlayerRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestPlayerXZRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InLevelNode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerInLevelNode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerInLevelNode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NodeRangePc(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Player2InTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_EitherPlayerInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_BaddyInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GoodyInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentInTriggerArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_GotOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentIsAThreat(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentOnSamePath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestOpponentRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_YawToOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentBelow(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OriginRange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentToOrigin(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerToOrigin(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentToLocatorXZ(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_OpponentToLocatorY(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PlayerToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_NearestPlayerToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_PathBlocked(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_InterruptID(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
void *Condition_IAmInit(AISYS_PC *, char *, AISCRIPT_PC *);
f32 Condition_IAm(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_StuckTime(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
f32 Condition_Param(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
i32 Action_SetState(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ResetTimer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Idle(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetCircleDirection(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FacePlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FaceOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_IgnoreWallSplines(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CheckWallSplines(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ShadowTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetViewDistance(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetMinViewHeight(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetMaxViewHeight(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetHearDistance(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetMoveRadius(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_OverrideAnimation(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ResetToOrigin(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetIgnoreAntinodes(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetParam(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetReturnToState(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ReturnToState(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
void *Condition_LocatorRangeInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_CurrentLocatorIsInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_InTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_InLevelNodeInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_PlayerInLevelNodeInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_NodeRangeInitPc(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_PlayerInTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_BaddyInTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_GoodyInTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_OpponentInTriggerAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_OpponentToLocatorInit(AISYS_PC *, char *, AISCRIPT_PC *);
void *Condition_PlayerToLocatorInit(AISYS_PC *, char *, AISCRIPT_PC *);
i32 Action_FaceLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetRespawnLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_BlockPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PathConnectionMaxLength(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_PathConnectionObstacle(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_SetInterrupt(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_ClearInterrupt(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_NotifyStateChange(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FollowOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FollowPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_Circle(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CirclePlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_CircleOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_MoveAwayFromOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_MoveAwayFromPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_MoveAwayFromPlayer2(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_RetreatFromOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_RetreatFromNearestOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToNode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToNodeRandom(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToOrigin(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_GoToLocator(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_FollowPath(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 Action_MoveAwayFromNode(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);

#pragma const_seg(".rdata$AIRegistryTextPc00751930")
const char AIRegistryTextPc00751930[11] = "BeingTowed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755150")
const char AIRegistryTextPc00755150[9] = "Freeplay";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755d9c")
const char AIRegistryTextPc00755d9c[11] = "NewSebulba";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755da8")
const char AIRegistryTextPc00755da8[14] = "CanPullLevers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755db8")
const char AIRegistryTextPc00755db8[11] = "DontAttack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755dc4")
const char AIRegistryTextPc00755dc4[23] = "CanMoveWhenDeactivated";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ddc")
const char AIRegistryTextPc00755ddc[23] = "IgnoreTurnAroundSpline";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755df4")
const char AIRegistryTextPc00755df4[8] = "Sebulba";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755dfc")
const char AIRegistryTextPc00755dfc[13] = "RaceOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e0c")
const char AIRegistryTextPc00755e0c[15] = "BoulderSection";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e1c")
const char AIRegistryTextPc00755e1c[17] = "MushroomCollapse";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e30")
const char AIRegistryTextPc00755e30[10] = "CreatePod";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e3c")
const char AIRegistryTextPc00755e3c[11] = "SetLapTime";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e48")
const char AIRegistryTextPc00755e48[21] = "PartyCanBeUnderCover";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e60")
const char AIRegistryTextPc00755e60[18] = "SetShootOpponents";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e74")
const char AIRegistryTextPc00755e74[22] = "CanCollideWithObjects";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755e8c")
const char AIRegistryTextPc00755e8c[20] = "CanHelpWithTriggers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ea0")
const char AIRegistryTextPc00755ea0[13] = "TurnOnPickup";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755eb0")
const char AIRegistryTextPc00755eb0[19] = "GizmoSetVisibility";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ec4")
const char AIRegistryTextPc00755ec4[14] = "GizmoActivate";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ed4")
const char AIRegistryTextPc00755ed4[14] = "CircleLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ee4")
const char AIRegistryTextPc00755ee4[13] = "GoToNewLevel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ef4")
const char AIRegistryTextPc00755ef4[14] = "CompleteLevel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f04")
const char AIRegistryTextPc00755f04[18] = "DrawBossHitPoints";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f18")
const char AIRegistryTextPc00755f18[15] = "LetGoOfBalloon";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f28")
const char AIRegistryTextPc00755f28[18] = "SetTechnoComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f3c")
const char AIRegistryTextPc00755f3c[18] = "SetAO_InitRowDist";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f50")
const char AIRegistryTextPc00755f50[14] = "SetAO_RowDist";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f60")
const char AIRegistryTextPc00755f60[22] = "SetAO_AttackersPerRow";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f78")
const char AIRegistryTextPc00755f78[19] = "SetAO_MaxAttackers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f8c")
const char AIRegistryTextPc00755f8c[16] = "SetUseOneAtOnce";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755f9c")
const char AIRegistryTextPc00755f9c[19] = "AddScriptProcessor";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755fb0")
const char AIRegistryTextPc00755fb0[14] = "WalkBackwards";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755fc0")
const char AIRegistryTextPc00755fc0[15] = "ForceLightning";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755fd0")
const char AIRegistryTextPc00755fd0[19] = "UseTimeBasedUpdate";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755fe4")
const char AIRegistryTextPc00755fe4[19] = "DisableNarrowSocks";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00755ff8")
const char AIRegistryTextPc00755ff8[17] = "SetScaleOverride";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075600c")
const char AIRegistryTextPc0075600c[15] = "ThrowDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075601c")
const char AIRegistryTextPc0075601c[19] = "SpeederBeingChased";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756030")
const char AIRegistryTextPc00756030[17] = "AddTorpedoPacket";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756044")
const char AIRegistryTextPc00756044[12] = "CreateRider";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756050")
const char AIRegistryTextPc00756050[9] = "SetLayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075605c")
const char AIRegistryTextPc0075605c[20] = "SplineFollowTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756070")
const char AIRegistryTextPc00756070[19] = "IgnoreSlideTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756084")
const char AIRegistryTextPc00756084[21] = "AwkwardShapeOverride";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075609c")
const char AIRegistryTextPc0075609c[22] = "IgnoreLastSafePathPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007560b4")
const char AIRegistryTextPc007560b4[13] = "CanBeCarried";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007560c4")
const char AIRegistryTextPc007560c4[15] = "SetCanTakeOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007560d4")
const char AIRegistryTextPc007560d4[15] = "AddMiscPickups";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007560e4")
const char AIRegistryTextPc007560e4[16] = "AddGameMsgCount";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007560f4")
const char AIRegistryTextPc007560f4[20] = "ClearTakeOverTarget";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756108")
const char AIRegistryTextPc00756108[18] = "SetTakeOverTarget";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075611c")
const char AIRegistryTextPc0075611c[23] = "RegisterTakeOverObject";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756134")
const char AIRegistryTextPc00756134[16] = "ReleaseTakeOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756144")
const char AIRegistryTextPc00756144[9] = "TakeOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756150")
const char AIRegistryTextPc00756150[23] = "LinkTurretToController";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756168")
const char AIRegistryTextPc00756168[16] = "SetLastAttacker";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756178")
const char AIRegistryTextPc00756178[15] = "AlertCreatures";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756188")
const char AIRegistryTextPc00756188[11] = "ProbeDroid";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756194")
const char AIRegistryTextPc00756194[25] = "MoveAwayFromLastAttacker";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561b0")
const char AIRegistryTextPc007561b0[18] = "GetLocatorFromSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561c4")
const char AIRegistryTextPc007561c4[14] = "AssignLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561d4")
const char AIRegistryTextPc007561d4[15] = "ReleaseLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561e4")
const char AIRegistryTextPc007561e4[10] = "UseTechno";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561f0")
const char AIRegistryTextPc007561f0[9] = "UsePanel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007561fc")
const char AIRegistryTextPc007561fc[10] = "PullLever";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756208")
const char AIRegistryTextPc00756208[14] = "UseTriggerSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756218")
const char AIRegistryTextPc00756218[17] = "SetObstacleToEnd";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075622c")
const char AIRegistryTextPc0075622c[15] = "PlayGizSpecial";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075623c")
const char AIRegistryTextPc0075623c[13] = "PlayObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075624c")
const char AIRegistryTextPc0075624c[16] = "PlayGizObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075625c")
const char AIRegistryTextPc0075625c[19] = "CanTriggerObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756270")
const char AIRegistryTextPc00756270[22] = "AlwaysTriggerObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756288")
const char AIRegistryTextPc00756288[25] = "RemoveThrownForceObjects";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562a4")
const char AIRegistryTextPc007562a4[22] = "SetFormationCommander";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562bc")
const char AIRegistryTextPc007562bc[12] = "MoveForward";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562c8")
const char AIRegistryTextPc007562c8[13] = "FollowPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562d8")
const char AIRegistryTextPc007562d8[16] = "FollowCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562e8")
const char AIRegistryTextPc007562e8[11] = "SpinOnSpot";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007562f4")
const char AIRegistryTextPc007562f4[14] = "FaceCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756304")
const char AIRegistryTextPc00756304[11] = "FaceCamera";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756310")
const char AIRegistryTextPc00756310[13] = "SetForceBack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756320")
const char AIRegistryTextPc00756320[15] = "DontRaycastLOS";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756330")
const char AIRegistryTextPc00756330[13] = "EndCameraCut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756340")
const char AIRegistryTextPc00756340[17] = "DynamicCameraCut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756354")
const char AIRegistryTextPc00756354[10] = "CameraCut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756360")
const char AIRegistryTextPc00756360[8] = "PlaySfx";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756368")
const char AIRegistryTextPc00756368[10] = "CnxHelper";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756374")
const char AIRegistryTextPc00756374[14] = "CnxController";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756384")
const char AIRegistryTextPc00756384[15] = "CycleCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756394")
const char AIRegistryTextPc00756394[11] = "CancelHint";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563a0")
const char AIRegistryTextPc007563a0[16] = "SetHintComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563b0")
const char AIRegistryTextPc007563b0[8] = "SetHint";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563b8")
const char AIRegistryTextPc007563b8[15] = "SetPathCnxFlag";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563c8")
const char AIRegistryTextPc007563c8[12] = "Respawnable";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563d4")
const char AIRegistryTextPc007563d4[14] = "ImmuneToBolts";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563e4")
const char AIRegistryTextPc007563e4[20] = "ImmuneToKillTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007563f8")
const char AIRegistryTextPc007563f8[13] = "SetLevelPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756408")
const char AIRegistryTextPc00756408[13] = "PlayCutScene";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756418")
const char AIRegistryTextPc00756418[16] = "ResetGameCamera";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756428")
const char AIRegistryTextPc00756428[12] = "CameraShake";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756434")
const char AIRegistryTextPc00756434[17] = "JudderGameCamera";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756448")
const char AIRegistryTextPc00756448[10] = "AddDebris";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756454")
const char AIRegistryTextPc00756454[11] = "EnableSock";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756460")
const char AIRegistryTextPc00756460[14] = "SetVisibility";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756470")
const char AIRegistryTextPc00756470[12] = "NoIdleSpeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075647c")
const char AIRegistryTextPc0075647c[8] = "CanTurn";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756484")
const char AIRegistryTextPc00756484[19] = "CannotBeForcedBack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756498")
const char AIRegistryTextPc00756498[13] = "CannotBeSeen";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007564a8")
const char AIRegistryTextPc007564a8[18] = "IgnoreShoveSystem";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007564bc")
const char AIRegistryTextPc007564bc[13] = "ApplyGravity";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007564cc")
const char AIRegistryTextPc007564cc[17] = "SetGravityHeight";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007564e0")
const char AIRegistryTextPc007564e0[24] = "SetDefaultMovementRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007564f8")
const char AIRegistryTextPc007564f8[20] = "SetMaxMovementRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075650c")
const char AIRegistryTextPc0075650c[16] = "UseCurrentSpeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075651c")
const char AIRegistryTextPc0075651c[14] = "SetHoverPhase";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075652c")
const char AIRegistryTextPc0075652c[20] = "LaunchGuidedMissile";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756540")
const char AIRegistryTextPc00756540[14] = "AddPartDebris";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756550")
const char AIRegistryTextPc00756550[8] = "AddPart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756558")
const char AIRegistryTextPc00756558[15] = "SetScriptParam";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756568")
const char AIRegistryTextPc00756568[12] = "CopyMessage";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756574")
const char AIRegistryTextPc00756574[11] = "SetMessage";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756580")
const char AIRegistryTextPc00756580[19] = "SetShieldHitPoints";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756594")
const char AIRegistryTextPc00756594[13] = "SetHitPoints";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565a4")
const char AIRegistryTextPc007565a4[13] = "SetWalkSpeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565b4")
const char AIRegistryTextPc007565b4[12] = "SetRunSpeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565c0")
const char AIRegistryTextPc007565c0[16] = "SetCurrentSpeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565d0")
const char AIRegistryTextPc007565d0[7] = "Launch";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565d8")
const char AIRegistryTextPc007565d8[22] = "CreateSplineCreatures";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007565f0")
const char AIRegistryTextPc007565f0[19] = "SelectRandomSpline";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756604")
const char AIRegistryTextPc00756604[16] = "CreateCreatures";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756614")
const char AIRegistryTextPc00756614[14] = "FormationMove";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756624")
const char AIRegistryTextPc00756624[15] = "BreakFormation";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756634")
const char AIRegistryTextPc00756634[16] = "FollowDirection";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756644")
const char AIRegistryTextPc00756644[20] = "SetZeroAcceleration";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756658")
const char AIRegistryTextPc00756658[17] = "SetControlSystem";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075666c")
const char AIRegistryTextPc0075666c[10] = "SetSpline";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756678")
const char AIRegistryTextPc00756678[9] = "AddToSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756684")
const char AIRegistryTextPc00756684[16] = "PressJumpButton";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756694")
const char AIRegistryTextPc00756694[19] = "DontAvoidCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566a8")
const char AIRegistryTextPc007566a8[9] = "DontPush";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566b4")
const char AIRegistryTextPc007566b4[16] = "SetInvulnerable";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566c4")
const char AIRegistryTextPc007566c4[10] = "UseWeapon";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566d0")
const char AIRegistryTextPc007566d0[18] = "PressActionButton";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566e4")
const char AIRegistryTextPc007566e4[15] = "PressTagButton";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566f4")
const char AIRegistryTextPc007566f4[19] = "PressSpecialButton";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756708")
const char AIRegistryTextPc00756708[19] = "DontSetStoppedFlag";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075671c")
const char AIRegistryTextPc0075671c[12] = "SetDontMove";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756728")
const char AIRegistryTextPc00756728[19] = "SetLastSafePathPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075673c")
const char AIRegistryTextPc0075673c[21] = "SetAIOverrideControl";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756754")
const char AIRegistryTextPc00756754[15] = "SetScriptState";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756764")
const char AIRegistryTextPc00756764[8] = "Explode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075676c")
const char AIRegistryTextPc0075676c[5] = "Kill";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756774")
const char AIRegistryTextPc00756774[19] = "DeflectPlayersPart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756788")
const char AIRegistryTextPc00756788[10] = "ForcePush";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756794")
const char AIRegistryTextPc00756794[14] = "TriggerBlowUp";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567a4")
const char AIRegistryTextPc007567a4[9] = "UseForce";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567b0")
const char AIRegistryTextPc007567b0[14] = "UpdateSockPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567c0")
const char AIRegistryTextPc007567c0[8] = "SetBoss";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567c8")
const char AIRegistryTextPc007567c8[13] = "CanUseWeapon";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567d8")
const char AIRegistryTextPc007567d8[10] = "DontAimAt";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567e4")
const char AIRegistryTextPc007567e4[10] = "CanDefend";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007567f0")
const char AIRegistryTextPc007567f0[14] = "ReleaseVictim";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756800")
const char AIRegistryTextPc00756800[10] = "EatVictim";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075680c")
const char AIRegistryTextPc0075680c[11] = "GrabVictim";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756818")
const char AIRegistryTextPc00756818[13] = "EngageObject";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756828")
const char AIRegistryTextPc00756828[16] = "ShootAtOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756838")
const char AIRegistryTextPc00756838[15] = "EngageOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756848")
const char AIRegistryTextPc00756848[15] = "AttackOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756858")
const char AIRegistryTextPc00756858[12] = "SetOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756864")
const char AIRegistryTextPc00756864[13] = "SetStateArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756874")
const char AIRegistryTextPc00756874[8] = "SetSide";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075687c")
const char AIRegistryTextPc0075687c[16] = "SetAnimSpeedMul";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075688c")
const char AIRegistryTextPc0075688c[18] = "PlayerSpeederHack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568a0")
const char AIRegistryTextPc007568a0[15] = "AlwaysBackFlip";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568b0")
const char AIRegistryTextPc007568b0[19] = "CanHitForceObjects";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568c4")
const char AIRegistryTextPc007568c4[13] = "TagCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568d4")
const char AIRegistryTextPc007568d4[11] = "TakeDamage";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568e0")
const char AIRegistryTextPc007568e0[13] = "NotWithParty";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568f0")
const char AIRegistryTextPc007568f0[10] = "CanAttack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007568fc")
const char AIRegistryTextPc007568fc[13] = "CannotDropIn";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075690c")
const char AIRegistryTextPc0075690c[17] = "CatchUpForbidden";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756920")
const char AIRegistryTextPc00756920[12] = "SetTaggable";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075692c")
const char AIRegistryTextPc0075692c[17] = "UseBigJumpToJump";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756940")
const char AIRegistryTextPc00756940[21] = "CanShootObstructions";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756958")
const char AIRegistryTextPc00756958[29] = "SetBoltsDontGetDeflectedBack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756978")
const char AIRegistryTextPc00756978[15] = "PrefersPlayers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756988")
const char AIRegistryTextPc00756988[13] = "ResetContext";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756998")
const char AIRegistryTextPc00756998[14] = "SnapWeaponOut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569a8")
const char AIRegistryTextPc007569a8[14] = "KeepWeaponOut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569b8")
const char AIRegistryTextPc007569b8[18] = "CanShootOffScreen";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569cc")
const char AIRegistryTextPc007569cc[13] = "CanOpenDoors";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569dc")
const char AIRegistryTextPc007569dc[15] = "AnimTimeRandom";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569ec")
const char AIRegistryTextPc007569ec[13] = "SetAnimation";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007569fc")
const char AIRegistryTextPc007569fc[19] = "SnapToSockPosition";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a10")
const char AIRegistryTextPc00756a10[15] = "SnapToPosition";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a20")
const char AIRegistryTextPc00756a20[23] = "SetDoomedEscapeLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a38")
const char AIRegistryTextPc00756a38[8] = "BigJump";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a40")
const char AIRegistryTextPc00756a40[17] = "BigJumpToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a54")
const char AIRegistryTextPc00756a54[13] = "SnapToOrigin";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a64")
const char AIRegistryTextPc00756a64[14] = "SetLocatorSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a74")
const char AIRegistryTextPc00756a74[11] = "SetLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a80")
const char AIRegistryTextPc00756a80[14] = "SnapToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a90")
const char AIRegistryTextPc00756a90[17] = "GoToOriginalPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756aa4")
const char AIRegistryTextPc00756aa4[8] = "SetPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756aac")
const char AIRegistryTextPc00756aac[14] = "GoToLevelPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756abc")
const char AIRegistryTextPc00756abc[11] = "DeActivate";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ac8")
const char AIRegistryTextPc00756ac8[9] = "Activate";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ad4")
const char AIRegistryTextPc00756ad4[10] = "InHubArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ae0")
const char AIRegistryTextPc00756ae0[19] = "NetworkGameOnGoing";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756af4")
const char AIRegistryTextPc00756af4[33] = "InSameTriggerAreaAsNearestPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b18")
const char AIRegistryTextPc00756b18[8] = "InSwamp";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b20")
const char AIRegistryTextPc00756b20[21] = "ShouldAttackOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b38")
const char AIRegistryTextPc00756b38[13] = "AreaComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b48")
const char AIRegistryTextPc00756b48[16] = "CharacterLoaded";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b58")
const char AIRegistryTextPc00756b58[8] = "MusicOn";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b60")
const char AIRegistryTextPc00756b60[8] = "RaceLap";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b68")
const char AIRegistryTextPc00756b68[13] = "CanHearRadio";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b78")
const char AIRegistryTextPc00756b78[16] = "FlowBoxComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b88")
const char AIRegistryTextPc00756b88[19] = "PickupBeenTurnedOn";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756b9c")
const char AIRegistryTextPc00756b9c[13] = "AnimSpeedMul";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756bac")
const char AIRegistryTextPc00756bac[28] = "AngleAboutMyLocatorToPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756bc8")
const char AIRegistryTextPc00756bc8[16] = "GizmoVisibility";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756bd8")
const char AIRegistryTextPc00756bd8[13] = "GizmoOutput3";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756be8")
const char AIRegistryTextPc00756be8[13] = "GizmoOutput2";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756bf8")
const char AIRegistryTextPc00756bf8[13] = "GizmoOutput1";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c08")
const char AIRegistryTextPc00756c08[13] = "GizmoOutput0";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c18")
const char AIRegistryTextPc00756c18[21] = "OpponentPathPosRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c30")
const char AIRegistryTextPc00756c30[22] = "OpponentToPlayerRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c48")
const char AIRegistryTextPc00756c48[20] = "NearestPartyXZRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c5c")
const char AIRegistryTextPc00756c5c[18] = "NearestPartyRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c70")
const char AIRegistryTextPc00756c70[5] = "Side";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c78")
const char AIRegistryTextPc00756c78[11] = "ShopActive";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c84")
const char AIRegistryTextPc00756c84[19] = "HeadTurnRestricted";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756c98")
const char AIRegistryTextPc00756c98[11] = "IAmPlayer2";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ca4")
const char AIRegistryTextPc00756ca4[11] = "ScreenWipe";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756cb0")
const char AIRegistryTextPc00756cb0[6] = "MySet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756cb8")
const char AIRegistryTextPc00756cb8[10] = "IsVisible";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756cc4")
const char AIRegistryTextPc00756cc4[10] = "GotVictim";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756cd0")
const char AIRegistryTextPc00756cd0[28] = "EitherPlayerInMyTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756cec")
const char AIRegistryTextPc00756cec[28] = "TakeOverTargetInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d08")
const char AIRegistryTextPc00756d08[14] = "TakeOverRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d18")
const char AIRegistryTextPc00756d18[18] = "HasTakeOverTarget";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d2c")
const char AIRegistryTextPc00756d2c[14] = "EmptyTakeOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d3c")
const char AIRegistryTextPc00756d3c[15] = "GotOpponentLOS";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d4c")
const char AIRegistryTextPc00756d4c[16] = "GotLocatorInSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d5c")
const char AIRegistryTextPc00756d5c[20] = "CharacterTypeExists";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d70")
const char AIRegistryTextPc00756d70[16] = "CharacterExists";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d80")
const char AIRegistryTextPc00756d80[19] = "UnderPlayerControl";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756d94")
const char AIRegistryTextPc00756d94[14] = "OnSpeederBike";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756da4")
const char AIRegistryTextPc00756da4[14] = "BeenTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756db4")
const char AIRegistryTextPc00756db4[22] = "EitherPlayerTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756dcc")
const char AIRegistryTextPc00756dcc[16] = "PlayerTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ddc")
const char AIRegistryTextPc00756ddc[10] = "TakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756de8")
const char AIRegistryTextPc00756de8[23] = "CannotReachDestination";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e00")
const char AIRegistryTextPc00756e00[20] = "PartyContainsDroids";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e14")
const char AIRegistryTextPc00756e14[27] = "LastAttackerIsActivePlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e30")
const char AIRegistryTextPc00756e30[18] = "LastAttackerRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e44")
const char AIRegistryTextPc00756e44[12] = "BeenSpawned";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e50")
const char AIRegistryTextPc00756e50[15] = "CharacterRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e60")
const char AIRegistryTextPc00756e60[27] = "EitherPlayerPushingSpinner";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e7c")
const char AIRegistryTextPc00756e7c[17] = "HelpWithTriggers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e90")
const char AIRegistryTextPc00756e90[16] = "DropBackInTimer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ea0")
const char AIRegistryTextPc00756ea0[18] = "MaulShouldRunAway";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756eb4")
const char AIRegistryTextPc00756eb4[10] = "InMiniCut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ec0")
const char AIRegistryTextPc00756ec0[17] = "RespawnLocatorIs";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ed4")
const char AIRegistryTextPc00756ed4[16] = "BigJumpComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ee4")
const char AIRegistryTextPc00756ee4[14] = "CheatProgress";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756ef4")
const char AIRegistryTextPc00756ef4[26] = "BoltsDontGetDeflectedBack";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f10")
const char AIRegistryTextPc00756f10[18] = "AIOverrideControl";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f24")
const char AIRegistryTextPc00756f24[4] = "PSP";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f28")
const char AIRegistryTextPc00756f28[14] = "ChallengeMode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f38")
const char AIRegistryTextPc00756f38[11] = "MissionWon";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f44")
const char AIRegistryTextPc00756f44[12] = "MissionMode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f50")
const char AIRegistryTextPc00756f50[5] = "Indy";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f58")
const char AIRegistryTextPc00756f58[13] = "HintComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f68")
const char AIRegistryTextPc00756f68[14] = "HintAvailable";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f78")
const char AIRegistryTextPc00756f78[14] = "CurrentHintId";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f88")
const char AIRegistryTextPc00756f88[15] = "FinishedSpline";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756f98")
const char AIRegistryTextPc00756f98[32] = "FurthestPlayerDistanceAlongSock";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756fb8")
const char AIRegistryTextPc00756fb8[24] = "PlayerDistanceAlongSock";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756fd0")
const char AIRegistryTextPc00756fd0[22] = "SockXDistanceToPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756fe8")
const char AIRegistryTextPc00756fe8[23] = "SockDistanceToOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757000")
const char AIRegistryTextPc00757000[21] = "SockDistanceToPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757018")
const char AIRegistryTextPc00757018[15] = "RigidAnimFrame";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757028")
const char AIRegistryTextPc00757028[16] = "CutScenePlaying";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757038")
const char AIRegistryTextPc00757038[13] = "PlayerInSock";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757048")
const char AIRegistryTextPc00757048[15] = "CutSceneExists";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757058")
const char AIRegistryTextPc00757058[17] = "CutSceneFinished";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075706c")
const char AIRegistryTextPc0075706c[16] = "CutSceneStarted";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075707c")
const char AIRegistryTextPc0075707c[12] = "ScriptParam";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757088")
const char AIRegistryTextPc00757088[8] = "Message";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757090")
const char AIRegistryTextPc00757090[10] = "LastLevel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075709c")
const char AIRegistryTextPc0075709c[12] = "BeenToLevel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570a8")
const char AIRegistryTextPc007570a8[16] = "NumForceObjects";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570b8")
const char AIRegistryTextPc007570b8[11] = "NumBaddies";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570c4")
const char AIRegistryTextPc007570c4[14] = "Player2Active";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570d4")
const char AIRegistryTextPc007570d4[16] = "OpponentContext";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570e4")
const char AIRegistryTextPc007570e4[10] = "InContext";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570f0")
const char AIRegistryTextPc007570f0[8] = "Context";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007570f8")
const char AIRegistryTextPc007570f8[14] = "NumInSetAlive";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757108")
const char AIRegistryTextPc00757108[11] = "IsSetAlive";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757114")
const char AIRegistryTextPc00757114[10] = "Player2Is";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757120")
const char AIRegistryTextPc00757120[10] = "Player1Is";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075712c")
const char AIRegistryTextPc0075712c[15] = "EitherPlayerIs";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075713c")
const char AIRegistryTextPc0075713c[17] = "PlayerCategoryIs";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757150")
const char AIRegistryTextPc00757150[11] = "CategoryIs";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075715c")
const char AIRegistryTextPc0075715c[19] = "IAmAPartyCharacter";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757170")
const char AIRegistryTextPc00757170[15] = "IAmAGoodyBaddy";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757180")
const char AIRegistryTextPc00757180[12] = "IAmANeutral";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075718c")
const char AIRegistryTextPc0075718c[10] = "IAmABaddy";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757198")
const char AIRegistryTextPc00757198[10] = "IAmAGoody";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571a4")
const char AIRegistryTextPc007571a4[18] = "CanFightLikeAJedi";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571b8")
const char AIRegistryTextPc007571b8[18] = "OpponentIsAThreat";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571cc")
const char AIRegistryTextPc007571cc[12] = "OpponentIsA";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571d8")
const char AIRegistryTextPc007571d8[5] = "IAmA";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571e0")
const char AIRegistryTextPc007571e0[14] = "BlowupBlownup";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571f0")
const char AIRegistryTextPc007571f0[16] = "BuildItComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757200")
const char AIRegistryTextPc00757200[14] = "ForceFinished";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757210")
const char AIRegistryTextPc00757210[14] = "ForceComplete";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757220")
const char AIRegistryTextPc00757220[21] = "PlayerDeflectingPart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757238")
const char AIRegistryTextPc00757238[12] = "TurretAlive";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757244")
const char AIRegistryTextPc00757244[13] = "ForcePushing";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757254")
const char AIRegistryTextPc00757254[15] = "ForceBeingUsed";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757264")
const char AIRegistryTextPc00757264[28] = "EitherPlayerOnForcePlatform";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757280")
const char AIRegistryTextPc00757280[22] = "PlayerOnForcePlatform";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757298")
const char AIRegistryTextPc00757298[16] = "OnForcePlatform";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007572a8")
const char AIRegistryTextPc007572a8[11] = "UsingForce";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007572b4")
const char AIRegistryTextPc007572b4[23] = "EitherPlayerUsingForce";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007572cc")
const char AIRegistryTextPc007572cc[17] = "PlayerUsingForce";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007572e0")
const char AIRegistryTextPc007572e0[28] = "NumBaddiesThatCanSeePlayers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007572fc")
const char AIRegistryTextPc007572fc[16] = "PartyUnderCover";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075730c")
const char AIRegistryTextPc0075730c[26] = "EitherPlayerWearingHelmet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757328")
const char AIRegistryTextPc00757328[23] = "EitherPlayerUsingPanel";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757340")
const char AIRegistryTextPc00757340[28] = "EitherPlayerUsingHatMachine";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075735c")
const char AIRegistryTextPc0075735c[25] = "EitherPlayerPullingLever";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757378")
const char AIRegistryTextPc00757378[18] = "AnimationFinished";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075738c")
const char AIRegistryTextPc0075738c[29] = "ObstacleOpenedByEitherPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007573ac")
const char AIRegistryTextPc007573ac[23] = "ObstacleOpenedByPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007573c4")
const char AIRegistryTextPc007573c4[11] = "ForceAtEnd";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007573d0")
const char AIRegistryTextPc007573d0[13] = "ForceAtStart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007573e0")
const char AIRegistryTextPc007573e0[19] = "ObstacleLockedShut";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007573f4")
const char AIRegistryTextPc007573f4[19] = "ObstacleLockedOpen";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757408")
const char AIRegistryTextPc00757408[13] = "SpecialAtEnd";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757418")
const char AIRegistryTextPc00757418[15] = "SpecialAtStart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757428")
const char AIRegistryTextPc00757428[14] = "ObstacleAtEnd";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757438")
const char AIRegistryTextPc00757438[16] = "ObstacleAtStart";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757448")
const char AIRegistryTextPc00757448[10] = "Colliding";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757454")
const char AIRegistryTextPc00757454[22] = "CollidingWithOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075746c")
const char AIRegistryTextPc0075746c[5] = "ZPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757474")
const char AIRegistryTextPc00757474[5] = "YPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075747c")
const char AIRegistryTextPc0075747c[5] = "XPos";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757484")
const char AIRegistryTextPc00757484[17] = "OnDynamicGrapple";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757498")
const char AIRegistryTextPc00757498[10] = "HitPoints";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574a4")
const char AIRegistryTextPc007574a4[11] = "HoverPhase";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574b0")
const char AIRegistryTextPc007574b0[8] = "BeenHit";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574b8")
const char AIRegistryTextPc007574b8[9] = "Blocking";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574c4")
const char AIRegistryTextPc007574c4[16] = "LocatorOnScreen";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574d4")
const char AIRegistryTextPc007574d4[13] = "BehindCamera";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574e4")
const char AIRegistryTextPc007574e4[11] = "SpawnCount";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007574f0")
const char AIRegistryTextPc007574f0[15] = "PlayerOnGround";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757500")
const char AIRegistryTextPc00757500[12] = "BeenAlerted";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075750c")
const char AIRegistryTextPc0075750c[9] = "OnGround";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757518")
const char AIRegistryTextPc00757518[27] = "EitherPlayerLocatorRangeXZ";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757534")
const char AIRegistryTextPc00757534[21] = "EitherPlayerOnObject";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075754c")
const char AIRegistryTextPc0075754c[15] = "PlayerOnObject";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075755c")
const char AIRegistryTextPc0075755c[21] = "OnSameObjectAsPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757574")
const char AIRegistryTextPc00757574[9] = "OnObject";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757580")
const char AIRegistryTextPc00757580[15] = "OffScreenTimer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00757590")
const char AIRegistryTextPc00757590[11] = "IsOnScreen";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075759c")
const char AIRegistryTextPc0075759c[8] = "IsAlive";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575a4")
const char AIRegistryTextPc007575a4[16] = "PrefersBrawling";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575b4")
const char AIRegistryTextPc007575b4[7] = "GotGun";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575bc")
const char AIRegistryTextPc007575bc[7] = "Active";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575c4")
const char AIRegistryTextPc007575c4[6] = "Debug";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575cc")
const char AIRegistryTextPc007575cc[9] = "GlynTest";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f8b8")
const char AIRegistryTextPc0075f8b8[18] = "NotifyStateChange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f8cc")
const char AIRegistryTextPc0075f8cc[14] = "ReturnToState";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f8dc")
const char AIRegistryTextPc0075f8dc[17] = "SetReturnToState";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f8f0")
const char AIRegistryTextPc0075f8f0[9] = "SetParam";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f8fc")
const char AIRegistryTextPc0075f8fc[10] = "NoShadows";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f908")
const char AIRegistryTextPc0075f908[19] = "SetIgnoreAntinodes";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f91c")
const char AIRegistryTextPc0075f91c[15] = "ClearInterrupt";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f92c")
const char AIRegistryTextPc0075f92c[13] = "SetInterrupt";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f93c")
const char AIRegistryTextPc0075f93c[14] = "ResetToOrigin";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f94c")
const char AIRegistryTextPc0075f94c[11] = "NoLosCheck";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f958")
const char AIRegistryTextPc0075f958[24] = "PathConnectionMaxLength";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f970")
const char AIRegistryTextPc0075f970[23] = "PathConnectionObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f988")
const char AIRegistryTextPc0075f988[10] = "BlockPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f994")
const char AIRegistryTextPc0075f994[18] = "OverrideAnimation";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9a8")
const char AIRegistryTextPc0075f9a8[17] = "MoveAwayFromNode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9bc")
const char AIRegistryTextPc0075f9bc[11] = "FollowPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9c8")
const char AIRegistryTextPc0075f9c8[18] = "SetRespawnLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9dc")
const char AIRegistryTextPc0075f9dc[12] = "GoToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9e8")
const char AIRegistryTextPc0075f9e8[11] = "GoToOrigin";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075f9f4")
const char AIRegistryTextPc0075f9f4[15] = "GoToNodeRandom";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa04")
const char AIRegistryTextPc0075fa04[9] = "GoToNode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa10")
const char AIRegistryTextPc0075fa10[14] = "SetMoveRadius";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa20")
const char AIRegistryTextPc0075fa20[16] = "SetHearDistance";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa30")
const char AIRegistryTextPc0075fa30[17] = "SetMinViewHeight";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa44")
const char AIRegistryTextPc0075fa44[17] = "SetMaxViewHeight";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa58")
const char AIRegistryTextPc0075fa58[16] = "SetViewDistance";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa68")
const char AIRegistryTextPc0075fa68[18] = "SetFullPathSearch";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa7c")
const char AIRegistryTextPc0075fa7c[12] = "RequiresLOS";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa88")
const char AIRegistryTextPc0075fa88[13] = "CanSeeBehind";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fa98")
const char AIRegistryTextPc0075fa98[21] = "DontUseShadowTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fab0")
const char AIRegistryTextPc0075fab0[14] = "ShadowTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fac0")
const char AIRegistryTextPc0075fac0[12] = "FlatTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075facc")
const char AIRegistryTextPc0075facc[10] = "NoTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fad8")
const char AIRegistryTextPc0075fad8[17] = "CheckWallSplines";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075faec")
const char AIRegistryTextPc0075faec[18] = "IgnoreWallSplines";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb00")
const char AIRegistryTextPc0075fb00[12] = "FaceLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb0c")
const char AIRegistryTextPc0075fb0c[13] = "FaceOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb1c")
const char AIRegistryTextPc0075fb1c[11] = "FacePlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb28")
const char AIRegistryTextPc0075fb28[15] = "FollowOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb38")
const char AIRegistryTextPc0075fb38[21] = "MoveAwayFromOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb50")
const char AIRegistryTextPc0075fb50[13] = "CirclePlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb60")
const char AIRegistryTextPc0075fb60[15] = "CircleOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb70")
const char AIRegistryTextPc0075fb70[7] = "Circle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb78")
const char AIRegistryTextPc0075fb78[19] = "SetCircleDirection";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fb8c")
const char AIRegistryTextPc0075fb8c[20] = "MoveAwayFromPlayer2";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fba0")
const char AIRegistryTextPc0075fba0[19] = "MoveAwayFromPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fbb4")
const char AIRegistryTextPc0075fbb4[20] = "RetreatFromOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fbc8")
const char AIRegistryTextPc0075fbc8[27] = "RetreatFromNearestOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fbe4")
const char AIRegistryTextPc0075fbe4[11] = "ResetTimer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fbf0")
const char AIRegistryTextPc0075fbf0[9] = "SetState";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fbfc")
const char AIRegistryTextPc0075fbfc[5] = "Idle";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc04")
const char AIRegistryTextPc0075fc04[6] = "Param";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc0c")
const char AIRegistryTextPc0075fc0c[10] = "StuckTime";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc18")
const char AIRegistryTextPc0075fc18[4] = "IAm";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc1c")
const char AIRegistryTextPc0075fc1c[12] = "InterruptID";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc28")
const char AIRegistryTextPc0075fc28[12] = "PathBlocked";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc34")
const char AIRegistryTextPc0075fc34[12] = "TimeOffPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc40")
const char AIRegistryTextPc0075fc40[15] = "OpponentOnPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc50")
const char AIRegistryTextPc0075fc50[13] = "PlayerOnPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc60")
const char AIRegistryTextPc0075fc60[7] = "OnPath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc68")
const char AIRegistryTextPc0075fc68[23] = "NearestPlayerToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc80")
const char AIRegistryTextPc0075fc80[16] = "PlayerToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fc90")
const char AIRegistryTextPc0075fc90[19] = "OpponentToLocatorY";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fca4")
const char AIRegistryTextPc0075fca4[20] = "OpponentToLocatorXZ";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fcb8")
const char AIRegistryTextPc0075fcb8[18] = "OpponentToLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fccc")
const char AIRegistryTextPc0075fccc[15] = "PlayerToOrigin";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fcdc")
const char AIRegistryTextPc0075fcdc[17] = "OpponentToOrigin";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fcf0")
const char AIRegistryTextPc0075fcf0[12] = "OriginRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fcfc")
const char AIRegistryTextPc0075fcfc[14] = "OpponentBelow";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd0c")
const char AIRegistryTextPc0075fd0c[14] = "YawToOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd1c")
const char AIRegistryTextPc0075fd1c[21] = "NearestOpponentRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd34")
const char AIRegistryTextPc0075fd34[14] = "OpponentRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd44")
const char AIRegistryTextPc0075fd44[19] = "OpponentOnSamePath";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd58")
const char AIRegistryTextPc0075fd58[12] = "GotOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd64")
const char AIRegistryTextPc0075fd64[22] = "OpponentInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd7c")
const char AIRegistryTextPc0075fd7c[19] = "GoodyInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fd90")
const char AIRegistryTextPc0075fd90[19] = "BaddyInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fda4")
const char AIRegistryTextPc0075fda4[26] = "EitherPlayerInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fdc0")
const char AIRegistryTextPc0075fdc0[21] = "Player2InTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fdd8")
const char AIRegistryTextPc0075fdd8[20] = "PlayerInTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fdec")
const char AIRegistryTextPc0075fdec[15] = "GotTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fdfc")
const char AIRegistryTextPc0075fdfc[10] = "NodeRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe08")
const char AIRegistryTextPc0075fe08[24] = "EitherPlayerInLevelNode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe20")
const char AIRegistryTextPc0075fe20[18] = "PlayerInLevelNode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe34")
const char AIRegistryTextPc0075fe34[12] = "InLevelNode";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe40")
const char AIRegistryTextPc0075fe40[21] = "NearestPlayerXZRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe58")
const char AIRegistryTextPc0075fe58[19] = "NearestPlayerRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe6c")
const char AIRegistryTextPc0075fe6c[12] = "PlayerRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe78")
const char AIRegistryTextPc0075fe78[14] = "InTriggerArea";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe88")
const char AIRegistryTextPc0075fe88[17] = "CurrentLocatorIs";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fe9c")
const char AIRegistryTextPc0075fe9c[14] = "GotLocatorSet";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075feac")
const char AIRegistryTextPc0075feac[11] = "GotLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075feb8")
const char AIRegistryTextPc0075feb8[7] = "Random";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fec0")
const char AIRegistryTextPc0075fec0[6] = "Timer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fec8")
const char AIRegistryTextPc0075fec8[14] = "LocatorRangeY";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fed8")
const char AIRegistryTextPc0075fed8[15] = "LocatorRangeXZ";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fee8")
const char AIRegistryTextPc0075fee8[13] = "LocatorRange";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075fef8")
const char AIRegistryTextPc0075fef8[11] = "AlwaysTrue";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc0075ff04")
const char AIRegistryTextPc0075ff04[15] = "PreviousResult";
#pragma const_seg()

#pragma data_seg(".data$lego_aiactiondefs")
AIACTIONDEF_PC lego_aiactiondefs[208] = {
    {AIRegistryTextPc00756ac8, Action_Activate, 1, 0, 0},
    {AIRegistryTextPc00756abc, Action_DeActivate, 1, 0, 0},
    {AIRegistryTextPc00756aac, Action_GoToLevelPath, 0, 0, 0},
    {AIRegistryTextPc00756aa4, Action_SetPath, 0, 0, 0},
    {AIRegistryTextPc00756a90, Action_GoToOriginalPath, 0, 0, 0},
    {AIRegistryTextPc00756a80, Action_SnapToLocator, 1, 0, 0},
    {AIRegistryTextPc00756a74, Action_GameSetLocator, 1, 0, 0},
    {AIRegistryTextPc00756a64, Action_SetLocatorSet, 0, 0, 0},
    {AIRegistryTextPc00756a54, Action_SnapToOrigin, 1, 0, 0},
    {AIRegistryTextPc00756a40, Action_BigJumpToLocator, 0, 0, 0},
    {AIRegistryTextPc00756a38, Action_BigJump, 0, 0, 0},
    {AIRegistryTextPc00756a20, Action_SetDoomedEscapeLocator, 0, 0, 0},
    {AIRegistryTextPc00756a10, Action_SnapToPosition, 1, 0, 0},
    {AIRegistryTextPc007569fc, Action_SnapToSockPosition, 1, 0, 0},
    {AIRegistryTextPc007569ec, Action_SetAnimation, 0, 0, 0},
    {AIRegistryTextPc007569dc, Action_AnimTimeRandom, 0, 0, 0},
    {AIRegistryTextPc007569cc, Action_CanOpenDoors, 0, 0, 0},
    {AIRegistryTextPc007569b8, Action_CanShootOffScreen, 0, 0, 0},
    {AIRegistryTextPc007569a8, Action_KeepWeaponOut, 0, 0, 0},
    {AIRegistryTextPc00756998, Action_SnapWeaponOut, 1, 0, 0},
    {AIRegistryTextPc00756988, Action_ResetContext, 0, 0, 0},
    {AIRegistryTextPc00756978, Action_PrefersPlayers, 0, 0, 0},
    {AIRegistryTextPc00756958, Action_SetBoltsDontGetDeflectedBack, 0, 0, 0},
    {AIRegistryTextPc00756940, Action_CanShootObstructions, 0, 0, 0},
    {AIRegistryTextPc0075692c, Action_UseBigJumpToJump, 0, 0, 0},
    {AIRegistryTextPc00756920, Action_SetTaggable, 1, 0, 0},
    {AIRegistryTextPc0075690c, Action_CatchUpForbidden, 0, 0, 0},
    {AIRegistryTextPc007568fc, Action_CannotDropIn, 1, 0, 0},
    {AIRegistryTextPc007568f0, Action_CanAttack, 0, 0, 0},
    {AIRegistryTextPc007568e0, Action_NotWithParty, 1, 0, 0},
    {AIRegistryTextPc007568d4, Action_TakeDamage, 0, 0, 0},
    {AIRegistryTextPc007568c4, Action_TagCharacter, 1, 0, 0},
    {AIRegistryTextPc007568b0, Action_CanHitForceObjects, 0, 0, 0},
    {AIRegistryTextPc007568a0, Action_AlwaysBackFlip, 0, 0, 0},
    {AIRegistryTextPc0075688c, Action_PlayerSpeederHack, 0, 0, 0},
    {AIRegistryTextPc0075687c, Action_SetAnimSpeedMul, 0, 0, 0},
    {AIRegistryTextPc00756874, Action_SetSide, 1, 0, 0},
    {AIRegistryTextPc00756864, Action_SetStateArea, 0, 0, 0},
    {AIRegistryTextPc00756858, Action_SetOpponent, 0, 0, 0},
    {AIRegistryTextPc00756848, Action_AttackOpponent, 0, 0, 0},
    {AIRegistryTextPc00756838, Action_EngageOpponent, 0, 0, 0},
    {AIRegistryTextPc00756828, Action_ShootAtOpponent, 0, 0, 0},
    {AIRegistryTextPc00756818, Action_EngageObject, 0, 0, 0},
    {AIRegistryTextPc0075680c, Action_GrabVictim, 0, 0, 0},
    {AIRegistryTextPc00756800, Action_EatVictim, 0, 0, 0},
    {AIRegistryTextPc007567f0, Action_ReleaseVictim, 0, 0, 0},
    {AIRegistryTextPc007567e4, Action_CanDefend, 0, 0, 0},
    {AIRegistryTextPc007567d8, Action_DontAimAt, 0, 0, 0},
    {AIRegistryTextPc007567c8, Action_CanUseWeapon, 0, 0, 0},
    {AIRegistryTextPc007567c0, Action_SetBoss, 0, 0, 0},
    {AIRegistryTextPc007567b0, Action_UpdateSockPos, 0, 0, 0},
    {AIRegistryTextPc007567a4, Action_UseForce, 0, 0, 0},
    {AIRegistryTextPc00756794, Action_TriggerBlowUp, 0, 0, 0},
    {AIRegistryTextPc00756788, Action_ForcePush, 0, 0, 0},
    {AIRegistryTextPc00756774, Action_DeflectPlayersPart, 0, 0, 0},
    {AIRegistryTextPc0075676c, Action_Kill, 1, 0, 0},
    {AIRegistryTextPc00756764, Action_Explode, 0, 0, 0},
    {AIRegistryTextPc00756754, Action_SetScriptState, 0, 0, 0},
    {AIRegistryTextPc0075673c, Action_SetAIOverrideControl, 0, 0, 0},
    {AIRegistryTextPc00756728, Action_SetLastSafePathPos, 0, 0, 0},
    {AIRegistryTextPc0075671c, Action_SetDontMove, 0, 0, 0},
    {AIRegistryTextPc00756708, Action_DontSetStoppedFlag, 0, 0, 0},
    {AIRegistryTextPc007566f4, Action_PressSpecialButton, 0, 0, 0},
    {AIRegistryTextPc007566e4, Action_PressTagButton, 0, 0, 0},
    {AIRegistryTextPc007566d0, Action_PressActionButton, 0, 0, 0},
    {AIRegistryTextPc007566c4, Action_UseWeapon, 0, 0, 0},
    {AIRegistryTextPc007566b4, Action_SetInvulnerable, 0, 0, 0},
    {AIRegistryTextPc007566a8, Action_GameDontPush, 0, 0, 0},
    {AIRegistryTextPc00756694, Action_DontAvoidCharacter, 0, 0, 0},
    {AIRegistryTextPc00756684, Action_PressJumpButton, 0, 0, 0},
    {AIRegistryTextPc00756678, Action_AddToSet, 0, 0, 0},
    {AIRegistryTextPc0075666c, Action_SetSpline, 0, 0, 0},
    {AIRegistryTextPc00756658, Action_SetControlSystem, 0, 0, 0},
    {AIRegistryTextPc00756644, Action_SetZeroAcceleration, 0, 0, 0},
    {AIRegistryTextPc00756634, Action_FollowDirection, 0, 0, 0},
    {AIRegistryTextPc00756624, Action_BreakFormation, 0, 0, 0},
    {AIRegistryTextPc00756614, Action_FormationMove, 0, 0, 0},
    {AIRegistryTextPc00756604, Action_CreateCreatures, 0, 0, 0},
    {AIRegistryTextPc007565f0, Action_SelectRandomSpline, 0, 0, 0},
    {AIRegistryTextPc007565d8, Action_CreateSplineCreatures, 0, 0, 0},
    {AIRegistryTextPc007565d0, Action_Launch, 0, 0, 0},
    {AIRegistryTextPc007565c0, Action_SetCurrentSpeed, 0, 0, 0},
    {AIRegistryTextPc007565b4, Action_SetRunSpeed, 0, 0, 0},
    {AIRegistryTextPc007565a4, Action_SetWalkSpeed, 0, 0, 0},
    {AIRegistryTextPc00756594, Action_SetHitPoints, 1, 0, 0},
    {AIRegistryTextPc00756580, Action_SetShieldHitPoints, 1, 0, 0},
    {AIRegistryTextPc00756574, Action_SetMessage, 1, 0, 0},
    {AIRegistryTextPc00756568, Action_CopyMessage, 0, 0, 0},
    {AIRegistryTextPc00756558, Action_SetScriptParam, 0, 0, 0},
    {AIRegistryTextPc00756550, Action_AddPart, 0, 0, 0},
    {AIRegistryTextPc00756540, Action_AddPartDebris, 0, 0, 0},
    {AIRegistryTextPc0075652c, Action_LaunchGuidedMissile, 0, 0, 0},
    {AIRegistryTextPc0075651c, Action_SetHoverPhase, 0, 0, 0},
    {AIRegistryTextPc0075650c, Action_UseCurrentSpeed, 0, 0, 0},
    {AIRegistryTextPc007564f8, Action_SetMaxMovementRange, 0, 0, 0},
    {AIRegistryTextPc007564e0, Action_SetDefaultMovementRange, 0, 0, 0},
    {AIRegistryTextPc007564cc, Action_SetGravityHeight, 0, 0, 0},
    {AIRegistryTextPc007564bc, Action_ApplyGravity, 0, 0, 0},
    {AIRegistryTextPc007564a8, Action_IgnoreShoveSystem, 0, 0, 0},
    {AIRegistryTextPc00756498, Action_CannotBeSeen, 0, 0, 0},
    {AIRegistryTextPc00756484, Action_CannotBeForcedBack, 0, 0, 0},
    {AIRegistryTextPc0075647c, Action_CanTurn, 0, 0, 0},
    {AIRegistryTextPc00756470, Action_NoIdleSpeed, 0, 0, 0},
    {AIRegistryTextPc00756460, Action_SetVisibility, 1, 0, 0},
    {AIRegistryTextPc00756454, Action_EnableSock, 1, 0, 0},
    {AIRegistryTextPc00756448, Action_AddDebris, 0, 0, 0},
    {AIRegistryTextPc00756434, Action_JudderGameCamera, 0, 0, 0},
    {AIRegistryTextPc00756428, Action_CameraShake, 0, 0, 0},
    {AIRegistryTextPc00756418, Action_ResetGameCamera, 1, 0, 0},
    {AIRegistryTextPc00756408, Action_PlayCutScene, 1, 0, 0},
    {AIRegistryTextPc007563f8, Action_SetLevelPath, 0, 0, 0},
    {AIRegistryTextPc007563e4, Action_ImmuneToKillTerrain, 0, 0, 0},
    {AIRegistryTextPc007563d4, Action_ImmuneToBolts, 0, 0, 0},
    {AIRegistryTextPc007563c8, Action_Respawnable, 0, 0, 0},
    {AIRegistryTextPc007563b8, Action_SetPathCnxFlag, 1, 0, 0},
    {AIRegistryTextPc007563b0, Action_SetHint, 0, 0, 0},
    {AIRegistryTextPc007563a0, Action_SetHintComplete, 0, 0, 0},
    {AIRegistryTextPc00756394, Action_CancelHint, 0, 0, 0},
    {AIRegistryTextPc00756384, Action_CycleCharacter, 0, 0, 0},
    {AIRegistryTextPc00756374, Action_CnxController, 0, 0, 0},
    {AIRegistryTextPc00756368, Action_CnxHelper, 0, 0, 0},
    {AIRegistryTextPc00756360, Action_PlaySfx, 0, 0, 0},
    {AIRegistryTextPc00756354, Action_CameraCut, 1, 0, 0},
    {AIRegistryTextPc00756340, Action_DynamicCameraCut, 1, 0, 0},
    {AIRegistryTextPc00756330, Action_EndCameraCut, 1, 0, 0},
    {AIRegistryTextPc00756320, Action_DontRaycastLOS, 0, 0, 0},
    {AIRegistryTextPc00756310, Action_SetForceBack, 1, 0, 0},
    {AIRegistryTextPc00756304, Action_FaceCamera, 0, 0, 0},
    {AIRegistryTextPc007562f4, Action_FaceCharacter, 0, 0, 0},
    {AIRegistryTextPc007562e8, Action_SpinOnSpot, 0, 0, 0},
    {AIRegistryTextPc007562d8, Action_FollowCharacter, 0, 0, 0},
    {AIRegistryTextPc007562c8, Action_GameFollowPlayer, 0, 0, 0},
    {AIRegistryTextPc007562bc, Action_MoveForward, 0, 0, 0},
    {AIRegistryTextPc007562a4, Action_SetFormationCommander, 0, 0, 0},
    {AIRegistryTextPc00756288, Action_RemoveThrownForceObjects, 0, 0, 0},
    {AIRegistryTextPc00756270, Action_AlwaysTriggerObstacle, 0, 0, 0},
    {AIRegistryTextPc0075625c, Action_CanTriggerObstacle, 0, 0, 0},
    {AIRegistryTextPc0075624c, Action_PlayGizObstacle, 1, 0, 0},
    {AIRegistryTextPc0075623c, Action_PlayGizObstacle, 1, 0, 0},
    {AIRegistryTextPc0075622c, Action_PlayGizSpecial, 1, 0, 0},
    {AIRegistryTextPc00756218, Action_SetObstacleToEnd, 1, 0, 0},
    {AIRegistryTextPc00756e7c, Action_HelpWithTriggers, 0, 0, 0},
    {AIRegistryTextPc00756208, Action_UseTriggerSet, 0, 0, 0},
    {AIRegistryTextPc007561fc, Action_PullLever, 0, 0, 0},
    {AIRegistryTextPc007561f0, Action_UsePanel, 0, 0, 0},
    {AIRegistryTextPc007561e4, Action_UseTechno, 0, 0, 0},
    {AIRegistryTextPc007561d4, Action_ReleaseLocator, 0, 0, 0},
    {AIRegistryTextPc007561c4, Action_AssignLocator, 0, 0, 0},
    {AIRegistryTextPc007561b0, Action_GetLocatorFromSet, 0, 0, 0},
    {AIRegistryTextPc00756194, Action_MoveAwayFromLastAttacker, 0, 0, 0},
    {AIRegistryTextPc00756188, Action_ProbeDroid, 0, 0, 0},
    {AIRegistryTextPc00756178, Action_AlertCreatures, 0, 0, 0},
    {AIRegistryTextPc00756168, Action_SetLastAttacker, 0, 0, 0},
    {AIRegistryTextPc00756150, Action_LinkTurretToController, 0, 0, 0},
    {AIRegistryTextPc00756144, Action_TakeOver, 1, 0, 0},
    {AIRegistryTextPc00756134, Action_ReleaseTakeOver, 1, 0, 0},
    {AIRegistryTextPc0075611c, Action_RegisterTakeOverObject, 0, 0, 0},
    {AIRegistryTextPc00756108, Action_SetTakeOverTarget, 0, 0, 0},
    {AIRegistryTextPc007560f4, Action_ClearTakeOverTarget, 0, 0, 0},
    {AIRegistryTextPc007560e4, Action_AddGameMsgCount, 1, 0, 0},
    {AIRegistryTextPc007560d4, Action_AddMiscPickups, 0, 0, 0},
    {AIRegistryTextPc007560c4, Action_SetCanTakeOver, 0, 0, 0},
    {AIRegistryTextPc007560b4, Action_CanBeCarried, 1, 0, 0},
    {AIRegistryTextPc0075609c, Action_IgnoreLastSafePathPos, 0, 0, 0},
    {AIRegistryTextPc00756084, Action_AwkwardShapeOverride, 0, 0, 0},
    {AIRegistryTextPc00756070, Action_IgnoreSlideTerrain, 0, 0, 0},
    {AIRegistryTextPc0075605c, Action_SplineFollowTerrain, 0, 0, 0},
    {AIRegistryTextPc00756050, Action_SetLayer, 0, 0, 0},
    {AIRegistryTextPc00756044, Action_CreateRider, 0, 0, 0},
    {AIRegistryTextPc00756030, Action_AddTorpedoPacket, 1, 0, 0},
    {AIRegistryTextPc0075601c, Action_SpeederBeingChased, 0, 0, 0},
    {AIRegistryTextPc0075600c, Action_ThrowDetonator, 0, 0, 0},
    {AIRegistryTextPc00755ff8, Action_SetScaleOverride, 0, 0, 0},
    {AIRegistryTextPc00755fe4, Action_DisableNarrowSocks, 1, 0, 0},
    {AIRegistryTextPc00755fd0, Action_UseTimeBasedUpdate, 0, 0, 0},
    {AIRegistryTextPc00755fc0, Action_ForceLightning, 0, 0, 0},
    {AIRegistryTextPc00755fb0, Action_WalkBackwards, 0, 0, 0},
    {AIRegistryTextPc00755f9c, Action_AddScriptProcessor, 0, 0, 0},
    {AIRegistryTextPc00755f8c, Action_SetUseOneAtOnce, 0, 0, 0},
    {AIRegistryTextPc00755f78, Action_SetAO_MaxAttackers, 0, 0, 0},
    {AIRegistryTextPc00755f60, Action_SetAO_AttackersPerRow, 0, 0, 0},
    {AIRegistryTextPc00755f50, Action_SetAO_RowDist, 0, 0, 0},
    {AIRegistryTextPc00755f3c, Action_SetAO_InitRowDist, 0, 0, 0},
    {AIRegistryTextPc00755f28, Action_SetTechnoComplete, 0, 0, 0},
    {AIRegistryTextPc00755f18, Action_LetGoOfBalloon, 0, 0, 0},
    {AIRegistryTextPc00755f04, Action_DrawBossHitPoints, 1, 0, 0},
    {AIRegistryTextPc00755ef4, Action_CompleteLevel, 1, 0, 0},
    {AIRegistryTextPc00755ee4, Action_GoToNewLevel, 0, 0, 0},
    {AIRegistryTextPc00755ed4, Action_CircleLocator, 0, 0, 0},
    {AIRegistryTextPc00755ec4, Action_GizmoActivate, 0, 0, 0},
    {AIRegistryTextPc00755eb0, Action_GizmoSetVisibility, 0, 0, 0},
    {AIRegistryTextPc00755ea0, Action_TurnOnPickup, 0, 0, 0},
    {AIRegistryTextPc00755e8c, Action_CanHelpWithTriggers, 0, 0, 0},
    {AIRegistryTextPc00755e74, Action_CanCollideWithObjects, 0, 0, 0},
    {AIRegistryTextPc00755e60, Action_SetShootOpponents, 0, 0, 0},
    {AIRegistryTextPc00755e48, Action_PartyCanBeUnderCover, 0, 0, 0},
    {AIRegistryTextPc00755e3c, Action_SetLapTime, 0, 0, 0},
    {AIRegistryTextPc00755e30, Action_CreatePod, 0, 0, 0},
    {AIRegistryTextPc00755e1c, Action_MushroomCollapse, 0, 0, 0},
    {AIRegistryTextPc00755e0c, Action_BoulderSection, 0, 0, 0},
    {AIRegistryTextPc00755dfc, Action_RaceOpponent, 0, 0, 0},
    {AIRegistryTextPc00755df4, Action_Sebulba, 0, 0, 0},
    {AIRegistryTextPc00755ddc, Action_IgnoreTurnAroundSpline, 0, 0, 0},
    {AIRegistryTextPc00755dc4, Action_CanMoveWhenDeactivated, 0, 0, 0},
    {AIRegistryTextPc00755db8, Action_DontAttack, 0, 0, 0},
    {AIRegistryTextPc00755da8, Action_CanPullLevers, 0, 0, 0},
    {AIRegistryTextPc00755d9c, Action_NewSebulba, 0, 0, 0},
    {NULL, NULL, 0, 0, 0},
};
#pragma data_seg()

#pragma data_seg(".data$lego_aiconditiondefs")
AICONDITIONDEF_PC lego_aiconditiondefs[171] = {
    {AIRegistryTextPc007575cc, Condition_GlynTest, NULL},
    {AIRegistryTextPc007575c4, Condition_Debug, NULL},
    {AIRegistryTextPc007575bc, Condition_Active, NULL},
    {AIRegistryTextPc007575b4, Condition_GotGun, NULL},
    {AIRegistryTextPc007575a4, Condition_PrefersBrawling, NULL},
    {AIRegistryTextPc0075759c, Condition_IsAlive, Condition_IsAliveInit},
    {AIRegistryTextPc00757590, Condition_IsOnScreen, Condition_IsOnScreenInit},
    {AIRegistryTextPc00757580, Condition_OffScreenTimer, Condition_OffScreenTimerInit},
    {AIRegistryTextPc00757574, Condition_OnObject, Condition_OnObjectInit},
    {AIRegistryTextPc0075755c, Condition_OnSameObjectAsPlayer, NULL},
    {AIRegistryTextPc0075754c, Condition_PlayerOnObject, Condition_OnObjectInit},
    {AIRegistryTextPc00757534, Condition_EitherPlayerOnObject, Condition_OnObjectInit},
    {AIRegistryTextPc00757518, Condition_EitherPlayerLocatorRangeXZ, Condition_EitherPlayerLocatorRangeXZInitPc},
    {AIRegistryTextPc0075750c, Condition_OnGround, NULL},
    {AIRegistryTextPc00757500, Condition_BeenAlerted, NULL},
    {AIRegistryTextPc007574f0, Condition_PlayerOnGround, NULL},
    {AIRegistryTextPc007574e4, Condition_SpawnCount, NULL},
    {AIRegistryTextPc007574d4, Condition_BehindCamera, NULL},
    {AIRegistryTextPc007574c4, Condition_LocatorOnScreen, Condition_LocatorOnScreenInit},
    {AIRegistryTextPc007574b8, Condition_Blocking, NULL},
    {AIRegistryTextPc007574b0, Condition_BeenHit, Condition_BeenHitInit},
    {AIRegistryTextPc007574a4, Condition_HoverPhase, NULL},
    {AIRegistryTextPc00757498, Condition_HitPoints, Condition_HitPointsInit},
    {AIRegistryTextPc00757484, Condition_OnDynamicGrapple, Condition_OnDynamicGrappleInit},
    {AIRegistryTextPc0075747c, Condition_XPos, Condition_XPosInitPc},
    {AIRegistryTextPc00757474, Condition_YPos, Condition_XPosInitPc},
    {AIRegistryTextPc0075746c, Condition_ZPos, Condition_XPosInitPc},
    {AIRegistryTextPc00757454, Condition_CollidingWithOpponent, NULL},
    {AIRegistryTextPc00757448, Condition_Colliding, NULL},
    {AIRegistryTextPc00757438, Condition_ObstacleAtStart, Condition_ObstacleAtEndInit},
    {AIRegistryTextPc00757428, Condition_ObstacleAtEnd, Condition_ObstacleAtEndInit},
    {AIRegistryTextPc00757418, Condition_ObstacleAtStart, Condition_SpecialAtStartInitPc},
    {AIRegistryTextPc00757408, Condition_ObstacleAtEnd, Condition_SpecialAtStartInitPc},
    {AIRegistryTextPc007573f4, Condition_ObstacleLockedOpen, Condition_ObstacleOpenedByPlayerInit},
    {AIRegistryTextPc007573e0, Condition_ObstacleLockedShut, Condition_ObstacleOpenedByPlayerInit},
    {AIRegistryTextPc007573d0, Condition_ForceAtStart, Condition_ForceInit},
    {AIRegistryTextPc007573c4, Condition_ForceAtEnd, Condition_ForceInit},
    {AIRegistryTextPc007573ac, Condition_ObstacleOpenedByPlayer, Condition_ObstacleOpenedByPlayerInit},
    {AIRegistryTextPc0075738c, Condition_ObstacleOpenedByEitherPlayer, Condition_ObstacleOpenedByPlayerInit},
    {AIRegistryTextPc00757378, Condition_AnimationFinished, Condition_AnimationFinishedInit},
    {AIRegistryTextPc0075735c, Condition_EitherPlayerPullingLever, NULL},
    {AIRegistryTextPc00757340, Condition_EitherPlayerUsingHatMachinePc, NULL},
    {AIRegistryTextPc00757328, Condition_EitherPlayerUsingPanel, NULL},
    {AIRegistryTextPc0075730c, Condition_EitherPlayerWearingHelmet, NULL},
    {AIRegistryTextPc007572fc, Condition_PartyUnderCover, NULL},
    {AIRegistryTextPc007572e0, Condition_NumBaddiesThatCanSeePlayers, NULL},
    {AIRegistryTextPc007572cc, Condition_PlayerUsingForce, Condition_UsingForceInit},
    {AIRegistryTextPc007572b4, Condition_EitherPlayerUsingForce, Condition_UsingForceInit},
    {AIRegistryTextPc007572a8, Condition_UsingForce, Condition_UsingForceInit},
    {AIRegistryTextPc00757298, Condition_OnForcePlatform, Condition_OnForcePlatformInit},
    {AIRegistryTextPc00757280, Condition_PlayerOnForcePlatform, Condition_OnForcePlatformInit},
    {AIRegistryTextPc00757264, Condition_EitherPlayerOnForcePlatform, Condition_OnForcePlatformInit},
    {AIRegistryTextPc00757254, Condition_ForceBeingUsed, Condition_UsingForceInit},
    {AIRegistryTextPc00757244, Condition_ForcePushing, Condition_ForcePushingInit},
    {AIRegistryTextPc00757238, Condition_TurretAlive, Condition_TurretAliveInit},
    {AIRegistryTextPc00757220, Condition_PlayerDeflectingPart, NULL},
    {AIRegistryTextPc00757210, Condition_ForceComplete, Condition_ForceCompleteInit},
    {AIRegistryTextPc00757200, Condition_ForceFinished, Condition_ForceCompleteInit},
    {AIRegistryTextPc007571f0, Condition_BuildItComplete, Condition_BuildItCompleteInit},
    {AIRegistryTextPc007571e0, Condition_BlowupBlownup, Condition_BlowupBlownupInitPc},
    {AIRegistryTextPc007571d8, Condition_IAmA, Condition_IAmAInit},
    {AIRegistryTextPc007571cc, Condition_OpponentIsA, Condition_OpponentIsAInit},
    {AIRegistryTextPc007571b8, Condition_GameOpponentIsAThreatPc, NULL},
    {AIRegistryTextPc007571a4, Condition_CanFightLikeAJedi, NULL},
    {AIRegistryTextPc00757198, Condition_IAmAGoody, NULL},
    {AIRegistryTextPc0075718c, Condition_IAmABaddy, NULL},
    {AIRegistryTextPc00757180, Condition_IAmANeutral, NULL},
    {AIRegistryTextPc00757170, Condition_IAmAGoodieBaddie, NULL},
    {AIRegistryTextPc0075715c, Condition_IAmAPartyCharacter, NULL},
    {AIRegistryTextPc00757150, Condition_CategoryIs, Condition_CategoryIsInit},
    {AIRegistryTextPc0075713c, Condition_PlayerCategoryIs, Condition_CategoryIsInit},
    {AIRegistryTextPc0075712c, Condition_EitherPlayerIs, Condition_EitherPlayerIsInit},
    {AIRegistryTextPc00757120, Condition_Player1Is, Condition_EitherPlayerIsInit},
    {AIRegistryTextPc00757114, Condition_Player2Is, Condition_EitherPlayerIsInit},
    {AIRegistryTextPc00757108, Condition_IsSetAlive, Condition_IsSetAliveInit},
    {AIRegistryTextPc007570f8, Condition_NumInSetAlive, Condition_IsSetAliveInit},
    {AIRegistryTextPc007570f0, Condition_Context, NULL},
    {AIRegistryTextPc007570e4, Condition_InContext, Condition_InContextInit},
    {AIRegistryTextPc007570d4, Condition_OpponentContext, Condition_InContextInit},
    {AIRegistryTextPc007570c4, Condition_Player2Active, NULL},
    {AIRegistryTextPc007570b8, Condition_NumBaddies, Condition_NumBaddiesInit},
    {AIRegistryTextPc007570a8, Condition_NumForceObjects, Condition_NumForceObjectsInit},
    {AIRegistryTextPc0075709c, Condition_BeenToLevel, Condition_BeenToLevelInit},
    {AIRegistryTextPc00757090, Condition_LastLevel, Condition_LastLevelInit},
    {AIRegistryTextPc00757088, Condition_Message, Condition_MessageInit},
    {AIRegistryTextPc0075707c, Condition_ScriptParam, Condition_ScriptParamInit},
    {AIRegistryTextPc0075706c, Condition_CutSceneStarted, Condition_CutSceneStartedInit},
    {AIRegistryTextPc00757058, Condition_CutSceneFinished, Condition_CutSceneFinishedInit},
    {AIRegistryTextPc00757048, Condition_CutSceneExists, Condition_CutSceneExistsInit},
    {AIRegistryTextPc00757038, Condition_PlayerInSock, Condition_PlayerInSockInit},
    {AIRegistryTextPc00757028, Condition_CutScenePlaying, Condition_CutScenePlayingInit},
    {AIRegistryTextPc00757018, Condition_RigidAnimFrame, Condition_RigidAnimFrameInit},
    {AIRegistryTextPc00757000, Condition_SockDistanceToPlayer, NULL},
    {AIRegistryTextPc00756fe8, Condition_SockDistanceToOpponent, NULL},
    {AIRegistryTextPc00756fd0, Condition_SockXDistanceToPlayer, NULL},
    {AIRegistryTextPc00756fb8, Condition_PlayerDistanceAlongSock, NULL},
    {AIRegistryTextPc00756f98, Condition_FurthestPlayerDistanceAlongSock, NULL},
    {AIRegistryTextPc00756f88, Condition_FinishedSpline, NULL},
    {AIRegistryTextPc00756f78, Condition_CurrentHintId, NULL},
    {AIRegistryTextPc00756f68, Condition_HintAvailable, Condition_HintAvailableInit},
    {AIRegistryTextPc00756f58, Condition_HintComplete, Condition_HintAvailableInit},
    {AIRegistryTextPc00755150, Condition_Freeplay, NULL},
    {AIRegistryTextPc00756f50, Condition_IndyPc, NULL},
    {AIRegistryTextPc00756f44, Condition_MissionMode, NULL},
    {AIRegistryTextPc00756f38, Condition_MissionWon, NULL},
    {AIRegistryTextPc00756f28, Condition_ChallengeMode, NULL},
    {AIRegistryTextPc00756f24, Condition_PSP, NULL},
    {AIRegistryTextPc00756f10, Condition_AIOverrideControl, Condition_AIOverrideControlInit},
    {AIRegistryTextPc00756ef4, Condition_BoltsDontGetDeflectedBack, NULL},
    {AIRegistryTextPc00756ee4, Condition_CheatProgress, NULL},
    {AIRegistryTextPc00756ed4, Condition_BigJumpComplete, NULL},
    {AIRegistryTextPc00756ec0, Condition_RespawnLocatorIs, Condition_RespawnLocatorIsInit},
    {AIRegistryTextPc00756eb4, Condition_InMiniCut, NULL},
    {AIRegistryTextPc00756ea0, Condition_MaulShouldRunAway, NULL},
    {AIRegistryTextPc00756e90, Condition_DropBackInTimer, NULL},
    {AIRegistryTextPc00756e7c, Condition_HelpWithTriggers, NULL},
    {AIRegistryTextPc00756e60, Condition_EitherPlayerPushingSpinner, Condition_EitherPlayerPushingSpinnerInit},
    {AIRegistryTextPc00756e50, Condition_CharacterRange, Condition_CharacterRangeInit},
    {AIRegistryTextPc00756e44, Condition_BeenSpawned, NULL},
    {AIRegistryTextPc00756e30, Condition_LastAttackerRange, NULL},
    {AIRegistryTextPc00756e14, Condition_LastAttackerIsActivePlayer, NULL},
    {AIRegistryTextPc00756e00, Condition_PartyContainsDroids, NULL},
    {AIRegistryTextPc00756de8, Condition_CannotReachDestination, NULL},
    {AIRegistryTextPc00756ddc, Condition_TakenOver, Condition_TakenOverInit},
    {AIRegistryTextPc00756dcc, Condition_PlayerTakenOver, NULL},
    {AIRegistryTextPc00756db4, Condition_EitherPlayerTakenOver, NULL},
    {AIRegistryTextPc00756da4, Condition_BeenTakenOver, Condition_BeenTakenOverInit},
    {AIRegistryTextPc00756d94, Condition_OnSpeederBike, Condition_OnSpeederBikeInit},
    {AIRegistryTextPc00756d80, Condition_UnderPlayerControl, Condition_UnderPlayerControlInit},
    {AIRegistryTextPc00756d70, Condition_CharacterExists, Condition_CharacterExistsInit},
    {AIRegistryTextPc00756d5c, Condition_CharacterTypeExists, Condition_CharacterTypeExistsInit},
    {AIRegistryTextPc00756d4c, Condition_GotLocatorInSet, Condition_GotLocatorInSetInit},
    {AIRegistryTextPc00756d3c, Condition_GotOpponentLOS, NULL},
    {AIRegistryTextPc00756d2c, Condition_EmptyTakeOver, NULL},
    {AIRegistryTextPc00756d18, Condition_HasTakeOver, NULL},
    {AIRegistryTextPc00756d08, Condition_TakeOverRange, NULL},
    {AIRegistryTextPc00756cec, Condition_TakeOverTargetInTriggerArea, Condition_TakeOverTargetInTriggerAreaInit},
    {AIRegistryTextPc00756cd0, Condition_EitherPlayerInMyTriggerArea, NULL},
    {AIRegistryTextPc00756cc4, Condition_GotVictim, NULL},
    {AIRegistryTextPc00756cb8, Condition_IsVisible, Condition_IsVisibleInit},
    {AIRegistryTextPc00756cb0, Condition_MySet, NULL},
    {AIRegistryTextPc00756ca4, Condition_ScreenWipe, NULL},
    {AIRegistryTextPc00756c98, Condition_IAmPlayer2, NULL},
    {AIRegistryTextPc00756c84, Condition_HeadTurnRestricted, NULL},
    {AIRegistryTextPc00756c78, Condition_ShopActive, NULL},
    {AIRegistryTextPc00756c70, Condition_Side, Condition_SideInit},
    {AIRegistryTextPc00756c5c, Condition_NearestPartyRange, NULL},
    {AIRegistryTextPc00756c48, Condition_NearestPartyXZRange, NULL},
    {AIRegistryTextPc00756c30, Condition_OpponentToPlayerRange, NULL},
    {AIRegistryTextPc00756c18, Condition_OpponentPathPosRange, NULL},
    {AIRegistryTextPc00756c08, Condition_GizmoOutput0, Condition_GizmoOutput0InitPc},
    {AIRegistryTextPc00756bf8, Condition_GizmoOutput1, Condition_GizmoOutput0InitPc},
    {AIRegistryTextPc00756be8, Condition_GizmoOutput2, Condition_GizmoOutput0InitPc},
    {AIRegistryTextPc00756bd8, Condition_GizmoOutput3, Condition_GizmoOutput0InitPc},
    {AIRegistryTextPc00756bc8, Condition_GizmoVisibility, Condition_GizmoVisibilityInit},
    {AIRegistryTextPc00756bac, Condition_AngleAboutMyLocatorToPlayer, Condition_AngleAboutMyLocatorToPlayerInit},
    {AIRegistryTextPc00756b9c, Condition_AnimSpeedMul, NULL},
    {AIRegistryTextPc00756b88, Condition_PickupBeenTurnedOn, Condition_PickupBeenTurnedOnInit},
    {AIRegistryTextPc00756b78, Condition_FlowBoxComplete, Condition_FlowBoxCompleteInit},
    {AIRegistryTextPc00756b68, Condition_CanHearRadio, NULL},
    {AIRegistryTextPc00751930, Condition_BeingTowed, NULL},
    {AIRegistryTextPc00756b60, Condition_RaceLapPc, NULL},
    {AIRegistryTextPc00756b58, Condition_MusicOn, NULL},
    {AIRegistryTextPc00756b48, Condition_CharacterLoaded, Condition_CharacterLoadedInit},
    {AIRegistryTextPc00756b38, Condition_AreaComplete, Condition_AreaCompleteInit},
    {AIRegistryTextPc00756b20, Condition_ShouldAttackOpponent, NULL},
    {AIRegistryTextPc00756b18, Condition_InSwamp, NULL},
    {AIRegistryTextPc00756af4, Condition_InSameTriggerAreaAsNearestPlayer, NULL},
    {AIRegistryTextPc00756ae0, Condition_NetworkGameOnGoing, NULL},
    {AIRegistryTextPc00756ad4, Condition_InHubArea, Condition_InHubAreaInit},
    {NULL, NULL, NULL},
};
#pragma data_seg()

#pragma data_seg(".data$api_aiactiondefs")
AIACTIONDEF_PC api_aiactiondefs[55] = {
    {AIRegistryTextPc0075fbfc, Action_Idle, 0, 0, 0},
    {AIRegistryTextPc0075fbf0, Action_SetState, 0, 0, 0},
    {AIRegistryTextPc0075fbe4, Action_ResetTimer, 0, 0, 0},
    {AIRegistryTextPc0075fbc8, Action_RetreatFromNearestOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fbb4, Action_RetreatFromOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fba0, Action_MoveAwayFromPlayer, 0, 0, 0},
    {AIRegistryTextPc0075fb8c, Action_MoveAwayFromPlayer2, 0, 0, 0},
    {AIRegistryTextPc0075fb78, Action_SetCircleDirection, 0, 0, 0},
    {AIRegistryTextPc0075fb70, Action_Circle, 0, 0, 0},
    {AIRegistryTextPc0075fb60, Action_CircleOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fb50, Action_CirclePlayer, 0, 0, 0},
    {AIRegistryTextPc007562c8, Action_FollowPlayer, 0, 0, 0},
    {AIRegistryTextPc0075fb38, Action_MoveAwayFromOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fb28, Action_FollowOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fb1c, Action_FacePlayer, 0, 0, 0},
    {AIRegistryTextPc0075fb0c, Action_FaceOpponent, 0, 0, 0},
    {AIRegistryTextPc0075fb00, Action_FaceLocator, 0, 0, 0},
    {AIRegistryTextPc0075faec, Action_IgnoreWallSplines, 0, 0, 0},
    {AIRegistryTextPc0075fad8, Action_CheckWallSplines, 0, 0, 0},
    {AIRegistryTextPc0075facc, Action_NoTerrain, 0, 0, 0},
    {AIRegistryTextPc0075fac0, Action_FlatTerrain, 0, 0, 0},
    {AIRegistryTextPc0075fab0, Action_ShadowTerrain, 0, 0, 0},
    {AIRegistryTextPc0075fa98, Action_DontUseShadowTerrain, 0, 0, 0},
    {AIRegistryTextPc007566a8, Action_DontPush, 0, 0, 0},
    {AIRegistryTextPc0075fa88, Action_CanSeeBehind, 0, 0, 0},
    {AIRegistryTextPc0075fa7c, Action_RequiresLOS, 0, 0, 0},
    {AIRegistryTextPc0075fa68, Action_SetFullPathSearch, 0, 0, 0},
    {AIRegistryTextPc0075fa58, Action_SetViewDistance, 0, 0, 0},
    {AIRegistryTextPc0075fa44, Action_SetMaxViewHeight, 0, 0, 0},
    {AIRegistryTextPc0075fa30, Action_SetMinViewHeight, 0, 0, 0},
    {AIRegistryTextPc0075fa20, Action_SetHearDistance, 0, 0, 0},
    {AIRegistryTextPc0075fa10, Action_SetMoveRadius, 0, 0, 0},
    {AIRegistryTextPc0075fa04, Action_GoToNode, 0, 0, 0},
    {AIRegistryTextPc0075f9f4, Action_GoToNodeRandom, 0, 0, 0},
    {AIRegistryTextPc0075f9e8, Action_GoToOrigin, 0, 0, 0},
    {AIRegistryTextPc0075f9dc, Action_GoToLocator, 0, 0, 0},
    {AIRegistryTextPc00756a74, Action_SetLocator, 0, 0, 0},
    {AIRegistryTextPc0075f9c8, Action_SetRespawnLocator, 0, 0, 0},
    {AIRegistryTextPc0075f9bc, Action_FollowPath, 0, 0, 0},
    {AIRegistryTextPc0075f9a8, Action_MoveAwayFromNode, 0, 0, 0},
    {AIRegistryTextPc0075f994, Action_OverrideAnimation, 1, 0, 0},
    {AIRegistryTextPc0075f988, Action_BlockPath, 0, 0, 0},
    {AIRegistryTextPc0075f970, Action_PathConnectionObstacle, 0, 0, 0},
    {AIRegistryTextPc0075f958, Action_PathConnectionMaxLength, 0, 0, 0},
    {AIRegistryTextPc0075f94c, Action_NoLosCheck, 0, 0, 0},
    {AIRegistryTextPc0075f93c, Action_ResetToOrigin, 0, 0, 0},
    {AIRegistryTextPc0075f92c, Action_SetInterrupt, 0, 0, 0},
    {AIRegistryTextPc0075f91c, Action_ClearInterrupt, 0, 0, 0},
    {AIRegistryTextPc0075f908, Action_SetIgnoreAntinodes, 0, 0, 0},
    {AIRegistryTextPc0075f8fc, Action_NoShadows, 0, 0, 0},
    {AIRegistryTextPc0075f8f0, Action_SetParam, 0, 0, 0},
    {AIRegistryTextPc0075f8dc, Action_SetReturnToState, 0, 0, 0},
    {AIRegistryTextPc0075f8cc, Action_ReturnToState, 0, 0, 0},
    {AIRegistryTextPc0075f8b8, Action_NotifyStateChange, 0, 0, 0},
    {NULL, NULL, 0, 0, 0},
};
#pragma data_seg()

#pragma data_seg(".data$api_aiconditiondefs")
AICONDITIONDEF_PC api_aiconditiondefs[50] = {
    {AIRegistryTextPc0075ff04, NULL, NULL},
    {AIRegistryTextPc0075fef8, Condition_AlwaysTrue, Condition_AlwaysTrueInit},
    {AIRegistryTextPc0075fee8, Condition_LocatorRange, Condition_LocatorRangeInit},
    {AIRegistryTextPc0075fed8, Condition_LocatorRangeXZ, Condition_LocatorRangeInit},
    {AIRegistryTextPc0075fec8, Condition_LocatorRangeY, Condition_LocatorRangeInit},
    {AIRegistryTextPc0075fec0, Condition_Timer, NULL},
    {AIRegistryTextPc0075feb8, Condition_Random, NULL},
    {AIRegistryTextPc0075feac, Condition_GotLocator, NULL},
    {AIRegistryTextPc0075fe9c, Condition_GotLocatorSet, NULL},
    {AIRegistryTextPc0075fe88, Condition_CurrentLocatorIs, Condition_CurrentLocatorIsInit},
    {AIRegistryTextPc0075fe78, Condition_InTriggerArea, Condition_InTriggerAreaInit},
    {AIRegistryTextPc0075fe6c, Condition_PlayerRange, NULL},
    {AIRegistryTextPc0075fe58, Condition_NearestPlayerRange, NULL},
    {AIRegistryTextPc0075fe40, Condition_NearestPlayerXZRange, NULL},
    {AIRegistryTextPc0075fe34, Condition_InLevelNode, Condition_InLevelNodeInit},
    {AIRegistryTextPc0075fe20, Condition_PlayerInLevelNode, Condition_PlayerInLevelNodeInit},
    {AIRegistryTextPc0075fe08, Condition_EitherPlayerInLevelNode, Condition_PlayerInLevelNodeInit},
    {AIRegistryTextPc0075fdfc, Condition_NodeRangePc, Condition_NodeRangeInitPc},
    {AIRegistryTextPc0075fdec, Condition_GotTriggerArea, NULL},
    {AIRegistryTextPc0075fdd8, Condition_PlayerInTriggerArea, Condition_PlayerInTriggerAreaInit},
    {AIRegistryTextPc0075fdc0, Condition_Player2InTriggerArea, Condition_PlayerInTriggerAreaInit},
    {AIRegistryTextPc0075fda4, Condition_EitherPlayerInTriggerArea, Condition_PlayerInTriggerAreaInit},
    {AIRegistryTextPc0075fd90, Condition_BaddyInTriggerArea, Condition_BaddyInTriggerAreaInit},
    {AIRegistryTextPc0075fd7c, Condition_GoodyInTriggerArea, Condition_GoodyInTriggerAreaInit},
    {AIRegistryTextPc0075fd64, Condition_OpponentInTriggerArea, Condition_OpponentInTriggerAreaInit},
    {AIRegistryTextPc0075fd58, Condition_GotOpponent, NULL},
    {AIRegistryTextPc007571b8, Condition_OpponentIsAThreat, NULL},
    {AIRegistryTextPc0075fd44, Condition_OpponentOnSamePath, NULL},
    {AIRegistryTextPc0075fd34, Condition_OpponentRange, NULL},
    {AIRegistryTextPc0075fd1c, Condition_NearestOpponentRange, NULL},
    {AIRegistryTextPc0075fd0c, Condition_YawToOpponent, NULL},
    {AIRegistryTextPc0075fcfc, Condition_OpponentBelow, NULL},
    {AIRegistryTextPc0075fcf0, Condition_OriginRange, NULL},
    {AIRegistryTextPc0075fcdc, Condition_OpponentToOrigin, NULL},
    {AIRegistryTextPc0075fccc, Condition_PlayerToOrigin, NULL},
    {AIRegistryTextPc0075fcb8, Condition_OpponentToLocator, Condition_OpponentToLocatorInit},
    {AIRegistryTextPc0075fca4, Condition_OpponentToLocatorXZ, Condition_OpponentToLocatorInit},
    {AIRegistryTextPc0075fc90, Condition_OpponentToLocatorY, Condition_OpponentToLocatorInit},
    {AIRegistryTextPc0075fc80, Condition_PlayerToLocator, Condition_PlayerToLocatorInit},
    {AIRegistryTextPc0075fc68, Condition_NearestPlayerToLocator, Condition_PlayerToLocatorInit},
    {AIRegistryTextPc0075fc60, Condition_OnPath, NULL},
    {AIRegistryTextPc0075fc50, Condition_PlayerOnPath, NULL},
    {AIRegistryTextPc0075fc40, Condition_OpponentOnPath, NULL},
    {AIRegistryTextPc0075fc34, Condition_TimeOffPath, NULL},
    {AIRegistryTextPc0075fc28, Condition_PathBlocked, NULL},
    {AIRegistryTextPc0075fc1c, Condition_InterruptID, NULL},
    {AIRegistryTextPc0075fc18, Condition_IAm, Condition_IAmInit},
    {AIRegistryTextPc0075fc0c, Condition_StuckTime, NULL},
    {AIRegistryTextPc0075fc04, Condition_Param, NULL},
    {NULL, NULL, NULL},
};
#pragma data_seg()

#pragma bss_seg(".bss$game_aiactiondefs")
AIACTIONDEF_PC *game_aiactiondefs = 0;
#pragma bss_seg(".bss$game_aiconditiondefs")
AICONDITIONDEF_PC *game_aiconditiondefs = 0;
#pragma bss_seg()
