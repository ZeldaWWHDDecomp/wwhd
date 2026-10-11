/**
 * d_a_npc_ob1.cpp (WWHD)
 * NPC - Rose (Outset; counts the pigs)
 *
 * The GameCube decompilation of this TU is
 * "Nonmatching" stubs (zeldaret/tww src/d/actor/d_a_npc_ob1.cpp): every function here is written
 * from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names. The common
 * NPC structure (jnt control, event cuts, talk) follows d_a_npc_km1.cpp.
 */
#include "d/actor/d_a_npc_ob1.h"

enum Ob1State : s8 {
    Ob1Wait = 1,
    Ob1Talk = 2,
    Ob1Walk = 3,
    Ob1WaitPath = 4,
    Ob1WaitAnimation = 5,
    Ob1PartnerTalk = 6
};
enum : s16 { Ob1PigProcess = 0xDC };
enum : u16 { Ob1MessageTyping = 6, Ob1MessageDisplayed = 0xE, Ob1MessageBoxClosed = 0x13 };

const gc_tu_data gc_tu = {.safeString = 0x1001F080, .objAcchDt = {0x1001F098, 0x1001F0A8}};
static BOOL nodeCallBack_Ob1(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
#define OB1_VTBL 0x1001F238
#define HIO_VTBL 0x1001F0B8
static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);

static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_headerZero = gabi::object_at<be<u32>[4]>(0x10467D40);
static auto& l_headerMin = gabi::object_at<be<f32>>(0x10467D34);
static auto& l_headerMax = gabi::object_at<be<f32>>(0x10467D38);
static auto& l_headerByte0 = gabi::object_at<be<u8>>(0x10467D3D);
static auto& l_headerByte1 = gabi::object_at<be<u8>>(0x10467D3E);
static auto& a_hio_prm = gabi::object_at<be<u32>[12]>(0x101C27D0);
static auto& l_debugGuard0 = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColour0 = gabi::object_at<be<u32>>(0x101FEBF4);
static auto& l_debugSource0 = gabi::object_at<be<u32>>(0x1001F070);
static auto& l_debugGuard1 = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_debugColour1 = gabi::object_at<be<u32>>(0x101FEBF8);
static auto& l_debugSource1 = gabi::object_at<be<u32>>(0x1001F074);
static auto& l_debugGuard2 = gabi::object_at<be<u32>>(0x101FDA48);
static auto& l_debugColour2 = gabi::object_at<be<u32>>(0x101FEBEC);
static auto& l_debugSource2 = gabi::object_at<be<u32>>(0x1001F078);
static auto& l_pig_cnt = gabi::object_at<be<s32>>(0x10467D30);
static auto& l_pig_bits = gabi::object_at<be<u8>>(0x10467D3C);
static auto& l_pig_tbl = gabi::object_at<gptr<fopAc_ac_c>[20]>(0x10467D98);
static auto& l_HIO = gabi::object_at<daNpc_Ob1_HIO_c>(0x10467D50);
static auto& l_eyeGuard = gabi::object_at<be<u32>>(0x10467DE8);
static auto& a_eye_pos_offst = gabi::object_at<cXyz>(0x10467D8C);
static auto& a_head_resID = gabi::object_at<be<s32>>(0x101C260C);
static auto& a_heap_size_tbl = gabi::object_at<be<u32>>(0x101C2610);
static auto& a_btp_resID_tbl = gabi::object_at<be<s32>[2]>(0x1001F194);
static auto& a_anm_resID_tbl = gabi::object_at<be<s32>[8]>(0x1001F208);
static auto& l_evn_tbl = gabi::object_at<gptr<char>>(0x101C25E8);
static auto& cut_name_tbl = gabi::object_at<gptr<char>>(0x101C2614);
static auto& a_anm_num_prm = gabi::object_at<daNpc_Ob1_c::anm_prm_c[8]>(0x101C2618);
static auto& a_anm_stt_prm = gabi::object_at<daNpc_Ob1_c::anm_prm_c[7]>(0x101C26B8);
static auto& a_anm_atr_prm = gabi::object_at<daNpc_Ob1_c::anm_prm_c[7]>(0x101C2744);
static auto& l_action1 = gabi::object_at<Ob1ActionFunc>(0x1001F060);
static auto& l_action2 = gabi::object_at<Ob1ActionFunc>(0x1001F068);

/* inlined into init_OB1_0, init_OB1_1, init_OB1_2 */
static inline void set_action_tbl(daNpc_Ob1_c* i_this, Ob1ActionFunc* entry) {
    Ob1ActionFunc f;
    Ob1_view<Ob1ActionWords>(gabi::ea(&f))->mWords[0] =
        Ob1_view<Ob1ActionWords>(gabi::ea(entry))->mWords[0];
    Ob1_view<Ob1ActionWords>(gabi::ea(&f))->mWords[1] =
        Ob1_view<Ob1ActionWords>(gabi::ea(entry))->mWords[1];
    i_this->set_action(&f, nullptr);
}

/* 022A98F8 */
// HD: preserve the allocating constructor and its pointer return.
static daNpc_Ob1_HIO_c* daNpc_Ob1_HIO_ct(daNpc_Ob1_HIO_c* p) {
    if (p == nullptr) {
        p = (daNpc_Ob1_HIO_c*)operator_new(0x3C);
        if (p == nullptr)
            return nullptr;
    }
    p->__vtbl = HIO_VTBL;
    Ob1_copyBytes(gabi::ea(&p->mPrm), gabi::ea(&a_hio_prm), 0x30);
    p->mNo = -1;
    p->mEntryID = -1;
    return p;
}

/* 022A6038 */
static void* searchActor_Kb(void* p, void*) {
    if (l_pig_cnt < 20 && fopAc_IsActor(p) && p != nullptr &&
        fpcM_GetName(p) == Ob1PigProcess /* PROC_KB */) {
        u8 bit = Ob1_view<Ob1Pig>(gabi::ea(p))->mPigBit; /* kb_class::m405: the pig's bit */
        if (bit != 0) {
            l_pig_bits |= bit;
        }
        s32 n = l_pig_cnt;
        l_pig_cnt = n + 1;
        l_pig_tbl[n] = (fopAc_ac_c*)p;
    }
    return nullptr;
}

/* 022A6274 */
static BOOL nodeCallBack_Ob1(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ob1_c* i_this = (daNpc_Ob1_c*)model->getUserArea(); /* getUserArea */
        if (i_this != nullptr) {
            i_this->nodeOb1Control(node, model);
        }
    }
    return TRUE;
}

