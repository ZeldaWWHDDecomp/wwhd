/* JPAMath (GameCube names): the particle matrix helpers actors call. Opt-in (raw units define
 * static copies). */
#pragma once
#include "bindings.h"

/* 028249B0 JPASetRMtxTVecfromMtx(const Mtx src, Mtx r, JGeometry::TVec3<f32>* t) [v: bk, gnd, ... (local copies)] */
inline void JPASetRMtxTVecfromMtx(const Mtx34* src, Mtx34* r, cXyz* t) { gabi::call(0x028249B0, src, r, t); }
/* 028245AC JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, Mtx out) [v: local copies] */
inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, Mtx34* out) { gabi::call(0x028245AC, x, y, z, out); }
