/**
 * d_a_npc_sv.cpp (WWHD)
 * NPC - Salvage Corp. members
 *
 * The GameCube decompilation of this TU
 * (zeldaret/tww src/d/actor/d_a_npc_sv.cpp) has only "Nonmatching" placeholders: every function
 * below is written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube
 * names and the TU's function list. "HD:" marks a known difference from the GameCube TU.
 */
#include "d/actor/d_a_npc_sv.h"

// HD: this translation unit owns the compiler-generated metadata.
const gc_tu_data gc_tu = {.safeString = 0x10022430, .safeStringAssure = 0x022E7584,
    .objAcchDt = {0x10022448, 0x10022458}, .headerStatics = {0x10468794, 0x101C6798}};
enum { SV_VTBL = 0x100226A4, SV_MSG_DISPLAYED = 0xE, SV_MSG_DESTROYED = 0x13 };
static const char* l_staff_name_tbl[] = {"Sv0", "Sv1", "Sv2", "Sv3"};
static const char* l_arcname_tbl[] = {"Sv", "Sv", "Sv", "Sv"};
static s32 l_bdl_ix_tbl[] = {1, 1, 1, 1};
static s32 l_bck_ix_tbl[] = {12, 8, 9, 10, 14, 6, 13, 5, 11, 4, 3, 7, 13, 5, 11, 4, 3, 7, 13, 5, 11, 4, 3, 7};
static u32 l_npc_anm_sv1_tbl[] = {0x101C63F0, 0x101C63FF, 0x101C63D0, 0x101C63D8, 0x101C63E0, 0x101C63E8};
static u32 l_npc_se_sv1_tbl[] = {0x00000000, 0x00000000, 0x00004927, 0x00004929, 0x00004928, 0x00000000};
static u32 l_msg_anm_tbl[] = {0x101C63F0, 0x101C63C0, 0x101C63F3, 0x101C63F6, 0x101C63F9, 0x101C63C8};
static u32 l_msg_tbl[] = {0x101C656C, 0x101C65A8, 0x101C65E4, 0x101C6454, 0x101C646C, 0x101C6484, 0x101C6600, 0x101C649C, 0x101C661C, 0x101C64B4, 0x101C6638, 0x101C6654};
static u16 l_msg_flag_tbl[] = {1, 1, 2, 2, 0, 0, 4, 0, 8, 0, 16, 0};
static s16 l_talk_ev_tbl[] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};
static u32 l_method[] = {0x022E5184, 0x022E5910, 0x00000000};
static u32 l_cut_name_tbl[] = {0x10022644, 0x1002264C, 0x10022658, 0x10022664, 0x10022670};
static sSvAnmDat l_anm_wait = {0,8,-1};
static sSvAnmDat l_anm_end = {1,8,-1};
static sSvAnmDat l_anm_ship = {5,0,-1};
static sSvNpcDat l_npc_dat[] = {
    {4000, 6000, 0, 8000, 0, -6000, -3000, -8000, 1000, 0.0f, -5.0f, 0.0f, 0.0f, 185.0f, 150.0f, 600.0f, 1500.0f, 32767, 1000, 60.0f, 150.0f, 30, 1, 1},
    {4096, 0, 0, 0, -4096, 0, 0, 0, 2048, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 150.0f, 0.0f, 0.0f, 32767, 2048, 60.0f, 150.0f, 30, 0, 1},
    {4096, 0, 0, 0, -4096, 0, 0, 0, 2048, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 150.0f, 0.0f, 0.0f, 32767, 2048, 60.0f, 150.0f, 30, 0, 1},
    {4096, 0, 0, 0, -4096, 0, 0, 0, 2048, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 150.0f, 0.0f, 0.0f, 16384, 2048, 60.0f, 150.0f, 30, 0, 1}
};
struct SvInitEntry { using Function = s32 (daNpcSv_c::*)(); gabi::ptmf<Function> mFunction; };
// HD: both initializers are called through a full-register GHS descriptor; the result is truncated by mMode.
static SvInitEntry l_init_tbl[] = {{&daNpcSv_c::executeWaitInit},{&daNpcSv_c::executeTalkInit}};
struct SvMoveEntry { using Function = void (daNpcSv_c::*)(); gabi::ptmf<Function> mFunction; };
static SvMoveEntry l_exec_tbl[] = {{&daNpcSv_c::executeWait},{&daNpcSv_c::executeTalk}};
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static be<s32>& l_present_item_no = gabi::object_at<be<s32>>(0x101C63BC);

/* 022E50CC */
static daNpcSv_c* Sv_actorCtor(daNpcSv_c* p) {
    if (p == nullptr) {
        p = (daNpcSv_c*)operator_new(0x860);
        if (p == nullptr) return nullptr;
    }
    Sv_npcCtor(p);
    p->__vtbl = SV_VTBL;
    u8 no = p->getPrmNpcNo();
    p->mAnm = 0;
    p->mNpcNo = no;
    p->mHomeAngleY = p->home.angle.y;
    p->mTalkEnabled = 1;
    p->mMode = 0;
    p->mHeadOnly = 1;
    p->mCreated = 0;
    p->mFrame = 0;
    p->mLookMode = 0;
    p->mFlags = 0;
    p->mNextMorf = -1.0f;
    if (dComIfGs_checkGetItem(0xE5)) {
        p->mFlags |= 0x20;
    }
    return p;
}

