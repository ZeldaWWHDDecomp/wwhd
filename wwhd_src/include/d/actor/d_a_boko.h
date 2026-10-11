// Enemy weapons: tww member names; actor base expands 0x290 -> 0x3AC in HD.
// HD: BRK, light and line material storage also expand; every used offset is asserted.
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

struct ke_c_s {
    cXyz m000[10];
    cXyz m078[10];
    // HD: cached ground heights are used when the staggered query is skipped.
    be<f32> mGroundHeight[10];
    u8 _118[0xC];
};
static_assert(sizeof(ke_c_s) == 0x124);
struct BokoLineRow {
    gptr<cXyz> mPositions;
    u8 _04[12];
};
// HD: line material storage expands from 0x1C to 0x148 bytes.
struct LineKe {
    u8 mLineMaterial[0x130];
    be<u32> mVtable;
    u8 _134[0x10];
    gptr<BokoLineRow> mRows;
    ke_c_s m001C[16];
    be<f32> mDamping;
};
static_assert(offsetof(LineKe, m001C) == 0x148);
static_assert(offsetof(LineKe, mDamping) == 0x1388);
static_assert(sizeof(LineKe) == 0x138C);

struct BokoCapsule : dCcD_GObjInf {
    cXyz mAabMin;
    cXyz mAabMax;
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
    cXyz mStart;
    cXyz mEnd;
    be<u32> mGeometryVtable;
    be<f32> mRadius;
};
static_assert(sizeof(BokoCapsule) == 0x138);
static_assert(offsetof(BokoCapsule, mRadius) == 0x134);
struct BokoEmitterHD {
    u8 _000[0x28];
    cXyz mEmitterDir;
    u8 _034[0x238 - 0x34];
    cXyz mGlobalScale;
    u8 _244[0x10];
    be<u32> mStatus;
};
static_assert(offsetof(BokoEmitterHD, mGlobalScale) == 0x238);
static_assert(offsetof(BokoEmitterHD, mStatus) == 0x254);
struct BokoDarkStatusHD {
    u8 _00[0x10];
    be<f32> mShadowRadius[4];
};

struct daBoko_c : fopAc_ac_c {
    enum Type {
        Type_BOKO_STICK_e, Type_MACHETE_e, Type_STALFOS_MACE_e,
        Type_DARKNUT_SWORD_e, Type_MOBLIN_SPEAR_e, Type_PGANON_SWORD_e,
        Type_COUNT_e, Type_UNK_7_e,
        // HD: a second placement parameter initializes a lit Boko stick.
        Type_LIT_STICK_e,
    };
    enum Mode {
        Mode_WAIT_e, Mode_MOVE_e, Mode_ENEMY_CARRY_e, Mode_PLAYER_CARRY_e,
        Mode_4_e, Mode_5_e, Mode_THROW_e,
    };

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_brkAnm mBrkAnm;
    /* 0x430 */ be<u8> mNowMode, m2B9, mbThrow, m2BB, mbFloorFlg;
    /* 0x435 */ u8 _435[3];
    /* 0x438 */ be<s16> m2C0, m2C2, mFlameTimer;
    /* 0x43E */ u8 _43E[2];
    /* 0x440 */ be<s16> m2C8, m2CA;
    /* 0x444 */ be<s16> mChainTimer, mSpawnDelay;
    /* 0x448 */ u8 _448[4];
    /* 0x44C */ cXyz m2D0, m2DC;
    /* 0x464 */ u8 _464[12];
    /* 0x470 */ cXyz m2F4;
    /* 0x47C */ LIGHT_INFLUENCE mLight;
    /* 0x4A0 */ cXyz m320;
    /* 0x4AC */ dPa_followEcallBack mParticleCallBack;
    /* 0x4C0 */ dBgS_ObjAcch mAcch;
    /* 0x684 */ dBgS_AcchCir mAcchCir;
    /* 0x6C4 */ dCcD_Stts mStts;
    /* 0x700 */ dCcD_Sph mSph;
    /* 0x82C */ BokoCapsule mCps;
    /* 0x964 */ be<f32> m7E4;
    // HD: four flame matrices are stored directly after the fluctuation scale.
    /* 0x968 */ Mtx34 mAlphaModelMtx[4];
    /* 0xA28 */ gptr<LineKe> mpLineKe;
    // HD: GHS member pointer descriptor, retained byte-for-byte.
    /* 0xA2C */ be<s16> mProcDelta, mProcSlot;
    /* 0xA30 */ be<u32> mCurrentProc;
    BOOL checkNoDraw();
    void keDraw();
    BOOL draw();
    void setTopRootPos(s32 usePosition);
    BOOL bokoDelete();
    BOOL procWait_init();
    BOOL procMove_init();
    void getTopPos(cXyz* out);
    void getBlurRootPos(cXyz* out);
    void setBaseMatrix();
    BOOL createHeap();
    void setRoomInfo();
    void setThrowReverse(s16 direction);
    s32 create();
    void setFlameEffect();
    void keCalc();
    BOOL execute();
    void keCalc1(ke_c_s* chainObject, s32 index);
    BOOL procThrow();
    BOOL procWait();
    BOOL procCarry();
    BOOL procMove();
};
WWHD_OFFSET(daBoko_c, mPhase, 0x3AC);
WWHD_OFFSET(daBoko_c, mpModel, 0x3B4);
WWHD_OFFSET(daBoko_c, mBrkAnm, 0x3B8);
WWHD_OFFSET(daBoko_c, mNowMode, 0x430);
WWHD_OFFSET(daBoko_c, mChainTimer, 0x444);
WWHD_OFFSET(daBoko_c, m2D0, 0x44C);
WWHD_OFFSET(daBoko_c, mLight, 0x47C);
WWHD_OFFSET(daBoko_c, m320, 0x4A0);
WWHD_OFFSET(daBoko_c, mParticleCallBack, 0x4AC);
WWHD_OFFSET(daBoko_c, mAcch, 0x4C0);
WWHD_OFFSET(daBoko_c, mAcchCir, 0x684);
WWHD_OFFSET(daBoko_c, mStts, 0x6C4);
WWHD_OFFSET(daBoko_c, mSph, 0x700);
WWHD_OFFSET(daBoko_c, mCps, 0x82C);
WWHD_OFFSET(daBoko_c, m7E4, 0x964);
WWHD_OFFSET(daBoko_c, mAlphaModelMtx, 0x968);
WWHD_OFFSET(daBoko_c, mpLineKe, 0xA28);
WWHD_OFFSET(daBoko_c, mCurrentProc, 0xA30);
WWHD_SIZE(daBoko_c, 0xA34);

