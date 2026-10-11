/**
 * d_a_bdkobj.cpp (WWHD)
 * Object - Helmaroc King fight objects (blocks, poles and the tower bridge, with fragments).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdkobj.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdkobj.h"

const gc_tu_data gc_tu = {.safeString = 0x1000835C};

#define BDKOBJ_VTBL 0x100084A4   /* bdkobj_class vtable (HD virtual destructor) */
#define CM3DGAAB_VTBL 0x10008374 /* this TU's cM3dGAab vtable */
// HD: original mapped tables include all u8-indexed entries, including indices beyond the GC models.
struct BdkObjModelTable {
    be<u16> entries[256];
};
static BdkObjModelTable bdl_data;
static BdkObjModelTable hahen_bdl_data;
static dCcD_SrcCyl cc_cyl_src;
static dCcD_SrcSph hahen_sph_src;
static cXyz non_pos;
struct BdkObjStringView {
    char mText[1];
};
static BdkObjStringView l_deleteArcName;
static BdkObjStringView l_heapArcName;
static BdkObjStringView l_assertFile;
static BdkObjStringView l_assertExpression;
static BdkObjStringView l_createArcName;

using BdkObjGuard = be<u32>;
static BdkObjGuard l_scaleGuard;
static BdkObjGuard l_angleGuard;
static cXyz l_fragmentScale;
static csXyz l_effAngle;
struct BdkObjHeaderStatics {
    u8 first;
    u8 second;
    u8 padding[2];
    be<u32> zero[4];
};
struct BdkObjPiPair {
    be<f32> negativePi;
    be<f32> positivePi;
};
struct BdkObjRegistration {
    u8 opaque;
};
static BdkObjHeaderStatics l_headerStatics;
static BdkObjPiPair l_pi;
static BdkObjRegistration l_piRegistration;
static BdkObjRegistration l_firstRegistration;
static BdkObjRegistration l_secondRegistration;

static inline BdkObjEmitterView *BdkObj_getEmitter(bdkobj_class *i_this) {
    return reinterpret_cast<BdkObjEmitterView *>(i_this->m918.getEmitter());
}
static inline J3DModel *actor_model(fopAc_ac_c *a) { return gabi::at<J3DModel>(a->model); }

static bdo_eff_s *bdo_eff_s_ct(bdo_eff_s *);
static void bdo_eff_s_dt(bdo_eff_s *, s32);

/* 02072888 */
static void ride_call_back(dBgW *param1, fopAc_ac_c *param2, fopAc_ac_c *param3) {
    if (param3 == nullptr || fpcM_GetName(param3) != fpcNm_PLAYER_e) { /* HD: fopAcM_GetName null check */
        return;
    }
    param2->health = 0xA;
}

// (hahen_draw inlined)
/* 020728A8 */
static BOOL daBdkobj_Draw(bdkobj_class *i_this) {
    fopAc_ac_c *a_this = i_this;
    if (a_this->model != 0) {
        J3DModel *model = actor_model(a_this);
        setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
        if (i_this->m298 < 2) {
            auto camera = dComIfGp_getCamera(0);
            gabi::Local<cXyz> vec;
            cXyz_mi(&a_this->current.pos, vec,
                    gabi::at<cXyz>(gabi::ea(camera) + offsetof(BdkObjCameraView, mEye)));
            f32 d = std_sqrtf(PSVECSquareMag(vec));
            if (d > REG_F(8, 11) + 300.0f) {
                mDoExt_modelUpdateDL(model);
            }
        } else {
            mDoExt_modelUpdateDL(model);
        }
    }
    for (s32 i = 0; i < 3; i++) {
        bdo_eff_s *fragment = &i_this->mEffs[i];
        if (fragment->m000 != 0) {
            J3DModel *model = fragment->mpModel;
            if (model != nullptr) {
                setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
                mDoExt_modelUpdateDL(model);
            }
        }
    }
    return TRUE;
}

/* rotation and scale matrix of a fragment, into its model */
static inline void eff_set_mtx(bdo_eff_s *i_eff, f32 y) {
    MtxTrans(i_eff->m008.x, y, i_eff->m008.z, 0);
    cMtx_YrotM(calc_mtx(), i_eff->m030.y);
    cMtx_XrotM(calc_mtx(), i_eff->m030.x);
    cMtx_ZrotM(calc_mtx(), i_eff->m030.z);
    f32 scale = i_eff->m028;
    MtxScale(scale, scale, scale, 1);
    J3DModel_setBaseTRMtx(i_eff->mpModel, calc_mtx());
}

