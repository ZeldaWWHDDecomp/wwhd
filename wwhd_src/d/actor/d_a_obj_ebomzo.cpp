/**
 * d_a_obj_ebomzo.cpp (WWHD)
 * Object - Bird statue that falls over when a bomb explodes next to it (dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ebomzo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10027A18)  /* "Ebomzo" */
#define SAFESTRING_VTBL 0x10027960 /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x10027A20        /* daObjEbomzo::Act_c vtable (HD) */
#define sph_check_src 0x101C8EC4   /* dCcD_SrcSph */
#define M_tmp_mtx gabi::at<Mtx34>(0x104696F8)

enum {
    dRes_INDEX_EBOMZO_BDL_EBOMZO_e = 4,
    dRes_INDEX_EBOMZO_DZB_EBOMZO_e = 7,
};
enum {
    JA_SE_OBJ_BOMB_DN_ST_S = 0x69AC,
    JA_SE_OBJ_BOMB_DN_ST_E = 0x69AD,
};
enum { dPa_name_ID_AK_SN_BIRDSTATUEHAHEN00 = 0x828E, dPa_name_ID_AK_SN_BIRDSTATUESPLASH00 = 0x828F }; /* HD ids */
enum { Ebomzo_Mode_Check = 0, Ebomzo_Mode_Demo = 1, Ebomzo_Mode_Fall = 2 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025166F0 dCcD_Sph::dCcD_Sph (out of line) */
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
/* 02515AE8 dCcD_Sph::~dCcD_Sph (matcher_errors.tsv) */
static inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }

namespace daObjEbomzo {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void check();
    void demo();
    void fall() {}
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Sph mCollider;
    /* 0x554 */ be<s32> mMode;
    /* 0x558 */ be<s16> mXRotRate;
    /* 0x55A */ u8 _55A[2];
    /* 0x55C */ gptr<JPABaseEmitter> mpParticleEmitter;
};
WWHD_OFFSET(Act_c, mCollider, 0x428);
WWHD_OFFSET(Act_c, mMode, 0x554);
WWHD_OFFSET(Act_c, mpParticleEmitter, 0x55C);
}  // namespace daObjEbomzo
using daObjEbomzo::Act_c;

/* 0233A23C: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU;
 * the matcher puts this name on 02339B14, which is init_mtx) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0233A23C, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0233A23C, PrmAbstract);
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 023399A4 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023399A4, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EBOMZO_BDL_EBOMZO_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(140, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x100279E0), 0x8C, STR(0x100279D0));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x023399A4, &Act_c::CreateHeap);

/* 02339B34 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x02339B34, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -1000.0f, -100.0f, -1000.0f, 1000.0f, 1000.0f, 1000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCollider.Set(gabi::at<dCcD_SrcSph>(sph_check_src));
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
VERIFY(0x02339B34, &Act_c::Create);

/* 02339858 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x02339858, cPhs_State, this);
    /* fopAcM_ct(this, daObjEbomzo::Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Sph_ct(&mCollider);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_EBOMZO_DZB_EBOMZO_e, 0, 0x1200);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(193, ...) */
            JUT_ASSERT_fail(STR(0x10027978), 0xC1, STR(0x1002798C));
    }
    return phase_state;
}
VERIFY(0x02339858, &Act_c::Mthd_Create);

/* 0233A1C8 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x0233A1C8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0233A1C8, Act_c_Delete);

/* 02339958 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x02339958, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result;
}
VERIFY(0x02339958, &Act_c::Mthd_Delete);

/* 02339A40 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02339A40, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x02339A40, &Act_c::set_mtx);

/* 02339B14 (the matcher names it daObj::PrmAbstract) */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02339B14, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x02339B14, &Act_c::init_mtx);

