/**
 * d_a_npc_yw1.cpp (WWHD)
 * NPC - Sue-Belle (Outset; carries a pot of water, watches the cat)
 *
 * The GameCube decompilation of this TU is
 * "Nonmatching" stubs (zeldaret/tww src/d/actor/d_a_npc_yw1.cpp): every function here is written
 * from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names. The hair
 * physics (setHairAngle) is in d_a_npc_yw1_hair.cpp.
 */
#include "d/actor/d_a_npc_yw1.h"

#define SAFESTRING_VTBL 0x100236F0
#define YW1_VTBL 0x10023920
#define HIO_VTBL 0x10023738
#define CHILDHIO_VTBL 0x10023728
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */
#define CheckCreateHeap_addr 0x022FBB00u
#define searchActor_Bm1_addr 0x022FBB04u
#define nodeCB_Hair_addr 0x022FB0C8u
#define nodeCB_Head_addr 0x022FB288u
#define nodeCB_BackBone_addr 0x022FB3F4u

/* TU globals: the partner search (searchActor_Bm1 fills them) */
static inline be<s32>& l_bm1_cnt() { return *gabi::at<be<s32>>(0x104689E0); }
static inline u32 l_bm1_tbl(s32 i) { return 0x10468A68 + 4 * i; } /* fopAc_ac_c* [20] */
#define l_HIO l_HIO_yw1()
#define l_child(i) l_HIO_child(i)

/* TU wrapper: resource names are SafeStrings with this TU's vtable */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 index) { return dComIfG_getObjectIDRes(arc, index, SAFESTRING_VTBL); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* mDoExt_McaMorf::play(pos, u32 se, s8 reverb): the reverb register is passed through as loaded */
static inline BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb) { return gabi::call<BOOL>(0x025E535C, m, pos, se, reverb); }
static inline BOOL dNpc_PathRun_nextIdx(dNpc_PathRun_c_l* p) { return gabi::call<BOOL>(0x0259ECFC, p); }
static inline void fopAcM_setCarryNow_l(fopAc_ac_c* a, BOOL stageLayer) { gabi::call(0x025D9D0C, a, stageLayer); }
static inline void __construct_array(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
/* dComIfGp_setItemRupeeCount(n): play+0x5B48 += n */
static inline void dComIfGp_setItemRupeeCount(s32 n) {
    u32 a = dComIfGp_ea() + 0x5B48;
    gabi::store<s32>(a, gabi::load<s32>(a) + n);
}

/* 022FAEEC */
void daNpc_Yw1_c::_nodeCB_Hair(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x022FAEEC, void, this, node, model);
    /* static cXyz (18.0f, 20.0f, 0.0f): initialised, not used */
    if (gabi::load<u32>(0x10468AB8) == 0) {
        gabi::store<u32>(0x10468AB8, 1);
        cXyz* v = gabi::at<cXyz>(0x10468A00);
        v->z = 0.0f;
        v->x = 18.0f;
        v->y = 20.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(node);
    u16 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), stack);
    if (m_hair1_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, m8FA);
        mDoMtx_ZrotM(stack, (s16)(m8F8 + m8F4 + m92A));
    } else if (m_hair2_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, m91A);
        mDoMtx_ZrotM(stack, (s16)(m918 + m92C));
    } else if (m_hair3_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, m922);
        mDoMtx_ZrotM(stack, (s16)(m920 + m92E));
    }
    PSMTXCopy(stack, J3DSys_mCurrentMtx);
    mtx_copy(J3DModel_getAnmMtx(model, jntNo), stack); /* model->setAnmMtx(jntNo, stack) */
}
VERIFY(0x022FAEEC, &daNpc_Yw1_c::_nodeCB_Hair);

/* 022FB0C8 */
static BOOL nodeCB_Hair(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022FB0C8, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpc_Yw1_c* i_this = gabi::at<daNpc_Yw1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeCB_Hair(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022FB0C8, nodeCB_Hair);

/* 022FB110 */
void daNpc_Yw1_c::_nodeCB_Head(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x022FB110, void, this, node, model);
    /* static cXyz a_eye_pos_offst(18.0f, 20.0f, 0.0f) */
    cXyz* a_eye_pos_offst = gabi::at<cXyz>(0x10468A0C);
    if (gabi::load<u32>(0x10468ABC) == 0) {
        gabi::store<u32>(0x10468ABC, 1);
        a_eye_pos_offst->z = 0.0f;
        a_eye_pos_offst->x = 18.0f;
        a_eye_pos_offst->y = 20.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(node);
    u16 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), stack);
    mAttPos.x = (f32)stack->m[0][3];
    mAttPos.y = (f32)stack->m[1][3];
    mAttPos.z = (f32)stack->m[2][3];
    mDoMtx_XrotM(stack, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[0][0]);
    PSMTXMultVec(stack, a_eye_pos_offst, &mEyePos);
    PSMTXCopy(stack, J3DSys_mCurrentMtx);
    mtx_copy(J3DModel_getAnmMtx(model, jntNo), stack);
}
VERIFY(0x022FB110, &daNpc_Yw1_c::_nodeCB_Head);

/* 022FB288 */
static BOOL nodeCB_Head(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022FB288, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpc_Yw1_c* i_this = gabi::at<daNpc_Yw1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeCB_Head(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022FB288, nodeCB_Head);

/* 022FB2D0 */
void daNpc_Yw1_c::_nodeCB_BackBone(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x022FB2D0, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    u16 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), stack);
    mDoMtx_XrotM(stack, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stack, J3DSys_mCurrentMtx);
    mtx_copy(J3DModel_getAnmMtx(model, jntNo), stack);
}
VERIFY(0x022FB2D0, &daNpc_Yw1_c::_nodeCB_BackBone);

/* 022FB3F4 */
static BOOL nodeCB_BackBone(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022FB3F4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpc_Yw1_c* i_this = gabi::at<daNpc_Yw1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeCB_BackBone(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022FB3F4, nodeCB_BackBone);

/* 022FB43C */
BOOL daNpc_Yw1_c::bodyCreateHeap() {
    WWHD_FUNC(0x022FB43C, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1002378C) /* "Yw" */, 6 /* BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x985, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023798), 0x985, STR(0x100237A8));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                    0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return FALSE;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(morf)) + 0xC), morf, 3); /* delete mpMorf */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x10023790) /* "head" */);
    if (m_head_jnt_num < 0) /* JUT_ASSERT(0x995, m_head_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10023798), 0x995, STR(0x100237BC));
    m_bbone_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x100237D0) /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(0x997, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10023798), 0x997, STR(0x100237DC));
    J3DModelData_setJointCallBack(J3DModel_modelData(mpMorf->getModel()), (u16)(s32)m_head_jnt_num, nodeCB_Head_addr);
    J3DModelData_setJointCallBack(J3DModel_modelData(mpMorf->getModel()), (u16)(s32)m_bbone_jnt_num, nodeCB_BackBone_addr);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    return TRUE;
}
VERIFY(0x022FB43C, &daNpc_Yw1_c::bodyCreateHeap);

/* 022FB6C0: HD: one texture pattern; the number is not used */
s32 daNpc_Yw1_c::btpResID(int) {
    WWHD_FUNC(0x022FB6C0, s32, this, (u32)0);
    return gabi::load<s32>(0x100237F4);
}
VERIFY(0x022FB6C0, &daNpc_Yw1_c::btpResID);

