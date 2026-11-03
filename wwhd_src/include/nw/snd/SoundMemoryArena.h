/* Sound arena layout reconstructed from WWHD accesses: 64-byte aligned storage that a sound player
 * lends to its sounds' deferred loads (SoundPlayer::addHeap 02897B34 / takeHeap / returnHeap). */
#pragma once
#include "wwhd.h"
#include "nw/snd/BasicSound.h"
namespace nw { namespace snd {
struct SoundMemoryArena {
    be<u32> mVtable;
    gptr<SoundPlayer> mPlayer;   // set by SoundPlayer::addHeap
    gptr<be<u8>> mBegin;
    gptr<be<u8>> mEnd;
    gptr<be<u8>> mCursor;
    be<u8> mState;               // PlayerHeap_Free: SoundPlayer::returnHeap puts it back on freeHeaps
    u8 mPadding[3];
    SoundListNode mLink;         // SoundPlayer::freeHeaps / busyHeaps
    static SoundMemoryArena* construct(SoundMemoryArena* arena);
    void reset();
    u32 initialize(void* storage,u32 size);
    void* allocate(u32 size);
};
static_assert(sizeof(SoundMemoryArena)==0x20);
static_assert(offsetof(SoundMemoryArena,mBegin)==8);
static_assert(offsetof(SoundMemoryArena,mEnd)==0xC);
static_assert(offsetof(SoundMemoryArena,mCursor)==0x10);
static_assert(offsetof(SoundMemoryArena,mState)==0x14);
static_assert(offsetof(SoundMemoryArena,mLink)==0x18);
}}
