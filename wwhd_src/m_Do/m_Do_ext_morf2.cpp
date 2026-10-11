/** Two-animation morph controller, tww field/method names; private source. */
#include "m_Do/m_Do_ext_morf2.h"
using gabi::cpu;
using gabi::ftoi;
// HD: reordered animation arguments and extended guest-stack constructor arguments stay exact.
// HD: embedded animation buffers, SafeString resource lookup and Md name assurance are retained.
static f32 l_zero;
static f32 l_one;
static mDoExtMorf2::Address l_stringVtable;
static mDoExtMorf2::Address l_mdName;
static mDoExtMorf2::Address l_assertFile;
static mDoExtMorf2::Address l_assertCondition;
static mDoExtMorf2::Word l_resources;
static mDoExtMorf2::Address l_baseVtable;
static mDoExtMorf2::Address l_morfVtable;
static mDoExtMorf2::Address l_secondaryVtable;
static mDoExtMorf2::Address l_soundAgent;
static mDoExtMorf2::Transform l_defaultTransform;
/* 025E5D54 */
void mDoExt_McaMorf2::setAnm(void *animation, void *secondAnimation, s32 mode,
                             void *bas, f32 mix, f32 morf, f32 speed, f32 start,
                             f32 end) {

  u32 o = gabi::ea(this), a = gabi::ea(animation), b = gabi::ea(bas);
  f32 zero = l_zero;
  s16 first = (s16)ftoi(start);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1 = a;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm2 = gabi::ea(secondAnimation);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mAnmRate = mix;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mStart = first;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mFrame = (f32)first;
  s32 last;
  if (!(end < zero))
    last = (s16)ftoi(end);
  else {
    u32 current = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1;
    if (!current)
      last = 0;
    else {
      u32 table = mDoExtMorf2_at<mDoExtMorf2::Animation>(current)->mpVtable,
          target =
              mDoExtMorf2_at<mDoExtMorf2::AnimationVtable>(table)->mGetFrameMax,
          data = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mpData,
          flags = mDoExtMorf2_at<mDoExtMorf2::AnimationData>(data)->mFlags;
      last = gabi::call_ptr<s32>(target, gabi::at<void>(current));
      if (flags & 4)
        last = (s16)(last - 1);
    }
  }
  mDoExtMorf2_frameInit(gabi::at<void>(o + 0xA4), last);
  if (mode < 0) {
    u32 table = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mpVtable;
    mode = gabi::call_ptr<s32>(
        mDoExtMorf2_at<mDoExtMorf2::AnimationVtable>(table)->mGetAttribute,
        animation);
  }
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mRate = speed;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mAttribute = (u8)mode;
  s16 frame = speed < zero ? mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mEnd
                           : first;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mFrame = (f32)frame;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mLoop = frame;
  mDoExtMorf2_setMorf(this, morf);
  u32 audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
  if (!audio)
    return;
  if (!b && a) {
    u32 data = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mpData,
        relative =
            mDoExtMorf2_at<mDoExtMorf2::AnimationData>(data)->mNameOffset,
        name = relative ? data + 4 + relative : 0;
    gabi::Local<mDoExtMorf2::SafeString> str;
    str->mData = name;
    str->mpVtable = gabi::ea(&l_stringVtable);
    b = mDoExtMorf2_resource(gabi::at<void>(l_resources.mValue), str.get(),
                             gabi::at<void>(a + 0xC));
    audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
  }
  mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mpBasAnm = b;
  audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
  u32 agent = mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mpAgent,
      target = mDoExtMorf2_at<mDoExtMorf2::Agent>(agent)->mInit;
  if (!b) {
    gabi::call_ptr(target, gabi::at<void>(audio), 0, 1, zero);
    return;
  }
  s32 direction =
      mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mRate < zero ? -1 : 1;
  f32 current = (f32)mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mLoop;
  gabi::call_ptr(target, gabi::at<void>(audio), gabi::at<void>(b), direction,
                 current);
  gabi::Local<mDoExtMorf2::SafeString> str;
  str->mData = gabi::ea(&l_mdName);
  str->mpVtable = gabi::ea(&l_stringVtable);
  mDoExtMorf2_checkName(str.get());
  gabi::call_ptr(*mDoExtMorf2_at<be<u32>>(str->mpVtable + 0x14), str.get());
  u32 nameVT = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mName.mpVtable,
      nameTarget =
          mDoExtMorf2_at<mDoExtMorf2::AnimationVtable>(nameVT)->mGetFrameMax,
      name = str->mData;
  gabi::call_ptr(nameTarget, gabi::at<void>(a + 0xC), b, direction, nameVT,
                 gabi::ea(&l_mdName));
  u32 other = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mName.mData;
  if (name != other) {
    for (u32 i = 0; i < 0x40001; i++) {
      u8 left = *mDoExtMorf2_at<be<u8>>(name + i),
         right = *mDoExtMorf2_at<be<u8>>(other + i);
      if (left != right)
        return;
      if (!left) {
        audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
        mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mIsMd = 1;
        return;
      }
    }
    return;
  }
  audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
  mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mIsMd = 1;
}

