/* Based on CC0 tww JASTrack.cpp vibrato helpers. */
#include "JSystem/JAudio/JASVibrate.h"
using namespace JASystem;
namespace JASystem {
/* 02816DE0 */
void TVibrate::init() {
    mPitch = 1.0f / 18.0f;
    mDepth = 0.0f;
    mPhase = 0.0f;
}
/* 02816E00 */
TVibrate::TVibrate() {
    init();
}
/* 02816E38 */
f32 TVibrate::getValue() const {
    if (mDepth == 0.0f) return 1.0f;
    // HD: evaluate the sine directly instead of folding into the GameCube lookup table.
    f32 angle = gabi::fmuls_ppc(mPhase, 1.5707963705062866f);
    f64 sine = vibratoSin(angle);
    f32 depth = mDepth;
    // Keep the double-precision sine until the single-precision multiply rounds.
    f32 value = (f32)(sine * (f64)depth);
    return vibratoPitchToCent(value, 12.0f);
}
/* 02816EA4 */
void TVibrate::incCounter() {
    f32 phase = gabi::fadds_ppc(mPhase, mPitch);
    // GHS subtracts on unordered values too.
    if (!(phase < 4.0)) phase = gabi::fsubs_ppc(phase, 4.0f);
    mPhase = phase;
}
}
/* WWHD glue
ext 028F43F8 f64 vibratoSin(f64)
ext 02845674 f32 vibratoPitchToCent(f32, f32)
*/
