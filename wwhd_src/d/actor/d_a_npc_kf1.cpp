/**
 * d_a_npc_kf1.cpp (WWHD)
 * NPC - Mila's father (rich): the Windfall pot shop owner ("angry" when pots are broken)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_kf1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_kf1.h"

enum : u32 { Kf1JudgeActorId = 0x025E1234 };
static BOOL nodeCB_Head(J3DNode*, int);
static BOOL nodeCB_Neck(J3DNode*, int);
static BOOL nodeCB_BackBone(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
const gc_tu_data gc_tu = {.safeString = 0x1001B8A0};
static auto& l_HIO = gabi::object_at<daNpc_Kf1_HIO_c>(0x104674E0);
static auto& a_eye_pos_off = gabi::object_at<cXyz>(0x1046751C);
static auto& l_eyeGuard = gabi::object_at<be<u32>>(0x10467578);
static auto& l_tsubo_num = gabi::object_at<be<s32>>(0x104674C0);
static auto& l_tsubo = gabi::object_at<gptr<fopAc_ac_c>[20]>(0x10467528);
static auto& a_btp_res_tbl = gabi::object_at<be<s32>[3]>(0x1001B918);
static auto& a_bck_res_tbl = gabi::object_at<be<s32>[10]>(0x1001BA20);
static auto& a_event_name_tbl = gabi::object_at<gptr<char>[3]>(0x101BEE14);
static auto& a_heap_size_tbl = gabi::object_at<be<u32>[2]>(0x101BEE20);
static auto& a_cut_tbl = gabi::object_at<gptr<char>[14]>(0x101BEF08);
static auto& a_anm_prm_tbl = gabi::object_at<daNpc_Kf1_c::anm_prm_c[4]>(0x101BEE28);
static auto& a_anm_num_tbl = gabi::object_at<daNpc_Kf1_c::anm_prm_c[10]>(0x101BEE68);
static auto& a_anm_atr_tbl = gabi::object_at<daNpc_Kf1_c::anm_prm_c[11]>(0x101BEF40);
static auto& a_prm_tbl = gabi::object_at<daNpc_Kf1_HIO_c::hio_prm_c>(0x101BEFF0);
static auto& l_waitAction = gabi::object_at<Kf1ActionFunc>(0x1001B878);
static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_message = gabi::object_at<gptr<void>>(0x101F4B5C);
static auto& l_frameCounter = gabi::object_at<be<s32>>(0x101FF558);
static auto& l_rupeeOffsets = gabi::object_at<be<u32>[3]>(0x1001BAEC);
static auto& l_debugGuard0 = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColour0 = gabi::object_at<be<u32>>(0x101FEBF4);
static auto& l_debugSource0 = gabi::object_at<be<u32>>(0x1001B888);
static auto& l_debugGuard1 = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_debugColour1 = gabi::object_at<be<u32>>(0x101FEBF8);
static auto& l_debugSource1 = gabi::object_at<be<u32>>(0x1001B88C);
static auto& l_debugGuard2 = gabi::object_at<be<u32>>(0x101FDA44);
static auto& l_debugColour2 = gabi::object_at<be<u32>>(0x101FEBE8);
static auto& l_debugSource2 = gabi::object_at<be<u32>>(0x1001B890);
static auto& l_arcName = gabi::object_at<be<u8>[3]>(0x1001B9F8);

/* 02260D64 */
void daNpc_Kf1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    /* static cXyz a_eye_pos_off(30.0f, 30.0f, 0.0f): guard 0x10467578, object 0x1046751C */
    cXyz* a_eye_pos_off = &::a_eye_pos_off;
    if ((u32)l_eyeGuard == 0) {
        a_eye_pos_off->x = 30.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 30.0f;
        l_eyeGuard = 1;
    }
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02260EB4 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Kf1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Head(i_node, model);
    }
    return TRUE;
}

/* 02260EFC */
void daNpc_Kf1_c::_nodeCB_Neck(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 02261020 */
static BOOL nodeCB_Neck(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Kf1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Neck(i_node, model);
    }
    return TRUE;
}

/* 02261068 */
void daNpc_Kf1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 0226118C */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Kf1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_BackBone(i_node, model);
    }
    return TRUE;
}

/* 022611D4 */
int daNpc_Kf1_c::btpResID(int i_btpNum) { return a_btp_res_tbl[i_btpNum]; }

/* 022611E8 */
u32 daNpc_Kf1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(525, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x20D, "a_btp != (0)");
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return Kf1_btpInit(mBtpAnm, morf_model_p->getModelData(), a_btp, TRUE, 0, 1.0f, 0, -1,
                       i_bModify, 0) != 0;
}

/* 022612D4 */
u32 daNpc_Kf1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    return setBtp(i_btpNum, i_bModify);
}

/* 022612D8 */
BOOL daNpc_Kf1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 1);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2448, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x990, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Kf1_deleteMorf(morf); /* delete (virtual deleting destructor) */
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
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x9A6, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone");
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x9A8, "m_bbone_jnt_num >= 0");
    m_nck_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("neck");
    if (m_nck_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x9AA, "m_nck_jnt_num >= 0");
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)m_hed_jnt_num)
        ->setCallBack(nodeCB_Head);
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)m_bbone_jnt_num)
        ->setCallBack(nodeCB_BackBone);
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)m_nck_jnt_num)
        ->setCallBack(nodeCB_Neck);
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea((uintptr_t)this) */
    return TRUE;
}

/* 02261624 */
BOOL daNpc_Kf1_c::itemCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2503, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x9C7, "a_mdl_dat != (0)");
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}

/* 022616BC */
BOOL daNpc_Kf1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 90.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 02261778 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Kf1_c*>(i_this)->CreateHeap();
}

/* 0226177C */
static void* searchActor_Tsubo(void* i_actor, void*) {
    fopAc_ac_c* actor = (fopAc_ac_c*)i_actor;
    if (l_tsubo_num < 20 && fopAc_IsActor(actor) && actor != nullptr &&
        fpcM_GetName(actor) == 0x1C5 && Kf1_prmAbstract(actor, 4, 0x18) == 0xE) {
        s32 n = l_tsubo_num;
        l_tsubo_num = n + 1;
        l_tsubo[n] = actor;
    }
    return nullptr;
}

