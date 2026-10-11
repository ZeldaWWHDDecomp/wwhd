/**
 * d_a_npc_bm1.cpp (WWHD)
 * NPC - Rito (generic Ritos on Dragon Roost Island): setup, animation, particles, events
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bm1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Part A: 021FA778 .. 021FFFFC (the rest is in d_a_npc_bm1_b.cpp).
 */
#include "d/actor/d_a_npc_bm1.h"
#include "gc_api/d/d_com_inf_game.h"

const gc_tu_data gc_tu = {.safeString = 0x10016F80};

// SafeString vtable is supplied by gc_tu.
#define BM1_VTBL 0x10017A18 /* daNpc_Bm1_c vtable (HD: merged with fopNpc_npc_c's) */

// HD: private bindings retain the measured ABI where shared APIs differ.
/* 02606900 HD: dRes_control_c::getRes(const sead::SafeString& arc, const sead::SafeString& name)
 * (Bm1_resource(arc, name)) */

/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at
 * +0x10 to the JUTNameTab), 027F3F94 (the matcher calls it __nw) returns the joint tree
 * header (joint count u16 at +8), 027F3F8C returns the model's name header (self-relative
 * offset at +4 to the name) */
static inline u32 J3DModelData_getJointName(J3DModelData* data) {
    return gabi::ea(data->getJointName());
}
static inline u16 J3DModelData_getJointNum(J3DModelData* data) { return data->getJointNum(); }
static inline u32 J3DModelData_getName(J3DModelData* data) {
    u32 header = Bm1_modelNameHeader(data);
    u32 offset = Bm1_view<Bm1NameHeader>(header)->mNameOffset;
    return offset ? header + 4 + offset : 0;
}
/* J3DModel (HD): model data at +0xAC */
static inline J3DModelData* Bm1_modelData(J3DModel* m) { return m->getModelData(); }

/* inline strcmp / strcat (byte loops in the WWHD code) */
static inline s32 strcmp_i(u32 a, const char* text) {
    u32 b = gabi::guest_addr(text);
    for (;; a++, b++) {
        u8 c1 = *Bm1_view<be<u8>>(a), c2 = *Bm1_view<be<u8>>(b);
        if (c1 != c2 || c1 == 0)
            return (s32)c1 - (s32)c2;
    }
}
static inline void strcat_i(u32 d, const char* text) {
    u32 s = gabi::guest_addr(text);
    while (*Bm1_view<be<u8>>(d) != 0)
        d++;
    for (;; d++, s++) {
        u8 c = *Bm1_view<be<u8>>(s);
        *Bm1_view<be<u8>>(d) = c;
        if (c == 0)
            break;
    }
}
/* delete of an mDoExt_McaMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */

/* joint node callback: getJointNodePointer(i)->setCallBack(cb) (HD: node table at modelData+8,
 * count +4, 0x1C bytes per node, callback +8) */
static inline void setJointCallBack(J3DModelData* data, u32 index, u32 callback) {
    Bm1JointTable* table = reinterpret_cast<Bm1JointTable*>(data);
    u32 count = table->mCount;
    Bm1JointCallback* joint = table->mJoints;
    if (index < count)
        joint += index;
    joint->mCallback = callback;
}
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, re-read at every use */
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */

/* JPABaseEmitter (HD): flags +0x254, max frame +0x5C, direction +0x28, global R/T +0x1F0/+0x22C */
static inline void JPABaseEmitter_becomeImmortalEmitter(JPABaseEmitter* e) {
    Bm1EmitterView* view = reinterpret_cast<Bm1EmitterView*>(e);
    view->mStatus |= 0x40;
}
static inline void JPABaseEmitter_quitImmortalEmitter(JPABaseEmitter* e) {
    Bm1EmitterView* view = reinterpret_cast<Bm1EmitterView*>(e);
    view->mStatus &= ~0x40u;
}
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    Bm1EmitterView* view = reinterpret_cast<Bm1EmitterView*>(e);
    view->mMaxFrame = -1;
    view->mStatus |= 1;
}
/* setGlobalRTMatrix(m): 028249B0 JPASetRMtxTVecfromMtx(m, &mGlobalRot, &mGlobalTrs) */
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    Bm1EmitterMatrix* view = reinterpret_cast<Bm1EmitterMatrix*>(e);
    Bm1_setEmitterMatrix(m, gabi::ea(&view->mGlobalRotation), gabi::ea(&view->mGlobalTranslation));
}

static Bm1SetupHio& l_HIO() { return *Bm1_view<Bm1SetupHio>(0x1046623C); }
/* l_HIO.children[mType - 1].hio_prm */
static Bm1SetupHioParam& hio_prm(s8 type) {
    return l_HIO().children[type - 1].hio_prm;
} /* &l_HIO.children[type - 1].hio_prm */
/* dBgS (play + 0x12A0) */

/* dNpc_PathRun_c (HD out of line) */

/* 0259D344 dNpc_setAnmFNDirect(morf, loopMode, morf, speed, const char* bckName, int soundIdx, const char* arc) */

static inline void J3DModel_setUserArea(J3DModel* m, void* p) { m->mUserArea = gabi::ea(p); }

enum { fpcNm_NPC_ZL1_e = 0x1AA, fpcNm_NPC_GP1_e = 0x165, fpcNm_NPC_BM2_e = 0x147 };

static const char* a_headBDLName_TBL[] = {
    "bmhead01.bdl", "bmhead01.bdl", "bmhead01.bdl", "bmhead01.bdl", "bmhead01.bdl", "bmhead02.bdl",
    "bmhead02.bdl", "bmhead07.bdl", "bmhead07.bdl", "bmhead05.bdl", "bmhead04.bdl", "bmhead04.bdl",
    "bmhead04.bdl", "bmhead09.bdl", "bmhead06.bdl", "bmhead03.bdl"};
static be<u32>* a_BTPName_TBL = Bm1_view<be<u32>>(0x101BC5F0);
static const char* a_BTPName2_TBL[] = {
    "bmhead01", "bmhead01", "bmhead01", "bmhead01", "bmhead01", "bmhead02", "bmhead02", "bmhead07",
    "bmhead07", "bmhead05", "bmhead03", "bmhead04", "bmhead04", "bmhead09", "bmhead06", "bmhead04"};
static const char* l_evn_tbl[] = {"Get_Mo3_Ltr", "Met_Ryu_Islnd", "Get_Rupee", "Skn_Islnd"};
static const char* a_staff_tbl[] = {"Bm1", "Bm1", "Bm1", "Bm1", "Bm1", "Bm2", "Bm2", "Bm3",
                                    "Bm3", "Bm3", "Bm4", "Bm4", "Bm4", "Bm4", "Bm5", "Bm5"};
static u8 a_att_dis_TBL[] = {169, 169, 169, 169, 109, 109, 111, 109, 169, 169, 128,
                             128, 128, 128, 169, 169, 168, 168, 168, 168, 170, 170};
static u32 a_size_tbl[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* ---- file statics ---- */
static be<s32>& l_check_wrk() { return *Bm1_view<be<s32>>(0x104661D0); }
static gptr<fopAc_ac_c>* l_check_inf() { return Bm1_view<gptr<fopAc_ac_c>>(0x10466590); } /* [20] */

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static Mtx34* getAnmMtx(J3DModel* model, s32 jointNo) { return model->getAnmMtx(jointNo); }
static Mtx34* j3dSys_mCurrentMtx() { return Bm1_view<Mtx34>(0x104B4868); }
/* mDoMtx_stack_c::multVecZero(o): the translation column */
static void mDoMtx_stack_multVecZero(cXyz* o) {
    Mtx34* m = mDoMtx_stack_c::get();
    o->x = m->m[0][3];
    o->y = m->m[1][3];
    o->z = m->m[2][3];
}

/* searchActor_*: collect actors into l_check_inf (HD: the actor pointer is checked for NULL) */
static inline bool searchActor_chk(void* i_param_1, s16 name) {
    return l_check_wrk() < 0x14 && fopAc_IsActor(i_param_1) && i_param_1 != nullptr &&
           fpcM_GetName(i_param_1) == name;
}
static inline void searchActor_add(void* i_param_1) {
    s32 n = l_check_wrk();
    l_check_wrk() = n + 1;
    l_check_inf()[n] = (fopAc_ac_c*)i_param_1;
}

/* ---- pointers to member functions in .data (copied to the stack before set_action) ---- */
enum : u32 {
    PMF_demo_action1 = 0x10016F18,
    PMF_wait_action1 = 0x10016F20,
    PMF_wait_action3 = 0x10016F28,
    PMF_wait_action9 = 0x10016F30,
    PMF_wait_action6 = 0x10016F38,
    PMF_wait_action8 = 0x10016F40,
    PMF_wait_action2 = 0x10016F48,
    PMF_wait_action4 = 0x10016F50,
    PMF_wait_action5 = 0x10016F58,
    PMF_wait_actionA = 0x10016F60,
    PMF_wait_action7 = 0x10016F68,
};

/* (this->*pmf)(arg) */

static u32 bckName(u32 tbl, int i, const char* ext) {
    u32 l_BCKName = 0x104661FC;
    strcpy_g(Bm1_view<char>(l_BCKName), Bm1_view<gptr<const char>>(tbl)[i].get());
    strcat_i(l_BCKName, ext);
    return l_BCKName;
}

/* 021FA778 */
static void* searchActor_Zl(void* i_param_1, void*) {
    if (searchActor_chk(i_param_1, fpcNm_NPC_ZL1_e)) {
        searchActor_add(i_param_1);
    }
    return nullptr;
}

/* 021FA7F8 */
static void* searchActor_Gp(void* i_param_1, void*) {
    if (searchActor_chk(i_param_1, fpcNm_NPC_GP1_e)) {
        searchActor_add(i_param_1);
    }
    return nullptr;
}

/* 021FA878 */
static void* searchActor_Bm_Skt(void* i_param_1, void*) {
    if (searchActor_chk(i_param_1, fpcNm_NPC_BM2_e)) {
        if (((daNpc_Bm1_c*)i_param_1)->mSpecificType ==
            daNpc_Bm1_c::SPECIFIC_TYPE_Skett_e) { /* IamSukketo() */
            searchActor_add(i_param_1);
        }
    }
    return nullptr;
}

/* 021FA908 */
static void* searchActor_Bm_Kkt(void* i_param_1, void*) {
    if (searchActor_chk(i_param_1, fpcNm_NPC_BM2_e)) {
        if (((daNpc_Bm1_c*)i_param_1)->mSpecificType ==
            daNpc_Bm1_c::SPECIFIC_TYPE_Akoot_e) { /* IamKakkuto() */
            searchActor_add(i_param_1);
        }
    }
    return nullptr;
}

/* 021FAB78 */
static BOOL nodeCallBack_Wng(J3DNode* i_param_1, int i_param_2) {
    if (i_param_2 == 0) {
        J3DModel* model = *Bm1_view<gptr<J3DModel>>(0x104B462C);
        daNpc_Bm1_c* user = Bm1_view<daNpc_Bm1_c>(model->mUserArea);
        if (user != nullptr) {
            user->nodeWngControl(i_param_1, model);
        }
    }
    return TRUE;
}

/* 021FA998 */
void daNpc_Bm1_c::nodeWngControl(J3DNode* i_node, J3DModel* i_model) {
    u32 jointNo = Bm1_jointNumber(J3DNode_toJoint(i_node)); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, jointNo), mDoMtx_stack_c::get());
    if (jointNo == (u32)(s32)m_wngL1_jnt_num) {
        PSMTXCopy(&mLeftArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, jointNo), &mLeftArmMtx); /* setAnmMtx */
    }
    if (jointNo == (u32)(s32)m_wngL3_jnt_num) {
        mDoMtx_stack_multVecZero(&mWingLPos);
    }
    if (jointNo == (u32)(s32)m_wngR1_jnt_num) {
        PSMTXCopy(&mRightArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, jointNo), &mRightArmMtx);
    }
    if (jointNo == (u32)(s32)m_wngR3_jnt_num) {
        mDoMtx_stack_multVecZero(&mWingRPos);
    }
}

