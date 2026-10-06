/* WWHD Timer. Derived from zeldaret/tww and audited against HD code.
 * Reconstruction; */
#include "d/actor/d_a_obj_timer.h"
namespace daObjTimer {
u32 prmAbstract(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x023A257C, u32, actor, width, shift);
  u32 parameters = gabi::load<u32>(gabi::ea(actor) + 0xB0);
  width &= 63;
  shift &= 63;
  u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
  return (shift < 32 ? parameters >> shift : 0u) & mask;
}
VERIFY(0x023A257C, prmAbstract);
void Act_c::mode_count_init() {
  WWHD_FUNC(0x023A2154, void, this);
  u32 time = prmAbstract(this, 8, 0) * 15u;
  mMode = 1;
  mTimer = (s32)time;
}
VERIFY(0x023A2154, &Act_c::mode_count_init);
void Act_c::mode_wait_init() {
  WWHD_FUNC(0x023A2198, void, this);
  mTimer = 0;
  mMode = 0;
}
VERIFY(0x023A2198, &Act_c::mode_wait_init);
s32 Act_c::Create() {
  WWHD_FUNC(0x023A21A8, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x10031028);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  gabi::call(0x025DA884, gabi::at<u8>(base + 0xDC));
  u32 sw = prmAbstract(this, 8, 16);
  s32 room = gabi::load<s8>(base + 0x2FE);
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room))
    mode_count_init();
  else
    mode_wait_init();
  return 4;
}
VERIFY(0x023A21A8, &Act_c::Create);
BOOL Act_c::Execute() {
  WWHD_FUNC(0x023A2258, BOOL, this);
  u32 entry = 0x10031038 + (u32)mMode * 8u;
  s32 index = gabi::load<s16>(entry + 2);
  s32 adjust = gabi::load<s16>(entry);
  u32 object = gabi::ea(this) + (u32)adjust;
  u32 target;
  if (index < 0) {
    target = gabi::load<u32>(entry + 4);
  } else {
    s32 offset = gabi::load<s16>(entry + 6);
    u32 table = gabi::load<u32>(object + (u32)offset);
    target = gabi::load<u32>(table + (u32)index * 8u + 4);
  }
  gabi::call_ptr(target, gabi::at<Act_c>(object));
  return TRUE;
}
VERIFY(0x023A2258, &Act_c::Execute);
void Act_c::mode_wait() {
  WWHD_FUNC(0x023A22CC, void, this);
  u32 sw = prmAbstract(this, 8, 16);
  s32 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room))
    mode_count_init();
}
VERIFY(0x023A22CC, &Act_c::mode_wait);
void Act_c::mode_count() {
  WWHD_FUNC(0x023A232C, void, this);
  if (mIsStop)
    return;
  s32 timer = (s32)((u32)mTimer - 1u);
  mTimer = timer;
  if (timer % 30 == 0) {
    s32 seconds = timer / 30;
    if (seconds <= 20) {
      u32 sound = seconds > 10  ? 0x861
                  : seconds > 5 ? 0x862
                  : seconds > 0 ? 0x863
                                : 0x864;
      gabi::call(0x025E1988, sound);
      timer = mTimer;
    }
  }
  if (timer > 0) {
    u32 sw = prmAbstract(this, 8, 16);
    u32 save = gabi::load<u32>(0x101F84DC);
    s32 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
    if (gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room))
      return;
  }
  u32 sw = prmAbstract(this, 8, 16);
  u32 save = gabi::load<u32>(0x101F84DC);
  s32 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
  gabi::call(0x025B9F7C, gabi::at<u8>(save + 0x20), sw, room);
  mode_wait_init();
}
VERIFY(0x023A232C, &Act_c::mode_count);
s32 wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x023A2474, s32, actor);
  return actor->Create();
}
VERIFY(0x023A2474, wrapperCreate);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x023A2478, BOOL, actor);
  return TRUE;
}
VERIFY(0x023A2478, Delete);
BOOL wrapperExecute(Act_c *actor) {
  WWHD_FUNC(0x023A2480, BOOL, actor);
  return actor->Execute();
}
VERIFY(0x023A2480, wrapperExecute);
void staticInitialize() {
  WWHD_FUNC(0x023A2484, void);
  gabi::store<u32>(0x1046C500, 0);
  gabi::store<u32>(0x1046C4F8, 0);
  gabi::store<u32>(0x1046C504, 0);
  gabi::store<u32>(0x1046C4FC, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD2B0));
  gabi::store<f32>(0x1046C4EC, -3.1415927410125732f);
  gabi::store<f32>(0x1046C4F0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046C4F4));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD2BC));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046C4F5));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD2C8));
}
VERIFY(0x023A2484, staticInitialize);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x023A2518, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023A2518, destruct);
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x023A256C, BOOL, actor);
  return TRUE;
}
VERIFY(0x023A256C, Draw);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x023A2574, BOOL, actor);
  return TRUE;
}
VERIFY(0x023A2574, IsDelete);
} // namespace daObjTimer
