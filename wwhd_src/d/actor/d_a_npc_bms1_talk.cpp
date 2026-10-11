/**
 * d_a_npc_bms1_talk.cpp (WWHD)
 * NPC - Bomb-Master Cannon: orders, messages and the shop, actions, event cuts.
 *
 * Written from the WWHD code (the GameCube TU is
 * "Nonmatching"); the structure follows d_a_npc_bs1 (Beedle). See d_a_npc_bms1.cpp.
 */
#include "d/actor/d_a_npc_bms1.h"

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline): m24 + normalize(m18 - m24) * 100, copied
 * through the FPRs into *out */
static inline void bms1_getItemZoomPos(Bms1ShopCam* cam, cXyz* out) {
    gabi::Local<cXyz> dir;
    cXyz_mi(&cam->m18, dir.get(), &cam->m24);
    if (!cXyz_normalizeRS_l(dir.get())) {
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

/* the B/cancel answer of the shop menus (inline): switch to the given message */
static inline u32 bms1_cancelMsg(u32 mng, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    bms1Message(mng)->mSendRequested = 1;
    bms1Play()->mMesgSendButton = 1;
    bms1Play()->mMesgCancelButton = 0;
    return fopMsgStts_MSG_CONTINUES_e;
}

/* 0220ABEC */
void daNpc_Bms1_c::setAnmFromMsgTag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType == 1) {
        return;
    }
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 0:
        setAnm(0, -1.0f);
        break;
    case 1:
        setAnm(1, -1.0f);
        break;
    case 2:
        setAnm(2, -1.0f);
        break;
    case 3:
        setAnm(3, -1.0f);
        break;
    case 4:
        setAnm(4, -1.0f);
        mAnimationRepeats = 1;
        break;
    case 5:
        setAnm(5, -1.0f);
        mAnimationRepeats = 3;
        break;
    case 6:
        setAnm(6, -1.0f);
        mAnimationRepeats = 3;
        break;
    }
    s8 anm = mAnmIdx;
    if (anm == 5) {
        if (McaMorf_checkEnd_l(mpMorf)) {
            s8 n = (s8)(mAnimationRepeats - 1);
            mAnimationRepeats = n;
            if (n <= 0) {
                setAnm(0, 13.0f);
            }
        }
    } else if (anm == 6 || anm == 4) {
        if (McaMorf_checkEnd_l(mpMorf)) {
            s8 n = (s8)(mAnimationRepeats - 1);
            mAnimationRepeats = n;
            if (n <= 0) {
                setAnm(1, -1.0f);
            }
        }
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}

/* 0220AEFC */
BOOL daNpc_Bms1_c::chkAttention(cXyz* i_pos, s16 i_angle) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 maxAttnDistXZ = bms1HIO().mMaxAttnDistXZ;
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff.get(), i_pos);
    gabi::Local<cXyz> dir;
    s16 angY = current.angle.y;
    dir->x = cM_ssin(angY);
    dir->y = 0.0f;
    dir->z = cM_scos(angY);
    return maxAttnDistXZ > PSVECDotProduct(dir.get(), diff.get());
}

/* 0220A36C */
void daNpc_Bms1_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        if (mOrderEvt == BMS1_ORDER_LANDING) {
            mOrderEvt = BMS1_ORDER_NONE;
            mShopCam.Reset();
            setAction(PMF_event_action, nullptr);
        } else if (mOrderEvt == BMS1_ORDER_GET_ITEM) {
            mOrderEvt = BMS1_ORDER_NONE;
            mShopCam.Reset();
            setAction(PMF_getdemo_action, nullptr);
        }
    } else if (command == dEvtCmd_INTALK_e) {
        if (mOrderEvt == BMS1_ORDER_TALK || mOrderEvt == BMS1_ORDER_SHOP) {
            mOrderEvt = BMS1_ORDER_NONE;
            mTalkRequested = 1;
            talkInit();
            mShopCam.shop_cam_action_init();
            gabi::Local<cXyz> pos;
            pos->x = -70.0f;
            pos->y = 0.0f;
            pos->z = 150.0f;
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            bms1SetPlayerPosAndAngle(player, pos.get(), (s16)-0x7000); /* setPlayerPosAndAngle */
        }
    } else {
        mShopCam.Save();
    }
}

