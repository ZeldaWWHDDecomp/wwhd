// Driver-side frame sequence, reconstructed from original WWHD calls and accesses.
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/AudioCurves.h"
#include "nw/snd/BasicSound.h"
#include "nw/snd/AudioMixRuntime.h"
#include "nw/snd/HardwareVoice.h"
#include "nw/snd/SoftwareMixer.h"
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;
void lockAudioFrame(void*);
void unlockAudioFrame(void*);
Pair32 audioFrameTime();
u32 swapAudioProfileBuffer(void*,u32);
// The NW-SND#2 singleton AudioMixRuntime: a guarded function-local static GHS inlines into each caller.
static AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeGuard;
/* 02898AF8 */
void AudioFrameDriver::processFrame() {
    gabi::AutoFrame frame{gabi::current_function()};
    lockAudioFrame(mutex);
    auto beginWords=audioFrameTime();
    u64 begin=(static_cast<u64>(beginWords.r3)<<32)|beginWords.r4;
    s32 oldIndex=profileBufferIndex;
    auto* oldBuffer=&profileBuffers[oldIndex];
    s32 nextIndex=static_cast<u32>(oldIndex)+1;
    if(nextIndex>=3) nextIndex=0;
    profileBufferIndex=nextIndex;
    u32 profiles=swapAudioProfileBuffer(&profileBuffers[nextIndex],32);
    nw::ut::HeapLink* node=players.root.next;
    while(node!=&players.root) {
        auto* listener=reinterpret_cast<FrameListener*>(node);
        u32 target=listener->vtable->begin;
        node=node->next;
        gabi::call_ptr<void>(target,listener);
    }
    PlaybackVoicePool::instance()->updatePlayback();
    while(DriverCommandManager::instance()->executePending()) {}
    while(DriverCommandManager::taskThreadInstance()->executePending()) {}
    node=listeners.root.next;
    while(node!=&listeners.root) {
        auto* listener=reinterpret_cast<FrameListener*>(node);
        u32 target=listener->vtable->begin;
        node=node->next;
        gabi::call_ptr<void>(target,listener);
    }
    WaveChannelPool::instance()->update();
    PlaybackVoicePool::instance()->updateParameters();
    u32 samples=0;
    if(mixer) {
        mixer->update();
        samples=mixer->render();
    }
    HardwareVoice::restorePriorities();
    if(sMixRuntimeGuard==0) {
        sMixRuntimeGuard=1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    sMixRuntime.updateVolumes();
    nextRandomHalfword();   // advances the sound random generator once per frame
    node=players.root.next;
    while(node!=&players.root) {
        auto* listener=reinterpret_cast<FrameListener*>(node);
        u32 target=listener->vtable->end;
        node=node->next;
        gabi::call_ptr<void>(target,listener);
    }
    if(callback) {
        u32 argument=callbackArgument;
        u32 target=callback;
        gabi::call_ptr<void>(target,argument);
    }
    auto endWords=audioFrameTime();
    u64 end=(static_cast<u64>(endWords.r3)<<32)|endWords.r4;
    if(havePreviousTiming) {
        if(previousProfile) {
            gabi::FrameLocal<be<u64>> priorBegin(0x18),priorEnd(0x20);
            *priorBegin=static_cast<u64>(previousBegin); *priorEnd=static_cast<u64>(previousEnd);
            publishProfile(previousProfile,priorBegin.get(),priorEnd.get(),samples);
            previousProfile=nullptr;
        }
        if(profiles>1) {
            auto* records=reinterpret_cast<AudioProfileRecord*>(oldBuffer);
            gabi::FrameLocal<be<u64>> priorBegin(8),priorEnd(0x10);
            for(u32 index=0;index<profiles-1;++index) {
                *priorBegin=static_cast<u64>(previousBegin); *priorEnd=static_cast<u64>(previousEnd);
                publishProfile(&records[index],priorBegin.get(),priorEnd.get(),samples);
            }
            previousProfile=&records[profiles-1];
        } else if(profiles) previousProfile=reinterpret_cast<AudioProfileRecord*>(oldBuffer);
    } else if(profiles) previousProfile=&reinterpret_cast<AudioProfileRecord*>(oldBuffer)[profiles-1];
    previousBegin=begin; previousEnd=end; havePreviousTiming=1;
    unlockAudioFrame(mutex);
}
/* WWHD glue
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
ext C0009C98 Pair32 audioFrameTime()
ext C0006DE0 u32 swapAudioProfileBuffer(void*,u32)
ext 0289F364 nw::snd::PlaybackVoicePool* nw::snd::PlaybackVoicePool::instance()
ext 0289F82C void nw::snd::PlaybackVoicePool::updatePlayback()
ext 0289F7B4 void nw::snd::PlaybackVoicePool::updateParameters()
ext 028A4DE0 nw::snd::WaveChannelPool* nw::snd::WaveChannelPool::instance()
ext 028A5008 void nw::snd::WaveChannelPool::update()
ext 028AA234 void nw::snd::SoftwareMixer::update()
ext 028AA23C u32 nw::snd::SoftwareMixer::render()
ext 028A88EC void nw::snd::HardwareVoice::restorePriorities()
ext 02888A50 nw::snd::AudioMixRuntime* nw::snd::AudioMixRuntime::create(nw::snd::AudioMixRuntime*)
ext 02889594 void nw::snd::AudioMixRuntime::updateVolumes()
ext 0289CE14 u32 nw::snd::nextRandomHalfword()
*/
