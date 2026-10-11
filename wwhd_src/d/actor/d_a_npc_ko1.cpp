/**
 * d_a_npc_ko1.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part A: HIO, search callbacks, joint callbacks, heap, init_*, eye animation.
 */
#include "d/actor/d_a_npc_ko1_a.h"

enum { fpcNm_NPC_KO1_e = 0x13F, fpcNm_NPC_KO2_e = 0x140, fpcNm_NPC_OB1_e = 0x14B };

/* ---- file statics ---- */
// Part A owns these TU-wide objects; later part files use these guest addresses.
static s32 l_check_wrk;
static Ko1ActorList l_check_inf;
static daNpc_Ko1_HIO_c l_HIO;
static cXyz a_eye_pos_off;
static u32 eyeOffsetGuard;
static J3DModel* l_currentModel;
static Mtx34 l_currentMatrix;
static s32 a_btp_resID_tbl[4] = {0x1F, 0x22, 0x20, 0x21};
static s32 a_bdl_resID_tbl[2] = {0x1B, 0x1E};
static s32 a_hed_bdl_resID_tbl[2] = {0x1C, 0x1D};
static s32 a_hed_bck_resID_tbl[2] = {0xA, 0x25};
static s8 a_specificType_tbl[4] = {5, 6, 7, 8};

/* inlined into model creation */
static void morf_delete(mDoExt_McaMorf* morf) {
    // HD: failed model construction uses the virtual deleting destructor with flag 3.
    reinterpret_cast<Ko1MorfDeleteView*>(morf)->destroy(3);
}

/* inlined into daNpc_Ko1_c::CreateHeap */
static void setJointCallBack(J3DModel* model, u16 i, J3DJointCallBack cb) {
    model->getModelData()->getJointNodePointer(i)->setCallBack(cb);
}

// HD: GHS uses 8-byte member pointers; the packed constants preserve that ABI.
static ProcFunc_l pmf_hana_action1 = {0, -1, 0x0227A374};
static ProcFunc_l pmf_hana_action2 = {0, -1, 0x0227A4DC};
static ProcFunc_l pmf_hana_action3 = {0, -1, 0x0227A60C};
static ProcFunc_l pmf_hana_action4 = {0, -1, 0x0227A750};
static ProcFunc_l pmf_hana_action5 = {0, -1, 0x0227A830};
static ProcFunc_l pmf_wait_action1 = {0, -1, 0x0227A8C8};
static ProcFunc_l pmf_wait_action2 = {0, -1, 0x0227A9C0};
static ProcFunc_l pmf_wait_action3 = {0, -1, 0x0227AB28};
static ProcFunc_l pmf_wait_action4 = {0, -1, 0x0227AC08};
/* inlined into initial action selection */
static void set_action_pmf(daNpc_Ko1_c* i_this, const ProcFunc_l* entry) {
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, gabi::ea(entry));
    i_this->set_action(pmf, nullptr);
}
enum {
    dSv_event_flag_UNK_0520 = 0x0520,
    dSv_event_flag_UNK_0E20 = 0x0E20,
    dSv_event_flag_UNK_2A80 = 0x2A80,
};


