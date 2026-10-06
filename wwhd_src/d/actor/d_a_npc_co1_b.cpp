/**
 * d_a_npc_co1_b.cpp (WWHD)
 * NPC - Prince Komali (before Dragon Roost Cavern): part B (events, execute, draw, messages,
 * actions, compiler-generated tail)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_co1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_co1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, the pointer re-read at each use */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* play object fields (dComIfGp_get() at each use) */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* event manager (play + 0x52C4) */
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 02544950 dEvent_manager_c::ChkPresentEnd, 02544980 CancelPresent */
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline void dComIfGp_evmng_CancelPresent() { gabi::call(0x02544980, dComIfGp_getPEvtManager()); }
/* 025B7270 dSv_player_bag_item_c::setReserveItemEmpty (save info + 0x96) */
static inline void dComIfGs_setReserveItemEmpty() { gabi::call(0x025B7270, gabi::load<u32>(0x101F84DC) + 0x96); }
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status */
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* dAttention_c (play + 0x5804): 024EC8D0 LockonTarget, 024EE464 ActionTarget (matcher swaps them) */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* d_npc */
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 desiredYrot, s16 maxVel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D734 dNpc_chkLetterPassed() */
static inline BOOL dNpc_chkLetterPassed() { return gabi::call<BOOL>(0x0259D734); }
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }

/* ---- file statics ---- */
/* daNpc_Co1_HIO_c, HD: vtable at 0; mPrmTbl (0x2C bytes) at +0xC */
struct daNpc_Co1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ be<s16> mMaxHeadX;
    /* 0x0E */ be<s16> mMaxHeadY;
    /* 0x10 */ be<s16> mMinHeadX;
    /* 0x12 */ be<s16> mMinHeadY;
    /* 0x14 */ be<s16> mMaxBackboneX;
    /* 0x16 */ be<s16> mMaxBackboneY;
    /* 0x18 */ be<s16> mMinBackboneX;
    /* 0x1A */ be<s16> mMinBackboneY;
    /* 0x1C */ be<s16> mMaxTurnStep;
    /* 0x1E */ be<s16> mCalcAngleTarget;
    /* 0x20 */ u8 _20[4];
    /* 0x24 */ be<u8> mDebugDraw;     /* _draw: debug leftovers */
    /* 0x25 */ u8 _25[0x38 - 0x25];
};
WWHD_SIZE(daNpc_Co1_HIO_c, 0x38);
static daNpc_Co1_HIO_c& l_HIO() { return *gabi::at<daNpc_Co1_HIO_c>(0x10466CB0); }

/* (this->*pmf)(arg) */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

static inline s32 morfFrameInt(mDoExt_McaMorf* morf) { return gabi::ftoi(morf->getFrame()); }

/* 02228CE4 */
void daNpc_Co1_c::eInit_MDR_() {
    WWHD_FUNC(0x02228CE4, void, this);
    setAnm_NUM(0xA, 1);
    mA33 = 0;
}
VERIFY(0x02228CE4, &daNpc_Co1_c::eInit_MDR_);

/* 02228D20 */
void daNpc_Co1_c::eInit_RED_LTR_() {
    WWHD_FUNC(0x02228D20, void, this);
    setAnm_NUM(8, 1);
}
VERIFY(0x02228D20, &daNpc_Co1_c::eInit_RED_LTR_);

/* 02228D2C */
void daNpc_Co1_c::event_actionInit(int i_staff_idx) {
    WWHD_FUNC(0x02228D2C, void, this, i_staff_idx);
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staff_idx, STR(0x10019600), 3); /* "ActNo" */
    if (act_no_p == nullptr) {
        return;
    }
    s8 act = (s8)*act_no_p;
    mA29 = act;
    switch ((u32)(s32)act) {
    case 0:
        eInit_MDR_();
        break;
    case 1:
        eInit_RED_LTR_();
        break;
    }
}
VERIFY(0x02228D2C, &daNpc_Co1_c::event_actionInit);

/* 02228DDC */
u32 daNpc_Co1_c::eMove_MDR_() {
    WWHD_FUNC(0x02228DDC, u32, this);
    bool ret = morfFrameInt(mpMorf) == 0;
    if (ret) {
        setAnm_NUM(0xB, 1);
    }
    return ret;
}
VERIFY(0x02228DDC, &daNpc_Co1_c::eMove_MDR_);

