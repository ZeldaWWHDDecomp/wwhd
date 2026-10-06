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

#define SAFESTRING_VTBL 0x1001F080
#define OB1_VTBL 0x1001F238
#define HIO_VTBL 0x1001F0B8
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */
#define nodeCallBack_Ob1_addr 0x022A6274u
#define searchActor_Kb_addr 0x022A6038u
#define CheckCreateHeap_addr 0x022A67D4u

/* TU globals: the pig search (searchActor_Kb fills them) */
static inline be<s32>& l_pig_cnt() { return *gabi::at<be<s32>>(0x10467D30); }
static inline be<u8>& l_pig_bits() { return *gabi::at<be<u8>>(0x10467D3C); }
static inline u32 l_pig_tbl(s32 i) { return 0x10467D98 + 4 * i; } /* fopAc_ac_c* [20] */
#define l_HIO l_HIO_ob1()

/* TU wrapper: resource names are SafeStrings with this TU's vtable */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 index) { return dComIfG_getObjectIDRes(arc, index, SAFESTRING_VTBL); }

/* 022A6038 */
static void* searchActor_Kb(void* p, void*) {
    WWHD_FUNC(0x022A6038, void*, p, (u32)0);
    if (l_pig_cnt() < 20 && fopAc_IsActor(p) && p != nullptr && fpcM_GetName(p) == 0xDC /* PROC_KB */) {
        u8 bit = gabi::load<u8>(gabi::ea(p) + 0x74D); /* kb_class::m405: the pig's bit */
        if (bit != 0) {
            l_pig_bits() |= bit;
        }
        s32 n = l_pig_cnt();
        l_pig_cnt() = n + 1;
        gabi::store<u32>(l_pig_tbl(n), gabi::ea(p));
    }
    return nullptr;
}
VERIFY(0x022A6038, searchActor_Kb);

/* 022A60D4 */
void daNpc_Ob1_c::nodeOb1Control(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x022A60D4, void, this, node, model);
    /* static cXyz a_eye_pos_offst(20.0f, -20.0f, 0.0f) */
    cXyz* a_eye_pos_offst = gabi::at<cXyz>(0x10467D8C);
    if (gabi::load<u32>(0x10467DE8) == 0) {
        a_eye_pos_offst->z = 0.0f;
        gabi::store<u32>(0x10467DE8, 1);
        a_eye_pos_offst->x = 20.0f;
        a_eye_pos_offst->y = -20.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(node);
    u16 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), stack);
    if (jntNo == m_hed_jnt_num) {
        mDoMtx_YrotM(stack, (s16)-m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[0][0]);
        PSMTXMultVec(stack, a_eye_pos_offst, &mEyePos);
    }
    if (jntNo == m_bbone_jnt_num) {
        mDoMtx_XrotM(stack, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(stack, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stack, J3DSys_mCurrentMtx);
    mtx_copy(J3DModel_getAnmMtx(model, jntNo), stack); /* model->setAnmMtx(jntNo, stack) */
}
VERIFY(0x022A60D4, &daNpc_Ob1_c::nodeOb1Control);

/* 022A6274 */
static BOOL nodeCallBack_Ob1(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022A6274, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpc_Ob1_c* i_this = gabi::at<daNpc_Ob1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea */
        if (i_this != nullptr) {
            i_this->nodeOb1Control(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022A6274, nodeCallBack_Ob1);

/* 022A62BC */
J3DModelData* daNpc_Ob1_c::create_Anm() {
    WWHD_FUNC(0x022A62BC, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001F104) /* "Ob" */, 5 /* BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x985, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001F110), 0x985, STR(0x1001F120));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001F104), 3 /* BCK */);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(morf)) + 0xC), morf, 3); /* delete mpMorf */
        }
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x1001F108) /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(0x99B, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001F110), 0x99B, STR(0x1001F134));
    m_bbone_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x1001F148) /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(0x99E, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001F110), 0x99E, STR(0x1001F154));
    return a_mdl_dat;
}
VERIFY(0x022A62BC, &daNpc_Ob1_c::create_Anm);

/* 022A64A4 */
J3DModelData* daNpc_Ob1_c::create_hed_Mdl() {
    WWHD_FUNC(0x022A64A4, J3DModelData*, this);
    J3DModelData* a_mdl_dat =
        (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001F16C) /* "Ob" */, gabi::load<s32>(0x101C260C) /* head BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x9B9, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001F170), 0x9B9, STR(0x1001F180));
    mpHedModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11020022);
    return a_mdl_dat;
}
VERIFY(0x022A64A4, &daNpc_Ob1_c::create_hed_Mdl);

/* 022A6540 */
s32 daNpc_Ob1_c::btpNum_toResID(int num) {
    WWHD_FUNC(0x022A6540, s32, this, num);
    return gabi::load<s32>(0x1001F194 + 4 * num); /* a_btp_resID_tbl */
}
VERIFY(0x022A6540, &daNpc_Ob1_c::btpNum_toResID);

/* 022A6554 */
BOOL daNpc_Ob1_c::setBtp(s32 modify, int num) {
    WWHD_FUNC(0x022A6554, BOOL, this, modify, num);
    u32 modelData = J3DModel_modelData(mpHedModel);
    s32 resID = btpNum_toResID(num);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x1001F19C) /* "Ob" */, resID);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(0x202, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x1001F1A0), 0x202, STR(0x1001F1B0));
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_hed_tex_pttrn, 1, 2 /* EMode_LOOP */, 1.0f, 0, -1, modify, 0)) {
        return FALSE;
    }
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022A6554, &daNpc_Ob1_c::setBtp);

/* 022A6658 */
BOOL daNpc_Ob1_c::iniTexPttrnAnm(s32 modify) {
    WWHD_FUNC(0x022A6658, BOOL, this, modify);
    return setBtp(modify, mTexNo);
}
VERIFY(0x022A6658, &daNpc_Ob1_c::iniTexPttrnAnm);

