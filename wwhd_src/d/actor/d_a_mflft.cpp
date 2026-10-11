/**
 * d_a_mflft.cpp (WWHD)
 * Object - Dragon Roost Cavern - Flame lift (platform lifted up by lava plume)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mflft.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mflft.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/JSystem/JUTAssert.h"

const gc_tu_data gc_tu = {.safeString = SAFESTRING_VTBL};
using MflftWind = gptr<cXyz>;
using MflftWindPower = gptr<be<f32>>;
static MflftWind wind_vec = {};
static s16 wy = {};
static MflftWindPower wp = {};

static GXColor ropeColor = {150, 150, 150, 255};
static u8 himo_off_check[3] = {1, 2, 4};
static s16 himo_off_ya[8] = {1, 0, 0x5555, 0x2AAA, -21846, -10923, -32768, 1};
static f32 bure_p[12] = {0, 0.1f, 0.3f, 0.6f, 0.85f, 1, 0.85f, 0.6f, 0.3f, 0.1f, 0, 0};
static f32 xd[3] = {0, -117, 117};
static f32 zd[3] = {-134, 70, 70};
static dCcD_SrcCyl himo_cyl_src = {};
static u32 hioStorage[4] = {};
static MflftRegistration hioRegistration = {};
static MflftRegistration signedCharRegistration = {};
static MflftRegistration unsignedCharRegistration = {};
static f32 minPi = {};
static f32 maxPi = {};
static u8 signedCharStorage = {};
static u8 unsignedCharStorage = {};

static dCcD_Cyl *mCyls_ct(dCcD_Cyl *);

// HD: the angle sum returns packed in r3 and the high half of r4.
static inline void mflftAddAngles(const csXyz *a, const csXyz *b, csXyz *out) {
    mflftAngleAdd(a, b);
    u32 hi = gabi::cpu->r[3], lo = gabi::cpu->r[4];
    out->x = (s16)(hi >> 16);
    out->y = (s16)hi;
    out->z = (s16)(lo >> 16);
}
/* JPABaseEmitter (HD): becomeInvalidEmitter = stopCreateParticle (+0x254 |= 1) + mMaxFrame = -1
 * (+0x5C) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter *e) {
    MflftEmitterView *emitter = reinterpret_cast<MflftEmitterView *>(e);
    emitter->mMaxFrame = -1;
    emitter->mStatus |= 1;
}
/* mDoExt_3DlineMat1_c */
/* dBgS_GndChk / dBgS_ObjGndChk_Yogan on the stack (this TU's vtables) */
static inline void yogan_ct(void *c) {
    dBgS_GndChk_ct(c, YOGAN_VT, true);
    reinterpret_cast<MflftGroundView *>(c)->mGrp = 4; /* lava */
}
static inline void dBgS_GndChk_dt(void *c) {
    MflftGroundView *ground = reinterpret_cast<MflftGroundView *>(c);
    ground->mPolyInfoVtable = MflftGroundPolyVtable;
    ground->mPolyPassVtable = MflftGroundPassVtable;
    ground->mGroupVtable = MflftGroundGroupVtable;
    mflftGroundBaseDestroy(c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
/* HD: lfs -> stfs with no arithmetic in between is bit-exact in the recompiled original (a
 * signalling NaN is not quieted): such copies are written as word copies */
static inline void fcopy(be<f32> *dst, const be<f32> *src) {
    *reinterpret_cast<be<u32> *>(dst) = (u32) * reinterpret_cast<const be<u32> *>(src);
}
static inline void J3DModel_setBaseScale_bits(J3DModel *m, const cXyz *s) {
    for (u32 i = 0; i < 3; i++)
        reinterpret_cast<be<u32> *>(&m->mBaseScale)[i] =
            (u32) reinterpret_cast<const be<u32> *>(s)[i];
}
static inline f32 fabsf_(f32 v) { return v < 0.0f ? -v : (v == 0.0f ? 0.0f * v + 0.0f : v); }

/* 021C3ECC */
void mflft_class::setLiftUp(cXyz *arg1) {

    if ((m29A != 0) || (fabsf_(arg1->y - current.pos.y) < 200.0f)) {
        fcopy(&current.pos.x, &arg1->x);
        fcopy(&current.pos.z, &arg1->z);
        cLib_addCalc2(&current.pos.y, arg1->y, 0.2f, 1000.0f * m2D0);
        cLib_addCalc2(&m2D0, 1.0f, 1.0f, 0.05f);
        speed.y = 0.0f;
        if (m2CE == 0) {
            gabi::Local<cXyz> v;
            dVibration_c *vib = dComIfGp_getVibration();
            v->set(0.0f, 1.0f, 0.0f);
            mflftShock(vib, REG0_S(2) + 8, -0x21, v.get()); /* StartShock */
        }
        m2CE = 5;
    }
}

/* 021C2064 */
static void ride_call_back(dBgW *bg, fopAc_ac_c *arg1, fopAc_ac_c *arg2) {

    mflft_class *i_this = (mflft_class *)arg1;

    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> sp38;
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    cXyz_mi(&arg2->current.pos, tmp.get(), &i_this->current.pos);
    sp38->copy(*tmp);
    MtxPosition(sp38.get(), sp2C.get());
    cXyz_mi(&arg2->old.pos, tmp.get(), &i_this->current.pos);
    sp38->copy(*tmp);
    MtxPosition(sp38.get(), sp20.get());
    f32 k = REG0_F(0) + 10.0f;
    s16 iVar2 = (s16)gabi::ftoi(sp2C->z * (k / i_this->scale.z));
    s16 iVar1 = (s16)gabi::ftoi(-(sp2C->x * (k / i_this->scale.x)));

    cLib_addCalcAngleS2(&i_this->current.angle.x, iVar2, 10, 0x800);
    cLib_addCalcAngleS2(&i_this->current.angle.z, iVar1, 10, 0x800);

    f32 fVar3 = fabsf_(sp2C->z - sp20->z) * (REG0_F(4) + 50.0f);
    if (i_this->m2BC < fVar3) {
        i_this->m2BC = fVar3;
    }
    fVar3 = fabsf_(sp2C->x - sp20->x) * (REG0_F(4) + 50.0f);
    if (i_this->m2C4 < fVar3) {
        i_this->m2C4 = fVar3;
    }
    fVar3 = fabsf_(sp2C->x - sp20->x) * (REG0_F(8) + 5.0f);
    if ((fVar3 > 10.0f) && (i_this->m2B0 < fVar3)) {
        cLib_addCalc2(&i_this->m2B0, fVar3, 1.0f, REG0_F(7) + 1.2f);
    }
    fVar3 = fabsf_(sp2C->z - sp20->z) * (REG0_F(8) + 5.0f);
    if ((fVar3 > 10.0f) && (i_this->m2B8 < fVar3)) {
        cLib_addCalc2(&i_this->m2B8, fVar3, 1.0f, REG0_F(7) + 1.2f);
    }
}

/* 021C2304 */
static BOOL daMflft_Draw(mflft_class *i_this) {

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    dScnKy_env_light_c *env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();

    mflftLineUpdate(&i_this->mLineMat, 10, &ropeColor, &i_this->tevStr);
    dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c *>(&i_this->mLineMat));
    return TRUE;
}

/* 021C23E0 */
static void himo_cut_control(mflft_class *i_this, cXyz *arg1, u8 *arg2, u8 arg3) {

    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk.get(), GNDCHK_VT, false);
    gabi::Local<cXyz> sp30;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;
    gabi::Local<cXyz> sp24;

    sp30->x = 0.0f;
    sp30->y = 0.0f;
    sp30->z = (f32)*wp * 7.0f;
    cMtx_YrotS(calc_mtx(), wy);
    MtxPosition(sp30.get(), sp18);

    if (arg3 != 0) {
        sp0C->x = 0.0f;
        sp0C->y = 0.0f;
        sp0C->z = 0.0f;
        sp30->z = REG0_F(2) + 135.0f;
    } else {
        cMtx_YrotS(calc_mtx(), i_this->m6F8 + i_this->shape_angle.y);
        sp30->z = REG0_F(11) + -4.0f;
        MtxPosition(sp30.get(), sp0C.get());
        sp30->z = REG0_F(2) + 10.0f;
    }

    arg1++;
    for (s32 i = 1; i < 10; i++, arg1++, arg2++) {
        f32 fVar2 = (arg1->x - arg1[-1].x) + sp18->x + REG0_F(3) + sp0C->x;
        f32 fVar1 = arg1->y - 10.0f;
        if (arg3 == 0) {
            f32 y = arg1->y + 50.0f;
            cXyz *gp = &gndChk->m_pos;
            fcopy(&gp->x, &arg1->x);
            fcopy(&sp24->x, &arg1->x);
            fcopy(&gp->z, &arg1->z);
            fcopy(&sp24->z, &arg1->z);
            sp24->y = y;
            gp->y = y;
            f32 fVar9 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get()) + 10.0f;
            if (!(fVar1 > fVar9)) {
                fVar1 = fVar9;
            }
        }
        f32 fz = (arg1->z - arg1[-1].z) + sp18->z + sp0C->z;
        f32 fVar8 = fVar1 - arg1[-1].y;

        s16 iVar4 = cM_atan2s(fVar2, fz);
        s16 iVar5 = -cM_atan2s(fVar8, std_sqrtf(gabi::fmadds(fVar2, fVar2, fz * fz)));

        cMtx_YrotS(calc_mtx(), iVar4);
        cMtx_XrotM(calc_mtx(), iVar5);
        MtxPosition(sp30.get(), sp24.get());

        arg1->y = arg1[-1].y + sp24->y;
        arg1->x = arg1[-1].x + sp24->x;
        arg1->z = arg1[-1].z + sp24->z;
        *reinterpret_cast<be<u8> *>(arg2) = 3;
    }
    dBgS_GndChk_dt(gndChk.get());
}

