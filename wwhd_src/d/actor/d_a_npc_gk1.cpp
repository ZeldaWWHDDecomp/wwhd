/**
 * d_a_npc_gk1.cpp (WWHD)
 * NPC - Mila's father (poor)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_gk1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_gk1.h"

// HD: per-translation-unit constructor/destructor metadata.
const gc_tu_data gc_tu = {
    .safeString = 0x10019F70,
    .safeStringAssure = 0x0223845C,
    .objAcchDt = {0x10019F88, 0x10019F98},
    .headerStatics = {0x10467044, 0x101BE488, 0x10467078},
};
using namespace gk1_cpp;
static s32 l_btpResID[1] = {8};
static s32 l_bckResID[5] = {3, 4, 0, 1, 2};
static s32 a_heap_size_tbl[2] = {0, 0};
static daNpc_Gk1_c::anm_prm_c a_anm_prm_tbl[3] = {
    {-1, -1, {}, 0.0f, 0.0f, -1}, {0, 0, {}, 8.0f, 1.0f, 2}, {-1, -1, {}, 0.0f, 0.0f, -1}};
static daNpc_Gk1_c::anm_prm_c a_anm_atr_tbl[5] = {{0, 0, {}, 8.0f, 1.0f, 2},
                                                  {1, 0, {}, 8.0f, 1.0f, 2},
                                                  {2, 0, {}, 8.0f, 1.0f, 2},
                                                  {3, 0, {}, 8.0f, 1.0f, 2},
                                                  {4, 0, {}, 8.0f, 1.0f, 2}};
static char* l_evn_tbl[1] = {"dummy"};
static char* a_cut_tbl[1] = {"dummy"};
static char l_archiveName[3] = {71, 107, 0};
static daNpc_Gk1_HIO_c::hio_prm_c a_prm_tbl;
static u32 l_red = 4278190208;
static u32 l_blue = 65408;
static u32 l_yellow = 4294901888;
static u32 l_redCopy;
static u32 l_blueCopy;
static u32 l_yellowCopy;
static u32 l_redGuard;
static u32 l_blueGuard;
static u32 l_yellowGuard;
static daNpc_Gk1_HIO_c l_HIO;
static ProcFunc_l l_waitAction;
static cXyz a_eye_pos_off;
static u32 l_eyeGuard;
static f32 l_cullDistance;
static msg_class* l_message;
static void* l_demoObject;
static CylinderSource dNpc_cyl_src;

/* 022357D4 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Gk1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Head(i_node, model);
    }
    return TRUE;
}

/* 02235684 */
void daNpc_Gk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* static cXyz a_eye_pos_off(30.0f, 30.0f, 0.0f): guard 0x10467094, object 0x10467088 */

    if (l_eyeGuard == 0) {
        a_eye_pos_off.x = 30.0f;
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.y = 30.0f;
        l_eyeGuard = 1;
    }
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, &a_eye_pos_off, &mEyePos);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02235940 */
static BOOL nodeCB_Neck(J3DNode* i_node, int i_calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Gk1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Neck(i_node, model);
    }
    return TRUE;
}

/* 0223581C */
void daNpc_Gk1_c::_nodeCB_Neck(J3DNode* i_node, J3DModel* i_model) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02235AAC */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Gk1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_BackBone(i_node, model);
    }
    return TRUE;
}

/* 02235988 */
void daNpc_Gk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02236138 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return static_cast<daNpc_Gk1_c*>(i_this)->CreateHeap();
}

/* 022362CC */
bool daNpc_Gk1_c::init_GK1_0() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGs_isEventBit(0x2D01)) {
        gabi::Local<ProcFunc_l> pmf;
        copyAction(pmf, &l_waitAction);
        set_action(pmf, nullptr);
        return true;
    }
    return false;
}

/* 02236748 */
bool daNpc_Gk1_c::createInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* l_evn_tbl (.data 0x101BE3BC): "dummy" */
    const char* name = entry(l_evn_tbl, 0);
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2("Gk1" /* "Gk1" */, (fopNpc_npc_c*)(void*)this);
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags */
    cullSizeFar = 12000.0f / l_cullDistance;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    gravity = -4.5f;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9; /* attention_info.distances[TALK] */
    mBckNum = 5;
    if (mType != 0) {
        return false;
    }
    if (!init_GK1_0()) {
        return false;
    }
    mAngle.z = current.angle.z;
    mAngle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    mAngle.y = current.angle.y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl*>(&dNpc_cyl_src) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = gkRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = gkPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 022363F0 */
