#include "saga.h"

void __cdecl APITransparentInit(void) {
    APITrans_Mtl[0] = NuMtlCreate3D(1);
    APITrans_Mtl[0]->attribs.unknown_1_1_2 = 1;
    APITrans_Mtl[0]->attribs.unknown_1_4_8 = 1;
    APITrans_Mtl[0]->diffuse_r = 0.0f;
    APITrans_Mtl[0]->diffuse_g = 0.0f;
    APITrans_Mtl[0]->diffuse_b = 0.0f;
    APITrans_Mtl[0]->opacity = 1.0f;
    APITrans_Mtl[0]->attribs.alpha_mode = 2;
    APITrans_Mtl[0]->attribs.z_mode = 0;
    APITrans_Mtl[0]->attribs.unknown_20_31 &= 0x807;
    APITrans_Mtl[0]->unknown_076 = 0;
    NuMtlUpdate(APITrans_Mtl[0]);

    APITrans_Mtl[1] = NuMtlCreate3D(1);
    APITrans_Mtl[1]->attribs.unknown_1_1_2 = 1;
    APITrans_Mtl[1]->attribs.unknown_1_4_8 = 1;
    APITrans_Mtl[1]->diffuse_r = 0.0f;
    APITrans_Mtl[1]->diffuse_g = 0.0f;
    APITrans_Mtl[1]->diffuse_b = 0.0f;
    APITrans_Mtl[1]->opacity = 1.0f;
    APITrans_Mtl[1]->attribs.alpha_mode = 2;
    APITrans_Mtl[1]->attribs.z_mode = 1;
    APITrans_Mtl[1]->attribs.unknown_20_31 &= 0x807;
    APITrans_Mtl[1]->unknown_076 = 0;
    NuMtlUpdate(APITrans_Mtl[1]);
}