/* inlined into daMflft_Execute */
static inline void mflft_move(mflft_class *i_this) {
    i_this->m298 = i_this->m298 + 1;
    switch ((u16)i_this->m29A) {
    case 0: {
        s32 uVar6 = 0;
        for (s32 i = 0; i < 3; i++) {
            if (i_this->m2D4[i] != 0) {
                uVar6 |= (u8)himo_off_check[i]; /* himo_off_check */
            }
        }
        s16 target;
        s16 sVar2 = (s16)himo_off_ya[uVar6]; /* himo_off_ya */
        if (sVar2 != 1) {
            target = -0x3000;
        } else {
            i_this->m6FA = 0;
            target = 0;
        }

        if (uVar6 == 7) {
            i_this->m29A = 1;
            dSv_event_onEventBit(dComIfGs_getEvent(), 0x2A10);
            gabi::Local<dBgS_GndChk> gndChk;
            yogan_ct(gndChk.get());
            cXyz *gp = &gndChk->m_pos;
            fcopy(&gp->x, &i_this->current.pos.x);
            gp->y = i_this->current.pos.y + 1000.0f;
            fcopy(&gp->z, &i_this->current.pos.z);
            f32 fVar10 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
            if (fVar10 != -1000000000.0f) {
                i_this->m734 = fVar10;
            }
            if (i_this->m2A1 != 0xff) {
                dComIfGs_onSwitch(i_this->m2A1, fopAcM_GetRoomNo(i_this));
            }
            dBgS_GndChk_dt(gndChk.get());
        } else {
            if (i_this->m6F5 != 0) {
                s8 v = (s8)(i_this->m6F5 - 1);
                i_this->m6F5 = v;
                if ((v == 0) && (uVar6 != 0)) {
                    u8 eventReg = dSv_event_getEventReg(dComIfGs_getEvent(), 0xA507);
                    if (eventReg < 6) {
                        eventReg++;
                        dSv_event_setEventReg(dComIfGs_getEvent(), 0xA507, eventReg);
                    }
                }
            }

            cLib_addCalcAngleS2(&i_this->m6FA, target, 4, 0x2000);
            cLib_addCalcAngleS2(&i_this->m6F8, sVar2, 2, 0x2000);
            cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
            cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

            f32 fVar10, fVar1;
            if (i_this->m2CE != 0) {
                fVar10 = 2000.0f;
                fVar1 = 250.0f;
                if (i_this->m778 == 0) {
                    i_this->m778 = 1;
                }
            } else {
                fVar10 = 100.0f;
                fVar1 = 20.0f;
                if (i_this->m778 == 2) {
                    i_this->m778 = 3;
                }
            }

            i_this->m2C8.x = (s16)gabi::ftoi(i_this->m2BC * cM_ssin(i_this->m298 * 0x5dc));
            i_this->m2C8.z = (s16)gabi::ftoi(i_this->m2C4 * cM_ssin(i_this->m298 * 0x514));
            cLib_addCalc2(&i_this->m2BC, fVar10, 1.0f, fVar1 + REG0_F(3));
            cLib_addCalc2(&i_this->m2C4, fVar10, 1.0f, fVar1 + REG0_F(3));
            i_this->m2A4.x = i_this->m2B0 * cM_ssin(i_this->m298 * 0x2ee);
            i_this->m2A4.z = i_this->m2B8 * cM_ssin(i_this->m298 * 900);
            cLib_addCalc0(&i_this->m2B0, 1.0f, REG0_F(6) + 0.25f);
            cLib_addCalc0(&i_this->m2B8, 1.0f, REG0_F(6) + 0.25f);
            mflftAddAngles(&i_this->current.angle, &i_this->m2C8, &i_this->shape_angle);
            gabi::Local<cXyz> tmp;
            cXyz_pl(&i_this->home.pos, tmp.get(), &i_this->m2A4);
            i_this->current.pos.copy(*tmp);
        }
    } break;

    case 1: {
        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.1f, 5.0f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.1f, 5.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x300);
        cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x300);
        cLib_addCalcAngleS2(&i_this->m6FA, 0, 4, 0x200);

        f32 fVar10, fVar1;
        if (i_this->m2CE != 0) {
            fVar10 = 2500.0f;
            fVar1 = 250.0f;
            if (i_this->m778 == 0) {
                i_this->m778 = 1;
            }
        } else {
            fVar10 = 400.0f;
            fVar1 = 20.0f;
            if (i_this->m778 == 2) {
                i_this->m778 = 3;
            }
        }
        i_this->m2C8.x = (s16)gabi::ftoi(i_this->m2BC * cM_ssin(i_this->m298 * 800));
        i_this->m2C8.z = (s16)gabi::ftoi(i_this->m2C4 * cM_ssin(i_this->m298 * 700));
        cLib_addCalc2(&i_this->m2BC, fVar10, 1.0f, fVar1);
        cLib_addCalc2(&i_this->m2C4, fVar10, 1.0f, fVar1);

        mflftAddAngles(&i_this->current.angle, &i_this->m2C8, &i_this->shape_angle);
        i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
        f32 sy = i_this->speed.y - 5.0f;
        if (sy < -300.0f) {
            sy = -300.0f;
        }
        i_this->speed.y = sy;

        f32 lim = i_this->m734 + 80.0f;
        if (!(i_this->current.pos.y > lim)) {
            i_this->current.pos.y = lim;
            if (i_this->speed.y < -50.0f) {
                i_this->m2C4 = 2000.0f;
                i_this->m2BC = 2000.0f;
                dComIfGp_particle_set(dPa_name::ID_AK_SN_MAGMALIFTSPLASH00, &i_this->current.pos,
                                      &i_this->shape_angle);
                /* fopAcM_seStartCurrent (HD: null check on the position) */
                if (gabi::ea(&i_this->current.pos) != 0) {
                    mDoAud_seStart(JA_SE_OBJ_BAL_LIFT_LANDING, &i_this->current.pos, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                }
                gabi::Local<cXyz> v;
                dVibration_c *vib = dComIfGp_getVibration();
                v->set(0.0f, 1.0f, 0.0f);
                mflftShock(vib, REG0_S(2) + 5, -0x21, v.get()); /* StartShock */
            }
            i_this->speed.y = 0.0f;
            cLib_addCalcAngleS2(&i_this->m6FA, 0, 2, 0x2000);
        }
        break;
    }
    }
}

