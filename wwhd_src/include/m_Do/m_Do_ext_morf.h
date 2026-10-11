#pragma once
#include "wwhd.h"
namespace mDoExtMorf {
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
  union {
    be<f32> mFrame;
    be<u32> mFrameWord;
  };
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
  union {
    be<u32> mFrameWord;
    be<f32> mFrame;
  };
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
  u8 padding0[0xC];
  be<u32> mpVtable;
  u8 padding10[0x9C];
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
  be<u32> mEnable, mBasAnm, mModelFlag, mDlFlag;
};
WWHD_SIZE(Transform, 0x20);
WWHD_SIZE(FrameCtrl, 0x10);
WWHD_SIZE(AnmBuffer, 0x14);
WWHD_SIZE(SafeString, 8);
WWHD_SIZE(BufferDesc, 0x14);
WWHD_SIZE(Sound, 0xA0);
} // namespace mDoExtMorf
// HD: J3DMtxCalcMaya has expanded state and an embedded animation buffer.
struct mDoExt_McaMorf {
  be<u32> mpVtable, mpControl;
  u8 padding8[0x30];
  be<u32> mpSecondaryVtable;
  u8 padding3C[12];
  be<u32> mBaseState;
  u8 padding4C[0x20];
  be<u32> mpJoints, mJointState, mCalcState;
  mDoExtMorf::AnmBuffer mAnmBuffer;
  be<u32> mpBufferMemory, mpModel, mpAnm;
  mDoExtMorf::FrameCtrl mFrameCtrl;
  be<u32> mpTransformInfo, mpQuat;
  be<f32> mCurMorf, mPrevMorf, mMorfStep;
  be<u32> mpSound, mpCallback1, mpCallback2;
  void setMorf(f32 duration);
  void createAnmBuffer();
  u32 play(void *position, u32 sound, s32 reverb);
  void update();
  void updateDL();
  void calc();
  void entry();
  void entryDL();
  void stopZelAnime();
  void setAnm(void *animation, s32 mode, void *bas, f32 morf, f32 speed,
              f32 start, f32 end);
};
WWHD_SIZE(mDoExt_McaMorf, 0xC8);
WWHD_OFFSET(mDoExt_McaMorf, mFrameCtrl, 0x98);
WWHD_OFFSET(mDoExt_McaMorf, mCurMorf, 0xB0);
WWHD_OFFSET(mDoExt_McaMorf, mpSound, 0xBC);
template <class T> inline T *mDoExtMorf_at(u32 address) {
  return gabi::at<T>(address);
}
u32 mDoExtMorf_joints(void *);
u32 mDoExtMorf_bufferSize(void *);
u32 mDoExtMorf_allocAligned(u32, u32);
void mDoExtMorf_bufferConstruct(void *, void *, void *, u32);
void mDoExtMorf_chaseF(void *, f32, f32);
void mDoExtMorf_frameUpdate(void *);
void mDoExtMorf_animSound(void *, void *, u32, s32, f32, f32);
void mDoExtMorf_calcBind(void *, void *);
void mDoExtMorf_modelUpdate(void *);
void mDoExtMorf_modelUpdateDL(void *, u32);
void mDoExtMorf_modelCalc(void *);
void mDoExtMorf_modelEntry(void *);
void mDoExtMorf_modelEntryDL(void *);
void mDoExtMorf_soundStop(void *);
void mDoExtMorf_frameInit(void *, s32);
void mDoExtMorf_setMorfCall(void *, f32);
u32 mDoExtMorf_resource(void *, void *, void *);
void mDoExtMorf_checkName(void *);
u32 mDoExtMorf_alloc(u32);
void mDoExtMorf_baseConstruct(void *);
u32 mDoExtMorf_modelCreate(void *, u32, u32);
void mDoExtMorf_bindModel(void *, void *, void *);
u32 mDoExtMorf_audioDisabled();
void mDoExtMorf_clear(void *, u32);
void mDoExtMorf_soundConstruct(void *);
void mDoExtMorf_setAnmCall(void *, void *, s32, void *, f32, f32, f32, f32);
u32 mDoExtMorf_allocArray(u32);
void mDoExtMorf_createBufferCall(void *);
void mDoExtMorf_EulerToQuat(s32, s32, s32, void *);
