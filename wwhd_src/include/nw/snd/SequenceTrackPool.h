/* Track allocation views reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/SoundArchive.h"
namespace nw { namespace snd {
struct SequenceTrack_l {
    SequenceTrack mBase;
    be<u32> mPoolArgument;
};
struct SequenceTrackPool {
    be<u32> mReserved;
    be<u32> mArgument;
    be<u32> mStorage[3];
    SequenceTrack_l* allocate(void* owner);
    void release(SequenceTrack_l* track);
    void initialize(void* storage,u32 size);
    void reset();
    u32 available() const;
};
static_assert(sizeof(SequenceTrack_l)==0x224);
static_assert(offsetof(SequenceTrack_l,mPoolArgument)==0x220);
static_assert(offsetof(SequenceTrackPool,mStorage)==8);
}}
