/* 3D collision and projection helpers, using tww geometry layouts.
 * HD: preserve the verified full-register return types and explicit output pointers. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_aab.h"
#include "SSystem/SComponent/c_m3d_g_lin.h"
#include "SSystem/SComponent/c_m3d_g_tri.h"
#include "bindings.h"

// HD: preserve Cafe entry-point argument and return widths.
void cM3dSDK_PSVECScale(const void* in, void* out, f32 scale);
void cM3dSDK_PSVECAdd(const void* a, const void* b, void* out);
void cM3dSDK_PSVECSubtract(const void* a, const void* b, void* out);
void cM3dSDK_PSVECCrossProduct(const void* a, const void* b, void* out);
f32 cM3dSDK_PSVECDotProduct(const void* a, const void* b);
f32 cM3dSDK_PSVECMag(const void* v);
f32 cM3dSDK_PSVECSquareMag(const void* v);
f32 cM3dSDK_PSVECSquareDistance(const void* a, const void* b);
f32 cM3dSDK_PSVECDistance(const void* a, const void* b);
void cM3dSDK_PSVECNormalize(const void* in, void* out);
void cM3dSDK_PSMTXIdentity(void* matrix);
void cM3dSDK_PSMTXRotAxisRad(void* matrix, const void* axis, f64 angle);
f32 cM3dSDK_sqrtfHD(f32 value);
f64 cM3dSDK_acosHD(f64 value);

// Calls within this translation unit retain guest dispatch through autoglue.
void cM3d_InDivPos1(void* a, void* b, void* out, f32 scale);
void cM3d_InDivPos2(void* a, void* b, void* out, f32 scale);
f32 cM3d_Len2dSq(f32 ax, f32 ay, f32 bx, f32 by);
s32 cM3d_Len3dSqPntAndSegLine(void* line, void* point, void* out, void* distance);
f32 cM3d_SignedLenPlaAndPos(void* plane, void* point);
f32 cM3d_VectorProduct2d(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy);
s32 cM3d_PlaneSegmentPosition(void* start, void* end, void* out, f32 startDistance,
                              f32 endDistance);
s32 cM3d_Cross_LinPla(void* line, void* plane, void* out, u32 front, u32 back);
s32 cM3d_Cross_SphPnt(void* sphere, void* point);
s32 cM3d_Cross_CylPnt(void* cylinder, void* point);
s32 cM3d_Cross_CylPntPnt(void* cylinder, void* start, void* end, void* count, void* out);
void cM3d_PlaneCrossLineProcWork(void* first, void* second, f32 a, f32 b, f32 c, f32 d,
                                 f32 denominator, f32 f, f32 g);
s32 cM3d_CrossX_LinTri_proc(void* triangle, void* point);
s32 cM3d_CrossY_Tri(void* triangle, void* point);
s32 cM3d_CrossY_TriPosition(void* triangle, void* point, void* height);
s32 cM3d_CrossY_TriRange(void* triangle, void* point, void* range, void* height);
s32 cM3d_CrossZ_LinTri_proc(void* triangle, void* point);
s32 cM3d_Check_LinLin(void* lineA, void* lineB, void* positionA, void* positionB);
void cM3d_CalcSphVsTriCrossPoint(void* sphere, void* triangle, void* out);
s32 cM3d_SphereSphereMetrics(void* a, void* b, void* distance, void* penetration);
s32 cM3d_CylinderSpherePosition(void* cylinder, void* sphere, void* out, void* penetration);
s32 cM3d_Cross_LinSph_CrossPos(void* sphere, void* line, void* first, void* second);
s32 cM3d_Cross_LinSph(void* line, void* sphere, void* out);
s32 cM3d_2PlaneCrossLine(void* a, void* b, void* line);
void cM3d_Cross_CpsSph_CrossPos(void* capsule, void* sphere, void* nearest, void* out);
s32 cM3d_Cross_LinTri(void* line, void* triangle, void* out, u32 front, u32 back);
s32 cM3d_Cross_CylLin(void* cylinder, void* line, void* firstOut, void* secondOut);
s32 cM3d_Cross_SphTri(void* sphere, void* triangle, void* out);
void cM3dGLin_SetStartEndHD(void* line, const void* start, const void* end);
void cM3dGLin_CalcPosHD(void* line, void* out, f32 scale);
void* cM3dGLin_constructHD(void* line, const void* start, const void* end);
void* cM3dGSph_constructHD(void* sphere);
void cM3dGSph_SetCHD(void* sphere, const void* center);
void cM3dGSph_SetRHD(void* sphere, f32 radius);
void* cM3dGPla_constructHD(void* plane);
s32 cM_atan2sHD(f32 y, f32 x);
void cM3d_assert(const char* file, u32 line, const char* condition);
void registerGeometryDestructor(void* descriptor);
void initializeGeometryByte(void* value);
void initializeGeometryFlag(void* value);

// The capsule header imports a cylinder definition that clashes with bindings.h.
// Keep this caller view private; its base and radius match the shared capsule layout.
struct cM3dCapsuleView : cM3dGLin {
    /* 0x1C */ be<f32> mRadius;
};
WWHD_OFFSET(cM3dCapsuleView, mRadius, 0x1C);
WWHD_SIZE(cM3dCapsuleView, 0x20);

// Geometry layouts supplied by the collision bindings.
WWHD_OFFSET(cM3dGSph, mCenter, 0x00);
WWHD_OFFSET(cM3dGSph, mRadius, 0x0C);
WWHD_SIZE(cM3dGSph, 0x14);
WWHD_OFFSET(cM3dGCyl, mCenter, 0x00);
WWHD_OFFSET(cM3dGCyl, mRadius, 0x0C);
WWHD_OFFSET(cM3dGCyl, mHeight, 0x10);
WWHD_SIZE(cM3dGCyl, 0x18);

