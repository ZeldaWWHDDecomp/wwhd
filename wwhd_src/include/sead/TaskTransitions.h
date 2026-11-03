// Task-transition state and pulse-pattern views from local WWHD instructions.
// Reuse the merged method-tree layouts; SEAD#10/17/20 bodies remain call boundaries.
#pragma once
#include "sead/TaskMgr.h"
#include "sead/ChunkResources.h"
namespace sead {
namespace tablet_audio_text { struct Text : SafeText { void destroy(u32 flags); void reserved(); }; }
namespace scale_text { struct Text : SafeText { void destroy(u32 flags); void reserved(); }; }
struct PulsePatternTable { u8 mPrefix[0x2C]; be<u32> mPulse; u8 mReserved[4]; be<u32> mRest; };
struct PulsePattern {
    be<u32> mReservedWords[2]; be<u32> mKind; gptr<void> mOwner;
    gptr<PulsePatternTable> mTable; gptr<const be<u8>> mPattern; be<u32> mCursor; be<u32> mRepeats;
    u32 advance();
};
struct TaskTransitionVtable : TaskBaseVtable {
    VtableSlot mReservedBeforeAdvance;
    VtableSlot mAdvance;
    VtableSlot mChangeState;
};
struct TaskTransition {
    TaskBase mTask;
    be<u32> mOperation;
    gptr<TaskBase> mPreviousTask;
    TaskCreateArg mParameters;
    gptr<TaskBase> mSelectedTask;
    CreateListener mRegistration;
    MethodTreeNode mCalcEntry;
    MethodTreeNode mTransitionNode;
    be<u32> mFrame;
    be<s32> mEnterFrames;
    be<s32> mExitStart;
    be<s32> mExitEnd;
    be<f32> mBlend;
    be<u8> mReady;
    u8 mReadyPadding[3];
    be<u32> mState;
    void detachNodes(); void pauseCalc(u32 pause);
    MethodTreeNode* getNode(u32 phase); void detachCalc(); void markReady();
    u32 selectTask(); void onTaskSelected(TaskBase* task);
    void setState(u32 state); void resetState(); void advanceBlend(); void advanceTransition();
    u32 request(const TaskCreateArg* request);
    void destroy(u32 flags);
};
// The null fader is a TaskTransition with its own tables (SEAD#20's constructor 027684xx..).
struct NullFaderTask : TaskTransition {
    static NullFaderTask* create(NullFaderTask* object,const TaskConstructArg* context);
    void destroy(u32 flags);
};
}
WWHD_SIZE(sead::PulsePattern,0x20);
WWHD_OFFSET(sead::PulsePattern,mTable,0x10);
WWHD_OFFSET(sead::PulsePattern,mCursor,0x18);
WWHD_OFFSET(sead::CreateListener,mState,0x24);
WWHD_OFFSET(sead::TaskTransition,mOperation,0x74);
WWHD_OFFSET(sead::TaskTransition,mSelectedTask,0xFC);
WWHD_OFFSET(sead::TaskTransition,mRegistration,0x100);
WWHD_OFFSET(sead::TaskTransition,mCalcEntry,0x13C);
WWHD_OFFSET(sead::TaskTransition,mTransitionNode,0x190);
WWHD_OFFSET(sead::TaskTransition,mFrame,0x1E4);
WWHD_OFFSET(sead::TaskTransition,mBlend,0x1F4);
WWHD_OFFSET(sead::TaskTransition,mState,0x1FC);
WWHD_SIZE(sead::TaskTransition,0x200);
WWHD_OFFSET(sead::TaskTransitionVtable,mAdvance,0xB0);
WWHD_OFFSET(sead::TaskTransitionVtable,mChangeState,0xB8);
void* allocateTransitionObject(u32 size);
void constructTransitionDisposer(sead::Disposer*);
void removeTransitionTask(sead::TaskMgr*,sead::TaskBase*);
u32 selectTransitionTask(sead::TaskMgr*,const sead::TaskCreateArg*,sead::CreateListener*);
void destroyTransitionTreeNode(void*,u32);

void destroyTransitionTaskBase(void*,u32);
void destroyTransitionOwner(void*,u32);

void taskTransitionSelectedCallback(sead::TaskTransition*,sead::TaskBase*);
void taskTransitionAdvanceCallback(sead::TaskTransition*);
// Read-only transition tables, owned by sead/TaskTransitionStatics.cpp (data only); shared with the SEAD#20 companions (2026-10-25).
extern be<u32>& sTransitionTaskDisposerTable;
extern be<u32>& sTransitionRegistrationTable;
extern be<u32>& sTransitionNodeTable;
extern be<u32>& sTransitionNodeDisposerTable;
