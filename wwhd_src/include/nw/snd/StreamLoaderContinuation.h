/* NW-SND#13 continuation view. */
#pragma once
#include "nw/snd/SequenceManager.h"
#include "nw/snd/SoundResourceDirectories.h"
#include "nw/snd/SoundResourceInput.h"
#include "nw/snd/DriverCommandDispatch.h"
namespace nw { namespace snd {
struct StreamLoaderContinuation_l;
/* The stream player that owns the loader: only its read-cancel flag is read here. */
struct StreamLoaderPlayerPrefix_l { u8 prefix[0x186]; be<u8> readCancelled; };
struct StreamTrackConfiguration_l {
    be<u8> volume,pan,reserved2,reserved3,output,routes[3],send,flags9,flagsA,channelCount,channels[2];
};
struct StreamLoaderConfiguration_l {
    be<u32> encoding,sampleRate; be<u8> looping; u8 reserved9[3];
    be<u32> loopStart; be<f32> pitch; be<u8> outputMode,routes[3];
    be<u32> sampleEnd,blockSamples,blockBytes,paddedBlockBytes,lastBlockSamples,channelCount,trackCount;
    StreamTrackConfiguration_l tracks[8];
};
struct StreamRegion_l { be<u32> start,end; AdpcmLoop histories[16]; u8 padding[152]; };
struct StreamLoaderJob_l {
    u8 prefix[8]; nw::ut::Event event; be<u32> reserved;
    gptr<StreamLoaderContinuation_l> owner;
    gptr<const be<u32>> vtable; gptr<StreamLoaderContinuation_l> argument;
};
struct StreamLoaderTaskVtable_l { u8 prefix[0xC]; gabi::gfn execute; };
struct StreamLoaderTask_l {
    u8 prefix[8]; nw::ut::Event event; be<u32> state;
    gptr<StreamLoaderContinuation_l> owner;
    gptr<const StreamLoaderTaskVtable_l> vtable;
    gptr<const void> buffers[16];
    be<u32> first,second;
    gptr<StreamLoaderContinuation_l> loader;
    SequenceListNode link;
};
struct StreamAdpcmRecord_l { InitialAdpcmState initial; AdpcmLoop loop; };
struct StreamLoaderContinuation_l {
    SoundResourceInput mInput;
    gptr<StreamLoaderPlayerPrefix_l> mPlayer;
    gptr<ResourceStream_l> mRetainedStream;
    gptr<StreamLoaderConfiguration_l> mConfiguration;
    be<u32> mSourceKind;
    gptr<void> mProvider;
    be<u32> mProviderArguments[3];
    be<u32> mChannelCount;
    be<u16> mIdentifierSuffix;
    be<u8> mLooping,mFlag;
    be<u32> mLoopStart,mLoopEnd;
    be<f32> mPitch;
    be<u8> mOutputMode,mRoutes[3];
    gptr<void> mCallback,mCallbackArgument;
    u8 mPath[0x27F]; u8 mReserved2CF;
    gptr<void> mMemoryStream; be<u32> mReserved2D4;
    gptr<void> mBuffer; be<u32> mBufferSize;
    gptr<void> mResourceArgument; be<u32> mResourceMode,mResourceContext;
    be<u32> mCurrentBlock;
    be<s32> mFinalBlock;
    be<u32> mLoopBlock,mDataStart,mRestartPosition,mSkippedSamples;
    be<u8> mInitialBlock,mFinished;
    u8 mReserved306[2];
    be<u32> mReadCursor,mSampleCursor,mRegionStart,mRegionEnd;
    StreamLoaderJob_l mJobs[2];
    SequenceList mTasks;
    be<u32> mTaskPool[3];
    u8 mTaskStorage[0x1180];
    gptr<void> mAllocation;
    be<u32> mMode;
    StreamAdpcmRecord_l mAdpcm[16];
    be<s32> mCachedIndex;
    AdpcmLoop mHistories[16];
    void reset();
    void scheduleRead();
    void releaseCompletedTasks();
    void queueReadTask(const gptr<const void>* buffers,u32 first,u32 second,u32 priority);
    void finish();
    void sendStopCommand();
    void closeStream();
    u32 openStream();
    u32 advanceRegion();
    void sendPrepareCommand();
    void sendReadCommand(const gptr<const void>* buffers,u32 first,u32 second);
    u32 readNativeMetadata(StreamBlockCommand* command);
    u32 readDecodedMetadata(StreamBlockCommand* command);
    u32 readPcmBlock(StreamPrepareCommand* command,const gptr<const void>* buffers,u32 first,u32 second);
    u32 readDecodedBlock(StreamPrepareCommand* command,const gptr<const void>* buffers,u32 first,u32 second);
    void cancelTasks();
    void scheduleFinalTask();
    u32 hasPendingTasks() const;
};
WWHD_SIZE(StreamAdpcmRecord_l,44);
WWHD_SIZE(StreamLoaderContinuation_l,0x1854);
WWHD_OFFSET(StreamLoaderContinuation_l,mChannelCount,0x30);
WWHD_OFFSET(StreamLoaderContinuation_l,mCallback,0x48);
WWHD_OFFSET(StreamLoaderContinuation_l,mCallbackArgument,0x4C);
WWHD_OFFSET(StreamLoaderContinuation_l,mCurrentBlock,0x2EC);
WWHD_OFFSET(StreamLoaderContinuation_l,mFinalBlock,0x2F0);
WWHD_OFFSET(StreamLoaderContinuation_l,mDataStart,0x2F8);
WWHD_OFFSET(StreamLoaderContinuation_l,mInitialBlock,0x304);
WWHD_OFFSET(StreamLoaderContinuation_l,mFinished,0x305);
WWHD_OFFSET(StreamLoaderContinuation_l,mReadCursor,0x308);
WWHD_OFFSET(StreamLoaderContinuation_l,mSampleCursor,0x30C);
WWHD_OFFSET(StreamLoaderContinuation_l,mRegionStart,0x310);
WWHD_OFFSET(StreamLoaderContinuation_l,mRegionEnd,0x314);
WWHD_OFFSET(StreamLoaderContinuation_l,mJobs,0x318);
WWHD_OFFSET(StreamLoaderContinuation_l,mTasks,0x390);
WWHD_OFFSET(StreamLoaderContinuation_l,mAllocation,0x1528);
WWHD_OFFSET(StreamLoaderContinuation_l,mMode,0x152C);
WWHD_OFFSET(StreamLoaderContinuation_l,mAdpcm,0x1530);
WWHD_OFFSET(StreamLoaderContinuation_l,mCachedIndex,0x17F0);
WWHD_OFFSET(StreamLoaderContinuation_l,mHistories,0x17F4);
static_assert(sizeof(StreamLoaderContinuation_l)==sizeof(SequenceManager));
static_assert(offsetof(StreamLoaderContinuation_l,mJobs)==offsetof(SequenceManager,mJobs));
static_assert(offsetof(StreamLoaderContinuation_l,mAllocation)==offsetof(SequenceManager,mAllocation));
}}

