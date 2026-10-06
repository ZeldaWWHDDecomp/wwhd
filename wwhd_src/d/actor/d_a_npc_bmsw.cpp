/**
 * d_a_npc_bmsw.cpp (WWHD)
 * NPC - Koboli (Rito mail sorter)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bmsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_bmsw.h"

#define SAFESTRING_VTBL 0x10018004 /* this TU's sead::SafeString vtable */
#define BMSW_VTBL 0x10018390       /* daNpc_Bmsw_c vtable (HD: merged with fopNpc_npc_c's) */

/* SwMail_c / daNpc_Bmsw_c member functions (GHS pointers to member: {0, -1, function}) */
enum : u32 {
    FN_SwMail_Dummy = 0x0220F3C4,
    FN_SwMail_Appear = 0x0220F4C0,
    FN_SwMail_Wait = 0x0220F62C,
    FN_SwMail_Throw = 0x0220F7C4,
    FN_SwMail_End = 0x0220FA84,
    FN_wait_action = 0x02210AC0,
    FN_shiwake_game_action = 0x02210F60,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info event flags / registers: *(0x101F84DC) + 0x644; temporary flags + 0x1178 */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out-of-line copy of another TU) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* 025E1988 HD: mDoAud_seStart(id) without a position */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
/* dComIfG_getTimerRestTimeMs (HD): (play+0x5CF8 - play+0x5CF4) * 1000 / 30, play re-read for each */
static inline s32 dComIfG_getTimerRestTimeMs() {
    s32 a = gabi::load<s32>(dComIfGp_ea() + 0x5CF8);
    s32 b = gabi::load<s32>(dComIfGp_ea() + 0x5CF4);
    return (s32)((u32)(a - b) * 1000u) / 30; /* wrapping product, as mulli */
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
/* (this->*pmf)() without an argument */
static inline void pmf_call0(void* self, ProcFunc_l* pmf) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr(pmf->f, thisp);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr(gabi::load<u32>(vt + i * 8 + 4), thisp);
    }
}
/* pmf == {0, -1, func} (GHS: equal indices, then zero index or equal delta and function) */
static inline bool pmf_is(ProcFunc_l* pmf, u32 func) {
    s16 i = pmf->i;
    if (i != -1)
        return false;
    if (i == 0)
        return true;
    return pmf->d == 0 && pmf->f == func;
}
static inline void pmf_set(ProcFunc_l* pmf, u32 func) {
    pmf->i = -1;
    pmf->f = func;
    pmf->d = 0;
}

/* c_lib */
static inline s16 cLib_targetAngleX(const cXyz* a, const cXyz* b) { return gabi::call<s16>(0x0200F974, a, b); }
static inline f32 cLib_addCalcPos(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200EE00, pos, target, scale, maxStep, minStep);
}
static inline void cLib_addCalcPos2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep) { gabi::call(0x0200F164, pos, target, scale, maxStep); }
static inline bool cXyz_normalizeRS(cXyz* v) { return gabi::call<bool>(0x0201B47C, v); }
static inline void mDoMtx_ZXYrotS(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1AA4, m, x, y, z); }
/* 025E19CC HD: mDoAud_seStart(id, pos) */
static inline void mDoAud_seStart_pos(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* dComIfGp_plusMiniGameRupee(n): play+0x5CEC (s16), HD: saturates (a non-positive sum gives 0) */
static inline void dComIfGp_plusMiniGameRupee_1() {
    u32 a = dComIfGp_ea() + PLAY_BGS + 0x4A4C;
    if ((s32)gabi::load<s16>(a) + 1 > 0)
        gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + 1));
    else
        gabi::store<s16>(a, 0);
}
/* mDoExt_btpAnm::remove(modelData) (inline): the model data's texture pattern (+0x38) is cleared */
static inline void mDoExt_btpAnm_remove(J3DModelData* d) { gabi::store<u32>(gabi::ea(d) + 0x38, 0); }

/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* HD: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), name) == 0 through two sead::SafeString
 * temporaries (this TU's assureTerminationImpl_ 02212004, called directly and through the vtable) */
static inline bool startStageIs(u32 name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = SAFESTRING_VTBL;
    b->__vtbl = SAFESTRING_VTBL;
    a->mStringTop = name;
    b->mStringTop = 0x1047E6B8;
    gabi::call(0x02212004, a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

/* STControl (d_lib) */
static inline void STControl_setWaitParm(STControl_l* c, s16 a, s16 b, s16 d, s16 e, f32 f, f32 g, s16 h, s16 i) {
    gabi::call(0x025885C4, c, a, b, d, e, f, g, h, i);
}
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 025D5AA4 fopAcM_createChild(name, parentPcId, param, pos, roomNo, angle, scale, createFunc) (HD: by name, no subtype) */
static inline u32 fopAcM_createChild(const char* name, u32 parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, u32 cb) {
    return gabi::call<u32>(0x025D5AA4, name, parent, param, pos, roomNo, angle, scale, cb);
}
/* 025D58B4 fopAcM_create(name, param, pos, roomNo, angle, scale, createFunc) (HD: by name, no subtype) */
static inline u32 fopAcM_create_name(const char* name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, u32 cb) {
    return gabi::call<u32>(0x025D58B4, name, param, pos, roomNo, angle, scale, cb);
}
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
/* 025BEBB8 dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_l(u8 type, fopAc_ac_c* a, cXyz* pos, s16 angleY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angleY, x, y, z);
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
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
/* dNpc_HIO_c (HD: vtable last, at +0x24) */
struct dNpc_HIO_l {
    /* 0x00 */ be<f32> m04;
    /* 0x04 */ be<s16> mMaxHeadX;
    /* 0x06 */ be<s16> mMaxBackboneX;
    /* 0x08 */ be<s16> mMaxHeadY;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinHeadX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMinHeadY;
    /* 0x12 */ be<s16> mMinBackboneY;
    /* 0x14 */ be<s16> mMaxTurnStep;
    /* 0x16 */ be<s16> mMaxHeadTurnVel;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<s16> mMaxAttnAngleY;
    /* 0x1E */ be<u8> m22;
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> mMaxAttnDistXZ;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_l, 0x28);
/* daNpc_Bmsw_HIO_c (HD: vtable last, at +0x54; GameCube members -4) */
struct daNpc_Bmsw_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ dNpc_HIO_l mNpc;
    /* 0x2C */ be<s16> field_0x30;
    /* 0x2E */ be<s16> field_0x32;
    /* 0x30 */ be<s16> field_0x34;
    /* 0x32 */ be<s16> r_1;
    /* 0x34 */ be<s16> g_1;
    /* 0x36 */ be<s16> b_1;
    /* 0x38 */ be<s16> r_2;
    /* 0x3A */ be<s16> g_2;
    /* 0x3C */ be<s16> b_2;
    /* 0x3E */ u8 _3E[2];
    /* 0x40 */ be<f32> field_0x44;
    /* 0x44 */ be<f32> field_0x48;
    /* 0x48 */ be<f32> field_0x4C;
    /* 0x4C */ be<f32> field_0x50;
    /* 0x50 */ be<f32> field_0x54;
    /* 0x54 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Bmsw_HIO_c, 0x58);
static daNpc_Bmsw_HIO_c& l_HIO() { return *gabi::at<daNpc_Bmsw_HIO_c>(0x10466708); }
/* SwMail_c::m_same_count / m_no_buff, SwCam_c::camera_center_data[2][3] / camera_eye */
static be<u8>& SwMail_m_same_count() { return *gabi::at<be<u8>>(0x101BD4B1); }
static be<u8>& SwMail_m_no_buff() { return *gabi::at<be<u8>>(0x101BD4B0); }
static cXyz* SwCam_camera_center_data(int row, int col) { return gabi::at<cXyz>(0x10466804 + (row * 3 + col) * 0xC); }
static cXyz* SwCam_camera_eye() { return gabi::at<cXyz>(0x1046684C); }

/* ======================================================================================= */

/* 0220E0D0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0220E0D0, BOOL, i_this);
    return ((daNpc_Bmsw_c*)i_this)->CreateHeap();
}
VERIFY(0x0220E0D0, CallbackCreateHeap);

/* 0220EB64 */
static cPhs_State daNpc_Bmsw_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0220EB64, cPhs_State, i_this);
    return ((daNpc_Bmsw_c*)i_this)->_create();
}
VERIFY(0x0220EB64, daNpc_Bmsw_Create);

/* 0220EBFC */
static BOOL daNpc_Bmsw_Delete(daNpc_Bmsw_c* i_this) {
    WWHD_FUNC(0x0220EBFC, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0220EBFC, daNpc_Bmsw_Delete);

/* 0220EFF0 */
static BOOL daNpc_Bmsw_Execute(daNpc_Bmsw_c* i_this) {
    WWHD_FUNC(0x0220EFF0, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0220EFF0, daNpc_Bmsw_Execute);

/* 0220F29C */
static BOOL daNpc_Bmsw_Draw(daNpc_Bmsw_c* i_this) {
    WWHD_FUNC(0x0220F29C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0220F29C, daNpc_Bmsw_Draw);

/* 02211F54 */
static BOOL daNpc_Bmsw_IsDelete(daNpc_Bmsw_c*) {
    WWHD_FUNC(0x02211F54, BOOL, (daNpc_Bmsw_c*)nullptr);
    return TRUE;
}
VERIFY(0x02211F54, daNpc_Bmsw_IsDelete);

/* 0220EB68 */
void SwMail_c::SeDelete() {
    WWHD_FUNC(0x0220EB68, void, this);
    mDoAud_seDeleteObject(&field_0x24);
}
VERIFY(0x0220EB68, &SwMail_c::SeDelete);

/* 0220F380 */
void SwMail_c::move() {
    WWHD_FUNC(0x0220F380, void, this);
    pmf_call0(this, &mFunc); /* (this->*mFunc)() */
}
VERIFY(0x0220F380, &SwMail_c::move);

/* 0220F3C4 */
void SwMail_c::Dummy() {
    WWHD_FUNC(0x0220F3C4, void, this);
    set_mtx();
}
VERIFY(0x0220F3C4, &SwMail_c::Dummy);

/* 0220F4A0 */
void SwMail_c::WaitInit() {
    WWHD_FUNC(0x0220F4A0, void, this);
    pmf_set(&mFunc, FN_SwMail_Wait);
}
VERIFY(0x0220F4A0, &SwMail_c::WaitInit);

/* 0220F744 (cXyz by value: pointer to a copy) */
void SwMail_c::ThrowInit(cXyz* param_1, u8 param_2) {
    WWHD_FUNC(0x0220F744, void, this, param_1, param_2);
    field_0x30.copy(*param_1);
    field_0x56 = 0;
    field_0x55 = param_2;
    pmf_set(&mFunc, FN_SwMail_Throw);
}
VERIFY(0x0220F744, &SwMail_c::ThrowInit);

/* 0220F784 */
void SwMail_c::EndInit() {
    WWHD_FUNC(0x0220F784, void, this);
    field_0x30.copy(*cXyz_Zero);
    field_0x30.z = -10.0f;
    pmf_set(&mFunc, FN_SwMail_End);
}
VERIFY(0x0220F784, &SwMail_c::EndInit);

/* 0220D320 */
BOOL daNpc_Bmsw_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    WWHD_FUNC(0x0220D320, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData_l(field_0x6D4);
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x10018090) /* "Bmsw" */, gabi::load<s32>(0x10017FE0 + 4 * field_0x9D4));
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(392, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x100180B4), 0x188, STR(0x10018098));
    if (!mDoExt_btpAnm_init(field_0x7F8, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    field_0x80C = 0;
    field_0x964 = 0;
    return TRUE;
}
VERIFY(0x0220D320, &daNpc_Bmsw_c::initTexPatternAnm);

/* 0220EC00 */
void daNpc_Bmsw_c::playTexPatternAnm() {
    WWHD_FUNC(0x0220EC00, void, this);
    if (cLib_calcTimer(&field_0x964) == 0) {
        s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)field_0x80C >= max) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            field_0x80C = (u8)(field_0x80C - frameMax);
            field_0x964 = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            field_0x80C = (u8)(field_0x80C + 1);
        }
    }
}
VERIFY(0x0220EC00, &daNpc_Bmsw_c::playTexPatternAnm);