/* 025E6188 */
void mDoExt_McaMorf2::createAnmBuffer() {

  u32 o = gabi::ea(this), model = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel,
      data = mDoExtMorf2_at<mDoExtMorf2::Model>(model)->mpModelData,
      joints = mDoExtMorf2_joints(gabi::at<void>(data));
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpJoints = joints;
  gabi::Local<mDoExtMorf2::BufferDesc>
      desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */
  desc->mFirst = 0;
  desc->mCount = 0xFFFFFFFF;
  desc->mSize = 0;
  (void)mDoExtMorf2_at<mDoExtMorf2::Joints>(joints)->mNum;
  u32 count = mDoExtMorf2_at<mDoExtMorf2::Joints>(joints)->mNum;
  desc->mCount = count;
  u32 size = mDoExtMorf2_bufferSize(desc.get()),
      buffer = mDoExtMorf2_allocAligned(size, 0x40);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpBufferMemory = buffer;
  mDoExtMorf2_bufferConstruct(gabi::at<void>(o + 0x78), desc.get(),
                              gabi::at<void>(buffer), size);
}

/* 025E65FC */
u32 mDoExt_McaMorf2::play(void *position, u32 sound, s32 reverb) {

  u32 o = gabi::ea(this);
  f32 progress = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mCurMorf, one = l_one;
  if (progress < one) {
    f32 delta = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mMorfStep;
    mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mPrevMorf = progress;
    mDoExtMorf2_chaseF(gabi::at<void>(o + 0xB4), one, delta);
  }
  mDoExtMorf2_frameUpdate(gabi::at<void>(o + 0xA4));
  u32 audio = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound;
  if (audio && mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mpBasAnm &&
      gabi::ea(position)) {
    f32 frame = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mFrame,
        rate = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mRate;
    mDoExtMorf2_animSound(gabi::at<void>(audio), position, sound, reverb, frame,
                          rate);
  }
  if (mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mState & 1)
    return 1;
  return mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mRate == l_zero ? 1 : 0;
}

/* 025E66E4 */
void mDoExt_McaMorf2::calc() {
  u32 o = gabi::ea(this);
  if (!mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel)
    return;
  u32 animation = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1;
  if (animation) {
    mDoExtMorf2_at<mDoExtMorf2::Animation>(animation)->mFrameWord =
        mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mFrameWord;
    animation = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1;
    mDoExtMorf2_calcBind(this, gabi::at<void>(animation));
    u32 control = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpControl;
    animation = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1;
    u32 target = mDoExtMorf2_at<mDoExtMorf2::Control>(control)->mCallback,
        receiver = mDoExtMorf2_at<mDoExtMorf2::Control>(control)->mReceiver;
    f32 second = mDoExtMorf2_at<mDoExtMorf2::Control>(control)->mSecond,
        third = mDoExtMorf2_at<mDoExtMorf2::Control>(control)->mThird,
        first = mDoExtMorf2_at<mDoExtMorf2::Animation>(animation)->mFrame;
    f32 result = gabi::call_ptr<f32>(target, gabi::at<void>(receiver), first,
                                     second, third);
    mDoExtMorf2_at<mDoExtMorf2::Control>(control)->mValue = result;
  }
  u32 secondAnimation = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm2;
  if (secondAnimation)
    mDoExtMorf2_at<mDoExtMorf2::Animation>(secondAnimation)->mFrameWord =
        mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mFrameCtrl.mFrameWord;
  u32 model = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel,
      data = mDoExtMorf2_at<mDoExtMorf2::Model>(model)->mpModelData,
      root = mDoExtMorf2_at<mDoExtMorf2::AnimationData>(data)->mpRoot;
  mDoExtMorf2_at<mDoExtMorf2::Root>(root)->mpMtxCalc = o;
  model = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel;
  mDoExtMorf2_modelCalc(gabi::at<void>(model));
}

/* 025E66D4 */
void mDoExt_McaMorf2::entryDL() {
  u32 model = static_cast<mDoExt_McaMorf2 *>(this)->mpModel;
  if (model)
    mDoExtMorf2_modelEntry(gabi::at<void>(model));
}

