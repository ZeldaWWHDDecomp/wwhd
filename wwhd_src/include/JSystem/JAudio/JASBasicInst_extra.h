#pragma once
#include "JSystem/JAudio/JASBasicInst.h"
namespace JASystem {
// HD: GHS allocating/deleting entry points retain explicit ABI facades.
TBasicInst* constructBasicInstrument(TBasicInst* inst);
TBasicInst::TKeymap* constructBasicInstrumentKeymap(TBasicInst::TKeymap* keymap);
void destroyBasicInstrumentKeymap(TBasicInst::TKeymap* keymap,u32 flags);
}
WWHD_OFFSET(JASystem::TInstParam,mVolume,0x10);
WWHD_OFFSET(JASystem::TInstParam,mPitch,0x14);
WWHD_OFFSET(JASystem::TInstParam,mPan,0x18);
WWHD_OFFSET(JASystem::TInstParam,mFxMix,0x1C);
WWHD_OFFSET(JASystem::TInstParam,mDolby,0x20);
WWHD_OFFSET(JASystem::TBasicInst,mPitch,8);
WWHD_OFFSET(JASystem::TBasicInst,mEffectCount,0x10);
WWHD_OFFSET(JASystem::TBasicInst,mOscCount,0x18);
WWHD_OFFSET(JASystem::TBasicInst,mKeyRegionCount,0x1C);
WWHD_SIZE(JASystem::TInstParam,0x28);
WWHD_SIZE(JASystem::TBasicInst,0x24);
WWHD_SIZE(JASystem::TBasicInst::TKeymap,0xC);
WWHD_SIZE(JASystem::TBasicInst::TVeloRegion,0x10);
void __sinit_JASBasicInst_cpp();