/* 022FB6CC */
BOOL daNpc_Yw1_c::init_texPttrnAnm(s8 num, s32 modify) {
    WWHD_FUNC(0x022FB6CC, BOOL, this, num, modify);
    J3DModel* hedModel = mpHedModel;
    if (num < 0) {
        return FALSE;
    }
    s32 resID = btpResID(num);
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x100237F8) /* "Yw" */, resID);
    mTexNo = num;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(&mBtpAnm, J3DModel_modelData(hedModel), btp, 1, 0 /* HD: EMode_NONE */, 1.0f, 0, -1, modify, 0) != 0;
}
VERIFY(0x022FB6CC, &daNpc_Yw1_c::init_texPttrnAnm);

/* 022FB7BC */
BOOL daNpc_Yw1_c::headCreateHeap() {
    WWHD_FUNC(0x022FB7BC, BOOL, this);
    J3DModelData* a_mdl_dat =
        (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10023814) /* "Yw" */, gabi::load<s32>(0x101C6DCC) /* head BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x9BA, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023818), 0x9BA, STR(0x10023828));
    mpHedModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x15020022);
    if (mpHedModel.get() == nullptr) {
        return FALSE;
    }
    if (!init_texPttrnAnm(gabi::load<s8>(0x101C6DD0), 0)) {
        return FALSE;
    }
    m_hair1_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x100237FC) /* "hair1" */);
    if (m_hair1_jnt_num < 0) JUT_ASSERT_fail(STR(0x10023818), 0x9CC, STR(0x1002383C));
    m_hair2_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x10023804) /* "hair2" */);
    if (m_hair2_jnt_num < 0) JUT_ASSERT_fail(STR(0x10023818), 0x9CE, STR(0x1002384C));
    m_hair3_jnt_num = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x1002380C) /* "hair3" */);
    if (m_hair3_jnt_num < 0) JUT_ASSERT_fail(STR(0x10023818), 0x9D0, STR(0x1002385C));
    J3DModelData_setJointCallBack(J3DModel_modelData(mpHedModel), (u16)(s32)m_hair1_jnt_num, nodeCB_Hair_addr);
    J3DModelData_setJointCallBack(J3DModel_modelData(mpHedModel), (u16)(s32)m_hair2_jnt_num, nodeCB_Hair_addr);
    J3DModelData_setJointCallBack(J3DModel_modelData(mpHedModel), (u16)(s32)m_hair3_jnt_num, nodeCB_Hair_addr);
    gabi::store<u32>(gabi::ea(mpHedModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */
    return TRUE;
}
VERIFY(0x022FB7BC, &daNpc_Yw1_c::headCreateHeap);

/* 022FBA48 */
BOOL daNpc_Yw1_c::CreateHeap() {
    WWHD_FUNC(0x022FBA48, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!headCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x022FBA48, &daNpc_Yw1_c::CreateHeap);

/* 022FBB00 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022FBB00, BOOL, i_this);
    return ((daNpc_Yw1_c*)i_this)->CreateHeap();
}
VERIFY(0x022FBB00, CheckCreateHeap);

/* 022FBB04 */
static void* searchActor_Bm1(void* p, void*) {
    WWHD_FUNC(0x022FBB04, void*, p, (u32)0);
    if (l_bm1_cnt() < 20 && fopAc_IsActor(p) && p != nullptr && fpcM_GetName(p) == 0x146 /* PROC_NPC_BM1 */) {
        s32 n = l_bm1_cnt();
        l_bm1_cnt() = n + 1;
        gabi::store<u32>(l_bm1_tbl(n), gabi::ea(p));
    }
    return nullptr;
}
VERIFY(0x022FBB04, searchActor_Bm1);

/* 022FBB84 */
u8 daNpc_Yw1_c::decideType(int type) {
    WWHD_FUNC(0x022FBB84, u8, this, type);
    mHioNo = 0;
    if ((u32)type > 3) {
        mType = -1;
        return FALSE;
    }
    mType = (s8)type;
    return mHioNo != -1 && (s8)type != -1;
}
VERIFY(0x022FBB84, &daNpc_Yw1_c::decideType);

/* 022FBBD8 */
BOOL daNpc_Yw1_c::set_action(ptmf_l* action, void* arg) {
    WWHD_FUNC(0x022FBBD8, BOOL, this, action, arg);
    s16 idx = action->idx;
    s16 delta = action->delta;
    u32 fn = action->fn;
    if (mAction.idx == idx) {
        if (idx == 0 || (mAction.delta == delta && mAction.fn == fn)) {
            return TRUE;
        }
    }
    if (mAction.idx != 0) {
        mActStep = 9; /* GameCube (km1): -1 */
        ptmf_call1(&mAction, this, arg);
    }
    mAction.fn = fn;
    mAction.delta = delta;
    mAction.idx = idx;
    mActStep = 0;
    ptmf_call1(&mAction, this, arg);
    return TRUE;
}
VERIFY(0x022FBBD8, &daNpc_Yw1_c::set_action);

static inline void set_action_tbl(daNpc_Yw1_c* i_this, u32 entry) {
    gabi::Local<ptmf_l> f;
    gabi::store<u32>(gabi::ea(f.get()), gabi::load<u32>(entry));
    gabi::store<u32>(gabi::ea(f.get()) + 4, gabi::load<u32>(entry + 4));
    i_this->set_action(f, nullptr);
}

/* 022FBD04 */
void daNpc_Yw1_c::set_pthPoint(u8 idx) {
    WWHD_FUNC(0x022FBD04, void, this, idx);
    if (mPathRun.mPath.get() != nullptr) {
        mPathRun.mIdx = idx;
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, idx);
        current.pos.copy(*pnt);
        if (dNpc_PathRun_nextIdx(&mPathRun)) {
            gabi::Local<cXyz> pnt2;
            dNpc_PathRun_getPoint(&mPathRun, pnt2, mPathRun.mIdx);
            gabi::Local<cXyz> next;
            next->copy(*pnt2);
            current.angle.y = cLib_targetAngleY(&current.pos, next);
        }
    }
}
VERIFY(0x022FBD04, &daNpc_Yw1_c::set_pthPoint);

/* 022FBDAC */
BOOL daNpc_Yw1_c::init_YW1_0() {
    WWHD_FUNC(0x022FBDAC, BOOL, this);
    if (dComIfGs_isEventBit(0x520) || dComIfGs_isEventBit(0x1) || mPathRun.mPath.get() == nullptr) {
        return FALSE;
    }
    mbTsubo = 1;
    mTsuboId = fopAcM_create(0x1C5 /* PROC_TSUBO */, 0x7F063F, &current.pos, current.roomNo, nullptr, nullptr, -1, 0);
    set_action_tbl(this, 0x100236C8); /* &daNpc_Yw1_c::wait_action1 */
    set_pthPoint(0);
    return mTsuboId != fpcM_ERROR_PROCESS_ID_e;
}
VERIFY(0x022FBDAC, &daNpc_Yw1_c::init_YW1_0);

/* 022FBEB4 */
BOOL daNpc_Yw1_c::init_YW1_1() {
    WWHD_FUNC(0x022FBEB4, BOOL, this);
    if (dComIfGs_isEventBit(0x520)) {
        return FALSE;
    }
    if (!dComIfGs_isEventBit(0x1)) {
        actor_status &= ~0x3Fu;
    }
    set_action_tbl(this, 0x100236D0); /* &daNpc_Yw1_c::wait_action2 */
    m8EB = 1;
    return TRUE;
}
VERIFY(0x022FBEB4, &daNpc_Yw1_c::init_YW1_1);

/* 022FBF70 */
BOOL daNpc_Yw1_c::init_YW1_2() {
    WWHD_FUNC(0x022FBF70, BOOL, this);
    if (!dComIfGs_isEventBit(0x520)) {
        return FALSE;
    }
    if (dKy_daynight_check() != 1 && dComIfGs_isEventBit(0x2A20)) {
        return FALSE;
    }
    set_action_tbl(this, 0x100236D0); /* &daNpc_Yw1_c::wait_action2 */
    return TRUE;
}
VERIFY(0x022FBF70, &daNpc_Yw1_c::init_YW1_2);

/* 022FC024 */
BOOL daNpc_Yw1_c::init_YW1_3() {
    WWHD_FUNC(0x022FC024, BOOL, this);
    if (!dComIfGs_isEventBit(0x520) || dKy_daynight_check() != 0 || mPathRun.mPath.get() == nullptr ||
        !dComIfGs_isEventBit(0x2A20)) {
        return FALSE;
    }
    mbTsubo = 1;
    mTsuboId = fopAcM_create(0x1C5 /* PROC_TSUBO */, 0x7F063F, &current.pos, current.roomNo, nullptr, nullptr, -1, 0);
    set_action_tbl(this, 0x100236C8); /* &daNpc_Yw1_c::wait_action1 */
    set_pthPoint(0);
    return mTsuboId != fpcM_ERROR_PROCESS_ID_e;
}
VERIFY(0x022FC024, &daNpc_Yw1_c::init_YW1_3);

static inline J3DAnmTexPattern* btpAnm_pattern(daNpc_Yw1_c* i) { return gabi::at<J3DAnmTexPattern>(gabi::load<u32>(gabi::ea(&i->mBtpAnm) + 0x10)); }

/* 022FC138 */
void daNpc_Yw1_c::play_texPttrnAnm() {
    WWHD_FUNC(0x022FC138, void, this);
    if (mTexNo == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if ((s32)frame < J3DAnmTexPattern_getFrameMax(btpAnm_pattern(this))) {
        return;
    }
    if (mTexNo != 0) {
        mBtpFrame = (u8)J3DAnmTexPattern_getFrameMax(btpAnm_pattern(this));
    } else {
        mBlinkTimer = (s16)cLib_getRndValue(60, 90);
        mBtpFrame = 0;
    }
}
VERIFY(0x022FC138, &daNpc_Yw1_c::play_texPttrnAnm);

/* 022FC1F0 */
void daNpc_Yw1_c::play_animation() {
    WWHD_FUNC(0x022FC1F0, void, this);
    u32 se = 0;
    play_texPttrnAnm();
    if (mObjAcch.ChkGroundHit()) {
        se = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd_poly(mObjAcch));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mAnmEnd = (s8)McaMorf_play(mpMorf, &eyePos, se, reverb);
    if (mpMorf->getFrame() < mPrevFrame) {
        mAnmEnd = 1;
    }
    mPrevFrame = mpMorf->getFrame();
}
VERIFY(0x022FC1F0, &daNpc_Yw1_c::play_animation);

/* 022FCBF4 */
fopAc_ac_c* daNpc_Yw1_c::searchByID(fpc_ProcID id, be<s32>* pErr) {
    WWHD_FUNC(0x022FCBF4, fopAc_ac_c*, this, id, pErr);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    if (pErr != nullptr) {
        *pErr = 0;
    }
    BOOL found = fopAcM_SearchByID2(id, actor);
    fopAc_ac_c* a = gabi::at<fopAc_ac_c>(*actor);
    if (!found && pErr != nullptr) {
        *pErr = 1;
    }
    return a;
}
VERIFY(0x022FCBF4, &daNpc_Yw1_c::searchByID);

/* 022FCC54: the pot follows the head */
u8 daNpc_Yw1_c::upLift() {
    WWHD_FUNC(0x022FCC54, u8, this);
    gabi::Local<be<s32>> err;
    fopAc_ac_c* tsubo = searchByID(mTsuboId, err);
    u8 lost = *err == 1;
    mbTsuboLost = lost;
    if (tsubo == nullptr) {
        return lost;
    }
    if (!(tsubo->actor_status & 0x2000)) {
        fopAcM_setCarryNow_l(tsubo, FALSE);
    }
    gabi::Local<cXyz> ofs;
    ofs->x = 34.0f;
    ofs->y = -4.0f;
    ofs->z = 0.0f;
    gabi::Local<cXyz> dir;
    PSMTXMultVecSR(J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num), gabi::at<cXyz>(0x10468A18) /* (0, 1, 0) */, dir);
    tsubo->shape_angle.y = cM_atan2s(dir->x, dir->z);
    PSMTXCopy(J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num), mDoMtx_stack_c::get());
    PSMTXMultVec(mDoMtx_stack_c::get(), ofs, &tsubo->current.pos);
    return mbTsuboLost;
}
VERIFY(0x022FCC54, &daNpc_Yw1_c::upLift);

