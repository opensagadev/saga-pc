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

#pragma const_seg(".rdata$AIRegistryTextPc007562c8")
const char AIRegistryTextPc007562c8[13] = "FollowPlayer";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007566a8")
const char AIRegistryTextPc007566a8[9] = "DontPush";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756a74")
const char AIRegistryTextPc00756a74[11] = "SetLocator";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc00756e7c")
const char AIRegistryTextPc00756e7c[17] = "HelpWithTriggers";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007571b8")
const char AIRegistryTextPc007571b8[18] = "OpponentIsAThreat";
#pragma const_seg()
#pragma const_seg(".rdata$AIRegistryTextPc007575bc")
const char AIRegistryTextPc007575bc[7] = "Active";
#pragma const_seg()

#pragma data_seg(".data$lego_aiactiondefs")
AIACTIONDEF_PC lego_aiactiondefs[208] = {
    {"Activate", Action_Activate, 1, 0, 0},
    {"DeActivate", Action_DeActivate, 1, 0, 0},
    {"GoToLevelPath", Action_GoToLevelPath, 0, 0, 0},
    {"SetPath", Action_SetPath, 0, 0, 0},
    {"GoToOriginalPath", Action_GoToOriginalPath, 0, 0, 0},
    {"SnapToLocator", Action_SnapToLocator, 1, 0, 0},
    {AIRegistryTextPc00756a74, Action_GameSetLocator, 1, 0, 0},
    {"SetLocatorSet", Action_SetLocatorSet, 0, 0, 0},
    {"SnapToOrigin", Action_SnapToOrigin, 1, 0, 0},
    {"BigJumpToLocator", Action_BigJumpToLocator, 0, 0, 0},
    {"BigJump", Action_BigJump, 0, 0, 0},
    {"SetDoomedEscapeLocator", Action_SetDoomedEscapeLocator, 0, 0, 0},
    {"SnapToPosition", Action_SnapToPosition, 1, 0, 0},
    {"SnapToSockPosition", Action_SnapToSockPosition, 1, 0, 0},
    {"SetAnimation", Action_SetAnimation, 0, 0, 0},
    {"AnimTimeRandom", Action_AnimTimeRandom, 0, 0, 0},
    {"CanOpenDoors", Action_CanOpenDoors, 0, 0, 0},
    {"CanShootOffScreen", Action_CanShootOffScreen, 0, 0, 0},
    {"KeepWeaponOut", Action_KeepWeaponOut, 0, 0, 0},
    {"SnapWeaponOut", Action_SnapWeaponOut, 1, 0, 0},
    {"ResetContext", Action_ResetContext, 0, 0, 0},
    {"PrefersPlayers", Action_PrefersPlayers, 0, 0, 0},
    {"SetBoltsDontGetDeflectedBack", Action_SetBoltsDontGetDeflectedBack, 0, 0, 0},
    {"CanShootObstructions", Action_CanShootObstructions, 0, 0, 0},
    {"UseBigJumpToJump", Action_UseBigJumpToJump, 0, 0, 0},
    {"SetTaggable", Action_SetTaggable, 1, 0, 0},
    {"CatchUpForbidden", Action_CatchUpForbidden, 0, 0, 0},
    {"CannotDropIn", Action_CannotDropIn, 1, 0, 0},
    {"CanAttack", Action_CanAttack, 0, 0, 0},
    {"NotWithParty", Action_NotWithParty, 1, 0, 0},
    {"TakeDamage", Action_TakeDamage, 0, 0, 0},
    {"TagCharacter", Action_TagCharacter, 1, 0, 0},
    {"CanHitForceObjects", Action_CanHitForceObjects, 0, 0, 0},
    {"AlwaysBackFlip", Action_AlwaysBackFlip, 0, 0, 0},
    {"PlayerSpeederHack", Action_PlayerSpeederHack, 0, 0, 0},
    {"SetAnimSpeedMul", Action_SetAnimSpeedMul, 0, 0, 0},
    {"SetSide", Action_SetSide, 1, 0, 0},
    {"SetStateArea", Action_SetStateArea, 0, 0, 0},
    {"SetOpponent", Action_SetOpponent, 0, 0, 0},
    {"AttackOpponent", Action_AttackOpponent, 0, 0, 0},
    {"EngageOpponent", Action_EngageOpponent, 0, 0, 0},
    {"ShootAtOpponent", Action_ShootAtOpponent, 0, 0, 0},
    {"EngageObject", Action_EngageObject, 0, 0, 0},
    {"GrabVictim", Action_GrabVictim, 0, 0, 0},
    {"EatVictim", Action_EatVictim, 0, 0, 0},
    {"ReleaseVictim", Action_ReleaseVictim, 0, 0, 0},
    {"CanDefend", Action_CanDefend, 0, 0, 0},
    {"DontAimAt", Action_DontAimAt, 0, 0, 0},
    {"CanUseWeapon", Action_CanUseWeapon, 0, 0, 0},
    {"SetBoss", Action_SetBoss, 0, 0, 0},
    {"UpdateSockPos", Action_UpdateSockPos, 0, 0, 0},
    {"UseForce", Action_UseForce, 0, 0, 0},
    {"TriggerBlowUp", Action_TriggerBlowUp, 0, 0, 0},
    {"ForcePush", Action_ForcePush, 0, 0, 0},
    {"DeflectPlayersPart", Action_DeflectPlayersPart, 0, 0, 0},
    {"Kill", Action_Kill, 1, 0, 0},
    {"Explode", Action_Explode, 0, 0, 0},
    {"SetScriptState", Action_SetScriptState, 0, 0, 0},
    {"SetAIOverrideControl", Action_SetAIOverrideControl, 0, 0, 0},
    {"SetLastSafePathPos", Action_SetLastSafePathPos, 0, 0, 0},
    {"SetDontMove", Action_SetDontMove, 0, 0, 0},
    {"DontSetStoppedFlag", Action_DontSetStoppedFlag, 0, 0, 0},
    {"PressSpecialButton", Action_PressSpecialButton, 0, 0, 0},
    {"PressTagButton", Action_PressTagButton, 0, 0, 0},
    {"PressActionButton", Action_PressActionButton, 0, 0, 0},
    {"UseWeapon", Action_UseWeapon, 0, 0, 0},
    {"SetInvulnerable", Action_SetInvulnerable, 0, 0, 0},
    {AIRegistryTextPc007566a8, Action_GameDontPush, 0, 0, 0},
    {"DontAvoidCharacter", Action_DontAvoidCharacter, 0, 0, 0},
    {"PressJumpButton", Action_PressJumpButton, 0, 0, 0},
    {"AddToSet", Action_AddToSet, 0, 0, 0},
    {"SetSpline", Action_SetSpline, 0, 0, 0},
    {"SetControlSystem", Action_SetControlSystem, 0, 0, 0},
    {"SetZeroAcceleration", Action_SetZeroAcceleration, 0, 0, 0},
    {"FollowDirection", Action_FollowDirection, 0, 0, 0},
    {"BreakFormation", Action_BreakFormation, 0, 0, 0},
    {"FormationMove", Action_FormationMove, 0, 0, 0},
    {"CreateCreatures", Action_CreateCreatures, 0, 0, 0},
    {"SelectRandomSpline", Action_SelectRandomSpline, 0, 0, 0},
    {"CreateSplineCreatures", Action_CreateSplineCreatures, 0, 0, 0},
    {"Launch", Action_Launch, 0, 0, 0},
    {"SetCurrentSpeed", Action_SetCurrentSpeed, 0, 0, 0},
    {"SetRunSpeed", Action_SetRunSpeed, 0, 0, 0},
    {"SetWalkSpeed", Action_SetWalkSpeed, 0, 0, 0},
    {"SetHitPoints", Action_SetHitPoints, 1, 0, 0},
    {"SetShieldHitPoints", Action_SetShieldHitPoints, 1, 0, 0},
    {"SetMessage", Action_SetMessage, 1, 0, 0},
    {"CopyMessage", Action_CopyMessage, 0, 0, 0},
    {"SetScriptParam", Action_SetScriptParam, 0, 0, 0},
    {"AddPart", Action_AddPart, 0, 0, 0},
    {"AddPartDebris", Action_AddPartDebris, 0, 0, 0},
    {"LaunchGuidedMissile", Action_LaunchGuidedMissile, 0, 0, 0},
    {"SetHoverPhase", Action_SetHoverPhase, 0, 0, 0},
    {"UseCurrentSpeed", Action_UseCurrentSpeed, 0, 0, 0},
    {"SetMaxMovementRange", Action_SetMaxMovementRange, 0, 0, 0},
    {"SetDefaultMovementRange", Action_SetDefaultMovementRange, 0, 0, 0},
    {"SetGravityHeight", Action_SetGravityHeight, 0, 0, 0},
    {"ApplyGravity", Action_ApplyGravity, 0, 0, 0},
    {"IgnoreShoveSystem", Action_IgnoreShoveSystem, 0, 0, 0},
    {"CannotBeSeen", Action_CannotBeSeen, 0, 0, 0},
    {"CannotBeForcedBack", Action_CannotBeForcedBack, 0, 0, 0},
    {"CanTurn", Action_CanTurn, 0, 0, 0},
    {"NoIdleSpeed", Action_NoIdleSpeed, 0, 0, 0},
    {"SetVisibility", Action_SetVisibility, 1, 0, 0},
    {"EnableSock", Action_EnableSock, 1, 0, 0},
    {"AddDebris", Action_AddDebris, 0, 0, 0},
    {"JudderGameCamera", Action_JudderGameCamera, 0, 0, 0},
    {"CameraShake", Action_CameraShake, 0, 0, 0},
    {"ResetGameCamera", Action_ResetGameCamera, 1, 0, 0},
    {"PlayCutScene", Action_PlayCutScene, 1, 0, 0},
    {"SetLevelPath", Action_SetLevelPath, 0, 0, 0},
    {"ImmuneToKillTerrain", Action_ImmuneToKillTerrain, 0, 0, 0},
    {"ImmuneToBolts", Action_ImmuneToBolts, 0, 0, 0},
    {"Respawnable", Action_Respawnable, 0, 0, 0},
    {"SetPathCnxFlag", Action_SetPathCnxFlag, 1, 0, 0},
    {"SetHint", Action_SetHint, 0, 0, 0},
    {"SetHintComplete", Action_SetHintComplete, 0, 0, 0},
    {"CancelHint", Action_CancelHint, 0, 0, 0},
    {"CycleCharacter", Action_CycleCharacter, 0, 0, 0},
    {"CnxController", Action_CnxController, 0, 0, 0},
    {"CnxHelper", Action_CnxHelper, 0, 0, 0},
    {"PlaySfx", Action_PlaySfx, 0, 0, 0},
    {"CameraCut", Action_CameraCut, 1, 0, 0},
    {"DynamicCameraCut", Action_DynamicCameraCut, 1, 0, 0},
    {"EndCameraCut", Action_EndCameraCut, 1, 0, 0},
    {"DontRaycastLOS", Action_DontRaycastLOS, 0, 0, 0},
    {"SetForceBack", Action_SetForceBack, 1, 0, 0},
    {"FaceCamera", Action_FaceCamera, 0, 0, 0},
    {"FaceCharacter", Action_FaceCharacter, 0, 0, 0},
    {"SpinOnSpot", Action_SpinOnSpot, 0, 0, 0},
    {"FollowCharacter", Action_FollowCharacter, 0, 0, 0},
    {AIRegistryTextPc007562c8, Action_GameFollowPlayer, 0, 0, 0},
    {"MoveForward", Action_MoveForward, 0, 0, 0},
    {"SetFormationCommander", Action_SetFormationCommander, 0, 0, 0},
    {"RemoveThrownForceObjects", Action_RemoveThrownForceObjects, 0, 0, 0},
    {"AlwaysTriggerObstacle", Action_AlwaysTriggerObstacle, 0, 0, 0},
    {"CanTriggerObstacle", Action_CanTriggerObstacle, 0, 0, 0},
    {"PlayGizObstacle", Action_PlayGizObstacle, 1, 0, 0},
    {"PlayObstacle", Action_PlayGizObstacle, 1, 0, 0},
    {"PlayGizSpecial", Action_PlayGizSpecial, 1, 0, 0},
    {"SetObstacleToEnd", Action_SetObstacleToEnd, 1, 0, 0},
    {AIRegistryTextPc00756e7c, Action_HelpWithTriggers, 0, 0, 0},
    {"UseTriggerSet", Action_UseTriggerSet, 0, 0, 0},
    {"PullLever", Action_PullLever, 0, 0, 0},
    {"UsePanel", Action_UsePanel, 0, 0, 0},
    {"UseTechno", Action_UseTechno, 0, 0, 0},
    {"ReleaseLocator", Action_ReleaseLocator, 0, 0, 0},
    {"AssignLocator", Action_AssignLocator, 0, 0, 0},
    {"GetLocatorFromSet", Action_GetLocatorFromSet, 0, 0, 0},
    {"MoveAwayFromLastAttacker", Action_MoveAwayFromLastAttacker, 0, 0, 0},
    {"ProbeDroid", Action_ProbeDroid, 0, 0, 0},
    {"AlertCreatures", Action_AlertCreatures, 0, 0, 0},
    {"SetLastAttacker", Action_SetLastAttacker, 0, 0, 0},
    {"LinkTurretToController", Action_LinkTurretToController, 0, 0, 0},
    {"TakeOver", Action_TakeOver, 1, 0, 0},
    {"ReleaseTakeOver", Action_ReleaseTakeOver, 1, 0, 0},
    {"RegisterTakeOverObject", Action_RegisterTakeOverObject, 0, 0, 0},
    {"SetTakeOverTarget", Action_SetTakeOverTarget, 0, 0, 0},
    {"ClearTakeOverTarget", Action_ClearTakeOverTarget, 0, 0, 0},
    {"AddGameMsgCount", Action_AddGameMsgCount, 1, 0, 0},
    {"AddMiscPickups", Action_AddMiscPickups, 0, 0, 0},
    {"SetCanTakeOver", Action_SetCanTakeOver, 0, 0, 0},
    {"CanBeCarried", Action_CanBeCarried, 1, 0, 0},
    {"IgnoreLastSafePathPos", Action_IgnoreLastSafePathPos, 0, 0, 0},
    {"AwkwardShapeOverride", Action_AwkwardShapeOverride, 0, 0, 0},
    {"IgnoreSlideTerrain", Action_IgnoreSlideTerrain, 0, 0, 0},
    {"SplineFollowTerrain", Action_SplineFollowTerrain, 0, 0, 0},
    {"SetLayer", Action_SetLayer, 0, 0, 0},
    {"CreateRider", Action_CreateRider, 0, 0, 0},
    {"AddTorpedoPacket", Action_AddTorpedoPacket, 1, 0, 0},
    {"SpeederBeingChased", Action_SpeederBeingChased, 0, 0, 0},
    {"ThrowDetonator", Action_ThrowDetonator, 0, 0, 0},
    {"SetScaleOverride", Action_SetScaleOverride, 0, 0, 0},
    {"DisableNarrowSocks", Action_DisableNarrowSocks, 1, 0, 0},
    {"UseTimeBasedUpdate", Action_UseTimeBasedUpdate, 0, 0, 0},
    {"ForceLightning", Action_ForceLightning, 0, 0, 0},
    {"WalkBackwards", Action_WalkBackwards, 0, 0, 0},
    {"AddScriptProcessor", Action_AddScriptProcessor, 0, 0, 0},
    {"SetUseOneAtOnce", Action_SetUseOneAtOnce, 0, 0, 0},
    {"SetAO_MaxAttackers", Action_SetAO_MaxAttackers, 0, 0, 0},
    {"SetAO_AttackersPerRow", Action_SetAO_AttackersPerRow, 0, 0, 0},
    {"SetAO_RowDist", Action_SetAO_RowDist, 0, 0, 0},
    {"SetAO_InitRowDist", Action_SetAO_InitRowDist, 0, 0, 0},
    {"SetTechnoComplete", Action_SetTechnoComplete, 0, 0, 0},
    {"LetGoOfBalloon", Action_LetGoOfBalloon, 0, 0, 0},
    {"DrawBossHitPoints", Action_DrawBossHitPoints, 1, 0, 0},
    {"CompleteLevel", Action_CompleteLevel, 1, 0, 0},
    {"GoToNewLevel", Action_GoToNewLevel, 0, 0, 0},
    {"CircleLocator", Action_CircleLocator, 0, 0, 0},
    {"GizmoActivate", Action_GizmoActivate, 0, 0, 0},
    {"GizmoSetVisibility", Action_GizmoSetVisibility, 0, 0, 0},
    {"TurnOnPickup", Action_TurnOnPickup, 0, 0, 0},
    {"CanHelpWithTriggers", Action_CanHelpWithTriggers, 0, 0, 0},
    {"CanCollideWithObjects", Action_CanCollideWithObjects, 0, 0, 0},
    {"SetShootOpponents", Action_SetShootOpponents, 0, 0, 0},
    {"PartyCanBeUnderCover", Action_PartyCanBeUnderCover, 0, 0, 0},
    {"SetLapTime", Action_SetLapTime, 0, 0, 0},
    {"CreatePod", Action_CreatePod, 0, 0, 0},
    {"MushroomCollapse", Action_MushroomCollapse, 0, 0, 0},
    {"BoulderSection", Action_BoulderSection, 0, 0, 0},
    {"RaceOpponent", Action_RaceOpponent, 0, 0, 0},
    {"Sebulba", Action_Sebulba, 0, 0, 0},
    {"IgnoreTurnAroundSpline", Action_IgnoreTurnAroundSpline, 0, 0, 0},
    {"CanMoveWhenDeactivated", Action_CanMoveWhenDeactivated, 0, 0, 0},
    {"DontAttack", Action_DontAttack, 0, 0, 0},
    {"CanPullLevers", Action_CanPullLevers, 0, 0, 0},
    {"NewSebulba", Action_NewSebulba, 0, 0, 0},
    {NULL, NULL, 0, 0, 0},
};
#pragma data_seg()

