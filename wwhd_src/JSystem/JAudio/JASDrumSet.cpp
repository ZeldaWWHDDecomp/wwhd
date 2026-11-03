// Percussion instrument regions, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASDrumSet.h"
#include "sead/ElementAt.h"
using namespace JASystem;
using TPerc = TDrumSet::TPerc;
static TOscillator::Osc_ osc;
using OscPointer = gptr<const TOscillator::Osc_>;
static OscPointer oscp;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
struct EffectVTable { u8 prefix[0xC]; gfn getY; };
extern void deletePercussionArray(void*);
extern void* getPercussionHeap();
extern void* allocatePercussionArray(u32, void*, u32);
extern void clearPercussionArray(void*, u32);
extern void registerPercussionGlobals(void*);
/* 02812020 */
TDrumSet::TPerc* TDrumSet::getPerc(u32 key) {
    // HD: the release build omits index assertions and bounds checks.
    return &mPercussion[key];
}
/* 02812030 */
u32 TDrumSet::getParam(s32 key, s32 velocity, TInstParam* param) const {
    if ((u32)key >= 128)
        return 0;
    const TPerc* perc = &mPercussion[(u32)key];
    param->mType = 0;
    param->mPercussion = 1;
    param->mVolume = perc->mVolume;
    param->mPitch = perc->mPitch;
    param->mPan = perc->mPan;
    param->mRelease = perc->mRelease;
    // HD: output volume/pitch are assigned rather than multiplied.
    osc.mScale = 1.0f;
    osc.mRate = 1.0f;
    osc.mOffset = 0.0f;
    osc.table = nullptr;
    // HD: the source writes the whole first word of the oscillator record.
    osc.mTargetWord = 0;
    osc.rel_table = nullptr;
    param->mOscData = &oscp;
    param->mOscCount = 1;
    for (u32 i = 0; i < perc->mEffectCount; ++i) {
        TInstEffect* effect = perc->mEffect.get()[i];
        if (!effect)
            continue;
        auto vtable = reinterpret_cast<EffectVTable*>(effect->mVtable.get());
        f32 y = gabi::call_ptr<f32>(vtable->getY.get(), effect, key, velocity);
        switch (effect->mTarget) {
        case 0: param->mVolume = fmuls_ppc(param->mVolume, y); break;
        case 1: param->mPitch = fmuls_ppc(param->mPitch, y); break;
        case 2: param->mPan = (f32)((f64)(f32)param->mPan + ((f64)y - 0.5)); break;
        case 3: param->mFxMix = fadds_ppc(param->mFxMix, y); break;
        case 4: param->mDolby = fadds_ppc(param->mDolby, y); break;
        }
    }
    u32 count = perc->mVelomapCount;
    JASVeloRegion* regions = perc->mVelomap;
    for (u32 i = 0; i < count; ++i) {
        JASVeloRegion* region = &regions[i];
        if (velocity > region->mBaseVel)
            continue;
        f32 volume = fmuls_ppc(param->mVolume, region->mVolume);
        f32 pitch = param->mPitch;
        param->mVolume = volume;
        param->mPitch = fmuls_ppc(pitch, region->mPitch);
        param->mWaveId = region->mWaveId;
        return 1;
    }
    return 0;
}
/* 02812270 */
TPerc::TPerc() {
    mRelease = 1000;
    mVelomapCount = 0;
    mPan = 0.5f;
    mVelomap = nullptr;
    mEffectCount = 0;
    mVolume = 1.0f;
    mEffect = nullptr;
    mPitch = 1.0f;
}
/* 028122DC */
void TDrumSet::TPerc::setVeloRegionCount(u32 count) {
    deletePercussionArray(mVelomap);
    void* heap = getPercussionHeap();
    auto regions = static_cast<JASVeloRegion*>(allocatePercussionArray(count * 16, heap, 4));
    mVelomap = regions;
    mVelomapCount = count;
}
/* 02812334 */
JASVeloRegion* TDrumSet::TPerc::getVeloRegion(u32 index) {
    return mVelomap.get() + index;
}
/* 02812344 */
void TDrumSet::TPerc::setEffectCount(u32 count) {
    deletePercussionArray(mEffect);
    mEffectCount = count;
    if (count == 0) {
        mEffect = nullptr;
        return;
    }
    u32 bytes = count * 4;
    void* heap = getPercussionHeap();
    auto effects = static_cast<gptr<TInstEffect>*>(allocatePercussionArray(bytes, heap, 4));
    mEffect = effects;
    clearPercussionArray(effects, bytes);
}
/* 028123D0 */
void TDrumSet::TPerc::setEffect(u32 index, TInstEffect* effect) {
    sead::elementAt(mEffect.get(), index) = effect;
}
/* 028123E0 */
void TDrumSet::TPerc::setRelease(u32 release) {
    mRelease = (u16)release;
}
/* 028123E8 */
void __sinit_JASDrumSet_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerPercussionGlobals(&sRegistration);
}
/* WWHD glue
data osc 104B52D0
data oscp 101F9DF0
data sInitialization 104B52E8
data sRegistration 101F9DF4
ext 0273AFC8 void deletePercussionArray(void*)
ext 0284416C void* getPercussionHeap()
ext 0273B0D4 void* allocatePercussionArray(u32, void*, u32)
ext 02843E34 void clearPercussionArray(void*, u32)
ext 028F026C void registerPercussionGlobals(void*)
*/
