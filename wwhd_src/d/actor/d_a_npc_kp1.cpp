/**
 * d_a_npc_kp1.cpp (WWHD)
 * NPC - Maggie (rich, Windfall)
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx). Its structure is d_a_npc_km1's (verified; much of the code is shared),
 * plus the letter / heart-piece events, messages, and two hand-held models; "differs from Km1"
 * marks the differences.
 */
#include "d/actor/d_a_npc_kp1.h"

#define SAFESTRING_VTBL 0x1001CDC4 /* this TU's sead::SafeString vtable */
#define KP1_VTBL 0x1001D070        /* daNpc_Kp1_c vtable (HD: merged with fopNpc_npc_c's) */

enum { fpcNm_NPC_KP1_e = 0x163 };

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_ba1.cpp / d_a_npc_ls1.cpp) ---- */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}

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
enum : u32 { PMF_wait_action1 = 0x1001CDB0 };

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

/* more local bindings (SHARED-CANDIDATE) */
static inline BOOL dComIfGs_isEventBit_l(u16 f) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
static inline void dComIfGs_setEventReg_l(u16 r, u8 v) {
    dSv_event_setEventReg(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), r, v);
}
static inline u8 dComIfGs_getEventReg_l(u16 r) {
    return dSv_event_getEventReg(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), r);
}
/* save info: dSv_player_bag_item_c at +0x96, dSv_player_get_bag_item_c at +0xB0 */
static inline BOOL dComIfGs_isReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, gabi::load<u32>(0x101F84DC) + 0xB0, i); }
static inline BOOL dComIfGs_isBeast(u8 i) { return gabi::call<BOOL>(0x025B7690, gabi::load<u32>(0x101F84DC) + 0xB0, i); }
static inline BOOL dComIfGs_checkReserveItem(u8 item) { return gabi::call<BOOL>(0x025B7570, gabi::load<u32>(0x101F84DC) + 0x96, item); }
static inline void dComIfGs_setReserveItemEmpty() { gabi::call(0x025B7270, gabi::load<u32>(0x101F84DC) + 0x96); }
/* event manager (play + 0x52C4) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline void dComIfGp_evmng_CancelPresent() { gabi::call(0x02544980, dComIfGp_getPEvtManager()); }
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 item, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, item, argFlag, bitNo, roomNo, angle, scale);
}
static inline void fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    gabi::call(0x025D77DC, a, name, flag, hind);
}
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc) {
    return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc);
}

/* ---- file statics ---- */
/* daNpc_Kp1_HIO_c: unlike Km1/Pm1, the vtable pointer stays first (GameCube layout) */
struct daNpc_Kp1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> field_0;
        /* 0x02 */ be<s16> field_2;
        /* 0x04 */ be<s16> field_4;
        /* 0x06 */ be<s16> field_6;
        /* 0x08 */ be<s16> field_8;
        /* 0x0A */ be<s16> field_A;
        /* 0x0C */ be<s16> field_C;
        /* 0x0E */ be<s16> field_E;
        /* 0x10 */ be<s16> field_10;
        /* 0x12 */ be<s16> field_12;
        /* 0x14 */ be<f32> mAttentionArrowYOffset;
        /* 0x18 */ be<u8> field_18; /* HD: a byte flag (debug colours in _draw) */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Kp1_HIO_c, 0x28);
static daNpc_Kp1_HIO_c& l_HIO() { return *gabi::at<daNpc_Kp1_HIO_c>(0x10467908); }

/* 0227D61C daNpc_Kp1_HIO_c::daNpc_Kp1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Kp1_HIO_c* daNpc_Kp1_HIO_c_ct(daNpc_Kp1_HIO_c* i_this) {
    WWHD_FUNC(0x0227D61C, daNpc_Kp1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Kp1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001CDFC;
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101BFEDC), 0x1C); /* a_prm_tbl */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x0227D61C, daNpc_Kp1_HIO_c_ct);

/* 0227AEE4 */
static BOOL nodeCallBack_Kp(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0227AEE4, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Kp1_c* kp1Actor = gabi::at<daNpc_Kp1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (kp1Actor) {
            /* static cXyz a_att_pos_offst(0, 0, 0): guard 0x10467958, object 0x10467940 */
            cXyz* a_att_pos_offst = gabi::at<cXyz>(0x10467940);
            if (gabi::load<u32>(0x10467958) == 0) {
                a_att_pos_offst->x = 0.0f;
                a_att_pos_offst->z = 0.0f;
                gabi::store<u32>(0x10467958, 1);
                a_att_pos_offst->y = 0.0f;
            }
            /* static cXyz a_eye_pos_offst(20, -20, 0): guard 0x1046795C, object 0x1046794C */
            cXyz* a_eye_pos_offst = gabi::at<cXyz>(0x1046794C);
            if (gabi::load<u32>(0x1046795C) == 0) {
                gabi::store<u32>(0x1046795C, 1);
                a_eye_pos_offst->x = 20.0f;
                a_eye_pos_offst->y = -20.0f;
                a_eye_pos_offst->z = 0.0f;
            }
            u32 jointIdx = jntNo_of(i_node);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(getAnmMtx(model, jointIdx), stk);
            if (jointIdx == (u32)(s32)kp1Actor->m_head_jnt_num) {
                PSMTXMultVec(stk, a_att_pos_offst, &kp1Actor->mAttPos);
                /* differs from Km1: Y instead of X, both angles negated */
                mDoMtx_YrotM(stk, (s16)-kp1Actor->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, (s16)-kp1Actor->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, a_eye_pos_offst, &kp1Actor->mEyePos);
            } else if (jointIdx == (u32)(s32)kp1Actor->m_backbone_jnt_num) {
                mDoMtx_XrotM(stk, kp1Actor->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk, (s16)-kp1Actor->m_jnt.mAngles[1][0]); /* differs from Km1: negated */
            }
            PSMTXCopy(stk, j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jointIdx), stk);
        }
    }
    return TRUE;
}
VERIFY(0x0227AEE4, nodeCallBack_Kp);

