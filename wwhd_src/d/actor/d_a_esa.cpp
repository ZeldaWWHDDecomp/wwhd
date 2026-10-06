/**
 * d_a_esa.cpp (WWHD)
 * Item - All-Purpose Bait
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_esa.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ESA_VTBL 0x1000E604        /* esa_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000E54C /* this TU's sead::SafeString vtable */
enum { dRes_INDEX_LINK_BDL_ESA_e = 0x2C };
enum { fpcNm_ESA_e = 0xDD };
enum { dPa_name_ID_AK_JN_HAMON00 = 0x33 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
static inline void* fpcLy_CurrentLayer() { return gabi::call<void*>(0x025DED64); }
static inline u32 fpcSCtRq_Request(void* layer, s16 name, u32 cb, u32 data, void* append) {
    return gabi::call<u32>(0x025E14A8, layer, name, cb, data, append);
}
/* fpcM_Create(name, NULL, append): inline in HD */
static inline u32 fpcM_Create(s16 name, u32 cb, void* append) { return fpcSCtRq_Request(fpcLy_CurrentLayer(), name, cb, 0, append); }
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline void cBgS_Chk_dt(void* c, s32 flags) { gabi::call(0x02008DAC, c, flags); }
static inline void dPa_rippleEcallBack_ct(void* p) { gabi::call(0x025A9084, p); }
/* debug registers REG18_F(i) (HD tuning values used by the bait) */
#define REG18_F(i) REG_F(18, i)

/* dPa_rippleEcallBack (HD 0x14): vtable, emitter, ..., rate at +0x10 */
struct dPa_rippleEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
    JPABaseEmitter* getEmitter() { return mpEmitter; }
    void setRate(f32 r) { mRate = r; }
};

struct esa_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ u8 _3B5[3];
    /* 0x3B8 */ be<f32> mGroundHeight;  /* HD: the water height of the last bg_check */
    /* 0x3BC */ be<s8> mActionState;
    /* 0x3BD */ be<s8> mState;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ dPa_rippleEcallBack_l field_0x2A4;
    /* 0x3D4 */ be<u8> field_0x2B9;
    /* 0x3D5 */ be<u8> field_0x2BA;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ gptr<J3DModel> mpModel;
    /* 0x3DC */ be<s16> mTimer[2];
    /* 0x3E0 */ be<s16> mBobAngle;      /* HD: bobbing phase on the water */
};
WWHD_OFFSET(esa_class, field_0x2A4, 0x3C0);
WWHD_OFFSET(esa_class, mpModel, 0x3D8);
WWHD_OFFSET(esa_class, mBobAngle, 0x3E0);

/* 0213007C */
static BOOL daEsa_Draw(esa_class* i_this) {
    WWHD_FUNC(0x0213007C, BOOL, i_this);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return true;
}
VERIFY(0x0213007C, daEsa_Draw);

/* ---- inline stack objects of bg_check (this TU's vtables) ---- */
static const dBgS_GndChk_vt l_gnd_vt = {0x1000E584, 0x1000E594, 0x1000E5B4, 0x1000E5A4};
static const dBgS_LinChk_vt l_lin_vt = {0x1000E5C4, 0x1000E5D4, 0x1000E5F4, 0x1000E5E4};
static void dBgS_LinChk_dt_l(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x1000E5F4);
    gabi::store<u32>(b + 0x20, 0x1000E564);
    gabi::store<u32>(b + 0x64, 0x1000E574);
    cBgS_LinChk_dt(c, 0);
}
static void dBgS_GndChk_dt_l(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x40, 0x1000E5B4);
    gabi::store<u32>(b + 0x20, 0x1000E594);
    gabi::store<u32>(b + 0x4C, 0x1000E574);
    cBgS_Chk_dt(c, 0);
}

/* bg_check (inlined into daEsa_Execute). HD: no +5 on the ground height; the water height is
 * stored in mGroundHeight (when the sea check does not apply); state 3 for water; a resting
 * bait is lifted 3 units above the ground. */
