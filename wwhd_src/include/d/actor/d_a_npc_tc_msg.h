#pragma once
#include "d/actor/d_a_npc_tc.h"

namespace tc_message_detail {
// HD: message selection lives in the shared HD manager instead of mpCurrMsg.
struct MessageManager {
    u8 padding[0x948];
    be<s32> mSelectNum;
};
WWHD_OFFSET(MessageManager, mSelectNum, 0x948);
WWHD_SIZE(MessageManager, 0x94C);

// daObjSmplbg::Act_c's stop flag (GameCube mIsStop).
struct Tower {
    u8 padding[0x3F0];
    be<u8> mIsStop;
    void onStop() {
        // HD: the assert returns; a NULL tower still writes guest address +0x3F0.
        *gabi::at<be<u8>>(gabi::ea(this) + offsetof(Tower, mIsStop)) = 1;
    }
};
WWHD_OFFSET(Tower, mIsStop, 0x3F0);
WWHD_SIZE(Tower, 0x3F1);
} // namespace tc_message_detail