#pragma data_seg(".data$lego_aiconditiondefs")
AICONDITIONDEF_PC lego_aiconditiondefs[171] = {
    {"GlynTest", Condition_GlynTest, NULL},
    {"Debug", Condition_Debug, NULL},
    {AIRegistryTextPc007575bc, Condition_Active, NULL},
    {"GotGun", Condition_GotGun, NULL},
    {"PrefersBrawling", Condition_PrefersBrawling, NULL},
    {"IsAlive", Condition_IsAlive, Condition_IsAliveInit},
    {"IsOnScreen", Condition_IsOnScreen, Condition_IsOnScreenInit},
    {"OffScreenTimer", Condition_OffScreenTimer, Condition_OffScreenTimerInit},
    {"OnObject", Condition_OnObject, Condition_OnObjectInit},
    {"OnSameObjectAsPlayer", Condition_OnSameObjectAsPlayer, NULL},
    {"PlayerOnObject", Condition_PlayerOnObject, Condition_OnObjectInit},
    {"EitherPlayerOnObject", Condition_EitherPlayerOnObject, Condition_OnObjectInit},
    {"EitherPlayerLocatorRangeXZ", Condition_EitherPlayerLocatorRangeXZ, Condition_EitherPlayerLocatorRangeXZInitPc},
    {"OnGround", Condition_OnGround, NULL},
    {"BeenAlerted", Condition_BeenAlerted, NULL},
    {"PlayerOnGround", Condition_PlayerOnGround, NULL},
    {"SpawnCount", Condition_SpawnCount, NULL},
    {"BehindCamera", Condition_BehindCamera, NULL},
    {"LocatorOnScreen", Condition_LocatorOnScreen, Condition_LocatorOnScreenInit},
    {"Blocking", Condition_Blocking, NULL},
    {"BeenHit", Condition_BeenHit, Condition_BeenHitInit},
    {"HoverPhase", Condition_HoverPhase, NULL},
    {"HitPoints", Condition_HitPoints, Condition_HitPointsInit},
    {"OnDynamicGrapple", Condition_OnDynamicGrapple, Condition_OnDynamicGrappleInit},
    {"XPos", Condition_XPos, Condition_XPosInitPc},
    {"YPos", Condition_YPos, Condition_XPosInitPc},
    {"ZPos", Condition_ZPos, Condition_XPosInitPc},
    {"CollidingWithOpponent", Condition_CollidingWithOpponent, NULL},
    {"Colliding", Condition_Colliding, NULL},
    {"ObstacleAtStart", Condition_ObstacleAtStart, Condition_ObstacleAtEndInit},
    {"ObstacleAtEnd", Condition_ObstacleAtEnd, Condition_ObstacleAtEndInit},
    {"SpecialAtStart", Condition_ObstacleAtStart, Condition_SpecialAtStartInitPc},
    {"SpecialAtEnd", Condition_ObstacleAtEnd, Condition_SpecialAtStartInitPc},
    {"ObstacleLockedOpen", Condition_ObstacleLockedOpen, Condition_ObstacleOpenedByPlayerInit},
    {"ObstacleLockedShut", Condition_ObstacleLockedShut, Condition_ObstacleOpenedByPlayerInit},
    {"ForceAtStart", Condition_ForceAtStart, Condition_ForceInit},
    {"ForceAtEnd", Condition_ForceAtEnd, Condition_ForceInit},
    {"ObstacleOpenedByPlayer", Condition_ObstacleOpenedByPlayer, Condition_ObstacleOpenedByPlayerInit},
    {"ObstacleOpenedByEitherPlayer", Condition_ObstacleOpenedByEitherPlayer, Condition_ObstacleOpenedByPlayerInit},
    {"AnimationFinished", Condition_AnimationFinished, Condition_AnimationFinishedInit},
    {"EitherPlayerPullingLever", Condition_EitherPlayerPullingLever, NULL},
    {"EitherPlayerUsingHatMachine", Condition_EitherPlayerUsingHatMachinePc, NULL},
    {"EitherPlayerUsingPanel", Condition_EitherPlayerUsingPanel, NULL},
    {"EitherPlayerWearingHelmet", Condition_EitherPlayerWearingHelmet, NULL},
    {"PartyUnderCover", Condition_PartyUnderCover, NULL},
    {"NumBaddiesThatCanSeePlayers", Condition_NumBaddiesThatCanSeePlayers, NULL},
    {"PlayerUsingForce", Condition_PlayerUsingForce, Condition_UsingForceInit},
    {"EitherPlayerUsingForce", Condition_EitherPlayerUsingForce, Condition_UsingForceInit},
    {"UsingForce", Condition_UsingForce, Condition_UsingForceInit},
    {"OnForcePlatform", Condition_OnForcePlatform, Condition_OnForcePlatformInit},
    {"PlayerOnForcePlatform", Condition_PlayerOnForcePlatform, Condition_OnForcePlatformInit},
    {"EitherPlayerOnForcePlatform", Condition_EitherPlayerOnForcePlatform, Condition_OnForcePlatformInit},
    {"ForceBeingUsed", Condition_ForceBeingUsed, Condition_UsingForceInit},
    {"ForcePushing", Condition_ForcePushing, Condition_ForcePushingInit},
    {"TurretAlive", Condition_TurretAlive, Condition_TurretAliveInit},
    {"PlayerDeflectingPart", Condition_PlayerDeflectingPart, NULL},
    {"ForceComplete", Condition_ForceComplete, Condition_ForceCompleteInit},
    {"ForceFinished", Condition_ForceFinished, Condition_ForceCompleteInit},
    {"BuildItComplete", Condition_BuildItComplete, Condition_BuildItCompleteInit},
    {"BlowupBlownup", Condition_BlowupBlownup, Condition_BlowupBlownupInitPc},
    {"IAmA", Condition_IAmA, Condition_IAmAInit},
    {"OpponentIsA", Condition_OpponentIsA, Condition_OpponentIsAInit},
    {AIRegistryTextPc007571b8, Condition_GameOpponentIsAThreatPc, NULL},
    {"CanFightLikeAJedi", Condition_CanFightLikeAJedi, NULL},
    {"IAmAGoody", Condition_IAmAGoody, NULL},
    {"IAmABaddy", Condition_IAmABaddy, NULL},
    {"IAmANeutral", Condition_IAmANeutral, NULL},
    {"IAmAGoodyBaddy", Condition_IAmAGoodieBaddie, NULL},
    {"IAmAPartyCharacter", Condition_IAmAPartyCharacter, NULL},
    {"CategoryIs", Condition_CategoryIs, Condition_CategoryIsInit},
    {"PlayerCategoryIs", Condition_PlayerCategoryIs, Condition_CategoryIsInit},
    {"EitherPlayerIs", Condition_EitherPlayerIs, Condition_EitherPlayerIsInit},
    {"Player1Is", Condition_Player1Is, Condition_EitherPlayerIsInit},
    {"Player2Is", Condition_Player2Is, Condition_EitherPlayerIsInit},
    {"IsSetAlive", Condition_IsSetAlive, Condition_IsSetAliveInit},
    {"NumInSetAlive", Condition_NumInSetAlive, Condition_IsSetAliveInit},
    {"Context", Condition_Context, NULL},
    {"InContext", Condition_InContext, Condition_InContextInit},
    {"OpponentContext", Condition_OpponentContext, Condition_InContextInit},
    {"Player2Active", Condition_Player2Active, NULL},
    {"NumBaddies", Condition_NumBaddies, Condition_NumBaddiesInit},
    {"NumForceObjects", Condition_NumForceObjects, Condition_NumForceObjectsInit},
    {"BeenToLevel", Condition_BeenToLevel, Condition_BeenToLevelInit},
    {"LastLevel", Condition_LastLevel, Condition_LastLevelInit},
    {"Message", Condition_Message, Condition_MessageInit},
    {"ScriptParam", Condition_ScriptParam, Condition_ScriptParamInit},
    {"CutSceneStarted", Condition_CutSceneStarted, Condition_CutSceneStartedInit},
    {"CutSceneFinished", Condition_CutSceneFinished, Condition_CutSceneFinishedInit},
    {"CutSceneExists", Condition_CutSceneExists, Condition_CutSceneExistsInit},
    {"PlayerInSock", Condition_PlayerInSock, Condition_PlayerInSockInit},
    {"CutScenePlaying", Condition_CutScenePlaying, Condition_CutScenePlayingInit},
    {"RigidAnimFrame", Condition_RigidAnimFrame, Condition_RigidAnimFrameInit},
    {"SockDistanceToPlayer", Condition_SockDistanceToPlayer, NULL},
    {"SockDistanceToOpponent", Condition_SockDistanceToOpponent, NULL},
    {"SockXDistanceToPlayer", Condition_SockXDistanceToPlayer, NULL},
    {"PlayerDistanceAlongSock", Condition_PlayerDistanceAlongSock, NULL},
    {"FurthestPlayerDistanceAlongSock", Condition_FurthestPlayerDistanceAlongSock, NULL},
    {"FinishedSpline", Condition_FinishedSpline, NULL},
    {"CurrentHintId", Condition_CurrentHintId, NULL},
    {"HintAvailable", Condition_HintAvailable, Condition_HintAvailableInit},
    {"HintComplete", Condition_HintComplete, Condition_HintAvailableInit},
    {"Freeplay", Condition_Freeplay, NULL},
    {"Indy", Condition_IndyPc, NULL},
    {"MissionMode", Condition_MissionMode, NULL},
    {"MissionWon", Condition_MissionWon, NULL},
    {"ChallengeMode", Condition_ChallengeMode, NULL},
    {"PSP", Condition_PSP, NULL},
    {"AIOverrideControl", Condition_AIOverrideControl, Condition_AIOverrideControlInit},
    {"BoltsDontGetDeflectedBack", Condition_BoltsDontGetDeflectedBack, NULL},
    {"CheatProgress", Condition_CheatProgress, NULL},
    {"BigJumpComplete", Condition_BigJumpComplete, NULL},
    {"RespawnLocatorIs", Condition_RespawnLocatorIs, Condition_RespawnLocatorIsInit},
    {"InMiniCut", Condition_InMiniCut, NULL},
    {"MaulShouldRunAway", Condition_MaulShouldRunAway, NULL},
    {"DropBackInTimer", Condition_DropBackInTimer, NULL},
    {AIRegistryTextPc00756e7c, Condition_HelpWithTriggers, NULL},
    {"EitherPlayerPushingSpinner", Condition_EitherPlayerPushingSpinner, Condition_EitherPlayerPushingSpinnerInit},
    {"CharacterRange", Condition_CharacterRange, Condition_CharacterRangeInit},
    {"BeenSpawned", Condition_BeenSpawned, NULL},
    {"LastAttackerRange", Condition_LastAttackerRange, NULL},
    {"LastAttackerIsActivePlayer", Condition_LastAttackerIsActivePlayer, NULL},
    {"PartyContainsDroids", Condition_PartyContainsDroids, NULL},
    {"CannotReachDestination", Condition_CannotReachDestination, NULL},
    {"TakenOver", Condition_TakenOver, Condition_TakenOverInit},
    {"PlayerTakenOver", Condition_PlayerTakenOver, NULL},
    {"EitherPlayerTakenOver", Condition_EitherPlayerTakenOver, NULL},
    {"BeenTakenOver", Condition_BeenTakenOver, Condition_BeenTakenOverInit},
    {"OnSpeederBike", Condition_OnSpeederBike, Condition_OnSpeederBikeInit},
    {"UnderPlayerControl", Condition_UnderPlayerControl, Condition_UnderPlayerControlInit},
    {"CharacterExists", Condition_CharacterExists, Condition_CharacterExistsInit},
    {"CharacterTypeExists", Condition_CharacterTypeExists, Condition_CharacterTypeExistsInit},
    {"GotLocatorInSet", Condition_GotLocatorInSet, Condition_GotLocatorInSetInit},
    {"GotOpponentLOS", Condition_GotOpponentLOS, NULL},
    {"EmptyTakeOver", Condition_EmptyTakeOver, NULL},
    {"HasTakeOverTarget", Condition_HasTakeOver, NULL},
    {"TakeOverRange", Condition_TakeOverRange, NULL},
    {"TakeOverTargetInTriggerArea", Condition_TakeOverTargetInTriggerArea, Condition_TakeOverTargetInTriggerAreaInit},
    {"EitherPlayerInMyTriggerArea", Condition_EitherPlayerInMyTriggerArea, NULL},
    {"GotVictim", Condition_GotVictim, NULL},
    {"IsVisible", Condition_IsVisible, Condition_IsVisibleInit},
    {"MySet", Condition_MySet, NULL},
    {"ScreenWipe", Condition_ScreenWipe, NULL},
    {"IAmPlayer2", Condition_IAmPlayer2, NULL},
    {"HeadTurnRestricted", Condition_HeadTurnRestricted, NULL},
    {"ShopActive", Condition_ShopActive, NULL},
    {"Side", Condition_Side, Condition_SideInit},
    {"NearestPartyRange", Condition_NearestPartyRange, NULL},
    {"NearestPartyXZRange", Condition_NearestPartyXZRange, NULL},
    {"OpponentToPlayerRange", Condition_OpponentToPlayerRange, NULL},
    {"OpponentPathPosRange", Condition_OpponentPathPosRange, NULL},
    {"GizmoOutput0", Condition_GizmoOutput0, Condition_GizmoOutput0InitPc},
    {"GizmoOutput1", Condition_GizmoOutput1, Condition_GizmoOutput0InitPc},
    {"GizmoOutput2", Condition_GizmoOutput2, Condition_GizmoOutput0InitPc},
    {"GizmoOutput3", Condition_GizmoOutput3, Condition_GizmoOutput0InitPc},
    {"GizmoVisibility", Condition_GizmoVisibility, Condition_GizmoVisibilityInit},
    {"AngleAboutMyLocatorToPlayer", Condition_AngleAboutMyLocatorToPlayer, Condition_AngleAboutMyLocatorToPlayerInit},
    {"AnimSpeedMul", Condition_AnimSpeedMul, NULL},
    {"PickupBeenTurnedOn", Condition_PickupBeenTurnedOn, Condition_PickupBeenTurnedOnInit},
    {"FlowBoxComplete", Condition_FlowBoxComplete, Condition_FlowBoxCompleteInit},
    {"CanHearRadio", Condition_CanHearRadio, NULL},
    {"BeingTowed", Condition_BeingTowed, NULL},
    {"RaceLap", Condition_RaceLapPc, NULL},
    {"MusicOn", Condition_MusicOn, NULL},
    {"CharacterLoaded", Condition_CharacterLoaded, Condition_CharacterLoadedInit},
    {"AreaComplete", Condition_AreaComplete, Condition_AreaCompleteInit},
    {"ShouldAttackOpponent", Condition_ShouldAttackOpponent, NULL},
    {"InSwamp", Condition_InSwamp, NULL},
    {"InSameTriggerAreaAsNearestPlayer", Condition_InSameTriggerAreaAsNearestPlayer, NULL},
    {"NetworkGameOnGoing", Condition_NetworkGameOnGoing, NULL},
    {"InHubArea", Condition_InHubArea, Condition_InHubAreaInit},
    {NULL, NULL, NULL},
};
#pragma data_seg()

