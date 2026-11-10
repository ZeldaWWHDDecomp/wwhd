/* Pushable box, GameCube member names and WWHD layout.
 * HD: moving-background actor and lighting growth shift Act_c members by +0x118.
 * Collision work and attributes retain their GameCube sizes. */
#pragma once
#include "bindings.h"
s32 moveboxGetParameterBits(const fopAc_ac_c*,s32,s32);
namespace daObjMovebox {
struct MoveboxPathPointHD {
    u8 mArgs[4];
    cXyz m_position;
};
WWHD_OFFSET(MoveboxPathPointHD, m_position, 4);
WWHD_SIZE(MoveboxPathPointHD, 0x10);
struct MoveboxPathHD {
    be<u16> m_num;
    u8 mTail[0xA];
};
WWHD_SIZE(MoveboxPathHD, 0xC);
// HD: water checks place their line-check base at +0x10.
struct MoveboxWaterDestructorHD {
    u8 mPrefix[0x10];
    u8 mLinePrefix[0x10];
    be<u32> mPolyVtable, mPassVtable;
    u8 mPassFlags[8];
    be<u32> mGroupVtable;
};
WWHD_OFFSET(MoveboxWaterDestructorHD, mPolyVtable, 0x20);
WWHD_OFFSET(MoveboxWaterDestructorHD, mPassVtable, 0x24);
WWHD_OFFSET(MoveboxWaterDestructorHD, mGroupVtable, 0x30);
WWHD_SIZE(MoveboxWaterDestructorHD,0x34);
struct MoveboxRegistrationHD {
    be<u32> mWords[3];
};
WWHD_SIZE(MoveboxRegistrationHD,0xC);
struct MoveboxRegistrationsHD {
    MoveboxRegistrationHD mTouch, mHeader, mByte, mFlag, mGround, mWater, mWall;
};
WWHD_SIZE(MoveboxRegistrationsHD, 0x54);
extern MoveboxRegistrationsHD& l_registrations;
struct MoveboxWaterWorkHD {
    u8 mPrefix[0x38];
    cXyz mGroundPos;
    be<f32> mRoofY, mHeight;
    u8 mTail[4];
};
WWHD_OFFSET(MoveboxWaterWorkHD, mGroundPos, 0x38);
WWHD_OFFSET(MoveboxWaterWorkHD, mRoofY, 0x44);
WWHD_OFFSET(MoveboxWaterWorkHD, mHeight, 0x48);
WWHD_SIZE(MoveboxWaterWorkHD, 0x50);
struct MoveboxCollisionWorkHD {
    MoveboxWaterWorkHD mWater;
    dBgS_ObjGndChk mGround[23];
    dBgS_ObjLinChk mWall[23];
};
WWHD_OFFSET(MoveboxCollisionWorkHD, mGround, 0x50);
WWHD_OFFSET(MoveboxCollisionWorkHD, mWall, 0x7DC);
WWHD_SIZE(MoveboxCollisionWorkHD, 0x1190);
extern MoveboxCollisionWorkHD& l_collisionWork;
enum { MOVEBOX_CRR_TRANSLATE = 0x024EE76C, MOVEBOX_CRR_TYPICAL = 0x024EE658 };
struct MoveboxBackgroundHD {
    u8 mPrefix[0xA8];
    be<u32> mCrrFunc;
    u8 mPadding[4];
    be<u32> mRideCallback, mPushPullCallback;
};
WWHD_OFFSET(MoveboxBackgroundHD, mCrrFunc, 0xA8);
WWHD_OFFSET(MoveboxBackgroundHD, mRideCallback, 0xB0);
WWHD_OFFSET(MoveboxBackgroundHD, mPushPullCallback, 0xB4);
WWHD_SIZE(MoveboxBackgroundHD, 0xB8);
inline MoveboxBackgroundHD* backgroundHD(dBgW* bg) { return reinterpret_cast<MoveboxBackgroundHD*>(bg); }

struct Act_c;
struct MoveboxPathHD;
struct Attr_c {
    be<s16> m00, m02, m04, m06, m08, m0A;
    be<f32> m0C, m10, m14, m18, m1C, m20, mLandSmokeScale, m28, m2C, m30, m34;
    be<s16> m38;
    u8 _3A[2];
    be<f32> m3C, m40, m44, m48, m4C, m50, m54;
    be<s32> mModelFileIndex, mDZBFileIndex, mDZBHeapSize;
    be<f32> mScaleY, m68, m6C, mScaleXZ, m74;
    be<u32> mMoveSE, mCantMoveSE, mNormalFallSE, mWaterFallSE, mMagmaFallSE;
    be<s16> mCullMinX, mCullMinY, mCullMinZ, mCullMaxX, mCullMaxY, mCullMaxZ;
    be<u8> mbUseBGTevType, mbCastsShadow, m9A;
    u8 _9B;
};
WWHD_SIZE(Attr_c, 0x9C);
struct MoveboxAttrStorageHD {
    Attr_c mItems[13];
};
WWHD_SIZE(MoveboxAttrStorageHD,0x7EC);
extern MoveboxAttrStorageHD& l_attributes;
struct MoveboxArchiveNamesHD {
    gptr<const char> mNames[13];
};
WWHD_SIZE(MoveboxArchiveNamesHD,0x34);
extern MoveboxArchiveNamesHD& l_archiveNames;
struct BgcSrc_c {
    be<f32> m00, m04, m08, m0C;
};
WWHD_SIZE(BgcSrc_c,0x10);
struct MoveboxLineSourcesHD {
    BgcSrc_c mSmall[5], mLarge[23];
};
WWHD_OFFSET(MoveboxLineSourcesHD,mLarge,0x50);
WWHD_SIZE(MoveboxLineSourcesHD,0x1C0);
extern MoveboxLineSourcesHD& l_lineSources;
struct Bgc_c {
    enum State_e {
        BgcState_ON_GROUND_e = 0x01,
        BgcState_JUST_LEFT_GROUND_e = 0x02,
        BgcState_JUST_HIT_GROUND_e = 0x04,
        BgcState_ABOVE_WATER_e = 0x08,
        BgcState_IN_WATER_e = 0x10,
        BgcState_JUST_HIT_WATER_e = 0x20,
    };
    static BgcSrc_c (&M_lin5)[5];
    static BgcSrc_c (&M_lin20)[23];
    static dBgS_ObjGndChk (&M_gnd_work)[23];
    static MoveboxWaterWorkHD& M_wrt_work;
    static dBgS_ObjLinChk (&M_wall_work)[23];
    be<f32> mGroundY[23];
    be<s32> mMaxGroundIdx;
    be<f32> mWaterY;
    cXyz mWallPos[23];
    be<s32> mWallIdx;
    be<f32> mNearestWallDist;
    be<u32> mStateFlags;
    void gnd_pos(const Act_c*, const BgcSrc_c*, s32, f32);
    void wrt_pos(const cXyz*);
    void wall_pos(const Act_c*, const BgcSrc_c*, s32, s16, f32);
    void proc_vertical(Act_c*);
    bool chk_wall_pre(const Act_c*, const BgcSrc_c*, s32, s16);
    bool chk_wall_touch(const Act_c*, const BgcSrc_c*, s16);
    bool chk_wall_touch2(const Act_c*, const BgcSrc_c*, s32, s16);
};
WWHD_OFFSET(Bgc_c,mMaxGroundIdx,0x5C);
WWHD_OFFSET(Bgc_c,mWaterY,0x60);
WWHD_OFFSET(Bgc_c,mWallPos,0x64);
WWHD_OFFSET(Bgc_c,mWallIdx,0x178);
WWHD_OFFSET(Bgc_c,mStateFlags,0x180);
WWHD_SIZE(Bgc_c, 0x184);

struct MoveboxStaticsHD {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mHeaderByte, mHeaderFlag;
    u8 mPadding[2];
    cXyz mSmokeBasePos[2];
    cXyz mWalkDirection[4];
    be<u32> mHeaderWords[4];
    cXyz mSmokeScale, mBreakScale;
    dBgS_ObjLinChk mTouchWork;
    be<u32> mTouchGuard, mBreakScaleGuard, mSmokeBaseGuard, mSmokeScaleGuard, mWalkDirectionGuard;
};
WWHD_OFFSET(MoveboxStaticsHD, mSmokeBasePos, 0xC);
WWHD_OFFSET(MoveboxStaticsHD, mWalkDirection, 0x24);
WWHD_OFFSET(MoveboxStaticsHD, mHeaderWords, 0x54);
WWHD_OFFSET(MoveboxStaticsHD, mSmokeScale, 0x64);
WWHD_OFFSET(MoveboxStaticsHD, mBreakScale, 0x70);
WWHD_OFFSET(MoveboxStaticsHD, mTouchWork, 0x7C);
WWHD_OFFSET(MoveboxStaticsHD, mTouchGuard, 0xE8);
WWHD_SIZE(MoveboxStaticsHD, 0xFC);
extern MoveboxStaticsHD& l_statics;
struct MoveboxEmitterHD {
    u8 mPrefix[0x34];
    be<f32> mRate;
    u8 mPaddingRate[0x58 - 0x38];
    be<f32> mSpread;
    u8 mPaddingSpread[4];
    be<u16> mLifeTime;
    u8 mPaddingLife[0x6C - 0x62];
    be<f32> mInitialVelAxis, mInitialVelDir;
};
WWHD_OFFSET(MoveboxEmitterHD, mLifeTime, 0x60);
WWHD_OFFSET(MoveboxEmitterHD, mInitialVelDir, 0x70);
WWHD_SIZE(MoveboxEmitterHD, 0x74);

struct EffSmokeCB : dPa_smokeEcallBack {
    /* 0x20 */ cXyz mPos;  /* was field_0x20: eff_set_slip_smoke_pos 02379010: PSMTXMultVec of the smoke base position into it (tww multVec(&base_pos[i])); eff_smoke_slip_start 0237918C passes it as the position of particle 0x2022 (ID_AK_JT_ELEMENTSMOKE00) */
    /* 0x2C */ csXyz mAngle;  /* was field_0x2C: eff_set_slip_smoke_pos 02379010: (0, push angle, 0) (tww set(0, angle, 0)); eff_smoke_slip_start 0237918C passes it as the angle of particle 0x2022 */
    u8 _32[2];
};
WWHD_SIZE(EffSmokeCB, 0x34);
struct Act_c : dBgS_MoveBgActor {
    enum Type {
        TYPE_BREAKABLE_WOODEN_CRATE = 0x0, // Actor names: Kkiba or DKkiba
        TYPE_BLACK_BOX = 0x1,              // Actor names: osiBLK0 or DBLK0
        TYPE_BLACK_BOX_WITH_STATUE = 0x2,  // Actor names: osiBLK1 or DBLK1
        TYPE_BIG_BLACK_BOX = 0x3,          // Actor names: MpwrB
        TYPE_WOODEN_CRATE = 0x4,           // Actor names: (N/A)
        TYPE_GOLDEN_CRATE = 0x5,           // Actor names: Hbox2
        TYPE_METAL_BOX = 0x6,              // Actor names: Hbox1
        TYPE_METAL_BOX_WITH_SPRING = 0x7,  // Actor names: Hjump1
        TYPE_WOODEN_CRATE_2 = 0x8,         // Actor names: Hseki2
        TYPE_WOODEN_CRATE_3 = 0x9,         // Actor names: Hseki7
        TYPE_MIRROR = 0xA,                 // Actor names: Mmrr
        TYPE_BLACK_BOX_WITH_MKIE = 0xB,    // Actor names: MkieBB
        TYPE_MOSSY_BLACK_BOX = 0xC,        // Actor names: Ecube
    };

