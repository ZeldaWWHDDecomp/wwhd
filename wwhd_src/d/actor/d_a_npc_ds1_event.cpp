/**
 * d_a_npc_ds1_event.cpp (WWHD)
 * NPC - Doc Bandam (Windfall Potion Shop shopkeeper): event cuts and event actions
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#include "d/actor/d_a_npc_ds1.h"

static inline s32 ds1EventActionIndex(s32 staffId, u32 table, s32 num, s32 force, s32 last) {
    return ds1Guest_eventActionIndex(dComIfGp_getPEvtManager(), staffId, table, num, force, last);
}
static inline u8 ds1EventAdvanced(s32 staffId) { return ds1Guest_eventAdvanced(dComIfGp_getPEvtManager(), staffId); }

/* daPy_py_c: player status word at +0x3B8; bit 0x08000000 hides the player (offPlayerNoDraw / on) */
static inline void daPy_offNoDraw(fopAc_ac_c* pl) {
    reinterpret_cast<daPy_py_c*>(pl)->mNoResetFlg0 &= ~0x08000000u;
}
static inline void daPy_onNoDraw(fopAc_ac_c* pl) {
    reinterpret_cast<daPy_py_c*>(pl)->mNoResetFlg0 |= 0x08000000u;
}
/* dNpc_HeadAnm_c (at 0x850) */

/* HD message manager (*0x101F4B5C) */

/* dComIfGp_setItemBeastNumCount(i, n): play+0x5B70 + 2i (s16) */
static inline void dComIfGp_addItemBeastNumCount(s32 i, s16 n) {
    auto& count = ((dComIfG_inf_c*)dComIfGp_get())->play.mItemBeastNumCounts[i];
    count = (s16)(count + n);
}
static inline s32* evt_int(s32 staffId, const char* name) {
    return (s32*)dComIfGp_evmng_getMySubstanceP(staffId, name, 3);
}
static inline s32 ldi(s32* p) { return *reinterpret_cast<be<s32>*>(p); }
static inline void ds1InvalidateEmitter(u32 e) {
    Ds1Emitter* emitter = gabi::at<Ds1Emitter>(e);
    u32 flags = emitter->mStatus;
    emitter->mMaxFrame = -1;
    emitter->mStatus = flags | 1;
}
static inline u8 gamma_u8(u32 c) { return (u8)gabi::ftoi(ds1Guest_gammaCorrect(c) * 255.0f); }

static inline void cutEnd(s32 staffId) { dComIfGp_evmng_cutEnd(staffId); }

/* l_msgId (0x10466EE0): the message set by evn_talk */