/* 021FADA0 */
static BOOL nodeCallBack_Arm(J3DNode* i_param_1, int i_param_2) {
    if (i_param_2 == 0) {
        J3DModel* model = *Bm1_view<gptr<J3DModel>>(0x104B462C);
        daNpc_Bm1_c* user = Bm1_view<daNpc_Bm1_c>(model->mUserArea);
        if (user != nullptr) {
            user->nodeArmControl(i_param_1, model);
        }
    }
    return TRUE;
}

/* 021FABC0 */
void daNpc_Bm1_c::nodeArmControl(J3DNode* i_node, J3DModel* i_model) {
    u32 jointNo = Bm1_jointNumber(J3DNode_toJoint(i_node)); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, jointNo), mDoMtx_stack_c::get());
    if (jointNo == (u32)(s32)m_armL1_jnt_num) {
        PSMTXCopy(&mLeftArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, jointNo), &mLeftArmMtx);
    }
    if (jointNo == (u32)(s32)m_armL2_jnt_num) {
        mDoMtx_stack_multVecZero(&mArmLPos);
    }
    if (jointNo == (u32)(s32)m_armR1_jnt_num) {
        PSMTXCopy(&mRightArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, jointNo), &mRightArmMtx);
    }
    if (jointNo == (u32)(s32)m_armR2_jnt_num) {
        mDoMtx_stack_multVecZero(&mArmRPos);
    }
}

/* 021FAFC8 */
static BOOL nodeCallBack_Bm1(J3DNode* i_param_1, int i_param_2) {
    if (i_param_2 == 0) {
        J3DModel* model = *Bm1_view<gptr<J3DModel>>(0x104B462C);
        daNpc_Bm1_c* user = Bm1_view<daNpc_Bm1_c>(model->mUserArea);
        if (user != nullptr) {
            user->nodeBm1Control(i_param_1, model);
        }
    }
    return TRUE;
}

/* 021FADE8 */
void daNpc_Bm1_c::nodeBm1Control(J3DNode* i_node, J3DModel* i_model) {

    cXyz* a_eye_pos_off = Bm1_view<cXyz>(0x104661F0);
    be<u32>& a_eye_pos_guard = *Bm1_view<be<u32>>(0x104665E0);
    if (a_eye_pos_guard == 0) {
        a_eye_pos_off->x = 26.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 26.0f;
        a_eye_pos_guard = 1;
    }
    u32 jointNo = Bm1_jointNumber(J3DNode_toJoint(i_node)); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, jointNo), mDoMtx_stack_c::get());
    if (jointNo == (u32)(s32)m_nec_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
    }
    if (jointNo == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    if (jointNo == (u32)(s32)m_arm_L_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mLeftArmMtx);
    }
    if (jointNo == (u32)(s32)m_arm_R_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mRightArmMtx);
    }
    if (jointNo == (u32)(s32)m_hed_jnt_num) {
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &mEyePos);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jointNo), mDoMtx_stack_c::get());
}

/* 021FC5DC */
bool daNpc_Bm1_c::chk_appCnd() {
    switch ((u32)(s32)mSpawnCondition) {
    case 0:
        return true;
    case 1:
        if (!dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
            return true;
        }
        break;
    case 2:
        if (dComIfGs_isSymbol(1)) {
            return !dComIfGs_isEventBit(0x1A80);
        }
        break;
    case 3:
        if (!dComIfGs_isSymbol(1)) {
            return false;
        }
        if (dComIfGs_isEventBit(0x1A80)) {
            return dKy_daynight_check() == 0 /* dKy_TIME_DAY_e */;
        }
        break;
    case 4:
        if (!dComIfGs_isSymbol(1)) {
            return false;
        }
        if (dComIfGs_isEventBit(0x1A80)) {
            return dKy_daynight_check() == 1 /* dKy_TIME_NIGHT_e */;
        }
        break;
    }
    return false;
}

/* 021FC884 */
bool daNpc_Bm1_c::init_PST_0() {
    bool o_retval = false;
    if (!dComIfGs_isEventBit(0x0001)) {
        {
            Bm1ActionFunc action;
            pmf_load(&action, PMF_demo_action1);
            set_action(&action, nullptr);
        }
        o_retval = true;
        actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
        mbInitPostman0 = true;
    }
    return o_retval;
}

/* 021FC910 */
bool daNpc_Bm1_c::init_PST_1() {
    {
        Bm1ActionFunc action;
        pmf_load(&action, PMF_wait_action1);
        set_action(&action, nullptr);
    }
    return true;
}

/* 021FC950 */
bool daNpc_Bm1_c::init_PST_2() {
    bool o_retval = false;
    if (!dComIfGs_isEventBit(0x1F40)) {
        {
            Bm1ActionFunc action;
            pmf_load(&action, PMF_wait_action3);
            set_action(&action, nullptr);
        }
        o_retval = true;
    }
    return o_retval;
}

/* 021FC9CC */
bool daNpc_Bm1_c::init_PST_3() {
    bool result = dComIfGs_isSymbol(1 /* dSymbol_DIN_e */) == 0;
    if (result) {
        result = dComIfGs_isEventBit(0x1102) != 0;
        if (result) {
            {
                Bm1ActionFunc action;
                pmf_load(&action, PMF_wait_action1);
                set_action(&action, nullptr);
            }
        }
    }
    return result;
}

/* 021FCA68 */
bool daNpc_Bm1_c::init_PST_4() {
    bool result = dComIfGs_isEventBit(0x1E80) == 0;
    if (result) {
        m888 = 1;
        gravity = 0.0f;
        {
            Bm1ActionFunc action;
            pmf_load(&action, PMF_wait_action9);
            set_action(&action, nullptr);
        }
        actor_status |= 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
        return true;
    }
    return result;
}

/* 021FCB50 */
bool daNpc_Bm1_c::init_BMB_0() {
    {
        Bm1ActionFunc action;
        pmf_load(&action, PMF_wait_action6);
        set_action(&action, nullptr);
    }
    return true;
}

/* 021FCB90 */
u32 daNpc_Bm1_c::init_BMB_1() { return Bm1_initBashtResult(this); /* init_BMB_0() */ }

/* 021FCB94 */
bool daNpc_Bm1_c::init_BMB_2() {
    {
        Bm1ActionFunc action;
        pmf_load(&action, PMF_wait_action8);
        set_action(&action, nullptr);
    }
    return true;
}

/* 021FCBD4 */
bool daNpc_Bm1_c::init_BMC_0() {
    {
        Bm1ActionFunc action;
        pmf_load(&action, PMF_wait_action2);
        set_action(&action, nullptr);
    }
    return true;
}

/* 021FCC14 */
bool daNpc_Bm1_c::init_BMC_1() {
    bool result = false;
    if (!dComIfGs_isEventBit(0x1808)) {
        result = dComIfGs_isEventBit(0x1220) != 0;
        if (result) {
            {
                Bm1ActionFunc action;
                pmf_load(&action, PMF_wait_action4);
                set_action(&action, nullptr);
            }
        }
    }
    return result;
}

/* 021FCCB0 */
bool daNpc_Bm1_c::init_BMC_2() {
    bool result = false;
    if (!dComIfGs_isGetItemReserve(0x0F)) {
        result = dComIfGs_isEventBit(0x1808) != 0;
        if (result) {
            {
                Bm1ActionFunc action;
                pmf_load(&action, PMF_wait_action5);
                set_action(&action, nullptr);
            }
        }
    }
    return result;
}

