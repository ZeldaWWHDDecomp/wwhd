/**
 * d_a_tori_flag.cpp (WWHD)
 * Object - Great Sea - Small red flag (Flight Control Platform, Horseshoe Island)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tori_flag.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_tori_flag.h"

// HD: per-unit constructor vtables and common header objects.
const gc_tu_data gc_tu = {.safeString = 0x10040854, .headerStatics = {0x1046E998, 0x101D29B4, 0x1046E9B4}};
enum { TORI_FLAG_VTBL = 0x1004088C, TORI_FLAG_AAB_VTBL = 0x1004086C, HIO_VTBL = 0x1004087C };
enum {
    dRes_INDEX_TRFLAG_BDL_ETHATA_e = 4,
    dRes_INDEX_TRFLAG_BTI_ETHATA_e = 7,
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 3,
};
static dCcD_SrcCyl l_cyl_src;
static cXyz l_flag_offset;
static daTori_Flag_HIO_c l_HIO;
static ToriFlagLightHD l_lightTemplate;
ToriFlagClothHD *dCloth_packet_create(ResTIMG *, ResTIMG *, s32, s32, f32, f32, dKy_tevstr_c *, void *);
void dKyw_get_AllWind_vecpow(cXyz *, cXyz *);
void dKy_tevstr_init(dKy_tevstr_c *, s8, u8);
void daObj_HitSeStart(cXyz *, s32, dCcD_GObjInf *, u32);
void ToriFlag_rollPlayerCrash(fopAc_ac_c *, f32, u32);

// HD: HIO constructor allocates when called with a null object.
/* 024C848C */
static daTori_Flag_HIO_c *daTori_Flag_HIO_c_ct(daTori_Flag_HIO_c *i_this) {
    if (i_this == nullptr) {
        i_this = (daTori_Flag_HIO_c *)operator_new(0x10);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mNo = -1;
    i_this->m08 = 0.0f;
    i_this->m0C = 0;
    i_this->__vtbl = HIO_VTBL;
    return i_this;
}

/* 024C7E50 */
void daTori_Flag_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transM(l_flag_offset.x, l_flag_offset.y, l_flag_offset.z);
    mpCloth->setMtx(mDoMtx_stack_c::get());
}

/* 024C7BFC */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return static_cast<daTori_Flag_c *>(i_this)->CreateHeap();
}

/* 024C7AD8 */
BOOL daTori_Flag_c::CreateHeap() {
    BOOL ret;
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes("Trflag", dRes_INDEX_TRFLAG_BDL_ETHATA_e);
    if (modelData == nullptr) /* JUT_ASSERT(0x120, modelData != NULL) */
        JUT_ASSERT_fail("d_a_tori_flag.cpp", 0x120, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!mpModel) {
        ret = FALSE;
    } else {
        ResTIMG *flagTimg = (ResTIMG *)dComIfG_getObjectRes("Trflag", dRes_INDEX_TRFLAG_BTI_ETHATA_e);
        ResTIMG *clothTimg = (ResTIMG *)dComIfG_getObjectRes("Cloth", dRes_INDEX_CLOTH_BTI_CLOTHTOON_e);
        mpCloth = dCloth_packet_create(flagTimg, clothTimg, 5, 5, 210.0f, 105.0f, &mClothTevStr, nullptr);
        ret = mpCloth ? TRUE : FALSE;
    }
    return ret;
}

/* 024C7F50 */
cPhs_State daTori_Flag_c::CreateInit() {
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    cXyz *wind_vec = dKyw_get_wind_vec();
    // HD: copy wind components as words, preserving NaN payloads.
    mWindvec = *wind_vec;
    set_mtx();
    dKy_tevstr_init(&mClothTevStr, current.roomNo, 0xFF);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    return cPhs_COMPLEATE_e;
}

// HD: out-of-line constructor allocates when called with a null object.
/* 024C7C00 */
static daTori_Flag_c *daTori_Flag_c_ct(daTori_Flag_c *i_this) {
    if (i_this == nullptr) {
        i_this = (daTori_Flag_c *)operator_new(0x834);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = TORI_FLAG_VTBL;
    ToriFlagTevCtorHD *tev = (ToriFlagTevCtorHD *)&i_this->mClothTevStr;
    ToriFlag_copyLight(&tev->mLight0, &l_lightTemplate);
    ToriFlag_copyLight(&tev->mLight1, &l_lightTemplate);
    ToriFlag_copyLight(&tev->mLight2, &l_lightTemplate);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, TORI_FLAG_AAB_VTBL);
    dCcD_Cyl_ct(&i_this->mCyl2, TORI_FLAG_AAB_VTBL);
    return i_this;
}

