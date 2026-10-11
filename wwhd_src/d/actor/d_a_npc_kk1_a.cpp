/**
 * d_a_npc_kk1_a.cpp (WWHD)
 * NPC - Mila (poor, Windfall): part A (joint callbacks, heaps, creation, animation, matrices,
 * orders, demo, the TU's static initialisation and compiler-generated functions)
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_kk1_a.h"

#define KK1_VTBL 0x1001C5C8 /* daNpc_Kk1_c vtable (HD: merged with fopNpc_npc_c's) */
enum { fpcNm_SWC00_e = 0x12A };
enum Kk1CreationResource {
    KK1_BCK_EFFECT = 0, KK1_BDL_BODY = 13, KK1_BDL_EFFECT = 14,
    KK1_BRK_EFFECT = 15, KK1_BTK_EFFECT = 16
};
static const dBgS_GndChk_vt l_ground_vtables = {0x1001C2A0, 0x1001C2B0, 0x1001C2D0, 0x1001C2C0};

// HD: one owning part for the translation unit's checked tables and mutable state.
static be<s32>& l_check_cnt = gabi::object_at<be<s32>>(0x1046766C);
static be<u32> (&l_check_wrk)[20] = gabi::object_at<be<u32>[20]>(0x104676F8);
static be<u32>& l_eye_offset_initialized = gabi::object_at<be<u32>>(0x10467748);
static cXyz& a_eye_pos_offst = gabi::object_at<cXyz>(0x1046768C);
static be<s16>& l_path_debug_x = gabi::object_at<be<s16>>(0x1047C76C);
static be<s16>& l_path_debug_z = gabi::object_at<be<s16>>(0x1047C76E);
static ProcFunc_l& l_wait_action = gabi::object_at<ProcFunc_l>(0x1001C210);
static dCcD_SrcCyl& l_cylinder_source = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static u8 a_arc_name[] = {75, 107, 0};
static s32 a_btp_tbl[] = {17};
static s32 a_bck_tbl[] = {9, 10, 11, 12, 4, 5, 6, 7, 1, 2, 3, 8};
static u32 a_heap_size_tbl[] = {0x00000000, 0x00000000};
static u32 a_event_name_tbl[] = {0x1001C32C, 0x1001C320, 0x1001C240, 0x1001C338, 0x1001C23C, 0x1001C218, 0x1001C220, 0x1001C248};
static u8 a_prm_tbl[] = {32, 0, 37, 28, 224, 2, 218, 228, 0, 0, 42, 248, 0, 0, 213, 8, 6, 64, 4, 176, 67, 22, 0, 0, 0, 0, 0, 210, 0, 40, 0, 90, 0, 30, 0, 20, 0, 4, 16, 0, 66, 32, 0, 0, 64, 64, 0, 0, 62, 76, 204, 205, 63, 0, 0, 0, 65, 80, 0, 0, 63, 128, 0, 0, 61, 204, 204, 205, 65, 32, 0, 0, 63, 128, 0, 0, 62, 76, 204, 205, 68, 122, 0, 0};
static daNpc_Kk1_c::anm_prm_c a_anm_num_tbl[] = {
    {0,0,8.0f,1.0f,2},
    {1,0,8.0f,1.0f,2},
    {2,0,8.0f,1.0f,2},
    {3,0,8.0f,1.0f,2},
    {4,0,8.0f,1.0f,2},
    {5,0,8.0f,1.0f,2},
    {6,0,8.0f,1.0f,2},
    {7,0,8.0f,1.0f,2},
    {8,0,8.0f,1.0f,0},
    {9,0,8.0f,1.0f,0},
    {10,0,8.0f,1.0f,0},
    {11,0,8.0f,1.0f,0}
};
static daNpc_Kk1_c::anm_prm_c a_anm_state_tbl[] = {
    {-1,-1,0.0f,0.0f,-1},
    {0,0,8.0f,1.0f,2},
    {-1,-1,0.0f,0.0f,-1},
    {3,0,8.0f,1.0f,2},
    {0,0,8.0f,1.0f,2},
    {-1,-1,0.0f,0.0f,-1},
    {0,0,8.0f,1.0f,2},
    {0,0,8.0f,1.0f,2}
};

