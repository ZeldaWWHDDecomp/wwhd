/* Per-tick traversal of allocated wave channels. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;

/* 028A5008 */
void WaveChannelPool::update() {
    gabi::AutoFrame frame{0x028A5008};
    SoundRangeLink* node = channels.root.next;
    SoundRangeLink* end = &channels.root;
    while (node != end) {
        // The pool list links WaveChannel::poolLink; step back to the channel.
        WaveChannel* channel = gabi::at<WaveChannel>(gabi::ea(node) - 0x2E0);
        node = node->next;
        channel->update(1);
        if (!channel->started) {
            channel->notify(0);
            release(channel);
        }
    }
}

/* WWHD glue
*/
