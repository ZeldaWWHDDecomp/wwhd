/* d_event: dEvt_control_c (the play object's event control), WWHD layout with GameCube names.
 *
 * HD keeps the GameCube layout and size (0xF4: play+0x3F30 .. mEvtManager +0x4024). Verified:
 * mMode +0xC2 [v agbsw0], mGetItemNo +0xD4 [v nz], mEventFlag +0xE8 [v ladder]. */
#pragma once
#include "wwhd.h"

struct dEvt_order_c {
    /* 0x00 */ u8 _00[0x18]; /* mEventType, mFlag, mHindFlag, mActor1/2, mEventId, mPriority, mNextOrderIdx, mEventInfoIdx */
};

struct dEvt_control_c {
    /* 0x00 */ dEvt_order_c mOrder[8];
    /* 0xC0 */ be<s8> mOrderCount;
    /* 0xC1 */ be<s8> mFirstOrderIdx;
    /* 0xC2 */ be<u8> mMode;
    /* 0xC3 */ be<u8> mbEndProc;
    /* 0xC4 */ be<u32> mPt1;
    /* 0xC8 */ be<u32> mPt2;
    /* 0xCC */ be<u32> mPtTalk;
    /* 0xD0 */ be<u32> mPtItem;
    /* 0xD4 */ be<u8> mGetItemNo;
    /* 0xD5 */ u8 field_0xD5;
    /* 0xD6 */ be<u16> mHindFlag;
    /* 0xD8 */ be<s16> mEventId;
    /* 0xDA */ be<u8> mEventEndSound;
    /* 0xDB */ be<u8> field_0xdb;
    /* 0xDC */ be<u8> field_0xdc;
    /* 0xDD */ be<u8> mDebugStb;
    /* 0xDE */ be<u8> field_0xde;
    /* 0xDF */ be<u8> mEventInfoIdx;
    /* 0xE0 */ be<u8> mTalkButton;
    /* 0xE1 */ be<u8> mItemNo;
    /* 0xE2 */ be<u8> mbInPhoto;
    /* 0xE3 */ u8 field_0xE3;
    /* 0xE4 */ be<f32> mCullFarClipRatio;
    /* 0xE8 */ be<u16> mEventFlag;
    /* 0xEA */ be<u8> mTactFreeMStick[5];
    /* 0xEF */ be<u8> mTactFreeCStick[4];
    /* 0xF3 */ u8 _F3;

    BOOL runCheck() { return mMode != 0; }
    void setGtItm(u8 itemNo) { mGetItemNo = itemNo; }
    void onEventFlag(u16 flag) { mEventFlag |= flag; }
    void reset() { onEventFlag(8); }
};
WWHD_OFFSET(dEvt_control_c, mMode, 0xC2);
WWHD_OFFSET(dEvt_control_c, mGetItemNo, 0xD4);
WWHD_OFFSET(dEvt_control_c, mEventFlag, 0xE8);
WWHD_SIZE(dEvt_control_c, 0xF4);