// HD: word stores preserve signaling NaNs and the original load/store order.
template<class T> inline be<u32>& bokoWord(T& field) { static_assert(sizeof(T)==4); return reinterpret_cast<be<u32>&>(field); }

// HD: guest temporaries retain the original eight-byte stack linkage area.
struct BokoVectorFrame {
    be<u32> linkage[2];
    cXyz payload;
};
static_assert(offsetof(BokoVectorFrame, payload) == 8);
static_assert(sizeof(BokoVectorFrame) == 0x14);

struct BokoName {
    be<u32> string, vtable;
};
struct BokoNameFrame {
    be<u32> linkage[2];
    BokoName payload;
};
struct BokoGroundFrame {
    be<u32> linkage[2];
    dBgS_GndChk payload;
};
struct BokoLineFrame {
    be<u32> linkage[2];
    dBgS_LinChk payload;
};
struct BokoFloatFrame {
    be<u32> linkage[2];
    be<f32> payload;
};
static_assert(sizeof(BokoNameFrame) == 0x10);
static_assert(sizeof(BokoGroundFrame) == 0x5C);
static_assert(sizeof(BokoLineFrame) == 0x74);
static_assert(sizeof(BokoFloatFrame) == 0xC);

// Guest entry points used by this unit.
s32 bokoAtan2(f32, f32);
f32 bokoRandom(f32);
f32 bokoRandomSymmetric(f32);
f32 bokoSqrt(f32);
dComIfG_inf_c* bokoGameInfo();
dScnKy_env_light_c* bokoEnvironment();
s32 bokoReverb(s32);
void bokoDeleteSound(void*);
void bokoCutLight(void*);
void bokoSetLight(void*);
void bokoModelEntry(void*);
void bokoModelCalculate(void*);
void* bokoAllocate(u32);
void bokoFree(void*);
void bokoRegisterGlobal(void*);
void bokoInitializeAngles(void*);
void bokoInitializeFlags(void*);
void bokoMatrixPush();
void bokoMatrixPop();
void bokoTranslateMatrix(f32, f32, f32);
void bokoScaleMatrix(f32, f32, f32);
void bokoActorConstruct(void*);
void bokoActorDestruct(void*, s32);
void bokoBrkConstruct(void*);
void bokoAcchConstruct(void*);
void bokoCircleConstruct(void*);
void bokoStatusConstruct(void*);
void bokoGlobalStatusConstruct(void*);
void bokoSphereConstruct(void*);
void bokoObjectInfoConstruct(void*);
void bokoCapsuleConstruct(void*);
void bokoGroundConstruct(void*);
void bokoLineConstruct(void*);
void bokoCheckDestruct(void*, s32);
void bokoLineDestruct(void*, s32);
void bokoSphereDestruct(void*, s32);
void bokoObjectInfoDestruct(void*, s32);
void bokoStatusDestruct(void*, s32);
void bokoCircleDestruct(void*, s32);
void bokoAcchDestruct(void*, s32);
void bokoCancelCarry(void*);
void bokoStartCarry(void*, s32);
void bokoMovePosition(void*, void*);
void bokoRequestDelete(void*);
void bokoEndFlame(void*);
void bokoFlameCallbackConstruct(void*, s32, s32);
void bokoLineMaterialConstruct(void*);
BOOL bokoLineMaterialInitialize(void*, s32, s32, s32);
f32 bokoGroundCross(void*, void*);
BOOL bokoLineCross(void*, void*);
void bokoRotateZXY(void*, s16, s16, s16);
void bokoRotateYSet(void*, s16);
void bokoRotateXSet(void*, s16);
void bokoRotateX(void*, s32);
void bokoRotateY(void*, s32);
void bokoMatrixToRotation(void*, void*);
void bokoMatrixTranslation(void*, f32, f32, f32);
void bokoCopyMatrix(void*, void*);
void bokoTransformVector(void*, void*, void*);
void bokoTransformStackVector(void*, void*);
f32 bokoVectorSquareMagnitude(void*);
void bokoAddVector(void*, void*, void*);
void bokoVectorAdd(void*, void*, void*);
void bokoVectorSubtract(void*, void*, void*);
void bokoVectorMultiply(void*, void*, f32);
void bokoCallFlame(void*);
void bokoCallChain(void*, void*, s32);
BOOL bokoCallMove(void*);
BOOL bokoCallCarry(void*);
BOOL bokoWaterHeight(void*, void*);
void bokoPlayAnimation(void*);
BOOL bokoAttackHit(void*);
void bokoResetAttackHit(void*);
void bokoResetTargetHit(void*);
BOOL bokoTargetHit(void*);
dCcD_GObjInf* bokoTargetHitObject(void*);

