/* Arrow actor (Link and Zelda), verified WWHD layout.
 * HD: actor base grows to0x3AC, collision/layout fields follow the HD engine,
 * and the GHS member-function pointer occupies8 bytes rather than the GC12. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"

enum {
    dRes_INDEX_LINK_BDL_ARROW_e = 0x37,
    dRes_INDEX_LINK_BDL_ARROWGLITTER_e = 0x38,
    fpcNm_ARROW_e = 0x1D8,
    fpcNm_ARROW_LIGHTEFF_e = 0x1DA,
    fpcNm_PZ_e = 0xD2,
    AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_LIGHT_ARROW_e = 0x36,
};

struct daArrow_c : fopAc_ac_c {
    enum ArrowType { TYPE_NORMAL = 0, TYPE_FIRE = 1, TYPE_ICE = 2, TYPE_LIGHT = 3 };
    static const char m_arc_name[];
    BOOL _createHeap();
    void _atHit(dCcD_GObjInf *thisObjInf, fopAc_ac_c *hitActor, dCcD_GObjInf *hitObjInf);
    void checkCreater();
    void setLightEffect();
    void setBlur();
    void createBlur();
    void setArrowShootSe();
    void setDrawShapeMaterial();
    void arrowShooting();
    void arrowUseMp();
    void ShieldReflect();
    bool check_water_in();
    daArrow_c *changeArrowType();
    void setRoomInfo();
    void setKeepMatrix();
    void setStopActorMatrix();
    BOOL procWait();
    BOOL procMove();
    BOOL procReturn();
    BOOL procStop_BG();
    BOOL procStop_Actor();
    BOOL procWater();
    void setTypeByPlayer();
    BOOL createInit();
    BOOL _execute();
    BOOL _draw();
    cPhs_State _create();
    BOOL _delete();

    void setProc(u32 fn) { /* GHS pointer to member {s16 delta 0, s16 -1 (not virtual), fn} */
        mCurrProcFunc.d = 0;
        mCurrProcFunc.i = -1;
        mCurrProcFunc.f = fn;
    }

    /* 0x3AC */ be<u8> mbSetByZelda;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ gptr<J3DModel> mpModel;
    /* 0x3B4 */ be<u32> mpTipMat;       /* J3DMaterial* */
    /* 0x3B8 */ be<u32> mpBtk;          /* J3DAnmTextureSRTKey* */
    /* 0x3BC */ dBgS_LinChk mLinChk;    /* dBgS_ArrowLinChk */
    /* 0x428 */ dBgS_ObjGndChk mGndChk; /* dBgS_ObjGndChk */
    /* 0x47C */ dCcD_Stts mStts;
    /* 0x4B8 */ dCcD_Cps mAtCps;
    /* 0x5F0 */ dCcD_Sph mCoSph;
    /* 0x71C */ be<u8> mDeleteRequest;  /* was field_0x600: procStop_BG 02054FE0 / procStop_Actor 020553C0 delete the stuck arrow once it is set; set when the stuck time (mStopTimer) ends, when the hit actor is gone, by mbSetByZelda, and by _execute 02052DB8 when l_count comes round to mShotNo again (tww the same) */
    /* 0x71D */ be<u8> mArrowType;
    /* 0x71E */ be<s16> mShotNo;  /* was field_0x602: arrowShooting 02053814 stores l_count (0-4) for the arrow; _execute 02052DB8 sets mDeleteRequest when l_count is back at it; createInit 02052738 sets -1 (tww the same) */
    /* 0x720 */ be<s16> mHitShakeTimer;  /* was field_0x604: procMove 02054040 sets 0x28 when the arrow sticks; setStopActorMatrix 02053DDC / procStop_BG 02054FE0 count it down and shake the arrow's x angle by (1024 * (t / 40)^2) * sin(t * 0x52FB) (tww the same) */
    /* 0x722 */ u8 _722[2];
    /* 0x724 */ be<s16> mSparkleTimer;
    /* 0x726 */ u8 _726[2];
    /* 0x728 */ be<u32> mpSparkleEmitter;
    /* 0x72C */ be<u32> mHitActorProcID;
    /* 0x730 */ be<s32> mHitJointIndex;
    /* 0x734 */ cXyz mHitJointOffset;  /* was field_0x618: procMove 02054040 stores the hit position local to the hit joint (ArrowSearchJointHit); setStopActorMatrix 02053DDC translates the joint matrix by it (tww the same) */
    /* 0x740 */ u8 field_0x624[0x77C - 0x740];
    /* 0x77C */ be<f32> mBtkFrame;
    /* 0x780 */ be<u8> mbLinkReflect;
    /* 0x781 */ u8 _781[3];
    /* 0x784 */ dPa_followEcallBack mBlurFollowCb; /* dPa_followEcallBack: vtable +0, emitter +4 */
    /* 0x798 */ csXyz mBlurAngle;
    /* 0x79E */ be<u8> mLightEffType;  /* was field_0x682: setLightEffect 02052C40: the arrow type the light effect child was made for; a different mArrowType deletes the child (tww the same) */
    /* 0x79F */ u8 _79F;
    /* 0x7A0 */ be<u32> mLightEffPID;
    /* 0x7A4 */ be<u8> mbHasLightEff;
    /* 0x7A5 */ u8 _7A5[3];
    /* 0x7A8 */ gabi::ptmf<BOOL (daArrow_c::*)()> mCurrProcFunc; // HD: GHS8-byte member pointer
    /* 0x7B0 */ be<u8> mDraw;  /* was field_0x698: _draw 02053188 draws only while set; createInit 02052738 sets it; procStop_BG 02054FE0 blinks it during the last 60 frames of mStopTimer; cleared when the arrow ends in water or hits (tww the same) */
    /* 0x7B1 */ be<u8> mWaterEffect;  /* was field_0x699: check_water_in 020532B8 / procReturn 02055450: the state byte passed to and returned by Link's setItemWaterEffect; createInit 02052738 clears it (tww the same) */
    /* 0x7B2 */ be<u8> mBounceGround;  /* was field_0x69a: procReturn 02055450 sets it when the falling arrow bounces off a polygon with normal.y >= 0.5 and deletes the arrow once it falls again; createInit 02052738 clears it (tww the same) */
    /* 0x7B3 */ u8 _7B3;
    /* 0x7B4 */ be<s16> mSpinSpeedX;  /* was field_0x69c: rebound (in procMove 02054040) sets 0x2C00; procReturn 02055450 adds it to shape_angle.x each frame and halves and reverses it on a bounce; createInit 02052738 clears it (tww the same) */
    /* 0x7B6 */ u8 _7B6[2];
    /* 0x7B8 */ be<s32> mStopTimer;  /* was field_0x6a0: createInit 02052738 sets 300; procStop_BG 02054FE0 counts it down for normal arrows (blink below 60, mDeleteRequest at 0) (tww the same) */
    /* 0x7BC */ be<s32> mInWaterTimer;
    /* 0x7C0 */ cXyz mImpactPos;  /* was field_0x6a8: arrowShooting 02053814 sets the shot position; procMove 02054040 stores the hit position (BG cross or AT hit) and plays the hit particles there; setLightEffect 02052C40 creates the light effect at it; procStop_BG 02054FE0 keeps the arrow tip there (moving BG); procMove sets mFlewFar beyond 20000 of it (tww the same) */
    /* 0x7CC */ Mtx34 mModelMtx;  /* was field_0x6b4: procWait 02053D24 / procMove 02054040 copy the model (or hit stack) matrix into it each frame; the light effect child daArrow_Lighteff_c reads it (setTopPos 020570BC / set_mtx 02056EB8) (tww the same) */
    /* 0x7FC */ be<u8> mFlewFar;  /* was field_0x6e4: procMove 02054040 sets it once the arrow is farther than 20000 from mImpactPos (the shot position); check_water_in 020532B8 / procMove then skip dKy_arrowcol_chg_on; createInit 02052738 clears it (tww the same) */
    /* 0x7FD */ u8 _7FD;
    /* 0x7FE */ csXyz mHitAngle;  /* was field_0x6e6: procWait 02053D24 / procMove 02054040 store shape_angle each frame, and the angle local to the hit joint on an actor hit; setStopActorMatrix 02053DDC rotates the stuck arrow by it, procStop_BG 02054FE0 uses it as the base of the shake (tww the same) */
    /* 0x804 */ be<u32> mpAtHitActor;
    /* 0x808 */ cXyz mAtHitPos;
    /* 0x814 */ be<f32> mNearestHitDist;
    /* 0x818 */ be<u8> mbHitActor;
    /* 0x819 */ u8 _819[3];
};
WWHD_OFFSET(daArrow_c, mLinChk, 0x3BC);
WWHD_OFFSET(daArrow_c, mAtCps, 0x4B8);
WWHD_OFFSET(daArrow_c, mCoSph, 0x5F0);
WWHD_OFFSET(daArrow_c, mArrowType, 0x71D);
WWHD_OFFSET(daArrow_c, mBtkFrame, 0x77C);
WWHD_OFFSET(daArrow_c, mCurrProcFunc, 0x7A8);
WWHD_OFFSET(daArrow_c, mModelMtx, 0x7CC);
WWHD_OFFSET(daArrow_c, mbHitActor, 0x818);
WWHD_SIZE(daArrow_c, 0x81C);

