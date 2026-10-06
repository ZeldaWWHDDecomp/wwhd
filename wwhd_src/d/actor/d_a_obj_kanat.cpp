/**
 * d_a_obj_kanat.cpp (WWHD)
 * Object - Forbidden Woods - Solid vine floor (blocks entry to B1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_kanat.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002BBE8) /* "Kanat" */
#define SAFESTRING_VTBL 0x1002BB38
#define ACT_VTBL 0x1002BBF0 /* HD: daObjKanat::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046A384)

enum {
    dRes_INDEX_KANAT_BDL_KANAT_e = 4,
    dRes_INDEX_KANAT_DZB_KANAT_e = 7,
};

/* dPa_smokeEcallBack (0x20, HD: vtable at +0): +0x10 flags (bit 0 = end), +0x11 rate-off */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x10 - 0x04];
    /* 0x10 */ be<u8> mFlags;
    /* 0x11 */ be<u8> mRateOff;
    /* 0x12 */ u8 _12[0x20 - 0x12];
    bool isEnd() { return (mFlags & 1) != 0; }
    void setRateOff(u8 v) { mRateOff = v; }
    void remove() { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x44), this); } /* virtual */
};

namespace daObjKanat {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Delete();
    BOOL Execute(Mtx34** pMtx);
    BOOL Draw();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dPa_smokeEcallBack_l mSmokeCb;
    /* 0x40C */ be<u8> mIsBroken;
    /* 0x40D */ be<u8> mIsVisible;
    /* 0x40E */ u8 _40E[2];
};
WWHD_OFFSET(Act_c, mSmokeCb, 0x3EC);
WWHD_OFFSET(Act_c, mIsBroken, 0x40C);
WWHD_SIZE(Act_c, 0x410);
}  // namespace daObjKanat
using daObjKanat::Act_c;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void dPa_smokeEcallBack_ct(dPa_smokeEcallBack_l* p, u8 v) { gabi::call(0x025A5B18, p, v); }
/* fpcM_CreateResult(p): base_process_class byte +0xD */
static inline u8 fpcM_CreateResult(void* p) { return gabi::load<u8>(gabi::ea(p) + 0xD); }

/* J3DModel::setBaseScale: a pure lfs -> stfs copy, bit-exact in the recompiled original */
static inline void J3DModel_setBaseScale_bits(J3DModel* m, const cXyz* s) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(m) + 0xBC + i, gabi::load<u32>(gabi::ea(s) + i));
}

/* 02367420: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x02367420, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x02367420, PrmAbstract);
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 02366E88 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x02366E88, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_KANAT_BDL_KANAT_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(79, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002BBB8), 0x4F, STR(0x1002BBA8));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr;
}
VERIFY(0x02366E88, &Act_c::CreateHeap);

/* 02367018 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x02367018, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -100.0f, -500.0f, 500.0f, 200.0f, 500.0f);
    mIsBroken = false;
    mIsVisible = true;
    mSmokeCb.setRateOff(0);
    return TRUE;
}
VERIFY(0x02367018, &Act_c::Create);

/* 02366D00 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x02366D00, cPhs_State, this);
    /* fopAcM_ct(this, daObjKanat::Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dPa_smokeEcallBack_ct(&mSmokeCb, 1);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        return 3; /* cPhs_STOP_e */
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_KANAT_DZB_KANAT_e, 0, 0x6440);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(126, ...) */
            JUT_ASSERT_fail(STR(0x1002BB50), 0x7D, STR(0x1002BB64));
    }
    return phase_state;
}
VERIFY(0x02366D00, &Act_c::Mthd_Create);

/* 023672BC */
BOOL Act_c::Delete() {
    WWHD_FUNC(0x023672BC, BOOL, this);
    mSmokeCb.remove();
    return TRUE;
}
VERIFY(0x023672BC, &Act_c::Delete);

/* 02366E30 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x02366E30, BOOL, this);
    BOOL result = MoveBGDelete();
    if (fpcM_CreateResult(this) != 3 /* cPhs_STOP_e */) {
        dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    }
    return result;
}
VERIFY(0x02366E30, &Act_c::Mthd_Delete);

/* 02366F24 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02366F24, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x02366F24, &Act_c::set_mtx);

/* 02366FF8 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02366FF8, void, this);
    J3DModel_setBaseScale_bits(mpModel, &scale);
    set_mtx();
}
VERIFY(0x02366FF8, &Act_c::init_mtx);

/* 023670A0 */
BOOL Act_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x023670A0, BOOL, this, pMtx);
    if (!mIsBroken) {
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            gabi::Local<GXColor> color;
            /* tevStr.mColorC0 (GXColorS10 at tevStr+0x90), truncated to bytes */
            u32 c0 = gabi::ea(&tevStr) + 0x90;
            color->r = (u8)gabi::load<s16>(c0 + 0);
            color->g = (u8)gabi::load<s16>(c0 + 2);
            color->b = (u8)gabi::load<s16>(c0 + 4);
            mIsBroken = true;
            color->a = (u8)gabi::load<s16>(c0 + 6);
            const GXColor* k0 = gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98); /* tevStr.mColorK0 */
            s8 room = current.roomNo;
            dPa_control_c* pa = dComIfGp_getParticle();
            dPa_control_set(pa, 0, 0x82A2 /* ID_AK_SN_KOKIRIHOUSEHAHEN00 */, &current.pos, &current.angle, nullptr, 0xFF,
                            nullptr, room, k0, color.get(), nullptr);
            /* dComIfGp_particle_setToon: HD group 2 */
            room = current.roomNo;
            pa = dComIfGp_getParticle();
            dPa_control_set(pa, 2, 0xA2A3 /* ID_AK_ST_KOKIRIHOUSESMOKE00 */, &current.pos, &current.angle, nullptr, 0xB4,
                            (dPa_levelEcallBack*)(void*)&mSmokeCb, room, k0, color.get(), nullptr);
        }
    } else {
        mIsVisible = false;
        if (mSmokeCb.isEnd()) {
            fopAcM_delete(this);
        }
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(pMtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023670A0, &Act_c::Execute);

/* 02367218 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x02367218, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    if (!mIsVisible) {
        return TRUE;
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02367218, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 023672EC */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023672EC, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x023672EC, Mthd_Create);
/* 023672F0 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023672F0, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023672F0, Mthd_Delete);
/* 023672F4 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023672F4, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023672F4, Mthd_Execute);
/* 023672F8: MoveBGDraw inlined: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023672F8, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023672F8, Mthd_Draw);
/* 02367308: MoveBGIsDelete inlined: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02367308, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x02367308, Mthd_IsDelete);

/* 02367318 */
static void __sinit_d_a_obj_kanat_cpp() {
    WWHD_FUNC(0x02367318, void, (u32)0);
    sinit_header_statics(0x1046A368, 0x101CA618);
}
VERIFY(0x02367318, __sinit_d_a_obj_kanat_cpp);

/* 023673AC: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023673AC, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x023673AC, SafeString_dt);

/* 023673C0: dBgS_MoveBgActor::IsDelete (this TU's copy, Act_c vtable +0x3C) */
static BOOL Act_c_IsDelete(Act_c* i_this) {
    WWHD_FUNC(0x023673C0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023673C0, Act_c_IsDelete);

/* 023673C8: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x023673C8, void, (u32)0);
}
VERIFY(0x023673C8, SafeString_assureTerminationImpl);

/* 023673CC: Act_c deleting destructor (compiler-generated, vtable +0xC) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x023673CC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023673CC, Act_c_dt);
