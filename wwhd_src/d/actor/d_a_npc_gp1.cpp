/**
 * d_a_npc_gp1.cpp (WWHD)
 * NPC - Maggie's father (rich)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_gp1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_gp1.h"

// HD: mapped objects are owned once by this translation unit.
const gc_tu_data gc_tu = {.safeString = 0x1001A170};
static daNpc_Gp1_HIO_c& l_HIO = gabi::object_at<daNpc_Gp1_HIO_c>(0x104670A8);
static cXyz& a_eye_pos_off = gabi::object_at<cXyz>(0x104670F8);
static be<u32>& l_eye_guard = gabi::object_at<be<u32>>(0x10467154);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Gp1ActionFunc& l_wait_action = gabi::object_at<Gp1ActionFunc>(0x1001A158);
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static gptr<void>& l_message_manager = gabi::object_at<gptr<void>>(0x101F4B5C);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_third_guard = gabi::object_at<be<u32>>(0x101FDA48);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& l_third_colour = gabi::object_at<be<u32>>(0x101FEBEC);
static be<u32>& a_red_colour = gabi::object_at<be<u32>>(0x1001A160);
static be<u32>& a_blue_colour = gabi::object_at<be<u32>>(0x1001A164);
static be<u32>& a_third_colour = gabi::object_at<be<u32>>(0x1001A168);
using Gp1PrmBytes = be<u8>[0x34];
static Gp1PrmBytes& a_prm_tbl = gabi::object_at<Gp1PrmBytes>(0x101BE670);
static be<s32>& a_btp_resID = gabi::object_at<be<s32>>(0x1001A290);
using Gp1BckIds = be<s32>[10];
static Gp1BckIds& a_bck_resID_tbl = gabi::object_at<Gp1BckIds>(0x1001A2CC);
using Gp1NumParams = daNpc_Gp1_c::anm_prm_c[5];
using Gp1StateParams = daNpc_Gp1_c::anm_prm_c[6];
using Gp1AttrParams = daNpc_Gp1_c::anm_prm_c[7];
static Gp1NumParams& a_anm_num_tbl = gabi::object_at<Gp1NumParams>(0x101BE508);
static Gp1StateParams& a_anm_state_tbl = gabi::object_at<Gp1StateParams>(0x101BE59C);
static Gp1AttrParams& a_anm_attr_tbl = gabi::object_at<Gp1AttrParams>(0x101BE600);
static be<u32>& a_heap_size = gabi::object_at<be<u32>>(0x101BE598);
using Gp1Events = gptr<char>[3];
static Gp1Events& l_evn_tbl = gabi::object_at<Gp1Events>(0x101BE4FC);
static gptr<char>& a_cut_tbl = gabi::object_at<gptr<char>>(0x101BE5FC);
static be<s32>& l_check_wrk = gabi::object_at<be<s32>>(0x10467098);
using Gp1Partners = gptr<fopAc_ac_c>[20];
static Gp1Partners& l_check_inf = gabi::object_at<Gp1Partners>(0x10467104);
static be<u32>& l_counter = gabi::object_at<be<u32>>(0x101FF558);
using Gp1AngleWords = be<u32>[3];
static Gp1AngleWords& a_angle_words = gabi::object_at<Gp1AngleWords>(0x1001A360);
enum Gp1State { GP1_WAIT = 1, GP1_TALK = 2, GP1_ARGUMENT = 3, GP1_WALK = 4, GP1_HAIR = 5 };
enum Gp1MessageStatus {
    GP1_MSG_SELECTING = 2,
    GP1_MSG_TYPING = 6,
    GP1_MSG_DISPLAYED = 14,
    GP1_MSG_CONTINUES = 15,
    GP1_MSG_ENDS = 16,
    GP1_MSG_DESTROYED = 19
};

/* inlined into talk_1 */
static inline void talk_end(daNpc_Gp1_c* i_this) {
    s8 prev = i_this->mPrevStt;
    i_this->mbTalk = false;
    i_this->mItemNo = 0xFF;
    i_this->setStt(prev);
    i_this->mTalkTimer = 0x3C;
    i_this->endEvent();
}

/* 02238460 */
static void* searchActor_Bm(void* i_actor, void*) {
    if ((s32)l_check_wrk < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == 0x149 /* NPC_BM? */) {
        s32 n = (s32)l_check_wrk;
        l_check_wrk = n + 1;
        l_check_inf[n] = static_cast<fopAc_ac_c*>(i_actor);
    }
    return nullptr;
}

/* 02238680 */
static BOOL nodeCallBack_Gp1(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Gp1_c* i_this = static_cast<daNpc_Gp1_c*>(model->getUserArea());
        if (i_this != nullptr)
            i_this->nodeGp1Control(i_node, model);
    }
    return TRUE;
}

