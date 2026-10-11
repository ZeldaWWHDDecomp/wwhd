// Standard process creation request.
#pragma once
#include "gc_api/f_pc/f_pc.h"
/* 025E14A8 [v d_a_bmd] */
inline u32 fpcSCtRq_Request(layer_class* layer, s16 name, u32 a, u32 b, void* params) { return gabi::call<u32>(0x025E14A8, layer, name, a, b, params); }
