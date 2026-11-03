/* Wave sample range invalidation. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
#include "nw/snd/AudioCurves.h"
using namespace nw::snd;

/* 028A4AC8 */
void WaveChannel::invalidateRange(const void* start,const void* end) {
    gabi::AutoFrame frame{0x028A4AC8};
    WaveChannel* channel = owner;
    PlaybackVoice* current = channel->voice;
    if (!current) return;
    s32 count = current->mChannelCount;
    if (count <= 0) return;
    bool invalidated = false;
    for (u32 remaining = (u32)count; remaining; --remaining) {
        for (u32 entry = 0; entry < 2; ++entry) {
            channel = owner;
            WaveSampleData* data = &channel->sampleData[((u32)count - remaining) * 2 + entry];
            if (data->state == RequestDone) continue;
            current = channel->voice;
            const void* base = data->samples;
            u32 format = current->sampleFormat();
            u32 length = sampleStorageBytes(data->sampleCount,format);
            u32 limit = gabi::ea(base) + length;
            if (gabi::ea(start) > limit) continue;
            const void* dataStart = data->samples;
            if (gabi::ea(end) < gabi::ea(dataStart)) continue;
            invalidated = true;
            break;
        }
    }
    if (invalidated) {
        owner->notify(3);
        owner->stop();
        owner->releaseToPool();
    }
}

/* WWHD glue
 ext 0289EF74 u32 nw::snd::PlaybackVoice::sampleFormat() const
 ext 0289CE3C u32 nw::snd::sampleStorageBytes(u32,u32)
*/