/* 022A60D4 */
void daNpc_Ob1_c::nodeOb1Control(J3DNode* node, J3DModel* model) {
    /* static cXyz a_eye_pos_offst(20.0f, -20.0f, 0.0f) */
    if (l_eyeGuard == 0) {
        a_eye_pos_offst.z = 0.0f;
        l_eyeGuard = 1;
        a_eye_pos_offst.x = 20.0f;
        a_eye_pos_offst.y = -20.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(node);
    u16 jntNo = Ob1_view<Ob1Joint>(gabi::ea(joint))->mJointNo;
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(jntNo), stack);
    if (jntNo == m_hed_jnt_num) {
        mDoMtx_YrotM(stack, (s16)-m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[0][0]);
        PSMTXMultVec(stack, &a_eye_pos_offst, &mEyePos);
    }
    if (jntNo == m_bbone_jnt_num) {
        mDoMtx_XrotM(stack, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(stack, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stack, j3dSys.mCurrentMtx);
    mtx_copy(model->getAnmMtx(jntNo), stack); /* model->setAnmMtx(jntNo, stack) */
}

/* 022A6934 */
BOOL daNpc_Ob1_c::init_OB1_0() {
    if (!dComIfGs_isEventBit(0x520)) {
        set_action_tbl(this, &l_action1); /* &daNpc_Ob1_c::wait_action1 */
        return TRUE;
    }
    return FALSE;
}

/* 022A69B8 */
BOOL daNpc_Ob1_c::init_OB1_1() {
    if (dComIfGs_isEventBit(0x520) && !dKy_daynight_check()) {
        actor_status &= ~0x80u;
        set_action_tbl(this, &l_action2); /* &daNpc_Ob1_c::wait_action2 */
        return TRUE;
    }
    return FALSE;
}

/* 022A6A54 */
BOOL daNpc_Ob1_c::init_OB1_2() {
    if (dComIfGs_isEventBit(0x520) && dKy_daynight_check() == 1) {
        set_action_tbl(this, &l_action2); /* &daNpc_Ob1_c::wait_action2 */
        return TRUE;
    }
    return FALSE;
}

/* 022A6E10 */
BOOL daNpc_Ob1_c::createInit() {
    const char* evName = l_evn_tbl.get(); /* l_evn_tbl[0] "Get_Rupee" */
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(evName, 0xFF);
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    mHomeAngle.x = current.angle.x;
    mHomeAngle.y = current.angle.y;
    mHomeAngle.z = current.angle.z;
    mHomePos.copy(current.pos);
    gravity = -4.5f;
    mLookPos.copy(current.pos);
    attention_info.distances[3] = 0xAC; /* attention_info.distances[3] */
    attention_info.distances[1] = 0xAC; /* attention_info.distances[1] */
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e |
        fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        mPathRun.setInf(pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return FALSE;
        }
        actor_status &= ~0x80u;
        weight = 0xD9;
    }
    setActorInfo2(&mEventCut2, "Ob1", this);
    mAnmNo = 8;
    switch (mType) {
    case 0:
        if (!init_OB1_0())
            return FALSE;
        break;
    case 1:
        if (!init_OB1_1())
            return FALSE;
        break;
    case 2:
        if (!init_OB1_2())
            return FALSE;
        break;
    default:
        return FALSE;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src);
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return TRUE;
}

/* 022A6C0C */
void daNpc_Ob1_c::setMtx(s32 force) {
    if (mbDemo == 0) {
        plyTexPttrnAnm();
        mAnmEnd = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevFrame) {
            mAnmEnd = 1;
        }
        mPrevFrame = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mRoomNo */
    tevStr.mEnvrIdxOverride =
        (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(mAngle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    /* mpHedModel->setBaseTRMtx(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num)) */
    Mtx34* src = mpMorf->getModel()->getAnmMtx(m_hed_jnt_num);
    J3DModel_setBaseTRMtx(mpHedModel, src);
    J3DModel_calc(mpHedModel);
    setAttention(force);
}

/* 022A7F50 */
s32 daNpc_Ob1_c::anmNum_toResID(int num) { return a_anm_resID_tbl[num]; /* a_anm_resID_tbl */ }

/* 022A6540 */
s32 daNpc_Ob1_c::btpNum_toResID(int num) { return a_btp_resID_tbl[num]; /* a_btp_resID_tbl */ }

/* 022A6554 */
BOOL daNpc_Ob1_c::setBtp(s32 modify, int num) {
    u32 modelData = J3DModel_modelData(mpHedModel);
    s32 resID = btpNum_toResID(num);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Ob", resID);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(0x202, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_ob1.cpp", 0x202, "m_hed_tex_pttrn != (0)");
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_hed_tex_pttrn, 1, 2 /* EMode_LOOP */, 1.0f, 0,
                            -1, modify, 0)) {
        return FALSE;
    }
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022A6658 */
BOOL daNpc_Ob1_c::iniTexPttrnAnm(s32 modify) { return setBtp(modify, mTexNo); }

/* 022A6AE4 */
void daNpc_Ob1_c::plyTexPttrnAnm() {
    if (mTexNo == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if ((s32)frame < J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn)) {
        return;
    }
    if (mTexNo != 0) {
        mBtpFrame = (u8)J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn);
    } else {
        s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
        mBtpFrame = 0;
        mBlinkTimer = t;
    }
}

