/**
 * d_a_npc_ko1.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part A: HIO, search callbacks, joint callbacks, heap, init_*, eye animation.
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1.h"

enum { fpcNm_NPC_KO1_e = 0x13F, fpcNm_NPC_KO2_e = 0x140, fpcNm_NPC_OB1_e = 0x14B };

/* ---- file statics ---- */
static be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x104677B0); }
static gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x104678A8); } /* [20] */

/* 02273170 */
static void* searchActor_Ko_Hna(void* i_actor, void*) {
    WWHD_FUNC(0x02273170, void*, i_actor, (void*)nullptr);
    if (l_check_wrk() < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_KO1_e) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}
VERIFY(0x02273170, searchActor_Ko_Hna);

/* 022731F0 */
static void* searchActor_Ko_Bou(void* i_actor, void*) {
    WWHD_FUNC(0x022731F0, void*, i_actor, (void*)nullptr);
    if (l_check_wrk() < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_KO2_e) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}
VERIFY(0x022731F0, searchActor_Ko_Bou);

/* 02273270 */
static void* searchActor_Ob(void* i_actor, void*) {
    WWHD_FUNC(0x02273270, void*, i_actor, (void*)nullptr);
    if (l_check_wrk() < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_OB1_e) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}
VERIFY(0x02273270, searchActor_Ob);

/* 022732F0 */
/* head model (mpMorf824): keeps the head joint's matrix for the balloon (nodeBlnControl) */
void daNpc_Ko1_c::nodeHedControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022732F0, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m7E7) {
        PSMTXCopy(mDoMtx_stack_c::get(), &m7EC);
    }
}
VERIFY(0x022732F0, &daNpc_Ko1_c::nodeHedControl);

/* 0227337C */
static BOOL nodeCallBack_Hed(J3DNode* i_node, int i_timing) {
    WWHD_FUNC(0x0227337C, BOOL, i_node, i_timing);
    if (i_timing == 0) {
        daNpc_Ko1_c* user = gabi::at<daNpc_Ko1_c>(gabi::load<u32>(gabi::load<u32>(0x104B462C) + 0xB8));
        if (user != nullptr) {
            user->nodeHedControl(i_node, j3dSys_mModel());
        }
    }
    return TRUE;
}
VERIFY(0x0227337C, nodeCallBack_Hed);

/* 022733C4 */
/* balloon model (mpMorf81C): its joint m7E8 takes the head joint's matrix */
void daNpc_Ko1_c::nodeBlnControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022733C4, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m7E8) {
        PSMTXCopy(&m7EC, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, jntNo), &m7EC);
    }
}
VERIFY(0x022733C4, &daNpc_Ko1_c::nodeBlnControl);

/* 022734D4 */
static BOOL nodeCallBack_Bln(J3DNode* i_node, int i_timing) {
    WWHD_FUNC(0x022734D4, BOOL, i_node, i_timing);
    if (i_timing == 0) {
        daNpc_Ko1_c* user = gabi::at<daNpc_Ko1_c>(gabi::load<u32>(gabi::load<u32>(0x104B462C) + 0xB8));
        if (user != nullptr) {
            user->nodeBlnControl(i_node, j3dSys_mModel());
        }
    }
    return TRUE;
}
VERIFY(0x022734D4, nodeCallBack_Bln);

/* 0227351C */
void daNpc_Ko1_c::nodeKo1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227351C, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(18.0f, 20.0f, 0.0f): guard 0x104678F8, object 0x104677D0 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x104677D0);
    if (gabi::load<u32>(0x104678F8) == 0) {
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 20.0f;
        a_eye_pos_off->x = 18.0f;
        gabi::store<u32>(0x104678F8, 1);
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    if (jntNo == (u32)(s32)m7E4) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
        PSMTXMultVec(stk, a_eye_pos_off, &m94C);
    }
    if (jntNo == (u32)(s32)m7E5) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
        mDoMtx_YrotM(stk, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x0227351C, &daNpc_Ko1_c::nodeKo1Control);

