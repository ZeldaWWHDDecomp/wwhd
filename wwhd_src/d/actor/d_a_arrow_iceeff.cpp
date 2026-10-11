/**
 * d_a_arrow_iceeff.cpp (WWHD)
 * Ice arrow effect (ice shards on a hit actor, or an ice block on water).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_arrow_iceeff.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_arrow_iceeff.h"

const gc_tu_data gc_tu = {.safeString = 0x100074AC};

#define ACT_VTBL 0x100074EC /* HD: daArrow_Iceeff_c vtable */
#define BCK_VTBL 0x100074C4 /* this TU's mDoExt_bckAnm vtable */
/* dPa_control_c::mSingleRippleEcallBack */
#define SINGLE_RIPPLE_CB 0x1047B2E4

enum {
    JA_SE_OBJ_MINI_ICE = 0x69DD,
    JA_SE_OBJ_MINI_ICE_BREAK = 0x69DE,
    ID_IT_JN_ARWICE_OUT00 = 0x0055,
    ID_IT_JN_ARWI_HITA00 = 0x029E,
    ID_IT_JN_WP_HAMON01 = 0x003D,
    ID_IT_JN_WP_HAMON03 = 0x003F,
};

// HD: out-of-line calls and the original particle groups are bound by this unit's glue.
static inline JPABaseEmitter*
dComIfGp_particle_setSingleRipple(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 5, id, pos, angle, scale, 0xff,
                           gabi::at<dPa_levelEcallBack>(SINGLE_RIPPLE_CB), -1, nullptr, nullptr,
                           nullptr);
}
/* HD: fopAcM_seStartCurrent without null checks here */
static inline void seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}
// HD: the bck constructor initializes the embedded frame controller and matrix calculator.
static inline void mDoExt_bckAnm_ct(mDoExt_bckAnm* b, u32 tu_vtbl) {
    auto* view = reinterpret_cast<ArrowIceBckHD*>(b);
    ArrowIce_bckBase_ct(b, 0);
    view->mFrameCtrlVtable = 0x1016E54C;
    ArrowIce_mtxCalc_ct(&view->mCalculator);
    view->mOldFrame = 0;
    view->mOldFrameMorf = 0;
    view->mAnm = 0;
    view->mMorf = 0;
    view->mFrameCtrlVtable = tu_vtbl;
    view->mCalculatorVtable = 0x1016D820;
    view->mAnmTransform = 0;
}
// HD: private texture-animation view uses the verified HD layout.
static inline void btk_setFrame(ArrowIceBtkHD* btk, f32 f) { btk->mFrame = f; }
static inline f32 btk_getFrameMax(ArrowIceBtkHD* btk) { return (f32)btk->mFrameMax; }

/* 02055D34 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daArrow_Iceeff_c*>(i_this)->CreateHeap();
}

/* 02055B84 */
BOOL daArrow_Iceeff_c::CreateHeap() {
    // HD: models enable the ice effect; assert texts spell NULL instead of HD (0).
    J3DModelData* modelData;
    if (field_0xA38 == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes("Link", dRes_INDEX_LINK_BDL_GICER00_e);
        if (modelData == nullptr) /* JUT_ASSERT(87, modelData != NULL) */
            JUT_ASSERT_fail("d_a_arrow_iceeff.cpp", 0x57, "modelData != NULL");
        for (int i = 0; i < 30; i++) {
            J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
            field_0x298[i] = m;
            if (m == nullptr) {
                return false;
            }
            reinterpret_cast<ArrowIceModelHD*>(m)->mIceEffectHD = 1;
        }
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes("Link", dRes_INDEX_LINK_BDL_GICER01_e);
        if (modelData == nullptr) /* JUT_ASSERT(98, modelData != NULL) */
            JUT_ASSERT_fail("d_a_arrow_iceeff.cpp", 0x65, "modelData != NULL");
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        mpModel = m;
        if (m == nullptr) {
            return false;
        }
        reinterpret_cast<ArrowIceModelHD*>(m)->mIceEffectHD = 1;

        J3DAnmTransform* bck =
            (J3DAnmTransform*)dComIfG_getObjectRes("Link", dRes_INDEX_LINK_BCK_GICER01_e);
        if (bck == nullptr) /* JUT_ASSERT(107, bck != NULL) */
            JUT_ASSERT_fail("d_a_arrow_iceeff.cpp", 0x72, "bck != NULL");
        if (!mBck.init(modelData, bck, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false)) {
            return false;
        }
    }

    return true;
}

