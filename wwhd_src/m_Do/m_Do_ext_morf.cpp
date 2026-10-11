/** Single-animation morph controller, tww fields and methods. Private source. */
#include "m_Do/m_Do_ext_morf.h"
using gabi::cpu;
using gabi::ftoi;
// HD: verified argument ordering, constructor guest-stack words and buffer state are retained.
// HD: resource SafeString calls and Md comparison preserve callback argument ordering.
static f32 l_zero;
static f32 l_one;
static f32 l_initialPrevMorf;
static mDoExtMorf::Address l_stringVtable;
static mDoExtMorf::Address l_mdName;
static mDoExtMorf::Word l_resources;
static mDoExtMorf::Address l_baseVtable;
static mDoExtMorf::Address l_morfVtable;
static mDoExtMorf::Address l_secondaryVtable;
static mDoExtMorf::Address l_soundAgent;
static mDoExtMorf::Transform l_defaultTransform;
/* 025E4A54 */
void mDoExt_McaMorf::setMorf(f32 duration) {

  u32 o = gabi::ea(this);
  f32 previous = mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf, zero = l_zero,
      one = l_one;
  if (previous < zero || !(duration > zero)) {
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf = one;
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = one;
    return;
  }
  f32 delta = one / duration;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf = zero;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = zero;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mMorfStep = delta;
}

/* 025E4ECC */
void mDoExt_McaMorf::createAnmBuffer() {

  u32 o = gabi::ea(this), model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel,
      data = mDoExtMorf_at<mDoExtMorf::Model>(model)->mpModelData,
      joints = mDoExtMorf_joints(gabi::at<void>(data));
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpJoints = joints;
  gabi::Local<mDoExtMorf::BufferDesc>
      desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */
  desc->mFirst = 0;
  desc->mCount = 0xFFFFFFFF;
  desc->mSize = 0;
  (void)mDoExtMorf_at<mDoExtMorf::Joints>(joints)->mNum;
  u32 count = mDoExtMorf_at<mDoExtMorf::Joints>(joints)->mNum;
  desc->mCount = count;
  u32 size = mDoExtMorf_bufferSize(desc.get()),
      buffer = mDoExtMorf_allocAligned(size, 0x40);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpBufferMemory = buffer;
  mDoExtMorf_bufferConstruct(gabi::at<void>(o + 0x78), desc.get(),
                             gabi::at<void>(buffer), size);
}

/* 025E535C */
u32 mDoExt_McaMorf::play(void *position, u32 sound, s32 reverb) {

  u32 o = gabi::ea(this);
  f32 progress = mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf, one = l_one;
  if (progress < one) {
    f32 delta = mDoExtMorf_at<mDoExt_McaMorf>(o)->mMorfStep;
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = progress;
    mDoExtMorf_chaseF(gabi::at<void>(o + 0xB0), one, delta);
  }
  mDoExtMorf_frameUpdate(gabi::at<void>(o + 0x98));
  u32 audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  if (audio && mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mpBasAnm &&
      gabi::ea(position)) {
    f32 frame = mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrame,
        rate = mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mRate;
    mDoExtMorf_animSound(gabi::at<void>(audio), position, sound, reverb, frame,
                         rate);
  }
  if (mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mState & 1)
    return 1;
  return mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mRate == l_zero ? 1 : 0;
}

/* 025E5434 */
void mDoExt_McaMorf::update() {

  u32 o = gabi::ea(this), model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  if (!model)
    return;
  u32 anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
  if (anim) {
    u32 frameBits = mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrameWord;
    mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrameWord = frameBits;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    mDoExtMorf_calcBind(this, gabi::at<void>(anim));
    u32 control = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpControl;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    u32 target = mDoExtMorf_at<mDoExtMorf::Control>(control)->mCallback;
    f32 second = mDoExtMorf_at<mDoExtMorf::Control>(control)->mSecond;
    u32 receiver = mDoExtMorf_at<mDoExtMorf::Control>(control)->mReceiver;
    f32 third = mDoExtMorf_at<mDoExtMorf::Control>(control)->mThird,
        first = mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrame;
    f32 result = gabi::call_ptr<f32>(target, gabi::at<void>(receiver), first,
                                     second, third);
    mDoExtMorf_at<mDoExtMorf::Control>(control)->mValue = result;
    model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  }
  u32 data = mDoExtMorf_at<mDoExtMorf::Model>(model)->mpModelData,
      root = mDoExtMorf_at<mDoExtMorf::AnimationData>(data)->mpRoot;
  mDoExtMorf_at<mDoExtMorf::Root>(root)->mpMtxCalc = o;
  model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  mDoExtMorf_modelUpdate(gabi::at<void>(model));
  f32 progress = mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = progress;
}

