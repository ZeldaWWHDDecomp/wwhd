#include "gabi.h"
using namespace gabi;
namespace f_pc_method_iter {
s32 method(void *list, u32 callback) {
  WWHD_FUNC(0x025DFD78, s32, list, callback);
  return call<s32>(0x020100A0, list, at<void>(callback), nullptr);
}
VERIFY(0x025DFD78, method);
void static_init() {
  WWHD_FUNC(0x025DFD80, void);
  store<u32>(0x1048AA90, 0);
  store<u32>(0x1048AA88, 0);
  store<u32>(0x1048AA94, 0);
  store<u32>(0x1048AA8C, 0);
  call<void>(0x028F026C, at<void>(0x101F3CD0));
  f32 first = load<f32>(0x100583D0);
  f32 second = load<f32>(0x100583D4);
  store<f32>(0x1048AA7C, first);
  store<f32>(0x1048AA80, second);
  call<void>(0x028ED6F8, at<void>(0x1048AA84));
  call<void>(0x028F026C, at<void>(0x101F3CDC));
  call<void>(0x028EAB2C, at<void>(0x1048AA85));
  call<void>(0x028F026C, at<void>(0x101F3CE8));
}
VERIFY(0x025DFD80, static_init);
}
