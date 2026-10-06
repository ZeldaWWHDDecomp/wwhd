/* Windfall ferris wheel: GameCube reconstruction corrected against WWHD
 * disassembly. */
#include "d/actor/d_a_obj_ferris.h"
namespace daObjFerris {
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
BOOL Act_c::set_event(s16 id) {
  WWHD_FUNC(0x0233E204, BOOL, this, id);
  if (mEventState != 0)
    return FALSE;
  mEventIdx = id;
  mEventState = 1;
  return TRUE;
}
VERIFY(0x0233E204, &Act_c::set_event);
bool Act_c::now_event(s16 id) {
  WWHD_FUNC(0x0233E22C, bool, this, id);
  return mEventState != 0 && mEventIdx == id;
}
VERIFY(0x0233E22C, &Act_c::now_event);
void Act_c::angle_mng() {
  WWHD_FUNC(0x0233E254, void, this);
  s16 angle = (s16)(mGondolaWaveTimer + 500);
  mGondolaWaveTimer = angle;
  f32 sine = gabi::load<f32>(0x104A44F8 + ((u16)angle >> 3) * 8);
  mGondolaWaveAngle = (s16)gabi::ftoi(380.0f * sine);
}
VERIFY(0x0233E254, &Act_c::angle_mng);
void ride_call_back(u8 *bg, Act_c *self, fopAc_ac_c *rider) {
  WWHD_FUNC(0x0233DD10, void, bg, self, rider);
  for (s32 i = 0; i < 5; i++)
    if (self->mpBgW[i] == bg) {
      self->mRidePos.copy(rider->current.pos);
      self->mRideState[i] = 1;
      break;
    }
}
VERIFY(0x0233DD10, ride_call_back);
BOOL Act_c::remove() {
  WWHD_FUNC(0x0233E174, BOOL, this);
  if (heap != nullptr)
    for (s32 i = 0; i < 6; i++) {
      u32 bg = gabi::ea((u8 *)mpBgW[i]);
      if (bg && gabi::load<u32>(bg) < 0x100) {
        gabi::store<u32>(bg + 0xB0, 0);
        u32 p = play();
        u8 *w = mpBgW[i];
        gabi::call(0x020087EC, ptr(p + 0x12A0), w);
      }
    }
  gabi::call(0x025204C8, mPhs, ptr(0x10028250));
  return TRUE;
}
VERIFY(0x0233E174, &Act_c::remove);
void Act_c::exe_event() {
  WWHD_FUNC(0x0233E668, void, this);
  if (mEventState == 1) {
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2)
      mEventState = 2;
    else {
      s16 id = mEventIdx;
      gabi::call(0x025D7A58, this, id, 255, 65535, 0, 1);
      u32 a = gabi::ea(this) + 0xFA;
      gabi::store<u16>(a, gabi::load<u16>(a) | 2);
    }
  } else if (mEventState == 2) {
    s16 id = mEventIdx;
    u32 p = play();
    if (gabi::call<s32>(0x025440C8, ptr(p + 0x52C4), id)) {
      p = play();
      gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
      mEventIdx = -1;
      mEventState = 0;
    }
  }
}
VERIFY(0x0233E668, &Act_c::exe_event);
BOOL Act_c::execute() {
  WWHD_FUNC(0x0233EDE0, BOOL, this);
  s16 speed = mRotSpeed;
  f32 volume = ((f32)speed / 45.0f) * 100.0f;
  mRotAngle = (s16)(mRotAngle + speed);
  if (volume > 100.0f)
    volume = 100.0f;
  gabi::call(0x025E1A04, 0x107D, &current.pos, (u32)gabi::ftoi(volume));
  gabi::call(0x0233E2A0, this);
  exe_event();
  gabi::call(0x0233E770, this);
  for (s32 i = 0; i < 6; i++) {
    gabi::call(0x0233D6B8, this, i);
    u8 *bg = mpBgW[i];
    gabi::call(0x024F43DC, bg);
  }
  gabi::call(0x0233E9C4, this);
  mFrameTimer = (s16)(mFrameTimer + 1);
  return TRUE;
}
VERIFY(0x0233EDE0, &Act_c::execute);
BOOL Act_c::draw() {
  WWHD_FUNC(0x0233EF4C, BOOL, this);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &current.pos, &tevStr);
  u32 p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D70));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D74));
  for (s32 i = 0; i < 6; i++) {
    env = gabi::call<u32>(0x02555D0C);
    u8 *model = mpModel[i];
    gabi::call(0x02562F5C, ptr(env), model, &tevStr);
    model = mpModel[i];
    gabi::call(0x025E2DE0, model, 0);
  }
  p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
  return TRUE;
}
VERIFY(0x0233EF4C, &Act_c::draw);
u32 param(Act_c *self, u32 width, u32 shift) {
  WWHD_FUNC(0x0233F2D4, u32, self, width, shift);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 bits = (shift & 32)
                 ? 0
                 : (gabi::load<u32>(gabi::ea(self) + 0xB0) >> (shift & 31));
  return bits & (mask - 1);
}
VERIFY(0x0233F2D4, param);
void Act_c::init_mtx() {
  WWHD_FUNC(0x0233D97C, void, this);
  for (s32 i = 0; i < 6; i++) {
    f32 z = scale.z, x = scale.x;
    u32 model = gabi::ea((u8 *)mpModel[i]);
    f32 y = scale.y;
    gabi::store<f32>(model + 0xC4, z);
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::call(0x0233D6B8, this, i);
    gabi::call(0x027F4D5C, (u8 *)mpModel[i]);
  }
}
VERIFY(0x0233D97C, &Act_c::init_mtx);
void Act_c::set_mtx(s32 idx) {
  WWHD_FUNC(0x0233D6B8, void, this, idx);
  const u32 offsets = 0x104698A0, stack = 0x1048D0CC;
  if (!gabi::load<u32>(0x104698E8)) {
    const f32 values[18] = {0.56f,    1078.13f, 162.68f,   1026.17f, 332.98f,
                            162.68f,  634.42f,  -872.69f,  162.68f,  -633.3f,
                            -872.69f, 162.68f,  -1025.04f, 332.98f,  162.68f,
                            0,        0,        0};
    for (u32 j = 0; j < 18; j++)
      gabi::store<f32>(offsets + j * 4, values[j]);
    gabi::store<u32>(0x104698E8, 1);
  }
  if (idx > 5)
    return;
  if (idx < 5) {
    f32 z = current.pos.z, x = current.pos.x, y = current.pos.y;
    gabi::call(0x028E93CC, ptr(stack), x, y, z);
    gabi::call(0x025F1C28, ptr(stack), (s16)shape_angle.y);
    gabi::call(0x025F1C5C, ptr(stack), (s16)mRotAngle);
    u32 off = offsets + (u32)idx * 12;
    x = gabi::load<f32>(off);
    y = gabi::load<f32>(off + 4);
    z = gabi::load<f32>(off + 8);
    gabi::call(0x025F24E0, x, y, z);
    gabi::call(0x025F1C5C, ptr(stack), (s16)-mRotAngle);
    gabi::call(0x025F1C5C, ptr(stack), (s16)mGondolaWaveAngle);
  } else {
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    gabi::call(0x028E8D88, pos.get(), ptr(offsets + 60), pos.get());
    f32 z = pos->z, y = pos->y, x = pos->x;
    gabi::call(0x028E93CC, ptr(stack), x, y, z);
    gabi::call(0x025F1C28, ptr(stack), (s16)shape_angle.y);
    gabi::call(0x025F1C5C, ptr(stack), (s16)mRotAngle);
  }
  f32 values[12];
  for (u32 j = 0; j < 12; j++)
    values[j] = gabi::load<f32>(stack + j * 4);
  u32 model = gabi::ea((u8 *)mpModel[idx]);
  for (u32 j = 0; j < 12; j++)
    gabi::store<f32>(model + 0xC8 + j * 4, values[j]);
  gabi::call(0x028E90D4, ptr(stack), &mMtx[idx][0]);
}
VERIFY(0x0233D6B8, &Act_c::set_mtx);
void Act_c::rot_mng() {
  WWHD_FUNC(0x0233E2A0, void, this);
  s16 state = mRotState;
  switch (state) {
  case 0: {
    mRotSpeed = 0;
    mRotAngle = 0x1800;
    s32 sw = gabi::call<s32>(0x0233F2D4, this, 8, 0);
    u32 save = gabi::load<u32>(0x101F84DC);
    s8 room = home.roomNo;
    if (!gabi::call<s32>(0x025BA0C0, ptr(save + 0x20), sw, room))
      break;
    u32 wind = gabi::call<u32>(0x0257DAA8);
    f32 x = gabi::load<f32>(wind), z = gabi::load<f32>(wind + 8);
    s32 angle = gabi::call<s32>(0x020195B0, x, z);
    if (angle == -32768) {
      if (set_event(mEventIdxStart)) {
        mRotTimer = 0;
        mRotSpeed = 0;
        mRotState = 6;
        gabi::call(0x025E1988, 0x806);
      }
    } else if (set_event(mEventIdxVive)) {
      mRotSpeed = 0;
      mRotState = 2;
      mRotTimer = 0;
      sw = gabi::call<s32>(0x0233F2D4, this, 8, 0);
      save = gabi::load<u32>(0x101F84DC);
      room = home.roomNo;
      gabi::call(0x025B9F7C, ptr(save + 0x20), sw, room);
    }
    break;
  }
  case 2:
  case 6: {
    s16 timer = (s16)(mRotTimer + 1);
    if (timer > 20) {
      mRotTimer = 0;
      mRotState = (s16)(mRotState + 1);
    } else
      mRotTimer = timer;
    break;
  }
  case 3:
  case 4:
  case 5:
  case 7:
  case 8:
  case 9: {
    s16 timer = mRotTimer;
    if (timer < 11)
      mRotSpeed = 4;
    else if ((state != 5 && state != 9) || !(mFrameTimer & 3))
      mRotSpeed = (s16)(mRotSpeed - 1);
    s16 speed = mRotSpeed;
    mRotTimer = (s16)(timer + 1);
    if (speed < 0) {
      s16 currentState = mRotState;
      mRotSpeed = 0;
      mRotTimer = 0;
      switch (currentState) {
      case 3:
      case 4:
      case 7:
      case 8:
        mRotState = (s16)(currentState + 1);
        break;
      case 5:
        mRotState = 10;
        break;
      case 9:
        mRotState = 1;
        break;
      }
    }
    break;
  }
  case 10: {
    s16 angle = mRotAngle;
    mRotSpeed = -5;
    if (angle < 0x1800 || !now_event(mEventIdxVive)) {
      mRotSpeed = 0;
      mRotState = 0;
    }
  } break;
  case 1: {
    u32 save = gabi::load<u32>(0x101F84DC);
    gabi::call(0x025B8B68, ptr(save + 0x644), 0x2104);
    s16 frame = mFrameTimer, id = mEventIdxStart, speed = mRotSpeed;
    if (!(frame & 3)) {
      speed = (s16)(speed + 1);
      mRotSpeed = speed;
    }
    s32 limit = now_event(id) ? 67 : 45;
    if (speed > limit)
      mRotSpeed = limit;
    angle_mng();
    break;
  }
  }
}
VERIFY(0x0233E2A0, &Act_c::rot_mng);
void Act_c::make_lean() {
  WWHD_FUNC(0x0233E770, void, this);
  gabi::Local<cXyz> origin, offset, pt0, pt1, delta;
  // Keep the outgoing linkage area below live locals: normalizeRS stores LR at SP+4.
  gabi::Local<cXyz> linkage;
  origin->set(0, 0, 0);
  offset->set(0, -100, -135);
  for (s32 i = 0; i < 5; i++) {
    s16 target = 0;
    if (mRideState[i] == 1) {
      gabi::call(0x028E8F64, &mMtx[i][0], origin.get(), pt0.get());
      gabi::call(0x028E8F64, &mMtx[i][0], offset.get(), pt1.get());
      f32 x0 = pt0->x, x1 = pt1->x, z1 = pt1->z, z0 = pt0->z;
      f32 rz = mRidePos.z, rx = mRidePos.x;
      f32 dx = rx - x0, dz = rz - z0;
      delta->set(x1 - x0, 0, z1 - z0);
      gabi::call(0x0201B47C, delta.get());
      f32 nz = delta->z, z = pt0->z, nx = delta->x;
      rz = mRidePos.z;
      x0 = pt0->x;
      rx = mRidePos.x;
      f32 a = gabi::fmadds(-nz, x0, nx * z), b = gabi::fmadds(-nz, rx, nx * rz);
      f32 cross = gabi::fmsubs(nx, dz, nz * dx);
      f32 height = std::fabs(b - a) / 162.0f;
      target = (s16)gabi::ftoi(height * (cross < 0.0f ? 550.0f : -550.0f));
    }
    mRideWaveTarget[i] = target;
    gabi::call(0x0200F428, &mRideWaveAngle[i], target, 4, 0x1000);
    mRideState[i] = 0;
  }
}
VERIFY(0x0233E770, &Act_c::make_lean);
BOOL Act_c::create_heap() {
  WWHD_FUNC(0x0233DA04, BOOL, this);
  auto resource = [&](s32 idx, s32 line, u32 assertion) {
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = 0x10028250;
    (*name)[1] = 0x10028048;
    u32 res = gabi::call<u32>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)),
                              name.get(), idx);
    if (!res)
      gabi::call(0x0273AA24, ptr(0x1002818C), line, ptr(assertion));
    return res;
  };
  u32 gondola = resource(4, 0x182, 0x1002815C);
  if (gondola)
    for (s32 i = 0; i < 5; i++)
      mpModel[i] = gabi::call<u8 *>(0x025E38E0, ptr(gondola), 0, 0x11020203);
  u32 wheel = resource(6, 0x18B, 0x1002812C);
  if (wheel)
    mpModel[5] = gabi::call<u8 *>(0x025E38E0, ptr(wheel), 0, 0x11020203);
  if (gondola && wheel) {
    mRotAngle = 0x1800;
    mGondolaWaveAngle = 0;
    init_mtx();
  }
  u32 bgGondola = resource(10, 0x1A7, 0x10028174);
  if (bgGondola)
    for (s32 i = 0; i < 5; i++) {
      u32 bg = gabi::call<u32>(0x024F23F4, (u8 *)nullptr);
      mpBgW[i] = ptr(bg);
      if (bg &&
          gabi::call<s32>(0x0200A030, ptr(bg), ptr(bgGondola), 1, &mMtx[i][0]))
        return FALSE;
    }
  u32 bgWheel = resource(11, 0x1B7, 0x10028144);
  if (bgWheel) {
    u32 bg = gabi::call<u32>(0x024F23F4, (u8 *)nullptr);
    mpBgW[5] = ptr(bg);
    if (bg &&
        gabi::call<s32>(0x0200A030, ptr(bg), ptr(bgWheel), 1, &mMtx[5][0]))
      return FALSE;
  }
  if (!gondola)
    return FALSE;
  for (s32 i = 0; i < 5; i++)
    if (mpModel[i] == nullptr)
      return FALSE;
  if (!wheel || mpModel[5] == nullptr || !bgGondola)
    return FALSE;
  for (s32 i = 0; i < 5; i++)
    if (mpBgW[i] == nullptr)
      return FALSE;
  return bgWheel && mpBgW[5] != nullptr;
}
VERIFY(0x0233DA04, &Act_c::create_heap);
BOOL solidHeapCB(Act_c *self) {
  WWHD_FUNC(0x0233DD0C, BOOL, self);
  return self->create_heap();
}
VERIFY(0x0233DD0C, solidHeapCB);
BOOL Act_c::create() {
  WWHD_FUNC(0x0233DD60, BOOL, this);
  u32 self = gabi::ea(this), flags = gabi::load<u32>(self + 0x2E4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(self + 0xB4, 0x10028070);
      gabi::call(0x028EFFD0, ptr(self + 0x3F0), 5, 0x3C, 0x0233F0BC);
      gabi::call(0x028EFFD0, ptr(self + 0x51C), 5, 0x130, 0x0233F124);
      gabi::call(0x028EFFD0, ptr(self + 0xB0C), 5, 0x3C, 0x0233F0BC);
      gabi::call(0x028EFFD0, ptr(self + 0xC38), 5, 0x130, 0x0233F124);
      gabi::call(0x028EFFD0, ptr(self + 0x1228), 5, 0x3C, 0x0233F0BC);
      gabi::call(0x028EFFD0, ptr(self + 0x1354), 5, 0x12C, 0x025166F0);
      flags = gabi::load<u32>(self + 0x2E4);
    }
    gabi::store<u32>(self + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, mPhs, ptr(0x10028250));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, this, 0x0233DD0C, 0x11A00))
    return 5;
  u32 model = gabi::ea((u8 *)mpModel[5]);
  gabi::store<u32>(self + 0x348, model ? model + 0xC8 : 0);
  for (s32 i = 0; i < 6; i++) {
    u32 p = play();
    u8 *bg = mpBgW[i];
    gabi::call(0x024EEA6C, ptr(p + 0x12A0), bg, this);
    u32 w = gabi::ea((u8 *)mpBgW[i]);
    gabi::store<u32>(w + 0xA8, 0x024EE658);
    if (i < 5) {
      w = gabi::ea((u8 *)mpBgW[i]);
      gabi::store<u32>(w + 0xB0, 0x0233DD10);
    }
  }
  gabi::call(0x025D674C, this, -1400.0f, -1400.0f, -500.0f, 1400.0f, 1400.0f,
             800.0f);
  gabi::store<f32>(self + 0x364, 10.0f);
  s32 sw = gabi::call<s32>(0x0233F2D4, this, 8, 0);
  s8 room = home.roomNo;
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025BA0C0, ptr(save + 0x20), sw, room)) {
    mRotSpeed = 45;
    mRotState = 1;
  } else {
    mRotSpeed = 0;
    mRotState = 0;
  }
  u32 p = play();
  mEventIdxVive =
      gabi::call<s32>(0x02543F10, ptr(p + 0x52C4), ptr(0x100281B4), 255);
  p = play();
  mEventIdxStart =
      gabi::call<s32>(0x02543F10, ptr(p + 0x52C4), ptr(0x100281C0), 255);
  for (s32 i = 0; i < 5; i++) {
    u32 status[3] = {self + 0x3F0 + i * 0x3C, self + 0xB0C + i * 0x3C,
                     self + 0x1228 + i * 0x3C};
    u32 coll[3] = {self + 0x51C + i * 0x130, self + 0xC38 + i * 0x130,
                   self + 0x1354 + i * 0x12C};
    for (u32 j = 0; j < 3; j++) {
      gabi::call(0x02515F14, ptr(status[j]), 255, 255, this);
      gabi::call(j == 2 ? 0x0251677C : 0x02516518, ptr(coll[j]),
                 ptr(j == 2 ? 0x10028080 : 0x100280C0));
      gabi::store<u32>(coll[j] + 0x44, status[j]);
      u32 x = gabi::load<u32>(0x101FFBA8),
          flags = gabi::load<u32>(coll[j] + 0x94);
      gabi::store<u32>(coll[j] + 0xB4, x);
      u32 y = gabi::load<u32>(0x101FFBAC);
      gabi::store<u32>(coll[j] + 0xB8, y);
      u32 z = gabi::load<u32>(0x101FFBB0);
      gabi::store<u32>(coll[j] + 0x94, flags | 4);
      gabi::store<u32>(coll[j] + 0xBC, z);
    }
  }
  return phase;
}
VERIFY(0x0233DD60, &Act_c::create);
void Act_c::set_collision() {
  WWHD_FUNC(0x0233E9C4, void, this);
  gabi::Local<cXyz> point, offset;
  for (s32 i = 0; i < 5; i++) {
    if (!gabi::load<u32>(0x104698EC)) {
      gabi::store<f32>(0x10469888, 0);
      gabi::store<f32>(0x1046988C, 0);
      gabi::store<f32>(0x10469890, 0);
      gabi::store<u32>(0x104698EC, 1);
    }
    gabi::call(0x028E8F64, &mMtx[i][0], ptr(0x10469888), point.get());
    if (!(point->y > 1820.0f)) {
      u32 cyl = gabi::ea(this) + 0x51C + i * 0x130;
      gabi::call(0x020184DC, ptr(cyl + 0x118), 50.0f);
      gabi::call(0x02018428, ptr(cyl + 0x118), 195.0f);
      offset->set(0, -100, -135);
      gabi::call(0x028E8F64, &mMtx[i][0], offset.get(), point.get());
      gabi::call(0x020182E0, ptr(cyl + 0x118), point.get());
      u32 p = play();
      gabi::call(0x0200E240, ptr(p + 0x26A4), ptr(cyl));
      cyl = gabi::ea(this) + 0xC38 + i * 0x130;
      gabi::call(0x020184DC, ptr(cyl + 0x118), 50.0f);
      gabi::call(0x02018428, ptr(cyl + 0x118), 148.0f);
      offset->set(0, -56, 127);
      gabi::call(0x028E8F64, &mMtx[i][0], offset.get(), point.get());
      gabi::call(0x020182E0, ptr(cyl + 0x118), point.get());
      p = play();
      gabi::call(0x0200E240, ptr(p + 0x26A4), ptr(cyl));
    }
  }
  const Vec3f offsets[5] = {{-993.54f, -325.78f, 37.63f},
                            {-616.86f, 844.24f, 37.63f},
                            {612.3f, 847.55f, 37.63f},
                            {995.28f, -320.43f, 37.63f},
                            {2.82f, -1045.59f, 37.63f}};
  const u32 stack = 0x1048D0CC;
  gabi::Local<be<f32>[12]> matrix;
  for (s32 i = 0; i < 5; i++) {
    if (!gabi::load<u32>(0x104698F0)) {
      gabi::store<f32>(0x10469894, 0);
      gabi::store<f32>(0x10469898, 0);
      gabi::store<f32>(0x1046989C, 0);
      gabi::store<u32>(0x104698F0, 1);
    }
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    gabi::call(0x028E93CC, ptr(stack), x, y, z);
    gabi::call(0x025F1C28, ptr(stack), (s16)shape_angle.y);
    gabi::call(0x025F1C5C, ptr(stack), (s16)mRotAngle);
    gabi::call(0x025F24E0, offsets[i].x, offsets[i].y, offsets[i].z);
    gabi::call(0x025F1C5C, ptr(stack), (s16)-mRotAngle);
    s16 angle = mRideWaveAngle[i], wave = mGondolaWaveAngle;
    gabi::call(0x025F1C5C, ptr(stack), (s16)(angle + wave));
    gabi::call(0x028E90D4, ptr(stack), matrix.get());
    gabi::call(0x028E8F64, matrix.get(), ptr(0x10469894), point.get());
    if (!(point->y > 1820.0f)) {
      u32 sph = gabi::ea(this) + 0x1354 + i * 0x12C;
      gabi::call(0x02018D40, ptr(sph + 0x118), point.get());
      gabi::call(0x02018C8C, ptr(sph + 0x118), 30.0f);
      u32 p = play();
      gabi::call(0x0200E240, ptr(p + 0x26A4), ptr(sph));
    }
  }
}
VERIFY(0x0233E9C4, &Act_c::set_collision);
BOOL methodCreate(Act_c *self) {
  WWHD_FUNC(0x0233F004, BOOL, self);
  return self->create();
}
VERIFY(0x0233F004, methodCreate);
BOOL methodDelete(Act_c *self) {
  WWHD_FUNC(0x0233F008, BOOL, self);
  return self->remove();
}
VERIFY(0x0233F008, methodDelete);
BOOL methodExecute(Act_c *self) {
  WWHD_FUNC(0x0233F00C, BOOL, self);
  return self->execute();
}
VERIFY(0x0233F00C, methodExecute);
BOOL methodDraw(Act_c *self) {
  WWHD_FUNC(0x0233F010, BOOL, self);
  return self->draw();
}
VERIFY(0x0233F010, methodDraw);
BOOL isDelete(Act_c *self) {
  WWHD_FUNC(0x0233F2CC, BOOL, self);
  return TRUE;
}
VERIFY(0x0233F2CC, isDelete);
void initStatics() {
  WWHD_FUNC(0x0233F014, void);
  for (u32 i = 0; i < 16; i += 4)
    gabi::store<u32>(0x10469878 + i, 0);
  gabi::call(0x028F026C, ptr(0x101C91E0));
  gabi::store<f32>(0x1046986C, -3.1415927410125732f);
  gabi::store<f32>(0x10469870, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x10469874));
  gabi::call(0x028F026C, ptr(0x101C91EC));
  gabi::call(0x028EAB2C, ptr(0x10469875));
  gabi::call(0x028F026C, ptr(0x101C91F8));
}
VERIFY(0x0233F014, initStatics);
void emptyDestructor(u8 *self, s32 flags) {
  WWHD_FUNC(0x0233F0A8, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0233F0A8, emptyDestructor);
u8 *statusConstructor(u8 *self) {
  WWHD_FUNC(0x0233F0BC, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x3C);
  if (self) {
    gabi::call(0x0200BD2C, self);
    gabi::call(0x02515DA0, ptr(gabi::ea(self) + 0x1C));
    gabi::store<u32>(gabi::ea(self) + 0x18, 0x1004AE88);
    gabi::store<u32>(gabi::ea(self) + 0x1C, 0x1004AEC0);
  }
  return self;
}
VERIFY(0x0233F0BC, statusConstructor);
u8 *cylinderConstructor(u8 *self) {
  WWHD_FUNC(0x0233F124, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x130);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::call(0x02515FB8, self);
    gabi::store<u32>(a + 0x114, 0x100015A8);
    gabi::store<u32>(a + 0x110, 0x10028060);
    gabi::call(0x02018590, ptr(a + 0x118));
    gabi::store<u32>(a + 0x3C, 0x1004B108);
    gabi::store<u32>(a + 0x12C, 0x1004B150);
    gabi::store<u32>(a + 0x114, 0x1004B160);
  }
  return self;
}
VERIFY(0x0233F124, cylinderConstructor);
void emptyVirtual(u8 *self) { WWHD_FUNC(0x0233F1B0, void, self); }
VERIFY(0x0233F1B0, emptyVirtual);
void actorDestructor(Act_c *self, s32 flags) {
  WWHD_FUNC(0x0233F1B4, void, self, flags);
  if (!self)
    return;
  u32 a = gabi::ea(self);
  gabi::call(0x028F0164, ptr(a + 0x1354), 5, 0x12C, 0x02515AE8, 0, 0);
  gabi::call(0x028F0164, ptr(a + 0x1228), 5, 0x3C, 0x02515860, 0, 0);
  gabi::call(0x028F0164, ptr(a + 0xC38), 5, 0x130, 0x02515A70, 0, 0);
  gabi::call(0x028F0164, ptr(a + 0xB0C), 5, 0x3C, 0x02515860, 0, 0);
  gabi::call(0x028F0164, ptr(a + 0x51C), 5, 0x130, 0x02515A70, 0, 0);
  gabi::call(0x028F0164, ptr(a + 0x3F0), 5, 0x3C, 0x02515860, 0, 0);
  gabi::call(0x025D50BC, self, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0233F1B4, actorDestructor);
} // namespace daObjFerris
