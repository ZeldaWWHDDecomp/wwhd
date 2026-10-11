/** HD root-task factory and runtime type check. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_sys_02035444.h"

// HD: these are this task's own and parent type records, initialized lazily.
static u32 sTaskTypeInitialized;
static u32 sParentTypeInitialized;
static hdTaskRuntimeTypeInfo sTaskType;
static hdTaskRuntimeTypeInfo sParentType;
enum : u32 {
    TASK_TYPE_NAME_ADDRESS = 0x10004E24,
    PARENT_TYPE_NAME_ADDRESS = 0x10004E14,
    TASK_SIZE = 0xE8,
};

/* 02035444 */
// HD: accept this task's type record or its parent's type record.
static u32 checkDerivedRuntimeTypeInfo(void* i_this, hdTaskRuntimeTypeInfo* typeInfo) {
    if (sTaskTypeInitialized == 0) {
        sTaskTypeInitialized = 1;
        sTaskType.mTypeNameAddress = TASK_TYPE_NAME_ADDRESS;
    }
    if (typeInfo == &sTaskType) {
        return TRUE;
    }
    if (sParentTypeInitialized == 0) {
        sParentTypeInitialized = 1;
        sParentType.mTypeNameAddress = PARENT_TYPE_NAME_ADDRESS;
    }
    return typeInfo == &sParentType ? TRUE : FALSE;
}

/* 020354BC */
// HD: allocate the task from the construction argument's current heap, aligned to four bytes.
static void* createRootTask(hdTaskConstructArg* argument) {
    hdTaskHeapArray* heapArray = argument->mHeapArray;
    void* heap = heapArray->mHeaps[heapArray->mCurrentHeapIndex];
    void* task = hdTaskAllocate(TASK_SIZE, heap, 4);
    if (task == NULL) {
        return NULL;
    }
    return hdRootTaskConstruct(task, argument);
}

/* 02035510 */
// HD: this translation unit's header statics copy the pi pair from rodata.
static void __sinit_hd_root_task_cpp() {
    sinit_header_statics_rodata(0x102008F4, 0x1018F314, 0x10200900, 0x10004F80);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and local HD calls
 data sTaskTypeInitialized 101FD658
 data sParentTypeInitialized 101FD2E0
 data sTaskType 101FD998
 data sParentType 101FD994
 ext 0273B050 void* hdTaskAllocate(u32, void*, s32)
 ext 0273C734 void* hdRootTaskConstruct(void*, hdTaskConstructArg*)
*/
