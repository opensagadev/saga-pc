#ifndef SAGA_LEGOAPI_RENDER_FX_H
#define SAGA_LEGOAPI_RENDER_FX_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DebrisPosOrientationMtx(i32, NUMTX *);
    void AddVariableShotDebrisEffectTimed1(i32, NUVEC *, i32, f32, i16, i16, NUMTX *);
    void AddVariableShotDebrisEffectTimed1(i32, NUVEC *, i32, f32, i16, i16, NUMTX *);
    i32 LookupDebrisEffectPageOnly(char *name, i8 page);
    void DebrisTypeStatusAlwaysOff(i32 type);
    void DebrisTypeStatusAlwaysOn(i32 type);
    i32 ParticlesPerFrame(f32, f32);
    void KillPart(PART_PC *, i32);
    i32 ParticlesPerSecond(f32, f32);
    i32 FindGameDebris(APIDEBRISSYS_PC *, char *);
    void AddDebrisEffect(i32 *, i32, f32, f32, f32);
    void AddFiniteShotDebrisEffect2(i32 *, i32, NUVEC *, NUVEC *, NUVEC *, i32);
    void AddFiniteShotDebrisEffect(i32 *, i32, NUVEC *, i32);
    void AddVariableShotDebrisEffectMtx3(i32, NUVEC *, NUVEC *, i32, NUMTX *, NUMTX *);
    void AddVariableShotDebrisEffect(i32, NUVEC *, i32, i16, i16);
    i32 AddGameDebris(APIDEBRISSYS_PC *, i32, NUVEC *);
    i32 LookupDebrisEffectPageIgnore(char *, i8, i32);
    i32 LookupDebrisEffectPage(char *, i8);
    i32 PARTLookupType(char *name);

    i32 AddGameDebrisMomentum(APIDEBRISSYS_PC *, i32, NUVEC *, NUVEC *, NUVEC *);
    i32 AddGameDebrisXYZ(APIDEBRISSYS_PC *, i32, f32, f32, f32);
    i32 AddGameDebrisRot(APIDEBRISSYS_PC *, i32, NUVEC *, i32, u16, u16);
    i32 AddGameDebrisMtx(APIDEBRISSYS_PC *, i32, NUVEC *, i32, NUMTX *);
    i32 AddGameDebrisMom(APIDEBRISSYS_PC *, i32, NUVEC *, i32, NUVEC *);
    i32 AddFiniteShotPART(i32, NUVEC *, i32);
    void DebFreeInstantly(i32 *);
    i32 PARTLookupTypePageOnly(char *, i32);
    i32 LookupDebrisEffect(char *);

    APIDEBRISSYS_PC *InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i32);
#ifdef __cplusplus
}
#endif

#endif
