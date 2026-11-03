// Own aggregate layout; child record internals remain an AGL#14 boundary pending adoption.
#pragma once
#include "sead/HeapRegistry.h"
namespace agl {
struct DrawRegistryRecord_l {
    u8 mPrefix[0x140];
    gptr<void> mStorage;
    u8 mTail[0x10];
};
struct DrawRegistry_l {
    sead::Disposer mRegistration;
    DrawRegistryRecord_l mRecords[3];
    gptr<sead::DisposerVtable> mMethods;
    static DrawRegistry_l* construct(DrawRegistry_l* object);
    static DrawRegistry_l* get(sead::AllocationHeap* heap);
    void initialize(sead::AllocationHeap* heap);
    void destroy(u32 flags);
};
struct DrawRegistryRegistration_l : sead::Disposer { void destroy(u32 flags); };
}
WWHD_SIZE(agl::DrawRegistryRecord_l, 0x154);
WWHD_OFFSET(agl::DrawRegistryRecord_l, mStorage, 0x140);
WWHD_OFFSET(agl::DrawRegistry_l, mRecords, 0x10);
WWHD_OFFSET(agl::DrawRegistry_l, mMethods, 0x40C);
WWHD_SIZE(agl::DrawRegistry_l, 0x410);
WWHD_SIZE(agl::DrawRegistryRegistration_l, 0x10);
