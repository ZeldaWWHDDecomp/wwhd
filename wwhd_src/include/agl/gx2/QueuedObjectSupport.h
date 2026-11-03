#pragma once
#include "wwhd.h"
#include "JSystem/JSupport/JSUList.h"
#include "sead/FileDevice.h"
#include "sead/Thread.h"
namespace queued_object_support {
struct QueuedObjectStaticState { be<u32> mWords[4]; };
WWHD_SIZE(QueuedObjectStaticState,16);
struct QueuedObject {
    sead::Disposer mDisposer;
    be<u8> mFlag; u8 mPadding[3];
    sead::PlatformMutexStorage mFirstMutex,mSecondMutex;
    be<u32> mReserved; be<u32> mPending; be<u32> mPaddingPending;
    be<u32> mCount; u8 mEmbedded[0x40];
    gptr<QueuedObject> mSelf;
    sead::PlatformMessageQueueStorage mInputQueue; sead::PlatformMessage mInputMessage;
    sead::PlatformMessageQueueStorage mOutputQueue; sead::PlatformMessage mOutputMessage;
    JSUPtrLink mLink; be<u32> mState;
    void initialize();
    u32 forwardEmbedded();
};
WWHD_OFFSET(QueuedObject,mDisposer,0); WWHD_OFFSET(QueuedObject,mFlag,0x10);
WWHD_OFFSET(QueuedObject,mFirstMutex,0x14); WWHD_OFFSET(QueuedObject,mSecondMutex,0x40);
WWHD_OFFSET(QueuedObject,mPending,0x70); WWHD_OFFSET(QueuedObject,mCount,0x78);
WWHD_OFFSET(QueuedObject,mEmbedded,0x7C); WWHD_OFFSET(QueuedObject,mSelf,0xBC);
WWHD_OFFSET(QueuedObject,mInputQueue,0xC0); WWHD_OFFSET(QueuedObject,mInputMessage,0xFC);
WWHD_OFFSET(QueuedObject,mOutputQueue,0x10C); WWHD_OFFSET(QueuedObject,mOutputMessage,0x148);
WWHD_OFFSET(QueuedObject,mLink,0x158); WWHD_OFFSET(QueuedObject,mState,0x168); WWHD_SIZE(QueuedObject,0x16C);
}
queued_object_support::QueuedObject* createQueuedObject(queued_object_support::QueuedObject*);
void* queuedObjectAllocate(u32);
void queuedDisposerConstruct(sead::Disposer*);
JSUPtrLink* queuedLinkConstruct(JSUPtrLink*,void*);
void queuedMutexInitialize(sead::PlatformMutexStorage*);
void queuedMessageQueueInitialize(sead::PlatformMessageQueueStorage*,sead::PlatformMessage*,s32);
u32 queuedEmbeddedForward(void*);

void queuedRegisterStatic(void*);
