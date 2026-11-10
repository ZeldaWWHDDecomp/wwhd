// Task transition helpers and pulse-pattern callbacks from local WWHD instructions.
#include "bindings.h"
#include "sead/TaskTransitions.h"
static sead::scale_text::Text sScaleText;
static f32 sScaleFactor;
static f32 sHalfScaleFactor;
static u32 sScaleTextTable;
static u32 sScaleTextName;
static u32 sPulsePatternTable;

/* 02767024 */
void sead::tablet_audio_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02767038 */
void sead::tablet_audio_text::Text::reserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0276703C */
static void initializeScaleText() {
    gabi::AutoFrame frame{gabi::current_function()};
    sHalfScaleFactor = 4.342944622039795f;
    sScaleFactor = 8.68588924407959f;
    sScaleText.mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sScaleTextTable);
    sScaleText.mText = reinterpret_cast<be<u8>*>(&sScaleTextName);
}
/* 02767080 */
void sead::scale_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02767094 */
void sead::scale_text::Text::reserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 02767588 */
static sead::PulsePattern* createPulsePattern(sead::PulsePattern* pattern,void* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (pattern == NULL) pattern = (sead::PulsePattern*)allocateTransitionObject(0x20);
    if (pattern != NULL) {
        pattern->mOwner = owner;
        pattern->mReservedWords[1] = 0;
        pattern->mReservedWords[0] = 0;
        pattern->mTable = reinterpret_cast<sead::PulsePatternTable*>(&sPulsePatternTable);
        pattern->mCursor = 0;
        pattern->mKind = 2;
        pattern->mRepeats = 0;
        pattern->mPattern = NULL;
    }
    return pattern;
}
/* 027675F8 */
u32 sead::PulsePattern::advance() {
    gabi::AutoFrame frame{gabi::current_function()};
    const be<u8>* pattern = mPattern;
    u32 cursor = mCursor;
    if (pattern != NULL) {
        u32 character = pattern[cursor];
        if (character == 0) {
            u32 repeats = mRepeats;
            mCursor = 0;
            if (repeats == 1) {
                mRepeats = 0;
                mPattern = NULL;
                gabi::call_ptr<void>(mTable->mRest, this);
                return 0;
            }
            if (repeats != 0) mRepeats = repeats - 1;
            pattern = mPattern;
            character = pattern[(u32)mCursor];
        }
        if (character == '*') gabi::call_ptr<void>(mTable->mPulse, this);
        else gabi::call_ptr<void>(mTable->mRest, this);
        mCursor = (u32)mCursor + 1;
    } else if (cursor != 0) {
        gabi::call_ptr<void>(mTable->mRest, this);
        mCursor = 0;
    }
    return 0;
}
/* 02767710 */
static void destroyTransitionNodeDisposer(void* disposer,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyTransitionTreeNode(gabi::at<void>(gabi::ea(disposer) - 0x1C), flags);
}
/* 02767718 */
static void destroyTransitionTaskDisposer(void* disposer,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyTransitionTaskBase(gabi::at<void>(gabi::ea(disposer) - 0x14), flags);
}
/* 02767720 */
static void destroyTransitionOwnerDisposer(void* disposer,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyTransitionOwner(gabi::at<void>(gabi::ea(disposer) - 0x14), flags);
}
static sead::TaskTransitionVtable* transitionTable(sead::TaskBase* task) {
    return reinterpret_cast<sead::TaskTransitionVtable*>(task->mVtable.get());
}
static void setTransitionPause(sead::MethodTreeNode* node,u32 pause) {
    sead::PauseListener* observer = node->mPauseListener;
    if (observer != NULL) gabi::call_ptr<void>(observer->mVtable->mOnPauseChange.mFunction, observer, node, pause);
    node->mPauseFlags = pause;
}
/* 02767B20 */
void sead::TaskTransition::detachNodes() {
    gabi::AutoFrame frame{gabi::current_function()};
    TaskMgr* manager = mTask.mTaskMgr;
    manager->mDestructionNode.pushBackChild(&mTransitionNode);
    mTask.attachToMethodTree(1, &mCalcEntry);
    mCalcEntry.lock();
    setTransitionPause(&mCalcEntry, 0);
    mCalcEntry.unlock();
}
/* 02767BA4 */
void sead::TaskTransition::pauseCalc(u32 pause) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCalcEntry.lock();
    setTransitionPause(&mCalcEntry, pause != 0);
    mCalcEntry.unlock();
}
/* 02767C50 */
sead::MethodTreeNode* sead::TaskTransition::getNode(u32 phase) {
    gabi::AutoFrame frame{gabi::current_function()};
    return phase <= 1 ? &mCalcEntry : NULL;
}
/* 02767C68 */
void sead::TaskTransition::detachCalc() {
    gabi::AutoFrame frame{gabi::current_function()};
    mCalcEntry.detachAll();
}
/* 02767C70 */
void sead::TaskTransition::markReady() {
    gabi::AutoFrame frame{gabi::current_function()};
    mReady = 1;
}
/* 02767C7C */
u32 sead::TaskTransition::selectTask() {
    gabi::AutoFrame frame{gabi::current_function()};
    TaskMgr* manager = mTask.mTaskMgr;
    CriticalSection_l* lock = &manager->mCriticalSection;
    lock->lock();
    if ((u32)mOperation == 1) {
        removeTransitionTask(mTask.mTaskMgr, mPreviousTask);
        mPreviousTask = NULL;
    }
    u32 result = selectTransitionTask(mTask.mTaskMgr, &mParameters, &mRegistration);
    lock->unlock();
    return result;
}
/* 02767CFC */
void sead::TaskTransition::onTaskSelected(TaskBase* task) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSelectedTask = task;
    task->mFlags = (u32)task->mFlags | 1;
    u32 operation = mOperation;
    if (operation <= 1 || operation == 3) markReady();
}
/* 02767D24 */
void sead::TaskTransition::setState(u32 state) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState = state;
    if (state == 0) {
        gabi::call_ptr<void>(transitionTable(&mTask)->mDetachCalc.mFunction, this);
        gabi::call_ptr<void>(transitionTable(&mTask)->mDetachDraw.mFunction, this);
    }
}
/* 02767D80 */
void sead::TaskTransition::resetState() {
    gabi::AutoFrame frame{gabi::current_function()};
    setState(0);
}
struct TransitionConversionScratch { be<u32> mUpper; be<u32> mLower; };
/* 02767D88 */
void sead::TaskTransition::advanceBlend() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<TransitionConversionScratch> scratch(8);
    gabi::FrameLocal<be<u32>> stateValue(16);
    u32 state = mState;
    if (state == 1) {
        s32 duration = mEnterFrames;
        u32 elapsed = mFrame;
        if (duration > 0) {
            scratch->mUpper = 0x43300000;
            scratch->mLower = elapsed ^ 0x80000000;
            f32 numerator = (f32)(s32)elapsed;
            u32 denominator = (u32)duration + 1;
            scratch->mLower = denominator ^ 0x80000000;
            mBlend = gabi::fdivs_ppc(numerator, (f32)(s32)denominator);
        } else mBlend = 0.0f;
        if (elapsed == 0) {
            *stateValue = 2;
            gabi::call_ptr<void>(transitionTable(&mTask)->mChangeState.mFunction, this, stateValue.get());
        }
        gabi::call_ptr<void>(transitionTable(&mTask)->mAdvance.mFunction, this);
        mFrame = (u32)mFrame + 1;
    } else if (state == 2) {
        gabi::call_ptr<void>(transitionTable(&mTask)->mAdvance.mFunction, this);
        u32 elapsed = mFrame;
        mBlend = 1.0f;
        mFrame = elapsed + 1;
    } else if (state == 3) {
        gabi::call_ptr<void>(transitionTable(&mTask)->mAdvance.mFunction, this);
        u32 end = mExitEnd;
        u32 start = mExitStart;
        if (end != start) {
            u32 elapsed = mFrame;
            u32 numerator = elapsed - start;
            u32 denominator = end - start;
            scratch->mUpper = 0x43300000;
            scratch->mLower = numerator ^ 0x80000000;
            f32 value = (f32)(s32)numerator;
            scratch->mLower = denominator ^ 0x80000000;
            mBlend = gabi::fsubs_ppc(1.0f, gabi::fdivs_ppc(value,(f32)(s32)denominator));
        }
        mFrame = (u32)mFrame + 1;
    }
}