    enum Mode {
        MODE_WAIT = 0x0,
        MODE_WALK = 0x1,
        MODE_AFLOAT = 0x2,
    };

    enum Prm_e {
        PRM_TYPE_W = 0x04,
        PRM_TYPE_S = 0x18,

        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x08,

        PRM_ITEMNO_W = 0x06,
        PRM_ITEMNO_S = 0x00,

        PRM_ITEMSAVE_W = 0x07,
        PRM_ITEMSAVE_S = 0x10,

        PRM_DMY_W = 0x01,
        PRM_DMY_S = 0x1E,

        PRM_BUOY_W = 0x01,
        PRM_BUOY_S = 0x1F,
    };
    static const be<s16> (&M_dir_base)[4];
    static gptr<const char> (&M_arcname)[13];
    static Attr_c (&M_attr)[13];
    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ Mtx34 mMtx;
    /* 0x418 */ gptr<J3DModel> mpModel;
    /* 0x41C */ be<s32> mMode;
    /* 0x420 */ dCcD_Stts mStts;
    /* 0x45C */ dCcD_Cyl mCyl;
    /* 0x58C */ Bgc_c mBgc;
    /* 0x710 */ be<s32> mType;
    /* 0x714 */ be<u16> mPrmZ;
    /* 0x716 */ be<u16> mPrmX;
    /* 0x718 */ gptr<MoveboxPathHD> mpPath;
    /* 0x71C */ be<s16> mAflBobAngle;  /* was m604: mode_afl 0237A0A8 adds attr m38 * (1 + cM_rnd()) to it each frame; the gravity gets attr m34 * cM_ssin(it) (tww mode_afl); Create 02377C1C 0 */
    /* 0x71E */ u8 _71E[2];
    /* 0x720 */ be<f32> mRideSinkAccel;  /* was m608: RideCallBack 02377308 adds the rider's push-down (attr m2C player / m30 other, scaled by distance), clamped; mode_afl 0237A0A8 adds it to the gravity and clears it */
    /* 0x724 */ be<f32> mSwayTargetX;  /* was m60C: RideCallBack 02377308 adds the rider's x offset times attr m74 * m44/m48; afl_sway 02379D00 clamps (mSwayTargetX, mSwayTargetZ) to attr m4C, springs mSwayX toward it, then clears it */
    /* 0x728 */ be<f32> mSwayTargetZ;  /* was m610: RideCallBack 02377308 adds the rider's z offset times attr m74 * m44/m48; afl_sway 02379D00 springs mSwayZ toward it, then clears it */
    /* 0x72C */ be<f32> mSwayX;  /* was m614: afl_sway 02379D00 += mSwaySpeedX, zeroed on a left/right wall touch; set_mtx 02377764 tilts the floating box by the up vector (mSwayX, 1, mSwayZ) (tww set_mtx) */
    /* 0x730 */ be<f32> mSwayZ;  /* was m618: afl_sway 02379D00 += mSwaySpeedZ, zeroed on a front/back wall touch; set_mtx 02377764 uses it as the up vector's z */
    /* 0x734 */ be<f32> mSwaySpeedX;  /* was m61C: afl_sway 02379D00: spring speed of mSwayX (attr m50 pull toward mSwayTargetX, attr m54 damping) */
    /* 0x738 */ be<f32> mSwaySpeedZ;  /* was m620: afl_sway 02379D00: spring speed of mSwayZ (attr m50 pull toward mSwayTargetZ, attr m54 damping) */
    /* 0x73C */ be<f32> mSinkDepth;  /* was m624: mode_afl 0237A0A8: mBgc.mWaterY - current.pos.y clamped to [0, attr m68]; set_mtx 02377764 rotates the floating box about that height (transM(0, it, 0) ... transM(0, -it, 0)); Create 02377C1C attr m68 * 0.5 */
    /* 0x740 */ be<s32> mGridX;  /* was m628: mode_wait 02379308 / mode_walk 02379834: x position in pushes (transM(it * attr m0C, 0, mGridZ * attr m0C)); mode_walk steps it by +-1 per finished push (directions 1 / 3) */
    /* 0x744 */ be<s32> mGridZ;  /* was m62C: mode_wait 02379308 / mode_walk 02379834: z position in pushes; mode_walk steps it by +-1 per finished push (directions 0 / 2) */
    /* 0x748 */ be<f32> mWalkAngleStep;  /* was m630: mode_wait 02379308: 0x8000 / the push length (attr m0A or m04); mode_walk 02379834 eases with cM_scos(mWalkTimer * it) */
    /* 0x74C */ be<s32> mWalkDir;  /* was m634: mode_wait 02379308: the direction from check_to_walk (index into M_dir_base, -1 none; mode_wait_init 02377B84 -1); mode_walk 02379834 / eff_set_slip_smoke_pos 02379010 use it */
    /* 0x750 */ be<u32> mPPLabel;
    /* 0x754 */ be<s16> mMomentCnt[4];
    /* 0x75C */ be<s16> mWalkTimer;  /* was m644: mode_wait 02379308: the push length in frames (attr m0A or m04); mode_walk 02379834 counts it down and ends the push at 0 */
    /* 0x75E */ be<s16> mAflCooldown;  /* was m646: Execute 02378638 sets 20 when an afloat box lands (mode_wait_init); mode_wait 02379308 counts it down; a waiting box goes afloat again only at 0 (tww Execute) */
    /* 0x760 */ be<s16> mLandFxDelay;  /* was m648: Create 02377C1C 0x14; Execute 02378638 counts it down and plays the landing sound / vibration / smoke only once it is 0 */
    /* 0x762 */ be<u8> mbPushPullHit;  /* was m64A: PPCallBack 023775DC sets it after counting mMomentCnt; check_to_walk 02378D18 starts a walk only when set (and on the ground), then clears it */
    /* 0x763 */ be<s8> mReverb;
    /* 0x764 */ be<u8> mbShouldAppear;
    /* 0x765 */ be<u8> mbPrmZInitialized;
    /* 0x766 */ be<u8> mbPrmXInitialized;
    /* 0x767 */ be<u8> mbPathSaved;  /* was m64F: Create 02377C1C true; mode_walk_init 023792E8 false; mode_wait 02379308 sets it on landing and calls path_save() */
    /* 0x768 */ EffSmokeCB mSmokeCbs[2];
    /* 0x7D0 */ be<u32> mChildPID;
    /* 0x7D4 */ be<s32> mbRollCrash;
    s32 prm(s32 width, s32 shift) const { return moveboxGetParameterBits(this, width, shift); }
    s32 prm_get_swSave1();
    u8 pathId() const { return mType == 11 ? 255 : (mPrmZ & 255); }
    u8 swSave2() const { return mType == 11 ? 255 : (mPrmZ >> 8); }
    const char* arc() const { return M_arcname[(s32)mType]; }
    const Attr_c* attr(u32 file, u32 msg = 0) const {
        if ((u32)mType >= 13)
            JUT_ASSERT_fail(STR(file), 0x5FF, STR(msg ? msg : file + 0x14));
        return &M_attr[(s32)mType];
    }
    void prmX_init();
    void prmZ_init();
    bool chk_appear();
    void path_init();
    void path_save();
    BOOL CreateHeap();
    void clr_moment_cnt();
    void set_mtx();
    void init_mtx();
    void mode_wait_init();
    BOOL Create();
    static void RideCallBack(dBgW*, fopAc_ac_c*, fopAc_ac_c*);
    static fopAc_ac_c* PPCallBack(fopAc_ac_c*, fopAc_ac_c*, s16, u32);
    void make_item();
    void eff_break();
    void sound_break();
    void mode_afl_init();
    void sound_land();
    void vib_land();
    void eff_land_smoke();
    BOOL Execute(gptr<Mtx34>*);
    BOOL Draw();
    void eff_smoke_slip_remove();
    BOOL Delete();
    s32 check_to_walk();
    void eff_set_slip_smoke_pos();
    void eff_smoke_slip_start();
    void mode_walk_init();
    void mode_wait();
    void sound_slip();
    void sound_limit();
    void eff_smoke_slip_end();
    void mode_walk();
    void afl_sway();
    void mode_afl();
};
WWHD_OFFSET(Act_c, mMtx, 0x3E8);
WWHD_OFFSET(Act_c, mBgc, 0x58C);
WWHD_OFFSET(Act_c, mType, 0x710);
WWHD_OFFSET(Act_c, mSmokeCbs, 0x768);
WWHD_SIZE(Act_c, 0x7D8);
} // namespace daObjMovebox

