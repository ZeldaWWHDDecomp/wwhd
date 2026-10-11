/* m_Do_mtx (mDoMtx_stack_c: bindings.h struct, GameCube-named members there). */
#pragma once
#include "bindings.h"

/* MTXCopy = PSMTXCopy (028E90D4) */
inline void MTXCopy(const Mtx34* src, Mtx34* dst) { gabi::call(0x028E90D4, src, dst); }
/* mDoMtx_quatSlerp = C_QUATSlerp (028E9BC0) [v lpalm] */
inline void mDoMtx_quatSlerp(const Quaternion* a, const Quaternion* b, Quaternion* ab, f32 t) { gabi::call(0x028E9BC0, a, b, ab, t); }
