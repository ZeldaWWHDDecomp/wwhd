/** J3D draw buckets. GameCube names, structured control flow and verified WWHD semantics. */
#include "J3DDrawBuffer.h"
using namespace drawBufferHD;

static f32 l_zNear;
static f32 l_zFar;
static f32 l_unsignedBoundary;
using HeapPointer = gptr<Heap>;
static HeapPointer sRootHeap;
struct DrawFunctions {
    ptmf<u32 (J3DDrawBuffer::*)(void*)> entry[2];
};
static DrawFunctions drawFuncTable;
struct ViewMatrix {
    be<f32> values[12];
};
static ViewMatrix sViewMatrix;
struct StaticPointers {
    gptr<void> entry[4];
};
static StaticPointers sStaticPointers;
struct StaticState {
    u8 bytes[1];
};
static StaticState sStaticState;

extern u32 packetDrawClearNative(J3DPacket*);
extern u32 bufferFrameInitNative(J3DDrawBuffer*);
extern Heap* bufferFindHeapNative(Heap*, PacketSlot*);
extern Heap* bufferCurrentHeapNative();
extern u32 bufferDeleteNative(J3DDrawBuffer*);
extern f64 bufferCalcZNative(ViewMatrix*, Vec*);
extern u32 bufferAssertNative(const char*, u32, const char*);
extern u32 bufferStaticInitNative(void*);

/* 027F0A9C */
u32 J3DDrawBuffer::initialize() {
    // HD: the caller supplies the count; initialization retains it and the bucket pointer.
    u32 count = mEntryTableSize;
    f32 near = l_zNear;
    f32 far = l_zFar;
    mDrawMode = DRAW_HEAD;
    mSortMode = SORT_MAT;
    mpZMtx = nullptr;
    mpCallBackPacket = nullptr;
    f32 ratio = (f32)(fsubs_ppc(far, near) / (f32)count);
    mZNear = near;
    mZFar = far;
    mZRatio = ratio;
    return ea(this);
}

/* 027F0B04 */
u32 J3DDrawBuffer::allocBuffer(u32 bufSize) {
    PacketSlot* begin = nullptr;
    bool allocated = false;
    if ((s32)bufSize > 0) {
        Heap* heap = bufferCurrentHeapNative();
        u32 target = heap->__vtbl->alloc.get();
        begin = at<PacketSlot>(call_ptr<u32>(target, heap, bufSize * 4u, 4u));
        allocated = begin != nullptr;
    }
    u32 end;
    if (allocated) {
        end = ea(begin) + bufSize * 4u;
        mEntryTableSize = bufSize;
        mpBuffer = begin;
    } else {
        u32 count = mEntryTableSize;
        begin = mpBuffer;
        end = ea(begin) + count * 4u;
    }
    // HD: failed or nonpositive allocations clear the existing buckets too.
    u32 cursor = ea(begin);
    while (cursor != end) {
        *at<PacketSlot>(cursor) = nullptr;
        u32 count = mEntryTableSize;
        u32 base = mpBuffer.v.get();
        cursor += 4;
        end = base + count * 4u;
    }
    u32 count = mEntryTableSize;
    f32 far = mZFar;
    f32 near = mZNear;
    f32 ratio = (f32)(fsubs_ppc(far, near) / (f32)count);
    mpCallBackPacket = nullptr;
    mZRatio = ratio;
    return 0;
}

/* 027F0A0C */
static u32 J3DDrawBuffer__dt(J3DDrawBuffer* i_this, u32 flags) {
    u32 result = ea(i_this);
    if (i_this != nullptr) {
        result = bufferFrameInitNative(i_this);
        PacketSlot* storage = i_this->mpBuffer;
        if (storage != nullptr) {
            Heap* heap = bufferFindHeapNative(sRootHeap, storage);
            u32 target = heap->__vtbl->free.get();
            storage = i_this->mpBuffer;
            result = call_ptr<u32>(target, heap, storage);
            i_this->mEntryTableSize = 0;
            i_this->mpBuffer = nullptr;
        }
        if (flags & 1)
            result = bufferDeleteNative(i_this);
    }
    return result;
}