void daNpc_Gk1_c::play_animation() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = gkMaterialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 022364E8 */
void daNpc_Gk1_c::setMtx(u32 i_setEyePos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(mpItemModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpItemModel);
    }
    if (mpHatModel.get()) {
        PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(mpHatModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpHatModel);
    }
    setAttention(i_setEyePos);
}

/* 022376A0 */
int daNpc_Gk1_c::bckResID(int i_bckNum) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return entry(l_bckResID, i_bckNum);
}

/* 02235AF4 */
int daNpc_Gk1_c::btpResID(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return entry(l_btpResID, 0);
}

/* 02235B00 */
u32 daNpc_Gk1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(454, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x1C6, "a_btp != (0)");
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return gkBtpInit(&mBtpAnm, J3DModel_getModelData(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify,
                     0) != 0;
}

/* 02235BEC */
u32 daNpc_Gk1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return setBtp(i_btpNum, i_bModify);
}

/* 02236350 */
void daNpc_Gk1_c::play_btp_anm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 frame_max = (u8)getFrameMax(mBtpAnm.mpAnm.get());
    if (mBtpNum != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= frame_max) {
            if (mBtpNum == 0) {
                mBlinkTimer = (s16)gkRandom(60, 90);
                frame_max = 0;
            }
            mBtpFrame = frame_max;
        }
    }
}

/* 022376B4 */
void daNpc_Gk1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    mAnimationWrapped = 0;
}

/* 0223774C */
void daNpc_Gk1_c::setAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    init_texPttrnAnm(entry(a_anm_prm_tbl, mStt).btpNum, true);
    setAnm_anm(&entry(a_anm_prm_tbl, mStt));
}

/* 02237824 */
void daNpc_Gk1_c::chngAnmAtr(u8 i_atr) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_atr == mAnmAtr || i_atr >= 5) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}

/* 022377BC */
void daNpc_Gk1_c::setAnm_ATR() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    init_texPttrnAnm(entry(a_anm_atr_tbl, mAnmAtr).btpNum, true);
    setAnm_anm(&entry(a_anm_atr_tbl, mAnmAtr));
}

/* 02237840 */
void daNpc_Gk1_c::anmAtr(u16 i_msgStatus) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            dComIfGp_clearMesgAnimeTagInfo();
            mMesgAnimeTag = tag;
            /* chngAnmTag(): empty */
        }
        break;
    }
    case 14:
        mAtrSet = 0;
        break;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 02237910 */
u16 daNpc_Gk1_c::next_msgStatus(be<u32>* o_msgNoP) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 msg_status = fopMsgStts_MSG_CONTINUES_e;
    switch (*o_msgNoP) {
    case 0x28A1:
        *o_msgNoP = 0x28A2;
        break;
    case 0x28A2:
        *o_msgNoP = 0x28A3;
        break;
    case 0x28A3:
        *o_msgNoP = 0x28A4;
        break;
    case 0x28A4:
        *o_msgNoP = 0x28A5;
        break;
    case 0x28A6:
        *o_msgNoP = 0x28A7;
        break;
    case 0x28A7:
        *o_msgNoP = 0x28A8;
        break;
    case 0x28A8:
        if (dComIfGs_isEventBit(0x1640)) {
            msg_status = fopMsgStts_MSG_ENDS_e;
        } else {
            *o_msgNoP = 0x28A9;
        }
        break;
    case 0x28A9:
        *o_msgNoP = 0x28AA;
        break;
    case 0x28AB:
        *o_msgNoP = 0x28AC;
        break;
    case 0x28AC:
        *o_msgNoP = 0x28AD;
        break;
    case 0x28AD:
        *o_msgNoP = 0x28AE;
        break;
    case 0x28AF:
        *o_msgNoP = 0x28B0;
        break;
    default:
        msg_status = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 02237A84 */
u32 daNpc_Gk1_c::getMsg_GK1_0() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGs_isEventBit(0x0B02) && !dComIfGs_isEventBit(0x1680)) {
        return 0x28A1;
    }
    if (!dKy_daynight_check()) {
        return dComIfGs_isEventBit(0x1640) ? 0x28A7 : 0x28A6;
    }
    return dComIfGs_isEventBit(0x0E08) ? 0x28AF : 0x28AB;
}