/* 022A6664 */
BOOL daNpc_Ob1_c::CreateHeap() {
    WWHD_FUNC(0x022A6664, BOOL, this);
    J3DModelData* a_mdl_dat = create_Anm();
    if (a_mdl_dat == nullptr) {
        return FALSE;
    }
    if (create_hed_Mdl() != nullptr) {
        mTexNo = 0;
        if (iniTexPttrnAnm(false)) {
            for (u16 i = 0; i < J3DModelData_getJointNum(a_mdl_dat); i++) {
                if (i == m_hed_jnt_num || i == m_bbone_jnt_num) {
                    J3DModel* model = mpMorf->getModel();
                    J3DModelData_setJointCallBack(J3DModel_modelData(model), i, nodeCallBack_Ob1_addr);
                }
            }
            J3DModel* model = mpMorf->getModel();
            gabi::store<u32>(gabi::ea(model) + 0xB8, gabi::ea(this)); /* setUserArea */
            mAcchCir.SetWall(30.0f, 80.0f);
            mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
            return TRUE;
        }
    }
    mpMorf = nullptr;
    return FALSE;
}
VERIFY(0x022A6664, &daNpc_Ob1_c::CreateHeap);

/* 022A67D4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022A67D4, BOOL, i_this);
    return ((daNpc_Ob1_c*)i_this)->CreateHeap();
}
VERIFY(0x022A67D4, CheckCreateHeap);

/* 022A67D8 */
BOOL daNpc_Ob1_c::charDecide(int type) {
    WWHD_FUNC(0x022A67D8, BOOL, this, type);
    field_0x978 = 0;
    if ((u32)type >= 1 && (u32)type <= 2) {
        mType = type;
    } else {
        mType = 0;
    }
    return TRUE;
}
VERIFY(0x022A67D8, &daNpc_Ob1_c::charDecide);

/* 022A680C */
BOOL daNpc_Ob1_c::set_action(ptmf_l* action, void* arg) {
    WWHD_FUNC(0x022A680C, BOOL, this, action, arg);
    s16 idx = action->idx;
    s16 delta = action->delta;
    u32 fn = action->fn;
    if (mAction.idx == idx) {
        if (idx == 0 || (mAction.delta == delta && mAction.fn == fn)) {
            return TRUE;
        }
    }
    if (mAction.idx != 0) {
        mActStep = 0; /* GameCube (km1): -1 */
        ptmf_call1(&mAction, this, arg);
    }
    mAction.delta = delta;
    mAction.idx = idx;
    mAction.fn = fn;
    mActStep = 0;
    ptmf_call1(&mAction, this, arg);
    return TRUE;
}
VERIFY(0x022A680C, &daNpc_Ob1_c::set_action);

static inline void set_action_tbl(daNpc_Ob1_c* i_this, u32 entry) {
    gabi::Local<ptmf_l> f;
    gabi::store<u32>(gabi::ea(f.get()), gabi::load<u32>(entry));
    gabi::store<u32>(gabi::ea(f.get()) + 4, gabi::load<u32>(entry + 4));
    i_this->set_action(f, nullptr);
}

/* 022A6934 */
BOOL daNpc_Ob1_c::init_OB1_0() {
    WWHD_FUNC(0x022A6934, BOOL, this);
    if (!dComIfGs_isEventBit(0x520)) {
        set_action_tbl(this, 0x1001F060); /* &daNpc_Ob1_c::wait_action1 */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022A6934, &daNpc_Ob1_c::init_OB1_0);

/* 022A69B8 */
BOOL daNpc_Ob1_c::init_OB1_1() {
    WWHD_FUNC(0x022A69B8, BOOL, this);
    if (dComIfGs_isEventBit(0x520) && !dKy_daynight_check()) {
        actor_status &= ~0x80u;
        set_action_tbl(this, 0x1001F068); /* &daNpc_Ob1_c::wait_action2 */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022A69B8, &daNpc_Ob1_c::init_OB1_1);

/* 022A6A54 */
BOOL daNpc_Ob1_c::init_OB1_2() {
    WWHD_FUNC(0x022A6A54, BOOL, this);
    if (dComIfGs_isEventBit(0x520) && dKy_daynight_check() == 1) {
        set_action_tbl(this, 0x1001F068); /* &daNpc_Ob1_c::wait_action2 */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022A6A54, &daNpc_Ob1_c::init_OB1_2);

/* 022A6AE4 */
void daNpc_Ob1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x022A6AE4, void, this);
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
VERIFY(0x022A6AE4, &daNpc_Ob1_c::plyTexPttrnAnm);

/* 022A6BB8 */
void daNpc_Ob1_c::setAttention(s32 force) {
    WWHD_FUNC(0x022A6BB8, void, this, force);
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attnPos->set(current.pos.x, current.pos.y + l_HIO.mAttnYOffset, current.pos.z);
    if (mActRet == 0 && !force) {
        return;
    }
    eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z);
}
VERIFY(0x022A6BB8, &daNpc_Ob1_c::setAttention);

/* 022A6C0C */
void daNpc_Ob1_c::setMtx(s32 force) {
    WWHD_FUNC(0x022A6C0C, void, this, force);
    if (mbDemo == 0) {
        plyTexPttrnAnm();
        mAnmEnd = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevFrame) {
            mAnmEnd = 1;
        }
        mPrevFrame = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    gabi::store<s8>(gabi::ea(this) + 0x1C9, (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)));      /* tevStr.mRoomNo */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch)));   /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(mAngle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    /* mpHedModel->setBaseTRMtx(mpMorf->getModel()->getAnmMtx(m_hed_jnt_num)) */
    Mtx34* src = J3DModel_getAnmMtx(mpMorf->getModel(), m_hed_jnt_num);
    J3DModel_setBaseTRMtx(mpHedModel, src);
    J3DModel_calc(mpHedModel);
    setAttention(force);
}
VERIFY(0x022A6C0C, &daNpc_Ob1_c::setMtx);