/* 021FCD4C */
bool daNpc_Bm1_c::init_BMC_3() {
    if (mPathRun.mPath.get() != nullptr) {
        {
            Bm1ActionFunc action;
            pmf_load(&action, PMF_wait_actionA);
            set_action(&action, nullptr);
        }
        return true;
    }
    return false;
}

/* 021FCDAC */
u32 daNpc_Bm1_c::init_BMD_0() { return Bm1_initPashliResult(this); /* init_BMC_3() */ }

/* 021FCDB0 */
bool daNpc_Bm1_c::init_BMD_1() {
    if (dComIfGs_isEventBit(0x1620)) {
        return false;
    }
    return init_BMC_0();
}

/* 021FCB00 */
bool daNpc_Bm1_c::init_SKT_0() {
    actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    {
        Bm1ActionFunc action;
        pmf_load(&action, PMF_wait_action7);
        set_action(&action, nullptr);
    }
    return true;
}

/* 021FCB4C */
u32 daNpc_Bm1_c::init_KKT_0() { return Bm1_initSkettResult(this); /* init_SKT_0() */ }

/* 021FDB44 */
bool daNpc_Bm1_c::createInit() {
    if (chk_appCnd() == 0) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        const char* name = l_evn_tbl[i];
        mEventIdTable[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }

    attention_info.flags = 0xA; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[1] = a_att_dis_TBL[mType * 2];     /* distances[TALK] */
    attention_info.distances[3] = a_att_dis_TBL[mType * 2 + 1]; /* distances[SPEAK] */
    gravity = -4.5f;
    m82C.copy(current.pos);
    s32 collisionWeight = 0xFF;
    u32 pathId = (fopAcM_GetParam(this) >> 16) & 0xFF;
    if (pathId != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathId, fopAcM_GetRoomNo(this), true);
        // HD:mPathRun.isPath() is the path pointer test

        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        collisionWeight = 0xF0;
    }

    mEventCut.setActorInfo2(a_staff_tbl[mSpecificType], (fopNpc_npc_c*)this);
    mAnmNum = 0x16;
    u32 init_success;
    switch ((u32)(s32)mSpecificType) {
    case SPECIFIC_TYPE_Quill_0_e:
        init_success = init_PST_0();
        break;
    case SPECIFIC_TYPE_Quill_1_e:
        init_success = init_PST_1();
        break;
    case SPECIFIC_TYPE_Quill_2_e:
        init_success = init_PST_2();
        break;
    case SPECIFIC_TYPE_Quill_3_e:
        init_success = init_PST_3();
        break;
    case SPECIFIC_TYPE_Quill_4_e:
        init_success = init_PST_4();
        break;
    case SPECIFIC_TYPE_Akoot_e:
        init_success = init_KKT_0();
        break;
    case SPECIFIC_TYPE_Skett_e:
        init_success = init_SKT_0();
        break;
    case SPECIFIC_TYPE_Basht_e:
        init_success = init_BMB_0();
        break;
    case SPECIFIC_TYPE_Bisht_e:
        init_success = init_BMB_1();
        break;
    case SPECIFIC_TYPE_Hoskit_e:
        init_success = init_BMB_2();
        break;
    case SPECIFIC_TYPE_Ilari_0xA_e:
        init_success = init_BMC_0();
        break;
    case SPECIFIC_TYPE_Ilari_0xB_e:
        init_success = init_BMC_1();
        break;
    case SPECIFIC_TYPE_Ilari_0xC_e:
        init_success = init_BMC_2();
        break;
    case SPECIFIC_TYPE_Pashli_e:
        init_success = init_BMC_3();
        break;
    case SPECIFIC_TYPE_Namali_e:
        init_success = init_BMD_0();
        break;
    case SPECIFIC_TYPE_Kogoli_e:
        init_success = init_BMD_1();
        break;
    default:
        init_success = false;
        break;
    }
    if (init_success == 0) {
        return false;
    }
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(collisionWeight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(Bm1_view<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    if (mbHasArms) {
        mpArmMorf->setMorf(0.0f);
    } else {
        mpWingMorf->setMorf(0.0f);
    }
    mpHeadMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 021FD41C */
void daNpc_Bm1_c::setMtx(u32 i_param_1) {
    void* gndPoly =
        static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* mObjAcch.m_gnd's cBgS_PolyInfo */
    if (mbInDemo == 0) {
        u32 materialSound = 0;
        plyTexPttrnAnm();
        if (mObjAcch.ChkGroundHit()) {
            materialSound = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)gndPoly);
        }
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, materialSound, (s8)reverb);
        if (mpMorf->getFrame() < mFrame) {
            mbMorfAnimStopped = true;
        }
        mFrame = mpMorf->getFrame();
        if (mbHasArms) {
            mpArmMorf->play(&eyePos, 0, 0);
        } else {
            mpWingMorf->play(&eyePos, 0, 0);
        }
        mpHeadMorf->play(&eyePos, 0, 0);
        mObjAcch.CrrPos(dComIfG_Bgsp());
        // HD:with the HD-only flag at 0xA08 set, the actor stays at its old position

        if (mbKeepOldPosition != 0) {
            current.pos.copy(old.pos);
        }
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    tevStr.mEnvrIdxOverride =
        dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (!mbHasArms) {
        mpWingMorf->calc();
    } else {
        mpArmMorf->calc();
    }
    J3DModel_setBaseTRMtx(mpHeadMorf->getModel(), getAnmMtx(mpMorf->getModel(), m_hed_jnt_num));
    // HD:calc only with an animation

    if (mpHeadMorf->getAnm() != nullptr) {
        mpHeadMorf->calc();
    }
    if (mpBinderModel.get() != nullptr) {
        if (mAnmNum == 4) {
            PSMTXCopy(getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(13.5f, 3.0f, -5.5f);
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), -0x5DDE /* cM_deg2s(228) */,
                           -0x5111 /* cM_deg2s(-114) */, 0x671D /* cM_deg2s(-215) */);
            J3DModel_setBaseTRMtx(mpBinderModel, mDoMtx_stack_c::get());
        } else {
            J3DModel_setBaseTRMtx(mpBinderModel, getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num));
        }
        J3DModel_calc(mpBinderModel);
    }
    if (mpBagModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpBagModel, getAnmMtx(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpBagModel);
    }
    if (mpKnifeModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpKnifeModel, getAnmMtx(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpKnifeModel);
    }
    if (mbHasArms && mpStickModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpStickModel, getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num));
        J3DModel_calc(mpStickModel);
    }
    setPrtcl_Flyaway();
    setPrtcl_Land0();
    flwPrtcl_Hane0();
    flwPrtcl_Hane1();
    setAttention(i_param_1);
}

/* 021FE794 */
u32 daNpc_Bm1_c::anmNum_toResID(int i_anmNum) {
    return bckName(0x101BC6B8 /* a_BCKName_TBL */, i_anmNum, ".bck");
}

/* 021FE87C */
u32 daNpc_Bm1_c::headAnmNum_toResID(int i_anmNum) {
    return bckName(0x101BC768 /* "bmhead01_dwait", .. */, i_anmNum, ".bck");
}

/* 021FE808 */
u32 daNpc_Bm1_c::wingAnmNum_toResID(int i_anmNum) {
    return bckName(0x101BC710 /* "bmwing_dwait", .. */, i_anmNum, ".bck");
}

/* 021FB4C4 */
// HD:reads a_BTPName_TBL[0] and indexes a_BTPName_TBL_2 by mSpecificType (the argument is unused)

u32 daNpc_Bm1_c::btpNum_toResID(int i_param_1) {
    u32 l_BTPName = 0x1046621C;
    u32 tbl0 = a_BTPName_TBL[0]; /* a_BTPName_TBL[0] */
    if (strcmp_i(tbl0, "bmhead01") == 0) {
        strcpy_g(Bm1_view<char>(l_BTPName), a_BTPName2_TBL[mSpecificType]);
        strcat_i(l_BTPName, ".btp");
    } else {
        strcpy_g(Bm1_view<char>(l_BTPName), Bm1_view<const char>(tbl0));
        strcat_i(l_BTPName, ".btp");
    }
    return l_BTPName;
}

/* 021FB5B8 */
// HD:unless the head model is "bmhead08" or "bmhead10", the btp is "<head model name>.btp"

bool daNpc_Bm1_c::setBtp(u32 i_param_1, int i_btp_num) {
    J3DModelData* model_data = Bm1_modelData(mpHeadMorf->getModel());
    u32 name = J3DModelData_getName(model_data);
    if (strcmp_i(name, "bmhead08") != 0 && strcmp_i(name, "bmhead10") != 0) {
        u32 l_BTPName = 0x1046621C;
        strcpy_g(Bm1_view<char>(l_BTPName), Bm1_view<const char>(name));
        strcat_i(l_BTPName, ".btp");
        m_hed_tex_pttrn =
            (J3DAnmTexPattern*)Bm1_resource(mArcName, Bm1_view<const char>(l_BTPName));
    } else {
        u32 res_id = btpNum_toResID(i_btp_num);
        m_hed_tex_pttrn = (J3DAnmTexPattern*)Bm1_resource(mArcName, Bm1_view<const char>(res_id));
    }
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(0x550, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x550, "m_hed_tex_pttrn != (0)");
    int initResult = mDoExt_btpAnm_init(mHeadBtpAnm, model_data, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1,
                                        i_param_1, 0);
    bool o_retval = initResult == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return o_retval;
}

/* 021FB78C */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Bm1_c::iniTexPttrnAnm(u32 i_param_1) {
    return Bm1_setBtpResult(this, i_param_1, (s32)mBtpNum); /* setBtp(i_param_1, mBtpNum) */
}

