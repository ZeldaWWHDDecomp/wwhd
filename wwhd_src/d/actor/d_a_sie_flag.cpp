/**
 * d_a_sie_flag.cpp (WWHD)
 * Object - Forsaken Fortress flag (Sie_Flag: Eshata model with a cloth flag)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sie_flag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1003CD38) /* "Eshata" */
#define SAFESTRING_VTBL 0x1003CC64 /* this TU's copy of the sead::SafeString vtable */
#define SIE_FLAG_VTBL 0x1003CC9C
#define HIO_VTBL 0x1003CC8C
#define AAB_VTBL 0x1003CC7C        /* this TU's cM3dGAab vtable */
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D06EC)
#define l_HIO_ea 0x1046DE84
#define l_flag_offset gabi::at<cXyz>(0x1046DEA4) /* (0, 900, 0) */
#define l_wind_offset gabi::at<cXyz>(0x1046DEB0) /* (0, 725, 0) */

enum {
    dRes_INDEX_ESHATA_BDL_ESHATA_e = 4,
    dRes_INDEX_ESHATA_BTI_ESHATA_e = 7,
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 3,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
WWHD_OPAQUE(ResTIMG);
/* dCloth_packet_c (HD layout: vtable at +0xC; GameCube fields 0x5C.. at +0x88, 0xD8.. at +0xC8) */
struct dCloth_packet_c_l {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0xE4 - 0x10];
    /* 0x0E4 */ be<f32> mWindSpeed;     /* GameCube 0x5C */
    /* 0x0E8 */ be<f32> mWindSpeedWave; /* GameCube 0x60 */
    /* 0x0EC */ u8 _0EC[0x1A0 - 0xEC];
    /* 0x1A0 */ be<f32> mSpring;        /* GameCube 0xD8 */
    /* 0x1A4 */ be<f32> mGravity;
    /* 0x1A8 */ be<f32> mDrag;
    /* 0x1AC */ be<f32> mFlyFlex;
    /* 0x1B0 */ be<f32> mHoistFlex;
    /* 0x1B4 */ be<s16> mWave;
    /* 0x1B6 */ be<s16> mWaveSpeed;
    /* 0x1B8 */ be<s16> field_0xF0;
    /* 0x1BA */ be<s16> field_0xF2;
    /* 0x1BC */ be<s16> mRipple;
    /* 0x1BE */ be<s16> mRotateY;

    void setWindPower(f32 wind, f32 windWave) {
        mWindSpeed = wind;
        mWindSpeedWave = windWave;
    }
    void setParam(f32 spring, f32 grav, f32 drag, f32 flyFlex, f32 hoistFlex, s16 wave, s16 param_1, s16 ripple, s16 rotate, f32 wind, f32 windWave) {
        mSpring = spring;
        mGravity = grav;
        mDrag = drag;
        mFlyFlex = flyFlex;
        mHoistFlex = hoistFlex;
        mWaveSpeed = wave;
        field_0xF2 = param_1;
        mRipple = ripple;
        mRotateY = rotate;
        setWindPower(wind, windWave);
    }
    /* 0251B638 dCloth_packet_c::setMtx(Mtx) */
    void setMtx(Mtx34* m) { gabi::call(0x0251B638, this, m); }
    /* 0251E7A8 dCloth_packet_c::setGlobalWind(cXyz*) */
    void setGlobalWind(cXyz* w) { gabi::call(0x0251E7A8, this, w); }
    /* virtuals (HD vtable at +0xC): deleting destructor +0xC, cloth_move +0x3C, cloth_draw +0x44 */
    void cloth_move() { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x3C), this); }
    void cloth_draw() { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x44), this); }
};
/* 0251BC80 dCloth_packet_create(flag, toon, flyGrid, hoistGrid, flyLen, hoistLen, tevstr, posArr) */
static inline dCloth_packet_c_l* dCloth_packet_create(ResTIMG* flag, ResTIMG* toon, s32 fly, s32 hoist, f32 flyLen, f32 hoistLen, dKy_tevstr_c* tev, void* posArr) {
    return gabi::call<dCloth_packet_c_l*>(0x0251BC80, flag, toon, fly, hoist, flyLen, hoistLen, tev, posArr);
}
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
/* 0257E34C dKyw_get_AllWind_vecpow(cXyz* pos): cXyz through a hidden result pointer */
static inline void dKyw_get_AllWind_vecpow(cXyz* res, cXyz* pos) { gabi::call(0x0257E34C, res, pos); }
static inline void daObj_HitSeStart(cXyz* pos, s32 room, dCcD_GObjInf* o, u32 se) { gabi::call(0x023129C4, pos, room, o, se); }
static inline BOOL fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 d, u32 f) { return gabi::call<BOOL>(0x025D69FC, a, d, f); }
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
static inline void cLib_addCalcPos2(cXyz* p, const cXyz* t, f32 scale, f32 maxStep) { gabi::call(0x0200F164, p, t, scale, maxStep); }