/* 0227BC14 */
bool daNpc_Kp1_c::createInit() {
    WWHD_FUNC(0x0227BC14, bool, this);
    if (!dComIfGs_isEventBit_l(0x2D01)) {
        return false;
    }
    mEventCut.setActorInfo2(STR(0x1001CF54) /* "Kp1" */, (fopNpc_npc_c*)(void*)this);
    gravity = -4.0f;
    field_0x798.copy(current.pos);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
    field_0x7B6 = 0xFF;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[SPEAK] */
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    field_0x7BC = 1;
    setMtx();
    return true;
}
VERIFY(0x0227BC14, &daNpc_Kp1_c::createInit);

/* 0227B9AC */
void daNpc_Kp1_c::setMtx() {
    WWHD_FUNC(0x0227B9AC, void, this);
    if (field_0x7C7 == 0) {
        playTexPatternAnm();
        field_0x7B4 = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < field_0x7A4) {
            field_0x7B4 = 1;
        }
        field_0x7A4 = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    /* differs from Km1: the two hand-held models follow the left hand */
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hand_jnt_num), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpItemModel[0], mDoMtx_stack_c::get());
    J3DModel_calc(mpItemModel[0]);
    J3DModel_setBaseTRMtx(mpItemModel[1], mDoMtx_stack_c::get());
    J3DModel_calc(mpItemModel[1]);
    setAttention();
}
VERIFY(0x0227B9AC, &daNpc_Kp1_c::setMtx);

/* 0227CAB4 */
bool daNpc_Kp1_c::anmResID(int i_num, be<s32>* o_bck_num, be<s32>* o_bas_num) {
    WWHD_FUNC(0x0227CAB4, bool, this, i_num, o_bck_num, o_bas_num);
    /* a_anm_idx_tbl[6][2] (.rodata 0x1001CFEC) = {{5,-1},{1,-1},{0,-1},{3,-1},{2,-1},{4,-1}} */
    if ((u32)i_num >= 6) /* JUT_ASSERT(301, 0 <= i_num && i_num < ANM_END) */
        JUT_ASSERT_fail(STR(0x1001D01C), 0x12D, STR(0x1001D02C));
    if (o_bck_num == nullptr || o_bas_num == nullptr) /* JUT_ASSERT(302, o_bck_num && o_bas_num) */
        JUT_ASSERT_fail(STR(0x1001D01C), 0x12E, STR(0x1001D04C));
    u32 e = 0x1001CFEC + i_num * 8;
    *o_bck_num = gabi::load<s32>(e);
    *o_bas_num = gabi::load<s32>(e + 4);
    return true;
}
VERIFY(0x0227CAB4, &daNpc_Kp1_c::anmResID);

/* 0227B168 */
void daNpc_Kp1_c::BtpNum2ResID(int i_num, be<s32>* o_btp_num) {
    WWHD_FUNC(0x0227B168, void, this, i_num, o_btp_num);
    if ((u32)i_num >= 2) /* JUT_ASSERT(324, 0 <= i_num && i_num < TEXPATTERN_END) */
        JUT_ASSERT_fail(STR(0x1001CE3C), 0x144, STR(0x1001CE4C));
    *o_btp_num = gabi::load<s32>(0x1001CE34 + i_num * 4); /* a_btp_arc_ix_tbl[i_num] */
}
VERIFY(0x0227B168, &daNpc_Kp1_c::BtpNum2ResID);

/* 0227CB64 */
u32 daNpc_Kp1_c::setAnm_tex(s8 i_param_1) {
    WWHD_FUNC(0x0227CB64, u32, this, i_param_1);
    if (i_param_1 >= 0 && i_param_1 != field_0x7CD) {
        field_0x7CD = i_param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x0227CB64, &daNpc_Kp1_c::setAnm_tex);

/* 0227B1D0 */
bool daNpc_Kp1_c::init_btp(u32 param_1, int param_2) {
    WWHD_FUNC(0x0227B1D0, bool, this, param_1, param_2);
    J3DModelData* pJVar4 = J3DModel_getModelData_l(mpMorf->getModel());
    if (param_2 >= 0) {
        gabi::Local<be<s32>> btpId;
        BtpNum2ResID(param_2, btpId);
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x1001CE78) /* "Kp" */, *btpId);
        if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(357, m_head_tex_pattern != NULL) */
            JUT_ASSERT_fail(STR(0x1001CE7C), 0x165, STR(0x1001CE8C));
        if (mDoExt_btpAnm_init(mBtpAnm, pJVar4, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE) == 0) {
            return false;
        }
        mBtpFrame = 0;
        field_0x6F2 = 0;
        if (field_0x7CD == 1) { /* differs from Km1: pattern 1 starts at frame 1 */
            mBtpFrame = 1;
        }
    }
    return true;
}
VERIFY(0x0227B1D0, &daNpc_Kp1_c::init_btp);

