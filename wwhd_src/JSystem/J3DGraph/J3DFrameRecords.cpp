// Dynamically reloaded frame-record release and resource initialization.
#include "JSystem/J3DGraph/J3DFrameRecords.h"
using namespace j3d;
void* allocateFramePair(u32);
void deallocateFrameRecords(void*);
void finalizeFrameRecord(FrameRecord*);
void initializeFrameRecord(FrameRecord*,const FrameSourceRecord*,sead::RegistryHeap*);
sead::RegistryHeap* frameRecordsContainingHeap(sead::RegistryManagerState*,const void*);
/* inlined into FrameRecords::initialize */
static void clearFrameBuffer(FrameBufferRange* pair) {
    if(pair==NULL) pair=static_cast<FrameBufferRange*>(allocateFramePair(sizeof(FrameBufferRange)));
    if(pair!=NULL) { pair->mData=NULL; pair->mCount=0; }
}
/* inlined into FrameRecords::initialize */
static void constructFrameRecord(FrameRecord* record) {
    if(record==NULL) return;
    clearFrameBuffer(&record->mRanges[0]);
    clearFrameBuffer(&record->mRanges[1]);
    clearFrameBuffer(&record->mRanges[2]);
    record->mActive=0;
    record->mState=0;
    record->mDirty=0;
}
/* 02801024 */
void FrameRecords::destroy(FrameRecords* owner,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(owner==NULL) return;
    FrameRecord* records=owner->mRecords;
    if(records!=NULL) {
        s32 count=owner->mCount;
        s32 index=0;
        u32 offset=0;
        while(index<count) {
            FrameRecord* record=gabi::at<FrameRecord>(gabi::ea(records)+offset);
            if(record!=NULL && record->mActive!=0) {
                finalizeFrameRecord(record);
                count=owner->mCount;
                records=owner->mRecords;
            }
            ++index;
            offset+=sizeof(FrameRecord);
        }
        sead::RegistryHeap* heap=frameRecordsContainingHeap(sRegistryManager,records);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<FrameRecord*>(owner->mRecords));
        owner->mCount=0;
        owner->mRecords=NULL;
    }
    if((flags&1)!=0) deallocateFrameRecords(owner);
}
/* 028010F8 */
void FrameRecords::initialize(FrameSource* source,sead::RegistryHeap* requestedHeap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSource=source;
    if(source==NULL) return;
    s32 count=source->mCount;
    if(count>0) {
        sead::RegistryHeap* heap=requestedHeap;
        if(heap==NULL) heap=registryCurrentHeap(sRegistryManager);
        FrameRecord* storage=gabi::call_ptr<FrameRecord*>(heap->dispatch()->mAllocate.mFunction,heap,u32(count)*u32(sizeof(FrameRecord)),4);
        u32 constructions=count>0?u32(count):1;
        u32 offset=0;
        for(u32 left=constructions;left!=0;--left) {
            constructFrameRecord(gabi::at<FrameRecord>(gabi::ea(storage)+offset));
            offset+=sizeof(FrameRecord);
        }
        if(storage!=NULL) { mRecords=storage; mCount=count; }
    }
    if(count>0) {
        u32 index=0;
        u32 sourceOffset=0;
        u32 destinationOffset=0;
        for(s32 left=count;left!=0;--left) {
            FrameSource* current=mSource;
            u32 sourceCount=current->mCount;
            const FrameSourceRecord* entry=NULL;
            u32 destinationCount=mCount;
            if(index<sourceCount) entry=gabi::at<FrameSourceRecord>(gabi::ea(static_cast<FrameSourceRecord*>(current->mRecords))+sourceOffset);
            FrameRecord* destination=mRecords;
            if(index<destinationCount) destination=gabi::at<FrameRecord>(gabi::ea(destination)+destinationOffset);
            initializeFrameRecord(destination,entry,requestedHeap);
            destinationOffset+=sizeof(FrameRecord);
            ++index;
            sourceOffset+=sizeof(FrameSourceRecord);
        }
    }
    mPublishedCount=count;
}
/* WWHD glue
 ext 0273AD10 void* allocateFramePair(u32)
 ext 0273AF40 void deallocateFrameRecords(void*)
 ext 028009A0 void finalizeFrameRecord(j3d::FrameRecord*)
 ext 02800B0C void initializeFrameRecord(j3d::FrameRecord*,const j3d::FrameSourceRecord*,sead::RegistryHeap*)
 ext 02755FEC sead::RegistryHeap* frameRecordsContainingHeap(sead::RegistryManagerState*,const void*)
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
*/