/* 0220AFB4 */
u32 daNpc_Bms1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mng = l_msgMng_l();
    u32 status = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    bms1Message(mng)->mSelectionChanged = 0;
    u32 msgNo = *pMsgNo;
    switch (msgNo) {
    case 0x2775:
    case 0x2777:
    case 0x2780:
    case 0x2782:
        *pMsgNo = msgNo + 1;
        break;
    case 0x2776:
        if (CPad_CHECK_TRIG_l(0)) {
            status = bms1_cancelMsg(mng, pMsgNo, pMsgNo2, 0x2779);
        } else {
            status = 0xE;
        }
        break;
    case 0x2778:
    case 0x2781:
        *pMsgNo = 0x2776;
        break;
    case 0x2783:
        if (CPad_CHECK_TRIG_l(0)) {
            status = bms1_cancelMsg(mng, pMsgNo, pMsgNo2, 0x2787);
        } else {
            status = 0xE;
        }
        break;
    case 0x2784:
        *pMsgNo = 0x2786;
        if (pMsgNo2 != nullptr) {
            *pMsgNo2 = 0x2786;
        }
        bms1Message(mng)->mSendRequested = 1;
        bms1Message(mng)->mSelectionChanged = 1;
        break;
    case 0x277A:
    case 0x277B:
    case 0x277C:
        if (CPad_CHECK_TRIG_l(0)) {
            status = bms1_cancelMsg(mng, pMsgNo, pMsgNo2, 0x2779);
        } else {
            *pMsgNo = *pMsgNo + 3;
        }
        break;
    case 0x2788:
    case 0x2789:
    case 0x278A:
        if (CPad_CHECK_TRIG_l(0)) {
            status = bms1_cancelMsg(mng, pMsgNo, pMsgNo2, 0x2787);
        } else {
            *pMsgNo = *pMsgNo + 3;
        }
        break;
    case 0x277D:
    case 0x277E:
    case 0x277F:
        if (bms1Play()->mMesgCancelButton == 0 && bms1Message(mng)->mChoice == 0) {
            *pMsgNo = 0x2780;
        } else {
            *pMsgNo = *pMsgNo - 3;
        }
        break;
    case 0x278B:
    case 0x278C:
    case 0x278D:
        if (bms1Play()->mMesgCancelButton == 0 && bms1Message(mng)->mChoice == 0) {
            s16 price = dComIfGp_getMessageRupee(); /* dComIfGp_getMessageRupee() */
            s32 err = dShop_BoughtErrorStatus_l(&mShopItems, 0, price);
            if (err & 0x20) {
                *pMsgNo = 0x278E;
            } else if (err & 0x4) {
                *pMsgNo = 0x278F;
            } else {
                mDoAud_seStart_l(0x87F, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
                mShopItems.hideSelectItem();
                mBoughtItemNo = mShopItems.getSelectItemNo();
                dComIfGp_setItemRupeeCount(-price); /* setItemRupeeCount(-price) */
                if (!checkItemGet(mShopItems.getSelectItemNo(), 1)) {
                    mOrderEvt = BMS1_ORDER_GET_ITEM;
                    status = fopMsgStts_MSG_ENDS_e;
                } else {
                    execItemGet(mShopItems.getSelectItemNo());
                    *pMsgNo = 0x2790;
                }
            }
        } else {
            *pMsgNo = *pMsgNo - 3;
        }
        break;
    case 0x2786:
    case 0x278E:
    case 0x278F:
    case 0x2790:
        *pMsgNo = 0x2783;
        break;
    default:
        status = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}

/* 0220B354 */
u32 daNpc_Bms1_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 msgNo = mNextMsgNo;
    if (msgNo != 0) {
        mNextMsgNo = 0;
    } else {
        msgNo = dComIfGs_isEventBit(0xA02) ? 0x2782 : 0x2775;
    }
    return msgNo;
}

