/* Worker options, registration prefix and per-TU text copies from local instructions. */
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/PoolHeap.h"
namespace sead { namespace worker_runtime {
struct CounterWord { be<u32> mValue; };
struct Registration {
    RegisteredObject mBase;
    Link mLink;
    u8 mOwnerPadding[4];
    gptr<LinkList> mOwner;
    u8 mReserved[0x18];
    be<u8> mRegistered;
    u8 mTailPadding[3];
    void destroy(u32 flags);
};
WWHD_SIZE(sead::WorkerManagerConfig,0x20);
WWHD_OFFSET(sead::WorkerManagerConfig,mPriority,0);
WWHD_OFFSET(sead::WorkerManagerConfig,mStackSize,0xC);
WWHD_OFFSET(sead::WorkerManagerConfig,mQueueCapacity,0x18);
WWHD_OFFSET(sead::WorkerManagerConfig,mName,0x1C);
WWHD_SIZE(CounterWord,4);
WWHD_OFFSET(Registration,mLink,0x10);
WWHD_OFFSET(Registration,mOwner,0x1C);
WWHD_OFFSET(Registration,mRegistered,0x38);
} 
namespace worker_runtime_text {
struct Text : SafeText { void destroy(u32); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32); void assureTerminationImpl(); };
struct OwnedText : SafeTextBuffer { void destroy(u32); };
// Complete fixed-buffer capacities are not established by deletion-only bodies.
struct FixedTextA : SafeTextBuffer { void destroy(u32); };
struct FixedTextB : SafeTextBuffer { void destroy(u32); };
}
namespace format_runtime_text {
struct Text : SafeText { void destroy(u32); };
}
}
