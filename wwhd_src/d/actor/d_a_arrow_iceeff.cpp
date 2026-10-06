/**
 * d_a_arrow_iceeff.cpp (WWHD)
 * Ice arrow effect (ice shards on a hit actor, or an ice block on water).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_arrow_iceeff.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10007500) /* "Link" */
#define SAFESTRING_VTBL 0x100074AC
#define FILE_NAME STR(0x10007508)  /* "d_a_arrow_iceeff.cpp" */
#define ACT_VTBL 0x100074EC        /* HD: daArrow_Iceeff_c vtable */
#define BCK_VTBL 0x100074C4        /* this TU's mDoExt_bckAnm vtable */
/* static cXyz ripple_scale(1, 1, 1) (function-local, guard 0x101FDA10) */
#define RIPPLE_SCALE_GUARD 0x101FDA10
#define RIPPLE_SCALE 0x101FDA14
/* dPa_control_c::mSingleRippleEcallBack */
#define SINGLE_RIPPLE_CB 0x1047B2E4

enum {
    dRes_INDEX_LINK_BCK_GICER01_e = 0x0D,
    dRes_INDEX_LINK_BDL_GICER00_e = 0x40,
    dRes_INDEX_LINK_BDL_GICER01_e = 0x41,
};
enum {
    JA_SE_OBJ_MINI_ICE = 0x69DD,
    JA_SE_OBJ_MINI_ICE_BREAK = 0x69DE,
    ID_IT_JN_ARWICE_OUT00 = 0x0055,
    ID_IT_JN_ARWI_HITA00 = 0x029E,
    ID_IT_JN_WP_HAMON01 = 0x003D,
    ID_IT_JN_WP_HAMON03 = 0x003F,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void mDoMtx_ZrotS(Mtx34* m, s16 z) { gabi::call(0x025F181C, m, z); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline BOOL fopAcM_getWaterY(const cXyz* pos, be<f32>* y) { return gabi::call<BOOL>(0x025D9F70, pos, y); }
/* dComIfGp_particle_setP1 (group 1) / setSingleRipple (group 5, mSingleRippleEcallBack) */
static inline JPABaseEmitter* particle_setP1(u16 id, const cXyz* pos, const csXyz* angle = nullptr, const cXyz* scale = nullptr) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 1, id, pos, angle, scale, 0xff, nullptr, -1, nullptr, nullptr, nullptr);
}
static inline JPABaseEmitter* particle_setSingleRipple(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 5, id, pos, angle, scale, 0xff, gabi::at<dPa_levelEcallBack>(SINGLE_RIPPLE_CB), -1, nullptr,
                           nullptr, nullptr);
}
/* HD: fopAcM_seStartCurrent without null checks here */
static inline void seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}
/* mDoExt_bckAnm inline constructor (HD 0x8C, as d_a_sbox): J3DFrameCtrl::init, vtables 1016E54C (then
 * the TU's) at +0x10, 027DA984 on +0x14, 1016D820 at +0x48 */
static inline void mDoExt_bckAnm_ct(mDoExt_bckAnm* b, u32 tu_vtbl) {
    u32 p = gabi::ea(b);
    gabi::call(0x027F2BC0, b, 0);
    gabi::store<u32>(p + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, gabi::at<void>(p + 0x14));
    gabi::store<u32>(p + 0x84, 0);
    gabi::store<u32>(p + 0x88, 0);
    gabi::store<u32>(p + 0x80, 0);
    gabi::store<u32>(p + 0x7C, 0);
    gabi::store<u32>(p + 0x10, tu_vtbl);
    gabi::store<u32>(p + 0x48, 0x1016D820);
    gabi::store<u32>(p + 0x58, 0);
}
/* J3DAnmTextureSRTKey: frame +4, frame max (s16) +0xA */
static inline void btk_setFrame(u32 btk, f32 f) { gabi::store<f32>(btk + 4, f); }
static inline f32 btk_getFrameMax(u32 btk) { return (f32)gabi::load<s16>(btk + 0xA); }

