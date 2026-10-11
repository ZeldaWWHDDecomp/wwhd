/**
 * d_a_npc_aj1.cpp (WWHD)
 * NPC - Sturgeon (Outset)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_aj1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_aj1.h"
static BOOL nodeCB_Head(J3DNode*, int);
static BOOL nodeCB_BackBone(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
static u32 daNpc_Aj1_XyCheck_CB(void*, int);

const gc_tu_data gc_tu = {.safeString = 0x10016038};
static auto& l_HIO = gabi::object_at<daNpc_Aj1_HIO_c>(0x104659F0);
static auto& a_eye_pos_off = gabi::object_at<cXyz>(0x10465A2C);
static auto& l_eyeGuard = gabi::object_at<be<u32>>(0x10465A38);
static auto& l_btpRes = gabi::object_at<be<s32>>(0x100160A8);
static auto& a_bck_res_tbl = gabi::object_at<be<s32>[11]>(0x100161D8);
static auto& l_eventName = gabi::object_at<gptr<char>>(0x101BB434);
static auto& l_heapSize = gabi::object_at<be<u32>>(0x101BB458);
static auto& a_anm_num_tbl = gabi::object_at<daNpc_Aj1_c::anm_prm_c[9]>(0x101BB45C);
static auto& a_cut_tbl = gabi::object_at<gptr<char>[8]>(0x101BB4EC);
static auto& a_anm_prm_tbl = gabi::object_at<daNpc_Aj1_c::anm_prm_c[5]>(0x101BB50C);
static auto& a_anm_atr_tbl = gabi::object_at<daNpc_Aj1_c::anm_prm_c[9]>(0x101BB55C);
static auto& a_prm_tbl = gabi::object_at<daNpc_Aj1_HIO_c::hio_prm_c>(0x101BB5EC);
static auto& l_wait_action1 = gabi::object_at<Aj1ActionFunc>(0x10016010);
static auto& l_wait_action2 = gabi::object_at<Aj1ActionFunc>(0x10016018);
static auto& l_cylSource = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_message = gabi::object_at<gptr<void>>(0x101F4B5C);
static auto& l_colourGuard0 = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_colour0 = gabi::object_at<be<u32>>(0x101FEBF4);
static auto& a_colour0 = gabi::object_at<be<u32>>(0x10016020);
static auto& l_colourGuard1 = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_colour1 = gabi::object_at<be<u32>>(0x101FEBF8);
static auto& a_colour1 = gabi::object_at<be<u32>>(0x10016024);
static auto& l_colourGuard2 = gabi::object_at<be<u32>>(0x101FDA44);
static auto& l_colour2 = gabi::object_at<be<u32>>(0x101FEBE8);
static auto& a_colour2 = gabi::object_at<be<u32>>(0x10016028);
/* inlined into set_pa_pun, set_pa_aka, set_pa_don, set_pa_smk */
static inline JPABaseEmitter* particle_set(u16 id, cXyz* pos, csXyz* angle, u8 alpha, void* cb,
                                           u8 grp, s8 roomNo) {
    return dPa_control_set(dComIfGp_getParticle(), grp, id, pos, angle, nullptr, alpha,
                           (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
// HD: constructor allocation and compiler-generated template code.
/* 021EAEA8 */
static daNpc_Aj1_HIO_c* daNpc_Aj1_HIO_c_ct(daNpc_Aj1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Aj1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10016070;
    memcpy_g(&i_this->mPrm, &a_prm_tbl, 0x30); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->mEntryID = -1;
    return i_this;
}

/* 021E70F0 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Aj1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Head(i_node, model);
    }
    return TRUE;
}

/* 021E6F70 */
void daNpc_Aj1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {

    cXyz* eyeOffset = &a_eye_pos_off;
    if ((u32)l_eyeGuard == 0) {
        l_eyeGuard = 1;
        eyeOffset->z = 0.0f;
        eyeOffset->x = 24.0f;
        eyeOffset->y = -16.0f;
    }
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXMultVec(stk, eyeOffset, &mEyePos);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */

/* 021E7254 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Aj1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_BackBone(i_node, model);
    }
    return TRUE;
}

/* 021E7138 */
void daNpc_Aj1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 021E7854 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Aj1_c*>(i_this)->CreateHeap();
}

/* 021E7898 */
static u32 daNpc_Aj1_XyCheck_CB(void* i_this, int i_btn) {
    return ((daNpc_Aj1_c*)i_this)->_XyCheckCB(i_btn);
}

/* 021E7858 */
u32 daNpc_Aj1_c::_XyCheckCB(int i_btn) { return dComIfGp_getSelectItem(i_btn) == 0x48; }

/* 021E7A2C */
bool daNpc_Aj1_c::init_AJ1_0() {
    if (!dComIfGs_isEventBit(0x0E20) && !dComIfGs_isEventBit(0x0502)) {
        attention_info.distances[fopAc_Attn_TYPE_TALK_e] =
            0x1A; /* attention_info.distances[TALK] */
        Aj1ActionFunc pmf;
        Aj1_loadAction(&pmf, &l_wait_action1);
        set_action(&pmf, nullptr);
        return true;
    }
    if (mSwitchNo != 0xFF && !dComIfGs_isSwitch(mSwitchNo, fopAcM_GetRoomNo(this))) {
        dComIfGs_onSwitch(mSwitchNo, fopAcM_GetRoomNo(this));
    }
    return false;
}

/* 021E7B1C */
bool daNpc_Aj1_c::init_AJ1_1() {
    if (dComIfGs_isEventBit(0x0520)) {
        return false;
    }
    dComIfGs_onEventBit(0x0502);
    Aj1ActionFunc pmf;
    Aj1_loadAction(&pmf, &l_wait_action2);
    set_action(&pmf, nullptr);
    actor_status = (actor_status & ~fopAcStts_NOCULLEXEC_e) |
                   fopAcStts_UNK4000_e; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
    return true;
}

/* 021E7BCC */
bool daNpc_Aj1_c::init_AJ1_2() {
    if (!dComIfGs_isEventBit(0x0520)) {
        return false;
    }
    eventInfo.mpCheckCB = gabi::guest_fn(
        (const void*)daNpc_Aj1_XyCheck_CB); /* eventInfo.setXyCheckCB(daNpc_Aj1_XyCheck_CB) */
    Aj1ActionFunc pmf;
    Aj1_loadAction(&pmf, &l_wait_action2);
    set_action(&pmf, nullptr);
    return true;
}

/* 021E7FB4 */
bool daNpc_Aj1_c::createInit() {
    mSwitchNo = (u8)(mParameters >> 8);

    const char* name = l_eventName.get();
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2("Aj1" /* "Aj1" */, (fopNpc_npc_c*)(void*)this);
    mBckNum = 9;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAD; /* attention_info.distances[SPEAK] */
    attention_info.flags = 0xA;                               /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAD;  /* attention_info.distances[TALK] */
    bool init_result;
    switch ((u32)(s32)mType) {
    case 0:
        init_result = init_AJ1_0();
        break;
    case 1:
        init_result = init_AJ1_1();
        break;
    case 2:
        init_result = init_AJ1_2();
        break;
    default:
        return false;
    }
    if (!init_result) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    gravity = -4.5f;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&l_cylSource /* dNpc_cyl_src */);
    play_animation();
    mObjAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = Aj1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Aj1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 021E7D14 */
void daNpc_Aj1_c::play_animation() {
    u32 snd_id = 0;
    play_texPttrnAnm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = Aj1_materialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 021E9AF4 */
void daNpc_Aj1_c::ctrl_WAITanm() {
    switch ((u32)(s32)mBckNum) {
    case 0:
        if (Aj1_calcTimer(&mWaitTimer) == 0 && mbMorfAnimStopped && chk_parts_notMov()) {
            setAnm_NUM(1, 1);
        }
        break;
    case 1:
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mWaitTimer = (s16)Aj1_random(90, 180);
        }
        break;
    }
}

/* 021E9BC0 */
void daNpc_Aj1_c::ctrl_TIREanm() {
    if (mBckNum == 6) {
        if (mbMorfAnimStopped && Aj1_calcTimer(&mTireTimer) == 0) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(20.0f);
        }
    } else {
        mTireTimer = 0;
    }
}

/* 021E7E0C */
void daNpc_Aj1_c::setMtx(u32 i_setEyePos) {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        J3DModel* item = mpItemModel;
        J3DModel_setBaseTRMtx(item, mpMorf->getModel()->getAnmMtx(m_hnd_L_jnt_num));
        J3DModel_calc(mpItemModel);
    }
    setAttention(i_setEyePos);
}

/* 021E86B0 */
int daNpc_Aj1_c::bckResID(int i_bckNum) { return a_bck_res_tbl[i_bckNum]; }

/* 021E72A8 */
u32 daNpc_Aj1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    J3DAnmTexPattern* a_btp =
        (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Aj" /* "Aj" */, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(534, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x216, "a_btp != (0)");
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return Aj1_btpInit(mBtpAnm, morf_model_p->getModelData(), a_btp, TRUE, 0, 1.0f, 0, -1,
                       i_bModify, 0) != 0;
}

/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */

/* 021E7C5C */
void daNpc_Aj1_c::play_texPttrnAnm() {
    if (mBtpNum != 0 || Aj1_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= Aj1_frameMax(Aj1_view<Aj1Btp>(mBtpAnm)->mpAnm.get())) {
            if (mBtpNum != 0) {
                mBtpFrame = (u8)Aj1_frameMax(Aj1_view<Aj1Btp>(mBtpAnm)->mpAnm.get());
            } else {
                mBlinkTimer = (s16)Aj1_random(60, 90);
                mBtpFrame = 0;
            }
        }
    }
}