/* inlined into daBdkobj_Execute */
static inline void top_hahen_move(bdkobj_class *i_this, bdo_eff_s *i_eff, dBgS_GndChk *gnd,
                                  dBgS_LinChk *lin) {
    gabi::Local<cXyz> local_14c;
    gabi::Local<cXyz> cStack344;

    BdkObj_groundCtor(gnd);
    BdkObj_lineCtor(lin);

    f32 dVar8 = i_eff->m1A4;
    i_eff->m014.copy(i_eff->m008);
    if (dVar8 > 0.1f) {
        f32 dVar9 = 20.0f * (1.0f - i_eff->m028);
        Mtx34 *m = calc_mtx();
        s16 a = i_eff->m1A8;
        if (dVar8 > dVar9) {
            dVar8 = dVar9;
        }
        cMtx_YrotS(m, a);
        local_14c->x = 0.0f;
        local_14c->y = 0.0f;
        local_14c->z = dVar8;
        MtxPosition(local_14c, cStack344);
        PSVECAdd(&i_eff->m008, cStack344, &i_eff->m008);
        i_eff->m030.y = i_eff->m030.y + i_eff->m1AA;
    }
    cLib_addCalc0(&i_eff->m1A4, 1.0f, REG_F(0, 18) + 0.2f);
    cMtx_YrotS(calc_mtx(), i_eff->m030.y);

    local_14c->x = 0.0f;
    local_14c->y = i_eff->m024;
    local_14c->z = i_eff->m020;
    MtxPosition(local_14c, cStack344);
    PSVECAdd(&i_eff->m008, cStack344, &i_eff->m008);
    i_eff->m024 = i_eff->m024 - 5.0f;

    /* GetCCMoveP(): &mStts, never NULL */
    i_eff->m008.x = i_eff->m008.x + i_eff->mStts.m_cc_move.x;
    i_eff->m008.z = i_eff->m008.z + i_eff->mStts.m_cc_move.z;

    {
        f32 x = i_eff->m008.x;
        f32 z = i_eff->m008.z;
        f32 y = i_eff->m008.y + 200.0f;
        BdkObj_groundSetPos(gnd, x, y, z);
    }

    f32 ground = dComIfG_Bgsp()->GroundCross(gnd);
    f32 fVar10 = gabi::fmadds(REG_F(0, 3) + 50.0f, i_eff->m028, ground);
    if (!(i_eff->m008.y > fVar10)) {
        i_eff->m008.y = fVar10;
        if (i_eff->m024 < REG_F(8, 9) + -20.0f) {
            i_eff->m024 = -(i_eff->m024 * (REG_F(8, 10) + 0.3f));
            s16 r = (s16)gabi::ftoi(cM_rndFX(5000.0f)); /* HD: 5000 (GameCube 8000) */
            i_eff->m030.y = i_eff->m030.y + r;
            i_eff->m02C = cM_rndFX(100.0f); /* HD: 100 (GameCube 200) */
        } else {
            i_eff->m024 = 0.0f;
            cLib_addCalc0(&i_eff->m020, 1.0f, REG_F(10, 7) + 2.0f); /* HD: 2.0 (GameCube 0.75) */
            i_eff->m036.z = 0;
            f32 v = i_eff->m020;
            s16 sx = (s16)gabi::ftoi(v * (REG_F(10, 8) + 300.0f));
            s16 sy = (s16)gabi::ftoi(v * i_eff->m02C);
            i_eff->m036.x = sx;
            i_eff->m036.y = sy;
        }
    }
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_eff->m008, d, &i_eff->m014);
        BdkObj_word(local_14c->x) = BdkObj_word(d->x);
        local_14c->y = 0.0f;
        BdkObj_word(local_14c->z) = BdkObj_word(d->z);
    }

    if (std_sqrtf(PSVECSquareMag(local_14c)) > 0.0f) {
        Mtx34 *m = calc_mtx();
        s16 a = cM_atan2s(local_14c->x, local_14c->z);
        cMtx_YrotS(m, a);
        local_14c->x = 0.0f;
        local_14c->y = 30.0f;
        local_14c->z = 70.0f * i_eff->m028;
        MtxPosition(local_14c, cStack344);
        local_14c->x = i_eff->m008.x;
        f32 y = i_eff->m008.y;
        local_14c->y = y;
        local_14c->z = i_eff->m008.z;
        local_14c->y = y + 30.0f;
        PSVECAdd(cStack344, &i_eff->m008, cStack344);
        lin->Set(local_14c, cStack344, i_this);
        if (dComIfG_Bgsp()->LineCross(lin)) {
            i_eff->m008.x = i_eff->m014.x;
            i_eff->m008.z = i_eff->m014.z;
            i_eff->m020 = 0.0f;
        }
    }
    csXyz_apl(&i_eff->m030, &i_eff->m036);
    eff_set_mtx(i_eff, i_eff->m008.y);

    i_eff->mSph.SetC(&i_eff->m008);
    i_eff->mSph.SetR((REG_F(8, 15) + 85.0f) * i_eff->m028);
    dComIfG_Ccsp_Set(&i_eff->mSph);
    if (i_eff->mSph.ChkTgHit() && i_eff->m1A4 < 1.0f) {
        gabi::Local<BdkObjAttackInfo> hit_atInfo;
        hit_atInfo->mpObj = static_cast<dCcD_GObjInf *>(i_eff->mSph.GetTgHitObj());
        hit_atInfo->mpActor = BdkObj_powerCheck(hit_atInfo);
        if (hit_atInfo->mResultingAttackType == 0x8) {
            f32 k = REG_F(14, 4) + 15.0f;
            f32 s = 1.0f - i_eff->m028;
            i_eff->m1A4 = s * (k + cM_rndF(1.0f));

            auto at = hit_atInfo->mpActor.get();
            if (at != nullptr) {
                s16 r = (s16)gabi::ftoi(cM_rndFX(2000.0f));
                i_eff->m1A8 = at->shape_angle.y + r;
            }
            i_eff->m1AA = (s16)gabi::ftoi(cM_rndFX(400.0f));
        } else if (hit_atInfo->mResultingAttackType == 0x9) {
            /* HD: a type-9 hit breaks the fragment and drops an item (with a sound and an effect) */
            i_eff->m000 = 0;
            fopAcM_createItem(&i_eff->m008, 0, -1, -1, 0, nullptr, 4, nullptr);
            mDoAud_seStart(0x692B, &i_eff->m008, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            /* static cXyz scale(3.0f, 3.0f, 3.0f) */
            auto &guard = l_scaleGuard;
            cXyz *scale = &l_fragmentScale;
            if (guard == 0) {
                guard = 1;
                scale->set(3.0f, 3.0f, 3.0f);
            }
            s8 room = fopAcM_GetRoomNo(i_this);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x3E3, &i_eff->m008, nullptr, scale, 0xFF, nullptr,
                            room, &i_this->tevStr.mColorK0, &i_this->tevStr.mColorK0, nullptr);
        }
    }

    if (i_eff->m008.y < 8500.0f) {
        i_eff->m000 = 0;
    }
    BdkObj_destroyChecks(lin, gnd);
}

