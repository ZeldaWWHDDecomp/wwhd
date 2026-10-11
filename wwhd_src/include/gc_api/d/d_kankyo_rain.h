/* Weather geometry API [v d_ky_thunder]. */
#pragma once
#include "bindings.h"
/* 02563F64 */
inline void dKyr_get_vectle_calc(const cXyz* eye, const cXyz* center, cXyz* dir) { gabi::call(0x02563F64, eye, center, dir); }