static void getPathPoint(cXyz* o_pos, dNpc_PathRun_l* i_path);
/* 022711F4 */
static daNpc_Kk1_HIO_c* daNpc_Kk1_HIO_c_ct(daNpc_Kk1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Kk1_HIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C300;
    memcpy_g(i_this->mPrm, a_prm_tbl, 0x54); /* a_prm_tbl */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 0226AC70 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_mModel();
        daNpc_Kk1_c* actor = static_cast<daNpc_Kk1_c*>(model->getUserArea());
        if (actor != nullptr) {
            actor->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}

/* 0226AAF8 */
void daNpc_Kk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    /* static cXyz a_eye_pos_offst(14, 18, 0): guard 0x10467748, object 0x1046768C */
    cXyz* offst = &a_eye_pos_offst;
    if ((u32)l_eye_offset_initialized == 0) {
        l_eye_offset_initialized = 1;
        offst->z = 0.0f;
        offst->x = 14.0f;
        offst->y = 18.0f;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mEyePos.x = (f32)stk->m[0][3];
    mEyePos.y = (f32)stk->m[1][3];
    mEyePos.z = (f32)stk->m[2][3];
    PSMTXMultVec(stk, offst, &mAttPos);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}

/* 0226ADDC */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_mModel();
        daNpc_Kk1_c* actor = static_cast<daNpc_Kk1_c*>(model->getUserArea());
        if (actor != nullptr) {
            actor->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}

/* 0226ACB8 */
void daNpc_Kk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}

/* 0226B4A4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Kk1_c*)i_this)->CreateHeap();
}

/* 0226B4A8 */
static void* searchActor_SWC00(void* i_actor, void* i_this) {
    daNpc_Kk1_c* kk1 = (daNpc_Kk1_c*)i_this;
    if ((s32)l_check_cnt < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == fpcNm_SWC00_e) {
        u32 prm = fopAcM_GetParam((fopAc_ac_c*)i_actor);
        if (((prm >> 16) & 3) == 0 && (prm & 0xFF) == (u32)Kk1A_fields(kk1).mSwitchNo) {
            s32 n = (s32)l_check_cnt;
            l_check_cnt = n + 1;
            l_check_wrk[n] = gabi::ea(i_actor);
        }
    }
    return nullptr;
}

/* 0226B830 */
bool daNpc_Kk1_c::init_KK1_0() {
    if (!dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    if (dKy_daynight_check() && dComIfGs_isEventBit(0xE08)) {
        return false;
    }
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, gabi::ea(&l_wait_action));
    set_action(pmf, nullptr);
    return true;
}

/* 0226BFD0 */
bool daNpc_Kk1_c::createInit() {
    /* event names (.data 0x101BF2AC, 8 entries) */
    be<s16>* evtIdx = Kk1A_eventIds(this);
    for (int i = 0; i < 8; i++) {
        u32 name = a_event_name_tbl[i];
        evtIdx[i] = dComIfGp_evmng_getEventIdx(Kk1A_string(name), 0xFF);
    }
    mEventCut.setActorInfo2("Kk1" /* "Kk1" */, this);
    u32 prm = fopAcM_GetParam(this);
    u8 path = (prm >> 16) & 0xFF;
    Kk1A_fields(this).mSwitchNo = (prm >> 8) & 0xFF;
    if (path != 0xFF) {
        Kk1A_pathSetInf(&mPathRun, path, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* actor_status */
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    attention_info.distances[3] = 0xA9; /* attention_info.distances */
    attention_info.distances[1] = 0xA9;
    gravity = -4.5f;
    Kk1A_fields(this).mAnimationNo = 0xC;
    attention_info.flags = 0xA; /* attention_info.flags */
    if (Kk1A_fields(this).mMessageType != 0) {
        return false;
    }
    if (!init_KK1_0()) {
        return false;
    }
    s16 az = current.angle.z;
    s16 ax = current.angle.x;
    Kk1A_fields(this).mModelAngle.z = az; /* Kk1A_fields(this).mModelAngle: the model rotation */
    s16 ay = current.angle.y;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;
    Kk1A_fields(this).mModelAngle.y = ay;
    Kk1A_fields(this).mModelAngle.x = ax;
    /* dBgS_GndChk on the stack: put the actor on the ground */
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, l_ground_vtables, false);
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mPos.x = current.pos.x;
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mPos.y = current.pos.y + 50.0f;
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mPos.z = current.pos.z;
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    if (groundY != -1000000000.0f) {
        old.pos.y = groundY;
        current.pos.y = groundY;
    }
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&l_cylinder_source);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    /* ~dBgS_GndChk */
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mPolyVtable = 0x1001C2B0;
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mPassVtable = 0x1001C2D0;
    Kk1A_view<Kk1A_Ground>(gndChk.get())->mGroupVtable = 0x1001C290;
    Kk1A_groundDestroy(gndChk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
    return true;
}

/* 0226BB5C */
void daNpc_Kk1_c::play_animation() {
    u32 sndId = 0;
    play_btp_anm();
    if (mObjAcch.ChkGroundHit()) { /* mObjAcch.ChkGroundHit() */
        sndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)); /* dBgS::GetMtrlSndId(gnd poly) */
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    Kk1A_fields(this).mAnimationEnded = (u8)Kk1A_play(mpMorf.get(), &eyePos, sndId, reverb); /* mpMorf->play */
    if (mpMorf->getFrame() < Kk1A_fields(this).mPreviousFrame) {
        Kk1A_fields(this).mAnimationEnded = 1;
    }
    Kk1A_fields(this).mPreviousFrame = mpMorf->getFrame();
    switch ((s32)(s8)Kk1A_fields(this).mAnimationNo) {
    case 8:
        if (mpMorf->checkFrame(4.0f)) {
            gabi::Local<cXyz> ofs;
            ofs->x = 0.0f;
            ofs->y = -50.0f;
            ofs->z = -15.0f;
            setBikon(ofs);
        }
        break;
    case 9:
        if (mpMorf->checkFrame(4.0f)) {
            gabi::Local<cXyz> ofs;
            ofs->x = 0.0f;
            ofs->y = -50.0f;
            ofs->z = 0.0f;
            setBikon(ofs);
        }
        break;
    }
    play_eff_anm();
}

