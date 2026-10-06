/**
 * d_a_npc_roten_exec.cpp (WWHD)
 * NPC - Traveling Merchants: _execute, the move procedures, attention and events.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_roten.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_roten.h"

/* static InitFunc_t l_execute_init[5] (.data 0x101C59C8), MoveFunc_t moveProc[5] (0x101C59F0):
 * GHS pointers to member functions, 8 bytes each */
enum : u32 { l_execute_init = 0x101C59C8, moveProc = 0x101C59F0 };

static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }

/* 022D5580 */
bool daNpcRoten_c::_execute() {
    WWHD_FUNC(0x022D5580, bool, this);
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || (eventInfo_getCommand(this) == 1 /* dEvtCmd_INTALK_e */ && (field_0x9B2 & 0x8000) == 0)) {
        roten_pmf_call(this, moveProc + field_0x9BB * 8); /* (this->*moveProc[field_0x9BB])() */
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    speedF = field_0x990;
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    setCollision(npc_dat(mNpcNo).field_0x2C, 200.0f);
    setCollisionH();
    RotenNpcDat& dat = npc_dat(mNpcNo);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->set(current.pos.x, current.pos.y + dat.field_0x1C, current.pos.z);
    eyePos.set(current.pos.x, current.pos.y + dat.field_0x28, current.pos.z);
    if (field_0x9C7) {
        m_jnt.setParam(0, 0, 0, 0, dat.field_0x04, dat.field_0x08, dat.field_0x0C, dat.field_0x10, dat.field_0x14);
    } else {
        m_jnt.setParam(dat.field_0x06, dat.field_0x0A, dat.field_0x0E, dat.field_0x12, dat.field_0x04, dat.field_0x08,
                       dat.field_0x0C, dat.field_0x10, dat.field_0x14);
    }
    lookBack();
    setCollisionB();
    setMtx();
    return false;
}
VERIFY(0x022D5580, &daNpcRoten_c::_execute);

/* 022D58F4 */
u8 daNpcRoten_c::executeCommon() {
    WWHD_FUNC(0x022D58F4, u8, this);
    if (field_0x9B6 != 0 && field_0x9BB != 4) {
        field_0x9B7 = 1;
    } else {
        field_0x9B7 = 0;
    }
    if (field_0x9B4 == 1) {
        executeSetMode(1);
    }
    return field_0x9B4;
}
VERIFY(0x022D58F4, &daNpcRoten_c::executeCommon);

/* 022D3FDC */
void daNpcRoten_c::executeSetMode(u8 param_1) {
    WWHD_FUNC(0x022D3FDC, void, this, param_1);
    field_0x990 = 0.0f;
    field_0x9BB = (u8)roten_pmf_call(this, l_execute_init + param_1 * 8); /* (this->*l_execute_init[param_1])() */
}
VERIFY(0x022D3FDC, &daNpcRoten_c::executeSetMode);


/* 022D5970 */
s32 daNpcRoten_c::executeWaitInit() {
    WWHD_FUNC(0x022D5970, s32, this);
    setAnmTbl(gabi::at<sRotenAnmDat>(l_npc_anm_wait));
    RotenNpcDat& dat = npc_dat(mNpcNo);
    f32 rnd = cM_rndF((f32)(dat.field_0x4E - dat.field_0x4C));
    field_0x9A6 = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).field_0x4C);
    return 0;
}
VERIFY(0x022D5970, &daNpcRoten_c::executeWaitInit);

/* 022D5A48 */
void daNpcRoten_c::executeWait() {
    WWHD_FUNC(0x022D5A48, void, this);
    if (!executeCommon() && mPathRun.isPath() && field_0x9A6 != 0 && !field_0x9B5) {
        s16 t = (s16)(field_0x9A6 - 1);
        field_0x9A6 = t;
        if (t == 0) {
            executeSetMode(3);
        }
    }
}
VERIFY(0x022D5A48, &daNpcRoten_c::executeWait);

/* 022D674C (unnamed by the matcher) */
s32 daNpcRoten_c::executeTalkInit() {
    WWHD_FUNC(0x022D674C, s32, this);
    return 1;
}
VERIFY(0x022D674C, &daNpcRoten_c::executeTalkInit);

