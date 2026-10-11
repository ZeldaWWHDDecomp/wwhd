/* Collision-object registration, dispatch and correction. Verified HD behavior. */
#include "SSystem/SComponent/c_cc_s.h"
using gabi::call_ptr;
static const f32 zero = 0.0f;
static const f32 rangeMin = -100000000000000000000000000000000.0f;
static const f32 rangeMax = 100000000000000000000000000000000.0f;
static const f32 minCorrection = 0.008f;
static const f32 tiny = 0.000003814697265625f;
static const f32 one = 1.0f;
static const f32 two = 2.0f;
static const f32 half = 0.5f;
static const f32 piMin = -3.1415927410125732f;
static const f32 piMax = 3.1415927410125732f;
static cCcD_GObjInf *getGObjInf(cCcD_Obj *obj, CcObjectVtable *vtable) {
    return call_ptr<cCcD_GObjInf *>(vtable->GetGObjInf, obj);
}
static cCcD_ShapeAttr *getShapeAttr(cCcD_Obj *obj) {
    return call_ptr<cCcD_ShapeAttr *>(obj->__vtbl->GetShapeAttr, obj);
}
static cCcD_GStts *getGStts(cCcD_Stts *status, CcStatusVtable *vtable) {
    return call_ptr<cCcD_GStts *>(vtable->GetGStts, status);
}
static WeightType GetWt(u32 weight) {
    return weight == 255 ? WeightType_0 : weight == 254 ? WeightType_1 : WeightType_2;
}

/* 0200CD0C */
void cCcSState::ClrAtHitInf() {
    s32 end = mObjAtCount;
    for (s32 i = 0; i < end; ++i) {
        cCcD_Obj *obj = mpObjAt[i].get();
        if (obj != NULL) {
            cCcD_GObjInf *info = getGObjInf(obj, obj->__vtbl.get());
            call_ptr<void>(info->__vtbl->ClrAtHit, info);
            obj = mpObjAt[i].get();
            cCcD_Stts *status = obj->mStts.get();
            if (status != NULL) {
                status = obj->mStts.get();
                call_ptr<void>(status->__vtbl->ClrAt, status);
            }
            end = mObjAtCount;
        }
    }
}

/* 0200CDBC */
void cCcSState::ClrTgHitInf() {
    s32 end = mObjTgCount;
    for (s32 i = 0; i < end; ++i) {
        cCcD_Obj *obj = mpObjTg[i].get();
        if (obj != NULL) {
            cCcD_GObjInf *info = getGObjInf(obj, obj->__vtbl.get());
            call_ptr<void>(info->__vtbl->ClrTgHit, info);
            obj = mpObjTg[i].get();
            cCcD_Stts *status = obj->mStts.get();
            if (status != NULL) {
                status = obj->mStts.get();
                call_ptr<void>(status->__vtbl->ClrTg, status);
            }
            end = mObjTgCount;
        }
    }
}

/* 0200D3B8 */
void cCcSState::ClrCoHitInf() {
    s32 end = mObjCoCount;
    for (s32 i = 0; i < end; ++i) {
        cCcD_Obj *obj = mpObjCo[i].get();
        if (obj != NULL) {
            cCcD_GObjInf *info = getGObjInf(obj, obj->__vtbl.get());
            call_ptr<void>(info->__vtbl->ClrCoHit, info);
            obj = mpObjCo[i].get();
            cCcD_Stts *status = obj->mStts.get();
            if (status != NULL) {
                status = obj->mStts.get();
                status->m_cc_move.z = zero;
                status->m_cc_move.x = zero;
                status->m_cc_move.y = zero;
            }
            end = mObjCoCount;
        }
    }
}

