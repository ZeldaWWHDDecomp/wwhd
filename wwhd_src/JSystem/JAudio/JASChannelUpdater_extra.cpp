// Cafe channel updater companion; its first argument is distinct from TChannel.
#include "JSystem/JAudio/JASChannelUpdater_extra.h"
using namespace JASystem;
struct UpdaterScalar_l { be<u32> bits; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void* allocateChannelUpdater(u32);
// HD: GHS constructor chaining observes the callee's r3, which init leaves intact.
extern TChannelUpdater* initializeConstructedChannelUpdater(TChannelUpdater*);
extern void updaterSetMixConfig(TChannel*,u32,u32);
extern void updaterInitAutoMixer(DSPInterface::DSPBuffer*);
extern void updaterSetMixerMode(DSPInterface::DSPBuffer*,u32);
extern void updaterSetMixerVolume(DSPInterface::DSPBuffer*,u32,s32);
extern void updaterSetMixerDelay(DSPInterface::DSPBuffer*,u32,s32);
extern void updaterSetIIR(DSPInterface::DSPBuffer*,be<s16>*);
extern void updaterSetFIR(DSPInterface::DSPBuffer*,be<s16>*);
extern void updaterSetFilterMode(DSPInterface::DSPBuffer*,u32);
extern void updaterSetFilterMix(DSPInterface::DSPBuffer*,s32);
extern void registerUpdaterGlobals(void*);
namespace JASystem {
/* 028452E8 */
void TChannelUpdater::init() {
    mFxMix = 0;
    mPan = 0.5f;
    mDolby = 0;
    mVolume = 1;
    mPitch = 1;
    for (u32 i = 0; i < 8; ++i) mFir[i] = 0;
    mFir[0] = 0x7FFF;
    mFilterMix = 0;
    for (u32 i = 0; i < 4; ++i) mIir[i] = 0;
    for (u32 i = 0; i < 6; ++i) mDelay[i] = 0;
    mMixerMode = 0;
    mFilterFlags = 0;
    mMixConfigs[0] = 0x150;
    mIir[0] = 0x7FFF;
    mCalcTypes[0] = 13;
    mMixConfigs[1] = 0x210;
    mMixConfigs[5] = 0;
    mMixConfigs[2] = 0x352;
    mMixConfigs[4] = 0;
    mMixConfigs[3] = 0x412;
    mCalcTypes[2] = 13;
    mCalcTypes[1] = 13;
}
/* 028453C0 */
TChannelUpdater* constructChannelUpdater(TChannelUpdater* updater) {
    if (!updater) {
        updater = static_cast<TChannelUpdater*>(allocateChannelUpdater(0x50));
        if (!updater) return nullptr;
    }
    return initializeConstructedChannelUpdater(updater);
}
/* 028453F8 */
void TChannelUpdater::updateJcToDSPInit(TChannel* channel,DSPInterface::DSPBuffer* buffer) {
    for (u32 i = 0; i < 6; ++i) updaterSetMixConfig(channel,i,mMixConfigs[i]);
    if ((u16)channel->mMixConfigs[0] == 0xFFFF) updaterInitAutoMixer(buffer);
    else {
        updaterSetMixerMode(buffer,mMixerMode);
        for (u32 i = 0; i < 6; ++i) updaterSetMixerVolume(buffer,i,(u8)(s8)mDelay[i]);
    }
    for (u32 i = 0; i < 3; ++i) channel->mCalcTypes[i] = mCalcTypes[i];
    // Scalar copies transport the lfs/stfs value without arithmetic or NaN quieting.
    reinterpret_cast<UpdaterScalar_l*>(&channel->mVolumeScale)->bits = reinterpret_cast<UpdaterScalar_l*>(&mVolume)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mPitchScale)->bits = reinterpret_cast<UpdaterScalar_l*>(&mPitch)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mPanEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mPan)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mFxMixEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mFxMix)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mDolbyEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mDolby)->bits;
    for (u32 i = 0; i < 6; ++i) updaterSetMixerDelay(buffer,i,(u8)(s8)mDelay[i]);
    u32 flags = mFilterFlags;
    if (flags & 0x20) {
        updaterSetIIR(buffer,mIir);
        flags = mFilterFlags;
    }
    if (flags & 0x1F) {
        updaterSetFIR(buffer,mFir);
        flags = mFilterFlags;
    }
    updaterSetFilterMode(buffer,flags);
    updaterSetFilterMix(buffer,mFilterMix);
}
/* 02845564 */
void TChannelUpdater::updateJcToDSP(TChannel* channel,DSPInterface::DSPBuffer* buffer) {
    if ((u32)channel->mStatus == 2) return;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mVolumeScale)->bits = reinterpret_cast<UpdaterScalar_l*>(&mVolume)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mPitchScale)->bits = reinterpret_cast<UpdaterScalar_l*>(&mPitch)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mPanEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mPan)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mFxMixEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mFxMix)->bits;
    reinterpret_cast<UpdaterScalar_l*>(&channel->mDolbyEffect)->bits = reinterpret_cast<UpdaterScalar_l*>(&mDolby)->bits;
    for (u32 i = 0; i < 6; ++i) updaterSetMixerDelay(buffer,i,(u8)(s8)mDelay[i]);
    u32 flags = mFilterFlags;
    if (flags & 0x20) {
        updaterSetIIR(buffer,mIir);
        flags = mFilterFlags;
    }
    if (flags & 0x1F) {
        updaterSetFIR(buffer,mFir);
        flags = mFilterFlags;
    }
    updaterSetFilterMode(buffer,flags);
    updaterSetFilterMix(buffer,mFilterMix);
}
}
/* 02845634 */
void __sinit_JASChannelUpdater_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerUpdaterGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B6634
data sRegistration 101FC270
ext 0273AD10 void* allocateChannelUpdater(u32)
ext 028452E8 JASystem::TChannelUpdater* initializeConstructedChannelUpdater(JASystem::TChannelUpdater*)
ext 02815494 void updaterSetMixConfig(JASystem::TChannel*,u32,u32)
ext 02816038 void updaterInitAutoMixer(JASystem::DSPInterface::DSPBuffer*)
ext 028160A0 void updaterSetMixerMode(JASystem::DSPInterface::DSPBuffer*,u32)
ext 028160C4 void updaterSetMixerVolume(JASystem::DSPInterface::DSPBuffer*,u32,s32)
ext 02816100 void updaterSetMixerDelay(JASystem::DSPInterface::DSPBuffer*,u32,s32)
ext 0281617C void updaterSetIIR(JASystem::DSPInterface::DSPBuffer*,be<s16>*)
ext 02816188 void updaterSetFIR(JASystem::DSPInterface::DSPBuffer*,be<s16>*)
ext 02816120 void updaterSetFilterMode(JASystem::DSPInterface::DSPBuffer*,u32)
ext 02816194 void updaterSetFilterMix(JASystem::DSPInterface::DSPBuffer*,s32)
ext 028F026C void registerUpdaterGlobals(void*)
*/
