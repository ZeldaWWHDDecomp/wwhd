/**
 * d_a_npc_photo_msg.cpp (WWHD)
 * NPC - Lenzo: messages (getMsg, next_msgStatus) and the picture checks.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_photo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_photo.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 photo_album() { return gabi::call<u32>(0x02720144, dComIfGs_save() + 0x12C0); }
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24); }
/* 025B5C4C dSv_player_item_c::setEquipBottleItemEmpty (save + 0x5C) */
static inline void dComIfGs_setEquipBottleItemEmpty() { gabi::call(0x025B5C4C, dComIfGs_save() + 0x5C); }
static inline u8 dComIfGp_event_chkPhoto() { return gabi::load<u8>(dComIfGp_ea() + 0x52B2); }
static inline be<u32>* MSG(u32 a) { return gabi::at<be<u32>>(a); }

/* 022CFE34 */
u16 daNpcPhoto_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022CFE34, u16, this, pMsgNo);
    u32 mgr = photo_msgManager(); /* HD: mpCurrMsg is the message manager */
    u32 msgNo = *pMsgNo;
    if (msgNo == 0x2A3A || msgNo == 0x2A3D) {
        if (gabi::load<u32>(mgr + 0x948) /* mSelectNum */ != 0) {
            *pMsgNo = 0x2A3B;
            field_0x9D0 = 0;
            field_0x980 = nullptr;
            return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
        }
    } else if (msgNo >= 0x378A && msgNo <= 0x378B) {
        field_0x9C6 = field_0x9C6 | 0x20;
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    if (field_0x980.get() == nullptr) {
        return 0x10;
    }
    field_0x980 = gabi::at<be<u32>>(gabi::ea(field_0x980.get()) + 4);
    if (field_0x9D0 != 0) {
        field_0x9D0 = field_0x9D0 + 1;
    }
    u32 v = *field_0x980;
    switch (v) {
    case 7:
        mItemNo = 0x26; /* HD: dItemNo_DELUXE_PICTO_BOX_e */
        return 0x10;
    case 8:
    case 11:
    case 12:
        return 0x10;
    case 9:
        /* HD */
        mItemNo = 0x1F;
        field_0x9C6 = field_0x9C6 | 2;
        return 0x10;
    case 10:
        /* HD: the picture purchase (GameCube: 9) */
        if (gabi::load<u32>(mgr + 0x948) == 0) {
            s16 msgRupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4); /* dComIfGp_getMessageRupee() */
            if ((s32)dComIfGs_getRupee() < (s32)msgRupee) {
                *pMsgNo = 0x378B;
            } else {
                field_0x980 = gabi::at<be<u32>>(gabi::ea(field_0x980.get()) + 4);
                if (field_0x9D0 != 0) {
                    field_0x9D0 = field_0x9D0 + 1;
                }
                *pMsgNo = *field_0x980;
                /* dComIfGp_setItemRupeeCount(-dComIfGp_getMessageRupee()) */
                s16 rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
                u32 play = dComIfGp_ea();
                gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - rupee);
                dComIfGs_onTmpBit(0x0301 /* UNK_0301 */);
            }
        } else {
            field_0x980 = nullptr;
            *pMsgNo = 0x378A;
        }
        return 0xF;
    case 6:
        field_0x9CC = true;
        field_0x9C6 = field_0x9C6 | 1;
        /* fall through */
    case 2:
    case 4: {
        /* dPb_erasePicture() (HD: the album slot in play + 0x5BEB) */
        u8 slot = gabi::load<u8>(dComIfGp_ea() + 0x5BEB);
        gabi::call(0x02725E0C, photo_album(), slot);
        u32 play = dComIfGp_ea();
        gabi::store<s16>(play + 0x5B66, gabi::load<s16>(play + 0x5B66) - 1);
    }
        /* fall through */
    case 1:
    case 3:
    case 5: {
        u8 reg = dComIfGs_getEventReg(0xC407 /* l_save_dat.field_0x06 */);
        dComIfGs_setEventReg(0xC407, reg + 1);
    }
        /* fall through */
    case 0:
        field_0x980 = nullptr;
        field_0x9D0 = 0;
        return 0x10;
    default:
        *pMsgNo = v;
        return 0xF;
    }
}
VERIFY(0x022CFE34, &daNpcPhoto_c::next_msgStatus);

/* 022D00B4 */
BOOL daNpcPhoto_c::isPhotoOk() {
    WWHD_FUNC(0x022D00B4, BOOL, this);
    u8 reg = dComIfGs_getEventReg(0xC407 /* l_save_dat.field_0x06 */);
    if (reg == 1) {
        if (dComIfGp_getPictureResult() == 1) {
            return TRUE;
        }
    } else if (reg == 3) {
        if (dComIfGp_getPictureResult() == 2) {
            return TRUE;
        }
    } else if (reg == 5 && dComIfGp_getPictureResult() == 3) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022D00B4, &daNpcPhoto_c::isPhotoOk);

