/* WWHD sead game framework: initialization (the framework's, then the heaps and instances of the
 * system managers: resource manager and file device manager), construction, the system tasks it
 * creates below the root task (the controller manager task and the hooks), the start-up loop that
 * runs the task manager until the root task exists, the draw-context lock and the system
 * singleton; the TU's text copy. Reconstructed from this game's instructions. */

#include "sead/GameFramework.h"
#include "sead/ResourceMgr.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void* allocateArrayFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
u32 setCurrentHeap(sead::RegistryManagerState* manager, sead::RegistryHeap* heap);
void constructHeapDisposer(sead::Disposer* disposer, sead::RegistryHeap* heap, u32 option);
sead::TaskBase* createControllerMgrTask(const sead::TaskConstructArg* arg);
void installControllerMgr(sead::TaskBase* task);
Pair32 divideTicks(u32 high, u32 low, u32 divisorHigh, u32 divisorLow);
void sleepTicks(void* span);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kHeapAlignment = 4;
static constexpr u32 kNoHeapSet = 1;                // setCurrentHeap's "nothing switched"
static constexpr u32 kInstanceDisposerOption = 3;
static constexpr u32 kStartWaitMilliseconds = 10;

/* the timer's 64-bit tick rate per second */
struct TickRate {
    be<u32> mHigh;
    be<u32> mLow;
};
/* a span of timer ticks */
struct TickSpan {
    be<u32> mHigh;
    be<u32> mLow;
};

static FrameworkVtable_l sGameFrameworkVtable;
static SafeTextVtable sGameFrameworkTextVtable;
static DisposerVtable sSystemSingletonVtable;
static SystemSingleton* sSystemSingleton;
static SystemSingleton* sSystemInstanceDisposer;
static u32 sCalcTaskTypeGuard;
static RuntimeType sCalcTaskType;
static u32 sCalcTaskTypeTable;
static PointerBuffer sRootHeaps;                    // the heap manager's root heaps
static Graphics_l* sGraphics;
static TickRate sTickRate;

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

static void lockGraphicsDrawContext(u32 lock);

static u32 maxAllocation(RegistryHeap* heap) {
    return gabi::call_ptr<u32>(heap->dispatch()->mMaxAllocation.mFunction, heap, kHeapAlignment);
}
static void trimToParent(RegistryHeap* heap) {
    gabi::call_ptr<void>(heap->dispatch()->mTrimToParent.mFunction, heap);
}

/* 0274AA74 */
void sead::GameFramework::initialize(FrameworkInitArgs* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    Framework::initialize(arg);
    RegistryHeap* root = NULL;
    if (sRootHeaps.mCount != 0) root = *static_cast<gptr<RegistryHeap>*>(sRootHeaps.mStorage.get());

    // the system managers' heap takes all of the first root heap
    u32 size = maxAllocation(root);
    gabi::FrameLocal<SafeText> systemName(8);
    systemName->mVtable = &sGameFrameworkTextVtable;
    systemName->mText.v = gabi::guest_str("sead::SystemManagers");
    RegistryHeap* systemHeap = FreeRegionHeap::create(size, systemName.get(), root, 1, 0);

    size = maxAllocation(systemHeap);
    gabi::FrameLocal<SafeText> resourceName(0x10);
    resourceName->mVtable = &sGameFrameworkTextVtable;
    resourceName->mText.v = gabi::guest_str("sead::ResourceMgr");
    RegistryHeap* resourceHeap = FreeRegionHeap::create(size, resourceName.get(), systemHeap, 1, 0);
    u32 previous = kNoHeapSet;
    if (resourceHeap != NULL) previous = setCurrentHeap(sRegistryManager.get(), resourceHeap);
    ResourceMgr::createInstance(resourceHeap);
    trimToParent(resourceHeap);
    if (previous != kNoHeapSet) setCurrentHeap(sRegistryManager.get(), gabi::at<RegistryHeap>(previous));

    size = maxAllocation(systemHeap);
    gabi::FrameLocal<SafeText> fileName(0x18);
    fileName->mVtable = &sGameFrameworkTextVtable;
    fileName->mText.v = gabi::guest_str("sead::FileDeviceMgr");
    RegistryHeap* fileHeap = FreeRegionHeap::create(size, fileName.get(), systemHeap, 1, 0);
    previous = kNoHeapSet;
    if (fileHeap != NULL) previous = setCurrentHeap(sRegistryManager.get(), fileHeap);
    FileDeviceManager::createInstance(fileHeap);
    trimToParent(fileHeap);
    if (previous != kNoHeapSet) setCurrentHeap(sRegistryManager.get(), gabi::at<RegistryHeap>(previous));

    trimToParent(systemHeap);
}