f32 moveboxSquareDistance(const cXyz*, const cXyz*);

void moveboxXrotS(Mtx34*, s16);
void moveboxUpdateModel(J3DModel*, u32);
enum { MOVEBOX_RIDE_CALLBACK = 0x02377308, MOVEBOX_PUSH_PULL_CALLBACK = 0x023775DC };

bool moveboxLineCrossBool(dBgS*, dBgS_LinChk*);

void moveboxMoveCollisionCenter(dCcD_Cyl*, cXyz*);

namespace daObjMovebox {
struct MoveboxChildHD {
    u8 mPrefix[0x314];
    cXyz mPosition;
    u8 mPaddingPosition[0x3FA - 0x320];
    be<u8> mSpringSetup;
    u8 mPaddingSpring[0xDD4 - 0x3FB];
    be<u8> mMirrorSetup;
    u8 mPaddingMirror[0x10D0 - 0xDD5];
    be<u8> mMkieSetup;
    u8 mPaddingMkie[0x3184 - 0x10D1];
    Mtx34 mBuoyMtx;
};
WWHD_OFFSET(MoveboxChildHD, mSpringSetup, 0x3FA);
WWHD_OFFSET(MoveboxChildHD, mMirrorSetup, 0xDD4);
WWHD_OFFSET(MoveboxChildHD, mMkieSetup, 0x10D0);
WWHD_OFFSET(MoveboxChildHD, mBuoyMtx, 0x3184);
WWHD_SIZE(MoveboxChildHD, 0x31B4);
struct MoveboxCylinderSourceHD {
    u8 mBytes[0x44];
};
WWHD_SIZE(MoveboxCylinderSourceHD,0x44);
struct MoveboxModeTableHD {
    be<u32> mWords[6];
};
WWHD_SIZE(MoveboxModeTableHD,0x18);
} // namespace daObjMovebox
daObjMovebox::MoveboxPathHD* moveboxGetRoomPath(s32, s32);
daObjMovebox::MoveboxPathPointHD* moveboxGetPathPoint(daObjMovebox::MoveboxPathHD*, s32);
fopAc_ac_c* moveboxGroundActor(dBgS*, u16);
void moveboxRideCallback(dBgS*, cBgS_PolyInfo*, daObjMovebox::Act_c*);
u32 moveboxPolyColor(dBgS*, cBgS_PolyInfo*);
BOOL moveboxWaterCheck(dBgS*, daObjMovebox::MoveboxWaterWorkHD*);
void moveboxQuaternion(Quaternion*, cXyz*);
BOOL moveboxFindActor(u32, be<u32>*);
fpc_ProcID moveboxCreateActor(s32, u32, cXyz*, s32, csXyz*, cXyz*, s32, u32);
fpc_ProcID moveboxCreateChild(s32, u32, u32, cXyz*, s32, csXyz*, cXyz*, s32, u32);
