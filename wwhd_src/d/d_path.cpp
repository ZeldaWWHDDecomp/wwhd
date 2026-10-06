// Full HD path TU025AAEB8..025AB363; adjacent sinit inferred.
#include "bindings.h"
namespace d_path_cpp {
static u32 pathInfo(u32 obj) {
 u32 vt = gabi::load<u32>(obj);
 return gabi::call_ptr<u32>(gabi::load<u32>(vt + 0x1AC), obj);
}
static u32 GetPnt(u32 path, s32 index) {
 WWHD_FUNC(0x025AAEB8, u32, path, index);
 if (path == 0) gabi::call<void>(0x0273AA24, 0x10052498u, 30u, 0x10052460u);
 bool valid = index >= 0 && index < gabi::load<u16>(path);
 if (!valid) {
  gabi::call<void>(0x0273AA24, 0x10052498u, 31u, 0x1005246Cu);
  if (path == 0) return 0;
 }
 u32 points = gabi::load<u32>(path + 8);
 if (points == 0 || index < 0 || index >= gabi::load<u16>(path)) return 0;
 return points + (u32)index * 16;
}
VERIFY(0x025AAEB8, GetPnt);
static u32 GetRoomPath(s32 index, s32 room) {
 WWHD_FUNC(0x025AAF88, u32, index, room);
 u32 info;
 if (room == -1) {
  u32 game = gabi::call<u32>(0x025200D4);
  info = pathInfo(game + 0x5150);
 } else {
  if ((u32)room >= 64) gabi::call<void>(0x0273AA24, 0x100524A4u, 61u, 0x100524B0u);
  u32 game = gabi::call<u32>(0x025200D4);
  u32 obj = gabi::call<u32>(0x025C11DC, game + 0x51CC, room);
  if (obj == 0) return 0;
  info = pathInfo(obj);
 }
 if (info == 0 || index < 0 || index >= gabi::load<s32>(info)) return 0;
 return gabi::load<u32>(info + 4) + (u32)index * 12;
}
VERIFY(0x025AAF88, GetRoomPath);
static u32 GetNextRoomPath(u32 path, s32 room) {
 WWHD_FUNC(0x025AB070, u32, path, room);
 u32 game = gabi::call<u32>(0x025200D4);
 u32 info;
 u32 next;
 if (room == -1) {
  info = pathInfo(game + 0x5150);
  next = gabi::load<u16>(path + 2);
 } else {
  u32 obj = gabi::call<u32>(0x025C11DC, game + 0x51CC, room);
  if (obj == 0) return 0;
  info = pathInfo(obj);
  next = gabi::load<u16>(path + 2);
 }
 if (info == 0 || next == 0xFFFF) return 0;
 if ((s32)next >= gabi::load<s32>(info)) {
  gabi::call<void>(0x0273AA24, 0x100524F8u, 114u, 0x100524D4u);
  if ((s32)next >= gabi::load<s32>(info)) return 0;
 }
 return gabi::load<u32>(info + 4) + next * 12;
}
VERIFY(0x025AB070, GetNextRoomPath);
static s32 GetPolyRoomPathVec(u32 poly, u32 out, u32 arg) {
 WWHD_FUNC(0x025AB184, s32, poly, out, arg);
 u32 game = gabi::call<u32>(0x025200D4);
 s32 room = gabi::call<s32>(0x024EF130, game + 0x12A0, poly);
 game = gabi::call<u32>(0x025200D4);
 s32 index = gabi::call<s32>(0x024EF360, game + 0x12A0, poly);
 f32 zero = gabi::load<f32>(0x10052504);
 gabi::store<f32>(out + 4, zero);
 gabi::store<f32>(out + 8, zero);
 gabi::store<f32>(out, zero);
 gabi::store<u32>(arg, 0);
 if (room == -1) return 0;
 u32 path = GetRoomPath(index, room);
 if (path == 0) return 0;
 u32 sw = gabi::load<u8>(path + 6);
 if (sw != 255) {
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025BA0C0, save + 0x20, sw, room) != 0) return 0;
 }
 game = gabi::call<u32>(0x025200D4);
 s32 point = gabi::call<s32>(0x024EF37C, game + 0x12A0, poly);
 if (point == 255 || point < 0) return 0;
 u32 count = gabi::load<u16>(path);
 if ((u32)point >= count) return 0;
 u32 points = gabi::load<u32>(path + 8);
 u32 cur = points + (u32)point * 16;
 u32 next = (u32)point == count - 1 ? points : cur + 16;
 f32 a = gabi::load<f32>(cur + 4);
 f32 b = gabi::load<f32>(next + 4);
 gabi::store<f32>(out, b - a);
 b = gabi::load<f32>(next + 8);
 a = gabi::load<f32>(cur + 8);
 gabi::store<f32>(out + 4, b - a);
 b = gabi::load<f32>(next + 12);
 a = gabi::load<f32>(cur + 12);
 gabi::store<f32>(out + 8, b - a);
 gabi::store<u32>(arg, gabi::load<u8>(path + 4));
 return 1;
}
VERIFY(0x025AB184, GetPolyRoomPathVec);
static void __sinit_d_path_cpp() {
 WWHD_FUNC(0x025AB2D0, void, (u32)0);
 sinit_header_statics(0x1047B30C, 0x101EA560);
}
VERIFY(0x025AB2D0, __sinit_d_path_cpp);
}