/* 022D5ABC */
void daNpcRoten_c::executeTalk() {
    WWHD_FUNC(0x022D5ABC, void, this);
    executeCommon();
    if (!dComIfGp_event_chkTalkXY() || dComIfGp_evmng_ChkPresentEnd()) {
        if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            field_0x9B4 = 0;
            executeSetMode(0);
            dComIfGp_event_reset();
            field_0x9B2 = field_0x9B2 & ~0x4000;
            field_0x9C6 = 0;
        } else {
            setAnmFromMsgTag();
        }
    }
}
VERIFY(0x022D5ABC, &daNpcRoten_c::executeTalk);

/* 022D5B7C */
s32 daNpcRoten_c::executeWalkInit() {
    WWHD_FUNC(0x022D5B7C, s32, this);
    setAnmTbl(gabi::at<sRotenAnmDat>(l_npc_anm_walk));
    RotenNpcDat& dat = npc_dat(mNpcNo);
    f32 rnd = cM_rndF((f32)(dat.field_0x52 - dat.field_0x50));
    field_0x9A6 = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).field_0x50);
    return 2;
}
VERIFY(0x022D5B7C, &daNpcRoten_c::executeWalkInit);

/* angle from the actor to the current path point (dNpc_calc_DisXZ_AngY on copies) */
static inline s16 roten_pathAngle(daNpcRoten_c* i_this) {
    gabi::Local<cXyz> point;
    dNpc_PathRun_getPoint(&i_this->mPathRun, point, i_this->mPathRun.mIdx);
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
    WWHD_FUNC(0x022D5C54, void, this);
    if (!executeCommon()) {
        bool temp = false;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.getDir()) && !dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            temp = true;
        }
        if (field_0x9B5 != 0) {
            executeSetMode(0);
        } else if (!temp) {
            s16 angle = roten_pathAngle(this);
            RotenNpcDat& dat = npc_dat(mNpcNo);
            field_0x9B0 = angle;
            field_0x99C = 0;
            field_0x9AC = dat.field_0x18;
            field_0x9C4 = 2;
            m_jnt.mbTrn = 1; /* setTrn() */
            field_0x990 = dat.field_0x44;
            if (field_0x9A6 != 0) {
                s16 t = (s16)(field_0x9A6 - 1);
                field_0x9A6 = t;
                if (t == 0) {
                    executeSetMode(0);
                }
            }
        } else {
            mPathRun.turnDir();
            executeSetMode(0);
        }
    }
}
VERIFY(0x022D5C54, &daNpcRoten_c::executeWalk);

/* 022D5DC0 */
s32 daNpcRoten_c::executeTurnInit() {
    WWHD_FUNC(0x022D5DC0, s32, this);
    s32 ret = 3;
    s16 angle = roten_pathAngle(this);
    if (angle == current.angle.y) {
        ret = 2;
        setAnmTbl(gabi::at<sRotenAnmDat>(l_npc_anm_walk));
        RotenNpcDat& dat = npc_dat(mNpcNo);
        f32 rnd = cM_rndF((f32)(dat.field_0x52 - dat.field_0x50));
        field_0x9A6 = (s16)gabi::ftoi(rnd + (f32)npc_dat(mNpcNo).field_0x50);
    }
    return ret;
}
VERIFY(0x022D5DC0, &daNpcRoten_c::executeTurnInit);

/* 022D5F10 */
void daNpcRoten_c::executeTurn() {
    WWHD_FUNC(0x022D5F10, void, this);
    if (!executeCommon()) {
        s16 angle = roten_pathAngle(this);
        field_0x9B0 = angle;
        field_0x99C = 0;
        field_0x9C4 = 2;
        if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
            executeSetMode(2);
        }
    }
}
VERIFY(0x022D5F10, &daNpcRoten_c::executeTurn);

/* 022D5FC8 */
s32 daNpcRoten_c::executeWindInit() {
    WWHD_FUNC(0x022D5FC8, s32, this);
    setAnmTbl(gabi::at<sRotenAnmDat>(l_npc_anm_wind));
    J3DAnmTransform* pAnmRes =
        (J3DAnmTransform*)dComIfG_getObjectIDRes(arcname(mNpcNo), gabi::load<s32>(l_head_bck_ix_tbl + mNpcNo * 4));
    field_0x6D8->setAnm(pAnmRes, 0 /* EMode_NONE */, 14.0f, 1.0f, 0.0f, 39.0f, nullptr);
    return 4;
}
VERIFY(0x022D5FC8, &daNpcRoten_c::executeWindInit);