/* inlined into daMflft_Execute */
static inline void himo_move(mflft_class *i_this) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    /* sp18 (csXyz) and sp48 (cXyz) are adjacent in the original frame (0x38 / 0x40): the
     * particle call compares more than the 6 bytes of the angle */
    gabi::Local<MflftParticleFrame> fr;
    cXyz *sp48 = &fr->sp48;
    csXyz *sp18 = &fr->sp18;
    gabi::Local<cXyz> sp3C;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> tmi;
    gabi::Local<CcAtInfo> ccAtInfo;

    for (s32 i = 0; i < 3; i++) {
        cMtx_YrotS(calc_mtx(), i_this->m298);
        sp48->x = 0.0f;
        sp48->y = 0.0f;
        sp48->z = (f32)(s16)i_this->m6EC[i] * cM_ssin(i_this->m298 * 15000);
        MtxPosition(sp48, sp3C.get());

        cXyz *line1 = i_this->mLineMat.getPos(i);
        cXyz *line2 = i_this->mLineMat.getPos(i + 3);

        if (i_this->m2D4[i] == 0) {
            cXyz_mi(&i_this->m2FC[i], tmi.get(), &i_this->m2D8[i]);
            sp48->copy(*tmi);
            u8 *lineSize = i_this->mLineMat.getSize(i);

            for (s32 j = 0; j < 10; line1++, line2++, lineSize++, j++) {
                if (j < 9) {
                    f32 tmp = (f32)j / 27.0f;
                    cXyz_ml(sp48, t1.get(), tmp);
                    cXyz_pl(&i_this->m2D8[i], t2.get(), t1.get());
                    f32 x = t2->x, z = t2->z;
                    fcopy(&line1->x, &t2->x);
                    fcopy(&line1->y, &t2->y);
                    fcopy(&line1->z, &t2->z);
                    line1->x = gabi::fmadds(sp3C->x * (f32)bure_p[j], REG0_F(0) + 1.0f, x);
                    line1->z = gabi::fmadds(sp3C->z * (f32)bure_p[j], REG0_F(0) + 1.0f, z);
                } else {
                    line1->copy(i_this->m2FC[i]);
                }
                line2->copy(i_this->m2D8[i]);
                *reinterpret_cast<be<u8> *>(lineSize) = 3;
            }

            dCcD_Cyl *cyl = &i_this->mCyls[i];
            if (cyl->ChkTgHit() && (i_this->m6EC[i] < 10)) {
                i_this->m6EC[i] = REG0_S(3) + 0xf;
                ccAtInfo->mpObj = static_cast<dCcD_GObjInf *>(cyl->GetTgHitObj());
                mflftAttackPower(ccAtInfo.get());

                u8 dmg = ccAtInfo->mDamage;
                if (dmg > 1) {
                    dmg = 4;
                    ccAtInfo->mDamage = 4;
                }
                s8 hp = (s8)(i_this->m6F2[i] - dmg);
                i_this->m6F2[i] = hp;
                fopAc_ac_c *actor = ccAtInfo->mpActor;

                s32 soundId;
                if (hp <= 0) {
                    i_this->m2D4[i] = 1;
                    i_this->m2C4 = 2000.0f;
                    i_this->m2BC = 2000.0f;
                    soundId = JA_SE_LK_CUT_SBRIDGE_ROPE; /* JA_SE_LK_CUT_SBRIDGE_ROPE */
                    i_this->m6F5 = 0x14;
                } else {
                    soundId = JA_SE_LK_HIT_SBRIDGE_ROPE; /* JA_SE_LK_HIT_SBRIDGE_ROPE */
                }

                if (actor != 0) {
                    sp48->copy(i_this->m2D8[i]);
                    fcopy(&sp48->y, &actor->eyePos.y); /* eyePos.y */
                    dComIfGp_particle_set(dPa_name::ID_AK_JN_OK, sp48, &player->shape_angle);
                    /* kikuzu_set inlined */
                    fopAc_ac_c *pl = dComIfGp_getPlayer(0);
                    sp18->x = pl->shape_angle.x;
                    sp18->y = pl->shape_angle.y;
                    sp18->y = (s16)(pl->shape_angle.y - 0x8000);
                    sp18->z = pl->shape_angle.z;
                    GXColor *k0 = &i_this->tevStr.mColorK0; /* tevStr.mColorK0 */
                    JPABaseEmitter *e =
                        dComIfGp_particle_set(dPa_name::ID_AK_JN_ELEMENTKIKUZU00, sp48, sp18,
                                              nullptr, 0xff, nullptr, -1, k0, k0, nullptr);
                    if (e != nullptr) {
                        MflftEmitterView *emitter = reinterpret_cast<MflftEmitterView *>(e);
                        emitter->mRate = 10.0f;
                        emitter->mSpread = 0.2f;
                        emitter->mMaxFrame = 1;
                        emitter->mVolumeSweep = 0.15f;
                        f32 s = REG0_F(16) + 0.7f;
                        emitter->mGlobalParticleScale.x = s;
                        emitter->mGlobalParticleScale.y = s;
                        emitter->mGlobalParticleScale.z = s;
                    }
                    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
                    mDoAud_seStart(soundId, &ccAtInfo->mpActor->eyePos, 0, reverb);
                }
            }
            cyl->SetC(&i_this->m2D8[i]);
            dComIfG_Ccsp_Set(cyl);
        } else {
            line1->copy(i_this->m2D8[i]);
            himo_cut_control(i_this, line1, i_this->mLineMat.getSize(i), 0);
            line2->copy(i_this->m2FC[i]);
            himo_cut_control(i_this, line2, i_this->mLineMat.getSize(i + 3), 1);
        }
    }
}

