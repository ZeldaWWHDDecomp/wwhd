/* Nintendo Gallery figurines and pedestal. */
#include "gabi.h"
#include <cmath>
#include <limits>
namespace figure {
using gabi::call;
using gabi::load;
using gabi::store;
static u32 play() { return call<u32>(0x025200D4); }
static u32 resource(u32 name, s32 index) {
  gabi::Local<u32[2]> text;
  store<u32>(text.a, name);
  store<u32>(text.a + 4, 0x100282AC);
  return call<u32>(0x026067F4, load<u32>(0x101F4F28), text.a, index);
}
static void copyMatrix(u32 src, u32 dst) {
  f32 v[12];
  for (int i = 0; i < 12; ++i)
    v[i] = load<f32>(src + 4 * i);
  for (int i = 0; i < 12; ++i)
    store<f32>(dst + 4 * i, v[i]);
}
static s32 truncWord(f64 value) {
  if (!std::isfinite(value) || value >= 2147483648.0 || value < -2147483648.0)
    return (s32)0x80000000u;
  return (s32)value;
}
s32 getFigureBmd(u32 a, u32 no) {
  WWHD_FUNC(0x0233F2F0, s32, a, no);
  s32 index = load<s32>(0x10028330 + 12 * no);
  if (no == 0x10 || no == 0x12) {
    if (call<s32>(0x025B8B94, load<u32>(0x101F84DC) + 0x644, 0x2D01))
      index = no == 0x10 ? 3 : 6;
  }
  return index;
}
VERIFY(0x0233F2F0, getFigureBmd);
bool createHeap(u32 a) {
  WWHD_FUNC(0x0233F37C, bool, a);
  s32 room = call<s32>(0x025BD64C, load<u8>(a + 0xA31));
  u32 no = load<u8>(a + 0xA31);
  s32 overrideRoom = load<s32>(0x10028338 + 12 * no);
  if (overrideRoom >= 0)
    room = overrideRoom;
  u32 arc = load<u32>(0x101C927C + 4 * (u32)room);
  s32 index = getFigureBmd(a, no);
  u32 data = resource(arc, index);
  if (!data)
    return false;
  no = load<u8>(a + 0xA31);
  u32 model =
      call<u32>(0x025E38E0, data, 0x80000, load<u32>(0x10028334 + 12 * no));
  store<u32>(a + 0x3C4, model);
  if (!model)
    return false;
  no = load<u8>(a + 0xA31);
  if (no == 0x3D) {
    u32 animation = resource(arc, 0x1A);
    if (!animation)
      return false;
    u32 controller = call<u32>(0x025E80D0, 0);
    store<u32>(a + 0x4C0, controller);
    if (!controller)
      return false;
    if (!call<s32>(0x025E8154, controller, data, animation, 1, 2, 0, -1, 0,
                   1.0f, 0))
      return false;
    no = load<u8>(a + 0xA31);
  }
  if (no == 0x40) {
    u32 pedestal = resource(arc, 0x15);
    u32 animation = resource(arc, 0);
    u32 morf = call<u32>(0x025E4F64, 0, pedestal, 0, 0, animation, 2, 0, -1,
                         1.0f, 1, 0, 0x80000, 0x11001222);
    store<u32>(a + 0x4BC, morf);
    if (!morf || !load<u32>(morf + 0x90))
      return false;
  }
  u32 pedestal = resource(0x1002897C, 1);
  if (!pedestal)
    return false;
  model = call<u32>(0x025E38E0, pedestal, 0x80000, 0x11020022);
  store<u32>(a + 0x3C8, model);
  if (!model)
    return false;
  u32 pattern = resource(0x1002897C, 2);
  store<u32>(a + 0x3CC, pattern);
  if (!pattern)
    return false;
  return call<s32>(0x025E789C, a + 0x3D0, pedestal, pattern, 1, 2, 0, -1, 0,
                   1.0f, 0) != 0;
}
VERIFY(0x0233F37C, createHeap);
bool heapCallback(u32 a) {
  WWHD_FUNC(0x0233F630, bool, a);
  return createHeap(a);
}
VERIFY(0x0233F630, heapCallback);
u32 PrmAbstract(u32 a, s32 width, s32 shift) {
  WWHD_FUNC(0x023415F8, u32, a, width, shift);
  u32 w = (u32)width & 63, s = (u32)shift & 63;
  return (s < 32 ? load<u32>(a + 0xB0) >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
VERIFY(0x023415F8, PrmAbstract);
u8 getPrmFigureNo(u32 a) {
  WWHD_FUNC(0x0233F634, u8, a);
  return (u8)PrmAbstract(a, 8, 0);
}
VERIFY(0x0233F634, getPrmFigureNo);
u8 isFigureGet(u32 a, u32 no) {
  WWHD_FUNC(0x0233F660, u8, a, no);
  u32 byte = no >> 3;
  if ((s32)byte >= 17)
    return 0;
  u16 event = load<u16>(0x101C92E0 + 2 * byte);
  return (u8)(call<u32>(0x025B8BB0, load<u32>(0x101F84DC) + 0x644, event) &
              (1u << (no & 7)));
}
VERIFY(0x0233F660, isFigureGet);
u32 constructor(u32 a) {
  WWHD_FUNC(0x0233F6DC, u32, a);
  if (!a) {
    a = call<u32>(0x0273AD10, 0xA38);
    if (!a)
      return 0;
  }
  call<void>(0x025D4ED0, a);
  store<u32>(a + 0xB4, 0x100282D4);
  call<void>(0x025E7820, a + 0x3D0);
  call<void>(0x025E7820, a + 0x448);
  f32 defaultf10 = load<f32>(0x1016E414);
  f32 defaultf9 = load<f32>(0x1016E418);
  f32 defaultf8 = load<f32>(0x1016E41C);
  f32 defaultf7 = load<f32>(0x1016E420);
  f32 defaultf12 = load<f32>(0x1016E424);
  f32 defaultf11 = load<f32>(0x1016E428);
  u8 defaultr0 = load<u8>(0x1016E42C);
  u8 defaultr9 = load<u8>(0x1016E42D);
  u8 defaultr7 = load<u8>(0x1016E42E);
  u8 defaultr6 = load<u8>(0x1016E42F);
  s16 defaultr10 = load<s16>(0x1016E430);
  s16 defaultr5 = load<s16>(0x1016E432);
  s16 defaultr12 = load<s16>(0x1016E434);
  s16 defaultr11 = load<s16>(0x1016E436);
  f32 defaultf6 = load<f32>(0x1016E438);
  f32 defaultf5 = load<f32>(0x1016E43C);
  f32 defaultf4 = load<f32>(0x1016E440);
  f32 defaultf3 = load<f32>(0x1016E444);
  f32 defaultf2 = load<f32>(0x1016E448);
  f32 defaultf1 = load<f32>(0x1016E44C);
  f32 defaultf0 = load<f32>(0x1016E450);
  f32 defaultf13 = load<f32>(0x1016E454);
  store<f32>(a + 0x4C4, defaultf10);
  store<f32>(a + 0x4C8, defaultf9);
  store<f32>(a + 0x4CC, defaultf8);
  store<f32>(a + 0x4D0, defaultf7);
  store<f32>(a + 0x4D4, defaultf12);
  store<f32>(a + 0x4D8, defaultf11);
  store<u8>(a + 0x4DC, defaultr0);
  store<u8>(a + 0x4DD, defaultr9);
  store<u8>(a + 0x4DE, defaultr7);
  store<u8>(a + 0x4DF, defaultr6);
  store<s16>(a + 0x4E0, defaultr10);
  store<s16>(a + 0x4E2, defaultr5);
  store<s16>(a + 0x4E4, defaultr12);
  store<s16>(a + 0x4E6, defaultr11);
  store<f32>(a + 0x4E8, defaultf6);
  store<f32>(a + 0x4EC, defaultf5);
  store<f32>(a + 0x4F0, defaultf4);
  store<f32>(a + 0x4F4, defaultf3);
  store<f32>(a + 0x4F8, defaultf2);
  store<f32>(a + 0x4FC, defaultf1);
  store<f32>(a + 0x500, defaultf0);
  store<u8>(a + 0x59E, defaultr7);
  store<f32>(a + 0x5C4, defaultf13);
  store<f32>(a + 0x598, defaultf11);
  store<f32>(a + 0x504, defaultf13);
  store<u8>(a + 0x620, defaultr0);
  store<f32>(a + 0x584, defaultf10);
  store<s16>(a + 0x62A, defaultr11);
  store<f32>(a + 0x5B4, defaultf3);
  store<s16>(a + 0x5A2, defaultr5);
  store<f32>(a + 0x590, defaultf7);
  store<f32>(a + 0x5A8, defaultf6);
  store<f32>(a + 0x5BC, defaultf1);
  store<f32>(a + 0x614, defaultf7);
  store<f32>(a + 0x634, defaultf4);
  store<f32>(a + 0x594, defaultf12);
  store<u8>(a + 0x622, defaultr7);
  store<f32>(a + 0x5B0, defaultf4);
  store<u8>(a + 0x59F, defaultr6);
  store<f32>(a + 0x58C, defaultf8);
  store<s16>(a + 0x624, defaultr10);
  store<f32>(a + 0x60C, defaultf9);
  store<f32>(a + 0x588, defaultf9);
  store<u8>(a + 0x59D, defaultr9);
  store<s16>(a + 0x626, defaultr5);
  store<f32>(a + 0x638, defaultf3);
  store<f32>(a + 0x618, defaultf12);
  store<f32>(a + 0x5C0, defaultf0);
  store<u8>(a + 0x623, defaultr6);
  store<f32>(a + 0x610, defaultf8);
  store<f32>(a + 0x630, defaultf5);
  store<s16>(a + 0x628, defaultr12);
  store<s16>(a + 0x5A4, defaultr12);
  store<u8>(a + 0x59C, defaultr0);
  store<s16>(a + 0x5A0, defaultr10);
  store<f32>(a + 0x5B8, defaultf2);
  store<s16>(a + 0x5A6, defaultr11);
  store<f32>(a + 0x63C, defaultf2);
  store<f32>(a + 0x5AC, defaultf5);
  store<u8>(a + 0x621, defaultr9);
  store<f32>(a + 0x62C, defaultf6);
  store<f32>(a + 0x640, defaultf1);
  store<f32>(a + 0x61C, defaultf11);
  store<f32>(a + 0x608, defaultf10);
  store<f32>(a + 0x644, defaultf0);
  store<f32>(a + 0x648, defaultf13);
  call<void>(0x0200BD2C, a + 0x6EC);
  call<void>(0x02515DA0, a + 0x708);
  store<u32>(a + 0x704, 0x1004AE88);
  store<u32>(a + 0x708, 0x1004AEC0);
  call<void>(0x02515FB8, a + 0x728);
  store<u32>(a + 0x83C, 0x100015A8);
  store<u32>(a + 0x838, 0x100282C4);
  call<void>(0x02018590, a + 0x840);
  store<u32>(a + 0x764, 0x1004B108);
  store<u32>(a + 0x83C, 0x1004B160);
  store<u32>(a + 0x854, 0x1004B150);
  call<void>(0x02515FB8, a + 0x858);
  store<u32>(a + 0x96C, 0x100015A8);
  store<u32>(a + 0x968, 0x100282C4);
  call<void>(0x02018590, a + 0x970);
  store<u32>(a + 0x894, 0x1004B108);
  store<u32>(a + 0x984, 0x1004B150);
  store<u32>(a + 0x96C, 0x1004B160);
  call<void>(0x0259F740, a + 0x994);
  u32 no = getPrmFigureNo(a);
  if (no != 0xFF && no < 0x86) {
    s16 angle = load<s16>(a + 0x322);
    store<u8>(a + 0xA31, no);
    store<s16>(a + 0xA28, angle);
    u8 display = isFigureGet(a, (u8)no);
    store<u16>(a + 0xA28, 0);
    store<u8>(a + 0xA32, display);
    store<u16>(a + 0xA26, 0);
    angle = load<s16>(a + 0x322);
    store<u8>(a + 0xA30, 0);
    store<s16>(a + 0xA2A, angle);
  } else {
    s16 angle = load<s16>(a + 0x322);
    store<u8>(a + 0xA31, 0);
    store<s16>(a + 0xA28, angle);
    u8 display = isFigureGet(a, 0);
    angle = load<s16>(a + 0x322);
    store<u8>(a + 0xA32, display);
    store<s16>(a + 0xA2A, angle);
    store<u16>(a + 0xA28, 0);
    store<u16>(a + 0xA26, 0);
    store<u8>(a + 0xA30, 0);
  }
  return a;
}
VERIFY(0x0233F6DC, constructor);
s32 phase1(u32 a) {
  WWHD_FUNC(0x0233F9C4, s32, a);
  u32 flags = load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      constructor(a);
      flags = load<u32>(a + 0x2E4);
    }
    store<u32>(a + 0x2E4, flags | 8);
  }
  store<u8>(a + 0xA36, load<u8>(a + 0xA36) | 1);
  s32 phase = call<s32>(0x02520460, a + 0x3B4, 0x10028984);
  return phase == 4 ? 2 : phase;
}
VERIFY(0x0233F9C4, phase1);
void setMtx(u32 a) {
  WWHD_FUNC(0x0233FA44, void, a);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334);
  u32 model = load<u32>(a + 0x3C8);
  f32 z = load<f32>(a + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  x = load<f32>(a + 0x314);
  y = load<f32>(a + 0x318);
  z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  call<void>(0x025F1C28, 0x1048D0CCu, load<s16>(a + 0x322));
  copyMatrix(0x1048D0CC, load<u32>(a + 0x3C8) + 0xC8);
  model = load<u32>(a + 0x3C4);
  store<f32>(model + 0xC4, 1.0f);
  store<f32>(model + 0xC0, 1.0f);
  store<f32>(model + 0xBC, 1.0f);
  y = load<f32>(a + 0x318);
  x = load<f32>(a + 0x314);
  z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, (f32)(y + 100.0f), z);
  call<void>(0x025F1C28, 0x1048D0CCu, load<s16>(a + 0xA2A));
  copyMatrix(0x1048D0CC, load<u32>(a + 0x3C4) + 0xC8);
  u32 morf = load<u32>(a + 0x4BC);
  if (morf) {
    model = load<u32>(morf + 0x90);
    store<f32>(model + 0xBC, 1.0f);
    store<f32>(model + 0xC0, 1.0f);
    store<f32>(model + 0xC4, 1.0f);
    morf = load<u32>(a + 0x4BC);
    model = load<u32>(morf + 0x90);
    copyMatrix(0x1048D0CC, model + 0xC8);
  }
}
VERIFY(0x0233FA44, setMtx);
s32 createInit(u32 a) {
  WWHD_FUNC(0x0233FC60, s32, a);
  call<void>(0x02515F14, a + 0x6EC, 0xFF, 0xFF, a);
  call<void>(0x02516518, a + 0x728, 0x101C9304);
  store<u32>(a + 0x76C, a + 0x6EC);
  call<void>(0x02516518, a + 0x858, 0x101C9304);
  store<u32>(a + 0x89C, a + 0x6EC);
  store<u32>(a + 0x884, 0x129);
  call<void>(0x020184DC, a + 0x970, 200.0f);
  call<void>(0x02018428, a + 0x970, 100.0f);
  u32 matrix = load<u32>(a + 0x3C8);
  if (matrix)
    matrix += 0xC8;
  store<u32>(a + 0x348, matrix);
  call<void>(0x025D674C, a, -50.0f, 0.0f, -50.0f, 50.0f, 450.0f, 50.0f);
  s32 event = call<s32>(0x02543F10, play() + 0x52C4, 0x100289ACu, 0xFF);
  store<s16>(a + 0xA24, event);
  call<void>(0x0259F7D4, a + 0x994, 0x100289A4u, a);
  s16 id = load<s16>(a + 0xA24);
  s8 room = load<s8>(a + 0x2FE);
  store<s16>(a + 0xFC, id);
  store<u32>(a + 0x39C, 0x2000000A);
  u16 rz = load<u16>(a + 0x324), rx = load<u16>(a + 0x320),
      ry = load<u16>(a + 0x322);
  store<u8>(a + 0x38B, 0xA8);
  store<u8>(a + 0x389, 0xA8);
  store<u16>(a + 0x328, rx);
  store<u16>(a + 0x32C, rz);
  store<u16>(a + 0x32A, ry);
  call<void>(0x0255FFF4, a + 0x4C4, room, 0xFF);
  call<void>(0x028E90D4, 0x101F48F0u, a + 0x6BC);
  setMtx(a);
  return 4;
}
VERIFY(0x0233FC60, createInit);
s32 phase2(u32 a) {
  WWHD_FUNC(0x0233FDD8, s32, a);
  u8 flags = load<u8>(a + 0xA36), no = load<u8>(a + 0xA31);
  store<u8>(a + 0xA36, flags | 2);
  s32 room = call<s32>(0x025BD64C, no);
  s32 overrideRoom = load<s32>(0x10028338 + 12 * (u32)no);
  if (overrideRoom >= 0)
    room = overrideRoom;
  s32 phase =
      call<s32>(0x02520460, a + 0x3BC, load<u32>(0x101C927C + 4 * (u32)room));
  if (phase == 4) {
    if (!call<s32>(0x025D63E8, a, 0x0233F630u, no == 0x40 ? 0x25000 : 0xCD90))
      return 5;
    return createInit(a);
  }
  return phase;
}
VERIFY(0x0233FDD8, phase2);
s32 create(u32 a) {
  WWHD_FUNC(0x0233FEB0, s32, a);
  return call<s32>(0x02525FE4, a + 0x3AC, 0x101C9348u, a);
}
VERIFY(0x0233FEB0, create);
s32 wrapperCreate(u32 a) {
  WWHD_FUNC(0x0233FEC4, s32, a);
  return create(a);
}
VERIFY(0x0233FEC4, wrapperCreate);
bool remove(u32 a) {
  WWHD_FUNC(0x0233FEC8, bool, a);
  call<void>(0x025204C8, a + 0x3B4, 0x100289BCu);
  s32 room = call<s32>(0x025BD64C, load<u8>(a + 0xA31));
  s32 overrideRoom = load<s32>(0x10028338 + 12 * (u32)load<u8>(a + 0xA31));
  if (overrideRoom >= 0)
    room = overrideRoom;
  call<void>(0x025204C8, a + 0x3BC, load<u32>(0x101C927C + 4 * (u32)room));
  return true;
}
VERIFY(0x0233FEC8, remove);
bool wrapperDelete(u32 a) {
  WWHD_FUNC(0x0233FF40, bool, a);
  return remove(a);
}
VERIFY(0x0233FF40, wrapperDelete);
void setMessage(u32 a, u32 message) {
  WWHD_FUNC(0x0233FF44, void, a, message);
  store<u32>(a + 0x990, message);
  store<u32>(a + 0x988, 0xFFFFFFFF);
}
VERIFY(0x0233FF44, setMessage);
u32 getMsg(u32 a) {
  WWHD_FUNC(0x0233FF54, u32, a);
  u8 no = load<u8>(a + 0xA31);
  store<u32>(a + 0xA20, 0);
  return 0x37DD + no;
}
VERIFY(0x0233FF54, getMsg);
void eventMesSetInit(u32 a, s32 staff) {
  WWHD_FUNC(0x0233FF68, void, a, staff);
  u32 message = call<u32>(0x0254487C, play() + 0x52C4, staff, 0x100289C8u, 3);
  if (message) {
    store<u32>(a + 0xA20, 0);
    u32 value = load<u32>(message);
    if (value != 1) {
      if (value == 0)
        value = getMsg(a);
      setMessage(a, value);
      u32 list = load<u32>(a + 0xA20);
      if (list)
        setMessage(a, load<u32>(list));
    }
  } else {
    u32 list = load<u32>(a + 0xA20) + 4;
    store<u32>(a + 0xA20, list);
    setMessage(a, load<u32>(list));
  }
  store<u16>(a + 0xA2E, 10);
  store<f32>(a + 0xA18, 50.0f);
  store<u16>(a + 0xA2C, 0);
  store<u8>(a + 0xA33, 0);
  store<f32>(a + 0xA1C, 118.0f);
}
VERIFY(0x0233FF68, eventMesSetInit);
void eventOnPlrInit(u32 a) {
  WWHD_FUNC(0x02340040, void, a);
  u32 player = load<u32>(play() + 0x5B34);
  store<u32>(player + 0x3B8, load<u32>(player + 0x3B8) & ~0x08000000u);
  call<void>(0x02515048, call<u32>(0x024F8044));
  call<void>(0x02514F38, call<u32>(0x024F8044));
  call<void>(0x025B8B7C, load<u32>(0x101F84DC) + 0x1178, 0x408);
}
VERIFY(0x02340040, eventOnPlrInit);
void eventOffPlrInit(u32 a) {
  WWHD_FUNC(0x02340094, void, a);
  u32 player = load<u32>(play() + 0x5B34);
  store<u32>(player + 0x3B8, load<u32>(player + 0x3B8) | 0x08000000u);
  call<void>(0x02514F2C, call<u32>(0x024F8044));
  call<void>(0x025B8B68, load<u32>(0x101F84DC) + 0x1178, 0x408);
}
VERIFY(0x02340094, eventOffPlrInit);
u16 next_msgStatus(u32 a, u32 message) {
  WWHD_FUNC(0x023400E0, u16, a, message);
  u32 list = load<u32>(a + 0xA20);
  if (!list)
    return 0x10;
  list += 4;
  store<u32>(a + 0xA20, list);
  u32 value = load<u32>(list);
  if (!value) {
    store<u32>(a + 0xA20, 0);
    return 0x10;
  }
  store<u32>(message, value);
  return 0xF;
}
VERIFY(0x023400E0, next_msgStatus);
u16 talk(u32 a, s32 param) {
  WWHD_FUNC(0x02340128, u16, a, param);
  u32 id = load<u32>(a + 0x988), message = load<u32>(0x101F4B5C);
  u16 status = 0xFF;
  if (id == 0xFFFFFFFF) {
    u32 text;
    if (param == 1) {
      text = getMsg(a);
      store<u32>(a + 0x990, text);
    } else
      text = load<u32>(a + 0x990);
    u32 newId = call<u32>(0x025F7DB0, message, text, a + 0x37C);
    store<u32>(a + 0x988, newId);
    if (newId != 0xFFFFFFFF)
      store<u8>(a + 0x98C, 0);
  } else if (load<u8>(a + 0x98C)) {
    status = (u16)call<u32>(0x025F795C, message);
    if (status == 0xE) {
      u16 next = next_msgStatus(a, a + 0x990);
      call<void>(0x025F74D0, message, next);
      if (call<u32>(0x025F795C, message) == 0xF)
        call<void>(0x025F7DB0, message, load<u32>(a + 0x990), 0);
    } else if (status == 0x12) {
      call<void>(0x025F74D0, message, 0x13);
      store<u32>(a + 0x988, 0xFFFFFFFF);
    }
  } else
    store<u8>(a + 0x98C, 1);
  return status;
}
VERIFY(0x02340128, talk);
bool eventMesSet(u32 a) {
  WWHD_FUNC(0x0234025C, bool, a);
  s16 timer = load<s16>(a + 0xA2E);
  if (timer)
    store<u16>(a + 0xA2E, (u16)(timer - 1));
  else {
    bool input = call<f32>(0x0200796C, 0) != 0.0f;
    if (!input)
      input = call<f32>(0x02007990, 0) != 0.0f;
    if (!input)
      input = call<f32>(0x02007B20, 0) != 0.0f;
    if (input) {
      store<u16>(a + 0xA2C, 0);
      f32 stick = call<f32>(0x0200796C, 0);
      s16 delta = (s16)truncWord((f32)(stick * 512.0f));
      store<u16>(a + 0xA28, (u16)(load<s16>(a + 0xA28) + delta));
      stick = call<f32>(0x02007990, 0);
      delta = (s16)truncWord((f32)(stick * 5.0f));
      f32 height = (f32)(load<f32>(a + 0xA1C) + (f32)delta);
      if (height < 80.0f)
        height = 80.0f;
      else if (height > 150.0f)
        height = 150.0f;
      store<f32>(a + 0xA1C, height);
      stick = call<f32>(0x02007B20, 0);
      f32 twice = (f32)(stick + stick);
      f32 zoom = (f32)(load<f32>(a + 0xA18) - twice);
      if (zoom < 20.0f)
        zoom = 20.0f;
      else if (zoom > 60.0f)
        zoom = 60.0f;
      store<f32>(a + 0xA18, zoom);
    } else {
      s16 wait = load<s16>(a + 0xA2C);
      if (wait)
        store<u16>(a + 0xA2C, (u16)(wait - 1));
    }
  }
  u8 state = load<u8>(a + 0xA33);
  if (state == 0) {
    store<u8>(play() + 0x5BBA, 0x21);
    store<u8>(play() + 0x5BB9, 0x27);
    if (load<s16>(a + 0xA2E) == 0) {
      store<u8>(play() + 0x5BBA, 0x21);
      store<u8>(play() + 0x5BB9, 0x27);
      if (call<s32>(0x020078BC, 0)) {
        s16 angle = load<s16>(a + 0x322);
        store<u16>(a + 0xA26, 0);
        store<u16>(a + 0xA28, 0);
        store<s16>(a + 0xA2A, angle);
        return true;
      }
      if (call<s32>(0x02007898, 0))
        store<u8>(a + 0xA33, 1);
    }
  } else if (state == 1) {
    talk(a, 0);
    if (load<u32>(a + 0x988) != 0xFFFFFFFF)
      store<u8>(a + 0xA33, 2);
  } else if (state == 2) {
    if (talk(a, 0) == 0x12)
      store<u8>(a + 0xA33, 0);
  }
  f32 height = load<f32>(a + 0xA1C);
  s16 angle = load<s16>(a + 0x322);
  f32 cameraHeight = (f32)(height + 30.0f);
  store<f32>(a + 0xA00, 0.0f);
  s16 rotation = load<s16>(a + 0xA28);
  store<f32>(a + 0xA04, cameraHeight);
  store<f32>(a + 0xA08, 0.0f);
  store<f32>(a + 0xA0C, 0.0f);
  store<f32>(a + 0xA10, cameraHeight);
  store<f32>(a + 0xA14, 200.0f);
  call<void>(0x025F1884, 0x1048D0CCu, (s16)(angle + rotation));
  call<void>(0x028E8F64, 0x1048D0CCu, a + 0xA0C, a + 0xA0C);
  call<void>(0x028E8D88, a + 0xA00, a + 0x314, a + 0xA00);
  call<void>(0x028E8D88, a + 0xA0C, a + 0x314, a + 0xA0C);
  f32 tx = load<f32>(a + 0xA00), ex = load<f32>(a + 0xA0C);
  f32 tz = load<f32>(a + 0xA08), ez = load<f32>(a + 0xA14);
  f32 ty = load<f32>(a + 0xA04), ey = load<f32>(a + 0xA10);
  gabi::Local<f32[3]> target, eye;
  store<f32>(target.a, tx);
  store<f32>(target.a + 4, ty);
  store<f32>(target.a + 8, tz);
  store<f32>(eye.a, ex);
  store<f32>(eye.a + 4, ey);
  store<f32>(eye.a + 8, ez);
  u32 camera = call<u32>(0x024F8044);
  call<void>(0x02514F88, camera, target.a, eye.a, load<f32>(a + 0xA18), 0);
  return false;
}
VERIFY(0x0234025C, eventMesSet);
void privateCut(u32 a) {
  WWHD_FUNC(0x02340664, void, a);
  s32 staff = call<s32>(0x02542D88, play() + 0x52C4, 0x100289F4u, 0, 0);
  if (staff == -1)
    return;
  s8 action =
      (s8)call<s32>(0x02542EDC, play() + 0x52C4, staff, 0x101C9354u, 3, 1, 0);
  store<s8>(a + 0xA35, action);
  u32 manager = play() + 0x52C4;
  if (action == -1) {
    call<void>(0x02543280, manager, staff);
    return;
  }
  if (call<s32>(0x025447C8, manager, staff)) {
    action = load<s8>(a + 0xA35);
    if (action == 0)
      eventMesSetInit(a, staff);
    else if (action == 1)
      eventOnPlrInit(a);
    else if (action == 2)
      eventOffPlrInit(a);
  }
  action = load<s8>(a + 0xA35);
  if (action == 0 && !eventMesSet(a))
    return;
  call<void>(0x02543280, play() + 0x52C4, staff);
}
VERIFY(0x02340664, privateCut);
void eventMove(u32 a) {
  WWHD_FUNC(0x023407A0, void, a);
  s16 event = load<s16>(a + 0xA24);
  if (call<s32>(0x025440C8, play() + 0x52C4, event)) {
    u32 p = play();
    store<u16>(p + 0x52B8, load<u16>(p + 0x52B8) | 8);
    return;
  }
  u8 attention = load<u8>(a + 0x9F4);
  if (call<s32>(0x0259F858, a + 0x994)) {
    if (!load<u8>(a + 0x9F4))
      store<u8>(a + 0x9F4, attention);
  } else
    privateCut(a);
}
VERIFY(0x023407A0, eventMove);
bool execute(u32 a) {
  WWHD_FUNC(0x02340858, bool, a);
  if (!load<u8>(play() + 0x5292)) {
    s16 offset = load<s16>(0x101C9254), index = load<s16>(0x101C9256);
    u32 object = a + (u32)(s32)offset;
    if (index < 0)
      call<void>(load<u32>(0x101C9258), object);
    else {
      s16 vptr = load<s16>(0x101C925A);
      u32 vtable = load<u32>(object + (u32)(s32)vptr);
      call<void>(load<u32>(vtable + 8 * (u32)(s32)index + 4), object);
    }
  } else
    eventMove(a);
  if (load<u8>(a + 0xA32) || load<u8>(a + 0xA31) != 0x40) {
    call<void>(0x020182E0, a + 0x840, a + 0x314);
    call<void>(0x020182E0, a + 0x970, a + 0x314);
    call<void>(0x0200E240, play() + 0x26A4, a + 0x728);
    call<void>(0x0200E240, play() + 0x26A4, a + 0x858);
  }
  f32 y = load<f32>(a + 0x318), x = load<f32>(a + 0x314),
      z = load<f32>(a + 0x31C);
  u32 animation = load<u32>(a + 0x4C0);
  store<f32>(a + 0x37C, x);
  store<f32>(a + 0x380, (f32)(y + 150.0f));
  store<f32>(a + 0x390, x);
  store<f32>(a + 0x384, z);
  store<f32>(a + 0x398, z);
  store<f32>(a + 0x394, (f32)(y + 200.0f));
  if (animation)
    call<void>(0x025E742C, animation);
  setMtx(a);
  return true;
}
VERIFY(0x02340858, execute);
bool wrapperExecute(u32 a) {
  WWHD_FUNC(0x023409BC, bool, a);
  return execute(a);
}
VERIFY(0x023409BC, wrapperExecute);
static bool equalText(u32 left, u32 right) {
  if (left == right)
    return true;
  for (u32 i = 0; i < 0x40001; ++i) {
    u8 a = load<u8>(left + i), b = load<u8>(right + i);
    if (a != b)
      return false;
    if (!a)
      return true;
  }
  return false;
}
static bool rootNameMatches(u32 strings, u32 name, u32 incoming) {
  store<u32>(strings + incoming, name);
  store<u32>(strings + incoming + 4, 0x100282AC);
  u32 target = load<u32>(load<u32>(strings + 4) + 0x14);
  call<void>(target, strings, 0);
  target = load<u32>(load<u32>(strings + 4) + 0x14);
  call<void>(target, strings);
  target = load<u32>(load<u32>(strings + incoming + 4) + 0x14);
  u32 left = load<u32>(strings);
  call<void>(target, strings + incoming);
  if (equalText(left, load<u32>(strings + incoming)))
    return true;
  store<u32>(strings + incoming + 0x10, name);
  store<u32>(strings + incoming + 0x14, 0x100282AC);
  target = load<u32>(load<u32>(strings + 12) + 0x14);
  call<void>(target, strings + 8, 1);
  target = load<u32>(load<u32>(strings + 12) + 0x14);
  call<void>(target, strings + 8);
  target = load<u32>(load<u32>(strings + incoming + 0x14) + 0x14);
  left = load<u32>(strings + 8);
  call<void>(target, strings + incoming + 0x10);
  return equalText(left, load<u32>(strings + incoming + 0x10));
}
bool draw(u32 a) {
  WWHD_FUNC(0x023409C0, bool, a);
  u32 env = call<u32>(0x02555D0C);
  call<void>(0x025626A4, env, 3, a + 0x314, a + 0x4C4);
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(a + 0x3C4), a + 0x4C4);
  env = call<u32>(0x02555D0C);
  call<void>(0x025626A4, env, 1, a + 0x314, a + 0x110);
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(a + 0x3C8), a + 0x110);
  u32 model = load<u32>(a + 0x3C8);
  u8 display = load<u8>(a + 0xA32);
  u32 data = load<u32>(model + 0xAC);
  if (!display) {
    if (load<u8>(a + 0xA31) != 0x40) {
      call<void>(0x025E7B3C, a + 0x3D0, data, 0);
      call<void>(0x025E2DE0, load<u32>(a + 0x3C8), 0);
      store<u32>(data + 0x38, 0);
    }
    return true;
  }
  u32 anim = load<u32>(a + 0x4C0);
  if (anim) {
    model = load<u32>(a + 0x3C4);
    f32 frame = load<f32>(anim + 4);
    call<void>(0x025E83FC, anim, load<u32>(model + 0xAC), frame);
  }
  call<void>(0x025E7B3C, a + 0x3D0, data, 1);
  call<void>(0x025E2DE0, load<u32>(a + 0x3C8), 0);
  call<void>(0x025E2DE0, load<u32>(a + 0x3C4), 0);
  store<u32>(data + 0x38, 0);
  if (load<u32>(a + 0x4C0))
    store<u32>(load<u32>(load<u32>(a + 0x3C4) + 0xAC) + 0x48, 0);
  u32 morf = load<u32>(a + 0x4BC);
  if (!morf)
    return true;
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(morf + 0x90), a + 0x4C4);
  morf = load<u32>(a + 0x4BC);
  call<void>(0x025E55A0, morf);
  model = load<u32>(morf + 0x90);
  u32 flag = load<u32>(0x104698FC);
  data = load<u32>(model + 0xAC);
  store<u32>(0x104B462C, model);
  if (!flag) {
    store<u32>(0x104698FC, 1);
    call<void>(0x027F1278, 0x10469920u);
    store<u32>(0x1046992C, 0x10058D10);
    call<void>(0x028F026C, 0x101C92B0u);
  }
  if (!load<u32>(0x10469900)) {
    store<u32>(0x10469900, 1);
    call<void>(0x027F1278, 0x104699B8u);
    store<u32>(0x104699C4, 0x10058D10);
    call<void>(0x028F026C, 0x101C92BCu);
  }
  if (!load<u32>(0x10469904)) {
    store<u32>(0x10469904, 1);
    call<void>(0x027F1278, 0x10469A50u);
    store<u32>(0x10469A5C, 0x10058D40);
    call<void>(0x028F026C, 0x101C92C8u);
  }
  if (!load<u32>(0x10469908)) {
    store<u32>(0x10469908, 1);
    call<void>(0x027F1278, 0x10469AE8u);
    store<u32>(0x10469AF4, 0x10058D40);
    call<void>(0x028F026C, 0x101C92D4u);
  }
  u32 count = load<u32>(data + 4), joints = load<u32>(data + 8);
  u32 root = joints, eye = count > 0x13 ? joints + 0x214 : joints,
      eyebrow = count > 0x15 ? joints + 0x24C : joints;
  u32 material = load<u32>((count > 0x13 ? joints + 0x214 : joints) + 0x10);
  // Keep the three adjacent guest shape lists and SafeStrings in their HD frame slots.
  gabi::Local<u32[44]> guestFrame;
  store<u32>(guestFrame.a, morf);
  u32 shapes = guestFrame.a + 0x34;
  for (u32 i = 0; i < 12; ++i)
    store<u32>(shapes + 4 * i, 0);
  u32 blends = 0, solids = 0, depths = 0;
  for (int pass = 0; pass < 2; ++pass) {
    while (material) {
      u32 block = load<u32>(material + 0x20);
      u32 mode = call<u32>(load<u32>(load<u32>(block) + 0x44), block);
      if (!load<u8>(mode)) {
        block = load<u32>(material + 0x20);
        mode = call<u32>(load<u32>(load<u32>(block) + 0x2C), block);
        u8 blend = load<u8>(mode);
        u32 shape = load<u32>(material + 8);
        if (blend == 1) {
          store<u32>(shapes + 4 * blends, shape);
          ++blends;
          if ((s32)blends > 4)
            call<void>(0x0273AA24, 0x100282F8u, 0x748, 0x100282E4u);
        } else {
          store<u32>(shapes + 16 + 4 * solids, shape);
          ++solids;
          if ((s32)solids > 4)
            call<void>(0x0273AA24, 0x100282F8u, 0x74B, 0x1002830Cu);
        }
      } else {
        store<u32>(shapes + 32 + 4 * depths, load<u32>(material + 8));
        ++depths;
        if ((s32)depths > 4)
          call<void>(0x0273AA24, 0x100282F8u, 0x74F, 0x10028320u);
      }
      material = load<u32>(material + 4);
    }
    count = load<u32>(data + 4);
    joints = load<u32>(data + 8);
    material = load<u32>((count > 0x15 ? joints + 0x24C : joints) + 0x10);
  }
  store<u32>(0x104B4634, load<u32>(play() + 0x5D54));
  u32 buffer = load<u32>(play() + 0x5D54);
  u32 opaque = load<u32>(0x104B4634);
  store<u32>(0x104B4638, buffer);
  call<void>(0x027F0E04, opaque, 0x10469AE8u, 0);
  for (u32 i = 0; i < 4; ++i) {
    u32 shape = load<u32>(shapes + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
    shape = load<u32>(shapes + 32 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
    shape = load<u32>(shapes + 16 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 1);
  }
  call<void>(0x027F583C, model, eye);
  call<void>(0x027F583C, model, eyebrow);
  call<void>(0x027F0E04, load<u32>(0x104B4634), 0x104699B8u, 0);
  for (u32 i = 0; i < 4; ++i) {
    u32 shape = load<u32>(shapes + 4 * i);
    if (shape)
      store<u8>(shape + 4, 1);
    shape = load<u32>(shapes + 16 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
  }
  call<void>(0x027F583C, model, eye);
  call<void>(0x027F583C, model, eyebrow);
  material = load<u32>(root + 0x10);
  u32 strings = guestFrame.a + 4;
  store<u32>(strings, 0x1002829C);
  store<u32>(strings + 4, 0x100282AC);
  store<u32>(strings + 8, 0x100282A4);
  store<u32>(strings + 12, 0x100282AC);
  while (material) {
    u32 record = load<u32>(material), offset = load<u32>(record + 4);
    u32 name = offset ? record + 4 + offset : 0;
    if (!rootNameMatches(strings, name, 16))
      store<u8>(load<u32>(material + 8) + 4, 0);
    material = load<u32>(material + 4);
  }
  call<void>(0x027F583C, model, root);
  material = load<u32>(root + 0x10);
  while (material) {
    u32 record = load<u32>(material), offset = load<u32>(record + 4);
    u32 name = offset ? record + 4 + offset : 0;
    bool match = rootNameMatches(strings, name, 24);
    store<u8>(load<u32>(material + 8) + 4, match ? 0 : 1);
    material = load<u32>(material + 4);
  }
  store<u32>(0x104B4634, load<u32>(play() + 0x5D5C));
  buffer = load<u32>(play() + 0x5D5C);
  opaque = load<u32>(0x104B4634);
  store<u32>(0x104B4638, buffer);
  call<void>(0x027F0E04, opaque, 0x10469A50u, 0);
  for (u32 i = 0; i < 4; ++i) {
    u32 shape = load<u32>(shapes + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
    shape = load<u32>(shapes + 32 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 1);
    shape = load<u32>(shapes + 16 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
  }
  call<void>(0x027F583C, model, eye);
  call<void>(0x027F583C, model, eyebrow);
  call<void>(0x027F0E04, load<u32>(0x104B4634), 0x10469920u, 0);
  for (u32 i = 0; i < 4; ++i) {
    u32 shape = load<u32>(shapes + 32 + 4 * i);
    if (shape)
      store<u8>(shape + 4, 0);
  }
  store<u32>(0x104B4634, load<u32>(play() + 0x5D78));
  store<u32>(0x104B4638, load<u32>(play() + 0x5D7C));
  call<void>(0x025E2E5C, load<u32>(load<u32>(guestFrame.a) + 0x90));
  material = load<u32>(root + 0x10);
  while (material) {
    store<u8>(load<u32>(material + 8) + 4, 1);
    material = load<u32>(material + 4);
  }
  count = load<u32>(data + 4);
  joints = load<u32>(data + 8);
  store<u8>(
      load<u32>(load<u32>((count > 0x14 ? joints + 0x230 : joints) + 0x10) +
                8) +
          4,
      1);
  count = load<u32>(data + 4);
  joints = load<u32>(data + 8);
  store<u8>(
      load<u32>(load<u32>((count > 0x29 ? joints + 0x47C : joints) + 0x10) +
                8) +
          4,
      1);
  return true;
}
VERIFY(0x023409C0, draw);
bool wrapperDraw(u32 a) {
  WWHD_FUNC(0x0234135C, bool, a);
  return draw(a);
}
VERIFY(0x0234135C, wrapperDraw);
void executeNormal(u32 a) {
  WWHD_FUNC(0x02341360, void, a);
  if (!load<u8>(a + 0xA32))
    return;
  u32 p = play();
  f32 x = load<f32>(a + 0x314);
  u32 player = load<u32>(p + 0x5B34);
  f32 y = load<f32>(a + 0x318), z = load<f32>(a + 0x31C);
  gabi::Local<f32[3]> source, target;
  store<f32>(source.a, x);
  store<f32>(source.a + 4, y);
  store<f32>(source.a + 8, z);
  x = load<f32>(player + 0x314);
  y = load<f32>(player + 0x318);
  z = load<f32>(player + 0x31C);
  store<f32>(target.a, x);
  store<f32>(target.a + 4, y);
  store<f32>(target.a + 8, z);
  gabi::Local<f32> distance;
  gabi::Local<s16> angle;
  call<void>(0x0259D624, source.a, target.a, distance.a, angle.a);
  s16 facing = load<s16>(a + 0x322), heading = load<s16>(angle.a);
  s16 delta = (s16)(heading - facing);
  f32 length = load<f32>(distance.a);
  if (150.0f > length) {
    s32 magnitude = delta < 0 ? -(s32)delta : (s32)delta;
    if (magnitude < 13000)
      store<u16>(a + 0xFA, load<u16>(a + 0xFA) | 1);
  }
}
VERIFY(0x02341360, executeNormal);
void staticInit() {
  WWHD_FUNC(0x02341424, void);
  store<u32>(0x10469918, 0);
  store<u32>(0x10469910, 0);
  store<u32>(0x1046991C, 0);
  store<u32>(0x10469914, 0);
  call<void>(0x028F026C, 0x101C9360u);
  store<f32>(0x104698F4, -3.1415927410125732f);
  store<f32>(0x104698F8, 3.1415927410125732f);
  call<void>(0x028ED6F8, 0x1046990Cu);
  call<void>(0x028F026C, 0x101C936Cu);
  call<void>(0x028EAB2C, 0x1046990Du);
  call<void>(0x028F026C, 0x101C9378u);
}
VERIFY(0x02341424, staticInit);
void staticDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x023414B8, void, a, flags);
  if (a && (flags & 1))
    call<void>(0x0273AF40, a);
}
VERIFY(0x023414B8, staticDtor);
bool isDelete(u32 a) {
  WWHD_FUNC(0x023414CC, bool, a);
  return true;
}
VERIFY(0x023414CC, isDelete);
void packetDtor1(u32 a, s32 flags) {
  WWHD_FUNC(0x023414D4, void, a, flags);
  if (a) {
    call<void>(0x027F13DC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x023414D4, packetDtor1);
void packetDtor2(u32 a, s32 flags) {
  WWHD_FUNC(0x02341528, void, a, flags);
  if (a) {
    call<void>(0x027F13DC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x02341528, packetDtor2);
void actorDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x0234157C, void, a, flags);
  if (a) {
    call<void>(0x02515A70, a + 0x858, 2);
    call<void>(0x02515A70, a + 0x728, 2);
    call<void>(0x02515860, a + 0x6EC, 2);
    call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x0234157C, actorDtor);
void empty(u32 a) { WWHD_FUNC(0x023415F4, void, a); }
VERIFY(0x023415F4, empty);
} // namespace figure