/* 0200CE78 */
u32 cCcSState::ChkNoHitAtTg(cCcD_Obj *at, cCcD_Obj *tg) {
    cCcD_Stts *atStts = at->mStts.get();
    cCcD_Stts *tgStts = tg->mStts.get();
    void *acAt = atStts ? atStts->mp_actor.get() : NULL;
    void *acTg = tgStts ? tgStts->mp_actor.get() : NULL;
    if (acAt && acTg && acAt == acTg)
        return 1;
    if (!((u32)at->mObjAt.mSPrm & (u32)tg->mObjTg.mSPrm & cCcD_AtSPrm_GrpAll_e))
        return 1;
    if (!((u32)at->mObjAt.mType & (u32)tg->mObjTg.mType))
        return 1;
    CcObjectVtable *atVt = at->__vtbl.get();
    CcObjectVtable *tgVt = tg->__vtbl.get();
    CcServerVtable *serverVt = __vtbl.get();
    CcStatusVtable *atSttsVt = atStts->__vtbl.get();
    CcStatusVtable *tgSttsVt = tgStts->__vtbl.get();
    cCcD_GObjInf *atInfo = getGObjInf(at, atVt);
    cCcD_GObjInf *tgInfo = getGObjInf(tg, tgVt);
    cCcD_GStts *atGStts = getGStts(atStts, atSttsVt);
    cCcD_GStts *tgGStts = getGStts(tgStts, tgSttsVt);
    return call_ptr<u32>(serverVt->ChkNoHitGAtTg, this, atInfo, tgInfo, atGStts, tgGStts);
}
/* 0200CFA8 */
void cCcSState::SetAtTgCommonHitInf(cCcD_Obj *obj1, cCcD_Obj *obj2, cXyz *xyz) {
    CcObjectVtable *obj1Vt = obj1->__vtbl.get();
    CcObjectVtable *obj2Vt = obj2->__vtbl.get();
    cCcD_Stts *obj2Stts = obj2->mStts.get();
    cCcD_Stts *obj1Stts = obj1->mStts.get();
    CcServerVtable *serverVt = __vtbl.get();
    CcStatusVtable *stts1Vt = obj1Stts->__vtbl.get();
    CcStatusVtable *stts2Vt = obj2Stts->__vtbl.get();
    cCcD_GObjInf *info1 = getGObjInf(obj1, obj1Vt);
    cCcD_GObjInf *info2 = getGObjInf(obj2, obj2Vt);
    cCcD_GStts *gStts1 = getGStts(obj1Stts, stts1Vt);
    cCcD_GStts *gStts2 = getGStts(obj2Stts, stts2Vt);
    u32 tgFlags = obj2->mObjTg.mSPrm, atFlags = obj1->mObjAt.mSPrm;
    if (call_ptr<u32>(serverVt->ChkAtTgHitAfterCross, this, ((tgFlags >> 4) & 1) ^ 1,
                      ((atFlags >> 4) & 1) ^ 1, info1, info2, obj1Stts, obj2Stts, gStts1, gStts2))
        return;
    tgFlags = obj2->mObjTg.mSPrm;
    atFlags = obj1->mObjAt.mSPrm;
    if (!(tgFlags & cCcD_TgSPrm_NoAtHitInfSet_e)) {
        obj1->mObjAt.mHitObj = obj2;
        obj1->mObjAt.mRPrm = 1;
    }
    if (!(atFlags & cCcD_AtSPrm_NoTgHitInfSet_e)) {
        obj2->mObjTg.mHitObj = obj1;
        obj2->mObjTg.mRPrm = 1;
        call_ptr<void>(__vtbl->CalcTgPlusDmg, this, obj1, obj2, obj1Stts, obj2Stts);
        atFlags = obj1->mObjAt.mSPrm;
    }
    // HD: reload vtables in callback order, retaining full r3 results from virtual getters.
    obj1Vt = obj1->__vtbl.get();
    stts2Vt = obj2Stts->__vtbl.get();
    stts1Vt = obj1Stts->__vtbl.get();
    obj2Vt = obj2->__vtbl.get();
    serverVt = __vtbl.get();
    info1 = getGObjInf(obj1, obj1Vt);
    info2 = getGObjInf(obj2, obj2Vt);
    gStts1 = getGStts(obj1Stts, stts1Vt);
    gStts2 = getGStts(obj2Stts, stts2Vt);
    tgFlags = obj2->mObjTg.mSPrm;
    call_ptr<void>(serverVt->SetAtTgGObjInf, this, ((tgFlags >> 4) & 1) ^ 1,
                   ((atFlags >> 4) & 1) ^ 1, obj1, obj2, info1, info2, obj1Stts, obj2Stts, gStts1,
                   gStts2, xyz);
}
struct CrossState {
    cXyz cross;
    be<u32> initialized;
};
#define crossState (*gabi::at<CrossState>(0x101FF52C))
/* 0200D19C */
void cCcSState::ChkAtTg() {
    gptr<cCcD_Obj> *objTgEnd = mpObjTg + (u32)mObjTgCount;
    ClrAtHitInf();
    ClrTgHitInf();
    gptr<cCcD_Obj> *end = mpObjAt + (u32)mObjAtCount;
    for (gptr<cCcD_Obj> *pObjAt = mpObjAt; pObjAt < end; ++pObjAt) {
        cCcD_Obj *at = pObjAt->get();
        if (!at || !((u32)at->mObjAt.mSPrm & cCcD_AtSPrm_Set_e))
            continue;
        cCcD_ShapeAttr *atShapeAttr = getShapeAttr(at);
        if (!atShapeAttr)
            continue;
        for (gptr<cCcD_Obj> *pObjTg = mpObjTg; pObjTg < objTgEnd; ++pObjTg) {
            cCcD_Obj *tg = pObjTg->get();
            if (!tg || !((u32)tg->mObjTg.mSPrm & cCcD_TgSPrm_Set_e))
                continue;
            if (!cCcD_DivideInfo_Chk(&pObjAt->get()->mDivideInfo, &tg->mDivideInfo))
                continue;
            tg = pObjTg->get();
            at = pObjAt->get();
            if (ChkNoHitAtTg(at, tg))
                continue;
            tg = pObjTg->get();
            cCcD_ShapeAttr *tgShapeAttr = getShapeAttr(tg);
            if (!tgShapeAttr)
                continue;
            if (!crossState.initialized)
                crossState.initialized = 1;
            u32 didCross = call_ptr<u32>(atShapeAttr->__vtbl->CrossAtTg, atShapeAttr, tgShapeAttr,
                                         &crossState.cross);
            at = pObjAt->get();
            bool anyBsRevHit = ((u32)at->mFlags & cCcD_BsRevHit_e) != 0;
            if (!anyBsRevHit) {
                tg = pObjTg->get();
                anyBsRevHit = ((u32)tg->mFlags & cCcD_BsRevHit_e) != 0;
            }
            if (!anyBsRevHit) {
                if (!didCross)
                    continue;
            } else {
                if (didCross)
                    continue;
                cCcD_ShapeAttr *atShape2 = getShapeAttr(at);
                if (!atShape2) {
                    crossState.cross.z = zero;
                    crossState.cross.x = zero;
                    crossState.cross.y = zero;
                } else
                    cM3dGAab_CalcCenter(&atShape2->mAab, &crossState.cross);
                tg = pObjTg->get();
                at = pObjAt->get();
            }
            SetAtTgCommonHitInf(at, tg, &crossState.cross);
        }
        end = mpObjAt + (u32)mObjAtCount;
    }
}
/* 0200D490 */
u32 cCcSState::ChkNoHitCo(cCcD_Obj *obj1, cCcD_Obj *obj2) {
    cCcD_Stts *stts1 = obj1->mStts.get();
    cCcD_Stts *stts2 = obj2->mStts.get();
    void *ac1 = stts1 ? stts1->mp_actor.get() : NULL;
    void *ac2 = stts2 ? stts2->mp_actor.get() : NULL;
    if (ac1 && ac2 && ac1 == ac2)
        return 1;
    u32 flags1 = obj1->mObjCo.mSPrm, flags2 = obj2->mObjCo.mSPrm;
    if (!(flags1 & ((flags2 >> 3) & cCcD_CoSPrm_IGrpAll_e) & cCcD_CoSPrm_IGrpAll_e))
        return 1;
    if (!(((flags1 >> 3) & cCcD_CoSPrm_IGrpAll_e) & flags2))
        return 1;
    return call_ptr<u32>(__vtbl->ChkNoHitGCo, this, obj1, obj2) != 0;
}
/* 0200D54C */
void cCcSState::SetCoCommonHitInf(cCcD_Obj *obj1, cXyz *pos1, cCcD_Obj *obj2, cXyz *pos2,
                                  f32 distance) {
    u32 flags2 = obj2->mObjCo.mSPrm, flags1 = obj1->mObjCo.mSPrm;
    u32 obj2CoHitInfSet = ((flags2 >> 9) & 1) ^ 1, obj1CoHitInfSet = ((flags1 >> 9) & 1) ^ 1;
    if (obj2CoHitInfSet) {
        obj1->mObjCo.mHitObj = obj2;
        obj1->mObjCo.mRPrm = 1;
    }
    if (obj1CoHitInfSet) {
        obj2->mObjCo.mHitObj = obj1;
        obj2->mObjCo.mRPrm = 1;
    }
    if (obj2CoHitInfSet && obj1CoHitInfSet)
        call_ptr<void>(__vtbl->SetPosCorrect, this, obj1, pos1, obj2, pos2, distance);
    CcObjectVtable *obj1Vt = obj1->__vtbl.get();
    cCcD_Stts *stts2 = obj2->mStts.get();
    cCcD_Stts *stts1 = obj1->mStts.get();
    CcObjectVtable *obj2Vt = obj2->__vtbl.get();
    CcStatusVtable *stts1Vt = stts1->__vtbl.get();
    CcServerVtable *serverVt = __vtbl.get();
    CcStatusVtable *stts2Vt = stts2->__vtbl.get();
    cCcD_GObjInf *info1 = getGObjInf(obj1, obj1Vt);
    cCcD_GObjInf *info2 = getGObjInf(obj2, obj2Vt);
    cCcD_GStts *gStts1 = getGStts(stts1, stts1Vt);
    cCcD_GStts *gStts2 = getGStts(stts2, stts2Vt);
    call_ptr<void>(serverVt->SetCoGObjInf, this, obj2CoHitInfSet, obj1CoHitInfSet, info1, info2,
                   stts1, stts2, gStts1, gStts2);
}
struct CrossLength {
    be<f32> value;
};
/* 0200D684 */
void cCcSState::ChkCo() {
    ClrCoHitInf();
    s32 count = mObjCoCount;
    if (count <= 1)
        return;
    gptr<cCcD_Obj> *end = mpObjCo + (u32)count;
    CrossLength crossLen;
    for (gptr<cCcD_Obj> *objCo1 = mpObjCo; objCo1 < end - 1; ++objCo1) {
        cCcD_Obj *obj1 = objCo1->get();
        if (!obj1 || !((u32)obj1->mObjCo.mSPrm & cCcD_CoSPrm_Set_e))
            continue;
        cCcD_ShapeAttr *shape1 = getShapeAttr(obj1);
        if (!shape1)
            continue;
        for (gptr<cCcD_Obj> *objCo2 = objCo1 + 1; objCo2 < end; ++objCo2) {
            cCcD_Obj *obj2 = objCo2->get();
            if (!obj2 || !((u32)obj2->mObjCo.mSPrm & cCcD_CoSPrm_Set_e))
                continue;
            if (!cCcD_DivideInfo_Chk(&objCo1->get()->mDivideInfo, &obj2->mDivideInfo))
                continue;
            obj2 = objCo2->get();
            obj1 = objCo1->get();
            if (ChkNoHitCo(obj1, obj2))
                continue;
            obj2 = objCo2->get();
            cCcD_ShapeAttr *shape2 = getShapeAttr(obj2);
            if (!shape2)
                continue;
            crossLen.value = zero;
            if (!call_ptr<u32>(shape1->__vtbl->CrossCo, shape1, shape2, &crossLen.value))
                continue;
            CcShapeVtable *vt1 = shape1->__vtbl.get();
            CcShapeVtable *vt2 = shape2->__vtbl.get();
            cXyz *pos1 = call_ptr<cXyz *>(vt1->GetCoCP, shape1);
            cXyz *pos2 = call_ptr<cXyz *>(vt2->GetCoCP, shape2);
            f32 distance = crossLen.value;
            obj2 = objCo2->get();
            obj1 = objCo1->get();
            SetCoCommonHitInf(obj1, pos1, obj2, pos2, distance);
        }
    }
}
/* 0200D834 */
void cCcSState::CalcTgPlusDmg(cCcD_Obj *at, cCcD_Obj *, cCcD_Stts *, cCcD_Stts *tgStts) {
    u8 power = at->mObjAt.mAtp, damage = tgStts->m_dmg;
    if (damage < power)
        tgStts->m_dmg = power;
}
static bool nan_bits(f32 x) {
    u32 bits;
    memcpy(&bits, &x, 4);
    return (bits << 1) > 0xff000000u;
}
static bool range_bad(cXyz *v, f32 lo, f32 hi) {
    f32 x = (f32)v->x;
    if (!(lo < x && x < hi))
        return true;
    f32 y = (f32)v->y;
    if (!(lo < y && y < hi))
        return true;
    f32 z = (f32)v->z;
    return !(lo < z && z < hi);
}
/* 0200D84C */
// HD: fused products and the double-precision square root retain the verified rounding.
void cCcSState::SetPosCorrect(cCcD_Obj *obj1, cXyz *ppos1, cCcD_Obj *obj2, cXyz *ppos2,
                              f32 cross_len) {
    if (nan_bits(cross_len))
        assertion("c_cc_s.cpp", 0x26d,
                  "!__generic(cross_len, , ,__isnand,__isnanf,__isnanl,,,)(cross_len)");
    f32 lo = rangeMin, hi = rangeMax;
    if (!(lo < cross_len && cross_len < hi))
        assertion("c_cc_s.cpp", 0x26e, "-1.0e32f < cross_len && cross_len < 1.0e32f");
    if ((u32)obj1->mObjCo.mSPrm & cCcD_CoSPrm_NoCrr_e)
        return;
    if ((u32)obj2->mObjCo.mSPrm & cCcD_CoSPrm_NoCrr_e)
        return;
    cCcD_Stts *obj1Stts = obj1->mStts.get();
    if (!obj1Stts)
        return;
    cCcD_Stts *obj2Stts = obj2->mStts.get();
    if (!obj2Stts)
        return;
    if (obj1Stts->mp_actor.get() && obj1Stts->mp_actor.get() == obj2Stts->mp_actor.get())
        return;
    f32 absCrossLen = std::fabs(cross_len);
    if (absCrossLen < minCorrection)
        return;
    call_ptr<void>(__vtbl->SetCoGCorrectProc, this, obj1, obj2);
    u32 obj1CoFlags = (u32)obj1->mObjCo.mSPrm;
    obj1Stts = obj1->mStts.get();
    u32 obj1WeightUc = obj1Stts->m_weight;
    bool correctY = false;
    if (obj1CoFlags & cCcD_CoSPrm_Sph3DCrr_e)
        correctY = ((u32)obj2->mObjCo.mSPrm & cCcD_CoSPrm_Sph3DCrr_e) != 0;
    obj2Stts = obj2->mStts.get();
    u32 obj2WeightUc = obj2Stts->m_weight;
    WeightType obj1WeightType = GetWt(obj1WeightUc);
    WeightType obj2WeightType = GetWt(obj2WeightUc);
    f32 obj1SrcWeight = (f32)obj1Stts->m_weight, obj2SrcWeight = (f32)obj2Stts->m_weight,
        combinedWeight = obj1SrcWeight + obj2SrcWeight;

    if (std::fabs(combinedWeight) < tiny) {
        obj1SrcWeight = one;
        obj2SrcWeight = one;
        combinedWeight = two;
    }
    f32 invCombinedWeight = one / combinedWeight, obj2Weight, obj1Weight;
    if (obj1WeightType == WeightType_0) {
        if (obj2WeightType == WeightType_0)
            return;
        obj2Weight = zero;
        obj1Weight = one;
    } else if (obj1WeightType == WeightType_1) {
        if (obj2WeightType == WeightType_0) {
            obj2Weight = one;
            obj1Weight = zero;
        } else if (obj2WeightType == WeightType_1) {
            obj2Weight = half;
            obj1Weight = obj2Weight;
        } else {
            obj2Weight = zero;
            obj1Weight = one;
        }
    } else {
        if (obj2WeightType == WeightType_2) {
            obj1Weight = obj1SrcWeight * invCombinedWeight;
            obj2Weight = obj2SrcWeight * invCombinedWeight;
        } else {
            obj2Weight = one;
            obj1Weight = zero;
        }
    }
    f32 dx = (f32)ppos2->x - (f32)ppos1->x, dz = (f32)ppos2->z - (f32)ppos1->z, dy = zero, squared;
    if (correctY) {
        dy = (f32)ppos2->y - (f32)ppos1->y;
        squared = gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy));
    } else
        squared = gabi::fmadds(dx, dx, dz * dz);
    f64 objDistLen = squareRoot((f64)squared);
    cXyz vec1;
    cXyz vec2;
    cXyz *vec1Ptr = &vec1;
    cXyz *vec2Ptr = &vec2;
    if (!(std::fabs(objDistLen) < tiny)) {
        f32 pushFactor = (f32)((f64)cross_len / objDistLen);
        dx *= pushFactor;
        dz *= pushFactor;
        vec2Ptr->x = dx * obj1Weight;
        vec1Ptr->x = -(dx * obj2Weight);
        vec1Ptr->z = -(dz * obj2Weight);
        vec2Ptr->z = dz * obj1Weight;
        if (correctY) {
            dy *= pushFactor;
            vec1Ptr->y = -(dy * obj2Weight);
            vec2Ptr->y = dy * obj1Weight;
        } else {
            vec2Ptr->y = zero;
            vec1Ptr->y = zero;
        }
    } else {
        vec2Ptr->y = zero;
        vec1Ptr->z = zero;
        vec2Ptr->z = zero;
        vec1Ptr->y = zero;
        if (!(absCrossLen < tiny)) {
            vec1Ptr->x = -(cross_len * obj2Weight);
            vec2Ptr->x = cross_len * obj1Weight;
        } else {
            vec2Ptr->x = obj1Weight;
            vec1Ptr->x = -obj2Weight;
        }
    }
    if (nan_bits((f32)vec1Ptr->x))
        assertion("c_cc_s.cpp", 0x32b,
                  "!__generic(vec1.x, , ,__isnand,__isnanf,__isnanl,,,)(vec1.x)");
    if (nan_bits((f32)vec1Ptr->y))
        assertion("c_cc_s.cpp", 0x32c,
                  "!__generic(vec1.y, , ,__isnand,__isnanf,__isnanl,,,)(vec1.y)");
    if (nan_bits((f32)vec1Ptr->z))
        assertion("c_cc_s.cpp", 0x32d,
                  "!__generic(vec1.z, , ,__isnand,__isnanf,__isnanl,,,)(vec1.z)");
    if (nan_bits((f32)vec2Ptr->x))
        assertion("c_cc_s.cpp", 0x32f,
                  "!__generic(vec2.x, , ,__isnand,__isnanf,__isnanl,,,)(vec2.x)");
    if (nan_bits((f32)vec2Ptr->y))
        assertion("c_cc_s.cpp", 0x330,
                  "!__generic(vec2.y, , ,__isnand,__isnanf,__isnanl,,,)(vec2.y)");
    if (nan_bits((f32)vec2Ptr->z))
        assertion("c_cc_s.cpp", 0x331,
                  "!__generic(vec2.z, , ,__isnand,__isnanf,__isnanl,,,)(vec2.z)");
    if (range_bad(vec1Ptr, lo, hi))
        assertion("c_cc_s.cpp", 0x333,
                  "-1.0e32f < vec1.x && vec1.x < 1.0e32f && -1.0e32f < vec1.y && vec1.y < 1.0e32f "
                  "&& -1.0e32f < vec1.z && vec1.z < 1.0e32f");
    if (range_bad(vec2Ptr, lo, hi))
        assertion("c_cc_s.cpp", 0x337,
                  "-1.0e32f < vec2.x && vec2.x < 1.0e32f && -1.0e32f < vec2.y && vec2.y < 1.0e32f "
                  "&& -1.0e32f < vec2.z && vec2.z < 1.0e32f");
    cCcD_Stts_PlusCcMove(obj1->mStts.get(), (f32)vec1Ptr->x, (f32)vec1Ptr->y, (f32)vec1Ptr->z);
    cCcD_Stts_PlusCcMove(obj2->mStts.get(), (f32)vec2Ptr->x, (f32)vec2Ptr->y, (f32)vec2Ptr->z);
    PSVECAdd(ppos1, &vec1, ppos1);
    PSVECAdd(ppos2, &vec2, ppos2);
    if (nan_bits((f32)ppos1->x))
        assertion("c_cc_s.cpp", 0x342,
                  "!__generic(ppos1->x, , ,__isnand,__isnanf,__isnanl,,,)(ppos1->x)");
    if (nan_bits((f32)ppos1->y))
        assertion("c_cc_s.cpp", 0x343,
                  "!__generic(ppos1->y, , ,__isnand,__isnanf,__isnanl,,,)(ppos1->y)");
    if (nan_bits((f32)ppos1->z))
        assertion("c_cc_s.cpp", 0x344,
                  "!__generic(ppos1->z, , ,__isnand,__isnanf,__isnanl,,,)(ppos1->z)");
    if (nan_bits((f32)ppos2->x))
        assertion("c_cc_s.cpp", 0x346,
                  "!__generic(ppos2->x, , ,__isnand,__isnanf,__isnanl,,,)(ppos2->x)");
    if (nan_bits((f32)ppos2->y))
        assertion("c_cc_s.cpp", 0x347,
                  "!__generic(ppos2->y, , ,__isnand,__isnanf,__isnanl,,,)(ppos2->y)");
    if (nan_bits((f32)ppos2->z))
        assertion("c_cc_s.cpp", 0x348,
                  "!__generic(ppos2->z, , ,__isnand,__isnanf,__isnanl,,,)(ppos2->z)");
    if (range_bad(ppos1, lo, hi))
        assertion("c_cc_s.cpp", 0x34a,
                  "-1.0e32f < ppos1->x && ppos1->x < 1.0e32f && -1.0e32f < ppos1->y && ppos1->y < "
                  "1.0e32f && -1.0e32f < ppos1->z && ppos1->z < 1.0e32f");
    if (range_bad(ppos2, lo, hi))
        assertion("c_cc_s.cpp", 0x34e,
                  "-1.0e32f < ppos2->x && ppos2->x < 1.0e32f && -1.0e32f < ppos2->y && ppos2->y < "
                  "1.0e32f && -1.0e32f < ppos2->z && ppos2->z < 1.0e32f");
}

