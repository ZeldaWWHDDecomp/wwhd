/**
 * d_a_npc_kamome.cpp (WWHD)
 * Player - Hyoi Seagull (0225D0D4..02260D60, without execute: d_a_npc_kamome_exec.cpp)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_kamome.h"

/* 0225D0D4 */
BOOL daNpc_kam_c::callDemoStartCheck() {
    WWHD_FUNC(0x0225D0D4, BOOL, this);
    if (gabi::load<s16>(KAM_L_DEMO_CHK_CNT) != 0) {
        return gabi::load<s16>(KAM_L_DEMO_CHK_FLAG);
    }
    gabi::store<s16>(KAM_L_DEMO_CHK_CNT, (s16)(gabi::load<s16>(KAM_L_DEMO_CHK_CNT) + 1));
    gabi::store<s16>(KAM_L_DEMO_CHK_FLAG, 0);

    u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> hyoi_pear_pos;
    hyoi_pear_pos->x = gabi::load<f32>(link + 0x3D8); /* getHeadTopPos() */
    hyoi_pear_pos->y = gabi::load<f32>(link + 0x3DC);
    hyoi_pear_pos->z = gabi::load<f32>(link + 0x3E0);
    hyoi_pear_pos->y = hyoi_pear_pos->y + 20.0f;

    gabi::Local<cXyz> descend_start_pos;
    gabi::Local<cXyz> line_start_pos;
    gabi::Local<cXyz> line_end_pos;

    s16 angleY = (s16)(gabi::load<s16>(link + 0x32A) + 0x8000); /* shape_angle.y */
    s16 angleX = -0x1555;
    gabi::store<u8>(gabi::ea(mLinChk) + 0x54, 1); /* mLinChk.OnBackFlag() */

    line_end_pos->x = hyoi_pear_pos->x;
    line_end_pos->y = hyoi_pear_pos->y;
    line_end_pos->z = hyoi_pear_pos->z;

    cXyz* kyori = gabi::at<cXyz>(KAM_L_CALL_LOCAL_KYORI);
    for (int i = 0; i < 0x10; i++) {
        mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)(angleY + 0x8000));
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, line_end_pos);
        mDoMtx_YrotS(mDoMtx_stack_c::get(), angleY);
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, line_start_pos);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), angleX);
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, descend_start_pos);

        PSVECAdd(line_end_pos, hyoi_pear_pos, line_end_pos);
        PSVECAdd(line_start_pos, hyoi_pear_pos, line_start_pos);
        PSVECAdd(descend_start_pos, line_start_pos, descend_start_pos);

        dBgS_LinChk_Set(mLinChk, descend_start_pos, line_start_pos, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            dBgS_LinChk_Set(mLinChk, line_start_pos, line_end_pos, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
                /* found a direction with no collisions */
                kam_copyWords(gabi::ea(&mDescendStartPos), descend_start_pos.a, 3);
                kam_copyWords(gabi::ea(&mDescendStartPosUnangled), line_start_pos.a, 3); /* unused */
                mDescendStartAngle.x = (s16)-angleX;
                mDescendStartAngle.y = (s16)(angleY + 0x8000);
                mDescendStartAngle.z = 0;
                gabi::store<u8>(gabi::ea(mLinChk) + 0x54, 0); /* mLinChk.OffBackFlag() */
                gabi::store<s16>(KAM_L_DEMO_CHK_FLAG, 1);
                return TRUE;
            }
        }
        angleY = (s16)(angleY + 0x2000);
    }
    gabi::store<u8>(gabi::ea(mLinChk) + 0x54, 0); /* mLinChk.OffBackFlag() */
    return FALSE;
}
VERIFY(0x0225D0D4, &daNpc_kam_c::callDemoStartCheck);

/* 0225D2FC */
s16 daNpc_kam_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x0225D2FC, s16, this, i_itemBtn);
    if (gabi::load<u8>(dComIfGp_ea() + i_itemBtn + 0x5BBB) == 0x83 /* dItemNo_HYOI_PEAR_e */) {
        return (s16)callDemoStartCheck();
    }
    return FALSE;
}
VERIFY(0x0225D2FC, &daNpc_kam_c::XyCheckCB);

/* 0225D354 daNpc_kam_XyCheckCB (not named by the matcher) */
static s16 daNpc_kam_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0225D354, s16, i_this, i_itemBtn);
    return static_cast<daNpc_kam_c*>(i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x0225D354, daNpc_kam_XyCheckCB);

/* 0225D358 */
s16 daNpc_kam_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x0225D358, s16, this, i_itemBtn);
    onEventAccept();
    mCurrEventIdxIdx = 1; /* "kamome_call" */
    return mEventIdxs[1];
}
VERIFY(0x0225D358, &daNpc_kam_c::XyEventCB);

/* 0225D378 daNpc_kam_XyEventCB (not named by the matcher) */
static s16 daNpc_kam_XyEventCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0225D378, s16, i_this, i_itemBtn);
    return static_cast<daNpc_kam_c*>(i_this)->XyEventCB(i_itemBtn);
}
VERIFY(0x0225D378, daNpc_kam_XyEventCB);

/* 0225D37C */
static BOOL headNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0225D37C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        u32 i_this = gabi::load<u32>(gabi::ea(model) + 0xB8);               /* getUserArea() */
        if (i_this != 0) {
            /* static cXyz l_offsetAttPos(0, 20, 0) */
            if (gabi::load<u32>(KAM_GUARD_OFFSET_ATT_POS) == 0) {
                gabi::store<u32>(KAM_GUARD_OFFSET_ATT_POS, 1);
                gabi::store<f32>(KAM_L_OFFSET_ATT_POS + 4, 20.0f);
                gabi::store<f32>(KAM_L_OFFSET_ATT_POS + 0, 0.0f);
                gabi::store<f32>(KAM_L_OFFSET_ATT_POS + 8, 0.0f);
            }
            u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
            PSMTXCopy(kam_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(KAM_L_OFFSET_ATT_POS), gabi::at<cXyz>(i_this + 0x3D8));
        }
    }
    return TRUE;
}
VERIFY(0x0225D37C, headNodeCallBack);

/* 0225D44C */
BOOL daNpc_kam_c::createHeap() {
    WWHD_FUNC(0x0225D44C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001B5C0) /* "Kamome" */, 0x18, KAM_SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001B5D8), 0x36F, STR(0x1001B5EC) /* "modelData != 0" */);
    }
    void* anm = dComIfG_getObjectRes(STR(0x1001B5C0), 0x12, KAM_SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, (J3DAnmTransform*)anm, 2 /* EMode_LOOP */, 1.0f,
                                    0, -1, 1, nullptr, 0x00080000, 0x11000002);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s16 head1JntIdx = (s16)JUTNameTab_getIndex_kam(kam_getJointName(modelData), 0x1001B600 /* "j_ka_head1" */);
    if (head1JntIdx >= 0) {
        kam_setJointCallBack(modelData, (u16)head1JntIdx, 0x0225D37C /* headNodeCallBack */);
    }
    m_jnt_body = (s16)JUTNameTab_getIndex_kam(kam_getJointName(modelData), 0x1001B60C /* "j_ka_spin1" */);
    if (m_jnt_body < 0) {
        JUT_ASSERT_fail(STR(0x1001B5D8), 0x38B, STR(0x1001B5C8) /* "m_jnt_body >= 0" */);
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    return TRUE;
}
VERIFY(0x0225D44C, &daNpc_kam_c::createHeap);

/* 0225D614 */
static BOOL checkCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0225D614, BOOL, i_this);
    return static_cast<daNpc_kam_c*>(i_this)->createHeap();
}
VERIFY(0x0225D614, checkCreateHeap);