/* 022D6074 */
void daNpcRoten_c::executeWind() {
    WWHD_FUNC(0x022D6074, void, this);
    if (!executeCommon()) {
        field_0x6D8->play(nullptr, 0, 0);
        if (field_0x9C1 & 1) {
            executeSetMode(0);
            field_0x9CA = 0;
        }
    }
}
VERIFY(0x022D6074, &daNpcRoten_c::executeWind);

/* 022D3EA0 */
void daNpcRoten_c::checkOrder() {
    WWHD_FUNC(0x022D3EA0, void, this);
    u16 command = eventInfo_getCommand(this);
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(field_0x99E) && field_0x9B7 == 3) {
            field_0x9B7 = 0;
        } else if (dComIfGp_evmng_startCheck(field_0x9A0) && field_0x9B7 == 4) {
            field_0x9B7 = 0;
        } else if (dComIfGp_evmng_startCheck(field_0x9A2) && field_0x9B7 == 5) {
            field_0x9B7 = 0;
        } else if (dComIfGp_evmng_startCheck(field_0x9A4) && field_0x9B7 == 6) {
            field_0x9B7 = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */ && (field_0x9B7 == 2 || field_0x9B7 == 1) && field_0x9B4 == 0) {
        field_0x9B4 = 1;
        field_0x990 = 0.0f;
        field_0x9C6 = 1;
    }
}
VERIFY(0x022D3EA0, &daNpcRoten_c::checkOrder);

/* 022D4EF0 */
void daNpcRoten_c::eventOrder() {
    WWHD_FUNC(0x022D4EF0, void, this);
    u8 order = field_0x9B7;
    if (order == 2 || order == 1) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (dComIfGs_isEventBit(save_dat(mNpcNo, 0))) {
            eventInfo_onCondition(this, 0x20 /* dEvtCnd_CANTALKITEM_e */);
        }
        if (field_0x9B7 == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order == 3) {
        fopAcM_orderOtherEventId(this, field_0x99E, 0xFF, 0xFF7F, 0, 1);
        field_0x9B2 = field_0x9B2 | 0x4000;
    } else if (order == 4) {
        fopAcM_orderOtherEventId(this, field_0x9A0, 0xFF, 0xFF7F, 0, 1);
        field_0x9B2 = field_0x9B2 | 0x4000;
    } else if (order == 5) {
        fopAcM_orderOtherEventId(this, field_0x9A2, 0xFF, 0xFF7F, 0, 1);
        field_0x9B2 = field_0x9B2 | 0x4000;
    } else if (order == 6) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, field_0x9A4, 0, 0xFFFF);
        field_0x9B2 = field_0x9B2 | 0x4000;
        dComIfGs_setReserveItemEmpty();
    }
}
VERIFY(0x022D4EF0, &daNpcRoten_c::eventOrder);

