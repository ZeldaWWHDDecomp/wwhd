/** Machine startup. Private decomp source. */
#include "bindings.h"
#include "m_Do/m_Do_machine.h"

static mDoMch_FrameworkSettings sFrameworkSettings;
static u32 sRequestedHeap;
static void* sHeapManager;
static s8 sDevelopmentMode;
static mDoMch_DbPrint* sDbPrint;
static u32 sMainThreadPriority;

/* 025F12F8 */
bool mDoMch_Create() {
    // HD: initialized-setting assertions remain active and may change the flag.
    auto settingAssert = [](u32 line) {
        JUT_ASSERT_fail(STR(0x10058FC4), line, STR(0x10058FDC));
    };
    u8 initialized = sFrameworkSettings.mInitialized;
    if (initialized) {
        settingAssert(0x33);
        initialized = sFrameworkSettings.mInitialized;
    }
    sFrameworkSettings.mMaxStdHeap = 1;
    if (initialized) {
        settingAssert(0x36);
        initialized = sFrameworkSettings.mInitialized;
    }
    sFrameworkSettings.mSysHeapSize = 0x02A22000;
    if (initialized) {
        settingAssert(0x39);
        initialized = sFrameworkSettings.mInitialized;
    }
    sFrameworkSettings.mFifoBufSize = 0xA0000;
    if (initialized) {
        settingAssert(0x3E);
        initialized = sFrameworkSettings.mInitialized;
    }
    sFrameworkSettings.mAramAudioBufSize = 0xA00000;
    if (initialized) {
        settingAssert(0x41);
    }
    sFrameworkSettings.mAramGraphBufSize = 0x5CE000;
    JFWSystem::init();

    u32 previousHeap = 1;
    u32 requestedHeap = sRequestedHeap;
    if (requestedHeap != 0) {
        previousHeap = mDoMch_setThreadHeap(sHeapManager, requestedHeap);
    }
    if (sDevelopmentMode == 0) {
        sDbPrint->mVisible = FALSE;
    }
    // HD: each current-heap getter is retained; its parent supplies the root heap.
    mDoMch_Heap* rootHeap = mDoMch_getCurrentHeap()->mParent;
    mDoMch_Heap* archiveRootHeap = mDoMch_getCurrentHeap()->mParent;
    auto allocationAssert = [](u32 line, const char* message) {
        JUT_ASSERT_fail(STR(0x10058FF0), line, message);
    };
    if (mDoExt_createCommandHeap(0x10000, rootHeap) == NULL) {
        allocationAssert(0xB2, STR(0x10058FD0));
    }
    if (mDoExt_createArchiveHeap(0x90000, archiveRootHeap) == NULL) {
        allocationAssert(0xB7, STR(0x10058FD0));
    }
    // HD: two indexed game heaps replace the GameCube's single game heap.
    if (mDoExt_createGameHeap(0, 0xA00000, rootHeap) == NULL) {
        allocationAssert(0xC4, STR(0x10058FD0));
    }
    if (mDoExt_createGameHeap(1, 0xA00000, rootHeap) == NULL) {
        allocationAssert(0xC8, STR(0x10058FD0));
    }
    mDoMch_Heap* systemHeap = mDoMch_getSystemHeap();
    u32 size = systemHeap->getFreeSize() - 0x10000;
    if ((s32)size <= 0) {
        allocationAssert(0xD4, STR(0x10059004));
    }
    void* zeldaHeap = mDoExt_createZeldaHeap(size, systemHeap);
    if (zeldaHeap == NULL) {
        allocationAssert(0xD6, STR(0x10059010));
    }
    mDoMch_setCurrentHeap(zeldaHeap);
    mDoExt_getZeldaHeap();
    // HD: retained call has no explicit argument; cMl::init receives the preceding return register.
    cMl::init();
    cM_initRnd(100, 100, 100);
    mDoDvdThd::create(sMainThreadPriority - 2);
    if (previousHeap != 1) {
        mDoMch_setThreadHeap(sHeapManager, previousHeap);
    }
    return true;
}

/* 025F15C0 */
static void __sinit_m_Do_machine_cpp() {
    sinit_header_statics_rodata(0x1048D054, 0x101F4874, 0x1048D060, 0x1005902C);
}

/* 025F1654 */
void mDoMch_HeapCheckAll() {
    // HD: the retail build omits the GameCube's recursive heap check.
}

/* WWHD glue (tools/verify/autoglue.py): private startup settings and calls
 data sFrameworkSettings 101F9B04
 data sRequestedHeap 101F9AF8
 data sHeapManager 101F8B4C
 data sDevelopmentMode 101F48C0
 data sDbPrint 101F97D4
 data sMainThreadPriority 101F8B9C
 ext 0280136C void JFWSystem_init()
 ext 02756170 u32 mDoMch_setThreadHeap(void*, u32)
 ext 027EC230 mDoMch_Heap* mDoMch_getCurrentHeap()
 ext 027EC42C mDoMch_Heap* mDoMch_getSystemHeap()
 ext 025E30DC void* mDoExt_createCommandHeap(u32, mDoMch_Heap*)
 ext 025E31B4 void* mDoExt_createArchiveHeap(u32, mDoMch_Heap*)
 ext 025E2E94 void* mDoExt_createGameHeap(u32, u32, mDoMch_Heap*)
 ext 025E3004 void* mDoExt_createZeldaHeap(u32, mDoMch_Heap*)
 ext 027EC220 void mDoMch_setCurrentHeap(void*)
 ext 025EE048 void* mDoExt_getZeldaHeap()
 ext 02019424 void cMl_initFromHeapReturnRegister()
 ext 0201976C void cM_initRnd(s32, s32, s32)
 ext 025E28B0 void mDoDvdThd_create(u32)
*/