/* inlined into daMflft_Execute */
static inline void eff_cont(mflft_class *i_this) {
    gabi::Local<cXyz> sp24;
    fcopy(&sp24->x, &i_this->current.pos.x);
    f32 y = i_this->current.pos.y;
    sp24->y = y;
    fcopy(&sp24->z, &i_this->current.pos.z);
    sp24->y = y - (REG0_F(1) + 90.0f);

    switch ((u8)i_this->m778) {
    case 0:
        break;
    case 1: {
        JPABaseEmitter *e = dComIfGp_particle_set(dPa_name::ID_AK_SN_MAGMAISLAND01, sp24.get());
        i_this->m774 = e;
        if (e != nullptr) {
            /* setGlobalScale(1.8): global and particle scale */
            MflftEmitterView *emitter = reinterpret_cast<MflftEmitterView *>(e);
            // HD: global and particle scale channels are stored in this order.
            for (u32 i = 0; i < 3; i++)
                reinterpret_cast<be<f32> *>(&emitter->mGlobalScale)[i] = 1.8f;
            for (u32 i = 0; i < 3; i++)
                reinterpret_cast<be<f32> *>(&emitter->mGlobalParticleScale)[i] = 1.8f;
            i_this->m778 = 2;
        } else {
            i_this->m778 = 0;
        }
        break;
    }
    case 2:
        if (i_this->m774 != nullptr) {
            MtxTrans(0.0f, -90.0f, 0.0f, 1);
            /* setGlobalRTMatrix */
            Mtx34 *m = calc_mtx();
            MflftEmitterView *emitter = reinterpret_cast<MflftEmitterView *>(i_this->m774.get());
            mflftSetRotationTranslation(m, emitter->mGlobalRotation, &emitter->mGlobalTranslation);
        }
    case 3:
        if (i_this->m778 == 3) {
            if (i_this->m774 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->m774);
                i_this->m774 = nullptr;
            }
            i_this->m778 = 0;
        }
        break;
    }
}

