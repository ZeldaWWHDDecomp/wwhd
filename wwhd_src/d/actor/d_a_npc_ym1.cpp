/**
 * d_a_npc_ym1.cpp (WWHD)
 * NPC - Mesa & Abe (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ym1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part A: area_check, joint callbacks, heap, decideType, set_action, init_*, animation, setMtx.
 * (022F6764..022F68A8 before area_check belong to d_a_npc_uk: its __sinit and destructors.)
 */
#define SAFESTRING_VTBL 0x10023448 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ym1.h"

enum { fpcNm_NPC_YM1_e = 0x13B, fpcNm_NPC_YM2_e = 0x13C };
enum { dSv_event_flag_UNK_0520 = 0x0520, dSv_event_flag_UNK_0E20 = 0x0E20 };

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 {
    PMF_wait_action1 = 0x10023410,
    PMF_wait_action4 = 0x10023418,
    PMF_wait_action3 = 0x10023420,
    PMF_wait_action2 = 0x10023428,
    PMF_demo_action1 = 0x10023430,
};

/* 022F68AC */
void daNpc_Ym1_c::setKariFlg() {
    WWHD_FUNC(0x022F68AC, void, this);
    mKariFlag = 1;
}
VERIFY(0x022F68AC, &daNpc_Ym1_c::setKariFlg);

/* 022F68B8 */
/* dCcMassS_Mng::SetAreaChk callback (_execute): the grass-cutting area hit this NPC */
static void area_check(fopAc_ac_c* i_actor, cXyz*, u32 i_flag) {
    WWHD_FUNC(0x022F68B8, void, i_actor, (cXyz*)nullptr, i_flag);
    if (i_flag == 0) {
        static_cast<daNpc_Ym1_c*>(i_actor)->setKariFlg();
    }
}
VERIFY(0x022F68B8, area_check);

/* 022F68C4 */
void daNpc_Ym1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022F68C4, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(26.0f, -25.0f, 0.0f): guard 0x104689DC, object 0x1046896C */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x1046896C);
    if (gabi::load<u32>(0x104689DC) == 0) {
        gabi::store<u32>(0x104689DC, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 26.0f;
        a_eye_pos_off->y = -25.0f;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    m9E0 = stk->m[0][3];
    m9E4 = stk->m[1][3];
    m9E8 = stk->m[2][3];
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[0][0]);
    PSMTXMultVec(stk, a_eye_pos_off, &m9BC);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x022F68C4, &daNpc_Ym1_c::_nodeCB_Head);

/* 022F6A34 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_timing) {
    WWHD_FUNC(0x022F6A34, BOOL, i_node, i_timing);
    if (i_timing == 0) {
        daNpc_Ym1_c* user = gabi::at<daNpc_Ym1_c>(gabi::load<u32>(gabi::load<u32>(0x104B462C) + 0xB8));
        if (user != nullptr) {
            user->_nodeCB_Head(i_node, j3dSys_mModel());
        }
    }
    return TRUE;
}
VERIFY(0x022F6A34, nodeCB_Head);

/* 022F6A7C */
void daNpc_Ym1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022F6A7C, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x022F6A7C, &daNpc_Ym1_c::_nodeCB_BackBone);

