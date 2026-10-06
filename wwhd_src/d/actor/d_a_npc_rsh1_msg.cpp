/**
 * d_a_npc_rsh1_msg.cpp (WWHD)
 * NPC - Zunari: messages, talk, shop talk, the "player leaves the shop" action.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_rsh1.h"

/* ---- local bindings (SHARED-CANDIDATE; same as d_a_npc_bms1.h) ---- */
static inline u32 rsh1m_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 rsh1m_msgGetStatus(u32 mng) { return gabi::call<u16>(0x025F795C, mng); }
static inline void rsh1m_msgSetStatus(u32 mng, u32 st) { gabi::call(0x025F74D0, mng, st); }
static inline u32 rsh1m_msgSet(u32 mng, u32 msgNo, void* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
static inline void rsh1m_demoMsgFlagOn() { gabi::call(0x025DB58C); }
static inline BOOL rsh1m_CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline u8 rsh1m_play_u8(u32 off) { return gabi::load<u8>(dComIfGp_ea() + off); }
static inline void rsh1m_play_set_u8(u32 off, u8 v) { gabi::store<u8>(dComIfGp_ea() + off, v); }
static inline u32 rsh1m_save() { return gabi::load<u32>(0x101F84DC); }
static inline BOOL rsh1m_isEventBit(u16 f) { return gabi::call<BOOL>(0x025B8B94, rsh1m_save() + 0x644, f); }
static inline void rsh1m_onEventBit(u16 f) { gabi::call(0x025B8B68, rsh1m_save() + 0x644, f); }
static inline BOOL rsh1m_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline BOOL rsh1m_checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
static inline void rsh1m_execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
static inline s32 rsh1m_BoughtErrorStatus(u32 items, s32 a, s32 price) { return gabi::call<s32>(0x025BBA1C, items, a, price); }
static inline BOOL rsh1m_now_triggercheck(void* stick, u32 items, be<u32>* msg, u32 cb, u32 cbArg) {
    return gabi::call<BOOL>(0x025BB698, stick, items, msg, cb, cbArg);
}
static inline void rsh1m_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }
static inline BOOL rsh1m_checkReserveItem(u8 item) { return gabi::call<BOOL>(0x025B7570, rsh1m_save() + 0x96, item); }
static inline BOOL rsh1m_normalizeRS(cXyz* v) { return gabi::call<BOOL>(0x0201B47C, v); }
static inline f32 rsh1m_addCalcPos(cXyz* pos, cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200EE00, pos, target, scale, maxStep, minStep);
}
/* HD-only save flags: *(save + 0x12C0) + 0x8C (02720118), a 32-bit word; 0271F954 tests, 0271F968 sets */
static inline u32 rsh1m_hdFlags() { return gabi::call<u32>(0x02720118, rsh1m_save() + 0x12C0); }
static inline BOOL rsh1m_hdFlagCheck(u32 p, u32 bit) { return gabi::call<BOOL>(0x0271F954, p, bit); }
static inline void rsh1m_hdFlagOn(u32 p, u32 bit) { gabi::call(0x0271F968, p, bit); }
/* ShopCam_action_c::rsh_talk_cam_action_init(actor, cXyz ctr, cXyz eye, f32 fovy) (cXyz by value) */
static inline void rsh1m_talkCamInit(ShopCam_action_c_l* cam, fopAc_ac_c* a, cXyz* ctr, cXyz* eye, f32 fovy) {
    gabi::call(0x025BC1B0, cam, a, ctr, eye, fovy);
}

#define rsh1m_l_msgId (*gabi::at<be<u32>>(0x104684BC))
#define RSH1_next_msgStatus 0x022D8F58u

/* the cancel path of the shop messages (HD): both message numbers, the manager's cancel flag and the
 * play object's send/cancel flags */
static inline void rsh1m_cancelMsg(u32 mng, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    gabi::store<u8>(mng + 0x922, 1);
    rsh1m_play_set_u8(0x5BD2, 1);
    rsh1m_play_set_u8(0x5BD3, 0);
}

/* 022D8F58 (HD: a second message number pointer, may be NULL; the status is returned as a
 * full register, which the callers pass on) */
