/**
 * d_a_npc_ls1.cpp (WWHD)
 * NPC - Aryll
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ls1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ls1.h"

#define SAFESTRING_VTBL 0x1001D0CC /* this TU's sead::SafeString vtable */
#define LS1_VTBL 0x1001D458        /* placeholder, see _create */

enum { fpcNm_KAMOME_e = 0xC2, fpcNm_NPC_BM1_e = 0x146 };
enum { dSv_event_flag_UNK_0001 = 0x0001, dSv_event_flag_UNK_0280 = 0x0280, dSv_event_flag_UNK_0310 = 0x0310, dSv_event_flag_UNK_2A80 = 0x2A80 };
enum { BCKNUM_WAIT01_e = 0, BCKNUM_WAIT02_e = 1, BCKNUM_WAIT05_e = 4, BCKNUM_WAIT06_e = 5, BCKNUM_WAIT07_e = 0xA, BCKNUM_TALK01_e = 0xB, BCKNUM_NULL_e = 0xE };
enum { ANMNUM_WAIT01_e = 0, ANMNUM_WAIT02_e = 1, ANMNUM_WAIT06_e = 5, ANMNUM_GET_e = 9, ANMNUM_WAIT07_e = 0xA, ANMNUM_DEMOWAIT_e = 0xC };
enum { BTPNUM_FUAN_e = 0, BTPNUM_MABA_e = 1, BTPNUM_FUAN02_e = 2, BTPNUM_KIZUKU_e = 3, BTPNUM_NGWARAI_e = 5, BTPNUM_WARAI_e = 8, BTPNUM_NULL_e = 0xB };

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_ba1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xD4 + i); }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
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
/* 025E789C mDoExt_btpAnm::init / 025E7CE0 mDoExt_btkAnm::init */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
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
/* cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }

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
enum : u32 { PMF_wait_action1 = 0x1001D098, PMF_demo_action1 = 0x1001D0A0 };

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
struct daNpc_Ls1_HIO_c {
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
        /* 0x12 */ be<s16> m12;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> m18;
        /* 0x19 */ u8 _19[3];
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m20;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<f32> m28;
        /* 0x2C */ be<f32> m2C;
        /* 0x30 */ be<s16> m30;
        /* 0x32 */ u8 _32[2];
        /* 0x34 */ be<f32> mPlayerEyePosOffsetY;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Ls1_HIO_c, 0x44);
static daNpc_Ls1_HIO_c& l_HIO() { return *gabi::at<daNpc_Ls1_HIO_c>(0x1046799C); }
static be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x10467960); }
static gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x104679E0); } /* [20] */

/* 0227D7DC */
void daNpc_Ls1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227D7DC, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(14.0f, 18.0f, 0.0f): guard 0x10467A30, object 0x10467990 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10467990);
    if (gabi::load<u32>(0x10467A30) == 0) {
        gabi::store<u32>(0x10467A30, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 14.0f;
        a_eye_pos_off->y = 18.0f;
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    m7E4.x = stk->m[0][3];
    m7E4.y = stk->m[1][3];
    m7E4.z = stk->m[2][3];
    mDoMtx_XrotM(stk, mHalfHeadAngleY);
    mDoMtx_ZrotM(stk, -mHalfHeadAngleX);
    PSMTXMultVec(stk, a_eye_pos_off, &mTransformedEyePos);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x0227D7DC, &daNpc_Ls1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 0227D954 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227D954, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227D7DC);
    return TRUE;
}
VERIFY(0x0227D954, nodeCB_Head);

/* 0227D99C */
void daNpc_Ls1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227D99C, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x0227D99C, &daNpc_Ls1_c::_nodeCB_BackBone);

/* 0227DAC0 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227DAC0, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227D99C);
    return TRUE;
}
VERIFY(0x0227DAC0, nodeCB_BackBone);

/* 0227DB08 */
void daNpc_Ls1_c::_nodeCB_Hand_L(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227DB08, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    PSMTXCopy(stk, &mHand_L_Mtx);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x0227DB08, &daNpc_Ls1_c::_nodeCB_Hand_L);

/* 0227DC18 */
static BOOL nodeCB_Hand_L(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227DC18, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227DB08);
    return TRUE;
}
VERIFY(0x0227DC18, nodeCB_Hand_L);

/* 0227DC60 */
void daNpc_Ls1_c::_nodeCB_Hand_R(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227DC60, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    PSMTXCopy(stk, &mHand_R_Mtx);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x0227DC60, &daNpc_Ls1_c::_nodeCB_Hand_R);

/* 0227DD70 */
static BOOL nodeCB_Hand_R(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227DD70, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227DC60);
    return TRUE;
}
VERIFY(0x0227DD70, nodeCB_Hand_R);

/* 0227DDB8 */
void daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_L(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227DDB8, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    PSMTXCopy(&mHand_L_Mtx, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), &mHand_L_Mtx);
}
VERIFY(0x0227DDB8, &daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_L);

/* 0227DE8C */
static BOOL Ls_hand_nodeCB_Hand_L(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227DE8C, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227DDB8);
    return TRUE;
}
VERIFY(0x0227DE8C, Ls_hand_nodeCB_Hand_L);

/* 0227DED4 */
void daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_R(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0227DED4, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    PSMTXCopy(&mHand_R_Mtx, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), &mHand_R_Mtx);
}
VERIFY(0x0227DED4, &daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_R);

/* 0227DFA8 */
static BOOL Ls_hand_nodeCB_Hand_R(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227DFA8, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x0227DED4);
    return TRUE;
}
VERIFY(0x0227DFA8, Ls_hand_nodeCB_Hand_R);

/* 0227DFF0 daNpc_Ls1_matAnm_c::daNpc_Ls1_matAnm_c (HD: allocates when this == NULL; the
 * J3DMaterialAnm base constructor is inlined: only the vtable is written) */
static daNpc_Ls1_matAnm_c* daNpc_Ls1_matAnm_c_ct(daNpc_Ls1_matAnm_c* i_this) {
    WWHD_FUNC(0x0227DFF0, daNpc_Ls1_matAnm_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ls1_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mbMove = 0;
    i_this->mOffsetX = 0.0f;
    i_this->__vtbl = 0x1001D448;
    i_this->mOffsetY = 0.0f;
    return i_this;
}
VERIFY(0x0227DFF0, daNpc_Ls1_matAnm_c_ct);

/* 0227E048 */
int daNpc_Ls1_c::btpResID(int i_btpNum) {
    WWHD_FUNC(0x0227E048, int, this, i_btpNum);
    return gabi::load<s32>(0x1001D158 + i_btpNum * 4);
}
VERIFY(0x0227E048, &daNpc_Ls1_c::btpResID);

/* 0227E05C */
u32 daNpc_Ls1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x0227E05C, u32, this, i_btpNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    /* HD: no JUT_ASSERT */
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mTimer1 = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}
VERIFY(0x0227E05C, &daNpc_Ls1_c::setBtp);

/* 0227E14C */
int daNpc_Ls1_c::btkResID(int i_btkNum) {
    WWHD_FUNC(0x0227E14C, int, this, i_btkNum);
    return gabi::load<s32>(0x1001D188 + i_btkNum * 4);
}
VERIFY(0x0227E14C, &daNpc_Ls1_c::btkResID);

/* 0227E160 */
void daNpc_Ls1_c::setMat() {
    WWHD_FUNC(0x0227E160, void, this);
    /* HD: the eye materials are found by name (static SafeStrings at 0x10467980) instead of
     * the btk's update material ids */
    u32 btk = gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68);
    u16 material_num = gabi::load<u16>(gabi::load<u32>(btk + 0xC) + 0x14);
    for (u16 i = 0; i < material_num; i++) {
        J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
        u32 name_obj = 0x10467980 + i * 8; /* sead::SafeString {top, vtbl} */
        u32 hdr = gabi::load<u32>(gabi::ea(md));
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(name_obj + 4) + 0x14), name_obj); /* assureTermination */
        u32 off = gabi::load<u32>(hdr + 0x18);
        u32 tab = off != 0 ? hdr + 0x18 + off : 0;
        s32 idx = gabi::call<s32>(0x027DF9B0, tab, gabi::at<const char>(gabi::load<u32>(name_obj))); /* JUTNameTab::getIndex (full register) */
        u32 mat;
        if (idx < 0) {
            mat = 0;
        } else {
            mat = gabi::load<u32>(gabi::ea(md) + 0x10);
            if ((u32)idx < gabi::load<u32>(gabi::ea(md) + 0xC))
                mat += idx * 0x39C;
        }
        mpMatAnms[i] = gabi::at<daNpc_Ls1_matAnm_c>(gabi::load<u32>(mat + 0x24)); /* getMaterialAnm() */
    }
}
VERIFY(0x0227E160, &daNpc_Ls1_c::setMat);

/* 0227E244 */
u32 daNpc_Ls1_c::setBtk(s8 i_btkNum, u32 i_bModify) {
    WWHD_FUNC(0x0227E244, u32, this, i_btkNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btkNum < 0) {
        return false;
    }
    void* a_btk = dComIfG_getObjectIDRes(mArcName, btkResID(i_btkNum));
    mBtkNum = i_btkNum;
    mBtkFrame = 0;
    if (mDoExt_btkAnm_init(mBtkAnm, J3DModel_getModelData_l(morf_model_p), a_btk, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0) {
        if (!i_bModify) {
            setMat();
        }
        return true;
    }
    return false;
}
VERIFY(0x0227E244, &daNpc_Ls1_c::setBtk);

/* 0227E344 */
u32 daNpc_Ls1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x0227E344, u32, this, i_btpNum, i_bModify);
    if (setBtp(i_btpNum, i_bModify) == false) {
        return false;
    }
    /* a_btk_num_tbl (.data 0x1001D194) */
    return setBtk(gabi::load<s8>(0x1001D194 + i_btpNum), i_bModify);
}
VERIFY(0x0227E344, &daNpc_Ls1_c::init_texPttrnAnm);

