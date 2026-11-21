// Floating-point constants the game reads from memory: shared by several modules, or read by a C++ unit
// that would fold a const of its own into an immediate.
#include "nu2api/nucore/common.h"
#include "batman.h"
#include "nu2api/numath/numtx.h"

const f64 GameObjectNearFloorNoFloorHeight = 2000000.0;
const f32 AIPositionUnset = 1000000000.0f;
const f64 VehicleTurnPhaseOffset = 16384.0;
const f64 NuQFntHalfScale = 0.5;
const f64 PlayerRandomScale = 1.5259021893143654e-05;
const f64 PCPathArcSineCoefficient9 = -0.0303819f;
const f64 PCPathArcSineCoefficient7 = -0.0446429f;
const f64 PCPathArcSineCoefficient5 = -0.075f;
const f64 PCPathArcSineCoefficient3 = -0.166667f;
const f64 TerrainScanMargin10 = 0.10000000149011612;
const f64 ActionSnapWeightedRandomZero = 0.0;
const f32 NuTimeBarQFontSize = 16.0f;
const f32 NuTimeBarQFontScale = 0.0625f;
const f64 NuTimeBarColumnStep = 3.0;
const f64 NuTimeBarSlotWidth = 0.02500000037252903;
const f32 PCKeyboardDefaultValue = 0.1f;
const f64 BoltTargetRandomDenominator = 65535.0;
const f32 LevelConfigFarDistance = 2000000.0f;
const f32 ApplyGravityMinimumFrameTime = 0.016666668f;
const f32 PermLegalScale = 2.2f;
const f64 AIPathAngularScale = 10430.3779296875;
const f64 NuTime_RefreshRate = 60.0;
const f32 ForcePushApproachOuterSquare = 0.040000003f;
const f32 ForcePushApproachInnerSquare = 0.010000001f;
const f64 PCAtan2RadianScale = 9.58738019107841e-05;
const f64 NuTime_MicroScale = 1000000.0;
const f64 NuVpFixed16Scale = 1.0 / 65536.0;
const f64 LightingSurfaceFadeSlope = -0.33399999141693115;
const f32 NuPrimPS2YScale = -1.0f / 112.0f;
const f32 NuPrimPS2XScale = 0.003125f;
const f64 NuTimeBarLinesPerSecond = 15734.0;
const f64 NuTimeBarNegativeSlotWidth = -0.02500000037252903;
const f64 TerrainOpposingContactThreshold = -0.7f;
const f32 PCSamplerBiasMinimum = -16.0f;