/* 022384E0 */
void daNpc_Gp1_c::nodeGp1Control(J3DNode* i_node, J3DModel* i_model) {

    if ((u32)l_eye_guard == 0) {
        a_eye_pos_off.z = 0.0f;
        l_eye_guard = 1;
        a_eye_pos_off.x = 24.0f;
        a_eye_pos_off.y = -24.0f;
    }
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    if (jnt_no == (u32)(s32)m_hed_jnt_num) {
        mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
        PSMTXMultVec(stk, &a_eye_pos_off, &mEyePos);
    }
    if (jnt_no == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02238C88 */
bool daNpc_Gp1_c::init_GP1_0() {
    if (dComIfGs_isEventBit(0x2D01)) {
        Gp1ActionFunc pmf;
        Gp1_loadAction(&pmf, &l_wait_action);
        set_action(&pmf, nullptr);
        return true;
    }
    return false;
}

/* 022391CC */
bool daNpc_Gp1_c::createInit() {

    for (int i = 0; i < 3; i++) {
        const char* name = l_evn_tbl[i].get();
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mLookPos.copy(current.pos);
    u32 path = (mParameters >> 16) & 0xFF;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB; /* attention_info.distances[TALK] */
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[SPEAK] */
    gravity = -4.5f;
    u8 weight = 0xFF;
    if (path != 0xFF) {
        mPathRun.setInf(path, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~fopAcStts_NOCULLEXEC_e; /* OffStatus(fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
    }
    mEventCut.setActorInfo2("Gp1", this);
    mBckNum = 8;
    if (mType != 0) {
        return false;
    }
    if (!init_GP1_0()) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 02239058 */
void daNpc_Gp1_c::setMtx(u32 i_setEyePos) {
    if (mbDemo == 0) {
        plyTexPttrnAnm();
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevMorfFrame) {
            mbMorfAnimStopped = 1;
        }
        mPrevMorfFrame = mpMorf->getFrame();
        ctrl_WAITanm();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = Gp1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Gp1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(mAngle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_setEyePos);
}

/* 02238E00 */
int daNpc_Gp1_c::anmNum_toResID(int i_anmNum) { return (s32)a_bck_resID_tbl[i_anmNum]; }

/* 022388E4 */
int daNpc_Gp1_c::btpNum_toResID(int) {
    // HD: the one-entry resource table ignores the incoming index.
    return (s32)a_btp_resID;
}

/* 022388F0 */
u32 daNpc_Gp1_c::setBtp(u32 i_bModify, int i_btpNum) {
    J3DModelData* md = mpMorf->getModel()->getModelData();
    int id = btpNum_toResID(i_btpNum);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Gp", id);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(438, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_gp1.cpp", 0x1B6, "m_hed_tex_pttrn != (0)");
    bool ok = Gp1_btpInit(mBtpAnm, md, m_hed_tex_pttrn, TRUE, 2, 1.0f, 0, -1, i_bModify, 0) == 1;
    if (ok) {
        mBlinkTimer = 0;
        mBtpFrame = 0;
    }
    return ok;
}

/* 022389DC */
u32 daNpc_Gp1_c::iniTexPttrnAnm(u32 i_bModify) { return setBtp(i_bModify, mBtpNum); }

/* 02238D0C */
void daNpc_Gp1_c::plyTexPttrnAnm() {
    if (mBtpNum == 0 && Gp1_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if ((s32)frame < Gp1_frameMax(m_hed_tex_pttrn.get())) {
        return;
    }
    if (mBtpNum != 0) {
        mBtpFrame = (u8)Gp1_frameMax(m_hed_tex_pttrn.get());
    } else {
        s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
        mBtpFrame = 0;
        mBlinkTimer = t;
    }
}

/* 02238DE0 */
u32 daNpc_Gp1_c::setAnm_tex(s8 i_btpNum) {
    // HD: an unchanged animation returns the original this register.
    if (mBtpNum == i_btpNum) {
        return (u32)gabi::ea(this); /* original: r3 still this */
    }
    mBtpNum = i_btpNum;
    return iniTexPttrnAnm(true);
}

/* 02238E14 */
BOOL daNpc_Gp1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 bck = i_anmPrmP->bckNum;
    if (mBckNum == bck) {
        return TRUE;
    }
    mBckNum = bck;
    int resID = anmNum_toResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    f32 morf = i_anmPrmP->morf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, "Gp");
    if (mBckNum == 0) {
        mWaitTimer = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
    }
    mLoopCnt = 0;
    mPrevMorfFrame = 0.0f;
    mbMorfAnimStopped = 0;
    return TRUE;
}

/* 02238EE0 */
void daNpc_Gp1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    anm_prm_c* a_anm_prm_tbl = a_anm_num_tbl; /* [5] */
    if (i_setBtp) {
        setAnm_tex(a_anm_prm_tbl[i_anmNum].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}

/* 0223992C */
BOOL daNpc_Gp1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = a_anm_state_tbl; /* [6] */
    if (a_anm_prm_tbl[mStt].btpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStt].btpNum);
    }
    if (a_anm_prm_tbl[mStt].bckNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStt]);
    }
    return TRUE;
}

/* 0223A7B8 */
void daNpc_Gp1_c::chg_anmAtr(u8 i_atr) {
    if (i_atr >= 7 || i_atr == mAnmAtr) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR(1);
}

/* 0223AB20 */
void daNpc_Gp1_c::control_anmAtr() {
    switch (mAnmAtr) {
    case 1:
        if (!mbMorfAnimStopped) {
            return;
        }
        break;
    case 5:
        if (!mbMorfAnimStopped) {
            if (mpMorf->checkFrame(80.0f)) {
                create_rupee();
                mDoAud_seStart(0x69E9, nullptr, 0, dComIfGp_getReverb(current.roomNo));
                mRupeeNum = 0;
            }
            return;
        }
        break;
    case 6:
        if (!mbMorfAnimStopped) {
            return;
        }
        mLoopCnt = mLoopCnt + 1;
        if (mLoopCnt < 2) {
            return;
        }
        break;
    default:
        return;
    }
    mAnmAtr = 0;
    setAnm_NUM(0, 1);
}

