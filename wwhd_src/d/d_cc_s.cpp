/* Collision manager. GameCube class/field names with verified HD arithmetic, virtual-call
 * ordering and explicit stack ABI. */
#include "d/d_cc_s.h"
namespace gc_tu {
static constexpr u32 headerState = 0x1046F000;
static constexpr u32 runtimeObjects = 0x101D57DC;
} // namespace gc_tu
static constexpr f32 kZeroEps = 3.814697265625e-06f;
using RankTable = u8[11][11];
using Vtable = be<u32>[1];
static RankTable rank_tbl;
static cCcS_vtableHD l_dCcSVtable;
static Vtable l_shapeVtable;
static Vtable l_aabVtable;
static Vtable l_cpsAttrVtable;
static Vtable l_cpsVtable;

// HD: these are ABI views only; shared collider fields and the guest stack stay authoritative.
static cCcD_objVtableHD* objectVtableHD(cCcD_Obj* obj) {
    return gabi::at<cCcD_objVtableHD>(obj->__vtbl_hitinf);
}
static dCcD_GObjInf* GetGObjInf(cCcD_Obj* obj) {
    return gabi::call_ptr<dCcD_GObjInf*>(objectVtableHD(obj)->GetGObjInf, obj);
}
static fopAc_ac_c* GetAc(cCcD_Stts* stts) { return stts ? stts->mp_actor.get() : nullptr; }
static dCcS_stackArgsHD* stackArgsHD() { return gabi::at<dCcS_stackArgsHD>(gabi::cpu->r[1] + 8); }
static be<s16>* shieldAnglePointerHD(dCcD_GObjInf* inf) {
    return gabi::at<be<s16>>(inf->mGObjTg.mpShieldFrontRangeYAngle);
}
/* 02518760 */
void dCcS::Ct() {
    cCcS::Ct();
    dCcMassS_Mng_CtHD(&mMass_Mng);
}

/* 02518794 */
void dCcS::Dt() { cCcS::Dt(); }

/* 02517C70 */
u32 dCcS::ChkShieldFrontRange(cCcD_Obj* obj1, cCcD_Obj* obj2) {
    dCcD_GObjInf* inf1 = GetGObjInf(obj1);
    if (inf1 == 0)
        return 0;
    cCcD_Stts* s1 = inf1->mStts;
    if (s1 == 0)
        return 0;
    fopAc_ac_c* ac1 = s1->mp_actor;
    if (ac1 == 0)
        return 0;
    dCcD_GObjInf* inf2 = GetGObjInf(obj2);
    if (inf2 == 0)
        return 0;
    cCcD_Stts* s2 = inf2->mStts;
    if (s2 == 0)
        return 0;
    fopAc_ac_c* ac2 = s2->mp_actor;
    if (ac2 == 0)
        return 0;

    cXyz delta;
    cXyz* d = &delta;
    PSVECSubtract(&ac1->eyePos, &ac2->eyePos, d); /* eyePos */
    f32 dist = PSVECMag(d);
    if (std::fabs(dist) < kZeroEps)
        return 0;
    PSVECNormalize(d, d);
    s16 deltaAngle = cM_atan2s(d->x, d->z);
    be<s16>* shield = shieldAnglePointerHD(inf2); /* mpShieldFrontRangeYAngle */
    s16 shieldAngle = shield != 0 ? *shield : (s16)ac2->shape_angle.y; /* shape_angle.y */
    if (cLib_distanceAngleS(deltaAngle, shieldAngle) > 0x4000)
        return 0;
    return 1;
}

/* 02517DEC */
u32 dCcS::ChkShield(cCcD_Obj* obj1, cCcD_Obj* obj2, dCcD_GObjInf* inf1, dCcD_GObjInf* inf2) {
    if (dCcD_GObjInf_ChkAtNoGuardHD(inf1))
        return 0;
    u32 tgSPrm = inf2->mGObjTg.mSPrm;
    if (!(tgSPrm & dCcG_TgSPrm_Shield_e))
        return 0; /* ChkTgShield */
    if (!(tgSPrm & dCcG_TgSPrm_ShieldFrontRange_e))
        return 1; /* ChkTgShieldFrontRange */
    return ChkShieldFrontRange(obj1, obj2);
}

