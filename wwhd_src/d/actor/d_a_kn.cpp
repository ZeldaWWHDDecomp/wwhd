/**
 * d_a_kn.cpp (WWHD)
 * NPC - Crab
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kn.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * Float comparisons are written in the form the WWHD code tests where it differs for NaN
 * (GHS branches on the negated comparison): `!(x > 1.0f)` for the source's `x <= 1.0f`.
 */
#include "d/actor/d_a_kn.h"

// HD: constructor/destructor vtables and common-header statics are per translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x100131FC,
    .objAcch = {0x10013214, 0x10013234, 0x10013224},
    .objAcchDt = {0x10013224, 0x10013234},
    .headerStatics = {0x10464C04, 0x101B85B4, 0x10464C10},
};
static constexpr u32 KN_VTBL = 0x10013244;

/* 021A1FA0 */
static BOOL daKN_Draw(kn_class* i_this) {
    fopAc_ac_c* a_this = i_this;
    if (i_this->m2CC < 0.0f) {
        return TRUE;
    }
    J3DModel* pJVar1 = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), pJVar1, &a_this->tevStr);
    i_this->mpMorf->updateDL();
    dSnap_RegistFig(DSNAP_TYPE_KN, a_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 021A2038 */
void anm_init(kn_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float speed,
              int soundResIdx) {
    if (soundResIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Kn", anmResIdx);
        void* snd = dComIfG_getObjectRes("Kn", soundResIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Kn", anmResIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 021A2160 */
void smoke_set(kn_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 fVar1 = REG_F(8, 10) + 1.0f;
    // HD: smoke removal calls end directly; actor deletion uses virtual remove.
    i_this->m314.end();
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    /* dComIfGp_particle_setToon: group 2 */
    dPa_control_c* pa = dComIfGp_getParticle();
    i_this->m334 =
        pa->set(2, ID_AK_JT_ELEMENTSMOKE01, &i_this->m300, &i_this->shape_angle, nullptr, 0xB9,
                (dPa_levelEcallBack*)&i_this->m314, roomNo, nullptr, nullptr, nullptr);
    if (i_this->m334 != nullptr) {
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mMaxFrame = 0x19;
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mInitialVelOmni = 0.0f;
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mInitialVelAxis = 5.0f;
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mRate = 3.0f;
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mInitialVelDir = 2.0f;
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->setGlobalScale(fVar1, fVar1, fVar1);
        reinterpret_cast<Kn::Emitter*>(i_this->m334.get())->mLifeTime = 25;
    }
}

/* 021A22AC */
void shibuki_set(kn_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 fVar1 = REG_F(8, 11) + 0.55f;
    JPABaseEmitter* pJVar2 = dComIfGp_particle_set(ID_AK_JN_ELEMENTSHIBUKI00, &i_this->current.pos);
    if (pJVar2 != nullptr) {
        Kn::Emitter* e = reinterpret_cast<Kn::Emitter*>(pJVar2);
        e->mRate = 1.5f;
        e->mSpread = 1.0f;
        e->mInitialVelDir = 4.0f;
        e->mMaxFrame = 0x19;
        e->setGlobalScale(fVar1, fVar1, fVar1);
    }
}

static inline f32 kn_dist(f32 x, f32 z) { return std_sqrtf(gabi::fmadds(x, x, z * z)); }

/* inlined into daKN_Execute */
static void kn_move(kn_class* i_this) {
    f32 f26 = 3.0f;
    f32 f29 = 200.0f;
    f32 f28 = 300.0f;
    f32 f27 = 500.0f;
    f32 fVar8 = i_this->m2EC.x - i_this->current.pos.x;
    f32 fVar1 = i_this->m2EC.z - i_this->current.pos.z;
    s16 sVar5;

    switch (i_this->m2BD) {
    case Mode_10_e: {
        int idx = dRes_INDEX_KN_BCK_WAIT01_e;
        if (!(cM_rnd() < 0.5f))
            idx = dRes_INDEX_KN_BCK_WAIT02_e;
        anm_init(i_this, idx, 5.0f, 2, 1.0f, -1);
        i_this->m2D2 = 0x800;
        i_this->m2CA = (s16)gabi::ftoi(cM_rndF(2.0f) + 1.0f);
        i_this->m2BD = i_this->m2BD + 1;
    }
        [[fallthrough]];
    case Mode_11_e:
        cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
        if (i_this->mpMorf->checkFrame(43.0f)) {
            i_this->m2CA = i_this->m2CA - 1;
            if (i_this->m2CA <= 0) {
                i_this->m2BD = Mode_12_e;
            }
        }
        if ((i_this->m2C2[2] == 0) && fopAcM_searchPlayerDistance(i_this) < f28) {
            i_this->m2BD = Mode_18_e;
        }
        break;
    case Mode_12_e: {
        anm_init(i_this, dRes_INDEX_KN_BCK_WALK_e, 5.0f, 2, 1.0f, -1);
        f32 ang = (f32)(s16)i_this->current.angle.y;
        f32 rnd = cM_rndFX(10752.0f);
        i_this->m2D0 = (s16)gabi::ftoi(ang + rnd);
        i_this->m2E8 = f26;
        if (cM_rnd() < 0.5f) {
            i_this->m2E8 = -f26;
        }
        if (i_this->m2C2[2] != 0) {
            i_this->m2D0 = cM_atan2s(fVar8, fVar1);
            i_this->m2E8 = f26;
        }
        i_this->m2C2[0] = (s16)gabi::ftoi(20.0f + cM_rndF(40.0f));
        i_this->m2E4 = i_this->m2E8;
        i_this->m2D2 = 0x800;
        i_this->m2BE = 0;
        i_this->m2BD = i_this->m2BD + 1;
    }
        [[fallthrough]];
    case Mode_13_e:
        if (fopAcM_searchPlayerDistance(i_this) < f28) {
            i_this->m2BD = Mode_18_e;
        } else if (i_this->m2C2[2] == 0) {
            if (i_this->m2BE != 0) {
                shibuki_set(i_this);
                i_this->m2BF = 0;
                i_this->m2BD = Mode_15_e;
            } else if (kn_dist(fVar8, fVar1) > f29) {
                i_this->m2C2[1] = 0x14;
                i_this->m2E4 = -(f32)i_this->m2E8; /* m2E8 * -1.0f */
                i_this->m2BD = Mode_14_e;
            } else if (i_this->m2C2[0] == 0) {
                i_this->m2BD = Mode_10_e;
            }
        }
        break;
    case Mode_14_e:
        if (i_this->m2C2[1] == 0) {
            if (kn_dist(fVar8, fVar1) > f29) {
                i_this->m2D0 = cM_atan2s(fVar8, fVar1);
                i_this->m2E4 = std::fabs((f32)i_this->m2E8);
                i_this->m2C2[1] = 8;
            } else {
                i_this->m2BD = Mode_10_e;
            }
        }
        break;
    case Mode_15_e:
        i_this->speedF = 0.0f;
        i_this->m2E4 = 0.0f;
        i_this->m2C0 = 0;
        i_this->m2C2[0] = 0x14;
        anm_init(i_this, dRes_INDEX_KN_BCK_PATA_e, 5.0f, 2, 1.0f, -1);
        i_this->m2BD = i_this->m2BD + 1;
        [[fallthrough]];
    case Mode_16_e:
        if (i_this->m2C2[0] == 0) {
            f32 fVar9 = -7.0f;
            if (i_this->scale.x > 1.5f) {
                fVar9 = -11.0f;
            }
            if (i_this->m2C0 == 0) {
                if (!(std::fabs(i_this->m2CC - fVar9) > 1.0f)) {
                    if (i_this->m2BF != 0) {
                        i_this->m314.end();
                    }
                    i_this->m2CC = fVar9;
                    i_this->m2C0 = 1;
                    i_this->m2C2[3] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
                } else {
                    cLib_addCalc2(&i_this->m2CC, fVar9, 0.1f, 1.0f);
                }
            }
            if ((i_this->m2C0 != 0) && (i_this->m2C2[3] == 0) &&
                fopAcM_searchPlayerDistance(i_this) > f27) {
                if (i_this->m2BF != 0) {
                    // HD: copy X/Z individually and add the height directly to Y.
                    i_this->m300.x = i_this->current.pos.x;
                    i_this->m300.z = i_this->current.pos.z;
                    i_this->m300.y = i_this->current.pos.y + 20.0f;
                    smoke_set(i_this);
                } else {
                    shibuki_set(i_this);
                }
                anm_init(i_this, dRes_INDEX_KN_BCK_PATA_e, 5.0f, 2, 1.0f, -1);
                i_this->m2BD = Mode_17_e;
            }
        }
        break;
    case Mode_17_e:
        cLib_addCalc0(&i_this->m2CC, 0.1f, 1.0f);
        if (i_this->m2CC < 0.2f) {
            if (i_this->m2BF != 0) {
                i_this->m314.end();
            }
            i_this->m2CC = 0.0f;
            i_this->m2C2[2] = 0x28;
            i_this->m2BD = Mode_12_e;
        }
        break;
    case Mode_18_e:
        anm_init(i_this, dRes_INDEX_KN_BCK_WALK_e, 5.0f, 2, 2.0f, -1);
        i_this->m2E8 = 40.0f;
        i_this->m2C2[0] = 0;
        i_this->m2C2[1] = 0;
        i_this->m2C2[2] = 0;
        i_this->m2D2 = 0x100;
        i_this->m2E4 = i_this->m2E8;
        i_this->m2BE = 0;
        sVar5 = fopAcM_searchPlayerAngleY(i_this) - -0x8000;
        if ((s16)cLib_distanceAngleS(sVar5, i_this->current.angle.y) < 0x4000) {
            i_this->m2D0 = sVar5;
        } else {
            i_this->m2D0 = i_this->current.angle.y;
            s16 d1 = (s16)cLib_distanceAngleS(sVar5, i_this->current.angle.y);
            s16 d2 = (s16)(cLib_distanceAngleS(sVar5, i_this->current.angle.y) - -0x8000);
            if (d1 > d2) {
                i_this->m2E4 = -(f32)i_this->m2E8; /* m2E8 * -1.0f */
            }
        }
        i_this->m2BD = i_this->m2BD + 1;
        [[fallthrough]];
    case Mode_19_e:
        if (i_this->mAcchCir.ChkWallHit() || kn_dist(fVar8, fVar1) > f29) {
            // HD: the USA version stops all motion before burrowing.
            i_this->speedF = 0.0f;
            i_this->m2E4 = 0.0f;
            i_this->speed.x = 0.0f;
            i_this->speed.y = 0.0f;
            i_this->speed.z = 0.0f;
            i_this->m300.copy(i_this->current.pos);
            i_this->m2BF = 1;
            smoke_set(i_this);
            i_this->m2BD = Mode_15_e;
        } else if (i_this->m2BE != 0) {
            i_this->m2BF = 0;
            shibuki_set(i_this);
            i_this->m2BD = Mode_15_e;
        }
        break;
    }

    u8 mode = i_this->m2BD;
    if (mode >= Mode_13_e) {
        if (mode < Mode_18_e) {
            i_this->m2E0 = 1.0f;
            if ((s16)cLib_distanceAngleS(i_this->current.angle.y, i_this->m2D0) > 0x100) {
                i_this->m2E0 = 2.0f;
            }
            cLib_addCalc2(&i_this->m2DC, i_this->m2E0, 0.5f, 1.0f);
            i_this->mpMorf->setPlaySpeed(i_this->m2DC);
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m2D0, 1, i_this->m2D2);
        cLib_addCalc2(&i_this->speedF, i_this->m2E4, 0.3f, 1.0f);
    }
    i_this->shape_angle.y = i_this->current.angle.y + 0x4000;
}

/* inlined into daKN_Execute */
static void BG_check(kn_class* i_this) {
    f32 h = i_this->m2CC;
    i_this->current.pos.y = i_this->current.pos.y - h;
    i_this->old.pos.y = i_this->old.pos.y - h;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    h = i_this->m2CC;
    i_this->current.pos.y = i_this->current.pos.y + h;
    i_this->old.pos.y = i_this->old.pos.y + h;
}

/* 021A23B8 */
static BOOL daKN_Execute(kn_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    fopAc_ac_c* a_this = i_this;
    for (s32 i = 0; i < 4; i++) {
        if (i_this->m2C2[i] != 0) {
            i_this->m2C2[i] = i_this->m2C2[i] - 1;
        }
    }
    kn_move(i_this);

    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), a_this->current.angle.x);
    gabi::Local<cXyz> sp14;
    sp14->x = 0.0f;
    sp14->y = 0.0f;
    sp14->z = a_this->speedF;
    gabi::Local<cXyz> sp08;
    MtxPosition(sp14.get(), sp08.get());
    a_this->speed.x = sp08->x;
    a_this->speed.z = sp08->z;
    f32 vy = a_this->speed.y + a_this->gravity;
    if (vy < -20.0f) {
        vy = -20.0f;
    }
    a_this->speed.y = vy;
    fopAcM_posMove(a_this, nullptr);
    BG_check(i_this);
    if (i_this->mAcch.ChkGroundHit()) {
        fopAcM_getGroundAngle(a_this, &i_this->m2F8);
        if (((i_this->m2BD == Mode_13_e) || (i_this->m2BD == Mode_19_e)) &&
            i_this->mAcch.GetGroundH() != -1000000000.0f &&
            knGetAttributeCode(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd)) ==
                dBgS_Attr_WATER_e) {
            i_this->m2BE = 1;
        }
    }
    i_this->mpMorf->play(nullptr, 0, 0);
    J3DModel* pJVar3 = i_this->mpMorf->getModel();
    mDoMtx_stack_c::transS(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m2F8.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->m2F8.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), a_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), a_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), a_this->shape_angle.z);
    J3DModel_setBaseScale(pJVar3, &a_this->scale);
    mDoMtx_stack_transM(0.0f, i_this->m2CC, 0.0f);
    J3DModel_setBaseTRMtx(pJVar3, mDoMtx_stack_c::get());
    return TRUE;
}

