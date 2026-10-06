/**
 * d_a_npc_p1.cpp (WWHD)
 * NPC - Gonzo, Senza, & Nudge (Tetra's pirates)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * _create/CreateHeap: d_a_npc_p1_create.cpp; kaji_anm, lookBack: d_a_npc_p1_kaji.cpp; actions:
 * d_a_npc_p1_action.cpp (normal/confuse/talk) and d_a_npc_p1_speak.cpp (speak/p1c_speak).
 */
#include "d/actor/d_a_npc_p1.h"

/* 022B0674 */
static BOOL nodeCallBack1(J3DNode* i_node, int i_param_2) {
    WWHD_FUNC(0x022B0674, BOOL, i_node, i_param_2);
    if (i_param_2 == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_P1_c* i_this = gabi::at<daNpc_P1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(i_node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(gabi::at<Mtx34>(p1_getAnmMtx(model, jntNo)), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> local_28;
                local_28->x = REG_F(10, 0);
                local_28->y = REG_F(10, 1);
                local_28->z = REG_F(10, 2);
                cMtx_YrotM(calc_mtx(), (s16)-(i_this->m_jnt.mAngles[0][1] + i_this->mHeadAnm.field_0x00.y));
                cMtx_ZrotM(calc_mtx(), (s16)-(i_this->m_jnt.mAngles[0][0] + i_this->mHeadAnm.field_0x00.x));
                MtxPosition(local_28, &i_this->eyePos);
            }
            if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            u32 anm = p1_getAnmMtx(model, jntNo); /* model->setAnmMtx(jntNo, *calc_mtx) */
            p1_mtx_copy(anm, gabi::ea(calc_mtx()));
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}
VERIFY(0x022B0674, nodeCallBack1);

/* 022B0810 */
BOOL daNpc_P1_c::_draw() {
    WWHD_FUNC(0x022B0810, BOOL, this);
    /* l_snap_idx_tbl (0x101C2BE4) */
    if (mType == TYPE_P1A_e && !p1_isEventBit(0x310)) {
        return TRUE;
    }
    J3DModel* pJVar7 = mpMorf->getModel();
    J3DModelData* head_model_data = J3DModel_getModelData(mpHeadModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pJVar7, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    gabi::call(0x025E7B3C, mBtp, head_model_data, (u32)(u8)mBlinkFrame); /* mBtp.entry(head_model_data, mBlinkFrame) */
    /* HD: no material table swap for P1B/P1C (separate body models, see CreateHeap) and no blob shadow */
    u8 type = mType;
    mpMorf->updateDL();
    if (type == TYPE_P1A_e) {
        J3DModel* head = mpHeadModel;
        u32 anm = p1_getAnmMtx(pJVar7, m_jnt.mHeadJntNum);
        p1_mtx_copy(gabi::ea(head) + 0xC8, anm); /* mpHeadModel->setBaseTRMtx(getAnmMtx(head)) */
        mDoExt_modelUpdateDL(mpHeadModel, 0);
    } else {
        J3DModel* head = mpHeadModel;
        u32 anm = p1_getAnmMtx(pJVar7, m_jnt.mHeadJntNum);
        p1_mtx_copy(gabi::ea(head) + 0xC8, anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
        if (mpDoraModel) {
            setLightTevColorType(dKy_getEnvlight(), mpDoraModel, &tevStr);
            s8 hand = m_handR_jnt_num;
            u32 anm2 = p1_getAnmMtx(pJVar7, hand);
            p1_mtx_copy(gabi::ea(mpDoraModel.get()) + 0xC8, anm2);
            mDoExt_modelUpdateDL(mpDoraModel, 0);
        }
    }
    u8 snap = gabi::load<u8>(0x101C2BE4 + mType);
    gabi::call(0x025BEBB8, (u32)snap, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
    return TRUE;
}
VERIFY(0x022B0810, &daNpc_P1_c::_draw);

/* 022B0ADC */
static BOOL daNpc_P1_Draw(daNpc_P1_c* i_this) {
    WWHD_FUNC(0x022B0ADC, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022B0ADC, daNpc_P1_Draw);

/* 022B1124 (GameCube CheckCreateHeap; unnamed by the matcher) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022B1124, BOOL, i_this);
    return ((daNpc_P1_c*)i_this)->CreateHeap();
}
VERIFY(0x022B1124, CheckCreateHeap);

/* 022B1128 */
void daNpc_P1_c::demo_end_init() {
    WWHD_FUNC(0x022B1128, void, this);
    if (mParam == 0) {
        mAnmNum = -1;
        /* setAction(&daNpc_P1_c::confuseAction) expanded without the comparison */
        ProcFunc_l* cur = &mActionFunc;
        if (cur->i != 0) {
            mActionStatus = ACTION_ENDING_e;
            p1_pmf_call(this, cur, nullptr);
        }
        s16 oi = cur->i;
        u32 of = cur->f;
        s16 od = cur->d;
        mPrevAction.f = of;
        cur->f = P1_confuseAction;
        cur->d = 0;
        cur->i = -1;
        mPrevAction.d = od;
        mActionStatus = ACTION_STARTING_e;
        mPrevAction.i = oi;
        gabi::call_ptr<BOOL>(P1_confuseAction, gabi::ea(this) + (s32)(s16)cur->d, (u32)0);
        /* fopAcM_SearchByName(fpcNm_Obj_Pirateship_e) */
        gabi::Local<be<s16>> name;
        *name = fpcNm_Obj_Pirateship_e;
        fopAc_ac_c* actor = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
        if (actor) {
            u32 px = gabi::load<u32>(gabi::ea(actor) + 0x314);
            f32 x = actor->current.pos.x;
            gabi::store<u32>(gabi::ea(this) + 0x314, px);
            f32 y = actor->current.pos.y;
            current.pos.y = y;
            u32 pz = gabi::load<u32>(gabi::ea(actor) + 0x31C);
            f32 z = actor->current.pos.z;
            gabi::store<u32>(gabi::ea(this) + 0x31C, pz);
            current.pos.y = y + 700.0f;
            x = gabi::fmadds(950.0f, cM_ssin((u16)actor->current.angle.y), x);
            current.pos.x = x;
            z = gabi::fmadds(950.0f, cM_scos((u16)actor->current.angle.y), z);
            current.pos.z = z;
            x = gabi::fmadds(150.0f, cM_scos((u16)actor->current.angle.y), x);
            current.pos.x = x;
            z = gabi::fnmsubs(150.0f, cM_ssin((u16)actor->current.angle.y), z);
            current.pos.z = z;
            s16 sVar2 = (s16)(actor->current.angle.y - 0x4000);
            actor->current.angle.y = sVar2;
            current.angle.y = sVar2;
        }
    }
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock */
    m670 = 0;
    m_jnt.mbHeadLock = 0; /* offHeadLock */
}
VERIFY(0x022B1128, &daNpc_P1_c::demo_end_init);

/* HD: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), "Demo17") == 0 through two sead::SafeString
 * temporaries; the first assureTermination is called directly (this TU's copy 022B5AE8) */
static inline bool p1_isStage_Demo17() {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = P1_SAFESTRING_VTBL;
    a->mStringTop = 0x1001F9D0; /* "Demo17" */
    b->mStringTop = 0x1047E6B8;
    b->__vtbl = P1_SAFESTRING_VTBL;
    gabi::call(0x022B5AE8, a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}

/* 022B12FC */
BOOL daNpc_P1_c::demo_move() {
    WWHD_FUNC(0x022B12FC, BOOL, this);
    /* dComIfGp_demo_getActor(demoActorID) */
    u8 id = demoActorID;
    u32 actor = 0;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) {
            JUT_ASSERT_fail(STR(0x1001F914), 0x23A, STR(0x1001F904));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        actor = gabi::call<u32>(0x02526E70, obj, (u32)id);
    }
    if (!actor) {
        if (m670 == 1) {
            demo_end_init();
        }
        return FALSE;
    }
    m_jnt.mbBackBoneLock = 1; /* onBackBoneLock */
    m_jnt.mbHeadLock = 1;     /* onHeadLock */
    m670 = 1;
    J3DAnmTexPattern* btp = gabi::call<J3DAnmTexPattern*>(0x02527828, actor, STR(0x1001F9CC) /* "P1" */);
    if (btp) {
        gabi::call<s32>(0x025E789C, mBtp, J3DModel_getModelData(mpHeadModel), btp, 1, 2, 1.0f, 0, -1, 1, 0);
        mBlinkFrame = 0;
    }
    u32 btpAnm = gabi::load<u32>(gabi::ea(this) + 0x3F0); /* mBtp.getBtpAnm() */
    if (btpAnm) {
        u32 vt = gabi::load<u32>(btpAnm + 4);
        u8 uVar1 = (u8)gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), btpAnm); /* getFrameMax() */
        u8 f = (u8)(mBlinkFrame + 1);
        if (f < uVar1) {
            mBlinkFrame = f;
        } else {
            mBlinkFrame = uVar1;
        }
    }
    /* HD: in the stage "Demo17", between demo frames 0x654 and 0x6B0, the demo actor's flag 0x40 is
     * cleared and the animation plays at double speed */
    if (p1_isStage_Demo17()) {
        u32 frame = gabi::load<u32>(0x101D600C);
        if (frame - 0x654 < 0x5D) {
            gabi::store<u16>(actor + 4, (u16)(gabi::load<u16>(actor + 4) & 0xFFBF));
            mpMorf->setPlaySpeed(2.0f);
        }
    }
    gabi::Local<cXyz> pos;
    p1_copy12(gabi::ea(pos.get()), gabi::ea(&current.pos));
    u32 snd = gabi::call<u32>(0x024F1914, pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(current.pos, 10.0f) */
    gabi::call<BOOL>(0x02527028, this, 0x6A, mpMorf.get(), STR(0x1001F9CC) /* "P1" */, 0, 0, snd, 0); /* dDemo_setDemoData */
    /* HD: Senza is moved 20 to the left between demo frames 0x38C and 0x406 of "Demo17" */
    if (mType == TYPE_P1B_e) {
        if (p1_isStage_Demo17()) {
            u32 frame = gabi::load<u32>(0x101D600C);
            if (frame - 0x38C < 0x7B) {
                current.pos.x = current.pos.x - 20.0f;
            }
        }
    }
    return TRUE;
}
VERIFY(0x022B12FC, &daNpc_P1_c::demo_move);

/* 022B16A4 */
BOOL daNpc_P1_c::playTexPatternAnm() {
    WWHD_FUNC(0x022B16A4, BOOL, this);
    if (!gabi::call<s16>(0x02055B64, &mBlinkTimer)) { /* cLib_calcTimer */
        u8 f = (u8)(mBlinkFrame + 1);
        mBlinkFrame = f;
        u32 tex = gabi::ea(mpTexture.get());
        s32 max = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(tex + 4) + 0x14), tex);
        if ((s32)f >= max) {
            tex = gabi::ea(mpTexture.get());
            max = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(tex + 4) + 0x14), tex);
            mBlinkFrame = (u8)(mBlinkFrame - max);
            s16 rng = (s16)gabi::ftoi(cM_rndF(100.0f));
            mBlinkTimer = (s16)(rng + 0x1E);
        }
    }
    return TRUE;
}
VERIFY(0x022B16A4, &daNpc_P1_c::playTexPatternAnm);

/* 022B1758 */
BOOL daNpc_P1_c::setAnm(int i_anm, f32 i_morf) {
    WWHD_FUNC(0x022B1758, BOOL, this, i_anm, i_morf);
    if ((u32)mAnmNum == (u32)i_anm) {
        return FALSE;
    }
    mAnmNum = i_anm;
    if (i_morf < 0.0f) {
        i_morf = p1_child(mType)->mMorfBackup;
    }
    /* dRes_INDEX_P1_BCK_* (HD indices), "P1" at 0x1001F9E0 */
    s32 idx;
    s32 attr = 2; /* J3DFrameCtrl::EMode_LOOP */
    switch ((u32)i_anm) {
    case 0: idx = 0x28; break;            /* WAIT */
    case 1: idx = 0x23; break;            /* TALK */
    case 2: idx = 0x24; break;            /* TALK02 */
    case 3: idx = 0x25; break;            /* TALK03 */
    case 9: idx = 0x29; break;            /* WAIT02 */
    case 10: idx = 0x22; attr = 0; break; /* OMOKJ */
    case 11: idx = 0x27; attr = 0; break; /* TORIKJ */
    case 12: idx = 0x1A; break;           /* ANGRY */
    case 13: idx = 0x26; break;           /* TALK04 */
    case 14: idx = 0x1F; break;           /* CHECK01 */
    case 15: idx = 0x20; break;           /* CHECK02 */
    case 16: idx = 0x21; break;           /* LOOK */
    case 4: idx = 0x1E; break;            /* C_WAIT */
    case 5: idx = 0x1C; break;            /* C_TALK01 */
    case 6: idx = 0x1D; attr = 0; break;  /* C_TALK02 */
    case 7: idx = 0x1B; break;            /* C_STOP */
    case 8: idx = 0x1C; break;            /* C_TALK01 */
    default: return TRUE;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)p1_getRes(0x1001F9E0, idx);
    mpMorf->setAnm(anm, attr, i_morf, 1.0f, 0.0f, -1.0f, nullptr);
    return TRUE;
}
VERIFY(0x022B1758, &daNpc_P1_c::setAnm);

/* 022B1B78 */
BOOL daNpc_P1_c::evn_setAnm_init(int i_staff_id) {
    WWHD_FUNC(0x022B1B78, BOOL, this, i_staff_id);
    be<s32>* idx_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staff_id, STR(0x1001F9E4) /* "idx" */, 3);
    if (idx_p != nullptr) {
        setAnm(*idx_p, -1.0f);
    } else if (mType == TYPE_P1C_e) {
        setAnm(4, -1.0f);
    } else {
        setAnm(0, -1.0f);
    }
    return TRUE;
}
VERIFY(0x022B1B78, &daNpc_P1_c::evn_setAnm_init);

