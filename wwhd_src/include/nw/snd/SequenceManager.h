/* Sequence manager lifecycle views reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/SequenceSound.h"
namespace nw { namespace snd {
struct SequenceListNode { gptr<SequenceListNode> next,previous; };
struct SequenceList { be<u32> count; SequenceListNode sentinel; };
struct SequenceAsyncJob_l {
    u8 prefix[8]; nw::ut::Event event; u8 reserved[8];
    gptr<const be<u32>> vtable; be<u32> argument;
};
struct SequenceAsyncTask_l {
    SequenceAsyncJob_l job; u8 reserved[0x48]; SequenceListNode link;
};
struct SequenceManager {
    be<u32> mState;
    be<u32> mReserved4;
    SequenceList mSounds;
    SequenceList mFreeSounds;
    be<u32> mPoolState[4];
    u8 mReserved30[0x20];
    u8 mFlags[0x27F];
    u8 mReserved2CF[0x49];
    SequenceAsyncJob_l mJobs[2];
    SequenceList mTasks;
    be<u32> mTaskPool[3];
    u8 mTaskStorage[0x1180];
    gptr<void> mAllocation;
    u8 mSuffix[0x328];
    static SequenceManager* construct(SequenceManager* manager);
    void waitForTasks();
    void destroy(u32 flags);
};
static_assert(sizeof(SequenceAsyncJob_l)==0x3C);
static_assert(sizeof(SequenceAsyncTask_l)==0x8C);
static_assert(offsetof(SequenceAsyncTask_l,link)==0x84);
static_assert(sizeof(SequenceManager)==0x1854);
static_assert(offsetof(SequenceManager,mJobs)==0x318);
static_assert(offsetof(SequenceManager,mTasks)==0x390);
static_assert(offsetof(SequenceManager,mTaskPool)==0x39C);
static_assert(offsetof(SequenceManager,mTaskStorage)==0x3A8);
static_assert(offsetof(SequenceManager,mAllocation)==0x1528);
}}
