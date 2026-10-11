// HD: entry-point return widths retain the verified signatures where tww only has stubs.
// Gale Isle palm. GameCube fields after the expanded HD actor base move by +0x118.
// HD: collision arrays and wind fields below complete the opaque tww layout without changing offsets.
#pragma once
#include "bindings.h"
#include "d/d_cc_d.h"

struct J3DAnmTransformKey;
void vyasiAddAngles(csXyz*, const csXyz*);
void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
void cXyz_outprod(const cXyz*, cXyz*, const cXyz*);
void PSQUATMultiply(const Quaternion*, const Quaternion*, Quaternion*);
namespace daObjVyasi {
struct CapsuleShapeAttr {
    u8 bounds[0x18];
    be<u32> boundsVtable, __vtbl;
};
struct CapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct Capsule : dCcD_GObjInf {
    CapsuleShapeAttr mShape;
    CapsuleGeometry mCps;
};
WWHD_SIZE(Capsule, 0x138);
WWHD_OFFSET(Capsule, mCps, 0x118);
struct CapsuleEnds {
    cXyz mStart, mEnd;
    be<f32> mRadius;
};
WWHD_SIZE(CapsuleEnds, 0x1C);
struct Act_c : fopAc_ac_c {
    u8 create_heap();
    BOOL process_init(s32 state);
    void set_first_process();
    void set_mtx();
    s32 _create();
    u8 _delete();
    BOOL PlayStopJointAnimation();
    void process_main();
    void set_collision();
    void quaternion_main();
    void calc_dif_angle();
    void leaf_scale_main();
    BOOL _execute();
    BOOL _draw();
    BOOL SetStopJointAnimation(void* animation, f32 speed, f32 morph);
    BOOL process_sag_init();
    void process_sag_main();
    BOOL process_sagWind_init();
    void process_sagWind_main();
    BOOL process_toNormal_init();
    void process_toNormal_main();
    BOOL process_normal_init();
    f32 process_normal_main();
    BOOL process_none_init();
    void process_none_main();