/* 0227ED64 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0227ED64, BOOL, i_this);
    return static_cast<daNpc_Ls1_c*>(i_this)->CreateHeap();
}
VERIFY(0x0227ED64, CheckCreateHeap);

/* 0227ED68 */
static void* searchActor_Bm1(void* i_actorP, void*) {
    WWHD_FUNC(0x0227ED68, void*, i_actorP, (void*)nullptr);
    if (l_check_wrk() < 20 && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_NPC_BM1_e) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}
VERIFY(0x0227ED68, searchActor_Bm1);

/* 0227EDE8 */
static void* searchActor_kamome_Set_NOSTOP_DEMO(void* i_actorP, void*) {
    WWHD_FUNC(0x0227EDE8, void*, i_actorP, (void*)nullptr);
    if (fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_KAMOME_e) {
        ((fopAc_ac_c*)i_actorP)->actor_status |= 0x4000u; /* fopAcM_OnStatus(fopAcStts_UNK4000_e) */
    }
    return nullptr;
}
VERIFY(0x0227EDE8, searchActor_kamome_Set_NOSTOP_DEMO);

/* 0227EE40 */
static void* searchActor_kamome_Clr_NOSTOP_DEMO(void* i_actorP, void*) {
    WWHD_FUNC(0x0227EE40, void*, i_actorP, (void*)nullptr);
    if (fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_KAMOME_e) {
        ((fopAc_ac_c*)i_actorP)->actor_status &= ~0x4000u;
    }
    return nullptr;
}
VERIFY(0x0227EE40, searchActor_kamome_Clr_NOSTOP_DEMO);

/* 0227EE98 */
u8 daNpc_Ls1_c::decideType(int i_type) {
    WWHD_FUNC(0x0227EE98, u8, this, i_type);
    if (m854 > 0) {
        return true;
    }
    m854 = 1;
    switch ((u32)i_type) {
    case 0:
        mType = -1;
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            mType = 0;
        }
        break;
    case 1:
    case 2:
    case 4:
        mType = (s8)i_type;
        break;
    case 3:
        mType = -1;
        if (!dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            mType = 3;
        }
        break;
    default:
        mType = -1;
        return false;
    }
    if (mType < 0) {
        return false;
    }
    /* strcpy(mArcName, "Ls") (3 bytes from .rodata 0x1001D338) */
    for (int i = 0; i < 3; i++) gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x1001D338 + i));
    return m854 != -1 && mType != -1;
}
VERIFY(0x0227EE98, &daNpc_Ls1_c::decideType);

/* 0227EFF4 */
BOOL daNpc_Ls1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x0227EFF4, BOOL, this, i_newProcFunc, i_argsP);
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
        m856 = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m856 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x0227EFF4, &daNpc_Ls1_c::set_action);

/* 0227F120 */
bool daNpc_Ls1_c::init_LS1_0() {
    WWHD_FUNC(0x0227F120, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    return true;
}
VERIFY(0x0227F120, &daNpc_Ls1_c::init_LS1_0);

/* 0227F178 */
bool daNpc_Ls1_c::init_LS1_1() {
    WWHD_FUNC(0x0227F178, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_demo_action1);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x0227F178, &daNpc_Ls1_c::init_LS1_1);

/* 0227F1B8 / 0227F1BC / 0227F1C0: tail calls, the result register is passed through (u32) */
static u32 init_LS1_2(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x0227F1B8, u32, i_this);
    return gabi::call<u32>(0x0227F178, i_this); /* init_LS1_1() */
}
VERIFY(0x0227F1B8, init_LS1_2);
static u32 init_LS1_3(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x0227F1BC, u32, i_this);
    return gabi::call<u32>(0x0227F120, i_this); /* init_LS1_0() */
}
VERIFY(0x0227F1BC, init_LS1_3);
static u32 init_LS1_4(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x0227F1C0, u32, i_this);
    return gabi::call<u32>(0x0227F178, i_this); /* init_LS1_1() */
}
VERIFY(0x0227F1C0, init_LS1_4);
bool daNpc_Ls1_c::init_LS1_2() { return gabi::call<bool>(0x0227F1B8, this); }
bool daNpc_Ls1_c::init_LS1_3() { return gabi::call<bool>(0x0227F1BC, this); }
bool daNpc_Ls1_c::init_LS1_4() { return gabi::call<bool>(0x0227F1C0, this); }

/* 0227F1C4 */
void daNpc_Ls1_c::setEyeCtrl() {
    WWHD_FUNC(0x0227F1C4, void, this);
    for (int i = 0; i < 2; i++) {
        daNpc_Ls1_matAnm_c* mat_anm_p = mpMatAnms[i];
        if (mat_anm_p) {
            mat_anm_p->mbMove = 1;
        }
    }
    mbEyeCtrlSet = true;
}
VERIFY(0x0227F1C4, &daNpc_Ls1_c::setEyeCtrl);

/* 0227F1FC */
void daNpc_Ls1_c::clrEyeCtrl() {
    WWHD_FUNC(0x0227F1FC, void, this);
    for (int i = 0; i < 2; i++) {
        daNpc_Ls1_matAnm_c* mat_anm_p = mpMatAnms[i];
        if (mat_anm_p) {
            mat_anm_p->mbMove = 0;
        }
    }
    mbEyeCtrlSet = false;
}
VERIFY(0x0227F1FC, &daNpc_Ls1_c::clrEyeCtrl);

/* 0227F234 */
void daNpc_Ls1_c::play_btp_anm() {
    WWHD_FUNC(0x0227F234, void, this);
    u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10))); /* mBtpAnm.getBtpAnm() */
    if (mBtpNum != BTPNUM_MABA_e || cLib_calcTimer(&mTimer1) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= frame_max) {
            if (mBtpNum != BTPNUM_MABA_e) {
                mBtpFrame = frame_max;
            } else {
                mTimer1 = (s16)cLib_getRndValue(60, 90);
                mBtpFrame = 0;
            }
        }
    }
}
VERIFY(0x0227F234, &daNpc_Ls1_c::play_btp_anm);

/* fsel-based clamp to 1.0 (cLib_minMaxLimit's upper bound, NaN kept) */
static inline f32 clamp_m1_1(f32 v) {
    if (v < -1.0f)
        return -1.0f;
    return (v - 1.0f >= 0.0f) ? 1.0f : v;
}

/* 0227F2DC */
void daNpc_Ls1_c::eye_ctrl() {
    WWHD_FUNC(0x0227F2DC, void, this);
    f32 fVar1, fVar2;
    if (mbEyeCtrlSet) {
        f32 div = (f32)(s32)l_HIO().mPrm.m30;
        fVar1 = ((f32)(s32)m848 / div) * 0.1f;
        fVar2 = ((f32)(s32)m846 / div) * 0.1f;
        fVar1 = clamp_m1_1(fVar1);
        fVar2 = clamp_m1_1(fVar2);
    } else {
        fVar1 = 0.0f;
        fVar2 = fVar1;
    }
    if (mpMatAnms[0].get()) {
        cLib_addCalc(&mpMatAnms[0]->mOffsetX, fVar2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&mpMatAnms[0]->mOffsetY, fVar1, 0.5f, 0.1f, 0.03f);
    }
    fVar2 = -fVar2;
    if (mpMatAnms[1].get()) {
        cLib_addCalc(&mpMatAnms[1]->mOffsetX, fVar2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&mpMatAnms[1]->mOffsetY, fVar1, 0.5f, 0.1f, 0.03f);
    }
}
VERIFY(0x0227F2DC, &daNpc_Ls1_c::eye_ctrl);

/* 0227F514 */
void daNpc_Ls1_c::play_btk_anm() {
    WWHD_FUNC(0x0227F514, void, this);
    u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68))); /* mBtkAnm.getBtkAnm() */
    if (m838) {
        eye_ctrl();
    } else {
        u8 frame = (u8)(mBtkFrame + 1);
        if (frame >= frame_max) {
            mBtkFrame = frame_max;
        } else {
            mBtkFrame = frame;
        }
    }
}
VERIFY(0x0227F514, &daNpc_Ls1_c::play_btk_anm);

/* 0227F5AC */
void daNpc_Ls1_c::play_animation() {
    WWHD_FUNC(0x0227F5AC, void, this);
    u32 snd_id = 0;
    if (mBtkNum == 0 && m838 != 0) {
        setEyeCtrl();
    } else {
        clrEyeCtrl();
    }
    play_btp_anm();
    play_btk_anm();
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
VERIFY(0x0227F5AC, &daNpc_Ls1_c::play_animation);

/* 0227F69C */
void daNpc_Ls1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x0227F69C, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = current.pos.y + l_HIO().mPrm.mAttPosOffsetY;
    attPos->x = current.pos.x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mTransformedEyePos.z;
    eyePos.y = mTransformedEyePos.y;
    eyePos.x = mTransformedEyePos.x;
}
VERIFY(0x0227F69C, &daNpc_Ls1_c::setAttention);

