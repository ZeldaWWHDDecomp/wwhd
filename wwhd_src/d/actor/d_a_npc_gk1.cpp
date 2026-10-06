/**
 * d_a_npc_gk1.cpp (WWHD)
 * NPC - Mila's father (poor)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_gk1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_gk1.h"

#define SAFESTRING_VTBL 0x10019F70 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_aj1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* dComIfG_getObjectIDRes(arc, id): the key is a sead::SafeString temporary {top, vtable} */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (out-of-line copy in another TU) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 025E789C mDoExt_btpAnm::init */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
/* 025D54C4 fopAcM_SearchByID(fpc_ProcID, fopAc_ac_c**) (out of line) */
static inline BOOL fopAcM_SearchByID_l(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }

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
enum : u32 { PMF_wait_action1 = 0x10019F50 };

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
struct daNpc_Gk1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackBoneX;
        /* 0x0A */ be<s16> mMaxBackBoneY;
        /* 0x0C */ be<s16> mMinBackBoneX;
        /* 0x0E */ be<s16> mMinBackBoneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mLookVel;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebugDraw;
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Gk1_HIO_c, 0x28);
static daNpc_Gk1_HIO_c& l_HIO() { return *gabi::at<daNpc_Gk1_HIO_c>(0x10467050); }

static inline const char* arcName(daNpc_Gk1_c* a) { return gabi::at<const char>(gabi::ea(a->mArcName)); }

/* 02235684 */
void daNpc_Gk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02235684, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(30.0f, 30.0f, 0.0f): guard 0x10467094, object 0x10467088 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10467088);
    if (gabi::load<u32>(0x10467094) == 0) {
        a_eye_pos_off->x = 30.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 30.0f;
        gabi::store<u32>(0x10467094, 1);
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x02235684, &daNpc_Gk1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 022357D4 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022357D4, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x02235684);
    return TRUE;
}
VERIFY(0x022357D4, nodeCB_Head);

/* 0223581C */
void daNpc_Gk1_c::_nodeCB_Neck(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0223581C, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x0223581C, &daNpc_Gk1_c::_nodeCB_Neck);

/* 02235940 */
static BOOL nodeCB_Neck(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02235940, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0223581C);
    return TRUE;
}
VERIFY(0x02235940, nodeCB_Neck);

/* 02235988 */
void daNpc_Gk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02235988, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x02235988, &daNpc_Gk1_c::_nodeCB_BackBone);

/* 02235AAC */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02235AAC, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x02235988);
    return TRUE;
}
VERIFY(0x02235AAC, nodeCB_BackBone);

/* 02235AF4 (one btp: the index is not used) */
int daNpc_Gk1_c::btpResID(int) {
    WWHD_FUNC(0x02235AF4, int, this, 0);
    return gabi::load<s32>(0x10019FDC);
}
VERIFY(0x02235AF4, &daNpc_Gk1_c::btpResID);

/* 02235B00 */
u32 daNpc_Gk1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x02235B00, u32, this, i_btpNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(arcName(this), btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(454, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x10019FE4), 0x1C6, STR(0x10019FF4));
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}
VERIFY(0x02235B00, &daNpc_Gk1_c::setBtp);

/* 02235BEC */
u32 daNpc_Gk1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x02235BEC, u32, this, i_btpNum, i_bModify);
    return setBtp(i_btpNum, i_bModify);
}
VERIFY(0x02235BEC, &daNpc_Gk1_c::init_texPttrnAnm);

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

/* 02235BF0 */
BOOL daNpc_Gk1_c::bodyCreateHeap() {
    WWHD_FUNC(0x02235BF0, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(arcName(this), 7);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1571, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001A014), 0x623, STR(0x1001A024));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3); /* delete (virtual deleting destructor) */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A004) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A014), 0x639, STR(0x1001A038));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A04C) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A014), 0x63B, STR(0x1001A058));
    m_nck_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A00C) /* "neck" */);
    if (m_nck_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A014), 0x63D, STR(0x1001A070));
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, 0x022357D4 /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, 0x02235AAC /* nodeCB_BackBone */);
    setJointCallBack(mpMorf->getModel(), m_nck_jnt_num, 0x02235940 /* nodeCB_Neck */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x02235BF0, &daNpc_Gk1_c::bodyCreateHeap);

