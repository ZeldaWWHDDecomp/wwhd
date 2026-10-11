/**
 * d_a_npc_ds1_talk.cpp (WWHD)
 * NPC - Doc Bandam: messages, talk, look-back and the wait action.
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#include "d/actor/d_a_npc_ds1.h"

/* HD messages: the manager (*101F4B5C); +0x922 u8 (a trigger was handled), +0x938 the current
 * message number, +0x948 a flag */

/* getSelectItemPos / getSelectItemBasePos return a cXyz through a hidden result pointer */

/* ShopCursor_c (HD): show/hide flag byte at +0xB4; setScale's m48/m4C/m50 at +0xA8.., m38/m3C at +0x98.. */
static inline void ShopCursor_setShow(u32 c, u8 v) { gabi::at<ShopCursor_c>(c)->m54 = v; }

static inline void cp_int(cXyz* dst, cXyz* src) { /* GHS struct copy: integer words */
    ds1VectorWords(dst).x = ds1VectorWords(src).x;
    ds1VectorWords(dst).y = ds1VectorWords(src).y;
    ds1VectorWords(dst).z = ds1VectorWords(src).z;
}

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline) into *out (a float copy) */
static void getItemZoomPos(daNpc_Ds1_c* t, cXyz* out) {
    gabi::Local<cXyz> dir;
    gabi::Local<cXyz> scaled;
    gabi::Local<cXyz> sum;
    cXyz_mi(&t->mShopCam.m18, dir, &t->mShopCam.m24);
    if (!ds1Guest_cXyz_normalizeRS(dir))
        dir->set(0.0f, 0.0f, 1.0f);
    cXyz_ml(dir, scaled, 100.0f);
    cXyz_pl(&t->mShopCam.m24, sum, scaled);
    f32 x = sum->x, y = sum->y, z = sum->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

/* 0222FAC4 */
void daNpc_Ds1_c::setAnmFromMsgTag() {
    u8 tag = ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgAnime; // HD: this byte is the animation attribute, distinct from the tag byte.
    switch (tag) {
    case 0: setAnm(0, -1.0f); setTexAnm(0); break;
    case 1: setAnm(1, -1.0f); setTexAnm(0); break;
    case 2: setAnm(2, -1.0f); setTexAnm(1); break;
    case 3: setAnm(3, -1.0f); setTexAnm(0); break;
    case 4: setAnm(4, -1.0f); setTexAnm(1); break;
    case 5: setAnm(9, -1.0f); setTexAnm(1); break;
    }
    if (mAnmIdx == 1) {
        mDoExt_McaMorf* m = mpMorf;
        if (ds1Guest_J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mAnmLoopCount + 1);
            mAnmLoopCount = n;
            if (n > 1) {
                setAnm(0, -1.0f);
                mAnmLoopCount = 0;
            }
        }
    }
    if (mAnmIdx == 9) {
        mDoExt_McaMorf* m = mpMorf;
        if (ds1Guest_J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mAnmLoopCount + 1);
            mAnmLoopCount = n;
            if (n > 4) {
                setAnm(0, -1.0f);
                mAnmLoopCount = 0;
            }
        }
    }
    if (mAnmIdx == 3) {
        mDoExt_McaMorf* m = mpMorf;
        if (ds1Guest_J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mAnmLoopCount + 1);
            mAnmLoopCount = n;
            if (n > 1) {
                setAnm(2, -1.0f);
                mAnmLoopCount = 0;
            }
        }
    }
    ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgAnime = 0xFF;
}

/* 0222FDBC */
bool daNpc_Ds1_c::chkAttention(cXyz* pos, s16 angle) {
    u32 play = dComIfGp_ea();
    daNpc_Ds1_childHIO_c& hio = l_HIO.mChild[mType];
    fopAc_ac_c* player = gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer;
    f32 maxX = hio.m2C;
    f32 maxZ = hio.m30;
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff, pos);
    gabi::Local<cXyz> axisZ;
    gabi::Local<cXyz> axisX;
    axisZ->set(0.0f, 0.0f, 1.0f);
    axisX->set(1.0f, 0.0f, 0.0f);
    f32 z = PSVECDotProduct(axisZ, diff);
    f32 x = PSVECDotProduct(axisX, diff);
    bool ret = false;
    if (maxZ > z && maxX > std::fabs(x))
        ret = true;
    return ret;
}

