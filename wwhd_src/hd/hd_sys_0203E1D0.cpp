/** HD ProfileTask, safe-string and delegate companions. No GameCube counterpart.
 * Private decomp source; descriptive companion names replace unknown original symbols. */
#include "bindings.h"
#include "hd/hd_sys_0203E1D0.h"

static hdProfileDownloadedDataArray sDownloadedData;
static hdProfileTask* sProfileTask;
static u32 sProfileTypeInitialized;
static u32 sTaskTypeInitialized;
static hdTaskRuntimeTypeInfo sProfileType;
static hdTaskRuntimeTypeInfo sTaskType;
static void destroyDownloadedData(void*, u32);
static void destroyProfileTask(hdProfileTask*, u32);

/* 0203E1D0 */
static void destroyDownloadedDataArray() {
    hdProfileDestroyArray(&sDownloadedData, 50, 0xC208, destroyDownloadedData, 0, 0);
}

/* 0203E1F8 */
static void destroyFirstCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0203E20C */
static void destroySecondCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0203E220 */
static void safeBufferEmpty() {}

/* 0203E224 */
static void assureSafeBufferTerminated(hdProfileSafeBuffer* i_this) {
    i_this->terminateByte();
}

/* 0203E23C */
static void destroySafeBufferCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0203E250 */
static void wideSafeBufferEmpty() {}

/* 0203E254 */
static void assureWideSafeBufferTerminated(hdProfileSafeBuffer* i_this) {
    i_this->terminateWide();
}

/* 0203E270 */
static void destroyWideSafeBufferCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0203E284 */
static u32 invokeProfileDelegate(hdProfileDelegate* i_this, u32 argument, u32 userData) {
    return i_this->invoke(argument, userData);
}

/* 0203E2D8 */
static void destroyDownloadedData(void* i_this, u32 flags) {
    if (i_this == NULL) {
        return;
    }
    hdProfileDestroyDownloadedData(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0203E32C */
static void destroyProfileInstanceGuard(hdProfileInstanceGuard* i_this, u32 flags) {
    if (i_this == NULL) {
        return;
    }
    if (i_this->mOwnsInstance != 0) {
        sProfileTask = NULL;
    }
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0203E358 */
static void destroyProfileTaskSecondary(void* i_this, u32 flags) {
    // HD: secondary base at +0x14; preserve wrapping guest-pointer subtraction.
    destroyProfileTask(gabi::at<hdProfileTask>(gabi::ea(i_this) - 0x14), flags);
}

/* 0203E360 */
static void claimProfileTaskInstance(hdProfileTask* i_this) {
    if (sProfileTask != NULL) {
        return;
    }
    sProfileTask = i_this;
    i_this->mInstanceGuard.mOwnsInstance = 1;
}

/* 0203E380 */
static hdProfileTask* constructProfileTask(hdProfileTask* i_this, hdTaskConstructArg* argument) {
    hdProfileTask* task = i_this;
    if (task == NULL) {
        task = (hdProfileTask*)operator_new(sizeof(hdProfileTask));
        if (task == NULL) {
            return NULL;
        }
    }
    hdProfileConstructTaskBase(task, argument, STR(0x1000621C));
    task->field_0x70 = 0x10006230;
    task->mTaskVtable = 0x100062E8;
    task->mInstanceGuard.mOwnsInstance = 0;
    return task;
}

/* 0203E400 */
static void calcProfileTask(hdProfileTask* i_this) {
    hdProfileCalcTaskBase(i_this);
}

/* 0203E404 */
static void __sinit_hd_profile_task_cpp() {
    sinit_header_statics_rodata(0x10461230, 0x1018F514, 0x1046123C, 0x10006228);
}

/* 0203E498 */
static void destroyProfileTask(hdProfileTask* i_this, u32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mTaskVtable = 0x100062E8;
    destroyProfileInstanceGuard(&i_this->mInstanceGuard, 2);
    hdProfileDestroyTaskBase(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0203E504 */
static u32 checkProfileTaskRuntimeType(hdProfileTask*, hdTaskRuntimeTypeInfo* type) {
    if (sProfileTypeInitialized == 0) {
        sProfileTypeInitialized = 1;
        sProfileType.mTypeNameAddress = 0x10006204;
    }
    if (type == &sProfileType) {
        return 1;
    }
    if (sTaskTypeInitialized == 0) {
        sTaskTypeInitialized = 1;
        sTaskType.mTypeNameAddress = 0x100061F4;
    }
    return type == &sTaskType;
}

/* 0203E57C */
static void profileTaskFirstEmpty() {}

/* 0203E580 */
static void profileTaskSecondEmpty() {}

/* WWHD glue (tools/verify/autoglue.py): HD task globals and imported calls
 data sDownloadedData 10202CA0
 data sProfileTask 1018F538
 data sProfileTypeInitialized 101FD598
 data sTaskTypeInitialized 101FD594
 data sProfileType 101FD97C
 data sTaskType 101FD980
 ext 028F0164 void hdProfileDestroyArray(void*, u32, u32, hdProfileElementDestructor, u32, u32)
 ext C0004FF0 void hdProfileDestroyDownloadedData(void*, u32)
 ext 027454E0 void hdProfileConstructTaskBase(hdProfileTask*, hdTaskConstructArg*, const char*)
 ext 027452C8 void hdProfileDestroyTaskBase(hdProfileTask*, u32)
 ext 027488E8 void hdProfileCalcTaskBase(hdProfileTask*)
*/