/* 0227B2FC */
bool daNpc_Kp1_c::initTexPatternAnm(u32 param_1) {
    WWHD_FUNC(0x0227B2FC, bool, this, param_1);
    bool var_31 = false;
    if (init_btp(param_1, field_0x7CD)) {
        var_31 = true;
    }
    return var_31;
}
VERIFY(0x0227B2FC, &daNpc_Kp1_c::initTexPatternAnm);

/* 0227B854 */
void daNpc_Kp1_c::playTexPatternAnm() {
    WWHD_FUNC(0x0227B854, void, this);
    s8 tex = field_0x7CD;
    if (tex == 1) { /* differs from Km1: pattern 1 is not animated */
        return;
    }
    if (tex == 0 && cLib_calcTimer(&field_0x6F2) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < J3DAnm_getFrameMax(m_head_tex_pattern)) {
        return;
    }
    if (field_0x7CD) {
        mBtpFrame = (u8)J3DAnm_getFrameMax(m_head_tex_pattern);
    } else {
        mBtpFrame = 0;
        field_0x6F2 = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
    }
}
VERIFY(0x0227B854, &daNpc_Kp1_c::playTexPatternAnm);

/* 0227CB88 */
s32 daNpc_Kp1_c::setAnm_anm(anm_prm_c* i_anm_ptr) {
    WWHD_FUNC(0x0227CB88, s32, this, i_anm_ptr);
    s32 uVar2 = 0;
    s8 num = i_anm_ptr->bckNum;
    if (num >= 0 && field_0x7CE != num) {
        field_0x7CE = num;
        if (mpMorf.get() != nullptr) {
            gabi::Local<be<s32>> local_18;
            gabi::Local<be<s32>> local_14;
            anmResID(num, local_18, local_14);
            if (*local_18 >= 0) {
                dNpc_setAnmIDRes(mpMorf, i_anm_ptr->loopMode, i_anm_ptr->morf, i_anm_ptr->speed, *local_18, *local_14, STR(0x1001D064) /* "Kp" */);
            }
            uVar2 = 1;
        }
        field_0x7B5 = 0;
        field_0x7A4 = 0.0f;
        field_0x7B4 = 0;
    }
    return uVar2;
}
VERIFY(0x0227CB88, &daNpc_Kp1_c::setAnm_anm);

/* 0227CC44 (HD-only name; GameCube setAnm_NUM) */
void daNpc_Kp1_c::setAnm_NUM(int i_idx, int i_tex) {
    WWHD_FUNC(0x0227CC44, void, this, i_idx, i_tex);
    anm_prm_c* tbl = gabi::at<anm_prm_c>(0x101BFDDC); /* a_anm_prm_tbl */
    if (i_tex != 0) {
        setAnm_tex(tbl[i_idx].btpNum);
    }
    setAnm_anm(&tbl[i_idx]);
}
VERIFY(0x0227CC44, &daNpc_Kp1_c::setAnm_NUM);

/* 0227CCB0 */
void daNpc_Kp1_c::setAnm() {
    WWHD_FUNC(0x0227CCB0, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BFE3C);
    setAnm_tex(a_anm_prm_tbl[field_0x7D0].btpNum);
    setAnm_anm(&a_anm_prm_tbl[field_0x7D0]);
}
VERIFY(0x0227CCB0, &daNpc_Kp1_c::setAnm);

/* 0227CD8C */
void daNpc_Kp1_c::chngAnmAtr(u32 param_1) {
    WWHD_FUNC(0x0227CD8C, void, this, (u32)(u8)param_1);
    if (param_1 >= 7 || param_1 == field_0x7CB) {
        return;
    }
    /* which hand-held model _draw shows */
    if (param_1 == 2) {
        m7F0 = 2;
    } else if (param_1 == 3) {
        m7F0 = 1;
    } else {
        m7F0 = 0;
    }
    field_0x7CB = param_1;
    setAnm_ATR(1);
}
VERIFY(0x0227CD8C, &daNpc_Kp1_c::chngAnmAtr);

/* 0227CD1C */
void daNpc_Kp1_c::setAnm_ATR(int param_1) {
    WWHD_FUNC(0x0227CD1C, void, this, param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BFE6C); /* [7] */
    if (param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[field_0x7CB].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[field_0x7CB]);
}
VERIFY(0x0227CD1C, &daNpc_Kp1_c::setAnm_ATR);