/* 021E88B4 */
void daNpc_Aj1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    f32 morf = i_anmPrmP->morf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, "Aj" /* "Aj" */);
    s8 new_bck = i_anmPrmP->bckNum;
    mLoopCnt = 0;
    mPrevMorfFrame = 0.0f;
    mbMorfAnimStopped = false;
    mBckNum = new_bck;
    if (new_bck == 2) {
        set_pa_pun();
        set_pa_aka();
        set_pa_don();
        return;
    }
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        Aj1_view<Aj1Emitter>(e)->mStatus |= 1; /* stopCreateParticle() */
        mbAkaStop = true;
    }
    del_pa(&mpPunEmitter);
    del_pa(&mpDonEmitter);
}

/* 021E89C0 */
void daNpc_Aj1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    anm_prm_c* a_anm_prm_tbl = a_anm_num_tbl; /* [9] */
    if (i_setBtp) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}

/* 021E9C50 */
void daNpc_Aj1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = ::a_anm_prm_tbl; /* [5] */
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}

/* 021E9D28 */
void daNpc_Aj1_c::chngAnmAtr(u8 i_atr) {
    if (i_atr == mAnmAtr || i_atr >= 9) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
    if (mAnmAtr == 8) {
        mTireTimer = 3;
    }
}

/* 021E9D84 */
void daNpc_Aj1_c::ctrlAnmAtr() {
    switch (mAnmAtr) {
    case 2:
        ctrl_WAITanm();
        break;
    case 5:
        if (mbMorfAnimStopped) {
            mAnmAtr = 0;
            setAnm_NUM(0, 1);
        }
        break;
    }
    ctrl_TIREanm();
}