/* 02237B50 */
u32 daNpc_Gk1_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType == 0) {
        return getMsg_GK1_0();
    }
    return 0;
}

/* 022371BC */
void daNpc_Gk1_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 02236A80 */
void daNpc_Gk1_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
            mEvtState = 0;
            mMesgAnimeTag = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}

/* 02237B88 */
u8 daNpc_Gk1_c::chk_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}

/* 02237C20 */
u8 daNpc_Gk1_c::chk_parts_notMov() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
             mActorAngleY == current.angle.y);
}

/* 02236ED4 */
fopAc_ac_c* daNpc_Gk1_c::searchByID(u32 i_id, be<s32>* o_err) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_err = 0;
    if (!gkSearchByID(i_id, actor)) {
        *o_err = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}

/* 02236F28 */
void daNpc_Gk1_c::lookBack() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> look_pos;
    s16 target_y = current.angle.y;
    u8 head_only = mbHeadOnly;
    f32 srcX = current.pos.x;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    look_pos->x = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    look_pos->y = 0.0f;
    mActorAngleY = target_y;
    look_pos->z = 0.0f;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case LOOK_PLAYER: {
        gabi::Local<cXyz> eye;
        gkPlayerEyePos(eye, -20.0f);
        mLookPos.copy(*eye);
        look_pos->copy(*eye);
        look_pos_p = look_pos;
        break;
    }
    case LOOK_POSITION:
        look_pos->copy(mLookPos);
        look_pos_p = look_pos;
        break;
    case LOOK_ANGLE:
        target_y = mLookAngleY;
        break;
    case LOOK_PARTNER: {
        gabi::Local<be<s32>> err;
        fopAc_ac_c* partner = searchByID(mPartnerProcId, err);
        if (partner != nullptr && *err == 0) {
            mLookPos.copy(partner->current.pos);
            f32 y = partner->eyePos.y;
            look_pos->x = mLookPos.x;
            look_pos->y = y;
            look_pos->z = mLookPos.z;
            look_pos_p = look_pos;
            mLookPos.y = y;
        }
        break;
    }
    }
    gabi::Local<cXyz> cur_pos; /* passed by value: a copy */
    cur_pos->x = srcX;
    cur_pos->y = srcY;
    cur_pos->z = srcZ;
    gkLookAtTarget(&m_jnt, &current.angle.y, look_pos_p, cur_pos, target_y, l_HIO.mPrm.mLookVel, head_only);
}

/* 02237C60 */
u8 daNpc_Gk1_c::chkAttention() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dAttention_c* attention = dComIfGp_getAttention();
    if (gkLockonTruth(attention)) {
        return this == (void*)gkLockonTarget(attention, 0);
    }
    return this == (void*)gkActionTarget(attention, 0);
}

/* 02236494 */
void daNpc_Gk1_c::setAttention(u32 i_setEyePos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 y = current.pos.y + l_HIO.mPrm.mAttPosOffsetY;
    attPos->x = current.pos.x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}

/* 0223613C */
u8 daNpc_Gk1_c::decideType(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mTypeSet > 0) {
        return true;
    }
    mTypeSet = 1;
    mType = 0;
    /* strcpy(mArcName, "Gk") (3-byte lswi/stswi copy) */
    for (int i = 0; i < 3; i++)
        archiveByte(mArcName, i) = entry(l_archiveName, i);
    return mTypeSet != -1 && mType != -1;
}

