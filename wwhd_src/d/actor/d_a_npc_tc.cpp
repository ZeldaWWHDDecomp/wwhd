/**
 * d_a_npc_tc.cpp (WWHD)
 * NPC - Tingle, Ankle, David Jr., Knuckle
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_tc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_tc.h"

#define SAFESTRING_VTBL 0x100226F4 /* this TU's sead::SafeString vtable */
#define TC_VTBL 0x10022D20         /* daNpc_Tc_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* J3DAnmTexPattern/J3DAnmBase::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out-of-line copy of another TU) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline BOOL fopAcM_orderOtherEvent2(fopAc_ac_c* a, u32 name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D77DC, a, name, flag, hind);
}
/* 022ED834 daObj::PrmAbstract<daObjSmplbg::Act_c::Prm_e> (this TU's copy) */
static u32 PrmAbstract_Smplbg(const fopAc_ac_c* a, s32 w, s32 s);
/* 02520864 dComIfGs_isStageTbox(stageNo, no) */
static inline BOOL dComIfGs_isStageTbox(s32 stageNo, s32 no) { return gabi::call<BOOL>(0x02520864, stageNo, no); }
/* save info event flags: *(0x101F84DC) + 0x644 */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* 025C109C dStage_searchName(name) -> dStage_objectNameInf* {char name[8]; s16 procname +8; s8 argument +0xA} */
static inline u32 dStage_searchName(const char* name) { return gabi::call<u32>(0x025C109C, name); }
/* fopAcM_GetProfName: HD null check (0x7FFF) */
static inline s16 fopAcM_GetProfName(void* a) { return a != nullptr ? gabi::load<s16>(gabi::ea(a) + 0xE) : (s16)0x7FFF; }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* 0254457C dEvent_manager_c::endCheckOld(name) */
static inline BOOL dComIfGp_evmng_endCheck(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* 025881A4 dLib_checkPlayerInCircle(cXyz center (copy), f32 radius, f32 height) */
static inline BOOL dLib_checkPlayerInCircle(cXyz* center, f32 r, f32 h) { return gabi::call<BOOL>(0x025881A4, center, r, h); }
/* 0200F268 cLib_addCalcPosXZ2(pos, const cXyz& target, scale, maxStep) */
static inline void cLib_addCalcPosXZ2(cXyz* pos, cXyz* target, f32 scale, f32 maxStep) { gabi::call(0x0200F268, pos, target, scale, maxStep); }
/* (a - b).absXZ(): cXyz::operator- into a temporary, then |(x, 0, z)| through PSVECSquareMag and sqrt */
static inline f32 diffAbsXZ(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* GHS pointer to member function call */
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
/* daNpc_Tc_c::setAction(func, NULL) (inline): func is a non-virtual member {0, -1, func} */
static inline BOOL setAction_l(daNpc_Tc_c* t, u32 func) {
    ProcFunc_l* cur = &t->mCurrActionFunc;
    s16 i = cur->i;
    if (i == -1) {
        if (cur->d == 0 && cur->f == func)
            return TRUE;
    } else if (i == 0) {
        goto set_new;
    }
    t->mActionStatus = -1; /* ACTION_ENDING */
    pmf_call(t, cur, nullptr);
set_new:
    cur->d = 0;
    cur->i = -1;
    cur->f = func;
    t->mActionStatus = 0; /* ACTION_STARTING */
    pmf_call(t, cur, nullptr);
    return TRUE;
}
/* 025BEBB8 dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_l(u8 type, fopAc_ac_c* a, cXyz* pos, s16 angleY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angleY, x, y, z);
}
/* the tower (daObjSmplbg::Act_c): stop flag at +0x3F0 */
static inline BOOL tower_isStop(fopAc_ac_c* t) { return gabi::load<u8>(gabi::ea(t) + 0x3F0); }
static inline void tower_onStop(fopAc_ac_c* t) { gabi::store<u8>(gabi::ea(t) + 0x3F0, 1); }
static inline void tower_offStop(fopAc_ac_c* t) { gabi::store<u8>(gabi::ea(t) + 0x3F0, 0); }

/* ---- file statics ---- */
/* daNpc_Tc_HIO_c (0xF4; vtable at +0, dNpc_HIO_c mNpc at +4 with its HD vtable last at +0x28) */
struct daNpc_Tc_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<s16> mMaxHeadX;
    /* 0x0A */ be<s16> mMaxBackboneX;
    /* 0x0C */ be<s16> mMaxHeadY;
    /* 0x0E */ be<s16> mMaxBackboneY;
    /* 0x10 */ be<s16> mMinHeadX;
    /* 0x12 */ be<s16> mMinBackboneX;
    /* 0x14 */ be<s16> mMinHeadY;
    /* 0x16 */ be<s16> mMinBackboneY;
    /* 0x18 */ be<s16> mMaxTurnStep;
    /* 0x1A */ be<s16> mMaxHeadTurnVel;
    /* 0x1C */ be<f32> mAttnYOffset;
    /* 0x20 */ be<s16> mMaxAttnAngleY;
    /* 0x22 */ be<u8> m22;
    /* 0x23 */ u8 _23;
    /* 0x24 */ be<f32> mMaxAttnDistXZ;
    /* 0x28 */ be<u32> mNpc_vtbl;
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<u8> field_0x34;
    /* 0x35 */ u8 _35[3];
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<u8> field_0x44;
    /* 0x45 */ u8 _45[3];
    /* 0x48 */ be<f32> field_0x48;
    /* 0x4C */ be<f32> field_0x4C;
    /* 0x50 */ be<f32> field_0x50;
    /* 0x54 */ be<f32> field_0x54;
    /* 0x58 */ be<f32> field_0x58;
    /* 0x5C */ be<f32> field_0x5C;
    /* 0x60 */ cXyz field_0x60;
    /* 0x6C */ cXyz field_0x6C;
    /* 0x78 */ be<s16> field_0x78;
    /* 0x7A */ be<s16> field_0x7A;
    /* 0x7C */ be<u8> field_0x7C;
    /* 0x7D */ be<u8> field_0x7D;
    /* 0x7E */ be<u8> field_0x7E;
    /* 0x7F */ u8 field_0x7F[8];
    /* 0x87 */ u8 field_0x87[8];
    /* 0x8F */ u8 field_0x8F[5];
    /* 0x94 */ be<f32> field_0x94[24];
};
WWHD_SIZE(daNpc_Tc_HIO_c, 0xF4);
static daNpc_Tc_HIO_c& l_HIO() { return *gabi::at<daNpc_Tc_HIO_c>(0x104687F4); }

/* 022E7B28 */
void daNpc_Tc_c::getArg() {
    WWHD_FUNC(0x022E7B28, void, this);
    mType = fopAcM_GetParam(this) & 0xf;
    if (mType != 0xF) {
        return;
    }
    mType = TYPE_NORMAL;
}
VERIFY(0x022E7B28, &daNpc_Tc_c::getArg);

/* 022ED4A4 daNpc_Tc_HIO_c::daNpc_Tc_HIO_c (HD: allocates when this == NULL; the debug copies of
 * event bits, collection maps and treasure chests (field_0x7C..0x93) are not read) */
static daNpc_Tc_HIO_c* daNpc_Tc_HIO_c_ct(daNpc_Tc_HIO_c* i_this) {
    WWHD_FUNC(0x022ED4A4, daNpc_Tc_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Tc_HIO_c*)operator_new(0xF4);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1002272C;
    gabi::call(0x0259DA18, gabi::ea(i_this) + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    i_this->field_0x60.set(-100.0f, 1700.0f, -50.0f);
    i_this->field_0x6C.set(100.0f, 1700.0f, 50.0f);
    i_this->field_0x7A = 0;
    i_this->field_0x78 = -0x8000;
    i_this->m04 = -20.0f;
    i_this->mMaxHeadX = 0x1FFE;
    i_this->mMaxHeadY = 10000;
    i_this->mMaxBackboneX = 0;
    i_this->mMaxBackboneY = 0x1C70;
    i_this->mMinHeadX = -0x9C4;
    i_this->mMinHeadY = -10000;
    i_this->mMinBackboneX = 0;
    i_this->mMinBackboneY = -0x1C70;
    i_this->mMaxTurnStep = 3000;
    i_this->mMaxHeadTurnVel = 0x800;
    i_this->mAttnYOffset = 65.0f;
    i_this->mMaxAttnAngleY = 0x4000;
    i_this->m22 = 0;
    i_this->mMaxAttnDistXZ = 400.0f;
    i_this->field_0x2C = 900.0f;
    i_this->field_0x30 = 400.0f;
    i_this->field_0x34 = 0;
    i_this->field_0x38 = 20.0f;
    i_this->field_0x40 = 14.0f;
    i_this->field_0x3C = -2.5f;
    i_this->field_0x48 = -25.0f;
    i_this->field_0x50 = 8.0f;
    i_this->field_0x4C = -2.5f;
    i_this->field_0x54 = 0.35f;
    i_this->field_0x58 = 4.0f;
    i_this->field_0x5C = 8.0f;
    static const float a_0x94[24] = {0.0f, 8.0f, 8.0f, 5.0f, 8.0f, 4.0f, 0.0f, 6.0f, 6.0f, 0.0f, 8.0f, 8.0f,
                                     8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
    for (int i = 0; i < 24; i++)
        i_this->field_0x94[i] = a_0x94[i];
    return i_this;
}
VERIFY(0x022ED4A4, daNpc_Tc_HIO_c_ct);

/* 022E7814 */
fopAc_ac_c* daNpc_Tc_c::_searchTower(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x022E7814, fopAc_ac_c*, this, i_actor);
    /* HD: fopAcM_GetName checks the actor for NULL */
    if (fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == 0x56 /* fpcNm_Obj_Smplbg_e */ &&
        PrmAbstract_Smplbg(i_actor, 8, 0) == 0 /* prm_get_type() */) {
        return i_actor;
    }
    return nullptr;
}
VERIFY(0x022E7814, &daNpc_Tc_c::_searchTower);

/* 022E7880 searchTower_CB */
static void* searchTower_CB(void* i_actor, void* i_this) {
    WWHD_FUNC(0x022E7880, void*, i_actor, i_this);
    return ((daNpc_Tc_c*)i_this)->_searchTower((fopAc_ac_c*)i_actor);
}
VERIFY(0x022E7880, searchTower_CB);

/* 022E7890 */
BOOL daNpc_Tc_c::initTexPatternAnm(u32 param_1) { /* bool, passed on unnormalised */
    WWHD_FUNC(0x022E7890, BOOL, this, param_1);
    J3DModelData* modeldata = J3DModel_getModelData_l(mpMorf->getModel());
    /* HD: l_btp_ix_tbl has one entry, read without the mTexPatternNum index */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1002274C) /* "Tc" */, gabi::load<s32>(0x100226E0), SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(0x192, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x1002276C), 0x192, STR(0x10022750));
    if (mDoExt_btpAnm_init(mBtpAnm, modeldata, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE) == FALSE) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022E7890, &daNpc_Tc_c::initTexPatternAnm);