/* 0220B3B0 */
u16 daNpc_Bms1_c::normal_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mng = l_msgMng_l();
    u16 status = fopMsgM_getStatus_l(mng);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo, nullptr));
        if (fopMsgM_getStatus_l(mng) == fopMsgStts_MSG_CONTINUES_e) {
            fopMsgM_messageSet_l(mng, mMsgNo, nullptr);
        }
    } else if (status == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        fopMsgM_setStatus_l(mng, 0x13);
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn_l();
    }
    gabi::Local<cXyz> pos;
    bms1_getItemZoomPos(&mShopCam, pos.get());
    mShopItems.Item_ZoomUp(pos.get());
    mpShopCursor->hide();
    if (bms1Play()->mMesgSendButton != 0) {
        mMsgNo = bms1Message(mng)->mMessageNo;
    }
    return status;
}

/* 0220B554 */
u16 daNpc_Bms1_c::shop_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mng = l_msgMng_l();
    mpShopCursor->show();
    if (dShop_now_triggercheck_l(&mStickControl, &mShopItems, &mMsgNo, 0, nullptr)) {
        mTriggerHandled = 1;
        mMsgNo2 = 0;
        if (mShopItems.mSelectedItemIdx >= 0 && !checkItemGet(0x69, 1)) {
            mHeadAnm.swing_vertical_init(1, 0x1800, 0x1000, 1);
        }
    }
    u16 status = fopMsgM_getStatus_l(mng);
    if (status == 0xE || status == fopMsgStts_MSG_CONTINUES_e) {
        if (mTriggerHandled != 0 || bms1Message(mng)->mSendRequested != 0) {
            mTriggerHandled = 0;
        } else {
            mMsgNo2 = mMsgNo;
            fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo2, &mMsgNo));
            if (fopMsgM_getStatus_l(mng) == fopMsgStts_MSG_CONTINUES_e) {
                fopMsgM_messageSet_l(mng, mMsgNo2, nullptr);
            }
        }
    } else if (status == fopMsgStts_BOX_CLOSED_e) {
        fopMsgM_setStatus_l(mng, 0x13);
        mShopItems.mSelectedItemIdx = -1;
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn_l();
    }
    return status;
}

/* 0220B6C4 */
u16 daNpc_Bms1_c::talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mng = l_msgMng_l();
    u16 status = 0xFF;
    if (mTalkStep == 0) {
        bms1MessageId() = 0xFFFFFFFF;
        mMsgNo = getMsg();
        mTalkStep = 1;
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        mMsgNo2 = 0;
        mBoughtItemNo = 0xFF;
        return status;
    }
    if (mTalkStep != -1) {
        if (bms1MessageId() == 0xFFFFFFFF) {
            bms1MessageId() = fopMsgM_messageSet_l(mng, mMsgNo, &eyePos);
            if (bms1MessageId() != 0xFFFFFFFF) {
                if (bms1ShopStickMessageCheck(mMsgNo)) {
                    mTalkStep = 3;
                } else {
                    mTalkStep = 2;
                }
            }
        } else {
            setAnmFromMsgTag();
            s8 step = mTalkStep;
            if (step == 2) {
                status = normal_talk();
            } else if (step == 3) {
                status = shop_talk();
            }
            if (bms1Play()->mMesgSendButton != 0) {
                u32 msgNo = bms1Message(mng)->mMessageNo;
                mMsgNo = msgNo;
                if (!bms1ShopStickMessageCheck(msgNo)) {
                    if (!bms1ShopMessageCheck(msgNo)) {
                        mShopItems.mSelectedItemIdx = -1;
                        mShopItems.showItem();
                    }
                    mTalkStep = 2;
                } else {
                    if (msgNo == 0x2776 || msgNo == 0x2783) {
                        mShopItems.mSelectedItemIdx = -1;
                        mShopItems.showItem();
                    }
                    mTalkStep = 3;
                }
            }
        }
    }
    mShopCam.m54 = mShopItems.mSelectedItemIdx;
    return status;
}

/* 0220B88C */
void daNpc_Bms1_c::setAttention(bool i_force) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!i_force && mAttentionMode >= 2) {
        return;
    }
    f32 z = mAttnBasePos.z;
    f32 y = mAttnBasePos.y + bms1HIO().mAttnYOffset;
    f32 x = mAttnBasePos.x;
    attention_info.position.z = z; /* attention_info.position */
    attention_info.position.x = x;
    attention_info.position.y = y;
}

