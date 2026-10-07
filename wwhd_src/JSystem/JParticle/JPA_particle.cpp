#include "JSystem/JParticle/JPA_particle.h"
#include <climits>
#include <cstring>
namespace {
constexpr u32 info = 0x104b5730;
template <class T> T rd(u32 p) { return gabi::load<T>(p); }
template <class T> void wr(u32 p, T v) { gabi::store<T>(p, v); }
u32 vfn(u32 p, u32 slot) { return rd<u32>(rd<u32>(p) + slot); }
s32 divide(s32 a, s32 b) {
  return !b || (a == INT_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}
} // namespace
void JPA_particleIncFrame(JPAParticle_l *p) {
  WWHD_FUNC(0x028266C0, void, p);
  f32 age = gabi::fadds_ppc(p->age, 1.0f);
  if (age < 0.0f)
    age = 0.0f;
  f32 lifetime = p->lifetime;
  p->status = (u32)p->status & ~1u;
  p->age = age;
  if (!(age < lifetime)) {
    p->normalizedTime = 1.0f;
    p->status = (u32)p->status | 2;
  } else {
    p->normalizedTime = age / lifetime;
  }
}
VERIFY(0x028266C0, JPA_particleIncFrame);
void JPA_particleCalcVelocity(JPAParticle_l *p) {
  WWHD_FUNC(0x02826720, void, p);
  p->fieldVelocity[0] = 0.0f;
  p->fieldVelocity[1] = 0.0f;
  p->fieldVelocity[2] = 0.0f;
  if ((u32)p->status & 0x20) {
    for (u32 i = 0; i < 3; ++i)
      p->offset[i] = rd<f32>(info + 0xe0 + 4 * i);
  }
  for (u32 i = 0; i < 3; ++i)
    p->baseVelocity[i] =
        gabi::fadds_ppc(p->baseVelocity[i], p->acceleration[i]);
  if (!((u32)p->status & 0x40))
    gabi::call<void>(0x028242E8, gabi::at<void>(rd<u32>(info + 4) + 0x19c), p);
  f32 air = p->airResistance;
  f32 scale = gabi::fmuls_ppc(p->moment, p->drag);
  for (u32 i = 0; i < 3; ++i) {
    f32 base = gabi::fmuls_ppc(p->baseVelocity[i], air);
    f32 field = gabi::fadds_ppc(p->fieldVelocity[i], p->fieldAcceleration[i]);
    p->baseVelocity[i] = base;
    p->fieldVelocity[i] = field;
    p->velocity[i] = gabi::fmuls_ppc(gabi::fadds_ppc(base, field), scale);
  }
}
VERIFY(0x02826720, JPA_particleCalcVelocity);
void JPA_particleCalcPosition(JPAParticle_l *p) {
  WWHD_FUNC(0x0282685C, void, p);
  for (u32 i = 0; i < 3; ++i) {
    f32 local = gabi::fadds_ppc(p->local[i], p->velocity[i]);
    p->local[i] = local;
    p->global[i] =
        gabi::fmadds(local, rd<f32>(info + 0xec + 4 * i), p->offset[i]);
  }
}
VERIFY(0x0282685C, JPA_particleCalcPosition);
bool JPA_particleCheckCreateChild(JPAParticle_l *p) {
  WWHD_FUNC(0x028268C8, bool, p);
  u32 emitter = rd<u32>(info + 4);
  u32 sweep = rd<u32>(rd<u32>(emitter + 0x1e0) + 0xc);
  f32 lifetime = p->lifetime, time = 1.0f;
  u32 timingTarget = vfn(sweep, 0x44);
  if (lifetime > 1.0f)
    time = (f32)p->age / (lifetime - 1.0f);
  f32 timing = gabi::call<f32>(timingTarget, gabi::at<void>(sweep));
  if (time < timing)
    return false;
  u32 stepTarget = vfn(sweep, 0x4c);
  s32 age = gabi::ftoi(p->age);
  u32 step = gabi::call<u32>(stepTarget, gabi::at<void>(sweep)) + 1;
  return (u32)age - (u32)divide(age, (s32)step) * step == 0;
}
VERIFY(0x028268C8, JPA_particleCheckCreateChild);
void JPA_particleDelete(void *emitter, JPAParticle_l *p) {
  WWHD_FUNC(0x0281FBAC, void, emitter, p);
  if ((u8)p->deletionDelay)
    return;
  u8 delay = p->deletionDelay;
  p->hidden = 1;
  if (!delay)
    p->deletionDelay = 2;
}
VERIFY(0x0281FBAC, JPA_particleDelete);
void JPA_particleReleaseHidden(void *emitter, JPAParticle_l *p, void *list) {
  WWHD_FUNC(0x0281FA98, void, emitter, p, list);
  u32 base = gabi::ea(p);
  for (u32 i = 1; i <= 14; ++i)
    gabi::call<void>(0x027FE3D4, gabi::at<void>(rd<u32>(base + 0xf0)), i);
  gabi::call<void>(0x02825868, p);
  gabi::call<void>(0x027EC560, list, p);
  gabi::call<void>(0x027EC778,
                   gabi::at<void>(rd<u32>(gabi::ea(emitter) + 0x1dc)), p);
}
VERIFY(0x0281FA98, JPA_particleReleaseHidden);
JPAParticle_l *JPA_particleGetVacant(void *emitter) {
  WWHD_FUNC(0x0281DC6C, JPAParticle_l *, emitter);
  u32 list = rd<u32>(gabi::ea(emitter) + 0x1dc);
  if (!rd<u32>(list + 8))
    return nullptr;
  u32 particle = rd<u32>(rd<u32>(list));
  gabi::call<void>(0x027EC560, gabi::at<void>(list), gabi::at<void>(particle));
  return gabi::at<JPAParticle_l>(particle);
}
VERIFY(0x0281DC6C, JPA_particleGetVacant);
JPAParticle_l *JPA_particleCreate(void *emitter) {
  WWHD_FUNC(0x0281DCB8, JPAParticle_l *, emitter);
  auto *particle = gabi::call<JPAParticle_l *>(0x0281DC6C, emitter);
  if (!particle)
    return nullptr;
  u32 e = gabi::ea(emitter);
  gabi::call<void>(0x027EC778, gabi::at<void>(e + 0x1ac), particle);
  u32 adjusted = e + (s32)rd<s16>(e);
  s16 slot = rd<s16>(e + 2);
  u32 target = slot < 0 ? rd<u32>(e + 4)
                        : rd<u32>(rd<u32>(adjusted + (s32)rd<s16>(e + 6)) +
                                  (s32)slot * 8 + 4);
  gabi::call<void>(target, gabi::at<void>(adjusted));
  gabi::call<void>(0x028258B0, particle);
  gabi::call<void>(0x0282DED0, gabi::at<void>(e + 0x9c), particle);
  return particle;
}
VERIFY(0x0281DCB8, JPA_particleCreate);
void JPA_particleCreateChildren(void *emitter, JPAParticle_l *parent) {
  WWHD_FUNC(0x0281FB08, void, emitter, parent);
  u32 e = gabi::ea(emitter);
  u32 sweep = rd<u32>(rd<u32>(e + 0x1e0) + 0xc);
  s32 count = gabi::call<s32>(vfn(sweep, 0x3c), gabi::at<void>(sweep));
  while (count > 0) {
    auto *particle = gabi::call<JPAParticle_l *>(0x0281DC6C, emitter);
    if (!particle)
      break;
    gabi::call<void>(0x027EC778, gabi::at<void>(e + 0x1b8), particle);
    gabi::call<void>(0x028260B4, particle, parent);
    gabi::call<void>(0x0282E770, gabi::at<void>(e + 0x9c), parent, particle);
    --count;
  }
}
VERIFY(0x0281FB08, JPA_particleCreateChildren);
namespace {
void calcParticles(void *emitter, bool children) {
  u32 e = gabi::ea(emitter), list = e + (children ? 0x1b8 : 0x1ac);
  u32 link = rd<u32>(list);
  while (link) {
    u32 particle = rd<u32>(link), next = rd<u32>(link + 0xc);
    auto *p = gabi::at<JPAParticle_l>(particle);
    u8 delay = p->deletionDelay;
    if (delay) {
      p->deletionDelay = --delay;
      if (!delay)
        gabi::call<void>(0x0281FA98, emitter, p, gabi::at<void>(list));
    } else {
      gabi::call<void>(0x028266C0, p);
      u32 status = p->status;
      if (status & 0x80)
        p->status = status | 2;
      else {
        if (!children || gabi::ftoi(p->age) != 0)
          gabi::call<void>(0x02826720, p);
        u32 cb = rd<u32>(particle + 0xc8);
        if (cb)
          gabi::call<void>(vfn(cb, 0x1c), gabi::at<void>(cb), emitter, p);
        if (!((u32)p->status & 2)) {
          gabi::call<void>(children ? 0x0282DD50 : 0x0282DCBC,
                           gabi::at<void>(e + 0x9c), p);
          if (!children && rd<u32>(rd<u32>(e + 0x1e0) + 0xc) &&
              gabi::call<bool>(0x028268C8, p))
            gabi::call<void>(0x0281FB08, emitter, p);
          gabi::call<void>(0x0282685C, p);
        }
      }
      if ((u32)p->status & 2)
        gabi::call<void>(0x0281FBAC, emitter, p);
    }
    link = next;
  }
}
} // namespace
void JPA_particleCalc(void *emitter) {
  WWHD_FUNC(0x0281FBD8, void, emitter);
  calcParticles(emitter, false);
}
VERIFY(0x0281FBD8, JPA_particleCalc);
void JPA_particleCalcChild(void *emitter) {
  WWHD_FUNC(0x0281FD14, void, emitter);
  calcParticles(emitter, true);
}
VERIFY(0x0281FD14, JPA_particleCalcChild);
void JPA_particleDeleteAll(void *emitter, s32 deferred) {
  WWHD_FUNC(0x0281DE68, void, emitter, deferred);
  if (deferred) {
    gabi::call<void>(0x0281DD6C, emitter);
    return;
  }
  u32 e = gabi::ea(emitter);
  for (u32 offset : {0x1c4u, 0x1d0u}) {
    u32 cursor = rd<u32>(e + offset + 8);
    u32 end = cursor + rd<u32>(e + offset) * 4;
    while (cursor != end) {
      u32 particle = rd<u32>(cursor);
      for (u32 i = 1; i <= 14; ++i)
        gabi::call<void>(0x027FE3D4, gabi::at<void>(rd<u32>(particle + 0xf0)),
                         i);
      gabi::call<void>(0x02825868, gabi::at<void>(rd<u32>(cursor)));
      cursor += 4;
    }
    gabi::call<void>(0x0273B5DC, gabi::at<void>(e + offset));
  }
  u32 link = rd<u32>(e + 0x1ac);
  wr<u8>(e + 0x3d5, 0);
  for (u32 list : {e + 0x1acu, e + 0x1b8u}) {
    if (list != e + 0x1ac)
      link = rd<u32>(list);
    while (link) {
      u32 particle = rd<u32>(link);
      for (u32 i = 1; i <= 14; ++i)
        gabi::call<void>(0x027FE3D4, gabi::at<void>(rd<u32>(particle + 0xf0)),
                         i);
      gabi::call<void>(0x02825868, gabi::at<void>(rd<u32>(link)));
      u32 next = rd<u32>(link + 0xc);
      gabi::call<void>(0x027EC560, gabi::at<void>(list), gabi::at<void>(link));
      gabi::call<void>(0x027EC778, gabi::at<void>(rd<u32>(e + 0x1dc)),
                       gabi::at<void>(link));
      link = next;
    }
  }
  if (rd<u8>(e + 0x3d6)) {
    if (!rd<u8>(e + 0x3d4))
      wr<u8>(e + 0x3d4, 1);
    wr<u8>(e + 0x3d6, 0);
  }
}
VERIFY(0x0281DE68, JPA_particleDeleteAll);
namespace {
u32 particleRandomNext(u32 e) {
  u32 seed = rd<u32>(e + 0x1ec) * 0x19660du + 0x3c6ef35fu;
  wr<u32>(e + 0x1ec, seed);
  return seed;
}
f32 particleRandom(u32 e) {
  u32 bits = 0x3f800000u | (particleRandomNext(e) >> 9);
  f32 value;
  std::memcpy(&value, &bits, 4);
  return value - 1.0f;
}
f32 particleRandomRF(u32 e) {
  f32 value = particleRandom(e);
  return gabi::fsubs_ppc(gabi::fadds_ppc(value, value), 1.0f);
}
f32 lengthSquared(f32 x, f32 y, f32 z) {
  return gabi::fmadds(z, z, gabi::fmadds(x, x, gabi::fmuls_ppc(y, y)));
}
f32 inverseLength(f32 squared) {
  return 1.0f / (f32)gabi::call<f64>(0x028F37F0, (f64)squared);
}
void particleInitTail(JPAParticle_l *p, u32 e, u32 sweep, bool child) {
  u32 base = gabi::ea(p);
  p->deletionDelay = 0;
  p->hidden = 0;
  u32 gpu = rd<u32>(base + 0xec);
  wr<u32>(gpu + 0x368, 0);
  wr<u32>(gpu + 0x36c, 0);
  bool useSlot = child;
  if (!child) {
    u32 data = rd<u32>(e + 0x1e0);
    u32 shape = rd<u32>(data + 0xc);
    useSlot =
        !shape || gabi::call<u32>(vfn(shape, 0x84), gabi::at<void>(shape));
  }
  if (useSlot) {
    u32 shape = child ? sweep : rd<u32>(rd<u32>(e + 0x1e0) + 4);
    s32 type = gabi::call<s32>(vfn(shape, 0x14), gabi::at<void>(shape));
    bool allocate = type == 5;
    if (!allocate) {
      shape = child ? sweep : rd<u32>(rd<u32>(e + 0x1e0) + 4);
      allocate = gabi::call<s32>(vfn(shape, 0x14), gabi::at<void>(shape)) == 6;
    }
    if (allocate) {
      gabi::call<void>(0x0281E8C0, gabi::at<void>(e), (s32)child);
      wr<u32>(base + 0xe4, rd<u32>(e + (child ? 0x304 : 0x300)));
    }
  }
  gabi::call<void>(0x02825750, p, gabi::at<void>(e));
  u32 cb = rd<u32>(base + 0xc8);
  if (cb)
    gabi::call<void>(vfn(cb, 0x14), gabi::at<void>(cb), gabi::at<void>(e), p);
  gabi::call<void>(0x02825430, p, gabi::at<void>(0x104a01ec));
  gabi::call<void>(0x02825514, p, gabi::at<void>(0x104a01ec));
}
} // namespace
void JPA_particleInit(JPAParticle_l *p) {
  WWHD_FUNC(0x028258B0, void, p);
  u32 e = rd<u32>(info + 4), base = gabi::ea(p);
  wr<u32>(base + 0xd4, e);
  for (u32 i = 0; i < 3; ++i)
    p->fieldAcceleration[i] = 0.0f;
  p->fieldDrag = 1.0f;
  p->drag = 1.0f;
  p->status = 1;
  gabi::call<void>(0x028E8F64, gabi::at<void>(info + 8),
                   gabi::at<void>(info + 0x134), gabi::at<void>(base + 0x1c));
  if (rd<u32>(e + 0x84) & 8)
    p->status = (u32)p->status | 0x20;
  for (u32 i = 0; i < 3; ++i)
    p->offset[i] = rd<f32>(info + 0xe0 + 4 * i);
  f32 omni[3]{}, axis[3]{}, direction[3]{}, random[3]{};
  for (u32 kind = 0; kind < 2; ++kind) {
    f32 speed = rd<f32>(e + 0x68 + 4 * kind);
    u32 vec = info + 0x140 + 12 * kind;
    f32 squared =
        lengthSquared(rd<f32>(vec), rd<f32>(vec + 4), rd<f32>(vec + 8));
    if (speed != 0.0f && squared > 0x1p-18f) {
      f32 scale = gabi::fmuls_ppc(inverseLength(squared), speed);
      for (u32 i = 0; i < 3; ++i)
        (kind ? axis : omni)[i] = gabi::fmuls_ppc(rd<f32>(vec + 4 * i), scale);
    }
  }
  if (rd<f32>(e + 0x70) != 0.0f) {
    f32 angle = gabi::fmuls_ppc(gabi::fmuls_ppc(32768.0f, particleRandomRF(e)),
                                rd<f32>(e + 0x58));
    s16 a = (s16)gabi::ftoi(angle), b = (s16)(particleRandomNext(e) >> 16);
    gabi::Local<be<f32>[12]> matrix;
    gabi::call<void>(0x02824520, a, b, matrix.get());
    gabi::call<void>(0x028E9108, gabi::at<void>(info + 0x98), matrix.get(),
                     matrix.get());
    for (u32 i = 0; i < 3; ++i)
      direction[i] = gabi::fmuls_ppc((*matrix)[i * 4 + 2], rd<f32>(e + 0x70));
  }
  if (rd<f32>(e + 0x74) != 0.0f) {
    f32 r[3];
    for (auto &v : r)
      v = particleRandom(e) - 0.5f;
    f32 speed = rd<f32>(e + 0x74);
    for (u32 i = 0; i < 3; ++i)
      random[i] = gabi::fmuls_ppc(r[i], speed);
  }
  f32 ratioRandom = particleRandomRF(e);
  f32 ratio = gabi::fmadds(rd<f32>(e + 0x78), ratioRandom, 1.0f);
  for (u32 i = 0; i < 3; ++i) {
    f32 sum = gabi::fadds_ppc(
        gabi::fadds_ppc(gabi::fadds_ppc(omni[i], axis[i]), direction[i]),
        random[i]);
    p->baseVelocity[i] = gabi::fmuls_ppc(sum, ratio);
  }
  if (rd<u32>(e + 0x84) & 4)
    for (u32 i = 0; i < 3; ++i)
      p->baseVelocity[i] =
          gabi::fmuls_ppc(p->baseVelocity[i], rd<f32>(e + 8 + 4 * i));
  gabi::call<void>(0x028E8F64, gabi::at<void>(info + 0x38),
                   gabi::at<void>(base + 0x40), gabi::at<void>(base + 0x40));
  f32 accelRandom = particleRandomRF(e);
  f32 accel = gabi::fmuls_ppc(
      rd<f32>(e + 0x3c), gabi::fmadds(rd<f32>(e + 0x40), accelRandom, 1.0f));
  f32 squared =
      lengthSquared(p->baseVelocity[0], p->baseVelocity[1], p->baseVelocity[2]);
  if (squared > 0x1p-18f) {
    f32 scale = gabi::fmuls_ppc(inverseLength(squared), accel);
    for (u32 i = 0; i < 3; ++i)
      p->acceleration[i] = gabi::fmuls_ppc(p->baseVelocity[i], scale);
  } else
    for (u32 i = 0; i < 3; ++i)
      p->acceleration[i] = 0.0f;
  f32 airRandom = particleRandom(e) - 0.5f;
  f32 air = gabi::fmadds(rd<f32>(e + 0x48), airRandom, rd<f32>(e + 0x44));
  p->airResistance = air;
  if (air > 1.0f)
    p->airResistance = 1.0f;
  f32 momentRandom = particleRandom(e);
  p->moment = gabi::fmuls_ppc(
      rd<f32>(e + 0x4c), gabi::fnmsubs(rd<f32>(e + 0x50), momentRandom, 1.0f));
  p->age = -1.0f;
  f32 lifetimeRandom = particleRandom(e);
  p->lifetime =
      gabi::fmuls_ppc((f32)rd<s16>(e + 0x60),
                      gabi::fnmsubs(rd<f32>(e + 0x54), lifetimeRandom, 1.0f));
  p->normalizedTime = 0.0f;
  for (u32 i = 0; i < 3; ++i)
    p->global[i] =
        gabi::fmadds(p->local[i], rd<f32>(info + 0xec + 4 * i), p->offset[i]);
  wr<u32>(base + 0xc8, rd<u32>(e + 0x1e8));
  gabi::call<void>(0x02824230, gabi::at<void>(e + 0x19c), p);
  wr<u32>(base + 0xd0, 0);
  particleInitTail(p, e, 0, false);
}
VERIFY(0x028258B0, JPA_particleInit);
void JPA_particleInitChild(JPAParticle_l *p, JPAParticle_l *parent) {
  WWHD_FUNC(0x028260B4, void, p, parent);
  u32 e = rd<u32>(info + 4), base = gabi::ea(p);
  u32 sweep = rd<u32>(rd<u32>(e + 0x1e0) + 0xc);
  wr<u32>(base + 0xd4, e);
  p->status = 5;
  if (!gabi::call<u32>(vfn(sweep, 0x7c), gabi::at<void>(sweep))) {
    p->fieldDrag = 1.0f;
    p->drag = 1.0f;
    p->status = (u32)p->status | 0x40;
  } else {
    p->drag = parent->drag;
    p->fieldDrag = parent->fieldDrag;
  }
  f32 randomRatio = particleRandomRF(e);
  u32 table = rd<u32>(sweep);
  f32 baseVel = gabi::call<f32>(rd<u32>(table + 0x64), gabi::at<void>(sweep));
  f32 baseRandom =
      gabi::call<f32>(rd<u32>(table + 0x6c), gabi::at<void>(sweep));
  f32 speed =
      gabi::fmuls_ppc(baseVel, gabi::fmadds(baseRandom, randomRatio, 1.0f));
  f32 random[3];
  for (auto &v : random)
    v = particleRandomRF(e);
  f32 squared = lengthSquared(random[0], random[1], random[2]);
  if (squared > 0x1p-18f) {
    f32 scale = gabi::fmuls_ppc(inverseLength(squared), speed);
    for (auto &v : random)
      v = gabi::fmuls_ppc(v, scale);
  }
  f32 influence = gabi::call<f32>(vfn(sweep, 0x5c), gabi::at<void>(sweep));
  for (u32 i = 0; i < 3; ++i)
    p->baseVelocity[i] =
        gabi::fmadds(parent->baseVelocity[i], influence, random[i]);
  influence = gabi::call<f32>(vfn(sweep, 0x5c), gabi::at<void>(sweep));
  for (u32 i = 0; i < 3; ++i)
    p->fieldAcceleration[i] =
        gabi::fmuls_ppc(parent->fieldVelocity[i], influence);
  f32 gravity = gabi::call<f32>(vfn(sweep, 0x74), gabi::at<void>(sweep));
  p->acceleration[0] = 0.0f;
  p->acceleration[1] = -gravity;
  p->acceleration[2] = 0.0f;
  p->airResistance = parent->airResistance;
  p->moment = parent->moment;
  p->age = -1.0f;
  s32 life = gabi::call<s32>(vfn(sweep, 0x34), gabi::at<void>(sweep));
  p->lifetime = (f32)life;
  p->normalizedTime = 0.0f;
  f32 scale = gabi::fmuls_ppc(p->moment, p->drag);
  for (u32 i = 0; i < 3; ++i) {
    f32 field = p->fieldAcceleration[i];
    p->fieldVelocity[i] = field;
    p->velocity[i] =
        gabi::fmuls_ppc(gabi::fadds_ppc(p->baseVelocity[i], field), scale);
  }
  if (rd<u32>(e + 0x84) & 16)
    p->status = (u32)p->status | 0x20;
  for (u32 i = 0; i < 3; ++i)
    p->offset[i] = parent->offset[i];
  for (u32 i = 0; i < 3; ++i)
    p->local[i] = parent->local[i];
  f32 positionRandom = gabi::call<f32>(vfn(sweep, 0x54), gabi::at<void>(sweep));
  if (positionRandom != 0.0f) {
    s16 a = (s16)(particleRandomNext(e) >> 16),
        b = (s16)(particleRandomNext(e) >> 16);
    gabi::Local<be<f32>[3]> unit;
    gabi::call<void>(0x02824AA0, a, b, unit.get());
    f32 amount = gabi::fmuls_ppc(particleRandom(e), positionRandom);
    for (u32 i = 0; i < 3; ++i) {
      (*unit)[i] = gabi::fmuls_ppc((*unit)[i], amount);
      p->local[i] = gabi::fadds_ppc(p->local[i], (*unit)[i]);
    }
  }
  wr<u32>(base + 0xc8, rd<u32>(e + 0x1e8));
  wr<u32>(base + 0xd0, rd<u32>(gabi::ea(parent) + 0xd0));
  particleInitTail(p, e, sweep, true);
}
VERIFY(0x028260B4, JPA_particleInitChild);
