/**
 * d_a_obj_ebomzo.cpp (WWHD)
 * Object - Bird statue that falls over when a bomb explodes next to it (dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ebomzo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_ebomzo.h"

const gc_tu_data gc_tu = {.safeString = 0x10027960};
static const char* M_arcname = "Ebomzo";
static Mtx34 M_tmp_mtx;
// HD: packed collision source checked against this image.
static const u32 sph_check_src[] = {0, 0, 0, 0, 32, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1092616192};
enum { dRes_INDEX_EBOMZO_BDL_EBOMZO_e = 4, dRes_INDEX_EBOMZO_DZB_EBOMZO_e = 7 };
enum { JA_SE_OBJ_BOMB_DN_ST_S = 0x69AC, JA_SE_OBJ_BOMB_DN_ST_E = 0x69AD };
namespace dPa_name { enum { ID_AK_SN_BIRDSTATUEHAHEN00 = 0x828E, ID_AK_SN_BIRDSTATUESPLASH00 = 0x828F }; }
enum { Ebomzo_Mode_Check = 0, Ebomzo_Mode_Demo = 1, Ebomzo_Mode_Fall = 2 };
using daObjEbomzo::Act_c;

// HD: inline constructors install this TU's virtual destructor table.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x10027A20;
    dCcD_Stts_ct(&mStts);
    Ebomzo_sphConstruct(&mCollider);
}

// HD: out-of-line parameter helper; the matcher assigns its name to init_mtx instead.
/* 0233A23C */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 023399A4 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EBOMZO_BDL_EBOMZO_e);
    JUT_ASSERT(140, model_data != nullptr);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 02339B34 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -1000.0f, -100.0f, -1000.0f, 1000.0f, 1000.0f, 1000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCollider.Set(reinterpret_cast<const dCcD_SrcSph*>(sph_check_src));
    mCollider.SetStts(&mStts);
    mXRotRate = 0;
    mpParticleEmitter = nullptr;
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mMode = Ebomzo_Mode_Fall;
        current.angle.x = 0x4000;
    } else {
        mMode = Ebomzo_Mode_Check;
    }
    return TRUE;
}

/* 02339858 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_EBOMZO_DZB_EBOMZO_e, 0, 0x1200);
        // HD: assertion line is 193, one lower than GameCube retail.
        JUT_ASSERT(193, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0233A1C8 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 02339958 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result;
}

/* 02339A40 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

// HD: (the matcher names it daObj::PrmAbstract)
/* 02339B14 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 02339C38 */
void Act_c::check() {
    if (mCollider.ChkTgHit()) {
        fopAc_ac_c* acActor = mCollider.GetTgHitAc();
        if (acActor) {
            gabi::Local<cXyz> diffVec;
            cXyz_mi(&mCollider.mSph.mCenter, diffVec, &acActor->current.pos);
            // HD: the unordered comparison also accepts NaN, unlike GameCube <.
            if (!(std_sqrtf(PSVECSquareMag(diffVec)) >= 100.0f)) {
                mXRotRate = 8;
                current.angle.x = (s16)(current.angle.x + 8);
                mDoAud_seStart(JA_SE_OBJ_BOMB_DN_ST_S, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
                mMode = Ebomzo_Mode_Demo;
                dComIfGs_onSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_onSwitch */
                if (!mpParticleEmitter) {
                    /* tevStr.mColorK0 (HD: tevStr+0x98) */
                    const u8 r = tevStr.mColorK0.r;
                    const u8 g = tevStr.mColorK0.g;
                    const u8 b = tevStr.mColorK0.b;
                    mpParticleEmitter = reinterpret_cast<EbomzoEmitter*>(dComIfGp_particle_set(dPa_name::ID_AK_SN_BIRDSTATUEHAHEN00, &current.pos, &current.angle));
                    if (mpParticleEmitter != nullptr)
                        mpParticleEmitter->setGlobalPrmColor(r, g, b);
                }
            }
        }
    }
}

/* 02339DA8 */
void Act_c::demo() {
    s16 rate = mXRotRate;
    rate = (s16)(rate + rate / 8);
    s16 ax = (s16)(current.angle.x + rate);
    mXRotRate = rate;
    if (ax >= 0x4000) {
        current.angle.x = 0x4000;
        mMode = Ebomzo_Mode_Fall;
        dVibration_c* vibration = dComIfGp_getVibration();
        gabi::Local<cXyz> dir;
        dir->set(0.0f, 1.0f, 0.0f);
        vibration->StartShock(4, -33, dir.get());
        mDoAud_seStart(JA_SE_OBJ_BOMB_DN_ST_E, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        if (mpParticleEmitter != nullptr) {
            mpParticleEmitter->becomeInvalidEmitter();
            mpParticleEmitter = nullptr;
        }
        dComIfGp_particle_set(dPa_name::ID_AK_SN_BIRDSTATUESPLASH00, &current.pos, &current.angle);
    } else {
        current.angle.x = ax;
    }
}

/* 02339EE8 */
BOOL Act_c::Execute(gptr<Mtx34>* matrix) {
    gabi::Local<cXyz> offset;
    s32 ay = (u16)current.angle.y;
    offset->x = cM_ssin(ay) * 50.0f;
    offset->y = 320.0f;
    offset->z = cM_scos(ay) * 50.0f;
    gabi::Local<cXyz> c;
    cXyz_pl(&current.pos, c, offset);
    mCollider.SetC(c);
    dComIfG_Ccsp_Set(&mCollider);

    switch ((u32)mMode) {
    case Ebomzo_Mode_Check: check(); break;
    case Ebomzo_Mode_Demo: demo(); break;
    default: fall(); break;
    }

    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    shape_angle.x = current.angle.x;
    set_mtx();
    *matrix = &M_tmp_mtx;
    return TRUE;
}

/* 0233A050 */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 0233A0E8 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0233A0EC */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0233A0F0 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
// HD: MoveBGDraw -> virtual Draw
/* 0233A0F4 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
// HD: MoveBGIsDelete -> virtual IsDelete
/* 0233A104 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 0233A114 */
static void __sinit_d_a_obj_ebomzo_cpp() {
    sinit_header_statics(0x104696DC, 0x101C8F04);
}

// HD: sead::SafeString deleting destructor (this TU's copy; trivial)
/* 0233A1A8 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: dBgS_MoveBgActor::IsDelete (virtual; this TU's copy)
/* 0233A1BC */
static BOOL MoveBgActor_IsDelete(void*) {
    return TRUE;
}

// HD: sead::SafeString::assureTermination (this TU's copy; empty)
/* 0233A1C4 */
static void SafeString_assureTermination(void*) {
}

// HD: daObjEbomzo::Act_c deleting destructor
/* 0233A1D0 */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        Ebomzo_sphDestruct(&i_this->mCollider, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 10027A18
str 100279E0 in 023399A4
str 100279D0 "model_data != nullptr" renamed
str 10027978 in 02339858
str 1002798C "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)" renamed
data sph_check_src 101C8EC4
data M_tmp_mtx 104696F8 nocheck # HD BSS matrix has no image bytes; set_mtx fills it before output.
ext 025166F0 void Ebomzo_sphConstruct(dCcD_Sph*)
ext 02515AE8 void Ebomzo_sphDestruct(dCcD_Sph*, s32)
*/