/* 0225D618 HD: nothing is drawn while the seagull waits hidden; no blob shadow */
BOOL daNpc_kam_c::draw() {
    WWHD_FUNC(0x0225D618, BOOL, this);
    if (mHidden == 0) {
        J3DModel* model = mpMorf->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        gabi::call(0x02445A00, this); /* daPy_npc_c::drawDamageFog */
        setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
        mpMorf->entryDL();
        dSnap_RegistFig(0x55 /* DSNAP_TYPE_KAMOME */, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}
VERIFY(0x0225D618, &daNpc_kam_c::draw);

/* 0225D6B0 */
static BOOL daNpc_kam_Draw(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x0225D6B0, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x0225D6B0, daNpc_kam_Draw);

/* 0225D6B4 (not named by the matcher) */
BOOL daNpc_kam_c::checkCommandTalk() {
    WWHD_FUNC(0x0225D6B4, BOOL, this);
    if (kam_eventCommand(this) == 1) { /* eventInfo.checkCommandTalk() */
        if ((u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
            if (mEventState == 6) {
                mEventState = -1;
            }
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0225D6B4, &daNpc_kam_c::checkCommandTalk);

/* 0225D738 */
void daNpc_kam_c::returnLinkPlayer() {
    WWHD_FUNC(0x0225D738, void, this);
    daPy_py_changePlayer(this, dComIfGp_getLinkPlayer());
    offHyoiKamome();
    kam_setAudioFlag(0); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 0 */
}
VERIFY(0x0225D738, &daNpc_kam_c::returnLinkPlayer);

/* 0225D78C */
void daNpc_kam_c::eventEnd() {
    WWHD_FUNC(0x0225D78C, void, this);
    dComIfGp_event_reset();
    offEventAccept();
    if (mCurrEventIdxIdx == 0) { /* "OPTION_CHAR_END" */
        returnLinkPlayer();
        offReturnLink();
    }
    mCurrEventIdxIdx = -1;
}
VERIFY(0x0225D78C, &daNpc_kam_c::eventEnd);

/* 0225D7F8 */
BOOL daNpc_kam_c::eventProc() {
    WWHD_FUNC(0x0225D7F8, BOOL, this);
    if (kam_eventCommand(this) == 2 /* checkCommandDemoAccrpt() */ && mEventState != -1) {
        if (mEventState == 0) {
            if (dEvmng_startCheckOld(STR(0x1001B620) /* "OPTION_CHAR_END" */) || dEvmng_endCheckOld(STR(0x1001B620))) {
                /* dComIfGp_event_setTalkPartner(dComIfGp_getLinkPlayer()) */
                u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
                u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
                gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, link));
                gabi::call(0x025E1988, 0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
            } else {
                offReturnLink();
                mEventState = -1;
            }
        }
        if (mEventState != -1) {
            onEventAccept();
            mEventState = -1;
        }
    }

    const char* staffName = gabi::at<const char>(gabi::load<u32>(KAM_L_STAFF_NAME));
    s32 staffId = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */ && !checkCommandTalk()) {
        if (staffId != -1) {
            s32 actIdx = gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, KAM_CUT_NAME_TBL, 4, 1, 0);
            dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
            if (actIdx == -1) {
                gabi::call(0x02543280, evmng, staffId); /* cutEnd */
            } else {
                if (gabi::call<BOOL>(0x025447C8, evmng, staffId)) { /* getIsAddvance */
                    md_pmf_call<void>(this, gabi::at<ProcFunc_l>(KAM_EVENT_INIT_TBL + actIdx * 8), staffId);
                }
                if (md_pmf_call<BOOL>(this, gabi::at<ProcFunc_l>(KAM_EVENT_ACTION_TBL + actIdx * 8), staffId)) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
            }
        }
        if (isEventAccept()) {
            s16 idx = mEventIdxs[mCurrEventIdxIdx];
            if (dComIfGp_evmng_endCheck(idx)) {
                eventEnd();
            }
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0225D7F8, &daNpc_kam_c::eventProc);

/* 0225DAC8 HD: the line angle starts from current.angle; when the front line hits and BG checks are
 * on, a short debug-register line forward stops the horizontal movement ("KAMOME FRONT HIT") */
void daNpc_kam_c::setLineBgCheck() {
    WWHD_FUNC(0x0225DAC8, void, this);
    s16 ax = current.angle.x;
    s16 az = current.angle.z;
    s16 ay = current.angle.y;
    offLineHit();
    gabi::Local<cXyz> lineEndPos;
    cXyz* localEnd = gabi::at<cXyz>(KAM_L_LINE_BG_LOCAL_END);

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, ay, az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(mLinChk, &current.pos, lineEndPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
        mHitFlags = mHitFlags | 1; /* onFrontLineHit() */
        if (!isNoBgCheck()) {
            gabi::Local<cXyz> vec;
            vec->z = 20.0f;
            vec->x = 0.0f;
            vec->y = REG_F(10, 3) + 20.0f;
            mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, ay, az);
            PSMTXMultVec(mDoMtx_stack_c::get(), vec, lineEndPos);
            PSVECAdd(lineEndPos, &current.pos, lineEndPos);
            dBgS_LinChk_Set(mLinChk, &current.pos, lineEndPos, this);
            if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
                gabi::call(0x027EC9E8, 0x1E, 0x190, STR(0x1001B630) /* "KAMOME FRONT HIT " */); /* JUTReport */
                current.pos.x = old.pos.x;
                current.pos.z = old.pos.z;
            }
        }
    }

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, (s16)(shape_angle.y + 0x2000), az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(mLinChk, &current.pos, lineEndPos, this);
    BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), mLinChk);
    s16 sy = shape_angle.y;
    if (hit) {
        mHitFlags = mHitFlags | 2; /* onLeftLineHit() */
    }

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, (s16)(sy - 0x2000), az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(mLinChk, &current.pos, lineEndPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
        mHitFlags = mHitFlags | 4; /* onRightLineHit() */
    }
}
VERIFY(0x0225DAC8, &daNpc_kam_c::setLineBgCheck);

/* 0225DD20 */
void daNpc_kam_c::animationPlay() {
    WWHD_FUNC(0x0225DD20, void, this);
    u32 mtrlSndId = 0;
    u32 gnd = gabi::ea(this) + 0x6FC; /* mAcch.m_gnd (its cBgS_PolyInfo) */
    if (mAcch.ChkGroundHit() && gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), gnd) /* ChkPolySafe */) {
        mtrlSndId = gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), gnd); /* GetMtrlSndId */
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mReachedAnimEnd = (s8)gabi::call<BOOL>(0x025E535C, mpMorf.get(), &eyePos, mtrlSndId, reverb);

    mDoExt_McaMorf* morf = mpMorf;
    f32 rate = morf->mFrameCtrl.mRate;
    f32 frame = morf->mFrameCtrl.mFrame;
    f32 prev = mPrevMorfFrame;
    if (rate < 0.0f) {
        if (frame > prev) {
            mReachedAnimEnd = 1;
        }
    } else if (frame < prev) {
        mReachedAnimEnd = 1;
    }
    mPrevMorfFrame = frame;
}
VERIFY(0x0225DD20, &daNpc_kam_c::animationPlay);

/* 0225DDF0 setAction (not named by the matcher): GHS pointers to member compare the index first */
BOOL daNpc_kam_c::setAction(ProcFunc_l* cur, ProcFunc_l* newFunc, void* arg) {
    WWHD_FUNC(0x0225DDF0, BOOL, this, cur, newFunc, arg);
    s16 newI = newFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0) {
            return TRUE;
        }
        newD = newFunc->d;
        newF = newFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF)) {
            return TRUE;
        }
    } else {
        newF = newFunc->f;
        newD = newFunc->d;
        if (cur->i == 0) {
            goto set;
        }
    }
    mActionStatus = ACTION_ENDING;
    md_pmf_call<BOOL>(this, cur, arg);
set:
    cur->i = newI;
    cur->f = newF;
    cur->d = newD;
    mUnusedC14 = 0.0f;
    mUnusedC0E = 0;
    mC0A = 0;
    mC0C = 0;
    mC08 = 0;
    mActionStatus = ACTION_STARTING;
    md_pmf_call<BOOL>(this, cur, arg);
    return TRUE;
}
VERIFY(0x0225DDF0, &daNpc_kam_c::setAction);

