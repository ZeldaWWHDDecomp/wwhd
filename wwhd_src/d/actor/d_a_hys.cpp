/**
 * d_a_hys.cpp (WWHD)
 * Object - Eye switch (arrow target).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hys.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_hys.h"

const gc_tu_data gc_tu = {.safeString = 0x100117D8};
// HD: checked archive indices and packed collision source match this image.
static const char* m_arcname[] = { "Hys", "Hys" };
static const s16 m_bdlidx[] = { 5, 5 };
static const s16 m_btpidx[] = { 8, 8 };
static const s16 m_dzbidx[] = { 11, 11 };
static const u32 m_heapsize[] = { 0xA00, 0xA00 };
static const f32 m_tg_r[] = { 35.0f, 70.0f };
static const u32 l_sph_src[] = {0, 0, 0, 0, 1851392, 9, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1106247680};
static daHys_c::ModeEntry mode_proc[] = {{&daHys_c::mode_wait}, {&daHys_c::mode_sw_on}};
enum { JA_SE_OBJ_ARROW_SW_ON = 0x695E, dBgS_MoveBGProc_TypicalRotY = 0x024EE708 };
// HD: the four arrow attack types are tested with one mask.
enum : u32 { AT_TYPE_ARROWS = 0x1C4000 };

// HD: inline constructors, with this TU's actor virtual destructor table.
daHys_c::daHys_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x1001186C;
    Hys_btpConstruct(&mBtpAnm);
    dCcD_Stts_ct(&mStts);
    Hys_sphConstruct(&mSph);
}

/* 0217AADC */
BOOL daHys_c::Delete() {
    dComIfG_resDelete(&mPhs, m_arcname[mType]); /* dComIfG_resDeleteDemo */
    return TRUE;
}

/* 0217A768 */
BOOL daHys_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname[mType], m_bdlidx[mType]);
    JUT_ASSERT(262, modelData != nullptr);

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020022);
    if (mpModel == nullptr) {
        return FALSE;
    }

    J3DAnmTexPattern* pbtp = (J3DAnmTexPattern*)dComIfG_getObjectRes(m_arcname[mType], m_btpidx[mType]);
    JUT_ASSERT(276, pbtp != nullptr);

    /* mBtpAnm.init(modelData, pbtp, FALSE, J3DFrameCtrl::EMode_NONE) */
    if (!mBtpAnm.init(modelData, pbtp, FALSE, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    field_0x458 = 0;
    return TRUE;
}

/* inlined into daHys_Create */
cPhs_State daHys_c::_create() {
    fopAcM_ct(this, daHys_c);

    u32 prm = fopAcM_GetParam(this);
    mType = prm >> 8;
    /* HD: here the index is masked to the two-entry tables (mType & 1) */
    cPhs_State res = dComIfG_resLoad(&mPhs, m_arcname[(prm >> 8) & 1]);
    if (res == cPhs_COMPLEATE_e) {
        u32 t = mType & 1;
        res = MoveBGCreate(m_arcname[t], m_dzbidx[t], dBgS_MoveBGProc_TypicalRotY, m_heapsize[t]);
        if (res == cPhs_ERROR_e) {
            return cPhs_ERROR_e;
        }
    }

    return res;
}

// HD: set_mtx() inlined
/* 0217AB18 */
BOOL daHys_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -240.0f, -240.0f, -90.0f, 240.0f, 240.0f, 90.0f);

    mStts.Init(255, 255, this);
    mSph.Set(reinterpret_cast<const dCcD_SrcSph*>(l_sph_src));
    mSph.SetStts(&mStts);
    mSph.SetR(m_tg_r[mType]);

    /* set_mtx() */
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
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

/* 0217A96C */
BOOL daHys_c::Execute(gptr<Mtx34>* mtx) {
    mode_proc_call();

    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx); /* cMtx_copy */

    *mtx = &mBgMtx;
    return TRUE;
}

