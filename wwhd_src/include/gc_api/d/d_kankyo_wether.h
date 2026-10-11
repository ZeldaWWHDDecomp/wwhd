/* d_kankyo_wether (GameCube names) -> WWHD: wind. */
#pragma once
#include "bindings.h"

/* 0257DAA8 / 02578348 [v lpalm] */
inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
inline f32 dKyw_get_wind_pow() { return gabi::call<f32>(0x02578348); }

/* Volumetric-cloud environment process [v d_kyeff2]. */
/* 0257D174 */
inline void dKyw_wether_draw2() { gabi::call<void>(0x0257D174); }
/* 0257C5B0: HD inlines the forwarding wrapper and calls wether_move_vrkumo directly. */
inline void dKyw_wether_move_draw2() { gabi::call<void>(0x0257C5B0); }
/* 025782D0 */
inline void dKyw_wether_delete2() { gabi::call<void>(0x025782D0); }
/* 02577E00 */
inline void dKyw_wether_init2() { gabi::call<void>(0x02577E00); }

#include "d/d_kankyo.h"
/* 0257DC90 / 0257D1B8 [v d_a_bmd] */
inline void dKyw_pntwind_set(dKankyo::WIND_INFLUENCE* wind) { gabi::call(0x0257DC90, wind); }
inline void dKyw_pntwind_cut(dKankyo::WIND_INFLUENCE* wind) { gabi::call(0x0257D1B8, wind); }