#pragma data_seg(".data$api_aiactiondefs")
AIACTIONDEF_PC api_aiactiondefs[55] = {
    {"Idle", Action_Idle, 0, 0, 0},
    {"SetState", Action_SetState, 0, 0, 0},
    {"ResetTimer", Action_ResetTimer, 0, 0, 0},
    {"RetreatFromNearestOpponent", Action_RetreatFromNearestOpponent, 0, 0, 0},
    {"RetreatFromOpponent", Action_RetreatFromOpponent, 0, 0, 0},
    {"MoveAwayFromPlayer", Action_MoveAwayFromPlayer, 0, 0, 0},
    {"MoveAwayFromPlayer2", Action_MoveAwayFromPlayer2, 0, 0, 0},
    {"SetCircleDirection", Action_SetCircleDirection, 0, 0, 0},
    {"Circle", Action_Circle, 0, 0, 0},
    {"CircleOpponent", Action_CircleOpponent, 0, 0, 0},
    {"CirclePlayer", Action_CirclePlayer, 0, 0, 0},
    {AIRegistryTextPc007562c8, Action_FollowPlayer, 0, 0, 0},
    {"MoveAwayFromOpponent", Action_MoveAwayFromOpponent, 0, 0, 0},
    {"FollowOpponent", Action_FollowOpponent, 0, 0, 0},
    {"FacePlayer", Action_FacePlayer, 0, 0, 0},
    {"FaceOpponent", Action_FaceOpponent, 0, 0, 0},
    {"FaceLocator", Action_FaceLocator, 0, 0, 0},
    {"IgnoreWallSplines", Action_IgnoreWallSplines, 0, 0, 0},
    {"CheckWallSplines", Action_CheckWallSplines, 0, 0, 0},
    {"NoTerrain", Action_NoTerrain, 0, 0, 0},
    {"FlatTerrain", Action_FlatTerrain, 0, 0, 0},
    {"ShadowTerrain", Action_ShadowTerrain, 0, 0, 0},
    {"DontUseShadowTerrain", Action_DontUseShadowTerrain, 0, 0, 0},
    {AIRegistryTextPc007566a8, Action_DontPush, 0, 0, 0},
    {"CanSeeBehind", Action_CanSeeBehind, 0, 0, 0},
    {"RequiresLOS", Action_RequiresLOS, 0, 0, 0},
    {"SetFullPathSearch", Action_SetFullPathSearch, 0, 0, 0},
    {"SetViewDistance", Action_SetViewDistance, 0, 0, 0},
    {"SetMaxViewHeight", Action_SetMaxViewHeight, 0, 0, 0},
    {"SetMinViewHeight", Action_SetMinViewHeight, 0, 0, 0},
    {"SetHearDistance", Action_SetHearDistance, 0, 0, 0},
    {"SetMoveRadius", Action_SetMoveRadius, 0, 0, 0},
    {"GoToNode", Action_GoToNode, 0, 0, 0},
    {"GoToNodeRandom", Action_GoToNodeRandom, 0, 0, 0},
    {"GoToOrigin", Action_GoToOrigin, 0, 0, 0},
    {"GoToLocator", Action_GoToLocator, 0, 0, 0},
    {AIRegistryTextPc00756a74, Action_SetLocator, 0, 0, 0},
    {"SetRespawnLocator", Action_SetRespawnLocator, 0, 0, 0},
    {"FollowPath", Action_FollowPath, 0, 0, 0},
    {"MoveAwayFromNode", Action_MoveAwayFromNode, 0, 0, 0},
    {"OverrideAnimation", Action_OverrideAnimation, 1, 0, 0},
    {"BlockPath", Action_BlockPath, 0, 0, 0},
    {"PathConnectionObstacle", Action_PathConnectionObstacle, 0, 0, 0},
    {"PathConnectionMaxLength", Action_PathConnectionMaxLength, 0, 0, 0},
    {"NoLosCheck", Action_NoLosCheck, 0, 0, 0},
    {"ResetToOrigin", Action_ResetToOrigin, 0, 0, 0},
    {"SetInterrupt", Action_SetInterrupt, 0, 0, 0},
    {"ClearInterrupt", Action_ClearInterrupt, 0, 0, 0},
    {"SetIgnoreAntinodes", Action_SetIgnoreAntinodes, 0, 0, 0},
    {"NoShadows", Action_NoShadows, 0, 0, 0},
    {"SetParam", Action_SetParam, 0, 0, 0},
    {"SetReturnToState", Action_SetReturnToState, 0, 0, 0},
    {"ReturnToState", Action_ReturnToState, 0, 0, 0},
    {"NotifyStateChange", Action_NotifyStateChange, 0, 0, 0},
    {NULL, NULL, 0, 0, 0},
};
#pragma data_seg()

