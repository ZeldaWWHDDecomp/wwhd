/**
 * d_a_npc_ah.cpp (WWHD)
 * NPC - Old Man Ho Ho
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ah.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ah.h"

// HD: compiler-generated copies owned by this translation unit.
const gc_tu_data gc_tu = {.safeString = 0x10015EC4, .safeStringAssure = 0x021E6F50,
    .objAcchDt = {0x10015EDC, 0x10015EEC}, .headerStatics = {0x104659B8, 0x101BB3E0}};
enum { AH_VTBL = 0x10015FE4, AH_PHASE_STOP = 3, AH_MSG_DISPLAYED = 0xE, AH_MSG_DESTROYED = 0x13, AH_SNAPSHOT_TYPE = 0x7F };
static const char* l_arcname_tbl[] = {"Ah"};
static const char* l_npc_staff_id[] = {"Ah"};
static s32 l_bmd_ix_tbl[] = {2};
static s32 l_bck_ix_tbl[] = {0, 1};
static sAhAnmDat l_npc_anm_wait = {0, 8, -1};
static sAhAnmDat l_npc_anm_wait2 = {1, 8, -1};
static NpcDatStruct l_npc_dat = {
    4096, 8192, 0, 4096, -4096, -8192, 0, -4096, 2048, 0,
    0.0f, 0.0f, 190.0f, -20.0f, 150.0f, 200.0f, 0.0f,
    32767, 2048, 400, 0, 60.0f, 0.5f, 3.0f,
    6, 1024, 60, 90, 200, 300, 30, 0, 0
};
// HD: each checked entry points to its two-word message sequence.
static u32 l_msg_ah_tbl[] = {
    0x101BB2B0, 0x101BB2B8, 0x101BB2C0, 0x101BB2C8, 0x101BB2D0,
    0x101BB2D8, 0x101BB2E0, 0x101BB2E8, 0x101BB2F0, 0x101BB2F8
};
struct AhInitEntry { using Function = BOOL (daNpcAh_c::*)(); gabi::ptmf<Function> mFunction; };
struct AhMoveEntry { using Function = void (daNpcAh_c::*)(); gabi::ptmf<Function> mFunction; };
static AhInitEntry l_execute_init[] = {{&daNpcAh_c::executeWaitInit},{&daNpcAh_c::executeTalkInit}};
static AhMoveEntry moveProc[] = {{&daNpcAh_c::executeWait},{&daNpcAh_c::executeTalk}};
static u32 l_method[] = {0x021E549C,0x021E5AD8,0};
static u32 cut_name_tbl[] = {0x10015FC0,0x10015FC8};
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
// Preserve the HD word-copy order, including NaN payloads.
static void wcopy(void* dst, const void* src) { Ah_word(dst) = Ah_word(const_cast<void*>(src)); }
static void wcopy3(cXyz* dst, const cXyz* src) {
    wcopy(&dst->x, &src->x); wcopy(&dst->y, &src->y); wcopy(&dst->z, &src->z);
}

static u32 daObj_PrmAbstract(fopAc_ac_c*, s32, s32);

/* 021E5398 */
static daNpcAh_c* Ah_actorCtor(daNpcAh_c* p) {
    if (p == nullptr) {
        p = (daNpcAh_c*)operator_new(0x8C8);
        if (p == nullptr) return nullptr;
    }
    Ah_npcCtor(p);
    p->__vtbl = AH_VTBL;
    mDoExt_btpAnm::ct(&p->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    p->mBckIdx = 0;
    p->field_0x747 = 0; /* setResFlag(0) */
    p->field_0x71C = -1.0f;
    p->field_0x736 = 0;
    p->field_0x732 = p->home.angle.y;
    p->field_0x718 = 0.0f;
    p->mHeadOnlyFollow = 1;
    p->mMoveState = 0;
    p->field_0x74E = 0;
    p->field_0x72C = 0;
    return p;
}

/* 021E4E8C */
static BOOL da_Npc_Ah_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        daNpcAh_c* i_this = static_cast<daNpcAh_c*>(model->getUserArea());
        u32 jointNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(jointNo), calc_mtx());
        if (jointNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        } else if (jointNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        mtx_copy(model->getAnmMtx(jointNo), calc_mtx()); /* MTXCopy(*calc_mtx, model->getAnmMtx(jointNo)) */
        PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}

