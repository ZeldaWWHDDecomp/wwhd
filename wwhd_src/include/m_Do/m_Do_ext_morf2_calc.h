/* HD joint calculation views. SDK buffers and callback contexts differ from J3D. */
#pragma once
#include "wwhd.h"
namespace mDoExtCalc {
enum RecordFlags : u32 {
  SCALE_DIRTY = 0x03000000,
  ROTATION_DIRTY = 0x04000000,
  TRANSLATION_DIRTY = 0x08000000,
  BUFFER_DIRTY = 8
};
struct Word {
  be<u32> value;
};
struct Animation {
  be<f32> mFrame;
};
struct Matrix {
  u8 pad0[0x20];
  be<f32> mRotation[3];
};
struct Vector {
  union {
    be<f32> value[3];
    be<u32> words[3];
  };
};
struct Transform {
  Vector mScale;
  be<s16> mRotation[3];
  u8 pad12[2];
  Vector mTranslate;
};
struct QuatView {
  be<f32> value[4];
};
struct OldBlendScratch {
  u8 pad0[8];
  Vector mScale;
  u8 pad14[8];
  Vector mTranslate;
  QuatView mQuat;
  u8 pad38[0x18];
  QuatView mConvertedQuat;
  be<f32> mBasis[12];
  u8 pad90[12];
  be<f32> mMatrix[12];
};
struct OldStoreScratch {
  u8 pad0[0x20];
  QuatView mQuat;
  be<f32> mBasis[12];
  u8 pad60[12];
  be<f32> mMatrix[12];
};
struct Morf2Scratch {
  u8 pad0[0xC0];
  be<f32> mWeights[2], mMatrix[12];
};
struct BasisMatrix {
  be<f32> row[3][4];
};
struct Record {
  be<u32> mFlags;
  Vector mScale, mTranslate;
  be<f32> mWeight;
  Vector mBasisX, mBasisY;
};
struct Buffer {
  be<u32> mpRecords;
  u8 pad4[2];
  be<u16> mNum;
  be<u32> mFlags;
  u8 padC[8];
};
struct Control {
  be<f32> mValue, mSecond, mThird;
  u8 padC[4];
  be<u32> mCallback, mReceiver;
};
struct Base {
  be<u32> mpVtable, mpControl;
  u8 pad8[0x28];
  be<u32> mpMatrices;
  u8 pad34[0x20];
  be<u32> mFlags;
  u8 pad58[0x20];
  Buffer mBuffer;
};
struct Morf : Base {
  be<u32> mpBufferMemory, mpModel, mpAnm;
  u8 mFrameCtrl[0x10];
  be<u32> mpTransformInfo, mpQuat;
  be<f32> mCurMorf, mPrevMorf, mMorfStep;
  be<u32> mpSound, mpCallback1, mpCallback2;
};
struct Morf2 : Base {
  be<u32> mpBufferMemory, mpModel, mpAnm1, mpAnm2, mpTransformInfo, mpQuat;
  u8 mFrameCtrl[0x10];
  be<f32> mCurMorf, mPrevMorf, mMorfStep, mAnmRate;
  be<u32> mpSound, mpCallback1, mpCallback2;
  be<u8> mHDEnabled;
  u8 padD1[3];
};
struct OldBlend {
  u8 pad0[0x78];
  be<s32> mCount;
  be<u32> mpEntries, mpModel, mpBuffers, mpMemory;
  Buffer mBuffer;
  be<u32> mpOldBufferMemory, mpOldRecords, mUserArea, mpOldFrame, mBefore,
      mAfter;
};
struct OldFrame {
  be<u8> mValid;
  u8 pad1[3];
  be<f32> mCounter, mInvCounter, mMorf;
  u8 pad10[8];
  be<u16> mFirstJoint, mLastJoint;
  be<u32> mpTransformInfo, mpQuat;
};
struct ModelContext {
  u8 pad0[0x2C];
  be<u32> mpReference;
};
struct Reference {
  be<u32> mpJoints;
  u8 pad4[0x28];
  be<u16> mNum, mIndex;
  be<u32> mpMtxCalc;
};
struct Joints {
  u8 pad0[8];
  be<u16> mNum;
};
struct Model {
  u8 pad0[0xC];
  be<u32> mpVtable;
  u8 pad10[0x9C];
  be<u32> mpData;
};
struct ModelData {
  u8 pad0[4];
  be<u32> mNum, mpJoints;
};
struct Callback {
  be<u32> mpVtable;
};
struct Vtable {
  u8 pad0[0xC];
  be<u32> mExecute;
};
struct BlendEntry {
  u8 pad0[4];
  be<u32> mEnabled;
  u8 pad8[4];
  be<u32> mpData;
};
struct Iterator {
  be<u32> mpNode, mNext;
};
struct Node {
  be<u32> mIndex;
};
WWHD_SIZE(Transform, 0x20);
WWHD_SIZE(Record, 0x38);
WWHD_SIZE(Buffer, 0x14);
WWHD_OFFSET(Base, mBuffer, 0x78);
WWHD_OFFSET(Morf, mCurMorf, 0xB0);
WWHD_OFFSET(Morf2, mCurMorf, 0xB4);
WWHD_OFFSET(OldBlend, mpOldFrame, 0xAC);
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
// HD SDK constant table; guest reads remain live across callbacks.
struct Constants {
  be<f32> mOne, mHalf, mZero;
  u8 padC[0x314];
  be<f32> mThree, mEpsilon;
};
inline Constants *constants() { return view<Constants>(0x100586D4); }
inline be<u32> &currentMtxCalc() { return view<Word>(0x104B4628)->value; }
// HD: named scratch views retain matrix/quaternion overlap and caller linkage offsets.
inline void clearBuffer(void *buffer) { gabi::call(0x027E0710, buffer); }
inline void blendBuffer(void *buffer, void *control, f32 weight) {
  gabi::call(0x027E0738, buffer, control, weight);
}
inline void entryBuffer(void *buffer, void *reference) {
  gabi::call(0x027E0834, buffer, reference);
}
inline void bindAnimation(void *calc, void *animation) {
  gabi::call(0x027F363C, calc, animation);
}
inline u32 getTransformInfo(void *joint) {
  return gabi::call<u32>(0x027F3CD0, joint);
}
inline s32 radianToAngle(f32 angle) {
  return gabi::call<s32>(0x02019510, angle);
}
inline void JMAEulerToQuat(s32 x, s32 y, s32 z, void *quat) {
  gabi::call(0x027ED2D0, x, y, z, quat);
}
inline void JMAQuatLerp(void *oldQuat, void *newQuat, void *out, f32 ratio) {
  gabi::call(0x027ED3AC, oldQuat, newQuat, out, ratio);
}
inline void mDoMtx_matrixToQuat(void *quat, void *matrix) {
  gabi::call(0x025EE5F0, quat, matrix);
}
inline void stopZelAnime(void *sound) { gabi::call(0x02801D3C, sound); }
inline void copyRecord(void *to, void *from, u32 size) {
  gabi::call(0xC000A848, to, from, size);
}
inline void blendAnimationEntry(void *buffer, void *entry, void *data) {
  gabi::call(0x025E3DEC, buffer, entry, data);
}
inline void calcMorfJoints(void *self, void *model) {
  gabi::call(0x025E564C, self, model);
}
inline void entryMorfJoints(void *self, void *model) {
  gabi::call(0x025E5C2C, self, model);
}
inline void calcMorf2Joints(void *self, void *model) {
  gabi::call(0x025E67A4, self, model);
}
inline void entryMorf2Joints(void *self, void *model) {
  gabi::call(0x025E7270, self, model);
}
inline void blendOldJoints(void *self, void *model) {
  gabi::call(0x025E410C, self, model);
}
inline void storeOldJoints(void *self, void *model) {
  gabi::call(0x025E46B4, self, model);
}
} // namespace mDoExtCalc