f32 cM3d_lineVsPosSuisenCross(void* line, void* point, void* out);

// Verified out-of-line functions; pointer and register widths retain the HD ABI.
void cM3d_InDivPos1(void* a, void* b, void* out, f32 scale);
void cM3d_InDivPos2(void* a, void* b, void* out, f32 scale);
f32 cM3d_Len2dSq(f32 ax, f32 ay, f32 bx, f32 by);
s32 cM3d_Len2dSqPntAndSegLine(void* ox, void* oy, void* distance, f32 px, f32 py, f32 ax, f32 ay,
                              f32 bx, f32 by);
s32 cM3d_Len3dSqPntAndSegLine(void* line, void* point, void* out, void* distance);
f32 cM3d_SignedLenPlaAndPos(void* plane, void* point);
f32 cM3d_VectorProduct2d(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy);
void cM3d_CalcPlaHD(void* a, void* b, void* c, void* normal, void* distance);
s32 cM3d_Cross_AabCyl(void* aab, void* cylinder);
s32 cM3d_PlaneSegmentPosition(void* start, void* end, void* out, f32 startDistance,
                              f32 endDistance);
s32 cM3d_Cross_LinPla(void* line, void* plane, void* out, u32 front, u32 back);
s32 cM3d_Cross_AabSph(void* aab, void* sphere);
s32 cM3d_Cross_SphPnt(void* sphere, void* point);
s32 cM3d_Cross_CylPnt(void* cylinder, void* point);
s32 cM3d_Cross_CylPntPnt(void* cylinder, void* start, void* end, void* count, void* out);
void cM3d_CalcVecAngleHD(void* vector, void* xAngle, void* zAngle);
void cM3d_CalcVecZAngle(void* vector, void* angles);
void cM3d_PlaneCrossLineProcWork(void* first, void* second, f32 a, f32 b, f32 c, f32 d,
                                 f32 denominator, f32 f, f32 g);
void cM3d_CrawVec(void* normal, void* vector, void* out);
void c_m3d_static_init();
s32 cM3d_InclusionCheckPosIn3PosBox2d(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 px,
                                      f32 py);
s32 cM3d_CrossX_LinTri_proc(void* triangle, void* point);
s32 cM3d_CrossY_Tri(void* triangle, void* point);
s32 cM3d_CrossY_TriVertices(void* a, void* b, void* c, void* plane, void* point);
s32 cM3d_CrossY_Tri_Front(void* a, void* b, void* c, void* point);
s32 cM3d_CrossY_TriPosition(void* triangle, void* point, void* height);
s32 cM3d_CrossY_TriRange(void* triangle, void* point, void* range, void* height);
s32 cM3d_CrossZ_LinTri_proc(void* triangle, void* point);
s32 cM3d_Check_LinLin(void* lineA, void* lineB, void* positionA, void* positionB);
s32 cM3d_Cross_CylSphDepthHD(void* cylinder, void* sphere, void* penetration);
s32 cM3d_Cross_SphSphDepth(void* a, void* b, void* penetration);
s32 cM3d_Cross_SphSphPosition(void* a, void* b, void* out);
void cM3d_CalcSphVsTriCrossPoint(void* sphere, void* triangle, void* out);
s32 cM3d_Cross_CylCylPosition(void* a, void* b, void* out);
s32 cM3d_SphereSphereMetrics(void* a, void* b, void* distance, void* penetration);
s32 cM3d_CylinderSpherePosition(void* cylinder, void* sphere, void* out, void* penetration);
s32 cM3d_Cross_CylCylDepth(void* a, void* b, void* penetration);
s32 cM3d_Cross_LinSph_CrossPos(void* sphere, void* line, void* first, void* second);
s32 cM3d_Cross_LinSph(void* line, void* sphere, void* out);
s32 cM3d_NearPos_Cps(void* capsule, void* point, void* out);
s32 cM3d_3PlaneCrossPos(void* a, void* b, void* c, void* out);
s32 cM3d_2PlaneLinePosNearPos(void* a, void* b, void* point, void* out);
f32 cM3d_lineVsPosSuisenCross(void* line, void* point, void* out);
f32 cM3d_lineVsPosSuisenCrossVertices(void* start, void* end, void* point, void* out);
s32 cM3d_2PlaneCrossLine(void* a, void* b, void* line);
s32 cM3d_UpMtx_Base(void* up, void* vector, void* matrix);
void cM3d_Cross_CpsSph_CrossPos(void* capsule, void* sphere, void* nearest, void* out);
s32 cM3d_Cross_CpsSph(void* capsule, void* sphere, void* out);
s32 cM3d_Cross_TriTri(void* a, void* b, void* out);
s32 cM3d_Cross_CpsCps(void* a, void* b, void* out);
s32 cM3d_Cross_CylTriHD(void* cylinder, void* triangle, void* out);
s32 cM3d_Cross_LinTri(void* line, void* triangle, void* out, u32 front, u32 back);
s32 cM3d_Cross_CpsTri(void* capsule, void* triangle, void* out);
s32 cM3d_Cross_CylLin(void* cylinder, void* line, void* firstOut, void* secondOut);
s32 cM3d_Cross_CpsCyl(void* capsule, void* cylinder, void* out);
s32 cM3d_Cross_SphTri(void* sphere, void* triangle, void* out);
s32 cM3d_Cross_MinMaxBoxLine(void* minimum, void* maximum, void* start, void* end);
void initializeGeometryHeaderStatics();
