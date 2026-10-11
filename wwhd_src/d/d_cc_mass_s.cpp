/* Collision mass manager and camera capsule tests, verified against WWHD. */
#include "d/d_cc_mass_s.h"
#include <cmath>
static f32 l_zero = 0.0;
static f32 l_negativeInfinity = -1000000000.0;
static f32 l_replaceChance = 0.5;
static f32 l_one = 1.0;
static f32 l_infinity = 1000000000.0;
static f32 l_cameraHeightMargin = 20.0;
static f32 l_zeroTolerance = 3.814697265625e-06;
static MassObjectVtable* objectVtable(u32 address) {
    return gabi::at<MassObjectVtable>(address);
}
static MassShapeVtable* shapeVtable(u32 address) {
    return gabi::at<MassShapeVtable>(address);
}
// HD: virtual shape lookup and explicit live-in register setup are part of the guest ABI.
static MassShape* shape(MassCollisionObj* obj) {
    return gabi::call_ptr<MassShape*>(objectVtable(obj->__vtbl)->GetShapeAttr, obj);
}
// HD: allocating constructor adapters retain their used r3 result and null allocation behavior.
/* 025168E8 */
void* dCcMassS_Obj_constructor(void* p) {
    dCcMassS_Obj* a = (dCcMassS_Obj*)p;
    if (!a)
        a = (dCcMassS_Obj*)operator_new(0x18);
    if (a) {
        a->mpCallback = 0;
        a->mPriority = 0;
        a->__vtbl = 0x1004B30C;
        a->mpObj = 0;
        massDivideCtor(&a->mDivideInfo);
    }
    return a;
}
/* 0251694C */
void* dCcMassS_HitInf_constructor(void* p) {
    dCcMassS_HitInf* a = (dCcMassS_HitInf*)p;
    if (!a)
        a = (dCcMassS_HitInf*)operator_new(0x14);
    if (a) {
        a->__vtbl = 0x1004B31C;
        a->mpArea = 0;
        a->mpAtObj = 0;
        a->mCoHitLen = l_zero;
        a->mpCoObj = 0;
    }
    return a;
}
/* 025169A8 */
void dCcMassS_MngNative::Clear() {
    mMassObjCount = 0;
    mMassAreaCount = 0;
    for (u32 i = 0; i < 5; i++) {
        dCcMassS_Obj* o = &mMassObjs[i];
        o->mpObj = 0;
        o->mPriority = 5;
        o->mpCallback = 0;
        o->mDivideInfo.mRangeBits = 0;
    }
    for (u32 i = 0; i < 2; i++) {
        dCcMassS_Obj* o = &mMassAreas[i];
        o->mpObj = 0;
        o->mPriority = 5;
        o->mpCallback = 0;
        o->mDivideInfo.mRangeBits = 0;
    }
    f32 z = l_zero;
    massSetRadius(&mCylAttr.mCyl, z);
    massSetHeight(&mCylAttr.mCyl, z);
    field_0x128 = 0;
    mResultCamBit = 4;
}
/* 02516A78 */
void dCcMassS_MngNative::Ct() {
    f32 z = l_zero, n = l_negativeInfinity;
    mFlag = 0;
    mCamBottomPos.x = z;
    mCamBottomPos.y = n;
    mResultCam = 0;
    mCamBottomPos.z = z;
    mCamTopPos.z = z;
    mCamTopPos.x = z;
    mCamTopPos.y = n;
    massClearEntry(this);
}
/* 02516AB0 */
void* dCcMassS_MngNative_constructor(void* p) {
    dCcMassS_MngNative* a = (dCcMassS_MngNative*)p;
    if (!a)
        a = (dCcMassS_MngNative*)operator_new(0x1a0);
    if (a) {
        a->__vtbl = 0x1004B32C;
        massDivideAreaCtor(&a->mDivideArea);
        a->mMassObjCount = 0;
        massArrayCtor(a->mMassObjs, 5, 0x18, dCcMassS_Obj_constructor);
        a->mMassAreaCount = 0;
        massArrayCtor(a->mMassAreas, 2, 0x18, dCcMassS_Obj_constructor);
        a->mCylAttr.mShape.mAab.__vtbl = 0x1004B2FC;
        a->mCylAttr.mShape.__vtbl = 0x100015A8;
        massCylinderCtor(&a->mCylAttr.mCyl);
        a->mResultCamBit = 0;
        a->mResultCam = 0;
        a->mCylAttr.mCyl.__vtbl = 0x100017D8;
        a->field_0x128 = 0;
        a->mCylAttr.mShape.__vtbl = 0x10001738;
        a->mFlag = 0;
        massZeroMemory(&a->mCamTopPos, 12);
        f32 z = l_zero;
        a->mCamTopDist = z;
        massZeroMemory(&a->mCamBottomPos, 12);
        a->mCpsAttr.mShape.__vtbl = 0x100015A8;
        a->mCpsAttr.mShape.mAab.__vtbl = 0x1004B2FC;
        a->mCamBottomDist = z;
        massCapsuleCtor(&a->mCpsAttr.mCps);
        a->mCpsAttr.mShape.__vtbl = 0x10001678;
        a->mCpsAttr.mCps.__vtbl = 0x10001718;
        massDivideCtor(&a->mDivideInfo);
        massCtEntry(a);
    }
    return a;
}
/* 02516C14 */
void dCcMassS_MngNative::Set(MassCollisionObj* obj, u32 priority) {
    dCcMassS_Obj* o;
    s32 count = (s32)mMassObjCount;
    if (count >= 5) {
        for (u32 i = 0; i < 5; i++) {
            o = &mMassObjs[i];
            s32 old = o->mPriority;
            if (old > (s32)priority ||
                (old == (s32)priority && massRandom(l_one) < l_replaceChance)) {
                o->mPriority = priority;
                o->mpObj = obj;
                o->mpCallback = 0;
                return;
            }
        }
    } else {
        o = &mMassObjs[(u32)count];
        o->mpObj = obj;
        o->mPriority = priority;
        o->mpCallback = 0;
        mMassObjCount = mMassObjCount + 1;
    }
}
/* 02516D48 */
void dCcMassS_MngNative::SetAreaChk(MassCollisionObj* obj, u32 priority, void* cb) {
    dCcMassS_Obj* o;
    s32 count = (s32)mMassAreaCount;
    if (count >= 2) {
        for (u32 i = 0; i < 2; i++) {
            o = &mMassAreas[i];
            if (o->mPriority > priority) {
                o->mPriority = priority;
                o->mpCallback = gabi::ea(cb);
                o->mpObj = obj;
                return;
            }
        }
    } else {
        o = &mMassAreas[(u32)count];
        o->mpObj = obj;
        o->mPriority = priority;
        o->mpCallback = gabi::ea(cb);
        mMassAreaCount = mMassAreaCount + 1;
    }
}
/* 02516DB4 */
void dCcMassS_MngNative::SetCam(void* cps) {
    massSetCapsule(&mCpsAttr.mCps, cps);
    mResultCam = 0;
    mFlag = mFlag | massFlag::CAMERA_ENABLED;
}
/* 02516DF8 */
u32 dCcMassS_MngNative::GetResultCam() {
    return mResultCam;
}
/* 02516E00 */
void dCcMassS_MngNative::GetCamTopPos(cXyz* out) {

    *out = mCamTopPos;
}
/* 02516E1C */
void dCcMassS_MngNative::Prepare() {
    // Dispatch preserves HD argument-register setup as well as the receiver.
    // Targets may consume live-in r4; no nolivein exemption is used.
    gabi::Local<MassAab> area;
    area->__vtbl = 0x1004B2FC;
    massClearAab(area.get());
    for (u32 group = 0; group < 2; group++) {
        dCcMassS_Obj* base = group ? mMassAreas : mMassObjs;
        be<s32>* count = group ? &mMassAreaCount : &mMassObjCount;
        if (!group)
            gabi::cpu->r[4] = gabi::ea(base + (u32)*count);
        // GHS compares wrapping 32-bit guest addresses, including after callbacks alter counts.
        for (u32 cursor = gabi::ea(base);
             cursor < u32(gabi::ea(base) + (u32)*count * sizeof(dCcMassS_Obj));
             cursor += sizeof(dCcMassS_Obj)) {
            dCcMassS_Obj* o = gabi::at<dCcMassS_Obj>(cursor);
            MassCollisionObj* obj = o->mpObj;
            if (!obj)
                massAssert("d_cc_mass_s.cpp", group ? 0x69 : 0x5d,
                           group ? "parea != (0)" : "pobj != (0)");
            u32 v = obj->__vtbl, target = objectVtable(v)->GetShapeAttr;
            if (!group) {
                gabi::cpu->r[5] = v;
                gabi::cpu->r[6] = target;
            } else {
                gabi::cpu->r[10] = v;
            }
            MassShape* s = gabi::call_ptr<MassShape*>(target, obj);
            u32 sv = s->__vtbl, calc = shapeVtable(sv)->CalcAabBox;
            if (!group) {
                gabi::cpu->r[7] = sv;
                gabi::cpu->r[8] = calc;
            } else
                gabi::cpu->r[4] = sv;
            gabi::call_ptr<void>(calc, s);
            massExpandAab(area.get(), s);
        }
    }
    if (mFlag & massFlag::CAMERA_ENABLED) {
        massCalcCapsuleAab(&mCpsAttr);
        massExpandAab(area.get(), &mCpsAttr);
    }
    massSetArea(this, area.get());
    for (u32 group = 0; group < 2; group++) {
        dCcMassS_Obj* base = group ? mMassAreas : mMassObjs;
        be<s32>* count = group ? &mMassAreaCount : &mMassObjCount;
        // GHS compares wrapping 32-bit guest addresses, including after callbacks alter counts.
        for (u32 cursor = gabi::ea(base);
             cursor < u32(gabi::ea(base) + (u32)*count * sizeof(dCcMassS_Obj));
             cursor += sizeof(dCcMassS_Obj)) {
            dCcMassS_Obj* o = gabi::at<dCcMassS_Obj>(cursor);
            MassCollisionObj* obj = o->mpObj;
            if (!obj)
                massAssert("d_cc_mass_s.cpp", group ? 0x88 : 0x7d,
                           group ? "parea != (0)" : "pobj != (0)");
            u32 v = obj->__vtbl, target = objectVtable(v)->GetShapeAttr;
            if (group) {
                gabi::cpu->r[4] = v;
                gabi::cpu->r[5] = target;
            } else
                gabi::cpu->r[6] = v;
            MassShape* s = gabi::call_ptr<MassShape*>(target, obj);
            massCalcDivideInfo(this, &o->mDivideInfo, s, 0);
        }
    }
    if (mFlag & massFlag::CAMERA_ENABLED)
        massCalcDivideInfo(this, &mDivideInfo, &mCpsAttr, 0);
    f32 z = l_zero, n = l_negativeInfinity, d = l_infinity;
    mCamBottomPos.y = n;
    mCamTopPos.z = z;
    mCamBottomPos.x = z;
    mCamTopPos.x = z;
    mCamTopDist = d;
    mCamTopPos.y = n;
    mCamBottomDist = d;
    mCamBottomPos.z = z;
}
// HD: optional hit information, null objects and null GObjInf are guarded before use.
/* 025170D8 */
u32 dCcMassS_MngNative::Chk(cXyz* pos, gptr<fopAc_ac_c>* actor, dCcMassS_HitInf* hit) {
    dCcMassS_HitInf* h = hit;
    u32 result = 0;
    gabi::Local<MassDivideInfo> divide;
    gabi::Local<cXyz> move;
    gabi::Local<be<f32>> len, areaLen, camLen;
    gabi::Local<cXyz> top, atHit;
    massDivideCtor(divide.get());
    *actor = (fopAc_ac_c*)nullptr;
    massSetCenter(&mCylAttr.mCyl, pos);
    massCalcCylinderAab(&mCylAttr);
    massCalcDivideOverArea(this, divide.get(), &mCylAttr);
    f32 z = l_zero;
    if (h) {
        h->mpAtObj = 0;
        h->mpCoObj = 0;
        h->mpArea = 0;
        h->mCoHitLen = z;
    }
    if (field_0x128 & massFlag::AREA) {
        dCcMassS_Obj* base = mMassAreas;
        // GHS compares wrapping 32-bit guest addresses, including after callbacks alter counts.
        for (u32 cursor = gabi::ea(base);
             cursor < u32(gabi::ea(base) + (u32)mMassAreaCount * sizeof(dCcMassS_Obj));
             cursor += sizeof(dCcMassS_Obj)) {
            dCcMassS_Obj* o = gabi::at<dCcMassS_Obj>(cursor);
            if (!massDivideChk(&o->mDivideInfo, divide.get()))
                continue;
            MassCollisionObj* obj = o->mpObj;
            if (!obj)
                massAssert("d_cc_mass_s.cpp", 0xd0, "pobj != (0)");
            MassShape* s = shape(obj);
            if (!(obj->mCoSPrm & massFlag::COLLIDER_SET) ||
                !massCrossCo(&mCylAttr, s, areaLen.get()))
                continue;
            result |= massFlag::RESULT_AREA;
            *actor = obj->mStts->mp_actor;
            if (h)
                h->mpArea = obj;
            u32 cb = o->mpCallback;
            if (cb)
                gabi::call_ptr<void>(cb, obj->mStts->mp_actor.get(), pos, (u32)mResultCamBit);
        }
    }
    {
        dCcMassS_Obj* base = mMassObjs;
        // GHS compares wrapping 32-bit guest addresses, including after callbacks alter counts.
        for (u32 cursor = gabi::ea(base);
             cursor < u32(gabi::ea(base) + (u32)mMassObjCount * sizeof(dCcMassS_Obj));
             cursor += sizeof(dCcMassS_Obj)) {
            dCcMassS_Obj* o = gabi::at<dCcMassS_Obj>(cursor);
            if (!massDivideChk(&o->mDivideInfo, divide.get()))
                continue;
            MassCollisionObj* obj = o->mpObj;
            if (!obj) {
                massAssert("d_cc_mass_s.cpp", 0xf6, "pobj != (0)");
                continue;
            }
            MassGObjInf* g =
                gabi::call_ptr<MassGObjInf*>(objectVtable(obj->__vtbl)->GetGObjInf, obj);
            MassShape* s = gabi::call_ptr<MassShape*>(
                objectVtable(obj->__vtbl)->GetShapeAttr, obj, gabi::at<void>(obj->__vtbl),
                gabi::at<void>(objectVtable(obj->__vtbl)->GetShapeAttr));
            if (g && (obj->mAtSPrm & massFlag::COLLIDER_SET) &&
                !(g->mAtSPrm & massFlag::AT_NO_MASS) && massCrossAtTg(&mCylAttr, s, atHit.get()) &&
                (field_0x128 & massFlag::ATTACK)) {
                *actor = obj->mStts->mp_actor;
                result |= massFlag::RESULT_ATTACK;
                if (h)
                    h->mpAtObj = obj;
            }
            if (!(obj->mCoSPrm & massFlag::COLLIDER_SET) || !massCrossCo(&mCylAttr, s, len.get()) ||
                !(field_0x128 & massFlag::COLLISION))
                continue;
            fopAc_ac_c* act = obj->mStts->mp_actor;
            *actor = act;
            result |= massFlag::RESULT_COLLISION;
            if (field_0x128 & massFlag::MOVE) {
                massVecSubtract(&act->current.pos, pos, move.get());
                move->y = z;
                f32 mag = massVecMag(move.get());
                if (std::fabs(mag) < l_zeroTolerance) {
                    move->x = l_one;
                    massPlusCcMove(obj->mStts.get(), (f32)move->x, (f32)move->y, (f32)move->z);
                } else {
                    f32 scale = (f32)((f64)(f32)*len.get() / (f64)mag);
                    massVecScale(move.get(), move.get(), scale);
                    massPlusCcMove(obj->mStts.get(), (f32)move->x, (f32)move->y, (f32)move->z);
                }
            }
            if (h) {
                h->mpCoObj = obj;
                h->mCoHitLen = (f32)*len.get();
            }
        }
    }
    if ((mFlag & massFlag::CAMERA_ENABLED) && massDivideChk(&mDivideInfo, divide.get()) &&
        massCrossCapsule(&mCylAttr, &mCpsAttr, camLen.get())) {
        u32 shift = (u32)mResultCamBit + 1;
        u32 bit = (shift & 0x20) ? 0 : 1u << (shift & 31);
        u32 cam = mResultCam | 1 | bit;
        mResultCam = cam;
        if (cam & 0xa) {
            f32 y = gabi::fadds_ppc((f32)pos->y, mCylAttr.mCyl.mHeight);
            f32 raised = gabi::fadds_ppc(y, l_cameraHeightMargin);
            top->x = (f32)pos->x;
            top->y = y;
            top->z = (f32)pos->z;
            if (mCamTopPos.y < raised) {
                f32 dist = massSquareDistance(top.get(), &mCpsAttr.mCps.mStart);
                if (mCamTopDist > dist) {
                    // GHS preserves payload bits and stores y, z, x in this order.
                    ((be<u32>*)&mCamTopPos)[1] = ((be<u32>*)top.get())[1];
                    ((be<u32>*)&mCamTopPos)[2] = ((be<u32>*)top.get())[2];
                    ((be<u32>*)&mCamTopPos)[0] = ((be<u32>*)top.get())[0];
                    mCamTopDist = dist;
                }
            }
            if (mCamBottomPos.y < raised) {
                f32 dist = massSquareDistance(top.get(), &mCpsAttr.mCps.mEnd);
                if (mCamBottomDist > dist) {
                    ((be<u32>*)&mCamBottomPos)[1] = ((be<u32>*)top.get())[1];
                    ((be<u32>*)&mCamBottomPos)[2] = ((be<u32>*)top.get())[2];
                    ((be<u32>*)&mCamBottomPos)[0] = ((be<u32>*)top.get())[0];
                    mCamBottomDist = dist;
                }
            }
        }
    }
    massDivideDtor(divide.get(), 2);
    return result;
}
// HD: compiler-generated header statics.
/* 02517588 */
void __sinit_d_cc_mass_s_cpp() {
    sinit_header_statics_rodata(0x1046EFE4, 0x101D573C, 0x1046EFF0, 0x1004B3A0);
}
/* WWHD glue
data l_zero 1004B33C
data l_negativeInfinity 1004B340
data l_replaceChance 1004B344
data l_one 1004B348
data l_infinity 1004B34C
data l_cameraHeightMargin 1004B37C
data l_zeroTolerance 100030B8
ext 0200B6C0 void massDivideCtor(MassDivideInfo* p)
ext 020184DC void massSetRadius(MassCylinder* p,f32 value)
ext 02018428 void massSetHeight(MassCylinder* p,f32 value)
ext 0200B750 void massDivideAreaCtor(MassDivideArea* p)
ext 028EFFD0 void massArrayCtor(dCcMassS_Obj* p,u32 count,u32 size,MassConstructor ctor)
ext 02018590 void massCylinderCtor(MassCylinder* p)
ext 028F521C void massZeroMemory(void* p,u32 size)
ext 02018150 void massCapsuleCtor(MassCapsule* p)
ext 020198D8 f32 massRandom(f32 scale)
ext 02018240 void massSetCapsule(MassCapsule* p,void* cps)
ext 02017DAC void massClearAab(MassAab* p)
ext 02017E98 void massExpandAab(MassAab* p,void* box)
ext 0200C2D8 void massCalcCapsuleAab(MassCpsAttr* p)
ext 0200B7D4 void massSetArea(dCcMassS_MngNative* p,MassAab* box)
ext 0200B8CC void massCalcDivideInfo(dCcMassS_MngNative* p,MassDivideInfo* info,void* shape,u32 flag)
ext 020182E0 void massSetCenter(MassCylinder* p,cXyz* pos)
ext 0200C864 void massCalcCylinderAab(MassCylAttr* p)
ext 0200BAC0 void massCalcDivideOverArea(dCcMassS_MngNative* p,MassDivideInfo* info,MassCylAttr* attr)
ext 0200B71C s32 massDivideChk(MassDivideInfo* a,MassDivideInfo* b)
ext 0200C7EC s32 massCrossCo(MassCylAttr* attr,MassShape* shape,be<f32>* distance)
ext 0200C79C s32 massCrossAtTg(MassCylAttr* attr,MassShape* shape,cXyz* hit)
ext 028E8DAC void massVecSubtract(cXyz* a,cXyz* b,cXyz* out)
ext 028E8E10 f32 massVecMag(cXyz* v)
ext 0200BE28 void massPlusCcMove(MassStatus* p,f32 x,f32 y,f32 z)
ext 028E8E64 void massVecScale(cXyz* in,cXyz* out,f32 scale)
ext 0200C808 s32 massCrossCapsule(MassCylAttr* cyl,MassCpsAttr* cps,be<f32>* distance)
ext 028E8DE8 f32 massSquareDistance(cXyz* a,cXyz* b)
ext 0200B708 void massDivideDtor(MassDivideInfo* p,s32 flags)
ext 0273AA24 void massAssert(const char* file,u32 line,const char* text)
ext 025169A8 void massClearEntry(dCcMassS_MngNative* p)
ext 02516A78 void massCtEntry(dCcMassS_MngNative* p)
str 1004B350 "d_cc_mass_s.cpp" in 02516E1C
str 1004B380 "d_cc_mass_s.cpp" in 025170D8
str 1004B360 "pobj != (0)" in 02516E1C
str 1004B390 "pobj != (0)" in 025170D8
str 1004B36C "parea != (0)"
*/