/* 022E4CA0 */
static BOOL daNpc_People_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpcSv_c* i_this = static_cast<daNpcSv_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
        // HD: no null check of the user area
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        mtx_copy(model->getAnmMtx(jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}

/* 022E50AC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpcSv_c*>(i_this)->createHeap();
}

/* 022E5184 */
static cPhs_State phase_1(daNpcSv_c* i_this) {
    /* fopAcM_SetupActor(i_this, daNpcSv_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) Sv_actorCtor(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->mCreated = 1;
    return cPhs_NEXT_e;
}

/* 022E5910 */
static cPhs_State phase_2(daNpcSv_c* i_this) {
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, l_arcname_tbl[i_this->mNpcNo]);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)CheckCreateHeap), 0x3800)) {
            i_this->mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        ret = i_this->createInit();
    }
    return ret;
}

/* 022E59A4 */
cPhs_State daNpcSv_c::_create() {
    return Sv_phaseHandler(&mPhsHandler, gabi::ea(l_method), this);
}

/* 022E4DF8 */
BOOL daNpcSv_c::createHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[mNpcNo], l_bdl_ix_tbl[mNpcNo]);
    J3DAnmTransform* a_anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[mNpcNo], l_bck_ix_tbl[mNpcNo * 6 + mAnm]);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, a_anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x37441422);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = (s8)a_mdl_dat->getJointName()->getIndex( "head");
    if (m_jnt.mHeadJntNum < 0) /* JUT_ASSERT(0x3B6, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_sv.cpp", 0x3B6, "m_jnt.getHeadJntNum() >= 0");
    m_jnt.mBackboneJntNum = (s8)a_mdl_dat->getJointName()->getIndex( "backbone");
    if (m_jnt.mBackboneJntNum < 0) /* JUT_ASSERT(0x3BA, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_sv.cpp", 0x3BA, "m_jnt.getBackboneJntNum() >= 0");
    for (u16 i = 0; i < a_mdl_dat->getJointNum(); i++) {
        if (i == m_jnt.mHeadJntNum || i == m_jnt.mBackboneJntNum) {
            a_mdl_dat->getJointNodePointer(i)->setCallBack(daNpc_People_nodeCallBack);
        }
    }
    mpMorf->mpModel->mUserArea = gabi::ea(this); /* model->setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}

/* 022E56C8 */
s32 daNpcSv_c::createInit() {
    gravity = -9.0f;
    setAnmTbl(&l_anm_wait);
    mEventIdx[0] = dComIfGp_evmng_getEventIdx("SV_TALK_P1_1ST", 0xFF);
    mEventIdx[1] = dComIfGp_evmng_getEventIdx("SV_TALK_P1_2ND", 0xFF);
    mEventIdx[2] = dComIfGp_evmng_getEventIdx("SV_TALK_P4_1ST", 0xFF);
    u8 talkNo = getTalkNo();
    eventInfo.mEventId = mEventIdx[l_talk_ev_tbl[talkNo]]; /* eventInfo.setEventId */
    mEventCut.setActorInfo2(l_staff_name_tbl[mNpcNo], this);
    mLookVel = 0;
    mAttn = 0;
    mTalk = 0;
    mPlayerNearby = 0;
    cullMtx = gabi::ea(mpMorf->getModel()->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xB1; /* attention_info.distances[1] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xB1; /* attention_info.distances[3] */
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    sSvNpcDat* dat = (&l_npc_dat[mNpcNo]);
    m_jnt.setParam(dat->mMaxBackboneX, dat->mMaxBackboneY, dat->mMinBackboneX, dat->mMinBackboneY, dat->mMaxHeadX,
                   dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY, dat->mMaxTurnStep);
    dat = (&l_npc_dat[mNpcNo]);
    mTurnToTarget = dat->mTurnToTarget;
    mFollowPlayer = dat->mFollowPlayer;
    mTalkDist = dat->mTalkDist;
    mLookAngle = dat->mLookAngle;
    setMtx();
    mpMorf->getModel()->calc();
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    dat = (&l_npc_dat[mNpcNo]);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    mCyl.SetStts(&mStts);
    pos->z = current.pos.z;
    setCollision(&mCyl, pos, dat->mCylR, dat->mCylH);
    return cPhs_COMPLEATE_e;
}

/* 022E59BC */
BOOL daNpcSv_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname_tbl[mNpcNo]);
    if (heap != nullptr && mpMorf != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022E6E04 */
BOOL daNpcSv_c::_draw() {
    // HD: no shadow
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mpMorf->updateDL();
    return TRUE;
}

/* 022E6BD4 */
BOOL daNpcSv_c::_execute() {
    gabi::Local<be<u32>> ship;
    Sv_searchByID(parentActorID, ship);
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck()) {
        (this->*l_exec_tbl[mMode].mFunction)();
        eventOrder();
    } else {
        eventMove();
        eventOrder();
    }
    if (mNpcNo == 0 && mAnm == 5) {
        mpMorf->setFrame((f32)mFrame);
    } else {
        playAnm();
    }
    if (*ship != 0) {
        mFrame = Sv_ship(*ship)->mAnimationFrame;
    }
    mObjAcch.CrrPos(dComIfG_Bgsp());
    sSvNpcDat* dat = (&l_npc_dat[mNpcNo]);
    gabi::Local<cXyz> pos;
    pos->y = current.pos.y;
    pos->x = current.pos.x;
    pos->z = current.pos.z;
    setCollision(&mCyl, pos, dat->mCylR, dat->mCylH);
    dat = (&l_npc_dat[mNpcNo]);
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    f32 x = current.pos.x;
    cXyz* attnPos = &attention_info.position; /* attention_info.position */
    attnPos->x = x;
    attnPos->y = y + dat->mAttnY;
    attnPos->z = z;
    eyePos.x = x;
    eyePos.y = y + dat->mEyeY;
    eyePos.z = z;
    lookBack();
    setMtx();
    return FALSE;
}

