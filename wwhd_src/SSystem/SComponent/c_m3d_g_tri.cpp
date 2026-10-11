/**
 * c_m3d_g_tri.cpp
 * Component - triangle geometry
 */
#include "SSystem/SComponent/c_m3d_g_tri.h"

enum : u32 { triangleVtable = 0x10003574 }; // HD: cM3dGTri vtable.

// HD: explicit base construction and runtime initialization of the vertex storage.
static void initializeTriangle(cM3dGTri* triangle) {
    cM3dGPla_construct(triangle);
    triangle->__vtbl = triangleVtable;
    initializeVertexStorage(&triangle->mA, sizeof(cXyz));
    initializeVertexStorage(&triangle->mB, sizeof(cXyz));
    initializeVertexStorage(&triangle->mC, sizeof(cXyz));
}

/* 02019040 */
// HD: GHS constructors allocate when called with NULL.
static cM3dGTri* cM3dGTri__ct(cM3dGTri* i_this) {
    if (i_this == NULL) {
        i_this = (cM3dGTri*)operator_new(sizeof(cM3dGTri));
    }
    if (i_this != NULL) {
        initializeTriangle(i_this);
    }
    return i_this;
}

/* 020190B8 */
static cM3dGTri* cM3dGTri__ct_pos(cM3dGTri* i_this, const cXyz* a, const cXyz* b, const cXyz* c) {
    if (i_this == NULL) {
        i_this = (cM3dGTri*)operator_new(sizeof(cM3dGTri));
    }
    if (i_this != NULL) {
        initializeTriangle(i_this);
        i_this->mA = *a;
        i_this->mB = *b;
        i_this->mC = *c;
        cM3d_CalcPla(&i_this->mA, &i_this->mB, &i_this->mC, &i_this->mNormal, &i_this->mD);
    }
    return i_this;
}

/* 020191B4 */
void cM3dGTri::Up(f32 distance) {
    gabi::Local<cXyz> displacement;
    PSVECScale(&mNormal, displacement.get(), distance);
    PSVECAdd(&mA, displacement.get(), &mA);
    PSVECAdd(&mB, displacement.get(), &mB);
    PSVECAdd(&mC, displacement.get(), &mC);
    mD = (f32)mD - distance;
}

/* 0201923C */
// HD: preserve the full integer result instead of narrowing it to GameCube bool.
u32 cM3dGTri::cross(const cM3dGCyl* other, cXyz* out) {
    return cM3d_Cross_CylTri(other, this, out);
}

/* 0201924C */
void cM3dGTri::setPos(const cXyz* a, const cXyz* b, const cXyz* c) {
    mA = *a;
    mB = *b;
    mC = *c;
    cM3d_CalcPla(&mA, &mB, &mC, &mNormal, &mD);
}

/* 020192AC */
void cM3dGTri::setBg(const cXyz* a, const cXyz* b, const cXyz* c, const cM3dGPla* plane) {
    mA = *a;
    mB = *b;
    mC = *c;
    cM3dGPla_Set(this, plane);
}

/* 020192FC */
static void __sinit_c_m3d_g_tri_cpp() {
    sinit_header_statics_rodata(0x101FF980, 0x1018C9D0, 0x101FF98C, 0x1000358C);
}

/* 02019390 */
// HD: separate, unidentified header-static-only translation unit hosted in this file.
static void initializeAdjacentHeaderStatics() {
    sinit_header_statics_rodata(0x101FF99C, 0x1018C9F4, 0x101FF9A8, 0x100035A4);
}

/* WWHD glue (tools/verify/autoglue.py): this unit's external calls
ext 020189A0 void cM3dGPla_construct(cM3dGPla*)
ext 02018B84 void cM3dGPla_Set(cM3dGPla*, const cM3dGPla*)
ext 028F521C void initializeVertexStorage(cXyz*, u32)
ext 02010D18 void cM3d_CalcPla(const cXyz*, const cXyz*, const cXyz*, cXyz*, be<f32>*)
ext 020156D0 u32 cM3d_Cross_CylTri(const cM3dGCyl*, const cM3dGTri*, cXyz*)
*/