/* 022D014C */
BOOL daNpcPhoto_c::isPhotoDxOk() {
    WWHD_FUNC(0x022D014C, BOOL, this);
    switch (dKy_get_dayofweek()) {
    case 0:
        if (dComIfGs_isEventBit(0x2D02 /* ZELDA_AWAKENED */)) {
            return TRUE;
        }
        break;
    case 1:
        if (dComIfGs_isEventBit(0x3910)) {
            return TRUE;
        }
        break;
    case 2:
        if (dComIfGs_isEventBit(0x3002) || dComIfGs_isEventBit(0x3001) || dComIfGs_isEventBit(0x3008) ||
            dComIfGs_isEventBit(0x3004) || dComIfGs_isEventBit(0x3020) || dComIfGs_isEventBit(0x3010) ||
            dComIfGs_isEventBit(0x3180)) {
            return TRUE;
        }
        break;
    case 3:
        if (dComIfGs_isEventBit(0x3920)) {
            return TRUE;
        }
        break;
    case 4:
        if (dComIfGs_isEventBit(0x1001)) {
            return TRUE;
        }
        break;
    case 5:
        if (dComIfGs_isEventBit(0x2D20)) {
            return TRUE;
        }
        break;
    case 6:
        if (dComIfGs_isEventBit(0x2D40)) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}
VERIFY(0x022D014C, &daNpcPhoto_c::isPhotoDxOk);

/* message tables (.data) */
enum : u32 {
    l_msg_1st_talk = 0x101C4F7C,
    l_msg_2nd_talk = 0x101C4F24,
    l_msg_1st_photo = 0x101C50AC,
    l_msg_2nd_photo = 0x101C50B8,
    l_msg_1st_order = 0x101C50C4,
    l_msg_1st_order_c = 0x101C5210,
    l_msg_1st_order_not_end = 0x101C50F0,
    l_msg_1st_order_not_end_c = 0x101C4F68,
    l_msg_2nd_order = 0x101C4F8C,
    l_msg_2nd_order_c = 0x101C4F79,
    l_msg_2nd_order_not_end = 0x101C50FC,
    l_msg_2nd_order_not_end_c = 0x101C4F6A,
    l_msg_3rd_order = 0x101C5108,
    l_msg_3rd_order_c = 0x101C4F50,
    l_msg_3rd_order_not_end = 0x101C511C,
    l_msg_3rd_order_not_end_c = 0x101C4F6C,
    l_msg_color = 0x101C5128,
    l_msg_color_c = 0x101C4F6E,
    l_msg_week_1st = 0x101C4F9C,
    l_msg_week = 0x101C5134, /* u32*[7] */
    l_msg_buy_photo = 0x101C4F2C,
    l_msg_get_photo = 0x101C4F34,
    l_msg_down = 0x101C4F3C,
    l_msg_ub4 = 0x101C4F44,
    l_msg_1st_order_xy = 0x101C5024,
    l_msg_2nd_order_xy = 0x101C5034,
    l_msg_3rd_order_xy = 0x101C51E0,
    l_msg_color_xy = 0x101C5044,
    l_msg_xy_buy_photo = 0x101C51EC,
};

/* 022D031C */
u32 daNpcPhoto_c::getMsg() {
    WWHD_FUNC(0x022D031C, u32, this);
    u32 msgNo = 0;
    mHD_B4E = 0;
    u8 eventReg = dComIfGs_getEventReg(0xC407 /* l_save_dat.field_0x06 */);
    field_0x980 = nullptr;
    field_0x9D0 = 0;
    if (dComIfGp_event_chkPhoto()) {
        if (eventReg < 1) {
            msgNo = 0x2A5C;
        } else if (eventReg < 3) {
            if (eventReg == 2) {
                msgNo = 0x2A57;
            } else if (isPhotoOk()) {
                field_0x9D0 = 0;
                field_0x980 = MSG(l_msg_1st_order_xy);
            } else {
                msgNo = 0x2A5D;
            }
        } else if (eventReg < 5) {
            if (eventReg == 4) {
                msgNo = 0x2A57;
            } else if (isPhotoOk()) {
                field_0x9D0 = 0;
                field_0x980 = MSG(l_msg_2nd_order_xy);
            } else {
                msgNo = 0x2A5D;
            }
        } else if (eventReg < 6) {
            if (isPhotoOk()) {
                field_0x9D0 = 0;
                mHD_B4E = 1; /* HD */
                field_0x980 = MSG(l_msg_3rd_order_xy);
            } else {
                msgNo = 0x2A5D;
            }
        } else {
            msgNo = 0x2A63;
        }
    } else if (dComIfGp_event_chkTalkXY()) {
        u8 itemNo = dComIfGp_event_getPreItemNo();
        if (itemNo == 0x26 /* dItemNo_DELUXE_PICTO_BOX_e */) {
            if (dComIfGs_isTmpBit(0x0302 /* UNK_0302 */)) {
                if (dComIfGs_getPictureNum() < 0xC) { /* HD: 12 pictures (GameCube 3) */
                    mItemNo = 0x9F; /* HD (GameCube dItemNo_LEGENDARY_PICTOGRAPH_e) */
                    field_0x980 = MSG(l_msg_xy_buy_photo);
                } else {
                    msgNo = 0x3787;
                }
            } else {
                msgNo = 0x2A56;
            }
        } else if (itemNo == 0x58 /* dItemNo_FIREFLY_BOTTLE_e */) {
            /* HD: no check of the Deluxe Picto Box, no mItemNo */
            if (eventReg >= 6) {
                field_0x9D0 = 0;
                field_0x980 = MSG(l_msg_color_xy);
                dComIfGs_setEquipBottleItemEmpty();
            } else {
                msgNo = 0x2A57;
            }
        } else {
            msgNo = 0x2A56;
        }
    } else if (field_0x9C1 == 3) {
        field_0x9D0 = 0;
        field_0x980 = MSG(l_msg_down);
    } else if (field_0x9C1 == 4) {
        field_0x9D0 = 0;
        field_0x980 = MSG(l_msg_ub4);
    } else if (dComIfGs_isEventBit(0x1601 /* l_save_dat.field_0x04 */)) {
        if (field_0x9C1 == 1) {
            msgNo = mMsgNno;
        } else if (dComIfGs_checkGetItem(0x26 /* dItemNo_DELUXE_PICTO_BOX_e */)) {
            if (dComIfGs_isTmpBit(0x0301 /* UNK_0301 */)) {
                field_0x9D0 = 0;
                field_0x980 = MSG(l_msg_buy_photo);
            } else if (dComIfGs_isTmpBit(0x0302 /* UNK_0302 */)) {
                field_0x9D0 = 0;
                field_0x980 = MSG(l_msg_get_photo);
            } else if (isPhotoDxOk()) {
                dComIfGs_onTmpBit(0x0302);
                if (!dComIfGs_isEventBit(0x3808 /* UNK_3808 */)) {
                    dComIfGs_onEventBit(0x3808);
                    field_0x9D0 = 0;
                    field_0x980 = MSG(l_msg_week_1st);
                } else {
                    u32 list = gabi::load<u32>(l_msg_week + dKy_get_dayofweek() * 4);
                    field_0x9D0 = 0;
                    field_0x980 = MSG(list);
                }
            } else {
                field_0x980 = MSG(l_msg_color);
                field_0x9D0 = l_msg_color_c;
            }
        } else {
            eventReg = dComIfGs_getEventReg(0xC407);
            if (eventReg < 1) {
                field_0x980 = MSG(l_msg_1st_order);
                field_0x9D0 = l_msg_1st_order_c;
            } else if (eventReg < 2) {
                field_0x9D0 = l_msg_1st_order_not_end_c;
                field_0x980 = MSG(l_msg_1st_order_not_end);
            } else if (eventReg < 3) {
                field_0x980 = MSG(l_msg_2nd_order);
                field_0x9D0 = l_msg_2nd_order_c;
            } else if (eventReg < 4) {
                field_0x980 = MSG(l_msg_2nd_order_not_end);
                field_0x9D0 = l_msg_2nd_order_not_end_c;
            } else if (eventReg < 5) {
                field_0x9D0 = l_msg_3rd_order_c;
                field_0x980 = MSG(l_msg_3rd_order);
            } else if (eventReg < 6) {
                field_0x980 = MSG(l_msg_3rd_order_not_end);
                field_0x9D0 = l_msg_3rd_order_not_end_c;
            } else {
                /* HD: no l_msg_3rd_order_end / l_msg_not_color */
                msgNo = 0x2A57;
            }
        }
    } else if (dComIfGs_checkGetItem(0x23 /* dItemNo_PICTO_BOX_e */) || dComIfGs_checkGetItem(0x26)) {
        BOOL got = dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */);
        field_0x9D0 = 0;
        if (!got) {
            field_0x980 = MSG(l_msg_1st_photo);
            dComIfGs_onEventBit(0x1701);
            initTexPatternAnm(true, 1);
        } else {
            field_0x980 = MSG(l_msg_2nd_photo);
        }
    } else {
        BOOL talked = dComIfGs_isEventBit(0x1208 /* l_save_dat.field_0x00 */);
        field_0x9D0 = 0;
        if (!talked) {
            field_0x980 = MSG(l_msg_1st_talk);
            dComIfGs_onEventBit(0x1208);
            field_0x9C6 = field_0x9C6 | 4;
        } else {
            field_0x980 = MSG(l_msg_2nd_talk);
            field_0x9C6 = field_0x9C6 | 4;
        }
    }
    if (field_0x980.get() != nullptr) {
        msgNo = *field_0x980;
    }
    return msgNo;
}
VERIFY(0x022D031C, &daNpcPhoto_c::getMsg);