/* 0220ECC4 */
void daNpc_Bmsw_c::checkOrder() {
    WWHD_FUNC(0x0220ECC4, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2) /* checkCommandDemoAccrpt() */
        return;
    if (cmd != 1) /* !checkCommandTalk() */
        return;
    if (field_0x9D7 != 1 && field_0x9D7 != 2)
        return;
    field_0x9D7 = 0;
    field_0x9B9 = 1;
}
VERIFY(0x0220ECC4, &daNpc_Bmsw_c::checkOrder);

/* 0220EE0C */
void daNpc_Bmsw_c::eventOrder() {
    WWHD_FUNC(0x0220EE0C, void, this);
    if (field_0x9D7 == 1 || field_0x9D7 == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (field_0x9D7 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x0220EE0C, &daNpc_Bmsw_c::eventOrder);

/* 0220FC70 */
void daNpc_Bmsw_c::setAnm(s8 idx) {
    WWHD_FUNC(0x0220FC70, void, this, idx);
    /* a_play_mode_tbl 0x101BD438, a_morf_frame_tbl 0x101BD454, a_play_speed_tbl 0x101BD470;
     * l_bck_ix_tbl 0x1001804C, l_arm_bck_ix_tbl 0x10018068 */
    if ((u32)(s32)idx != (u32)(s32)field_0x9D5) {
        field_0x9D5 = idx;
        s32 m = field_0x9DA;
        dNpc_setAnmIDRes(mpMorf, gabi::load<s32>(0x101BD438 + 4 * m), gabi::load<f32>(0x101BD454 + 4 * m),
                         gabi::load<f32>(0x101BD470 + 4 * m), gabi::load<s32>(0x1001804C + 4 * (s32)idx), -1, STR(0x100182BC));
        m = field_0x9DA;
        s32 n = field_0x9D5;
        dNpc_setAnmIDRes(mpMorfHand, gabi::load<s32>(0x101BD438 + 4 * m), gabi::load<f32>(0x101BD454 + 4 * m),
                         gabi::load<f32>(0x101BD470 + 4 * m), gabi::load<s32>(0x10018068 + 4 * n), -1, STR(0x100182BC));
    }
}
VERIFY(0x0220FC70, &daNpc_Bmsw_c::setAnm);

/* 0221047C */
void daNpc_Bmsw_c::setAttention() {
    WWHD_FUNC(0x0221047C, void, this);
    f32 z = mAttPos.z;
    f32 y = mAttPos.y;
    f32 x = mAttPos.x;
    f32 yy = y + l_HIO().mNpc.mAttnYOffset;
    gabi::store<f32>(gabi::ea(this) + 0x398, z); /* attention_info.position */
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    gabi::store<f32>(gabi::ea(this) + 0x394, yy);
}
VERIFY(0x0221047C, &daNpc_Bmsw_c::setAttention);

/* 02210D20 */
void daNpc_Bmsw_c::TimerCountDown() {
    WWHD_FUNC(0x02210D20, void, this);
    s32 rest = dComIfG_getTimerRestTimeMs();
    if (field_0x9B4 > rest) {
        if (field_0x9B4 <= 10000) {
            mDoAud_seStart_id(0x8AD); /* JA_SE_MINIGAME_TIMER_30 */
            field_0x9B4 = field_0x9B4 - 1000;
        } else {
            mDoAud_seStart_id(0x8AE); /* JA_SE_MINIGAME_TIMER_10 */
            field_0x9B4 = field_0x9B4 - 10000;
        }
    }
}
VERIFY(0x02210D20, &daNpc_Bmsw_c::TimerCountDown);

/* 02210DCC */
BOOL daNpc_Bmsw_c::checkNextMailThrowOK() {
    WWHD_FUNC(0x02210DCC, BOOL, this);
    u8 mailIdx = field_0x93C;
    if (mailIdx < 2) {
        mailIdx++;
    } else {
        mailIdx = 0;
    }
    u8 ret = 1;
    SwMail_c* mail = field_0x930[mailIdx];
    if (!pmf_is(&mail->mFunc, FN_SwMail_Dummy)) {
        if (!pmf_is(&mail->mFunc, FN_SwMail_End)) {
            ret = 0;
        }
    }
    return ret;
}
VERIFY(0x02210DCC, &daNpc_Bmsw_c::checkNextMailThrowOK);

/* 02211F40 sead::SafeString::~SafeString (deleting; this TU's copy, vtable 0x10018004 +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02211F40, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x02211F40, SafeString_dt);

/* 02212004 sead::SafeString::assureTerminationImpl_ (empty; this TU's copy, vtable 0x10018004 +0x14) */
static void SafeString_assureTerminationImpl_(SafeString* i_this) {
    WWHD_FUNC(0x02212004, void, i_this);
}
VERIFY(0x02212004, SafeString_assureTerminationImpl_);

/* 02211C80 */
static daNpc_Bmsw_HIO_c* daNpc_Bmsw_HIO_c_ct(daNpc_Bmsw_HIO_c* i_this) {
    WWHD_FUNC(0x02211C80, daNpc_Bmsw_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Bmsw_HIO_c*)operator_new(0x58);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001803C;
    gabi::call(0x0259DA18, gabi::ea(i_this) + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    i_this->mNpc.m04 = -20.0f;
    i_this->mNpc.mMaxHeadX = 0x1FFE;
    i_this->mNpc.mMaxHeadY = 3000;
    i_this->mNpc.mMaxBackboneX = 0;
    i_this->mNpc.mMaxBackboneY = 0x1C70;
    i_this->mNpc.mMinHeadX = -0x9C4;
    i_this->mNpc.mMinHeadY = -3000;
    i_this->mNpc.mMinBackboneX = 0;
    i_this->mNpc.mMinBackboneY = -0x1C70;
    i_this->mNpc.mMaxTurnStep = 0x1000;
    i_this->mNpc.mMaxHeadTurnVel = 0x800;
    i_this->mNpc.mAttnYOffset = 50.0f;
    i_this->mNpc.mMaxAttnAngleY = 0x2000;
    i_this->mNpc.m22 = 0;
    i_this->mNpc.mMaxAttnDistXZ = 300.0f;
    i_this->field_0x30 = 0x1E;
    i_this->field_0x32 = 0xF;
    i_this->field_0x34 = 0x3C;
    i_this->r_1 = 192;
    i_this->g_1 = 174;
    i_this->b_1 = 192;
    i_this->r_2 = 192;
    i_this->g_2 = 174;
    i_this->b_2 = 192;
    i_this->field_0x44 = 40.0f;
    i_this->field_0x48 = 80.0f;
    i_this->field_0x4C = 50.0f;
    i_this->field_0x50 = 0.8f;
    i_this->field_0x54 = 0.75f;
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x02211C80, daNpc_Bmsw_HIO_c_ct);

/* 02211DC4 */
static void __sinit_d_a_npc_bmsw_cpp() {
    WWHD_FUNC(0x02211DC4, void);
    /* the per-TU header statics (see bindings.h sinit_header_statics), in this unit's layout */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104666D4 + 4 * i, 0);
    __register_global_object(0x101BD48C);
    gabi::store<f32>(0x104666B8, -3.1415927f);
    gabi::store<f32>(0x104666BC, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104666D0);
    __register_global_object(0x101BD498);
    gabi::call(0x028EAB2C, 0x104666D1);
    __register_global_object(0x101BD4A4);
    gabi::store<f32>(0x104666CC, 10000.0f);
    gabi::store<f32>(0x104666C0, 50000.0f);
    gabi::store<f32>(0x104666C4, 50000.0f);
    gabi::store<f32>(0x104666C8, 10000.0f);
    daNpc_Bmsw_HIO_c_ct(&l_HIO());
    static const f32 cc[2][3][3] = {
        {{-104.0f, 805.0f, 875.0f}, {-139.0f, 805.0f, 882.0f}, {-173.0f, 805.0f, 875.0f}},
        {{-104.0f, 825.0f, 868.0f}, {-139.0f, 825.0f, 875.0f}, {-173.0f, 825.0f, 868.0f}},
    };
    for (int r = 0; r < 2; r++)
        for (int c = 0; c < 3; c++)
            SwCam_camera_center_data(r, c)->set(cc[r][c][0], cc[r][c][1], cc[r][c][2]);
    SwCam_camera_eye()->set(-139.0f, 815.0f, 662.0f);
}
VERIFY(0x02211DC4, __sinit_d_a_npc_bmsw_cpp);

/* ---- SwMail_c ---- */

/* 0220D430 */
void SwMail_c::set_mtx() {
    WWHD_FUNC(0x0220D430, void, this);
    mDoMtx_stack_c::transS(field_0x24.x + field_0x3C.x, field_0x24.y + field_0x3C.y, field_0x24.z + field_0x3C.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), field_0x48.x, field_0x48.y, field_0x48.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0220D430, &SwMail_c::set_mtx);

/* 0220F2A0 */
void SwMail_c::set_mtx_throw() {
    WWHD_FUNC(0x0220F2A0, void, this);
    mDoMtx_stack_c::transS(field_0x24.x + field_0x3C.x, field_0x24.y + field_0x3C.y, field_0x24.z + field_0x3C.z);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), field_0x48.x);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), field_0x48.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0220F2A0, &SwMail_c::set_mtx_throw);