/* 021E9CC0 */
void daNpc_Aj1_c::setAnm_ATR() {
    anm_prm_c* a_anm_prm_tbl = a_anm_atr_tbl; /* [9] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}

/* 021E9E08 */
void daNpc_Aj1_c::anmAtr(u16 i_msgStatus) {
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
    ctrlAnmAtr();
}

/* 021E9EC8 */
u16 daNpc_Aj1_c::next_msgStatus(be<u32>* o_msgNoP) {
    u16 msg_status = fopMsgStts_MSG_CONTINUES_e;
    switch (*o_msgNoP) {
    case 0x9C6:
        *o_msgNoP = 0x9C7;
        break;
    case 0x9C7:
        *o_msgNoP = 0x9C8;
        break;
    case 0x9C8:
    case 0x9CA:
        if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x3704)) {
            *o_msgNoP = 0x9DA;
        } else {
            msg_status = fopMsgStts_MSG_ENDS_e;
        }
        break;
    case 0x9C9:
        *o_msgNoP = 0x9CA;
        break;
    case 0x9CB:
    case 0x9DC:
        *o_msgNoP = 0x9CC;
        break;
    case 0x9CD:
        *o_msgNoP = 0x9CE;
        break;
    case 0x9CF:
        if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x3704)) {
            *o_msgNoP = 0x9DD;
        } else {
            msg_status = fopMsgStts_MSG_ENDS_e;
        }
        break;
    case 0x9D0:
        if (!dComIfGs_isEventBit(0x2A20)) {
            *o_msgNoP = 0x9D1;
        } else {
            *o_msgNoP = dKy_daynight_check() ? 0x9D3 : 0x9D2;
        }
        break;
    case 0x9DB:
        if (dComIfGs_isEventBit(0x0504)) {
            *o_msgNoP = 0x9CF;
        } else {
            *o_msgNoP = dComIfGs_isEventBit(0x2A80) ? 0x9DC : 0x9CB;
        }
        break;
    default:
        msg_status = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 021EA0E0 */
u32 daNpc_Aj1_c::getMsg_AJ1_0() { return dComIfGs_isEventBit(0x0510) ? 0x9C9 : 0x9C6; }

/* 021EA12C */
u32 daNpc_Aj1_c::getMsg_AJ1_1() {
    if (dComIfGs_isEventBit(0x0E20) && !dComIfGs_isEventBit(0x3702)) {
        dComIfGs_onEventBit(0x3702);
        return 0x9DB;
    }
    if (dComIfGs_isEventBit(0x0504)) {
        return 0x9CF;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x9DC : 0x9CB;
}

/* 021EA200 */
u32 daNpc_Aj1_c::getMsg_AJ1_2() {
    if (mItemNo == 0x48) {
        if (!dComIfGs_isEventBit(0x3701)) {
            return 0x9D7;
        }
        return dComIfGs_isEventBit(0x0B20) ? 0x9D8 : 0x9D9;
    }
    if (!dComIfGs_isEventBit(0x3708)) {
        return 0x9D0;
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        return 0x9D4;
    }
    return dKy_daynight_check() ? 0x9D6 : 0x9D5;
}

/* 021EA308 */
u32 daNpc_Aj1_c::getMsg() {
    switch ((u32)(s32)mType) {
    case 0:
        return getMsg_AJ1_0();
    case 1:
        return getMsg_AJ1_1();
    case 2:
        return getMsg_AJ1_2();
    }
    return 0;
}

/* 021E9284 */
void daNpc_Aj1_c::eventOrder() {
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e); /* dEvtCnd_CANTALK_e */
        if (mType == 2) {
            eventInfo_onCondition(this, dEvtCnd_CANTALKITEM_e); /* dEvtCnd_CANTALKITEM_e */
        }
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 021E8378 */
void daNpc_Aj1_c::checkOrder() {
    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex])) {
            if (mEventIndex == 0) {
                actor_status &= ~fopAcStts_UNK4000_e;
            }
            mEvtState = 0;
        }
    } else if (command == dEvtCmd_INTALK_e /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}

