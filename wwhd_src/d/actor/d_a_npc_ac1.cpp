/**
 * d_a_npc_ac1.cpp (WWHD)
 * NPC - Prince Komali (after Dragon Roost Cavern)
 *
 * Written from the WWHD code (every GameCube function
 * of zeldaret/tww src/d/actor/d_a_npc_ac1.cpp is a "Nonmatching" stub; the names follow the
 * GameCube symbols), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_ac1.h"

// HD: per-TU resource-key vtable and data owners.
const gc_tu_data gc_tu = {.safeString = 0x10015B3C};
static daNpc_Ac1_HIO_c& l_HIO = gabi::object_at<daNpc_Ac1_HIO_c>(0x1046596C);
static cXyz& a_eye_pos_off = gabi::object_at<cXyz>(0x104659A8);
static be<u32>& l_eye_guard = gabi::object_at<be<u32>>(0x104659B4);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Ac1ActionFunc& l_wait_action = gabi::object_at<Ac1ActionFunc>(0x10015B28);
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static gptr<void>& l_message_manager = gabi::object_at<gptr<void>>(0x101F4B5C);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& a_red_colour = gabi::object_at<be<u32>>(0x10015B30);
static be<u32>& a_blue_colour = gabi::object_at<be<u32>>(0x10015B34);
using Ac1PrmBytes = be<u8>[0x30];
static Ac1PrmBytes& a_prm_tbl = gabi::object_at<Ac1PrmBytes>(0x101BB22C);
using Ac1BtpIds = be<s32>[4];
static Ac1BtpIds& a_btp_resID_tbl = gabi::object_at<Ac1BtpIds>(0x10015C58);
using Ac1BckIds = be<s32>[4];
static Ac1BckIds& a_bck_resID_tbl = gabi::object_at<Ac1BckIds>(0x10015E08);
static Ac1BckIds& a_arm_resID_tbl = gabi::object_at<Ac1BckIds>(0x10015E18);
using Ac1WingNames = be<u32>[4];
static Ac1WingNames& a_wng_name_tbl = gabi::object_at<Ac1WingNames>(0x101BB168);
using Ac1AnmParams = daNpc_Ac1_c::anm_prm_c[3];
using Ac1AttrParams = daNpc_Ac1_c::anm_prm_c[6];
static Ac1AnmParams& a_anm_state_tbl = gabi::object_at<Ac1AnmParams>(0x101BB178);
static Ac1AttrParams& a_anm_attr_tbl = gabi::object_at<Ac1AttrParams>(0x101BB1B4);
static be<u32>& a_heap_size = gabi::object_at<be<u32>>(0x101BB160);
static gptr<char>& a_cut_tbl = gabi::object_at<gptr<char>>(0x101BB164);
enum Ac1MessageStatus { AC1_MSG_TYPING = 6, AC1_MSG_DISPLAYED = 14, AC1_MSG_DESTROYED = 19 };
enum Ac1State { AC1_WAIT = 1, AC1_TALK = 2 };

/* 021E4CCC */
static daNpc_Ac1_HIO_c* daNpc_Ac1_HIO_c_ct(daNpc_Ac1_HIO_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_Ac1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10015B74;
    // HD: the parameter block is copied byte-for-byte.
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x30); /* 028FEAC0 */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 021E2088 */
static BOOL nodeCallBack_Wng(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ac1_c* user = static_cast<daNpc_Ac1_c*>(model->getUserArea());
        if (user != nullptr) {
            user->nodeWngControl(i_node, model);
        }
    }
    return TRUE;
}

/* 021E1EFC */
void daNpc_Ac1_c::nodeWngControl(J3DNode* i_node, J3DModel* i_model) {

    u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    PSMTXCopy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m700) {
        PSMTXCopy(&mArmLMtx, J3DSys::mCurrentMtx);
        mtx_copy(i_model->getAnmMtx(jointIdx), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m701) {
        PSMTXCopy(&mArmRMtx, J3DSys::mCurrentMtx);
        mtx_copy(i_model->getAnmMtx(jointIdx), &mArmRMtx);
    }
}

/* 021E225C */
static BOOL nodeCallBack_Arm(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ac1_c* user = static_cast<daNpc_Ac1_c*>(model->getUserArea());
        if (user != nullptr) {
            user->nodeArmControl(i_node, model);
        }
    }
    return TRUE;
}

