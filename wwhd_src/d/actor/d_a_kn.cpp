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
#include "bindings.h"

#define SAFESTRING_VTBL 0x100131FC
#define KN_VTBL 0x10013244 /* HD: kn_class vtable */
static const dBgS_ObjAcch_vt KN_OBJACCH_VT = {0x10013214, 0x10013234, 0x10013224};

enum { /* HD archive order */
    dRes_INDEX_KN_BCK_PATA_e = 4,
    dRes_INDEX_KN_BCK_WAIT01_e = 5,
    dRes_INDEX_KN_BCK_WAIT02_e = 6,
    dRes_INDEX_KN_BCK_WALK_e = 7,
    dRes_INDEX_KN_BDL_KN_e = 0xA,
};

enum Mode {
    Mode_10_e = 0,
    Mode_11_e,
    Mode_12_e,
    Mode_13_e,
    Mode_14_e,
    Mode_15_e,
    Mode_16_e,
    Mode_17_e,
    Mode_18_e,
    Mode_19_e,
    Mode_20_e,
};

struct kn_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 m2B6[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ be<u8> m2BD;
    /* 0x3DA */ be<u8> m2BE;
    /* 0x3DB */ be<u8> m2BF;
    /* 0x3DC */ be<u8> m2C0;
    /* 0x3DD */ u8 m2C1;
    /* 0x3DE */ be<s16> m2C2[4];
    /* 0x3E6 */ be<s16> m2CA;
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<s16> m2D0;
    /* 0x3EE */ be<s16> m2D2;
    /* 0x3F0 */ be<s16> m2D4;
    /* 0x3F2 */ u8 m2D6[6];
    /* 0x3F8 */ be<f32> m2DC;
    /* 0x3FC */ be<f32> m2E0;
    /* 0x400 */ be<f32> m2E4;
    /* 0x404 */ be<f32> m2E8;
    /* 0x408 */ cXyz m2EC;
    /* 0x414 */ csXyz m2F8;
    /* 0x41A */ u8 m2FE[2];
    /* 0x41C */ cXyz m300;
    /* 0x428 */ u8 m30C[8];
    /* 0x430 */ u8 m314[0x20]; /* dPa_smokeEcallBack (HD: vtable at +0) */
    /* 0x450 */ gptr<JPABaseEmitter> m334;
    /* 0x454 */ dBgS_AcchCir mAcchCir;
    /* 0x494 */ dBgS_ObjAcch mAcch;
};
WWHD_OFFSET(kn_class, mpMorf, 0x3D4);
WWHD_OFFSET(kn_class, m2CC, 0x3E8);
WWHD_OFFSET(kn_class, m300, 0x41C);
WWHD_OFFSET(kn_class, mAcch, 0x494);
WWHD_SIZE(kn_class, 0x658);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E54D8 mDoExt_McaMorf::updateDL is in m_Do_ext.h; 025BED80 dSnap_RegistFig in bindings.h */
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0F4, bgs, poly); }
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }
/* JPABaseEmitter setters (HD offsets) */
static inline void JPA_setMaxFrame(u32 e, s32 v) { gabi::store<s32>(e + 0x5C, v); }
static inline void JPA_setLifeTime(u32 e, s16 v) { gabi::store<s16>(e + 0x60, v); }
static inline void JPA_setRate(u32 e, f32 v) { gabi::store<f32>(e + 0x34, v); }
static inline void JPA_setSpread(u32 e, f32 v) { gabi::store<f32>(e + 0x58, v); }
static inline void JPA_setAwayFromCenterSpeed(u32 e, f32 v) { gabi::store<f32>(e + 0x68, v); }
static inline void JPA_setAwayFromAxisSpeed(u32 e, f32 v) { gabi::store<f32>(e + 0x6C, v); }
static inline void JPA_setDirectionalSpeed(u32 e, f32 v) { gabi::store<f32>(e + 0x70, v); }
static inline void JPA_setGlobalScale(u32 e, f32 x, f32 y, f32 z) {
    gabi::store<f32>(e + 0x220, x);
    gabi::store<f32>(e + 0x224, y);
    gabi::store<f32>(e + 0x228, z);
    gabi::store<f32>(e + 0x238, x);
    gabi::store<f32>(e + 0x23C, y);
    gabi::store<f32>(e + 0x240, z);
}