/* 022B1C14 */
BOOL daNpc_P1_c::evn_talk_init(int i_staff_id) {
    WWHD_FUNC(0x022B1C14, BOOL, this, i_staff_id);
    be<u32>* mesg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staff_id, STR(0x1001F9E8) /* "MsgNo" */, 3);
    p1_msgId() = 0xFFFFFFFF;
    /* HD: no l_msg */
    if (mesg_no_p != nullptr) {
        mCurrMesg = *mesg_no_p;
    } else {
        mCurrMesg = 0;
    }
    return TRUE;
}
VERIFY(0x022B1C14, &daNpc_P1_c::evn_talk_init);

/* 022B1C84 */
BOOL daNpc_P1_c::minigameExplainCut() {
    WWHD_FUNC(0x022B1C84, BOOL, this);
    u32 mgr = p1_msgMgr(); /* HD: the message manager, read once */
    /* ActionNames[] = {"4013_msg", "4014_msg"} (0x101C2BE8) */
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(mEventCut6B0.mpEvtStaffName), nullptr, 0);
    s32 actIdx = p1_getMyActIdx(staffId, 0x101C2BE8, 2, 1, 0);
    if (staffId == -1) {
        mbAttentionFlag = 0;
        return FALSE;
    }
    if (m65A == 0) {
        p1_msgId() = 0xFFFFFFFF;
        p1_onCameraAttentionStatus4();
        mPrevMesg = mCurrMesg;
        mCurrMesg = 0xFAD;
        m65A = (s8)(m65A + 1);
        return TRUE;
    }
    if (m65A == -1) {
        return TRUE;
    }
    if (p1_msgId() == 0xFFFFFFFF) {
        p1_msgId() = p1_msgSet(mgr, mCurrMesg, &eyePos);
        return TRUE;
    }
    if (m65A == 1) {
        /* HD: no fopMsgM_SearchByID */
        m65A = (s8)(m65A + 1);
    } else if (p1_msgGetStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        if (mCurrMesg == 0xFAD) {
            p1_msgSetStatus(mgr, 0xF /* fopMsgStts_MSG_CONTINUES_e */);
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFAE;
            p1_msgSet(mgr, 0xFAE, nullptr);
        } else {
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFFFFFFFF;
            p1_msgSetStatus(mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
        }
    } else if (p1_msgGetStatus(mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (actIdx == 1) {
            dComIfGp_evmng_cutEnd(staffId);
        }
        p1_msgSetStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
        p1_offCameraAttentionStatus4();
    } else if (p1_checkMesgSendButton() && actIdx == 0) {
        dComIfGp_evmng_cutEnd(staffId);
    }
    if (p1_checkAction(this, P1_explainAction)) {
        if (gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), STR(0x1001F9F0) /* "sea_exp_cam" */)) {
            p1_setAction(this, P1_speakAction);
            p1_event_reset();
            u32 cam = p1_camera0(); /* SkipSmoother */
            gabi::store<u8>(cam + 0x348, 1);
            gabi::store<u8>(cam + 0x34A, 1);
            gabi::store<u8>(cam + 0x349, 1);
            fopAcM_orderSpeakEvent(this);
            p1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        }
    }
    return TRUE;
}
VERIFY(0x022B1C84, &daNpc_P1_c::minigameExplainCut);

