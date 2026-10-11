/** Eight model/collision test-object variants. */
#include "d/actor/d_a_obj_mtest.h"
#include "bindings.h"
using Test = daObjMtest::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x1002DC60};
static const char* M_arcname[] = {"Mtest", "Mtest", "Mtest", "Mtest", "Mtest", "Mtest", "Owater", "Astop"};
static MtestScaleTableHD M_scl_mult;
// HD: native cylinder source, guest pointers and per-unit header objects.
static MtestCylinderSourceHD Mtest_cyl_src;
static cXyz l_zero;
static MtestHeaderZeroHD l_headerZero;
static MtestPiPairHD l_headerPi;
static MtestPiPairHD l_piConstants;
static MtestHeaderByteHD l_headerByte;
static MtestHeaderByteHD l_headerFlag;
static MtestRegistrationHD l_registrationFirst;
static MtestRegistrationHD l_registrationSecond;
static MtestRegistrationHD l_registrationThird;
static s16 dzb_data[] = {8,  12, 8, 12, 15, 16, 3,  4,  9,  13, 9, 13, 15, 16, 3, 5,  11, 14, 11, 14,
                         15, 16, 3, 6,  10, 12, 10, 12, 15, 16, 3, 7,  8,  12, 8, 12, 15, 16, 3,  3};
static u32 heap_size[] = {5664,  8352,  5664,  8352,  512,   512,   16384, 16384, 5664,  8352,
                          5664,  8352,  512,   512,   16384, 16384, 5664,  8352,  5664,  8352,
                          512,   512,   16384, 16384, 5664,  8352,  5664,  8352,  512,   512,
                          16384, 16384, 5664,  8352,  5664,  8352,  512,   512,   16384, 16384};
static s16 bmd_data[] = {-1, -1, -1, -1, -1, -1, -1, -1};
void Mtest_constructMoveBg(Test *);
s32 Mtest_isSwitch(dSv_info_c *, s32, s32);
void Mtest_assert(const char *, s32, const char *);
s32 Mtest_resLoad(request_of_phase_process_class *, const char *);
// HD: pass the native return register through to the appearance byte.
s32 Mtest_checkAppearNative(Test *);
s32 Mtest_MoveBGCreate(Test *, const char *, s32, void *, u32);
void Mtest_releaseBackground(dBgS *, dBgW *);
s32 Mtest_MoveBGDelete(Test *);
void Mtest_resDelete(request_of_phase_process_class *, const char *);
void Mtest_copyMatrix(const Mtx34 *, Mtx34 *);
void Mtest_setMatrixNative(Test *);
void Mtest_initMatrixNative(Test *);
void Mtest_registerBackground(dBgS *, dBgW *, fopAc_ac_c *);
void Mtest_moveCylinder(dCcD_Cyl *, cXyz *);
void Mtest_registerCollider(cCcS *, dCcD_Cyl *);
dScnKy_env_light_c *Mtest_getEnvLight();
void Mtest_settingTev(dScnKy_env_light_c *, s32, cXyz *, dKy_tevstr_c *);
void Mtest_setLight(dScnKy_env_light_c *, J3DModel *, dKy_tevstr_c *);
s32 Mtest_createNative(Test *);
s32 Mtest_deleteNative(Test *);
s32 Mtest_executeNative(Test *);
s32 Mtest_getParameter(Test *, s32, s32);
void Mtest_registerGlobal(MtestRegistrationHD *);
void Mtest_initializeHeaderByte(MtestHeaderByteHD *);
void Mtest_initializeHeaderFlag(MtestHeaderByteHD *);