/* 0223A748 */
void daNpc_Gp1_c::setAnm_ATR(int i_setBtp) {
    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl; /* [7] */
    if (i_setBtp) {
        setAnm_tex(a_anm_prm_tbl[mAnmAtr].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}

/* 0223AC3C */
void daNpc_Gp1_c::anmAtr(u16 i_msgStatus) {
    switch (i_msgStatus) {
    case GP1_MSG_TYPING: {
        if (mAtrSet == 0) {
            mAnmAtr = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        dComIfGp_clearMesgAnimeTagInfo(); // HD: clear the tag even when no animation tag is consumed.
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            mMesgAnimeTag = tag;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case GP1_MSG_DISPLAYED:
        mAtrSet = 0;
        break;
    }
    control_anmAtr();
}

/* 0223A330 */
void daNpc_Gp1_c::eventOrder() {
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(
            this, dEvtCnd_CANTALK_e |
                      dEvtCnd_CANTALKITEM_e); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 0223967C */
void daNpc_Gp1_c::checkOrder() {
    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex])) {
            mEvtState = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}

/* 0223AD04 */
u8 daNpc_Gp1_c::chk_talk() {
    u8 ret = true;
    mItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            mItemNo =
                dComIfGp_event_getPreItemNo(); // HD: the pre-item accessor is evaluated twice.
        } else {
            ret = false;
            mItemNo = dComIfGp_event_getPreItemNo();
        }
    }
    return ret;
}

/* 0223AD98 */
u8 daNpc_Gp1_c::chk_partsNotMove() {
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
           mActorAngleY == current.angle.y;
}

/* 0223ADD8 */
u8 daNpc_Gp1_c::chk_forceTlkArea() {
    cXyz diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, &diff, &current.pos);
    cXyz xz;
    xz.x = diff.x;
    xz.z = diff.z;
    xz.y = 0.0f;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    f32 dy = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
    return -1.0f < dy && !(dy > 100.0f) && dist < l_HIO.mPrm.mForceTlkDist;
}

/* 02239D00 */
void daNpc_Gp1_c::lookBack() {
    mJointBackboneY = m_jnt.mAngles[1][1];
    s16 target_y = current.angle.y;
    cXyz look_pos;
    look_pos.y = 0.0f;
    look_pos.x = 0.0f;
    f32 srcX = current.pos.x;
    look_pos.z = 0.0f;
    mActorAngleY = target_y;
    s8 mode = mLookMode;
    f32 srcY = eyePos.y;
    u8 head_only = mbHeadOnly;
    cXyz* look_pos_p = nullptr;
    f32 srcZ = current.pos.z;
    mJointHeadY = m_jnt.mAngles[0][1];
    switch ((u32)(s32)mode) {
    case 1: {
        cXyz eye;
        Gp1_playerEyePos(&eye, -20.0f);
        look_pos.copy(eye);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        look_pos_p = &look_pos;
        break;
    }
    case 2:
        look_pos.copy(mLookPos);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        look_pos_p = &look_pos;
        break;
    case 3:
        target_y = mLookAngleY;
        break;
    }
    cLib_addCalcAngleS2(&mLookVel, l_HIO.mPrm.mLookVelMax, 4, 0x800);
    if (!m_jnt.mbTrn) {
        mLookVel = 0;
    }
    cXyz cur_pos; /* passed by value: a copy */
    cur_pos.x = srcX;
    cur_pos.y = srcY;
    cur_pos.z = srcZ;
    Gp1_lookAtTarget(&m_jnt, &current.angle.y, look_pos_p, &cur_pos, target_y, mLookVel, head_only);
}

/* 0223AEAC */
u16 daNpc_Gp1_c::next_msgStatus(be<u32>* o_msgNoP) {
    u16 msg_status = GP1_MSG_CONTINUES;
    u32 msgNo = *o_msgNoP;
    u8 num = dComIfGs_getBeastNum(0); /* save info: the number of skull necklaces? */
    switch (msgNo) {
    case 0x1E15:
        *o_msgNoP = 0x1E16;
        break;
    case 0x1E16:
        *o_msgNoP = 0x1E17;
        break;
    case 0x1E17:
        *o_msgNoP = 0x1E18;
        break;
    case 0x1E19:
        *o_msgNoP = 0x1E1A;
        break;
    case 0x1E1A:
        *o_msgNoP = 0x1E1B;
        break;
    case 0x1E1B:
        *o_msgNoP = 0x1E1C;
        break;
    case 0x1E20:
        *o_msgNoP = 0x1E21;
        break;
    case 0x1E21:
        *o_msgNoP = num < 0x14 ? 0x1E30 : 0x1E22;
        break;
    case 0x1E22:
    case 0x1E28:
        *o_msgNoP = 0x1E24;
        mbPresent = 1;
        mBuyNum = 0x14;
        break;
    case 0x1E24:
        *o_msgNoP = 0x1E25;
        break;
    case 0x1E26:
        *o_msgNoP = 0x1E27;
        break;
    case 0x1E29:
        *o_msgNoP = 0x1E2A;
        mbPresent = 1;
        mBuyNum = num;
        break;
    case 0x1E2A:
        if (mBuyNum >= 0x10) {
            *o_msgNoP = 0x1E2D;
            mRupeeNum = 3;
        } else if (mBuyNum >= 6) {
            *o_msgNoP = 0x1E2C;
            mRupeeNum = 2;
        } else {
            *o_msgNoP = 0x1E2B;
            mRupeeNum = 1;
        }
        break;
    case 0x1E30:
        *o_msgNoP = 0x1E23;
        break;
    case 0x1EE6:
        *o_msgNoP = 0x1EE7;
        break;
    default:
        msg_status = GP1_MSG_ENDS;
        break;
    }
    return msg_status;
}