/* 021E20D0 */
void daNpc_Ac1_c::nodeArmControl(J3DNode* i_node, J3DModel* i_model) {

    u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    PSMTXCopy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_armL_loc_jnt_num) {
        PSMTXCopy(&mArmLMtx, J3DSys::mCurrentMtx);
        mtx_copy(i_model->getAnmMtx(jointIdx), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m_armR_loc_jnt_num) {
        PSMTXCopy(&mArmRMtx, J3DSys::mCurrentMtx);
        mtx_copy(i_model->getAnmMtx(jointIdx), &mArmRMtx);
    }
}

/* 021E247C */
static BOOL nodeCallBack_Ac1(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ac1_c* user = static_cast<daNpc_Ac1_c*>(model->getUserArea());
        if (user != nullptr) {
            user->nodeAc1Control(i_node, model);
        }
    }
    return TRUE;
}

/* 021E22A4 */
void daNpc_Ac1_c::nodeAc1Control(J3DNode* i_node, J3DModel* i_model) {

    // HD: preserve the guarded eye-offset initialization and its store order.

    if ((u32)l_eye_guard == 0) {
        l_eye_guard = 1;
        a_eye_pos_off.x = 20.0f;
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.y = 18.0f;
    }
    u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    PSMTXCopy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m6CC) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), &a_eye_pos_off, &mTransformedEyePos);
    }
    if (jointIdx == (u32)(s32)m6CD) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    if (jointIdx == (u32)(s32)m6CE) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m6CF) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mArmRMtx);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
}

/* 021E31A0 */
bool daNpc_Ac1_c::init_AC1_0() {

    bool ret = dComIfGs_isEventBit(0x2E04) != 0;
    if (ret) {
        dComIfGs_isEventBit(0x1580); /* the result is not used */
        gabi::Local<Ac1ActionFunc> pmf;
        Ac1_loadAction(pmf, &l_wait_action);
        set_action(pmf, nullptr);
    }
    return ret;
}

/* 021E35E4 */
bool daNpc_Ac1_c::createInit() {

    m814.copy(current.pos);
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;  /* attention_info.distances[TALK] */
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e |
        fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -4.5f;
    u8 path = (u8)(fopAcM_GetParam(this) >> 16);
    if (path != 0xFF) {
        mPathRun.setInf(path, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &=
            ~u32(fopAcStts_NOCULLEXEC_e); /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    }
    mEventCut.setActorInfo2("Ac1" /* "Ac1" */, (fopNpc_npc_c*)this);
    m871 = 4;
    if (m879 != 0) {
        return false;
    }
    if (!init_AC1_0()) {
        return false;
    }
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    shape_angle.x = current.angle.x;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    if (!m85E) {
        m6FC->setMorf(0.0f);
    } else {
        m708->setMorf(0.0f);
    }
    setMtx(true);
    return true;
}

/* 021E3358 */
void daNpc_Ac1_c::setMtx(u32 param_1) {

    if (!m86B) {
        plyTexPttrnAnm();
        m858 = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < m82C) {
            m858 = 1;
        }
        m82C = mpMorf->getFrame();
        if (m85E) {
            m708->play(&eyePos, 0, 0);
        } else {
            m6FC->play(&eyePos, 0, 0);
        }
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    /* the ground polygon: cBgS_PolyInfo at m_gnd + 0x14 */
    cBgS_PolyInfo* gndPoly = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd);
    tevStr.mRoomNo = Ac1_roomId(dComIfG_Bgsp(), gndPoly);
    tevStr.mEnvrIdxOverride = Ac1_polyColour(dComIfG_Bgsp(), gndPoly); /* tevStr.mEnvrIdxOverride */

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (!m85E) {
        m6FC->calc();
    } else {
        m708->calc();
    }
    if (m6D4.get() != nullptr && m85E) {
        J3DModel_setBaseTRMtx(m6D4, m708->getModel()->getAnmMtx(m_handR_jnt_num));
        J3DModel_calc(m6D4);
    }
    setAttention(param_1);
}

/* 021E4348 */
int daNpc_Ac1_c::anmNum_toResID(int i_anmNum) {

    return (s32)a_bck_resID_tbl[i_anmNum]; /* a_bck_resID_tbl */
}

/* 021E4370 */
u32 daNpc_Ac1_c::wingAnmNum_toResID(int i_anmNum) { return (u32)a_wng_name_tbl[i_anmNum]; }

/* 021E435C */
int daNpc_Ac1_c::armAnmNum_toResID(int i_anmNum) { return (s32)a_arm_resID_tbl[i_anmNum]; }

/* 021E274C */
int daNpc_Ac1_c::btpNum_toResID(int i_btpNum) {

    return (s32)a_btp_resID_tbl[i_btpNum]; /* a_btp_resID_tbl */
}

