// Sound-handle forwarding and local text lifetime helpers, from WWHD instructions.
#include "sead/AudioHandleCompanions.h"
using namespace sead;
void releaseAudioHandleStorage(void* object);
void stopAudioHandleSound(nw::snd::BasicSound* sound,s32 fadeFrames);

/* 0276AC1C */
void AudioHandle_l::stop(s32 fadeFrames) {
    nw::snd::BasicSound* sound=mSound;
    if(sound) gabi::tail_ptr<void>(gabi::guest_fn((const void*)stopAudioHandleSound),sound,fadeFrames);
}
/* 0276AC2C */
u32 AudioHandle_l::hasSound() const { return mSound!=nullptr; }
/* 0276AC3C */
u32 AudioHandle_l::soundId() const {
    if(mSound) return mSound->soundId;
    return 0xFFFFFFFF;
}
/* 0276AC5C */
void audio_handle_text::Text::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAudioHandleStorage),this);
}
/* 0276AC70 */
void audio_handle_text::Text::assureTerminationImpl() {}
/* 0276AC74 */
void audio_handle_text::TextBuffer::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAudioHandleStorage),this);
}
/* 0276AC88 */
void audio_handle_text::TextBuffer::assureTerminationImpl() {
    u32 text=gabi::ea(mText.get());
    u32 capacity=mCapacity;
    *gabi::at<be<u8>>(text+capacity-1)=0;
}
/* WWHD glue
 ext 02883FA8 void stopAudioHandleSound(nw::snd::BasicSound*,s32) callback
 ext 0273AF40 void releaseAudioHandleStorage(void*) callback
*/
