/**
 * d_a_hys.cpp (WWHD)
 * Object - Eye switch (arrow target).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hys.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x100117D8
#define HYS_VTBL 0x1001186C /* HD: daHys_c vtable */
#define l_sph_src gabi::at<dCcD_SrcSph>(0x101B77C4)
#define mode_proc 0x10011820u /* static const ModeFunc mode_proc[2] (pointers to members, 8 bytes each) */

/* static const tables (indexed with mType) */
#define m_arcname(t) STR(gabi::load<u32>(0x101B7848 + 4 * (t))) /* "Hys", "Hys" (.data) */
#define m_bdlidx(t) gabi::load<s16>(0x10011860 + 2 * (t))
#define m_btpidx(t) gabi::load<s16>(0x10011864 + 2 * (t))
#define m_dzbidx(t) gabi::load<s16>(0x10011868 + 2 * (t))
#define m_heapsize(t) gabi::load<u32>(0x10011850 + 4 * (t))
#define m_tg_r(t) gabi::load<f32>(0x10011858 + 4 * (t))

enum { JA_SE_OBJ_ARROW_SW_ON = 0x695E };
/* AT_TYPE_NORMAL_ARROW | AT_TYPE_FIRE_ARROW | AT_TYPE_ICE_ARROW | AT_TYPE_LIGHT_ARROW */
enum : u32 { AT_TYPE_ARROWS = 0x1C4000 };

struct daHys_c : dBgS_MoveBgActor {
    cPhs_State _create();
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Execute(Mtx34** mtx);
    void mode_proc_call();
    void mode_wait();
    void mode_sw_on_init();
    void mode_sw_on();
    void mode_wait_init();
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs; /* GameCube 0x2C8 (+0x118) */
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74, GameCube 0x14) */
    /* 0x460 */ dCcD_Stts mStts;                     /* GameCube 0x2E8 */
    /* 0x49C */ dCcD_Sph mSph;
    /* 0x5C8 */ be<u32> field_0x450;
    /* 0x5CC */ be<u32> mSwitchNo;
    /* 0x5D0 */ be<u8> field_0x458;
    /* 0x5D1 */ be<u8> mType;
    /* 0x5D2 */ u8 _5D2[2];
};
WWHD_OFFSET(daHys_c, mStts, 0x460);
WWHD_OFFSET(daHys_c, mSph, 0x49C);
WWHD_OFFSET(daHys_c, field_0x450, 0x5C8);
WWHD_OFFSET(daHys_c, mType, 0x5D1);
WWHD_SIZE(daHys_c, 0x5D4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E7820 mDoExt_btpAnm::mDoExt_btpAnm (HD 0x74) */
static inline void mDoExt_btpAnm_ct(void* a) { gabi::call(0x025E7820, a); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry (stack))
 * (local in npc_people / npc_photo) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, s32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 025166F0 dCcD_Sph::dCcD_Sph (out of line in HD) */
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
/* 02515AE8 dCcD_Sph::~dCcD_Sph (out of line) */
static inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
static inline void fopAcM_onSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_onSwitch(sw, a->home.roomNo); }

/* 0217AADC */
BOOL daHys_c::Delete() {
    WWHD_FUNC(0x0217AADC, BOOL, this);
    dComIfG_resDelete(&mPhs, m_arcname(mType)); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x0217AADC, &daHys_c::Delete);

/* 0217A768 */
BOOL daHys_c::CreateHeap() {
    WWHD_FUNC(0x0217A768, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname(mType), m_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(262, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100117F4), 0x106, STR(0x1001180C));

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020022);
    if (mpModel == nullptr) {
        return FALSE;
    }

    J3DAnmTexPattern* pbtp = (J3DAnmTexPattern*)dComIfG_getObjectRes(m_arcname(mType), m_btpidx(mType), SAFESTRING_VTBL);
    if (pbtp == nullptr) /* JUT_ASSERT(276, pbtp != NULL) */
        JUT_ASSERT_fail(STR(0x100117F4), 0x114, STR(0x10011800));

    /* mBtpAnm.init(modelData, pbtp, FALSE, J3DFrameCtrl::EMode_NONE) */
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, pbtp, FALSE, 0, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    field_0x458 = 0;
    return TRUE;
}
VERIFY(0x0217A768, &daHys_c::CreateHeap);