/* 02273170 */
static void* searchActor_Ko_Hna(void* i_actor, void*) {
    if (l_check_wrk < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_KO1_e) {
        s32 n = l_check_wrk;
        l_check_wrk = n + 1;
        l_check_inf.mActors[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}

/* 022731F0 */
static void* searchActor_Ko_Bou(void* i_actor, void*) {
    if (l_check_wrk < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_KO2_e) {
        s32 n = l_check_wrk;
        l_check_wrk = n + 1;
        l_check_inf.mActors[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}

/* 02273270 */
static void* searchActor_Ob(void* i_actor, void*) {
    if (l_check_wrk < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_OB1_e) {
        s32 n = l_check_wrk;
        l_check_wrk = n + 1;
        l_check_inf.mActors[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}

/* 0227337C */
static BOOL nodeCallBack_Hed(J3DNode* i_node, int i_timing) {
    if (i_timing == 0) {
        daNpc_Ko1_c* user = static_cast<daNpc_Ko1_c*>(l_currentModel->getUserArea());
        if (user != nullptr) {
            user->nodeHedControl(i_node, l_currentModel);
        }
    }
    return TRUE;
}

/* 022732F0 */
/* head model (mpMorf824): keeps the head joint's matrix for the balloon (nodeBlnControl) */
void daNpc_Ko1_c::nodeHedControl(J3DNode* i_node, J3DModel* i_model) {
    u32 jntNo = jntNo_of(i_node);
    PSMTXCopy(i_model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m7E7) {
        PSMTXCopy(mDoMtx_stack_c::get(), &m7EC);
    }
}

/* 022734D4 */
static BOOL nodeCallBack_Bln(J3DNode* i_node, int i_timing) {
    if (i_timing == 0) {
        daNpc_Ko1_c* user = static_cast<daNpc_Ko1_c*>(l_currentModel->getUserArea());
        if (user != nullptr) {
            user->nodeBlnControl(i_node, l_currentModel);
        }
    }
    return TRUE;
}

/* 022733C4 */
/* balloon model (mpMorf81C): its joint m7E8 takes the head joint's matrix */
void daNpc_Ko1_c::nodeBlnControl(J3DNode* i_node, J3DModel* i_model) {
    u32 jntNo = jntNo_of(i_node);
    PSMTXCopy(i_model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m7E8) {
        PSMTXCopy(&m7EC, &l_currentMatrix);
        mtx_copy(i_model->getAnmMtx(jntNo), &m7EC);
    }
}

/* 022736B4 */
static BOOL nodeCallBack_Ko1(J3DNode* i_node, int i_timing) {
    if (i_timing == 0) {
        daNpc_Ko1_c* user = static_cast<daNpc_Ko1_c*>(l_currentModel->getUserArea());
        if (user != nullptr) {
            user->nodeKo1Control(i_node, l_currentModel);
        }
    }
    return TRUE;
}

/* 0227351C */
void daNpc_Ko1_c::nodeKo1Control(J3DNode* i_node, J3DModel* i_model) {
    // HD: initialize z, y, x before setting the GHS guard.
    if (eyeOffsetGuard == 0) {
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.y = 20.0f;
        a_eye_pos_off.x = 18.0f;
        eyeOffsetGuard = 1;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jntNo), stk);
    if (jntNo == (u32)(s32)m7E4) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
        PSMTXMultVec(stk, &a_eye_pos_off, &m94C);
    }
    if (jntNo == (u32)(s32)m7E5) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
        mDoMtx_YrotM(stk, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stk, &l_currentMatrix);
    mtx_copy(i_model->getAnmMtx(jntNo), stk);
}

/* 022743EC */
bool daNpc_Ko1_c::init_HNA_0() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80) && !dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        mpMorf81C = nullptr;
        set_action_pmf(this, &pmf_hana_action1);
        return true;
    }
    return false;
}

/* 02274498 */
bool daNpc_Ko1_c::init_HNA_1() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        mpMorf81C = nullptr;
        actor_status &= ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        set_action_pmf(this, &pmf_hana_action2);
        return true;
    }
    return false;
}

/* 0227452C */
bool daNpc_Ko1_c::init_HNA_2() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
        mpMorf81C = nullptr;
        set_action_pmf(this, &pmf_hana_action3);
        return true;
    }
    return false;
}

/* 022745B4 */
bool daNpc_Ko1_c::init_HNA_3() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        mpMorf81C = nullptr;
        actor_status = (actor_status & ~fopAcStts_NOCULLEXEC_e) | fopAcStts_UNK4000_e; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
        set_action_pmf(this, &pmf_hana_action4);
        return true;
    }
    return false;
}

/* 02274658 */
bool daNpc_Ko1_c::init_HNA_4() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, &pmf_hana_action5);
        return true;
    }
    return false;
}

/* 022746E8 */
bool daNpc_Ko1_c::init_BOU_0() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, &pmf_wait_action1);
        actor_status &= ~fopAcStts_NOCULLEXEC_e;
        return true;
    }
    return false;
}

/* 02274778 */
bool daNpc_Ko1_c::init_BOU_1() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        set_action_pmf(this, &pmf_wait_action2);
        actor_status &= ~fopAcStts_NOCULLEXEC_e;
        return true;
    }
    return false;
}

/* 02274808 */
bool daNpc_Ko1_c::init_BOU_2() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dKy_daynight_check()) {
        actor_status = (actor_status & ~fopAcStts_NOCULLEXEC_e) | fopAcStts_UNK4000_e;
        set_action_pmf(this, &pmf_wait_action3);
        mpModel820 = nullptr;
        return true;
    }
    return false;
}