/* 02235F3C */
BOOL daNpc_Gk1_c::itemCreateHeap() {
    WWHD_FUNC(0x02235F3C, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(arcName(this), 5);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1626, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001A084), 0x65A, STR(0x1001A094));
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}
VERIFY(0x02235F3C, &daNpc_Gk1_c::itemCreateHeap);

/* 02235FD4 */
BOOL daNpc_Gk1_c::hat_CreateHeap() {
    WWHD_FUNC(0x02235FD4, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(arcName(this), 6);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1642, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001A0A8), 0x66A, STR(0x1001A0B8));
    mpHatModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpHatModel.get() != nullptr;
}
VERIFY(0x02235FD4, &daNpc_Gk1_c::hat_CreateHeap);

/* 0223606C */
BOOL daNpc_Gk1_c::CreateHeap() {
    WWHD_FUNC(0x0223606C, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap() || !hat_CreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 90.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x0223606C, &daNpc_Gk1_c::CreateHeap);

/* 02236138 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02236138, BOOL, i_this);
    return static_cast<daNpc_Gk1_c*>(i_this)->CreateHeap();
}
VERIFY(0x02236138, CheckCreateHeap);

/* 0223613C (unnamed by the matcher): the parameter is not used; the archive name is set once */
u8 daNpc_Gk1_c::decideType(int) {
    WWHD_FUNC(0x0223613C, u8, this, 0);
    if (mTypeSet > 0) {
        return true;
    }
    mTypeSet = 1;
    mType = 0;
    /* strcpy(mArcName, "Gk") (3-byte lswi/stswi copy) */
    for (int i = 0; i < 3; i++)
        gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x1001A0D8 + i));
    return mTypeSet != -1 && mType != -1;
}
VERIFY(0x0223613C, &daNpc_Gk1_c::decideType);

/* 022361A0 */
BOOL daNpc_Gk1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x022361A0, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
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
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x022361A0, &daNpc_Gk1_c::set_action);

/* 022362CC */
bool daNpc_Gk1_c::init_GK1_0() {
    WWHD_FUNC(0x022362CC, bool, this);
    if (dComIfGs_isEventBit(0x2D01)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
        return true;
    }
    return false;
}
VERIFY(0x022362CC, &daNpc_Gk1_c::init_GK1_0);

/* 02236350 */
void daNpc_Gk1_c::play_btp_anm() {
    WWHD_FUNC(0x02236350, void, this);
    u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
    if (mBtpNum != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= frame_max) {
            if (mBtpNum == 0) {
                mBlinkTimer = (s16)cLib_getRndValue(60, 90);
                frame_max = 0;
            }
            mBtpFrame = frame_max;
        }
    }
}
VERIFY(0x02236350, &daNpc_Gk1_c::play_btp_anm);

/* 022363F0 */
void daNpc_Gk1_c::play_animation() {
    WWHD_FUNC(0x022363F0, void, this);
    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}
VERIFY(0x022363F0, &daNpc_Gk1_c::play_animation);

/* 02236494 */
void daNpc_Gk1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x02236494, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = current.pos.y + l_HIO().mPrm.mAttPosOffsetY;
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
VERIFY(0x02236494, &daNpc_Gk1_c::setAttention);

/* 022364E8: the item and the hat follow the head joint */
void daNpc_Gk1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x022364E8, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(mpItemModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpItemModel);
    }
    if (mpHatModel.get()) {
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(mpHatModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpHatModel);
    }
    setAttention(i_setEyePos);
}
VERIFY(0x022364E8, &daNpc_Gk1_c::setMtx);

