// GameCube API bindings for verified HD callers.
#pragma once
#include "bindings.h"
/* 025BEBB8 [v d_a_bmd]: HD preserves the full type register. */
inline void dSnap_RegistFig(s32 type, fopAc_ac_c* actor, cXyz* pos, s16 angle, f32 x, f32 y, f32 z) { gabi::call(0x025BEBB8, type, actor, pos, angle, x, y, z); }