/* 022736B4 */
static BOOL nodeCallBack_Ko1(J3DNode* i_node, int i_timing) {
    WWHD_FUNC(0x022736B4, BOOL, i_node, i_timing);
    if (i_timing == 0) {
        daNpc_Ko1_c* user = gabi::at<daNpc_Ko1_c>(gabi::load<u32>(gabi::load<u32>(0x104B462C) + 0xB8));
        if (user != nullptr) {
            user->nodeKo1Control(i_node, j3dSys_mModel());
        }
    }
    return TRUE;
}
VERIFY(0x022736B4, nodeCallBack_Ko1);

/* 02273B08 */
s32 daNpc_Ko1_c::btpNum_toResID(s32 i_btpNum) {
    WWHD_FUNC(0x02273B08, s32, this, i_btpNum);
    /* a_btp_resID_tbl (.data 0x1001CA7C); the head eye textures 0x1F/0x20 depend on mType */
    s32 resID = gabi::load<s32>(0x1001CA7C + i_btpNum * 4);
    if (resID == 0x1F) {
        switch (mType) {
        case 0:
            return 0x1F;
        case 1:
            return 0x23;
        }
    } else if (resID == 0x20) {
        switch (mType) {
        case 0:
            return 0x20;
        case 1:
            return 0x24;
        }
    }
    return resID;
}
VERIFY(0x02273B08, &daNpc_Ko1_c::btpNum_toResID);

/* 02273B80 */
/* i_modify (GameCube bool) is passed on unnormalised: typed u32 */
bool daNpc_Ko1_c::setBtp(u32 i_modify, s32 i_btpNum) {
    WWHD_FUNC(0x02273B80, bool, this, i_modify, i_btpNum);
    mDoExt_McaMorf* morf = mpMorf824;
    J3DModelData* modelData = J3DModel_getModelData_l(morf->getModel());
    s32 resID = btpNum_toResID(i_btpNum);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x1001CA8C), resID);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(942, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x1001CA90), 0x3AE, STR(0x1001CAA0));
    if (mDoExt_btpAnm_init(mBtpAnm, modelData, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_modify, 0)) {
        mBlinkFrame = 0;
        mBlinkTimer = 0;
        return true;
    }
    return false;
}
VERIFY(0x02273B80, &daNpc_Ko1_c::setBtp);

/* 02273C88 */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Ko1_c::iniTexPttrnAnm(u32 i_modify) {
    WWHD_FUNC(0x02273C88, u32, this, i_modify);
    return gabi::call<u32>(0x02273B80, this, i_modify, (s32)mBtpNum); /* setBtp(i_modify, mBtpNum) */
}
VERIFY(0x02273C88, &daNpc_Ko1_c::iniTexPttrnAnm);

/* a morf whose model could not be created is deleted (virtual deleting destructor, slot +0xC) */
static inline void morf_delete(mDoExt_McaMorf* morf) {
    u32 vt = gabi::load<u32>(gabi::ea(morf));
    gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
}

/* 022736FC */
J3DModelData* daNpc_Ko1_c::create_Anm() {
    WWHD_FUNC(0x022736FC, J3DModelData*, this);
    /* a_bdl_resID_tbl (.data 0x101BF790), by mType */
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001C9B4), gabi::load<s32>(0x101BF790 + mType * 4));
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4291, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001C9C0), 0x10C3, STR(0x1001C9D0));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001C9B4), 0x16);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020203);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            morf_delete(morf);
        mpMorf = nullptr;
        return nullptr;
    }
    m7E4 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001C9B8) /* "head" */);
    if (m7E4 < 0) /* JUT_ASSERT(4317, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001C9C0), 0x10DD, STR(0x1001C9E4));
    m7E5 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001C9F8) /* "backbone" */);
    if (m7E5 < 0)
        JUT_ASSERT_fail(STR(0x1001C9C0), 0x10E0, STR(0x1001CA04));
    m7E6 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001C9AC) /* "armR2" */);
    if (m7E6 < 0)
        JUT_ASSERT_fail(STR(0x1001C9C0), 0x10E3, STR(0x1001CA1C));
    return a_mdl_dat;
}
VERIFY(0x022736FC, &daNpc_Ko1_c::create_Anm);

