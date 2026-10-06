/**
 * d_a_npc_ac1.cpp (WWHD)
 * NPC - Ac1 (Rito with wing/arm models)
 *
 * Written from the WWHD code (every GameCube function
 * of zeldaret/tww src/d/actor/d_a_npc_ac1.cpp is a "Nonmatching" stub; the names follow the
 * GameCube symbols), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_ac1.h"

#define SAFESTRING_VTBL 0x10015B3C /* this TU's sead::SafeString vtable */
#define AC1_VTBL 0x10015E88        /* daNpc_Ac1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: the event flags (dSv_event_c) are at *(0x101F84DC) + 0x644; re-read at every use */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* play object fields (dComIfGp_get() at each use) */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* resources by id / by name (HD: sead::SafeString keys, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 02606900 dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* dComIfG_getObjectRes_name(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::ea(arc);
    a->__vtbl = SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    n->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* J3DModel (HD): model data at +0xAC */
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* dAttention_c (play + 0x5804): 024EC8D0 is LockonTarget, 024EE464 ActionTarget (matcher swapped) */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
/* d_npc */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline void dNpc_PathRun_setInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
/* 0259D344 dNpc_setAnmFNDirect(morf, loopMode, morf, speed, name, soundId, arc) */
static inline BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, u32 name, s32 snd, const char* arc) {
    return gabi::call<BOOL>(0x0259D344, morf, loopMode, morfF, speed, name, snd, arc);
}
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status (the
 * matcher calls it fopMsgM_SearchByID) */
static inline u32 fopMsgM_getStatus(u32 mng) { return gabi::call<u32>(0x025F795C, mng); }
/* J3DModelData (HD): 027F68FC joint name table header, 027F3F94 (matcher: __nw) joint tree header */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
/* dDemo */
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc) { return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc); }
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* McaMorf: HD virtual deleting destructor (vtable at +0, slot +0xC) */
static inline void McaMorf_delete(mDoExt_McaMorf* m) {
    u32 vt = gabi::load<u32>(gabi::ea(m));
    gabi::call_ptr(gabi::load<u32>(vt + 0xC), m, 3);
}

/* ---- file statics ---- */
/* daNpc_Ac1_HIO_c, HD: vtable at 0 */
struct daNpc_Ac1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mCalcAngleTarget;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebug;
        /* 0x19 */ u8 _19[0x30 - 0x19];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Ac1_HIO_c, 0x3C);
static daNpc_Ac1_HIO_c& l_HIO() { return *gabi::at<daNpc_Ac1_HIO_c>(0x1046596C); }

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 { PMF_wait_action1 = 0x10015B28 };
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
/* (this->*pmf)(arg) */
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

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline Mtx34* j3dSys_currentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) */
static inline void setJointCallBack(mDoExt_McaMorf* morf, u16 i, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(morf->getModel());
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (i < n)
        joint += i * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 021E1EFC */
void daNpc_Ac1_c::nodeWngControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021E1EFC, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_wngL_jnt_num) {
        PSMTXCopy(&mArmLMtx, j3dSys_currentMtx());
        mtx_copy(getAnmMtx(i_model, jointIdx), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m_wngR_jnt_num) {
        PSMTXCopy(&mArmRMtx, j3dSys_currentMtx());
        mtx_copy(getAnmMtx(i_model, jointIdx), &mArmRMtx);
    }
}
VERIFY(0x021E1EFC, &daNpc_Ac1_c::nodeWngControl);

/* 021E2088 */
static BOOL nodeCallBack_Wng(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021E2088, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Ac1_c* user = gabi::at<daNpc_Ac1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeWngControl(i_node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021E2088, nodeCallBack_Wng);

/* 021E20D0 */
void daNpc_Ac1_c::nodeArmControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021E20D0, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_armL_loc_jnt_num) {
        PSMTXCopy(&mArmLMtx, j3dSys_currentMtx());
        mtx_copy(getAnmMtx(i_model, jointIdx), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m_armR_loc_jnt_num) {
        PSMTXCopy(&mArmRMtx, j3dSys_currentMtx());
        mtx_copy(getAnmMtx(i_model, jointIdx), &mArmRMtx);
    }
}
VERIFY(0x021E20D0, &daNpc_Ac1_c::nodeArmControl);