/* 02236748 */
bool daNpc_Gk1_c::createInit() {
    WWHD_FUNC(0x02236748, bool, this);
    /* l_evn_tbl (.data 0x101BE3BC): "dummy" */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101BE3BC));
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2(STR(0x1001A0E4) /* "Gk1" */, (fopNpc_npc_c*)(void*)this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    cullSizeFar = 12000.0f / gabi::load<f32>(0x1048D04C);
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gravity = -4.5f;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    mBckNum = 5;
    if (mType != 0) {
        return false;
    }
    if (!init_GK1_0()) {
        return false;
    }
    mAngle.z = current.angle.z;
    mAngle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    mAngle.y = current.angle.y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x02236748, &daNpc_Gk1_c::createInit);

/* 022368D4 */
cPhs_State daNpc_Gk1_c::_create() {
    WWHD_FUNC(0x022368D4, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Gk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001A130;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, arcName(this));
    mbResLoaded = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    /* a_heap_size_tbl (.data 0x101BE3E0) */
    if (!fopAcM_entrySolidHeap(this, 0x02236138 /* CheckCreateHeap */, gabi::load<u32>(0x101BE3E0 + mTypeSet * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -90.0f, -20.0f, -80.0f, 90.0f, 200.0f, 80.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x022368D4, &daNpc_Gk1_c::_create);

/* 02236A24 */
static cPhs_State daNpc_Gk1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02236A24, cPhs_State, i_this);
    return ((daNpc_Gk1_c*)i_this)->_create();
}
VERIFY(0x02236A24, daNpc_Gk1_Create);

/* 02236A28 */
BOOL daNpc_Gk1_c::_delete() {
    WWHD_FUNC(0x02236A28, BOOL, this);
    dComIfG_resDelete(&mPhs, arcName(this));
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x02236A28, &daNpc_Gk1_c::_delete);

/* 02236A7C */
static BOOL daNpc_Gk1_Delete(daNpc_Gk1_c* i_this) {
    WWHD_FUNC(0x02236A7C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02236A7C, daNpc_Gk1_Delete);

/* 02236A80 */
void daNpc_Gk1_c::checkOrder() {
    WWHD_FUNC(0x02236A80, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
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
VERIFY(0x02236A80, &daNpc_Gk1_c::checkOrder);

/* 02236B58 */
u8 daNpc_Gk1_c::demo() {
    WWHD_FUNC(0x02236B58, u8, this);
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
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x10019FC8), 0x23A, STR(0x10019FB8));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    u32 btp = gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10);
    if (btp != 0) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(btp));
        u8 frame = (u8)(mBtpFrame + 1);
        if (frame < frame_max) {
            mBtpFrame = frame;
        } else {
            mBtpFrame = frame_max;
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, arcName(this)); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 1;
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), arcName(this), 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}
VERIFY(0x02236B58, &daNpc_Gk1_c::demo);

/* 02236D14 */
s32 daNpc_Gk1_c::isEventEntry() {
    WWHD_FUNC(0x02236D14, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x02236D14, &daNpc_Gk1_c::isEventEntry);

/* 02236D54 */
void daNpc_Gk1_c::endEvent() {
    WWHD_FUNC(0x02236D54, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x02236D54, &daNpc_Gk1_c::endEvent);

/* 02236D98 (unnamed by the matcher) */
void daNpc_Gk1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x02236D98, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101BE3E8): "dummy" */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BE3E8, 1, TRUE, 0);
    mActionIndex = idx;
    void* evmng = dComIfGp_getPEvtManager();
    if (idx != -1) {
        gabi::call<BOOL>(0x025447C8, evmng, i_staffIdx); /* dComIfGp_evmng_getIsAddvance (no cut to init) */
        evmng = dComIfGp_getPEvtManager();
    }
    gabi::call(0x02543280, evmng, i_staffIdx); /* dComIfGp_evmng_cutEnd */
}
VERIFY(0x02236D98, &daNpc_Gk1_c::privateCut);

/* 02236E30 */
void daNpc_Gk1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x02236E30, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        endEvent();
    } else if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x02236E30, &daNpc_Gk1_c::event_proc);

