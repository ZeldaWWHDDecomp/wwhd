/** HD SystemTask tail: lifecycle, runtime types and child-task factories.
 * No GameCube counterpart. Private decomp source. */
#include "bindings.h"
#include "hd_sys/hd_sys_0203F8CC_tail.h"

static void* sForwardTarget;
static u32 sSystemTaskTypeInitialized;
static u32 sParentTaskTypeInitialized;
static hdTaskRuntimeTypeInfo sSystemTaskType;
static hdTaskRuntimeTypeInfo sParentTaskType;
enum : u32 {
    SYSTEM_TASK_TYPE_NAME_ADDRESS = 0x100067DC,
    PARENT_TASK_TYPE_NAME_ADDRESS = 0x100067CC,
    TASK_DESTRUCTOR_VTABLE = 0x100068E0,
    ERROR_VIEWER_TASK_SIZE = 0x1AC4,
    GAME_TASK_SIZE = 0xCC,
};

/* 0204007C */
static u32 forwardSystemCall() {
    void* target = sForwardTarget;
    if (target == NULL) {
        return 0;
    }
    return hdForwardSystemCall(target);
}

/* 02040090 */
static void __sinit_hd_system_task_cpp() {
    sinit_header_statics_rodata(0x104612E4, 0x1018F584, 0x104612F0, 0x1000681C);
}

/* 02040124 */
static void destroySystemTaskCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02040138 */
static void destroySystemTask(hdSystemTaskTailView* i_this, u32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mTaskVtable = TASK_DESTRUCTOR_VTABLE;
    hdSystemTaskDestroyInstanceGuard(&i_this->mInstanceGuard, 2);
    hdSystemTaskDestroyBase(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 020401A4 */
static void systemTaskCompanionEmpty() {}

/* 020401A8 */
static u32 checkDerivedRuntimeTypeInfo(void* i_this, hdTaskRuntimeTypeInfo* typeInfo) {
    if (sSystemTaskTypeInitialized == 0) {
        sSystemTaskTypeInitialized = 1;
        sSystemTaskType.mTypeNameAddress = SYSTEM_TASK_TYPE_NAME_ADDRESS;
    }
    if (typeInfo == &sSystemTaskType) {
        return TRUE;
    }
    if (sParentTaskTypeInitialized == 0) {
        sParentTaskTypeInitialized = 1;
        sParentTaskType.mTypeNameAddress = PARENT_TASK_TYPE_NAME_ADDRESS;
    }
    return typeInfo == &sParentTaskType ? TRUE : FALSE;
}

/* 02040220 */
static void* createErrorViewerTask(hdTaskConstructArg* argument) {
    hdTaskHeapArray* heapArray = argument->mHeapArray;
    void* heap = heapArray->mHeaps[heapArray->mCurrentHeapIndex];
    void* task = hdTaskAllocate(ERROR_VIEWER_TASK_SIZE, heap, 4);
    if (task == NULL) {
        return NULL;
    }
    return hdErrorViewerTaskConstruct(task, argument);
}

/* 02040274 */
static void* createGameTask(hdTaskConstructArg* argument) {
    hdTaskHeapArray* heapArray = argument->mHeapArray;
    void* heap = heapArray->mHeaps[heapArray->mCurrentHeapIndex];
    void* task = hdTaskAllocate(GAME_TASK_SIZE, heap, 4);
    if (task == NULL) {
        return NULL;
    }
    return hdGameTaskConstruct(task, argument);
}

/* WWHD glue (tools/verify/autoglue.py): private HD globals and methods
 data sForwardTarget 101F4BD8
 data sSystemTaskTypeInitialized 101FD598
 data sParentTaskTypeInitialized 101FD594
 data sSystemTaskType 101FD97C
 data sParentTaskType 101FD980
 ext 025F9B14 u32 hdForwardSystemCall(void*)
 ext 0203FC44 void hdSystemTaskDestroyInstanceGuard(hdSystemTaskInstanceGuard*, u32)
 ext 027452C8 void hdSystemTaskDestroyBase(hdSystemTaskTailView*, u32)
 ext 0273B050 void* hdTaskAllocate(u32, void*, s32)
 ext 02032DF0 void* hdErrorViewerTaskConstruct(void*, hdTaskConstructArg*)
 ext 020355F8 void* hdGameTaskConstruct(void*, hdTaskConstructArg*)
*/