/* 0223B08C */
u32 daNpc_Gp1_c::getMsg_GP1_0() {
    u8 item = mItemNo;
    if (item == 0x45 /* skull necklace shown */) {
        if (dComIfGs_isEventBit(0x1420)) {
            return 0x1E29;
        }
        if (dComIfGs_isEventBit(0x1804)) {
            return dComIfGs_getBeastNum(0) >= 0x14 ? 0x1E28 : 0x1E30;
        }
        return 0x1E20;
    }
    if (item != 0xFF) {
        return 0x1E2F;
    }
    if (mbAfterEvent) {
        mbAfterEvent = 0;
        return 0x1E26;
    }
    if (!dComIfGs_isEventBit(0x1501)) {
        return 0x1E15;
    }
    if (dComIfGs_isEventBit(0x1808)) {
        if (dComIfGs_checkReserveItem(0x9B)) {
            return 0x1E1D;
        }
        return dComIfGs_getEventReg(0xC5FF) >= 0xA ? 0x1E1E : 0x1E1F;
    }
    return dComIfGs_isEventBit(0x1920) ? 0x1E1B : 0x1E19;
}

/* 0223B268 */
u32 daNpc_Gp1_c::getMsg() {
    if (mType == 0) {
        return getMsg_GP1_0();
    }
    return 0;
}

/* 0223B2A0 */
u8 daNpc_Gp1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Gp1_lockonTruth(attention)) {
        return this == (void*)Gp1_lockonTarget(attention, 0);
    }
    return this == (void*)Gp1_actionTarget(attention, 0);
}

/* 02238FFC */
void daNpc_Gp1_c::setAttention(u32 i_setEyePos) {
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 y = current.pos.y + l_HIO.mPrm.mAttPosOffsetY;
    attPos->z = current.pos.z;
    attPos->y = y;
    attPos->x = current.pos.x;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    f32 ey = mEyePos.y + mEyeOffsetY;
    eyePos.x = mEyePos.x;
    eyePos.z = mEyePos.z;
    eyePos.y = ey;
}

/* 02239508 */
fopAc_ac_c* daNpc_Gp1_c::searchByID(u32 i_id) {
    typedef be<u32> ActorResult;
    ActorResult actor;
    actor = 0;
    Gp1_searchById(i_id, &actor);
    return Gp1_actorFromResult(actor);
}

/* 0223953C */
u8 daNpc_Gp1_c::partner_srch_sub(u32 i_func) {
    mPartnerID = 0xFFFFFFFF;
    l_check_wrk = 0;
    for (int i = 0; i < 0x14; i++) {
        l_check_inf[i] = nullptr;
    }
    Gp1_search(i_func, this);
    if ((s32)l_check_wrk == 0) {
        return false;
    }
    fopAc_ac_c* a = l_check_inf[0].get();
    mPartnerID = a != 0 ? fopAcM_GetID(a) /* fopAcM_GetID */ : 0xFFFFFFFF;
    return true;
}

/* 022395E8 */
void daNpc_Gp1_c::partner_srch() {
    if (mActState != 1) {
        return;
    }
    if (mStt == 3) {
        if (!partner_srch_sub(0x02238460 /* searchActor_Bm */)) {
            return;
        }
        fopAc_ac_c* partner = searchByID(mPartnerID);
        if (partner != nullptr) {
            current.angle.y = cLib_targetAngleY(&current.pos, &partner->current.pos);
        }
    }
    mActState = mActState + 1;
}

/* 02238F4C */
void daNpc_Gp1_c::ctrl_WAITanm() {
    switch ((u32)(s32)mBckNum) {
    case 0:
        if (Gp1_calcTimer(&mWaitTimer) == 0) {
            setAnm_NUM(4, 1);
        }
        break;
    case 4:
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(0.0f);
        }
        break;
    }
}

/* 0223A144 */
int daNpc_Gp1_c::gp_movPass() {
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (Gp1_view<Gp1PathFlags>(path)->mClosed & 1) /* dPath_ChkClose(path) */) {
        cXyz pos;
        pos.x = current.pos.x;
        pos.y = current.pos.y;
        pos.z = current.pos.z;
        if (mPathRun.chkPointPass(&pos, mPathRun.mbDir != 0)) {
            mPathRun.nextIdxAuto();
            return 1;
        }
        return 0;
    }
    cXyz diff;
    cXyz_mi(&mPathPos, &diff, &current.pos);
    cXyz xz;
    xz.x = diff.x;
    xz.y = 0.0f;
    xz.z = diff.z;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    int ret = 0;
    if (!(dist > mPassDist)) {
        ret = 1;
        if (mPathRun.mPath.get() != nullptr && !mPathRun.nextIdxAuto()) {
            ret = 2;
        }
    }
    return ret;
}

/* 0223A0E0 */
void daNpc_Gp1_c::gp_clcMovSpd() {
    s16 angle = cLib_targetAngleY(&current.pos, &mPathPos);
    cLib_addCalcAngleS(&current.angle.y, angle, l_HIO.mPrm.mTurnScale, l_HIO.mPrm.mTurnMaxStep, 0);
    cLib_chaseF(&speedF, mWalkSpd, mWalkAccel);
}