/* 022F6B98 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_timing) {
    WWHD_FUNC(0x022F6B98, BOOL, i_node, i_timing);
    if (i_timing == 0) {
        daNpc_Ym1_c* user = gabi::at<daNpc_Ym1_c>(gabi::load<u32>(gabi::load<u32>(0x104B462C) + 0xB8));
        if (user != nullptr) {
            user->_nodeCB_BackBone(i_node, j3dSys_mModel());
        }
    }
    return TRUE;
}
VERIFY(0x022F6B98, nodeCB_BackBone);

/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */
static inline void setJointCallBack(J3DModel* model, s8 jnt, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(model);
    u32 idx = (u16)(s16)jnt;
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (idx < n)
        joint += idx * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 022F6BE0 */
bool daNpc_Ym1_c::bodyCreateHeap() {
    WWHD_FUNC(0x022F6BE0, bool, this);
    /* a_bdl_resID_tbl (.data 0x101C6A14), by mSubType */
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, gabi::load<s32>(0x101C6A14 + mSubType * 4));
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2471, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x100234F4), 0x9A7, STR(0x10023504));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x15021222);
    if (mpMorf.get() == nullptr) {
        return false;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return false;
    }
    mHeadJointIdx = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100234EC) /* "head" */);
    if (mHeadJointIdx < 0)
        JUT_ASSERT_fail(STR(0x100234F4), 0x9B6, STR(0x10023518));
    mBboneJointIdx = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1002352C) /* "backbone" */);
    if (mBboneJointIdx < 0)
        JUT_ASSERT_fail(STR(0x100234F4), 0x9B8, STR(0x10023538));
    mHandLJointIndex = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100234DC) /* "handL" */);
    if (mHandLJointIndex < 0)
        JUT_ASSERT_fail(STR(0x100234F4), 0x9BA, STR(0x10023550));
    mHandRJointIndex = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100234E4) /* "handR" */);
    if (mHandRJointIndex < 0)
        JUT_ASSERT_fail(STR(0x100234F4), 0x9BC, STR(0x10023568));
    setJointCallBack(mpMorf->getModel(), mHeadJointIdx, 0x022F6A34 /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), mBboneJointIdx, 0x022F6B98 /* nodeCB_BackBone */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return true;
}
VERIFY(0x022F6BE0, &daNpc_Ym1_c::bodyCreateHeap);

/* 022F6EF4 */
/* HD: one btp (a_btp_resID_tbl[0] at .data 0x10023580, the parameter is not used); its id
 * 0xE is replaced by mSubType */
s32 daNpc_Ym1_c::btpResID(s32 i_btpNum) {
    WWHD_FUNC(0x022F6EF4, s32, this, i_btpNum);
    s32 resID = gabi::load<s32>(0x10023580);
    if (resID == 0xE) {
        switch (mSubType) {
        case 1:
            return 0xE;
        case 2:
            return 0xF;
        }
    }
    return resID;
}
VERIFY(0x022F6EF4, &daNpc_Ym1_c::btpResID);

/* 022F6F38 */
bool daNpc_Ym1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_modify) {
    WWHD_FUNC(0x022F6F38, bool, this, i_btpNum, i_modify);
    J3DModel* model = mpHeadModel;
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    mA1A = i_btpNum; /* mBtpNum */
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(model), a_btp, 1, 0, 1.0f, 0, -1, i_modify, 0) != 0;
}
VERIFY(0x022F6F38, &daNpc_Ym1_c::init_texPttrnAnm);

/* 022F7024 */
bool daNpc_Ym1_c::headCreateHeap() {
    WWHD_FUNC(0x022F7024, bool, this);
    /* a_hed_resID_tbl (.data 0x101C6A24), by mSubType */
    s32 resID = gabi::load<s32>(0x101C6A24 + mSubType * 4);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, resID);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2533, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023584), 0x9E5, STR(0x10023594));
    mpHeadModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x15020022);
    if (mpHeadModel.get() == nullptr) {
        return false;
    }
    /* a_btp_num_tbl (.data 0x101C6A20), by mSubType */
    if (!init_texPttrnAnm(gabi::load<s8>(0x101C6A20 + mSubType), false)) {
        return false;
    }
    return true;
}
VERIFY(0x022F7024, &daNpc_Ym1_c::headCreateHeap);

/* 022F7114 */
bool daNpc_Ym1_c::itemCreateHeap() {
    WWHD_FUNC(0x022F7114, bool, this);
    if (mStaff != 0) {
        m7E8 = nullptr;
        return true;
    }
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 9);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2567, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x100235A8), 0xA07, STR(0x100235B8));
    m7E8 = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    if (m7E8.get() == nullptr) {
        return false;
    }
    return true;
}
VERIFY(0x022F7114, &daNpc_Ym1_c::itemCreateHeap);