u32 daNpc_Rsh1_c::next_msgStatus(be<u32>* o_pMsgNo, be<u32>* o_pMsgNo2) {
    WWHD_FUNC(0x022D8F58, u32, this, o_pMsgNo, o_pMsgNo2);
    daNpc_Rsh1_c* i_this = this;
    u32 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgNo = *o_pMsgNo;
    u32 mng = rsh1m_msgMng();
    switch (msgNo) {
    case 0x2845:
    case 0x2846:
    case 0x2847:
    case 0x2848:
    case 0x284B:
    case 0x284D:
    case 0x284E:
    case 0x284F:
    case 0x2850:
    case 0x2851:
    case 0x2852:
    case 0x2855:
    case 0x2857:
    case 0x2859:
    case 0x285A:
    case 0x285D:
    case 0x285E:
    case 0x2887:
    case 0x2888:
    case 0x288D:
        *o_pMsgNo = msgNo + 1;
        break;
    case 0x2886:
        /* HD: a new message 0x2892 until event bit 0x0B80 is set */
        if (!rsh1m_isEventBit(0xB80)) {
            *o_pMsgNo = 0x2892;
        } else {
            *o_pMsgNo = *o_pMsgNo + 1;
        }
        break;
    case 0x2892:
        /* HD: sets bit 1 of the HD save flags and ends */
        rsh1m_hdFlagOn(rsh1m_hdFlags(), 1);
        msg_status = 0x10;
        break;
    case 0x2895:
        /* HD: goes on with the purchase offer and sets bit 2 of the HD save flags */
        *o_pMsgNo = 0x2889;
        rsh1m_hdFlagOn(rsh1m_hdFlags(), 2);
        break;
    case 0x288C:
        *o_pMsgNo = 0x2889;
        break;
    case 0x2889:
        if (gabi::load<u32>(mng + 0x948) == 0) {
            s32 msg_rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4); /* dComIfGp_getMessageRupee() */
            if (msg_rupee > (s32)gabi::load<u16>(rsh1m_save() + 0x24)) { /* dComIfGs_getRupee() */
                *o_pMsgNo = 0x288A;
                break;
            }
            u32 play = dComIfGp_ea();
            gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - msg_rupee); /* setItemRupeeCount */
            *o_pMsgNo = 0x288D;
            break;
        }
        *o_pMsgNo = 0x288B;
        break;
    case 0x2862:
        if (!i_this->shopPosMove()) {
            msg_status = 0xE; /* fopMsgStts_MSG_DISPLAYED_e */
        } else {
            i_this->setAnm(0);
            *o_pMsgNo = *o_pMsgNo + 1;
        }
        break;
    case 0x283D:
    case 0x283F:
    case 0x2841:
    case 0x2843:
        *o_pMsgNo = msgNo - 1;
        break;
    case 0x284C:
        if (gabi::load<u32>(mng + 0x948) == 0) {
            *o_pMsgNo = 0x284D;
        } else {
            *o_pMsgNo = 0x284A;
        }
        break;
    case 0x2853:
        if (gabi::load<u32>(mng + 0x948) == 0) {
            *o_pMsgNo = 0x2855;
        } else {
            *o_pMsgNo = 0x2854;
        }
        break;
    case 0x2854:
        *o_pMsgNo = 0x284E;
        break;
    case 0x2866:
        *o_pMsgNo = 0x2863;
        break;
    case 0x2863:
        /* HD: B cancels through the shared cancel path; otherwise no forced button status */
        if (rsh1m_CPad_CHECK_TRIG_B(0)) {
            rsh1m_cancelMsg(mng, o_pMsgNo, o_pMsgNo2, 0x2867);
        } else {
            msg_status = 0xE;
        }
        break;
    case 0x286B:
    case 0x286D:
    case 0x286F:
    case 0x2871:
    case 0x2873:
    case 0x2875:
    case 0x2877:
    case 0x2879:
    case 0x287B:
    case 0x287D:
    case 0x287F:
    case 0x2881:
        if (rsh1m_CPad_CHECK_TRIG_B(0)) {
            rsh1m_cancelMsg(mng, o_pMsgNo, o_pMsgNo2, 0x2867);
        } else {
            *o_pMsgNo = *o_pMsgNo + 1;
        }
        break;
    case 0x2868:
    case 0x2869:
    case 0x286A:
    case 0x2884:
        if (i_this->mpShopItems) {
            i_this->mpShopItems->mSelectedItemIdx = -1;
        }
        *o_pMsgNo = 0x2863;
        break;
    case 0x286C:
    case 0x286E:
    case 0x2870:
    case 0x2872:
    case 0x2874:
    case 0x2876:
    case 0x2878:
    case 0x287A:
    case 0x287C:
    case 0x287E:
    case 0x2880:
    case 0x2882:
        if (i_this->mpShopItems) {
            if (rsh1m_play_u8(0x5BD3) != 0 /* dComIfGp_checkMesgCancelButton() */ || gabi::load<u32>(mng + 0x948) != 0) {
                *o_pMsgNo = *o_pMsgNo - 1;
                break;
            }
            s32 msg_rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
            s32 status = rsh1m_BoughtErrorStatus(gabi::ea(i_this->mpShopItems.get()), 0, msg_rupee);
            if (status & 0x20) {
                *o_pMsgNo = 0x2868;
                break;
            }
            if (status & 0x4) {
                *o_pMsgNo = 0x2869;
                break;
            }
            /* daNpc_Rsh1_RotenItemNumInBag() (inline): reserved items 0x8C..0x97 */
            s32 num = 0;
            u8 item = 0x8C;
            for (s32 i = 12; i != 0; i--) {
                num += rsh1m_checkReserveItem(item);
                item = (u8)(item + 1);
            }
            if (num >= 3) {
                *o_pMsgNo = 0x2884;
                break;
            }
            rsh1m_seStart(0x87F /* JA_SE_SHOP_BOUGHT */, &i_this->eyePos, 0, dComIfGp_getReverb(i_this->current.roomNo));
            i_this->mpShopItems->hideSelectItem();
            i_this->mShopSelectItemNo = i_this->mpShopItems->getSelectItemNo();
            u32 play = dComIfGp_ea();
            gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - msg_rupee);
            if (!rsh1m_checkItemGet(i_this->mpShopItems->getSelectItemNo(), TRUE)) {
                i_this->m95B = 4;
                msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
                break;
            }
            rsh1m_execItemGet(i_this->mpShopItems->getSelectItemNo());
            *o_pMsgNo = 0x286A;
        }
        break;
    case 0x2849:
        rsh1m_onEventBit(0x1110);
        msg_status = 0x10;
        break;
    case 0x2858:
        rsh1m_onEventBit(0x1108);
        msg_status = 0x10;
        break;
    default:
        msg_status = 0x10;
        break;
    }
    return msg_status;
}
VERIFY(0x022D8F58, &daNpc_Rsh1_c::next_msgStatus);

