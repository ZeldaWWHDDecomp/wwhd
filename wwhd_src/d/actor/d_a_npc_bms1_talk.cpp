/**
 * d_a_npc_bms1_talk.cpp (WWHD)
 * NPC - Bomb-Master Cannon: orders, messages and the shop, actions, event cuts.
 *
 * Written from the WWHD code (the GameCube TU is
 * "Nonmatching"); the structure follows d_a_npc_bs1 (Beedle). See d_a_npc_bms1.cpp.
 */
#include "d/actor/d_a_npc_bms1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline bool cXyz_normalizeRS_l(cXyz* v) { return gabi::call<bool>(0x0201B47C, v); }
static inline BOOL daNpc_Bms1_shopMsgCheck(u32 msgNo) { return gabi::call<BOOL>(0x02208334, msgNo); }
static inline BOOL daNpc_Bms1_shopStickMoveMsgCheck(u32 msgNo) { return gabi::call<BOOL>(0x0220836C, msgNo); }
static inline s8 cLib_calcTimer_s8(be<s8>* t) { return gabi::call<s8>(0x0220CCB4, t); }
/* dComIfGp_setDoStatusForce / setAStatusForce (play+0x5BBA / +0x5BB9) */
static inline void dComIfGp_setDoStatusForce_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BBA, s); }
static inline void dComIfGp_setAStatusForce_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB9, s); }
/* the message flags the shop sets: play+0x5BD2 / +0x5BD3, manager +0x922 / +0x923 / +0x938 / +0x948 */
static inline u8 play_u8(u32 off) { return gabi::load<u8>(dComIfGp_ea() + off); }
static inline void play_set_u8(u32 off, u8 v) { gabi::store<u8>(dComIfGp_ea() + off, v); }

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline): m24 + normalize(m18 - m24) * 100, copied
 * through the FPRs into *out */
static inline void bms1_getItemZoomPos(ShopCam_action_c_l* cam, cXyz* out) {
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

/* 0220A36C */
void daNpc_Bms1_c::checkOrder() {
    WWHD_FUNC(0x0220A36C, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt */) {
        if (mOrderEvt == 4) {
            mOrderEvt = 0;
            mShopCamAction.Reset();
            setAction(PMF_event_action, nullptr);
        } else if (mOrderEvt == 3) {
            mOrderEvt = 0;
            mShopCamAction.Reset();
            setAction(PMF_getdemo_action, nullptr);
        }
    } else if (command == 1 /* checkCommandTalk */) {
        if (mOrderEvt == 1 || mOrderEvt == 2) {
            mOrderEvt = 0;
            m941 = 1;
            talkInit();
            mShopCamAction.shop_cam_action_init();
            gabi::Local<cXyz> pos;
            pos->x = -70.0f;
            pos->y = 0.0f;
            pos->z = 150.0f;
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            gabi::call_ptr(player_vfunc(player, 0x114), player, pos.get(), (s16)-0x7000); /* setPlayerPosAndAngle */
        }
    } else {
        mShopCamAction.Save();
    }
}
VERIFY(0x0220A36C, &daNpc_Bms1_c::checkOrder);

/* 0220ABEC */
void daNpc_Bms1_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x0220ABEC, void, this);
    if (mType == 1) {
        return;
    }
    switch (dComIfGp_getMesgAnimeAttrInfo_l()) {
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
        mA02 = 1;
        break;
    case 5:
        setAnm(5, -1.0f);
        mA02 = 3;
        break;
    case 6:
        setAnm(6, -1.0f);
        mA02 = 3;
        break;
    }
    s8 anm = mAnmIdx;
    if (anm == 5) {
        if (McaMorf_checkEnd_l(mpMorf)) {
            s8 n = (s8)(mA02 - 1);
            mA02 = n;
            if (n <= 0) {
                setAnm(0, 13.0f);
            }
        }
    } else if (anm == 6 || anm == 4) {
        if (McaMorf_checkEnd_l(mpMorf)) {
            s8 n = (s8)(mA02 - 1);
            mA02 = n;
            if (n <= 0) {
                setAnm(1, -1.0f);
            }
        }
    }
    dComIfGp_clearMesgAnimeAttrInfo_l();
}
VERIFY(0x0220ABEC, &daNpc_Bms1_c::setAnmFromMsgTag);

