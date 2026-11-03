// Stream playback state transitions reconstructed from WWHD instructions.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;
void finalizeStreamReader(void*);
void deleteStreamPlayer(void*);
using AdpcmDestructor=void(*)(StreamAdpcmState*,u32);
using ChannelDestructor=void(*)(StreamPlaybackChannel*,u32);
void destroyStreamChannels(void*,u32,u32,ChannelDestructor,u32,u32);
void destroyStreamStates(void*,u32,u32,AdpcmDestructor,u32,u32);
void destroyStreamAdpcm(StreamAdpcmState*,u32);
struct PlaybackLifecycleTable { be<u32> words[18]; };
static PlaybackLifecycleTable sPlaybackLifecycleTable;
struct ListenerLifecycleTable { be<u32> words[6]; };
static ListenerLifecycleTable sListenerLifecycleTable;
/* 02899900 */
void StreamPlayback::releaseChannels() {
    u32 count=channelCount;
    for(u32 index=0;index<count;++index) {
        void* storage=channels[index].storage;
        if(storage) {
            pool->release(storage);
            channels[index].storage=nullptr;
            count=channelCount;
        }
    }
}
/* 02899970 */
void StreamPlayback::releaseVoices() {
    u32 count=channelCount;
    for(u32 index=0;index<count;++index) {
        PlaybackVoice* voice=channels[index].voice;
        if(voice) {
            voice->release();
            channels[index].voice=nullptr;
            count=channelCount;
        }
    }
}
/* 028999F0 */
void StreamPlayback::finalize() {
    if(active) {
        cancelRequested=1;
        releaseChannels();
        releaseVoices();
        finalizeStreamReader(&sourceState);
        pool=nullptr;
        reinterpret_cast<ChannelParameters*>(this)->finish();
        active=0;
        started=0;
    }
}
/* 02899A64 */
void StreamPlayback::destroy(u32 flags) {
    if(!this) return;
    vtable=reinterpret_cast<StreamPlaybackVtable*>(&sPlaybackLifecycleTable);
    listenerVtable=gabi::ea(&sListenerLifecycleTable);
    finalize();
    destroyStreamChannels(channels,16,0x4C8,&StreamPlaybackChannel::destroy,0,0);
    sourceState.destroy(2);
    event.destroy(2);
    if(flags&1) deleteStreamPlayer(this);
}
/* 02899B04 */
void StreamPlayback::updatePause() {
    u32 pause=paused ? 1 : 0;
    if(loadingPaused) pause=1;
    if(pause==static_cast<u8>(effectivePaused)) return;
    u32 count=channelCount;
    for(u32 index=0;index<count;++index) {
        PlaybackVoice* voice=channels[index].voice;
        if(voice) { voice->requestPause(pause); count=channelCount; }
    }
    effectivePaused=pause;
}
/* 0289B0E0 */
void StreamPlayback::setPaused(u32 pause) {
    paused=pause;
    if(pause) loadingPaused=1;
    updatePause();
}
/* 0289BDE4 */
void StreamPlaybackChannel::destroy(StreamPlaybackChannel* channel,u32 flags) {
    if(!channel) return;
    destroyStreamStates(channel->adpcm,32,32,&destroyStreamAdpcm,0,0);
    if(flags&1) deleteStreamPlayer(channel);
}
/* WWHD glue
data sPlaybackLifecycleTable 10182368
data sListenerLifecycleTable 101823B0
ext 0289D598 void nw::snd::PlaybackVoice::release()
ext 0289D8BC void nw::snd::PlaybackVoice::requestPause(u32)
ext 028AE5F8 void finalizeStreamReader(void*)
ext 028A370C void nw::snd::ChannelParameters::finish()
ext 028AE46C void nw::snd::SequenceManager::destroy(u32)
ext 0273AF40 void deleteStreamPlayer(void*)
ext 028F0164 void destroyStreamChannels(void*,u32,u32,ChannelDestructor,u32,u32)
ext 028F0164 void destroyStreamStates(void*,u32,u32,AdpcmDestructor,u32,u32)
*/