/* 021C2748 */
static BOOL daMflft_Execute(mflft_class *i_this) {

    dComIfGp_get(); /* HD: an unused accessor call */
    if (i_this->m2CE != 0) {
        i_this->m2CE = i_this->m2CE - 1;
    } else {
        i_this->m2D0 = 0.0f;
    }

    cXyz *wv = dKyw_get_wind_vec();
    wind_vec = wv;
    wy = cM_atan2s(wv->x, wv->z);
    wp = mflftWindPower();
    dComIfGp_get(); /* HD: an unused accessor call */
    mflft_move(i_this);

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    cMtx_YrotM(calc_mtx(), i_this->m6F8);
    cMtx_XrotM(calc_mtx(), i_this->m6FA);
    cMtx_YrotM(calc_mtx(), -i_this->m6F8);
    cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());

    for (s32 i = 0; i < 3; i++) {

        MtxPush();
        gabi::Local<cXyz> sp08;
        sp08->x = (f32)xd[i] * i_this->scale.x;
        sp08->y = 15.0f;
        sp08->z = (f32)zd[i] * i_this->scale.z;
        MtxPosition(sp08.get(), &i_this->m2D8[i]);

        if (i_this->m6F6 != 0) {
            i_this->m6F6 = i_this->m6F6 - 1;
            i_this->m2FC[i].copy(i_this->m2D8[i]);
            i_this->m2FC[i].y = i_this->home.pos.y + 2000.0f + REG0_F(19);
        }
        if (i_this->m6EC[i] != 0) {
            i_this->m6EC[i] = i_this->m6EC[i] - 1;
        }
        MtxPull();
    }

    MtxPush();
    eff_cont(i_this);
    MtxPull();

    f32 y = 1.0f;
    if (i_this->m2CE != 0) {
        y = 5.0f;
    }
    MtxScale(1.0f, y, 1.0f, 1);
    PSMTXCopy(calc_mtx(), &i_this->m700);
    dBgW_Move(i_this->pm_bgw);
    himo_move(i_this);
    return TRUE;
}