/* 027F093C */
u32 J3DDrawBuffer::frameInit() {
    u32 result = ea(this);
    u32 count = mEntryTableSize;
    u32 cursor = mpBuffer.v.get();
    u32 end = cursor + count * 4u;
    while (cursor != end) {
        J3DPacket* packet = *at<PacketSlot>(cursor);
        if (packet != nullptr) {
            do {
                if (ea(packet->mpEntrySlot.get()) != cursor) {
                    bufferAssertNative("J3DDrawBuffer.cpp", 143, "pkt->getEntryPtr() == &*it");
                }
                packet->mpEntrySlot = nullptr;
                J3DPacket* next = packet->mpNextPacket;
                result = packetDrawClearNative(packet);
                packet = next;
            } while (packet != nullptr);
            *at<PacketSlot>(cursor) = nullptr;
            count = mEntryTableSize;
            u32 base = mpBuffer.v.get();
            end = base + count * 4u;
        }
        cursor += 4;
    }
    mpCallBackPacket = nullptr;
    return result;
}

/* 027F0C14 */
u32 J3DDrawBuffer::entryZSort(J3DPacket* i_packet) {
    packetDrawClearNative(i_packet);
    Mtx34* matrix = mpZMtx;
    Vec tmp;
    tmp.x = (f32)(f64)(f32)matrix->m[0][3];
    tmp.y = (f32)(f64)(f32)matrix->m[1][3];
    tmp.z = (f32)(f64)(f32)matrix->m[2][3];
    // HD: depth remains double precision through the near/far comparisons.
    f64 value = -bufferCalcZNative(&sViewMatrix, &tmp);
    f32 near = mZNear;
    f32 ratio = mZRatio;
    u32 idx;
    if (fadds_ppc(near, ratio) < value) {
        if (fsubs_ppc(mZFar, ratio) > value) {
            f32 fIdx = to_single(value / ratio);
            u32 distance;
            if (fIdx < l_unsignedBoundary)
                distance = (u32)ftoi(fIdx);
            else
                distance = (u32)ftoi(fsubs_ppc(fIdx, l_unsignedBoundary)) + 0x80000000u;
            idx = (u32)mEntryTableSize - 1u - distance;
        } else
            idx = 0;
    } else
        idx = (u32)mEntryTableSize - 1u;
    // Backlink/next stores may alias buffer metadata: obtain each bucket separately.
    i_packet->mpEntrySlot = getEntrySlot(idx);
    i_packet->mpNextPacket = *getEntrySlot(idx);
    *getEntrySlot(idx) = i_packet;
    return 1;
}

/* 027F0DA8 */
u32 J3DDrawBuffer::entryNonSort(J3DPacket* i_packet) {
    packetDrawClearNative(i_packet);
    i_packet->mpEntrySlot = mpBuffer;
    PacketSlot* storage = mpBuffer;
    i_packet->mpNextPacket = *storage;
    storage = mpBuffer;
    *storage = i_packet;
    return 1;
}

/* 027F0E04 */
u32 J3DDrawBuffer::entryImm(J3DPacket* i_packet, u32 index) {
    if (i_packet->mpEntrySlot.get() != nullptr) {
        bufferAssertNative("J3DDrawBuffer.cpp", 243, "p_pkt->getEntryPtr() == (0)");
    }
    i_packet->mpEntrySlot = getEntrySlot(index);
    i_packet->mpNextPacket = *getEntrySlot(index);
    *getEntrySlot(index) = i_packet;
    return 1;
}

/* 027F0ECC */
u32 J3DDrawBuffer::remove(J3DPacket* i_packet, u32 index) {
    if (i_packet == nullptr)
        return 0;
    PacketSlot* backlink = i_packet->mpEntrySlot;
    u32 count;
    u32 base;
    if (backlink != nullptr) {
        count = mEntryTableSize;
        base = mpBuffer.v.get();
        u32 expected = base + (index < count ? index * 4u : 0u);
        if (ea(backlink) != expected) {
            bufferAssertNative("J3DDrawBuffer.cpp", 272,
                               "entryPtr == (0) || entryPtr == &mEntryTable[key]");
            count = mEntryTableSize;
            base = mpBuffer.v.get();
        }
    } else {
        count = mEntryTableSize;
        base = mpBuffer.v.get();
    }
    u32 slot = base + (index < count ? index * 4u : 0u);
    J3DPacket* head = *at<PacketSlot>(slot);
    if (head == nullptr) {
        if (backlink != nullptr)
            bufferAssertNative("J3DDrawBuffer.cpp", 278, "entryPtr == (0)");
        return 0;
    }
    if (head == i_packet) {
        if (ea(backlink) != slot) {
            bufferAssertNative("J3DDrawBuffer.cpp", 283, "entryPtr == &mEntryTable[key]");
            count = mEntryTableSize;
            base = mpBuffer.v.get();
        }
        J3DPacket* next = i_packet->mpNextPacket;
        slot = base + (index < count ? index * 4u : 0u);
        *at<PacketSlot>(slot) = next;
    } else {
        J3DPacket* next = head->mpNextPacket;
        // HD: the original spins without advancing when this link equals the target.
        while (next == i_packet) {
        }
        if (ea(backlink) != slot) {
            bufferAssertNative("J3DDrawBuffer.cpp", 304, "entryPtr == &mEntryTable[key]");
        }
        head->mpNextPacket = i_packet->mpNextPacket;
    }
    i_packet->mpEntrySlot = nullptr;
    i_packet->mpNextPacket = nullptr;
    return 1;
}