#pragma data_seg(".data$api_aiconditiondefs")
AICONDITIONDEF_PC api_aiconditiondefs[50] = {
    {"PreviousResult", NULL, NULL},
    {"AlwaysTrue", Condition_AlwaysTrue, Condition_AlwaysTrueInit},
    {"LocatorRange", Condition_LocatorRange, Condition_LocatorRangeInit},
    {"LocatorRangeXZ", Condition_LocatorRangeXZ, Condition_LocatorRangeInit},
    {"LocatorRangeY", Condition_LocatorRangeY, Condition_LocatorRangeInit},
    {"Timer", Condition_Timer, NULL},
    {"Random", Condition_Random, NULL},
    {"GotLocator", Condition_GotLocator, NULL},
    {"GotLocatorSet", Condition_GotLocatorSet, NULL},
    {"CurrentLocatorIs", Condition_CurrentLocatorIs, Condition_CurrentLocatorIsInit},
    {"InTriggerArea", Condition_InTriggerArea, Condition_InTriggerAreaInit},
    {"PlayerRange", Condition_PlayerRange, NULL},
    {"NearestPlayerRange", Condition_NearestPlayerRange, NULL},
    {"NearestPlayerXZRange", Condition_NearestPlayerXZRange, NULL},
    {"InLevelNode", Condition_InLevelNode, Condition_InLevelNodeInit},
    {"PlayerInLevelNode", Condition_PlayerInLevelNode, Condition_PlayerInLevelNodeInit},
    {"EitherPlayerInLevelNode", Condition_EitherPlayerInLevelNode, Condition_PlayerInLevelNodeInit},
    {"NodeRange", Condition_NodeRangePc, Condition_NodeRangeInitPc},
    {"GotTriggerArea", Condition_GotTriggerArea, NULL},
    {"PlayerInTriggerArea", Condition_PlayerInTriggerArea, Condition_PlayerInTriggerAreaInit},
    {"Player2InTriggerArea", Condition_Player2InTriggerArea, Condition_PlayerInTriggerAreaInit},
    {"EitherPlayerInTriggerArea", Condition_EitherPlayerInTriggerArea, Condition_PlayerInTriggerAreaInit},
    {"BaddyInTriggerArea", Condition_BaddyInTriggerArea, Condition_BaddyInTriggerAreaInit},
    {"GoodyInTriggerArea", Condition_GoodyInTriggerArea, Condition_GoodyInTriggerAreaInit},
    {"OpponentInTriggerArea", Condition_OpponentInTriggerArea, Condition_OpponentInTriggerAreaInit},
    {"GotOpponent", Condition_GotOpponent, NULL},
    {AIRegistryTextPc007571b8, Condition_OpponentIsAThreat, NULL},
    {"OpponentOnSamePath", Condition_OpponentOnSamePath, NULL},
    {"OpponentRange", Condition_OpponentRange, NULL},
    {"NearestOpponentRange", Condition_NearestOpponentRange, NULL},
    {"YawToOpponent", Condition_YawToOpponent, NULL},
    {"OpponentBelow", Condition_OpponentBelow, NULL},
    {"OriginRange", Condition_OriginRange, NULL},
    {"OpponentToOrigin", Condition_OpponentToOrigin, NULL},
    {"PlayerToOrigin", Condition_PlayerToOrigin, NULL},
    {"OpponentToLocator", Condition_OpponentToLocator, Condition_OpponentToLocatorInit},
    {"OpponentToLocatorXZ", Condition_OpponentToLocatorXZ, Condition_OpponentToLocatorInit},
    {"OpponentToLocatorY", Condition_OpponentToLocatorY, Condition_OpponentToLocatorInit},
    {"PlayerToLocator", Condition_PlayerToLocator, Condition_PlayerToLocatorInit},
    {"NearestPlayerToLocator", Condition_NearestPlayerToLocator, Condition_PlayerToLocatorInit},
    {"OnPath", Condition_OnPath, NULL},
    {"PlayerOnPath", Condition_PlayerOnPath, NULL},
    {"OpponentOnPath", Condition_OpponentOnPath, NULL},
    {"TimeOffPath", Condition_TimeOffPath, NULL},
    {"PathBlocked", Condition_PathBlocked, NULL},
    {"InterruptID", Condition_InterruptID, NULL},
    {"IAm", Condition_IAm, Condition_IAmInit},
    {"StuckTime", Condition_StuckTime, NULL},
    {"Param", Condition_Param, NULL},
    {NULL, NULL, NULL},
};
#pragma data_seg()

#pragma bss_seg(".bss$game_aiactiondefs")
AIACTIONDEF_PC *game_aiactiondefs = 0;
#pragma bss_seg(".bss$game_aiconditiondefs")
AICONDITIONDEF_PC *game_aiconditiondefs = 0;
#pragma bss_seg()
