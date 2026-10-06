#include "d/actor/d_a_obj_jump.h"
using namespace daObjJump;
using gabi::load;
using gabi::store;
static u32 address(const void *p) { return gabi::ea(p); }
static u32 attr(Act_c *a);
static u32 joint(u32 data, u16 index);
s32 Act_c::Mthd_Create() {
  WWHD_FUNC(0x02365C98, s32, this);
  u32 condition = load<u32>(address(this) + 0x2E4);
  if (!(condition & 8)) {
    if (address(this)) {
      gabi::call<void>(0x024F1D40, this);
      condition = load<u32>(address(this) + 0x2E4);
      store<u32>(address(this) + 0xB4, 0x1002BAE8);
    }
    store<u32>(address(this) + 0x2E4, condition | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x1002BA40));
  if (phase == 4) {
    mType = gabi::call<s32>(0x02366CE4, this, 1, 0);
    u32 table = attr(this), heap = load<u32>(table);
    s16 dzb = load<s16>(table + 4);
    phase = gabi::call<s32>(0x024F1D9C, this, STR(0x1002BA40), dzb,
                            gabi::at<void>(0x024EE658), heap);
    if (phase != 4 && phase != 5)
      gabi::call<void>(0x0273AA24, STR(0x1002B980), 0x186, STR(0x1002B994));
  }
  return phase;
}
VERIFY(0x02365C98, &Act_c::Mthd_Create);
s32 Act_c::Mthd_Delete() {
  WWHD_FUNC(0x02365D8C, s32, this);
  s32 result = gabi::call<s32>(0x024F1F64, this);
  gabi::call<void>(0x025204C8, &mPhase, STR(0x1002BA40));
  return result;
}
VERIFY(0x02365D8C, &Act_c::Mthd_Delete);
static u32 attr(Act_c *a) { return 0x1002BA48 + u32(s32(a->mType)) * 0x50; }
static u32 joint(u32 data, u16 index) {
  u32 count = load<u32>(data + 4), nodes = load<u32>(data + 8);
  return index < count ? nodes + u32(index) * 0x1C : nodes;
}
s32 jnodeCB_lower(void *node, s32 timing) {
  WWHD_FUNC(0x02365DD8, s32, node, timing);
  if (timing == 0) {
    u32 model = load<u32>(0x104B462C), actor = load<u32>(model + 0xB8);
    u32 desc = address(gabi::call<void *>(0x027F7878, node));
    u16 index = load<u16>(desc + 4);
    u32 block = load<u32>(model + 0x2C),
        matrix = load<u32>(block + 0x10) + u32(index) * 48;
    store<u16>(block + 4, load<u16>(block + 4) | 0x10);
    gabi::call<void>(0x028E90D4, gabi::at<void>(matrix),
                     gabi::at<void>(0x1048D0CC));
    if (index == 1)
      gabi::call<void>(0x025F2518, load<f32>(actor + 0x400), 1.0f, 1.0f);
    else if (index == 2)
      gabi::call<void>(0x025F2518, 1.0f / load<f32>(actor + 0x400), 1.0f, 1.0f);
    block = load<u32>(model + 0x2C);
    matrix = load<u32>(block + 0x10) + u32(index) * 48;
    store<u16>(block + 4, load<u16>(block + 4) | 0x10);
    mtx_copy(gabi::at<Mtx34>(matrix), gabi::at<Mtx34>(0x1048D0CC));
    u32 table = 0x1002BA48 + load<u32>(actor + 0x3EC) * 0x50;
    f32 scale = load<f32>(actor + 0x400), low = load<f32>(table + 0x18),
        high = load<f32>(table + 0x1C);
    store<f32>(0x104B4884,
               gabi::fmadds(high - low, scale - 1.0f, load<f32>(0x104B4884)));
  }
  return 1;
}
VERIFY(0x02365DD8, jnodeCB_lower);
s32 Act_c::CreateHeap() {
  WWHD_FUNC(0x02365F68, s32, this);
  u32 table = attr(this), resources = load<u32>(0x101F4F28);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x1002BA40;
  name->__vtbl = 0x1002B968;
  s16 index = load<s16>(table + 6);
  void *data = gabi::call<void *>(0x026066C4, gabi::at<void>(resources),
                                  name.get(), index);
  if (!data)
    gabi::call<void>(0x0273AA24, STR(0x1002B9E8), 0x11A, STR(0x1002B9D8));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
  if (mModel.get()) {
    u16 spring = load<u16>(attr(this) + 0x16);
    store<u32>(joint(address(data), spring) + 8, 0x02365DD8);
    spring = load<u16>(attr(this) + 0x16);
    store<u32>(joint(address(data), u16(spring + 1)) + 8, 0x02365DD8);
    store<u32>(address(mModel.get()) + 0xB8, address(this));
  }
  return mModel.get() != nullptr;
}
VERIFY(0x02365F68, &Act_c::CreateHeap);
void Act_c::set_mtx() {
  WWHD_FUNC(0x023660A4, void, this);
  auto matrix = gabi::at<Mtx34>(0x1048D0CC);
  f32 x = load<f32>(address(this) + 0x314),
      y = load<f32>(address(this) + 0x318),
      z = load<f32>(address(this) + 0x31C);
  gabi::call<void>(0x028E93CC, matrix, x, y, z);
  s16 rx = load<s16>(address(this) + 0x328),
      ry = load<s16>(address(this) + 0x32A),
      rz = load<s16>(address(this) + 0x32C);
  gabi::call<void>(0x025F1B48, matrix, rx, ry, rz);
  mtx_copy(gabi::at<Mtx34>(address(mModel.get()) + 0xC8), matrix);
  u32 table = attr(this);
  f32 scale = mSpringScale, offset = load<f32>(table + 0x24);
  f32 low = load<f32>(table + 0x18), high = load<f32>(table + 0x1C),
      span = load<f32>(table + 0x20) - offset;
  f32 ratio = gabi::fmadds(high - low, scale - 1.0f, span) / span;
  gabi::call<void>(0x025F24E0, 0.0f, offset, 0.0f);
  gabi::call<void>(0x025F2518, 1.0f, ratio, 1.0f);
  offset = load<f32>(attr(this) + 0x24);
  gabi::call<void>(0x025F24E0, 0.0f, -offset, 0.0f);
  gabi::call<void>(0x028E90D4, matrix, gabi::at<Mtx34>(0x1046A338));
}
VERIFY(0x023660A4, &Act_c::set_mtx);
void Act_c::init_mtx() {
  WWHD_FUNC(0x02366248, void, this);
  u32 model = address(mModel.get());
  f32 x = load<f32>(address(this) + 0x330),
      y = load<f32>(address(this) + 0x334),
      z = load<f32>(address(this) + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  set_mtx();
}
VERIFY(0x02366248, &Act_c::init_mtx);
void rideCB(void *bg, Act_c *actor, fopAc_ac_c *rider) {
  WWHD_FUNC(0x02366268, void, bg, actor, rider);
  actor->mIsRide = 1;
  if (!rider || load<s16>(address(rider) + 0xE) != 0xA8)
    return;
  actor->mIsPlayerRide = 1;
  u32 flags = load<u32>(address(rider) + 0x3B8);
  if (flags & 0x02000000)
    actor->mIsHeavyRide = 1;
  f32 speed = load<f32>(address(rider) + 0x370),
      threshold = load<f32>(attr(actor) + 0x4C);
  if (!(speed <= threshold))
    actor->mWobble = 1;
}
VERIFY(0x02366268, rideCB);
void Act_c::mode_wait_init() {
  WWHD_FUNC(0x023662C8, void, this);
  mMode = 0;
  mTargetScale = 1.0f;
  mVibrationTimer = 0;
}
VERIFY(0x023662C8, &Act_c::mode_wait_init);
s32 Act_c::Create() {
  WWHD_FUNC(0x023662E4, s32, this);
  u32 model = address(mModel.get());
  mTargetScale = 1.0f;
  mSpringScale = 1.0f;
  store<u32>(address(this) + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  u32 table = attr(this);
  f32 minx = load<s16>(table + 8), miny = load<s16>(table + 10),
      minz = load<s16>(table + 12);
  f32 maxx = load<s16>(table + 14), maxy = load<s16>(table + 16),
      maxz = load<s16>(table + 18);
  gabi::call<void>(0x025D674C, this, minx, miny, minz, maxx, maxy, maxz);
  if (s32(mType) == 1) {
    u32 status = load<u32>(address(this) + 0x2E0);
    store<u8>(address(this) + 0x2DE, 0);
    store<u32>(address(this) + 0x2E0, status & ~63u);
  }
  u32 bg = load<u32>(address(this) + 0x3AC);
  store<u32>(bg + 0xB0, 0x02366268);
  mode_wait_init();
  return 1;
}
VERIFY(0x023662E4, &Act_c::Create);
void Act_c::set_push_flag() {
  WWHD_FUNC(0x0236642C, void, this);
  mRideStarted = 0;
  if (mIsRide) {
    u8 count = mRideCount;
    if (count < 255)
      mRideCount = ++count;
    if (count == load<u8>(attr(this) + 0x43))
      mRideStarted = 1;
  } else
    mRideCount = 0;
  mRideEnded = 0;
  if (mIsRide)
    mUnriddenCount = 0;
  else {
    u8 count = mUnriddenCount;
    if (count < 255)
      mUnriddenCount = ++count;
    if (count == load<u8>(attr(this) + 0x44))
      mRideEnded = 1;
  }
  mPlayerPush = 0;
  bool player = mIsPlayerRide != 0;
  if (player) {
    u32 play = address(gabi::call<void *>(0x025200D4));
    player = (load<u32>(play + 0x5CD8) & 0x100) != 0;
  }
  if (player) {
    u8 count = mPlayerCount;
    if (count < 255)
      mPlayerCount = ++count;
    if (count == load<u8>(attr(this) + 0x45))
      mPlayerPush = 1;
  } else
    mPlayerCount = 0;
  mHeavyStarted = 0;
  if (mIsHeavyRide) {
    u8 count = mHeavyCount;
    if (count < 255)
      mHeavyCount = ++count;
    if (count == load<u8>(attr(this) + 0x46))
      mHeavyStarted = 1;
  } else
    mHeavyCount = 0;
  mHeavyEnded = 0;
  if (mIsHeavyRide)
    mLightCount = 0;
  else {
    u8 count = mLightCount;
    if (count < 255)
      mLightCount = ++count;
    if (count == load<u8>(attr(this) + 0x47))
      mHeavyEnded = 1;
  }
}
VERIFY(0x0236642C, &Act_c::set_push_flag);
void Act_c::calc_vib_pos() {
  WWHD_FUNC(0x02366668, void, this);
  u32 table = attr(this);
  f32 target = mTargetScale, scale = mSpringScale, velocity = mVelocity;
  velocity = gabi::fnmsubs(scale - target, load<f32>(table + 0x2C), velocity);
  mVelocity = velocity;
  velocity = gabi::fnmsubs(velocity, load<f32>(table + 0x28), velocity);
  mVelocity = velocity;
  mSpringScale = scale + velocity;
}
VERIFY(0x02366668, &Act_c::calc_vib_pos);
void Act_c::clear_push_flag() {
  WWHD_FUNC(0x023666B0, void, this);
  mIsRide = 0;
  mWobble = 0;
  mIsHeavyRide = 0;
  mIsPlayerRide = 0;
}
VERIFY(0x023666B0, &Act_c::clear_push_flag);
s32 Act_c::Execute(Mtx34 **out) {
  WWHD_FUNC(0x023666C8, s32, this, out);
  set_push_flag();
  u32 entry = 0x1002BA08 + u32(s32(mMode)) * 8;
  s16 vindex = load<s16>(entry + 2), adjustment = load<s16>(entry);
  u32 receiver = address(this) + s32(adjustment), target;
  if (vindex < 0)
    target = load<u32>(entry + 4);
  else {
    s16 voffset = load<s16>(entry + 6);
    u32 vtable = load<u32>(receiver + s32(voffset));
    target = load<u32>(vtable + u32(s32(vindex)) * 8 + 4);
  }
  gabi::call<void>(target, gabi::at<void>(receiver));
  s32 mode = mMode;
  if ((mode == 0 || mode == 2) && mWobble)
    mVelocity = f32(mVelocity) + load<f32>(attr(this) + 0x34);
  calc_vib_pos();
  set_mtx();
  store<u32>(address(out), 0x1046A338);
  clear_push_flag();
  return 1;
}
VERIFY(0x023666C8, &Act_c::Execute);
s32 Act_c::Draw() {
  WWHD_FUNC(0x023667C4, s32, this);
  void *env = gabi::call<void *>(0x02555D0C);
  gabi::call<void>(0x025626A4, env, 0, gabi::at<cXyz>(address(this) + 0x314),
                   gabi::at<void>(address(this) + 0x110));
  env = gabi::call<void *>(0x02555D0C);
  J3DModel *model = mModel.get();
  gabi::call<void>(0x02562F5C, env, model,
                   gabi::at<void>(address(this) + 0x110));
  u32 play = address(gabi::call<void *>(0x025200D4));
  store<u32>(0x104B4634, load<u32>(play + 0x5D70));
  play = address(gabi::call<void *>(0x025200D4));
  store<u32>(0x104B4638, load<u32>(play + 0x5D74));
  model = mModel.get();
  gabi::call<void>(0x025E2DE0, model, 0);
  play = address(gabi::call<void *>(0x025200D4));
  store<u32>(0x104B4634, load<u32>(play + 0x5D78));
  play = address(gabi::call<void *>(0x025200D4));
  store<u32>(0x104B4638, load<u32>(play + 0x5D7C));
  return 1;
}
VERIFY(0x023667C4, &Act_c::Draw);
void Act_c::mode_w_l_init() {
  WWHD_FUNC(0x0236685C, void, this);
  mMode = 1;
  mTargetScale = load<f32>(attr(this) + 0x38);
}
VERIFY(0x0236685C, &Act_c::mode_w_l_init);
static void sound(Act_c *actor, u32 id) {
  s8 room = load<s8>(address(actor) + 0x326);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call<void>(0x025E1A40, id, gabi::at<void>(address(actor) + 0x37C), 0,
                   reverb);
}
void Act_c::mode_wait() {
  WWHD_FUNC(0x0236687C, void, this);
  s16 timer = mVibrationTimer;
  if (timer <= 0 &&
      (mRideStarted || mRideEnded || mPlayerPush || mHeavyStarted)) {
    mVelocity = f32(mVelocity) + load<f32>(attr(this) + 0x30);
    mVibrationTimer = load<u8>(attr(this) + 0x48);
  } else if (timer > 0)
    mVibrationTimer = s16(timer - 1);
  if (u8(mHeavyCount) > load<u8>(attr(this) + 0x40)) {
    sound(this, 0x69B9);
    mode_w_l_init();
  }
}
VERIFY(0x0236687C, &Act_c::mode_wait);
void Act_c::mode_lower_init() {
  WWHD_FUNC(0x02366998, void, this);
  mMode = 2;
  mVibrationTimer = 0;
}
VERIFY(0x02366998, &Act_c::mode_lower_init);
void Act_c::mode_w_l() {
  WWHD_FUNC(0x023669AC, void, this);
  if (!(f32(mSpringScale) > f32(mTargetScale)))
    mode_lower_init();
}
VERIFY(0x023669AC, &Act_c::mode_w_l);
void Act_c::mode_l_u_init() {
  WWHD_FUNC(0x023669C0, void, this);
  mMode = 3;
  mTargetScale = load<f32>(attr(this) + 0x3C);
}
VERIFY(0x023669C0, &Act_c::mode_l_u_init);
void Act_c::mode_lower() {
  WWHD_FUNC(0x023669E0, void, this);
  s16 timer = mVibrationTimer;
  if (timer <= 0 && mHeavyEnded) {
    mVelocity = f32(mVelocity) + load<f32>(attr(this) + 0x30);
    mVibrationTimer = load<u8>(attr(this) + 0x48);
  } else if (timer > 0)
    mVibrationTimer = s16(timer - 1);
  if (u8(mLightCount) > load<u8>(attr(this) + 0x41)) {
    sound(this, 0x69BA);
    mode_l_u_init();
  }
}
VERIFY(0x023669E0, &Act_c::mode_lower);
void Act_c::mode_upper_init() {
  WWHD_FUNC(0x02366AD4, void, this);
  mMode = 4;
  mTimer = load<u8>(attr(this) + 0x42);
}
VERIFY(0x02366AD4, &Act_c::mode_upper_init);
void Act_c::mode_l_u() {
  WWHD_FUNC(0x02366AF4, void, this);
  f32 scale = mSpringScale, target = mTargetScale;
  if (!(scale < target)) {
    bool player = mIsPlayerRide != 0;
    mSpringScale = target;
    mVelocity = 0.0f;
    if (player) {
      u32 play = address(gabi::call<void *>(0x025200D4)),
          actor = load<u32>(play + 0x5B2C);
      store<u32>(actor + 0x3BC, load<u32>(actor + 0x3BC) | 0x10);
    }
    mode_upper_init();
  }
}
VERIFY(0x02366AF4, &Act_c::mode_l_u);
void Act_c::mode_u_w_init() {
  WWHD_FUNC(0x02366B64, void, this);
  mMode = 5;
  mTargetScale = 1.0f;
}
VERIFY(0x02366B64, &Act_c::mode_u_w_init);
void Act_c::mode_upper() {
  WWHD_FUNC(0x02366B7C, void, this);
  s16 timer = s16(s16(mTimer) - 1);
  mTimer = timer;
  if (timer <= 0)
    mode_u_w_init();
}
VERIFY(0x02366B7C, &Act_c::mode_upper);
void Act_c::mode_u_w() {
  WWHD_FUNC(0x02366B94, void, this);
  if (!(f32(mSpringScale) > f32(mTargetScale)))
    mode_wait_init();
}
VERIFY(0x02366B94, &Act_c::mode_u_w);
s32 jump_Mthd_Create(Act_c *actor) {
  WWHD_FUNC(0x02366BA8, s32, actor);
  return actor->Mthd_Create();
}
VERIFY(0x02366BA8, jump_Mthd_Create);
s32 jump_Mthd_Delete(Act_c *actor) {
  WWHD_FUNC(0x02366BAC, s32, actor);
  return actor->Mthd_Delete();
}
VERIFY(0x02366BAC, jump_Mthd_Delete);
s32 jump_Mthd_Execute(Act_c *actor) {
  WWHD_FUNC(0x02366BB0, s32, actor);
  return gabi::call<s32>(0x024F1E9C, actor);
}
VERIFY(0x02366BB0, jump_Mthd_Execute);
s32 jump_Mthd_Draw(Act_c *actor) {
  WWHD_FUNC(0x02366BB4, s32, actor);
  u32 vtable = load<u32>(address(actor) + 0xB4);
  return gabi::call<s32>(load<u32>(vtable + 0x2C), actor);
}
VERIFY(0x02366BB4, jump_Mthd_Draw);
s32 jump_Mthd_IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02366BC4, s32, actor);
  u32 vtable = load<u32>(address(actor) + 0xB4);
  return gabi::call<s32>(load<u32>(vtable + 0x3C), actor);
}
VERIFY(0x02366BC4, jump_Mthd_IsDelete);
void jump_sinit() {
  WWHD_FUNC(0x02366BD4, void);
  store<u32>(0x1046A330, 0);
  store<u32>(0x1046A328, 0);
  store<u32>(0x1046A334, 0);
  store<u32>(0x1046A32C, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CA5A4));
  store<f32>(0x1046A31C, load<f32>(0x1002BA38));
  store<f32>(0x1046A320, load<f32>(0x1002BA3C));
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x1046A324));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CA5B0));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x1046A325));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CA5BC));
}
VERIFY(0x02366BD4, jump_sinit);
void jump_static_dtor(void *object, s32 flags) {
  WWHD_FUNC(0x02366C68, void, object, flags);
  if (object && (flags & 1))
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x02366C68, jump_static_dtor);
s32 jump_IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02366C7C, s32, actor);
  return 1;
}
VERIFY(0x02366C7C, jump_IsDelete);
s32 Act_c::Delete() {
  WWHD_FUNC(0x02366C88, s32, this);
  return 1;
}
VERIFY(0x02366C88, &Act_c::Delete);
void jump_dtor(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x02366C90, void, actor, flags);
  if (actor) {
    gabi::call<void>(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, actor);
  }
}
VERIFY(0x02366C90, jump_dtor);
u32 jump_PrmAbstract(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x02366CE4, u32, actor, width, shift);
  u32 parameters = load<u32>(address(actor) + 0xB0);
  u32 mask = (width & 32) ? 0u : (1u << (width & 31));
  u32 value = (shift & 32) ? 0u : (parameters >> (shift & 31));
  return value & (mask - 1);
}
VERIFY(0x02366CE4, jump_PrmAbstract);

// SafeString vtable1002B968+0x14 points to this empty termination hook.
void jump_SafeString_assureTermination(const SafeString *key) {
  WWHD_FUNC(0x02366C84, void, key);
}
VERIFY(0x02366C84, jump_SafeString_assureTermination);
