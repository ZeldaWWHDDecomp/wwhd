/* WWHD key search and cubic interpolation, retaining each single-precision operation. */
#include "nw/lyt/AnimationCurves.h"
using namespace nw::lyt;
struct LayoutNumericLimits { be<f32> quietNaN, positiveInfinity, negativeInfinity, tolerance; };
static LayoutNumericLimits sLayoutLimits;

/* 028712E4 */
void nw::lyt::initializeLayoutNumericLimits() {
    sLayoutLimits.tolerance = 0x1p-22f;
    sLayoutLimits.positiveInfinity = gabi::f32_from_bits(0x7F800000);
    sLayoutLimits.quietNaN = gabi::f32_from_bits(0x7FC00000);
    sLayoutLimits.negativeInfinity = gabi::f32_from_bits(0xFF800000);
}

/* 02871B44 */
f32 nw::lyt::interpolateHermiteKeys(const HermiteKey* keys, u32 count, f32 time) {
    if (count == 1) return keys[0].value;
    f32 firstTime = keys[0].time;
    if (!(time > firstTime)) return keys[0].value;
    f32 lastTime = keys[count - 1].time;
    if (!(time < lastTime)) return keys[count - 1].value;
    u32 last = count - 1;
    u32 upper = last;
    u32 lower = 0;
    u32 beforeUpper = upper - 1;
    while (lower != beforeUpper && lower != upper) {
        u32 middle = u32(s32(lower + upper) / 2);
        f32 middleTime = keys[middle].time;
        if (time > middleTime) lower = middle;
        else { upper = middle; beforeUpper = upper - 1; }
    }
    f32 upperTime = keys[upper].time;
    f32 difference = gabi::fsubs_ppc(time, upperTime);
    if (-0.001f < difference && difference < 0.001f) {
        if (upper < last) {
            f32 followingTime = keys[upper + 1].time;
            if (upperTime == followingTime) return keys[upper + 1].value;
        }
        return keys[upper].value;
    }
    f32 lowerTime = keys[lower].time;
    f32 interval = gabi::fsubs_ppc(upperTime, lowerTime);
    f32 elapsed = gabi::fsubs_ppc(time, lowerTime);
    f32 reciprocal = gabi::fdivs_ppc(1.0f, interval);
    f32 square = gabi::fmuls_ppc(elapsed, elapsed);
    f32 scaledSquare = gabi::fmuls_ppc(square, reciprocal);
    f32 normalizedSquare = gabi::fmuls_ppc(scaledSquare, reciprocal);
    f32 twiceScaledSquare = gabi::fadds_ppc(scaledSquare, scaledSquare);
    f32 scaledCube = gabi::fmuls_ppc(elapsed, normalizedSquare);
    f32 threeNormalizedSquare = gabi::fmuls_ppc(3.0f, normalizedSquare);
    f32 normalizedCube = gabi::fmuls_ppc(scaledCube, reciprocal);
    f32 tangentDifference = gabi::fsubs_ppc(scaledCube, twiceScaledSquare);
    f32 twiceCube = gabi::fadds_ppc(normalizedCube, normalizedCube);
    f32 lowerTangentWeight = gabi::fadds_ppc(tangentDifference, elapsed);
    f32 lowerWeightDifference = gabi::fsubs_ppc(twiceCube, threeNormalizedSquare);
    f32 upperWeight = gabi::fmadds(-2.0f, normalizedCube, threeNormalizedSquare);
    f32 upperValue = keys[upper].value;
    f32 lowerWeight = gabi::fadds_ppc(lowerWeightDifference, 1.0f);
    f32 upperContribution = gabi::fmuls_ppc(upperValue, upperWeight);
    f32 lowerValue = keys[lower].value;
    f32 values = gabi::fmadds(lowerValue, lowerWeight, upperContribution);
    f32 lowerSlope = keys[lower].slope;
    f32 upperTangentWeight = gabi::fsubs_ppc(scaledCube, scaledSquare);
    f32 lowerTangentContribution = gabi::fmadds(lowerSlope, lowerTangentWeight, values);
    f32 upperSlope = keys[upper].slope;
    return gabi::fmadds(upperSlope, upperTangentWeight, lowerTangentContribution);
}

/* 02871D4C */
u32 nw::lyt::sampleDiscreteKeys(const DiscreteKey* keys, u32 count, f32 time) {
    if (count == 1) return keys[0].value;
    f32 firstTime = keys[0].time;
    if (!(time > firstTime)) return keys[0].value;
    f32 lastTime = keys[count - 1].time;
    if (!(time < lastTime)) return keys[count - 1].value;
    u32 lower = 0;
    u32 upper = count - 1;
    u32 beforeUpper = count - 2;
    while (lower != beforeUpper && lower != upper) {
        u32 middle = u32(s32(lower + upper) / 2);
        f32 middleTime = keys[middle].time;
        if (time < middleTime) { upper = middle; beforeUpper = upper - 1; }
        else lower = middle;
    }
    f32 upperTime = keys[upper].time;
    f32 difference = gabi::fsubs_ppc(time, upperTime);
    if (-0.001f < difference && difference < 0.001f) return keys[upper].value;
    return keys[lower].value;
}

/* WWHD glue
data sLayoutLimits 104B6848 nocheck
*/