/* 02228E34 */
u32 daNpc_Co1_c::eMove_RED_LTR_() {
    WWHD_FUNC(0x02228E34, u32, this);
    bool ret = false;
    if (m9D8 != 0) {
        switch ((u32)(s32)mA2D) {
        case 4:
            mA32 = 7;
            setStt(2);
            ret = true;
            break;
        case 8:
            setAnm_NUM(4, 1);
            break;
        default:
            setAnm_NUM(8, 1);
            break;
        }
    }
    return ret;
}
VERIFY(0x02228E34, &daNpc_Co1_c::eMove_RED_LTR_);

/* 02228EB8 */
u32 daNpc_Co1_c::event_action() {
    WWHD_FUNC(0x02228EB8, u32, this);
    switch ((u32)(s32)mA29) {
    case 0:
        return eMove_MDR_();
    case 1:
        return eMove_RED_LTR_();
    default:
        return 1;
    }
}
VERIFY(0x02228EB8, &daNpc_Co1_c::event_action);

/* 02228EFC */
void daNpc_Co1_c::privateCut(int i_staff_idx) {
    WWHD_FUNC(0x02228EFC, void, this, i_staff_idx);
    /* static char* a_cut_tbl[] = {"ACTION"} (.data 0x101BDDA0) */
    if (i_staff_idx == -1) {
        return;
    }
    mA28 = dComIfGp_evmng_getMyActIdx(i_staff_idx, 0x101BDDA0, 1, TRUE, 0);
    if (mA28 == -1) {
        dComIfGp_evmng_cutEnd(i_staff_idx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staff_idx)) {
        if (mA28 == 0) {
            event_actionInit(i_staff_idx);
        }
    }
    bool endCut;
    if (mA28 == 0) {
        endCut = event_action();
    } else {
        endCut = true;
    }
    if (endCut) {
        dComIfGp_evmng_cutEnd(i_staff_idx);
    }
}
VERIFY(0x02228EFC, &daNpc_Co1_c::privateCut);

/* 02228FD0 */
void daNpc_Co1_c::lookBack() {
    WWHD_FUNC(0x02228FD0, void, this);
    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    m9BE = m_jnt.mAngles[1][1];
    s16 desiredYrot = current.angle.y;
    m9C0 = desiredYrot;
    m9BC = m_jnt.mAngles[0][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    dstPos_p = nullptr;
    u8 headOnlyFollow = m9EE;

    switch ((u32)(s32)mA33) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case 2:
        dstPos->copy(m98C);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        break;
    case 3:
        desiredYrot = m9D4;
        break;
    }
    cLib_addCalcAngleS2(&m9D2, l_HIO().mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        m9D2 = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, m9D2, headOnlyFollow);
}
VERIFY(0x02228FD0, &daNpc_Co1_c::lookBack);

/* 02229210 */
void daNpc_Co1_c::event_proc(int i_staff_idx) {
    WWHD_FUNC(0x02229210, void, this, i_staff_idx);
    if (dComIfGp_evmng_endCheck(mEventIdx[m9C8])) {
        switch ((u32)(s32)m9C8) {
        case 0:
            dComIfGs_onEventBit(0x0F04);
            setStt(4);
            break;
        case 1:
            dComIfGs_onEventBit(0x1810);
            break;
        case 2:
            mA30 = 1;
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staff_idx);
        }
        lookBack();
    }
}
VERIFY(0x02229210, &daNpc_Co1_c::event_proc);