/* 0222FECC */
u16 daNpc_Ds1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    u32 mng = (u32)l_messageManager.mValue;
    u32 msgNo = *pMsgNo;
    u16 next = 0xF;
    switch (msgNo) {
    case 0x1DB1: case 0x1DB2: case 0x1DB3: case 0x1DB5: case 0x1DB6: case 0x1DB7: case 0x1DB8:
    case 0x1DBB: case 0x1DBC: case 0x1DC9: case 0x1DCE:
        *pMsgNo = msgNo + 1;
        break;
    case 0x1DCF:
        *pMsgNo = 0x1DDE;
        break;
    case 0x1DC0:
        if (ds1Guest_CPad_CHECK_TRIG_A(0)) {
            *pMsgNo = 0x1DDC;
            if (pMsgNo2 != nullptr)
                *pMsgNo2 = 0x1DDC;
            gabi::at<Ds1MessageManager>(mng)->mTriggerHandled = 1;
            ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgSendButton = 1;
            ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgCancelButton = 0;
        } else {
            next = 0xE;
        }
        break;
    case 0x1DD1: case 0x1DD2: case 0x1DD3:
        if (ds1Guest_CPad_CHECK_TRIG_A(0)) {
            *pMsgNo = 0x1DDC;
            if (pMsgNo2 != nullptr)
                *pMsgNo2 = 0x1DDC;
            gabi::at<Ds1MessageManager>(mng)->mTriggerHandled = 1;
            ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgSendButton = 1;
            ((dComIfG_inf_c*)dComIfGp_get())->play.mMesgCancelButton = 0;
        } else {
            *pMsgNo = *pMsgNo + 3;
        }
        break;
    case 0x1DC3:
        if (!ds1Guest_daNpc_Ds1_checkCreateDrugChuchu(mItemNo)) {
            *pMsgNo = 0x1DC8;
        } else {
            u8 item = mItemNo;
            bool enough = false;
            if (item == DS1_RED_POTION) enough = dComIfGs_getBeastNum(4) >= 5;
            else if (item == DS1_GREEN_POTION) enough = dComIfGs_getBeastNum(5) >= 5;
            else if (item == DS1_BLUE_POTION) enough = dComIfGs_getBeastNum(6) >= 5;
            *pMsgNo = enough ? 0x1DC4 : 0x1DC7;
        }
        break;
    case 0x1DC4:
        if (dComIfGs_checkEmptyBottle())
            return 0x10;
        *pMsgNo = 0x1DC5;
        break;
    case 0x1DC8: {
        u8 item = mItemNo;
        bool enough = false;
        if (item == DS1_RED_POTION) enough = dComIfGs_getBeastNum(4) >= 10;
        else if (item == DS1_GREEN_POTION) enough = dComIfGs_getBeastNum(5) >= 15;
        else if (item == DS1_BLUE_POTION) enough = dComIfGs_getBeastNum(6) >= 15;
        *pMsgNo = enough ? 0x1DC9 : 0x1DCE;
        break;
    }
    case 0x1DCB:
        if (dComIfGs_checkEmptyBottle())
            return 0x10;
        *pMsgNo = 0x1DCC;
        break;
    case 0x1DD4: case 0x1DD5: case 0x1DD6:
        if (((dComIfG_inf_c*)dComIfGp_get())->play.mMesgCancelButton != 0 || gabi::at<Ds1MessageManager>(mng)->mSelectionFlag != 0) {
            *pMsgNo = *pMsgNo - 3;
        } else {
            s16 price = ((dComIfG_inf_c*)dComIfGp_get())->play.mMessageRupee;
            u8 err = ds1Guest_dShop_BoughtErrorStatus(&mShopItems, 0, price);
            if (err & 8) {
                *pMsgNo = 0x1DD7;
            } else if (err & 0x10) {
                *pMsgNo = 0x1DD8;
            } else if (err & 0x20) {
                *pMsgNo = 0x1DD9;
            } else {
                BOOL got = ds1Guest_checkItemGet(ds1Guest_ShopItems_getSelectItemNo(&mShopItems), 0);
                mDoAud_seStart(0x87F, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
                ds1Guest_ShopItems_hideSelectItem(&mShopItems);
                mBoughtItemNo = ds1Guest_ShopItems_getSelectItemNo(&mShopItems);
                if (got == 0) {
                    ((dComIfG_inf_c*)dComIfGp_get())->play.mItemRupeeCount -= price;
                    mEventActionIdx = 4;
                    mOrderType = 3;
                    return 0x10;
                }
                ds1Guest_execItemGet(ds1Guest_ShopItems_getSelectItemNo(&mShopItems));
                ((dComIfG_inf_c*)dComIfGp_get())->play.mItemRupeeCount -= price;
                *pMsgNo = 0x1DDA;
            }
        }
        break;
    case 0x1DC1: case 0x1DD7: case 0x1DD8: case 0x1DD9: case 0x1DDA: case 0x1DDB:
        *pMsgNo = 0x1DC0;
        break;
    case 0x1DB9:
        if (gabi::at<Ds1MessageManager>(mng)->mSelectionFlag != 0)
            *pMsgNo = 0x1DBA;
        else
            *pMsgNo = 0x1DBB;
        break;
    default:
        return 0x10;
    }
    return next;
}

