/**
 * d_a_npc_bs1_msg.cpp (WWHD)
 * NPC - Beedle (boat shop): setAnmFromMsgTag, next_msgStatus, getMsg, normal_talk, shop_talk, talk.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bs1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_bs1.h"

/* ---- local bindings (SHARED-CANDIDATE; the same as d_a_npc_bms1.h) ---- */
/* HD message manager *(0x101F4B5C): 025F795C status, 025F74D0 setStatus, 025F7DB0 messageSet;
 * cancel flag +0x922, msgNo +0x938, select number +0x948 */

// Pad B-trigger ABI binding.

/* dLetter_send */

/* dComIfGs_checkGetItem */

/* save events: dSv_event_c at *(0x101F84DC) + 0x644 */

/* play-object bytes: 0x5BD2 message sent, 0x5BD3 cancel button, 0x52B0/0x52B1 talk XY / item,
 * 0x5BA2 message set number, 0x5BA4 message rupee, 0x5B48 item rupee count, 0x5B70 beast counts */

/* dComIfGp_event_chkTalkXY(): event mode byte 1..4 */

/* file statics */
/* l_msgId */
#define BS1M_PAY_RUPEE 0x101D5F3Au    /* s16 */
#define BS1M_BUY_ITEM_MAX 0x101D5F47u /* u8 */
#define BS1M_BUY_ITEM 0x101D5F48u     /* u8 */
#define BS1M_getDefaultMsgCB 0x02212700u

/* mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f) */

/* 02214388 */
void daNpc_Bs1_c::setAnmFromMsgTag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeAttrInfo() */) {
    case 0:
        setAnm(0);
        setTexAnm(1);
        break;
    case 1:
        setAnm(1);
        setTexAnm(2);
        break;
    case 2:
        setAnm(2);
        setTexAnm(2);
        break;
    case 3:
        setAnm(3);
        setTexAnm(2);
        break;
    case 4:
        setAnm(0);
        setTexAnm(2);
        break;
    case 5:
        setAnm(4);
        setTexAnm(2);
        break;
    case 6:
        setAnm(5);
        setTexAnm(2);
        break;
    case 7:
        setAnm(6);
        setTexAnm(3);
        break;
    case 8:
        setAnm(7);
        setTexAnm(2);
        break;
    case 9:
        setAnm(8);
        setTexAnm(2);
        break;
    case 0xA:
        setAnm(9);
        setTexAnm(2);
        break;
    default:
        if ((m829 == 2 && bs1m_checkEnd(this)) || (m829 == 4 && bs1m_checkEnd(this)) ||
            (m829 == 6 && bs1m_checkEnd(this)) || (m829 == 8 && bs1m_checkEnd(this))) {
            setAnm(0);
        } else if (m829 == 9 && bs1m_checkEnd(this)) {
            setAnm(3);
        }
        break;
    }
    dComIfGp_clearMesgAnimeAttrInfo(); /* dComIfGp_clearMesgAnimeAttrInfo() */
}

/* cancel: both message numbers (the second may be NULL), the manager's cancel flag, sent, not cancelled */

/* B pressed on a shop menu message: leave (sold out / gold member / normal) */

/* the beast item buy: rupees and beast count (setItemRupeeCount / setItemBeastNumCount) */

/* buy the selected item (sound, hide, rupees) */