/* 021EA360 */
u8 daNpc_Aj1_c::chk_talk() {
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

/* 021E9AB4 */
u8 daNpc_Aj1_c::chk_parts_notMov() {
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
           mActorAngleY == current.angle.y;
}

/* 021E8F80 */
void daNpc_Aj1_c::lookBack() {
    u8 head_only = mbHeadOnly;
    cXyz look_pos;
    look_pos.set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mJointHeadY = m_jnt.mAngles[0][1];
    mActorAngleY = target_y;
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        cXyz eye;
        Aj1_playerEyePos(&eye, -20.0f);
        look_pos.copy(eye);
        look_pos_p = &look_pos;
        break;
    }
    case 2:
        look_pos.copy(mLookPos);
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
    Aj1_lookAtTarget(&m_jnt, &current.angle.y, look_pos_p, &cur_pos, target_y, mLookVel, head_only);
}

/* 021EA3F8 */
u8 daNpc_Aj1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Aj1_lockonTruth(attention)) {
        return this == (void*)Aj1_lockonTarget(attention, 0);
    }
    return this == (void*)Aj1_actionTarget(attention, 0);
}

/* 021E7DB8 */
void daNpc_Aj1_c::setAttention(u32 i_setEyePos) {
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

/* 021E789C */
u8 daNpc_Aj1_c::decideType(int i_type) {
    mSubtype = 0;
    s8 type = (u32)i_type <= 2 ? (s8)i_type : (s8)-1;
    mType = type;
    return mSubtype != -1 && type != -1;
}

/* 021E8698 */
void daNpc_Aj1_c::cut_init_AJ1_TLK() {
    mAnmAtr = 0xFF;
    mAtrSet = 0;
    mMesgAnimeTag = 0xFF;
}

/* 021E8ABC */
u32 daNpc_Aj1_c::cut_move_AJ1_TLK() {
    if (talk(1) == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        mAnmAtr = 0xFF;
        return TRUE;
    }
    return FALSE;
}

/* 021E8A30 */
void daNpc_Aj1_c::cut_init_INI_ANGRY() {
    setAnm_NUM(0, 1);
    mpMorf->setMorf(8.0f);
}

/* 021E8B24 */
u32 daNpc_Aj1_c::cut_move_VIVRATE() {
    if (mbMorfAnimStopped) {
        mDoAud_seStart(0x58A2, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        cXyz up;
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x11, &up);
        setAnm_NUM(3, 1);
        return TRUE;
    }
    return FALSE;
}

/* 021E8BD8 */
u32 daNpc_Aj1_c::cut_move_JMP() {
    if (mbMorfAnimStopped) {
        Aj1_quake(&Aj1_view<dComIfG_inf_c>(dComIfGp_get())->play.mDetect, 0);
        mCutTimer = 0x14;
        return TRUE;
    }
    return FALSE;
}

/* 021E8C40 */
u32 daNpc_Aj1_c::cut_move_SPPRISE() {
    if (Aj1_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(4, 1);
        mCutTimer = l_HIO.mPrm.mSpprTime;
        return TRUE;
    }
    return FALSE;
}

/* 021E8CB0 */
u32 daNpc_Aj1_c::cut_move_LOK() {
    if (mbMorfAnimStopped && Aj1_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(5, 1);
        mDoAud_seStart(0x4894, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        mCutTimer = l_HIO.mPrm.mLokTime;
        return TRUE;
    }
    return FALSE;
}

/* 021E8D4C */
u32 daNpc_Aj1_c::cut_move_DAN() {
    if (mbMorfAnimStopped && Aj1_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(2, 1);
        return TRUE;
    }
    return FALSE;
}

/* 021E8A74 */
void daNpc_Aj1_c::cut_init_INVIT() {
    mDoAud_seStart(0x4893, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}

/* 021E8DBC */
void daNpc_Aj1_c::privateCut(int i_staffIdx) {

    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = (s8)dComIfGp_evmng_getMyActIdx(i_staffIdx, a_cut_tbl, 8, TRUE, 0);
    mActionIndex = idx;
    if (idx == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch ((u32)(s32)mActionIndex) {
        case 0:
            cut_init_AJ1_TLK();
            break;
        case 1:
            cut_init_INI_ANGRY();
            break;
        case 7:
            cut_init_INVIT();
            break;
        }
    }
    u32 cut_end;
    switch ((u32)(s32)mActionIndex) {
    case 0:
        cut_end = cut_move_AJ1_TLK();
        break;
    case 2:
        cut_end = cut_move_VIVRATE();
        break;
    case 3:
        cut_end = cut_move_JMP();
        break;
    case 4:
        cut_end = cut_move_SPPRISE();
        break;
    case 5:
        cut_end = cut_move_LOK();
        break;
    case 6:
        cut_end = cut_move_DAN();
        break;
    default:
        cut_end = TRUE;
        break;
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 021E8654 */
void daNpc_Aj1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}

/* 021E8614 */
s32 daNpc_Aj1_c::isEventEntry() {
    const char* name = Aj1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 021E91A8 */
void daNpc_Aj1_c::event_proc(int i_staffIdx) {
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        if (mEventIndex == 0) {
            dComIfGs_onEventBit(0x0508);
            dComIfGs_onEventBit(0x0504);
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
        lookBack();
    }
}

/* 021E86C4 */
void daNpc_Aj1_c::set_pa_pun() {
    s8 roomNo = fopAcM_GetRoomNo(this);
    JPABaseEmitter* e = particle_set(0x8113, &current.pos, nullptr, 0xFF, nullptr, 0, roomNo);
    mpPunEmitter = e;
    if (e) {
        mPunTimer = 0;
    }
}

/* 021E874C */
void daNpc_Aj1_c::set_pa_aka() {
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        Aj1_stopEmitter(e);
    }
    s8 roomNo = fopAcM_GetRoomNo(this);
    mpAkaEmitter = particle_set(0x811F, &current.pos, nullptr, 0xFF, nullptr, 0, roomNo);
}

/* 021E87E4 */
void daNpc_Aj1_c::set_pa_don() {
    cXyz offset;
    offset.x = 37.3f;
    offset.y = 0.0f;
    offset.z = 13.8f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &mDonPos);
    s8 roomNo = fopAcM_GetRoomNo(this);
    mpDonEmitter = particle_set(0x8114, &mDonPos, nullptr, 0xFF, nullptr, 0, roomNo);
}

/* 021E9448 */
void daNpc_Aj1_c::set_pa_smk() {
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_foot_L_jnt_num), stk);
    mSmokePos.x = stk->m[0][3];
    mSmokePos.y = stk->m[1][3];
    mSmokePos.z = stk->m[2][3];
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)(void*)mSmokeCB);
    s8 roomNo = fopAcM_GetRoomNo(this);
    /* dComIfGp_particle_setToon(0x2027, &mSmokePos, &current.angle, NULL, 0xC8, &mSmokeCB, roomNo) */
    JPABaseEmitter* e = particle_set(0x2027, &mSmokePos, &current.angle, 0xC8, mSmokeCB, 2, roomNo);
    mpSmokeEmitter = e;
    if (e) {
        auto* a = Aj1_view<Aj1Emitter>(e);
        a->mGlobalParticleScale.z = 1.0f; /* setGlobalParticleScale(0.3, 0.3, 1.0) */
        a->mGlobalParticleScale.x = 0.3f;
        a->mGlobalParticleScale.y = 0.3f;
        a = Aj1_view<Aj1Emitter>(mpSmokeEmitter.get());
        a->mGlobalScale.z = 0.1f;
        a->mGlobalScale.y = 0.1f;
        a->mGlobalScale.x = 0.1f;
        Aj1_view<Aj1Emitter>(mpSmokeEmitter.get())->mLifeTime = 0x28;
        Aj1_view<Aj1Emitter>(mpSmokeEmitter.get())->mInitialVelocity = 3.0f;
        Aj1_view<Aj1Emitter>(mpSmokeEmitter.get())->mMaxFrame = 1;
        Aj1_view<Aj1Emitter>(mpSmokeEmitter.get())->mRate = 120.0f;
        a = Aj1_view<Aj1Emitter>(mpSmokeEmitter.get());
        a->mLocalScale.x = 1.0f;
        a->mLocalScale.y = 0.1f;
        a->mLocalScale.z = 1.0f;
        /* mSmokeCB.setSmokeColor({0xA0, 0xA0, 0x80, 0xC8}) */
        auto* c = Aj1_view<Aj1SmokeColour>(mSmokeCB);
        c->r = 0xA0;
        c->g = 0xA0;
        c->b = 0x80;
        c->a = 0xC8;
    }
}

