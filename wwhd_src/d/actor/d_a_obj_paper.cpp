/* Readable notice-board/message actor. GC reconstruction corrected against HD.
 */
#include "d/actor/d_a_obj_paper.h"
using daObjPaper::Act_c;
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u32 attribute(s32 type) { return 0x1002E458 + (u32)type * 0x1C; }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 paperParameter(Act_c *self, s32 width, s32 shift) {
  WWHD_FUNC(0x02381ED8, u32, self, width, shift);
  u32 value = gabi::load<u32>(gabi::ea(self) + 0xB0);
  u32 left = ((u32)width & 32) ? 0 : 1u << ((u32)width & 31);
  u32 right = ((u32)shift & 32) ? 0 : value >> ((u32)shift & 31);
  return right & (left - 1);
}
VERIFY(0x02381ED8, paperParameter);
bool Act_c::create_heap() {
  WWHD_FUNC(0x023814A0, bool, this);
  u32 a = attribute(mType);
  u32 control = gabi::load<u32>(0x101F4F28);
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = gabi::load<u32>(a);
  (*name)[1] = 0x1002E420;
  s32 index = gabi::load<s16>(a + 8);
  u8 *data = gabi::call<u8 *>(0x026066C4, ptr(control), name.get(), index);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002E4AC), 0x141, ptr(0x1002E4C0));
  mpModel = gabi::call<u8 *>(0x025E38E0, data, 0x80000, 0x11000022);
  return mpModel != nullptr;
}
VERIFY(0x023814A0, &Act_c::create_heap);
BOOL paperHeap(Act_c *self) {
  WWHD_FUNC(0x02381560, BOOL, self);
  return self->create_heap();
}
VERIFY(0x02381560, paperHeap);
void Act_c::set_mtx() {
  WWHD_FUNC(0x02381564, void, this);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  u32 a = gabi::ea(this);
  gabi::call(0x025F1B48, ptr(0x1048D0CC), gabi::load<s16>(a + 0x328),
             gabi::load<s16>(a + 0x32A), gabi::load<s16>(a + 0x32C));
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  u32 model = gabi::ea((u8 *)mpModel);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x02381564, &Act_c::set_mtx);
void Act_c::init_mtx() {
  WWHD_FUNC(0x02381628, void, this);
  u32 model = gabi::ea((u8 *)mpModel);
  f32 y = scale.y, x = scale.x, z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  set_mtx();
}
VERIFY(0x02381628, &Act_c::init_mtx);
void Act_c::mode_wait_init() {
  WWHD_FUNC(0x02381648, void, this);
  u32 a = gabi::ea(this), status = gabi::load<u32>(a + 0x2E0);
  mMode = 0;
  gabi::store<u32>(a + 0x2E0, status | 0x80);
}
VERIFY(0x02381648, &Act_c::mode_wait_init);
BOOL Act_c::remove() {
  WWHD_FUNC(0x02381A18, BOOL, this);
  u32 a = attribute(mType);
  u32 name = gabi::load<u32>(a);
  gabi::call(0x025204C8, mPhase, ptr(name));
  return 1;
}
VERIFY(0x02381A18, &Act_c::remove);
void Act_c::damage_cc_proc() {
  WWHD_FUNC(0x02381A54, void, this);
  if (gabi::call<s32>(0x025162A4, mCylinder)) {
    gabi::call(0x023129C4, &eyePos, (s8)current.roomNo, mCylinder, 13);
    gabi::Local<cXyz> position;
    position->x = (f32)current.pos.x;
    position->y = (f32)current.pos.y;
    position->z = (f32)current.pos.z;
    u32 id = gabi::load<u32>(gabi::ea(this) + 4);
    gabi::call(0x0255F458, position.get(), 4, id, 100);
    gabi::call(0x02312E54, this, mCylinder);
    gabi::call(0x0251621C, mCylinder);
  }
  gabi::call(0x02515E50, ptr(gabi::ea(this) + 0x504));
}
VERIFY(0x02381A54, &Act_c::damage_cc_proc);
void Act_c::mode_talk0_init() {
  WWHD_FUNC(0x02381C3C, void, this);
  u32 a = gabi::ea(this), status = gabi::load<u32>(a + 0x2E0);
  mMessageId = -1;
  mMode = 1;
  gabi::store<u32>(a + 0x2E0, status & ~0x80u);
}
VERIFY(0x02381C3C, &Act_c::mode_talk0_init);
void Act_c::mode_wait() {
  WWHD_FUNC(0x02381C5C, void, this);
  u32 a = gabi::ea(this);
  if (gabi::load<u16>(a + 0xF8) == 1)
    mode_talk0_init();
  else
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 1);
}
VERIFY(0x02381C5C, &Act_c::mode_wait);
void Act_c::mode_talk1_init() {
  WWHD_FUNC(0x02381C7C, void, this);
  mMode = 2;
}
VERIFY(0x02381C7C, &Act_c::mode_talk1_init);
void Act_c::mode_talk0() {
  WWHD_FUNC(0x02381C88, void, this);
  if (mMessageId != -1)
    return;
  u32 p = play();
  s32 camera = gabi::load<s8>(p + 0x5B30);
  p = play();
  if (!(gabi::load<u32>(p + (u32)camera * 0x34 + 0x5B00) & 4))
    return;
  u32 messages = gabi::load<u32>(0x101F4B5C);
  u32 message = paperParameter(this, 16, 0);
  s32 id = gabi::call<s32>(0x025F7DB0, ptr(messages), message, &eyePos);
  mMessageId = id;
  if (id != -1)
    mode_talk1_init();
}
VERIFY(0x02381C88, &Act_c::mode_talk0);
void Act_c::mode_talk2_init() {
  WWHD_FUNC(0x02381D24, void, this);
  mMode = 3;
}
VERIFY(0x02381D24, &Act_c::mode_talk2_init);
void Act_c::mode_talk1() {
  WWHD_FUNC(0x02381D30, void, this);
  mode_talk2_init();
}
VERIFY(0x02381D30, &Act_c::mode_talk1);
void Act_c::mode_talk2() {
  WWHD_FUNC(0x02381D34, void, this);
  u32 messages = gabi::load<u32>(0x101F4B5C);
  if (gabi::call<s32>(0x025F795C, ptr(messages)) == 0x12) {
    gabi::call(0x025F74D0, ptr(messages), 0x13);
    mMessageId = -1;
    u32 p = play();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
    mode_wait_init();
  }
}
VERIFY(0x02381D34, &Act_c::mode_talk2);
s32 Act_c::create() {
  WWHD_FUNC(0x02381660, s32, this);
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(a + 0xB4, 0x1002E448);
      gabi::call(0x02515FB8, mCylinder);
      gabi::store<u32>(a + 0x4CC, 0x100015A8);
      gabi::store<u32>(a + 0x4C8, 0x1002E438);
      gabi::call(0x02018590, ptr(a + 0x4D0));
      gabi::store<u32>(a + 0x3F4, 0x1004B108);
      gabi::store<u32>(a + 0x4E4, 0x1004B150);
      gabi::store<u32>(a + 0x4CC, 0x1004B160);
      gabi::call(0x0200BD2C, &mColStatus);
      gabi::call(0x02515DA0, ptr(a + 0x504));
      flags = gabi::load<u32>(a + 0x2E4);
      gabi::store<u32>(a + 0x500, 0x1004AE88);
      gabi::store<u32>(a + 0x504, 0x1004AEC0);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 type = paperParameter(this, 4, 16);
  mType = type;
  u32 at = attribute(type), name = gabi::load<u32>(at);
  s32 phase = gabi::call<s32>(0x02520460, mPhase, ptr(name));
  if (phase != 4)
    return phase;
  at = attribute(mType);
  u32 heap = gabi::load<u32>(at + 4);
  if (!gabi::call<s32>(0x025D63E8, this, ptr(0x02381560), heap))
    return 5;
  type = mType;
  at = attribute(type);
  f32 eyeOffset = (f32)gabi::load<s16>(at + 0xA);
  gabi::store<f32>(a + 0x380, gabi::load<f32>(a + 0x380) + eyeOffset);
  f32 attention = (f32)gabi::load<s16>(at + 0xC);
  gabi::store<f32>(a + 0x394, gabi::load<f32>(a + 0x394) + attention);
  u8 talk = gabi::load<u8>(at + 0x12);
  u32 attentionFlags = gabi::load<u32>(a + 0x39C);
  gabi::store<u8>(a + 0x389, talk);
  u8 speak = gabi::load<u8>(at + 0x13);
  mMessageId = -1;
  gabi::store<u8>(a + 0x38B, speak);
  gabi::store<u32>(a + 0x39C, attentionFlags | 0x4000000A);
  if (type == 2) {
    u32 status = gabi::load<u32>(a + 0x2E0);
    type = mType;
    gabi::store<u32>(a + 0x2E0, (status & ~0x3Fu) | 0x38);
  }
  at = attribute(type);
  if (gabi::load<s16>(at + 0x16) != 0) {
    mHasCollision = 1;
    gabi::call(0x02515F14, &mColStatus, 255, 255, this);
    gabi::call(0x02516518, mCylinder, ptr(0x1002E508));
    at = attribute(mType);
    gabi::store<u32>(a + 0x3FC, gabi::ea(&mColStatus));
    f32 radius = (f32)gabi::load<s16>(at + 0x16);
    gabi::call(0x020184DC, ptr(a + 0x4D0), radius);
    at = attribute(mType);
    f32 height = (f32)gabi::load<s16>(at + 0x18);
    gabi::call(0x02018428, ptr(a + 0x4D0), height);
  } else
    mHasCollision = 0;
  at = attribute(mType);
  f32 y = (f32)gabi::load<s16>(at + 0x10),
      radius = (f32)gabi::load<s16>(at + 0xE);
  gabi::call(0x025D6768, this, 0.0f, y, 0.0f, radius);
  u32 model = gabi::ea((u8 *)mpModel);
  gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  mode_wait_init();
  return phase;
}
VERIFY(0x02381660, &Act_c::create);
static void paperMode(u32 entry, Act_c *self) {
  s16 delta = gabi::load<s16>(entry), index = gabi::load<s16>(entry + 2);
  u32 adjusted = gabi::ea(self) + delta, target;
  if (index < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    u32 table = gabi::load<u32>(adjusted + gabi::load<s16>(entry + 6));
    target = gabi::load<u32>(table + (u32)index * 8 + 4);
  }
  gabi::call_ptr(target, ptr(adjusted));
}
BOOL Act_c::execute() {
  WWHD_FUNC(0x02381AEC, BOOL, this);
  if (mHasCollision)
    damage_cc_proc();
  paperMode(0x1002E4DC + (u32)(s32)mMode * 8, this);
  set_mtx();
  if (mHasCollision) {
    gabi::call(0x020182E0, ptr(gabi::ea(mCylinder) + 0x118), &current.pos);
    u32 p = play();
    gabi::call(0x0200E240, ptr(p + 0x26A4), mCylinder);
  }
  return 1;
}
VERIFY(0x02381AEC, &Act_c::execute);
BOOL Act_c::draw() {
  WWHD_FUNC(0x02381BC4, BOOL, this);
  u32 at = attribute(mType);
  u32 tev = gabi::load<u8>(at + 0x14) ^ 1u;
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), tev, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpModel, ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)mpModel, 0);
  return 1;
}
VERIFY(0x02381BC4, &Act_c::draw);
s32 paperCreate(Act_c *self) {
  WWHD_FUNC(0x02381DA8, s32, self);
  return self->create();
}
VERIFY(0x02381DA8, paperCreate);
BOOL paperDelete(Act_c *self) {
  WWHD_FUNC(0x02381DAC, BOOL, self);
  return self->remove();
}
VERIFY(0x02381DAC, paperDelete);
BOOL paperExecute(Act_c *self) {
  WWHD_FUNC(0x02381DB0, BOOL, self);
  return self->execute();
}
VERIFY(0x02381DB0, paperExecute);
BOOL paperDraw(Act_c *self) {
  WWHD_FUNC(0x02381DB4, BOOL, self);
  return self->draw();
}
VERIFY(0x02381DB4, paperDraw);
static void paperStaticInit() {
  WWHD_FUNC(0x02381DB8, void);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x1046BBAC + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CC24C));
  gabi::store<f32>(0x1046BBA0, gabi::load<f32>(0x1002E4FC));
  gabi::store<f32>(0x1046BBA4, gabi::load<f32>(0x1002E500));
  gabi::call(0x028ED6F8, ptr(0x1046BBA8));
  gabi::call(0x028F026C, ptr(0x101CC258));
  gabi::call(0x028EAB2C, ptr(0x1046BBA9));
  gabi::call(0x028F026C, ptr(0x101CC264));
}
VERIFY(0x02381DB8, paperStaticInit);
static void paperHioDelete(u8 *self, s32 flags) {
  WWHD_FUNC(0x02381E4C, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02381E4C, paperHioDelete);
static void paperEmpty() { WWHD_FUNC(0x02381E60, void); }
VERIFY(0x02381E60, paperEmpty);
static void paperDestructor(Act_c *self, s32 flags) {
  WWHD_FUNC(0x02381E64, void, self, flags);
  if (self) {
    gabi::call(0x02515860, &self->mColStatus, 2);
    gabi::call(0x02515A70, self->mCylinder, 2);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x02381E64, paperDestructor);
BOOL paperIsDelete(Act_c *self) {
  WWHD_FUNC(0x02381ED0, BOOL, self);
  return 1;
}
VERIFY(0x02381ED0, paperIsDelete);