/* 02261814 */
u8 daNpc_Kf1_c::decideType(int i_type) {
    if (mHeapType > 0) {
        return true;
    }
    mHeapType = 1;
    mType = 0;
    /* strcpy(mArcName, "Kf") (3 bytes) */
    u8 c0 = l_arcName[0], c1 = l_arcName[1], c2 = l_arcName[2];
    Kf1_view<be<u8>>(mArcName)[0] = c0;
    Kf1_view<be<u8>>(mArcName)[1] = c1;
    Kf1_view<be<u8>>(mArcName)[2] = c2;
    return mHeapType != -1 && mType != -1;
}

/* 02261878 */
void daNpc_Kf1_c::set_pthPoint(u32 i_idx) { /* u8, passed on unnormalised */
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    cXyz pnt;
    Kf1_pathPoint(&mPathRun, &pnt, i_idx); /* getPoint */
    current.pos.copy(pnt);
    if (mPathRun.nextIdx()) {
        cXyz next;
        mPathRun.getPoint(&next, mPathRun.mIdx);
        cXyz target;
        target.copy(next);
        current.angle.y = cLib_targetAngleY(&current.pos, &target);
    }
}

/* 02261920 */
BOOL daNpc_Kf1_c::set_action(Kf1ActionFunc* i_newProcFunc, void* i_argsP) {
    Kf1ActionFunc* cur = &mCurrProcFunc;
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
        Kf1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Kf1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 02261A4C */
bool daNpc_Kf1_c::init_KF1_0() {
    if (dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    Kf1ActionFunc pmf;
    Kf1_loadAction(&pmf, &l_waitAction);
    set_action(&pmf, nullptr);
    return true;
}

/* 02261AD0 */
void daNpc_Kf1_c::play_btp_anm() {
    u8 frame_max = (u8)Kf1_frameMax(Kf1_view<Kf1Btp>(mBtpAnm)->mpAnm.get());
    if (mBtpNum == 0 && Kf1_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frame_max) {
        return;
    }
    if (mBtpNum == 0) {
        mBlinkTimer = (s16)Kf1_random(60, 90);
        frame_max = 0;
    }
    mBtpFrame = frame_max;
}

/* 02261B70 */
void daNpc_Kf1_c::play_animation() {
    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = Kf1_materialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 02261C14 */
void daNpc_Kf1_c::setAttention(u32 i_setEyePos) {
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

/* 02261C68 */
void daNpc_Kf1_c::setMtx(u32 i_setEyePos) {
    mpMorf->getModel()->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        /* the item (a pot) sits on the head */
        PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_transM(33.87f, 3.26f, 0.0f);
        Kf1_XYZrotM(mDoMtx_stack_c::get(), -0x4000, -0x4000, 0);
        mpItemModel->setBaseTRMtx(mDoMtx_stack_c::get());
        mpItemModel->calc();
    }
    setAttention(i_setEyePos);
}

/* 02261E4C */
bool daNpc_Kf1_c::createInit() {
    /* l_evn_tbl (.data 0x101BEE14): "angry", "rupee_age", "bensyou" */
    for (int i = 0; i < 3; i++) {
        const char* name = a_event_name_tbl[i].get();
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2("Kf1" /* "Kf1" */, (fopNpc_npc_c*)this);
    u32 prm = mParameters;
    mSwitchNo = (u8)(prm >> 8);
    u8 path_no = (u8)(prm >> 16);
    u8 weight = 0xFF;
    if (path_no != 0xFF) {
        mPathRun.setInf(path_no, fopAcM_GetRoomNo(this), 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[SPEAK] */
    mBckNum = 0xA;
    attention_info.flags =
        (1 << fopAc_Attn_TYPE_TALK_e) | (1 << fopAc_Attn_TYPE_SPEAK_e); /* attention_info.flags */
    gravity = -4.5f;
    if (mType != 0) {
        return false;
    }
    if (!init_KF1_0()) {
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
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = Kf1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Kf1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 02262030 */
cPhs_State daNpc_Kf1_c::_create() {
    /* fopAcM_ct(this, daNpc_Kf1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Kf1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001BC18;
            Kf1_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
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
    /* a_heap_size_tbl (.data 0x101BEE20) */
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap),
                               a_heap_size_tbl[mHeapType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(mpMorf->getModel()->getBaseTRMtx()); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -90.0f, -20.0f, -80.0f, 90.0f, 200.0f, 80.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 02262180 */
static cPhs_State daNpc_Kf1_Create(fopAc_ac_c* i_this) { return ((daNpc_Kf1_c*)i_this)->_create(); }

/* 02262184 */
BOOL daNpc_Kf1_c::_delete() {
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022621D8 */
static BOOL daNpc_Kf1_Delete(daNpc_Kf1_c* i_this) { return i_this->_delete(); }

/* 022621DC */
u8 daNpc_Kf1_c::srch_Tsubo() {
    if (mActState != 1) {
        return false;
    }
    mSearchActorId = 0xFFFFFFFF;
    l_tsubo_num = 0;
    for (int i = 0; i < 20; i++) {
        l_tsubo[i] = 0;
    }
    Kf1_searchActors(gabi::guest_fn((const void*)searchActor_Tsubo), this);
    if (l_tsubo_num < 8) {
        return false;
    }
    mTsuboNum = 0;
    for (int i = 0; i < 8; i++) {
        fopAc_ac_c* a = l_tsubo[i];
        mTsuboIds[i] = a != 0 ? fopAcM_GetID(a) /* fopAcM_GetID */ : 0xFFFFFFFF;
        mTsuboNum = mTsuboNum + 1;
    }
    mActState = mActState + 1;
    return true;
}

/* 022622D0 */
void daNpc_Kf1_c::checkOrder() {
    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
            if (mEventIndex == 0) {
                /* the player faces the way it faces now (player + 0x422) */
                fopAc_ac_c* player = dComIfGp_getPlayer(0);
                s16 ang = dComIfGp_getPlayer(0)->current.angle.y;
                Kf1_view<Kf1PlayerDemo>(player)->mAngle = ang;
            }
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

/* 022623CC */
u8 daNpc_Kf1_c::demo() {
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
        u32 obj = l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = l_demoObject;
        }
        demo_actor_p = Kf1_demoActor(obj, id);
    }
    J3DAnmTexPattern* btp = Kf1_view<Kf1Btp>(mBtpAnm)->mpAnm;
    if (btp != 0) {
        u8 frame_max = (u8)Kf1_frameMax(btp);
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = Kf1_demoBtp(demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            Kf1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), demo_btp_p, TRUE, 0, 1.0f, 0,
                        -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 2;
        }
    }
    Kf1_demoData(this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}

/* 02262588 */
s32 daNpc_Kf1_c::isEventEntry() {
    const char* name = Kf1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022625C8 */
fopAc_ac_c* daNpc_Kf1_c::searchByID(u32 i_id, be<s32>* o_isDeleted) {
    Kf1GuestWord actor;
    actor = 0;
    *o_isDeleted = 0;
    if (!Kf1_searchByID(i_id, &actor)) {
        *o_isDeleted = 1;
    }
    return Kf1_actorAt((u32)actor);
}

/* 0226261C */
s32 daNpc_Kf1_c::chk_tsubo() {
    s16 num = 0;
    Kf1GuestInt is_deleted;
    for (int i = 0; i < 8; i++) {
        searchByID(mTsuboIds[i], &is_deleted);
        if (is_deleted == 0) {
            num++;
        }
    }
    return num;
}

/* 02262698 */
int daNpc_Kf1_c::bckResID(int i_bckNum) { return a_bck_res_tbl[i_bckNum]; }

/* 022626AC */
void daNpc_Kf1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    dNpc_setAnmIDRes(mpMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, resID, -1,
                     mArcName);
    s8 new_bck = i_anmPrmP->bckNum;
    mbMorfAnimStopped = false;
    mBckNum = new_bck;
    mPrevMorfFrame = 0.0f;
    mAtrLoopCnt = 0;
}

/* 02262744 */
void daNpc_Kf1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = ::a_anm_prm_tbl;
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}

/* 022627B4 */
void daNpc_Kf1_c::setStt(s8 i_stt) {
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
        mEvtState = 0;
        mWalkTimer = (s16)Kf1_random(60, 90);
        speedF = 0.0f;
        break;
    case 2:
        mEvtState = 0;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        break;
    case 3:
        mEvtState = 0;
        mWalkTimer = (s16)Kf1_random(90, 90);
        break;
    }
    setAnm();
}

/* 02262880 */
void daNpc_Kf1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    anm_prm_c* a_anm_prm_tbl = ::a_anm_num_tbl;
    if (i_setBtp) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}

/* 022628F0 */
void daNpc_Kf1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}

/* 02262934 */
void daNpc_Kf1_c::cut_init_ANGRY_START(int i_staffIdx) {
    cXyz* pos = (cXyz*)dComIfGp_evmng_getMyXyzP(i_staffIdx, "Pos" /* "Pos" */);
    if (pos != nullptr) {
        f32 z = pos->z, y = pos->y, x = pos->x;
        current.pos.y = y;
        current.pos.z = z;
        current.pos.x = x;
        /* *mObjAcch.GetOldPos() = *mObjAcch.GetPos() (word copy) */
        auto* positions = Kf1_view<Kf1AcchPositions>(&mObjAcch);
        be<u32>* src = positions->mpPosition;
        u32 w0 = src[0];
        be<u32>* dst = positions->mpOldPosition;
        dst[0] = w0;
        dst[1] = src[1];
        dst[2] = src[2];
    }
    f32 eye_y = eyePos.y;
    mLookMode = 2;
    mLookPos.z = 0.0f;
    mLookPos.y = eye_y;
    mLookPos.x = 0.0f;
    current.angle.y = cLib_targetAngleY(&current.pos, &mLookPos);
    speedF = 0.0f;
    setAnm_NUM(0, 1);
}

/* 02262A20 */
void daNpc_Kf1_c::cut_init_BENSYOU_START(int i_staffIdx) {
    cut_init_ANGRY_START(i_staffIdx);
    cXyz goal;
    goal.x = 0.0f;
    goal.z = 700.0f;
    goal.y = 0.0f;
    dComIfGp_evmng_setGoal(&goal);
}

/* 02262A6C */
void daNpc_Kf1_c::cut_init_TSUBO_CNT(int i_staffIdx) {
    s32 num = chk_tsubo();
    m_jnt.mAngles[0][0] = 0;
    m_jnt.mAngles[1][0] = 0;
    mBrokenNum = (s16)(mTsuboNum - num);
    m_jnt.mAngles[0][1] = 0;
    mLookMode = 0;
    m_jnt.mAngles[1][1] = 0;
}

/* 02262ABC */
void daNpc_Kf1_c::cut_init_BENSYOU(int i_staffIdx) {
    s32 broken = mBrokenNum;
    mRupee = dComIfGs_getRupee();
    dComIfGp_setItemRupeeCount(-broken * 10);
}

/* 02262B08 */
void daNpc_Kf1_c::cut_init_GET_OUT(int i_staffIdx) {
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Timer" /* "Timer" */);
    mCutTimer = 0;
    if (timer != nullptr) {
        mCutTimer = (s16)(s32)*timer;
    }
    /* player + 0x420/0x422/0x428/0x430: the player's event move */
    Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0))->mAngle = 0;
    auto* player = Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0));
    player->mMode = 3;
    player->mMoveMode = 0;
    if ((s32)mRupee >= mBrokenNum * 10) {
        cXyz sea;
        sea.x = 0.0f;
        sea.z = 999.0f;
        sea.y = 0.0f;
        cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &sea); /* result unused */
        Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0))->mOriginalDemo = 3;
    } else {
        Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0))->mMoveMode = 1;
        Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0))->mOriginalDemo = 9;
    }
}