/* 022FCD88 */
void daNpc_Yw1_c::setAttention(s32 force) {
    WWHD_FUNC(0x022FCD88, void, this, force);
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attnPos->set(current.pos.x, current.pos.y + l_child(mHioNo).mAttnYOffset, current.pos.z);
    if (mActRet == 0 && !force) {
        return;
    }
    eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z);
}
VERIFY(0x022FCD88, &daNpc_Yw1_c::setAttention);

/* 022FCDEC */
void daNpc_Yw1_c::setMtx(s32 force) {
    WWHD_FUNC(0x022FCDEC, void, this, force);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setHairAngle();
    Mtx34* src = J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num);
    J3DModel_setBaseTRMtx(mpHedModel, src);
    J3DModel_calc(mpHedModel);
    upLift();
    setAttention(force);
}
VERIFY(0x022FCDEC, &daNpc_Yw1_c::setMtx);

/* 022FCF9C */
BOOL daNpc_Yw1_c::createInit() {
    WWHD_FUNC(0x022FCF9C, BOOL, this);
    setActorInfo2(&mEventCut, STR(0x100238B8) /* "Yw1" */, this);
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[1] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[3] */
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return FALSE;
        }
        actor_status &= ~0x80u;
        weight = 0xF0;
    }
    mAnmNo = 7;
    BOOL ok;
    switch (mType) {
    case 0:
        ok = init_YW1_0();
        break;
    case 1:
        ok = init_YW1_1();
        break;
    case 2:
        ok = init_YW1_2();
        break;
    case 3:
        ok = init_YW1_3();
        break;
    default:
        return FALSE;
    }
    if (!ok) {
        return FALSE;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    gravity = -4.5f;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(l_cyl_src);
    play_animation();
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::store<s8>(gabi::ea(this) + 0x1C9, (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)));    /* tevStr.mRoomNo */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch))); /* tevStr.mEnvrIdxOverride */
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return TRUE;
}
VERIFY(0x022FCF9C, &daNpc_Yw1_c::createInit);