/* 0220AEFC (HD: the position by pointer to a copy; the angle is not used) */
BOOL daNpc_Bms1_c::chkAttention(cXyz* i_pos, s16 i_angle) {
    WWHD_FUNC(0x0220AEFC, BOOL, this, i_pos, i_angle);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 maxAttnDistXZ = l_HIO().mMaxAttnDistXZ;
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff.get(), i_pos);
    gabi::Local<cXyz> dir;
    s16 angY = current.angle.y;
    dir->x = cM_ssin(angY);
    dir->y = 0.0f;
    dir->z = cM_scos(angY);
    return maxAttnDistXZ > PSVECDotProduct(dir.get(), diff.get());
}
VERIFY(0x0220AEFC, &daNpc_Bms1_c::chkAttention);

/* the B/cancel answer of the shop menus (inline): switch to the given message */
static inline u32 bms1_cancelMsg(u32 mng, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    gabi::store<u8>(mng + 0x922, 1);
    play_set_u8(0x5BD2, 1);
    play_set_u8(0x5BD3, 0);
    return 0xF;
}

/* 0220AFB4 (HD: a second message number pointer, may be NULL) */
u32 daNpc_Bms1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    WWHD_FUNC(0x0220AFB4, u32, this, pMsgNo, pMsgNo2);
    u32 mng = l_msgMng_l();
    u32 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    gabi::store<u8>(mng + 0x923, 0);
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
        gabi::store<u8>(mng + 0x922, 1);
        gabi::store<u8>(mng + 0x923, 1);
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
        if (play_u8(0x5BD3) == 0 && gabi::load<u32>(mng + 0x948) == 0) {
            *pMsgNo = 0x2780;
        } else {
            *pMsgNo = *pMsgNo - 3;
        }
        break;
    case 0x278B:
    case 0x278C:
    case 0x278D:
        if (play_u8(0x5BD3) == 0 && gabi::load<u32>(mng + 0x948) == 0) {
            s16 price = gabi::load<s16>(dComIfGp_ea() + 0x5BA4); /* dComIfGp_getMessageRupee() */
            s32 err = dShop_BoughtErrorStatus_l(&mShopItems, 0, price);
            if (err & 0x20) {
                *pMsgNo = 0x278E;
            } else if (err & 0x4) {
                *pMsgNo = 0x278F;
            } else {
                mDoAud_seStart_l(0x87F, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
                mShopItems.hideSelectItem();
                mBoughtItemNo = mShopItems.getSelectItemNo();
                u32 play = dComIfGp_ea();
                gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - price); /* setItemRupeeCount(-price) */
                if (!checkItemGet(mShopItems.getSelectItemNo(), 1)) {
                    mOrderEvt = 3;
                    status = 0x10;
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
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}
VERIFY(0x0220AFB4, &daNpc_Bms1_c::next_msgStatus);

/* 0220B354 */
u32 daNpc_Bms1_c::getMsg() {
    WWHD_FUNC(0x0220B354, u32, this);
    u32 msgNo = mNextMsgNo;
    if (msgNo != 0) {
        mNextMsgNo = 0;
    } else {
        msgNo = dComIfGs_isEventBit_l(0xA02) ? 0x2782 : 0x2775;
    }
    return msgNo;
}
VERIFY(0x0220B354, &daNpc_Bms1_c::getMsg);

/* 0220B3B0 */
u16 daNpc_Bms1_c::normal_talk() {
    WWHD_FUNC(0x0220B3B0, u16, this);
    u32 mng = l_msgMng_l();
    u16 status = fopMsgM_getStatus_l(mng);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo, nullptr));
        if (fopMsgM_getStatus_l(mng) == 0xF) {
            fopMsgM_messageSet_l(mng, mMsgNo, nullptr);
        }
    } else if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopMsgM_setStatus_l(mng, 0x13);
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn_l();
    }
    gabi::Local<cXyz> pos;
    bms1_getItemZoomPos(&mShopCamAction, pos.get());
    mShopItems.Item_ZoomUp(pos.get());
    mpShopCursor->hide();
    if (play_u8(0x5BD2) != 0) {
        mMsgNo = gabi::load<u32>(mng + 0x938);
    }
    return status;
}
VERIFY(0x0220B3B0, &daNpc_Bms1_c::normal_talk);

