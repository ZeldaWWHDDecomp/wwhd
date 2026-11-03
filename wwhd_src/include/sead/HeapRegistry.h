// WWHD heap registration, region accessors and per-TU text adapters.
// Layouts reconstructed from the local game instructions.
#pragma once
#include "sead/FoundationObjects.h"
#include "sead/RuntimeType.h"
#include "sead/Allocation.h"
#include "sead/FileDevice.h"
#include "JSystem/JKRHeap_adapters.h"
namespace sead {
struct ResourcePathText : SafeText {
    void destroy(u32 flags);
    void terminateEmpty();
};
// Fixed-buffer capacities are not established by these deletion-only bodies.
struct HeapFixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct HeapFixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct HeapFixedTextC : SafeTextBuffer { void destroy(u32 flags); };
struct HeapFixedTextD : SafeTextBuffer { void destroy(u32 flags); };
struct RegistryText : SafeTextBuffer {
    void destroy(u32 flags);
    void terminateEmpty();
    void terminateBuffer();
    void destroyBuffer(u32 flags);
    void destroyBase(u32 flags);
    void destroyBufferBase(u32 flags);
};
struct RegionText : SafeTextBuffer {
    void destroy(u32 flags);
    void terminateEmpty();
    void terminateBuffer();
    void destroyBuffer(u32 flags);
    void destroyBase(u32 flags);
    void destroyBufferBase(u32 flags);
};
struct HeapText : SafeTextBuffer {
    void destroy(u32 flags);
    void terminateEmpty();
    void terminateBuffer();
    void destroyBuffer(u32 flags);
};
struct RegistryMatrix34 { be<u32> mWords[12]; };
struct RegistryMatrix44 { be<u32> mWords[16]; };
struct PrimitiveReceiverDispatch { VtableSlot mHeader; VtableSlot mConsume; };
struct PrimitiveRenderer {
    be<u32> mTable;
    RegistryMatrix34 mModel;
    RegistryMatrix44 mProjection;
    gptr<void> mVertexShader;
    gptr<void> mPixelShader;
    be<u32> mProjectionRegister;
    be<u32> mModelRegister;
    be<u32> mTextureRegister;
    be<u32> mTopColorRegister;
    be<u32> mBottomColorRegister;
    be<u32> mTextureSlot;
    u8 mShaderResources[112];
    u8 mFetchShader[32];
    u8 mSampler[24];
    u8 mFilledMeshState[24];
    gptr<void> mFilledVertices;
    gptr<void> mFilledIndices;
    u8 mWireMeshState[32];
    gptr<void> mWireVertices;
    gptr<void> mWireIndices;
    // The constructor allocates 0x18C bytes for this object.
    u8 mTrailingStorage[8];
    u32 loadFromFile(void* heap,const SafeText* path);
    void finish();
    void setModel(const RegistryMatrix34* matrix);
    void setProjection(void* projection);
    void prepare();
    void draw(const RegistryMatrix34* model,const void* topColor,const void* bottomColor,
              const void* vertices,u32 vertexCount,const void* indices,u32 indexCount,const void* texture);
    void drawFilled(const RegistryMatrix34* model,const void* topColor,const void* bottomColor);
    void drawWire(const RegistryMatrix34* model,const void* topColor,const void* bottomColor);
};
struct RegistryLifetimeDispatch {
    VtableSlot mHeader;
    VtableSlot mDestroy;
};
// Virtual table shared by every heap kind (base, free-region and frontier heaps).
struct RegistryHeapDispatch {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mMatchesType;
    VtableSlot mRuntimeType;
    VtableSlot mDestroyHeap;
    VtableSlot mTrimToParent;
    VtableSlot mAllocate;
    VtableSlot mRelease;
    VtableSlot mResizeFront;
    VtableSlot mResizeBack;
    VtableSlot mReleaseAll;
    VtableSlot mRegionStart;
    VtableSlot mRegionEnd;
    VtableSlot mRegionSize;
    VtableSlot mFreeSize;
    VtableSlot mMaxAllocation;
    VtableSlot mContains;
    VtableSlot mSupportsRelease;
};
struct RegistryHeap;
// Offset lists: each element embeds its Link at the list's offset.
inline Link* linkAt(const void* object,u32 offset) { return gabi::at<Link>(gabi::ea(object) + offset); }
template <class T> inline T* objectOf(const Link* link,u32 offset) { return gabi::at<T>(gabi::ea(link) - offset); }
struct RegisteredObject : Disposer {
    void destroy(u32 flags);
};
struct RegistryObjectList : LinkList { be<u32> mOffset; };
struct RegistryHeapBuffer {
    PointerBuffer mBuffer;
    gptr<RegistryHeap> mInlineStorage[4];
};
struct AllocationFailureEvent {
    gptr<RegistryHeap> mHeap;
    be<u32> mRequestedSize;
    be<s32> mRequestedAlignment;
    be<u32> mWorkingSize;
    be<s32> mWorkingAlignment;
};
struct AllocationFailureDispatch { VtableSlot mHeader; VtableSlot mReport; };
struct AllocationFailureReceiver { gptr<AllocationFailureDispatch> mTable; };
struct RegistryManagerState {
    be<u32> mTable;
    gptr<AllocationFailureReceiver> mFailureReceiver;
};
struct RegistryArena_l { be<u32> mStart; be<u32> mSize; be<u32> mState; };
struct RegistryMutex_l { u8 mStorage[60]; };
struct RegistryHeap {
    ObjectHeader mObject;
    gptr<be<u8>> mName;
    be<u32> mTextTable;
    be<u32> mHeapInterface;
    be<u32> mRegionStart;
    be<u32> mRegionSize;
    gptr<RegistryHeap> mParent;
    RegistryObjectList mChildren;
    Link mParentLink;
    RegistryObjectList mDisposables;
    be<s32> mDirection;          // +1: from the region start, -1: from its end
    u8 mLock[60];
    be<u32> mFlags;
    RegistryHeapDispatch* dispatch() const { return gabi::at<RegistryHeapDispatch>(mObject.mTable); }
    void inspectState();
    RegistryHeap* findContaining(void* address);
    void appendChild(void* child);
    void removeChild(void* child);
    void disposeRange(u32 begin,u32 end);
    void appendDisposable(void* object);
    void removeDisposable(void* object);
    void destroy(u32 flags);
    void detachAndDispose();
    u32 matchesType(const RuntimeType* type) const;
};
struct DirectionalHeap : RegistryHeap {
    u32 usableStart() const;
    u32 usableEnd() const;
};
struct FrontierCheckpoint { be<u32> mFront; be<u32> mBack; };
struct FrontierHeap : DirectionalHeap {
    static RegistryHeap* create(u32 size,const SafeText* name,RegistryHeap* parent,s32 direction,u32 locking);
    void* allocate(u32 size,s32 alignment);
    u32 maxAllocation(s32 alignment);
    void restoreCursors(const FrontierCheckpoint* checkpoint);
    u32 queryRegionHeapTrait() const;
    u32 queryFrontierHeapTrait() const;
    u32 trimFrontierEnd();
    u32 trimFrontierStart();
    u32 trimToParent();
    u32 supportsRelease() const;
    void release(void* allocation);
    u32 resizeFront(void* allocation,u32 size);
    u32 resizeBack(void* allocation,u32 size);
    void destroyHeap();
    void destroy(u32 flags);
    u32 matchesType(const RuntimeType* type) const;
    be<u32> mFront;
    be<u32> mBack;
    u32 regionStart() const;
    u32 regionEnd() const;
    u32 regionSize() const;
    u32 freeSize();
    u32 contains(u32 address) const;
    void synchronize();
    void resetCursors();
    void releaseAll();
};
struct RegistryBlock {
    Link mLink;
    be<u32> mSize;
    be<u32> mPadding;
};
struct FreeRegionHeap : RegistryHeap {
    static RegistryHeap* create(u32 size,const SafeText* name,RegistryHeap* parent,s32 direction,u32 locking);
    static RegistryHeap* createAt(u32 start,u32 size,const SafeText* name,u32 locking);
    void* allocate(u32 size,s32 alignment);
    u32 queryRegionHeapTrait() const;
    void destroyHeap();
    void destroy(u32 flags);
    u32 matchesType(const RuntimeType* type) const;
    be<u32> mSearchMode;
    RegistryObjectList mFreeBlocks;
    RegistryObjectList mUsedBlocks;
    u32 regionStart() const;
    u32 regionSize() const;
    u32 contains(u32 address) const;
    RegistryBlock* findFreeBlock(u32 size,u32 mode);
    RegistryBlock* findAlignedFreeBlock(u32 size,u32 alignment,u32 mode);
    RegistryBlock* findAlignedFreeBlockBack(u32 size,u32 alignment,u32 mode);
    void appendUsedBlock(RegistryBlock* block);
    void insertFreeBlock(RegistryBlock* block);
    void coalesceFreeBlocks();
    u32 supportsRelease() const;
    void* resizeFront(void* allocation,u32 size);
    void* resizeBack(void* allocation,u32 size);
    void release(void* allocation);
    void releaseAll();
    void walkUsedBlocks();
    void walkFreeBlocks();
    void walkBlocks();
    u32 allocationSize(void* allocation);
    u32 trimTrailingFreeBlock();
    u32 trimLeadingFreeBlock();
    u32 trimToParent();
    u32 freeSize();
    u32 maxAllocation(s32 alignment);
    RegistryBlock* findTrailingFreeBlock();
    RegistryBlock* findLeadingFreeBlock();
    void seedFreeBlocks();
    RegistryBlock* allocateBlockFront(u32 size);
    RegistryBlock* allocateAlignedBlockFront(u32 size,u32 alignment);
    RegistryBlock* findFreeBlockBack(u32 size,u32 mode);
    RegistryBlock* allocateBlockBack(u32 size);
    RegistryBlock* allocateAlignedBlockBack(u32 size,u32 alignment);
};
}
WWHD_OFFSET(sead::RegisteredObject,mHeap,0);
WWHD_OFFSET(sead::RegisteredObject,mLink,4);
WWHD_OFFSET(sead::RegisteredObject,mVtable,12);
WWHD_SIZE(sead::RegisteredObject,16);
WWHD_SIZE(sead::RegistryText,12);
WWHD_SIZE(sead::RegionText,12);
WWHD_SIZE(sead::HeapText,12);
WWHD_OFFSET(sead::RegistryHeap,mRegionStart,0x1C);
WWHD_OFFSET(sead::RegistryHeap,mRegionSize,0x20);
WWHD_OFFSET(sead::RegistryHeap,mDirection,0x50);
WWHD_OFFSET(sead::RegistryHeap,mParent,0x24);
WWHD_OFFSET(sead::RegistryHeap,mChildren,0x28);
WWHD_OFFSET(sead::RegistryObjectList,mOffset,12);
WWHD_SIZE(sead::RegistryObjectList,16);
WWHD_OFFSET(sead::RegistryHeap,mParentLink,0x38);
WWHD_OFFSET(sead::RegistryHeap,mDisposables,0x40);