/* 0225DF38 setPlayerAction (not named by the matcher) */
void daNpc_kam_c::setPlayerAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x0225DF38, void, this, actionFunc, arg);
    mCurrNpcActionFunc.d = 0;
    mCurrNpcActionFunc.i = 0;
    mCurrNpcActionFunc.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mCurrPlayerActionFunc, fn, arg);
}
VERIFY(0x0225DF38, &daNpc_kam_c::setPlayerAction);

/* 0225DF88 */
void daNpc_kam_c::playerAction(void* arg) {
    WWHD_FUNC(0x0225DF88, void, this, arg);
    if (mCurrPlayerActionFunc.i == 0) {
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, KAM_PMF_waitPlayerAction);
        setPlayerAction(fn, nullptr);
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BB8, 7);    /* dComIfGp_setRStatusForce(dActStts_RETURN_e) */
    gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0x23); /* dComIfGp_setDoStatus(dActStts_FLY_e) */
    gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 0x3E); /* dComIfGp_setAStatus(dActStts_HIDDEN_e) */
    md_pmf_call<BOOL>(this, &mCurrPlayerActionFunc, arg);
}
VERIFY(0x0225DF88, &daNpc_kam_c::playerAction);

/* 0225E064 returnLinkCheck (not named by the matcher). HD: one pad check (out of line), the
 * GameCube R-or-START test */
BOOL daNpc_kam_c::returnLinkCheck() {
    WWHD_FUNC(0x0225E064, BOOL, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */) {
        if (gabi::call<BOOL>(0x02007840, 0)) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0225E064, &daNpc_kam_c::returnLinkCheck);

/* 0225E0B8 setNpcAction (not named by the matcher) */
void daNpc_kam_c::setNpcAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x0225E0B8, void, this, actionFunc, arg);
    mCurrPlayerActionFunc.d = 0;
    mCurrPlayerActionFunc.i = 0;
    mCurrPlayerActionFunc.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mCurrNpcActionFunc, fn, arg);
}
VERIFY(0x0225E0B8, &daNpc_kam_c::setNpcAction);

/* 0225E108 npcAction (not named by the matcher) */
void daNpc_kam_c::npcAction(void* arg) {
    WWHD_FUNC(0x0225E108, void, this, arg);
    if (mCurrNpcActionFunc.i == 0) {
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, KAM_PMF_waitNpcAction);
        offHyoiKamome();
        setNpcAction(fn, nullptr);
        kam_setAudioFlag(0); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 0 */
    }
    md_pmf_call<BOOL>(this, &mCurrNpcActionFunc, arg);
}
VERIFY(0x0225E108, &daNpc_kam_c::npcAction);

/* 0225E1E0 */
void daNpc_kam_c::eventOrder() {
    WWHD_FUNC(0x0225E1E0, void, this);
    if (isEventAccept()) {
        return;
    }
    if (mEventState == 5 || mEventState == 4) {
        if (dComIfGp_getPlayer(0) != this) {
            eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
            if (mEventState == 5) {
                fopAcM_orderSpeakEvent(this);
            }
        } else {
            mEventState = -1;
        }
    } else if (mEventState == 6) {
        if (dComIfGp_getPlayer(0) != this) {
            eventInfo_onCondition(this, 0x21 /* dEvtCnd_CANTALKITEM_e | dEvtCnd_CANTALK_e */);
        } else {
            mEventState = -1;
        }
    } else if (mEventState != -1 && mEventState < 3) {
        s8 st = mEventState;
        mCurrEventIdxIdx = st;
        fopAcM_orderOtherEventId(this, mEventIdxs[st], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x0225E1E0, &daNpc_kam_c::eventOrder);

/* 0225E304 HD: debug register REG0_S(0) moves the model offset after the rotation */
void daNpc_kam_c::setBaseMtx() {
    WWHD_FUNC(0x0225E304, void, this);
    /* static cXyz l_offset(0, 30, 0) */
    if (gabi::load<u32>(KAM_GUARD_OFFSET) == 0) {
        gabi::store<u32>(KAM_GUARD_OFFSET, 1);
        gabi::store<f32>(KAM_L_OFFSET + 4, 30.0f);
        gabi::store<f32>(KAM_L_OFFSET + 0, 0.0f);
        gabi::store<f32>(KAM_L_OFFSET + 8, 0.0f);
    }
    cXyz* l_offset = gabi::at<cXyz>(KAM_L_OFFSET);
    J3DModel* model = mpMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    if (REG0_S(0) == 0) {
        mDoMtx_stack_transM(l_offset->x, l_offset->y, l_offset->z);
    }
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    if (REG0_S(0) != 0) {
        mDoMtx_stack_transM(l_offset->x, l_offset->y, l_offset->z);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    mpMorf->calc();
}
VERIFY(0x0225E304, &daNpc_kam_c::setBaseMtx);

/* 0225E444 HD: one sphere; it is registered with the mass collision manager (GameCube: the
 * separate tg sphere) */
void daNpc_kam_c::setCollision() {
    WWHD_FUNC(0x0225E444, void, this);
    J3DModel* model = mpMorf->getModel();
    Mtx34* mtx = kam_getAnmMtx(model, (u32)(s32)m_jnt_body);
    gabi::Local<cXyz> center;
    center->x = mtx->m[0][3];
    center->y = mtx->m[1][3];
    center->z = mtx->m[2][3];

    mAtSph.SetC(center);
    dComIfG_Ccsp_Set(&mAtSph);
    gabi::call(0x02516C14, dComIfGp_ea() + PLAY_CCMASS, &mAtSph, 3); /* dCcMassS_Mng::Set */

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    gabi::Local<cXyz> at_global_start;
    gabi::Local<cXyz> at_global_end;
    gabi::Local<cXyz> at_global_vec;
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(KAM_L_MS_AT_LOCAL_START), at_global_start);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(KAM_L_MS_AT_LOCAL_END), at_global_end);
    PSMTXMultVecSR(mDoMtx_stack_c::get(), gabi::at<cXyz>(KAM_L_MS_AT_LOCAL_VEC), at_global_vec);
    PSVECAdd(at_global_start, center, at_global_start);
    PSVECAdd(at_global_end, center, at_global_end);

    u32 cps = gabi::ea(&mCps);
    gabi::call(0x02018808, cps + 0x118, at_global_start.get(), at_global_end.get()); /* cM3dGCps::SetStartEnd */
    kam_copyWords(cps + 0x7C, at_global_vec.a, 3); /* SetAtVec */
    dComIfG_Ccsp_Set(&mCps);
}
VERIFY(0x0225E444, &daNpc_kam_c::setCollision);

/* 0225EE60 */
static BOOL daNpc_kam_Execute(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x0225EE60, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x0225EE60, daNpc_kam_Execute);

/* 0225EE64 */
static BOOL daNpc_kam_IsDelete(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x0225EE64, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0225EE64, daNpc_kam_IsDelete);

/* 0225EE6C HD: the destructor runs through the vtable (framework) */
static BOOL daNpc_kam_Delete(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x0225EE6C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0225EE6C, daNpc_kam_Delete);

/* 0225EE74 HD: mMinY is 0 (GameCube: home.y - range); one wall circle sized by debug registers;
 * roof correction height 40 (GameCube 20); no tg sphere */
BOOL daNpc_kam_c::init() {
    WWHD_FUNC(0x0225EE74, BOOL, this);
    offHyoiKamome();
    mCurrEventIdxIdx = -1;
    mUnusedC06 = 0;
    mEventState = -1;
    PSVECScale(&scale, &scale, 1000.0f);
    mMinY = 0.0f;
    mMaxY = home.pos.y + scale.y;

    daNpc_kam_HIO1_l& hio = kam_l_HIO().mHio1;
    mTargetSpeedF = hio.mSpeedF;
    mAngVelY = hio.mGlidingAngVelY;
    mAngVelX = hio.mGlidingAngVelX;
    mTargetAngVelY = hio.mGlidingAngVelY;
    mTargetAngVelX = hio.mGlidingAngVelX;

    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, KAM_PMF_waitNpcAction);
    setNpcAction(fn, nullptr);

    mAcchCir.SetWall(REG_F(10, 4) + 20.0f, REG_F(10, 5) + 50.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.m_flags = (mAcch.m_flags & ~8u) | 0x62000; /* ClrRoofNone, OnLineCheck, OnSeaCheckOn, OnSeaWaterHeight */
    mAcch.SetRoofCrrHeight(40.0f);
    mStts.Init(100, 0xFF, this);
    mAtSph.Set(gabi::at<dCcD_SrcSph>(KAM_L_SPH_SRC));
    mAtSph.SetStts(&mStts);
    gabi::call(0x025164C0, &mCps, KAM_L_KAM_AT_CPS_SRC); /* dCcD_Cps::Set */
    mCps.SetStts(&mStts);

    setBaseMtx();

    u32 t = gabi::ea(this);
    gabi::store<u32>(t + 0x39C, 0); /* attention_info.flags */
    gabi::store<u8>(t + 0x389, 38); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(t + 0x38B, 38); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */

    for (int i = 0; i < 3; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(KAM_EVENT_NAME_TBL + i * 4));
        mEventIdxs[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }

    gabi::store<u32>(t + 0x104, 0x0225D354); /* eventInfo.setXyCheckCB(daNpc_kam_XyCheckCB) */
    gabi::store<u32>(t + 0x100, 0x0225D378); /* eventInfo.setXyEventCB(daNpc_kam_XyEventCB) */
    return TRUE;
}
VERIFY(0x0225EE74, &daNpc_kam_c::init);

/* 0225F064 */
cPhs_State daNpc_kam_c::create() {
    WWHD_FUNC(0x0225F064, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_kam_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (this != nullptr) {
            u32 t = gabi::ea(this);
            gabi::call(0x024450B4, this); /* daPy_npc_c::daPy_npc_c */
            __vtbl = KAM_VTBL;
            dBgS_ObjAcch_ct(&mAcch, dBgS_ObjAcch_vt{0x1001B4EC, 0x1001B50C, 0x1001B4FC});
            dBgS_AcchCir_ct(&mAcchCir);
            dBgS_LinChk_ct(mLinChk, dBgS_LinChk_vt{0x1001B51C, 0x1001B52C, 0x1001B54C, 0x1001B53C}, false);
            dCcD_Stts_ct(&mStts);
            gabi::call(0x025166F0, &mAtSph); /* dCcD_Sph::dCcD_Sph */
            /* dCcD_Cps mCps */
            gabi::call(0x02515FB8, &mCps); /* dCcD_GObjInf::dCcD_GObjInf */
            gabi::store<u32>(t + 0xB00, 0x100015A8);
            gabi::store<u32>(t + 0xAFC, 0x1001B46C);
            gabi::call(0x02018150, t + 0xB04); /* cM3dGCps::cM3dGCps */
            gabi::store<u32>(t + 0xB1C, 0x1004AF60);
            gabi::store<u32>(t + 0xB00, 0x1004AF70);
            gabi::store<u32>(t + 0xA28, 0x1004AF18);
            /* cBgS_PolyInfo mPolyInfo */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = 0x1001B47C;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    u32 act = gabi::load<u32>(KAM_L_ACT);
    if (act != 0 && act != gabi::ea(this)) {
        return cPhs_ERROR_e;
    }
    gabi::store<u32>(KAM_L_ACT, gabi::ea(this));

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, STR(0x1001B680) /* "Kamome" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0225D614 /* checkCreateHeap */, 0x1360)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        daNpc_kam_HIO_l& hio = kam_l_HIO();
        if (hio.mNo < 0) {
            s8 no = mDoHIO_createChild(STR(0x1001B688) /* "kamome" */, &hio);
            hio.mpActor = gabi::ea(this);
            hio.mNo = no;
        }
        if (!init()) {
            phase_state = cPhs_ERROR_e;
        }
    }
    return phase_state;
}
VERIFY(0x0225F064, &daNpc_kam_c::create);

/* 0225F310 */
static cPhs_State daNpc_kam_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0225F310, cPhs_State, i_this);
    return static_cast<daNpc_kam_c*>(i_this)->create();
}
VERIFY(0x0225F310, daNpc_kam_Create);