/* 02273944 */
J3DModelData* daNpc_Ko1_c::create_hed_Anm() {
    WWHD_FUNC(0x02273944, J3DModelData*, this);
    /* head model and animation by mType (.data 0x101BF798 / 0x101BF7A0) */
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CA3C), gabi::load<s32>(0x101BF798 + mType * 4));
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4358, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001CA40), 0x1106, STR(0x1001CA50));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001CA3C), gabi::load<s32>(0x101BF7A0 + mType * 4));
    mpMorf824 = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf824;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            morf_delete(morf);
        mpMorf824 = nullptr;
        return nullptr;
    }
    if (mType == 0) {
        m7E7 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001CA34) /* "head2" */);
        if (m7E7 < 0)
            JUT_ASSERT_fail(STR(0x1001CA40), 0x1120, STR(0x1001CA64));
    }
    return a_mdl_dat;
}
VERIFY(0x02273944, &daNpc_Ko1_c::create_hed_Anm);

/* 02273C94 */
J3DModelData* daNpc_Ko1_c::create_bln_Anm() {
    WWHD_FUNC(0x02273C94, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CAB8), 0x1A);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4400, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001CABC), 0x1130, STR(0x1001CAD8));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001CAB8), 1);
    mpMorf81C = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11000022);
    mDoExt_McaMorf* morf = mpMorf81C;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            morf_delete(morf);
        mpMorf81C = nullptr;
        return nullptr;
    }
    m7E8 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001CACC));
    if (m7E8 < 0)
        JUT_ASSERT_fail(STR(0x1001CABC), 0x1146, STR(0x1001CAEC));
    m7E9 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001CB04));
    if (m7E9 < 0)
        JUT_ASSERT_fail(STR(0x1001CABC), 0x1149, STR(0x1001CB10));
    return a_mdl_dat;
}
VERIFY(0x02273C94, &daNpc_Ko1_c::create_bln_Anm);

/* 02273E7C */
bool daNpc_Ko1_c::create_itm_Mdl() {
    WWHD_FUNC(0x02273E7C, bool, this);
    if (mType == 1) {
        J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CB24), 0x19);
        if (a_mdl_dat == nullptr) /* JUT_ASSERT(4451, a_mdl_dat != NULL) */
            JUT_ASSERT_fail(STR(0x1001CB28), 0x1163, STR(0x1001CB38));
        mpModel820 = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
        if (mpModel820.get() == nullptr) {
            return false;
        }
    }
    return true;
}
VERIFY(0x02273E7C, &daNpc_Ko1_c::create_itm_Mdl);

