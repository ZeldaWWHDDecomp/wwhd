#pragma once
#include "d/actor/d_a_ship.h"
namespace ShipTail {
// HD: rendering packet/state layouts have no GameCube counterpart.
struct DrawInfo {
  u8 prefix[0xc];
  be<u32> pass;
};
struct DrawState {
  u8 prefix[8];
  be<u32> state0, state1;
  u8 pipeline[0xec - 0x10];
  be<u32> flags;
  u8 suffix[0x11c - 0xf0];
};
WWHD_SIZE(DrawState, 0x11c);
struct ShapePacket {
  u8 packet[0xac];
};
struct Segment {
  u8 packet[0xc];
  be<u32> vtable;
  u8 material[0x74 - 0x10];
  Mtx34 matrix;
  be<u32> flags;
};
WWHD_SIZE(Segment, 0xa8);
struct SightModel {
  u8 instance[0x128];
  be<u32> numShapePackets, shapePackets;
  u8 drawState[4];
  be<u32> shapeIndex;
  ShapePacket *currentShape() {
    u32 index = *gabi::at<be<u16>>(shapeIndex);
    u32 packet = shapePackets;
    if (index < numShapePackets)
      packet += index * sizeof(ShapePacket);
    return gabi::at<ShapePacket>(packet);
  }
};
struct SightPacket {
  u8 packet[0xc];
  be<u32> vtable;
  u8 material[0x98 - 0x10];
  be<u32> model, modelPacket, segments;
};
WWHD_SIZE(SightPacket, 0xa4);
// HD: varargs use the original guest frame and register-save area, not host varargs.
struct FormatFrame {
  be<u8> gpr, fpr;
  u8 pad[2];
  be<u32> overflow, registerSave;
  u8 reserved[4];
  be<u32> registers[8];
};
WWHD_SIZE(FormatFrame, 0x30);
struct FixedString {
  be<u32> text, vtable, size;
  be<u8> buffer[32];
};
WWHD_SIZE(FixedString, 0x2c);
struct WordVector {
  be<u32> x, y, z;
};
struct FollowCallback {
  be<u32> vtable;
  void endVirtual() { gabi::call(*gabi::at<be<u32>>(vtable + 0x44), this); }
};
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline DrawState *defaultDrawState() { return view<DrawState>(0x104b474c); }
inline cXyz *ropeBaseVec() { return view<cXyz>(0x1046dce0); }
inline void initShipHeaderStatics() {
  sinit_header_statics(0x1046dcc4, 0x101d0484);
}
inline void initStaticHeaderStatics() {
  sinit_header_statics(0x1046ddec, 0x101d04d8);
}
enum GuestAddress : u32 {
  FixedStringVtable = 0x1003a204,
  BufferedStringVtable = 0x1003a21c,
  FormatStringVtable = 0x1003a2fc,
  SegmentVtable = 0x1016ef84,
  CylinderBoxVtable = 0x1003a24c,
  SightPacketVtable = 0x1003a7f8,
  AcchGroupVtable = 0x1003a28c,
  AcchPolyVtable = 0x1003a29c,
  SegmentDestructor = 0x02483014,
  AcchCirDestructor = 0x02482fc0,
  SphereDestructor = 0x02515ae8,
  StatusDestructor = 0x02515860,
  CylinderDestructor = 0x02515a70
};
namespace sdk {
/* 02750250 [v d_a_ship] */
template <class R = void, class... A> inline R constructDrawState(A... args) {
  return gabi::call<R>(0x02750250, args...);
}
/* 02750370 [v d_a_ship] */
template <class R = void, class... A> inline R applyDrawState(A... args) {
  return gabi::call<R>(0x02750370, args...);
}
/* 027F1FA8 [v d_a_ship] */
template <class R = void, class... A> inline R drawShape(A... args) {
  return gabi::call<R>(0x027F1FA8, args...);
}
/* 027F26C4 [v d_a_ship] */
template <class R = void, class... A> inline R drawSightSegment(A... args) {
  return gabi::call<R>(0x027F26C4, args...);
}
/* 02759C10 [v d_a_ship] */
template <class R = void, class... A> inline R formatV(A... args) {
  return gabi::call<R>(0x02759C10, args...);
}
/* 027FB40C [v d_a_ship] */
template <class R = void, class... A> inline R constructMatPacket(A... args) {
  return gabi::call<R>(0x027FB40C, args...);
}
/* 028F521C [v d_a_ship] */
template <class R = void, class... A> inline R clearMemory(A... args) {
  return gabi::call<R>(0x028F521C, args...);
}
/* 02018034 [v d_a_ship] */
template <class R = void, class... A> inline R destroyCircle(A... args) {
  return gabi::call<R>(0x02018034, args...);
}
/* 027FB528 [v d_a_ship] */
template <class R = void, class... A> inline R destroyMatPacket(A... args) {
  return gabi::call<R>(0x027FB528, args...);
}
/* 027FD764 [v d_a_ship] */
template <class R = void, class... A> inline R destroyModelPacket(A... args) {
  return gabi::call<R>(0x027FD764, args...);
}
/* 028F0164 [v d_a_ship] */
template <class R = void, class... A> inline R destroyArray(A... args) {
  return gabi::call<R>(0x028F0164, args...);
}
/* 027F13DC [v d_a_ship] */
template <class R = void, class... A> inline R destroyPacket(A... args) {
  return gabi::call<R>(0x027F13DC, args...);
}
/* 02515AE8 [v d_a_ship] */
template <class R = void, class... A> inline R destroySphere(A... args) {
  return gabi::call<R>(0x02515AE8, args...);
}
/* 02515860 [v d_a_ship] */
template <class R = void, class... A> inline R destroyStatus(A... args) {
  return gabi::call<R>(0x02515860, args...);
}
/* 024EFD9C [v d_a_ship] */
template <class R = void, class... A> inline R destroyAcch(A... args) {
  return gabi::call<R>(0x024EFD9C, args...);
}
/* 025EB8B8 [v d_a_ship] */
template <class R = void, class... A> inline R destroyRopeLine(A... args) {
  return gabi::call<R>(0x025EB8B8, args...);
}
/* 025D50BC [v d_a_ship] */
template <class R = void, class... A> inline R destroyActor(A... args) {
  return gabi::call<R>(0x025D50BC, args...);
}
/* 025A92C0 [v d_a_ship] */
template <class R = void, class... A> inline R removeWave(A... args) {
  return gabi::call<R>(0x025A92C0, args...);
}
/* 025A99B8 [v d_a_ship] */
template <class R = void, class... A> inline R removeSplash(A... args) {
  return gabi::call<R>(0x025A99B8, args...);
}
/* 025A9E38 [v d_a_ship] */
template <class R = void, class... A> inline R removeTrack(A... args) {
  return gabi::call<R>(0x025A9E38, args...);
}
/* 025A9270 [v d_a_ship] */
template <class R = void, class... A> inline R endRipple(A... args) {
  return gabi::call<R>(0x025A9270, args...);
}
/* 025A5AC8 [v d_a_ship] */
template <class R = void, class... A> inline R endFollow(A... args) {
  return gabi::call<R>(0x025A5AC8, args...);
}
} // namespace sdk
} // namespace ShipTail