/* 02236D98 */
void daNpc_Gk1_c::privateCut(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* a_cut_tbl (.data 0x101BE3E8): "dummy" */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = gkActionIndex(dComIfGp_getPEvtManager(), i_staffIdx, a_cut_tbl, 1, TRUE, 0);
    mActionIndex = idx;
    void* evmng = dComIfGp_getPEvtManager();
    if (idx != -1) {
        gkEventAdvance(evmng, i_staffIdx); /* dComIfGp_evmng_getIsAddvance (no cut to init) */
        evmng = dComIfGp_getPEvtManager();
    }
    gkCutEnd(evmng, i_staffIdx); /* dComIfGp_evmng_cutEnd */
}

/* 02236D54 */
void daNpc_Gk1_c::endEvent() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}

/* 02236D14 */
s32 daNpc_Gk1_c::isEventEntry() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 02236E30 */
void daNpc_Gk1_c::event_proc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        endEvent();
    } else if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 022361A0 */
BOOL daNpc_Gk1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        mActState = 9;
        callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    callAction(this, cur, i_argsP);
    return TRUE;
}

/* 02237CE8 */
void daNpc_Gk1_c::setStt(s8 i_stt) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case STT_WAIT:
        mEvtState = 0;
        speedF = 0.0f;
        break;
    case STT_TALK:
        mEvtState = 0;
        mMesgAnimeTag = 0xFF;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mAtrSet = 0;
        break;
    }
    setAnm();
}

/* 02237D40 */
u8 daNpc_Gk1_c::chk_attn() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getPlayer(0)->current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dy = current.pos.y - player->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - current.angle.y);
    f32 max_angle = mLookMode == LOOK_PLAYER ? 90.0f : 60.0f;
    if (dist < 200.0f) {
        s32 abs_angle = angle < 0 ? -angle : angle;
        if ((f32)abs_angle / 182.04444885253906f < max_angle && std::fabs(dy) < 300.0f) {
            return true;
        }
    }
    return false;
}

/* 02237F9C */
BOOL daNpc_Gk1_c::wait_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getPlayer(0)->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    mbFar = std_sqrtf(PSVECSquareMag(xz)) > 300.0f;
    if (mbFar) {
        cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    }
    if (mbTalk) {
        if (chk_talk()) {
            setStt(STT_TALK);
            mLookMode = LOOK_PLAYER;
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mbFar = false;
        }
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? LOOK_PLAYER : LOOK_HOME;
    return TRUE;
}

/* 022380B0 */
BOOL daNpc_Gk1_c::talk_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    BOOL res = chk_parts_notMov();
    talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    // HD: message status comes from the shared message manager.
    if (gkMessageStatus(l_message) == fopMsgStts_MSG_DESTROYED_e) {
        switch (mCurrMsgNo) {
        case 0x28A5:
            dComIfGs_onEventBit(0x1680);
            break;
        case 0x28AA:
            dComIfGs_onEventBit(0x1640);
            break;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        mTalkEndTimer = (s16)gkRandom(15, 30);
        endEvent();
    }
    return res;
}