/* 02518670 */
void dCcS::CalcTgPlusDmg(cCcD_Obj* obj1, cCcD_Obj* obj2, cCcD_Stts* stts1, cCcD_Stts* stts2) {
    dCcD_GObjInf* inf1 = GetGObjInf(obj1);
    dCcD_GObjInf* inf2 = GetGObjInf(obj2);
    if (!ChkShield(obj1, obj2, inf1, inf2)) {
        u8 atp = obj1->mAtp; /* GetAtAtp */
        if ((s32)stts2->m_dmg < (s32)atp)
            stts2->m_dmg = atp; /* PlusDmg */
    }
}

/* 025185F8 */
bool dCcS::ChkAtTgHitAfterCross(u32 setAt, u32 setTg, dCcD_GObjInf* atInf, dCcD_GObjInf* tgInf,
                                cCcD_Stts* atStts, cCcD_Stts* tgStts, dCcD_GStts* atGStts) {
    dCcD_GStts* tgGStts = stackArgsHD()->tgSttsAsGSttsHD();
    u32 tgApid = tgStts->m_apid;
    u32 atApid = atStts->m_apid;
    if (setAt) {
        atGStts->mAtApid = tgApid; /* SetAtApid */
        if ((atInf->mGObjAt.mSPrm & dCcG_AtSPrm_NoConHit_e) &&
            atGStts->mAtOldApid == tgStts->m_apid)
            return true; /* ChkAtNoConHit */
    }
    if (setTg) {
        tgGStts->mTgApid = atApid; /* SetTgApid */
        if ((tgInf->mGObjTg.mSPrm & dCcG_TgSPrm_NoConHit_e) &&
            !(atInf->mGObjAt.mSPrm & dCcG_AtSPrm_StopNoConHit_e) &&
            tgGStts->mTgOldApid == atStts->m_apid)
            return true; /* ChkTgNoConHit, ChkAtStopNoConHit */
    }
    return false;
}

/* 025176B8 */
void dCcS::SetCoGObjInf(u32 co1Set, u32 co2Set, dCcD_GObjInf* co1Obj, dCcD_GObjInf* co2Obj,
                        cCcD_Stts* co1Stts, cCcD_Stts* co2Stts, dCcD_GStts* co1GStts) {
    dCcD_GStts* co2GStts = stackArgsHD()->tgSttsAsGSttsHD();
    if (co1Set) {
        co1Obj->mGObjCo.mAc = nullptr; /* SetCoHitApid: actor cache cleared */
        co1Obj->mGObjCo.mApid = co2Stts->m_apid;
        if (co2GStts->mFlag & dCcD_GStts::Stts_NoActor_e)
            co1Obj->mGObjCo.mRPrm =
                co1Obj->mGObjCo.mRPrm | dCcG_CoRPrm_HitNoActor_e; /* OnCoHitNoActor */
    }
    if (co2Set) {
        co2Obj->mGObjCo.mAc = nullptr;
        co2Obj->mGObjCo.mApid = co1Stts->m_apid;
        if (co1GStts->mFlag & dCcD_GStts::Stts_NoActor_e)
            co2Obj->mGObjCo.mRPrm = co2Obj->mGObjCo.mRPrm | dCcG_CoRPrm_HitNoActor_e;
    }
    if (co1Set) {
        u32 cb = co1Obj->mGObjCo.mHitCallback;
        if (cb != 0) {
            cCcD_Stts *s1 = co1Obj->mStts, *s2 = co2Obj->mStts;
            fopAc_ac_c* ac1 = GetAc(s1);
            fopAc_ac_c* ac2 = GetAc(s2);
            gabi::call_ptr(cb, ac1, co1Obj, ac2, co2Obj);
        }
    }
    if (co2Set) {
        u32 cb = co2Obj->mGObjCo.mHitCallback;
        if (cb != 0) {
            cCcD_Stts *s2 = co2Obj->mStts, *s1 = co1Obj->mStts;
            fopAc_ac_c* ac2 = GetAc(s2);
            fopAc_ac_c* ac1 = GetAc(s1);
            gabi::call_ptr(cb, ac2, co2Obj, ac1, co1Obj);
        }
    }
}

// HD: weight and hit flags retain the complete incoming registers.
/* 0251761C */
s32 dCcS::GetRank(u32 weight) {
    if ((s32)weight == 0xFF)
        return 10;
    if ((s32)weight == 0xFE)
        return 9;
    if (weight >= 0xD9)
        return 8;
    if (weight >= 0xB5)
        return 7;
    if (weight >= 0x91)
        return 6;
    if (weight >= 0x6D)
        return 5;
    if (weight >= 0x49)
        return 4;
    if (weight >= 0x25)
        return 3;
    if (weight >= 0x02)
        return 2;
    return weight == 1;
}