/* 021E225C */
static BOOL nodeCallBack_Arm(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021E225C, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Ac1_c* user = gabi::at<daNpc_Ac1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeArmControl(i_node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021E225C, nodeCallBack_Arm);

/* 021E22A4 */
void daNpc_Ac1_c::nodeAc1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021E22A4, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(20.0f, 18.0f, 0.0f): guard 0x104659B4, object 0x104659A8 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x104659A8);
    if (gabi::load<u32>(0x104659B4) == 0) {
        gabi::store<u32>(0x104659B4, 1);
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 18.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_hed_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &mTransformedEyePos);
    }
    if (jointIdx == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    if (jointIdx == (u32)(s32)m_armL_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mArmLMtx);
    }
    if (jointIdx == (u32)(s32)m_armR_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mArmRMtx);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_currentMtx());
    mtx_copy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
}
VERIFY(0x021E22A4, &daNpc_Ac1_c::nodeAc1Control);

/* 021E247C */
static BOOL nodeCallBack_Ac1(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021E247C, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Ac1_c* user = gabi::at<daNpc_Ac1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeAc1Control(i_node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021E247C, nodeCallBack_Ac1);

/* 021E24C4 */
J3DModelData* daNpc_Ac1_c::create_Anm() {
    WWHD_FUNC(0x021E24C4, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10015BB0) /* "Ac" */, 5);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10015BCC), 0x709, STR(0x10015BDC));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x10015BB0), 1);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            McaMorf_delete(morf);
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015BB4) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015BCC), 0x71F, STR(0x10015BF0));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015C04) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015BCC), 0x722, STR(0x10015C10));
    m_armL_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015BBC) /* "armL" */);
    if (m_armL_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015BCC), 0x725, STR(0x10015C28));
    m_armR_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015BC4) /* "armR" */);
    if (m_armR_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015BCC), 0x728, STR(0x10015C40));
    return a_mdl_dat;
}
VERIFY(0x021E24C4, &daNpc_Ac1_c::create_Anm);

/* 021E274C */
int daNpc_Ac1_c::btpNum_toResID(int i_btpNum) {
    WWHD_FUNC(0x021E274C, int, this, i_btpNum);
    return gabi::load<s32>(0x10015C58 + i_btpNum * 4); /* a_btp_resID_tbl */
}
VERIFY(0x021E274C, &daNpc_Ac1_c::btpNum_toResID);

/* 021E2760 */
/* i_param_1 (GameCube bool) is passed on unnormalised: typed u32 */
bool daNpc_Ac1_c::setBtp(u32 i_param_1, int i_btp_num) {
    WWHD_FUNC(0x021E2760, bool, this, i_param_1, i_btp_num);
    J3DModelData* model_data = J3DModel_getModelData_l(mpMorf->getModel());
    int res_id = btpNum_toResID(i_btp_num);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x10015C64) /* "Ac" */, res_id);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(572, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x10015C68), 0x23C, STR(0x10015C78));
    int iVar1 = mDoExt_btpAnm_init(mBtpAnm, model_data, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_param_1, 0);
    bool o_retval = iVar1 == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return o_retval;
}
VERIFY(0x021E2760, &daNpc_Ac1_c::setBtp);

/* 021E284C */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Ac1_c::iniTexPttrnAnm(u32 i_param_1) {
    WWHD_FUNC(0x021E284C, u32, this, i_param_1);
    return gabi::call<u32>(0x021E2760, this, i_param_1, (s32)mBtpNum); /* setBtp(i_param_1, mBtpNum) */
}
VERIFY(0x021E284C, &daNpc_Ac1_c::iniTexPttrnAnm);

/* 021E2858 */
J3DModelData* daNpc_Ac1_c::create_wng_Anm() {
    WWHD_FUNC(0x021E2858, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10015C90) /* "Ac" */, 3);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x10015C94), 0x745, STR(0x10015CBC));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes_name(STR(0x10015C90), STR(0x10015CD0) /* "acwing_wait01.fskb" */);
    mpWngMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                       0x11000002);
    mDoExt_McaMorf* morf = mpWngMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            McaMorf_delete(morf);
        mpWngMorf = nullptr;
        return nullptr;
    }
    m_wngL_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015CA4) /* "wingL_loc" */);
    if (m_wngL_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015C94), 0x760, STR(0x10015CE4));
    m_wngR_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015CB0) /* "wingR_loc" */);
    if (m_wngR_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015C94), 0x763, STR(0x10015CFC));
    return a_mdl_dat;
}
VERIFY(0x021E2858, &daNpc_Ac1_c::create_wng_Anm);

/* 021E2A50 */
J3DModelData* daNpc_Ac1_c::create_arm_Anm() {
    WWHD_FUNC(0x021E2A50, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10015D1C) /* "Ac" */, 2);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x10015D20), 0x77A, STR(0x10015D30));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x10015D1C), 0);
    mpArmMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                       0x11000002);
    mDoExt_McaMorf* morf = mpArmMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr)
            McaMorf_delete(morf);
        mpArmMorf = nullptr;
        return nullptr;
    }
    m_handR_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015D14) /* "handR" */);
    if (m_handR_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015D20), 0x791, STR(0x10015D44));
    m_armL_loc_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015D5C) /* "armL_loc" */);
    if (m_armL_loc_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015D20), 0x794, STR(0x10015D68));
    m_armR_loc_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10015D80) /* "armR_loc" */);
    if (m_armR_loc_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10015D20), 0x797, STR(0x10015D8C));
    return a_mdl_dat;
}
VERIFY(0x021E2A50, &daNpc_Ac1_c::create_arm_Anm);