/* 0220B554 */
u16 daNpc_Bms1_c::shop_talk() {
    WWHD_FUNC(0x0220B554, u16, this);
    u32 mng = l_msgMng_l();
    mpShopCursor->show();
    if (dShop_now_triggercheck_l(&mStickControl, &mShopItems, &mMsgNo, 0, nullptr)) {
        m918 = 1;
        mMsgNo2 = 0;
        if (mShopItems.mSelectedItemIdx >= 0 && !checkItemGet(0x69, 1)) {
            mHeadAnm.swing_vertical_init(1, 0x1800, 0x1000, 1);
        }
    }
    u16 status = fopMsgM_getStatus_l(mng);
    if (status == 0xE || status == 0xF) {
        if (m918 != 0 || gabi::load<u8>(mng + 0x922) != 0) {
            m918 = 0;
        } else {
            mMsgNo2 = mMsgNo;
            fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo2, &mMsgNo));
            if (fopMsgM_getStatus_l(mng) == 0xF) {
                fopMsgM_messageSet_l(mng, mMsgNo2, nullptr);
            }
        }
    } else if (status == 0x12) {
        fopMsgM_setStatus_l(mng, 0x13);
        mShopItems.mSelectedItemIdx = -1;
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn_l();
    }
    return status;
}
VERIFY(0x0220B554, &daNpc_Bms1_c::shop_talk);