/* 021E2760 */
// HD: the GameCube bool argument retains its full incoming register value.
bool daNpc_Ac1_c::setBtp(u32 i_param_1, int i_btp_num) {

    J3DModelData* model_data = mpMorf->getModel()->getModelData();
    int res_id = btpNum_toResID(i_btp_num);
    m6DC = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Ac" /* "Ac" */, res_id);
    if (m6DC.get() == nullptr) /* JUT_ASSERT(572, m6DC != NULL) */
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x23C, "m_hed_tex_pttrn != (0)");
    int iVar1 = Ac1_btpInit(mBtpAnim, model_data, m6DC, 1, 2, 1.0f, 0, -1, i_param_1, 0);
    bool o_retval = iVar1 == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        m6F4 = 0;
    }
    return o_retval;
}

/* 021E284C */
// HD: the tail call preserves the complete return register, without narrowing to bool.
u32 daNpc_Ac1_c::iniTexPttrnAnm(u32 i_param_1) {

    return Ac1_setBtpResult(this, i_param_1, (s32)m870); /* setBtp(i_param_1, m870) */
}

/* 021E3230 */
void daNpc_Ac1_c::plyTexPttrnAnm() {

    if (m870 != 0 || !Ac1_calcTimer(&mBlinkTimer)) {
        u8 frame = (u8)(m6F4 + 1);
        m6F4 = frame;
        if ((s32)frame >= Ac1_frameMax(m6DC)) {
            if (m870 != 0) {
                m6F4 = (u8)Ac1_frameMax(m6DC);
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                m6F4 = 0;
                mBlinkTimer = t;
            }
        }
    }
}

/* 021E4384 */
u32 daNpc_Ac1_c::setAnm_tex(s8 i_btp_num) {

    if (m870 != i_btp_num) {
        m870 = i_btp_num;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); // HD: unchanged-animation path returns the original this register.
}

/* 021E43A4 */
BOOL daNpc_Ac1_c::setAnm_anm(daNpc_Ac1_c::anm_prm_c* i_anmPrmP) {

    if (m871 == i_anmPrmP->mAnmNum) {
        return TRUE;
    }
    m871 = i_anmPrmP->mAnmNum;
    int resID = anmNum_toResID(m871);
    dNpc_setAnmIDRes(mpMorf, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, resID, -1,
                     "Ac" /* "Ac" */);
    m85E = i_anmPrmP->mbArm == 1;
    if (m85E) {
        int armID = armAnmNum_toResID(m871);
        dNpc_setAnmIDRes(m708, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, armID, -1,
                         "Ac");
    } else {
        u32 name = wingAnmNum_toResID(m871);
        Ac1_setWingAnimation(m6FC, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, name,
                             0, "Ac");
    }
    mAnimationLoopCount = 0;
    m82C = 0.0f;
    m858 = 0;
    return TRUE;
}

/* 021E44DC */
bool daNpc_Ac1_c::setAnm() {

    anm_prm_c* a_anm_prm_tbl = a_anm_state_tbl; /* [3] */
    if (a_anm_prm_tbl[m875].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[m875].mBtpNum);
    }
    if (a_anm_prm_tbl[m875].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[m875]);
    }
    return true;
}

/* 021E45D0 */
void daNpc_Ac1_c::chg_anmAtr(u8 i_param_1) {

    if ((i_param_1 >= 6) || (i_param_1 == m86E)) {
        return;
    }
    m86E = i_param_1;
    setAnm_ATR(1);
}

/* 021E4560 */
void daNpc_Ac1_c::setAnm_ATR(int i_param_1) {

    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl; /* [6] */
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[m86E].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[m86E]);
}

