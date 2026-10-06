/**
 * d_a_npc_hi1.cpp (WWHD)
 * NPC - King of Hyrule
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_hi1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 * Compiler-generated functions (HIO constructor, __sinit, destructors) are written from the WWHD
 * code.
 */
#include "d/actor/d_a_npc_hi1.h"

#define SAFESTRING_VTBL 0x1001A3CC /* this TU's sead::SafeString vtable */
#define HI1_VTBL 0x1001A548        /* daNpc_Hi1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_zk1.cpp) ---- */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* resources by id; the key is the actor's own archive name (mArcName) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* J3DAnmBase::getFrameMax through the vtable (slot +0x14) */
static inline s32 J3DAnm_getFrameMax(u32 anm) {
    u32 vt = gabi::load<u32>(anm + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), anm);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (out-of-line copy in another TU) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 025E7CE0 mDoExt_btkAnm::init (same argument list as btpAnm::init) */
static inline s32 mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D8BC dNpc_chkAttn(actor, cXyz pos (pointer to a copy), f32, f32, f32, bool) */
static inline BOOL dNpc_chkAttn(fopAc_ac_c* a, cXyz* pos, f32 r0, f32 r1, f32 h, u32 flag) {
    return gabi::call<BOOL>(0x0259D8BC, a, pos, r0, r1, h, flag);
}
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline void* dDemo_actor_getP_BtpData(void* ac, const char* arc) { return gabi::call<void*>(0x02527828, ac, arc); }
static inline void* dDemo_actor_getP_BtkData(void* ac, const char* arc) { return gabi::call<void*>(0x025279C8, ac, arc); }
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_o(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }

/* GHS pointer to member function call */
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
enum : u32 { PMF_wait_action1 = 0x1001A3B0 };

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
/* daNpc_Hi1_HIO_c, HD: vtable at 0 */
struct daNpc_Hi1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mJnt[9];      /* dNpc_JntCtrl_c::setParam arguments */
        /* 0x12 */ be<s16> mMaxTurnStep; /* lookBack (GameCube field_12) */
        /* 0x14 */ be<f32> mAttentionYOffset;
        /* 0x18 */ be<u8> mDebug;        /* debug colours in _draw (GameCube f32 field_18) */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Hi1_HIO_c, 0x28);
static daNpc_Hi1_HIO_c& l_HIO() { return *gabi::at<daNpc_Hi1_HIO_c>(0x10467164); }

/* 0223BD84 */
void daNpc_Hi1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0223BD84, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(20.0f, 30.0f, 0.0f): guard 0x104671A8, object 0x1046719C */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x1046719C);
    if (gabi::load<u32>(0x104671A8) == 0) {
        gabi::store<u32>(0x104671A8, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->y = 30.0f;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mHeadPos.x = stk->m[0][3]; /* mDoMtx_stack_c::multVecZero(&field_0x768) */
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x0223BD84, &daNpc_Hi1_c::_nodeCB_Head);

/* 0223BEFC */
static BOOL nodeCB_Head(J3DNode* i_node, int i_param) {
    WWHD_FUNC(0x0223BEFC, BOOL, i_node, i_param);
    if (i_param == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Hi1_c* actor = gabi::at<daNpc_Hi1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (actor != nullptr) {
            actor->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x0223BEFC, nodeCB_Head);

/* 0223BF44 */
void daNpc_Hi1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0223BF44, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x0223BF44, &daNpc_Hi1_c::_nodeCB_BackBone);

/* 0223C068 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_param) {
    WWHD_FUNC(0x0223C068, BOOL, i_node, i_param);
    if (i_param == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C));
        daNpc_Hi1_c* actor = gabi::at<daNpc_Hi1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (actor != nullptr) {
            actor->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x0223C068, nodeCB_BackBone);

/* 0223C0B0 (unnamed by the matcher) */
s32 daNpc_Hi1_c::btpResID(int) {
    WWHD_FUNC(0x0223C0B0, s32, this, 0);
    /* a_resID_tbl (.data 0x1001A43C): one entry, HD reads [0] */
    return gabi::load<s32>(0x1001A43C);
}
VERIFY(0x0223C0B0, &daNpc_Hi1_c::btpResID);

/* 0223C0BC */
bool daNpc_Hi1_c::setBtp(s32 param_1, u32 param_2) {
    WWHD_FUNC(0x0223C0BC, bool, this, param_1, param_2);
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(param_1));
    if (a_btp == nullptr) /* JUT_ASSERT(0x195, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x1001A444), 0x195, STR(0x1001A454));
    field_0x7BE = (s8)param_1;
    mBtpAnmFrame = 0;
    mTimer1 = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(model), a_btp, 1, 0 /* EMode_RESET */, 1.0f, 0, -1, param_2, 0) != 0;
}
VERIFY(0x0223C0BC, &daNpc_Hi1_c::setBtp);

