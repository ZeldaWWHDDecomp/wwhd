/* Triangle geometry: GameCube names, WWHD layout.
 * The cross method omits const for verifier member-pointer support. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_pla.h"

class cM3dGTri : public cM3dGPla {
public:
    void Up(f32 distance);
    u32 cross(const cM3dGCyl* other, cXyz* out);
    void setPos(const cXyz* a, const cXyz* b, const cXyz* c);
    void setBg(const cXyz* a, const cXyz* b, const cXyz* c, const cM3dGPla* plane);

    /* 0x14 */ cXyz mA;
    /* 0x20 */ cXyz mB;
    /* 0x2C */ cXyz mC;
};
WWHD_OFFSET(cM3dGTri, mA, 0x14);
WWHD_OFFSET(cM3dGTri, mB, 0x20);
WWHD_OFFSET(cM3dGTri, mC, 0x2C);
WWHD_SIZE(cM3dGTri, 0x38);
void cM3dGPla_construct(cM3dGPla* plane);
void cM3dGPla_Set(cM3dGPla* plane, const cM3dGPla* src);
void initializeVertexStorage(cXyz* vertex, u32 size);
void cM3d_CalcPla(const cXyz* a, const cXyz* b, const cXyz* c, cXyz* normal, be<f32>* distance);
u32 cM3d_Cross_CylTri(const cM3dGCyl* cylinder, const cM3dGTri* triangle, cXyz* out);