/* 022A7F64 */
u32 daNpc_Ob1_c::setAnm_tex(s8 tex) {
    if (mTexNo != tex) {
        mTexNo = tex;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022A7F84 */
BOOL daNpc_Ob1_c::setAnm_anm(anm_prm_c* prm) {
    s8 anmNo = prm->mAnmNo;
    if (mAnmNo != anmNo) {
        mAnmNo = anmNo;
        s32 resID = anmNum_toResID(anmNo);
        s32 loop = prm->mLoopMode;
        f32 speed = prm->mSpeed;
        f32 morf = prm->mMorf;
        dNpc_setAnmIDRes(mpMorf, loop, morf, speed, resID, -1, "Ob");
        mLoopCnt = 0;
        mPrevFrame = 0.0f;
        mAnmEnd = 0;
    }
    return TRUE;
}

/* 022A8018 */
void daNpc_Ob1_c::setAnm_NUM(int num, int withTex) {
    /* a_anm_prm_tbl (101C2618) */
    if (withTex) {
        setAnm_tex((&a_anm_num_prm[num])->mTexNo);
    }
    setAnm_anm((&a_anm_num_prm[num]));
}

/* 022A8084 */
BOOL daNpc_Ob1_c::setAnm() {
    /* a_anm_prm_tbl (101C26B8), by state */
    if ((&a_anm_stt_prm[mStt])->mTexNo >= 0) {
        setAnm_tex((&a_anm_stt_prm[mStt])->mTexNo);
    }
    if ((&a_anm_stt_prm[mStt])->mAnmNo >= 0) {
        setAnm_anm((&a_anm_stt_prm[mStt]));
    }
    return TRUE;
}

/* 022A8178 */
void daNpc_Ob1_c::chg_anmAtr(u8 atr) {
    u32 msgNo = mMsgNo;
    if (msgNo != 0xFFFFFFFF) {
        switch (msgNo) {
        case 0xAFC:
        case 0xAFD:
        case 0xAFF:
        case 0xB02:
        case 0xB05:
        case 0xB06:
        case 0xB09:
        case 0xB0C:
        case 0xB0D:
        case 0xB5D:
        case 0xB61:
        case 0xB64:
        case 0xB65:
            m_jnt.mbTrn = 1;
            mbHeadOnly = 0;
            mLookMode = 1;
            break;
        }
    }
    if (atr < 7 && atr != mAnmAtr) {
        mAnmAtr = atr;
        setAnm_ATR(1);
    }
}

/* 022A8244 */
void daNpc_Ob1_c::control_anmAtr() {
    switch (mAnmAtr) {
    case 3:
        if (mAnmEnd != 0) {
            setAnm_NUM(4, 1);
            mAnmAtr = 7;
        }
        break;
    case 6:
        if (mAnmEnd != 0) {
            setAnm_NUM(7, 1);
            mAnmAtr = 7;
        }
        break;
    }
}

/* 022A8108 */
void daNpc_Ob1_c::setAnm_ATR(int withTex) {
    /* a_anm_prm_tbl (101C2744), by message attribute */
    if (withTex) {
        setAnm_tex((&a_anm_atr_prm[mAnmAtr])->mTexNo);
    }
    setAnm_anm((&a_anm_atr_prm[mAnmAtr]));
}

/* 022A82E0 */
void daNpc_Ob1_c::anmAtr(u16 msgStatus) {
    u32 play = dComIfGp_ea();
    if (mTalkState == 2 && gabi::ea(this) != dComIfGp_talkActor(play)) {
        control_anmAtr();
        return;
    }
    if (msgStatus == Ob1MessageTyping) {
        if (mAtrCnt == 0) {
            mAnmAtr = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo()); /* dComIfGp_getMesgAnimeAttrInfo */
            mAtrCnt = (s8)(mAtrCnt + 1);
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo */
        dComIfGp_clearMesgAnimeTagInfo();        /* HD: cleared unconditionally */
        if (tag != 0xFF && tag != mAnmTag) {
            mAnmTag = tag;
            /* chg_anmTag(): empty */
        }
    } else if (msgStatus == Ob1MessageDisplayed) {
        mAtrCnt = 0;
    }
    control_anmAtr();
    /* control_anmTag(): empty */
}

/* 022A7B44 */
void daNpc_Ob1_c::eventOrder() {
    s8 type = mOrderType;
    if (type == 1 || type == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
        if (mOrderType == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (type >= 3) {
        s16 evtNo = (s16)(type - 3);
        mEvtNo = evtNo;
        fopAcM_orderOtherEventId(this, eventIdx(evtNo), 0xFF, 0xFFFF, 0, 1);
    }
}

/* 022A71A4 */
void daNpc_Ob1_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        if (dComIfGp_evmng_startCheck(eventIdx(mEvtNo))) {
            mOrderType = 0;
        }
    } else if (cmd == dEvtCmd_INTALK_e) {
        if (mOrderType == 1 || mOrderType == 2) {
            mOrderType = 0;
            mbTalk = 1;
        }
    }
}

/* 022A83D8 */
BOOL daNpc_Ob1_c::chk_talk() {
    BOOL ret = FALSE;
    u8 talkXY = dComIfGp_event_getTalkXYBtn();
    if ((u32)(talkXY - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mPreItemNo = dComIfGp_event_getPreItemNo(); /* dComIfGp_event_getPreItemNo() */
            ret = TRUE;
        }
    } else {
        mPreItemNo = 0xFF;
        ret = TRUE;
    }
    return ret;
}

/* 022A8458 */
u8 daNpc_Ob1_c::chk_partsNotMove() {
    return mSaveHeadY == m_jnt.mAngles[0][1] && mSaveBboneY == m_jnt.mAngles[1][1] &&
           mSaveAngleY == current.angle.y;
}

/* 022A75CC */
void daNpc_Ob1_c::lookBack() {
    cXyz dst;
    mSaveBboneY = m_jnt.mAngles[1][1];
    s16 targetY = current.angle.y;
    mSaveHeadY = m_jnt.mAngles[0][1];
    dst.set(0.0f, 0.0f, 0.0f);
    f32 eyeX = current.pos.x;
    mSaveAngleY = targetY;
    s8 mode = mLookMode;
    f32 eyeY = eyePos.y;
    u8 headOnly = mbHeadOnly;
    cXyz* dstPos = nullptr;
    f32 eyeZ = current.pos.z;
    switch (mode) {
    case 0:
        break;
    case 1: {
        cXyz tmp;
        dNpc_playerEyePos(&tmp, -20.0f);
        dst.copy(tmp);
        eyeZ = current.pos.z;
        eyeX = current.pos.x;
        eyeY = eyePos.y;
        dstPos = &dst;
        break;
    }
    case 2:
        dst.copy(mLookPos);
        eyeZ = current.pos.z;
        eyeX = current.pos.x;
        dstPos = &dst;
        break;
    case 3:
        targetY = mTargetAngY;
        break;
    }
    cLib_addCalcAngleS2(&mHeadTurnSpd, l_HIO.mHeadTurnSpd, 4, 0x800);
    s16 maxVel;
    if (!m_jnt.mbTrn) {
        maxVel = 0;
        mHeadTurnSpd = 0;
    } else {
        maxVel = mHeadTurnSpd;
    }
    cXyz eye;
    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;
    lookAtTarget(&m_jnt, &current.angle.y, dstPos, &eye, targetY, maxVel, headOnly);
}

/* 022A8498 */
u16 daNpc_Ob1_c::next_msgStatus(u32* pMsgNo) {
    be<u32>* msgNo = (be<u32>*)pMsgNo;
    u16 ret = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    switch ((u32)*msgNo) {
    case 0xA8D:
        *msgNo = 0xA8E;
        break;
    case 0xA90:
        *msgNo = 0xA91;
        break;
    case 0xA93:
        *msgNo = 0xA94;
        break;
    case 0xA97:
        *msgNo = 0xA98;
        break;
    case 0xA92:
    case 0xA96:
    case 0xA9B:
    case 0xA9D:
    case 0xAA0:
    case 0xAA2:
    case 0xAA3:
    case 0xAA4:
    case 0xAA5:
    case 0xAA6:
        if (dComIfGs_isEventBit(0xE20) && !dComIfGs_isEventBit(0x308)) {
            *msgNo = 0xAA7;
        } else {
            ret = fopMsgStts_MSG_ENDS_e;
        }
        break;
    case 0xAA8:
        *msgNo = dKy_daynight_check() ? 0xAAA : 0xAA9;
        break;
    case 0xAAD:
    case 0xAAE:
        *msgNo = dComIfGs_isEventBit(0x304) ? 0xA8F : 0xA8D;
        break;
    default:
        ret = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return ret;
}

/* 022A8650 */
u32 daNpc_Ob1_c::getMsg_OB1_0() {
    u32 msgNo = 0;
    if (mPigCntHere == 0 && mPigCntGot == 0) {
        if (dComIfGs_isEventBit(0x2A80)) {
            if (!dComIfGs_isEventBit(0x2C40)) {
                dComIfGs_onEventBit(0x2C40);
                return 0xAAD;
            }
        } else {
            if (!dComIfGs_isEventBit(0x2C80)) {
                dComIfGs_onEventBit(0x2C80);
                return 0xAAE;
            }
        }
        msgNo = 0xA8D;
        if (dComIfGs_isEventBit(0x304)) {
            msgNo = 0xA8F;
        }
        return msgNo;
    }
    if (dComIfGs_isEventBit(0x302)) {
        s8 newCnt = mPigCntNew;
        if (newCnt != 0) {
            if (mPigCntGot == 2) {
                if (mTalkStep != 0) {
                    msgNo = 0xAA2;
                    mTalkStep = 0;
                } else {
                    mTalkStep = 1;
                    msgNo = 0xAA1;
                }
            } else {
                switch (newCnt) {
                case 1:
                    if (mTalkStep == 1) {
                        msgNo = 0xA9D;
                        mTalkStep = 0;
                    } else {
                        mTalkStep = 1;
                        msgNo = 0xA9C;
                    }
                    break;
                case 2: {
                    s8 step = mTalkStep;
                    if (step == 2) {
                        mTalkStep = 1;
                        msgNo = 0xA9F;
                    } else if (step == 1) {
                        msgNo = 0xAA0;
                        mTalkStep = 0;
                    } else {
                        mTalkStep = 2;
                        msgNo = 0xA9E;
                    }
                    break;
                }
                }
            }
        } else {
            s8 got = mPigCntGot;
            s8 here = mPigCntHere;
            if (got > here) {
                msgNo = 0xAA3;
            } else {
                switch (here) {
                case 1:
                    msgNo = 0xAA4;
                    break;
                case 2:
                    msgNo = 0xAA5;
                    break;
                default:
                    msgNo = 0xAA6;
                    break;
                }
            }
        }
    } else {
        switch (mPigCntNew) {
        case 0:
            break;
        case 1:
            if (mTalkStep == 1) {
                mTalkStep = 0;
                msgNo = 0xA92;
            } else {
                mTalkStep = 1;
                msgNo = 0xA90;
            }
            break;
        case 2: {
            s8 step = mTalkStep;
            if (step == 2) {
                mTalkStep = 1;
                msgNo = 0xA95;
            } else if (step == 1) {
                mTalkStep = 0;
                msgNo = 0xA96;
            } else {
                mTalkStep = 2;
                msgNo = 0xA93;
            }
            break;
        }
        case 3: {
            s8 step = mTalkStep;
            if (step == 3) {
                mTalkStep = 2;
                msgNo = 0xA99;
            } else if (step == 2) {
                mTalkStep = 1;
                msgNo = 0xA9A;
            } else if (step == 1) {
                mTalkStep = 0;
                msgNo = 0xA9B;
            } else {
                mTalkStep = 3;
                msgNo = 0xA97;
            }
            break;
        }
        }
        mbFirstPigTalk = 1;
    }
    return msgNo;
}

/* 022A89B4 */
u32 daNpc_Ob1_c::getMsg_OB1_1() {
    if (!dComIfGs_isEventBit(0x2C20)) {
        return 0xAA8;
    }
    return dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS) == 0 ? 0xAAC : 0xAAB;
}

/* 022A8A30 */
u32 daNpc_Ob1_c::getMsg_OB1_2() { return getMsg_OB1_1(); }

/* 022A8A34 */
u32 daNpc_Ob1_c::getMsg() {
    switch (mType) {
    case 0:
        return getMsg_OB1_0();
    case 1:
        return getMsg_OB1_1();
    case 2:
        return getMsg_OB1_2();
    default:
        return 0;
    }
}

/* 022A8A8C */
BOOL daNpc_Ob1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    } else {
        return this == dAttention_ActionTarget(attention, 0);
    }
}