void bokoUpdateLine(void*, s32, void*, s32, void*, f32);
void bokoSortLine(void*, void*);
void bokoSetupLighting(void*, s32, void*, void*);
void bokoModelLighting(void*, void*, void*);
void bokoEnterBrk(void*, void*, f32);
void bokoDeleteResource(void*, void*);
s32 bokoLoadResource(void*, void*);
void bokoAssert(void*, s32, void*);
void* bokoGetResource(void*, void*, u32);
void* bokoCreateModel(void*, u32, u32);
BOOL bokoInitializeBrk(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32);
u8 bokoPolygonRoom(void*, void*);
u8 bokoPolygonColor(void*, void*);
void bokoStartSound(u32, void*, u32, s32);
s32 bokoAngleDistance(s32, s32);
using BokoHeapCallback = BOOL (*)(daBoko_c*);
BOOL bokoCreateSolidHeap(void*, BokoHeapCallback, u32);
void bokoCullMinimum(void*, f32, f32, f32);
void bokoCullMaximum(void*, f32, f32, f32);
void bokoSetCircleWall(void*, f32, f32);
void bokoSetAcch(void*, void*, void*, void*, s32, void*, void*, void*, void*);
void bokoInitializeStatus(void*, s32, s32, void*);
void bokoSetSphere(void*, void*);
void bokoSetCapsule(void*, void*);
BokoDarkStatusHD* bokoDarkStatus();
void bokoApproachFloat(void*, f32, f32, f32);
void bokoSetParticle(void*, u32, u32, void*, void*, void*, u32, void*, s32, void*, void*, void*);
void bokoSetSimpleParticle(void*, u32, void*, u32, void*, void*, u32);
void bokoChaseFloat(void*, f32, f32);
void bokoSetSphereCenter(void*, void*);
void bokoRegisterCollision(void*, void*);
void bokoSetCollisionMass(void*, void*, u32);
BOOL bokoCheckEventBit(void*, u32);
void bokoCorrectPosition(void*, void*);
void bokoSetCapsuleEnds(void*, void*, void*);
void bokoSetLine(void*, void*, void*, void*);
cM3dGPla* bokoTrianglePlane(void*, u32, u32);
void bokoApproachAngle(void*, s32, s32, s32, s32);
BOOL bokoCheckMovingBackground(void*, void*);
fopAc_ac_c* bokoBackgroundActor(void*, u32);