/* 0220BB98 */
BOOL daNpc_Bms1_c::checkPlayerLanding() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 playerY = player->current.pos.y;
    BOOL ret = FALSE;
    if (mPlayerHeight - playerY > 200.0f) {
        if (!bms1PlayerAirborne(player)) {
            ret = TRUE;
        }
    }
    if (!bms1PlayerAirborne(player)) {
        mPlayerHeight = playerY;
    }
    return ret;
}

/* 0220B8C8 */
void daNpc_Bms1_c::lookBack() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> target;
    cXyz* dstPos = nullptr;
    f32 ex = 0.0f, ey = 0.0f, ez = 0.0f;
    s16 desiredYRot = current.angle.y;
    s8 mode = mMode;
    if (mode == 1) {
        if (mPlayerInRange != 0) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye.get(), bms1HIO().m04);
            ex = current.pos.x;
            ez = current.pos.z;
            dstPos = target.get();
            target->copy(*eye);
            ey = eyePos.y;
        }
    } else if (mode == 2) {
        if (mShopItems.mSelectedItemIdx == -1) {
            bms1_getItemZoomPos(&mShopCam, target.get());
        } else {
            gabi::Local<cXyz> basePos;
            mShopItems.getSelectItemBasePos(basePos.get());
            gabi::Local<cXyz> itemPos;
            mShopItems.getSelectItemPos(itemPos.get());
            target->copy(*itemPos);
            mpShopCursor->setPos(basePos.get());
            daNpc_Bms1_HIO_c& hio = bms1HIO();
            mpShopCursor->setScale(hio.mCursorScale[0], hio.mCursorScale[1], hio.mCursorScale[2],
                                   hio.mCursorScale[3], hio.mCursorScale[4]);
            mpShopCursor->anm_play();
        }
        ex = current.pos.x;
        ey = eyePos.y;
        ez = current.pos.z;
        dstPos = target.get();
    }
    s16 vel;
    if (mJntCtrl.mbTrn != 0) {
        cLib_addCalcAngleS2(&mHeadTurnVel, bms1HIO().mMaxHeadTurnVel, 4, 0x800);
        vel = mHeadTurnVel;
    } else {
        mHeadTurnVel = 0;
        vel = 0;
    }
    gabi::Local<cXyz> eyeCopy;
    eyeCopy->x = ex;
    eyeCopy->y = ey;
    eyeCopy->z = ez;
    dNpc_JntCtrl_lookAtTarget_l(&mJntCtrl, &current.angle.y, dstPos, eyeCopy.get(), desiredYRot, vel, 1);
}

/* 0220BC54 */
BOOL daNpc_Bms1_c::wait01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mTalkRequested != 0) {
        s8 mode = mMode;
        mMode = 2;
        mPrevMode = mode;
        return McaMorf_curMorf_l(mpMorf) < 1.0f; /* mpMorf->isMorf() */
    }
    if (mPlayerInRange != 0) {
        mOrderEvt = (mType == 1) ? 0 : 2;
    }
    return McaMorf_curMorf_l(mpMorf) < 1.0f;
}

/* 0220BCD8 */
BOOL daNpc_Bms1_c::talk01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 status = talk();
    if (status == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        mMode = mPrevMode;
        dComIfGp_event_reset();
        mShopCam.Reset();
        daPy_offNoDraw_l(player);
        mTalkRequested = 0;
    } else if (bms1ShopMessageCheck(mMsgNo) && status == 8) {
        mShopItems.getSelectItemBuyMsg();
    }
    if (mpShopCursor->m54 != 0) {
        dComIfGp_setDoStatusForce(0x17 /* dActStts_CHOOSE_e */);
        dComIfGp_setAStatusForce(0x27 /* dActStts_CANCEL_e */);
    }
    return McaMorf_curMorf_l(mpMorf) < 1.0f;
}