/* 022A6BB8 */
void daNpc_Ob1_c::setAttention(s32 force) {
    cXyz* attnPos = &attention_info.position; /* attention_info.position */
    attnPos->set(current.pos.x, current.pos.y + l_HIO.mAttnYOffset, current.pos.z);
    if (mActRet == 0 && !force) {
        return;
    }
    eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z);
}

/* 022A8B14 */
fopAc_ac_c* daNpc_Ob1_c::searchByID(fpc_ProcID id) {
    gabi::Local<be<u32>> actor;
    *actor = 0;
    fopAcM_SearchByID2(id, actor);
    return Ob1_view<fopAc_ac_c>(*actor);
}

/* 022A71A0 */
void daNpc_Ob1_c::partner_srch() {}

/* 022A8B48 */
s8 daNpc_Ob1_c::bitCount(u8 bits) {
    s8 n = 0;
    for (int i = 0; i < 8; i++) {
        if (bits & 1) {
            n++;
        }
        bits >>= 1;
    }
    return n;
}

/* 022A8B70 */
void daNpc_Ob1_c::set_pigCnt() {
    mPigCntGot = bitCount(dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS));
    l_pig_cnt = 0;
    l_pig_bits = 0;
    for (int i = 0; i < 20; i++) {
        l_pig_tbl[i] = nullptr;
    }
    fpcM_Search(gabi::guest_fn((const void*)searchActor_Kb), this);
    mPigCntHere = bitCount(l_pig_bits);
    u8 reg = dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS);
    mPigCntNew = bitCount((u8)(l_pig_bits & ~reg));
    mTalkStep = 0;
}

/* 022A8C40 */
void daNpc_Ob1_c::ob_setPthPos() {
    if (mPathRun.mPath.get() != nullptr) {
        cXyz pnt;
        mPathRun.getPoint(&pnt, mPathRun.mIdx);
        current.pos.copy(pnt);
        mPathRun.nextIdxAuto();
        mPathRun.getPoint(&pnt, mPathRun.mIdx);
        cXyz next;
        next.copy(pnt);
        current.angle.y = cLib_targetAngleY(&current.pos, &next);
    }
}

