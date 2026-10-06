#include "gabi.h"
using namespace gabi;
namespace c_tree {
s32 single_cut(void *node) {
  WWHD_FUNC(0x0201AAB0, s32, node);
  return call<s32>(0x0200FDF4, node);
}
VERIFY(0x0201AAB0, single_cut);
s32 addition(void *tree, s32 index, void *node) {
  WWHD_FUNC(0x0201AAB4, s32, tree, index, node);
  if (index >= load<s32>(ea(tree) + 4)) return 0;
  u32 lists = load<u32>(ea(tree));
  return call<s32>(0x0200FE78, at<void>(lists + u32(index) * 12u), node);
}
VERIFY(0x0201AAB4, addition);
s32 insert(void *tree, s32 listIndex, void *node, s32 index) {
  WWHD_FUNC(0x0201AADC, s32, tree, listIndex, node, index);
  if (listIndex >= load<s32>(ea(tree) + 4)) return 0;
  u32 lists = load<u32>(ea(tree));
  return call<s32>(0x0200FF10, at<void>(lists + u32(listIndex) * 12u), index, node);
}
VERIFY(0x0201AADC, insert);
void create(void *tree, void *lists, s32 count) {
  WWHD_FUNC(0x0201AB04, void, tree, lists, count);
  store<u32>(ea(tree) + 4, u32(count));
  store<u32>(ea(tree), ea(lists));
  for (s32 i = 0; i < count; ++i)
    call<void>(0x02010008, at<void>(ea(lists) + u32(i) * 12u));
}
VERIFY(0x0201AB04, create);
void static_init() {
  WWHD_FUNC(0x0201AB5C, void);
  store<u32>(0x101FFB68, 0);
  store<u32>(0x101FFB60, 0);
  store<u32>(0x101FFB6C, 0);
  store<u32>(0x101FFB64, 0);
  call<void>(0x028F026C, at<void>(0x1018D418));
  f32 first = load<f32>(0x100036D8);
  f32 second = load<f32>(0x100036DC);
  store<f32>(0x101FFB54, first);
  store<f32>(0x101FFB58, second);
  call<void>(0x028ED6F8, at<void>(0x101FFB5C));
  call<void>(0x028F026C, at<void>(0x1018D424));
  call<void>(0x028EAB2C, at<void>(0x101FFB5D));
  call<void>(0x028F026C, at<void>(0x1018D430));
}
VERIFY(0x0201AB5C, static_init);
}