/* 0225F314 daNpc_kam_c::~daNpc_kam_c (deleting destructor, vtable slot; not named by the matcher) */
static void daNpc_kam_c_dtor(daNpc_kam_c* i_this, s32 flags) {
    WWHD_FUNC(0x0225F314, void, i_this, flags);
    if (i_this == nullptr) {
        return;
    }
    u32 t = gabi::ea(i_this);
    i_this->__vtbl = KAM_VTBL;
    if (gabi::load<u32>(KAM_L_ACT) == t) {
        gabi::store<u32>(KAM_L_ACT, 0);
    }
    dComIfG_resDelete(&i_this->mPhs, STR(0x1001B690) /* "Kamome" */);
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    daNpc_kam_HIO_l& hio = kam_l_HIO();
    if (hio.mNo >= 0) {
        mDoHIO_deleteChild(hio.mNo);
        hio.mNo = -1;
    }
    daNpc_kam_c::offHyoiKamome();
    /* member destructors */
    gabi::call(0x02515980, &i_this->mCps, 2);   /* dCcD_GObjInf::~dCcD_GObjInf */
    gabi::call(0x02515AE8, &i_this->mAtSph, 2); /* dCcD_Sph::~dCcD_Sph */
    gabi::call(0x02515860, &i_this->mStts, 2);  /* dCcD_Stts::~dCcD_Stts */
    gabi::store<u32>(t + 0x870, 0x1001B54C);    /* dBgS_LinChk vtables */
    gabi::store<u32>(t + 0x87C, 0x1001B49C);
    gabi::store<u32>(t + 0x838, 0x1001B48C);
    gabi::call(0x02008B4C, i_this->mLinChk, 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::call(0x02018034, t + 0x7EC, 2);       /* mAcchCir: cM3dGCir::~cM3dGCir */
    gabi::store<u32>(t + 0x634, 0x1001B4FC);    /* dBgS_ObjAcch vtables */
    gabi::store<u32>(t + 0x628, 0x1001B50C);
    gabi::call(0x024EFD9C, &i_this->mAcch, 0);  /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x0244513C, i_this, 0);          /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}
VERIFY(0x0225F314, daNpc_kam_c_dtor);

/* 0225F468 */
BOOL daNpc_kam_c::actionDefault(int evtStaffId) {
    WWHD_FUNC(0x0225F468, BOOL, this, evtStaffId);
    return TRUE;
}
VERIFY(0x0225F468, &daNpc_kam_c::actionDefault);

/* 0225F470 */
void daNpc_kam_c::initialWaitEvent(int evtStaffId) {
    WWHD_FUNC(0x0225F470, void, this, evtStaffId);
    u32 timerP = gabi::ea(dComIfGp_evmng_getMySubstanceP(evtStaffId, STR(0x1001B698) /* "timer" */, 3));
    s16 timer = 0;
    if (timerP != 0) {
        timer = gabi::load<s16>(timerP + 2); /* (s16)*timerP */
    }
    mWaitTimer = timer;
}
VERIFY(0x0225F470, &daNpc_kam_c::initialWaitEvent);

/* 0225F4D4 HD: the int parameter is gone; with `false` the attention point is placed ahead of the
 * player (debug-register offsets), with `true` at the seagull */
