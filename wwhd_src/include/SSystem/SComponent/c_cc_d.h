// Collision component layouts used by this unit.
#pragma once
#include "wwhd.h"

namespace CcDataHD {
struct Vector {
    be<f32> mCoord[3];
};
struct WordVector {
    be<u32> mCoord[3];
};
WWHD_SIZE(Vector, 12);
WWHD_SIZE(WordVector, 12);
struct Aab {
    Vector mMin, mMax;
    be<u32> mVtable;
};
WWHD_SIZE(Aab, 0x1C);
struct Plane {
    be<f32> mCoefficients[4];
    be<u32> mVtable;
};
WWHD_SIZE(Plane, 0x14);
struct Triangle {
    Plane mPlane;
    WordVector mVertex[3];
};
WWHD_SIZE(Triangle, 0x38);
struct Capsule {
    Vector mStart, mEnd;
    be<u32> mVtable;
    be<f32> mRadius;
};
WWHD_SIZE(Capsule, 0x20);
WWHD_OFFSET(Capsule, mVtable, 0x18);
struct Cylinder {
    Vector mCenter;
    be<f32> mRadius, mHeight;
    be<u32> mVtable;
};
WWHD_SIZE(Cylinder, 0x18);
struct Sphere {
    Vector mCenter;
    be<f32> mRadius;
    be<u32> mVtable;
};
WWHD_SIZE(Sphere, 0x14);
struct cBgW_TriElm {
    Plane mPlane;
    be<u32> mVtable;
};
WWHD_SIZE(cBgW_TriElm, 0x18);
struct cBgW_NodeTree {
    Aab mAab;
};
WWHD_SIZE(cBgW_NodeTree, 0x1C);
struct cCcD_DivideInfo {
    u32 Chk(u32 q);
    be<u32> mRangeBits, mVtable;
};
WWHD_SIZE(cCcD_DivideInfo, 8);
struct DivideAxis {
    be<u8> mDiffIsZero;
    u8 mPadding[3];
    be<f32> mScaledDiff, mInvScaledDiff;
};
WWHD_SIZE(DivideAxis, 0xC);
struct cCcD_DivideArea {
    void SetArea(u32 box);
    void CalcDivideInfo(u32 out, u32 box, u32 all);
    void CalcDivideInfoOverArea(u32 out, u32 box);
    Aab mAab;
    DivideAxis mAxis[3];
};
WWHD_SIZE(cCcD_DivideArea, 0x40);
WWHD_OFFSET(cCcD_DivideArea, mAxis, 0x1C);
struct cCcD_ShapeAttr {
    Aab mAab;
    be<u32> mVtable;
};
WWHD_SIZE(cCcD_ShapeAttr, 0x20);
struct cCcD_CpsAttr {
    void CalcAabBox();
    u32 GetNVec(u32 point, u32 out);
    cCcD_ShapeAttr mShape;
    Capsule mCps;
};
WWHD_SIZE(cCcD_CpsAttr, 0x40);
struct cCcD_TriAttr {
    void CalcAabBox();
    u32 GetNVec(u32 point, u32 out);
    cCcD_ShapeAttr mShape;
    Triangle mTri;
};
WWHD_SIZE(cCcD_TriAttr, 0x58);
struct cCcD_CylAttr {
    u32 GetCoCP();
    void CalcAabBox();
    u32 GetNVec(u32 point, u32 out);
    cCcD_ShapeAttr mShape;
    Cylinder mCyl;
};
WWHD_SIZE(cCcD_CylAttr, 0x38);
struct cCcD_SphAttr {
    u32 GetCoCP();
    void CalcAabBox();
    u32 GetNVec(u32 point, u32 out);
    cCcD_ShapeAttr mShape;
    Sphere mSph;
};
WWHD_SIZE(cCcD_SphAttr, 0x34);
struct cCcD_Stts {
    void Ct();
    void Init(s32 weight, s32 flags, u32 actor, u32 id);
    void PlusCcMove(f32 x, f32 y, f32 z);
    void ClrTg();
    Vector m_cc_move;
    be<u32> mp_actor, m_apid;
    be<u8> m_weight, mInitFlags, m_dmg, mReserved;
    be<u32> mVtable;
};
WWHD_SIZE(cCcD_Stts, 0x1C);
WWHD_OFFSET(cCcD_Stts, mVtable, 0x18);
struct cCcD_ObjCommonBase {
    be<u32> mSPrm, mRPrm, mHitObj, mVtable;
};
WWHD_SIZE(cCcD_ObjCommonBase, 0x10);
struct cCcD_ObjAt {
    cCcD_ObjCommonBase mBase;
    be<u32> mType;
    be<u8> mAtp;
    u8 mPadding[3];
};
WWHD_SIZE(cCcD_ObjAt, 0x18);
struct cCcD_ObjTg {
    cCcD_ObjCommonBase mBase;
    be<u32> mType;
};
WWHD_SIZE(cCcD_ObjTg, 0x14);
struct cCcD_Obj {
    void Set(u32 q);
    cCcD_ObjAt mObjAt;
    cCcD_ObjTg mObjTg;
    cCcD_ObjCommonBase mObjCo;
    be<u32> mVtable, mFlags, mStts;
    cCcD_DivideInfo mDivideInfo;
};
WWHD_SIZE(cCcD_Obj, 0x50);
WWHD_OFFSET(cCcD_Obj, mVtable, 0x3C);
WWHD_OFFSET(cCcD_Obj, mDivideInfo, 0x48);
struct cCcD_SrcObj {
    be<u32> mFlags, mAtType;
    be<u8> mAtp;
    u8 mPadding[3];
    be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
};
WWHD_SIZE(cCcD_SrcObj, 0x1C);
// HD: explicit GHS vtable slots retain double dispatch through the original other shape.
struct ShapeVtable {
    u8 mOtherSlots[0x24];
    be<u32> mAtCps;
    u8 mPad28[4];
    be<u32> mAtTri;
    u8 mPad30[0xC];
    be<u32> mAtCyl;
    u8 mPad40[4];
    be<u32> mAtSph;
    u8 mPad48[0x14];
    be<u32> mCoCps;
    u8 mPad60[4];
    be<u32> mCoTri;
    u8 mPad68[0xC];
    be<u32> mCoCyl;
    u8 mPad78[4];
    be<u32> mCoSph;
};
WWHD_OFFSET(ShapeVtable, mAtCps, 0x24);
WWHD_OFFSET(ShapeVtable, mAtTri, 0x2C);
WWHD_OFFSET(ShapeVtable, mAtCyl, 0x3C);
WWHD_OFFSET(ShapeVtable, mAtSph, 0x44);
WWHD_OFFSET(ShapeVtable, mCoCps, 0x5C);
WWHD_OFFSET(ShapeVtable, mCoTri, 0x64);
WWHD_OFFSET(ShapeVtable, mCoCyl, 0x74);
WWHD_OFFSET(ShapeVtable, mCoSph, 0x7C);
struct StatusVtable {
    u8 mOtherSlots[0x24];
    be<u32> Ct;
};
WWHD_OFFSET(StatusVtable, Ct, 0x24);
struct HeaderStatics {
    be<f32> mPiMin, mPiMax;
    be<u8> mByte, mFlag;
    u8 mPadding[2];
};
struct HeaderZero {
    be<u32> mWords[4];
};
struct DestructorRecord {
    be<u32> mWords[4];
};
} // namespace CcDataHD
