#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"

struct debris_info_s;

#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrSetParticleRotation(NUMTX *);
    void NuDebrisRendererFlushBuffers(void);
    i32 NuDebrisRendererNextBuffer(void);
    void NuRndrParticleSetRepeat(NUVEC *);
    void BuildDebrisVerts(DEBRIS_PART_HEADER *, DEBRIS_PARTICLE_CHUNK *, NUMTL *, f32, NUMTX *, i32, f32, f32, f32, f32);
    void NuRndrParticleGroup(DEBRIS_PARTICLE_CHUNK *, DEBRIS_PART_HEADER *, NUMTL *, f32, NUMTX *, i32, f32, f32, f32, f32);
    void AddParticleGroupToDisplayList(NATIVE_PARTICLE_PACKET *);

    void GenericDebinfoDmaTypeUpdate(struct debris_info_s *effect);
    DEBRIS_PARTICLE_CHUNK *CreateDmaParticleSet(void *, i32 *);
    DEBRIS_PARTICLE_CHUNK *CreateDmaParticleSetGlass(void *, i32 *);
    DEBRIS_PART_HEADER *CreateDmaPartEffectList(void *, i32 *);
    void LinkDmaParticalSets(DEBRIS_PARTICLE_CHUNK **, i32);
    void NuInitDebrisRenderer(VARIPTR *, VARIPTR);
#ifdef __cplusplus
}
#endif