void daNpc_kam_c::setAttention(bool param_1) {
    WWHD_FUNC(0x0225F4D4, void, this, param_1);
    if (param_1) {
        f32 hy = mHeadTopPos.y;
        f32 hz = mHeadTopPos.z;
        f32 py = current.pos.y;
        f32 px = current.pos.x;
        eyePos.z = hz;
        eyePos.y = hy - 20.0f;
        eyePos.x = mHeadTopPos.x;
        u32 t = gabi::ea(this);
        gabi::store<f32>(t + 0x394, py); /* attention_info.position */
        gabi::store<f32>(t + 0x390, px);
        gabi::store<f32>(t + 0x398, current.pos.z);
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        gabi::Local<cXyz> local;
        gabi::Local<cXyz> pos;
        local->y = REG_F(10, 10) + 100.0f;
        local->x = 0.0f;
        local->z = REG_F(10, 11) + 1000.0f;
        MtxPosition(local, pos);
        PSVECAdd(pos, &player->current.pos, pos);
        u32 t = gabi::ea(this);
        f32 x = pos->x, y = pos->y, z = pos->z;
        eyePos.z = z;
        eyePos.x = x;
        eyePos.y = y;
        gabi::store<f32>(t + 0x390, x);
        gabi::store<f32>(t + 0x394, y);
        gabi::store<f32>(t + 0x398, z);
    }
}
VERIFY(0x0225F4D4, &daNpc_kam_c::setAttention);

/* 0225F5E0 */
BOOL daNpc_kam_c::actionWaitEvent(int evtStaffId) {
    WWHD_FUNC(0x0225F5E0, BOOL, this, evtStaffId);
    setAttention(true);
    return kam_calcTimer(&mWaitTimer) == 0 ? TRUE : FALSE;
}
VERIFY(0x0225F5E0, &daNpc_kam_c::actionWaitEvent);

/* 0225F620 */
void daNpc_kam_c::initialChangeEvent(int evtStaffId) {
    WWHD_FUNC(0x0225F620, void, this, evtStaffId);
    current.roomNo = home.roomNo; /* fopAcM_SetRoomNo(this, fopAcM_GetHomeRoomNo(this)) */
    daPy_py_changePlayer(this, this);
    gabi::call(0x025B6994, gabi::load<u32>(0x101F84DC) + 0x96); /* dComIfGs_setBaitItemEmpty() */
    mHitFlags = mHitFlags & ~0x10u; /* offNoBgCheck() */
}
VERIFY(0x0225F620, &daNpc_kam_c::initialChangeEvent);

/* 0225F674 HD: the event ends at once when the descent would start more than 500 above Link;
 * the seagull is shown again (mHidden); descent timer 150 frames (GameCube 900) */
void daNpc_kam_c::initialDescendEvent(int evtStaffId) {
    WWHD_FUNC(0x0225F674, void, this, evtStaffId);
    u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
    f32 startY = mDescendStartPos.y;
    if (startY - gabi::load<f32>(link + 0x318) > 500.0f) {
        eventEnd();
        return;
    }
    current.pos.y = startY;
    current.pos.x = mDescendStartPos.x;
    current.pos.z = mDescendStartPos.z;
    s16 ax = mDescendStartAngle.x, ay = mDescendStartAngle.y, az = mDescendStartAngle.z;
    current.angle.x = ax;
    current.angle.y = ay;
    current.angle.z = az;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;

    daNpc_kam_HIO1_l& hio = kam_l_HIO().mHio1;
    f32 speed = hio.mSpeedF;
    mTargetSpeedF = speed;
    speedF = speed;
    mAngVelY = hio.mGlidingAngVelY;
    mAngVelX = hio.mGlidingAngVelX;
    mTargetAngVelY = hio.mGlidingAngVelY;
    mTargetAngVelX = hio.mGlidingAngVelX;

    mDoAud_seStart(0x8EF /* JA_SE_HYOI_USE_DEMO */, nullptr, 0, dComIfGp_getReverb(current.roomNo));
    kam_setAudioFlag(1); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 1 */
    mHidden = 0;
    mHitFlags = mHitFlags | 0x10; /* onNoBgCheck() */
    mWaitTimer = 150;
}
VERIFY(0x0225F674, &daNpc_kam_c::initialDescendEvent);