/* 0220D50C */
void SwMail_c::init() {
    WWHD_FUNC(0x0220D50C, void, this);
    field_0x54 = 0;
    field_0x56 = 0;
    field_0x30.copy(*cXyz_Zero);
    field_0x3C.copy(*cXyz_Zero);
    field_0x55 = 0;
    cXyz* p = field_0x5C;
    field_0x24.set(p->x, p->y - 200.0f, p->z);
    set_mtx();
}
VERIFY(0x0220D50C, &SwMail_c::init);

/* 0220D57C (static) */
u8 SwMail_c::getNextNo(u8 previous_no) {
    WWHD_FUNC(0x0220D57C, u8, previous_no);
    f32 random = cM_rndF(10.0f);
    u8 cnt = SwMail_m_same_count();
    f32 fVar4 = gabi::fnmsubs(1.25f, (f32)cnt, 2.5f); /* 2.5f - m_same_count * 1.25f (fused) */
    s32 box_x = previous_no / 3;
    s32 box_y = previous_no - box_x * 3;
    if (random < fVar4) {
        SwMail_m_same_count() = (u8)(cnt + 1);
    } else {
        f32 new_threshold = 2.5f + fVar4;
        if (random < new_threshold) {
            if (cM_rndF(1.0f) > 0.5f) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
            } else {
                box_x = box_x == 0;
            }
        } else { /* HD: also for NaN */
            fVar4 = cM_rndF(1.0f);
            if (fVar4 < 0.3333f) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
                box_x = box_x == 0;
            } else if (fVar4 < 0.6666f) {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
            } else {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
                box_x = box_x == 0;
            }
        }
        SwMail_m_same_count() = 0;
    }
    u8 res = (u8)(box_y + box_x * 3);
    SwMail_m_no_buff() = res;
    return res;
}
VERIFY(0x0220D57C, &SwMail_c::getNextNo);

/* 0220D7AC */
void SwMail_c::DummyInit() {
    WWHD_FUNC(0x0220D7AC, void, this);
    cLib_targetAngleY(field_0x58, field_0x5C);
    field_0x48.y = -0x4000;
    field_0x48.z = 0;
    field_0x48.x = 0;
    cXyz* p = field_0x5C;
    field_0x24.set(p->x - 150.0f, p->y - 200.0f, p->z);
    field_0x54 = getNextNo(field_0x54);
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x3C.copy(*cXyz_Zero);
    pmf_set(&mFunc, FN_SwMail_Dummy);
}
VERIFY(0x0220D7AC, &SwMail_c::DummyInit);

/* 0220D884 */
BOOL SwMail_c::MailCreateInit(cXyz* param_1, cXyz* param_2) {
    WWHD_FUNC(0x0220D884, BOOL, this, param_1, param_2);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x100180F0) /* "Bmsw" */, 0xF /* BDL_QMAIL */);
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    if (mpModel.get() == nullptr) {
        return FALSE;
    }
    void* texPattern = dComIfG_getObjectIDRes(STR(0x100180F0), 0x10 /* BTP_QMAIL */);
    if (!mDoExt_btpAnm_init(field_0x10, modelData, texPattern, TRUE, 2 /* EMode_LOOP */, 0.0f, 0, -1, 0, FALSE)) {
        return FALSE;
    }
    field_0x5C = param_2;
    field_0x54 = 0;
    field_0x58 = param_1;
    init();
    DummyInit();
    return TRUE;
}
VERIFY(0x0220D884, &SwMail_c::MailCreateInit);

/* 0220F3C8 */
void SwMail_c::AppearInit() {
    WWHD_FUNC(0x0220F3C8, void, this);
    cLib_targetAngleY(field_0x58, field_0x5C);
    cXyz* p = field_0x5C;
    u8 no = field_0x54;
    field_0x48.y = -0x4000;
    field_0x48.x = 0;
    field_0x48.z = 0;
    field_0x24.set(p->x, p->y - 200.0f, p->z);
    u8 res = getNextNo(no);
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x54 = res;
    field_0x3C.copy(*cXyz_Zero);
    field_0x56 = 0;
    mDoAud_seStart_id(0x69A5); /* JA_SE_LETTER_GAME_NEW */
    pmf_set(&mFunc, FN_SwMail_Appear);
}
VERIFY(0x0220F3C8, &SwMail_c::AppearInit);

/* 0220F4C0 */
void SwMail_c::Appear() {
    WWHD_FUNC(0x0220F4C0, void, this);
    s16 new_y = cLib_targetAngleY(field_0x58, field_0x5C);
    gabi::Local<cXyz> diff;
    cXyz_mi(field_0x58, diff, field_0x5C);
    if (!cXyz_normalizeRS(diff)) {
        diff->set(0.0f, 0.0f, 1.0f);
    }
    gabi::Local<cXyz> vec;
    vec->copy(*field_0x5C.get());
    gabi::Local<cXyz> tmp;
    cXyz_ml(diff, tmp, 120.0f);
    diff->copy(*tmp);
    PSVECAdd(vec, diff, vec);
    f32 y = vec->y - 40.0f;
    field_0x48.x = field_0x4E.x;
    field_0x48.y = (s16)(field_0x4E.y + new_y);
    vec->y = y;
    cLib_addCalcAngleS2(&field_0x48.z, field_0x4E.z, 4, 0x1000);
    if (std::fabs(cLib_addCalcPos(&field_0x24, vec, 0.25f, 30.0f, 2.5f)) < 2.5f) {
        WaitInit();
    }
    set_mtx();
}
VERIFY(0x0220F4C0, &SwMail_c::Appear);

/* 0220F62C */
void SwMail_c::Wait() {
    WWHD_FUNC(0x0220F62C, void, this);
    s16 new_y = cLib_targetAngleY(field_0x58, field_0x5C);
    gabi::Local<cXyz> diff;
    cXyz_mi(field_0x58, diff, field_0x5C);
    if (!cXyz_normalizeRS(diff)) {
        diff->set(0.0f, 0.0f, 1.0f);
    }
    field_0x24.copy(*field_0x5C.get());
    gabi::Local<cXyz> tmp;
    cXyz_ml(diff, tmp, 120.0f);
    diff->copy(*tmp);
    PSVECAdd(&field_0x24, diff, &field_0x24);
    f32 y = field_0x24.y - 40.0f;
    field_0x48.y = (s16)(field_0x4E.y + new_y);
    field_0x48.z = field_0x4E.z;
    field_0x48.x = field_0x4E.x;
    field_0x24.y = y;
    set_mtx();
}
VERIFY(0x0220F62C, &SwMail_c::Wait);

/* 0220F7C4 */
void SwMail_c::Throw() {
    WWHD_FUNC(0x0220F7C4, void, this);
    u8 step = field_0x56;
    s16 x_angle = cLib_targetAngleX(&field_0x30, field_0x5C); /* HD: hoisted out of both branches */
    if (step == 0) {
        s16 y_angle = cLib_targetAngleY(field_0x58, field_0x5C);
        gabi::Local<cXyz> vec;
        vec->set(-40.0f, -50.0f, 0.0f);
        gabi::Local<cXyz> multVec;
        mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), x_angle, y_angle, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), vec, multVec);
        PSVECAdd(multVec, field_0x5C, multVec);
        if (cLib_addCalcPos(&field_0x24, multVec, 0.5f, 30.0f, 2.0f) < 1.0f) {
            field_0x48.y = 0;
            field_0x4E.x = 0;
            mDoAud_seStart_id(0x2800); /* JA_SE_LK_SW_KAZEKIRI_S */
            field_0x56 = (u8)(field_0x56 + 1);
        }
        set_mtx();
    } else {
        field_0x48.x = (s16)-x_angle;
        s16 add = (s16)(field_0x54 * 0x80 + 0x1000);
        field_0x48.z = 0;
        field_0x48.y = (s16)(field_0x48.y + add);
        f32 pos_step = cLib_addCalcPos(&field_0x24, &field_0x30, 0.5f, l_HIO().field_0x44, 1.0f);
        field_0x4E.x = (s16)(field_0x4E.x + 4000);
        field_0x3C.y = 20.0f * (1.0f - cM_scos(field_0x4E.x));
        if (pos_step < 1.0f) {
            mDoAud_seStart_pos(0x69A6, &field_0x24); /* JA_SE_LETTER_IN_BOX */
            u8 aim = field_0x55;
            u8 no = field_0x54;
            if (aim == no) {
                dComIfGp_plusMiniGameRupee_1();
                mDoAud_seStart_id(0x8A5); /* JA_SE_MINIGAME_RIGHT */
            } else {
                gabi::Local<cXyz> dir;
                dir->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(3, 9, dir);
                mDoAud_seStart_id(0x8A6); /* JA_SE_MINIGAME_WRONG */
            }
            EndInit();
        }
        set_mtx_throw();
    }
}
VERIFY(0x0220F7C4, &SwMail_c::Throw);

/* 0220FA84 */
void SwMail_c::End() {
    WWHD_FUNC(0x0220FA84, void, this);
    if (field_0x55 == field_0x54) {
        cLib_addCalcAngleS2(&field_0x48.y, 0, 4, 0x1000);
        gabi::Local<cXyz> target;
        target->set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
        cLib_addCalcPos2(&field_0x3C, target, 0.25f, 5.0f);
        set_mtx_throw();
    } else {
        cLib_addCalcAngleS2(&field_0x48.y, -0x8000, 4, 0x1000);
        gabi::Local<cXyz> target;
        target->set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
        cLib_addCalcPos2(&field_0x3C, target, 0.25f, 5.0f);
        if (field_0x24.z < 995.0f) {
            s16 tgt;
            if (field_0x24.y > 720.0f) {
                tgt = -0x4000;
                field_0x30.y = field_0x30.y - 3.0f;
            } else {
                tgt = 0;
                field_0x30.y = field_0x30.y * 0.6f;
                field_0x30.x = field_0x30.x * 0.9f;
                field_0x30.z = field_0x30.z * 0.9f;
            }
            if (!(field_0x24.y > 700.0f)) {
                field_0x24.y = 700.0f;
                field_0x30.y = 0.0f;
            }
            cLib_addCalcAngleS2(&field_0x48.x, tgt, 2, 0x800);
        }
        PSVECScale(&field_0x30, &field_0x30, 0.9f);
        PSVECAdd(&field_0x24, &field_0x30, &field_0x24);
        set_mtx_throw();
    }
}
VERIFY(0x0220FA84, &SwMail_c::End);

