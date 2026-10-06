#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct cM3dGCps {
  cXyz start, end;
  be<u32> vtable;
  be<f32> radius;
};
struct cM3dGCpsS { cXyz start, end; be<f32> radius; };
WWHD_SIZE(cM3dGCps, 0x20);
WWHD_SIZE(cM3dGCpsS, 0x1C);
WWHD_OFFSET(cM3dGCps, vtable, 0x18);
WWHD_OFFSET(cM3dGCps, radius, 0x1C);
