// Scoped HD Telescope screen states, reconstructed from WWHD.
#include "gabi.h"
using namespace gabi;
namespace hd_screen_telescope {
void zoomIn(u32 self) {
  WWHD_FUNC(0x026E8050, void, self);
  u32 play = call<u32>(0x025200D4), frame = load<u32>(self + 0x5c) + 1,
      actor = load<u32>(play + 0x5b2c);
  store<u32>(self + 0x5c, frame);
  if ((s32)frame <= 5 || (s32)frame < 10) {
    store<u32>(actor + 0x3b8, load<u32>(actor + 0x3b8) | 0x80000);
    return;
  }
  call<void>(0x020063C0, self + 0x18, 0x1049CA98u);
  play = call<u32>(0x025200D4);
  store<u8>(play + 0x5bb2, 12);
  play = call<u32>(0x025200D4);
  store<u8>(play + 0x5bb3, 13);
}
VERIFY(0x026E8050, zoomIn);
void zoomOut(u32 self) {
  WWHD_FUNC(0x026E83C4, void, self);
  u32 frame = load<u32>(self + 0x5c) + 1;
  store<u32>(self + 0x5c, frame);
  if ((s32)frame < 5)
    return;
  u32 play = call<u32>(0x025200D4);
  store<u8>(play + 0x5bb2, 18);
  call<void>(0x020063C0, self + 0x18, 0x1049CA38u);
}
VERIFY(0x026E83C4, zoomOut);
} // namespace hd_screen_telescope