/* 0274AC28 */
GameFramework* sead::GameFramework::construct(GameFramework* framework) {
    framework = memberStorage(framework);
    if (framework == NULL) return NULL;
    Framework::construct(framework);
    framework->mVtable = &sGameFrameworkVtable;
    framework->mDisplayRunning = 0;
    for (u32 i = 0; i < 3; i++) {
        SafeText* text = memberStorage(&framework->mTexts[i]);
        if (text != NULL) {
            text->mText.v = gabi::guest_str("");
            text->mVtable = &sGameFrameworkTextVtable;
        }
    }
    framework->mReserved4C = 0;
    framework->mReserved44 = 0;
    framework->mDrawContextLock = gabi::guest_fn(reinterpret_cast<const void*>(&lockGraphicsDrawContext));
    return framework;
}

/* 0274AD68 */
void sead::GameFramework::startDisplay() {
    if (mDisplayRunning == 0) mDisplayRunning = 1;
}

/* 0274AD80 */
void sead::GameFramework::createSystemTasks(TaskBase* rootTask, SystemTaskSettings* settings) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SystemTaskArg> arg(8);
    arg->mWords[2] = 0;
    arg->mWords[0] = 0;
    arg->mWords[3] = 0;
    framework::createSystemTasks(this, rootTask, arg.get());
    gabi::call_ptr<void>(vtable()->mCreateControllerTask.mFunction, this, rootTask);
    gabi::call_ptr<void>(vtable()->mCreateSystemSingleton.mFunction, this, rootTask);
    gabi::call_ptr<void>(vtable()->mCreateMenuTask.mFunction, this, rootTask);
    u32 flags = settings->mWatchdogFlags;
    gabi::call_ptr<void>(vtable()->mCreateWatchdogTask.mFunction, this, rootTask, flags);
    gabi::call_ptr<void>(vtable()->mCreateClockTask.mFunction, this, rootTask, &settings->mClockSettings);
}

/* 0274AE58 */
void sead::GameFramework::createControllerTask(TaskBase* rootTask) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<TaskClassID> classID(8);
    classID->mType = TaskClassID::cTag_Factory;
    classID->mValue = gabi::guest_fn(reinterpret_cast<const void*>(&createControllerMgrTask));
    gabi::FrameLocal<TaskCreateArg> createArg(0x10);
    TaskCreateArg::constructSystem(createArg.get(), classID.get());
    createArg->mParent = rootTask;
    gabi::FrameLocal<TaskCreateArg> request(0x90);
    const be<u32>* from = reinterpret_cast<const be<u32>*>(createArg.get());
    be<u32>* to = reinterpret_cast<be<u32>*>(request.get());
    for (u32 i = 0; i < sizeof(TaskCreateArg) / 4; i++) to[i] = from[i];
    request->mCreateCallback = gabi::guest_fn(reinterpret_cast<const void*>(&installControllerMgr));
    TaskBase* task = taskMgr()->createTaskSync(request.get());
    // a runtime-type check of the new task against the calc task, whose result is unused
    if (sCalcTaskTypeGuard == 0) {
        sCalcTaskTypeGuard = 1;
        sCalcTaskType.mTable = &sCalcTaskTypeTable;
    }
    if (task != NULL) gabi::call_ptr<u32>(task->mVtable->mIsKindOf.mFunction, task, &sCalcTaskType);
}

/* 0274AF40 */
void sead::GameFramework::createWatchdogTask(TaskBase* rootTask, u32 flags) {}

/* 0274AF44 */
SystemSingleton* sead::GameFramework::createSystemSingleton(TaskBase* rootTask) {
    return SystemSingleton::createInstance(rootTask->mHeapArray.primaryHeap());
}

/* 0274AF58 */
void sead::GameFramework::createMenuTask(TaskBase* rootTask) {}

/* 0274AF5C */
void sead::GameFramework::createClockTask(TaskBase* rootTask, void* clockSettings) {}