/* 0227CDE4 */
void daNpc_Kp1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x0227CDE4, void, this);
    switch (field_0x7CB) {
    case 1:
        if (field_0x7B4 == 0)
            return;
        field_0x7B5 = field_0x7B5 + 1;
        if (field_0x7B5 < 2)
            return;
        break;
    case 6:
        if (field_0x7B4 == 0)
            return;
        field_0x7B5 = field_0x7B5 + 1;
        if (field_0x7B5 <= 0)
            return;
        break;
    default:
        return;
    }
    field_0x7CB = 0;
    setAnm_NUM(0, 1);
}
VERIFY(0x0227CDE4, &daNpc_Kp1_c::ctrlAnmAtr);

/* 0227CE54 */
void daNpc_Kp1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x0227CE54, void, this, i_msgStatus);
    if (i_msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (field_0x7D6 == 0) {
            field_0x7CC = 0xFF;
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x7D6 = field_0x7D6 + 1;
        }
        u8 uVar1 = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (uVar1 != 0xFF && uVar1 != field_0x7CC) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            field_0x7CC = uVar1;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == 14 /* fopMsgStts_MSG_DISPLAYED_e */) {
        field_0x7D6 = 0;
    }
    ctrlAnmAtr();
    /* ctrlAnmTag(): empty */
}
VERIFY(0x0227CE54, &daNpc_Kp1_c::anmAtr);

/* 0227CF10 */
void daNpc_Kp1_c::setStt(s8 param_1) {
    WWHD_FUNC(0x0227CF10, void, this, param_1);
    s8 uVar1 = field_0x7D0;
    field_0x7D0 = param_1;
    m7F0 = 0;
    if (param_1 == 2) {
        field_0x7D1 = uVar1;
        m932 = 0;
        field_0x7D2 = 1;
        field_0x7CB = 0xFF;
        m_jnt.mbTrn = 1; /* setTrn() */
        return;
    }
    setAnm();
}
VERIFY(0x0227CF10, &daNpc_Kp1_c::setStt);

/* 0227CF50 */
u16 daNpc_Kp1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x0227CF50, u16, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgMng = gabi::load<u32>(0x101F4B5C);
    switch ((u32)*pMsgNo) {
    case 0x1E84: *pMsgNo = 0x1E85; break;
    case 0x1E85: *pMsgNo = 0x1E86; break;
    case 0x1E88: *pMsgNo = 0x1E89; break;
    case 0x1E8A: *pMsgNo = 0x1E8B; break;
    case 0x1E8B: *pMsgNo = 0x1E8C; break;
    case 0x1E8C: *pMsgNo = 0x1E8D; break;
    case 0x1E8D: *pMsgNo = 0x1E8E; break;
    case 0x1E8E: *pMsgNo = 0x1E8F; break;
    case 0x1E8F: {
        u32 select = gabi::load<u32>(msgMng + 0x948); /* the answer chosen */
        if (select == 0) {
            *pMsgNo = 0x1E91;
        } else if (select == 1) {
            *pMsgNo = 0x1E90;
        }
        break;
    }
    case 0x1E90:
        m931 = 1;
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    case 0x1E91:
        field_0x7CF = 3;
        msgStatus = 0x10;
        break;
    case 0x1E96: *pMsgNo = 0x1E97; m932 = 1; break;
    case 0x1E97: *pMsgNo = 0x1E98; break;
    case 0x1E98: *pMsgNo = 0x1E99; break;
    case 0x1E99: *pMsgNo = 0x1E9A; break;
    case 0x1E9A: *pMsgNo = 0x1E9B; break;
    case 0x1E9B: *pMsgNo = 0x1E9C; break;
    case 0x1E9C: *pMsgNo = 0x1E9D; break;
    case 0x1E9D:
        field_0x7CF = 4;
        msgStatus = 0x10;
        break;
    default:
        msgStatus = 0x10;
        break;
    }
    return msgStatus;
}
VERIFY(0x0227CF50, &daNpc_Kp1_c::next_msgStatus);

/* 0227D104 */
u32 daNpc_Kp1_c::getMsg() {
    WWHD_FUNC(0x0227D104, u32, this);
    u8 item = field_0x7B6;
    if (item != 0xFF) { /* an item was shown */
        if (item == 0x9B) {
            return 0x1E96;
        }
        if (item == 0x45) {
            return 0x1EA0;
        }
        return 0x1E9F;
    }
    if (m92F) {
        m92F = 0;
        return 0x1E92;
    }
    if (m930) {
        m930 = 0;
        return 0x1E9E;
    }
    if (dComIfGs_isReserve(0xF) && !dComIfGs_checkReserveItem(0x9B)) {
        if (dComIfGs_getEventReg_l(0xCCFF) == 0) {
            return 0x1E83;
        }
        return 0x1E84;
    }
    if (dComIfGs_isReserve(0xE)) {
        if (dComIfGs_isBeast(0)) {
            return 0x1E87;
        }
        return 0x1E88;
    }
    if (m931) {
        return 0x1E93;
    }
    return 0x1E8A;
}
VERIFY(0x0227D104, &daNpc_Kp1_c::getMsg);