/* the status is passed on unmasked: a 32-bit result in HD */
static inline u32 rsh1m_next_msgStatus(daNpc_Rsh1_c* a, be<u32>* p, be<u32>* p2) {
    return gabi::call<u32>(RSH1_next_msgStatus, a, p, p2);
}

/* 022D94B8 */
u32 daNpc_Rsh1_c::getMsg() {
    WWHD_FUNC(0x022D94B8, u32, this);
    u32 msg_no = m780;
    if (msg_no != 0) {
        m780 = 0;
        return msg_no;
    }
    if (!rsh1m_checkItemGet(0x2A /* dItemNo_MAGIC_ARMOR_e */, TRUE) && daNpc_Rsh1_countShop() >= 3) {
        return 0x285D;
    }
    /* HD: item 0x77 (probably the HD Swift Sail) counts as the sail */
    if (!rsh1m_checkGetItem(0x78 /* dItemNo_SAIL_e */) && !rsh1m_checkGetItem(0x77)) {
        if (rsh1m_isEventBit(0x2420)) {
            /* HD: once the HD flags have bit 1 but not bit 2, new messages 0x2893/0x2894/0x2895 */
            u32 flags = rsh1m_hdFlags();
            if (rsh1m_hdFlagCheck(flags, 1) && !rsh1m_hdFlagCheck(flags, 2)) {
                if (rsh1m_isEventBit(0xB80)) {
                    return 0x2895;
                }
                return gabi::load<u8>(rsh1m_save() + 0x1C0) != 0 ? 0x2894 : 0x2893; /* dComIfGs_getClearCount() */
            }
            return 0x288C;
        }
        rsh1m_onEventBit(0x2420);
        return 0x2886;
    }
    if (!rsh1m_checkGetItem(0x30 /* dItemNo_DELIVERY_BAG_e */)) {
        return 0x2890;
    }
    if (mShopIdx == -1) {
        if (rsh1m_isEventBit(0x2D01) && !rsh1m_isEventBit(0xE08) && rsh1m_isEventBit(0x1108)) {
            return 0x2859;
        }
        int shop_cnt = daNpc_Rsh1_countShop();
        if (shop_cnt >= 8) {
            return 0x283E;
        }
        if (shop_cnt > 1) {
            return 0x2840;
        }
        if (rsh1m_isEventBit(0x1108)) {
            return 0x2842;
        }
        if (!rsh1m_isEventBit(0x1110)) {
            msg_no = 0x2845;
        } else {
            msg_no = 0x284B;
        }
    } else {
        if (!rsh1m_isEventBit(0x1108)) {
            msg_no = 0x2865;
        } else {
            msg_no = 0x2862;
        }
    }
    return msg_no;
}
VERIFY(0x022D94B8, &daNpc_Rsh1_c::getMsg);