class daSie_Flag_c : public fopAc_ac_c {
public:
    void set_mtx();
    BOOL CreateHeap();
    cPhs_State CreateInit();
    cPhs_State _create();
    bool _delete();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhsEshata; /* GameCube 0x290 */
    /* 0x3B4 */ request_of_phase_process_class mPhsCloth;
    /* 0x3BC */ gptr<J3DModel> mpModel;
    /* 0x3C0 */ gptr<dCloth_packet_c_l> mpClothPacket;
    /* 0x3C4 */ cXyz mWindvec;
    /* 0x3D0 */ dKy_tevstr_c mTevStr;
    /* 0x598 */ dCcD_Stts mStts;
    /* 0x5D4 */ dCcD_Cyl mCyl;
    /* 0x704 */ dCcD_Cyl mCyl2;
};
WWHD_OFFSET(daSie_Flag_c, mTevStr, 0x3D0);
WWHD_OFFSET(daSie_Flag_c, mStts, 0x598);
WWHD_OFFSET(daSie_Flag_c, mCyl2, 0x704);
WWHD_SIZE(daSie_Flag_c, 0x834);

struct daSie_Flag_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0c;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<u32> __vtbl; /* HD: GHS places the vtable pointer after the members */
};
WWHD_SIZE(daSie_Flag_HIO_c, 0x10);

