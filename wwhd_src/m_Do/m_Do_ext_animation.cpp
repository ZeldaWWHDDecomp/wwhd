/** HD animation helpers, using tww old-frame and blend-table names. Private source. */
#include "m_Do/m_Do_ext_animation.h"
// HD: blend tables include packed animation buffers and dynamic member dispatch.
// HD: constructors retain their null-this allocation and verified return-word ABI.
static mDoExtAnimation::Address l_modelFile;
static mDoExtAnimation::Address l_modelCondition;
static mDoExtAnimation::Address l_frameRefVtable;
static mDoExtAnimation::Address l_frameOwnerVtable;
static f32 l_one;
static f32 l_zero;
static mDoExtAnimation::Address l_blendOldVtable;
static mDoExtAnimation::Address l_calcBaseVtable;
static mDoExtAnimation::Address l_blendVtable;
static mDoExtAnimation::Address l_calcSecondaryVtable;
static mDoExtAnimation::Address l_bufferConstructor;
static mDoExtAnimation::Address l_dispatchTable;
/* 025E38E0 */
u32 mDoExt_J3DModel_create(void *data, u32 flags) {

  if (!gabi::ea(data)) {
    mDoExt_animationAssert(gabi::at<void>(gabi::ea(&l_modelFile)), 0x25A0,
                           gabi::at<void>(gabi::ea(&l_modelCondition)));
    return 0;
  }
  u32 model = mDoExt_animationAlloc(0x144);
  if (!model)
    return 0;
  model = mDoExt_animationModelConstruct(gabi::at<void>(model));
  if (!model)
    return 0;
  if (mDoExt_animationEntryModelData(gabi::at<void>(model), data, flags, 2))
    return 0;
  return model;
}

/* 025E399C */
void mDoExt_animationFrameRef_destroy(void *self, u32 flags) {

  if (!gabi::ea(self))
    return;
  u32 object = static_cast<mDoExtAnimation::FrameRef *>(self)->mpFrame;
  static_cast<mDoExtAnimation::FrameRef *>(self)->mpVtable =
      gabi::ea(&l_frameRefVtable);
  *mDoExtAnimation_at<be<u32>>(object) = 0;
  if (flags & 1)
    mDoExt_animationFree(self);
}

/* 025E39C8 */
void mDoExt_animationFrameOwner_destroy(void *self, u32 flags) {

  if (!gabi::ea(self))
    return;
  static_cast<mDoExtAnimation::FrameOwner *>(self)->mFrameRef.mpVtable =
      gabi::ea(&l_frameOwnerVtable);
  mDoExt_animationFrameRef_destroyCall(
      &static_cast<mDoExtAnimation::FrameOwner *>(self)->mFrameRef, 0);
  mDoExt_animationFrameBaseDestroy(self, 0);
  if (flags & 1)
    mDoExt_animationFree(self);
}

/* 025E3A34 */
u32 mDoExt_animationAlignedAlloc(u32 size, u32 align) {
  return mDoExt_animationAllocAligned(size, align);
}

/* 025E3EC8 */
void mDoExtAnimation::mDoExt_MtxCalcOldFrame::decOldFrameMorfCounter() {

  f32 count = mOldFrameMorfCounter;
  if (!(count > 0))
    return;
  f32 one = l_one, zero = l_zero, next = gabi::fsubs_ppc(count, one);
  if (!(next > zero)) {
    field_0x8 = zero;
    next = zero;
    mOldFrameRate = zero;
  }
  mOldFrameMorfCounter = next;
  f32 inv = field_0x8, old = field_0x10, value = gabi::fmuls_ppc(next, inv);
  field_0x10 = value;
  field_0x14 = old;
  if (old > zero) {
    f32 ratio = gabi::fsubs_ppc(old, value) / old;
    mOldFrameRate = gabi::fsubs_ppc(one, ratio);
  } else
    mOldFrameRate = zero;
}

/* 025E3F3C */
void mDoExtAnimation::mDoExt_MtxCalcOldFrame::initOldFrameMorf(f32 duration, u32 start,
                                                 u32 end) {

  f32 zero = l_zero;
  if (duration > zero) {
    f32 one = l_one, inv = one / duration;
    field_0x10 = one;
    field_0x14 = one;
    mOldFrameRate = one;
    mOldFrameMorfCounter = duration;
    field_0x8 = inv;
    mDoExt_MtxCalcOldFrame_decOldFrameMorfCounterCall(this);
    mOldFrameEndJoint = end;
    mOldFrameStartJoint = start;
  } else {
    mOldFrameStartJoint = start;
    field_0x10 = zero;
    field_0x14 = zero;
    mOldFrameRate = zero;
    mOldFrameEndJoint = end;
    mOldFrameMorfCounter = zero;
    field_0x8 = zero;
  }
}