/* 0223C1A8 (unnamed by the matcher) */
s32 daNpc_Hi1_c::btkResID(int) {
    WWHD_FUNC(0x0223C1A8, s32, this, 0);
    return gabi::load<s32>(0x1001A464); /* a_resID_tbl[0] */
}
VERIFY(0x0223C1A8, &daNpc_Hi1_c::btkResID);

/* 0223C1B4 */
bool daNpc_Hi1_c::setBtk(s32 param_1, u32 param_2) {
    WWHD_FUNC(0x0223C1B4, bool, this, param_1, param_2);
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btk = dComIfG_getObjectIDRes(mArcName, btkResID(param_1));
    if (a_btk == nullptr) /* JUT_ASSERT(0x1ad, a_btk != NULL) */
        JUT_ASSERT_fail(STR(0x1001A468), 0x1AD, STR(0x1001A478));
    field_0x7BF = (s8)param_1;
    mBtkAnmFrame = 0;
    return mDoExt_btkAnm_init(mBtkAnm, J3DModel_getModelData_l(model), a_btk, 1, 0 /* EMode_RESET */, 1.0f, 0, -1, param_2, 0) != 0;
}
VERIFY(0x0223C1B4, &daNpc_Hi1_c::setBtk);

/* 0223C29C */
bool daNpc_Hi1_c::init_texPttrnAnm(s32 param_1, u32 param_2) {
    WWHD_FUNC(0x0223C29C, bool, this, param_1, param_2);
    if (setBtp(param_1, param_2) == false) {
        return false;
    }
    /* a_btk_num_tbl (.rodata 0x1001A488): one entry, HD reads [0] */
    return setBtk(gabi::load<s8>(0x1001A488), param_2);
}
VERIFY(0x0223C29C, &daNpc_Hi1_c::init_texPttrnAnm);

/* 0223C308 */
BOOL daNpc_Hi1_c::bodyCreateHeap() {
    WWHD_FUNC(0x0223C308, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 2 /* dRes_ID_HI_BDL_HI_e */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1487, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001A494), 0x5CF, STR(0x1001A4B0));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        /* HD: mpMorf is deleted (virtual deleting destructor, vtable at +0, slot +0xC) */
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A48C) /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(1509, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001A494), 0x5E5, STR(0x1001A4C4));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A4A4) /* "backbone1" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(1511, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001A494), 0x5E7, STR(0x1001A4D8));
    /* getJointNodePointer(jnt)->setCallBack(...): a joint index out of range uses the table base */
    {
        J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
        u32 i = (u16)(s32)m_hed_jnt_num;
        u32 n = gabi::load<u32>(gabi::ea(md) + 4);
        u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
        if (i < n)
            joint += i * 0x1C;
        gabi::store<u32>(joint + 8, 0x0223BEFC /* nodeCB_Head */);
    }
    {
        J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
        u32 i = (u16)(s32)m_bbone_jnt_num;
        u32 n = gabi::load<u32>(gabi::ea(md) + 4);
        u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
        if (i < n)
            joint += i * 0x1C;
        gabi::store<u32>(joint + 8, 0x0223C068 /* nodeCB_BackBone */);
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x0223C308, &daNpc_Hi1_c::bodyCreateHeap);

