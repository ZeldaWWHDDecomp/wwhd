#ifndef WWHD_JASINST_H
#define WWHD_JASINST_H
#include "JSystem/JAudio/JASOscillator.h"
namespace JASystem {
// HD: the instrument result omits the separate modulation multipliers and key-map indexes.
struct TInstParam {
    be<u8> mType; u8 padding[3];
    be<u32> mWaveId;
    gptr<gptr<const TOscillator::Osc_>> mOscData;
    be<u32> mOscCount;
    be<f32> mVolume, mPitch, mPan, mFxMix, mDolby;
    be<u8> mPercussion; u8 paddingPercussion;
    be<u16> mRelease;
};
struct InstVTable { u8 prefix[0x14]; gfn getParam; };
class TInst { public: gptr<InstVTable> mVtable; };
}
WWHD_SIZE(JASystem::TInstParam, 0x28);
WWHD_OFFSET(JASystem::TInstParam, mOscData, 8);
WWHD_OFFSET(JASystem::TInstParam, mVolume, 0x10);
WWHD_OFFSET(JASystem::TInstParam, mPercussion, 0x24);
WWHD_OFFSET(JASystem::TInstParam, mRelease, 0x26);
WWHD_SIZE(JASystem::TInst, 4);
#endif
