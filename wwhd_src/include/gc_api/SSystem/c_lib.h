/* c_lib (SSystem/SComponent/c_lib.h), GameCube names -> WWHD.
 * Already in bindings.h: cLib_addCalc2, cLib_addCalc0, cLib_addCalcAngleS2, cLib_targetAngleY,
 * cLib_distanceAngleS (HD: s32 result), cLib_chaseF, cLib_chaseAngleS, cLib_addCalcAngleS. */
#pragma once
#include "bindings.h"

/* 0200EF78 [v nz] */
inline f32 cLib_addCalcPosXZ(cXyz* pos, const cXyz& target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200EF78, pos, &target, scale, maxStep, minStep);
}
/* 0200F164 [v nz] */
inline void cLib_addCalcPos2(cXyz* pos, const cXyz& target, f32 scale, f32 maxStep) {
    gabi::call(0x0200F164, pos, &target, scale, maxStep);
}
/* GameCube inline: fabsf(f) < G_CM3D_F_ABS_MIN (HD constant 8e-11) [v nz] */
inline bool cLib_IsZero(f32 f) { return std::fabs(f) < 7.999999968033578e-11f; }
/* 02055B64 cLib_calcTimer<s16> (HD out of line) [v nz] */
inline s16 cLib_calcTimer(be<s16>* timer) { return gabi::call<s16>(0x02055B64, timer); }
template <class T> inline T cLib_maxLimit(T v, T max) { return v > max ? max : v; }