/* 021E930C */
void daNpc_Aj1_c::flw_pa_pun() {
    JPABaseEmitter* e = mpPunEmitter;
    if (e == nullptr) {
        return;
    }
    Aj1_emitterMatrix(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num),
                      Aj1_view<Aj1Emitter>(e)->mGlobalRotation,
                      &Aj1_view<Aj1Emitter>(e)->mGlobalTranslation);
    if (Aj1_calcTimer(&mPunTimer) == 0) {
        mDoAud_seStart(0x58A3, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        mPunTimer = 5;
    }
}

/* 021E9404 */
void daNpc_Aj1_c::flw_pa_aka() {
    JPABaseEmitter* e = mpAkaEmitter;
    if (e == nullptr) {
        return;
    }
    Aj1_emitterMatrix(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num),
                      Aj1_view<Aj1Emitter>(e)->mGlobalRotation,
                      &Aj1_view<Aj1Emitter>(e)->mGlobalTranslation);
}

/* 021E93B0 */
void daNpc_Aj1_c::del_pa_aka() {
    if (!mbAkaStop) {
        return;
    }
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        auto* emitter = Aj1_view<Aj1Emitter>(e);
        if ((u32)(emitter->mAliveCount + emitter->mFreeCount) != 0) { /* getParticleNumber() != 0 */
            return;
        }
        Aj1_stopEmitter(e);
        mbAkaStop = false;
        mpAkaEmitter = nullptr;
    } else {
        mbAkaStop = false;
    }
}

/* 021E82CC */
void daNpc_Aj1_c::del_pa(gptr<JPABaseEmitter>* i_emitterP) {
    JPABaseEmitter* e = *i_emitterP;
    if (e == nullptr) {
        return;
    }
    Aj1_stopEmitter(e); /* becomeInvalidEmitter() */
    *i_emitterP = nullptr;
}