/* inlined into daHys_Create */
cPhs_State daHys_c::_create() {
    /* fopAcM_ct(this, daHys_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = HYS_VTBL;
            mDoExt_btpAnm_ct(mBtpAnm);
            dCcD_Stts_ct(&mStts);
            dCcD_Sph_ct(&mSph);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    u32 prm = fopAcM_GetParam(this);
    mType = prm >> 8;
    /* HD: here the index is masked to the two-entry tables (mType & 1) */
    cPhs_State res = dComIfG_resLoad(&mPhs, m_arcname((prm >> 8) & 1));
    if (res == cPhs_COMPLEATE_e) {
        u32 t = mType & 1;
        res = MoveBGCreate(m_arcname(t), m_dzbidx(t), 0x024EE708 /* dBgS_MoveBGProc_TypicalRotY */, m_heapsize(t));
        if (res == cPhs_ERROR_e) {
            return cPhs_ERROR_e;
        }
    }

    return res;
}

/* 0217AB18: set_mtx() inlined */
BOOL daHys_c::Create() {
    WWHD_FUNC(0x0217AB18, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -240.0f, -240.0f, -90.0f, 240.0f, 240.0f, 90.0f);

    mStts.Init(255, 255, this);
    mSph.Set(l_sph_src);
    mSph.SetStts(&mStts);
    mSph.SetR(m_tg_r(mType));

    /* set_mtx() */
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx);

    mSwitchNo = fopAcM_GetParam(this) & 0xFF;

    if (fopAcM_isSwitch(this, mSwitchNo)) {
        field_0x450 = 1;
        field_0x458 = 3;
    } else {
        field_0x450 = 0;
        field_0x458 = 0;
    }

    if (mType == 1) {
        /* scale.setall(2.0f) */
        scale.x = 2.0f;
        scale.z = 2.0f;
        scale.y = 2.0f;
    }

    return TRUE;
}
VERIFY(0x0217AB18, &daHys_c::Create);

/* 0217A96C */
BOOL daHys_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x0217A96C, BOOL, this, mtx);
    mode_proc_call();

    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx); /* cMtx_copy */

    gabi::store<u32>(gabi::ea(mtx), gabi::ea(&mBgMtx));
    return TRUE;
}
VERIFY(0x0217A96C, &daHys_c::Execute);

/* 0217A8B4 */
void daHys_c::mode_proc_call() {
    WWHD_FUNC(0x0217A8B4, void, this);
    /* (this->*mode_proc[field_0x450])(): { &daHys_c::mode_wait, &daHys_c::mode_sw_on } */
    ptmf_call(mode_proc + (u32)field_0x450 * 8, this);
    mSph.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mSph);
}
VERIFY(0x0217A8B4, &daHys_c::mode_proc_call);

/* 0217ADF8 */
void daHys_c::mode_wait() {
    WWHD_FUNC(0x0217ADF8, void, this);
    if (mSph.ChkTgHit()) {
        void* obj = mSph.GetTgHitObj();
        /* ChkAtType(NORMAL) || ChkAtType(FIRE) || ChkAtType(ICE) || ChkAtType(LIGHT): one mask test */
        if (obj != nullptr && cCcD_Obj_ChkAtType(obj, AT_TYPE_ARROWS)) {
            mSph.ClrTgHit();
            mode_sw_on_init();
            return;
        }
    }

    if (field_0x458 != 0) {
        field_0x458 = field_0x458 - 1;
    }
}
VERIFY(0x0217ADF8, &daHys_c::mode_wait);

