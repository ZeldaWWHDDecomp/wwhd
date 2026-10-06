#include "gabi.h"
using namespace gabi;
namespace f_pc_create_tag {
u32 to_create_queue(void *tag) {
  WWHD_FUNC(0x025DDC18, u32, tag);
  return call<u32>(0x0201A8B4, at<void>(0x101F39A4), tag);
}
VERIFY(0x025DDC18, to_create_queue);
void from_create_queue(void *tag) {
  WWHD_FUNC(0x025DDC28, void, tag);
  call<void>(0x0201A868, tag);
}
VERIFY(0x025DDC28, from_create_queue);
s32 initialize(void *tag, void *data) {
  WWHD_FUNC(0x025DDC2C, s32, tag, data);
  call<void>(0x0201A918, tag, data);
  return 1;
}
VERIFY(0x025DDC2C, initialize);
void static_init() {
  WWHD_FUNC(0x025DDC50, void);
  store<u32>(0x1048A708, 0);
  store<u32>(0x1048A700, 0);
  store<u32>(0x1048A70C, 0);
  store<u32>(0x1048A704, 0);
  call<void>(0x028F026C, at<void>(0x101F3980));
  f32 first = load<f32>(0x10057E94);
  f32 second = load<f32>(0x10057E98);
  store<f32>(0x1048A6F4, first);
  store<f32>(0x1048A6F8, second);
  call<void>(0x028ED6F8, at<void>(0x1048A6FC));
  call<void>(0x028F026C, at<void>(0x101F398C));
  call<void>(0x028EAB2C, at<void>(0x1048A6FD));
  call<void>(0x028F026C, at<void>(0x101F3998));
}
VERIFY(0x025DDC50, static_init);
}