/* 02230364 */
u32 daNpc_Ds1_c::getMsg() {
    u32 msgNo = mPendingMsgNo;
    if (msgNo != 0) {
        mPendingMsgNo = 0;
        return msgNo;
    }
    if (dComIfGp_event_chkTalkXY()) {
        if ((u32)(mItemNo - DS1_RED_POTION) <= 2) {
            dComIfGs_onEventBit(0x1120);
            return 0x1DC3;
        }
        if (dComIfGs_isEventBit(0x1120))
            return 0x1DC2;
        return 0x1DDD;
    }
    msgNo = 0x1DB1;
    if (dComIfGs_isEventBit(0x1120))
        msgNo = 0x1DDB;
    return msgNo;
}

/* 02230420 */
u16 daNpc_Ds1_c::normal_talk() {
    u32 mng = (u32)l_messageManager.mValue;
    u16 status = (u16)ds1Guest_fopMsgM_SearchByID(mng);
    if (status == 0xE) {
        ds1Guest_fopMsgM_setStatus(mng, ds1Guest_nextMessageStatusRegister(this, &mMsgNo, nullptr)); /* next_msgStatus: r3 passed on unmasked */
        if (ds1Guest_fopMsgM_SearchByID(mng) == 0xF)
            ds1Guest_fopMsgM_messageSet(mng, mMsgNo, nullptr);
    } else if (status == 0x12) {
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DCA) {
            u8 item = mItemNo;
            if (item == DS1_RED_POTION) {
                auto& count = ((dComIfG_inf_c*)dComIfGp_get())->play.mItemBeastNumCounts[4];
                count = (s16)(count - 10);
            } else if (item == DS1_GREEN_POTION) {
                auto& count = ((dComIfG_inf_c*)dComIfGp_get())->play.mItemBeastNumCounts[5];
                count = (s16)(count + -15);
            } else {
                auto& count = ((dComIfG_inf_c*)dComIfGp_get())->play.mItemBeastNumCounts[6];
                count = (s16)(count + -15);
            }
            msgNo = mMsgNo;
        } else if (msgNo == 0x1DC4) {
            u8 item = mItemNo;
            s32 i = item == DS1_RED_POTION ? 4 : (item == DS1_GREEN_POTION ? 5 : 6);
            auto& count = ((dComIfG_inf_c*)dComIfGp_get())->play.mItemBeastNumCounts[i];
                count = (s16)(count - 5);
            msgNo = mMsgNo;
        }
        if (msgNo == 0x1DCA) {
            mOrderType = 4;
        } else if (msgNo == 0x1DCB || msgNo == 0x1DC4) {
            mOrderType = 3;
        } else if (msgNo == 0x1DCD || msgNo == 0x1DCC) {
            u8 item = mItemNo;
            mOrderType = 0;
            if (item != DS1_RED_POTION)
                dComIfGs_onEventBit(item == DS1_GREEN_POTION ? DS1_GREEN_POTION_UNLOCKED : DS1_BLUE_POTION_UNLOCKED);
            ds1Guest_dComIfGp_setNextStage("Pdrgsh", 1, current.roomNo, -1, 0.0f, 0, 1, 0);
        }
        ds1Guest_fopMsgM_setStatus(mng, 0x13);
    } else if (status == 1) {
        ds1Guest_fopMsgM_demoMsgFlagOn();
    }
    gabi::Local<cXyz> zoom;
    getItemZoomPos(this, zoom);
    ds1Guest_ShopItems_Item_ZoomUp(&mShopItems, zoom);
    ShopCursor_setShow(mpShopCursor, 0);
    return status;
}

