/**
 * d_a_npc_roten_exec.cpp (WWHD)
 * NPC - Traveling Merchants: _execute, the move procedures, attention and events.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_roten.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_roten.h"

// HD: the action tables use GHS eight-byte member pointers.

/* 022D5580 */
bool daNpcRoten_c::_execute() {
    chkAttention();
    checkOrder();
    if (!roten_dComIfGp_event_runCheck() ||
        (eventInfo.getCommand() == dEvtCmd_INTALK_e && (mEventFlags & 0x8000) == 0)) {
        (this->*moveProc.entries[mExecuteMode])(); /* (this->*moveProc[mExecuteMode])() */
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    speedF = mSpeedF;
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    setCollision(npc_dat(mNpcNo).mCylRadius, 200.0f);
    setCollisionH();
    NpcDatStruct& dat = npc_dat(mNpcNo);
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->set(current.pos.x, current.pos.y + dat.mAttnYOffset, current.pos.z);
    eyePos.set(current.pos.x, current.pos.y + dat.mEyeYOffset, current.pos.z);
    if (mNoBackboneTurn) {
        m_jnt.setParam(0, 0, 0, 0, dat.mMaxHeadX, dat.mMaxHeadY, dat.mMinHeadX, dat.mMinHeadY,
                       dat.mMaxTurnStep);
    } else {
        m_jnt.setParam(dat.mMaxBackboneX, dat.mMaxBackboneY, dat.mMinBackboneX, dat.mMinBackboneY,
                       dat.mMaxHeadX, dat.mMaxHeadY, dat.mMinHeadX, dat.mMinHeadY,
                       dat.mMaxTurnStep);
    }
    lookBack();
    setCollisionB();
    setMtx();
    return false;
}
/* 022D58F4 */
u8 daNpcRoten_c::executeCommon() {
    if (mCanTalk != 0 && mExecuteMode != MODE_WIND) {
        mOrderEvent = 1;
    } else {
        mOrderEvent = 0;
    }
    if (mTalking == 1) {
        executeSetMode(MODE_TALK);
    }
    return mTalking;
}
/* 022D3FDC */
void daNpcRoten_c::executeSetMode(u8 param_1) {
    mSpeedF = 0.0f;
    mExecuteMode =
        (u8)(this->*l_execute_init.entries[param_1])(); /* (this->*l_execute_init[param_1])() */
}
/* 022D5970 */
s32 daNpcRoten_c::executeWaitInit() {
    setAnmTbl(&l_npc_anm_wait);
    NpcDatStruct& dat = npc_dat(mNpcNo);
    f32 rnd = cM_rndF((f32)(dat.mWaitTimeMax - dat.mWaitTimeMin));
    mWaitTimer = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).mWaitTimeMin);
    return 0;
}
/* 022D5A48 */
void daNpcRoten_c::executeWait() {
    if (!executeCommon() && mPathRun.isPath() && mWaitTimer != 0 && !mInAttention) {
        s16 t = (s16)(mWaitTimer - 1);
        mWaitTimer = t;
        if (t == 0) {
            executeSetMode(MODE_TURN);
        }
    }
}
/* 022D674C */
s32 daNpcRoten_c::executeTalkInit() { return 1; }
/* 022D5ABC */
void daNpcRoten_c::executeTalk() {
    executeCommon();
    if (!dComIfGp_event_chkTalkXY() || dComIfGp_evmng_ChkPresentEnd()) {
        if (talk(1) == fopMsgStts_BOX_CLOSED_e) {
            mTalking = 0;
            executeSetMode(MODE_WAIT);
            dComIfGp_event_reset();
            mEventFlags = mEventFlags & ~0x4000;
            mBodyTurn = 0;
        } else {
            setAnmFromMsgTag();
        }
    }
}
/* 022D5B7C */
s32 daNpcRoten_c::executeWalkInit() {
    setAnmTbl(&l_npc_anm_walk);
    NpcDatStruct& dat = npc_dat(mNpcNo);
    f32 rnd = cM_rndF((f32)(dat.mWalkTimeMax - dat.mWalkTimeMin));
    mWaitTimer = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).mWalkTimeMin);
    return 2;
}
/* angle from the actor to the current path point (dNpc_calc_DisXZ_AngY on copies) */
static inline s16 roten_pathAngle(daNpcRoten_c* i_this) {
    gabi::Local<cXyz> point;
    roten_dNpc_PathRun_getPoint(&i_this->mPathRun, point, i_this->mPathRun.mIdx);
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> angle;
    a->x = i_this->current.pos.x;
    a->y = i_this->current.pos.y;
    a->z = i_this->current.pos.z;
    b->x = point->x;
    b->y = point->y;
    b->z = point->z;
    dNpc_calc_DisXZ_AngY(a, b, nullptr, angle);
    return *angle;
}