/* 0227F6F0 */
void daNpc_Ls1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x0227F6F0, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    J3DModel_calc(mpLsHandModel);
    if (mpTelescopeModel.get()) {
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hnd_R_jnt_num), mDoMtx_stack_c::get());
        if (m841) {
            mDoMtx_stack_c::transM(5.7f, -17.5f, -1.0f);
        } else {
            mDoMtx_stack_c::transM(5.5f, -3.0f, -2.0f);
        }
        f32 s = mTelescopeScale;
        mDoMtx_stack_c::scaleM(s, s, s);
        mDoMtx_XYZrotM(mDoMtx_stack_c::get(), -0x1D27, 0x3B05, -0x5C71);
        J3DModel_setBaseTRMtx(mpTelescopeModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpTelescopeModel);
    }
    setAttention(i_setEyePos);
}
VERIFY(0x0227F6F0, &daNpc_Ls1_c::setMtx);

/* 0227F9A8 */
bool daNpc_Ls1_c::createInit() {
    WWHD_FUNC(0x0227F9A8, bool, this);
    /* l_evn_tbl (.data 0x101BFF4C): "zelda_fly", "omedeto", "get_telescope", "eTalk" */
    for (int i = 0; i < 4; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BFF4C + i * 4));
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2(STR(0x1001D360) /* "Ls1" */, (fopNpc_npc_c*)(void*)this);
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    mBckNum = BCKNUM_NULL_e;
    gravity = 0.0f;
    bool init_result;
    switch (mType) {
    case 0:
        init_result = init_LS1_0();
        break;
    case 1:
        init_result = init_LS1_1();
        break;
    case 2:
        init_result = init_LS1_2();
        break;
    case 3:
        init_result = init_LS1_3();
        break;
    case 4:
        init_result = init_LS1_4();
        break;
    default:
        init_result = false;
        break;
    }
    if (!init_result) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = mAngle.x;
    shape_angle.y = mAngle.y;
    shape_angle.z = mAngle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    play_animation();
    if (mType) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x0227F9A8, &daNpc_Ls1_c::createInit);

/* 0227FD40 */
static cPhs_State daNpc_Ls1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0227FD40, cPhs_State, i_this);
    return ((daNpc_Ls1_c*)i_this)->_create();
}
VERIFY(0x0227FD40, daNpc_Ls1_Create);

/* 0227FD44 */
BOOL daNpc_Ls1_c::_delete() {
    WWHD_FUNC(0x0227FD44, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x0227FD44, &daNpc_Ls1_c::_delete);

/* 0227FD98 */
static BOOL daNpc_Ls1_Delete(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x0227FD98, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0227FD98, daNpc_Ls1_Delete);

/* 0227FD9C */
u8 daNpc_Ls1_c::partner_search_sub(u32 i_judgeFunc) {
    WWHD_FUNC(0x0227FD9C, u8, this, i_judgeFunc);
    bool result = false;
    mBm1ProcID = 0xFFFFFFFF;
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    if (l_check_wrk() != 0) {
        mBm1ProcID = fopAcM_GetID(l_check_inf()[0].get());
        result = true;
    }
    return result;
}
VERIFY(0x0227FD9C, &daNpc_Ls1_c::partner_search_sub);

/* 0227FE48 */
void daNpc_Ls1_c::partner_search() {
    WWHD_FUNC(0x0227FE48, void, this);
    bool temp = false;
    if (m856 == 1) {
        switch ((u32)(s32)mType) {
        case 0:
            temp = partner_search_sub(0x0227ED68 /* searchActor_Bm1 */);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            temp = true;
            break;
        }
        if (temp) {
            m856 = m856 + 1;
        }
    }
}
VERIFY(0x0227FE48, &daNpc_Ls1_c::partner_search);

/* 0227FED0 */
int daNpc_Ls1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x0227FED0, int, this, i_bckNum);
    return gabi::load<s32>(0x1001D374 + i_bckNum * 4);
}
VERIFY(0x0227FED0, &daNpc_Ls1_c::bckResID);

/* 0227FEE4 */
void daNpc_Ls1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x0227FEE4, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    m831 = 0;
}
VERIFY(0x0227FEE4, &daNpc_Ls1_c::setAnm_anm);

/* 0227FF7C */
void daNpc_Ls1_c::setAnm_NUM(int i_anmNum, BOOL param_2) {
    WWHD_FUNC(0x0227FF7C, void, this, i_anmNum, param_2);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BFF84); /* [14] */
    if (param_2) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x0227FF7C, &daNpc_Ls1_c::setAnm_NUM);

/* 02280548 */
void daNpc_Ls1_c::setAnm() {
    WWHD_FUNC(0x02280548, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C0064); /* [6] */
    init_texPttrnAnm(a_anm_prm_tbl[m851].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[m851]);
}
VERIFY(0x02280548, &daNpc_Ls1_c::setAnm);

/* 022805B8 */
void daNpc_Ls1_c::setStt(s8 param_1) {
    WWHD_FUNC(0x022805B8, void, this, param_1);
    s8 prev_m851 = m851;
    m851 = param_1;
    switch ((u32)(s32)param_1) {
    case 1:
        m850 = 0;
        mTimer5 = (s16)cLib_getRndValue(60, 90);
        break;
    case 2:
        m833 = 0;
        mTimer3 = 0;
        m850 = 0;
        break;
    case 3:
        m850 = 0;
        break;
    case 4:
        m850 = 0;
        mTelescopeScale = 1.0f;
        break;
    case 5:
        fpcM_Search(0x0227EDE8 /* searchActor_kamome_Set_NOSTOP_DEMO */, this);
        m850 = 0;
        m857 = 0;
        mMesgAnimeTag = 0xFF;
        m84B = 0xFF;
        m852 = prev_m851;
        break;
    }
    setAnm();
}
VERIFY(0x022805B8, &daNpc_Ls1_c::setStt);

/* 02280508 */
s32 daNpc_Ls1_c::isEventEntry() {
    WWHD_FUNC(0x02280508, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x02280508, &daNpc_Ls1_c::isEventEntry);

/* 022806DC */
void daNpc_Ls1_c::endEvent() {
    WWHD_FUNC(0x022806DC, void, this);
    dComIfGp_event_reset();
    m84B = 0xFF;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x022806DC, &daNpc_Ls1_c::endEvent);

/* 02280720 */
void daNpc_Ls1_c::cut_init_LOK_PLYER(int param_1) {
    WWHD_FUNC(0x02280720, void, this, param_1);
    be<s32>* prm_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001D3B0) /* "prm_0" */);
    m853 = 1;
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
    m840 = 0;
    m_jnt.mbHeadLock = 0;     /* offHeadLock() */
    if (prm_p) {
        s32 prm = *prm_p;
        if (prm & (1 << 3)) {
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            if (prm & 1) {
                m840 = 1;
            }
            if (prm & (1 << 1)) {
                m_jnt.mbBackBoneLock = 1;
            }
            if (prm & (1 << 2)) {
                m_jnt.mbHeadLock = 1;
            }
        }
    }
}
VERIFY(0x02280720, &daNpc_Ls1_c::cut_init_LOK_PLYER);

/* 022807DC */
void daNpc_Ls1_c::cut_init_PLYER_MOV(int i_staffIdx) {
    WWHD_FUNC(0x022807DC, void, this, i_staffIdx);
    dComIfGp_evmng_setGoal(&m7CC[1]);
}
VERIFY(0x022807DC, &daNpc_Ls1_c::cut_init_PLYER_MOV);

/* 02280814 */
void daNpc_Ls1_c::cut_init_ANM_CHG(int param_1) {
    WWHD_FUNC(0x02280814, void, this, param_1);
    be<s32>* anm_no_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001D3B8) /* "AnmNo" */);
    int anm_no = mBckNum;
    if (anm_no_p) {
        anm_no = *anm_no_p;
    }
    setAnm_NUM(anm_no, TRUE);
}
VERIFY(0x02280814, &daNpc_Ls1_c::cut_init_ANM_CHG);

/* 02280884 */
u32 daNpc_Ls1_c::cut_move_WAI() {
    WWHD_FUNC(0x02280884, u32, this);
    if (mBckNum != BCKNUM_WAIT02_e) {
        return TRUE;
    }
    if (mbMorfAnimStopped) {
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02280884, &daNpc_Ls1_c::cut_move_WAI);

/* 022808F4 */
void daNpc_Ls1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x022808F4, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101C00C4): "LOK_PLYER", "PLYER_MOV", "WAI", "ANM_CHG" */
    if (i_staffIdx == -1) {
        return;
    }
    mActionIndex = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101C00C4, 4, TRUE, 0);
    if (mActionIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch (mActionIndex) {
        case 0:
            cut_init_LOK_PLYER(i_staffIdx);
            break;
        case 1:
            cut_init_PLYER_MOV(i_staffIdx);
            break;
        case 2: /* cut_init_WAI: empty */
            break;
        case 3:
            cut_init_ANM_CHG(i_staffIdx);
            break;
        }
    }
    /* cut_move_LOK_PLYER / PLYER_MOV / ANM_CHG return TRUE (inlined) */
    bool cut_end = true;
    if (mActionIndex == 2) {
        cut_end = cut_move_WAI();
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x022808F4, &daNpc_Ls1_c::privateCut);

/* 02280A1C */
void daNpc_Ls1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x02280A1C, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0:
            /* HD: no scope message status check, no camera restart and no player SCOPE_CANCEL
             * flag; after releasing the scope mode the event ends in the same frame */
            if (!m835) {
                if (!gabi::call<BOOL>(0x025F8BEC, gabi::load<u32>(0x101F4B5C))) { /* fopMsgM_releaseScopeMode() */
                    return;
                }
                gabi::call(0x025E1904, 4);           /* mDoAud_bgmStop(4) */
                gabi::call(0x025E18EC, 0x8000000Eu); /* mDoAud_bgmStart(0x8000000E) */
                m835 = true;
            }
            dComIfGs_onEventBit(1);
            setStt(3);
            m840 = 0;
            gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIDTbl[3]); /* eventInfo.setEventId(mEventIDTbl[3]) */
            m850 = 1;
            mEventIndex = 3;
            m83B = false;
            m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
            m834 = false;
            break;
        case 1:
            m850 = 5;
            break;
        case 2:
            setStt(2);
            m850 = 1;
            m836 = true;
            break;
        }
        endEvent();
    } else {
        if (mEventCut.cutProc()) {
            return;
        }
        privateCut(i_staffIdx);
    }
}
VERIFY(0x02280A1C, &daNpc_Ls1_c::event_proc);