/* 022A6E10 */
BOOL daNpc_Ob1_c::createInit() {
    WWHD_FUNC(0x022A6E10, BOOL, this);
    const char* evName = gabi::at<const char>(gabi::load<u32>(0x101C25E8)); /* l_evn_tbl[0] "Get_Rupee" */
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(evName, 0xFF);
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    mHomeAngle.x = current.angle.x;
    mHomeAngle.y = current.angle.y;
    mHomeAngle.z = current.angle.z;
    mHomePos.copy(current.pos);
    gravity = -4.5f;
    mLookPos.copy(current.pos);
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAC); /* attention_info.distances[3] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAC); /* attention_info.distances[1] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return FALSE;
        }
        actor_status &= ~0x80u;
        weight = 0xD9;
    }
    setActorInfo2(&mEventCut2, STR(0x1001F1D8) /* "Ob1" */, this);
    mAnmNo = 8;
    switch (mType) {
    case 0:
        if (!init_OB1_0()) return FALSE;
        break;
    case 1:
        if (!init_OB1_1()) return FALSE;
        break;
    case 2:
        if (!init_OB1_2()) return FALSE;
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
    mCyl.Set(l_cyl_src);
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return TRUE;
}
VERIFY(0x022A6E10, &daNpc_Ob1_c::createInit);

/* 022A7004 */
cPhs_State daNpc_Ob1_c::_create() {
    WWHD_FUNC(0x022A7004, cPhs_State, this);
    /* fopAcM_SetupActor(this, daNpc_Ob1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = OB1_VTBL;
            gabi::call(0x025E7820, &mBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut2); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, STR(0x1001F1E4) /* "Ob" */);
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap_addr, gabi::load<u32>(0x101C2610) /* a_heap_size_tbl[0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -60.0f, 60.0f, 170.0f, 60.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}
VERIFY(0x022A7004, &daNpc_Ob1_c::_create);

/* 022A7140 */
static cPhs_State daNpc_Ob1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022A7140, cPhs_State, i_this);
    return ((daNpc_Ob1_c*)i_this)->_create();
}
VERIFY(0x022A7140, daNpc_Ob1_Create);

/* 022A7144 */
BOOL daNpc_Ob1_c::_delete() {
    WWHD_FUNC(0x022A7144, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001F1E7) /* "Ob" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) { /* HD: only when the heap was created */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022A7144, &daNpc_Ob1_c::_delete);

/* 022A719C */
static BOOL daNpc_Ob1_Delete(daNpc_Ob1_c* i_this) {
    WWHD_FUNC(0x022A719C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022A719C, daNpc_Ob1_Delete);

/* 022A71A0 */
void daNpc_Ob1_c::partner_srch() {
    WWHD_FUNC(0x022A71A0, void, this);
}
VERIFY(0x022A71A0, &daNpc_Ob1_c::partner_srch);

/* 022A71A4 */
void daNpc_Ob1_c::checkOrder() {
    WWHD_FUNC(0x022A71A4, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        if (dComIfGp_evmng_startCheck(eventIdx(mEvtNo))) {
            mOrderType = 0;
        }
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrderType == 1 || mOrderType == 2) {
            mOrderType = 0;
            mbTalk = 1;
        }
    }
}
VERIFY(0x022A71A4, &daNpc_Ob1_c::checkOrder);