/* 0217A8B4 */
void daHys_c::mode_proc_call() {
    /* (this->*mode_proc[field_0x450])(): { &daHys_c::mode_wait, &daHys_c::mode_sw_on } */
    (this->*mode_proc[field_0x450].proc)();
    mSph.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mSph);
}

/* 0217ADF8 */
void daHys_c::mode_wait() {
    if (mSph.ChkTgHit()) {
        dCcD_GObjInf* obj = static_cast<dCcD_GObjInf*>(mSph.GetTgHitObj());
        /* ChkAtType(NORMAL) || ChkAtType(FIRE) || ChkAtType(ICE) || ChkAtType(LIGHT): one mask test */
        if (obj != nullptr && obj->ChkAtType(AT_TYPE_ARROWS)) {
            mSph.ClrTgHit();
            mode_sw_on_init();
            return;
        }
    }

    if (field_0x458 != 0) {
        field_0x458 = field_0x458 - 1;
    }
}

/* 0217AD8C */
void daHys_c::mode_sw_on_init() {
    dComIfGs_onSwitch(mSwitchNo, home.roomNo);
    field_0x450 = 1;
    fopAcM_seStart(this, JA_SE_OBJ_ARROW_SW_ON, 0);
}

// HD: (not named by the matcher)
/* 0217AD10 */
void daHys_c::mode_sw_on() {
    if (!fopAcM_isSwitch(this, mSwitchNo)) {
        mode_wait_init();
    } else {
        if (field_0x458 < 3) {
            field_0x458 = field_0x458 + 1;
        }
    }
}

// HD: (not named by the matcher)
/* 0217AD04 */
void daHys_c::mode_wait_init() {
    field_0x450 = 0;
}

/* 0217AA6C */
BOOL daHys_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBtpAnm.entry(mpModel->getModelData(), field_0x458);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 0217A654 */
static cPhs_State daHys_Create(void* i_this) {
    return ((daHys_c*)i_this)->_create();
}

/* 0217A750 */
static BOOL daHys_Delete(void* i_this) {
    return ((daHys_c*)i_this)->MoveBGDelete();
}

// HD: MoveBGDraw is the virtual Draw
/* 0217A754 */
static BOOL daHys_Draw(void* i_this) {
    return ((daHys_c*)i_this)->Draw_v();
}

/* 0217A764 */
static BOOL daHys_Execute(void* i_this) {
    return ((daHys_c*)i_this)->MoveBGExecute();
}

/* 0217AF24 */
static BOOL daHys_IsDelete(void*) {
    return TRUE;
}

/* ---- compiler-generated (no GameCube source) ---- */
/* 0217AE90 */
static void __sinit_d_a_hys_cpp() {
    sinit_header_statics(0x1046477C, 0x101B7824);
}

// HD: sead::SafeString deleting destructor (SafeString vtable +0xC)
/* 0217AF2C */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: dBgS_MoveBgActor::IsDelete (this TU's copy, vtable +0x3C)
/* 0217AF40 */
static BOOL MoveBgActor_IsDelete(daHys_c*) {
    return TRUE;
}

// HD: daHys_c deleting destructor (vtable +0xC); mDoExt_btpAnm has no destructor
/* 0217AF48 */
static void daHys_c_dt(daHys_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        Hys_sphDestruct(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 0217AFB4 */
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 100117D4
str 100117F4
str 1001180C "modelData != nullptr" renamed
str 10011800 "pbtp != nullptr" renamed
data m_arcname 101B7848
data m_bdlidx 10011860
data m_btpidx 10011864
data m_dzbidx 10011868
data m_heapsize 10011850
data m_tg_r 10011858
data l_sph_src 101B77C4
data mode_proc 10011820
ext 025E7820 void Hys_btpConstruct(HysBtpAnm*)
ext 025E789C s32 HysBtpAnm::init(J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, s32, s32)
ext 025E7B3C void HysBtpAnm::entry(J3DModelData*, s16)
ext 025166F0 void Hys_sphConstruct(dCcD_Sph*)
ext 02515AE8 void Hys_sphDestruct(dCcD_Sph*, s32)
*/