// HD: unit-local bindings retain the verified argument widths.
void PSMTXConcat(const Mtx34 *a, const Mtx34 *b, Mtx34 *ab);
void mDoMtx_XYZrotM(Mtx34 *m, s16 x, s16 y, s16 z);
void mDoMtx_MtxToRot(Mtx34 *m, csXyz *rot);
void fopAcM_SetMin(fopAc_ac_c *a, f32 x, f32 y, f32 z);
void fopAcM_SetMax(fopAc_ac_c *a, f32 x, f32 y, f32 z);
BOOL fopAcM_SearchByID_out(u32 id, be<u32> *out);
u32 fopAcM_createChild(s16 name, u32 parent, u32 prm, cXyz *pos, s32 room, csXyz *angle, cXyz *scale,
                       s8 subtype, u32 createFunc);
fopAc_ac_c *fopAcM_fastCreate(s16 name, u32 prm, cXyz *pos, s32 room, csXyz *angle, cXyz *scale, s8 subtype,
                              u32 createFunc, u32 data);
f32 cBgS_GroundCross(dBgS *bgs, void *chk);
s32 dBgS_GetRoomId(dBgS *bgs, void *poly);
s32 dBgS_GetPolyColor(dBgS *bgs, void *poly);
void dCcD_Cps_Set(void *cps, u32 src);
void dCcD_Sph_Set(void *sph, u32 src);
void cM3dGCps_Set(void *cps, cXyz *start, cXyz *end, f32 r);
void PSVECSubtract(const cXyz *a, const cXyz *b, cXyz *ab);
void JPASetRMtxTVecfromMtx(const Mtx34 *m, void *r, void *t);
f32 cM_rnd();
u8 daPy_lk_c_setItemWaterEffect(fopAc_ac_c *a, s32 prev, s32 p);
void dKy_arrowcol_chg_on(cXyz *pos, s32 type);
BOOL fopAcM_getWaterY(const cXyz *pos, be<f32> *y);
BOOL fopAcM_SearchByName(s16 name, be<u32> *out);
BOOL dAttention_LockonTruth(void *att);
u32 dAttention_LockonTarget(void *att, s32 i);
s16 cLib_targetAngleX(cXyz *a, cXyz *b);
void mDoMtx_XrotM(Mtx34 *m, s16 x);
BOOL cBgS_ChkPolySafe(dBgS *bgs, void *poly);
BOOL dBgS_ChkMoveBG(dBgS *bgs, void *poly);
void dBgS_MoveBgTransPos(dBgS *bgs, void *poly, bool b, cXyz *pos, csXyz *angle, csXyz *shape);
BOOL dBgS_ChkGrpInf(dBgS *bgs, void *poly, u32 grp);
s32 dBgS_GetAttributeCode(dBgS *bgs, void *poly);
void C_VECReflect(const cXyz *in, const void *normal, cXyz *out);
void cM3d_CalcVecZAngle(const void *v, csXyz *out);
s32 fopAcM_createItemForSimpleDemo(cXyz *pos, s32 item, s32 room, csXyz *angle, cXyz *scale, f32 sp, f32 spy);
void mDoAud_seStart_noPos(u32 id);
void cM3dGSph_SetC(void *sph, cXyz *c);
void ArrowMassSet(void *, void *, u8);
void ArrowLineConstruct(void *);
void ArrowCollisionConstruct(void *);
void ArrowCapsuleConstruct(void *);
void ArrowSphereConstruct(void *);
void ArrowFollowConstruct(void *, s32, s32);
fopAc_ac_c *ArrowAtHitActor(dCcD_GObjAt *);
s32 ArrowSearchJointHit(u32, cXyz *, csXyz *, cXyz *, csXyz *);
void ArrowSphereDestroy(void *, s32);
void ArrowCollisionDestroy(void *, s32);
void ArrowStatusDestroy(void *, s32);
void ArrowGroundDestroy(void *, s32);
void ArrowLineDestroy(void *, s32);
void ArrowActorDestroy(daArrow_c *, s32);