/* 02214858 */
u16 daNpc_Bs1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = bs1m_mgr();
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgNo = *pMsgNo;
    switch (msgNo) {
    case 0xF50:
    case 0xF53:
    case 0xF55:
    case 0xF56:
    case 0xF58:
    case 0xF6F:
    case 0xF70:
    case 0xF71:
    case 0xF73:
    case 0xF79:
    case 0xF81:
    case 0xF86:
    case 0xF8B:
    case 0xF90:
    case 0xF95:
    case 0xF9A:
    case 0xF9F:
    case 0xFD6:
    case 0xFD7:
    case 0x2F62:
    case 0x2F64:
    case 0x2F65:
    case 0x2F67:
        *pMsgNo = msgNo + 1;
        break;
    case 0xF78:
    case 0xF80:
    case 0xF85:
    case 0xF8A:
    case 0xF8F:
    case 0xF94:
    case 0xF99:
    case 0xF9E:
    case 0xFD4: {
        s32 rupee = dComIfGp_getMessageRupee(); /* dComIfGp_getMessageRupee() */
        m83C = rupee;
        s32 buyMax = bs1m_getBuyItemMax(rupee, m840);
        l_tag_buy_item_max = (u8)buyMax; /* setBuyItemMax */
        l_tag_buy_item = (u8)buyMax;     /* setBuyItem */
        if (buyMax == 0) {
            *pMsgNo = 0xF7E;
        } else if (msgNo == 0xFD4) {
            *pMsgNo = 0xF9F;
        } else {
            *pMsgNo = *pMsgNo + 1;
        }
        break;
    }
    case 0xF7A:
    case 0xF82:
    case 0xF87:
    case 0xF8C:
    case 0xF91:
    case 0xF96:
    case 0xF9B:
    case 0xFA0:
        bs1_play()->mMsgSetNumber = 1; /* dComIfGp_setMessageSetNumber(1) */
        if (bs1_message(mgr)->mSelectNum == 0 && bs1_play()->mMesgCancelButton == 0) {
            if (*pMsgNo == 0xFA0) {
                *pMsgNo = 0xFD2;
            } else {
                *pMsgNo = *pMsgNo + 1;
            }
        } else {
            *pMsgNo = 0xF7F;
        }
        break;
    case 0xF7B:
    case 0xF83:
    case 0xF88:
    case 0xF8D:
    case 0xF92:
    case 0xF97:
    case 0xF9C:
    case 0xFD2: {
        /* daNpc_Bs1_setPayRupee(m83C, getBuyItem()) (inline): limited by the wallet */
        dSv_player_c* playerSave = bs1_savePlayer();
        u8 n = (u8)l_tag_buy_item;
        u8 wallet = (u8)playerSave->mPlayerStatusA.mWalletSize;
        s32 price = m83C;
        u16 rupees = (u16)playerSave->mPlayerStatusA.mRupee;
        s32 max;
        if (wallet < 1) {
            max = 500;
        } else if (wallet == 1) {
            max = 1000;
        } else {
            max = 5000;
        }
        s32 pay = price * n;
        s32 limit = max - rupees;
        l_tag_pay_rupee = (s16)(pay < limit ? pay : limit);
        *pMsgNo = *pMsgNo + 1;
        break;
    }
    case 0xF7C:
    case 0xF84:
    case 0xF89:
    case 0xF8E:
    case 0xF93:
    case 0xF98:
    case 0xF9D:
    case 0xFD3:
        if (bs1_play()->mMesgCancelButton /* dComIfGp_checkMesgCancelButton() */ != 0 ||
            bs1_message(mgr)->mSelectNum != 0) {
            *pMsgNo = 0xF7F;
            break;
        }
        {
            u32 beastIdx;
            switch (*pMsgNo) {
            case 0xF7C:
                beastIdx = 1;
                break; /* dBeastIdx_BOKOBABA_SEED_e */
            case 0xF84:
                beastIdx = 0;
                break; /* dBeastIdx_SKULL_NECKLACE_e */
            case 0xF89:
                beastIdx = 4;
                break; /* dBeastIdx_RED_JELLY_e */
            case 0xF8E:
                beastIdx = 5;
                break; /* dBeastIdx_GREEN_JELLY_e */
            case 0xF93:
                beastIdx = 6;
                break; /* dBeastIdx_BLUE_JELLY_e */
            case 0xF98:
                beastIdx = 7;
                break; /* dBeastIdx_JOY_PENDANT_e */
            case 0xF9D:
                beastIdx = 2;
                break; /* dBeastIdx_GOLDEN_FEATHER_e */
            case 0xFD3:
                beastIdx = 3;
                break; /* dBeastIdx_KNIGHTS_CREST_e */
            default:
                /* uninitialised in the source: GHS leaves the register that holds `this` */
                beastIdx = gabi::ea(this);
                break;
            }
            bs1m_payBeast(beastIdx);
        }
        if (*pMsgNo == 0xFD3) {
            u8 r = dComIfGs_getEventReg(0x7F0F);
            r = (u8)(r + (u8)l_tag_buy_item);
            if (r > 0xF) {
                r = 0xF;
            }
            dComIfGs_setEventReg(0x7F0F, r);
            if (r < 0xA) {
                *pMsgNo = 0xFD5;
            } else if (dComIfGs_isEventBit(0x3B04)) {
                *pMsgNo = 0xFD9;
            } else {
                dComIfGs_onEventBit(0x3B04);
                *pMsgNo = 0xFD6;
            }
            break;
        }
        *pMsgNo = 0xF7D;
        break;
    case 0xFD5:
        *pMsgNo = 0xF7D;
        break;
    case 0x2F45:
    case 0x2F46:
    case 0x2F48:
    case 0x2F50:
    case 0x2F51:
    case 0x2F52:
    case 0x2F53:
    case 0x2F54:
    case 0x2F55:
    case 0x2F58:
    case 0x2F59:
    case 0x2F5A:
    case 0x2F5B:
    case 0x2F5C:
    case 0x2F5D:
    case 0x2F5E:
    case 0x2F5F:
    case 0x2F60:
    case 0x2F61:
    case 0x2F66:
    case 0x2F68:
    case 0x2F69:
    case 0x2F6A: /* HD: also the two new greetings */
        *pMsgNo = 0x2F47;
        break;
    case 0x2F6B:
        if (bs1m_trigB()) {
            bs1m_cancel(mgr, pMsgNo, pMsgNo2, 0x2F49); /* HD: cancels the message */
        } else {
            *pMsgNo = 0x2F6C; /* HD: no setDoStatusForce / setAStatusForce */
        }
        break;
    case 0x2F6C:
        if (bs1_play()->mMesgCancelButton != 0) {
            *pMsgNo = 0x2F6B;
        } else if (bs1_message(mgr)->mSelectNum == 0) {
            *pMsgNo = gabi::ftoi(cM_rndF(4.0f)) + 0x2F6E;
        } else {
            *pMsgNo = 0x2F6D;
        }
        break;
    case 0x2F6D:
    case 0x2F6E:
    case 0x2F6F:
    case 0x2F70:
    case 0x2F71:
        *pMsgNo = 0x2F6B;
        break;
    case 0xF3D:
    case 0xF3F:
    case 0xF41:
    case 0xF48:
    case 0xF49:
    case 0xF4A:
    case 0xF4B:
    case 0xF4D:
    case 0xF4F:
    case 0xF51:
    case 0xF52:
    case 0xF54:
    case 0xF57:
    case 0xF59:
    case 0xF5A:
    case 0xF5B:
    case 0xF5C:
    case 0xF5D:
    case 0xF5F:
    case 0xF60:
        *pMsgNo = 0xF3E;
        break;
    case 0xF5E:
        *pMsgNo = 0xF62;
        break;
    case 0xF3E:
    case 0xF63:
    case 0xF64:
    case 0xF65:
    case 0xF66:
        if (bs1m_trigB()) {
            bs1m_cancelLeave(this, mgr, pMsgNo, pMsgNo2);
        } else {
            msgStatus = 0xE; /* fopMsgStts_MSG_DISPLAYED_e; HD: no setDoStatusForce / setAStatusForce */
        }
        break;
    case 0x2F47:
    case 0x2F78:
        if (bs1m_trigB()) {
            bs1m_cancel(mgr, pMsgNo, pMsgNo2, 0x2F49);
        } else {
            msgStatus = 0xE;
        }
        break;
    case 0xF42:
    case 0xF43:
    case 0xF44:
    case 0xF67:
    case 0xF69:
    case 0xF6B:
    case 0xF6D:
        if (bs1m_trigB()) {
            bs1m_cancelLeave(this, mgr, pMsgNo, pMsgNo2);
        } else {
            *pMsgNo = mShopItems.getSelectItemBuyMsg();
        }
        break;
    case 0x2F4A:
    case 0x2F4B:
    case 0x2F4C:
    case 0x2F72:
    case 0x2F74:
    case 0x2F76:
        if (bs1m_trigB()) {
            bs1m_cancel(mgr, pMsgNo, pMsgNo2, 0x2F49);
        } else {
            *pMsgNo = mShopItems.getSelectItemBuyMsg();
        }
        break;
    case 0xF45:
    case 0xF46:
    case 0xF47:
    case 0xF68:
    case 0xF6A:
    case 0xF6C:
    case 0xF6E: {
        if (bs1_play()->mMesgCancelButton != 0 || bs1_message(mgr)->mSelectNum != 0) {
            *pMsgNo = bs1m_getSelectItemShowMsg(&mShopItems);
            break;
        }
        s16 rupee = dComIfGp_getMessageRupee();
        s32 status = bs1m_boughtErrorStatus(&mShopItems, 0, rupee);
        if (status & 1) {
            *pMsgNo = 0xF49;
        } else if (status & 2) {
            *pMsgNo = 0xF48;
        } else if (status & 0x20) {
            *pMsgNo = 0xF4A;
        } else if (status & 4) {
            *pMsgNo = 0xF4B;
        } else if (status & 0x10) {
            *pMsgNo = 0xF4D;
        } else {
            bs1m_bought(this, rupee);
            if (mShopItems.getSelectItemNo() == 0x2C /* dItemNo_BAIT_BAG_e */) {
                bs1m_SoldOutItem(&mShopItems, mShopItems.mSelectedItemIdx);
                m76C[mShopItems.mSelectedItemIdx] = 1;
            }
            if (!bs1m_checkItemGet(mShopItems.getSelectItemNo(), 0)) {
                m82A = 3;
                msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
                break;
            }
            bs1m_execItemGet(mShopItems.getSelectItemNo());
            if (mType == 0) {
                if (dComIfGs_getEventReg(0x86FF) != 0) {
                    *pMsgNo = 0xF4C;
                } else {
                    *pMsgNo = 0xF4E;
                }
            } else {
                *pMsgNo = 0x2F53;
            }
        }
        break;
    }
    case 0x2F4D:
    case 0x2F4E:
    case 0x2F4F:
    case 0x2F73:
    case 0x2F75:
    case 0x2F77: {
        if (bs1_play()->mMesgCancelButton != 0 || bs1_message(mgr)->mSelectNum != 0) {
            *pMsgNo = bs1m_getSelectItemShowMsg(&mShopItems);
            break;
        }
        s16 rupee = dComIfGp_getMessageRupee();
        s32 status = bs1m_boughtErrorStatus(&mShopItems, 0, rupee);
        if (status & 0x20) {
            *pMsgNo = 0x2F50;
        } else if (status & 4) {
            *pMsgNo = 0x2F51;
        } else if (status & 0x10) {
            *pMsgNo = 0x2F52;
        } else {
            bs1m_bought(this, rupee);
            u8 itemNo = mShopItems.getSelectItemNo();
            if (itemNo == 0x50 /* dItemNo_EMPTY_BOTTLE_e */ || itemNo == 7 /* dItemNo_HEART_PIECE_e */ ||
                itemNo == 0xE1 /* dItemNo_COLLECT_MAP_30_e */) {
                bs1m_SoldOutItem(&mShopItems, mShopItems.mSelectedItemIdx);
                m76C[mShopItems.mSelectedItemIdx] = 1;
                switch (itemNo) {
                case 0x50:
                    dComIfGs_onEventBit(0x2020);
                    break;
                case 7:
                    dComIfGs_onEventBit(0x2010);
                    break;
                case 0xE1:
                    dComIfGs_onEventBit(0x2008);
                    break;
                }
            }
            if (!bs1m_checkItemGet(mShopItems.getSelectItemNo(), 0)) {
                m82A = 3;
                msgStatus = 0x10;
                break;
            }
            bs1m_execItemGet(mShopItems.getSelectItemNo());
            *pMsgNo = 0x2F53;
        }
        break;
    }
    case 0xF4C:
    case 0xF4E: {
        s32 points = dComIfGs_getEventReg(0x86FF) + 1;
        if (points > 0xFF) {
            points = 0xFF;
        }
        dComIfGs_setEventReg(0x86FF, (u8)points);
        if (points >= 60) {
            if (points == 60) {
                bs1m_letterSend(0xAF03); /* LETTER_GOLD_MEMBERSHIP */
                *pMsgNo = 0xF53;
            } else {
                *pMsgNo = 0xF3E;
            }
        } else if (points >= 30) {
            if (points == 30) {
                bs1m_letterSend(0xB003); /* LETTER_SILVER_MEMBERSHIP */
                *pMsgNo = 0xF50;
            } else {
                *pMsgNo = 0xF52;
            }
        } else {
            *pMsgNo = 0xF4F;
        }
        break;
    }
    default:
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}

