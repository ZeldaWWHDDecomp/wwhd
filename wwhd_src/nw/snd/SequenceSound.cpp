/* Sequence sound lifecycle and handle adapters, reconstructed from WWHD. */
#include "nw/snd/SequenceSound.h"
using namespace nw::snd;
void* allocateSequenceSound(u32 size);
void releaseSequenceSound(void* storage);
BasicSound* constructSequenceBasicSound(BasicSound* sound);
void initializeSequenceBasicSound(BasicSound* sound);
void constructSequencePlayer(SequenceSoundPlayer_l* player);
void destroySequencePlayer(SequenceSoundPlayer_l* player,u32 flags);
/* 028ADBE4 */
SequenceSound* SequenceSound::construct(SequenceSound* sound,void* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sound==nullptr) sound=static_cast<SequenceSound*>(allocateSequenceSound(sizeof(SequenceSound)));
    if (sound==nullptr) return sound;
    constructSequenceBasicSound(&sound->mBasic);
    sound->mManager=static_cast<SequenceManager*>(manager);
    sound->mActive=0;
    sound->mBasic.vtable=gabi::at<BasicSoundVtable>(0x10187F70);
    constructSequencePlayer(&sound->mPlayer);
    return sound;
}
/* 028ADF6C */
void SequenceSound::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeSequenceBasicSound(&mBasic);
    mHandle=nullptr;
    mActive=1;
}
/* 028AE030 */
u32 SequenceSound::hasHandle() const { return mHandle!=nullptr; }
/* 028AE040 */
void SequenceSound::detachHandle() {
    gabi::AutoFrame frame{gabi::current_function()};
    mHandle->detach();
}
/* 028AE118 */
u32 SequenceSound::isPrepared() const {
    if (mBasic.driverInitialized==0) return 0;
    return mPlayer.mPrepareState>=3;
}
/* 028AE140 */
void SequenceSound::updateParameters() {}
/* 028AE144 */
SequenceSoundPlayer_l* SequenceSound::driverPlayer() { return &mPlayer; }
/* 028AE14C */
void SequenceSound::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this==nullptr) return;
    destroySequencePlayer(&mPlayer,2);
    mBasic.state=3;
    mBasic.vtable=gabi::at<BasicSoundVtable>(0x10181B60);
    if (flags&1) releaseSequenceSound(this);
}
/* 028AE2C0 */
void SequenceSoundHandle::detach() {
    SequenceSound* sound=mSound;
    if (sound==nullptr) return;
    if (sound->mHandle==this) sound->mHandle=nullptr;
    mSound=nullptr;
}
/* WWHD glue
ext 0273AD10 void* allocateSequenceSound(u32)
ext 0273AF40 void releaseSequenceSound(void*)
ext 02883C64 nw::snd::BasicSound* constructSequenceBasicSound(nw::snd::BasicSound*)
ext 0288540C void initializeSequenceBasicSound(nw::snd::BasicSound*)
ext 0288B914 void constructSequencePlayer(nw::snd::SequenceSoundPlayer_l*)
ext 0288A9F0 void destroySequencePlayer(nw::snd::SequenceSoundPlayer_l*,u32)
*/
