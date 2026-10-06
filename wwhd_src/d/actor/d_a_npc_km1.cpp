/**
 * d_a_npc_km1.cpp (WWHD)
 * NPC - Mila (rich)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_km1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_km1.h"

#define SAFESTRING_VTBL 0x1001C604 /* this TU's sead::SafeString vtable */
#define KM1_VTBL 0x1001C7F8        /* daNpc_Km1_c vtable (HD: merged with fopNpc_npc_c's) */

enum { fpcNm_NPC_KM1_e = 0x14D };

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
enum : u32 { PMF_wait_action1 = 0x1001C5F0 };

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
/* daNpc_Km1_HIO_c, HD: the vtable pointer is last (+0x24) */
struct daNpc_Km1_HIO_c {
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
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> field_0x8;
    /* 0x08 */ hio_prm_c mPrmTbl;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Km1_HIO_c, 0x28);
static daNpc_Km1_HIO_c& l_HIO() { return *gabi::at<daNpc_Km1_HIO_c>(0x10467758); }

/* 02272FB0 daNpc_Km1_HIO_c::daNpc_Km1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Km1_HIO_c* daNpc_Km1_HIO_c_ct(daNpc_Km1_HIO_c* i_this) {
    WWHD_FUNC(0x02272FB0, daNpc_Km1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Km1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C63C;
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101BF700), 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x02272FB0, daNpc_Km1_HIO_c_ct);

/* 022713C0 */
static BOOL nodeCallBack_Km(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022713C0, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Km1_c* km1Actor = gabi::at<daNpc_Km1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (km1Actor) {
            /* static cXyz a_att_pos_offst(0, 0, 0): guard 0x104677A8, object 0x10467790 */
            cXyz* a_att_pos_offst = gabi::at<cXyz>(0x10467790);
            if (gabi::load<u32>(0x104677A8) == 0) {
                a_att_pos_offst->x = 0.0f;
                a_att_pos_offst->z = 0.0f;
                gabi::store<u32>(0x104677A8, 1);
                a_att_pos_offst->y = 0.0f;
            }
            /* static cXyz a_eye_pos_offst(20, 20, 0): guard 0x104677AC, object 0x1046779C */
            cXyz* a_eye_pos_offst = gabi::at<cXyz>(0x1046779C);
            if (gabi::load<u32>(0x104677AC) == 0) {
                a_eye_pos_offst->z = 0.0f;
                gabi::store<u32>(0x104677AC, 1);
                a_eye_pos_offst->y = 20.0f;
                a_eye_pos_offst->x = 20.0f;
            }
            u32 jointIdx = jntNo_of(i_node);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(getAnmMtx(model, jointIdx), stk);
            if (jointIdx == (u32)(s32)km1Actor->m_head_jnt_num) {
                PSMTXMultVec(stk, a_att_pos_offst, &km1Actor->mAttPos);
                mDoMtx_XrotM(stk, km1Actor->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, km1Actor->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, a_eye_pos_offst, &km1Actor->mEyePos);
            } else if (jointIdx == (u32)(s32)km1Actor->m_backbone_jnt_num) {
                mDoMtx_XrotM(stk, km1Actor->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk, km1Actor->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(stk, j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jointIdx), stk);
        }
    }
    return TRUE;
}
VERIFY(0x022713C0, nodeCallBack_Km);

