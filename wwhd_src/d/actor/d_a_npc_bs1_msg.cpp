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
static inline u32 bs1m_mgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 bs1m_getStatus(u32 m) { return gabi::call<u32>(0x025F795C, m); }
static inline void bs1m_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 bs1m_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
static inline void bs1m_demoMsgFlagOn() { gabi::call(0x025DB58C); }
/* 020078BC: pad trigger check (CPad_CHECK_TRIG_B(0)) */
static inline BOOL bs1m_trigB() { return gabi::call<BOOL>(0x020078BC, 0); }
static inline s32 bs1m_boughtErrorStatus(ShopItems_c_l* items, s32 a, s32 price) { return gabi::call<s32>(0x025BBA1C, items, a, price); }
static inline BOOL bs1m_nowTriggercheck(STControl_l* stick, ShopItems_c_l* items, be<u32>* msg, u32 cb, void* arg) {
    return gabi::call<BOOL>(0x025BB698, stick, items, msg, cb, arg);
}
static inline BOOL bs1m_isSoldOutItemAll(ShopItems_c_l* items) { return gabi::call<BOOL>(0x025BCF44, items); }
static inline void bs1m_SoldOutItem(ShopItems_c_l* items, s16 idx) { gabi::call(0x025BCECC, items, idx); }
static inline u32 bs1m_getSelectItemShowMsg(ShopItems_c_l* items) { return gabi::call<u32>(0x025BD260, items); }
static inline BOOL bs1m_checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
static inline void bs1m_execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
static inline void bs1m_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }
static inline void bs1m_letterSend(u16 id) { gabi::call(0x02586D5C, id); }               /* dLetter_send */
static inline BOOL bs1m_isEmono(u8 item) { return gabi::call<BOOL>(0x02550FF4, item); }
static inline BOOL bs1m_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); } /* dComIfGs_checkGetItem */
static inline s32 bs1m_getBuyItemMax(s32 rupee, s32 item) { return gabi::call<s32>(0x022124F0, rupee, item); }
static inline BOOL bs1m_chkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline BOOL bs1m_normalizeRS(cXyz* v) { return gabi::call<BOOL>(0x0201B47C, v); }
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 */
static inline dSv_event_c* bs1m_ev() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL bs1m_isEventBit(u16 f) { return dSv_event_isEventBit(bs1m_ev(), f); }
static inline void bs1m_onEventBit(u16 f) { dSv_event_onEventBit(bs1m_ev(), f); }
static inline u8 bs1m_getEventReg(u16 r) { return dSv_event_getEventReg(bs1m_ev(), r); }
static inline void bs1m_setEventReg(u16 r, u8 v) { gabi::call(0x025B8AF4, bs1m_ev(), r, v); }
/* play-object bytes: 0x5BD2 message sent, 0x5BD3 cancel button, 0x52B0/0x52B1 talk XY / item,
 * 0x5BA2 message set number, 0x5BA4 message rupee, 0x5B48 item rupee count, 0x5B70 beast counts */
static inline u8 bs1m_play_u8(u32 off) { return gabi::load<u8>(dComIfGp_ea() + off); }
static inline void bs1m_play_set_u8(u32 off, u8 v) { gabi::store<u8>(dComIfGp_ea() + off, v); }
/* dComIfGp_event_chkTalkXY(): event mode byte 1..4 */
static inline bool bs1m_chkTalkXY(u8 mode) { return (u32)(mode - 1) <= 3; }

/* file statics */
static inline be<u32>& bs1m_msgId() { return *gabi::at<be<u32>>(0x10466858); }  /* l_msgId */
#define BS1M_PAY_RUPEE 0x101D5F3Au   /* s16 */
#define BS1M_BUY_ITEM_MAX 0x101D5F47u /* u8 */
#define BS1M_BUY_ITEM 0x101D5F48u     /* u8 */
#define BS1M_getDefaultMsgCB 0x02212700u

/* mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f) */
static inline BOOL bs1m_checkEnd(daNpc_Bs1_c* a) {
    mDoExt_McaMorf* m = a->mpMorf;
    f32 e = (f32)(s32)m->mFrameCtrl.mEnd;
    return m->mFrameCtrl.checkPass(e - 1.0f);
}

/* 02214388 */
void daNpc_Bs1_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x02214388, void, this);
    switch (bs1m_play_u8(0x5BC5) /* dComIfGp_getMesgAnimeAttrInfo() */) {
    case 0: setAnm(0); setTexAnm(1); break;
    case 1: setAnm(1); setTexAnm(2); break;
    case 2: setAnm(2); setTexAnm(2); break;
    case 3: setAnm(3); setTexAnm(2); break;
    case 4: setAnm(0); setTexAnm(2); break;
    case 5: setAnm(4); setTexAnm(2); break;
    case 6: setAnm(5); setTexAnm(2); break;
    case 7: setAnm(6); setTexAnm(3); break;
    case 8: setAnm(7); setTexAnm(2); break;
    case 9: setAnm(8); setTexAnm(2); break;
    case 0xA: setAnm(9); setTexAnm(2); break;
    default:
        if ((m829 == 2 && bs1m_checkEnd(this)) || (m829 == 4 && bs1m_checkEnd(this)) ||
            (m829 == 6 && bs1m_checkEnd(this)) || (m829 == 8 && bs1m_checkEnd(this))) {
            setAnm(0);
        } else if (m829 == 9 && bs1m_checkEnd(this)) {
            setAnm(3);
        }
        break;
    }
    bs1m_play_set_u8(0x5BC5, 0xFF); /* dComIfGp_clearMesgAnimeAttrInfo() */
}
VERIFY(0x02214388, &daNpc_Bs1_c::setAnmFromMsgTag);

/* cancel: both message numbers (the second may be NULL), the manager's cancel flag, sent, not cancelled */
static inline void bs1m_cancel(u32 mgr, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    gabi::store<u8>(mgr + 0x922, 1);
    bs1m_play_set_u8(0x5BD2, 1);
    bs1m_play_set_u8(0x5BD3, 0);
}
/* B pressed on a shop menu message: leave (sold out / gold member / normal) */
static inline void bs1m_cancelLeave(daNpc_Bs1_c* a, u32 mgr, be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    u8 points = bs1m_getEventReg(0x86FF);
    u32 msgNo;
    if (bs1m_isSoldOutItemAll(&a->mShopItems)) {
        msgNo = 0xF62;
    } else if (points >= 60) {
        msgNo = 0xF61;
    } else {
        msgNo = 0xF40;
    }
    bs1m_cancel(mgr, pMsgNo, pMsgNo2, msgNo);
}
/* the beast item buy: rupees and beast count (setItemRupeeCount / setItemBeastNumCount) */
static inline void bs1m_payBeast(u32 beastIdx) {
    s16 pay = gabi::load<s16>(BS1M_PAY_RUPEE);
    u32 play = dComIfGp_ea();
    gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) + pay);
    u8 n = gabi::load<u8>(BS1M_BUY_ITEM);
    play = dComIfGp_ea();
    u32 p = play + 0x5B70 + beastIdx * 2;
    gabi::store<s16>(p, (s16)(gabi::load<s16>(p) - n));
}
/* buy the selected item (sound, hide, rupees) */
static inline void bs1m_bought(daNpc_Bs1_c* a, s16 rupee) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    bs1m_seStart(0x87F /* JA_SE_SHOP_BOUGHT */, &a->eyePos, 0, reverb);
    a->mShopItems.hideSelectItem();
    u32 play = dComIfGp_ea();
    gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - rupee);
}