/* 021E2C88 */
bool daNpc_Ac1_c::create_itm_Mdl() {
    WWHD_FUNC(0x021E2C88, bool, this);
    mpItemModel = nullptr;
    J3DModelData* a_mdl_dat;
    if (dComIfGs_isEventBit(0x1580)) {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10015DA4) /* "Ac" */, 0xF);
    } else {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10015DA4), 4);
    }
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x10015DA8), 0x7BA, STR(0x10015DB8));
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return true;
}
VERIFY(0x021E2C88, &daNpc_Ac1_c::create_itm_Mdl);

/* 021E2D68 */
BOOL daNpc_Ac1_c::CreateHeap() {
    WWHD_FUNC(0x021E2D68, BOOL, this);
    J3DModelData* body_mdl = create_Anm();
    if (body_mdl == nullptr) {
        return FALSE;
    }
    mBtpNum = 0;
    J3DModelData* wng_mdl;
    if (!iniTexPttrnAnm(false) || (wng_mdl = create_wng_Anm()) == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    J3DModelData* arm_mdl = create_arm_Anm();
    if (arm_mdl != nullptr && create_itm_Mdl()) {
        for (u16 i = 0; i < J3DModelData_getJointNum(wng_mdl); i++) {
            if (i == (u32)(s32)m_wngL_jnt_num || i == (u32)(s32)m_wngR_jnt_num)
                setJointCallBack(mpWngMorf, i, 0x021E2088 /* nodeCallBack_Wng */);
        }
        gabi::store<u32>(gabi::ea(mpWngMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
        for (u16 i = 0; i < J3DModelData_getJointNum(arm_mdl); i++) {
            if (i == (u32)(s32)m_armL_loc_jnt_num || i == (u32)(s32)m_armR_loc_jnt_num)
                setJointCallBack(mpArmMorf, i, 0x021E225C /* nodeCallBack_Arm */);
        }
        gabi::store<u32>(gabi::ea(mpArmMorf->getModel()) + 0xB8, gabi::ea(this));
        for (u16 i = 0; i < J3DModelData_getJointNum(body_mdl); i++) {
            if (i == (u32)(s32)m_hed_jnt_num || i == (u32)(s32)m_bbone_jnt_num || i == (u32)(s32)m_armL_jnt_num ||
                i == (u32)(s32)m_armR_jnt_num)
                setJointCallBack(mpMorf, i, 0x021E247C /* nodeCallBack_Ac1 */);
        }
        gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this));
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    mpWngMorf = nullptr;
    return FALSE;
}
VERIFY(0x021E2D68, &daNpc_Ac1_c::CreateHeap);

/* 021E305C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E305C, BOOL, i_this);
    return static_cast<daNpc_Ac1_c*>(i_this)->CreateHeap();
}
VERIFY(0x021E305C, CheckCreateHeap);

/* 021E3060 */
bool daNpc_Ac1_c::charDecide(int) {
    WWHD_FUNC(0x021E3060, bool, this, 0);
    mSpecificType = 0;
    mType = 0;
    return true;
}
VERIFY(0x021E3060, &daNpc_Ac1_c::charDecide);

/* 021E3074 */
BOOL daNpc_Ac1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x021E3074, BOOL, this, i_newProcFunc, i_argsP);
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
VERIFY(0x021E3074, &daNpc_Ac1_c::set_action);

/* 021E31A0 */
bool daNpc_Ac1_c::init_AC1_0() {
    WWHD_FUNC(0x021E31A0, bool, this);
    bool ret = dComIfGs_isEventBit(0x2E04) != 0;
    if (ret) {
        dComIfGs_isEventBit(0x1580); /* the result is not used */
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
    }
    return ret;
}
VERIFY(0x021E31A0, &daNpc_Ac1_c::init_AC1_0);

/* 021E3230 */
void daNpc_Ac1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x021E3230, void, this);
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
VERIFY(0x021E3230, &daNpc_Ac1_c::plyTexPttrnAnm);

/* 021E3304 */
void daNpc_Ac1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x021E3304, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_HIO().mPrmTbl.mAttPosOffsetY;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mTransformedEyePos.z;
    eyePos.y = mTransformedEyePos.y;
    eyePos.x = mTransformedEyePos.x;
}
VERIFY(0x021E3304, &daNpc_Ac1_c::setAttention);

