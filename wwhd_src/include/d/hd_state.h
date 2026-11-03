/* sead state-machine views shared by the HD singletons and scenes (WWHD layouts): the state machine, its state
 * ids, the singleton disposer and the member-function delegate. Reconstructed from the game's instructions. */
#pragma once
#include "wwhd.h"
struct HdStateVTableObject { gptr<gfn> mVTable; };
struct HdStateObject { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdStateDisposer { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdStateMachine {
 gptr<HdStateVTableObject> mFactory; u8 mPad4[4]; gptr<HdStateVTableObject> mCurrent;
 u8 mPadC[4]; gptr<HdStateVTableObject> mNext;
};
struct HdStateDelegate {
 gptr<gfn> mVTable; gptr<void> mObject; be<u16> mThisAdjustment; be<u16> mVirtualOffset; gptr<void> mFunction;
};
struct HdStateId {
 be<u32> mId; gptr<const char> mName; gptr<gfn> mVTable;
 be<u16> mEnterAdjustment; be<u16> mEnterIndex; be<u32> mEnterOffset;
 be<u16> mExecuteAdjustment; be<u16> mExecuteIndex; be<u32> mExecuteOffset;
 be<u16> mExitAdjustment; be<u16> mExitIndex; be<u32> mExitOffset;
 be<u16> mReserved24; be<u16> mReserved26; be<u32> mReserved28; gptr<void> mFactory;
};
struct HdStateDisposerHolder { gptr<HdStateDisposer> mDisposer; };
/* statics every state-machine translation unit's static initializer sets up */
struct HdStateStatics { be<u32> mWords[4]; };
struct HdStateRange { be<f32> mLower; be<f32> mUpper; be<u8> mSigned; be<u8> mUnsigned; u8 mPad[2]; };
struct HdStateRegistration { be<u32> mWords[3]; };
WWHD_SIZE(HdStateId,0x30);