/* 022157C8 */
u32 daNpc_Bs1_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 msgNo = m740;
    if (msgNo != 0) {
        m740 = 0;
        return msgNo;
    }
    if (bs1m_chkTalkXY(bs1_play()->mEvtCtrl.mTalkButton)) {
        dComIfG_play_c* play = bs1_play();
        s8 type = mType;
        u8 itemNo = (u8)play->mEvtCtrl.mItemNo; /* dComIfGp_event_getPreItemNo() */
        if (type == 0) {
            if (bs1m_isEmono(itemNo)) {
                m840 = itemNo;
                if (itemNo == 0x1F) {
                    msgNo = 0xF94; /* dItemNo_JOY_PENDANT_e */
                } else if ((itemNo >= 0x45 && itemNo <= 0x47) || (itemNo >= 0x49 && itemNo <= 0x4B)) {
                    /* skull necklace, boko baba seed, golden feather, red / green / blue jelly: switch table
                     */
                    msgNo = (u16)l_beastMessages.mEntries[itemNo - 0x45];
                } else if (dComIfGs_getEventReg(0x7F0F) < 10) {
                    msgNo = 0xF9E;
                } else {
                    msgNo = 0xFD4;
                }
            } else if (itemNo == 0x9D /* dItemNo_COMPLIMENTARY_ID_e */) {
                msgNo = 0xF6F;
            } else if (itemNo == 0x9E /* dItemNo_FILL_UP_COUPON_e */) {
                msgNo = 0xF73;
            } else {
                msgNo = 0xF75;
            }
        } else if (itemNo == 0x9D || itemNo == 0x9E) {
            msgNo = 0x2F56;
        } else if (bs1m_isEmono(itemNo)) {
            msgNo = 0x2F79;
        } else {
            msgNo = 0x2F57;
        }
    } else if (mType == 0) {
        u8 points = dComIfGs_getEventReg(0x86FF);
        if (bs1m_isSoldOutItemAll(&mShopItems)) {
            msgNo = 0xF3D;
        } else if (bs1m_checkGetItem(0x31 /* dItemNo_BOMB_BAG_e */) && !dComIfGs_isEventBit(0x1F20) &&
                   isSellBomb()) {
            dComIfGs_onEventBit(0x1F20);
            m837 = 1;
            msgNo = 0xF55;
        } else if (m837 != 0) {
            msgNo = 0xF58;
        } else if (points >= 60) {
            if (m836 != 0) {
                msgNo = 0xF5D;
            } else {
                msgNo = 0xF5C;
                m836 = 1;
            }
        } else if (points != 0) {
            if (m836 != 0) {
                msgNo = 0xF5B;
            } else {
                msgNo = 0xF5A;
                m836 = 1;
            }
        } else if (m836 != 0) {
            msgNo = 0xF41;
        } else {
            m836 = 1;
            msgNo = 0xF3D;
        }
    } else if (bs1m_isSoldOutItemAll(&mShopItems)) {
        msgNo = 0x2F62;
    } else if (bs1m_checkGetItem(0x31) && !dComIfGs_isEventBit(0x1F20) && isSellBomb()) {
        dComIfGs_onEventBit(0x1F20);
        m837 = 1;
        msgNo = 0x2F64;
    } else if (m837 != 0) {
        msgNo = 0x2F67;
    } else if (dComIfGs_isEventBit(0x1F08)) {
        if (dComIfGs_isEventBit(0x2040)) {
            if (m838 == 1) {
                msgNo = 0x2F60;
            } else {
                msgNo = 0x2F61;
            }
        } else {
            dComIfGs_onEventBit(0x2040);
            msgNo = 0x2F5F;
            m838 = 1;
        }
    } else {
        switch (dComIfGs_getEventReg(0xBB07)) {
        case 0:
            if (m836 != 0 || dComIfGs_isEventBit(0x1F10)) {
                msgNo = 0x2F46;
                break;
            }
            dComIfGs_onEventBit(0x1F10);
            msgNo = 0x2F45;
            break;
        case 1:
            msgNo = m836 != 0 ? 0x2F46 : 0x2F58;
            break;
        case 2:
            msgNo = m836 != 0 ? 0x2F46 : 0x2F59;
            break;
        case 3:
            msgNo = m836 != 0 ? 0x2F46 : 0x2F5A;
            break;
        case 4:
            msgNo = m836 != 0 ? 0x2F46 : 0x2F5B;
            break;
        case 5:
            msgNo = m836 != 0 ? 0x2F46 : 0x2F5C;
            break;
        case 6:
            msgNo = m836 != 0 ? 0x2F69 : 0x2F5D;
            break;
        case 7:
            msgNo = m836 != 0 ? 0x2F6A : 0x2F5E;
            break;
        default:
            /* HD: an assertion (line 0x5D5) instead of an uninitialised message number */
            JUT_ASSERT_fail("d_a_npc_bs1.cpp", 0x5D5, "0");
            msgNo = 0;
            break;
        }
        if (dComIfGs_isEventBit(0x1F10)) {
            dComIfGs_onEventBit(0x1F10);
        }
        m836 = 1;
    }
    return msgNo;
}

