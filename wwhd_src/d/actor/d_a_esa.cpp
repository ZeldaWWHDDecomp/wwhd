/**
 * d_a_esa.cpp (WWHD)
 * Item - All-Purpose Bait
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_esa.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_esa.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/f_pc/f_pc.h"

#define ESA_VTBL 0x1000E604        /* esa_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000E54C /* this TU's sead::SafeString vtable */
enum { dRes_INDEX_LINK_BDL_ESA_e = 0x2C };
enum { dPa_name_ID_AK_JN_HAMON00 = 0x33 };

// HD: the event check takes a name, whereas the shared startCheck takes an event index.
BOOL esaEventStartCheck(void*, const char*);
fopAcM_prm_class* esaCreateAppend();
u32 esaRequestCreate(layer_class*, s16, u32, u32, fopAcM_prm_class*);
void esaCheckDestruct(cBgS_Chk*, s32);
void esaRippleConstruct(EsaRippleCallback*);
void esaActorDestruct(fopAc_ac_c*, s32);
void esaFirstHeaderObjectConstruct(be<u8>*);
void esaSecondHeaderObjectConstruct(be<u8>*);
static EsaHeaderStatics l_headerStatics;
static EsaDestructorRecords l_destructorRecords;
static cXyz ripple_scale;
static constexpr u32 LIN_POLYPASS_VTBL = 0x1000E5F4, LIN_POLY_VTBL = 0x1000E564;
static constexpr u32 GRPPASS_VTBL = 0x1000E574, GND_POLYPASS_VTBL = 0x1000E5B4;
static constexpr u32 GND_POLY_VTBL = 0x1000E594;
#define REG18_F(i) REG_F(18, i)

static u32 esaCreate(s16 name, u32 cb, fopAcM_prm_class* append) {
    layer_class* layer = fpcLy_CurrentLayer();
    return esaRequestCreate(layer, name, cb, 0, append);
}

/* 0213007C */
static BOOL daEsa_Draw(esa_class* i_this) {
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return true;
}

/* ---- inline stack objects of bg_check (this TU's vtables) ---- */
static const dBgS_GndChk_vt l_gnd_vt = {0x1000E584, 0x1000E594, 0x1000E5B4, 0x1000E5A4};
static const dBgS_LinChk_vt l_lin_vt = {0x1000E5C4, 0x1000E5D4, 0x1000E5F4, 0x1000E5E4};
static void esaLineCheckDestruct(dBgS_LinChk* c) {
    c->dBgS_PolyPassChk::__vtbl = LIN_POLYPASS_VTBL;
    c->cBgS_PolyInfo::__vtbl = LIN_POLY_VTBL;
    c->dBgS_GrpPassChk::__vtbl = GRPPASS_VTBL;
    cBgS_LinChk_dt(c, 0);
}
static void esaGroundCheckDestruct(dBgS_GndChk* c) {
    c->dBgS_PolyPassChk::__vtbl = GND_POLYPASS_VTBL;
    c->cBgS_PolyInfo::__vtbl = GND_POLY_VTBL;
    c->dBgS_GrpPassChk::__vtbl = GRPPASS_VTBL;
    esaCheckDestruct(static_cast<cBgS_Chk*>(c), 0);
}

/* bg_check (inlined into daEsa_Execute). HD: no +5 on the ground height; the water height is
 * stored in mGroundHeight (when the sea check does not apply); state 3 for water; a resting
 * bait is lifted 3 units above the ground. */
