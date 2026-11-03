/* Assigned queued-object helpers reconstructed from the WWHD binary. */
// SDK boundary layout facts: wut coreinit mutex and messagequeue declarations.
#include "agl/gx2/QueuedObjectSupport.h"
using namespace queued_object_support;
static sead::DisposerVtable sQueuedObjectDispatch;
static QueuedObjectStaticState sQueuedStatic;
static u8 sQueuedRegistration;
/* 027EAB24 */
static void __sinit_QueuedObjectSupport_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    sQueuedStatic.mWords[2] = 0;
    sQueuedStatic.mWords[0] = 0;
    sQueuedStatic.mWords[3] = 0;
    sQueuedStatic.mWords[1] = 0;
    queuedRegisterStatic(&sQueuedRegistration);
}
/* 027EAB4C */
void QueuedObject::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mSelf = this;
    queuedMutexInitialize(&mFirstMutex);
    queuedMutexInitialize(&mSecondMutex);
    queuedMessageQueueInitialize(&mOutputQueue,&mOutputMessage,1);
    queuedMessageQueueInitialize(&mInputQueue,&mInputMessage,1);
    mState = 0;
    mCount = 0;
    mPending = 0;
}
/* 027EABB8 */
QueuedObject* createQueuedObject(QueuedObject* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object = static_cast<QueuedObject*>(queuedObjectAllocate(sizeof(QueuedObject)));
    if (object) {
        queuedDisposerConstruct(&object->mDisposer);
        object->mFlag = 0;
        object->mDisposer.mVtable = &sQueuedObjectDispatch;
        JSUPtrLink* link = gabi::at<JSUPtrLink>(gabi::ea(object) + offsetof(QueuedObject,mLink));
        if (!link) link = static_cast<JSUPtrLink*>(queuedObjectAllocate(sizeof(JSUPtrLink)));
        if (link) queuedLinkConstruct(link,object);
        object->initialize();
    }
    return object;
}
/* 027EAC3C */
u32 QueuedObject::forwardEmbedded() {
    gabi::AutoFrame frame{gabi::current_function()};
    return queuedEmbeddedForward(mEmbedded);
}
/* WWHD glue
data sQueuedStatic 104A2354
data sQueuedRegistration 101F970C
ext 028F026C void queuedRegisterStatic(void*)
data sQueuedObjectDispatch 1016DB24
ext 0273AD10 void* queuedObjectAllocate(u32)
ext 02752A84 void queuedDisposerConstruct(sead::Disposer*)
ext 027EC50C JSUPtrLink* queuedLinkConstruct(JSUPtrLink*,void*)
ext C0009D68 void queuedMutexInitialize(sead::PlatformMutexStorage*)
ext C0009D58 void queuedMessageQueueInitialize(sead::PlatformMessageQueueStorage*,sead::PlatformMessage*,s32)
ext 0284B8B8 u32 queuedEmbeddedForward(void*)
*/