/* 0220BF90 */
BOOL daNpc_Bms1_c::getdemo_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Bms1" /* "Bms1" */, nullptr, 0);
    if (mActionStatus == 0) {
        daPy_offNoDraw_l(dComIfGp_getPlayer(0));
        mMode = mPrevMode;
        mShopCam.Reset();
        u8 itemNo = mShopItems.getSelectItemNo();
        u32 itemPID =
            fopAcM_createItemForPresentDemo_l(&current.pos, itemNo, 0, -1, current.roomNo, nullptr, nullptr);
        if (itemPID != 0xFFFFFFFF) {
            dComIfGp_event_setItemPartnerId(itemPID); /* dComIfGp_event_setItemPartnerId */
        }
        dComIfGp_evmng_cutEnd(staffIdx);
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (mActionStatus != -1) {
        fopMsgM_demoMsgFlagOn_l();
        dComIfGp_evmng_cutEnd(staffIdx);
        if (dComIfGp_evmng_endCheckOld_l("BMS_GET_DEMO")) {
            mOrderEvt = BMS1_ORDER_TALK;
            dComIfGp_event_reset();
            mNextMsgNo = 0x2790;
            setAction(PMF_wait_action, nullptr);
        }
    }
    return TRUE;
}

/* 0220BDCC */
BOOL daNpc_Bms1_c::wait_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == 0) {
        mMode = 1;
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (mActionStatus != -1) {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        pos->x = x;
        pos->y = y;
        pos->z = z;
        mPlayerInRange = (u8)chkAttention(
            pos.get(), (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]));
        if (mType == 1 && McaMorf_checkEnd_l(mpMorf) && bms1CalcTimer(&mAnimationRepeats) == 0) {
            s8 room = current.roomNo;
            mAnimationRepeats = 3;
            mDoAud_seStart_l(0x492E, &eyePos, 0, dComIfGp_getReverb(room));
        }
        s8 mode = mMode;
        if (mode == 1) {
            wait01();
        } else if (mode == 2) {
            talk01();
        }
        lookBack();
        if (checkPlayerLanding() && mType != 1) {
            mOrderEvt = BMS1_ORDER_LANDING;
        }
        setAttention(true);
    }
    return TRUE;
}

/* 0220C7CC */
BOOL daNpc_Bms1_c::event_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == 0) {
        if (mType != 1) {
            setAnm(dComIfGs_isEventBit(0xA02) ? 1 : 0, -1.0f);
        }
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (mActionStatus != -1) {
        privateCut();
        if (dComIfGp_evmng_endCheckOld_l("BMS_LAND_DEMO")) {
            mOrderEvt = BMS1_ORDER_NONE;
            dComIfGp_event_reset();
            setAction(PMF_wait_action, nullptr);
        }
    }
    return TRUE;
}

/* 0220C1C0 */
BOOL daNpc_Bms1_c::evn_talk_init(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "MsgNo" /* "MsgNo" */, 3);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "EndMsgNo" /* "EndMsgNo" */, 3);
    bms1MessageId() = 0xFFFFFFFF;
    if (pMsgNo != nullptr) {
        u32 msgNo = *pMsgNo;
        mMsgNo = msgNo;
        if (msgNo == 0x2791 && dComIfGs_isEventBit(0xA02)) {
            mMsgNo = 0x2792;
        }
    } else {
        mMsgNo = 0;
    }
    mEndMsgNo = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}

/* 0220C2A4 */
BOOL daNpc_Bms1_c::evn_continue_talk_init(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "EndMsgNo" /* "EndMsgNo" */, 3);
    mEndMsgNo = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}