/* 022FD19C */
cPhs_State daNpc_Yw1_c::_create() {
    WWHD_FUNC(0x022FD19C, cPhs_State, this);
    /* fopAcM_SetupActor(this, daNpc_Yw1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = YW1_VTBL;
            gabi::call(0x025E7820, &mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, STR(0x100238CC) /* "Yw" */);
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap_addr, gabi::load<u32>(0x101C6DD4) /* a_heap_size_tbl[0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -40.0f, -20.0f, -40.0f, 40.0f, 210.0f, 40.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}
VERIFY(0x022FD19C, &daNpc_Yw1_c::_create);

/* 022FD2D0 */
static cPhs_State daNpc_Yw1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022FD2D0, cPhs_State, i_this);
    return ((daNpc_Yw1_c*)i_this)->_create();
}
VERIFY(0x022FD2D0, daNpc_Yw1_Create);

/* 022FD2D4 */
BOOL daNpc_Yw1_c::_delete() {
    WWHD_FUNC(0x022FD2D4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x100238CF) /* "Yw" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) { /* HD: only when the heap was created */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022FD2D4, &daNpc_Yw1_c::_delete);

/* 022FD32C */
static BOOL daNpc_Yw1_Delete(daNpc_Yw1_c* i_this) {
    WWHD_FUNC(0x022FD32C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022FD32C, daNpc_Yw1_Delete);

/* 022FD330 */
BOOL daNpc_Yw1_c::partner_search_sub(u32 searchFn) {
    WWHD_FUNC(0x022FD330, BOOL, this, searchFn);
    BOOL ret = FALSE;
    mBm1Id = fpcM_ERROR_PROCESS_ID_e;
    l_bm1_cnt() = 0;
    for (int i = 0; i < 20; i++) {
        gabi::store<u32>(l_bm1_tbl(i), 0);
    }
    fpcM_Search(searchFn, this);
    if (l_bm1_cnt() != 0) {
        mBm1Id = fopAcM_GetID(gabi::at<void>(gabi::load<u32>(l_bm1_tbl(0))));
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x022FD330, &daNpc_Yw1_c::partner_search_sub);

/* 022FD3DC */
void daNpc_Yw1_c::partner_search() {
    WWHD_FUNC(0x022FD3DC, void, this);
    if (mActStep == 1) {
        if (mType != 0 || partner_search_sub(searchActor_Bm1_addr)) {
            mActStep = (s8)(mActStep + 1);
        }
    }
}
VERIFY(0x022FD3DC, &daNpc_Yw1_c::partner_search);

/* 022FD448 */
void daNpc_Yw1_c::checkOrder() {
    WWHD_FUNC(0x022FD448, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        return;
    }
    if (cmd != 1 /* dEvtCmd_INTALK_e */) {
        return;
    }
    if (mOrderType == 1 || mOrderType == 2) {
        mOrderType = 0;
        mbTalk = 1;
    }
}
VERIFY(0x022FD448, &daNpc_Yw1_c::checkOrder);

/* 022FD488 */
u8 daNpc_Yw1_c::demo() {
    WWHD_FUNC(0x022FD488, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo != 0) {
            mbDemo = 0;
        }
        return 0;
    }
    if (mbDemo == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        mbDemo = 1;
        m8EC = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* demoAc = nullptr;
    if (id != 0 && id <= 0x20) { /* dComIfGp_demo_getActor(id) */
        if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail(STR(0x10023770), 0x23A, STR(0x10023748));
        demoAc = dDemo_object_getActor(gabi::load<u32>(0x101D5FFC), id);
    }
    if (btpAnm_pattern(this) != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnmTexPattern_getFrameMax(btpAnm_pattern(this))) {
            mBtpFrame = (u8)J3DAnmTexPattern_getFrameMax(btpAnm_pattern(this));
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(demoAc, STR(0x100238D2) /* "Yw" */);
        if (btp != nullptr) {
            mDoExt_btpAnm_init(&mBtpAnm, J3DModel_modelData(mpHedModel), btp, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mTexNo = 1;
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x100238D2) /* "Yw" */, 0, 0, 0, 0);
    return mbDemo;
}
VERIFY(0x022FD488, &daNpc_Yw1_c::demo);

/* 022FD64C */
s32 daNpc_Yw1_c::isEventEntry() {
    WWHD_FUNC(0x022FD64C, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022FD64C, &daNpc_Yw1_c::isEventEntry);

/* 022FD68C */
void daNpc_Yw1_c::endEvent() {
    WWHD_FUNC(0x022FD68C, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mAnmTag = 0xFF;
}
VERIFY(0x022FD68C, &daNpc_Yw1_c::endEvent);

/* 022FD6D0 (matcher: unnamed) */
void daNpc_Yw1_c::privateCut(int staffIdx) {
    WWHD_FUNC(0x022FD6D0, void, this, staffIdx);
    if (staffIdx == -1) {
        return;
    }
    /* static char* cut_name_tbl[] = {"DUMMY"} (101C6DD8) */
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, 0x101C6DD8, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(staffIdx);
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}
VERIFY(0x022FD6D0, &daNpc_Yw1_c::privateCut);

/* 022FD768 */
void daNpc_Yw1_c::lookBack() {
    WWHD_FUNC(0x022FD768, void, this);
    gabi::Local<cXyz> dst;
    u8 headOnly = mbHeadOnly;
    dst->x = 0.0f;
    s16 targetY = current.angle.y;
    mSaveAngleY = targetY;
    mSaveHeadY = m_jnt.mAngles[0][1];
    dst->y = 0.0f;
    f32 eyeX = current.pos.x;
    f32 eyeY = eyePos.y;
    dst->z = 0.0f;
    mSaveBboneY = m_jnt.mAngles[1][1];
    f32 eyeZ = current.pos.z;
    cXyz* dstPos = nullptr;
    switch (mLookMode) {
    case 0:
        break;
    case 1: {
        gabi::Local<cXyz> tmp;
        dNpc_playerEyePos_l(tmp, -20.0f);
        dst->copy(*tmp);
        dstPos = dst;
        break;
    }
    case 2:
        dst->copy(mLookPos);
        dstPos = dst;
        break;
    case 3:
        targetY = mTargetAngY;
        break;
    case 4: {
        fopAc_ac_c* actor = searchByID(mLookActorId, nullptr);
        if (actor != nullptr) {
            mLookPos.copy(actor->current.pos);
            dst->x = (f32)mLookPos.x;
            f32 y = actor->eyePos.y;
            dst->y = y;
            dst->z = (f32)mLookPos.z;
            mLookPos.y = y;
            dstPos = dst;
        }
        break;
    }
    }
    cLib_addCalcAngleS2(&mHeadTurnSpd, l_child(mHioNo).mHeadTurnSpd, 4, 0x800);
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
VERIFY(0x022FD768, &daNpc_Yw1_c::lookBack);

/* 022FDA4C */
void daNpc_Yw1_c::event_proc(int staffIdx) {
    WWHD_FUNC(0x022FDA4C, void, this, staffIdx);
    if (!mEventCut.cutProc()) {
        privateCut(staffIdx);
    }
    lookBack();
}
VERIFY(0x022FDA4C, &daNpc_Yw1_c::event_proc);

/* 022FDAA4 */
void daNpc_Yw1_c::eventOrder() {
    WWHD_FUNC(0x022FDAA4, void, this);
    if (mOrderType == 1 || mOrderType == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderType == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x022FDAA4, &daNpc_Yw1_c::eventOrder);

/* 022FDADC */
BOOL daNpc_Yw1_c::_execute() {
    WWHD_FUNC(0x022FDADC, BOOL, this);
    if (mbInit == 0) {
        mHomeAngle.y = current.angle.y;
        gabi::store<u32>(gabi::ea(&mHomePos.y), gabi::load<u32>(gabi::ea(&current.pos.y)));
        mHomeAngle.z = current.angle.z;
        mHomeAngle.x = current.angle.x;
        gabi::store<u32>(gabi::ea(&mHomePos.z), gabi::load<u32>(gabi::ea(&current.pos.z)));
        gabi::store<u32>(gabi::ea(&mHomePos.x), gabi::load<u32>(gabi::ea(&current.pos.x)));
        mbInit = 1;
    }
    daNpc_Yw1_childHIO_c& hio = l_child(mHioNo);
    m_jnt.setParam(hio.mPrm[4], hio.mPrm[5], hio.mPrm[6], hio.mPrm[7], hio.mPrm[0], hio.mPrm[1], hio.mPrm[2], hio.mPrm[3],
                   hio.mPrm[8]);
    if (m8EB != 0 && demoActorID == 0) {
        if (!dComIfGs_isEventBit(0x1)) {
            return TRUE;
        }
        if (mType == 1) {
            actor_status = (actor_status & ~0x3Fu) | 0x28;
        }
        m8EB = 0;
    }
    partner_search();
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !checkCommandTalk */ &&
            (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            ptmf_call1(&mAction, this, nullptr);
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        play_animation();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (m8EC == 0) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    gabi::store<s8>(gabi::ea(this) + 0x1C9, (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)));    /* tevStr.mRoomNo */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch))); /* tevStr.mEnvrIdxOverride */
    setMtx(0);
    setCollision(30.0f, 150.0f);
    return TRUE;
}
VERIFY(0x022FDADC, &daNpc_Yw1_c::_execute);

/* 022FDD64 */
static BOOL daNpc_Yw1_Execute(daNpc_Yw1_c* i_this) {
    WWHD_FUNC(0x022FDD64, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022FDD64, daNpc_Yw1_Execute);

/* 022FDD68 */
BOOL daNpc_Yw1_c::_draw() {
    WWHD_FUNC(0x022FDD68, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel* hedModel = mpHedModel;
    u32 hedData = J3DModel_modelData(hedModel);
    if (m8EB != 0 || m8ED != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    dComIfGp_get(); /* HD: unused */
    mpMorf->entryDL();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&mBtpAnm, gabi::at<J3DModelData>(hedData), mBtpFrame);
    mDoExt_modelEntryDL(hedModel);
    gabi::store<u32>(hedData + 0x38, 0); /* mBtpAnm.remove(hedData) */
    dSnap_RegistFig(0x4B /* DSNAP_TYPE_NPC_YW1 */, this, 1.0f, 1.0f, 1.0f);
    if (l_child(mHioNo).mDebugDraw != 0) {
        /* debug colours (GXColor function-local statics, unused) */
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_l(0x101FEBF4, 0x100236D8, 4);
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_l(0x101FEBF8, 0x100236DC, 4);
        }
        if (gabi::load<u32>(0x101FDA44) == 0) {
            gabi::store<u32>(0x101FDA44, 1);
            memcpy_l(0x101FEBE8, 0x100236E0, 4);
        }
    }
    return TRUE;
}
VERIFY(0x022FDD68, &daNpc_Yw1_c::_draw);

/* 022FDEFC */
static BOOL daNpc_Yw1_Draw(daNpc_Yw1_c* i_this) {
    WWHD_FUNC(0x022FDEFC, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022FDEFC, daNpc_Yw1_Draw);

/* 022FDF00 */
static BOOL daNpc_Yw1_IsDelete(daNpc_Yw1_c*) {
    WWHD_FUNC(0x022FDF00, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x022FDF00, daNpc_Yw1_IsDelete);

/* 022FDF08 */
s32 daNpc_Yw1_c::bckResID(int num) {
    WWHD_FUNC(0x022FDF08, s32, this, num);
    return gabi::load<s32>(0x100238E4 + 4 * num); /* a_bck_resID_tbl */
}
VERIFY(0x022FDF08, &daNpc_Yw1_c::bckResID);

/* 022FDF1C */
void daNpc_Yw1_c::setAnm_anm(anm_prm_c* prm) {
    WWHD_FUNC(0x022FDF1C, void, this, prm);
    s8 anmNo = prm->mAnmNo;
    if (anmNo < 0 || mAnmNo == anmNo) {
        return;
    }
    s32 resID = bckResID(anmNo);
    s32 loop = prm->mLoopMode;
    f32 speed = prm->mSpeed;
    f32 morf = prm->mMorf;
    dNpc_setAnmIDRes(mpMorf, loop, morf, speed, resID, -1, STR(0x10023900) /* "Yw" */);
    mAnmNo = prm->mAnmNo;
    mPrevFrame = 0.0f;
    mAnmEnd = 0;
    mLoopCnt = 0;
}
VERIFY(0x022FDF1C, &daNpc_Yw1_c::setAnm_anm);

static inline daNpc_Yw1_c::anm_prm_c* anm_prm(u32 tbl, s32 i) { return gabi::at<daNpc_Yw1_c::anm_prm_c>(tbl + 0x10 * i); }

/* 022FDFB8 */
void daNpc_Yw1_c::setAnm_NUM(int num, int withTex) {
    WWHD_FUNC(0x022FDFB8, void, this, num, withTex);
    /* a_anm_prm_tbl (101C6DDC) */
    if (withTex) {
        init_texPttrnAnm(anm_prm(0x101C6DDC, num)->mTexNo, 1);
    }
    setAnm_anm(anm_prm(0x101C6DDC, num));
}
VERIFY(0x022FDFB8, &daNpc_Yw1_c::setAnm_NUM);

/* 022FE028 */
void daNpc_Yw1_c::setAnm() {
    WWHD_FUNC(0x022FE028, void, this);
    /* a_anm_prm_tbl (101C6E4C), by state */
    init_texPttrnAnm(anm_prm(0x101C6E4C, mStt)->mTexNo, 1);
    setAnm_anm(anm_prm(0x101C6E4C, mStt));
}
VERIFY(0x022FE028, &daNpc_Yw1_c::setAnm);

/* 022FE098 */
void daNpc_Yw1_c::setAnm_ATR() {
    WWHD_FUNC(0x022FE098, void, this);
    /* a_anm_prm_tbl (101C6EBC), by message attribute */
    init_texPttrnAnm(anm_prm(0x101C6EBC, mAnmAtr)->mTexNo, 1);
    setAnm_anm(anm_prm(0x101C6EBC, mAnmAtr));
}
VERIFY(0x022FE098, &daNpc_Yw1_c::setAnm_ATR);

/* 022FE100 */
void daNpc_Yw1_c::chngAnmAtr(u8 atr) {
    WWHD_FUNC(0x022FE100, void, this, atr);
    if (atr == mAnmAtr || atr >= 7) {
        return;
    }
    mAnmAtr = atr;
    setAnm_ATR();
}
VERIFY(0x022FE100, &daNpc_Yw1_c::chngAnmAtr);

/* 022FE11C */
void daNpc_Yw1_c::anmAtr(u16 msgStatus) {
    WWHD_FUNC(0x022FE11C, void, this, msgStatus);
    if (msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (mAtrCnt == 0) {
            chngAnmAtr(gabi::load<u8>(dComIfGp_ea() + 0x5BC5)); /* dComIfGp_getMesgAnimeAttrInfo */
            mAtrCnt = (s8)(mAtrCnt + 1);
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo */
        if (tag != 0xFF && tag != mAnmTag) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF); /* dComIfGp_clearMesgAnimeTagInfo */
            mAnmTag = tag;
            /* chngAnmTag(): empty */
        }
    } else if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        mAtrCnt = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x022FE11C, &daNpc_Yw1_c::anmAtr);

/* 022FE1EC */
u16 daNpc_Yw1_c::next_msgStatus(u32* pMsgNo) {
    WWHD_FUNC(0x022FE1EC, u16, this, pMsgNo);
    be<u32>* msgNo = (be<u32>*)pMsgNo;
    if (*msgNo != 0x8A3) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        *msgNo = 0x8A4;
        return 0xF;
    }
    *msgNo = dKy_daynight_check() ? 0x8A5 : 0x8A6;
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}
VERIFY(0x022FE1EC, &daNpc_Yw1_c::next_msgStatus);

/* 022FE28C */
u32 daNpc_Yw1_c::getMsg_YW1_0() {
    WWHD_FUNC(0x022FE28C, u32, this);
    if (m8E8 != 0) {
        return 0x8A0;
    }
    if (mbTsuboLost != 0) {
        return 0x89F;
    }
    if (dComIfGs_isEventBit(0x1)) {
        return dComIfGs_isEventBit(0x140) ? 0x89C : 0x89B;
    }
    if (mPathRun.mPath.get() != nullptr && mbPathEnd != 0) {
        return mPathRun.mbDir == 1 ? 0x8A2 : 0x8A1;
    }
    if (dComIfGs_isEventBit(0x180)) {
        return 0x89A;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x8AA : 0x899;
}
VERIFY(0x022FE28C, &daNpc_Yw1_c::getMsg_YW1_0);

/* 022FE3FC */
u32 daNpc_Yw1_c::getMsg_YW1_1() {
    WWHD_FUNC(0x022FE3FC, u32, this);
    if (dComIfGs_isEventBit(0xE20)) {
        return dComIfGs_isEventBit(0x120) ? 0x89E : 0x89D;
    }
    return dComIfGs_isEventBit(0x140) ? 0x89C : 0x89B;
}
VERIFY(0x022FE3FC, &daNpc_Yw1_c::getMsg_YW1_1);

/* 022FE48C */
u32 daNpc_Yw1_c::getMsg_YW1_2() {
    WWHD_FUNC(0x022FE48C, u32, this);
    if (!dComIfGs_isEventBit(0x3A40)) {
        return 0x8A3;
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        return 0x8A7;
    }
    return dKy_daynight_check() ? 0x8A8 : 0x8A9;
}
VERIFY(0x022FE48C, &daNpc_Yw1_c::getMsg_YW1_2);

/* 022FE52C */
u32 daNpc_Yw1_c::getMsg_YW1_3() {
    WWHD_FUNC(0x022FE52C, u32, this);
    if (m8E8 != 0) {
        return 0x8A0;
    }
    if (mbTsuboLost != 0) {
        return 0x89F;
    }
    if (mPathRun.mPath.get() != nullptr && mbPathEnd != 0) {
        return mPathRun.mbDir == 1 ? 0x8A2 : 0x8A1;
    }
    return getMsg_YW1_2();
}
VERIFY(0x022FE52C, &daNpc_Yw1_c::getMsg_YW1_3);

/* 022FE588 */
u32 daNpc_Yw1_c::getMsg() {
    WWHD_FUNC(0x022FE588, u32, this);
    switch (mType) {
    case 0:
        return getMsg_YW1_0();
    case 1:
        return getMsg_YW1_1();
    case 2:
        return getMsg_YW1_2();
    case 3:
        return getMsg_YW1_3();
    default:
        return 0;
    }
}
VERIFY(0x022FE588, &daNpc_Yw1_c::getMsg);

/* 022FE5EC */
BOOL daNpc_Yw1_c::chk_talk() {
    WWHD_FUNC(0x022FE5EC, BOOL, this);
    u8 talkXY = gabi::load<u8>(dComIfGp_ea() + 0x52B0);
    if ((u32)(talkXY - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mPreItemNo = gabi::load<u8>(dComIfGp_ea() + 0x52B1); /* dComIfGp_event_getPreItemNo() */
            return TRUE;
        }
        return FALSE;
    }
    mPreItemNo = 0xFF;
    return TRUE;
}
VERIFY(0x022FE5EC, &daNpc_Yw1_c::chk_talk);

/* 022FE684 */
u8 daNpc_Yw1_c::chk_parts_notMov() {
    WWHD_FUNC(0x022FE684, u8, this);
    return mSaveHeadY == m_jnt.mAngles[0][1] && mSaveBboneY == m_jnt.mAngles[1][1] && mSaveAngleY == current.angle.y;
}
VERIFY(0x022FE684, &daNpc_Yw1_c::chk_parts_notMov);

/* 022FE6C4 */
BOOL daNpc_Yw1_c::chkAttention() {
    WWHD_FUNC(0x022FE6C4, BOOL, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    } else {
        return this == dAttention_ActionTarget(attention, 0);
    }
}
VERIFY(0x022FE6C4, &daNpc_Yw1_c::chkAttention);

/* 022FE74C */
void daNpc_Yw1_c::chngTsuboAnm() {
    WWHD_FUNC(0x022FE74C, void, this);
    if (mbTsubo == 0) {
        return;
    }
    if (mbTsuboLost == 0) {
        switch (mAnmNo) {
        case 4:
            setAnm_NUM(3, 1);
            break;
        case 5:
            setAnm_NUM(1, 1);
            break;
        }
        return;
    }
    switch (mAnmNo) {
    case 1:
        setAnm_NUM(5, 1);
        break;
    case 3:
        setAnm_NUM(4, 1);
        break;
    }
    mbTsubo = 0;
}
VERIFY(0x022FE74C, &daNpc_Yw1_c::chngTsuboAnm);

/* 022FE850 */
void daNpc_Yw1_c::setStt(s8 stt) {
    WWHD_FUNC(0x022FE850, void, this, stt);
    s8 prev = mStt;
    mStt = stt;
    switch (stt) {
    case 2:
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        mAnmTag = 0xFF;
        mAtrCnt = 0;
        break;
    case 3:
    case 5:
        mbHeadOnly = 1;
        break;
    }
    setAnm();
    chngTsuboAnm();
}
VERIFY(0x022FE850, &daNpc_Yw1_c::setStt);

/* 022FE8F8 */
BOOL daNpc_Yw1_c::chk_areaIN(f32 r, f32 h, s16 ang, cXyz* pos) {
    WWHD_FUNC(0x022FE8F8, BOOL, this, r, h, ang, pos);
    gabi::Local<cXyz> d;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, d, pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = (f32)d->x;
    xz->z = (f32)d->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    f32 dy = dComIfGp_getLinkPlayer()->current.pos.y - pos->y;
    s16 a = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos) - current.angle.y);
    if (dist < r && std::fabs(dy) < h && std::abs((s32)a) < ang) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022FE8F8, &daNpc_Yw1_c::chk_areaIN);

/* 022FEA7C */
u8 daNpc_Yw1_c::chk_brkTsubo() {
    WWHD_FUNC(0x022FEA7C, u8, this);
    if (mbTsuboLost == 0) {
        return 0;
    }
    setStt(4);
    u8 ret = mbTsuboLost;
    mbHeadOnly = 0;
    mLookMode = 0;
    speedF = 0.0f;
    return ret;
}
VERIFY(0x022FEA7C, &daNpc_Yw1_c::chk_brkTsubo);

/* 022FEAD8 */
u8 daNpc_Yw1_c::chk_bm1Odoroki() {
    WWHD_FUNC(0x022FEAD8, u8, this);
    fopAc_ac_c* bm1 = searchByID(mBm1Id, nullptr);
    if (bm1 == nullptr) {
        return 0;
    }
    return gabi::load<u8>(gabi::ea(bm1) + 0x9F0); /* daNpc_Bm1_c: the cat is startled */
}
VERIFY(0x022FEAD8, &daNpc_Yw1_c::chk_bm1Odoroki);

/* 022FEB10 */
BOOL daNpc_Yw1_c::wait_1() {
    WWHD_FUNC(0x022FEB10, BOOL, this);
    if (chk_brkTsubo()) {
        return TRUE;
    }
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (chk_bm1Odoroki()) {
        mLookMode = 4;
        mLookActorId = mBm1Id;
        return TRUE;
    }
    mOrderType = 2;
    chngTsuboAnm();
    gabi::Local<cXyz> pos;
    pos->x = (f32)current.pos.x;
    pos->y = (f32)current.pos.y;
    pos->z = (f32)current.pos.z;
    if (chk_areaIN(l_child(mHioNo).mAreaRadius + 100.0f, 100.0f, 0x4400, pos)) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
        mLookMode = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mWaitTimer) != 0) {
        mLookMode = 1;
        return TRUE;
    }
    setStt(3);
    mLookMode = 0;
    mbHeadOnly = 1;
    return TRUE;
}
VERIFY(0x022FEB10, &daNpc_Yw1_c::wait_1);

/* 022FEC58 */
BOOL daNpc_Yw1_c::wait_2() {
    WWHD_FUNC(0x022FEC58, BOOL, this);
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (chk_bm1Odoroki()) {
        mbHeadOnly = 0;
        mLookActorId = mBm1Id;
        mLookMode = 4;
        m_jnt.mbTrn = 1;
        return TRUE;
    }
    mOrderType = 2;
    mbHeadOnly = 1;
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    if (mbAttention != 0) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
    }
    mLookMode = cLib_calcTimer(&mWaitTimer) != 0 ? 1 : 0;
    return TRUE;
}
VERIFY(0x022FEC58, &daNpc_Yw1_c::wait_2);

/* 022FED54 */
BOOL daNpc_Yw1_c::wait_3() {
    WWHD_FUNC(0x022FED54, BOOL, this);
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
            mLookMode = 1;
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mOrderType = 2;
    mbHeadOnly = 1;
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    if (mbAttention != 0) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
    }
    mLookMode = cLib_calcTimer(&mWaitTimer) != 0 ? 1 : 0;
    return TRUE;
}
VERIFY(0x022FED54, &daNpc_Yw1_c::wait_3);