/* 02271E78 */
bool daNpc_Km1_c::createInit() {
    WWHD_FUNC(0x02271E78, bool, this);
    mEventCut.setActorInfo2(STR(0x1001C754) /* "Km1" */, (fopNpc_npc_c*)(void*)this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gravity = -4.0f;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    field_0x798.copy(current.pos);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    field_0x7BC = 1;
    setMtx();
    return true;
}
VERIFY(0x02271E78, &daNpc_Km1_c::createInit);

/* 02271D1C */
void daNpc_Km1_c::setMtx() {
    WWHD_FUNC(0x02271D1C, void, this);
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
    setAttention();
}
VERIFY(0x02271D1C, &daNpc_Km1_c::setMtx);

/* 02272980 */
bool daNpc_Km1_c::anmResID(int i_num, be<s32>* o_bck_num, be<s32>* o_bas_num) {
    WWHD_FUNC(0x02272980, bool, this, i_num, o_bck_num, o_bas_num);
    /* a_anm_idx_tbl[1][2] = {dRes_ID_KM_BCK_KM_WAIT01_e (1), dRes_ID_KM_BAS_KM_WAIT01_e (0)} */
    if (i_num != 0) /* JUT_ASSERT(285, 0 <= i_num && i_num < ANM_END) */
        JUT_ASSERT_fail(STR(0x1001C7A4), 0x11D, STR(0x1001C7B4));
    if (o_bck_num == nullptr || o_bas_num == nullptr) /* JUT_ASSERT(286, o_bck_num && o_bas_num) */
        JUT_ASSERT_fail(STR(0x1001C7A4), 0x11E, STR(0x1001C7D4));
    *o_bck_num = 1;
    *o_bas_num = 0;
    return true;
}
VERIFY(0x02272980, &daNpc_Km1_c::anmResID);

/* 02271624 */
void daNpc_Km1_c::BtpNum2ResID(int i_num, be<s32>* o_btp_num) {
    WWHD_FUNC(0x02271624, void, this, i_num, o_btp_num);
    if (i_num != 0) /* JUT_ASSERT(307, 0 <= i_num && i_num < TEXPATTERN_END) */
        JUT_ASSERT_fail(STR(0x1001C674), 0x133, STR(0x1001C684));
    *o_btp_num = gabi::load<s32>(0x1001C670); /* a_btp_arc_ix_tbl[0] */
}
VERIFY(0x02271624, &daNpc_Km1_c::BtpNum2ResID);

/* 02272A10 */
u32 daNpc_Km1_c::setAnm_tex(s8 i_param_1) {
    WWHD_FUNC(0x02272A10, u32, this, i_param_1);
    if (i_param_1 >= 0 && i_param_1 != field_0x7CD) {
        field_0x7CD = i_param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x02272A10, &daNpc_Km1_c::setAnm_tex);

/* 02271678 */
bool daNpc_Km1_c::init_btp(u32 param_1, int param_2) {
    WWHD_FUNC(0x02271678, bool, this, param_1, param_2);
    J3DModelData* pJVar4 = J3DModel_getModelData_l(mpMorf->getModel());
    if (param_2 >= 0) {
        gabi::Local<be<s32>> btpId;
        BtpNum2ResID(param_2, btpId);
        const char* key_arc = STR(0x1001C6B0); /* "Km" */
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(key_arc, *btpId);
        if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(340, m_head_tex_pattern != NULL) */
            JUT_ASSERT_fail(STR(0x1001C6B4), 0x154, STR(0x1001C6C4));
        if (mDoExt_btpAnm_init(mBtpAnm, pJVar4, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE) == 0) {
            return false;
        }
        field_0x6F2 = 0;
        mBtpFrame = 0;
    }
    return true;
}
VERIFY(0x02271678, &daNpc_Km1_c::init_btp);

/* 0227178C */
bool daNpc_Km1_c::initTexPatternAnm(u32 param_1) {
    WWHD_FUNC(0x0227178C, bool, this, param_1);
    bool var_31 = false;
    if (init_btp(param_1, field_0x7CD)) {
        var_31 = true;
    }
    return var_31;
}
VERIFY(0x0227178C, &daNpc_Km1_c::initTexPatternAnm);

/* 02271BD0 */
void daNpc_Km1_c::playTexPatternAnm() {
    WWHD_FUNC(0x02271BD0, void, this);
    if (field_0x7CD == 0 && cLib_calcTimer(&field_0x6F2) != 0) {
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
VERIFY(0x02271BD0, &daNpc_Km1_c::playTexPatternAnm);

/* 02272A34 */
s32 daNpc_Km1_c::setAnm_anm(anm_prm_c* i_anm_ptr) {
    WWHD_FUNC(0x02272A34, s32, this, i_anm_ptr);
    s32 uVar2 = 0;
    s8 num = i_anm_ptr->bckNum;
    if (num >= 0 && field_0x7CE != num) {
        field_0x7CE = num;
        if (mpMorf.get() != nullptr) {
            gabi::Local<be<s32>> local_18;
            gabi::Local<be<s32>> local_14;
            anmResID(num, local_18, local_14);
            if (*local_18 >= 0) {
                dNpc_setAnmIDRes(mpMorf, i_anm_ptr->loopMode, i_anm_ptr->morf, i_anm_ptr->speed, *local_18, *local_14, STR(0x1001C7EC) /* "Km" */);
            }
            uVar2 = 1;
        }
        field_0x7B5 = 0;
        field_0x7A4 = 0.0f;
        field_0x7B4 = 0;
    }
    return uVar2;
}
VERIFY(0x02272A34, &daNpc_Km1_c::setAnm_anm);

/* 02272AF0 */
void daNpc_Km1_c::setAnm() {
    WWHD_FUNC(0x02272AF0, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BF6C0); /* [3] */
    setAnm_tex(a_anm_prm_tbl[field_0x7D0].btpNum);
    setAnm_anm(&a_anm_prm_tbl[field_0x7D0]);
}
VERIFY(0x02272AF0, &daNpc_Km1_c::setAnm);

/* 02272BC0 (unnamed by the matcher) */
void daNpc_Km1_c::chngAnmAtr(u32 param_1) {
    WWHD_FUNC(0x02272BC0, void, this, param_1);
    if (param_1 < 1 && param_1 != field_0x7CB) {
        field_0x7CB = param_1;
        setAnm_ATR(1);
    }
}
VERIFY(0x02272BC0, &daNpc_Km1_c::chngAnmAtr);

/* 02272B5C */
void daNpc_Km1_c::setAnm_ATR(int param_1) {
    WWHD_FUNC(0x02272B5C, void, this, param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BF6F0); /* [1] */
    if (param_1 != 0) {
        /* HD: the one-entry table is indexed with 0 here */
        setAnm_tex(a_anm_prm_tbl[0].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[field_0x7CB]);
}
VERIFY(0x02272B5C, &daNpc_Km1_c::setAnm_ATR);

/* 02272BE4 */
void daNpc_Km1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x02272BE4, void, this, i_msgStatus);
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
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x02272BE4, &daNpc_Km1_c::anmAtr);

/* 02272CA4 */
void daNpc_Km1_c::setStt(s8 param_1) {
    WWHD_FUNC(0x02272CA4, void, this, param_1);
    s8 uVar1 = field_0x7D0;
    field_0x7D0 = param_1;
    if (param_1 == 2) {
        field_0x7D1 = uVar1;
        field_0x7CB = 0xFF;
        field_0x7D2 = 1;
        return;
    }
    setAnm();
}
VERIFY(0x02272CA4, &daNpc_Km1_c::setStt);

/* 02272CD4 (unnamed by the matcher) */
u16 daNpc_Km1_c::next_msgStatus(be<u32>*) {
    WWHD_FUNC(0x02272CD4, u16, this, (be<u32>*)nullptr);
    return 0x10; /* fopMsgStts_MSG_ENDS_e */
}
VERIFY(0x02272CD4, &daNpc_Km1_c::next_msgStatus);

/* 02272CDC (unnamed by the matcher) */
u32 daNpc_Km1_c::getMsg() {
    WWHD_FUNC(0x02272CDC, u32, this);
    return 0;
}
VERIFY(0x02272CDC, &daNpc_Km1_c::getMsg);

/* 02272668 (unnamed by the matcher) */
void daNpc_Km1_c::eventOrder() {
    WWHD_FUNC(0x02272668, void, this);
    if (field_0x7CF == 1 || field_0x7CF == 2) {
        s8 v = field_0x7CF;
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (v == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x02272668, &daNpc_Km1_c::eventOrder);

/* 0227218C */
void daNpc_Km1_c::checkOrder() {
    WWHD_FUNC(0x0227218C, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        return;
    }
    if (command != 1 /* dEvtCmd_INTALK_e */) {
        return;
    }
    if (field_0x7CF != 1 && field_0x7CF != 2) {
        return;
    }
    field_0x7CF = 0;
    field_0x7C5 = 1;
}
VERIFY(0x0227218C, &daNpc_Km1_c::checkOrder);

/* 02272424 */
void daNpc_Km1_c::lookBack() {
    WWHD_FUNC(0x02272424, void, this);
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
VERIFY(0x02272424, &daNpc_Km1_c::lookBack);

/* 02272CE4 (unnamed by the matcher) */
u8 daNpc_Km1_c::chkAttention() {
    WWHD_FUNC(0x02272CE4, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02272CE4, &daNpc_Km1_c::chkAttention);

/* 02271CBC */
void daNpc_Km1_c::setAttention() {
    WWHD_FUNC(0x02271CBC, void, this);
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
VERIFY(0x02271CBC, &daNpc_Km1_c::setAttention);

/* 02271A7C */
bool daNpc_Km1_c::decideType(int i_type) {
    WWHD_FUNC(0x02271A7C, bool, this, i_type);
    field_0x7D3 = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_KM1_e) {
        field_0x7D4 = 0;
        field_0x7D3 = 0;
    }
    return true;
}
VERIFY(0x02271A7C, &daNpc_Km1_c::decideType);

/* 022722A4 */
void daNpc_Km1_c::event_actionInit(int param_1) {
    WWHD_FUNC(0x022722A4, void, this, param_1);
    be<s32>* puVar1 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001C784) /* "ActNo" */);
    dComIfGp_evmng_getMyIntegerP(param_1, STR(0x1001C78C) /* "Timer" */);
    if (puVar1 != nullptr) {
        field_0x7CA = (s8)(s32)*puVar1;
    }
}
VERIFY(0x022722A4, &daNpc_Km1_c::event_actionInit);

/* 0227232C (unnamed by the matcher) */
bool daNpc_Km1_c::event_action() {
    WWHD_FUNC(0x0227232C, bool, this);
    return true;
}
VERIFY(0x0227232C, &daNpc_Km1_c::event_action);

/* 02272334 */
void daNpc_Km1_c::privateCut() {
    WWHD_FUNC(0x02272334, void, this);
    /* cut_name_tbl (.data 0x101BF6BC): "ACTION" */
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001C794) /* "Km1" */, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    field_0x7C9 = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BF6BC, 1, TRUE, 0);
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
VERIFY(0x02272334, &daNpc_Km1_c::privateCut);

/* 02272D6C */
void daNpc_Km1_c::endEvent() {
    WWHD_FUNC(0x02272D6C, void, this);
    dComIfGp_event_reset();
    field_0x7CB = 0xFF;
}
VERIFY(0x02272D6C, &daNpc_Km1_c::endEvent);

/* 02272608 */
void daNpc_Km1_c::event_proc() {
    WWHD_FUNC(0x02272608, void, this);
    if (!mEventCut.cutProc()) {
        privateCut();
    }
    lookBack();
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
VERIFY(0x02272608, &daNpc_Km1_c::event_proc);

/* 02271AA4 (unnamed by the matcher) */
bool daNpc_Km1_c::set_action(ProcFunc_l* i_action, void* param_2) {
    WWHD_FUNC(0x02271AA4, bool, this, i_action, param_2);
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
VERIFY(0x02271AA4, &daNpc_Km1_c::set_action);

/* 02272DAC */
BOOL daNpc_Km1_c::wait01() {
    WWHD_FUNC(0x02272DAC, BOOL, this);
    if (field_0x7C5 != 0) {
        field_0x7B7 = 0xFF;
        if (dComIfGp_event_chkTalkXY()) {
            if (dComIfGp_evmng_ChkPresentEnd() == 0) {
                return TRUE;
            }
            field_0x7B7 = dComIfGp_event_getPreItemNo();
        }
        setStt(2);
    } else {
        field_0x7CF = 2;
        if (field_0x7C4) {
            field_0x7D2 = 1;
        } else {
            s16 y = field_0x76C.y;
            field_0x7D2 = 3;
            field_0x7B2 = y;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
    }
    return TRUE;
}
VERIFY(0x02272DAC, &daNpc_Km1_c::wait01);

/* 02272E6C */
BOOL daNpc_Km1_c::talk01() {
    WWHD_FUNC(0x02272E6C, BOOL, this);
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            field_0x7B7 = 0xFF;
            setStt(field_0x7D1);
            field_0x7C8 = 0;
            field_0x7C5 = 0;
            endEvent();
        }
    }
    return TRUE;
}
VERIFY(0x02272E6C, &daNpc_Km1_c::talk01);

/* 02272EEC */
BOOL daNpc_Km1_c::wait_action1(void*) {
    WWHD_FUNC(0x02272EEC, BOOL, this, (void*)nullptr);
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
VERIFY(0x02272EEC, &daNpc_Km1_c::wait_action1);

/* 022721CC */
u8 daNpc_Km1_c::demo() {
    WWHD_FUNC(0x022721CC, u8, this);
    if (demoActorID == 0) {
        if (field_0x7C7 != 0) {
            field_0x7C7 = 0;
        }
    } else {
        u8 id = demoActorID;
        field_0x7C7 = 1;
        /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check; result unused) */
        if (id != 0 && id <= 0x20) {
            u32 obj = gabi::load<u32>(0x101D5FFC);
            if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
                JUT_ASSERT_fail(STR(0x1001C65C), 0x23A, STR(0x1001C64C));
                obj = gabi::load<u32>(0x101D5FFC);
            }
            gabi::call<void*>(0x02526E70, obj, id);
        }
        dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x1001C77F) /* "Km" */, 0, nullptr, 0, 0);
    }
    return field_0x7C7;
}
VERIFY(0x022721CC, &daNpc_Km1_c::demo);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 02272858 */
BOOL daNpc_Km1_c::_draw() {
    WWHD_FUNC(0x02272858, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(model);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove() */
    /* HD: no shadow model, no real/simple shadow */
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO().mPrmTbl.field_18) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001C5F8);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001C5FC);
    }
    dSnap_RegistFig(0x58 /* DSNAP_TYPE_NPC_KM1_KK1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02272858, &daNpc_Km1_c::_draw);

/* 022726A0 */
BOOL daNpc_Km1_c::_execute() {
    WWHD_FUNC(0x022726A0, BOOL, this);
    if (field_0x7B8 == 0) {
        field_0x774.copy(current.pos);
        field_0x76C.x = current.angle.x;
        field_0x76C.y = current.angle.y;
        field_0x76C.z = current.angle.z;
        field_0x7B8 = 1;
    }
    daNpc_Km1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
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
    setCollision(60.0f, 150.0f);
    return true;
}
VERIFY(0x022726A0, &daNpc_Km1_c::_execute);

/* 02272110 */
BOOL daNpc_Km1_c::_delete() {
    WWHD_FUNC(0x02272110, BOOL, this);
    /* HD: no fopAcM_RegisterDeleteID; dComIfG_resDelete instead of resDeleteDemo */
    dComIfG_resDelete(&mPhs, STR(0x1001C77C) /* "Km" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    if (l_HIO().field_0x8 >= 0) {
        s32 n = l_HIO().field_0x8 - 1;
        l_HIO().field_0x8 = n;
        if (n < 0) {
            mDoHIO_deleteChild(l_HIO().mNo);
        }
    }
    return true;
}
VERIFY(0x02272110, &daNpc_Km1_c::_delete);

/* 02271A78 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* actor) {
    WWHD_FUNC(0x02271A78, BOOL, actor);
    return ((daNpc_Km1_c*)actor)->CreateHeap();
}
VERIFY(0x02271A78, CheckCreateHeap);

/* 02271F84 */
cPhs_State daNpc_Km1_c::_create() {
    WWHD_FUNC(0x02271F84, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Km1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);          /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KM1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);       /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut);    /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State resLoadResult = dComIfG_resLoad(&mPhs, STR(0x1001C768) /* "Km" */);
    if (resLoadResult != cPhs_COMPLEATE_e) {
        return resLoadResult;
    }
    if (l_HIO().field_0x8 < 0) {
        l_HIO().mNo = mDoHIO_createChild(STR(0x1001C76C) /* "金持ちム−ル" */, &l_HIO());
    }
    l_HIO().field_0x8 = l_HIO().field_0x8 + 1;
    /* a_heap_size_tbl (.data 0x101BF6B8; HD: always entry 0) */
    if (!fopAcM_entrySolidHeap(this, 0x02271A78 /* CheckCreateHeap */, gabi::load<u32>(0x101BF6B8))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -80.0f, 60.0f, 160.0f, 60.0f);
    if (createInit() == 0) {
        resLoadResult = cPhs_ERROR_e;
    }
    return resLoadResult;
}
VERIFY(0x02271F84, &daNpc_Km1_c::_create);

/* 022717D0 */
BOOL daNpc_Km1_c::CreateHeap() {
    WWHD_FUNC(0x022717D0, BOOL, this);
    J3DModelData* a_mdl_data = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001C6E8) /* "Km" */, 2 /* dRes_ID_KM_BDL_KM_e */);
    if (a_mdl_data == nullptr) /* JUT_ASSERT(1335, a_mdl_data != NULL) */
        JUT_ASSERT_fail(STR(0x1001C6F4), 0x537, STR(0x1001C730));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001C6E8), 1 /* dRes_ID_KM_BCK_KM_WAIT01_e */);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_data, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() != nullptr) {
        m_head_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_data), STR(0x1001C6EC) /* "head" */);
        if (m_head_jnt_num < 0)
            JUT_ASSERT_fail(STR(0x1001C6F4), 0x54B, STR(0x1001C71C));
        m_backbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_data), STR(0x1001C744) /* "backbone" */);
        if (m_backbone_jnt_num < 0)
            JUT_ASSERT_fail(STR(0x1001C6F4), 0x54D, STR(0x1001C704));
        field_0x7CD = gabi::load<s8>(0x101BF6B4); /* a_tex_pattern_num_tbl[0] (HD: always entry 0) */
        if (initTexPatternAnm(false) != 0) {
            /* HD: no shadow model */
            for (u16 i = 0; i < J3DModelData_getJointNum(a_mdl_data); i++) {
                if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
                    /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Km) */
                    J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
                    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
                    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
                    if ((u32)i < n)
                        joint += i * 0x1C;
                    gabi::store<u32>(joint + 8, 0x022713C0);
                }
            }
            gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
            mAcchCir.SetWall(30.0f, 60.0f);
            mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
            return TRUE;
        }
    }
    mpMorf = nullptr;
    return FALSE;
}
VERIFY(0x022717D0, &daNpc_Km1_c::CreateHeap);

