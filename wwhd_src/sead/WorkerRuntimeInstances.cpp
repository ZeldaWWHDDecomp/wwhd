/* Worker creation defaults, registration teardown and retained text entry points.
 * Local instruction reconstruction only; no external reference used. */
#include "sead/WorkerRuntimeInstances.h"
#include "sead/PoolHeap.h"
using namespace sead::worker_runtime;
extern void* allocateWorkerObject(u32);
extern void releaseWorkerObject(void*);
extern void releaseWorkerArray(void*);

static u32 sDefaultWorkerPriority;
static sead::SafeTextVtable sWorkerBufferTable;
static sead::DisposerVtable sWorkerRegistrationTable;
static sead::SafeTextVtable sFormatWideTable;
static sead::SafeTextVtable sFormatByteTable;
static u16 sEmptyWideText;
static u8 sEmptyByteText;
struct ConstantText { gptr<void> mText; gptr<void> mVtable; };
static ConstantText sFormatWideText;
static ConstantText sFormatByteText;
/* 02759810 */
static void finishWorkerManagerThunk(void* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    static_cast<sead::WorkerManager*>(manager)->settle();
}
/* 02759814 */
static sead::WorkerManagerConfig* constructWorkerOptions(sead::WorkerManagerConfig* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) object=static_cast<sead::WorkerManagerConfig*>(allocateWorkerObject(sizeof(sead::WorkerManagerConfig)));
    if(object!=NULL) {
        object->mQueueCapacity=32;
        object->mName.v=gabi::guest_str("WorkerMgr");
        for(u32 index=0;index<3;++index) {
            object->mPriority[index]=sDefaultWorkerPriority;
            object->mStackSize[index]=0x4000;
        }
    }
    return object;
}
/* 02759880 */
void sead::worker_runtime_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this!=NULL && (flags&1)) releaseWorkerObject(this);
}
/* 02759894 */
void sead::worker_runtime_text::Text::assureTerminationImpl() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 02759898 */
void sead::worker_runtime_text::TextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this!=NULL && (flags&1)) releaseWorkerObject(this);
}
/* 027598AC */
void sead::worker_runtime_text::TextBuffer::assureTerminationImpl() {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* text=mText;
    u32 capacity=mCapacity;
    text[capacity-1]=0;
}
/* 027598C4 */
void sead::worker_runtime_text::OwnedText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this==NULL) return;
    void* text=mText;
    mVtable=&sWorkerBufferTable;
    if(text!=NULL) releaseWorkerArray(text);
    if(flags&1) releaseWorkerObject(this);
}
/* 02759928 */
CounterWord* constructWorkerCounter(CounterWord* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) object=static_cast<CounterWord*>(allocateWorkerObject(sizeof(CounterWord)));
    if(object!=NULL) object->mValue=0;
    return object;
}
/* 02759964 */
void sead::worker_runtime::Registration::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this==NULL) return;
    u32 registered=mRegistered;
    mBase.mVtable=&sWorkerRegistrationTable;
    if(registered!=0) {
        sead::LinkList* owner=mOwner;
        if(owner!=NULL) {
            mOwner=NULL;
            mLink.detach();
            owner->mCount=static_cast<u32>(owner->mCount)-1;
        }
        mRegistered=0;
    }
    mBase.destroy(0);
    if(flags&1) releaseWorkerObject(this);
}
/* 02759A0C */
void sead::worker_runtime_text::FixedTextA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this!=NULL && (flags&1)) releaseWorkerObject(this);
}
/* 02759A20 */
void sead::worker_runtime_text::FixedTextB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this!=NULL && (flags&1)) releaseWorkerObject(this);
}
/* 02759B40 */
static void initializeFormatTextObjects() {
    gabi::AutoFrame frame{gabi::current_function()};
    sFormatWideText.mVtable=&sFormatWideTable;
    sFormatWideText.mText=&sEmptyWideText;
    sFormatByteText.mVtable=&sFormatByteTable;
    sFormatByteText.mText=&sEmptyByteText;
}
/* 02759B84 */
void sead::format_runtime_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this!=NULL && (flags&1)) releaseWorkerObject(this);
}
/* WWHD glue
 data sDefaultWorkerPriority 101F8B9C
 data sWorkerBufferTable 1014754C
 data sWorkerRegistrationTable 10147584
 data sFormatWideTable 10147634
 data sFormatByteTable 1014764C
 data sEmptyWideText 10147664
 data sEmptyByteText 10147630
 data sFormatWideText 104A0CD8
 data sFormatByteText 104A0CE0
 str 101475E4 "WorkerMgr"
 ext 0273AD10 void* allocateWorkerObject(u32)
 ext 0273AF40 void releaseWorkerObject(void*)
 ext 0273AFC8 void releaseWorkerArray(void*)
 ext 02759148 void sead::WorkerManager::settle()
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 02752BEC void sead::RegisteredObject::destroy(u32)
*/
