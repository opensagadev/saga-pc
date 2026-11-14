#ifndef SAGA_NU2API_NU3D_ANDROID_NUPTL_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUPTL_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edgra.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edgra.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edgra.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrSetParticleRotation(NUMTX *);
    void NuDebrisRendererFlushBuffers(void);
    i32 NuDebrisRendererNextBuffer(void);
    void NuRndrParticleSetRepeat(NUVEC *);
    void BuildDebrisVerts(DEBRIS_PART_HEADER_PC *, DEBRIS_PARTICLE_CHUNK_PC *, NUMTL *, f32, NUMTX *, i32, f32, f32,
                          f32, f32);
    void NuRndrParticleGroup(DEBRIS_PARTICLE_CHUNK_PC *, DEBRIS_PART_HEADER_PC *, NUMTL *, f32, NUMTX *, i32, f32, f32,
                             f32, f32);
    void AddParticleGroupToDisplayList(NATIVE_PARTICLE_PACKET_PC *);

    void GenericDebinfoDmaTypeUpdate(DEBRIS_INFO_PC *effect);
    DEBRIS_PARTICLE_CHUNK_PC *CreateDmaParticleSet(void *, i32 *);
    DEBRIS_PARTICLE_CHUNK_PC *CreateDmaParticleSetGlass(void *, i32 *);
    DEBRIS_PART_HEADER_PC *CreateDmaPartEffectList(void *, i32 *);
    void LinkDmaParticalSets(DEBRIS_PARTICLE_CHUNK_PC **, i32);
    void NuInitDebrisRenderer(VARIPTR *, VARIPTR);
#ifdef __cplusplus
}
#endif

#endif