/* 022E6E6C */
s32 daNpcSv_c::executeCommon() {
    mOrder = mAttn != 0;
    return 0;
}

/* 022E5DC8 */
void daNpcSv_c::executeSetMode(int mode) {
    mMode = (u8)(this->*l_init_tbl[mode].mFunction)();
}

/* 022E6E84 */
s32 daNpcSv_c::executeWaitInit() {
    setAnmTbl(&l_anm_wait);
    sSvNpcDat* dat = (&l_npc_dat[mNpcNo]);
    m_jnt.setParam(dat->mMaxBackboneX, dat->mMaxBackboneY, dat->mMinBackboneX, dat->mMinBackboneY, dat->mMaxHeadX,
                   dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY, dat->mMaxTurnStep);
    return 0;
}

/* 022E6F04 */
void daNpcSv_c::executeWait() {
    gabi::Local<be<u32>> shipId;
    Sv_searchByID(parentActorID, shipId);
    if (*shipId == 0) {
        return;
    }
    fopAc_ac_c* ship = Sv_actor(*shipId);
    if (mNpcNo == 0) {
        if (mPlayerNearby != 0) {
            mOrder = 1;
            if (mPlayerNearby != mPreviousPlayerNearby) {
                mLookAtPlayer = 1;
            }
            Sv_view<SvShip>(ship)->mPlayerNearby = 1;
        } else {
            mOrder = 0;
            mLookAtPlayer = 0;
            Sv_view<SvShip>(ship)->mPlayerNearby = 0;
        }
        if (mLookAtPlayer == 0) {
            ship = Sv_actor(*shipId);
            mLookAngleY = ship->shape_angle.y;
            current.angle.x = ship->shape_angle.x;
            mHeadOnly = 0;
            current.angle.z = ship->shape_angle.z;
            m_jnt.mbTrn = 1;
            mLookMode = 2;
            setAnmTbl(&l_anm_ship);
        } else {
            setAnmTbl(&l_anm_wait);
        }
        mPreviousPlayerNearby = mPlayerNearby;
        return;
    }

    mTurnToTarget = 1;
    s32 shipMode = Sv_view<SvShip>(ship)->mMode;
    if (shipMode == 8 || shipMode == 9 || shipMode == 5) {
        mHeadOnly = 0;
        mLookAngleY = ship->shape_angle.y;
        m_jnt.mbTrn = 1;
        mLookMode = 2;
    } else if (shipMode == 4 || shipMode == 6) {
        s16 y = ship->shape_angle.y;
        if (Sv_view<SvShip>(ship)->mCraneSide == 0) {
            mHeadOnly = 0;
            mLookAngleY = y + 0x4000;
        } else {
            mHeadOnly = 0;
            mLookAngleY = y - 0x4000;
        }
        m_jnt.mbTrn = 1;
        mLookMode = 2;
    } else {
        s16 target;
        if (Sv_view<SvShip>(ship)->mCraneSide == 0) {
            target = (s16)(ship->shape_angle.y + 0x4000);
        } else {
            target = (s16)(ship->shape_angle.y - 0x4000);
        }
        if (current.angle.y == target) {
            cXyz* p = &Sv_view<SvShip>(ship)->mRopeEndPos;
            f32 x = p->x;
            f32 y = p->y;
            f32 z = p->z;
            mHeadOnly = 1;
            mLookPos.y = y;
            mLookPos.x = x;
            mLookPos.z = z;
            mLookMode = 1;
            mTurnToTarget = 0;
        } else {
            mLookAngleY = target;
            mHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 2;
        }
    }
    if (Sv_view<SvShip>(ship)->mMode == 7) {
        if (mAttentionAnimationStarted == 0) {
            mAttentionAnimationStarted = 1;
            setAnmTbl(Sv_animation(l_npc_anm_sv1_tbl[1]));
        }
    } else if (mAttentionAnimationStarted == 1) {
        mAttentionAnimationStarted = 0;
        setAnmTbl(Sv_animation(l_npc_anm_sv1_tbl[0]));
    }
}