/* 02281004 */
void daNpc_Ls1_c::eventOrder() {
    WWHD_FUNC(0x02281004, void, this);
    if (m850 == 1 || m850 == 2) {
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (m850 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (m850 >= 3) {
        mEventIndex = m850 - 3;
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02281004, &daNpc_Ls1_c::eventOrder);

/* 02281314 */
static BOOL daNpc_Ls1_Execute(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x02281314, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02281314, daNpc_Ls1_Execute);

/* 02281558 */
static BOOL daNpc_Ls1_Draw(daNpc_Ls1_c* i_this) {
    WWHD_FUNC(0x02281558, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02281558, daNpc_Ls1_Draw);

/* 0228155C */
static BOOL daNpc_Ls1_IsDelete(daNpc_Ls1_c*) {
    WWHD_FUNC(0x0228155C, BOOL, (daNpc_Ls1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0228155C, daNpc_Ls1_IsDelete);

/* 02281564 daNpc_Ls1_matAnm_c::calc(J3DMaterial*) (HD: the texture matrix is found through the
 * material's resource tables by the name "texmtx1"; the GameCube loops over 8 texture matrices) */
static void daNpc_Ls1_matAnm_c_calc(daNpc_Ls1_matAnm_c* i_this, void* i_material) {
    WWHD_FUNC(0x02281564, void, i_this, i_material);
    u32 mat = gabi::ea(i_material);
    auto selfrel = [](u32 base) -> u32 { u32 off = gabi::load<u32>(base); return off != 0 ? base + off : 0; };
    u32 hdr = gabi::load<u32>(mat);
    s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x38), STR(0x1001D3F0) /* "texmtx1" */);
    hdr = gabi::load<u32>(mat);
    u32 entry = selfrel(hdr + 0x34) + idx * 0x14;
    if (gabi::load<s32>(entry + 4) >= 0) {
        gabi::store<u16>(mat + 4, gabi::load<u16>(mat + 4) | 4);
        u32 bits = gabi::load<u32>(mat + 0xC) + ((idx >> 5) << 2);
        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx & 31)));
        hdr = gabi::load<u32>(mat);
    }
    u32 idx2 = gabi::load<u16>(entry + 0xC);
    u32 entry2 = selfrel(hdr + 0x34) + idx2 * 0x14;
    if (gabi::load<s32>(entry2 + 4) >= 0) {
        gabi::store<u16>(mat + 4, gabi::load<u16>(mat + 4) | 4);
        u32 bits = gabi::load<u32>(mat + 0xC) + (((s32)idx2 >> 5) << 2);
        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx2 & 31)));
    }
    u32 srt = gabi::load<u32>(mat + 0x28) + gabi::load<u16>(entry + 2);
    if (i_this->mbMove != 0) {
        gabi::store<f32>(srt + 0x10, i_this->mOffsetX);
        gabi::store<f32>(srt + 0x14, i_this->mOffsetY);
    }
}
VERIFY(0x02281564, daNpc_Ls1_matAnm_c_calc);

/* 022816A4 */
u32 daNpc_Ls1_c::chngAnmTag() {
    WWHD_FUNC(0x022816A4, u32, this);
    u8 tag = mMesgAnimeTag;
    if (tag == 0) {
        return init_texPttrnAnm(BTPNUM_WARAI_e, true);
    } else if (tag >= 11 && tag <= 13) {
        /* 11: KIZUKU, 12: NGWARAI, 13: FUAN (.rodata 0x1001D3F8) */
        return init_texPttrnAnm(gabi::load<u8>(0x1001D3ED + tag), true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022816A4, &daNpc_Ls1_c::chngAnmTag);

/* 022816E4 */
void daNpc_Ls1_c::setAnm_ATR(BOOL param_1) {
    WWHD_FUNC(0x022816E4, void, this, param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C00D4); /* [17] */
    if (param_1) {
        init_texPttrnAnm(a_anm_prm_tbl[m84B].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[m84B]);
}
VERIFY(0x022816E4, &daNpc_Ls1_c::setAnm_ATR);

static inline u32 g_Counter_mCounter0() { return gabi::load<u32>(0x101FF558); }

/* 02281758 */
void daNpc_Ls1_c::chngAnmAtr(u8 param_1) {
    WWHD_FUNC(0x02281758, void, this, param_1);
    if ((m84B == 3 || m84B == 5) && (param_1 == 3 || param_1 == 5)) {
        init_texPttrnAnm(BTPNUM_WARAI_e, true);
        m84B = param_1;
        return;
    }
    /* HD: >= 17 (GameCube: > 17) */
    if (param_1 == m84B || param_1 >= 17) {
        return;
    }
    m84B = param_1;
    setAnm_ATR(TRUE);
    switch (param_1) {
    case 1:
        mTimer2 = (s16)cLib_getRndValue(60, 90);
        break;
    case 3:
    case 5:
        mTimer2 = (g_Counter_mCounter0() & 3) + 3;
        break;
    case 7:
        mTelescopeScale = 0.0f;
        break;
    case 12:
    case 14:
    case 15:
    case 16:
        mTimer2 = 2;
        break;
    }
}
VERIFY(0x02281758, &daNpc_Ls1_c::chngAnmAtr);

/* 022818EC */
void daNpc_Ls1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x022818EC, void, this);
    switch (m84B) {
    case 1:
        if (cLib_calcTimer(&mTimer2) == 0) {
            m84B = 2;
            setAnm_ATR(TRUE);
        }
        break;
    case 2:
        if (mbMorfAnimStopped) {
            m84B = 1;
            setAnm_ATR(TRUE);
        }
        break;
    case 3:
    case 5:
        if (mbMorfAnimStopped && cLib_calcTimer(&mTimer2) == 0) {
            mTimer2 = (g_Counter_mCounter0() & 3) + 3;
            switch (m84B) {
            case 3:
                m84B = 5;
                break;
            case 5:
                m84B = 3;
                break;
            }
            setAnm_ATR(FALSE);
        }
        break;
    case 7:
        if (mpMorf->checkFrame(23.0f) != 0) {
            mTelescopeScale = 1.0f;
        }
        break;
    case 12:
    case 14:
    case 15:
    case 16:
        if (mbMorfAnimStopped && cLib_calcTimer(&mTimer2) == 0) {
            switch (m84B) {
            case 12:
                if (mBckNum != BCKNUM_WAIT07_e) {
                    setAnm_NUM(ANMNUM_WAIT07_e, TRUE);
                    init_texPttrnAnm(BTPNUM_FUAN02_e, true);
                }
                break;
            case 14:
                if (mBckNum != BCKNUM_WAIT06_e) {
                    setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            case 15:
                if (mBckNum != BCKNUM_TALK01_e) {
                    setAnm_NUM(BCKNUM_TALK01_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            case 16:
                if (mBckNum != BCKNUM_WAIT05_e) {
                    setAnm_NUM(BCKNUM_WAIT05_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            }
        }
        break;
    }
}
VERIFY(0x022818EC, &daNpc_Ls1_c::ctrlAnmAtr);

/* 02281C14 */
void daNpc_Ls1_c::anmAtr(u16 param_1) {
    WWHD_FUNC(0x02281C14, void, this, param_1);
    switch (param_1) {
    case 6: {
        if (m857 == 0) {
            chngAnmAtr(gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925) /* dComIfGp_getMesgAnimeAttrInfo() */);
            m857 = m857 + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            mMesgAnimeTag = tag;
            chngAnmTag();
        }
        break;
    }
    case 14:
        m857 = 0;
        break;
    }
    ctrlAnmAtr();
    /* ctrlAnmTag(): empty */
}
VERIFY(0x02281C14, &daNpc_Ls1_c::anmAtr);

/* 02280BEC: the matcher calls it cLib_getRndValue<int>; it is searchByID */
fopAc_ac_c* daNpc_Ls1_c::searchByID(fpc_ProcID i_PID, be<s32>* param_2) {
    WWHD_FUNC(0x02280BEC, fopAc_ac_c*, this, i_PID, param_2);
    gabi::Local<gptr<fopAc_ac_c>> actor_p;
    *actor_p = nullptr;
    *param_2 = 0;
    if (!gabi::call<BOOL>(0x025D54C4, i_PID, actor_p.get())) { /* fopAcM_SearchByID(id, &actor) */
        *param_2 = 1;
    }
    return *actor_p;
}
VERIFY(0x02280BEC, &daNpc_Ls1_c::searchByID);

/* 02281CDC */
u16 daNpc_Ls1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x02281CDC, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*o_msgNoP) {
    case 0xBB9:
        if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) /* dComIfGs_getClearCount() */) {
            *o_msgNoP = 0xBDA;
            break;
        }
        *o_msgNoP = 0xBBA;
        break;
    case 0xBBA:
    case 0xBDA:
        *o_msgNoP = 0xBBB;
        break;
    case 0xBBB:
        *o_msgNoP = 0xBBC;
        break;
    case 0xBBC:
        *o_msgNoP = 0xBBD;
        break;
    case 0xBBD:
        *o_msgNoP = 0xBDC;
        break;
    case 0xBBE:
        *o_msgNoP = 0xBBF;
        break;
    case 0xBC5:
        *o_msgNoP = 0xBC6;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x02281CDC, &daNpc_Ls1_c::next_msgStatus);

/* 02281DA4 */
u32 daNpc_Ls1_c::getMsg_LS1_0() {
    WWHD_FUNC(0x02281DA4, u32, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        if (m835) {
            /* HD: m835 is cleared only while play+0x5BB3 is 0 (GameCube: always) */
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BB3) == 0) {
                m835 = false;
            }
            return dComIfGs_checkCollect(0) != 0 ? 0xBC4 : 0xBC5;
        }
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_0280)) {
            return 0xBC9;
        }
        return dComIfGs_checkCollect(0) != 0 ? 0xBC8 : 0xBC7;
    }
    if (m836) {
        m836 = false;
        return 0xBBE;
    }
    if (dComIfGp_getSelectItem(0) == 0x20 /* dItemNo_TELESCOPE_e */) {
        return 0xBCB;
    }
    if (dComIfGp_getSelectItem(1) == 0x20) {
        return 0xBCC;
    }
    if (dComIfGp_getSelectItem(2) == 0x20) {
        return 0xBCD;
    }
    return 0xBC0;
}
VERIFY(0x02281DA4, &daNpc_Ls1_c::getMsg_LS1_0);