/* 0220EFF4 */
void SwMail_c::draw(dKy_tevstr_c* tevStr) {
    WWHD_FUNC(0x0220EFF4, void, this, tevStr);
    J3DModelData* modelData = J3DModel_getModelData_l(mpModel);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)field_0x10, modelData, field_0x54);
    dScnKy_env_light_c* l = dKy_getEnvlight();
    setLightTevColorType(l, mpModel, tevStr);
    mDoExt_modelUpdateDL(mpModel);
    mDoExt_btpAnm_remove(modelData);
}
VERIFY(0x0220EFF4, &SwMail_c::draw);

/* ---- SwCam_c ---- */

/* 0220ED04 */
void SwCam_c::Move() {
    WWHD_FUNC(0x0220ED04, void, this);
    if (mActive) {
        /* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): play+0x5AF8 + id * 0x34, id at play+0x5B30 */
        s32 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
        u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + camId * 0x34);
        cXyz* src = SwCam_camera_center_data(field_0x1D, field_0x1C + 1);
        gabi::Local<cXyz> center;
        center->set(src->x, src->y, src->z);
        cLib_addCalcPos(&mCenter, center, 0.25f, 10.0f, 1.0f);
        gabi::call(0x02514F44, camera + 0x248); /* camera->mCamera.Stay() */
        gabi::Local<cXyz> c;
        gabi::Local<cXyz> e;
        c->set(mCenter.x, mCenter.y, mCenter.z);
        e->set(mEye.x, mEye.y, mEye.z);
        gabi::call(0x02514F88, camera + 0x248, c.get(), e.get(), (f32)mFovY, 0); /* mCamera.Set(center, eye, fovy, 0) */
    }
}
VERIFY(0x0220ED04, &SwCam_c::Move);

/* ---- daNpc_Bmsw_c ---- */

/* 0220EB70 */
BOOL daNpc_Bmsw_c::_delete() {
    WWHD_FUNC(0x0220EB70, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x10018280) /* "Bmsw" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    mSwMail0.SeDelete();
    mSwMail1.SeDelete();
    mSwMail2.SeDelete();
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
        l_HIO().mNo = -1;
    }
    return TRUE;
}
VERIFY(0x0220EB70, &daNpc_Bmsw_c::_delete);

/* 0220FD3C (cXyz by value: pointer to a copy) */
u8 daNpc_Bmsw_c::chkAttention(cXyz* pos, s16 angle) {
    WWHD_FUNC(0x0220FD3C, u8, this, pos, angle);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - pos->z;
    f32 dx = player->current.pos.x - pos->x;
    f32 maxAttnDistXZ = l_HIO().mNpc.mMaxAttnDistXZ;
    s32 maxAttnAngleY = l_HIO().mNpc.mMaxAttnAngleY;
    f32 distXZ = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 targetAngleY = cM_atan2s(dx, dz);
    if (mHasAttention) {
        maxAttnDistXZ += 40.0f;
        maxAttnAngleY += 0x71C; /* cM_deg2s(10.0f) */
    }
    targetAngleY -= angle;
    s32 a = targetAngleY < 0 ? -targetAngleY : targetAngleY;
    return maxAttnAngleY > a && maxAttnDistXZ > distXZ;
}
VERIFY(0x0220FD3C, &daNpc_Bmsw_c::chkAttention);

/* 0220FE68 */
u16 daNpc_Bmsw_c::next_msgStatus(be<u32>* currMsgNo) {
    WWHD_FUNC(0x0220FE68, u16, this, currMsgNo);
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    /* HD: the message's select number is in the message manager (*(0x101F4B5C) + 0x948) */
    u32 msgMgr = gabi::load<u32>(0x101F4B5C);
#define SELECT_NUM gabi::load<s32>(msgMgr + 0x948)
    switch (*currMsgNo) {
    case 0x1A2D:
    case 0x1A31:
    case 0x1A32:
    case 0x1A35:
    case 0x1A37:
    case 0x1A41:
    case 0x1A42:
    case 0x1A47:
    case 0x1A4A:
    case 0x1A4C:
    case 0x1A50:
    case 0x1A55:
    case 0x1A57:
    case 0x1A59:
    case 0x1A5A:
    case 0x1A5D:
    case 0x1A67:
    case 0x1A6D:
    case 0x1A6E:
        *currMsgNo = *currMsgNo + 1;
        break;
    case 0x1A63:
    case 0x1A66:
        *currMsgNo = 0x1A64;
        break;
    case 0x1A6A:
    case 0x1A6B:
        *currMsgNo = 0x1A6C;
        break;
    case 0x1A52:
        /* dComIfGp_getMiniGameRupee(): play+0x5CEC */
        if (gabi::load<s16>(dComIfGp_ea() + 0x5CEC) < 15) {
            *currMsgNo = 0x1A51;
        } else {
            *currMsgNo = 0x1A53;
        }
        break;
    case 0x1A60:
        if (gabi::load<s16>(dComIfGp_ea() + 0x5CEC) < 20) {
            *currMsgNo = 0x1A5E;
        } else {
            *currMsgNo = 0x1A61;
        }
        break;
    case 0x1A2E:
        if (SELECT_NUM == 0) {
            dComIfGs_onEventBit(0x1A01);
            *currMsgNo = 0x1A31;
        } else if (SELECT_NUM == 1) {
            if (gabi::call<BOOL>(0x02520A84, 3) /* dComIfGs_isStageBossEnemy(dSv_save_c::STAGE_DRC) */) {
                *currMsgNo = 0x1A2F;
            } else {
                *currMsgNo = 0x1A30;
            }
        }
        break;
    case 0x1A33:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A35;
        } else if (SELECT_NUM == 1) {
            *currMsgNo = 0x1A34;
        }
        break;
    case 0x1A34:
        if (!dComIfGs_isEventBit(0x1A01)) {
            *currMsgNo = 0x1A32;
        } else {
            *currMsgNo = 0x1A42;
        }
        break;
    case 0x1A38:
        *currMsgNo = 0x1A39; /* (*currMsgNo)++ */
        dComIfGs_setEventReg(0xC203, 1);
        break;
    case 0x1A3D:
        if (SELECT_NUM == 0) {
            dComIfGs_onEventBit(0x1A01);
            *currMsgNo = 0x1A31;
        } else {
            *currMsgNo = 0x1A3E;
        }
        break;
    case 0x1A4D:
        dComIfGs_setEventReg(0xC203, 2);
        // fallthrough
    case 0x1A51:
    case 0x1A53:
        *currMsgNo = 0x1A4E;
        break;
    case 0x1A58:
        dComIfGs_setEventReg(0xC203, 3);
        *currMsgNo = *currMsgNo + 1;
        break;
    case 0x1A5E:
    case 0x1A61:
        *currMsgNo = 0x1A5F;
        break;
    case 0x1A3F:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A41;
        } else {
            *currMsgNo = 0x1A40;
        }
        break;
    case 0x1A43:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A35;
        } else {
            *currMsgNo = 0x1A34;
        }
        break;
    case 0x1A44:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A46;
        } else {
            *currMsgNo = 0x1A45;
        }
        break;
    case 0x1A46:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A4A;
        } else {
            *currMsgNo = 0x1A47;
        }
        break;
    case 0x1A48:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A4A;
        } else {
            *currMsgNo = 0x1A49;
        }
        break;
    case 0x1A49:
        *currMsgNo = 0x1A47;
        break;
    case 0x1A54:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A55;
        } else {
            *currMsgNo = 0x1A45;
        }
        break;
    case 0x1A64:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A67;
        } else {
            *currMsgNo = 0x1A65;
        }
        break;
    default:
        ret = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
#undef SELECT_NUM
    return ret;
}
VERIFY(0x0220FE68, &daNpc_Bmsw_c::next_msgStatus);

/* 022102CC */
u32 daNpc_Bmsw_c::getMsg() {
    WWHD_FUNC(0x022102CC, u32, this);
    u32 msg = field_0x9C0;
    if (field_0x9C0 != fpcM_ERROR_PROCESS_ID_e) {
        field_0x9C0 = fpcM_ERROR_PROCESS_ID_e;
    } else if (dComIfGs_isEventBit(0x2701)) {
        if (!dComIfGs_isTmpBit(0x0320)) {
            dComIfGs_onTmpBit(0x0320);
            msg = 0x1A63;
        } else {
            msg = 0x1A66;
        }
    } else if (!dComIfGs_isEventBit(0x1A02)) {
        dComIfGs_onEventBit(0x1A02);
        msg = 0x1A2D;
    } else if (!dComIfGs_isEventBit(0x1A01)) {
        msg = 0x1A3D;
    } else {
        switch (dComIfGs_getEventReg(0xC203)) {
        case 0:
            msg = 0x1A3F;
            break;
        case 1:
            msg = 0x1A44;
            break;
        case 2:
            msg = 0x1A54;
            break;
        case 3:
        default:
            msg = 0x1A62;
        }
    }
    return msg;
}
VERIFY(0x022102CC, &daNpc_Bmsw_c::getMsg);

/* 022103D4 */
void daNpc_Bmsw_c::anmAtr(u16) {
    WWHD_FUNC(0x022103D4, void, this, (u16)0);
    /* dComIfGp_getMesgAnimeAttrInfo(): play+0x5BC5 */
    switch (gabi::load<u8>(dComIfGp_ea() + 0x5BC5)) {
    case 5:
        setAnm(1);
        break;
    case 6:
        setAnm(2);
        break;
    case 9:
        setAnm(3);
        break;
    case 14:
        setAnm(4);
        break;
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); /* dComIfGp_setMesgAnimeAttrInfo(0xFF) */
}
VERIFY(0x022103D4, &daNpc_Bmsw_c::anmAtr);

/* daNpc_Bmsw_c::setAction(func, NULL) (inline): func is a non-virtual member {0, -1, func} */
static inline void setAction_l(daNpc_Bmsw_c* t, u32 func) {
    ProcFunc_l* cur = &t->mCurrActionFunc;
    s16 i = cur->i;
    if (i == -1) {
        if (cur->d == 0 && cur->f == func)
            return;
    } else if (i == 0) {
        goto set_new;
    }
    t->mActionStatus = -1; /* ACTION_ENDING */
    pmf_call(t, cur, nullptr);
set_new:
    t->mActionStatus = 0; /* ACTION_STARTING */
    cur->d = 0;
    cur->i = -1;
    cur->f = func;
    pmf_call(t, cur, nullptr);
}

