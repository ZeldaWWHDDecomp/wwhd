/* WWHD sead framework, second part of its translation unit (the first is in sead/MethodTree.cpp,
 * SEAD#9): the reset that fires the reset event and finalizes the task manager, the base
 * framework's empty hooks, the run-argument constructors and the TU's text and method-node
 * copies. Reconstructed from this game's instructions. */

#include "sead/Framework.h"
#include "sead/TaskMgr.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void destroyDisposer(sead::Disposer* disposer, u32 flags);

static constexpr u32 kDestroyRelease = 1;

static MethodTreeNodeVtable sFrameworkNodeVtable;
static DisposerVtable sFrameworkNodeDisposerVtable;

/* 02747818 */
void sead::framework::processReset(Framework* framework) {
    // mReady is the reset request and mHeap the reset argument; mObjectList is the reset event
    if (framework->mReady == 0) return;
    void* argument = framework->mHeap;
    Link* end = &framework->mObjectList;
    Link* node = end->mNext;
    Link* next = node->mNext;
    while (node != end) {
        ResetDelegate* delegate = static_cast<ResetEventLink*>(node)->mSlot->mDelegate;
        if (delegate != NULL) gabi::call_ptr<void>(delegate->mVtable->mInvoke.mFunction, delegate, argument);
        node = next;
        next = next->mNext;
    }
    static_cast<TaskMgr*>(framework->mTaskMgr.get())->finalize();
    framework->mHeap = NULL;
    framework->mReady = 0;
}

/* 027478C4 */
FrameworkInitArgs* sead::framework::constructInitArgs(FrameworkInitArgs* arg) {
    if (arg == NULL) arg = static_cast<FrameworkInitArgs*>(allocateObject(sizeof(FrameworkInitArgs)));
    if (arg != NULL) {
        arg->mArena.v = kDefaultFrameworkHeapSize;     // the first word is the heap size
        arg->mRootHeap = NULL;
    }
    return arg;
}

/* 02747908 */
FrameworkRunSettings* sead::framework::constructRunSettings(FrameworkRunSettings* settings) {
    if (settings == NULL) settings = static_cast<FrameworkRunSettings*>(allocateObject(sizeof(FrameworkRunSettings)));
    if (settings != NULL) {
        settings->mStackSize = 0;
        settings->mPriority = kDefaultPriority;
    }
    return settings;
}

/* 0274794C */
void sead::framework_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02747960 */
void sead::framework_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sFrameworkNodeVtable;
    mDisposer.mVtable = &sFrameworkNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027479D4 */
void sead::framework_instances::Text::assureTerminationImpl() {}

/* 027479D8 */
void sead::framework::createSystemTasks(Framework* framework, void* rootTask, void* arg) {}

/* 027479DC */
void sead::framework::processFrameEntry(Framework* framework) {
    gabi::tail_ptr<void>(framework->mVtable->mHeader[kFrameworkProcessFrameSlot].mFunction, framework);
}

/* 027479EC */
void sead::framework::prepareRun(Framework* framework, void* heap) {}

/* 027479F0 */
void sead::framework::runLoop(Framework* framework) {}

/* WWHD glue
 data sFrameworkNodeVtable 101448E4 nocheck
 data sFrameworkNodeDisposerVtable 10144904 nocheck
*/