/* 0223A244 */
void daNpc_Gp1_c::gp_nMove() {
    if (mbPathEnd) {
        mbNoMove = 1;
        return;
    }
    if (mMoveState != 1) {
        return;
    }
    gp_clcMovSpd();
    f32 rate = speedF * l_HIO.mPrm.mAnmSpdRate;
    if (rate < 0.5f) {
        rate = 0.5f;
    }
    mpMorf->setPlaySpeed(rate);
    switch (gp_movPass()) {
    case 1:
        mbPathEnd = 1;
        break;
    case 2:
        mbPathEnd = 1;
        mMoveState = 0;
        break;
    }
}

/* 0223A7DC */
BOOL daNpc_Gp1_c::create_rupee() {
    // HD: keep the angle table adjacent to pos; negative counter remainders alias pos.y/z.
    csXyz angle;
    csXyz_ct(&angle, 0, 0, 0);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_hnd_L_jnt_num), stk);
    Gp1RupeeFrame frm;
    frm.pos.x = stk->m[0][3];
    frm.pos.y = stk->m[1][3];
    frm.pos.z = stk->m[2][3];
    s32 cnt = (s32)(u32)l_counter;
    for (int i = 0; i < mRupeeNum; i++) {
        auto* tbl = Gp1_view<be<u32>>(&frm.a_angle_tbl[0]);
        tbl[0] = a_angle_words[0];
        tbl[1] = a_angle_words[1];
        tbl[2] = a_angle_words[2];
        f32 r = cM_rndF(30.0f);
        f32 base = Gp1_rupeeAngle(&frm, cnt % 3);
        s16 deg = (s16)gabi::ftoi((r - 15.0f) + base);
        f32 a = (f32)deg * 182.04445f; /* cM_deg2s */
        angle.y = (s16)(current.angle.y + (s16)gabi::ftoi(a));
        f32 spd = cM_rndFX(3.0f) + 10.0f;
        f32 spdY = cM_rndFX(6.0f) + 33.0f;
        fopAc_ac_c* item = Gp1_createItem(&frm.pos, 4 /* dItem_BLUE_RUPEE_e */, current.roomNo,
                                          nullptr, nullptr, spd, spdY, -2.0f, -1, 0);
        if (item == nullptr) {
            break;
        }
        item->actor_status |= fopAcStts_UNK4000_e;
        item->scale.x = 0.2f;
        item->scale.y = 0.2f;
        item->scale.z = 0.2f;
        item->current.angle.x = angle.x;
        item->current.angle.y = angle.y;
        item->current.angle.z = angle.z;
        item->shape_angle.x = angle.x;
        item->shape_angle.y = angle.y;
        item->shape_angle.z = angle.z;
        cnt++;
    }
    return TRUE;
}

/* 02238B48 */
BOOL daNpc_Gp1_c::charDecide(int) {
    mType = 0;
    mSubtype = 0;
    return TRUE;
}

/* 02239B0C */
void daNpc_Gp1_c::eInit_INI_KAERE_KAERE_() {
    fopAc_ac_c* partner = searchByID(mPartnerID);
    if (partner != nullptr) {
        dEvt_control_c* evt = &Gp1_view<dComIfG_inf_c>(dComIfGp_get())->play.mEvtCtrl;
        evt->mPtItem = Gp1_eventPid(evt, partner); /* dComIfGp_event_setPt2 */
    }
}

/* 02239B60 */
void daNpc_Gp1_c::eInit_END_KAERE_KAERE_() { current.angle.y = mHomeAngle.y; }

/* 02239B6C */
void daNpc_Gp1_c::event_actionInit(int i_staffIdx) {
    be<s32>* act = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "ActNo" /* "ActNo" */, 3);
    if (act == nullptr) {
        return;
    }
    s8 actNo = (s8)(s32)*act;
    mActNo = actNo;
    mbSetEyePos = 0;
    switch ((u32)(s32)actNo) {
    case 0:
        eInit_INI_KAERE_KAERE_();
        break;
    case 1:
        eInit_END_KAERE_KAERE_();
        break;
    }
}

/* 02239C24 */
BOOL daNpc_Gp1_c::event_action() { return TRUE; }

/* 02239C2C */
void daNpc_Gp1_c::privateCut(int i_staffIdx) {

    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, &a_cut_tbl, 1, TRUE, 0);
    mActionIndex = idx;
    if (idx == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        if (mActionIndex == 0) {
            event_actionInit(i_staffIdx);
        }
    }
    BOOL end = TRUE;
    if (mActionIndex == 0) {
        end = event_action();
    }
    if (end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 02239ACC */
void daNpc_Gp1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}

/* 022398EC */
s32 daNpc_Gp1_c::isEventEntry() {
    const char* name = Gp1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 02239F40 */
void daNpc_Gp1_c::event_proc(int i_staffIdx) {
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0:
            dComIfGs_onEventBit(0x1808);
            actor_status |= fopAcStts_NOCULLEXEC_e; /* OnStatus(fopAcStts_NOCULLEXEC_e) */
            setStt(GP1_WAIT);
            mWaitTimer2 = 0;
            mHairTimer = (s16)(((u32)l_counter & 3) + 2);
            break;
        case 1:
            dComIfGs_onEventBit(0x1420);
            mbAfterEvent = 1;
            mEvtState = 1;
            break;
        case 2:
            setStt(GP1_WAIT);
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
        lookBack();
    }
}