/* 022A7260 */
u8 daNpc_Ob1_c::demo() {
    WWHD_FUNC(0x022A7260, u8, this);
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
        if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail(STR(0x1001F0E4), 0x23A, STR(0x1001F0C8));
        demoAc = dDemo_object_getActor(gabi::load<u32>(0x101D5FFC), id);
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn)) {
            mBtpFrame = (u8)J3DAnmTexPattern_getFrameMax(m_hed_tex_pttrn);
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(demoAc, STR(0x1001F1EA) /* "Ob" */);
        if (btp != nullptr) {
            m_hed_tex_pttrn = btp;
            if (mDoExt_btpAnm_init(&mBtpAnm, J3DModel_modelData(mpHedModel), btp, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBtpFrame = 0;
                mTexNo = 2;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x1001F1EA) /* "Ob" */, 0, 0, 0, 0);
    return mbDemo;
}
VERIFY(0x022A7260, &daNpc_Ob1_c::demo);

/* 022A7410 */
s32 daNpc_Ob1_c::isEventEntry() {
    WWHD_FUNC(0x022A7410, s32, this);
    const char* name = gabi::at<const char>(mEventCut2.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022A7410, &daNpc_Ob1_c::isEventEntry);

/* 022A7450 */
void daNpc_Ob1_c::endEvent() {
    WWHD_FUNC(0x022A7450, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}
VERIFY(0x022A7450, &daNpc_Ob1_c::endEvent);

/* 022A7490 */
void daNpc_Ob1_c::event_actionInit(int staffIdx) {
    WWHD_FUNC(0x022A7490, void, this, staffIdx);
    void* p = dComIfGp_evmng_getMySubstanceP(staffIdx, STR(0x1001F1F0) /* "ActNo" */, 3 /* integer */);
    if (p != nullptr) {
        mEvtActNo = (s8)gabi::load<s32>(gabi::ea(p));
    }
}
VERIFY(0x022A7490, &daNpc_Ob1_c::event_actionInit);

/* 022A74F0 */
BOOL daNpc_Ob1_c::event_action() {
    WWHD_FUNC(0x022A74F0, BOOL, this);
    return TRUE;
}
VERIFY(0x022A74F0, &daNpc_Ob1_c::event_action);

/* 022A74F8 */
void daNpc_Ob1_c::privateCut(int staffIdx) {
    WWHD_FUNC(0x022A74F8, void, this, staffIdx);
    if (staffIdx == -1) {
        return;
    }
    /* static char* cut_name_tbl[] = {"ACTION"} (101C2614) */
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, 0x101C2614, 1, TRUE, 0);
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
VERIFY(0x022A74F8, &daNpc_Ob1_c::privateCut);

/* 022A75CC */
void daNpc_Ob1_c::lookBack() {
    WWHD_FUNC(0x022A75CC, void, this);
    gabi::Local<cXyz> dst;
    mSaveBboneY = m_jnt.mAngles[1][1];
    s16 targetY = current.angle.y;
    mSaveHeadY = m_jnt.mAngles[0][1];
    dst->set(0.0f, 0.0f, 0.0f);
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
        gabi::Local<cXyz> tmp;
        dNpc_playerEyePos_l(tmp, -20.0f);
        dst->copy(*tmp);
        eyeZ = current.pos.z;
        eyeX = current.pos.x;
        eyeY = eyePos.y;
        dstPos = dst;
        break;
    }
    case 2:
        dst->copy(mLookPos);
        eyeZ = current.pos.z;
        eyeX = current.pos.x;
        dstPos = dst;
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
    gabi::Local<cXyz> eye;
    eye->x = eyeX;
    eye->y = eyeY;
    eye->z = eyeZ;
    lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, targetY, maxVel, headOnly);
}
VERIFY(0x022A75CC, &daNpc_Ob1_c::lookBack);

/* 022A780C */
void daNpc_Ob1_c::event_proc(int staffIdx) {
    WWHD_FUNC(0x022A780C, void, this, staffIdx);
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
VERIFY(0x022A780C, &daNpc_Ob1_c::event_proc);

/* 022A78C0 */
void daNpc_Ob1_c::ob_clcMovSpd() {
    WWHD_FUNC(0x022A78C0, void, this);
    gabi::Local<cXyz> d;
    cXyz_mi(&mTargetPos, d, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    PSVECSquareMag(xz); /* the distance is not used */
    s16 ang = cLib_targetAngleY(&current.pos, &mTargetPos);
    cLib_chaseAngleS(&current.angle.y, ang, l_HIO.mWalkTurnStep);
    cLib_chaseF(&speedF, mSpdTarget, mSpdStep);
}
VERIFY(0x022A78C0, &daNpc_Ob1_c::ob_clcMovSpd);

/* 022A794C */
s32 daNpc_Ob1_c::ob_movPass() {
    WWHD_FUNC(0x022A794C, s32, this);
    s32 ret = 0;
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (gabi::load<u8>(gabi::ea(path) + 5) & 1) /* closed loop */) {
        gabi::Local<cXyz> pos;
        pos->x = (f32)current.pos.x;
        pos->y = (f32)current.pos.y;
        pos->z = (f32)current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
            dNpc_PathRun_nextIdxAuto(&mPathRun);
            return 1;
        }
    } else {
        gabi::Local<cXyz> d;
        cXyz_mi(&mTargetPos, d, &current.pos);
        gabi::Local<cXyz> xz;
        xz->x = (f32)d->x;
        xz->y = 0.0f;
        xz->z = (f32)d->z;
        f32 dist = std_sqrtf(PSVECSquareMag(xz));
        if (!(dist > mPassDist)) {
            ret = 1;
            if (mPathRun.mPath.get() != nullptr) {
                if (!dNpc_PathRun_nextIdxAuto(&mPathRun)) {
                    ret = 2;
                }
            }
        }
    }
    return ret;
}
VERIFY(0x022A794C, &daNpc_Ob1_c::ob_movPass);

/* 022A7A4C */
void daNpc_Ob1_c::ob_nMove() {
    WWHD_FUNC(0x022A7A4C, void, this);
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
            m960 = 1;
            m962 = 1;
            return;
        }
        if ((u32)pass == 2) {
            m962 = 1;
            mMoveMode = 0;
        }
    }
    if (m962 != 0) {
        m960 = 1;
    }
}
VERIFY(0x022A7A4C, &daNpc_Ob1_c::ob_nMove);

