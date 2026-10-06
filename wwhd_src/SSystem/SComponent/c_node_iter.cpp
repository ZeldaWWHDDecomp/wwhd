#include "gabi.h"
using namespace gabi;
namespace c_node_iter {
s32 method(void *node, u32 callback, void *user) {
  WWHD_FUNC(0x02019D50, s32, node, callback, user);
  s32 result = 1;
  u32 next = node ? load<u32>(ea(node) + 8) : 0;
  while (node) {
    if (!call_ptr<s32>(callback, node, user)) result = 0;
    node = at<void>(next);
    next = node ? load<u32>(ea(node) + 8) : 0;
  }
  return result;
}
VERIFY(0x02019D50, method);
void *judge(void *node, u32 callback, void *user) {
  WWHD_FUNC(0x02019DE8, void *, node, callback, user);
  u32 next = node ? load<u32>(ea(node) + 8) : 0;
  while (node) {
    void *result = call_ptr<void *>(callback, node, user);
    if (result) return result;
    node = at<void>(next);
    next = node ? load<u32>(ea(node) + 8) : 0;
  }
  return nullptr;
}
VERIFY(0x02019DE8, judge);
void static_init() {
  WWHD_FUNC(0x02019E6C, void);
  store<u32>(0x101FFA48, 0);
  store<u32>(0x101FFA40, 0);
  store<u32>(0x101FFA4C, 0);
  store<u32>(0x101FFA44, 0);
  call<void>(0x028F026C, at<void>(0x1018D2B0));
  f32 first = load<f32>(0x10003648);
  f32 second = load<f32>(0x1000364C);
  store<f32>(0x101FFA34, first);
  store<f32>(0x101FFA38, second);
  call<void>(0x028ED6F8, at<void>(0x101FFA3C));
  call<void>(0x028F026C, at<void>(0x1018D2BC));
  call<void>(0x028EAB2C, at<void>(0x101FFA3D));
  call<void>(0x028F026C, at<void>(0x1018D2C8));
}
VERIFY(0x02019E6C, static_init);
}