// HD: the stair fragments also move horizontally (with
// a wall check) and rock on the ground with sizes depending on their scale
/* inlined into daBdkobj_Execute */
static inline void kaidan_hahen_move(bdkobj_class *i_this, bdo_eff_s *i_eff, dBgS_GndChk *gnd,
                                     dBgS_LinChk *lin) {
    gabi::Local<cXyz> local_78;
    gabi::Local<cXyz> out;

    BdkObj_specialGroundCtor(gnd);
    csXyz_apl(&i_eff->m030, &i_eff->m036);

    {
        f32 vy = i_eff->m024;
        f32 y = i_eff->m008.y + vy;
        f32 x = i_eff->m008.x + i_eff->mHorizontalSpeedX;
        u32 ox = BdkObj_word(i_eff->m008.x);
        i_eff->m008.y = y;
        u32 ny = BdkObj_word(i_eff->m008.y);
        i_eff->m024 = vy - 3.0f;
        f32 z = i_eff->m008.z + i_eff->mHorizontalSpeedZ;
        i_eff->m008.x = x;
        u32 oz = BdkObj_word(i_eff->m008.z);
        i_eff->m008.z = z;
        BdkObj_word(i_eff->m014.x) = ox;
        BdkObj_word(i_eff->m014.y) = ny;
        BdkObj_word(i_eff->m014.z) = oz;
        f32 vx = i_eff->mHorizontalSpeedX;
        i_eff->mHorizontalSpeedX = vx * (REG_F(18, 3) + 0.985f);
        f32 vz = i_eff->mHorizontalSpeedZ;
        i_eff->mHorizontalSpeedZ = vz * (REG_F(18, 3) + 0.985f);
    }

    BdkObj_lineCtor(lin);
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_eff->m008, d, &i_eff->m014);
        BdkObj_word(local_78->x) = BdkObj_word(d->x);
        local_78->y = 0.0f;
        BdkObj_word(local_78->z) = BdkObj_word(d->z);
    }
    if (std_sqrtf(PSVECSquareMag(local_78)) > 0.0f) {
        Mtx34 *m = calc_mtx();
        s16 a = cM_atan2s(local_78->x, local_78->z);
        cMtx_YrotS(m, a);
        local_78->x = 0.0f;
        local_78->y = 30.0f;
        local_78->z = 50.0f;
        MtxPosition(local_78, out);
        local_78->x = i_eff->m008.x;
        f32 y = i_eff->m008.y;
        local_78->y = y;
        local_78->z = i_eff->m008.z;
        local_78->y = y + 30.0f;
        PSVECAdd(out, &i_eff->m008, out);
        lin->Set(local_78, out, i_this);
        if (dComIfG_Bgsp()->LineCross(lin)) {
            f32 x = i_eff->m014.x;
            f32 z = i_eff->m014.z;
            i_eff->m008.x = x;
            i_eff->m008.z = z;
        }
    }
    BdkObj_groundSetPos(gnd, i_eff->m008.x, i_eff->m008.y + 5000.0f, i_eff->m008.z);

    f32 fVal1 = dComIfG_Bgsp()->GroundCross(gnd);
    f32 fVal2 = 0.0f;
    if (fVal1 != -1000000000.0f) {
        if (!(i_eff->m008.y > fVal1)) {
            i_eff->m008.y = fVal1;
            i_eff->m024 = 0.0f;
            cLib_addCalcAngleS2(&i_eff->m036.y, REG_S(18, 0), 1, 10); /* HD: a debug-register target */
            cLib_addCalcAngleS2(&i_eff->m036.z, 0, 1, 10);
            f32 k = (i_eff->m028 - 0.7f) * 2000.0f;
            s16 s1 = (s16)gabi::ftoi(1500.0f - k);
            f32 sin1 = cM_ssin((u16)(i_eff->m1AC * s1));
            s16 s2 = (s16)gabi::ftoi(1800.0f - k);
            s16 target = (s16)gabi::ftoi(sin1 * (REG_F(10, 11) + 800.0f));
            cLib_addCalcAngleS2(&i_eff->m030.x, target, 10, 400);
            f32 sin2 = cM_ssin((u16)(i_eff->m1AC * s2));
            fVal2 = (sin2 * (REG_F(10, 10) + 5.0f)) * i_eff->m028;
        }
    } else {
        i_eff->m000 = 0;
    }

    eff_set_mtx(i_eff, i_eff->m008.y + fVal2);
    BdkObj_destroyChecks(lin, gnd);
}

