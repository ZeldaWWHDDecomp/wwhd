// Collision-object lifetime calls.
#pragma once
#include "bindings.h"
/* 025166F0 / 02515AE8 [v d_a_bmd] */
inline void __ct(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void __dt(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