/* 021C38D4 */
static BOOL daMflft_IsDelete(mflft_class *i_this) { return TRUE; }

/* 021C38DC */
static BOOL daMflft_Delete(mflft_class *i_this) {

    dComIfG_resDelete(&i_this->mPhase, "Mflft"); /* dComIfG_resDeleteDemo */
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
    }
    if (i_this->m774 != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->m774);
        i_this->m774 = nullptr;
    }
    return TRUE;
}

/* 021C395C */
static BOOL CallbackCreateHeap(fopAc_ac_c *a_this) {

    mflft_class *actor = (mflft_class *)a_this;

    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
        "Mflft", dRes_INDEX_MFLFT_BDL_MFLFT_e, SAFESTRING_VTBL);
    J3DModel *model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    actor->mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    if (modelData == nullptr) /* JUT_ASSERT(1053, modelData != NULL) */
        JUT_ASSERT_fail("d_a_mflft.cpp", 0x41E, "modelData != (0)");

    dBgW *bgw = new_dBgW();
    actor->pm_bgw = bgw;
    if (bgw == nullptr) /* JUT_ASSERT(1058, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail("d_a_mflft.cpp", 0x423, "actor->pm_bgw != (0)");

    cBgD_t *dzb =
        (cBgD_t *)dComIfG_getObjectRes("Mflft", dRes_INDEX_MFLFT_DZB_MFLFT_e, SAFESTRING_VTBL);
    cBgW_Set(actor->pm_bgw, dzb, cBgW_MOVE_BG_e, &actor->m700);
    reinterpret_cast<MflftBgView *>(actor->pm_bgw.get())->mCorrectCallback =
        MflftTypicalRotY; /* SetCrrFunc(dBgS_MoveBGProc_Typical) */
    reinterpret_cast<MflftBgView *>(actor->pm_bgw.get())->mRideCallback.v =
        gabi::guest_fn((const void *)ride_call_back); /* SetRideCallback(ride_call_back) */
    void *img = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTI_ROPE_e, SAFESTRING_VTBL);
    if (!mflftLineInit(&actor->mLineMat, 6, 10, img, 1)) {
        return FALSE;
    }
    return TRUE;
}