/* 021E95D4 */
void daNpc_Aj1_c::setSmoke() {
    if (mBckNum != 2) {
        return;
    }
    if (mpMorf->checkFrame(0.0f) || mpMorf->checkFrame(9.0f)) {
        set_pa_smk();
    }
    if (mpMorf->checkFrame(0.0f) || mpMorf->checkFrame(10.0f)) {
        mDoAud_seStart(0x58A4, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}

/* 021E7900 */
BOOL daNpc_Aj1_c::set_action(Aj1ActionFunc* i_newProcFunc, void* i_argsP) {
    Aj1ActionFunc* cur = &mCurrProcFunc;
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
        Aj1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Aj1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 021EA480 */
void daNpc_Aj1_c::setStt(s8 i_stt) {
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
        mEvtState = 0;
        mFarTimer = (s16)Aj1_random(90, 120);
        break;
    case 2:
        mEvtState = 0;
        break;
    case 3:
        mEvtState = 0;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        break;
    case 4:
        mEvtState = 0;
        mWaitTimer = (s16)Aj1_random(90, 180);
        if (mBckNum == 1 || mBckNum == 6) {
            return;
        }
        break;
    }
    setAnm();
}

/* 021EA6DC */
BOOL daNpc_Aj1_c::FARwai() {
    cXyz pos;
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (chk_areaIN(l_HIO.mPrm.mFarDist, l_HIO.mPrm.mFarAngle, &pos)) {
        setStt(2);
        mLookMode = 1;
        mbHeadOnly = false;
        m_jnt.mbTrn = 1; /* setTrn() */
        return TRUE;
    }
    if (mBckNum == 8) {
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(8.0f);
        }
        return TRUE;
    }
    if (Aj1_calcTimer(&mFarTimer) == 0) {
        setAnm_NUM(8, 1);
        mFarTimer = (s16)Aj1_random(90, 120);
    }
    return TRUE;
}

/* 021EA7D8 */
BOOL daNpc_Aj1_c::call_1() {
    if (mbTalk) {
        if (chk_talk()) {
            setStt(3);
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mLookMode = 1;
        }
        return TRUE;
    }
    cXyz pos;
    pos.z = current.pos.z;
    pos.y = current.pos.y;
    pos.x = current.pos.x;
    if (!chk_areaIN(l_HIO.mPrm.mFarDist, l_HIO.mPrm.mFarAngle, &pos)) {
        setStt(1);
        mpMorf->setMorf(15.0f);
        mLookMode = 3;
        m_jnt.mbTrn = 1;
        mbHeadOnly = false;
        mLookAngleY = mHomeAngle.y;
        return TRUE;
    }
    m_jnt.mbTrn = 1;
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (chk_areaIN(l_HIO.mPrm.mCallDist, l_HIO.mPrm.mCallAngle, &pos)) {
        mEvtState = 2;
        if (mbAttention) {
            if (mBckNum != 0) {
                setAnm_NUM(0, 1);
                mpMorf->setMorf(15.0f);
            }
        } else if (mBckNum != 7) {
            setAnm_NUM(7, 1);
        }
    }
    return TRUE;
}

/* 021EA97C */
BOOL daNpc_Aj1_c::wait_1() {
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x400, 0);
    ctrl_WAITanm();
    if (mEvtState == 1 || mEvtState >= 3) {
        return TRUE;
    }
    ctrl_TIREanm();
    if (mbTalk) {
        if (chk_talk()) {
            setStt(3);
            mLookMode = 1;
            mbHeadOnly = false;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mEvtState = 2;
    if (mBckNum != 1 && mbAttention) {
        mLookMode = 1;
        return TRUE;
    }
    mLookMode = 3;
    mLookAngleY = mHomeAngle.y;
    return TRUE;
}

/* 021EAA68 */
BOOL daNpc_Aj1_c::talk_1() {
    BOOL res = chk_parts_notMov();
    talk(1);
    // HD: the message status comes from the message manager.
    if (Aj1_messageStatus(l_message.get()) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0x9C8:
            dComIfGs_onEventBit(0x0510);
            break;
        case 0x9D1:
        case 0x9D2:
        case 0x9D3:
            dComIfGs_onEventBit(0x3708);
            break;
        case 0x9D7:
            dComIfGs_onEventBit(0x3701);
            break;
        case 0x9DA:
            dComIfGs_onEventBit(0x3704);
            dComIfGs_onEventBit(0x0510);
            break;
        case 0x9DD:
            dComIfGs_onEventBit(0x3704);
            break;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        mTalkEndTimer = (s16)Aj1_random(15, 30);
        endEvent();
    }
    return res;
}

/* 021EAC18 */
BOOL daNpc_Aj1_c::wait_action1(void*) {
    switch ((u32)(s32)mActState) {
    case 0:
        setStt(1);
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 1:
            mbSetEyePos = FARwai();
            break;
        case 2:
            mbSetEyePos = call_1();
            break;
        case 3:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    }
    return TRUE;
}

/* sead::SafeString operator== (HD inline: assureTermination, then strcmp bounded by 0x40001) */

/* 021EACEC */
BOOL daNpc_Aj1_c::wait_action2(void*) {
    switch ((u32)(s32)mActState) {
    case 0: {
        setStt(4);
        /* in the stage "Ojhous2" (Sturgeon's house), type 1 starts the "angry" event until event 0x508 */
        SafeString house;
        house.mStringTop = gabi::guest_addr("Ojhous2"); /* "Ojhous2" */
        house.__vtbl = gc_tu.safeString;
        SafeString stage;
        stage.__vtbl = gc_tu.safeString;
        stage.mStringTop =
            gabi::guest_addr(dComIfGp_getStartStageName()); /* dComIfGp_getStartStageName() */
        bool start = false;
        if (Aj1_equal(&house, &stage) && mType == 1) {
            start = true;
        }
        if (start && !dComIfGs_isEventBit(0x0508)) {
            mEvtState = 3;
        }
        mActState = mActState + 1;
        break;
    }
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 3:
            mbSetEyePos = talk_1();
            break;
        case 4:
            mbSetEyePos = wait_1();
            break;
        }
        lookBack();
        break;
    }
    return TRUE;
}

/* 021E844C */
u8 daNpc_Aj1_c::demo() {
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
        u32 obj = (u32)l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demoObject;
        }
        demo_actor_p = Aj1_demoGetActor(obj, id);
    }
    if (Aj1_view<Aj1Btp>(mBtpAnm)->mpAnm.get() != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= Aj1_frameMax(Aj1_view<Aj1Btp>(mBtpAnm)->mpAnm.get())) {
            mBtpFrame = (u8)Aj1_frameMax(Aj1_view<Aj1Btp>(mBtpAnm)->mpAnm.get());
        }
    }
    if (demo_actor_p) {
        J3DAnmTexPattern* demo_btp_p =
            Aj1_demoBtp(demo_actor_p, "Aj" /* "Aj" */); /* getP_BtpData */
        if (demo_btp_p) {
            Aj1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), demo_btp_p, TRUE, 0, 1.0f, 0,
                        -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 1;
        }
    }
    Aj1_demoData(this, 0x6A, mpMorf.get(), "Aj", 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}