/* 022F71E0 */
bool daNpc_Ym1_c::CreateHeap() {
    WWHD_FUNC(0x022F71E0, bool, this);
    if (!bodyCreateHeap()) {
        return false;
    }
    if (!headCreateHeap() || !itemCreateHeap()) {
        mpMorf = nullptr;
        return false;
    }
    mAcchCir.SetWall(30.0f, 60.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return true;
}
VERIFY(0x022F71E0, &daNpc_Ym1_c::CreateHeap);

/* 022F72AC */
/* tail call: CreateHeap's result register is passed through (typed u32) */
static u32 CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022F72AC, u32, i_this);
    return gabi::call<u32>(0x022F71E0, i_this); /* static_cast<daNpc_Ym1_c*>(i_this)->CreateHeap() */
}
VERIFY(0x022F72AC, CheckCreateHeap);

/* 022F72B0 */
bool daNpc_Ym1_c::decideType(int i_prm) {
    WWHD_FUNC(0x022F72B0, bool, this, i_prm);
    if (mSubType > 0) {
        return true;
    }
    s16 name = fpcM_GetName(this);
    mSubType = -1;
    mStaff = -1;
    if (name == fpcNm_NPC_YM1_e) {
        mSubType = 1; /* Mesa */
        if ((u32)i_prm <= 1)
            mStaff = (s8)i_prm;
    } else if (name == fpcNm_NPC_YM2_e) {
        mSubType = 2; /* Abe */
        if ((u32)i_prm <= 3)
            mStaff = gabi::load<s8>(0x100235D8 + i_prm); /* a_staff_tbl */
    }
    /* strcpy(mArcName, "Ym") (HD: a 3-byte string move) */
    for (int i = 0; i < 3; i++) {
        gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x100235DC + i));
    }
    return mSubType != -1 && mStaff != -1;
}
VERIFY(0x022F72B0, &daNpc_Ym1_c::decideType);

/* 022F7388 */
BOOL daNpc_Ym1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x022F7388, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
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
        mA22 = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mA22 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x022F7388, &daNpc_Ym1_c::set_action);

static inline void set_action_pmf(daNpc_Ym1_c* i_this, u32 pmf_addr) {
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, pmf_addr);
    i_this->set_action(pmf, nullptr);
}

/* 022F74B4 */
bool daNpc_Ym1_c::init_YM1_0() {
    WWHD_FUNC(0x022F74B4, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520)) {
        /* HD: only in the stage "Demo46" (start stage name at 0x1047E6B8) */
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        b->__vtbl = SAFESTRING_VTBL;
        b->mStringTop = 0x1047E6B8;
        a->mStringTop = 0x100235E0; /* "Demo46" */
        a->__vtbl = SAFESTRING_VTBL;
        gabi::call(0x022FAEE8, a.get()); /* sead::SafeString::assureTerminationImpl_ (empty, this TU's copy) */
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
        u32 s1 = a->mStringTop;
        gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
        u32 s2 = b->mStringTop;
        if (s1 != s2) {
            bool equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 c1 = gabi::load<u8>(s1 + i);
                u8 c2 = gabi::load<u8>(s2 + i);
                if (c1 != c2)
                    break;
                if (c1 == 0) {
                    equal = true;
                    break;
                }
            }
            if (!equal)
                return false;
        }
    }
    set_action_pmf(this, PMF_wait_action1);
    return true;
}
VERIFY(0x022F74B4, &daNpc_Ym1_c::init_YM1_0);

/* 022F75E4 */
bool daNpc_Ym1_c::init_YM1_1() {
    WWHD_FUNC(0x022F75E4, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520)) {
        set_action_pmf(this, PMF_wait_action4);
        return true;
    }
    return false;
}
VERIFY(0x022F75E4, &daNpc_Ym1_c::init_YM1_1);

/* 022F7668 */
bool daNpc_Ym1_c::init_YM2_0() {
    WWHD_FUNC(0x022F7668, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, PMF_wait_action3);
        return true;
    }
    return false;
}
VERIFY(0x022F7668, &daNpc_Ym1_c::init_YM2_0);

/* 022F7710 */
bool daNpc_Ym1_c::init_YM2_1() {
    WWHD_FUNC(0x022F7710, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, PMF_wait_action2);
        return true;
    }
    return false;
}
VERIFY(0x022F7710, &daNpc_Ym1_c::init_YM2_1);

/* 022F77B8 */
bool daNpc_Ym1_c::init_YM2_2() {
    WWHD_FUNC(0x022F77B8, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        set_action_pmf(this, PMF_wait_action2);
        return true;
    }
    return false;
}
VERIFY(0x022F77B8, &daNpc_Ym1_c::init_YM2_2);