/* 021E5394 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpcAh_c*)i_this)->createHeap();
}

/* 021E549C */
static cPhs_State phase_1(daNpcAh_c* i_this) {
    /* fopAcM_ct(i_this, daNpcAh_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) Ah_actorCtor(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    switch (i_this->getPrmArg0()) {
    case 2:
        if (dComIfGs_isSwitch(0x6C, i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return AH_PHASE_STOP;
        }
        if (!dComIfGs_isEventBit(0x0520 /* UNK_0520 */)) {
            return AH_PHASE_STOP;
        }
        break;
    case 5:
        if (dComIfGs_isSwitch(0x10, i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return AH_PHASE_STOP;
        }
        break;
    case 9:
        if (dComIfGs_isSwitch(i_this->getSwBit(), i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return AH_PHASE_STOP;
        }
        break;
    }
    i_this->field_0x747 = 1; /* setResFlag(1) */
    return cPhs_NEXT_e;
}

/* 021E5AD8 */
static cPhs_State phase_2(daNpcAh_c* i_this) {
    cPhs_State state = dComIfG_resLoad(&i_this->mPhs, l_arcname_tbl[0]);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)CheckCreateHeap), 0x2F00)) {
            state = i_this->createInit();
        } else {
            i_this->mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}

/* 021E5B60 */
cPhs_State daNpcAh_c::_create() {
    return Ah_phaseHandler(&mPhsMethod, gabi::ea(l_method), this);
}

/* 021E50E8 */
BOOL daNpcAh_c::createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_bmd_ix_tbl[0]);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_bck_ix_tbl[mBckIdx]);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) return FALSE;
    m_jnt.mHeadJntNum = (s8)modelData->getJointName()->getIndex( "head" /* "head" */);
    if (m_jnt.mHeadJntNum < 0) /* JUT_ASSERT(0x2BC, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_ah.cpp", 0x2BC, "m_jnt.getHeadJntNum() >= 0");
    m_jnt.mBackboneJntNum = (s8)modelData->getJointName()->getIndex( "backbone" /* "backbone" */);
    if (m_jnt.mBackboneJntNum < 0) /* JUT_ASSERT(0x2C0, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_ah.cpp", 0x2C0, "m_jnt.getBackboneJntNum() >= 0");
    if (initTexPatternAnm(false) == FALSE) return FALSE;
    for (u16 jntIdx = 0; jntIdx < modelData->getJointNum(); jntIdx++) {
        if (jntIdx == m_jnt.mHeadJntNum || jntIdx == m_jnt.mBackboneJntNum) {
            modelData->getJointNodePointer(jntIdx)->setCallBack(da_Npc_Ah_nodeCallBack);
        }
    }
    mpMorf->mpModel->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}

/* 021E5920 */
cPhs_State daNpcAh_c::createInit() {
    gravity = -9.0f;
    setAnmTbl(&l_npc_anm_wait);
    mEventCut.setActorInfo2(l_npc_staff_id[0], this);
    mLookAtMaxVel = 0;
    field_0x743 = 0;
    field_0x742 = 0;
    field_0x751 = 0;
    cullMtx = gabi::ea(mpMorf->getModel()->getBaseTRMtx()); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA7;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_UNK1000000_e; /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK | UNK1000000 */
    NpcDatStruct* dat = &l_npc_dat;
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y, dat->mMax_head_x,
                   dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    field_0x74F = (u8)dat->field_0x52;
    field_0x750 = (u8)dat->field_0x53;
    wcopy(&field_0x720, &dat->field_0x28);
    field_0x730 = dat->field_0x30;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    // HD: matrix and collision setup follow the post-demo GameCube version.
    setMtx();
    mpMorf->getModel()->calc();
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    wcopy3(center, &current.pos);
    setCollision(&mCyl, center, dat->field_0x38, 150.0f);
    return cPhs_COMPLEATE_e;
}

