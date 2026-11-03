/* Sound heap lists, reconstructed from WWHD (the list node and PrioritySoundQueue are in BasicSound.h). */
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace snd {
struct SoundHeapAllocation {
    SoundListNode link;
    gptr<void> data;
    be<u32> size;
    be<u32> callback;
    be<u32> callbackArgument;
};
struct SoundHeapLevel {
    SoundListNode link;
    be<u32> count;
    SoundListNode sentinel;
    be<u8> releaseEnabled;
    u8 padding[3];
    static SoundHeapLevel* create(SoundHeapLevel* level);
    void destroy(u32 flags);
    void append(SoundHeapAllocation* allocation);
};
WWHD_OFFSET(SoundHeapAllocation, data, 8);
WWHD_OFFSET(SoundHeapAllocation, callback, 16);
WWHD_SIZE(SoundHeapAllocation, 24);
WWHD_OFFSET(SoundHeapLevel, count, 8);
WWHD_OFFSET(SoundHeapLevel, sentinel, 12);
WWHD_OFFSET(SoundHeapLevel, releaseEnabled, 20);
WWHD_SIZE(SoundHeapLevel, 24);
struct SoundHeap {
    gptr<nw::ut::FrameHeap> allocator;
    be<u32> count;
    SoundListNode sentinel;
    static SoundHeap* create(SoundHeap* heap);
    u32 releaseTogether(u32 level);
    void clearLevels();
    void finalize();
    void destroy(u32 flags);
    u32 pushLevel();
    u32 initialize(u32 buffer, u32 size);
    void* allocate(u32 size, u32 callback, u32 argument);
    void reset();
    u32 totalSize();
};
WWHD_OFFSET(SoundHeap, count, 4);
WWHD_OFFSET(SoundHeap, sentinel, 8);
WWHD_SIZE(SoundHeap, 16);
}}
nw::snd::SoundListNode* insertSoundList(void*, nw::snd::SoundListIterator*, nw::snd::SoundListNode*);
nw::snd::SoundListNode* removeSoundList(void*, nw::snd::SoundListNode*);

nw::snd::SoundListNode* eraseSoundListRange(void*, nw::snd::SoundListIterator*, nw::snd::SoundListIterator*);
