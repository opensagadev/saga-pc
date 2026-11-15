#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PortalFlagRoomVisible(NUGSCNROOM *, NUGSCN *);
    void PortalClipRoom(NUGSCN *, NUGSCNROOM *, NUPORTAL_FRUSTUM *);
    void NuKeyboardWindowStateLeaf(void);
    void NuPrimReleaseResources(void);
#ifdef __cplusplus
}
#endif
