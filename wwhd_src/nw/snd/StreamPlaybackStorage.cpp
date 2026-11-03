// Stream state construction and buffer reservation, reconstructed from WWHD.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;
void* allocateStreamPlayback(u32);
void deleteStreamPlayback(void*);
using StreamMixConstructor=ChannelMixState* (*)(ChannelMixState*);
using StreamOutputConstructor=ChannelMixGroup* (*)(ChannelMixGroup*);
using StreamAdpcmConstructor=StreamAdpcmState* (*)(StreamAdpcmState*);
using StreamChannelConstructor=StreamPlaybackChannel* (*)(StreamPlaybackChannel*);
using StreamTrackConstructor=StreamTrackParam* (*)(StreamTrackParam*);
void constructStreamMixArray(void*,u32,u32,StreamMixConstructor);
void constructStreamOutputArray(void*,u32,u32,StreamOutputConstructor);
void constructStreamAdpcmArray(void*,u32,u32,StreamAdpcmConstructor);
void constructStreamChannelArray(void*,u32,u32,StreamChannelConstructor);
void constructStreamTrackArray(void*,u32,u32,StreamTrackConstructor);

void destroyStreamAdpcm(StreamAdpcmState*,u32);
struct StreamPlaybackTable { be<u32> words[18]; };
static StreamPlaybackTable sStreamPlaybackTable;
struct StreamListenerTable { be<u32> words[6]; };
static StreamListenerTable sStreamListenerTable;
/* 0289BE4C */
ChannelMixState* nw::snd::constructStreamMixState(ChannelMixState* value) {
    if(!value) value=static_cast<ChannelMixState*>(allocateStreamPlayback(0x18));
    if(value) {
        value->pitch=1.0f; value->auxiliarySend=0.0f; value->pan=0.0f;
        value->mainSend=0.0f; value->gain=1.0f; value->surroundPan=0.0f;
    }
    return value;
}
/* 0289BEA8 */
ChannelMixGroup* nw::snd::constructStreamMixGroup(ChannelMixGroup* value) {
    if(!value) value=static_cast<ChannelMixGroup*>(allocateStreamPlayback(0x50));
    if(value) constructStreamMixArray(value->channels,2,0x18,&constructStreamMixState);
    return value;
}
/* 0289BF14 */
StreamAdpcmState* StreamAdpcmState::construct(StreamAdpcmState* value) {
    if(!value) value=static_cast<StreamAdpcmState*>(allocateStreamPlayback(0x20));
    if(value) {
        value->position=0; value->loaded=0; value->state=0; value->reserved=0;
        for(u32 index=0;index<4;++index) value->coefficients[index]=0;
    }
    return value;
}
/* 0289BF6C */
StreamPlaybackChannel* StreamPlaybackChannel::construct(StreamPlaybackChannel* value) {
    if(!value) value=static_cast<StreamPlaybackChannel*>(allocateStreamPlayback(0x4C8));
    if(value) constructStreamAdpcmArray(value->adpcm,32,32,&StreamAdpcmState::construct);
    return value;
}
/* 0289BFC4 */
StreamTrackParam* StreamTrackParam::construct(StreamTrackParam* value) {
    if(!value) value=static_cast<StreamTrackParam*>(allocateStreamPlayback(0x110));
    if(value) {
        constructStreamMixArray(value->mix,2,0x18,&constructStreamMixState);
        constructStreamOutputArray(value->outputs,2,0x50,&constructStreamMixGroup);
    }
    return value;
}
/* 0289AADC */
StreamPlayback* StreamPlayback::construct(StreamPlayback* value) {
    if(!value) value=static_cast<StreamPlayback*>(allocateStreamPlayback(0x7144));
    if(value) {
        ChannelParameters::construct(reinterpret_cast<ChannelParameters*>(value));
        value->active=0;
        value->link.next=nullptr;
        value->listenerVtable=gabi::ea(&sStreamListenerTable);
        value->link.previous=nullptr;
        value->vtable=reinterpret_cast<StreamPlaybackVtable*>(&sStreamPlaybackTable);
        SequenceManager::construct(&value->sourceState);
        value->channelCount=0; value->trackCount=0;
        constructStreamChannelArray(value->channels,16,0x4C8,&StreamPlaybackChannel::construct);
        constructStreamTrackArray(value->tracks,8,0x110,&StreamTrackParam::construct);
    }
    return value;
}
/* 0289B5FC */
u32 StreamPlayback::allocateChannels() {
    u32 count=channelCount;
    for(u32 index=0;index<count;++index) {
        void* memory=pool->allocate();
        if(!memory) {
            for(u32 prior=0;prior<index;++prior) {
                AudioBlockPool* allocator=pool;
                void* allocated=channels[prior].storage;
                allocator->release(allocated);
                channels[prior].storage=nullptr;
            }
            return 0;
        }
        channels[index].storage=memory;
        count=channelCount;
    }
    return 1;
}
/* 0289B6CC */
u32 StreamPlayback::configureBlocks() {
    u32 payload=blockPayloadBytes;
    AudioBlockPool* allocator=pool;
    u32 bytes=allocator->blockBytes;
    if(payload>0x2000) return 0;
    u32 count=bytes/(payload+0x900);
    if(count<4) { blockCount=count; return 0; }
    if(count>32) count=32;
    blockCount=count;
    readBlock=0; completedBlock=0; writeBlock=0;
    return 1;
}
/* 0289BDD0 */
void destroyStreamAdpcm(StreamAdpcmState* value,u32 flags) {
    if(value && (flags&1)) deleteStreamPlayback(value);
}
/* WWHD glue
data sStreamPlaybackTable 10182368
data sStreamListenerTable 101823B0
ext 0273AD10 void* allocateStreamPlayback(u32)
ext 0273AF40 void deleteStreamPlayback(void*)
ext 028EFFD0 void constructStreamMixArray(void*,u32,u32,StreamMixConstructor)
ext 028EFFD0 void constructStreamOutputArray(void*,u32,u32,StreamOutputConstructor)
ext 028EFFD0 void constructStreamAdpcmArray(void*,u32,u32,StreamAdpcmConstructor)
ext 028EFFD0 void constructStreamChannelArray(void*,u32,u32,StreamChannelConstructor)
ext 028EFFD0 void constructStreamTrackArray(void*,u32,u32,StreamTrackConstructor)
ext 028A3610 nw::snd::ChannelParameters* nw::snd::ChannelParameters::construct(nw::snd::ChannelParameters*)
ext 028AE2E8 nw::snd::SequenceManager* nw::snd::SequenceManager::construct(nw::snd::SequenceManager*)
*/
