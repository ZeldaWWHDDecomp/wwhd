/** HD ProcUI game framework: controller creation, frame processing and foreground/background lifecycle.
 * No GameCube counterpart. Verified call order includes save-state frame restoration and mocked exit fallthrough. */
#include "hd/hd_sys_02034B14.h"
using namespace gabi;

using FrameworkWord = be<u32>;
using ControllerManagerPointer = gptr<HDFrameworkControllerManager>;
using TaskManagerPointer = gptr<HDFrameworkTaskManager>;
using RendererPointer = gptr<HDFrameworkRenderer>;
static u8 l_heapVtable;
static u8 l_frameworkVtable;
static u8 l_callbackVtable;
static u8 l_callbackFunction;
static u8 l_parameterVtable;
static u8 l_controllerTaskParentType;
static u8 l_frameTaskParentType;
static u8 l_shutdownTaskParentType;
static FrameworkWord l_controllerTaskTypeInitialized;
static FrameworkWord l_frameTaskTypeInitialized;
static FrameworkWord l_shutdownTaskTypeInitialized;
static HDFrameworkTypeInfo l_controllerTaskType;
static HDFrameworkTypeInfo l_frameTaskType;
static HDFrameworkTypeInfo l_shutdownTaskType;
static ControllerManagerPointer l_controllerManager;
static TaskManagerPointer l_taskManager;
static RendererPointer l_renderer;
static FrameworkWord l_frameArena;
static FrameworkWord l_frameService;
static FrameworkWord l_backgroundService;
static FrameworkWord l_hostService;
static FrameworkWord l_networkService;
static FrameworkWord l_systemService;
static FrameworkWord l_services;
static FrameworkWord l_tickHigh;
static FrameworkWord l_tickLow;
static HDFrameworkPiPair l_pi;
static HDFrameworkHeaderStatics l_headerStatics;
static constexpr u32 HDFramework_taskArgumentConstructorAddress = 0x020354BC;
static constexpr u32 HDFramework_taskCallbackAddress = 0x0273C714;
static constexpr u32 HDFramework_baseInitializeAddress = 0x027476D8;
static constexpr u32 HDFramework_baseProcFrameAddress = 0x02746790;

/* 02034B14 */
static u32 HDFrameworkDelegate_invoke(HDFrameworkDelegate* i_this, u32 argument) {
    void* object = i_this->mpObject;
    if (!object) return ea(i_this);
    s32 index = i_this->mVirtualIndex;
    if (!index) return ea(i_this);
    // HD: signed halfword adjustments wrap in the 32-bit guest address space.
    u32 adjusted = ea(object) + u32(s32(i_this->mObjectAdjustment));
    if (index < 0) return gabi::call_ptr<u32>(i_this->mFunctionOrVtableAdjustment, adjusted, argument);
    be<u32>* slot = gabi::at<be<u32>>(adjusted + u32(s32(s16(i_this->mVtableAdjustment))));
    be<u32>* table = gabi::at<be<u32>>(*slot);
    return gabi::call_ptr<u32>(table[u32(index) * 2 + 1], adjusted, argument);
}

