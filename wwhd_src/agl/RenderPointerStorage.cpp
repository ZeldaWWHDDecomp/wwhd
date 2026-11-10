// This unit's own copies of the sead pointer-buffer storage, erase and heap-sort methods (render pointer arrays).
#include "agl/RenderInputCollections.h"
#include "sead/ElementAt.h"
using namespace agl::render;
/* 027D78FC */
void agl::render::RenderPointerArray::adoptStorage(s32 capacity,be<u32>* storage) {
    if (capacity <= 0 || !storage) return;
    mCapacity = capacity;
    mStorage = storage;
    mCount = 0;
}
/* 027D7920 */
void agl::render::RenderPointerArray::allocateStorage(s32 capacity,void* heap,s32 alignment) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (capacity > 0) {
        be<u32>* storage = static_cast<be<u32>*>(allocateRenderPointerStorage((u32)capacity * 4u,heap,alignment));
        adoptStorage(capacity,storage);
    }
}
/* 027D7978 */
void agl::render::RenderPointerArray::releaseStorage() {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* storage = entries();
    if (storage) {
        releaseRenderPointerStorage(storage);
        mStorage = NULL;
        mCapacity = 0;
        mCount = 0;
    }
}
/* 027D79C0 */
void agl::render::RenderPointerArray::eraseRange(s32 first,s32 length) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (first < 0 || length < 0) return;
    s32 count = mCount;
    s32 end = (s32)((u32)first + (u32)length);
    if (end > count) return;
    if (end < count) {
        be<u32>* storage = entries();
        moveRenderPointerStorage(storage + first,storage + end,((u32)count - (u32)end) * 4u,0);
    }
    mCount = (u32)count - (u32)length;
}
// Moves `held` down from the 1-based heap position `hole` of the first `count` entries (max-heap under `compare`).
// guest EA arithmetic for the heap slots: wraps at 32 bits as on the console
static inline be<u32>& slotAt(be<u32>* storage,s32 index) { return sead::elementAt<be<u32>>(gabi::ea(storage),(u32)index); }
static void siftRenderPointers(be<u32>* storage,s32 count,s32 hole,u32 held,u32 compare) {
    s32 child = hole * 2;
    while (child <= count) {
        if (child < count) {
            u32 right = slotAt(storage,child);
            u32 left = slotAt(storage,child - 1);
            if (gabi::call_ptr<s32>(compare,left,right) < 0) ++child;
        }
        u32 candidate = slotAt(storage,child - 1);
        if (gabi::call_ptr<s32>(compare,held,candidate) >= 0) break;
        slotAt(storage,hole - 1) = slotAt(storage,child - 1);
        hole = child;
        child = hole * 2;
    }
    slotAt(storage,hole - 1) = held;
}
/* 027D7A4C */
void agl::render::RenderPointerArray::sort(u32 compare) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = mCount;
    be<u32>* storage = entries();
    for (s32 root = count / 2; root > 0; --root) siftRenderPointers(storage,count,root,slotAt(storage,root - 1),compare);
    while (count > 1) {
        u32 first = slotAt(storage,0);
        --count;
        u32 held = slotAt(storage,count);
        slotAt(storage,count) = first;
        siftRenderPointers(storage,count,1,held,compare);
    }
}
/* WWHD glue
ext 0273B0D4 void* allocateRenderPointerStorage(u32,void*,s32)
ext 0273AFC8 void releaseRenderPointerStorage(void*)
ext C0009988 void moveRenderPointerStorage(void*,const void*,u32,u32)
*/
