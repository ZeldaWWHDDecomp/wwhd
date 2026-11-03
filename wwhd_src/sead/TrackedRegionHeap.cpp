// Allocation records and region queries reconstructed from local WWHD instructions.
#include "sead/TrackedRegionHeap.h"
using namespace sead;
static RegistryHeapDispatch sTrackedHeapDispatch;
static RegistryLifetimeDispatch sTrackedHeapInterface;
static RegistryLifetimeDispatch sTrackedHeapTypeTable;
static RuntimeType sTrackedHeapType;
static u32 sTrackedHeapTypeGuard;
// This heap kind supplies its own parent-type table; the guard and node are shared.
static RegistryLifetimeDispatch sTrackedHeapParentTypeTable;
void* allocateTrackedHeapStorage(u32 size);
void releaseTrackedHeapStorage(void* object);
void constructTrackedHeapBase(RegistryHeap* object,const SafeText* name,RegistryHeap* parent,
    void* region,u32 size,s32 direction,u32 locking);
void disposeTrackedHeapBase(RegistryHeap* object);
void destroyTrackedHeapBase(RegistryHeap* object,u32 flags);
void disposeTrackedHeapRange(RegistryHeap* object,u32 begin,u32 end);
void lockTrackedHeap(void* mutex);
void unlockTrackedHeap(void* mutex);

static void reportTrackedAllocationFailure(RegistryManagerState* manager,TrackedRegionHeap* heap,
        u32 requestedSize,s32 requestedAlignment,u32 workingSize,u32 workingAlignment,AllocationFailureEvent* event) {
    if(!manager) return;
    AllocationFailureReceiver* receiver=manager->mFailureReceiver;
    if(!receiver) return;
    event->mWorkingAlignment=workingAlignment;
    event->mRequestedAlignment=requestedAlignment;
    event->mRequestedSize=requestedSize;
    event->mWorkingSize=workingSize;
    event->mHeap=heap;
    gabi::call_ptr<void>(receiver->mTable->mReport.mFunction,receiver,event);
}