/* 02339C38 */
void Act_c::check() {
    WWHD_FUNC(0x02339C38, void, this);
    if (mCollider.ChkTgHit()) {
        fopAc_ac_c* acActor = mCollider.GetTgHitAc();
        if (acActor) {
            gabi::Local<cXyz> diffVec;
            cXyz_mi(&mCollider.mSph.mCenter, diffVec, &acActor->current.pos);
            if (!(std_sqrtf(PSVECSquareMag(diffVec)) >= 100.0f)) { /* diffVec.abs() < 100.0f */
                mXRotRate = 8;
                current.angle.x = (s16)(current.angle.x + 8);
                mDoAud_seStart(JA_SE_OBJ_BOMB_DN_ST_S, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
                mMode = Ebomzo_Mode_Demo;
                dComIfGs_onSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_onSwitch */
                if (!mpParticleEmitter) {
                    /* tevStr.mColorK0 (HD: tevStr+0x98) */
                    const u8 r = gabi::load<u8>(gabi::ea(this) + 0x1A8);
                    const u8 g = gabi::load<u8>(gabi::ea(this) + 0x1A9);
                    const u8 b = gabi::load<u8>(gabi::ea(this) + 0x1AA);
                    mpParticleEmitter = dComIfGp_particle_set(dPa_name_ID_AK_SN_BIRDSTATUEHAHEN00, &current.pos, &current.angle);
                    u32 e = gabi::ea((JPABaseEmitter*)mpParticleEmitter);
                    if (e) { /* setGlobalPrmColor (HD: +0x244) */
                        gabi::store<u8>(e + 0x244, r);
                        gabi::store<u8>(e + 0x245, g);
                        gabi::store<u8>(e + 0x246, b);
                    }
                }
            }
        }
    }
}
VERIFY(0x02339C38, &Act_c::check);

/* 02339DA8 */
void Act_c::demo() {
    WWHD_FUNC(0x02339DA8, void, this);
    s16 rate = mXRotRate;
    rate = (s16)(rate + rate / 8);
    s16 ax = (s16)(current.angle.x + rate);
    mXRotRate = rate;
    if (ax >= 0x4000) {
        current.angle.x = 0x4000;
        mMode = Ebomzo_Mode_Fall;
        u8* play = dComIfGp_get();
        gabi::Local<cXyz> dir;
        dir->set(0.0f, 1.0f, 0.0f);
        gabi::call<BOOL>(0x025CB374, play + PLAY_VIBRATION, 4, -33, dir.get()); /* StartShock(4, -33, cXyz(0, 1, 0)) */
        mDoAud_seStart(JA_SE_OBJ_BOMB_DN_ST_E, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        u32 e = gabi::ea((JPABaseEmitter*)mpParticleEmitter);
        if (e) { /* becomeInvalidEmitter */
            gabi::store<s32>(e + 0x5C, -1);
            gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
            mpParticleEmitter = nullptr;
        }
        dComIfGp_particle_set(dPa_name_ID_AK_SN_BIRDSTATUESPLASH00, &current.pos, &current.angle);
    } else {
        current.angle.x = ax;
    }
}
VERIFY(0x02339DA8, &Act_c::demo);

/* 02339EE8 */
BOOL Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x02339EE8, BOOL, this, matrix);
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
    gabi::store<u32>(gabi::ea(matrix), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x02339EE8, &Act_c::Execute);

/* 0233A050 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0233A050, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0233A050, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 0233A0E8 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0233A0E8, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0233A0E8, Mthd_Create);
/* 0233A0EC */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0233A0EC, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0233A0EC, Mthd_Delete);
/* 0233A0F0 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0233A0F0, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0233A0F0, Mthd_Execute);
/* 0233A0F4: MoveBGDraw -> virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0233A0F4, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0233A0F4, Mthd_Draw);
/* 0233A104: MoveBGIsDelete -> virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0233A104, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0233A104, Mthd_IsDelete);

/* 0233A114 */
static void __sinit_d_a_obj_ebomzo_cpp() {
    WWHD_FUNC(0x0233A114, void, (u32)0);
    sinit_header_statics(0x104696DC, 0x101C8F04);
}
VERIFY(0x0233A114, __sinit_d_a_obj_ebomzo_cpp);

/* 0233A1A8: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0233A1A8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0233A1A8, SafeString_dt);

/* 0233A1BC: dBgS_MoveBgActor::IsDelete (virtual; this TU's copy) */
static BOOL MoveBgActor_IsDelete(void*) {
    WWHD_FUNC(0x0233A1BC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0233A1BC, MoveBgActor_IsDelete);

/* 0233A1C4: sead::SafeString::assureTermination (this TU's copy; empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0233A1C4, void, (u32)0);
}
VERIFY(0x0233A1C4, SafeString_assureTermination);

/* 0233A1D0: daObjEbomzo::Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0233A1D0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Sph_dt(&i_this->mCollider, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0233A1D0, Act_c_dt);