/* 02230784 */
u16 daNpc_Ds1_c::shop_talk() {
    u32 mng = (u32)l_messageManager.mValue;
    ShopCursor_setShow(mpShopCursor, 1);
    if (ds1Guest_dShop_now_triggercheck(&mStick, &mShopItems, &mMsgNo, 0, 0)) {
        mNextMsgNo = 0;
        mShopInputHandled = 1;
    }
    u16 status = (u16)ds1Guest_fopMsgM_SearchByID(mng);
    if (status == 0xE || status == 0xF) {
        if ((s8)mShopInputHandled != 0 || gabi::at<Ds1MessageManager>(mng)->mTriggerHandled != 0) {
            mShopInputHandled = 0;
        } else {
            mNextMsgNo = mMsgNo;
            ds1Guest_fopMsgM_setStatus(mng, ds1Guest_nextMessageStatusRegister(this, &mNextMsgNo, &mMsgNo)); /* next_msgStatus */
            if (ds1Guest_fopMsgM_SearchByID(mng) == 0xF)
                ds1Guest_fopMsgM_messageSet(mng, mNextMsgNo, nullptr);
        }
    } else if (status == 0x12) {
        ds1Guest_fopMsgM_setStatus(mng, 0x13);
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DC4 || (msgNo >= 0x1DD4 && msgNo <= 0x1DD6))
            mOrderType = 3;
        mShopItems.mSelectedItemIdx = -1;
    } else if (status == 1) {
        ds1Guest_fopMsgM_demoMsgFlagOn();
    }
    return status;
}

/* 022308E0 */
u16 daNpc_Ds1_c::talk() {
    u32 mng = (u32)l_messageManager.mValue;
    u16 ret = 0xFF;
    s8 st = mTalkState;
    if (st == 0) {
        l_messageID.mValue = -1;
        mMsgNo = getMsg();
        mTalkState = 1;
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        mNextMsgNo = 0;
        mBoughtItemNo = 0xFF;
        return ret;
    }
    if (st == -1) {
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        return ret;
    }
    if (l_messageID.mValue == -1) {
        if (dComIfGp_event_chkTalkXY() && !dComIfGp_evmng_ChkPresentEnd())
            return ret;
        s32 id = (s32)ds1Guest_fopMsgM_messageSet(mng, mMsgNo, &eyePos);
        l_messageID.mValue = id;
        if (id != -1) {
            if (mMsgNo != 0x1DC0)
                mTalkState = 2;
            else
                mTalkState = 3;
        }
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        return ret;
    }
    setAnmFromMsgTag();
    s8 st2 = mTalkState;
    if ((u32)(s32)st2 == 2)
        ret = normal_talk();
    else if ((u32)(s32)st2 == 3)
        ret = shop_talk();
    if (((dComIfG_inf_c*)dComIfGp_get())->play.mMesgSendButton != 0) {
        u32 msgNo = gabi::at<Ds1MessageManager>(mng)->mMessageNo;
        mMsgNo = msgNo;
        if (msgNo - 0x1DD1 < 3 || msgNo == 0x1DC0) {
            if (msgNo == 0x1DC0) {
                s16 hide = mShopItems.mbIsHide;
                mShopItems.mSelectedItemIdx = -1;
                if (hide != 0)
                    ds1Guest_ShopItems_showItem(&mShopItems);
            }
            mTalkState = 3;
        } else {
            if (!ds1Guest_daNpc_Ds1_shopMsgCheck(msgNo)) {
                s16 hide = mShopItems.mbIsHide;
                mShopItems.mSelectedItemIdx = -1;
                if (hide != 0)
                    ds1Guest_ShopItems_showItem(&mShopItems);
            }
            mTalkState = 2;
        }
    }
    mShopCam.m54 = mShopItems.mSelectedItemIdx;
    return ret;
}