/* ShopCam_action_c::getItemZoomPos(f) (inline): m24 + normalize(m18 - m24) * f, copied through the FPRs */
static inline void rsh1m_getItemZoomPos(ShopCam_action_c_l* cam, cXyz* out, f32 f) {
    gabi::Local<cXyz> dir;
    cXyz_mi(&cam->m18, dir.get(), &cam->m24);
    if (!rsh1m_normalizeRS(dir.get())) {
        dir->x = 0.0f;
        dir->z = 1.0f;
        dir->y = 0.0f;
    }
    gabi::Local<cXyz> scaled;
    cXyz_ml(dir.get(), scaled.get(), f);
    gabi::Local<cXyz> zoom;
    cXyz_pl(&cam->m24, zoom.get(), scaled.get());
    f32 x = zoom->x, y = zoom->y, z = zoom->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

static inline void rsh1m_copyF(cXyz* dst, u32 src) {
    dst->x = gabi::load<f32>(src);
    dst->y = gabi::load<f32>(src + 4);
    dst->z = gabi::load<f32>(src + 8);
}

/* 022D96E4 */
u16 daNpc_Rsh1_c::normal_talk() {
    WWHD_FUNC(0x022D96E4, u16, this);
    u32 mng = rsh1m_msgMng();
    u16 status = rsh1m_msgGetStatus(mng);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        rsh1m_msgSetStatus(mng, rsh1m_next_msgStatus(this, &m778, nullptr));
        if (rsh1m_msgGetStatus(mng) == 0xF) {
            rsh1m_msgSet(mng, m778, nullptr);
        }
    } else if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        rsh1m_msgSetStatus(mng, 0x13);
    } else if (status == 1 /* fopMsgStts_MSG_PREPARING_e */) {
        rsh1m_demoMsgFlagOn();
    }
    if (mpShopItems) {
        gabi::Local<cXyz> zoom_pos;
        rsh1m_getItemZoomPos(&mShopCamAct, zoom_pos.get(), 125.0f);
        mpShopItems->Item_ZoomUp(zoom_pos.get());
    }
    mpShopCursor->hide();
    if (rsh1m_play_u8(0x5BD2) == 1) { /* dComIfGp_checkMesgSendButton() */
        m778 = gabi::load<u32>(mng + 0x938);
        /* function-local statics rel_cam_ctr_data (0x10468560, guard 0x1046860C) and
         * rel_cam_eye_data (0x10468578, guard 0x10468610) */
        const u32 ctr = 0x10468560, eye = 0x10468578;
        if (gabi::load<u32>(0x1046860C) == 0) {
            gabi::store<u32>(0x1046860C, 1);
            gabi::store<f32>(ctr + 0x00, 96.0f);
            gabi::store<f32>(ctr + 0x04, -46.0f);
            gabi::store<f32>(ctr + 0x08, -26.0f);
            gabi::store<f32>(ctr + 0x0C, 2.0f);
            gabi::store<f32>(ctr + 0x10, -32.0f);
            gabi::store<f32>(ctr + 0x14, 29.0f);
        }
        if (gabi::load<u32>(0x10468610) == 0) {
            gabi::store<u32>(0x10468610, 1);
            gabi::store<f32>(eye + 0x00, -156.0f);
            gabi::store<f32>(eye + 0x04, -16.0f);
            gabi::store<f32>(eye + 0x08, 166.0f);
            gabi::store<f32>(eye + 0x0C, 144.0f);
            gabi::store<f32>(eye + 0x10, 19.0f);
            gabi::store<f32>(eye + 0x14, 215.0f);
        }
        s32 sel;
        switch ((u32)m778) {
        case 0x2846:
        case 0x284E:
        case 0x2858:
        case 0x285A:
        case 0x285E:
        case 0x2887:
        case 0x2888:
        case 0x2892: /* HD */
        case 0x2895: /* HD */
            sel = 0;
            break;
        case 0x2848:
        case 0x2851:
        case 0x2852:
        case 0x288D:
            sel = 1;
            break;
        default:
            sel = -1;
            break;
        }
        if (sel >= 0) {
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> e;
            rsh1m_copyF(c.get(), ctr + sel * 0xC);
            rsh1m_copyF(e.get(), eye + sel * 0xC);
            rsh1m_talkCamInit(&mShopCamAct, this, c.get(), e.get(), 45.0f);
        } else {
            /* !checkCamAction(NULL) && !checkCamAction(&ShopCam_action_c::shop_cam_action) */
            u32 cam = gabi::ea(&mShopCamAct);
            s16 i = gabi::load<s16>(cam + 2);
            if (i != 0) {
                if (!(i == -1 && gabi::load<s16>(cam + 0) == 0 && gabi::load<u32>(cam + 4) == 0x025BBF4C)) {
                    mShopCamAct.shop_cam_action_init();
                }
            }
        }
    }
    return status;
}
VERIFY(0x022D96E4, &daNpc_Rsh1_c::normal_talk);