/* 021E3358 */
void daNpc_Ac1_c::setMtx(u32 param_1) {
    WWHD_FUNC(0x021E3358, void, this, param_1);
    if (!mbInDemo) {
        plyTexPttrnAnm();
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevMorfFrame) {
            mbMorfAnimStopped = 1;
        }
        mPrevMorfFrame = mpMorf->getFrame();
        if (mbArmAnm) {
            mpArmMorf->play(&eyePos, 0, 0);
        } else {
            mpWngMorf->play(&eyePos, 0, 0);
        }
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    /* the ground polygon: cBgS_PolyInfo at m_gnd + 0x14 */
    void* gndPoly = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly)); /* tevStr.mEnvrIdxOverride */

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (!mbArmAnm) {
        mpWngMorf->calc();
    } else {
        mpArmMorf->calc();
    }
    if (mpItemModel.get() != nullptr && mbArmAnm) {
        J3DModel_setBaseTRMtx(mpItemModel, getAnmMtx(mpArmMorf->getModel(), m_handR_jnt_num));
        J3DModel_calc(mpItemModel);
    }
    setAttention(param_1);
}
VERIFY(0x021E3358, &daNpc_Ac1_c::setMtx);

/* 021E35E4 */
bool daNpc_Ac1_c::createInit() {
    WWHD_FUNC(0x021E35E4, bool, this);
    m984.copy(current.pos);
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -4.5f;
    u8 path = (u8)(fopAcM_GetParam(this) >> 16);
    if (path != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, path, current.roomNo, 1);
        if (mPathRun.mPath == 0) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    }
    mEventCut.setActorInfo2(STR(0x10015DDC) /* "Ac1" */, (fopNpc_npc_c*)this);
    mAnmNum = 4;
    if (mSpecificType != 0) {
        return false;
    }
    if (!init_AC1_0()) {
        return false;
    }
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    shape_angle.x = current.angle.x;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    if (!mbArmAnm) {
        mpWngMorf->setMorf(0.0f);
    } else {
        mpArmMorf->setMorf(0.0f);
    }
    setMtx(true);
    return true;
}
VERIFY(0x021E35E4, &daNpc_Ac1_c::createInit);

/* 021E3794 */
cPhs_State daNpc_Ac1_c::_create() {
    WWHD_FUNC(0x021E3794, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Ac1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = AC1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x10015DEC) /* "Ac" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    /* a_size_tbl[] = {0x272E0} (.data 0x101BB160) */
    if (!fopAcM_entrySolidHeap(this, 0x021E305C /* CheckCreateHeap */, gabi::load<u32>(0x101BB160))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x021E3794, &daNpc_Ac1_c::_create);

/* 021E38D0 */
static cPhs_State daNpc_Ac1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E38D0, cPhs_State, i_this);
    return ((daNpc_Ac1_c*)i_this)->_create();
}
VERIFY(0x021E38D0, daNpc_Ac1_Create);

/* 021E38D4 */
BOOL daNpc_Ac1_c::_delete() {
    WWHD_FUNC(0x021E38D4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x10015DEF) /* "Ac" */);
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr)
            mpMorf->stopZelAnime();
        if (mpWngMorf.get() != nullptr)
            mpWngMorf->stopZelAnime();
        if (mpArmMorf.get() != nullptr)
            mpArmMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x021E38D4, &daNpc_Ac1_c::_delete);

/* 021E394C */
static BOOL daNpc_Ac1_Delete(daNpc_Ac1_c* i_this) {
    WWHD_FUNC(0x021E394C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x021E394C, daNpc_Ac1_Delete);

/* 021E3950 */
void daNpc_Ac1_c::checkOrder() {
    WWHD_FUNC(0x021E3950, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        return;
    }
    if (command == 1 /* checkCommandTalk() */) {
        if (mTalkState == 1 || mTalkState == 2) {
            mTalkState = 0;
            mbTalkReq = 1;
        }
    }
}
VERIFY(0x021E3950, &daNpc_Ac1_c::checkOrder);

/* 021E3990 */
/* returns the mbInDemo byte (typed u8) */
u8 daNpc_Ac1_c::demo() {
    WWHD_FUNC(0x021E3990, u8, this);
    if (demoActorID == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return 0;
    }
    u8 id = demoActorID;
    mbInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check, the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x10015B94), 0x23A, STR(0x10015B84));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = dDemo_object_getActor(obj, id);
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    /* HD: demo_actor is checked for NULL */
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = dDemo_actor_getP_BtpData(demo_actor, STR(0x10015DF2) /* "Ac" */);
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            if (mDoExt_btpAnm_init(mBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 3;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x10015DF2) /* "Ac" */, 0, nullptr, 0, 0);
    return mbInDemo;
}
VERIFY(0x021E3990, &daNpc_Ac1_c::demo);