/* 02230ADC */
void daNpc_Ds1_c::setAttention(u32 force) {
    if (force == 0 && mAttnSetCount >= 2)
        return;
    daNpc_Ds1_childHIO_c& hio = l_HIO.mChild[mType];
    f32 y = mAttentionBasePos.y + hio.mNpc.mAttnYOffset;
    attention_info.position.z = mAttentionBasePos.z;
    attention_info.position.x = mAttentionBasePos.x;
    attention_info.position.y = y;
}

/* 02230B24 */
void daNpc_Ds1_c::lookBack() {
    f32 eyeX = 0.0f, eyeY = 0.0f, eyeZ = 0.0f;
    cXyz* target = nullptr;
    s16 angleY = current.angle.y;
    gabi::Local<cXyz> dst;
    s8 st = mWaitState;
    if ((u32)(s32)st == 1) {
        if (mPlayerInRange != 0) {
            gabi::Local<cXyz> eye;
            ds1Guest_dNpc_playerEyePos(eye, l_HIO.mChild[mType].mNpc.m04);
            cp_int(dst, eye);
            target = dst;
            eyeZ = current.pos.z;
            eyeX = current.pos.x;
            eyeY = eyePos.y;
            setTexAnm(0);
        } else {
            target = nullptr;
            setTexAnm(0);
        }
    } else if ((u32)(s32)st == 2) {
        if (mShopCam.mCurrActionFunc.mVtableIndex == 0) {
            gabi::Local<cXyz> eye;
            ds1Guest_dNpc_playerEyePos(eye, l_HIO.mChild[mType].mNpc.m04);
            eyeY = eyePos.y;
            cp_int(dst, eye);
            eyeZ = current.pos.z;
            eyeX = current.pos.x;
            target = dst;
        } else if (mShopItems.mSelectedItemIdx == -1) {
            getItemZoomPos(this, dst);
            eyeX = current.pos.x;
            eyeZ = current.pos.z;
            target = dst;
            eyeY = eyePos.y;
        } else {
            gabi::Local<cXyz> itemPos;
            gabi::Local<cXyz> basePos;
            ds1Guest_ShopItems_getSelectItemPos(&mShopItems, itemPos);
            ds1Guest_ShopItems_getSelectItemBasePos(&mShopItems, basePos);
            cp_int(dst, basePos);
            ds1Guest_ShopCursor_setPos(mpShopCursor, itemPos);
            daNpc_Ds1_childHIO_c& hio = l_HIO.mChild[mType];
            u32 c = mpShopCursor;
            f32 s4 = hio.m40, s2 = hio.m38, s3 = hio.m3C, s1 = hio.mCursorScale, s5 = hio.m44;
            gabi::at<ShopCursor_c>(c)->m48 = s1;
            gabi::at<ShopCursor_c>(c)->m4C = s2;
            gabi::at<ShopCursor_c>(c)->m50 = s3;
            gabi::at<ShopCursor_c>(c)->m38 = s4;
            gabi::at<ShopCursor_c>(c)->m3C = s5;
            ds1Guest_ShopCursor_anm_play(mpShopCursor);
            eyeX = current.pos.x;
            eyeY = eyePos.y;
            eyeZ = current.pos.z;
            target = dst;
        }
    }
    if (mAction.idx == -1 && mAction.delta == 0 && mAction.fn == ds1_tu.mEventAction) {
        gabi::Local<cXyz> eye;
        ds1Guest_dNpc_playerEyePos(eye, l_HIO.mChild[mType].mNpc.m04);
        eyeX = current.pos.x;
        cp_int(dst, eye);
        eyeY = eyePos.y;
        target = dst;
        eyeZ = current.pos.z;
    }
    gabi::Local<cXyz> eyeCopy;
    if (mJntCtrl.mbTrn) {
        cLib_addCalcAngleS2(&mHeadAngleY, l_HIO.mChild[mType].mNpc.mMaxHeadTurnVel, 4, 0x800);
        eyeCopy->x = eyeX;
        eyeCopy->y = eyeY;
        eyeCopy->z = eyeZ;
        ds1Guest_lookAtTarget(&mJntCtrl, &current.angle.y, target, eyeCopy, angleY, mHeadAngleY, 1);
    } else {
        eyeCopy->x = eyeX;
        mHeadAngleY = 0;
        eyeCopy->y = eyeY;
        eyeCopy->z = eyeZ;
        ds1Guest_lookAtTarget(&mJntCtrl, &current.angle.y, target, eyeCopy, angleY, 0, 1);
    }
}

