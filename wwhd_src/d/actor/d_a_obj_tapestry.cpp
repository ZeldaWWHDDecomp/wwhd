/* Cloth tapestry, fire and custom drawing. Direct reconstruction from WWHD,
 * with the GameCube translation-unit map as naming reference.
 */
#include "d/actor/d_a_obj_tapestry.h"
static u8 *ptr(u32 address) { return gabi::at<u8>(address); }
static u8 *tapestryResource(s32 index) {
  gabi::Local<be<u32>[2]> archive;
  (*archive)[0] = 0x1003064C;
  (*archive)[1] = 0x10030654;
  return gabi::call<u8 *>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)),
                          archive.get(), index);
}
s32 daObjTapestry_c::create_res_load() {
  WWHD_FUNC(0x023996F4, s32, this);
  return gabi::call<s32>(0x02520460, mPhase, ptr(0x1003064C));
}
VERIFY(0x023996F4, &daObjTapestry_c::create_res_load);
BOOL daObjTapestry_c::create_heap() {
  WWHD_FUNC(0x02399704, BOOL, this);
  u8 *data = tapestryResource(4);
  if (!data) {
    gabi::call(0x0273AA24, ptr(0x10030828), 0xA25, ptr(0x10030824));
    return 0;
  }
  mpModel = gabi::call<u8 *>(0x025E38E0, data, 0x80000, 0x11000022);
  data = tapestryResource(7);
  mpBg = gabi::call<u8 *>(0x024F2478, data, 1, mBgMatrix);
  return mpModel != nullptr && mpBg != nullptr;
}
VERIFY(0x02399704, &daObjTapestry_c::create_heap);
BOOL tapestryHeap(daObjTapestry_c *self) {
  WWHD_FUNC(0x023997DC, BOOL, self);
  return self->create_heap();
}
VERIFY(0x023997DC, tapestryHeap);
void daObjTapestry_c::set_mtx() {
  WWHD_FUNC(0x023997E0, void, this);
  u32 a = gabi::ea(this);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  gabi::call(0x025F1B48, ptr(0x1048D0CC), gabi::load<s16>(a + 0x328),
             gabi::load<s16>(a + 0x32A), gabi::load<s16>(a + 0x32C));
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  u32 model = gabi::ea((u8 *)mpModel);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
  gabi::call(0x025F2518, (f32)scale.x, (f32)scale.y, (f32)scale.z);
  gabi::call(0x028E90D4, ptr(0x1048D0CC), mBgMatrix);
}
VERIFY(0x023997E0, &daObjTapestry_c::set_mtx);
void daObjTapestry_c::init_mtx() {
  WWHD_FUNC(0x023998C0, void, this);
  u32 model = gabi::ea((u8 *)mpModel);
  f32 y = scale.y, x = scale.x, z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  set_mtx();
}
VERIFY(0x023998C0, &daObjTapestry_c::init_mtx);
static daObjTapestryPLight_c *
tapestryLightConstruct(daObjTapestryPLight_c *self) {
  WWHD_FUNC(0x023998E0, daObjTapestryPLight_c *, self);
  if (!self)
    self = gabi::call<daObjTapestryPLight_c *>(0x0273AD10, 0x30);
  if (self) {
    self->mpEmitter = nullptr;
    self->mPower = gabi::load<f32>(0x10030840);
    self->mActive = 0;
  }
  return self;
}
VERIFY(0x023998E0, tapestryLightConstruct);
void daObjTapestryPacket_c::smokeCallback_init() {
  WWHD_FUNC(0x02399F90, void, this);
  gabi::store<u8>(gabi::ea(this) + 0x24D5, 0);
}
VERIFY(0x02399F90, &daObjTapestryPacket_c::smokeCallback_init);
cXyz *daObjTapestryPacket_c::get_now_pos(s32 row, s32 column) {
  WWHD_FUNC(0x0239A7A0, cXyz *, this, row, column);
  return gabi::at<cXyz>(gabi::ea(this) + 0x106C + (u32)mPositionBuffer * 0x6C0 +
                        ((u32)row * 6 + (u32)column) * 12);
}
VERIFY(0x0239A7A0, &daObjTapestryPacket_c::get_now_pos);
void daObjTapestryPacket_c::calc_acc_gravity() {
  WWHD_FUNC(0x0239B100, void, this);
  f32 coefficient = gabi::load<f32>(0x10030884),
      mass = gabi::load<f32>(0x1046C184), gravity = gabi::load<f32>(0x1046C15C);
  mAcceleration.y =
      gabi::fmadds(mass * coefficient, gravity, (f32)mAcceleration.y);
}
VERIFY(0x0239B100, &daObjTapestryPacket_c::calc_acc_gravity);
void daObjTapestryPLight_c::plight_make() {
  WWHD_FUNC(0x0239C120, void, this);
  gabi::call(0x025564B4, mInfluence);
  mActive = 1;
}
VERIFY(0x0239C120, &daObjTapestryPLight_c::plight_make);
void daObjTapestryPLight_c::plight_delete() {
  WWHD_FUNC(0x0239D8BC, void, this);
  if (mActive) {
    gabi::call(0x0255A374, mInfluence);
    u32 emitter = gabi::ea((u8 *)mpEmitter);
    if (emitter) {
      u32 flags = gabi::load<u32>(emitter + 0x254);
      gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFF);
      gabi::store<u32>(emitter + 0x254, flags | 1);
      emitter = gabi::ea((u8 *)mpEmitter);
      gabi::store<u32>(emitter + 0x1E4, 0);
      mpEmitter = nullptr;
    }
    mActive = 0;
  }
}
VERIFY(0x0239D8BC, &daObjTapestryPLight_c::plight_delete);
void daObjTapestryPacket_c::eff_delete() {
  WWHD_FUNC(0x0239D92C, void, this);
  u32 a = gabi::ea(this);
  for (u32 i = 0; i < 16; i++) {
    u32 cb = a + 0x20C0 + i * 0x2C,
        target = gabi::load<u32>(gabi::load<u32>(cb) + 0x44);
    gabi::call(target, ptr(cb));
  }
  u32 cb = a + 0x24C4, target = gabi::load<u32>(gabi::load<u32>(cb) + 0x44);
  gabi::call(target, ptr(cb));
  gabi::at<daObjTapestryPLight_c>(a + 0x24F0)->plight_delete();
}
VERIFY(0x0239D92C, &daObjTapestryPacket_c::eff_delete);
void daObjTapestryPacket_c::eff_end() {
  WWHD_FUNC(0x0239E334, void, this);
  s32 i = 0;
  u32 a = gabi::ea(this);
  while (i < (s32)mFireCount) {
    gabi::call(0x025A5AC8, ptr(a + 0x20C0 + (u32)i * 0x2C));
    i++;
  }
  mFireCount = 0;
  gabi::call(0x025A5AC8, mSmokeCallback);
  gabi::at<daObjTapestryPLight_c>(a + 0x24F0)->plight_delete();
}
VERIFY(0x0239E334, &daObjTapestryPacket_c::eff_end);
static void tapestryBurnInit(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239EE88, void, self);
  s8 room = gabi::load<s8>(gabi::ea(self) + 0x326);
  self->mBurnTimer = gabi::load<u8>(0x1046C1A4);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x69C1, ptr(gabi::ea(self) + 0x37C), 0, reverb);
}
VERIFY(0x0239EE88, tapestryBurnInit);
static void tapestryFineInit(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239EEDC, void, self);
  u32 a = gabi::ea(self);
  gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) | 0x80);
}
VERIFY(0x0239EEDC, tapestryFineInit);
static u32 tapestryParam(daObjTapestry_c *self, u32 width, u32 shift) {
  WWHD_FUNC(0x0239FB40, u32, self, width, shift);
  u32 p = gabi::load<u32>(gabi::ea(self) + 0xB0),
      bit = (width & 32) ? 0 : 1u << (width & 31),
      shifted = (shift & 32) ? 0 : p >> (shift & 31);
  return shifted & (bit - 1);
}
VERIFY(0x0239FB40, tapestryParam);
BOOL daObjTapestry_c::chk_appear() {
  WWHD_FUNC(0x0239D550, BOOL, this);
  u32 sw = tapestryParam(this, 8, 0), save = gabi::load<u32>(0x101F84DC);
  s8 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
  return !gabi::call<s32>(0x025BA0C0, ptr(save + 0x20), sw, room);
}
VERIFY(0x0239D550, &daObjTapestry_c::chk_appear);
static void actionCall(u32 descriptor, u32 self) {
  s16 index = gabi::load<s16>(descriptor + 2),
      delta = gabi::load<s16>(descriptor);
  self += (s32)delta;
  u32 target;
  if (index < 0)
    target = gabi::load<u32>(descriptor + 4);
  else {
    s16 offset = gabi::load<s16>(descriptor + 6);
    u32 vt = gabi::load<u32>(self + (s32)offset);
    target = gabi::load<u32>(vt + (s32)index * 8 + 4);
  }
  gabi::call(target, ptr(self));
}
void daObjTapestry_c::setup_action(s32 action) {
  WWHD_FUNC(0x0239D5B0, void, this, action);
  u32 descriptor = 0x101CD130 + (u32)action * 8;
  if (gabi::load<s16>(descriptor + 2) != 0)
    actionCall(descriptor, gabi::ea(this));
  u32 table = 0x101CD150 + (u32)action * 8;
  mActionDescriptor[0] = gabi::load<u32>(table);
  u32 second = gabi::load<u32>(table + 4);
  mAction = action;
  mActionDescriptor[1] = second;
}
VERIFY(0x0239D5B0, &daObjTapestry_c::setup_action);
void daObjTapestry_c::set_cc_pos() {
  WWHD_FUNC(0x0239D26C, void, this);
  u32 a = gabi::ea(this);
  f32 zero = gabi::load<f32>(0x10030848), offset = gabi::load<f32>(0x10030938),
      horizontal = gabi::load<f32>(0x100308D4),
      vertical = gabi::load<f32>(0x1003093C);
  gabi::Local<cXyz[3]> points;
  for (u32 tri = 0; tri < 2; tri++) {
    u32 model = gabi::ea((u8 *)mpModel);
    gabi::call(0x028E90D4, ptr(model ? model + 0xC8 : 0), ptr(0x1048D0CC));
    gabi::call(0x025F2518, horizontal, vertical, horizontal);
    gabi::call(0x025F24E0, zero, offset, zero);
    for (u32 vertex = 0; vertex < 3; vertex++) {
      u32 table = 0x101CD124 + tri * 6 + vertex * 2;
      u8 row = gabi::load<u8>(table), column = gabi::load<u8>(table + 1);
      cXyz *position = gabi::call<cXyz *>(
          0x0239A7A0, (daObjTapestryPacket_c *)mpPacket, row, column);
      gabi::call(0x028E8F64, ptr(0x1048D0CC), position, &(*points)[vertex]);
    }
    gabi::call(0x0201924C, ptr(a + 0x508 + tri * 0x150), &(*points)[0],
               &(*points)[1], &(*points)[2]);
  }
}
VERIFY(0x0239D26C, &daObjTapestry_c::set_cc_pos);
void daObjTapestry_c::init_cc() {
  WWHD_FUNC(0x0239D3F0, void, this);
  u32 a = gabi::ea(this);
  for (u32 i = 0; i < 2; i++) {
    u32 triangle = a + 0x3F0 + i * 0x150, status = a + 0x690 + i * 0x3C;
    gabi::call(0x02515F14, ptr(status), 0xFF, 0xFF, this);
    gabi::call(0x0251650C, ptr(triangle), ptr(0x10030774));
    u32 flags = gabi::load<u32>(triangle + 0x94);
    gabi::store<u32>(triangle + 0x44, status);
    gabi::store<u32>(triangle + 0x94, flags | 4);
  }
  set_cc_pos();
}
VERIFY(0x0239D3F0, &daObjTapestry_c::init_cc);
void daObjTapestry_c::set_eye_pos() {
  WWHD_FUNC(0x0239D470, void, this);
  u32 a = gabi::ea(this);
  daObjTapestryPacket_c *packet = mpPacket;
  cXyz *right = gabi::call<cXyz *>(0x0239A7A0, packet, 0, 5),
       *left = gabi::call<cXyz *>(0x0239A7A0, packet, 0, 0);
  gabi::Local<cXyz[4]> corners;
  gabi::call(0x0201AD78, left, &(*corners)[0], right);
  left =
      gabi::call<cXyz *>(0x0239A7A0, (daObjTapestryPacket_c *)mpPacket, 7, 0);
  gabi::call(0x0201AD78, &(*corners)[0], &(*corners)[1], left);
  right =
      gabi::call<cXyz *>(0x0239A7A0, (daObjTapestryPacket_c *)mpPacket, 7, 5);
  gabi::call(0x0201AD78, &(*corners)[1], &(*corners)[2], right);
  gabi::call(0x0201AE48, &(*corners)[2], &(*corners)[3],
             gabi::load<f32>(0x10030940));
  u32 model = gabi::ea((u8 *)mpModel);
  gabi::call(0x028E8F64, ptr(model ? model + 0xC8 : 0), &(*corners)[3],
             ptr(a + 0x37C));
  f32 z = gabi::load<f32>(a + 0x384), y = gabi::load<f32>(a + 0x380);
  gabi::store<f32>(a + 0x398, z);
  f32 x = gabi::load<f32>(a + 0x37C);
  gabi::store<f32>(a + 0x394, y);
  gabi::store<f32>(a + 0x390, x);
}
VERIFY(0x0239D470, &daObjTapestry_c::set_eye_pos);
BOOL daObjTapestry_c::_delete() {
  WWHD_FUNC(0x0239D9A4, BOOL, this);
  u32 a = gabi::ea(this);
  gabi::call(0x0239D92C, (daObjTapestryPacket_c *)mpPacket);
  u32 packet = gabi::ea((daObjTapestryPacket_c *)mpPacket);
  if (packet) {
    u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xC) + 0xC);
    gabi::call(target, ptr(packet), 3);
  }
  gabi::call(0x025204C8, mPhase, ptr(0x1003064C));
  if (gabi::load<u32>(a + 0xF4)) {
    u32 bg = gabi::ea((u8 *)mpBg);
    if (bg) {
      if (gabi::load<u32>(bg) < 0x100) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x020087EC, ptr(play + 0x12A0), (u8 *)mpBg);
      }
      mpBg = nullptr;
    }
  }
  s8 child = gabi::load<s8>(0x1046C154);
  if (child >= 0) {
    gabi::call(0x025F0A18, child);
    gabi::store<s8>(0x1046C154, -1);
  }
  return 1;
}
VERIFY(0x0239D9A4, &daObjTapestry_c::_delete);
BOOL tapestryDelete(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239DA60, BOOL, self);
  return self->_delete();
}
VERIFY(0x0239DA60, tapestryDelete);
BOOL daObjTapestry_c::_execute() {
  WWHD_FUNC(0x0239DA64, BOOL, this);
  gabi::call(0x0239D470, this);
  u32 bg = gabi::ea((u8 *)mpBg);
  if (bg && gabi::load<u32>(bg) < 0x100)
    gabi::call(0x024F43DC, ptr(bg));
  u32 a = gabi::ea(this);
  if (gabi::load<s16>(a + 0x716) != 0)
    actionCall(a + 0x714, a);
  if (mAction != 3)
    gabi::call(0x0239C574, (daObjTapestryPacket_c *)mpPacket, this);
  return 1;
}
VERIFY(0x0239DA64, &daObjTapestry_c::_execute);
BOOL tapestryExecute(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239DB10, BOOL, self);
  return self->_execute();
}
VERIFY(0x0239DB10, tapestryExecute);
BOOL daObjTapestry_c::_draw() {
  WWHD_FUNC(0x0239E080, BOOL, this);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpModel, ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)mpModel, 0);
  if (mAction != 3)
    gabi::call(0x0239E020, (daObjTapestryPacket_c *)mpPacket);
  return 1;
}
VERIFY(0x0239E080, &daObjTapestry_c::_draw);
BOOL tapestryDraw(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239E0F0, BOOL, self);
  return self->_draw();
}
VERIFY(0x0239E0F0, tapestryDraw);
BOOL tapestryIsDelete(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239E0F4, BOOL, self);
  return 1;
}
VERIFY(0x0239E0F4, tapestryIsDelete);
void daObjTapestry_c::wait_act_proc() {
  WWHD_FUNC(0x0239EEEC, void, this);
  u32 a = gabi::ea(this);
  if (gabi::call<s32>(0x0239EA0C, this)) {
    gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) & ~0x80u);
    mBurned = 0;
    u32 bg = gabi::ea((u8 *)mpBg);
    if (bg && gabi::load<u32>(bg) < 0x100) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x020087EC, ptr(play + 0x12A0), (u8 *)mpBg);
    }
    s16 event = mEvent;
    u32 play = gabi::call<u32>(0x025200D4);
    s32 action = gabi::call<u32>(0x02544044, ptr(play + 0x52C4), event) ? 1 : 2;
    setup_action(action);
  } else {
    set_cc_pos();
    for (u32 i = 0; i < 2; i++) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x0200E240, ptr(play + 0x26A4), ptr(a + 0x3F0 + i * 0x150));
    }
  }
}
VERIFY(0x0239EEEC, &daObjTapestry_c::wait_act_proc);
void daObjTapestry_c::demo_request_act_proc() {
  WWHD_FUNC(0x0239EFE8, void, this);
  u32 a = gabi::ea(this);
  if (gabi::load<u16>(a + 0xF8) == 2) {
    mBurned = 1;
    setup_action(2);
  } else {
    u8 id = tapestryParam(this, 8, 8);
    gabi::call(0x025D7A58, this, (s16)mEvent, id, 0xFFFF, 0, 1);
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
  }
}
VERIFY(0x0239EFE8, &daObjTapestry_c::demo_request_act_proc);
void daObjTapestry_c::burn_act_proc() {
  WWHD_FUNC(0x0239F078, void, this);
  if (mBurnTimer > 0) {
    mBurnTimer = (s16)mBurnTimer - 1;
    return;
  }
  f32 target = gabi::load<f32>(0x10030848), speed = gabi::load<f32>(0x1046C1A0),
      old = mOpacity;
  s32 finished = gabi::call<s32>(0x0200F5C8, &mOpacity, target, speed);
  f32 threshold = gabi::load<f32>(0x10030868);
  if (!(old < threshold) && ((f32)mOpacity < threshold)) {
    u32 sw = tapestryParam(this, 8, 0), save = gabi::load<u32>(0x101F84DC);
    s8 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
    gabi::call(0x025B9E38, ptr(save + 0x20), sw, room);
  }
  threshold = gabi::load<f32>(0x10030860);
  if (!(old < threshold) && ((f32)mOpacity < threshold))
    gabi::call(0x0239E334, (daObjTapestryPacket_c *)mpPacket);
  if (finished)
    setup_action(3);
}
VERIFY(0x0239F078, &daObjTapestry_c::burn_act_proc);
s32 daObjTapestry_c::_create() {
  WWHD_FUNC(0x0239D658, s32, this);
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(a + 0xB4, 0x1003071C);
      gabi::call(0x028EFFD0, mTriangles, 2, 0x150, ptr(0x0239F308));
      gabi::call(0x028EFFD0, mCollisionStatus, 2, 0x3C, ptr(0x0239F394));
      flags = gabi::load<u32>(a + 0x2E4);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = create_res_load();
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, ptr(0x023997DC), 0x8A0))
      phase = 5;
    else {
      u32 play = gabi::call<u32>(0x025200D4);
      if (gabi::call<s32>(0x024EEA6C, ptr(play + 0x12A0), (u8 *)mpBg, this))
        phase = 5;
      else {
        u32 model = gabi::ea((u8 *)mpModel);
        gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
        init_mtx();
        gabi::call(0x025D674C, this, gabi::load<f32>(0x10030944),
                   gabi::load<f32>(0x10030948), gabi::load<f32>(0x1003094C),
                   gabi::load<f32>(0x10030950), gabi::load<f32>(0x10030878),
                   gabi::load<f32>(0x10030954));
        daObjTapestryPacket_c *packet =
            gabi::call<daObjTapestryPacket_c *>(0x0273AD10, 0x2520);
        if (packet)
          packet = gabi::call<daObjTapestryPacket_c *>(0x0239992C, packet);
        mpPacket = packet;
        gabi::call(0x0239CEF4, packet, this);
        init_cc();
        set_eye_pos();
        mBurned = 0;
        u8 event = tapestryParam(this, 8, 8);
        play = gabi::call<u32>(0x025200D4);
        mEvent = gabi::call<s16>(0x02543F10, ptr(play + 0x52C4), 0, event);
        if (chk_appear()) {
          mOpacity = gabi::load<f32>(0x10030840);
          setup_action(0);
        } else {
          u32 bg = gabi::ea((u8 *)mpBg);
          mOpacity = gabi::load<f32>(0x10030848);
          if (bg && gabi::load<u32>(bg) < 0x100) {
            play = gabi::call<u32>(0x025200D4);
            gabi::call(0x020087EC, ptr(play + 0x12A0), (u8 *)mpBg);
          }
          setup_action(3);
        }
      }
    }
  }
  if (gabi::load<s8>(0x1046C154) < 0) {
    s8 child = gabi::call<s8>(0x025F0A10, ptr(0x10030958), ptr(0x1046C154));
    gabi::store<s8>(0x1046C154, child);
  }
  return phase;
}
VERIFY(0x0239D658, &daObjTapestry_c::_create);
s32 tapestryCreate(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239D8B8, s32, self);
  return self->_create();
}
VERIFY(0x0239D8B8, tapestryCreate);
static u8 *tapestryHIOConstruct(u8 *self) {
  WWHD_FUNC(0x0239F198, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x6C);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::store<s8>(a, -1);
    gabi::store<u32>(a + 0x68, 0x1003070C);
    gabi::store<u8>(a + 1, 0);
    gabi::store<u8>(a + 2, 0);
    gabi::store<u32>(a + 4, 0);
    for (u32 i = 0; i < 23; i++)
      gabi::store<u32>(a + 8 + i * 4, gabi::load<u32>(0x100307C8 + i * 4));
    gabi::store<u8>(a + 0x64, 1);
  }
  return self;
}
VERIFY(0x0239F198, tapestryHIOConstruct);
static u8 *tapestryTriangleConstruct(u8 *self) {
  WWHD_FUNC(0x0239F308, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x150);
  if (self) {
    gabi::call(0x02515FB8, self);
    u32 a = gabi::ea(self);
    gabi::store<u32>(a + 0x114, 0x100015A8);
    gabi::store<u32>(a + 0x110, 0x1003066C);
    gabi::call(0x02019040, ptr(a + 0x118));
    gabi::store<u32>(a + 0x3C, 0x1004B010);
    gabi::store<u32>(a + 0x128, 0x1004B058);
    gabi::store<u32>(a + 0x114, 0x1004B068);
  }
  return self;
}
VERIFY(0x0239F308, tapestryTriangleConstruct);
static u8 *tapestryStatusConstruct(u8 *self) {
  WWHD_FUNC(0x0239F394, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x3C);
  if (self) {
    gabi::call(0x0200BD2C, self);
    gabi::call(0x02515DA0, self + 0x1C);
    gabi::store<u32>(gabi::ea(self) + 0x18, 0x1004AE88);
    gabi::store<u32>(gabi::ea(self) + 0x1C, 0x1004AEC0);
  }
  return self;
}
VERIFY(0x0239F394, tapestryStatusConstruct);
static void tapestryLightDestruct(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F3FC, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0239F3FC, tapestryLightDestruct);
static u8 *tapestryVertexConstruct(u8 *self) {
  WWHD_FUNC(0x0239F410, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x254);
  if (self) {
    gabi::call(0x027B5BD8, self + 4);
    gabi::call(0x027BF734, self + 0x158);
    gabi::store<u32>(gabi::ea(self) + 0x250, 0);
    gabi::store<u32>(gabi::ea(self) + 0x24C, 0);
  }
  return self;
}
VERIFY(0x0239F410, tapestryVertexConstruct);
static u8 *tapestryQuadConstruct(u8 *self) {
  WWHD_FUNC(0x0239F46C, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x10);
  return self;
}
VERIFY(0x0239F46C, tapestryQuadConstruct);
static u8 *tapestryFireConstruct(u8 *self) {
  WWHD_FUNC(0x0239F498, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x2C);
  if (self) {
    gabi::call(0x025A5894, self, 0, 0);
    u32 a = gabi::ea(self);
    gabi::store<u32>(a, 0x100309C8);
    f32 x = gabi::load<f32>(0x101FFBA8);
    gabi::store<f32>(a + 0x14, x);
    f32 y = gabi::load<f32>(0x101FFBAC);
    gabi::store<f32>(a + 0x18, y);
    f32 z = gabi::load<f32>(0x101FFBB0);
    gabi::store<f32>(a + 0x20, x);
    gabi::store<f32>(a + 0x1C, z);
    gabi::store<f32>(a + 0x24, y);
    gabi::store<f32>(a + 0x28, z);
  }
  return self;
}
VERIFY(0x0239F498, tapestryFireConstruct);
static cXyz *tapestryPositionCopy(cXyz *self, cXyz *other) {
  WWHD_FUNC(0x0239F51C, cXyz *, self, other);
  if (!self)
    self = gabi::call<cXyz *>(0x0273AD10, 12);
  if (self) {
    self->x = other->x;
    self->y = other->y;
    self->z = other->z;
  }
  return self;
}
VERIFY(0x0239F51C, tapestryPositionCopy);
static void tapestryLineDestruct(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F574, void, self, flags);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::store<u32>(a + 0x20, 0x1003069C);
    gabi::store<u32>(a + 0x40, 0x100306BC);
    gabi::store<u32>(a + 0x4C, 0x1003067C);
    gabi::call(0x02008DAC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0239F574, tapestryLineDestruct);
