/* NPC path-following and runaway utility; derived WWHD source. */
#include "d/actor/d_a_npc_mk_static.h"
#include <cstdlib>

static cXyz *mk_pos(fopAc_ac_c *a) {
  return gabi::at<cXyz>(gabi::ea(a) + 0x314);
}
static void mk_floatCopy(cXyz *d, const cXyz *s) {
  f32 x = s->x, y = s->y, z = s->z;
  d->x = x;
  d->y = y;
  d->z = z;
}
static void mk_point(MkPath_l *p, cXyz *out, u8 index) {
  gabi::call(0x0259E778, p, out, index);
}
static void mk_inc(MkPath_l *p) { gabi::call(0x0259EB60, p); }
static void mk_dec(MkPath_l *p) { gabi::call(0x0259EC50, p); }
static void mk_info(MkPath_l *p, void *path) {
  gabi::call(0x0259E730, p, path);
}
static s16 mk_angle(cXyz *a, cXyz *b) {
  return gabi::call<s16>(0x0200F93C, a, b);
}
static f32 mk_distance(cXyz *a, cXyz *b) {
  gabi::Local<cXyz> diff, flat;
  gabi::call(0x0201ADE0, a, diff.get(), b);
  flat->x = diff->x;
  flat->y = 0.0f;
  flat->z = diff->z;
  f32 sq = gabi::call<f32>(0x028E8DD0, flat.get());
  return gabi::call<f32>(0x028F4384, sq);
}
static bool mk_segment(fopAc_ac_c *player, cXyz *a, cXyz *b) {
  gabi::Local<cXyz> hit;
  gabi::Local<be<f32>> distance;
  f32 ax = a->x, az = a->z, bx = b->x, bz = b->z;
  f32 px = mk_pos(player)->x, pz = mk_pos(player)->z;
  s32 ok = gabi::call<s32>(0x020109FC, px, pz, ax, az, bx, bz, &hit->x, &hit->z,
                           distance.get());
  return ok && (f32)*distance < 10000.0f;
}
static fopAc_ac_c *mk_player() {
  u32 play = gabi::call<u32>(0x025200D4);
  return gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B34));
}

u32 daNpc_Mk_Static_c::turnPath(fopAc_ac_c *actor, MkPath_l *path, u8 options) {
  WWHD_FUNC(0x0229A8EC, u32, this, actor, path, options);
  gabi::Local<cXyz> point, pos;
  gabi::Local<be<s16>> angle;
  mk_point(path, point.get(), path->index);
  mk_floatCopy(pos.get(), mk_pos(actor));
  // HD getPoint's return is copied through floating point registers.
  gabi::Local<cXyz> target;
  mk_floatCopy(target.get(), point.get());
  gabi::call(0x0259D624, pos.get(), target.get(), (void *)nullptr, angle.get());
  be<s16> *yaw = gabi::at<be<s16>>(gabi::ea(actor) + 0x322);
  if (options & 2)
    gabi::call(0x0200F378, yaw, (s16)*angle, 2, 0x2000, 0x400);
  else
    gabi::call(0x0200F378, yaw, (s16)*angle, 8, 0x800, 0x200);
  f32 cosine = cM_scos((s16)*yaw - (s16)*angle);
  if (cosine < 0.0f)
    speedScale = gabi::fadds_ppc(cosine, 1.0f);
  return 0;
}
VERIFY(0x0229A8EC, &daNpc_Mk_Static_c::turnPath);