/* 022104A4 */
void daNpc_Bmsw_c::lookBack() {
    WWHD_FUNC(0x022104A4, void, this);
    gabi::Local<cXyz> vec2;
    f32 vx = 0.0f, vy = 0.0f, vz = 0.0f; /* cXyz vec(0.0f, 0.0f, 0.0f) */
    cXyz* dstPos = nullptr;
    s16 desired_y_rot = current.angle.y;
    switch (field_0x9DA) {
    case 1:
    case 2:
        if (mHasAttention) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye, l_HIO().mNpc.m04);
            vec2->copy(*eye);
            dstPos = vec2;
            vx = current.pos.x;
            vy = eyePos.y;
            vz = current.pos.z;
        } else if (dComIfGs_isEventBit(0x2701) && field_0x9E0 != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* ac = fopAcM_SearchByID(field_0x9E0);
            if (ac != nullptr) {
                vec2->copy(ac->eyePos);
                dstPos = vec2;
                vx = current.pos.x;
                vy = eyePos.y;
                vz = current.pos.z;
            }
        }
    }
    gabi::Local<cXyz> vec;
    if (m_jnt.mbTrn) { /* trnChk() */
        cLib_addCalcAngleS2(&field_0x97A, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
        vec->set(vx, vy, vz);
        dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec, desired_y_rot, field_0x97A, true);
    } else {
        field_0x97A = 0;
        vec->set(vx, vy, vz);
        dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec, desired_y_rot, 0, true);
    }
}
VERIFY(0x022104A4, &daNpc_Bmsw_c::lookBack);

/* 0221068C */
void daNpc_Bmsw_c::wait01() {
    WWHD_FUNC(0x0221068C, void, this);
    if (field_0x9B9) {
        m_jnt.mbHeadLock = 0;
        m_jnt.mbBackBoneLock = 0;
        field_0x9DA = 2;
    } else {
        if (mHasAttention) {
            field_0x9D7 = 2;
        }
        if (!dComIfGs_isEventBit(0x2701)) {
            if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
                field_0x9D6 = (s8)(field_0x9D6 - 1);
                if (field_0x9D6 <= 0) {
                    if (field_0x9D5 == 5) {
                        field_0x9D6 = 1;
                        setAnm(6);
                    } else {
                        field_0x9D6 = (s8)gabi::ftoi(cM_rndF(4.0f) + 2.0f);
                        setAnm(5);
                    }
                }
            }
            if (field_0x9D5 == 5 || field_0x9D5 == 6) {
                m_jnt.mbHeadLock = 1;
                m_jnt.mbBackBoneLock = 1;
            } else {
                m_jnt.mbHeadLock = 0;
                m_jnt.mbBackBoneLock = 0;
            }
        }
    }
}
VERIFY(0x0221068C, &daNpc_Bmsw_c::wait01);

/* 02210804 */
void daNpc_Bmsw_c::talk01() {
    WWHD_FUNC(0x02210804, void, this);
    u16 talk_res = talk(1);
    /* dComIfGp_checkMesgSendButton(): play+0x5BD2 */
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BD2) == 1) {
        u32 no = mCurrMsgNo;
        /* HD: 0x1A68 also activates the game camera */
        if (no == 0x1A32 || no == 0x1A42 || no == 0x1A47 || no == 0x1A4A || no == 0x1A55 || no == 0x1A67 || no == 0x1A68) {
            /* mSwCam.ActiveOn() */
            mSwCam.mActive = 1;
            mSwCam.mCenter.copy(*SwCam_camera_center_data(0, 1));
            mSwCam.mEye.copy(*SwCam_camera_eye());
            mSwCam.mFovY = 58.0f;
            mSwCam.field_0x1C = 0;
            mSwCam.field_0x1D = 0;
        } else if (no == 0x1A39 || no == 0x1A4E || no == 0x1A5F || no == 0x1A59 || no == 0x1A6F) {
            /* dComIfGp_setItemRupeeCount(field_0x960): play+0x5B48 += n */
            s32 n = field_0x960;
            u32 a = dComIfGp_ea() + 0x5B48;
            gabi::store<s32>(a, gabi::load<s32>(a) + n);
        }
    }
    if (talk_res == 0x12) { /* fopMsgStts_BOX_CLOSED_e */
        field_0x9DA = 1;
        dComIfGp_event_reset();
        if (!dComIfGs_isEventBit(0x2701)) {
            setAnm(0);
        } else {
            setAnm(1);
        }
        field_0x9D6 = 1;
        field_0x9B9 = 0;
        u32 no = mCurrMsgNo;
        if (no == 0x1A36 || no == 0x1A4B || no == 0x1A56 || no == 0x1A68) {
            setAction_l(this, FN_shiwake_game_action);
        }
    }
}
VERIFY(0x02210804, &daNpc_Bmsw_c::talk01);

/* 02210AC0 */
BOOL daNpc_Bmsw_c::wait_action(void*) {
    WWHD_FUNC(0x02210AC0, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        field_0x9DA = 1;
        field_0x9D6 = 1;
        if (!dComIfGs_isEventBit(0x2701)) {
            /* HD: in "Demo10" Koboli starts sorting (animation 5) */
            if (startStageIs(0x100182DC /* "Demo10" */)) {
                setAnm(5);
            } else {
                setAnm(0);
            }
        } else {
            setAnm(1);
        }
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        s16 angle = (s16)(current.angle.y + m_jnt.mAngles[0][1] + m_jnt.mAngles[1][1]);
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y, current.pos.z);
        mHasAttention = chkAttention(pos, angle);
        field_0x9D7 = 0;
        switch (field_0x9DA) {
        case 1:
            wait01();
            break;
        case 2:
            talk01();
            break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}
VERIFY(0x02210AC0, &daNpc_Bmsw_c::wait_action);

/* 02210E74 */
void daNpc_Bmsw_c::setGameGetRupee(s16 rupees) {
    WWHD_FUNC(0x02210E74, void, this, rupees);
    s16 v;
    switch (dComIfGs_getEventReg(0xC203)) {
    case 0:
        if (rupees < 10) {
            v = 0;
        } else {
            v = rupees >> 1;
        }
        break;
    case 1:
        if (rupees < 15) {
            v = 0;
        } else if (rupees < 20) {
            v = rupees >> 1;
        } else {
            v = rupees;
        }
        break;
    case 2:
        if (rupees < 20) {
            v = 0;
        } else if (rupees < 25) {
            v = rupees;
        } else {
            v = (s16)(rupees * 3);
        }
        break;
    case 3:
    default:
        if (dComIfGs_isEventBit(0x2701)) {
            v = rupees;
        } else {
            v = (s16)(rupees * 3);
        }
        break;
    }
    field_0x960 = v;
    gabi::store<s16>(dComIfGp_ea() + 0x5BA0, v); /* dComIfGp_setMessageCountNumber */
}
VERIFY(0x02210E74, &daNpc_Bmsw_c::setGameGetRupee);

/* ---- joint callbacks ---- */

/* 0220CCD4 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0220CCD4, BOOL, node, calcTiming);
    if (calcTiming == 0) { /* J3DNodeCBCalcTiming_In */
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Bmsw_c* i_this = gabi::at<daNpc_Bmsw_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (i_this != nullptr) {
            /* static cXyz a_att_pos_offst(0.0f, 0.0f, 0.0f): guard 0x104667F0, object 0x104666E4 */
            cXyz* a_att_pos_offst = gabi::at<cXyz>(0x104666E4);
            if (gabi::load<u32>(0x104667F0) == 0) {
                a_att_pos_offst->x = 0.0f;
                a_att_pos_offst->z = 0.0f;
                gabi::store<u32>(0x104667F0, 1);
                a_att_pos_offst->y = 0.0f;
            }
            /* static cXyz a_eye_pos_offst(26.0f, 26.0f, 0.0f): guard 0x104667F4, object 0x104666F0 */
            cXyz* a_eye_pos_offst = gabi::at<cXyz>(0x104666F0);
            if (gabi::load<u32>(0x104667F4) == 0) {
                a_eye_pos_offst->z = 0.0f;
                gabi::store<u32>(0x104667F4, 1);
                a_eye_pos_offst->y = 26.0f;
                a_eye_pos_offst->x = 26.0f;
            }
            u32 jointNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
            PSMTXCopy(getAnmMtx(model, jointNo), mDoMtx_stack_c::get());
            if (jointNo == (u32)(s32)i_this->m_neck_jnt_num) {
                mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-i_this->m_jnt.mAngles[0][0]);
            } else if (jointNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                PSMTXMultVec(mDoMtx_stack_c::get(), a_att_pos_offst, &i_this->mAttPos);
                PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_offst, &i_this->eyePos);
            } else if (jointNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-i_this->m_jnt.mAngles[1][0]);
            } else if (jointNo == (u32)(s32)i_this->m_body_ArmL) {
                PSMTXCopy(mDoMtx_stack_c::get(), &i_this->field_0x794);
            } else if (jointNo == (u32)(s32)i_this->m_body_ArmR) {
                PSMTXCopy(mDoMtx_stack_c::get(), &i_this->field_0x7C4);
            }
            PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jointNo), mDoMtx_stack_c::get()); /* model->setAnmMtx(jointNo, ...) */
        }
    }
    return TRUE;
}
VERIFY(0x0220CCD4, nodeCallBack);

/* 0220D134 */
static BOOL nodeCallBackArm(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0220D134, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C));
        daNpc_Bmsw_c* i_this = gabi::at<daNpc_Bmsw_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            /* static cXyz a_eff_pos_offst(0.0f, 0.0f, 0.0f) (unused): guard 0x104667F8, object 0x104666FC */
            cXyz* a_eff_pos_offst = gabi::at<cXyz>(0x104666FC);
            if (gabi::load<u32>(0x104667F8) == 0) {
                gabi::store<u32>(0x104667F8, 1);
                a_eff_pos_offst->x = 0.0f;
                a_eff_pos_offst->z = 0.0f;
                a_eff_pos_offst->y = 0.0f;
            }
            u32 jointNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
            PSMTXCopy(getAnmMtx(model, jointNo), mDoMtx_stack_c::get());
            if (jointNo == (u32)(s32)i_this->m_ArmL) {
                PSMTXCopy(&i_this->field_0x794, j3dSys_mCurrentMtx());
                mtx_copy(getAnmMtx(model, jointNo), &i_this->field_0x794);
            } else if (jointNo == (u32)(s32)i_this->m_ArmR) {
                PSMTXCopy(&i_this->field_0x7C4, j3dSys_mCurrentMtx());
                mtx_copy(getAnmMtx(model, jointNo), &i_this->field_0x7C4);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0220D134, nodeCallBackArm);

/* ---- create / execute / draw ---- */

