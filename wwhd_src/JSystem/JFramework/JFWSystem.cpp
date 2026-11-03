/** JFW startup, using GameCube names and HD heap/thread adapters. */
#include "bindings.h"
#include "JSystem/JFramework/JFWSystem.h"
static u32 sRootHeapInitialized;
static JFWHeap_l* systemHeap;
static u32 sysHeapSize;
static u8 sInitCalled;
static void* sHeapContext;
static JFWHeapManager_l* sHeapManager;
static u32 sSystemHeapHandle;
static void* mainThread;
/* 028012A8 */
static void JFWSystemBase__dt(void* object,u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 028012BC */
static void JFWSystemBase_reserved() {}
/* 028012C0 */
void JFWSystem::firstInit() {
    if (sRootHeapInitialized != 0) JFWSystemAssert("JFWSystem.cpp",86,"rootHeap == NULL");
    u32 size = sysHeapSize;
    // HD: root heap setup belongs to the platform manager; retain its initialized flag.
    sRootHeapInitialized = 1;
    systemHeap = JFWCreateSystemHeap(size,0,0);
    JFWHeap_l* heap = systemHeap;
    if (heap == NULL) {
        JFWSystemAssert("JFWSystem.cpp",92,"systemHeap != NULL");
        heap = systemHeap;
    }
    heap->mName.v = gabi::guest_str("SystemHeap");
    JFWBecomeSystemHeap(systemHeap);
}
/* 0280136C */
void JFWSystem::init() {
    if (sInitCalled != 0) JFWSystemAssert("JFWSystem.cpp",117,"sInitCalled == false");
    if (sRootHeapInitialized == 0) firstInit();
    u32 previous = 1;
    JFWHeap_l* heap = systemHeap;
    if (heap != NULL) previous = JFWBecomeCurrentHeap(sHeapContext,gabi::ea(heap));
    JFWHeapManager_l* manager = sHeapManager;
    sInitCalled = 1;
    sSystemHeapHandle = manager->mSystemHeap;
    mainThread = JFWCreateMainThread(NULL,0);
    if (previous != 1) JFWBecomeCurrentHeap(sHeapContext,previous);
}
/* WWHD glue
 data sRootHeapInitialized 101F9AF4
 data systemHeap 101F9AF8
 data sysHeapSize 101F9B08
 data sInitCalled 101F9B18
 data sHeapContext 101F8B4C
 data sHeapManager 101F8B94
 data sSystemHeapHandle 101F9AFC
 data mainThread 101F9B00
 ext 0273AA24 void JFWSystemAssert(const char*,s32,const char*)
 ext 027EBCB0 JFWHeap_l* JFWCreateSystemHeap(u32,u32,u32)
 ext 027EC20C void* JFWBecomeSystemHeap(void*)
 ext 02756170 u32 JFWBecomeCurrentHeap(void*,u32)
 ext 027ECC0C void* JFWCreateMainThread(void*,u32)
 str 1016F258 "rootHeap == NULL"
 str 1016F268 "JFWSystem.cpp" in 028012C0
 str 1016F278 "systemHeap != NULL"
 str 1016F28C "SystemHeap"
 str 1016F298 "JFWSystem.cpp" in 0280136C
 str 1016F2A8 "sInitCalled == false"
*/