/* 02281F64 */
u32 daNpc_Ls1_c::getMsg() {
    WWHD_FUNC(0x02281F64, u32, this);
    u32 result = 0;
    switch (mType) {
    case 0:
        result = getMsg_LS1_0();
        break;
    case 3:
        result = 0xBD9; /* getMsg_LS1_3() */
        break;
    }
    return result;
}
VERIFY(0x02281F64, &daNpc_Ls1_c::getMsg);

/* 02281FAC */
u8 daNpc_Ls1_c::chk_talk() {
    WWHD_FUNC(0x02281FAC, u8, this);
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
VERIFY(0x02281FAC, &daNpc_Ls1_c::chk_talk);

/* 02282044 */
u8 daNpc_Ls1_c::chk_parts_notMov() {
    WWHD_FUNC(0x02282044, u8, this);
    return mJointHeadY != m_jnt.mAngles[0][1] || mJointBackboneY != m_jnt.mAngles[1][1] || mActorAngleY != current.angle.y;
}
VERIFY(0x02282044, &daNpc_Ls1_c::chk_parts_notMov);

/* 02282084 */
u8 daNpc_Ls1_c::chkAttention() {
    WWHD_FUNC(0x02282084, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02282084, &daNpc_Ls1_c::chkAttention);

/* 0228210C */
u8 daNpc_Ls1_c::chk_areaIN(f32 param_1, f32 param_2, s16 param_3, cXyz* param_4) {
    WWHD_FUNC(0x0228210C, u8, this, param_1, param_2, param_3, param_4);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, param_4);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 abs_xz = std_sqrtf(PSVECSquareMag(xz));
    f32 fVar1 = dComIfGp_getLinkPlayer()->current.pos.y - param_4->y;
    s16 sVar = cLib_targetAngleY(param_4, &dComIfGp_getLinkPlayer()->current.pos) - current.angle.y;
    if (abs_xz < param_1 && std::fabs(fVar1) < param_2 && abs((int)sVar) < (int)param_3) {
        return true;
    }
    return false;
}
VERIFY(0x0228210C, &daNpc_Ls1_c::chk_areaIN);

/* 02282290: returns a cXyz through the hidden result pointer (r4; HD: a NULL result pointer
 * allocates the object) */
void daNpc_Ls1_c::get_playerEvnPos(cXyz* o_pos, int param_1) {
    WWHD_FUNC(0x02282290, void, this, o_pos, param_1);
    /* const Vec pos[] = {{200, 0, -200}, {100, 0, -80}} (copied from .rodata 0x1001D400) */
    /* the original frame: sp08 at +8, pos[2] at +0x14 (an out-of-range index points elsewhere in it) */
    gabi::Local<u8[0x40]> frame;
    cXyz* sp08 = gabi::at<cXyz>(gabi::ea(frame.get()) + 8);
    cXyz* pos = gabi::at<cXyz>(gabi::ea(frame.get()) + 0x14);
    for (u32 i = 0; i < 24; i += 4) gabi::store<u32>(gabi::ea(pos) + i, gabi::load<u32>(0x1001D400 + i));
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(gabi::ea(pos) + param_1 * 12), sp08);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->x = sp08->x;
    o_pos->y = sp08->y;
    o_pos->z = sp08->z;
}
VERIFY(0x02282290, &daNpc_Ls1_c::get_playerEvnPos);

/* 025F0C48 mDoLib_project(cXyz* pos, cXyz* out) */
static inline void mDoLib_project(cXyz* pos, cXyz* out) { gabi::call(0x025F0C48, pos, out); }

/* 02282380 (the matcher calls it chkTelescope; it is chkTelescope_sph) */
u8 daNpc_Ls1_c::chkTelescope_sph(cXyz* param_1, f32 param_2, f32 param_3) {
    WWHD_FUNC(0x02282380, u8, this, param_1, param_2, param_3);
    dComIfGp_get(); /* HD: an unused accessor call */
    gabi::Local<cXyz> sp18;
    mDoLib_project(param_1, sp18);
    f32 x = sp18->x, y = sp18->y;
    gabi::Local<cXyz> sp0C;
    sp18->z = y;
    sp0C->x = x - 319.5f;
    sp18->y = 0.0f;
    sp0C->z = y - 186.5f;
    bool chk = false;
    sp0C->y = 0.0f;
    if (gabi::call<BOOL>(0x0201B47C, sp0C.get())) { /* sp0C.normalizeRS() */
        PSVECScale(sp0C, sp0C, param_2);        /* sp0C *= param_2 */
        PSVECAdd(sp18, sp0C, sp18);             /* sp18 += sp0C */
        f32 dz = sp18->z - 186.5f;
        f32 dx = sp18->x - 319.5f;
        chk = param_3 > std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    }
    return chk;
}
VERIFY(0x02282380, &daNpc_Ls1_c::chkTelescope_sph);

/* 022824B0 chkTelescope */
u8 daNpc_Ls1_c::chkTelescope(cXyz* param_1, f32 param_2, f32 param_3) {
    WWHD_FUNC(0x022824B0, u8, this, param_1, param_2, param_3);
    /* dComIfGd_getView()->mFovy (view at play+0x5FA4, +0xD4) */
    f32 fov_y = gabi::load<f32>(gabi::load<u32>(dComIfGp_ea() + 0x5FA4) + 0xD4) / 40.0f;
    if (fov_y > 1.0f) {
        fov_y = 1.0f;
    }
    gabi::Local<cXyz> sp14;
    mDoLib_project(param_1, sp14);
    f32 x = (sp14->x - 319.5f) * fov_y;
    f32 z = (sp14->y - 186.5f) * fov_y;
    return std::fabs(x) < param_2 && z > param_3;
}
VERIFY(0x022824B0, &daNpc_Ls1_c::chkTelescope);

/* HD message manager (*(0x101F4B5C)) scope helpers */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline void fopMsgM_getScopeMode(u32 m) { gabi::call(0x025F8B5C, m); }
static inline BOOL fopMsgM_releaseScopeMode(u32 m) { return gabi::call<BOOL>(0x025F8BEC, m); }
static inline void fopMsgM_scopeMessageSet(u32 m, u32 no) { gabi::call(0x025F7E68, m, no); }
static inline void fopMsgM_forceSendOn(u32 m) { gabi::store<u8>(m + 0x928, 1); }
/* camera 0 (play+0x5AF8): dCamera_c at +0x248, flags at +0x758 */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }

/* 022825A0 */
u8 daNpc_Ls1_c::telescope_proc() {
    WWHD_FUNC(0x022825A0, u8, this);
    gabi::Local<be<s32>> param;
    fopAc_ac_c* bm1_npc_p = searchByID(mBm1ProcID, param);
    if (!bm1_npc_p || *param == 1) {
        return false;
    }
    u8 scope_mesg_status = gabi::load<u8>(dComIfGp_ea() + 0x5BB3); /* dComIfGp_getScopeMesgStatus() */
    if (scope_mesg_status == 0) {
        if (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x00200000) { /* daPyStts0_TELESCOPE_LOOK_e */
            if (gabi::load<u8>(dComIfGp_ea() + 0x5C21) /* mScopeWipeFlag */) {
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                u32 vt = gabi::load<u32>(gabi::ea(link) + 0xB4);
                gabi::call_ptr(gabi::load<u32>(vt + 0x114), link, &m7CC[1], (s16)-0x3390); /* setPlayerPosAndAngle(&m7CC[1], 0xCC70) */
            }
            gabi::store<u8>(dComIfGp_ea() + 0x5BCF, 1); /* dComIfGp_setScopeType(1) */
            return true;
        }
        return false;
    }
    u32 msg = msgMgr();
    if (m833 == 0) {
        gabi::call(0x025E1934, 0xC0000001u); /* mDoAud_bgmStreamPrepare(JA_STRM_DEMO_TETRA_FLY) */
        gabi::store<u8>(gabi::ea(bm1_npc_p) + 0x9F1, 1); /* bm1->setTelescopeDemo() */
        m833 = 1;
        mTimer5 = 150;
        m810 = 0xFFFFFFFF;
        fopMsgM_getScopeMode(msg);
        return true;
    }
    u8 m833v = m833;
    if (scope_mesg_status == 0x12 /* BOX_CLOSED */) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BB3, 0x11); /* setScopeMesgStatus(BOX_CLOSING) */
        if (m833 == 3 && m810 != 0) {
            u32 cam = dComIfGp_getCamera0();
            gabi::store<u32>(cam + 0x758, gabi::load<u32>(cam + 0x758) & ~0x01000000u); /* clrFlag(0x800000) */
            gabi::call(0x0251544C, dComIfGp_getCamera0() + 0x248);                   /* SubjectLockOff() */
            dComIfGs_onEventBit(dSv_event_flag_UNK_0310);
            mTimer5 = 0;
            m833 = 4;
            fopMsgM_getScopeMode(msg);
            return true;
        }
        m833v = m833;
    } else if (scope_mesg_status != 0xA /* CLOSE_WAIT */ && scope_mesg_status != 0xD /* SCOPE_WAIT */) {
        fopMsgM_getScopeMode(msg);
        return true;
    }
    u32 temp_msg_no = 0;
    switch (m833v) {
    case 1: {
        temp_msg_no = m810;
        m810 = 0xBC1;
        gabi::Local<cXyz> att;
        att->x = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x390);
        att->y = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x394);
        att->z = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x398);
        if (chkTelescope_sph(att, 0.0f, l_HIO().mPrm.m2C)) {
            /* HD: zoom < m24 ? 0xBCA : 0xBCB (dComIfGp_getCameraZoomScale: play+0x5B04) */
            m810 = gabi::load<f32>(dComIfGp_ea() + 0x5B04) < l_HIO().mPrm.m24 ? 0xBCA : 0xBCB;
        }
        m833v = m833;
        break;
    }
    case 2:
        m810 = 0xBCA;
        if (!(gabi::load<f32>(dComIfGp_ea() + 0x5B04) < l_HIO().mPrm.m28)) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msgMgr());
            }
            u32 cam = dComIfGp_getCamera0();
            gabi::store<u32>(cam + 0x758, gabi::load<u32>(cam + 0x758) | 0x01000000u); /* setFlag(0x800000) */
            gabi::store<u8>(gabi::ea(bm1_npc_p) + 0x9F2, 1);                          /* bm1->setFocus() */
            mTimer5 = 0;
            m810 = 0;
            m833 = 3;
            fopMsgM_getScopeMode(msg);
            return true;
        }
        m833v = m833;
        break;
    case 3:
        if (m810 == 0 && gabi::load<u8>(gabi::ea(bm1_npc_p) + 0x9F0) /* bm1->getOdoroki() */) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msg);
            } else if (fopMsgM_releaseScopeMode(msg)) {
                m810 = 0xBC2;
                fopMsgM_scopeMessageSet(msg, 0xBC2);
            }
        }
        fopMsgM_getScopeMode(msg);
        return true;
    case 4: {
        gabi::Local<cXyz> pos;
        f32 y = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x394);
        f32 z = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x398);
        f32 x = gabi::load<f32>(gabi::ea(bm1_npc_p) + 0x390);
        pos->z = z;
        pos->y = y + 2000.0f;
        pos->x = x;
        /* HD: 90/30 (GameCube: 60/-30) */
        if (chkTelescope(pos, 90.0f, 30.0f)) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msg);
                m833v = m833;
                break;
            }
            if (fopMsgM_releaseScopeMode(msg)) {
                m850 = 3;
            }
        }
        m810 = 0xBC3;
        m833v = m833;
        break;
    }
    }
    if ((m833v == 1 && temp_msg_no != m810) || cLib_calcTimer(&mTimer5) == 0) {
        if (scope_mesg_status == 0xA) {
            fopMsgM_forceSendOn(msg);
            mTimer5 = 0;
        } else if (fopMsgM_releaseScopeMode(msg)) {
            mTimer5 = 150;
            if (m833 == 1 && m810 == 0xBCB) {
                gabi::call(0x02515434, dComIfGp_getCamera0() + 0x248, bm1_npc_p); /* SubjectLockOn(bm1) */
                m810 = 0xBCA;
                mTimer5 = 150;
                m833 = 2;
            }
            if (m810 == 0xBC3) {
                mTimer3 = (s16)cLib_getRndValue(30, 30);
            }
            fopMsgM_scopeMessageSet(msg, m810);
        }
    }
    fopMsgM_getScopeMode(msg);
    return true;
}
VERIFY(0x022825A0, &daNpc_Ls1_c::telescope_proc);

/* 02282A60 */
BOOL daNpc_Ls1_c::wait_1() {
    WWHD_FUNC(0x02282A60, BOOL, this);
    if (m850 < 3) {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        m850 = 0;
        if (chk_areaIN(l_HIO().mPrm.m20, 100.0f, 0x7FFF, pos)) {
            m850 = 4;
        }
    }
    m853 = 0;
    if (mBckNum == BCKNUM_WAIT01_e) {
        if (cLib_calcTimer(&mTimer5) == 0) {
            setAnm_NUM(ANMNUM_WAIT02_e, TRUE);
        }
        return true;
    }
    if (mbMorfAnimStopped) {
        mTimer5 = (s16)cLib_getRndValue(60, 90);
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
    }
    return true;
}
VERIFY(0x02282A60, &daNpc_Ls1_c::wait_1);

/* 02282B54 */
BOOL daNpc_Ls1_c::wait_2() {
    WWHD_FUNC(0x02282B54, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, m7A0.y, 4, 0x800, 0x80);
    if (current.angle.y == m7A0.y && mBckNum != BCKNUM_WAIT01_e) {
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
    }
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
            m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
            m840 = 0;
            m853 = 1;
            m_jnt.mbTrn = 1;          /* setTrn() */
        }
        return TRUE;
    }
    if (m850 != 1 && m850 < 3) {
        bool telescope_proc_result = false;
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, y = current.pos.y;
        pos->x = x;
        pos->y = y;
        pos->z = current.pos.z;
        m850 = 0;
        if (chk_areaIN(l_HIO().mPrm.m1C, 100.0f, 0x7FFF, pos) &&
            (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x00200000) /* daPyStts0_TELESCOPE_LOOK_e */) {
            telescope_proc_result = telescope_proc();
        }
        if (!telescope_proc_result) {
            m850 = 2;
        }
    }
    m_jnt.mbBackBoneLock = 1; /* onBackBoneLock() */
    m840 = 1;
    if (m833 != 0) {
        gabi::Local<cXyz> base_pos;
        base_pos->x = 20.0f;
        base_pos->y = 0.0f;
        base_pos->z = 50.0f;
        if (m833 >= 4) {
            if (mBtpNum != BTPNUM_FUAN02_e) {
                init_texPttrnAnm(BTPNUM_FUAN02_e, true);
            }
            if (cLib_calcTimer(&mTimer3) != 0) {
                m853 = 1;
                return TRUE;
            }
            base_pos->y = 120.0f;
        }
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), base_pos, &mPlayerEyePos);
        m853 = 2;
        m838 = true;
        return TRUE;
    }
    if (mbAttention && mBckNum == BCKNUM_WAIT01_e) {
        m853 = 1;
        return TRUE;
    }
    m853 = 0;
    return TRUE;
}
VERIFY(0x02282B54, &daNpc_Ls1_c::wait_2);

/* 02282D94 */
BOOL daNpc_Ls1_c::wait_3() {
    WWHD_FUNC(0x02282D94, BOOL, this);
    gabi::Local<cXyz> local_18;
    local_18->x = 40.0f;
    local_18->y = 140.0f;
    local_18->z = 100.0f;
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m7A0.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), local_18, &mPlayerEyePos);
    cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(&current.pos, &mPlayerEyePos), 4, 0x800, 0x80);
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            m_jnt.mbTrn = 1; /* setTrn() */
            m840 = 0;
            m853 = 1;
        }
        return true;
    }
    if (m850 != 1) {
        m850 = 2;
    }
    m840 = 1;
    m853 = 2;
    m838 = true;
    return true;
}
VERIFY(0x02282D94, &daNpc_Ls1_c::wait_3);

/* 02282EB0 */
BOOL daNpc_Ls1_c::wait_4() {
    WWHD_FUNC(0x02282EB0, BOOL, this);
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            m853 = 1;
            m840 = 0;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
        return true;
    }
    m850 = 2;
    m840 = 1;
    if (mbAttention) {
        mTimer4 = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&mTimer4) != 0) {
        m853 = 1;
        return true;
    }
    m853 = 0;
    return true;
}
VERIFY(0x02282EB0, &daNpc_Ls1_c::wait_4);

/* 02282F6C */
BOOL daNpc_Ls1_c::talk_1() {
    WWHD_FUNC(0x02282F6C, BOOL, this);
    BOOL res = chk_parts_notMov();
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (!mbHasMsg) {
        return res;
    }
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0xBC4:
        case 0xBC6:
            gabi::store<s16>(gabi::ea(this) + 0xFC, -1); /* eventInfo.setEventId(-1) */
            break;
        }
        fpcM_Search(0x0227EE40 /* searchActor_kamome_Clr_NOSTOP_DEMO */, this);
        mItemNo = 0xFF;
        m83F = false;
        setStt(m852);
        mTimer4 = (s16)cLib_getRndValue(15, 30);
        endEvent();
    }
    return res;
}
VERIFY(0x02282F6C, &daNpc_Ls1_c::talk_1);

/* 02283038 */
BOOL daNpc_Ls1_c::wait_action1(void*) {
    WWHD_FUNC(0x02283038, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)m856) {
    case 0: {
        gabi::Local<cXyz> tmp;
        get_playerEvnPos(tmp, 0);
        m7CC[0].copy(*tmp);
        get_playerEvnPos(tmp, 1);
        m7CC[1].copy(*tmp);
        m856 = m856 + 1;
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            if (dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
                setStt(3);
            } else if (gabi::call<BOOL>(0x02520C0C, 0x20) /* dComIfGs_checkGetItem(dItemNo_TELESCOPE_e) */) {
                m7A0.y = current.angle.y;
                setStt(2);
            } else {
                setStt(1);
            }
        } else {
            setStt(4);
        }
        break;
    }
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)m851) {
        case 1:
            mbSetEyePos = wait_1();
            break;
        case 2:
            mbSetEyePos = wait_2();
            break;
        case 3:
            mbSetEyePos = wait_3();
            break;
        case 4:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x02283038, &daNpc_Ls1_c::wait_action1);

