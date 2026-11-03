// SEAD#11 companion views for the canonical Cafe game framework and graphics interfaces.
#pragma once
#include "sead/CafeGameFramework.h"
#include "sead/GraphicsDevice.h"
#include "sead/DisplayTargets.h"
#include "sead/ColorChannels.h"
#include "sead/GraphicsSurface.h"
namespace sead {
struct GraphicsClock { be<u32> mHigh,mLow; };
struct GraphicsQueries { GraphicsClock mTimes[4]; };

// Refines CafeGameFramework::mArg, without replacing its canonical prefix.
struct CafeGraphicsParameters {
    be<u32> mWaitVsync;
    ColorChannels mClearColor;
    be<u32> mDisplayWidth,mDisplayHeight,mCommandBytes;
    be<u8> mAllocateTargets;
    u8 mPadding[3];
};
// Exactly the opaque mRuntimeData bytes, starting at the command-memory pointer.
struct CafeGraphicsStorage {
    gptr<void> mCommandMemory;
    GraphicsColorBuffer mTvColor,mDrcColor;
    GraphicsDepthBuffer mTvDepth,mDrcDepth;
    gptr<ScanTarget> mTvBuffer,mDrcBuffer;
};
// Constructor-only tail words, refined by this companion's rendering operations.
struct CafeGraphicsTail {
    gptr<DisplayFrame> mTvOverride,mDrcOverride;
    be<u32> mSwapCallback;
    be<u8> mDeferBinding;
    u8 mPadding[3];
    gptr<GraphicsQueries> mGpuQueries;
    u8 mTail[4];
};
// Only the dual-screen manager's two trailing words; its base is canonical.
struct CafeMethodTreeScreenState { be<u32> mAuxiliaryPass,mAlternatePass; };
static constexpr u32 kDualScreenMethodTreeManagerBytes=0x3E4;
static constexpr u32 kMethodTreeScreenStateOffset=0x3DC;

void activateFrameBuffer(DisplayFrame*);
}
WWHD_SIZE(sead::GraphicsClock,8);
WWHD_SIZE(sead::GraphicsQueries,32);
WWHD_SIZE(sead::CafeGraphicsParameters,0x24);
WWHD_OFFSET(sead::CafeGraphicsParameters,mClearColor,4);
WWHD_OFFSET(sead::CafeGraphicsParameters,mDisplayWidth,0x14);
WWHD_OFFSET(sead::CafeGraphicsParameters,mAllocateTargets,0x20);
WWHD_SIZE(sead::CafeGraphicsStorage,0x29C);
WWHD_OFFSET(sead::CafeGraphicsStorage,mTvColor,4);
WWHD_OFFSET(sead::CafeGraphicsStorage,mDrcColor,0xA0);
WWHD_OFFSET(sead::CafeGraphicsStorage,mTvDepth,0x13C);
WWHD_OFFSET(sead::CafeGraphicsStorage,mDrcDepth,0x1E8);
WWHD_OFFSET(sead::CafeGraphicsStorage,mTvBuffer,0x294);
WWHD_SIZE(sead::CafeGraphicsTail,0x18);
WWHD_OFFSET(sead::CafeGraphicsTail,mSwapCallback,8);
WWHD_OFFSET(sead::CafeGraphicsTail,mDeferBinding,0xC);
WWHD_OFFSET(sead::CafeGraphicsTail,mGpuQueries,0x10);
WWHD_SIZE(sead::CafeMethodTreeScreenState,8);
Pair32 seadGraphicsSystemTime();
void seadGraphicsSetContext(void*);
void seadGraphicsWaitVsync();
void seadGraphicsSwapStatus(u32*,u32*,void*,void*);
void* seadAllocateOwnerStorage(u32,void*,s32);
sead::MethodTreeMgrBase* seadInitializeGraphicsOwner(sead::MethodTreeMgrBase*);
void seadUpdateGraphicsTaskManager(sead::TaskMgr*);
void seadUpdateGraphicsOwner(sead::MethodTreeMgrBase*);
void seadGraphicsSampleTop(sead::GraphicsClock*);
void seadGraphicsSampleBottom(sead::GraphicsClock*);
void seadDrawGraphicsOwnerTv(sead::MethodTreeMgrBase*);
void seadDrawGraphicsOwnerDrc(sead::MethodTreeMgrBase*);
void seadEnterGraphicsFrame(sead::CafeGameFramework*);
void seadLeaveGraphicsFrame(sead::CafeGameFramework*);
void seadPrepareGraphicsTaskManager(sead::TaskMgr*);
Pair32 seadElapsedGraphicsTime(const sead::GraphicsClock*);
void seadGraphicsFlush();
void seadGraphicsDrawDone();
void seadGraphicsSwapBuffers();
void seadInvalidateGraphicsQueries(sead::GraphicsQueries*,u32);
Pair32 seadGraphicsGpuToCpu(u32,u32);
Pair32 seadAddGraphicsClock(const sead::GraphicsClock*,const sead::GraphicsClock*);
void seadGraphicsInitialize(void*);
void seadClearDriverMemory(void*,u32);
void seadInitializeDriverBase(void*);
void seadConstructBlendArray(sead::BlendState*,u32,u32,u32);
void seadResetGraphicsDriver(void*);
void seadGraphicsInitializeContext(void*,u32);
void seadGraphicsSurfaceLayout(sead::GraphicsSurface*);
void seadGraphicsColorRegisters(sead::GraphicsColorBuffer*);
void seadGraphicsDepthRegisters(sead::GraphicsDepthBuffer*);
void seadGraphicsHiZInfo(sead::GraphicsDepthBuffer*,u32*,u32*);
void seadGraphicsHiZEnable(sead::GraphicsDepthBuffer*,u32);
sead::ScanTarget* seadConstructScanBuffer(sead::ScanTarget*,u32,u32);
void seadGraphicsSetSwapInterval(u32);