WWHD_OFFSET(sead::RegistryHeap,mLock,0x54);
WWHD_OFFSET(sead::RegistryHeap,mFlags,0x90);
WWHD_SIZE(sead::RegistryHeap,0x94);
WWHD_OFFSET(sead::FrontierHeap,mFront,0x94);
WWHD_OFFSET(sead::FrontierHeap,mBack,0x98);
WWHD_SIZE(sead::FrontierHeap,0x9C);
void releaseRegistryStorage(void*);
void lockRegistryHeap(void*);
void unlockRegistryHeap(void*);
void* allocateRegistryStorage(u32);
sead::RegistryHeap* findRegistryHeap(void*,void*);
sead::RegistryHeap* registryCurrentHeap(void*);
WWHD_SIZE(sead::RegistryMatrix34,48);
WWHD_SIZE(sead::RegistryMatrix44,64);
WWHD_OFFSET(sead::PrimitiveRenderer,mModel,4);
WWHD_OFFSET(sead::PrimitiveRenderer,mProjection,0x34);
WWHD_OFFSET(sead::PrimitiveRenderer,mVertexShader,0x74);
WWHD_OFFSET(sead::PrimitiveRenderer,mPixelShader,0x78);
WWHD_OFFSET(sead::PrimitiveRenderer,mProjectionRegister,0x7C);
WWHD_OFFSET(sead::PrimitiveRenderer,mModelRegister,0x80);
WWHD_OFFSET(sead::PrimitiveRenderer,mTextureRegister,0x84);
WWHD_OFFSET(sead::PrimitiveRenderer,mTopColorRegister,0x88);
WWHD_OFFSET(sead::PrimitiveRenderer,mBottomColorRegister,0x8C);
WWHD_OFFSET(sead::PrimitiveRenderer,mTextureSlot,0x90);
WWHD_OFFSET(sead::PrimitiveRenderer,mFetchShader,0x104);
WWHD_OFFSET(sead::PrimitiveRenderer,mSampler,0x124);
WWHD_OFFSET(sead::PrimitiveRenderer,mFilledVertices,0x154);
WWHD_OFFSET(sead::PrimitiveRenderer,mFilledIndices,0x158);
WWHD_OFFSET(sead::PrimitiveRenderer,mWireVertices,0x17C);
WWHD_OFFSET(sead::PrimitiveRenderer,mWireIndices,0x180);
WWHD_SIZE(sead::PrimitiveRenderer,0x18C);
void copyRegistryModel(const sead::RegistryMatrix34*,sead::RegistryMatrix34*);
const sead::RegistryMatrix44* updateRegistryProjection(void*);
void copyRegistryProjection(const sead::RegistryMatrix44*,sead::RegistryMatrix44*);
void combineRegistryMatrices(sead::RegistryMatrix44*,const sead::RegistryMatrix44*,const sead::RegistryMatrix34*);
void selectRegistryFetchShader(const void*);
void selectRegistryVertexShader(const void*);
void selectRegistryPixelShader(const void*);
void setRegistryVertexUniform(u32,u32,const void*);
void setRegistryPixelUniform(u32,u32,const void*);
void setRegistryLineWidth(f32);
void setRegistryPixelTexture(const void*,u32);
void setRegistryPixelSampler(const void*,u32);
void setRegistryAttribute(u32,u32,u32,const void*);
void drawRegistryIndices(u32,u32,u32,const void*,u32,u32);
void constructRegistryLock(void*,sead::RegistryHeap*);
WWHD_SIZE(sead::RegistryManagerState,8);
WWHD_OFFSET(sead::RegistryManagerState,mFailureReceiver,4);
WWHD_SIZE(sead::RegistryArena_l,12);
WWHD_OFFSET(sead::RegistryArena_l,mSize,4);
WWHD_SIZE(sead::RegistryMutex_l,60);
void configureRegistryArena(sead::RegistryArena_l*,u32);
WWHD_SIZE(sead::SafeText,8);
WWHD_OFFSET(sead::SafeText,mVtable,4);
WWHD_SIZE(sead::RegistryBlock,16);
WWHD_OFFSET(sead::RegistryBlock,mSize,8);
WWHD_OFFSET(sead::RegistryBlock,mPadding,12);
WWHD_OFFSET(sead::FreeRegionHeap,mSearchMode,0x94);
WWHD_OFFSET(sead::FreeRegionHeap,mFreeBlocks,0x98);
WWHD_OFFSET(sead::FreeRegionHeap,mUsedBlocks,0xA8);
WWHD_SIZE(sead::FreeRegionHeap,0xB8);
WWHD_SIZE(sead::RegistryHeapBuffer,28);
WWHD_OFFSET(sead::RegistryHeapBuffer,mInlineStorage,12);
void constructStaticRegistryArena(sead::RegistryArena_l*);
void constructStaticRegistryMutex(void*);
void registerRegistryLifetime(void*);
void destroyRegistryLock(void*,u32);
void clearRegistryMemory(void*,u32);
WWHD_OFFSET(sead::RegistryHeapDispatch,mTrimToParent,0x28);
WWHD_OFFSET(sead::RegistryHeapDispatch,mAllocate,0x30);
WWHD_OFFSET(sead::RegistryHeapDispatch,mRelease,0x38);
WWHD_OFFSET(sead::RegistryHeapDispatch,mResizeFront,0x40);
WWHD_OFFSET(sead::RegistryHeapDispatch,mResizeBack,0x48);
WWHD_OFFSET(sead::RegistryHeapDispatch,mMaxAllocation,0x78);
WWHD_OFFSET(sead::RegistryHeapDispatch,mContains,0x80);
WWHD_OFFSET(sead::RegistryHeapDispatch,mSupportsRelease,0x88);
WWHD_SIZE(sead::RegistryHeapDispatch,0x90);
WWHD_OFFSET(sead::RegistryLifetimeDispatch,mDestroy,8);
WWHD_SIZE(sead::RegistryLifetimeDispatch,16);

WWHD_SIZE(sead::AllocationFailureEvent,20);
WWHD_OFFSET(sead::RegistryManagerState,mFailureReceiver,4);

// The registry manager pointer (101F8B4C), owned by sead/HeapRegistryStatics.cpp.
extern gptr<sead::RegistryManagerState>& sRegistryManager;
// The registry-heap base runtime type (node 101FDF9C, guard 101FD2C4): a guarded static each heap kind's matchesType
// initialises inline, storing its own translation unit's table; owned by sead/HeapRegistryStatics.cpp.
extern be<u32>& sRegistryHeapTypeGuard;
extern sead::RuntimeType& sRegistryHeapType;
