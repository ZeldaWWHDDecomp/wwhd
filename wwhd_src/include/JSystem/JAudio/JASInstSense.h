#ifndef WWHD_JASINSTSENSE_H
#define WWHD_JASINSTSENSE_H
#include "JSystem/JAudio/JASInstEffect.h"
namespace JASystem {
class TInstSense : public TInstEffect {
public:
    f32 getY(s32 key, s32 velocity) const;
    void setParams(s32 trigger, s32 centerkey, f32 low, f32 high);
    be<u8> mTrigger;
    be<u8> mCenterKey;
    u8 padding[2];
    be<f32> mLow;
    be<f32> mHigh;
};
}
WWHD_OFFSET(JASystem::TInstSense, mTrigger, 8);
WWHD_OFFSET(JASystem::TInstSense, mCenterKey, 9);
WWHD_OFFSET(JASystem::TInstSense, mLow, 0xC);
WWHD_OFFSET(JASystem::TInstSense, mHigh, 0x10);
WWHD_SIZE(JASystem::TInstSense, 0x14);
#endif