/* 021FCE10 */
void daNpc_Bm1_c::plyTexPttrnAnm() {
    if (mBtpNum != 0 || !cLib_calcTimer_g(&mBlinkTimer)) {
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

/* 021FE774 */
u32 daNpc_Bm1_c::setAnm_tex(s8 i_param_1) {
    if (mBtpNum != i_param_1) {
        mBtpNum = i_param_1;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 021FEAB4 */
BOOL daNpc_Bm1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 anmNum = i_anmPrmP->anmNum;
    if (mAnmNum == anmNum) {
        return TRUE;
    }
    mAnmNum = anmNum;
    u32 name = anmNum_toResID(anmNum);
    dNpc_setAnmFNDirect(mpMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, name, 0,
                        mArcName);
    bool hasArms = i_anmPrmP->hasArms == 1;
    mbHasArms = hasArms;
    f32 morf = i_anmPrmP->morf;
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    // HD:the arms or wings take the wing table, the head the head table

    mDoExt_McaMorf* partsMorf = hasArms ? mpArmMorf.get() : mpWingMorf.get();
    name = wingAnmNum_toResID(mAnmNum);
    dNpc_setAnmFNDirect(partsMorf, loopMode, morf, speed, name, 0, mArcName);
    name = headAnmNum_toResID(mAnmNum);
    dNpc_setAnmFNDirect(mpHeadMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, name, 0,
                        mArcName);
    delPrtcl_Hane0();
    delPrtcl_Hane1();
    switch ((u32)(s32)mAnmNum) {
    case 2:
        setPrtcl_Hane1();
        break;
    case 3:
        setPrtcl_Hane0();
        break;
    }
    mbMorfAnimStopped = false;
    mFrame = 0.0f;
    m87B = 0;
    return TRUE;
}

/* 021FECF4 */
void daNpc_Bm1_c::setAnm_NUM(int i_param_1, int i_param_2) {
    anm_prm_c* a_anm_prm_tbl = Bm1_view<anm_prm_c>(0x101BC7C0); /* [0x17] */
    if (i_param_2 != 0) {
        setAnm_tex(a_anm_prm_tbl[i_param_1].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_param_1]);
}

/* 021FE408 */
void daNpc_Bm1_c::checkOrder() {
    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx])) {
            if (mEventIdx == 3) {
                actor_status &= ~0x4000u; /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
            }
            m8FD = 0;
        }
    } else if (command == 1 /* dEvtCmd_INTALK_e */ && (m8FD == 1 || m8FD == 2)) {
        m8FD = 0;
        m895 = true;
    }
}

/* 021FD3BC */
void daNpc_Bm1_c::setAttention(u32 i_setEyePos) {
    f32 l_hio_offset = hio_prm(mType).mAttPosOffsetY; /* mAttPosOffsetY */
    cXyz* attPos = &attention_info.position;          /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_hio_offset;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
}

/* 021FE2C4 */
fopAc_ac_c* daNpc_Bm1_c::searchByID(fpc_ProcID i_procID) {
    Bm1ActorResult result;
    result.actor = nullptr;
    Bm1_searchById(i_procID, &result.actor); /* fopAcM_SearchByID(id, &actor) */
    return result.actor;
}

/* 021FE218 */
bool daNpc_Bm1_c::partner_srch_sub(u32 i_searchFunc) {
    bool o_retval = false;
    mPartnerProcID = 0xFFFFFFFF;
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_searchFunc, this);
    if (l_check_wrk() != 0) {
        mPartnerProcID = fopAcM_GetID(l_check_inf()[0].get());
        o_retval = true;
    }
    return o_retval;
}

/* 021FE2F8 */
void daNpc_Bm1_c::partner_srch() {
    bool found_partner = false;
    if (m904 == 1) {
        switch ((u32)(s32)mSpecificType) {
        case SPECIFIC_TYPE_Quill_1_e:
            found_partner = partner_srch_sub(0x021FA778 /* searchActor_Zl */);
            break;
        case SPECIFIC_TYPE_Skett_e:
            found_partner = partner_srch_sub(0x021FA908 /* searchActor_Bm_Kkt */);
            break;
        case SPECIFIC_TYPE_Akoot_e:
            found_partner = partner_srch_sub(0x021FA878 /* searchActor_Bm_Skt */);
            break;
        case SPECIFIC_TYPE_Ilari_0xB_e:
            if (partner_srch_sub(0x021FA7F8 /* searchActor_Gp */)) {
                fopAc_ac_c* actor = searchByID(mPartnerProcID);
                if (actor != nullptr) {
                    current.angle.y = cLib_targetAngleY(&current.pos, &actor->current.pos);
                }
                found_partner = true;
            }
            break;
        }
        if (found_partner) {
            m904 = m904 + 1;
        }
    }
}

/* 021FEEB0 */
void daNpc_Bm1_c::bm_setFlyAnm() { setAnm_NUM(m889 ? 9 : 8, 1); }

/* 021FCF54 */
void daNpc_Bm1_c::setPrtcl_Flyaway() {
    if (mAnmNum == 0xB && mpMorf->checkFrame(10.0f)) {
        delPrtcl_Flyaway();
        mpFlyawayEmitterL = dComIfGp_particle_set(
            0x8275 /* ID_IT_SN_BM_HANEL_FLYAWAY00 */, &mWingLPos, &current.angle, nullptr, 0xFF,
            nullptr, fopAcM_GetRoomNo(this), &tevStr.mColorK0, &tevStr.mColorK0);
        mpFlyawayEmitterR =
            dComIfGp_particle_set(0x8275, &mWingRPos, &current.angle, nullptr, 0xFF, nullptr,
                                  fopAcM_GetRoomNo(this), &tevStr.mColorK0, &tevStr.mColorK0);
        if (mpFlyawayEmitterL.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpFlyawayEmitterL);
        }
        if (mpFlyawayEmitterR.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpFlyawayEmitterR);
            JPABaseEmitter_setDirection(mpFlyawayEmitterR, -1.0f, 1.0f, 0.0f);
        }
    }
}

/* 021FCEE4 */
void daNpc_Bm1_c::delPrtcl_Flyaway() {
    if (mpFlyawayEmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFlyawayEmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpFlyawayEmitterL);
        mpFlyawayEmitterL = nullptr;
    }
    if (mpFlyawayEmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFlyawayEmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpFlyawayEmitterR);
        mpFlyawayEmitterR = nullptr;
    }
}

/* 021FD110 */
void daNpc_Bm1_c::setPrtcl_Land0() {
    if (mAnmNum == 0xD && mpMorf->checkFrame(1.0f)) {
        delPrtcl_Land0();
        mpLandEmitterL = dComIfGp_particle_set(
            0x8276 /* ID_IT_SN_BM_HANEL_LAND00 */, &mArmLPos, &current.angle, nullptr, 0xFF,
            nullptr, fopAcM_GetRoomNo(this), &tevStr.mColorK0, &tevStr.mColorK0);
        mpLandEmitterR =
            dComIfGp_particle_set(0x8276, &mArmRPos, &current.angle, nullptr, 0xFF, nullptr,
                                  fopAcM_GetRoomNo(this), &tevStr.mColorK0, &tevStr.mColorK0);
        if (mpLandEmitterL.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpLandEmitterL);
        }
        if (mpLandEmitterR.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpLandEmitterR);
            JPABaseEmitter_setDirection(mpLandEmitterR, -1.0f, 0.5f, 0.0f);
        }
    }
}

/* 021FD0A0 */
void daNpc_Bm1_c::delPrtcl_Land0() {
    if (mpLandEmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpLandEmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpLandEmitterL);
        mpLandEmitterL = nullptr;
    }
    if (mpLandEmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpLandEmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpLandEmitterR);
        mpLandEmitterR = nullptr;
    }
}

/* 021FE9E0 */
void daNpc_Bm1_c::setPrtcl_Hane0() {
    delPrtcl_Hane0();
    mpFeatherEmitterL =
        dComIfGp_particle_set(0x80D6 /* ID_IT_SN_TORIZOKU_HANE00 */, &current.pos, nullptr, nullptr,
                              0xFF, nullptr, fopAcM_GetRoomNo(this));
    mpFeatherEmitterR = dComIfGp_particle_set(0x80D6, &current.pos, nullptr, nullptr, 0xFF, nullptr,
                                              fopAcM_GetRoomNo(this));
}

/* 021FD25C */
void daNpc_Bm1_c::flwPrtcl_Hane0() {
    if (mpFeatherEmitterL.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeatherEmitterL,
                                         getAnmMtx(mpWingMorf->getModel(), m_wngL3_jnt_num));
    }
    if (mpFeatherEmitterR.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeatherEmitterR,
                                         getAnmMtx(mpWingMorf->getModel(), m_wngR3_jnt_num));
    }
}

/* 021FE0BC */
void daNpc_Bm1_c::delPrtcl_Hane0() {
    if (mpFeatherEmitterL.get() != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(mpFeatherEmitterL);
        mpFeatherEmitterL = nullptr;
    }
    if (mpFeatherEmitterR.get() != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(mpFeatherEmitterR);
        mpFeatherEmitterR = nullptr;
    }
}

/* 021FE8EC */
void daNpc_Bm1_c::setPrtcl_Hane1() {
    delPrtcl_Hane1();
    mpFeather1EmitterL =
        dComIfGp_particle_set(0x80DB /* ID_IT_SN_TORIZOKU_HANE01 */, &current.pos, nullptr, nullptr,
                              0xFF, nullptr, fopAcM_GetRoomNo(this));
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_becomeImmortalEmitter(mpFeather1EmitterL);
    }
    mpFeather1EmitterR = dComIfGp_particle_set(0x80DB, &current.pos, nullptr, nullptr, 0xFF,
                                               nullptr, fopAcM_GetRoomNo(this));
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_becomeImmortalEmitter(mpFeather1EmitterR);
    }
}