/* 0225F794 */
BOOL daNpc_kam_c::actionDescendEvent(int evtStaffId) {
    WWHD_FUNC(0x0225F794, BOOL, this, evtStaffId);
    u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> linkHeadTopPos;
    linkHeadTopPos->x = gabi::load<f32>(link + 0x3D8);
    linkHeadTopPos->y = gabi::load<f32>(link + 0x3DC);
    linkHeadTopPos->z = gabi::load<f32>(link + 0x3E0);
    linkHeadTopPos->y = linkHeadTopPos->y + 20.0f;

    setAttention(true);

    f32 hy = linkHeadTopPos->y;
    f32 y = current.pos.y;
    if (y < hy) {
        y = hy;
        current.pos.y = y;
    }
    if (y < hy + 50.0f) {
        cLib_addCalcAngleS(&current.angle.x, 0, 30, 0x2000, 0x400);
        shape_angle.x = current.angle.x;
    }

    gabi::Local<cXyz> delta;
    cXyz_mi(linkHeadTopPos, delta, &current.pos);
    gabi::Local<cXyz> xz; /* abs2XZ() */
    xz->y = 0.0f;
    xz->x = delta->x;
    xz->z = delta->z;
    if (PSVECSquareMag(xz) < 10000.0f /* SQUARE(100.0f) */) {
        return TRUE;
    }
    if (kam_calcTimer(&mWaitTimer) == 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0225F794, &daNpc_kam_c::actionDescendEvent);

/* 0225F8B4 */
void daNpc_kam_c::initialAreaOutTurn(int evtStaffId) {
    WWHD_FUNC(0x0225F8B4, void, this, evtStaffId);
    mWaitTimer = 10;
    mAreaOutTimer = 180 * 30;
    current.angle.x = 0;
    current.angle.z = 0;
}
VERIFY(0x0225F8B4, &daNpc_kam_c::initialAreaOutTurn);

/* 0225F8D4 HD: the range is widened by a debug register */
BOOL daNpc_kam_c::areaOutCheck() {
    WWHD_FUNC(0x0225F8D4, BOOL, this);
    f32 dist = kam_absXZ_delta(&current.pos, &home.pos);
    return dist > scale.x + REG_F(10, 0) ? TRUE : FALSE;
}
VERIFY(0x0225F8D4, &daNpc_kam_c::areaOutCheck);

/* 0225F950 HD: no onNoBgCheck/offNoBgCheck around the turn */
BOOL daNpc_kam_c::actionAreaOutTurn(int evtStaffId) {
    WWHD_FUNC(0x0225F950, BOOL, this, evtStaffId);
    if (kam_calcTimer(&mWaitTimer) == 0) {
        speedF = 0.0f;
        s16 targetAngle = cLib_targetAngleY(&current.pos, &home.pos);
        s16 angleDiff = (s16)(current.angle.y - targetAngle);
        if (angleDiff != 0) {
            cLib_addCalcAngleS(&current.angle.y, targetAngle, 16, 0x2000, 0x400);
        } else {
            speedF = kam_l_HIO().mHio1.mSpeedF;
        }
        shape_angle.y = current.angle.y;
        current.angle.z = angleDiff;
        s32 maxZ = kam_l_HIO().mHio1.mMaxAngleZ;
        if (angleDiff > maxZ) {
            current.angle.z = (s16)maxZ;
        } else if (angleDiff < -maxZ) {
            current.angle.z = (s16)-maxZ;
        }
        if (!areaOutCheck()) {
            return TRUE;
        }
    }
    cLib_addCalcAngleS(&shape_angle.x, current.angle.x, 8, 0x2000, 0x400);
    cLib_addCalcAngleS(&shape_angle.z, current.angle.z, 8, 0x2000, 0x400);
    setAttention(true);
    return kam_calcTimer(&mAreaOutTimer) == 0 ? TRUE : FALSE;
}
VERIFY(0x0225F950, &daNpc_kam_c::actionAreaOutTurn);

/* 0225FABC */
BOOL daNpc_kam_c::changeAreaCheck() {
    WWHD_FUNC(0x0225FABC, BOOL, this);
    /* Can Link take control of a Hyoi Seagull from where he stands? */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    BOOL isForestHaven = FALSE;
    if (kam_isStartStage(0x1001B6B8 /* "sea" */) && current.roomNo == 41) {
        isForestHaven = TRUE;
    }
    if (isForestHaven) {
        /* Forest Haven sector: Link must stand on the rock with the Nintendo Gallery entrance */
        if (gabi::load<u32>(KAM_GUARD_CENTER) == 0) {
            gabi::store<u32>(KAM_GUARD_CENTER, 1);
            gabi::store<f32>(KAM_L_CENTER + 8, 191870.0f);
            gabi::store<f32>(KAM_L_CENTER + 0, 211555.0f);
            gabi::store<f32>(KAM_L_CENTER + 4, 1000.0f);
        }
        cXyz* l_center = gabi::at<cXyz>(KAM_L_CENTER);
        if (player->current.pos.y > l_center->y) {
            f32 dist = kam_absXZ_delta(&player->current.pos, l_center);
            if (dist < 1300.0f) {
                goto ok;
            }
        }
    } else {
        f32 dist = kam_absXZ_delta(&player->current.pos, &home.pos);
        if (dist < scale.x) {
            goto ok;
        }
    }
    mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
    return FALSE;
ok:
    mNoResetFlg1 = mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
    return TRUE;
}
VERIFY(0x0225FABC, &daNpc_kam_c::changeAreaCheck);

/* 0225FD2C wallHitCheck (not named by the matcher). HD: one wall circle */
int daNpc_kam_c::wallHitCheck() {
    WWHD_FUNC(0x0225FD2C, int, this);
    if (mAcch.ChkWallHit() && mAcchCir.ChkWallHit()) {
        return 0;
    }
    return -1;
}
VERIFY(0x0225FD2C, &daNpc_kam_c::wallHitCheck);

/* 0225FD54 */
BOOL daNpc_kam_c::getStickAngY(be<s16>* pTargetAngleY, be<s16>* pTargetAngleZ) {
    WWHD_FUNC(0x0225FD54, BOOL, this, pTargetAngleY, pTargetAngleZ);
    BOOL isTurning = FALSE;
    s16 deltaAngleY = 0;
    s16 targetAngleZ = 0;
    if (CPad_GET_STICK_POS_X_kam() != 0.0f) {
        f32 sx = CPad_GET_STICK_POS_X_kam();
        deltaAngleY = (s16)gabi::ftoi(sx * (f32)(-(s32)mAngVelY));
        sx = CPad_GET_STICK_POS_X_kam();
        targetAngleZ = (s16)gabi::ftoi(sx * (f32)(s32)kam_l_HIO().mHio1.mMaxAngleZ);
        isTurning = TRUE;
    } else if (wallHitCheck() >= 0) {
        if (isLeftLineHit()) {
            deltaAngleY = -0x4000;
            targetAngleZ = kam_l_HIO().mHio1.mMaxAngleZ;
            isTurning = TRUE;
        } else if (isRightLineHit()) {
            deltaAngleY = 0x4000;
            targetAngleZ = (s16)-kam_l_HIO().mHio1.mMaxAngleZ;
            isTurning = TRUE;
        } else if (isFrontLineHit()) {
            deltaAngleY = -0x4000;
            targetAngleZ = kam_l_HIO().mHio1.mMaxAngleZ;
            isTurning = TRUE;
        }
    }
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    s16 angleY = gabi::call<s16>(0x024F8018, camera);     /* dCam_getControledAngleY */
    *pTargetAngleY = (s16)(deltaAngleY + angleY);
    *pTargetAngleZ = targetAngleZ;
    return isTurning;
}
VERIFY(0x0225FD54, &daNpc_kam_c::getStickAngY);

/* 0225FEA4 getAngleX (not named by the matcher). HD: on the sea in room 44, close to one spot, the
 * seagull is turned down (as above its maximum height) */
s16 daNpc_kam_c::getAngleX() {
    WWHD_FUNC(0x0225FEA4, s16, this);
    s16 angle;
    BOOL special = FALSE;
    if (current.pos.y < mMinY || mAcch.ChkGroundHit() || isWaterHit()) {
        /* turn up */
        angle = (s16)-mAngVelX;
        mLockAngleXTimer = 30;
    } else if (current.pos.y > mMaxY || mAcch.ChkRoofHit()) {
        /* turn down */
        angle = mAngVelX;
        mLockAngleXTimer = 30;
    } else {
        f32 sy = CPad_GET_STICK_POS_Y_kam();
        angle = (s16)gabi::ftoi(sy * (f32)(s32)mAngVelX);
    }
    if (kam_isStartStage(0x1001B6D0 /* "sea" */) && current.roomNo == 44) {
        special = TRUE;
    }
    if (special) {
        f32 dy = (2100.0f - current.pos.y) + REG_F(18, 7);
        f32 dx = -195260.0f - current.pos.x;
        f32 dz = 313671.0f - current.pos.z;
        if (dy < 50.0f && dy > -50.0f) {
            if (gabi::fmadds(dx, dx, dz * dz) < REG_F(18, 8) + 250000.0f) {
                angle = mAngVelX;
                mLockAngleXTimer = 30;
            }
        }
    }
    return angle;
}
VERIFY(0x0225FEA4, &daNpc_kam_c::getAngleX);

/* 022600FC HD: debug register REG0_S(0) selects fixed pitch speeds */
BOOL daNpc_kam_c::keyProc() {
    WWHD_FUNC(0x022600FC, BOOL, this);
    daNpc_kam_HIO1_l& hio = kam_l_HIO().mHio1;
    if (kam_calcTimer(&mFlapExhaustedTimer) == 0) {
        if (gabi::call<BOOL>(0x02007898, 0) /* CPad_CHECK_TRIG_A(0) */) {
            mFlapTimer = hio.mFlapDuration;
        }
        if (kam_calcTimer(&mFlapTimer) != 0) {
            /* flapping */
            mTargetSpeedF = hio.mFlappingSpeedF;
            mTargetAngVelY = hio.mFlappingAngVelY;
            mTargetAngVelX = REG0_S(0) != 0 ? (s16)0x1555 : (s16)hio.mFlappingAngVelX;
            if (kam_calcTimer(&mFlapEnergyTimer) == 0) {
                /* flapping for too long without rest: exhausted */
                mFlapExhaustedTimer = hio.mFlapExhaustedDuration;
            }
        } else {
            /* gliding */
            mTargetSpeedF = hio.mSpeedF;
            mTargetAngVelY = hio.mGlidingAngVelY;
            mTargetAngVelX = REG0_S(0) != 0 ? (s16)0xAAA : (s16)hio.mGlidingAngVelX;
            /* build up energy to flap while gliding */
            mFlapEnergyTimer = (s16)(mFlapEnergyTimer + 1);
            if (mFlapEnergyTimer > hio.mFlapEnergyDuration) {
                mFlapEnergyTimer = hio.mFlapEnergyDuration;
            }
        }
    } else {
        mFlapTimer = 0;
        mFlapEnergyTimer = hio.mFlapEnergyDuration;
    }
    return TRUE;
}
VERIFY(0x022600FC, &daNpc_kam_c::keyProc);

/* 02260220 HD: the seagull waits hidden at its home position (no flight, no animation) until
 * Link calls it */
BOOL daNpc_kam_c::waitNpcAction(void*) {
    WWHD_FUNC(0x02260220, BOOL, this, (void*)nullptr);
    kam_copyWords(gabi::ea(&current), gabi::ea(&home), 5); /* current = home */
    mHidden = 1;
    u32 t = gabi::ea(this);
    if (changeAreaCheck()) {
        gabi::store<u32>(t + 0x39C, gabi::load<u32>(t + 0x39C) | 0x02000008); /* ACTION_SPEAK | TALKFLAG_NOTALK */
        mEventState = 6;
    } else {
        gabi::store<u32>(t + 0x39C, gabi::load<u32>(t + 0x39C) & ~0x02000008u);
        mEventState = -1;
    }
    setAttention(false);
    return TRUE;
}
VERIFY(0x02260220, &daNpc_kam_c::waitNpcAction);

/* 022602DC HD: debug register REG0_S(0) forces morf 8; the wait animation starts at a random
 * frame with a random speed (0.7..1.0) */
void daNpc_kam_c::setAnm(int anmIdx) {
    WWHD_FUNC(0x022602DC, void, this, anmIdx);
    mAnmIdx = anmIdx;
    u32 prm = KAM_L_ANM_PRM + anmIdx * 0x14; /* anmPrm_c {s8 tbl; int loop; f32 morf; f32 speed; int m10} */
    s8 tblIdx = gabi::load<s8>(prm);
    f32 playSpeed = gabi::load<f32>(prm + 0xC);
    if ((s32)tblIdx == (s32)mAnmTblIdx && playSpeed == mpMorf->mFrameCtrl.mRate) {
        return;
    }
    mAnmTblIdx = tblIdx;
    u32 tbl = KAM_L_ANM_TBL + tblIdx * 2; /* anmTbl_c {s8 bck; s8 bas} */
    mPrevMorfFrame = 0.0f;
    mReachedAnimEnd = 0;
    f32 morf = REG0_S(0) == 0 ? gabi::load<f32>(prm + 8) : 8.0f;
    gabi::call(0x0259D454 /* dNpc_setAnm */, mpMorf.get(), gabi::load<s32>(prm + 4), morf, playSpeed,
               (s32)gabi::load<s8>(tbl), (s32)gabi::load<s8>(tbl + 1), STR(0x1001B6E0) /* "Kamome" */);
    if (gabi::load<s32>(prm + 0x10) < 0) {
        mDoExt_McaMorf* m = mpMorf;
        m->setFrame(m->getEndFrame());
    }
    if (anmIdx == 0 && REG0_S(0) == 0) {
        mDoExt_McaMorf* m = mpMorf;
        s32 rnd = gabi::call<s32>(0x021E1E78, 0, gabi::ftoi(m->getEndFrame())); /* cLib_getRndValue<int> */
        m->setFrame((f32)rnd);
        mDoExt_McaMorf* m2 = mpMorf;
        m2->mFrameCtrl.mRate = cM_rndF(0.3f) + 0.7f;
    }
}
VERIFY(0x022602DC, &daNpc_kam_c::setAnm);

/* 02260510 HD: with REG0_S(0) == 0 (the release value) the steering turns shape_angle quickly and
 * lets current.angle follow (GameCube: current.angle turns and shape_angle copies it) */
BOOL daNpc_kam_c::waitPlayerAction(void*) {
    WWHD_FUNC(0x02260510, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = 1; /* ACTION_ONGOING_1 */
        onHyoiKamome();
        setAnm(ANM_WAIT1);
        daNpc_kam_HIO1_l& hio = kam_l_HIO().mHio1;
        speedF = hio.mSpeedF;
        mTargetSpeedF = hio.mSpeedF;
        mAngVelY = hio.mGlidingAngVelY;
        mAngVelX = hio.mGlidingAngVelX;
        mTargetAngVelY = hio.mGlidingAngVelY;
        mTargetAngVelX = hio.mGlidingAngVelX;
        mC08 = (s16)kam_getRndValue(60, 90);
        mFlapTimer = 0;
        mC0A = 0;
        mFlapExhaustedTimer = 0;
        mFlapEnergyTimer = hio.mFlapEnergyDuration;
        return TRUE;
    }
    if (mActionStatus == ACTION_ENDING) {
        offHyoiKamome();
        return TRUE;
    }

    keyProc();
    kam_calcTimer(&mLockAngleXTimer);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */ && areaOutCheck()) {
        mEventState = 2;
    }

    gabi::Local<be<s16>> targetAngleZ;
    gabi::Local<be<s16>> targetAngleY;
    *targetAngleZ = 0;
    *targetAngleY = 0;
    if (getStickAngY(targetAngleY, targetAngleZ)) {
        if (REG0_S(0) == 0) {
            cLib_addCalcAngleS2(&shape_angle.y, *targetAngleY, 8, 0x3000);
        } else {
            cLib_addCalcAngleS(&current.angle.y, *targetAngleY, 8, 0x2000, 0x400);
        }
    }
    if (REG0_S(0) == 0) {
        cLib_addCalcAngleS2(&current.angle.y, shape_angle.y, 8, 0x400);
    } else {
        shape_angle.y = current.angle.y;
    }

    if (mLockAngleXTimer == 0) {
        mTargetAngleX = getAngleX();
    }

    if (REG0_S(0) == 0) {
        cLib_addCalcAngleS2(&shape_angle.x, mTargetAngleX, 8, 0x3000);
        cLib_addCalcAngleS2(&shape_angle.z, *targetAngleZ, 8, 0x3000);
    } else {
        cLib_addCalcAngleS(&current.angle.x, mTargetAngleX, 8, 0x2000, 0x400);
        cLib_addCalcAngleS(&current.angle.z, *targetAngleZ, 8, 0x2000, 0x400);
    }
    setAttention(true);

    if (mAnmIdx != ANM_WAIT2) {
        if (mFlapTimer != 0) {
            setAnm(ANM_WAIT2);
        } else if (kam_calcTimer(&mC08) == 0) {
            mC08 = (s16)kam_getRndValue(90, 90);
            setAnm(ANM_SING);
        }
        if (mAnmIdx == ANM_SING) {
            if (mReachedAnimEnd) {
                setAnm(ANM_WAIT1);
            }
            if (mAnmIdx == ANM_SING && mpMorf->checkFrame(8.0f)) {
                mDoAud_seStart(0x4805 /* JA_SE_CV_KAMOME */, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
            }
        }
    } else if (mReachedAnimEnd && mFlapTimer == 0) {
        setAnm(ANM_SING);
    }

    if (mAtSph.ChkTgHit()) { /* HD: the hit check uses the at sphere */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, KAM_PMF_damagePlayerAction);
        setPlayerAction(fn, nullptr);
    }
    return TRUE;
}
VERIFY(0x02260510, &daNpc_kam_c::waitPlayerAction);