// HD: retain fused sums and unordered comparisons on the original PPC path.
/* 02517828 */
void dCcS::SetPosCorrect(cCcD_Obj* obj1, cXyz* ppos1, cCcD_Obj* obj2, cXyz* ppos2, f32 cross_len) {
    if (obj1->mObjCo.mSPrm & cCcD_CoSPrm_NoCrr_e)
        return; /* ChkCoNoCrr */
    if (obj2->mObjCo.mSPrm & cCcD_CoSPrm_NoCrr_e)
        return;
    {
        cCcD_Stts* s1 = obj1->mStts;
        if (s1 == 0)
            return;
        cCcD_Stts* s2 = obj2->mStts;
        if (s2 == 0)
            return;
        if (s1->mp_actor != 0 && s1->mp_actor == s2->mp_actor)
            return;
    }
    f32 abs_cross = std::fabs(cross_len);
    if (abs_cross < kZeroEps)
        return;

    gabi::call_ptr((u32)__vtbl->SetCoGCorrectProc, this, obj1, obj2); /* SetCoGCorrectProc */

    bool correctY = false;
    if ((obj1->mObjCo.mSPrm & cCcD_CoSPrm_Sph3DCrr_e) &&
        (obj2->mObjCo.mSPrm & cCcD_CoSPrm_Sph3DCrr_e))
        correctY = true; /* ChkCoSph3DCrr */

    cCcD_Stts* s1 = obj1->mStts;
    cCcD_Stts* s2 = obj2->mStts;
    if (s1->m_weight == 0 && s2->m_weight == 0)
        return;
    if (s1->m_weight == 0xFF && s2->m_weight == 0xFF)
        return;
    s32 rank1 = GetRank(s1->m_weight);
    s32 rank2 = GetRank(s2->m_weight);

    f32 dz = ppos2->z - ppos1->z;
    // HD: rank calls return full registers; preserve PPC address wrap for those results.
    u8 rank = gabi::load<u8>(gabi::ea(&rank_tbl) + (u32)rank1 * 11u + (u32)rank2);
    f32 dx = ppos2->x - ppos1->x;
    u8 invRank = (u8)(100 - rank);
    f32 obj2Weight = (f32)rank * 0.01f;
    f32 obj1Weight = (f32)invRank * 0.01f;

    cXyz vec2; /* sp+0x08 */
    cXyz vec1; /* sp+0x14 */
    f32 dy, len;
    bool main_path;
    if (correctY) {
        dy = ppos2->y - ppos1->y;
        len = std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
        main_path = !(std::fabs(len) < kZeroEps); /* bge: taken on NaN */
    } else {
        f32 zz = dz * dz;
        dy = 0.0f;
        len = std_sqrtf(gabi::fmadds(dx, dx, zz));
        main_path = !(std::fabs(len) < kZeroEps); /* blt: not taken on NaN */
    }
    cXyz* v1 = &vec1;
    cXyz* v2 = &vec2;
    if (main_path) {
        f32 push = cross_len / len;
        dx = dx * push;
        dz = dz * push;
        f32 x1 = -(dx * obj2Weight);
        v2->x = dx * obj1Weight;
        f32 z1 = -(dz * obj2Weight);
        v1->x = x1;
        v1->z = z1;
        v2->z = dz * obj1Weight;
        f32 y1;
        if (correctY) {
            dy = dy * push;
            y1 = -(dy * obj2Weight);
            v1->y = y1;
            v2->y = dy * obj1Weight;
        } else {
            y1 = 0.0f;
            v2->y = 0.0f;
            v1->y = y1;
        }
        cCcD_Stts_PlusCcMoveHD(obj1->mStts, x1, y1, z1);
    } else {
        v2->z = 0.0f;
        v2->y = 0.0f;
        v1->y = 0.0f;
        v1->z = 0.0f;
        f32 x1;
        if (!(abs_cross < kZeroEps)) {
            x1 = -(cross_len * obj2Weight);
            v1->x = x1;
            v2->x = cross_len * obj1Weight;
        } else {
            x1 = -obj2Weight;
            v2->x = obj1Weight;
            v1->x = x1;
        }
        cCcD_Stts_PlusCcMoveHD(obj1->mStts, x1, 0.0f, 0.0f);
    }
    {
        f32 x2 = v2->x, y2 = v2->y, z2 = v2->z;
        cCcD_Stts_PlusCcMoveHD(obj2->mStts, x2, y2, z2);
    }
    PSVECAdd(ppos1, v1, ppos1);
    PSVECAdd(ppos2, v2, ppos2);
}

