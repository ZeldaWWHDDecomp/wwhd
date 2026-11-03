#pragma once
#include "JSystem/JAudio/JASTrack.h"
#include "JSystem/JAudio/JASChannel.h"
// HD: the canonical Cafe updater is distinct from its enclosing TTrack state.
namespace JASystem {
TChannelUpdater* constructChannelUpdater(TChannelUpdater* updater);
}
WWHD_OFFSET(JASystem::TChannelUpdater,mVolume,0);
WWHD_OFFSET(JASystem::TChannelUpdater,mPitch,4);
WWHD_OFFSET(JASystem::TChannelUpdater,mPan,8);
WWHD_OFFSET(JASystem::TChannelUpdater,mFxMix,0xC);
WWHD_OFFSET(JASystem::TChannelUpdater,mDolby,0x10);
WWHD_OFFSET(JASystem::TChannelUpdater,mFir,0x14);
WWHD_OFFSET(JASystem::TChannelUpdater,mIir,0x24);
WWHD_OFFSET(JASystem::TChannelUpdater,mFilterMix,0x34);
WWHD_OFFSET(JASystem::TChannelUpdater,mMixConfigs,0x36);
WWHD_OFFSET(JASystem::TChannelUpdater,mDelay,0x42);
WWHD_OFFSET(JASystem::TChannelUpdater,mMixerMode,0x48);
WWHD_OFFSET(JASystem::TChannelUpdater,mFilterFlags,0x49);
WWHD_OFFSET(JASystem::TChannelUpdater,mCalcTypes,0x4A);
void __sinit_JASChannelUpdater_cpp();