/* 022312F8 */
BOOL daNpc_Ds1_c::getdemo_action(void* arg) {
    s32 staffId = dComIfGp_evmng_getMyStaffId("Ds1", nullptr, 0);
    s8 status = mActionStatus;
    if (status == 0) {
        s32 item = mBoughtItemNo;
        if (item == 0xFF) {
            u8 itemNo = mItemNo;
            if (itemNo == DS1_RED_POTION)
                item = 0x51;
            else if (itemNo == DS1_GREEN_POTION)
                item = 0x52;
            else
                item = 0x53;
        }
        daPy_offNoDraw(dComIfGp_getPlayer(0));
        mShopCam.mCurrActionFunc.mFunction = 0;
        mShopCam.mCurrActionFunc.mVtableIndex = 0;
        mShopCam.mCurrActionFunc.mThisDelta = 0;
        mWaitState = mPreviousWaitState;
        u32 id = ds1Guest_fopAcM_createItemForPresentDemo(&current.pos, item, 0, -1, current.roomNo, nullptr, nullptr);
        if (id != 0xFFFFFFFF)
            dComIfGp_event_setItemPartnerId(id);
        cutEnd(staffId);
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (status != -1) {
        ds1Guest_fopMsgM_demoMsgFlagOn();
        cutEnd(staffId);
        if (dComIfGp_evmng_endCheck(mEventIdx[1])) {
            mOrderType = 1;
            if (mBoughtItemNo != 0xFF) {
                mPendingMsgNo = 0x1DDA;
                ds1Guest_ShopItems_showItem(&mShopItems);
                mBoughtItemNo = 0xFF;
                mShopItems.mSelectedItemIdx = -1;
                dComIfGp_event_reset();
            } else {
                u32 msg = 0x1DCD;
                if (ds1Guest_createDrugRegister((u32)mItemNo)) /* ds1Guest_daNpc_Ds1_checkCreateDrugChuchu */
                    msg = 0x1DC6;
                mPendingMsgNo = msg;
                dComIfGp_event_reset();
            }
            u8 itemNo = mItemNo;
            if (itemNo == DS1_GREEN_POTION)
                dComIfGs_onEventBit(DS1_GREEN_POTION_UNLOCKED);
            else if (itemNo == DS1_BLUE_POTION)
                dComIfGs_onEventBit(DS1_BLUE_POTION_UNLOCKED);
            ds1_setAction(this, ds1_tu.mWaitAction, nullptr);
        }
    }
    return TRUE;
}

/* 0223164C */
BOOL daNpc_Ds1_c::dummy_action(void* arg) {
    if (mActionStatus == 0)
        mActionStatus = 1;
    return TRUE;
}

/* 02231668 */
BOOL daNpc_Ds1_c::evn_talk_init(int staffId) {
    s32* msgNo = evt_int(staffId, "MsgNo");
    s32* endMsgNo = evt_int(staffId, "EndMsgNo");
    l_messageID.mValue = -1;
    mMsgNo = msgNo != nullptr ? ldi(msgNo) : 0;
    mEndMsgNo = endMsgNo != nullptr ? ldi(endMsgNo) : 0;
    return TRUE;
}

/* 02231710 */
BOOL daNpc_Ds1_c::evn_continue_talk_init(int staffId) {
    s32* endMsgNo = evt_int(staffId, "EndMsgNo");
    mEndMsgNo = endMsgNo != nullptr ? ldi(endMsgNo) : 0;
    return TRUE;
}

/* 02231774 */
BOOL daNpc_Ds1_c::evn_ItemModel_init(int staffId) {
    s32* p = evt_int(staffId, "prm");
    mItemModelFlags = (u8)(p != nullptr ? ldi(p) : 0);
    return TRUE;
}

/* 022317DC */
BOOL daNpc_Ds1_c::evn_head_swing_init(int staffId) {
    s32* p = evt_int(staffId, "prm");
    u32 type = p != nullptr ? ldi(p) : 0;
    if (type < 1)
        ds1Guest_dNpc_HeadAnm_swing_horizone_init(&mHeadAnm, 2, 0x1000, 0x1000, 1);
    else if (type == 1)
        ds1Guest_dNpc_HeadAnm_swing_vertical_init(&mHeadAnm, 2, 0x1000, 0x800, 1);
    return TRUE;
}

/* 0223188C */
BOOL daNpc_Ds1_c::evn_setAnm_init(int staffId) {
    s32* anmNo = evt_int(staffId, "AnmNo");
    s32* loopCnt = evt_int(staffId, "count");
    f32* morf = (f32*)dComIfGp_evmng_getMySubstanceP(staffId, "hokan", 0);
    if (anmNo != nullptr) {
        s8 idx = (s8)ldi(anmNo);
        f32 m = -1.0f;
        if (morf != nullptr)
            m = *reinterpret_cast<be<f32>*>(morf);
        setAnm(idx, m);
        mAnmLoopCount = loopCnt != nullptr ? (u8)(s8)ldi(loopCnt) : 0;
    }
    if (mAnmIdx == 6) {
        /* static GXColor prm[3], env[3] (guards 0x10466F88 / 0x10466F8C) */
        GXColor* prm = l_particlePrmColors.mColors;
        if (l_prmColorGuard.mValue == 0) {
            l_prmColorGuard.mValue = 1;
            prm[0].r = gamma_u8(0xBC);
            prm[0].g = gamma_u8(0x27);
            prm[0].b = gamma_u8(0x40);
            prm[1].r = gamma_u8(0x27);
            prm[1].g = gamma_u8(0xBC);
            prm[1].b = gamma_u8(0x40);
            prm[2].r = gamma_u8(0x27);
            prm[2].g = gamma_u8(0x63);
            prm[2].b = gamma_u8(0xBC);
        }
        GXColor* env = l_particleEnvColors.mColors;
        if (l_envColorGuard.mValue == 0) {
            l_envColorGuard.mValue = 1;
            env[0].r = gamma_u8(0xDE);
            env[0].g = gamma_u8(0x80);
            env[0].b = gamma_u8(0x38);
            env[1].r = gamma_u8(0x34);
            env[1].g = gamma_u8(0xDE);
            env[1].b = gamma_u8(0x38);
            env[2].r = gamma_u8(0x38);
            env[2].g = gamma_u8(0xDE);
            env[2].b = gamma_u8(0x80);
        }
        s32 i = 0;
        u8 itemNo = mItemNo;
        if (itemNo != DS1_RED_POTION)
            i = itemNo == DS1_GREEN_POTION ? 1 : 2;
        if (mMixEmitter == 0) {
            GXColor* p = &prm[i];
            GXColor* e = &env[i];
            mMixEmitter = gabi::ea(dComIfGp_particle_set(0x81BD, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, p, e, nullptr));
            mLight.mColorR = (s16)(p->r << 2);
            mLight.mColorG = (s16)(p->g << 2);
            mLight.mColorB = (s16)(p->b << 2);
        }
        if (mMixChildEmitter == 0) {
            GXColor* p = &prm[i];
            GXColor* e = &env[i];
            mMixChildEmitter = gabi::ea(dComIfGp_particle_set(0x81BE, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, p, e, nullptr));
        }
    }
    return TRUE;
}

/* 02231D0C */
BOOL daNpc_Ds1_c::evn_move_pos_init(int staffId) {
    u32 pos = gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, "pos", 1));
    evt_int(staffId, "angle");
    Ds1VectorWords& dst = ds1VectorWords(&mMovePos);
    u32 src = pos != 0 ? pos : gabi::ea(&home.pos);
    dst.x = gabi::at<Ds1VectorWords>(src)->x;
    dst.y = gabi::at<Ds1VectorWords>(src)->y;
    dst.z = gabi::at<Ds1VectorWords>(src)->z;
    return TRUE;
}

