// Instrument key/velocity sensitivity, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASInstSense.h"
using JASystem::TInstSense;
struct Initialization { be<u32> words[4]; };
static Initialization sInitialization;
struct Registration { be<u32> words[3]; };
static Registration sRegistration;
extern void registerSenseGlobals(void*);
/* 028124D4 */
f32 TInstSense::getY(s32 key, s32 velocity) const {
    s32 value = 0;
    switch (mTrigger) {
    case 1: value = velocity; break;
    case 2: value = key; break;
    }
    u32 center = mCenterKey;
    if (center == 0x7F || center == 0) {
        f32 low = mLow;
        f32 difference = fsubs_ppc(mHigh, low);
        return fadds_ppc((f32)(fmuls_ppc(difference, (f32)value) / 127.0f), low);
    }
    if (value < (s32)center) {
        f32 low = mLow;
        return fmadds(fsubs_ppc(1.0f, low), (f32)((f32)value / (f32)center), low);
    }
    f32 fraction = (f32)((f32)(value - (s32)center) / (f32)(127 - (s32)center));
    return fmadds(fsubs_ppc(mHigh, 1.0f), fraction, 1.0f);
}
/* 0281262C */
void TInstSense::setParams(s32 trigger, s32 centerkey, f32 low, f32 high) {
    // HD: range assertions are absent; preserve the store order for aliasing.
    mLow = low;
    mCenterKey = (u8)centerkey;
    mHigh = high;
    mTrigger = (u8)trigger;
}
/* 02812640 */
void __sinit_JASInstSense_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerSenseGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B5318
data sRegistration 101F9E18
ext 028F026C void registerSenseGlobals(void*)
*/