/* 02262C1C */
void daNpc_Kf1_c::cut_init_DSP_RUPEE_CNT(int i_staffIdx) { Kf1_offRupeeCounter(); }

/* 02262C48 */
void daNpc_Kf1_c::cut_init_PLYER_TRN(int i_staffIdx) {
    s16 ang = cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &current.pos);
    Kf1_view<Kf1PlayerDemo>(dComIfGp_getPlayer(0))->mAngle = ang;
}

/* 02262C94 */
void daNpc_Kf1_c::cut_init_START_AGE(int i_staffIdx) {
    mLookPos.y = eyePos.y;
    mLookPos.x = 0.0f;
    mLookPos.z = 0.0f;
    s16 ang = cLib_targetAngleY(&current.pos, &mLookPos);
    mLookAngleY = ang;
    shape_angle.y = ang;
    m_jnt.mbTrn = 1; /* setTrn() */
    mLookMode = 3;
    mbNoShapeAngle = true;
    setAnm_NUM(0, 1);
}

/* 02262D08 */
void daNpc_Kf1_c::cut_init_PLYER_MOV(int i_staffIdx) {
    s16 diff = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) -
                     current.angle.y);
    if (abs((int)diff) > 0x2000) {
        cXyz* pos = &dComIfGp_getPlayer(0)->current.pos;
        dComIfGp_evmng_setGoal(pos);
        return;
    }
    cXyz offset;
    offset.y = 0.0f;
    offset.x = 0.0f;
    offset.z = 0.0f;
    s16 rot = diff > 0 ? 0x2800 : -0x2800;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(current.angle.y + rot));
    offset.z = 150.0f;
    cXyz goal;
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &goal);
    dComIfGp_evmng_setGoal(&goal);
}