/* 021FD30C */
void daNpc_Bm1_c::flwPrtcl_Hane1() {
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeather1EmitterL,
                                         getAnmMtx(mpWingMorf->getModel(), m_wngL3_jnt_num));
    }
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeather1EmitterR,
                                         getAnmMtx(mpWingMorf->getModel(), m_wngR3_jnt_num));
    }
}

/* 021FE10C */
void daNpc_Bm1_c::delPrtcl_Hane1() {
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFeather1EmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpFeather1EmitterL);
        mpFeather1EmitterL = nullptr;
    }
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFeather1EmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpFeather1EmitterR);
        mpFeather1EmitterR = nullptr;
    }
}

/* 021FC36C */
bool daNpc_Bm1_c::decideType(int i_type_param, int i_spawn_cond_param) {
    mType = TYPE_Invalid_e;
    mSpawnCondition = 0;
    mSpecificType = SPECIFIC_TYPE_Invalid_e;
    u32 type = (u32)i_type_param;
    switch ((u16)fpcM_GetName(this)) {
    case 0x146: /* fpcNm_NPC_BM1_e */
        mType = TYPE_Quill_e;
        if (type > 4)
            return false;
        mSpecificType = (s8)type; /* SPECIFIC_TYPE_Quill_0_e + type */
        break;
    case 0x147: /* fpcNm_NPC_BM2_e */
        switch (type) {
        case 0:
            mSpecificType = SPECIFIC_TYPE_Skett_e;
            mType = TYPE_Skett_e;
            break;
        case 1:
            mSpecificType = SPECIFIC_TYPE_Akoot_e;
            mType = TYPE_Akoot_e;
            break;
        default:
            return false;
        }
        break;
    case 0x148: /* fpcNm_NPC_BM3_e */
        switch (type) {
        case 0:
            mSpecificType = SPECIFIC_TYPE_Basht_e;
            mType = TYPE_Basht_e;
            break;
        case 1:
            mSpecificType = SPECIFIC_TYPE_Bisht_e;
            mType = TYPE_Bisht_e;
            break;
        case 2:
            mSpecificType = SPECIFIC_TYPE_Hoskit_e;
            mType = TYPE_Hoskit_e;
            break;
        default:
            return false;
        }
        break;
    case 0x149: /* fpcNm_NPC_BM4_e */
        switch (type) {
        case 0:
            mSpecificType = SPECIFIC_TYPE_Ilari_0xA_e;
            mType = TYPE_Ilari_e;
            break;
        case 1:
            mSpecificType = SPECIFIC_TYPE_Ilari_0xB_e;
            mType = TYPE_Ilari_e;
            break;
        case 2:
            mSpecificType = SPECIFIC_TYPE_Ilari_0xC_e;
            mType = TYPE_Ilari_e;
            break;
        case 3:
            mSpecificType = SPECIFIC_TYPE_Pashli_e;
            mType = TYPE_Pashli_e;
            break;
        default:
            return false;
        }
        break;
    case 0x14A: /* fpcNm_NPC_BM5_e */
        switch (type) {
        case 0:
            mSpecificType = SPECIFIC_TYPE_Namali_e;
            mType = TYPE_Namali_e;
            break;
        case 1:
            mSpecificType = SPECIFIC_TYPE_Kogoli_e;
            mType = TYPE_Kogoli_e;
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }

    if ((u32)i_spawn_cond_param <= 3) {
        mSpawnCondition = Bm1_view<be<s8>>(0x100174F8)[i_spawn_cond_param];
    }
    s32 st = mSpecificType;
    if (st == SPECIFIC_TYPE_Quill_0_e) {
        /* strcpy(mArcName, "Bm2") */
        for (int i = 0; i < 4; i++)
            Bm1_view<be<u8>>(gabi::ea(mArcName))[i] = Bm1_view<be<u8>>(0x100174F4)[i];
        return true;
    }
    if ((u32)st <= 0xF) {
        /* strcpy(mArcName, "Bm") */
        for (int i = 0; i < 3; i++)
            Bm1_view<be<u8>>(gabi::ea(mArcName))[i] = Bm1_view<be<u8>>(0x100174FC)[i];
        return true;
    }
    return false;
}

/* 021FF2B8 */
void daNpc_Bm1_c::eInit_setLocFlag(be<s32>* i_param_1) {
    m896 = false;
    if (i_param_1 == nullptr) {
        return;
    }
    switch ((u32)(s32)*i_param_1) {
    case 2:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        break;
    case 1:
        m896 = true;
        break;
    }
}

/* 021FF2F8 */
void daNpc_Bm1_c::eInit_setShapeAngleY(be<s32>* i_setShapeAngle, s16 i_shapeAngle) {
    mbSetShapeAngle = false;
    if (i_setShapeAngle == nullptr) {
        return;
    }
    bool set = *i_setShapeAngle == 1;
    mbSetShapeAngle = set;
    if (!set) {
        return;
    }
    shape_angle.y = i_shapeAngle;
}

/* 021FF328 */
void daNpc_Bm1_c::eInit_setEvTimer(be<s32>* i_param_1) {
    m86C = 0;
    if (i_param_1 == nullptr) {
        return;
    }
    m86C = (s16)*i_param_1;
}

/* 021FF0CC */
/* returns the cXyz through the hidden result pointer; HD (GHS): a NULL result pointer allocates */
cXyz* daNpc_Bm1_c::eInit_calcRelativPos(cXyz* o_result, cXyz* i_param_2, be<s32>* arg2) {
    s16 angleY;
    if (arg2 != nullptr) {
        angleY = (s16)(shape_angle.y + Bm1_eventParam(arg2)->mHalf);
    } else {
        angleY = shape_angle.y;
    }
    cXyz offset;
    cXyz relativePos;
    f32 z = current.pos.z;
    if (i_param_2 != nullptr) {
        offset.x = i_param_2->x;
        offset.y = i_param_2->y;
        offset.z = i_param_2->z;
    } else {
        offset.x = 0.0f;
        offset.z = 0.0f;
        offset.y = 0.0f;
    }
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), angleY);
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &relativePos);
    cXyz* res = o_result;
    if (res == nullptr) {
        res = (cXyz*)operator_new(0xC);
        if (res == nullptr)
            return res;
    }
    res->x = relativePos.x;
    res->y = relativePos.y;
    res->z = relativePos.z;
    return res;
}

/* 021FF344 */
// HD:the look angle starts as shape_angle.y (GameCube: uninitialised)

void daNpc_Bm1_c::eInit_ATTENTION_(be<s32>* i_prm_0_p, be<s32>* i_prm_1_p, be<s32>* i_prm_2_p,
                                   cXyz* i_offset_p, be<s32>* i_angle_p, be<s32>* i_index_p,
                                   be<s32>* i_timer_p) {
    if (i_prm_0_p == nullptr) {
        return;
    }
    s16 angle = shape_angle.y;
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        break;
    case 1:
        mLookBackState = 1;
        angle = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
        break;
    case 2:
        if (i_offset_p == nullptr) {
            return;
        }
        mLookBackState = 2;
        angle = cLib_targetAngleY(&current.pos, i_offset_p);
        break;
    case 3: {
        mLookBackState = 2;
        cXyz rel;
        eInit_calcRelativPos(&rel, i_offset_p, i_angle_p);
        m82C.copy(rel);
        angle = cLib_targetAngleY(&current.pos, &m82C);
        break;
    }
    case 4:
        if (i_angle_p == nullptr) {
            return;
        }
        mLookBackState = 3;
        angle = Bm1_eventParam(i_angle_p)->mHalf; /* (s16)*i_angle_p */
        break;
    case 5: {
        if (mPathRun.mPath.get() == nullptr) {
            return;
        }
        u8 pointIndex = mPathRun.mIdx;
        if (i_index_p != nullptr) {
            pointIndex = Bm1_eventParam(i_index_p)->mByte;
        }
        cXyz pt;
        Bm1_pathGetPoint(&mPathRun, &pt, pointIndex);
        mLookBackState = 2;
        m82C.copy(pt);
        break;
    }
    case 6: {
        fopAc_ac_c* actor = searchByID(mPartnerProcID);
        if (actor == nullptr) {
            return;
        }
        mLookBackState = 2;
        m82C.copy(actor->eyePos);
        angle = cLib_targetAngleY(&current.pos, &m82C);
        break;
    }
    default:
        mLookBackState = 0;
        break;
    }
    eInit_setLocFlag(i_prm_1_p);
    eInit_setShapeAngleY(i_prm_2_p, angle);
    eInit_setEvTimer(i_timer_p);
}

/* 021FF210 */
void daNpc_Bm1_c::eInit_SET_PLYER_GOL_(be<s32>* i_prm_0_p, cXyz* i_offset_p, be<s32>* i_angle_p) {
    if (i_prm_0_p == nullptr) {
        return;
    }
    cXyz goal;
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        if (i_offset_p != nullptr) {
            goal.x = i_offset_p->x;
            goal.y = i_offset_p->y;
            goal.z = i_offset_p->z;
            dComIfGp_evmng_setGoal(&goal);
        }
        break;
    case 1: {
        cXyz rel;
        eInit_calcRelativPos(&rel, i_offset_p, i_angle_p);
        goal.copy(rel);
        dComIfGp_evmng_setGoal(&goal);
        break;
    }
    }
}

/* 021FEEA0 */
f32 daNpc_Bm1_c::eInit_prmFloat(be<f32>* i_param_1, f32 i_param_2) {
    if (i_param_1 != nullptr) {
        return *i_param_1;
    }
    return i_param_2;
}

