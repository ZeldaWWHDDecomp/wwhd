#include "JSystem/JParticle/JPA_math_drawcalc.h"
#include <cstring>
namespace {
template <class T> T rd(u32 p) { return gabi::load<T>(p); }
template <class T> void wr(u32 p, T v) { gabi::store<T>(p, v); }
u32 vfn(u32 p, u32 slot) { return rd<u32>(rd<u32>(p) + slot); }
f32 sinAngle(u16 a) { return rd<f32>(0x104a44f8 + (a >> 3) * 8); }
f32 cosAngle(u16 a) { return rd<f32>(0x104a44fc + (a >> 3) * 8); }
} // namespace
void JPA_mathYZ(s16 y, s16 z, be<f32> *m) {
  WWHD_FUNC(0x02824520, void, y, z, m);
  f32 sy = sinAngle(y), sz = sinAngle(z), cy = cosAngle(y), cz = cosAngle(z);
  m[0] = gabi::fmuls_ppc(cy, cz);
  m[1] = -sz;
  m[2] = gabi::fmuls_ppc(sy, cz);
  m[3] = 0.0f;
  m[4] = gabi::fmuls_ppc(cy, sz);
  m[5] = cz;
  m[6] = gabi::fmuls_ppc(sy, sz);
  m[7] = 0.0f;
  m[8] = -sy;
  m[9] = 0.0f;
  m[10] = cy;
  m[11] = 0.0f;
}
VERIFY(0x02824520, JPA_mathYZ);
void JPA_mathXYZ(s16 x, s16 y, s16 z, be<f32> *m) {
  WWHD_FUNC(0x028245AC, void, x, y, z, m);
  f32 sx = sinAngle(x), sy = sinAngle(y), sz = sinAngle(z), cx = cosAngle(x),
      cy = cosAngle(y), cz = cosAngle(z);
  f32 sxcz = gabi::fmuls_ppc(sx, cz), cxsz = gabi::fmuls_ppc(cx, sz);
  f32 sxsz = gabi::fmuls_ppc(sx, sz), cxcz = gabi::fmuls_ppc(cx, cz);
  m[0] = gabi::fmuls_ppc(cy, cz);
  m[4] = gabi::fmuls_ppc(cy, sz);
  m[8] = -sy;
  m[9] = gabi::fmuls_ppc(sx, cy);
  m[10] = gabi::fmuls_ppc(cx, cy);
  m[1] = gabi::fmsubs(sxcz, sy, cxsz);
  m[6] = gabi::fmsubs(cxsz, sy, sxcz);
  m[2] = gabi::fmadds(cxcz, sy, sxsz);
  m[5] = gabi::fmadds(sxsz, sy, cxcz);
  m[3] = 0.0f;
  m[7] = 0.0f;
  m[11] = 0.0f;
}
VERIFY(0x028245AC, JPA_mathXYZ);
void JPA_mathUnitVector(s16 x, s16 y, be<f32> *v) {
  WWHD_FUNC(0x02824AA0, void, x, y, v);
  f32 sx = sinAngle(x);
  v[0] = gabi::fmuls_ppc(cosAngle(y), sx);
  v[1] = gabi::fmuls_ppc(sinAngle(y), sx);
  v[2] = cosAngle(x);
}
VERIFY(0x02824AA0, JPA_mathUnitVector);
void JPA_mathDirection(const be<f32> *dir, be<f32> *m) {
  WWHD_FUNC(0x0282466C, void, dir, m);
  f32 y = -(f32)dir[0], x = dir[1], z = dir[2];
  f32 squared = gabi::fmadds(x, x, gabi::fmuls_ppc(y, y));
  f32 length = (f32)gabi::call<f64>(0x028F37F0, (f64)squared);
  if (length > 0x1p-18f) {
    f32 inv = 1.0f / length;
    x = gabi::fmuls_ppc(x, inv);
    y = gabi::fmuls_ppc(y, inv);
  } else {
    x = 0.0f;
    y = 0.0f;
  }
  f32 xx = gabi::fmuls_ppc(x, x), yy = gabi::fmuls_ppc(y, y),
      xy = gabi::fmuls_ppc(x, y);
  f32 ym = gabi::fmuls_ppc(y, length), xm = gabi::fmuls_ppc(x, length);
  f32 off = gabi::fmuls_ppc(xy, gabi::fsubs_ppc(1.0f, z));
  m[0] = gabi::fmadds(z, gabi::fsubs_ppc(1.0f, xx), xx);
  m[1] = off;
  m[2] = -ym;
  m[3] = 0.0f;
  m[4] = off;
  m[5] = gabi::fmadds(z, gabi::fsubs_ppc(1.0f, yy), yy);
  m[6] = xm;
  m[7] = 0.0f;
  m[8] = ym;
  m[9] = -xm;
  m[10] = z;
  m[11] = 0.0f;
}
VERIFY(0x0282466C, JPA_mathDirection);
f32 JPA_mathKeyValue(f32 time, s32 count, const be<f32> *keys) {
  WWHD_FUNC(0x028249D8, f32, time, count, keys);
  f32 first = keys[0];
  if (time < first)
    return keys[1];
  if (!((f32)keys[(u32)count * 4 - 4] > time))
    return keys[(u32)count * 4 - 3];
  while (count > 1) {
    s32 half = count / 2;
    if (time < (f32)keys[half * 4])
      count = half;
    else {
      count -= half;
      keys += half * 4;
    }
  }
  f32 t0 = keys[0], t1 = keys[4];
  f32 delta = gabi::fsubs_ppc(time, t0), span = gabi::fsubs_ppc(t1, t0),
      u = delta / span;
  f32 square = gabi::fmuls_ppc(u, u), difference = gabi::fsubs_ppc(square, u);
  f32 value = keys[1], next = keys[5], out = keys[3], in = keys[6];
  f32 a = gabi::fmadds(out, difference, out);
  f32 b = gabi::fmsubs(gabi::fadds_ppc(u, u), difference, square);
  f32 c = gabi::fmadds(in, difference, a);
  f32 d = gabi::fmadds(b, gabi::fsubs_ppc(value, next), value);
  f32 e = gabi::fmsubs(u, out, c);
  return -gabi::fmsubs(delta, e, d);
}
VERIFY(0x028249D8, JPA_mathKeyValue);
void JPA_drawBindTextureState(JPADrawMath_l *draw, s16 index, void *state) {
  WWHD_FUNC(0x0282DDE4, void, draw, index, state);
  if (!state)
    return;
  u32 d = gabi::ea(draw), s = gabi::ea(state);
  u32 entry = rd<u32>(rd<u32>(d + 0xdc) + 0x54) + (s32)index * 16;
  u32 oldA = rd<u32>(s + 0x15c), a = rd<u32>(entry + 8),
      oldB = rd<u32>(s + 0x160), b = rd<u32>(entry + 12);
  if (oldA != a) {
    wr<u32>(s + 0x15c, a);
    wr<u8>(s + 0x190, rd<u8>(s + 0x190) | 2);
  }
  if (oldB != b) {
    wr<u32>(s + 0x160, b);
    wr<u8>(s + 0x190, rd<u8>(s + 0x190) | 2);
  }
  for (u32 off : {0x150u, 0x154u})
    if (rd<u32>(s + off) != 1) {
      wr<u32>(s + off, 1);
      wr<u8>(s + 0x190, rd<u8>(s + 0x190) | 4);
    }
  for (u32 off : {0x168u, 0x16cu, 0x170u, 0x174u})
    wr<u32>(s + off, 0);
  wr<u8>(s + 0x190, rd<u8>(s + 0x190) | 0x20);
}
VERIFY(0x0282DDE4, JPA_drawBindTextureState);
void JPA_drawCalc(JPADrawMath_l *draw) {
  WWHD_FUNC(0x0282DC48, void, draw);
  u32 d = gabi::ea(draw);
  for (s32 i = 0; i < (u8)draw->emitterVisitorCount; ++i) {
    u32 visitor = rd<u32>(d + 0x24 + i * 4);
    gabi::call<void>(vfn(visitor, 0x14), gabi::at<void>(visitor),
                     gabi::at<void>(d + 0xb4));
  }
}
VERIFY(0x0282DC48, JPA_drawCalc);
namespace {
void calcDrawParticle(JPADrawMath_l *draw, void *particle, bool child) {
  u32 d = gabi::ea(draw), p = gabi::ea(particle);
  wr<u16>(p + 0xc0, rd<u16>(p + 0xc0) + rd<s16>(p + 0xc2));
  u32 array = child ? 0x80 : 0x48, count = child ? 0xaf : 0xad;
  for (s32 i = 0; i < rd<u8>(d + count); ++i) {
    u32 visitor = rd<u32>(d + array + i * 4);
    gabi::call<void>(vfn(visitor, 0x14), gabi::at<void>(visitor),
                     gabi::at<void>(d + 0xb4), particle);
  }
}
} // namespace
void JPA_drawCalcParticle(JPADrawMath_l *draw, void *particle) {
  WWHD_FUNC(0x0282DCBC, void, draw, particle);
  calcDrawParticle(draw, particle, false);
}
VERIFY(0x0282DCBC, JPA_drawCalcParticle);
void JPA_drawCalcChild(JPADrawMath_l *draw, void *particle) {
  WWHD_FUNC(0x0282DD50, void, draw, particle);
  calcDrawParticle(draw, particle, true);
}
VERIFY(0x0282DD50, JPA_drawCalcChild);
namespace {
template <class T> T getter(u32 d, u32 member, u32 slot) {
  u32 object = rd<u32>(d + member);
  return gabi::call<T>(vfn(object, slot), gabi::at<void>(object));
}
f32 randomF(u32 d) {
  u32 emitter = rd<u32>(d + 0xc0),
      seed = rd<u32>(emitter + 0x1ec) * 0x19660du + 0x3c6ef35fu;
  wr<u32>(emitter + 0x1ec, seed);
  u32 bits = (seed >> 9) | 0x3f800000;
  f32 value;
  std::memcpy(&value, &bits, 4);
  return gabi::fsubs_ppc(value, 1.0f);
}
f32 randomRF(u32 d) {
  f32 f = randomF(d);
  return gabi::fsubs_ppc(gabi::fadds_ppc(f, f), 1.0f);
}
u32 textureIndex(u32 d, u32 index) {
  return rd<u16>(rd<u32>(d + 0xe0) + index * 2);
}
u32 texture(u32 d, u32 index) {
  return rd<u32>(rd<u32>(rd<u32>(d + 0xdc) + 0x50) + index * 4);
}
void bindTexture(u32 d, u32 p, u32 slot, u32 index) {
  u32 object = texture(d, index), context = rd<u32>(rd<u32>(d + 0xc0) + 0x3dc);
  gabi::call<void>(0x027FE500, gabi::at<void>(rd<u32>(p + 0xf0)), slot,
                   gabi::at<void>(object), gabi::at<void>(context));
}
u32 textureState(u32 p, u32 slot) {
  return gabi::ea(
      gabi::call<void *>(0x027FE640, gabi::at<void>(rd<u32>(p + 0xf0)), slot));
}
void resetTextureState(u32 d, u32 p, u32 slot, u32 index) {
  u32 state = textureState(p, slot);
  gabi::call<void>(0x0282DDE4, gabi::at<JPADrawMath_l>(d), (s16)index,
                   gabi::at<void>(state));
}
// HD caches a texture descriptor and the emitter texture generation on each
// particle.
void cacheTexture(u32 d, u32 p, u32 index, u32 object) {
  if (rd<u32>(p + 0xf4) == index &&
      rd<u32>(p + 0xf8) == rd<u16>(rd<u32>(d + 0xc0) + 0x260))
    return;
  u32 state = textureState(p, 0);
  bool equal = true;
  for (u32 off : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u}) {
    if (rd<u32>(state + off) != rd<u32>(object + off)) {
      equal = false;
      break;
    }
  }
  if (!equal)
    gabi::call<void>(0x027BDEB4, gabi::at<void>(state), gabi::at<void>(object));
  else {
    u32 a = rd<u32>(object + 0x28), b = rd<u32>(object + 0x30);
    wr<u32>(state + 0xd4, a);
    wr<u32>(state + 0xdc, b);
    wr<u32>(state + 0x28, a);
    wr<u32>(state + 0x30, b);
  }
  wr<u32>(p + 0xf4, index);
  wr<u32>(p + 0xf8, rd<u16>(rd<u32>(d + 0xc0) + 0x260));
  if (!getter<u32>(d, 0xc4, 0x124))
    resetTextureState(d, p, 0, index);
}
} // namespace
void JPA_drawInitParticle(JPADrawMath_l *draw, void *particle) {
  WWHD_FUNC(0x0282DED0, void, draw, particle);
  u32 d = gabi::ea(draw), p = gabi::ea(particle);
  f32 ax = rd<f32>(0x104b576c), ay = rd<f32>(0x104b577c),
      az = rd<f32>(0x104b578c);
  wr<f32>(p + 0x90, ay);
  wr<f32>(p + 0x8c, ax);
  wr<f32>(p + 0x94, az);
  wr<u32>(p + 0xb8, rd<u32>(d + 0xe8));
  wr<u32>(p + 0xbc, rd<u32>(d + 0xec));
  wr<f32>(p + 0xac, 1.0f);
  f32 random = randomF(d);
  s32 offset = getter<s32>(d, 0xc4, 0x44);
  wr<s32>(p + 0xb4, gabi::ftoi(gabi::fmuls_ppc(random, (f32)offset)));
  if (rd<u32>(d + 0xc8)) {
    if (getter<u32>(d, 0xc8, 0x124)) {
      f32 r = gabi::fsubs_ppc(randomF(d), 0.5f);
      u32 extra = rd<u32>(d + 0xc8), table = rd<u32>(extra);
      f32 a = gabi::call<f32>(rd<u32>(table + 0x13c), gabi::at<void>(extra));
      f32 angleRandom = gabi::fmuls_ppc(r, a);
      f32 angle = gabi::call<f32>(rd<u32>(table + 0x12c),
                                  gabi::at<void>(rd<u32>(d + 0xc8)));
      wr<u16>(p + 0xc0,
              gabi::ftoi(gabi::fmadds(angleRandom, 65536.0f,
                                      gabi::fmuls_ppc(angle, 32768.0f))));
      r = randomRF(d);
      f32 direction = getter<f32>(d, 0xc8, 0x14c);
      extra = rd<u32>(d + 0xc8);
      table = rd<u32>(extra);
      f32 speedRandom = randomRF(d);
      f32 speed = gabi::call<f32>(rd<u32>(table + 0x134),
                                  gabi::at<void>(rd<u32>(d + 0xc8)));
      if (!(r < direction))
        speed = -speed;
      f32 variation = gabi::call<f32>(rd<u32>(table + 0x144),
                                      gabi::at<void>(rd<u32>(d + 0xc8)));
      wr<u16>(p + 0xc2,
              gabi::ftoi(gabi::fmuls_ppc(
                  gabi::fmuls_ppc(speed,
                                  gabi::fmadds(variation, speedRandom, 1.0f)),
                  32768.0f)));
    } else {
      wr<u16>(p + 0xc0, 0);
      wr<u16>(p + 0xc2, 0);
    }
    f32 scale;
    if (getter<u32>(d, 0xc8, 0x14)) {
      f32 r = randomRF(d), variation = getter<f32>(d, 0xc8, 0xb4);
      scale =
          gabi::fmuls_ppc(gabi::fmadds(r, variation, 1.0f), rd<f32>(d + 0xe4));
    } else
      scale = rd<f32>(d + 0xe4);
    wr<f32>(p + 0x98, scale);
    wr<f32>(p + 0xa0, scale);
    wr<f32>(p + 0x9c, scale);
    f32 alpha = 1.0f;
    if (getter<u32>(d, 0xc8, 0xbc)) {
      f32 r = randomRF(d), variation = getter<f32>(d, 0xc8, 0x10c);
      alpha = gabi::fmadds(r, variation, 1.0f);
    }
    wr<f32>(p + 0xb0, alpha);
  } else {
    wr<u16>(p + 0xc0, 0);
    wr<u16>(p + 0xc2, 0);
    f32 scale = rd<f32>(d + 0xe4);
    wr<f32>(p + 0xa0, scale);
    wr<f32>(p + 0x9c, scale);
    wr<f32>(p + 0x98, scale);
    wr<f32>(p + 0xb0, 1.0f);
  }
  u32 index = textureIndex(d, 0), object = texture(d, index);
  if (getter<u32>(d, 0xc4, 0x124)) {
    u32 resource = rd<u32>(d + 0xdc);
    object = texture(d, rd<u32>(resource + 0x5c));
    index = 0x31;
  }
  cacheTexture(d, p, index, object);
  if (!rd<u32>(d + 0xd0))
    return;
  u32 type = getter<u32>(d, 0xd0, 0x14);
  if (type == 1 || type == 2) {
    index = textureIndex(d, getter<u32>(d, 0xd0, 0x34));
    bindTexture(d, p, 5, index);
    if (type == 1)
      resetTextureState(d, p, 5, index);
    else {
      index = textureIndex(d, getter<u32>(d, 0xd0, 0x3c));
      bindTexture(d, p, 6, index);
    }
  }
  if (getter<u32>(d, 0xd0, 0x44)) {
    index = textureIndex(d, getter<u32>(d, 0xd0, 0x4c));
    bindTexture(d, p, 7, index);
    resetTextureState(d, p, 7, index);
  }
}
VERIFY(0x0282DED0, JPA_drawInitParticle);
void JPA_drawInitChild(JPADrawMath_l *draw, void *parent, void *child) {
  WWHD_FUNC(0x0282E770, void, draw, parent, child);
  u32 d = gabi::ea(draw), a = gabi::ea(parent), p = gabi::ea(child);
  wr<u32>(p + 0x8c, rd<u32>(a + 0x8c));
  wr<u32>(p + 0x90, rd<u32>(a + 0x90));
  wr<f32>(p + 0xac, 1.0f);
  wr<u32>(p + 0x94, rd<u32>(a + 0x94));
  if (getter<u32>(d, 0xcc, 0xbc)) {
    f32 ratio = getter<f32>(d, 0xcc, 0x114);
    for (u32 off : {0xb8u, 0xb9u, 0xbau, 0xbcu, 0xbdu, 0xbeu})
      wr<u8>(p + off, gabi::ftoi(gabi::fmuls_ppc((f32)rd<u8>(a + off), ratio)));
  } else {
    wr<u32>(p + 0xb8, getter<u32>(d, 0xcc, 0xdc));
    wr<u32>(p + 0xbc, getter<u32>(d, 0xcc, 0xe4));
  }
  if (getter<u32>(d, 0xcc, 0xb4)) {
    f32 ratio = getter<f32>(d, 0xcc, 0x10c);
    ratio = gabi::fmuls_ppc(ratio, rd<f32>(a + 0xac));
    wr<u8>(p + 0xbb, gabi::ftoi(gabi::fmuls_ppc((f32)rd<u8>(a + 0xbb), ratio)));
    wr<u8>(p + 0xbf, gabi::ftoi(gabi::fmuls_ppc((f32)rd<u8>(a + 0xbf), ratio)));
  } else {
    wr<u8>(p + 0xbb, getter<u32>(d, 0xcc, 0xec));
    wr<u8>(p + 0xbf, getter<u32>(d, 0xcc, 0xf4));
  }
  if (getter<u32>(d, 0xcc, 0xac)) {
    f32 ratio = getter<f32>(d, 0xcc, 0x104),
        x = gabi::fmuls_ppc(ratio, rd<f32>(a + 0x9c));
    wr<f32>(p + 0x98, x);
    wr<f32>(p + 0x9c, x);
    f32 y = gabi::fmuls_ppc(ratio, rd<f32>(a + 0xa0));
    wr<f32>(p + 0xb0, y);
    wr<f32>(p + 0xa0, y);
  } else {
    for (u32 off : {0xb0u, 0x98u, 0x9cu, 0xa0u})
      wr<f32>(p + off, 1.0f);
  }
  wr<u16>(p + 0xc0, rd<u16>(a + 0xc0));
  if (getter<u32>(d, 0xcc, 0xa4)) {
    f32 speed = getter<f32>(d, 0xcc, 0xfc);
    wr<u16>(p + 0xc2, gabi::ftoi(gabi::fmuls_ppc(speed, 32768.0f)));
  } else
    wr<u16>(p + 0xc2, 0);
  u32 index = textureIndex(d, 0);
  if (!getter<u32>(d, 0xc4, 0x124))
    index = textureIndex(d, getter<u32>(d, 0xcc, 0xc4));
  u32 object = texture(d, index);
  if (getter<u32>(d, 0xc4, 0x124)) {
    object = texture(d, rd<u32>(rd<u32>(d + 0xdc) + 0x5c));
    index = 0x31;
  }
  cacheTexture(d, p, index, object);
}
VERIFY(0x0282E770, JPA_drawInitChild);