/* 02231DC8 */
BOOL daNpc_Ds1_c::evn_init_pos_init(int staffId) {
    u32 pos = gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, "pos", 1));
    s32* angle = evt_int(staffId, "angle");
    u32 src = pos != 0 ? pos : gabi::ea(&home.pos);
    Ds1VectorWords* words = gabi::at<Ds1VectorWords>(src);
    u32 x = words->x, y = words->y, z = words->z;
    ds1VectorWords(&old.pos).x = x;
    ds1VectorWords(&old.pos).y = y;
    ds1VectorWords(&old.pos).z = z;
    ds1VectorWords(&current.pos).x = x;
    ds1VectorWords(&current.pos).y = y;
    ds1VectorWords(&current.pos).z = z;
    if (angle != nullptr)
        current.angle.y = reinterpret_cast<be<s16>*>(angle)[1];
    else
        current.angle.y = home.angle.y;
    return TRUE;
}

/* 02231EBC */
BOOL daNpc_Ds1_c::evn_jnt_lock_init(int staffId) {
    s32* p = evt_int(staffId, "prm");
    u32 type = p != nullptr ? ldi(p) : 0;
    if (type < 2) {
        mJntCtrl.mbHeadLock = (u8)type;
        mJntCtrl.mbBackBoneLock = 0;
    } else if (type == 2) {
        mJntCtrl.mbHeadLock = 0;
        mJntCtrl.mbBackBoneLock = 1;
    } else if (type == 3) {
        mJntCtrl.mbHeadLock = 1;
        mJntCtrl.mbBackBoneLock = 1;
    }
    return TRUE;
}

/* 02231F6C */
BOOL daNpc_Ds1_c::evn_player_hide_init(int staffId) {
    s32* p = evt_int(staffId, "prm");
    u32 type = p != nullptr ? ldi(p) : 0;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (type < 1)
        daPy_offNoDraw(player);
    else if (type == 1)
        daPy_onNoDraw(player);
    return TRUE;
}