/* inlined into daBdkobj_Execute */
static inline void tower_kaidan_move(bdkobj_class *i_this) {
    if (i_this->pm_bgw != nullptr) {
        s8 h = i_this->health;
        u32 prm = fopAcM_GetParam(i_this);
        if (h != 0) {
            i_this->health = h - 1;
        }
        if ((prm & 0xf) == 0xf) {
            cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
            i_this->pm_bgw = nullptr;
            i_this->model = 0;
            i_this->m299 = 100;

            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_L00, &i_this->current.pos, &i_this->current.angle);
            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_S00, &i_this->current.pos, &i_this->current.angle);
            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_TENITA00, &i_this->current.pos, &i_this->current.angle);
            i_this->m918.end(); /* m918.remove() */
            /* m918.setColor({0x46, 0x3C, 0x28, 0xB4}) */
            reinterpret_cast<be<u32> &>(i_this->m918.mColor) = 0x463C28B4;
            s8 room = fopAcM_GetRoomNo(i_this);
            /* dComIfGp_particle_setToon: the toon group (2) */
            dPa_control_set(dComIfGp_getParticle(), 2, ID_IT_ST_DK_BRIDGE_SMOKE00, &i_this->current.pos,
                            &i_this->current.angle, nullptr, 0xB4,
                            (dPa_levelEcallBack *)(void *)&i_this->m918, room, nullptr, nullptr, nullptr);
            auto emitter = BdkObj_getEmitter(i_this);
            if (emitter != nullptr) {
                /* becomeImmortalEmitter */
                emitter->mStatus = emitter->mStatus | 0x40;
                i_this->m938 = 0xB4;
            }

            /* fopAcM_seStartCurrent (HD inline, null-checked) */
            if (gabi::ea(&i_this->current.pos) != 0)
                mDoAud_seStart(JA_SE_OBJ_MJ_WBOARD_BRK, &i_this->current.pos, 0,
                               dComIfGp_getReverb(i_this->current.roomNo));
            for (s32 i = 0; i < 2; i++) {
                bdo_eff_s *e = &i_this->mEffs[i];
                e->m000 = 2;
                e->m1AC = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m028 = cM_rndFX(0.25f) + 0.7f;
                e->m008.x = i_this->current.pos.x + cM_rndFX(500.0f);
                e->m008.y = i_this->current.pos.y;
                e->m008.z = i_this->current.pos.z + cM_rndFX(500.0f);
                e->m030.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m030.z = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m036.y = (s16)gabi::ftoi(cM_rndFX(800.0f));
                e->m030.x = (s16)gabi::ftoi(cM_rndFX(10000.0f));
                e->m036.z = (s16)gabi::ftoi(cM_rndFX(800.0f));
            }
            i_this->actor_status = i_this->actor_status & ~0x100u; /* fopAcM_OffStatus(fopAcStts_CULL_e) */
        }
    } else {
        auto emitter = BdkObj_getEmitter(i_this);
        if (emitter != nullptr) {
            u8 a = i_this->m938;
            if (i_this->m299 <= 0x5A && a != 0) {
                a = a - 2;
                emitter = BdkObj_getEmitter(i_this);
                i_this->m938 = a;
            }
            emitter->mGlobalAlpha = a; /* setGlobalAlpha */
        }
    }
}