/* 025E3FB4 */
u32 mDoExt_MtxCalcAnmBlendTblOld_construct(void *self, void *old, u32 count,
                                           void *table, void *model) {

  u32 o = gabi::ea(self);
  if (!o) {
    o = mDoExt_animationAlloc(0xB8);
    if (!o)
      return 0;
  }
  mDoExt_MtxCalcAnmBlendTbl_constructCall(gabi::at<void>(o), count, table,
                                          model);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpVtable =
      gabi::ea(&l_blendOldVtable);
  mDoExt_animationBufferInit(
      &mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mOldBuffer, 0x14);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mOldFrame = gabi::ea(old);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpOldBufferMemory = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mBeforeCallback = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAfterCallback = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpOldTransformInfo = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mOldBuffer.mState = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mUserArea = 0;
  return o;
}

/* 025E3A6C */
void mDoExt_MtxCalcAnmBlendTbl_bindAnimation(void *self, u32 index) {

  u32 o = gabi::ea(self), offset = index << 4,
      entry =
          mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio + offset;
  if (!mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform)
    return;
  u32 anim =
          mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform,
      stride = index * 0x68;
  u32 buffer =
          mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers + stride,
      data = mDoExtAnimation_at<mDoExtAnimation::Animation>(anim)->mpData;
  mDoExt_animationBufferSetData(gabi::at<void>(buffer), gabi::at<void>(data));
  buffer =
      mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers + stride;
  data = mDoExtAnimation_at<mDoExtAnimation::AnimationBuffer>(buffer)->mpData;
  u32 relative = mDoExtAnimation_at<mDoExtAnimation::AnimationData>(data)
                     ->mJointDataOffset,
      reference = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints,
      resolved = relative ? data + 0x24 + relative : 0;
  if (reference == resolved) {
    mDoExt_animationBufferBindDirect(gabi::at<void>(buffer),
                                     gabi::at<void>(reference));
  } else {
    entry =
        mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio + offset;
    anim = mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform;
    data = mDoExtAnimation_at<mDoExtAnimation::Animation>(anim)->mpData;
    u32 kind = mDoExt_animationFindBinding(gabi::at<void>(data),
                                           gabi::at<void>(reference));
    if ((kind & 0xFFFF) == 0) {
      buffer =
          mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers + stride;
      reference = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints;
      mDoExt_animationBufferBindDirect(gabi::at<void>(buffer),
                                       gabi::at<void>(reference));
    } else {
      entry =
          mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio + offset;
      anim =
          mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform;
      data = mDoExtAnimation_at<mDoExtAnimation::Animation>(anim)->mpData;
      mDoExt_animationPrepareBinding(gabi::at<void>(data));
      buffer =
          mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers + stride;
      reference = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints;
      mDoExt_animationBufferBindRemapped(gabi::at<void>(buffer),
                                         gabi::at<void>(reference));
    }
  }
  u32 list = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio,
      buffers = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers;
  entry = list + offset;
  u32 control = *mDoExtAnimation_at<be<u32>>(buffers + stride);
  anim = mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform;
  u32 target =
      mDoExtAnimation_at<mDoExtAnimation::AnimationControl>(control)->mCallback;
  f32 second =
      mDoExtAnimation_at<mDoExtAnimation::AnimationControl>(control)->mSecond;
  u32 receiver =
      mDoExtAnimation_at<mDoExtAnimation::AnimationControl>(control)->mReceiver;
  f32 third = mDoExtAnimation_at<mDoExtAnimation::AnimationControl>(control)
                  ->mThird,
      first = mDoExtAnimation_at<mDoExtAnimation::Animation>(anim)->mFrame;
  f32 result = gabi::call_ptr<f32>(target, gabi::at<void>(receiver), first,
                                   second, third);
  mDoExtAnimation_at<mDoExtAnimation::AnimationControl>(control)->mValue =
      result;
}

