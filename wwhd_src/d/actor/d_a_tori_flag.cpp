/**
 * d_a_tori_flag.cpp (WWHD)
 * Object - Great Sea - Small red flag (Flight Control Platform, Horseshoe Island)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tori_flag.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "bindings.h"

/* guest string literals (.rodata), not pooled */
#define M_arcname_res gabi::at<const char>(0x10040918)   /* "Trflag" */
#define CLOTH_res gabi::at<const char>(0x100408A4)       /* "Cloth" (CreateHeap) */
#define CLOTH_load gabi::at<const char>(0x1004084C)      /* "Cloth" (_create) */
#define CLOTH_del gabi::at<const char>(0x100408D4)       /* "Cloth" (_delete) */
#define SAFESTRING_VTBL 0x10040854                       /* this TU's sead::SafeString vtable */
#define TORI_FLAG_VTBL 0x1004088C
#define TORI_FLAG_AAB_VTBL 0x1004086C                    /* this TU's cM3dGAab vtable */
#define HIO_VTBL 0x1004087C
#define L_CYL_SRC gabi::at<dCcD_SrcCyl>(0x101D2970)

/* statics (.bss) */
#define l_flag_offset gabi::at<cXyz>(0x1046E9C4)
#define l_HIO_ea 0x1046E9A4u

enum {
    dRes_INDEX_TRFLAG_BDL_ETHATA_e = 4,
    dRes_INDEX_TRFLAG_BTI_ETHATA_e = 7,
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 3,
};

WWHD_OPAQUE(ResTIMG);

/* dCloth_packet_c (HD): only the parameters this actor writes (setParam / setWindPower inline) */
struct dCloth_packet_c_l {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> __vtbl;               /* +0xC delete(3), +0x3C cloth_move, +0x44 cloth_draw */
    /* 0x010 */ u8 _010[0xE4 - 0x10];
    /* 0x0E4 */ be<f32> mWindSpeed;           /* setParam's 10th argument, then setWindPower */
    /* 0x0E8 */ be<f32> mWindSpeedWave;
    /* 0x0EC */ u8 _0EC[0x1A0 - 0xEC];
    /* 0x1A0 */ be<f32> mSpringRate;
    /* 0x1A4 */ be<f32> mGravity;
    /* 0x1A8 */ be<f32> mDecayRate;
    /* 0x1AC */ be<f32> mFlyFlex;
    /* 0x1B0 */ be<f32> mHangFlex;
    /* 0x1B4 */ u8 _1B4[2];
    /* 0x1B6 */ be<s16> mWaveSpeed;
    /* 0x1B8 */ u8 _1B8[2];
    /* 0x1BA */ be<s16> mRotateY;
    /* 0x1BC */ be<s16> mRatio;
    /* 0x1BE */ be<s16> mRatioWave;
};
WWHD_OFFSET(dCloth_packet_c_l, mSpringRate, 0x1A0);
WWHD_OFFSET(dCloth_packet_c_l, mRatioWave, 0x1BE);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0251BC80 dCloth_packet_create(flagTimg, toonTimg, w, h, sizeW, sizeH, tevStr, posArr) */
static inline dCloth_packet_c_l* dCloth_packet_create(ResTIMG* flag, ResTIMG* toon, s32 w, s32 h, f32 sw, f32 sh, dKy_tevstr_c* tev, void* pos) {
    return gabi::call<dCloth_packet_c_l*>(0x0251BC80, flag, toon, w, h, sw, sh, tev, pos);
}
/* 0251B638 dCloth_packet_c::setMtx(Mtx) */
static inline void dCloth_packet_setMtx(dCloth_packet_c_l* c, Mtx34* m) { gabi::call(0x0251B638, c, m); }
/* 0251E7A8 dCloth_packet_c::setGlobalWind(cXyz*) */
static inline void dCloth_packet_setGlobalWind(dCloth_packet_c_l* c, cXyz* w) { gabi::call(0x0251E7A8, c, w); }
static inline void dCloth_packet_vcall(dCloth_packet_c_l* c, u32 slot) { gabi::call_ptr(gabi::load<u32>(c->__vtbl + slot), c); }
static inline void dCloth_packet_delete(dCloth_packet_c_l* c) { gabi::call_ptr(gabi::load<u32>(c->__vtbl + 0xC), c, 3); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
/* 0257E34C dKyw_get_AllWind_vecpow(cXyz* pos): cXyz through a hidden result pointer */
static inline void dKyw_get_AllWind_vecpow(cXyz* res, cXyz* pos) { gabi::call(0x0257E34C, res, pos); }
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
static inline void cLib_addCalcPos2(cXyz* v, const cXyz* t, f32 scale, f32 maxStep) { gabi::call(0x0200F164, v, t, scale, maxStep); }
static inline void daObj_HitSeStart(cXyz* pos, s32 roomNo, dCcD_GObjInf* obj, u32 p) { gabi::call(0x023129C4, pos, roomNo, obj, p); }
static inline void fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 r, u32 p) { gabi::call(0x025D69FC, a, r, p); }
static inline void mDoMtx_stack_transM_v(const cXyz* v) { mDoMtx_stack_transM(v->x, v->y, v->z); }
/* dKy_tevstr_c inline constructor (HD): three light blocks at +0, +0xC0, +0x144 from the
 * template at 0x1016E414 (floats, then bytes and shorts) -- as in d_a_npc_bmsw */
