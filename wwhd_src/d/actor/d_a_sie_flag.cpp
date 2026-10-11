/**
 * d_a_sie_flag.cpp (WWHD)
 * Object - Forsaken Fortress flag (Sie_Flag: Eshata model with a cloth flag)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sie_flag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sie_flag.h"
// HD: per-unit constructor vtables and header objects.
const gc_tu_data gc_tu = {.safeString = 0x1003CC64, .headerStatics = {0x1046DE78, 0x101D0730, 0x1046DE94}};
enum { SIE_FLAG_VTBL = 0x1003CC9C, HIO_VTBL = 0x1003CC8C, AAB_VTBL = 0x1003CC7C };
enum {
    dRes_INDEX_ESHATA_BDL_ESHATA_e = 4,
    dRes_INDEX_ESHATA_BTI_ESHATA_e = 7,
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 3
};
static dCcD_SrcCyl l_cyl_src;
static daSie_Flag_HIO_c l_HIO;
static cXyz l_flag_offset;
static cXyz l_wind_offset;
static SieFlagLightHD l_lightTemplate;

/* 024861C0 */
static daSie_Flag_HIO_c *daSie_Flag_HIO_c_ct(daSie_Flag_HIO_c *h) {
    if (h == nullptr) {
        h = (daSie_Flag_HIO_c *)operator_new(0x10);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->m08 = 0.0f;
    h->m0c = 0;
    h->__vtbl = HIO_VTBL;
    return h;
}

/* 024859F8 */
static daSie_Flag_c *daSie_Flag_c_ct(daSie_Flag_c *i_this) {
    if (i_this == nullptr) {
        i_this = (daSie_Flag_c *)operator_new(0x834);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = SIE_FLAG_VTBL;
    SieFlagTevCtorHD *tev = (SieFlagTevCtorHD *)&i_this->mTevStr;
    SieFlag_copyLight(&tev->mLight0, &l_lightTemplate);
    SieFlag_copyLight(&tev->mLight1, &l_lightTemplate);
    SieFlag_copyLight(&tev->mLight2, &l_lightTemplate);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, AAB_VTBL);
    dCcD_Cyl_ct(&i_this->mCyl2, AAB_VTBL);
    return i_this;
}

/* 02485C48 */
void daSie_Flag_c::set_mtx() {
    mpModel->setBaseScale(scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);

    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoMtx_stack_c::transM(l_flag_offset.x, l_flag_offset.y, l_flag_offset.z);
    mpClothPacket->setMtx(mDoMtx_stack_c::get());
}

/* 024859F4 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_actor) {
    return static_cast<daSie_Flag_c *>(i_actor)->CreateHeap();
}

/* 024858D0 */
BOOL daSie_Flag_c::CreateHeap() {
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes("Eshata", dRes_INDEX_ESHATA_BDL_ESHATA_e);
    if (modelData == nullptr) /* JUT_ASSERT(0x109, modelData != NULL) */
        JUT_ASSERT_fail("d_a_sie_flag.cpp", 0x109, "modelData != (0)");

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (!mpModel) {
        return FALSE;
    } else {
        ResTIMG *eshata_timg = (ResTIMG *)dComIfG_getObjectRes("Eshata", dRes_INDEX_ESHATA_BTI_ESHATA_e);
        ResTIMG *cloth_timg =
            (ResTIMG *)dComIfG_getObjectRes("Cloth" /* "Cloth" */, dRes_INDEX_CLOTH_BTI_CLOTHTOON_e);
        mpClothPacket =
            dCloth_packet_create(eshata_timg, cloth_timg, 5, 5, 700.0f, 350.0f, &mTevStr, nullptr);
        return mpClothPacket ? TRUE : FALSE;
    }
}

/* 02485D48 */
cPhs_State daSie_Flag_c::CreateInit() {
    mStts.Init(0xFF, 0xFF, this);

    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);

    mWindvec.copy(*dKyw_get_wind_vec()); /* struct copy (integer words) */

    set_mtx();
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -700.0f, 0.0f, -700.0f, 700.0f, 1100.0f, 700.0f);

    dKy_tevstr_init(&mTevStr, fopAcM_GetRoomNo(this), 0xFF);

    return cPhs_COMPLEATE_e;
}

/* 02485E1C */
cPhs_State daSie_Flag_c::_create() {
    /* fopAcM_ct(this, daSie_Flag_c): the out-of-line constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daSie_Flag_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State result = dComIfG_resLoad(&mPhsEshata, "Eshata");
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    result = dComIfG_resLoad(&mPhsCloth, "Cloth" /* "Cloth" */);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x1020)) {
        return CreateInit();
    } else {
        return cPhs_ERROR_e;
    }
}