/* 021E9920 */
BOOL daNpc_Aj1_c::_draw() {
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = morf_model_p->getModelData();
    if (mbHidden || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call.
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    Aj1_view<Aj1ModelAnimation>(morf_model_info_p)->mpBtpAnimation = 0; /* mBtpAnm.remove() */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrm.mDebugDraw) {
        Aj1_initDebugColour(&l_colourGuard0, &l_colour0, &a_colour0);
        Aj1_initDebugColour(&l_colourGuard1, &l_colour1, &a_colour1);
        Aj1_initDebugColour(&l_colourGuard2, &l_colour2, &a_colour2);
    }
    // HD: no shadowDraw().
    dSnap_RegistFig(0x4C /* DSNAP_TYPE_NPC_AJ1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 021E96BC */
BOOL daNpc_Aj1_c::_execute() {
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Aj1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mbHidden && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() &&
            eventInfo.mCommand != dEvtCmd_INTALK_e /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            Aj1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        play_animation();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!mbNoShapeAngle) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    tevStr.mRoomNo = Aj1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Aj1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(false);
    flw_pa_pun();
    del_pa_aka();
    flw_pa_aka();
    setSmoke();
    if (!mbDemo) {
        setCollision(60.0f, 140.0f);
    }
    return TRUE;
}

/* 021E82F8 */
BOOL daNpc_Aj1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Aj" /* "Aj" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    del_pa(&mpPunEmitter);
    del_pa(&mpAkaEmitter);
    del_pa(&mpDonEmitter);
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)(void*)mSmokeCB);
    return TRUE;
}

/* 021E8184 */
cPhs_State daNpc_Aj1_c::_create() {
    /* fopAcM_ct(this, daNpc_Aj1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Aj1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x10016280;
            Aj1_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            Aj1_smokeCtor(mSmokeCB, 1);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Aj" /* "Aj" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, l_heapSize)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -60.0f, 80.0f, 260.0f, 100.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 021E7398 */
BOOL daNpc_Aj1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Aj", dRes_INDEX_AJ_BDL_BODY_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2296, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x8F8, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Aj1_destroyMorf(morf); /* delete (virtual deleting destructor) */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x90F, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone");
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x911, "m_bbone_jnt_num >= 0");
    m_hnd_L_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("handL");
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x913, "m_hnd_L_jnt_num >= 0");
    m_foot_L_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("footL");
    if (m_foot_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x915, "m_fot_L_jnt_num >= 0");
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)m_hed_jnt_num)
        ->setCallBack(nodeCB_Head);
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)m_bbone_jnt_num)
        ->setCallBack(nodeCB_BackBone);
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
    return TRUE;
}

/* 021E76FC */
BOOL daNpc_Aj1_c::itemCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Aj", dRes_INDEX_AJ_BDL_ITEM_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2361, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_aj1.cpp", 0x939, "a_mdl_dat != (0)");
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}

/* 021E7798 */
BOOL daNpc_Aj1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 60.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 021E82C8 */
static cPhs_State daNpc_Aj1_Create(fopAc_ac_c* i_this) { return ((daNpc_Aj1_c*)i_this)->_create(); }

/* 021E8374 */
static BOOL daNpc_Aj1_Delete(daNpc_Aj1_c* i_this) { return i_this->_delete(); }

/* 021E991C */
static BOOL daNpc_Aj1_Execute(daNpc_Aj1_c* i_this) { return i_this->_execute(); }