/* the callers pass next_msgStatus's whole r3 on (the status is a u16 in the source) */

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline): m24 + normalize(m18 - m24) * 100, copied
 * through the FPRs into *out */

/* 02215DA0 */
u16 daNpc_Bs1_c::normal_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = bs1m_mgr();
    u16 status = (u16)bs1m_getStatus(mgr);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        bs1m_setStatus(mgr, bs1m_nextMsgStatus(this, &m738, nullptr));
        if (bs1m_getStatus(mgr) == 0xF) {
            bs1m_messageSet(mgr, m738, nullptr);
        }
    } else if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        bs1m_setStatus(mgr, 0x13);
    } else if (status == 1 /* fopMsgStts_MSG_PREPARING_e */) {
        bs1m_demoMsgFlagOn();
    }
    gabi::Local<cXyz> pos;
    bs1m_getItemZoomPos(&mShopCamAction, pos.get());
    mShopItems.Item_ZoomUp(pos.get());
    mpShopCursor->m54 = 0; /* hide() */
    return status;
}

/* 02215F2C */
u16 daNpc_Bs1_c::shop_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = bs1m_mgr();
    mpShopCursor->m54 = 1; /* show() */
    if (bs1m_nowTriggercheck(&mStickControl, &mShopItems, &m738, BS1M_getDefaultMsgCB, this)) {
        m708 = 1;
        m73C = 0;
        if (m738 == 0xF3F) {
            m835 = 2;
        }
    }
    u16 status = (u16)bs1m_getStatus(mgr);
    if (status == 0xE || status == 0xF) {
        if (m708 != 0 || bs1_message(mgr)->mCancel != 0) { /* HD: also after a cancel */
            m708 = 0;
        } else {
            m73C = m738;
            bs1m_setStatus(mgr, bs1m_nextMsgStatus(this, &m73C, &m738));
            if (bs1m_getStatus(mgr) == 0xF) {
                bs1m_messageSet(mgr, m73C, nullptr);
            }
        }
    } else if (status == 0x12) {
        bs1m_setStatus(mgr, 0x13);
    }
    return status;
}