/* 0223C5C8 */
BOOL daNpc_Hi1_c::CreateHeap() {
    WWHD_FUNC(0x0223C5C8, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 110.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x0223C5C8, &daNpc_Hi1_c::CreateHeap);

/* 0223C658 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* ac) {
    WWHD_FUNC(0x0223C658, BOOL, ac);
    return ((daNpc_Hi1_c*)ac)->CreateHeap();
}
VERIFY(0x0223C658, CheckCreateHeap);

/* 0223C65C */
bool daNpc_Hi1_c::decideType(int param_1) {
    WWHD_FUNC(0x0223C65C, bool, this, param_1); /* the parameter is unused */
    if (field_0x7C5 > 0) {
        return true;
    }
    field_0x7C5 = 1;
    field_0x7C6 = 0;
    /* strcpy(mArcName, "Hi") (.rodata 0x1001A4F8, three bytes) */
    u8 c0 = gabi::load<u8>(0x1001A4F8), c1 = gabi::load<u8>(0x1001A4F9), c2 = gabi::load<u8>(0x1001A4FA);
    gabi::store<u8>(gabi::ea(mArcName), c0);
    gabi::store<u8>(gabi::ea(mArcName) + 1, c1);
    gabi::store<u8>(gabi::ea(mArcName) + 2, c2);
    bool temp = false;
    if (field_0x7C5 != -1 && field_0x7C6 != -1) {
        temp = true;
    }
    return temp;
}
VERIFY(0x0223C65C, &daNpc_Hi1_c::decideType);

/* 0223C6C0 */
BOOL daNpc_Hi1_c::set_action(ProcFunc_l* i_action, void* param_2) {
    WWHD_FUNC(0x0223C6C0, BOOL, this, i_action, param_2);
    ProcFunc_l* cur = &mCurrActionFunc;
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
        field_0x7C7 = 9;
        pmf_call(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7C7 = 0;
    pmf_call(this, cur, param_2);
    return TRUE;
}
VERIFY(0x0223C6C0, &daNpc_Hi1_c::set_action);

/* 0223C7EC */
bool daNpc_Hi1_c::init_HI1_0() {
    WWHD_FUNC(0x0223C7EC, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x0223C7EC, &daNpc_Hi1_c::init_HI1_0);

/* 0223C82C */
void daNpc_Hi1_c::play_btp_anm() {
    WWHD_FUNC(0x0223C82C, void, this);
    u8 frame = (u8)J3DAnm_getFrameMax(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10));
    if (field_0x7BE != 0 || cLib_calcTimer(&mTimer1) == 0) {
        u8 f = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = f;
        if (f >= frame) {
            if (field_0x7BE == 0) {
                mTimer1 = (s16)cLib_getRndValue(0x3C, 0x5A);
                frame = 0;
            }
            mBtpAnmFrame = frame;
        }
    }
}
VERIFY(0x0223C82C, &daNpc_Hi1_c::play_btp_anm);

/* 0223C8CC (unnamed by the matcher) */
void daNpc_Hi1_c::play_btk_anm() {
    WWHD_FUNC(0x0223C8CC, void, this);
    u8 frame = (u8)J3DAnm_getFrameMax(gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68));
    u8 f = (u8)(mBtkAnmFrame + 1);
    if (f >= frame) {
        f = frame;
    }
    mBtkAnmFrame = f;
}
VERIFY(0x0223C8CC, &daNpc_Hi1_c::play_btk_anm);

/* 0223C928 */
void daNpc_Hi1_c::play_animation() {
    WWHD_FUNC(0x0223C928, void, this);
    u32 mtrlSndId = 0;
    play_btp_anm();
    play_btk_anm();
    if (gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28) & 0x20 /* mObjAcch.ChkGroundHit() */) {
        mtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    field_0x7AC = (s8)gabi::call<BOOL>(0x025E535C, mpMorf.get(), &eyePos, mtrlSndId, reverb); /* mpMorf->play() */
    if (mpMorf->getFrame() < mFrame) {
        field_0x7AC = 1;
    }
    mFrame = mpMorf->getFrame();
}
VERIFY(0x0223C928, &daNpc_Hi1_c::play_animation);

/* 0223C9D4 */
void daNpc_Hi1_c::setAttention(u32 param_1) {
    WWHD_FUNC(0x0223C9D4, void, this, param_1);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO().mPrmTbl.mAttentionYOffset;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (field_0x790 == 0 && param_1 == 0) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}
VERIFY(0x0223C9D4, &daNpc_Hi1_c::setAttention);

/* 0223CA28 */
void daNpc_Hi1_c::setMtx(u32 param_1) {
    WWHD_FUNC(0x0223CA28, void, this, param_1);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(param_1);
}
VERIFY(0x0223CA28, &daNpc_Hi1_c::setMtx);

/* 0223CB30 */
bool daNpc_Hi1_c::createInit() {
    WWHD_FUNC(0x0223CB30, bool, this);
    /* l_evn_tbl (.data 0x101BE6F8): one event name, "dummy" */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101BE6F8));
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2(STR(0x1001A4FC) /* "Hi1" */, (fopNpc_npc_c*)(void*)this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    field_0x7C0 = 2;
    gravity = 0.0f;
    bool ret;
    switch (field_0x7C6) {
    case 0:
        ret = init_HI1_0();
        break;
    default:
        ret = false;
        break;
    }
    if (!ret) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return true;
}
VERIFY(0x0223CB30, &daNpc_Hi1_c::createInit);

