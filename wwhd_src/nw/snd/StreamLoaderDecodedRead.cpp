/* Decoded stream block progression recovered from NW-SND#13 instructions. */
#include "nw/snd/StreamLoaderContinuation.h"
using namespace nw::snd;
static void flushStreamReadBuffer(void*,u32);

struct DecodedReadVtable_l {
    u8 prefix[0x2C];
    gabi::gfn advance;
    be<u32> reserved30;
    gabi::gfn read;
};
struct DecodedReadObject_l { gptr<const DecodedReadVtable_l> vtable; };
WWHD_OFFSET(DecodedReadVtable_l,advance,0x2C);
WWHD_OFFSET(DecodedReadVtable_l,read,0x34);
struct DecodedReadBinding_l { gptr<DecodedReadObject_l> object; };
static DecodedReadBinding_l sDecodedReadAllocator;

/* 028AFD14 */
u32 StreamLoaderContinuation_l::readDecodedBlock(StreamPrepareCommand* command,const gptr<const void>* buffers,u32 first,u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<const void>[16]> savedBuffers(8);
    gptr<const void>* localBuffers=*savedBuffers;
    u32 current=mCurrentBlock;
    u32 finalBlock=static_cast<u32>(mFinalBlock);
    u32 finalRead=current==finalBlock;
    StreamLoaderConfiguration_l* configuration=mConfiguration;
    u32 samples=configuration->blockSamples;
    if (finalRead) samples=configuration->lastBlockSamples;
    u32 skipped=0;
    u32 crossedRegions=0;

    if (current>finalBlock && configuration->looping) {
        mCurrentBlock=mLoopBlock;
        ResourceStream_l* stream=mRetainedStream;
        gabi::call<u32>(stream->mVtable->mSeek.get(),stream,static_cast<u32>(mRestartPosition),0u);
        if (static_cast<u32>(mLoopBlock)) {
            u32 channels=mChannelCount;
            for (u32 i=0;i<channels;++i) localBuffers[i]=buffers[i];
            DecodedReadObject_l* decoder=sDecodedReadAllocator.object;
            if (!gabi::call<u32>(decoder->vtable->read.get(),decoder,mRetainedStream.get(),channels,2u,localBuffers,mAllocation.get())) return 0;
        }
        configuration=mConfiguration;
        u32 loopStart=configuration->loopStart;
        mInitialBlock=1;
        mSampleCursor=loopStart;
    }

    if (second) {
        u32 cursor=mSampleCursor;
        u32 end=mRegionEnd;
        u32 requested=cursor+second;
        if (requested>=end) {
            u32 remaining;
            do {
                remaining=requested-end;
                if (!advanceRegion()) {
                    mFinished=1;
                    command->position=0;
                    return 1;
                }
                cursor=mSampleCursor;
                end=mRegionEnd;
                requested=cursor+remaining;
                ++crossedRegions;
            } while (requested>=end);
        }
        configuration=mConfiguration;
        mSampleCursor=requested;
        u32 blockSamples=configuration->blockSamples;
        u32 block=blockSamples ? requested/blockSamples : 0;
        mCurrentBlock=block;
        ResourceStream_l* stream=mRetainedStream;
        blockSamples=configuration->blockSamples;
        skipped=requested-block*blockSamples;
        gabi::call<u32>(stream->mVtable->mSeek.get(),stream,0u,0u);
        u32 index=0;
        while (index<static_cast<u32>(mCurrentBlock)) {
            u32 loopBlock=mLoopBlock;
            if (loopBlock && index==loopBlock-1) {
                stream=mRetainedStream;
                mRestartPosition=gabi::call<u32>(stream->mVtable->mTell.get(),stream);
            }
            DecodedReadObject_l* decoder=sDecodedReadAllocator.object;
            gabi::call<void>(decoder->vtable->advance.get(),decoder,mRetainedStream.get());
            ++index;
        }
    } else if (mInitialBlock) skipped=mSkippedSamples;

    u32 loopBlock=mLoopBlock;
    if (loopBlock && static_cast<u32>(mCurrentBlock)==loopBlock-1) {
        ResourceStream_l* stream=mRetainedStream;
        mRestartPosition=gabi::call<u32>(stream->mVtable->mTell.get(),stream);
    }
    ResourceStream_l* stream=mRetainedStream;
    const ResourceStreamVtable_l* table=stream->mVtable;
    u32 sampleCursor=mSampleCursor;
    u32 position=gabi::call<u32>(table->mTell.get(),stream);
    u32 size=gabi::call<u32>(table->mSize.get(),mRetainedStream.get());
    if (position>=size) samples=0;
    else {
        if (samples) {
            u32 channels=mChannelCount;
            for (u32 i=0;i<channels;++i) localBuffers[i]=buffers[i];
            DecodedReadObject_l* decoder=sDecodedReadAllocator.object;
            if (!gabi::call<u32>(decoder->vtable->read.get(),decoder,mRetainedStream.get(),channels,finalRead,localBuffers,mAllocation.get())) return 0;
            u32 channel=0;
            while (channel<static_cast<u32>(mChannelCount)) {
                flushStreamReadBuffer(const_cast<void*>(localBuffers[channel].get()),samples*2u);
                ++channel;
            }
            configuration=mConfiguration;
            if (!configuration->looping) {
                stream=mRetainedStream;
                table=stream->mVtable;
                position=gabi::call<u32>(table->mTell.get(),stream);
                size=gabi::call<u32>(table->mSize.get(),mRetainedStream.get());
                if (position>=size) mFinalBlock=mCurrentBlock;
            }
        }
        mSampleCursor=static_cast<u32>(mSampleCursor)+samples;
    }
    current=static_cast<u32>(mCurrentBlock)+1;
    finalBlock=static_cast<u32>(mFinalBlock);
    mInitialBlock=0;
    mCurrentBlock=current;
    if (current>finalBlock && !mConfiguration->looping) mFinished=1;
    command->byteCount=sampleCursor;
    command->position=samples;
    command->source=first;
    command->mode=0;
    command->bufferSize=crossedRegions;
    command->buffer=skipped;
    command->flags=mFinished;
    return 1;
}
/* WWHD glue
data sDecodedReadAllocator 101FCB70
ext 028898F0 void flushStreamReadBuffer(void*,u32)
*/