/* 022A7B44 */
void daNpc_Ob1_c::eventOrder() {
    WWHD_FUNC(0x022A7B44, void, this);
    s8 type = mOrderType;
    if (type == 1 || type == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderType == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (type >= 3) {
        s16 evtNo = (s16)(type - 3);
        mEvtNo = evtNo;
        fopAcM_orderOtherEventId(this, eventIdx(evtNo), 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x022A7B44, &daNpc_Ob1_c::eventOrder);

/* 022A7BB4 */
BOOL daNpc_Ob1_c::_execute() {
    WWHD_FUNC(0x022A7BB4, BOOL, this);
    if (mbInit == 0) {
        mHomeAngle.y = current.angle.y;
        gabi::store<u32>(gabi::ea(&mHomePos.z), gabi::load<u32>(gabi::ea(&current.pos.z)));
        gabi::store<u32>(gabi::ea(&mHomePos.x), gabi::load<u32>(gabi::ea(&current.pos.x)));
        mHomeAngle.x = current.angle.x;
        mHomeAngle.z = current.angle.z;
        gabi::store<u32>(gabi::ea(&mHomePos.y), gabi::load<u32>(gabi::ea(&current.pos.y)));
        mbInit = 1;
    }
    daNpc_Ob1_HIO_c& hio = l_HIO;
    m_jnt.setParam(hio.mPrm[4], hio.mPrm[5], hio.mPrm[6], hio.mPrm[7], hio.mPrm[0], hio.mPrm[1], hio.mPrm[2], hio.mPrm[3],
                   hio.mPrm[8]);
    if (m95E != 0 && demoActorID == 0) {
        return TRUE;
    }
    m960 = 0;
    m95E = 0;
    partner_srch();
    checkOrder();
    if (demo() == 0) {
        bool moved = false;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !checkCommandTalk */) {
            s32 staffIdx = isEventEntry();
            if (staffIdx >= 0) {
                event_proc(staffIdx);
                moved = true;
            }
        }
        if (!moved) {
            ptmf_call1(&mAction, this, nullptr);
        }
        if (m960 == 0) {
            ob_nMove();
            fopAcM_posMoveF(this, &mStts.m_cc_move);
        }
        if (m95F == 0) {
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
VERIFY(0x022A7BB4, &daNpc_Ob1_c::_execute);

/* 022A7DB8 */
static BOOL daNpc_Ob1_Execute(daNpc_Ob1_c* i_this) {
    WWHD_FUNC(0x022A7DB8, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022A7DB8, daNpc_Ob1_Execute);

/* 022A7DBC */
BOOL daNpc_Ob1_c::_draw() {
    WWHD_FUNC(0x022A7DBC, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel* hedModel = mpHedModel;
    u32 hedData = J3DModel_modelData(hedModel);
    if (m95E != 0 || m961 != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: unused */
    mpMorf->entryDL();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&mBtpAnm, gabi::at<J3DModelData>(hedData), mBtpFrame);
    mDoExt_modelEntryDL(hedModel);
    gabi::store<u32>(hedData + 0x38, 0); /* mBtpAnm.remove(hedData) */
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    dSnap_RegistFig(0x4F /* DSNAP_TYPE_NPC_OB1 */, this, 1.0f, 1.0f, 1.0f);
    if (l_HIO.mDebugDraw != 0) {
        /* debug colours (GXColor function-local statics, unused) */
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_l(0x101FEBF4, 0x1001F070, 4);
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_l(0x101FEBF8, 0x1001F074, 4);
        }
        if (gabi::load<u32>(0x101FDA48) == 0) {
            gabi::store<u32>(0x101FDA48, 1);
            memcpy_l(0x101FEBEC, 0x1001F078, 4);
        }
    }
    return TRUE;
}
VERIFY(0x022A7DBC, &daNpc_Ob1_c::_draw);

/* 022A7F44 */
static BOOL daNpc_Ob1_Draw(daNpc_Ob1_c* i_this) {
    WWHD_FUNC(0x022A7F44, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022A7F44, daNpc_Ob1_Draw);

/* 022A7F48 */
static BOOL daNpc_Ob1_IsDelete(daNpc_Ob1_c*) {
    WWHD_FUNC(0x022A7F48, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x022A7F48, daNpc_Ob1_IsDelete);

/* 022A7F50 */
s32 daNpc_Ob1_c::anmNum_toResID(int num) {
    WWHD_FUNC(0x022A7F50, s32, this, num);
    return gabi::load<s32>(0x1001F208 + 4 * num); /* a_anm_resID_tbl */
}
VERIFY(0x022A7F50, &daNpc_Ob1_c::anmNum_toResID);

/* 022A7F64 */
u32 daNpc_Ob1_c::setAnm_tex(s8 tex) {
    WWHD_FUNC(0x022A7F64, u32, this, tex);
    if (mTexNo != tex) {
        mTexNo = tex;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022A7F64, &daNpc_Ob1_c::setAnm_tex);

/* 022A7F84 */
BOOL daNpc_Ob1_c::setAnm_anm(anm_prm_c* prm) {
    WWHD_FUNC(0x022A7F84, BOOL, this, prm);
    s8 anmNo = prm->mAnmNo;
    if (mAnmNo != anmNo) {
        mAnmNo = anmNo;
        s32 resID = anmNum_toResID(anmNo);
        s32 loop = prm->mLoopMode;
        f32 speed = prm->mSpeed;
        f32 morf = prm->mMorf;
        dNpc_setAnmIDRes(mpMorf, loop, morf, speed, resID, -1, STR(0x1001F228) /* "Ob" */);
        mLoopCnt = 0;
        mPrevFrame = 0.0f;
        mAnmEnd = 0;
    }
    return TRUE;
}
VERIFY(0x022A7F84, &daNpc_Ob1_c::setAnm_anm);

static inline daNpc_Ob1_c::anm_prm_c* anm_prm(u32 tbl, s32 i) { return gabi::at<daNpc_Ob1_c::anm_prm_c>(tbl + 0x14 * i); }

/* 022A8018 */
void daNpc_Ob1_c::setAnm_NUM(int num, int withTex) {
    WWHD_FUNC(0x022A8018, void, this, num, withTex);
    /* a_anm_prm_tbl (101C2618) */
    if (withTex) {
        setAnm_tex(anm_prm(0x101C2618, num)->mTexNo);
    }
    setAnm_anm(anm_prm(0x101C2618, num));
}
VERIFY(0x022A8018, &daNpc_Ob1_c::setAnm_NUM);

/* 022A8084 */
BOOL daNpc_Ob1_c::setAnm() {
    WWHD_FUNC(0x022A8084, BOOL, this);
    /* a_anm_prm_tbl (101C26B8), by state */
    if (anm_prm(0x101C26B8, mStt)->mTexNo >= 0) {
        setAnm_tex(anm_prm(0x101C26B8, mStt)->mTexNo);
    }
    if (anm_prm(0x101C26B8, mStt)->mAnmNo >= 0) {
        setAnm_anm(anm_prm(0x101C26B8, mStt));
    }
    return TRUE;
}
VERIFY(0x022A8084, &daNpc_Ob1_c::setAnm);

/* 022A8108 */
void daNpc_Ob1_c::setAnm_ATR(int withTex) {
    WWHD_FUNC(0x022A8108, void, this, withTex);
    /* a_anm_prm_tbl (101C2744), by message attribute */
    if (withTex) {
        setAnm_tex(anm_prm(0x101C2744, mAnmAtr)->mTexNo);
    }
    setAnm_anm(anm_prm(0x101C2744, mAnmAtr));
}
VERIFY(0x022A8108, &daNpc_Ob1_c::setAnm_ATR);

/* 022A8178 */
void daNpc_Ob1_c::chg_anmAtr(u8 atr) {
    WWHD_FUNC(0x022A8178, void, this, atr);
    u32 msgNo = mMsgNo;
    if (msgNo != 0xFFFFFFFF) {
        switch (msgNo) {
        case 0xAFC: case 0xAFD: case 0xAFF: case 0xB02: case 0xB05: case 0xB06: case 0xB09: case 0xB0C: case 0xB0D:
        case 0xB5D: case 0xB61: case 0xB64: case 0xB65:
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
VERIFY(0x022A8178, &daNpc_Ob1_c::chg_anmAtr);

/* 022A8244 */
void daNpc_Ob1_c::control_anmAtr() {
    WWHD_FUNC(0x022A8244, void, this);
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
VERIFY(0x022A8244, &daNpc_Ob1_c::control_anmAtr);

/* 022A82E0 */
void daNpc_Ob1_c::anmAtr(u16 msgStatus) {
    WWHD_FUNC(0x022A82E0, void, this, msgStatus);
    u32 play = dComIfGp_ea();
    if (mManzaiStt == 2 && gabi::ea(this) != dComIfGp_talkActor(play)) {
        control_anmAtr();
        return;
    }
    if (msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (mAtrCnt == 0) {
            mAnmAtr = 0xFF;
            chg_anmAtr(gabi::load<u8>(dComIfGp_ea() + 0x5BC5)); /* dComIfGp_getMesgAnimeAttrInfo */
            mAtrCnt = (s8)(mAtrCnt + 1);
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);    /* HD: cleared unconditionally */
        if (tag != 0xFF && tag != mAnmTag) {
            mAnmTag = tag;
            /* chg_anmTag(): empty */
        }
    } else if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        mAtrCnt = 0;
    }
    control_anmAtr();
    /* control_anmTag(): empty */
}
VERIFY(0x022A82E0, &daNpc_Ob1_c::anmAtr);

/* 022A83D8 */
BOOL daNpc_Ob1_c::chk_talk() {
    WWHD_FUNC(0x022A83D8, BOOL, this);
    BOOL ret = FALSE;
    u8 talkXY = gabi::load<u8>(dComIfGp_ea() + 0x52B0);
    if ((u32)(talkXY - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mPreItemNo = gabi::load<u8>(dComIfGp_ea() + 0x52B1); /* dComIfGp_event_getPreItemNo() */
            ret = TRUE;
        }
    } else {
        mPreItemNo = 0xFF;
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x022A83D8, &daNpc_Ob1_c::chk_talk);

/* 022A8458 */
u8 daNpc_Ob1_c::chk_partsNotMove() {
    WWHD_FUNC(0x022A8458, u8, this);
    return mSaveHeadY == m_jnt.mAngles[0][1] && mSaveBboneY == m_jnt.mAngles[1][1] && mSaveAngleY == current.angle.y;
}
VERIFY(0x022A8458, &daNpc_Ob1_c::chk_partsNotMove);

/* 022A8498 */
u16 daNpc_Ob1_c::next_msgStatus(u32* pMsgNo) {
    WWHD_FUNC(0x022A8498, u16, this, pMsgNo);
    be<u32>* msgNo = (be<u32>*)pMsgNo;
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
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
    case 0xA92: case 0xA96: case 0xA9B: case 0xA9D: case 0xAA0:
    case 0xAA2: case 0xAA3: case 0xAA4: case 0xAA5: case 0xAA6:
        if (dComIfGs_isEventBit(0xE20) && !dComIfGs_isEventBit(0x308)) {
            *msgNo = 0xAA7;
        } else {
            ret = 0x10;
        }
        break;
    case 0xAA8:
        *msgNo = dKy_daynight_check() ? 0xAAA : 0xAA9;
        break;
    case 0xAAD: case 0xAAE:
        *msgNo = dComIfGs_isEventBit(0x304) ? 0xA8F : 0xA8D;
        break;
    default:
        ret = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return ret;
}
VERIFY(0x022A8498, &daNpc_Ob1_c::next_msgStatus);

/* 022A8650 */
u32 daNpc_Ob1_c::getMsg_OB1_0() {
    WWHD_FUNC(0x022A8650, u32, this);
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
                case 1: msgNo = 0xAA4; break;
                case 2: msgNo = 0xAA5; break;
                default: msgNo = 0xAA6; break;
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
        m95C = 1;
    }
    return msgNo;
}
VERIFY(0x022A8650, &daNpc_Ob1_c::getMsg_OB1_0);

/* 022A89B4 */
u32 daNpc_Ob1_c::getMsg_OB1_1() {
    WWHD_FUNC(0x022A89B4, u32, this);
    if (!dComIfGs_isEventBit(0x2C20)) {
        return 0xAA8;
    }
    return dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS) == 0 ? 0xAAC : 0xAAB;
}
VERIFY(0x022A89B4, &daNpc_Ob1_c::getMsg_OB1_1);

/* 022A8A30 */
u32 daNpc_Ob1_c::getMsg_OB1_2() {
    WWHD_FUNC(0x022A8A30, u32, this);
    return getMsg_OB1_1();
}
VERIFY(0x022A8A30, &daNpc_Ob1_c::getMsg_OB1_2);

/* 022A8A34 */
u32 daNpc_Ob1_c::getMsg() {
    WWHD_FUNC(0x022A8A34, u32, this);
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
VERIFY(0x022A8A34, &daNpc_Ob1_c::getMsg);

/* 022A8A8C */
BOOL daNpc_Ob1_c::chkAttention() {
    WWHD_FUNC(0x022A8A8C, BOOL, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    } else {
        return this == dAttention_ActionTarget(attention, 0);
    }
}
VERIFY(0x022A8A8C, &daNpc_Ob1_c::chkAttention);

/* 022A8B14 (matcher: cLib_calcTimer<s> in d_a_npc_ac1.cpp) */
fopAc_ac_c* daNpc_Ob1_c::searchByID(fpc_ProcID id) {
    WWHD_FUNC(0x022A8B14, fopAc_ac_c*, this, id);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    fopAcM_SearchByID2(id, actor);
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x022A8B14, &daNpc_Ob1_c::searchByID);

/* 022A8B48 */
s8 daNpc_Ob1_c::bitCount(u8 bits) {
    WWHD_FUNC(0x022A8B48, s8, this, bits);
    s8 n = 0;
    for (int i = 0; i < 8; i++) {
        if (bits & 1) {
            n++;
        }
        bits >>= 1;
    }
    return n;
}
VERIFY(0x022A8B48, &daNpc_Ob1_c::bitCount);

/* 022A8B70 */
void daNpc_Ob1_c::set_pigCnt() {
    WWHD_FUNC(0x022A8B70, void, this);
    mPigCntGot = bitCount(dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS));
    l_pig_cnt() = 0;
    l_pig_bits() = 0;
    for (int i = 0; i < 20; i++) {
        gabi::store<u32>(l_pig_tbl(i), 0);
    }
    fpcM_Search(searchActor_Kb_addr, this);
    mPigCntHere = bitCount(l_pig_bits());
    u8 reg = dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS);
    mPigCntNew = bitCount((u8)(l_pig_bits() & ~reg));
    mTalkStep = 0;
}
VERIFY(0x022A8B70, &daNpc_Ob1_c::set_pigCnt);

/* 022A8C40 */
void daNpc_Ob1_c::ob_setPthPos() {
    WWHD_FUNC(0x022A8C40, void, this);
    if (mPathRun.mPath.get() != nullptr) {
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        current.pos.copy(*pnt);
        dNpc_PathRun_nextIdxAuto(&mPathRun);
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        gabi::Local<cXyz> next;
        next->copy(*pnt);
        current.angle.y = cLib_targetAngleY(&current.pos, next);
    }
}
VERIFY(0x022A8C40, &daNpc_Ob1_c::ob_setPthPos);

/* 022A8CE8: returns a cXyz (hidden result pointer) */
void daNpc_Ob1_c::get_attPos(cXyz* out) {
    WWHD_FUNC(0x022A8CE8, void, this, out);
    gabi::Local<cXyz> pos;
    dPath* path = dNpc_PathRun_nextPath(&mPathRun, current.roomNo);
    if (path != nullptr) {
        u8 idx = mPathRun.mIdx;
        if (idx == 0) {
            idx = dNpc_PathRun_maxPoint(&mPathRun);
        }
        u32 pnt = gabi::load<u32>(gabi::ea(path) + 8) + (u8)(idx - 1) * 0x10;
        pos->x = gabi::load<f32>(pnt + 4);
        pos->y = gabi::load<f32>(pnt + 8);
        pos->z = gabi::load<f32>(pnt + 0xC);
    }
    if (out == nullptr) { /* GHS: the result's constructor allocates when NULL */
        out = (cXyz*)operator_new(0xC);
        if (out == nullptr) return;
    }
    out->x = (f32)pos->x;
    out->y = (f32)pos->y;
    out->z = (f32)pos->z;
}
VERIFY(0x022A8CE8, &daNpc_Ob1_c::get_attPos);

/* 022A8DA8 */
void daNpc_Ob1_c::clrSpd() {
    WWHD_FUNC(0x022A8DA8, void, this);
    mSpdStep = 0.0f;
    gravity = -4.5f;
    mSpdTarget = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022A8DA8, &daNpc_Ob1_c::clrSpd);

/* 022A8DCC */
void daNpc_Ob1_c::setStt(s8 stt) {
    WWHD_FUNC(0x022A8DCC, void, this, stt);
    searchByID(field_0x8e4); /* result unused */
    s8 prev = mStt;
    mStt = stt;
    mTimer = 0;
    switch (stt) {
    case 4:
        mWaitTimer = (s16)cLib_getRndValue(90, 180);
        /* fallthrough */
    case 1:
    case 5:
        if (prev != 2) {
            switch (mStt) {
            case 1:
                mLookMode = 3;
                mTargetAngY = mHomeAngle.y;
                mbHeadOnly = 0;
                m_jnt.mbTrn = 1;
                break;
            case 4: {
                mLookMode = 2;
                gabi::Local<cXyz> att;
                get_attPos(att);
                mLookPos.copy(*att);
                mbHeadOnly = 0;
                m_jnt.mbTrn = 1;
                break;
            }
            case 5:
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
    case 2:
        mOrderType = 0;
        mbHeadOnly = 0;
        mLookMode = 1;
        m_jnt.mbTrn = 1;
        mMoveMode = 0;
        clrSpd();
        mAnmAtr = 0xFF;
        if (prev != 5) {
            mPrevStt = prev;
        }
        set_pigCnt();
        setAnm();
        break;
    case 3: {
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        m962 = 0;
        mOrderType = 0;
        mTargetPos.copy(*pnt);
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
    case 6:
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        break;
    default:
        setAnm();
        break;
    }
}
VERIFY(0x022A8DCC, &daNpc_Ob1_c::setStt);

/* 022A9024 */
BOOL daNpc_Ob1_c::wait_1() {
    WWHD_FUNC(0x022A9024, BOOL, this);
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    if (mManzaiStt == 1) {
        mManzaiStt = 2;
        setStt(6);
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
VERIFY(0x022A9024, &daNpc_Ob1_c::wait_1);

/* 022A9108 */
BOOL daNpc_Ob1_c::wait_2() {
    WWHD_FUNC(0x022A9108, BOOL, this);
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
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
        setStt(3);
        return TRUE;
    }
    mLookMode = 2;
    gabi::Local<cXyz> att;
    get_attPos(att);
    mLookPos.copy(*att);
    m_jnt.mbTrn = 1;
    return TRUE;
}
VERIFY(0x022A9108, &daNpc_Ob1_c::wait_2);

/* 022A91F0 */
BOOL daNpc_Ob1_c::wait_3() {
    WWHD_FUNC(0x022A91F0, BOOL, this);
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
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
VERIFY(0x022A91F0, &daNpc_Ob1_c::wait_3);

/* 022A929C */
BOOL daNpc_Ob1_c::walk_1() {
    WWHD_FUNC(0x022A929C, BOOL, this);
    if (mbTalk != 0) {
        setAnm_NUM(0, 1);
        speedF = 0.0f;
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    mOrderType = 2;
    if (mbAttention != 0) {
        mTimer = 20;
    }
    mLookMode = cLib_calcTimer(&mTimer) != 0;
    if (m962 != 0) {
        if (mPathRun.mIdx == 0) {
            dNpc_PathRun_nextIdxAuto(&mPathRun);
        }
        setStt(4);
        return TRUE;
    }
    m962 = 0;
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    mTargetPos.copy(*pnt);
    return TRUE;
}
VERIFY(0x022A929C, &daNpc_Ob1_c::walk_1);

/* 022A93A8 */
BOOL daNpc_Ob1_c::talk_1() {
    WWHD_FUNC(0x022A93A8, BOOL, this);
    u32 mng = l_msgMng();
    BOOL notMove = chk_partsNotMove();
    s8 order = mOrderType;
    if (order == 1 || order >= 3) {
        return TRUE;
    }
    u16 status = 0;
    if (mbCurrMsg != 0) {
        status = (u16)fopMsgM_SearchByID(mng);
    }
    mMsgStatus = status;
    if (m95D != 0 && mCurrMsgBsPcId == 0xFFFFFFFF) {
        mCurrMsgNo = vcall_getMsg(this);
        u32 play = dComIfGp_ea();
        cXyz* pos = gabi::at<cXyz>(gabi::load<u32>(play + 0x5C38) + 0x37C); /* the talk partner's eyePos */
        mCurrMsgBsPcId = fopMsgM_messageSet(mng, mCurrMsgNo, pos);
        mbCurrMsg = 0;
        return notMove;
    }
    mMsgNo = 0xFFFFFFFF;
    talk(1);
    if (mbCurrMsg != 0 && fopMsgM_SearchByID(mng) == 0x13 /* fopMsgStts_BOX_CLOSED_e */) {
        if (mTalkStep != 0) {
            mOrderType = 3;
            setAnm_NUM(0, 1);
            endEvent();
            return notMove;
        }
        u8 reg = dComIfGs_getEventReg(dSv_evtReg_OB1_PIGS);
        u8 bits = l_pig_bits() | reg;
        l_pig_bits() = bits;
        dComIfGs_setEventReg(dSv_evtReg_OB1_PIGS, bits);
        u32 msgNo = mCurrMsgNo;
        if (msgNo == 0xA8E) {
            dComIfGs_onEventBit(0x304);
        } else if (msgNo == 0xAA7) {
            dComIfGs_onEventBit(0x308);
        } else if (msgNo >= 0xAA9 && msgNo <= 0xAAC) {
            dComIfGs_onEventBit(0x2C20);
        }
        if (m95C != 0) {
            dComIfGs_onEventBit(0x302);
        }
        mPreItemNo = 0xFF;
        mbTalk = 0;
        if (mType == 0) {
            setStt(5);
            endEvent();
        } else {
            setStt(mPrevStt);
            mTimer = 60;
            endEvent();
        }
    }
    return notMove;
}
VERIFY(0x022A93A8, &daNpc_Ob1_c::talk_1);

/* 022A9610 */
BOOL daNpc_Ob1_c::manzai() {
    WWHD_FUNC(0x022A9610, BOOL, this);
    u32 play = dComIfGp_ea();
    switch (mManzaiStt) {
    case 2: {
        fopNpc_npc_c_l* partner = (fopNpc_npc_c_l*)searchByID(mPartnerId);
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
            mStt = 6;
            mAnmAtr = 0xFF;
            return TRUE;
        }
        if (partner != nullptr) {
            mMsgNo = partner->mCurrMsgNo;
            vcall_anmAtr(this, partner->mPartnerMsgStts);
        }
        return TRUE;
    }
    case 3:
        actor_status &= ~0x4000u;
        setStt(mPrevStt);
        mManzaiStt = 0;
        return TRUE;
    }
    return TRUE;
}
VERIFY(0x022A9610, &daNpc_Ob1_c::manzai);

/* 022A9730 */
BOOL daNpc_Ob1_c::wait_action1(void*) {
    WWHD_FUNC(0x022A9730, BOOL, this, (u32)0);
    if (mActStep == 0) {
        ob_setPthPos();
        setStt(3);
        mActStep = (s8)(mActStep + 1);
        return TRUE;
    }
    if ((u32)(s32)mActStep > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch (mStt) {
    case 2:
        mActRet = talk_1();
        break;
    case 3:
        mActRet = walk_1();
        break;
    case 4:
        mActRet = wait_2();
        break;
    case 5:
        mActRet = wait_3();
        break;
    }
    lookBack();
    return TRUE;
}
VERIFY(0x022A9730, &daNpc_Ob1_c::wait_action1);

/* 022A9824 */
BOOL daNpc_Ob1_c::wait_action2(void*) {
    WWHD_FUNC(0x022A9824, BOOL, this, (u32)0);
    if (mActStep == 0) {
        setStt(1);
        mActStep = (s8)(mActStep + 1);
        return TRUE;
    }
    if ((u32)(s32)mActStep > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch (mStt) {
    case 1:
        mActRet = wait_1();
        break;
    case 2:
        mActRet = talk_1();
        break;
    case 6:
        mActRet = manzai();
        break;
    }
    lookBack();
    return TRUE;
}
VERIFY(0x022A9824, &daNpc_Ob1_c::wait_action2);

/* 022A98F8: daNpc_Ob1_HIO_c::daNpc_Ob1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ob1_HIO_c* daNpc_Ob1_HIO_ct(daNpc_Ob1_HIO_c* p) {
    WWHD_FUNC(0x022A98F8, daNpc_Ob1_HIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Ob1_HIO_c*)operator_new(0x3C);
        if (p == nullptr) return nullptr;
    }
    p->__vtbl = HIO_VTBL;
    memcpy_l(gabi::ea(p) + 0xC, 0x101C27D0 /* a_prm_tbl */, 0x30);
    p->mNo = -1;
    p->field_0x8 = -1;
    return p;
}
VERIFY(0x022A98F8, daNpc_Ob1_HIO_ct);

/* 022A9964 */
static void __sinit_d_a_npc_ob1_cpp() {
    WWHD_FUNC(0x022A9964, void, (u32)0);
    /* header statics (this TU: the objects at P+9 / P+10, P = 10467D34) */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10467D40 + 4 * i, 0);
    __register_global_object(0x101C2800);
    gabi::store<f32>(0x10467D34, -3.1415927f);
    gabi::store<f32>(0x10467D38, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10467D3Du);
    __register_global_object(0x101C280C);
    gabi::call(0x028EAB2C, 0x10467D3Eu);
    __register_global_object(0x101C2818);
    daNpc_Ob1_HIO_ct(&l_HIO); /* static daNpc_Ob1_HIO_c l_HIO */
}
VERIFY(0x022A9964, __sinit_d_a_npc_ob1_cpp);

/* 022A9A04: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022A9A04, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x022A9A04, SafeString_dt);

/* 022A9A18: daNpc_Ob1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ob1_dt(daNpc_Ob1_c* p, s32 flags) {
    WWHD_FUNC(0x022A9A18, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x1001F098);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x1001F0A8);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x022A9A18, daNpc_Ob1_dt);

/* 022A9AB4: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022A9AB4, void, (u32)0);
}
VERIFY(0x022A9AB4, SafeString_assureTerminationImpl);
