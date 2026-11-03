#pragma once
#include "JSystem/J3DGraph/ModelPipeline.h"
#include "sead/PoolHeap.h"
#include "sead/Thread.h"
#include "sead/InlineInstances.h"
namespace j3d::pipeline {
struct JobPointerArray_l {
    be<s32> mCount,mCapacity; gptr<gptr<sead::JobItem>> mItems;
    static JobPointerArray_l* create(JobPointerArray_l*);
};
struct ModelPointerArray_l { be<u32> mCount,mCapacity; gptr<gptr<ModelPipeline_l>> mItems; };
struct QueueTask_l {
    sead::SafeText mName;
    be<u8> mFlag; u8 mPadding[3];
    gptr<sead::JobItemTable> mMethods;
    sead::instances::DelegateLayout mDelegate;
    void invokeProviderCallback();
    void invokeWorkCallback();
    void invokeCompletionCallback();
};
struct PipelineCoreMap_l { be<u32> mSlots[3]; };
struct FrameWorkerSet {
    JobPointerArray_l mInputs[3];
    QueueTask_l mTask;
    sead::JobQueue mQueues[3];
    sead::WorkerManager mWorkers;
    static FrameWorkerSet* create(FrameWorkerSet*);
    void executePending();
    void runQueues();
    void initialize(sead::RegistryHeap* heap);
    void clearQueues();
    void append(sead::JobItem*);
};
struct PipelineQueueMethods_l { u8 mPrefix[0x1C]; gfn mSendMessage; u8 mBeforeStart[0xC]; gfn mStart; u8 mBeforeHook[0x14]; gfn mHook; };
struct PipelineFrameQueue {
    sead::Disposer mDisposer;
    ModelPointerArray_l mModels;
    sead::CriticalSection mLocks[3];
    be<u8> mFlag; u8 mPadding[7];
    FrameWorkerSet mWork;
    sead::instances::DelegateLayout mDelegate;
    gptr<sead::DelegateThread> mDispatcher;
    sead::JobQueue mJobs;
    JobPointerArray_l mOutputs[2];
    QueueTask_l mCompletion;
    be<u8> mCompleted; u8 mTail[7];
    static PipelineFrameQueue* create(PipelineFrameQueue*);
    static PipelineFrameQueue* instance(sead::RegistryHeap* heap);
    static void destroy(PipelineFrameQueue* queue,u32 flags);
    u32 initialize(s32 modelCapacity,sead::RegistryHeap* heap);
    void finishFrame();
    void flushModels();
    void resetWork();
    u32 submitWork();
    void notifyCompleted();
    void executeJob(void* context,sead::JobItem* job);
    void enqueue(sead::JobItem* job);
    void submitOrExecute(sead::JobItem* job);
};
}
WWHD_SIZE(j3d::pipeline::JobPointerArray_l,12);
WWHD_SIZE(j3d::pipeline::ModelPointerArray_l,12);
WWHD_OFFSET(j3d::pipeline::QueueTask_l,mMethods,0xC);
WWHD_OFFSET(j3d::pipeline::QueueTask_l,mDelegate,0x10);
WWHD_SIZE(j3d::pipeline::QueueTask_l,0x20);
WWHD_OFFSET(j3d::pipeline::FrameWorkerSet,mTask,0x24);
WWHD_OFFSET(j3d::pipeline::FrameWorkerSet,mQueues,0x44);
WWHD_OFFSET(j3d::pipeline::FrameWorkerSet,mWorkers,0x188);
WWHD_SIZE(j3d::pipeline::FrameWorkerSet,0x1F8);
WWHD_OFFSET(j3d::pipeline::PipelineQueueMethods_l,mSendMessage,0x1C);
WWHD_OFFSET(j3d::pipeline::PipelineQueueMethods_l,mStart,0x2C);
WWHD_OFFSET(j3d::pipeline::PipelineQueueMethods_l,mHook,0x44);
WWHD_SIZE(j3d::pipeline::PipelineQueueMethods_l,0x48);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mModels,0x10);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mLocks,0x1C);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mFlag,0xD0);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mWork,0xD8);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mDelegate,0x2D0);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mDispatcher,0x2E0);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mJobs,0x2E4);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mOutputs,0x350);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mCompletion,0x368);
WWHD_OFFSET(j3d::pipeline::PipelineFrameQueue,mCompleted,0x388);
WWHD_SIZE(j3d::pipeline::PipelineFrameQueue,0x390);
WWHD_SIZE(j3d::pipeline::PipelineCoreMap_l,12);