/* 022E8FEC */
void daNpc_Tc_c::playTexPatternAnm() {
    WWHD_FUNC(0x022E8FEC, void, this);
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= max) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = (u8)(mBlinkFrame + 1);
        }
    }
}
VERIFY(0x022E8FEC, &daNpc_Tc_c::playTexPatternAnm);

/* 022E7CAC */
u32 daNpc_Tc_c::setTexAnm() {
    WWHD_FUNC(0x022E7CAC, u32, this);
    s8 texPatternNum = gabi::load<s8>(0x101C680C + (s8)mTexPatternNumIdx); /* a_tex_pattern_num_tbl {-1, 0} */
    if (texPatternNum != mTexPatternNum && texPatternNum != -1) {
        mTexPatternNum = texPatternNum;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022E7CAC, &daNpc_Tc_c::setTexAnm);

/* 022EA648 (cXyz by value: pointer to a copy) */
u8 daNpc_Tc_c::chkAttention(cXyz* param_1, s16 param_2) {
    WWHD_FUNC(0x022EA648, u8, this, param_1, param_2);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 maxAttnDist = l_HIO().mMaxAttnDistXZ;
    switch (mType) {
    case TYPE_RED:
    case TYPE_WHITE:
        maxAttnDist = l_HIO().mMaxAttnDistXZ;
        maxAttnDist *= 0.5f;
        break;
    }
    if (mStatus == STATUS_WALK_TO_JAIL || mStatus == STATUS_JUMP || mStatus == STATUS_SIT) {
        maxAttnDist = l_HIO().field_0x2C;
    }
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    s32 maxAttnAngleY = l_HIO().mMaxAttnAngleY;
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 angle = cM_atan2s(dx, dz);
    if (mHasAttention) {
        maxAttnDist += 40.0f;
        maxAttnAngleY += 0x71C;
    }
    angle -= param_2;
    s32 a = angle < 0 ? -angle : angle;
    return maxAttnAngleY > a && maxAttnDist > dist;
}
VERIFY(0x022EA648, &daNpc_Tc_c::chkAttention);

/* 022EA0B4 */
void daNpc_Tc_c::eventOrder() {
    WWHD_FUNC(0x022EA0B4, void, this);
    u32 a_demo_name_tbl = 0x101C6834; /* "TC_JUMP_DEMO", "TC_RESCUE", ... (7) */
    if (mEventIdx == 1 || mEventIdx == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mEventIdx == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (mEventIdx == 8 || mEventIdx == 9) {
        fopAcM_orderOtherEvent2(this, gabi::load<u32>(a_demo_name_tbl + (mEventIdx - 3) * 4), 1, 0x14F);
    } else if (mEventIdx >= 3) {
        fopAcM_orderOtherEvent2(this, gabi::load<u32>(a_demo_name_tbl + (mEventIdx - 3) * 4), 1, 0xFFFF);
    }
}
VERIFY(0x022EA0B4, &daNpc_Tc_c::eventOrder);

/* 022E90B0 */
void daNpc_Tc_c::checkOrder() {
    WWHD_FUNC(0x022E90B0, void, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mEventIdx = 0;
        return;
    }
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* eventInfo.checkCommandTalk() */) {
        return;
    }
    if (mEventIdx != 1 && mEventIdx != 2) {
        return;
    }
    mEventIdx = 0;
    mTalkingNearJail = true;
}
VERIFY(0x022E90B0, &daNpc_Tc_c::checkOrder);

/* 022EB970 */
void daNpc_Tc_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x022EB970, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: { /* fopMsgStts_MSG_TYPING_e */
        if (field_0x6C8) {
            return;
        }
        u8 msgAnmAtr = dComIfGp_getMesgAnimeAttrInfo();
        field_0x6C8 = TRUE;
        if (msgAnmAtr >= 0x10) /* HD: JUT_ASSERT(0x3E5, msgAnmAtr < ARRAY_SIZE(anm_atr)) */
            JUT_ASSERT_fail(STR(0x10022B98), 0x3E5, STR(0x10022BA8));
        s8 atr = gabi::load<s8>(0x10022B88 + msgAnmAtr); /* anm_atr */
        if (atr == 16 /* ANM_PRM_IDX_HAPPY2 */ && mAnmPrmIdx == 4 /* ANM_PRM_IDX_TALK01 */) {
            return;
        }
        mAnmPrmIdx = atr;
        break;
    }
    case 14: /* fopMsgStts_MSG_DISPLAYED_e */
        field_0x6C8 = FALSE;
        break;
    }
}
VERIFY(0x022EB970, &daNpc_Tc_c::anmAtr);

/* 022ED834 daObj::PrmAbstract<daObjSmplbg::Act_c::Prm_e> */
static u32 PrmAbstract_Smplbg(const fopAc_ac_c* a, s32 w, s32 s) {
    WWHD_FUNC(0x022ED834, u32, a, w, s);
    /* slw/srw: shift counts 32..63 give 0 */
    u32 sw = (u32)w & 0x3F, ss = (u32)s & 0x3F;
    u32 mask = (sw >= 32 ? 0u : (1u << sw)) - 1;
    u32 prm = gabi::load<u32>(gabi::ea(a) + 0xB0);
    return (ss >= 32 ? 0u : (prm >> ss)) & mask;
}
VERIFY(0x022ED834, PrmAbstract_Smplbg);

/* 022ED850 cLib_calcTimer<s8> (this TU's copy) */
static u8 cLib_calcTimer_c(be<u8>* t) {
    WWHD_FUNC(0x022ED850, u8, t);
    u8 v = *t;
    if (v != 0) {
        v = (u8)(v - 1);
        *t = v;
    }
    return v;
}
VERIFY(0x022ED850, cLib_calcTimer_c);

/* 022ED78C */
static BOOL daNpc_Tc_IsDelete(daNpc_Tc_c*) {
    WWHD_FUNC(0x022ED78C, BOOL, (daNpc_Tc_c*)nullptr);
    return TRUE;
}
VERIFY(0x022ED78C, daNpc_Tc_IsDelete);

/* 022ED778: daNpc_Tc_HIO_c deleting destructor (nothing to destroy) */
static void daNpc_Tc_HIO_c_dt(daNpc_Tc_HIO_c* i_this, s32 flags) {
    WWHD_FUNC(0x022ED778, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022ED778, daNpc_Tc_HIO_c_dt);

/* 022ED794: daNpc_Tc_c deleting destructor (compiler-generated, HD virtual destructor): only the
 * fopNpc_npc_c members have destructors */
static void daNpc_Tc_c_dt(daNpc_Tc_c* i_this, s32 flags) {
    WWHD_FUNC(0x022ED794, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1002270C);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1002271C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);             /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022ED794, daNpc_Tc_c_dt);

