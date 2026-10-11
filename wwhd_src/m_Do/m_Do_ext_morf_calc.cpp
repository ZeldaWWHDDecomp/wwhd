/* WWHD: SDK animation-buffer joint calculation; GameCube morph flow.
 * HD: callback order, guest scratch aliasing, word copies and PPC arithmetic are retained. */
#include "m_Do/m_Do_ext_morf2_calc.h"
using namespace mDoExtCalc;
using namespace gabi;
/* 025E5C2C */
void mDoExt_McaMorf_entryJoints(void *self, void *model) {
  u32 reference = static_cast<u32>(view<ModelContext>(ea(model))->mpReference);
  entryBuffer(view<void>(ea(self) + 0x78), view<void>(reference));
}
/* 025E5C38 */
void mDoExt_McaMorf_calcContext(void *self, void *model) {
  u32 reference = static_cast<u32>(view<ModelContext>(ea(model))->mpReference);
  (view<Reference>(reference)->mpMtxCalc = ea(self));
  reference = static_cast<u32>(view<ModelContext>(ea(model))->mpReference);
  (view<Reference>(reference)->mIndex = 0);
  calcMorfJoints(self, model);
  entryMorfJoints(self, model);
}
/* 025E5C90 */
void mDoExt_McaMorf2_ERROR_EXIT(void *self) {
  u32 o = ea(self), audio = static_cast<u32>(view<Morf2>(o)->mpSound);
  if (audio) {
    stopZelAnime(view<void>(audio));
    (view<Morf2>(o)->mpSound = 0);
  }
  u32 transforms = static_cast<u32>(view<Morf2>(o)->mpTransformInfo),
      quats = static_cast<u32>(view<Morf2>(o)->mpQuat);
  if (transforms)
    (view<Morf2>(o)->mpTransformInfo = 0);
  u32 model = static_cast<u32>(view<Morf2>(o)->mpModel);
  if (quats)
    (view<Morf2>(o)->mpQuat = 0);
  if (model) {
    call_ptr(static_cast<u32>(
                 view<Vtable>(static_cast<u32>(view<Model>(model)->mpVtable))
                     ->mExecute),
             view<void>(model), 2);
    (view<Morf2>(o)->mpModel = 0);
  }
}
/* 025E5D1C */
void mDoExt_McaMorf2_setMorf(void *self, f32 duration) {
  u32 o = ea(self);
  f32 zero = static_cast<f32>(constants()->mZero),
      one = static_cast<f32>(constants()->mOne);
  if (!(duration > zero)) {
    (view<Morf2>(o)->mCurMorf = one);
    (view<Morf2>(o)->mPrevMorf = one);
    return;
  }
  f32 delta = one / duration;
  (view<Morf2>(o)->mCurMorf = zero);
  (view<Morf2>(o)->mPrevMorf = zero);
  (view<Morf2>(o)->mMorfStep = delta);
}
/* 025E564C */
void mDoExt_McaMorf_calcJoints(void *self, void *model) {
  u32 o = ea(self),
      reference = static_cast<u32>(view<ModelContext>(ea(model))->mpReference),
      joints = static_cast<u32>(view<Reference>(reference)->mpJoints);
  clearBuffer(view<void>(o + 0x78));
  f32 one = static_cast<f32>(constants()->mOne);
  if (static_cast<u32>(view<Morf>(o)->mpModel) &&
      static_cast<u32>(view<Morf>(o)->mpAnm)) {
    (currentMtxCalc() = o);
    u32 animation = static_cast<u32>(view<Morf>(o)->mpAnm);
    bindAnimation(self, view<void>(animation));
    u32 control = static_cast<u32>(view<Morf>(o)->mpControl);
    animation = static_cast<u32>(view<Morf>(o)->mpAnm);
    u32 target = static_cast<u32>(view<Control>(control)->mCallback),
        receiver = static_cast<u32>(view<Control>(control)->mReceiver);
    f32 second = static_cast<f32>(view<Control>(control)->mSecond),
        third = static_cast<f32>(view<Control>(control)->mThird),
        first = static_cast<f32>(view<Animation>(animation)->mFrame);
    f32 result =
        call_ptr<f32>(target, view<void>(receiver), first, second, third);
    (view<Control>(control)->mValue = result);
    blendBuffer(view<void>(o + 0x78), view<void>(o + 4), one);
  }
  (view<Morf>(o)->mBuffer.mFlags =
       static_cast<u32>(view<Morf>(o)->mBuffer.mFlags) | BUFFER_DIRTY);
  u32 count = static_cast<u16>(view<Joints>(joints)->mNum),
      record = static_cast<u32>(view<Morf>(o)->mBuffer.mpRecords);
  f32 zero = static_cast<f32>(constants()->mZero);
  Local<u8[96]> workspace;
  // HD: GHS scratch offsets include the caller linkage area.
  u32 frame = ea(workspace.get()) - 8;
  for (u32 i = 0; i < count; i++, record += 0x38) {
    u32 transforms = static_cast<u32>(view<Morf>(o)->mpTransformInfo),
        quats = static_cast<u32>(view<Morf>(o)->mpQuat),
        info = transforms ? transforms + i * 32 : frame + 0x48,
        quat = quats ? quats + i * 16 : frame + 0x38;
    u32 before = 0, after = 0, scale = 0, translation = 0;
    if (!static_cast<u32>(view<Morf>(o)->mpAnm)) {
      u32 currentModel = static_cast<u32>(view<Morf>(o)->mpModel),
          data = static_cast<u32>(view<Model>(currentModel)->mpData),
          num = static_cast<u32>(view<ModelData>(data)->mNum),
          joint = static_cast<u32>(view<ModelData>(data)->mpJoints),
          index = i & 0xFFFF;
      if (index < num)
        joint += index * 0x1C;
      u32 source = getTransformInfo(view<void>(joint));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(info)->mScale.value[j] =
             static_cast<f32>(view<Transform>(source)->mScale.value[j]));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(info)->mRotation[j] =
             static_cast<s16>(view<Transform>(source)->mRotation[j]));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(info)->mTranslate.value[j] =
             static_cast<f32>(view<Transform>(source)->mTranslate.value[j]));
      scale = record + 4;
      translation = record + 0x10;
      before = static_cast<u32>(view<Morf>(o)->mpCallback1);
      if (before)
        call_ptr(
            static_cast<u32>(
                view<Vtable>(static_cast<u32>(view<Callback>(before)->mpVtable))
                    ->mExecute),
            view<void>(before), index, view<void>(info));
      JMAEulerToQuat(
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[0]),
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[1]),
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[2]),
          view<void>(quat));
      after = static_cast<u32>(view<Morf>(o)->mpCallback2);
    } else {
      f32 progress = static_cast<f32>(view<Morf>(o)->mCurMorf);
      bool blend = progress < one && transforms && quats;
      u32 flags = static_cast<u32>(view<Morf>(o)->mFlags),
          matrix = static_cast<u32>(view<Morf>(o)->mpMatrices) + i * 0x30;
      (view<Morf>(o)->mFlags = flags | BUFFER_DIRTY);
      u32 temp = blend ? frame + 8 : info;
      for (u32 j = 0; j < 3; j++) {
        s32 angle =
            radianToAngle(static_cast<f32>(view<Matrix>(matrix)->mRotation[j]));
        (view<Transform>(temp)->mRotation[j] = (s16)angle);
      }
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(temp)->mScale.words[j] =
             static_cast<u32>(view<Record>(record)->mScale.words[j]));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(temp)->mTranslate.words[j] =
             static_cast<u32>(view<Record>(record)->mTranslate.words[j]));
      scale = record + 4;
      translation = record + 0x10;
      before = static_cast<u32>(view<Morf>(o)->mpCallback1);
      if (before)
        call_ptr(
            static_cast<u32>(
                view<Vtable>(static_cast<u32>(view<Callback>(before)->mpVtable))
                    ->mExecute),
            view<void>(before), i & 0xFFFF, view<void>(temp));
      if (blend) {
        f32 previous = static_cast<f32>(view<Morf>(o)->mPrevMorf),
            current = static_cast<f32>(view<Morf>(o)->mCurMorf),
            ratio = fsubs_ppc(current, previous) / fsubs_ppc(one, previous),
            oldRatio = fsubs_ppc(one, ratio);
        JMAEulerToQuat(
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[0]),
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[1]),
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[2]),
            view<void>(frame + 0x28));
        JMAQuatLerp(view<void>(quat), view<void>(frame + 0x28),
                    view<void>(quat), ratio);
        for (u32 j = 0; j < 3; j++) {
          f32 fresh =
                  static_cast<f32>(view<Transform>(temp)->mTranslate.value[j]),
              old =
                  static_cast<f32>(view<Transform>(info)->mTranslate.value[j]);
          (view<Transform>(info)->mTranslate.value[j] =
               fmadds(old, oldRatio, fmuls_ppc(fresh, ratio)));
        }
        for (u32 j = 0; j < 3; j++) {
          f32 fresh = static_cast<f32>(view<Transform>(temp)->mScale.value[j]),
              old = static_cast<f32>(view<Transform>(info)->mScale.value[j]);
          (view<Transform>(info)->mScale.value[j] =
               fmadds(old, oldRatio, fmuls_ppc(fresh, ratio)));
        }
      } else
        JMAEulerToQuat(
            (s32) static_cast<s16>(view<Transform>(info)->mRotation[0]),
            (s32) static_cast<s16>(view<Transform>(info)->mRotation[1]),
            (s32) static_cast<s16>(view<Transform>(info)->mRotation[2]),
            view<void>(quat));
      after = static_cast<u32>(view<Morf>(o)->mpCallback2);
    }
    if (after)
      call_ptr(
          static_cast<u32>(
              view<Vtable>(static_cast<u32>(view<Callback>(after)->mpVtable))
                  ->mExecute),
          view<void>(after), i & 0xFFFF);
    f32 w = static_cast<f32>(view<QuatView>(quat)->value[3]),
        x = static_cast<f32>(view<QuatView>(quat)->value[0]),
        y = static_cast<f32>(view<QuatView>(quat)->value[1]),
        z = static_cast<f32>(view<QuatView>(quat)->value[2]),
        dw = fadds_ppc(w, w), dx = fadds_ppc(x, x), dy = fadds_ppc(y, y),
        dz = fadds_ppc(z, z);
    f32 yy = fnmsubs(dy, y, one), xy = fmuls_ppc(dx, y), yz = fmuls_ppc(dy, z),
        wy = fmuls_ppc(dw, y), xx = fnmsubs(dx, x, one), zz = fmuls_ppc(dz, z),
        wz = fmuls_ppc(dw, z);
    (view<Record>(record)->mBasisX.value[2] = fmadds(dx, z, wy));
    (view<Record>(record)->mBasisX.value[1] = fsubs_ppc(xy, wz));
    (view<Record>(record)->mBasisX.value[0] = fsubs_ppc(yy, zz));
    (view<Record>(record)->mBasisY.value[0] = fadds_ppc(xy, wz));
    (view<Record>(record)->mBasisY.value[1] = fsubs_ppc(xx, zz));
    (view<Record>(record)->mBasisY.value[2] = fnmsubs(dw, x, yz));
    f32 sc[3] = {static_cast<f32>(view<Transform>(info)->mScale.value[0]),
                 static_cast<f32>(view<Transform>(info)->mScale.value[1]),
                 static_cast<f32>(view<Transform>(info)->mScale.value[2])};
    (view<Vector>(scale)->value[0] = sc[0]);
    (view<Vector>(scale)->value[2] = sc[2]);
    (view<Vector>(scale)->value[1] = sc[1]);
    f32 tr[3] = {static_cast<f32>(view<Transform>(info)->mTranslate.value[0]),
                 static_cast<f32>(view<Transform>(info)->mTranslate.value[1]),
                 static_cast<f32>(view<Transform>(info)->mTranslate.value[2])};
    (view<Vector>(translation)->value[0] = tr[0]);
    (view<Vector>(translation)->value[1] = tr[1]);
    (view<Vector>(translation)->value[2] = tr[2]);
    if (static_cast<f32>(view<QuatView>(quat)->value[0]) != zero ||
        static_cast<f32>(view<QuatView>(quat)->value[1]) != zero ||
        static_cast<f32>(view<QuatView>(quat)->value[2]) != zero ||
        static_cast<f32>(view<QuatView>(quat)->value[3]) != one)
      (view<Record>(record)->mFlags =
           static_cast<u32>(view<Record>(record)->mFlags) | ROTATION_DIRTY);
    if (static_cast<f32>(view<Transform>(info)->mScale.value[0]) != one ||
        static_cast<f32>(view<Transform>(info)->mScale.value[1]) != one ||
        static_cast<f32>(view<Transform>(info)->mScale.value[2]) != one)
      (view<Record>(record)->mFlags =
           static_cast<u32>(view<Record>(record)->mFlags) | SCALE_DIRTY);
    if (static_cast<f32>(view<Transform>(info)->mTranslate.value[0]) != zero ||
        static_cast<f32>(view<Transform>(info)->mTranslate.value[1]) != zero ||
        static_cast<f32>(view<Transform>(info)->mTranslate.value[2]) != zero)
      (view<Record>(record)->mFlags =
           static_cast<u32>(view<Record>(record)->mFlags) | TRANSLATION_DIRTY);
    count = static_cast<u16>(view<Joints>(joints)->mNum);
  }
}

/* WWHD glue (tools/verify/autoglue.py): no TU-wide static objects. */