/* 02055F04 */
void daArrow_Iceeff_c::CreateInit() {
    if (field_0xA38 == 0) {
        for (int i = 0; i < 30; i++) {
            J3DModel* m = field_0x298[i];
            cullMtx = m != nullptr ? gabi::ea(&m->mBaseTransformMtx)
                                   : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */

            f32 s = cM_rndF(4.5f) + 6.2999997f;
            field_0x8B4[i].set(s, s, s);

            f32 temp = 180.0f * (f32)i / 30.0f;
            u16 angle = (u16)gabi::ftoi(cM_rndF(65536.0f));
            mDoMtx_ZrotS(mDoMtx_stack_c::get(), (s16)gabi::ftoi(cM_rndF(65536.0f)));
            mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)gabi::ftoi(cM_rndF(65536.0f)));
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)gabi::ftoi(cM_rndF(65536.0f)));
            mDoMtx_stack_c::transM(cM_ssin(angle) * temp, cM_scos(angle) * temp, 0.0f);
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), current.angle.z);
            mDoMtx_XrotM(mDoMtx_stack_c::get(), current.angle.x);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
            PSMTXCopy(mDoMtx_stack_c::get(), &field_0x314[i]); /* cMtx_copy */
        }
    } else {
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
    }

    fopAcM_setCullSizeBox(this, -100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f);
    cullSizeFar = 1.0f; /* fopAcM_setCullSizeFar */
    set_mtx();
    field_0xA30 = 0;
    field_0xA34 = 0.0f;
    s32 kind = field_0xA38;
    auto* link = reinterpret_cast<ArrowIceLinkHD*>(
        dComIfGp_getLinkPlayer()); /* daPy_getPlayerLinkActorClass() */
    f32 frame = field_0xA34;
    if (kind == 0) {
        field_0xA1C = &link->mIceParticleBtk; /* getIceParticleBtk() */
    } else {
        field_0xA1C = &link->mIceWaterParticleBtk; /* getIceWaterParticleBtk() */
    }
    btk_setFrame(field_0xA1C, frame);
    field_0xA3C = 1;
}

/* 02055D38 */
void daArrow_Iceeff_c::set_mtx() {
    if (field_0xA38 == 0) {
        for (int i = 0; i < 30; i++) {
            J3DModel_setBaseScale(field_0x298[i], &field_0x8B4[i]);
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, 0);
            PSMTXConcat(mDoMtx_stack_c::get(), &field_0x314[i], mDoMtx_stack_c::get()); /* concat */
            J3DModel_setBaseTRMtx(field_0x298[i], mDoMtx_stack_c::get());
        }
    } else {
        J3DModel* m = mpModel;
        scale.set(1.0f, 1.0f, 1.0f);
        J3DModel_setBaseScale(m, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    }
}