/* 0227210C */
static cPhs_State daNpc_Km1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0227210C, cPhs_State, i_this);
    return ((daNpc_Km1_c*)i_this)->_create();
}
VERIFY(0x0227210C, daNpc_Km1_Create);

/* 02272188 */
static BOOL daNpc_Km1_Delete(daNpc_Km1_c* i_this) {
    WWHD_FUNC(0x02272188, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02272188, daNpc_Km1_Delete);

/* 02272854 */
static BOOL daNpc_Km1_Execute(daNpc_Km1_c* i_this) {
    WWHD_FUNC(0x02272854, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02272854, daNpc_Km1_Execute);

/* 02272974 */
static BOOL daNpc_Km1_Draw(daNpc_Km1_c* i_this) {
    WWHD_FUNC(0x02272974, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02272974, daNpc_Km1_Draw);

/* 02272978 */
static BOOL daNpc_Km1_IsDelete(daNpc_Km1_c*) {
    WWHD_FUNC(0x02272978, BOOL, (daNpc_Km1_c*)nullptr);
    return TRUE;
}
VERIFY(0x02272978, daNpc_Km1_IsDelete);

/* 0227301C: static initialisation of the translation unit */
static void __sinit_d_a_npc_km1_cpp() {
    WWHD_FUNC(0x0227301C, void);
    sinit_header_statics_z(0x1046774C, 0x101BF71C, 0x10467780);
    daNpc_Km1_HIO_c_ct(&l_HIO()); /* static daNpc_Km1_HIO_c l_HIO */
}
VERIFY(0x0227301C, __sinit_d_a_npc_km1_cpp);

/* 022730BC: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001C604 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022730BC, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022730BC, SafeString_dt);

/* 022730D0: daNpc_Km1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Km1_c_dt(daNpc_Km1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022730D0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001C61C);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001C62C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022730D0, daNpc_Km1_c_dt);

/* 0227316C: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0227316C, void, (SafeString*)nullptr);
}
VERIFY(0x0227316C, SafeString_assureTerminationImpl);
