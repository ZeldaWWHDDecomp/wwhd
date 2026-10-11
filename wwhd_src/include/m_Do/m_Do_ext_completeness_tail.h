/* m_Do_ext HD SDK tail and packed animation blending. */
#pragma once
#include "wwhd.h"
namespace mDoExtTail {
// Host-only scalar interpolation pair; never a guest stack object.
struct TrigPair {
  f32 first, second;
};
enum DeleteFlags : u32 { DELETE_MEMORY = 1 };
enum BlendFlags : u32 {
  BUFFER_DIRTY = 8,
  SKIP_JOINT = 0x80000000,
  JOINT_INDEX_MASK = 0x7FFF
};
enum BaseDestructor : u32 {
  MTX_CALC_BASE = 0x027F3628,
  PACKET_BASE = 0x027F13DC
};
struct Word {
  be<u32> value;
};
struct Vector {
  be<f32> value[3];
};
struct QuatView {
  union {
    be<f32> value[4];
    be<u32> words[4];
  };
};
struct Matrix34 {
  be<f32> value[3][4];
};
struct LineArray {
  be<s32> mCount;
  be<u32> mpEntries;
};
struct LineGroup {
  be<u32> mState;
  LineArray mArrays[2];
};
struct LineManager {
  be<u32> mState;
  be<s32> mCount;
  be<u32> mpGroups;
};
struct LineEntry {
  u8 pad0[0xF0];
  be<u32> mpVtable;
};
struct Packet {
  u8 pad0[0xC];
  be<u32> mpVtable;
};
struct PacketVtable {
  u8 pad0[0x24];
  be<u32> mDraw;
};
struct Heap {
  u8 pad0[0xC];
  be<u32> mpVtable;
};
struct HeapVtable {
  u8 pad0[0x3C];
  be<u32> mFree;
};
struct EntryVtable {
  u8 pad0[0xC];
  be<u32> mDestroy;
};
struct LineElement {
  be<u32> mState;
  be<u32> mValue, mNext;
};
struct LineMember {
  be<u32> mValue, mNext;
};
// HD: SDK source records contain quaternion/Euler channels. Destination records
// hold either a quaternion or two basis vectors in the same six-float slot.
struct SourceRecord {
  be<u32> mFlags;
  Vector mScale, mTranslate;
  be<f32> mWeight;
  QuatView mOrientation;
};
struct BlendRecord {
  be<u32> mFlags;
  Vector mScale, mTranslate;
  be<f32> mWeight;
  union {
    QuatView mQuat;
    be<f32> mBasis[6];
  };
};
struct Animation {
  u8 pad0[0x2C];
  be<u32> mpRecords;
  u8 pad30[8];
  be<u32> mpJointIndices;
  u8 pad3C[4];
  be<u16> mNum;
  u8 pad42[0xE];
  be<u32> mFlags;
};
struct BlendBuffer {
  be<u32> mpRecords;
  u8 pad4[4];
  be<u32> mFlags;
};
struct TrigEntry {
  be<f32> mSin, mCos, mSinSlope, mCosSlope;
};
struct Constants {
  be<f32> mQuarter, mOne, mHalf, mZero, mThree;
  u8 pad14[0x2DC];
  be<f32> mAngleScale, mFraction16, mFraction24, mUnsignedBoundary,
      mDotThreshold;
  u8 pad304[4];
  be<f64> mPositive, mNegative;
  u8 pad318[8];
  be<f32> mFour, mThreeHalves;
};
WWHD_SIZE(SourceRecord, 48);
WWHD_SIZE(BlendRecord, 56);
WWHD_SIZE(LineGroup, 20);
WWHD_OFFSET(Constants, mAngleScale, 0x2F0);
WWHD_OFFSET(Constants, mPositive, 0x308);
WWHD_OFFSET(Constants, mThreeHalves, 0x324);
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline Constants *constants() { return view<Constants>(0x100586D0); }
inline TrigEntry *trigTable() { return view<TrigEntry>(0x1016C7E0); }
inline u32 rootHeap() { return view<Word>(0x101F8B4C)->value; }
inline u32 findFromRoot(void *heap, void *block) {
  return gabi::call<u32>(0x02755FEC, heap, block);
}
inline u32 allocate(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
inline void deleteObject(void *object) { gabi::call(0x0273AF40, object); }
inline void truncateUnsigned64(f32 value) { gabi::call(0x028F5680, value); }
inline f64 arcCos(f64 value) { return gabi::call<f64>(0x028F4FAC, value); }
} // namespace mDoExtTail
