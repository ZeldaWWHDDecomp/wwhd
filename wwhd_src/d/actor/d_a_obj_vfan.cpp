/**
 * d_a_obj_vfan.cpp (WWHD)
 * Object - Ganon's Tower: barrier fan broken by the Master Sword (dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_vfan.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10032B28)  /* "Vfan" */
#define SAFESTRING_VTBL 0x10032A60 /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x10032B30        /* daObjVfan::Act_c vtable (HD) */
#define AAB_VTBL 0x10032A78        /* this TU's cM3dGAab vtable */
#define cyl_check_src 0x101CD89C   /* dCcD_SrcCyl */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046C844)
/* namespace daObjVfan { s16 m_evid; } */
static inline be<s16>& m_evid() { return *gabi::at<be<s16>>(0x1046C874); }

enum {
    dRes_INDEX_VFAN_BDL_V_FAN_00_e = 4,
    dRes_INDEX_VFAN_DZB_V_FAN_00_e = 7,
};
enum {
    JA_SE_READ_RIDDLE_1 = 0x806,
    JA_SE_OBJ_GN_SW_DR_LIGHT = 0x6A20,
    JA_SE_OBJ_GN_SW_DR_BREAK = 0x6A21,
};
enum { cPhs_STOP_e = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1988 HD mDoAud_seStart(id) (no position) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* fpcM_CreateResult: base_process_class byte +0xD */
static inline u8 fpcM_CreateResult(void* p) { return gabi::load<u8>(gabi::ea(p) + 0xD); }

namespace daObjVfan {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void ParticleSet();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Cyl mCyl;
    /* 0x558 */ be<s32> mState;
    /* 0x55C */ be<u8> mIsAlive;
    /* 0x55D */ u8 _55D[3];
    /* 0x560 */ be<s32> mBreakTimer;
};
WWHD_OFFSET(Act_c, mStts, 0x3EC);
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mBreakTimer, 0x560);
}  // namespace daObjVfan
using daObjVfan::Act_c;

/* 023B1108: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x023B1108, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x023B1108, PrmAbstract);
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 023B076C */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023B076C, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VFAN_BDL_V_FAN_00_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x8c, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x10032AF0), 0x8C, STR(0x10032AE0));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x023B076C, &Act_c::CreateHeap);

/* 023B0914 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x023B0914, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -4000.0f, -500.0f, -4000.0f, 4000.0f, 500.0f, 4000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(cyl_check_src));
    mCyl.SetC(&current.pos);
    mCyl.SetStts(&mStts);
    mIsAlive = true;
    mBreakTimer = 0;
    mState = 0;
    m_evid() = dComIfGp_evmng_getEventIdx(STR(0x10032B14) /* "Vfan" */, 0xFF);
    return TRUE;
}
VERIFY(0x023B0914, &Act_c::Create);

/* 023B057C */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x023B057C, cPhs_State, this);
    /* fopAcM_ct(this, daObjVfan::Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state;
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        return cPhs_STOP_e;
    } else {
        phase_state = dComIfG_resLoad(&mPhs, M_arcname);
        if (phase_state == cPhs_COMPLEATE_e) {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_VFAN_DZB_V_FAN_00_e, 0, 0xA60);
            if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0xc5, ...) */
                JUT_ASSERT_fail(STR(0x10032A88), 0xC5, STR(0x10032A9C));
        }
    }
    return phase_state;
}
VERIFY(0x023B057C, &Act_c::Mthd_Create);