/* 022ED830: empty virtual of this TU (sead::SafeString::assureTerminationImpl_) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022ED830, void, (void*)nullptr);
}
VERIFY(0x022ED830, SafeString_assureTerminationImpl);

/* 022ED6A8: static initialisation of the translation unit */
static void __sinit_d_a_npc_tc_cpp() {
    WWHD_FUNC(0x022ED6A8, void);
    /* the header statics (see bindings.h sinit_header_statics), here with the objects at
     * 0x104687C8/C9 and the zeroed object at 0x104687CC */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104687CC + 4 * i, 0);
    __register_global_object(0x101C6850);
    gabi::store<f32>(0x104687B0, -3.1415927f);
    gabi::store<f32>(0x104687B4, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104687C8);
    __register_global_object(0x101C685C);
    gabi::call(0x028EAB2C, 0x104687C9);
    __register_global_object(0x101C6868);
    /* four header floats {50000, 50000, 10000, 10000} */
    gabi::store<f32>(0x104687B8, 50000.0f);
    gabi::store<f32>(0x104687C4, 10000.0f);
    gabi::store<f32>(0x104687BC, 50000.0f);
    gabi::store<f32>(0x104687C0, 10000.0f);
    daNpc_Tc_HIO_c_ct(&l_HIO()); /* static daNpc_Tc_HIO_c l_HIO */
}
VERIFY(0x022ED6A8, __sinit_d_a_npc_tc_cpp);

/* 022E7588 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022E7588, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Tc_c* i_this = gabi::at<daNpc_Tc_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (i_this != nullptr) {
            u32 jntNo = jntNo_of(node);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(getAnmMtx(model, jntNo), stk);
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                /* static cXyz l_offsetAttPos(0, 0, 0): guard 0x104688E8, object 0x104687DC */
                cXyz* l_offsetAttPos = gabi::at<cXyz>(0x104687DC);
                if (gabi::load<u32>(0x104688E8) == 0) {
                    l_offsetAttPos->x = 0.0f;
                    l_offsetAttPos->z = 0.0f;
                    gabi::store<u32>(0x104688E8, 1);
                    l_offsetAttPos->y = 0.0f;
                }
                /* static cXyz l_offsetEyePos(24, -16, 0): guard 0x104688EC, object 0x104687E8 */
                cXyz* l_offsetEyePos = gabi::at<cXyz>(0x104687E8);
                if (gabi::load<u32>(0x104688EC) == 0) {
                    gabi::store<u32>(0x104688EC, 1);
                    l_offsetEyePos->x = 24.0f;
                    l_offsetEyePos->z = 0.0f;
                    l_offsetEyePos->y = -16.0f;
                }
                PSMTXMultVec(stk, l_offsetAttPos, &i_this->mAttPos);
                mDoMtx_YrotM(stk, (s16)-i_this->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, (s16)-i_this->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, l_offsetEyePos, &i_this->mEyePos);
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = (u8)(i_this->mAttnSetCount + 1);
                }
            } else if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                mDoMtx_XrotM(stk, i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk, i_this->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(stk, j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), stk); /* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
        }
    }
    return TRUE;
}
VERIFY(0x022E7588, nodeCallBack);

/* 022E7994 */
BOOL daNpc_Tc_c::_createHeap() {
    WWHD_FUNC(0x022E7994, BOOL, this);
    /* HD: the model depends on the type (index table 0x10022780) */
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1002277C) /* "Tc" */, gabi::load<s32>(0x10022780 + mType * 4), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xA76, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10022794), 0xA76, STR(0x100227A4));
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1 /* EMode_NULL */, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                    0x15021222);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = 2;     /* TC_JNT_HEAD_e (HD: no asserts) */
    m_jnt.mBackboneJntNum = 1; /* TC_JNT_BACKBONE_e */
    setJointCallBack(modelData, 2, 0x022E7588 /* nodeCallBack */);
    setJointCallBack(modelData, 1, 0x022E7588);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mTexPatternNum = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x022E7994, &daNpc_Tc_c::_createHeap);

/* 022E7B24 createHeap_CB (tail call: _createHeap's result register is passed through) */
static u32 createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022E7B24, u32, i_this);
    return gabi::call<u32>(0x022E7994, i_this); /* static_cast<daNpc_Tc_c*>(i_this)->_createHeap() */
}
VERIFY(0x022E7B24, createHeap_CB);

/* 022E7B4C */
bool daNpc_Tc_c::isCreate() {
    WWHD_FUNC(0x022E7B4C, bool, this);
    switch (mType) {
    case TYPE_BLUE:
        /* HD: all five Tingle statues' chests (GameCube: event bit 0x1708) */
        if (!dComIfGs_isStageTbox(3, 0xF) || !dComIfGs_isStageTbox(4, 0xF) || !dComIfGs_isStageTbox(5, 0xF) ||
            !dComIfGs_isStageTbox(7, 0xF) || !dComIfGs_isStageTbox(6, 0xF)) {
            return false;
        }
        break;
    case TYPE_NORMAL2:
        if (!dComIfGs_isEventBit(0x0B80)) {
            return false;
        }
        break;
    }
    return true;
}
VERIFY(0x022E7B4C, &daNpc_Tc_c::isCreate);

/* 022E7C14 */
void* daNpc_Tc_c::searchStoolPos(void* i_actor, void* i_this) {
    WWHD_FUNC(0x022E7C14, void*, i_actor, i_this);
    fopAc_ac_c* actor = (fopAc_ac_c*)i_actor;
    daNpc_Tc_c* a_this = (daNpc_Tc_c*)i_this;
    u32 inf = dStage_searchName(STR(0x100227B8) /* "Ostool" */);
    if (inf == 0) {
        return nullptr;
    }
    if (gabi::load<s16>(inf + 8) == fopAcM_GetProfName(i_actor) &&
        gabi::load<s8>(inf + 0xA) == gabi::load<s8>(gabi::ea(actor) + 0x2DD) /* argument */) {
        a_this->mStoolPos.copy(actor->current.pos);
    }
    return nullptr;
}
VERIFY(0x022E7C14, &daNpc_Tc_c::searchStoolPos);

/* 022E8714 */
void daNpc_Tc_c::set_mtx() {
    WWHD_FUNC(0x022E8714, void, this);
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}
VERIFY(0x022E8714, &daNpc_Tc_c::set_mtx);

/* 022E8CAC */
cPhs_State daNpc_Tc_c::_create() {
    WWHD_FUNC(0x022E8CAC, cPhs_State, this);
    /* fopAcM_SetupActor(this, daNpc_Tc_c) */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = TC_VTBL;
            gabi::call(0x025A5B18, &mSmokeCallBack, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
            dPa_followEcallBack_ct(&field_0x714, 0, 0);
            dPa_followEcallBack_ct(&field_0x728, 0, 0);
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, STR(0x100229CB) /* "Tc" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        getArg();
        if (!isCreate()) {
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, 0x022E7B24 /* createHeap_CB */, 0x1C80)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        createInit();
    }
    return phase_state;
}
VERIFY(0x022E8CAC, &daNpc_Tc_c::_create);

/* 022E8DD0 */
static cPhs_State daNpc_Tc_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022E8DD0, cPhs_State, i_this);
    return ((daNpc_Tc_c*)i_this)->_create();
}
VERIFY(0x022E8DD0, daNpc_Tc_Create);

/* 022E8DD4 */
BOOL daNpc_Tc_c::_delete() {
    WWHD_FUNC(0x022E8DD4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x100229CE) /* "Tc" */);
    gabi::call_ptr(gabi::load<u32>(mSmokeCallBack.__vtbl + 0x44), &mSmokeCallBack); /* mSmokeCallBack.remove() */
    field_0x714.remove();
    field_0x728.remove();
    if (heap.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022E8DD4, &daNpc_Tc_c::_delete);

/* 022E8E60 */
static BOOL daNpc_Tc_Delete(daNpc_Tc_c* i_this) {
    WWHD_FUNC(0x022E8E60, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022E8E60, daNpc_Tc_Delete);

/* 022EA460 */
static BOOL daNpc_Tc_Execute(daNpc_Tc_c* i_this) {
    WWHD_FUNC(0x022EA460, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022EA460, daNpc_Tc_Execute);

/* 022EA644 */
static BOOL daNpc_Tc_Draw(daNpc_Tc_c* i_this) {
    WWHD_FUNC(0x022EA644, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022EA644, daNpc_Tc_Draw);

/* 022E8E64 */
void daNpc_Tc_c::setTower() {
    WWHD_FUNC(0x022E8E64, void, this);
    m_tower_actor = fopAcIt_Judge(0x022E7880 /* searchTower_CB */, this); /* fopAcM_Search */
    if (m_tower_actor.get() == nullptr) /* JUT_ASSERT(0x917, m_tower_actor != NULL) */
        JUT_ASSERT_fail(STR(0x100229D4), 0x917, STR(0x100229E4));
    gabi::Local<cXyz> temp;
    s16 temp2;
    u8 type = mType;
    switch (type) {
    case TYPE_WHITE:
        temp->copy(l_HIO().field_0x60);
        temp2 = l_HIO().field_0x7A;
        break;
    case TYPE_RED:
        temp->copy(l_HIO().field_0x6C);
        temp2 = l_HIO().field_0x78;
        break;
    }
    switch (type) {
    case TYPE_RED:
    case TYPE_WHITE: {
        fopAc_ac_c* t = m_tower_actor;
        mDoMtx_stack_c::transS(t->current.pos.x, t->current.pos.y, t->current.pos.z);
        t = m_tower_actor;
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), t->shape_angle.x, t->shape_angle.y, t->shape_angle.z);
        PSMTXMultVec(mDoMtx_stack_c::get(), temp, &current.pos);
        t = m_tower_actor;
        s16 temp3 = (s16)(t->shape_angle.y + temp2);
        if (tower_isStop(t)) {
            if (mAnmPrmIdx == 19 /* ANM_PRM_IDX_MAWASU */ || mAnmPrmIdx == 20 /* ANM_PRM_IDX_WAIT04 */) {
                cLib_addCalcAngleS2(&current.angle.y, temp3, 4, 0x800);
            }
        } else {
            current.angle.y = temp3;
        }
        break;
    }
    }
}
VERIFY(0x022E8E64, &daNpc_Tc_c::setTower);

/* 022EBFB8 */
void daNpc_Tc_c::startTower() {
    WWHD_FUNC(0x022EBFB8, void, this);
    fopAc_ac_c* tower = m_tower_actor;
    if (tower == nullptr) { /* JUT_ASSERT(0x412, m_tower_actor != NULL) */
        JUT_ASSERT_fail(STR(0x10022C0C), 0x412, STR(0x10022C1C));
        tower = m_tower_actor;
    }
    s16 temp2;
    u8 type = mType;
    switch (type) {
    case TYPE_WHITE:
        temp2 = l_HIO().field_0x7A; /* the position (field_0x60) is not used */
        break;
    case TYPE_RED:
        temp2 = l_HIO().field_0x78;
        break;
    }
    switch (type) {
    case TYPE_RED:
    case TYPE_WHITE: {
        m_jnt.mbHeadLock = 1; /* onHeadLock() */
        fopAc_ac_c* t = m_tower_actor;
        mDoMtx_stack_c::transS(t->current.pos.x, t->current.pos.y, t->current.pos.z);
        t = m_tower_actor;
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), t->shape_angle.x, t->shape_angle.y, t->shape_angle.z);
        t = m_tower_actor;
        temp2 = (s16)(t->shape_angle.y + temp2);
        cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x400);
        if (cLib_distanceAngleS(current.angle.y, temp2) < 0x600) {
            tower_offStop(tower);
            mAnmPrmIdx = 19; /* ANM_PRM_IDX_MAWASU */
        }
        break;
    }
    default:
        tower_offStop(tower);
        break;
    }
}
VERIFY(0x022EBFB8, &daNpc_Tc_c::startTower);

