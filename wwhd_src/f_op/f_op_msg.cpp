// Full HD message lifecycle TU025DB050..025DB2BB, adjacent initializer attribution inferred.
#include "bindings.h"
namespace f_op_msg_cpp {
static s32 Draw(u32 msg) {
 WWHD_FUNC(0x025DB050, s32, msg);
 return gabi::call<s32>(0x025DF2C0, gabi::load<u32>(msg + 0xDC), msg);
}
VERIFY(0x025DB050, Draw);
static s32 Execute(u32 msg) {
 WWHD_FUNC(0x025DB05C, s32, msg);
 s32 result = 1;
 if (gabi::call<s32>(0x025AF2A4) == 0)
  result = gabi::call<s32>(0x025DFCC4, gabi::load<u32>(msg + 0xDC), msg);
 return result;
}
VERIFY(0x025DB05C, Execute);
static s32 IsDelete(u32 msg) {
 WWHD_FUNC(0x025DB0B0, s32, msg);
 s32 result = gabi::call<s32>(0x025DFCCC, gabi::load<u32>(msg + 0xDC), msg);
 if (result == 1) gabi::call<void>(0x025DA884, msg + 0xC8);
 return result;
}
VERIFY(0x025DB0B0, IsDelete);
static s32 Delete(u32 msg) {
 WWHD_FUNC(0x025DB104, s32, msg);
 s32 result = gabi::call<s32>(0x025DFCD4, gabi::load<u32>(msg + 0xDC), msg);
 gabi::call<void>(0x025DA884, msg + 0xC8);
 return result;
}
VERIFY(0x025DB104, Delete);
static s32 Create(u32 msg) {
 WWHD_FUNC(0x025DB150, s32, msg);
 if (gabi::load<u8>(msg + 0xC) == 0) {
  u32 profile = gabi::load<u32>(msg + 0x10);
  u32 type = gabi::call<u32>(0x025DD268, 0x101F3468u);
  gabi::store<u32>(msg + 0xC4, type);
  gabi::store<u32>(msg + 0xDC, gabi::load<u32>(profile + 0x24));
  gabi::call<void>(0x025DA888, msg + 0xC8, msg);
  u32 append = gabi::load<u32>(msg + 0xAC);
  if (append != 0) {
   for (u32 i = 0; i < 7; i++)
    gabi::store<u32>(msg + 0xE0 + 4 * i, gabi::load<u32>(append + 4 * i));
  }
 }
 s32 result = gabi::call<s32>(0x025DFCDC, gabi::load<u32>(msg + 0xDC), msg);
 if (result == 4) {
  s32 priority = gabi::call<s32>(0x025DF2B8, msg);
  gabi::call<void>(0x025DA874, msg + 0xC8, priority);
 }
 return result;
}
VERIFY(0x025DB150, Create);
static void __sinit_f_op_msg_cpp() {
 WWHD_FUNC(0x025DB228, void, (u32)0);
 sinit_header_statics(0x1048A4C4, 0x101F3444);
}
VERIFY(0x025DB228, __sinit_f_op_msg_cpp);
}
