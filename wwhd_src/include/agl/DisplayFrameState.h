// Display element allocation is 0x800 bytes; only assigned-method fields are exposed here.
#pragma once
#include "wwhd.h"
#include "sead/Foundation.h"
#include "sead/Viewport.h"
#include "sead/CafeGameFramework.h"
#include "agl/RenderInputCollections.h"
#include "agl/RenderCommandPool.h"
#include "agl/RenderJobOrder.h"
#include "agl/TextureData.h"
#include "agl/RenderSurface.h"
#include "agl/RenderBuffer.h"
#include "agl/gx2/RuntimeUtility.h"
#include "agl/RenderRecording.h"
namespace agl { namespace render {
struct RenderDispatchContext;
// Borrowed listener (the HD graphics task's fade listener, installed by RenderQueueManager::setViewOutput 027C1C64):
// the view calls the interface subobject at +0x20 through its table slot +8; the rest of the listener stays opaque.
struct RenderViewListenerTable_l { u8 mPrefix[8]; sead::VtableSlot mInvoke; };
struct RenderViewListenerInterface_l {
    gptr<RenderViewListenerTable_l> mMethods;
    void invoke(RenderDispatchContext* context) { gabi::call_ptr<void>(mMethods->mInvoke.mFunction,this,context); }
};
struct RenderFrameHook_l { u8 mPrefix[0x20]; RenderViewListenerInterface_l mListener; };
WWHD_OFFSET(RenderViewListenerTable_l,mInvoke,8);
WWHD_OFFSET(RenderFrameHook_l,mListener,0x20);
// Scalar type names for the display sources' .rodata/.data words (glue `data` lines need a plain type name).
typedef be<f32> DisplayFloatConstant;
typedef be<f64> DisplayDoubleConstant;
typedef gptr<void> DisplayObjectPointer;
struct DisplayIntegerExtent { be<s32> x,y; };
// Job and command tables the view allocates from the heap in allocateFrameStorage (0x1C-byte jobs, 0x40-byte commands).
// A guest array's bounds as the original walks it: the count is loaded first, then the storage, and the end is
// storage + count * element size in 32-bit guest addresses (so a count past the storage wraps as on the console).
template <class T> struct GuestRange { T* first; T* end; };
template <class T> inline GuestRange<T> guestRange(u32 count,u32 storage) {
    return GuestRange<T>{gabi::at<T>(storage),gabi::at<T>(storage + count * (u32)sizeof(T))};
}
template <class T> inline GuestRange<gptr<T>> pointerRange(const sead::PointerBuffer& list) {
    u32 count = (u32)(s32)list.mCount;
    u32 storage = gabi::ea(list.mStorage.get());
    return guestRange<gptr<T>>(count,storage);
}
struct RenderJobTable {
    be<u32> mCount;
    gptr<RenderJob> mJobs;
    RenderJob* at(u32 index) const { return &static_cast<RenderJob*>(mJobs)[index]; }
    GuestRange<RenderJob> range() const {
        u32 count = mCount;
        u32 jobs = gabi::ea(mJobs.get());
        return guestRange<RenderJob>(count,jobs);
    }
};
struct RenderCommandTable {
    be<u32> mCount;
    gptr<agl::gx2util::TimedCounter> mCommands;
    agl::gx2util::TimedCounter* at(u32 index) const { return &static_cast<agl::gx2util::TimedCounter*>(mCommands)[index]; }
};
struct DisplayFrameState {
    be<u32> mMethods;
    be<u32> mCategory;
    u8 mCategoryPadding[4];
    gptr<agl::RenderBuffer> mRenderTarget;
    gptr<agl::TextureData> mAttachment;
    gptr<agl::TextureData> mSecondaryAttachment;
    agl::TextureData mTextureA;
    agl::TextureData mTextureB;
    agl::ColorSurface mSurfaceA;
    agl::DepthSurface mSurfaceAuxiliary;
    agl::ColorSurface mSurfaceB;
    agl::RenderBuffer mEmbeddedBuffer;
    gptr<agl::TextureData> mExtraAttachment;
    agl::TextureData mTextureC;
    agl::ColorSurface mSurfaceC;
    sead::RenderVector2 mSize;
    gptr<RenderCommandPool> mCommands;
    sead::VirtualFrameBuffer mFrameBuffer;
    sead::Viewport mViewport;
    sead::Viewport mScaledViewport;
    be<u32> mFlags;
    RenderPointerArray mPending;
    RenderJobTable mPrimaryJobs;
    RenderJobTable mSecondaryJobs;
    be<s32> mCurrentJob;
    be<u32> mClearMask;
    be<f32> mClearColor[4];
    be<f32> mClearDepth;
    be<u32> mClearStencil;
    be<u8> mSurfaceChanged;
    u8 mDeferredPadding[3];
    RenderPointerArray mDeferred;
    RenderCommandTable mSubmittedCommands;
    RenderPointerArray mSubmitted;
    gptr<RenderFrameHook_l> mBeginHook;
    gptr<RenderFrameHook_l> mPassHook;
    gptr<RenderFrameHook_l> mCommandHook;
    // The render target (an agl::RenderBuffer) as the logical surface the sead viewport and layer calls take: the
    // buffer begins with sead::VirtualFrameBuffer, whose size/area words are sead::RenderSurface's.
    sead::RenderSurface* targetSurface() const {
        return reinterpret_cast<sead::RenderSurface*>(static_cast<agl::RenderBuffer*>(mRenderTarget));
    }
    // The pending layers (appended by RenderQueueManager::update through appendPending).
    gptr<agl::RenderLayerPrefix>* pendingLayers() const {
        return static_cast<gptr<agl::RenderLayerPrefix>*>(static_cast<void*>(mPending.mStorage));
    }
    // Recorded commands (agl::gx2util::TimedCounter, AGL-GX2#1's view of the 0x40-byte command: a DisplayListRecording
    // prefix, the request tag at +0x1C (mCount there) and the owning layer at +0x20 (mRate there)).
    static agl::DisplayListStorage* recording(agl::gx2util::TimedCounter* command) {
        return static_cast<agl::DisplayListStorage*>(static_cast<void*>(command));
    }
    static agl::RenderLayerPrefix* commandLayer(const agl::gx2util::TimedCounter* command) {
        return *reinterpret_cast<const gptr<agl::RenderLayerPrefix>*>(&command->mRate);
    }
    gptr<agl::gx2util::TimedCounter>* deferredCommands() const {
        return static_cast<gptr<agl::gx2util::TimedCounter>*>(static_cast<void*>(mDeferred.mStorage));
    }
    gptr<agl::gx2util::TimedCounter>* submittedCommands() const {
        return static_cast<gptr<agl::gx2util::TimedCounter>*>(static_cast<void*>(mSubmitted.mStorage));
    }
    void clearPending();
    void appendPending(agl::RenderLayerPrefix*);
    void setSurfaceExtent(const sead::RenderVector2*);
    void refreshSurfaceExtent();
    void releaseAttachments();
    void prepareRenderPass(agl::RenderLayerPrefix*,u32);
    void finishRenderPass(agl::RenderLayerPrefix*);
    void sortDeferred();
    void invokePassHook();
    void removePending(agl::RenderLayerPrefix*);
    void submitPrimaryJobs(RenderPointerArray*);
    void submitSecondaryJobs(RenderPointerArray*);
    void appendDeferred(agl::gx2util::TimedCounter*,agl::RenderLayerPrefix*);
    void prepareAttachedSurface();
    void refreshAttachmentBounds();
    void markRandomDeferredWord();
    void submitDisplayLists(u32);
    void renderPendingPasses();
    void allocateFrameStorage(u32,const DisplayIntegerExtent*,const DisplayIntegerExtent*,s32,sead::RegistryHeap*);
    void configureAttachments(u32,DisplayFrameState*,u32,s32,u32,s32,u32,u8);
};
WWHD_SIZE(DisplayFrameState,0x800);
WWHD_OFFSET(DisplayFrameState,mFlags,0x790);
WWHD_OFFSET(DisplayFrameState,mPending,0x794);
WWHD_OFFSET(DisplayFrameState,mPrimaryJobs,0x7A0);
WWHD_OFFSET(DisplayFrameState,mSecondaryJobs,0x7A8);
WWHD_OFFSET(DisplayFrameState,mCurrentJob,0x7B0);
WWHD_OFFSET(DisplayFrameState,mDeferred,0x7D4);
s32 randomizeRenderOrder();
} }
// Bindings the display sources share (bound once, in DisplaySurfacePreparation.cpp): sead's frame-buffer activation
// (0274D690) and the game's colour-texture attach for a surface (025C0928, d_snap's TU).
void bindRenderBuffer(agl::RenderBuffer*);
void attachSurfaceTexture(agl::ColorSurface*,const agl::TextureData*);
namespace agl { namespace render {
void destroyDisplayFrameState(DisplayFrameState*,u32);
DisplayFrameState* createDisplayFrameState(DisplayFrameState*);
} }
WWHD_OFFSET(agl::render::DisplayFrameState,mSize,0x738);
WWHD_OFFSET(agl::render::DisplayFrameState,mFrameBuffer,0x744);
WWHD_OFFSET(agl::render::DisplayFrameState,mViewport,0x760);
WWHD_OFFSET(agl::render::DisplayFrameState,mScaledViewport,0x778);
WWHD_OFFSET(agl::render::DisplayFrameState,mSurfaceChanged,0x7D0);
WWHD_OFFSET(agl::render::DisplayFrameState,mSecondaryAttachment,0x14);
WWHD_OFFSET(agl::render::DisplayFrameState,mExtraAttachment,0x55C);
// Attachment ownership word101F8DD4 is borrowed from the dynamic texture cache without initialization.