/* 021E5B78 */
BOOL daNpcAh_c::_delete() {
    if (field_0x747 != 0) {
        dComIfG_resDelete(&mPhs, l_arcname_tbl[0]); // HD: dComIfG_resDelete (GameCube dComIfG_resDeleteDemo)
    }
    if (heap != nullptr && mpMorf != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 021E6B14 */
BOOL daNpcAh_c::_draw() {
    J3DModel* morfModel = mpMorf->getModel();
    J3DModelData* morfModelData = morfModel->getModelData();
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morfModel, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, morfModelData, mBtpFrame);
    mpMorf->updateDL();
    Ah_view<AhModelDataAnimation>(morfModelData)->mTexPatternAnimation = 0; /* mBtpAnm.remove(morfModelData) */
    // HD: no dComIfGd_setShadow; the renderer supplies shadows.
    dSnap_RegistFig(AH_SNAPSHOT_TYPE, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 021E688C */
BOOL daNpcAh_c::_execute() {
    switch (getPrmArg0()) {
    case 2:
        if (dComIfGs_isSwitch(0x6C, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    case 5:
        if (dComIfGs_isSwitch(0x10, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    case 9:
        if (dComIfGs_isSwitch(getSwBit() & 0xFF, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    }
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || eventInfo.mCommand == dEvtCmd_INTALK_e) {
        (this->*moveProc[mMoveState].mFunction)();
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    NpcDatStruct* dat = &l_npc_dat;
    gabi::Local<cXyz> center;
    wcopy3(center, &current.pos);
    setCollision(&mCyl, center, dat->field_0x38, 150.0f);

    gabi::Local<cXyz> attnInfoPos;
    wcopy(&attnInfoPos->z, &dat->field_0x20);
    wcopy(&attnInfoPos->x, &dat->field_0x18);
    wcopy(&attnInfoPos->y, &dat->field_0x1C);
    cMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), attnInfoPos, attnInfoPos);
    PSVECAdd(attnInfoPos, &current.pos, attnInfoPos); /* attnInfoPos += current.pos */
    cXyz* attnPos = &attention_info.position; /* attention_info.position */
    wcopy3(attnPos, attnInfoPos);
    f32 eyeOffs = dat->field_0x24;
    f32 y = current.pos.y;
    wcopy(&eyePos.z, &current.pos.z);
    wcopy(&eyePos.x, &current.pos.x);
    eyePos.y = gabi::fadds_ppc(y, eyeOffs);
    lookBack();
    setMtx();
    return FALSE;
}

/* 021E6BC0 */
u8 daNpcAh_c::executeCommon() {
    field_0x744 = field_0x743 != 0 ? 1 : 0;
    if (field_0x742 == 1 && mMoveState != 1) executeSetMode(1);
    return field_0x742;
}

/* 021E5EB0 */
void daNpcAh_c::executeSetMode(u8 proc) {
    field_0x718 = 0.0f;
    mMoveState = (u8)(this->*l_execute_init[proc].mFunction)();
}

/* 021E6C20 */
BOOL daNpcAh_c::executeWaitInit() {
    speedF = 0.0f;
    setAnmTbl(&l_npc_anm_wait);
    NpcDatStruct* dat = &l_npc_dat;
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y, dat->mMax_head_x,
                   dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    return FALSE;
}

/* 021E6CA0 */
void daNpcAh_c::executeWait() {
    executeCommon();
}

/* 021E6CA4 */
BOOL daNpcAh_c::executeTalkInit() {
    return TRUE;
}

/* 021E6CAC */
void daNpcAh_c::executeTalk() {
    executeCommon();
    if (talk2(1) == fopMsgStts_BOX_CLOSED_e) {
        field_0x742 = 0;
        executeSetMode(0);
        dComIfGp_event_reset();
    } else {
        setAnmFromMsgTag();
    }
}

/* 021E5F38 */
void daNpcAh_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        return;
    }
    if (cmd == dEvtCmd_INTALK_e && (field_0x744 == 2 || field_0x744 == 1)) {
        field_0x742 = 1;
        executeSetMode(1);
    }
}

/* 021E64E0 */
void daNpcAh_c::eventOrder() {
    if (field_0x744 == 2 || field_0x744 == 1) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
        if (field_0x744 == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 021E6464 */
void daNpcAh_c::eventMove() {
    /* chkEndEvent() is FALSE (inlined) */
    u8 attnFlag = mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (mEventCut.mbAttention == 0) {
            mEventCut.mbAttention = attnFlag;
        }
    } else {
        privateCut();
        setAnmFromMsgTag();
    }
}

/* 021E62C8 */
void daNpcAh_c::privateCut() {
    const char* name = l_npc_staff_id[0];
    s32 staff_idx = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    if (staff_idx != -1) {
        s8 act = (s8)dComIfGp_evmng_getMyActIdx(staff_idx, reinterpret_cast<gptr<char>*>(cut_name_tbl), 2, 1, 0);
        mActIdx = act;
        dEvent_manager_c* evtMng = dComIfGp_getPEvtManager();
        if (act == -1) {
            Ah_cutEnd(evtMng, staff_idx);
        } else {
            if (Ah_advance(evtMng, staff_idx) /* getIsAddvance */) {
                switch ((u32)(s32)mActIdx) {
                case 0:
                    eventMesSetInit(staff_idx);
                    break;
                case 1:
                    eventGetItemInit();
                    break;
                }
            }
            bool end;
            switch (mActIdx) {
            case 0:
                end = eventMesSet();
                break;
            default:
                end = true;
                break;
            }
            if (end) {
                dComIfGp_evmng_cutEnd(staff_idx);
            }
        }
    }
}

/* 021E5F74 */
void daNpcAh_c::eventMesSetInit(int staffIdx) {
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "MsgNo" /* "MsgNo" */, 3 /* integer */);
    if (pData != nullptr) {
        mpMsgNo = 0;
        u32 msg = *pData;
        switch (msg) {
        case 1:
            return;
        case 0:
            msg = Ah_getMessage(this);
            setMessage(msg);
            break;
        default:
            setMessage(msg);
            break;
        }
        if (mpMsgNo != 0) {
            setMessage(*Ah_messagePointer(mpMsgNo));
        }
    } else {
        mpMsgNo = mpMsgNo + 4;
        setMessage(*Ah_messagePointer(mpMsgNo));
    }
}

/* 021E623C */
BOOL daNpcAh_c::eventMesSet() {
    u16 talkVal = talk2(0);
    if (talkVal == fopMsgStts_BOX_CLOSED_e) {
        u8 f = field_0x748;
        if ((f & 1) != 0) {
            field_0x748 = f & ~1;
            mItemNo = 7;
            field_0x744 = 3;
        } else if ((f & 2) != 0) {
            field_0x748 = f & ~2;
            mItemNo = 5;
            field_0x744 = 3;
        }
    }
    return talkVal == fopMsgStts_BOX_CLOSED_e;
}

/* 021E6030 */
void daNpcAh_c::eventGetItemInit() {
    s32 procItem = Ah_createPresent(&current.pos, mItemNo, 0, -1, -1, nullptr, nullptr);
    if (procItem != -1 /* fpcM_ERROR_PROCESS_ID_e */) {
        dComIfGp_event_setItemPartnerId(procItem);
    }
}

/* 021E6088 */
u16 daNpcAh_c::talk2(int i_param) {
    u32 mng = gabi::ea(fopNpc_npc_c::mpCurrMsg.get());
    u16 msg_status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (i_param == 1) mCurrMsgNo = Ah_getMessage(this);
        mCurrMsgBsPcId = Ah_messageSet(mng, mCurrMsgNo, &eyePos);
        // HD: the message is reached through the manager; the flag replaces mpCurrMsg
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
            field_0x734 = 0xFFFF;
        }
    } else if (mbHasMsg != 0) {
        msg_status = (u16)Ah_messageStatus(mng);
        switch (msg_status) {
        case AH_MSG_DISPLAYED:
            Ah_setMessageStatus(mng, Ah_nextMessage(this, &mCurrMsgNo));
            if (Ah_messageStatus(mng) == fopMsgStts_MSG_CONTINUES_e) {
                Ah_messageSet(mng, mCurrMsgNo, nullptr);
            }
            break;
        // HD: no fopMsgStts_MSG_TYPING_e case (GameCube chkMsg, empty)
        case fopMsgStts_BOX_CLOSED_e:
            Ah_setMessageStatus(mng, AH_MSG_DESTROYED);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        field_0x734 = msg_status;
        Ah_animationAttribute(this, msg_status);
    } else {
        mbHasMsg = 1; /* mpCurrMsg = Ah_messageStatus(mCurrMsgBsPcId) */
    }
    return msg_status;
}

/* 021E6D24 */
u16 daNpcAh_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 ret = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    if (mpMsgNo != 0) {
        mpMsgNo = mpMsgNo + 4;
        u32 next = *Ah_messagePointer(mpMsgNo);
        switch (next) {
        case 0:
            mpMsgNo = 0;
            ret = fopMsgStts_MSG_ENDS_e; /* fopNpc_npc_c::next_msgStatus(pMsgNo) inlined: fopMsgStts_MSG_ENDS_e */
            break;
        default:
            *pMsgNo = next;
            break;
        }
    } else {
        ret = fopMsgStts_MSG_ENDS_e; /* fopNpc_npc_c::next_msgStatus(pMsgNo) inlined */
    }
    return ret;
}

/* 021E6D6C */
u32 daNpcAh_c::getMsg() {
    u32 ret = 0;
    mpMsgNo = 0;
    if (!(dComIfGp_getEvent()->mbInPhoto != 0)) {
        if (!dComIfGp_event_chkTalkXY()) {
            u8 index = getPrmArg0();
            mpMsgNo = l_msg_ah_tbl[index];
        }
    }
    if (mpMsgNo != 0) ret = *Ah_messagePointer(mpMsgNo);
    return ret;
}

/* 021E5F6C */
void daNpcAh_c::setMessage(u32 msgNo) {
    mCurrMsgNo = msgNo;
}

/* 021E63E8 */
void daNpcAh_c::setAnmFromMsgTag() {
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 0:
        setAnmTbl(&l_npc_anm_wait);
        break;
    case 1:
        setAnmTbl(&l_npc_anm_wait2);
        break;
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}

/* 021E5438 */
u8 daNpcAh_c::getPrmArg0() {
    u8 ret = (u8)daObj_PrmAbstract(this, PRM_SWSAVE_S, PRM_SWSAVE_W);
    if (ret >= 10) ret = 0;
    return ret;
}

/* 021E5470 */
u8 daNpcAh_c::getSwBit() {
    return (u8)daObj_PrmAbstract(this, PRM_SWSAVE2_S, PRM_SWSAVE2_W);
}

/* 021E57B0 */
void daNpcAh_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 021E5BE0 */
void daNpcAh_c::chkAttention() {
    NpcDatStruct* dat = &l_npc_dat;
    field_0x751 = 0;
    if (mEventCut.mbAttention != 0) { /* mEventCut.getAttnFlag() */
        wcopy3(&mEyePos, &mEventCut.mPos);
        field_0x74E = 1;
        if (field_0x74F != 0) {
            mHeadOnlyFollow = 0;
            m_jnt.mbTrn = 1;
        } else {
            mHeadOnlyFollow = 1;
        }
        if (field_0x743 == 0) {
            field_0x743 = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> pos;
        wcopy3(pos, &current.pos);
        gabi::Local<cXyz> plPos;
        wcopy3(plPos, &player->current.pos);
        f32 temp720 = field_0x720;
        s32 temp730 = field_0x730;
        gabi::Local<be<f32>> distXZ;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos, plPos, distXZ, angle);
        u8 attn = field_0x743;
        if (attn != 0) {
            temp720 = temp720 + 40.0f;
            temp730 += 0x71C;
        }
        s16 ang = (s16)(*angle - shape_angle.y);
        *angle = ang;
        f32 dist = *distXZ;
        s32 absAng = ang < 0 ? -ang : ang;
        if (temp720 > dist && temp730 > absAng) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos(eye, dat->field_0x14);
            wcopy3(&mEyePos, eye);
            mHeadOnlyFollow = field_0x74F == 0;
            field_0x74E = 1;
            if (field_0x750 == 0) {
                mTargetYRot = field_0x732;
                mHeadOnlyFollow = 0;
                field_0x74E = 2;
                m_jnt.mbTrn = 1;
            }
            if (field_0x743 == 0) {
                field_0x743 = 1;
            }
        } else {
            if (attn == 1) {
                field_0x743 = 0;
                field_0x72E = dat->field_0x50;
            }
            if (dat->field_0x2C > dist) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos(eye, dat->field_0x14);
                wcopy3(&mEyePos, eye);
                mHeadOnlyFollow = field_0x74F == 0;
                field_0x74E = 1;
                if (field_0x750 == 0) {
                    mTargetYRot = field_0x732;
                    mHeadOnlyFollow = 0;
                    field_0x74E = 2;
                    m_jnt.mbTrn = 1;
                }
                field_0x751 = 1;
            } else {
                u32 path = gabi::ea(mPathRun.mPath.get());
                field_0x74E = 0;
                if (path == 0) { /* !mPathRun.isPath() */
                    if (field_0x72E != 0) {
                        field_0x72E = field_0x72E - 1;
                    } else {
                        mTargetYRot = field_0x732;
                        mHeadOnlyFollow = 0;
                        field_0x74E = 2;
                        m_jnt.mbTrn = 1;
                    }
                }
            }
        }
    }
    mTargetAngle = dat->field_0x32;
}

/* 021E6680 */
void daNpcAh_c::lookBack() {
    s8 mode = field_0x74E;
    u32 eyeX = Ah_word(&eyePos.x); /* cXyz eyePosFollow = eyePos */
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = mHeadOnlyFollow;
    s16 target = mTargetAngle;
    u32 eyeZ = Ah_word(&eyePos.z);
    cXyz* dstPos = nullptr;
    u32 eyeY = Ah_word(&eyePos.y);
    gabi::Local<cXyz> newDes;
    switch ((u32)(s32)mode) {
    case 1:
        wcopy3(newDes, &mEyePos);
        dstPos = newDes;
        break;
    case 2:
        desiredYRot = mTargetYRot;
        break;
    }
    bool turn;
    if (field_0x742 != 0 && field_0x74F != 0) {
        headOnlyFollow = 0;
        m_jnt.mbTrn = 1;
        turn = true;
    } else {
        turn = m_jnt.mbTrn != 0; /* m_jnt.trnChk() */
    }
    gabi::Local<cXyz> eye;
    if (turn) {
        s16 turnSpeed = mEventCut.mTurnSpeed;
        if (turnSpeed != 0) target = turnSpeed;
        cLib_addCalcAngleS2(&mLookAtMaxVel, target, 4, 0x800);
        s16 vel = mLookAtMaxVel;
        Ah_word(&eye->x) = eyeX;
        Ah_word(&eye->y) = eyeY;
        Ah_word(&eye->z) = eyeZ;
        Ah_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, vel, headOnlyFollow);
    } else {
        mLookAtMaxVel = 0;
        Ah_word(&eye->x) = eyeX;
        Ah_word(&eye->y) = eyeY;
        Ah_word(&eye->z) = eyeZ;
        Ah_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, 0, headOnlyFollow);
    }
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
}