/* 02238B5C */
BOOL daNpc_Gp1_c::set_action(Gp1ActionFunc* i_newProcFunc, void* i_argsP) {
    Gp1ActionFunc* cur = &mCurrProcFunc;
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
        Gp1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Gp1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 022399B0 */
void daNpc_Gp1_c::setStt(s8 i_stt) {
    s8 prev = mStt;
    mStt = i_stt;
    mTalkTimer = 0;
    switch ((u32)(s32)i_stt) {
    case GP1_WAIT:
        mWaitTimer2 = 0x5A;
        break;
    case GP1_TALK:
        m_jnt.mbTrn = 1; /* setTrn() */
        mAnmAtr = 0xFF;
        mLookMode = 1;
        mPrevStt = prev;
        return;
    case GP1_WALK: {
        mLookMode = 0;
        mMoveState = 1;
        mWaitTimer2 = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
        mWalkSpd = l_HIO.mPrm.mWalkSpd;
        mWalkAccel = l_HIO.mPrm.mWalkAccel;
        mPassDist = l_HIO.mPrm.mPassDist;
        break;
    }
    case GP1_HAIR:
        mLookMode = 0;
        break;
    }
    setAnm();
}

/* 0223B328 */
BOOL daNpc_Gp1_c::wait_1() {
    mWaitTimer = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
    if (mbTalk && mBckNum != 4) {
        if (chk_talk()) {
            setStt(GP1_TALK);
            mbHeadOnly = false;
        }
        return TRUE;
    }
    if (mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    if (chk_forceTlkArea() || mbAttention) {
        mTalkTimer = 0x3C;
    }
    if (Gp1_calcTimer(&mTalkTimer)) {
        mLookMode = 1;
        return TRUE;
    }
    if (Gp1_calcTimer(&mWaitTimer2) == 0) {
        setStt(GP1_WALK);
        return TRUE;
    }
    mLookMode = 2;
    m_jnt.mbTrn = 1; /* setTrn() */
    mLookPos.x = -2.0f;
    mLookPos.y = eyePos.y;
    mLookPos.z = 940.0f;
    return TRUE;
}

/* 0223B47C */
u8 daNpc_Gp1_c::talk_1() {
    u8 res = chk_partsNotMove();
    talk(1);
    if (!mbHasMsg) {
        return res;
    }
    // HD: status comes from the message manager.
    u32 status = Gp1_messageStatus(l_message_manager.get());
    if (status == GP1_MSG_SELECTING || status == GP1_MSG_TYPING) {
        if (mbPresent) {
            dComIfGp_evmng_CancelPresent();
            if (mBuyNum != 0) {
                /* hand the necklaces over: event register 0xC5FF counts them (up to 127) */
                u8 reg = dComIfGs_getEventReg(0xC5FF);
                s16 n = mBuyNum;
                dComIfGp_setItemBeastNumCount(0, (s16)-n);
                s32 total = reg + mBuyNum;
                if (total > 0x7F) {
                    total = 0x7F;
                }
                dComIfGs_setEventReg(0xC5FF, (u8)total);
            }
            mbPresent = 0;
        }
    } else if (status == GP1_MSG_DESTROYED) {
        u32 msgNo = mCurrMsgNo;
        switch (msgNo) {
        case 0x1E18:
            dComIfGs_onEventBit(0x1501);
            break;
        case 0x1E1C:
            dComIfGs_onEventBit(0x1920);
            break;
        case 0x1E23:
            dComIfGs_onEventBit(0x1804);
            break;
        case 0x1E25:
            mEvtState = 4;
            break;
        case 0x1E2B:
        case 0x1E2C:
        case 0x1E2D:
            mEvtState = 5;
            break;
        }
        talk_end(this);
    }
    return res;
}

/* 0223B740 */
BOOL daNpc_Gp1_c::walk_1() {
    if (mbTalk || chk_forceTlkArea() || Gp1_calcTimer(&mWaitTimer2) == 0) {
        mWalkSpd = 0.0f;
    }
    if (mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    if (mbPathEnd) {
        mbPathEnd = 0;
        if (mMoveState == 0) {
            mPathRun.mIdx = 0;
            mMoveState = 1;
        }
    }
    cXyz pnt;
    mPathRun.getPoint(&pnt, mPathRun.mIdx);
    mPathPos.copy(pnt);
    if (gabi::ftoi(mWalkSpd) == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk || Gp1_calcTimer(&mHairTimer) != 0) {
            setStt(GP1_WAIT);
            mWalkAccel = 0.0f;
            mMoveState = 0;
            speedF = 0.0f;
        } else {
            setStt(GP1_HAIR);
            mWalkAccel = 0.0f;
            mMoveState = 0;
            speedF = 0.0f;
            mHairTimer = (s16)(((u32)l_counter & 3) + 2);
        }
    }
    return TRUE;
}

/* 0223B8D4 */
BOOL daNpc_Gp1_c::hair_1() {
    u8 talk = mbTalk;
    if (mbMorfAnimStopped) {
        if (talk || chk_forceTlkArea()) {
            setStt(GP1_WAIT);
        } else {
            setStt(GP1_WALK);
        }
        return TRUE;
    }
    if (!talk && mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    return TRUE;
}

/* 0223B978 */
BOOL daNpc_Gp1_c::wait_2() {
    if (mEvtState != 1 && mEvtState < 3) {
        f32 dy = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
        if (dy > -1.0f) {
            mEvtState = 3;
        } else {
            mEvtState = 0;
        }
    }
    return TRUE;
}

/* 0223B9F8 */
BOOL daNpc_Gp1_c::wait_action1(void*) {
    s8 state = mActState;
    if (state == 0) {
        if (dNpc_chkArasoi()) {
            actor_status &= ~fopAcStts_NOCULLEXEC_e;
            setStt(GP1_ARGUMENT);
        } else {
            actor_status |= fopAcStts_NOCULLEXEC_e;
            setStt(GP1_WAIT);
            mHairTimer = (s16)(((u32)l_counter & 3) + 2);
        }
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case GP1_WAIT:
        mbSetEyePos = wait_1();
        break;
    case GP1_TALK:
        mbSetEyePos = talk_1();
        break;
    case GP1_ARGUMENT:
        mbSetEyePos = wait_2();
        break;
    case GP1_WALK:
        mbSetEyePos = walk_1();
        break;
    case GP1_HAIR:
        mbSetEyePos = hair_1();
        break;
    }
    if (mEvtState != 1 && mEvtState < 3 && !dComIfGs_isEventBit(0x1501) && !mbTalk &&
        chk_forceTlkArea()) {
        mEvtState = 1;
    }
    lookBack();
    return TRUE;
}

/* 02239738 */
u8 daNpc_Gp1_c::demo() {
    if (demoActorID == 0) {
        if (mbDemo) {
            mbDemo = false;
        }
        return mbDemo;
    }
    u8 id = demoActorID;
    mbDemo = true;
    // HD: demo actor lookup is inlined with the same range check.
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = (u32)l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demo_object;
        }
        demo_actor_p = Gp1_demoGetActor(obj, id);
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= Gp1_frameMax(m_hed_tex_pttrn.get())) {
            mBtpFrame = (u8)Gp1_frameMax(m_hed_tex_pttrn.get());
        }
    }
    if (demo_actor_p) {
        J3DAnmTexPattern* demo_btp_p = Gp1_demoBtp(demo_actor_p, "Gp"); /* getP_BtpData */
        if (demo_btp_p) {
            m_hed_tex_pttrn = (J3DAnmTexPattern*)demo_btp_p;
            if (Gp1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), demo_btp_p, TRUE, 2, 1.0f,
                            0, -1, 1, 0)) {
                mBtpFrame = 0;
                mBtpNum = 1;
            }
        }
    }
    Gp1_demoData(this, 0x6A, mpMorf.get(), "Gp", 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}