/* 02230F2C */
BOOL daNpc_Ds1_c::wait01() {
    if (mIsTalking != 0) {
        u8 prev = mWaitState;
        mWaitState = 2;
        mPreviousWaitState = prev;
        return TRUE;
    }
    if (mPlayerInRange != 0) {
        mOrderType = 2;
        return TRUE;
    }
    if (mOrderType == 2)
        mOrderType = 0;
    return TRUE;
}

/* 02230F8C */
BOOL daNpc_Ds1_c::talk01() {
    u16 status = talk();
    if (status == 0x12) {
        u32 play = dComIfGp_ea();
        u32 msgNo = mMsgNo;
        u8 prev = mPreviousWaitState;
        fopAc_ac_c* player = gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer;
        mWaitState = prev;
        if (msgNo == 0x1DCD || msgNo == 0x1DCC) {
            ds1_setAction(this, ds1_tu.mDummyAction, nullptr);
            mIsTalking = 0;
            setAnm(0, -1.0f);
        } else {
            dComIfGp_event_reset();
            if (mOrderType != 3) {
                mShopCam.mCurrActionFunc.mVtableIndex = 0;
                mShopCam.mCurrActionFunc.mThisDelta = 0;
                mShopCam.mCurrActionFunc.mFunction = 0;
            }
            reinterpret_cast<daPy_py_c*>(player)->mNoResetFlg0 &= ~0x08000000u;
            mIsTalking = 0;
            setAnm(0, -1.0f);
        }
        if (mMsgNo == 0x1DB4)
            dComIfGs_onEventBit(0x1120);
    } else if (status == 8) {
        if (ds1Guest_daNpc_Ds1_shopMsgCheck(mMsgNo))
            ds1Guest_ShopItems_getSelectItemBuyMsg(&mShopItems);
    }
    if (gabi::at<ShopCursor_c>(mpShopCursor)->m54 != 0) {
        ((dComIfG_inf_c*)dComIfGp_get())->play.mDoStatusForce = 0x17;
        ((dComIfG_inf_c*)dComIfGp_get())->play.mAStatusForce = 0x27;
    }
    return mpMorf->mCurMorf < 1.0f;
}

/* 022311F4 */
BOOL daNpc_Ds1_c::wait_action(void*) {
    if (mActionStatus == 0) {
        mWaitState = 1;
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != -1) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        s16 a = (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]);
        pos->z = current.pos.z;
        pos->y = current.pos.y;
        mPlayerInRange = (u8)ds1Guest_attentionRegister(this, pos.get(), a); /* chkAttention: the result byte stored as is */
        s8 st = mWaitState;
        BOOL r;
        if ((u32)(s32)st == 1) {
            r = wait01();
        } else if ((u32)(s32)st == 2) {
            r = talk01();
        } else {
            r = 0;
        }
        lookBack();
        setAttention(r);
    }
    return TRUE;
}

/* WWHD glue
str 10019BD0 "Pdrgsh" in 02230420
str 101BE240 "Ds"
*/