/* 022608A4 */
BOOL daNpc_kam_c::damagePlayerAction(void*) {
    WWHD_FUNC(0x022608A4, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x21, dir);
        mDamageFogTimer = 150; /* setDamageFogTimer(150) */
        mActionStatus = (s8)(mActionStatus + 1); /* ACTION_ONGOING_1 */
        onHyoiKamome();
        setAnm(ANM_WAIT1);
        speedF = 0.0f;
        mDoAud_seStart(0x4805 /* JA_SE_CV_KAMOME */, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        mC08 = 30;
    } else if (mActionStatus == ACTION_ENDING) {
        offHyoiKamome();
    } else {
        s16 y = (s16)(shape_angle.y + 0x2000);
        current.angle.y = y;
        shape_angle.y = y;
        setAttention(true);
        if (kam_calcTimer(&mC08) == 0) {
            returnLink();
        }
    }
    return TRUE;
}
VERIFY(0x022608A4, &daNpc_kam_c::damagePlayerAction);

/* 022609E0 HD: steeper pitch speeds (gliding 0x1555, flapping 0x2000; GameCube 0xAAA / 0x1555) */
static daNpc_kam_HIO1_l* daNpc_kam_HIO1_c_ct(daNpc_kam_HIO1_l* i_this) {
    WWHD_FUNC(0x022609E0, daNpc_kam_HIO1_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_kam_HIO1_l*)operator_new(0x2C);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->mSpeedF = 17.0f;
    i_this->mUnused08 = 0.1f;
    i_this->mGlidingAngVelY = 0x2000;
    i_this->mGlidingAngVelX = 0x1555;
    i_this->mMaxAngleZ = 0x2000;
    i_this->mAccelF = 0.7f;
    i_this->mFlappingSpeedF = 23.0f;
    i_this->mFlappingAngVelY = 0x2710;
    i_this->mFlappingAngVelX = 0x2000;
    i_this->mAngVelStepScale = 5;
    i_this->mAngVelMaxStep = 0x1000;
    i_this->mAngVelMinStep = 0x0400;
    i_this->mFlapDuration = 30;
    i_this->mFlapExhaustedDuration = 150;
    i_this->mFlapEnergyDuration = 150;
    i_this->__vtbl = 0x1001B55C;
    return i_this;
}
VERIFY(0x022609E0, daNpc_kam_HIO1_c_ct);