struct daArrow_Iceeff_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    bool _execute();

    /* 0x3AC */ be<u32> field_0x290;
    /* 0x3B0 */ be<u32> field_0x294;
    /* 0x3B4 */ gptr<J3DModel> field_0x298[30];
    /* 0x42C */ gptr<J3DModel> mpModel;
    /* 0x430 */ Mtx34 field_0x314[30];
    /* 0x9D0 */ cXyz field_0x8B4[30];
    /* 0xB38 */ be<u32> field_0xA1C; /* J3DAnmTextureSRTKey* */
    /* 0xB3C */ mDoExt_bckAnm mBck;  /* HD 0x8C */
    /* 0xBC8 */ be<s32> field_0xA30;
    /* 0xBCC */ be<f32> field_0xA34;
    /* 0xBD0 */ be<s32> field_0xA38;
    /* 0xBD4 */ be<u8> field_0xA3C;
    /* 0xBD5 */ u8 _BD5[3];
};
WWHD_OFFSET(daArrow_Iceeff_c, field_0x314, 0x430);
WWHD_OFFSET(daArrow_Iceeff_c, field_0x8B4, 0x9D0);
WWHD_OFFSET(daArrow_Iceeff_c, mBck, 0xB3C);
WWHD_OFFSET(daArrow_Iceeff_c, field_0xA3C, 0xBD4);
WWHD_SIZE(daArrow_Iceeff_c, 0xBD8);