/* 02216154 */
u16 daNpc_Bs1_c::talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = bs1m_mgr();
    u16 status = 0xFF;
    if (m835 == 0) {
        bs1m_msgId() = 0xFFFFFFFF;
        m738 = getMsg();
        m73C = 0;
        m835 = 1;
    } else if (m835 != -1) {
        if (bs1m_msgId() == 0xFFFFFFFF) {
            if (bs1m_chkTalkXY(bs1_play()->mEvtCtrl.mTalkButton) && !bs1m_chkPresentEnd()) {
                return 0xFF;
            }
            bs1m_msgId() = bs1m_messageSet(mgr, m738, &eyePos); /* HD: the message is anchored at the eyes */
        } else {
            setAnmFromMsgTag();
            s8 step = m835;
            if (step == 1) {
                m835 = 2; /* HD: no message process search */
            } else if (step == 2) {
                status = normal_talk();
            } else if (step == 3) {
                status = shop_talk();
            }
            if (bs1_play()->mMesgSendButton /* dComIfGp_checkMesgSendButton() */ != 0) {
                u32 msgNo = bs1_message(mgr)->mMsgNo;
                m738 = msgNo;
                if (shopStickMoveMsgCheck(msgNo)) {
                    if (msgNo == 0xF3E || msgNo == 0x2F47) {
                        s16 hide = mShopItems.mbIsHide;
                        mShopItems.mSelectedItemIdx = -1;
                        if (hide != 0) {
                            mShopItems.showItem();
                        }
                    }
                    m835 = 3;
                } else {
                    if (!shopMsgCheck(msgNo)) {
                        s16 hide = mShopItems.mbIsHide;
                        mShopItems.mSelectedItemIdx = -1;
                        if (hide != 0) {
                            mShopItems.showItem();
                        }
                    }
                    m835 = 2;
                }
            }
        }
    }
    mShopCamAction.m54 = mShopItems.mSelectedItemIdx;
    return status;
}

// clang-format off
/* WWHD glue
str 100185E4 "d_a_npc_bs1.cpp" in 022157C8
str 100185D0 "0" in 022157C8
*/
// clang-format on