/* 0220E340 */
void daNpc_Bmsw_c::set_mtx() {
    WWHD_FUNC(0x0220E340, void, this);
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(field_0x6D4, getAnmMtx(model, m_jnt.mHeadJntNum));
    J3DModel_setBaseTRMtx(field_0x6D8, getAnmMtx(model, m_jnt.mBackboneJntNum));
    s8 anm = field_0x9D5;
    if (anm == 5 || anm == 6) {
        /* (anm == 5 && frame < 18.0f + REG10_F(5)) || (anm == 6 && frame < 18.0f): HD has no REG10_F(5) */
        if (mpMorfHand->mFrameCtrl.mFrame < 18.0f) {
            PSMTXCopy(getAnmMtx(mpMorfHand->getModel(), m_handR), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(24.77f, -3.73f, 22.69f);
            /* cM_deg2s(98.02001654f), cM_deg2s(-61.737669014f), cM_deg2s(20.2807611f) */
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0x45B4, -0x2BE7, 0xE6C);
        } else {
            PSMTXCopy(getAnmMtx(mpMorfHand->getModel(), m_handL), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(26.18f, 8.72f, 18.55f);
            /* cM_deg2s(113.05480612f), cM_deg2s(57.128904507f), cM_deg2s(-179.296869528f) */
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0x5065, 0x28A0, -0x7F80);
        }
        J3DModel_setBaseTRMtx(field_0x6DC, mDoMtx_stack_c::get());
    }
}
VERIFY(0x0220E340, &daNpc_Bmsw_c::set_mtx);

/* 0220E6E8 */
BOOL daNpc_Bmsw_c::CreateInit() {
    WWHD_FUNC(0x0220E6E8, BOOL, this);
    /* field_0x97E = current.angle */
    gabi::store<u16>(gabi::ea(&field_0x97E) + 4, gabi::load<u16>(gabi::ea(&current.angle) + 4));
    gabi::store<u16>(gabi::ea(&field_0x97E) + 2, gabi::load<u16>(gabi::ea(&current.angle) + 2));
    gabi::store<u16>(gabi::ea(&field_0x97E), gabi::load<u16>(gabi::ea(&current.angle)));
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAA); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAA); /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    setAction_l(this, FN_wait_action);
    mAttPos.copy(current.pos);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BD3F4) /* l_cyl_src */);
    mCyl.SetStts(&mStts);
    setCollision(60.0f, 150.0f);
    field_0x9C4 = 0;
    mEventCut.setActorInfo2(STR(0x10018248) /* "Bmsw" */, this);
    field_0x962 = 0;
    field_0x963 = 0;
    field_0x9C0 = fpcM_ERROR_PROCESS_ID_e;
    STControl_setWaitParm(&field_0x988, 5, 2, 3, 2, 1.0f, 0.9f, 0, 0x2000);
    set_mtx();
    mpMorf->calc();
    mpMorfHand->calc();
    set_mtx(); /* HD: again after the animation update */
    dKy_tevstr_init(&field_0x6E0, home.roomNo, 0xFF);
    if (dComIfGs_isEventBit(0x2701)) {
        field_0x9E0 = fopAcM_createChild(STR(0x10018250) /* "Btsw" */, fopAcM_GetID(this), fopAcM_GetParam(this), &current.pos,
                                         fopAcM_GetRoomNo(this), &current.angle, nullptr, 0);
        /* *fopAcM_GetOldPosition_p(this) = cXyz(120.0f, 700.0f, 810.0f); current.pos = old.pos */
        gabi::Local<be<f32>> t;
        *t = 700.0f;
        u32 y = gabi::load<u32>(gabi::ea(t.get()));
        *t = 120.0f;
        u32 x = gabi::load<u32>(gabi::ea(t.get()));
        *t = 810.0f;
        u32 z = gabi::load<u32>(gabi::ea(t.get()));
        gabi::store<u32>(gabi::ea(&old.pos.y), y);
        gabi::store<u32>(gabi::ea(&old.pos.x), x);
        gabi::store<u32>(gabi::ea(&current.pos.z), z);
        gabi::store<u32>(gabi::ea(&current.pos.x), x);
        gabi::store<u32>(gabi::ea(&current.pos.y), y);
        gabi::store<u32>(gabi::ea(&old.pos.z), z);
    } else {
        field_0x9E0 = fpcM_ERROR_PROCESS_ID_e;
    }
    return TRUE;
}
VERIFY(0x0220E6E8, &daNpc_Bmsw_c::CreateInit);

/* 0220E9D8 */
cPhs_State daNpc_Bmsw_c::_create() {
    WWHD_FUNC(0x0220E9D8, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Bmsw_c) */
    u32 cond = actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            gabi::call(0x0220E0D4, this); /* daNpc_Bmsw_c::daNpc_Bmsw_c */
            cond = actor_condition;
        }
        actor_condition = cond | fopAcCnd_INIT_e;
    }
    if (dComIfGs_getEventReg(0xC203) >= 3 && !dComIfGs_isEventBit(0x2701)) {
        fopAcM_create_name(STR(0x10018258) /* "Btsw" */, fopAcM_GetParam(this), &current.pos, fopAcM_GetRoomNo(this), &current.angle,
                           nullptr, 0);
        return cPhs_ERROR_e;
    }
    cPhs_State res = dComIfG_resLoad(&mPhs, STR(0x10018260) /* "Bmsw" */);
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0220E0D0 /* CallbackCreateHeap */, 0xCDA0)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if (l_HIO().mNo < 0) {
            l_HIO().mNo = mDoHIO_createChild(STR(0x10018268) /* "リト族（仕分けゲーム）" */, &l_HIO());
        }
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return res;
}
VERIFY(0x0220E9D8, &daNpc_Bmsw_c::_create);

/* 0220EE44 */
BOOL daNpc_Bmsw_c::_execute() {
    WWHD_FUNC(0x0220EE44, BOOL, this);
    dNpc_HIO_l& n = l_HIO().mNpc;
    m_jnt.setParam(n.mMaxBackboneX, n.mMaxBackboneY, n.mMinBackboneX, n.mMinBackboneY, n.mMaxHeadX, n.mMaxHeadY, n.mMinHeadX,
                   n.mMinHeadY, n.mMaxTurnStep);
    STControl_setWaitParm(&field_0x988, 5, 2, 3, 2, l_HIO().field_0x50, l_HIO().field_0x54, 0, 0x2000);
    playTexPatternAnm();
    mpMorf->play(&eyePos, 0, 0);
    mpMorf->calc();
    mpMorfHand->play(nullptr, 0, 0);
    mpMorfHand->calc();
    checkOrder();
    if (!fopNpc_npc_c::mEventCut.cutProc()) {
        pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    }
    mSwCam.Move();
    eventOrder();
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    dBgS_Acch_CrrPos(&mObjAcch, dComIfG_Bgsp());
    u32 gnd = gabi::ea(&mObjAcch) + 0xD4 + 0x14; /* mObjAcch.m_gnd */
    gabi::store<s8>(gabi::ea(&tevStr) + 0xB9, gabi::call<s8>(0x024EF130, dComIfG_Bgsp(), gnd)); /* mRoomNo = GetRoomId */
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, gabi::call<u8>(0x024EEEB8, dComIfG_Bgsp(), gnd)); /* mEnvrIdxOverride = GetPolyColor */
    set_mtx();
    setCollision(60.0f, 150.0f);
    return TRUE;
}
VERIFY(0x0220EE44, &daNpc_Bmsw_c::_execute);

/* 0220F06C */
BOOL daNpc_Bmsw_c::_draw() {
    WWHD_FUNC(0x0220F06C, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* headModelData = J3DModel_getModelData_l(field_0x6D4);
    J3DModel* handModel = mpMorfHand->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), field_0x6D4, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), field_0x6D8, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), handModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)field_0x7F8, headModelData, field_0x80C);
    mpMorf->entryDL();
    mpMorfHand->entryDL();
    mDoExt_modelUpdateDL(field_0x6D4);
    mDoExt_modelUpdateDL(field_0x6D8);
    if (field_0x9D5 == 5 || field_0x9D5 == 6) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &field_0x6E0); /* HD: no REG10_S(9) */
        u32 t = gabi::ea(&field_0x6E0);
        gabi::store<u8>(t + 0x98, (u8)l_HIO().r_1); /* mColorK0 (HD +0x98) */
        gabi::store<u8>(t + 0x99, (u8)l_HIO().g_1);
        gabi::store<u8>(t + 0x9A, (u8)l_HIO().b_1);
        gabi::store<s16>(t + 0x90, l_HIO().r_2); /* mColorC0 (HD +0x90) */
        gabi::store<s16>(t + 0x92, l_HIO().g_2);
        gabi::store<s16>(t + 0x94, l_HIO().b_2);
        setLightTevColorType(dKy_getEnvlight(), field_0x6DC, &field_0x6E0);
        mDoExt_modelUpdateDL(field_0x6DC);
    }
    mDoExt_btpAnm_remove(headModelData);
    if (field_0x963 != 0) {
        for (s32 i = 0; i < 3; i++) {
            field_0x930[i]->draw(&tevStr);
        }
    }
    gabi::call(0x025BD338, mpShopCursor.get()); /* mpShopCursor->draw() */
    dSnap_RegistFig_l(0x96 /* DSNAP_TYPE_NPC_BMSW */, this, &current.pos, current.angle.y, 1.0f, 1.0f, 1.0f);
    /* HD debug display (l_HIO.m22): only function-local statics initialised on first use */
    if (l_HIO().mNpc.m22 != 0) {
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x10017FE8), 4);
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF8), gabi::at<u8>(0x10017FEC), 4);
        }
    }
    return TRUE;
}
VERIFY(0x0220F06C, &daNpc_Bmsw_c::_draw);

/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at +0x10
 * to the JUTNameTab), 027F3F94 (the matcher calls it __nw) the joint tree header (count u16 at +8) */
static inline s8 jointIndex(J3DModelData* d, const char* name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return gabi::call<s8>(0x027DF9B0, tab, name);
}
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
#define BMSW_RES STR(0x10018120) /* "Bmsw" */
#define BMSW_ASSERT(line, msg) JUT_ASSERT_fail(STR(0x1001815C), line, STR(msg))