/* modelData->getJointNodePointer(i)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */
static inline void setJointCallBack(J3DModel* model, u32 i, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(model);
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (i < n)
        joint += i * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 02273F48 */
bool daNpc_Ko1_c::CreateHeap() {
    WWHD_FUNC(0x02273F48, bool, this);
    u32 sp_at_entry = gabi::cpu->r[1];
    J3DModelData* anm_model = create_Anm();
    if (anm_model == nullptr) {
        return false;
    }
    J3DModelData* hed_model = create_hed_Anm();
    if (hed_model == nullptr) {
        mpMorf = nullptr;
        return false;
    }
    /* s8 a_btp_num[2] = {0, 0} (a stack array), indexed by mType. The array sits at the
     * original's frame offset (entry SP - 0x1C) so that an out-of-range mType reads the same
     * neighbouring stack bytes on both sides (harness exactness, not behaviour). */
    u32 a_btp_num = sp_at_entry - 0x1C;
    gabi::store<u8>(a_btp_num, 0);
    gabi::store<u8>(a_btp_num + 1, 0);
    mBtpNum = gabi::load<s8>(a_btp_num + mType);
    if (!iniTexPttrnAnm(false) || (mType == 0 && !create_bln_Anm())) {
        mpMorf824 = nullptr;
        mpMorf = nullptr;
        return false;
    }
    if (!create_itm_Mdl()) {
        mpMorf = nullptr;
        mpMorf81C = nullptr;
        mpMorf824 = nullptr;
        return false;
    }
    if (mType == 0) {
        /* the balloon joint follows the head (nodeBlnControl / nodeHedControl) */
        for (u16 i = 0; i < J3DModelData_getJointNum(hed_model); i++) {
            if (i == (u32)(s32)m7E8)
                setJointCallBack(mpMorf81C->getModel(), i, 0x022734D4 /* nodeCallBack_Bln */);
        }
        gabi::store<u32>(gabi::ea(mpMorf81C->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
        for (u16 i = 0; i < J3DModelData_getJointNum(hed_model); i++) {
            if (i == (u32)(s32)m7E7)
                setJointCallBack(mpMorf824->getModel(), i, 0x0227337C /* nodeCallBack_Hed */);
        }
        gabi::store<u32>(gabi::ea(mpMorf824->getModel()) + 0xB8, gabi::ea(this));
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
        if (i == (u32)(s32)m7E4 || i == (u32)(s32)m7E5)
            setJointCallBack(mpMorf->getModel(), i, 0x022736B4 /* nodeCallBack_Ko1 */);
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this));
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return true;
}
VERIFY(0x02273F48, &daNpc_Ko1_c::CreateHeap);

/* 0227424C */
/* tail call: CreateHeap's result register is passed through (typed u32) */
static u32 CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0227424C, u32, i_this);
    return gabi::call<u32>(0x02273F48, i_this); /* static_cast<daNpc_Ko1_c*>(i_this)->CreateHeap() */
}
VERIFY(0x0227424C, CheckCreateHeap);

/* 02274250 */
bool daNpc_Ko1_c::charDecide(int i_prm) {
    WWHD_FUNC(0x02274250, bool, this, i_prm);
    s16 name = fpcM_GetName(this);
    mType = -1;
    mSpecificType = -1;
    if (name == fpcNm_NPC_KO1_e) {
        mType = 0; /* Joel */
        if ((u32)i_prm <= 4) {
            mSpecificType = (s8)i_prm;
            return true;
        }
    } else if (name == fpcNm_NPC_KO2_e) {
        mType = 1; /* Zill */
        if ((u32)i_prm <= 3) {
            mSpecificType = gabi::load<s8>(0x1001CB50 + i_prm); /* a_specificType_tbl */
            return true;
        }
    } else {
        return false;
    }
    return false;
}
VERIFY(0x02274250, &daNpc_Ko1_c::charDecide);

/* 022742C0 */
BOOL daNpc_Ko1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x022742C0, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if (cur->i == newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if (cur->d == newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        mA18 = 9; /* leaving the current action */
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mA18 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x022742C0, &daNpc_Ko1_c::set_action);

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 {
    PMF_hana_action1 = 0x1001C820,
    PMF_hana_action2 = 0x1001C828,
    PMF_hana_action3 = 0x1001C830,
    PMF_hana_action4 = 0x1001C838,
    PMF_hana_action5 = 0x1001C840,
    PMF_wait_action1 = 0x1001C848,
    PMF_wait_action2 = 0x1001C850,
    PMF_wait_action3 = 0x1001C858,
    PMF_wait_action4 = 0x1001C860,
};
static inline void set_action_pmf(daNpc_Ko1_c* i_this, u32 pmf_addr) {
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, pmf_addr);
    i_this->set_action(pmf, nullptr);
}
enum {
    dSv_event_flag_UNK_0520 = 0x0520,
    dSv_event_flag_UNK_0E20 = 0x0E20,
    dSv_event_flag_UNK_2A80 = 0x2A80,
};

/* 022743EC */
bool daNpc_Ko1_c::init_HNA_0() {
    WWHD_FUNC(0x022743EC, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80) && !dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        mpMorf81C = nullptr;
        set_action_pmf(this, PMF_hana_action1);
        return true;
    }
    return false;
}
VERIFY(0x022743EC, &daNpc_Ko1_c::init_HNA_0);

