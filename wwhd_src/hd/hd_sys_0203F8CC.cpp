/** HD SystemTask construction, startup and companion factories. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_sys_0203F8CC.h"
static u32 sFirstTypeInitialized;
static u32 sFirstParentInitialized;
static u32 sSecondTypeInitialized;
static u32 sSecondParentInitialized;
static u32 sSystemTypeInitialized;
static hdTaskRuntimeTypeInfo sFirstType;
static hdTaskRuntimeTypeInfo sFirstParent;
static hdTaskRuntimeTypeInfo sSecondType;
static hdTaskRuntimeTypeInfo sSecondParent;
static hdTaskRuntimeTypeInfo sSystemType;
static hdSystemStartupTask* sSystemInstance;
static hdSystemRootHeapHolder* sRootHeapHolder;
static void* sFontManager;
static void* sMessageManager;
static void* sMessageGroupManager;
static void* sBackgroundManager;
static u32 sDebugDrawEnabled;

/* 0203F8CC */
static void destroyFirstCompanion(hdSystemDestroyableA* i_this, u32 flags) {
    if (i_this == NULL) return;
    i_this->mVtable = 0x1015E028;
    hdSystemDestroyFirstBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 0203F92C */
static void destroySecondCompanion(hdSystemDestroyableB* i_this, u32 flags) {
    if (i_this == NULL) return;
    i_this->mVtable = 0x100067A0;
    hdSystemDestroySecondBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 0203F98C */
static void emptyFirstCompanion() {}
/* 0203F990 */
static void assureSystemStringTermination(hdSystemFixedString* i_this) { i_this->terminate(); }
/* 0203F9A8 */
static void destroyThirdCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0203F9BC */
static void destroyFourthCompanion(void* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0203F9D0 */
static u32 isFirstTaskDerived(void* i_this, hdTaskRuntimeTypeInfo* type) {
    (void)i_this;
    if (sFirstTypeInitialized == 0) {
        sFirstTypeInitialized = 1;
        sFirstType.mTypeNameAddress = 0x10006448;
    }
    if (type == &sFirstType) return 1;
    if (sFirstParentInitialized == 0) {
        sFirstParentInitialized = 1;
        sFirstParent.mTypeNameAddress = 0x10006428;
    }
    return type == &sFirstParent;
}
/* 0203FA48 */
static u32 isSecondTaskDerived(void* i_this, hdTaskRuntimeTypeInfo* type) {
    (void)i_this;
    if (sSecondTypeInitialized == 0) {
        sSecondTypeInitialized = 1;
        sSecondType.mTypeNameAddress = 0x10006438;
    }
    if (type == &sSecondType) return 1;
    if (sSecondParentInitialized == 0) {
        sSecondParentInitialized = 1;
        sSecondParent.mTypeNameAddress = 0x10006428;
    }
    return type == &sSecondParent;
}
/* 0203FAC0 */
static void* createSmallSystemObject(u32 unused, void* heap, s32 alignment) {
    (void)unused;
    void* object = hdSystemAllocate(0x44, heap, alignment);
    if (object == NULL) return NULL;
    return hdSystemConstructSmallObject(object);
}
/* 0203FAF0 */
static void emptySecondCompanion() {}
static void* allocateSystemTask(hdTaskConstructArg* argument, u32 size) {
    hdTaskHeapArray* heaps = argument->mHeapArray;
    void* heap = heaps->mHeaps[heaps->mCurrentHeapIndex];
    return hdSystemAllocate(size, heap, 4);
}
/* 0203FAF4 */
static void* createSystemDelegateTask(hdTaskConstructArg* argument) {
    void* task = allocateSystemTask(argument, 0xD0);
    if (task == NULL) return NULL;
    return hdSystemConstructDelegateTask(task, argument);
}
/* 0203FB48 */
static void* createLargeSystemTask(hdTaskConstructArg* argument) {
    void* task = allocateSystemTask(argument, 0x16AC);
    if (task == NULL) return NULL;
    return hdSystemConstructLargeTask(task, argument);
}
static hdSystemStartupTask* constructSystemTask(hdSystemStartupTask*, hdTaskConstructArg*);
/* 0203FB9C */
static hdSystemStartupTask* createSystemTask(hdTaskConstructArg* argument) {
    hdSystemStartupTask* task = (hdSystemStartupTask*)allocateSystemTask(argument, sizeof(hdSystemStartupTask));
    if (task == NULL) return NULL;
    return constructSystemTask(task, argument);
}
/* 0203FBF0 */
static void* createSystemProfileTask(hdTaskConstructArg* argument) {
    void* task = allocateSystemTask(argument, 0xCC);
    if (task == NULL) return NULL;
    return hdSystemConstructProfileTask(task, argument);
}
/* 0203FC44 */
static void destroySystemInstanceGuard(hdSystemInstanceGuard* i_this, u32 flags) {
    if (i_this == NULL) return;
    if (i_this->mOwnsInstance != 0) sSystemInstance = NULL;
    if (flags & 1) operator_delete(i_this);
}
/* 0203FC70 */
static void destroySystemTaskSecondary(void* base, u32 flags) {
    // HD: secondary-base adjustment wraps in the guest address space.
    hdSystemDestroyTask(gabi::at<hdSystemStartupTask>(gabi::ea(base) - 0x14), flags);
}
/* 0203FC78 */
static void claimSystemInstance(hdSystemStartupTask* i_this) {
    if (sSystemInstance != NULL) return;
    sSystemInstance = i_this;
    i_this->mOwnsInstance = 1;
}
/* 0203FC98 */
static hdSystemStartupTask* constructSystemTask(hdSystemStartupTask* i_this, hdTaskConstructArg* argument) {
    if (i_this == NULL) {
        i_this = (hdSystemStartupTask*)operator_new(sizeof(hdSystemStartupTask));
        if (i_this == NULL) return NULL;
    }
    hdSystemConstructTaskBase(i_this, argument, 0x100067F4);
    i_this->mRuntimeVtable = 0x10006828;
    i_this->mTaskVtable = 0x100068E0;
    i_this->mOwnsInstance = 0;
    return i_this;
}
/* 0203FD18 */
static void prepareSystemTask(hdSystemStartupTask* i_this) {
    hdSystemInitializeFontHeap(sRootHeapHolder->mHeap);
    hdSystemInitializeFontManager(sFontManager);
    hdSystemInitializeMessageHeap(sRootHeapHolder->mHeap);
    hdSystemInitializeMessageManager(sMessageManager);
    hdSystemHeapName messageName;
    messageName.mVtable = 0x100067B4;
    messageName.mTextAddress = 0x10006800;
    void* parent = hdSystemGetRootHeap();
    void* messageHeap = hdSystemCreateExpandedHeap(0x1000, &messageName, parent, 1, 0);
    hdSystemInitializeMessageGroupHeap(messageHeap);
    hdSystemAssignMessageGroupHeap(sMessageGroupManager, messageHeap);
    hdSystemHeapName backgroundName;
    backgroundName.mVtable = 0x100067B4;
    backgroundName.mTextAddress = 0x10006808;
    parent = hdSystemGetRootHeap();
    void* backgroundHeap = hdSystemCreateExpandedHeap(0x300000, &backgroundName, parent, 1, 0);
    hdSystemInitializeBackgroundHeap(backgroundHeap);
    hdSystemAssignBackgroundHeap(sBackgroundManager, backgroundHeap);
    hdSystemInitializeMain();
    hdSystemPrepareBase(i_this);
}
/* inlined into enterSystemTask */
static void createSystemChild(hdSystemStartupTask* i_this, u32 factory, u32 callback, bool setHeapSize) {
    hdSystemChildTaskId id;
    hdSystemChildTaskArgument source;
    hdSystemChildTaskArgument argument;
    id.mKind = 2;
    id.mFactory = factory;
    hdSystemConstructChildArgument(&source, &id);
    if (setHeapSize) source.mHeapOptions[source.currentHeapIndex()].mSize = 0x2800;
    argument.copyFrom(source);
    argument.mCreateCallback = callback;
    hdSystemCreatedTask* task = hdSystemCreateChildTask(i_this->mTaskManager, &argument);
    if (sSystemTypeInitialized == 0) {
        sSystemType.mTypeNameAddress = 0x100067DC;
        sSystemTypeInitialized = 1;
    }
    if (task != NULL) task->mVtable->check(task, &sSystemType);
}
/* 0203FE20 */
static void enterSystemTask(hdSystemStartupTask* i_this) {
    createSystemChild(i_this, 0x02040220, 0x02032DD0, true);
    createSystemChild(i_this, 0x02040274, 0x020355D8, false);
    if (sDebugDrawEnabled == 0) return;
    hdSystemViewport viewport;
    hdSystemViewport screen;
    hdSystemViewport camera;
    hdSystemConstructViewport(&viewport);
    hdSystemSetViewportRectangle(&screen, 0.0f, 0.0f, 1.0f, 1.0f);
    hdSystemViewportObject* object = hdSystemGetViewportObject(i_this);
    object->mVtable->setViewport(object);
    hdSystemSetViewportRectangle(&screen, 0.0f, 0.0f, 1.0f, 1.0f);
    hdSystemSetViewportRectangle(&camera, 0.0f, 0.0f, 1.0f, 1.0f);
}
/* WWHD glue (tools/verify/autoglue.py): private startup globals and calls
 data sFirstTypeInitialized 101FD6D0
 data sFirstParentInitialized 101FD594
 data sSecondTypeInitialized 101FD720
 data sSecondParentInitialized 101FD2E0
 data sSystemTypeInitialized 101FD598
 data sFirstType 101FD9DC
 data sFirstParent 101FD980
 data sSecondType 101FD9E0
 data sSecondParent 101FD994
 data sSystemType 101FD97C
 data sSystemInstance 1018F5A8
 data sRootHeapHolder 1018C404
 data sFontManager 101F8378
 data sMessageManager 101F4BAC
 data sMessageGroupManager 101F4B5C
 data sBackgroundManager 101F4BD8
 data sDebugDrawEnabled 101F8B10
 ext 0275CFD0 void hdSystemDestroyFirstBase(hdSystemDestroyableA*, u32)
 ext 02747D40 void hdSystemDestroySecondBase(hdSystemDestroyableB*, u32)
 ext 0273B050 void* hdSystemAllocate(u32, void*, s32)
 ext 0275E11C void* hdSystemConstructSmallObject(void*)
 ext 02761004 void* hdSystemConstructDelegateTask(void*, hdTaskConstructArg*)
 ext 027294A8 void* hdSystemConstructLargeTask(void*, hdTaskConstructArg*)
 ext 0203E380 void* hdSystemConstructProfileTask(void*, hdTaskConstructArg*)
 ext 02040138 void hdSystemDestroyTask(hdSystemStartupTask*, u32)
 ext 027454E0 void hdSystemConstructTaskBase(hdSystemStartupTask*, hdTaskConstructArg*, u32)
 ext 0271AFA8 void hdSystemInitializeFontHeap(void*)
 ext 0271B200 void hdSystemInitializeFontManager(void*)
 ext 025F93B4 void hdSystemInitializeMessageHeap(void*)
 ext 025F9448 void hdSystemInitializeMessageManager(void*)
 ext 0203E8E8 void* hdSystemGetRootHeap()
 ext 02753004 void* hdSystemCreateExpandedHeap(u32, hdSystemHeapName*, void*, u32, u32)
 ext 025F7658 void hdSystemInitializeMessageGroupHeap(void*)
 ext 025F90C4 void hdSystemAssignMessageGroupHeap(void*, void*)
 ext 025F9818 void hdSystemInitializeBackgroundHeap(void*)
 ext 025F9A90 void hdSystemAssignBackgroundHeap(void*, void*)
 ext 025F1660 void hdSystemInitializeMain()
 ext 027488E8 void hdSystemPrepareBase(hdSystemStartupTask*)
 ext 02748B98 void hdSystemConstructChildArgument(hdSystemChildTaskArgument*, hdSystemChildTaskId*)
 ext 02749ECC hdSystemCreatedTask* hdSystemCreateChildTask(void*, hdSystemChildTaskArgument*)
 ext 0274F358 void hdSystemConstructViewport(hdSystemViewport*)
 ext 0274F424 void hdSystemSetViewportRectangle(hdSystemViewport*, f32, f32, f32, f32)
 ext 02748974 hdSystemViewportObject* hdSystemGetViewportObject(hdSystemStartupTask*)
*/