/* 022D9ACC */
u16 daNpc_Rsh1_c::shop_talk() {
    WWHD_FUNC(0x022D9ACC, u16, this);
    u32 mng = rsh1m_msgMng();
    mpShopCursor->show();
    if (mpShopItems && rsh1m_now_triggercheck(&mSTControl, gabi::ea(mpShopItems.get()), &m778, 0, 0)) {
        m77C = 0;
        m749 = 1;
    }
    u16 status = rsh1m_msgGetStatus(mng);
    if (status == 0xE || status == 0xF) {
        /* HD: also skipped when the message manager's cancel flag is set */
        if (m749 != 0 || gabi::load<u8>(mng + 0x922) != 0) {
            m749 = 0;
        } else {
            m77C = m778;
            rsh1m_msgSetStatus(mng, rsh1m_next_msgStatus(this, &m77C, &m778));
            if (rsh1m_msgGetStatus(mng) == 0xF) {
                rsh1m_msgSet(mng, m77C, nullptr);
            }
        }
    } else if (status == 0x12) {
        rsh1m_msgSetStatus(mng, 0x13);
        if (mpShopItems) {
            mpShopItems->mSelectedItemIdx = -1;
        }
    } else if (status == 1) {
        rsh1m_demoMsgFlagOn();
    }
    return status;
}
VERIFY(0x022D9ACC, &daNpc_Rsh1_c::shop_talk);

/* 022D9C18 */
u16 daNpc_Rsh1_c::talk() {
    WWHD_FUNC(0x022D9C18, u16, this);
    u32 mng = rsh1m_msgMng();
    u16 status = 0xFF;
    if (m961 == 0) {
        rsh1m_l_msgId = 0xFFFFFFFF; /* HD: no l_msg */
        m778 = getMsg();
        m77C = 0;
        m961 = 1;
        mShopSelectItemNo = 0xFF;
    } else if (m961 != -1) {
        if (rsh1m_l_msgId == 0xFFFFFFFF) {
            u32 id = rsh1m_msgSet(mng, m778, &eyePos);
            rsh1m_l_msgId = id;
            /* HD: the talk step is chosen as soon as the message is set */
            if (id != 0xFFFFFFFF) {
                if (!daNpc_Rsh1_shopStickMoveMsgCheck(m778)) {
                    m961 = 2;
                } else {
                    m961 = 3;
                }
            }
        } else {
            setAnmFromMsgTag();
            s8 step = m961;
            if (step == 2) {
                status = normal_talk();
            } else if (step == 3) {
                status = shop_talk();
            }
            if (rsh1m_play_u8(0x5BD2) != 0) {
                u32 msgNo = gabi::load<u32>(mng + 0x938);
                m778 = msgNo;
                if (!daNpc_Rsh1_shopStickMoveMsgCheck(msgNo)) {
                    if (!daNpc_Rsh1_shopMsgCheck(msgNo) && mpShopItems) {
                        mpShopItems->mSelectedItemIdx = -1;
                        mpShopItems->showItem();
                    }
                    m961 = 2;
                } else {
                    if (msgNo == 0x2863 && mpShopItems) {
                        mpShopItems->mSelectedItemIdx = -1;
                        mpShopItems->showItem();
                    }
                    m961 = 3;
                }
            }
        }
    }
    if (mpShopItems) {
        mShopCamAct.m54 = mpShopItems->mSelectedItemIdx;
    }
    return status;
}
VERIFY(0x022D9C18, &daNpc_Rsh1_c::talk);