/* 022FEE28 */
BOOL daNpc_Yw1_c::walk_1() {
    WWHD_FUNC(0x022FEE28, BOOL, this);
    if (chk_brkTsubo()) {
        return TRUE;
    }
    f32 target;
    bool walk = false;
    if (mbPathEnd == 0) {
        gabi::Local<cXyz> pos;
        pos->x = (f32)current.pos.x;
        pos->y = (f32)current.pos.y;
        pos->z = (f32)current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
            mbPathEnd = (u8)(dNpc_PathRun_nextIdx(&mPathRun) ^ 1);
            walk = mbPathEnd == 0;
        } else {
            walk = mbPathEnd == 0;
        }
    }
    if (walk) {
        gabi::Local<cXyz> pos;
        pos->x = (f32)current.pos.x;
        pos->y = (f32)current.pos.y;
        pos->z = (f32)current.pos.z;
        if (chk_areaIN(l_child(mHioNo).mAreaRadius, 100.0f, 0x4000, pos) || mbTalk != 0 || chk_bm1Odoroki()) {
            walk = false;
        }
    }
    if (walk) {
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        gabi::Local<cXyz> next;
        next->copy(*pnt);
        s16 ang = cLib_targetAngleY(&current.pos, next);
        cLib_addCalcAngleS(&current.angle.y, ang, l_child(mHioNo).mTurnScale, l_child(mHioNo).mTurnStep, 0);
        target = l_child(mHioNo).mWalkSpd;
    } else {
        target = 0.0f;
    }
    cLib_chaseF(&speedF, target, l_child(mHioNo).mWalkAccel);
    f32 rate = speedF * l_child(mHioNo).mWalkAnmRate;
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f); /* fsel */
    if (gabi::ftoi(target) != 0 || gabi::ftoi(speedF) != 0) {
        mOrderType = 2;
        mLookMode = 0;
        return TRUE;
    }
    speedF = 0.0f;
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (mbPathEnd != 0) {
        setStt(5);
        mLookMode = 0;
        return TRUE;
    }
    setStt(1);
    mbHeadOnly = 0;
    m_jnt.mbTrn = 1;
    mLookMode = 1;
    return TRUE;
}
VERIFY(0x022FEE28, &daNpc_Yw1_c::walk_1);