static void tapestryVertexEmpty(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F5EC, void, self, flags);
}
VERIFY(0x0239F5EC, tapestryVertexEmpty);
static void tapestryQuadEmpty(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F5F0, void, self, flags);
}
VERIFY(0x0239F5F0, tapestryQuadEmpty);
static void tapestryPositionDestruct(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F5F4, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0239F5F4, tapestryPositionDestruct);
static void tapestryVertexDestruct(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239F608, void, self, flags);
  if (self) {
    gabi::call(0x027BF880, self + 0x158, 2);
    gabi::call(0x027B5CBC, self + 4, 2);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0239F608, tapestryVertexDestruct);
static void tapestryFireEmpty(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239FAA4, void, self, flags);
}
VERIFY(0x0239FAA4, tapestryFireEmpty);
static void tapestryActorDestruct(daObjTapestry_c *self, u32 flags) {
  WWHD_FUNC(0x0239FAA8, void, self, flags);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::call(0x028F0164, ptr(a + 0x690), 2, 0x3C, 0x02515860, 0, 0);
    gabi::call(0x028F0164, ptr(a + 0x3F0), 2, 0x150, 0x025159F8, 0, 0);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0239FAA8, tapestryActorDestruct);
static void tapestryHIOEmpty(u8 *self, u32 flags) {
  WWHD_FUNC(0x0239FB3C, void, self, flags);
}
VERIFY(0x0239FB3C, tapestryHIOEmpty);
static void tapestryPacketUpdate(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239E020, void, self);
  u32 a = gabi::ea(self);
  gabi::call(0x028E9108, ptr(0x104B45F8), ptr(a + 0x2390), ptr(a + 0x23C0));
  gabi::call(0x027F0E04, ptr(gabi::load<u32>(0x104B4634)), self, 0);
  gabi::call(0x0239DD2C, self);
}
VERIFY(0x0239E020, tapestryPacketUpdate);
static void tapestryTextureCoordinates(u8 *self) {
  WWHD_FUNC(0x0239E0FC, void, self);
  u32 a = gabi::ea(self);
  for (s32 row = 0; row < 8; row++) {
    f32 uStep = gabi::load<f32>(0x10030858),
        vStep = gabi::load<f32>(0x10030934);
    f32 v = (f32)row * vStep;
    for (s32 column = 0; column < 6; column++) {
      f32 u = (f32)column * uStep;
      gabi::store<f32>(a + 0xA8 + row * 0x30 + column * 8, u);
      gabi::store<f32>(a + 0xAC + row * 0x30 + column * 8, v);
    }
  }
}
VERIFY(0x0239E0FC, tapestryTextureCoordinates);
static void tapestryDrawIndices(u8 *self) {
  WWHD_FUNC(0x0239E1A8, void, self);
  u32 a = gabi::ea(self);
  for (u32 row = 0; row < 7; row++)
    for (u32 column = 0; column < 6; column++) {
      gabi::store<u16>(a + row * 24 + column * 4, row * 6 + column);
      gabi::store<u16>(a + row * 24 + column * 4 + 2, row * 6 + column + 6);
    }
}
VERIFY(0x0239E1A8, tapestryDrawIndices);
static u8 *tapestryDrawDataConstruct(u8 *self) {
  WWHD_FUNC(0x0239E224, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x228);
  if (self) {
    u8 *result = gabi::call<u8 *>(0x0239E0FC, self);
    gabi::call(0x0239E1A8, result);
  }
  return self;
}
VERIFY(0x0239E224, tapestryDrawDataConstruct);
static void tapestryStaticInit() {
  WWHD_FUNC(0x0239F218, void);
  for (s32 i = 3; i >= 0; i--)
    gabi::store<u32>(0x1046C108 + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CD170));
  f32 low = gabi::load<f32>(0x10030970), high = gabi::load<f32>(0x10030974);
  gabi::store<f32>(0x1046C0EC, low);
  gabi::store<f32>(0x1046C0F0, high);
  gabi::call(0x028ED6F8, ptr(0x1046C104));
  gabi::call(0x028F026C, ptr(0x101CD17C));
  gabi::call(0x028EAB2C, ptr(0x1046C105));
  gabi::call(0x028F026C, ptr(0x101CD188));
  f32 value = gabi::load<f32>(0x10030978), input = gabi::load<f32>(0x10030980);
  gabi::store<f32>(0x1046C0F8, value);
  f32 second = gabi::load<f32>(0x1003097C), third = gabi::load<f32>(0x10030954);
  gabi::store<f32>(0x1046C0FC, second);
  gabi::store<f32>(0x1046C0F4, third);
  gabi::store<f32>(0x1046C100, gabi::call<f32>(0x028F4384, input));
  gabi::call(0x0239F198, ptr(0x1046C154));
  gabi::call(0x0239E224, ptr(0x1046C22C));
}
VERIFY(0x0239F218, tapestryStaticInit);
static void tapestryLightMove(daObjTapestryPLight_c *self, cXyz *position,
                              u8 *rotation) {
  WWHD_FUNC(0x0239C50C, void, self, position, rotation);
  if (self->mActive) {
    gabi::Local<cXyz> p;
    gabi::Local<be<u16>[3]> r;
    f32 x = position->x, z = position->z, y = position->y;
    u32 a = gabi::ea(rotation);
    u16 ry = gabi::load<u16>(a + 2), rz = gabi::load<u16>(a + 4),
        rx = gabi::load<u16>(a);
    p->z = z;
    (*r)[0] = rx;
    (*r)[2] = rz;
    (*r)[1] = ry;
    p->y = y;
    p->x = x;
    gabi::call(0x0239C32C, self, p.get(), r.get());
  }
}
VERIFY(0x0239C50C, tapestryLightMove);
static void tapestryPacketCalc(daObjTapestryPacket_c *self,
                               daObjTapestry_c *actor) {
  WWHD_FUNC(0x0239C574, void, self, actor);
  u32 a = gabi::ea(self), model = gabi::ea((u8 *)actor->mpModel);
  gabi::call(0x028E90D4, ptr(model ? model + 0xC8 : 0), ptr(0x1048D0CC));
  gabi::call(0x028E90D4, ptr(0x1048D0CC), ptr(a + 0x2390));
  u32 result = gabi::call<u32>(0x028E91EC, ptr(a + 0x2390), ptr(a + 0x23F0));
  u32 index = gabi::load<u32>(a + 0x20BC);
  gabi::store<u8>(a + 0x2420, result != 0);
  gabi::store<u32>(a + 0x20BC, index ^ 1);
  gabi::call(0x02399F9C, self);
  gabi::call(0x0239A38C, self);
  gabi::call(0x0239A9EC, self);
  gabi::call(0x0239B71C, self);
  gabi::call(0x0239B85C, self);
  gabi::call(0x0239BF60, self);
  gabi::call(0x0239C210, self, actor);
  gabi::Local<cXyz> p;
  gabi::Local<be<u16>[3]> r;
  u16 z = gabi::load<u16>(a + 0x24E8), x = gabi::load<u16>(a + 0x24E4);
  f32 py = gabi::load<f32>(a + 0x24DC);
  u16 y = gabi::load<u16>(a + 0x24E6);
  p->y = py;
  p->x = gabi::load<f32>(a + 0x24D8);
  (*r)[1] = y;
  (*r)[2] = z;
  f32 pz = gabi::load<f32>(a + 0x24E0);
  (*r)[0] = x;
  p->z = pz;
  gabi::call(0x0239C50C, ptr(a + 0x24F0), p.get(), r.get());
}
VERIFY(0x0239C574, tapestryPacketCalc);
static void tapestrySetHit(daObjTapestryPacket_c *self, cXyz *position,
                           cXyz *direction, s32 smoke, f32 force, f32 radius) {
  WWHD_FUNC(0x0239E3B4, void, self, position, direction, smoke, force, radius);
  u32 a = gabi::ea(self);
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(a + 0x2478 + i * 4,
                     gabi::load<f32>(gabi::ea(position) + i * 4));
  f32 x = direction->x;
  gabi::store<f32>(a + 0x2490, x);
  f32 y = direction->y;
  gabi::store<f32>(a + 0x2494, y);
  f32 z = direction->z;
  gabi::store<f32>(a + 0x24AC, radius);
  gabi::store<f32>(a + 0x2498, z);
  gabi::store<u8>(a + 0x24B0, smoke);
  gabi::store<f32>(a + 0x24A8, force);
  if (smoke) {
    f32 random = gabi::call<f32>(0x02019788);
    f32 first = gabi::load<f32>(0x1003087C);
    gabi::store<u8>(a + 0x24C1, 1);
    u8 type;
    if (random < first)
      type = 1;
    else
      type = random < gabi::load<f32>(0x10030868) ? 2 : 0;
    gabi::store<u8>(a + 0x24C2, type);
    gabi::store<u32>(a + 0x24EC, 30);
  }
}
VERIFY(0x0239E3B4, tapestrySetHit);
static void tapestryFireExecute(u8 *self, u8 *emitter) {
  WWHD_FUNC(0x0239E270, void, self, emitter);
  f32 factor = gabi::load<f32>(0x1046C1A8), limit = gabi::load<f32>(0x1046C1AC);
  gabi::Local<cXyz> scaled;
  gabi::call(0x0201AE48, self + 0x20, scaled.get(), factor);
  f32 x = scaled->x, z = scaled->z, negative = -limit;
  if (x < negative)
    x = negative;
  else
    x = ((f32)(x - limit) >= 0.0f ? limit : x);
  if (z < negative)
    z = negative;
  else
    z = ((f32)(z - limit) >= 0.0f ? limit : z);
  u32 a = gabi::ea(emitter);
  gabi::store<f32>(a + 0x30, z);
  f32 y = gabi::load<f32>(0x10030850);
  gabi::store<f32>(a + 0x28, x);
  gabi::store<f32>(a + 0x2C, y);
  gabi::call(0x025A590C, self, emitter);
}
VERIFY(0x0239E270, tapestryFireExecute);
static void tapestryFireLeap(daObjTapestryPacket_c *self, s32 row, s32 column) {
  WWHD_FUNC(0x0239A90C, void, self, row, column);
  u32 a = gabi::ea(self), index = (u32)row * 6 + (u32)column;
  if (gabi::load<u8>(a + 0x205C + index))
    return;
  bool special =
      row == 0 && ((column == 0 && gabi::load<u8>(a + 0x24C2) == 1) ||
                   (column == 5 && gabi::load<u8>(a + 0x24C2) == 2));
  u8 delay;
  if (special)
    delay = 1;
  else {
    f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10030870));
    delay = gabi::ftoi((f32)(random + gabi::load<f32>(0x10030840)));
  }
  gabi::store<u8>(a + 0x205C + index, delay);
  u32 result = gabi::call<u32>(0x0239A7C4, self, row, column);
  gabi::store<u8>(a + 0x208C + index, result);
}
VERIFY(0x0239A90C, tapestryFireLeap);
static void tapestrySpringForce(daObjTapestryPacket_c *self, cXyz *first,
                                cXyz *second, f32 rest, f32 factor) {
  WWHD_FUNC(0x0239AD14, void, self, first, second, rest, factor);
  gabi::Local<cXyz> difference, scaled, force;
  f32 sx = second->x, fy = first->y, fx = first->x, sy = second->y;
  f32 x = fx - sx, sz = second->z, y = fy - sy, fz = first->z;
  difference->x = x;
  difference->y = y;
  difference->z = fz - sz;
  f32 square = gabi::call<f32>(0x028E8DD0, difference.get());
  f32 distance = gabi::call<f32>(0x028F4384, square);
  if (distance > gabi::load<f32>(0x10030864)) {
    f32 displacement = distance - rest, stiffness = gabi::load<f32>(0x1046C160);
    f32 amount = -(f32)((f32)(displacement * stiffness) * factor);
    gabi::call(0x0201AE48, difference.get(), scaled.get(), amount);
    f32 mass = gabi::load<f32>(0x1046C15C);
    gabi::call(0x0201AEAC, scaled.get(), force.get(), (f32)(distance * mass));
    gabi::call(0x028E8D88, ptr(gabi::ea(self) + 0x2384), force.get(),
               ptr(gabi::ea(self) + 0x2384));
  }
}
VERIFY(0x0239AD14, tapestrySpringForce);
static void tapestrySmokeMove(daObjTapestryPacket_c *self,
                              daObjTapestry_c *actor) {
  WWHD_FUNC(0x0239C210, void, self, actor);
  u32 a = gabi::ea(self);
  s32 timer = gabi::load<s32>(a + 0x24EC);
  if (timer > 0) {
    timer--;
    gabi::store<s32>(a + 0x24EC, timer);
    if (timer <= 0)
      gabi::call(0x0239C158, self);
  }
  f32 rowFraction = gabi::load<f32>(a + 0x24B8);
  f32 rows = gabi::load<f32>(0x10030874), columns = gabi::load<f32>(0x10030878);
  f32 rowValue = rowFraction * rows,
      columnValue = gabi::load<f32>(a + 0x24BC) * columns;
  u32 buffer = gabi::load<u32>(a + 0x20BC), row = (u32)gabi::ftoi(rowValue),
      column = (u32)gabi::ftoi(columnValue);
  u32 normal = a + 0x106C + buffer * 0x6C0 + (row * 6 + column) * 12 + 0x240;
  f32 nx = gabi::load<f32>(normal), nz = gabi::load<f32>(normal + 8);
  s32 yaw = gabi::call<s32>(0x020195B0, nx, -nz);
  nz = gabi::load<f32>(normal + 8);
  f32 square = nz * nz;
  nx = gabi::load<f32>(normal);
  f32 length = gabi::call<f32>(0x028F4384, gabi::fmadds(nx, nx, square));
  s32 pitch = gabi::call<s32>(0x020195B0, gabi::load<f32>(normal + 4), length);
  u32 owner = gabi::ea(actor);
  gabi::store<f32>(a + 0x24D8, gabi::load<f32>(owner + 0x37C));
  gabi::store<f32>(a + 0x24DC, gabi::load<f32>(owner + 0x380));
  f32 z = gabi::load<f32>(owner + 0x384);
  gabi::store<u16>(a + 0x24E4, pitch);
  gabi::store<u16>(a + 0x24E6, yaw);
  gabi::store<f32>(a + 0x24E0, z);
  gabi::store<u16>(a + 0x24E8, 0);
}
VERIFY(0x0239C210, tapestrySmokeMove);
static void tapestryVelocity(daObjTapestryPacket_c *self, s32 row, s32 column) {
  WWHD_FUNC(0x0239B498, void, self, row, column);
  u32 a = gabi::ea(self), index = (u32)row * 6 + (u32)column;
  u8 flags = gabi::load<u8>(a + 0x202C + index);
  u8 *velocity = ptr(a + 0x1DEC + index * 12);
  f32 friction = -gabi::load<f32>((flags & 2) ? 0x1046C168 : 0x1046C164);
  gabi::call(0x028E8D88, velocity, ptr(a + 0x2384), velocity);
  gabi::Local<cXyz> drag;
  gabi::call(0x0201AE48, velocity, drag.get(), friction);
  gabi::call(0x028E8D88, velocity, drag.get(), velocity);
}
VERIFY(0x0239B498, tapestryVelocity);
static void tapestrySetPointLight(daObjTapestryPLight_c *self, cXyz *position,
                                  u8 *rotation) {
  WWHD_FUNC(0x0239C32C, void, self, position, rotation);
  u32 a = gabi::ea(self);
  f32 half = gabi::load<f32>(0x10030860), one = gabi::load<f32>(0x10030840);
  f32 random = gabi::call<f32>(0x020198D8, half);
  gabi::call(0x0200ED84, ptr(a + 0x2C), (f32)(random + one), half,
             gabi::load<f32>(0x100308C8));
  u32 x = gabi::load<u32>(gabi::ea(position));
  f32 intensity = gabi::load<f32>(a + 0x2C);
  f32 range = gabi::load<f32>(0x100308CC) * intensity;
  gabi::store<u32>(a + 4, x);
  u32 y = gabi::load<u32>(gabi::ea(position) + 4);
  gabi::store<u32>(a + 8, y);
  u32 z = gabi::load<u32>(gabi::ea(position) + 8);
  s16 signedRange = (s16)gabi::ftoi(range);
  gabi::store<u16>(a + 0x10, 600);
  gabi::store<u16>(a + 0x12, 400);
  gabi::store<u32>(a + 0xC, z);
  gabi::store<u16>(a + 0x14, 120);
  f32 constant = gabi::load<f32>(0x100308D0);
  gabi::store<f32>(a + 0x18, (f32)signedRange);
  gabi::store<f32>(a + 0x1C, constant);
  if (intensity > one) {
    f32 scale = intensity * gabi::load<f32>(0x100308D4);
    gabi::Local<cXyz> size;
    size->x = scale;
    size->y = scale;
    size->z = scale;
    u32 emitter = gabi::load<u32>(a + 0x28);
    if (!emitter) {
      u32 play = gabi::call<u32>(0x025200D4),
          particles = gabi::load<u32>(play + 0x5AB0);
      self->mpEmitter =
          gabi::call<u8 *>(0x025A847C, ptr(particles), 4, 0x4004, position,
                           rotation, size.get(), 255, 0, -1, 0, 0, 0);
    } else {
      gabi::store<f32>(emitter + 0x238, scale);
      gabi::store<f32>(emitter + 0x23C, scale);
      gabi::store<f32>(emitter + 0x240, scale);
      gabi::store<u32>(gabi::load<u32>(a + 0x28) + 0x1E4, 0x1047B2DC);
    }
  }
}
VERIFY(0x0239C32C, tapestrySetPointLight);
static void tapestrySmokeSet(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239C158, void, self);
  u32 a = gabi::ea(self);
  if (gabi::load<u32>(0x1046C224) == 0) {
    f32 one = gabi::load<f32>(0x10030840);
    gabi::store<u32>(0x1046C224, 1);
    gabi::store<f32>(0x1046C148, one);
    gabi::store<f32>(0x1046C150, one);
    gabi::store<f32>(0x1046C14C, one);
  }
  gabi::call(0x025A5AC8, ptr(a + 0x24C4));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x025A847C, ptr(gabi::load<u32>(play + 0x5AB0)), 2, 0xA329,
             ptr(a + 0x24D8), ptr(a + 0x24E4), ptr(0x1046C148), 255,
             ptr(a + 0x24C4), -1, 0, 0, 0);
  gabi::call(0x0239C120, ptr(a + 0x24F0));
}
VERIFY(0x0239C158, tapestrySmokeSet);
static u8 tapestryStartFire(daObjTapestryPacket_c *self, s32 row, s32 column) {
  WWHD_FUNC(0x0239A7C4, u8, self, row, column);
  u32 a = gabi::ea(self);
  u8 result = 255;
  if (gabi::load<s32>(a + 0x2380) < 16 &&
      gabi::call<s32>(0x0239A550, self, row, column)) {
    u32 index = gabi::load<u32>(a + 0x2380),
        callback = a + 0x20C0 + index * 0x2C;
    cXyz *position = gabi::call<cXyz *>(0x0239A7A0, self, row, column);
    gabi::Local<cXyz> transformed, size;
    gabi::call(0x028E8F64, ptr(a + 0x2390), position, transformed.get());
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x14 + i * 4,
                       gabi::load<f32>(gabi::ea(transformed.get()) + i * 4));
    f32 minimum = gabi::load<f32>(0x1046C1B0),
        maximum = gabi::load<f32>(0x1046C1B4);
    f32 random = gabi::call<f32>(0x020198D8, (f32)(maximum - minimum));
    f32 scale = minimum + random;
    size->x = scale;
    size->z = scale;
    size->y = scale;
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x025A847C, ptr(gabi::load<u32>(play + 0x5AB0)), 0, 0x1EA,
               ptr(callback + 0x14), 0, size.get(), 255, ptr(callback), -1, 0,
               0, 0);
    index = gabi::load<u32>(a + 0x2380);
    gabi::store<u32>(a + 0x2380, index + 1);
    result = index;
  }
  return result;
}
VERIFY(0x0239A7C4, tapestryStartFire);
static void tapestryPositions(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239B71C, void, self);
  u32 a = gabi::ea(self), buffer = gabi::load<u32>(a + 0x20BC);
  u32 current = a + 0x106C + buffer * 0x6C0,
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  for (s32 row = 0; row < 8; row++)
    for (s32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column;
      if (row == 0 && !(gabi::load<u8>(a + 0x202C + index) & 1))
        continue;
      for (u32 i = 0; i < 3; i++)
        gabi::store<f32>(a + 0x2384 + i * 4,
                         gabi::load<f32>(0x101FFBA8 + i * 4));
      gabi::call(0x0239AE28, self, row, column);
      u8 *returned = gabi::call<u8 *>(0x0239B100, self);
      gabi::call(0x0239B12C, returned, row, column);
      gabi::call(0x0239B3AC, self, row, column);
      gabi::call(0x0239B498, self, row, column);
      gabi::Local<cXyz> sum;
      gabi::call(0x0201AD78, ptr(previous + index * 12), sum.get(),
                 ptr(a + 0x1DEC + index * 12));
      for (u32 i = 0; i < 3; i++)
        gabi::store<u32>(current + index * 12 + i * 4,
                         gabi::load<u32>(gabi::ea(sum.get()) + i * 4));
      gabi::call(0x0239B578, self, row, column);
    }
}
VERIFY(0x0239B71C, tapestryPositions);
static void tapestryHitForce(daObjTapestryPacket_c *self, s32 row, s32 column) {
  WWHD_FUNC(0x0239B3AC, void, self, row, column);
  u32 a = gabi::ea(self);
  f32 strength = gabi::load<f32>(a + 0x24A8),
      epsilon = gabi::load<f32>(0x10030864);
  if (strength > epsilon) {
    f32 rows = gabi::load<f32>(0x10030874), rf = (f32)row / rows;
    f32 columns = gabi::load<f32>(0x10030878), cf = (f32)column / columns;
    f32 hr = gabi::load<f32>(a + 0x24B8), hc = gabi::load<f32>(a + 0x24BC);
    f32 dc = cf - hc, dr = rf - hr, square = dc * dc;
    f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dr, dr, square));
    f32 radius = gabi::load<f32>(a + 0x24AC);
    if (distance < radius) {
      f32 attenuation = gabi::load<f32>(a + 0x24B4),
          product = distance * attenuation;
      strength = gabi::load<f32>(a + 0x24A8);
      product = product * strength;
      f32 factor = gabi::load<f32>(0x1046C198);
      gabi::Local<cXyz> force;
      gabi::call(0x0201AE48, ptr(a + 0x249C), force.get(),
                 (f32)(product * factor));
      gabi::call(0x028E8D88, ptr(a + 0x2384), force.get(), ptr(a + 0x2384));
    }
  }
}
VERIFY(0x0239B3AC, tapestryHitForce);
static void tapestryWaveForce(daObjTapestryPacket_c *self, s32 row,
                              s32 column) {
  WWHD_FUNC(0x0239B12C, void, self, row, column);
  u32 a = gabi::ea(self), buffer = gabi::load<u32>(a + 0x20BC),
      index = (u32)row * 6 + (u32)column;
  f32 rowScale = gabi::fmadds((f32)row, gabi::load<f32>(0x10030890),
                              gabi::load<f32>(0x10030868));
  u32 previous = a + 0x106C + (buffer ^ 1) * 0x6C0 + index * 12,
      acceleration = a + 0x2384;
  gabi::Local<cXyz> wind, flutter, top, middle, bottom;
  gabi::call(0x0201AE48, ptr(a + 0x2430), wind.get(), rowScale);
  gabi::call(0x028E8D88, ptr(acceleration), wind.get(), ptr(acceleration));
  gabi::call(0x0201AE48, ptr(a + 0x2440), wind.get(), rowScale);
  gabi::call(0x028E8D88, ptr(acceleration), wind.get(), ptr(acceleration));
  u8 active = gabi::load<u8>(a + 0x24C0);
  f32 attenuation = gabi::load<f32>(0x1003084C);
  if (active) {
    f32 center = gabi::load<f32>(0x10030894),
        distance = std::fabs((f32)column - center);
    f32 scale = gabi::fmadds((f32)(center - distance),
                             gabi::load<f32>(0x10030898), attenuation);
    cXyz *out = row < 2 ? top.get() : row < 5 ? middle.get() : bottom.get();
    u32 source = row < 2 ? a + 0x2454 : row < 5 ? a + 0x2460 : a + 0x246C;
    gabi::call(0x0201AE48, ptr(source), out, scale);
    gabi::call(0x028E8D88, ptr(acceleration), out, ptr(acceleration));
  }
  rowScale = gabi::load<f32>(0x1046C174) * rowScale;
  f32 half = gabi::load<f32>(0x10030860);
  for (u32 i = 0; i < 3; i++) {
    f32 random = gabi::call<f32>(0x02019788), delta = random - half,
        current = gabi::load<f32>(acceleration + i * 4);
    gabi::store<f32>(acceleration + i * 4,
                     gabi::fmadds(delta, rowScale, current));
  }
  f32 z = gabi::load<f32>(previous + 8), limit = gabi::load<f32>(0x1003089C);
  if (z < limit) {
    f32 threshold = gabi::load<f32>(0x100308A0),
        force = gabi::load<f32>(0x1046C194);
    f32 current = gabi::load<f32>(acceleration + 8);
    if (z > threshold)
      gabi::store<f32>(
          acceleration + 8,
          gabi::fmadds(force, gabi::load<f32>(0x10030850), current));
    else {
      if (z > gabi::load<f32>(0x100308A4))
        force = force * attenuation;
      gabi::store<f32>(acceleration + 8, (f32)(current + force));
    }
  }
}
VERIFY(0x0239B12C, tapestryWaveForce);
static void tapestrySpringNeighbors(daObjTapestryPacket_c *self, s32 row,
                                    s32 column) {
  WWHD_FUNC(0x0239AE28, void, self, row, column);
  u32 a = gabi::ea(self), index = (u32)row * 6 + (u32)column;
  u32 previous = a + 0x106C + (gabi::load<u32>(a + 0x20BC) ^ 1) * 0x6C0;
  u8 *position = ptr(previous + index * 12);
  f32 axial = gabi::load<f32>(0x1046C188),
      diagonal = gabi::load<f32>(0x1046C190);
  auto spring = [&](u32 neighbor, f32 rest, f32 factor) {
    gabi::call(0x0239AD14, self, position, ptr(previous + neighbor * 12), rest,
               factor);
  };
  if (row - 1 >= 0) {
    u32 above = index - 6;
    spring(above, gabi::load<f32>(0x1046C0F8), axial);
    if (column - 1 >= 0)
      spring(above - 1, gabi::load<f32>(0x1046C100), diagonal);
    if (column + 1 < 6)
      spring(above + 1, gabi::load<f32>(0x1046C100), diagonal);
  }
  if (row + 1 < 8) {
    u32 below = index + 6;
    spring(below, gabi::load<f32>(0x1046C0F8), axial);
    if (column - 1 >= 0)
      spring(below - 1, gabi::load<f32>(0x1046C100), diagonal);
    if (column + 1 < 6)
      spring(below + 1, gabi::load<f32>(0x1046C100), diagonal);
  }
  if (column - 1 >= 0)
    spring(index - 1, gabi::load<f32>(0x1046C0FC), axial);
  if (column + 1 < 6)
    spring(index + 1, gabi::load<f32>(0x1046C0FC), axial);
  f32 vertical = gabi::load<f32>(0x1046C0F8),
      horizontal = gabi::load<f32>(0x1046C0FC);
  vertical = vertical + vertical;
  horizontal = horizontal + horizontal;
  f32 bending = gabi::load<f32>(0x1046C18C);
  if (row - 2 >= 0)
    spring(index - 12, vertical, bending);
  if (row + 2 < 8)
    spring(index + 12, vertical, bending);
  if (column - 2 >= 0)
    spring(index - 2, horizontal, bending);
  if (column + 2 < 6)
    spring(index + 2, horizontal, bending);
}
VERIFY(0x0239AE28, tapestrySpringNeighbors);
static void tapestrySignedColor(be<f32> *out, be<s16> *color) {
  WWHD_FUNC(0x0239DBB4, void, out, color);
  s16 r = color[0], b = color[2], g = color[1], a = color[3];
  f32 divisor = gabi::load<f32>(0x100308D8);
  f32 rf = (f32)r / divisor, gf = (f32)g / divisor, bf = (f32)b / divisor,
      af = (f32)a / divisor;
  out[0] = rf;
  out[1] = gf;
  out[2] = bf;
  out[3] = af;
}
VERIFY(0x0239DBB4, tapestrySignedColor);
static void tapestryByteColor(be<f32> *out, u8 *color) {
  WWHD_FUNC(0x0239DC78, void, out, color);
  u32 address = gabi::ea(color);
  u8 r = gabi::load<u8>(address), g = gabi::load<u8>(address + 1);
  f32 divisor = gabi::load<f32>(0x100308D8);
  u8 b = gabi::load<u8>(address + 2), a = gabi::load<u8>(address + 3);
  f32 rf = (f32)r / divisor, gf = (f32)g / divisor, bf = (f32)b / divisor,
      af = (f32)a / divisor;
  out[0] = rf;
  out[1] = gf;
  out[2] = bf;
  out[3] = af;
}
VERIFY(0x0239DC78, tapestryByteColor);
static void tapestryMatrixCopy(be<f32> *out, be<f32> *matrix) {
  WWHD_FUNC(0x0239DB14, void, out, matrix);
  f32 copy[12];
  for (u32 i = 0; i < 12; i++)
    copy[i] = matrix[i];
  for (u32 i = 0; i < 12; i++)
    out[i] = copy[i];
}
VERIFY(0x0239DB14, tapestryMatrixCopy);
static void tapestryHitCoordinates(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239A38C, void, self);
  u32 a = gabi::ea(self);
  f32 half = gabi::load<f32>(0x10030860), epsilon = gabi::load<f32>(0x10030864);
  f32 strength = gabi::load<f32>(a + 0x24A8) * half;
  gabi::store<f32>(a + 0x24A8, strength);
  if (strength > epsilon) {
    gabi::call(0x028E8F64, ptr(a + 0x23F0), ptr(a + 0x2478), ptr(a + 0x2484));
    gabi::call(0x028E9044, ptr(a + 0x23F0), ptr(a + 0x2490), ptr(a + 0x249C));
    u32 previous = a + 0x106C + (gabi::load<u32>(a + 0x20BC) ^ 1) * 0x6C0;
    gabi::Local<cXyz> crossing;
    f32 left = gabi::call<f32>(0x0201794C, ptr(previous), ptr(previous + 0x1F8),
                               ptr(a + 0x2484), crossing.get());
    f32 right =
        gabi::call<f32>(0x0201794C, ptr(previous + 0x3C), ptr(previous + 0x234),
                        ptr(a + 0x2484), crossing.get());
    f32 top = gabi::call<f32>(0x0201794C, ptr(previous), ptr(previous + 0x3C),
                              ptr(a + 0x2484), crossing.get());
    f32 bottom =
        gabi::call<f32>(0x0201794C, ptr(previous + 0x1F8),
                        ptr(previous + 0x234), ptr(a + 0x2484), crossing.get());
    f32 column = (f32)(top + bottom) * half, row = (f32)(left + right) * half;
    gabi::store<f32>(a + 0x24BC, column);
    f32 zero = gabi::load<f32>(0x10030848), one = gabi::load<f32>(0x10030840);
    gabi::store<f32>(a + 0x24B8, row);
    if (row < zero)
      row = zero;
    else
      row = (f32)(row - one) >= 0 ? one : row;
    column = gabi::load<f32>(a + 0x24BC);
    if (column < zero)
      column = zero;
    else
      column = (f32)(column - one) >= 0 ? one : column;
    gabi::store<f32>(a + 0x24B8, row);
    f32 radius = gabi::load<f32>(a + 0x24AC);
    gabi::store<f32>(a + 0x24BC, column);
    gabi::store<f32>(a + 0x24B4, radius > one ? (f32)(one / radius) : one);
  }
}
VERIFY(0x0239A38C, tapestryHitCoordinates);
static void tapestryPositionCorrection(daObjTapestryPacket_c *self, s32 row,
                                       s32 column) {
  WWHD_FUNC(0x0239B578, void, self, row, column);
  u32 a = gabi::ea(self), index = (u32)row * 6 + (u32)column;
  u32 position = a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0 + index * 12;
  f32 one = gabi::load<f32>(0x10030840), z = gabi::load<f32>(position + 8);
  if (z < one)
    gabi::store<f32>(position + 8, one);
  if (!gabi::load<u8>(a + 0x24C1))
    return;
  u32 check = 0x1046C1C0;
  if (gabi::load<u32>(0x1046C220) == 0) {
    gabi::store<u32>(0x1046C220, 1);
    gabi::call(0x02008E0C, ptr(check));
    gabi::store<u32>(check + 0x50, 1);
    gabi::store<u32>(check, check + 0x40);
    gabi::store<u32>(check + 0x10, 0x100306CC);
    gabi::store<u8>(check + 0x45, 0);
    gabi::store<u8>(check + 0x4A, 0);
    gabi::store<u32>(check + 0x4C, 0x100306EC);
    gabi::store<u8>(check + 0x44, 1);
    gabi::store<u32>(check + 4, check + 0x4C);
    gabi::store<u32>(check + 0x20, 0x100306DC);
    gabi::store<u8>(check + 0x48, 0);
    gabi::store<u8>(check + 0x49, 0);
    gabi::store<u8>(check + 0x47, 0);
    gabi::store<u8>(check + 0x46, 0);
    gabi::store<u32>(check + 0x40, 0x100306FC);
    gabi::call(0x028F026C, ptr(0x101CD118));
  }
  gabi::Local<cXyz> world;
  gabi::call(0x028E8F64, ptr(a + 0x2390), ptr(position), world.get());
  f32 wz = world->z, wy = world->y, offset = gabi::load<f32>(0x100308A8),
      wx = world->x;
  gabi::store<f32>(check + 0x2C, wz);
  gabi::store<f32>(check + 0x24, wx);
  gabi::store<f32>(check + 0x28, (f32)(wy + offset));
  u32 play = gabi::call<u32>(0x025200D4);
  f32 height = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ptr(check));
  height = height + gabi::load<f32>(0x10030880);
  wy = world->y;
  if (wy < height) {
    world->y = height;
    gabi::call(0x028E8F64, ptr(a + 0x23F0), world.get(), ptr(position));
    gabi::store<u8>(a + 0x202C + index, gabi::load<u8>(a + 0x202C + index) | 2);
  } else
    gabi::store<u8>(a + 0x202C + index,
                    gabi::load<u8>(a + 0x202C + index) & 0xFD);
}
VERIFY(0x0239B578, tapestryPositionCorrection);
static BOOL tapestryCanStartFire(daObjTapestryPacket_c *self, s32 row,
                                 s32 column) {
  WWHD_FUNC(0x0239A550, BOOL, self, row, column);
  u32 a = gabi::ea(self);
  f32 diagonal = gabi::load<f32>(0x1003086C),
      axial = gabi::load<f32>(0x10030868);
  BOOL allowed = 1;
  auto occupied = [&](s32 r, s32 c) {
    return gabi::load<u8>(a + 0x208C + (u32)r * 6 + (u32)c) != 255;
  };
  for (s32 neighbor : {row - 1, row + 1}) {
    if (neighbor == row - 1 ? neighbor < 0 : neighbor >= 8)
      continue;
    if (gabi::call<f32>(0x02019788) < axial && occupied(neighbor, column))
      allowed = 0;
    if (gabi::call<f32>(0x02019788) < diagonal && column - 1 >= 0 &&
        occupied(neighbor, column - 1))
      allowed = 0;
    if (gabi::call<f32>(0x02019788) < diagonal && column + 1 < 6 &&
        occupied(neighbor, column + 1))
      allowed = 0;
  }
  if (gabi::call<f32>(0x02019788) < axial && column - 1 >= 0 &&
      occupied(row, column - 1))
    allowed = 0;
  if (gabi::call<f32>(0x02019788) < axial && column + 1 < 6 &&
      occupied(row, column + 1))
    allowed = 0;
  if (occupied(row, column))
    allowed = 0;
  if (row == 0)
    allowed = 0;
  return allowed;
}
VERIFY(0x0239A550, tapestryCanStartFire);
static void tapestryFireSimulation(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239A9EC, void, self);
  u32 a = gabi::ea(self);
  if (gabi::load<u8>(a + 0x24B0)) {
    f32 r = gabi::load<f32>(a + 0x24B8), rows = gabi::load<f32>(0x10030874),
        columns = gabi::load<f32>(0x10030878);
    f32 rv = r * rows, cv = gabi::load<f32>(a + 0x24BC) * columns;
    u32 index = (u32)gabi::ftoi(rv) * 6 + (u32)gabi::ftoi(cv);
    gabi::store<u8>(a + 0x205C + index, gabi::load<u8>(a + 0x205C + index) + 1);
    gabi::store<u8>(a + 0x24B0, 0);
  }
  f32 edgeChance = gabi::load<f32>(0x1003087C),
      normalChance = gabi::load<f32>(0x10030868);
  for (s32 row = 0; row < 8; row++)
    for (s32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column;
      u8 timer = gabi::load<u8>(a + 0x205C + index);
      if (!timer || timer >= gabi::load<u8>(0x1046C19D))
        continue;
      bool special =
          row == 0 && ((column == 0 && gabi::load<u8>(a + 0x24C2) == 1) ||
                       (column == 5 && gabi::load<u8>(a + 0x24C2) == 2));
      if (gabi::call<f32>(0x02019788) < (special ? edgeChance : normalChance))
        gabi::store<u8>(a + 0x205C + index,
                        gabi::load<u8>(a + 0x205C + index) + 1);
      timer = gabi::load<u8>(a + 0x205C + index);
      if (timer < gabi::load<u8>(0x1046C19C) ||
          (gabi::load<u8>(a + 0x202C + index) & 1))
        continue;
      gabi::store<u8>(a + 0x202C + index,
                      gabi::load<u8>(a + 0x202C + index) | 1);
      if (row - 1 >= 0) {
        gabi::call(0x0239A90C, self, row - 1, column);
        if (column - 1 >= 0)
          gabi::call(0x0239A90C, self, row - 1, column - 1);
        if (column + 1 < 6)
          gabi::call(0x0239A90C, self, row - 1, column + 1);
      }
      if (row + 1 < 8) {
        gabi::call(0x0239A90C, self, row + 1, column);
        if (column - 1 >= 0)
          gabi::call(0x0239A90C, self, row + 1, column - 1);
        if (column + 1 < 6)
          gabi::call(0x0239A90C, self, row + 1, column + 1);
      }
      if (column - 1 >= 0)
        gabi::call(0x0239A90C, self, row, column - 1);
      if (column + 1 < 6)
        gabi::call(0x0239A90C, self, row, column + 1);
    }
  if (!gabi::load<u8>(a + 0x24C0)) {
    bool burned = true;
    for (u32 column = 0; column < 6; column++)
      if (!(gabi::load<u8>(a + 0x202C + column) & 1)) {
        burned = false;
        break;
      }
    if (burned)
      gabi::store<u8>(a + 0x24C0, 1);
  }
}
VERIFY(0x0239A9EC, tapestryFireSimulation);
static void tapestryParticlePositions(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239BF60, void, self);
  u32 a = gabi::ea(self);
  if (gabi::load<s32>(a + 0x2380) <= 0)
    return;
  u32 buffer = gabi::load<u32>(a + 0x20BC),
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  gabi::Local<cXyz[144]> old, current;
  for (u32 group = 0; group < 3; group++)
    gabi::call(0x028F0294, &(*old)[group * 48], 48, 12, 0x0239F51C,
               ptr(previous + group * 0x240));
  u32 now = a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0;
  for (u32 group = 0; group < 3; group++)
    gabi::call(0x028F0294, &(*current)[group * 48], 48, 12, 0x0239F51C,
               ptr(now + group * 0x240));
  for (u32 index = 0; index < 48; index++) {
    u8 id = gabi::load<u8>(a + 0x208C + index);
    if (id == 255)
      continue;
    gabi::Local<cXyz> world, difference, copied, velocity;
    gabi::call(0x028E8F64, ptr(a + 0x2390), &(*current)[index], world.get());
    u32 callback = a + 0x20C0 + (u32)id * 0x2C;
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x14 + i * 4,
                       gabi::load<f32>(gabi::ea(world.get()) + i * 4));
    gabi::call(0x0201ADE0, &(*current)[index], difference.get(),
               &(*old)[index]);
    f32 z = difference->z, y = difference->y;
    copied->z = z;
    f32 x = difference->x;
    copied->y = y;
    copied->x = x;
    gabi::call(0x028E9044, ptr(a + 0x2390), copied.get(), velocity.get());
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x20 + i * 4,
                       gabi::load<f32>(gabi::ea(velocity.get()) + i * 4));
  }
}
VERIFY(0x0239BF60, tapestryParticlePositions);
static void tapestryPacketInit(daObjTapestryPacket_c *self, u8 *owner) {
  WWHD_FUNC(0x0239CEF4, void, self, owner);
  u32 a = gabi::ea(self);
  f32 zero = gabi::load<f32>(0x10030848);
  if (!gabi::load<u32>(0x1046C228)) {
    gabi::store<f32>(0x1046C118, zero);
    gabi::store<u32>(0x1046C228, 1);
    gabi::store<f32>(0x1046C11C, zero);
    gabi::store<f32>(0x1046C120, gabi::load<f32>(0x100308AC));
  }
  gabi::store<u32>(a + 0x18, gabi::ea(owner));
  f32 width = gabi::load<f32>(0x1046C0F4), nz = gabi::load<f32>(0x1046C120),
      ny = gabi::load<f32>(0x1046C11C);
  f32 tangentX = gabi::load<f32>(0x101FFBCC),
      tangentZ = gabi::load<f32>(0x101FFBD4), nx = gabi::load<f32>(0x1046C118),
      tangentY = gabi::load<f32>(0x101FFBD0);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), -width, gabi::load<f32>(0x10030928),
             gabi::load<f32>(0x1003089C));
  gabi::call(0x025F2518, gabi::load<f32>(0x1003092C),
             gabi::load<f32>(0x10030930), gabi::load<f32>(0x10030840));
  f32 rowStep = gabi::load<f32>(0x10030934);
  gabi::Local<cXyz> vertex;
  vertex->z = zero;
  for (u32 buffer = 0; buffer < 2; buffer++) {
    f32 columnStep = gabi::load<f32>(0x10030858);
    for (s32 row = 0; row < 8; row++) {
      vertex->y = (f32)(7 - row) * rowStep;
      for (s32 column = 0; column < 6; column++) {
        vertex->x = (f32)column * columnStep;
        u32 position = a + 0x106C + buffer * 0x6C0 + (row * 6 + column) * 12;
        gabi::call(0x028E8F64, ptr(0x1048D0CC), vertex.get(), ptr(position));
        gabi::store<f32>(position + 0x484, ny);
        gabi::store<f32>(position + 0x488, nz);
        gabi::store<f32>(position + 0x480, nx);
        gabi::store<f32>(position + 0x244, tangentY);
        gabi::store<f32>(position + 0x240, tangentX);
        gabi::store<f32>(position + 0x248, tangentZ);
      }
    }
  }
  u8 *result = gabi::call<u8 *>(0x02399F90, self);
  gabi::call(0x0239C574, result, owner);
  gabi::call(0x0239C678, self);
}
VERIFY(0x0239CEF4, tapestryPacketInit);
static void tapestryWind(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x02399F9C, void, self);
  u32 a = gabi::ea(self);
  f32 reduction = gabi::load<f32>(0x1003084C);
  gabi::call(0x028E8E64, ptr(a + 0x2424), ptr(a + 0x2424), reduction);
  f32 square = gabi::call<f32>(0x028E8DD0, ptr(a + 0x2424)),
      epsilon = gabi::load<f32>(0x10030850);
  u8 valid = gabi::load<u8>(a + 0x2420);
  f32 zero = gabi::load<f32>(0x10030848);
  if (square < epsilon) {
    gabi::store<f32>(a + 0x2428, zero);
    gabi::store<f32>(a + 0x2424, zero);
    gabi::store<f32>(a + 0x242C, zero);
  }
  if (valid) {
    gabi::call(0x028E9044, ptr(a + 0x23F0), ptr(a + 0x2424), ptr(a + 0x2430));
    f32 z = gabi::load<f32>(a + 0x2438), x = gabi::load<f32>(a + 0x2430);
    if (z < zero) {
      z = z * gabi::load<f32>(0x10030854);
      gabi::store<f32>(a + 0x2438, z);
    }
    f32 multiplier = reduction;
    if (std::fabs(x) > reduction && std::fabs(z) < gabi::load<f32>(0x10030858))
      multiplier = gabi::load<f32>(0x1003085C);
    gabi::store<f32>(a + 0x2430, (f32)(x * multiplier));
    gabi::call(0x028E8E64, ptr(a + 0x2430), ptr(a + 0x2430),
               gabi::load<f32>(0x1046C16C));
  } else
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(a + 0x2430 + i * 4, gabi::load<f32>(0x101FFBA8 + i * 4));
  s16 phase = (s16)(gabi::load<s16>(a + 0x243C) + 500);
  gabi::store<s16>(a + 0x243C, phase);
  gabi::Local<cXyz> scaled;
  if (phase > 0) {
    f32 factor = gabi::load<f32>(0x1046C170),
        sine = gabi::load<f32>(0x104A44F8 + ((u16)phase >> 3) * 8);
    gabi::call(0x0201AE48, ptr(0x101FFBCC), scaled.get(), (f32)(sine * factor));
    for (u32 i = 0; i < 3; i++)
      gabi::store<u32>(a + 0x2440 + i * 4,
                       gabi::load<u32>(gabi::ea(scaled.get()) + i * 4));
  } else
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(a + 0x2440 + i * 4, gabi::load<f32>(0x101FFBA8 + i * 4));
  if (!gabi::load<u8>(a + 0x24C0))
    return;
  if (!gabi::load<u32>(0x1046C214)) {
    f32 y = gabi::load<f32>(0x104A7F90), z = gabi::load<f32>(0x104A7F94);
    gabi::store<u32>(0x1046C214, 1);
    gabi::store<f32>(0x1046C12C, z);
    gabi::store<f32>(0x1046C124, zero);
    gabi::store<f32>(0x1046C128, y);
  }
  if (!gabi::load<u32>(0x1046C218)) {
    f32 y = gabi::load<f32>(0x104A6FF0), z = gabi::load<f32>(0x104A6FF4);
    gabi::store<f32>(0x1046C138, z);
    gabi::store<f32>(0x1046C130, zero);
    gabi::store<u32>(0x1046C218, 1);
    gabi::store<f32>(0x1046C134, y);
  }
  if (!gabi::load<u32>(0x1046C21C)) {
    f32 z = gabi::load<f32>(0x104A5A74), y = gabi::load<f32>(0x104A5A70);
    gabi::store<u32>(0x1046C21C, 1);
    gabi::store<f32>(0x1046C144, z);
    gabi::store<f32>(0x1046C13C, zero);
    gabi::store<f32>(0x1046C140, y);
  }
  f32 one = gabi::load<f32>(0x10030840);
  s16 middle = (s16)(gabi::load<s16>(a + 0x244E) + 1133);
  gabi::store<s16>(a + 0x244E, middle);
  s16 top = (s16)(gabi::load<s16>(a + 0x244C) + 1366);
  gabi::store<s16>(a + 0x244C, top);
  s16 bottom = (s16)(gabi::load<s16>(a + 0x2450) + 811);
  gabi::store<s16>(a + 0x2450, bottom);
  for (u32 i = 0; i < 3; i++) {
    u16 angle = gabi::load<u16>(a + 0x244C + i * 2);
    f32 sine = gabi::load<f32>(0x104A44F8 + (angle >> 3) * 8);
    f32 factor = gabi::load<f32>(0x1046C178 + i * 4);
    gabi::call(0x0201AE48, ptr(0x1046C124 + i * 12), scaled.get(),
               (f32)((f32)(sine + one) * factor));
    for (u32 j = 0; j < 3; j++)
      gabi::store<u32>(a + 0x2454 + i * 12 + j * 4,
                       gabi::load<u32>(gabi::ea(scaled.get()) + j * 4));
  }
}
VERIFY(0x02399F9C, tapestryWind);
static daObjTapestryPacket_c *
tapestryPacketConstruct(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239992C, daObjTapestryPacket_c *, self);
  if (!self)
    self = gabi::call<daObjTapestryPacket_c *>(0x0273AD10, 0x2520);
  if (!self)
    return self;
  u32 a = gabi::ea(self);
  gabi::call(0x027F1278, self);
  gabi::store<u32>(a + 0xC, 0x10030998);
  gabi::store<u32>(a + 0x98, 0);
  u32 list = a + 0x9C;
  if (!list)
    list = gabi::call<u32>(0x0273AD10, 8);
  if (list) {
    gabi::store<u32>(list + 4, 0);
    gabi::store<u32>(list, 0);
  }
  u32 vertices = a + 0xA4;
  if (!vertices)
    vertices = gabi::call<u32>(0x0273AD10, 0x968);
  if (vertices) {
    gabi::call(0x028EFFD0, ptr(vertices), 4, 0x254, 0x0239F410);
    gabi::store<u32>(vertices + 0x950, 0);
    gabi::store<u32>(vertices + 0x960, 0);
    gabi::store<u32>(vertices + 0x954, 0);
    gabi::store<u8>(vertices + 0x964, 0);
    gabi::store<u32>(vertices + 0x958, 48);
    for (u32 i = 0; i < 4; i++)
      gabi::store<u32>(vertices + i * 0x254, 0);
  }
  gabi::call(0x027B5430, ptr(a + 0xA0C));
  gabi::call(0x027FD6F4, ptr(a + 0xA24));
  gabi::call(0x027FB40C, ptr(a + 0xA30));
  gabi::store<u32>(a + 0xA3C, 0x1016EF84);
  gabi::call(0x028F521C, ptr(a + 0xAA4), 0x34);
  if (a + 0xAA4 == 0)
    gabi::call(0x0273AD10, 0x30);
  gabi::call(0x027FB40C, ptr(a + 0xAD8));
  gabi::store<u32>(a + 0xAE4, 0x1016EFB4);
  gabi::call(0x028F521C, ptr(a + 0xB4C), 0x2F0);
  f32 zero = gabi::load<f32>(0x10145180), one = gabi::load<f32>(0x1014517C);
  for (u32 i = 0; i < 11; i++)
    for (u32 j = 0; j < 4; j++)
      gabi::store<f32>(a + 0xB4C + i * 16 + j * 4, j == 3 ? one : zero);
  for (u32 i = 0; i < 3; i++)
    gabi::call(0x028EFFD0, ptr(a + 0xBFC + i * 0x20), 2, 0x10, 0x0239F46C);
  for (u32 i = 0; i < 8; i++)
    if (a + 0xC5C + i * 0x30 == 0)
      gabi::call(0x0273AD10, 0x30);
  for (u32 i = 0; i < 6; i++)
    if (a + 0xDDC + i * 0x10 == 0)
      gabi::call(0x0273AD10, 0x10);
  gabi::store<u8>(a + 0xE3C, 0);
  gabi::call(0x027BE6B8, ptr(a + 0xE40));
  gabi::call(0x027BDF7C, ptr(a + 0xED0));
  gabi::call(0x028EFFD0, ptr(a + 0x20C0), 16, 0x2C, 0x0239F498);
  gabi::call(0x025A5894, ptr(a + 0x24C4), 0, 0);
  gabi::store<u32>(a + 0x24C4, 0x1003072C);
  gabi::call(0x023998E0, ptr(a + 0x24F0));
  for (u32 buffer = 0; buffer < 2; buffer++)
    for (u32 row = 0; row < 8; row++) {
      f32 nz = gabi::load<f32>(0x101FFBD4), px = gabi::load<f32>(0x101FFBA8),
          ny = gabi::load<f32>(0x101FFBD0), nx = gabi::load<f32>(0x101FFBCC),
          pz = gabi::load<f32>(0x101FFBB0), py = gabi::load<f32>(0x101FFBAC);
      for (u32 column = 0; column < 6; column++) {
        u32 p = a + 0x106C + buffer * 0x6C0 + (row * 6 + column) * 12;
        gabi::store<f32>(p, px);
        gabi::store<f32>(p + 0x240, nx);
        gabi::store<f32>(p + 4, py);
        gabi::store<f32>(p + 0x480, nx);
        gabi::store<f32>(p + 0x484, ny);
        gabi::store<f32>(p + 0x488, nz);
        gabi::store<f32>(p + 0x244, ny);
        gabi::store<f32>(p + 0x248, nz);
        gabi::store<f32>(p + 8, pz);
      }
    }
  for (u32 i = 0; i < 48; i++) {
    for (u32 j = 0; j < 3; j++)
      gabi::store<u32>(a + 0x1DEC + i * 12 + j * 4,
                       gabi::load<u32>(0x101FFBA8 + j * 4));
    gabi::store<u8>(a + 0x202C + i, 0);
    gabi::store<u8>(a + 0x205C + i, 0);
    gabi::store<u8>(a + 0x208C + i, 255);
  }
  gabi::store<u32>(a + 0x20BC, 0);
  gabi::store<u32>(a + 0x2380, 0);
  for (u32 i = 0; i < 3; i++)
    gabi::call(0x028E9098, ptr(a + 0x2390 + i * 0x30));
  gabi::store<u8>(a + 0x2420, 1);
  f32 x = gabi::load<f32>(0x101FFBA8), y = gabi::load<f32>(0x101FFBAC),
      range = gabi::load<f32>(0x10030844), z = gabi::load<f32>(0x101FFBB0);
  for (u32 base : {0x2424u, 0x2430u, 0x2440u}) {
    gabi::store<f32>(a + base, x);
    gabi::store<f32>(a + base + 4, y);
    gabi::store<f32>(a + base + 8, z);
  }
  gabi::store<u16>(a + 0x243C, 0);
  for (u32 i = 0; i < 3; i++)
    gabi::store<u16>(a + 0x244C + i * 2,
                     gabi::ftoi(gabi::call<f32>(0x020198D8, range)));
  for (u32 base = 0x2454; base < 0x24A8; base += 12) {
    gabi::store<f32>(a + base, x);
    gabi::store<f32>(a + base + 4, y);
    gabi::store<f32>(a + base + 8, z);
  }
  for (u32 offset : {0x24B0u, 0x24C0u, 0x24C1u, 0x24C2u})
    gabi::store<u8>(a + offset, 0);
  one = gabi::load<f32>(0x10030840);
  zero = gabi::load<f32>(0x10030848);
  gabi::store<f32>(a + 0x24A8, zero);
  gabi::store<f32>(a + 0x24AC, one);
  gabi::store<f32>(a + 0x24B4, one);
  gabi::store<f32>(a + 0x24B8, zero);
  gabi::store<f32>(a + 0x24BC, zero);
  gabi::store<u32>(a + 0x24EC, 0);
  return self;
}
VERIFY(0x0239992C, tapestryPacketConstruct);
static void tapestryUploadVertices(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239DD2C, void, self);
  u32 a = gabi::ea(self), owner = gabi::load<u32>(a + 0x18);
  gabi::store<u32>(a + 0x1068, owner + 0x110);
  for (u32 face = 0; face < 2; face++) {
    u32 index = face * 2 + gabi::load<u32>(a + 0x9F4);
    u32 destination = gabi::load<u32>(a + 0xA4 + index * 0x254);
    for (u32 vertex = 0; vertex < 48; vertex++) {
      u32 source =
          a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0 + vertex * 12;
      f32 z = gabi::load<f32>(source + 8), x = gabi::load<f32>(source),
          y = gabi::load<f32>(source + 4);
      gabi::store<f32>(destination + vertex * 0x30, x);
      gabi::store<f32>(destination + vertex * 0x30 + 4, y);
      gabi::store<f32>(destination + vertex * 0x30 + 8, z);
      source = a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0 + vertex * 12 +
               (face ? 0x480 : 0x240);
      x = gabi::load<f32>(source);
      y = gabi::load<f32>(source + 4);
      z = gabi::load<f32>(source + 8);
      gabi::store<f32>(destination + vertex * 0x30 + 12, x);
      gabi::store<f32>(destination + vertex * 0x30 + 16, y);
      gabi::store<f32>(destination + vertex * 0x30 + 20, z);
    }
  }
  u32 first = a + 0xA8 + gabi::load<u32>(a + 0x9F4) * 0x254;
  for (u32 i = 0; i < 2; i++)
    gabi::call(0x027B5E94, ptr(first + i * 0x4A8), 0,
               gabi::load<u32>(first + i * 0x4A8 + 0x14C));
  gabi::store<u32>(a + 0x9F4, gabi::load<u32>(a + 0x9F4) == 0);
  gabi::call(0x0255F8F4, ptr(gabi::load<u32>(a + 0x1068)));
  gabi::Local<be<f32>[12]> view, world;
  gabi::Local<be<f32>[4]> color, adjusted;
  gabi::call(0x0239DB14, view.get(), ptr(0x104B45F8));
  u32 camera = gabi::load<u32>(0x104B4708);
  gabi::call(0x027FDA54, ptr(a + 0xA24), 0, view.get(), ptr(0x104B470C),
             ptr(camera + 0x240));
  for (u32 i = 0; i < 2; i++) {
    u32 settings = gabi::load<u32>(a + 0x1068),
        material = gabi::load<u32>(a + 0xA28);
    gabi::call(0x0239DBB4, color.get(), ptr(settings + (i ? 0x160 : 0x90)));
    settings = gabi::load<u32>(a + 0x1068);
    gabi::call(0x0274D458, adjusted.get(), color.get(),
               gabi::load<f32>(settings + (i ? 0x16C : 0x28)));
    for (u32 j = 0; j < 4; j++)
      gabi::store<u32>(material + 0x1C4 + i * 0x10 + j * 4,
                       gabi::load<u32>(gabi::ea(adjusted.get()) + j * 4));
  }
  gabi::call(0x027FDFF4, ptr(a + 0xA24), 0);
  u32 settings = gabi::load<u32>(a + 0x1068), diffuse = settings + 0x98,
      ambient = settings + 0x9C;
  gabi::call(0x0239DBB4, ptr(a + 0xB8C), ptr(settings + 0x90));
  gabi::call(0x0239DC78, ptr(a + 0xB9C), ptr(diffuse));
  settings = gabi::load<u32>(a + 0x1068);
  gabi::call(0x0274D2AC, ptr(a + 0xB9C), gabi::load<f32>(settings + 0x24));
  gabi::call(0x0239DC78, ptr(a + 0xBAC), ptr(ambient));
  gabi::store<f32>(a + 0xBF8, gabi::load<f32>(owner + 0x710));
  if (gabi::load<u8>(0x1046C1B8) == 1)
    gabi::call(0x0239DC78, ptr(a + 0xBBC), ptr(diffuse));
  gabi::call(0x027FB678, ptr(a + 0xAD8));
  gabi::call(0x0239DB14, world.get(), ptr(a + 0x2390));
  gabi::call(0x028E90D4, world.get(), ptr(a + 0xAA4));
  gabi::call(0x027FB678, ptr(a + 0xA30));
}
VERIFY(0x0239DD2C, tapestryUploadVertices);
static void tapestryFreeBuffer(u32 vertex) {
  gabi::call(0x027BF7E8, ptr(vertex + 0x158));
  u32 buffer = gabi::load<u32>(vertex + 0x250);
  gabi::store<u32>(vertex, 0);
  if (buffer) {
    (void)gabi::load<u32>(vertex + 0x24C);
    u32 allocator = gabi::call<u32>(
        0x02755FEC, ptr(gabi::load<u32>(0x101F8B4C)), ptr(buffer));
    u32 table = gabi::load<u32>(allocator + 0xC),
        target = gabi::load<u32>(table + 0x3C);
    gabi::call(target, ptr(allocator), ptr(gabi::load<u32>(vertex + 0x250)));
    gabi::store<u32>(vertex + 0x24C, 0);
    gabi::store<u32>(vertex + 0x250, 0);
  }
}
static void tapestryFreeVectorArray(u32 group, u32 countOffset,
                                    u32 pointerOffset) {
  u32 buffer = gabi::load<u32>(group + pointerOffset);
  if (!buffer)
    return;
  s32 index = 0;
  while (index < gabi::load<s32>(group + countOffset)) {
    u32 object = buffer + (u32)index * 0xF4,
        table = gabi::load<u32>(object + 0xF0),
        target = gabi::load<u32>(table + 0xC);
    gabi::call(target, ptr(object), 2);
    index++;
    buffer = gabi::load<u32>(group + pointerOffset);
  }
  u32 allocator = gabi::call<u32>(0x02755FEC, ptr(gabi::load<u32>(0x101F8B4C)),
                                  ptr(buffer));
  u32 table = gabi::load<u32>(allocator + 0xC),
      target = gabi::load<u32>(table + 0x3C);
  gabi::call(target, ptr(allocator),
             ptr(gabi::load<u32>(group + pointerOffset)));
  gabi::store<u32>(group + countOffset, 0);
  gabi::store<u32>(group + pointerOffset, 0);
}
static void tapestryPacketDestruct(daObjTapestryPacket_c *self, u32 flags) {
  WWHD_FUNC(0x0239F668, void, self, flags);
  if (!self)
    return;
  u32 a = gabi::ea(self), vertices = a + 0xA4;
  gabi::store<u32>(a + 0xC, 0x10030998);
  for (u32 i = 0; i < 4; i++)
    tapestryFreeBuffer(vertices + i * 0x254);
  gabi::store<u32>(vertices + 0x960, 0);
  for (s32 i = 0; i < gabi::load<s32>(a + 0xA24); i++) {
    u32 buffer = gabi::load<u32>(a + 0xA28);
    if ((u32)i < gabi::load<u32>(a + 0xA24))
      buffer += (u32)i * 0x23C;
    for (u32 j = 0; j < 2; j++)
      gabi::call(0x027BEBEC, ptr(buffer + 0x10 + j * 0x1C));
  }
  for (u32 base : {0xA40u, 0xAE8u})
    for (u32 i = 0; i < 2; i++)
      gabi::call(0x027BEBEC, ptr(a + base + i * 0x1C));
  gabi::call(0x028F0164, ptr(a + 0x20C0), 16, 0x2C, 0x0239F5F4, 0, 0);
  gabi::call(0x027BE2B0, ptr(a + 0xED0), 2);
  gabi::call(0x027FB528, ptr(a + 0xAD8), 0);
  gabi::call(0x027FB528, ptr(a + 0xA30), 0);
  gabi::call(0x027FD764, ptr(a + 0xA24), 2);
  gabi::call(0x027B54A0, ptr(a + 0xA0C), 2);
  if (vertices) {
    for (u32 i = 0; i < 4; i++)
      tapestryFreeBuffer(vertices + i * 0x254);
    gabi::store<u32>(vertices + 0x960, 0);
    gabi::call(0x028F0164, ptr(vertices), 4, 0x254, 0x0239F608, 0, 0);
  }
  u32 groups = gabi::load<u32>(a + 0xA0);
  if (groups) {
    s32 i = 0;
    while (i < gabi::load<s32>(a + 0x9C)) {
      u32 group = groups + (u32)i * 0x14;
      if (group) {
        gabi::store<u32>(group, 0);
        tapestryFreeVectorArray(group, 4, 8);
        tapestryFreeVectorArray(group, 0xC, 0x10);
      }
      i++;
      groups = gabi::load<u32>(a + 0xA0);
    }
    u32 allocator = gabi::call<u32>(
            0x02755FEC, ptr(gabi::load<u32>(0x101F8B4C)), ptr(groups)),
        table = gabi::load<u32>(allocator + 0xC),
        target = gabi::load<u32>(table + 0x3C);
    gabi::call(target, ptr(allocator), ptr(gabi::load<u32>(a + 0xA0)));
    gabi::store<u32>(a + 0x9C, 0);
    gabi::store<u32>(a + 0xA0, 0);
  }
  gabi::call(0x027F13DC, self, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0239F668, tapestryPacketDestruct);
static void tapestryCopyVector(cXyz *destination, cXyz *source) {
  u32 from = gabi::ea(source), to = gabi::ea(destination);
  u32 x = gabi::load<u32>(from), y = gabi::load<u32>(from + 4),
      z = gabi::load<u32>(from + 8);
  gabi::store<u32>(to, x);
  gabi::store<u32>(to + 4, y);
  gabi::store<u32>(to + 8, z);
}
static void tapestryInteriorTangent(u32 before, u32 center, u32 after,
                                    cXyz *out, const f32 *weights) {
  gabi::Local<cXyz> temporary, firstDelta, secondDelta, left, right;
  gabi::call(0x0201ADE0, ptr(center), temporary.get(), ptr(before));
  tapestryCopyVector(firstDelta.get(), temporary.get());
  gabi::call(0x0201ADE0, ptr(after), temporary.get(), ptr(center));
  tapestryCopyVector(secondDelta.get(), temporary.get());
  gabi::call(0x0201AE48, ptr(before), temporary.get(), weights[0]);
  tapestryCopyVector(left.get(), temporary.get());
  gabi::call(0x0201AE48, firstDelta.get(), temporary.get(), weights[1]);
  gabi::call(0x028E8D88, left.get(), temporary.get(), left.get());
  gabi::call(0x0201AE48, secondDelta.get(), temporary.get(), weights[2]);
  gabi::call(0x028E8D88, left.get(), temporary.get(), left.get());
  gabi::call(0x0201AE48, ptr(after), temporary.get(), weights[3]);
  gabi::call(0x028E8D88, left.get(), temporary.get(), left.get());
  gabi::call(0x0201AE48, ptr(before), temporary.get(), weights[3]);
  tapestryCopyVector(right.get(), temporary.get());
  gabi::call(0x0201AE48, firstDelta.get(), temporary.get(), weights[4]);
  gabi::call(0x028E8D88, right.get(), temporary.get(), right.get());
  gabi::call(0x0201AE48, secondDelta.get(), temporary.get(), weights[5]);
  gabi::call(0x028E8D88, right.get(), temporary.get(), right.get());
  gabi::call(0x0201AE48, ptr(after), temporary.get(), weights[0]);
  gabi::call(0x028E8D88, right.get(), temporary.get(), right.get());
  gabi::call(0x0201ADE0, right.get(), temporary.get(), left.get());
  tapestryCopyVector(out, temporary.get());
}
static void tapestryNormals(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239B85C, void, self);
  u32 a = gabi::ea(self), buffer = gabi::load<u32>(a + 0x20BC);
  f32 weights[6] = {gabi::load<f32>(0x100308C0), gabi::load<f32>(0x100308B0),
                    gabi::load<f32>(0x100308B4), gabi::load<f32>(0x100308C4),
                    gabi::load<f32>(0x100308B8), gabi::load<f32>(0x100308BC)};
  f32 reverse = gabi::load<f32>(0x100308AC);
  u32 current = a + 0x106C + buffer * 0x6C0,
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  for (u32 row = 0; row < 8; row++)
    for (u32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column, center = previous + index * 12;
      gabi::Local<cXyz> vertical, horizontal, temporary, cross, normal;
      if (row == 0) {
        gabi::call(0x0201ADE0, ptr(center + 0x48), temporary.get(),
                   ptr(center));
        tapestryCopyVector(vertical.get(), temporary.get());
      } else if (row == 7) {
        gabi::call(0x0201ADE0, ptr(center), temporary.get(),
                   ptr(center - 0x48));
        tapestryCopyVector(vertical.get(), temporary.get());
      } else
        tapestryInteriorTangent(center - 0x48, center, center + 0x48,
                                vertical.get(), weights);
      if (column == 0) {
        gabi::call(0x0201ADE0, ptr(center + 12), temporary.get(), ptr(center));
        tapestryCopyVector(horizontal.get(), temporary.get());
      } else if (column == 5) {
        gabi::call(0x0201ADE0, ptr(center), temporary.get(), ptr(center - 12));
        tapestryCopyVector(horizontal.get(), temporary.get());
      } else
        tapestryInteriorTangent(center - 12, center, center + 12,
                                horizontal.get(), weights);
      gabi::call(0x0201B080, horizontal.get(), cross.get(), vertical.get());
      tapestryCopyVector(normal.get(), cross.get());
      if (gabi::call<s32>(0x0201B47C, normal.get())) {
        for (u32 i = 0; i < 3; i++)
          gabi::store<u32>(current + index * 12 + 0x240 + i * 4,
                           gabi::load<u32>(gabi::ea(normal.get()) + i * 4));
        for (u32 i = 0; i < 3; i++)
          gabi::store<u32>(current + index * 12 + 0x480 + i * 4,
                           gabi::load<u32>(gabi::ea(normal.get()) + i * 4));
        gabi::call(0x028E8E64, ptr(current + index * 12 + 0x480),
                   ptr(current + index * 12 + 0x480), reverse);
      }
    }
}
VERIFY(0x0239B85C, tapestryNormals);
static BOOL tapestryCheckCollision(daObjTapestry_c *self) {
  WWHD_FUNC(0x0239EA0C, BOOL, self);
  u32 a = gabi::ea(self);
  f32 zero = gabi::load<f32>(0x10030848), force = gabi::load<f32>(0x100308AC),
      maxSquare = gabi::load<f32>(0x10030968);
  f32 small = gabi::load<f32>(0x10030858), one = gabi::load<f32>(0x10030840),
      two = gabi::load<f32>(0x10030880), half = gabi::load<f32>(0x10030860),
      large = gabi::load<f32>(0x10030964);
  f32 radius = zero;
  BOOL fire = 0;
  u32 hitPosition = 0;
  gabi::Local<cXyz> direction, wind;
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(gabi::ea(direction.get()) + i * 4,
                     gabi::load<f32>(0x101FFBA8 + i * 4));
  for (u32 i = 0; i < 2; i++) {
    u32 triangle = a + 0x3F0 + i * 0x150;
    if (!gabi::call<s32>(0x025162A4, ptr(triangle)))
      continue;
    u32 object = gabi::call<u32>(0x02516300, ptr(triangle));
    if (object) {
      u32 attack = gabi::load<u32>(object + 0x10);
      for (u32 j = 0; j < 3; j++)
        gabi::store<f32>(gabi::ea(direction.get()) + j * 4,
                         gabi::load<f32>(triangle + 0xC0 + j * 4));
      hitPosition = triangle + 0xCC;
      if (!gabi::call<s32>(0x0201B47C, direction.get())) {
        u16 angle = gabi::load<u16>(a + 0x32A);
        u32 table = 0x104A44F8 + (angle >> 3) * 8;
        f32 sine = gabi::load<f32>(table), cosine = gabi::load<f32>(table + 4);
        direction->x = sine;
        direction->y = zero;
        direction->z = -cosine;
      }
      if (attack == 0x200 || attack == 0x20000 || attack == 0x40000)
        fire = 1;
      switch (attack) {
      case 2:
      case 8:
      case 0x40:
      case 0x80:
      case 0x200:
      case 0x400:
      case 0x800:
      case 0x1000:
      case 0x2000:
      case 0x10000:
      case 0x1000000:
      case 0x4000000:
      case 0x10000000:
        force = one;
        radius = half;
        break;
      case 0x4000:
      case 0x8000:
      case 0x40000:
      case 0x80000:
      case 0x100000:
        force = two;
        radius = small;
        break;
      case 0x20000:
        force = zero;
        radius = small;
        break;
      case 0x20:
        radius = large;
        force = two;
        break;
      case 0x200000: {
        for (u32 j = 0; j < 3; j++)
          gabi::store<f32>(gabi::ea(wind.get()) + j * 4,
                           gabi::load<f32>(triangle + 0xC0 + j * 4));
        f32 square = gabi::call<f32>(0x028E8DD0, wind.get());
        if (square > maxSquare) {
          f32 length = gabi::call<f32>(0x028F4384, square);
          gabi::call(0x028E8E64, wind.get(), wind.get(),
                     (f32)(gabi::load<f32>(0x1003096C) / length));
        }
        u32 packet = gabi::load<u32>(a + 0x3B4);
        for (u32 j = 0; j < 3; j++)
          gabi::store<f32>(packet + 0x2424 + j * 4,
                           gabi::load<f32>(gabi::ea(wind.get()) + j * 4));
        break;
      }
      default:
        break;
      }
    }
    gabi::call(0x0251621C, ptr(triangle));
  }
  if (!(force < zero) && hitPosition) {
    gabi::Local<cXyz> position, copiedDirection;
    for (u32 j = 0; j < 3; j++)
      gabi::store<f32>(gabi::ea(position.get()) + j * 4,
                       gabi::load<f32>(hitPosition + j * 4));
    tapestryCopyVector(copiedDirection.get(), direction.get());
    gabi::call(0x0239E3B4, ptr(gabi::load<u32>(a + 0x3B4)), position.get(),
               copiedDirection.get(), fire, force, radius);
  }
  return fire;
}
VERIFY(0x0239EA0C, tapestryCheckCollision);
static void tapestryApplyMaterial(u32 packet, u32 material) {
  u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xAE4) + 0x2C);
  gabi::call(target, ptr(packet + 0xAD8), ptr(material));
  target = gabi::load<u32>(gabi::load<u32>(packet + 0xA3C) + 0x2C);
  gabi::call(target, ptr(packet + 0xA30), ptr(material));
}
static bool tapestryMaterialBindings(u32 material, u32 table) {
  u32 flags = gabi::load<u32>(material + 0xC),
      binding = flags ? gabi::load<u32>(material + 0x10) : 0;
  s16 texture = gabi::load<s16>(binding + 0xC);
  u32 descriptor = gabi::load<u32>(table + 4),
      data = gabi::load<u32>(table + 0xC);
  s16 color = gabi::load<s16>(binding + 0xE),
      position = gabi::load<s16>(binding + 0x10);
  if (color != -1)
    gabi::call(0xC0006900, color, ptr(data), ptr(descriptor));
  if (texture != -1)
    gabi::call(0xC0006A38, texture, ptr(data), ptr(descriptor));
  if (position != -1)
    gabi::call(0xC00068A8, position, ptr(data), ptr(descriptor));
  return position != -1;
}
static void tapestryBindTexture(u32 packet, u32 material) {
  u32 texture =
      gabi::load<u32>(material + 0x14) ? gabi::load<u32>(material + 0x18) : 0;
  gabi::call(0x027BE53C, ptr(packet + 0xED0), ptr(texture + 4), -1, 0);
}
static void tapestryRenderSurface(daObjTapestryPacket_c *self, u8 *surface) {
  WWHD_FUNC(0x0239E474, void, self, surface);
  u32 a = gabi::ea(self), s = gabi::ea(surface),
      index = gabi::load<u32>(s + 0xC), material = 0;
  if ((s32)index < 4) {
    u32 groups = gabi::load<u32>(a + 0xA0);
    if (index < gabi::load<u32>(a + 0x9C))
      groups += index * 0x14;
    material = gabi::load<u32>(groups);
  }
  u32 state = gabi::call<u32>(0x027F29D4, ptr(0x104B45C0)),
      shape = gabi::load<u32>(material), active = gabi::load<u32>(state + 4);
  if (shape != active) {
    u8 flags = gabi::load<u8>(shape);
    u32 current = gabi::load<u32>(state);
    if (flags & 2) {
      gabi::store<u8>(shape, flags & ~2);
      gabi::call(0x027BB9E0, ptr(shape), 0);
    }
    u32 table = gabi::load<u32>(shape + 0x7C),
        next = gabi::load<u32>(table + 0x28);
    if (current != next)
      gabi::call(0x027B9F68, ptr(next));
    u32 display = gabi::load<u32>(shape + 0xC);
    if (display)
      gabi::call(0xC00060E0, ptr(gabi::load<u32>(shape + 4)), display);
    else
      gabi::call(0x027BB7CC, ptr(shape));
    gabi::store<u32>(state, next);
    gabi::store<u32>(state + 4, shape);
  }
  index = gabi::load<u32>(s + 0xC);
  if (index == 0) {
    tapestryApplyMaterial(a, material);
    u32 source = gabi::load<u32>(s + 0x14);
    if (source) {
      u32 data = gabi::load<u32>(source + 4),
          table = data + 0x10 + gabi::load<u32>(data + 0x4C) * 0x1C;
      tapestryMaterialBindings(material, table);
    }
  } else if (index == 1 || index == 2) {
    u32 data = gabi::load<u32>(a + 0xA28),
        table = data + 0x10 + gabi::load<u32>(data + 0x4C) * 0x1C;
    tapestryMaterialBindings(material, table);
    tapestryApplyMaterial(a, material);
    if (index == 2) {
      u32 extra = gabi::load<u32>(s + 0x30);
      if (extra)
        gabi::call(gabi::load<u32>(gabi::load<u32>(extra + 0xC) + 0x2C),
                   ptr(extra), ptr(material));
    }
    tapestryBindTexture(a, material);
    if (index == 2)
      gabi::call(0x027FFE54, surface, ptr(material));
  }
  gabi::Local<u8[0x11C]> drawState; /* HD sizeof (ctor 02750250 allocates 0x11C when this == NULL, vtable at +0x118); was 240 */
  gabi::call(0x02750250, drawState.get());
  u32 local = gabi::ea(drawState.get());
  gabi::store<u32>(local + 8, 0);
  index = gabi::load<u32>(s + 0xC);
  u32 flags = gabi::load<u32>(local + 0xEC);
  gabi::store<u32>(local + 0xC, 2);
  gabi::store<f32>(local + 0xE8, gabi::load<f32>(0x10030860));
  gabi::store<u8>(local + 0xE0, 1);
  gabi::store<u32>(local + 0xE4, 4);
  gabi::store<u32>(local + 0xEC,
                   (((flags & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10);
  gabi::call(0x0280037C, index, drawState.get());
  gabi::call(0x02750370, drawState.get());
  auto group = [&]() {
    u32 id = gabi::load<u32>(s + 0xC), count = gabi::load<u32>(a + 0x9C),
        groups = gabi::load<u32>(a + 0xA0);
    if (id < count)
      groups += id * 0x14;
    return groups + (gabi::load<u32>(a + 0x9F4) == 0 ? 8 : 0);
  };
  u32 selected = group();
  gabi::call(0x027BFE5C, ptr(gabi::load<u32>(selected + 8)));
  auto strips = [&]() {
    for (u32 row = 0; row < 7; row++) {
      u32 size = gabi::load<u32>(a + 0xA0C),
          stride = gabi::load<u32>(a + 0xA1C),
          indices = gabi::load<u32>(a + 0xA14),
          buffer = gabi::load<u32>(a + 0xA10);
      gabi::call(0xC0006178, ptr(buffer), 12, size,
                 ptr(indices + stride * row * 12), 0, 1);
    }
  };
  strips();
  gabi::store<u32>(local + 8, 1);
  gabi::call(0x02750370, drawState.get());
  selected = group();
  u32 count = gabi::load<u32>(selected + 4),
      buffer = gabi::load<u32>(selected + 8);
  if (count > 1)
    buffer += 0xF4;
  gabi::call(0x027BFE5C, ptr(buffer));
  strips();
  gabi::call(0x02750370, ptr(0x104B474C));
}
VERIFY(0x0239E474, tapestryRenderSurface);
static u32 tapestryAllocate(u32 size, u32 alignment) {
  u32 allocator = gabi::call<u32>(0x02756140, ptr(gabi::load<u32>(0x101F8B4C)));
  u32 target = gabi::load<u32>(gabi::load<u32>(allocator + 0xC) + 0x34);
  return gabi::call<u32>(target, ptr(allocator), size, alignment);
}
static void tapestryRenderInit(daObjTapestryPacket_c *self) {
  WWHD_FUNC(0x0239C678, void, self);
  u32 a = gabi::ea(self);
  gabi::Local<be<u32>[2]> materialName;
  (*materialName)[0] = 0x100308F4;
  (*materialName)[1] = 0x10030654;
  u32 model = gabi::call<u32>(0x027FFCBC, self, ptr(0x100308F4));
  s32 found = gabi::call<s32>(0x027B90AC, ptr(gabi::load<u32>(model + 4)),
                              materialName.get());
  u32 material = 0;
  if (found >= 0) {
    u32 count = gabi::load<u32>(model + 8), base = gabi::load<u32>(model + 0xC);
    u32 selected = base + ((u32)found < count ? (u32)found * 0x24 : 0);
    if (!gabi::load<u8>(selected + 0x20)) {
      u32 data = gabi::load<u32>(model + 4),
          limit = gabi::load<u32>(data + 0x1C), definition = 0;
      if ((u32)found < limit)
        definition = gabi::load<u32>(data + 0x20) + (u32)found * 0x84;
      gabi::call(0x02800B0C, ptr(selected), ptr(definition), 0);
      count = gabi::load<u32>(model + 8);
      base = gabi::load<u32>(model + 0xC);
    }
    material = base + ((u32)found < count ? (u32)found * 0x24 : 0);
  }
  gabi::call(0x0280068C, ptr(a + 0x98), ptr(material), 0);
  gabi::store<u32>(a + 0x9F8, 0x1013);
  gabi::store<u32>(a + 0xA00, 0x10030988);
  for (u32 buffer = 0; buffer < 2; buffer++)
    for (u32 face = 0; face < 2; face++) {
      u32 vertex = a + 0xA4 + buffer * 0x254 + face * 0x4A8,
          data = gabi::load<u32>(vertex);
      if (!data) {
        u32 allocated = tapestryAllocate(0x900, 64);
        if (allocated) {
          gabi::store<u32>(vertex + 0x250, allocated);
          gabi::store<u32>(vertex + 0x24C, 48);
        }
        data = gabi::load<u32>(vertex + 0x250);
        gabi::store<u32>(vertex, data);
      }
      gabi::call(0x027FF478, ptr(vertex + 4), ptr(data), 48, ptr(a + 0x9F8));
    }
  gabi::store<u32>(a + 0xA04, 0);
  gabi::store<u8>(a + 0xA08, 1);
  for (u32 groupIndex = 0; groupIndex < gabi::load<u32>(a + 0x98);
       groupIndex++) {
    u32 groups = gabi::load<u32>(a + 0xA0), count = gabi::load<u32>(a + 0x9C);
    u32 group = groups + (groupIndex < count ? groupIndex * 0x14 : 0),
        definition = gabi::load<u32>(group);
    gabi::store<u32>(group, 0);
    tapestryFreeVectorArray(group, 4, 8);
    tapestryFreeVectorArray(group, 0xC, 0x10);
    gabi::store<u32>(group, definition);
    for (u32 side = 0; side < 2; side++) {
      u32 allocated = tapestryAllocate(0x1E8, 4);
      for (u32 i = 0; i < 2; i++)
        if (allocated + i * 0xF4)
          gabi::call(0x027BF734, ptr(allocated + i * 0xF4));
      if (allocated) {
        gabi::store<u32>(group + 4 + side * 8, 2);
        gabi::store<u32>(group + 8 + side * 8, allocated);
      }
    }
    for (u32 side = 0; side < 2; side++)
      for (u32 face = 0; face < 2; face++) {
        u32 arrayCount = gabi::load<u32>(group + 4 + side * 8),
            data = gabi::load<u32>(group + 8 + side * 8);
        if (face < arrayCount)
          data += face * 0xF4;
        gabi::call(0x027FF530, ptr(definition), ptr(data),
                   ptr(a + 0xA8 + side * 0x254 + face * 0x4A8), ptr(a + 0x9F8),
                   0);
      }
  }
  gabi::call(0x027FE084, ptr(a + 0xA24), 1, 0);
  gabi::call(0x027B54E0, ptr(a + 0xA0C), ptr(0x1046C22C), 4, 0x54);
  f32 divisor = gabi::load<f32>(0x100308D8);
  gabi::store<u32>(a + 0xA10, 6);
  for (u32 face = 0; face < 2; face++) {
    u32 index = face * 2 + gabi::load<u32>(a + 0x9F4),
        data = gabi::load<u32>(a + 0xA4 + index * 0x254), end = data + 0x900;
    for (u32 block = data; block < end; block += 0x20)
      for (u32 word = 0; word < 8; word++)
        gabi::store<u32>((block & ~31u) + word * 4, 0);
    index = face * 2 + gabi::load<u32>(a + 0x9F4);
    data = gabi::load<u32>(a + 0xA4 + index * 0x254);
    for (u32 vertex = 0; vertex < 48; vertex++) {
      u32 source =
          a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0 + vertex * 12;
      f32 position[3];
      for (u32 component = 0; component < 3; component++)
        position[component] = gabi::load<f32>(source + component * 4);
      for (u32 component = 0; component < 3; component++)
        gabi::store<f32>(data + vertex * 0x30 + component * 4,
                         position[component]);
      source = a + 0x106C + gabi::load<u32>(a + 0x20BC) * 0x6C0 + vertex * 12 +
               (face ? 0x480 : 0x240);
      f32 normal[3];
      normal[0] = gabi::load<f32>(source);
      normal[face ? 2 : 1] = gabi::load<f32>(source + (face ? 8 : 4));
      normal[face ? 1 : 2] = gabi::load<f32>(source + (face ? 4 : 8));
      gabi::store<f32>(data + vertex * 0x30 + 16, normal[1]);
      gabi::store<f32>(data + vertex * 0x30 + 12, normal[0]);
      gabi::store<f32>(data + vertex * 0x30 + 20, normal[2]);
      for (u32 channel = 0; channel < 4; channel++)
        gabi::store<f32>(data + vertex * 0x30 + 24 + channel * 4,
                         (f32)gabi::load<u8>(0x101CD194 + channel) / divisor);
      f32 u = gabi::load<f32>(0x1046C2D4 + vertex * 8),
          v = gabi::load<f32>(0x1046C2D8 + vertex * 8);
      gabi::store<f32>(data + vertex * 0x30 + 40, u);
      gabi::store<f32>(data + vertex * 0x30 + 44, v);
    }
  }
  u32 index = gabi::load<u32>(a + 0x9F4), destinationIndex = index == 0;
  for (u32 face = 0; face < 2; face++) {
    u32 destination =
        gabi::load<u32>(a + 0xA4 + (destinationIndex + face * 2) * 0x254);
    u32 source = gabi::load<u32>(a + 0xA4 + (index + face * 2) * 0x254);
    for (u32 vertex = 0; vertex < 48; vertex++) {
      source = gabi::load<u32>(a + 0xA4 + (index + face * 2) * 0x254);
      destination =
          gabi::load<u32>(a + 0xA4 + (destinationIndex + face * 2) * 0x254);
      for (u32 word = 0; word < 12; word++)
        gabi::store<f32>(destination + vertex * 0x30 + word * 4,
                         gabi::load<f32>(source + vertex * 0x30 + word * 4));
    }
    index = gabi::load<u32>(a + 0x9F4);
  }
  u32 first = a + 0xA8 + index * 0x254;
  for (u32 face = 0; face < 2; face++)
    gabi::call(0x027B5E94, ptr(first + face * 0x4A8), 0,
               gabi::load<u32>(first + face * 0x4A8 + 0x14C));
  gabi::store<u32>(a + 0x9F4, gabi::load<u32>(a + 0x9F4) == 0);
  gabi::call(0x0274FBF8, ptr(gabi::load<u32>(0x101F8B18)));
  gabi::Local<be<u32>[2]> archive, resource;
  (*archive)[0] = 0x10030900;
  (*archive)[1] = 0x10030654;
  (*resource)[0] = 0x10030910;
  (*resource)[1] = 0x10030654;
  u8 *texture = gabi::call<u8 *>(0x026124B0, ptr(gabi::load<u32>(0x101F4F7C)),
                                 archive.get(), resource.get(), 0);
  gabi::call(0x02773870, ptr(a + 0xE40), texture, ptr(0x100308E8));
  gabi::call(0x0274FCCC, ptr(gabi::load<u32>(0x101F8B18)));
  bool same = true;
  for (u32 offset : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
    if (gabi::load<u32>(a + 0xED0 + offset) !=
        gabi::load<u32>(a + 0xE40 + offset)) {
      same = false;
      break;
    }
  if (!same)
    gabi::call(0x027BDEB4, ptr(a + 0xED0), ptr(a + 0xE40));
  u8 flags = gabi::load<u8>(a + 0x1060);
  gabi::store<u32>(a + 0x102C, 0);
  gabi::store<u32>(a + 0x1030, 0);
  gabi::store<u32>(a + 0x1034, 0);
  if (same) {
    u32 first = gabi::load<u32>(a + 0xE68), second = gabi::load<u32>(a + 0xE70);
    gabi::store<u32>(a + 0xFA4, first);
    gabi::store<u32>(a + 0xFAC, second);
    gabi::store<u32>(a + 0xEF8, first);
    gabi::store<u32>(a + 0xF00, second);
  }
  gabi::store<u8>(a + 0x1060, flags | 2);
}
VERIFY(0x0239C678, tapestryRenderInit);