/* 02232000 */
BOOL daNpc_Ds1_c::evn_talk() {
    u32 mng = (u32)l_messageManager.mValue;
    if (l_messageID.mValue == -1) {
        u32 id = ds1Guest_fopMsgM_messageSet(mng, mMsgNo, &eyePos);
        l_messageID.mValue = id;
        if (id != 0xFFFFFFFF)
            ds1Guest_fopMsgM_demoMsgFlagOn();
        return FALSE;
    }
    setAnmFromMsgTag();
    if (ds1Guest_fopMsgM_SearchByID(mng) == 0xE) {
        u32 st = ds1Guest_nextMessageStatusRegister(this, &mMsgNo, 0); /* next_msgStatus(&mMsgNo, NULL): the whole r3 is passed on */
        ds1Guest_fopMsgM_setStatus(mng, st);
        if (ds1Guest_fopMsgM_SearchByID(mng) == 0xF)
            ds1Guest_fopMsgM_messageSet(mng, mMsgNo, nullptr);
        return FALSE;
    }
    if (ds1Guest_fopMsgM_SearchByID(mng) == 0x12) {
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DCA) {
            u8 itemNo = mItemNo;
            if (itemNo == DS1_RED_POTION)
                dComIfGp_addItemBeastNumCount(4, -10);
            else if (itemNo == DS1_GREEN_POTION)
                dComIfGp_addItemBeastNumCount(5, -15);
            else
                dComIfGp_addItemBeastNumCount(6, -15);
        } else if (msgNo == 0x1DC4) {
            u8 itemNo = mItemNo;
            if (itemNo == DS1_RED_POTION)
                dComIfGp_addItemBeastNumCount(4, -5);
            else
                dComIfGp_addItemBeastNumCount(itemNo == DS1_GREEN_POTION ? 5 : 6, -5);
        }
        ds1Guest_fopMsgM_setStatus(mng, 0x13);
        l_messageID.mValue = -1;
        return TRUE;
    }
    if (ds1Guest_fopMsgM_SearchByID(mng) == 2 || ds1Guest_fopMsgM_SearchByID(mng) == 6) {
        if (mMsgNo == mEndMsgNo) {
            mEndMsgNo = 0;
            return TRUE;
        }
    }
    return FALSE;
}

static inline f32 endFrameM1(mDoExt_McaMorf* m) { return (f32)(s32)m->mFrameCtrl.mEnd - 1.0f; }

/* 022322D0 */
BOOL daNpc_Ds1_c::evn_Anm() {
    BOOL pass = mpMorf->mFrameCtrl.checkPass(endFrameM1(mpMorf));
    s8 cnt = mAnmLoopCount;
    if (pass) {
        cnt = (s8)(cnt - 1);
        mAnmLoopCount = cnt;
    }
    if (cnt <= 0)
        return TRUE;
    s8 anm = mAnmIdx;
    if (anm == 6) {
        J3DModel* model = mpMorf->getModel();
        s32 jnt = mJntNo[2];
        J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
        u32 mtx = buffer->mpAnmMtx;
        buffer->mFlags |= 0x10;
        Mtx34* stk = mDoMtx_stack_c::get();
        PSMTXCopy(gabi::at<Mtx34>(jnt * 0x30 + mtx), stk);
        u32 e0 = mMixEmitter;
        f32 x = stk->m[0][3], z = stk->m[2][3], y = stk->m[1][3];
        if (e0 != 0) {
            f32 yy = y;
            if (gabi::at<Ds1Emitter>(e0)->mCoordinateSystem >= 7)
                yy = -yy;
            gabi::at<Ds1Emitter>(e0)->mGlobalTranslation.x = x;
            gabi::at<Ds1Emitter>(e0)->mGlobalTranslation.y = yy;
            gabi::at<Ds1Emitter>(e0)->mGlobalTranslation.z = z;
        }
        u32 e1 = mMixChildEmitter;
        if (e1 != 0) {
            f32 yy = y;
            if (gabi::at<Ds1Emitter>(e1)->mCoordinateSystem >= 7)
                yy = -yy;
            gabi::at<Ds1Emitter>(e1)->mGlobalTranslation.x = x;
            gabi::at<Ds1Emitter>(e1)->mGlobalTranslation.y = yy;
            gabi::at<Ds1Emitter>(e1)->mGlobalTranslation.z = z;
        }
        if (mpMorf->mFrameCtrl.checkPass(endFrameM1(mpMorf))) {
            if (mMixEmitter != 0) {
                ds1InvalidateEmitter(mMixEmitter);
                mMixEmitter = 0;
            }
            if (mMixChildEmitter != 0) {
                ds1InvalidateEmitter(mMixChildEmitter);
                mMixChildEmitter = 0;
            }
        }
        if (mpMorf->mFrameCtrl.checkPass(135.0f)) {
            gabi::Local<cXyz> dir;
            dir->y = 1.0f;
            dir->x = 0.0f;
            dir->z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x11, dir);
            mDoAud_seStart(0x69A0, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
            mLight.mPower = 100.0f;
            return FALSE;
        }
        if (mpMorf->mFrameCtrl.checkPass(10.0f))
            mItemModelFlags = mItemModelFlags | 2;
    } else if (anm == 8) {
        if (mpMorf->mFrameCtrl.checkPass(128.0f))
            mItemModelFlags = mItemModelFlags | 1;
    }
    return FALSE;
}