/* 021E45F4 */
void daNpc_Ac1_c::anmAtr(u16 i_param_1) {

    switch (i_param_1) {
    case AC1_MSG_TYPING: {
        if (mMessageAnimationStarted == 0) {
            m86E = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mMessageAnimationStarted = mMessageAnimationStarted + 1;
        }
        u8 mesgAnimeTagInfo = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        dComIfGp_clearMesgAnimeTagInfo();                     /* dComIfGp_clearMesgAnimeTagInfo() */
        if (mesgAnimeTagInfo != 0xFF && mMesgAnimeTagInfo != mesgAnimeTagInfo) {
            mMesgAnimeTagInfo = mesgAnimeTagInfo;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case AC1_MSG_DISPLAYED:
        mMessageAnimationStarted = 0;
        break;
    }
    /* control_anmTag(), control_anmAtr(): empty */
}

/* 021E3F58 */
void daNpc_Ac1_c::eventOrder() {

    if (m874 == 1 || m874 == 2) {
        s8 state = m874;
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (state == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 021E3950 */
void daNpc_Ac1_c::checkOrder() {

    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        return;
    }
    if (command == dEvtCmd_INTALK_e) {
        if (m874 == 1 || m874 == 2) {
            m874 = 0;
            m869 = 1;
        }
    }
}

/* 021E46CC */
bool daNpc_Ac1_c::chk_talk() {

    bool ret = true;
    mItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}

/* 021E474C */
u8 daNpc_Ac1_c::chk_partsNotMove() {

    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
           mActorAngleY == current.angle.y;
}

/* 021E3CC0 */
void daNpc_Ac1_c::lookBack() {

    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    mJointBackboneY = m_jnt.mAngles[1][1];
    s16 desiredYrot = current.angle.y;
    mActorAngleY = desiredYrot;
    mJointHeadY = m_jnt.mAngles[0][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    dstPos_p = nullptr;
    u8 headOnlyFollow = mHeadOnlyFollow;

    switch ((u32)(s32)m877) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case 2:
        dstPos->copy(m814);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    case 3:
        desiredYrot = m854;
        break;
    }
    cLib_addCalcAngleS2(&m852, l_HIO.mPrmTbl.mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        m852 = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    Ac1_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, m852,
                     headOnlyFollow);
}

/* 021E478C */
u16 daNpc_Ac1_c::next_msgStatus(be<u32>* i_msg_no) {

    u16 ret = fopMsgStts_MSG_CONTINUES_e;
    switch (*i_msg_no) {
    case 0x184C:
        *i_msg_no = 0x184D;
        break;
    case 0x184D:
        *i_msg_no = 0x184E;
        break;
    case 0x1850:
        *i_msg_no = 0x1851;
        break;
    case 0x1851:
        *i_msg_no = 0x1852;
        break;
    default:
        ret = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return ret;
}

/* 021E47EC */
u32 daNpc_Ac1_c::getBitMask() {

    u32 ret = 0;
    if (m879 == 0) {
        ret = 0x10;
    }
    return ret;
}

/* 021E4804 */
u32 daNpc_Ac1_c::getMsg_AC1_0() {

    s32 reg = (s8)dComIfGs_getEventReg(0xB8FF);
    u32 mask = getBitMask();
    if (dComIfGs_isEventBit(0x1580)) {
        if (mask & reg) {
            return 0x1853;
        }
        dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
        return 0x1850;
    }
    if (mask & reg) {
        return 0x184F;
    }
    dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
    return 0x184C;
}

/* 021E4938 */
u32 daNpc_Ac1_c::getMsg() {

    u32 ret = 0;
    if (m879 == 0) {
        ret = getMsg_AC1_0();
    }
    return ret;
}

/* 021E4970 */
u8 daNpc_Ac1_c::chkAttention() {

    dAttention_c* attention = dComIfGp_getAttention();
    if (Ac1_lockonTruth(attention)) {
        return this == (void*)Ac1_lockonTarget(attention, 0);
    }
    return this == (void*)Ac1_actionTarget(attention, 0);
}

/* 021E3304 */
void daNpc_Ac1_c::setAttention(u32 i_setEyePos) {

    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_HIO.mPrmTbl.mAttPosOffsetY;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mTransformedEyePos.z;
    eyePos.y = mTransformedEyePos.y;
    eyePos.x = mTransformedEyePos.x;
}

/* 021E3060 */
bool daNpc_Ac1_c::charDecide(int) {

    m879 = 0;
    mType = 0;
    return true;
}

/* 021E3B84 */
void daNpc_Ac1_c::event_actionInit(int i_staffIdx) {

    be<s32>* actNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "ActNo" /* "ActNo" */);
    if (actNo != nullptr) {
        mActNo = (s8)(s32)*actNo;
    }
}

/* 021E3BE4 */
bool daNpc_Ac1_c::event_action() { return true; }

/* 021E3BEC */
void daNpc_Ac1_c::privateCut(int i_staffIdx) {

    // The single private cut is ACTION.
    if (i_staffIdx == -1) {
        return;
    }
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(i_staffIdx, &a_cut_tbl, 1, 1, 0); /* getMyActIdx */
    mActionIndex = actIdx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (actIdx != -1) {
        if (Ac1_eventAdvanced(evmng, i_staffIdx) /* getIsAddvance */) {
            if (mActionIndex == 0) {
                event_actionInit(i_staffIdx);
            }
        }
        if (mActionIndex == 0) {
            if (!event_action()) {
                return;
            }
        }
        evmng = dComIfGp_getPEvtManager();
    }
    Ac1_cutEnd(evmng, i_staffIdx); /* cutEnd */
}

/* 021E49F8 */
void daNpc_Ac1_c::endEvent() {

    dComIfGp_event_reset();
    m86E = 0xFF;
}

/* 021E3B44 */
s32 daNpc_Ac1_c::isEventEntry() {

    const char* name = Ac1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 021E3F00 */
void daNpc_Ac1_c::event_proc(int i_staffIdx) {

    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
    lookBack();
}

/* 021E3074 */
BOOL daNpc_Ac1_c::set_action(Ac1ActionFunc* i_newProcFunc, void* i_argsP) {

    Ac1ActionFunc* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if ((u16)cur->d == (u16)newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        mActState = 9;
        Ac1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Ac1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 021E4A38 */
void daNpc_Ac1_c::setStt(s8 i_status) {

    s8 prev = m875;
    m875 = i_status;
    m84C = 0;
    if (i_status == 2) {
        m876 = prev;
        m86E = 0xFF;
        m877 = 1;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        return;
    }
    setAnm();
}

/* 021E4A74 */
BOOL daNpc_Ac1_c::wait_1() {

    if (m874 == 1 || m874 >= 3) {
        return TRUE;
    }
    if (m869) {
        if (chk_talk()) {
            setStt(AC1_TALK);
        }
        return TRUE;
    }
    m874 = 2;
    if (mbAttention) {
        m84C = 60;
    }
    if (Ac1_calcTimer(&m84C)) {
        m877 = 1;
    } else {
        m877 = 3;
        m854 = m800.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}

/* 021E4B38 */
BOOL daNpc_Ac1_c::talk_1() {

    // HD: the message manager pointer is read once
    void* msgMng = l_message_manager;
    u8 ret = chk_partsNotMove();
    u16 status = 0;
    if (mbHasMsg) {
        status = (u16)Ac1_messageStatus(msgMng);
    }
    mMsgStatus = status;
    talk(1);
    if (mbHasMsg) {
        if (Ac1_messageStatus(msgMng) == AC1_MSG_DESTROYED) {
            dComIfGs_onEventBit(0x3F02);
            mItemNo = 0xFF;
            m869 = 0;
            setStt(m876);
            m84C = 60;
            endEvent();
        }
    }
    return ret;
}

/* 021E4C10 */
BOOL daNpc_Ac1_c::wait_action1(void*) {

    if (mActState == 0) {
        setStt(AC1_WAIT);
        mActState = mActState + 1;
    } else if ((u32)(s32)mActState <= 3) {
        mbAttention = chkAttention();
        switch ((u32)(s32)m875) {
        case 1:
            mbSetEyePos = wait_1();
            lookBack();
            break;
        case 2:
            mbSetEyePos = talk_1();
            lookBack();
            break;
        default:
            lookBack();
            break;
        }
    }
    return TRUE;
}

/* 021E3990 */
/* returns the m86B byte (typed u8) */
u8 daNpc_Ac1_c::demo() {

    if (demoActorID == 0) {
        if (m86B != 0) {
            m86B = 0;
        }
        return 0;
    }
    u8 id = demoActorID;
    m86B = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check, the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = (u32)l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demo_object;
        }
        demo_actor = Ac1_demoGetActor(obj, id);
    }
    if (m6DC.get() != nullptr) {
        u8 frame = (u8)(m6F4 + 1);
        m6F4 = frame;
        if ((s32)frame >= Ac1_frameMax(m6DC)) {
            m6F4 = (u8)Ac1_frameMax(m6DC);
        }
    }
    // HD: demo_actor is checked for NULL
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = Ac1_demoBtp(demo_actor, "Ac" /* "Ac" */);
        if (demopattern != nullptr) {
            m6DC = demopattern;
            J3DModelData* md = mpMorf->getModel()->getModelData();
            if (Ac1_btpInit(mBtpAnim, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                m6F4 = 0;
                m870 = 3;
            }
        }
    }
    Ac1_demoData(this, 0x6A, mpMorf, "Ac" /* "Ac" */, 0, nullptr, 0, 0);
    return m86B;
}

/* 021E417C */
BOOL daNpc_Ac1_c::_draw() {

    J3DModel* morf_model = mpMorf->getModel();
    J3DModelData* model_data = morf_model->getModelData();
    if (m85D || m861) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnim, model_data, m6F4);
    mpMorf->entryDL();
    Ac1_view<Ac1ModelAnimation>(model_data)->mpBtpAnimation = 0; /* mBtpAnim.remove(model_data) */
    if (m85E) {
        J3DModel* m = m708->getModel();
        setLightTevColorType(dKy_getEnvlight(), m, &tevStr);
        m708->entryDL();
    } else {
        J3DModel* m = m6FC->getModel();
        setLightTevColorType(dKy_getEnvlight(), m, &tevStr);
        m6FC->entryDL();
    }
    if (m6D4.get() != nullptr && m85E) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, m6D4, &tevStr);
        mDoExt_modelEntryDL(m6D4);
    }
    // HD: no shadowDraw()
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrmTbl.mDebug != 0) {
        if ((u32)l_red_guard == 0) {
            l_red_guard = 1;
            memcpy_g(&l_red_colour, &a_red_colour, 4); /* 028FEAC0 */
        }
        if ((u32)l_blue_guard == 0) {
            l_blue_guard = 1;
            memcpy_g(&l_blue_colour, &a_blue_colour, 4); /* 028FEAC0 */
        }
    }
    dSnap_RegistFig(0x8B /* DSNAP_TYPE_NPC_AC1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 021E3F90 */
BOOL daNpc_Ac1_c::_execute() {

    if (!m863) {
        m7F4.copy(current.pos);
        m800.z = current.angle.z;
        m800.x = current.angle.x;
        m800.y = current.angle.y;
        m863 = 1;
    }
    daNpc_Ac1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m85D && demoActorID == 0) {
        return TRUE;
    }
    m860 = 0;
    m85D = 0;
    checkOrder();
    if (!demo()) {
        bool ran = false;
        if (dComIfGp_event_runCheck() &&
            (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */) {
            s32 staffIdx = isEventEntry();
            if (staffIdx >= 0) {
                event_proc(staffIdx);
                ran = true;
            }
        }
        if (!ran) {
            Ac1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!m860) {
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }
        if (!m85F) {
            shape_angle.z = current.angle.z;
            shape_angle.y = current.angle.y;
            shape_angle.x = current.angle.x;
        }
    }
    eventOrder();
    setMtx(false);
    if (!m86B) {
        setCollision(50.0f, 140.0f);
    }
    return TRUE;
}

/* 021E38D4 */
BOOL daNpc_Ac1_c::_delete() {

    dComIfG_resDelete(&mPhs, "Ac" /* "Ac" */);
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr)
            mpMorf->stopZelAnime();
        if (m6FC.get() != nullptr)
            m6FC->stopZelAnime();
        if (m708.get() != nullptr)
            m708->stopZelAnime();
    }
    return TRUE;
}

/* 021E305C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {

    return static_cast<daNpc_Ac1_c*>(i_this)->CreateHeap();
}

/* 021E3794 */
cPhs_State daNpc_Ac1_c::_create() {

    /* fopAcM_ct(this, daNpc_Ac1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Ac1_npcCtor(
                this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = 0x10015E88;
            Ac1_btpCtor(mBtpAnim);        /* mDoExt_btpAnm::mDoExt_btpAnm */
            Ac1_eventCutCtor(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Ac" /* "Ac" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    // Heap size follows this actor archive.
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 021E24C4 */
J3DModelData* daNpc_Ac1_c::create_Anm() {

    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ac" /* "Ac" */, 5);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x709, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes("Ac", 1);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm,
                                    2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            Ac1_destroyMorf(morf);
        mpMorf = nullptr;
        return nullptr;
    }
    m6CC = (s8)a_mdl_dat->getJointName()->getIndex("head" /* "head" */);
    if (m6CC < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x71F, "m_hed_jnt_num >= 0");
    m6CD = (s8)a_mdl_dat->getJointName()->getIndex("backbone" /* "backbone" */);
    if (m6CD < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x722, "m_bbone_jnt_num >= 0");
    m6CE = (s8)a_mdl_dat->getJointName()->getIndex("armL" /* "armL" */);
    if (m6CE < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x725, "m_arm_L_jnt_num >= 0");
    m6CF = (s8)a_mdl_dat->getJointName()->getIndex("armR" /* "armR" */);
    if (m6CF < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x728, "m_arm_R_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021E2858 */
J3DModelData* daNpc_Ac1_c::create_wng_Anm() {

    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ac" /* "Ac" */, 3);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x745, "a_mdl_dat != (0)");
    J3DAnmTransform* anm =
        (J3DAnmTransform*)Ac1_objectResource("Ac", "acwing_wait01.fskb" /* "acwing_wait01.fskb" */);
    m6FC = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1,
                                  nullptr, 0x80000, 0x11000002);
    mDoExt_McaMorf* morf = m6FC;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            Ac1_destroyMorf(morf);
        m6FC = nullptr;
        return nullptr;
    }
    m700 = (s8)a_mdl_dat->getJointName()->getIndex("wingL_loc" /* "wingL_loc" */);
    if (m700 < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x760, "m_wngL1_jnt_num >= 0");
    m701 = (s8)a_mdl_dat->getJointName()->getIndex("wingR_loc" /* "wingR_loc" */);
    if (m701 < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x763, "m_wngR1_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021E2A50 */
J3DModelData* daNpc_Ac1_c::create_arm_Anm() {

    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ac" /* "Ac" */, 2);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x77A, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes("Ac", 0);
    m708 = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1,
                                  nullptr, 0x80000, 0x11000002);
    mDoExt_McaMorf* morf = m708;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            Ac1_destroyMorf(morf);
        m708 = nullptr;
        return nullptr;
    }
    m_handR_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("handR" /* "handR" */);
    if (m_handR_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x791, "m_hnd_R_jnt_num >= 0");
    m_armL_loc_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("armL_loc" /* "armL_loc" */);
    if (m_armL_loc_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x794, "m_armL1_jnt_num >= 0");
    m_armR_loc_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("armR_loc" /* "armR_loc" */);
    if (m_armR_loc_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x797, "m_armR1_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021E2C88 */
bool daNpc_Ac1_c::create_itm_Mdl() {

    m6D4 = nullptr;
    J3DModelData* a_mdl_dat;
    if (dComIfGs_isEventBit(0x1580)) {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ac" /* "Ac" */, 0xF);
    } else {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ac", 4);
    }
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_ac1.cpp", 0x7BA, "a_mdl_dat != (0)");
    m6D4 = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return true;
}

/* 021E2D68 */
BOOL daNpc_Ac1_c::CreateHeap() {

    J3DModelData* body_mdl = create_Anm();
    if (body_mdl == nullptr) {
        return FALSE;
    }
    m870 = 0;
    J3DModelData* wng_mdl;
    if (!iniTexPttrnAnm(false) || (wng_mdl = create_wng_Anm()) == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    J3DModelData* arm_mdl = create_arm_Anm();
    if (arm_mdl != nullptr && create_itm_Mdl()) {
        for (u16 i = 0; i < wng_mdl->getJointNum(); i++) {
            if (i == (u32)(s32)m700 || i == (u32)(s32)m701)
                m6FC->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
                    nodeCallBack_Wng);
        }
        m6FC->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
        for (u16 i = 0; i < arm_mdl->getJointNum(); i++) {
            if (i == (u32)(s32)m_armL_loc_jnt_num || i == (u32)(s32)m_armR_loc_jnt_num)
                m708->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
                    nodeCallBack_Arm);
        }
        m708->getModel()->setUserArea((uintptr_t)this);
        for (u16 i = 0; i < body_mdl->getJointNum(); i++) {
            if (i == (u32)(s32)m6CC || i == (u32)(s32)m6CD || i == (u32)(s32)m6CE ||
                i == (u32)(s32)m6CF)
                mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
                    nodeCallBack_Ac1);
        }
        mpMorf->getModel()->setUserArea((uintptr_t)this);
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    m6FC = nullptr;
    return FALSE;
}

/* 021E38D0 */
static cPhs_State daNpc_Ac1_Create(fopAc_ac_c* i_this) { return ((daNpc_Ac1_c*)i_this)->_create(); }

/* 021E394C */
static BOOL daNpc_Ac1_Delete(daNpc_Ac1_c* i_this) { return i_this->_delete(); }

/* 021E4178 */
static BOOL daNpc_Ac1_Execute(daNpc_Ac1_c* i_this) { return i_this->_execute(); }

/* 021E433C */
static BOOL daNpc_Ac1_Draw(daNpc_Ac1_c* i_this) { return i_this->_draw(); }

/* 021E4340 */
static BOOL daNpc_Ac1_IsDelete(daNpc_Ac1_c*) { return TRUE; }

/* 021E4D38 */
static void __sinit_d_a_npc_ac1_cpp() {

    sinit_header_statics(0x10465950, 0x101BB25C);
    daNpc_Ac1_HIO_c_ct(&l_HIO); /* static daNpc_Ac1_HIO_c l_HIO */
}

/* 021E4DD8 */
static void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 021E4DEC */
static void daNpc_Ac1_c_dt(daNpc_Ac1_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Ac1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        Ac1_view<Ac1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x10015B54;
        Ac1_view<Ac1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x10015B64;
        Ac1_acchDestroy(&i_this->mObjAcch, 0);
        Ac1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021E4E88 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 10015BB0 "Ac" in 021E24C4
str 10015BCC "d_a_npc_ac1.cpp" in 021E24C4
str 10015BDC "a_mdl_dat != (0)" in 021E24C4
str 10015BB4 "head" in 021E24C4
str 10015BF0 "m_hed_jnt_num >= 0" in 021E24C4
str 10015C04 "backbone" in 021E24C4
str 10015C10 "m_bbone_jnt_num >= 0" in 021E24C4
str 10015BBC "armL" in 021E24C4
str 10015C28 "m_arm_L_jnt_num >= 0" in 021E24C4
str 10015BC4 "armR" in 021E24C4
str 10015C40 "m_arm_R_jnt_num >= 0" in 021E24C4
str 10015C64 "Ac" in 021E2760
str 10015C68 "d_a_npc_ac1.cpp" in 021E2760
str 10015C78 "m_hed_tex_pttrn != (0)" in 021E2760
str 10015C90 "Ac" in 021E2858
str 10015C94 "d_a_npc_ac1.cpp" in 021E2858
str 10015CBC "a_mdl_dat != (0)" in 021E2858
str 10015CD0 "acwing_wait01.fskb" in 021E2858
str 10015CA4 "wingL_loc" in 021E2858
str 10015CE4 "m_wngL1_jnt_num >= 0" in 021E2858
str 10015CB0 "wingR_loc" in 021E2858
str 10015CFC "m_wngR1_jnt_num >= 0" in 021E2858
str 10015D1C "Ac" in 021E2A50
str 10015D20 "d_a_npc_ac1.cpp" in 021E2A50
str 10015D30 "a_mdl_dat != (0)" in 021E2A50
str 10015D14 "handR" in 021E2A50
str 10015D44 "m_hnd_R_jnt_num >= 0" in 021E2A50
str 10015D5C "armL_loc" in 021E2A50
str 10015D68 "m_armL1_jnt_num >= 0" in 021E2A50
str 10015D80 "armR_loc" in 021E2A50
str 10015D8C "m_armR1_jnt_num >= 0" in 021E2A50
str 10015DA4 "Ac" in 021E2C88
str 10015DA8 "d_a_npc_ac1.cpp" in 021E2C88
str 10015DB8 "a_mdl_dat != (0)" in 021E2C88
str 10015DDC "Ac1" in 021E35E4
str 10015DEC "Ac" in 021E3794
str 10015DEF "Ac" in 021E38D4
str 10015B94 "d_demo.h" in 021E3990
str 10015B84 "m_object != (0)" in 021E3990
str 10015DF2 "Ac" in 021E3990
str 10015DF8 "ActNo" in 021E3B84
str 10015E78 "Ac" in 021E43A4
ext 024EF130 s8 Ac1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Ac1_polyColour(dBgS*, cBgS_PolyInfo*)
ext 02055B64 s16 Ac1_calcTimer(be<s16>* t)
ext 025E789C s32 Ac1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool Ac1_lockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* Ac1_lockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* Ac1_actionTarget(dAttention_c* a, s32 i)
ext 0259DED0 void Ac1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 02527028 BOOL Ac1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 02526E70 void* Ac1_demoGetActor(u32 obj, u8 id)
ext 025A1458 void Ac1_npcCtor(daNpc_Ac1_c* p)
ext 025E7820 void Ac1_btpCtor(void* p)
ext 0259F740 void Ac1_eventCutCtor(dNpc_EventCut_c* p)
ext 02018034 void Ac1_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Ac1_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Ac1_actorDestroy(daNpc_Ac1_c* p, s32 flags)
ext 02527828 J3DAnmTexPattern* Ac1_demoBtp(void*,const char*)
ext 02606900 void* Ac1_getNamedResource(dRes_control_c*,SafeString*,SafeString*)
ext 021E2760 u32 Ac1_setBtpResult(daNpc_Ac1_c*,u32,s32)
ext 0259D344 BOOL Ac1_setWingAnimation(mDoExt_McaMorf*,s32,f32,f32,u32,s32,const char*)
ext 025F795C u32 Ac1_messageStatus(void*)
ext 025447C8 BOOL Ac1_eventAdvanced(dEvent_manager_c*,s32)
ext 02543280 void Ac1_cutEnd(dEvent_manager_c*,s32)
*/
