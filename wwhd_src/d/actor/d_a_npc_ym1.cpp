/**
 * d_a_npc_ym1.cpp (WWHD)
 * NPC - Mesa & Abe (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ym1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part A: area_check, joint callbacks, heap, decideType, set_action, init_*, animation, setMtx.
 */
#define SAFESTRING_VTBL 0x10023448 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ym1_readable.h"

enum { fpcNm_NPC_YM1_e = 0x13B, fpcNm_NPC_YM2_e = 0x13C };
enum { dSv_event_flag_UNK_0520 = 0x0520, dSv_event_flag_UNK_0E20 = 0x0E20 };

// HD: resource tables and eight-byte GHS member pointers belong to this translation unit.
static s32 a_bdl_resID_tbl[4] = {};
static s32 a_hed_resID_tbl[4] = {};
static s8 a_btp_num_tbl[4] = {};
static s32 a_btp_resID_tbl[1] = {};
static s8 a_staff_tbl[4] = {};
static ProcFunc_l wait_action1_pmf;
static ProcFunc_l wait_action4_pmf;
static ProcFunc_l wait_action3_pmf;
static ProcFunc_l wait_action2_pmf;
static ProcFunc_l demo_action1_pmf;
static cXyz a_eye_pos_off;
static u32 eyeOffsetInitialized;
static Ym1ChildHIO attentionHIO[3] = {};
static Ym1Text archiveName;
static Ym1Text demoStageName;
static Ym1StageName startStageName;

/* 022F68AC */
void daNpc_Ym1_c::setKariFlg() { mKariFlag = 1; }

/* 022F68B8 */
/* dCcMassS_Mng::SetAreaChk callback (_execute): the grass-cutting area hit this NPC */
static void area_check(fopAc_ac_c *i_actor, cXyz *, u32 i_flag) {
    if (i_flag == 0) {
        static_cast<daNpc_Ym1_c *>(i_actor)->setKariFlg();
    }
}

/* 022F68C4 */
void daNpc_Ym1_c::_nodeCB_Head(J3DNode *i_node, J3DModel *i_model) {
    // HD: the guard is published before the z/x/y stores.
    if (eyeOffsetInitialized == 0) {
        eyeOffsetInitialized = 1;
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.x = 26.0f;
        a_eye_pos_off.y = -25.0f;
    }
    u32 jntNo = reinterpret_cast<J3DJoint *>(i_node)->getJntNo();
    Mtx34 *stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jntNo), stk);
    ym1State(this)->mHeadPosition.x = stk->m[0][3];
    ym1State(this)->mHeadPosition.y = stk->m[1][3];
    ym1State(this)->mHeadPosition.z = stk->m[2][3];
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[0][0]);
    PSMTXMultVec(stk, &a_eye_pos_off, &ym1State(this)->mEyePosition);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jntNo), stk);
}

/* 022F6A34 */
static BOOL nodeCB_Head(J3DNode *i_node, int i_timing) {
    if (i_timing == 0) {
        daNpc_Ym1_c *user = static_cast<daNpc_Ym1_c *>(j3dSys.getModel()->getUserArea());
        if (user != nullptr) {
            user->_nodeCB_Head(i_node, j3dSys.getModel());
        }
    }
    return TRUE;
}

/* 022F6A7C */
void daNpc_Ym1_c::_nodeCB_BackBone(J3DNode *i_node, J3DModel *i_model) {
    u32 jntNo = reinterpret_cast<J3DJoint *>(i_node)->getJntNo();
    Mtx34 *stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jntNo), stk);
}

/* 022F6B98 */
static BOOL nodeCB_BackBone(J3DNode *i_node, int i_timing) {
    if (i_timing == 0) {
        daNpc_Ym1_c *user = static_cast<daNpc_Ym1_c *>(j3dSys.getModel()->getUserArea());
        if (user != nullptr) {
            user->_nodeCB_BackBone(i_node, j3dSys.getModel());
        }
    }
    return TRUE;
}

