/**
 * d_a_fallrock.cpp (WWHD)
 * Falling rock (breaks on landing, splashes in water or lava).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fallrock.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x1000E850) /* "Always" */
#define SAFESTRING_VTBL 0x1000E6BC
#define FALLROCK_VTBL 0x1000E7E4  /* HD: daFallRock_c vtable */
#define m_cyl_src gabi::at<dCcD_SrcCyl>(0x1000E858)
#define AAB_VTBL 0x1000E6D4       /* this TU's cM3dGAab vtable */
static const dBgS_ObjAcch_vt FALLROCK_OBJACCH_VT = {0x1000E7B4, 0x1000E7D4, 0x1000E7C4};
/* dBgS_ObjGndChk_Yogan: this TU's vtables */
static const dBgS_GndChk_vt YOGAN_VT = {0x1000E774, 0x1000E784, 0x1000E7A4, 0x1000E794};

enum {
    dRes_INDEX_ALWAYS_BDL_KROCK_00_e = 0x20,
    dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e = 0x30,
    dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e = 0x66,
};
enum {
    JA_SE_OBJ_BREAK_ROCK = 0x692C,
    JA_SE_OBJ_FALL_WATER_M = 0x6919,
    JA_SE_OBJ_FALL_WATER_L = 0x691A,
    JA_SE_OBJ_FALL_MAGMA_M = 0x691C,
    JA_SE_OBJ_FALL_MAGMA_L = 0x691D,
};

struct daFallRock_c : fopAc_ac_c {
    cPhs_State create();
    BOOL execute();
    BOOL CreateHeap();
    void set_mtx();
    void setParticle(int, cXyz*);

    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
    /* 0x3B8 */ dBgS_ObjAcch field_0x29C;
    /* 0x57C */ dBgS_AcchCir field_0x460;
    /* 0x5BC */ dCcD_Stts mStts;
    /* 0x5F8 */ dCcD_Cyl mCyl;
    /* 0x728 */ GXColor field_0x60C;
    /* 0x72C */ be<f32> field_0x610;
    /* 0x730 */ be<s32> field_0x614;
};
WWHD_OFFSET(daFallRock_c, field_0x29C, 0x3B8);
WWHD_OFFSET(daFallRock_c, mCyl, 0x5F8);
WWHD_OFFSET(daFallRock_c, field_0x614, 0x730);
WWHD_SIZE(daFallRock_c, 0x734);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E2DA8 mDoExt_modelUpdate(J3DModel*) (HD: one argument) */
static inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
/* 025DAF3C fopKyM_createMpillar(const cXyz* pos, f32 scale) (also local in tsubo_dmg) */
static inline void fopKyM_createMpillar(const cXyz* pos, f32 s) { gabi::call(0x025DAF3C, pos, s); }
/* 025A3BDC dPa_J3DmodelEmitter_c::dPa_J3DmodelEmitter_c (allocates when this == NULL; HD: an extra
 * argument before the tevstr, 1 here) (also local in tsubo_mode) */
static inline void* new_dPa_J3DmodelEmitter(JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i) {
    return gabi::call<void*>(0x025A3BDC, (u32)0, e, d, hd, tev, btp, n, i);
}
/* 0200FE78 cLs_Addition(list, node): dComIfGp_particle_addModelEmitter (the list at particle+0x130) */
static inline void dComIfGp_particle_addModelEmitter(void* e) {
    u32 pa = gabi::ea(dComIfGp_getParticle());
    gabi::call(0x0200FE78, gabi::load<u32>(pa + 0x130), e);
}
/* 025F19F8 mDoMtx_XYZrotM(Mtx, s16, s16, s16) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* tevStr: mColorC0 (s16 x4) +0x90, mColorK0 (GXColor) +0x98 */
static inline s16 tev_colorC0(fopAc_ac_c* a, int i) { return gabi::load<s16>(gabi::ea(&a->tevStr) + 0x90 + 2 * i); }
static inline GXColor* tev_colorK0(fopAc_ac_c* a) { return gabi::at<GXColor>(gabi::ea(&a->tevStr) + 0x98); }
/* 02008DAC cBgS_Chk::~cBgS_Chk (the dBgS_GndChk destructor's tail) */
static inline void cBgS_Chk_dt(void* chk, s32 flags) { gabi::call(0x02008DAC, chk, flags); }

/* 021311AC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021311AC, BOOL, i_this);
    return ((daFallRock_c*)i_this)->CreateHeap();
}
VERIFY(0x021311AC, CheckCreateHeap);

/* 02131114 */
BOOL daFallRock_c::CreateHeap() {
    WWHD_FUNC(0x02131114, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BDL_KROCK_00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(161, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000E7F4), 0xa1, STR(0x1000E808));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    return TRUE;
}
VERIFY(0x02131114, &daFallRock_c::CreateHeap);