/* 0200E138 */
static cCcSState *cCcS__ct(cCcSState *object) {
    if (!object)
        object = (cCcSState *)operator_new(sizeof(cCcSState));
    if (object) {
        object->__vtbl = gabi::at<CcServerVtable>(0x10001DD8);
        zeroMemory(object->mpObjTg, sizeof(object->mpObjTg));
        zeroMemory(object->mpObjCo, sizeof(object->mpObjCo));
        zeroMemory(object->mpObj, sizeof(object->mpObj));
        object->mObjAtCount = 0;
        object->mObjTgCount = 0;
        object->mObjCoCount = 0;
        object->mObjCount = 0;
        cCcD_DivideArea__ct(&object->mDivideArea);
    }
    return object;
}
/* 0200E1C4 */
void cCcSState::Ct() {
    for (u32 i = 0; i < 0x100; ++i)
        mpObjAt[i] = NULL;
    for (u32 i = 0; i < 0x300; ++i)
        mpObjTg[i] = NULL;
    for (u32 i = 0; i < 0x100; ++i)
        mpObjCo[i] = NULL;
    for (u32 i = 0; i < 0x500; ++i)
        mpObj[i] = NULL;
    mObjAtCount = 0;
    mObjCount = 0;
    mObjTgCount = 0;
    mObjCoCount = 0;
}
/* 0200E23C */
void cCcSState::Dt() { Ct(); }
/* 0200E240 */
void cCcSState::Set(cCcD_Obj *obj) {
    if ((u32)obj->mObjAt.mSPrm & cCcD_AtSPrm_Set_e) {
        s32 n = mObjAtCount;
        if (n < 0x100) {
            mpObjAt[(u32)n] = obj;
            mObjAtCount = (u32)mObjAtCount + 1;
        }
    }
    if ((u32)obj->mObjTg.mSPrm & cCcD_TgSPrm_Set_e) {
        s32 n = mObjTgCount;
        if (n < 0x300) {
            mpObjTg[(u32)n] = obj;
            mObjTgCount = (u32)mObjTgCount + 1;
        }
    }
    if ((u32)obj->mObjCo.mSPrm & cCcD_CoSPrm_Set_e) {
        s32 n = mObjCoCount;
        if (n < 0x100) {
            mpObjCo[(u32)n] = obj;
            mObjCoCount = (u32)mObjCoCount + 1;
        }
    }
    s32 n = mObjCount;
    if (n < 0x500) {
        mpObj[(u32)n] = obj;
        mObjCount = (u32)mObjCount + 1;
    }
}
/* 0200E2F4 */
// HD: removal leaves holes and retains the registered list counts.
void cCcSState::Remove(cCcD_Obj *obj) {
    for (gptr<cCcD_Obj> *p = mpObjAt; p < mpObjAt + (u32)mObjAtCount; ++p)
        if (p->get() == obj)
            *p = NULL;
    for (gptr<cCcD_Obj> *p = mpObjTg; p < mpObjTg + (u32)mObjTgCount; ++p)
        if (p->get() == obj)
            *p = NULL;
    for (gptr<cCcD_Obj> *p = mpObjCo; p < mpObjCo + (u32)mObjCoCount; ++p)
        if (p->get() == obj)
            *p = NULL;
    for (gptr<cCcD_Obj> *p = mpObj; p < mpObj + (u32)mObjCount; ++p)
        if (p->get() == obj)
            *p = NULL;
}
/* 0200E414 */
void cCcSState::CalcArea() {
    CcAab aab;
    aab.__vtbl = 0x1000180C;
    cM3dGAab_ClearForMinMax(&aab);
    gptr<cCcD_Obj> *end = mpObj + (u32)mObjCount;
    for (gptr<cCcD_Obj> *pObj = mpObj; pObj < end; ++pObj) {
        cCcD_Obj *obj = pObj->get();
        if (!obj)
            continue;
        cCcD_ShapeAttr *shape = getShapeAttr(obj);
        if (shape) {
            call_ptr<void>(shape->__vtbl->CalcAabBox, shape);
            cM3dGAab_SetMinMaxAab(&aab, &shape->mAab);
        }
        end = mpObj + (u32)mObjCount;
    }
    cCcD_DivideArea_SetArea(&mDivideArea, &aab);
    end = mpObj + (u32)mObjCount;
    for (gptr<cCcD_Obj> *pObj = mpObj; pObj < end; ++pObj) {
        cCcD_Obj *obj = pObj->get();
        if (!obj)
            continue;
        cCcD_ShapeAttr *shape = getShapeAttr(obj);
        if (shape) {
            obj = pObj->get();
            u32 flag = (u32)obj->mFlags & cCcD_BsRevHit_e;
            cCcD_DivideArea_CalcDivideInfo(&mDivideArea, &obj->mDivideInfo, &shape->mAab, flag);
        }
        end = mpObj + (u32)mObjCount;
    }
}
/* 0200E558 */
void cCcSState::Move() {
    CalcArea();
    ChkAtTg();
    ChkCo();
    call_ptr<void>(__vtbl->MoveAfterCheck, this);
    mObjAtCount = 0;
    mObjCount = 0;
    mObjTgCount = 0;
    mObjCoCount = 0;
}
/* 0200E5BC */
void cCcSState::DrawClear() {
    for (u32 i = 0; i < 0x100; ++i)
        mpObjAt[i] = NULL;
    mObjAtCount = 0;
    for (u32 i = 0; i < 0x300; ++i)
        mpObjTg[i] = NULL;
    mObjTgCount = 0;
    for (u32 i = 0; i < 0x100; ++i)
        mpObjCo[i] = NULL;
    mObjCoCount = 0;
    for (u32 i = 0; i < 0x500; ++i)
        mpObj[i] = NULL;
    mObjCount = 0;
}
/* 0200E630 */
void cCcSState::SetCoGCorrectProc(cCcD_Obj *, cCcD_Obj *) {}
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, pad[2];
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF510))
/* 0200E634 */
static void __sinit_c_cc_s_cpp() {
    headerStatics.words[2] = 0;
    headerStatics.words[0] = 0;
    headerStatics.words[3] = 0;
    headerStatics.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C6B4));
    f32 first = piMin, second = piMax;
    headerStatics.piMin = first;
    headerStatics.piMax = second;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C6C0));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C6CC));
}
/* 0200E6C8 */
void cCcSState::SetCoGObjInf(u32 obj2HitInfSet, u32 obj1HitInfSet, cCcD_GObjInf *info1,
                             cCcD_GObjInf *info2, cCcD_Stts *stts1, cCcD_Stts *stts2,
                             cCcD_GStts *gStts1, cCcD_GStts *gStts2) {}