/* 021C3AC0 */
static cPhs_State daMflft_Create(fopAc_ac_c *a_this) {

    mflft_class *i_this = (mflft_class *)a_this;

    /* fopAcM_ct(&i_this->actor, mflft_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = MFLFT_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            mflftConstructArray(i_this->mCyls, 3, sizeof(dCcD_Cyl),
                                gabi::guest_fn((const void *)mCyls_ct));
            mflftLineConstruct(&i_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    dComIfGp_get(); /* HD: an unused accessor call */

    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, "Mflft");
    if (PVar2 == cPhs_COMPLEATE_e) {
        u8 m2A0 = fopAcM_GetParam(a_this) & 0xFF;
        if (m2A0 == 0xff) {
            m2A0 = 0;
        }
        i_this->m2A0 = m2A0;
        i_this->m2A1 = fopAcM_GetParam(a_this) >> 0x18;
        if (!fopAcM_entrySolidHeap(a_this, CallbackCreateHeap, 0x10000)) {
            return cPhs_ERROR_e;
        }
        if ((i_this->pm_bgw != nullptr) && dBgS_Regist(dComIfG_Bgsp(), i_this->pm_bgw, a_this)) {
            return cPhs_ERROR_e;
        }

        switch (i_this->m2A0) {
        case 1:
            a_this->scale.x = 0.9f;
            a_this->scale.z = 0.9f;
            break;
        case 2:
            a_this->scale.x = 0.8f;
            a_this->scale.z = 0.8f;
            break;
        case 3:
            a_this->scale.x = 0.7f;
            a_this->scale.z = 0.7f;
            break;
        default:
            a_this->scale.z = 1.0f;
            a_this->scale.x = 1.0f;
            break;
        }
        a_this->scale.y = 1.0f;
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
        mflftCullMin(a_this, -200.0f * a_this->scale.x, -10000.0f, -200.0f * a_this->scale.z);
        mflftCullMax(a_this, 200.0f * a_this->scale.x, 10000.0f, 200.0f * a_this->scale.z);

        J3DModel_setBaseScale_bits(i_this->mpModel, &a_this->scale);
        i_this->mStts.Init(0xff, 0xff, a_this);

        for (s32 i = 0; i < 3; i++) {
            i_this->mCyls[i].Set(&himo_cyl_src /* himo_cyl_src */);
            i_this->mCyls[i].SetStts(&i_this->mStts);
            i_this->m6F2[i] = 3;
        }

        i_this->m6F6 = 10;
        if ((i_this->m2A1 != 0xff) && dComIfGs_isSwitch(i_this->m2A1, fopAcM_GetRoomNo(a_this))) {
            for (s32 i = 0; i < 3; i++) {
                i_this->m2D4[i] = 1;
            }
            gabi::Local<dBgS_GndChk> gndChk;
            yogan_ct(gndChk.get());
            cXyz *gp = &gndChk->m_pos;
            fcopy(&gp->x, &a_this->current.pos.x);
            gp->y = a_this->current.pos.y + 1000.0f;
            fcopy(&gp->z, &a_this->current.pos.z);
            f32 fVar9 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
            if (fVar9 != -1000000000.0f) {
                a_this->current.pos.y = fVar9;
            }
            dBgS_GndChk_dt(gndChk.get());
        }

        daMflft_Execute(i_this);
    }
    return PVar2;
}