/* 021E4FE8 */
BOOL daNpcAh_c::initTexPatternAnm(u32 modify) {
    u32 modelData = gabi::ea(mpMorf->getModel()->getModelData());
    // HD: the first texture-pattern resource index is immediate.
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(l_arcname_tbl[0], dRes_ID_AH_BTP_MABA_e);
    if (m_head_tex_pattern == nullptr) /* JUT_ASSERT(0x67D, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_ah.cpp", 0x67D, "m_head_tex_pattern != (0)");
    BOOL ret = Ah_initBtp(&mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, modify, FALSE);
    if (ret == FALSE) return FALSE;
    mBtpFrame = 0;
    mTimer = 0;
    return TRUE;
}

/* 021E6510 */
void daNpcAh_c::playTexPatternAnm() {
    if (Ah_calcTimer(&mTimer) == 0) {
        s32 frameMax = Ah_frameMax(m_head_tex_pattern);
        if ((s32)mBtpFrame >= frameMax) {
            s32 frameMax2 = Ah_frameMax(m_head_tex_pattern);
            mTimer = 0x78;
            mBtpFrame = (u8)(mBtpFrame - frameMax2);
        } else {
            mBtpFrame = mBtpFrame + 1;
        }
    }
}

/* 021E65B0 */
void daNpcAh_c::playAnm() {
    mDoExt_McaMorf* morf = mpMorf;
    field_0x74A = field_0x74A & ~1;
    if (morf->play(nullptr, 0, 0)) {
        if (mpAnmDat != nullptr) {
            s8 n = field_0x74B;
            if (n > 0) {
                n = (s8)(n - 1);
                field_0x74B = n;
                if (n == 0) {
                    sAhAnmDat* next = mpAnmDat.get() + 1;
                    mpAnmDat = next;
                    if (setAnmTbl(next)) {
                        field_0x74A = field_0x74A | 1;
                    }
                } else {
                    setAnm(mpAnmDat->mBckIdx, J3DFrameCtrl::EMode_NONE, 0.0f);
                }
            }
        }
    }
}

/* 021E55D8 */
void daNpcAh_c::setAnm(u8 bck_ix, int loopMode, f32 morf) {
    f32 tempMorf = field_0x71C;
    if (!(tempMorf < 0.0f)) {
        morf = tempMorf;
        field_0x71C = -1.0f;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_bck_ix_tbl[bck_ix]);
    mpMorf->setAnm(anm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mBckIdx = bck_ix;
}

/* 021E56E4 */
BOOL daNpcAh_c::setAnmTbl(sAhAnmDat* i_anmDat) {
    if (i_anmDat->mBckIdx == 0xFF) {
        mpAnmDat = nullptr;
        return TRUE;
    }
    mpAnmDat = i_anmDat;
    s8 n = i_anmDat->field_0x02;
    field_0x74B = n;
    u8 bck = i_anmDat->mBckIdx;
    if (n > 0) {
        setAnm(bck, J3DFrameCtrl::EMode_NONE, (f32)i_anmDat->mMorf);
    } else if (mBckIdx != bck) {
        setAnm(bck, J3DFrameCtrl::EMode_LOOP, (f32)i_anmDat->mMorf);
    }
    return FALSE;
}

/* 021E5890 */
void daNpcAh_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}

/* 021E5B74 */
static cPhs_State daNpc_AhCreate(void* i_this) {
    return ((daNpcAh_c*)i_this)->_create();
}

/* 021E5BDC */
static BOOL daNpc_AhDelete(void* i_this) {
    return ((daNpcAh_c*)i_this)->_delete();
}

/* 021E6B10 */
static BOOL daNpc_AhExecute(void* i_this) {
    return ((daNpcAh_c*)i_this)->_execute();
}

/* 021E6BBC */
static BOOL daNpc_AhDraw(void* i_this) {
    return ((daNpcAh_c*)i_this)->_draw();
}

/* 021E6EAC */
static BOOL daNpc_AhIsDelete(void*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 021E6E04 */
static void __sinit_d_a_npc_ah_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 021E6E98 */
// HD: sead::SafeString deleting destructor (this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) operator_delete(p);
}

/* 021E6EB4 */
// HD: daNpcAh_c deleting destructor (compiler-generated, HD virtual destructor)
static void daNpcAh_dt(daNpcAh_c* p, s32 flags) {
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        Ah_circleDt(&p->mAcchCir.m_cir, 2); /* ~cM3dGCir */
        Ah_view<AhAcchDestructor>(&p->mObjAcch)->mActorCheckVtable = gc_tu.objAcchDt[0];            /* ~dBgS_ObjAcch */
        Ah_view<AhAcchDestructor>(&p->mObjAcch)->mCheckVtable = gc_tu.objAcchDt[1];
        Ah_acchDt(&p->mObjAcch, 0);
        Ah_actorDt(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}

/* 021E6F50 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* 021E6F54 */
static u32 daObj_PrmAbstract(fopAc_ac_c* ac, s32 width, s32 shift) {
    u32 prm = ac->mParameters;
    /* PowerPC slw/srw: shift amounts of 32..63 give 0 */
    u32 bit = (width & 0x20) ? 0 : (1u << (width & 0x1F));
    u32 v = (shift & 0x20) ? 0 : (prm >> (shift & 0x1F));
    return v & (bit - 1);
}

/* WWHD glue
str 10015F00 "m_head_tex_pattern != (0)"
str 10015F1C "d_a_npc_ah.cpp" in 021E4FE8
str 10015F30 "head"
str 10015F38 "d_a_npc_ah.cpp" in 021E50E8
str 10015F48 "m_jnt.getHeadJntNum() >= 0"
str 10015F64 "backbone"
str 10015F70 "m_jnt.getBackboneJntNum() >= 0"
str 10015FB8 "MsgNo"
data l_arcname_tbl 101BB304
data l_npc_staff_id 101BB300
data l_bmd_ix_tbl 10015EBC
data l_bck_ix_tbl 10015EB0
data l_npc_anm_wait 101BB308
data l_npc_anm_wait2 101BB30B
data l_npc_dat 101BB378
data l_msg_ah_tbl 101BB310
data l_execute_init 101BB338
data moveProc 101BB348
data l_method 101BB3CC
data cut_name_tbl 101BB3D8
ext 0259DED0 void Ah_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 02543280 void Ah_cutEnd(dEvent_manager_c*, s32)
ext 025447C8 BOOL Ah_advance(dEvent_manager_c*, s32)
ext 02055B64 s16 Ah_calcTimer(be<s16>*)
ext 025A1458 void Ah_npcCtor(void*)
ext 02525FE4 cPhs_State Ah_phaseHandler(request_of_phase_process_class*, u32, void*)
ext 025D7DEC s32 Ah_createPresent(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 025F74D0 void Ah_setMessageStatus(u32, u32)
ext 025E789C BOOL Ah_initBtp(void*, u32, J3DAnmTexPattern*, s32, s32, f32, s16, s16, s32, s32)
ext 025F795C u32 Ah_messageStatus(u32)
ext 025F7DB0 u32 Ah_messageSet(u32, u32, cXyz*)
ext 02018034 void Ah_circleDt(void*, s32)
ext 024EFD9C void Ah_acchDt(void*, s32)
ext 025D50BC void Ah_actorDt(void*, s32)
*/
