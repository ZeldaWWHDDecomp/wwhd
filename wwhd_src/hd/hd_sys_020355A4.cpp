/* HD-only GameTask lifecycle, hold counters and per-frame subsystem scheduling. */
#include "hd/hd_sys_020355A4.h"

static HdGameTaskPointer l_currentTask;
static u32 l_taskVtable;
static u32 l_gameVtable;
static u32 l_stringVtable;
static u32 l_rootFactory;
static HdGameTaskPointerValue l_graphics;
static HdGameTaskPointerValue l_audio;
static HdGameTaskPointerValue l_events;
static HdGameTaskPointerValue l_system;
static HdGameTaskPointerValue l_input;
static HdGameTaskPointerValue l_heapManager;
static HdGameTaskPointerValue l_errorViewer;
static HdGameTaskPointerValue l_frameManager;
static HdGameTaskPointerValue l_controller;
static HdGameTaskGameInfoPointer l_gameInfo;
static HdGameTaskEffectOwnerPointer l_effectOwner;
static HdGameTaskHoldCounters l_holdCounters;
static HdGameTaskGlobals l_globals;
static HdGameTaskRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdGameTaskStaticRegistration l_registration;
static HdGameTaskStaticRegistration l_signedRegistration;
static HdGameTaskStaticRegistration l_unsignedRegistration;

enum HdGameTaskConstants : u32 { HdGameTask_Delete = 1, HdGameTask_HoldFrames = 30,
    HdGameTask_MainHeap = 4, HdGameTask_RootKind = 2, HdGameTask_PrepareSlot = 11 };

/* 020355A4 */
static void HdGameTaskRegistration__dt(HdGameTaskRegistration* self, s32 flags) {
    if (self == NULL) return;
    if (self->mRegistered != 0) l_currentTask.mpTask = NULL;
    if (flags & HdGameTask_Delete) hdGameTaskDelete(self);
}

/* 020355D0 */
// HD: adjust the secondary Task base pointer before invoking the deleting destructor.
static void HdGameTask__dtThunk(void* self, s32 flags) {
    HdGameTask* task = gabi::at<HdGameTask>(gabi::ea(self) - 0x14);
    hdGameTaskDestroy(task, flags);
}

/* 020355D8 */
void HdGameTask::registerCurrent() {
    if (l_currentTask.mpTask != NULL) return;
    l_currentTask.mpTask = this;
    mRegistered = 1;
}

/* 020355F8 */
static HdGameTask* HdGameTask__ct(HdGameTask* self, void* param) {
    if (self == NULL) {
        self = static_cast<HdGameTask*>(hdGameTaskAllocate(sizeof(HdGameTask)));
        if (self == NULL) return NULL;
    }
    hdGameTaskConstruct(self, param, "GameTask");
    self->mGameVtable = &l_gameVtable;
    self->mTaskVtable = &l_taskVtable;
    self->mRegistered = 0;
    return self;
}

static HdGameTaskManager* createManager(HdGameTaskSafeString* name, bool system) {
    HdGameTaskHeapManager* manager = hdGameTaskGetHeapManager(HdGameTask_MainHeap);
    name->mVtable = &l_stringVtable;
    name->mpString = gabi::at<const char>(gabi::guest_addr(system ? "system manager" : "event manager"));
    void* factory = system ? hdGameTaskSystemFactory() : hdGameTaskEventFactory();
    void* heap = system ? static_cast<void*>(&manager->mSystemHeap) : static_cast<void*>(&manager->mEventHeap);
    return hdGameTaskCreateManager(heap, name, factory, 1, 0);
}

/* 02035678 */
void HdGameTask::prepare() {
    // HD: preserve the native frame used by the manager SafeString callbacks.
    gabi::NativeFrame<0x38> native;
    HdGameTaskPrepareFrame* frame = gabi::at<HdGameTaskPrepareFrame>(native.a);
    hdGameTaskGraphicsPrepare(0);
    hdGameTaskGraphicsAttach(l_graphics.mpValue, 0);
    hdGameTaskAudioPrepare(0);
    hdGameTaskAudioAttach(l_audio.mpValue);
    hdGameTaskBasePrepare(this);
    void* events = l_events.mpValue;
    if (events == NULL) {
        HdGameTaskManager* manager = createManager(&frame->mEventName, false);
        hdGameTaskCreateEvents();
        gabi::call_ptr(manager->mVtable[HdGameTask_PrepareSlot].get(), manager);
        events = l_events.mpValue;
    }
    if (events != NULL) hdGameTaskPrepareEvents(events);
    if (l_system.mpValue == NULL) {
        HdGameTaskManager* manager = createManager(&frame->mSystemName, true);
        hdGameTaskCreateSystem();
        hdGameTaskPrepareSystem(l_system.mpValue);
        gabi::call_ptr(manager->mVtable[HdGameTask_PrepareSlot].get(), manager);
    }
}

