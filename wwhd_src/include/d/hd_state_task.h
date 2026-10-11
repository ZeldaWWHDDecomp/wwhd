/* HD worker task and sead state/thread views. Private reconstruction. */
#pragma once
#include "wwhd.h"
enum HdStateTaskProgress { HD_TASK_IDLE, HD_TASK_WORKING, HD_TASK_DONE };
enum HdStateTaskMessageKind { HD_TASK_UPDATE = 0x75706474 };
struct HdStateVTableObject { gptr<gfn> mVTable; };
struct HdStateObject { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdStateDisposer { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdStateQueue { u8 mOpaque[0x28]; };
struct HdStateThread { u8 mOpaque[0x1524]; };
struct HdStateMachine {
 gptr<HdStateVTableObject> mFactory; u8 mPad4[4]; gptr<HdStateVTableObject> mCurrent;
 u8 mPadC[4]; gptr<HdStateVTableObject> mNext;
};
struct HdStateDelegate {
 gptr<gfn> mVTable; gptr<void> mObject; be<u16> mThisAdjustment; be<u16> mVirtualOffset; gptr<void> mFunction;
};
struct HdStateTask {
 gptr<gfn> mVTable; gptr<gfn> mObjectVTable; gptr<HdStateTask> mSelf; be<u32> mReserved;
 HdStateMachine mState; gptr<gfn> mTaskVTable; HdStateDisposer mDisposer;
 gptr<void> mHeap; HdStateQueue mQueue; HdStateThread mThread;
 gptr<HdStateVTableObject> mWorker; be<u32> mSharedData; HdStateDelegate mUpdateDelegate;
 gptr<HdStateObject> mObject; be<u32> mProgress; be<u8> mFlag; u8 mPad15A9[3]; be<u32> mBusyCount;
};
struct HdStateMessage {
 gptr<HdStateThread> mThread; be<u32> mKind; be<u16> mFlags; be<u8> mReserved; be<u8> mBlocking;
};
struct HdStateThreadParams {
 be<u32> mStackSize; be<u32> mMessageSize; gptr<HdStateVTableObject> mWorker;
 gptr<be<u32>> mSharedData; gptr<be<u32>> mSharedDataSize; be<u32> mReserved14;
 be<u32> mReserved18; gptr<be<u32>> mWord; be<u32> mPriority; be<u32> mReserved24;
 be<u32> mReserved28; be<u32> mAffinity;
};
struct HdStateStartFrame { HdStateMessage mMessage; be<u32> mSize; be<u32> mWord; HdStateThreadParams mParams; u8 mPad[4]; };
struct HdStateId {
 be<u32> mId; gptr<const char> mName; gptr<gfn> mVTable;
 be<u16> mEnterAdjustment; be<u16> mEnterIndex; be<u32> mEnterOffset;
 be<u16> mExecuteAdjustment; be<u16> mExecuteIndex; be<u32> mExecuteOffset;
 be<u16> mExitAdjustment; be<u16> mExitIndex; be<u32> mExitOffset;
 be<u16> mReserved24; be<u16> mReserved26; be<u32> mReserved28; gptr<void> mFactory;
};
struct HdStateSingleton { gptr<HdStateTask> mInstance; };
struct HdStateDisposerHolder { gptr<HdStateDisposer> mDisposer; };
struct HdStateStatics { be<u32> mWords[4]; };
struct HdStateRange { be<f32> mLower; be<f32> mUpper; be<u8> mSigned; be<u8> mUnsigned; u8 mPad[2]; };
struct HdStateRegistration { be<u32> mWords[3]; };
WWHD_SIZE(HdStateTask,0x15B0);
WWHD_OFFSET(HdStateTask,mState,0x10);
WWHD_OFFSET(HdStateTask,mDisposer,0x28);
WWHD_OFFSET(HdStateTask,mThread,0x64);
WWHD_OFFSET(HdStateTask,mWorker,0x1588);
WWHD_OFFSET(HdStateTask,mUpdateDelegate,0x1590);
WWHD_OFFSET(HdStateTask,mBusyCount,0x15AC);
WWHD_SIZE(HdStateStartFrame,0x48);
WWHD_SIZE(HdStateId,0x30);
WWHD_SIZE(HdStateThreadParams,0x30);