/* 025E54D8 */
void mDoExt_McaMorf::updateDL() {

  u32 o = gabi::ea(this), model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  if (!model)
    return;
  u32 anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
  if (anim) {
    u32 frameBits = mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrameWord;
    mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrameWord = frameBits;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    mDoExtMorf_calcBind(this, gabi::at<void>(anim));
    u32 control = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpControl;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    u32 target = mDoExtMorf_at<mDoExtMorf::Control>(control)->mCallback;
    f32 second = mDoExtMorf_at<mDoExtMorf::Control>(control)->mSecond;
    u32 receiver = mDoExtMorf_at<mDoExtMorf::Control>(control)->mReceiver;
    f32 third = mDoExtMorf_at<mDoExtMorf::Control>(control)->mThird,
        first = mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrame;
    f32 result = gabi::call_ptr<f32>(target, gabi::at<void>(receiver), first,
                                     second, third);
    mDoExtMorf_at<mDoExtMorf::Control>(control)->mValue = result;
    model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  }
  u32 data = mDoExtMorf_at<mDoExtMorf::Model>(model)->mpModelData,
      root = mDoExtMorf_at<mDoExtMorf::AnimationData>(data)->mpRoot;
  mDoExtMorf_at<mDoExtMorf::Root>(root)->mpMtxCalc = o;
  model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  mDoExtMorf_modelUpdateDL(gabi::at<void>(model), 0);
  f32 progress = mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = progress;
}

/* 025E55A0 */
void mDoExt_McaMorf::calc() {

  u32 o = gabi::ea(this), model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  if (!model)
    return;
  u32 anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
  if (anim) {
    u32 frameBits = mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrameWord;
    mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrameWord = frameBits;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    mDoExtMorf_calcBind(this, gabi::at<void>(anim));
    u32 control = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpControl;
    anim = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    u32 target = mDoExtMorf_at<mDoExtMorf::Control>(control)->mCallback;
    f32 second = mDoExtMorf_at<mDoExtMorf::Control>(control)->mSecond;
    u32 receiver = mDoExtMorf_at<mDoExtMorf::Control>(control)->mReceiver;
    f32 third = mDoExtMorf_at<mDoExtMorf::Control>(control)->mThird,
        first = mDoExtMorf_at<mDoExtMorf::Animation>(anim)->mFrame;
    f32 result = gabi::call_ptr<f32>(target, gabi::at<void>(receiver), first,
                                     second, third);
    mDoExtMorf_at<mDoExtMorf::Control>(control)->mValue = result;
    model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  }
  u32 data = mDoExtMorf_at<mDoExtMorf::Model>(model)->mpModelData,
      root = mDoExtMorf_at<mDoExtMorf::AnimationData>(data)->mpRoot;
  mDoExtMorf_at<mDoExtMorf::Root>(root)->mpMtxCalc = o;
  model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  mDoExtMorf_modelCalc(gabi::at<void>(model));
}

/* 025E5580 */
void mDoExt_McaMorf::entry() {
  u32 target = static_cast<mDoExt_McaMorf *>(this)->mpModel;
  if (target)
    mDoExtMorf_modelEntry(gabi::at<void>(target));
}

/* 025E5590 */
void mDoExt_McaMorf::entryDL() {
  u32 target = static_cast<mDoExt_McaMorf *>(this)->mpModel;
  if (target)
    mDoExtMorf_modelEntryDL(gabi::at<void>(target));
}

/* 025E563C */
void mDoExt_McaMorf::stopZelAnime() {
  u32 target = static_cast<mDoExt_McaMorf *>(this)->mpSound;
  if (target)
    mDoExtMorf_soundStop(gabi::at<void>(target));
}