static inline void bg_check(esa_class* i_this) {
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, l_gnd_vt, false);
    {
        u32 g = gabi::ea(gndChk.get());
        f32 x = i_this->current.pos.x, y = i_this->current.pos.y + 100.0f, z = i_this->current.pos.z;
        gabi::store<f32>(g + 0x24, x);
        gabi::store<f32>(g + 0x28, y);
        gabi::store<f32>(g + 0x2C, z);
    }
    f32 gnd = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    s8 state = 1;
    bool rest;
    f32 wave;
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z) &&
        (wave = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z), !(gnd > wave))) {
        gnd = wave;
        state = 2;
        rest = !(i_this->speed.y > 0.0f);
    } else {
        gabi::Local<cXyz> sp54;
        sp54->x = i_this->current.pos.x;
        sp54->y = i_this->current.pos.y;
        sp54->z = i_this->current.pos.z;
        sp54->y = sp54->y + 100.0f;
        f32 waterHeight = dBgS_GetWaterHeight(sp54);
        i_this->mGroundHeight = waterHeight;
        if (waterHeight != -1000000000.0f && !(gnd > waterHeight)) {
            gnd = waterHeight;
            state = 3;
        }
        rest = !(i_this->speed.y > 0.0f);
    }
    if (rest && !(i_this->current.pos.y > gnd)) {
        i_this->current.pos.y = gnd;
        i_this->mState = state;
        if (state == 1)
            i_this->current.pos.y = i_this->current.pos.y + 3.0f;
    } else {
        i_this->mState = 0;
    }

    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, l_lin_vt, false);
    gabi::Local<cXyz> sp5C, spE0, spD4, sp50, spEC;
    cXyz_mi(&i_this->current.pos, sp5C, &i_this->old.pos);
    cXyz_ml(sp5C, spE0, 1.5f);
    cXyz_pl(&i_this->old.pos, spD4, spE0);
    sp50->copy(*spD4);
    sp50->y = i_this->old.pos.y;
    cXyz_mi(&i_this->old.pos, sp5C, sp50);
    spEC->copy(*sp5C);
    if (std_sqrtf(PSVECSquareMag(spEC)) > 1.0f) {
        dBgS_LinChk_Set(linChk, &i_this->old.pos, sp50, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            i_this->current.angle.y -= 0x8000;
            i_this->current.pos.x = i_this->old.pos.x;
            i_this->current.pos.z = i_this->old.pos.z;
            i_this->speedF = i_this->speedF * 0.5f;
            gabi::Local<cXyz> sp6C, sp60;
            sp6C->x = 0.0f;
            sp6C->y = 0.0f;
            sp6C->z = i_this->speedF;
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            MtxPosition(sp6C, sp60);
            i_this->speed.x = sp60->x;
            i_this->speed.z = sp60->z;
        }
    } else {
        i_this->actor_status &= ~0x4000u; /* cLib_offBit(actor_status, fopAcStts_UNK4000_e) */
    }
    dBgS_LinChk_dt_l(linChk);
    dBgS_GndChk_dt_l(gndChk);
}

/* the matrix and lighting update at the end of daEsa_Execute (GHS duplicates it per exit) */
static inline BOOL esa_setMtx(esa_class* i_this) {
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    return true;
}

/* ripple on the water (HD: rate from a debug register + 0.3; GameCube setRate(0.4f)) */
static inline void esa_setRipple(esa_class* i_this) {
    /* static cXyz ripple_scale(0.2f, 0.2f, 0.2f); */
    be<u32>& guard = *gabi::at<be<u32>>(0x10463C9C);
    cXyz* ripple_scale = gabi::at<cXyz>(0x10463CB4);
    if (guard == 0) {
        guard = 1;
        ripple_scale->x = 0.2f;
        ripple_scale->y = 0.2f;
        ripple_scale->z = 0.2f;
    }
    /* dComIfGp_particle_setShipTail: dPa_control_c::set with group 5 */
    dPa_control_set(dComIfGp_getParticle(), 5, dPa_name_ID_AK_JN_HAMON00, &i_this->current.pos, nullptr, ripple_scale, 0xFF,
                    (dPa_levelEcallBack*)(void*)&i_this->field_0x2A4, -1, nullptr, nullptr, nullptr);
    if (i_this->field_0x2A4.getEmitter()) {
        i_this->field_0x2A4.setRate(REG18_F(2) + 0.3f);
    }
}