/* 021A2F78 */
static BOOL daKN_IsDelete(kn_class*) { return TRUE; }

/* 021A2F80 */
static BOOL daKN_Delete(kn_class* i_this) {
    /* m314.remove(): virtual (vtable +0x44) */
    i_this->m314.remove();
    dComIfG_resDelete(&i_this->mPhase, "Kn"); /* resource deletion */
    return TRUE;
}

/* 021A2FD0 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    kn_class* i_this = (kn_class*)a_this;
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes("Kn", dRes_INDEX_KN_BDL_KN_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Kn", dRes_INDEX_KN_BCK_PATA_e);
    mDoExt_McaMorf* morf =
        mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f,
                               0, -1, 1, nullptr, 0x80000, 0x11000002);
    i_this->mpMorf = morf;
    if ((morf == nullptr) || (morf->getModel() == nullptr)) {
        return FALSE;
    }
    return TRUE;
}

/* 021A30C0 */
static cPhs_State daKN_Create(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    kn_class* i_this = (kn_class*)a_this;
    /* fopAcM_ct(a_this, kn_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = KN_VTBL;
            __ct(&i_this->m314, 1);
            __ct(&i_this->mAcchCir);
            __ct(&i_this->mAcch);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, "Kn");
    if (PVar1 == cPhs_COMPLEATE_e) {
        i_this->m2B4 = fopAcM_GetParam(a_this);
        i_this->m2B5 = fopAcM_GetParam(a_this) >> 8;
        i_this->m2EC.copy(a_this->current.pos);
        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0x2860)) {
            return cPhs_ERROR_e;
        }
        a_this->cullMtx =
            gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(a_this, -30.0f, -0.0f, -30.0f, 30.0f, 60.0f, 30.0f);
        a_this->gravity = -5.0f;
        a_this->attention_info.flags = 0;
        i_this->mAcchCir.SetWall(10.0f, 30.0f);
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                          &a_this->speed);
        a_this->scale.x = 1.5f;
        a_this->scale.y = 1.5f;
        a_this->scale.z = 1.5f;
        if (cM_rnd() < 0.5f) {
            a_this->scale.x = 2.5f;
            a_this->scale.y = 2.5f;
            a_this->scale.z = 2.5f;
        }
        i_this->m2BD = Mode_10_e;
        daKN_Execute(i_this);
    }
    return PVar1;
}

// HD: compiler-generated and template code of this translation unit.
/* 021A3300 */
static void __sinit_d_a_kn_cpp() { __sinit_header_statics(); }

/* 021A3394 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021A33A8 */
static void kn_class_dt(kn_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcch);
        __dt(&i_this->mAcchCir);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021A342C */
static void SafeString_assureTerminationImpl(void* p) {}

// clang-format off
/* WWHD glue: HD archive capitalization differs from the GameCube "KN".
str 10013258 "Kn" in 021A2038
str 100132C0 "Kn" in 021A2F80
str 100132C3 "Kn" in 021A2FD0
str 100132E0 "Kn" in 021A30C0
ext 024EF0F4 s32 knGetAttributeCode(dBgS*, cBgS_PolyInfo*)
*/