/* 02055B84. HD: each new model gets byte +0x78 = 1 */
BOOL daArrow_Iceeff_c::CreateHeap() {
    WWHD_FUNC(0x02055B84, BOOL, this);
    J3DModelData* modelData;
    if (field_0xA38 == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_LINK_BDL_GICER00_e, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(87, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x57, STR(0x10007520));
        for (int i = 0; i < 30; i++) {
            J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
            field_0x298[i] = m;
            if (m == nullptr) {
                return false;
            }
            gabi::store<u8>(gabi::ea(m) + 0x78, 1);
        }
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_LINK_BDL_GICER01_e, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(98, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x65, STR(0x10007520));
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        mpModel = m;
        if (m == nullptr) {
            return false;
        }
        gabi::store<u8>(gabi::ea(m) + 0x78, 1);

        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_LINK_BCK_GICER01_e, SAFESTRING_VTBL);
        if (bck == nullptr) /* JUT_ASSERT(107, bck != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x72, STR(0x10007534));
        if (!mBck.init(modelData, bck, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false)) {
            return false;
        }
    }

    return true;
}
VERIFY(0x02055B84, &daArrow_Iceeff_c::CreateHeap);

/* 02055D34 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02055D34, BOOL, i_this);
    return static_cast<daArrow_Iceeff_c*>(i_this)->CreateHeap();
}
VERIFY(0x02055D34, CheckCreateHeap);

/* 02055D38 */
void daArrow_Iceeff_c::set_mtx() {
    WWHD_FUNC(0x02055D38, void, this);
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
VERIFY(0x02055D38, &daArrow_Iceeff_c::set_mtx);

/* 02055F04 */
void daArrow_Iceeff_c::CreateInit() {
    WWHD_FUNC(0x02055F04, void, this);
    if (field_0xA38 == 0) {
        for (int i = 0; i < 30; i++) {
            J3DModel* m = field_0x298[i];
            cullMtx = m != nullptr ? gabi::ea(m) + 0xC8 : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */

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
    u32 link = gabi::ea(dComIfGp_getLinkPlayer()); /* daPy_getPlayerLinkActorClass() */
    f32 frame = field_0xA34;
    if (kind == 0) {
        field_0xA1C = link + 0x4638; /* getIceParticleBtk() */
    } else {
        field_0xA1C = link + 0x4794; /* getIceWaterParticleBtk() */
    }
    btk_setFrame(field_0xA1C, frame);
    field_0xA3C = 1;
}
VERIFY(0x02055F04, &daArrow_Iceeff_c::CreateInit);

/* 02056248: daArrow_Iceeff_Create (_create inlined) */
static cPhs_State daArrow_Iceeff_Create(void* p) {
    WWHD_FUNC(0x02056248, cPhs_State, p);
    daArrow_Iceeff_c* i_this = static_cast<daArrow_Iceeff_c*>(p);
    /* fopAcM_ct(this, daArrow_Iceeff_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = ACT_VTBL;
            mDoExt_bckAnm_ct(&i_this->mBck, BCK_VTBL);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    fopAc_ac_c* arrow = fopAcM_SearchByID(i_this->parentActorID);
    if (arrow == nullptr) {
        return cPhs_ERROR_e;
    }
    i_this->field_0xA38 = fopAcM_GetParam(arrow) == 4 ? 1 : 0;

    if (!fopAcM_entrySolidHeap(i_this, 0x02055D34 /* CheckCreateHeap */, 0xD5E0)) {
        return cPhs_ERROR_e;
    }

    i_this->CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02056248, daArrow_Iceeff_Create);

/* 02056378: daArrow_Iceeff_Delete (_delete inlined) */
static BOOL daArrow_Iceeff_Delete(void* i_this) {
    WWHD_FUNC(0x02056378, BOOL, i_this);
    return true;
}
VERIFY(0x02056378, daArrow_Iceeff_Delete);

/* 02056380: daArrow_Iceeff_Draw (_draw inlined). HD: draws while field_0xA3C != 0 */
static BOOL daArrow_Iceeff_Draw(void* p) {
    WWHD_FUNC(0x02056380, BOOL, p);
    daArrow_Iceeff_c* i_this = static_cast<daArrow_Iceeff_c*>(p);
    s32 temp = i_this->field_0xA30;
    if (30 < temp) {
        temp = 30;
    }

    btk_setFrame(i_this->field_0xA1C, i_this->field_0xA34);
    /* dComIfGd_setListP1 (HD: play+0x5D58 / +0x5D60) */
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D58));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    if (i_this->field_0xA38 == 0) {
        for (int i = 0; i < temp; i++) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, i_this->field_0x298[i], &i_this->tevStr);

            if (i_this->field_0xA3C != 0) {
                mDoExt_modelUpdateDL(i_this->field_0x298[i]);
            }
        }
    } else {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);
        J3DModelData* mdl_data = J3DModel_getModelData(i_this->mpModel);
        i_this->mBck.entry(mdl_data, i_this->mBck.getFrame());
        if (i_this->field_0xA3C != 0) {
            mDoExt_modelUpdateDL(i_this->mpModel);
        }

        /* mBck.remove(mpModel->getModelData()): the joint tree's animation pointer (+0x14) = NULL */
        u32 md = gabi::ea(J3DModel_getModelData(i_this->mpModel));
        gabi::store<u32>(gabi::load<u32>(md + 8) + 0x14, 0);
    }

    dComIfGd_setList();

    return true;
}
VERIFY(0x02056380, daArrow_Iceeff_Draw);

/* frame += 1, clamped to the btk's last frame */
static inline void advance_frame(daArrow_Iceeff_c* i_this) {
    f32 f = i_this->field_0xA34 + 1.0f;
    i_this->field_0xA34 = f;
    u32 btk = i_this->field_0xA1C;
    if (!(f < btk_getFrameMax(btk))) {
        i_this->field_0xA34 = btk_getFrameMax(btk);
    }
}

