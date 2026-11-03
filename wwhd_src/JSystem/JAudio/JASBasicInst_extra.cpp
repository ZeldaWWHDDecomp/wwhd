// Cafe basic instrument key/velocity regions, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASBasicInst_extra.h"
using namespace JASystem;
using namespace gabi;
struct BasicInstVTable_l { u8 bytes[0x20]; };
struct InstrumentEffectVTable_l { u8 prefix[0xC]; gfn getY; };
struct InstrumentScalar_l { be<u32> bits; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using KeymapConstructor = TBasicInst::TKeymap* (*)(TBasicInst::TKeymap*);
using KeymapDestructor = void (*)(TBasicInst::TKeymap*,u32);
static BasicInstVTable_l sVtable;
static u32 sArrayCookie;
static Initialization sInitialization;
static Registration sRegistration;
extern void* allocateBasicInstObject(u32);
extern void* allocateBasicInstArray(u32,void*,u32);
extern void freeBasicInstArray(void*);
extern void freeBasicInstObject(void*);
extern void* getBasicInstHeap();
extern void clearBasicInstArray(void*,u32);
extern void destroyBasicInstArray(void*,s32,u32,KeymapDestructor,u32,u32);
extern TBasicInst::TKeymap* constructBasicInstArray(void*,u32,u32,KeymapConstructor,u32);
extern void registerBasicInstGlobals(void*);
namespace JASystem {
/* 028442C8 */
TBasicInst* constructBasicInstrument(TBasicInst* inst) {
    if (!inst) {
        inst = static_cast<TBasicInst*>(allocateBasicInstObject(sizeof(TBasicInst)));
        if (!inst) return nullptr;
    }
    inst->mVtable = reinterpret_cast<InstVTable*>(&sVtable);
    inst->mPitch = 1.0f;
    inst->mOscCount = 0;
    inst->mKeyRegionCount = 0;
    inst->mEffect = nullptr;
    inst->mVolume = 1.0f;
    inst->mOsc = nullptr;
    inst->mKeymap = nullptr;
    inst->mEffectCount = 0;
    return inst;
}
/* 02844334 */
void destroyBasicInstrumentKeymap(TBasicInst::TKeymap* keymap,u32 flags) {
    if (!keymap) return;
    freeBasicInstArray(keymap->mVelomap);
    if (flags & 1) freeBasicInstObject(keymap);
}
}
/* 02844384 */
void TBasicInst::setKeyRegionCount(u32 count) {
    destroyBasicInstArray(mKeymap,-1,sizeof(TKeymap),destroyBasicInstrumentKeymap,1,0);
    void* heap = getBasicInstHeap();
    u32 bytes = count * (u32)sizeof(TKeymap) + (u32)sArrayCookie;
    void* storage = allocateBasicInstArray(bytes,heap,4);
    if (storage) {
        storage = static_cast<u8*>(storage) + (u32)sArrayCookie;
        mKeymap = constructBasicInstArray(storage,count,sizeof(TKeymap),constructBasicInstrumentKeymap,0);
        mKeyRegionCount = count;
    } else {
        mKeyRegionCount = count;
        mKeymap = nullptr;
    }
}
/* 02844440 */
TBasicInst::TKeymap* TBasicInst::getKeyRegion(s32 index) {
    if ((u32)index >= (u32)mKeyRegionCount) return nullptr;
    return mKeymap.get() + index;
}
/* 02844464 */
void TBasicInst::setEffectCount(u32 count) {
    freeBasicInstArray(mEffect);
    mEffectCount = count;
    if (!count) { mEffect = nullptr; return; }
    u32 bytes = count * 4;
    void* heap = getBasicInstHeap();
    void* storage = allocateBasicInstArray(bytes,heap,4);
    mEffect = static_cast<gptr<TInstEffect>*>(storage);
    clearBasicInstArray(storage,bytes);
}
/* 028444F0 */
void TBasicInst::setEffect(s32 index,TInstEffect* effect) {
    mEffect.get()[index] = effect;
}
/* 02844500 */
void TBasicInst::setOscCount(u32 count) {
    freeBasicInstArray(mOsc);
    mOscCount = count;
    if (!count) { mOsc = nullptr; return; }
    u32 bytes = count * 4;
    void* heap = getBasicInstHeap();
    void* storage = allocateBasicInstArray(bytes,heap,4);
    mOsc = static_cast<gptr<TOscillator::Osc_>*>(storage);
    clearBasicInstArray(storage,bytes);
}
/* 0284458C */
void TBasicInst::setOsc(s32 index,TOscillator::Osc_* oscillator) {
    mOsc.get()[index] = oscillator;
}
/* 0284459C */
const TBasicInst::TVeloRegion* TBasicInst::TKeymap::getVeloRegion(s32 index) const {
    if ((u32)index >= (u32)mVeloRegionCount) return nullptr;
    return mVelomap.get() + index;
}
/* 028445C0 */
u32 TBasicInst::getParam(s32 key,s32 velocity,TInstParam* param) const {
    param->mType = 0;
    param->mPercussion = 0;
    param->mOscData = reinterpret_cast<gptr<const TOscillator::Osc_>*>(mOsc.get());
    param->mOscCount = mOscCount;
    // HD: unscaled instrument defaults are copied without quieting signaling NaNs.
    reinterpret_cast<InstrumentScalar_l*>(&param->mVolume)->bits = reinterpret_cast<const InstrumentScalar_l*>(&mVolume)->bits;
    reinterpret_cast<InstrumentScalar_l*>(&param->mPitch)->bits = reinterpret_cast<const InstrumentScalar_l*>(&mPitch)->bits;
    for (u32 i = 0; i < (u32)mEffectCount; ++i) {
        TInstEffect* effect = mEffect.get()[i];
        if (!effect) continue;
        auto vtable = static_cast<InstrumentEffectVTable_l*>(effect->mVtable.get());
        f32 y = call_ptr<f32>(vtable->getY.get(),effect,key,velocity);
        switch ((u8)effect->mTarget) {
        case 0: param->mVolume = fmuls_ppc(param->mVolume,y); break;
        case 1: param->mPitch = fmuls_ppc(param->mPitch,y); break;
        case 2: param->mPan = (f32)((f64)(f32)param->mPan + ((f64)y - 0.5)); break;
        case 3: param->mFxMix = fadds_ppc(param->mFxMix,y); break;
        case 4: param->mDolby = fadds_ppc(param->mDolby,y); break;
        }
    }
    u32 keyCount = mKeyRegionCount;
    const TKeymap* keymaps = mKeymap;
    const TKeymap* selected = nullptr;
    for (u32 i = 0; i < keyCount; ++i) {
        if (key <= (s32)keymaps[i].mBaseKey) { selected = keymaps + i; break; }
    }
    if (!selected) return 0;
    u32 velocityCount = selected->mVeloRegionCount;
    for (u32 i = 0; i < velocityCount; ++i) {
        const TVeloRegion* region = selected->getVeloRegion((s32)i);
        if (velocity > (s32)region->mBaseVel) continue;
        f32 volume = fmuls_ppc(param->mVolume,region->mVolume);
        f32 pitch = param->mPitch;
        param->mVolume = volume;
        pitch = fmuls_ppc(pitch,region->mPitch);
        param->mPitch = pitch;
        param->mWaveId = region->mWaveId;
        return 1;
    }
    return 0;
}
/* 028447F0 */
void TBasicInst::TKeymap::setVeloRegionCount(u32 count) {
    freeBasicInstArray(mVelomap);
    void* heap = getBasicInstHeap();
    mVelomap = static_cast<TVeloRegion*>(allocateBasicInstArray(count * 16,heap,4));
    mVeloRegionCount = count;
}
/* 02844848 */
TBasicInst::TVeloRegion* TBasicInst::TKeymap::getVeloRegion(s32 index) {
    if ((u32)index >= (u32)mVeloRegionCount) return nullptr;
    return mVelomap.get() + index;
}
/* 0284486C */
void __sinit_JASBasicInst_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerBasicInstGlobals(&sRegistration);
}
namespace JASystem {
/* 02844894 */
TBasicInst::TKeymap* constructBasicInstrumentKeymap(TBasicInst::TKeymap* keymap) {
    if (!keymap) {
        keymap = static_cast<TBasicInst::TKeymap*>(allocateBasicInstObject(sizeof(TBasicInst::TKeymap)));
        if (!keymap) return nullptr;
    }
    keymap->mBaseKey = -1;
    keymap->mVelomap = nullptr;
    keymap->mVeloRegionCount = 0;
    return keymap;
}
}
/* WWHD glue
data sVtable 1017A4DC
data sArrayCookie 101FCD08
data sInitialization 104B65F4
data sRegistration 101FC23C
ext 0273AD10 void* allocateBasicInstObject(u32)
ext 0273B0D4 void* allocateBasicInstArray(u32,void*,u32)
ext 0273AFC8 void freeBasicInstArray(void*)
ext 0273AF40 void freeBasicInstObject(void*)
ext 0284416C void* getBasicInstHeap()
ext 02843E34 void clearBasicInstArray(void*,u32)
ext 028F0164 void destroyBasicInstArray(void*,s32,u32,KeymapDestructor,u32,u32)
ext 028EFF8C JASystem::TBasicInst::TKeymap* constructBasicInstArray(void*,u32,u32,KeymapConstructor,u32)
ext 028F026C void registerBasicInstGlobals(void*)
*/