/* 02214858 (HD: a second message number pointer, may be NULL) */
u16 daNpc_Bs1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    WWHD_FUNC(0x02214858, u16, this, pMsgNo, pMsgNo2);
    u32 mgr = bs1m_mgr();
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgNo = *pMsgNo;
    switch (msgNo) {
    case 0xF50: case 0xF53: case 0xF55: case 0xF56: case 0xF58: case 0xF6F: case 0xF70: case 0xF71:
    case 0xF73: case 0xF79: case 0xF81: case 0xF86: case 0xF8B: case 0xF90: case 0xF95: case 0xF9A:
    case 0xF9F: case 0xFD6: case 0xFD7: case 0x2F62: case 0x2F64: case 0x2F65: case 0x2F67:
        *pMsgNo = msgNo + 1;
        break;
    case 0xF78: case 0xF80: case 0xF85: case 0xF8A: case 0xF8F: case 0xF94: case 0xF99: case 0xF9E:
    case 0xFD4: {
        s32 rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4); /* dComIfGp_getMessageRupee() */
        m83C = rupee;
        s32 buyMax = bs1m_getBuyItemMax(rupee, m840);
        gabi::store<u8>(BS1M_BUY_ITEM_MAX, (u8)buyMax); /* setBuyItemMax */
        gabi::store<u8>(BS1M_BUY_ITEM, (u8)buyMax);     /* setBuyItem */
        if (buyMax == 0) {
            *pMsgNo = 0xF7E;
        } else if (msgNo == 0xFD4) {
            *pMsgNo = 0xF9F;
        } else {
            *pMsgNo = *pMsgNo + 1;
        }
        break;
    }
    case 0xF7A: case 0xF82: case 0xF87: case 0xF8C: case 0xF91: case 0xF96: case 0xF9B: case 0xFA0:
        gabi::store<u16>(dComIfGp_ea() + 0x5BA2, 1); /* dComIfGp_setMessageSetNumber(1) */
        if (gabi::load<u32>(mgr + 0x948) == 0 && bs1m_play_u8(0x5BD3) == 0) {
            if (*pMsgNo == 0xFA0) {
                *pMsgNo = 0xFD2;
            } else {
                *pMsgNo = *pMsgNo + 1;
            }
        } else {
            *pMsgNo = 0xF7F;
        }
        break;
    case 0xF7B: case 0xF83: case 0xF88: case 0xF8D: case 0xF92: case 0xF97: case 0xF9C: case 0xFD2: {
        /* daNpc_Bs1_setPayRupee(m83C, getBuyItem()) (inline): limited by the wallet */
        u32 save = gabi::load<u32>(0x101F84DC);
        u8 n = gabi::load<u8>(BS1M_BUY_ITEM);
        u8 wallet = gabi::load<u8>(save + 0x32);
        s32 price = m83C;
        u16 rupees = gabi::load<u16>(save + 0x24);
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
        gabi::store<s16>(BS1M_PAY_RUPEE, (s16)(pay < limit ? pay : limit));
        *pMsgNo = *pMsgNo + 1;
        break;
    }
    case 0xF7C: case 0xF84: case 0xF89: case 0xF8E: case 0xF93: case 0xF98: case 0xF9D: case 0xFD3:
        if (bs1m_play_u8(0x5BD3) /* dComIfGp_checkMesgCancelButton() */ != 0 || gabi::load<u32>(mgr + 0x948) != 0) {
            *pMsgNo = 0xF7F;
            break;
        }
        {
            u32 beastIdx;
            switch (*pMsgNo) {
            case 0xF7C: beastIdx = 1; break; /* dBeastIdx_BOKOBABA_SEED_e */
            case 0xF84: beastIdx = 0; break; /* dBeastIdx_SKULL_NECKLACE_e */
            case 0xF89: beastIdx = 4; break; /* dBeastIdx_RED_JELLY_e */
            case 0xF8E: beastIdx = 5; break; /* dBeastIdx_GREEN_JELLY_e */
            case 0xF93: beastIdx = 6; break; /* dBeastIdx_BLUE_JELLY_e */
            case 0xF98: beastIdx = 7; break; /* dBeastIdx_JOY_PENDANT_e */
            case 0xF9D: beastIdx = 2; break; /* dBeastIdx_GOLDEN_FEATHER_e */
            case 0xFD3: beastIdx = 3; break; /* dBeastIdx_KNIGHTS_CREST_e */
            default:
                /* uninitialised in the source: GHS leaves the register that holds `this` */
                beastIdx = gabi::ea(this);
                break;
            }
            bs1m_payBeast(beastIdx);
        }
        if (*pMsgNo == 0xFD3) {
            u8 r = bs1m_getEventReg(0x7F0F);
            r = (u8)(r + gabi::load<u8>(BS1M_BUY_ITEM));
            if (r > 0xF) {
                r = 0xF;
            }
            bs1m_setEventReg(0x7F0F, r);
            if (r < 0xA) {
                *pMsgNo = 0xFD5;
            } else if (bs1m_isEventBit(0x3B04)) {
                *pMsgNo = 0xFD9;
            } else {
                bs1m_onEventBit(0x3B04);
                *pMsgNo = 0xFD6;
            }
            break;
        }
        *pMsgNo = 0xF7D;
        break;
    case 0xFD5:
        *pMsgNo = 0xF7D;
        break;
    case 0x2F45: case 0x2F46: case 0x2F48: case 0x2F50: case 0x2F51: case 0x2F52: case 0x2F53: case 0x2F54:
    case 0x2F55: case 0x2F58: case 0x2F59: case 0x2F5A: case 0x2F5B: case 0x2F5C: case 0x2F5D: case 0x2F5E:
    case 0x2F5F: case 0x2F60: case 0x2F61: case 0x2F66: case 0x2F68:
    case 0x2F69: case 0x2F6A: /* HD: also the two new greetings */
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
        if (bs1m_play_u8(0x5BD3) != 0) {
            *pMsgNo = 0x2F6B;
        } else if (gabi::load<u32>(mgr + 0x948) == 0) {
            *pMsgNo = gabi::ftoi(cM_rndF(4.0f)) + 0x2F6E;
        } else {
            *pMsgNo = 0x2F6D;
        }
        break;
    case 0x2F6D: case 0x2F6E: case 0x2F6F: case 0x2F70: case 0x2F71:
        *pMsgNo = 0x2F6B;
        break;
    case 0xF3D: case 0xF3F: case 0xF41: case 0xF48: case 0xF49: case 0xF4A: case 0xF4B: case 0xF4D:
    case 0xF4F: case 0xF51: case 0xF52: case 0xF54: case 0xF57: case 0xF59: case 0xF5A: case 0xF5B:
    case 0xF5C: case 0xF5D: case 0xF5F: case 0xF60:
        *pMsgNo = 0xF3E;
        break;
    case 0xF5E:
        *pMsgNo = 0xF62;
        break;
    case 0xF3E:
    case 0xF63: case 0xF64: case 0xF65: case 0xF66:
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
    case 0xF42: case 0xF43: case 0xF44: case 0xF67: case 0xF69: case 0xF6B: case 0xF6D:
        if (bs1m_trigB()) {
            bs1m_cancelLeave(this, mgr, pMsgNo, pMsgNo2);
        } else {
            *pMsgNo = mShopItems.getSelectItemBuyMsg();
        }
        break;
    case 0x2F4A: case 0x2F4B: case 0x2F4C: case 0x2F72: case 0x2F74: case 0x2F76:
        if (bs1m_trigB()) {
            bs1m_cancel(mgr, pMsgNo, pMsgNo2, 0x2F49);
        } else {
            *pMsgNo = mShopItems.getSelectItemBuyMsg();
        }
        break;
    case 0xF45: case 0xF46: case 0xF47: case 0xF68: case 0xF6A: case 0xF6C: case 0xF6E: {
        if (bs1m_play_u8(0x5BD3) != 0 || gabi::load<u32>(mgr + 0x948) != 0) {
            *pMsgNo = bs1m_getSelectItemShowMsg(&mShopItems);
            break;
        }
        s16 rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
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
                if (bs1m_getEventReg(0x86FF) != 0) {
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
    case 0x2F4D: case 0x2F4E: case 0x2F4F: case 0x2F73: case 0x2F75: case 0x2F77: {
        if (bs1m_play_u8(0x5BD3) != 0 || gabi::load<u32>(mgr + 0x948) != 0) {
            *pMsgNo = bs1m_getSelectItemShowMsg(&mShopItems);
            break;
        }
        s16 rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
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
                case 0x50: bs1m_onEventBit(0x2020); break;
                case 7: bs1m_onEventBit(0x2010); break;
                case 0xE1: bs1m_onEventBit(0x2008); break;
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
    case 0xF4C: case 0xF4E: {
        s32 points = bs1m_getEventReg(0x86FF) + 1;
        if (points > 0xFF) {
            points = 0xFF;
        }
        bs1m_setEventReg(0x86FF, (u8)points);
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
VERIFY(0x02214858, &daNpc_Bs1_c::next_msgStatus);

/* 022157C8 */
u32 daNpc_Bs1_c::getMsg() {
    WWHD_FUNC(0x022157C8, u32, this);
    u32 msgNo = m740;
    if (msgNo != 0) {
        m740 = 0;
        return msgNo;
    }
    if (bs1m_chkTalkXY(bs1m_play_u8(0x52B0))) {
        u32 play = dComIfGp_ea();
        s8 type = mType;
        u8 itemNo = gabi::load<u8>(play + 0x52B1); /* dComIfGp_event_getPreItemNo() */
        if (type == 0) {
            if (bs1m_isEmono(itemNo)) {
                m840 = itemNo;
                if (itemNo == 0x1F) {
                    msgNo = 0xF94; /* dItemNo_JOY_PENDANT_e */
                } else if ((itemNo >= 0x45 && itemNo <= 0x47) || (itemNo >= 0x49 && itemNo <= 0x4B)) {
                    /* skull necklace, boko baba seed, golden feather, red / green / blue jelly: switch table */
                    msgNo = gabi::load<u16>(0x1001854A + itemNo * 2);
                } else if (bs1m_getEventReg(0x7F0F) < 10) {
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
        u8 points = bs1m_getEventReg(0x86FF);
        if (bs1m_isSoldOutItemAll(&mShopItems)) {
            msgNo = 0xF3D;
        } else if (bs1m_checkGetItem(0x31 /* dItemNo_BOMB_BAG_e */) && !bs1m_isEventBit(0x1F20) && isSellBomb()) {
            bs1m_onEventBit(0x1F20);
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
    } else if (bs1m_checkGetItem(0x31) && !bs1m_isEventBit(0x1F20) && isSellBomb()) {
        bs1m_onEventBit(0x1F20);
        m837 = 1;
        msgNo = 0x2F64;
    } else if (m837 != 0) {
        msgNo = 0x2F67;
    } else if (bs1m_isEventBit(0x1F08)) {
        if (bs1m_isEventBit(0x2040)) {
            if (m838 == 1) {
                msgNo = 0x2F60;
            } else {
                msgNo = 0x2F61;
            }
        } else {
            bs1m_onEventBit(0x2040);
            msgNo = 0x2F5F;
            m838 = 1;
        }
    } else {
        switch (bs1m_getEventReg(0xBB07)) {
        case 0:
            if (m836 != 0 || bs1m_isEventBit(0x1F10)) {
                msgNo = 0x2F46;
                break;
            }
            bs1m_onEventBit(0x1F10);
            msgNo = 0x2F45;
            break;
        case 1: msgNo = m836 != 0 ? 0x2F46 : 0x2F58; break;
        case 2: msgNo = m836 != 0 ? 0x2F46 : 0x2F59; break;
        case 3: msgNo = m836 != 0 ? 0x2F46 : 0x2F5A; break;
        case 4: msgNo = m836 != 0 ? 0x2F46 : 0x2F5B; break;
        case 5: msgNo = m836 != 0 ? 0x2F46 : 0x2F5C; break;
        case 6: msgNo = m836 != 0 ? 0x2F69 : 0x2F5D; break;
        case 7: msgNo = m836 != 0 ? 0x2F6A : 0x2F5E; break;
        default:
            /* HD: an assertion (line 0x5D5) instead of an uninitialised message number */
            JUT_ASSERT_fail(STR(0x100185E4), 0x5D5, STR(0x100185D0));
            msgNo = 0;
            break;
        }
        if (bs1m_isEventBit(0x1F10)) {
            bs1m_onEventBit(0x1F10);
        }
        m836 = 1;
    }
    return msgNo;
}
VERIFY(0x022157C8, &daNpc_Bs1_c::getMsg);

/* the callers pass next_msgStatus's whole r3 on (the status is a u16 in the source) */
static inline u32 bs1m_nextMsgStatus(daNpc_Bs1_c* a, be<u32>* p, be<u32>* p2) { return gabi::call<u32>(0x02214858, a, p, p2); }

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline): m24 + normalize(m18 - m24) * 100, copied
 * through the FPRs into *out */
static inline void bs1m_getItemZoomPos(ShopCam_action_c_l* cam, cXyz* out) {
    gabi::Local<cXyz> dir;
    cXyz_mi(&cam->m18, dir.get(), &cam->m24);
    if (!bs1m_normalizeRS(dir.get())) {
        dir->x = 0.0f;
        dir->z = 1.0f;
        dir->y = 0.0f;
    }
    gabi::Local<cXyz> scaled;
    cXyz_ml(dir.get(), scaled.get(), 100.0f);
    gabi::Local<cXyz> zoom;
    cXyz_pl(&cam->m24, zoom.get(), scaled.get());
    f32 x = zoom->x, y = zoom->y, z = zoom->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

/* 02215DA0 */
u16 daNpc_Bs1_c::normal_talk() {
    WWHD_FUNC(0x02215DA0, u16, this);
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
    mpShopCursor->mbShow = 0; /* hide() */
    return status;
}
VERIFY(0x02215DA0, &daNpc_Bs1_c::normal_talk);

/* 02215F2C */
u16 daNpc_Bs1_c::shop_talk() {
    WWHD_FUNC(0x02215F2C, u16, this);
    u32 mgr = bs1m_mgr();
    mpShopCursor->mbShow = 1; /* show() */
    if (bs1m_nowTriggercheck(&mStickControl, &mShopItems, &m738, BS1M_getDefaultMsgCB, this)) {
        m708 = 1;
        m73C = 0;
        if (m738 == 0xF3F) {
            m835 = 2;
        }
    }
    u16 status = (u16)bs1m_getStatus(mgr);
    if (status == 0xE || status == 0xF) {
        if (m708 != 0 || gabi::load<u8>(mgr + 0x922) != 0) { /* HD: also after a cancel */
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
VERIFY(0x02215F2C, &daNpc_Bs1_c::shop_talk);

/* 02216154 */
u16 daNpc_Bs1_c::talk() {
    WWHD_FUNC(0x02216154, u16, this);
    u32 mgr = bs1m_mgr();
    u16 status = 0xFF;
    if (m835 == 0) {
        bs1m_msgId() = 0xFFFFFFFF;
        m738 = getMsg();
        m73C = 0;
        m835 = 1;
    } else if (m835 != -1) {
        if (bs1m_msgId() == 0xFFFFFFFF) {
            if (bs1m_chkTalkXY(bs1m_play_u8(0x52B0)) && !bs1m_chkPresentEnd()) {
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
            if (bs1m_play_u8(0x5BD2) /* dComIfGp_checkMesgSendButton() */ != 0) {
                u32 msgNo = gabi::load<u32>(mgr + 0x938);
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
VERIFY(0x02216154, &daNpc_Bs1_c::talk);