/* 022F6BE0 */
bool daNpc_Ym1_c::bodyCreateHeap() {
    /* a_bdl_resID_tbl, indexed by mSubType */
    J3DModelData *a_mdl_dat =
        (J3DModelData *)dComIfG_getObjectIDRes(mArcName, a_bdl_resID_tbl[mSubType]);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2471, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9A7, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x15021222);
    if (mpMorf.get() == nullptr) {
        return false;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return false;
    }
    mHeadJointIdx = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head" /* "head" */);
    if (mHeadJointIdx < 0)
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9B6, "m_hed_jnt_num >= 0");
    mBboneJointIdx =
        JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "backbone" /* "backbone" */);
    if (mBboneJointIdx < 0)
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9B8, "m_bbone_jnt_num >= 0");
    mHandLJointIndex =
        JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "handL" /* "handL" */);
    if (mHandLJointIndex < 0)
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9BA, "m_hnd_L_jnt_num >= 0");
    mHandRJointIndex =
        JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "handR" /* "handR" */);
    if (mHandRJointIndex < 0)
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9BC, "m_hnd_R_jnt_num >= 0");
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)mHeadJointIdx)
        ->setCallBack(nodeCB_Head);
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer((u16)(s16)mBboneJointIdx)
        ->setCallBack(nodeCB_BackBone);
    mpMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(this));
    return true;
}

/* 022F6EF4 */
/* HD: one btp (a_btp_resID_tbl[0], the parameter is not used); its id
 * 0xE is replaced by mSubType */
s32 daNpc_Ym1_c::btpResID(s32 i_btpNum) {
    s32 resID = a_btp_resID_tbl[0];
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

/* 022F6F38 */
bool daNpc_Ym1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_modify) {
    J3DModel *model = mpHeadModel;
    if (i_btpNum < 0) {
        return false;
    }
    void *a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    ym1State(this)->mBtpNum = i_btpNum; /* mBtpNum */
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, model->getModelData(), a_btp, 1, 0, 1.0f, 0, -1, i_modify,
                              0) != 0;
}

/* 022F7024 */
bool daNpc_Ym1_c::headCreateHeap() {
    /* a_hed_resID_tbl, indexed by mSubType */
    s32 resID = a_hed_resID_tbl[mSubType];
    J3DModelData *a_mdl_dat = (J3DModelData *)dComIfG_getObjectIDRes(mArcName, resID);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2533, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0x9E5, "a_mdl_dat != (0)");
    mpHeadModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x15020022);
    if (mpHeadModel.get() == nullptr) {
        return false;
    }
    /* a_btp_num_tbl, indexed by mSubType */
    if (!init_texPttrnAnm(a_btp_num_tbl[mSubType], false)) {
        return false;
    }
    return true;
}

/* 022F7114 */
bool daNpc_Ym1_c::itemCreateHeap() {
    if (mStaff != 0) {
        ym1State(this)->mItemModel = nullptr;
        return true;
    }
    J3DModelData *a_mdl_dat = (J3DModelData *)dComIfG_getObjectIDRes(mArcName, 9);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2567, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ym1.cpp", 0xA07, "a_mdl_dat != (0)");
    ym1State(this)->mItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    if (ym1State(this)->mItemModel.get() == nullptr) {
        return false;
    }
    return true;
}

/* 022F71E0 */
bool daNpc_Ym1_c::CreateHeap() {
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

/* 022F72AC */
/* tail call: CreateHeap's result register is passed through (typed u32) */
static u32 CheckCreateHeap(fopAc_ac_c *i_this) {
    return ym1CreateHeapResult(i_this); /* static_cast<daNpc_Ym1_c*>(i_this)->CreateHeap() */
}

/* 022F72B0 */
bool daNpc_Ym1_c::decideType(int i_prm) {
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
            mStaff = a_staff_tbl[i_prm]; /* a_staff_tbl */
    }
    /* strcpy(mArcName, "Ym") (HD: a 3-byte string move) */
    for (int i = 0; i < 3; i++) {
        reinterpret_cast<be<u8> *>(mArcName)[i] = archiveName.bytes[i];
    }
    return mSubType != -1 && mStaff != -1;
}