BOOL daNpc_Mk_Static_c::chkPath(fopAc_ac_c *actor, MkPath_l *path, u8 options) {
  WWHD_FUNC(0x0229AA38, BOOL, this, actor, path, options);
  gabi::Local<cXyz> pos;
  mk_floatCopy(pos.get(), mk_pos(actor));
  if (!gabi::call<s32>(0x0259E838, path, pos.get(), (options & 1) ^ 1))
    return FALSE;
  if (options & 1)
    mk_dec(path);
  else
    mk_inc(path);
  oldPointIndex = pointIndex;
  pointIndex = path->index;
  return TRUE;
}
VERIFY(0x0229AA38, &daNpc_Mk_Static_c::chkPath);
BOOL daNpc_Mk_Static_c::walkPath(fopAc_ac_c *actor, MkPath_l *path,
                                 u8 options) {
  WWHD_FUNC(0x0229AB24, BOOL, this, actor, path, options);
  BOOL result = chkPath(actor, path, options);
  turnPath(actor, path, options);
  return result;
}
VERIFY(0x0229AB24, &daNpc_Mk_Static_c::walkPath);
void daNpc_Mk_Static_c::aroundWalk(fopAc_ac_c *actor, fopAc_ac_c *target,
                                   u8 options) {
  WWHD_FUNC(0x0229AB78, void, this, actor, target, options);
  gabi::Local<cXyz> a, b;
  gabi::Local<be<s16>> angle;
  mk_floatCopy(a.get(), mk_pos(actor));
  mk_floatCopy(b.get(), mk_pos(target));
  gabi::call(0x0259D624, a.get(), b.get(), (void *)nullptr, angle.get());
  if (options) {
    *angle = (s16)((s16)*angle + 0x3000);
    gabi::call(0x0200F378, gabi::at<be<s16>>(gabi::ea(actor) + 0x322),
               (s16)*angle, 2, 0x800, 0x400);
  } else {
    f32 distance = mk_distance(mk_pos(actor), mk_pos(target));
    int offset =
        distance > 150.0f ? 0x3400 : (distance < 120.0f ? 0x4000 : 0x3A00);
    *angle = (s16)((s16)*angle + offset);
    gabi::call(0x0200F378, gabi::at<be<s16>>(gabi::ea(actor) + 0x322),
               (s16)*angle, 8, 0x800, 0x200);
  }
}
VERIFY(0x0229AB78, &daNpc_Mk_Static_c::aroundWalk);
f32 daNpc_Mk_Static_c::getSpeedF(f32 normal, f32 running) {
  WWHD_FUNC(0x0229ACC0, f32, this, normal, running);
  f32 speed = normal;
  if (accelerating) {
    if ((u16)timer >= ((u16)timerMax >> 1))
      speed = running;
    else {
      f32 delta = gabi::fsubs_ppc(running, normal);
      f32 factor = 2.0f / (f32)(u16)timerMax;
      speed =
          gabi::fmadds(gabi::fmuls_ppc(delta, factor), (f32)(u16)timer, normal);
    }
  }
  f32 result = gabi::fmuls_ppc(speed, (f32)speedScale);
  speedScale = 1.0f;
  return result;
}
VERIFY(0x0229ACC0, &daNpc_Mk_Static_c::getSpeedF);
void daNpc_Mk_Static_c::init(u8 wait, u16 duration) {
  WWHD_FUNC(0x0229AD60, void, this, wait, duration);
  state = 0;
  cooldown = wait;
  timer = duration;
  timerMax = duration;
  accelerating = 0;
  speedScale = 1.0f;
}
VERIFY(0x0229AD60, &daNpc_Mk_Static_c::init);
void daNpc_Mk_Static_c::runaway_com2(MkPath_l *path, u8 direction) {
  WWHD_FUNC(0x0229AD88, void, this, path, direction);
  path->index = oldPointIndex;
  if (direction == 1)
    mk_inc(path);
  else
    mk_dec(path);
  pointIndex = path->index;
}
VERIFY(0x0229AD88, &daNpc_Mk_Static_c::runaway_com2);
u8 daNpc_Mk_Static_c::goFarLink_2(fopAc_ac_c *actor, MkPath_l *source) {
  WWHD_FUNC(0x0229ADEC, u8, this, actor, source);
  fopAc_ac_c *player = mk_player();
  gabi::Local<MkPath_l> path;
  void *resource = source->path;
  mk_info(path.get(), resource);
  gabi::Local<cXyz> origin, next, prev;
  path->index = oldPointIndex;
  mk_point(path.get(), origin.get(), path->index);
  mk_inc(path.get());
  mk_point(path.get(), next.get(), path->index);
  mk_dec(path.get());
  mk_dec(path.get());
  mk_point(path.get(), prev.get(), path->index);
  s16 playerAngle = mk_angle(origin.get(), mk_pos(player));
  s16 nextAngle = mk_angle(origin.get(), next.get());
  s16 prevAngle = mk_angle(origin.get(), prev.get());
  int dn = (int)nextAngle - playerAngle, dp = (int)prevAngle - playerAngle;
  if (std::abs(dn) < 0x1800 && mk_segment(player, origin.get(), next.get()))
    return 2;
  if (std::abs(dp) < 0x1800 && mk_segment(player, origin.get(), prev.get()))
    return 1;
  u8 old = oldPointIndex;
  if (gabi::call<s32>(0x0259F0F8, path.get(), mk_pos(player), old, 5)) {
    int delta = (int)(u8)path->index - (u8)oldPointIndex;
    return ((delta > 0) == (std::abs(delta) <= 5)) ? 2 : 1;
  }
  return std::abs(dn) < std::abs(dp) ? 2 : 1;
}
VERIFY(0x0229ADEC, &daNpc_Mk_Static_c::goFarLink_2);
u8 daNpc_Mk_Static_c::goFarLink_3(fopAc_ac_c *actor, MkPath_l *source) {
  WWHD_FUNC(0x0229B010, u8, this, actor, source);
  fopAc_ac_c *player = mk_player();
  gabi::Local<MkPath_l> path;
  void *resource = source->path;
  mk_info(path.get(), resource);
  u8 old = oldPointIndex;
  if (old == (u8)pointIndex)
    return state;
  gabi::Local<cXyz> a, b;
  mk_point(path.get(), a.get(), old);
  mk_point(path.get(), b.get(), pointIndex);
  s16 playerAngle = mk_angle(mk_pos(actor), mk_pos(player));
  s16 pathAngle = mk_angle(a.get(), b.get());
  u8 current = state;
  if ((current == 1 || current == 2) &&
      std::abs((int)pathAngle - playerAngle) < 0x1800) {
    if (mk_segment(player, a.get(), b.get()))
      return (u8)state == 1 ? 2 : 1;
    return state;
  }
  return current;
}
VERIFY(0x0229B010, &daNpc_Mk_Static_c::goFarLink_3);
u8 daNpc_Mk_Static_c::runAwayProc(fopAc_ac_c *actor, MkPath_l *path,
                                  void *cylinder, be<s16> *outAngle) {
  WWHD_FUNC(0x0229B164, u8, this, actor, path, cylinder, outAngle);
  fopAc_ac_c *player = mk_player();
  fopAc_ac_c *hit = gabi::call<fopAc_ac_c *>(
      0x02515BBC, gabi::at<void>(gabi::ea(cylinder) + 0xDC));
  if (hit == player) {
    *outAngle = mk_angle(mk_pos(player), mk_pos(actor));
    return 4;
  }
  u8 wait = cooldown;
  if (wait)
    cooldown = wait - 1;
  u8 current = state;
  switch (current) {
  case 0: {
    f32 distance = gabi::call<f32>(0x0259EFE0, path, mk_pos(actor));
    u8 index = path->index;
    pointIndex = index;
    oldPointIndex = index;
    return distance < 50.0f ? 3 : 5;
  }
  case 5: {
    gabi::Local<cXyz> point, copy;
    mk_point(path, point.get(), pointIndex);
    copy->copy(*point);
    if (mk_distance(copy.get(), mk_pos(actor)) < 20.0f)
      return 3;
    turnPath(actor, path, 2);
    return state;
  }
  case 3: {
    f32 distance = mk_distance(mk_pos(player), mk_pos(actor));
    if (distance < 800.0f) {
      cooldown = 10;
      u8 direction = goFarLink_2(actor, path);
      runaway_com2(path, direction);
      return direction;
    }
    u16 time = timer;
    if ((int)time + 3 < (u16)timerMax) {
      u8 result = state;
      timer = time + 3;
      return result;
    }
    return state;
  }
  case 1:
  case 2: {
    f32 distance = mk_distance(mk_pos(player), mk_pos(actor));
    u8 option = (u8)state == 1 ? 2 : 3;
    if (walkPath(actor, path, option)) {
      if (distance > 900.0f)
        return 3;
      u8 direction = goFarLink_2(actor, path);
      if (direction != (u8)state)
        runaway_com2(path, direction);
      return direction;
    }
    if (!(u8)cooldown) {
      cooldown = 10;
      u8 direction = goFarLink_3(actor, path);
      if (direction != (u8)state) {
        oldPointIndex = pointIndex;
        runaway_com2(path, direction);
      }
      return direction;
    }
    if (accelerating) {
      u16 time = timer;
      if (time)
        timer = time - 1;
      if (distance > 400.0f) {
        u8 result = state;
        accelerating = 0;
        return result;
      }
    } else if (distance < 200.0f)
      accelerating = 1;
    return state;
  }
  default:
    return current;
  }
}
VERIFY(0x0229B164, &daNpc_Mk_Static_c::runAwayProc);
BOOL daNpc_Mk_Static_c::chkGameSet() {
  WWHD_FUNC(0x0229B54C, BOOL, this);
  for (u16 flag : {(u16)0x20, (u16)0x10, (u16)8, (u16)4}) {
    u32 save = gabi::load<u32>(0x101F84DC);
    if (!gabi::call<s32>(0x025B8B94, save + 0x1178, flag))
      return FALSE;
  }
  return TRUE;
}
VERIFY(0x0229B54C, &daNpc_Mk_Static_c::chkGameSet);
void daNpc_Mk_Static_c::setRndPathPos(fopAc_ac_c *actor, MkPath_l *path) {
  WWHD_FUNC(0x0229B5F0, void, this, actor, path);
  gabi::Local<dBgS_GndChk> ground;
  u32 g = gabi::ea(ground.get());
  gabi::call(0x02008E0C, ground.get());
  gabi::store<u32>(g + 0x50, 1);
  gabi::store<u32>(g + 0x10, 0x1001E554);
  for (int i = 0x44; i <= 0x4A; i++)
    gabi::store<u8>(g + i, 0);
  gabi::store<u32>(g + 4, g + 0x4C);
  gabi::store<u32>(g, g + 0x40);
  if (path->path) {
    gabi::store<u32>(g + 0x40, 0x1001E584);
    gabi::store<u32>(g + 0x20, 0x1001E564);
    gabi::store<u32>(g + 0x4C, 0x1001E574);
    s32 max = gabi::call<s32>(0x0259EDB8, path);
    f32 random = gabi::call<f32>(0x020198D8, (f32)max);
    gabi::Local<cXyz> point;
    mk_point(path, point.get(), (u8)gabi::ftoi(random));
    f32 y = point->y;
    mk_pos(actor)->y = y;
    gabi::store<u32>(g + 0x30, gabi::load<u32>(g + 0x30) & ~2u);
    f32 high = gabi::fadds_ppc(y, 200.0f), z = point->z, x = point->x;
    mk_pos(actor)->z = z;
    gabi::store<f32>(g + 0x24, x);
    gabi::store<f32>(g + 0x28, high);
    gabi::store<f32>(g + 0x2C, z);
    mk_pos(actor)->x = x;
    u32 play = gabi::call<u32>(0x025200D4);
    f32 floor = gabi::call<f32>(0x02008974, play + 0x12A0, ground.get());
    f32 finalX = mk_pos(actor)->x, finalZ = mk_pos(actor)->z;
    gabi::store<f32>(gabi::ea(actor) + 0x300, finalX);
    gabi::store<f32>(gabi::ea(actor) + 0x304, floor);
    gabi::store<f32>(gabi::ea(actor) + 0x308, finalZ);
    mk_pos(actor)->y = floor;
  }
  gabi::store<u32>(g + 0x20, 0x1001E564);
  gabi::store<u32>(g + 0x40, 0x1001E584);
  gabi::store<u32>(g + 0x4C, 0x1001E544);
  gabi::call(0x02008DAC, ground.get(), 0);
}
VERIFY(0x0229B5F0, &daNpc_Mk_Static_c::setRndPathPos);
BOOL daNpc_Mk_Static_c::chkPointPass(cXyz *a, cXyz *b, cXyz *c) {
  WWHD_FUNC(0x0229B768, BOOL, this, a, b, c);
  f32 bx = b->x;
  if ((f32)a->x == bx && (f32)a->z == (f32)b->z)
    return TRUE;
  if ((f32)c->x == bx && (f32)c->z == (f32)b->z)
    return TRUE;
  s16 first = mk_angle(a, b);
  s16 second = mk_angle(c, b);
  return cM_scos((int)first - second) < 0.0f;
}
VERIFY(0x0229B768, &daNpc_Mk_Static_c::chkPointPass);
static void __sinit_d_a_npc_mk_static_cpp() {
  WWHD_FUNC(0x0229B84C, void, (u32)0);
  for (int i = 0; i < 4; i++)
    gabi::store<u32>(0x10467C94 + 4 * i, 0);
  __register_global_object(0x101C1FF8);
  gabi::store<f32>(0x10467C88, -3.1415927410125732f);
  gabi::store<f32>(0x10467C8C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, 0x10467C90u);
  __register_global_object(0x101C2004);
  gabi::call(0x028EAB2C, 0x10467C91u);
  __register_global_object(0x101C2010);
}
VERIFY(0x0229B84C, __sinit_d_a_npc_mk_static_cpp);