/* 0220B6C4 */
u16 daNpc_Bms1_c::talk() {
    WWHD_FUNC(0x0220B6C4, u16, this);
    u32 mng = l_msgMng_l();
    u16 status = 0xFF;
    if (mTalkStep == 0) {
        l_msgId = 0xFFFFFFFF;
        mMsgNo = getMsg();
        mTalkStep = 1;
        mShopCamAction.m54 = mShopItems.mSelectedItemIdx;
        mMsgNo2 = 0;
        mBoughtItemNo = 0xFF;
        return status;
    }
    if (mTalkStep != -1) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = fopMsgM_messageSet_l(mng, mMsgNo, &eyePos);
            if (l_msgId != 0xFFFFFFFF) {
                if (daNpc_Bms1_shopStickMoveMsgCheck(mMsgNo)) {
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
            if (play_u8(0x5BD2) != 0) {
                u32 msgNo = gabi::load<u32>(mng + 0x938);
                mMsgNo = msgNo;
                if (!daNpc_Bms1_shopStickMoveMsgCheck(msgNo)) {
                    if (!daNpc_Bms1_shopMsgCheck(msgNo)) {
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
    mShopCamAction.m54 = mShopItems.mSelectedItemIdx;
    return status;
}
VERIFY(0x0220B6C4, &daNpc_Bms1_c::talk);

/* 0220B88C */
void daNpc_Bms1_c::setAttention(bool i_force) {
    WWHD_FUNC(0x0220B88C, void, this, i_force);
    if (!i_force && m93F >= 2) {
        return;
    }
    f32 z = mAttnBasePos.z;
    f32 y = mAttnBasePos.y + l_HIO().mAttnYOffset;
    f32 x = mAttnBasePos.x;
    gabi::store<f32>(gabi::ea(this) + 0x398, z); /* attention_info.position */
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    gabi::store<f32>(gabi::ea(this) + 0x394, y);
}
VERIFY(0x0220B88C, &daNpc_Bms1_c::setAttention);

/* 0220B8C8 */
void daNpc_Bms1_c::lookBack() {
    WWHD_FUNC(0x0220B8C8, void, this);
    gabi::Local<cXyz> target;
    cXyz* dstPos = nullptr;
    f32 ex = 0.0f, ey = 0.0f, ez = 0.0f;
    s16 desiredYRot = current.angle.y;
    s8 mode = mMode;
    if (mode == 1) {
        if (m940 != 0) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye.get(), l_HIO().m04);
            ex = current.pos.x;
            ez = current.pos.z;
            dstPos = target.get();
            target->copy(*eye);
            ey = eyePos.y;
        }
    } else if (mode == 2) {
        if (mShopItems.mSelectedItemIdx == -1) {
            bms1_getItemZoomPos(&mShopCamAction, target.get());
        } else {
            gabi::Local<cXyz> basePos;
            mShopItems.getSelectItemBasePos(basePos.get());
            gabi::Local<cXyz> itemPos;
            mShopItems.getSelectItemPos(itemPos.get());
            target->copy(*itemPos);
            mpShopCursor->setPos(basePos.get());
            daNpc_Bms1_HIO_c& hio = l_HIO();
            mpShopCursor->setScale(hio.mCursorScale[0], hio.mCursorScale[1], hio.mCursorScale[2], hio.mCursorScale[3],
                                   hio.mCursorScale[4]);
            mpShopCursor->anm_play();
        }
        ex = current.pos.x;
        ey = eyePos.y;
        ez = current.pos.z;
        dstPos = target.get();
    }
    s16 vel;
    if (mJntCtrl.mbTrn != 0) {
        cLib_addCalcAngleS2(&mHeadTurnVel, l_HIO().mMaxHeadTurnVel, 4, 0x800);
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
VERIFY(0x0220B8C8, &daNpc_Bms1_c::lookBack);

/* 0220BB98 */
BOOL daNpc_Bms1_c::checkPlayerLanding() {
    WWHD_FUNC(0x0220BB98, BOOL, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 playerY = player->current.pos.y;
    BOOL ret = FALSE;
    if (m948 - playerY > 200.0f) {
        if (!gabi::call_ptr<BOOL>(player_vfunc(player, 0x4C), player)) {
            ret = TRUE;
        }
    }
    if (!gabi::call_ptr<BOOL>(player_vfunc(player, 0x4C), player)) {
        m948 = playerY;
    }
    return ret;
}
VERIFY(0x0220BB98, &daNpc_Bms1_c::checkPlayerLanding);

/* 0220BC54 */
BOOL daNpc_Bms1_c::wait01() {
    WWHD_FUNC(0x0220BC54, BOOL, this);
    if (m941 != 0) {
        s8 mode = mMode;
        mMode = 2;
        mPrevMode = mode;
        return McaMorf_curMorf_l(mpMorf) < 1.0f; /* mpMorf->isMorf() */
    }
    if (m940 != 0) {
        mOrderEvt = (mType == 1) ? 0 : 2;
    }
    return McaMorf_curMorf_l(mpMorf) < 1.0f;
}
VERIFY(0x0220BC54, &daNpc_Bms1_c::wait01);

/* 0220BCD8 */
BOOL daNpc_Bms1_c::talk01() {
    WWHD_FUNC(0x0220BCD8, BOOL, this);
    u16 status = talk();
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        mMode = mPrevMode;
        dComIfGp_event_reset_l();
        mShopCamAction.Reset();
        daPy_offNoDraw_l(player);
        m941 = 0;
    } else if (daNpc_Bms1_shopMsgCheck(mMsgNo) && status == 8) {
        mShopItems.getSelectItemBuyMsg();
    }
    if (mpShopCursor->mbShow != 0) {
        dComIfGp_setDoStatusForce_l(0x17 /* dActStts_CHOOSE_e */);
        dComIfGp_setAStatusForce_l(0x27 /* dActStts_CANCEL_e */);
    }
    return McaMorf_curMorf_l(mpMorf) < 1.0f;
}
VERIFY(0x0220BCD8, &daNpc_Bms1_c::talk01);

/* 0220BDCC */
BOOL daNpc_Bms1_c::wait_action(void*) {
    WWHD_FUNC(0x0220BDCC, BOOL, this, (void*)nullptr);
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
        m940 = (u8)chkAttention(pos.get(), (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]));
        if (mType == 1 && McaMorf_checkEnd_l(mpMorf) && cLib_calcTimer_s8(&mA02) == 0) {
            s8 room = current.roomNo;
            mA02 = 3;
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
            mOrderEvt = 4;
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0220BDCC, &daNpc_Bms1_c::wait_action);

/* 0220BF90 */
BOOL daNpc_Bms1_c::getdemo_action(void*) {
    WWHD_FUNC(0x0220BF90, BOOL, this, (void*)nullptr);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x10017F40) /* "Bms1" */, nullptr, 0);
    if (mActionStatus == 0) {
        daPy_offNoDraw_l(dComIfGp_getPlayer(0));
        mMode = mPrevMode;
        mShopCamAction.Reset();
        u8 itemNo = mShopItems.getSelectItemNo();
        u32 itemPID = fopAcM_createItemForPresentDemo_l(&current.pos, itemNo, 0, -1, current.roomNo, nullptr, nullptr);
        if (itemPID != 0xFFFFFFFF) {
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, itemPID); /* dComIfGp_event_setItemPartnerId */
        }
        dComIfGp_evmng_cutEnd(staffIdx);
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (mActionStatus != -1) {
        fopMsgM_demoMsgFlagOn_l();
        dComIfGp_evmng_cutEnd(staffIdx);
        if (dComIfGp_evmng_endCheckOld_l(STR(0x10017F48))) {
            mOrderEvt = 1;
            dComIfGp_event_reset_l();
            mNextMsgNo = 0x2790;
            setAction(PMF_wait_action, nullptr);
        }
    }
    return TRUE;
}
VERIFY(0x0220BF90, &daNpc_Bms1_c::getdemo_action);

/* 0220C1C0 */
BOOL daNpc_Bms1_c::evn_talk_init(int i_staffIdx) {
    WWHD_FUNC(0x0220C1C0, BOOL, this, i_staffIdx);
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10017F58) /* "MsgNo" */, 3);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10017F60) /* "EndMsgNo" */, 3);
    l_msgId = 0xFFFFFFFF;
    if (pMsgNo != nullptr) {
        u32 msgNo = *pMsgNo;
        mMsgNo = msgNo;
        if (msgNo == 0x2791 && dComIfGs_isEventBit_l(0xA02)) {
            mMsgNo = 0x2792;
        }
    } else {
        mMsgNo = 0;
    }
    mEndMsgNo = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}