/* 022D4C40 */
void daNpcRoten_c::eventMove() {
    WWHD_FUNC(0x022D4C40, void, this);
    if (field_0x9B2 & 0x4000) {
        if (dComIfGp_evmng_endCheck(field_0x99E)) {
            dComIfGp_event_reset();
            field_0x9B2 = field_0x9B2 & ~0xC000;
            executeSetMode(0);
            field_0x9B4 = 0;
            if (field_0x9B2 & 8) {
                field_0x9B7 = 2;
                eventOnPlrInit();
            } else {
                field_0x9B7 = 4;
            }
        } else if (dComIfGp_evmng_endCheck(field_0x9A0)) {
            dComIfGp_event_reset();
            field_0x9B2 = field_0x9B2 & ~0x4000;
            executeSetMode(0);
            u16 f = field_0x9B2;
            if (f & 0x40) {
                field_0x9B2 = f & ~0x40;
                eventOnPlrInit();
            } else if (f & 0x30) {
                field_0x9B7 = 2;
                eventOnPlrInit();
            } else {
                field_0x9B7 = 5;
            }
        } else if (dComIfGp_evmng_endCheck(field_0x9A2)) {
            field_0x9C6 = 0;
            dComIfGp_event_reset();
            field_0x9B2 = field_0x9B2 & ~0x4000;
            executeSetMode(0);
        } else if (dComIfGp_evmng_endCheck(field_0x9A4)) {
            field_0x9B4 = 0;
            field_0x9C6 = 0;
            dComIfGp_event_reset();
            field_0x9B2 = field_0x9B2 & ~0xC000;
            executeSetMode(0);
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
VERIFY(0x022D4C40, &daNpcRoten_c::eventMove);

/* 022D4828 */
void daNpcRoten_c::privateCut() {
    WWHD_FUNC(0x022D4828, void, this);
    /* static char* cut_name_tbl[8] (.data 0x101C600C): INIT, MES_SET, SET_ITEM, CLR_ITEM, GET_ITEM,
     * SET_ANGLE, ON_PLR, OFF_PLR */
    const char* staffName = gabi::at<const char>(gabi::load<u32>(l_npc_staff_id + mNpcNo * 4));
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    s8 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101C600C, 8, TRUE, 0);
    field_0x9C3 = actIdx;
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch ((s32)field_0x9C3) {
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
    switch ((s32)field_0x9C3) {
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
    if (fopAcM_SearchByID_o(field_0x6F8, pItem) && pItem->get() != nullptr) {
        if (field_0x9C0 == 7) {
            if ((s16)gabi::ftoi(mpMorf->getFrame()) >= 0x3C) {
                Mtx34* mtx = getAnmMtx(mpMorf->getModel(), m_hand_L_jnt_num);
                PSMTXCopy(mtx, calc_mtx());
                mDoMtx_stack_c::transS(20.0f, -30.0f, -30.0f);
                PSMTXConcat(calc_mtx(), mDoMtx_stack_c::get(), calc_mtx());
                Mtx34* m = calc_mtx();
                f32 x = m->m[0][3]; /* mDoMtx_multVecZero */
                f32 y = m->m[1][3];
                f32 z = m->m[2][3];
                u32 item = gabi::ea(pItem->get());
                gabi::store<f32>(item + 0x760, x); /* pItem->setOffsetPos(offset) */
                gabi::store<f32>(item + 0x764, y);
                gabi::store<f32>(item + 0x768, z);
                daItemBase_show(pItem->get());
                field_0x714.x = x;
                field_0x714.y = y;
                field_0x714.z = z;
            }
        } else if (field_0x9C0 == 8) {
            field_0x708.copy(field_0x714);
            field_0x99C = 1;
            field_0x9C4 = 1;
            field_0x9C6 = 0;
            field_0x9C7 = 1;
        }
    }
}
VERIFY(0x022D4828, &daNpcRoten_c::privateCut);

/* 022D40A4 */
void daNpcRoten_c::eventMesSetInit(int staffIdx) {
    WWHD_FUNC(0x022D40A4, void, this, staffIdx);
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1002155C) /* "MsgNo" */);
    if (pData) {
        u32 msgNo = *pData;
        switch (msgNo) {
        case 0x0:
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5E50 + (mNpcNo * 12 + field_0x9BE) * 4)); /* l_msg_xy_koukan_item */
            setMessage(*field_0x98C);
            dComIfGs_onEventBit(save_dat(mNpcNo, 6));
            break;
        case 0x1:
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5EE0 + (mNpcNo * 12 + field_0x9BE) * 4)); /* l_msg_xy_koukan_item2 */
            setMessage(*field_0x98C);
            break;
        case 0xA:
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5F70 + (mNpcNo * 12 + field_0x9BE) * 4)); /* l_msg_xy_koukan_item3 */
            setMessage(*field_0x98C);
            break;
        case 0x14:
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5D08 + mNpcNo * 4)); /* l_msg_xy_after_get_demo */
            setMessage(*field_0x98C);
            break;
        case 0x15:
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5D14 + mNpcNo * 4)); /* l_msg_xy_after_get_demo2 */
            setMessage(*field_0x98C);
            break;
        case 0x63: {
            /* getMsg() is virtual (vtable +0x1C) */
            u32 vt = __vtbl;
            u32 no = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0x1C), this);
            setMessage(no);
            break;
        }
        default:
            setMessage(msgNo);
            break;
        }
    } else {
        field_0x98C = gabi::at<be<u32>>(gabi::ea(field_0x98C.get()) + 4);
        setMessage(*field_0x98C);
    }
}
VERIFY(0x022D40A4, &daNpcRoten_c::eventMesSetInit);