/* 022B205C */
u32 daNpc_P1_c::getNextMsgNo(int i_param_1) {
    WWHD_FUNC(0x022B205C, u32, this, i_param_1);
    u32 mgr = p1_msgMgr();
    u32 o_retval;
    u32 curr = mCurrMesg;
    switch (curr) {
    case 0xC94:
        o_retval = 0xC99;
        break;
    case 0xFA1:
    case 0xFA2:
    case 0xFA5:
    case 0xFA6:
    case 0xFA7:
    case 0xFAA:
    case 0xFAB:
    case 0x1015:
    case 0x1007:
        o_retval = curr + 1;
        break;
    case 0x1009:
        o_retval = 0x102E;
        break;
    case 0x1016:
        o_retval = 0x1033;
        break;
    case 0xFAE:
        o_retval = 0xFA5;
        break;
    case 0xFA8:
    case 0xFAC:
        o_retval = 0xFA2;
        break;
    case 0xFA3: {
        s32 sel = gabi::load<s32>(mgr + 0x948); /* l_msg->mSelectNum */
        if (sel == 0) {
            o_retval = 0xFA4;
        } else if (sel == 1) {
            o_retval = 0xFB0;
        } else {
            o_retval = 0xFFFFFFFF;
        }
        break;
    }
    default:
        o_retval = 0xFFFFFFFF;
        break;
    }
    if (i_param_1 == 1) {
        mCurrMesg = o_retval;
        mPrevMesg = curr;
    }
    return o_retval;
}
VERIFY(0x022B205C, &daNpc_P1_c::getNextMsgNo);

