/**
 * d_a_npc_rsh1_msg.cpp (WWHD)
 * NPC - Zunari: messages, talk, shop talk, the "player leaves the shop" action.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_rsh1.h"

/* the cancel path of the shop messages (HD): both message numbers, the manager's cancel flag and the
 * play object's send/cancel flags */
static inline void rsh1m_cancelMsg(RshMessageManager* mng, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    mng->mCancel = 1;
    rsh1m_play_set_u8(0x5BD2, 1);
    rsh1m_play_set_u8(0x5BD3, 0);
}

/* the status is passed on unmasked: a 32-bit result in HD */

/* ShopCam_action_c::getItemZoomPos(f) (inline): m24 + normalize(m18 - m24) * f, copied through the FPRs */
/* inlined into normal_talk */
static inline void rsh1m_getItemZoomPos(RshShopCamera* cam, cXyz* out, f32 f) {
    cXyz dir;
    cXyz_mi(&cam->m18, &dir, &cam->m24);
    if (!rsh1m_normalizeRS(&dir)) {
        dir.x = 0.0f;
        dir.z = 1.0f;
        dir.y = 0.0f;
    }
    cXyz scaled;
    cXyz_ml(&dir, &scaled, f);
    cXyz zoom;
    cXyz_pl(&cam->m24, &zoom, &scaled);
    f32 x = zoom.x, y = zoom.y, z = zoom.z;
    out->x = x;
    out->y = y;
    out->z = z;
}