/* 022D47A4 */
bool daNpcRoten_c::eventMesSet() {
    WWHD_FUNC(0x022D47A4, bool, this);
    return talk(0) == 0x12; /* fopMsgStts_BOX_CLOSED_e */
}
VERIFY(0x022D47A4, &daNpcRoten_c::eventMesSet);

/* 022D4330 */
void daNpcRoten_c::eventSetItemInit() {
    WWHD_FUNC(0x022D4330, void, this);
    u8 itemIdx = item_dat(mNpcNo, field_0x9BE);
    gabi::Local<cXyz> pos;
    pos->x = 0.0f;
    pos->z = 0.0f;
    pos->y = 0.0f;
    field_0x6F8 = fopAcM_createItemForPresentDemo(pos, (u8)(itemIdx + 0x8C /* dItemNo_TOWN_FLOWER_e */),
                                                  9 /* FLAG_UNK01 | FLAG_UNK08 */, -1, current.roomNo);
}
VERIFY(0x022D4330, &daNpcRoten_c::eventSetItemInit);

/* 022D47D4 */
bool daNpcRoten_c::eventSetItem() {
    WWHD_FUNC(0x022D47D4, bool, this);
    gabi::Local<gptr<fopAc_ac_c>> pActor;
    if (fopAcM_SearchByID_o(field_0x6F8, pActor)) {
        if (pActor->get() != nullptr) {
            return true;
        }
        return false;
    }
    return true;
}
VERIFY(0x022D47D4, &daNpcRoten_c::eventSetItem);

/* 022D45B4 */
void daNpcRoten_c::eventClrItemInit() {
    WWHD_FUNC(0x022D45B4, void, this);
    gabi::Local<gptr<fopAc_ac_c>> pItem;
    if (fopAcM_SearchByID_o(field_0x6F8, pItem) && pItem->get() != nullptr) {
        daItemBase_dead(pItem->get());
    }
    field_0x9C6 = 1;
    field_0x9C7 = 0;
    setAnmTbl(gabi::at<sRotenAnmDat>(l_npc_anm_wait));
}
VERIFY(0x022D45B4, &daNpcRoten_c::eventClrItemInit);

/* 022D4620 */
void daNpcRoten_c::eventGetItemInit(int staffIdx) {
    WWHD_FUNC(0x022D4620, void, this, staffIdx);
    u32 pcId;
    void* pData = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10021570) /* "ItemNo" */);
    if (pData != nullptr) {
        /* HD: l_get_item_no has one entry, read without the index (GameCube l_get_item_no[*pData]) */
        u32 itemNo = gabi::load<u32>(0x101C546C);
        pcId = gabi::call<u32>(0x025D7DEC, &current.pos, itemNo, 0, -1, (s32)current.roomNo, (u32)0, (u32)0);
    } else {
        u8 temp = item_dat(mNpcNo, field_0x9BE);
        dComIfGs_onGetItemReserve(temp);
        pcId = fopAcM_createItemForPresentDemo(&current.pos, (u8)(temp + 0x8C), 1 /* FLAG_UNK01 */, -1, current.roomNo);
    }
    if (pcId != 0xFFFFFFFF) {
        dComIfGp_event_setItemPartnerId(pcId);
    }
}
VERIFY(0x022D4620, &daNpcRoten_c::eventGetItemInit);

/* 022D471C */
void daNpcRoten_c::eventSetAngleInit() {
    WWHD_FUNC(0x022D471C, void, this);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> delta;
    cXyz_mi(&current.pos, delta, &player->current.pos);
    s16 angle = cM_atan2s(delta->x, delta->z);
    gabi::store<s16>(gabi::ea(player) + 0x422, angle); /* player->changeDemoMoveAngle(angle) */
}
VERIFY(0x022D471C, &daNpcRoten_c::eventSetAngleInit);

/* 022D4064 (the matcher calls it daDitem_c::setOffsetPos) */
void daNpcRoten_c::eventOnPlrInit() {
    WWHD_FUNC(0x022D4064, void, this);
    u32 player = gabi::ea(dComIfGp_getLinkPlayer());
    gabi::store<u32>(player + 0x3B8, gabi::load<u32>(player + 0x3B8) & ~0x08000000u); /* offPlayerNoDraw() */
}
VERIFY(0x022D4064, &daNpcRoten_c::eventOnPlrInit);