/* 0237DC40 */
BOOL daObjMtest::Act_c::chk_appear() {
    s32 save = Mtest_getParameter(this, 8, 8), arg = Mtest_getParameter(this, 4, 16);
    if (arg == 0 || arg == 1) {
        s32 enabled = Mtest_isSwitch(dComIfGs_info(), save, (s32)(s8)home.roomNo);
        return arg == 0 ? enabled != 0 : enabled == 0;
    }
    return 1;
}
/* 0237DD18 */
s32 daObjMtest::Act_c::Mthd_Create() {
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Mtest_constructMoveBg(this);
            __vtbl = 0x1002DD9C;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1002DC78);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s32 type = Mtest_getParameter(this, 3, 0);
    M_type = type;
    if (type >= Type_Max) {
        Mtest_assert("d_a_obj_mtest.cpp", 335, "M_type < Type_Max");
        type = M_type;
    }
    s32 phase = Mtest_resLoad(&mPhase, M_arcname[type]);
    if (phase == cPhs_COMPLEATE_e) {
        s32 arg = Mtest_getParameter(this, 4, 24), row = arg >= 1 && arg <= 4 ? arg : 0;
        mbAppear = (u8)Mtest_checkAppearNative(this);
        type = M_type;
        const cXyz &scl_mult = M_scl_mult.mScales[type];
        scale.x = (f32)scale.x * (f32)scl_mult.x;
        scale.y = (f32)scale.y * (f32)scl_mult.y;
        scale.z = (f32)scale.z * (f32)scl_mult.z;
        u32 index = (u32)row * Type_Max + (u32)type;
        phase = Mtest_MoveBGCreate(this, M_arcname[type], (s32)dzb_data[index], nullptr, heap_size[index]);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            Mtest_assert("d_a_obj_mtest.cpp", 458,
                         "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
        else if (phase == cPhs_COMPLEATE_e) {
            ((MtestBackgroundHD *)mpBgW.get())->mFlags |= 0x80;
            if (!mbAppear)
                Mtest_releaseBackground(dComIfG_Bgsp(), mpBgW.get());
        }
    }
    return phase;
}
/* 0237E114 */
s32 daObjMtest::Act_c::Mthd_Delete() {
    s32 result = Mtest_MoveBGDelete(this);
    Mtest_resDelete(&mPhase, M_arcname[M_type]);
    return result;
}
/* 0237E16C */
BOOL daObjMtest::Act_c::CreateHeap() {
    s32 index = bmd_data[M_type];
    if (index < 0) {
        mpModel = nullptr;
        return 1;
    }
    J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes(M_arcname[M_type], index);
    if (!data)
        Mtest_assert("d_a_obj_mtest.cpp", 265, "model_data != NULL");
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    return mpModel != 0;
}
/* 0237E24C */
void daObjMtest::Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    if (mpModel)
        J3DModel_setBaseTRMtx(mpModel.get(), mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    Mtest_copyMatrix(mDoMtx_stack_c::get(), &mMtx);
}
/* 0237E334 */
void daObjMtest::Act_c::init_mtx() {
    if (mpModel)
        J3DModel_setBaseScale(mpModel.get(), &scale);
    Mtest_setMatrixNative(this);
}
/* 0237E35C */
BOOL daObjMtest::Act_c::Create() {
    fopAcM_SetMtx(this, &mMtx);
    Mtest_initMatrixNative(this);
    if (M_type == Type_MwtrSB || M_type == Type_MygnSB)
        fopAcM_setCullSizeBox(this, -3000.f, -10.f, -3000.f, 3000.f, 10.f, 3000.f);
    else if (M_type != Type_Owater)
        fopAcM_setCullSizeBox(this, -51.f, -1.f, -51.f, 51.f, 101.f, 51.f);
    if (M_type == Type_Astop)
        actor_status |= fopAcStts_NOCULLEXEC_e;
    if (Mtest_getParameter(this, 4, 24) == 0) {
        mStts.Init(255, 255, this);
        mCyl.Set(&Mtest_cyl_src);
        mCyl.SetStts(&mStts);
        f32 x = scale.x, z = scale.z;
        mCyl.mGObjTg.mVec = l_zero;
        mCyl.mGObjTg.mSPrm |= 4;
        f32 smaller = ((f64)z - (f64)x) >= 0. ? x : z;
        mCyl.SetR(50.f * smaller);
        mCyl.SetH(100.f * (f32)scale.y);
    }
    return 1;
}
/* 0237E4D8 */
BOOL daObjMtest::Act_c::Execute(be<u32> *matrixOut) {
    s32 appear = Mtest_checkAppearNative(this);
    if (mCyl.ChkTgHit())
        fopAcM_delete(this);
    else if (appear) {
        if (!mbAppear)
            Mtest_registerBackground(dComIfG_Bgsp(), mpBgW.get(), this);
        if (Mtest_getParameter(this, 4, 24) == 0) {
            Mtest_moveCylinder(&mCyl, &current.pos);
            Mtest_registerCollider(dComIfG_Ccsp(), &mCyl);
        }
    } else if (mbAppear)
        Mtest_releaseBackground(dComIfG_Bgsp(), mpBgW.get());
    *matrixOut = gabi::ea(&mMtx);
    mbAppear = (u8)appear;
    return 1;
}
/* 0237E5E0 */
BOOL daObjMtest::Act_c::Draw() {
    if (mbAppear && mpModel) {
        s32 arg = Mtest_getParameter(this, 4, 24);
        if (arg != 3 && arg != 4) {
            dScnKy_env_light_c *light = Mtest_getEnvLight();
            Mtest_settingTev(light, 1, &current.pos, &tevStr);
            light = Mtest_getEnvLight();
            Mtest_setLight(light, mpModel.get(), &tevStr);
            dComIfGd_setListBG();
            mDoExt_modelUpdateDL(mpModel.get());
            dComIfGd_setList();
        }
    }
    return 1;
}
/* 0237E6B0 */
s32 daObjMtest_Mthd_Create(Test *test) {
    return Mtest_createNative(test);
}
/* 0237E6B4 */
s32 daObjMtest_Mthd_Delete(Test *test) {
    return Mtest_deleteNative(test);
}
/* 0237E6B8 */
s32 daObjMtest_Mthd_Execute(Test *test) {
    return Mtest_executeNative(test);
}
/* 0237E6BC */
s32 daObjMtest_Mthd_Draw(Test *test) {
    return test->Draw_v();
}
/* 0237E6CC */
s32 daObjMtest_Mthd_IsDelete(Test *test) {
    return test->IsDelete_v();
}
// HD: compiler-generated header objects and scale-table initialization.
/* 0237E6DC */
void __sinit_d_a_obj_mtest_cpp() {
    for (s32 i = 3; i >= 0; i--)
        l_headerZero.mWords[i] = 0;
    Mtest_registerGlobal(&l_registrationFirst);
    l_headerPi.mMin = l_piConstants.mMin;
    l_headerPi.mMax = l_piConstants.mMax;
    Mtest_initializeHeaderByte(&l_headerByte);
    Mtest_registerGlobal(&l_registrationSecond);
    Mtest_initializeHeaderFlag(&l_headerFlag);
    Mtest_registerGlobal(&l_registrationThird);
    const f32 values[24] = {1,   1,   1,   1,   .5f, 1,   10, 10, 10, 10, 5, 10,
                            .2f, .2f, .2f, .2f, .2f, .2f, 1,  1,  1,  40, 1, 40};
    for (u32 i = 0; i < 24; i++)
        Mtest_setScaleComponent(M_scl_mult.mScales[i / 3], i % 3, values[i]);
}
// HD: the header object has a deleting destructor.
/* 0237E804 */
void Mtest_headerDeletingDestructor(void *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0237E818 */
BOOL Mtest_MoveBgIsDelete(Test *test) {
    return 1;
}
/* 0237E820 */
void Mtest_MoveBgNoop() {}
/* 0237E824 */
BOOL daObjMtest_Delete(Test *test) {
    return 1;
}
/* 0237E82C */
void daObjMtest_deletingDestructor(Test *test, s32 flags) {
    if (test) {
        __dt(&test->mCyl);
        __dt(&test->mStts);
        __dt(test);
        if (flags & 1)
            operator_delete(test);
    }
}
// HD: GHS masks shift counts to six bits; counts with bit 5 set produce zero.
/* 0237E898 */
u32 daObjMtest_PrmAbstract(Test *test, u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0 : 1u << (width & 31),
        value = (shift & 32) ? 0 : ((u32)test->mParameters >> (shift & 31));
    return value & (mask - 1);
}

/* WWHD glue
data M_arcname 101CBFC8
data M_scl_mult 1046BA14
data Mtest_cyl_src 1002DD58
data l_zero 101FFBA8
data l_headerZero 1046BA04
data l_headerPi 1046B9F8
data l_piConstants 1002DD40
data l_headerByte 1046BA00
data l_headerFlag 1046BA01
data l_registrationFirst 101CBF74
data l_registrationSecond 101CBF80
data l_registrationThird 101CBF8C
data dzb_data 101CBE74
data heap_size 101CBEC4
data bmd_data 101CBF64
str 1002DC88 in 0237DD18 # "d_a_obj_mtest.cpp"
str 1002DC9C in 0237DD18 # "M_type < Type_Max"
str 1002DCB0 in 0237DD18 # "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1002DD04 in 0237E16C # "d_a_obj_mtest.cpp"
str 1002DCF4 in 0237E16C # "model_data != NULL"
ext 024F1D40 void Mtest_constructMoveBg(Test*)
ext 025BA0C0 s32 Mtest_isSwitch(dSv_info_c*, s32, s32)
ext 0273AA24 void Mtest_assert(const char*, s32, const char*)
ext 02520460 s32 Mtest_resLoad(request_of_phase_process_class*, const char*)
ext 0237DC40 s32 Mtest_checkAppearNative(Test*)
ext 024F1D9C s32 Mtest_MoveBGCreate(Test*, const char*, s32, void*, u32)
ext 020087EC void Mtest_releaseBackground(dBgS*, dBgW*)
ext 024F1F64 s32 Mtest_MoveBGDelete(Test*)
ext 025204C8 void Mtest_resDelete(request_of_phase_process_class*, const char*)
ext 028E90D4 void Mtest_copyMatrix(const Mtx34*, Mtx34*)
ext 0237E24C void Mtest_setMatrixNative(Test*)
ext 0237E334 void Mtest_initMatrixNative(Test*)
ext 024EEA6C void Mtest_registerBackground(dBgS*, dBgW*, fopAc_ac_c*)
ext 025165A4 void Mtest_moveCylinder(dCcD_Cyl*, cXyz*)
ext 0200E240 void Mtest_registerCollider(cCcS*, dCcD_Cyl*)
ext 02555D0C dScnKy_env_light_c* Mtest_getEnvLight()
ext 025626A4 void Mtest_settingTev(dScnKy_env_light_c*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void Mtest_setLight(dScnKy_env_light_c*, J3DModel*, dKy_tevstr_c*)
ext 0237DD18 s32 Mtest_createNative(Test*)
ext 0237E114 s32 Mtest_deleteNative(Test*)
ext 024F1E9C s32 Mtest_executeNative(Test*)
ext 0237E898 s32 Mtest_getParameter(Test*, s32, s32)
ext 028F026C void Mtest_registerGlobal(MtestRegistrationHD*)
ext 028ED6F8 void Mtest_initializeHeaderByte(MtestHeaderByteHD*)
ext 028EAB2C void Mtest_initializeHeaderFlag(MtestHeaderByteHD*)
*/