/* inlined into daEsa_Execute */
static inline void bg_check(esa_class* i_this) {
    dBgS_GndChk gndChk;
    dBgS_GndChk_ct(&gndChk, l_gnd_vt, false);
    {
        // HD: preserve the original float load/store order.
        f32 x = i_this->current.pos.x, y = i_this->current.pos.y + 100.0f,
            z = i_this->current.pos.z;
        gndChk.m_pos.x = x;
        gndChk.m_pos.y = y;
        gndChk.m_pos.z = z;
    }
    f32 gnd = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
    s8 state = 1;
    bool rest;
    f32 wave;
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z) &&
        (wave = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z), !(gnd > wave))) {
        gnd = wave;
        state = 2;
        rest = !(i_this->speed.y > 0.0f);
    } else {
        cXyz sp54;
        sp54.x = i_this->current.pos.x;
        sp54.y = i_this->current.pos.y;
        sp54.z = i_this->current.pos.z;
        sp54.y = sp54.y + 100.0f;
        f32 waterHeight = dBgS_GetWaterHeight(&sp54);
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

    dBgS_LinChk linChk;
    dBgS_LinChk_ct(&linChk, l_lin_vt, false);
    cXyz sp5C;
    cXyz spE0;
    cXyz spD4;
    cXyz sp50;
    cXyz spEC;
    cXyz_mi(&i_this->current.pos, &sp5C, &i_this->old.pos);
    cXyz_ml(&sp5C, &spE0, 1.5f);
    cXyz_pl(&i_this->old.pos, &spD4, &spE0);
    sp50.copy(spD4);
    sp50.y = i_this->old.pos.y;
    cXyz_mi(&i_this->old.pos, &sp5C, &sp50);
    spEC.copy(sp5C);
    if (std_sqrtf(PSVECSquareMag(&spEC)) > 1.0f) {
        dBgS_LinChk_Set(&linChk, &i_this->old.pos, &sp50, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
            i_this->current.angle.y -= 0x8000;
            i_this->current.pos.x = i_this->old.pos.x;
            i_this->current.pos.z = i_this->old.pos.z;
            i_this->speedF = i_this->speedF * 0.5f;
            cXyz sp6C;
            cXyz sp60;
            sp6C.x = 0.0f;
            sp6C.y = 0.0f;
            sp6C.z = i_this->speedF;
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            MtxPosition(&sp6C, &sp60);
            i_this->speed.x = sp60.x;
            i_this->speed.z = sp60.z;
        }
    } else {
        i_this->actor_status &= ~static_cast<u32>(
            fopAcStts_UNK4000_e); /* cLib_offBit(actor_status, fopAcStts_UNK4000_e) */
    }
    esaLineCheckDestruct(&linChk);
    esaGroundCheckDestruct(&gndChk);
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
/* inlined into daEsa_Execute */
static inline void esa_setRipple(esa_class* i_this) {
    if (l_headerStatics.mRippleGuard == 0) {
        l_headerStatics.mRippleGuard = 1;
        ripple_scale.x = 0.2f;
        ripple_scale.y = 0.2f;
        ripple_scale.z = 0.2f;
    }
    /* dComIfGp_particle_setShipTail: dPa_control_c::set with group 5 */
    dPa_control_set(dComIfGp_getParticle(), 5, dPa_name_ID_AK_JN_HAMON00, &i_this->current.pos,
                    nullptr, &ripple_scale, 0xFF, (dPa_levelEcallBack*)(void*)&i_this->field_0x2A4,
                    -1, nullptr, nullptr, nullptr);
    if (i_this->field_0x2A4.getEmitter()) {
        i_this->field_0x2A4.setRate(REG18_F(2) + 0.3f);
    }
}

// HD: esa_1_move and bg_check inlined
/* 021300C4 */
static BOOL daEsa_Execute(esa_class* i_this) {
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
            cXyz sp24;
            sp24.x = 0.0f;
            sp24.y = cM_rndF(8.0f) + 15.0f;
            sp24.z = cM_rndF(5.0f) + 10.0f;
            i_this->speedF = sp24.z;
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            MtxPosition(&sp24, &i_this->speed);
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
                cXyz sp24;
                sp24.y = 0.0f;
                sp24.x = 0.0f;
                sp24.z = i_this->speedF;
                cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
                cXyz sp18;
                MtxPosition(&sp24, &sp18);
                i_this->speed.x = sp18.x;
                i_this->speed.z = sp18.z;
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
        if (esaEventStartCheck(dComIfGp_getPEvtManager(), "SO_ESA_XY")) {
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
                cXyz pos;
                pos.x = i_this->current.pos.x;
                pos.y = y;
                pos.z = i_this->current.pos.z;
                fopKyM_createWpillar(&pos, REG18_F(3) + 0.3f, REG18_F(4) + 0.3f, 0);
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
            m->mBaseScale.x = s;
            m->mBaseScale.y = s;
            m->mBaseScale.z = s;
            if (i_this->scale.x < 0.05f) {
                fopAcM_delete(i_this);
            }
        }
    }
    /* case 2 (HD: nothing) and others */
    return esa_setMtx(i_this);
}

/* 02130D64 */
static BOOL daEsa_IsDelete(esa_class* i_this) {
    return true;
}

/* 02130D6C */
static BOOL daEsa_Delete(esa_class* i_this) {
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)&i_this->field_0x2A4); /* remove() */
    return true;
}