/* 021E3B44 */
s32 daNpc_Ac1_c::isEventEntry() {
    WWHD_FUNC(0x021E3B44, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x021E3B44, &daNpc_Ac1_c::isEventEntry);

/* 021E3B84 */
void daNpc_Ac1_c::event_actionInit(int i_staffIdx) {
    WWHD_FUNC(0x021E3B84, void, this, i_staffIdx);
    be<s32>* actNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x10015DF8) /* "ActNo" */);
    if (actNo != nullptr) {
        mActNo = (s8)(s32)*actNo;
    }
}
VERIFY(0x021E3B84, &daNpc_Ac1_c::event_actionInit);

/* 021E3BE4 */
bool daNpc_Ac1_c::event_action() {
    WWHD_FUNC(0x021E3BE4, bool, this);
    return true;
}
VERIFY(0x021E3BE4, &daNpc_Ac1_c::event_action);

/* 021E3BEC */
void daNpc_Ac1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x021E3BEC, void, this, i_staffIdx);
    /* static char* a_cut_tbl[] = {"ACTION"} (.data 0x101BB164) */
    if (i_staffIdx == -1) {
        return;
    }
    s8 actIdx = gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), i_staffIdx, 0x101BB164, 1, 1, 0); /* getMyActIdx */
    mActionIndex = actIdx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (actIdx != -1) {
        if (gabi::call<BOOL>(0x025447C8, evmng, i_staffIdx) /* getIsAddvance */) {
            if (mActionIndex == 0) {
                event_actionInit(i_staffIdx);
            }
        }
        if (mActionIndex == 0) {
            if (!event_action()) {
                return;
            }
        }
        evmng = dComIfGp_getPEvtManager();
    }
    gabi::call(0x02543280, evmng, i_staffIdx); /* cutEnd */
}
VERIFY(0x021E3BEC, &daNpc_Ac1_c::privateCut);

/* 021E3CC0 */
void daNpc_Ac1_c::lookBack() {
    WWHD_FUNC(0x021E3CC0, void, this);
    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    mJointBackboneY = m_jnt.mAngles[1][1];
    s16 desiredYrot = current.angle.y;
    mActorAngleY = desiredYrot;
    mJointHeadY = m_jnt.mAngles[0][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    dstPos_p = nullptr;
    u8 headOnlyFollow = mHeadOnlyFollow;

    switch ((u32)(s32)mLookBackState) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case 2:
        dstPos->copy(m984);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    case 3:
        desiredYrot = mTargetYRot;
        break;
    }
    cLib_addCalcAngleS2(&mLookAtMaxVel, l_HIO().mPrmTbl.mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mLookAtMaxVel = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, mLookAtMaxVel, headOnlyFollow);
}
VERIFY(0x021E3CC0, &daNpc_Ac1_c::lookBack);

/* 021E3F00 */
void daNpc_Ac1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x021E3F00, void, this, i_staffIdx);
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
    lookBack();
}
VERIFY(0x021E3F00, &daNpc_Ac1_c::event_proc);

/* 021E3F58 */
void daNpc_Ac1_c::eventOrder() {
    WWHD_FUNC(0x021E3F58, void, this);
    if (mTalkState == 1 || mTalkState == 2) {
        s8 state = mTalkState;
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (state == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x021E3F58, &daNpc_Ac1_c::eventOrder);

/* 021E3F90 */
BOOL daNpc_Ac1_c::_execute() {
    WWHD_FUNC(0x021E3F90, BOOL, this);
    if (!mbRanExecute) {
        mInitialPos.copy(current.pos);
        mInitialAngle.z = current.angle.z;
        mInitialAngle.x = current.angle.x;
        mInitialAngle.y = current.angle.y;
        mbRanExecute = 1;
    }
    daNpc_Ac1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m9CD && demoActorID == 0) {
        return TRUE;
    }
    m9D0 = 0;
    m9CD = 0;
    checkOrder();
    if (!demo()) {
        bool ran = false;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            s32 staffIdx = isEventEntry();
            if (staffIdx >= 0) {
                event_proc(staffIdx);
                ran = true;
            }
        }
        if (!ran) {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!m9D0) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (!m9CF) {
            shape_angle.z = current.angle.z;
            shape_angle.y = current.angle.y;
            shape_angle.x = current.angle.x;
        }
    }
    eventOrder();
    setMtx(false);
    if (!mbInDemo) {
        setCollision(50.0f, 140.0f);
    }
    return TRUE;
}
VERIFY(0x021E3F90, &daNpc_Ac1_c::_execute);