static u32 sTransitionTaskTable;
static u32 sTransitionRegistrationDelegateTable;
static u32 sTransitionNodeNameTable;
static u32 sTransitionNodeDefaultName;
static u32 sTransitionDelegateBaseTable;
static u32 sTransitionDelegateTable;
static u32 sTransitionCallbackTable;
static u32 sTransitionNodeName;
static u32 sTransitionContextTable;

template <class T> static T* transitionStorage(T* object,u32 size) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocateTransitionObject(size));
    return object;
}
// GHS scheduled the two inlined copies differently: the calc node (027677C4..) stores mCriticalSection, mVtable,
// mDisposer.mVtable and then mUserData, mPauseListener; the transition node (02767988..) stores mDisposer.mVtable first
// and mPauseListener before mUserData. Only the order of these stores differs (visible when a store faults).
template <bool kTransitionOrder>
static void initializeTransitionNode(sead::MethodTreeNode* node) {
    using namespace sead;
    TreeNode* tree = transitionStorage(&node->mTreeNode,0x14);
    if (tree != NULL) {
        createChunkTreeLink(reinterpret_cast<chunk_resource::TreeLink*>(tree));
        tree->mData = node;
    }
    SafeText* name = transitionStorage(&node->mName,8);
    if (name != NULL) {
        name->mText = reinterpret_cast<be<u8>*>(&sTransitionNodeDefaultName);
        name->mVtable = reinterpret_cast<SafeTextVtable*>(&sTransitionNodeNameTable);
    }
    constructTransitionDisposer(&node->mDisposer);
    if (kTransitionOrder) node->mDisposer.mVtable = reinterpret_cast<DisposerVtable*>(&sTransitionNodeDisposerTable);
    node->mCriticalSection = NULL;
    node->mVtable = reinterpret_cast<MethodTreeNodeVtable*>(&sTransitionNodeTable);
    if (!kTransitionOrder) node->mDisposer.mVtable = reinterpret_cast<DisposerVtable*>(&sTransitionNodeDisposerTable);
    be<u32>* pause = transitionStorage(&node->mPauseFlags,4);
    if (pause != NULL) *pause = 0;
    if (kTransitionOrder) {
        node->mPauseListener = NULL;
        node->mUserData = NULL;
    } else {
        node->mUserData = NULL;
        node->mPauseListener = NULL;
    }
    MethodDelegate* delegate = gabi::at<MethodDelegate>(gabi::ea(node) + 0x2C);
    if (delegate != NULL) {
        delegate->mMethod.mTarget = 0;
        delegate->mVtable = reinterpret_cast<DelegateVtable*>(&sTransitionDelegateBaseTable);
        delegate->mMethod.mObjectAdjustment = 0;
        delegate->mMethod.mVirtualIndex = 0;
        delegate->mObject = NULL;
        delegate->mVtable = reinterpret_cast<DelegateVtable*>(&sTransitionDelegateTable);
    }
    node->mPauseFlags = 0;
}
/* 02767728 */
static sead::TaskTransition* createTaskTransition(sead::TaskTransition* task,const sead::TaskConstructArg* parameters,const be<u8>* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    using namespace sead;
    if (task == NULL) task = (TaskTransition*)allocateTransitionObject(sizeof(TaskTransition));
    if (task == NULL) return task;
    TaskBase::construct(&task->mTask, parameters, name);
    task->mOperation = 5;
    task->mTask.mVtable = reinterpret_cast<TaskBaseVtable*>(&sTransitionTaskTable);
    task->mTask.mDisposer.mVtable = reinterpret_cast<DisposerVtable*>(&sTransitionTaskDisposerTable);
    TaskCreateArg::construct(&task->mParameters);
    task->mSelectedTask = NULL;
    CreateListener* registration = transitionStorage(&task->mRegistration,0x3C);
    if (registration != NULL) {
        constructTransitionDisposer(reinterpret_cast<Disposer*>(registration));
        registration->mLink.mPrevious = NULL;
        registration->mListening = 0;
        registration->mList = NULL;
        reinterpret_cast<Disposer*>(registration)->mVtable = reinterpret_cast<DisposerVtable*>(&sTransitionRegistrationTable);
        registration->mLink.mListener = registration;
        registration->mLink.mNext = NULL;
        MethodDelegate* delegate = reinterpret_cast<MethodDelegate*>(registration->mState);
        if (delegate != NULL) {
            delegate->mObject = reinterpret_cast<u8*>(task);
            delegate->mMethod.mObjectAdjustment = 0;
            delegate->mVtable = reinterpret_cast<DelegateVtable*>(&sTransitionRegistrationDelegateTable);
            delegate->mMethod.mTarget = gabi::guest_fn(reinterpret_cast<const void*>(taskTransitionSelectedCallback));
            delegate->mMethod.mVirtualIndex = -1;
        }
        registration->mDelegate = reinterpret_cast<CreateDelegate*>(delegate);
    }
    initializeTransitionNode<false>(&task->mCalcEntry);
    initializeTransitionNode<true>(&task->mTransitionNode);
    task->mPreviousTask = NULL;
    task->mExitEnd = 61;
    task->mExitStart = 31;
    task->mBlend = 0.0f;
    task->mState = 0;
    task->mFrame = 0;
    task->mEnterFrames = 30;
    task->mReady = 0;
    task->mCalcEntry.mName.mText = const_cast<be<u8>*>(name);
    task->mCalcEntry.lock();
    setTransitionPause(&task->mCalcEntry,3);
    task->mCalcEntry.unlock();
    task->mCalcEntry.lock();
    MethodDelegate* calc = gabi::at<MethodDelegate>(gabi::ea(&task->mCalcEntry) + 0x2C);
    if (calc != NULL) {
        calc->mVtable = reinterpret_cast<DelegateVtable*>(&sTransitionCallbackTable);
        calc->mObject = reinterpret_cast<u8*>(task);
        calc->mMethod.mVirtualIndex = 0x18;
        calc->mMethod.mTarget = 0x70;
        calc->mMethod.mObjectAdjustment = 0;
    }
    task->mCalcEntry.unlock();
    if (name != NULL) task->mCalcEntry.mName.mText = const_cast<be<u8>*>(name);
    task->mTransitionNode.lock();
    MethodDelegate* transition = gabi::at<MethodDelegate>(gabi::ea(&task->mTransitionNode) + 0x2C);
    if (transition != NULL) {
        transition->mVtable = reinterpret_cast<DelegateVtable*>(&sTransitionCallbackTable);
        transition->mObject = reinterpret_cast<u8*>(task);
        transition->mMethod.mVirtualIndex = -1;
        transition->mMethod.mTarget = gabi::guest_fn(reinterpret_cast<const void*>(taskTransitionAdvanceCallback));
        transition->mMethod.mObjectAdjustment = 0;
    }
    task->mTransitionNode.unlock();
    task->mTransitionNode.mName.mText = reinterpret_cast<be<u8>*>(&sTransitionNodeName);
    return task;
}

