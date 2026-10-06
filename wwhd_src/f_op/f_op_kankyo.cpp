// Full HD environment lifecycle TU025DA958..025DAC4F; adjacent initializer attribution inferred.
#include "bindings.h"
namespace f_op_kankyo_cpp {
static s32 IsKankyo(u32 env) {
 WWHD_FUNC(0x025DA958, s32, env);
 u32 type = gabi::load<u32>(env + 0xC4);
 return gabi::call<s32>(0x025DD258, gabi::load<u32>(0x101F3408), type);
}
VERIFY(0x025DA958, IsKankyo);
static s32 Draw(u32 env) {
 WWHD_FUNC(0x025DA968, s32, env);
 s32 result = 1;
 if (gabi::call<s32>(0x025986BC) == 0)
  result = gabi::call<s32>(0x025DF2C0, gabi::load<u32>(env + 0xDC), env);
 return result;
}
VERIFY(0x025DA968, Draw);
static s32 Execute(u32 env) {
 WWHD_FUNC(0x025DA9BC, s32, env);
 s32 result = 1;
 if (gabi::call<s32>(0x025AF2A4) != 0) return result;
 bool allowed = gabi::call<s32>(0x025986BC) == 0;
 if (!allowed && env != 0) {
  allowed = gabi::load<s16>(env + 8) == 21;
  if (!allowed && env != 0) allowed = gabi::load<s16>(env + 8) == 24;
 }
 if (allowed) result = gabi::call<s32>(0x025DFCC4, gabi::load<u32>(env + 0xDC), env);
 return result;
}
VERIFY(0x025DA9BC, Execute);
static s32 IsDelete(u32 env) {
 WWHD_FUNC(0x025DAA44, s32, env);
 s32 result = gabi::call<s32>(0x025DFCCC, gabi::load<u32>(env + 0xDC), env);
 if (result == 1) gabi::call<void>(0x025DA884, env + 0xC8);
 return result;
}
VERIFY(0x025DAA44, IsDelete);
static s32 Delete(u32 env) {
 WWHD_FUNC(0x025DAA98, s32, env);
 s32 result = gabi::call<s32>(0x025DFCD4, gabi::load<u32>(env + 0xDC), env);
 gabi::call<void>(0x025DA884, env + 0xC8);
 return result;
}
VERIFY(0x025DAA98, Delete);
static s32 Create(u32 env) {
 WWHD_FUNC(0x025DAAE4, s32, env);
 if (gabi::load<u8>(env + 0xC) == 0) {
  u32 profile = gabi::load<u32>(env + 0x10);
  u32 type = gabi::call<u32>(0x025DD268, 0x101F3408u);
  gabi::store<u32>(env + 0xC4, type);
  gabi::store<u32>(env + 0xDC, gabi::load<u32>(profile + 0x24));
  gabi::call<void>(0x025DA888, env + 0xC8, env);
  u32 append = gabi::load<u32>(env + 0xAC);
  if (append != 0) {
   for (u32 i = 0; i < 7; i++)
    gabi::store<u32>(env + 0xE0 + 4 * i, gabi::load<u32>(append + 4 * i));
  }
 }
 s32 result = gabi::call<s32>(0x025DFCDC, gabi::load<u32>(env + 0xDC), env);
 if (result == 4) {
  s32 priority = gabi::call<s32>(0x025DF2B8, env);
  gabi::call<void>(0x025DA874, env + 0xC8, priority);
 }
 return result;
}
VERIFY(0x025DAAE4, Create);
static void __sinit_f_op_kankyo_cpp() {
 WWHD_FUNC(0x025DABBC, void, (u32)0);
 sinit_header_statics(0x1048A48C, 0x101F33E4);
}
VERIFY(0x025DABBC, __sinit_f_op_kankyo_cpp);
}