/* 02131418 */
void daFallRock_c::set_mtx() {
    WWHD_FUNC(0x02131418, void, this);
    J3DModel* model = mpModel;
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, -(scale.x * 120.0f), 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}
VERIFY(0x02131418, &daFallRock_c::set_mtx);

/* 021311B0: HD: mDoExt_modelUpdate only, no simple shadow */
static BOOL daFallRock_Draw(daFallRock_c* i_this) {
    WWHD_FUNC(0x021311B0, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdate(i_this->mpModel);
    return TRUE;
}
VERIFY(0x021311B0, daFallRock_Draw);

/* 02131208 */
void daFallRock_c::setParticle(int param_1, cXyz* pos) {
    WWHD_FUNC(0x02131208, void, this, param_1, pos);
    gabi::Local<cXyz> particle_scale;
    gabi::Local<cXyz> pos_copy;

    field_0x60C.r = (u8)tev_colorC0(this, 0);
    field_0x60C.g = (u8)tev_colorC0(this, 1);
    field_0x60C.b = (u8)tev_colorC0(this, 2);
    field_0x60C.a = (u8)tev_colorC0(this, 3);

    f32 s = 5.0f * scale.x;
    particle_scale->set(s, s, s);
    pos_copy->set(pos->x, pos->y, pos->z);

    switch ((u32)param_1) {
    case 0: {
        dComIfGp_particle_set(0x3E3 /* dPa_name::ID_IT_JN_STS_HAHEN */, pos_copy.get(), nullptr, particle_scale.get(), 0xFF,
                              nullptr, -1, tev_colorK0(this), tev_colorK0(this));
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e, SAFESTRING_VTBL);
        void* anmTexPattern = dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e, SAFESTRING_VTBL);
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x3E2 /* ID_IT_JN_M_STS_HAHEN */, pos_copy.get(), nullptr,
                                                        particle_scale.get(), 0xFF, nullptr, -1, &field_0x60C, tev_colorK0(this));
        if (emitter != nullptr && modelData != nullptr && anmTexPattern != nullptr) {
            void* modelEmitter = new_dPa_J3DmodelEmitter(emitter, modelData, 1, &tevStr, anmTexPattern, 0, 0);
            if (modelEmitter != nullptr) {
                dComIfGp_particle_addModelEmitter(modelEmitter);
            }
        }
        break;
    }
    case 1:
        fopKyM_createMpillar(pos, scale.x);
        break;
    case 2:
        fopKyM_createWpillar(pos, scale.x, 1.0f, 0);
        break;
    }
}
VERIFY(0x02131208, &daFallRock_c::setParticle);

/* 02131520: daFallRock_c::execute() inlined */
static BOOL daFallRock_Execute(daFallRock_c* i_this) {
    WWHD_FUNC(0x02131520, BOOL, i_this);
    BOOL deleted = FALSE;
    i_this->field_0x614 = i_this->field_0x614 + 1;

    fopAcM_posMoveF(i_this, &i_this->mStts.m_cc_move);
    i_this->field_0x29C.CrrPos(dComIfG_Bgsp());

    i_this->field_0x610 = i_this->field_0x610 + std::fabs((f32)i_this->speed.y);
    if (i_this->field_0x610 > 7000.0f /* m_falllen */) {
        deleted = fopAcM_delete(i_this);
    }

    if (i_this->field_0x29C.ChkGroundLanding()) {
        i_this->setParticle(0, &i_this->current.pos);
        fopAcM_seStart(i_this, JA_SE_OBJ_BREAK_ROCK, 0);
        if (deleted == FALSE) {
            deleted = fopAcM_delete(i_this);
        }
    }

    if (i_this->field_0x29C.ChkWaterIn()) {
        u32 se = i_this->scale.x < 1.3f ? JA_SE_OBJ_FALL_WATER_M : JA_SE_OBJ_FALL_WATER_L;
        fopAcM_seStart(i_this, se, 0);
        i_this->setParticle(2, &i_this->current.pos);
        if (!deleted) {
            deleted = fopAcM_delete(i_this);
        }
    }

    /* dBgS_ObjGndChk_Yogan chk (inline constructor: obj pass flag, group 4 = lava) */
    gabi::Local<dBgS_GndChk> chk;
    gabi::Local<cXyz> check_pos;
    dBgS_GndChk_ct(chk.get(), YOGAN_VT, true);
    gabi::store<u32>(gabi::ea(chk.get()) + 0x50, 4);
    check_pos->set(i_this->old.pos.x, gabi::fmadds(120.0f, i_this->scale.x, i_this->old.pos.y), i_this->old.pos.z);
    dBgS_GndChk_SetPos(chk.get(), check_pos.get());

    f32 height = cBgS_GroundCross(dComIfG_Bgsp(), chk.get());
    if (height != -1000000000.0f /* -G_CM3D_F_INF */) {
        f32 sx = i_this->scale.x;
        if (height > gabi::fmadds(120.0f, sx, i_this->current.pos.y)) {
            u32 se = sx < 1.3f ? JA_SE_OBJ_FALL_MAGMA_M : JA_SE_OBJ_FALL_MAGMA_L;
            fopAcM_seStart(i_this, se, 0);
            check_pos->y = height;
            i_this->setParticle(1, check_pos.get());
            if (!deleted) {
                fopAcM_delete(i_this);
            }
        }
    }
    cLib_chaseAngleS(&i_this->shape_angle.x, (s16)(i_this->shape_angle.x + 0x3E8), 0x3E8 /* m_rot_speed */);

    i_this->set_mtx();
    i_this->mCyl.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mCyl);

    /* ~dBgS_ObjGndChk_Yogan */
    u32 c = gabi::ea(chk.get());
    gabi::store<u32>(c + 0x20, 0x1000E704);
    gabi::store<u32>(c + 0x40, 0x1000E724);
    gabi::store<u32>(c + 0x4C, 0x1000E6E4);
    cBgS_Chk_dt(chk.get(), 0);
    return TRUE;
}
VERIFY(0x02131520, daFallRock_Execute);