static void enableTransitionTask(sead::TaskBase* task,u32 value) {
    gabi::call_ptr<void>(transitionTable(task)->mPauseCalc.mFunction, task, value);
}
static void activateTransitionTask(sead::TaskBase* task,u32 value) {
    gabi::call_ptr<void>(transitionTable(task)->mPauseDraw.mFunction, task, value);
}
static void sendTransitionContext(sead::TaskBase* task,sead::TaskEvent* context,u32 kind) {
    // This task supplies an eight-byte context through the inherited event slot.
    context->mKind = kind;
    context->mData = gabi::ea(&sTransitionContextTable);
    gabi::call_ptr<void>(transitionTable(task)->mOnEvent.mFunction, task, context);
}
/* 02767F70 */
void sead::TaskTransition::advanceTransition() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> enterValue(8), leaveValue(12), finishValue(16);
    gabi::FrameLocal<TaskEvent> parentContext(20), previousContext(28), selectedContext(36), readyContext(44);
    u32 state = mState;
    if (state == 1) {
        s32 duration = mEnterFrames;
        if ((s32)(u32)mFrame <= duration) return;
        TaskMgr* manager = mTask.mTaskMgr;
        CriticalSection_l* lock = &manager->mCriticalSection;
        mFrame = (u32)duration + 1;
        if (lock->tryLock() == 0) return;
        *enterValue = 3;
        gabi::call_ptr<void>(transitionTable(&mTask)->mChangeState.mFunction, this, enterValue.get());
        setState(2);
        u32 operation = mOperation;
        if (operation == 1) {
            selectTask();
            lock->unlock();
            return;
        }
        if (operation == 2) {
            enableTransitionTask(mPreviousTask, 1);
            activateTransitionTask(mPreviousTask, 1);
            enableTransitionTask(mSelectedTask, 0);
            activateTransitionTask(mSelectedTask, 0);
            sendTransitionContext(mPreviousTask, previousContext.get(), 1);
            sendTransitionContext(mSelectedTask, selectedContext.get(), 2);
            markReady();
            lock->unlock();
            return;
        }
        if (operation == 3) {
            enableTransitionTask(mPreviousTask, 1);
            activateTransitionTask(mPreviousTask, 1);
            lock->unlock();
            return;
        }
        if (operation == 4) {
            TaskBase* previous = mPreviousTask;
            TreeNode* link = previous->mTreeNode.mParent;
            TaskBase* parent = link != NULL ? static_cast<TaskBase*>(link->mData.get()) : NULL;
            TaskBase* selected = mSelectedTask;
            if (selected == NULL) {
                removeTransitionTask(mTask.mTaskMgr, previous);
            } else {
                removeTransitionTask(mTask.mTaskMgr, previous);
                while (parent != mSelectedTask.get()) {
                    removeTransitionTask(mTask.mTaskMgr, parent);
                    TreeNode* ancestor = parent->mTreeNode.mParent;
                    parent = ancestor != NULL ? static_cast<TaskBase*>(ancestor->mData.get()) : NULL;
                }
            }
            enableTransitionTask(parent, 0);
            activateTransitionTask(parent, 0);
            sendTransitionContext(parent, parentContext.get(), 0);
            markReady();
            lock->unlock();
            return;
        }
        lock->unlock();
    } else if (state == 2) {
        s32 start = mExitStart;
        if ((s32)(u32)mFrame <= start) return;
        bool ready = mReady != 0;
        mFrame = (u32)start + 1;
        if (ready) {
            if ((u32)mOperation == 1) sendTransitionContext(mSelectedTask, readyContext.get(), 4);
            *leaveValue = 0;
            gabi::call_ptr<void>(transitionTable(&mTask)->mChangeState.mFunction, this, leaveValue.get());
            setState(3);
        } else mFrame = (u32)(s32)mExitStart + 1;
    } else if (state == 3) {
        if ((s32)(u32)mFrame <= (s32)mExitEnd) return;
        *finishValue = 1;
        gabi::call_ptr<void>(transitionTable(&mTask)->mChangeState.mFunction, this, finishValue.get());
        mFrame = (u32)(s32)mExitEnd + 1;
        setState(0);
        TaskBase* previous = mPreviousTask;
        if (previous != NULL) {
            previous->mFlags = (u32)previous->mFlags & ~1u;
        }
        TaskBase* selected = mSelectedTask;
        if (selected != NULL) {
            selected->mFlags = (u32)selected->mFlags & ~1u;
        }
    }
}

