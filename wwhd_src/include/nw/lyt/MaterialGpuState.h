/* Color, blending and alpha-test material state. */
#pragma once
#include "nw/lyt/MaterialGpuStorage.h"
void materialStateCopy(void*,const void*,u32);
void materialColorControl(u32,u32,u32,u32);
void materialBlendControl(u32,u32,u32,u32,u32,u32,u32,u32);
void materialAlphaTest(u32,u32,f32);