static inline void set_actor_mtx(bdkobj_class *i_this) {
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    Mtx34 *m = calc_mtx();
    J3DModel_setBaseTRMtx(actor_model(i_this), m);
}

/* 020729A8 */
static BOOL daBdkobj_Execute(bdkobj_class *i_this) {
    if (i_this->m298 == 2) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
        tower_kaidan_move(i_this);
    } else {
        settingTevStruct(dKy_getEnvlight(), 4 /* TEV_TYPE_BG3 */, &i_this->current.pos, &i_this->tevStr);
    }

    if (i_this->m299 != 0) {
        i_this->m299 = i_this->m299 - 1;
    }

    if (i_this->m298 == 2) {
        if (i_this->model != 0) {
            set_actor_mtx(i_this);
            if (i_this->pm_bgw != nullptr) {
                PSMTXCopy(calc_mtx(), &i_this->mMtx);
                dBgW_Move(i_this->pm_bgw);
            }
        }
    } else {
        if (i_this->model != 0) {
            i_this->mCyl.SetC(&i_this->current.pos);
            if (i_this->m298 == 0) {
                i_this->mCyl.SetH(REG_F(6, 0) + 100.0f);
                i_this->mCyl.SetR(REG_F(6, 1) + 100.0f);
            } else if (i_this->m298 == 1) {
                i_this->mCyl.SetH(REG_F(6, 2) + 900.0f);
                i_this->mCyl.SetR(REG_F(6, 3) + 100.0f);
            }

            if (i_this->mCyl.ChkTgHit()) {
                gabi::Local<BdkObjAttackInfo> hit_atInfo;
                hit_atInfo->mpObj = static_cast<dCcD_GObjInf *>(i_this->mCyl.GetTgHitObj());
                auto at = BdkObj_powerCheck(hit_atInfo);
                if (at != nullptr && hit_atInfo->mResultingAttackType == 0xB) {
                    i_this->eyePos.copy(i_this->current.pos);

                    u16 uVar8, uVar6, uVar4;
                    if (i_this->m298 == 0) {
                        uVar8 = ID_IT_ST_DK_BLOCK_SMOKE00;
                        uVar6 = ID_IT_SN_DK_BLOCK_S00;
                        uVar4 = ID_IT_SN_DK_BLOCK_L00;
                    } else {
                        uVar8 = ID_IT_ST_DK_POLE_SMOKE00;
                        uVar6 = ID_IT_SN_DK_POLE_S00;
                        uVar4 = ID_IT_SN_DK_POLE_L00;
                    }
                    // HD: the guarded effect angle copies only x and y before forcing z and x to zero.
                    auto &guard = l_angleGuard;
                    csXyz *eff_ang = &l_effAngle;
                    if (guard == 0) {
                        guard = 1;
                        eff_ang->x = at->shape_angle.x;
                        eff_ang->y = at->shape_angle.y;
                    }
                    eff_ang->z = 0;
                    eff_ang->x = 0;

                    i_this->m918.end(); /* m918.remove() */
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 2, uVar8, &i_this->current.pos, eff_ang,
                                        nullptr, 0xB9, (dPa_levelEcallBack *)(void *)&i_this->m918, room,
                                        nullptr, nullptr, nullptr);
                    }
                    GXColor *k0 = &i_this->tevStr.mColorK0; /* tevStr.mColorK0 */
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 0, uVar6, &i_this->current.pos, eff_ang,
                                        nullptr, 0xFF, nullptr, room, k0, k0, nullptr);
                    }
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 0, uVar4, &i_this->current.pos,
                                        &at->shape_angle, nullptr, 0xFF, nullptr, room, k0, k0, nullptr);
                    }

                    i_this->model = 0;
                    for (int i = 0; i < 3; i++) {
                        bdo_eff_s *e = &i_this->mEffs[i];
                        e->m000 = 1;
                        e->m028 = cM_rndFX(0.25f) + 0.7f;
                        f32 px = i_this->current.pos.x;
                        e->m008.x = px;
                        f32 py = i_this->current.pos.y;
                        e->m008.y = py;
                        e->m008.z = i_this->current.pos.z;
                        e->m008.y = py + (REG_F(8, 12) + 100.0f);
                        e->m024 = (cM_rndF(20.0f) + 60.0f) + REG_F(8, 16);
                        s16 r = (s16)gabi::ftoi(cM_rndFX(10000.0f));
                        e->m030.y = at->shape_angle.y + r;
                        e->m020 = (cM_rndF(15.0f) + 20.0f) + REG_F(8, 14);
                        e->m036.x = (s16)gabi::ftoi(cM_rndFX(6000.0f));
                        e->m036.z = (s16)gabi::ftoi(cM_rndFX(6000.0f));
                    }
                    i_this->actor_status =
                        i_this->actor_status & ~0x100u; /* fopAcM_OffStatus(fopAcStts_CULL_e) */
                    return TRUE;
                }
            }
        } else {
            i_this->mCyl.SetC(&non_pos);
        }

        if (i_this->model != 0) {
            set_actor_mtx(i_this);
            dComIfG_Ccsp_Set(&i_this->mCyl);
        }
    }

    /* hahen_move */
    for (s32 i = 0; i < 3; i++) {
        bdo_eff_s *fragment = &i_this->mEffs[i];
        if (fragment->m000 != 0) {
            fragment->m1AC = fragment->m1AC + 1;
            gabi::Local<dBgS_GndChk> gnd;
            gabi::Local<dBgS_LinChk> lin;
            if (fragment->m000 == 1) {
                top_hahen_move(i_this, fragment, gnd.get(), lin.get());
            } else {
                kaidan_hahen_move(i_this, fragment, gnd.get(), lin.get());
            }
        }
    }
    return TRUE;
}