/* 0226BDCC */
void daNpc_Kk1_c::setMtx(u32 i_force) {
    J3DModel* model = mpMorf->getModel();
    f32 sy = scale.y;
    f32 sx = scale.x;
    f32 sz = scale.z;
    model->mBaseScale.x = sx; /* model->setBaseScale(scale) */
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    Kk1A_zxyRot(mDoMtx_stack_c::get(), Kk1A_fields(this).mModelAngle.x, Kk1A_fields(this).mModelAngle.y, Kk1A_fields(this).mModelAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    Kk1A_bckEntry(&mBckAnm, Kk1A_actorEffectModel(this)->getModelData(), (f32)(s16)Kk1A_fields(this).mEffectFrame.z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::transM(0.0f, 120.0f, 30.0f);
    J3DModel_setBaseTRMtx(Kk1A_actorEffectModel(this), mDoMtx_stack_c::get());
    J3DModel_calc(Kk1A_actorEffectModel(this));
    /* mBckAnm.remove(): the joint tree's animation */
    u32 md = gabi::ea(Kk1A_actorEffectModel(this)->getModelData());
    Kk1A_jointTree(Kk1A_modelData(md)->mJointNodes)->mpAnm = 0;
    flwAse();
    setAttention(i_force);
}

/* 0226C5DC */
s32 daNpc_Kk1_c::bckResID(int i_num) {
    return a_bck_tbl[i_num]; /* a_bck_tbl */
}

/* 0226AE24 */
s32 daNpc_Kk1_c::btpResID(int i_num) {
    return a_btp_tbl[0]; /* a_btp_tbl[0] */
}

/* 0226AE30 */
BOOL daNpc_Kk1_c::setBtp(s8 i_num, u32 i_modify) {
    J3DModel* model = mpMorf->getModel();
    if (i_num < 0) {
        return false;
    }
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mArcName, btpResID(i_num));
    if (btp == nullptr) /* JUT_ASSERT(618, btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0x26A, "a_btp != (0)");
    Kk1A_fields(this).mTextureAnimation = i_num;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, model->getModelData(), btp, TRUE, 0, 1.0f, 0, -1, i_modify, FALSE) != 0;
}

/* 0226AF1C */
BOOL daNpc_Kk1_c::init_texPttrnAnm(s8 i_num, u32 i_modify) {
    return setBtp(i_num, i_modify);
}

/* 0226B8E4 */
void daNpc_Kk1_c::play_btp_anm() {
    u8 frameMax = (u8)J3DAnm_getFrameMax(Kk1A_view<Kk1A_Btp>(mBtpAnm)->mpAnm.get());
    if (Kk1A_fields(this).mTextureAnimation == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frameMax) {
        return;
    }
    if (Kk1A_fields(this).mTextureAnimation == 0) {
        mBlinkTimer = (s16)cLib_getRndValue(0x3C, 0x5A);
        frameMax = 0;
    }
    mBtpFrame = frameMax;
}

/* 0226BA48 */
void daNpc_Kk1_c::play_eff_anm() {
    s16 brkFrame = (s16)(Kk1A_fields(this).mEffectFrame.x + 1);
    void* btk = Kk1A_view<Kk1A_Btk>(&mBtkAnm)->mpAnm.get();
    if (brkFrame < 30) {
        Kk1A_fields(this).mEffectFrame.x = brkFrame;
    } else {
        Kk1A_fields(this).mEffectFrame.x = 29;
    }
    s32 btkMax = J3DAnm_getFrameMax(btk);
    s16 btkFrame = (s16)(Kk1A_fields(this).mEffectFrame.y + 1);
    if (btkFrame < btkMax) {
        Kk1A_fields(this).mEffectFrame.y = btkFrame;
    } else {
        Kk1A_fields(this).mEffectFrame.y = (s16)(btkMax - 1);
    }
    s32 bckMax = J3DAnm_getFrameMax(Kk1A_bck(this)->mpAnm.get());
    s16 bckFrame = (s16)(Kk1A_fields(this).mEffectFrame.z + 1);
    if (bckFrame < bckMax) {
        Kk1A_fields(this).mEffectFrame.z = bckFrame;
    } else {
        Kk1A_fields(this).mEffectFrame.z = 0x3B;
    }
}

/* 0226C660 */
void daNpc_Kk1_c::setAnm_anm(anm_prm_c* i_prm) {
    s8 num = i_prm->mAnmNo;
    if (num < 0 || (s8)Kk1A_fields(this).mAnimationNo == num) {
        return;
    }
    s32 bck = bckResID(num);
    Kk1A_setAnmIDRes(mpMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, bck, -1, mArcName);
    Kk1A_fields(this).mAnimationNo = (u8)(s8)i_prm->mAnmNo;
    delAse();
    Kk1A_fields(this).mEffectHidden = 1;
    if ((s8)Kk1A_fields(this).mAnimationNo == 1) {
        setAse();
        Kk1A_fields(this).mEffectFrame.y = 0;
        Kk1A_fields(this).mEffectFrame.x = 0;
        Kk1A_fields(this).mEffectFrame.z = 0;
        Kk1A_fields(this).mEffectHidden = 0;
    }
    Kk1A_fields(this).mAnimationStarted = 0;
    Kk1A_fields(this).mAnimationEnded = 0;
    Kk1A_fields(this).mPreviousFrame = 0.0f;
}

/* 0226C730 */
void daNpc_Kk1_c::setAnm_NUM(int i_num, int i_tex) {
    anm_prm_c* tbl = a_anm_num_tbl; /* a_anm_prm_tbl */
    if (i_tex != 0) {
        init_texPttrnAnm(tbl[i_num].mTexNo, true);
    }
    setAnm_anm(&tbl[i_num]);
}

/* 0226CAD4 */
void daNpc_Kk1_c::setAnm() {
    anm_prm_c* tbl = a_anm_state_tbl; /* a_anm_prm_tbl by status */
    init_texPttrnAnm(tbl[(s8)Kk1A_fields(this).mActionState].mTexNo, true);
    setAnm_anm(&tbl[(s8)Kk1A_fields(this).mActionState]);
}

/* 0226C7DC */
void daNpc_Kk1_c::checkOrder() {
    if (eventInfo.mCommand == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        be<s16>* evtIdx = Kk1A_eventIds(this);
        if (dComIfGp_evmng_startCheck(evtIdx[Kk1A_fields(this).mEventAction]) && (s8)Kk1A_fields(this).mEventState >= 3) {
            if (Kk1A_fields(this).mEventAction == 4) {
                setAnm_NUM(0, 1);
            }
            Kk1A_fields(this).mEventState = 0;
            Kk1A_fields(this).mMessageTag = 0xFF;
            Kk1A_fields(this).mTalkAnimation = 0xFF;
        }
    } else if (checkCommandTalk()) {
        if ((s8)Kk1A_fields(this).mEventState == 1 || (s8)Kk1A_fields(this).mEventState == 2) {
            Kk1A_fields(this).mEventState = 0;
            Kk1A_fields(this).mTalkRequested = 1;
        }
    }
}

/* 0226CC2C */
fopAc_ac_c* daNpc_Kk1_c::searchByID(fpc_ProcID i_id, be<s32>* o_res) {
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_res = 0;
    if (!Kk1A_searchById(i_id, actor.get())) { /* fopAcM_SearchByID */
        *o_res = 1;
    }
    return Kk1A_actor((u32)*actor);
}

/* ---- the TU's static initialisation and compiler-generated functions ---- */

/* 0226C4C4 */
bool daNpc_Kk1_c::partner_search_sub(u32 i_func) {
    bool found = false;
    Kk1A_fields(this).mPartnerId = (u32)-1;
    l_check_cnt = 0;
    for (int i = 0; i < 0x14; i++)
        l_check_wrk[i] = 0;
    Kk1A_processSearch(i_func, this); /* fpcEx_Search */
    if ((s32)l_check_cnt != 0) {
        u32 a = (u32)l_check_wrk[0];
        u32 id = (u32)-1;
        if (a != 0)
            id = Kk1A_actor(a)->mBsPcId; /* fopAcM_GetID */
        found = true;
        Kk1A_fields(this).mPartnerId = id;
    }
    return found;
}

/* 0226C570 */
void daNpc_Kk1_c::partner_search() {
    if ((s8)Kk1A_fields(this).mActionStep != 1) {
        return;
    }
    if (Kk1A_fields(this).mMessageType == 0 && !partner_search_sub(0x0226B4A8 /* searchActor_SWC00 */)) {
        return;
    }
    Kk1A_fields(this).mActionStep = (s8)Kk1A_fields(this).mActionStep + 1;
}

/* 0226BD78 */
void daNpc_Kk1_c::setAttention(u32 i_force) {
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_HIO_kk1().mAttentionYOffset;
    attPos->z = current.pos.z;
    if (Kk1A_fields(this).mActionResult == 0 && !i_force) {
        return;
    }
    eyePos.z = (f32)mAttPos.z;
    eyePos.y = (f32)mAttPos.y;
    eyePos.x = (f32)mAttPos.x;
}

/* 0226B550 */
bool daNpc_Kk1_c::decideType(int i_type) {
    if (mType > 0) {
        return true;
    }
    mType = 1;
    Kk1A_fields(this).mMessageType = 0;
    for (int i = 0; i < 3; i++) /* strcpy(mArcName, "Kk") */
        Kk1A_arcBytes(this)[i] = a_arc_name[i];
    bool ok = false;
    if (mType != -1 && Kk1A_fields(this).mMessageType != -1) {
        ok = true;
    }
    return ok;
}

/* 0226CBE8 */
void daNpc_Kk1_c::endEvent() {
    dComIfGp_event_reset();
    Kk1A_fields(this).mTalkAnimation = 0xFF;
    Kk1A_fields(this).mMessageTag = 0xFF;
}

/* 0226CA94 */
s32 daNpc_Kk1_c::isEventEntry() {
    return dComIfGp_evmng_getMyStaffId(Kk1A_string(mEventCut.mpEvtStaffName), nullptr, 0);
}

/* 0226C7A0 */
bool daNpc_Kk1_c::checkCommandTalk() {
    bool ret = false;
    if (eventInfo.mCommand == 1) { /* eventInfo.checkCommandTalk() */
        ret = true;
        if ((s8)Kk1A_fields(this).mCommentAction == 5 && Kk1A_fields(this).mActionTimer != 0) {
            ret = false;
        }
    }
    return ret;
}

/* 0226B704 */
BOOL daNpc_Kk1_c::set_action(ProcFunc_l* i_action, void* i_arg) {
    ProcFunc_l* cur = &mAction;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        Kk1A_fields(this).mActionStep = 9;
        pmf_call(this, cur, i_arg);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    Kk1A_fields(this).mActionStep = 0;
    pmf_call(this, cur, i_arg);
    return TRUE;
}

/* 0226CB44 */
void daNpc_Kk1_c::setStt(s8 i_stt) {
    s8 old = (s8)Kk1A_fields(this).mActionState;
    Kk1A_fields(this).mActionState = i_stt;
    switch ((u32)(s32)i_stt) {
    case KK1A_ACTION_WAIT:
    case KK1A_ACTION_WAIT_PARTNER:
    case KK1A_ACTION_WAIT_NEAR:
    case KK1A_ACTION_WAIT_FAR:
        Kk1A_fields(this).mActionTimer = 0;
        Kk1A_fields(this).mEventState = 0;
        speedF = 0.0f;
        break;
    case KK1A_ACTION_TALK:
        Kk1A_fields(this).mPreviousState = old;
        Kk1A_fields(this).mTalkAnimation = 0xFF;
        Kk1A_fields(this).mMessageAnimationStarted = 0;
        Kk1A_fields(this).mEventState = 0;
        Kk1A_fields(this).mTalkLookTimer = 0;
        Kk1A_fields(this).mMessageTag = 0xFF;
        break;
    case KK1A_ACTION_WALK:
        Kk1A_fields(this).mEventState = 0;
        break;
    case KK1A_ACTION_COMMENT:
        Kk1A_fields(this).mEventState = 0;
        Kk1A_fields(this).mCommentAction = 0;
        Kk1A_fields(this).mOnPath = 1;
        Kk1A_fields(this).mLookState = 2;
        break;
    }
    setAnm();
}

/* 0226B690 */
void daNpc_Kk1_c::set_pthPoint(u8 i_idx) {
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    getPathPoint(&current.pos, &mPathRun);
    if (Kk1A_pathNextIdx(&mPathRun)) {
        gabi::Local<cXyz> next;
        getPathPoint(next, &mPathRun);
        current.angle.y = cLib_targetAngleY(&current.pos, next);
    }
}

/* 0226B984 */
void daNpc_Kk1_c::setBikon(cXyz* i_offset) {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), i_offset, pos);
    if (dPa_control_set(dComIfGp_getParticle(), 0, 0x8152, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr)) {
        Kk1A_soundStart(0x58BD, &current.pos);
    }
}

