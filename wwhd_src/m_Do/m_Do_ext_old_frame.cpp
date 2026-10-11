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
/* 025E4944 */
void mDoExt_MtxCalcAnmBlendTblOld_nextJoint(void *self, void *iterator) {
  u32 o = ea(self), it = ea(iterator),
      node = static_cast<u32>(view<Iterator>(it)->mpNode),
      old = static_cast<u32>(view<OldBlend>(o)->mpOldFrame),
      index = static_cast<u32>(view<Node>(node)->mIndex),
      target = static_cast<u32>(view<OldBlend>(o)->mAfter);
  u32 quats = static_cast<u32>(view<OldFrame>(old)->mpQuat),
      trans = static_cast<u32>(view<OldFrame>(old)->mpTransformInfo),
      receiver = static_cast<u32>(view<OldBlend>(o)->mUserArea);
  call_ptr(target, view<void>(receiver), index & 0xFFFF,
           view<void>(trans + (index << 5)), view<void>(quats + (index << 4)));
  u32 model = static_cast<u32>(view<OldBlend>(o)->mpModel),
      data = static_cast<u32>(view<ModelContext>(model)->mpReference),
      count = static_cast<u16>(view<Reference>(data)->mNum), next = index + 1;
  if (s32(next) < s32(count)) {
    (view<Iterator>(it)->mNext = next);
    return;
  }
  (view<Iterator>(it)->mNext = 0xFFFFFFFF);
  old = static_cast<u32>(view<OldBlend>(o)->mpOldFrame);
  if (!static_cast<u8>(view<OldFrame>(old)->mValid))
    (view<OldFrame>(old)->mValid = 1);
}
/* 025E4A10 */
void mDoExt_MtxCalcAnmBlendTblOld_calcContext(void *self, void *argument) {
  blendOldJoints(self, argument);
  storeOldJoints(self, argument);
}
/* 025E410C */
void mDoExt_MtxCalcAnmBlendTblOld_calcJoints(void *self, void *unused) {
  u32 o = ea(self);
  clearBuffer(view<void>(o + 0x8C));
  s32 count = static_cast<s32>(view<OldBlend>(o)->mCount);
  for (s32 i = 0; i < count; i++) {
    u32 entry = static_cast<u32>(view<OldBlend>(o)->mpEntries) + u32(i) * 16;
    if (static_cast<u32>(view<BlendEntry>(entry)->mEnabled)) {
      u32 buffers = static_cast<u32>(view<OldBlend>(o)->mpBuffers),
          data = static_cast<u32>(view<BlendEntry>(entry)->mpData);
      blendAnimationEntry(view<void>(o + 0x8C),
                          view<void>(buffers + u32(i) * 0x68),
                          view<void>(data));
      count = static_cast<s32>(view<OldBlend>(o)->mCount);
    }
  }
  u32 flags = static_cast<u32>(view<OldBlend>(o)->mBuffer.mFlags),
      n = static_cast<u16>(view<OldBlend>(o)->mBuffer.mNum);
  (view<OldBlend>(o)->mBuffer.mFlags = flags | BUFFER_DIRTY);
  u32 base = static_cast<u32>(view<OldBlend>(o)->mBuffer.mpRecords);
  Local<u8[196]> workspace;
  // HD: GHS scratch offsets include the caller linkage area.
  u32 frame = ea(workspace.get()) - 8;
  f32 zero = static_cast<f32>(constants()->mZero),
      one = static_cast<f32>(constants()->mOne),
      epsilon = static_cast<f32>(constants()->mEpsilon);
  for (u32 i = 0; i < n; i++) {
    u32 record = base + i * 0x38;
    if (!(static_cast<f32>(view<Record>(record)->mWeight) < epsilon)) {
      u32 old = static_cast<u32>(view<OldBlend>(o)->mpOldRecords);
      copyRecord(view<void>(old + i * 0x38), view<void>(record), 0x38);
      normalizeRecordBasis(record, frame, 0x60);
      for (u32 j = 0; j < 12; j++)
        (view<OldBlendScratch>(frame)->mMatrix[j] =
             static_cast<f32>(view<OldBlendScratch>(frame)->mBasis[j]));
      mDoMtx_matrixToQuat(view<void>(frame + 0x50), view<void>(frame + 0x9C));
      for (u32 j = 0; j < 4; j++)
        (view<OldBlendScratch>(frame)->mQuat.value[j] = static_cast<f32>(
             view<OldBlendScratch>(frame)->mConvertedQuat.value[j]));
      for (u32 j = 0; j < 3; j++)
        (view<OldBlendScratch>(frame)->mScale.value[j] = zero);
      for (u32 j = 0; j < 3; j++)
        (view<OldBlendScratch>(frame)->mScale.value[j] =
             static_cast<f32>(view<Record>(record)->mScale.value[j]));
      for (u32 j = 0; j < 3; j++)
        (view<OldBlendScratch>(frame)->mTranslate.value[j] = zero);
      for (u32 j = 0; j < 3; j++)
        (view<OldBlendScratch>(frame)->mTranslate.value[j] =
             static_cast<f32>(view<Record>(record)->mTranslate.value[j]));
      u32 oldframe = static_cast<u32>(view<OldBlend>(o)->mpOldFrame);
      f32 priorRatio = 0, newRatio = 0;
      if (static_cast<u8>(view<OldFrame>(oldframe)->mValid) &&
          static_cast<f32>(view<OldFrame>(oldframe)->mMorf) > zero &&
          static_cast<f32>(view<Record>(record)->mWeight) > zero) {
        priorRatio = static_cast<f32>(view<OldFrame>(oldframe)->mMorf);
        newRatio = fsubs_ppc(one, priorRatio);
        u32 quats = static_cast<u32>(view<OldFrame>(oldframe)->mpQuat),
            trans = static_cast<u32>(view<OldFrame>(oldframe)->mpTransformInfo),
            previous = trans + i * 0x20;
        JMAQuatLerp(view<void>(quats + i * 16), view<void>(frame + 0x28),
                    view<void>(frame + 0x28), newRatio);
        for (u32 j = 0; j < 3; j++) {
          f32 v = static_cast<f32>(
                  view<Transform>(previous)->mTranslate.value[j]),
              temp = static_cast<f32>(
                  view<OldBlendScratch>(frame)->mTranslate.value[j]);
          (view<OldBlendScratch>(frame)->mTranslate.value[j] =
               fmadds(temp, newRatio, fmuls_ppc(v, priorRatio)));
        }
        for (u32 j = 0; j < 3; j++) {
          f32 v = static_cast<f32>(view<Transform>(previous)->mScale.value[j]),
              temp = static_cast<f32>(
                  view<OldBlendScratch>(frame)->mScale.value[j]);
          (view<OldBlendScratch>(frame)->mScale.value[j] =
               fmadds(temp, newRatio, fmuls_ppc(v, priorRatio)));
        }
      }
      u32 target = static_cast<u32>(view<OldBlend>(o)->mBefore),
          receiver = static_cast<u32>(view<OldBlend>(o)->mUserArea);
      call_ptr(target, view<void>(receiver), i & 0xFFFF, view<void>(frame + 8),
               view<void>(frame + 0x28));
      f32 w = static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[3]),
          x = static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[0]),
          z = static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[2]),
          y = static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[1]);
      f32 dw = fadds_ppc(w, w), dx = fadds_ppc(x, x), dz = fadds_ppc(z, z),
          dy = fadds_ppc(y, y);
      f32 a = fnmsubs(dy, y, one), b = fmuls_ppc(dx, y), c = fmuls_ppc(dy, z),
          d = fmuls_ppc(dw, y), e = fnmsubs(dx, x, one), f = fmuls_ppc(dz, z),
          g = fmuls_ppc(dw, z);
      (view<Record>(record)->mBasisX.value[0] = fsubs_ppc(a, f));
      (view<Record>(record)->mBasisX.value[1] = fsubs_ppc(b, g));
      (view<Record>(record)->mBasisX.value[2] = fmadds(dx, z, d));
      (view<Record>(record)->mBasisY.value[1] = fsubs_ppc(e, f));
      (view<Record>(record)->mBasisY.value[2] = fnmsubs(dw, x, c));
      (view<Record>(record)->mBasisY.value[0] = fadds_ppc(b, g));
      f32 sc[3] = {
          static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[0]),
          static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[1]),
          static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[2])};
      for (u32 j = 0; j < 3; j++)
        (view<Record>(record)->mScale.value[j] = sc[j]);
      f32 tr[3] = {
          static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[0]),
          static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[1]),
          static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[2])};
      (view<Record>(record)->mTranslate.value[0] = tr[0]);
      (view<Record>(record)->mTranslate.value[2] = tr[2]);
      (view<Record>(record)->mTranslate.value[1] = tr[1]);
      if (static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[0]) !=
              zero ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[1]) !=
              zero ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[2]) !=
              zero ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mQuat.value[3]) != one)
        (view<Record>(record)->mFlags =
             static_cast<u32>(view<Record>(record)->mFlags) | ROTATION_DIRTY);
      if (static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[0]) !=
              one ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[1]) !=
              one ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mScale.value[2]) !=
              one)
        (view<Record>(record)->mFlags =
             static_cast<u32>(view<Record>(record)->mFlags) | SCALE_DIRTY);
      if (static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[0]) !=
              zero ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[1]) !=
              zero ||
          static_cast<f32>(view<OldBlendScratch>(frame)->mTranslate.value[2]) !=
              zero)
        (view<Record>(record)->mFlags =
             static_cast<u32>(view<Record>(record)->mFlags) |
             TRANSLATION_DIRTY);
    }
    n = static_cast<u16>(view<OldBlend>(o)->mBuffer.mNum);
  }
}
/* 025E46B4 */
void mDoExt_MtxCalcAnmBlendTblOld_storeJoints(void *self, void *data) {
  u32 o = ea(self), old = static_cast<u32>(view<OldBlend>(o)->mpOldFrame),
      flags = static_cast<u32>(view<OldBlend>(o)->mBuffer.mFlags),
      records = static_cast<u32>(view<OldBlend>(o)->mBuffer.mpRecords),
      n = static_cast<u16>(view<OldBlend>(o)->mBuffer.mNum);
  (view<OldBlend>(o)->mBuffer.mFlags = flags | BUFFER_DIRTY);
  u32 trans = static_cast<u32>(view<OldFrame>(old)->mpTransformInfo),
      quats = static_cast<u32>(view<OldFrame>(old)->mpQuat);
  f32 zero = static_cast<f32>(constants()->mZero);
  Local<u8[148]> workspace;
  // HD: GHS scratch offsets include the caller linkage area.
  u32 frame = ea(workspace.get()) - 8;
  for (u32 i = 0; i < n; i++) {
    u32 record = records + i * 0x38;
    if (static_cast<f32>(view<Record>(record)->mWeight) > zero) {
      normalizeRecordBasis(record, frame, 0x30);
      for (u32 j = 0; j < 12; j++)
        (view<OldStoreScratch>(frame)->mMatrix[j] =
             static_cast<f32>(view<OldStoreScratch>(frame)->mBasis[j]));
      mDoMtx_matrixToQuat(view<void>(frame + 0x20), view<void>(frame + 0x6C));
      for (u32 j = 0; j < 4; j++)
        (view<QuatView>(quats)[i].value[j] =
             static_cast<f32>(view<OldStoreScratch>(frame)->mQuat.value[j]));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(trans)[i].mScale.value[j] =
             static_cast<f32>(view<Record>(record)->mScale.value[j]));
      for (u32 j = 0; j < 3; j++)
        (view<Transform>(trans)[i].mTranslate.value[j] =
             static_cast<f32>(view<Record>(record)->mTranslate.value[j]));
      n = static_cast<u16>(view<OldBlend>(o)->mBuffer.mNum);
    }
  }
  u32 reference = static_cast<u32>(view<ModelContext>(ea(data))->mpReference);
  entryBuffer(view<void>(o + 0x8C), view<void>(reference));
}

/* WWHD glue (tools/verify/autoglue.py): no TU-wide static objects. */