/* 025E3BCC */
u32 mDoExt_MtxCalcAnmBlendTbl_construct(void *self, s32 count, void *table,
                                        void *model) {

  u32 o = gabi::ea(self);
  if (!o) {
    o = mDoExt_animationAlloc(0x8C);
    if (!o)
      return 0;
  }
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpVtable =
      gabi::ea(&l_calcBaseVtable);
  mDoExt_animationCalcBaseConstruct(gabi::at<void>(o + 4));
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mNum = count;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mBaseState = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpVtable =
      gabi::ea(&l_blendVtable);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpSecondaryVtable =
      gabi::ea(&l_calcSecondaryVtable);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mJointState = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBufferMemory = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio =
      gabi::ea(table);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mCalcState = 0;
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpModel = gabi::ea(model);
  s32 total = count;
  for (s32 i = 0; i < total; i++) {
    u32 entry = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio +
                u32(i) * 16;
    if (!mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform) {
      s32 n = mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mValueCount;
      for (s32 j = 0; j < n; j++) {
        u32 data =
            mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpValues;
        *mDoExtAnimation_at<be<f32>>(data + u32(j) * 4) = l_zero;
        n = mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mValueCount;
      }
      total = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mNum;
    }
  }
  u32 buffers = mDoExt_animationConstructArray(
      0, count, 0x68, gabi::at<void>(gabi::ea(&l_bufferConstructor)));
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers = buffers;
  u32 pointers = mDoExt_animationAllocArray(u32(count) << 2);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBufferMemory = pointers;
  gabi::Local<mDoExtAnimation::BufferDescriptor>
      desc; /* game test 2026-10-05: 027DFCCC/027DFB70 fill desc+0x10..+0x30 */
  for (s32 i = 0; i < count; i++) {
    u32 mod = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpModel,
        md = mDoExtAnimation_at<mDoExtAnimation::Model>(mod)->mpModelData,
        joints = mDoExt_animationModelJoints(gabi::at<void>(md));
    mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints = joints;
    u32 first = mDoExtAnimation_at<mDoExtAnimation::JointData>(joints)->mNum;
    desc->mWorkCount = 0;
    desc->mBound = 0;
    desc->mWorkSize = 0;
    desc->mCount = 0xFFFFFFFF;
    desc->mEnabled = 1;
    desc->mValueBytes = 0xFFFFFFFF;
    desc->mFirst = 0xFFFFFFFF;
    joints = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints;
    u32 second = mDoExtAnimation_at<mDoExtAnimation::JointData>(joints)->mNum;
    desc->mFirst = first;
    desc->mCount = second;
    desc->mBound = 1;
    desc->mValueBytes = first << 2;
    u32 size = mDoExt_animationBufferSize(desc.get());
    u32 array =
        mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBufferMemory;
    u32 buffer = mDoExt_animationAlignedAllocCall(size, 0x40);
    *mDoExtAnimation_at<be<u32>>(array + u32(i) * 4) = buffer;
    buffers = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBuffers;
    array = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpBufferMemory;
    buffer = *mDoExtAnimation_at<be<u32>>(array + u32(i) * 4);
    mDoExt_animationBufferConstruct(gabi::at<void>(buffers + u32(i) * 0x68),
                                    desc.get(), gabi::at<void>(buffer), size);
    u32 entry = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mAnmRatio +
                u32(i) * 16;
    if (mDoExtAnimation_at<mDoExtAnimation::AnmRatio>(entry)->mpAnmTransform)
      mDoExt_MtxCalcAnmBlendTbl_bindAnimationCall(gabi::at<void>(o), i);
  }
  return o;
}

/* 025E3DEC */
void mDoExt_MtxCalcAnmBlendTbl_dispatch(void *self, void *buffer,
                                        void *targetObj) {

  if (!gabi::ea(targetObj))
    return;
  u32 o = gabi::ea(self), b = gabi::ea(buffer),
      vt = mDoExtAnimation_at<mDoExtAnimation::AnimationBuffer>(b)->mpVtable,
      target = mDoExtAnimation_at<mDoExtAnimation::BufferVtable>(vt)->mUpdate;
  gabi::call_ptr(target, buffer);
  u32 data = mDoExtAnimation_at<mDoExtAnimation::AnimationBuffer>(b)->mpData,
      flags = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mFlags,
      kind = mDoExtAnimation_at<mDoExtAnimation::AnimationData>(data)->mKind;
  u32 index = (flags & 2) | ((kind & 0x7000) >> 12);
  u32 entry = gabi::ea(&l_dispatchTable) + (index << 3);
  s16 slot = mDoExtAnimation_at<mDoExtAnimation::DispatchEntry>(entry)->mSlot,
      adjust = mDoExtAnimation_at<mDoExtAnimation::DispatchEntry>(entry)
                   ->mThisAdjust;
  u32 receiver = o + s32(adjust);
  if (slot < 0)
    target = mDoExtAnimation_at<mDoExtAnimation::DispatchEntry>(entry)->mTarget;
  else {
    s16 voff = mDoExtAnimation_at<mDoExtAnimation::VirtualDispatchEntry>(entry)
                   ->mVtableOffset;
    vt = *mDoExtAnimation_at<be<u32>>(receiver + s32(voff));
    target = *mDoExtAnimation_at<be<u32>>(vt + u32(s32(slot) * 8) + 4);
  }
  gabi::call_ptr(target, gabi::at<void>(receiver), buffer, targetObj);
}