/* 024C7FF4 */
static cPhs_State daTori_FlagCreate(void *v) {
    daTori_Flag_c *i_this = (daTori_Flag_c *)v;
    /* fopAcM_ct(this, daTori_Flag_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr)
            daTori_Flag_c_ct(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State result = dComIfG_resLoad(&i_this->mPhsTrflag, "Trflag");
    if (result != cPhs_COMPLEATE_e)
        return result;
    result = dComIfG_resLoad(&i_this->mPhsCloth, "Cloth");
    if (result != cPhs_COMPLEATE_e)
        return result;
    if (fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0x1020)) {
        result = i_this->CreateInit();
    } else {
        result = cPhs_ERROR_e;
    }
    return result;
}

// HD: deleting the actor also deletes its cloth packet.
/* 024C80D8 */
static BOOL daTori_FlagDelete(void *v) {
    daTori_Flag_c *i_this = (daTori_Flag_c *)v;
    dComIfG_resDelete(&i_this->mPhsTrflag, "Trflag");
    dComIfG_resDelete(&i_this->mPhsCloth, "Cloth");
    if (i_this->mpCloth)
        i_this->mpCloth->destroy(3);
    i_this->mpCloth = nullptr;
    return TRUE;
}

// HD: GHS duplicates the common tail into both wind branches.
/* inlined into daTori_FlagExecute */
static inline void tori_flag_execute_tail(daTori_Flag_c *i_this) {
    i_this->mCyl.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    ToriFlagClothHD *c = i_this->mpCloth;
    c->setParam(0.4f, -1.5f, 0.75f, 0.9f, 0.9f, 0x400, 0, 900, -800, 7.0f, 6.0f);
    c = i_this->mpCloth;
    c->setWindPower(8.0f, 3.0f);
    i_this->mpCloth->setGlobalWind(&i_this->mWindvec);
    i_this->mpCloth->cloth_move();
}

/* 024C814C */
static BOOL daTori_FlagExecute(void *v) {
    daTori_Flag_c *i_this = (daTori_Flag_c *)v;
    i_this->set_mtx();
    i_this->mStts.Move();
    if (i_this->mCyl.ChkTgHit())
        daObj_HitSeStart(&i_this->current.pos, fopAcM_GetRoomNo(i_this), &i_this->mCyl, 0x0B);
    ToriFlag_rollPlayerCrash(i_this, 40.0f, 7);
    gabi::Local<cXyz> pt;
    cXyz_pl(&i_this->current.pos, pt, &l_flag_offset);
    gabi::Local<cXyz> tmp;
    dKyw_get_AllWind_vecpow(tmp, pt);
    gabi::Local<cXyz> wind;
    *wind.get() = *tmp.get();
    f32 windStrength = std_sqrtf(PSVECSquareMag(wind));
    f32 currStrength = std_sqrtf(PSVECSquareMag(&i_this->mWindvec));
    if (windStrength > currStrength) {
        i_this->mWindvec = *wind.get();
    } else {
        cLib_addCalcPos2(&i_this->mWindvec, *wind.get(), 0.05f, 0.05f);
    }
    tori_flag_execute_tail(i_this);
    return FALSE;
}

/* 024C8408 */
static BOOL daTori_FlagDraw(void *v) {
    daTori_Flag_c *i_this = (daTori_Flag_c *)v;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->mClothTevStr);
    dScnKy_env_light_c *env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    i_this->mpCloth->cloth_draw();
    return TRUE;
}

/* 024C85BC */
static BOOL daTori_FlagIsDelete(void *v) {
    return TRUE;
}

/* 024C85A8 */
static void daTori_Flag_HIO_c_dt(daTori_Flag_HIO_c *i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 024C85C4 */
static void daTori_Flag_c_dt(daTori_Flag_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024C84E8 */
static void __sinit_d_a_tori_flag_cpp() {
    __sinit_header_statics();
    daTori_Flag_HIO_c_ct(&l_HIO);
    /* l_flag_offset(0.0f, 350.0f, 0.0f) */
    l_flag_offset.x = 0.0f;
    l_flag_offset.y = 350.0f;
    l_flag_offset.z = 0.0f;
}

// HD: compiler-generated SafeString virtual slot.

/* 024C863C */
static void toriFlag_SafeString_assureTermination(void *p) {}

/* WWHD glue
data l_cyl_src 101D2970
data l_HIO 1046E9A4
data l_flag_offset 1046E9C4
data l_lightTemplate 1016E414
str 10040918 "Trflag"
str 100408A4 in 024C7AD8 # "Cloth"
str 1004084C in 024C7FF4 # "Cloth"
str 100408D4 in 024C80D8 # "Cloth"
str 100408AC "d_a_tori_flag.cpp"
str 100408C0 "modelData != NULL"
ext 0251B638 void ToriFlagClothHD::setMtx(Mtx34*)
ext 0251E7A8 void ToriFlagClothHD::setGlobalWind(cXyz*)
ext 0251BC80 ToriFlagClothHD* dCloth_packet_create(ResTIMG*, ResTIMG*, s32, s32, f32, f32, dKy_tevstr_c*, void*)
ext 0257E34C void dKyw_get_AllWind_vecpow(cXyz*, cXyz*)
ext 0255FFF4 void dKy_tevstr_init(dKy_tevstr_c*, s8, u8)
ext 023129C4 void daObj_HitSeStart(cXyz*, s32, dCcD_GObjInf*, u32)
ext 025D69FC void ToriFlag_rollPlayerCrash(fopAc_ac_c*, f32, u32)
*/
