#ifndef WWHD_JASDRUMSET_H
#define WWHD_JASDRUMSET_H
#include "JSystem/JAudio/JASInstEffect.h"
#include "JSystem/JAudio/JASBasicInst.h"
namespace JASystem {
using JASVeloRegion = TBasicInst::TVeloRegion;
class TDrumSet : public TInst {
public:
    struct TPerc {
        be<f32> mVolume, mPitch, mPan;
        be<u16> mRelease; u8 padding[2];
        gptr<gptr<TInstEffect>> mEffect;
        be<u32> mEffectCount, mVelomapCount;
        gptr<JASVeloRegion> mVelomap;
        TPerc();
        void setVeloRegionCount(u32 count);
        JASVeloRegion* getVeloRegion(u32 index);
        void setEffectCount(u32 count);
        void setEffect(u32 index, TInstEffect* effect);
        void setRelease(u32 release);
    };
    TPerc mPercussion[128];
    TPerc* getPerc(u32 key);
    u32 getParam(s32 key, s32 velocity, TInstParam* param) const;
};
}
WWHD_OFFSET(JASystem::TDrumSet::TPerc, mEffect, 0x10);
WWHD_OFFSET(JASystem::TDrumSet::TPerc, mVelomap, 0x1C);
WWHD_SIZE(JASystem::TDrumSet::TPerc, 0x20);
WWHD_OFFSET(JASystem::TDrumSet, mPercussion, 4);
WWHD_SIZE(JASystem::TDrumSet, 0x1004);
#endif