/* 022FF190 */
BOOL daNpc_Yw1_c::turn_1() {
    WWHD_FUNC(0x022FF190, BOOL, this);
    if (chk_brkTsubo()) {
        return TRUE;
    }
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    mOrderType = 2;
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    gabi::Local<cXyz> next;
    next->copy(*pnt);
    s16 ang = cLib_targetAngleY(&current.pos, next);
    cLib_chaseAngleS(&current.angle.y, ang, l_child(mHioNo).mTurnStep);
    if (current.angle.y == ang) {
        setStt(3);
        mbPathEnd = 0;
        mLookMode = 0;
        mPathRun.mbDir = (u8)(mPathRun.mbDir ^ 1);
    }
    return TRUE;
}
VERIFY(0x022FF190, &daNpc_Yw1_c::turn_1);

/* 022FF290 */
BOOL daNpc_Yw1_c::talk_1() {
    WWHD_FUNC(0x022FF290, BOOL, this);
    u8 notMove = chk_parts_notMov();
    talk(1);
    if (mbCurrMsg != 0 && fopMsgM_SearchByID(l_msgMng()) == 0x13 /* fopMsgStts_BOX_CLOSED_e */) {
        u32 msgNo = mCurrMsgNo;
        switch (msgNo) {
        case 0x899:
        case 0x8AA:
            dComIfGs_onEventBit(0x180);
            break;
        case 0x89B:
            dComIfGs_onEventBit(0x140);
            break;
        case 0x89D:
            dComIfGs_onEventBit(0x120);
            break;
        case 0x89F:
            dComIfGp_setItemRupeeCount(-10);
            mbTalk = 0;
            m8E8 = 1;
            break;
        case 0x8A4:
        case 0x8A5:
        case 0x8A6:
            dComIfGs_onEventBit(0x3A40);
            break;
        }
        s8 prev = mPrevStt;
        mPreItemNo = 0xFF;
        mbTalk = 0;
        setStt(prev);
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
        endEvent();
    }
    return notMove;
}
VERIFY(0x022FF290, &daNpc_Yw1_c::talk_1);