/* 022E7214 */
u8 daNpcSv_c::executeTalkInit() {
    if (mTalkEnabled != 0) {
        return 1;
    }
    return mMode;
}

/* 022E7230 */
void daNpcSv_c::executeTalk() {
    executeCommon();
    if (talk2(1, this) == fopMsgStts_BOX_CLOSED_e) {
        mTalk = 0;
        mLookAtPlayer = 0;
        executeSetMode(0);
        dComIfGp_event_reset();
    } else {
        setAnmFromMsgTag();
    }
}

/* 022E5D94 */
void daNpcSv_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        return;
    }
    if (cmd == dEvtCmd_INTALK_e) {
        if (mOrder == 2 || mOrder == 1) {
            mTalk = 1;
        }
    }
}

/* 022E688C */
void daNpcSv_c::eventOrder() {
    if (mOrder == 2 || mOrder == 1) {
        if (isTalkOK()) {
            eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
            if (mOrder == 2) {
                fopAcM_orderSpeakEvent(this);
            }
        }
    }
}

/* 022E6700 */
void daNpcSv_c::eventMove() {
    if (dComIfGp_evmng_endCheck(mEventIdx[0]) || dComIfGp_evmng_endCheck(mEventIdx[1]) || dComIfGp_evmng_endCheck(mEventIdx[2])) {
        u8 talkNo = getTalkNo();
        eventInfo.mEventId = mEventIdx[l_talk_ev_tbl[talkNo]]; /* eventInfo.setEventId */
        dComIfGp_event_reset();
        mLookAtPlayer = 0;
        mEvtFlags &= ~2;
        mTalk = 0;
        executeSetMode(0);
        return;
    }
    u8 attn = mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (mEventCut.mbAttention == 0) {
            mEventCut.mbAttention = attn;
        }
    } else {
        privateCut();
        if (mNpcNo == 0) {
            setAnmFromMsgTag();
        }
    }
}

/* 022E64C8 */
void daNpcSv_c::privateCut() {
    const char* name = l_staff_name_tbl[mNpcNo];
    s32 staffId = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    if (staffId != -1) {
        s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffId, reinterpret_cast<gptr<char>*>(l_cut_name_tbl), 5, TRUE, 0);
        mActIdx = actIdx;
        dEvent_manager_c* evtMng = dComIfGp_getPEvtManager();
        if (actIdx == -1) {
            Sv_cutEnd(evtMng, staffId); /* cutEnd */
        } else {
            if (Sv_advance(evtMng, staffId) /* getIsAddvance */) {
                switch ((u32)(s32)mActIdx) {
                case 0: eventMesSetInit(staffId); break;
                case 1: eventGetItemInit(staffId); break;
                case 2: eventSetAngleInit(); break;
                case 3: eventAttentionInit(staffId); break;
                case 4: eventTurnOkInit(); break;
                }
            }
            switch ((u32)(s32)mActIdx) {
            case 0:
                if (eventMesSet()) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
                break;
            case 3:
                if (eventAttention()) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
                break;
            default:
                dComIfGp_evmng_cutEnd(staffId);
                break;
            }
        }
    }
    if (mEvtFlags & 2) {
        mLookMode = 2;
        mLookAngleY = mEvtAngleY;
        mHeadOnly = 0;
        m_jnt.mbTrn = 1;
    }
}

/* 022E5E50 */
void daNpcSv_c::eventMesSetInit(int staffId) {
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffId, "MsgNo", 3 /* integer */);
    if (pMsgNo != nullptr) {
        mpMsgTbl = 0;
        if (*pMsgNo == 0) {
            setMessage(Sv_getMessage(this));
        } else {
            setMessage(*pMsgNo);
        }
    } else {
        mpMsgTbl = mpMsgTbl + 4;
        setMessage(*Sv_messagePointer(mpMsgTbl));
    }
    dComIfGp_event_setTalkPartner(this);
}

/* 022E6360 */
BOOL daNpcSv_c::eventMesSet() {
    return talk2(0, this) == fopMsgStts_BOX_CLOSED_e;
}

/* 022E5F4C */
void daNpcSv_c::eventGetItemInit(int staffId) {
    void* pItemNo = dComIfGp_evmng_getMySubstanceP(staffId, "ItemNo", 3 /* integer */);
    s8 roomNo = current.roomNo;
    s32 itemPid;
    if (pItemNo != nullptr) {
        itemPid = Sv_createPresent(&current.pos, (s32)l_present_item_no, 0, -1, roomNo, nullptr, nullptr);
    } else {
        itemPid = Sv_createPresent(&current.pos, mItemNo, 0, -1, roomNo, nullptr, nullptr);
    }
    if (itemPid != -1) {
        dComIfGp_event_setItemPartnerId(itemPid);
    }
}