/* 021FEECC */
void daNpc_Bm1_c::eInit_FLY_(be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_spd_y_p,
                             be<f32>* i_accel_p, be<f32>* i_acc_y_p) {
    if (i_prm_0_p == nullptr) {
        return;
    }
    m858 = hio_prm(mType).m30; /* m30 */
    if (*i_prm_0_p != 2) {
        mTargetFlySpeed = eInit_prmFloat(i_speed_p, hio_prm(mType).m20); /* m20 */
        mFlySpeedY = eInit_prmFloat(i_spd_y_p, hio_prm(mType).m28);      /* m28 */
        mTargetFlyStep = eInit_prmFloat(i_accel_p, hio_prm(mType).m24);  /* m24 */
        mFlyAccelY = eInit_prmFloat(i_acc_y_p, hio_prm(mType).m2C);      /* m2C */
    }
    m88A = 0;
    mLookBackState = 0;
    switch ((u32)(s32)*i_prm_0_p) {
    case 1:
        m8F4 = 5;
        speedF = mTargetFlySpeed;
        speed.y = mFlySpeedY;
        gravity = 0.0f;
        m889 = 1;
        bm_setFlyAnm();
        break;
    case 3:
        speedF = mTargetFlySpeed;
        break;
    case 4:
        m8F4 = 3;
        mTargetFlySpeed = 0.0f;
    case 5:
        speed.y = 0.0f;
        speedF = mTargetFlySpeed;
        gravity = 0.0f;
        break;
    default:
        m889 = 1;
        m8F4 = 1;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = 0.0f;
    case 2:
        break;
    }
}

/* 021FE70C */
u32 daNpc_Bm1_c::eInit_DEL_ACTOR_() { return fopAcM_delete(this); }

/* 021FF5D0 */
void daNpc_Bm1_c::eInit_WLK_(be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_accel_p,
                             cXyz* i_offset_p, be<s32>* i_angle_p, be<s32>* i_index_p,
                             be<s32>* i_timer_p) {
    if (i_prm_0_p == nullptr) {
        return;
    }
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        if (i_offset_p == nullptr) {
            return;
        }
        mTargetPos.x = i_offset_p->x;
        mTargetPos.y = i_offset_p->y;
        mTargetPos.z = i_offset_p->z;
        break;
    case 1: {
        cXyz rel;
        eInit_calcRelativPos(&rel, i_offset_p, i_angle_p);
        mTargetPos.copy(rel);
        break;
    }
    case 2: {
        if (mPathRun.mPath.get() == nullptr) {
            return;
        }
        u8 pointIdx = mPathRun.mIdx;
        if (i_index_p != nullptr) {
            pointIdx = Bm1_eventParam(i_index_p)->mByte;
        }
        cXyz pt;
        Bm1_pathGetPoint(&mPathRun, &pt, pointIdx);
        mTargetPos.copy(pt);
        break;
    }
    default:
        return;
    }
    eInit_setEvTimer(i_timer_p);
    mTargetFlySpeed = eInit_prmFloat(i_speed_p, hio_prm(mType).m3C); /* m3C */
    mTargetFlyStep = eInit_prmFloat(i_accel_p, hio_prm(mType).m40);  /* m40 */
    f32 m44 = hio_prm(mType).m44;
    mLookBackState = 0;
    m858 = m44;
    setAnm_NUM(0xE, 1);
}

/* 021FE768 */
void daNpc_Bm1_c::eInit_INI_EVN_1_() { m888 = 0; }

/* 021FE710 */
void daNpc_Bm1_c::eInit_SET_NXT_PTH_INF_() {
    if (mPathRun.mPath.get() != nullptr) {
        dPath* path = dNpc_PathRun_nextPath(&mPathRun, fopAcM_GetRoomNo(this));
        if (path != nullptr) {
            dNpc_PathRun_setInfDrct(&mPathRun, path);
        }
    }
}

/* 021FED60 */
void daNpc_Bm1_c::eInit_SET_ANM_(be<s32>* i_param_1) {
    if (i_param_1 != nullptr && (u32)(s32)*i_param_1 < 0x17) {
        setAnm_NUM(*i_param_1, 1);
    }
}

/* 021FED7C */
void daNpc_Bm1_c::eInit_MOV_PTH_POINT_(be<s32>* i_prm_0_p, be<s32>* i_anm_no_p, be<s32>* i_index_p,
                                       be<s32>* i_angle_p) {
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    u8 idx = mPathRun.mIdx;
    if (i_index_p != nullptr) {
        u32 i = Bm1_eventParam(i_index_p)->mByte; /* (u8)*i_index_p */
        u32 max = Bm1_pathMaxPoint(&mPathRun);
        idx = (u8)(i >= max ? max : i); /* cLib_maxLimit */
        mPathRun.mIdx = idx;
    }
    cXyz pos;
    Bm1_pathGetPoint(&mPathRun, &pos, idx);
    current.pos.copy(pos);
    Bm1_pathNextIdxAuto(&mPathRun);
    Bm1_pathGetPoint(&mPathRun, &pos, mPathRun.mIdx);
    cXyz pathpos;
    pathpos.copy(pos);
    current.angle.y = cLib_targetAngleY(&current.pos, &pathpos);
    if (i_prm_0_p != nullptr) {
        if (*i_prm_0_p == 1) {
            current.angle.y =
                cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
        }
    } else if (i_angle_p != nullptr) {
        current.angle.y = Bm1_eventParam(i_angle_p)->mHalf; /* (s16)*i_angle_p */
    }
    eInit_SET_ANM_(i_anm_no_p);
}

/* 021FF888 */
void daNpc_Bm1_c::event_actionInit(int arg0) {
    cXyz* offset_p = (cXyz*)dComIfGp_evmng_getMySubstanceP(arg0, "Offst", 1);
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "ActNo", 3);
    be<s32>* prm_0_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "prm_0", 3);
    be<s32>* prm_1_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "prm_1", 3);
    be<s32>* prm_2_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "prm_2", 3);
    be<s32>* angle_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Angle", 3);
    be<s32>* index_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Index", 3);
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Timer", 3);
    be<s32>* anmno_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, "AnmNo", 3);
    be<f32>* speed_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Speed", 0);
    be<f32>* accel_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Accel", 0);
    be<f32>* spd_y_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Spd_y", 0);
    be<f32>* acc_y_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, "Acc_y", 0);
    if (act_no_p == nullptr) {
        return;
    }
    s8 actNo = (s8)*act_no_p;
    mActNo = actNo;
    switch ((u32)(s32)actNo) {
    case 4:
        eInit_DEL_ACTOR_();
        break;
    case 0x10:
        eInit_SET_NXT_PTH_INF_();
        break;
    case 0x14:
        eInit_INI_EVN_1_();
        break;
    case 0x15:
        eInit_MOV_PTH_POINT_(prm_0_p, anmno_p, index_p, angle_p);
        break;
    case 0x16:
    case 0x17:
        eInit_FLY_(prm_0_p, speed_p, spd_y_p, accel_p, acc_y_p);
        break;
    case 0x18:
        eInit_SET_PLYER_GOL_(prm_0_p, offset_p, angle_p);
        break;
    case 0x19:
        eInit_ATTENTION_(prm_0_p, prm_1_p, prm_2_p, offset_p, angle_p, index_p, timer_p);
        break;
    case 0x1A:
        eInit_SET_ANM_(anmno_p);
        break;
    case 0x1B:
        eInit_WLK_(prm_0_p, speed_p, accel_p, offset_p, angle_p, index_p, timer_p);
        break;
    }
}

/* 021FFCA8 */
u32 daNpc_Bm1_c::eMove_ATTENTION_() {
    if (m86C >= 0) {
        return cLib_calcTimer_g(&m86C) == 0;
    }
    return (u8)(m_jnt.mbTrn ^ 1); /* m_jnt.trnChk() == 0 */
}

/* 021FFCA0 */
u8 daNpc_Bm1_c::eMove_KMA_FLY_() { return m88A; }

/* 021FFC84 */
bool daNpc_Bm1_c::eMove_FLY_() {
    m88A = 0;
    return m8F4 == 0;
}

/* 021FFCFC */
bool daNpc_Bm1_c::eMove_WLK_() {
    bool is_terminate = cLib_calcTimer_g(&m86C) == 0;
    if (is_terminate) {
        setAnm_NUM(4, 1);
        speedF = 0.0f;
        mTargetFlySpeed = 0.0f;
    }
    return is_terminate;
}

/* 021FFD64 */
/* the eMove_* results are returned as is: typed u32 */
u32 daNpc_Bm1_c::event_action() {
    switch ((u32)(s32)mActNo) {
    case 0x16:
        return Bm1_moveFlyResult(this); /* eMove_FLY_() */
    case 0x17:
        return Bm1_moveKmaFlyResult(this); /* eMove_KMA_FLY_() */
    case 0x19:
        return Bm1_moveAttentionResult(this); /* eMove_ATTENTION_() */
    case 0x1B:
        return Bm1_moveWalkResult(this); /* eMove_WLK_() */
    default:
        return 1;
    }
}

/* 021FFC48 */
void daNpc_Bm1_c::cut_init_360_TRN(int i_staffIdx) {
    /* the argument is unused */
    setAnm_NUM(0x13, 1);
    mLookBackState = 0;
}

/* 021FFDCC */
bool daNpc_Bm1_c::cut_move_360_TRN() {
    if (mbMorfAnimStopped) {
        current.angle.y = current.angle.y + -0x8000;
        setAnm_NUM(4, 1);
        mpMorf->setMorf(0.0f);
        if (mbHasArms) {
            mpArmMorf->setMorf(0.0f);
        } else {
            mpWingMorf->setMorf(0.0f);
        }
        mpHeadMorf->setMorf(0.0f);
        return true;
    }
    return false;
}

