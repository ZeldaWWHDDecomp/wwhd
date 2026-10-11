/** HD SpotPass manager and task registration. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_net_02038498.h"
static hdNetBossVirtualTable sDisposerTable;
static hdNetBossManager* sNetBossManager;
static hdNetBossManager* sNetBossDisposer;
static u8 sNetBossInitialized;
static u32 sThreadPriority;
static void* sSaveManager;
static hdNetBossString sTaskId;
static hdNetBossStringTable sStringTable;
static char sTaskIdText;

/* 02038498 */
static void destroyNetCompanion(hdNetBossCompanion* i_this, u32 flags) {
    if (i_this == NULL) return;
    i_this->mFirstVtable = 0x10005378;
    i_this->mSecondVtable = 0x10005398;
    i_this->mThirdVtable = 0x10005328;
    hdNetBossDestroyCompanionBase(i_this->mBase, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 02038510 */
static void destroyFirstNetCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02038524 */
static void emptyFirstNetCompanion() {}
/* 02038528 */
static void assureNetStringTermination(hdNetBossFixedString* i_this) { i_this->terminate(); }
/* 02038540 */
static void destroySecondNetCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02038554 */
static void destroyThirdNetCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02038568 */
static void destroyFourthNetCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0203857C */
static hdNetBossFlagObject* constructNetFlagObject(hdNetBossFlagObject* i_this) {
    if (i_this == NULL) {
        i_this = (hdNetBossFlagObject*)operator_new(sizeof(hdNetBossFlagObject));
        if (i_this == NULL) return NULL;
    }
    i_this->mFlag = 0;
    return i_this;
}
/* 020385B8 */
static void destroyNetFlagObject(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020385CC */
static void emptySecondNetCompanion() {}
/* 020385D0 */
static void emptyThirdNetCompanion() {}
/* 020385D4 */
static hdNetBossManager* constructNetBossManager(hdNetBossManager* i_this) {
    if (i_this == NULL) {
        i_this = (hdNetBossManager*)operator_new(sizeof(hdNetBossManager));
        if (i_this == NULL) return NULL;
    }
    i_this->mThread = NULL;
    i_this->mLastResult = 0xFFFFFFFF;
    i_this->mWorkBuffer = NULL;
    i_this->mState = NET_BOSS_IDLE;
    i_this->mHeap = NULL;
    return i_this;
}
/* 02038624 */
static hdNetBossManager* createNetBossInstance(void* heap) {
    hdNetBossManager* current = sNetBossManager;
    if (current != NULL) return current;
    hdNetBossManager* disposer = (hdNetBossManager*)hdNetBossAllocateAligned(sizeof(hdNetBossManager), heap, 4);
    if (disposer != NULL) {
        hdNetBossConstructDisposer(disposer, heap, 3);
        disposer->mDisposerVtable = &sDisposerTable;
    }
    sNetBossDisposer = disposer;
    hdNetBossManager* manager = disposer;
    if (disposer != NULL) manager = constructNetBossManager(disposer);
    sNetBossManager = manager;
    return manager;
}
/* 020386B8 */
static void destroyNetBossManager(hdNetBossManager* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020386CC */
static void deleteNetBossInstance() {
    hdNetBossManager* disposer = sNetBossDisposer;
    if (disposer == NULL) return;
    sNetBossDisposer = NULL;
    disposer->destroyDisposer(2);
    destroyNetBossManager(sNetBossManager, 3);
    sNetBossManager = NULL;
}
/* 02038738 */
static void setNetBossResult(hdNetBossManager* i_this, hdNetBossResult* result, u32 message) {
    (void)message; // HD: diagnostic message is passed but unused by this function.
    i_this->mLastResult = result->mValue;
}
/* 02038744 */
static void initializeNetBoss(hdNetBossManager* i_this, void* parent) {
    hdNetBossThreadName name;
    name.mVtable = 0x100054F0;
    name.mTextAddress = 0x10005528;
    hdNetBossObject* heap = hdNetBossCreateHeap(0x96000, &name, parent, 1, 0);
    i_this->mHeap = heap;
    hdNetBossObject* thread = (hdNetBossObject*)hdNetBossAllocateFromHeap(0x98, heap, 4);
    if (thread != NULL) {
        name.mVtable = 0x100054F0;
        name.mTextAddress = 0x10005518;
        hdNetBossDelegate* delegate = (hdNetBossDelegate*)hdNetBossAllocateFromHeap(sizeof(hdNetBossDelegate), i_this->mHeap, 4);
        if (delegate != NULL) {
            delegate->mObject = i_this;
            delegate->mThisAdjustment = 0;
            delegate->mFunction = 0x02038C7C;
            delegate->mVirtualIndex = -1;
            delegate->mVtable = 0x10005508;
        }
        thread = hdNetBossConstructThread(thread, &name, delegate, i_this->mHeap, sThreadPriority, 0, 0x7FFFFFFF, 0x4C000, 0x20);
    }
    i_this->mThread = thread;
    thread->start();
    i_this->mWorkBuffer = hdNetBossAllocateAligned(0x276, i_this->mHeap, 0x40);
    hdNetBossResult result;
    result.mValue = hdNetBossInitializeLibrary();
    setNetBossResult(i_this, &result, 0x10005534);
    sNetBossInitialized = 1;
}
/* 0203899C */
static void startNetBossThread(hdNetBossManager* i_this) {
    if (i_this->mState != NET_BOSS_IDLE) return;
    hdNetBossObject* thread = i_this->mThread;
    thread->startThread();
    i_this->mState = NET_BOSS_THREAD_STARTED;
}
/* 020389F0 */
static void finalizeNetBoss(hdNetBossManager* i_this) {
    hdNetBossObject* heap = i_this->mHeap;
    heap->destroyHeap();
    i_this->mHeap = NULL;
    i_this->mThread = NULL;
    i_this->mWorkBuffer = NULL;
}
/* 02038A3C */
static u32 requestNetBossRegistration(hdNetBossManager* i_this) {
    if (i_this->mState != NET_BOSS_IDLE) return gabi::ea(i_this);
    hdNetBossObject* thread = i_this->mThread;
    i_this->mState = NET_BOSS_REGISTERING;
    return thread->sendMessage(1, 1);
}
/* 02038A6C */
static u32 getNetBossParameter(hdNetBossManager* i_this, u32 index, hdNetBossSaveParameters* parameters) {
    (void)i_this;
    return hdNetBossGetSaveParameter(parameters, index);
}
/* 02038A74 */
static void fillNetBossTaskSetting(hdNetBossManager* i_this, hdNetBossTaskSetting* setting) {
    hdNetBossSaveParameters* parameters = (hdNetBossSaveParameters*)hdNetBossAllocateFromHeap(sizeof(hdNetBossSaveParameters), i_this->mHeap, 4);
    if (parameters != NULL) parameters = hdNetBossConstructSaveParameters(parameters);
    hdNetBossReadSaveParameters(sSaveManager, parameters);
    // HD: save parameter indices 0..20 map to nn::boss setting keys 1..21.
    for (u32 index = 0; index < 21; ++index) {
        u32 value = getNetBossParameter(i_this, index, parameters);
        hdNetBossSetTaskParameter(setting, index + 1, value);
    }
}
/* 02038B10 */
static void registerNetBossTask(hdNetBossManager* i_this) {
    hdNetBossTaskSetting setting;
    hdNetBossTask task;
    hdNetBossResult result;
    hdNetBossConstructTaskSetting(&setting);
    hdNetBossInitializeTaskSetting(&setting, i_this->mWorkBuffer, 0x276);
    fillNetBossTaskSetting(i_this, &setting);
    result.mValue = 0xFFFFFFFF;
    hdNetBossConstructTask(&task);
    sTaskId.assureTermination();
    u32 value = hdNetBossInitializeTask(&task, sTaskId.mText, 0);
    result.mValue = value;
    setNetBossResult(i_this, &result, 0x10005568);
    if ((result.mValue & 0x80000000u) == 0) {
        bool success = true;
        if (hdNetBossIsTaskRegistered(&task) != 0) {
            value = hdNetBossUnregisterTask(&task);
            result.mValue = value;
            setNetBossResult(i_this, &result, 0x10005584);
            success = (result.mValue & 0x80000000u) == 0;
        }
        if (success) {
            value = hdNetBossRegisterTask(&task, &setting);
            result.mValue = value;
            setNetBossResult(i_this, &result, 0x100055A0);
            if ((result.mValue & 0x80000000u) == 0) {
                value = hdNetBossStartScheduling(&task, 1);
                result.mValue = value;
                setNetBossResult(i_this, &result, 0x10005554);
            }
        }
    }
    hdNetBossDestroyTask(&task, 2);
    hdNetBossDestroyTaskSetting(&setting, 2);
}
/* 02038C7C */
static void processNetBossMessage(hdNetBossManager* i_this, void* thread, u32 message) {
    (void)thread;
    if (message != 1) return;
    registerNetBossTask(i_this);
    i_this->mState = NET_BOSS_IDLE;
}
/* 02038CBC */
static void destroyNetBossDisposer(hdNetBossManager* i_this, u32 flags) {
    if (i_this == NULL) return;
    i_this->mDisposerVtable = &sDisposerTable;
    if (i_this == sNetBossDisposer) {
        hdNetBossManager* manager = sNetBossManager;
        sNetBossDisposer = NULL;
        destroyNetBossManager(manager, 2);
        sNetBossManager = NULL;
    }
    hdNetBossDestroyDisposerBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 02038D58 */
static void __sinit_hd_net_boss_cpp() {
    sinit_header_statics_rodata(0x10200A50, 0x1018F458, 0x10200A5C, 0x100055BC);
    sTaskId.mVtable = &sStringTable;
    sTaskId.mText = (be<u8>*)&sTaskIdText;
}
/* 02038E0C */
static void destroyFinalNetCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02038E20 */
static void emptyFinalNetCompanion() {}
/* WWHD glue (tools/verify/autoglue.py): private NetBossMgr globals and library calls
 data sDisposerTable 100055D0
 data sNetBossManager 1018F47C
 data sNetBossDisposer 1018F480
 data sNetBossInitialized 1018F484
 data sThreadPriority 101F8B9C
 data sSaveManager 101F852C
 data sTaskId 10200A6C
 data sStringTable 100054F0
 data sTaskIdText 100055C4
 ext 02008B4C void hdNetBossDestroyCompanionBase(void*, u32)
 ext 0273B050 void* hdNetBossAllocateFromHeap(u32, void*, u32)
 ext 0273B0D4 void* hdNetBossAllocateAligned(u32, void*, u32)
 ext 02752B0C void hdNetBossConstructDisposer(hdNetBossManager*, void*, u32)
 ext 02752BEC void hdNetBossDestroyDisposerBase(hdNetBossManager*, u32)
 ext 02753004 hdNetBossObject* hdNetBossCreateHeap(u32, hdNetBossThreadName*, void*, u32, u32)
 ext 0275FBD8 hdNetBossObject* hdNetBossConstructThread(hdNetBossObject*, hdNetBossThreadName*, hdNetBossDelegate*, void*, u32, u32, u32, u32, u32)
 ext C0005338 u32 hdNetBossInitializeLibrary()
 ext 02726CE0 u32 hdNetBossGetSaveParameter(hdNetBossSaveParameters*, u32)
 ext 02726A34 hdNetBossSaveParameters* hdNetBossConstructSaveParameters(hdNetBossSaveParameters*)
 ext 02722E88 void hdNetBossReadSaveParameters(void*, hdNetBossSaveParameters*)
 ext C0005608 void hdNetBossSetTaskParameter(hdNetBossTaskSetting*, u32, u32)
 ext C0005728 void hdNetBossConstructTaskSetting(hdNetBossTaskSetting*)
 ext C0005380 void hdNetBossInitializeTaskSetting(hdNetBossTaskSetting*, void*, u32)
 ext C0005750 void hdNetBossConstructTask(hdNetBossTask*)
 ext C0005398 u32 hdNetBossInitializeTask(hdNetBossTask*, const void*, u32)
 ext C00053C8 u32 hdNetBossIsTaskRegistered(hdNetBossTask*)
 ext C0005638 u32 hdNetBossUnregisterTask(hdNetBossTask*)
 ext C0005450 u32 hdNetBossRegisterTask(hdNetBossTask*, hdNetBossTaskSetting*)
 ext C0005610 u32 hdNetBossStartScheduling(hdNetBossTask*, u32)
 ext C0005850 void hdNetBossDestroyTask(hdNetBossTask*, u32)
 ext C0005830 void hdNetBossDestroyTaskSetting(hdNetBossTaskSetting*, u32)
*/