/* 02229378 */
void daNpc_Co1_c::eventOrder() {
    WWHD_FUNC(0x02229378, void, this);
    s8 condition = mA30;
    if (condition == 1 || condition == 2) {
        s8 c = mA30;
        u32 a = gabi::ea(this) + 0xFA; /* eventInfo.mCondition */
        gabi::store<u16>(a, gabi::load<u16>(a) | 0x21); /* onCondition(CANTALK | CANTALKITEM) */
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        s16 idx = condition - 3;
        m9C8 = idx;
        fopAcM_orderOtherEventId(this, mEventIdx[idx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02229378, &daNpc_Co1_c::eventOrder);

/* 022293E8 */
void daNpc_Co1_c::setCollision_SP_() {
    WWHD_FUNC(0x022293E8, void, this);
    if (m9ED != 0) {
        return;
    }
    if (mA2D == 1) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        gabi::Local<cXyz> off;
        off->x = 0.0f;
        off->y = 0.0f;
        off->z = -20.0f;
        gabi::Local<cXyz> center;
        PSMTXMultVec(mDoMtx_stack_c::get(), off, center);
        mCyl.SetC(center);
        mCyl.SetR(70.0f);
        mCyl.SetH(60.0f);
        dComIfG_Ccsp_Set(&mCyl);
    } else {
        gabi::Local<cXyz> center;
        center->x = current.pos.x;
        center->y = current.pos.y;
        center->z = current.pos.z;
        mCyl.SetC(center);
        mCyl.SetR(40.0f);
        mCyl.SetH(90.0f);
        dComIfG_Ccsp_Set(&mCyl);
    }
}
VERIFY(0x022293E8, &daNpc_Co1_c::setCollision_SP_);

/* 02229558 */
BOOL daNpc_Co1_c::_execute() {
    WWHD_FUNC(0x02229558, BOOL, this);
    if (!m9E5) {
        m978 = current.angle.x;
        m97A = current.angle.y;
        m97C = current.angle.z;
        m96C.copy(current.pos);
        m9E5 = 1;
    }
    daNpc_Co1_HIO_c& prm = l_HIO();
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m9DD && demoActorID == 0) {
        return TRUE;
    }
    m9E0 = 0;
    m9DD = 0;
    checkOrder();
    if (!demo()) {
        s32 cond = -1;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* eventInfo.mCommand != dEvtCmd_INTALK_e */) {
            cond = isEventEntry();
        }
        if (cond >= 0) {
            event_proc(cond);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!m9E0) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (!m9DF) {
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(0);
    if (!m9EF) {
        setCollision_SP_();
    }
    return TRUE;
}
VERIFY(0x02229558, &daNpc_Co1_c::_execute);

/* 02229734 */
static BOOL daNpc_Co1_Execute(daNpc_Co1_c* i_this) {
    WWHD_FUNC(0x02229734, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02229734, daNpc_Co1_Execute);

/* 02229738 */
BOOL daNpc_Co1_c::_draw() {
    WWHD_FUNC(0x02229738, BOOL, this);
    J3DModelData* model_data = gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(mpMorf->getModel()) + 0xAC));
    if (m7E8 == 0) {
        return TRUE;
    }
    mDoExt_McaMorf* prlMorf = gabi::at<mDoExt_McaMorf>(m7E8);
    J3DModel* prlModel = prlMorf->getModel();
    J3DModelData* prl_data = gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(prlModel) + 0xAC));
    if (m9DD || m9E1) {
        return TRUE;
    }
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove(model_data) */
    setLightTevColorType(dKy_getEnvlight(), prlModel, &tevStr);
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, prl_data, (f32)(u8)m868);
    gabi::at<mDoExt_McaMorf>(m7E8)->entryDL();
    gabi::store<u32>(gabi::ea(prl_data) + 0x44, 0); /* mBtkAnm.remove(prl_data) */
    if (m9E3) {
        setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(m86C), &tevStr);
        mDoExt_modelEntryDL(gabi::at<J3DModel>(m86C));
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mDebugDraw != 0) {
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x10019370), 4); /* 028FEAC0 memcpy */
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF8), gabi::at<u8>(0x10019374), 4);
        }
        if (m7E8 != 0) {
            if (gabi::load<u32>(0x101FDA48) == 0) {
                gabi::store<u32>(0x101FDA48, 1);
                memcpy_g(gabi::at<u8>(0x101FEBEC), gabi::at<u8>(0x10019378), 4);
            }
        }
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x8B /* DSNAP_TYPE_NPC_CO1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02229738, &daNpc_Co1_c::_draw);

/* 022298F8 */
static BOOL daNpc_Co1_Draw(daNpc_Co1_c* i_this) {
    WWHD_FUNC(0x022298F8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022298F8, daNpc_Co1_Draw);

/* 022298FC */
static BOOL daNpc_Co1_IsDelete(daNpc_Co1_c*) {
    WWHD_FUNC(0x022298FC, BOOL, (daNpc_Co1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022298FC, daNpc_Co1_IsDelete);

/* 02229904 */
void daNpc_Co1_c::set_target(int i_type) {
    WWHD_FUNC(0x02229904, void, this, i_type);
    switch ((u32)i_type) {
    case 0:
        mA33 = 0;
        break;
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        gabi::Local<cXyz> vec;
        vec->copy(*eye);
        f32 dist = fopAcM_searchPlayerDistance(this);
        s16 angle = fopAcM_searchPlayerAngleY(this) - 0x2000;
        mDoMtx_stack_c::transS(current.pos.x, vec->y, current.pos.z);
        mDoMtx_stack_c::YrotM(angle);
        vec->z = dist;
        vec->x = 0.0f;
        vec->y = 0.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), vec, &m98C);
        mA33 = 2;
        break;
    }
    }
}
VERIFY(0x02229904, &daNpc_Co1_c::set_target);