/* 022EBA4C */
void daNpc_Tc_c::setAttention() {
    WWHD_FUNC(0x022EBA4C, void, this);
    s8 temp = mAnmPrmIdx;
    /* ANM_PRM_IDX_WAIT01, WALK01, TALK01, GUARD, MAWASU, TALK02, WAIT04 */
    if (temp != 1 && temp != 5 && temp != 4 && temp != 10 && temp != 19 && temp != 21 && temp != 20) {
        return;
    }
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    eyePos.x = mEyePos.x;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = mAttPos.y + l_HIO().mAttnYOffset;
    attPos->z = mAttPos.z;
    attPos->x = mAttPos.x;
    attPos->y = y;
}
VERIFY(0x022EBA4C, &daNpc_Tc_c::setAttention);

/* 022EBAC4 */
void daNpc_Tc_c::calc_sitpos() {
    WWHD_FUNC(0x022EBAC4, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> temp4;
    temp3->set(0.0f, 80.0f, -25.0f);
    temp4->set(0.0f, 0.0f, -60.0f);
    s16 angle = cLib_targetAngleY(&player->current.pos, &mStoolPos);
    mDoMtx_stack_c::transS(mStoolPos.x, mStoolPos.y, mStoolPos.z);
    mDoMtx_stack_c::YrotM(angle);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp3, &mSitPos);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp4, &mWalkToStoolPos);
}
VERIFY(0x022EBAC4, &daNpc_Tc_c::calc_sitpos);

/* 022EBB90 */
void daNpc_Tc_c::calcMove() {
    WWHD_FUNC(0x022EBB90, void, this);
    f32 speed;
    if (mAnmPrmIdx != 5 /* ANM_PRM_IDX_WALK01 */) {
        return;
    }
    cLib_chaseF(&speedF, mTargetSpeed, 0.1f);
    f32 moveSpeed = mEventCut.mSpeed;
    if (moveSpeed != 0.0f) {
        switch ((u32)mEventCut.mCurActIdx) { /* getNowCut() */
        case 2:
        case 4:
            speed = moveSpeed * l_HIO().field_0x54;
            break;
        default:
            speed = 0.5f; /* HD: the uninitialised speed is the clamp value */
            break;
        }
    } else {
        speed = speedF * l_HIO().field_0x54;
    }
    if (speed < 0.5f) {
        speed = 0.5f;
    }
    mpMorf->setPlaySpeed(speed);
}
VERIFY(0x022EBB90, &daNpc_Tc_c::calcMove);

/* 022EBF00 */
void daNpc_Tc_c::statusWait() {
    WWHD_FUNC(0x022EBF00, void, this);
    if (mType == TYPE_WHITE || mType == TYPE_RED) {
        fopAc_ac_c* tower = m_tower_actor;
        if (tower == nullptr) { /* JUT_ASSERT(0x611, m_tower_actor != NULL) */
            JUT_ASSERT_fail(STR(0x10022BE4), 0x611, STR(0x10022BF4));
            tower = m_tower_actor;
        }
        if (tower_isStop(tower)) {
            mAnmPrmIdx = 20; /* ANM_PRM_IDX_WAIT04 */
        } else {
            mAnmPrmIdx = 19; /* ANM_PRM_IDX_MAWASU */
        }
    }
    if (mTalkingNearJail) {
        mStatus = STATUS_TALK;
    } else if (mHasAttention) {
        mEventIdx = 2;
    }
}
VERIFY(0x022EBF00, &daNpc_Tc_c::statusWait);

/* 022EC0EC */
void daNpc_Tc_c::statusTalk() {
    WWHD_FUNC(0x022EC0EC, void, this);
    if (talk(1) != 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        return;
    }
    switch ((u8)mType) {
    case TYPE_RED:
    case TYPE_WHITE:
        startTower();
        mAnmPrmIdx = 19; /* ANM_PRM_IDX_MAWASU */
        /* fallthrough */
    case TYPE_NORMAL2:
        startTower();
        /* fallthrough */
    default:
        mStatus = STATUS_WAIT;
        mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
        dComIfGp_event_reset();
        mTalkingNearJail = false;
        break;
    }
}
VERIFY(0x022EC0EC, &daNpc_Tc_c::statusTalk);

/* 022EC174 */
void daNpc_Tc_c::statusSit() {
    WWHD_FUNC(0x022EC174, void, this);
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
    calc_sitpos();
    current.pos.copy(mSitPos);
    gravity = 0.0f;
    if (mAnmPrmIdx != 6 /* ANM_PRM_IDX_JAMP_A */) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (dLib_checkPlayerInCircle(pos, l_HIO().field_0x2C, 100.0f)) {
            if (!mHasEnteredSitRadius) {
                mHasEnteredSitRadius = true;
                mEventIdx = 3;
                mStatus = STATUS_DEMO_JUMP;
                return;
            }
            mAnmPrmIdx = 6; /* ANM_PRM_IDX_JAMP_A */
        }
    }
    if (mpMorf->isStop() && mAnmPrmIdx == 6 /* ANM_PRM_IDX_JAMP_A */) {
        speedF = l_HIO().field_0x38;
        speed.y = l_HIO().field_0x40;
        gravity = l_HIO().field_0x3C;
        mAnmPrmIdx = 7; /* ANM_PRM_IDX_JAMP_B */
        mStatus = STATUS_JUMP;
    }
}
VERIFY(0x022EC174, &daNpc_Tc_c::statusSit);

/* 022EC2C0 */
void daNpc_Tc_c::statusJump() {
    WWHD_FUNC(0x022EC2C0, void, this);
    if (!(speed.y > 0.0f) && mAnmPrmIdx == 7 /* ANM_PRM_IDX_JAMP_B */) {
        mAnmPrmIdx = 8; /* ANM_PRM_IDX_JAMP_C1 */
    }
    if (mObjAcch.ChkGroundHit() && mAnmPrmIdx == 8 /* ANM_PRM_IDX_JAMP_C1 */) {
        speedF = 0.0f;
        mAnmPrmIdx = 9; /* ANM_PRM_IDX_JAMP_C2 */
        mJumpLandingTimer = 0x33;
        return;
    }
    if (cLib_calcTimer_c((be<u8>*)&mJumpLandingTimer) == 0 && mAnmPrmIdx == 9 /* ANM_PRM_IDX_JAMP_C2 */) {
        mStatus = STATUS_WALK_TO_JAIL;
        mAnmPrmIdx = 5; /* ANM_PRM_IDX_WALK01 */
    }
}
VERIFY(0x022EC2C0, &daNpc_Tc_c::statusJump);

/* 022EC5C4 */
void daNpc_Tc_c::statusTalkNearJail() {
    WWHD_FUNC(0x022EC5C4, void, this);
    mTargetSpeed = 0.0f;
    speedF = 0.0f;
    if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mStatus = STATUS_WAIT_NEAR_JAIL;
        dComIfGp_event_reset();
        mTalkingNearJail = false;
    }
}
VERIFY(0x022EC5C4, &daNpc_Tc_c::statusTalkNearJail);