/* 024861C0 */
static daSie_Flag_HIO_c* daSie_Flag_HIO_c_ct(daSie_Flag_HIO_c* h) {
    WWHD_FUNC(0x024861C0, daSie_Flag_HIO_c*, h);
    if (h == nullptr) {
        h = (daSie_Flag_HIO_c*)operator_new(0x10);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->m08 = 0.0f;
    h->m0c = 0;
    h->__vtbl = HIO_VTBL;
    return h;
}
VERIFY(0x024861C0, daSie_Flag_HIO_c_ct);

/* 024859F8: daSie_Flag_c::daSie_Flag_c (inline sub-object constructors) */
static daSie_Flag_c* daSie_Flag_c_ct(daSie_Flag_c* i_this) {
    WWHD_FUNC(0x024859F8, daSie_Flag_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daSie_Flag_c*)operator_new(0x834);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = SIE_FLAG_VTBL;
    /* dKy_tevstr_c mTevStr: three copies of a 0x44-byte light block (template 0x1016E414) at +0, +0xC0, +0x144 */
    const u32 T = 0x1016E414;
    const u32 d[3] = {gabi::ea(&i_this->mTevStr), gabi::ea(&i_this->mTevStr) + 0xC0, gabi::ea(&i_this->mTevStr) + 0x144};
    for (int k = 0; k < 3; k++) {
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(d[k] + o, gabi::load<f32>(T + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d[k] + o, gabi::load<u8>(T + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(d[k] + o, gabi::load<s16>(T + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d[k] + o, gabi::load<f32>(T + o));
    }
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, AAB_VTBL);
    dCcD_Cyl_ct(&i_this->mCyl2, AAB_VTBL);
    return i_this;
}
VERIFY(0x024859F8, daSie_Flag_c_ct);

/* 02485C48 */
void daSie_Flag_c::set_mtx() {
    WWHD_FUNC(0x02485C48, void, this);
    J3DModel_setBaseScale(mpModel, &scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);

    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transM(l_flag_offset->x, l_flag_offset->y, l_flag_offset->z);
    mpClothPacket->setMtx(mDoMtx_stack_c::get());
}
VERIFY(0x02485C48, &daSie_Flag_c::set_mtx);

/* 024859F4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x024859F4, BOOL, i_actor);
    return static_cast<daSie_Flag_c*>(i_actor)->CreateHeap();
}
VERIFY(0x024859F4, CheckCreateHeap);

/* 024858D0 */
BOOL daSie_Flag_c::CreateHeap() {
    WWHD_FUNC(0x024858D0, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ESHATA_BDL_ESHATA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x109, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003CCBC), 0x109, STR(0x1003CCD0));

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (!mpModel) {
        return FALSE;
    } else {
        ResTIMG* eshata_timg = (ResTIMG*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ESHATA_BTI_ESHATA_e, SAFESTRING_VTBL);
        ResTIMG* cloth_timg = (ResTIMG*)dComIfG_getObjectRes(STR(0x1003CCB4) /* "Cloth" */, dRes_INDEX_CLOTH_BTI_CLOTHTOON_e, SAFESTRING_VTBL);
        mpClothPacket = dCloth_packet_create(eshata_timg, cloth_timg, 5, 5, 700.0f, 350.0f, &mTevStr, nullptr);
        return mpClothPacket ? TRUE : FALSE;
    }
}
VERIFY(0x024858D0, &daSie_Flag_c::CreateHeap);

/* 02485D48 */
cPhs_State daSie_Flag_c::CreateInit() {
    WWHD_FUNC(0x02485D48, cPhs_State, this);
    mStts.Init(0xFF, 0xFF, this);

    mCyl.Set(l_cyl_src);
    mCyl.SetStts(&mStts);

    mWindvec.copy(*dKyw_get_wind_vec()); /* struct copy (integer words) */

    set_mtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    fopAcM_setCullSizeBox(this, -700.0f, 0.0f, -700.0f, 700.0f, 1100.0f, 700.0f);

    dKy_tevstr_init(&mTevStr, fopAcM_GetRoomNo(this), 0xFF);

    return cPhs_COMPLEATE_e;
}
VERIFY(0x02485D48, &daSie_Flag_c::CreateInit);

/* 02485E1C */
cPhs_State daSie_Flag_c::_create() {
    WWHD_FUNC(0x02485E1C, cPhs_State, this);
    /* fopAcM_ct(this, daSie_Flag_c): the out-of-line constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daSie_Flag_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State result = dComIfG_resLoad(&mPhsEshata, M_arcname);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    result = dComIfG_resLoad(&mPhsCloth, STR(0x1003CCF0) /* "Cloth" */);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    if (fopAcM_entrySolidHeap(this, 0x024859F4 /* CheckCreateHeap */, 0x1020)) {
        return CreateInit();
    } else {
        return cPhs_ERROR_e;
    }
}
VERIFY(0x02485E1C, &daSie_Flag_c::_create);

/* 02485ECC */
bool daSie_Flag_c::_delete() {
    WWHD_FUNC(0x02485ECC, bool, this);
    dComIfG_resDelete(&mPhsEshata, M_arcname);
    dComIfG_resDelete(&mPhsCloth, STR(0x1003CCF8) /* "Cloth" */);
    /* HD: the cloth packet is deleted here (virtual deleting destructor) */
    dCloth_packet_c_l* pkt = mpClothPacket;
    if (pkt != nullptr) {
        gabi::call_ptr(gabi::load<u32>(pkt->__vtbl + 0xC), pkt, 3);
    }
    mpClothPacket = nullptr;
    return true;
}
VERIFY(0x02485ECC, &daSie_Flag_c::_delete);

/* 02485F44 */
bool daSie_Flag_c::_execute() {
    WWHD_FUNC(0x02485F44, bool, this);
    gabi::Local<cXyz> allwind;
    gabi::Local<cXyz> position_plus_offset;

    set_mtx();
    mStts.Move();

    if (mCyl.ChkTgHit() != 0) {
        daObj_HitSeStart(&current.pos, fopAcM_GetRoomNo(this), &mCyl, 0x0B);
    }

    fopAcM_rollPlayerCrash(this, 40.0f, 0x07);

    cXyz_pl(&current.pos, position_plus_offset, l_flag_offset);
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
        cLib_addCalcPos2(&mWindvec, allwind, 0.1f, 0.1f);
    }

    mCyl.mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);

    mpClothPacket->setParam(0.4f, -0.75f, 0.9f, 1.0f, 1.0f, 0x400, 0, 900, -800, 7.0f, 6.0f);
    mpClothPacket->setWindPower(13.0f, 8.0f);
    mpClothPacket->setGlobalWind(&mWindvec);
    mpClothPacket->cloth_move();

    return false;
}
VERIFY(0x02485F44, &daSie_Flag_c::_execute);