/* 02236ED4 (the matcher names it cLib_getRndValue<int> of d_a_npc_aj1) */
fopAc_ac_c* daNpc_Gk1_c::searchByID(u32 i_id, be<s32>* o_err) {
    WWHD_FUNC(0x02236ED4, fopAc_ac_c*, this, i_id, o_err);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_err = 0;
    if (!fopAcM_SearchByID_l(i_id, actor)) {
        *o_err = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x02236ED4, &daNpc_Gk1_c::searchByID);

/* 02236F28 */
void daNpc_Gk1_c::lookBack() {
    WWHD_FUNC(0x02236F28, void, this);
    gabi::Local<cXyz> look_pos;
    s16 target_y = current.angle.y;
    u8 head_only = mbHeadOnly;
    f32 srcX = current.pos.x;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    look_pos->x = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    look_pos->y = 0.0f;
    mActorAngleY = target_y;
    look_pos->z = 0.0f;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        mLookPos.copy(*eye);
        look_pos->copy(*eye);
        look_pos_p = look_pos;
        break;
    }
    case 2:
        look_pos->copy(mLookPos);
        look_pos_p = look_pos;
        break;
    case 3:
        target_y = mLookAngleY;
        break;
    case 4: {
        gabi::Local<be<s32>> err;
        fopAc_ac_c* partner = searchByID(mPartnerProcId, err);
        if (partner != nullptr && *err == 0) {
            mLookPos.copy(partner->current.pos);
            f32 y = partner->eyePos.y;
            look_pos->x = mLookPos.x;
            look_pos->y = y;
            look_pos->z = mLookPos.z;
            look_pos_p = look_pos;
            mLookPos.y = y;
        }
        break;
    }
    }
    gabi::Local<cXyz> cur_pos; /* passed by value: a copy */
    cur_pos->x = srcX;
    cur_pos->y = srcY;
    cur_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, look_pos_p, cur_pos, target_y, l_HIO().mPrm.mLookVel, head_only);
}
VERIFY(0x02236F28, &daNpc_Gk1_c::lookBack);

/* 022371BC */
void daNpc_Gk1_c::eventOrder() {
    WWHD_FUNC(0x022371BC, void, this);
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
VERIFY(0x022371BC, &daNpc_Gk1_c::eventOrder);

/* 0223722C */
BOOL daNpc_Gk1_c::_execute() {
    WWHD_FUNC(0x0223722C, BOOL, this);
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Gk1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m910 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        m910 = 0;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!mbNoShapeAngle) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(false);
    if (!mbDemo) {
        setCollision(90.0f, 200.0f);
    }
    return TRUE;
}
VERIFY(0x0223722C, &daNpc_Gk1_c::_execute);

/* 022374E4 */
static BOOL daNpc_Gk1_Execute(daNpc_Gk1_c* i_this) {
    WWHD_FUNC(0x022374E4, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022374E4, daNpc_Gk1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 022374E8 */
BOOL daNpc_Gk1_c::_draw() {
    WWHD_FUNC(0x022374E8, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m910 || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    if (mpHatModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpHatModel, &tevStr);
        mDoExt_modelEntryDL(mpHatModel);
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x59 /* DSNAP_TYPE_NPC_GK1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mPrm.mDebugDraw) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10019F58);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x10019F5C);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x10019F60);
    }
    return TRUE;
}
VERIFY(0x022374E8, &daNpc_Gk1_c::_draw);

/* 02237694 */
static BOOL daNpc_Gk1_Draw(daNpc_Gk1_c* i_this) {
    WWHD_FUNC(0x02237694, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02237694, daNpc_Gk1_Draw);

/* 02237698 */
static BOOL daNpc_Gk1_IsDelete(daNpc_Gk1_c*) {
    WWHD_FUNC(0x02237698, BOOL, (daNpc_Gk1_c*)nullptr);
    return TRUE;
}
VERIFY(0x02237698, daNpc_Gk1_IsDelete);

/* 022376A0 (unnamed by the matcher) */
int daNpc_Gk1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x022376A0, int, this, i_bckNum);
    return gabi::load<s32>(0x1001A104 + i_bckNum * 4);
}
VERIFY(0x022376A0, &daNpc_Gk1_c::bckResID);

/* 022376B4 */
void daNpc_Gk1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x022376B4, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, arcName(this));
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    m90D = 0;
}
VERIFY(0x022376B4, &daNpc_Gk1_c::setAnm_anm);

/* 0223774C */
void daNpc_Gk1_c::setAnm() {
    WWHD_FUNC(0x0223774C, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE3EC); /* [3] */
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}
VERIFY(0x0223774C, &daNpc_Gk1_c::setAnm);

/* 022377BC */
void daNpc_Gk1_c::setAnm_ATR() {
    WWHD_FUNC(0x022377BC, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE41C); /* [5] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x022377BC, &daNpc_Gk1_c::setAnm_ATR);

/* 02237824 */
void daNpc_Gk1_c::chngAnmAtr(u8 i_atr) {
    WWHD_FUNC(0x02237824, void, this, i_atr);
    if (i_atr == mAnmAtr || i_atr >= 5) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}
VERIFY(0x02237824, &daNpc_Gk1_c::chngAnmAtr);

/* 02237840 */
void daNpc_Gk1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x02237840, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            chngAnmAtr(gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925) /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            mMesgAnimeTag = tag;
            /* chngAnmTag(): empty */
        }
        break;
    }
    case 14:
        mAtrSet = 0;
        break;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x02237840, &daNpc_Gk1_c::anmAtr);