/* 021FFED0 */
void daNpc_Bm1_c::privateCut(int arg0) {
    /* static char* a_cut_tbl[] = {"ACTION", "360_TRN"} (.data 0x101BC98C) */
    if (arg0 == -1) {
        return;
    }
    s8 idx =
        Bm1_getMyActIndex(dComIfGp_getPEvtManager(), arg0, 0x101BC98C, 2, 1, 0); /* getMyActIdx */
    mActionIndex = idx;
    void* evmng = dComIfGp_getPEvtManager();
    if (idx == -1) {
        Bm1_cutEnd(evmng, arg0); /* dComIfGp_evmng_cutEnd */
        return;
    }
    if (Bm1_isAdvance(evmng, arg0)) { /* dComIfGp_evmng_getIsAddvance */
        switch ((u32)(s32)mActionIndex) {
        case 0:
            event_actionInit(arg0);
            break;
        case 1:
            cut_init_360_TRN(arg0);
            break;
        }
    }
    u32 cVar3;
    switch ((u32)(s32)mActionIndex) {
    case 0:
        cVar3 = event_action();
        break;
    case 1:
        cVar3 = cut_move_360_TRN();
        break;
    default:
        cVar3 = 1;
        break;
    }
    if (cVar3) {
        dComIfGp_evmng_cutEnd(arg0);
    }
}

/* 021FE6CC */
void daNpc_Bm1_c::endEvent() {
    dComIfGp_event_reset();
    m8F7 = 0xFF;
}

/* 021FE68C */
BOOL daNpc_Bm1_c::isEventEntry() {
    const char* name = Bm1_view<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 021FC758 */
BOOL daNpc_Bm1_c::set_action(Bm1ActionFunc* i_newProcFunc, void* i_argsP) {
    Bm1ActionFunc* cur = &mCurrActionFunc;
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
        m904 = 9;
        Bm1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m904 = 0;
    Bm1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 021FE4DC */
/* returns the mbInDemo byte as is (typed u8) */
u8 daNpc_Bm1_c::demo() {
    if (demoActorID == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    u8 id = demoActorID;
    mbInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = *Bm1_view<be<u32>>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = *Bm1_view<be<u32>>(0x101D5FFC);
        }
        demo_actor = Bm1_demoGetActor(obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    // HD:demo_actor is checked for NULL

    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = Bm1_demoGetBtp(demo_actor, mArcName); /* getP_BtpData */
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = Bm1_modelData(mpHeadMorf->getModel());
            if (mDoExt_btpAnm_init(mHeadBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 1;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, 0, 0);
    return mbInDemo;
}

/* 021FE17C */
BOOL daNpc_Bm1_c::_delete() {
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        if (mpHeadMorf.get() != nullptr) {
            mpHeadMorf->stopZelAnime();
        }
        if (mpWingMorf.get() != nullptr) {
            mpWingMorf->stopZelAnime();
        }
        if (mpArmMorf.get() != nullptr) {
            mpArmMorf->stopZelAnime();
        }
    }
    delPrtcl_Flyaway();
    delPrtcl_Land0();
    delPrtcl_Hane0();
    delPrtcl_Hane1();
    return TRUE;
}

/* 021FC368 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Bm1_c*>(i_this)->CreateHeap();
}

/* 021FDF08 */
cPhs_State daNpc_Bm1_c::_create() {
    /* fopAcM_ct(this, daNpc_Bm1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Bm1_constructNpc(
                this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = BM1_VTBL;
            Bm1_constructBtp(mHeadBtpAnm);     /* mDoExt_btpAnm::mDoExt_btpAnm */
            Bm1_constructEventCut(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u32 param = fopAcM_GetParam(this);
    if (!decideType(param & 0xFF, (param >> 8) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    m87F = state == cPhs_COMPLEATE_e;
    if (!m87F) {
        return state;
    }

    if (!fopAcM_entrySolidHeap(this, 0x021FC368 /* CheckCreateHeap */, a_size_tbl[mType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    if (mSpecificType == SPECIFIC_TYPE_Quill_0_e) {
        fopAcM_setCullSizeBox(this, -250.0f, -20.0f, -200.0f, 250.0f, 500.0f, 200.0f);
    } else {
        fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 70.0f, 220.0f, 70.0f);
    }
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 021FB010 */
J3DModelData* daNpc_Bm1_c::create_Anm() {
    J3DModelData* a_mdl_dat;
    // HD:types 2..6 (Akoot .. Hoskit) use "bm02.bmt"

    if ((u32)(s32)mType >= 2 && (u32)(s32)mType <= 6) {
        a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm02.bmt");
    } else {
        a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm.bdl");
    }
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x15DC, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x15DC, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)Bm1_resource(mArcName, "bm_dwait.bck");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm,
                                    2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020203);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        McaMorf_delete(mpMorf);
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head");
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x15F6, "m_hed_jnt_num >= 0");
    m_nec_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "neck");
    if (m_nec_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x15F9, "m_nec_jnt_num >= 0");
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "backbone");
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x15FC, "m_bbone_jnt_num >= 0");
    m_arm_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armL");
    if (m_arm_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x15FF, "m_arm_L_jnt_num >= 0");
    m_arm_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armR");
    if (m_arm_R_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1602, "m_arm_R_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021FB354 */
J3DModelData* daNpc_Bm1_c::create_hed_Anm() {

    const char* name = a_headBDLName_TBL[mSpecificType];
    J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, name);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1635, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)Bm1_resource(mArcName, "bmhead01_dwait.bck");
    mpHeadMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1,
                                        1, nullptr, 0x80000, 0x11020022);
    if (mpHeadMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpHeadMorf->getModel() == nullptr) {
        McaMorf_delete(mpHeadMorf);
        mpHeadMorf = nullptr;
        return nullptr;
    }
    return a_mdl_dat;
}

/* 021FB798 */
J3DModelData* daNpc_Bm1_c::create_wng_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bmwing.bdl");
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1659, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)Bm1_resource(mArcName, "bmwing_dwait.bck");
    mpWingMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1,
                                        1, nullptr, 0x80000, 0x11020203);
    if (mpWingMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpWingMorf->getModel() == nullptr) {
        McaMorf_delete(mpWingMorf);
        mpWingMorf = nullptr;
        return nullptr;
    }
    m_wngL1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "wingLloc");
    if (m_wngL1_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1673, "m_wngL1_jnt_num >= 0");
    m_wngL3_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "wingL3");
    if (m_wngL3_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1676, "m_wngL3_jnt_num >= 0");
    m_wngR1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "wingRloc");
    if (m_wngR1_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1679, "m_wngR1_jnt_num >= 0");
    m_wngR3_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "wingR3");
    if (m_wngR3_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x167C, "m_wngR3_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021FBA3C */
J3DModelData* daNpc_Bm1_c::create_arm_Anm() {
    J3DModelData* a_mdl_dat;
    // HD:types 2..6 use "bmarm02.bmt"

    if ((u32)(s32)mType >= 2 && (u32)(s32)mType <= 6) {
        a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bmarm02.bmt");
    } else {
        a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bmarm.bdl");
    }
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16AD, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)Bm1_resource(mArcName, "bmarm_wait01.bck");
    mpArmMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1,
                                       nullptr, 0x80000, 0x11020203);
    if (mpArmMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpArmMorf->getModel() == nullptr) {
        McaMorf_delete(mpArmMorf);
        mpArmMorf = nullptr;
        return nullptr;
    }
    m_armL1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armLloc");
    if (m_armL1_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16C7, "m_armL1_jnt_num >= 0");
    m_armR1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armRloc");
    if (m_armR1_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16CA, "m_armR1_jnt_num >= 0");
    m_armL2_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armL2");
    if (m_armL2_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16CD, "m_armL2_jnt_num >= 0");
    m_armR2_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "armR2");
    if (m_armR2_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16D0, "m_armR2_jnt_num >= 0");
    m_hnd_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "handR");
    if (m_hnd_R_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16D3, "m_hnd_R_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021FBD80 */
bool daNpc_Bm1_c::create_itm_Mdl() {
    mpBagModel = nullptr;
    mpStickModel = nullptr;
    mpKnifeModel = nullptr;
    mpBinderModel = nullptr;
    if (mType == TYPE_Pashli_e || mType == TYPE_Namali_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm_binder.bdl");
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x16FD, "a_mdl_dat != (0)");
        mpBinderModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpBinderModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Quill_e || mType == TYPE_Ilari_e || mType == TYPE_Pashli_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm_bag.bdl");
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x170C, "a_mdl_dat != (0)");
        mpBagModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpBagModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Akoot_e || mType == TYPE_Skett_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm_knife.bdl");
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1719, "a_mdl_dat != (0)");
        mpKnifeModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpKnifeModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Basht_e || mType == TYPE_Bisht_e || mType == TYPE_Hoskit_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)Bm1_resource(mArcName, "bm_stick.bdl");
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1727, "a_mdl_dat != (0)");
        mpStickModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpStickModel.get() == nullptr) {
            return false;
        }
    }
    return true;
}

