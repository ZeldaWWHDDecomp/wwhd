/** HD ProcUI framework, controller-list and compiler helper views.
 * HD-only: these views describe verified fields; opaque base storage stays byte-exact. */
#pragma once
#include "bindings.h"
enum HDFrameworkProcUIState { HDFramework_Foreground = 0, HDFramework_Background = 2, HDFramework_ExitRequested = 3 };
enum HDFrameworkRunMode { HDFramework_Idle = 0, HDFramework_Running = 1, HDFramework_ReenableOutputs = 2 };
struct HDFrameworkDelegate {
    u8 padding[4];
    gptr<void> mpObject;
    be<s16> mObjectAdjustment, mVirtualIndex;
    union { be<u32> mFunctionOrVtableAdjustment; struct { be<u16> mFunctionHigh; be<s16> mVtableAdjustment; }; };
};
struct HDFrameworkHeapParent { u8 padding[8]; be<u32> mChildCount; };
struct HDFrameworkHeap {
    u8 padding_0[0xC];
    gptr<void> __vtbl;
    u8 mListNode[0xC];
    gptr<HDFrameworkHeapParent> mpParent;
    u8 padding_20[0x18];
    be<u8> mLinked;
};
struct HDFrameworkOffsetList {
    u8 mSentinel[8];
    be<u32> mCount, mNodeOffset;
};
struct HDFrameworkController {
    u8 padding[0x138];
    HDFrameworkOffsetList mDevices;
};
struct HDFrameworkControllerManager {
    u8 padding[0xCC];
    HDFrameworkOffsetList mDevices;
    be<s32> mControllerCount, mControllerCapacity;
    gptr<gptr<void>> mpControllers;
};
struct HDFrameworkRootTask { u8 padding[0xB4]; be<u32> mDone; };
struct HDFrameworkTask { u8 padding[0x3C]; gptr<be<u32>> mpVtable; u8 padding_40[0x30]; gptr<be<u32>> mpTaskVtable; };
struct HDFrameworkRuntimeObject { gptr<be<u32>> __vtbl; };
struct HDFrameworkTaskManager { u8 padding[0x14]; gptr<HDFrameworkRuntimeObject> mpTask; };
struct HDFrameworkRenderer { u8 padding[0x138]; gptr<void> mpContextState; };
struct HDFrameworkCallback { gptr<void> mpFunction, __vtbl; };
struct HDGameFramework {
    void createControllers(u32);
    void procFrame();
    void procUI();
    void mainLoop();
    u8 padding_0[0x18];
    gptr<HDFrameworkRootTask> mpRootTask;
    gptr<HDFrameworkTask> mpTask;
    u8 padding_20[4];
    gptr<void> __vtbl;
    be<u32> mRunMode;
    u8 padding_2C[0x354];
    be<u32> mProcUIState;
    HDFrameworkCallback mCallback;
    be<u8> mOutputsReenabled;
    u8 padding_38D[3];
};
struct HDFrameworkTaskParameter { gptr<void> __vtbl; be<u32> mMode, mFlags; };
struct HDFrameworkTaskArgument {
    be<u32> mWordsBefore[0x5C / 4];
    be<u32> mContext;
    gptr<HDFrameworkTaskParameter> mpParameter;
    be<u32> mWordsAfter[0x1C / 4];
};
struct HDFrameworkControllerFrame {
    u8 padding_0[8];
    be<u32> mArgumentCount, mArgumentConstructor;
    HDFrameworkTaskArgument mArgument;
    HDFrameworkTaskArgument mCopy;
    u8 padding_110[0x18];
};
struct HDFrameworkWaitFrame {
    u8 padding[8];
    be<u32> mHigh, mLow, mWaitHigh, mWaitLow;
    u8 padding_18[0x10];
};
struct HDFrameworkTypeInfo { gptr<void> mpParent; };
struct HDFrameworkPiPair { be<f32> mNegative, mPositive; };
struct HDFrameworkHeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    be<u8> mFirstObject, mSecondObject;
    u8 padding[2];
    be<u32> mZero[4];
};
WWHD_SIZE(HDGameFramework, 0x390);
WWHD_OFFSET(HDGameFramework, mpRootTask, 0x18);
WWHD_OFFSET(HDGameFramework, mProcUIState, 0x380);
WWHD_OFFSET(HDGameFramework, mOutputsReenabled, 0x38C);
WWHD_OFFSET(HDFrameworkHeap, mLinked, 0x38);
WWHD_OFFSET(HDFrameworkController, mDevices, 0x138);
WWHD_OFFSET(HDFrameworkControllerManager, mControllerCount, 0xDC);
WWHD_SIZE(HDFrameworkTaskArgument, 0x80);
WWHD_SIZE(HDFrameworkControllerFrame, 0x128);
WWHD_SIZE(HDFrameworkWaitFrame, 0x28);
WWHD_SIZE(HDFrameworkHeaderStatics, 0x1C);
void HDFramework_firstHeaderObject(be<u8>*);
void HDFramework_secondHeaderObject(be<u8>*);
void HDFramework_unlink(void*);
void HDFramework_heapBaseDestroy(void*, s32);
void HDFramework_baseConstruct(void*, u32);
void HDFramework_taskArgumentConstruct(void*, void*);
HDFrameworkTask* HDFramework_createTask(u32, void*);
void* HDFramework_createGamepad(u32, u32, u32);
u32 HDFramework_createRemoteDevice(u32, u32, u32);
HDFrameworkController* HDFramework_createRemote(u32, u32, u32);
u32 HDFramework_createRemoteAddon(u32, u32);
u32 HDFramework_createDebugDevice(u32, u32);
HDFrameworkController* HDFramework_createDebugController(u32, u32);
u32 HDFramework_createDebugAddon(u32, u32);
void* HDFramework_createKeyboard(u32, u32, u32, u32);
void HDFramework_listPushBack(u32, u32);
u32 HDFramework_baseDelegate(u32);
void HDFramework_rootBeginFrame(u32);
void HDFramework_resetFrameArena(u32);
void HDFramework_beginFrame(u32);
void HDFramework_endFrame(u32);
void HDFramework_rootEndFrame(u32);
void HDFramework_baseProcUI(u32);
u32 HDFramework_isBackgroundActive(u32);
void HDFramework_setBackgroundActive(u32, u32);
void HDFramework_beginRender(u32);
void HDFramework_setExitRequested(u32, u32);
u32 HDFramework_getExitManager();
void HDFramework_shutdownHost(u32);
void HDFramework_destroyTask(u32);
void HDFramework_finalizeNetwork(u32);
void HDFramework_finalizeSystem(u32);
void HDFramework_finalizeServices(u32);
void HDFramework_endRender(u32);
Pair32 HDFramework_divideTicks(u32, u32, u32, u32);
void HDFramework_waitTicks(void*);
u32 HDFramework_OSSaveDoneReady();
void HDFramework_ProcUIInit(u32);
void HDFramework_ProcUIDrawDoneRelease();
u32 HDFramework_ProcUIProcessMessages(u32);
void HDFramework_ProcUIShutdown();
void HDFramework_GX2SetTVEnable(u32);
void HDFramework_GX2SetDRCEnable(u32);
void HDFramework_GX2SetContextState(u32);
u32 HDFramework_bossIsInitialized();
void HDFramework_bossFinalize();
void HDFramework_actFinalize();
void HDFramework_OSBlockThreadsOnExit();
void HDFramework_Exit(s32);

WWHD_OFFSET(HDFrameworkDelegate, mObjectAdjustment, 8);
WWHD_OFFSET(HDFrameworkDelegate, mVirtualIndex, 0xA);
WWHD_OFFSET(HDFrameworkDelegate, mVtableAdjustment, 0xE);
WWHD_SIZE(HDFrameworkDelegate, 0x10);
WWHD_OFFSET(HDFrameworkHeap, mpParent, 0x1C);
WWHD_SIZE(HDFrameworkOffsetList, 0x10);
WWHD_OFFSET(HDFrameworkTask, mpTaskVtable, 0x70);
WWHD_OFFSET(HDFrameworkTaskManager, mpTask, 0x14);
WWHD_OFFSET(HDFrameworkRenderer, mpContextState, 0x138);
WWHD_OFFSET(HDFrameworkTaskArgument, mContext, 0x5C);
WWHD_OFFSET(HDFrameworkControllerFrame, mCopy, 0x90);