static inline void dKy_tevstr_c_ct(u32 base) {
    static const u32 blk[3] = {0x0, 0xC0, 0x144};
    const u32 T = 0x1016E414;
    for (int b = 0; b < 3; b++) {
        u32 d = base + blk[b];
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d + o, gabi::load<u8>(T + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(d + o, gabi::load<s16>(T + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
    }
}

struct daTori_Flag_HIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1[3];
    /* 0x4 */ be<f32> m08;
    /* 0x8 */ be<s16> m0C;
    /* 0xA */ u8 _A[2];
    /* 0xC */ be<u32> __vtbl;
};
WWHD_SIZE(daTori_Flag_HIO_c, 0x10);

struct daTori_Flag_c : fopAc_ac_c {
    void set_mtx();
    BOOL CreateHeap();
    cPhs_State CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhsTrflag;   /* GameCube 0x290 */
    /* 0x3B4 */ request_of_phase_process_class mPhsCloth;
    /* 0x3BC */ gptr<J3DModel> mpModel;
    /* 0x3C0 */ gptr<dCloth_packet_c_l> mpCloth;
    /* 0x3C4 */ cXyz mWindvec;
    /* 0x3D0 */ dKy_tevstr_c mClothTevStr;
    /* 0x598 */ dCcD_Stts mStts;
    /* 0x5D4 */ dCcD_Cyl mCyl;
    /* 0x704 */ dCcD_Cyl mCyl2;
};
WWHD_OFFSET(daTori_Flag_c, mpCloth, 0x3C0);
WWHD_OFFSET(daTori_Flag_c, mClothTevStr, 0x3D0);
WWHD_OFFSET(daTori_Flag_c, mStts, 0x598);
WWHD_OFFSET(daTori_Flag_c, mCyl2, 0x704);
WWHD_SIZE(daTori_Flag_c, 0x834);

/* 024C848C daTori_Flag_HIO_c::daTori_Flag_HIO_c (HD: allocates when this == NULL) */
static daTori_Flag_HIO_c* daTori_Flag_HIO_c_ct(daTori_Flag_HIO_c* i_this) {
    WWHD_FUNC(0x024C848C, daTori_Flag_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daTori_Flag_HIO_c*)operator_new(0x10);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mNo = -1;
    i_this->m08 = 0.0f;
    i_this->m0C = 0;
    i_this->__vtbl = HIO_VTBL;
    return i_this;
}
VERIFY(0x024C848C, daTori_Flag_HIO_c_ct);

/* 024C7E50 */
void daTori_Flag_c::set_mtx() {
    WWHD_FUNC(0x024C7E50, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    mDoMtx_stack_transM_v(l_flag_offset);
    dCloth_packet_setMtx(mpCloth, mDoMtx_stack_c::get());
}
VERIFY(0x024C7E50, &daTori_Flag_c::set_mtx);

/* 024C7BFC CheckCreateHeap (a tail branch to CreateHeap) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024C7BFC, BOOL, i_this);
    return static_cast<daTori_Flag_c*>(i_this)->CreateHeap();
}
VERIFY(0x024C7BFC, CheckCreateHeap);

/* 024C7AD8 */
BOOL daTori_Flag_c::CreateHeap() {
    WWHD_FUNC(0x024C7AD8, BOOL, this);
    BOOL ret;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_TRFLAG_BDL_ETHATA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x120, modelData != NULL) */
        JUT_ASSERT_fail(gabi::at<const char>(0x100408AC), 0x120, gabi::at<const char>(0x100408C0));
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!mpModel) {
        ret = FALSE;
    } else {
        ResTIMG* flagTimg = (ResTIMG*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_TRFLAG_BTI_ETHATA_e, SAFESTRING_VTBL);
        ResTIMG* clothTimg = (ResTIMG*)dComIfG_getObjectRes(CLOTH_res, dRes_INDEX_CLOTH_BTI_CLOTHTOON_e, SAFESTRING_VTBL);
        mpCloth = dCloth_packet_create(flagTimg, clothTimg, 5, 5, 210.0f, 105.0f, &mClothTevStr, nullptr);
        ret = mpCloth ? TRUE : FALSE;
    }
    return ret;
}
VERIFY(0x024C7AD8, &daTori_Flag_c::CreateHeap);

/* 024C7F50 */
cPhs_State daTori_Flag_c::CreateInit() {
    WWHD_FUNC(0x024C7F50, cPhs_State, this);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(L_CYL_SRC);
    mCyl.SetStts(&mStts);
    cXyz* wind_vec = dKyw_get_wind_vec();
    /* word copies */
    gabi::store<u32>(gabi::ea(&mWindvec) + 0, gabi::load<u32>(gabi::ea(wind_vec) + 0));
    gabi::store<u32>(gabi::ea(&mWindvec) + 4, gabi::load<u32>(gabi::ea(wind_vec) + 4));
    gabi::store<u32>(gabi::ea(&mWindvec) + 8, gabi::load<u32>(gabi::ea(wind_vec) + 8));
    set_mtx();
    dKy_tevstr_init(&mClothTevStr, current.roomNo, 0xFF);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024C7F50, &daTori_Flag_c::CreateInit);

/* 024C7C00 daTori_Flag_c::daTori_Flag_c (out of line in HD; allocates when this == NULL) */
static daTori_Flag_c* daTori_Flag_c_ct(daTori_Flag_c* i_this) {
    WWHD_FUNC(0x024C7C00, daTori_Flag_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daTori_Flag_c*)operator_new(0x834);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = TORI_FLAG_VTBL;
    dKy_tevstr_c_ct(gabi::ea(&i_this->mClothTevStr));
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, TORI_FLAG_AAB_VTBL);
    dCcD_Cyl_ct(&i_this->mCyl2, TORI_FLAG_AAB_VTBL);
    return i_this;
}
VERIFY(0x024C7C00, daTori_Flag_c_ct);