/* 022E600C */
void daNpcSv_c::eventSetAngleInit() {
    gabi::Local<be<u32>> ship;
    Sv_searchByID(parentActorID, ship);
    if (*ship != 0) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        fopAc_ac_c* a = Sv_actor(*ship);
        gabi::Local<cXyz> shipPos;
        shipPos->x = a->current.pos.x;
        shipPos->y = a->current.pos.y;
        shipPos->z = a->current.pos.z;
        gabi::Local<cXyz> plPos;
        plPos->x = player->current.pos.x;
        plPos->y = player->current.pos.y;
        plPos->z = player->current.pos.z;
        gabi::Local<be<s16>> angY;
        dNpc_calc_DisXZ_AngY(shipPos, plPos, nullptr, angY);
        a = Sv_actor(*ship);
        s16 diff = (s16)(*angY - a->shape_angle.y);
        *angY = diff;
        if (diff > 0) {
            s16 y = a->shape_angle.y + 0x4000;
            mHomeAngleY = y;
            mEvtAngleY = y;
        } else {
            s16 y = a->shape_angle.y - 0x4000;
            mHomeAngleY = y;
            mEvtAngleY = y;
        }
        mEvtFlags |= 2;
    } else {
        mEvtAngleY = mHomeAngleY;
        mEvtFlags |= 2;
    }
}

/* 022E610C */
void daNpcSv_c::eventAttentionInit(int staffId) {
    void* pTimer = dComIfGp_evmng_getMySubstanceP(staffId, "Timer", 3 /* integer */);
    s16 timer = 12;
    u8 no = mNpcNo;
    if (pTimer != nullptr) {
        timer = Sv_view<SvTimerArgument>(pTimer)->mTimer; /* (s16)*(int*)pTimer */
    }
    mAttnTimer = timer;
    mAttentionAnimationStarted = 0;
    if (no == 2) {
        dComIfGp_event_setTalkPartner(this);
    }
}

/* 022E6394 */
BOOL daNpcSv_c::eventAttention() {
    if (mAttnTimer != 0) {
        mAttnTimer = mAttnTimer - 1;
        if (mAttnTimer == 0) {
            fopAc_ac_c* sv0 = Sv_searchFromName(l_staff_name_tbl[0] /* "Sv0" */, 0, 0);
            if (sv0 != nullptr) {
                u8 a_ptn = static_cast<daNpcSv_c*>(sv0)->mMsgAnm;
                if (a_ptn >= 6) /* JUT_ASSERT(0x6BE, a_ptn < ARRAY_SIZE(l_npc_anm_sv1_tbl)) */
                    JUT_ASSERT_fail("d_a_npc_sv.cpp", 0x6BE, "a_ptn < (sizeof(l_npc_anm_sv1_tbl) / sizeof(l_npc_anm_sv1_tbl[0]))");
                if (a_ptn < 6) {
                    setAnmTbl(Sv_animation(l_npc_anm_sv1_tbl[a_ptn]));
                }
                if (mNpcNo == 1) {
                    if (a_ptn >= 6) /* JUT_ASSERT(0x6C4, a_ptn < ARRAY_SIZE(l_npc_se_sv1_tbl)) */
                        JUT_ASSERT_fail("d_a_npc_sv.cpp", 0x6C4, "a_ptn < (sizeof(l_npc_se_sv1_tbl) / sizeof(l_npc_se_sv1_tbl[0]))");
                    if (a_ptn < 6) {
                        Sv_soundStart(l_npc_se_sv1_tbl[a_ptn]);
                    }
                }
            } else {
                setAnmTbl(Sv_animation(l_npc_anm_sv1_tbl[2]));
                Sv_soundStart(l_npc_se_sv1_tbl[2]);
            }
        }
        return FALSE;
    }
    if (mAnmFlags & 1) {
        setAnmTbl(Sv_animation(l_npc_anm_sv1_tbl[0]));
        return TRUE;
    }
    return FALSE;
}

/* 022E619C */
void daNpcSv_c::eventTurnOkInit() {
    mTurnToTarget = 1;
}

/* 022E61A8 */
u16 daNpcSv_c::talk2(int mode, fopAc_ac_c* actor) {
    u32 mng = gabi::ea(fopNpc_npc_c::mpCurrMsg.get());
    u16 status = 0xFF; /* fopMsgStts_BOX_CLOSED_e */
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (mode == 1) {
            mCurrMsgNo = Sv_getMessage(this);
        }
        mCurrMsgBsPcId = Sv_messageSet(mng, mCurrMsgNo, &actor->eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
        }
        return status;
    }
    if (mbHasMsg == 0) {
        mbHasMsg = 1;
        return status;
    }
    status = (u16)Sv_messageStatus(mng);
    if (status == SV_MSG_DISPLAYED) {
        Sv_setMessageStatus(mng, Sv_nextMessage(this, &mCurrMsgNo));
        if (Sv_messageStatus(mng) == fopMsgStts_MSG_CONTINUES_e) {
            Sv_messageSet(mng, mCurrMsgNo, nullptr);
        }
    } else if (status == fopMsgStts_BOX_CLOSED_e) {
        Sv_setMessageStatus(mng, SV_MSG_DESTROYED);
        mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
    }
    Sv_animationAttribute(this, status);
    return status;
}