/* 0226C5F0 */
void daNpc_Kk1_c::setAse() {
    delAse();
    Kk1A_fields(this).mSweatEmitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x819E, &current.pos, &current.angle, nullptr, 0xFF, nullptr, -1,
                                    nullptr, nullptr, nullptr));
}

/* 0226BCD4 */
void daNpc_Kk1_c::flwAse() {
    if (Kk1A_fields(this).mSweatEmitter == 0) {
        return;
    }
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_head_jnt_num), stk);
    u32 emitter = Kk1A_fields(this).mSweatEmitter;
    f32 x = stk->m[0][3];
    f32 y = stk->m[1][3];
    f32 z = stk->m[2][3];
    if (Kk1A_emitter(emitter)->mCoordinateMode >= 7) {
        y = -y;
    }
    Kk1A_emitter(emitter)->mGlobalTranslation.y = y; /* emitter->setGlobalTranslation */
    Kk1A_emitter(emitter)->mGlobalTranslation.x = x;
    Kk1A_emitter(emitter)->mGlobalTranslation.z = z;
}

/* 0226C438 */
void daNpc_Kk1_c::delAse() {
    u32 emitter = Kk1A_fields(this).mSweatEmitter;
    if (emitter == 0) {
        return;
    }
    /* emitter->becomeInvalidEmitter() */
    Kk1A_emitter(emitter)->mMaxFrame = -1;
    Kk1A_emitter(emitter)->mFlags |= 1;
    Kk1A_fields(this).mSweatEmitter = 0;
}