/* 024C7FF4 daTori_FlagCreate (inlines _create) */
static cPhs_State daTori_FlagCreate(void* v) {
    WWHD_FUNC(0x024C7FF4, cPhs_State, v);
    daTori_Flag_c* i_this = (daTori_Flag_c*)v;
    /* fopAcM_ct(this, daTori_Flag_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr)
            daTori_Flag_c_ct(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State result = dComIfG_resLoad(&i_this->mPhsTrflag, M_arcname_res);
    if (result != cPhs_COMPLEATE_e)
        return result;
    result = dComIfG_resLoad(&i_this->mPhsCloth, CLOTH_load);
    if (result != cPhs_COMPLEATE_e)
        return result;
    if (fopAcM_entrySolidHeap(i_this, 0x024C7BFC /* CheckCreateHeap */, 0x1020)) {
        result = i_this->CreateInit();
    } else {
        result = cPhs_ERROR_e;
    }
    return result;
}
VERIFY(0x024C7FF4, daTori_FlagCreate);

/* 024C80D8 daTori_FlagDelete (inlines _delete; HD: deletes the cloth packet) */
static BOOL daTori_FlagDelete(void* v) {
    WWHD_FUNC(0x024C80D8, BOOL, v);
    daTori_Flag_c* i_this = (daTori_Flag_c*)v;
    dComIfG_resDelete(&i_this->mPhsTrflag, M_arcname_res);
    dComIfG_resDelete(&i_this->mPhsCloth, CLOTH_del);
    if (i_this->mpCloth)
        dCloth_packet_delete(i_this->mpCloth);
    i_this->mpCloth = nullptr;
    return TRUE;
}
VERIFY(0x024C80D8, daTori_FlagDelete);

/* the common tail of _execute (GHS duplicated it into both wind branches) */
static inline void tori_flag_execute_tail(daTori_Flag_c* i_this) {
    i_this->mCyl.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    dCloth_packet_c_l* c = i_this->mpCloth;
    /* mpCloth->setParam(0.4f, -1.5f, 0.75f, 0.9f, 0.9f, 0x400, 0, 900, -800, 7.0f, 6.0f) */
    c->mSpringRate = 0.4f;
    c->mGravity = -1.5f;
    c->mDecayRate = 0.75f;
    c->mFlyFlex = 0.9f;
    c->mHangFlex = 0.9f;
    c->mWaveSpeed = 0x400;
    c->mRotateY = 0;
    c->mRatio = 900;
    c->mRatioWave = -800;
    c->mWindSpeed = 7.0f;
    c->mWindSpeedWave = 6.0f;
    /* mpCloth->setWindPower(8.0f, 3.0f) */
    c = i_this->mpCloth;
    c->mWindSpeed = 8.0f;
    c->mWindSpeedWave = 3.0f;
    dCloth_packet_setGlobalWind(i_this->mpCloth, &i_this->mWindvec);
    dCloth_packet_vcall(i_this->mpCloth, 0x3C); /* cloth_move */
}

