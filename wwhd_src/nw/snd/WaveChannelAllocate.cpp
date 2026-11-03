/* Channel/voice allocation handshake. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
#include "nw/snd/AudioEventLog.h"
using namespace nw::snd;
// the sound driver's event log (function-local static of an inlined accessor, shared with NW-SND#5/#7/#8)
struct EventLogGuard { be<u32> value; };
static AudioEventLog sEventLog;
static EventLogGuard sEventLogGuard;

/* 028A3DA0 */
WaveChannel* WaveChannel::allocate(u32 channelCount,u32 priority,u32 callback,u32 argument,u32 mode) {
    gabi::AutoFrame frame{0x028A3DA0};
    WaveChannel* channel = WaveChannelPool::instance()->allocate();
    if (!channel) {
        if (!sEventLogGuard.value) {
            sEventLog.mWriting = &sEventLog.mBuffers[0];
            sEventLog.mBuffers[1].mCount = 0;
            sEventLogGuard.value = 1;
            sEventLog.mBuffers[0].mCount = 0;
        }
        sEventLog.append(0, 0, 0);
        return nullptr;
    }
    channel->allocated = 1;
    PlaybackVoicePool* pool = PlaybackVoicePool::instance();
    PlaybackVoice* voice = pool->acquire(channelCount,priority,
        gabi::guest_fn((const void*)&voiceEvent),gabi::ea(channel),mode);
    if (!voice) {
        WaveChannelPool::instance()->release(channel);
        return nullptr;
    }
    channel->voice = voice;
    channel->initialize(callback,argument);
    return channel;
}

/* WWHD glue
 data sEventLog 101FE250
 data sEventLogGuard 101FE24C
 ext 0289D1D8 void nw::snd::AudioEventLog::append(u32,u32,u32)
 ext 0289F364 nw::snd::PlaybackVoicePool* nw::snd::PlaybackVoicePool::instance()
 ext 0289F694 nw::snd::PlaybackVoice* nw::snd::PlaybackVoicePool::acquire(s32,u32,u32,u32,u32)
*/
