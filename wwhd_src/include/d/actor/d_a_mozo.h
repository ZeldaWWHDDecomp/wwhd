#pragma once
/* d_a_mozo.h (WWHD) - daMozo_c layout. */
#include "f_op/f_op_actor.h"
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"

struct MozoProcedure {
    be<s16> adjustment, virtualIndex;
    be<u32> target;
};
struct MozoColorAnimation {
    J3DFrameCtrl frame;
    u8 storage[0x78 - 0x10];
};

// HD: GHS places HIO vtables after the GameCube payload.
struct daMozo_childHIO_c {
    be<f32> m04, m08;
    be<s16> m0C, m0E;
    be<u8> m10;
    u8 pad11[3];
    be<u32> __vtbl;
};
struct daMozo_HIO_c {
    be<s8> mNo;
    u8 pad1[3];
    be<s32> m08;
    be<u32> mpBeamChild, mpFireChild;
    daMozo_childHIO_c mBeamChild, mFireChild;
    be<f32> m3C, m40, m44;
    be<u32> __vtbl;
};
WWHD_SIZE(daMozo_childHIO_c, 0x14);
WWHD_SIZE(daMozo_HIO_c, 0x48);
struct MozoCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct MozoCapsule : dCcD_GObjInf {
    u8 mBounds[0x18];
    be<u32> mBoundsVtable, mShapeVtable;
    MozoCapsuleGeometry mGeometry;
};
WWHD_SIZE(MozoCapsule, 0x138);
WWHD_OFFSET(MozoCapsule, mGeometry, 0x118);
struct MozoModel {
    u8 pad0[0x2C];
    be<u32> mpMatrices;
    u8 pad30[0x7C];
    be<u32> mpModelData;
    u8 padB0[8];
    be<u32> mUserArea;
    cXyz mBaseScale;
    be<f32> mBaseTRMtx[12];
};
struct MozoMatrices {
    u8 pad0[4];
    be<u16> mFlags;
    u8 pad6[0xA];
    be<u32> mpAnimationMatrices;
};
struct MozoJoint {
    u8 pad0[4];
    be<u16> mJntNo;
    u8 pad6[2];
    be<u32> mpCallback;
};
struct MozoModelData {
    u8 pad0[4];
    be<u32> mJointCapacity, mpJoints;
};
struct MozoJointTree {
    u8 pad0[8];
    be<u16> mJointNum;
};
struct MozoPlay {
    u8 pad0[0x26A4];
    be<u8> mCollision;
    u8 pad26A5[0x2C13];
    be<u16> mEventFlags;
    u8 pad52BA[0xA];
    be<u8> mEventManager;
    u8 pad52C5[0x7EB];
    be<u32> mpParticleControl;
    u8 pad5AB4[0x78];
    be<u32> mpPlayer;
};
struct MozoBeam : fopAc_ac_c {
    u8 pad3AC[0x36C];
    be<f32> mBeamScale;
    u8 pad71C[0x7C];
    be<f32> mBeamTimer;
    u8 pad79C[0xA8];
    be<s16> mBeamState;
    u8 pad846[0x9A];
    be<u32> mpEmitter;
    be<f32> mThickness;
};
struct MozoPlayer : fopAc_ac_c {
    u8 pad3AC[0x20];
    be<f32> mStatueRangeState;
};
struct MozoPlayerVtable {
    u8 pad0[0x4C];
    be<u32> mpCheckStatus;
};
struct MozoEmitter {
    u8 pad0[0x34];
    be<f32> mRate;
    u8 pad38[0x24];
    be<s32> mFrameCount;
    u8 pad60[0x190];
    be<f32> mGlobalRotation[15];
    cXyz mGlobalTranslation;
    u8 pad238[0x1C];
    be<u32> mStatus;
};
struct MozoSincos {
    be<f32> mSin, mCos;
};
struct MozoSincosTable {
    MozoSincos mEntries[8192];
};
struct MozoWords4 {
    be<u32> mWords[4];
};
struct MozoAngles {
    be<u16> mAngles[3];
};
struct MozoMtx {
    be<f32> mElements[12];
};
WWHD_OFFSET(MozoModel, mpModelData, 0xAC);
WWHD_OFFSET(MozoModel, mBaseTRMtx, 0xC8);
WWHD_OFFSET(MozoPlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(MozoBeam, mpEmitter, 0x8E0);
WWHD_OFFSET(MozoEmitter, mGlobalTranslation, 0x22C);

struct daMozo_c : fopAc_ac_c {
    BOOL CreateHeap();
    BOOL _draw();
    void event_move();
    void set_mtx();
    BOOL _execute();
    BOOL _delete();
    void setAnm(u32 animation, f32 blend);
    void wait_proc_init();
    s32 CreateInit();
    s32 _create();
    void anime_proc();
    BOOL checkRange(s32 expanded);
    u32 getBeamActor(s32 actorId);
    void search_beam_proc_init();
    void search_beam_proc();
    void search_fire_proc_init();
    void search_fire_proc();
    void towait_proc_init();
    void towait_proc();
    void wait_proc();

    gabi::ptmf<void (daMozo_c::*)()> mCurrentProc; // 0x3AC
    u8 mPhs[8];                                    // 0x3B4
    gptr<mDoExt_McaMorf> mAnimMorf;                // 0x3BC
    MozoColorAnimation mBrkAnm;                    // 0x3C0
    be<u32> m_brk;                                 // 0x438
    mDoExt_btkAnm mBtkAnm;                         // 0x43C
    be<u32> m_btk;                                 // 0x4B0
    cXyz beamOrigin, beamTarget;                   // 0x4B4, 0x4C0
    cXyz leftOrigin, rightOrigin;                  // 0x4CC, 0x4D8
    cXyz unrotatedOrigin, unrotatedTarget;         // 0x4E4, 0x4F0
    u8 unused4FC[12];
    cXyz fireOrigin, fireTarget;   // 0x508, 0x514
    cXyz soundPosition;            // 0x520
    be<u32> leftChild, rightChild; // 0x52C, 0x530
    be<f32> mQuatRotation[4];      // 0x534
    be<s32> animation;             // 0x544
    be<s16> fireTimer;             // 0x548
    u8 unused54A[4];
    be<u8> field_0x376, requestEvent;  // 0x54E
    be<u32> fireEmitter, smokeEmitter; // 0x550
    dCcD_Stts mStts;                   // 0x558
    MozoCapsule mCps;                  // 0x594
};
WWHD_SIZE(MozoProcedure, 8);
WWHD_SIZE(MozoColorAnimation, 0x78);
WWHD_OFFSET(daMozo_c, mCurrentProc, 0x3AC);
WWHD_OFFSET(daMozo_c, mPhs, 0x3B4);
WWHD_OFFSET(daMozo_c, mAnimMorf, 0x3BC);
WWHD_OFFSET(daMozo_c, mBrkAnm, 0x3C0);
WWHD_OFFSET(daMozo_c, mBtkAnm, 0x43C);
WWHD_OFFSET(daMozo_c, beamOrigin, 0x4B4);
WWHD_OFFSET(daMozo_c, fireOrigin, 0x508);
WWHD_OFFSET(daMozo_c, mQuatRotation, 0x534);
WWHD_OFFSET(daMozo_c, field_0x376, 0x54E);
WWHD_OFFSET(daMozo_c, mStts, 0x558);
WWHD_OFFSET(daMozo_c, mCps, 0x594);
WWHD_SIZE(daMozo_c, 0x6CC);

void mozoStatusConstruct(void *);
void mozoCollisionSet(void *, void *);
s32 mozoDistanceAngle(s32, s32);
void mozoCapsuleGeometryConstruct(void *);
void mozoCapsuleSetEnds(void *, void *, void *);
s32 mozoAtan2(f32, f32);
void mozoVectorDifference(void *, void *, void *);
BOOL mozoVectorNormalize(void *);
void mozoQuatFromVectors(void *, void *, void *);
void mozoStatusDestroy(void *, s32);
void mozoColliderDestroy(void *, s32);
void mozoGlobalStatusConstruct(void *);
void mozoStatusInit(void *, s32, s32, void *);
void mozoColliderConstruct(void *);
void mozoColliderSet(void *, void *);
u32 mozoGetPlay();
s32 mozoResLoad(void *, const char *);
void mozoResDelete(void *, const char *);
s32 mozoGetReverb(s32);
BOOL mozoEventEnd(void *, const char *);
u32 mozoGetEnvironment();
void mozoSettingTev(void *, s32, void *, void *);
void mozoLightTev(void *, void *, void *);
u32 mozoParticleSet(void *, s32, u32, void *, s32, s32, s32, s32, s32, s32, s32, s32);
BOOL mozoIsActor(void *);
void mozoActorConstruct(void *);
void mozoActorDestroy(void *, s32);
u32 mozoSearchActor(void *, void *);
u32 mozoCreateChild(s32, u32, u32, void *, s32, s32, void *, s32, s32);
BOOL mozoEntryHeap(void *, void *, u32);
void mozoCullMin(void *, f32, f32, f32);
void mozoCullMax(void *, f32, f32, f32);
s32 mozoSearchAngle(void *, void *);
void mozoOrderEvent(void *, const char *, s32, u32);
void mozoLevelSound(u32, void *);
void mozoSoundStart(u32, void *, s32, s32);
void mozoSoundStop(void *, u32);
void mozoSoundDelete(void *);
void mozoMorfSetAnm(void *, void *, s32, f32, f32, f32, f32, s32);
u32 mozoMorfConstruct(s32, void *, s32, s32, void *, s32, f32, s32, s32, s32, s32, s32, u32);
void mozoMorfPlay(void *, s32, s32, s32);
void mozoMorfEntry(void *);
void mozoMorfCalc(void *);
void mozoAnimationPlay(void *);
void mozoBtkConstruct(void *);
BOOL mozoBtkInit(void *, void *, void *, s32, s32, f32, s32, s32, s32, s32);
void mozoBtkEntry(void *, void *, f32);
void mozoBrkConstruct(void *);
BOOL mozoBrkInit(void *, void *, void *, s32, s32, f32, s32, s32, s32, s32);
void mozoBrkEntry(void *, void *, f32);
u32 mozoCreateHioChild(const char *, void *);
void mozoDeleteHioChild(s32);
void mozoMtxYrotS(void *, s32);
void mozoMtxXYZrotM(void *, s32, s32, s32);
void mozoMtxYrotM(void *, s32);
void mozoMtxTranslateM(f32, f32, f32);
void mozoMtxQuatM(void *);
u32 mozoGetResource(void *, void *, s32);
void mozoAssertion(const char *, s32, const char *);
void *mozoAllocate(u32);
void mozoDeallocate(void *);
BOOL mozoFrameCheck(void *, f32);
u32 mozoGetJointTree(void *);
u32 mozoGetJoint(void *);
void mozoEmitterMatrix(void *, void *, void *);
void mozoVectorAdd(void *, void *, void *);
void mozoVectorSubtract(void *, void *, void *);
f32 mozoVectorSquare(void *);
void mozoVectorScale(void *, void *, f32);
f32 mozoVectorDot(void *, void *);
void mozoMtxMultVec(void *, void *, void *);
void mozoMtxCopy(void *, void *);
void mozoMtxConcat(void *, void *, void *);
void mozoMtxTranslation(void *, f32, f32, f32);
void mozoQuatSlerp(void *, void *, void *, f32);
void mozoCircleConstruct(void *);
void mozoAngleConstruct(void *);
void mozoRegisterGlobal(void *);
f32 mozoSqrt(f32);

// HD: verified code identities in the GHS member pointer; type 2 also uses beam processing.
enum MozoProcess : u32 {
    MOZO_WAIT = 0x021D53D8,
    MOZO_SEARCH_BEAM = 0x021D551C,
    MOZO_SEARCH_FIRE = 0x021D593C,
    MOZO_TO_WAIT = 0x021D5E08,
};
enum MozoType { MOZO_BEAM = 0, MOZO_FIRE = 1, MOZO_BEAM_ALT = 2 };
enum MozoAnimation {
    MOZO_ANM_WAIT = 0,
    MOZO_ANM_SEARCH = 1,
    MOZO_ANM_TO_WAIT = 2,
    MOZO_ANM_FIRE = 3,
    MOZO_ANM_FIRE_REVERSE = 4
};
namespace mozo_tu {
constexpr u32 safeString = 0x10015254;
constexpr u32 beamHio = 0x1001528C;
constexpr u32 fireHio = 0x1001529C;
constexpr u32 hio = 0x100152AC;
constexpr u32 actor = 0x1001527C;
constexpr u32 status = 0x1004AE88;
constexpr u32 globalStatus = 0x1004AEC0;
constexpr u32 shapeBase = 0x100015A8;
constexpr u32 bounds = 0x1001526C;
constexpr u32 shape = 0x1004AF70;
constexpr u32 hitInfo = 0x1004AF18;
constexpr u32 capsule = 0x1004AF60;
} // namespace mozo_tu

struct MozoWords3 {
    be<u32> mWords[3];
};
struct MozoCollisionSource {
    be<u32> mWords[19];
};

namespace mozo_tu {
constexpr u32 assertFile = 0x100152E8;
constexpr u32 brkAssertion = 0x100152F8;
constexpr u32 btkAssertion = 0x10015308;
constexpr u32 eventName = 0x10015318;
constexpr u32 hioName = 0x1001535C;
constexpr u32 loadArchive = 0x10015364;
constexpr u32 deleteArchive = 0x10015324;
constexpr u32 searchByIdJudge = 0x025E1234;
} // namespace mozo_tu

WWHD_OFFSET(MozoModel, mpMatrices, 0x2C);
WWHD_OFFSET(MozoModel, mUserArea, 0xB8);
WWHD_OFFSET(MozoModel, mBaseScale, 0xBC);
WWHD_OFFSET(MozoMatrices, mFlags, 0x4);
WWHD_OFFSET(MozoMatrices, mpAnimationMatrices, 0x10);
WWHD_OFFSET(MozoJoint, mJntNo, 0x4);
WWHD_OFFSET(MozoJoint, mpCallback, 0x8);
WWHD_OFFSET(MozoModelData, mJointCapacity, 0x4);
WWHD_OFFSET(MozoModelData, mpJoints, 0x8);
WWHD_OFFSET(MozoJointTree, mJointNum, 0x8);
WWHD_OFFSET(MozoPlay, mCollision, 0x26A4);
WWHD_OFFSET(MozoPlay, mEventFlags, 0x52B8);
WWHD_OFFSET(MozoPlay, mEventManager, 0x52C4);
WWHD_OFFSET(MozoPlay, mpParticleControl, 0x5AB0);
WWHD_OFFSET(MozoBeam, mBeamScale, 0x718);
WWHD_OFFSET(MozoBeam, mBeamTimer, 0x798);
WWHD_OFFSET(MozoBeam, mBeamState, 0x844);
WWHD_OFFSET(MozoBeam, mThickness, 0x8E4);
WWHD_OFFSET(MozoPlayer, mStatueRangeState, 0x3CC);
WWHD_OFFSET(MozoPlayerVtable, mpCheckStatus, 0x4C);
WWHD_OFFSET(MozoEmitter, mRate, 0x34);
WWHD_OFFSET(MozoEmitter, mFrameCount, 0x5C);
WWHD_OFFSET(MozoEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(MozoEmitter, mStatus, 0x254);
WWHD_OFFSET(MozoCapsule, mBoundsVtable, 0x110);
WWHD_OFFSET(MozoCapsule, mShapeVtable, 0x114);
WWHD_OFFSET(daMozo_HIO_c, mNo, 0x0);
WWHD_OFFSET(daMozo_HIO_c, m08, 0x4);
WWHD_OFFSET(daMozo_HIO_c, mpBeamChild, 0x8);
WWHD_OFFSET(daMozo_HIO_c, mpFireChild, 0xC);
WWHD_OFFSET(daMozo_HIO_c, mBeamChild, 0x10);
WWHD_OFFSET(daMozo_HIO_c, mFireChild, 0x24);
WWHD_OFFSET(daMozo_HIO_c, m3C, 0x38);
WWHD_OFFSET(daMozo_HIO_c, m40, 0x3C);
WWHD_OFFSET(daMozo_HIO_c, m44, 0x40);
WWHD_OFFSET(daMozo_HIO_c, __vtbl, 0x44);