// HD: private views fill fields not exposed by the shared partial engine layouts.
struct ArrowAttackInfo {
    be<u32> mAtType;
    be<u8> mAtp;
    u8 pad5;
    be<u16> mTipJointIdx;
};
struct ArrowAttackTable {
    ArrowAttackInfo mEntries[4];
};
struct ArrowJointBuffer {
    u8 pad0[4];
    be<u32> mCount, mpEntries;
};
struct ArrowJoint {
    u8 pad0[0x10];
    be<u32> mpMesh;
    u8 pad14[8];
};
struct ArrowMaterial {
    u8 pad0[4];
    be<u32> mpNext, mpShape;
};
struct ArrowAnimation {
    u8 pad0[4];
    be<f32> mFrame;
    u8 pad8[2];
    be<s16> mFrameMax;
};
struct ArrowZelda {
    u8 pad0[0x44C];
    be<u32> mpMorf;
};
struct ArrowMorf {
    u8 pad0[0x90];
    be<u32> mpModel;
};
struct ArrowJointHit {
    u8 pad0[4];
    be<u32> mpModel;
};
struct ArrowPlayerVtable {
    be<u32> mFunctions[0x50];
};
struct ArrowSafeStringVtable {
    u8 pad0[0x14];
    be<u32> mpAssureTermination;
};
struct ArrowCapsuleSource {
    u8 pad0[0x48];
    be<f32> mRadius;
};
struct ArrowSphereSource {
    u8 pad0[0x3C];
    be<f32> mRadius;
};
struct ArrowWords {
    be<u32> x, y, z;
};
struct ArrowEmitter {
    u8 pad0[0x5C];
    be<s32> mMaxFrame;
    u8 pad60[0x190];
    be<f32> mGlobalRotation[9];
    u8 pad214[0xC];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
    u8 pad244[3];
    be<u8> mGlobalAlpha;
    u8 pad248[0xC];
    be<u32> mFlags;
    u8 pad258[0xA];
    be<u8> mVersion;
};
WWHD_SIZE(ArrowAttackInfo, 8);
WWHD_OFFSET(ArrowAttackInfo, mAtp, 4);
WWHD_OFFSET(ArrowAttackInfo, mTipJointIdx, 6);
WWHD_SIZE(ArrowJoint, 0x1C);
WWHD_OFFSET(ArrowEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(ArrowEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(ArrowEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(ArrowEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(ArrowEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(ArrowEmitter, mFlags, 0x254);
WWHD_OFFSET(ArrowEmitter, mVersion, 0x262);
WWHD_OFFSET(ArrowAnimation, mFrameMax, 0xA);

struct ArrowShapeAttr {
    u8 pad0[0x18];
    be<u32> mAabVtable, mShapeVtable;
};
struct ArrowDispatchEntry {
    be<u32> mAdjustment, mpFunction;
};
struct ArrowDispatchVtable {
    ArrowDispatchEntry mEntries[0x8000];
};
struct ArrowPtmfView {
    be<s16> mDelta, mIndex;
    be<u16> mFunctionHigh;
    be<s16> mVtableOffset;
};
WWHD_SIZE(ArrowPtmfView, 8);
WWHD_OFFSET(ArrowPtmfView, mVtableOffset, 6);
WWHD_OFFSET(ArrowShapeAttr, mAabVtable, 0x18);
WWHD_OFFSET(ArrowShapeAttr, mShapeVtable, 0x1C);

struct ArrowCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(ArrowCapsuleGeometry, __vtbl, 0x18);
WWHD_SIZE(ArrowCapsuleGeometry, 0x20);

// HD: this translation unit owns distinct constructor/destructor vtables.
namespace arrow_tu {
constexpr u32 actorVtable = 0x10007358;
constexpr u32 linePolyInfoVtable = 0x10007328;
constexpr u32 linePolyPassVtable = 0x10007348;
constexpr u32 lineGroupVtable = 0x10007338;
constexpr u32 lineCheckVtable = 0x10007318;
constexpr u32 groundCheckVtable = 0x10007298;
constexpr u32 groundPolyInfoVtable = 0x100072A8;
constexpr u32 groundPolyPassVtable = 0x100072C8;
constexpr u32 groundGroupVtable = 0x100072B8;
constexpr u32 shapeVtable = 0x100015A8;
constexpr u32 aabVtable = 0x10007228;
constexpr u32 capsuleHitInfoVtable = 0x1004AF18;
constexpr u32 capsuleAttrVtable = 0x1004AF70;
constexpr u32 capsuleVtable = 0x1004AF60;
constexpr u32 groundPolyInfoDestroyVtable = 0x10007268;
constexpr u32 groupDestroyVtable = 0x10007248;
constexpr u32 groundPolyPassDestroyVtable = 0x10007288;
constexpr u32 linePolyPassDestroyVtable = 0x10007308;
constexpr u32 linePolyInfoDestroyVtable = 0x10007238;
constexpr u32 headerVectors = 0x1046146C;
constexpr u32 headerVectorGuard = 0x1018FDC4;
} // namespace arrow_tu

// HD: immediate GHS procedure targets used by the eight-byte member pointer.
enum : u32 {
    A_procWait = 0x02053D24,
    A_procMove = 0x02054040,
    A_procStop_BG = 0x02054FE0,
    A_procStop_Actor = 0x020553C0,
    A_procReturn = 0x02055450,
    A_procWater = 0x02055734,
    A_createHeap_CB = 0x02052268,
    A_atHit_CB = 0x02052388,
};

WWHD_OFFSET(ArrowJointBuffer, mCount, 4);
WWHD_OFFSET(ArrowJointBuffer, mpEntries, 8);
WWHD_OFFSET(ArrowJoint, mpMesh, 0x10);
WWHD_OFFSET(ArrowMaterial, mpNext, 4);
WWHD_OFFSET(ArrowMaterial, mpShape, 8);
WWHD_OFFSET(ArrowZelda, mpMorf, 0x44C);
WWHD_OFFSET(ArrowMorf, mpModel, 0x90);
WWHD_OFFSET(ArrowJointHit, mpModel, 4);
WWHD_OFFSET(ArrowSafeStringVtable, mpAssureTermination, 0x14);
WWHD_OFFSET(ArrowCapsuleSource, mRadius, 0x48);
WWHD_OFFSET(ArrowSphereSource, mRadius, 0x3C);
WWHD_OFFSET(ArrowDispatchEntry, mpFunction, 4);
WWHD_SIZE(ArrowDispatchEntry, 8);
WWHD_SIZE(ArrowWords, 12);