/* 022F7388 */
BOOL daNpc_Ym1_c::set_action(ProcFunc_l *i_newProcFunc, void *i_argsP) {
    ProcFunc_l *cur = &mCurrProcFunc;
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
        ym1State(this)->mActionState = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    ym1State(this)->mActionState = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}

static inline void set_action_pmf(daNpc_Ym1_c *i_this, const ProcFunc_l *action) {
    gabi::Local<ProcFunc_l> pmf;
    ym1CopyAction(pmf, action);
    i_this->set_action(pmf, nullptr);
}

/* 022F74B4 */
bool daNpc_Ym1_c::init_YM1_0() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520)) {
        /* HD: only in the stage "Demo46"  */
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        b->__vtbl = SAFESTRING_VTBL;
        b->mStringTop = gabi::ea(&startStageName);
        a->mStringTop = gabi::ea(&demoStageName);
        a->__vtbl = SAFESTRING_VTBL;
        ym1AssureTermination(
            a.get()); /* sead::SafeString::assureTerminationImpl_ (empty, this TU's copy) */
        gabi::call_ptr(ym1StringVtable(a)->assure.get(), a.get());
        u32 s1 = a->mStringTop;
        gabi::call_ptr(ym1StringVtable(b)->assure.get(), b.get());
        u32 s2 = b->mStringTop;
        if (s1 != s2) {
            bool equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 c1 = gabi::at<be<u8>>(s1)[i];
                u8 c2 = gabi::at<be<u8>>(s2)[i];
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
    set_action_pmf(this, &wait_action1_pmf);
    return true;
}

/* 022F75E4 */
bool daNpc_Ym1_c::init_YM1_1() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520)) {
        set_action_pmf(this, &wait_action4_pmf);
        return true;
    }
    return false;
}

/* 022F7668 */
bool daNpc_Ym1_c::init_YM2_0() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) &&
        !dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, &wait_action3_pmf);
        return true;
    }
    return false;
}

/* 022F7710 */
bool daNpc_Ym1_c::init_YM2_1() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) &&
        dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, &wait_action2_pmf);
        return true;
    }
    return false;
}

/* 022F77B8 */
bool daNpc_Ym1_c::init_YM2_2() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        set_action_pmf(this, &wait_action2_pmf);
        return true;
    }
    return false;
}

/* 022F7848 */
bool daNpc_Ym1_c::init_YM2_3() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, &wait_action2_pmf);
        return true;
    }
    return false;
}

/* 022F78D8 */
bool daNpc_Ym1_c::init_YMx_error() {
    set_action_pmf(this, &demo_action1_pmf);
    return true;
}

/* 022F7918 */
void daNpc_Ym1_c::play_texPttrnAnm() {
    if (ym1State(this)->mBtpNum != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        void *btp = reinterpret_cast<Ym1BtpAnm *>(mBtpAnm)->mpAnm.get(); /* mBtpAnm.getBtpAnm() */
        if ((s32)frame >= J3DAnm_getFrameMax(btp)) {
            if (ym1State(this)->mBtpNum != 0) {
                btp = reinterpret_cast<Ym1BtpAnm *>(mBtpAnm)->mpAnm.get();
                mBlinkFrame = (u8)J3DAnm_getFrameMax(btp);
            } else {
                mBlinkTimer = (s16)cLib_getRndValue(60, 90);
                mBlinkFrame = 0;
            }
        }
    }
}