/* 0251821C */
void dCcS::CalcParticleAngle(dCcD_GObjInf* atInf, cCcD_Stts* atStts, cCcD_Stts* tgStts,
                             csXyz* o_angle) {
    cXyz vec;
    cXyz* v = &vec;
    v->x = atInf->mGObjAt.mVec.x; /* *GetAtVecP() */
    v->y = atInf->mGObjAt.mVec.y;
    v->z = atInf->mGObjAt.mVec.z;
    bool normalize = !(std::fabs(PSVECMag(v)) < kZeroEps); /* bge: taken on NaN */
    if (!normalize) {
        fopAc_ac_c* atActor = atStts->mp_actor;
        fopAc_ac_c* tgActor = tgStts->mp_actor;
        if (atActor != 0 && tgActor != 0) {
            PSVECSubtract(&tgActor->current.pos, &atActor->current.pos, v); /* current.pos */
            normalize = !(std::fabs(PSVECMag(v)) < kZeroEps);
        }
    }
    if (normalize) {
        PSVECNormalize(v, v);
    } else {
        v->z = 0.0f;
        v->y = -1.0f;
        v->x = 0.0f;
    }
    cM3d_CalcVecZAngleHD(v, o_angle);
}

/* 02517E94 */
void dCcS::ProcAtTgHitmark(u32 b1, u32 b2, cCcD_Obj* atObj, cCcD_Obj* tgObj, dCcD_GObjInf* atInf,
                           dCcD_GObjInf* tgInf, cCcD_Stts* atStts) {

    cCcD_Stts* tgStts = stackArgsHD()->tgStts;
    dCcD_GStts* tgGStts = stackArgsHD()->tgGStts;
    cXyz* pos = stackArgsHD()->hitPos;
    cXyz* ppos = pos;

    if (atInf->mGObjAt.mSPrm & dCcG_AtSPrm_NoHitMark_e)
        return; /* ChkAtNoHitMark */
    if (tgInf->mGObjTg.mSPrm & dCcG_TgSPrm_NoHitMark_e)
        return; /* ChkTgNoHitMark */
    if (tgInf->mGObjTg.mHitMark == dCcG_TgHitMark_Unk255_e)
        return; /* GetTgHitMark */
    if (tgGStts == 0)
        return; /* HD: null check */
    if (tgGStts->mActorPerfTblId != 0xFFFF)
        return; /* ChkNoneActorPerfTblId */

    if (!ChkShield(atObj, tgObj, atInf, tgInf)) {
        u8 mark = atInf->mGObjAt.mHitMark; /* GetAtHitMark */
        if (mark == dCcG_AtHitMark_None_e)
            return;
        if (mark == dCcG_AtHitMark_Unk1_e && tgInf->mGObjTg.mHitMark == 1) {
            dComIfGp_particle_set(dPa_name::ID_IT_JN_PIYOHIT00, ppos); /* ID_IT_JN_PIYOHIT00 */
            return;
        }
        csXyz angle; /* sp+0x3C */
        gabi::call_ptr((u32)__vtbl->CalcParticleAngle, this, atInf, atStts, tgStts,
                       &angle); /* CalcParticleAngle */
        mark = atInf->mGObjAt.mHitMark;
        cXyz flush; /* sp+0x18 */
        if (mark == dCcG_AtHitMark_Big_e) {
            dComIfGp_particle_set(dPa_name::ID_AK_JN_CRITICALHITFLASH,
                                  ppos); /* ID_AK_JN_CRITICALHITFLASH */
            cXyz scale;
            scale.x = 2.0f;
            scale.z = 2.0f;
            scale.y = 2.0f;
            dComIfGp_particle_set(dPa_name::ID_AK_JN_OK, ppos, &angle, &scale); /* ID_AK_JN_OK */
        } else if (mark == dCcG_AtHitMark_Unk1_e) {
            dComIfGp_particle_set(dPa_name::ID_AK_JN_OK, ppos, &angle);
        } else {
            dComIfGp_particle_set(mark, ppos, &angle);
        }
        flush.x = ppos->x;
        flush.y = ppos->y;
        flush.z = ppos->z;
        dKy_SordFlush_setHD(&flush, 1);
    } else {
        if (tgInf->mGObjTg.mHitMark == dCcG_TgHitMark_None_e)
            return;
        {
            cXyz flush; /* sp+0x30 */
            flush.x = ppos->x;
            flush.y = ppos->y;
            flush.z = ppos->z;
            dKy_SordFlush_setHD(&flush, 0);
        }
        csXyz angle; /* sp+0x44 */
        gabi::call_ptr((u32)__vtbl->CalcParticleAngle, this, atInf, atStts, tgStts, &angle);
        u8 mark = tgInf->mGObjTg.mHitMark;
        dComIfGp_particle_set(mark, ppos, &angle);
    }
}