void destroySequenceReadTask(nw::snd::SequenceAsyncTask_l* task,u32 flags);

namespace nw { namespace snd {
WWHD_SIZE(StreamLoaderJob_l,0x3C);
WWHD_OFFSET(StreamLoaderJob_l,event,8);
WWHD_OFFSET(StreamLoaderJob_l,owner,0x30);
WWHD_OFFSET(StreamLoaderJob_l,vtable,0x34);
WWHD_OFFSET(StreamLoaderJob_l,argument,0x38);
WWHD_OFFSET(StreamLoaderContinuation_l,mInput,0);
WWHD_OFFSET(StreamLoaderContinuation_l,mPlayer,0x10);
WWHD_OFFSET(StreamLoaderContinuation_l,mRetainedStream,0x14);
WWHD_OFFSET(StreamLoaderContinuation_l,mConfiguration,0x18);
WWHD_OFFSET(StreamLoaderContinuation_l,mSourceKind,0x1C);
WWHD_OFFSET(StreamLoaderContinuation_l,mProvider,0x20);
WWHD_OFFSET(StreamLoaderContinuation_l,mProviderArguments,0x24);
WWHD_OFFSET(StreamLoaderContinuation_l,mIdentifierSuffix,0x34);
WWHD_OFFSET(StreamLoaderContinuation_l,mFlag,0x37);
WWHD_OFFSET(StreamLoaderContinuation_l,mMemoryStream,0x2D0);
WWHD_OFFSET(StreamLoaderContinuation_l,mBuffer,0x2D8);
WWHD_OFFSET(StreamLoaderContinuation_l,mBufferSize,0x2DC);
WWHD_OFFSET(StreamLoaderContinuation_l,mResourceArgument,0x2E0);
WWHD_OFFSET(StreamLoaderContinuation_l,mResourceMode,0x2E4);
WWHD_OFFSET(StreamLoaderContinuation_l,mResourceContext,0x2E8);
}}

namespace nw { namespace snd {
WWHD_SIZE(StreamLoaderTask_l,0x8C);
WWHD_OFFSET(StreamLoaderTask_l,state,0x2C);
WWHD_OFFSET(StreamLoaderTask_l,owner,0x30);
WWHD_OFFSET(StreamLoaderTask_l,vtable,0x34);
WWHD_OFFSET(StreamLoaderTask_l,buffers,0x38);
WWHD_OFFSET(StreamLoaderTask_l,first,0x78);
WWHD_OFFSET(StreamLoaderTask_l,second,0x7C);
WWHD_OFFSET(StreamLoaderTask_l,loader,0x80);
WWHD_OFFSET(StreamLoaderTask_l,link,0x84);
}}

void closeStreamLoaderJob(nw::snd::StreamLoaderJob_l* job);
void runStreamLoaderReadTask(nw::snd::StreamLoaderTask_l* task);

namespace nw { namespace snd {
WWHD_SIZE(StreamTrackConfiguration_l,14);
WWHD_OFFSET(StreamLoaderConfiguration_l,blockSamples,0x1C);
WWHD_OFFSET(StreamLoaderConfiguration_l,channelCount,0x2C);
WWHD_OFFSET(StreamLoaderConfiguration_l,tracks,0x34);
WWHD_SIZE(StreamRegion_l,256);
}}

void runStreamLoaderPrepareJob(nw::snd::StreamLoaderJob_l* job);