/* 022381E8 */
BOOL daNpc_Gk1_c::wait_action1(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch ((u32)(s32)mActState) {
    case 0:
        setStt(STT_WAIT);
        mbFar = true;
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case STT_WAIT:
            mbSetEyePos = wait_1();
            break;
        case STT_TALK:
            mbSetEyePos = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}

/* 02236B58 */
u8 daNpc_Gk1_c::demo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo) {
            mbDemo = false;
        }
        return mbDemo;
    }
    if (!mbDemo) {
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        mbDemo = true;
        mbNoShapeAngle = false;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        void* obj = l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = l_demoObject;
        }
        demo_actor_p = gkDemoActor(obj, id);
    }
    J3DAnmTexPattern* btp = mBtpAnm.mpAnm;
    if (btp != 0) {
        u8 frame_max = (u8)getFrameMax(btp);
        u8 frame = (u8)(mBtpFrame + 1);
        if (frame < frame_max) {
            mBtpFrame = frame;
        } else {
            mBtpFrame = frame_max;
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = gkDemoBtp(demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            gkBtpInit(&mBtpAnm, J3DModel_getModelData(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1,
                      1, 0);
            mBtpFrame = 0;
            mBtpNum = 1;
        }
    }
    gkDemoData(this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}

/* 022374E8 */
BOOL daNpc_Gk1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData(morf_model_p);
    if (mDemoOnly || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); // HD: retain this otherwise unused accessor call.
    mDoExt_btpAnm_entry(&mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    removeTexPattern(morf_model_info_p); /* mBtpAnm.remove() */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    if (mpHatModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpHatModel, &tevStr);
        mDoExt_modelEntryDL(mpHatModel);
    }
    // HD: the GameCube shadow draw is absent.
    dSnap_RegistFig(0x59 /* DSNAP_TYPE_NPC_GK1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrm.mDebugDraw) {
        initializeColor(&l_redGuard, &l_redCopy, &l_red);
        initializeColor(&l_blueGuard, &l_blueCopy, &l_blue);
        initializeColor(&l_yellowGuard, &l_yellowCopy, &l_yellow);
    }
    return TRUE;
}

/* 0223722C */
BOOL daNpc_Gk1_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Gk1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mDemoOnly && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        mDemoOnly = 0;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!mbNoShapeAngle) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = gkRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = gkPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(false);
    if (!mbDemo) {
        setCollision(90.0f, 200.0f);
    }
    return TRUE;
}

/* 02236A28 */
BOOL daNpc_Gk1_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022368D4 */
cPhs_State daNpc_Gk1_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* fopAcM_ct(this, daNpc_Gk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gkNpcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = GK1_VTABLE;
            gkBtpCtor(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoaded = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    /* a_heap_size_tbl (.data 0x101BE3E0) */
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, entry(a_heap_size_tbl, mTypeSet))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -90.0f, -20.0f, -80.0f, 90.0f, 200.0f, 80.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 02235BF0 */
BOOL daNpc_Gk1_c::bodyCreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 7);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1571, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x623, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            deleteMorf(morf); /* delete (virtual deleting destructor) */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = gkNameIndex(a_mdl_dat->getJointName(), "head" /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x639, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = gkNameIndex(a_mdl_dat->getJointName(), "backbone" /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x63B, "m_bbone_jnt_num >= 0");
    m_nck_jnt_num = gkNameIndex(a_mdl_dat->getJointName(), "neck" /* "neck" */);
    if (m_nck_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x63D, "m_nck_jnt_num >= 0");
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, nodeCB_Head);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, nodeCB_BackBone);
    setJointCallBack(mpMorf->getModel(), m_nck_jnt_num, nodeCB_Neck);
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    return TRUE;
}

/* 02235F3C */
BOOL daNpc_Gk1_c::itemCreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 5);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1626, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x65A, "a_mdl_dat != (0)");
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}

/* 02235FD4 */
BOOL daNpc_Gk1_c::hat_CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 6);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1642, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_gk1.cpp", 0x66A, "a_mdl_dat != (0)");
    mpHatModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpHatModel.get() != nullptr;
}

/* 0223606C */
BOOL daNpc_Gk1_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap() || !hat_CreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 90.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 02236A24 */
static cPhs_State daNpc_Gk1_Create(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Gk1_c*)i_this)->_create();
}

/* 02236A7C */
static BOOL daNpc_Gk1_Delete(daNpc_Gk1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_delete();
}

/* 022374E4 */
static BOOL daNpc_Gk1_Execute(daNpc_Gk1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_execute();
}

/* 02237694 */
static BOOL daNpc_Gk1_Draw(daNpc_Gk1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_draw();
}