/* 022D5C54 */
void daNpcRoten_c::executeWalk() {
    if (!executeCommon()) {
        bool temp = false;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (roten_dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.getDir()) &&
            !roten_dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            temp = true;
        }
        if (mInAttention != 0) {
            executeSetMode(MODE_WAIT);
        } else if (!temp) {
            s16 angle = roten_pathAngle(this);
            NpcDatStruct& dat = npc_dat(mNpcNo);
            mLookAngleY = angle;
            mLookHeadOnly = 0;
            mLookMaxVelTarget = dat.mWalkHeadTurnVel;
            mLookMode = 2;
            m_jnt.mbTrn = 1; /* setTrn() */
            mSpeedF = dat.mWalkSpeed;
            if (mWaitTimer != 0) {
                s16 t = (s16)(mWaitTimer - 1);
                mWaitTimer = t;
                if (t == 0) {
                    executeSetMode(MODE_WAIT);
                }
            }
        } else {
            mPathRun.turnDir();
            executeSetMode(MODE_WAIT);
        }
    }
}
/* 022D5DC0 */
s32 daNpcRoten_c::executeTurnInit() {
    s32 ret = 3;
    s16 angle = roten_pathAngle(this);
    if (angle == current.angle.y) {
        ret = 2;
        setAnmTbl(&l_npc_anm_walk);
        NpcDatStruct& dat = npc_dat(mNpcNo);
        f32 rnd = cM_rndF((f32)(dat.mWalkTimeMax - dat.mWalkTimeMin));
        mWaitTimer = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).mWalkTimeMin);
    }
    return ret;
}
/* 022D5F10 */
void daNpcRoten_c::executeTurn() {
    if (!executeCommon()) {
        s16 angle = roten_pathAngle(this);
        mLookAngleY = angle;
        mLookHeadOnly = 0;
        mLookMode = 2;
        if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
            executeSetMode(MODE_WALK);
        }
    }
}
/* 022D5FC8 */
s32 daNpcRoten_c::executeWindInit() {
    setAnmTbl(l_npc_anm_wind.entries);
    J3DAnmTransform* pAnmRes = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), l_head_bck_ix_tbl.entries[mNpcNo]);
    mpHeadMorf->setAnm(pAnmRes, 0 /* EMode_NONE */, 14.0f, 1.0f, 0.0f, 39.0f, nullptr);
    return 4;
}
/* 022D6074 */
void daNpcRoten_c::executeWind() {
    if (!executeCommon()) {
        mpHeadMorf->play(nullptr, 0, 0);
        if (mAnmTblFlags & 1) {
            executeSetMode(MODE_WAIT);
            mWindHit = 0;
        }
    }
}
/* 022D3EA0 */
void daNpcRoten_c::checkOrder() {
    u16 command = eventInfo.getCommand();
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mExchange1stEventIdx) && mOrderEvent == 3) {
            mOrderEvent = 0;
        } else if (dComIfGp_evmng_startCheck(mExchange2ndEventIdx) && mOrderEvent == 4) {
            mOrderEvent = 0;
        } else if (dComIfGp_evmng_startCheck(mChangeItemEventIdx) && mOrderEvent == 5) {
            mOrderEvent = 0;
        } else if (dComIfGp_evmng_startCheck(mGetMapEventIdx) && mOrderEvent == 6) {
            mOrderEvent = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */ && (mOrderEvent == 2 || mOrderEvent == 1) &&
               mTalking == 0) {
        mTalking = 1;
        mSpeedF = 0.0f;
        mBodyTurn = 1;
    }
}
/* 022D4EF0 */
void daNpcRoten_c::eventOrder() {
    u8 order = mOrderEvent;
    if (order == 2 || order == 1) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (dComIfGs_isEventBit(save_dat(mNpcNo, 0))) {
            eventInfo.onCondition(dEvtCnd_CANTALKITEM_e);
        }
        if (mOrderEvent == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order == 3) {
        fopAcM_orderOtherEventId(this, mExchange1stEventIdx, 0xFF, 0xFF7F, 0, 1);
        mEventFlags = mEventFlags | 0x4000;
    } else if (order == 4) {
        fopAcM_orderOtherEventId(this, mExchange2ndEventIdx, 0xFF, 0xFF7F, 0, 1);
        mEventFlags = mEventFlags | 0x4000;
    } else if (order == 5) {
        fopAcM_orderOtherEventId(this, mChangeItemEventIdx, 0xFF, 0xFF7F, 0, 1);
        mEventFlags = mEventFlags | 0x4000;
    } else if (order == 6) {
        roten_fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, mGetMapEventIdx, 0, 0xFFFF);
        mEventFlags = mEventFlags | 0x4000;
        dComIfGs_setReserveItemEmpty();
    }
}
/* 022D4C40 */
void daNpcRoten_c::eventMove() {
    if (mEventFlags & 0x4000) {
        if (dComIfGp_evmng_endCheck(mExchange1stEventIdx)) {
            dComIfGp_event_reset();
            mEventFlags = mEventFlags & ~0xC000;
            executeSetMode(MODE_WAIT);
            mTalking = 0;
            if (mEventFlags & 8) {
                mOrderEvent = 2;
                eventOnPlrInit();
            } else {
                mOrderEvent = 4;
            }
        } else if (dComIfGp_evmng_endCheck(mExchange2ndEventIdx)) {
            dComIfGp_event_reset();
            mEventFlags = mEventFlags & ~0x4000;
            executeSetMode(MODE_WAIT);
            u16 f = mEventFlags;
            if (f & 0x40) {
                mEventFlags = f & ~0x40;
                eventOnPlrInit();
            } else if (f & 0x30) {
                mOrderEvent = 2;
                eventOnPlrInit();
            } else {
                mOrderEvent = 5;
            }
        } else if (dComIfGp_evmng_endCheck(mChangeItemEventIdx)) {
            mBodyTurn = 0;
            dComIfGp_event_reset();
            mEventFlags = mEventFlags & ~0x4000;
            executeSetMode(MODE_WAIT);
        } else if (dComIfGp_evmng_endCheck(mGetMapEventIdx)) {
            mTalking = 0;
            mBodyTurn = 0;
            dComIfGp_event_reset();
            mEventFlags = mEventFlags & ~0xC000;
            executeSetMode(MODE_WAIT);
        } else {
            u8 oldFlag = mEventCut.mbAttention;
            if (mEventCut.cutProc()) {
                if (!mEventCut.mbAttention) {
                    mEventCut.mbAttention = oldFlag;
                }
            } else {
                privateCut();
                setAnmFromMsgTag();
            }
        }
    }
}
/* 022D4828 */
void daNpcRoten_c::privateCut() {

    const char* staffName = l_npc_staff_id.entries[mNpcNo].get();
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    s8 actIdx =
        roten_dComIfGp_evmng_getMyActIdx(staffIdx, gabi::ea(cut_name_tbl.entries), 8, TRUE, 0);
    mCutNo = actIdx;
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch ((s32)mCutNo) {
        case 0: /* eventInit(): empty */
            break;
        case 1:
            eventMesSetInit(staffIdx);
            break;
        case 2:
            eventSetItemInit();
            break;
        case 3:
            eventClrItemInit();
            break;
        case 4:
            eventGetItemInit(staffIdx);
            break;
        case 5:
            eventSetAngleInit();
            break;
        case 6:
            eventOnPlrInit();
            break;
        case 7:
            eventOffPlrInit();
            break;
        }
    }
    bool temp;
    switch ((s32)mCutNo) {
    case 1:
        temp = eventMesSet();
        break;
    case 2:
        temp = eventSetItem();
        break;
    default:
        temp = true;
        break;
    }
    if (temp) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
    gabi::Local<gptr<fopAc_ac_c>> pItem;
    if (roten_fopAcM_SearchByID_o(mItemPid, pItem) && pItem->get() != nullptr) {
        if (mAnmIdx == ANM_WATASU) {
            if ((s16)gabi::ftoi(mpMorf->getFrame()) >= 0x3C) {
                Mtx34* mtx = getAnmMtx(mpMorf->getModel(), m_hand_L_jnt_num);
                PSMTXCopy(mtx, calc_mtx());
                mDoMtx_stack_c::transS(20.0f, -30.0f, -30.0f);
                roten_PSMTXConcat(calc_mtx(), mDoMtx_stack_c::get(), calc_mtx());
                Mtx34* m = calc_mtx();
                f32 x = m->m[0][3]; /* mDoMtx_multVecZero */
                f32 y = m->m[1][3];
                f32 z = m->m[2][3];
                RotenPresentItem* item = static_cast<RotenPresentItem*>(pItem->get());
                item->mOffsetPos.x = x; /* pItem->setOffsetPos(offset) */
                item->mOffsetPos.y = y;
                item->mOffsetPos.z = z;
                daItemBase_show(pItem->get());
                mHandItemPos.x = x;
                mHandItemPos.y = y;
                mHandItemPos.z = z;
            }
        } else if (mAnmIdx == ANM_ITEMWAIT) {
            mLookPos.copy(mHandItemPos);
            mLookHeadOnly = 1;
            mLookMode = 1;
            mBodyTurn = 0;
            mNoBackboneTurn = 1;
        }
    }
}
/* 022D40A4 */
void daNpcRoten_c::eventMesSetInit(int staffIdx) {
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "MsgNo" /* "MsgNo" */);
    if (pData) {
        u32 msgNo = *pData;
        switch (msgNo) {
        case 0x0:
            mpMsgList = l_msg_xy_koukan_item.entries[mNpcNo][mTradeItemIdx].get();
            setMessage(*mpMsgList);
            dComIfGs_onEventBit(save_dat(mNpcNo, 6));
            break;
        case 0x1:
            mpMsgList = l_msg_xy_koukan_item2.entries[mNpcNo][mTradeItemIdx].get();
            setMessage(*mpMsgList);
            break;
        case 0xA:
            mpMsgList = l_msg_xy_koukan_item3.entries[mNpcNo][mTradeItemIdx].get();
            setMessage(*mpMsgList);
            break;
        case 0x14:
            mpMsgList = l_msg_xy_after_get_demo.entries[mNpcNo].get();
            setMessage(*mpMsgList);
            break;
        case 0x15:
            mpMsgList = l_msg_xy_after_get_demo2.entries[mNpcNo].get();
            setMessage(*mpMsgList);
            break;
        case 0x63: {
            /* getMsg() is virtual (vtable +0x1C) */
            u32 vt = __vtbl;
            u32 no = gabi::call_ptr<u32>(gabi::at<RotenActorVtable>(vt)->getMsg, this);
            setMessage(no);
            break;
        }
        default:
            setMessage(msgNo);
            break;
        }
    } else {
        mpMsgList = mpMsgList.get() + 1;
        setMessage(*mpMsgList);
    }
}
/* 022D47A4 */
bool daNpcRoten_c::eventMesSet() {
    return talk(0) == fopMsgStts_BOX_CLOSED_e; /* fopMsgStts_BOX_CLOSED_e */
}
/* 022D4330 */
void daNpcRoten_c::eventSetItemInit() {
    u8 itemIdx = item_dat(mNpcNo, mTradeItemIdx);
    gabi::Local<cXyz> pos;
    pos->x = 0.0f;
    pos->z = 0.0f;
    pos->y = 0.0f;
    mItemPid =
        roten_fopAcM_createItemForPresentDemo(pos, (u8)(itemIdx + dItemNo_TOWN_FLOWER_e),
                                              9 /* FLAG_UNK01 | FLAG_UNK08 */, -1, current.roomNo);
}
/* 022D47D4 */
bool daNpcRoten_c::eventSetItem() {
    gabi::Local<gptr<fopAc_ac_c>> pActor;
    if (roten_fopAcM_SearchByID_o(mItemPid, pActor)) {
        if (pActor->get() != nullptr) {
            return true;
        }
        return false;
    }
    return true;
}
/* 022D45B4 */
void daNpcRoten_c::eventClrItemInit() {
    gabi::Local<gptr<fopAc_ac_c>> pItem;
    if (roten_fopAcM_SearchByID_o(mItemPid, pItem) && pItem->get() != nullptr) {
        daItemBase_dead(pItem->get());
    }
    mBodyTurn = 1;
    mNoBackboneTurn = 0;
    setAnmTbl(&l_npc_anm_wait);
}
/* 022D4620 */
void daNpcRoten_c::eventGetItemInit(int staffIdx) {
    u32 pcId;
    void* pData = dComIfGp_evmng_getMyIntegerP(staffIdx, "ItemNo" /* "ItemNo" */);
    if (pData != nullptr) {
        // HD: l_get_item_no has one entry, read without the index (GameCube l_get_item_no[*pData])
        u32 itemNo = (u32)l_get_item_no.itemNo;
        pcId = roten_createPresentItem(&current.pos, itemNo, 0, -1, (s32)current.roomNo, 0, 0);
    } else {
        u8 temp = item_dat(mNpcNo, mTradeItemIdx);
        dComIfGs_onGetItemReserve(temp);
        pcId =
            roten_fopAcM_createItemForPresentDemo(&current.pos, (u8)(temp + dItemNo_TOWN_FLOWER_e),
                                                  1 /* FLAG_UNK01 */, -1, current.roomNo);
    }
    if (pcId != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(pcId);
    }
}
/* 022D471C */
void daNpcRoten_c::eventSetAngleInit() {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> delta;
    cXyz_mi(&current.pos, delta, &player->current.pos);
    s16 angle = cM_atan2s(delta->x, delta->z);
    static_cast<daPy_py_c*>(player)->mDemo.mDemoMoveAngle =
        angle; /* player->changeDemoMoveAngle(angle) */
}
/* 022D4064 */
void daNpcRoten_c::eventOnPlrInit() {
    daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
    player->mNoResetFlg0 = (u32)player->mNoResetFlg0 & ~0x08000000u; /* offPlayerNoDraw() */
}
/* 022D4774 */
void daNpcRoten_c::eventOffPlrInit() {
    daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
    player->mNoResetFlg0 = (u32)player->mNoResetFlg0 | 0x08000000u; /* onPlayerNoDraw() */
}
/* 022D3B9C */
void daNpcRoten_c::chkAttention() {
    if (mEventCut.mbAttention) {
        mLookPos.x = mEventCut.mPos.x; /* mEventCut.getAttnPos() */
        mLookPos.y = mEventCut.mPos.y;
        mLookPos.z = mEventCut.mPos.z;
        mLookMode = 1;
        if (mBodyTurn != 0) {
            mLookHeadOnly = 0;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            mLookHeadOnly = 1;
        }
        if (mInAttention == 0) {
            mInAttention = 1;
            mCanTalk = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        NpcDatStruct& dat = npc_dat(mNpcNo);
        f32 temp = dat.mMaxAttnDistXZ;
        s32 temp2 = dat.mMaxAttnAngleY;
        s32 temp3 = 0x4000;
        if (mTalking != 0) {
            temp3 = 0x7FFF;
        }
        gabi::Local<cXyz> a, b;
        gabi::Local<be<f32>> dist;
        gabi::Local<be<s16>> angle;
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        b->x = player->current.pos.x;
        b->y = player->current.pos.y;
        b->z = player->current.pos.z;
        dNpc_calc_DisXZ_AngY(a, b, dist, angle);
        u8 b5 = mInAttention;
        if (b5 != 0) {
            temp += 40.0f;
            temp2 += 0x071C;
        }
        s16 temp5 = (s16)(*angle - current.angle.y);
        *angle = temp5;
        s32 absAngle = temp5 < 0 ? -temp5 : temp5;
        f32 d = *dist;
        if (temp > d && mWindHit == 0 && temp3 > absAngle) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos(eye, npc_dat(mNpcNo).mPlayerEyeOffsetY);
            mLookHeadOnly = mBodyTurn == 0;
            mLookPos.copy(*eye);
            mLookMode = 1;
            if (mInAttention == 0) {
                mInAttention = 1;
            }
            mCanTalk = 1;
        } else if (temp > d && mWindHit == 0 && temp2 > absAngle) {
            if (mCanTalk == 0) {
                mCanTalk = 1;
            }
            if (b5 == 1) {
                mInAttention = 0;
                mHomeTurnDelay = 0x1E;
            }
        } else {
            if (b5 == 1) {
                mInAttention = 0;
                mHomeTurnDelay = 0x1E;
            }
            mCanTalk = 0;
            mLookMode = 0;
            if (!mPathRun.isPath()) {
                if (mHomeTurnDelay != 0) {
                    mHomeTurnDelay = (s16)(mHomeTurnDelay - 1);
                } else {
                    mLookAngleY = home.angle.y;
                    mLookHeadOnly = 0;
                    mLookMode = 2;
                    m_jnt.mbTrn = 1; /* setTrn() */
                }
            }
        }
    }
    mLookMaxVelTarget = npc_dat(mNpcNo).mMaxHeadTurnVel;
}
/* 022D5374 */
void daNpcRoten_c::lookBack() {
    f32 eyeX = eyePos.x;
    f32 eyeY = eyePos.y;
    f32 eyeZ = eyePos.z;
    s16 temp1 = mLookMaxVelTarget;
    s16 targetY = current.angle.y;
    u8 temp3 = mLookHeadOnly;
    gabi::Local<cXyz> temp4;
    cXyz* dstPos = nullptr;
    switch ((s32)mLookMode) {
    case 1:
        temp4->copy(mLookPos);
        dstPos = temp4;
        break;
    case 2:
        targetY = mLookAngleY;
        break;
    }
    if (mTalking != 0 && mBodyTurn != 0) {
        temp3 = 0;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn) { /* trnChk() */
        s16 speed = mEventCut.mTurnSpeed;
        if (speed != 0) {
            temp1 = speed;
        }
        cLib_addCalcAngleS2(&mLookMaxVel, temp1, 4, 0x800);
    } else {
        mLookMaxVel = 0;
    }
    gabi::Local<cXyz> eye; /* eyePos, passed by value */
    eye->x = eyeX;
    eye->y = eyeY;
    eye->z = eyeZ;
    roten_dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, targetY, mLookMaxVel,
                                    temp3);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}

/* WWHD glue
str 1002155C "MsgNo" in 022D40A4
str 10021570 "ItemNo" in 022D4620
*/