/* 0223CCC4 */
cPhs_State daNpc_Hi1_c::_create() {
    WWHD_FUNC(0x0223CCC4, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Hi1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = HI1_VTBL;
            gabi::call(0x025E7C6C, mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm (the matcher calls it init) */
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mStateIsComplaete = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    /* a_siz_tbl (.data 0x101BE71C)[field_0x7C5] */
    if (!fopAcM_entrySolidHeap(this, 0x0223C658 /* CheckCreateHeap */, gabi::load<u32>(0x101BE71C + field_0x7C5 * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -110.0f, -20.0f, -100.0f, 110.0f, 280.0f, 100.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x0223CCC4, &daNpc_Hi1_c::_create);

/* 0223CE20 */
static cPhs_State daNpc_Hi1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0223CE20, cPhs_State, i_this);
    return ((daNpc_Hi1_c*)i_this)->_create();
}
VERIFY(0x0223CE20, daNpc_Hi1_Create);

/* 0223CE24 */
BOOL daNpc_Hi1_c::_delete() {
    WWHD_FUNC(0x0223CE24, BOOL, this);
    /* retail: no mStateIsComplaete test (DEMO_SELECT) */
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x0223CE24, &daNpc_Hi1_c::_delete);

/* 0223CE78 */
static BOOL daNpc_Hi1_Delete(daNpc_Hi1_c* i_this) {
    WWHD_FUNC(0x0223CE78, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0223CE78, daNpc_Hi1_Delete);

/* 0223CE7C */
void daNpc_Hi1_c::checkOrder() {
    WWHD_FUNC(0x0223CE7C, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdx[field_0x796]) && field_0x7C1 >= 3) {
            field_0x7C1 = 0;
            field_0x7BC = 0xFF;
            field_0x7BD = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (field_0x7C1 == 1 || field_0x7C1 == 2) {
            field_0x7C1 = 0;
            field_0x7B7 = 1;
        }
    }
}
VERIFY(0x0223CE7C, &daNpc_Hi1_c::checkOrder);

/* 0223CF54 */
u8 daNpc_Hi1_c::demo() {
    WWHD_FUNC(0x0223CF54, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (field_0x7BA != 0) {
            field_0x7BA = 0;
        }
        return field_0x7BA;
    }
    if (field_0x7BA == 0) {
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        field_0x7BA = 1;
        field_0x7B3 = 0;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) */
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001A424), 0x23A, STR(0x1001A414));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = dDemo_object_getActor(obj, id);
    }
    u32 btp = gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10);
    if (btp != 0) {
        u8 frame = (u8)J3DAnm_getFrameMax(btp);
        u8 f = (u8)(mBtpAnmFrame + 1);
        if (f < frame) {
            mBtpAnmFrame = f;
        } else {
            mBtpAnmFrame = frame;
        }
    }
    if (demo_actor != nullptr) {
        void* p = dDemo_actor_getP_BtpData(demo_actor, mArcName);
        if (p != nullptr) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), p, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtpAnmFrame = 0;
            field_0x7BE = 1;
        }
    }
    u32 btk = gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68);
    if (btk != 0) {
        u8 frame = (u8)J3DAnm_getFrameMax(btk);
        u8 f = (u8)(mBtkAnmFrame + 1);
        if (f < frame) {
            mBtkAnmFrame = f;
        } else {
            mBtkAnmFrame = frame;
        }
    }
    if (demo_actor != nullptr) {
        void* p = dDemo_actor_getP_BtkData(demo_actor, mArcName);
        if (p != nullptr) {
            mDoExt_btkAnm_init(mBtkAnm, J3DModel_getModelData_l(mpMorf->getModel()), p, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtkAnmFrame = 0;
            field_0x7BF = 1;
        }
    }
    /* ENABLE_TRANS | ENABLE_ROTATE | ENABLE_ANM | ENABLE_ANM_FRAME */
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, 0, 0);
    return field_0x7BA;
}
VERIFY(0x0223CF54, &daNpc_Hi1_c::demo);

