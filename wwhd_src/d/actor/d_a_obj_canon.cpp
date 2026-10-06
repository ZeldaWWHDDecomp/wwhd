#include "d/actor/d_a_obj_canon.h"
#include "bindings.h"
using Actor = daObj_Canon_c;
static constexpr u32 HIO = 0x10469460;
static constexpr u32 MTX = 0x1048D0CC;
static constexpr u32 SINCOS = 0x104A44F8;
template <class T> static T read(u32 p, u32 off) {
  return gabi::load<T>(p + off);
}
template <class T> static void write(u32 p, u32 off, T v) {
  gabi::store<T>(p + off, v);
}
static void *ptr(u32 p) { return gabi::at<void>(p); }
static u32 address(Actor *a, u32 off) { return gabi::ea(a) + off; }
static u32 animationMatrix(u32 model, u32 joint) {
  u32 buffer = read<u32>(model, 0x2C);
  u32 matrices = read<u32>(buffer, 0x10);
  u16 flags = read<u16>(buffer, 4);
  write<u16>(buffer, 4, flags | 0x10);
  return matrices + joint * 0x30;
}
static f32 sine(s16 a) { return gabi::load<f32>(SINCOS + ((u16(a) >> 3) * 8)); }
static f32 cosine(s16 a) {
  return gabi::load<f32>(SINCOS + ((u16(a) >> 3) * 8) + 4);
}
static void nodeControl(Actor *a, void *node, void *model) {
  WWHD_FUNC(0x0232FDF0, void, a, node, model);
  u32 joint = gabi::call<u32>(0x027F7878, node);
  u32 index = read<u16>(joint, 4);
  u32 matrix = animationMatrix(gabi::ea(model), index);
  gabi::call(0x028E90D4, ptr(matrix), ptr(MTX));
  gabi::Local<csXyz> rot;
  gabi::call(0x0201A478, rot.get(), s16(0), s16(0), s16(0));
  s16 x, y, z;
  if (gabi::load<s16>(0x1047BD4A)) {
    x = gabi::load<s16>(0x1047BD4C);
    y = gabi::load<s16>(0x1047BD4E);
    z = gabi::load<s16>(0x1047BD50);
  } else {
    x = -s16(a->pitch);
    y = rot->y;
    z = -s16(a->yaw);
  }
  rot->x = x;
  rot->y = y;
  rot->z = z;
  gabi::call(0x025F1B48, ptr(MTX), x, y, z);
  s16 phase = s16(s16(a->shakePhase) + gabi::load<s16>(0x1047BD4C) + 0x1830);
  s16 recoil = a->recoil;
  a->shakePhase = phase;
  s32 factor = s32(gabi::load<s16>(0x1047BD52)) + 5;
  s16 shake = s16(gabi::ftoi(f32(s32(recoil) * factor) * sine(phase)));
  gabi::call(0x025F1BF4, ptr(MTX), shake);
  f32 rise = a->rise;
  f32 tx = gabi::load<f32>(0x1047BCDC), tz = gabi::load<f32>(0x1047BCE4);
  gabi::call(0x025F24E0, tx, -rise, tz);
  gabi::call(0x028E90D4, ptr(MTX), ptr(0x104B4868));
  matrix = animationMatrix(gabi::ea(model), index);
  mtx_copy(gabi::at<Mtx34>(matrix), gabi::at<Mtx34>(MTX));
}
VERIFY(0x0232FDF0, nodeControl);
static s32 nodeCallback(void *node, s32 timing) {
  WWHD_FUNC(0x0232FFD4, s32, node, timing);
  if (timing == 0) {
    u32 model = gabi::load<u32>(0x104B462C);
    u32 actor = read<u32>(model, 0xB8);
    if (actor)
      nodeControl(gabi::at<Actor>(actor), node, ptr(model));
  }
  return 1;
}
VERIFY(0x0232FFD4, nodeCallback);
static s32 createHeap(Actor *a) {
  WWHD_FUNC(0x0233001C, s32, a);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10026BE0;
  name->__vtbl = 0x10026A78;
  u32 resources = gabi::load<u32>(0x101F4F28);
  u32 data = gabi::call<u32>(0x026066C4, ptr(resources), name.get(), s32(3));
  if (!data)
    gabi::call(0x0273AA24, STR(0x10026AB8), s32(0x115), STR(0x10026ACC));
  u32 model =
      gabi::call<u32>(0x025E38E0, ptr(data), u32(0x80000), u32(0x11000022));
  a->model = ptr(model);
  if (!model)
    return 0;
  write<u32>(model, 0xB8, gabi::ea(a));
  u32 count = read<u32>(data, 4), joints = read<u32>(data, 8);
  if (count > 3)
    joints += 0x54;
  write<u32>(joints, 8, 0x0232FFD4);
  return 1;
}
VERIFY(0x0233001C, createHeap);
static s32 heapCallback(Actor *a) {
  WWHD_FUNC(0x023300FC, s32, a);
  return createHeap(a);
}
VERIFY(0x023300FC, heapCallback);
static void getArg(Actor *a) {
  WWHD_FUNC(0x02330100, void, a);
  u32 param = a->mParameters;
  a->scaleArg = u8(param);
  a->appearSwitch = u8(param >> 8);
  a->pathIndex = u8(param >> 16);
  a->deathSwitch = u8(param >> 24);
  if (u8(param) != 255) {
    f32 scale = f32(gabi::fmadd(f64(u8(param)), 0.1, 1.0));
    a->scale.x = scale;
    a->scale.y = scale;
    a->scale.z = scale;
  }
}
VERIFY(0x02330100, getArg);
static void modeProc(Actor *a, s32 proc, s32 mode) {
  WWHD_FUNC(0x02330178, void, a, proc, mode);
  if (proc == 0) {
    a->mode = mode;
    ptmf_call(0x10026AF8 + u32(mode) * 20, a);
  } else if (proc == 1) {
    s32 current = a->mode;
    ptmf_call(0x10026B00 + u32(current) * 20, a);
  }
}
VERIFY(0x02330178, modeProc);
static void setMtx(Actor *a) {
  WWHD_FUNC(0x02330224, void, a);
  f32 x = a->scale.x, y = a->scale.y;
  u32 model = gabi::ea(a->model.get());
  f32 z = a->scale.z;
  write<f32>(model, 0xBC, x);
  write<f32>(model, 0xC0, y);
  write<f32>(model, 0xC4, z);
  x = a->current.pos.x;
  y = a->current.pos.y;
  z = a->current.pos.z;
  gabi::call(0x028E93CC, ptr(MTX), x, y, z);
  s16 rx = a->shape_angle.x, ry = a->shape_angle.y, rz = a->shape_angle.z;
  gabi::call(0x025F1B48, ptr(MTX), rx, ry, rz);
  model = gabi::ea(a->model.get());
  mtx_copy(gabi::at<Mtx34>(model + 0xC8), gabi::at<Mtx34>(MTX));
}
VERIFY(0x02330224, setMtx);
static void createInit(Actor *a) {
  WWHD_FUNC(0x02330304, void, a);
  u8 sw = a->deathSwitch;
  s32 mode = 0;
  if (sw != 255 &&
      gabi::call<s32>(0x025BA0C0, ptr(gabi::load<u32>(0x101F84DC) + 0x20),
                      s32(sw), s32(a->current.roomNo)))
    mode = 2;
  else {
    sw = a->appearSwitch;
    if (sw != 255 &&
        !gabi::call<s32>(0x025BA0C0, ptr(gabi::load<u32>(0x101F84DC) + 0x20),
                         s32(sw), s32(a->current.roomNo)))
      mode = 3;
  }
  modeProc(a, 0, mode);
  s8 room = a->current.roomNo;
  u8 path = a->pathIndex;
  a->health = 2;
  a->max_health = 2;
  u32 route = gabi::call<u32>(0x025AAF88, s32(path), s32(room));
  write<u32>(gabi::ea(a), 0x3CC, route);
  gabi::call(0x0259E730, ptr(address(a, 0x3B4)), ptr(route));
  setMtx(a);
  gabi::call(0x027F4D5C, a->model.get());
  u32 model = gabi::ea(a->model.get());
  f32 scale = a->scale.x;
  a->cullMtx = model ? model + 0xC8 : 0;
  gabi::call(0x025D674C, a, -100.f * scale, -100.f * scale, -10.f * scale,
             100.f * scale, 100.f * scale, 150.f * scale);
  write<f32>(gabi::ea(a), 0x364, 10.f);
  gabi::call(0x02515F14, ptr(address(a, 0x520)), u8(255), u8(255), a);
  gabi::call(0x0251677C, ptr(address(a, 0x3F4)), ptr(0x10026BEC));
  write<u32>(gabi::ea(a), 0x438, address(a, 0x520));
}
VERIFY(0x02330304, createInit);
static s32 create(Actor *a) {
  WWHD_FUNC(0x02330524, s32, a);
  u32 cond = a->actor_condition;
  if (!(cond & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      a->__vtbl = 0x10026A90;
      gabi::call(0x025166F0, ptr(address(a, 0x3F4)));
      gabi::call(0x0200BD2C, ptr(address(a, 0x520)));
      gabi::call(0x02515DA0, ptr(address(a, 0x53C)));
      write<u32>(gabi::ea(a), 0x538, 0x1004AE88);
      write<u32>(gabi::ea(a), 0x53C, 0x1004AEC0);
      gabi::call(0x025A5894, ptr(address(a, 0x58C)), u32(0), u32(0));
      cond = a->actor_condition;
    }
    a->actor_condition = cond | 8;
  }
  s32 result =
      gabi::call<s32>(0x02520460, ptr(address(a, 0x3D4)), STR(0x10026BE0));
  if (result == 4) {
    getArg(a);
    if (u8(a->pathIndex) == 255 ||
        !gabi::call<s32>(0x025D63E8, a, ptr(0x023300FC), u32(0x8C0)))
      return 5;
    createInit(a);
  }
  return result;
}
VERIFY(0x02330524, create);
static s32 createWrapper(Actor *a) {
  WWHD_FUNC(0x0233063C, s32, a);
  return create(a);
}
VERIFY(0x0233063C, createWrapper);
static s32 remove(Actor *a) {
  WWHD_FUNC(0x02330640, s32, a);
  gabi::call(0x025204C8, ptr(address(a, 0x3D4)), STR(0x10026BE0));
  u32 vt = read<u32>(gabi::ea(a), 0x58C), target = read<u32>(vt, 0x44);
  gabi::call_ptr<void>(target, ptr(address(a, 0x58C)));
  gabi::call(0x025E1B34, ptr(address(a, 0x56C)));
  return 1;
}
VERIFY(0x02330640, remove);
static s32 deleteWrapper(Actor *a) {
  WWHD_FUNC(0x02330698, s32, a);
  return remove(a);
}
VERIFY(0x02330698, deleteWrapper);
static void setAttention(Actor *a) {
  WWHD_FUNC(0x0233069C, void, a);
  f32 z = a->attention.z, y = a->attention.y, x = a->attention.x;
  write<f32>(gabi::ea(a), 0x398, z);
  write<f32>(gabi::ea(a), 0x394, y);
  write<f32>(gabi::ea(a), 0x390, x);
  f32 offset = read<f32>(HIO, 0xC);
  a->eyePos.z = z;
  a->eyePos.y = y;
  a->eyePos.x = x;
  write<f32>(gabi::ea(a), 0x394, y + offset);
  a->eyePos.y = y + read<f32>(HIO, 0x10);
}
VERIFY(0x0233069C, setAttention);
static void setCollision(Actor *a) {
  WWHD_FUNC(0x023306E4, void, a);
  f32 radius = (gabi::load<f32>(0x1047BCD0) + 80.f) * f32(a->scale.x);
  gabi::call(0x02018C8C, ptr(address(a, 0x50C)), radius);
  gabi::call(0x02018D40, ptr(address(a, 0x50C)), &a->attention);
  s32 mode = a->mode;
  if (mode != 2 && mode != 3) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, ptr(play + 0x26A4), ptr(address(a, 0x3F4)));
  }
}
VERIFY(0x023306E4, setCollision);
static s32 execute(Actor *a) {
  WWHD_FUNC(0x02330760, s32, a);
  gabi::call(0x0200F428, &a->recoil, s16(0), s16(10), s16(10));
  s16 pitch = a->targetPitch;
  gabi::call(0x0200F428, &a->pitch, pitch, s16(6), s16(0x300));
  s16 yaw = a->targetYaw;
  gabi::call(0x0200F428, &a->yaw, yaw, s16(6), s16(0x300));
  setAttention(a);
  setCollision(a);
  modeProc(a, 1, 5);
  gabi::call(0x027F4D5C, a->model.get());
  gabi::Local<cXyz> muzzle, attention;
  f32 scale = a->scale.x, height = read<f32>(HIO, 0x18),
      x = read<f32>(HIO, 0x14);
  muzzle->x = x;
  muzzle->y = height * scale;
  muzzle->z = read<f32>(HIO, 0x1C);
  u32 matrix = animationMatrix(gabi::ea(a->model.get()), 3);
  gabi::call(0x028E8F64, ptr(matrix), muzzle.get(), &a->muzzle);
  scale = a->scale.x;
  f32 y = gabi::load<f32>(0x1047BCD4) + 60.f;
  x = gabi::load<f32>(0x1047BCD0);
  u32 model = gabi::ea(a->model.get());
  attention->x = x;
  attention->z = gabi::load<f32>(0x1047BCD8);
  attention->y = y * scale;
  matrix = animationMatrix(model, 3);
  gabi::call(0x028E8F64, ptr(matrix), attention.get(), &a->attention);
  if (read<u32>(gabi::ea(a), 0x590) &&
      gabi::call<s32>(0x0211D2F8, &a->smokeTimer) == 0)
    gabi::call(0x025A5AC8, ptr(address(a, 0x58C)));
  setMtx(a);
  return 0;
}
VERIFY(0x02330760, execute);
static s32 executeWrapper(Actor *a) {
  WWHD_FUNC(0x023308E8, s32, a);
  return execute(a);
}
VERIFY(0x023308E8, executeWrapper);
static void debugDraw(Actor *a) {
  WWHD_FUNC(0x023308EC, void, a);
  if (!gabi::load<u32>(0x101FDA50)) {
    gabi::store<u32>(0x101FDA50, 1);
    gabi::call(0xC000A848, ptr(0x101FEBF4), ptr(0x10026A70), u32(4));
    if (!gabi::load<u32>(0x101FDA50)) {
      gabi::store<u32>(0x101FDA50, 1);
      gabi::call(0xC000A848, ptr(0x101FEBF4), ptr(0x10026A70), u32(4));
    }
  }
}
VERIFY(0x023308EC, debugDraw);
static s32 draw(Actor *a) {
  WWHD_FUNC(0x02330974, s32, a);
  if (read<u8>(HIO, 4))
    debugDraw(a);
  s32 mode = a->mode;
  u32 model = gabi::ea(a->model.get()), data = read<u32>(model, 0xAC);
  u32 count = read<u32>(data, 4), joints = read<u32>(data, 8);
  if (count > 3)
    joints += 0x54;
  u32 mesh = read<u32>(joints, 0x10), shape = read<u32>(mesh, 8);
  write<u8>(shape, 4, (mode == 2 || mode == 3) ? 0 : 1);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), s32(0), &a->current.pos,
             ptr(address(a, 0x110)));
  env = gabi::call<u32>(0x02555D0C);
  model = gabi::ea(a->model.get());
  gabi::call(0x02562F5C, ptr(env), ptr(model), ptr(address(a, 0x110)));
  gabi::call(0x025E2E5C, a->model.get());
  return 1;
}
VERIFY(0x02330974, draw);
static s32 drawWrapper(Actor *a) {
  WWHD_FUNC(0x02330A84, s32, a);
  return draw(a);
}
VERIFY(0x02330A84, drawWrapper);
static void waitInit(Actor *a) {
  WWHD_FUNC(0x02330A88, void, a);
  a->waitTimer = read<s16>(HIO, 0x3E);
}
VERIFY(0x02330A88, waitInit);
static void particle(u16 id, void *pos, void *angle, void *scale,
                     void *callback, void *color) {
  u32 play = gabi::call<u32>(0x025200D4), control = read<u32>(play, 0x5AB0);
  gabi::call(0x025A847C, ptr(control), u32(0), id, pos, angle, scale, u8(255),
             callback, s8(-1), color, color, u32(0));
}
static s32 checkHit(Actor *a) {
  WWHD_FUNC(0x02330A98, s32, a);
  gabi::call(0x02515E50, ptr(address(a, 0x53C)));
  if (read<u8>(HIO, 5)) {
    a->health = 0;
    modeProc(a, 0, 2);
    return 1;
  }
  if (gabi::call<s32>(0x0211D2F8, &a->hitTimer))
    return 0;
  u32 hit = gabi::call<u32>(0x02516300, ptr(address(a, 0x3F4)));
  gabi::Local<cXyz> pos;
  pos->copy(*gabi::at<cXyz>(address(a, 0x4C0)));
  gabi::call<u32>(0x02516300, ptr(address(a, 0x3F4)));
  if (!hit)
    return 0;
  u32 type = read<u32>(hit, 0x10);
  if (type == 0x20) {
    a->hitTimer = 5;
    a->health = u8(u8(a->health) - 2);
  } else if (type == 0x40) {
    s8 room = a->current.roomNo;
    a->recoil = 0x190;
    s32 reverb = gabi::call<s32>(0x02520540, s32(room));
    gabi::call(0x025E1A40, u32(0x2833), &a->eyePos, u32(0x20), reverb);
    a->hitTimer = 5;
    a->health = u8(u8(a->health) - 1);
  } else
    return 0;
  particle(0x10, pos.get(), nullptr, nullptr, nullptr, nullptr);
  if (s8(u8(a->health)) > 0) {
    modeProc(a, 0, 0);
    return 1;
  }
  u32 play = gabi::call<u32>(0x025200D4), player = read<u32>(play, 0x5B2C);
  gabi::Local<cXyz> scale;
  scale->x = 2.f;
  scale->y = 2.f;
  scale->z = 2.f;
  particle(0xF, pos.get(), ptr(player + 0x328), scale.get(), nullptr, nullptr);
  s32 reverb = gabi::call<s32>(0x02520540, s32(a->current.roomNo));
  gabi::call(0x025E1A40, u32(0x2828), &a->eyePos, u32(0), reverb);
  modeProc(a, 0, 2);
  return 1;
}
VERIFY(0x02330A98, checkHit);
static void waitMode(Actor *a) {
  WWHD_FUNC(0x02330D3C, void, a);
  if (checkHit(a))
    return;
  u32 play = gabi::call<u32>(0x025200D4), player = read<u32>(play, 0x5B2C);
  play = gabi::call<u32>(0x025200D4);
  if (read<u32>(play, 0x5CD8) & 0x1000000) {
    a->shots = 0;
    a->waitTimer = s32(read<s16>(HIO, 0x3E)) * 10;
  }
  play = gabi::call<u32>(0x025200D4);
  u32 ship = read<u32>(play, 0x5B3C);
  if (ship && (read<u32>(ship, 0x644) & 0x20000))
    a->shots = s32(u32(s32(a->shots)) + 1);
  if (gabi::call<s32>(0x0211D2F8, &a->waitTimer))
    return;
  if (!gabi::call<s32>(0x0259F220, ptr(address(a, 0x3B4)),
                       ptr(player + 0x314))) {
    a->targetPitch = 0;
    a->targetYaw = 0;
    a->waitTimer = read<s16>(HIO, 0x3E);
    return;
  }
  s16 prediction = s16(gabi::ftoi(read<f32>(HIO, 8)));
  gabi::Local<cXyz> difference, horizontal;
  gabi::call(0x0201ADE0, &a->current.pos, difference.get(), &a->target);
  horizontal->x = difference->x;
  horizontal->y = 0.f;
  horizontal->z = difference->z;
  f32 sq = gabi::call<f32>(0x028E8DD0, horizontal.get());
  f32 distance = gabi::call<f32>(0x028F4384, sq);
  f32 threshold = read<f32>(HIO, 0x40);
  if (distance > threshold)
    prediction = s16(gabi::ftoi(gabi::fmadds(
        distance - threshold, read<f32>(HIO, 0x44), f32(prediction))));
  play = gabi::call<u32>(0x025200D4);
  player = read<u32>(play, 0x5B2C);
  a->target.copy(*gabi::at<cXyz>(player + 0x314));
  if (!gabi::call<s32>(0x0259F220, ptr(address(a, 0x3B4)), &a->target))
    return;
  play = gabi::call<u32>(0x025200D4);
  if (read<u32>(play, 0x5CD8) & 0x1100000) {
    a->shots = 0;
    prediction = s16(gabi::ftoi(f32(prediction) + 3000.f));
  } else if (distance > read<f32>(HIO, 0x40) && s32(a->shots) < 6)
    prediction = s16(gabi::ftoi(gabi::fmadds(
        f32(s32(u32(6) - u32(s32(a->shots)))), 200.f, f32(prediction))));
  play = gabi::call<u32>(0x025200D4);
  player = read<u32>(play, 0x5B2C);
  s16 angle = gabi::call<s16>(0x025D6894, a, ptr(player));
  f32 x = a->target.x;
  a->target.x = gabi::fnmsubs(f32(prediction), sine(angle), x);
  f32 z = a->target.z;
  a->target.z = gabi::fnmsubs(f32(prediction), cosine(angle), z);
  a->waitTimer = read<s16>(HIO, 0x3E);
  modeProc(a, 0, 1);
}
VERIFY(0x02330D3C, waitMode);
static void attackInit(Actor *a) {
  WWHD_FUNC(0x0233112C, void, a);
  a->waitTimer = read<s16>(HIO, 0x3C);
}
VERIFY(0x0233112C, attackInit);
static void lockon(Actor *a) {
  WWHD_FUNC(0x0233113C, void, a);
  gabi::Local<cXyz> difference, horizontal;
  gabi::call(0x0201ADE0, &a->current.pos, difference.get(), &a->target);
  horizontal->x = difference->x;
  horizontal->y = 0.f;
  horizontal->z = difference->z;
  f32 sq = gabi::call<f32>(0x028E8DD0, horizontal.get());
  f32 distance = gabi::call<f32>(0x028F4384, sq);
  s16 yaw = gabi::call<s16>(0x0200F93C, &a->current.pos, &a->target);
  a->targetYaw = s16(yaw - s16(a->shape_angle.y));
  f32 threshold = read<f32>(HIO, 0x2C);
  s16 bias = 0;
  if (distance > threshold) {
    bias = s16(gabi::ftoi(distance - threshold));
    s16 maximum = read<s16>(HIO, 0x30);
    if (bias > maximum)
      bias = maximum;
  }
  s16 pitch = gabi::call<s16>(0x0200F974, &a->current.pos, &a->target);
  pitch = s16(pitch + bias);
  a->targetPitch = pitch;
  s16 low = read<s16>(HIO, 0x34), high = read<s16>(HIO, 0x32);
  if (pitch < low)
    pitch = low;
  else if (pitch > high)
    pitch = high;
  a->targetPitch = pitch;
  yaw = a->targetYaw;
  low = read<s16>(HIO, 0x38);
  high = read<s16>(HIO, 0x36);
  if (yaw < low)
    yaw = low;
  else if (yaw > high)
    yaw = high;
  a->targetYaw = yaw;
}
VERIFY(0x0233113C, lockon);
static void attack(Actor *a) {
  WWHD_FUNC(0x02331298, void, a);
  gabi::Local<csXyz> angle;
  angle->x = s16(s16(a->shape_angle.x) - s16(a->pitch));
  angle->y = s16(s16(a->shape_angle.y) + s16(a->yaw));
  angle->z = a->shape_angle.z;
  u32 params = gabi::call<u32>(0x020CB8D8, s32(4), s32(1), s32(1));
  s8 room = read<s8>(gabi::ea(a), 0x1C9);
  u32 bomb = gabi::call<u32>(0x025D5928, s16(0x126), params, &a->muzzle, room,
                             angle.get(), nullptr, s32(-1), u32(0), u32(0));
  if (!bomb)
    return;
  s16 noGravity = read<s16>(HIO, 0x28);
  gabi::call(0x020CB89C, ptr(bomb), noGravity);
  s16 pitch = angle->x;
  write<f32>(bomb, 0x370, cosine(pitch) * read<f32>(HIO, 0x20));
  pitch = angle->x;
  write<f32>(bomb, 0x340, -(sine(pitch) * read<f32>(HIO, 0x20)));
  write<f32>(bomb, 0x374, read<f32>(HIO, 0x24));
  a->recoil = 0xC8;
  gabi::call(0x025E19CC, u32(0x2852), &a->muzzle);
}
VERIFY(0x02331298, attack);
static void attackMode(Actor *a) {
  WWHD_FUNC(0x023313B4, void, a);
  if (!checkHit(a)) {
    lockon(a);
    if (!gabi::call<s32>(0x0211D2F8, &a->waitTimer)) {
      attack(a);
      modeProc(a, 0, 0);
    }
  }
}
VERIFY(0x023313B4, attackMode);
static void effect(Actor *a, u16 id) {
  WWHD_FUNC(0x02331418, void, a, id);
  void *color = id == 0x82E4 ? ptr(address(a, 0x1A8)) : nullptr;
  void *callback = id == 0x3E1 ? ptr(address(a, 0x58C)) : nullptr;
  particle(id, &a->current.pos, &a->shape_angle, &a->scale, callback, color);
  if (id == 0x3E1)
    a->smokeTimer = 0xE6;
}
VERIFY(0x02331418, effect);
static void deleteInit(Actor *a) {
  WWHD_FUNC(0x02331558, void, a);
  u8 sw = a->deathSwitch;
  a->recoil = 0;
  if (sw != 255) {
    s8 room = a->current.roomNo;
    u32 save = gabi::load<u32>(0x101F84DC);
    gabi::call(0x025B9E38, ptr(save + 0x20), s32(sw), s32(room));
  }
  effect(a, 0x82E4);
  effect(a, 0x82E5);
  effect(a, 0x82E6);
  effect(a, 0x3E1);
  a->actor_status = u32(a->actor_status) & ~u32(0x3F);
}
VERIFY(0x02331558, deleteInit);
static void switchWaitInit(Actor *a) {
  WWHD_FUNC(0x023315F8, void, a);
  a->actor_status = u32(a->actor_status) & ~u32(0x3F);
  a->rise = 120.f;
}
VERIFY(0x023315F8, switchWaitInit);
static void switchWait(Actor *a) {
  WWHD_FUNC(0x02331614, void, a);
  u8 sw = a->appearSwitch;
  if (sw != 255) {
    s8 room = a->current.roomNo;
    u32 save = gabi::load<u32>(0x101F84DC);
    if (gabi::call<s32>(0x025BA0C0, ptr(save + 0x20), s32(sw), s32(room)))
      modeProc(a, 0, 4);
  }
}
VERIFY(0x02331614, switchWait);
static void appearInit(Actor *a) {
  WWHD_FUNC(0x02331678, void, a);
  u32 status = a->actor_status;
  a->recoil = 0x258;
  a->rise = 120.f;
  a->actor_status = (status & ~u32(0x3F)) | 0x35;
}
VERIFY(0x02331678, appearInit);
static void appear(Actor *a) {
  WWHD_FUNC(0x023316A0, void, a);
  gabi::call(0x0200ED84, &a->rise, 0.f, 0.1f, 1.f);
  if (f32(a->rise) < 2.f) {
    a->rise = 0.f;
    modeProc(a, 0, 0);
  }
}
VERIFY(0x023316A0, appear);
static void *hioConstructor(void *self) {
  WWHD_FUNC(0x0233172C, void *, self);
  u32 p = gabi::ea(self);
  if (!p) {
    p = gabi::call<u32>(0x0273AD10, u32(0x4C));
    if (!p)
      return nullptr;
  }
  write<f32>(p, 0x24, -1.1f);
  write<s16>(p, 0x3A, 0x2800);
  write<s16>(p, 0x32, 0x2328);
  write<f32>(p, 0x20, 90.f);
  write<s16>(p, 0x36, 0x2328);
  write<f32>(p, 8, 150.f);
  write<s16>(p, 0x28, 0x16);
  write<f32>(p, 0x18, 180.f);
  write<s16>(p, 0x3E, 30);
  write<f32>(p, 0x44, 0.2f);
  write<s16>(p, 0x3C, 30);
  write<u32>(p, 0, 0x10026AA0);
  write<f32>(p, 0x2C, 5000.f);
  write<f32>(p, 0x40, 2000.f);
  write<s16>(p, 0x38, -0x2328);
  write<s16>(p, 0x48, 0);
  write<u8>(p, 4, 0);
  write<u8>(p, 5, 0);
  write<s16>(p, 0x30, 0x3000);
  write<f32>(p, 0x1C, 0.f);
  write<f32>(p, 0x14, 0.f);
  write<s16>(p, 0x34, -0x2328);
  return ptr(p);
}
VERIFY(0x0233172C, hioConstructor);
static void staticInit() {
  WWHD_FUNC(0x0233181C, void);
  gabi::store<u32>(0x10469458, 0);
  gabi::store<u32>(0x10469450, 0);
  gabi::store<u32>(0x1046945C, 0);
  gabi::store<u32>(0x10469454, 0);
  gabi::call(0x028F026C, ptr(0x101C8960));
  gabi::store<f32>(0x10469444, -3.1415927410125732f);
  gabi::store<f32>(0x10469448, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046944C));
  gabi::call(0x028F026C, ptr(0x101C896C));
  gabi::call(0x028EAB2C, ptr(0x1046944D));
  gabi::call(0x028F026C, ptr(0x101C8978));
  hioConstructor(ptr(HIO));
}
VERIFY(0x0233181C, staticInit);
static void deletingDestructor(void *self, s32 flags) {
  WWHD_FUNC(0x023318BC, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x023318BC, deletingDestructor);
static s32 isDelete(void *a) {
  WWHD_FUNC(0x023318D0, s32, a);
  return 1;
}
VERIFY(0x023318D0, isDelete);

/* ---- leftover functions of the translation unit ---- */

/* 023318D8 daObj_Canon_c::modeDelete (empty; mode table entry 10026B2C, as on GameCube) */
static void canon_modeDelete(void* p) {
    WWHD_FUNC(0x023318D8, void, p);
}
VERIFY(0x023318D8, canon_modeDelete);

/* 023318DC daObj_Canon_c::~daObj_Canon_c (deleting; vtable slot 10026A9C) */
static void daObj_Canon_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023318DC, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::call(0x02515860, t + 0x520, 2); /* dCcD_Stts */
        gabi::call(0x02515AE8, t + 0x3F4, 2); /* dCcD_Sph */
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x023318DC, daObj_Canon_c_dt);

/* 02331948 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10026A8C, after the destructor 023318BC */
static void canon_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02331948, void, p);
}
VERIFY(0x02331948, canon_SafeString_assureTermination);