/* 021A1FA0 */
static BOOL daKN_Draw(kn_class* i_this) {
    WWHD_FUNC(0x021A1FA0, BOOL, i_this);
    fopAc_ac_c* a_this = i_this;
    if (i_this->m2CC < 0.0f) {
        return TRUE;
    }
    J3DModel* pJVar1 = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), pJVar1, &a_this->tevStr);
    i_this->mpMorf->updateDL();
    dSnap_RegistFig(0x56 /* DSNAP_TYPE_KN */, a_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021A1FA0, daKN_Draw);

/* 021A2038 */
void anm_init(kn_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float speed, int soundResIdx) {
    WWHD_FUNC(0x021A2038, void, i_this, anmResIdx, morf, loopMode, speed, soundResIdx);
    if (soundResIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10013258) /* "Kn" */, anmResIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR(0x10013258), soundResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10013258), anmResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x021A2038, anm_init);

/* 021A2160 */
void smoke_set(kn_class* i_this) {
    WWHD_FUNC(0x021A2160, void, i_this);
    f32 fVar1 = REG_F(8, 10) + 1.0f;
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)i_this->m314); /* m314.remove() */
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    /* dComIfGp_particle_setToon: group 2 */
    dPa_control_c* pa = dComIfGp_getParticle();
    i_this->m334 = dPa_control_set(pa, 2, 0x2027 /* ID_AK_JT_ELEMENTSMOKE01 */, &i_this->m300, &i_this->shape_angle, nullptr,
                                   0xB9, (dPa_levelEcallBack*)i_this->m314, roomNo, nullptr, nullptr, nullptr);
    if (i_this->m334 != nullptr) {
        JPA_setMaxFrame(gabi::ea(i_this->m334.get()), 0x19);
        JPA_setAwayFromCenterSpeed(gabi::ea(i_this->m334.get()), 0.0f);
        JPA_setAwayFromAxisSpeed(gabi::ea(i_this->m334.get()), 5.0f);
        JPA_setRate(gabi::ea(i_this->m334.get()), 3.0f);
        JPA_setDirectionalSpeed(gabi::ea(i_this->m334.get()), 2.0f);
        JPA_setGlobalScale(gabi::ea(i_this->m334.get()), fVar1, fVar1, fVar1);
        JPA_setLifeTime(gabi::ea(i_this->m334.get()), 25);
    }
}
VERIFY(0x021A2160, smoke_set);

/* 021A22AC */
void shibuki_set(kn_class* i_this) {
    WWHD_FUNC(0x021A22AC, void, i_this);
    f32 fVar1 = REG_F(8, 11) + 0.55f;
    JPABaseEmitter* pJVar2 = dComIfGp_particle_set(0x23 /* ID_AK_JN_ELEMENTSHIBUKI00 */, &i_this->current.pos);
    if (pJVar2 != nullptr) {
        u32 e = gabi::ea(pJVar2);
        JPA_setRate(e, 1.5f);
        JPA_setSpread(e, 1.0f);
        JPA_setDirectionalSpeed(e, 4.0f);
        JPA_setMaxFrame(e, 0x19);
        JPA_setGlobalScale(e, fVar1, fVar1, fVar1);
    }
}
VERIFY(0x021A22AC, shibuki_set);

static inline f32 kn_dist(f32 x, f32 z) { return std_sqrtf(gabi::fmadds(x, x, z * z)); }