/* 02283228 */
BOOL daNpc_Ls1_c::demo_action1(void*) {
    WWHD_FUNC(0x02283228, BOOL, this, (void*)nullptr);
    if (m856 == 0) {
        setAnm_NUM(mType == 1 ? ANMNUM_DEMOWAIT_e : ANMNUM_WAIT06_e, TRUE);
        m856 = m856 + 1;
    }
    return TRUE;
}
VERIFY(0x02283228, &daNpc_Ls1_c::demo_action1);

/* 0227FFEC */
void daNpc_Ls1_c::checkOrder() {
    WWHD_FUNC(0x0227FFEC, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && m850 >= 3) {
            switch ((u32)(s32)mEventIndex) {
            case 0:
                gabi::call(0x02514F2C, dComIfGp_getCamera0() + 0x248); /* dComIfGp_getCamera(0)->mCamera.Stop() */
                gabi::call(0x025E1944);                                /* mDoAud_bgmStreamPlay() */
                m834 = true;
                break;
            case 1:
                cLib_targetAngleY(&m7CC[0], &current.pos);
                m83B = true;
                break;
            case 2:
                mTelescopeScale = 0.0f;
                setAnm_NUM(ANMNUM_GET_e, TRUE);
                break;
            }
            m850 = 0;
            mMesgAnimeTag = 0xFF;
            m84B = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (m850 == 1 || m850 == 2) {
            m850 = 0;
            m83F = true;
            /* HD: the camera is restarted here */
            gabi::call(0x02514F38, dComIfGp_getCamera0() + 0x248); /* dComIfGp_getCamera(0)->mCamera.Start() */
        }
    }
}
VERIFY(0x0227FFEC, &daNpc_Ls1_c::checkOrder);

/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}

/* 02280C40 */
void daNpc_Ls1_c::lookBack() {
    WWHD_FUNC(0x02280C40, void, this);
    mJointHeadY = m_jnt.mAngles[0][1];
    gabi::Local<cXyz> player_eye_pos;
    player_eye_pos->set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mActorAngleY = target_y;
    f32 srcY = eyePos.y;
    u8 temp_m840 = m840;
    f32 srcZ = current.pos.z;
    f32 srcX = current.pos.x;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* player_eye_pos_p = nullptr;

    switch ((u32)(s32)m853) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, l_HIO().mPrm.mPlayerEyePosOffsetY);
        mPlayerEyePos.copy(*eye);
        player_eye_pos->copy(*eye);
        player_eye_pos_p = player_eye_pos;
        m838 = true;
        break;
    }
    case 2:
        player_eye_pos->copy(mPlayerEyePos);
        player_eye_pos_p = player_eye_pos;
        break;
    case 3:
        target_y = m82E;
        m838 = false;
        break;
    case 4: {
        gabi::Local<be<s32>> param;
        fopAc_ac_c* actor_p = searchByID(m790, param);
        if (actor_p && *param == 0) {
            mPlayerEyePos.copy(actor_p->current.pos);
            mPlayerEyePos.y = actor_p->eyePos.y;
            player_eye_pos->x = mPlayerEyePos.x;
            player_eye_pos->y = mPlayerEyePos.y;
            player_eye_pos->z = mPlayerEyePos.z;
            player_eye_pos_p = player_eye_pos;
            m838 = true;
        }
        break;
    }
    default:
        m838 = false;
        break;
    }
    gabi::Local<cXyz> current_pos; /* passed by value: a copy */
    current_pos->x = srcX;
    current_pos->y = srcY;
    current_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, player_eye_pos_p, current_pos, target_y, l_HIO().mPrm.m12, temp_m840);
    s16 halfX = (s16)(m_jnt.mAngles[0][0] / 2);
    mHalfHeadAngleX = halfX;
    s16 halfY = (s16)(m_jnt.mAngles[0][1] / 2);
    m848 = halfX;
    mHalfHeadAngleY = halfY;
    m846 = -halfY;
}
VERIFY(0x02280C40, &daNpc_Ls1_c::lookBack);

/* 022801BC */
u8 daNpc_Ls1_c::demo() {
    WWHD_FUNC(0x022801BC, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (m841) {
            if (mType == 3) {
                mBckNum = BCKNUM_NULL_e;
                setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
            }
            m841 = false;
        }
        return m841;
    }
    if (!m841) {
        for (int i = 0; i < 2; i++) {
            if (mpMatAnms[i].get()) {
                mpMatAnms[i]->mbMove = 0; /* clrMoveFlag() */
            }
        }
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        m841 = true;
        m83B = false;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001D140), 0x23A, STR(0x1001D114));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    void* btp_anm = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10));
    if (btp_anm) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btp_anm);
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = BTPNUM_NULL_e;
        }
    }
    void* btk_anm = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68));
    if (btk_anm) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btk_anm);
        u8 frame = (u8)(mBtkFrame + 1);
        mBtkFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btk_p = gabi::call<void*>(0x025279C8, demo_actor_p, mArcName); /* getP_BtkData */
        if (demo_btk_p) {
            mDoExt_btkAnm_init(mBtkAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btk_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtkFrame = 0;
            mBtkNum = 3; /* BTKNUM_NULL_e */
        }
        u32 da = gabi::ea(demo_actor_p);
        if (gabi::load<u16>(da + 4) & 0x10) { /* checkEnable(ENABLE_SHAPE_e) */
            mTelescopeScale = gabi::load<s32>(da + 0x28) == 1 ? 1.0f : 0.0f; /* getShapeId() */
        }
        /* HD: for type 3 the demo actor's flag 0x40 is cleared */
        if (mType == 3) {
            gabi::store<u16>(da + 4, gabi::load<u16>(da + 4) & 0xFFBF);
        }
    }
    gabi::Local<cXyz> pos; /* passed by value */
    f32 z = current.pos.z, y = current.pos.y;
    pos->z = z;
    pos->y = y;
    pos->x = current.pos.x;
    u32 snd = gabi::call<u32>(0x024F1914, pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(pos, 10.0f) */
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), mArcName, 0, 0, snd, reverb); /* dDemo_setDemoData */
    return m841;
}
VERIFY(0x022801BC, &daNpc_Ls1_c::demo);

/* 02281074 */
BOOL daNpc_Ls1_c::_execute() {
    WWHD_FUNC(0x02281074, BOOL, this);
    if (!m83D) {
        m7A0.y = current.angle.y;
        m794.copy(current.pos);
        m7A0.x = current.angle.x;
        m83D = true;
        m7A0.z = current.angle.z;
    }
    daNpc_Ls1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m83A && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            staff_id = isEventEntry();
        }
        if (staff_id >= 0 || m834) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        if (mType != 0) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
            mObjAcch.CrrPos(dComIfG_Bgsp());
        }
        play_animation();
    } else {
        m83A = false;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!m83B) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(false);
    if (!m841) {
        setCollision(40.0f, 100.0f);
    }
    return TRUE;
}
VERIFY(0x02281074, &daNpc_Ls1_c::_execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 02281318 */
BOOL daNpc_Ls1_c::_draw() {
    WWHD_FUNC(0x02281318, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModel* model_p = mpLsHandModel;
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m83A || m83C != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, morf_model_info_p, (f32)(u32)mBtkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x44, 0); /* mBtkAnm.remove() */
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    mDoExt_modelEntryDL(model_p);
    if (mpTelescopeModel.get()) {
        setLightTevColorType(dKy_getEnvlight(), mpTelescopeModel, &tevStr);
        mDoExt_modelEntryDL(mpTelescopeModel);
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x49 /* DSNAP_TYPE_NPC_LS1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (gabi::load<u8>(0x104679C0) /* l_HIO.mPrm.m18 */) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001D0B0);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001D0B4);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001D0B8);
        dComIfGp_get();
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001D0B8);
        local_static_init(0x101FDA48, 0x101FEBEC, 0x1001D0BC);
    }
    return TRUE;
}
VERIFY(0x02281318, &daNpc_Ls1_c::_draw);

/* PowerPC 32-bit shifts (shift amount: low 6 bits, >= 32 gives 0) */
static inline u32 ppc_slw(u32 x, u32 n) { n &= 0x3F; return n & 0x20 ? 0 : x << n; }
static inline u32 ppc_srw(u32 x, u32 n) { n &= 0x3F; return n & 0x20 ? 0 : x >> n; }
static inline u32 selfrel(u32 base) { u32 off = gabi::load<u32>(base); return off != 0 ? base + off : 0; }
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