/* 0226C8D8 */
u8 daNpc_Kk1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (Kk1A_fields(this).mInDemo != 0) {
            Kk1A_fields(this).mInDemo = 0;
        }
        return 0;
    }
    if (Kk1A_fields(this).mInDemo == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        Kk1A_fields(this).mInDemo = 1;
        Kk1A_fields(this).mDemoAnimation = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* demoAc = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) (HD inline with a range check) */
        u32 obj = (u32)l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demo_object;
        }
        demoAc = Kk1A_demoGetActor(obj, id);
    }
    J3DAnmTexPattern* btp = Kk1A_view<Kk1A_Btp>(mBtpAnm)->mpAnm.get();
    if (btp != nullptr) {
        u8 frameMax = (u8)J3DAnm_getFrameMax(btp);
        u8 frame = (u8)(mBtpFrame + 1);
        if (frame < frameMax) {
            mBtpFrame = frame;
        } else {
            mBtpFrame = frameMax;
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* demoBtp = Kk1A_demoBtp(demoAc, mArcName);
        if (demoBtp != nullptr) {
            mDoExt_btpAnm_init(mBtpAnm, mpMorf->getModel()->getModelData(), demoBtp, TRUE, 0, 1.0f, 0, -1, TRUE, FALSE);
            mBtpFrame = 0;
            Kk1A_fields(this).mTextureAnimation = 1;
        }
    }
    Kk1A_demoData(this, 0x6A, mpMorf, mArcName, 0, 0, 0, 0);
    return Kk1A_fields(this).mInDemo;
}