/* 0213184C */
static BOOL daFallRock_IsDelete(daFallRock_c*) {
    WWHD_FUNC(0x0213184C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0213184C, daFallRock_IsDelete);

/* 02131854: HD: the destructor (resource release) is the virtual deleting destructor, run by the
 * framework; Delete itself does nothing */
static BOOL daFallRock_Delete(daFallRock_c*) {
    WWHD_FUNC(0x02131854, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02131854, daFallRock_Delete);

/* 0213185C: daFallRock_c::create() inlined */
cPhs_State daFallRock_c::create() {
    WWHD_FUNC(0x0213185C, cPhs_State, this);
    /* fopAcM_ct(this, daFallRock_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = FALLROCK_VTBL;
            dBgS_ObjAcch_ct(&field_0x29C, FALLROCK_OBJACCH_VT);
            dBgS_AcchCir_ct(&field_0x460);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&mPhs, m_arcname);
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x021311AC /* CheckCreateHeap */, 0xB80)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */

        mStts.Init(0xFF, 0xFF, this);
        mCyl.Set(m_cyl_src);
        mCyl.SetStts(&mStts);
        mCyl.SetH(scale.x * 120.0f);
        mCyl.SetR(scale.x * 90.0f);
        field_0x460.SetWall(30.0f, 30.0f);
        field_0x29C.Set(&current.pos, &old.pos, this, 1, &field_0x460, &speed);
        set_mtx();

        field_0x29C.m_flags &= ~(u32)dBgS_Acch::WATER_NONE; /* ClrWaterNone */
        gravity = -5.0f;
        field_0x610 = 0.0f;
        maxFallSpeed = -70.0f;
        speedF = 0.0f;
    }
    return res;
}
VERIFY(0x0213185C, &daFallRock_c::create);

/* 02131AB0 */
static void __sinit_d_a_fallrock_cpp() {
    WWHD_FUNC(0x02131AB0, void, (u32)0);
    sinit_header_statics(0x10463CC0, 0x101B4A3C);
}
VERIFY(0x02131AB0, __sinit_d_a_fallrock_cpp);

/* 02131B44: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02131B44, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02131B44, SafeString_dt);

/* 02131110: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02131110, void, (u32)0);
}
VERIFY(0x02131110, SafeString_assureTerminationImpl);

/* 02131B58: daFallRock_c deleting destructor (HD virtual; GameCube ~daFallRock_c: resDeleteDemo) */
static void daFallRock_c_dt(daFallRock_c* i_this, s32 flags) {
    WWHD_FUNC(0x02131B58, void, i_this, flags);
    if (i_this != nullptr) {
        i_this->__vtbl = FALLROCK_VTBL;
        dComIfG_resDelete(&i_this->mPhs, m_arcname);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->field_0x460) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        u32 acch = gabi::ea(&i_this->field_0x29C);                       /* ~dBgS_ObjAcch (inline) */
        gabi::store<u32>(acch + 0x20, FALLROCK_OBJACCH_VT.v20);
        gabi::store<u32>(acch + 0x14, FALLROCK_OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->field_0x29C, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);               /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02131B58, daFallRock_c_dt);

/* 02131C10: empty (this TU's copy of an empty virtual) */
static void fallrock_empty_02131C10(void*) {
    WWHD_FUNC(0x02131C10, void, (u32)0);
}
VERIFY(0x02131C10, fallrock_empty_02131C10);