/* 0217AD8C */
void daHys_c::mode_sw_on_init() {
    WWHD_FUNC(0x0217AD8C, void, this);
    fopAcM_onSwitch(this, mSwitchNo);
    field_0x450 = 1;
    fopAcM_seStart(this, JA_SE_OBJ_ARROW_SW_ON, 0);
}
VERIFY(0x0217AD8C, &daHys_c::mode_sw_on_init);

/* 0217AD10 (not named by the matcher) */
void daHys_c::mode_sw_on() {
    WWHD_FUNC(0x0217AD10, void, this);
    if (!fopAcM_isSwitch(this, mSwitchNo)) {
        mode_wait_init();
    } else {
        if (field_0x458 < 3) {
            field_0x458 = field_0x458 + 1;
        }
    }
}
VERIFY(0x0217AD10, &daHys_c::mode_sw_on);

/* 0217AD04 (not named by the matcher) */
void daHys_c::mode_wait_init() {
    WWHD_FUNC(0x0217AD04, void, this);
    field_0x450 = 0;
}
VERIFY(0x0217AD04, &daHys_c::mode_wait_init);

/* 0217AA6C */
BOOL daHys_c::Draw() {
    WWHD_FUNC(0x0217AA6C, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)mBtpAnm, J3DModel_getModelData(mpModel), field_0x458);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}
VERIFY(0x0217AA6C, &daHys_c::Draw);

/* 0217A654 */
static cPhs_State daHys_Create(void* i_this) {
    WWHD_FUNC(0x0217A654, cPhs_State, i_this);
    return ((daHys_c*)i_this)->_create();
}
VERIFY(0x0217A654, daHys_Create);

/* 0217A750 */
static BOOL daHys_Delete(void* i_this) {
    WWHD_FUNC(0x0217A750, BOOL, i_this);
    return ((daHys_c*)i_this)->MoveBGDelete();
}
VERIFY(0x0217A750, daHys_Delete);

/* 0217A754: MoveBGDraw is the virtual Draw */
static BOOL daHys_Draw(void* i_this) {
    WWHD_FUNC(0x0217A754, BOOL, i_this);
    return ((daHys_c*)i_this)->Draw_v();
}
VERIFY(0x0217A754, daHys_Draw);

/* 0217A764 */
static BOOL daHys_Execute(void* i_this) {
    WWHD_FUNC(0x0217A764, BOOL, i_this);
    return ((daHys_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0217A764, daHys_Execute);

/* 0217AF24 */
static BOOL daHys_IsDelete(void*) {
    WWHD_FUNC(0x0217AF24, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0217AF24, daHys_IsDelete);

/* ---- compiler-generated (no GameCube source) ---- */
/* 0217AE90 */
static void __sinit_d_a_hys_cpp() {
    WWHD_FUNC(0x0217AE90, void, (u32)0);
    sinit_header_statics(0x1046477C, 0x101B7824);
}
VERIFY(0x0217AE90, __sinit_d_a_hys_cpp);

/* 0217AF2C: sead::SafeString deleting destructor (SafeString vtable +0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0217AF2C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0217AF2C, SafeString_dt);

/* 0217AF40: dBgS_MoveBgActor::IsDelete (this TU's copy, vtable +0x3C) */
static BOOL MoveBgActor_IsDelete(daHys_c*) {
    WWHD_FUNC(0x0217AF40, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0217AF40, MoveBgActor_IsDelete);

/* 0217AF48: daHys_c deleting destructor (vtable +0xC); mDoExt_btpAnm has no destructor */
static void daHys_c_dt(daHys_c* i_this, s32 flags) {
    WWHD_FUNC(0x0217AF48, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Sph_dt(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0217AF48, daHys_c_dt);

/* 0217AFB4: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x0217AFB4, void, (u32)0);
}
VERIFY(0x0217AFB4, SafeString_assureTerminationImpl);
