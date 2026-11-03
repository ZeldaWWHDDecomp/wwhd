/* Cafe SDK matrix layout matches the GameCube API. */
// HD: PSMTX44Scale is a 4x4 scale helper with no body in the tww GameCube reference.
#pragma once
#include "sdk/mtx/vec.h"
WWHD_OFFSET(Mtx34, m, 0);
void PSMTXRotTrig(Mtx34 *, u32, f64, f64);
void __PSMTXRotAxisRadInternal(Mtx34 *, const Vec *, f64, f64);
