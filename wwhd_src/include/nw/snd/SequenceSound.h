/* Sequence sound extension, reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/SoundArchive.h"
namespace nw { namespace snd {
struct SequenceSound;
struct SequenceManager;
struct SequenceSoundHandle {
    gptr<SequenceSound> mSound;
    void detach();
};
struct SequenceSoundPlayer_l {
    SequencePlayer_l mBase;
    u8 mReserved280[0x18];
    be<u8> mPrepareState;
    u8 mSuffix[0x4B];
};
struct SequenceConfiguration {
    be<u32> mHeader;
    be<u32> mFirst[4];
    be<u32> mSecond[4];
    be<u8> mFlags[4];
};
struct SequenceCommandParameters { be<u32> mValue; be<u32> mOption; be<s32> mCount; };
struct SequenceCommandPair { be<u32> mFirst; be<u32> mSecond; };
struct SequenceReferenceConfiguration {
    be<u32> mFirst; be<u32> mSecond;
    gptr<const SequenceCommandPair> mPairs[5];
    be<u32> mLast;
};
struct SequenceSound {
    BasicSound mBasic;
    gptr<SequenceSoundHandle> mHandle;
    gptr<SequenceManager> mManager;
    be<u8> mActive;
    u8 mPadding[3];
    SequenceSoundPlayer_l mPlayer;
    static SequenceSound* construct(SequenceSound* sound,void* manager);
    void sendSetupValues(u32 first,u32 second,u32 third,u32 flag,u32 secondFlag,u32 fourth,u32 fifth);
    void sendRange(u32 first,s32 second);
    void sendConfiguration(const SequenceConfiguration* config,const SequenceCommandParameters* parameters);
    void sendReferenceConfiguration(const SequenceReferenceConfiguration* config,const SequenceCommandParameters* parameters);
    void destroy(u32 flags);
    void initialize();
    void finalize();
    void updatePriority();
    u32 hasHandle() const;
    void detachHandle();
    u32 isPrepared() const;
    void updateParameters();
    SequenceSoundPlayer_l* driverPlayer();
};
static_assert(sizeof(SequenceSoundPlayer_l)==0x2E4);
static_assert(sizeof(SequenceSound)==0x588);
static_assert(offsetof(SequenceSound,mHandle)==0x298);
static_assert(offsetof(SequenceSound,mManager)==0x29C);
static_assert(offsetof(SequenceSound,mActive)==0x2A0);
static_assert(offsetof(SequenceSound,mPlayer)==0x2A4);
}}