/* 0226C464 */
BOOL daNpc_Kk1_c::_delete() {
    /* HD: no HIO child deletion */
    dComIfG_resDelete(&mPhs, mArcName);
    delAse();
    if (heap != 0 && mpMorf.get() != nullptr) { /* heap */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 0226C28C */
cPhs_State daNpc_Kk1_c::_create() {
    /* fopAcM_ct(this, daNpc_Kk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Kk1A_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KK1_VTBL;
            Kk1A_btpCtor(mBtpAnm);  /* mDoExt_btpAnm::mDoExt_btpAnm */
            Kk1A_brkCtor(mBrkAnm);  /* mDoExt_brkAnm::mDoExt_brkAnm */
            Kk1A_btkCtor(&mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm (matcher: init) */
            /* inline mDoExt_bckAnm::mDoExt_bckAnm() */
            // HD: inlined bck constructor retains each subobject initializer and store order.
            Kk1A_Bck* b = Kk1A_bck(this);
            Kk1A_frameInit(b, 0); /* J3DFrameCtrl::init */
            b->mAnmVtable = 0x1016E54C;
            Kk1A_transformCtor(b->mAnmTransform);
            b->mFrameFlags = 0;
            b->mBlendVtable = 0x1016D820;
            b->mBlendFlags = 0;
            b->mAnimationFlags = 0;
            b->mFlags = 0;
            b->mpAnm = 0;
            b->mAnmVtable = 0x1001C268; /* this TU's vtable */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State result = dComIfG_resLoad(&mPhs, mArcName);
    Kk1A_fields(this).mResourceLoaded = result == cPhs_COMPLEATE_e;
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }
    /* a_heap_size_tbl (.data 0x101BF2EC) */
    if (!fopAcM_entrySolidHeap(this, 0x0226B4A4 /* CheckCreateHeap */, a_heap_size_tbl[(s32)mType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return result;
}

/* 0226AF20 */
bool daNpc_Kk1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, KK1_BDL_BODY);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(3684, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xE64, "a_mdl_dat != (0)");
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr,
                                                  0x80000, 0x11020022);
    mpMorf = morf;
    if (morf == nullptr) {
        return false;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            /* delete mpMorf (deleting destructor, vtable slot 3) */
            Kk1A_deleteMorf(morf);
        }
        mpMorf = nullptr;
        return false;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return false;
    }
    m_head_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head" /* "head" */);
    if (m_head_jnt_num < 0) /* JUT_ASSERT(3706, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xE7A, "m_hed_jnt_num >= 0");
    m_backbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "backbone" /* "backbone" */);
    if (m_backbone_jnt_num < 0) /* JUT_ASSERT(3708, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xE7C, "m_bbone_jnt_num >= 0");
    mpMorf->getModel()->getModelData()->getJointNodePointer((u16)(s32)m_head_jnt_num)->setCallBack(nodeCB_Head);
    mpMorf->getModel()->getModelData()->getJointNodePointer((u16)(s32)m_backbone_jnt_num)->setCallBack(nodeCB_BackBone);
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
    return true;
}

/* 0226B1E0 */
bool daNpc_Kk1_c::effcCreateHeap() {
    J3DModelData* md = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, KK1_BDL_EFFECT);
    Kk1A_fields(this).mEffectModel = gabi::ea(mDoExt_J3DModel__create(md, 0, 0x11020203));
    if (Kk1A_fields(this).mEffectModel == 0) {
        return true;
    }
    void* brk = dComIfG_getObjectIDRes(mArcName, KK1_BRK_EFFECT);
    if (brk == nullptr) /* JUT_ASSERT(3735) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xE97, "(0) != a_bpk");
    if (!Kk1A_brkInit(mBrkAnm, Kk1A_actorEffectModel(this)->getModelData(), brk, TRUE, 0, 0.0f, 0, -1, FALSE, FALSE)) {
        return false;
    }
    void* btk = dComIfG_getObjectIDRes(mArcName, KK1_BTK_EFFECT);
    if (btk == nullptr) /* JUT_ASSERT(3743) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xE9F, "(0) != a_btk");
    if (!mDoExt_btkAnm_init(&mBtkAnm, Kk1A_actorEffectModel(this)->getModelData(), btk, TRUE, 0, 0.0f, 0, -1, FALSE, FALSE)) {
        return false;
    }
    void* bck = dComIfG_getObjectIDRes(mArcName, KK1_BCK_EFFECT);
    if (bck == nullptr) /* JUT_ASSERT(3751) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0xEA7, "(0) != a_bck");
    if (!Kk1A_bckInit(&mBckAnm, Kk1A_actorEffectModel(this)->getModelData(), bck, TRUE, 0, 0.0f, 0, -1, FALSE)) {
        return false;
    }
    Kk1A_fields(this).mEffectFrame.y = 0;
    Kk1A_fields(this).mEffectFrame.x = 0;
    Kk1A_fields(this).mEffectFrame.z = 0;
    return true;
}

/* 0226B3E8 */
bool daNpc_Kk1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return false;
    }
    if (!effcCreateHeap()) {
        mpMorf = nullptr;
        return false;
    }
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return true;
}

