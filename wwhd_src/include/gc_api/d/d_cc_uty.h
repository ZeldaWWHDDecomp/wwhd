// GameCube API bindings for verified HD callers.
#pragma once
#include "d/d_cc_uty.h"
/* 025192A8 [v d_a_bmd] */
inline fopAc_ac_c* cc_at_check(fopAc_ac_c* actor, CcAtInfo* info) { return gabi::call<fopAc_ac_c*>(0x025192A8, actor, info); }