/* 020357CC */
static void hdGameTask_updateHoldCounters() {
    void* state = hdGameTaskGetHoldState(&l_gameInfo.mpGame->mControllerState);
    s32 count = (s32)l_holdCounters.mHome + 1;
    l_holdCounters.mHome = (u32)count;
    if (count >= (s32)HdGameTask_HoldFrames) {
        hdGameTaskHomeHeld(state, 1);
        l_holdCounters.mHome = 0;
    }
    if (hdGameTaskIsSystemHeld(l_input.mpValue) != 0) {
        count = (s32)l_holdCounters.mSystem + 1;
        l_holdCounters.mSystem = (u32)count;
        if (count >= (s32)HdGameTask_HoldFrames) {
            hdGameTaskSystemHeld(state, 1);
            l_holdCounters.mSystem = 0;
        }
    } else l_holdCounters.mSystem = 0;
    if (hdGameTaskIsButtonHeld(l_input.mpValue) != 0) {
        count = (s32)l_holdCounters.mButton + 1;
        l_holdCounters.mButton = (u32)count;
        if (count >= (s32)HdGameTask_HoldFrames) {
            hdGameTaskButtonHeld(state, 1);
            l_holdCounters.mButton = 0;
        }
    } else l_holdCounters.mButton = 0;
    void* controller = hdGameTaskGetController(&l_gameInfo.mpGame->mControllerState);
    u32 active = hdGameTaskIsControllerActive(controller);
    if (active != 0) {
        count = (s32)l_holdCounters.mActive + 1;
        l_holdCounters.mInactive = 0;
        l_holdCounters.mActive = (u32)count;
        if (count >= (s32)HdGameTask_HoldFrames) {
            hdGameTaskControllerActive(state, 1);
            l_holdCounters.mActive = 0;
        }
    } else {
        count = (s32)l_holdCounters.mInactive + 1;
        l_holdCounters.mActive = 0;
        l_holdCounters.mInactive = (u32)count;
        if (count >= (s32)HdGameTask_HoldFrames) {
            hdGameTaskControllerInactive(state, 1);
            l_holdCounters.mInactive = 0;
        }
    }
}

/* 0203593C */
static void hdGameTask_calc() {
    if (hdGameTaskIsErrorVisible(l_errorViewer.mpValue) != 0) return;
    hdGameTask_updateHoldCounters();
    u32 heap = hdGameTaskGetFrameHeap();
    u32 previousHeap = 1;
    if (heap != 0) previousHeap = hdGameTaskChangeHeap(l_heapManager.mpValue, heap);
    hdGameTaskInputCalc(l_input.mpValue);
    void* events = l_events.mpValue;
    if (events != NULL) hdGameTaskEventsCalc(events);
    void* frameManager = l_frameManager.mpValue;
    if (frameManager != NULL) hdGameTaskBeginFrame(frameManager);
    hdGameTaskMatrixCalc();
    void* audio = l_audio.mpValue;
    if (audio != NULL) hdGameTaskAudioCalc(audio);
    hdGameTaskLightingCalc();
    frameManager = l_frameManager.mpValue;
    if (frameManager != NULL) hdGameTaskEndFrame(frameManager);
    HdGameTaskEffect* effect = l_effectOwner.mpOwner->mpEffectPointer->mpEffect;
    if (effect != NULL && gabi::ea(&effect->mEffect) != 0) hdGameTaskEffectCalc(&effect->mEffect);
    hdGameTaskControllerCalc(l_controller.mpValue);
    hdGameTaskInputFinalize(l_input.mpValue);
    hdGameTaskGraphicsCalc(l_graphics.mpValue);
    hdGameTaskSystemCalc(l_system.mpValue);
    if (previousHeap != 1) hdGameTaskChangeHeap(l_heapManager.mpValue, previousHeap);
}

/* 02035A78 */
void HdGameTask::createRoot() {
    gabi::NativeFrame<0xA8> native;
    HdGameTaskRootFrame* frame = gabi::at<HdGameTaskRootFrame>(native.a);
    void* events = l_events.mpValue;
    if (events != NULL) hdGameTaskEventsBeforeRoot(events);
    frame->mFactory.mKind = HdGameTask_RootKind;
    frame->mFactory.mFactory = &l_rootFactory;
    hdGameTaskCreateParams(&frame->mParams, &frame->mFactory);
    hdGameTaskCreateRoot(this, &frame->mParams);
}

/* 02035AE0 */
static void __sinit_hd_game_task_cpp() {
    l_globals.mThird = 0;
    l_globals.mFirst = 0;
    l_globals.mFourth = 0;
    l_globals.mSecond = 0;
    hdGameTaskRegisterStatic(&l_registration);
    f32 lower = l_lower;
    f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdGameTaskInitSigned(&l_range.mSignedInitialized);
    hdGameTaskRegisterStatic(&l_signedRegistration);
    hdGameTaskInitUnsigned(&l_range.mUnsignedInitialized);
    hdGameTaskRegisterStatic(&l_unsignedRegistration);
}

