/* f_op_msg (GameCube msg_class), HD: the NPC's current message is the message manager reached
 * through the pointer at 0x101F4B5C (fopNpc_npc_c::mpCurrMsg below); its status is read through a
 * call (025F795C, which ignores `this` and returns play+0x5BB2). */
#pragma once
#include "bindings.h"

enum fopMsg_MessageStatus_e {
    fopMsgStts_MSG_CONTINUES_e = 0x0F,
    fopMsgStts_MSG_ENDS_e = 0x10,
    fopMsgStts_BOX_CLOSING_e = 0x11,
    fopMsgStts_BOX_CLOSED_e = 0x12,
};

struct msg_class {
    /* HD: reading mStatus calls 025F795C [v nz] */
    struct Status {
        operator int() const { return gabi::call<int>(0x025F795C, this); }
    };
    union {
        /* 0x000 */ Status mStatus;
        u8 _000[0x938];
    };
    /* 0x938 */ be<u32> mMsgNo;     /* [v nz] */
    /* 0x93C */ u8 _93C[0xC];
    /* 0x948 */ be<u32> mSelectNum; /* [v nz] */
};
WWHD_OFFSET(msg_class, mMsgNo, 0x938);
WWHD_OFFSET(msg_class, mSelectNum, 0x948);