/* 0220C394 */
BOOL daNpc_Bms1_c::evn_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mng = l_msgMng_l();
    if (bms1MessageId() == 0xFFFFFFFF) {
        bms1MessageId() = fopMsgM_messageSet_l(mng, mMsgNo, &eyePos);
        if (bms1MessageId() != 0xFFFFFFFF) {
            fopMsgM_demoMsgFlagOn_l();
        }
        return FALSE;
    }
    setAnmFromMsgTag();
    if (fopMsgM_getStatus_l(mng) == 0xE) {
        fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo, nullptr));
        if (fopMsgM_getStatus_l(mng) == fopMsgStts_MSG_CONTINUES_e) {
            fopMsgM_messageSet_l(mng, mMsgNo, nullptr);
        }
        return FALSE;
    }
    if (fopMsgM_getStatus_l(mng) == fopMsgStts_BOX_CLOSED_e) {
        u32 msgNo = mMsgNo;
        // HD: these bomb purchases adjust beast-item counters, preserving the original indices.
        if (msgNo == 0x1DCA) {
            u8 item = mBoughtBombItem;
            dComIfG_play_c* play;
            if (item == 0x49) {
                play = bms1Play();
                play->mItemBeastNumCounts[4] = (s16)(play->mItemBeastNumCounts[4] - 10);
            } else if (item == 0x4A) {
                play = bms1Play();
                play->mItemBeastNumCounts[5] = (s16)(play->mItemBeastNumCounts[5] - 20);
            } else {
                play = bms1Play();
                play->mItemBeastNumCounts[6] = (s16)(play->mItemBeastNumCounts[6] - 30);
            }
        } else if (msgNo == 0x1DC4) {
            u8 item = mBoughtBombItem;
            if (item == 0x49) {
                dComIfG_play_c* play = bms1Play();
                play->mItemBeastNumCounts[4] = (s16)(play->mItemBeastNumCounts[4] - 10);
            } else {
                s32 i = (item == 0x4A) ? 5 : 6;
                be<s16>& count = bms1Play()->mItemBeastNumCounts[i];
                count = (s16)(count - 10);
            }
        }
        fopMsgM_setStatus_l(mng, 0x13);
        bms1MessageId() = 0xFFFFFFFF;
        return TRUE;
    }
    if (fopMsgM_getStatus_l(mng) == 2 || fopMsgM_getStatus_l(mng) == 6) {
        if (mMsgNo == mEndMsgNo) {
            mEndMsgNo = 0;
            return TRUE;
        }
    }
    return FALSE;
}

/* 0220C308 */
BOOL daNpc_Bms1_c::evn_viblation_init(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> dir;
    dir->z = 0.0f;
    dir->y = 1.0f;
    dir->x = 0.0f;
    vib->StartShock(5, -0x11, dir.get()); /* StartShock(5, -0x11, cXyz(0, 1, 0)) */
    return TRUE;
}

/* 0220C35C */
BOOL daNpc_Bms1_c::evn_head_swing_init(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mHeadAnm.swing_vertical_init(1, 0x1800, 0x1000, 1);
    return TRUE;
}

/* 0220C660 */
BOOL daNpc_Bms1_c::privateCut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const char* staffName = gabi::at<const char>(mEventCut.mpEvtStaffName);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (staffIdx == -1) {
        return FALSE;
    }
    s32 actIdx =
        dComIfGp_evmng_getMyActIdx_l(dComIfGp_getPEvtManager(), staffIdx, gabi::ea(bms1CutNames()), 4, 1, 0);
    dEvent_manager_c* evtMng = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        dEvent_manager_cutEnd_l(evtMng, staffIdx);
        return TRUE;
    }
    if (dEvent_manager_getIsAddvance_l(evtMng, staffIdx)) {
        switch (actIdx) {
        case 0:
            evn_talk_init(staffIdx);
            break;
        case 1:
            evn_continue_talk_init(staffIdx);
            break;
        case 2:
            evn_viblation_init(staffIdx);
            break;
        case 3:
            evn_head_swing_init(staffIdx);
            break;
        }
    }
    if ((u32)actIdx <= 1) {
        if (!evn_talk()) {
            return TRUE;
        }
    }
    dComIfGp_evmng_cutEnd(staffIdx);
    return TRUE;
}

// clang-format off
/* WWHD glue
str 101BD3A0 "Bms"
str 10017FA4 "BMS_LAND_DEMO" in 0220C7CC
str 10017F6C "EndMsgNo" in 0220C2A4
str 10017F60 "EndMsgNo" in 0220C1C0
str 10017F58 "MsgNo" in 0220C1C0
str 10017F48 "BMS_GET_DEMO" in 0220BF90
str 10017F40 "Bms1" in 0220BF90
*/
// clang-format on