/* 021C3FEC */
static void __sinit_d_a_mflft_cpp() {

    for (int i = 0; i < 4; i++)
        hioStorage[i] = 0;
    mflftRegisterDestructor(&hioRegistration);
    minPi = -3.1415927f;
    maxPi = 3.1415927f;
    mflftConstructSignedChar(&signedCharStorage);
    mflftRegisterDestructor(&signedCharRegistration);
    mflftConstructUnsignedChar(&unsignedCharStorage);
    mflftRegisterDestructor(&unsignedCharRegistration);
}

/* 021C4080 */
static void SafeString_dt(void *p, s32 flags) {

    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 021C4094: array element constructor of mCyls (dCcD_Cyl::dCcD_Cyl, this TU's copy; allocates when
 * this == NULL) */
/* 021C4094 */
static dCcD_Cyl *mCyls_ct(dCcD_Cyl *c) {

    if (c == nullptr) {
        c = (dCcD_Cyl *)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, AAB_VTBL);
    return c;
}

/* 021C4120 */
static void mflft_class_dt(mflft_class *i_this, s32 flags) {

    if (i_this != nullptr) {
        mflftLineDestroy(&i_this->mLineMat, 2); /* mDoExt_3DlineMat1_c::~mDoExt_3DlineMat1_c */
        mflftDestroyArray(i_this->mCyls, 3, 0x130, MflftCylinderDestructor, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        mflftActorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021C41AC */
static void SafeString_assureTerminationImpl(void *p) {}
/* WWHD glue (tools/verify/autoglue.py)
data wind_vec 10464FBC nocheck
data wy 10464FCC nocheck
data wp 10464FC0 nocheck
ext 028249B0 void mflftSetRotationTranslation(Mtx34*, be<f32>*, cXyz*)
data ropeColor 101BA45C
data himo_off_check 101BA460
data himo_off_ya 101BA4B4
data bure_p 101BA464
data xd 101BA4C4
data zd 101BA4D0
data himo_cyl_src 101BA4DC nocheck
data hioStorage 10464FD0 nocheck
data hioRegistration 101BA520 nocheck
data signedCharRegistration 101BA52C nocheck
data unsignedCharRegistration 101BA538 nocheck
data minPi 10464FC4 nocheck
data maxPi 10464FC8 nocheck
data signedCharStorage 10464FCE nocheck
data unsignedCharStorage 10464FCF nocheck
str 10014B64 "Mflft" in 021C38DC
str 10014B6C "Mflft" in 021C395C
str 10014BD0 "Mflft" in 021C3AC0
str 10014B74 "Always"
str 10014B7C "d_a_mflft.cpp"
str 10014B8C "modelData != (0)"
str 10014BA0 "actor->pm_bgw != (0)"
ext 028F026C void mflftRegisterDestructor(MflftRegistration*)
ext 028ED6F8 void mflftConstructSignedChar(be<u8>*)
ext 028EAB2C void mflftConstructUnsignedChar(be<u8>*)
ext 028EFFD0 void mflftConstructArray(void*, u32, u32, u32)
ext 028F0164 void mflftDestroyArray(void*, u32, u32, u32, u32)
ext 0257DB04 be<f32>* mflftWindPower()
ext 0201A4DC void mflftAngleAdd(const csXyz*, const csXyz*)
ext 025D672C void mflftCullMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void mflftCullMax(fopAc_ac_c*, f32, f32, f32)
ext 02518DB0 void mflftAttackPower(CcAtInfo*)
ext 025ED1BC void mflftLineUpdate(mDoExt_3DlineMat1_mflft_view*, s32, const GXColor*, dKy_tevstr_c*)
ext 025EBA58 BOOL mflftLineInit(mDoExt_3DlineMat1_mflft_view*, s32, s32, void*, s32)
ext 02008DAC void mflftGroundBaseDestroy(void*, s32)
ext 025CB374 void mflftShock(dVibration_c*, s32, s32, cXyz*)
ext 025EB82C void mflftLineConstruct(mDoExt_3DlineMat1_mflft_view*)
ext 025EB8B8 void mflftLineDestroy(mDoExt_3DlineMat1_mflft_view*, s32)
ext 025D50BC void mflftActorDestroy(fopAc_ac_c*, s32)
*/