VERIFY(0x0220C1C0, &daNpc_Bms1_c::evn_talk_init);

/* 0220C2A4 */
BOOL daNpc_Bms1_c::evn_continue_talk_init(int i_staffIdx) {
    WWHD_FUNC(0x0220C2A4, BOOL, this, i_staffIdx);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10017F6C) /* "EndMsgNo" */, 3);
    mEndMsgNo = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}
VERIFY(0x0220C2A4, &daNpc_Bms1_c::evn_continue_talk_init);

/* 0220C308 */
BOOL daNpc_Bms1_c::evn_viblation_init(int i_staffIdx) {
    WWHD_FUNC(0x0220C308, BOOL, this, i_staffIdx);
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> dir;
    dir->z = 0.0f;
    dir->y = 1.0f;
    dir->x = 0.0f;
    gabi::call<BOOL>(0x025CB374, vib, 5, -0x11, dir.get()); /* StartShock(5, -0x11, cXyz(0, 1, 0)) */
    return TRUE;
}
VERIFY(0x0220C308, &daNpc_Bms1_c::evn_viblation_init);

/* 0220C35C */
BOOL daNpc_Bms1_c::evn_head_swing_init(int i_staffIdx) {
    WWHD_FUNC(0x0220C35C, BOOL, this, i_staffIdx);
    mHeadAnm.swing_vertical_init(1, 0x1800, 0x1000, 1);
    return TRUE;
}
VERIFY(0x0220C35C, &daNpc_Bms1_c::evn_head_swing_init);