/* 022748B0 */
bool daNpc_Ko1_c::init_BOU_3() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && dKy_daynight_check() == 1) {
        set_action_pmf(this, &pmf_wait_action4);
        mpModel820 = nullptr;
        return true;
    }
    return false;
}

/* 02273B08 */
s32 daNpc_Ko1_c::btpNum_toResID(s32 i_btpNum) {
    // The head eye textures depend on the child type.
    s32 resID = s32(a_btp_resID_tbl[i_btpNum]);
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

/* 02273B80 */
// HD: retain the unnormalized modify argument in its full register.
bool daNpc_Ko1_c::setBtp(u32 i_modify, s32 i_btpNum) {
    mDoExt_McaMorf* morf = mpMorf824;
    J3DModelData* modelData = morf->getModel()->getModelData();
    s32 resID = btpNum_toResID(i_btpNum);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)ko1ObjectRes("Ko", resID);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(942, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x3AE, "m_hed_tex_pttrn != (0)");
    if (mDoExt_btpAnm_init(mBtpAnm, modelData, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_modify, 0)) {
        mBlinkFrame = 0;
        mBlinkTimer = 0;
        return true;
    }
    return false;
}

/* 02273C88 */
// HD: the tail call preserves the full result register.
u32 daNpc_Ko1_c::iniTexPttrnAnm(u32 i_modify) {
    return setBtpRegister(this, i_modify, (s32)mBtpNum);
}

/* 02274948 */
void daNpc_Ko1_c::plyTexPttrnAnm() {
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

/* 02274A1C */
void daNpc_Ko1_c::setAttention(u32 i_setEyePos) {
    f32 offset = f32(l_HIO.mChild[mType].mAttentionOffsetY);
    cXyz* attPos = &attention_info.position;
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

/* 02274250 */
bool daNpc_Ko1_c::charDecide(int i_prm) {
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
            mSpecificType = s8(a_specificType_tbl[i_prm]); /* a_specificType_tbl */
            return true;
        }
    } else {
        return false;
    }
    return false;
}

/* 022742C0 */
BOOL daNpc_Ko1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
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

/* 0227424C */
// HD: the tail call preserves the full result register.
static u32 CheckCreateHeap(fopAc_ac_c* i_this) {
    return createHeapRegister(static_cast<daNpc_Ko1_c*>(i_this));
}

/* 022736FC */
J3DModelData* daNpc_Ko1_c::create_Anm() {
    // Select the body model for Joel or Zill.
    J3DModelData* a_mdl_dat = (J3DModelData*)ko1ObjectRes("Ko", s32(a_bdl_resID_tbl[mType]));
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4291, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x10C3, "a_mdl_dat != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)ko1ObjectRes("Ko", 0x16);
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
    m7E4 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head");
    if (m7E4 < 0) /* JUT_ASSERT(4317, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x10DD, "m_hed_jnt_num >= 0");
    m7E5 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "backbone");
    if (m7E5 < 0)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x10E0, "m_bbone_jnt_num >= 0");
    m7E6 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armR2");
    if (m7E6 < 0)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x10E3, "m_armR2_jnt_num >= 0");
    return a_mdl_dat;
}

/* 02273944 */
J3DModelData* daNpc_Ko1_c::create_hed_Anm() {
    // Select the matching head model and animation.
    J3DModelData* a_mdl_dat = (J3DModelData*)ko1ObjectRes("Ko", s32(a_hed_bdl_resID_tbl[mType]));
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4358, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1106, "a_mdl_dat != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)ko1ObjectRes("Ko", s32(a_hed_bck_resID_tbl[mType]));
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
        m7E7 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head2");
        if (m7E7 < 0)
            JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1120, "m_hed_2_jnt_num >= 0");
    }
    return a_mdl_dat;
}

/* 02273C94 */
J3DModelData* daNpc_Ko1_c::create_bln_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)ko1ObjectRes("Ko", 0x1A);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(4400, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1130, "a_mdl_dat != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)ko1ObjectRes("Ko", 1);
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
    m7E8 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "balloon_loc");
    if (m7E8 < 0)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1146, "m_bln_loc_jnt_num >= 0");
    m7E9 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "ko_balloon");
    if (m7E9 < 0)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1149, "m_bln_jnt_num >= 0");
    return a_mdl_dat;
}

