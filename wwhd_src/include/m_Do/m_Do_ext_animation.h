#pragma once
#include "wwhd.h"
namespace mDoExtAnimation {
struct Address {
  u8 opaque;
};
struct FrameRef {
  be<u32> mpFrame, mpVtable;
};
struct FrameOwner {
  u8 padding0[0x10];
  FrameRef mFrameRef;
};
struct mDoExt_MtxCalcOldFrame {
  void decOldFrameMorfCounter();
  void initOldFrameMorf(f32 duration, u32 start, u32 end);
  be<u8> mOldFrameFlg, field_0x1;
  u8 padding2[2];
  be<f32> mOldFrameMorfCounter, field_0x8, mOldFrameRate, field_0x10,
      field_0x14;
  be<u16> mOldFrameStartJoint, mOldFrameEndJoint;
  be<u32> mOldFrameTransInfo, mOldFrameQuaternion;
};
// HD: additional animation-buffer state extends the tww blend calculator.
struct OldBuffer {
  u8 padding0[0x10];
  be<u32> mState;
};
struct BlendCalc {
  be<u32> mpVtable;
  u8 padding4[4];
  be<u32> mFlags;
  u8 paddingC[0x2C];
  be<u32> mpSecondaryVtable;
  u8 padding3C[0xC];
  be<u32> mBaseState;
  u8 padding4C[0x20];
  be<u32> mpJoints, mJointState, mCalcState;
  be<s32> mNum;
  be<u32> mAnmRatio, mpModel, mpBuffers, mpBufferMemory;
  OldBuffer mOldBuffer;
  be<u32> mpOldBufferMemory, mpOldTransformInfo, mUserArea, mOldFrame,
      mBeforeCallback, mAfterCallback;
};
struct AnmRatio {
  be<f32> mRatio;
  be<u32> mpAnmTransform;
  be<s32> mValueCount;
  be<u32> mpValues;
};
struct Animation {
  be<f32> mFrame;
  u8 padding4[4];
  be<u32> mpData;
};
struct AnimationData {
  u8 padding0[12];
  be<u32> mKind;
  u8 padding10[0x14];
  be<u32> mJointDataOffset;
};
struct AnimationControl {
  be<f32> mValue, mSecond, mThird;
  u8 paddingC[4];
  be<u32> mCallback, mReceiver;
};
struct AnimationBuffer {
  be<u32> mpControl;
  u8 padding4[0x30];
  be<u32> mpVtable;
  u8 padding38[12];
  be<u32> mpData;
  u8 padding48[0x20];
};
struct BufferVtable {
  u8 padding0[0x1C];
  be<u32> mUpdate;
};
struct Model {
  u8 padding0[0xAC];
  be<u32> mpModelData;
};
struct JointData {
  u8 padding0[8];
  be<u16> mNum;
};
struct BufferDescriptor {
  be<u32> mCount, mFirst, mValueBytes;
  be<u8> mEnabled, mBound;
  u8 paddingE[2];
  be<u32> mWorkSize, mWorkCount;
  u8 scratch[24];
};
struct OldBufferDescriptor {
  be<u32> mCount, mFirst, mValueBytes;
  u8 scratch[8];
};
struct DispatchEntry {
  be<s16> mThisAdjust, mSlot;
  be<u32> mTarget;
};
// HD: a virtual dispatch record aliases its target word with a signed vtable offset.
struct VirtualDispatchEntry {
  be<s16> mThisAdjust, mSlot;
  u8 padding4[2];
  be<s16> mVtableOffset;
};
WWHD_SIZE(mDoExt_MtxCalcOldFrame, 0x24);
WWHD_SIZE(BlendCalc, 0xB8);
WWHD_OFFSET(BlendCalc, mAnmRatio, 0x7C);
WWHD_OFFSET(BlendCalc, mOldFrame, 0xAC);
WWHD_SIZE(AnmRatio, 16);
WWHD_SIZE(AnimationBuffer, 0x68);
WWHD_SIZE(BufferDescriptor, 0x30);
WWHD_SIZE(OldBufferDescriptor, 0x14);
WWHD_SIZE(DispatchEntry, 8);
} // namespace mDoExtAnimation
template <class T> inline T *mDoExtAnimation_at(u32 address) {
  return gabi::at<T>(address);
}
void mDoExt_animationAssert(void *, u32, void *);
u32 mDoExt_animationAlloc(u32);
u32 mDoExt_animationModelConstruct(void *);
u32 mDoExt_animationEntryModelData(void *, void *, u32, u32);
void mDoExt_animationFree(void *);
void mDoExt_animationFrameBaseDestroy(void *, u32);
u32 mDoExt_animationAllocAligned(u32, u32);
void mDoExt_animationBufferInit(void *, u32);
void mDoExt_animationBufferSetData(void *, void *);
void mDoExt_animationBufferBindDirect(void *, void *);
u32 mDoExt_animationFindBinding(void *, void *);
void mDoExt_animationPrepareBinding(void *);
void mDoExt_animationBufferBindRemapped(void *, void *);
void mDoExt_animationCalcBaseConstruct(void *);
u32 mDoExt_animationConstructArray(u32, s32, u32, void *);
u32 mDoExt_animationAllocArray(u32);
u32 mDoExt_animationModelJoints(void *);
u32 mDoExt_animationBufferSize(void *);
void mDoExt_animationBufferConstruct(void *, void *, void *, u32);
void mDoExt_animationCalcBindModel(void *, void *, u32);
u32 mDoExt_animationModelJointData(void *);
u32 mDoExt_animationOldBufferSize(void *);
void mDoExt_animationOldBufferConstruct(void *, void *, void *, u32);
u32 mDoExt_J3DModel_createCall(void *, u32);
void mDoExt_animationFrameRef_destroyCall(void *, u32);
void mDoExt_animationFrameOwner_destroyCall(void *, u32);
u32 mDoExt_animationAlignedAllocCall(u32, u32);
void mDoExt_MtxCalcOldFrame_decOldFrameMorfCounterCall(void *);
void mDoExt_MtxCalcOldFrame_initOldFrameMorfCall(void *, f32, u32, u32);
u32 mDoExt_MtxCalcAnmBlendTblOld_constructCall(void *, void *, u32, void *,
                                               void *);
void mDoExt_MtxCalcAnmBlendTbl_bindAnimationCall(void *, u32);
u32 mDoExt_MtxCalcAnmBlendTbl_constructCall(void *, s32, void *, void *);
void mDoExt_MtxCalcAnmBlendTbl_dispatchCall(void *, void *, void *);
void mDoExt_MtxCalcAnmBlendTblOld_bindModelCall(void *, u32);