/* 025E4050 */
void mDoExt_MtxCalcAnmBlendTblOld_bindModel(void *self, u32 index) {

  u32 o = gabi::ea(self),
      model = mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpModel,
      data = mDoExtAnimation_at<mDoExtAnimation::Model>(model)->mpModelData;
  mDoExt_animationCalcBindModel(self, gabi::at<void>(data), index);
  u32 joints = mDoExt_animationModelJointData(gabi::at<void>(data));
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpJoints = joints;
  gabi::Local<mDoExtAnimation::OldBufferDescriptor>
      desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */
  desc->mFirst = 0;
  desc->mCount = 0xFFFFFFFF;
  desc->mValueBytes = 0;
  u32 first = mDoExtAnimation_at<mDoExtAnimation::JointData>(joints)->mNum;
  u32 second = mDoExtAnimation_at<mDoExtAnimation::JointData>(joints)->mNum;
  desc->mCount = second;
  u32 size = mDoExt_animationOldBufferSize(desc.get()),
      buffer = mDoExt_animationAlignedAllocCall(size, 0x40);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpOldBufferMemory = buffer;
  mDoExt_animationOldBufferConstruct(
      &mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mOldBuffer,
      desc.get(), gabi::at<void>(buffer), size);
  u32 old = mDoExt_animationAllocArray(second * 0x38);
  mDoExtAnimation_at<mDoExtAnimation::BlendCalc>(o)->mpOldTransformInfo = old;
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AA24 void mDoExt_animationAssert(void*, u32, void*)
ext 0273AD10 u32 mDoExt_animationAlloc(u32)
ext 027F3E04 u32 mDoExt_animationModelConstruct(void*)
ext 027F4404 u32 mDoExt_animationEntryModelData(void*, void*, u32, u32)
ext 0273AF40 void mDoExt_animationFree(void*)
ext 02752BEC void mDoExt_animationFrameBaseDestroy(void*, u32)
ext 0273AE48 u32 mDoExt_animationAllocAligned(u32, u32)
ext 028F521C void mDoExt_animationBufferInit(void*, u32)
ext 027E0368 void mDoExt_animationBufferSetData(void*, void*)
ext 027E00AC void mDoExt_animationBufferBindDirect(void*, void*)
ext 027E2AF4 u32 mDoExt_animationFindBinding(void*, void*)
ext 027E2CFC void mDoExt_animationPrepareBinding(void*)
ext 027DFF1C void mDoExt_animationBufferBindRemapped(void*, void*)
ext 027DA984 void mDoExt_animationCalcBaseConstruct(void*)
ext 028EFFD0 u32 mDoExt_animationConstructArray(u32, s32, u32, void*)
ext 0273ADAC u32 mDoExt_animationAllocArray(u32)
ext 027F3F94 u32 mDoExt_animationModelJoints(void*)
ext 027DFCCC u32 mDoExt_animationBufferSize(void*)
ext 027DFB70 void mDoExt_animationBufferConstruct(void*, void*, void*, u32)
ext 027F36FC void mDoExt_animationCalcBindModel(void*, void*, u32)
ext 027F68FC u32 mDoExt_animationModelJointData(void*)
ext 027E06E0 u32 mDoExt_animationOldBufferSize(void*)
ext 027E063C void mDoExt_animationOldBufferConstruct(void*, void*, void*, u32)
ext 025E38E0 u32 mDoExt_J3DModel_createCall(void*, u32)
ext 025E399C void mDoExt_animationFrameRef_destroyCall(void*, u32)
ext 025E39C8 void mDoExt_animationFrameOwner_destroyCall(void*, u32)
ext 025E3A34 u32 mDoExt_animationAlignedAllocCall(u32, u32)
ext 025E3EC8 void mDoExt_MtxCalcOldFrame_decOldFrameMorfCounterCall(void*)
ext 025E3F3C void mDoExt_MtxCalcOldFrame_initOldFrameMorfCall(void*, f32, u32, u32)
ext 025E3FB4 u32 mDoExt_MtxCalcAnmBlendTblOld_constructCall(void*, void*, u32, void*, void*)
ext 025E3A6C void mDoExt_MtxCalcAnmBlendTbl_bindAnimationCall(void*, u32)
ext 025E3BCC u32 mDoExt_MtxCalcAnmBlendTbl_constructCall(void*, s32, void*, void*)
ext 025E3DEC void mDoExt_MtxCalcAnmBlendTbl_dispatchCall(void*, void*, void*)
ext 025E4050 void mDoExt_MtxCalcAnmBlendTblOld_bindModelCall(void*, u32)
data l_modelFile 100589AC
data l_modelCondition 100589A4
data l_frameRefVtable 10058DA0
data l_frameOwnerVtable 10058DB0
data l_one 100586D4
data l_zero 100586DC
data l_blendOldVtable 10058C98
data l_calcBaseVtable 1016E54C
data l_blendVtable 10058C70
data l_calcSecondaryVtable 1016D820
data l_bufferConstructor 025EE17C
data l_dispatchTable 10058C50
*/