/* 025E4A98 */
void mDoExt_McaMorf::setAnm(void *animation, s32 mode, void *bas, f32 morf,
                            f32 speed, f32 start, f32 end) {

  u32 o = gabi::ea(this), a = gabi::ea(animation), b = gabi::ea(bas);
  f32 zero = l_zero;
  s16 first = (s16)ftoi(start);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm = a;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mStart = first;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrame = (f32)first;
  s32 last;
  if (!(end < zero))
    last = (s16)ftoi(end);
  else {
    u32 current = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm;
    if (!current)
      last = 0;
    else {
      u32 table = mDoExtMorf_at<mDoExtMorf::Animation>(current)->mpVtable,
          target =
              mDoExtMorf_at<mDoExtMorf::AnimationVtable>(table)->mGetFrameMax,
          data = mDoExtMorf_at<mDoExtMorf::Animation>(a)->mpData,
          flags = mDoExtMorf_at<mDoExtMorf::AnimationData>(data)->mFlags;
      last = gabi::call_ptr<s32>(target, gabi::at<void>(current));
      if (flags & 4)
        last = (s16)(last - 1);
    }
  }
  mDoExtMorf_frameInit(gabi::at<void>(o + 0x98), last);
  if (a && mode < 0) {
    u32 table = mDoExtMorf_at<mDoExtMorf::Animation>(a)->mpVtable;
    mode = gabi::call_ptr<s32>(
        mDoExtMorf_at<mDoExtMorf::AnimationVtable>(table)->mGetAttribute,
        animation);
  }
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mRate = speed;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mAttribute = (u8)mode;
  s16 frame =
      speed < zero ? mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mEnd : first;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mFrame = (f32)frame;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mLoop = frame;
  mDoExtMorf_setMorfCall(this, morf);
  u32 audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  if (!audio)
    return;
  if (!b && a) {
    u32 data = mDoExtMorf_at<mDoExtMorf::Animation>(a)->mpData,
        relative = mDoExtMorf_at<mDoExtMorf::AnimationData>(data)->mNameOffset,
        name = relative ? data + 4 + relative : 0;
    gabi::Local<mDoExtMorf::SafeString> str;
    str->mData = name;
    str->mpVtable = gabi::ea(&l_stringVtable);
    b = mDoExtMorf_resource(gabi::at<void>(l_resources.mValue), str.get(),
                            gabi::at<void>(a + 0xC));
    audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  }
  mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mpBasAnm = b;
  audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  u32 agent = mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mpAgent,
      target = mDoExtMorf_at<mDoExtMorf::Agent>(agent)->mInit;
  if (!b) {
    gabi::call_ptr(target, gabi::at<void>(audio), 0, 1, zero);
    return;
  }
  s32 direction =
      mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mRate < zero ? -1 : 1;
  f32 current = (f32)mDoExtMorf_at<mDoExt_McaMorf>(o)->mFrameCtrl.mLoop;
  gabi::call_ptr(target, gabi::at<void>(audio), gabi::at<void>(b), direction,
                 current);
  gabi::Local<mDoExtMorf::SafeString> str;
  str->mData = gabi::ea(&l_mdName);
  str->mpVtable = gabi::ea(&l_stringVtable);
  mDoExtMorf_checkName(str.get());
  gabi::call_ptr(*mDoExtMorf_at<be<u32>>(str->mpVtable + 0x14), str.get());
  u32 name = str->mData;
  gabi::call_ptr(
      *mDoExtMorf_at<be<u32>>(
          mDoExtMorf_at<mDoExtMorf::Animation>(a)->mName.mpVtable + 0x14),
      gabi::at<void>(a + 0xC));
  u32 other = mDoExtMorf_at<mDoExtMorf::Animation>(a)->mName.mData;
  if (name != other) {
    for (u32 i = 0; i < 0x40001; i++) {
      u8 left = *mDoExtMorf_at<be<u8>>(name + i),
         right = *mDoExtMorf_at<be<u8>>(other + i);
      if (left != right)
        return;
      if (!left) {
        audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
        mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mIsMd = 1;
        return;
      }
    }
    return;
  }
  audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mIsMd = 1;
}