/* 02518328 */
void dCcS::SetAtTgGObjInf(u32 setAt, u32 setTg, cCcD_Obj* atObj, cCcD_Obj* tgObj,
                          dCcD_GObjInf* atInf, dCcD_GObjInf* tgInf, cCcD_Stts* atStts) {

    cCcD_Stts* tgStts = stackArgsHD()->tgStts;
    dCcD_GStts* atGStts = stackArgsHD()->atGStts;
    dCcD_GStts* tgGStts = stackArgsHD()->tgGStts;
    cXyz* hitPos = stackArgsHD()->hitPos;

    u32 shield = ChkShield(atObj, tgObj, atInf, tgInf);
    if (setAt) {
        atInf->mGObjAt.mHitPos.copy(*hitPos);           /* SetAtHitPos */
        atInf->mGObjAt.mRVec.copy(tgInf->mGObjTg.mVec); /* SetAtRVec(*GetTgVecP()) */
        if (atGStts != 0 && atGStts->mTgSpl == 0)
            atGStts->mTgSpl = tgInf->mGObjTg.mSpl; /* TgSpl */
        u32 apid = tgStts->m_apid;
        atInf->mGObjAt.mAc = nullptr; /* SetAtHitApid */
        atInf->mGObjAt.mApid = apid;
        if (shield)
            atInf->mGObjAt.mRPrm =
                atInf->mGObjAt.mRPrm | dCcG_AtRPrm_ShieldHit_e; /* OnAtShieldHit */
        if (tgGStts != 0 && (tgGStts->mFlag & dCcD_GStts::Stts_NoActor_e))
            atInf->mGObjAt.mRPrm =
                atInf->mGObjAt.mRPrm | dCcG_AtRPrm_HitNoActor_e; /* OnAtHitNoActor */
    }
    if (setTg) {
        tgInf->mGObjTg.mHitPos.copy(*hitPos);           /* SetTgHitPos */
        tgInf->mGObjTg.mRVec.copy(atInf->mGObjAt.mVec); /* SetTgRVec(*GetAtVecP()) */
        if (atGStts != 0) {                             /* HD: the rest needs the at GStts */
            if (tgGStts != 0 && atGStts->mAtSpl == 0)
                tgGStts->mAtSpl = atInf->mGObjAt.mSpl; /* AtSpl */
            u32 apid = atStts->m_apid;
            tgInf->mGObjTg.mAc = nullptr; /* SetTgHitApid */
            tgInf->mGObjTg.mApid = apid;
            if (shield)
                tgInf->mGObjTg.mRPrm =
                    tgInf->mGObjTg.mRPrm | dCcG_TgRPrm_ShieldHit_e; /* OnTgShieldHit */
            if (atGStts->mFlag & dCcD_GStts::Stts_NoActor_e)
                tgInf->mGObjTg.mRPrm =
                    tgInf->mGObjTg.mRPrm | dCcG_TgRPrm_HitNoActor_e; /* OnTgHitNoActor */
        }
    }
    if (setAt) {
        u32 cb = atInf->mGObjAt.mHitCallback;
        if (cb != 0) {
            cCcD_Stts *sa = atInf->mStts, *sb = tgInf->mStts;
            fopAc_ac_c* acA = GetAc(sa);
            fopAc_ac_c* acB = GetAc(sb);
            gabi::call_ptr(cb, acA, atInf, acB, tgInf);
        }
    }
    if (setTg) {
        u32 cb = tgInf->mGObjTg.mHitCallback;
        if (cb != 0) {
            cCcD_Stts *sb = tgInf->mStts, *sa = atInf->mStts;
            fopAc_ac_c* acB = GetAc(sb);
            fopAc_ac_c* acA = GetAc(sa);
            gabi::call_ptr(cb, acB, tgInf, acA, atInf);
        }
    }
    if (setAt && setTg &&
        (!(atInf->mGObjAt.mEffCounter > 0) || !(tgInf->mGObjTg.mEffCounter > 0))) {
        dCcD_GAtTgCoCommonBase_SetEffCounterTimerHD(&atInf->mGObjAt);
        dCcD_GAtTgCoCommonBase_SetEffCounterTimerHD(&tgInf->mGObjTg);
        dCcS_ProcAtTgHitmarkStackHD(this, setAt, setTg, atObj, tgObj, atInf, tgInf, atStts, tgStts,
                                    atGStts, tgGStts, hitPos); /* ProcAtTgHitmark */
    }
}