/* 02229A38 */
void daNpc_Co1_c::setAnm_ATR(int i_param_1) {
    WWHD_FUNC(0x02229A38, void, this, i_param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BDDA4);
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[mA2A].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[mA2A]);
}
VERIFY(0x02229A38, &daNpc_Co1_c::setAnm_ATR);

/* 02229AA8 */
void daNpc_Co1_c::chg_anmAtr(u8 i_anmAtr) {
    WWHD_FUNC(0x02229AA8, void, this, i_anmAtr);
    if (i_anmAtr >= 0xE || i_anmAtr == mA2A) {
        return;
    }
    switch (i_anmAtr) {
    case 3:
    case 4:
    case 5:
    case 7:
    case 0xB:
        mA33 = 0;
        break;
    case 0xD:
        mA33 = 1;
        set_target(1);
        break;
    default:
        mA33 = 1;
        break;
    }
    mA2A = i_anmAtr;
    setAnm_ATR(1);
}
VERIFY(0x02229AA8, &daNpc_Co1_c::chg_anmAtr);

/* 02229B74 */
void daNpc_Co1_c::control_anmAtr() {
    WWHD_FUNC(0x02229B74, void, this);
    switch (mA2A) {
    case 2:
    case 8:
        if (m9D8 != 0) {
            setAnm_NUM(7, 1);
            mA2A = 6;
        }
        break;
    case 3:
        if (m9D8 != 0) {
            setAnm_NUM(4, 1);
            mA2A = 0xA;
        }
        break;
    case 7:
        if (m9D8 != 0) {
            setAnm_NUM(6, 1);
            mA2A = 5;
        }
        break;
    case 9:
        if (m9D8 != 0) {
            setAnm_NUM(1, 1);
            mA2A = 1;
        }
        break;
    }
}
VERIFY(0x02229B74, &daNpc_Co1_c::control_anmAtr);

/* 02229C90 */
void daNpc_Co1_c::anmAtr(u16 i_param_1) {
    WWHD_FUNC(0x02229C90, void, this, i_param_1);
    switch (i_param_1) {
    case 6: {
        if (mA37 == 0) {
            mA2A = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mA37 = (s8)((u8)mA37 + 1);
        }
        u8 mesgAnimeTagInfo = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);                /* dComIfGp_clearMesgAnimeTagInfo() */
        if (mesgAnimeTagInfo != 0xFF && mA2B != mesgAnimeTagInfo) {
            mA2B = mesgAnimeTagInfo;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        mA37 = 0;
        break;
    }
    /* control_anmTag(): empty */
    control_anmAtr();
}
VERIFY(0x02229C90, &daNpc_Co1_c::anmAtr);

/* 02229D58 */
bool daNpc_Co1_c::chk_talk() {
    WWHD_FUNC(0x02229D58, bool, this);
    bool ret = true;
    m9DB = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            m9DB = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}
VERIFY(0x02229D58, &daNpc_Co1_c::chk_talk);

/* 02229DD8 */
u8 daNpc_Co1_c::chk_partsNotMove() {
    WWHD_FUNC(0x02229DD8, u8, this);
    return m9BC == m_jnt.mAngles[0][1] && m9BE == m_jnt.mAngles[1][1] && m9C0 == current.angle.y;
}
VERIFY(0x02229DD8, &daNpc_Co1_c::chk_partsNotMove);

/* 02229E18 */
u16 daNpc_Co1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02229E18, u16, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x183E: *pMsgNo = 0x183F; break;
    case 0x183F: *pMsgNo = 0x1840; break;
    case 0x1841: *pMsgNo = 0x1842; break;
    case 0x1842: *pMsgNo = 0x1843; break;
    case 0x1843: *pMsgNo = 0x1844; break;
    case 0x1844: *pMsgNo = 0x1845; break;
    case 0x1845: *pMsgNo = 0x1846; break;
    case 0x1846: *pMsgNo = 0x1847; break;
    case 0x1847: *pMsgNo = 0x1848; break;
    case 0x1849: *pMsgNo = 0x184A; break;
    case 0x184A: *pMsgNo = 0x184B; break;
    default:
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}
VERIFY(0x02229E18, &daNpc_Co1_c::next_msgStatus);

