/**
 * d_a_amiprop.cpp (WWHD)
 * Object - Rotating mesh propeller (Hami1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_amiprop.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x100071A0) /* "Hami1" */
#define SAFESTRING_VTBL 0x10007124
#define AMIPROP_VTBL 0x1000714C   /* HD: daAmiProp_c vtable */
#define AAB_VTBL 0x1000713C       /* this TU's cM3dGAab vtable */
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x1018FC0C)

enum {
    dRes_INDEX_HAMI1_BDL_HAMI1_e = 4,
    dRes_INDEX_HAMI1_DZB_HAMI1_e = 7,
};
enum { JA_SE_OBJ_AMI_PROP = 0x6166 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3DModel::setUserArea: mUserArea at +0xB8 (HD) */
static inline void J3DModel_setUserArea_l(J3DModel* m, u32 v) { gabi::store<u32>(gabi::ea(m) + 0xB8, v); }
/* dBgW::SetCrrFunc: mpCrrFunc at +0xA8 (HD); dBgS_MoveBGProc_TypicalRotY = 024EE708 */
static inline void dBgW_SetCrrFunc_l(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }
/* fopAcM_setCullSizeFar: cullSizeFar (+0x364) */
static inline void fopAcM_setCullSizeFar_l(fopAc_ac_c* a, f32 v) { a->cullSizeFar = v; }

struct daAmiProp_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    void setMoveBGMtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ gptr<dBgW> mpBgW;
    /* 0x528 */ Mtx34 unk_40C;
    /* 0x558 */ be<u32> unk_43C;
    /* 0x55C */ be<s32> unk_440;
    /* 0x560 */ be<u8> unk_444;
    /* 0x561 */ u8 _561;
    /* 0x562 */ be<s16> unk_446;
};
WWHD_OFFSET(daAmiProp_c, mStts, 0x3B8);
WWHD_OFFSET(daAmiProp_c, mCyl, 0x3F4);
WWHD_OFFSET(daAmiProp_c, mpBgW, 0x524);
WWHD_OFFSET(daAmiProp_c, unk_43C, 0x558);
WWHD_OFFSET(daAmiProp_c, unk_446, 0x562);
WWHD_SIZE(daAmiProp_c, 0x564);

namespace daAmiProp_prm {
inline u8 getSwitchNo(daAmiProp_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u8 getType(daAmiProp_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xF; }
} // namespace daAmiProp_prm

/* 02050C1C */
bool daAmiProp_c::_delete() {
    WWHD_FUNC(0x02050C1C, bool, this);
    if (heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), mpBgW);
    }
    dComIfG_resDelete(&mPhase, m_arcname);
    return true;
}
VERIFY(0x02050C1C, &daAmiProp_c::_delete);

/* 02050914 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02050914, BOOL, i_this);
    return ((daAmiProp_c*)i_this)->CreateHeap();
}
VERIFY(0x02050914, CheckCreateHeap);

/* 02050800 */
BOOL daAmiProp_c::CreateHeap() {
    WWHD_FUNC(0x02050800, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HAMI1_BDL_HAMI1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(255, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000715C), 0xFF, STR(0x1000716C));

    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    J3DModel_setUserArea_l(model, gabi::ea(this));
    setMoveBGMtx();
    mpBgW = new_dBgW();
    if (mpBgW != nullptr) {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HAMI1_DZB_HAMI1_e, SAFESTRING_VTBL);
        if (cBgW_Set(mpBgW, dzb, cBgW_MOVE_BG_e, &unk_40C) == true) {
            return FALSE;
        }
        dBgW_SetCrrFunc_l(mpBgW, 0x024EE708 /* dBgS_MoveBGProc_TypicalRotY */);
    } else {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02050800, &daAmiProp_c::CreateHeap);

/* 02050A04 */
void daAmiProp_c::CreateInit() {
    WWHD_FUNC(0x02050A04, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -850.0f, -30.0f, -850.0f, 850.0f, 30.0f, 850.0f);
    fopAcM_setCullSizeFar_l(this, 1.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(l_cyl_src);
    mCyl.mStts = &mStts; /* SetStts */
    unk_43C = daAmiProp_prm::getSwitchNo(this);
    unk_446 = (s16)(current.angle.x + 0x4000);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    set_mtx();
    dBgW_Move(mpBgW);
}
VERIFY(0x02050A04, &daAmiProp_c::CreateInit);

/* 02050AE0 */
cPhs_State daAmiProp_c::_create() {
    WWHD_FUNC(0x02050AE0, cPhs_State, this);
    /* fopAcM_ct(this, daAmiProp_c): HD vtable, inline dCcD_Stts / dCcD_Cyl constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = AMIPROP_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhase, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x02050914 /* CheckCreateHeap */, 0xC00)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}