WWHD_OFFSET(agl::render::DisplayFrameState,mCategory,4);

WWHD_OFFSET(agl::render::DisplayFrameState,mBeginHook,0x7F4);
WWHD_OFFSET(agl::render::DisplayFrameState,mPassHook,0x7F8);
WWHD_OFFSET(agl::render::DisplayFrameState,mCommandHook,0x7FC);

WWHD_OFFSET(agl::render::DisplayFrameState,mEmbeddedBuffer,0x51C);
WWHD_OFFSET(agl::render::DisplayFrameState,mSubmittedCommands,0x7E0);
WWHD_OFFSET(agl::render::DisplayFrameState,mSubmitted,0x7E8);

WWHD_OFFSET(agl::render::DisplayFrameState,mTextureA,0x18);
WWHD_OFFSET(agl::render::DisplayFrameState,mTextureB,0xA8);
WWHD_OFFSET(agl::render::DisplayFrameState,mSurfaceA,0x138);
WWHD_OFFSET(agl::render::DisplayFrameState,mSurfaceAuxiliary,0x280);
WWHD_OFFSET(agl::render::DisplayFrameState,mSurfaceB,0x3D4);
WWHD_OFFSET(agl::render::DisplayFrameState,mTextureC,0x560);
WWHD_OFFSET(agl::render::DisplayFrameState,mSurfaceC,0x5F0);
WWHD_OFFSET(agl::render::DisplayFrameState,mClearMask,0x7B4);
WWHD_OFFSET(agl::render::DisplayFrameState,mClearColor,0x7B8);
WWHD_OFFSET(agl::render::DisplayFrameState,mClearDepth,0x7C8);
WWHD_OFFSET(agl::render::DisplayFrameState,mClearStencil,0x7CC);