/* 0227C714 */
void daNpc_Kp1_c::eventOrder() {
    WWHD_FUNC(0x0227C714, void, this);
    s8 order = field_0x7CF;
    if (order == 1 || order == 2) {
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 0x21); /* eventInfo.onCondition(dEvtCnd_CANTALK_e | CANTALKITEM) */
        if (field_0x7CF == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        /* l_evn_tbl[order] (.data 0x101BFDC8): "GET_MAGYS_LTTR", "GET_KAKERA_HRT" for 3, 4 */
        fopAcM_orderOtherEvent2(this, gabi::at<const char>(gabi::load<u32>(0x101BFDC8 + order * 4)), 1, 0xFFFF);
    }
}
VERIFY(0x0227C714, &daNpc_Kp1_c::eventOrder);

/* 0227BF00 */
void daNpc_Kp1_c::checkOrder() {
    WWHD_FUNC(0x0227BF00, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* dEvtCmd_INDEMO_e */) {
        if ((dComIfGp_evmng_startCheckOld(STR(0x1001CF6C) /* "GET_MAGYS_LTTR" */) && field_0x7CF == 3) ||
            (dComIfGp_evmng_startCheckOld(STR(0x1001CF7C) /* "GET_KAKERA_HRT" */) && field_0x7CF == 4)) {
            field_0x7CF = 0;
        }
    } else if (command == 1 /* dEvtCmd_INTALK_e */) {
        if (field_0x7CF == 1 || field_0x7CF == 2) {
            field_0x7CF = 0;
            field_0x7C5 = 1;
        }
    }
}
VERIFY(0x0227BF00, &daNpc_Kp1_c::checkOrder);

/* 0227C41C */
void daNpc_Kp1_c::lookBack() {
    WWHD_FUNC(0x0227C41C, void, this);
    gabi::Local<cXyz> vec1;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    vec1->z = 0.0f;
    vec1->x = 0.0f;
    u8 headOnlyFollow = mHeadOnlyFollow;
    f32 srcX = current.pos.x;
    vec1->y = 0.0f;
    s16 targetY = current.angle.y;
    cXyz* dstPos = nullptr;
    switch ((u32)(s32)field_0x7D2) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        srcX = current.pos.x;
        vec1->copy(*eye);
        srcY = eyePos.y;
        dstPos = vec1;
        srcZ = current.pos.z;
        break;
    }
    case 2:
        vec1->copy(field_0x798);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        dstPos = vec1;
        break;
    case 3:
        targetY = field_0x7B2;
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&field_0x7B0, l_HIO().mPrmTbl.field_12, 4, 0x800);
    } else {
        field_0x7B0 = 0;
    }
    gabi::Local<cXyz> vec2; /* passed by value: a copy */
    vec2->x = srcX;
    vec2->y = srcY;
    vec2->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec2, targetY, field_0x7B0, headOnlyFollow);
}
VERIFY(0x0227C41C, &daNpc_Kp1_c::lookBack);

/* 0227D2C0 */
u8 daNpc_Kp1_c::chkAttention() {
    WWHD_FUNC(0x0227D2C0, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x0227D2C0, &daNpc_Kp1_c::chkAttention);

/* 0227B94C */
void daNpc_Kp1_c::setAttention() {
    WWHD_FUNC(0x0227B94C, void, this);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO().mPrmTbl.mAttentionArrowYOffset;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!field_0x7BC && !field_0x7C0) {
        return;
    }
    f32 z = mEyePos.z;
    f32 ex = mEyePos.x;
    eyePos.z = z;
    f32 ey = mEyePos.y;
    eyePos.x = ex;
    eyePos.y = ey;
    field_0x7BC = 0;
}
VERIFY(0x0227B94C, &daNpc_Kp1_c::setAttention);

/* 0227B700 */
bool daNpc_Kp1_c::decideType(int i_type) {
    WWHD_FUNC(0x0227B700, bool, this, i_type);
    field_0x7D3 = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_KP1_e) {
        field_0x7D4 = 0;
        field_0x7D3 = 0;
    }
    return true;
}
VERIFY(0x0227B700, &daNpc_Kp1_c::decideType);

/* 0227C1E0 */
void daNpc_Kp1_c::event_actionInit(int param_1) {
    WWHD_FUNC(0x0227C1E0, void, this, param_1);
    be<s32>* puVar1 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001CF90) /* "ActNo" */);
    dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001CF98) /* "Timer" */);
    if (puVar1 != nullptr) {
        field_0x7CA = (s8)(s32)*puVar1;
    }
}
VERIFY(0x0227C1E0, &daNpc_Kp1_c::event_actionInit);

/* 0227C268 */
bool daNpc_Kp1_c::event_action() {
    WWHD_FUNC(0x0227C268, bool, this);
    bool ret = false;
    switch ((u32)(s32)field_0x7CA) {
    case 0: { /* hand over the item 0x9A */
        u32 id = fopAcM_createItemForPresentDemo(&current.pos, 0x9A, 0, -1, -1, nullptr, nullptr);
        if (id != (u32)-1) {
            dComIfGp_event_setItemPartnerId(id);
            ret = true;
        }
        break;
    }
    case 1: { /* hand over the item 7 */
        u32 id = fopAcM_createItemForPresentDemo(&current.pos, 7, 0, -1, -1, nullptr, nullptr);
        if (id != (u32)-1) {
            dComIfGp_event_setItemPartnerId(id);
            ret = true;
        }
        break;
    }
    default:
        ret = true;
        break;
    }
    return ret;
}
VERIFY(0x0227C268, &daNpc_Kp1_c::event_action);