/* WWHD glue
 data sTransitionTaskTable 1015F530
 data sTransitionRegistrationDelegateTable 1015F4D8
 data sTransitionNodeNameTable 1015F428
 data sTransitionNodeDefaultName 1015F504
 data sTransitionDelegateBaseTable 1015F440
 data sTransitionDelegateTable 1015F498
 data sTransitionCallbackTable 1015F4E8
 data sTransitionNodeName 1015F508
 data sTransitionContextTable 1015F4B0
 data sScaleText 104A1238
 data sScaleFactor 104A1240
 data sHalfScaleFactor 104A1244
 data sScaleTextTable 1015F34C
 data sScaleTextName 1015F3C4
 data sPulsePatternTable 1015F3EC
 ext 0273AD10 void* allocateTransitionObject(u32)
 ext 02748980 sead::TaskBase* sead::TaskBase::construct(sead::TaskBase*,const sead::TaskConstructArg*,const be<u8>*)
 ext 02748AFC sead::TaskCreateArg* sead::TaskCreateArg::construct(sead::TaskCreateArg*)
 ext 02752A84 void constructTransitionDisposer(sead::Disposer*)
 ext 02767CFC void taskTransitionSelectedCallback(sead::TaskTransition*,sead::TaskBase*) callback
 ext 02767F70 void taskTransitionAdvanceCallback(sead::TaskTransition*) callback
 ext 02747A64 void sead::MethodTreeNode::lock()
 ext 02747A74 void sead::MethodTreeNode::unlock()
 ext 02747A84 void sead::MethodTreeNode::detachAll()
 ext 02747AE4 void sead::MethodTreeNode::pushBackChild(sead::MethodTreeNode*)
 ext 027487A4 void sead::TaskBase::attachToMethodTree(u32,sead::MethodTreeNode*)
 ext 027601BC void sead::CriticalSection_l::lock()
 ext 027601C4 u32 sead::CriticalSection_l::tryLock()
 ext 027601F0 void sead::CriticalSection_l::unlock()
 ext 02749AB4 void removeTransitionTask(sead::TaskMgr*,sead::TaskBase*)
 ext 0274905C u32 selectTransitionTask(sead::TaskMgr*,const sead::TaskCreateArg*,sead::CreateListener*)
 ext 027685B0 void destroyTransitionTreeNode(void*,u32)
 ext 027686CC void destroyTransitionTaskBase(void*,u32)
 ext 027687C4 void destroyTransitionOwner(void*,u32)
*/