/* 027F1088 */
u32 J3DDrawBuffer::draw(void* context) {
    return (this->*drawFuncTable.entry[mDrawMode])(context);
}

/* 027F10CC */
u32 J3DDrawBuffer::drawHead(void* context) {
    u32 result = ea(this);
    u32 count = mEntryTableSize;
    u32 cursor = mpBuffer.v.get();
    u32 end = cursor + count * 4u;
    while (cursor != end) {
        J3DPacket* packet = *at<PacketSlot>(cursor);
        if (packet != nullptr) {
            do {
                u32 target = packet->__vtbl->draw.get();
                result = call_ptr<u32>(target, packet, context);
                packet = packet->mpNextPacket;
            } while (packet != nullptr);
            count = mEntryTableSize;
            u32 base = mpBuffer.v.get();
            end = base + count * 4u;
        }
        cursor += 4;
    }
    return result;
}

/* 027F1174 */
u32 J3DDrawBuffer::drawTail(void* context) {
    u32 result = ea(this);
    u32 count = mEntryTableSize;
    if (count == 0)
        return result;
    u32 base = mpBuffer.v.get();
    u32 cursor = base + count * 4u;
    do {
        cursor -= 4;
        J3DPacket* packet = *at<PacketSlot>(cursor);
        if (packet != nullptr) {
            do {
                u32 target = packet->__vtbl->draw.get();
                result = call_ptr<u32>(target, packet, context);
                packet = packet->mpNextPacket;
            } while (packet != nullptr);
            base = mpBuffer.v.get();
        }
    } while (cursor != base);
    return result;
}

/* 027F1210 */
u32 J3DDrawBuffer::isEmpty() {
    u32 count = mEntryTableSize;
    u32 cursor = mpBuffer.v.get();
    u32 end = cursor + count * 4u;
    while (cursor != end) {
        if (at<PacketSlot>(cursor)->get() != nullptr)
            return 0;
        cursor += 4;
    }
    return 1;
}

/* ---------------- HD: compiler-generated initialization ---------------- */
/* 027F1250 */
static u32 __sinit_J3DDrawBuffer_cpp() {
    sStaticPointers.entry[2] = nullptr;
    sStaticPointers.entry[0] = nullptr;
    sStaticPointers.entry[3] = nullptr;
    sStaticPointers.entry[1] = nullptr;
    return bufferStaticInitNative(&sStaticState);
}

/* WWHD glue
str 1016E100 in 027F093C
str 1016E114
str 1016E160 in 027F0E04
str 1016E144
str 1016E184 in 027F0ECC
str 1016E1B8
str 1016E174
str 1016E198
data l_zNear 1016E130
data l_zFar 1016E134
data l_unsignedBoundary 1016E140
data sRootHeap 101F8B4C
data drawFuncTable 101F98C8
data sViewMatrix 104B45F8
data sStaticPointers 104B4588
data sStaticState 101F988C
ext 027F1508 u32 packetDrawClearNative(J3DPacket*)
ext 027F093C u32 bufferFrameInitNative(J3DDrawBuffer*)
ext 02755FEC Heap* bufferFindHeapNative(Heap*, PacketSlot*)
ext 02756140 Heap* bufferCurrentHeapNative()
ext 0273AF40 u32 bufferDeleteNative(J3DDrawBuffer*)
ext 027F2B68 f64 bufferCalcZNative(ViewMatrix*, Vec*)
ext 0273AA24 u32 bufferAssertNative(const char*, u32, const char*)
ext 028F026C u32 bufferStaticInitNative(void*)
*/