/* 0274AF60 */
void sead::GameFramework::waitStartDisplayLoop() {
    gabi::AutoFrame frame{gabi::current_function()};
    // wait in steps of 10 ms: the tick rate times 10, divided by 1000
    u64 rate = ((u64)sTickRate.mHigh << 32) | sTickRate.mLow;
    u64 scaled = rate * kStartWaitMilliseconds;
    auto step = divideTicks((u32)(scaled >> 32), (u32)scaled, 0, 1000);
    gabi::FrameLocal<TickSpan> stepSpan(8);
    gabi::FrameLocal<TickSpan> waitSpan(0x10);
    while (true) {
        sGraphics->lockDrawContext();
        taskMgr()->beginFrame();
        taskMgr()->endFrame();
        sGraphics->unlockDrawContext();
        if (taskMgr()->mRootTask != NULL || mDisplayRunning != 0) break;
        stepSpan->mLow = step.r4;
        waitSpan->mLow = step.r4;
        stepSpan->mHigh = step.r3;
        waitSpan->mHigh = step.r3;
        sleepTicks(waitSpan.get());
    }
    MethodTreeMgrBase* manager = reinterpret_cast<MethodTreeMgrBase*>(mMethodTreeMgr.get());
    gabi::call_ptr<void>(manager->mVtable->mPauseAll.mFunction, manager, 0);
    startDisplay();
}

/* 0274B054 */
void sead::GameFramework::lockFrameDrawContext() {
    u32 lock = mDrawContextLock;
    if (lock != 0) gabi::tail_ptr<void>(lock, 1);
}

/* 0274B06C */
void sead::GameFramework::unlockFrameDrawContext() {
    u32 lock = mDrawContextLock;
    if (lock != 0) gabi::tail_ptr<void>(lock, 0);
}

/* 0274B084 */
static void lockGraphicsDrawContext(u32 lock) {
    Graphics_l* graphics = sGraphics;
    if (lock != 0) graphics->lockDrawContext();
    else graphics->unlockDrawContext();
}

/* 0274B09C */
void sead::game_framework_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274B0B0 */
void sead::game_framework_instances::Text::assureTerminationImpl() {}

/* 0274B0B4 */
void sead::GameFramework::processFrameEntry() {
    gabi::tail_ptr<void>(mVtable->mHeader[kFrameworkProcessFrameSlot].mFunction, this);
}

/* 0274B0C4 */
void sead::GameFramework::prepareRun(void* heap) {}

/* 0274B0C8 */
void sead::GameFramework::runLoop() {}

/* 0274B0CC */
SystemSingleton* sead::SystemSingleton::createInstance(RegistryHeap* heap) {
    SystemSingleton* instance = sSystemSingleton;
    if (instance != NULL) return instance;
    instance = static_cast<SystemSingleton*>(allocateArrayFromHeap(sizeof(SystemSingleton), heap, kHeapAlignment));
    if (instance != NULL) {
        constructHeapDisposer(instance, heap, kInstanceDisposerOption);
        instance->mVtable = &sSystemSingletonVtable;
    }
    sSystemInstanceDisposer = instance;
    sSystemSingleton = instance;
    return instance;
}

/* 0274B158 */
void sead::SystemSingleton::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sSystemSingletonVtable;
    if (this == sSystemInstanceDisposer) {
        sSystemInstanceDisposer = NULL;
        sSystemSingleton = NULL;
    }
    destroyDisposer(this, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 ext 027475F4 void sead::Framework::initialize(sead::FrameworkInitArgs*)
 ext 02747558 sead::Framework* sead::Framework::construct(sead::Framework*)
 ext 02741A20 sead::FileDeviceManager* sead::FileDeviceManager::createInstance(void*)
 ext 0275D9A0 sead::ResourceMgr* sead::ResourceMgr::createInstance(sead::RegistryHeap*)
 ext 02752B0C void constructHeapDisposer(sead::Disposer*, sead::RegistryHeap*, u32)
 ext 020354BC sead::TaskBase* createControllerMgrTask(const sead::TaskConstructArg*) callback
 ext 0273C714 void installControllerMgr(sead::TaskBase*) callback
 ext 0274FBF8 void sead::Graphics_l::lockDrawContext()
 ext 0274FCCC void sead::Graphics_l::unlockDrawContext()
 ext 028F5B8C Pair32 divideTicks(u32, u32, u32, u32)
 ext 02760C3C void sleepTicks(void*)
 data sGameFrameworkVtable 10144E30 nocheck
 data sGameFrameworkTextVtable 10144DB8 nocheck
 data sSystemSingletonVtable 10144EE8 nocheck
 data sSystemSingleton 101F8B10
 data sSystemInstanceDisposer 101F8B14
 data sCalcTaskTypeGuard 101FD598
 data sCalcTaskType 101FD97C
 data sCalcTaskTypeTable 10144DE0
 data sRootHeaps 104A030C nocheck
 data sGraphics 101F8B18
 data sTickRate 104A11C8 nocheck
 str 10144E18 "sead::SystemManagers"
 str 10144E04 "sead::ResourceMgr"
 str 10144DF0 "sead::FileDeviceMgr"
 str 10144DB4 "" in 0274AC28
*/