/* 022A8CE8 */
void daNpc_Ob1_c::get_attPos(cXyz* out) {
    cXyz pos;
    dPath* path = mPathRun.nextPath(current.roomNo);
    if (path != nullptr) {
        u8 idx = mPathRun.mIdx;
        if (idx == 0) {
            idx = mPathRun.maxPoint();
        }
        Ob1PathPoint* pnt = &Ob1_view<Ob1Path>(gabi::ea(path))->mPoints.get()[(u8)(idx - 1)];
        pos.x = (f32)pnt->mPosition.x;
        pos.y = (f32)pnt->mPosition.y;
        pos.z = (f32)pnt->mPosition.z;
    }
    if (out == nullptr) { /* GHS: the result's constructor allocates when NULL */
        out = (cXyz*)operator_new(0xC);
        if (out == nullptr)
            return;
    }
    out->x = (f32)pos.x;
    out->y = (f32)pos.y;
    out->z = (f32)pos.z;
}

/* 022A794C */
s32 daNpc_Ob1_c::ob_movPass() {
    s32 ret = 0;
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (Ob1_view<Ob1Path>(gabi::ea(path))->mClosed & 1) /* closed loop */) {
        cXyz pos;
        pos.x = (f32)current.pos.x;
        pos.y = (f32)current.pos.y;
        pos.z = (f32)current.pos.z;
        if (mPathRun.chkPointPass(&pos, mPathRun.mbDir != 0)) {
            mPathRun.nextIdxAuto();
            return 1;
        }
    } else {
        cXyz d;
        cXyz_mi(&mTargetPos, &d, &current.pos);
        cXyz xz;
        xz.x = (f32)d.x;
        xz.y = 0.0f;
        xz.z = (f32)d.z;
        f32 dist = std_sqrtf(PSVECSquareMag(&xz));
        if (!(dist > mPassDist)) {
            ret = 1;
            if (mPathRun.mPath.get() != nullptr) {
                if (!mPathRun.nextIdxAuto()) {
                    ret = 2;
                }
            }
        }
    }
    return ret;
}

/* 022A78C0 */
void daNpc_Ob1_c::ob_clcMovSpd() {
    cXyz d;
    cXyz_mi(&mTargetPos, &d, &current.pos);
    cXyz xz;
    xz.x = (f32)d.x;
    xz.y = 0.0f;
    xz.z = (f32)d.z;
    PSVECSquareMag(&xz); /* the distance is not used */
    s16 ang = cLib_targetAngleY(&current.pos, &mTargetPos);
    cLib_chaseAngleS(&current.angle.y, ang, l_HIO.mWalkTurnStep);
    cLib_chaseF(&speedF, mSpdTarget, mSpdStep);
}

/* 022A7A4C */
void daNpc_Ob1_c::ob_nMove() {
    f32 rate = 1.0f;
    if (mMoveMode == 1) {
        ob_clcMovSpd();
        if (mMoveMode == 1) {
            rate = speedF * l_HIO.mWalkAnmRate;
            if (rate < 0.5f) {
                rate = 0.5f;
            }
        }
        mpMorf->setPlaySpeed(rate);
        s32 pass = ob_movPass();
        if ((u32)pass == 1) {
            mbSkipMove = 1;
            mbPointPassed = 1;
            return;
        }
        if ((u32)pass == 2) {
            mbPointPassed = 1;
            mMoveMode = 0;
        }
    }
    if (mbPointPassed != 0) {
        mbSkipMove = 1;
    }
}

/* 022A67D8 */
BOOL daNpc_Ob1_c::charDecide(int type) {
    mCharacterFlags = 0;
    if ((u32)type >= 1 && (u32)type <= 2) {
        mType = type;
    } else {
        mType = 0;
    }
    return TRUE;
}

/* 022A7490 */
void daNpc_Ob1_c::event_actionInit(int staffIdx) {
    void* p = dComIfGp_evmng_getMySubstanceP(staffIdx, "ActNo", 3 /* integer */);
    if (p != nullptr) {
        mEvtActNo = (s8)*Ob1_view<be<s32>>(gabi::ea(p));
    }
}

/* 022A74F0 */
BOOL daNpc_Ob1_c::event_action() { return TRUE; }

/* 022A74F8 */
void daNpc_Ob1_c::privateCut(int staffIdx) {
    if (staffIdx == -1) {
        return;
    }
    /* static char* cut_name_tbl[] = {"ACTION"} (101C2614) */
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, &cut_name_tbl, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch (mActIdx) {
        case 0:
            event_actionInit(staffIdx);
            break;
        }
    }
    BOOL done;
    switch (mActIdx) {
    case 0:
        done = event_action();
        break;
    default:
        done = TRUE;
        break;
    }
    if (done) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}

/* 022A7450 */
void daNpc_Ob1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}

/* 022A7410 */
s32 daNpc_Ob1_c::isEventEntry() {
    const char* name = Ob1_view<const char>(mEventCut2.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022A780C */
void daNpc_Ob1_c::event_proc(int staffIdx) {
    if (dComIfGp_evmng_endCheck(eventIdx(mEvtNo))) {
        mOrderType = 1;
        endEvent();
        return;
    }
    if (!mEventCut2.cutProc()) {
        privateCut(staffIdx);
    }
    lookBack();
}

/* 022A680C */
BOOL daNpc_Ob1_c::set_action(Ob1ActionFunc* action, void* arg) {
    s16 idx = action->idx;
    s16 delta = action->delta;
    u32 fn = action->fn;
    if (mAction.idx == idx) {
        if (idx == 0 || (mAction.delta == delta && mAction.fn == fn)) {
            return TRUE;
        }
    }
    if (mAction.idx != 0) {
        mActStep = 0; // HD: resets the retiring action to zero (GameCube common NPC uses -1).
        ptmf_call1(&mAction, this, arg);
    }
    mAction.delta = delta;
    mAction.idx = idx;
    mAction.fn = fn;
    mActStep = 0;
    ptmf_call1(&mAction, this, arg);
    return TRUE;
}

/* 022A8DA8 */
void daNpc_Ob1_c::clrSpd() {
    mSpdStep = 0.0f;
    gravity = -4.5f;
    mSpdTarget = 0.0f;
    speedF = 0.0f;
}

/* 022A8DCC */
void daNpc_Ob1_c::setStt(s8 stt) {
    searchByID(mSearchProcId); /* result unused */
    s8 prev = mStt;
    mStt = stt;
    mTimer = 0;
    switch (stt) {
    case Ob1WaitPath:
        mWaitTimer = (s16)cLib_getRndValue(90, 180);
        /* fallthrough */
    case Ob1Wait:
    case Ob1WaitAnimation:
        if (prev != Ob1Talk) {
            switch (mStt) {
            case Ob1Wait:
                mLookMode = 3;
                mTargetAngY = mHomeAngle.y;
                mbHeadOnly = 0;
                m_jnt.mbTrn = 1;
                break;
            case Ob1WaitPath: {
                mLookMode = 2;
                cXyz att;
                get_attPos(&att);
                mLookPos.copy(att);
                mbHeadOnly = 0;
                m_jnt.mbTrn = 1;
                break;
            }
            case Ob1WaitAnimation:
                mbHeadOnly = 0;
                mLookMode = 1;
                m_jnt.mbTrn = 1;
                break;
            default:
                mbHeadOnly = 0;
                m_jnt.mbTrn = 1;
                break;
            }
        }
        mOrderType = 0;
        mMoveMode = 0;
        clrSpd();
        setAnm();
        break;
    case Ob1Talk:
        mOrderType = 0;
        mbHeadOnly = 0;
        mLookMode = 1;
        m_jnt.mbTrn = 1;
        mMoveMode = 0;
        clrSpd();
        mAnmAtr = 0xFF;
        if (prev != Ob1WaitAnimation) {
            mPrevStt = prev;
        }
        set_pigCnt();
        setAnm();
        break;
    case Ob1Walk: {
        cXyz pnt;
        mPathRun.getPoint(&pnt, mPathRun.mIdx);
        mbPointPassed = 0;
        mOrderType = 0;
        mTargetPos.copy(pnt);
        mbHeadOnly = 1;
        mMoveMode = 1;
        gravity = -4.5f;
        mLookMode = 0;
        mSpdTarget = l_HIO.mWalkSpd;
        mSpdStep = l_HIO.mWalkAccel;
        mPassDist = l_HIO.mPassDist;
        setAnm();
        break;
    }
    case Ob1PartnerTalk:
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        break;
    default:
        setAnm();
        break;
    }
}

/* 022A9024 */
BOOL daNpc_Ob1_c::wait_1() {
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Ob1Talk);
        }
        return TRUE;
    }
    if (mTalkState == 1) {
        mTalkState = 2;
        setStt(Ob1PartnerTalk);
        mLookMode = 3;
        mTargetAngY = mHomeAngle.y;
        return TRUE;
    }
    mOrderType = 2;
    if (mbAttention != 0) {
        mTimer = 60;
    }
    if (cLib_calcTimer(&mTimer) != 0) {
        mLookMode = 1;
    } else {
        mLookMode = 3;
        mTargetAngY = mHomeAngle.y;
        m_jnt.mbTrn = 1;
    }
    return TRUE;
}