/* 021FBFF0 */
BOOL daNpc_Bm1_c::CreateHeap() {
    J3DModelData* modeldat;
    J3DModelData* anm_model = create_Anm();
    if (!anm_model) {
        return FALSE;
    }
    J3DModelData* head_modeldat = create_hed_Anm();
    if (!head_modeldat) {
        mpMorf = nullptr;
        return FALSE;
    }
    mBtpNum = 0;
    if (!iniTexPttrnAnm(false) || (modeldat = create_wng_Anm()) == nullptr) {
        mpHeadMorf = nullptr;
        mpMorf = nullptr;
        return FALSE;
    }
    J3DModelData* arm_anmdata = create_arm_Anm();
    if (arm_anmdata != nullptr && create_itm_Mdl()) {
        for (u16 i = 0; i < J3DModelData_getJointNum(modeldat); i++) {
            if ((i == (u32)(s32)m_wngL1_jnt_num) || (i == (u32)(s32)m_wngL3_jnt_num) ||
                (i == (u32)(s32)m_wngR1_jnt_num) || (i == (u32)(s32)m_wngR3_jnt_num)) {
                setJointCallBack(Bm1_modelData(mpWingMorf->getModel()), i,
                                 0x021FAB78 /* nodeCallBack_Wng */);
            }
        }
        J3DModel_setUserArea(mpWingMorf->getModel(), this);
        for (u16 i = 0; i < J3DModelData_getJointNum(arm_anmdata); i++) {
            if ((i == (u32)(s32)m_armL1_jnt_num) || (i == (u32)(s32)m_armL2_jnt_num) ||
                (i == (u32)(s32)m_armR1_jnt_num) || (i == (u32)(s32)m_armR2_jnt_num)) {
                setJointCallBack(Bm1_modelData(mpArmMorf->getModel()), i,
                                 0x021FADA0 /* nodeCallBack_Arm */);
            }
        }
        J3DModel_setUserArea(mpArmMorf->getModel(), this);
        for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
            if ((i == (u32)(s32)m_hed_jnt_num) || (i == (u32)(s32)m_nec_jnt_num) ||
                (i == (u32)(s32)m_bbone_jnt_num) || (i == (u32)(s32)m_arm_L_jnt_num) ||
                i == (u32)(s32)m_arm_R_jnt_num) {
                setJointCallBack(Bm1_modelData(mpMorf->getModel()), i,
                                 0x021FAFC8 /* nodeCallBack_Bm1 */);
            }
        }
        J3DModel_setUserArea(mpMorf->getModel(), this);
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    } else {
        mpMorf = nullptr;
        mpWingMorf = nullptr;
        mpHeadMorf = nullptr;
        return FALSE;
    }
}

/* 021FE0B8 */
static cPhs_State daNpc_Bm1_Create(fopAc_ac_c* i_this) { return ((daNpc_Bm1_c*)i_this)->_create(); }

/* 021FE214 */
static BOOL daNpc_Bm1_Delete(daNpc_Bm1_c* i_this) { return i_this->_delete(); }

/* WWHD glue
str 10017288 ".btp" in 021FB5B8
str 100172AC "bmhead10" in 021FB5B8
str 100172A0 "bmhead08" in 021FB5B8
str 10017220 ".btp" in 021FB4C4
str 10017228 "bmhead01" in 021FB4C4
str 10017664 ".bck" in 021FE808
str 100177C4 ".bck" in 021FE87C
str 10017558 ".bck" in 021FE794
data a_size_tbl 101BC68C
data a_att_dis_TBL 101BC674
data a_staff_tbl 101BC634
data l_evn_tbl 101BC580
data a_BTPName2_TBL 101BC5F4
data a_headBDLName_TBL 101BC5B0
str 10017950 "Acc_y" in 021FF888
str 10017948 "Spd_y" in 021FF888
str 10017940 "Accel" in 021FF888
str 10017938 "Speed" in 021FF888
str 10017930 "AnmNo" in 021FF888
str 10017928 "Timer" in 021FF888
str 10017920 "Index" in 021FF888
str 10017918 "Angle" in 021FF888
str 10017910 "prm_2" in 021FF888
str 10017908 "prm_1" in 021FF888
str 10017900 "prm_0" in 021FF888
str 100178F8 "ActNo" in 021FF888
str 100178F0 "Offst" in 021FF888
str 10017028 "m_object != (0)" in 021FE4DC
str 1001706C "d_demo.h" in 021FE4DC
str 100174AC "a_mdl_dat != (0)" in 021FBD80
str 1001748C "d_a_npc_bm1.cpp" in 021FBD80
str 100174DC "bm_stick.bdl" in 021FBD80
str 100174CC "bm_knife.bdl" in 021FBD80
str 100174C0 "bm_bag.bdl" in 021FBD80
str 1001749C "bm_binder.bdl" in 021FBD80
str 10017474 "m_hnd_R_jnt_num >= 0" in 021FBA3C
str 100173C4 "d_a_npc_bm1.cpp" in 021FBA3C
str 100173BC "handR" in 021FBA3C
str 1001745C "m_armR2_jnt_num >= 0" in 021FBA3C
str 100173B4 "armR2" in 021FBA3C
str 10017444 "m_armL2_jnt_num >= 0" in 021FBA3C
str 100173AC "armL2" in 021FBA3C
str 1001742C "m_armR1_jnt_num >= 0" in 021FBA3C
str 100173A4 "armRloc" in 021FBA3C
str 10017414 "m_armL1_jnt_num >= 0" in 021FBA3C
str 1001739C "armLloc" in 021FBA3C
str 10017400 "bmarm_wait01.bck" in 021FBA3C
str 100173EC "a_mdl_dat != (0)" in 021FBA3C
str 100173E0 "bmarm.bdl" in 021FBA3C
str 100173D4 "bmarm02.bmt" in 021FBA3C
str 10017384 "m_wngR3_jnt_num >= 0" in 021FB798
str 100172E0 "d_a_npc_bm1.cpp" in 021FB798
str 100172D8 "wingR3" in 021FB798
str 1001736C "m_wngR1_jnt_num >= 0" in 021FB798
str 10017360 "wingRloc" in 021FB798
str 10017348 "m_wngL3_jnt_num >= 0" in 021FB798
str 100172D0 "wingL3" in 021FB798
str 10017330 "m_wngL1_jnt_num >= 0" in 021FB798
str 10017324 "wingLloc" in 021FB798
str 10017310 "bmwing_dwait.bck" in 021FB798
str 100172FC "a_mdl_dat != (0)" in 021FB798
str 100172F0 "bmwing.bdl" in 021FB798
str 100172B8 "m_hed_tex_pttrn != (0)" in 021FB5B8
str 10017290 "d_a_npc_bm1.cpp" in 021FB5B8
str 1001718C "bmhead01_dwait.bck" in 021FB354
str 10017178 "a_mdl_dat != (0)" in 021FB354
str 10017168 "d_a_npc_bm1.cpp" in 021FB354
str 10017150 "m_arm_R_jnt_num >= 0" in 021FB010
str 100170AC "d_a_npc_bm1.cpp" in 021FB010
str 100170A4 "armR" in 021FB010
str 10017138 "m_arm_L_jnt_num >= 0" in 021FB010
str 1001709C "armL" in 021FB010
str 10017120 "m_bbone_jnt_num >= 0" in 021FB010
str 10017114 "backbone" in 021FB010
str 10017100 "m_nec_jnt_num >= 0" in 021FB010
str 10017094 "neck" in 021FB010
str 100170EC "m_hed_jnt_num >= 0" in 021FB010
str 1001708C "head" in 021FB010
str 100170DC "bm_dwait.bck" in 021FB010
str 100170C8 "a_mdl_dat != (0)" in 021FB010
str 10017084 "bm.bdl" in 021FB010
str 100170BC "bm02.bmt" in 021FB010
ext 025447C8 BOOL Bm1_isAdvance(void* manager, s32 staff)
ext 02543280 void Bm1_cutEnd(void* manager, s32 staff)
ext 02542EDC s8 Bm1_getMyActIndex(void* manager, s32 staff, u32 table, s32 count, s32 force, s32 extra)
ext 02527828 J3DAnmTexPattern* Bm1_demoGetBtp(void* actor, const char* archive)
ext 02526E70 void* Bm1_demoGetActor(u32 object, u32 id)
ext 025D54C4 void Bm1_searchById(fpc_ProcID id, gptr<fopAc_ac_c>* result)
ext 0259F740 void Bm1_constructEventCut(dNpc_EventCut_c* cut)
ext 025E7820 void Bm1_constructBtp(void* animation)
ext 025A1458 void Bm1_constructNpc(daNpc_Bm1_c* npc)
ext 028249B0 void Bm1_setEmitterMatrix(Mtx34* matrix, u32 rotation, u32 translation)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 027F3F8C u32 Bm1_modelNameHeader(J3DModelData* data)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 028F040C char* strcpy_g(char* d, const char* s)
ext 02055B64 s16 cLib_calcTimer_g(be<s16>* t)
ext 024EF130 s8 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 u8 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z)
ext 0259E6D0 void dNpc_PathRun_setInf(dNpc_PathRun_bm1* p, u32 idx, s8 room, u8 fwd)
ext 0259E744 dPath* dNpc_PathRun_nextPath(dNpc_PathRun_bm1* p, s8 room)
ext 0259E730 void dNpc_PathRun_setInfDrct(dNpc_PathRun_bm1* p, dPath* path)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 0259D344 BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, u32 name, s32 snd, const char* arc)
ext 0259EDB8 u32 Bm1_pathMaxPoint(dNpc_PathRun_bm1* p)
ext 0259E778 void Bm1_pathGetPoint(dNpc_PathRun_bm1* p, cXyz* out, u8 idx)
ext 0259ED58 void Bm1_pathNextIdxAuto(dNpc_PathRun_bm1* p)
ext 021FB5B8 u32 Bm1_setBtpResult(daNpc_Bm1_c* npc, u32 modify, s32 btp)
ext 021FCB00 u32 Bm1_initSkettResult(daNpc_Bm1_c* npc)
ext 021FCB50 u32 Bm1_initBashtResult(daNpc_Bm1_c* npc)
ext 021FCD4C u32 Bm1_initPashliResult(daNpc_Bm1_c* npc)
ext 021FFC84 u32 Bm1_moveFlyResult(daNpc_Bm1_c* npc)
ext 021FFCA0 u32 Bm1_moveKmaFlyResult(daNpc_Bm1_c* npc)
ext 021FFCA8 u32 Bm1_moveAttentionResult(daNpc_Bm1_c* npc)
ext 021FFCFC u32 Bm1_moveWalkResult(daNpc_Bm1_c* npc)
*/