/* 0220D98C */
BOOL daNpc_Bmsw_c::CreateHeap() {
    WWHD_FUNC(0x0220D98C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(BMSW_RES, 3 /* BDL_BM */);
    if (modelData == nullptr)
        BMSW_ASSERT(0x612, 0x10018170);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(BMSW_RES, 0x16 /* BCK_BM_TALK01 */);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0,
                                    0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr)
        return FALSE;
    m_jnt.mHeadJntNum = jointIndex(modelData, STR(0x10018128) /* "head" */);
    if (m_jnt.mHeadJntNum < 0)
        BMSW_ASSERT(0x621, 0x10018184);
    m_neck_jnt_num = jointIndex(modelData, STR(0x10018130) /* "neck" */);
    if (m_neck_jnt_num < 0)
        BMSW_ASSERT(0x623, 0x10018148);
    m_jnt.mBackboneJntNum = jointIndex(modelData, STR(0x100181A0) /* "backbone" */);
    m_body_ArmL = jointIndex(modelData, STR(0x10018138) /* "armL" */);
    m_body_ArmR = jointIndex(modelData, STR(0x10018140) /* "armR" */);
    if (!(m_body_ArmL >= 0 || m_body_ArmR >= 0))
        BMSW_ASSERT(0x627, 0x100181AC);

    J3DModelData* headModelData = (J3DModelData*)dComIfG_getObjectIDRes(BMSW_RES, 0x21 /* BDL_BMHEAD11 */);
    field_0x6D4 = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
    if (field_0x6D4.get() == nullptr)
        return FALSE;

    J3DModelData* armModelData = (J3DModelData*)dComIfG_getObjectIDRes(BMSW_RES, 4 /* BDL_BMARM */);
    J3DAnmTransform* armAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(BMSW_RES, 0 /* BCK_BMARM_TALK01 */);
    mpMorfHand = mDoExt_McaMorf::create(nullptr, armModelData, nullptr, nullptr, armAnm, 2, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (mpMorfHand.get() == nullptr || mpMorfHand->getModel() == nullptr)
        return FALSE;
    m_ArmL = jointIndex(armModelData, STR(0x10018100) /* "armLloc" */);
    m_ArmR = jointIndex(armModelData, STR(0x10018108) /* "armRloc" */);
    m_handL = jointIndex(armModelData, STR(0x10018110) /* "handL" */);
    m_handR = jointIndex(armModelData, STR(0x10018118) /* "handR" */);
    if (!(m_ArmL >= 0 || m_ArmR >= 0))
        BMSW_ASSERT(0x65C, 0x100181D4);
    if (!(m_handL >= 0 || m_handR >= 0))
        BMSW_ASSERT(0x65D, 0x100181F0);

    J3DModelData* bagModelData = (J3DModelData*)dComIfG_getObjectIDRes(BMSW_RES, 2 /* BDL_BM_BAG */);
    field_0x6D8 = mDoExt_J3DModel__create(bagModelData, 0, 0x11020203);
    if (field_0x6D8.get() == nullptr)
        return FALSE;
    J3DModelData* letterModelData = (J3DModelData*)dComIfG_getObjectIDRes(BMSW_RES, 0x1C /* BDL_BM_LETTER */);
    field_0x6DC = mDoExt_J3DModel__create(letterModelData, 0, 0x11020203);
    if (field_0x6DC.get() == nullptr)
        return FALSE;

    field_0x9D4 = 0;
    if (!initTexPatternAnm(false))
        return FALSE;

    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum || i == (u32)(s32)m_neck_jnt_num ||
            i == (u32)(s32)m_body_ArmL || i == (u32)(s32)m_body_ArmR) {
            setJointCallBack(J3DModel_getModelData_l(mpMorf->getModel()), i, 0x0220CCD4 /* nodeCallBack */);
        }
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(armModelData); i++) {
        if (i == (u32)(s32)m_ArmR || i == (u32)(s32)m_ArmL) {
            setJointCallBack(armModelData, i, 0x0220D134 /* nodeCallBackArm */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this));     /* setUserArea */
    gabi::store<u32>(gabi::ea(mpMorfHand->getModel()) + 0xB8, gabi::ea(this));

    field_0x930[0] = &mSwMail0;
    field_0x930[1] = &mSwMail1;
    field_0x930[2] = &mSwMail2;
    field_0x93C = 0;
    cXyz* cameraCenter = &mSwCam.mCenter;
    cXyz* eyeP = &mSwCam.mEye;
    if (!field_0x930[0]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;
    if (!field_0x930[1]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;
    if (!field_0x930[2]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;

    void* tevRegKey = dComIfG_getObjectIDRes(BMSW_RES, 0x19 /* BRK_SHOP_CURSOR01 */);
    void* cursorModelData = dComIfG_getObjectIDRes(BMSW_RES, 0x18 /* BMD_SHOP_CURSOR01 */);
    mpShopCursor = gabi::call<ShopCursor_c*>(0x025BBD7C, cursorModelData, tevRegKey, 0.65f); /* ShopCursor_create */
    if (mpShopCursor.get() == nullptr)
        return FALSE;
    gabi::store<u8>(gabi::ea(mpShopCursor.get()) + 0xB4, 0); /* mpShopCursor->m54 (HD +0xB4) */

    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x0220D98C, &daNpc_Bmsw_c::CreateHeap);

/* ---- constructor / destructor ---- */

/* 0220E0D4 daNpc_Bmsw_c::daNpc_Bmsw_c (HD: allocates when this == NULL) */
static daNpc_Bmsw_c* daNpc_Bmsw_c_ct(daNpc_Bmsw_c* i_this) {
    WWHD_FUNC(0x0220E0D4, daNpc_Bmsw_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Bmsw_c*)operator_new(0xD84);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x025A1458, i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = BMSW_VTBL;
    /* dKy_tevstr_c field_0x6E0 (HD, inline constructor): three 0x44-byte light blocks at +0, +0xC0
     * and +0x144 initialised from the template at 0x1016E414 (floats, then bytes and shorts) */
    static const u32 blk[3] = {0x7F8, 0x8B8, 0x93C};
    const u32 T = 0x1016E414;
    for (int b = 0; b < 3; b++) {
        u32 d = gabi::ea(i_this) + blk[b];
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d + o, gabi::load<u8>(T + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(d + o, gabi::load<s16>(T + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
    }
    gabi::call(0x025E7820, i_this->field_0x7F8);           /* mDoExt_btpAnm::mDoExt_btpAnm */
    gabi::call(0x025E7820, i_this->mSwMail0.field_0x10);
    gabi::call(0x025E7820, i_this->mSwMail1.field_0x10);
    gabi::call(0x025E7820, i_this->mSwMail2.field_0x10);
    /* SwCam_c::SwCam_c() */
    i_this->mSwCam.mActive = 0;
    i_this->mSwCam.field_0x1C = 0;
    i_this->mSwCam.field_0x1D = 0;
    i_this->mSwCam.mCenter.copy(*SwCam_camera_center_data(0, 1));
    i_this->mSwCam.mEye.copy(*SwCam_camera_eye());
    i_this->mSwCam.mFovY = 58.0f;
    /* STControl::STControl(15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000) */
    i_this->field_0x988.__vtbl = 0x10050788;
    STControl_setWaitParm(&i_this->field_0x988, 15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    gabi::call(0x025885E8, &i_this->field_0x988); /* STControl::init */
    return i_this;
}
VERIFY(0x0220E0D4, daNpc_Bmsw_c_ct);

/* 02211F5C daNpc_Bmsw_c::~daNpc_Bmsw_c (deleting; fopNpc_npc_c's destructor inline) */
static void daNpc_Bmsw_c_dt(daNpc_Bmsw_c* i_this, s32 flags) {
    WWHD_FUNC(0x02211F5C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025886D8, &i_this->field_0x988, 2);                 /* STControl::~STControl */
        gabi::call(0x02515A70, &i_this->mCyl, 2);                        /* dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
        gabi::call(0x02515860, &i_this->mStts, 2);                       /* dCcD_Stts::~dCcD_Stts */
        gabi::call(0x02018034, gabi::ea(i_this) + 0x628, 2);             /* mAcchCir's cM3dGCir */
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): vtables, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(i_this) + 0x470, 0x1001801C);
        gabi::store<u32>(gabi::ea(i_this) + 0x464, 0x1001802C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0);                               /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x02211F5C, daNpc_Bmsw_c_dt);

/* ---- the sorting game ---- */

/* mini game 7 (HD): play+0x5CE8 flags (u16, bit 0x40), +0x5CEA type (u8), +0x5CEE (u8); HD also
 * sets the byte at 0x1047B09A and notifies an object at *(*(0x101F8344) + 0x178) */
static inline void dComIfGp_startMiniGame7() {
    u32 p = dComIfGp_ea();
    gabi::store<u8>(p + 0x5CEA, 7);
    gabi::store<u16>(p + 0x5CE8, (u16)(gabi::load<u16>(p + 0x5CE8) | 0x40));
    gabi::store<u8>(0x1047B09A, 1);
    gabi::call(0x02676900, gabi::load<u32>(gabi::load<u32>(0x101F8344) + 0x178));
}
static inline void dComIfGp_endMiniGame7() {
    u32 p = dComIfGp_ea();
    u16 f = gabi::load<u16>(p + 0x5CE8);
    gabi::store<u8>(p + 0x5CEA, 0);
    gabi::store<u8>(p + 0x5CEE, 0);
    gabi::store<u16>(p + 0x5CE8, (u16)(f ^ 0x40));
    gabi::store<u8>(0x1047B09A, 0);
    gabi::call(0x026768E4, gabi::load<u32>(gabi::load<u32>(0x101F8344) + 0x178));
}
/* dComIfG_getTimerPtr(): play+0x5CF0; the timer mode play+0x5CFC */
static inline u32 dComIfG_getTimerPtr() { return gabi::load<u32>(dComIfGp_ea() + 0x5CF0); }
/* HD input (replaces the GameCube STControl): the controller object at *(0x101F5088); triggers
 * +0x18 | +0x20, while +0x124 has a bit of 0x00F00000 set only those bits count */
static inline u32 hdInput() { return gabi::load<u32>(0x101F5088); }

/* daNpc_Bmsw_getGameEndMsg (inline in shiwake_game_action) */
static inline u32 daNpc_Bmsw_getGameEndMsg(s16 rupees) {
    u32 msgNo;
    switch (dComIfGs_getEventReg(0xC203)) {
    case 0:
        if (rupees == 0) msgNo = 0x1A3A;
        else if (rupees == 1) msgNo = 0x1A3B;
        else if (rupees < 10) msgNo = 0x1A3C;
        else msgNo = 0x1A37;
        break;
    case 1:
        if (rupees == 0) msgNo = 0x1A4F;
        else if (rupees == 1) msgNo = 0x1A50;
        else if (rupees < 20) msgNo = 0x1A52;
        else msgNo = 0x1A4C;
        break;
    case 2:
        if (rupees == 0) msgNo = 0x1A5C;
        else if (rupees == 1) msgNo = 0x1A5D;
        else if (rupees < 25) msgNo = 0x1A60;
        else msgNo = 0x1A57;
        break;
    case 3:
    default:
        if (dComIfGs_isEventBit(0x2701)) {
            if (rupees == 0) {
                msgNo = 0x1A69;
            } else if (rupees == 1) {
                msgNo = 0x1A6A;
            } else if (rupees <= (s32)dComIfGs_getEventReg(0x8AFF)) {
                msgNo = 0x1A6B;
            } else {
                dComIfGs_setEventReg(0x8AFF, (u8)rupees);
                msgNo = 0x1A6D;
            }
        } else {
            msgNo = 0x1A62;
        }
        break;
    }
    return msgNo;
}

/* 02210F60 */
BOOL daNpc_Bmsw_c::shiwake_game_action(void*) {
    WWHD_FUNC(0x02210F60, BOOL, this, (void*)nullptr);
    /* static cXyz aim_pos_data[2][3]: guard 0x104667FC, object 0x10466760 */
    if (gabi::load<u32>(0x104667FC) == 0) {
        gabi::store<u32>(0x104667FC, 1);
        static const f32 a[6][3] = {{-40.0f, 772.0f, 1035.0f}, {-139.0f, 772.0f, 1035.0f}, {-240.0f, 772.0f, 1035.0f},
                                    {-40.0f, 855.0f, 1035.0f}, {-139.0f, 855.0f, 1035.0f}, {-240.0f, 855.0f, 1035.0f}};
        for (int i = 0; i < 6; i++) gabi::at<cXyz>(0x10466760 + i * 0xC)->set(a[i][0], a[i][1], a[i][2]);
    }
    /* static cXyz cursor_pos_data[2][3]: guard 0x10466800, object 0x104667A8 */
    if (gabi::load<u32>(0x10466800) == 0) {
        static const f32 c[6][3] = {{-52.0f, 804.0f, 996.0f}, {-140.0f, 804.0f, 996.0f}, {-227.0f, 804.0f, 996.0f},
                                    {-52.0f, 889.0f, 996.0f}, {-140.0f, 889.0f, 996.0f}, {-227.0f, 889.0f, 996.0f}};
        for (int i = 0; i < 6; i++) gabi::at<cXyz>(0x104667A8 + i * 0xC)->set(c[i][0], c[i][1], c[i][2]);
        gabi::store<u32>(0x10466800, 1);
    }
#define AIM_POS(row, col) gabi::at<cXyz>(0x10466760 + ((row) * 3 + (col)) * 0xC)
#define CURSOR_POS(row, col) gabi::at<cXyz>(0x104667A8 + ((row) * 3 + (col)) * 0xC)

    if (mActionStatus == ACTION_STARTING) {
        field_0x9B0 = 0;
        field_0x9DA = 3;
        field_0x9B1 = 0;
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            BOOL two = dComIfGs_isEventBit(0x2701);
            /* getEventIdx's result register is passed on as is (typed u32) */
            u32 idx;
            if (!two) {
                idx = gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), STR(0x10018324) /* "SHIWAKEGAME" */, 0xFF);
            } else {
                idx = gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), STR(0x10018330) /* "SHIWAKEGAME2" */, 0xFF);
            }
            field_0x9D8 = (s16)idx;
            gabi::call<BOOL>(0x025D7A58, this, idx, 0xFF, (u16)0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
            eventInfo_onCondition(this, 2); /* dEvtCnd_UNK2_e */
            return FALSE;
        }
        if (!field_0x962) {
            /* dTimer_createTimer(6, limit, 3, 4, 221.0f, field_0x48, 32.0f, field_0x4C) */
            gabi::call(0x025C60F4, 6, (u16)l_HIO().field_0x30, 3, 4, 221.0f, (f32)l_HIO().field_0x48, 32.0f, (f32)l_HIO().field_0x4C);
            field_0x962 = 1;
            gabi::store<s16>(dComIfGp_ea() + 0x5CEC, 0); /* dComIfGp_setMiniGameRupee(0) */
            dComIfGp_startMiniGame7();
        }
        if (dComIfG_getTimerPtr() == 0) {
            return FALSE;
        }
        /* dComIfG_TimerStart(6, field_0x32) (HD inline) */
        s16 limit = l_HIO().field_0x32;
        if (gabi::load<s32>(dComIfGp_ea() + 0x5CFC) == 6) {
            u32 timer = dComIfG_getTimerPtr();
            if (timer != 0) {
                if (limit == 0) {
                    gabi::call(0x025C5864, timer); /* dTimer_c::start() */
                } else {
                    gabi::call(0x025C6250, timer, limit); /* dTimer_c::start(s16) */
                }
            }
        }
        field_0x963 = 1;
        field_0x930[0]->init();
        field_0x930[0]->AppearInit();
        field_0x930[1]->init();
        field_0x930[1]->DummyInit();
        field_0x930[2]->init();
        field_0x930[2]->DummyInit();
        s32 row = field_0x9B1;
        s32 col = field_0x9B0;
        field_0x93C = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        gabi::call(0x025BD31C, mpShopCursor.get(), CURSOR_POS(row, col + 1)); /* mpShopCursor->setPos() */
        gabi::store<u8>(gabi::ea(mpShopCursor.get()) + 0xB4, 1);             /* mpShopCursor->show() */
        field_0x9B4 = 20000;
        gabi::call(0x02618760, hdInput(), 5, 5); /* HD: cursor repeat on */
        return TRUE;
    } else if (mActionStatus == ACTION_ENDING) {
        field_0x962 = 0;
        field_0x963 = 0;
        gabi::call(0x02618774, hdInput()); /* HD: cursor repeat off */
        return TRUE;
    }

    if (dComIfG_getTimerRestTimeMs() > 0) {
        TimerCountDown();
        /* HD: the controller triggers replace STControl (field_0x988.checkTrigger()) */
        u32 in = hdInput();
        s8 x = field_0x9B0;
        u32 trig = gabi::load<u32>(in + 0x18) | gabi::load<u32>(in + 0x20);
        if (gabi::load<u32>(in + 0x124) & 0x00F00000)
            trig &= 0x00F00000;
        if (x > -1 && (trig & 0x00440000)) { /* left */
            mDoAud_seStart_id(0x8B0);          /* JA_SE_LETTER_GAME_CURSOR */
            field_0x9B0 = (s8)(field_0x9B0 - 1);
        } else if (x < 1 && (trig & 0x00880000)) { /* right */
            mDoAud_seStart_id(0x8B0);
            field_0x9B0 = (s8)(field_0x9B0 + 1);
        }
        if (field_0x9B1 < 1 && (trig & 0x00110000)) { /* up */
            mDoAud_seStart_id(0x8B0);
            field_0x9B1 = (s8)(field_0x9B1 + 1);
        } else if (field_0x9B1 > 0 && (trig & 0x00220000)) { /* down */
            mDoAud_seStart_id(0x8B0);
            field_0x9B1 = (s8)(field_0x9B1 - 1);
        }
        if (pmf_is(&field_0x930[field_0x93C]->mFunc, FN_SwMail_Wait) &&
            gabi::load<u8>(dComIfG_getTimerPtr() + 0x124) != 1 /* dComIfG_getTimerPtr()->getStatus() */ &&
            gabi::call<BOOL>(0x02007898, 0) /* CPad_CHECK_TRIG_A(0) */ && checkNextMailThrowOK()) {
            s32 col = field_0x9B0;
            s32 row = field_0x9B1;
            u8 idx = (u8)(col - row * 3 + 4); /* (col + 1) + (1 - row) * 3 */
            SwMail_c* mail = field_0x930[field_0x93C];
            cXyz* src = AIM_POS(row, col + 1);
            gabi::Local<cXyz> aim_pos;
            aim_pos->set(src->x, src->y, src->z);
            mail->ThrowInit(aim_pos, idx);
            if (field_0x93C < 2) {
                field_0x93C = (u8)(field_0x93C + 1);
            } else {
                field_0x93C = 0;
            }
            mDoAud_seStart_id(0x8B1); /* JA_SE_LETTER_GAME_OK */
            field_0x930[field_0x93C]->AppearInit();
        }
        s8 row = field_0x9B1;
        s8 col = field_0x9B0;
        mSwCam.field_0x1D = row; /* mSwCam.setAimIdx(field_0x9B0, field_0x9B1) */
        mSwCam.field_0x1C = col;
        gabi::call(0x025BD31C, mpShopCursor.get(), CURSOR_POS(row, col + 1));
        /* mpShopCursor->setScale(0.65f, 0.9f, 0.5f, 27.0f, 20.0f) */
        u32 cur = gabi::ea(mpShopCursor.get());
        gabi::store<f32>(cur + 0x98, 27.0f);
        gabi::store<f32>(cur + 0xA8, 0.65f);
        gabi::store<f32>(cur + 0x9C, 20.0f);
        gabi::store<f32>(cur + 0xB0, 0.5f);
        gabi::store<f32>(cur + 0xAC, 0.9f);
        gabi::call(0x025BD290, mpShopCursor.get()); /* anm_play() */
    } else if (mActionStatus == ACTION_ONGOING) {
        mDoAud_seStart_id(0x8AF); /* JA_SE_LETTER_GAME_TIMER_0 */
        gabi::call(0x025C58A8, dComIfG_getTimerPtr(), (s32)l_HIO().field_0x34); /* dTimer_c::end */
        gabi::store<u8>(gabi::ea(mpShopCursor.get()) + 0xB4, 0);            /* hide() */
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus == ACTION_UNK_2) {
        if (gabi::call<BOOL>(0x025C6278, dComIfG_getTimerPtr()) /* deleteCheck() */) {
            gabi::call(0x025C58D8, dComIfG_getTimerPtr()); /* deleteRequest() */
            mActionStatus = (s8)(mActionStatus + 1);
        }
    } else if (dComIfG_getTimerPtr() == 0) {
        s32 staff_id = dComIfGp_evmng_getMyStaffId(STR(0x1001831C) /* "Bmsw" */, nullptr, 0);
        dComIfGp_evmng_cutEnd(staff_id);
        if (dComIfGp_evmng_endCheck(field_0x9D8)) {
            dComIfGp_event_reset();
            s16 rupees = gabi::load<s16>(dComIfGp_ea() + 0x5CEC); /* dComIfGp_getMiniGameRupee() */
            field_0x9C0 = daNpc_Bmsw_getGameEndMsg(rupees);
            setGameGetRupee(rupees);
            field_0x9D7 = 1;
            setAction_l(this, FN_wait_action);
            mSwCam.mActive = 0; /* ActiveOff() */
            if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 7) { /* dComIfGp_getMiniGameType() */
                dComIfGp_endMiniGame7();
            }
        }
    }
    field_0x930[0]->move();
    field_0x930[1]->move();
    field_0x930[2]->move();
#undef AIM_POS
#undef CURSOR_POS
    return TRUE;
}
VERIFY(0x02210F60, &daNpc_Bmsw_c::shiwake_game_action);