/* 02273E7C */
bool daNpc_Ko1_c::create_itm_Mdl() {
    if (mType == 1) {
        J3DModelData* a_mdl_dat = (J3DModelData*)ko1ObjectRes("Ko", 0x19);
        if (a_mdl_dat == nullptr) /* JUT_ASSERT(4451, a_mdl_dat != NULL) */
            JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x1163, "a_mdl_dat != (0)");
        mpModel820 = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
        if (mpModel820.get() == nullptr) {
            return false;
        }
    }
    return true;
}

/* 02273F48 */
bool daNpc_Ko1_c::CreateHeap() {
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
    // HD: retain the original stack location so unchecked child-type indexing also
    // preserves reads of neighboring stack bytes.
    be<s8>* a_btp_num = gabi::at<be<s8>>(sp_at_entry - 0x1C);
    a_btp_num[0] = 0;
    a_btp_num[1] = 0;
    mBtpNum = a_btp_num[mType];
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
                setJointCallBack(mpMorf81C->getModel(), i, nodeCallBack_Bln);
        }
        mpMorf81C->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
        for (u16 i = 0; i < J3DModelData_getJointNum(hed_model); i++) {
            if (i == (u32)(s32)m7E7)
                setJointCallBack(mpMorf824->getModel(), i, nodeCallBack_Hed);
        }
        mpMorf824->getModel()->setUserArea((uintptr_t)this);
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
        if (i == (u32)(s32)m7E4 || i == (u32)(s32)m7E5)
            setJointCallBack(mpMorf->getModel(), i, nodeCallBack_Ko1);
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this);
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return true;
}

/* WWHD glue
str 1001CA8C "Ko" in 02273B80
str 1001CA90 "d_a_npc_ko1.cpp" in 02273B80
str 1001CAA0 "m_hed_tex_pttrn != (0)" in 02273B80
str 1001C9B4 "Ko" in 022736FC
str 1001C9C0 "d_a_npc_ko1.cpp" in 022736FC
str 1001C9D0 "a_mdl_dat != (0)" in 022736FC
str 1001C9B8 "head" in 022736FC
str 1001C9E4 "m_hed_jnt_num >= 0" in 022736FC
str 1001C9F8 "backbone" in 022736FC
str 1001CA04 "m_bbone_jnt_num >= 0" in 022736FC
str 1001C9AC "armR2" in 022736FC
str 1001CA1C "m_armR2_jnt_num >= 0" in 022736FC
str 1001CA3C "Ko" in 02273944
str 1001CA40 "d_a_npc_ko1.cpp" in 02273944
str 1001CA50 "a_mdl_dat != (0)" in 02273944
str 1001CA34 "head2" in 02273944
str 1001CA64 "m_hed_2_jnt_num >= 0" in 02273944
str 1001CAB8 "Ko" in 02273C94
str 1001CABC "d_a_npc_ko1.cpp" in 02273C94
str 1001CAD8 "a_mdl_dat != (0)" in 02273C94
str 1001CACC "balloon_loc" in 02273C94
str 1001CAEC "m_bln_loc_jnt_num >= 0" in 02273C94
str 1001CB04 "ko_balloon" in 02273C94
str 1001CB10 "m_bln_jnt_num >= 0" in 02273C94
str 1001CB24 "Ko" in 02273E7C
str 1001CB28 "d_a_npc_ko1.cpp" in 02273E7C
str 1001CB38 "a_mdl_dat != (0)" in 02273E7C
data l_check_wrk 104677B0
data l_check_inf 104678A8
data l_HIO 104677E8
data a_eye_pos_off 104677D0
data eyeOffsetGuard 104678F8
data l_currentModel 104B462C
data l_currentMatrix 104B4868
data a_btp_resID_tbl 1001CA7C
data a_bdl_resID_tbl 101BF790
data a_hed_bdl_resID_tbl 101BF798
data a_hed_bck_resID_tbl 101BF7A0
data a_specificType_tbl 1001CB50
data pmf_hana_action1 1001C820
data pmf_hana_action2 1001C828
data pmf_hana_action3 1001C830
data pmf_hana_action4 1001C838
data pmf_hana_action5 1001C840
data pmf_wait_action1 1001C848
data pmf_wait_action2 1001C850
data pmf_wait_action3 1001C858
data pmf_wait_action4 1001C860
ext 02273B80 u32 setBtpRegister(daNpc_Ko1_c*, u32, s32)
ext 02273F48 u32 createHeapRegister(daNpc_Ko1_c*)
*/