/* 02237910 */
u16 daNpc_Gk1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x02237910, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*o_msgNoP) {
    case 0x28A1:
        *o_msgNoP = 0x28A2;
        break;
    case 0x28A2:
        *o_msgNoP = 0x28A3;
        break;
    case 0x28A3:
        *o_msgNoP = 0x28A4;
        break;
    case 0x28A4:
        *o_msgNoP = 0x28A5;
        break;
    case 0x28A6:
        *o_msgNoP = 0x28A7;
        break;
    case 0x28A7:
        *o_msgNoP = 0x28A8;
        break;
    case 0x28A8:
        if (dComIfGs_isEventBit(0x1640)) {
            msg_status = 0x10;
        } else {
            *o_msgNoP = 0x28A9;
        }
        break;
    case 0x28A9:
        *o_msgNoP = 0x28AA;
        break;
    case 0x28AB:
        *o_msgNoP = 0x28AC;
        break;
    case 0x28AC:
        *o_msgNoP = 0x28AD;
        break;
    case 0x28AD:
        *o_msgNoP = 0x28AE;
        break;
    case 0x28AF:
        *o_msgNoP = 0x28B0;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x02237910, &daNpc_Gk1_c::next_msgStatus);

/* 02237A84 */
u32 daNpc_Gk1_c::getMsg_GK1_0() {
    WWHD_FUNC(0x02237A84, u32, this);
    if (dComIfGs_isEventBit(0x0B02) && !dComIfGs_isEventBit(0x1680)) {
        return 0x28A1;
    }
    if (!dKy_daynight_check()) {
        return dComIfGs_isEventBit(0x1640) ? 0x28A7 : 0x28A6;
    }
    return dComIfGs_isEventBit(0x0E08) ? 0x28AF : 0x28AB;
}
VERIFY(0x02237A84, &daNpc_Gk1_c::getMsg_GK1_0);

/* 02237B50 */
u32 daNpc_Gk1_c::getMsg() {
    WWHD_FUNC(0x02237B50, u32, this);
    if (mType == 0) {
        return getMsg_GK1_0();
    }
    return 0;
}
VERIFY(0x02237B50, &daNpc_Gk1_c::getMsg);

/* 02237B88 */
u8 daNpc_Gk1_c::chk_talk() {
    WWHD_FUNC(0x02237B88, u8, this);
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
VERIFY(0x02237B88, &daNpc_Gk1_c::chk_talk);

/* 02237C20: true when the head, backbone or body turned this frame */
u8 daNpc_Gk1_c::chk_parts_notMov() {
    WWHD_FUNC(0x02237C20, u8, this);
    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y);
}
VERIFY(0x02237C20, &daNpc_Gk1_c::chk_parts_notMov);