    /* 0x3AC */ be<s16> m0294[14];
    /* 0x3C8 */ be<s16> m02B0[14];
    /* 0x3E4 */ Quaternion mJointQuat[14];
    /* 0x4C4 */ csXyz m03AC[14];
    /* 0x518 */ cXyz m0400[14];
    /* 0x5C0 */ be<f32> m04A8, m04AC, m04B0;
    /* 0x5CC */ request_of_phase_process_class mPhs;
    /* 0x5D4 */ Mtx34 m04BC;
    /* 0x604 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x608 */ gptr<J3DAnmTransformKey> mpBckData;
    /* 0x60C */ cXyz mEkszsPos;
    /* 0x618 */ be<s16> mEkszsRotY;
    u8 padding61A[2];
    /* 0x61C */ be<f32> m0504;
    /* 0x620 */ be<s16> mAnglePhase[14];
    /* 0x63C */ be<s16> m0524[15];
    u8 padding65A[2];
    /* 0x65C */ be<s16> mNormalCounter;
    u8 padding65E[2];
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Cyl mCyl;
    /* 0x7CC */ dCcD_Stts mCapsuleStts[5];
    /* 0x8F8 */ Capsule mCapsules[5];
    /* 0xF10 */ CapsuleEnds mCapsuleEnds[5];
    /* 0xF9C */ dCcD_Stts mSphereStts[8];
    /* 0x117C */ dCcD_Sph mSpheres[8];
    /* 0x1ADC */ be<s32> m19C4;
    /* 0x1AE0 */ be<s32> mState;
    /* 0x1AE4 */ be<f32> m19CC;
    /* 0x1AE8 */ be<s16> m19D0;
    u8 padding1AEA[2];
    /* 0x1AEC */ be<f32> m19D4;
};
WWHD_OFFSET(Act_c, mJointQuat, 0x3E4);
WWHD_OFFSET(Act_c, m03AC, 0x4C4);
WWHD_OFFSET(Act_c, m0400, 0x518);
WWHD_OFFSET(Act_c, mPhs, 0x5CC);
WWHD_OFFSET(Act_c, mpMorf, 0x604);
WWHD_OFFSET(Act_c, mNormalCounter, 0x65C);
WWHD_OFFSET(Act_c, mStts, 0x660);
WWHD_OFFSET(Act_c, mCyl, 0x69C);
WWHD_OFFSET(Act_c, mCapsuleStts, 0x7CC);
WWHD_OFFSET(Act_c, mCapsules, 0x8F8);
WWHD_OFFSET(Act_c, mCapsuleEnds, 0xF10);
WWHD_OFFSET(Act_c, mSphereStts, 0xF9C);
WWHD_OFFSET(Act_c, mSpheres, 0x117C);
WWHD_OFFSET(Act_c, m19C4, 0x1ADC);
WWHD_SIZE(Act_c, 0x1AF0);

// HD: these GHS PTMF descriptors are eight bytes; virtual displacement overlaps the function word.
struct ProcessAction {
    be<s16> mThisDelta, mSelector;
    be<u32> mFunction;
    template <class R> R invoke(Act_c* actor) const {
        u32 adjusted = gabi::ea(actor) + s32(mThisDelta);
        s32 selector = mSelector;
        u32 fn;
        if (selector < 0)
            fn = mFunction;
        else {
            u32 vtable = gabi::load<u32>(adjusted + s32(s16(u32(mFunction))));
            fn = gabi::load<u32>(vtable + selector * 8 + 4);
        }
        return gabi::call<R>(fn, gabi::at<Act_c>(adjusted));
    }
};
WWHD_SIZE(ProcessAction, 8);
struct ProcessTable {
    ProcessAction entries[5];
};
enum ProcessState {
    STATE_NONE,
    STATE_SAG,
    STATE_SAG_WIND,
    STATE_TO_NORMAL,
    STATE_NORMAL,
    STATE_COUNT
};
enum ResourceIndex { RES_BCK = 4, RES_MODEL = 7 };
// HD: the source actor's profile is 0xA1 here (tww Obj_Ekskz is 0xA3).
enum { PROFILE_EKSZS = 0xA1 };
struct JointKinds {
    be<u8> entries[14];
};
struct WindMatrixPointer {
    gptr<Mtx34> mMatrix;
};
struct BaseAngles {
    csXyz entries[14];
};
// The source records use the GameCube collision-field order, unchanged in HD.
struct SourceAt {
    be<u32> mType;
    be<u8> mAtp;
    u8 padding[3];
    be<u32> mSPrm;
};
struct SourceTg {
    be<u32> mType, mSPrm;
};
struct SourceHitInfo {
    SourceAt mObjAt;
    SourceTg mObjTg;
    be<u32> mCoSPrm;
};
struct SourceObject {
    be<u32> mFlags;
    SourceHitInfo mSrcObjHitInf;
};
struct SourceGAtTg {
    be<u8> mSe, mHitMark, mSpl, mMtrl;
    be<u32> mSPrm;
};
struct SourceGObjInf {
    SourceObject mObj;
    SourceGAtTg mGObjAt, mGObjTg;
    be<u32> mGObjCo;
};
WWHD_SIZE(SourceGObjInf, 0x30);
struct CylSource {
    SourceGObjInf mObjInf;
    cXyz mCenter;
    be<f32> mRadius, mHeight;
};
struct CpsSource {
    SourceGObjInf mObjInf;
    cXyz mStart, mEnd;
    be<f32> mRadius;
};
struct SphSource {
    SourceGObjInf mObjInf;
    cXyz mCenter;
    be<f32> mRadius;
};
struct MathStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mSinTable, mAtanTable, padding[2];
    be<u32> mZero[4];
};
struct AngleLimits {
    be<f32> mMin, mMax;
};
struct GlobalDescriptors {
    u8 entries[3][12];
};
WWHD_SIZE(JointKinds, 14);
WWHD_SIZE(BaseAngles, 84);
WWHD_SIZE(CylSource, 0x44);
WWHD_SIZE(CpsSource, 0x4C);
WWHD_SIZE(SphSource, 0x40);
WWHD_SIZE(MathStatics, 0x1C);
WWHD_SIZE(GlobalDescriptors, 36);
enum Vtable : u32 {
    VT_RESOURCE = 0x10033350,
    VT_ACTOR = 0x10033378,
    VT_BOUNDS = 0x10033368,
    VT_SHAPE = 0x100015A8,
    VT_CYL_SHAPE = 0x1004B160,
    VT_CYL_HIT = 0x1004B108,
    VT_CYL = 0x1004B150,
    VT_CAPSULE_HIT = 0x1004AF18,
    VT_CAPSULE = 0x1004AF60,
    VT_CAPSULE_SHAPE = 0x1004AF70,
};
} // namespace daObjVyasi

void daObj_HitSeStart(cXyz*, s8, dCcD_GObjInf*, s32);

void daObj_HitEff_kikuzu(fopAc_ac_c*, dCcD_Cyl*);

void dKy_Sound_set(cXyz*, s32, u32, s32);

void vyasiSetCapsuleGeometry(daObjVyasi::CapsuleGeometry*, const daObjVyasi::CapsuleEnds*);

void vyasiActorConstruct(fopAc_ac_c*);

void vyasiHitInfoConstruct(void*);

void vyasiCylinderGeometryConstruct(cM3dGCyl*);

void vyasiConstructArray(void*, s32, s32, u32);

void vyasiSetCapsule(daObjVyasi::Capsule*, const void*);

// HD: an external GHS array-construction callback remains a guest token.
constexpr u32 VYASI_SPHERE_CONSTRUCTOR = 0x025166F0;

s32 vyasiPrmAbstract(const fopAc_ac_c*, u32, u32);

void fopAcM_rollPlayerCrash(fopAc_ac_c*, s32, f32);

f32 PSVECSquareDistance(const cXyz*, const cXyz*);

f32 vyasiAddCalc(be<f32>*, f32, f32, f32, f32);

// HD: external actor-search callback remains a guest token.
constexpr u32 VYASI_PROFILE_JUDGE = 0x025E121C;

void vyasiConstructSinTable(u8*);

void vyasiConstructAtanTable(u8*);

void vyasiCapsuleGeometryConstruct(daObjVyasi::CapsuleGeometry*);

void vyasiDestructArray(void*, s32, s32, u32, s32, s32);

void vyasiActorDestruct(fopAc_ac_c*, s32);

// HD: deleting-array callbacks are external guest addresses, not host function pointers.
constexpr u32 VYASI_SPHERE_DESTRUCTOR = 0x02515AE8;
constexpr u32 VYASI_STATUS_DESTRUCTOR = 0x02515860;
constexpr u32 VYASI_CAPSULE_DESTRUCTOR = 0x02515980;