/* 022F79D0 */
void daNpc_Ym1_c::play_animation() {
    play_texPttrnAnm();
    ym1State(this)->mMorfAnimationStopped = (u8)mpMorf->play(&eyePos, 0, 0); /* mbMorfAnimStopped */
    mDoExt_McaMorf *morf = mpMorf;
    if (morf->getFrame() < ym1State(this)->mPreviousMorfFrame) {
        morf = mpMorf;
        ym1State(this)->mMorfAnimationStopped = 1;
    }
    ym1State(this)->mPreviousMorfFrame = morf->getFrame(); /* mPrevMorfFrame */
}

/* 022F7A3C */
u8 daNpc_Ym1_c::chk_nbt_attn() { return (u32)(ym1State(this)->mAnimationAttribute - 5) <= 2; }

/* 022F7A60 */
void daNpc_Ym1_c::setAttention(u32 i_setEyePos) {
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    /* l_HIO.mChild[mSubType] attention offset  */
    f32 offs = attentionHIO[mSubType].mAttentionOffset;
    if (chk_nbt_attn()) {
        z = ym1State(this)->mNbtAttentionPosition.z;
        if (ym1State(this)->mAnimationAttribute == 6) {
            offs = ym1State(this)->mHeadPosition.y - 210.0f;
            y = ym1State(this)->mNbtAttentionPosition.y;
            x = ym1State(this)->mNbtAttentionPosition.x;
        } else {
            x = ym1State(this)->mNbtAttentionPosition.x;
            y = ym1State(this)->mNbtAttentionPosition.y;
            offs = 116.0f;
        }
    }
    cXyz *attPos = &attention_info.position; /* attention_info.position */
    attPos->x = x;
    attPos->z = z;
    attPos->y = y + offs;
    if (ym1State(this)->mEyePositionEnabled != 0 || i_setEyePos) {
        eyePos.z = ym1State(this)->mEyePosition.z;
        eyePos.y = ym1State(this)->mEyePosition.y;
        eyePos.x = ym1State(this)->mEyePosition.x;
    }
}

/* 022F7B28 */
void daNpc_Ym1_c::setMtx(u32 i_setEyePos) {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), ym1State(this)->mModelAngle.x,
                   ym1State(this)->mModelAngle.y, ym1State(this)->mModelAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    /* the head model follows the head joint */
    mtx_copy(&mpHeadModel->mBaseTransformMtx, mpMorf->getModel()->getAnmMtx(mHeadJointIdx));
    mpHeadModel->calc();
    if (ym1State(this)->mItemModel.get() != nullptr) {
        /* the item is held in the right hand */
        mtx_copy(&ym1State(this)->mItemModel->mBaseTransformMtx,
                 mpMorf->getModel()->getAnmMtx(mHandRJointIndex));
        ym1State(this)->mItemModel->calc();
    }
    setAttention(i_setEyePos);
}

/* WWHD glue (tools/verify/autoglue.py)
data a_bdl_resID_tbl 101C6A14 nocheck
data a_hed_resID_tbl 101C6A24 nocheck
data a_btp_num_tbl 101C6A20 nocheck
data a_btp_resID_tbl 10023580 nocheck
data a_staff_tbl 100235D8 nocheck
data wait_action1_pmf 10023410 nocheck
data wait_action4_pmf 10023418 nocheck
data wait_action3_pmf 10023420 nocheck
data wait_action2_pmf 10023428 nocheck
data demo_action1_pmf 10023430 nocheck
data a_eye_pos_off 1046896C nocheck
data eyeOffsetInitialized 104689DC nocheck
data attentionHIO 10468970 nocheck
data archiveName 100235DC nocheck
data demoStageName 100235E0 nocheck
data startStageName 1047E6B8 nocheck
ext 022F71E0 u32 ym1CreateHeapResult(fopAc_ac_c*)
ext 022FAEE8 void ym1AssureTermination(SafeString*)
str 100234DC
str 100234E4
str 100234EC
str 100234F4 in 022F6BE0
str 10023504 in 022F6BE0
str 10023518
str 1002352C
str 10023538
str 10023550
str 10023568
str 10023584 in 022F7024
str 10023594 in 022F7024
str 100235A8 in 022F7114
str 100235B8 in 022F7114
*/