/* 0200E6CC */
void cCcSState::SetAtTgGObjInf(u32 obj2HitInfSet, u32 obj1HitInfSet, cCcD_Obj *obj1, cCcD_Obj *obj2,
                               cCcD_GObjInf *info1, cCcD_GObjInf *info2, cCcD_Stts *stts1,
                               cCcD_Stts *stts2, cCcD_GStts *gStts1, cCcD_GStts *gStts2,
                               cXyz *xyz1) {}
/* 0200E6D0 */
u32 cCcSState::ChkNoHitGAtTg(cCcD_GObjInf *, cCcD_GObjInf *, cCcD_GStts *, cCcD_GStts *) {
    return 0;
}
/* 0200E6D8 */
u32 cCcSState::ChkAtTgHitAfterCross(u32 obj2HitInfSet, u32 obj1HitInfSet, cCcD_GObjInf *info1,
                                    cCcD_GObjInf *info2, cCcD_Stts *stts1, cCcD_Stts *stts2,
                                    cCcD_GStts *gStts1, cCcD_GStts *gStts2) {
    return 0;
}
/* 0200E6E0 */
u32 cCcSState::ChkNoHitGCo(cCcD_Obj *, cCcD_Obj *) { return 0; }
/* 0200E6E8 */
void cCcSState::MoveAfterCheck() {}

