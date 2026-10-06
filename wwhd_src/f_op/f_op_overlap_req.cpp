// Full HD overlap request TU025DC050..025DC5B7; adjacent sinit attribution inferred.
#include "bindings.h"
namespace f_op_overlap_req_cpp {
static s32 phase_Done(u32 req) {
 WWHD_FUNC(0x025DC050, s32, req);
 if (gabi::call<s32>(0x025DF944, gabi::load<u32>(req + 0x20)) != 1) return 0;
 gabi::store<u32>(req + 0x20, 0);
 gabi::store<u16>(req + 6, 0);
 gabi::store<u32>(req + 0xC, 0);
 gabi::store<u32>(req + 8, 0);
 gabi::store<u16>(req + 4, 0);
 return 2;
}
VERIFY(0x025DC050, phase_Done);
static s32 phase_IsDone(u32 req) {
 WWHD_FUNC(0x025DC0BC, s32, req);
 gabi::call<void>(0x0201A330, req);
 s32 delay = gabi::load<s16>(req + 2);
 gabi::store<u16>(req + 2, (u16)(delay - 1));
 return delay <= 0 ? 2 : 0;
}
VERIFY(0x025DC0BC, phase_IsDone);
static s32 phase_IsWaitOfFadeout(u32 req) {
 WWHD_FUNC(0x025DC104, s32, req);
 if (gabi::call<s32>(0x0201A308, gabi::load<u32>(req + 0x20) + 0xC8) == 0) return 0;
 gabi::store<u32>(req + 8, 0);
 return 2;
}
VERIFY(0x025DC104, phase_IsWaitOfFadeout);
static s32 phase_WaitOfFadeout(u32 req) {
 WWHD_FUNC(0x025DC164, s32, req);
 u16 peek = gabi::load<u16>(req + 6);
 u8 command = gabi::load<u8>(req);
 if (peek != 0) {
  peek = (u16)(peek - 1);
  gabi::store<u16>(req + 6, peek);
 }
 if ((command & 0x3F) == 2 && peek == 0) {
  gabi::call<void>(0x0201A34C, gabi::load<u32>(req + 0x20) + 0xC8, 2u);
  return 2;
 }
 gabi::store<u32>(req + 8, 1);
 return 0;
}
VERIFY(0x025DC164, phase_WaitOfFadeout);
static s32 phase_IsComplete(u32 req) {
 WWHD_FUNC(0x025DC1E0, s32, req);
 if (gabi::call<s32>(0x0201A308, gabi::load<u32>(req + 0x20) + 0xC8) == 0) return 0;
 gabi::call<void>(0x0201A330, req);
 return 2;
}
VERIFY(0x025DC1E0, phase_IsComplete);
static s32 phase_IsCreated(u32 req) {
 WWHD_FUNC(0x025DC240, s32, req);
 if (gabi::call<s32>(0x025DD868, gabi::load<u32>(req + 0x14)) != 0) return 0;
 u32 task = gabi::call<u32>(0x025DE50C, gabi::load<u32>(req + 0x14));
 if (task == 0) return 5;
 gabi::store<u32>(req + 0x20, task);
 u8 mode = gabi::load<u8>(req + 0x28);
 u32 game = gabi::load<u32>(0x101F86B4);
 gabi::store<u8>(game + 0x444, mode);
 return 2;
}
VERIFY(0x025DC240, phase_IsCreated);
static s32 phase_Create(u32 req) {
 WWHD_FUNC(0x025DC2D8, s32, req);
 gabi::call<void>(0x025DEAB4, gabi::load<u32>(req + 0x24));
 s32 name = gabi::load<s16>(req + 0x10);
 u32 layer = gabi::call<u32>(0x025DED64);
 u32 id = gabi::call<u32>(0x025E14A8, layer, name, 0u, 0u, 0u);
 gabi::store<u32>(req + 0x14, id);
 return 2;
}
VERIFY(0x025DC2D8, phase_Create);
static void SetPeektime(u32 req, u32 peek) {
 WWHD_FUNC(0x025DC334, void, req, peek);
 if (peek > 0x7FFF) return;
 gabi::store<u16>(req + 6, (u16)peek);
}
VERIFY(0x025DC334, SetPeektime);
static u32 Request(u32 req, u32 name, u32 peek, u32 mode) {
 WWHD_FUNC(0x025DC344, u32, req, name, peek, mode);
 if (gabi::load<u16>(req + 4) == 1) return 0;
 gabi::call<void>(0x0201A34C, req, 1u);
 gabi::store<u16>(req + 0x10, (u16)name);
 gabi::call<void>(0x02019F0C, req + 0x18, 0x101F36F4u);
 SetPeektime(req, peek);
 gabi::store<u16>(req + 4, 1);
 gabi::store<u32>(req + 8, 0);
 gabi::store<u16>(req + 2, 1);
 gabi::store<u32>(req + 0xC, 0);
 gabi::store<u32>(req + 0x20, 0);
 u32 root = gabi::call<u32>(0x025DEAA8);
 gabi::store<u32>(req + 0x24, root);
 gabi::store<u8>(req + 0x28, (u8)mode);
 return req;
}
VERIFY(0x025DC344, Request);
static s32 Handler(u32 req) {
 WWHD_FUNC(0x025DC418, s32, req);
 for (;;) {
  u32 phase = gabi::call<u32>(0x02019F74, req + 0x18, req);
  if (phase < 2) return 0;
  if (phase == 2) continue;
  if (phase == 4) return 4;
  return 5;
 }
}
VERIFY(0x025DC418, Handler);
static s32 Cancel(u32 req) {
 WWHD_FUNC(0x025DC488, s32, req);
 return phase_Done(req) == 2;
}
VERIFY(0x025DC488, Cancel);
static s32 IsPeektimeLimit(u32 req) {
 WWHD_FUNC(0x025DC4B4, s32, req);
 return gabi::load<u16>(req + 6) == 0;
}
VERIFY(0x025DC4B4, IsPeektimeLimit);
static s32 OverlapClr(u32 req) {
 WWHD_FUNC(0x025DC4C4, s32, req);
 if ((gabi::load<u8>(req) & 0x80) != 0) return 0;
 if (IsPeektimeLimit(req) == 0) return 0;
 gabi::call<void>(0x0201A33C, req, 2u);
 return 1;
}
VERIFY(0x025DC4C4, OverlapClr);
static void __sinit_f_op_overlap_req_cpp() {
 WWHD_FUNC(0x025DC524, void, (u32)0);
 sinit_header_statics(0x1048A588, 0x101F3714);
}
VERIFY(0x025DC524, __sinit_f_op_overlap_req_cpp);
}