/* static initialisation of a 4-byte function-local constant on first use */

/* 021E9AA8 */
static BOOL daNpc_Aj1_Draw(daNpc_Aj1_c* i_this) { return i_this->_draw(); }

/* 021E9AAC */
static BOOL daNpc_Aj1_IsDelete(daNpc_Aj1_c*) { return TRUE; }

/* 021E729C */
int daNpc_Aj1_c::btpResID(int) { return l_btpRes; }

/* 021EA570 */
u8 daNpc_Aj1_c::chk_areaIN(f32 i_dist, s16 i_angle, cXyz* i_pos) {
    cXyz diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, &diff, i_pos);
    cXyz xz;
    f32 dx = diff.x, dz = diff.z;
    xz.x = dx;
    xz.y = 0.0f;
    xz.z = dz;
    f32 abs_xz = std_sqrtf(PSVECSquareMag(&xz));
    f32 dy = dComIfGp_getPlayer(0)->current.pos.y - i_pos->y;
    s16 angle =
        (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - mHomeAngle.y);
    if (abs_xz < i_dist && std::fabs(dy) < 500.0f && abs((int)angle) < (int)i_angle) {
        return true;
    }
    return false;
}

// HD: compiler-generated and template code of this translation unit.
/* 021EAF14 */
static void __sinit_d_a_npc_aj1_cpp() {
    sinit_header_statics(0x104659D4, 0x101BB61C);
    daNpc_Aj1_HIO_c_ct(&l_HIO); /* static daNpc_Aj1_HIO_c l_HIO */
}

/* 021EAFB4 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 021EAFC8 */

static void daNpc_Aj1_c_dt(daNpc_Aj1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Aj1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Aj1_view<Aj1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x10016050;
        Aj1_view<Aj1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x10016060;
        Aj1_acchDestroy(&i_this->mObjAcch, 0);
        Aj1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021EB064 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 100160B0 "Aj" in 021E72A8
str 100160B4 "d_a_npc_aj1.cpp" in 021E72A8
str 100160C4 "a_btp != (0)" in 021E72A8
str 100160E4 "Aj" in 021E7398
str 100160F0 "d_a_npc_aj1.cpp" in 021E7398
str 10016100 "a_mdl_dat != (0)" in 021E7398
str 100160E8 "head" in 021E7398
str 10016114 "m_hed_jnt_num >= 0" in 021E7398
str 10016128 "backbone" in 021E7398
str 10016134 "m_bbone_jnt_num >= 0" in 021E7398
str 100160D4 "handL" in 021E7398
str 1001614C "m_hnd_L_jnt_num >= 0" in 021E7398
str 100160DC "footL" in 021E7398
str 10016164 "m_fot_L_jnt_num >= 0" in 021E7398
str 1001617C "Aj" in 021E76FC
str 10016180 "d_a_npc_aj1.cpp" in 021E76FC
str 10016190 "a_mdl_dat != (0)" in 021E76FC
str 100161B4 "Aj1" in 021E7FB4
str 100161CC "Aj" in 021E8184
str 100161CF "Aj" in 021E82F8
str 10016090 "d_demo.h" in 021E844C
str 10016080 "m_object != (0)" in 021E844C
str 100161D2 "Aj" in 021E844C
str 10016204 "Aj" in 021E88B4
str 1001626C "Ojhous2" in 021EACEC
ext 024EF130 s8 Aj1_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 Aj1_polyColour(dBgS*,cBgS_PolyInfo*)
ext 024EECAC u32 Aj1_materialSound(dBgS*,cBgS_PolyInfo*)
ext 02055B64 s16 Aj1_calcTimer(be<s16>*)
ext 0207A9A0 u8 Aj1_calcTimer(be<u8>*)
ext 021E1E78 s32 Aj1_random(s32,s32)
ext 025E789C s32 Aj1_btpInit(void*,J3DModelData*,J3DAnmTexPattern*,s32,s32,f32,s32,s32,u32,s32)
ext 024EDFCC bool Aj1_lockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* Aj1_lockonTarget(dAttention_c*,s32)
ext 024EE464 fopAc_ac_c* Aj1_actionTarget(dAttention_c*,s32)
ext 0259D54C void Aj1_playerEyePos(cXyz*,f32)
ext 0259DED0 void Aj1_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 028249B0 void Aj1_emitterMatrix(Mtx34*,void*,cXyz*)
ext 02529D7C void Aj1_quake(void*,u32)
ext 025A5B18 void Aj1_smokeCtor(void*,u8)
ext 025A1458 void Aj1_npcCtor(daNpc_Aj1_c*)
ext 025E7820 void Aj1_btpCtor(void*)
ext 02526E70 void* Aj1_demoGetActor(u32,u8)
ext 02527828 J3DAnmTexPattern* Aj1_demoBtp(void*,const char*)
ext 02527028 void Aj1_demoData(daNpc_Aj1_c*,u32,mDoExt_McaMorf*,const char*,s32,u32,u32,s32)
ext 02018034 void Aj1_circleDestroy(void*,s32)
ext 024EFD9C void Aj1_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Aj1_actorDestroy(daNpc_Aj1_c*,s32)
ext 025F795C u32 Aj1_messageStatus(void*)
*/
