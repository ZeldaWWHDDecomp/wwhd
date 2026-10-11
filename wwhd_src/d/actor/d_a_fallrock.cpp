/**
 * d_a_fallrock.cpp (WWHD)
 * Falling rock (breaks on landing, splashes in water or lava).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fallrock.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fallrock.h"
// HD: GHS per-unit constructor and destructor vtables.
const gc_tu_data gc_tu = {
    .safeString = 0x1000E6BC,
    .objAcch = {0x1000E7B4, 0x1000E7D4, 0x1000E7C4},
    .objAcchDt = {0x1000E7C4, 0x1000E7D4},
    .gndChk = {0x1000E774, 0x1000E784, 0x1000E7A4, 0x1000E794},
    .gndChkDt = {0x1000E704, 0x1000E724, 0x1000E6E4},
    .headerStatics = {0x10463CC0, 0x101B4A3C, 0x10463CCC},
};
enum { FALLROCK_VTBL = 0x1000E7E4, AAB_VTBL = 0x1000E6D4 };
static dCcD_SrcCyl m_cyl_src;
enum {
    dRes_INDEX_ALWAYS_BDL_KROCK_00_e = 0x20,
    dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e = 0x30,
    dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e = 0x66
};
enum {
    JA_SE_OBJ_BREAK_ROCK = 0x692C,
    JA_SE_OBJ_FALL_WATER_M = 0x6919,
    JA_SE_OBJ_FALL_WATER_L = 0x691A,
    JA_SE_OBJ_FALL_MAGMA_M = 0x691C,
    JA_SE_OBJ_FALL_MAGMA_L = 0x691D
};
enum { dPa_name_ID_IT_JN_STS_HAHEN = 0x3E3, dPa_name_ID_IT_JN_M_STS_HAHEN = 0x3E2 };

/* 021311AC */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return ((daFallRock_c *)i_this)->CreateHeap();
}

/* 02131114 */
BOOL daFallRock_c::CreateHeap() {
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_KROCK_00_e);
    if (modelData == nullptr) /* JUT_ASSERT(161, modelData != NULL) */
        JUT_ASSERT_fail("d_a_fallrock.cpp", 0xa1, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    return TRUE;
}

/* 02131418 */
void daFallRock_c::set_mtx() {
    J3DModel *model = mpModel;
    model->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, -(scale.x * 120.0f), 0.0f);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

// HD: mDoExt_modelUpdate only, no simple shadow
/* 021311B0 */
static BOOL daFallRock_Draw(daFallRock_c *i_this) {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    g_env_light.setLightTevColorType(i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdate(i_this->mpModel);
    return TRUE;
}

/* 02131208 */
void daFallRock_c::setParticle(int param_1, cXyz *pos) {
    gabi::Local<cXyz> particle_scale;
    gabi::Local<cXyz> pos_copy;

    field_0x60C.r = (u8)tevStr.mColorC0.r;
    field_0x60C.g = (u8)tevStr.mColorC0.g;
    field_0x60C.b = (u8)tevStr.mColorC0.b;
    field_0x60C.a = (u8)tevStr.mColorC0.a;

    f32 s = 5.0f * scale.x;
    particle_scale->set(s, s, s);
    pos_copy->set(pos->x, pos->y, pos->z);

    switch ((u32)param_1) {
    case 0: {
        dComIfGp_particle_set(dPa_name_ID_IT_JN_STS_HAHEN, pos_copy.get(), nullptr, particle_scale.get(),
                              0xFF, nullptr, -1, &tevStr.mColorK0, &tevStr.mColorK0);
        J3DModelData *modelData =
            (J3DModelData *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e);
        void *anmTexPattern = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e);
        JPABaseEmitter *emitter =
            dComIfGp_particle_set(dPa_name_ID_IT_JN_M_STS_HAHEN, pos_copy.get(), nullptr,
                                  particle_scale.get(), 0xFF, nullptr, -1, &field_0x60C, &tevStr.mColorK0);
        if (emitter != nullptr && modelData != nullptr && anmTexPattern != nullptr) {
            void *modelEmitter =
                FallRock_newModelEmitter(nullptr, emitter, modelData, 1, &tevStr, anmTexPattern, 0, 0);
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

// HD: daFallRock_c::execute() inlined
/* 02131520 */
static BOOL daFallRock_Execute(daFallRock_c *i_this) {
    BOOL deleted = FALSE;
    i_this->field_0x614 = i_this->field_0x614 + 1;

    fopAcM_posMoveF(i_this, &i_this->mStts.m_cc_move);
    i_this->field_0x29C.CrrPos(dComIfG_Bgsp());

    i_this->field_0x610 = i_this->field_0x610 + std::fabs((f32)i_this->speed.y);
    if (i_this->field_0x610 > daFallRock_c::m_falllen) {
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
    dBgS_ObjGndChk_Yogan chk;
    gabi::Local<cXyz> check_pos;
    check_pos->set(i_this->old.pos.x, gabi::fmadds(120.0f, i_this->scale.x, i_this->old.pos.y),
                   i_this->old.pos.z);
    chk.SetPos(check_pos.get());

    f32 height = dComIfG_Bgsp()->GroundCross(&chk);
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
    cLib_chaseAngleS(&i_this->shape_angle.x, (s16)(i_this->shape_angle.x + daFallRock_c::m_rot_speed),
                     daFallRock_c::m_rot_speed);

    i_this->set_mtx();
    i_this->mCyl.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mCyl);

    return TRUE;
}

/* 0213184C */
static BOOL daFallRock_IsDelete(daFallRock_c *) {
    return TRUE;
}

// HD: resource release runs in the framework virtual deleting destructor.
/* 02131854 */
static BOOL daFallRock_Delete(daFallRock_c *) {
    return TRUE;
}

// HD: daFallRock_c::create() inlined
/* 0213185C */
cPhs_State daFallRock_c::create() {
    /* fopAcM_ct(this, daFallRock_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = FALLROCK_VTBL;
            __ct(&field_0x29C);
            __ct(&field_0x460);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&mPhs, "Always");
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xB80)) {
            return cPhs_ERROR_e;
        }
        fopAcM_SetMtx(this, mpModel->getBaseTRMtx());

        mStts.Init(0xFF, 0xFF, this);
        mCyl.Set(&m_cyl_src);
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

/* 02131AB0 */
static void __sinit_d_a_fallrock_cpp() {
    __sinit_header_statics();
}

// HD: sead::SafeString deleting destructor (this TU's vtable slot)
/* 02131B44 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 02131110 */
static void SafeString_assureTerminationImpl(void *) {}

// HD: daFallRock_c deleting destructor (HD virtual; GameCube ~daFallRock_c: resDeleteDemo)
/* 02131B58 */
static void daFallRock_c_dt(daFallRock_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        i_this->__vtbl = FALLROCK_VTBL;
        dComIfG_resDelete(&i_this->mPhs, "Always");
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(&i_this->field_0x460);
        __dt(&i_this->field_0x29C);
        __dt(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: empty (this TU's copy of an empty virtual)
/* 02131C10 */
static void empty_virtual(void *) {}

/* WWHD glue
data m_cyl_src 1000E858
str 1000E850 "Always"
str 1000E7F4 "d_a_fallrock.cpp"
str 1000E808 "modelData != (0)"
ext 025E2DA8 void mDoExt_modelUpdate(J3DModel*)
ext 025DAF3C void fopKyM_createMpillar(const cXyz*, f32)
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 025A3BDC void* FallRock_newModelEmitter(void*, JPABaseEmitter*, J3DModelData*, u32, dKy_tevstr_c*, void*, u16, s32)
*/