/* 02237C60 */
u8 daNpc_Gk1_c::chkAttention() {
    WWHD_FUNC(0x02237C60, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02237C60, &daNpc_Gk1_c::chkAttention);

/* 02237CE8 */
void daNpc_Gk1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x02237CE8, void, this, i_stt);
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
        mEvtState = 0;
        speedF = 0.0f;
        break;
    case 2:
        mEvtState = 0;
        mMesgAnimeTag = 0xFF;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mAtrSet = 0;
        break;
    }
    setAnm();
}
VERIFY(0x02237CE8, &daNpc_Gk1_c::setStt);

/* 02237D40: the player is near and in front (within 90 degrees when looking at him, else 60) */
u8 daNpc_Gk1_c::chk_attn() {
    WWHD_FUNC(0x02237D40, u8, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getPlayer(0)->current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dy = current.pos.y - player->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - current.angle.y);
    f32 max_angle = mLookMode == 1 ? 90.0f : 60.0f;
    if (dist < 200.0f) {
        s32 abs_angle = angle < 0 ? -angle : angle;
        if ((f32)abs_angle / 182.04444885253906f < max_angle && std::fabs(dy) < 300.0f) {
            return true;
        }
    }
    return false;
}
VERIFY(0x02237D40, &daNpc_Gk1_c::chk_attn);

/* 02237F9C */
BOOL daNpc_Gk1_c::wait_1() {
    WWHD_FUNC(0x02237F9C, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getPlayer(0)->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    mbFar = std_sqrtf(PSVECSquareMag(xz)) > 300.0f;
    if (mbFar) {
        cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    }
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
            mLookMode = 1;
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mbFar = false;
        }
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? 1 : 0;
    return TRUE;
}
VERIFY(0x02237F9C, &daNpc_Gk1_c::wait_1);

/* 022380B0 */
BOOL daNpc_Gk1_c::talk_1() {
    WWHD_FUNC(0x022380B0, BOOL, this);
    BOOL res = chk_parts_notMov();
    talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    /* HD: the message status comes from the message manager */
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0x28A5:
            dComIfGs_onEventBit(0x1680);
            break;
        case 0x28AA:
            dComIfGs_onEventBit(0x1640);
            break;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        mTalkEndTimer = (s16)cLib_getRndValue(15, 30);
        endEvent();
    }
    return res;
}
VERIFY(0x022380B0, &daNpc_Gk1_c::talk_1);

/* 022381E8 */
BOOL daNpc_Gk1_c::wait_action1(void*) {
    WWHD_FUNC(0x022381E8, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)mActState) {
    case 0:
        setStt(1);
        mbFar = true;
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 1:
            mbSetEyePos = wait_1();
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x022381E8, &daNpc_Gk1_c::wait_action1);

/* 022382A0 daNpc_Gk1_HIO_c::daNpc_Gk1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Gk1_HIO_c* daNpc_Gk1_HIO_c_ct(daNpc_Gk1_HIO_c* i_this) {
    WWHD_FUNC(0x022382A0, daNpc_Gk1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Gk1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10019FA8;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101BE46C), 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x022382A0, daNpc_Gk1_HIO_c_ct);

/* 0223830C: static initialisation of the translation unit */
static void __sinit_d_a_npc_gk1_cpp() {
    WWHD_FUNC(0x0223830C, void);
    sinit_header_statics_z(0x10467044, 0x101BE488, 0x10467078);
    daNpc_Gk1_HIO_c_ct(&l_HIO()); /* static daNpc_Gk1_HIO_c l_HIO */
}
VERIFY(0x0223830C, __sinit_d_a_npc_gk1_cpp);

/* 022383AC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022383AC, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022383AC, SafeString_dt);

/* 022383C0: daNpc_Gk1_c deleting destructor (compiler-generated, HD virtual destructor; the
 * btpAnm member has no destructor call) */
static void daNpc_Gk1_c_dt(daNpc_Gk1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022383C0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10019F88);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10019F98);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022383C0, daNpc_Gk1_c_dt);

/* 0223845C: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0223845C, void, (SafeString*)nullptr);
}
VERIFY(0x0223845C, SafeString_assureTerminationImpl);
