/* Frame heaps and the shared library heap tree, reconstructed from WWHD. */
#include "nw/ut/FrameHeap.h"
using namespace nw::ut;
static HeapChildren sHeapRoots;
struct FillPatterns { be<u32> values[3]; };
static FillPatterns sFillPatterns;
static u8 sHeapVtable;
static u8 sFrameHeapVtable;
/* 028F626C */
FrameHeap* FrameHeap::create(u32 memory,u32 size,u32 options) {
    u32 end=(memory+size)&~3u;
    u32 start=(memory+3)&~3u;
    if(start>end || end-start<sizeof(FrameHeap)) return nullptr;
    FrameHeap* heap=gabi::at<FrameHeap>(start);
    if(start) {
        heap->sibling.previous=nullptr;
        heap->sibling.next=nullptr;
        heap->vtable=gabi::ea(&sHeapVtable);
        HeapChildren* children=&heap->children;
        if(!gabi::ea(children)) children=(HeapChildren*)utAllocateHeap(sizeof(HeapChildren));
        if(children) {
            children->count=0;
            children->root.previous=&children->root;
            children->root.next=&children->root;
        }
        heap->vtable=gabi::ea(&sFrameHeapVtable);
    }
    heap->initialize(0x46524D48,start+sizeof(FrameHeap),end,options);
    heap->checkpoint=nullptr;
    heap->head=heap->begin;
    heap->tail=heap->end;
    return heap;
}
/* 028F637C */
FrameHeap* FrameHeap::destroy() { finalize(); return this; }
/* 028F63AC */
void* FrameHeap::allocateHead(u32 size,u32 alignment) {
    u32 mask=alignment-1;
    u32 result=(head+mask)&~mask;
    u32 next=result+size;
    if(next>(u32)tail) return nullptr;
    u32 old=head;
    fillAllocated(gabi::at<void>(old),next-old);
    head=next;
    return gabi::at<void>(result);
}
/* 028F6440 */
void* FrameHeap::allocateTail(u32 size,u32 alignment) {
    u32 result=((u32)tail-size)&~(alignment-1);
    if(result<(u32)head) return nullptr;
    u32 old=tail;
    fillAllocated(gabi::at<void>(result),old-result);
    tail=result;
    return gabi::at<void>(result);
}
/* 028F64C4 */
void* FrameHeap::allocate(u32 size,s32 alignment) {
    if(!size) size=1;
    size=(size+3)&~3u;
    lock();
    void* result=alignment>=0?allocateHead(size,alignment):allocateTail(size,0u-(u32)alignment);
    unlock();
    return result;
}
/* 028F655C */
void FrameHeap::resetHead() {
    u32 start=begin;
    fillFreed(gabi::at<void>(start),(u32)head-start);
    u32 first=begin;
    checkpoint=nullptr;
    head=first;
}
/* 028F65A4 */
void FrameHeap::resetTail() {
    u32 limit=end,old=tail;
    fillFreed(gabi::at<void>(old),limit-old);
    HeapCheckpoint* record=checkpoint;
    u32 last=end;
    while(record) {
        record->tail=last;
        record=record->previous;
        last=end;
    }
    tail=last;
}
/* 028F6604 */
void FrameHeap::reset(u32 direction) {
    lock();
    if(direction&ResetHead) resetHead();
    if(direction&ResetTail) resetTail();
    unlock();
}
/* 028F6664 */
Heap* nw::ut::findContaining(HeapChildren* children,u32 pointer) {
    HeapLink* sentinel=&children->root;
    HeapLink* node=sentinel->next;
    while(node!=sentinel) {
        Heap* heap=(Heap*)node;
        u32 begin=heap->begin;
        node=node->next;
        if(begin<=pointer && (u32)heap->end>pointer) {
            Heap* nested=findContaining(&heap->children,pointer);
            return nested?nested:heap;
        }
    }
    return nullptr;
}
/* 028F66F8 */
u32 Heap::fillPattern(u32 index) { return sFillPatterns.values[index]; }
/* 028F670C */
HeapChildren* nw::ut::findParent(u32 pointer) {
    Heap* heap=findContaining(&sHeapRoots,pointer);
    return heap?&heap->children:&sHeapRoots;
}
/* 028F6754 */
void Heap::setOptions(u32 value) { options=((u32)options&~255u)|(value&255); }
/* 028F6764 */
u32 Heap::getOptions() { return (u32)options&255; }
/* 028F6770 */
void Heap::fillInitial(void* memory,u32 size) {
    if(getOptions()&FillDebugPattern) utFill(memory,fillPattern(0),size);
}
/* 028F67B8 */
void Heap::initialize(u32 tag,u32 first,u32 last,u32 flags) {
    signature=tag; begin=first; end=last; options=0;
    setOptions(flags);
    fillInitial(gabi::at<void>(first),last-first);
    HeapChildren* parent=findParent(gabi::ea(this));
    gabi::Local<gptr<HeapLink>> root;
    *root=&parent->root;
    utInsertChild(parent,root.get(),this);
}
/* 028F6824 */
void Heap::finalize() {
    HeapChildren* parent=findParent(gabi::ea(this));
    utRemoveChild(parent,this);
    signature=0;
}
/* 028F6860 */
void Heap::fillFreed(void* memory,u32 size) {
    if(getOptions()&FillDebugPattern) utFill(memory,fillPattern(2),size);
}
/* 028F68A8 */
void Heap::fillAllocated(void* memory,u32 size) {
    u32 flags=getOptions();
    if(flags&ClearAllocated) utFill(memory,0,size);
    else if(flags&FillDebugPattern) utFill(memory,fillPattern(1),size);
}
/* 028F6908 */
void nw::ut::initializeHeapRoots() {
    sHeapRoots.count=0;
    sHeapRoots.root.previous=&sHeapRoots.root;
    sHeapRoots.root.next=&sHeapRoots.root;
}
/* 028F6928 */
void Heap::lock() {}
/* 028F692C */
void Heap::unlock() {}
/* 028F6930 */
u32 nw::ut::appendText(be<u8>* destination,u32 capacity,const be<u8>* source,u32 count) {
    u32 length=0;
    while((u8)destination[length]) ++length;
    if(capacity<=length+count) count=capacity-length-1;
    if(count) {
        u32 end=0;
        while((u8)destination[end]) ++end;
        for(u32 i=0;i<count;++i) {
            u8 byte=source[i];
            destination[end+i]=byte;
            if(!byte) break;
            if(i+1==count) destination[end+i+1]=0;
        }
    }
    destination[capacity-1]=0;
    return count;
}
/* WWHD glue
data sHeapRoots 104DA1BC
data sFillPatterns 101FCD2C
data sHeapVtable 1018AAF0
data sFrameHeapVtable 1018AB00
ext 0273AD10 void* utAllocateHeap(u32)
ext C000A858 void utFill(void*,u32,u32)
ext 0286DA70 void utInsertChild(nw::ut::HeapChildren*,const gptr<nw::ut::HeapLink>*,nw::ut::Heap*)
ext 0286DA9C void utRemoveChild(nw::ut::HeapChildren*,nw::ut::Heap*)
*/