/* 02229F00 */
u32 daNpc_Co1_c::getMsg_CO1_0() {
    WWHD_FUNC(0x02229F00, u32, this);
    if (m9DB == 0x98) {
        return 0x183C;
    }
    if (m9DB != 0xFF) {
        return 0x183B;
    }
    if (m9DA != 0) {
        return 0x183E;
    }
    if (dNpc_chkLetterPassed()) {
        if (dComIfGs_isEventBit(0x0F04)) {
            return 0x1849;
        }
        return 0x1841;
    }
    u32 msgNo = 0x1839;
    if (dComIfGs_isEventBit(0x0F08)) {
        msgNo = 0x183A;
    }
    return msgNo;
}
VERIFY(0x02229F00, &daNpc_Co1_c::getMsg_CO1_0);

/* 02229FEC */
u32 daNpc_Co1_c::getMsg() {
    WWHD_FUNC(0x02229FEC, u32, this);
    u32 msgNo = 0;
    if (mA35 == 0) {
        msgNo = getMsg_CO1_0();
    }
    return msgNo;
}
VERIFY(0x02229FEC, &daNpc_Co1_c::getMsg);

/* 0222A024 */
u8 daNpc_Co1_c::chkAttention() {
    WWHD_FUNC(0x0222A024, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x0222A024, &daNpc_Co1_c::chkAttention);

/* 0222A0AC */
BOOL daNpc_Co1_c::wait_1() {
    WWHD_FUNC(0x0222A0AC, BOOL, this);
    if (mA30 == 1 || mA30 >= 3) {
        return TRUE;
    }
    if (m9ED) {
        if (chk_talk()) {
            mA32 = 4;
            setStt(3);
        }
        return TRUE;
    }
    mA30 = 2;
    return TRUE;
}
VERIFY(0x0222A0AC, &daNpc_Co1_c::wait_1);

/* 0222A130 */
BOOL daNpc_Co1_c::wait_2() {
    WWHD_FUNC(0x0222A130, BOOL, this);
    if (m9ED) {
        if (chk_talk()) {
            s8 next = 4;
            if (dNpc_chkLetterPassed()) {
                next = 7;
            }
            mA32 = next;
            setStt(2);
        }
        return TRUE;
    }
    mA30 = 2;
    if (m9EC) {
        m9CC = 60;
    }
    mA33 = cLib_calcTimer(&m9CC) != 0;
    return TRUE;
}
VERIFY(0x0222A130, &daNpc_Co1_c::wait_2);

/* 0222A1DC */
BOOL daNpc_Co1_c::wakeup() {
    WWHD_FUNC(0x0222A1DC, BOOL, this);
    if (m9D8 != 0) {
        mA32 = 4;
        setStt(2);
    }
    return TRUE;
}
VERIFY(0x0222A1DC, &daNpc_Co1_c::wakeup);

/* 0222A218 */
BOOL daNpc_Co1_c::talk_1() {
    WWHD_FUNC(0x0222A218, BOOL, this);
    u8 ret = chk_partsNotMove();
    if (mA30 == 1 || mA30 >= 3) {
        return TRUE;
    }
    if (dNpc_chkLetterPassed() && m9DA == 0 && !dComIfGs_isEventBit(0x0F04)) {
        if (mA30 != 3) {
            m9ED = 0;
            m9DB = 0xFF;
            endEvent();
            mA30 = 3;
        }
        return TRUE;
    }
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        u32 status = fopMsgM_getStatus();
        if (status == 2 || status == 6) {
            if (mCurrMsgNo == 0x184B) {
                mA33 = 0;
            }
        } else if (status == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0x1839:
                dComIfGs_onEventBit(0x0F08);
                break;
            case 0x1840:
                m9DA = 0;
                break;
            case 0x1848:
                dComIfGs_onEventBit(0x0F04);
                break;
            }
            if (m9DB == 0x98) {
                mA30 = 5;
                dComIfGp_evmng_CancelPresent();
                dComIfGs_setReserveItemEmpty();
                endEvent();
                m9DA = 1;
            } else {
                setStt(mA32);
                if (mA32 != 7) {
                    m9ED = 0;
                    endEvent();
                }
            }
            m9DB = 0xFF;
            m9CC = 60;
        }
    }
    return ret;
}
VERIFY(0x0222A218, &daNpc_Co1_c::talk_1);