/* 02274498 */
bool daNpc_Ko1_c::init_HNA_1() {
    WWHD_FUNC(0x02274498, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        mpMorf81C = nullptr;
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        set_action_pmf(this, PMF_hana_action2);
        return true;
    }
    return false;
}
VERIFY(0x02274498, &daNpc_Ko1_c::init_HNA_1);

/* 0227452C */
bool daNpc_Ko1_c::init_HNA_2() {
    WWHD_FUNC(0x0227452C, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
        mpMorf81C = nullptr;
        set_action_pmf(this, PMF_hana_action3);
        return true;
    }
    return false;
}
VERIFY(0x0227452C, &daNpc_Ko1_c::init_HNA_2);

/* 022745B4 */
bool daNpc_Ko1_c::init_HNA_3() {
    WWHD_FUNC(0x022745B4, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        mpMorf81C = nullptr;
        actor_status = (actor_status & ~0x80u) | 0x4000; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
        set_action_pmf(this, PMF_hana_action4);
        return true;
    }
    return false;
}
VERIFY(0x022745B4, &daNpc_Ko1_c::init_HNA_3);

/* 02274658 */
bool daNpc_Ko1_c::init_HNA_4() {
    WWHD_FUNC(0x02274658, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, PMF_hana_action5);
        return true;
    }
    return false;
}
VERIFY(0x02274658, &daNpc_Ko1_c::init_HNA_4);

/* 022746E8 */
bool daNpc_Ko1_c::init_BOU_0() {
    WWHD_FUNC(0x022746E8, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, PMF_wait_action1);
        actor_status &= ~0x80u;
        return true;
    }
    return false;
}
VERIFY(0x022746E8, &daNpc_Ko1_c::init_BOU_0);

/* 02274778 */
bool daNpc_Ko1_c::init_BOU_1() {
    WWHD_FUNC(0x02274778, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, PMF_wait_action2);
        actor_status &= ~0x80u;
        return true;
    }
    return false;
}
VERIFY(0x02274778, &daNpc_Ko1_c::init_BOU_1);

/* 02274808 */
bool daNpc_Ko1_c::init_BOU_2() {
    WWHD_FUNC(0x02274808, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        actor_status = (actor_status & ~0x80u) | 0x4000;
        set_action_pmf(this, PMF_wait_action3);
        mpModel820 = nullptr;
        return true;
    }
    return false;
}
VERIFY(0x02274808, &daNpc_Ko1_c::init_BOU_2);

/* 022748B0 */
bool daNpc_Ko1_c::init_BOU_3() {
    WWHD_FUNC(0x022748B0, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, PMF_wait_action4);
        mpModel820 = nullptr;
        return true;
    }
    return false;
}
VERIFY(0x022748B0, &daNpc_Ko1_c::init_BOU_3);

/* 02274948 */
void daNpc_Ko1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x02274948, void, this);
    if (mBtpNum != 0 || !cLib_calcTimer(&mBlinkTimer)) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            if (mBtpNum != 0) {
                mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                mBlinkFrame = 0;
                mBlinkTimer = t;
            }
        }
    }
}
VERIFY(0x02274948, &daNpc_Ko1_c::plyTexPttrnAnm);

/* daNpc_Ko1_childHIO_c (one per mType, 0x60 bytes each, at 0x104677E8): attention offset at +0x18 */
static inline f32 l_HIO_attPosOffsetY(s32 type) { return gabi::load<f32>(0x10467800 + type * 0x60); }

/* 02274A1C */
void daNpc_Ko1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x02274A1C, void, this, i_setEyePos);
    f32 offset = l_HIO_attPosOffsetY(mType);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->z = current.pos.z;
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + offset;
    if (m9E0 == 0 && i_setEyePos == 0) {
        return;
    }
    eyePos.z = m94C.z;
    eyePos.y = m94C.y;
    eyePos.x = m94C.x;
}
VERIFY(0x02274A1C, &daNpc_Ko1_c::setAttention);
