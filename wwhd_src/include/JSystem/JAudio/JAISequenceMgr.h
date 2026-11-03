/* Sequence manager and Cafe interpolation constructor thunks, following CC0 tww. */
#pragma once
#include "JSystem/JAudio/JAISoundParams.h"
#include "JSystem/JAudio/JASCmdStack.h"
#include "JSystem/JAudio/JASTrack.h"
#include "JSystem/JAudio/JASOuterParam.h"
struct JAISound;
namespace JAInter {
struct MuteBit { be<u8> mFlags; };
WWHD_SIZE(MuteBit,1);
struct SeqUpdateData;
struct SeqParameter {
    MoveParaSet mTempo;
    MoveParaSet mPortParameters[16];
    MoveParaSet mVolumes[20];
    gptr<MoveParaSet> seqPan,seqPitch,seqFxmix,seqDolby;
    MoveParaSet mTrackVolumes[32];
    MoveParaSetInitHalf mTrackPans[32];
    MoveParaSet mTrackPitches[32];
    MoveParaSetInitZero mTrackFxmixes[32];
    MoveParaSetInitZero mTrackDolbys[32];
    MoveParaSetInitZero mTrackAuxMixes[32];
    be<s16> mTrackPorts[32][16];
    be<u8> mHeapIndex,mState;
    be<u16> mPrepareFlags;
    be<s32> mHeapNumber;
    be<u32> mParameterFlags[13];
    be<u32> mPortFlags[32];
    be<u8> mTrackStates[32];
    MuteBit mMuteBits[32];
    gptr<SeqUpdateData> mUpdateData;
    JASystem::TTrack mTrack;
    TOuterParam mOuterParam;
};
WWHD_OFFSET(SeqParameter,mVolumes,0x110);
WWHD_OFFSET(SeqParameter,seqPan,0x250);
WWHD_OFFSET(SeqParameter,mTrackVolumes,0x260);
WWHD_OFFSET(SeqParameter,mTrackPorts,0xE60);
WWHD_OFFSET(SeqParameter,mState,0x1261);
WWHD_OFFSET(SeqParameter,mPortFlags,0x129C);
WWHD_OFFSET(SeqParameter,mMuteBits,0x133C);
WWHD_OFFSET(SeqParameter,mTrack,0x1360);
WWHD_OFFSET(SeqParameter,mOuterParam,0x16C8);
WWHD_SIZE(SeqParameter,0x16F4);
union SequencePortValue { be<f32> asFloat; be<u32> asWord; };
struct PlayerParameter {
    gptr<JASystem::TTrack> mTrack;
    union { JASystem::Kernel::TPortArgs asStruct; SequencePortValue asArray[10]; } mPortArgs;
    JASystem::Kernel::TPortCmd mCommand;
};
struct SeqUpdateData {
    be<u8> mState,mReadState,mPrepareFlag,mUpdateState;
    be<u32> mUpdateFlags,mActiveTrackFlag;
    be<f32> mSeqVolume,mSeqPitch,mSeqFxmix,mSeqPan,mSeqDolby,mSeqTempo;
    gptr<be<f32>> trackVolume,trackPitch,trackFxmix,trackPan,trackDolby;
    be<u32> mReadOffset,mWaveLoadState;
    gptr<be<u8>> mTrackState;
    gptr<be<u32>> trackupdate;
    gptr<JAISound> mSequence;
    gptr<PlayerParameter> systemTrackParameter;
};
WWHD_OFFSET(PlayerParameter,mPortArgs,4);
WWHD_OFFSET(PlayerParameter,mCommand,0x2C);
WWHD_SIZE(PlayerParameter,0x44);
WWHD_OFFSET(SeqUpdateData,trackupdate,0x44);
WWHD_OFFSET(SeqUpdateData,systemTrackParameter,0x4C);
WWHD_SIZE(SeqUpdateData,0x50);
}
namespace JAInter { namespace SequenceMgr {
void checkReadSeq();
void checkPlayingSeq();
void checkPlayingSeqUpdateMultiplication(u32,u32,u32,MoveParaSet*,be<u32>*,u32,be<f32>*);
void checkPlayingSeqUpdateAddition(u32,u32,u32,MoveParaSet*,be<u32>*,u32,be<f32>*,f32);
void checkPlayingSeqUpdateTrack(u32,u32,MoveParaSet*,be<u32>*,u32,be<f32>*);
void stopSeq(JAISound*);
void checkFadeoutSeq();
void checkStoppedSeq();
void checkStartedSeq();
void releaseSeqBuffer(JAISound*,u32);
void checkDvdLoadArc(u32,u32);
void checkSeqWave();
void processGFrameSequence();
SeqUpdateData* getPlayTrackInfo(u32);
}}
void sequenceCheckEntried();
void sequenceCheckFadeout();
void sequenceCheckStopped();
void sequenceCheckPlaying();
void sequenceCheckStarted();
void sequenceCheckRead();
void sequenceCheckWave();

void* sequenceObjectAllocate(u32);
void sequencePortLinkConstruct(JSUPtrLink*,void*);