/* 0227E3BC */
BOOL daNpc_Ls1_c::bodyCreateHeap() {
    WWHD_FUNC(0x0227E3BC, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0xD /* dRes_ID_LS_BDL_LS_e */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(3132, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001D1B8), 0xC3C, STR(0x1001D1C8));
    u32 md = gabi::ea(a_mdl_dat);
    /* HD: only the two eye materials (named by the static SafeStrings at 0x10467980) get a
     * daNpc_Ls1_matAnm_c (GameCube: every material) */
    for (int i = 0; i < 2; i++) {
        u32 name_obj = 0x10467980 + i * 8;
        u32 hdr = gabi::load<u32>(md);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(name_obj + 4) + 0x14), name_obj); /* assureTermination */
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x18), gabi::at<const char>(gabi::load<u32>(name_obj)));
        u32 mat;
        if (idx < 0) {
            mat = 0;
        } else {
            mat = gabi::load<u32>(md + 0x10);
            if ((u32)idx < gabi::load<u32>(md + 0xC))
                mat += idx * 0x39C;
        }
        daNpc_Ls1_matAnm_c* anm = (daNpc_Ls1_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = gabi::call<daNpc_Ls1_matAnm_c*>(0x0227DFF0, anm); /* new daNpc_Ls1_matAnm_c() */
        gabi::store<u32>(mat + 0x24, gabi::ea(anm)); /* mat->setMaterialAnm() */
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1 /* EMode_NULL */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        u32 vt = gabi::load<u32>(gabi::ea(morf));
        gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3); /* delete (virtual deleting destructor) */
        mpMorf = nullptr;
        return FALSE;
    }
    if (mType == 4 || gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) /* dComIfGs_getClearCount() */ != 0) {
        u32 tex_info_p = gabi::ea(dComIfG_getObjectIDRes(mArcName, 0x1D /* dRes_ID_LS_BTI_LSBODY02_e */));
        u32 a_texture = gabi::load<u32>(md + 0x30);
        if (a_texture == 0) /* JUT_ASSERT(3176, a_texture != NULL) */
            JUT_ASSERT_fail(STR(0x1001D1B8), 0xC68, STR(0x1001D1DC));
        u32 a_textureName = gabi::load<u32>(md + 0x34);
        if (a_textureName == 0) /* JUT_ASSERT(3178, a_textureName != NULL) */
            JUT_ASSERT_fail(STR(0x1001D1B8), 0xC6A, STR(0x1001D1F0));
        for (u16 i = 0; i < gabi::load<u16>(a_texture); i++) {
            u32 name = gabi::call<u32>(0x027ED1F0, a_textureName, i); /* JUTNameTab::getName */
            if (name == 0) /* HD: JUT_ASSERT(3182, a_nme != NULL) */
                JUT_ASSERT_fail(STR(0x1001D1B8), 0xC6E, STR(0x1001D208));
            /* strcmp(name, "lsbody01") */
            u32 a = name, b = 0x1001D218;
            u8 c1, c2;
            do {
                c1 = gabi::load<u8>(a++);
                c2 = gabi::load<u8>(b++);
            } while (c1 == c2 && c1 != 0);
            if (c1 != c2)
                continue;
            /* a_texture->setResTIMG(i, *tex_info_p) (HD inline) */
            u32 off = i * 0x24;
            u32 entry = gabi::load<u32>(a_texture + 4) + off;
            for (u32 k = 0; k < 0x24; k += 4) gabi::store<u32>(entry + k, gabi::load<u32>(tex_info_p + k));
            entry = gabi::load<u32>(a_texture + 4) + off;
            gabi::store<u32>(entry + 0x1C, gabi::load<u32>(entry + 0x1C) + tex_info_p - entry);
            entry = gabi::load<u32>(a_texture + 4) + off;
            gabi::store<u32>(entry + 0xC, gabi::load<u32>(entry + 0xC) + tex_info_p - entry);
            entry = gabi::load<u32>(a_texture + 4) + off;
            gabi::store<u32>(entry + 0x20, gabi::load<u32>(tex_info_p + 0x20));
            /* mark texture i as changed: a 128-bit mask at +8 gets (base << i), base at +0x18 */
            u32 hi = gabi::load<u32>(a_texture + 0x18);
            u32 lo = gabi::load<u32>(a_texture + 0x1C);
            if (i < 0x40) {
                u32 n = i;
                u32 nh = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                gabi::store<u32>(a_texture + 8, gabi::load<u32>(a_texture + 8) | nh);
                gabi::store<u32>(a_texture + 0xC, gabi::load<u32>(a_texture + 0xC) | ppc_slw(lo, n));
            } else {
                u32 n = i - 0x40;
                gabi::store<u32>(a_texture + 0x14, gabi::load<u32>(a_texture + 0x14) | ppc_slw(lo, n));
                u32 nh = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                gabi::store<u32>(a_texture + 0x10, gabi::load<u32>(a_texture + 0x10) | nh);
            }
        }
    }
    if (!init_texPttrnAnm(BTPNUM_FUAN_e, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D1B0) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D1B8), 0xC7D, STR(0x1001D224));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D238) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D1B8), 0xC7F, STR(0x1001D244));
    m_hnd_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D1A0) /* "handL" */);
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D1B8), 0xC81, STR(0x1001D25C));
    m_hnd_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D1A8) /* "handR" */);
    if (m_hnd_R_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D1B8), 0xC83, STR(0x1001D274));
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, 0x0227D954 /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, 0x0227DAC0 /* nodeCB_BackBone */);
    setJointCallBack(mpMorf->getModel(), m_hnd_L_jnt_num, 0x0227DC18 /* nodeCB_Hand_L */);
    setJointCallBack(mpMorf->getModel(), m_hnd_R_jnt_num, 0x0227DD70 /* nodeCB_Hand_R */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x0227E3BC, &daNpc_Ls1_c::bodyCreateHeap);

/* 0227EA10 */
BOOL daNpc_Ls1_c::handCreateHeap() {
    WWHD_FUNC(0x0227EA10, BOOL, this);
    mpLsHandModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0xC /* dRes_ID_LS_BDL_LSHAND_e */);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x1001D28C), 0xCAC, STR(0x1001D29C));
    mpLsHandModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    if (!mpLsHandModel.get()) {
        return FALSE;
    }
    m_lsHnd_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D2B0) /* "ls_handL" */);
    if (m_lsHnd_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D28C), 0xCB5, STR(0x1001D2BC));
    m_lsHnd_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001D2D4) /* "ls_handR" */);
    if (m_lsHnd_R_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001D28C), 0xCB7, STR(0x1001D2E0));
    setJointCallBack(mpLsHandModel, m_lsHnd_L_jnt_num, 0x0227DE8C /* Ls_hand_nodeCB_Hand_L */);
    setJointCallBack(mpLsHandModel, m_lsHnd_R_jnt_num, 0x0227DFA8 /* Ls_hand_nodeCB_Hand_R */);
    gabi::store<u32>(gabi::ea(mpLsHandModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x0227EA10, &daNpc_Ls1_c::handCreateHeap);

/* 0227EBF4 */
BOOL daNpc_Ls1_c::itemCreateHeap() {
    WWHD_FUNC(0x0227EBF4, BOOL, this);
    mpTelescopeModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001D2F8) /* "Link" */, 0x2F /* dRes_INDEX_LINK_BDL_TELESCOPE_e */, SAFESTRING_VTBL);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x1001D300), 0xCD7, STR(0x1001D310));
    mpTelescopeModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpTelescopeModel.get() != nullptr;
}
VERIFY(0x0227EBF4, &daNpc_Ls1_c::itemCreateHeap);

/* 0227EC98 */
BOOL daNpc_Ls1_c::CreateHeap() {
    WWHD_FUNC(0x0227EC98, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!handCreateHeap() || !itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x0227EC98, &daNpc_Ls1_c::CreateHeap);

/* 0227FBF0 */
cPhs_State daNpc_Ls1_c::_create() {
    WWHD_FUNC(0x0227FBF0, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Ls1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);           /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001D460;
            gabi::call(0x025E7C6C, mBtkAnm);        /* mDoExt_btkAnm::mDoExt_btkAnm (the matcher calls it init) */
            gabi::call(0x025E7820, mBtpAnm);        /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoadIsComplete = state == cPhs_COMPLEATE_e;
    if (!mbResLoadIsComplete) {
        return state;
    }
    /* a_siz_tbl (.data 0x101BFF7C) */
    if (!fopAcM_entrySolidHeap(this, 0x0227ED64 /* CheckCreateHeap */, gabi::load<u32>(0x101BFF7C + m854 * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x0227FBF0, &daNpc_Ls1_c::_create);

/* 02283290 daNpc_Ls1_HIO_c::daNpc_Ls1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ls1_HIO_c* daNpc_Ls1_HIO_c_ct(daNpc_Ls1_HIO_c* i_this) {
    WWHD_FUNC(0x02283290, daNpc_Ls1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ls1_HIO_c*)operator_new(0x44);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D104;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101C01E4), 0x38); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x02283290, daNpc_Ls1_HIO_c_ct);

/* 022832FC: static initialisation of the translation unit */
static void __sinit_d_a_npc_ls1_cpp() {
    WWHD_FUNC(0x022832FC, void, (u32)0);
    /* header statics (as sinit_header_statics, but the zeroed object is at 0x10467970) */
    const u32 P = 0x10467964, D = 0x101C021C;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10467970 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
    /* HD: two static sead::SafeStrings naming the eye materials (setMat, bodyCreateHeap) */
    gabi::store<u32>(0x10467984, SAFESTRING_VTBL);
    gabi::store<u32>(0x10467980, 0x1001D438);
    gabi::store<u32>(0x1046798C, SAFESTRING_VTBL);
    gabi::store<u32>(0x10467988, 0x1001D440);
    daNpc_Ls1_HIO_c_ct(&l_HIO()); /* static daNpc_Ls1_HIO_c l_HIO */
}
VERIFY(0x022832FC, __sinit_d_a_npc_ls1_cpp);

/* 022833CC: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001D0CC slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022833CC, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022833CC, SafeString_dt);

/* 022833E0: daNpc_Ls1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ls1_c_dt(daNpc_Ls1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022833E0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001D0E4);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001D0F4);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022833E0, daNpc_Ls1_c_dt);

/* 0228347C: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0228347C, void, (SafeString*)nullptr);
}
VERIFY(0x0228347C, SafeString_assureTerminationImpl);