/* 0227C32C */
void daNpc_Kp1_c::privateCut() {
    WWHD_FUNC(0x0227C32C, void, this);
    /* cut_name_tbl (.data 0x101BFDD0): "ACTION" */
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001CFA0) /* "Kp1" */, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    field_0x7C9 = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BFDD0, 1, TRUE, 0);
    if (field_0x7C9 == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        if (field_0x7C9 == 0) {
            event_actionInit(staffIdx);
        }
    }
    bool bVar1 = true;
    if (field_0x7C9 == 0) {
        bVar1 = event_action();
    }
    if (bVar1) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}
VERIFY(0x0227C32C, &daNpc_Kp1_c::privateCut);

/* 0227C1A0 */
void daNpc_Kp1_c::endEvent() {
    WWHD_FUNC(0x0227C1A0, void, this);
    dComIfGp_event_reset();
    field_0x7CB = 0xFF;
}
VERIFY(0x0227C1A0, &daNpc_Kp1_c::endEvent);

/* 0227C600 */
void daNpc_Kp1_c::event_proc() {
    WWHD_FUNC(0x0227C600, void, this);
    /* differs from Km1: the two letter/heart-piece events end here */
    if (dComIfGp_evmng_endCheckOld(STR(0x1001CFAC) /* "GET_MAGYS_LTTR" */)) {
        endEvent();
        shape_angle.x = current.angle.x;
        shape_angle.z = current.angle.z;
        field_0x7CF = 1;
        m92F = 1;
        shape_angle.y = current.angle.y;
    } else if (dComIfGp_evmng_endCheckOld(STR(0x1001CFBC) /* "GET_KAKERA_HRT" */)) {
        dComIfGs_setEventReg_l(0xCCFF, 0);
        endEvent();
        shape_angle.x = current.angle.x;
        shape_angle.z = current.angle.z;
        m930 = 1;
        field_0x7CF = 1;
        shape_angle.y = current.angle.y;
    } else {
        if (!mEventCut.cutProc()) {
            privateCut();
        }
        lookBack();
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
}
VERIFY(0x0227C600, &daNpc_Kp1_c::event_proc);

/* 0227B728 (unnamed by the matcher) */
bool daNpc_Kp1_c::set_action(ProcFunc_l* i_action, void* param_2) {
    WWHD_FUNC(0x0227B728, bool, this, i_action, param_2);
    ProcFunc_l* cur = &mAction;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return true;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return true;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        field_0x7D5 = -1;
        pmf_call(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7D5 = 0;
    pmf_call(this, cur, param_2);
    return true;
}
VERIFY(0x0227B728, &daNpc_Kp1_c::set_action);

/* 0227D348 */
BOOL daNpc_Kp1_c::chk_talk() {
    WWHD_FUNC(0x0227D348, BOOL, this);
    BOOL ret = TRUE;
    field_0x7B6 = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd() == 0) {
            return FALSE;
        }
        field_0x7B6 = dComIfGp_event_getPreItemNo();
    }
    return ret;
}
VERIFY(0x0227D348, &daNpc_Kp1_c::chk_talk);

/* 0227D3C8 */
BOOL daNpc_Kp1_c::wait01() {
    WWHD_FUNC(0x0227D3C8, BOOL, this);
    s8 order = field_0x7CF;
    if (order == 3 || order == 4 || order == 1) {
        return TRUE;
    }
    if (field_0x7C5 != 0) {
        if (chk_talk()) {
            setStt(2);
        }
    } else {
        field_0x7CF = 2;
        if (field_0x7C4) {
            field_0x7D2 = 1;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            s16 y = field_0x76C.y;
            field_0x7D2 = 3;
            field_0x7B2 = y;
            m_jnt.mbTrn = 1;
        }
    }
    return TRUE;
}
VERIFY(0x0227D3C8, &daNpc_Kp1_c::wait01);

/* 0227D480 */
BOOL daNpc_Kp1_c::talk01() {
    WWHD_FUNC(0x0227D480, BOOL, this);
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        u32 status = fopMsgM_getStatus();
        if (status == 2 || status == 6) {
            if (m932) {
                dComIfGp_evmng_CancelPresent();
                m932 = 0;
            }
        } else if (status == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            if (mCurrMsgNo == 0x1E9D) {
                dComIfGs_setReserveItemEmpty();
            }
            field_0x7B6 = 0xFF;
            setStt(field_0x7D1);
            field_0x7C8 = 0;
            field_0x7C5 = 0;
            endEvent();
        }
    }
    return TRUE;
}
VERIFY(0x0227D480, &daNpc_Kp1_c::talk01);

/* 0227D558 */
BOOL daNpc_Kp1_c::wait_action1(void*) {
    WWHD_FUNC(0x0227D558, BOOL, this, (void*)nullptr);
    if (field_0x7D5 == 0) {
        setStt(1);
        field_0x7D5 = field_0x7D5 + 1;
    } else if (field_0x7D5 != -1) {
        field_0x7C4 = chkAttention();
        switch ((u32)(s32)field_0x7D0) {
        case 2:
            field_0x7C0 = talk01();
            break;
        case 1:
            field_0x7C0 = wait01();
            break;
        default:
            field_0x7C0 = 0;
            break;
        }
        lookBack();
    }
    return 1;
}
VERIFY(0x0227D558, &daNpc_Kp1_c::wait_action1);