/* 022ECB58 */
void daNpc_Tc_c::statusDemoJump() {
    WWHD_FUNC(0x022ECB58, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022C4C) /* "TC_JUMP_DEMO" */)) {
        mStatus = STATUS_WALK_TO_JAIL;
        dComIfGp_event_reset();
    }
}
VERIFY(0x022ECB58, &daNpc_Tc_c::statusDemoJump);

/* 022ECBB4 */
void daNpc_Tc_c::statusDemoRescue() {
    WWHD_FUNC(0x022ECBB4, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022C5C) /* "TC_RESCUE" */)) {
        dComIfGs_onEventBit(0x0B80);
        dComIfGp_event_reset();
        fopAcM_delete(this);
        gabi::store<u8>(gabi::load<u32>(0x101F8344) + 0x243, 1); /* HD */
    }
    /* HD: fades the environment light's override (envlight+0x10A4) after 0xCB frames */
    if (gabi::load<u8>(gabi::ea(dKy_getEnvlight()) + 0x10A4) != 0xFF) {
        s32 n = field_0x7F4_hd + 1;
        field_0x7F4_hd = n;
        if (n > 0xCB) {
            gabi::store<u8>(gabi::ea(dKy_getEnvlight()) + 0x10A4, 0xFF);
        }
    }
}
VERIFY(0x022ECBB4, &daNpc_Tc_c::statusDemoRescue);

/* 022ECC64 */
void daNpc_Tc_c::statusDemoTalk() {
    WWHD_FUNC(0x022ECC64, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022C7C)) || dComIfGp_evmng_endCheck(STR(0x10022C68)) ||
        dComIfGp_evmng_endCheck(STR(0x10022C90))) { /* "TC_TALK_NEAR_JAIL", "TC_TALK_NEAR_JAIL_S", "TC_TALK_NEAR_JAIL2" */
        mStatus = STATUS_WAIT_NEAR_JAIL;
        dComIfGp_event_reset();
        mTalkingNearJail = false;
    }
}
VERIFY(0x022ECC64, &daNpc_Tc_c::statusDemoTalk);

/* 022ECD00 */
void daNpc_Tc_c::statusGetRupee() {
    WWHD_FUNC(0x022ECD00, void, this);
    if (talk(1) == 0x12) {
        dComIfGp_event_reset();
        mEventIdx = 8;
        mStatus = STATUS_DEMO_GET_RUPEE;
    }
}
VERIFY(0x022ECD00, &daNpc_Tc_c::statusGetRupee);

/* 022ECD58 */
void daNpc_Tc_c::statusDemoGetRupee() {
    WWHD_FUNC(0x022ECD58, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022CA4) /* "TC_GET_RUPEE" */)) {
        field_0x812 = true;
        dComIfGp_event_reset();
        mEventIdx = 1;
        mStatus = STATUS_TALK;
    }
}
VERIFY(0x022ECD58, &daNpc_Tc_c::statusDemoGetRupee);

/* 022ECDC8 */
void daNpc_Tc_c::statusPayRupee() {
    WWHD_FUNC(0x022ECDC8, void, this);
    if (talk(1) == 0x12) {
        dComIfGp_event_reset();
        mEventIdx = 9;
        mStatus = STATUS_DEMO_PAY_RUPEE;
    }
}
VERIFY(0x022ECDC8, &daNpc_Tc_c::statusPayRupee);

/* 022ECE20 */
void daNpc_Tc_c::statusDemoPayRupee() {
    WWHD_FUNC(0x022ECE20, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022CB4) /* "TC_PAY_RUPEE" */)) {
        dComIfGp_event_reset();
        mEventIdx = 1;
        mStatus = STATUS_TALK;
    }
}
VERIFY(0x022ECE20, &daNpc_Tc_c::statusDemoPayRupee);

/* 022ECE84 */
void daNpc_Tc_c::statusMonumentComplete() {
    WWHD_FUNC(0x022ECE84, void, this);
    if (talk(1) == 0x12) {
        dComIfGp_event_reset();
        mEventIdx = 8;
        mStatus = STATUS_DEMO_MONUMENT_COMPLETE;
    }
}
VERIFY(0x022ECE84, &daNpc_Tc_c::statusMonumentComplete);

/* 022ECEDC */
void daNpc_Tc_c::statusDemoMonumentComplete() {
    WWHD_FUNC(0x022ECEDC, void, this);
    if (dComIfGp_evmng_endCheck(STR(0x10022CC4) /* "TC_GET_RUPEE" */)) {
        field_0x813 = true;
        dComIfGp_event_reset();
        mEventIdx = 1;
        mStatus = STATUS_TALK;
    }
}
VERIFY(0x022ECEDC, &daNpc_Tc_c::statusDemoMonumentComplete);

/* 022EC384 */
void daNpc_Tc_c::statusWalkToJail() {
    WWHD_FUNC(0x022EC384, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 angle = cLib_targetAngleY(&current.pos, &player->current.pos);
    if (mObjAcch.ChkWallHit()) {
        if (mTalkingNearJail) {
            mTargetSpeed = 0.0f;
            speedF = 0.0f;
            if (!mHasTalkedNearJail || l_HIO().field_0x44 != 0) {
                mAnmPrmIdx = 2; /* ANM_PRM_IDX_WAIT03 */
                dComIfGp_event_reset();
                mStatus = STATUS_DEMO_TALK;
                if (dComIfGs_isEventBit(0x0B40) == 0) {
                    if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0) { /* dComIfGs_getClearCount() */
                        mEventIdx = 6;
                    } else {
                        mEventIdx = 5;
                    }
                } else {
                    mEventIdx = 7;
                }
                dComIfGs_onEventBit(0x0B40);
                mHasTalkedNearJail = true;
                return;
            }
            mStatus = STATUS_TALK_NEAR_JAIL;
        } else if (mHasAttention) {
            mEventIdx = 2;
        }
    }
    if (mObjAcch.ChkWallHit() && std::fabs((f32)cM_scos(angle)) > 0.9f) {
        mTargetSpeed = 0.0f;
        if (speedF == 0.0f) {
            mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
        }
    } else {
        mAnmPrmIdx = 5; /* ANM_PRM_IDX_WALK01 */
        mTargetSpeed = l_HIO().field_0x58;
        if (speedF == 0.0f) {
            speedF = l_HIO().field_0x5C;
        }
    }
    if (mObjAcch.ChkWallHit()) {
        gabi::Local<cXyz> pos;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        pos->x = current.pos.x;
        if (!dLib_checkPlayerInCircle(pos, l_HIO().field_0x30, 100.0f)) {
            mStatus = STATUS_WALK_TO_STOOL;
        }
    }
}
VERIFY(0x022EC384, &daNpc_Tc_c::statusWalkToJail);

