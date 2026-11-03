/* WWHD sead game framework of the Cafe platform, first part of its translation unit (the rest is
 * SEAD#11): the TU's text copy, the initialize forwarder and the constructor (the construction
 * arguments, frame counters and timer, and the logical and physical frame buffers).
 * Reconstructed from this game's instructions. */

#include "sead/CafeGameFramework.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void constructTickTime(void* time);

static constexpr u32 kDestroyRelease = 1;

static FrameworkVtable_l sCafeGameFrameworkVtable;
static u32 sVirtualFrameBufferVtable;

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* inlined into the constructor: unit size, the whole area */
static void constructFrameBuffer(VirtualFrameBuffer* buffer) {
    FrameBufferSize* size = memberStorage(&buffer->mSize);
    if (size != NULL) {
        size->mHeight = 1.0f;
        size->mWidth = 1.0f;
    }
    FrameBufferArea* area = memberStorage(&buffer->mArea);
    if (area != NULL) {
        area->mMaxX = 1.0f;
        area->mMinY = 0.0f;
        area->mMinX = 0.0f;
        area->mMaxY = 1.0f;
    }
}

/* 0274B1D8 */
void sead::cafe_game_framework_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274B1EC */
void sead::cafe_game_framework_instances::Text::assureTerminationImpl() {}

/* 0274B1F0 */
void sead::CafeGameFramework::initialize(FrameworkInitArgs* arg) { GameFramework::initialize(arg); }

/* 0274B1F4 */
CafeGameFramework* sead::CafeGameFramework::construct(CafeGameFramework* framework, const CafeGameFrameworkArg* arg) {
    framework = memberStorage(framework);
    if (framework == NULL) return NULL;
    GameFramework::construct(framework);
    framework->mVtable = &sCafeGameFrameworkVtable;
    for (u32 i = 0; i < 9; i++) framework->mArg.mWords[i] = arg->mWords[i];
    be<u32>* pendingFrame = memberStorage(&framework->mPendingFrame);
    if (pendingFrame != NULL) *pendingFrame = 0;
    framework->mFrameCount = 0;
    framework->mFrameIndex = 0;
    constructTickTime(framework->mFrameTimer);
    framework->mCommand = 0;
    framework->mVirtualFrameBuffer.mVtable = &sVirtualFrameBufferVtable;
    framework->mMessage = 0;
    constructFrameBuffer(&framework->mVirtualFrameBuffer);
    framework->mPhysicalFrameBuffer.mVtable = &sVirtualFrameBufferVtable;
    constructFrameBuffer(&framework->mPhysicalFrameBuffer);
    framework->mReady = 0;
    framework->mSurface = 0;
    framework->mEnabled = 0;
    framework->mDirty = 0;
    framework->mEngineFlags = 0;
    return framework;
}

/* WWHD glue
 ext 02760DE8 void constructTickTime(void*)
 data sCafeGameFrameworkVtable 10144F9C nocheck
 data sVirtualFrameBufferVtable 10144F30 nocheck
*/