/* 0222A450 */
BOOL daNpc_Co1_c::toru_1() {
    WWHD_FUNC(0x0222A450, BOOL, this);
    if (m9ED) {
        if (m9DB == 0x98) {
            m9DB = 0xFF;
            setAnm_NUM(8, 1);
        }
        if (m9D8 != 0) {
            setStt(6);
        }
    }
    return FALSE;
}
VERIFY(0x0222A450, &daNpc_Co1_c::toru_1);

/* 0222A4C4 */
BOOL daNpc_Co1_c::read_1() {
    WWHD_FUNC(0x0222A4C4, BOOL, this);
    if (m9D8 != 0) {
        mA32 = 7;
        setStt(2);
    }
    return FALSE;
}
VERIFY(0x0222A4C4, &daNpc_Co1_c::read_1);

/* 0222A500 */
BOOL daNpc_Co1_c::modoru() {
    WWHD_FUNC(0x0222A500, BOOL, this);
    if (morfFrameInt(mpMorf) == 0) {
        setStt(4);
        m9ED = 0;
        endEvent();
    }
    return TRUE;
}
VERIFY(0x0222A500, &daNpc_Co1_c::modoru);

/* 0222A568 */
BOOL daNpc_Co1_c::wait_action1(void*) {
    WWHD_FUNC(0x0222A568, BOOL, this, (void*)nullptr);
    LIGHT_INFLUENCE* light = (LIGHT_INFLUENCE*)(void*)&m9F0;
    if (mA36 == 0) {
        if (dNpc_chkLetterPassed()) {
            setStt(4);
        } else {
            if (!dComIfGs_isEventBit(0x1810)) {
                mA30 = 4;
            }
            setStt(1);
        }
        light->mPower = 0.0f;
        light->mPos.copy(current.pos);
        dKy_plight_set(light);
        mA36 = (s8)((u8)mA36 + 1);
        return TRUE;
    }
    if ((u32)(s32)mA36 > 3) {
        return TRUE;
    }
    m9EC = chkAttention();
    switch ((u32)(s32)mA31) {
    case 1:
        m9E8 = wait_1();
        break;
    case 2:
        m9E8 = talk_1();
        break;
    case 3:
        m9E8 = wakeup();
        break;
    case 4:
        m9E8 = wait_2();
        break;
    case 5:
        m9E8 = toru_1();
        break;
    case 6:
        m9E8 = read_1();
        break;
    case 7:
        m9E8 = modoru();
        break;
    }
    lookBack();
    return TRUE;
}
VERIFY(0x0222A568, &daNpc_Co1_c::wait_action1);

/* 0222A73C daNpc_Co1_HIO_c::daNpc_Co1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Co1_HIO_c* daNpc_Co1_HIO_c_ct(daNpc_Co1_HIO_c* i_this) {
    WWHD_FUNC(0x0222A73C, daNpc_Co1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Co1_HIO_c*)operator_new(0x38);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x100193B8;
    /* memcpy(&mPrmTbl, &a_prm_tbl (.data 0x101BDE84), 0x2C) */
    memcpy_g(&i_this->mMaxHeadX, gabi::at<u8>(0x101BDE84), 0x2C); /* 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x0222A73C, daNpc_Co1_HIO_c_ct);

/* 0222A7A8: static initialisation of the translation unit */
static void __sinit_d_a_npc_co1_cpp() {
    WWHD_FUNC(0x0222A7A8, void, (u32)0);
    /* header statics (as sinit_header_statics, the zeroed object at 0x10466CE8) */
    const u32 P = 0x10466CA4, D = 0x101BDEB0;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10466CE8 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
    daNpc_Co1_HIO_c_ct(&l_HIO()); /* static daNpc_Co1_HIO_c l_HIO */
}
VERIFY(0x0222A7A8, __sinit_d_a_npc_co1_cpp);

/* 0222A848: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0222A848, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0222A848, SafeString_dt);

/* 0222A85C: daNpc_Co1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Co1_c_dt(daNpc_Co1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0222A85C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10019398);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x100193A8);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0222A85C, daNpc_Co1_c_dt);

/* 0222A8F8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0222A8F8, void, (SafeString*)nullptr);
}
VERIFY(0x0222A8F8, SafeString_assureTerminationImpl);