using SequenceMoveInitializer=void(*)(JAInter::MoveParaSet*);
using SequenceMoveConstructor=JAInter::MoveParaSet*(*)(JAInter::MoveParaSet*);
using SequenceMuteConstructor=JAInter::MuteBit*(*)(JAInter::MuteBit*);
void constructSequenceMoveArray(void*,u32,u32,SequenceMoveInitializer);
void constructSequenceAllocatingMoveArray(void*,u32,u32,SequenceMoveConstructor);
void constructSequenceMuteArray(void*,u32,u32,SequenceMuteConstructor);
void constructSequenceTrack(JASystem::TTrack*);
void constructSequenceOuter(TOuterParam*);

void sequenceAssert(const char*,u32,const char*);
u32 sequenceTrackMax();
void* sequenceHeapAllocate(u32,JKRHeap*,s32);
using SequencePlayerConstructor=JAInter::PlayerParameter*(*)(JAInter::PlayerParameter*);
JAInter::PlayerParameter* constructSequencePlayerArray(void*,u32,u32,SequencePlayerConstructor,u32);


void sequenceRegister(void*);
void sequenceGuardConstruct(void*);

namespace JAInter { namespace SequenceMgr {
struct SequenceArchiveEntry { gptr<be<u8>> mData; be<u32> mSize; };
struct SequenceArchive { be<u32> mCount; u8 reserved[28]; SequenceArchiveEntry mEntries[1]; };
WWHD_OFFSET(SequenceArchive,mEntries,0x20);
WWHD_SIZE(SequenceArchiveEntry,8);
void setArchivePointer(SequenceArchive*);
void getArchiveName(be<u8>*);
}}
be<u8>* sequenceAudioPath();
be<u8>* sequenceArchivePath();
be<u8>* sequenceArchiveFilename();

u32 sequencePlayTrackMax();
u32 sequenceSoundID(JAISound*);

void sequenceSetHeapLoaded(u32,u32);
void sequenceReleaseHeap(u32);

void sequenceSetInterVolume(JAISound*,u32,f32,u32);
f32 sequenceInterVolume(JAISound*,u32);
void sequenceTrackStop(JASystem::TTrack*);
u32 sequenceActiveFlag(JASystem::TTrack*);
void sequenceTrackInit(JAInter::SeqUpdateData*);

void sequenceSetPortF32(JAInter::SeqUpdateData*,u32,u32,f32);

void sequencePlayingTrack(u32);
void sequenceSetPortU32(JAInter::SeqUpdateData*,u32,u32,u32);

u32 sequenceArchiveSize(JAInter::SequenceMgr::SequenceArchive*,u32);
u32 sequenceSetData(JASystem::TTrack*,void*);
void sequencePauseMode(JAISound*,u32,u32);
void sequenceRootInit(JAInter::SeqUpdateData*);
u32 sequenceStartTrack(JASystem::TTrack*);

namespace JAInter { namespace SequenceMgr { void init(); }}
u32 sequenceControlMax();
u32 sequenceParameterLines();
void* sequenceParameterAllocate(u32,JKRHeap*,s32);
void sequenceInitPan(JAInter::MoveParaSet*);
using SequenceUpdateConstructor=JAInter::SeqUpdateData*(*)(JAInter::SeqUpdateData*);
JAInter::MoveParaSet* sequenceConstructDynamicMoves(void*,u32,u32,SequenceMoveInitializer,u32);
JAInter::SeqUpdateData* sequenceConstructUpdates(void*,u32,u32,SequenceUpdateConstructor,u32);

namespace JAInter { struct HeapBlock; namespace SequenceMgr { void checkEntriedSeq(); }}
void* sequenceFindHeap(u32,be<u8>*);
void* sequenceStayHeap(u32,u32);
u32 sequenceUsefulHeap();
u32 sequenceAutoHeapMax();
u32 sequenceAutoHeapSize();
JAInter::HeapBlock* sequenceAutoHeaps();
void* sequenceAllocateAutoHeap(u32,u32);
using SequenceLoadCallback=void(*)(u32,u32);
u32 sequenceLoadAsync(JAInter::SequenceMgr::SequenceArchive*,u32,void*,u32,SequenceLoadCallback,u32);
u32 sequenceLoadSync(JAInter::SequenceMgr::SequenceArchive*,u32,void*,u32);

struct JAISoundInfoView;
namespace JAInter { struct Actor; namespace SequenceMgr { void storeSeqBuffer(gptr<JAISound>*,Actor*,u32,u32,u32,JAISoundInfoView*); }}
void sequenceSetOuter(JASystem::TTrack*,TOuterParam*);

namespace JAInter { namespace SequenceMgr {void checkPlayingSeqTrack(u32);}}
u32 sequenceCameraMax();
void sequenceTransform(void*,cXyz*,cXyz*);
f64 sequenceSqrt(f64);
f64 sequenceDistanceMax();
u32 sequenceDistanceMoveTime();
u32 sequenceDopplerTime();
void sequenceSetInterPan(JAISound*,u32,f32,u32);
void sequenceSetInterPitch(JAISound*,u32,f32,u32);
u32 sequenceReadPort(JASystem::TTrack*,u32,be<u16>*);
u32 sequenceWritePort(JASystem::TTrack*,u32,u32);
JASystem::TTrack* sequenceChildTrack(JAISound*,u32);
void sequenceTrackMute(JASystem::TTrack*,u32);
