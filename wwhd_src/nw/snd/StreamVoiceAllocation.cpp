// Stream channel voice acquisition and release callbacks, from original instructions.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;
void streamVoiceDropped(PlaybackVoice*,u32,StreamPlaybackChannel*);
/* 0289B724 */
void streamVoiceDropped(PlaybackVoice* voice,u32 event,StreamPlaybackChannel* channel) {
    if(event>3) return;
    if(event<=1) voice->release();
    channel->voice=nullptr;
}
/* 0289B77C */
u32 allocatePlaybackVoices(StreamPlayback* player) {
    u32 count=player->channelCount;
    u32 index=0;
    for(;index<count;++index) {
        PlaybackVoicePool* pool=PlaybackVoicePool::instance();
        u32 priority=gabi::load<u32>(gabi::ea(player)+0x4C);
        PlaybackVoice* voice=pool->acquire(1,255,gabi::guest_fn((const void*)&streamVoiceDropped),gabi::ea(&player->channels[index]),priority);
        if(!voice) {
            for(u32 prior=0;prior<index;++prior) {
                PlaybackVoice* allocated=player->channels[prior].voice;
                if(allocated) { allocated->release(); player->channels[prior].voice=nullptr; }
            }
            return 0;
        }
        player->channels[index].voice=voice;
        count=player->channelCount;
    }
    return 1;
}
/* 0289C034 */
void StreamPlaybackChannel::queueSamples(StreamAdpcmState* samples,u32 flag) {
    PlaybackVoice* output=voice;
    // a StreamAdpcmState is queued as the voice's sample request
    if(output) output->appendRequest(0,reinterpret_cast<VoiceRequest*>(samples),flag);
}
/* WWHD glue (PlaybackVoice::release and PlaybackVoicePool::instance are bound in StreamPlaybackLifecycle.cpp / AudioFrameProcess.cpp)
ext 0289F174 void nw::snd::PlaybackVoice::appendRequest(u32,nw::snd::VoiceRequest*,u32)
ext 0289F694 nw::snd::PlaybackVoice* nw::snd::PlaybackVoicePool::acquire(s32,u32,u32,u32,u32)
*/