/* 022A9108 */
BOOL daNpc_Ob1_c::wait_2() {
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Ob1Talk);
        }
        return TRUE;
    }
    mOrderType = 2;
    if (mbAttention != 0) {
        mTimer = 60;
    }
    if (cLib_calcTimer(&mTimer) != 0) {
        mLookMode = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mWaitTimer) == 0) {
        setStt(Ob1Walk);
        return TRUE;
    }
    mLookMode = 2;
    cXyz att;
    get_attPos(&att);
    mLookPos.copy(att);
    m_jnt.mbTrn = 1;
    return TRUE;
}

/* 022A91F0 */
BOOL daNpc_Ob1_c::wait_3() {
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Ob1Talk);
        }
        return TRUE;
    }
    if (mAnmEnd != 0) {
        s8 cnt = (s8)(mLoopCnt + 1);
        mLoopCnt = cnt;
        if (cnt > 3) {
            setStt(mPrevStt);
            return TRUE;
        }
    }
    mOrderType = 2;
    m_jnt.mbTrn = 1;
    mLookMode = 1;
    return TRUE;
}

/* 022A929C */
BOOL daNpc_Ob1_c::walk_1() {
    if (mbTalk != 0) {
        setAnm_NUM(0, 1);
        speedF = 0.0f;
        if (chk_talk()) {
            setStt(Ob1Talk);
        }
        return TRUE;
    }
    mOrderType = 2;
    if (mbAttention != 0) {
        mTimer = 20;
    }
    mLookMode = cLib_calcTimer(&mTimer) != 0;
    if (mbPointPassed != 0) {
        if (mPathRun.mIdx == 0) {
            mPathRun.nextIdxAuto();
        }
        setStt(Ob1WaitPath);
        return TRUE;
    }
    mbPointPassed = 0;
    cXyz pnt;
    mPathRun.getPoint(&pnt, mPathRun.mIdx);
    mTargetPos.copy(pnt);
    return TRUE;
}

/* 022A93A8 */
BOOL daNpc_Ob1_c::talk_1() {
    u32 mng = l_msgMng();
    BOOL notMove = chk_partsNotMove();
    s8 order = mOrderType;
    if (order == 1 || order >= 3) {
        return TRUE;
    }
    u16 status = 0;
    if (mbHasMsg != 0) {
        status = (u16)fopMsgM_SearchByID(mng);
    }
    mMsgStatus = status;
    if (mbPartnerMessage != 0 && mCurrMsgBsPcId == 0xFFFFFFFF) {
        mCurrMsgNo = vcall_getMsg(this);
        u32 play = dComIfGp_ea();
        cXyz* pos = &Ob1_view<Ob1TalkList>(play + 0x5C30)
                         ->mActors[0]
                         ->eyePos; /* the talk partner's eyePos */
        mCurrMsgBsPcId = fopMsgM_messageSet(mng, mCurrMsgNo, pos);
        mbHasMsg = 0;
        return notMove;
    }
    mMsgNo = 0xFFFFFFFF;
    talk(1);
    if (mbHasMsg != 0 && fopMsgM_SearchByID(mng) == Ob1MessageBoxClosed) {
        if (mTalkStep != 0) {
            mOrderType = 3;
            setAnm_NUM(0, 1);
            endEvent();
            return notMove;
        }
        u8 reg = dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS);
        u8 bits = l_pig_bits | reg;
        l_pig_bits = bits;
        dComIfGs_setEventReg(dSv_evtReg_OB1_PIGS, bits);
        u32 msgNo = mCurrMsgNo;
        if (msgNo == 0xA8E) {
            dComIfGs_onEventBit(0x304);
        } else if (msgNo == 0xAA7) {
            dComIfGs_onEventBit(0x308);
        } else if (msgNo >= 0xAA9 && msgNo <= 0xAAC) {
            dComIfGs_onEventBit(0x2C20);
        }
        if (mbFirstPigTalk != 0) {
            dComIfGs_onEventBit(0x302);
        }
        mPreItemNo = 0xFF;
        mbTalk = 0;
        if (mType == 0) {
            setStt(Ob1WaitAnimation);
            endEvent();
        } else {
            setStt(mPrevStt);
            mTimer = 60;
            endEvent();
        }
    }
    return notMove;
}