/* 022F7848 */
bool daNpc_Ym1_c::init_YM2_3() {
    WWHD_FUNC(0x022F7848, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, PMF_wait_action2);
        return true;
    }
    return false;
}
VERIFY(0x022F7848, &daNpc_Ym1_c::init_YM2_3);

/* 022F78D8 */
bool daNpc_Ym1_c::init_YMx_error() {
    WWHD_FUNC(0x022F78D8, bool, this);
    set_action_pmf(this, PMF_demo_action1);
    return true;
}
VERIFY(0x022F78D8, &daNpc_Ym1_c::init_YMx_error);

/* 022F7918 */
void daNpc_Ym1_c::play_texPttrnAnm() {
    WWHD_FUNC(0x022F7918, void, this);
    if (mA1A != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        void* btp = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)); /* mBtpAnm.getBtpAnm() */
        if ((s32)frame >= J3DAnm_getFrameMax(btp)) {
            if (mA1A != 0) {
                btp = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10));
                mBlinkFrame = (u8)J3DAnm_getFrameMax(btp);
            } else {
                mBlinkTimer = (s16)cLib_getRndValue(60, 90);
                mBlinkFrame = 0;
            }
        }
    }
}
VERIFY(0x022F7918, &daNpc_Ym1_c::play_texPttrnAnm);

/* 022F79D0 */
void daNpc_Ym1_c::play_animation() {
    WWHD_FUNC(0x022F79D0, void, this);
    play_texPttrnAnm();
    mA08 = (u8)mpMorf->play(&eyePos, 0, 0); /* mbMorfAnimStopped */
    mDoExt_McaMorf* morf = mpMorf;
    if (morf->getFrame() < m9EC) {
        morf = mpMorf;
        mA08 = 1;
    }
    m9EC = morf->getFrame(); /* mPrevMorfFrame */
}
VERIFY(0x022F79D0, &daNpc_Ym1_c::play_animation);

/* 022F7A3C */
u8 daNpc_Ym1_c::chk_nbt_attn() {
    WWHD_FUNC(0x022F7A3C, u8, this);
    return (u32)(mA1B - 5) <= 2;
}
VERIFY(0x022F7A3C, &daNpc_Ym1_c::chk_nbt_attn);

/* 022F7A60 */
void daNpc_Ym1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x022F7A60, void, this, i_setEyePos);
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    /* l_HIO.mChild[mSubType] attention offset (0x10468970 + mSubType * 0x2C) */
    f32 offs = gabi::load<f32>(0x10468970 + mSubType * 0x2C);
    if (chk_nbt_attn()) {
        z = m9D4.z;
        if (mA1B == 6) {
            offs = m9E4 - 210.0f;
            y = m9D4.y;
            x = m9D4.x;
        } else {
            x = m9D4.x;
            y = m9D4.y;
            offs = 116.0f;
        }
    }
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = x;
    attPos->z = z;
    attPos->y = y + offs;
    if (m9F8 != 0 || i_setEyePos) {
        eyePos.z = m9BC.z;
        eyePos.y = m9BC.y;
        eyePos.x = m9BC.x;
    }
}
VERIFY(0x022F7A60, &daNpc_Ym1_c::setAttention);

/* 022F7B28 */
void daNpc_Ym1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x022F7B28, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), m9B6.x, m9B6.y, m9B6.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    /* the head model follows the head joint */
    mtx_copy(gabi::at<Mtx34>(gabi::ea(mpHeadModel.get()) + 0xC8), getAnmMtx(mpMorf->getModel(), mHeadJointIdx));
    J3DModel_calc(mpHeadModel);
    if (m7E8.get() != nullptr) {
        /* the item is held in the right hand */
        mtx_copy(gabi::at<Mtx34>(gabi::ea(m7E8.get()) + 0xC8), getAnmMtx(mpMorf->getModel(), mHandRJointIndex));
        J3DModel_calc(m7E8);
    }
    setAttention(i_setEyePos);
}
VERIFY(0x022F7B28, &daNpc_Ym1_c::setMtx);
