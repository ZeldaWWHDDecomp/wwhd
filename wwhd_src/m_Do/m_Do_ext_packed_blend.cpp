/* HD SDK packed animation blending; GameCube packet/line roles retained.
 * Preserve guest reloads, write order and PPC arithmetic. */
#include "m_Do/m_Do_ext_completeness_tail.h"
using namespace mDoExtTail;
using namespace gabi;

static u32 angleWord(f32 angle) {
  truncateUnsigned64(angle);
  return cpu->r[4];
} // Native unsigned64 conversion returns low word in r4.
// HD: this interpolation path uses the middle 16 bits of the fraction.
static TrigPair tablePair(u32 word, f32 factor) {
  u32 table = ea(&trigTable()[(word >> 24) & 255]);
  f32 fraction = fmuls_ppc((f32)(u16)(word >> 8), factor);
  f32 slope0 = static_cast<f32>(view<TrigEntry>(table)->mSinSlope),
      slope1 = static_cast<f32>(view<TrigEntry>(table)->mCosSlope);
  f32 base0 = static_cast<f32>(view<TrigEntry>(table)->mSin),
      base1 = static_cast<f32>(view<TrigEntry>(table)->mCos);
  return {fmadds(slope0, fraction, base0), fmadds(slope1, fraction, base1)};
}
static void linear(u32 out, u32 record, u32 weights, u32 index) {
  f32 weight = static_cast<f32>(view<be<f32>>(weights)[index]);
  f32 a0 = static_cast<f32>(view<SourceRecord>(record)->mScale.value[0]),
      a1 = static_cast<f32>(view<SourceRecord>(record)->mScale.value[1]),
      b0 = static_cast<f32>(view<BlendRecord>(out)->mScale.value[0]),
      b1 = static_cast<f32>(view<BlendRecord>(out)->mScale.value[1]);
  f32 b2 = static_cast<f32>(view<BlendRecord>(out)->mScale.value[2]),
      b3 = static_cast<f32>(view<BlendRecord>(out)->mTranslate.value[0]);
  (view<BlendRecord>(out)->mScale.value[0] = fmadds(a0, weight, b0));
  (view<BlendRecord>(out)->mScale.value[1] = fmadds(a1, weight, b1));
  f32 a2 = static_cast<f32>(view<SourceRecord>(record)->mScale.value[2]),
      a3 = static_cast<f32>(view<SourceRecord>(record)->mTranslate.value[0]),
      total = static_cast<f32>(view<BlendRecord>(out)->mWeight);
  f32 b4 = static_cast<f32>(view<BlendRecord>(out)->mTranslate.value[1]),
      b5 = static_cast<f32>(view<BlendRecord>(out)->mTranslate.value[2]);
  (view<BlendRecord>(out)->mScale.value[2] = fmadds(a2, weight, b2));
  (view<BlendRecord>(out)->mTranslate.value[0] = fmadds(a3, weight, b3));
  f32 a4 = static_cast<f32>(view<SourceRecord>(record)->mTranslate.value[1]),
      a5 = static_cast<f32>(view<SourceRecord>(record)->mTranslate.value[2]);
  u32 flags = static_cast<u32>(view<BlendRecord>(out)->mFlags),
      source = static_cast<u32>(view<SourceRecord>(record)->mFlags);
  (view<BlendRecord>(out)->mTranslate.value[1] = fmadds(a4, weight, b4));
  (view<BlendRecord>(out)->mTranslate.value[2] = fmadds(a5, weight, b5));
  (view<BlendRecord>(out)->mWeight = total + weight);
  (view<BlendRecord>(out)->mFlags = flags | ~source);
}
static f32 sine(u32 word, f32 factor) {
  u32 table = ea(&trigTable()[(word >> 24) & 255]);
  f32 fraction = fmuls_ppc((f32)(word & 0xFFFFFF), factor);
  f32 slope = static_cast<f32>(view<TrigEntry>(table)->mSinSlope),
      base = static_cast<f32>(view<TrigEntry>(table)->mSin);
  return fmadds(slope, fraction, base);
}
static f32 reciprocalRefined(f32 x) {
  f32 estimate = (f32)fres(x);
  f32 twice = estimate + estimate;
  return fnmsubs(fmuls_ppc(x, estimate), estimate, twice);
}
static u32 unsignedTruncate(f32 x, f32 boundary) {
  if (!(x < boundary))
    return (u32)ftoi(x - boundary) + 0x80000000;
  return (u32)ftoi(x);
}
static void quaternionBlend(u32 out, u32 record, u32 weights, u32 index,
                            f32 zero, f32 scale, f32 fractionScale,
                            f32 boundary, f32 threshold, f32 one, f64 positive,
                            f64 negative, const f32 *computed = nullptr,
                            f32 savedOldWeight = 0, f32 savedWeight = 0) {
  f32 oldWeight = computed ? savedOldWeight
                           : static_cast<f32>(view<BlendRecord>(out)->mWeight),
      weight = computed ? savedWeight
                        : static_cast<f32>(view<be<f32>>(weights)[index]);
  if (oldWeight == zero) {
    for (u32 j = 0; j < 4; j++) {
      if (computed)
        (view<BlendRecord>(out)->mQuat.value[j] = computed[j]);
      else
        (view<BlendRecord>(out)->mQuat.words[j] = static_cast<u32>(
             view<SourceRecord>(record)->mOrientation.words[j]));
    }
    linear(out, record, weights, index);
    return;
  }
  f32 old[4] = {static_cast<f32>(view<BlendRecord>(out)->mQuat.value[0]),
                static_cast<f32>(view<BlendRecord>(out)->mQuat.value[1]),
                static_cast<f32>(view<BlendRecord>(out)->mQuat.value[2]),
                static_cast<f32>(view<BlendRecord>(out)->mQuat.value[3])};
  f32 incoming[4];
  for (u32 j = 0; j < 4; j++)
    incoming[j] = computed
                      ? computed[j]
                      : static_cast<f32>(
                            view<SourceRecord>(record)->mOrientation.value[j]);
  f32 p0 = fmuls_ppc(old[0], incoming[0]), p1 = fmuls_ppc(old[1], incoming[1]);
  f32 a = fmadds(old[2], incoming[2], p0), b = fmadds(old[3], incoming[3], p1),
      dot = a + b;
  f32 total = oldWeight + weight, estimate = (f32)fres(total),
      prod = fmuls_ppc(total, estimate);
  f32 sign = (f32)(dot >= 0 ? negative : positive), magnitude = std::fabs(dot);
  f32 ratio = fmuls_ppc(weight, fnmsubs(prod, estimate, estimate + estimate));
  f32 oldCoefficient, newCoefficient;
  if (magnitude > threshold) {
    newCoefficient = fmuls_ppc(sign, ratio);
    oldCoefficient = one - ratio;
  } else {
    f64 angle = arcCos((f64)magnitude);
    u32 word = angleWord((f32)(angle * (f64)scale));
    f32 denominator = sine(word, fractionScale),
        inv = reciprocalRefined(denominator);
    u32 next = unsignedTruncate(fmuls_ppc(ratio, (f32)word), boundary),
        previous = word - next;
    oldCoefficient = fmuls_ppc(sine(previous, fractionScale), inv);
    newCoefficient = fmuls_ppc(sign, fmuls_ppc(sine(next, fractionScale), inv));
  }
  for (u32 pair = 0; pair < 2; pair++) {
    u32 p = out + 32 + pair * 8, r = record + 32 + pair * 8;
    f32 new0 = computed ? computed[pair * 2]
                        : static_cast<f32>(view<be<f32>>(r)[0]),
        new1 = computed ? computed[pair * 2 + 1]
                        : static_cast<f32>(view<be<f32>>(r)[1]),
        old0 = static_cast<f32>(view<be<f32>>(p)[0]),
        old1 = static_cast<f32>(view<be<f32>>(p)[1]);
    f32 p0 = fmuls_ppc(new0, newCoefficient),
        p1 = fmuls_ppc(new1, newCoefficient);
    (view<be<f32>>(p)[0] = fmadds(old0, oldCoefficient, p0));
    (view<be<f32>>(p)[1] = fmadds(old1, oldCoefficient, p1));
  }
  linear(out, record, weights, index);
}
/* 025EEC68 */
void mDoExt_blendQuaternionRecords(void *destination, void *animation,
                                   void *weights) {

  u32 d = ea(destination), a = ea(animation), w = ea(weights);
  (view<Animation>(a)->mFlags =
       static_cast<u32>(view<Animation>(a)->mFlags) | BUFFER_DIRTY);
  u32 record = static_cast<u32>(view<Animation>(a)->mpRecords);
  (view<BlendBuffer>(d)->mFlags =
       static_cast<u32>(view<BlendBuffer>(d)->mFlags) | BUFFER_DIRTY);
  u16 count = static_cast<u16>(view<Animation>(a)->mNum);
  u32 buffer = static_cast<u32>(view<BlendBuffer>(d)->mpRecords);
  if (!count)
    return;
  f32 zero = static_cast<f32>(constants()->mZero),
      factor = static_cast<f32>(constants()->mFraction24),
      scale = static_cast<f32>(constants()->mAngleScale),
      threshold = static_cast<f32>(constants()->mDotThreshold),
      boundary = static_cast<f32>(constants()->mUnsignedBoundary),
      one = static_cast<f32>(constants()->mOne);
  f64 positive = static_cast<f64>(constants()->mPositive),
      negative = static_cast<f64>(constants()->mNegative);
  for (u32 i = 0; i < count; i++, record += 48) {
    u32 flag = static_cast<u32>(
        view<be<u32>>(static_cast<u32>(view<Animation>(a)->mpJointIndices))[i]);
    if (flag & SKIP_JOINT)
      continue;
    quaternionBlend(buffer + (flag & JOINT_INDEX_MASK) * 56, record, w, i, zero,
                    scale, factor, boundary, threshold, one, positive,
                    negative);
  }
}
// HD: quaternion blending uses all 24 fractional bits.
static TrigPair fullTablePair(u32 word, f32 factor) {
  u32 table = ea(&trigTable()[(word >> 24) & 255]);
  f32 fraction = fmuls_ppc((f32)(word & 0xFFFFFF), factor);
  f32 slope0 = static_cast<f32>(view<TrigEntry>(table)->mSinSlope),
      base0 = static_cast<f32>(view<TrigEntry>(table)->mSin),
      slope1 = static_cast<f32>(view<TrigEntry>(table)->mCosSlope),
      base1 = static_cast<f32>(view<TrigEntry>(table)->mCos);
  return {fmadds(slope0, fraction, base0), fmadds(slope1, fraction, base1)};
}
static f32 squareRootRefined(f32 value, f32 half, f32 threeHalves) {
  if (value == 0)
    return value;
  f32 estimate = (f32)frsqrte(value),
      product = fmuls_ppc(fmuls_ppc(half, value), estimate);
  estimate = fmuls_ppc(estimate, fnmsubs(product, estimate, threeHalves));
  return fmuls_ppc(value, estimate);
}
// HD: retain SDK diagonal selection and comparison order, including NaNs/ties.
static void matrixQuaternion(const f32 *m, f32 *q, f32 zero, f32 quarter,
                             f32 half, f32 threeHalves, f32 four) {
  f32 trace = (m[0] + m[4]) + m[8], argument = trace;
  u32 axis = 3;
  if (trace < zero) {
    argument = (m[0] + m[0]) - trace;
    axis = 0;
    if (argument < zero) {
      argument = (m[4] + m[4]) - trace;
      axis = 1;
      if (argument < zero) {
        argument = (m[8] + m[8]) - trace;
        axis = 2;
      }
    }
  }
  f32 root = squareRootRefined(fmadds(argument, quarter, quarter), half,
                               threeHalves),
      inv = reciprocalRefined(fmuls_ppc(root, four));
  if (axis == 3) {
    q[0] = fmuls_ppc(m[7] - m[5], inv);
    q[1] = fmuls_ppc(m[2] - m[6], inv);
    q[2] = fmuls_ppc(m[3] - m[1], inv);
    q[3] = root;
  } else if (axis == 0) {
    q[0] = root;
    q[1] = fmuls_ppc(m[3] + m[1], inv);
    q[2] = fmuls_ppc(m[2] + m[6], inv);
    q[3] = fmuls_ppc(m[7] - m[5], inv);
  } else if (axis == 1) {
    q[0] = fmuls_ppc(m[1] + m[3], inv);
    q[1] = root;
    q[2] = fmuls_ppc(m[5] + m[7], inv);
    q[3] = fmuls_ppc(m[2] - m[6], inv);
  } else {
    q[0] = fmuls_ppc(m[6] + m[2], inv);
    q[1] = fmuls_ppc(m[7] + m[5], inv);
    q[2] = root;
    q[3] = fmuls_ppc(m[3] - m[1], inv);
  }
}
/* 025EF13C */
void mDoExt_blendEulerQuaternionRecords(void *destination, void *animation,
                                        void *weights) {

  u32 d = ea(destination), a = ea(animation), w = ea(weights);
  (view<Animation>(a)->mFlags =
       static_cast<u32>(view<Animation>(a)->mFlags) | BUFFER_DIRTY);
  u32 record = static_cast<u32>(view<Animation>(a)->mpRecords);
  (view<BlendBuffer>(d)->mFlags =
       static_cast<u32>(view<BlendBuffer>(d)->mFlags) | BUFFER_DIRTY);
  u16 count = static_cast<u16>(view<Animation>(a)->mNum);
  u32 buffer = static_cast<u32>(view<BlendBuffer>(d)->mpRecords);
  if (!count)
    return;
  f32 threshold = static_cast<f32>(constants()->mDotThreshold),
      quarter = static_cast<f32>(constants()->mQuarter),
      scale = static_cast<f32>(constants()->mAngleScale),
      fractionScale = static_cast<f32>(constants()->mFraction24),
      boundary = static_cast<f32>(constants()->mUnsignedBoundary),
      zero = static_cast<f32>(constants()->mZero),
      one = static_cast<f32>(constants()->mOne),
      four = static_cast<f32>(constants()->mFour),
      threeHalves = static_cast<f32>(constants()->mThreeHalves),
      half = static_cast<f32>(constants()->mHalf);
  f64 negative = static_cast<f64>(constants()->mNegative),
      positive = static_cast<f64>(constants()->mPositive);
  for (u32 i = 0; i < count; i++, record += 48) {
    u32 flag = static_cast<u32>(
        view<be<u32>>(static_cast<u32>(view<Animation>(a)->mpJointIndices))[i]);
    if (flag & SKIP_JOINT)
      continue;
    u32 out = buffer + (flag & JOINT_INDEX_MASK) * 56;
    f32 angle =
            static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[0]),
        weight = static_cast<f32>(view<be<f32>>(w)[i]);
    auto x = fullTablePair(angleWord(fmuls_ppc(angle, scale)), fractionScale);
    auto y = fullTablePair(
        angleWord(fmuls_ppc(
            static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[1]),
            scale)),
        fractionScale);
    u32 zword = angleWord(fmuls_ppc(
        static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[2]),
        scale));
    // The native reads destination weight after the third conversion, before final trig interpolation.
    f32 oldWeight = static_cast<f32>(view<BlendRecord>(out)->mWeight);
    auto z = fullTablePair(zword, fractionScale);
    f32 sxsy = fmuls_ppc(x.first, y.first), sxcy = fmuls_ppc(x.first, y.second),
        cxcy = fmuls_ppc(x.second, y.second);
    f32 cxsz = fmuls_ppc(x.second, z.first),
        sxcz = fmuls_ppc(x.first, z.second), sxsz = fmuls_ppc(x.first, z.first),
        cxcz = fmuls_ppc(x.second, z.second);
    f32 m[9] = {fmuls_ppc(y.second, z.second),
                fmsubs(sxsy, z.second, cxsz),
                fmadds(cxcz, y.first, sxsz),
                fmuls_ppc(y.second, z.first),
                fmadds(sxsy, z.first, cxcz),
                fmsubs(cxsz, y.first, sxcz),
                -y.first,
                sxcy,
                cxcy};
    f32 q[4];
    matrixQuaternion(m, q, zero, quarter, half, threeHalves, four);
    quaternionBlend(out, record, w, i, zero, scale, fractionScale, boundary,
                    threshold, one, positive, negative, q, oldWeight, weight);
  }
}
/* 025EE9F4 */
void mDoExt_blendEulerRecords(void *destination, void *animation,
                              void *weights) {

  u32 d = ea(destination), a = ea(animation), w = ea(weights);
  (view<Animation>(a)->mFlags =
       static_cast<u32>(view<Animation>(a)->mFlags) | BUFFER_DIRTY);
  u32 record = static_cast<u32>(view<Animation>(a)->mpRecords);
  (view<BlendBuffer>(d)->mFlags =
       static_cast<u32>(view<BlendBuffer>(d)->mFlags) | BUFFER_DIRTY);
  u16 count = static_cast<u16>(view<Animation>(a)->mNum);
  u32 buffer = static_cast<u32>(view<BlendBuffer>(d)->mpRecords);
  if (!count)
    return;
  f32 scale = static_cast<f32>(constants()->mAngleScale),
      factor = static_cast<f32>(constants()->mFraction16);
  for (u32 i = 0; i < count; i++, record += 48) {
    u32 flag = static_cast<u32>(
        view<be<u32>>(static_cast<u32>(view<Animation>(a)->mpJointIndices))[i]);
    if (flag & SKIP_JOINT)
      continue;
    f32 angle =
            static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[0]),
        weight = static_cast<f32>(view<be<f32>>(w)[i]);
    u32 out = buffer + (flag & JOINT_INDEX_MASK) * 56;
    auto x = tablePair(angleWord(fmuls_ppc(angle, scale)), factor);
    auto y = tablePair(
        angleWord(fmuls_ppc(
            static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[1]),
            scale)),
        factor);
    auto z = tablePair(
        angleWord(fmuls_ppc(
            static_cast<f32>(view<SourceRecord>(record)->mOrientation.value[2]),
            scale)),
        factor);
    auto xz =
        TrigPair{fmuls_ppc(x.first, z.first), fmuls_ppc(x.second, z.second)};
    auto cross =
        TrigPair{fmuls_ppc(x.second, z.first), fmuls_ppc(x.first, z.second)};
    auto plus = TrigPair{fmadds(xz.second, y.first, xz.first),
                         fmadds(xz.first, y.first, xz.second)};
    auto minus = TrigPair{fmsubs(cross.second, y.first, cross.first),
                          fmsubs(cross.first, y.first, cross.second)};
    f32 values[6] = {fmuls_ppc(z.second, y.second), minus.first, plus.first,
                     fmuls_ppc(z.first, y.second),  plus.second, minus.second};
    for (u32 pair = 0; pair < 3; pair++) {
      u32 p = out + 0x20 + pair * 8;
      f32 old0 = static_cast<f32>(view<be<f32>>(p)[0]),
          old1 = static_cast<f32>(view<be<f32>>(p)[1]);
      (view<be<f32>>(p)[0] = fmadds(values[pair * 2], weight, old0));
      (view<be<f32>>(p)[1] = fmadds(values[pair * 2 + 1], weight, old1));
    }
    linear(out, record, w, i);
  }
}

/* WWHD glue (tools/verify/autoglue.py): no TU-wide static objects. */