/* 022A9610 */
BOOL daNpc_Ob1_c::manzai() {
    u32 play = dComIfGp_ea();
    switch (mTalkState) {
    case 2: {
        fopNpc_npc_c* partner = (fopNpc_npc_c*)searchByID(mPartnerProcId);
        if (gabi::ea(this) != dComIfGp_talkActor(play)) {
            if (mAnmAtr == 0xFF) {
                return TRUE;
            }
            s8 prev = mPrevStt;
            mLookMode = 3;
            mTargetAngY = mHomeAngle.y;
            m_jnt.mbTrn = 1;
            mStt = prev;
            setAnm();
            mStt = Ob1PartnerTalk;
            mAnmAtr = 0xFF;
            return TRUE;
        }
        if (partner != nullptr) {
            mMsgNo = partner->mCurrMsgNo;
            vcall_anmAtr(this, partner->mMsgStatus);
        }
        return TRUE;
    }
    case 3:
        actor_status &= ~(u32)fopAcStts_UNK4000_e;
        setStt(mPrevStt);
        mTalkState = 0;
        return TRUE;
    }
    return TRUE;
}

/* 022A9730 */
BOOL daNpc_Ob1_c::wait_action1(void* arg) {
    if (mActStep == 0) {
        ob_setPthPos();
        setStt(Ob1Walk);
        mActStep = (s8)(mActStep + 1);
        return TRUE;
    }
    if ((u32)(s32)mActStep > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch (mStt) {
    case Ob1Talk:
        mActRet = talk_1();
        break;
    case Ob1Walk:
        mActRet = walk_1();
        break;
    case Ob1WaitPath:
        mActRet = wait_2();
        break;
    case Ob1WaitAnimation:
        mActRet = wait_3();
        break;
    }
    lookBack();
    return TRUE;
}

/* 022A9824 */
BOOL daNpc_Ob1_c::wait_action2(void* arg) {
    if (mActStep == 0) {
        setStt(Ob1Wait);
        mActStep = (s8)(mActStep + 1);
        return TRUE;
    }
    if ((u32)(s32)mActStep > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch (mStt) {
    case Ob1Wait:
        mActRet = wait_1();
        break;
    case Ob1Talk:
        mActRet = talk_1();
        break;
    case Ob1PartnerTalk:
        mActRet = manzai();
        break;
    }
    lookBack();
    return TRUE;
}

/* 022A7260 */
u8 daNpc_Ob1_c::demo() {
    if (demoActorID == 0) {
        if (mbDemo != 0) {
            mbDemo = 0;
        }
        return 0;
    }
    u8 id = demoActorID;
    mbDemo = 1;
    void* demoAc = nullptr;
    if (id != 0 && id <= 0x20) { /* dComIfGp_demo_getActor(id) */
        if (l_demoObject == 0)   /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
        demoAc = dDemo_object_getActor(l_demoObject, id);
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn)) {
            mBtpFrame = (u8)J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn);
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(demoAc, "Ob");
        if (btp != nullptr) {
            m_hed_tex_pttrn = btp;
            if (mDoExt_btpAnm_init(&mBtpAnm, J3DModel_modelData(mpHedModel), btp, 1, 2, 1.0f, 0, -1,
                                   1, 0)) {
                mBtpFrame = 0;
                mTexNo = 2;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, "Ob", 0, 0, 0, 0);
    return mbDemo;
}

/* 022A7DBC */
BOOL daNpc_Ob1_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModel* hedModel = mpHedModel;
    u32 hedData = J3DModel_modelData(hedModel);
    if (mbHidden != 0 || mbHideBody != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: unused */
    mpMorf->entryDL();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&mBtpAnm, Ob1_view<J3DModelData>(hedData), mBtpFrame);
    mDoExt_modelEntryDL(hedModel);
    Ob1_view<Ob1ModelEffects>(hedData)->mTexPattern = 0; /* mBtpAnm.remove(hedData) */
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    dSnap_RegistFig(0x4F /* DSNAP_TYPE_NPC_OB1 */, this, 1.0f, 1.0f, 1.0f);
    if (l_HIO.mDebugDraw != 0) {
        /* debug colours (GXColor function-local statics, unused) */
        if (l_debugGuard0 == 0) {
            l_debugGuard0 = 1;
            Ob1_copyBytes(gabi::ea(&l_debugColour0), gabi::ea(&l_debugSource0), 4);
        }
        if (l_debugGuard1 == 0) {
            l_debugGuard1 = 1;
            Ob1_copyBytes(gabi::ea(&l_debugColour1), gabi::ea(&l_debugSource1), 4);
        }
        if (l_debugGuard2 == 0) {
            l_debugGuard2 = 1;
            Ob1_copyBytes(gabi::ea(&l_debugColour2), gabi::ea(&l_debugSource2), 4);
        }
    }
    return TRUE;
}

/* 022A7BB4 */
BOOL daNpc_Ob1_c::_execute() {
    if (mbInit == 0) {
        // HD: preserve the GHS word-copy order z, x, y without quieting NaNs.
        mHomeAngle.y = current.angle.y;
        *Ob1_view<be<u32>>(gabi::ea(&mHomePos.z)) = *Ob1_view<be<u32>>(gabi::ea(&current.pos.z));
        *Ob1_view<be<u32>>(gabi::ea(&mHomePos.x)) = *Ob1_view<be<u32>>(gabi::ea(&current.pos.x));
        mHomeAngle.x = current.angle.x;
        mHomeAngle.z = current.angle.z;
        *Ob1_view<be<u32>>(gabi::ea(&mHomePos.y)) = *Ob1_view<be<u32>>(gabi::ea(&current.pos.y));
        mbInit = 1;
    }
    daNpc_Ob1_HIO_c& hio = l_HIO;
    m_jnt.setParam(hio.mPrm[4], hio.mPrm[5], hio.mPrm[6], hio.mPrm[7], hio.mPrm[0], hio.mPrm[1],
                   hio.mPrm[2], hio.mPrm[3], hio.mPrm[8]);
    if (mbHidden != 0 && demoActorID == 0) {
        return TRUE;
    }
    mbSkipMove = 0;
    mbHidden = 0;
    partner_srch();
    checkOrder();
    if (demo() == 0) {
        bool moved = false;
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != dEvtCmd_INTALK_e) {
            s32 staffIdx = isEventEntry();
            if (staffIdx >= 0) {
                event_proc(staffIdx);
                moved = true;
            }
        }
        if (!moved) {
            ptmf_call1(&mAction, this, nullptr);
        }
        if (mbSkipMove == 0) {
            ob_nMove();
            fopAcM_posMoveF(this, &mStts.m_cc_move);
        }
        if (mbKeepAngle == 0) {
            mAngle.x = current.angle.x;
            mAngle.y = current.angle.y;
            mAngle.z = current.angle.z;
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(0);
    if (mbDemo == 0) {
        setCollision(80.0f, 160.0f);
    }
    return TRUE;
}

/* 022A7144 */
BOOL daNpc_Ob1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Ob");
    if (heap.get() != nullptr && mpMorf.get() != nullptr) { /* HD: only when the heap was created */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022A67D4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Ob1_c*)i_this)->CreateHeap(); }