/* 0223D1D4 */
s32 daNpc_Hi1_c::isEventEntry() {
    WWHD_FUNC(0x0223D1D4, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x0223D1D4, &daNpc_Hi1_c::isEventEntry);

/* 0223D214 */
void daNpc_Hi1_c::endEvent() {
    WWHD_FUNC(0x0223D214, void, this);
    dComIfGp_event_reset();
    field_0x7BC = 0xFF;
    field_0x7BD = 0xFF;
}
VERIFY(0x0223D214, &daNpc_Hi1_c::endEvent);

/* 0223D258 (unnamed by the matcher) */
void daNpc_Hi1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x0223D258, void, this, i_staffIdx);
    if (i_staffIdx == -1) {
        return;
    }
    /* a_cut_tbl (.data 0x101BE724): one cut, "DUMMY" */
    s8 actIdx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BE724, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x0223D258, &daNpc_Hi1_c::privateCut);

/* 0223D2F0 */
void daNpc_Hi1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x0223D2F0, void, this, i_staffIdx);
    s16 ev = mEventIdx[field_0x796];
    if (dComIfGp_evmng_endCheck(ev)) {
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x0223D2F0, &daNpc_Hi1_c::event_proc);

/* 0223D394 (the matcher calls it cLib_getRndValue<i> from d_a_npc_aj1) */
fopAc_ac_c* daNpc_Hi1_c::searchByID(fpc_ProcID pid, be<s32>* param_2) {
    WWHD_FUNC(0x0223D394, fopAc_ac_c*, this, pid, param_2);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *param_2 = 0;
    if (!fopAcM_SearchByID_o(pid, actor)) {
        *param_2 = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x0223D394, &daNpc_Hi1_c::searchByID);

static inline void copy_words(u32 dst, u32 src) {
    u32 x = gabi::load<u32>(src), y = gabi::load<u32>(src + 4), z = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, x);
    gabi::store<u32>(dst + 4, y);
    gabi::store<u32>(dst + 8, z);
}

/* 0223D3E8 */
void daNpc_Hi1_c::lookBack() {
    WWHD_FUNC(0x0223D3E8, void, this);
    gabi::Local<cXyz> temp5;
    temp5->x = 0.0f;
    temp5->y = 0.0f;
    temp5->z = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    f32 srcX = current.pos.x;
    s16 targetY = current.angle.y;
    mActorAngleY = targetY;
    u8 temp4 = field_0x7B8;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* temp2 = nullptr;
    switch ((u32)(s32)field_0x7C4) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        copy_words(gabi::ea(&mLookTarget), gabi::ea(eye.get()));
        copy_words(gabi::ea(temp5.get()), gabi::ea(eye.get()));
        temp2 = temp5;
        break;
    }
    case 2:
        copy_words(gabi::ea(temp5.get()), gabi::ea(&mLookTarget));
        temp2 = temp5;
        break;
    case 3:
        targetY = field_0x7AA;
        break;
    case 4: {
        gabi::Local<be<s32>> temp3;
        fopAc_ac_c* actor = searchByID(mPId, temp3);
        if (actor != nullptr && *temp3 == 0) {
            u32 src = gabi::ea(&actor->current.pos), dst = gabi::ea(&mLookTarget);
            gabi::store<u32>(dst, gabi::load<u32>(src));
            gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
            u32 z = gabi::load<u32>(src + 8);
            f32 x = mLookTarget.x;
            gabi::store<u32>(dst + 8, z);
            f32 ey = actor->eyePos.y;
            temp5->x = x;
            f32 zf = mLookTarget.z;
            temp5->y = ey;
            temp5->z = zf;
            temp2 = temp5;
            mLookTarget.y = ey;
        }
        break;
    }
    default:
        break;
    }
    gabi::Local<cXyz> temp; /* passed by value: a copy */
    temp->x = srcX;
    temp->z = srcZ;
    temp->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, temp2, temp, targetY, l_HIO().mPrmTbl.mMaxTurnStep, temp4);
}
VERIFY(0x0223D3E8, &daNpc_Hi1_c::lookBack);