/* 022B21B0 */
BOOL daNpc_P1_c::evn_talk() {
    WWHD_FUNC(0x022B21B0, BOOL, this);
    u32 mgr = p1_msgMgr();
    if (p1_msgId() == 0xFFFFFFFF) {
        p1_msgId() = p1_msgSet(mgr, mCurrMesg, &eyePos);
    } else if (p1_msgGetStatus(mgr) == 0xE) { /* HD: no l_msg search */
        if (getNextMsgNo(1) != 0xFFFFFFFF) {
            p1_msgSetStatus(mgr, 0xF);
            p1_msgSet(mgr, mCurrMesg, nullptr);
        } else {
            p1_msgSetStatus(mgr, 0x10);
        }
    } else if (p1_msgGetStatus(mgr) == 0x12) {
        p1_msgSetStatus(mgr, 0x13);
        p1_msgId() = 0xFFFFFFFF;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022B21B0, &daNpc_P1_c::evn_talk);

/* 022B22B8 */
void daNpc_P1_c::setAnimFromMsg() {
    WWHD_FUNC(0x022B22B8, void, this);
    if ((mAnmNum == 10 || mAnmNum == 11) && !p1_checkEnd(mpMorf, 1.0f)) {
        return;
    }
    if (mAnmNum == 8 && p1_checkEnd(mpMorf, 1.0f)) {
        m66C = m66C - 1;
        if (m66C <= 0) {
            setAnm(4, 15.0f);
        }
    } else if (mAnmNum == 12 && p1_checkEnd(mpMorf, 1.0f)) {
        m66C = m66C - 1;
        if (m66C <= 0) {
            setAnm(9, 8.0f);
        }
    }
    int iVar1;
    switch (p1_getMesgAnimeAttrInfo()) {
    case 0: iVar1 = mType != TYPE_P1C_e ? 0 : 4; break;
    case 1: iVar1 = 1; break;
    case 2: iVar1 = 2; break;
    case 3: iVar1 = 3; break;
    case 4: iVar1 = mType != TYPE_P1C_e ? 9 : 4; break;
    case 5: iVar1 = 10; break;
    case 6: iVar1 = 0xB; break;
    case 7:
        iVar1 = 0xC;
        m66C = 4;
        break;
    case 8: iVar1 = 0xD; break;
    case 9: iVar1 = 0xE; break;
    case 10: iVar1 = 0xF; break;
    case 0xB: iVar1 = 0x10; break;
    case 0xC: iVar1 = 4; break;
    case 0xD: iVar1 = 5; break;
    case 0xE: iVar1 = 6; break;
    case 0xF: iVar1 = 7; break;
    case 0x10:
        iVar1 = 8;
        m66C = 6;
        break;
    default:
        return;
    }
    p1_setMesgAnimeAttrInfo(0xFF);
    setAnm(iVar1, -1.0f);
}
VERIFY(0x022B22B8, &daNpc_P1_c::setAnimFromMsg);

/* 022B2638 */
BOOL daNpc_P1_c::privateCut() {
    WWHD_FUNC(0x022B2638, BOOL, this);
    /* cut_name_tbl[] = {"4013_msg", "4014_msg", "SETANM", "HEADSWING", "TALKMSG"} (0x101C2BF0) */
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(mEventCut6B0.mpEvtStaffName), nullptr, 0);
    if (staffIdx == -1) {
        return FALSE;
    }
    s32 actIdx = p1_getMyActIdx(staffIdx, 0x101C2BF0, 5, 1, 0);
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
    } else {
        int iVar4 = 0;
        if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
            switch (actIdx) {
            case 2:
                evn_setAnm_init(staffIdx);
                break;
            case 3:
                mHeadAnm.swing_vertical_init(2, 0x1000, 0x800, 1);
                break;
            case 4:
                evn_talk_init(staffIdx);
                break;
            }
        }
        switch ((u32)actIdx) {
        case 0:
        case 1:
            minigameExplainCut();
            return TRUE;
        case 4:
            iVar4 = evn_talk();
            setAnimFromMsg();
            break;
        default:
            iVar4 = 1;
            break;
        }
        if (iVar4 != 0) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    return TRUE;
}
VERIFY(0x022B2638, &daNpc_P1_c::privateCut);