/* 022A7004 */
cPhs_State daNpc_Ob1_c::_create() {
    /* fopAcM_SetupActor(this, daNpc_Ob1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Ob1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = OB1_VTBL;
            Ob1_btpCtor(&mBtpAnm);         /* mDoExt_btpAnm::mDoExt_btpAnm */
            Ob1_eventCutCtor(&mEventCut2); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Ob");
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap),
                               (u32)a_heap_size_tbl /* a_heap_size_tbl[0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -60.0f, 60.0f, 170.0f, 60.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 022A62BC */
J3DModelData* daNpc_Ob1_c::create_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ob", Ob1BodyBdl);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x985, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ob1.cpp", 0x985, "a_mdl_dat != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes("Ob", Ob1WaitBck);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            gabi::call_ptr(Ob1_view<Ob1VirtualObject>(gabi::ea(morf))->mVtable->mWords[3], morf,
                           3); /* delete mpMorf */
        }
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(0x99B, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ob1.cpp", 0x99B, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone");
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(0x99E, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ob1.cpp", 0x99E, "m_bbone_jnt_num >= 0");
    return a_mdl_dat;
}

/* 022A64A4 */
J3DModelData* daNpc_Ob1_c::create_hed_Mdl() {
    J3DModelData* a_mdl_dat =
        (J3DModelData*)dComIfG_getObjectIDRes("Ob", (s32)a_head_resID /* head BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x9B9, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ob1.cpp", 0x9B9, "a_mdl_dat != (0)");
    mpHedModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11020022);
    return a_mdl_dat;
}

/* 022A6664 */
BOOL daNpc_Ob1_c::CreateHeap() {
    J3DModelData* a_mdl_dat = create_Anm();
    if (a_mdl_dat == nullptr) {
        return FALSE;
    }
    if (create_hed_Mdl() != nullptr) {
        mTexNo = 0;
        if (iniTexPttrnAnm(false)) {
            for (u16 i = 0; i < a_mdl_dat->getJointNum(); i++) {
                if (i == m_hed_jnt_num || i == m_bbone_jnt_num) {
                    J3DModel* model = mpMorf->getModel();
                    J3DModelData_setJointCallBack(J3DModel_modelData(model), i,
                                                  gabi::guest_fn((const void*)nodeCallBack_Ob1));
                }
            }
            J3DModel* model = mpMorf->getModel();
            model->mUserArea = gabi::ea(this); /* setUserArea */
            mAcchCir.SetWall(30.0f, 80.0f);
            mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
            return TRUE;
        }
    }
    mpMorf = nullptr;
    return FALSE;
}

/* 022A7140 */
static cPhs_State daNpc_Ob1_Create(fopAc_ac_c* i_this) { return ((daNpc_Ob1_c*)i_this)->_create(); }

/* 022A719C */
static BOOL daNpc_Ob1_Delete(daNpc_Ob1_c* i_this) { return i_this->_delete(); }

/* 022A7DB8 */
static BOOL daNpc_Ob1_Execute(daNpc_Ob1_c* i_this) { return i_this->_execute(); }

/* 022A7F44 */
static BOOL daNpc_Ob1_Draw(daNpc_Ob1_c* i_this) { return i_this->_draw(); }

/* 022A7F48 */
static BOOL daNpc_Ob1_IsDelete(daNpc_Ob1_c* i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 022A9964 */
static void __sinit_d_a_npc_ob1_cpp() {
    /* header statics (this TU: the objects at P+9 / P+10, P = 10467D34) */
    for (int i = 0; i < 4; i++)
        l_headerZero[i] = 0;
    __register_global_object(0x101C2800);
    l_headerMin = -3.1415927f;
    l_headerMax = 3.1415927f;
    Ob1_headerByteCtor0(&l_headerByte0);
    __register_global_object(0x101C280C);
    Ob1_headerByteCtor1(&l_headerByte1);
    __register_global_object(0x101C2818);
    daNpc_Ob1_HIO_ct(&l_HIO); /* static daNpc_Ob1_HIO_c l_HIO */
}

/* 022A9A04 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 022A9A18 */
static void daNpc_Ob1_dt(daNpc_Ob1_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopNpc_npc_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 022A9AB4 */
static void SafeString_assureTerminationImpl(void* p) {}

/* WWHD glue
ext 028EAB2C void Ob1_headerByteCtor1(be<u8>* p)
ext 028ED6F8 void Ob1_headerByteCtor0(be<u8>* p)
ext 0259F740 void Ob1_eventCutCtor(dNpc_EventCut_c* p)
ext 025E7820 void Ob1_btpCtor(mDoExt_btpAnm_ob1* p)
ext 025A1458 void Ob1_npcCtor(fopNpc_npc_c* p)
ext 025E789C BOOL mDoExt_btpAnm_init(void* a, u32 d, J3DAnmTexPattern* btp, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end, s32 modify, s32 entry)
ext 021E1E78 s32 cLib_getRndValue(s32 min, s32 range)
ext 024EF130 s32 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 s32 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 0259DED0 void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly)
ext 0259F814 void setActorInfo2(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a)
ext 02526E70 void* dDemo_object_getActor(u32 obj, u8 id)
ext 02527828 J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i)
ext 025D54C4 BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out)
ext 025F7DB0 u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos)
ext C000A848 void Ob1_copyBytes(u32 dst, u32 src, u32 n)
ext 02055B64 s16 cLib_calcTimer(be<s16>* t)
ext 024EDFCC BOOL dAttention_LockonTruth(dAttention_c* a)
ext 025F795C u32 fopMsgM_SearchByID(u32 mng)
str 1001F104 "Ob" in 022A62BC
str 1001F110 "d_a_npc_ob1.cpp" in 022A62BC
str 1001F120 "a_mdl_dat != (0)" in 022A62BC
str 1001F108 "head" in 022A62BC
str 1001F134 "m_hed_jnt_num >= 0" in 022A62BC
str 1001F148 "backbone" in 022A62BC
str 1001F154 "m_bbone_jnt_num >= 0" in 022A62BC
str 1001F16C "Ob" in 022A64A4
str 1001F170 "d_a_npc_ob1.cpp" in 022A64A4
str 1001F180 "a_mdl_dat != (0)" in 022A64A4
str 1001F19C "Ob" in 022A6554
str 1001F1A0 "d_a_npc_ob1.cpp" in 022A6554
str 1001F1B0 "m_hed_tex_pttrn != (0)" in 022A6554
str 1001F1D8 "Ob1" in 022A6E10
str 1001F1E4 "Ob" in 022A7004
str 1001F1E7 "Ob" in 022A7144
str 1001F0E4 "d_demo.h" in 022A7260
str 1001F0C8 "m_object != (0)" in 022A7260
str 1001F1EA "Ob" in 022A7260
str 1001F1F0 "ActNo" in 022A7490
str 1001F228 "Ob" in 022A7F84
*/
