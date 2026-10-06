#pragma once
#include "f_op/f_op_actor.h"
struct MdCbAction { be<s16> delta, index; be<u32> target; };
struct daTag_MdCb_c : fopAc_ac_c {
    MdCbAction mAction; // HD GHS member pointer is eight bytes.
    be<s32> mMessage, mSwitchStart;
    be<u32> mSwitchCount, mCurrentMessage, mFlags;
    be<u16> mSwitch;
    be<s16> mTimer;
    be<s8> mPendingEvent, mEvent, mActionState;
    be<u8> mReserved, mWasInArea, mInArea, mFoundSwitch, mPadding;
    be<s16> mEvents[7], mCounterA, mCounterB, mCounterC, mCounterD;
    u8 padding_3EA[6];
    be<f32> mValue;
    BOOL checkCondition(), checkAreaIn(fopAc_ac_c*), checkTimer(), checkEventFinish();
    BOOL eventProc(), checkCommandTalk(), setAction(MdCbAction*,void*), warpAction(void*), messageAction(void*);
    BOOL init(), execute(), talk(), talk_init();
    s32 getMyStaffId();
    u16 next_msgStatus(be<u32>*);
    void eventEnd(), eventOrder(), action(void*), initialInitEvent(s32), initialMsgSetEvent(s32), initialPlayerOffDrow(s32), initialPlayerOnDrow(s32);
};
WWHD_OFFSET(daTag_MdCb_c,mAction,0x3AC);
WWHD_OFFSET(daTag_MdCb_c,mMessage,0x3B4);
WWHD_OFFSET(daTag_MdCb_c,mTimer,0x3CA);
WWHD_OFFSET(daTag_MdCb_c,mEvents,0x3D4);
WWHD_OFFSET(daTag_MdCb_c,mValue,0x3F0);