/* 022B27C4 */
BOOL daNpc_P1_c::event_move() {
    WWHD_FUNC(0x022B27C4, BOOL, this);
    if (mEventCut6B0.cutProc()) {
        u8 attn = gabi::load<u8>(gabi::ea(&mEventCut6B0) + 0x60); /* getAttnFlag() */
        mbAttentionFlag = attn;
        if (attn == 0) {
            gabi::store<u8>(gabi::ea(&mEventCut6B0) + 0x60, attn); /* setAttnFlag(mbAttentionFlag) */
        }
        return TRUE;
    } else {
        return privateCut();
    }
}
VERIFY(0x022B27C4, &daNpc_P1_c::event_move);

/* 022B3274 */
BOOL daNpc_P1_c::setAttentionPos(cXyz* i_param_1) {
    WWHD_FUNC(0x022B3274, BOOL, this, i_param_1);
    u32 o = gabi::ea(i_param_1);
    gabi::store<u32>(o, gabi::load<u32>(gabi::ea(&eyePos)));
    f32 y = eyePos.y;
    gabi::store<u32>(o + 4, gabi::load<u32>(gabi::ea(&eyePos) + 4));
    gabi::store<u32>(o + 8, gabi::load<u32>(gabi::ea(&eyePos) + 8));
    i_param_1->y = y + p1_child(mType)->mAttnYPosOffset;
    return TRUE;
}
VERIFY(0x022B3274, &daNpc_P1_c::setAttentionPos);