/* 027698D8 */
TrackedRegionHeap* TrackedRegionHeap::create(TrackedRegionHeap* object,const SafeText* name,
        RegistryHeap* parent,TrackedAllocationRecord* records,u32 recordBytes,
        void* region,u32 regionSize,u32 locking) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!object) object=static_cast<TrackedRegionHeap*>(allocateTrackedHeapStorage(sizeof(TrackedRegionHeap)));
    if(!object) return object;
    constructTrackedHeapBase(object,name,parent,region,regionSize,1,locking);
    object->mObject.mTable=gabi::ea(&sTrackedHeapDispatch);
    TrackedAllocationPool* pool=gabi::at<TrackedAllocationPool>(gabi::ea(object)+offsetof(TrackedRegionHeap,mRecords));
    if(!pool) pool=static_cast<TrackedAllocationPool*>(allocateTrackedHeapStorage(sizeof(TrackedAllocationPool)));
    if(pool) {
        pool->mActive.mCount=0;
        pool->mStorage=nullptr;
        pool->mCapacity=0;
        pool->mUnused=nullptr;
        pool->mActive.mPrevious=&pool->mActive;
        pool->mActive.mNext=&pool->mActive;
    }
    void* mutex=nullptr;
    if(static_cast<u32>(object->mFlags)&1) { mutex=object->mLock; lockTrackedHeap(mutex); }
    s32 count=recordBytes>>4;
    if(count>0 && records) {
        object->mRecords.mUnused=records;
        for(s32 i=1;i<count;++i) records[i-1].mAddress=&records[i];
        records[count-1].mAddress=nullptr;
        object->mRecords.mStorage=records;
        object->mRecords.mCapacity=count;
    }
    if(mutex) unlockTrackedHeap(mutex);
    return object;
}
/* 02769A04 */
TrackedRegionHeap* TrackedRegionHeap::createWithBuffer(const SafeText* name,void* buffer,
        u32 bufferBytes,void* region,u32 regionSize,u32 locking) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!buffer) return nullptr;
    TrackedAllocationRecord* records=gabi::at<TrackedAllocationRecord>(gabi::ea(buffer)+sizeof(TrackedRegionHeap));
    return create(static_cast<TrackedRegionHeap*>(buffer),name,nullptr,records,
        bufferBytes-sizeof(TrackedRegionHeap),region,regionSize,locking);
}
/* 02769A50 */
u32 TrackedRegionHeap::metadataSize(u32 recordCount) {
    return sizeof(TrackedRegionHeap)+recordCount*sizeof(TrackedAllocationRecord);
}
/* 02769A5C */
void TrackedRegionHeap::destroyHeap() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 target=dispatch()->mDestroy.mFunction;
    RegistryHeap* parent=mParent;
    gabi::call_ptr<void>(target,this,2u);
    if(parent && gabi::call_ptr<u32>(parent->dispatch()->mSupportsRelease.mFunction,parent)) {
        gabi::call_ptr<void>(parent->dispatch()->mRelease.mFunction,parent,this);
    }
}
/* 02769AE0 */
void* TrackedRegionHeap::allocate(u32 size,s32 alignment) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 workingSize=size<4?4:size;
    RegistryManagerState* manager=sRegistryManager;
    u32 sign=0u-(static_cast<u32>(alignment)>>31);
    u32 workingAlignment=(static_cast<u32>(alignment)^sign)-sign;
    if(workingAlignment&(workingAlignment-1)) {
        gabi::FrameLocal<AllocationFailureEvent> event(8);
        reportTrackedAllocationFailure(manager,this,size,alignment,workingSize,workingAlignment,event.get());
        return nullptr;
    }
    void* mutex=nullptr;
    u32 flags=mFlags;
    workingSize=(workingSize+3)&~3u;
    if(flags&1) { mutex=mLock; lockTrackedHeap(mutex); }
    s32 count=mRecords.mActive.mCount;
    s32 capacity=mRecords.mCapacity;
    if(count>=capacity) {
        gabi::FrameLocal<AllocationFailureEvent> event(0x1C);
        reportTrackedAllocationFailure(manager,this,size,alignment,workingSize,workingAlignment,event.get());
        if(mutex) unlockTrackedHeap(mutex);
        return nullptr;
    }
    Link* end=&mRecords.mActive;
    Link* link=mRecords.mActive.mNext;
    u32 begin=mRegionStart;
    u32 cursor=begin;
    u32 candidate=0;
    TrackedAllocationRecord* before=nullptr;
    while(link!=end) {
        candidate=(cursor+workingAlignment-1)&~(workingAlignment-1);
        TrackedAllocationRecord* record=objectOf<TrackedAllocationRecord>(link,offsetof(TrackedAllocationRecord,mLink));
        u32 address=gabi::ea(record->mAddress.get());
        if(address>=candidate && address-candidate>=workingSize) { before=record; break; }
        u32 bytes=record->mSize;
        cursor=address+bytes;
        link=record->mLink.mNext;
    }
    if(!before || candidate==0) {
        candidate=(cursor+workingAlignment-1)&~(workingAlignment-1);
        u32 regionEnd=begin+static_cast<u32>(mRegionSize);
        if(regionEnd<candidate || regionEnd-candidate<workingSize) candidate=0;
    }
    if(candidate==0) {
        gabi::FrameLocal<AllocationFailureEvent> event(0x30);
        reportTrackedAllocationFailure(manager,this,size,alignment,workingSize,workingAlignment,event.get());
        if(mutex) unlockTrackedHeap(mutex);
        return nullptr;
    }
    TrackedAllocationRecord* record=mRecords.mUnused;
    if(record) mRecords.mUnused=static_cast<TrackedAllocationRecord*>(record->mAddress.get());
    if(record) { record->mLink.mNext=nullptr; record->mLink.mPrevious=nullptr; }
    Link* inserted=gabi::at<Link>(gabi::ea(record)+offsetof(TrackedAllocationRecord,mLink));
    if(before) before->mLink.insertBefore(inserted);
    else mRecords.mActive.insertBefore(inserted);
    mRecords.mActive.mCount=static_cast<s32>(mRecords.mActive.mCount)+1;
    if(record) {
        if(before) { record->mAddress=gabi::at<void>(candidate); record->mSize=workingSize; }
        else { record->mSize=workingSize; record->mAddress=gabi::at<void>(candidate); }
    } else {
        // The original still stores both fields if inconsistent metadata supplies no free record.
        if(before) {
            gabi::store<u32>(gabi::ea(record)+offsetof(TrackedAllocationRecord,mAddress),candidate);
            gabi::store<u32>(gabi::ea(record)+offsetof(TrackedAllocationRecord,mSize),workingSize);
        } else {
            gabi::store<u32>(gabi::ea(record)+offsetof(TrackedAllocationRecord,mSize),workingSize);
            gabi::store<u32>(gabi::ea(record)+offsetof(TrackedAllocationRecord,mAddress),candidate);
        }
    }
    if(mutex) unlockTrackedHeap(mutex);
    return gabi::at<void>(candidate);
}
/* 02769DEC */
TrackedAllocationRecord* TrackedRegionHeap::findAllocation(void* address) {
    Link* end=&mRecords.mActive;
    Link* link=mRecords.mActive.mNext;
    while(link!=end) {
        TrackedAllocationRecord* record=objectOf<TrackedAllocationRecord>(link,offsetof(TrackedAllocationRecord,mLink));
        if(record->mAddress==address) return record;
        link=record->mLink.mNext;
    }
    return nullptr;
}
/* 02769E24 */
void TrackedRegionHeap::release(void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!address) return;
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    TrackedAllocationRecord* record=findAllocation(address);
    if(record) {
        record->mLink.detach();
        s32 count=mRecords.mActive.mCount;
        TrackedAllocationRecord* unused=mRecords.mUnused;
        mRecords.mActive.mCount=count-1;
        record->mAddress=unused;
        mRecords.mUnused=record;
    }
    if(mutex) unlockTrackedHeap(mutex);
}
/* 02769EC4 */
void* TrackedRegionHeap::resizeFront(void* address,u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    TrackedAllocationRecord* record=findAllocation(address);
    void* result=nullptr;
    if(record) {
        u32 previousSize=record->mSize;
        if(size<previousSize) {
            u32 start=gabi::ea(record->mAddress.get());
            u32 difference=previousSize-size;
            record->mSize=size;
            result=gabi::at<void>(start+difference);
            record->mAddress=result;
        } else if(size==previousSize) result=record->mAddress;
        else {
            Link* previous=record->mLink.mPrevious;
            TrackedAllocationRecord* neighbor=previous==&mRecords.mActive?nullptr:
                objectOf<TrackedAllocationRecord>(previous,offsetof(TrackedAllocationRecord,mLink));
            u32 lowerBound;
            if(!neighbor) lowerBound=mRegionStart;
            else {
                u32 start=gabi::ea(neighbor->mAddress.get());
                u32 bytes=neighbor->mSize;
                lowerBound=start+bytes;
            }
            u32 start=gabi::ea(record->mAddress.get());
            u32 resized=start+(previousSize-size);
            if(lowerBound<=resized) {
                result=gabi::at<void>(resized);
                record->mAddress=result;
                record->mSize=size;
            }
        }
    }
    if(mutex) unlockTrackedHeap(mutex);
    return result;
}
/* 0276A070 */
void* TrackedRegionHeap::resizeBack(void* address,u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    TrackedAllocationRecord* record=findAllocation(address);
    void* result=nullptr;
    if(record) {
        u32 previousSize=record->mSize;
        if(size>previousSize) {
            Link* next=record->mLink.mNext;
            TrackedAllocationRecord* neighbor=next==&mRecords.mActive?nullptr:
                objectOf<TrackedAllocationRecord>(next,offsetof(TrackedAllocationRecord,mLink));
            u32 upperBound;
            if(!neighbor) {
                u32 begin=mRegionStart;
                u32 bytes=mRegionSize;
                upperBound=begin+bytes;
            } else upperBound=gabi::ea(neighbor->mAddress.get());
            u32 start=gabi::ea(record->mAddress.get());
            if(start+size<=upperBound) { record->mSize=size; result=record->mAddress; }
        } else {
            if(size<previousSize) record->mSize=size;
            result=record->mAddress;
        }
    }
    if(mutex) unlockTrackedHeap(mutex);
    return result;
}
/* 0276A208 */
void TrackedRegionHeap::releaseAll() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    disposeTrackedHeapRange(this,0,0);
    Link* link=mRecords.mActive.mNext;
    Link* end=&mRecords.mActive;
    while(link!=end) {
        Link* next=link->mNext;
        link->detach();
        s32 count=mRecords.mActive.mCount;
        TrackedAllocationRecord* unused=mRecords.mUnused;
        mRecords.mActive.mCount=count-1;
        TrackedAllocationRecord* record=objectOf<TrackedAllocationRecord>(link,offsetof(TrackedAllocationRecord,mLink));
        record->mAddress=unused;
        link=next;
        mRecords.mUnused=record;
    }
    if(mutex) unlockTrackedHeap(mutex);
}
/* 0276A2C8 */
u32 TrackedRegionHeap::freeSize() const {
    const Link* end=&mRecords.mActive;
    const Link* link=mRecords.mActive.mNext;
    u32 used=0;
    while(link!=end) {
        const TrackedAllocationRecord* record=objectOf<TrackedAllocationRecord>(link,offsetof(TrackedAllocationRecord,mLink));
        const Link* next=record->mLink.mNext;
        u32 size=record->mSize;
        link=next;
        used+=size;
    }
    return static_cast<u32>(mRegionSize)-used;
}
/* 0276A304 */
u32 TrackedRegionHeap::maxAllocation(u32 alignment) const {
    const Link* end=&mRecords.mActive;
    const Link* link=mRecords.mActive.mNext;
    u32 start=mRegionStart;
    u32 cursor=start;
    u32 result=0;
    while(link!=end) {
        cursor=(cursor+alignment-1)&~(alignment-1);
        const TrackedAllocationRecord* record=objectOf<TrackedAllocationRecord>(link,offsetof(TrackedAllocationRecord,mLink));
        u32 address=gabi::ea(record->mAddress.get());
        u32 gap=address-cursor;
        if(result<gap) result=gap;
        u32 size=record->mSize;
        cursor=address+size;
        link=record->mLink.mNext;
    }
    u32 regionSize=mRegionSize;
    cursor=(cursor+alignment-1)&~(alignment-1);
    u32 trailing=start+regionSize-cursor;
    if(result<trailing) result=trailing;
    return result;
}
/* 0276A390 */
u32 TrackedRegionHeap::contains(u32 address) const {
    u32 begin=mRegionStart;
    if(address<begin) return 0;
    u32 end=begin+static_cast<u32>(mRegionSize);
    return address<end;
}
/* 0276A3BC */
void TrackedRegionHeap::inspectRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    Link* end=&mRecords.mActive;
    Link* link=mRecords.mActive.mNext;
    while(link!=end) link=link->mNext;
    if(mutex) unlockTrackedHeap(mutex);
}
/* 0276A43C */
void TrackedRegionHeap::synchronize() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=nullptr;
    if(static_cast<u32>(mFlags)&1) { mutex=mLock; lockTrackedHeap(mutex); }
    inspectRecords();
    if(mutex) unlockTrackedHeap(mutex);
}
/* 0276A4A0 */
void TrackedRegionHeap::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this) {
        mObject.mTable=gabi::ea(&sTrackedHeapDispatch);
        mHeapInterface=gabi::ea(&sTrackedHeapInterface);
        disposeTrackedHeapBase(this);
        destroyTrackedHeapBase(this,0);
        if(flags&1) releaseTrackedHeapStorage(this);
    }
}
/* 0276A514 */
void tracked_heap_text::Text::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseTrackedHeapStorage),this);
}
/* 0276A528 */
void tracked_heap_text::Text::assureTerminationImpl() {}
/* 0276A52C */
void tracked_heap_text::TextBuffer::assureTerminationImpl() {
    u32 text=gabi::ea(mText.get());
    u32 capacity=mCapacity;
    *gabi::at<be<u8>>(text+capacity-1)=0;
}
/* 0276A544 */
void tracked_heap_text::TextBuffer::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseTrackedHeapStorage),this);
}
/* 0276A558 */
void tracked_heap_text::FixedText::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseTrackedHeapStorage),this);
}
/* 0276A56C */
u32 TrackedRegionHeap::matchesType(const RuntimeType* type) const {
    if(!sTrackedHeapTypeGuard) {
        sTrackedHeapTypeGuard=1;
        sTrackedHeapType.mTable=&sTrackedHeapTypeTable;
    }
    if(type==&sTrackedHeapType) return 1;
    if(!sRegistryHeapTypeGuard) {
        sRegistryHeapTypeGuard=1;
        sRegistryHeapType.mTable=&sTrackedHeapParentTypeTable;
    }
    return type==&sRegistryHeapType;
}
/* 0276A5E4 */
u32 TrackedRegionHeap::trimToParent() {
    return gabi::tail_ptr<u32>(dispatch()->mRegionSize.mFunction,this);
}
/* 0276A5F4 */
u32 TrackedRegionHeap::regionStart() const { return mRegionStart; }
/* 0276A5FC */
u32 TrackedRegionHeap::regionSize() const { return mRegionSize; }
/* 0276A604 */
u32 TrackedRegionHeap::supportsRelease() const { return 1; }
/* 0276A60C */
u32 TrackedRegionHeap::queryStorageTrait() const { return 1; }
/* WWHD glue
 ext 0273AD10 void* allocateTrackedHeapStorage(u32)
 ext 0273AF40 void releaseTrackedHeapStorage(void*) callback
 ext 0275589C void constructTrackedHeapBase(sead::RegistryHeap*,const sead::SafeText*,sead::RegistryHeap*,void*,u32,s32,u32)
 ext 02755C24 void disposeTrackedHeapBase(sead::RegistryHeap*)
 ext 02755A48 void destroyTrackedHeapBase(sead::RegistryHeap*,u32)
 ext 02755AB4 void disposeTrackedHeapRange(sead::RegistryHeap*,u32,u32)
 ext 027601BC void lockTrackedHeap(void*)
 ext 027601F0 void unlockTrackedHeap(void*)
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 data sTrackedHeapDispatch 1015F83C
 data sTrackedHeapInterface 1015F7EC
 data sTrackedHeapTypeTable 1015F7FC
 data sTrackedHeapType 101FDFCC
 data sTrackedHeapTypeGuard 101FD768
 data sTrackedHeapParentTypeTable 1015F7DC
*/