/* 020742BC */
static BOOL daBdkobj_IsDelete(bdkobj_class *) { return TRUE; }

// HD: dComIfG_resDelete (GameCube resDeleteDemo)
/* 020742C4 */
static BOOL daBdkobj_Delete(bdkobj_class *i_this) {
    dComIfG_resDelete(&i_this->mPhase, l_deleteArcName.mText /* "Bdkobj" */);
    i_this->m918.remove(); /* m918.remove() (virtual) */
    if (i_this->pm_bgw != nullptr) {
        dBgS *bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->pm_bgw);
    }
    return TRUE;
}

/* 02074330 */
static BOOL useHeapInit(fopAc_ac_c *a_this) {
    bdkobj_class *i_this = (bdkobj_class *)a_this;

    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(l_heapArcName.mText /* "Bdkobj" */,
                                                                   bdl_data.entries[i_this->m298]);
    J3DModel *model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    a_this->model = gabi::ea(model);
    if (model == nullptr) {
        return FALSE;
    }
    s32 iVar4 = 0;
    if (i_this->m298 == 2) {
        i_this->pm_bgw = new_dBgW();
        if (i_this->pm_bgw == nullptr) /* JUT_ASSERT(0x394, i_this->pm_bgw != NULL) */
            JUT_ASSERT_fail(l_assertFile.mText, 0x394, l_assertExpression.mText);
        cBgD_t *dzb =
            (cBgD_t *)dComIfG_getObjectRes(l_heapArcName.mText, dRes_INDEX_BDKOBJ_DZB_S_TOWER_BRIDGE_e);
        if (cBgW_Set(i_this->pm_bgw, dzb, cBgW_MOVE_BG_e, &i_this->mMtx)) {
            return FALSE;
        }
        auto world = reinterpret_cast<BdkObjBgWorldView *>(i_this->pm_bgw.get());
        world->mCrrFunc = BdkObjBgWorldView::TypicalCorrection;
        world->mRideCallback = gabi::guest_fn((const void *)ride_call_back);
        iVar4 = -1;
    }
    for (s32 i = 0; i < iVar4 + 3; i++) {
        J3DModelData *hahen_modelData =
            (J3DModelData *)dComIfG_getObjectRes(l_heapArcName.mText, hahen_bdl_data.entries[i_this->m298]);
        i_this->mEffs[i].mpModel = mDoExt_J3DModel__create(hahen_modelData, 0x80000, 0x11000022);
        if (i_this->mEffs[i].mpModel == nullptr) {
            return FALSE;
        }
    }
    return TRUE;
}