/* 02486138 */
bool daSie_Flag_c::_draw() {
    WWHD_FUNC(0x02486138, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &mTevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    mpClothPacket->cloth_draw();
    return true;
}
VERIFY(0x02486138, &daSie_Flag_c::_draw);

/* 02485EC8 */
static cPhs_State daSie_FlagCreate(void* i_this) {
    WWHD_FUNC(0x02485EC8, cPhs_State, i_this);
    return ((daSie_Flag_c*)i_this)->_create();
}
VERIFY(0x02485EC8, daSie_FlagCreate);

/* 02485F40 */
static BOOL daSie_FlagDelete(void* i_this) {
    WWHD_FUNC(0x02485F40, bool, i_this);
    return ((daSie_Flag_c*)i_this)->_delete();
}
VERIFY(0x02485F40, daSie_FlagDelete);

/* 02486134 */
static BOOL daSie_FlagExecute(void* i_this) {
    WWHD_FUNC(0x02486134, BOOL, i_this);
    /* tail call: r3 passes through unchanged */
    return gabi::call<BOOL>(0x02485F44, i_this); /* ((daSie_Flag_c*)i_this)->_execute() */
}
VERIFY(0x02486134, daSie_FlagExecute);

/* 024861BC */
static BOOL daSie_FlagDraw(void* i_this) {
    WWHD_FUNC(0x024861BC, bool, i_this);
    return ((daSie_Flag_c*)i_this)->_draw();
}
VERIFY(0x024861BC, daSie_FlagDraw);

/* 0248630C */
static BOOL daSie_FlagIsDelete(void*) {
    WWHD_FUNC(0x0248630C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0248630C, daSie_FlagIsDelete);

/* 0248621C: static initialisation: header statics (zeroed object at 0x1046DE94), l_HIO,
 * l_flag_offset, l_wind_offset */
static void __sinit_d_a_sie_flag_cpp() {
    WWHD_FUNC(0x0248621C, void, (u32)0);
    sinit_header_statics_z(0x1046DE78, 0x101D0730, 0x1046DE94);
    daSie_Flag_HIO_c_ct(gabi::at<daSie_Flag_HIO_c>(l_HIO_ea));
    l_wind_offset->x = 0.0f;
    l_wind_offset->y = 725.0f;
    l_wind_offset->z = 0.0f;
    l_flag_offset->x = 0.0f;
    l_flag_offset->y = 900.0f;
    l_flag_offset->z = 0.0f;
}
VERIFY(0x0248621C, __sinit_d_a_sie_flag_cpp);

/* 024862F8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x024862F8, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x024862F8, SafeString_dt);

/* 02486314: daSie_Flag_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daSie_Flag_c_dt(daSie_Flag_c* i_this, s32 flags) {
    WWHD_FUNC(0x02486314, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02486314, daSie_Flag_c_dt);

/* 0248638C: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x0248638C, void, (u32)0);
}
VERIFY(0x0248638C, SafeString_assureTermination);