/* Three of the 8 pots, chosen at random, will hold the rupees.
 * The random index is not range-checked (cM_rndF(32) / 4 is 0..7 in the game); to stay exact for any
 * index, the candidate keeps the original's 0x70-byte stack frame at the same address: back chain,
 * the search key (0x8), the float-to-int temporary (0x10), used[8] (0x18), saved r20..r31 (0x20),
 * f30/f31 (0x50/0x60, with their second halves as singles at 0x58/0x68), and the saved LR at 0x74. */
/* 02262E10 */
void daNpc_Kf1_c::cut_init_RUPEE_SET(int i_staffIdx) {
    // HD: retain the original 0x70-byte frame, including saved registers and caller LR.
    Kf1RupeeSelectionFrame frame;
    Kf1_saveRupeeFrame(&frame);
    if (mTsuboNum == 8) {
        frame.mUsed[0] = 0;
        frame.mUsed[1] = 0;
        for (int i = 0; i < 3; i++) {
            s32 idx;
            do {
                idx = gabi::ftoi(cM_rndF(32.0f) * 0.25f);
                frame.mIndex = idx;
            } while ((u8)Kf1_usedPot(&frame, idx) != 0);
            /* fopAcM_SearchByID(mTsuboIds[idx]) (HD inline; the key at 0x8) */
            u32 id = mTsuboIds[idx];
            frame.mSearchKey = id;
            fopAc_ac_c* a_tsubo_actor =
                id == 0xFFFFFFFFu ? nullptr : fopAcIt_Judge(Kf1JudgeActorId, &frame.mSearchKey);
            if (a_tsubo_actor == nullptr) /* JUT_ASSERT(1485, NULL != a_tsubo_actor) */
                JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x5CD, "(0) != a_tsubo_actor");
            a_tsubo_actor->mParameters =
                (a_tsubo_actor->mParameters & ~0x3Fu) | 4; /* the pot's item: a rupee */
            dComIfGp_event_setItemPartner(a_tsubo_actor);
            id = mTsuboIds[idx];
            Kf1_usedPot(&frame, idx) = 1;
            mKutaniIds[i] = id;
        }
    }
    setAnm_NUM(8, 1);
}

/* 02262F64 */
void daNpc_Kf1_c::cut_init_TSUBO_ATN(int i_staffIdx) {
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Timer" /* "Timer" */);
    be<s32>* count = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Count" /* "Count" */);
    mCutTimer = 0;
    if (timer != nullptr) {
        mCutTimer = (s16)(s32)*timer;
    }
    mCutCount = 0;
    if (count != nullptr) {
        mCutCount = (s16)(s32)*count;
    }
}

/* 02263008 */
void daNpc_Kf1_c::cut_init_TLK_MSG(int i_staffIdx) {
    be<u32>* msg_num = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "MsgNum" /* "MsgNum" */);
    be<u32>* end_msg = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "EndMsg" /* "EndMsg" */);
    mAnmAtr = 0xFF;
    mCurrMsgNo = 0;
    mEndMsgNo = 0xFFFFFFFF;
    mMesgAnimeTag = 0xFF;
    mAtrSet = 0;
    if (end_msg != nullptr) {
        mEndMsgNo = *end_msg;
    }
    if (msg_num != nullptr) {
        u32 no = *msg_num;
        mCurrMsgNo = no;
        switch (no) {
        case 0x1C2D: {
            s16 v = (s16)(mBrokenNum * 10);
            dComIfGp_setMessageCountNumber(v); /* the message's number value */
            break;
        }
        case 0x1C2F:
        case 0x1C30:
            mCurrMsgNo = (s32)mRupee >= mBrokenNum * 10 ? 0x1C2F : 0x1C30;
            break;
        case 0x1C39: {
            cXyz up;
            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x21, &up);
            break;
        }
        }
    }
    mCurrMsgBsPcId = 0xFFFFFFFF;
}

/* 02263170 */
void daNpc_Kf1_c::cut_init_CONTNUE_TLK(int i_staffIdx) {
    be<u32>* end_msg = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "EndMsg" /* "EndMsg" */);
    mEndMsgNo = 0xFFFFFFFF;
    if (end_msg != nullptr) {
        mEndMsgNo = *end_msg;
    }
}

/* 022631D8 */
u32 daNpc_Kf1_c::cut_move_GET_OUT() {
    if (Kf1_calcTimer(&mCutTimer) == 0) {
        Kf1_nextStage("sea" /* "sea" */, 3, 0xB, -1, 0.0f, 0, 1, 0);
    }
    return FALSE;
}

/* 02263234 */
u32 daNpc_Kf1_c::cut_move_RUPEE_CNT_END() {
    u32 p = dComIfGp_ea();
    if (dComIfGs_getRupee() == (u16)Kf1_savedRupeeAt(p)->mSavedRupee) {
        Kf1_onRupeeCounter();
        return TRUE;
    }
    return FALSE;
}

/* 02263294 */
u32 daNpc_Kf1_c::cut_move_START_AGE() { return (u8)(m_jnt.mbTrn ^ 1); }