static inline void rsh1m_copyF(cXyz* dst, const cXyz* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

/* setAction(&daNpc_Rsh1_c::wait_action, NULL) (inline; HD: GHS pointer to member, no previous action) */

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

/* 022D8F58 */
u32 daNpc_Rsh1_c::next_msgStatus(be<u32>* o_pMsgNo, be<u32>* o_pMsgNo2) {
    daNpc_Rsh1_c* i_this = this;
    u32 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgNo = *o_pMsgNo;
    RshMessageManager* mng = rsh1_messageManager();
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
        // HD: a new message 0x2892 until event bit 0x0B80 is set
        if (!rsh1m_isEventBit(0xB80)) {
            *o_pMsgNo = 0x2892;
        } else {
            *o_pMsgNo = *o_pMsgNo + 1;
        }
        break;
    case 0x2892:
        // HD: sets bit 1 of the HD save flags and ends
        rsh1m_hdFlagOn(rsh1m_hdFlags(), 1);
        msg_status = 0x10;
        break;
    case 0x2895:
        // HD: goes on with the purchase offer and sets bit 2 of the HD save flags
        *o_pMsgNo = 0x2889;
        rsh1m_hdFlagOn(rsh1m_hdFlags(), 2);
        break;
    case 0x288C:
        *o_pMsgNo = 0x2889;
        break;
    case 0x2889:
        if (mng->mSelectNum == 0) {
            s32 msg_rupee = dComIfGp_getMessageRupee(); /* dComIfGp_getMessageRupee() */
            if (msg_rupee > (s32)dComIfGs_getRupee()) { /* dComIfGs_getRupee() */
                *o_pMsgNo = 0x288A;
                break;
            }
            dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            play->play.mItemRupeeCount = play->play.mItemRupeeCount - msg_rupee; /* setItemRupeeCount */
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
        if (mng->mSelectNum == 0) {
            *o_pMsgNo = 0x284D;
        } else {
            *o_pMsgNo = 0x284A;
        }
        break;
    case 0x2853:
        if (mng->mSelectNum == 0) {
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
        // HD: B cancels through the shared cancel path; otherwise no forced button status
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
            if (rsh1m_play_u8(0x5BD3) != 0 /* dComIfGp_checkMesgCancelButton() */ || mng->mSelectNum != 0) {
                *o_pMsgNo = *o_pMsgNo - 1;
                break;
            }
            s32 msg_rupee = dComIfGp_getMessageRupee();
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
            dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            play->play.mItemRupeeCount = play->play.mItemRupeeCount - msg_rupee;
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

/* 022D94B8 */
u32 daNpc_Rsh1_c::getMsg() {
    u32 msg_no = m780;
    if (msg_no != 0) {
        m780 = 0;
        return msg_no;
    }
    if (!rsh1m_checkItemGet(dItemNo_MAGIC_ARMOR_e, TRUE) && daNpc_Rsh1_countShop() >= 3) {
        return 0x285D;
    }
    // HD: item 0x77 (probably the HD Swift Sail) counts as the sail
    if (!rsh1m_checkGetItem(dItemNo_SAIL_e) && !rsh1m_checkGetItem(dItemNo_SWIFT_SAIL_e)) {
        if (rsh1m_isEventBit(0x2420)) {
            // HD: once the HD flags have bit 1 but not bit 2, new messages 0x2893/0x2894/0x2895
            u32 flags = rsh1m_hdFlags();
            if (rsh1m_hdFlagCheck(flags, 1) && !rsh1m_hdFlagCheck(flags, 2)) {
                if (rsh1m_isEventBit(0xB80)) {
                    return 0x2895;
                }
                return dComIfGs_getClearCount() != 0 ? 0x2894 : 0x2893; /* dComIfGs_getClearCount() */
            }
            return 0x288C;
        }
        rsh1m_onEventBit(0x2420);
        return 0x2886;
    }
    if (!rsh1m_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
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

/* 022D96E4 */
u16 daNpc_Rsh1_c::normal_talk() {
    RshMessageManager* mng = rsh1_messageManager();
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
        cXyz zoom_pos;
        rsh1m_getItemZoomPos(&mShopCamAct, &zoom_pos, 125.0f);
        mpShopItems->Item_ZoomUp(&zoom_pos);
    }
    mpShopCursor->hide();
    if (rsh1m_play_u8(0x5BD2) == 1) { /* dComIfGp_checkMesgSendButton() */
        m778 = mng->mMsgNo;
        /* function-local statics rel_cam_ctr_data (0x10468560, guard 0x1046860C) and
         * rel_cam_eye_data (0x10468578, guard 0x10468610) */
        cXyz* ctr = rsh1_statics().mTalkCenter;
        cXyz* eye = rsh1_statics().mTalkEye;
        if (rsh1_statics().mTalkCenterGuard == 0) {
            rsh1_statics().mTalkCenterGuard = 1;
            ctr[0].x = 96.0f;
            ctr[0].y = -46.0f;
            ctr[0].z = -26.0f;
            ctr[1].x = 2.0f;
            ctr[1].y = -32.0f;
            ctr[1].z = 29.0f;
        }
        if (rsh1_statics().mTalkEyeGuard == 0) {
            rsh1_statics().mTalkEyeGuard = 1;
            eye[0].x = -156.0f;
            eye[0].y = -16.0f;
            eye[0].z = 166.0f;
            eye[1].x = 144.0f;
            eye[1].y = 19.0f;
            eye[1].z = 215.0f;
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
        case 0x2892: // HD: Swift Sail dialogue
        case 0x2895: // HD: Swift Sail purchase offer
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
            cXyz c;
            cXyz e;
            rsh1m_copyF(&c, &ctr[sel]);
            rsh1m_copyF(&e, &eye[sel]);
            rsh1m_talkCamInit(&mShopCamAct, this, &c, &e, 45.0f);
        } else {
            /* !checkCamAction(NULL) && !checkCamAction(&ShopCam_action_c::shop_cam_action) */
            const ShopAction& cam = mShopCamAct.mCurrActionFunc;
            s16 i = cam.mVtableIndex;
            if (i != 0) {
                if (!(i == -1 && cam.mThisDelta == 0 && cam.mFunction == RSH1_SHOP_CAM_ACTION)) {
                    mShopCamAct.shop_cam_action_init();
                }
            }
        }
    }
    return status;
}

/* 022D9ACC */
u16 daNpc_Rsh1_c::shop_talk() {
    RshMessageManager* mng = rsh1_messageManager();
    mpShopCursor->show();
    if (mpShopItems && rsh1m_now_triggercheck(&mSTControl, gabi::ea(mpShopItems.get()), &m778, 0, 0)) {
        m77C = 0;
        m749 = 1;
    }
    u16 status = rsh1m_msgGetStatus(mng);
    if (status == 0xE || status == 0xF) {
        // HD: also skipped when the message manager's cancel flag is set
        if (m749 != 0 || mng->mCancel != 0) {
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

/* 022D9C18 */
u16 daNpc_Rsh1_c::talk() {
    RshMessageManager* mng = rsh1_messageManager();
    u16 status = 0xFF;
    if (m961 == 0) {
        rsh1_messageId() = 0xFFFFFFFF; // HD: no l_msg
        m778 = getMsg();
        m77C = 0;
        m961 = 1;
        mShopSelectItemNo = 0xFF;
    } else if (m961 != -1) {
        if (rsh1_messageId() == 0xFFFFFFFF) {
            u32 id = rsh1m_msgSet(mng, m778, &eyePos);
            rsh1_messageId() = id;
            // HD: the talk step is chosen as soon as the message is set
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
                u32 msgNo = mng->mMsgNo;
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

/* 022DAA78 */
BOOL daNpc_Rsh1_c::pl_shop_out_action(void*) {
    f32 trig_amplitude_1 = 140.0f;
    if (mActionStatus == 0) {
        setAnm(6);
        rsh1m_setShopOutPos(this, trig_amplitude_1);
        mActionStatus = (s8)(mActionStatus + 1);
        m793 = 0;
    } else if (mActionStatus != -1) {
        // HD: when the global byte 0x101D5F45 is set, Zunari goes straight back to waiting
        if (rsh1_forceWait()) {
            setAnm(0);
            rsh1m_setAction(this, RSH1_wait_action);
            return TRUE;
        }
        gabi::Local<be<s32>> temp_78C;
        *temp_78C = (s32)m78C;
        fopAc_ac_c* link_p = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
        cXyz link_pos;
        rsh1m_copyF(&link_pos, &link_p->current.pos);
        s16 target = cLib_targetAngleY(&current.pos, &link_pos);
        if (pathMove(temp_78C.get())) {
            cXyz temp;
            rsh1m_copyF(&temp, &cXyz::Zero); /* cXyz::Zero */
            f32 calc_pos = rsh1m_addCalcPos(&m794, &current.pos, 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
            cLib_addCalcAngleS2(&current.angle.y, target, 4, 0x1000);
            if (calc_pos < 1.0f) {
                rsh1m_addCalcPos(&m7A0, &temp, 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
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
                temp.x = gabi::fmadds(trig_amplitude_2, cM_ssin(ang), temp.x);
                temp.z = gabi::fmadds(trig_amplitude_2, cM_scos(ang), temp.z);
                rsh1m_addCalcPos(&m7A0, &temp, 0.25f, REG_F(10, 1) + 5.0f, 1.0f);
            }
        } else {
            rsh1m_setShopOutPos(this, trig_amplitude_1);
        }
    }
    return TRUE;
}

/* WWHD glue

*/