/* 02485ECC */
bool daSie_Flag_c::_delete() {
    dComIfG_resDelete(&mPhsEshata, "Eshata");
    dComIfG_resDelete(&mPhsCloth, "Cloth" /* "Cloth" */);
    /* HD: the cloth packet is deleted here (virtual deleting destructor) */
    SieFlagClothHD *pkt = mpClothPacket;
    if (pkt != nullptr) {
        pkt->destroy(3);
    }
    mpClothPacket = nullptr;
    return true;
}

/* 02485F44 */
bool daSie_Flag_c::_execute() {
    gabi::Local<cXyz> allwind;
    gabi::Local<cXyz> position_plus_offset;

    set_mtx();
    mStts.Move();

    if (mCyl.ChkTgHit() != 0) {
        daObj_HitSeStart(&current.pos, fopAcM_GetRoomNo(this), &mCyl, 0x0B);
    }

    fopAcM_rollPlayerCrash(this, 40.0f, 0x07);

    cXyz_pl(&current.pos, position_plus_offset, &l_flag_offset);
    {
        gabi::Local<cXyz> tmp;
        dKyw_get_AllWind_vecpow(tmp, position_plus_offset);
        allwind->copy(*tmp.get());
    }
    f32 a = std_sqrtf(PSVECSquareMag(allwind));
    f32 b = std_sqrtf(PSVECSquareMag(&mWindvec));
    if (!(a <= b)) {
        mWindvec.copy(*allwind.get());
    } else {
        cLib_addCalcPos2(&mWindvec, *allwind.get(), 0.1f, 0.1f);
    }

    mCyl.mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);

    mpClothPacket->setParam(0.4f, -0.75f, 0.9f, 1.0f, 1.0f, 0x400, 0, 900, -800, 7.0f, 6.0f);
    mpClothPacket->setWindPower(13.0f, 8.0f);
    mpClothPacket->setGlobalWind(&mWindvec);
    mpClothPacket->cloth_move();

    return false;
}

/* 02486138 */
bool daSie_Flag_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &mTevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    mpClothPacket->cloth_draw();
    return true;
}

/* 02485EC8 */
static cPhs_State daSie_FlagCreate(void *i_this) {
    return ((daSie_Flag_c *)i_this)->_create();
}

/* 02485F40 */
static BOOL daSie_FlagDelete(void *i_this) {
    return ((daSie_Flag_c *)i_this)->_delete();
}

/* 02486134 */
static BOOL daSie_FlagExecute(void *i_this) {
    /* tail call: r3 passes through unchanged */
    return SieFlag_executeHD(i_this); /* ((daSie_Flag_c*)i_this)->_execute() */
}

/* 024861BC */
static BOOL daSie_FlagDraw(void *i_this) {
    return ((daSie_Flag_c *)i_this)->_draw();
}

/* 0248630C */
static BOOL daSie_FlagIsDelete(void *) {
    return TRUE;
}

/* 0248621C */
static void __sinit_d_a_sie_flag_cpp() {
    __sinit_header_statics();
    daSie_Flag_HIO_c_ct(&l_HIO);
    l_wind_offset.x = 0.0f;
    l_wind_offset.y = 725.0f;
    l_wind_offset.z = 0.0f;
    l_flag_offset.x = 0.0f;
    l_flag_offset.y = 900.0f;
    l_flag_offset.z = 0.0f;
}

/* 024862F8 */
static void SafeString_dt(SafeString *s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 02486314 */
static void daSie_Flag_c_dt(daSie_Flag_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mCyl2);
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0248638C */
static void SafeString_assureTermination(SafeString *) {}

/* WWHD glue
data l_cyl_src 101D06EC
data l_HIO 1046DE84
data l_flag_offset 1046DEA4
data l_wind_offset 1046DEB0
data l_lightTemplate 1016E414
str 1003CD38 "Eshata"
str 1003CCB4 in 024858D0 # "Cloth"
str 1003CCF0 in 02485E1C # "Cloth"
str 1003CCF8 in 02485ECC # "Cloth"
str 1003CCBC "d_a_sie_flag.cpp"
str 1003CCD0 "modelData != (0)"
ext 0251B638 void SieFlagClothHD::setMtx(Mtx34*)
ext 0251E7A8 void SieFlagClothHD::setGlobalWind(cXyz*)
ext 0251BC80 SieFlagClothHD* dCloth_packet_create(ResTIMG*, ResTIMG*, s32, s32, f32, f32, dKy_tevstr_c*, void*)
ext 0257E34C void dKyw_get_AllWind_vecpow(cXyz*, cXyz*)
ext 023129C4 void daObj_HitSeStart(cXyz*, s32, dCcD_GObjInf*, u32)
ext 025D69FC BOOL fopAcM_rollPlayerCrash(fopAc_ac_c*, f32, u32)
ext 0255FFF4 void dKy_tevstr_init(dKy_tevstr_c*, s8, u8)
ext 02485F44 BOOL SieFlag_executeHD(void*)
*/