// bdkobj_class::bdkobj_class (HD: out of line; allocates when this == NULL)
/* 020744C0 */
static bdkobj_class *bdkobj_class_ct(bdkobj_class *i_this) {
    if (i_this == nullptr) {
        i_this = (bdkobj_class *)operator_new(0xAA4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BDKOBJ_VTBL;
    BdkObj_constructArray(&i_this->mEffs[0], 3, sizeof(bdo_eff_s),
                          gabi::guest_fn((const void *)bdo_eff_s_ct));
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, CM3DGAAB_VTBL);
    __ct(&i_this->m918, 1);
    return i_this;
}

/* 020745AC */
static cPhs_State daBdkobj_Create(fopAc_ac_c *a_this) {
    bdkobj_class *i_this = (bdkobj_class *)a_this;
    /* fopAcM_ct(a_this, bdkobj_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bdkobj_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, l_createArcName.mText /* "Bdkobj" */);
    if (res == cPhs_ERROR_e) {
        return cPhs_ERROR_e;
    }
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    i_this->m918.m12 = 1; /* m918.setFollowOff() */
    u8 prm = fopAcM_GetParam(i_this) & 0xFF;
    i_this->m298 = (prm == 0xFF) ? 0 : prm;

    if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x5000)) {
        return cPhs_ERROR_e;
    }

    fopAcM_SetMin(i_this, -500.0f, -1500.0f, -500.0f);
    fopAcM_SetMax(i_this, 500.0f, 1500.0f, 500.0f);
    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(actor_model(i_this))); /* fopAcM_SetMtx */

    if (i_this->m298 == 2) {
        if (dBgS_Regist(dComIfG_Bgsp(), i_this->pm_bgw, i_this)) {
            return cPhs_ERROR_e;
        } else {
            return cPhs_COMPLEATE_e;
        }
    }

    i_this->mStts.Init(0xFF, 0xFF, a_this);
    i_this->mCyl.Set(&cc_cyl_src);
    i_this->mCyl.SetStts(&i_this->mStts);
    if (i_this->m298 == 0) {
        i_this->mCyl.SetH(REG_F(6, 0) + 300.0f);
        i_this->mCyl.SetR(REG_F(6, 1) + 200.0f);
    } else {
        i_this->mCyl.SetH(REG_F(6, 2) + 300.0f);
        i_this->mCyl.SetR(REG_F(6, 3) + 200.0f);
    }

    for (s32 i = 0; i < 3; i++) {
        i_this->mEffs[i].mStts.Init(0xC8, 0xFF, a_this);
        i_this->mEffs[i].mSph.Set(&hahen_sph_src);
        i_this->mEffs[i].mSph.SetStts(&i_this->mEffs[i].mStts);
    }

    return cPhs_COMPLEATE_e;
}