// HD: reference arguments use pointers; one frame preserves constructor payloads and layout.
/* 025187D8 */
bool dCcS::ChkCamera(cXyz* start, cXyz* end, f32 radius, fopAc_ac_c* ac1, fopAc_ac_c* ac2) {
    s32 count = mObjCoCount;
    if (count <= 0)
        return false;
    gptr<cCcD_Obj>* it = mpObjCo;
    // HD: count and end address wrap at 32 bits, including an uninitialized null receiver.
    u32 end_it = gabi::ea(it) + (u32)count * sizeof(*it);
    dCcS_cameraFrameHD frame;
    cCcD_DivideInfo* div = &frame.divideInfo;
    be<f32>* crossLen = &frame.cross_len;
    cCcD_CpsAttr* cps = &frame.cpsAttr;
    cps->__vtbl_shape = gabi::ea(&l_shapeVtable);
    cps->mAab.__vtbl = gabi::ea(&l_aabVtable);
    cM3dGCps_constructHD(&cps->mCps);
    cps->__vtbl_shape = gabi::ea(&l_cpsAttrVtable);
    cps->mCps.__vtbl = gabi::ea(&l_cpsVtable);
    cM3dGCps_SetHD(&cps->mCps, start, end, radius);
    cCcD_CpsAttr_CalcAabBoxHD(cps);
    cCcD_DivideInfo_constructHD(div);
    cCcD_DivideArea_CalcDivideInfoOverAreaHD(&mDivideArea, div, cps);
    for (; gabi::ea(it) < end_it; ++it) {
        cCcD_Obj* obj = *it;
        if (!(obj->mObjCo.mSPrm & cCcD_CoSPrm_Set_e))
            continue;
        if (GetAc(obj->mStts) == ac1)
            continue;
        if (GetAc(obj->mStts) == ac2)
            continue;
        if (!cCcD_DivideInfo_ChkHD(&obj->mDivideInfo, div))
            continue;
        obj = *it;
        void* shape = gabi::call_ptr<void*>(objectVtableHD(obj)->GetShapeAttr, obj);
        if (!shape)
            continue;
        if (cCcD_CpsAttr_CrossCoHD(cps, shape, crossLen)) {
            cCcD_DivideInfo_destroyHD(div, 2);
            cCcD_CpsAttr_destroyHD(cps, 2);
            return true;
        }
    }
    cCcD_DivideInfo_destroyHD(div, 2);
    cCcD_CpsAttr_destroyHD(cps, 2);
    return false;
}

/* 02518B24 */
void dCcS::MoveAfterCheck() {}

/* 02518798 */
void dCcS::Move() { cCcS::Move(); }

/* 0251879C */
void dCcS::Draw() {
    cCcS::DrawClear();
    dCcMassS_Mng_ClearHD(&mMass_Mng);
}

/* 025187D0 */
void dCcS::MassClear() { dCcMassS_Mng_ClearHD(&mMass_Mng); }

/* 02518A80 */
bool dCcS::ChkNoHitGCo(cCcD_Obj* obj1, cCcD_Obj* obj2) {
    dCcD_GObjInf* inf1 = GetGObjInf(obj1);
    dCcD_GObjInf* inf2 = GetGObjInf(obj2);
    u32 co1 = inf1->mGObjCo.mSPrm,
        co2 = inf2->mGObjCo.mSPrm; /* dCcD_GObjCo SPrm: 1 AtLasso, 2 TgLasso */
    if ((co1 & dCcG_CoSPrm_AtLasso_e) && !(co2 & dCcG_CoSPrm_TgLasso_e))
        return true;
    if ((co2 & dCcG_CoSPrm_AtLasso_e) && !(co1 & dCcG_CoSPrm_TgLasso_e))
        return true;
    return false;
}

