#include "gabi.h"
using namespace gabi;
namespace c_phase {
void reset(void *phase) {
  WWHD_FUNC(0x02019F00, void, phase);
  store<u32>(ea(phase) + 4, 0);
}
VERIFY(0x02019F00, reset);
void set(void *phase, void *table) {
  WWHD_FUNC(0x02019F0C, void, phase, table);
  store<u32>(ea(phase), ea(table));
  store<u32>(ea(phase) + 4, 0);
}
VERIFY(0x02019F0C, set);
void uncomplete(void *phase) {
  WWHD_FUNC(0x02019F1C, void, phase);
  store<u32>(ea(phase), 0);
  call<void>(0x02019F00, phase);
}
VERIFY(0x02019F1C, uncomplete);
s32 complete(void *phase) {
  WWHD_FUNC(0x02019F28, s32, phase);
  store<u32>(ea(phase), 0);
  return 4;
}
VERIFY(0x02019F28, complete);
s32 next(void *phase) {
  WWHD_FUNC(0x02019F38, s32, phase);
  u32 table = load<u32>(ea(phase));
  if (!table) return 4;
  u32 index = load<u32>(ea(phase) + 4) + 1u;
  store<u32>(ea(phase) + 4, index);
  if (!load<u32>(table + index * 4u))
    return call<s32>(0x02019F28, phase);
  return 1;
}
VERIFY(0x02019F38, next);
s32 perform(void *phase, void *user) {
  WWHD_FUNC(0x02019F74, s32, phase, user);
  u32 table = load<u32>(ea(phase));
  if (!table) return call<s32>(0x02019F28, phase);
  u32 index = load<u32>(ea(phase) + 4);
  s32 state = call_ptr<s32>(load<u32>(table + index * 4u), user);
  switch (state) {
  case 1: return call<s32>(0x02019F38, phase);
  case 2: return call<s32>(0x02019F38, phase) == 1 ? 2 : 4;
  case 3: call<void>(0x02019F1C, phase); return 3;
  case 4: return call<s32>(0x02019F28, phase);
  case 5: call<void>(0x02019F1C, phase); return 5;
  default: return state;
  }
}
VERIFY(0x02019F74, perform);
s32 handler(void *phase, void *table, void *user) {
  WWHD_FUNC(0x0201A0AC, s32, phase, table, user);
  store<u32>(ea(phase), ea(table));
  return call<s32>(0x02019F74, phase, user);
}
VERIFY(0x0201A0AC, handler);
void static_init() {
  WWHD_FUNC(0x0201A0B8, void);
  store<u32>(0x101FFA64, 0);
  store<u32>(0x101FFA5C, 0);
  store<u32>(0x101FFA68, 0);
  store<u32>(0x101FFA60, 0);
  call<void>(0x028F026C, at<void>(0x1018D2D4));
  f32 first = load<f32>(0x10003650);
  f32 second = load<f32>(0x10003654);
  store<f32>(0x101FFA50, first);
  store<f32>(0x101FFA54, second);
  call<void>(0x028ED6F8, at<void>(0x101FFA58));
  call<void>(0x028F026C, at<void>(0x1018D2E0));
  call<void>(0x028EAB2C, at<void>(0x101FFA59));
  call<void>(0x028F026C, at<void>(0x1018D2EC));
}
VERIFY(0x0201A0B8, static_init);
}

/* ---- hosted here: the static initializer(s) of three separate header-static-only TUs linked between c_phase and c_request (names unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_0201A14C() {
 WWHD_FUNC(0x0201A14C,void);
 gabi::store<u32>(0x101FFA80,0);gabi::store<u32>(0x101FFA78,0);gabi::store<u32>(0x101FFA84,0);gabi::store<u32>(0x101FFA7C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D2F8));
 f32 negativePi=gabi::load<f32>(0x10003660),positivePi=gabi::load<f32>(0x10003664);
 gabi::store<f32>(0x101FFA6C,negativePi);gabi::store<f32>(0x101FFA70,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFA74));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D304));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFA75));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D310));
}
VERIFY(0x0201A14C,hd_static_init_0201A14C);
void hd_static_init_0201A1E0() {
 WWHD_FUNC(0x0201A1E0,void);
 gabi::store<u32>(0x101FFA9C,0);gabi::store<u32>(0x101FFA94,0);gabi::store<u32>(0x101FFAA0,0);gabi::store<u32>(0x101FFA98,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D31C));
 f32 negativePi=gabi::load<f32>(0x1000367C),positivePi=gabi::load<f32>(0x10003680);
 gabi::store<f32>(0x101FFA88,negativePi);gabi::store<f32>(0x101FFA8C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFA90));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D328));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFA91));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D334));
}
VERIFY(0x0201A1E0,hd_static_init_0201A1E0);
void hd_static_init_0201A274() {
 WWHD_FUNC(0x0201A274,void);
 gabi::store<u32>(0x101FFAB8,0);gabi::store<u32>(0x101FFAB0,0);gabi::store<u32>(0x101FFABC,0);gabi::store<u32>(0x101FFAB4,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D340));
 f32 negativePi=gabi::load<f32>(0x10003694),positivePi=gabi::load<f32>(0x10003698);
 gabi::store<f32>(0x101FFAA4,negativePi);gabi::store<f32>(0x101FFAA8,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFAAC));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D34C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFAAD));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D358));
}
VERIFY(0x0201A274,hd_static_init_0201A274);
