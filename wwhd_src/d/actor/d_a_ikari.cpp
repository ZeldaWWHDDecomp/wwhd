/**
 * d_a_ikari.cpp (WWHD)
 * Object - Forsaken Fortress - Anchors
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ikari.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * WWHD range 0217D744..0217DDDB (0217D6BC/0217D740 belong to the previous unit).
 */
#include "bindings.h"

#define M_arcname STR(0x10011D18)       /* "Ikari" */
#define SAFESTRING_VTBL 0x10011C7C      /* this TU's sead::SafeString vtable */
#define IKARI_VTBL 0x10011C94           /* daIkari_c vtable (HD virtual destructor) */
#define HIO_VTBL 0x10011CA4             /* daObjIkariHIO_c vtable */
#define FILE_NAME STR(0x10011CB4)       /* "d_a_ikari.cpp" */
#define ASSERT_MSG STR(0x10011CC4)      /* "modelData != NULL" */
#define ikari_bdl(i) gabi::load<s32>(0x101B7990 + 4 * (i)) /* {3, 3, 3, 4, 5} */

/* l_HIO (daObjIkariHIO_c) at 0x104647EC: vtable, unk[2] at +4, mWindPowerScale at +8 */
#define L_HIO 0x104647ECu
#define l_HIO_mWindPowerScale gabi::load<f32>(L_HIO + 8)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0257DAA8 dKyw_get_wind_vec(), 0257DB04 dKyw_get_wind_power() */
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline be<f32>* dKyw_get_wind_power() { return gabi::call<be<f32>*>(0x0257DB04); }

struct daIkari_c : fopAc_ac_c {
    void setMtx();
    BOOL _createHeap();
    void getArg();
    bool _execute();
    bool _draw();
    cPhs_State _create();
    bool _delete();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ be<u8> mEnvType;
    /* 0x3B9 */ be<u8> mModelType;
    /* 0x3BA */ u8 _3BA[2];
    /* 0x3BC */ be<s32> mTimer;
};
WWHD_OFFSET(daIkari_c, mpModel, 0x3B4);
WWHD_OFFSET(daIkari_c, mTimer, 0x3BC);

/* 0217D744 */
BOOL daIkari_c::_createHeap() {
    WWHD_FUNC(0x0217D744, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, ikari_bdl(mModelType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x7e, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x7e, ASSERT_MSG);
    mpModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (!mpModel)
        return false;
    return true;
}
VERIFY(0x0217D744, &daIkari_c::_createHeap);

/* 0217D7F0 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0217D7F0, BOOL, i_this);
    return ((daIkari_c*)i_this)->_createHeap();
}
VERIFY(0x0217D7F0, createHeap_CB);

/* 0217D7F4 */
void daIkari_c::getArg() {
    WWHD_FUNC(0x0217D7F4, void, this);
    u32 param = fopAcM_GetParam(this);

    mModelType = (param >> 8) & 0xFF;
    mEnvType = param;

    if (mModelType == 0xff) {
        mModelType = 0;
    }

    if (mEnvType == 0xff) {
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
    } else if (mEnvType == 0x01) {
        scale.x = 1.27f;
        scale.y = 1.27f;
        scale.z = 1.27f;
    }
}
VERIFY(0x0217D7F4, &daIkari_c::getArg);

/* 0217D860 */
void daIkari_c::setMtx() {
    WWHD_FUNC(0x0217D860, void, this);
    J3DModel_setBaseScale(mpModel, &scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), current.angle.x);
    mDoMtx_stack_c::YrotM(-shape_angle.y);
    mDoMtx_stack_c::YrotM(current.angle.y);

    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0217D860, &daIkari_c::setMtx);

/* 0217D964 */
cPhs_State daIkari_c::_create() {
    WWHD_FUNC(0x0217D964, cPhs_State, this);
    cPhs_State phase = dComIfG_resLoad(&mPhs, M_arcname);

    /* fopAcM_ct(this, daIkari_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = IKARI_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    if (phase == cPhs_COMPLEATE_e) {
        getArg();

        if (!fopAcM_entrySolidHeap(this, 0x0217D7F0 /* createHeap_CB */, 0xC20)) {
            return cPhs_ERROR_e;
        } else {
            setMtx();

            J3DModel* model = mpModel;
            f32 scaleX = scale.x;
            cullMtx = model ? gabi::ea(model) + 0xC8 : 0; /* fopAcM_SetMtx */
            fopAcM_setCullSizeBox(this, -160.0f * scaleX, -2500.0f * scaleX, -600.0f * scaleX, 160.0f * scaleX,
                                  100.0f * scaleX, 600.0f * scaleX);
            cullSizeFar = 10.0f;

            mTimer = (s16)gabi::ftoi(cM_rndF(32768.0f));
        }
    }

    return phase;
}
VERIFY(0x0217D964, &daIkari_c::_create);