// HD: GHS nullable construction returns the guest address in the full-width result register.
/* 02518704 */
u32 dCcS__ct(dCcS* p) {
    if (!p) {
        p = static_cast<dCcS*>(operator_new(sizeof(dCcS)));
        if (!p)
            return 0;
    }
    cCcS_constructHD(p);
    p->__vtbl = &l_dCcSVtable;
    dCcMassS_Mng_constructHD(&p->mMass_Mng);
    return gabi::ea(p);
}

// HD: compiler-generated and template code of this translation unit.
/* 025189E4 */
void __sinit_d_cc_s_cpp() { sinit_header_statics(gc_tu::headerState, gc_tu::runtimeObjects); }

/* 02518A78 */
bool cCcS::ChkNoHitGAtTg(cCcD_GObjInf* atInf, cCcD_GObjInf* tgInf, cCcD_GStts* atGStts,
                         cCcD_GStts* tgGStts) {
    return false;
}
// clang-format off
/* WWHD glue
data rank_tbl 101D5760
data l_dCcSVtable 1004B3E8
data l_shapeVtable 100015A8
data l_aabVtable 1004B3AC
data l_cpsAttrVtable 10001678
data l_cpsVtable 10001718
ext 028E8DAC void PSVECSubtract(const cXyz*,const cXyz*,cXyz*)
ext 028E8E10 f32 PSVECMag(const cXyz*)
ext 028E8EF0 void PSVECNormalize(const cXyz*,cXyz*)
ext 02017300 void cM3d_CalcVecZAngleHD(cXyz*,csXyz*)
ext 0255F554 void dKy_SordFlush_setHD(cXyz*,s32)
ext 0200BE28 void cCcD_Stts_PlusCcMoveHD(cCcD_Stts*,f32,f32,f32)
ext 0200E138 void cCcS_constructHD(cCcS*)
ext 0200E1C4 void cCcS::Ct()
ext 0200E23C void cCcS::Dt()
ext 0200E558 void cCcS::Move()
ext 0200E5BC void cCcS::DrawClear()
ext 02516AB0 void dCcMassS_Mng_constructHD(dCcS_massStorageHD*)
ext 02516A78 void dCcMassS_Mng_CtHD(dCcS_massStorageHD*)
ext 025169A8 void dCcMassS_Mng_ClearHD(dCcS_massStorageHD*)
ext 02018150 void cM3dGCps_constructHD(cCcD_capsuleHD*)
ext 020181B0 void cM3dGCps_SetHD(cCcD_capsuleHD*,cXyz*,cXyz*,f32)
ext 0200C2D8 void cCcD_CpsAttr_CalcAabBoxHD(cCcD_CpsAttr*)
ext 0200B600 void cCcD_CpsAttr_destroyHD(cCcD_CpsAttr*,s32)
ext 0200C20C BOOL cCcD_CpsAttr_CrossCoHD(cCcD_CpsAttr*,void*,be<f32>*)
ext 0200B6C0 void cCcD_DivideInfo_constructHD(cCcD_DivideInfo*)
ext 0200B708 void cCcD_DivideInfo_destroyHD(cCcD_DivideInfo*,s32)
ext 0200B71C BOOL cCcD_DivideInfo_ChkHD(void*,cCcD_DivideInfo*)
ext 0200BAC0 void cCcD_DivideArea_CalcDivideInfoOverAreaHD(cCcD_DivideArea*,cCcD_DivideInfo*,cCcD_CpsAttr*)
ext 02516204 u32 dCcD_GObjInf_ChkAtNoGuardHD(dCcD_GObjInf*)
ext 02515C40 void dCcD_GAtTgCoCommonBase_SetEffCounterTimerHD(dCcD_GObjCommon*)
ext 02517E94 void dCcS_ProcAtTgHitmarkStackHD(dCcS*,u32,u32,cCcD_Obj*,cCcD_Obj*,dCcD_GObjInf*,dCcD_GObjInf*,cCcD_Stts*,cCcD_Stts*,dCcD_GStts*,dCcD_GStts*,cXyz*)
*/
// clang-format on
