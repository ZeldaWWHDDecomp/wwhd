// Complete HD scene lifecycle TU025DC5B8..025DC73B, adjacent sinit attributed by adjacency.
#include "bindings.h"
namespace f_op_scene_cpp {
static s32 Draw(u32 scene) {
 WWHD_FUNC(0x025DC5B8, s32, scene);
 return gabi::call<s32>(0x025DFFA8, gabi::load<u32>(scene + 0x1B0), scene);
}
VERIFY(0x025DC5B8, Draw);
static s32 Execute(u32 scene) {
 WWHD_FUNC(0x025DC5C4, s32, scene);
 return gabi::call<s32>(0x025DFCC4, gabi::load<u32>(scene + 0x1B0), scene);
}
VERIFY(0x025DC5C4, Execute);
static s32 IsDelete(u32 scene) {
 WWHD_FUNC(0x025DC5D0, s32, scene);
 return gabi::call<s32>(0x025DFCCC, gabi::load<u32>(scene + 0x1B0), scene);
}
VERIFY(0x025DC5D0, IsDelete);
static s32 Delete(u32 scene) {
 WWHD_FUNC(0x025DC5DC, s32, scene);
 s32 result = gabi::call<s32>(0x025DFCD4, gabi::load<u32>(scene + 0x1B0), scene);
 if (result == 1) gabi::call<void>(0x025DCFA4, scene + 0x1B4);
 gabi::call<void>(0x025F0A0C);
 return result;
}
VERIFY(0x025DC5DC, Delete);
static s32 Create(u32 scene) {
 WWHD_FUNC(0x025DC634, s32, scene);
 if (gabi::load<u8>(scene + 0xC) == 0) {
  u32 profile = gabi::load<u32>(scene + 0x10);
  gabi::store<u32>(scene + 0x1B0, gabi::load<u32>(profile + 0x20));
  gabi::call<void>(0x025DCFB8, scene + 0x1B4, scene);
  gabi::call<void>(0x025DCFA8, scene + 0x1B4);
  u32 append = gabi::load<u32>(scene + 0xAC);
  if (append != 0) gabi::store<u32>(scene + 0xB0, gabi::load<u32>(append));
 }
 return gabi::call<s32>(0x025DFCDC, gabi::load<u32>(scene + 0x1B0), scene);
}
VERIFY(0x025DC634, Create);
static void __sinit_f_op_scene_cpp() {
 WWHD_FUNC(0x025DC6A8, void, (u32)0);
 sinit_header_statics(0x1048A5A4, 0x101F3738);
}
VERIFY(0x025DC6A8, __sinit_f_op_scene_cpp);
}