/* 025E4F64 */
void *mDoExt_McaMorf_create(void *self, void *data, void *cb1, void *cb2,
                            void *animation, s32 mode, s32 start, s32 end,
                            f32 speed) {

  u32 entrySP = cpu->r[1],
      enable = mDoExtMorf_at<mDoExtMorf::StackArgs>(entrySP)->mEnable,
      b = mDoExtMorf_at<mDoExtMorf::StackArgs>(entrySP)->mBasAnm,
      modelFlag = mDoExtMorf_at<mDoExtMorf::StackArgs>(entrySP)->mModelFlag,
      dlFlag = mDoExtMorf_at<mDoExtMorf::StackArgs>(entrySP)->mDlFlag;
  u32 o = gabi::ea(self), d = gabi::ea(data), a = gabi::ea(animation);
  if (!o) {
    o = mDoExtMorf_alloc(0xC8);
    if (!o)
      return nullptr;
  }
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpVtable = gabi::ea(&l_baseVtable);
  mDoExtMorf_baseConstruct(gabi::at<void>(o + 4));
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpVtable = gabi::ea(&l_morfVtable);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSecondaryVtable =
      gabi::ea(&l_secondaryVtable);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mBaseState = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpJoints = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mJointState = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mCalcState = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mAnmBuffer.mState = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpBufferMemory = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm = 0;
  mDoExtMorf_frameInit(gabi::at<void>(o + 0x98), 0);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpCallback1 = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpCallback2 = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpAnm = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpTransformInfo = 0;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpQuat = 0;
  f32 zero = l_zero;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mCurMorf = zero;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = zero;
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mMorfStep = zero;
  if (!d)
    return gabi::at<void>(o);
  u32 model = mDoExtMorf_modelCreate(data, modelFlag, dlFlag);
  mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel = model;
  if (!model)
    return gabi::at<void>(o);
  mDoExtMorf_bindModel(gabi::at<void>(o), data, animation);
  bool found = false;
  if (!b && a) {
    u32 ad = mDoExtMorf_at<mDoExtMorf::Animation>(a)->mpData,
        rel = mDoExtMorf_at<mDoExtMorf::AnimationData>(ad)->mNameOffset,
        name = rel ? ad + 4 + rel : 0;
    gabi::Local<mDoExtMorf::SafeString> str;
    str->mData = name;
    str->mpVtable = gabi::ea(&l_stringVtable);
    b = mDoExtMorf_resource(gabi::at<void>(l_resources.mValue), str.get(),
                            gabi::at<void>(a + 0xC));
    found = b != 0;
  }
  bool failed = false;
  if ((found || enable) && !mDoExtMorf_audioDisabled()) {
    u32 audio = mDoExtMorf_alloc(0xA0);
    if (audio) {
      mDoExtMorf_clear(gabi::at<void>(audio), 0xA0);
      mDoExtMorf_soundConstruct(gabi::at<void>(audio));
      mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mIsMd = 0;
      mDoExtMorf_at<mDoExtMorf::Sound>(audio)->mpAgent =
          gabi::ea(&l_soundAgent);
    }
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound = audio;
    if (!audio)
      failed = true;
  }
  if (!failed) {
    mDoExtMorf_setAnmCall(gabi::at<void>(o), animation, mode, gabi::at<void>(b),
                          zero, speed, (f32)start, (f32)end);
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mPrevMorf = l_initialPrevMorf;
    u32 joints = mDoExtMorf_joints(data),
        count = mDoExtMorf_at<mDoExtMorf::Joints>(joints)->mNum,
        trans = mDoExtMorf_allocArray(count << 5);
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpTransformInfo = trans;
    if (!trans)
      failed = true;
    if (!failed) {
      joints = mDoExtMorf_joints(data);
      count = mDoExtMorf_at<mDoExtMorf::Joints>(joints)->mNum;
      u32 quat = mDoExtMorf_allocArray(count << 4);
      mDoExtMorf_at<mDoExt_McaMorf>(o)->mpQuat = quat;
      if (!quat)
        failed = true;
      if (!failed) {
        mDoExtMorf_createBufferCall(gabi::at<void>(o));
        model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
        trans = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpTransformInfo;
        u32 md = mDoExtMorf_at<mDoExtMorf::Model>(model)->mpModelData;
        quat = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpQuat;
        joints = mDoExtMorf_joints(gabi::at<void>(md));
        count = mDoExtMorf_at<mDoExtMorf::Joints>(joints)->mNum;
        if (count) {
          f32 sx = l_defaultTransform.mScale.x,
              sy = l_defaultTransform.mScale.y,
              sz = l_defaultTransform.mScale.z,
              tx = l_defaultTransform.mTranslate.x,
              ty = l_defaultTransform.mTranslate.y,
              tz = l_defaultTransform.mTranslate.z;
          s16 rx = l_defaultTransform.mRotation.x,
              ry = l_defaultTransform.mRotation.y,
              rz = l_defaultTransform.mRotation.z;
          for (u32 i = 0; i < count; i++) {
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mScale.x = sx;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mScale.y = sy;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mScale.z = sz;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mTranslate.x = tx;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mTranslate.y = ty;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mTranslate.z = tz;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mRotation.x = rx;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mRotation.y = ry;
            mDoExtMorf_at<mDoExtMorf::Transform>(trans)->mRotation.z = rz;
            mDoExtMorf_EulerToQuat((s32)rx, (s32)ry, (s32)rz,
                                   gabi::at<void>(quat));
            trans += 32;
            quat += 16;
          }
        }
        mDoExtMorf_at<mDoExt_McaMorf>(o)->mpCallback1 = gabi::ea(cb1);
        mDoExtMorf_at<mDoExt_McaMorf>(o)->mpCallback2 = gabi::ea(cb2);
        return gabi::at<void>(o);
      }
    }
  }
  u32 audio = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound;
  if (audio) {
    mDoExtMorf_soundStop(gabi::at<void>(audio));
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpSound = 0;
  }
  u32 trans = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpTransformInfo,
      quat = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpQuat;
  if (trans)
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpTransformInfo = 0;
  if (quat)
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpQuat = 0;
  model = mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel;
  if (model) {
    gabi::call_ptr(*mDoExtMorf_at<be<u32>>(
                       mDoExtMorf_at<mDoExtMorf::Model>(model)->mpVtable + 0xC),
                   gabi::at<void>(model), 2);
    mDoExtMorf_at<mDoExt_McaMorf>(o)->mpModel = 0;
  }
  return gabi::at<void>(o);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 027F3F94 u32 mDoExtMorf_joints(void*)
ext 027E06E0 u32 mDoExtMorf_bufferSize(void*)
ext 025E3A34 u32 mDoExtMorf_allocAligned(u32, u32)
ext 027E063C void mDoExtMorf_bufferConstruct(void*, void*, void*, u32)
ext 0200F5C8 void mDoExtMorf_chaseF(void*, f32, f32)
ext 027F2FC4 void mDoExtMorf_frameUpdate(void*)
ext 0201BF08 void mDoExtMorf_animSound(void*, void*, u32, s32, f32, f32)
ext 027F363C void mDoExtMorf_calcBind(void*, void*)
ext 025E2DA8 void mDoExtMorf_modelUpdate(void*)
ext 025E2DE0 void mDoExtMorf_modelUpdateDL(void*, u32)
ext 027F4D5C void mDoExtMorf_modelCalc(void*)
ext 025E2E24 void mDoExtMorf_modelEntry(void*)
ext 025E2E5C void mDoExtMorf_modelEntryDL(void*)
ext 02801D3C void mDoExtMorf_soundStop(void*)
ext 027F2BC0 void mDoExtMorf_frameInit(void*, s32)
ext 025E4A54 void mDoExtMorf_setMorfCall(void*, f32)
ext 0260702C u32 mDoExtMorf_resource(void*, void*, void*)
ext 025EE5EC void mDoExtMorf_checkName(void*)
ext 0273AD10 u32 mDoExtMorf_alloc(u32)
ext 027DA984 void mDoExtMorf_baseConstruct(void*)
ext 025E38E0 u32 mDoExtMorf_modelCreate(void*, u32, u32)
ext 027F36FC void mDoExtMorf_bindModel(void*, void*, void*)
ext 025E20F8 u32 mDoExtMorf_audioDisabled()
ext 028F521C void mDoExtMorf_clear(void*, u32)
ext 02801DA0 void mDoExtMorf_soundConstruct(void*)
ext 025E4A98 void mDoExtMorf_setAnmCall(void*, void*, s32, void*, f32, f32, f32, f32)
ext 0273ADAC u32 mDoExtMorf_allocArray(u32)
ext 025E4ECC void mDoExtMorf_createBufferCall(void*)
ext 027ED2D0 void mDoExtMorf_EulerToQuat(s32, s32, s32, void*)
data l_zero 100586DC
data l_one 100586D4
data l_initialPrevMorf 10058A0C
data l_stringVtable 10058704
data l_mdName 10058A08
data l_resources 101F4F28
data l_baseVtable 1016E54C
data l_morfVtable 10058CC0
data l_secondaryVtable 1016D820
data l_soundAgent 1005871C
data l_defaultTransform 1016E464
*/
