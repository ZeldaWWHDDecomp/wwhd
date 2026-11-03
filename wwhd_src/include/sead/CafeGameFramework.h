/* WWHD sead game framework of the Cafe platform: its translation unit starts here (the text copy,
 * the initialize forwarder and the constructor; the rest of the TU is SEAD#11). The layout is
 * known as far as the constructor writes it (0x380 bytes). */
#pragma once
#include "sead/GameFramework.h"

namespace sead {

/* the logical size of a frame buffer and the area of it in use */
struct FrameBufferSize {
    be<f32> mWidth;
    be<f32> mHeight;
};
struct FrameBufferArea {
    be<f32> mMinX;
    be<f32> mMinY;
    be<f32> mMaxX;
    be<f32> mMaxY;
};
struct VirtualFrameBuffer {
    FrameBufferSize mSize;
    FrameBufferArea mArea;
    gptr<void> mVtable;
};

/* the construction arguments copied into the framework (nine words) */
struct CafeGameFrameworkArg {
    be<u32> mWords[9];
};

struct DisplayFrame;
struct CafeGraphicsParameters;
struct CafeGraphicsStorage;
struct CafeGraphicsTail;
struct GraphicsClock;

struct CafeGameFramework : GameFramework {
    CafeGameFrameworkArg mArg;
    be<u32> mPendingFrame;
    be<u32> mFrameIndex;
    be<u32> mFrameCount;
    u8 mFrameTimer[8];
    be<u32> mCommand;
    be<u32> mMessage;
    VirtualFrameBuffer mVirtualFrameBuffer;
    VirtualFrameBuffer mPhysicalFrameBuffer;
    be<u32> mSurface;
    u8 mRuntimeData[0x29C];
    be<u32> mEngineFlags;
    be<u32> mEnabled;
    be<u32> mDirty;
    be<u8> mReady;
    u8 mPadding375[0xB];

    static void initialize(FrameworkInitArgs* arg);
    static CafeGameFramework* construct(CafeGameFramework* framework, const CafeGameFrameworkArg* arg);
    // The remaining Cafe framework operations are implemented by the SEAD#11 companion.
    void initializeGraphics(RegistryHeap* heap, u32 flags, u32 queueMode,
                            const FrameBufferSize* tvSize, const FrameBufferSize* drcSize,
                            RegistryHeap* scanHeap);
    DisplayFrame* targetForPass(u32 pass);
    VirtualFrameBuffer* viewportForPass(u32 pass);
    void beginTick();
    MethodTreeMgrBase* createMethodTreeManager(RegistryHeap* heap);
    void threadLoop();
    void measureGpuTime();
    void processFrame();
    void drawTv();
    void drawDrc();
    void bindTv();
    void bindDrc();
    void renderFrame();
    void updateManager();
    void waitForSwap();
    void swapBuffers();
    void clearTarget();

    // Typed views of constructor-only opaque storage; no field or size changes.
    CafeGraphicsParameters* graphicsParameters() {
        return gabi::at<CafeGraphicsParameters>(gabi::ea(&mArg));
    }
    CafeGraphicsStorage* graphicsStorage() {
        return gabi::at<CafeGraphicsStorage>(gabi::ea(mRuntimeData));
    }
    CafeGraphicsTail* graphicsTail() {
        return gabi::at<CafeGraphicsTail>(gabi::ea(&mEngineFlags));
    }
    GraphicsClock* frameStartClock() {
        return gabi::at<GraphicsClock>(gabi::ea(mFrameTimer));
    }

};

/* per-TU copies of the Cafe game framework translation unit */
namespace cafe_game_framework_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}

}

WWHD_OFFSET(sead::VirtualFrameBuffer, mArea, 8);
WWHD_OFFSET(sead::VirtualFrameBuffer, mVtable, 0x18);
WWHD_SIZE(sead::VirtualFrameBuffer, 0x1C);
WWHD_SIZE(sead::CafeGameFrameworkArg, 0x24);
WWHD_OFFSET(sead::CafeGameFramework, mArg, 0x50);
WWHD_OFFSET(sead::CafeGameFramework, mPendingFrame, 0x74);
WWHD_OFFSET(sead::CafeGameFramework, mFrameTimer, 0x80);
WWHD_OFFSET(sead::CafeGameFramework, mCommand, 0x88);
WWHD_OFFSET(sead::CafeGameFramework, mVirtualFrameBuffer, 0x90);
WWHD_OFFSET(sead::CafeGameFramework, mPhysicalFrameBuffer, 0xAC);
WWHD_OFFSET(sead::CafeGameFramework, mSurface, 0xC8);
WWHD_OFFSET(sead::CafeGameFramework, mEngineFlags, 0x368);
WWHD_OFFSET(sead::CafeGameFramework, mReady, 0x374);
WWHD_SIZE(sead::CafeGameFramework, 0x380);