/* 021300C4: esa_1_move and bg_check inlined */
static BOOL daEsa_Execute(esa_class* i_this) {
    WWHD_FUNC(0x021300C4, BOOL, i_this);
    for (int i = 0; i < 2; i++) {
        if (i_this->mTimer[i] != 0) {
            i_this->mTimer[i] -= 1;
        }
    }

    /* esa_1_move */
    u32 act = (u32)(s32)i_this->mActionState;
    if (act < 1 || act == 1) {
        if (act < 1) {
            i_this->current.angle.y += (s16)gabi::ftoi(cM_rndFX(4000.0f));
            i_this->current.angle.z = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            gabi::Local<cXyz> sp24;
            sp24->x = 0.0f;
            sp24->y = cM_rndF(8.0f) + 15.0f;
            sp24->z = cM_rndF(5.0f) + 10.0f;
            i_this->speedF = sp24->z;
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            MtxPosition(sp24, &i_this->speed);
            i_this->mActionState = 1;
        }
        /* case 1 */
        s8 st = i_this->mState;
        if (st != 0) {
            if (st >= 2) { /* HD: also water (state 3) */
                i_this->mTimer[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 200.0f);
                bool noEmitter = i_this->field_0x2A4.getEmitter() == nullptr;
                i_this->mActionState = 3;
                if (noEmitter)
                    esa_setRipple(i_this);
            } else {
                f32 spdY = i_this->speed.y;
                if (spdY < 5.0f) {
                    i_this->speed.y = spdY * -(cM_rndF(0.05f) + 0.15f);
                    i_this->current.angle.z = (s16)gabi::ftoi(cM_rndFX(32768.0f));
                }
                i_this->current.angle.y += (s16)gabi::ftoi(cM_rndFX(8000.0f));
                f32 k = cM_rndF(0.3f) + 0.3f;
                i_this->speedF = i_this->speedF * k;
                gabi::Local<cXyz> sp24;
                sp24->y = 0.0f;
                sp24->x = 0.0f;
                sp24->z = i_this->speedF;
                cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
                gabi::Local<cXyz> sp18;
                MtxPosition(sp24, sp18);
                i_this->speed.x = sp18->x;
                i_this->speed.z = sp18->z;
                if (i_this->speedF < 0.1f) {
                    i_this->mActionState = 4; /* HD: GameCube 2 */
                    i_this->mTimer[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 200.0f);
                    return esa_setMtx(i_this);
                }
            }
        }
        f32 sy = i_this->speed.y;
        i_this->current.pos.x = i_this->current.pos.x + i_this->speed.x;
        i_this->current.pos.y = i_this->current.pos.y + sy;
        i_this->current.pos.z = i_this->current.pos.z + i_this->speed.z;
        i_this->speed.y = sy - 3.0f;
        bg_check(i_this);
    } else if (act == 3) {
        /* HD: no field_0x2A4.remove(); bobs on the water or at the water height */
        if (dComIfGp_evmng_startCheckOld(STR(0x1000E614) /* "SO_ESA_XY" */)) {
            i_this->mTimer[0] = (s16)(i_this->mTimer[0] | 0x20);
        }
        f32 y;
        if (i_this->mState == 2) {
            y = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
        } else {
            y = i_this->mGroundHeight;
        }
        s16 bob = (s16)(i_this->mBobAngle + 3000);
        i_this->mBobAngle = bob;
        i_this->current.pos.y = y + cM_ssin(bob);
        if (i_this->mTimer[0] == 0) {
            i_this->mActionState = 4;
        }
        if (i_this->field_0x298 == 0x23) { /* HD */
            if (cM_rndF(1.0f) < 0.5f) {
                gabi::Local<cXyz> pos;
                pos->x = i_this->current.pos.x;
                pos->y = y;
                pos->z = i_this->current.pos.z;
                fopKyM_createWpillar(pos, REG18_F(3) + 0.3f, REG18_F(4) + 0.3f, 0);
            }
            fopAcM_delete(i_this); /* the bait is taken: a splash half of the time */
        }
    } else if (act == 4) {
        /* HD: shrinks away (GameCube: deleted when the timer ends) */
        if (i_this->mTimer[0] == 0) {
            cLib_addCalc0(&i_this->scale.x, 1.0f, 0.05f);
            f32 s = i_this->scale.x;
            i_this->scale.z = s;
            i_this->scale.y = s;
            J3DModel* m = i_this->mpModel;
            gabi::store<f32>(gabi::ea(m) + 0xBC, s);
            gabi::store<f32>(gabi::ea(m) + 0xC0, s);
            gabi::store<f32>(gabi::ea(m) + 0xC4, s);
            if (i_this->scale.x < 0.05f) {
                fopAcM_delete(i_this);
            }
        }
    }
    /* case 2 (HD: nothing) and others */
    return esa_setMtx(i_this);
}
VERIFY(0x021300C4, daEsa_Execute);

/* 02130D64 */
static BOOL daEsa_IsDelete(esa_class* i_this) {
    WWHD_FUNC(0x02130D64, BOOL, i_this);
    return true;
}
VERIFY(0x02130D64, daEsa_IsDelete);

