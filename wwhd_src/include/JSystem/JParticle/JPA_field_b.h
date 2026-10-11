/* JPAField.cpp's second WWHD part. GameCube names, HD field-data layout (-4).
 * Unit-local views avoid changing the first part's independent header. */
#pragma once
#include "JSystem/JParticle/JPAParticle.h"
#include "bindings.h"
namespace jpa_field_b {
struct JPABaseField;
struct JPAFieldVtable {
    u8 _00[0x14];
    be<u32> mInit;
    u8 _18[4];
    be<u32> mPreCalc;
    u8 _20[4];
    be<u32> mCalc;
    u8 _28[4];
    be<u32> mIsItinRange;
};
struct JPAFieldData {
    gptr<JPABaseField> mpBaseField;
    u8 mLink[0x10];
    cXyz mVel;
    union {
        struct {
            cXyz mWork0, mWork1, mWork2;
        };
        be<u32> mWorkWords[3][3];
    };
    be<f32> mMaxDistSq, mFadeOutRate, mFadeInRate;
    cXyz mPos, mDir;
    be<f32> mMag, mMagRndm, mMaxDist, mVal1, mVal2, mVal3;
    be<f32> mFadeIn, mFadeOut, mEnTime, mDisTime;
    be<u16> mSttFlag;
    be<u8> mType, mID, mVelType, mCycle;
};
WWHD_OFFSET(JPAFieldData, mVel, 0x14);
WWHD_OFFSET(JPAFieldData, mWork0, 0x20);
WWHD_OFFSET(JPAFieldData, mPos, 0x50);
WWHD_OFFSET(JPAFieldData, mVal1, 0x74);
WWHD_OFFSET(JPAFieldData, mSttFlag, 0x90);
WWHD_OFFSET(JPAFieldData, mCycle, 0x95);
struct JPABaseField {
    gptr<JPAFieldVtable> __vtbl;
};
struct JPAVortexField : JPABaseField {
    void preCalc(JPAFieldData *);
    void calc(JPAFieldData *, JPABaseParticle *);
    u32 isItinRange(JPAFieldData *, f32);
};
struct JPAConvectionField : JPABaseField {
    void preCalc(JPAFieldData *);
    void calc(JPAFieldData *, JPABaseParticle *);
    u32 isItinRange(JPAFieldData *, f32);
};
struct JPARandomField : JPABaseField {
    void calc(JPAFieldData *, JPABaseParticle *);
};
struct JPADragField : JPABaseField {
    void init(JPAFieldData *, JPABaseParticle *);
    void calc(JPAFieldData *, JPABaseParticle *);
};
struct JPASpinField : JPABaseField {
    void preCalc(JPAFieldData *);
    void calc(JPAFieldData *, JPABaseParticle *);
    u32 isItinRange(JPAFieldData *, f32);
};
struct JPAFieldLink {
    gptr<JPAFieldData> mObject;
    gptr<void> mList;
    gptr<JPAFieldLink> mPrev, mNext;
};
struct JPAFieldList {
    gptr<JPAFieldLink> mHead, mTail;
    be<u32> mLength;
};
struct JPAFieldManager {
    JPAFieldList mList;
    gptr<JPAFieldList> mVacList;
    void init(JPABaseParticle *);
    void preCalc();
    void calc(JPABaseParticle *);
};
// HD: only the emitter RNG and the matrix used by this part are needed.
struct JPAFieldEmitter {
    u8 _00[0x1EC];
    be<u32> mRandomSeed;
};
struct JPAFieldEmitterInfo {
    be<u32> mRandom;
    gptr<JPAFieldEmitter> mpCurEmitter;
    u8 _08[0x30];
    Mtx34 mEmitterGlobalRot;
};
struct JPAFieldEmitterSlot {
    gptr<JPAFieldEmitterInfo> mpInfo;
};
struct JPAFieldMatrix {
    be<u32> m[3][4];
};
WWHD_SIZE(JPAFieldMatrix, 0x30);
WWHD_OFFSET(JPAFieldEmitterInfo, mEmitterGlobalRot, 0x38);
} // namespace jpa_field_b
using namespace jpa_field_b;
void JPAField_basePre(JPABaseField *, JPAFieldData *);
void JPAField_baseVel(JPABaseField *, JPAFieldData *, JPABaseParticle *);
f32 JPAField_calcFade(JPABaseField *, JPAFieldData *, f32);
f64 JPAField_sqrt(f32);
void JPAField_RotAxis(Mtx34 *, cXyz *, f32);