/* 024C814C daTori_FlagExecute (inlines _execute) */
static BOOL daTori_FlagExecute(void* v) {
    WWHD_FUNC(0x024C814C, BOOL, v);
    daTori_Flag_c* i_this = (daTori_Flag_c*)v;
    i_this->set_mtx();
    i_this->mStts.Move();
    if (i_this->mCyl.ChkTgHit())
        daObj_HitSeStart(&i_this->current.pos, fopAcM_GetRoomNo(i_this), &i_this->mCyl, 0x0B);
    fopAcM_rollPlayerCrash(i_this, 40.0f, 7);
    gabi::Local<cXyz> pt;
    cXyz_pl(&i_this->current.pos, pt, l_flag_offset);
    gabi::Local<cXyz> tmp;
    dKyw_get_AllWind_vecpow(tmp, pt);
    gabi::Local<cXyz> wind;
    for (u32 o = 0; o < 12; o += 4) gabi::store<u32>(gabi::ea(wind.get()) + o, gabi::load<u32>(gabi::ea(tmp.get()) + o));
    f32 windStrength = std_sqrtf(PSVECSquareMag(wind));
    f32 currStrength = std_sqrtf(PSVECSquareMag(&i_this->mWindvec));
    if (windStrength > currStrength) {
        for (u32 o = 0; o < 12; o += 4)
            gabi::store<u32>(gabi::ea(&i_this->mWindvec) + o, gabi::load<u32>(gabi::ea(wind.get()) + o));
    } else {
        cLib_addCalcPos2(&i_this->mWindvec, wind, 0.05f, 0.05f);
    }
    tori_flag_execute_tail(i_this);
    return FALSE;
}
VERIFY(0x024C814C, daTori_FlagExecute);

/* 024C8408 daTori_FlagDraw (inlines _draw) */
static BOOL daTori_FlagDraw(void* v) {
    WWHD_FUNC(0x024C8408, BOOL, v);
    daTori_Flag_c* i_this = (daTori_Flag_c*)v;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->mClothTevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    dCloth_packet_vcall(i_this->mpCloth, 0x44); /* cloth_draw */
    return TRUE;
}
VERIFY(0x024C8408, daTori_FlagDraw);

/* 024C85BC */
static BOOL daTori_FlagIsDelete(void* v) {
    WWHD_FUNC(0x024C85BC, BOOL, v);
    return TRUE;
}
VERIFY(0x024C85BC, daTori_FlagIsDelete);

/* 024C85A8 daTori_Flag_HIO_c::~daTori_Flag_HIO_c (deleting) */
static void daTori_Flag_HIO_c_dt(daTori_Flag_HIO_c* i_this, s32 flags) {
    WWHD_FUNC(0x024C85A8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x024C85A8, daTori_Flag_HIO_c_dt);

/* 024C85C4 daTori_Flag_c::~daTori_Flag_c (deleting) */
static void daTori_Flag_c_dt(daTori_Flag_c* i_this, s32 flags) {
    WWHD_FUNC(0x024C85C4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024C85C4, daTori_Flag_c_dt);

/* 024C84E8 __sinit_d_a_tori_flag_cpp */
static void __sinit_d_a_tori_flag_cpp() {
    WWHD_FUNC(0x024C84E8, void);
    sinit_header_statics_z(0x1046E998, 0x101D29B4, 0x1046E9B4);
    daTori_Flag_HIO_c_ct(gabi::at<daTori_Flag_HIO_c>(l_HIO_ea)); /* l_HIO */
    /* l_flag_offset(0.0f, 350.0f, 0.0f) */
    l_flag_offset->x = 0.0f;
    l_flag_offset->y = 350.0f;
    l_flag_offset->z = 0.0f;
}
VERIFY(0x024C84E8, __sinit_d_a_tori_flag_cpp);

/* ---- leftover functions of the translation unit ---- */

/* 024C863C sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10040868, after the destructor 024C85A8 */
static void toriFlag_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x024C863C, void, p);
}
VERIFY(0x024C863C, toriFlag_SafeString_assureTermination);
