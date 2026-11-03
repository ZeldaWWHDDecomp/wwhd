// WWHD graphics-device fields and virtual dispatch observed in local instructions.
#pragma once
#include "sead/RenderMath.h"
#include "sead/RenderState.h"
namespace sead {
struct GraphicsDeviceTable {
    u8 mTypeDestroy[20];
    be<u32> mReset;
    be<u32> mResetAdjustment;
    be<u32> mSetViewport;
    be<u32> mViewportAdjustment;
    be<u32> mSetScissor;
    u8 mDrawingMethods[0x8C-0x28];
    be<u32> mAcquire;
    be<u32> mAcquireAdjustment;
    be<u32> mRelease;
};
struct GraphicsDevice {
    u8 mBase[12];
    gptr<GraphicsDeviceTable> mTable;
    be<u32> mOwnerThread;
    be<s32> mLockCount;
    u8 mMutex[0x3C];
    be<u32> mLockCallback;
    static GraphicsDevice* create(GraphicsDevice* object);
    static void destroy(GraphicsDevice* object, u32 flags);
    void acquire();
    void release();
    void reset();
};
}
WWHD_OFFSET(sead::GraphicsDevice,mTable,0xC);
WWHD_OFFSET(sead::GraphicsDevice,mOwnerThread,0x10);
WWHD_OFFSET(sead::GraphicsDevice,mLockCount,0x14);
WWHD_OFFSET(sead::GraphicsDevice,mMutex,0x18);
WWHD_OFFSET(sead::GraphicsDevice,mLockCallback,0x54);
WWHD_SIZE(sead::GraphicsDevice,0x58);
WWHD_OFFSET(sead::GraphicsDeviceTable,mReset,0x14);
WWHD_OFFSET(sead::GraphicsDeviceTable,mSetViewport,0x1C);
WWHD_OFFSET(sead::GraphicsDeviceTable,mSetScissor,0x24);
WWHD_OFFSET(sead::GraphicsDeviceTable,mAcquire,0x8C);
WWHD_OFFSET(sead::GraphicsDeviceTable,mRelease,0x94);

// The current graphics device (101F8B18), owned by sead/RuntimeStatics.cpp.
extern gptr<sead::GraphicsDevice>& sCurrentGraphicsDevice;
