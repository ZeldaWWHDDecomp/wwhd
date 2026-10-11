/* Collision mass manager. GameCube layout is unchanged; HD adds null guards and preserves
 * live-in registers at virtual dispatch. Private shape views avoid pending shared-header work. */
#pragma once
#include "bindings.h"
struct MassAab {
    cXyz mMin, mMax;
    be<u32> __vtbl;
};
struct MassDivideInfo {
    be<u32> mRangeBits, __vtbl;
};
struct MassDivideArea {
    u8 mData[0x40];
};
struct MassShape {
    MassAab mAab;
    be<u32> __vtbl;
};
struct MassCylinder {
    cXyz mCenter;
    be<f32> mRadius, mHeight;
    be<u32> __vtbl;
};
struct MassCapsule {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct MassCylAttr {
    MassShape mShape;
    MassCylinder mCyl;
};
struct MassCpsAttr {
    MassShape mShape;
    MassCapsule mCps;
};
struct MassStatus {
    cXyz m_cc_move;
    gptr<fopAc_ac_c> mp_actor;
};
struct MassCollisionObj {
    be<u32> mAtSPrm;
    u8 mPadding4[0x28];
    be<u32> mCoSPrm;
    u8 mPadding30[0xC];
    be<u32> __vtbl;
    be<u32> mFlags;
    gptr<MassStatus> mStts;
};
struct MassGObjInf {
    u8 mPadding[0x50];
    be<u32> mAtSPrm;
};
struct dCcMassS_Obj {
    gptr<MassCollisionObj> mpObj;
    be<u8> mPriority;
    u8 mPadding5[3];
    be<u32> mpCallback;
    MassDivideInfo mDivideInfo;
    be<u32> __vtbl;
};
struct dCcMassS_HitInf {
    gptr<MassCollisionObj> mpArea, mpAtObj, mpCoObj;
    be<f32> mCoHitLen;
    be<u32> __vtbl;
};
// HD: the shared dCcMassS_Mng is opaque; this unit owns its complete native view.
class dCcMassS_MngNative {
  public:
    /* 0x000 */ MassDivideArea mDivideArea;
    /* 0x040 */ be<s32> mMassObjCount;
    /* 0x044 */ dCcMassS_Obj mMassObjs[5];
    /* 0x0bc */ be<s32> mMassAreaCount;
    /* 0x0c0 */ dCcMassS_Obj mMassAreas[2];
    /* 0x0f0 */ MassCylAttr mCylAttr;
    /* 0x128 */ be<u8> field_0x128;
    /* 0x129 */ be<u8> mResultCamBit;
    u8 mPadding12a[2];
    /* 0x12C */ be<u32> mFlag;
    /* 0x130 */ be<u32> mResultCam;
    /* 0x134 */ cXyz mCamTopPos;
    /* 0x140 */ be<f32> mCamTopDist;
    /* 0x144 */ cXyz mCamBottomPos;
    /* 0x150 */ be<f32> mCamBottomDist;
    /* 0x154 */ MassCpsAttr mCpsAttr;
    /* 0x194 */ MassDivideInfo mDivideInfo;
    /* 0x19c */ be<u32> __vtbl;
    void Clear();
    void Ct();
    void Prepare();
    void Set(MassCollisionObj*, u32);
    void SetAreaChk(MassCollisionObj*, u32, void*);
    void SetCam(void*);
    u32 GetResultCam();
    void GetCamTopPos(cXyz*);
    u32 Chk(cXyz*, gptr<fopAc_ac_c>*, dCcMassS_HitInf*);
};
WWHD_OFFSET(MassAab, mMin, 0x0);
WWHD_OFFSET(MassAab, mMax, 0xc);
WWHD_OFFSET(MassAab, __vtbl, 0x18);
WWHD_SIZE(MassAab, 0x1c);
WWHD_OFFSET(MassDivideInfo, mRangeBits, 0x0);
WWHD_OFFSET(MassDivideInfo, __vtbl, 0x4);
WWHD_SIZE(MassDivideInfo, 0x8);
WWHD_SIZE(MassDivideArea, 0x40);
WWHD_OFFSET(MassShape, mAab, 0x0);
WWHD_OFFSET(MassShape, __vtbl, 0x1c);
WWHD_SIZE(MassShape, 0x20);
WWHD_OFFSET(MassCylinder, mCenter, 0x0);
WWHD_OFFSET(MassCylinder, mRadius, 0xc);
WWHD_OFFSET(MassCylinder, mHeight, 0x10);
WWHD_OFFSET(MassCylinder, __vtbl, 0x14);
WWHD_SIZE(MassCylinder, 0x18);
WWHD_OFFSET(MassCapsule, mStart, 0x0);
WWHD_OFFSET(MassCapsule, mEnd, 0xc);
WWHD_OFFSET(MassCapsule, __vtbl, 0x18);
WWHD_OFFSET(MassCapsule, mRadius, 0x1c);
WWHD_SIZE(MassCapsule, 0x20);
WWHD_OFFSET(MassCylAttr, mShape, 0x0);
WWHD_OFFSET(MassCylAttr, mCyl, 0x20);
WWHD_SIZE(MassCylAttr, 0x38);
WWHD_OFFSET(MassCpsAttr, mShape, 0x0);
WWHD_OFFSET(MassCpsAttr, mCps, 0x20);
WWHD_SIZE(MassCpsAttr, 0x40);
WWHD_OFFSET(MassStatus, mp_actor, 0xc);
WWHD_SIZE(MassStatus, 0x10);
WWHD_OFFSET(MassCollisionObj, mAtSPrm, 0x0);
WWHD_OFFSET(MassCollisionObj, mCoSPrm, 0x2c);
WWHD_OFFSET(MassCollisionObj, __vtbl, 0x3c);
WWHD_OFFSET(MassCollisionObj, mStts, 0x44);
WWHD_SIZE(MassCollisionObj, 0x48);
WWHD_OFFSET(MassGObjInf, mAtSPrm, 0x50);
WWHD_SIZE(MassGObjInf, 0x54);
WWHD_OFFSET(dCcMassS_Obj, mpObj, 0x0);
WWHD_OFFSET(dCcMassS_Obj, mPriority, 0x4);
WWHD_OFFSET(dCcMassS_Obj, mpCallback, 0x8);
WWHD_OFFSET(dCcMassS_Obj, mDivideInfo, 0xc);
WWHD_OFFSET(dCcMassS_Obj, __vtbl, 0x14);
WWHD_SIZE(dCcMassS_Obj, 0x18);
WWHD_OFFSET(dCcMassS_HitInf, mpArea, 0x0);
WWHD_OFFSET(dCcMassS_HitInf, mpAtObj, 0x4);
WWHD_OFFSET(dCcMassS_HitInf, mpCoObj, 0x8);
WWHD_OFFSET(dCcMassS_HitInf, mCoHitLen, 0xc);
WWHD_OFFSET(dCcMassS_HitInf, __vtbl, 0x10);
WWHD_SIZE(dCcMassS_HitInf, 0x14);
WWHD_OFFSET(dCcMassS_MngNative, mDivideArea, 0x0);
WWHD_OFFSET(dCcMassS_MngNative, mMassObjCount, 0x40);
WWHD_OFFSET(dCcMassS_MngNative, mMassObjs, 0x44);
WWHD_OFFSET(dCcMassS_MngNative, mMassAreaCount, 0xbc);
WWHD_OFFSET(dCcMassS_MngNative, mMassAreas, 0xc0);
WWHD_OFFSET(dCcMassS_MngNative, mCylAttr, 0xf0);
WWHD_OFFSET(dCcMassS_MngNative, field_0x128, 0x128);
WWHD_OFFSET(dCcMassS_MngNative, mResultCamBit, 0x129);
WWHD_OFFSET(dCcMassS_MngNative, mFlag, 0x12c);
WWHD_OFFSET(dCcMassS_MngNative, mResultCam, 0x130);
WWHD_OFFSET(dCcMassS_MngNative, mCamTopPos, 0x134);
WWHD_OFFSET(dCcMassS_MngNative, mCamTopDist, 0x140);
WWHD_OFFSET(dCcMassS_MngNative, mCamBottomPos, 0x144);
WWHD_OFFSET(dCcMassS_MngNative, mCamBottomDist, 0x150);
WWHD_OFFSET(dCcMassS_MngNative, mCpsAttr, 0x154);
WWHD_OFFSET(dCcMassS_MngNative, mDivideInfo, 0x194);
WWHD_OFFSET(dCcMassS_MngNative, __vtbl, 0x19c);
WWHD_SIZE(dCcMassS_MngNative, 0x1a0);
using MassConstructor = void* (*)(void*);
void massDivideCtor(MassDivideInfo* p);
void massSetRadius(MassCylinder* p, f32 value);
void massSetHeight(MassCylinder* p, f32 value);
void massDivideAreaCtor(MassDivideArea* p);
void massArrayCtor(dCcMassS_Obj* p, u32 count, u32 size, MassConstructor ctor);
void massCylinderCtor(MassCylinder* p);
void massZeroMemory(void* p, u32 size);
void massCapsuleCtor(MassCapsule* p);
f32 massRandom(f32 scale);
void massSetCapsule(MassCapsule* p, void* cps);
void massClearAab(MassAab* p);
void massExpandAab(MassAab* p, void* box);
void massCalcCapsuleAab(MassCpsAttr* p);
void massSetArea(dCcMassS_MngNative* p, MassAab* box);
void massCalcDivideInfo(dCcMassS_MngNative* p, MassDivideInfo* info, void* shape, u32 flag);
void massSetCenter(MassCylinder* p, cXyz* pos);
void massCalcCylinderAab(MassCylAttr* p);
void massCalcDivideOverArea(dCcMassS_MngNative* p, MassDivideInfo* info, MassCylAttr* attr);
s32 massDivideChk(MassDivideInfo* a, MassDivideInfo* b);
s32 massCrossCo(MassCylAttr* attr, MassShape* shape, be<f32>* distance);
s32 massCrossAtTg(MassCylAttr* attr, MassShape* shape, cXyz* hit);
void massVecSubtract(cXyz* a, cXyz* b, cXyz* out);
f32 massVecMag(cXyz* v);
void massPlusCcMove(MassStatus* p, f32 x, f32 y, f32 z);
void massVecScale(cXyz* in, cXyz* out, f32 scale);
s32 massCrossCapsule(MassCylAttr* cyl, MassCpsAttr* cps, be<f32>* distance);
f32 massSquareDistance(cXyz* a, cXyz* b);
void massDivideDtor(MassDivideInfo* p, s32 flags);
void massAssert(const char* file, u32 line, const char* text);

void massClearEntry(dCcMassS_MngNative* p);
void massCtEntry(dCcMassS_MngNative* p);

struct MassObjectVtable {
    be<u32> mPrefix[7], GetGObjInf, mGap[3], GetShapeAttr;
};
struct MassShapeVtable {
    be<u32> mPrefix[37], CalcAabBox;
};
WWHD_OFFSET(MassObjectVtable, GetGObjInf, 0x1C);
WWHD_OFFSET(MassObjectVtable, GetShapeAttr, 0x2C);
WWHD_SIZE(MassObjectVtable, 0x30);
WWHD_OFFSET(MassShapeVtable, CalcAabBox, 0x94);
WWHD_SIZE(MassShapeVtable, 0x98);

namespace massFlag {
enum : u32 {
    CAMERA_ENABLED = 1,
    ATTACK = 1,
    COLLISION = 2,
    AREA = 8,
    MOVE = 16,
    RESULT_ATTACK = 1,
    RESULT_COLLISION = 2,
    RESULT_AREA = 4,
    COLLIDER_SET = 1,
    AT_NO_MASS = 8
};
}