VERIFY(0x02050AE0, &daAmiProp_c::_create);

/* 02050918 */
void daAmiProp_c::set_mtx() {
    WWHD_FUNC(0x02050918, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &unk_40C); /* MTXCopy */
}
VERIFY(0x02050918, &daAmiProp_c::set_mtx);

/* 02050788 */
void daAmiProp_c::setMoveBGMtx() {
    WWHD_FUNC(0x02050788, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &unk_40C); /* MTXCopy */
}
VERIFY(0x02050788, &daAmiProp_c::setMoveBGMtx);

/* 02050D14 */
bool daAmiProp_c::_execute() {
    WWHD_FUNC(0x02050D14, bool, this);
    s32 is_switch = fopAcM_isSwitch(this, unk_43C);
    if ((u32)is_switch != (u32)unk_440) {
        unk_444 = true;
    }

    if (unk_444) {
        s16 tmp;
        switch (daAmiProp_prm::getType(this)) {
        case 0:
            tmp = cLib_addCalcAngleS(&current.angle.x, unk_446, 10, 0x200, 0x10);
            break;
        case 1:
            tmp = cLib_addCalcAngleS(&current.angle.y, unk_446, 10, 0x200, 0x10);
            break;
        default:
            tmp = 1; /* GameCube: uninitialised; GHS takes the sound path */
            break;
        }
        if (tmp == 0) {
            unk_444 = false;
            unk_446 = (s16)(unk_446 + 0x4000);
            dVibration_c* vib = dComIfGp_getVibration();
            gabi::Local<cXyz> pos;
            pos->x = 0.0f;
            pos->y = 1.0f;
            pos->z = 0.0f;
            gabi::call<BOOL>(0x025CB374, vib, 4, -0x21, pos.get()); /* StartShock(4, -0x21, cXyz(0, 1, 0)) */
        } else {
            /* fopAcM_seStart: HD inline without the null checks here */
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(JA_SE_OBJ_AMI_PROP, &eyePos, 0, reverb);
        }
    }
    unk_440 = is_switch;
    set_mtx();
    dBgW_Move(mpBgW);
    return true;
}
VERIFY(0x02050D14, &daAmiProp_c::_execute);

/* 02050C78 */
bool daAmiProp_c::_draw() {
    WWHD_FUNC(0x02050C78, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}
VERIFY(0x02050C78, &daAmiProp_c::_draw);

/* method table entries (HD: tail branches) */
/* 02050C18 */
static cPhs_State daAmiProp_Create(void* i_this) {
    WWHD_FUNC(0x02050C18, cPhs_State, i_this);
    return ((daAmiProp_c*)i_this)->_create();
}
VERIFY(0x02050C18, daAmiProp_Create);
/* 02050C74 */
static BOOL daAmiProp_Delete(void* i_this) {
    WWHD_FUNC(0x02050C74, BOOL, i_this);
    return ((daAmiProp_c*)i_this)->_delete();
}
VERIFY(0x02050C74, daAmiProp_Delete);
/* 02050D10 */
static BOOL daAmiProp_Draw(void* i_this) {
    WWHD_FUNC(0x02050D10, BOOL, i_this);
    return ((daAmiProp_c*)i_this)->_draw();
}
VERIFY(0x02050D10, daAmiProp_Draw);
/* 02050ECC */
static BOOL daAmiProp_Execute(void* i_this) {
    WWHD_FUNC(0x02050ECC, BOOL, i_this);
    return ((daAmiProp_c*)i_this)->_execute();
}
VERIFY(0x02050ECC, daAmiProp_Execute);
/* 02050F78 */
static BOOL daAmiProp_IsDelete(void*) {
    WWHD_FUNC(0x02050F78, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02050F78, daAmiProp_IsDelete);

/* ---- compiler-generated (HD) ---- */

/* 02050ED0 */
static void __sinit_d_a_amiprop_cpp() {
    WWHD_FUNC(0x02050ED0, void, (u32)0);
    sinit_header_statics(0x104613F8, 0x1018FC50);
}
VERIFY(0x02050ED0, __sinit_d_a_amiprop_cpp);

/* 02050F64: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02050F64, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02050F64, trivial_dt);

/* 02050F80: daAmiProp_c deleting destructor */
static void daAmiProp_c_dt(daAmiProp_c* i_this, s32 flags) {
    WWHD_FUNC(0x02050F80, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02050F80, daAmiProp_c_dt);

/* 02050FEC: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02050FEC, void, p);
}
VERIFY(0x02050FEC, empty_virtual);
