// Scoped HD Shortcut screen event handlers, reconstructed from WWHD.
#include "gabi.h"
using namespace gabi;
namespace hd_screen_shortcut {
void prepareState(u32 self, u32 state) {
  WWHD_FUNC(0x026DC1E8, void, self, state);
  store<u32>(self + 0x94, state);
  call<void>(0x020063C0, self + 0x18, 0x1049B540u);
}
VERIFY(0x026DC1E8, prepareState);
s32 advanceOne(u32 self) {
  WWHD_FUNC(0x026DC1FC, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  call<void>(0x026DC1E8, self, 0x1049B630u);
  return 0;
}
VERIFY(0x026DC1FC, advanceOne);
s32 advanceThree(u32 self) {
  WWHD_FUNC(0x026DC234, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x78);
  call<void>(0x026DC1E8, self, 0x1049B510u);
  return 0;
}
VERIFY(0x026DC234, advanceThree);
s32 advanceToNext(u32 self) {
  WWHD_FUNC(0x026DC29C, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  call<void>(0x026DC1E8, self, 0x1049B690u);
  return 0;
}
VERIFY(0x026DC29C, advanceToNext);
s32 activate(u32 self) {
  WWHD_FUNC(0x026DC2D4, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  store<u8>(0x1047B07C, 1);
  call<void>(0x020063C0, self + 0x18, 0x1049B600u);
  store<u8>(self + 0xd3, 1);
  return 0;
}
VERIFY(0x026DC2D4, activate);
s32 activateAlternate(u32 self) {
  WWHD_FUNC(0x026DC334, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  store<u8>(0x1047B07C, 1);
  call<void>(0x020063C0, self + 0x18, 0x1049B600u);
  store<u8>(self + 0xd3, 1);
  return 0;
}
VERIFY(0x026DC334, activateAlternate);
s32 advanceReturn(u32 self) {
  WWHD_FUNC(0x026DC414, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  call<void>(0x026DC1E8, self, 0x1049B510u);
  return 0;
}
VERIFY(0x026DC414, advanceReturn);
s32 advanceReturnAlternate(u32 self) {
  WWHD_FUNC(0x026DC45C, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) + 0x28);
  call<void>(0x026DC1E8, self, 0x1049B510u);
  return 0;
}
VERIFY(0x026DC45C, advanceReturnAlternate);
s32 retreatReturn(u32 self) {
  WWHD_FUNC(0x026DC4A4, s32, self);
  store<u32>(self + 0x8c, load<u32>(self + 0x8c) - 0x28);
  call<void>(0x026DC1E8, self, 0x1049B510u);
  return 0;
}
VERIFY(0x026DC4A4, retreatReturn);
} // namespace hd_screen_shortcut