/* 02237698 */
static BOOL daNpc_Gk1_IsDelete(daNpc_Gk1_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 022382A0 */
static daNpc_Gk1_HIO_c* daNpc_Gk1_HIO_c_ct(daNpc_Gk1_HIO_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = (daNpc_Gk1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO_VTABLE;
    memcpy_g(&i_this->mPrm, &a_prm_tbl, 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->mDebugIndex = -1;
    return i_this;
}

/* 0223830C */
static void __sinit_d_a_npc_gk1_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
    daNpc_Gk1_HIO_c_ct(&l_HIO); /* static daNpc_Gk1_HIO_c l_HIO */
}

/* 022383AC */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

// HD: compiler-generated deleting destructor; no btp destructor call.
/* 022383C0 */
static void daNpc_Gk1_c_dt(daNpc_Gk1_c* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gkCircleDtor(circle(&i_this->mAcchCir), 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        acch(&i_this->mObjAcch)->mActorVtable = gc_tu.objAcchDt[0];
        acch(&i_this->mObjAcch)->mCheckVtable = gc_tu.objAcchDt[1];
        gkAcchDtor(&i_this->mObjAcch, 0);
        gkActorDtor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0223845C */
static void SafeString_assureTerminationImpl(SafeString*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): TU data and ABI bindings
str 10019FE4 "d_a_npc_gk1.cpp" in 02235B00
str 10019FF4 "a_btp != (0)" in 02235B00
str 1001A014 "d_a_npc_gk1.cpp" in 02235BF0
str 1001A024 "a_mdl_dat != (0)" in 02235BF0
str 1001A004 "head" in 02235BF0
str 1001A038 "m_hed_jnt_num >= 0" in 02235BF0
str 1001A04C "backbone" in 02235BF0
str 1001A058 "m_bbone_jnt_num >= 0" in 02235BF0
str 1001A00C "neck" in 02235BF0
str 1001A070 "m_nck_jnt_num >= 0" in 02235BF0
str 1001A084 "d_a_npc_gk1.cpp" in 02235F3C
str 1001A094 "a_mdl_dat != (0)" in 02235F3C
str 1001A0A8 "d_a_npc_gk1.cpp" in 02235FD4
str 1001A0B8 "a_mdl_dat != (0)" in 02235FD4
str 1001A0E4 "Gk1" in 02236748
str 10019FC8 "d_demo.h" in 02236B58
str 10019FB8 "m_object != (0)" in 02236B58
data l_btpResID 10019FDC
data l_bckResID 1001A104
data a_heap_size_tbl 101BE3E0
data a_anm_prm_tbl 101BE3EC
data a_anm_atr_tbl 101BE41C
data l_evn_tbl 101BE3BC
str 10019F68
data a_cut_tbl 101BE3E8
str 1001A0FC
data l_archiveName 1001A0D8
data a_prm_tbl 101BE46C
data l_red 10019F58
data l_blue 10019F5C
data l_yellow 10019F60
data l_redCopy 101FEBF4
data l_blueCopy 101FEBF8
data l_yellowCopy 101FEBE8
data l_redGuard 101FDA50
data l_blueGuard 101FDAC0
data l_yellowGuard 101FDA44
data l_HIO 10467050
data l_waitAction 10019F50
data a_eye_pos_off 10467088
data l_eyeGuard 10467094
data l_cullDistance 1048D04C
data l_message 101F4B5C
data l_demoObject 101D5FFC
data dNpc_cyl_src 101EA190
ext 021E1E78 s32 gkRandom(s32, s32)
ext 025E789C s32 gkBtpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 027DF9B0 s8 gkNameIndex(JUTNameTab*, const char*)
ext 024EDFCC bool gkLockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* gkLockonTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* gkActionTarget(dAttention_c*, s32)
ext 024EF130 s8 gkRoomId(dBgS*, void*)
ext 024EEEB8 u8 gkPolyColor(dBgS*, void*)
ext 024EECAC u32 gkMaterialSound(dBgS*, void*)
ext 0259D54C void gkPlayerEyePos(cXyz*, f32)
ext 0259E2D4 void gkLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 025D54C4 BOOL gkSearchByID(u32, be<u32>*)
ext 025F795C u32 gkMessageStatus(msg_class*)
ext 025A1458 void gkNpcCtor(fopNpc_npc_l*)
ext 025E7820 void gkBtpCtor(void*)
ext 02526E70 void* gkDemoActor(void*, u8)
ext 02527828 void* gkDemoBtp(void*, const char*)
ext 02527028 void gkDemoData(daNpc_Gk1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, s32, s32)
ext 02542EDC s8 gkActionIndex(dEvent_manager_c*, s32, gptr<char>*, s32, BOOL, s32)
ext 025447C8 BOOL gkEventAdvance(void*, s32)
ext 02543280 void gkCutEnd(void*, s32)
ext 02018034 void gkCircleDtor(void*, s32)
ext 024EFD9C void gkAcchDtor(dBgS_ObjAcch*, s32)
ext 025D50BC void gkActorDtor(fopAc_ac_c*, s32)
*/
// clang-format on