/* 022EC62C */
void daNpc_Tc_c::statusWaitNearJail() {
    WWHD_FUNC(0x022EC62C, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 angle = cLib_targetAngleY(&current.pos, &player->current.pos);
    if (mObjAcch.ChkWallHit()) {
        if (mTalkingNearJail) {
            mTargetSpeed = 0.0f;
            speedF = 0.0f;
            if (!mHasTalkedNearJail || l_HIO().field_0x44 != 0) {
                mAnmPrmIdx = 2; /* ANM_PRM_IDX_WAIT03 */
                dComIfGp_event_reset();
                mStatus = STATUS_DEMO_TALK;
                if (dComIfGs_isEventBit(0x0B40) == 0) {
                    mEventIdx = 5;
                } else {
                    mEventIdx = 7;
                }
                dComIfGs_onEventBit(0x0B40);
                mHasTalkedNearJail = true;
                return;
            }
            mStatus = STATUS_TALK_NEAR_JAIL;
        } else if (mHasAttention) {
            mEventIdx = 2;
        }
    }
    if (mObjAcch.ChkWallHit() && std::fabs((f32)cM_scos(angle)) > 0.9f) {
        mTargetSpeed = 0.0f;
        if (speedF == 0.0f) {
            mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
        }
    } else {
        mAnmPrmIdx = 5; /* ANM_PRM_IDX_WALK01 */
        mTargetSpeed = l_HIO().field_0x58;
        if (speedF == 0.0f) {
            speedF = l_HIO().field_0x5C;
        }
    }
}
VERIFY(0x022EC62C, &daNpc_Tc_c::statusWaitNearJail);

/* 022EC7E8 */
void daNpc_Tc_c::statusWalkToStool() {
    WWHD_FUNC(0x022EC7E8, void, this);
    calc_sitpos();
    gabi::Local<cXyz> pos;
    pos->z = current.pos.z;
    pos->y = current.pos.y;
    pos->x = current.pos.x;
    if (dLib_checkPlayerInCircle(pos, l_HIO().field_0x30, 100.0f)) {
        mTargetSpeed = 0.0f;
        if (speedF == 0.0f) {
            mStatus = STATUS_WALK_TO_JAIL;
        }
    } else {
        mStoolLookPos.copy(mStoolPos);
        cLib_targetAngleY(&current.pos, &mWalkToStoolPos);
        cLib_addCalcPosXZ2(&current.pos, &mWalkToStoolPos, 0.1f, speedF);
        f32 temp = diffAbsXZ(&mWalkToStoolPos, &current.pos);
        if (temp < 5.0f) {
            mTargetSpeed = 0.0f;
            if (speedF == 0.0f) {
                mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
                mStatus = STATUS_SIT_TO_STOOL;
            }
        } else {
            mAnmPrmIdx = 5; /* ANM_PRM_IDX_WALK01 */
            mTargetSpeed = l_HIO().field_0x58;
        }
    }
}
VERIFY(0x022EC7E8, &daNpc_Tc_c::statusWalkToStool);

/* 022EC984 */
void daNpc_Tc_c::statusSitToStool() {
    WWHD_FUNC(0x022EC984, void, this);
    calc_sitpos();
    cLib_addCalcAngleS2(&current.angle.y, field_0x790.y, 4, 0x800);
    if (std::fabs((f32)(s32)(field_0x790.y - current.angle.y)) < 16.0f) {
        f32 sy = speed.y;
        if (sy == 0.0f && mObjAcch.ChkGroundHit()) {
            sy = 22.0f;
            speed.y = sy;
            gravity = -2.5f;
        }
        if (sy < 0.0f) {
            mAnmPrmIdx = 3; /* ANM_PRM_IDX_WAIT02 */
            if (speed.y < 0.0f && current.pos.y < mSitPos.y) {
                gravity = 0.0f;
                speed.y = 0.0f;
                current.pos.y = mSitPos.y;
            }
        }
        cLib_addCalcPosXZ2(&current.pos, &mSitPos, 0.1f, 4.0f);
        f32 diff = diffAbsXZ(&mSitPos, &current.pos);
        f32 y = current.pos.y;
        if (y == mSitPos.y && diff < 5.0f) {
            mSmokePos.x = current.pos.x;
            mSmokePos.y = y;
            mSmokePos.z = current.pos.z;
            mSmokeAngle.x = current.angle.x;
            mSmokeAngle.y = current.angle.y;
            mSmokeAngle.z = current.angle.z;
            smoke_set(2.0f, 0.25f, 0.0f, 5.0f, 20.0f);
            mStatus = STATUS_SIT;
        }
    }
}
VERIFY(0x022EC984, &daNpc_Tc_c::statusSitToStool);

/* 022E7CE4 */
void daNpc_Tc_c::smoke_set(f32 i_rate, f32 i_spread, f32 i_initialVelOmni, f32 i_initialVelAxis, f32 i_initialVelDir) {
    WWHD_FUNC(0x022E7CE4, void, this, i_rate, i_spread, i_initialVelOmni, i_initialVelAxis, i_initialVelDir);
    /* static JGeometry::TVec3<f32> smoke_scale(1, 1, 1): guard 0x104688F0, object 0x104688F4 */
    u32 smoke_scale = 0x104688F4;
    if (gabi::load<u32>(0x104688F0) == 0) {
        gabi::store<u32>(0x104688F0, 1);
        gabi::store<f32>(smoke_scale, 1.0f);
        gabi::store<f32>(smoke_scale + 8, 1.0f);
        gabi::store<f32>(smoke_scale + 4, 1.0f);
    }
    if (mSmokeCallBack.mpEmitter.get() == nullptr) {
        /* dComIfGp_particle_setToon(dPa_name::ID_AK_JT_ELEMENTSMOKE00, ...) */
        s8 roomNo = fopAcM_GetRoomNo(this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &mSmokePos, &mSmokeAngle, nullptr, 0xB9,
                        (dPa_levelEcallBack*)&mSmokeCallBack, roomNo, nullptr, nullptr, nullptr);
    }
    if (mSmokeCallBack.mpEmitter.get() != nullptr) {
        gabi::store<f32>(gabi::ea(mSmokeCallBack.mpEmitter.get()) + 0x34, i_rate);            /* setRate */
        gabi::store<f32>(gabi::ea(mSmokeCallBack.mpEmitter.get()) + 0x58, i_spread);          /* setSpread */
        gabi::store<f32>(gabi::ea(mSmokeCallBack.mpEmitter.get()) + 0x68, i_initialVelOmni);  /* setAwayFromCenterSpeed */
        gabi::store<f32>(gabi::ea(mSmokeCallBack.mpEmitter.get()) + 0x6C, i_initialVelAxis);  /* setAwayFromAxisSpeed */
        gabi::store<f32>(gabi::ea(mSmokeCallBack.mpEmitter.get()) + 0x70, i_initialVelDir);   /* setDirectionalSpeed */
        u32 e = gabi::ea(mSmokeCallBack.mpEmitter.get()); /* setGlobalScale(smoke_scale) */
        f32 x = gabi::load<f32>(smoke_scale), y = gabi::load<f32>(smoke_scale + 4), z = gabi::load<f32>(smoke_scale + 8);
        gabi::store<f32>(e + 0x220, x);
        gabi::store<f32>(e + 0x224, y);
        gabi::store<f32>(e + 0x228, z);
        gabi::store<f32>(e + 0x238, x);
        gabi::store<f32>(e + 0x23C, y);
        gabi::store<f32>(e + 0x240, z);
    }
}
VERIFY(0x022E7CE4, &daNpc_Tc_c::smoke_set);

/* chkAttention(current.pos, head_y + angle.y + backbone_y): cXyz by value, a float copy on the stack */
static inline u8 chkAttention_cur(daNpc_Tc_c* i_this) {
    s16 temp = (s16)(i_this->current.angle.y + i_this->m_jnt.mAngles[0][1] + i_this->m_jnt.mAngles[1][1]);
    gabi::Local<cXyz> pos;
    pos->x = i_this->current.pos.x;
    pos->y = i_this->current.pos.y;
    pos->z = i_this->current.pos.z;
    return i_this->chkAttention(pos, temp);
}

/* 022ED17C */
BOOL daNpc_Tc_c::help_action(void*) {
    WWHD_FUNC(0x022ED17C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mStatus = STATUS_SIT;
        mActionStatus = (s8)(mActionStatus + 1); /* ACTION_ONGOING */
    } else if (mActionStatus != ACTION_ENDING) {
        if (dComIfGs_isEventBit(0x0B80)) {
            fopAcM_delete(this);
        }
        if (dComIfGp_evmng_endCheck(STR(0x10022CD4) /* "OpenDoor" */)) {
            mTargetSpeed = 0.0f;
            speedF = 0.0f;
            mEventIdx = 4;
            mStatus = STATUS_DEMO_RESCUE;
            /* HD: environment light override for the rescue (see statusDemoRescue) */
            gabi::store<u8>(gabi::ea(dKy_getEnvlight()) + 0x10A4, 2);
            field_0x7F4_hd = 0;
            return true;
        } else {
            mHasAttention = chkAttention_cur(this);
            mEventIdx = 0;
            switch (mStatus) {
            case STATUS_WAIT: statusWait(); break;
            case STATUS_TALK: statusTalk(); break;
            case STATUS_SIT: statusSit(); break;
            case STATUS_JUMP: statusJump(); break;
            case STATUS_WALK_TO_JAIL: statusWalkToJail(); break;
            case STATUS_TALK_NEAR_JAIL: statusTalkNearJail(); break;
            case STATUS_WAIT_NEAR_JAIL: statusWaitNearJail(); break;
            case STATUS_WALK_TO_STOOL: statusWalkToStool(); break;
            case STATUS_SIT_TO_STOOL: statusSitToStool(); break;
            case STATUS_DEMO_JUMP: statusDemoJump(); break;
            case STATUS_DEMO_RESCUE: statusDemoRescue(); break;
            case STATUS_DEMO_TALK: statusDemoTalk(); break;
            }
            calcMove();
            lookBack();
            setAttention();
        }
    }
    return true;
}
VERIFY(0x022ED17C, &daNpc_Tc_c::help_action);

/* 022ECF4C */
BOOL daNpc_Tc_c::wait_action(void*) {
    WWHD_FUNC(0x022ECF4C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mStatus = STATUS_WAIT;
        mActionStatus = (s8)(mActionStatus + 1); /* ACTION_ONGOING */
    } else if (mActionStatus != ACTION_ENDING) {
        mHasAttention = chkAttention_cur(this);
        mEventIdx = 0;
        switch (mStatus) {
        case STATUS_WAIT: statusWait(); break;
        case STATUS_TALK: statusTalk(); break;
        case STATUS_PAY_RUPEE: statusPayRupee(); break;
        case STATUS_DEMO_PAY_RUPEE: statusDemoPayRupee(); break;
        case STATUS_GET_RUPEE: statusGetRupee(); break;
        case STATUS_DEMO_GET_RUPEE: statusDemoGetRupee(); break;
        case STATUS_MONUMENT_COMPLETE: statusMonumentComplete(); break;
        case STATUS_DEMO_MONUMENT_COMPLETE: statusDemoMonumentComplete(); break;
        }
        calcMove();
        lookBack();
        setAttention();
    }
    return TRUE;
}
VERIFY(0x022ECF4C, &daNpc_Tc_c::wait_action);

/* 022EBC48 */
void daNpc_Tc_c::lookBack() {
    WWHD_FUNC(0x022EBC48, void, this);
    gabi::Local<cXyz> temp6;
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f; /* cXyz temp3(0, 0, 0) */
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    if (mStatus == STATUS_TALK) {
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (mStatus == STATUS_WALK_TO_JAIL || mStatus == STATUS_WAIT_NEAR_JAIL || mStatus == STATUS_SIT) {
        m_jnt.mbTrn = 1;
        mHasAttention = true;
    }
    if (mType == TYPE_WHITE || mType == TYPE_RED) {
        if (mAnmPrmIdx == 19 /* ANM_PRM_IDX_MAWASU */ || mAnmPrmIdx == 20 /* ANM_PRM_IDX_WAIT04 */) {
            m_jnt.mbBackBoneLock = 1; /* onBackBoneLock() */
            m_jnt.mbTrn = 0;          /* clrTrn() */
        } else if (mStatus != STATUS_WAIT) {
            m_jnt.mbBackBoneLock = 0;
            m_jnt.mbTrn = 1;
            mHasAttention = true;
        }
    }
    if (mStatus == STATUS_WALK_TO_STOOL) {
        m_jnt.mbTrn = 1;
        temp6->copy(mStoolLookPos);
        dstPos = temp6;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
    } else if (mEventCut.mbAttention /* getAttnFlag() */) {
        m_jnt.mbTrn = 1;
        temp6->x = mEventCut.mPos.x; /* getAttnPos() */
        temp6->y = mEventCut.mPos.y;
        temp6->z = mEventCut.mPos.z;
        dstPos = temp6;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
    } else if (mHasAttention) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, l_HIO().m04);
        temp6->copy(*eye);
        dstPos = temp6;
        ty = eyePos.y;
        tz = current.pos.z;
        tx = current.pos.x;
    }
    if (m_jnt.mbTrn) { /* trnChk() */
        s16 target = l_HIO().mMaxHeadTurnVel;
        s16 temp4 = mEventCut.mTurnSpeed; /* getTurnSpeed() */
        if (temp4 != 0) {
            target = temp4;
        }
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, target, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    gabi::Local<cXyz> temp3;
    temp3->x = tx;
    temp3->y = ty;
    temp3->z = tz;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp3, desiredYRot, mMaxHeadTurnVelocity, false);
}
VERIFY(0x022EBC48, &daNpc_Tc_c::lookBack);