/* 022D4774 */
void daNpcRoten_c::eventOffPlrInit() {
    WWHD_FUNC(0x022D4774, void, this);
    u32 player = gabi::ea(dComIfGp_getLinkPlayer());
    gabi::store<u32>(player + 0x3B8, gabi::load<u32>(player + 0x3B8) | 0x08000000u); /* onPlayerNoDraw() */
}
VERIFY(0x022D4774, &daNpcRoten_c::eventOffPlrInit);

/* 022D3B9C */
void daNpcRoten_c::chkAttention() {
    WWHD_FUNC(0x022D3B9C, void, this);
    if (mEventCut.mbAttention) {
        field_0x708.x = mEventCut.mPos.x; /* mEventCut.getAttnPos() */
        field_0x708.y = mEventCut.mPos.y;
        field_0x708.z = mEventCut.mPos.z;
        field_0x9C4 = 1;
        if (field_0x9C6 != 0) {
            field_0x99C = 0;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            field_0x99C = 1;
        }
        if (field_0x9B5 == 0) {
            field_0x9B5 = 1;
            field_0x9B6 = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        RotenNpcDat& dat = npc_dat(mNpcNo);
        f32 temp = dat.field_0x24;
        s32 temp2 = dat.field_0x20;
        s32 temp3 = 0x4000;
        if (field_0x9B4 != 0) {
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
        u8 b5 = field_0x9B5;
        if (b5 != 0) {
            temp += 40.0f;
            temp2 += 0x071C;
        }
        s16 temp5 = (s16)(*angle - current.angle.y);
        *angle = temp5;
        s32 absAngle = temp5 < 0 ? -temp5 : temp5;
        f32 d = *dist;
        if (temp > d && field_0x9CA == 0 && temp3 > absAngle) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_r(eye, npc_dat(mNpcNo).field_0x00);
            field_0x99C = field_0x9C6 == 0;
            field_0x708.copy(*eye);
            field_0x9C4 = 1;
            if (field_0x9B5 == 0) {
                field_0x9B5 = 1;
            }
            field_0x9B6 = 1;
        } else if (temp > d && field_0x9CA == 0 && temp2 > absAngle) {
            if (field_0x9B6 == 0) {
                field_0x9B6 = 1;
            }
            if (b5 == 1) {
                field_0x9B5 = 0;
                field_0x9A8 = 0x1E;
            }
        } else {
            if (b5 == 1) {
                field_0x9B5 = 0;
                field_0x9A8 = 0x1E;
            }
            field_0x9B6 = 0;
            field_0x9C4 = 0;
            if (!mPathRun.isPath()) {
                if (field_0x9A8 != 0) {
                    field_0x9A8 = (s16)(field_0x9A8 - 1);
                } else {
                    field_0x9B0 = home.angle.y;
                    field_0x99C = 0;
                    field_0x9C4 = 2;
                    m_jnt.mbTrn = 1; /* setTrn() */
                }
            }
        }
    }
    field_0x9AC = npc_dat(mNpcNo).field_0x16;
}
VERIFY(0x022D3B9C, &daNpcRoten_c::chkAttention);

/* 022D5374 */
void daNpcRoten_c::lookBack() {
    WWHD_FUNC(0x022D5374, void, this);
    f32 eyeX = eyePos.x;
    f32 eyeY = eyePos.y;
    f32 eyeZ = eyePos.z;
    s16 temp1 = field_0x9AC;
    s16 targetY = current.angle.y;
    u8 temp3 = field_0x99C;
    gabi::Local<cXyz> temp4;
    cXyz* dstPos = nullptr;
    switch ((s32)field_0x9C4) {
    case 1:
        temp4->copy(field_0x708);
        dstPos = temp4;
        break;
    case 2:
        targetY = field_0x9B0;
        break;
    }
    if (field_0x9B4 != 0 && field_0x9C6 != 0) {
        temp3 = 0;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn) { /* trnChk() */
        s16 speed = mEventCut.mTurnSpeed;
        if (speed != 0) {
            temp1 = speed;
        }
        cLib_addCalcAngleS2(&field_0x9AE, temp1, 4, 0x800);
    } else {
        field_0x9AE = 0;
    }
    gabi::Local<cXyz> eye; /* eyePos, passed by value */
    eye->x = eyeX;
    eye->y = eyeY;
    eye->z = eyeZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, targetY, field_0x9AE, temp3);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
VERIFY(0x022D5374, &daNpcRoten_c::lookBack);