/* inlined into daArrow_Iceeff_Create */
cPhs_State daArrow_Iceeff_c::_create() {
    /* fopAcM_ct(this, daArrow_Iceeff_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            mDoExt_bckAnm_ct(&mBck, BCK_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (arrow == nullptr) {
        return cPhs_ERROR_e;
    }
    field_0xA38 = fopAcM_GetParam(arrow) == 4 ? 1 : 0;

    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((void*)CheckCreateHeap), 0xD5E0)) {
        return cPhs_ERROR_e;
    }

    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 02056248 */
static cPhs_State daArrow_Iceeff_Create(void* i_this) {
    return static_cast<daArrow_Iceeff_c*>(i_this)->_create();
}

/* inlined into daArrow_Iceeff_Delete */
bool daArrow_Iceeff_c::_delete() { return true; }

/* 02056378 */
static BOOL daArrow_Iceeff_Delete(void* i_this) {
    return static_cast<daArrow_Iceeff_c*>(i_this)->_delete();
}

/* inlined into daArrow_Iceeff_Draw */
bool daArrow_Iceeff_c::_draw() {
    // HD: any nonzero draw flag is visible.
    s32 temp = field_0xA30;
    if (30 < temp) {
        temp = 30;
    }

    btk_setFrame(field_0xA1C, field_0xA34);
    dComIfGd_setListP1();

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    if (field_0xA38 == 0) {
        for (int i = 0; i < temp; i++) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, field_0x298[i], &tevStr);

            if (field_0xA3C != 0) {
                mDoExt_modelUpdateDL(field_0x298[i]);
            }
        }
    } else {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpModel, &tevStr);
        J3DModelData* mdl_data = J3DModel_getModelData(mpModel);
        mBck.entry(mdl_data, mBck.getFrame());
        if (field_0xA3C != 0) {
            mDoExt_modelUpdateDL(mpModel);
        }

        // mBck.remove(mpModel->getModelData()).
        u32 md = gabi::ea(J3DModel_getModelData(mpModel));
        auto* modelData = gabi::at<J3DModelData>(md);
        gabi::at<ArrowIceJointTreeHD>(modelData->mJointNodes)->mpMtxCalc = 0;
    }

    dComIfGd_setList();

    return true;
}

/* 02056380 */
static BOOL daArrow_Iceeff_Draw(void* i_this) {
    return static_cast<daArrow_Iceeff_c*>(i_this)->_draw();
}

/* frame += 1, clamped to the btk's last frame */
static inline void advance_frame(daArrow_Iceeff_c* i_this) {
    f32 f = i_this->field_0xA34 + 1.0f;
    i_this->field_0xA34 = f;
    ArrowIceBtkHD* btk = i_this->field_0xA1C;
    if (!(f < btk_getFrameMax(btk))) {
        i_this->field_0xA34 = btk_getFrameMax(btk);
    }
}