/* 02260A9C daNpc_kam_HIO_c::daNpc_kam_HIO_c (not named by the matcher) */
static daNpc_kam_HIO_l* daNpc_kam_HIO_c_ct(daNpc_kam_HIO_l* i_this) {
    WWHD_FUNC(0x02260A9C, daNpc_kam_HIO_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_kam_HIO_l*)operator_new(0x50);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->__vtbl = 0x1001B56C;
    daNpc_kam_HIO1_c_ct(&i_this->mHio1);
    i_this->mNo = -1;
    /* static const hio_prm_c init_data (3000, 1000, 250, 0, 0, 0x4000, 0): prm = init_data */
    kam_copyWords(gabi::ea(i_this) + 4, 0x1001B6F8, 6);
    return i_this;
}
VERIFY(0x02260A9C, daNpc_kam_HIO_c_ct);

/* 02260B2C __sinit_d_a_npc_kamome_cpp (new: compiler-generated) */
static void __sinit_d_a_npc_kamome_cpp() {
    WWHD_FUNC(0x02260B2C, void);
    sinit_header_statics(0x104673C8, 0x101BEDA0);
    daNpc_kam_HIO_c_ct(&kam_l_HIO());
    kam_setVec(KAM_L_LINE_BG_LOCAL_END, 0.0f, 0.0f, 500.0f);
    kam_setVec(KAM_L_MS_AT_LOCAL_END, -100.0f, 20.0f, 0.0f);
    kam_setVec(KAM_L_MS_AT_LOCAL_START, 100.0f, 20.0f, 0.0f);
    gabi::store<f32>(0x101BED58, 20.0f); /* HD: l_kam_at_cps_src radius set at run time */
    kam_setVec(KAM_L_CALL_LOCAL_KYORI, 0.0f, 0.0f, 500.0f);
    kam_setVec(KAM_L_MS_AT_LOCAL_VEC, 0.0f, 0.0f, -1.0f);
}
VERIFY(0x02260B2C, __sinit_d_a_npc_kamome_cpp);

/* 02260C54 sead::SafeString deleting destructor (this TU's copy; new: compiler-generated) */
static void kam_SafeString_dtor(void* i_this, s32 flags) {
    WWHD_FUNC(0x02260C54, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x02260C54, kam_SafeString_dtor);

/* ---- this TU's copies of daPy_py_c inline virtuals (vtable 0x1001B720; new: compiler-generated) ---- */
static s32 kam_inline_02260C68(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C68, s32, i_this);
    return -1;
}
VERIFY(0x02260C68, kam_inline_02260C68);
static s32 kam_inline_02260C70(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C70, s32, i_this);
    return 0;
}
VERIFY(0x02260C70, kam_inline_02260C70);
static s32 kam_inline_02260C78(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C78, s32, i_this);
    return 0;
}
VERIFY(0x02260C78, kam_inline_02260C78);
static s32 kam_inline_02260C80(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C80, s32, i_this);
    return 0;
}
VERIFY(0x02260C80, kam_inline_02260C80);
static s32 kam_inline_02260C88(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C88, s32, i_this);
    return 0;
}
VERIFY(0x02260C88, kam_inline_02260C88);
static s32 kam_inline_02260C90(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C90, s32, i_this);
    return 0;
}
VERIFY(0x02260C90, kam_inline_02260C90);
static s32 kam_inline_02260C98(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260C98, s32, i_this);
    return 0;
}
VERIFY(0x02260C98, kam_inline_02260C98);
static s32 kam_inline_02260CA0(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CA0, s32, i_this);
    return 0;
}
VERIFY(0x02260CA0, kam_inline_02260CA0);
static s32 kam_inline_02260CA8(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CA8, s32, i_this);
    return 0;
}
VERIFY(0x02260CA8, kam_inline_02260CA8);
static s32 kam_inline_02260CB0(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CB0, s32, i_this);
    return 0;
}
VERIFY(0x02260CB0, kam_inline_02260CB0);
static void kam_inline_02260CB8(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CB8, void, i_this);
}
VERIFY(0x02260CB8, kam_inline_02260CB8);
static s32 kam_inline_02260CBC(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CBC, s32, i_this);
    return 0;
}
VERIFY(0x02260CBC, kam_inline_02260CBC);
static s32 kam_inline_02260CC4(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CC4, s32, i_this);
    return -1;
}
VERIFY(0x02260CC4, kam_inline_02260CC4);
static s32 kam_inline_02260CCC(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CCC, s32, i_this);
    return -1;
}
VERIFY(0x02260CCC, kam_inline_02260CCC);
static s32 kam_inline_02260CD4(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CD4, s32, i_this);
    return -1;
}
VERIFY(0x02260CD4, kam_inline_02260CD4);
static s32 kam_inline_02260CDC(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CDC, s32, i_this);
    return 0;
}
VERIFY(0x02260CDC, kam_inline_02260CDC);
static s32 kam_inline_02260CE4(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CE4, s32, i_this);
    return 0;
}
VERIFY(0x02260CE4, kam_inline_02260CE4);
static s32 kam_inline_02260CEC(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CEC, s32, i_this);
    return 0;
}
VERIFY(0x02260CEC, kam_inline_02260CEC);
static s32 kam_inline_02260CF4(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CF4, s32, i_this);
    return 0;
}
VERIFY(0x02260CF4, kam_inline_02260CF4);
static void kam_inline_02260CFC(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260CFC, void, i_this);
}
VERIFY(0x02260CFC, kam_inline_02260CFC);
static void kam_inline_02260D00(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D00, void, i_this);
}
VERIFY(0x02260D00, kam_inline_02260D00);
static void kam_inline_02260D04(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D04, void, i_this);
}
VERIFY(0x02260D04, kam_inline_02260D04);
static s32 kam_inline_02260D08(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D08, s32, i_this);
    return 0;
}
VERIFY(0x02260D08, kam_inline_02260D08);
static void kam_inline_02260D10(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D10, void, i_this);
}
VERIFY(0x02260D10, kam_inline_02260D10);
static void kam_inline_02260D14(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D14, void, i_this);
}
VERIFY(0x02260D14, kam_inline_02260D14);
static void kam_inline_02260D18(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D18, void, i_this);
}
VERIFY(0x02260D18, kam_inline_02260D18);
static s32 kam_inline_02260D1C(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D1C, s32, i_this);
    return 0;
}
VERIFY(0x02260D1C, kam_inline_02260D1C);
static s32 kam_inline_02260D24(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D24, s32, i_this);
    return 1;
}
VERIFY(0x02260D24, kam_inline_02260D24);
static void kam_inline_02260D2C(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D2C, void, i_this);
}
VERIFY(0x02260D2C, kam_inline_02260D2C);
/* sead::SafeString::assureTerminationImpl_ (empty)  */
static void kam_inline_02260D60(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D60, void, i_this);
}
VERIFY(0x02260D60, kam_inline_02260D60);

/* 02260D30 getGroundY(): mAcch.GetGroundH() */
static f32 kam_getGroundY(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D30, f32, i_this);
    return i_this->mAcch.GetGroundH();
}
VERIFY(0x02260D30, kam_getGroundY);
/* 02260D38 getLeftHandMatrix(): cullMtx */
static u32 kam_getLeftHandMatrix(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D38, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x02260D38, kam_getLeftHandMatrix);
/* 02260D40 getRightHandMatrix(): cullMtx */
static u32 kam_getRightHandMatrix(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D40, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x02260D40, kam_getRightHandMatrix);
/* 02260D48 getBaseAnimeFrameRate() */
static f32 kam_getBaseAnimeFrameRate(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D48, f32, i_this);
    return 1.0f;
}
VERIFY(0x02260D48, kam_getBaseAnimeFrameRate);
/* 02260D54 getBaseAnimeFrame() */
static f32 kam_getBaseAnimeFrame(daNpc_kam_c* i_this) {
    WWHD_FUNC(0x02260D54, f32, i_this);
    return 0.0f;
}
VERIFY(0x02260D54, kam_getBaseAnimeFrame);