/* 0227BFE0 */
u8 daNpc_Kp1_c::demo() {
    WWHD_FUNC(0x0227BFE0, u8, this);
    if (demoActorID == 0) {
        if (field_0x7C7 != 0) {
            field_0x7C7 = 0;
        }
    } else {
        u8 id = demoActorID;
        field_0x7C7 = 1;
        void* demoAc = nullptr;
        /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
        if (id != 0 && id <= 0x20) {
            u32 obj = gabi::load<u32>(0x101D5FFC);
            if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
                JUT_ASSERT_fail(STR(0x1001CE1C), 0x23A, STR(0x1001CE0C));
                obj = gabi::load<u32>(0x101D5FFC);
            }
            demoAc = gabi::call<void*>(0x02526E70, obj, id);
        }
        /* differs from Km1: the eye pattern runs, and the demo may replace it */
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= J3DAnm_getFrameMax(m_head_tex_pattern)) {
            mBtpFrame = (u8)J3DAnm_getFrameMax(m_head_tex_pattern);
        }
        if (demoAc != nullptr) {
            J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(demoAc, STR(0x1001CF8C) /* "Kp" */);
            if (btp != nullptr) {
                m_head_tex_pattern = btp;
                if (mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), btp, TRUE, 2, 1.0f, 0, -1, TRUE, FALSE)) {
                    mBtpFrame = 0;
                }
            }
        }
        dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x1001CF8C) /* "Kp" */, 0, nullptr, 0, 0);
    }
    return field_0x7C7;
}
VERIFY(0x0227BFE0, &daNpc_Kp1_c::demo);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 0227C930 */
BOOL daNpc_Kp1_c::_draw() {
    WWHD_FUNC(0x0227C930, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(model);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove() */
    /* differs from Km1: the hand-held model chosen by chngAnmAtr */
    J3DModel* item = nullptr;
    bool draw = true;
    if (m7F0 == 1) {
        item = mpItemModel[0];
    } else if (m7F0 == 2) {
        item = mpItemModel[1];
    } else {
        draw = false;
    }
    if (draw && item != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), item, &tevStr);
        mDoExt_modelEntryDL(item);
    }
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO().mPrmTbl.field_18) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001CDB8);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001CDBC);
    }
    dSnap_RegistFig(0x5A /* DSNAP_TYPE_NPC_KP1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0227C930, &daNpc_Kp1_c::_draw);

/* 0227C774 */
BOOL daNpc_Kp1_c::_execute() {
    WWHD_FUNC(0x0227C774, BOOL, this);
    if (field_0x7B8 == 0) {
        field_0x774.copy(current.pos);
        field_0x76C.x = current.angle.x;
        field_0x76C.y = current.angle.y;
        field_0x76C.z = current.angle.z;
        field_0x7B8 = 1;
    }
    daNpc_Kp1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    m_jnt.setParam(prm.field_8, prm.field_A, prm.field_C, prm.field_E, prm.field_0, prm.field_2, prm.field_4,
                   prm.field_6, prm.field_10);
    checkOrder();
    if (demo() == 0) {
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            event_proc();
        } else {
            pmf_call(this, &mAction, nullptr); /* (this->*field_0x6F4)(NULL) */
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    if (field_0x7C7 == 0) {
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    }
    setMtx();
    setCollision(60.0f, 170.0f);
    return true;
}
VERIFY(0x0227C774, &daNpc_Kp1_c::_execute);

/* 0227BEA4 (unnamed by the matcher: _delete) */
BOOL daNpc_Kp1_c::_delete() {
    WWHD_FUNC(0x0227BEA4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001CF67) /* "Kp" */);
    /* differs from Km1: no HIO child; the animation is stopped only if the heap was made */
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x0227BEA4, &daNpc_Kp1_c::_delete);

/* 0227B6FC (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* actor) {
    WWHD_FUNC(0x0227B6FC, BOOL, actor);
    return ((daNpc_Kp1_c*)actor)->CreateHeap();
}
VERIFY(0x0227B6FC, CheckCreateHeap);

/* 0227BD5C */
cPhs_State daNpc_Kp1_c::_create() {
    WWHD_FUNC(0x0227BD5C, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Kp1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);          /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KP1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);       /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut);    /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    /* differs from Km1: the resource first, no HIO child */
    cPhs_State resLoadResult = dComIfG_resLoad(&mPhs, STR(0x1001CF64) /* "Kp" */);
    if (resLoadResult != cPhs_COMPLEATE_e) {
        return resLoadResult;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, 0x0227B6FC /* CheckCreateHeap */, gabi::load<u32>(0x101BFDCC))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 170.0f, 50.0f);
    if (createInit() == 0) {
        resLoadResult = cPhs_ERROR_e;
    }
    return resLoadResult;
}
VERIFY(0x0227BD5C, &daNpc_Kp1_c::_create);