/* 0217DAC0 */
static cPhs_State daIkariCreate(void* i_this) {
    WWHD_FUNC(0x0217DAC0, cPhs_State, i_this);
    return ((daIkari_c*)i_this)->_create();
}
VERIFY(0x0217DAC0, daIkariCreate);

/* 0217DAC4 */
bool daIkari_c::_delete() {
    WWHD_FUNC(0x0217DAC4, bool, this);
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}
VERIFY(0x0217DAC4, &daIkari_c::_delete);

/* 0217DAF4 */
static BOOL daIkariDelete(void* i_this) {
    WWHD_FUNC(0x0217DAF4, BOOL, i_this);
    return ((daIkari_c*)i_this)->_delete();
}
VERIFY(0x0217DAF4, daIkariDelete);

/* 0217DAF8 */
bool daIkari_c::_execute() {
    WWHD_FUNC(0x0217DAF8, bool, this);
    mTimer = mTimer + 1;

    cXyz* windVec = dKyw_get_wind_vec();
    s16 windAngle = cM_atan2s(windVec->x, windVec->z);
    be<f32>* windPow = dKyw_get_wind_power();

    shape_angle.y = windAngle;

    f32 rotX = *windPow * 10000.0f * l_HIO_mWindPowerScale;

    current.angle.x = (s16)gabi::ftoi(gabi::fmadds(cM_ssin(mTimer * (REG0_S(5) + 500)), rotX, rotX));

    setMtx();

    return true;
}
VERIFY(0x0217DAF8, &daIkari_c::_execute);

/* 0217DBBC */
static BOOL daIkariExecute(void* i_this) {
    WWHD_FUNC(0x0217DBBC, BOOL, i_this);
    return ((daIkari_c*)i_this)->_execute();
}
VERIFY(0x0217DBBC, daIkariExecute);

/* 0217DBC0 */
bool daIkari_c::_draw() {
    WWHD_FUNC(0x0217DBC0, bool, this);
    u8* t = (u8*)&tevStr;
    if (mEnvType == 1) {
        u32 light = gabi::ea(dKy_getEnvlight());
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
        /* HD: mColorC0 (s16 x4 at tevStr+0x90) from the u8 mActorC0 (light+0xB64); no alpha */
        gabi::store<s16>(gabi::ea(t) + 0x90, gabi::load<u8>(light + 0xB64));
        gabi::store<s16>(gabi::ea(t) + 0x92, gabi::load<u8>(light + 0xB65));
        gabi::store<s16>(gabi::ea(t) + 0x94, gabi::load<u8>(light + 0xB66));
        gabi::store<u8>(gabi::ea(t) + 0x98, gabi::load<u8>(light + 0xB68));
        gabi::store<u8>(gabi::ea(t) + 0x99, gabi::load<u8>(light + 0xB69));
        gabi::store<u8>(gabi::ea(t) + 0x9A, gabi::load<u8>(light + 0xB6A));
    } else {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    }
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x0217DBC0, &daIkari_c::_draw);

/* 0217DCA0 */
static BOOL daIkariDraw(void* i_this) {
    WWHD_FUNC(0x0217DCA0, BOOL, i_this);
    return ((daIkari_c*)i_this)->_draw();
}
VERIFY(0x0217DCA0, daIkariDraw);

/* 0217DCA4: header statics, then l_HIO's constructor */
static void __sinit_d_a_ikari_cpp() {
    WWHD_FUNC(0x0217DCA4, void, (u32)0);
    sinit_header_statics(0x104647D0, 0x101B79A4);
    gabi::store<u8>(L_HIO + 4, 0);
    gabi::store<f32>(L_HIO + 8, 0.1f);
    gabi::store<u32>(L_HIO + 0, HIO_VTBL);
    gabi::store<u8>(L_HIO + 5, 0);
}
VERIFY(0x0217DCA4, __sinit_d_a_ikari_cpp);

/* 0217DD68: sead::SafeString deleting destructor (per-TU copy) */
static void safestring_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0217DD68, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0217DD68, safestring_dt);

/* 0217DD7C */
static BOOL daIkariIsDelete(void* i_this) {
    WWHD_FUNC(0x0217DD7C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0217DD7C, daIkariIsDelete);

/* 0217DD84: daIkari_c deleting destructor */
static void daIkari_c_dt(daIkari_c* i_this, s32 flags) {
    WWHD_FUNC(0x0217DD84, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0217DD84, daIkari_c_dt);

/* 0217DDD8: an empty virtual of the SafeString vtable (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x0217DDD8, void, p);
}
VERIFY(0x0217DDD8, empty_virtual);