/* 022FF4FC */
BOOL daNpc_Yw1_c::wait_action1(void*) {
    WWHD_FUNC(0x022FF4FC, BOOL, this, (u32)0);
    s8 step = mActStep;
    if (step == 0) {
        setStt(3);
        mActStep = (s8)(mActStep + 1);
    } else if ((u32)(s32)step <= 3) {
        mbAttention = chkAttention();
        switch (mStt) {
        case 1:
            mActRet = wait_1();
            break;
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
            mActRet = turn_1();
            break;
        }
        lookBack();
    }
    if (dComIfGs_isEventBit(0x1) && mType == 0) {
        gabi::Local<be<s32>> err;
        fopAc_ac_c* tsubo = searchByID(mTsuboId, err);
        if (*err == 0 && tsubo != nullptr) {
            fopAcM_delete(tsubo);
        }
        fopAcM_delete(this);
    }
    return TRUE;
}
VERIFY(0x022FF4FC, &daNpc_Yw1_c::wait_action1);

/* 022FF698 */
BOOL daNpc_Yw1_c::wait_action2(void*) {
    WWHD_FUNC(0x022FF698, BOOL, this, (u32)0);
    if (mActStep == 0) {
        setStt(6);
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
    case 6:
        mActRet = wait_3();
        break;
    }
    lookBack();
    return TRUE;
}
VERIFY(0x022FF698, &daNpc_Yw1_c::wait_action2);

