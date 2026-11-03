#ifndef WWHD_JASBASICINST_H
#define WWHD_JASBASICINST_H
#include "JSystem/JAudio/JASInst.h"
#include "JSystem/JAudio/JASInstEffect.h"
namespace JASystem {
class TBasicInst : public TInst {
public:
    struct TVeloRegion { be<s32> mBaseVel; be<u32> mWaveId; be<f32> mVolume, mPitch; };
    class TKeymap {
    public:
        be<s32> mBaseKey;
        be<u32> mVeloRegionCount;
        gptr<TVeloRegion> mVelomap;
        void setVeloRegionCount(u32);
        TVeloRegion* getVeloRegion(s32);
        const TVeloRegion* getVeloRegion(s32) const;
    };
    be<f32> mVolume, mPitch;
    gptr<gptr<TInstEffect>> mEffect;
    be<u32> mEffectCount;
    gptr<gptr<TOscillator::Osc_>> mOsc;
    be<u32> mOscCount, mKeyRegionCount;
    gptr<TKeymap> mKeymap;
    void setKeyRegionCount(u32);
    TKeymap* getKeyRegion(s32);
    void setEffectCount(u32);
    void setEffect(s32, TInstEffect*);
    void setOscCount(u32);
    void setOsc(s32, TOscillator::Osc_*);
    u32 getParam(s32, s32, TInstParam*) const;
};
}
WWHD_OFFSET(JASystem::TBasicInst, mVolume, 4);
WWHD_OFFSET(JASystem::TBasicInst, mEffect, 0xC);
WWHD_OFFSET(JASystem::TBasicInst, mOsc, 0x14);
WWHD_OFFSET(JASystem::TBasicInst, mKeymap, 0x20);
WWHD_SIZE(JASystem::TBasicInst, 0x24);
WWHD_SIZE(JASystem::TBasicInst::TKeymap, 0xC);
WWHD_SIZE(JASystem::TBasicInst::TVeloRegion, 0x10);
#endif