/* 020564D0 */
bool daArrow_Iceeff_c::_execute() {
    /* static cXyz ripple_scale(1.0f, 1.0f, 1.0f) */
    auto* ripple = gabi::at<ArrowIceRippleHD>(0x101FDA10);
    cXyz* ripple_scale = &ripple->mScale;
    if ((u32)ripple->mGuard == 0) {
        ripple_scale->y = 1.0f;
        ripple_scale->z = 1.0f;
        ripple->mGuard = 1;
        ripple_scale->x = 1.0f;
    }

    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (field_0xA38 == 0) {
        if (arrow == nullptr) {
            dComIfGp_particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos, &current.angle);
            fopAcM_delete(this);

            return true;
        }

        current.pos.copy(
            static_cast<ArrowIceParentHD*>(arrow)->field_0x6a8); /* arrow->field_0x6a8 */
        current.angle.x = arrow->shape_angle.x;
        current.angle.y = arrow->shape_angle.y;
        current.angle.z = arrow->shape_angle.z;
        if (field_0xA30 == 0) {
            seStartCurrent(this, JA_SE_OBJ_MINI_ICE, 0);
        }

        if (field_0xA30 >= 30) {
            advance_frame(this);
        }

        if (field_0xA30 < 0x3C) {
            field_0xA30 = field_0xA30 + 1;
        } else {
            dComIfGp_particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos, &current.angle);
            seStartCurrent(this, JA_SE_OBJ_MINI_ICE_BREAK, 0);
            fopAcM_delete(arrow);
            fopAcM_delete(this);
        }
    } else {
        mDoExt_baseAnm_play(&mBck);
        gabi::Local<be<f32>> waterY;
        fopAcM_getWaterY(&current.pos, waterY.get());
        current.pos.y = *waterY;
        if (field_0xA30 == 0) {
            gabi::Local<csXyz> angle;
            angle->z = 0;
            angle->y = 0;
            angle->x = -0x4000;

            JPABaseEmitter* ptcl =
                dComIfGp_particle_setP1(ID_IT_JN_ARWI_HITA00, &current.pos, angle.get());
            if (ptcl) {
                auto* emitter = reinterpret_cast<ArrowIceEmitterHD*>(ptcl);
                /* setGlobalScale(0.5, 0.5, 0.5) (HD: also the particle scale), setRate(50),
                 * setGlobalAlpha(0x80) */
                emitter->mGlobalDynamicsScale.x = 0.5f;
                emitter->mGlobalDynamicsScale.y = 0.5f;
                emitter->mGlobalPrmColor[3] = 0x80;
                emitter->mRate = 50.0f;
                emitter->mGlobalDynamicsScale.z = 0.5f;
                emitter->mGlobalParticleScale.x = 0.5f;
                emitter->mGlobalParticleScale.y = 0.5f;
                emitter->mGlobalParticleScale.z = 0.5f;
            }

            seStartCurrent(this, JA_SE_OBJ_MINI_ICE, 0);
        }

        advance_frame(this);

        if (field_0xA30 < 300) {
            field_0xA30 = field_0xA30 + 1;
        } else {
            fopAcM_delete(this);
        }

        if (field_0xA30 == 0x23) {
            JPABaseEmitter* ptcl = dComIfGp_particle_setSingleRipple(
                ID_IT_JN_WP_HAMON01, &current.pos, nullptr, ripple_scale);
            if (ptcl) {
                auto* emitter = reinterpret_cast<ArrowIceEmitterHD*>(ptcl);
                /* setGlobalParticleScale(0.67, 0.67) */
                emitter->mGlobalParticleScale.x = 0.67f;
                emitter->mGlobalParticleScale.y = 0.67f;
                emitter->mGlobalParticleScale.z = 1.0f;
            }
        } else if (field_0xA30 == 0x28) {
            JPABaseEmitter* ptcl = dComIfGp_particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos);
            if (ptcl) {
                auto* emitter = reinterpret_cast<ArrowIceEmitterHD*>(ptcl);
                emitter->mEmitterScale.x = 0.5f; /* setEmitterScale(0.5, 1.0, 0.5) */
                emitter->mEmitterScale.y = 1.0f;
                emitter->mEmitterScale.z = 0.5f;
                emitter->mGlobalParticleScale.x = 0.33f; /* setGlobalParticleScale(0.33, 0.33) */
                emitter->mGlobalParticleScale.y = 0.33f;
                emitter->mGlobalParticleScale.z = 1.0f;
                emitter->mInitialVelOmni = 25.0f; /* setAwayFromCenterSpeed */
                emitter->mInitialVelAxis = 5.0f;  /* setAwayFromAxisSpeed */
                emitter->mInitialVelDir = 5.0f;   /* setDirectionalSpeed */
            }

            dComIfGp_particle_setSingleRipple(ID_IT_JN_WP_HAMON03, &current.pos, nullptr,
                                              ripple_scale);
            seStartCurrent(this, JA_SE_OBJ_MINI_ICE_BREAK, 0);

            field_0xA3C = 0;
        }
    }

    set_mtx();

    return true;
}

// HD: compiler-generated and template code of this translation unit.

/* 02056A40 */
static void __sinit_d_a_arrow_iceeff_cpp() { sinit_header_statics(0x10461498, 0x1018FEC8); }

/* 02056AD4 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02056AE8 */
static BOOL daArrow_Iceeff_IsDelete(void*) { return true; }

/* 02056AF0 */
static void daArrow_Iceeff_c_dt(daArrow_Iceeff_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        ArrowIce_frameCtrl_dt(&reinterpret_cast<ArrowIceBckHD*>(&i_this->mBck)->mFrameCtrlVtable,
                              0);
        ArrowIce_actor_dt(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02056B50 */
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue
str 10007500 "Link"
str 10007508 "d_a_arrow_iceeff.cpp"
str 10007520 "modelData != NULL" renamed
str 10007534 "bck != NULL" renamed
ext 025F181C void mDoMtx_ZrotS(Mtx34*,s16)
ext 028E9108 void PSMTXConcat(const Mtx34*,const Mtx34*,Mtx34*)
ext 025D9F70 BOOL fopAcM_getWaterY(const cXyz*,be<f32>*)
ext 027F2BC0 void ArrowIce_bckBase_ct(mDoExt_bckAnm*,s32)
ext 027DA984 void ArrowIce_mtxCalc_ct(void*)
ext 027F3628 void ArrowIce_frameCtrl_dt(void*,s32)
ext 025D50BC void ArrowIce_actor_dt(fopAc_ac_c*,s32)
*/