/* 0220C394 */
BOOL daNpc_Bms1_c::evn_talk() {
    WWHD_FUNC(0x0220C394, BOOL, this);
    u32 mng = l_msgMng_l();
    if (l_msgId == 0xFFFFFFFF) {
        l_msgId = fopMsgM_messageSet_l(mng, mMsgNo, &eyePos);
        if (l_msgId != 0xFFFFFFFF) {
            fopMsgM_demoMsgFlagOn_l();
        }
        return FALSE;
    }
    setAnmFromMsgTag();
    if (fopMsgM_getStatus_l(mng) == 0xE) {
        fopMsgM_setStatus_l(mng, next_msgStatus(&mMsgNo, nullptr));
        if (fopMsgM_getStatus_l(mng) == 0xF) {
            fopMsgM_messageSet_l(mng, mMsgNo, nullptr);
        }
        return FALSE;
    }
    if (fopMsgM_getStatus_l(mng) == 0x12) {
        u32 msgNo = mMsgNo;
        /* bombs (item 0x49/0x4A/other: 10/20/30): the counters at play+0x5B70.. */
        if (msgNo == 0x1DCA) {
            u8 item = mA0B;
            u32 play;
            if (item == 0x49) {
                play = dComIfGp_ea();
                gabi::store<s16>(play + 0x5B78, (s16)(gabi::load<s16>(play + 0x5B78) - 10));
            } else if (item == 0x4A) {
                play = dComIfGp_ea();
                gabi::store<s16>(play + 0x5B7A, (s16)(gabi::load<s16>(play + 0x5B7A) - 20));
            } else {
                play = dComIfGp_ea();
                gabi::store<s16>(play + 0x5B7C, (s16)(gabi::load<s16>(play + 0x5B7C) - 30));
            }
        } else if (msgNo == 0x1DC4) {
            u8 item = mA0B;
            if (item == 0x49) {
                u32 play = dComIfGp_ea();
                gabi::store<s16>(play + 0x5B78, (s16)(gabi::load<s16>(play + 0x5B78) - 10));
            } else {
                s32 i = (item == 0x4A) ? 5 : 6;
                u32 a = dComIfGp_ea() + 0x5B70 + i * 2;
                gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - 10));
            }
        }
        fopMsgM_setStatus_l(mng, 0x13);
        l_msgId = 0xFFFFFFFF;
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
VERIFY(0x0220C394, &daNpc_Bms1_c::evn_talk);

/* 0220C660 */
BOOL daNpc_Bms1_c::privateCut() {
    WWHD_FUNC(0x0220C660, BOOL, this);
    const char* staffName = gabi::at<const char>(mEventCut.mpEvtStaffName);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (staffIdx == -1) {
        return FALSE;
    }
    s32 actIdx = dComIfGp_evmng_getMyActIdx_l(dComIfGp_getPEvtManager(), staffIdx, 0x101BD36C /* cut_name_tbl */, 4, 1, 0);
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
VERIFY(0x0220C660, &daNpc_Bms1_c::privateCut);

/* 0220C7CC */
BOOL daNpc_Bms1_c::event_action(void*) {
    WWHD_FUNC(0x0220C7CC, BOOL, this, (void*)nullptr);
    if (mActionStatus == 0) {
        if (mType != 1) {
            setAnm(dComIfGs_isEventBit_l(0xA02) ? 1 : 0, -1.0f);
        }
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (mActionStatus != -1) {
        privateCut();
        if (dComIfGp_evmng_endCheckOld_l(STR(0x10017FA4))) {
            mOrderEvt = 0;
            dComIfGp_event_reset_l();
            setAction(PMF_wait_action, nullptr);
        }
    }
    return TRUE;
}
VERIFY(0x0220C7CC, &daNpc_Bms1_c::event_action);