/* 022B32B0 */
BOOL daNpc_P1_c::_execute() {
    WWHD_FUNC(0x022B32B0, BOOL, this);
    dComIfGp_get(); /* HD: an unused call */
    mHeadAnm.move();
    if (!demo_move()) {
        playTexPatternAnm();
        u32 mtrlSndId;
        if (mObjAcch.m_flags & 0x20 /* ChkGroundHit */) {
            mtrlSndId = gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), gabi::ea(this) + 0x544 /* mObjAcch.m_gnd */);
        } else {
            mtrlSndId = 0;
        }
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mpMorf->play(&current.pos, mtrlSndId, (s8)reverb);
        if (p1_event_getMode() == 0 || p1_checkCommandTalk(this)) {
            p1_pmf_call(this, &mActionFunc, nullptr);
            m65A = 0;
            mbAttentionFlag = 0;
        } else {
            event_move();
        }
    }
    kaji_anm();
    lookBack();
    setAttentionPos(gabi::at<cXyz>(gabi::ea(this) + 0x390)); /* &attention_info.position */
    if (m670 == 0) {
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    gabi::store<s8>(gabi::ea(this) + 0x1C9, gabi::call<s8>(0x024EF130, dComIfG_Bgsp(), gabi::ea(this) + 0x544)); /* tevStr.mRoomNo = GetRoomId */
    u8 color = gabi::call<u8>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(this) + 0x544); /* GetPolyColor */
    J3DModel* model = mpMorf->getModel();
    gabi::store<u8>(gabi::ea(this) + 0x1CA, color); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    p1_mtx_copy(gabi::ea(model) + 0xC8, gabi::ea(mDoMtx_stack_c::get()));
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return TRUE;
}
VERIFY(0x022B32B0, &daNpc_P1_c::_execute);

