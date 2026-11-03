/* Wave-channel pool bookkeeping. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;
void* allocateWavePool(u32 size);
u32 initializeWavePoolStorage(WaveChannelPool* pool, void* storage, u32 size, u32 stride);
void finishWavePoolStorage(WaveChannelPool* pool);
WaveChannel* allocateWavePoolSlot(WaveChannelPool* pool);
void releaseWavePoolSlot(WaveChannelPool* pool, WaveChannel* channel);
SoundRangeLink* insertWavePoolLink(SoundRangeListeners* list, gptr<SoundRangeLink>* position, SoundRangeLink* node);
SoundRangeLink* removeWavePoolLink(SoundRangeListeners* list, SoundRangeLink* node);
static WaveChannelPool sWaveChannelPool;
static u32 sWaveChannelPoolGuard;

/* 028A4D58 */
WaveChannelPool* WaveChannelPool::construct(WaveChannelPool* pool) {
    gabi::AutoFrame frame{0x028A4D58};
    if (!pool) pool = (WaveChannelPool*)allocateWavePool(sizeof(WaveChannelPool));
    if (pool) {
        pool->freeCount = 0;
        pool->storage = nullptr;
        // Inlined member constructor with GHS's null test of the member: address formed by hand.
        SoundRangeListeners* list = gabi::at<SoundRangeListeners>(gabi::ea(pool) + 0xC);
        pool->capacity = 0;
        if (!list) list = (SoundRangeListeners*)allocateWavePool(sizeof(SoundRangeListeners));
        if (list) {
            list->count = 0;
            list->root.previous = &list->root;
            list->root.next = &list->root;
        }
        pool->count = 0;
        pool->initialized = 0;
    }
    return pool;
}

/* 028A4DE0 */
WaveChannelPool* WaveChannelPool::instance() {
    gabi::AutoFrame frame{0x028A4DE0};
    if (!sWaveChannelPoolGuard) {
        sWaveChannelPoolGuard = 1;
        construct(&sWaveChannelPool);
    }
    return &sWaveChannelPool;
}

/* 028A4E2C */
u32 WaveChannelPool::requiredBytes(void* unused, u32 count) {
    return (count << 1) * sizeof(WaveChannel);
}

/* 028A4E38 */
void WaveChannelPool::initialize(void* memory, u32 size) {
    gabi::AutoFrame frame{0x028A4E38};
    if (initialized) return;
    count = initializeWavePoolStorage(this, memory, size, sizeof(WaveChannel));
    initialized = 1;
}

/* 028A4E84 */
void WaveChannelPool::release(WaveChannel* channel) {
    gabi::AutoFrame frame{0x028A4E84};
    // The link address is formed before the null test of channel, as GHS does.
    SoundRangeLink* node = gabi::at<SoundRangeLink>(gabi::ea(channel) + 0x2E0);
    removeWavePoolLink(&channels, node);
    if (channel) {
        WaveChannel::destroy(channel,2);
        releaseWavePoolSlot(this, channel);
    }
}

/* 028A4EE4 */
void WaveChannelPool::finish() {
    gabi::AutoFrame frame{0x028A4EE4};
    if (!initialized) return;
    SoundRangeLink* node = channels.root.next;
    SoundRangeLink* end = &channels.root;
    while (node != end) {
        // The pool list links WaveChannel::poolLink; step back to the channel.
        WaveChannel* channel = gabi::at<WaveChannel>(gabi::ea(node) - 0x2E0);
        node = node->next;
        channel->stop();
        channel->notify(3);
        release(channel);
    }
    finishWavePoolStorage(this);
    initialized = 0;
}

/* 028A4F80 */
WaveChannel* WaveChannelPool::allocate() {
    gabi::AutoFrame frame{0x028A4F80};
    WaveChannel* storage = allocateWavePoolSlot(this);
    if (!storage) return nullptr;
    WaveChannel* channel = WaveChannel::construct(storage);
    if (!channel) return nullptr;
    gabi::FrameLocal<gptr<SoundRangeLink>> position(8);
    *position = &channels.root;
    insertWavePoolLink(&channels, position.get(), &channel->poolLink);
    return channel;
}

/* WWHD glue
 data sWaveChannelPool 104BC2D8
 data sWaveChannelPoolGuard 104BC2F8
 ext 0273AD10 void* allocateWavePool(u32)
 ext 0288A184 u32 initializeWavePoolStorage(nw::snd::WaveChannelPool*,void*,u32,u32)
 ext 0288A1D0 void finishWavePoolStorage(nw::snd::WaveChannelPool*)
 ext 0288A248 nw::snd::WaveChannel* allocateWavePoolSlot(nw::snd::WaveChannelPool*)
 ext 0288A26C void releaseWavePoolSlot(nw::snd::WaveChannelPool*,nw::snd::WaveChannel*)
 ext 0286DA70 nw::snd::SoundRangeLink* insertWavePoolLink(nw::snd::SoundRangeListeners*,gptr<nw::snd::SoundRangeLink>*,nw::snd::SoundRangeLink*)
 ext 0286DA9C nw::snd::SoundRangeLink* removeWavePoolLink(nw::snd::SoundRangeListeners*,nw::snd::SoundRangeLink*)
*/