/* 020564D0: _execute (the method table calls it directly) */
bool daArrow_Iceeff_c::_execute() {
    WWHD_FUNC(0x020564D0, bool, this);
    /* static cXyz ripple_scale(1.0f, 1.0f, 1.0f) */
    cXyz* ripple_scale = gabi::at<cXyz>(RIPPLE_SCALE);
    if (gabi::load<u32>(RIPPLE_SCALE_GUARD) == 0) {
        ripple_scale->y = 1.0f;
        ripple_scale->z = 1.0f;
        gabi::store<u32>(RIPPLE_SCALE_GUARD, 1);
        ripple_scale->x = 1.0f;
    }

    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (field_0xA38 == 0) {
        if (arrow == nullptr) {
            particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos, &current.angle);
            fopAcM_delete(this);

            return true;
        }

        current.pos.copy(*gabi::at<cXyz>(gabi::ea(arrow) + 0x7C0)); /* arrow->field_0x6a8 */
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
            particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos, &current.angle);
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

            JPABaseEmitter* ptcl = particle_setP1(ID_IT_JN_ARWI_HITA00, &current.pos, angle.get());
            if (ptcl) {
                u32 e = gabi::ea(ptcl);
                /* setGlobalScale(0.5, 0.5, 0.5) (HD: also the particle scale), setRate(50), setGlobalAlpha(0x80) */
                gabi::store<f32>(e + 0x220, 0.5f);
                gabi::store<f32>(e + 0x224, 0.5f);
                gabi::store<u8>(e + 0x247, 0x80);
                gabi::store<f32>(e + 0x34, 50.0f);
                gabi::store<f32>(e + 0x228, 0.5f);
                gabi::store<f32>(e + 0x238, 0.5f);
                gabi::store<f32>(e + 0x23C, 0.5f);
                gabi::store<f32>(e + 0x240, 0.5f);
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
            JPABaseEmitter* ptcl = particle_setSingleRipple(ID_IT_JN_WP_HAMON01, &current.pos, nullptr, ripple_scale);
            if (ptcl) {
                u32 e = gabi::ea(ptcl);
                /* setGlobalParticleScale(0.67, 0.67) */
                gabi::store<f32>(e + 0x238, 0.67f);
                gabi::store<f32>(e + 0x23C, 0.67f);
                gabi::store<f32>(e + 0x240, 1.0f);
            }
        } else if (field_0xA30 == 0x28) {
            JPABaseEmitter* ptcl = particle_setP1(ID_IT_JN_ARWICE_OUT00, &current.pos);
            if (ptcl) {
                u32 e = gabi::ea(ptcl);
                gabi::store<f32>(e + 0x8, 0.5f); /* setEmitterScale(0.5, 1.0, 0.5) */
                gabi::store<f32>(e + 0xC, 1.0f);
                gabi::store<f32>(e + 0x10, 0.5f);
                gabi::store<f32>(e + 0x238, 0.33f); /* setGlobalParticleScale(0.33, 0.33) */
                gabi::store<f32>(e + 0x23C, 0.33f);
                gabi::store<f32>(e + 0x240, 1.0f);
                gabi::store<f32>(e + 0x68, 25.0f); /* setAwayFromCenterSpeed */
                gabi::store<f32>(e + 0x6C, 5.0f);  /* setAwayFromAxisSpeed */
                gabi::store<f32>(e + 0x70, 5.0f);  /* setDirectionalSpeed */
            }

            particle_setSingleRipple(ID_IT_JN_WP_HAMON03, &current.pos, nullptr, ripple_scale);
            seStartCurrent(this, JA_SE_OBJ_MINI_ICE_BREAK, 0);

            field_0xA3C = 0;
        }
    }

    set_mtx();

    return true;
}
VERIFY(0x020564D0, &daArrow_Iceeff_c::_execute);

/* 02056A40 */
static void __sinit_d_a_arrow_iceeff_cpp() {
    WWHD_FUNC(0x02056A40, void, (u32)0);
    sinit_header_statics(0x10461498, 0x1018FEC8);
}
VERIFY(0x02056A40, __sinit_d_a_arrow_iceeff_cpp);

/* 02056AD4: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02056AD4, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02056AD4, trivial_dt);

/* 02056AE8: daArrow_Iceeff_IsDelete */
static BOOL daArrow_Iceeff_IsDelete(void*) {
    WWHD_FUNC(0x02056AE8, BOOL, (u32)0);
    return true;
}
VERIFY(0x02056AE8, daArrow_Iceeff_IsDelete);

/* 02056AF0: daArrow_Iceeff_c deleting destructor (mBck's member at +0x10) */
static void daArrow_Iceeff_c_dt(daArrow_Iceeff_c* i_this, s32 flags) {
    WWHD_FUNC(0x02056AF0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(i_this) + 0xB4C), 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02056AF0, daArrow_Iceeff_c_dt);

/* 02056B50: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02056B50, void, p);
}
VERIFY(0x02056B50, empty_virtual);
