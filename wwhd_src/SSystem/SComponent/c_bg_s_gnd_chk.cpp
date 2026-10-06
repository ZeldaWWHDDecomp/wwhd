#include "gabi.h"
using namespace gabi;
namespace c_bg_s_gnd_chk {
void destroy(void *object, s32 deleting) {
  WWHD_FUNC(0x02008DAC, void, object, deleting);
  if (!object) return;
  store<u32>(ea(object) + 0x20, 0x10000F30);
  call<void>(0x02008B4C, object, 0);
  if (u32(deleting) & 1u) call<void>(0x0273AF40, object);
}
VERIFY(0x02008DAC, destroy);
void *construct(void *object) {
  WWHD_FUNC(0x02008E0C, void *, object);
  if (!object) object = call<void *>(0x0273AD10, 0x40);
  if (!object) return nullptr;
  call<void>(0x02008B60, object);
  u32 a = ea(object);
  store<u16>(a + 0x16, 0x100);
  store<u32>(a + 0x20, 0x10000F30);
  store<u32>(a + 0x3C, 0);
  store<u32>(a + 0x38, 0);
  store<u32>(a + 0x30, 0);
  store<u32>(a + 0x18, 0);
  store<f32>(a + 0x34, load<f32>(0x10000F0C));
  store<u32>(a + 0x10, 0x10000F20);
  store<u16>(a + 0x14, 0xFFFF);
  store<u32>(a + 0x1C, 0xFFFFFFFF);
  store<u32>(a + 0x24, load<u32>(0x101FFBA8));
  store<u32>(a + 0x28, load<u32>(0x101FFBAC));
  u32 z = load<u32>(0x101FFBB0);
  store<u32>(a + 0x30, 3);
  store<u32>(a + 0x2C, z);
  store<u32>(a + 8, 0xFFFFFFFF);
  return object;
}
VERIFY(0x02008E0C, construct);
void static_init() {
  WWHD_FUNC(0x02008ED0, void);
  store<u32>(0x101FF454, 0);
  store<u32>(0x101FF44C, 0);
  store<u32>(0x101FF458, 0);
  store<u32>(0x101FF450, 0);
  call<void>(0x028F026C, at<void>(0x1018C5B8));
  f32 first = load<f32>(0x10000F14);
  f32 second = load<f32>(0x10000F18);
  store<f32>(0x101FF440, first);
  store<f32>(0x101FF444, second);
  call<void>(0x028ED6F8, at<void>(0x101FF448));
  call<void>(0x028F026C, at<void>(0x1018C5C4));
  call<void>(0x028EAB2C, at<void>(0x101FF449));
  call<void>(0x028F026C, at<void>(0x1018C5D0));
}
VERIFY(0x02008ED0, static_init);
}