/* 022632A4 */
void daNpc_Kf1_c::create_rupee(cXyz* i_pos, int i_num) {
    /* the stack frame: the angle, then the offset table */
    Kf1RupeeCreationFrame frm;
    csXyz_ct(&frm.angle, 0, 0, 0);
    s32 cnt = (s32)l_frameCounter;
    for (int i = 0; i < i_num; i++, cnt++) {
        /* static const f32 a_offs[3] = {-30, 0, 30} (copied) */
        u32 w0 = l_rupeeOffsets[0], w1 = l_rupeeOffsets[1], w2 = l_rupeeOffsets[2];
        Kf1_view<be<u32>>(frm.offs)[0] = w0;
        Kf1_view<be<u32>>(frm.offs)[1] = w1;
        Kf1_view<be<u32>>(frm.offs)[2] = w2;
        f32 r = cM_rndF(30.0f) - 15.0f;
        f32 off = Kf1_rupeeOffset(frm.offs, cnt % 3);
        s16 deg = (s16)gabi::ftoi(r + off);
        frm.angle.y = (s16)(current.angle.y + (s16)gabi::ftoi((f32)deg * 182.0389f)); /* cM_deg2s */
        f32 speed_f = cM_rndFX(2.0f) + 13.0f;
        f32 speed_y = cM_rndFX(4.0f) + 31.0f;
        fopAc_ac_c* a_actor_p = Kf1_createRupee(i_pos, 4 /* RED_RUPEE? */, fopAcM_GetRoomNo(this),
                                                nullptr, nullptr, speed_f, speed_y, -2.0f, 1);
        if (a_actor_p == nullptr) { /* JUT_ASSERT(1052, a_actor_p != NULL) */
            JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x41C, "a_actor_p != (0)");
            continue;
        }
        u32 st = a_actor_p->actor_status;
        a_actor_p->scale.x = 0.2f;
        a_actor_p->scale.y = 0.2f;
        a_actor_p->scale.z = 0.2f;
        a_actor_p->actor_status = (st | 0x4000) & ~0x80u;
        u16 ax = (u16)frm.angle.x;
        a_actor_p->shape_angle.x = ax;
        u16 ay = (u16)frm.angle.y;
        a_actor_p->shape_angle.y = ay;
        u16 az = (u16)frm.angle.z;
        a_actor_p->shape_angle.z = az;
        a_actor_p->current.angle.x = ax;
        a_actor_p->current.angle.y = ay;
        a_actor_p->current.angle.z = az;
        mRupeeIds[i] = fopAcM_GetID(a_actor_p); /* fopAcM_GetID */
    }
}

/* 022635C4 */
void daNpc_Kf1_c::ready_kutaniCamera(int i_idx, int i_flg) {
    Kf1GuestInt is_deleted;
    fopAc_ac_c* a_actor = searchByID(mCameraItemId, &is_deleted);
    if (a_actor != nullptr && is_deleted == 0) {
        fopAcM_delete(a_actor);
    }
    if (!i_flg) {
        return;
    }
    csXyz angle;
    csXyz_ct(&angle, 0, 0, 0);
    a_actor = searchByID(mKutaniIds[i_idx], &is_deleted);
    if (a_actor == nullptr || is_deleted != 0) /* JUT_ASSERT(1084, NULL != a_actor && 0 == i_flg) */
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x43C, "(0) != a_actor && 0 == i_flg");
    dComIfGp_event_setItemPartner(a_actor);
    angle.y = a_actor->current.angle.y;
    cXyz pos;
    pos.x = a_actor->current.pos.x;
    pos.y = a_actor->current.pos.y + 180.0f;
    pos.z = a_actor->current.pos.z;
    fopAc_ac_c* item =
        Kf1_createRupee(&pos, 4, fopAcM_GetRoomNo(this), nullptr, nullptr, 0.0f, 0.0f, -4.0f, 1);
    if (item == nullptr) /* JUT_ASSERT(1096, NULL != a_actor) */
        JUT_ASSERT_fail("d_a_npc_kf1.cpp", 0x448, "(0) != a_actor");
    u32 st = item->actor_status;
    item->scale.z = 0.1f;
    item->scale.y = 0.1f;
    item->scale.x = 0.1f;
    item->actor_status = (st | 0x4000) & ~0x80u;
    item->current.angle.x = (u16)angle.x;
    item->current.angle.y = (u16)angle.y;
    item->current.angle.z = (u16)angle.z;
    mCameraItemId = item != nullptr ? fopAcM_GetID(item) : 0xFFFFFFFF;
}

/* 022637C4 */
u32 daNpc_Kf1_c::cut_move_RUPEE_SET() {
    Kf1GuestInt is_deleted;
    if (mbMorfAnimStopped == 0) {
        if (mpMorf->checkFrame(68.0f)) {
            cXyz offset;
            offset.x = 0.0f;
            offset.z = 40.0f;
            offset.y = 40.0f;
            mDoAud_seStart(0x69E9, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_stack_c::YrotM(current.angle.y);
            cXyz pos;
            PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &pos);
            cXyz pos_copy;
            f32 x = pos.x, y = pos.y;
            pos_copy.x = x;
            f32 z = pos.z;
            pos_copy.y = y;
            pos_copy.z = z;
            create_rupee(&pos_copy, 3);
            return FALSE;
        }
        if (68.0f < mpMorf->getFrame()) {
            for (int i = 0; i < 3; i++) {
                fopAc_ac_c* a = searchByID(mRupeeIds[i], &is_deleted);
                if (a != nullptr && is_deleted == 0) {
                    eyePos.copy(a->current.pos);
                }
            }
            mbSetEyePos = 0;
        }
        return FALSE;
    }
    u32 num = 0;
    for (int i = 0; i < 3; i++) {
        fopAc_ac_c* a = searchByID(mRupeeIds[i], &is_deleted);
        if (a != nullptr) {
            fopAcM_delete(a);
            num++;
        } else if (is_deleted != 0) {
            num++;
        }
    }
    if (num != 3) {
        return FALSE;
    }
    ready_kutaniCamera(0, 1);
    mbSetEyePos = 1;
    setAnm_NUM(0, 1);
    return TRUE;
}

