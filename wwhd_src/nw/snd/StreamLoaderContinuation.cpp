/* Continuation state transitions recovered from NW-SND#13 instructions. */
#include "nw/snd/StreamLoaderContinuation.h"
#include "nw/snd/AudioCurves.h"
using namespace nw::snd;
static void flushStreamReadBuffer(void*,u32);
void waitForSequenceContinuationTasks(StreamLoaderContinuation_l*);
static void* sequenceTaskScheduler();
static void cancelSequenceTasks(void*,StreamLoaderContinuation_l*);
static void scheduleSequenceTask(void*,StreamLoaderJob_l*,u32);
static void destroySequenceTaskBase(SequenceAsyncTask_l*,u32);
static void deleteSequenceTask(void*);
static void removeStreamLoaderTask(SequenceList*,SequenceListNode*);
static void releaseStreamLoaderTask(be<u32>*,StreamLoaderTask_l*);
static StreamLoaderTask_l* allocateStreamLoaderTask(be<u32>*);
static void clearStreamLoaderTask(StreamLoaderTask_l*,u32);
static StreamLoaderTask_l* initializeStreamLoaderTask(StreamLoaderTask_l*);
static void insertStreamLoaderTask(SequenceList*,gptr<SequenceListNode>*,SequenceListNode*);
static void scheduleStreamLoaderReadTask(void*,StreamLoaderTask_l*,u32);
static StreamLoaderTaskVtable_l sStreamLoaderTaskVtable;
struct StreamMetadataStorage_l { be<u8> bytes[0x5200]; };
static StreamMetadataStorage_l sStreamMetadataStorage;
static void clearStreamTrackChannels(void*,u32,u32);
static void copyStreamReadBytes(void*,const void*,u32);
static void updateStreamReadHistory(AdpcmLoop*,const StreamAdpcmRecord_l*,u32,const void*);
enum StreamLoaderCommandExtent { StreamMetadataCommandBytes=0x5C, StreamReadCommandBytes=0x94 };
static_assert(sizeof(StreamBlockCommand)<=StreamMetadataCommandBytes);
static_assert(sizeof(StreamPrepareCommand)<=StreamReadCommandBytes);
struct StreamResourceProviderVtable_l { u8 prefix[0x2C]; gabi::gfn open; };
struct StreamResourceProvider_l { be<u8> active; u8 padding[3]; gptr<const StreamResourceProviderVtable_l> vtable; };
struct ResourceMemoryStream_l { ResourceStream_l base; u8 reserved10[0x74]; be<u32> context; };
static ResourceMemoryStream_l* constructResourceMemoryStream(ResourceMemoryStream_l*);
static u32 configureResourceMemoryStream(ResourceMemoryStream_l*,void*,void*,const char*,u32);
static void setResourceMemoryStreamBuffer(ResourceMemoryStream_l*,void*,u32);
static void reportStreamPrepareFailure(const char*,u32,const char*);
struct StreamDecoderAllocatorVtable_l {
    u8 prefix[0x14]; gabi::gfn allocate; be<u32> reserved18; gabi::gfn release;
    be<u32> reserved20; gabi::gfn describe; be<u32> reserved28; gabi::gfn advance;
    be<u32> reserved30; gabi::gfn read;
};
struct StreamDecoderAllocator_l { gptr<const StreamDecoderAllocatorVtable_l> vtable; };
struct StreamDecoderSingleton_l { gptr<StreamDecoderAllocator_l> allocator; };
static StreamDecoderSingleton_l sStreamDecoderAllocator;
/* 028AE510 */
void StreamLoaderContinuation_l::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    waitForSequenceContinuationTasks(this);
    mCallback=nullptr;
    mRestartPosition=0;
    mMode=2;
    mFinalBlock=-1;
    mLoopBlock=0;
    mInitialBlock=0;
    mSkippedSamples=0;
    mCallbackArgument=nullptr;
    mFinished=0;
    mCurrentBlock=0;
    mCachedIndex=-1;
}
/* 028AE768 */
u32 StreamLoaderContinuation_l::hasPendingTasks() const {
    if (mFinished) return 0;
    return static_cast<u32>(mTasks.count)!=0;
}
/* 028AE574 */
void StreamLoaderContinuation_l::cancelTasks() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* scheduler=sequenceTaskScheduler();
    cancelSequenceTasks(scheduler,this);
}
/* 028AE5A8 */
void StreamLoaderContinuation_l::scheduleFinalTask() {
    gabi::AutoFrame frame{gabi::current_function()};
    mJobs[1].event.wait();
    mJobs[1].argument=this;
    void* scheduler=sequenceTaskScheduler();
    scheduleSequenceTask(scheduler,&mJobs[1],1);
}
/* 028B02D8 */
void destroySequenceReadTask(SequenceAsyncTask_l* task,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (task) {
        destroySequenceTaskBase(task,0);
        if (flags&1) deleteSequenceTask(task);
    }
}
/* 028AE790 */
void StreamLoaderContinuation_l::scheduleRead() {
    gabi::AutoFrame frame{gabi::current_function()};
    mJobs[0].argument=this;
    mReadCursor=0;
    mJobs[0].owner=this;
    void* scheduler=sequenceTaskScheduler();
    scheduleSequenceTask(scheduler,&mJobs[0],1);
}
/* 028AE5F8 */
void StreamLoaderContinuation_l::finish() {
    gabi::AutoFrame frame{gabi::current_function()};
    cancelTasks();
    scheduleFinalTask();
    StreamDecoderAllocator_l* allocator=sStreamDecoderAllocator.allocator;
    if (allocator && mAllocation.get()) {
        gabi::call<void>(allocator->vtable->release.get(),allocator,mAllocation.get());
        mAllocation=nullptr;
    }
}
/* 028AE700 */
void StreamLoaderContinuation_l::sendStopCommand() {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=DriverCommandManager::instance();
    TargetDriverCommand* command=static_cast<TargetDriverCommand*>(manager->reserve(sizeof(TargetDriverCommand)));
    command->id=0x35;
    command->target=mPlayer.get();
    manager->pushCommand(command);
    manager->submit(1);
}
/* 028AE660 */
void StreamLoaderContinuation_l::releaseCompletedTasks() {
    gabi::AutoFrame frame{gabi::current_function()};
    SequenceListNode* node=mTasks.sentinel.next;
    SequenceListNode* sentinel=&mTasks.sentinel;
    while (node!=sentinel) {
        StreamLoaderTask_l* task=gabi::at<StreamLoaderTask_l>(gabi::ea(node)-offsetof(StreamLoaderTask_l,link));
        u32 state=task->state;
        node=node->next;
        if (state!=3) break;
        removeStreamLoaderTask(&mTasks,&task->link);
        gabi::call<void>(task->vtable->execute.get(),task,2);
        releaseStreamLoaderTask(mTaskPool,task);
    }
}
/* 028AE7D8 */
void StreamLoaderContinuation_l::queueReadTask(const gptr<const void>* buffers,u32 first,u32 second,u32 priority) {
    gabi::AutoFrame frame{gabi::current_function()};
    StreamLoaderTask_l* task=allocateStreamLoaderTask(mTaskPool);
    if (task) {
        clearStreamLoaderTask(task,sizeof(StreamLoaderTask_l));
        initializeStreamLoaderTask(task);
        task->loader=this;
        task->link.next=nullptr;
        task->vtable=&sStreamLoaderTaskVtable;
        task->first=first;
        task->second=second;
        task->link.previous=nullptr;
        task->owner=this;
    } else {
        task->loader=this;
        task->second=second;
        task->first=first;
        task->owner=this;
    }
    for (u32 i=0;i<static_cast<u32>(mChannelCount);++i) task->buffers[i]=buffers[i];
    gabi::FrameLocal<gptr<SequenceListNode>> cursor(8);
    *cursor=&mTasks.sentinel;
    insertStreamLoaderTask(&mTasks,cursor.get(),&task->link);
    void* scheduler=sequenceTaskScheduler();
    scheduleStreamLoaderReadTask(scheduler,task,priority);
}
/* 028AEA24 */
void StreamLoaderContinuation_l::closeStream() {
    gabi::AutoFrame frame{gabi::current_function()};
    ResourceStream_l* stream=mRetainedStream;
    if (stream) {
        gabi::call<void>(stream->mVtable->mClose.get(),stream);
        mInput.mStream=nullptr;
        mRetainedStream=nullptr;
    }
}
/* 028B032C */
void closeStreamLoaderJob(StreamLoaderJob_l* job) {
    gabi::AutoFrame frame{gabi::current_function()};
    StreamLoaderContinuation_l* loader=job->argument.get();
    loader->closeStream();
}
/* 028AF3B0 */
void StreamLoaderContinuation_l::sendPrepareCommand() {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=DriverCommandManager::instance();
    StreamBlockCommand* command=static_cast<StreamBlockCommand*>(manager->reserve(StreamMetadataCommandBytes));
    command->id=0x33;
    u32 result=0;
    if (static_cast<u32>(mSourceKind)==0) result=readNativeMetadata(command);
    else if (static_cast<u32>(mSourceKind)==1) result=readDecodedMetadata(command);
    command->value=result;
    manager->pushCommand(command);
    manager->submit(1);
}
/* 028B0140 */
void StreamLoaderContinuation_l::sendReadCommand(const gptr<const void>* buffers,u32 first,u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mFinished) return;
    DriverCommandManager* manager=DriverCommandManager::instance();
    StreamPrepareCommand* command=static_cast<StreamPrepareCommand*>(manager->reserve(StreamReadCommandBytes));
    command->id=0x34;
    u32 result=0;
    if (static_cast<u32>(mSourceKind)==0) result=readPcmBlock(command,buffers,first,second);
    else if (static_cast<u32>(mSourceKind)==1) result=readDecodedBlock(command,buffers,first,second);
    command->channel=result;
    command->target=mPlayer.get();
    manager->pushCommand(command);
    manager->submit(1);
}
/* 028B02C0 */
void runStreamLoaderReadTask(StreamLoaderTask_l* task) {
    gabi::AutoFrame frame{gabi::current_function()};
    StreamLoaderContinuation_l* loader=task->loader;
    loader->sendReadCommand(task->buffers,task->first,task->second);
}
/* 028AF474 */
u32 StreamLoaderContinuation_l::advanceRegion() {
    gabi::AutoFrame frame{gabi::current_function()};
    const StreamLoaderConfiguration_l* configuration=mConfiguration;
    if (configuration->looping) {
        u32 start=configuration->loopStart;
        configuration=mConfiguration;
        mRegionStart=start;
        mSampleCursor=start;
        mRegionEnd=configuration->sampleEnd;
        return 1;
    }
    if (mCallback.get()) {
        gabi::FrameLocal<be<u32>> cursor(8);
        *cursor=mReadCursor;
        if (gabi::call<u32>(gabi::ea(mCallback.get()),cursor.get(),mCallbackArgument.get())) {
            u32 next=*cursor;
            mReadCursor=next;
            gabi::FrameLocal<StreamRegion_l> region(0x14);
            if (mInput.readRegion(gabi::at<be<u32>>(gabi::ea(region.get())),next)) {
                u32 start=region->start;
                u32 mode=mMode;
                mRegionStart=start;
                mRegionEnd=region->end;
                if (mode==2) {
                    for (u32 i=0;i<static_cast<u32>(mChannelCount);++i) {
                        mHistories[i].predictor=region->histories[i].predictor;
                        mHistories[i].history[0]=region->histories[i].history[0];
                        mHistories[i].history[1]=region->histories[i].history[1];
                    }
                    mCachedIndex=start;
                    mSampleCursor=mRegionStart;
                    return 1;
                }
            } else {
                configuration=mConfiguration;
                mRegionStart=0;
                mRegionEnd=configuration->sampleEnd;
            }
            mSampleCursor=mRegionStart;
            return 1;
        }
    }
    mSampleCursor=mRegionEnd;
    return 0;
}
/* 028AE8E4 */
u32 StreamLoaderContinuation_l::openStream() {
    gabi::AutoFrame frame{gabi::current_function()};
    StreamResourceProvider_l* provider=static_cast<StreamResourceProvider_l*>(mProvider.get());
    ResourceStream_l* stream=nullptr;
    if (provider) {
        if (provider->active) {
            stream=gabi::call<ResourceStream_l*>(provider->vtable->open.get(),provider,static_cast<u32>(mProviderArguments[0]),static_cast<u32>(mProviderArguments[1]),static_cast<u32>(mProviderArguments[2]),"bxstm");
        }
        mRetainedStream=stream;
    } else stream=mRetainedStream;
    if (!stream) {
        ResourceMemoryStream_l* memory=static_cast<ResourceMemoryStream_l*>(mMemoryStream.get());
        if (memory) memory=constructResourceMemoryStream(memory);
        memory->context=mResourceContext;
        if (!configureResourceMemoryStream(memory,mResourceArgument.get(),mPath,"r",mResourceMode)) return 0;
        if (gabi::call<u32>(memory->base.mVtable->mCanRead.get(),memory)) {
            void* buffer=mBuffer;
            if (buffer) {
                u32 size=mBufferSize;
                if (size) setResourceMemoryStreamBuffer(memory,buffer,size);
            }
        }
        stream=&memory->base;
        mRetainedStream=stream;
    }
    mInput.mStream=stream;
    return 1;
}
/* 028B0230 */
void runStreamLoaderPrepareJob(StreamLoaderJob_l* job) {
    gabi::AutoFrame frame{gabi::current_function()};
    StreamLoaderContinuation_l* loader=job->argument.get();
    if (!loader->openStream()) {
        loader=job->argument.get();
        if (loader->mFlag) {
            reportStreamPrepareFailure("snd_StreamSoundLoader.h",0x101,"StreamSoundLoader::Open is Failed\n");
            loader=job->argument.get();
            loader->sendPrepareCommand();
        } else loader->sendStopCommand();
    } else {
        loader=job->argument.get();
        loader->sendPrepareCommand();
    }
}
/* 028AEFF8 */
u32 StreamLoaderContinuation_l::readDecodedMetadata(StreamBlockCommand* command) {
    gabi::AutoFrame frame{gabi::current_function()};
    command->target=mPlayer.get();
    command->count=mIdentifierSuffix;
    StreamDecoderAllocator_l* allocator=sStreamDecoderAllocator.allocator;
    if (!allocator) return 0;
    void* allocation=gabi::call<void*>(allocator->vtable->allocate.get(),allocator);
    mAllocation=allocation;
    allocator=sStreamDecoderAllocator.allocator;
    struct DecoderDescription_l { be<u32> channels,rate,blockSamples,blockBytes; };
    gabi::FrameLocal<DecoderDescription_l> description(8);
    if (!gabi::call<u32>(allocator->vtable->describe.get(),allocator,mRetainedStream.get(),description.get(),allocation)) return 0;
    u32 channels=description->channels;
    mChannelCount=channels;
    mConfiguration->channelCount=channels;
    mConfiguration->trackCount=1;
    mConfiguration->encoding=1;
    mConfiguration->sampleRate=description->rate;
    mConfiguration->looping=mLooping;
    mConfiguration->loopStart=mLoopStart;
    mConfiguration->sampleEnd=mLoopEnd;
    mConfiguration->pitch=mPitch;
    mConfiguration->outputMode=mOutputMode;
    for (u32 i=0;i<3;++i) mConfiguration->routes[i]=mRoutes[i];
    mConfiguration->blockSamples=description->blockSamples;
    mConfiguration->blockBytes=description->blockBytes;
    StreamLoaderConfiguration_l* configuration=mConfiguration;
    configuration->paddedBlockBytes=configuration->blockBytes;
    for (u32 i=0;i<channels;++i) gabi::store<u32>(gabi::ea(command->block)+i*4,0);
    configuration=mConfiguration;
    u32 end=configuration->sampleEnd;
    u32 blockSamples=configuration->blockSamples;
    u32 finalBlock=(end-1)/blockSamples;
    u32 last=blockSamples;
    if (configuration->looping) last=end-finalBlock*blockSamples;
    configuration->lastBlockSamples=last;
    configuration=mConfiguration;
    u32 start=configuration->loopStart;
    blockSamples=configuration->blockSamples;
    u32 loopBlock=start/blockSamples;
    u32 skipped=start-loopBlock*blockSamples;
    u32 remainder=blockSamples-skipped;
    u32 extra=remainder;
    if (blockSamples-last>remainder) extra+=blockSamples;
    configuration->loopStart=start+extra;
    mConfiguration->lastBlockSamples=static_cast<u32>(mConfiguration->lastBlockSamples)+extra;
    configuration=mConfiguration;
    start=configuration->loopStart;
    blockSamples=configuration->blockSamples;
    loopBlock=start/blockSamples;
    configuration->tracks[0].volume=127;
    mConfiguration->tracks[0].pan=64;
    mConfiguration->tracks[0].channelCount=channels;
    mConfiguration->tracks[0].output=127;
    for (u32 i=0;i<3;++i) mConfiguration->tracks[0].routes[i]=0;
    mConfiguration->tracks[0].send=64;
    mConfiguration->tracks[0].flags9=0;
    mConfiguration->tracks[0].flagsA=0;
    for (u32 i=0;i<channels;++i) {
        configuration=mConfiguration;
        gabi::store<u8>(gabi::ea(configuration)+offsetof(StreamLoaderConfiguration_l,tracks)+offsetof(StreamTrackConfiguration_l,channels)+i,i);
    }
    mLoopBlock=loopBlock;
    mSkippedSamples=start-loopBlock*blockSamples;
    mFinalBlock=finalBlock;
    if (mCallback.get()) {
        gabi::FrameLocal<be<u32>> cursor(0x18);
        *cursor=0;
        if (!gabi::call<u32>(gabi::ea(mCallback.get()),cursor.get(),mCallbackArgument.get())) return 0;
        mReadCursor=*cursor;
        configuration=mConfiguration;
        mSampleCursor=0;
        mRegionEnd=configuration->sampleEnd;
        mRegionStart=0;
    } else {
        mRegionStart=0;
        mRegionEnd=mLoopEnd;
        mSampleCursor=0;
    }
    return 1;
}
/* 028AEA74 */
u32 StreamLoaderContinuation_l::readNativeMetadata(StreamBlockCommand* command) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<StreamMetadataReader> reader(0x18);
    StreamMetadataReader::create(reader.get());
    if (!mInput.readMetadata(reader.get(),&sStreamMetadataStorage,sizeof(StreamMetadataStorage_l))) return 0;
    command->target=mPlayer.get();
    command->count=mIdentifierSuffix;
    gabi::FrameLocal<StreamInfoData> info(0x48);
    if (!reader->readInfo(info.get())) return 0;
    const StreamInfoReferences* references=reader->mInfo;
    const GroupReferenceTable* channels=static_cast<const GroupReferenceTable*>(references->channelTable());
    u32 channelCount=channels->mCount;
    mChannelCount=channelCount;
    mConfiguration->channelCount=channelCount;
    StreamLoaderConfiguration_l* configuration=mConfiguration;
    configuration->encoding=decodeSampleEncoding(gabi::load<u8>(info.a));
    mConfiguration->sampleRate=info->mWords[1];
    mConfiguration->looping=gabi::load<u8>(info.a+1);
    mConfiguration->loopStart=info->mWords[2];
    mConfiguration->sampleEnd=info->mWords[3];
    mConfiguration->blockSamples=info->mWords[6];
    mConfiguration->blockBytes=info->mWords[5];
    mConfiguration->paddedBlockBytes=info->mWords[9];
    mConfiguration->lastBlockSamples=info->mWords[8];
    if (reader->legacyLayout()) {
        references=reader->mInfo;
        const GroupReferenceTable* tracks=static_cast<const GroupReferenceTable*>(references->trackTable());
        u32 trackCount=tracks->mCount;
        if (trackCount>8) trackCount=8;
        gabi::FrameLocal<StreamTrackInfo> track(8);
        for (u32 i=0;i<trackCount;++i) {
            track->mParameters[0]=0;
            track->mParameters[1]=0;
            track->mChannelCount=0;
            clearStreamTrackChannels(track->mChannels,0,2);
            if (!reader->readTrack(track.get(),i)) return 0;
            mConfiguration->tracks[i].volume=track->mParameters[0];
            mConfiguration->tracks[i].pan=track->mParameters[1];
            mConfiguration->tracks[i].channelCount=track->mChannelCount;
            for (s32 c=0;c<static_cast<s32>(track->mChannelCount);++c) mConfiguration->tracks[i].channels[c]=track->mChannels[c];
            mConfiguration->tracks[i].reserved2=0;
            mConfiguration->tracks[i].reserved3=0;
            mConfiguration->tracks[i].output=127;
            for (u32 c=0;c<3;++c) mConfiguration->tracks[i].routes[c]=0;
            mConfiguration->tracks[i].send=64;
            mConfiguration->tracks[i].flags9=0;
            mConfiguration->tracks[i].flagsA=0;
        }
    }
    u32 mode=mConfiguration->encoding;
    mMode=mode;
    if (mode==2) {
        gabi::FrameLocal<InitialAdpcmState> initial(0x20);
        gabi::FrameLocal<AdpcmLoop> loop(0x10);
        for (u32 i=0;i<channelCount;++i) {
            if (!reader->readChannel(initial.get(),loop.get(),i)) return 0;
            StreamAdpcmRecord_l* record=&mAdpcm[i];
            for (u32 c=0;c<16;++c) record->initial.mCoefficients[c]=initial->mCoefficients[c];
            record->initial.mContext.predictor=initial->mContext.predictor;
            record->initial.mContext.history[0]=initial->mContext.history[0];
            record->initial.mContext.history[1]=initial->mContext.history[1];
            record->loop.predictor=loop->predictor;
            record->loop.history[0]=loop->history[0];
            record->loop.history[1]=loop->history[1];
            gabi::store<u32>(gabi::ea(command->block)+i*4,gabi::ea(record));
        }
    } else {
        for (u32 i=0;i<channelCount;++i) gabi::store<u32>(gabi::ea(command->block)+i*4,0);
    }
    u32 dataStart=0;
    const StreamFileDirectory* header=reader->mHeader;
    if (header) {
        u32 data=header->dataOffset()+8;
        references=reader->mInfo;
        const void* streamInfo=references->streamInfo();
        dataStart=data+gabi::load<u32>(gabi::ea(streamInfo)+0x34);
    }
    u32 blockSamples=info->mWords[6];
    u32 loopStart=info->mWords[2];
    u32 blockBytes=info->mWords[5];
    u32 loopBlock=loopStart/blockSamples;
    mLoopBlock=loopBlock;
    u32 end=info->mWords[3];
    mDataStart=dataStart;
    mRestartPosition=dataStart+blockBytes*channelCount*loopBlock;
    mSkippedSamples=0;
    mFinalBlock=(end-1)/blockSamples;
    if (mCallback.get()) {
        gabi::FrameLocal<be<u32>> cursor(0x20);
        *cursor=0;
        if (!gabi::call<u32>(gabi::ea(mCallback.get()),cursor.get(),mCallbackArgument.get())) return 0;
        u32 next=*cursor;
        mReadCursor=next;
        gabi::FrameLocal<StreamRegion_l> region(0x8C);
        if (mInput.readRegion(gabi::at<be<u32>>(gabi::ea(region.get())),next)) {
            u32 start=region->start;
            u32 currentMode=mMode;
            mRegionStart=start;
            mRegionEnd=region->end;
            if (currentMode==2) {
                for (u32 i=0;i<channelCount;++i) {
                    mHistories[i].predictor=region->histories[i].predictor;
                    mHistories[i].history[0]=region->histories[i].history[0];
                    mHistories[i].history[1]=region->histories[i].history[1];
                }
                mSampleCursor=mRegionStart;
                mCachedIndex=start;
            }
        } else {
            mRegionStart=0;
            mRegionEnd=info->mWords[3];
        }
    } else {
        mRegionEnd=end;
        mRegionStart=0;
    }
    u32 sample=mRegionStart;
    configuration=mConfiguration;
    mSampleCursor=sample;
    u32 block=sample/static_cast<u32>(configuration->blockSamples);
    ResourceStream_l* stream=mRetainedStream;
    u32 channelsNow=mChannelCount;
    mCurrentBlock=block;
    u32 bytes=configuration->blockBytes;
    gabi::call<void>(stream->mVtable->mSeek.get(),stream,static_cast<u32>(mDataStart)+bytes*channelsNow*block,0);
    return 1;
}
/* 028AF5F0 */
u32 StreamLoaderContinuation_l::readPcmBlock(StreamPrepareCommand* command,const gptr<const void>* buffers,u32 first,u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 crossings=0,skipped=0;
    bool resetHistory=false;
    if (second) {
        u32 sample=static_cast<u32>(mSampleCursor)+second;
        while (sample>=static_cast<u32>(mRegionEnd)) {
            u32 overshoot=sample-static_cast<u32>(mRegionEnd);
            if (!advanceRegion()) { mFinished=1; command->position=0; return 1; }
            sample=static_cast<u32>(mSampleCursor)+overshoot;
            ++crossings;
        }
        StreamLoaderConfiguration_l* configuration=mConfiguration;
        mSampleCursor=sample;
        u32 block=sample/static_cast<u32>(configuration->blockSamples);
        u32 channels=mChannelCount;
        ResourceStream_l* stream=mRetainedStream;
        mCurrentBlock=block;
        u32 bytes=configuration->blockBytes;
        gabi::call<void>(stream->mVtable->mSeek.get(),stream,static_cast<u32>(mDataStart)+bytes*channels*block,0);
        resetHistory=static_cast<u32>(mMode)==2;
    }
    u32 mode=mMode,initialSample=mSampleCursor;
    bool firstChunk=true;
    u32 totalSamples=0,totalBytes=0;
    gabi::FrameLocal<be<s16>[16]> historiesA(0x2C),historiesB(0x4C);
    for (;;) {
        u32 block=mCurrentBlock;
        StreamLoaderConfiguration_l* configuration=mConfiguration;
        u32 blockSamples=configuration->blockSamples;
        bool last=block==static_cast<u32>(mFinalBlock);
        u32 blockBytes=last?static_cast<u32>(configuration->paddedBlockBytes):static_cast<u32>(configuration->blockBytes);
        u32 samples=last?static_cast<u32>(configuration->lastBlockSamples):blockSamples;
        u32 sample=mSampleCursor,within=sample-(sample/blockSamples)*blockSamples;
        u32 rounded=mode==2?(within/14)*14:within;
        if (firstChunk) {
            skipped=within-rounded;
            if (resetHistory) {
                ResourceStream_l* stream=mRetainedStream;
                u32 position=gabi::call<u32>(stream->mVtable->mTell.get(),stream);
                if (!mInput.readSeekHistories(*historiesA,*historiesB,mCurrentBlock,mChannelCount)) return 0;
                stream=mRetainedStream;
                gabi::call<void>(stream->mVtable->mSeek.get(),stream,position,0);
                for (u32 i=0;i<static_cast<u32>(mChannelCount);++i) {
                    mHistories[i].history[0]=(*historiesA)[i];
                    mHistories[i].history[1]=(*historiesB)[i];
                }
                mode=mMode;
            }
        }
        u32 byteOffset=sampleStorageBytes(rounded,mode);
        samples-=within;
        sample=mSampleCursor;
        u32 bytes=blockBytes-byteOffset;
        if (sample+samples>static_cast<u32>(mRegionEnd)) {
            samples=static_cast<u32>(mRegionEnd)-sample;
            if (samples<0x480) bytes=0x900;
        }
        u32 channel=0;
        while (channel<static_cast<u32>(mChannelCount)) {
            const StreamLoaderPlayerPrefix_l* player=mPlayer;
            if (player->readCancelled) return 0;
            u32 channels=mChannelCount,group=channel+2<=channels?2:channels-channel;
            u32 readBytes=blockBytes*group;
            ResourceStream_l* stream=mRetainedStream;
            if (gabi::call<u32>(stream->mVtable->mRead.get(),stream,&sStreamMetadataStorage,readBytes)!=readBytes) return 0;
            u32 firstChannel=channel;
            channel+=group;
            for (u32 i=0;i<group;++i) {
                u32 current=firstChannel+i;
                const void* input=gabi::at<be<u8>>(gabi::ea(&sStreamMetadataStorage)+i*blockBytes);
                void* output=gabi::at<be<u8>>(static_cast<u32>(buffers[current].v)+totalBytes);
                copyStreamReadBytes(output,gabi::at<be<u8>>(gabi::ea(input)+byteOffset),bytes);
                flushStreamReadBuffer(output,bytes);
                if (firstChunk && resetHistory) {
                    mHistories[current].predictor=gabi::load<u8>(gabi::ea(input));
                    updateStreamReadHistory(&mHistories[current],&mAdpcm[current],(within/14)*14,input);
                    mCachedIndex=mSampleCursor;
                }
            }
        }
        sample=static_cast<u32>(mSampleCursor)+samples;
        u32 end=mRegionEnd;
        block=mCurrentBlock;
        totalSamples+=samples; totalBytes+=bytes;
        mSampleCursor=sample; mCurrentBlock=block+1;
        if (sample==end) {
            if (advanceRegion()) {
                configuration=mConfiguration;
                sample=mSampleCursor;
                u32 next=sample/static_cast<u32>(configuration->blockSamples);
                ResourceStream_l* stream=mRetainedStream;
                u32 channels=mChannelCount;
                mCurrentBlock=next;
                u32 bytesPerBlock=configuration->blockBytes;
                ++crossings;
                gabi::call<void>(stream->mVtable->mSeek.get(),stream,static_cast<u32>(mDataStart)+bytesPerBlock*channels*next,0);
            } else mFinished=1;
            command->mode=0; break;
        }
        firstChunk=false;
        if (totalSamples==0 || totalSamples<0x480 || end-sample<0x480) { mode=mMode; continue; }
        command->mode=0; break;
    }
    if (static_cast<u32>(mMode)==2) {
        enum ContextSource { None,Initial,Loop,Cached } source=None;
        if (initialSample==0) source=Initial;
        else {
            const StreamLoaderConfiguration_l* configuration=mConfiguration;
            if (configuration->looping && initialSample==static_cast<u32>(configuration->loopStart)) source=Loop;
            else if (initialSample==static_cast<u32>(mCachedIndex)) source=Cached;
        }
        if (source!=None) {
            for (u32 i=0;i<static_cast<u32>(mChannelCount);++i) {
                const AdpcmLoop* context=source==Initial?&mAdpcm[i].initial.mContext:source==Loop?&mAdpcm[i].loop:&mHistories[i];
                AdpcmLoop* destination=gabi::at<AdpcmLoop>(gabi::ea(command->descriptor)+i*sizeof(AdpcmLoop));
                destination->predictor=context->predictor;
                destination->history[0]=context->history[0];
                destination->history[1]=context->history[1];
            }
            command->mode=1;
        }
    }
    command->position=totalSamples; command->source=first;
    command->bufferSize=crossings; command->buffer=skipped;
    command->byteCount=initialSample; command->flags=mFinished;
    return 1;
}
/* WWHD glue
str 10187FD8 "bxstm" in 028AE8E4
str 10188084 "r" in 028AE8E4
str 10188048 "snd_StreamSoundLoader.h" in 028B0230
str 10188060 "StreamSoundLoader::Open is Failed\n" in 028B0230
ext 028A2394 ResourceMemoryStream_l* constructResourceMemoryStream(ResourceMemoryStream_l*)
ext 028A28F8 u32 configureResourceMemoryStream(ResourceMemoryStream_l*,void*,void*,const char*,u32)
ext 028A2400 void setResourceMemoryStreamBuffer(ResourceMemoryStream_l*,void*,u32)
ext 02034974 void reportStreamPrepareFailure(const char*,u32,const char*)
ext 0289CE3C u32 nw::snd::sampleStorageBytes(u32,u32)
ext C000A848 void copyStreamReadBytes(void*,const void*,u32)
ext 028898F0 void flushStreamReadBuffer(void*,u32)
ext 0289EFFC void updateStreamReadHistory(nw::snd::AdpcmLoop*,const nw::snd::StreamAdpcmRecord_l*,u32,const void*)
data sStreamMetadataStorage 104D4900
ext C000A858 void clearStreamTrackChannels(void*,u32,u32)
data sStreamDecoderAllocator 101FCB70
data sStreamLoaderTaskVtable 10188018
ext 0286DA9C void removeStreamLoaderTask(nw::snd::SequenceList*,nw::snd::SequenceListNode*)
ext 0288A26C void releaseStreamLoaderTask(be<u32>*,nw::snd::StreamLoaderTask_l*)
ext 0288A248 nw::snd::StreamLoaderTask_l* allocateStreamLoaderTask(be<u32>*)
ext 028F521C void clearStreamLoaderTask(nw::snd::StreamLoaderTask_l*,u32)
ext 0289C050 nw::snd::StreamLoaderTask_l* initializeStreamLoaderTask(nw::snd::StreamLoaderTask_l*)
ext 0286DA70 void insertStreamLoaderTask(nw::snd::SequenceList*,gptr<nw::snd::SequenceListNode>*,nw::snd::SequenceListNode*)
ext 0289C464 void scheduleStreamLoaderReadTask(void*,nw::snd::StreamLoaderTask_l*,u32)
ext 02885F68 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::reserve(u32)
ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
ext 02887314 u32 nw::snd::DriverCommandManager::submit(u32)
ext 028AE3D0 void waitForSequenceContinuationTasks(StreamLoaderContinuation_l*)
ext 0289C1A4 void* sequenceTaskScheduler()
ext 0289C5B4 void cancelSequenceTasks(void*,StreamLoaderContinuation_l*)
ext 028F6A2C void nw::ut::Event::wait()
ext 0289C464 void scheduleSequenceTask(void*,nw::snd::StreamLoaderJob_l*,u32)
ext 0289C0C8 void destroySequenceTaskBase(nw::snd::SequenceAsyncTask_l*,u32)
ext 0273AF40 void deleteSequenceTask(void*)
*/
