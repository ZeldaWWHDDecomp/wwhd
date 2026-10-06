// Local WWHD compatibility reconstruction;
#include "gabi.h"
using namespace gabi;
namespace d_com_lib_game {
s32 phase_handler(void* phase, void* handlers, void* user) {
  WWHD_FUNC(0x02525FE4, s32, phase, handlers, user);
  s32 result;
  do { result = call<s32>(0x0201A0AC, phase, handlers, user); } while (result == 2);
  return result;
}
VERIFY(0x02525FE4, phase_handler);
// Trailing per-TU SDK initializer attributed by adjacency; previous initializer belongs to d_com_inf_game.
void static_init() {
  WWHD_FUNC(0x02526044, void);
  store<u32>(0x1047541C, 0); store<u32>(0x10475414, 0);
  store<u32>(0x10475420, 0); store<u32>(0x10475418, 0);
  call<void>(0x028F026C, at<void>(0x101D5EC4));
  f32 first = load<f32>(0x1004BCB8), second = load<f32>(0x1004BCBC);
  store<f32>(0x10475408, first); store<f32>(0x1047540C, second);
  call<void>(0x028ED6F8, at<void>(0x10475410));
  call<void>(0x028F026C, at<void>(0x101D5ED0));
  call<void>(0x028EAB2C, at<void>(0x10475411));
  call<void>(0x028F026C, at<void>(0x101D5EDC));
}
VERIFY(0x02526044, static_init);
}