/* WWHD glue (tools/verify/autoglue.py)
data zero 1000181C
data rangeMin 10001824
data rangeMax 10001820
data minCorrection 10001828
data tiny 100030B8
data one 10001838
data two 1000183C
data half 10001840
data piMin 10001DD0
data piMax 10001DD4
str 10001870 in 0200D84C
str 10001844 in 0200D84C
str 1000187C in 0200D84C
str 100018BC in 0200D84C
str 100018FC in 0200D84C
str 1000193C in 0200D84C
str 1000197C in 0200D84C
str 100019BC in 0200D84C
str 100019FC in 0200D84C
str 10001A40 in 0200D84C
str 10001AB8 in 0200D84C
str 10001B30 in 0200D84C
str 10001B74 in 0200D84C
str 10001BB8 in 0200D84C
str 10001BFC in 0200D84C
str 10001C40 in 0200D84C
str 10001C84 in 0200D84C
str 10001CC8 in 0200D84C
str 10001D4C in 0200D84C
ext 0200B71C u32 cCcD_DivideInfo_Chk(cCcD_DivideInfo*,cCcD_DivideInfo*)
ext 02017F0C void cM3dGAab_CalcCenter(CcAab*,cXyz*)
ext 0200BE28 void cCcD_Stts_PlusCcMove(cCcD_Stts*,f32,f32,f32)
ext 028E8D88 void PSVECAdd(cXyz*,cXyz*,cXyz*)
ext 028F4384 f64 squareRoot(f64)
ext 0273AD10 void* operator_new(u32)
ext 028F521C void zeroMemory(void*,u32)
ext 0200B750 void cCcD_DivideArea__ct(cCcD_DivideArea*)
ext 02017DAC void cM3dGAab_ClearForMinMax(CcAab*)
ext 02017E98 void cM3dGAab_SetMinMaxAab(CcAab*,CcAab*)
ext 0200B7D4 void cCcD_DivideArea_SetArea(cCcD_DivideArea*,CcAab*)
ext 0200B8CC void cCcD_DivideArea_CalcDivideInfo(cCcD_DivideArea*,cCcD_DivideInfo*,CcAab*,u32)
ext 0273AA24 void assertion(const char*,u32,const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