/* 0226B5B4 */
static void getPathPoint(cXyz* o_pos, dNpc_PathRun_l* i_path) {
    gabi::Local<cXyz> pt;
    Kk1A_pathGetPoint(i_path, pt, i_path->mIdx);
    f32 x = pt->x;
    f32 y = pt->y;
    o_pos->x = x;
    f32 z = pt->z;
    o_pos->y = y;
    o_pos->z = z;
    if (i_path->mIdx == 0x11) {
        o_pos->x = x + ((f32)(s16)l_path_debug_x + 20.0f);
        o_pos->z = o_pos->z - ((f32)(s16)l_path_debug_z + 50.0f);
    }
}

/* 02271260 */
static void __sinit_d_a_npc_kk1_cpp() {
    sinit_header_statics_z(0x10467670, 0x101BF640, 0x1046767C);
    daNpc_Kk1_HIO_c_ct(&l_HIO_kk1()); /* static daNpc_Kk1_HIO_c l_HIO */
}

/* 02271300 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 02271314 */
static void daNpc_Kk1_c_dt(daNpc_Kk1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        Kk1A_anmDestroy(&Kk1A_bck(i_this)->mAnmVtable, 0); /* mBckAnm's J3DAnm object (+0x10) */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Kk1A_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Kk1A_view<Kk1A_AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001C2E0;
        Kk1A_view<Kk1A_AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001C2F0;
        Kk1A_acchDestroy(&i_this->mObjAcch, 0);
        Kk1A_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022713BC */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue
str 1001C368 "d_a_npc_kk1.cpp" in 0226AE30
str 1001C378 "a_btp != (0)" in 0226AE30
str 1001C388 "head" in 0226AF20
str 1001C390 "d_a_npc_kk1.cpp" in 0226AF20
str 1001C3A0 "a_mdl_dat != (0)" in 0226AF20
str 1001C3B4 "m_hed_jnt_num >= 0" in 0226AF20
str 1001C3C8 "backbone" in 0226AF20
str 1001C3D4 "m_bbone_jnt_num >= 0" in 0226AF20
str 1001C3EC "d_a_npc_kk1.cpp" in 0226B1E0
str 1001C3FC "(0) != a_bpk" in 0226B1E0
str 1001C40C "(0) != a_btk" in 0226B1E0
str 1001C41C "(0) != a_bck" in 0226B1E0
str 1001C464 "Kk1" in 0226BFD0
str 1001C310 "m_object != (0)" in 0226C8D8
str 1001C348 "d_demo.h" in 0226C8D8
ext 0259E6D0 void Kk1A_pathSetInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd)
ext 0259E778 void Kk1A_pathGetPoint(dNpc_PathRun_l* p, cXyz* out, u8 idx)
ext 0259ECFC BOOL Kk1A_pathNextIdx(dNpc_PathRun_l* p)
ext 025E19CC void Kk1A_soundStart(u32 id, cXyz* pos)
ext 025E74FC s32 Kk1A_brkInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 025E8508 s32 Kk1A_bckInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify)
ext 025E86B8 void Kk1A_bckEntry(void* anm, J3DModelData* d, f32 frame)
ext 0259D24C BOOL Kk1A_setAnmIDRes(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 bck, s32 bas, const char* arc)
ext 02526E70 void* Kk1A_demoGetActor(u32 obj, u8 id)
ext 02527828 J3DAnmTexPattern* Kk1A_demoBtp(void* ac, const char* arc)
ext 02527028 BOOL Kk1A_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r)
ext 025F1B48 void Kk1A_zxyRot(Mtx34* m, s16 x, s16 y, s16 z)
ext 025E535C BOOL Kk1A_play(mDoExt_McaMorf* m, cXyz* pos, u32 snd, s32 reverb)
ext 02008DAC void Kk1A_groundDestroy(dBgS_GndChk* p, s32 flags)
ext 025A1458 void Kk1A_npcCtor(daNpc_Kk1_c* p)
ext 025E7820 void Kk1A_btpCtor(void* p)
ext 025E7480 void Kk1A_brkCtor(void* p)
ext 025E7C6C void Kk1A_btkCtor(void* p)
ext 027F2BC0 void Kk1A_frameInit(void* p, s32 n)
ext 027DA984 void Kk1A_transformCtor(void* p)
ext 025DE508 void* Kk1A_processSearch(u32 fn, void* p)
ext 025D54C4 BOOL Kk1A_searchById(fpc_ProcID id, be<u32>* out)
ext 027F3628 void Kk1A_anmDestroy(void* p, s32 flags)
ext 02018034 void Kk1A_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Kk1A_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Kk1A_actorDestroy(daNpc_Kk1_c* p, s32 flags)
data a_btp_tbl 1001C360
data a_bck_tbl 1001C470
data a_heap_size_tbl 101BF2EC
data a_event_name_tbl 101BF2AC
data a_prm_tbl 101BF5EC
data a_anm_num_tbl 101BF2F4
data a_anm_state_tbl 101BF3D4
data a_arc_name 1001C440
*/