/* 021E4178 */
static BOOL daNpc_Ac1_Execute(daNpc_Ac1_c* i_this) {
    WWHD_FUNC(0x021E4178, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x021E4178, daNpc_Ac1_Execute);

/* 021E417C */
BOOL daNpc_Ac1_c::_draw() {
    WWHD_FUNC(0x021E417C, BOOL, this);
    J3DModel* morf_model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(morf_model);
    if (m9CD || m9D1) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove(model_data) */
    if (mbArmAnm) {
        J3DModel* m = mpArmMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), m, &tevStr);
        mpArmMorf->entryDL();
    } else {
        J3DModel* m = mpWngMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), m, &tevStr);
        mpWngMorf->entryDL();
    }
    if (mpItemModel.get() != nullptr && mbArmAnm) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    /* HD: no shadowDraw() */
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mPrmTbl.mDebug != 0) {
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x10015B30), 4); /* 028FEAC0 memcpy */
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF8), gabi::at<u8>(0x10015B34), 4); /* 028FEAC0 memcpy */
        }
    }
    dSnap_RegistFig(0x8B /* DSNAP_TYPE_NPC_AC1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021E417C, &daNpc_Ac1_c::_draw);

/* 021E433C */
static BOOL daNpc_Ac1_Draw(daNpc_Ac1_c* i_this) {
    WWHD_FUNC(0x021E433C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x021E433C, daNpc_Ac1_Draw);

/* 021E4340 */
static BOOL daNpc_Ac1_IsDelete(daNpc_Ac1_c*) {
    WWHD_FUNC(0x021E4340, BOOL, (daNpc_Ac1_c*)nullptr);
    return TRUE;
}
VERIFY(0x021E4340, daNpc_Ac1_IsDelete);

/* 021E4348 */
int daNpc_Ac1_c::anmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021E4348, int, this, i_anmNum);
    return gabi::load<s32>(0x10015E08 + i_anmNum * 4); /* a_bck_resID_tbl */
}
VERIFY(0x021E4348, &daNpc_Ac1_c::anmNum_toResID);

/* 021E435C (HD: the arm animation resource table; no GameCube symbol) */
int daNpc_Ac1_c::armAnmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021E435C, int, this, i_anmNum);
    return gabi::load<s32>(0x10015E18 + i_anmNum * 4);
}
VERIFY(0x021E435C, &daNpc_Ac1_c::armAnmNum_toResID);

/* 021E4370 (HD: returns the wing animation's file name, "acwing_*.fskb") */
u32 daNpc_Ac1_c::wingAnmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021E4370, u32, this, i_anmNum);
    return gabi::load<u32>(0x101BB168 + i_anmNum * 4);
}
VERIFY(0x021E4370, &daNpc_Ac1_c::wingAnmNum_toResID);

/* 021E4384 */
u32 daNpc_Ac1_c::setAnm_tex(s8 i_btp_num) {
    WWHD_FUNC(0x021E4384, u32, this, i_btp_num);
    if (mBtpNum != i_btp_num) {
        mBtpNum = i_btp_num;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x021E4384, &daNpc_Ac1_c::setAnm_tex);

/* 021E43A4 */
BOOL daNpc_Ac1_c::setAnm_anm(daNpc_Ac1_c::anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x021E43A4, BOOL, this, i_anmPrmP);
    if (mAnmNum == i_anmPrmP->mAnmNum) {
        return TRUE;
    }
    mAnmNum = i_anmPrmP->mAnmNum;
    int resID = anmNum_toResID(mAnmNum);
    dNpc_setAnmIDRes(mpMorf, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, resID, -1, STR(0x10015E78) /* "Ac" */);
    mbArmAnm = i_anmPrmP->mbArm == 1;
    if (mbArmAnm) {
        int armID = armAnmNum_toResID(mAnmNum);
        dNpc_setAnmIDRes(mpArmMorf, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, armID, -1, STR(0x10015E78));
    } else {
        u32 name = wingAnmNum_toResID(mAnmNum);
        dNpc_setAnmFNDirect(mpWngMorf, i_anmPrmP->mLoopMode, i_anmPrmP->mMorf, i_anmPrmP->mSpeed, name, 0, STR(0x10015E78));
    }
    m9C9 = 0;
    mPrevMorfFrame = 0.0f;
    mbMorfAnimStopped = 0;
    return TRUE;
}
VERIFY(0x021E43A4, &daNpc_Ac1_c::setAnm_anm);