/* 02035B74 */
static void hdGameTask_deleteCompanion(void* self, s32 flags) {
    if (self == NULL) return;
    if (!(flags & HdGameTask_Delete)) return;
    hdGameTaskDelete(self);
}

/* WWHD glue (tools/verify/autoglue.py): HD task framework and subsystem calls.
data l_currentTask 1018F370
data l_taskVtable 100050D8
data l_gameVtable 10005020
data l_stringVtable 10004F8C
data l_rootFactory 02035C74
data l_graphics 101F86B4
data l_audio 101F8294
data l_events 101F4FF0
data l_system 1018F4AC
data l_input 101F5088
data l_heapManager 101F8B4C
data l_errorViewer 1018F2BC
data l_frameManager 101F8A20
data l_controller 101F8378
data l_gameInfo 101F84DC
data l_effectOwner 101F95D0
data l_holdCounters 1018F338
data l_globals 1020091C
data l_range 10200910
data l_lower 10005014
data l_upper 10005018
data l_registration 1018F34C
data l_signedRegistration 1018F358
data l_unsignedRegistration 1018F364
str 10004FCC "GameTask"
str 10004FD8 "event manager" renamed
str 10004FF4 "system manager" renamed
ext 0273AD10 void* hdGameTaskAllocate(u32)
ext 0273AF40 void hdGameTaskDelete(void*)
ext 02035B88 void hdGameTaskDestroy(HdGameTask*, s32)
ext 027454E0 void hdGameTaskConstruct(HdGameTask*, void*, const char*)
ext 0275320C HdGameTaskHeapManager* hdGameTaskGetHeapManager(u32)
ext 0203E8E8 void* hdGameTaskEventFactory()
ext 0203E8C4 void* hdGameTaskSystemFactory()
ext 02753004 HdGameTaskManager* hdGameTaskCreateManager(void*, HdGameTaskSafeString*, void*, u32, u32)
ext 0272879C void hdGameTaskGraphicsPrepare(u32)
ext 027289DC void hdGameTaskGraphicsAttach(void*, u32)
ext 02708910 void hdGameTaskAudioPrepare(u32)
ext 02708634 void hdGameTaskAudioAttach(void*)
ext 027488E8 void hdGameTaskBasePrepare(HdGameTask*)
ext 02614DAC void hdGameTaskCreateEvents()
ext 02614EA8 void hdGameTaskPrepareEvents(void*)
ext 020393E0 void hdGameTaskCreateSystem()
ext 02039480 void hdGameTaskPrepareSystem(void*)
ext 027201DC void* hdGameTaskGetHoldState(void*)
ext 02726D3C void hdGameTaskHomeHeld(void*, u32)
ext 026184AC u32 hdGameTaskIsSystemHeld(void*)
ext 02726D5C void hdGameTaskSystemHeld(void*, u32)
ext 02617AE4 u32 hdGameTaskIsButtonHeld(void*)
ext 02726D7C void hdGameTaskButtonHeld(void*, u32)
ext 027200D0 void* hdGameTaskGetController(void*)
ext 0271FC5C u32 hdGameTaskIsControllerActive(void*)
ext 02726EEC void hdGameTaskControllerActive(void*, u32)
ext 02726ECC void hdGameTaskControllerInactive(void*, u32)
ext 02032FE8 u32 hdGameTaskIsErrorVisible(void*)
ext 025EE048 u32 hdGameTaskGetFrameHeap()
ext 02756170 u32 hdGameTaskChangeHeap(void*, u32)
ext 02617AF4 void hdGameTaskInputCalc(void*)
ext 02614F74 void hdGameTaskEventsCalc(void*)
ext 0273841C void hdGameTaskBeginFrame(void*)
ext 025F172C void hdGameTaskMatrixCalc()
ext 0270870C void hdGameTaskAudioCalc(void*)
ext 0255E854 void hdGameTaskLightingCalc()
ext 02738438 void hdGameTaskEndFrame(void*)
ext 0278FEBC void hdGameTaskEffectCalc(void*)
ext 027199C0 void hdGameTaskControllerCalc(void*)
ext 02618940 void hdGameTaskInputFinalize(void*)
ext 02728A74 void hdGameTaskGraphicsCalc(void*)
ext 02039834 void hdGameTaskSystemCalc(void*)
ext 02616ED0 void hdGameTaskEventsBeforeRoot(void*)
ext 02748B98 void hdGameTaskCreateParams(HdGameTaskCreateParams*, HdGameTaskFactory*)
ext 027487D4 void hdGameTaskCreateRoot(HdGameTask*, HdGameTaskCreateParams*)
ext 028F026C void hdGameTaskRegisterStatic(void*)
ext 028ED6F8 void hdGameTaskInitSigned(be<u8>*)
ext 028EAB2C void hdGameTaskInitUnsigned(be<u8>*)
*/