/* 022E87DC */
void daNpc_Tc_c::createInit() {
    WWHD_FUNC(0x022E87DC, void, this);
    if (mType == TYPE_NORMAL) {
        gabi::store<u32>(gabi::ea(this) + 0x2E0, gabi::load<u32>(gabi::ea(this) + 0x2E0) | 0x4000); /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
    }
    field_0x790.x = current.angle.x;
    field_0x790.y = current.angle.y;
    field_0x790.z = current.angle.z;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    mAttPos.copy(current.pos);
    mEyePos.copy(current.pos);
    mStts.Init(0xFE, 0xFE, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    mHasTalkedNearJail = false;
    mTexPatternNumIdx = 1;
    switch ((u8)mType) {
    case TYPE_NORMAL:
        mAnmPrmIdx = 3; /* ANM_PRM_IDX_WAIT02 */
        fopAcIt_Judge(0x022E7C14 /* searchStoolPos */, this); /* fopAcM_Search */
        if (setAction_l(this, 0x022ED17C /* help_action */)) {
            gravity = 0.0f;
            mAcchCir.SetWall(60.0f, 50.0f);
        }
        break;
    case TYPE_BLUE:
    case TYPE_NORMAL2:
        gravity = -9.0f;
        mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
        if (setAction_l(this, 0x022ECF4C /* wait_action */)) {
            mAcchCir.SetWall(60.0f, 50.0f);
        }
        break;
    case TYPE_RED:
    case TYPE_WHITE:
        gravity = -9.0f;
        m_jnt.mbHeadLock = 1; /* onHeadLock() */
        mAnmPrmIdx = 19;      /* ANM_PRM_IDX_MAWASU */
        if (setAction_l(this, 0x022ECF4C /* wait_action */)) {
            mAcchCir.SetWall(60.0f, 10.0f);
        }
        break;
    }
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    mOldAnmPrmIdx = 0;
    setTexAnm();
    setAnm();
    set_mtx();
    mpMorf->calc();
    mEventCut.setActorInfo2(STR(0x100229C8) /* "Tc" */, (fopNpc_npc_c*)(void*)this);
    field_0x7F4_hd = 0; /* HD */
}
VERIFY(0x022E87DC, &daNpc_Tc_c::createInit);

/* 022EA464 */
BOOL daNpc_Tc_c::_draw() {
    WWHD_FUNC(0x022EA464, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData_l(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    gabi::call(0x025E7B3C, mBtpAnm, pModelData, (u32)mBlinkFrame); /* mBtpAnm.entry(pModelData, mBlinkFrame) */
    /* HD: one material set for all types (GameCube: a_bmt_tbl[mType]) */
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(pModelData) + 0x38, 0); /* mBtpAnm.remove(pModelData) */
    /* HD: no shadow */
    dSnap_RegistFig_l(gabi::load<u8>(0x10022B50 + mType) /* a_snap_tbl[mType] */, this, &current.pos, current.angle.y, 1.0f, 1.0f, 1.0f);
    /* HD debug display (l_HIO.m22, l_HIO.field_0x34): only function-local statics initialised on first use */
    if (l_HIO().m22 != 0) {
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x100226D0), 4); /* 028FEAC0 memcpy */
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF8), gabi::at<u8>(0x100226D4), 4); /* 028FEAC0 memcpy */
        }
    }
    if (l_HIO().field_0x34 != 0) {
        for (int i = 0; i < 3; i++) {
            if (gabi::load<u32>(0x101FDA50) != 0)
                break;
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x100226D0), 4); /* 028FEAC0 memcpy */
        }
        if (gabi::load<u32>(0x101FDA48) == 0) {
            gabi::store<u32>(0x101FDA48, 1);
            memcpy_g(gabi::at<u8>(0x101FEBEC), gabi::at<u8>(0x100226D8), 4); /* 028FEAC0 memcpy */
        }
    }
    return TRUE;
}
VERIFY(0x022EA464, &daNpc_Tc_c::_draw);

/* 022EA140 */
BOOL daNpc_Tc_c::_execute() {
    WWHD_FUNC(0x022EA140, BOOL, this);
    if (mType == TYPE_WHITE || mType == TYPE_RED || mType == TYPE_NORMAL2) {
        setTower();
    }
    m_jnt.setParam(l_HIO().mMaxBackboneX, l_HIO().mMaxBackboneY, l_HIO().mMinBackboneX, l_HIO().mMinBackboneY,
                   l_HIO().mMaxHeadX, l_HIO().mMaxHeadY, l_HIO().mMinHeadX, l_HIO().mMinHeadY, l_HIO().mMaxTurnStep);
    playTexPatternAnm();
    u32 mtrlSndId = 0;
    if (mObjAcch.ChkGroundHit()) {
        mtrlSndId = gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* GetMtrlSndId(mObjAcch.m_gnd) */
    }
    s8 roomNo = fopAcM_GetRoomNo(this);
    s32 reverb = dComIfGp_getReverb(roomNo);
    field_0x798 = (s8)mpMorf->play(&eyePos, mtrlSndId, (s8)reverb);
    mpMorf->calc();
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    if (mStatus != STATUS_WAIT) {
        if (!mEventCut.cutProc()) {
            gabi::call(0x022E9D60, this); /* cutProc() */
        } else {
            s32 nowCut = mEventCut.mCurActIdx; /* getNowCut() */
            if (nowCut != -1) {
                switch ((u32)nowCut) {
                case 2:
                case 4:
                    if (mEventCut.mSpeed == 0.0f) { /* getMoveSpeed() */
                        mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
                    } else {
                        mAnmPrmIdx = 5; /* ANM_PRM_IDX_WALK01 */
                    }
                    break;
                case 0:
                    mAnmPrmIdx = 1;
                    break;
                }
            } else {
                mAnmPrmIdx = 1;
            }
        }
    }
    if (!gabi::load<u8>(dComIfGp_ea() + 0x5292) /* dComIfGp_event_runCheck() */) {
        mEventCut.mbNoTurn = 0;    /* setAttnNoTurnFlag(false) */
        mEventCut.mbAttention = 0; /* setAttnFlag(false) */
    }
    eventOrder();
    setAnm();
    setTexAnm();
    if (mStatus != STATUS_WALK_TO_STOOL) {
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    }
    gabi::Local<cXyz> tempPos;
    tempPos->x = current.pos.x;
    tempPos->y = current.pos.y;
    tempPos->z = current.pos.z;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> delta;
    cXyz_mi(&current.pos, delta, tempPos);
    mDeltaPos.copy(*delta);
    gabi::store<s8>(gabi::ea(&tevStr) + 0xB9, gabi::call<s8>(0x024EF130, dComIfG_Bgsp(), gabi::ea(&mObjAcch) + 0xD4 + 0x14)); /* tevStr.mRoomNo = GetRoomId */
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, gabi::call<u8>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(&mObjAcch) + 0xD4 + 0x14)); /* mEnvrIdxOverride = GetPolyColor */
    set_mtx();
    if (mType == TYPE_NORMAL) {
        if (mStatus == STATUS_DEMO_RESCUE) {
            setCollision(60.0f, 150.0f);
        }
    } else {
        setCollision(60.0f, 150.0f);
    }
    return TRUE;
}
VERIFY(0x022EA140, &daNpc_Tc_c::_execute);