/* 025E6794 */
void mDoExt_McaMorf2::stopZelAnime() {
  u32 audio = static_cast<mDoExt_McaMorf2 *>(this)->mpSound;
  if (audio)
    mDoExtMorf2_soundStop(gabi::at<void>(audio));
}

/* 025E6220 */
void *mDoExt_McaMorf2_create(void *self, void *data, void *cb1, void *cb2,
                             void *animation, void *secondAnimation, s32 mode,
                             s32 start, f32 speed) {

  u32 sp = cpu->r[1];
  s32 end = (s32)mDoExtMorf2_at<mDoExtMorf2::StackArgs>(sp)->mEnd;
  u32 enable = mDoExtMorf2_at<mDoExtMorf2::StackArgs>(sp)->mEnable,
      b = mDoExtMorf2_at<mDoExtMorf2::StackArgs>(sp)->mBasAnm,
      modelFlag = mDoExtMorf2_at<mDoExtMorf2::StackArgs>(sp)->mModelFlag,
      dlFlag = mDoExtMorf2_at<mDoExtMorf2::StackArgs>(sp)->mDlFlag,
      o = gabi::ea(self), d = gabi::ea(data), a = gabi::ea(animation);
  if (!o) {
    o = mDoExtMorf2_alloc(0xD4);
    if (!o)
      return nullptr;
  }
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpVtable = gabi::ea(&l_baseVtable);
  mDoExtMorf2_baseConstruct(gabi::at<void>(o + 4));
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpVtable = gabi::ea(&l_morfVtable);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSecondaryVtable =
      gabi::ea(&l_secondaryVtable);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mBaseState = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpJoints = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mJointState = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mCalcState = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mAnmBuffer.mState = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpBufferMemory = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm1 = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpAnm2 = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpTransformInfo = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpQuat = 0;
  mDoExtMorf2_frameInit(gabi::at<void>(o + 0xA4), 0);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpCallback1 = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpCallback2 = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel = 0;
  f32 zero = l_zero;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mCurMorf = zero;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpTransformInfo = 0;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mHDEnabled = 1;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mPrevMorf = zero;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mMorfStep = zero;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mAnmRate = zero;
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpQuat = 0;
  if (!d)
    return gabi::at<void>(o);
  u32 model = mDoExtMorf2_modelCreate(data, modelFlag, dlFlag);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel = model;
  if (!model)
    return gabi::at<void>(o);
  if (!a)
    mDoExtMorf2_assert(gabi::at<void>(gabi::ea(&l_assertFile)), 0x153B,
                       gabi::at<void>(gabi::ea(&l_assertCondition)));
  mDoExtMorf2_bindModel(gabi::at<void>(o), data, animation);
  bool found = false;
  if (!b && a) {
    u32 ad = mDoExtMorf2_at<mDoExtMorf2::Animation>(a)->mpData,
        rel = mDoExtMorf2_at<mDoExtMorf2::AnimationData>(ad)->mNameOffset,
        name = rel ? ad + 4 + rel : 0;
    gabi::Local<mDoExtMorf2::SafeString> str;
    str->mData = name;
    str->mpVtable = gabi::ea(&l_stringVtable);
    b = mDoExtMorf2_resource(gabi::at<void>(l_resources.mValue), str.get(),
                             gabi::at<void>(a + 0xC));
    found = b != 0;
  }
  if ((found || enable) && !mDoExtMorf2_audioDisabled()) {
    u32 audio = mDoExtMorf2_alloc(0xA0);
    if (audio) {
      mDoExtMorf2_clear(gabi::at<void>(audio), 0xA0);
      mDoExtMorf2_soundConstruct(gabi::at<void>(audio));
      mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mIsMd = 0;
      mDoExtMorf2_at<mDoExtMorf2::Sound>(audio)->mpAgent =
          gabi::ea(&l_soundAgent);
    }
    mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpSound = audio;
    if (!audio) {
      mDoExtMorf2_ERROR_EXIT(gabi::at<void>(o));
      return gabi::at<void>(o);
    }
  }
  mDoExtMorf2_setAnmCall(gabi::at<void>(o), animation, secondAnimation, mode,
                         gabi::at<void>(b), zero, zero, speed, (f32)start,
                         (f32)end);
  u32 joints = mDoExtMorf2_joints(data),
      count = mDoExtMorf2_at<mDoExtMorf2::Joints>(joints)->mNum,
      trans = mDoExtMorf2_allocArray(count << 5);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpTransformInfo = trans;
  if (!trans) {
    mDoExtMorf2_ERROR_EXIT(gabi::at<void>(o));
    return gabi::at<void>(o);
  }
  joints = mDoExtMorf2_joints(data);
  count = mDoExtMorf2_at<mDoExtMorf2::Joints>(joints)->mNum;
  u32 quat = mDoExtMorf2_allocArray(count << 4);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpQuat = quat;
  if (!quat) {
    mDoExtMorf2_ERROR_EXIT(gabi::at<void>(o));
    return gabi::at<void>(o);
  }
  mDoExtMorf2_createBufferCall(gabi::at<void>(o));
  model = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpModel;
  trans = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpTransformInfo;
  u32 md = mDoExtMorf2_at<mDoExtMorf2::Model>(model)->mpModelData;
  quat = mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpQuat;
  joints = mDoExtMorf2_joints(gabi::at<void>(md));
  count = mDoExtMorf2_at<mDoExtMorf2::Joints>(joints)->mNum;
  if (count) {
    f32 sx = l_defaultTransform.mScale.x, sy = l_defaultTransform.mScale.y,
        sz = l_defaultTransform.mScale.z, tx = l_defaultTransform.mTranslate.x,
        ty = l_defaultTransform.mTranslate.y,
        tz = l_defaultTransform.mTranslate.z;
    s16 rx = l_defaultTransform.mRotation.x,
        ry = l_defaultTransform.mRotation.y,
        rz = l_defaultTransform.mRotation.z;
    for (u32 i = 0; i < count; i++) {
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mScale.x = sx;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mScale.y = sy;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mScale.z = sz;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mTranslate.x = tx;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mTranslate.y = ty;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mRotation.x = rx;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mRotation.y = ry;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mRotation.z = rz;
      mDoExtMorf2_at<mDoExtMorf2::Transform>(trans)->mTranslate.z = tz;
      mDoExtMorf2_EulerToQuat((s32)rx, (s32)ry, (s32)rz, gabi::at<void>(quat));
      trans += 32;
      quat += 16;
    }
  }
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpCallback1 = gabi::ea(cb1);
  mDoExtMorf2_at<mDoExt_McaMorf2>(o)->mpCallback2 = gabi::ea(cb2);
  return gabi::at<void>(o);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 027F2BC0 void mDoExtMorf2_frameInit(void*, s32)
ext 025E5D1C void mDoExtMorf2_setMorf(void*, f32)
ext 0260702C u32 mDoExtMorf2_resource(void*, void*, void*)
ext 025EE5EC void mDoExtMorf2_checkName(void*)
ext 027F3F94 u32 mDoExtMorf2_joints(void*)
ext 027E06E0 u32 mDoExtMorf2_bufferSize(void*)
ext 025E3A34 u32 mDoExtMorf2_allocAligned(u32, u32)
ext 027E063C void mDoExtMorf2_bufferConstruct(void*, void*, void*, u32)
ext 0200F5C8 void mDoExtMorf2_chaseF(void*, f32, f32)
ext 027F2FC4 void mDoExtMorf2_frameUpdate(void*)
ext 0201BF08 void mDoExtMorf2_animSound(void*, void*, u32, s32, f32, f32)
ext 027F363C void mDoExtMorf2_calcBind(void*, void*)
ext 027F4D5C void mDoExtMorf2_modelCalc(void*)
ext 025E2E5C void mDoExtMorf2_modelEntry(void*)
ext 02801D3C void mDoExtMorf2_soundStop(void*)
ext 0273AD10 u32 mDoExtMorf2_alloc(u32)
ext 027DA984 void mDoExtMorf2_baseConstruct(void*)
ext 025E38E0 u32 mDoExtMorf2_modelCreate(void*, u32, u32)
ext 0273AA24 void mDoExtMorf2_assert(void*, u32, void*)
ext 027F36FC void mDoExtMorf2_bindModel(void*, void*, void*)
ext 025E20F8 u32 mDoExtMorf2_audioDisabled()
ext 028F521C void mDoExtMorf2_clear(void*, u32)
ext 02801DA0 void mDoExtMorf2_soundConstruct(void*)
ext 025E5C90 void mDoExtMorf2_ERROR_EXIT(void*)
ext 025E5D54 void mDoExtMorf2_setAnmCall(void*, void*, void*, s32, void*, f32, f32, f32, f32, f32)
ext 0273ADAC u32 mDoExtMorf2_allocArray(u32)
ext 025E6188 void mDoExtMorf2_createBufferCall(void*)
ext 027ED2D0 void mDoExtMorf2_EulerToQuat(s32, s32, s32, void*)
data l_zero 100586DC
data l_one 100586D4
data l_stringVtable 10058704
data l_mdName 10058A10
data l_assertFile 10058A1C
data l_assertCondition 10058A14
data l_resources 101F4F28
data l_baseVtable 1016E54C
data l_morfVtable 10058CE8
data l_secondaryVtable 1016D820
data l_soundAgent 1005871C
data l_defaultTransform 1016E464
*/
