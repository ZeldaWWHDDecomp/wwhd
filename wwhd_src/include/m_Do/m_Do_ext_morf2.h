#pragma once
#include "wwhd.h"
namespace mDoExtMorf2 {
struct Address {
  u8 opaque;
};
struct Word {
  be<u32> mValue;
};
struct Vec {
  be<f32> x, y, z;
};
struct Rotation {
  be<s16> x, y, z;
};
struct Transform {
  Vec mScale;
  Rotation mRotation;
  u8 padding12[2];
  Vec mTranslate;
};
// HD: frame words are copied without floating conversion in calc().
struct FrameCtrl {
  be<f32> mRate;
  union { be<f32> mFrame; be<u32> mFrameWord; };
  be<s16> mStart, mEnd, mLoop;
  be<u8> mAttribute, mState;
};
struct AnmBuffer {
  u8 padding0[0x10];
  be<u32> mState;
};
struct SafeString {
  be<u32> mData, mpVtable;
};
struct Animation {
  union { be<u32> mFrameWord; be<f32> mFrame; };
  be<u32> mpVtable, mpData;
  SafeString mName;
};
struct AnimationData {
  u8 padding0[4];
  be<u32> mNameOffset, mpRoot, mFlags;
};
struct AnimationVtable {
  u8 padding0[0xC];
  be<u32> mGetAttribute;
  u8 padding10[4];
  be<u32> mGetFrameMax;
};
struct Model {
  u8 padding0[0xAC];
  be<u32> mpModelData;
};
struct Joints {
  u8 padding0[8];
  be<u16> mNum;
};
struct Root {
  u8 padding0[0x14];
  be<u32> mpMtxCalc;
};
struct Control {
  be<f32> mValue, mSecond, mThird;
  u8 paddingC[4];
  be<u32> mCallback, mReceiver;
};
struct Agent {
  u8 padding0[0x14];
  be<u32> mInit;
};
struct Sound {
  u8 padding0[0x94];
  be<u32> mpAgent, mIsMd, mpBasAnm;
};
struct BufferDesc {
  be<u32> mCount, mFirst, mSize;
  u8 scratch[8];
};
struct StackArgs {
  u8 callerFrame[8];
  be<s32> mEnd;
  be<u32> mEnable, mBasAnm, mModelFlag, mDlFlag;
};
WWHD_SIZE(Transform, 0x20);
WWHD_SIZE(FrameCtrl, 0x10);
WWHD_SIZE(AnmBuffer, 0x14);
WWHD_SIZE(SafeString, 8);
WWHD_SIZE(BufferDesc, 0x14);
WWHD_SIZE(Sound, 0xA0);
} // namespace mDoExtMorf2
// HD: J3DMtxCalcMaya has expanded state and an embedded animation buffer.
struct mDoExt_McaMorf2 {
  be<u32> mpVtable, mpControl;
  u8 padding8[0x30];
  be<u32> mpSecondaryVtable;
  u8 padding3C[12];
  be<u32> mBaseState;
  u8 padding4C[0x20];
  be<u32> mpJoints, mJointState, mCalcState;
  mDoExtMorf2::AnmBuffer mAnmBuffer;
  be<u32> mpBufferMemory, mpModel, mpAnm1, mpAnm2, mpTransformInfo, mpQuat;
  mDoExtMorf2::FrameCtrl mFrameCtrl;
  be<f32> mCurMorf, mPrevMorf, mMorfStep, mAnmRate;
  be<u32> mpSound, mpCallback1, mpCallback2;
  be<u8> mHDEnabled;
  u8 paddingD1[3];
  void setAnm(void *animation, void *secondAnimation, s32 mode, void *bas,
              f32 mix, f32 morf, f32 speed, f32 start, f32 end);
  void createAnmBuffer();
  u32 play(void *position, u32 sound, s32 reverb);
  void calc();
  void entryDL();
  void stopZelAnime();
};
WWHD_SIZE(mDoExt_McaMorf2, 0xD4);
WWHD_OFFSET(mDoExt_McaMorf2, mpModel, 0x90);
WWHD_OFFSET(mDoExt_McaMorf2, mFrameCtrl, 0xA4);
WWHD_OFFSET(mDoExt_McaMorf2, mpSound, 0xC4);
template <class T> inline T *mDoExtMorf2_at(u32 address) {
  return gabi::at<T>(address);
}
void mDoExtMorf2_frameInit(void *, s32);
void mDoExtMorf2_setMorf(void *, f32);
u32 mDoExtMorf2_resource(void *, void *, void *);
void mDoExtMorf2_checkName(void *);
u32 mDoExtMorf2_joints(void *);
u32 mDoExtMorf2_bufferSize(void *);
u32 mDoExtMorf2_allocAligned(u32, u32);
void mDoExtMorf2_bufferConstruct(void *, void *, void *, u32);
void mDoExtMorf2_chaseF(void *, f32, f32);
void mDoExtMorf2_frameUpdate(void *);
void mDoExtMorf2_animSound(void *, void *, u32, s32, f32, f32);
void mDoExtMorf2_calcBind(void *, void *);
void mDoExtMorf2_modelCalc(void *);
void mDoExtMorf2_modelEntry(void *);
void mDoExtMorf2_soundStop(void *);
u32 mDoExtMorf2_alloc(u32);
void mDoExtMorf2_baseConstruct(void *);
u32 mDoExtMorf2_modelCreate(void *, u32, u32);
void mDoExtMorf2_assert(void *, u32, void *);
void mDoExtMorf2_bindModel(void *, void *, void *);
u32 mDoExtMorf2_audioDisabled();
void mDoExtMorf2_clear(void *, u32);
void mDoExtMorf2_soundConstruct(void *);
void mDoExtMorf2_ERROR_EXIT(void *);
void mDoExtMorf2_setAnmCall(void *, void *, void *, s32, void *, f32, f32, f32,
                            f32, f32);
u32 mDoExtMorf2_allocArray(u32);
void mDoExtMorf2_createBufferCall(void *);
void mDoExtMorf2_EulerToQuat(s32, s32, s32, void *);
