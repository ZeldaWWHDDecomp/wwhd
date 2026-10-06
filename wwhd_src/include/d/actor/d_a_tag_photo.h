#pragma once
#include "f_op/f_op_actor.h"
struct daTagPhoto_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    be<u32> mMsgID;
    be<u8> mMessageActive; u8 _3b9[3];
    be<u32> mMsgNo;
    u8 mEventCut[0x6c];
    gptr<be<u32>> mpMessageSequence;
    be<s16> mPhotoTalkEventIdx, mPhotoTalk2EventIdx;
    be<u8> mTagNo, mMode, _436;
    be<s8> mActIdx;
    be<u8> mWaitEntered; u8 _439[3];
};
WWHD_SIZE(daTagPhoto_c, 0x43c);
WWHD_OFFSET(daTagPhoto_c, mMsgID, 0x3b4);
WWHD_OFFSET(daTagPhoto_c, mEventCut, 0x3c0);
WWHD_OFFSET(daTagPhoto_c, mpMessageSequence, 0x42c);
WWHD_OFFSET(daTagPhoto_c, mTagNo, 0x434);
