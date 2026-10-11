/**
 * d_a_npc_photo_msg.cpp (WWHD)
 * NPC - Lenzo: messages (getMsg, next_msgStatus) and the picture checks.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_photo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_photo.h"

// HD bindings and helpers.

/* dSv_player_item_c::setEquipBottleItemEmpty (save + 0x5C) */

static inline u8 photo_inPhotoEvent() { return gabi::object_at<dComIfG_inf_c>(dComIfGp_ea()).play.mEvtCtrl.mbInPhoto; }
static inline be<u32>* MSG(u32 a) { return &gabi::object_at<be<u32>>(a); }
static inline be<u32>* MSG(be<u32>* table) { return table; }

/* 022CFE34 */
u16 daNpcPhoto_c::next_msgStatus(be<u32>* pMsgNo) {
    u32 mgr = photo_msgManager(); // HD: mpCurrMsg is the message manager
    u32 msgNo = *pMsgNo;
    if (msgNo == 0x2A3A || msgNo == 0x2A3D) {
        if (gabi::object_at<dMesg_core_c>(mgr).mSelectNum /* mSelectNum */ != 0) {
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
    field_0x980 = field_0x980.get() + 1;
    if (field_0x9D0 != 0) {
        field_0x9D0 = field_0x9D0 + 1;
    }
    u32 v = *field_0x980;
    switch (v) {
    case 7:
        mItemNo = 0x26; // HD: dItemNo_DELUXE_PICTO_BOX_e
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
        // HD: the picture purchase (GameCube: 9)
        if (gabi::object_at<dMesg_core_c>(mgr).mSelectNum == 0) {
            s16 msgRupee = dComIfGp_getMessageRupee(); /* dComIfGp_getMessageRupee() */
            if ((s32)dComIfGs_getRupee() < (s32)msgRupee) {
                *pMsgNo = 0x378B;
            } else {
                field_0x980 = field_0x980.get() + 1;
                if (field_0x9D0 != 0) {
                    field_0x9D0 = field_0x9D0 + 1;
                }
                *pMsgNo = *field_0x980;
                /* dComIfGp_setItemRupeeCount(-dComIfGp_getMessageRupee()) */
                s16 rupee = dComIfGp_getMessageRupee();
                u32 play = dComIfGp_ea();
                gabi::object_at<dComIfG_inf_c>(play).play.mItemRupeeCount -= rupee;
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
        u8 slot = gabi::object_at<PhotoSelectedPicture>(dComIfGp_ea()).mSlot;
        photo_erasePicture(photo_album(), slot);
        u32 play = dComIfGp_ea();
        gabi::object_at<dComIfG_inf_c>(play).play.mItemPictureNumCount -= 1;
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
/* 022D00B4 */
BOOL daNpcPhoto_c::isPhotoOk() {
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
/* 022D014C */
BOOL daNpcPhoto_c::isPhotoDxOk() {
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
/* message tables (.data) */


/* 022D031C */
u32 daNpcPhoto_c::getMsg() {
    u32 msgNo = 0;
    mReplaceDateMessages = 0;
    u8 eventReg = dComIfGs_getEventReg(0xC407 /* l_save_dat.field_0x06 */);
    field_0x980 = nullptr;
    field_0x9D0 = 0;
    if (photo_inPhotoEvent()) {
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
                mReplaceDateMessages = 1; /* HD */
                field_0x980 = MSG(l_msg_3rd_order_xy);
            } else {
                msgNo = 0x2A5D;
            }
        } else {
            msgNo = 0x2A63;
        }
    } else if (dComIfGp_event_chkTalkXY()) {
        u8 itemNo = dComIfGp_event_getPreItemNo();
        if (itemNo == dItemNo_DELUXE_PICTO_BOX_e) {
            if (dComIfGs_isTmpBit(0x0302 /* UNK_0302 */)) {
                if (photo_getPictureNum() < 0xC) { // HD: 12 pictures (GameCube 3)
                    mItemNo = 0x9F; /* HD (GameCube dItemNo_LEGENDARY_PICTOGRAPH_e) */
                    field_0x980 = MSG(l_msg_xy_buy_photo);
                } else {
                    msgNo = 0x3787;
                }
            } else {
                msgNo = 0x2A56;
            }
        } else if (itemNo == dItemNo_FIREFLY_BOTTLE_e) {
            // HD: no check of the Deluxe Picto Box, no mItemNo
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
        } else if (dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
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
                    u32 list = l_msg_week[dKy_get_dayofweek()];
                    field_0x9D0 = 0;
                    field_0x980 = MSG(list);
                }
            } else {
                field_0x980 = MSG(l_msg_color);
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_color_c);
            }
        } else {
            eventReg = dComIfGs_getEventReg(0xC407);
            if (eventReg < 1) {
                field_0x980 = MSG(l_msg_1st_order);
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_1st_order_c);
            } else if (eventReg < 2) {
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_1st_order_not_end_c);
                field_0x980 = MSG(l_msg_1st_order_not_end);
            } else if (eventReg < 3) {
                field_0x980 = MSG(l_msg_2nd_order);
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_2nd_order_c);
            } else if (eventReg < 4) {
                field_0x980 = MSG(l_msg_2nd_order_not_end);
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_2nd_order_not_end_c);
            } else if (eventReg < 5) {
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_3rd_order_c);
                field_0x980 = MSG(l_msg_3rd_order);
            } else if (eventReg < 6) {
                field_0x980 = MSG(l_msg_3rd_order_not_end);
                field_0x9D0 = reinterpret_cast<be<s8>*>(l_msg_3rd_order_not_end_c);
            } else {
                // HD: no l_msg_3rd_order_end / l_msg_not_color
                msgNo = 0x2A57;
            }
        }
    } else if (dComIfGs_checkGetItem(dItemNo_PICTO_BOX_e) || dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
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

/* WWHD glue (tools/verify/autoglue.py): data owned by d_a_npc_photo.cpp

*/