/* 022B3578 */
static BOOL daNpc_P1_Execute(daNpc_P1_c* i_this) {
    WWHD_FUNC(0x022B3578, BOOL, i_this);
    i_this->_execute();
    return TRUE;
}
VERIFY(0x022B3578, daNpc_P1_Execute);

/* 022B359C */
static BOOL daNpc_P1_IsDelete(daNpc_P1_c*) {
    WWHD_FUNC(0x022B359C, BOOL, (daNpc_P1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022B359C, daNpc_P1_IsDelete);

/* 022B35A4 */
BOOL daNpc_P1_c::_delete() {
    WWHD_FUNC(0x022B35A4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001FA70) /* "P1" */);
    if (heap && mpMorf) {
        mpMorf->stopZelAnime();
    }
    daNpc_P1_HIO_c* hio = p1_HIO();
    s32 n = hio->m8;
    if (n >= 0) {
        n = n - 1;
        hio->m8 = n;
        if (n < 0) {
            mDoHIO_deleteChild(hio->mNo);
        }
    }
    return TRUE;
}
VERIFY(0x022B35A4, &daNpc_P1_c::_delete);

/* 022B3628 */
static BOOL daNpc_P1_Delete(daNpc_P1_c* i_this) {
    WWHD_FUNC(0x022B3628, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022B3628, daNpc_P1_Delete);

/* 022B362C */
u32 daNpc_P1_c::getKajiID() {
    WWHD_FUNC(0x022B362C, u32, this);
    u32 kaji_id = 0xFFFFFFFF;
    u32 parent_id = parentActorID;
    if (parent_id != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = parent_id;
        fopAc_ac_c* actor = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
        if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == fpcNm_Obj_Pirateship_e) {
            kaji_id = gabi::load<u32>(gabi::ea(actor) + 0x490); /* daObjPirateship::Act_c::getKajiID() */
        }
    }
    return kaji_id;
}
VERIFY(0x022B362C, &daNpc_P1_c::getKajiID);

/* 022B43E8 */
static cPhs_State daNpc_P1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022B43E8, cPhs_State, i_this);
    return ((daNpc_P1_c*)i_this)->_create();
}
VERIFY(0x022B43E8, daNpc_P1_Create);

