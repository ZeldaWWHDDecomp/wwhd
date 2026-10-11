/* WWHD: SDK animation-buffer joint calculation; GameCube morph flow.
 * HD: callback order, guest scratch aliasing, word copies and PPC arithmetic are retained. */
#include "m_Do/m_Do_ext_morf2_calc.h"
using namespace mDoExtCalc;
using namespace gabi;
static void normalizeRecordBasis(u32 record, u32 frame, u32 basis) {
  f32 x = static_cast<f32>(view<Record>(record)->mBasisX.value[0]),
      y = static_cast<f32>(view<Record>(record)->mBasisX.value[1]),
      z = static_cast<f32>(view<Record>(record)->mBasisX.value[2]);
  f32 a = static_cast<f32>(view<Record>(record)->mBasisY.value[0]),
      b = static_cast<f32>(view<Record>(record)->mBasisY.value[1]),
      c = static_cast<f32>(view<Record>(record)->mBasisY.value[2]);
  f32 cross[3] = {fmsubs(y, c, fmuls_ppc(z, b)), fmsubs(z, a, fmuls_ppc(x, c)),
                  fmsubs(x, b, fmuls_ppc(y, a))};
  f32 lenA = fmadds(c, c, fadds_ppc(fmuls_ppc(a, a), fmuls_ppc(b, b)));
  f32 lenB = fmadds(
      cross[2], cross[2],
      fadds_ppc(fmuls_ppc(cross[0], cross[0]), fmuls_ppc(cross[1], cross[1])));
  if (lenA != 0 && lenB != 0) {
    f32 ra = (f32)frsqrte(lenA), rb = (f32)frsqrte(lenB),
        half = static_cast<f32>(constants()->mHalf),
        three = static_cast<f32>(constants()->mThree);
    ra =
        fmuls_ppc(ra, fnmsubs(fmuls_ppc(fmuls_ppc(lenA, half), ra), ra, three));
    rb =
        fmuls_ppc(rb, fnmsubs(fmuls_ppc(fmuls_ppc(lenB, half), rb), rb, three));
    f32 v[3] = {fmuls_ppc(a, ra), fmuls_ppc(b, ra), fmuls_ppc(c, ra)};
    f32 w[3] = {fmuls_ppc(cross[0], rb), fmuls_ppc(cross[1], rb),
                fmuls_ppc(cross[2], rb)};
    for (u32 i = 0; i < 3; i++) {
      (view<BasisMatrix>(frame + basis)->row[1][i] = v[i]);
      (view<BasisMatrix>(frame + basis)->row[2][i] = w[i]);
    }
    (view<BasisMatrix>(frame + basis)->row[0][0] =
         fmsubs(v[1], w[2], fmuls_ppc(v[2], w[1])));
    (view<BasisMatrix>(frame + basis)->row[0][1] =
         fmsubs(v[2], w[0], fmuls_ppc(v[0], w[2])));
    (view<BasisMatrix>(frame + basis)->row[0][2] =
         fmsubs(v[0], w[1], fmuls_ppc(v[1], w[0])));
  }
}
/* 025E67A4 */
void mDoExt_McaMorf2_calcJoints(void *self, void *model) {
  u32 o = ea(self),
      reference = static_cast<u32>(view<ModelContext>(ea(model))->mpReference),
      joints = static_cast<u32>(view<Reference>(reference)->mpJoints);
  clearBuffer(view<void>(o + 0x78));
  f32 zero = static_cast<f32>(constants()->mZero),
      one = static_cast<f32>(constants()->mOne);
  u32 anim[2] = {static_cast<u32>(view<Morf2>(o)->mpAnm1),
                 static_cast<u32>(view<Morf2>(o)->mpAnm2)};
  Local<u8[304]> workspace;
  // HD: GHS scratch offsets include the caller linkage area.
  u32 frame = ea(workspace.get()) - 8;
  if (static_cast<u32>(view<Morf2>(o)->mpModel) && anim[0]) {
    f32 mix = static_cast<f32>(view<Morf2>(o)->mAnmRate);
    (view<Morf2Scratch>(frame)->mWeights[0] =
         anim[1] ? fsubs_ppc(one, mix) : one);
    (view<Morf2Scratch>(frame)->mWeights[1] = anim[1] ? mix : zero);
    for (u32 n = 0; n < 2 && anim[n]; n++) {
      (currentMtxCalc() = o);
      bindAnimation(self, view<void>(anim[n]));
      u32 c = static_cast<u32>(view<Morf2>(o)->mpControl);
      f32 v = call_ptr<f32>(
          static_cast<u32>(view<Control>(c)->mCallback),
          view<void>(static_cast<u32>(view<Control>(c)->mReceiver)),
          static_cast<f32>(view<Animation>(anim[n])->mFrame),
          static_cast<f32>(view<Control>(c)->mSecond),
          static_cast<f32>(view<Control>(c)->mThird));
      (view<Control>(c)->mValue = v);
      blendBuffer(view<void>(o + 0x78), view<void>(o + 4),
                  static_cast<f32>(view<Morf2Scratch>(frame)->mWeights[n]));
    }
  }
  (view<Morf2>(o)->mBuffer.mFlags =
       static_cast<u32>(view<Morf2>(o)->mBuffer.mFlags) | BUFFER_DIRTY);
  u32 count = static_cast<u16>(view<Joints>(joints)->mNum),
      record = static_cast<u32>(view<Morf2>(o)->mBuffer.mpRecords);
  for (u32 i = 0; i < count; i++, record += 0x38) {
    u32 transforms = static_cast<u32>(view<Morf2>(o)->mpTransformInfo),
        quats = static_cast<u32>(view<Morf2>(o)->mpQuat),
        info = transforms ? transforms + i * 32 : frame + 0x110,
        quat = quats ? quats + i * 16 : frame + 0x100, scale = record + 4,
        translation = record + 0x10;
    if (!static_cast<u32>(view<Morf2>(o)->mpAnm1)) {
      u32 data = static_cast<u32>(
              view<Model>(static_cast<u32>(view<Morf2>(o)->mpModel))->mpData),
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
      u32 before = static_cast<u32>(view<Morf2>(o)->mpCallback1);
      if (before)
        call_ptr(
            static_cast<u32>(
                view<Vtable>(static_cast<u32>(view<Callback>(before)->mpVtable))
                    ->mExecute),
            view<void>(before), index, view<void>(info), (f32)cpu->f[1].ps0,
            (f32)cpu->f[2].ps0, (f32)cpu->f[3].ps0);
      JMAEulerToQuat(
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[0]),
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[1]),
          (s32) static_cast<s16>(view<Transform>(info)->mRotation[2]),
          view<void>(quat));
    } else {
      bool blend = static_cast<f32>(view<Morf2>(o)->mCurMorf) < one &&
                   transforms && quats &&
                   !static_cast<u8>(view<Morf2>(o)->mHDEnabled);
      bool second = static_cast<u32>(view<Morf2>(o)->mpAnm2) != 0;
      u32 temp = blend ? frame + 8 : info;
      if (!second) {
        u32 flags = static_cast<u32>(view<Morf2>(o)->mFlags),
            matrix = static_cast<u32>(view<Morf2>(o)->mpMatrices) + i * 0x30;
        (view<Morf2>(o)->mFlags = flags | BUFFER_DIRTY);
        for (u32 j = 0; j < 3; j++) {
          s32 angle = radianToAngle(
              static_cast<f32>(view<Matrix>(matrix)->mRotation[j]));
          (view<Transform>(temp)->mRotation[j] = (s16)angle);
        }
        for (u32 j = 0; j < 3; j++)
          (view<Transform>(temp)->mScale.words[j] =
               static_cast<u32>(view<Record>(record)->mScale.words[j]));
        for (u32 j = 0; j < 3; j++)
          (view<Transform>(temp)->mTranslate.words[j] =
               static_cast<u32>(view<Record>(record)->mTranslate.words[j]));
        u32 before = static_cast<u32>(view<Morf2>(o)->mpCallback1);
        if (before)
          call_ptr(static_cast<u32>(
                       view<Vtable>(
                           static_cast<u32>(view<Callback>(before)->mpVtable))
                           ->mExecute),
                   view<void>(before), i & 0xFFFF, view<void>(temp),
                   (f32)cpu->f[1].ps0, (f32)cpu->f[2].ps0, (f32)cpu->f[3].ps0);
        JMAEulerToQuat(
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[0]),
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[1]),
            (s32) static_cast<s16>(view<Transform>(temp)->mRotation[2]),
            view<void>(blend ? frame + 0x28 : quat));
      } else {
        u32 basis = blend ? 0x78 : 0x68;
        normalizeRecordBasis(record, frame, basis);
        for (u32 j = 0; j < 12; j++)
          (view<Morf2Scratch>(frame)->mMatrix[j] =
               static_cast<f32>(view<be<f32>>(frame + basis)[j]));
        u32 out = blend ? frame + 0x68 : frame + 8;
        mDoMtx_matrixToQuat(view<void>(out), view<void>(frame + 0xC8));
        for (u32 j = 0; j < 4; j++)
          (view<QuatView>(blend ? frame + 0x28 : quat)->value[j] =
               static_cast<f32>(view<QuatView>(out)->value[j]));
        for (u32 j = 0; j < 3; j++)
          (view<Transform>(temp)->mScale.words[j] =
               static_cast<u32>(view<Record>(record)->mScale.words[j]));
        for (u32 j = 0; j < 3; j++)
          (view<Transform>(temp)->mTranslate.words[j] =
               static_cast<u32>(view<Record>(record)->mTranslate.words[j]));
      }
      if (blend) {
        f32 previous = static_cast<f32>(view<Morf2>(o)->mPrevMorf),
            current = static_cast<f32>(view<Morf2>(o)->mCurMorf),
            ratio = fsubs_ppc(current, previous) / fsubs_ppc(one, previous),
            oldRatio = fsubs_ppc(one, ratio);
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
      }
    }
    u32 after = static_cast<u32>(view<Morf2>(o)->mpCallback2);
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
    // HD: callback FP live-outs are observable in the next joint.
    cpu->f[2].ps0 = zz;
    cpu->f[3].ps0 = xy;
    count = static_cast<u16>(view<Joints>(joints)->mNum);
  }
  (view<Morf2>(o)->mHDEnabled = 0);
}
/* 025E7270 */
void mDoExt_McaMorf2_entryJoints(void *self, void *model) {
  entryBuffer(
      view<void>(ea(self) + 0x78),
      view<void>(static_cast<u32>(view<ModelContext>(ea(model))->mpReference)));
}
/* 025E727C */
void mDoExt_McaMorf2_calcContext(void *self, void *model) {
  u32 r = static_cast<u32>(view<ModelContext>(ea(model))->mpReference);
  (view<Reference>(r)->mpMtxCalc = ea(self));
  r = static_cast<u32>(view<ModelContext>(ea(model))->mpReference);
  (view<Reference>(r)->mIndex = 0);
  calcMorf2Joints(self, model);
  entryMorf2Joints(self, model);
}

/* WWHD glue (tools/verify/autoglue.py): no TU-wide static objects. */