/* 022E72B0 */
u16 daNpcSv_c::next_msgStatus(be<u32>* pMsgNo) {
    if (*pMsgNo == 0) {
        return fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
    }
    if (mpMsgTbl == 0) {
        return fopMsgStts_MSG_ENDS_e;
    }
    mpMsgTbl = mpMsgTbl + 4;
    u32 next = *Sv_messagePointer(mpMsgTbl);
    switch (next) {
    case 0:
        mpMsgTbl = 0;
        return fopMsgStts_MSG_ENDS_e;
    case 1:
        return fopMsgStts_MSG_ENDS_e;
    case 2:
    case 3:
    case 4:
    case 5:
        mMsgAnm = (u8)next;
        return fopMsgStts_MSG_ENDS_e;
    default:
        *pMsgNo = next;
        return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    }
}

/* 022E7364 */
u32 daNpcSv_c::getMsg() {
    u32 msgNo = 0;
    mpMsgTbl = 0;
    if (dComIfGp_event_chkTalkXY()) {
        dComIfGp_get(); // HD: a play accessor whose result is unused
    } else {
        u8 talkNo = mTalkNo;
        u16 flags = mFlags;
        mpMsgTbl = l_msg_tbl[talkNo];
        flags |= l_msg_flag_tbl[talkNo];
        mFlags = flags;
        if (flags & 0x10) {
            dComIfGs_onEventBit(0x2F80);
        }
    }
    if (mpMsgTbl != 0) {
        msgNo = *Sv_messagePointer(mpMsgTbl);
        if (msgNo != 0) {
            return msgNo;
        }
    }
    setAnmTbl(&l_anm_end);
    return msgNo;
}

/* 022E5E48 */
void daNpcSv_c::setMessage(u32 msgNo) {
    mCurrMsgNo = msgNo;
}