// static initialisation (header statics at a different layout; non_pos)
/* 0207481C */
static void __sinit_d_a_bdkobj_cpp() {
    for (int i = 0; i < 4; i++)
        l_headerStatics.zero[i] = 0;
    __register_global_object(gabi::ea(&l_piRegistration));
    l_pi.negativePi = -3.1415927f;
    l_pi.positivePi = 3.1415927f;
    BdkObj_firstHeaderCtor(&l_headerStatics.first);
    __register_global_object(gabi::ea(&l_firstRegistration));
    BdkObj_secondHeaderCtor(&l_headerStatics.second);
    __register_global_object(gabi::ea(&l_secondRegistration));
    non_pos.set(10000.0f, -10000.0f, 20000.0f);
}

// sead::SafeString deleting destructor (this TU's copy)
/* 020748D8 */
static void SafeString_dt(SafeString *s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

// bdo_eff_s::bdo_eff_s (array element constructor; allocates when this == NULL)
/* 020748EC */
static bdo_eff_s *bdo_eff_s_ct(bdo_eff_s *e) {
    if (e == nullptr) {
        e = (bdo_eff_s *)operator_new(0x1B8);
        if (e == nullptr)
            return e;
    }
    dCcD_Stts_ct(&e->mStts);
    BdkObj_sphereCtor(&e->mSph); /* dCcD_Sph::dCcD_Sph */
    return e;
}

// bdo_eff_s::~bdo_eff_s (array element destructor)
/* 0207495C */
static void bdo_eff_s_dt(bdo_eff_s *e, s32 flags) {
    if (e != nullptr) {
        BdkObj_sphereDtor(&e->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&e->mStts, 2);
        if (flags & 1)
            operator_delete(e);
    }
}

// bdkobj_class deleting destructor (compiler-generated, HD virtual destructor)
/* 020749BC */
static void bdkobj_class_dt(bdkobj_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        BdkObj_destroyArray(&i_this->mEffs[0], 3, sizeof(bdo_eff_s),
                            gabi::guest_fn((const void *)bdo_eff_s_dt), 0, 0);
        BdkObj_actorDtor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy)
/* 02074A48 */
static void SafeString_assureTermination(SafeString *) {}

/* WWHD glue
ext 02518DB0 fopAc_ac_c* BdkObj_powerCheck(BdkObjAttackInfo*)
ext 0201A554 void csXyz_apl(csXyz*, csXyz*)
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
ext 028EFFD0 void BdkObj_constructArray(void*, s32, u32, u32)
ext 028F0164 void BdkObj_destroyArray(void*, s32, u32, u32, u32, u32)
data l_deleteArcName 10008540
data l_heapArcName 10008548
data l_assertFile 10008568
data l_assertExpression 10008550
data l_createArcName 10008580

ext 025166F0 void BdkObj_sphereCtor(dCcD_Sph*)
ext 02515AE8 void BdkObj_sphereDtor(dCcD_Sph*, s32)
ext 025D50BC void BdkObj_actorDtor(fopAc_ac_c*, s32)
ext 028ED6F8 void BdkObj_firstHeaderCtor(u8*)
ext 028EAB2C void BdkObj_secondHeaderCtor(u8*)
data bdl_data 10190AC4
data hahen_bdl_data 10190ACC
data cc_cyl_src 10190B34
data hahen_sph_src 10190AF4
data non_pos 10461988
data l_scaleGuard 10461968
data l_angleGuard 104619A0
data l_fragmentScale 10461994
data l_effAngle 1046196C
data l_headerStatics 10461974
data l_pi 10461960
data l_piRegistration 10190B78
data l_firstRegistration 10190B84
data l_secondRegistration 10190B90
ext 02008E0C void BdkObj_groundBaseCtor(dBgS_GndChk*)
ext 02008FEC void BdkObj_lineBaseCtor(dBgS_LinChk*)
ext 02008DAC void BdkObj_groundBaseDtor(dBgS_GndChk*, s32)
ext 02008B4C void BdkObj_lineBaseDtor(dBgS_LinChk*, s32)
*/