/* 0227B340 */
BOOL daNpc_Kp1_c::CreateHeap() {
    WWHD_FUNC(0x0227B340, BOOL, this);
    J3DModelData* a_mdl_data = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CEB8) /* "Kp" */, 8);
    if (a_mdl_data == nullptr) /* JUT_ASSERT(1593, a_mdl_data != NULL) */
        JUT_ASSERT_fail(STR(0x1001CEC4), 0x639, STR(0x1001CF00));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001CEB8), 5);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_data, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() != nullptr) {
        m_head_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_data), STR(0x1001CEBC) /* "head" */);
        if (m_head_jnt_num < 0)
            JUT_ASSERT_fail(STR(0x1001CEC4), 0x64D, STR(0x1001CEEC));
        m_backbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_data), STR(0x1001CF2C) /* "backbone" */);
        if (m_backbone_jnt_num < 0)
            JUT_ASSERT_fail(STR(0x1001CEC4), 0x64F, STR(0x1001CED4));
        m_hand_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_data), STR(0x1001CEB0) /* "handL" */);
        if (m_hand_jnt_num < 0)
            JUT_ASSERT_fail(STR(0x1001CEC4), 0x651, STR(0x1001CF38));
        field_0x7CD = gabi::load<s8>(0x101BFDC8); /* a_tex_pattern_num_tbl[0] */
        if (initTexPatternAnm(false) != 0) {
            /* differs from Km1: two hand-held models (ids 6 and 7) */
            J3DModelData* itemData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CEB8), 6);
            if (itemData == nullptr)
                JUT_ASSERT_fail(STR(0x1001CEC4), 0x66B, STR(0x1001CF14));
            mpItemModel[0] = mDoExt_J3DModel__create(itemData, 0x80000, 0x11000002);
            if (mpItemModel[0].get() != nullptr) {
                itemData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001CEB8), 7);
                if (itemData == nullptr)
                    JUT_ASSERT_fail(STR(0x1001CEC4), 0x675, STR(0x1001CF14));
                mpItemModel[1] = mDoExt_J3DModel__create(itemData, 0x80000, 0x11000002);
                if (mpItemModel[1].get() != nullptr) {
                    for (u16 i = 0; i < J3DModelData_getJointNum(a_mdl_data); i++) {
                        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
                            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
                            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
                            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
                            if ((u32)i < n)
                                joint += i * 0x1C;
                            gabi::store<u32>(joint + 8, 0x0227AEE4);
                        }
                    }
                    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
                    mAcchCir.SetWall(30.0f, 60.0f);
                    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
                    return TRUE;
                }
            }
        }
        /* differs from Km1: the morf is deleted (virtual deleting destructor) */
        if (mpMorf.get() != nullptr) {
            u32 m = gabi::ea(mpMorf.get());
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(m) + 0xC), m, 3);
        }
    } else {
        u32 m = gabi::ea(mpMorf.get());
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(m) + 0xC), m, 3);
    }
    mpMorf = nullptr;
    return FALSE;
}
VERIFY(0x0227B340, &daNpc_Kp1_c::CreateHeap);

/* 0227BEA0 */
static cPhs_State daNpc_Kp1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0227BEA0, cPhs_State, i_this);
    return ((daNpc_Kp1_c*)i_this)->_create();
}
VERIFY(0x0227BEA0, daNpc_Kp1_Create);

/* 0227BEFC */
static BOOL daNpc_Kp1_Delete(daNpc_Kp1_c* i_this) {
    WWHD_FUNC(0x0227BEFC, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0227BEFC, daNpc_Kp1_Delete);

/* 0227C92C */
static BOOL daNpc_Kp1_Execute(daNpc_Kp1_c* i_this) {
    WWHD_FUNC(0x0227C92C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0227C92C, daNpc_Kp1_Execute);

/* 0227CAA8 */
static BOOL daNpc_Kp1_Draw(daNpc_Kp1_c* i_this) {
    WWHD_FUNC(0x0227CAA8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0227CAA8, daNpc_Kp1_Draw);

/* 0227CAAC */
static BOOL daNpc_Kp1_IsDelete(daNpc_Kp1_c*) {
    WWHD_FUNC(0x0227CAAC, BOOL, (daNpc_Kp1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0227CAAC, daNpc_Kp1_IsDelete);

/* 0227D688: static initialisation of the translation unit */
static void __sinit_d_a_npc_kp1_cpp() {
    WWHD_FUNC(0x0227D688, void);
    sinit_header_statics_z(0x104678FC, 0x101BFEF8, 0x10467930);
    daNpc_Kp1_HIO_c_ct(&l_HIO()); /* static daNpc_Kp1_HIO_c l_HIO */
}
VERIFY(0x0227D688, __sinit_d_a_npc_kp1_cpp);

/* 0227D728: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0227D728, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0227D728, SafeString_dt);

/* 0227D73C: daNpc_Kp1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Kp1_c_dt(daNpc_Kp1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0227D73C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001CDDC);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001CDEC);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0227D73C, daNpc_Kp1_c_dt);

/* 0227D7D8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0227D7D8, void, (SafeString*)nullptr);
}
VERIFY(0x0227D7D8, SafeString_assureTerminationImpl);