/* dLib_anm_prm_c (0x10) */
struct dLib_anm_prm_l {
    /* 0x00 */ be<s8> mBckIdx;
    /* 0x01 */ be<s8> mNextPrmIdx;
    /* 0x02 */ be<s16> field_0x02;
    /* 0x04 */ be<f32> mMorf;
    /* 0x08 */ be<f32> mPlaySpeed;
    /* 0x0C */ be<s32> mLoopMode;
};
WWHD_SIZE(dLib_anm_prm_l, 0x10);

/* mSmokePos = current.pos (integer copy); mSmokeAngle = current.angle */
static inline void setSmokeAtPos(daNpc_Tc_c* t) {
    t->mSmokePos.copy(t->current.pos);
    t->mSmokeAngle.x = t->current.angle.x;
    t->mSmokeAngle.y = t->current.angle.y;
    t->mSmokeAngle.z = t->current.angle.z;
}

/* 022E7E70 */
void daNpc_Tc_c::setAnm() {
    WWHD_FUNC(0x022E7E70, void, this);
    /* a_anm_bck_tbl (.rodata 0x100227F8, 17 entries); a_anm_prm_tbl[24] copied from .data 0x1002283C */
    gabi::Local<dLib_anm_prm_l[24]> tbl;
    dLib_anm_prm_l* a_anm_prm_tbl = gabi::at<dLib_anm_prm_l>(gabi::ea(tbl.get()));
    for (int i = 0; i < 0x180; i += 4)
        gabi::store<u32>(gabi::ea(a_anm_prm_tbl) + i, gabi::load<u32>(0x1002283C + i));

    if (mAnmPrmIdx == 22 /* ANM_PRM_IDX_TALK01_WAIT01 */) {
        if (mpMorf->isLoop()) {
            u8 t = (u8)(mTalk01Wait01Timer + 1);
            if (t >= 3) {
                mTalk01Wait01Timer = 0;
                mAnmPrmIdx = 1; /* ANM_PRM_IDX_WAIT01 */
            } else {
                mTalk01Wait01Timer = t;
            }
        }
    } else {
        mTalk01Wait01Timer = 0;
    }
    if (mAnmPrmIdx == 23 /* ANM_PRM_IDX_TALK01_TALK02 */) {
        if (mpMorf->isLoop()) {
            u8 t = (u8)(mTalk01Talk02Timer + 1);
            if (t >= 5) {
                mTalk01Talk02Timer = 0;
                mAnmPrmIdx = 21; /* ANM_PRM_IDX_TALK02 */
            } else {
                mTalk01Talk02Timer = t;
            }
        }
    } else {
        mTalk01Talk02Timer = 0;
    }
    if (mAnmPrmIdx == 11 /* ANM_PRM_IDX_JTBT */) {
        if (mpMorf->isLoop()) {
            u8 t = (u8)(mJtbtTimer + 1);
            if (t >= 4) {
                mJtbtTimer = 0;
                mAnmPrmIdx = 4; /* ANM_PRM_IDX_TALK01 */
            } else {
                mJtbtTimer = t;
            }
        }
    } else {
        mJtbtTimer = 0;
    }
    for (int i = 0; i < 24; i++) {
        a_anm_prm_tbl[i].mMorf = l_HIO().field_0x94[i];
    }

    if (mBckIdx == 14 /* BCK_IDX_MAWASU */ && gabi::ftoi(mpMorf->getFrame()) == 1) {
        u32 se = 0;
        if (mType == TYPE_RED) {
            se = 0x48FE; /* JA_SE_CV_TC_B_PUSH_BAR */
        } else if (mType == TYPE_WHITE) {
            se = 0x48FF; /* JA_SE_CV_TC_T_PUSH_BAR */
        }
        if (se != 0) { /* fopAcM_monsSeStart(this, se, 0) */
            u32 id = gabi::load<u32>(gabi::ea(this) + 4);
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
            gabi::call(0x025E1AA4, se, &eyePos, id, 0, reverb); /* mDoAud_monsSeStart */
        }
    }

    if (mBckIdx == 7 /* BCK_IDX_JAMP_C */) {
        if (mpMorf->getFrame() == 1.0f) {
            setSmokeAtPos(this);
            smoke_set(10.0f, 0.25f, 0.0f, 2.0f, 15.0f);
        } else {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
        }
    }

    if (mBckIdx == 11 /* BCK_IDX_DANCE01 */) {
        mParticlePos.x = 0.0f;
        mParticlePos.y = 0.0f;
        mParticlePos.z = 0.0f;
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), 11), mDoMtx_stack_c::get());
        PSMTXMultVec(mDoMtx_stack_c::get(), &mParticlePos, &mParticlePos);
        gabi::Local<cXyz> pos;
        pos->z = 1.0f;
        pos->y = 1.0f;
        pos->x = 1.0f;
        if (mpMorf->getFrame() == 1.0f) {
            dPa_control_set(dComIfGp_getParticle(), 0, 0x8189 /* ID_IT_SN_TG_KURURINP_KAMI_M00 */, &current.pos, &current.angle, pos, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x818A /* ID_IT_SN_TG_KURURINP_KAMI_R00 */, &current.pos, &current.angle, pos, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x818B /* ID_IT_SN_TG_KURURINP_KAMI_L00 */, &current.pos, &current.angle, pos, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            s8 roomNo = fopAcM_GetRoomNo(this);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x818C /* ID_IT_SN_TG_KURURINP_BLUR_R00 */, &mParticlePos, nullptr, nullptr, 0xFF,
                            (dPa_levelEcallBack*)&field_0x714, roomNo, nullptr, nullptr, nullptr);
            roomNo = fopAcM_GetRoomNo(this);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x818D /* ID_IT_SN_TG_KURURINP_BLUR_G00 */, &mParticlePos, nullptr, nullptr, 0xFF,
                            (dPa_levelEcallBack*)&field_0x728, roomNo, nullptr, nullptr, nullptr);
        }
        if (mpMorf->getFrame() == 88.0f) {
            s16 ay = current.angle.y;
            f32 sinv = cM_ssin(ay);
            f32 cosv = cM_scos(ay);
            f32 x = gabi::fmadds(10.0f, sinv, current.pos.x);
            f32 z = gabi::fmadds(10.0f, cosv, current.pos.z);
            mSmokePos.x = x;
            mSmokePos.y = current.pos.y;
            mSmokePos.z = z;
            mSmokeAngle.x = current.angle.x;
            mSmokeAngle.y = ay;
            mSmokeAngle.z = current.angle.z;
            smoke_set(5.0f, 0.25f, 0.0f, 10.0f, 1.0f);
        } else if (mpMorf->getFrame() == 143.0f) {
            setSmokeAtPos(this);
            smoke_set(10.0f, 0.25f, 0.0f, 15.0f, 1.0f);
        } else {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
        }
        if (mpMorf->getFrame() == 178.0f) {
            dPa_followEcallBack_end(&field_0x714);
            dPa_followEcallBack_end(&field_0x728);
        }
    }

    if (mBckIdx == 9 /* BCK_IDX_JTBT */) {
        if (mpMorf->getFrame() == 0.0f || mpMorf->getFrame() == 12.0f) {
            setSmokeAtPos(this);
            smoke_set(2.0f, 0.25f, 0.0f, 1.0f, 10.0f);
        } else {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
        }
    }

    if (mBckIdx == 10 /* BCK_IDX_HAPPY */) {
        if (mpMorf->getFrame() == 24.0f) {
            setSmokeAtPos(this);
            smoke_set(4.0f, 0.25f, 0.0f, 2.0f, 10.0f);
        } else {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
        }
    }

    if (mOldAnmPrmIdx != mAnmPrmIdx) {
        if (mAnmPrmIdx == 6 /* ANM_PRM_IDX_JAMP_A */) {
            gabi::Local<cXyz> particlePos;
            particlePos->x = current.pos.x;
            particlePos->y = current.pos.y - 80.0f;
            particlePos->z = current.pos.z;
            gabi::Local<cXyz> particleScale;
            particleScale->x = 1.0f;
            particleScale->y = 1.0f;
            particleScale->z = 1.0f;
            JPABaseEmitter* pEmitter = dPa_control_set(dComIfGp_getParticle(), 0, 0x8152 /* ID_IT_SN_PF_BIKON00 */, particlePos, nullptr,
                                                       particleScale, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            /* pEmitter->setGlobalParticleScale(0.62f, 0.6f) (HD: z = 1) */
            gabi::store<f32>(gabi::ea(pEmitter) + 0x238, 0.62f);
            gabi::store<f32>(gabi::ea(pEmitter) + 0x23C, 0.6f);
            gabi::store<f32>(gabi::ea(pEmitter) + 0x240, 1.0f);
            /* fopAcM_seStart(this, JA_SE_CM_CMN_NOTICE, 0) (HD: no null checks here) */
            mDoAud_seStart(0x58BD, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        }
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&mSmokeCallBack);
    }
    gabi::call(0x02587A0C, STR(0x100227F4) /* "Tc" */, mpMorf.get(), &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx,
               0x100227F8 /* a_anm_bck_tbl */, a_anm_prm_tbl, 0 /* false */, 0 /* HD */); /* dLib_bcks_setAnm */
}
VERIFY(0x022E7E70, &daNpc_Tc_c::setAnm);