/* 021E44DC */
bool daNpc_Ac1_c::setAnm() {
    WWHD_FUNC(0x021E44DC, bool, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB178); /* [3] */
    if (a_anm_prm_tbl[mStatus].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStatus].mBtpNum);
    }
    if (a_anm_prm_tbl[mStatus].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStatus]);
    }
    return true;
}
VERIFY(0x021E44DC, &daNpc_Ac1_c::setAnm);

/* 021E4560 */
void daNpc_Ac1_c::setAnm_ATR(int i_param_1) {
    WWHD_FUNC(0x021E4560, void, this, i_param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB1B4); /* [6] */
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[mAnmAtr].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x021E4560, &daNpc_Ac1_c::setAnm_ATR);

/* 021E45D0 */
void daNpc_Ac1_c::chg_anmAtr(u8 i_param_1) {
    WWHD_FUNC(0x021E45D0, void, this, i_param_1);
    if ((i_param_1 >= 6) || (i_param_1 == mAnmAtr)) {
        return;
    }
    mAnmAtr = i_param_1;
    setAnm_ATR(1);
}
VERIFY(0x021E45D0, &daNpc_Ac1_c::chg_anmAtr);

/* 021E45F4 */
void daNpc_Ac1_c::anmAtr(u16 i_param_1) {
    WWHD_FUNC(0x021E45F4, void, this, i_param_1);
    switch (i_param_1) {
    case 6: {
        if (m9EB == 0) {
            mAnmAtr = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m9EB = m9EB + 1;
        }
        u8 mesgAnimeTagInfo = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);                /* dComIfGp_clearMesgAnimeTagInfo() */
        if (mesgAnimeTagInfo != 0xFF && mMesgAnimeTagInfo != mesgAnimeTagInfo) {
            mMesgAnimeTagInfo = mesgAnimeTagInfo;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        m9EB = 0;
        break;
    }
    /* control_anmTag(), control_anmAtr(): empty */
}
VERIFY(0x021E45F4, &daNpc_Ac1_c::anmAtr);

/* 021E46CC */
bool daNpc_Ac1_c::chk_talk() {
    WWHD_FUNC(0x021E46CC, bool, this);
    bool ret = true;
    mItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}
VERIFY(0x021E46CC, &daNpc_Ac1_c::chk_talk);

/* 021E474C */
u8 daNpc_Ac1_c::chk_partsNotMove() {
    WWHD_FUNC(0x021E474C, u8, this);
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}
VERIFY(0x021E474C, &daNpc_Ac1_c::chk_partsNotMove);

/* 021E478C */
u16 daNpc_Ac1_c::next_msgStatus(be<u32>* i_msg_no) {
    WWHD_FUNC(0x021E478C, u16, this, i_msg_no);
    u16 ret = 0xF;
    switch (*i_msg_no) {
    case 0x184C:
        *i_msg_no = 0x184D;
        break;
    case 0x184D:
        *i_msg_no = 0x184E;
        break;
    case 0x1850:
        *i_msg_no = 0x1851;
        break;
    case 0x1851:
        *i_msg_no = 0x1852;
        break;
    default:
        ret = 0x10;
        break;
    }
    return ret;
}
VERIFY(0x021E478C, &daNpc_Ac1_c::next_msgStatus);

/* 021E47EC */
u32 daNpc_Ac1_c::getBitMask() {
    WWHD_FUNC(0x021E47EC, u32, this);
    u32 ret = 0;
    if (mSpecificType == 0) {
        ret = 0x10;
    }
    return ret;
}
VERIFY(0x021E47EC, &daNpc_Ac1_c::getBitMask);

/* 021E4804 */
u32 daNpc_Ac1_c::getMsg_AC1_0() {
    WWHD_FUNC(0x021E4804, u32, this);
    s32 reg = (s8)dSv_event_getEventReg(dComIfGs_event(), 0xB8FF);
    u32 mask = getBitMask();
    if (dComIfGs_isEventBit(0x1580)) {
        if (mask & reg) {
            return 0x1853;
        }
        dSv_event_setEventReg(dComIfGs_event(), 0xB8FF, (u8)(reg | mask));
        return 0x1850;
    }
    if (mask & reg) {
        return 0x184F;
    }
    dSv_event_setEventReg(dComIfGs_event(), 0xB8FF, (u8)(reg | mask));
    return 0x184C;
}
VERIFY(0x021E4804, &daNpc_Ac1_c::getMsg_AC1_0);

/* 021E4938 */
u32 daNpc_Ac1_c::getMsg() {
    WWHD_FUNC(0x021E4938, u32, this);
    u32 ret = 0;
    if (mSpecificType == 0) {
        ret = getMsg_AC1_0();
    }
    return ret;
}
VERIFY(0x021E4938, &daNpc_Ac1_c::getMsg);

/* 021E4970 */
u8 daNpc_Ac1_c::chkAttention() {
    WWHD_FUNC(0x021E4970, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x021E4970, &daNpc_Ac1_c::chkAttention);

/* 021E49F8 */
void daNpc_Ac1_c::endEvent() {
    WWHD_FUNC(0x021E49F8, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}
VERIFY(0x021E49F8, &daNpc_Ac1_c::endEvent);

/* 021E4A38 */
void daNpc_Ac1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x021E4A38, void, this, i_status);
    s8 prev = mStatus;
    mStatus = i_status;
    mEvTimer2 = 0;
    if (i_status == 2) {
        mPrevStatus = prev;
        mAnmAtr = 0xFF;
        mLookBackState = 1;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        return;
    }
    setAnm();
}
VERIFY(0x021E4A38, &daNpc_Ac1_c::setStt);

/* 021E4A74 */
BOOL daNpc_Ac1_c::wait_1() {
    WWHD_FUNC(0x021E4A74, BOOL, this);
    if (mTalkState == 1 || mTalkState >= 3) {
        return TRUE;
    }
    if (mbTalkReq) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    mTalkState = 2;
    if (mbAttention) {
        mEvTimer2 = 60;
    }
    if (cLib_calcTimer(&mEvTimer2)) {
        mLookBackState = 1;
    } else {
        mLookBackState = 3;
        mTargetYRot = mInitialAngle.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}
VERIFY(0x021E4A74, &daNpc_Ac1_c::wait_1);

/* 021E4B38 */
BOOL daNpc_Ac1_c::talk_1() {
    WWHD_FUNC(0x021E4B38, BOOL, this);
    /* HD: the message manager pointer is read once */
    u32 msgMng = gabi::load<u32>(0x101F4B5C);
    u8 ret = chk_partsNotMove();
    u16 status = 0;
    if (mbHasMsg) {
        status = (u16)fopMsgM_getStatus(msgMng);
    }
    mMsgStatus = status;
    talk(1);
    if (mbHasMsg) {
        if (fopMsgM_getStatus(msgMng) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            dComIfGs_onEventBit(0x3F02);
            mItemNo = 0xFF;
            mbTalkReq = 0;
            setStt(mPrevStatus);
            mEvTimer2 = 60;
            endEvent();
        }
    }
    return ret;
}
VERIFY(0x021E4B38, &daNpc_Ac1_c::talk_1);

/* 021E4C10 */
BOOL daNpc_Ac1_c::wait_action1(void*) {
    WWHD_FUNC(0x021E4C10, BOOL, this, (void*)nullptr);
    if (mActState == 0) {
        setStt(1);
        mActState = mActState + 1;
    } else if ((u32)(s32)mActState <= 3) {
        mbAttention = chkAttention();
        switch ((u32)(s32)mStatus) {
        case 1:
            mbSetEyePos = wait_1();
            lookBack();
            break;
        case 2:
            mbSetEyePos = talk_1();
            lookBack();
            break;
        default:
            lookBack();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x021E4C10, &daNpc_Ac1_c::wait_action1);

/* 021E4CCC daNpc_Ac1_HIO_c::daNpc_Ac1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ac1_HIO_c* daNpc_Ac1_HIO_c_ct(daNpc_Ac1_HIO_c* i_this) {
    WWHD_FUNC(0x021E4CCC, daNpc_Ac1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ac1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10015B74;
    /* memcpy(&mPrmTbl, &a_prm_tbl (.data 0x101BB22C), sizeof(hio_prm_c)) */
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101BB22C), 0x30); /* 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x021E4CCC, daNpc_Ac1_HIO_c_ct);

/* 021E4D38: static initialisation of the translation unit */
static void __sinit_d_a_npc_ac1_cpp() {
    WWHD_FUNC(0x021E4D38, void, (u32)0);
    sinit_header_statics(0x10465950, 0x101BB25C);
    daNpc_Ac1_HIO_c_ct(&l_HIO()); /* static daNpc_Ac1_HIO_c l_HIO */
}
VERIFY(0x021E4D38, __sinit_d_a_npc_ac1_cpp);

/* 021E4DD8: sead::SafeString deleting destructor (this TU's copy; vtable 0x10015B3C slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x021E4DD8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x021E4DD8, SafeString_dt);

/* 021E4DEC: daNpc_Ac1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ac1_c_dt(daNpc_Ac1_c* i_this, s32 flags) {
    WWHD_FUNC(0x021E4DEC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10015B54);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10015B64);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021E4DEC, daNpc_Ac1_c_dt);

/* 021E4E88: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x021E4E88, void, (SafeString*)nullptr);
}
VERIFY(0x021E4E88, SafeString_assureTerminationImpl);