/* 022E66A4 */
void daNpcSv_c::setAnmFromMsgTag() {
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (attr <= 5) {
        setAnmTbl(Sv_animation(l_msg_anm_tbl[attr]));
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}

/* 022E50B0 */
u8 daNpcSv_c::getPrmNpcNo() {
    s32 no = argument;
    if ((u32)no >= 4) return 0;
    return (u8)no;
}

/* 022E54E8 */
void daNpcSv_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    sSvNpcDat* dat = (&l_npc_dat[mNpcNo]);
    gabi::Local<cXyz> offset;
    offset->x = dat->mOffsetX;
    offset->y = dat->mOffsetY;
    offset->z = dat->mOffsetZ;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
    mDoMtx_stack_c::transS(current.pos.x + offset->x, current.pos.y + offset->y, current.pos.z + offset->z);
    Sv_xyzRotate(mDoMtx_stack_c::get(), current.angle.x, 0, current.angle.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 022E5A24 */
void daNpcSv_c::chkAttention() {
    mPlayerNearby = 0;
    if (mEventCut.mbAttention != 0) {
        mLookPos.y = mEventCut.mPos.y;
        mLookPos.z = mEventCut.mPos.z;
        mLookMode = 1;
        mLookPos.x = mEventCut.mPos.x;
        if (mTurnToTarget != 0) {
            mHeadOnly = 0;
            m_jnt.mbTrn = 1;
        } else {
            mHeadOnly = 1;
        }
        if (mAttn == 0) {
            mAttn = 1;
        }
        mLookSpeed = (&l_npc_dat[mNpcNo])->mLookSpeed;
        return;
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    gabi::Local<cXyz> plPos;
    plPos->x = player->current.pos.x;
    plPos->y = player->current.pos.y;
    f32 talkDist = mTalkDist;
    s32 lookAngle = mLookAngle;
    plPos->z = player->current.pos.z;
    gabi::Local<be<f32>> dist;
    gabi::Local<be<s16>> angY;
    dNpc_calc_DisXZ_AngY(pos, plPos, dist, angY);
    u8 attn = mAttn;
    s16 ang = *angY;
    s16 shapeY = shape_angle.y;
    if (attn != 0) {
        talkDist += 100.0f;
        lookAngle += 0x71C;
    }
    s16 diff = (s16)(ang - shapeY);
    *angY = diff;
    f32 d = *dist;
    s32 absDiff = diff < 0 ? -diff : diff;
    if (talkDist > d && lookAngle > absDiff) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, (&l_npc_dat[mNpcNo])->mEyeOffset);
        mLookPos.copy(*eye);
        mLookMode = 1;
        mHeadOnly = mTurnToTarget == 0;
        u8 attn2 = mAttn;
        if (mFollowPlayer == 0) {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
        if (attn2 == 0) {
            mAttn = 1;
        }
        mPlayerNearby = 1;
        mLookSpeed = (&l_npc_dat[mNpcNo])->mLookSpeed;
        return;
    }

    if (attn == 1) {
        mAttn = 0;
        mLookTimer = (&l_npc_dat[mNpcNo])->mLookTimer;
    }
    sSvNpcDat* dat = (&l_npc_dat[mNpcNo]);
    if (dat->mLookDist > d) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, dat->mEyeOffset);
        mLookPos.copy(*eye);
        mLookMode = 1;
        mHeadOnly = mTurnToTarget == 0;
        if (mFollowPlayer == 0) {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
        mPlayerNearby = 1;
        mLookSpeed = (&l_npc_dat[mNpcNo])->mLookSpeed;
        return;
    }

    u32 keep = mKeepLookTimer;
    mLookMode = 0;
    if (keep == 0) {
        if (mLookTimer != 0) {
            mLookTimer = mLookTimer - 1;
        } else {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
    }
    mLookSpeed = (&l_npc_dat[mNpcNo])->mLookSpeed;
}

/* 022E69C8 */
void daNpcSv_c::lookBack() {
    s8 mode = mLookMode;
    f32 eyeX = eyePos.x;
    s16 defY = current.angle.y;
    u8 headOnly = mHeadOnly;
    s16 vel = mLookSpeed;
    f32 eyeZ = eyePos.z;
    cXyz* target = nullptr;
    f32 eyeY = eyePos.y;
    gabi::Local<cXyz> dst;
    bool turn;
    if (mode == 1) {
        dst->copy(mLookPos);
        target = dst;
        turn = mTalk != 0;
    } else if (mode == 2) {
        turn = mTalk != 0;
        defY = mLookAngleY;
    } else {
        turn = mTalk != 0;
    }
    bool calc;
    if (turn && mTurnToTarget != 0) {
        m_jnt.mbTrn = 1;
        headOnly = 0;
        calc = true;
    } else {
        calc = m_jnt.mbTrn != 0;
    }
    gabi::Local<cXyz> eye;
    if (!calc) {
        mLookVel = 0;
        eye->x = eyeX;
        eye->y = eyeY;
        eye->z = eyeZ;
        Sv_lookAtTarget(&m_jnt, &current.angle.y, target, eye, defY, 0, headOnly);
    } else {
        s16 turnSpeed = mEventCut.mTurnSpeed;
        if (turnSpeed != 0) {
            vel = turnSpeed;
        }
        cLib_addCalcAngleS2(&mLookVel, vel, 4, 0x800);
        s16 lookVel = mLookVel;
        eye->x = eyeX;
        eye->y = eyeY;
        eye->z = eyeZ;
        Sv_lookAtTarget(&m_jnt, &current.angle.y, target, eye, defY, lookVel, headOnly);
    }
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
}

/* 022E68F8 */
void daNpcSv_c::playAnm() {
    mDoExt_McaMorf* morf = mpMorf;
    mAnmFlags &= ~1;
    if (morf->play(nullptr, 0, 0)) {
        if (mpAnmTbl != nullptr && mAnmLoop > 0) {
            mAnmLoop = mAnmLoop - 1;
            if (mAnmLoop == 0) {
                mpAnmTbl = mpAnmTbl.get() + 1;
                if (setAnmTbl(mpAnmTbl.get())) {
                    mAnmFlags |= 1;
                }
            } else {
                setAnm(mpAnmTbl->mAnm, 0, 0.0f);
            }
        }
    }
}

/* 022E51E0 */
void daNpcSv_c::setAnm(u8 anm, int loopMode, f32 morf) {
    if (!(mNextMorf < 0.0f)) {
        morf = mNextMorf;
        mNextMorf = -1.0f;
    }
    u32 no = mNpcNo;
    J3DAnmTransform* a_anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[no], l_bck_ix_tbl[no * 6 + anm]);
    mpMorf->setAnm(a_anm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mAnm = anm;
}

/* 022E52FC */
BOOL daNpcSv_c::setAnmTbl(sSvAnmDat* p) {
    if (p->mAnm == 0xFF) {
        mpAnmTbl = nullptr;
        return TRUE;
    }
    mpAnmTbl = p;
    s8 loop = p->mLoop;
    mAnmLoop = loop;
    if (loop > 0) {
        setAnm(p->mAnm, 0, (f32)p->mMorf);
    } else if (mAnm != p->mAnm) {
        setAnm(p->mAnm, 2, (f32)p->mMorf);
    }
    return FALSE;
}

/* 022E5638 */
void daNpcSv_c::setCollision(dCcD_Cyl* cyl, cXyz* pos, f32 r, f32 h) {
    cyl->SetC(pos);
    cyl->SetR(r);
    cyl->SetH(h);
    dComIfG_Ccsp_Set(cyl);
}

/* 022E53C8 */
u8 daNpcSv_c::getTalkNo() {
    BOOL has = dComIfGs_checkGetItem(0x25) != 0;
    u16 flags = mFlags;
    u8 no;
    mTalkNo = 0;
    if (!(flags & 0x20)) {
        if (!(flags & 1)) {
            no = (u8)has;
        } else if (!(flags & 2)) {
            no = has ? 3 : 2;
        } else {
            no = has ? 5 : 4;
        }
    } else if (!has) {
        no = (flags & 4) ? 7 : 6;
    } else if (!dComIfGs_isCollect(0, 1)) {
        no = (mFlags & 8) ? 9 : 8;
    } else {
        no = dComIfGs_isEventBit(0x2F80) ? 0xB : 0xA;
    }
    mTalkNo = no;
    return no;
}

/* 022E685C */
BOOL daNpcSv_c::isTalkOK() {
    /* !daPy_getPlayerLinkActorClass()->checkPlayerFly-like status bit (play + 0x5CD8, 0x100000) */
    return dComIfGp_checkPlayerStatus0(0, 0x100000) == 0;
}

/* 022E59B8 */
static cPhs_State daNpc_PeopleCreate(void* i_this) {
    return ((daNpcSv_c*)i_this)->_create();
}

/* 022E5A20 */
static BOOL daNpc_PeopleDelete(void* i_this) {
    return ((daNpcSv_c*)i_this)->_delete();
}

/* 022E6E00 */
static BOOL daNpc_PeopleExecute(void* i_this) {
    return ((daNpcSv_c*)i_this)->_execute();
}

/* 022E6E68 */
static BOOL daNpc_PeopleDraw(void* i_this) {
    return ((daNpcSv_c*)i_this)->_draw();
}

/* 022E74E0 */
static BOOL daNpc_PeopleIsDelete(void*) {
    return TRUE;
}

// HD: compiler-generated code of this translation unit.
/* 022E7438 */
static void __sinit_d_a_npc_sv_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 022E74CC */
// HD: sead::SafeString deleting destructor (this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) operator_delete(p);
}

/* 022E74E8 */
// HD: daNpcSv_c deleting destructor (compiler-generated, HD virtual destructor)
static void daNpcSv_dt(daNpcSv_c* p, s32 flags) {
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        Sv_circleDt(&p->mAcchCir.m_cir, 2); /* ~cM3dGCir */
        Sv_view<SvAcchDestructor>(&p->mObjAcch)->mActorCheckVtable = gc_tu.objAcchDt[0];            /* ~dBgS_ObjAcch */
        Sv_view<SvAcchDestructor>(&p->mObjAcch)->mCheckVtable = gc_tu.objAcchDt[1];
        Sv_acchDt(&p->mObjAcch, 0);
        Sv_actorDt(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}

/* 022E7584 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue
str 100224E0 "head"
str 100224E8 "d_a_npc_sv.cpp" in 022E4DF8
str 100224F8 "m_jnt.getHeadJntNum() >= 0"
str 10022514 "backbone"
str 10022520 "m_jnt.getBackboneJntNum() >= 0"
str 10022560 "SV_TALK_P1_1ST"
str 10022570 "SV_TALK_P1_2ND"
str 10022580 "SV_TALK_P4_1ST"
str 10022594 "MsgNo"
str 1002259C "ItemNo"
str 100225A4 "Timer"
str 100225AC "d_a_npc_sv.cpp" in 022E6394
str 100225BC "a_ptn < (sizeof(l_npc_anm_sv1_tbl) / sizeof(l_npc_anm_sv1_tbl[0]))"
str 10022600 "a_ptn < (sizeof(l_npc_se_sv1_tbl) / sizeof(l_npc_se_sv1_tbl[0]))"
data l_staff_name_tbl 101C6404
data l_arcname_tbl 101C6414
data l_bdl_ix_tbl 10022468
data l_bck_ix_tbl 10022478
data l_npc_anm_sv1_tbl 101C6424
data l_npc_se_sv1_tbl 101C643C
data l_msg_anm_tbl 10022678
data l_msg_tbl 101C64CC
data l_msg_flag_tbl 101C64FC
data l_talk_ev_tbl 101C6514
data l_method 101C6778
data l_cut_name_tbl 101C6784
data l_anm_wait 101C63F0
data l_anm_end 101C63F3
data l_anm_ship 101C63FC
data l_npc_dat 101C6668
data l_init_tbl 101C652C
data l_exec_tbl 101C653C
ext 025A1458 void Sv_npcCtor(void*)
ext 02525FE4 cPhs_State Sv_phaseHandler(request_of_phase_process_class*, u32, void*)
ext 025D7DEC s32 Sv_createPresent(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 025F74D0 void Sv_setMessageStatus(u32, u32)
ext 025F795C u32 Sv_messageStatus(u32)
ext 025F7DB0 u32 Sv_messageSet(u32, u32, cXyz*)
ext 02018034 void Sv_circleDt(void*, s32)
ext 024EFD9C void Sv_acchDt(void*, s32)
ext 025D50BC void Sv_actorDt(void*, s32)
ext 0259DED0 void Sv_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 02543280 void Sv_cutEnd(dEvent_manager_c*, s32)
ext 025447C8 BOOL Sv_advance(dEvent_manager_c*, s32)
ext 025D9F38 fopAc_ac_c* Sv_searchFromName(const char*, u32, u32)
ext 025E1988 void Sv_soundStart(u32)
ext 025F19F8 void Sv_xyzRotate(Mtx34*, s16, s16, s16)
ext 025D54C4 BOOL Sv_searchByID(fpc_ProcID, be<u32>*)
*/
