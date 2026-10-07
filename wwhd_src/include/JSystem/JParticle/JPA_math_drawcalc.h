#pragma once
#include "wwhd.h"
struct JPADrawMath_l {
  u8 opaque000[0xab];
  be<u8> emitterVisitorCount;
  u8 opaque0ac;
  be<u8> particleVisitorCount;
  u8 opaque0ae;
  be<u8> childVisitorCount;
  u8 opaque0b0[4];
  u8 context[0x30];
  be<f32> scale;
  be<u32> primaryColor, environmentColor;
};
WWHD_OFFSET(JPADrawMath_l, emitterVisitorCount, 0xab);
WWHD_OFFSET(JPADrawMath_l, particleVisitorCount, 0xad);
WWHD_OFFSET(JPADrawMath_l, childVisitorCount, 0xaf);
WWHD_OFFSET(JPADrawMath_l, context, 0xb4);
WWHD_OFFSET(JPADrawMath_l, scale, 0xe4);