/* 02130D6C */
static BOOL daEsa_Delete(esa_class* i_this) {
    WWHD_FUNC(0x02130D6C, BOOL, i_this);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)&i_this->field_0x2A4); /* remove() */
    return true;
}
VERIFY(0x02130D6C, daEsa_Delete);

/* 02130D94 */
static BOOL daEsa_CreateHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02130D94, BOOL, i_actor);
    esa_class* i_this = static_cast<esa_class*>(i_actor);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000E674) /* "Link" */, dRes_INDEX_LINK_BDL_ESA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x250, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000E67C), 0x250, STR(0x1000E688));
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    return i_this->mpModel != nullptr;
}
VERIFY(0x02130D94, daEsa_CreateHeap);

/* 02130E30 */
static cPhs_State daEsa_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02130E30, cPhs_State, i_actor);
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = ESA_VTBL;
            dPa_rippleEcallBack_ct(gabi::at<u8>(gabi::ea(i_actor) + 0x3C0));
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    esa_class* i_this = static_cast<esa_class*>(i_actor);
    i_this->field_0x2B9 = fopAcM_GetParam(i_this) & 0xFF;
    i_this->field_0x2BA = fopAcM_GetParam(i_this) >> 8 & 0xFF;
    /* HD: no REG0_S(0) override */
    if (!fopAcM_entrySolidHeap(i_this, 0x02130D94 /* daEsa_CreateHeap */, 0x4C0)) {
        return cPhs_ERROR_e;
    }
    if (i_this->field_0x2BA == 0) {
        s8 num = i_this->field_0x2B9;
        if (num > 0) {
            for (int i = 0; i < num; i++) {
                u8* params = fopAcM_CreateAppend();
                u32 pa = gabi::ea(params);
                /* cXyz pos = player->getLeftHandPos() (HD: +0x3F0) */
                u32 pl = gabi::ea(player);
                f32 px = gabi::load<f32>(pl + 0x3F0), pz = gabi::load<f32>(pl + 0x3F8), py = gabi::load<f32>(pl + 0x3F4);
                gabi::store<f32>(pa + 0x4, px);
                gabi::store<f32>(pa + 0x8, py);
                gabi::store<f32>(pa + 0xC, pz);
                gabi::store<s16>(pa + 0x10, 0);
                gabi::store<s16>(pa + 0x12, i_this->current.angle.y);
                s16 az = (s16)gabi::ftoi(cM_rndF(65536.0f));
                gabi::store<u32>(pa + 0x0, 0x000000FF);
                gabi::store<s16>(pa + 0x14, az);
                fpcM_Create(fpcNm_ESA_e, 0, params);
            }
        }
    }
    /* HD: no REG0_F(5)/(6) terms; the scale goes to the actor too */
    f32 hi = 1.0f, lo = 0.65f;
    f32 scaleF = cM_rndF(hi - lo) + lo;
    i_this->scale.x = scaleF;
    i_this->scale.y = scaleF;
    i_this->scale.z = scaleF;
    J3DModel* m = i_this->mpModel;
    gabi::store<f32>(gabi::ea(m) + 0xBC, scaleF);
    gabi::store<f32>(gabi::ea(m) + 0xC0, scaleF);
    gabi::store<f32>(gabi::ea(m) + 0xC4, scaleF);
    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
    i_this->mBobAngle = (s16)gabi::ftoi(cM_rndF(65535.0f)); /* HD */
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02130E30, daEsa_Create);

/* 02131014: static initialisation of the translation unit (header statics; HD: this unit's
 * layout puts the two objects at P+0xC/P+0xD and the zeroed object at P+0x10) */
static void __sinit_d_a_esa_cpp() {
    WWHD_FUNC(0x02131014, void, (u32)0);
    const u32 P = 0x10463C94, D = 0x101B49C8;
    for (int i = 0; i < 4; i++) gabi::store<u32>(P + 0x10 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 0xC);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 0xD);
    __register_global_object(D + 0x18);
}
VERIFY(0x02131014, __sinit_d_a_esa_cpp);

/* 021310A8: sead::SafeString deleting destructor (this TU's vtable 0x1000E54C) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021310A8, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021310A8, SafeString_dt);

/* 021310BC: esa_class deleting destructor (compiler-generated, HD virtual destructor) */
static void esa_class_dt(esa_class* i_this, s32 flags) {
    WWHD_FUNC(0x021310BC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021310BC, esa_class_dt);

/* 02131110: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02131110, void, (u32)0);
}
VERIFY(0x02131110, SafeString_assureTerminationImpl);