/* kn_move, inlined in daKN_Execute */
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
                        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)i_this->m314); /* m314.remove() */
                    }
                    i_this->m2CC = fVar9;
                    i_this->m2C0 = 1;
                    i_this->m2C2[3] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
                } else {
                    cLib_addCalc2(&i_this->m2CC, fVar9, 0.1f, 1.0f);
                }
            }
            if ((i_this->m2C0 != 0) && (i_this->m2C2[3] == 0) && fopAcM_searchPlayerDistance(i_this) > f27) {
                if (i_this->m2BF != 0) {
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
                dPa_smokeEcallBack_end((dPa_smokeEcallBack*)i_this->m314); /* m314.remove() */
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
            /* USA/HD: stop */
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

/* BG_check, inlined */
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
    WWHD_FUNC(0x021A23B8, BOOL, i_this);
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
        if (((i_this->m2BD == Mode_13_e) || (i_this->m2BD == Mode_19_e)) && i_this->mAcch.GetGroundH() != -1000000000.0f &&
            dBgS_GetAttributeCode(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(&i_this->mAcch) + 0xD4 + 0x14)) == 0x13 /* dBgS_Attr_WATER_e */)
        {
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
VERIFY(0x021A23B8, daKN_Execute);

/* 021A2F78 */
static BOOL daKN_IsDelete(kn_class*) {
    WWHD_FUNC(0x021A2F78, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021A2F78, daKN_IsDelete);

/* 021A2F80 */
static BOOL daKN_Delete(kn_class* i_this) {
    WWHD_FUNC(0x021A2F80, BOOL, i_this);
    /* m314.remove(): virtual (vtable +0x44) */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this->m314)) + 0x44), i_this->m314);
    dComIfG_resDelete(&i_this->mPhase, STR(0x100132C0) /* "Kn" */); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x021A2F80, daKN_Delete);

/* 021A2FD0 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021A2FD0, BOOL, a_this);
    kn_class* i_this = (kn_class*)a_this;
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x100132C3) /* "Kn" */, dRes_INDEX_KN_BDL_KN_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100132C3), dRes_INDEX_KN_BCK_PATA_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                  nullptr, 0x80000, 0x11000002);
    i_this->mpMorf = morf;
    if ((morf == nullptr) || (morf->getModel() == nullptr)) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x021A2FD0, useHeapInit);

/* 021A30C0 */
static cPhs_State daKN_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021A30C0, cPhs_State, a_this);
    kn_class* i_this = (kn_class*)a_this;
    /* fopAcM_ct(a_this, kn_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = KN_VTBL;
            dPa_smokeEcallBack_ct(i_this->m314, 1);
            dBgS_AcchCir_ct(&i_this->mAcchCir);
            dBgS_ObjAcch_ct(&i_this->mAcch, KN_OBJACCH_VT);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, STR(0x100132E0) /* "Kn" */);
    if (PVar1 == cPhs_COMPLEATE_e) {
        i_this->m2B4 = fopAcM_GetParam(a_this);
        i_this->m2B5 = fopAcM_GetParam(a_this) >> 8;
        i_this->m2EC.copy(a_this->current.pos);
        if (!fopAcM_entrySolidHeap(a_this, 0x021A2FD0 /* useHeapInit */, 0x2860)) {
            return cPhs_ERROR_e;
        }
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(a_this, -30.0f, -0.0f, -30.0f, 30.0f, 60.0f, 30.0f);
        a_this->gravity = -5.0f;
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0); /* attention_info.flags */
        i_this->mAcchCir.SetWall(10.0f, 30.0f);
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
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
VERIFY(0x021A30C0, daKN_Create);

/* 021A3300 */
static void __sinit_d_a_kn_cpp() {
    WWHD_FUNC(0x021A3300, void, (u32)0);
    sinit_header_statics(0x10464C04, 0x101B85B4);
}
VERIFY(0x021A3300, __sinit_d_a_kn_cpp);

/* 021A3394: sead::SafeString deleting destructor (this TU's SafeString vtable) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021A3394, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021A3394, SafeString_dt);

/* 021A33A8: kn_class deleting destructor (vtable +0xC) */
static void kn_class_dt(kn_class* i_this, s32 flags) {
    WWHD_FUNC(0x021A33A8, void, i_this, flags);
    if (i_this != nullptr) {
        u32 acch = gabi::ea(&i_this->mAcch); /* ~dBgS_ObjAcch (inline) */
        gabi::store<u32>(acch + 0x20, KN_OBJACCH_VT.v20);
        gabi::store<u32>(acch + 0x14, KN_OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);                          /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2);      /* cM3dGCir::~cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0);                                  /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021A33A8, kn_class_dt);

/* 021A342C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void* p) { WWHD_FUNC(0x021A342C, void, p); }
VERIFY(0x021A342C, SafeString_assureTerminationImpl);