#include "SSystem/SComponent/c_m3d_g_pla.h"

struct BokoArchivePtr {
    be<u32> address;
    char* get() const { return gabi::at<char>(address); }
};
static_assert(sizeof(BokoArchivePtr) == 4);

struct BokoStageInfo {
    u8 _00[12];
    be<u32> mFlags;
};
static_assert(offsetof(BokoStageInfo, mFlags) == 0xC);

struct BokoLineVtable {
    u8 _00[0x14];
    gfn mGetSortIndex;
};
inline s32 bokoLineSortIndex(LineKe* line) {
    auto* vtable = gabi::at<BokoLineVtable>(line->mVtable);
    return gabi::call<s32>(vtable->mGetSortIndex.get(), line);
}
struct BokoPlayerVtable {
    u8 _00[0xCC];
    gfn mCheckPlayerNoDraw;
};
inline BOOL bokoPlayerNoDraw(fopAc_ac_c* player) {
    auto* vtable = gabi::at<BokoPlayerVtable>(player->__vtbl);
    return gabi::call<BOOL>(vtable->mCheckPlayerNoDraw.get(), player);
}
struct BokoJointNodeHD {
    u8 _00[0x10];
    gptr<J3DMaterial> mMaterial;
};
struct BokoModelDataHD : J3DModelData {
    u8 _14[0x48 - 0x14];
    be<u32> mBrkAnimation;
};
static_assert(offsetof(BokoModelDataHD, mBrkAnimation) == 0x48);
// HD: preserve the eight-byte GHS descriptor while using the shared PTMF dispatcher.
inline const gabi::ptmf<void (daBoko_c::*)()>& bokoCurrentProcedure(daBoko_c& actor) {
    return *reinterpret_cast<const gabi::ptmf<void (daBoko_c::*)()>*>(&actor.mProcDelta);
}

// HD: line sort packets and the mass manager reside inside the game-info singleton.
inline void* bokoLineSortPacket(dComIfG_inf_c* info, s32 index) {
    return gabi::at<void>(gabi::ea(info) + 0x5FB4 + u32(index) * 0x9C);
}
inline void* bokoCollisionMass(dComIfG_inf_c* info) {
    return gabi::at<void>(gabi::ea(info) + PLAY_CCMASS);
}

using BokoResourceControlPointer = gptr<void>;
// HD: the save singleton places dSv_info_c after a 0x20-byte prefix.
struct BokoSaveSingleton {
    u8 _00[0x20];
    dSv_info_c save;
};
static_assert(offsetof(BokoSaveSingleton, save) == 0x20);
using BokoGameInfoPointer = gptr<BokoSaveSingleton>;
using BokoMatrixPointer = gptr<Mtx34>;
enum BokoAssertText : u32 {
    BOKO_SOURCE_FILE = 0x1000A51C,
    BOKO_TYPE_ASSERT = 0x1000A4F8,
    BOKO_MODEL_ASSERT = 0x1000A52C,
};
inline char* bokoAssertText(BokoAssertText text) { return gabi::at<char>(text); }

// HD: compiler-generated vtables and SafeString metadata belong to this TU.
enum BokoTuMetadata : u32 {
    BOKO_ACTOR_VTABLE = 0x1000A478,
    BOKO_ACCH_GROUP_VTABLE = 0x1000A3D8,
    BOKO_ACCH_CHECK_VTABLE = 0x1000A3C8,
    BOKO_ACCH_POLY_VTABLE = 0x1000A3E8,
    BOKO_STATUS_VTABLE = 0x1004AE88,
    BOKO_GLOBAL_STATUS_VTABLE = 0x1004AEC0,
    BOKO_SHAPE_BASE_VTABLE = 0x100015A8,
    BOKO_CAPSULE_AAB_VTABLE = 0x1000A318,
    BOKO_CAPSULE_HIT_VTABLE = 0x1004AF18,
    BOKO_CAPSULE_GEOMETRY_VTABLE = 0x1004AF60,
    BOKO_CAPSULE_SHAPE_VTABLE = 0x1004AF70,
    BOKO_GROUND_POLY_INFO_VTABLE = 0x1000A358,
    BOKO_GROUND_POLY_PASS_VTABLE = 0x1000A378,
    BOKO_GROUP_DESTRUCTOR_VTABLE = 0x1000A338,
    BOKO_LINE_POLY_PASS_VTABLE = 0x1000A428,
    BOKO_LINE_POLY_DESTRUCTOR_VTABLE = 0x1000A328,
    BOKO_OBJECT_GROUND_CHECK_VTABLE = 0x1000A388,
    BOKO_OBJECT_GROUND_POLY_INFO_VTABLE = 0x1000A398,
    BOKO_OBJECT_GROUND_GROUP_VTABLE = 0x1000A3A8,
    BOKO_OBJECT_GROUND_POLY_PASS_VTABLE = 0x1000A3B8,
    BOKO_OBJECT_LINE_CHECK_VTABLE = 0x1000A438,
    BOKO_OBJECT_LINE_POLY_INFO_VTABLE = 0x1000A448,
    BOKO_OBJECT_LINE_GROUP_VTABLE = 0x1000A458,
    BOKO_OBJECT_LINE_POLY_PASS_VTABLE = 0x1000A468,
    BOKO_SAFE_STRING_VTABLE = 0x1000A300,
    BOKO_BRK_ARCHIVE_NAME = 0x1000A4FC,
    BOKO_GROUND_CHECK_VTABLE = 0x1000A348,
    BOKO_GROUND_GROUP_VTABLE = 0x1000A368,
    BOKO_LINE_POLY_INFO_VTABLE = 0x1000A408,
    BOKO_LINE_CHECK_VTABLE = 0x1000A3F8,
    BOKO_LINE_GROUP_VTABLE = 0x1000A418,
};