/* 022639E0 */
u32 daNpc_Kf1_c::cut_move_TSUBO_ATN() {
    if (Kf1_calcTimer(&mCutTimer) == 0) {
        u32 count = (u32)(s32)mCutCount;
        if (count < 4) {
            if (count <= 2) {
                ready_kutaniCamera((s32)count, 1);
            } else {
                ready_kutaniCamera(0, 0);
            }
        }
        return TRUE;
    }
    if (mCutTimer == 0x14) {
        mDoAud_seStart(0x6981, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
    return FALSE;
}

/* 02263A94 */
u32 daNpc_Kf1_c::cut_move_TLK_MSG() {
    u16 status = talk(0);
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        mAnmAtr = 0xFF;
        return TRUE;
    }
    if (status == 2 || status == 6) {
        return mCurrMsgNo == mEndMsgNo;
    }
    return FALSE;
}

/* 02263B34 */
void daNpc_Kf1_c::privateCut(int i_staffIdx) {
    /* a_cut_tbl (.data 0x101BEF08): ANGRY_START, BENSYOU_START, TSUBO_CNT, BENSYOU, GET_OUT, DSP_RUPEE_CNT,
     * PLYER_TRN, RUPEE_CNT_END, START_AGE, PLYER_MOV, RUPEE_SET, TSUBO_ATN, TLK_MSG, CONTNUE_TLK */
    if (i_staffIdx == -1) {
        return;
    }
    mActionIndex = (s8)dComIfGp_evmng_getMyActIdx(i_staffIdx, a_cut_tbl, 14, TRUE, 0);
    if (mActionIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch ((u32)(s32)mActionIndex) {
        case 0:
            cut_init_ANGRY_START(i_staffIdx);
            break;
        case 1:
            cut_init_BENSYOU_START(i_staffIdx);
            break;
        case 2:
            cut_init_TSUBO_CNT(i_staffIdx);
            break;
        case 3:
            cut_init_BENSYOU(i_staffIdx);
            break;
        case 4:
            cut_init_GET_OUT(i_staffIdx);
            break;
        case 5:
            cut_init_DSP_RUPEE_CNT(i_staffIdx);
            break;
        case 6:
            cut_init_PLYER_TRN(i_staffIdx);
            break;
        case 8:
            cut_init_START_AGE(i_staffIdx);
            break;
        case 9:
            cut_init_PLYER_MOV(i_staffIdx);
            break;
        case 10:
            cut_init_RUPEE_SET(i_staffIdx);
            break;
        case 11:
            cut_init_TSUBO_ATN(i_staffIdx);
            break;
        case 12:
            cut_init_TLK_MSG(i_staffIdx);
            break;
        case 13:
            cut_init_CONTNUE_TLK(i_staffIdx);
            break;
        }
    }
    u32 cut_end;
    switch ((u32)(s32)mActionIndex) {
    case 4:
        cut_end = cut_move_GET_OUT();
        break;
    case 7:
        cut_end = cut_move_RUPEE_CNT_END();
        break;
    case 8:
        cut_end = cut_move_START_AGE();
        break;
    case 10:
        cut_end = cut_move_RUPEE_SET();
        break;
    case 11:
        cut_end = cut_move_TSUBO_ATN();
        break;
    case 12:
    case 13:
        cut_end = cut_move_TLK_MSG();
        break;
    default:
        cut_end = TRUE;
        break;
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 02263E2C */
void daNpc_Kf1_c::event_proc(int i_staffIdx) {
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0: /* "angry" */
            dComIfGs_onEventBit(0x2780);
            mTsuboNum = chk_tsubo();
            break;
        case 1: { /* "rupee_age" */
            u8 reg = dComIfGs_getEventReg(0xBCFF);
            dComIfGs_setEventReg(0xBCFF, reg | 1);
            mbNoShapeAngle = false;
            setStt(1);
            setAnm_NUM(0, 1);
            mWaitTimer = (s16)Kf1_random(30, 60);
            mTalkTimer = (s16)Kf1_random(15, 30);
            mbRupeesGiven = 1;
            break;
        }
        }
        mWaitTimer = (s16)Kf1_random(30, 60);
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 02263FF0 */
void daNpc_Kf1_c::lookBack() {
    cXyz look_pos;
    look_pos.x = 0.0f;
    f32 srcX = current.pos.x;
    s16 target_y = current.angle.y;
    look_pos.y = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    u8 head_only = mbHeadOnly;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    look_pos.z = 0.0f;
    mActorAngleY = target_y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        cXyz eye;
        Kf1_playerEyePos(&eye, -20.0f);
        mLookPos.copy(eye);
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
    case 4: {
        Kf1GuestInt is_deleted;
        fopAc_ac_c* a = searchByID(mLookActorId, &is_deleted);
        if (a != nullptr && is_deleted == 0) {
            mLookPos.copy(a->current.pos);
            look_pos.x = mLookPos.x;
            f32 eye_y = a->eyePos.y;
            look_pos.y = eye_y;
            look_pos.z = mLookPos.z;
            look_pos_p = &look_pos;
            mLookPos.y = eye_y;
        }
        break;
    }
    }
    cXyz cur_pos; /* passed by value: a copy */
    cur_pos.x = srcX;
    cur_pos.z = srcZ;
    cur_pos.y = srcY;
    Kf1_lookAtTarget(&m_jnt, &current.angle.y, look_pos_p, &cur_pos, target_y,
                     l_HIO.mPrm.mLookVelMax, head_only);
}

/* 02264284 */
void daNpc_Kf1_c::eventOrder() {
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 022642F4 */
BOOL daNpc_Kf1_c::_execute() {
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Kf1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mbWaitForDemo && demoActorID == 0) {
        return TRUE;
    }
    srch_Tsubo();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if ((dComIfGp_event_runCheck() &&
             (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */ &&
             (staff_id = isEventEntry()) >= 0) ||
            mbForceEvent) {
            event_proc(staff_id);
        } else {
            Kf1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        u32 flags = mObjAcch.m_flags;
        mbAcchFlag20 = (flags >> 5) & 1;
        mbAcchFlag1000 = (flags >> 12) & 1;
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        mbWaitForDemo = 0;
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
    tevStr.mRoomNo = Kf1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Kf1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(false);
    if (!mbDemo) {
        setCollision(90.0f, 200.0f);
    }
    return TRUE;
}

/* 022645F4 */
static BOOL daNpc_Kf1_Execute(daNpc_Kf1_c* i_this) { return i_this->_execute(); }

/* 022645F8 */
BOOL daNpc_Kf1_c::_draw() {
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = morf_model_p->getModelData();
    if (mbWaitForDemo || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    Kf1_view<Kf1ModelAnimation>(morf_model_info_p)->mpBtpAnimation = 0; /* HD animation removal */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    // HD: no shadowDraw()
    dSnap_RegistFig(0x59 /* DSNAP_TYPE_NPC_KF1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrm.mDebugDraw) {
        Kf1_initDebugColour(&l_debugGuard0, &l_debugColour0, &l_debugSource0);
        Kf1_initDebugColour(&l_debugGuard1, &l_debugColour1, &l_debugSource1);
        Kf1_initDebugColour(&l_debugGuard2, &l_debugColour2, &l_debugSource2);
    }
    return TRUE;
}

/* 02264780 */
static BOOL daNpc_Kf1_Draw(daNpc_Kf1_c* i_this) { return i_this->_draw(); }

/* 02264784 */
static BOOL daNpc_Kf1_IsDelete(daNpc_Kf1_c*) { return TRUE; }

/* 0226478C */
void daNpc_Kf1_c::setAnm_ATR() {
    anm_prm_c* a_anm_prm_tbl = ::a_anm_atr_tbl; /* [11] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}

/* 022647F4 */
void daNpc_Kf1_c::chngAnmAtr(u8 i_atr) {
    if (mCurrMsgNo == 0x1C2E) {
        Kf1_offRupeeCounter();
    }
    if (i_atr == mAnmAtr || i_atr >= 11) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}

/* 02264864 */
void daNpc_Kf1_c::ctrlAnmAtr() {
    u8 atr = mAnmAtr;
    if ((atr == 7 || atr == 9) && mbMorfAnimStopped) {
        setAnm_NUM(0, 1);
        mAnmAtr = 0;
    }
}

/* 022648C4 */
void daNpc_Kf1_c::anmAtr(u16 i_msgStatus) {
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

/* 02264984 */
u16 daNpc_Kf1_c::next_msgStatus(be<u32>* o_msgNoP) {
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    Kf1Message* mesg = Kf1_view<Kf1Message>(
        l_message.get()); /* HD message manager: the selected answer at +0x948 */
    switch (*o_msgNoP) {
    case 0x1C21:
        *o_msgNoP = 0x1C22;
        break;
    case 0x1C22:
        *o_msgNoP = 0x1C34;
        break;
    case 0x1C23: {
        u32 sel = (u32)mesg->mSelectedAnswer;
        if (sel == 0) {
            *o_msgNoP = 0x1C24;
        } else if (sel == 1) {
            *o_msgNoP = 0x1C25;
        }
        break;
    }
    case 0x1C24:
        dComIfGs_onEventBit(0x0B02);
        msg_status = 0x10;
        break;
    case 0x1C27:
        *o_msgNoP = 0x1C28;
        break;
    case 0x1C28: {
        u32 sel = (u32)mesg->mSelectedAnswer;
        if (sel == 0) {
            *o_msgNoP = 0x1C2A;
        } else if (sel == 1) {
            *o_msgNoP = 0x1C29;
        }
        break;
    }
    case 0x1C2A:
        *o_msgNoP = 0x1C2B;
        break;
    case 0x1C2B:
        mbCompensationAgreed = 1;
        msg_status = 0x10;
        break;
    case 0x1C2D:
        *o_msgNoP = 0x1C2E;
        break;
    case 0x1C30:
        *o_msgNoP = 0x1C31;
        break;
    case 0x1C33:
        *o_msgNoP = (s32)mRupee >= mBrokenNum * 10 ? 0x1C2F : 0x1C30;
        break;
    case 0x1C34:
        *o_msgNoP = 0x1C23;
        break;
    case 0x1C36:
        *o_msgNoP = 0x1C37;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 02264B38 */
u32 daNpc_Kf1_c::getMsg_KF1_0() {
    if (mbRupeesGiven) {
        return 0x1C38;
    }
    if (dComIfGs_isEventBit(0x0A02) && !dComIfGs_isSymbol(0)) {
        return 0x1C3B;
    }
    if (dComIfGs_isEventBit(0x0B02)) {
        u8 reg = dComIfGs_getEventReg(0xBCFF);
        if (dKy_daynight_check() == 1 || !dComIfGs_isEventBit(0x2780) || (reg & 1)) {
            return 0x1C26;
        }
        return 0x1C27;
    }
    return 0x1C21;
}

/* 02264C74 */
u32 daNpc_Kf1_c::getMsg() {
    if (mType == 0) {
        return getMsg_KF1_0();
    }
    return 0;
}

/* 02264CAC */
u8 daNpc_Kf1_c::chk_talk() {
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

/* 02264D44 */
u8 daNpc_Kf1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Kf1_lockonTruth(attention)) {
        return this == (void*)Kf1_lockonTarget(attention, 0);
    }
    return this == (void*)Kf1_actionTarget(attention, 0);
}

/* 02264DCC */
u8 daNpc_Kf1_c::orderTsuboEvent() {
    if (mbTalk) {
        return false;
    }
    if (!dComIfGs_isEventBit(0x2780)) {
        s32 num = chk_tsubo();
        if (mTsuboNum > num) {
            mEvtState = 3;
            return true;
        }
        return false;
    }
    if (mSwitchNo != 0xFF && dComIfGs_isSwitch(mSwitchNo, fopAcM_GetRoomNo(this))) {
        s32 num = chk_tsubo();
        if (mTsuboNum > num) {
            mEvtState = 5;
            return true;
        }
    }
    return false;
}

/* 02264EC0 */
BOOL daNpc_Kf1_c::wait_1() {
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mLookMode = 1;
        }
        return TRUE;
    }
    s8 bck = mBckNum;
    if (mEvtState < 3) {
        mEvtState = 2;
    }
    mbHeadOnly = true;
    if (bck == 9) {
        mWaitTimer = 0;
    }
    if (Kf1_calcTimer(&mWaitTimer) == 0) {
        if (mBckNum != 9) {
            setAnm_NUM(9, 1);
            mWalkTimer = 0;
        }
        if (Kf1_calcTimer(&mWalkTimer) == 0) {
            setStt(3);
            return TRUE;
        }
    }
    mLookMode = 0;
    return TRUE;
}

/* 02264FC4 */
BOOL daNpc_Kf1_c::walk_1() {
    dPath* path = mPathRun.mPath;
    if (path == nullptr || ((u8)Kf1_view<Kf1Path>(path)->mClosed & 1) == 0 /* dPath_ChkClose() */) {
        return TRUE;
    }
    cXyz pos;
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (mPathRun.chkPointPass(&pos, mPathRun.mbDir != 0)) {
        mPathRun.nextIdxAuto();
    }
    cXyz pnt;
    mPathRun.getPoint(&pnt, mPathRun.mIdx);
    cXyz target;
    target.copy(pnt);
    s16 ang = cLib_targetAngleY(&current.pos, &target);
    cLib_addCalcAngleS(&current.angle.y, ang, l_HIO.mPrm.mWalkTurnScale, l_HIO.mPrm.mWalkTurnStep,
                       0x80);
    f32 spd = l_HIO.mPrm.mWalkSpeed;
    if (Kf1_calcTimer(&mWalkTimer) == 0 || mbTalk) {
        spd = 0.0f;
    }
    cLib_chaseF(&speedF, spd, l_HIO.mPrm.mWalkAccel);
    f32 rate = speedF * l_HIO.mPrm.mWalkAnmRate;
    s32 ispd = gabi::ftoi(spd);
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
    if (ispd == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk) {
            if (chk_talk()) {
                setStt(1);
                setAnm_NUM(9, 1);
                mbHeadOnly = false;
                m_jnt.mbTrn = 1; /* setTrn() */
                mLookMode = 1;
            }
        } else {
            setStt(1);
            setAnm_NUM(9, 1);
        }
        return TRUE;
    }
    if (mEvtState < 3) {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    return TRUE;
}

/* 022651E0 */
BOOL daNpc_Kf1_c::talk_1() {
    bool sold = false;
    talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    // HD: the message status comes from the message manager
    if (Kf1_messageStatus(l_message.get()) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        if (mCurrMsgNo == 0x1C2B) {
            sold = true;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        setAnm_NUM(0, 1);
        mWaitTimer = (s16)Kf1_random(30, 60);
        mTalkTimer = (s16)Kf1_random(15, 30);
        if (sold) {
            mEvtState = 4;
        }
        endEvent();
    }
    return mBckNum != 4;
}

/* 022652F0 */
BOOL daNpc_Kf1_c::wait_action1(void*) {
    s8 act = mActState;
    if (act == 0) {
        setStt(3);
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)act > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case 1:
        mbSetEyePos = wait_1();
        break;
    case 2:
        mbSetEyePos = talk_1();
        break;
    case 3:
        mbSetEyePos = walk_1();
        break;
    }
    if (mActState > 1) {
        orderTsuboEvent();
    }
    return TRUE;
}

/* 022653DC */
static daNpc_Kf1_HIO_c* daNpc_Kf1_HIO_c_ct(daNpc_Kf1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Kf1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001B8D8;
    memcpy_g(&i_this->mPrm, &a_prm_tbl, 0x30); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->mEntryID = -1;
    return i_this;
}

/* 02265448 */
static void __sinit_d_a_npc_kf1_cpp() {
    sinit_header_statics(0x104674C4, 0x101BF020);
    daNpc_Kf1_HIO_c_ct(&l_HIO); /* static daNpc_Kf1_HIO_c l_HIO */
}

/* 022654E8 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022654FC */
static void daNpc_Kf1_c_dt(daNpc_Kf1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Kf1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Kf1_view<Kf1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001B8B8;
        Kf1_view<Kf1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001B8C8;
        Kf1_acchDestroy(&i_this->mObjAcch, 0);
        Kf1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02265598 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
ext 021E1E78 s32 Kf1_random(s32,s32)
ext 025E789C s32 Kf1_btpInit(void*,J3DModelData*,void*,s32,s32,f32,s32,s32,u32,s32)
ext 024EC8D0 fopAc_ac_c* Kf1_lockonTarget(dAttention_c*,s32)
ext 024EE464 fopAc_ac_c* Kf1_actionTarget(dAttention_c*,s32)
ext 024EF130 s8 Kf1_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 Kf1_polyColour(dBgS*,cBgS_PolyInfo*)
ext 024EECAC u32 Kf1_materialSound(dBgS*,cBgS_PolyInfo*)
ext 0259D54C void Kf1_playerEyePos(cXyz*,f32)
ext 0259E2D4 void Kf1_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 025D54C4 BOOL Kf1_searchByID(fpc_ProcID,be<u32>*)
ext 0259E778 void Kf1_pathPoint(dNpc_PathRun_c*,cXyz*,u32)
ext 02526E70 void* Kf1_demoActor(u32,u8)
ext 02527828 void* Kf1_demoBtp(void*,const char*)
ext 02527028 void Kf1_demoData(daNpc_Kf1_c*,u32,mDoExt_McaMorf*,const char*,s32,u32,u32,s32)
ext 02018034 void Kf1_circleDestroy(void*,s32)
ext 024EFD9C void Kf1_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Kf1_actorDestroy(daNpc_Kf1_c*,s32)
ext 025F19F8 void Kf1_XYZrotM(Mtx34*,s16,s16,s16)
ext 0252012C void Kf1_nextStage(const char*,s16,s8,s8,f32,u32,s32,s8)
ext 025D8E6C fopAc_ac_c* Kf1_createRupee(cXyz*,s32,s32,csXyz*,cXyz*,f32,f32,f32,s32)
str 1001B924 "d_a_npc_kf1.cpp" in 022611E8
str 1001B934 "a_btp != (0)" in 022611E8
str 1001B954 "d_a_npc_kf1.cpp" in 022612D8
str 1001B964 "a_mdl_dat != (0)" in 022612D8
str 1001B944 "head" in 022612D8
str 1001B954 "d_a_npc_kf1.cpp" in 022612D8
str 1001B978 "m_hed_jnt_num >= 0" in 022612D8
str 1001B98C "backbone" in 022612D8
str 1001B954 "d_a_npc_kf1.cpp" in 022612D8
str 1001B998 "m_bbone_jnt_num >= 0" in 022612D8
str 1001B94C "neck" in 022612D8
str 1001B954 "d_a_npc_kf1.cpp" in 022612D8
str 1001B9B0 "m_nck_jnt_num >= 0" in 022612D8
str 1001B9C4 "d_a_npc_kf1.cpp" in 02261624
str 1001B9D4 "a_mdl_dat != (0)" in 02261624
str 1001BA08 "Kf1" in 02261E4C
str 1001B904 "d_demo.h" in 022623CC
str 1001B8E8 "m_object != (0)" in 022623CC
str 1001BA48 "Pos" in 02262934
str 1001BA54 "Timer" in 02262B08
str 1001BA68 "d_a_npc_kf1.cpp" in 02262E10
str 1001BA78 "(0) != a_tsubo_actor" in 02262E10
str 1001BA90 "Timer" in 02262F64
str 1001BA98 "Count" in 02262F64
str 1001BAA0 "MsgNum" in 02263008
str 1001BAA8 "EndMsg" in 02263008
str 1001BAB0 "EndMsg" in 02263170
str 1001BAB8 "sea" in 022631D8
str 1001BADC "d_a_npc_kf1.cpp" in 022632A4
str 1001BAF8 "a_actor_p != (0)" in 022632A4
str 1001BB18 "d_a_npc_kf1.cpp" in 022635C4
str 1001BB28 "(0) != a_actor && 0 == i_flg" in 022635C4
str 1001BB18 "d_a_npc_kf1.cpp" in 022635C4
str 1001BB48 "(0) != a_actor" in 022635C4
ext 02055B64 s16 Kf1_calcTimer(be<s16>*)
ext 024EDFCC bool Kf1_lockonTruth(dAttention_c*)
ext 025A1458 void Kf1_npcCtor(daNpc_Kf1_c*)
ext 025E7820 void Kf1_btpCtor(void*)
ext 025F795C u32 Kf1_messageStatus(void*)
ext 02048038 u32 Kf1_prmAbstract(fopAc_ac_c*, s32, s32)
ext 025DE508 void* Kf1_searchActors(u32, void*)
*/