/* 0223D67C */
void daNpc_Hi1_c::eventOrder() {
    WWHD_FUNC(0x0223D67C, void, this);
    s8 cond = field_0x7C1;
    if (cond == 1 || cond == 2) {
        s8 c = field_0x7C1;
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (cond >= 3) {
        s16 idx = (s16)(cond - 3);
        field_0x796 = idx;
        fopAcM_orderOtherEventId(this, mEventIdx[idx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x0223D67C, &daNpc_Hi1_c::eventOrder);

/* the common tail of _execute's three paths */
static inline void hi1_setAngle(daNpc_Hi1_c* i_this) {
    s16 z = i_this->current.angle.z, y = i_this->current.angle.y, x = i_this->current.angle.x;
    i_this->mAngle.x = x;
    i_this->mAngle.y = y;
    i_this->mAngle.z = z;
    if (i_this->field_0x7B3 == 0) {
        i_this->shape_angle.z = i_this->current.angle.z;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.x = i_this->current.angle.x;
    }
}

/* 0223D6EC */
BOOL daNpc_Hi1_c::_execute() {
    WWHD_FUNC(0x0223D6EC, BOOL, this);
    if (field_0x7B5 == 0) {
        mInitAngle.y = current.angle.y;
        mInitAngle.z = current.angle.z;
        copy_words(gabi::ea(&mInitPos), gabi::ea(&current.pos));
        mInitAngle.x = current.angle.x;
        field_0x7B5 = 1;
    }
    daNpc_Hi1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    m_jnt.setParam(prm.mJnt[4], prm.mJnt[5], prm.mJnt[6], prm.mJnt[7], prm.mJnt[0], prm.mJnt[1], prm.mJnt[2], prm.mJnt[3],
                   prm.mJnt[8]);
    if (field_0x7B2 != 0 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (demo() == 0) {
        s32 temp = -1;
        bool evt = false;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            temp = isEventEntry();
            if (temp >= 0)
                evt = true;
        }
        if (evt || field_0x7B0 != 0) {
            event_proc(temp);
        } else {
            pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
        hi1_setAngle(this);
    } else {
        field_0x7B2 = 0;
        eventOrder();
        hi1_setAngle(this);
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(0);
    if (field_0x7BA == 0) {
        setCollision(110.0f, 260.0f);
    }
    return TRUE;
}
VERIFY(0x0223D6EC, &daNpc_Hi1_c::_execute);

/* 0223D9BC */
static BOOL daNpc_Hi1_Execute(daNpc_Hi1_c* i_this) {
    WWHD_FUNC(0x0223D9BC, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0223D9BC, daNpc_Hi1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 0223D9C0 */
BOOL daNpc_Hi1_c::_draw() {
    WWHD_FUNC(0x0223D9C0, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData_l(pModel);
    if (field_0x7B2 != 0 || field_0x7B4 != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, modelData, mBtpAnmFrame);
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, modelData, (f32)(u8)mBtkAnmFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(modelData) + 0x44, 0); /* mBtkAnm.remove(modelData) */
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* mBtpAnm.remove(modelData) */
    /* HD: no shadowDraw() */
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO().mPrmTbl.mDebug != 0) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001A3B8);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001A3BC);
    }
    return TRUE;
}
VERIFY(0x0223D9C0, &daNpc_Hi1_c::_draw);

/* 0223DB0C */
static BOOL daNpc_Hi1_Draw(daNpc_Hi1_c* i_this) {
    WWHD_FUNC(0x0223DB0C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0223DB0C, daNpc_Hi1_Draw);

/* 0223DB10 */
static BOOL daNpc_Hi1_IsDelete(daNpc_Hi1_c*) {
    WWHD_FUNC(0x0223DB10, BOOL, (daNpc_Hi1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0223DB10, daNpc_Hi1_IsDelete);

/* 0223DB18 (unnamed by the matcher) */
s32 daNpc_Hi1_c::bckResID(int idx) {
    WWHD_FUNC(0x0223DB18, s32, this, idx);
    /* a_resID_tbl (.rodata 0x1001A528) */
    return gabi::load<s32>(0x1001A528 + idx * 4);
}
VERIFY(0x0223DB18, &daNpc_Hi1_c::bckResID);

/* 0223DB2C */
void daNpc_Hi1_c::setAnm_anm(anm_prm_c* param_1) {
    WWHD_FUNC(0x0223DB2C, void, this, param_1);
    s8 temp = param_1->mAnmNum;
    if (temp < 0 || field_0x7C0 == temp) {
        return;
    }
    s32 resID = bckResID(temp);
    s32 loopMode = param_1->mLoopMode;
    f32 morf = param_1->mMorf;
    f32 speed = param_1->mSpeed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    field_0x7AC = 0;
    field_0x7C0 = param_1->mAnmNum;
    mFrame = 0.0f;
    field_0x7AD = 0;
}
VERIFY(0x0223DB2C, &daNpc_Hi1_c::setAnm_anm);

/* 0223DBC4 */
void daNpc_Hi1_c::setAnm_NUM(int param_1, int param_2) {
    WWHD_FUNC(0x0223DBC4, void, this, param_1, param_2);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE728); /* [2] */
    if (param_2 != 0) {
        init_texPttrnAnm(a_anm_prm_tbl[param_1].mTexNum, 1);
    }
    setAnm_anm(&a_anm_prm_tbl[param_1]);
}
VERIFY(0x0223DBC4, &daNpc_Hi1_c::setAnm_NUM);

/* 0223DC34 */
void daNpc_Hi1_c::setAnm() {
    WWHD_FUNC(0x0223DC34, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE748); /* [3] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x7C2].mTexNum, 1);
    setAnm_anm(&a_anm_prm_tbl[field_0x7C2]);
}
VERIFY(0x0223DC34, &daNpc_Hi1_c::setAnm);

/* 0223DCA4 */
void daNpc_Hi1_c::setAnm_ATR() {
    WWHD_FUNC(0x0223DCA4, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE778); /* [2] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x7BC].mTexNum, 1);
    setAnm_anm(&a_anm_prm_tbl[field_0x7BC]);
}
VERIFY(0x0223DCA4, &daNpc_Hi1_c::setAnm_ATR);

/* 0223DD0C */
void daNpc_Hi1_c::chngAnmAtr(u32 param_1) {
    WWHD_FUNC(0x0223DD0C, void, this, param_1);
    /* HD: >= 2 (GameCube > 2, which indexed past the two-entry table) */
    if (param_1 == field_0x7BC || param_1 >= 2) {
        return;
    }
    field_0x7BC = (u8)param_1;
    setAnm_ATR();
}
VERIFY(0x0223DD0C, &daNpc_Hi1_c::chngAnmAtr);

/* 0223DD28 */
void daNpc_Hi1_c::anmAtr(u32 i_msgStatus) {
    WWHD_FUNC(0x0223DD28, void, this, i_msgStatus);
    if (i_msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (field_0x7C8 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x7C8 = field_0x7C8 + 1;
        }
        u8 tagInfo = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tagInfo != 0xFF && tagInfo != field_0x7BD) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF); /* dComIfGp_clearMesgAnimeTagInfo() */
            field_0x7BD = tagInfo;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        field_0x7C8 = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x0223DD28, &daNpc_Hi1_c::anmAtr);

/* 0223DDF8 (unnamed by the matcher; vtable 0x1001A548 + 0x14) */
u16 daNpc_Hi1_c::next_msgStatus(be<u32>*) {
    WWHD_FUNC(0x0223DDF8, u16, this, (be<u32>*)nullptr);
    return 0x10; /* fopMsgStts_MSG_ENDS_e */
}
VERIFY(0x0223DDF8, &daNpc_Hi1_c::next_msgStatus);

/* 0223DE00 (unnamed by the matcher; vtable + 0x1C): getMsg with getMsg_HI1_0 (returns 0) inlined */
u32 daNpc_Hi1_c::getMsg() {
    WWHD_FUNC(0x0223DE00, u32, this);
    return 0;
}
VERIFY(0x0223DE00, &daNpc_Hi1_c::getMsg);

/* 0223DE08 */
bool daNpc_Hi1_c::chk_talk() {
    WWHD_FUNC(0x0223DE08, bool, this);
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
VERIFY(0x0223DE08, &daNpc_Hi1_c::chk_talk);

/* 0223DEA0 */
u8 daNpc_Hi1_c::chk_parts_notMov() {
    WWHD_FUNC(0x0223DEA0, u8, this);
    return mJointHeadY != m_jnt.mAngles[0][1] || mJointBackboneY != m_jnt.mAngles[1][1] || mActorAngleY != current.angle.y;
}
VERIFY(0x0223DEA0, &daNpc_Hi1_c::chk_parts_notMov);

/* 0223DEE0 */
u8 daNpc_Hi1_c::chkAttention() {
    WWHD_FUNC(0x0223DEE0, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x0223DEE0, &daNpc_Hi1_c::chkAttention);

/* 0223DF68 */
void daNpc_Hi1_c::setStt(u32 param_1) {
    WWHD_FUNC(0x0223DF68, void, this, param_1);
    s8 temp = field_0x7C2;
    field_0x7C2 = (s8)param_1;
    switch (param_1) {
    case 1:
        field_0x7C1 = 0;
        break;
    case 2:
        field_0x7C1 = 0;
        field_0x7BD = 0xFF;
        field_0x7BC = 0xFF;
        field_0x7C3 = temp;
        field_0x7C8 = 0;
        break;
    }
    setAnm();
}
VERIFY(0x0223DF68, &daNpc_Hi1_c::setStt);

/* 0223DFB4 */
BOOL daNpc_Hi1_c::wait_1() {
    WWHD_FUNC(0x0223DFB4, BOOL, this);
    s16 timer = 0;
    if (field_0x7B9 != 0) {
        cLib_addCalcAngleS(&current.angle.y, mInitAngle.y, 4, 0x800, 0x80);
        timer = (s16)(mInitAngle.y - current.angle.y);
    }
    if (field_0x7B7 != 0) {
        if (chk_talk()) {
            setStt(2);
            field_0x7B8 = 0;
            field_0x7C4 = 1;
            field_0x7B9 = 0;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
        return TRUE;
    }
    field_0x7C4 = 0;
    field_0x7C1 = 2;
    field_0x7B8 = 1;
    if (timer == 0) {
        if (mHasAttention != 0) {
            mTimer2 = (s16)cLib_getRndValue(0xF, 0x1E);
        }
        if (cLib_calcTimer(&mTimer2) != 0) {
            field_0x7C4 = 1;
        }
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> pos; /* passed by value: a copy */
        pos->x = player->current.pos.x;
        pos->y = player->current.pos.y;
        pos->z = player->current.pos.z;
        if (!dNpc_chkAttn(this, pos, 200.0f, 50.0f, 55.0f, field_0x7C4 == 1)) {
            field_0x7C4 = 0;
            field_0x7B9 = 1;
        }
    }
    return TRUE;
}
VERIFY(0x0223DFB4, &daNpc_Hi1_c::wait_1);

/* 0223E12C */
u32 daNpc_Hi1_c::talk_1() {
    WWHD_FUNC(0x0223E12C, u32, this);
    u32 ret = chk_parts_notMov();
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg == 0) {
        return TRUE;
    }
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        mItemNo = 0xFF;
        field_0x7B7 = 0;
        setStt(field_0x7C3);
        setAnm_NUM(0, 1);
        mTimer2 = (s16)cLib_getRndValue(0xF, 0x1E);
        endEvent();
    }
    return ret;
}
VERIFY(0x0223E12C, &daNpc_Hi1_c::talk_1);

/* 0223E204 */
BOOL daNpc_Hi1_c::wait_action1(void*) {
    WWHD_FUNC(0x0223E204, BOOL, this, (void*)nullptr);
    s8 state = field_0x7C7;
    if (state == 0) {
        setStt(1);
        field_0x7B9 = 1;
        field_0x7C7 = field_0x7C7 + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) { /* case 9 and the rest */
        return TRUE;
    }
    mHasAttention = chkAttention();
    switch ((u32)(s32)field_0x7C2) {
    case 1:
        field_0x790 = wait_1();
        break;
    case 2:
        field_0x790 = talk_1();
        break;
    }
    return TRUE;
}
VERIFY(0x0223E204, &daNpc_Hi1_c::wait_action1);

/* 0223E2BC daNpc_Hi1_HIO_c::daNpc_Hi1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Hi1_HIO_c* daNpc_Hi1_HIO_c_ct(daNpc_Hi1_HIO_c* i_this) {
    WWHD_FUNC(0x0223E2BC, daNpc_Hi1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Hi1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001A404;
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101BE798), 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x0223E2BC, daNpc_Hi1_HIO_c_ct);

/* 0223E328: static initialisation of the translation unit */
static void __sinit_d_a_npc_hi1_cpp() {
    WWHD_FUNC(0x0223E328, void);
    sinit_header_statics_z(0x10467158, 0x101BE7B4, 0x1046718C);
    daNpc_Hi1_HIO_c_ct(&l_HIO()); /* static daNpc_Hi1_HIO_c l_HIO */
}
VERIFY(0x0223E328, __sinit_d_a_npc_hi1_cpp);

/* 0223E3C8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0223E3C8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0223E3C8, SafeString_dt);

/* 0223E3DC: daNpc_Hi1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Hi1_c_dt(daNpc_Hi1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0223E3DC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001A3E4);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001A3F4);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0223E3DC, daNpc_Hi1_c_dt);

/* 0223E478: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0223E478, void, (SafeString*)nullptr);
}
VERIFY(0x0223E478, SafeString_assureTerminationImpl);