/* setAction(&daNpc_Rsh1_c::wait_action, NULL) (inline; HD: GHS pointer to member, no previous action) */
static inline void rsh1m_setAction(daNpc_Rsh1_c* a, u32 fn) {
    daNpc_Rsh1_c::ProcFunc_l* cur = &a->mCurrProc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn) {
        return;
    }
    if (i != 0) {
        u32 thisp = gabi::ea(a) + (s32)(s16)cur->d;
        s16 ci = cur->i;
        a->mActionStatus = -1;
        if (ci < 0) {
            gabi::call_ptr<BOOL>(cur->f, thisp, (u32)0);
        } else {
            u32 vt = gabi::load<u32>(thisp + (s32)gabi::load<s16>(gabi::ea(cur) + 6));
            gabi::call_ptr<BOOL>(gabi::load<u32>(vt + ci * 8 + 4), thisp, (u32)0);
        }
    }
    cur->d = 0;
    u32 thisp = gabi::ea(a) + (s32)(s16)cur->d;
    cur->i = -1;
    cur->f = fn;
    a->mActionStatus = 0;
    gabi::call_ptr<BOOL>(cur->f, thisp, (u32)0);
}

/* m794 = current.pos - (sin, 0, cos)(angle.y) * 140 */
static inline void rsh1m_setShopOutPos(daNpc_Rsh1_c* a, f32 amp) {
    u16 ang = (u16)a->current.angle.y;
    f32 x = a->current.pos.x;
    f32 y = a->current.pos.y;
    a->m794.x = x;
    f32 z = a->current.pos.z;
    a->m794.y = y;
    a->m794.z = z;
    a->m794.x = gabi::fnmsubs(amp, cM_ssin(ang), x);
    a->m794.z = gabi::fnmsubs(amp, cM_scos(ang), z);
}

/* 022DAA78 */
BOOL daNpc_Rsh1_c::pl_shop_out_action(void*) {
    WWHD_FUNC(0x022DAA78, BOOL, this, (u32)0);
    f32 trig_amplitude_1 = 140.0f;
    if (mActionStatus == 0) {
        setAnm(6);
        rsh1m_setShopOutPos(this, trig_amplitude_1);
        mActionStatus = (s8)(mActionStatus + 1);
        m793 = 0;
    } else if (mActionStatus != -1) {
        /* HD: when the global byte 0x101D5F45 is set, Zunari goes straight back to waiting */
        if (gabi::load<u8>(0x101D5F45) != 0) {
            setAnm(0);
            rsh1m_setAction(this, RSH1_wait_action);
            return TRUE;
        }
        gabi::Local<be<s32>> temp_78C;
        *temp_78C = (s32)m78C;
        fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); /* daPy_getPlayerLinkActorClass() */
        gabi::Local<cXyz> link_pos;
        rsh1m_copyF(link_pos.get(), gabi::ea(&link_p->current.pos));
        s16 target = cLib_targetAngleY(&current.pos, link_pos.get());
        if (pathMove(temp_78C.get())) {
            gabi::Local<cXyz> temp;
            rsh1m_copyF(temp.get(), 0x101FFBA8); /* cXyz::Zero */
            f32 calc_pos = rsh1m_addCalcPos(&m794, &current.pos, 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
            cLib_addCalcAngleS2(&current.angle.y, target, 4, 0x1000);
            if (calc_pos < 1.0f) {
                rsh1m_addCalcPos(&m7A0, temp.get(), 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
                f32 mag = std_sqrtf(PSVECSquareMag(&m7A0));
                if (mag <= 1.0f) {
                    setAnm(0);
                    rsh1m_setAction(this, RSH1_wait_action);
                } else {
                    setAnm(5);
                }
            } else {
                f32 trig_amplitude_2 = REG_F(10, 2) + 100.0f;
                u16 ang = (u16)current.angle.y;
                temp->x = gabi::fmadds(trig_amplitude_2, cM_ssin(ang), temp->x);
                temp->z = gabi::fmadds(trig_amplitude_2, cM_scos(ang), temp->z);
                rsh1m_addCalcPos(&m7A0, temp.get(), 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
            }
        } else {
            rsh1m_setShopOutPos(this, trig_amplitude_1);
        }
    }
    return TRUE;
}
VERIFY(0x022DAA78, &daNpc_Rsh1_c::pl_shop_out_action);