/* 02034B68 */
static void HDFrameworkHeap__dt(HDFrameworkHeap* i_this, s32 flags) {
    if (!i_this) return;
    u8 linked = i_this->mLinked;
    i_this->__vtbl = &l_heapVtable;
    if (linked) {
        HDFrameworkHeapParent* parent = i_this->mpParent;
        if (parent) {
            i_this->mpParent = nullptr;
            HDFramework_unlink(i_this->mListNode);
            parent->mChildCount = u32(parent->mChildCount) - 1;
        }
        i_this->mLinked = 0;
    }
    HDFramework_heapBaseDestroy(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}

/* 02034C10 */
static HDGameFramework* HDGameFramework__ct(HDGameFramework* i_this, u32 argument) {
    if (!i_this) {
        i_this = static_cast<HDGameFramework*>(operator_new(sizeof(HDGameFramework)));
        if (!i_this) return nullptr;
    }
    HDFramework_baseConstruct(i_this, argument);
    i_this->mProcUIState = HDFramework_Foreground;
    i_this->__vtbl = &l_frameworkVtable;
    // HD: the embedded callback may wrap to guest NULL for the top-of-memory fixture.
    HDFrameworkCallback* callback = gabi::at<HDFrameworkCallback>(ea(i_this) + offsetof(HDGameFramework, mCallback));
    bool initialized = true;
    if (!callback) {
        callback = static_cast<HDFrameworkCallback*>(operator_new(sizeof(HDFrameworkCallback)));
        if (!callback) initialized = false;
    }
    if (initialized) {
        callback->__vtbl = &l_callbackVtable;
        callback->mpFunction = &l_callbackFunction;
    }
    i_this->mOutputsReenabled = 0;
    return i_this;
}

static void push_obj(HDFrameworkControllerManager* manager, void* object) {
    s32 count = manager->mControllerCount;
    if (count < s32(manager->mControllerCapacity)) {
        manager->mpControllers[count] = object;
        manager->mControllerCount = s32(manager->mControllerCount) + 1;
    }
}
static void link_tail(HDFrameworkOffsetList* list, u32 object) {
    HDFramework_listPushBack(ea(list), object + u32(list->mNodeOffset));
    list->mCount = u32(list->mCount) + 1;
}

/* 02034CC8 */
void HDGameFramework::createControllers(u32 argument) {
    // HD: retain the original frame and overlapping task-argument copies exactly.
    u32 entry = cpu->r[1];
    u32 frameAddress = entry - sizeof(HDFrameworkControllerFrame);
    cpu->r[1] = frameAddress;
    HDFrameworkControllerFrame* frame = gabi::at<HDFrameworkControllerFrame>(frameAddress);
    frame->mArgumentCount = 2;
    frame->mArgumentConstructor = HDFramework_taskArgumentConstructorAddress;
    HDFramework_taskArgumentConstruct(&frame->mArgument, &frame->mArgumentCount);
    HDFrameworkTaskParameter* parameter = static_cast<HDFrameworkTaskParameter*>(operator_new(sizeof(HDFrameworkTaskParameter)));
    if (parameter) {
        parameter->mFlags = 0;
        parameter->__vtbl = &l_parameterVtable;
    }
    parameter->mMode = 7;
    frame->mArgument.mContext = argument;
    frame->mArgument.mpParameter = parameter;
    be<u32>* copy = reinterpret_cast<be<u32>*>(&frame->mCopy);
    be<u32>* original = reinterpret_cast<be<u32>*>(&frame->mArgument);
    for (u32 i = 0; i < sizeof(HDFrameworkTaskArgument) / 4; ++i) copy[i] = u32(original[i]);
    frame->mCopy.mWordsAfter[6] = HDFramework_taskCallbackAddress;
    HDFrameworkTask* task = HDFramework_createTask(ea(mpRootTask.get()), &frame->mCopy);
    if (!l_controllerTaskTypeInitialized) {
        l_controllerTaskTypeInitialized = 1;
        l_controllerTaskType.mpParent = &l_controllerTaskParentType;
    }
    if (task) {
        gabi::call_ptr(task->mpTaskVtable[3], task, &l_controllerTaskType);
    }
    HDFrameworkControllerManager* manager = l_controllerManager;
    push_obj(manager, HDFramework_createGamepad(0, ea(manager), 0));
    link_tail(&manager->mDevices, HDFramework_createRemoteDevice(0, ea(manager), 0));
    for (u32 i = 0; i < 4; ++i) {
        HDFrameworkController* controller = HDFramework_createRemote(0, ea(manager), i);
        link_tail(&controller->mDevices, HDFramework_createRemoteAddon(0, ea(controller)));
        push_obj(manager, controller);
    }
    link_tail(&manager->mDevices, HDFramework_createDebugDevice(0, ea(manager)));
    HDFrameworkController* controller = HDFramework_createDebugController(0, ea(manager));
    link_tail(&controller->mDevices, HDFramework_createDebugAddon(0, ea(controller)));
    push_obj(manager, controller);
    push_obj(manager, HDFramework_createKeyboard(0, ea(manager), 2, 0));
    cpu->r[1] = entry;
}

/* 02034F88 */
static u32 saveCallback() {
    return HDFramework_OSSaveDoneReady();
}


/* 02034F8C */
static void HDGameFramework_initialize(u32 a, u32 b, u32 c, u32 d) {
    // HD: replay the prologue so the saved-state chain call sees the original frame.
    auto originalFrame = gabi::AutoFrame(0x02034F8C);
    HDFramework_ProcUIInit(guest_fn((const void*)saveCallback));
    // HD: preserve the original return site and r28..r31 for cross-build save states.
    gabi::call_site(0x02034FD8, {{28, b}, {29, c}, {30, d}, {31, a}}, {}, HDFramework_baseInitializeAddress, a, b, c, d);
}


/* 02034FF8 */
static u32 HDGameFramework_delegate(u32 self) {
    return HDFramework_baseDelegate( self);
}


/* 02034FFC */
void HDGameFramework::procFrame() {
    HDGameFramework* i_this = this;
    // HD: replay the prologue and epilogue for the frame-exact save-state call.
    auto originalFrame = gabi::AutoFrame(0x02034FFC);
    HDFramework_rootBeginFrame(ea(i_this->mpRootTask.get()));
    u32 arena = l_frameArena;
    if (arena) HDFramework_resetFrameArena(arena);
    u32 initialized = l_frameTaskTypeInitialized;
    HDFrameworkTask* task = i_this->mpTask;
    if (!initialized) {
        l_frameTaskTypeInitialized = 1;
        l_frameTaskType.mpParent = &l_frameTaskParentType;
    }
    if (!task || !gabi::call_ptr<u32>(task->mpVtable[3], task, &l_frameTaskType)) task = nullptr;
    // HD: preserve the save-state chain call's LR, r30/r31, and frame restoration.
    gabi::call_site(0x02035090, {{30, ea(i_this)}, {31, ea(task)}}, {}, HDFramework_baseProcFrameAddress, task);
    i_this = gabi::at<HDGameFramework>(cpu->r[30]);
    HDFramework_beginFrame(l_frameService);
    HDFramework_endFrame(l_frameService);
    HDFramework_rootEndFrame(ea(i_this->mpRootTask.get()));
}

/* 020350C4 */
void HDGameFramework::procUI() {
    if (!mOutputsReenabled && mRunMode == HDFramework_ReenableOutputs) {
        mOutputsReenabled = 1;
        mRunMode = HDFramework_Running;
        HDFramework_GX2SetTVEnable(1);
        HDFramework_GX2SetDRCEnable(1);
    }
    HDFramework_baseProcUI(ea(this));
    u32 oldState = mProcUIState;
    if (oldState == HDFramework_Background) {
        if (HDFramework_isBackgroundActive(l_backgroundService)) HDFramework_setBackgroundActive(l_backgroundService, 0);
        HDFramework_ProcUIDrawDoneRelease();
        HDFramework_beginRender(ea(l_renderer.get()));
        oldState = mProcUIState;
    }
    u32 state = HDFramework_ProcUIProcessMessages(1);
    mProcUIState = state;
    if (state == HDFramework_ExitRequested) {
        HDFramework_setExitRequested(HDFramework_getExitManager(), 1);
        HDFramework_shutdownHost(l_hostService);
        HDFramework_ProcUIShutdown();
        HDFrameworkTaskManager* manager = l_taskManager;
        u32 exitFallthrough = ea(manager);
        if (manager) {
            HDFrameworkRuntimeObject* task = manager->mpTask;
            exitFallthrough = l_shutdownTaskTypeInitialized;
            if (!exitFallthrough) {
                l_shutdownTaskTypeInitialized = 1;
                l_shutdownTaskType.mpParent = &l_shutdownTaskParentType;
            }
            if (task && gabi::call_ptr<u32>(task->__vtbl[3], task, &l_shutdownTaskType) && task) HDFramework_destroyTask(ea(task));
        }
        u32 service = l_networkService;
        if (service) HDFramework_finalizeNetwork(service);
        service = l_systemService;
        if (service) HDFramework_finalizeSystem(service);
        service = l_services;
        if (service) HDFramework_finalizeServices(service);
        if (HDFramework_bossIsInitialized()) HDFramework_bossFinalize();
        HDFramework_actFinalize();
        HDFramework_OSBlockThreadsOnExit();
        HDFramework_Exit(-1);
        // HD: retain the binary's fallthrough if the mocked non-returning exit call returns.
        state = exitFallthrough;
    }
    if (oldState != HDFramework_Background) return;
    if (state == HDFramework_Foreground) {
        HDFrameworkRenderer* renderer = l_renderer;
        HDFramework_GX2SetContextState(ea(renderer->mpContextState.get()));
        HDFramework_GX2SetTVEnable(1);
        HDFramework_GX2SetDRCEnable(1);
    }
    HDFramework_endRender(ea(l_renderer.get()));
}

/* 020352AC */
void HDGameFramework::mainLoop() {
    u32 low = l_tickLow;
    u32 high = l_tickHigh;
    u64 ticks = ((u64(high) << 32) | low) * 10;
    auto interval = HDFramework_divideTicks(u32(ticks >> 32), u32(ticks), 0, 1000);
    // HD: preserve the original wait frame rather than adding a guest Local slot.
    u32 entry = cpu->r[1];
    u32 frameAddress = entry - sizeof(HDFrameworkWaitFrame);
    cpu->r[1] = frameAddress;
    HDFrameworkWaitFrame* frame = gabi::at<HDFrameworkWaitFrame>(frameAddress);
    for (;;) {
        HDFramework_beginRender(ea(l_renderer.get()));
        HDFramework_rootBeginFrame(ea(mpRootTask.get()));
        HDFramework_rootEndFrame(ea(mpRootTask.get()));
        HDFramework_endRender(ea(l_renderer.get()));
        if (mpRootTask->mDone || mRunMode != HDFramework_Idle) break;
        frame->mLow = interval.r4;
        frame->mWaitLow = interval.r4;
        frame->mHigh = interval.r3;
        frame->mWaitHigh = interval.r3;
        HDFramework_waitTicks(&frame->mWaitHigh);
    }
    HDFrameworkTask* task = mpTask;
    gabi::call_ptr(task->mpVtable[13], task, 0);
    cpu->r[1] = entry;
}

// HD: compiler-generated header objects and deleting-destructor companions.
/* 02035398 */
static void __sinit_HDGameFramework_cpp() {
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    __register_global_object(0x1018F2F0);
    f32 negative = l_pi.mNegative;
    f32 positive = l_pi.mPositive;
    l_headerStatics.mNegativePi = negative;
    l_headerStatics.mPositivePi = positive;
    HDFramework_firstHeaderObject(&l_headerStatics.mFirstObject);
    __register_global_object(0x1018F2FC);
    HDFramework_secondHeaderObject(&l_headerStatics.mSecondObject);
    __register_global_object(0x1018F308);
}

/* 0203542C */
static void HDFrameworkCompanion__dt(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}

/* 02035440 */
static void HDFrameworkCompanion_empty() {}

/* WWHD glue (tools/verify/autoglue.py):
data l_heapVtable 10004CB0
data l_frameworkVtable 10004E88
data l_callbackVtable 10004DFC
data l_callbackFunction 10004DF8
data l_parameterVtable 10004E54
data l_controllerTaskParentType 10004E34
data l_frameTaskParentType 10004E6C
data l_shutdownTaskParentType 10004E44
data l_controllerTaskTypeInitialized 101FD598
data l_frameTaskTypeInitialized 101FD718
data l_shutdownTaskTypeInitialized 101FD648
data l_controllerTaskType 101FD97C
data l_frameTaskType 101FD990
data l_shutdownTaskType 101FD958
data l_controllerManager 101F8AE8
data l_taskManager 101F8BA8
data l_renderer 101F8B18
data l_frameArena 101F8DC0
data l_frameService 101F86E8
data l_backgroundService 101F5088
data l_hostService 1018F2BC
data l_networkService 101F4FF0
data l_systemService 101F50D8
data l_services 1018F504
data l_tickHigh 104A11C8
data l_tickLow 104A11CC
data l_pi 10004E7C
data l_headerStatics 102008D8
ext 028ED6F8 void HDFramework_firstHeaderObject(be<u8>*)
ext 028EAB2C void HDFramework_secondHeaderObject(be<u8>*)
ext 0273B310 void HDFramework_unlink(void*)
ext 02752BEC void HDFramework_heapBaseDestroy(void*, s32)
ext 0274B1F4 void HDFramework_baseConstruct(void*, u32)
ext 02748CFC void HDFramework_taskArgumentConstruct(void*, void*)
ext 02749ECC HDFrameworkTask* HDFramework_createTask(u32, void*)
ext 0273D6A4 void* HDFramework_createGamepad(u32, u32, u32)
ext 0273D820 u32 HDFramework_createRemoteDevice(u32, u32, u32)
ext 0273D9E4 HDFrameworkController* HDFramework_createRemote(u32, u32, u32)
ext 0273E2A0 u32 HDFramework_createRemoteAddon(u32, u32)
ext 0273E380 u32 HDFramework_createDebugDevice(u32, u32)
ext 0273E4B4 HDFrameworkController* HDFramework_createDebugController(u32, u32)
ext 0273EAC8 u32 HDFramework_createDebugAddon(u32, u32)
ext 0273D2D0 void* HDFramework_createKeyboard(u32, u32, u32, u32)
ext 0273B2F0 void HDFramework_listPushBack(u32, u32)
ext 0274AF44 u32 HDFramework_baseDelegate(u32)
ext 0274A414 void HDFramework_rootBeginFrame(u32)
ext 02799C70 void HDFramework_resetFrameArena(u32)
ext 0272A8C4 void HDFramework_beginFrame(u32)
ext 0272AD80 void HDFramework_endFrame(u32)
ext 0274A5EC void HDFramework_rootEndFrame(u32)
ext 0274C8C4 void HDFramework_baseProcUI(u32)
ext 02617AE4 u32 HDFramework_isBackgroundActive(u32)
ext 02618720 void HDFramework_setBackgroundActive(u32, u32)
ext 0274FBF8 void HDFramework_beginRender(u32)
ext 02032AD8 void HDFramework_setExitRequested(u32, u32)
ext 02032C6C u32 HDFramework_getExitManager()
ext 020330E8 void HDFramework_shutdownHost(u32)
ext 02762F34 void HDFramework_destroyTask(u32)
ext 02614FBC void HDFramework_finalizeNetwork(u32)
ext 02619EEC void HDFramework_finalizeSystem(u32)
ext 0203BCF4 void HDFramework_finalizeServices(u32)
ext 0274FCCC void HDFramework_endRender(u32)
ext 028F5B8C Pair32 HDFramework_divideTicks(u32, u32, u32, u32)
ext 02760C3C void HDFramework_waitTicks(void*)
ext C0009F68 u32 HDFramework_OSSaveDoneReady()
ext C0007868 void HDFramework_ProcUIInit(u32)
ext C0007850 void HDFramework_ProcUIDrawDoneRelease()
ext C0007880 u32 HDFramework_ProcUIProcessMessages(u32)
ext C00078B8 void HDFramework_ProcUIShutdown()
ext C00069D0 void HDFramework_GX2SetTVEnable(u32)
ext C0006808 void HDFramework_GX2SetDRCEnable(u32)
ext C00067E0 void HDFramework_GX2SetContextState(u32)
ext C00053C0 u32 HDFramework_bossIsInitialized()
ext C0005108 void HDFramework_bossFinalize()
ext C00044E8 void HDFramework_actFinalize()
ext C0009998 void HDFramework_OSBlockThreadsOnExit()
ext C000A468 void HDFramework_Exit(s32)
*/