/* 022B5744 */
BOOL daNpc_P1_c::explainAction(void*) {
    WWHD_FUNC(0x022B5744, BOOL, this, (u32)0);
    if (mActionStatus == ACTION_STARTING_e) {
        mActionStatus = 1;
    } else if (mActionStatus == ACTION_ENDING_e && !p1_checkCommandTalk(this)) {
        fopAcM_orderSpeakEvent(this);
        p1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x022B5744, &daNpc_P1_c::explainAction);

/* 022B57C8: daNpc_P1_childHIO_c::daNpc_P1_childHIO_c (GHS: allocates when this == NULL) */
static daNpc_P1_childHIO_c* daNpc_P1_childHIO_ct(daNpc_P1_childHIO_c* self) {
    WWHD_FUNC(0x022B57C8, daNpc_P1_childHIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_P1_childHIO_c*)operator_new(0x30);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x1001F8E4;
    return self;
}
VERIFY(0x022B57C8, daNpc_P1_childHIO_ct);

/* 022B5808: daNpc_P1_HIO_c::daNpc_P1_HIO_c */
static daNpc_P1_HIO_c* daNpc_P1_HIO_ct(daNpc_P1_HIO_c* self) {
    WWHD_FUNC(0x022B5808, daNpc_P1_HIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_P1_HIO_c*)operator_new(0x9C);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x1001F8F4;
    gabi::call(0x028EFFD0, self->children, 3, 0x30, 0x022B57C8); /* __construct_array */
    daNpc_P1_childHIO_c* c = self->children;
    c[0].mAttnYPosOffset = 50.0f;
    c[0].mMaxHeadX = 0x9C4;
    c[0].mMinHeadX = (s16)0xF63C;
    c[0].mMaxBackboneX = 2000;
    c[0].mMinBackboneX = (s16)0xF830;
    c[0].mMaxHeadY = 5000;
    c[0].mMinHeadY = (s16)0xEC78;
    c[0].mMaxBackboneY = 8000;
    c[0].mMinBackboneY = (s16)0xE0C0;
    c[0].mMaxTurnStep = 1000;
    c[0].mLookBackTargetY = 0x708;
    c[0].mUnused20 = 0;
    c[0].mMaxTalkDist = 300.0f;
    c[0].mMorfBackup = 8.0f;
    c[0].mUnused2C = 8.0f;
    c[1].mAttnYPosOffset = 45.0f;
    c[1].mMaxHeadX = 0x9C4;
    c[1].mMinHeadX = (s16)0xF63C;
    c[1].mMaxBackboneX = 2000;
    c[1].mMinBackboneX = (s16)0xF830;
    c[1].mMaxHeadY = 10000;
    c[1].mMinHeadY = (s16)0xD8F0;
    c[1].mMaxBackboneY = 8000;
    c[1].mMinBackboneY = (s16)0xE0C0;
    c[1].mMaxTurnStep = 1000;
    c[1].mLookBackTargetY = 0x708;
    c[1].mUnused20 = 0;
    c[1].mMaxTalkDist = 250.0f;
    c[1].mMorfBackup = 8.0f;
    c[1].mUnused2C = 8.0f;
    c[2].mAttnYPosOffset = 45.0f;
    c[2].mMaxHeadX = 0x9C4;
    c[2].mMinHeadX = (s16)0xF63C;
    c[2].mMaxBackboneX = 2000;
    c[2].mMinBackboneX = (s16)0xF830;
    c[2].mMaxHeadY = 5000;
    c[2].mMinHeadY = (s16)0xEC78;
    c[2].mMaxBackboneY = 8000;
    c[2].mMinBackboneY = (s16)0xE0C0;
    c[2].mMaxTurnStep = 1000;
    c[2].mLookBackTargetY = 0x708;
    c[2].mUnused20 = 0;
    c[2].mMaxTalkDist = 250.0f;
    c[2].mMorfBackup = 7.0f;
    c[2].mUnused2C = 16.0f;
    self->mNo = -1;
    self->m8 = -1;
    return self;
}
VERIFY(0x022B5808, daNpc_P1_HIO_ct);

/* 022B5998: __sinit_d_a_npc_p1_cpp (compiler-generated) */
static void __sinit_d_a_npc_p1_cpp() {
    WWHD_FUNC(0x022B5998, void);
    sinit_header_statics(0x10467F24, 0x101C2C04);
    daNpc_P1_HIO_ct(p1_HIO()); /* l_HIO */
}
VERIFY(0x022B5998, __sinit_d_a_npc_p1_cpp);

/* 022B5A38: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dtor(SafeString* self, u32 flags) {
    WWHD_FUNC(0x022B5A38, void, self, flags);
    if (self != nullptr && (flags & 1)) operator_delete(self);
}
VERIFY(0x022B5A38, SafeString_dtor);

/* 022B5A4C: daNpc_P1_c deleting destructor (HD virtual destructor) */
static void daNpc_P1_dtor(daNpc_P1_c* self, u32 flags) {
    WWHD_FUNC(0x022B5A4C, void, self, flags);
    if (self == nullptr) return;
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    u32 p = gabi::ea(self);
    gabi::store<u32>(p + 0x47C, 0x1001F8B4); /* dBgS_ObjAcch vtables (this TU) */
    gabi::store<u32>(p + 0x470, 0x1001F8C4);
    gabi::call(0x024EFD9C, &self->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x025D50BC, self, 0);            /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x022B5A4C, daNpc_P1_dtor);

/* 022B5AE8: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(SafeString* self) {
    WWHD_FUNC(0x022B5AE8, void, self);
}
VERIFY(0x022B5AE8, SafeString_assureTerminationImpl);