/* 022FF748: daNpc_Yw1_childHIO_c::daNpc_Yw1_childHIO_c (HD: allocates when this == NULL) */
static daNpc_Yw1_childHIO_c* daNpc_Yw1_childHIO_ct(daNpc_Yw1_childHIO_c* p) {
    WWHD_FUNC(0x022FF748, daNpc_Yw1_childHIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Yw1_childHIO_c*)operator_new(0x38);
        if (p == nullptr) return nullptr;
    }
    p->__vtbl = CHILDHIO_VTBL;
    return p;
}
VERIFY(0x022FF748, daNpc_Yw1_childHIO_ct);

/* 022FF788: daNpc_Yw1_HIO_c::daNpc_Yw1_HIO_c */
static daNpc_Yw1_HIO_c* daNpc_Yw1_HIO_ct(daNpc_Yw1_HIO_c* p) {
    WWHD_FUNC(0x022FF788, daNpc_Yw1_HIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Yw1_HIO_c*)operator_new(0x44);
        if (p == nullptr) return nullptr;
    }
    p->__vtbl = HIO_VTBL;
    __construct_array(gabi::ea(p) + 0xC, 1, 0x38, 0x022FF748);
    p->mChild[0].field_0x34 = 0;
    memcpy_l(gabi::ea(p) + 0x10, 0x101C6F2C /* a_prm_tbl */, 0x30);
    p->field_0x8 = -1;
    p->mNo = -1;
    return p;
}
VERIFY(0x022FF788, daNpc_Yw1_HIO_ct);

/* 022FF814 */
static void __sinit_d_a_npc_yw1_cpp() {
    WWHD_FUNC(0x022FF814, void, (u32)0);
    /* header statics (this TU: P = 104689E4, objects at P+8 / P+9) */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104689F0 + 4 * i, 0);
    __register_global_object(0x101C6F5C);
    gabi::store<f32>(0x104689E8, 3.1415927f);
    gabi::store<f32>(0x104689E4, -3.1415927f);
    gabi::call(0x028ED6F8, 0x104689ECu);
    __register_global_object(0x101C6F68);
    gabi::call(0x028EAB2C, 0x104689EDu);
    __register_global_object(0x101C6F74);
    daNpc_Yw1_HIO_ct(&l_HIO); /* static daNpc_Yw1_HIO_c l_HIO */
    /* dynamically initialised statics: cXyz (0, 1, 0) (upLift), hair joint offsets (setHairAngle) */
    gabi::store<f32>(0x101C6DB4, 15.0f);
    gabi::store<f32>(0x10468A18, 0.0f);
    gabi::store<f32>(0x10468A1C, 1.0f);
    gabi::store<f32>(0x10468A20, 0.0f);
    gabi::store<f32>(0x101C6DC0, 15.0f);
    gabi::store<f32>(0x101C6DB8, 10.0f);
}
VERIFY(0x022FF814, __sinit_d_a_npc_yw1_cpp);

/* 022FF8F8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022FF8F8, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x022FF8F8, SafeString_dt);

/* 022FF90C: daNpc_Yw1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Yw1_dt(daNpc_Yw1_c* p, s32 flags) {
    WWHD_FUNC(0x022FF90C, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x10023708);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x10023718);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x022FF90C, daNpc_Yw1_dt);

/* 022FF9A8: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022FF9A8, void, (u32)0);
}
VERIFY(0x022FF9A8, SafeString_assureTerminationImpl);