/* 0223A5AC */
BOOL daNpc_Gp1_c::_draw() {
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = morf_model_p->getModelData();
    if (mbHidden || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    // HD: retain the otherwise unused accessor call.
    dComIfGp_get();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    Gp1_view<Gp1ModelAnimation>(morf_model_info_p)->mpBtpAnimation = 0; /* mBtpAnm.remove() */
    // HD: preserve guarded debug colours, including the repeated third check.
    if (l_HIO.mPrm.mDebugDraw) {
        Gp1_initDebugColour(&l_red_guard, &l_red_colour, &a_red_colour);
        Gp1_initDebugColour(&l_blue_guard, &l_blue_colour, &a_blue_colour);
        if ((u32)l_third_guard == 0) {
            l_third_guard = 1;
            memcpy_g(&l_third_colour, &a_third_colour, 4);
            Gp1_initDebugColour(&l_third_guard, &l_third_colour, &a_third_colour);
        }
    }
    // HD: shadowDraw is absent.
    dSnap_RegistFig(0x5B /* DSNAP_TYPE_NPC_GP1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 0223A3A0 */
BOOL daNpc_Gp1_c::_execute() {
    if (!mbHomeSet) {
        mHomeAngle.y = current.angle.y;
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Gp1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mbHidden && demoActorID == 0) {
        return TRUE;
    }
    mbNoMove = 0;
    mbHidden = 0;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() &&
            (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            Gp1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!mbNoMove) {
            gp_nMove();
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }
        if (!mbNoShapeAngle) {
            mAngle.x = current.angle.x;
            mAngle.y = current.angle.y;
            mAngle.z = current.angle.z;
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(false);
    if (!mbDemo) {
        setCollision(70.0f, 230.0f);
    }
    return TRUE;
}

/* 022394AC */
BOOL daNpc_Gp1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Gp");
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 02238B44 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Gp1_c*>(i_this)->CreateHeap();
}

/* 0223936C */
cPhs_State daNpc_Gp1_c::_create() {
    // HD: install this TU's vtable and construct the enlarged animation/event objects.
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Gp1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001A388;
            Gp1_btpCtor(mBtpAnm);         /* mDoExt_btpAnm::mDoExt_btpAnm */
            Gp1_eventCutCtor(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Gp");
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 50.0f, 240.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 022386C8 */
J3DModelData* daNpc_Gp1_c::create_Anm() {
    const char* arc = "Gp";
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(arc, 6);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2169, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_gp1.cpp", 0x879, "a_mdl_dat != (0)");
    void* bck = dComIfG_getObjectIDRes(arc, 5);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, (J3DAnmTransform*)bck, 2,
                                    1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gp1.cpp", 0x88E, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone");
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gp1.cpp", 0x891, "m_bbone_jnt_num >= 0");
    m_hnd_L_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("handL");
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_gp1.cpp", 0x894, "m_hnd_L_jnt_num >= 0");
    return a_mdl_dat;
}

/* 022389E8 */
BOOL daNpc_Gp1_c::CreateHeap() {
    J3DModelData* a_mdl_dat = create_Anm();
    if (a_mdl_dat == nullptr) {
        return FALSE;
    }
    mBtpNum = 0;
    if (!iniTexPttrnAnm(false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    for (u16 i = 0; i < a_mdl_dat->getJointNum(); i++) {
        if (i == (u32)(s32)m_hed_jnt_num || i == (u32)(s32)m_bbone_jnt_num) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Gp1) */
            J3DModelData* md = mpMorf->getModel()->getModelData();
            md->getJointNodePointer(i)->setCallBack(nodeCallBack_Gp1);
        }
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 70.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 022394A8 */
static cPhs_State daNpc_Gp1_Create(fopAc_ac_c* i_this) { return ((daNpc_Gp1_c*)i_this)->_create(); }

/* 02239504 */
static BOOL daNpc_Gp1_Delete(daNpc_Gp1_c* i_this) { return i_this->_delete(); }

/* 0223A5A8 */
static BOOL daNpc_Gp1_Execute(daNpc_Gp1_c* i_this) { return i_this->_execute(); }

/* 0223A73C */
static BOOL daNpc_Gp1_Draw(daNpc_Gp1_c* i_this) { return i_this->_draw(); }

/* 0223A740 */
static BOOL daNpc_Gp1_IsDelete(daNpc_Gp1_c*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 0223BBC4 */
static daNpc_Gp1_HIO_c* daNpc_Gp1_HIO_c_ct(daNpc_Gp1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Gp1_HIO_c*)operator_new(0x40);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001A1A8;
    memcpy_g(&i_this->mPrm, &a_prm_tbl, 0x34); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->mEntryID = -1;
    return i_this;
}

/* 0223BC30 */
static void __sinit_d_a_npc_gp1_cpp() {
    sinit_header_statics_z(0x1046709C, 0x101BE6A4, 0x104670E8);
    daNpc_Gp1_HIO_c_ct(&l_HIO); /* static daNpc_Gp1_HIO_c l_HIO */
}

/* 0223BCD0 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0223BCE4 */
static void daNpc_Gp1_c_dt(daNpc_Gp1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Gp1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Gp1_view<Gp1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001A188;
        Gp1_view<Gp1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001A198;
        Gp1_acchDestroy(&i_this->mObjAcch, 0);
        Gp1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0223BD80 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 1001A210 "Gp" in 022386C8
str 1001A21C "d_a_npc_gp1.cpp" in 022386C8
str 1001A22C "a_mdl_dat != (0)" in 022386C8
str 1001A214 "head" in 022386C8
str 1001A240 "m_hed_jnt_num >= 0" in 022386C8
str 1001A254 "backbone" in 022386C8
str 1001A260 "m_bbone_jnt_num >= 0" in 022386C8
str 1001A208 "handL" in 022386C8
str 1001A278 "m_hnd_L_jnt_num >= 0" in 022386C8
str 1001A294 "Gp" in 022388F0
str 1001A298 "d_a_npc_gp1.cpp" in 022388F0
str 1001A2A8 "m_hed_tex_pttrn != (0)" in 022388F0
str 1001A2F4 "Gp" in 02238E14
str 1001A2FC "Gp1" in 022391CC
str 1001A310 "Gp" in 0223936C
str 1001A313 "Gp" in 022394AC
str 1001A1EC "d_demo.h" in 02239738
str 1001A1B8 "m_object != (0)" in 02239738
str 1001A316 "Gp" in 02239738
str 1001A31C "ActNo" in 02239B6C
ext 024EF130 s8 Gp1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Gp1_polyColour(dBgS*, cBgS_PolyInfo*)
ext 02055B64 s16 Gp1_calcTimer(be<s16>*)
ext 025E789C s32 Gp1_btpInit(void*,J3DModelData*,J3DAnmTexPattern*,s32,s32,f32,s32,s32,u32,s32)
ext 024EDFCC bool Gp1_lockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* Gp1_lockonTarget(dAttention_c*,s32)
ext 024EE464 fopAc_ac_c* Gp1_actionTarget(dAttention_c*,s32)
ext 0259D54C void Gp1_playerEyePos(cXyz*,f32)
ext 025F795C u32 Gp1_messageStatus(void*)
ext 0259DED0 void Gp1_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 025DE508 void* Gp1_search(u32,void*)
ext 025D54C4 BOOL Gp1_searchById(u32,be<u32>*)
ext 0253F124 u32 Gp1_eventPid(dEvt_control_c*,fopAc_ac_c*)
ext 025D8AB0 fopAc_ac_c* Gp1_createItem(cXyz*,s32,s32,csXyz*,cXyz*,f32,f32,f32,s32,u32)
ext 025A1458 void Gp1_npcCtor(daNpc_Gp1_c*)
ext 025E7820 void Gp1_btpCtor(void*)
ext 0259F740 void Gp1_eventCutCtor(dNpc_EventCut_c*)
ext 02526E70 void* Gp1_demoGetActor(u32,u8)
ext 02527828 J3DAnmTexPattern* Gp1_demoBtp(void*,const char*)
ext 02527028 void Gp1_demoData(daNpc_Gp1_c*,u32,mDoExt_McaMorf*,const char*,s32,u32,u32,s32)
ext 02018034 void Gp1_circleDestroy(void*,s32)
ext 024EFD9C void Gp1_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Gp1_actorDestroy(daNpc_Gp1_c*,s32)
*/