/* 022325D0 */
BOOL daNpc_Ds1_c::evn_move_pos() {
    ds1Guest_cLib_addCalcPos2(&current.pos, &mMovePos, 0.25f, 5.0f);
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> v;
    cXyz_mi(&mMovePos, d, &current.pos);
    u32 x = ds1VectorWords(d.get()).x, y = ds1VectorWords(d.get()).y, z = ds1VectorWords(d.get()).z;
    ds1VectorWords(v.get()).y = y;
    ds1VectorWords(v.get()).z = z;
    ds1VectorWords(v.get()).x = x;
    if (std_sqrtf(PSVECSquareMag(v)) < 1.0f)
        return TRUE;
    s16 angle = cLib_targetAngleY(&current.pos, &mMovePos);
    mMoveAngle = angle;
    cLib_addCalcAngleS2(&current.angle.y, angle, 4, 0x1000);
    return FALSE;
}

/* 02232698 */
BOOL daNpc_Ds1_c::privateCut() {
    s32 staffId = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
    if (staffId == -1)
        return FALSE;
    s32 actIdx = ds1EventActionIndex(staffId, gabi::ea(&l_cutNames), 10, 1, 0);
    if (actIdx == -1) {
        cutEnd(staffId);
        return TRUE;
    }
    if (ds1EventAdvanced(staffId)) {
        switch (actIdx) {
        case 0: evn_talk_init(staffId); break;
        case 1: evn_continue_talk_init(staffId); break;
        case 2: evn_ItemModel_init(staffId); break;
        case 3: evn_head_swing_init(staffId); break;
        case 5: evn_setAnm_init(staffId); break;
        case 6: evn_move_pos_init(staffId); break;
        case 7: evn_init_pos_init(staffId); break;
        case 8: evn_jnt_lock_init(staffId); break;
        case 9: evn_player_hide_init(staffId); break;
        }
    }
    BOOL end;
    if ((u32)actIdx <= 1)
        end = evn_talk();
    else if (actIdx == 5)
        end = evn_Anm();
    else if (actIdx == 6)
        end = evn_move_pos();
    else
        end = TRUE;
    if (end)
        cutEnd(staffId);
    return TRUE;
}

/* 022328E4 */
BOOL daNpc_Ds1_c::event_action(void* arg) {
    s8 status = mActionStatus;
    if (status == 0) {
        dComIfGp_evmng_getMyStaffId("Ds1", nullptr, 0);
        daPy_offNoDraw(dComIfGp_getPlayer(0));
        s8 st = mActionStatus;
        mShopCam.mCurrActionFunc.mVtableIndex = 0;
        mWaitState = mPreviousWaitState;
        mShopCam.mCurrActionFunc.mThisDelta = 0;
        mShopCam.mCurrActionFunc.mFunction = 0;
        mOrderType = 0;
        mActionStatus = (s8)(st + 1);
        return TRUE;
    }
    if (status == -1)
        return TRUE;
    privateCut();
    if (dComIfGp_evmng_endCheck(mEventIdx[mEventActionIdx])) {
        s32 a50 = mEventActionIdx;
        if (a50 == 2) {
            mOrderType = 4;
            mEventActionIdx = 0;
        } else if (a50 == 3) {
            if (mMsgNo == 0x1DC4) {
                mOrderType = 3;
                mEventActionIdx = 4;
            }
        } else if (a50 == 0) {
            mOrderType = 1;
            mPendingMsgNo = 0x1DCB;
            mEventActionIdx = 4;
        }
        dComIfGp_event_reset();
        ds1_setAction(this, ds1_tu.mWaitAction, nullptr);
    }
    lookBack();
    return TRUE;
}

/* WWHD glue
str 10019BD8 "Ds1" in 022312F8
str 10019BE0 "MsgNo" in 02231668
str 10019BE8 "EndMsgNo" in 02231668
str 10019BF4 "EndMsgNo" in 02231710
str 10019C00 "prm" in 02231774
str 10019C04 "prm" in 022317DC
str 10019C08 "AnmNo" in 0223188C
str 10019C10 "count" in 0223188C
str 10019C18 "hokan" in 0223188C
str 10019C20 "pos" in 02231D0C
str 10019C24 "angle" in 02231D0C
str 10019C2C "pos" in 02231DC8
str 10019C30 "angle" in 02231DC8
str 10019C38 "prm" in 02231EBC
str 10019C3C "prm" in 02231F6C
str 10019CB0 "Ds1" in 022328E4
str 101BE240 "Ds"
*/