/* 023B0FB4 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x023B0FB4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023B0FB4, Act_c_Delete);

/* 023B0714 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x023B0714, BOOL, this);
    BOOL res = MoveBGDelete();
    if (fpcM_CreateResult(this) != cPhs_STOP_e) {
        dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    }
    return res;
}
VERIFY(0x023B0714, &Act_c::Mthd_Delete);

/* 023B0808 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x023B0808, void, this);
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x023B0808, &Act_c::set_mtx);

/* 023B08F4 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x023B08F4, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x023B08F4, &Act_c::init_mtx);

/* 023B0A00 HD: dPa_control_c::set with the HD particle ids 0x83CD..0x83D7 */
void Act_c::ParticleSet() {
    WWHD_FUNC(0x023B0A00, void, this);
    for (u16 id = 0x83CD; id <= 0x83D7; id++) /* GANONDOORSMOKE00, GANONDOORFLASH00..09 */
        dComIfGp_particle_set(id, &current.pos, &current.angle);
}
VERIFY(0x023B0A00, &Act_c::ParticleSet);

/* 023B0CB8 */
BOOL Act_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x023B0CB8, BOOL, this, mtx);
    switch ((u32)mState) {
    case 0:
        dComIfG_Ccsp_Set(&mCyl);
        if (mCyl.ChkTgHit()) {
            if (mpBgW != nullptr) {
                if (dBgW_ChkUsed(mpBgW)) {
                    dBgS* bgs = dComIfG_Bgsp();
                    cBgS_Release(bgs, mpBgW);
                }
            }
            fopAcM_orderOtherEventId(this, m_evid(), 0xFF, 0xFFFF, 0, 1);
            mState = 1;
        }
        break;

    case 1:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            mDoAud_seStart_1(JA_SE_READ_RIDDLE_1);
            mDoAud_seStart(JA_SE_OBJ_GN_SW_DR_LIGHT, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
            ParticleSet();
            mBreakTimer = 0;
            mState = 2;
        } else {
            fopAcM_orderOtherEventId(this, m_evid(), 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 2:
        if (mBreakTimer == 150) {
            mIsAlive = false;
            mDoAud_seStart(JA_SE_OBJ_GN_SW_DR_BREAK, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        }
        mBreakTimer = mBreakTimer + 1;
        if (dComIfGp_evmng_endCheck(m_evid())) {
            dComIfGs_onSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_onSwitch */
            dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x3A08); /* dComIfGs_onEventBit(UNK_3A08) */
            dComIfGp_event_reset();
            fopAcM_delete(this);
        }
        break;
    }

    set_mtx();
    gabi::store<u32>(gabi::ea(mtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023B0CB8, &Act_c::Execute);

/* 023B0F10 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023B0F10, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    if (mIsAlive) { /* HD: tests != 0 */
        mDoExt_modelUpdateDL(mpModel);
    }
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023B0F10, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 023B0FBC */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023B0FBC, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x023B0FBC, Mthd_Create);
/* 023B0FC0 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023B0FC0, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023B0FC0, Mthd_Delete);
/* 023B0FC4 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023B0FC4, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023B0FC4, Mthd_Execute);
/* 023B0FC8: MoveBGDraw -> virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023B0FC8, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023B0FC8, Mthd_Draw);
/* 023B0FD8: MoveBGIsDelete -> virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x023B0FD8, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x023B0FD8, Mthd_IsDelete);

/* 023B0FE8 */
static void __sinit_d_a_obj_vfan_cpp() {
    WWHD_FUNC(0x023B0FE8, void, (u32)0);
    sinit_header_statics(0x1046C828, 0x101CD8E0);
}
VERIFY(0x023B0FE8, __sinit_d_a_obj_vfan_cpp);

/* 023B107C: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023B107C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023B107C, SafeString_dt);

/* 023B1090: dBgS_MoveBgActor::IsDelete (virtual; this TU's copy) */
static BOOL MoveBgActor_IsDelete(void*) {
    WWHD_FUNC(0x023B1090, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023B1090, MoveBgActor_IsDelete);

/* 023B1098: sead::SafeString::assureTermination (this TU's copy; empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x023B1098, void, (u32)0);
}
VERIFY(0x023B1098, SafeString_assureTermination);

/* 023B109C: daObjVfan::Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x023B109C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023B109C, Act_c_dt);
