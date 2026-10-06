/** Blade Trap (WWHD). Derived from HD disassembly; */
#include "d/actor/d_a_obj_trap.h"
#include <cmath>
namespace Trap {
template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *sub(daObjTrap_c *a, u32 off) {
  return at<T>(gabi::ea(a), off);
}
u32 word(u32 p) { return *at<be<u32>>(p); }
void put(u32 p, u32 v) { *at<be<u32>>(p) = v; }
void copy(cXyz *dst, const cXyz *src) { dst->copy(*src); }
void floats(cXyz *dst, const cXyz *src) {
  f32 x = src->x, y = src->y, z = src->z;
  dst->x = x;
  dst->y = y;
  dst->z = z;
}
void plus(cXyz *a, cXyz *out, cXyz *b) {
  gabi::call<void>(0x0201AD78, a, out, b);
}
void minus(cXyz *a, cXyz *out, cXyz *b) {
  gabi::call<void>(0x0201ADE0, a, out, b);
}
void scale(cXyz *a, cXyz *out, f32 s) {
  gabi::call<void>(0x0201AE48, a, out, s);
}
void add(cXyz *a, cXyz *b, cXyz *out) {
  gabi::call<void>(0x028E8D88, a, b, out);
}
f64 lengthXZ(cXyz *v, f32 zero = 0.0f) {
  gabi::Local<cXyz> h;
  h->x = v->x;
  h->y = zero;
  h->z = v->z;
  f64 n = gabi::call<f64>(0x028E8DD0, h.get());
  return gabi::call<f64>(0x028F4384, n);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
constexpr u32 matrix = 0x1048D0CC, zero = 0x101FFBA8;
BOOL createHeap(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6AF8, BOOL, a);
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x10031880;
  (*name)[1] = 0x100316F0;
  u32 data =
      gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)), name.get(), 5);
  if (!data) {
    gabi::call<void>(0x0273AA24, at<void>(0x100317F8), 0x163,
                     at<void>(0x100317D8));
    return 0;
  }
  u32 model = gabi::call<u32>(0x025E38E0, at<void>(data), 0x80000, 0x11000222);
  a->model = at<void>(model);
  if (!model)
    return 0;
  gabi::Local<be<u32>[2]> texName;
  (*texName)[0] = 0x10031880;
  (*texName)[1] = 0x100316F0;
  u32 tex =
      gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)), texName.get(), 8);
  if (!tex) {
    gabi::call<void>(0x0273AA24, at<void>(0x100317F8), 0x16c,
                     at<void>(0x100317E8));
    return 0;
  }
  if (!gabi::call<s32>(0x025E7CE0, sub<void>(a, 0x3b0), at<void>(data),
                       at<void>(tex), 1, 1.0f, 0, 0, -1, 0, 0))
    return 0;
  gabi::Local<be<u32>[2]> bgName;
  (*bgName)[0] = 0x10031880;
  (*bgName)[1] = 0x100316F0;
  u32 bg =
      gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)), bgName.get(), 11);
  model = gabi::ea((void *)a->model);
  u32 result = gabi::call<u32>(0x024F2478, at<void>(bg), 1,
                               at<void>(model ? model + 0xc8 : 0));
  a->background = at<void>(result);
  return result != 0;
}
VERIFY(0x023A6AF8, createHeap);
BOOL heapCallback(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6C44, BOOL, a);
  return createHeap(a);
}
VERIFY(0x023A6C44, heapCallback);
void setMoveInfo(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6C48, void, a);
  u32 path = gabi::ea((void *)a->path), points = word(path + 8),
      index = a->pathPoint;
  f32 y = a->current.pos.y, z = *at<be<f32>>(points + index * 16 + 12),
      x = *at<be<f32>>(points + index * 16 + 4);
  a->origin.y = y;
  a->origin.z = z;
  a->origin.x = x;
  points = word(path + 8);
  u32 next = ((index + 1) & 1) * 16;
  z = *at<be<f32>>(points + next + 12);
  x = *at<be<f32>>(points + next + 4);
  a->target.z = z;
  a->target.x = x;
  a->target.y = y;
  gabi::Local<cXyz> delta;
  minus(&a->target, delta.get(), &a->origin);
  copy(&a->direction, delta.get());
  a->directionValid = gabi::call<s32>(0x0201B47C, &a->direction);
  copy(&a->displacement, at<cXyz>(zero));
  scale(&a->direction, delta.get(), 100.0f);
  copy(&a->searchVelocity, delta.get());
}
VERIFY(0x023A6C48, setMoveInfo);
void initMatrix(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6D3C, void, a);
  floats(at<cXyz>(gabi::ea((void *)a->model), 0xbc), &a->scale);
  gabi::call<void>(0x028E93CC, at<void>(matrix), (f32)a->current.pos.x,
                   (f32)a->current.pos.y, (f32)a->current.pos.z);
  gabi::call<void>(0x025F1B48, at<void>(matrix), (s16)a->shape_angle.x,
                   (s16)a->shape_angle.y, (s16)a->shape_angle.z);
  f32 m[12];
  for (int i = 0; i < 12; ++i)
    m[i] = *at<be<f32>>(matrix + i * 4);
  u32 model = gabi::ea((void *)a->model);
  for (int i = 0; i < 12; ++i)
    *at<be<f32>>(model + 0xc8 + i * 4) = m[i];
  gabi::call<void>(0x027F4D5C, (void *)a->model);
}
VERIFY(0x023A6D3C, initMatrix);
void setCollisionPosition(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6E24, void, a);
  gabi::Local<cXyz> p;
  f32 z = a->current.pos.z, x = a->current.pos.x, y = a->current.pos.y;
  p->x = x;
  p->z = z;
  p->y = y - 40.0f;
  gabi::call<void>(0x020182E0, sub<void>(a, 0x580), p.get());
}
VERIFY(0x023A6E24, setCollisionPosition);
s32 create(daObjTrap_c *a) {
  WWHD_FUNC(0x023A6E70, s32, a);
  u32 flags = *sub<be<u32>>(a, 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      gabi::call<void>(0x025D4ED0, a);
      *sub<be<u32>>(a, 0xb4) = 0x100317B8;
      gabi::call<void>(0x025E7C6C, sub<void>(a, 0x3b0));
      gabi::call<void>(0x0200BD2C, sub<void>(a, 0x42c));
      gabi::call<void>(0x02515DA0, sub<void>(a, 0x448));
      *sub<be<u32>>(a, 0x444) = 0x1004AE88;
      *sub<be<u32>>(a, 0x448) = 0x1004AEC0;
      gabi::call<void>(0x02515FB8, sub<void>(a, 0x468));
      *sub<be<u32>>(a, 0x57c) = 0x100015A8;
      *sub<be<u32>>(a, 0x578) = 0x10031708;
      gabi::call<void>(0x02018590, sub<void>(a, 0x580));
      *sub<be<u32>>(a, 0x4a4) = 0x1004B108;
      *sub<be<u32>>(a, 0x594) = 0x1004B150;
      flags = *sub<be<u32>>(a, 0x2e4);
      *sub<be<u32>>(a, 0x57c) = 0x1004B160;
    }
    *sub<be<u32>>(a, 0x2e4) = flags | 8;
  }
  s32 phase =
      gabi::call<s32>(0x02520460, sub<void>(a, 0x424), at<void>(0x10031880));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, a, at<void>(0x023A6C44), 0))
    return 5;
  u32 pathId = *sub<be<u8>>(a, 0xb3);
  a->pathId = pathId;
  if (pathId == 255)
    return 5;
  u32 path = gabi::call<u32>(0x025AAF88, pathId, (s8)*sub<be<u8>>(a, 0x326));
  a->path = at<void>(path);
  if (!path || !word(path + 8)) {
    a->pathId = 255;
    return 5;
  }
  u32 type = (*sub<be<u32>>(a, 0xb0) >> 8) & 15;
  if (type == 15) {
    a->speedType = 0;
    a->moveSpeed = 50.0f;
    a->waitDuration = *at<be<s16>>(0x10031878);
  } else {
    a->speedType = type;
    if (type >= 3) {
      gabi::call<void>(0x0273AA24, at<void>(0x10031818), 0x19c,
                       at<void>(0x1003182C));
      type = a->speedType;
    }
    u32 delayType = a->speedType;
    path = gabi::ea((void *)a->path);
    a->moveSpeed = *at<be<f32>>(0x10031888 + type * 4);
    a->waitDuration = *at<be<s16>>(0x10031878 + delayType * 2);
  }
  path = gabi::ea((void *)a->path);
  u32 point = word(path + 8);
  f32 y = a->current.pos.y, x = *at<be<f32>>(point + 4),
      z = *at<be<f32>>(point + 12);
  a->current.pos.x = x;
  a->current.pos.z = z;
  a->current.pos.y = y;
  setMoveInfo(a);
  gabi::Local<cXyz> delta;
  minus(&a->target, delta.get(), &a->origin);
  a->pathLength = lengthXZ(delta.get());
  copy(&a->savedPosition, &a->current.pos);
  initMatrix(a);
  gabi::call<void>(0x02515F14, sub<void>(a, 0x42c), 0, 255, a);
  *sub<be<u32>>(a, 0x4ac) = gabi::ea(a) + 0x42c;
  gabi::call<void>(0x02516518, sub<void>(a, 0x468), at<void>(0x101CD5A8));
  setCollisionPosition(a);
  u32 p = play();
  gabi::call<void>(0x024EEA6C, at<void>(p + 0x12a0), (void *)a->background, a);
  return 4;
}
VERIFY(0x023A6E70, create);
BOOL remove(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7184, BOOL, a);
  if (*sub<be<u32>>(a, 0xf4)) {
    u32 bg = gabi::ea((void *)a->background);
    if (bg && word(bg) < 256) {
      u32 p = play();
      gabi::call<void>(0x020087EC, at<void>(p + 0x12a0), (void *)a->background);
      a->background = nullptr;
    }
  }
  gabi::call<void>(0x025204C8, sub<void>(a, 0x424), at<void>(0x10031880));
  return 1;
}
VERIFY(0x023A7184, remove);
// Each query has its own static line-check and first-use guard.
void initializeLine(u32 line, u32 guard, u32 registration, bool wall) {
  if (word(guard))
    return;
  put(guard, 1);
  gabi::call<void>(0x02008FEC, at<void>(line));
  put(line, line + 0x58);
  put(line + 4, line + 0x64);
  put(line + 0x10, 0x10031778);
  put(line + 0x20, 0x10031788);
  put(line + 0x64, 0x10031798);
  put(line + 0x68, 1);
  put(line + 0x58, 0x100317A8);
  *at<be<u8>>(line + 0x5c) = 1;
  for (u32 i = 0x5d; i <= 0x62; ++i)
    *at<be<u8>>(line + i) = 0;
  gabi::call<void>(0x028F026C, at<void>(registration));
}
void checkWall(daObjTrap_c *a, cXyz *output) {
  WWHD_FUNC(0x023A71FC, void, a, output);
  constexpr u32 line = 0x1046C5B0;
  initializeLine(line, 0x1046C688, 0x101CD5EC, true);
  gabi::Local<cXyz> reach, height, best, side, tmp, start, end, hit, shortVec,
      sum, adjusted;
  scale(&a->direction, reach.get(), 150.0f);
  scale(at<cXyz>(0x101FFBC0), height.get(), 75.0f);
  floats(best.get(), at<cXyz>(zero));
  for (int i = 0; i < 3; ++i) {
    s16 angle = *at<be<s16>>(0x100316E8 + i * 2);
    gabi::call<void>(0x025F1884, at<void>(matrix), angle);
    gabi::call<void>(0x028E8F64, at<void>(matrix), &a->direction, side.get());
    f32 distance = *at<be<f32>>(0x100317C8 + i * 4);
    gabi::call<void>(0x028E8E64, side.get(), side.get(), distance);
    add(side.get(), height.get(), side.get());
    plus(&a->current.pos, tmp.get(), side.get());
    copy(start.get(), tmp.get());
    plus(start.get(), tmp.get(), &a->displacement);
    copy(end.get(), tmp.get());
    add(end.get(), reach.get(), end.get());
    gabi::call<void>(0x024F1AFC, at<void>(line), start.get(), end.get(), a);
    put(line + 8, *sub<be<u32>>(a, 4));
    u32 p = play();
    if (!gabi::call<s32>(0x02008860, at<void>(p + 0x12a0), at<void>(line)))
      continue;
    copy(hit.get(), at<cXyz>(line + 0x30));
    gabi::call<void>(0x028E8DAC, hit.get(), start.get(), hit.get());
    bool replace = gabi::call<s32>(0x0201AF98, best.get(), at<cXyz>(zero)) != 0;
    if (!replace) {
      f64 current = lengthXZ(best.get());
      f64 next = lengthXZ(hit.get());
      replace = current > next;
    }
    if (replace) {
      plus(hit.get(), sum.get(), &a->current.pos);
      minus(sum.get(), adjusted.get(), reach.get());
      copy(best.get(), adjusted.get());
    }
  }
  if (!output)
    output = at<cXyz>(gabi::call<u32>(0x0273AD10, 12));
  if (output)
    floats(output, best.get());
}
VERIFY(0x023A71FC, checkWall);
BOOL checkBlockTarget(daObjTrap_c *a, cXyz *position) {
  WWHD_FUNC(0x023A752C, BOOL, a, position);
  gabi::Local<cXyz> delta, side;
  minus(position, delta.get(), &a->current.pos);
  f32 dz = a->direction.z, x = delta->x, z = delta->z, dx = a->direction.x;
  f32 along = gabi::fmadds(dx, x, dz * z);
  if (along < 0.0f)
    return 0;
  if (!(along < (f32)a->pathLength + 150.0f))
    return 0;
  gabi::call<void>(0x025F1884, at<void>(matrix), 0x4000);
  gabi::call<void>(0x028E8F64, at<void>(matrix), &a->direction, side.get());
  f32 sx = side->x, sz = side->z;
  x = delta->x;
  z = delta->z;
  return std::fabs(gabi::fmadds(sx, x, sz * z)) < 225.0f;
}
VERIFY(0x023A752C, checkBlockTarget);
void checkBlock(daObjTrap_c *a, cXyz *output, cXyz *input) {
  WWHD_FUNC(0x023A760C, void, a, output, input);
  constexpr u32 line = 0x1046C61C;
  initializeLine(line, 0x1046C68C, 0x101CD5F8, false);
  gabi::Local<cXyz> reach, best, height, side, tmp, start, end, translation,
      normal, blockSum, blockScaled, blockPos, target, hit, sum, adjusted;
  scale(&a->direction, reach.get(), 150.0f);
  floats(best.get(), input);
  scale(at<cXyz>(0x101FFBC0), height.get(), 75.0f);
  for (int i = 0; i < 2; ++i) {
    s16 angle = *at<be<s16>>(0x101CD5A4 + i * 2);
    gabi::call<void>(0x025F1884, at<void>(matrix), angle);
    gabi::call<void>(0x028E8F64, at<void>(matrix), &a->direction, side.get());
    gabi::call<void>(0x028E8E64, side.get(), side.get(), 153.0f);
    add(side.get(), height.get(), side.get());
    plus(&a->current.pos, tmp.get(), side.get());
    copy(start.get(), tmp.get());
    plus(start.get(), tmp.get(), &a->displacement);
    copy(end.get(), tmp.get());
    add(end.get(), reach.get(), end.get());
    gabi::call<void>(0x024F1AFC, at<void>(line), start.get(), end.get(), a);
    put(line + 8, *sub<be<u32>>(a, 4));
    u32 p = play();
    if (!gabi::call<s32>(0x02008860, at<void>(p + 0x12a0), at<void>(line)))
      continue;
    p = play();
    u32 block = gabi::call<u32>(0x02008438, at<void>(p + 0x12a0),
                                (u16)*at<be<u16>>(line + 0x16));
    if (!block)
      continue;
    if (!gabi::call<s32>(0x025D4604, at<void>(block)))
      continue;
    if (*at<be<s16>>(block + 8) != 0x2b)
      continue;
    if (word(block + 0x41c) != 1)
      continue;
    gabi::call<void>(0x025F23EC);
    gabi::call<void>(0x025F1884, at<void>(matrix),
                     (s16)*at<be<s16>>(block + 0x2fa));
    s32 bx = word(block + 0x740), bz = word(block + 0x744);
    gabi::call<void>(0x025F24E0, (f32)bx, 0.0f, (f32)bz);
    translation->z = *at<be<f32>>(matrix + 0x2c);
    translation->x = *at<be<f32>>(matrix + 0xc);
    translation->y = *at<be<f32>>(matrix + 0x1c);
    u32 direction = word(block + 0x74c);
    s16 rotation = *at<be<s16>>(block + 0x2fa),
        addition = *at<be<s16>>(0x1004BCE8 + direction * 2);
    gabi::call<void>(0x025F1884, at<void>(matrix), (s16)(rotation + addition));
    gabi::call<void>(0x028E9044, at<void>(matrix), at<cXyz>(0x101FFBCC),
                     normal.get());
    gabi::call<void>(0x025F2468);
    plus(translation.get(), blockSum.get(), normal.get());
    scale(blockSum.get(), blockScaled.get(), 75.0f);
    plus(blockScaled.get(), blockPos.get(), at<cXyz>(block + 0x2ec));
    copy(target.get(), blockPos.get());
    if (!checkBlockTarget(a, target.get()))
      continue;
    copy(hit.get(), at<cXyz>(line + 0x30));
    gabi::call<void>(0x028E8DAC, hit.get(), start.get(), hit.get());
    bool replace = gabi::call<s32>(0x0201AF98, best.get(), at<cXyz>(zero)) != 0;
    if (!replace) {
      f64 current = lengthXZ(best.get());
      f64 next = lengthXZ(hit.get());
      replace = current > next;
    }
    if (replace) {
      plus(hit.get(), sum.get(), &a->current.pos);
      minus(sum.get(), adjusted.get(), reach.get());
      copy(best.get(), adjusted.get());
    }
  }
  if (!output)
    output = at<cXyz>(gabi::call<u32>(0x0273AD10, 12));
  if (output)
    floats(output, best.get());
}
VERIFY(0x023A760C, checkBlock);
void vibrate(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7AD8, void, a);
  u32 index = ((u32)a->vibrationTimer * 0x5555) & 65535;
  index = (index >> 3) * 8;
  f32 sine = *at<be<f32>>(0x104A44F8 + index);
  a->shape_angle.x = (s16)gabi::ftoi(288.0f * sine);
}
VERIFY(0x023A7AD8, vibrate);
void bound(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7B20, void, a);
  gabi::Local<cXyz> offset;
  scale(&a->direction, offset.get(), -1.0f);
  gabi::call<void>(0x028E8DAC, &a->savedPosition, &a->bounceOffset,
                   &a->savedPosition);
  u32 phase = ((u32)a->bounceTimer << 14) & 0xc000;
  f32 amplitude = a->bounceAmplitude, sine = *at<be<f32>>(0x104A44F8 + phase);
  f32 step = std::fabs((f32)(s16)gabi::ftoi(amplitude * sine));
  gabi::call<void>(0x028E8E64, offset.get(), offset.get(), step);
  add(&a->savedPosition, offset.get(), &a->savedPosition);
  copy(&a->bounceOffset, offset.get());
  gabi::call<void>(0x0200ECD4, &a->bounceAmplitude, 0.0f, 0.17f, 35.0f, 1.0f);
}
VERIFY(0x023A7B20, bound);
void setVibrationMode(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7C24, void, a);
  a->savedAngle = (s16)a->shape_angle.x;
  a->bounceTimer = 16;
  a->vibrationTimer = 6;
  a->bounceAmplitude = 35.0f;
  copy(&a->bounceOffset, at<cXyz>(zero));
  a->mode = 2;
  gabi::Local<cXyz> scaled, position, direction;
  scale(&a->direction, scaled.get(), 150.0f);
  copy(position.get(), scaled.get());
  add(position.get(), &a->current.pos, position.get());
  scale(&a->direction, scaled.get(), -1.0f);
  copy(direction.get(), scaled.get());
  position->y = (f32)position->y + 50.0f;
  gabi::call<void>(0x02312D6C, position.get(), direction.get());
  if (a->vibrationTimer) {
    vibrate(a);
    a->vibrationTimer = (u16)((u16)a->vibrationTimer - 1);
  }
  if (a->bounceTimer) {
    bound(a);
    a->bounceTimer = (u16)((u16)a->bounceTimer - 1);
  }
}
VERIFY(0x023A7C24, setVibrationMode);
void setShine(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7D5C, void, a);
  a->shining = 1;
  f32 dot = gabi::call<f32>(0x028E8F44, at<cXyz>(0x101FFBB4), &a->direction);
  if (dot < 0.0f) {
    a->anmFrame = 46.0f;
    a->anmSpeed = -1.0f;
  } else {
    a->anmFrame = 35.0f;
    a->anmSpeed = 1.0f;
  }
}
VERIFY(0x023A7D5C, setShine);
BOOL circleSearch(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7DE0, BOOL, a);
  u32 p = play(), player = word(p + 0x5b2c);
  gabi::Local<cXyz> delta;
  minus(at<cXyz>(player + 0x314), delta.get(), &a->current.pos);
  f64 distance = lengthXZ(delta.get());
  if (distance > 400.0f || a->directionValid != 1)
    return 0;
  f32 dz = a->direction.z, z = delta->z, x = delta->x, dx = a->direction.x;
  return !(gabi::fmadds(dx, x, dz * z) < 0.0f);
}
VERIFY(0x023A7DE0, circleSearch);
BOOL checkArrival(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7EA4, BOOL, a);
  gabi::Local<cXyz> current, total;
  minus(&a->savedPosition, current.get(), &a->origin);
  minus(&a->target, total.get(), &a->origin);
  f64 travelled = lengthXZ(current.get()), whole = lengthXZ(total.get());
  return !(travelled < whole);
}
VERIFY(0x023A7EA4, checkArrival);
void shineMove(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7F74, void, a);
  if (a->shining == 1 && gabi::call<s32>(0x025E742C, sub<void>(a, 0x3b0)))
    a->shining = 0;
}
VERIFY(0x023A7F74, shineMove);
void impactSound(daObjTrap_c *a, u32 sound) {
  s32 reverb = gabi::call<s32>(0x02520540, (s8)*sub<be<u8>>(a, 0x326));
  gabi::call<void>(0x025E1A40, sound, &a->current.pos, 0, reverb);
}
BOOL execute(daObjTrap_c *a) {
  WWHD_FUNC(0x023A7FC0, BOOL, a);
  copy(&a->savedPosition, &a->current.pos);
  u32 mode = a->mode;
  gabi::Local<cXyz> wall, block, delta;
  if (mode == 0) {
    s32 arrived = gabi::call<s32>(0x0200F764, &a->savedPosition, &a->target,
                                  (f32)a->moveSpeed);
    minus(&a->savedPosition, delta.get(), &a->current.pos);
    copy(&a->displacement, delta.get());
    checkWall(a, delta.get());
    floats(block.get(), delta.get());
    checkBlock(a, delta.get(), block.get());
    copy(wall.get(), delta.get());
    if (gabi::call<s32>(0x0201AFD8, wall.get(), at<cXyz>(zero))) {
      copy(&a->savedPosition, wall.get());
      setVibrationMode(a);
      setShine(a);
      impactSound(a, 0x6a02);
    } else if (arrived == 1) {
      setVibrationMode(a);
      setShine(a);
      impactSound(a, 0x6a02);
    } else if (circleSearch(a) == 1) {
      copy(&a->current.pos, &a->savedPosition);
      a->mode = 1;
      goto finish;
    } else
      impactSound(a, 0x704f);
  } else if (mode == 1) {
    gabi::call<void>(0x0200EF78, &a->displacement, &a->searchVelocity, 0.06f,
                     100.0f, 1.0f);
    add(&a->savedPosition, &a->displacement, &a->savedPosition);
    checkWall(a, block.get());
    floats(delta.get(), block.get());
    checkBlock(a, block.get(), delta.get());
    copy(wall.get(), block.get());
    if (gabi::call<s32>(0x0201AFD8, wall.get(), at<cXyz>(zero))) {
      copy(&a->savedPosition, wall.get());
      setVibrationMode(a);
      setShine(a);
      impactSound(a, 0x6a02);
    } else if (checkArrival(a) == 1) {
      copy(&a->savedPosition, &a->target);
      setVibrationMode(a);
      setShine(a);
      impactSound(a, 0x6a02);
    } else
      impactSound(a, 0x704f);
  } else if (mode == 2) {
    u16 vibration = a->vibrationTimer;
    if (vibration) {
      vibrate(a);
      vibration = (u16)((u16)a->vibrationTimer - 1);
      a->vibrationTimer = vibration;
    }
    if (a->bounceTimer) {
      bound(a);
      u16 bounce = a->bounceTimer;
      vibration = a->vibrationTimer;
      a->bounceTimer = (u16)(bounce - 1);
    }
    if (vibration == 0) {
      u16 bounce = a->bounceTimer;
      s16 angle = a->savedAngle;
      a->shape_angle.x = angle;
      if (bounce == 0) {
        a->waitTimer = (s16)a->waitDuration;
        a->mode = 3;
      }
    }
  } else if (mode == 3) {
    s16 timer = a->waitTimer;
    if (timer >= 0) {
      if (timer > 0) {
        timer = (s16)(timer - 1);
        a->waitTimer = timer;
      }
      if (timer == 0) {
        a->pathPoint = ((u8)a->pathPoint + 1) & 1;
        setMoveInfo(a);
        a->mode = 0;
      }
    }
  }
  copy(&a->current.pos, &a->savedPosition);
finish:
  shineMove(a);
  initMatrix(a);
  setCollisionPosition(a);
  u32 p = play();
  gabi::call<void>(0x0200E240, at<void>(p + 0x26a4), sub<void>(a, 0x468));
  if (*sub<be<u32>>(a, 0xf4)) {
    u32 bg = gabi::ea((void *)a->background);
    if (bg && word(bg) < 256)
      gabi::call<void>(0x024F43DC, at<void>(bg));
  }
  return 1;
}
VERIFY(0x023A7FC0, execute);
BOOL draw(daObjTrap_c *a) {
  WWHD_FUNC(0x023A85D4, BOOL, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, at<void>(env), 1, &a->current.pos,
                   sub<void>(a, 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x02562F5C, at<void>(env), (void *)a->model,
                   sub<void>(a, 0x110));
  u32 model = gabi::ea((void *)a->model), data = word(model + 0xac);
  gabi::call<void>(0x025E7FC4, sub<void>(a, 0x3b0), at<void>(data),
                   (f32)a->anmFrame);
  gabi::call<void>(0x025E2DE0, (void *)a->model, 0);
  return 1;
}
VERIFY(0x023A85D4, draw);
s32 methodCreate(daObjTrap_c *a) {
  WWHD_FUNC(0x023A8644, s32, a);
  return create(a);
}
VERIFY(0x023A8644, methodCreate);
BOOL methodDelete(daObjTrap_c *a) {
  WWHD_FUNC(0x023A8648, BOOL, a);
  return remove(a);
}
VERIFY(0x023A8648, methodDelete);
BOOL methodExecute(daObjTrap_c *a) {
  WWHD_FUNC(0x023A864C, BOOL, a);
  return execute(a);
}
VERIFY(0x023A864C, methodExecute);
BOOL methodDraw(daObjTrap_c *a) {
  WWHD_FUNC(0x023A8650, BOOL, a);
  return draw(a);
}
VERIFY(0x023A8650, methodDraw);
void staticInit() {
  WWHD_FUNC(0x023A8654, void);
  put(0x1046C5A8, 0);
  put(0x1046C5A0, 0);
  put(0x1046C5AC, 0);
  put(0x1046C5A4, 0);
  gabi::call<void>(0x028F026C, at<void>(0x101CD604));
  *at<be<f32>>(0x1046C594) = -3.1415927410125732f;
  *at<be<f32>>(0x1046C598) = 3.1415927410125732f;
  gabi::call<void>(0x028ED6F8, at<void>(0x1046C59C));
  gabi::call<void>(0x028F026C, at<void>(0x101CD610));
  gabi::call<void>(0x028EAB2C, at<void>(0x1046C59D));
  gabi::call<void>(0x028F026C, at<void>(0x101CD61C));
}
VERIFY(0x023A8654, staticInit);
void lineDestructor(void *p, u32 flags) {
  WWHD_FUNC(0x023A86E8, void, p, flags);
  if (p) {
    u32 ea = gabi::ea(p);
    put(ea + 0x58, 0x10031768);
    put(ea + 0x64, 0x10031728);
    put(ea + 0x20, 0x10031718);
    gabi::call<void>(0x02008B4C, p, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, p);
  }
}
VERIFY(0x023A86E8, lineDestructor);
void emptyDestructor(void *p, u32 flags) {
  WWHD_FUNC(0x023A8760, void, p, flags);
  if (p && (flags & 1))
    gabi::call<void>(0x0273AF40, p);
}
VERIFY(0x023A8760, emptyDestructor);
void actorDestructor(daObjTrap_c *a, u32 flags) {
  WWHD_FUNC(0x023A8774, void, a, flags);
  if (a) {
    gabi::call<void>(0x02515A70, sub<void>(a, 0x468), 2);
    gabi::call<void>(0x02515860, sub<void>(a, 0x42c), 2);
    gabi::call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, a);
  }
}
VERIFY(0x023A8774, actorDestructor);
void emptyVirtual(void *p) { WWHD_FUNC(0x023A87E0, void, p); }
VERIFY(0x023A87E0, emptyVirtual);
BOOL isDelete(daObjTrap_c *a) {
  WWHD_FUNC(0x023A87E4, BOOL, a);
  return 1;
}
VERIFY(0x023A87E4, isDelete);
} // namespace Trap