/* 02130D94 */
static BOOL daEsa_CreateHeap(fopAc_ac_c* i_actor) {
    esa_class* i_this = static_cast<esa_class*>(i_actor);
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes("Link", dRes_INDEX_LINK_BDL_ESA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x250, modelData != NULL) */
        JUT_ASSERT_fail("d_a_esa.cpp", 0x250, "modelData != NULL");
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    return i_this->mpModel != nullptr;
}

/* 02130E30 */
static cPhs_State daEsa_Create(fopAc_ac_c* i_actor) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = ESA_VTBL;
            esaRippleConstruct(&static_cast<esa_class*>(i_actor)->field_0x2A4);
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    esa_class* i_this = static_cast<esa_class*>(i_actor);
    i_this->field_0x2B9 = fopAcM_GetParam(i_this) & 0xFF;
    i_this->field_0x2BA = fopAcM_GetParam(i_this) >> 8 & 0xFF;
    /* HD: no REG0_S(0) override */
    if (!fopAcM_entrySolidHeap(i_this, daEsa_CreateHeap, 0x4C0)) {
        return cPhs_ERROR_e;
    }
    if (i_this->field_0x2BA == 0) {
        s8 num = i_this->field_0x2B9;
        if (num > 0) {
            for (int i = 0; i < num; i++) {
                fopAcM_prm_class* params = esaCreateAppend();
                /* cXyz pos = player->getLeftHandPos() (HD: +0x3F0) */
                EsaPlayerView* playerView = static_cast<EsaPlayerView*>(player);
                f32 px = playerView->mLeftHandPos.x, pz = playerView->mLeftHandPos.z,
                    py = playerView->mLeftHandPos.y;
                params->base.position.x = px;
                params->base.position.y = py;
                params->base.position.z = pz;
                params->base.angle.x = 0;
                params->base.angle.y = i_this->current.angle.y;
                s16 az = (s16)gabi::ftoi(cM_rndF(65536.0f));
                params->base.parameters = 0x000000FF;
                params->base.angle.z = az;
                esaCreate(fpcNm_ESA_e, 0, params);
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
    m->mBaseScale.x = scaleF;
    m->mBaseScale.y = scaleF;
    m->mBaseScale.z = scaleF;
    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
    i_this->mBobAngle = (s16)gabi::ftoi(cM_rndF(65535.0f));             /* HD */
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and template code of this translation unit.
// HD: header objects occupy P+0xC/P+0xD, with zeroed words at P+0x10.
/* 02131014 */
static void __sinit_d_a_esa_cpp() {
    for (int i = 0; i < 4; i++)
        l_headerStatics.mZero[i] = 0;
    __register_global_object(gabi::ea(&l_destructorRecords.mRecords[0]));
    l_headerStatics.mNegativePi = -3.1415927f;
    l_headerStatics.mPositivePi = 3.1415927f;
    esaFirstHeaderObjectConstruct(&l_headerStatics.mFirstObject);
    __register_global_object(gabi::ea(&l_destructorRecords.mRecords[1]));
    esaSecondHeaderObjectConstruct(&l_headerStatics.mSecondObject);
    __register_global_object(gabi::ea(&l_destructorRecords.mRecords[2]));
}

// HD: sead::SafeString deleting destructor (this TU's vtable 0x1000E54C)
/* 021310A8 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

// HD: esa_class deleting destructor (compiler-generated, HD virtual destructor)
/* 021310BC */
static void esa_class_dt(esa_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        esaActorDestruct(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 02131110 */
static void SafeString_assureTerminationImpl(void* p) {
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_headerStatics 10463C94
data l_destructorRecords 101B49C8
data ripple_scale 10463CB4
str 1000E614
str 1000E674
str 1000E67C
str 1000E688 "modelData != NULL"
ext 025445B8 BOOL esaEventStartCheck(void*,const char*)
ext 025D5600 fopAcM_prm_class* esaCreateAppend()
ext 025E14A8 u32 esaRequestCreate(layer_class*,s16,u32,u32,fopAcM_prm_class*)
ext 02008DAC void esaCheckDestruct(cBgS_Chk*,s32)
ext 025A9084 void esaRippleConstruct(EsaRippleCallback*)
ext 025D50BC void esaActorDestruct(fopAc_ac_c*,s32)
ext 028ED6F8 void esaFirstHeaderObjectConstruct(be<u8>*)
ext 028EAB2C void esaSecondHeaderObjectConstruct(be<u8>*)
*/
// clang-format on