WWHD_OFFSET(daBoko_c, m2B9, 0x431);
WWHD_OFFSET(daBoko_c, mbThrow, 0x432);
WWHD_OFFSET(daBoko_c, m2BB, 0x433);
WWHD_OFFSET(daBoko_c, mbFloorFlg, 0x434);
WWHD_OFFSET(daBoko_c, m2C0, 0x438);
WWHD_OFFSET(daBoko_c, m2C2, 0x43A);
WWHD_OFFSET(daBoko_c, mFlameTimer, 0x43C);
WWHD_OFFSET(daBoko_c, m2C8, 0x440);
WWHD_OFFSET(daBoko_c, m2CA, 0x442);
WWHD_OFFSET(daBoko_c, mSpawnDelay, 0x446);
WWHD_OFFSET(daBoko_c, m2DC, 0x458);
WWHD_OFFSET(daBoko_c, m2F4, 0x470);
WWHD_OFFSET(daBoko_c, mProcDelta, 0xA2C);
WWHD_OFFSET(daBoko_c, mProcSlot, 0xA2E);

daBoko_c* daBoko_c__ct(daBoko_c* actor);

WWHD_OFFSET(ke_c_s, m000, 0x00);
WWHD_OFFSET(ke_c_s, m078, 0x78);
WWHD_OFFSET(ke_c_s, mGroundHeight, 0xF0);
WWHD_SIZE(BokoLineRow, 0x10);
WWHD_OFFSET(LineKe, mVtable, 0x130);
WWHD_OFFSET(LineKe, mRows, 0x144);
WWHD_OFFSET(BokoCapsule, mAabMin, 0xF8);
WWHD_OFFSET(BokoCapsule, mAabMax, 0x104);
WWHD_OFFSET(BokoCapsule, mAabVtable, 0x110);
WWHD_OFFSET(BokoCapsule, mShapeVtable, 0x114);
WWHD_OFFSET(BokoCapsule, mStart, 0x118);
WWHD_OFFSET(BokoCapsule, mEnd, 0x124);
WWHD_OFFSET(BokoCapsule, mGeometryVtable, 0x130);
WWHD_OFFSET(BokoEmitterHD, mEmitterDir, 0x28);
WWHD_SIZE(BokoEmitterHD, 0x258);
WWHD_OFFSET(BokoDarkStatusHD, mShadowRadius, 0x10);
WWHD_SIZE(BokoDarkStatusHD, 0x20);
WWHD_SIZE(BokoName, 8);
WWHD_OFFSET(BokoNameFrame, payload, 8);
WWHD_OFFSET(BokoGroundFrame, payload, 8);
WWHD_OFFSET(BokoLineFrame, payload, 8);
WWHD_OFFSET(BokoFloatFrame, payload, 8);
WWHD_SIZE(BokoStageInfo, 0x10);
WWHD_OFFSET(BokoLineVtable, mGetSortIndex, 0x14);
WWHD_SIZE(BokoLineVtable, 0x18);
WWHD_OFFSET(BokoPlayerVtable, mCheckPlayerNoDraw, 0xCC);
WWHD_SIZE(BokoPlayerVtable, 0xD0);
WWHD_OFFSET(BokoJointNodeHD, mMaterial, 0x10);
WWHD_SIZE(BokoJointNodeHD, 0x14);
WWHD_SIZE(BokoModelDataHD, 0x4C);
