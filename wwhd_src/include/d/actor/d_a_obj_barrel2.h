// Barrel and sea mine layout; GameCube field names with the HD actor allocation.
// HD: Attr_c remains 0x74; the actor fields shift by 0x11C after the 0x3AC base,
// growing Act_c from 0x4A8 to 0x5C4. Private prefixes describe only verified access.
#pragma once
#include "bindings.h"
#include "d/actor/d_a_obj_buoyflag.h"
#include "f_op/f_op_camera.h"
#include "gc_api/gc_api.h"
namespace daObjBarrel2 {
struct Attr_c {
    be<u16> mModelResIdx, mBrkResIdx;  /* was m00: create_heap 0231E670: dComIfG_getObjectRes("Ktaru_00", it) is the model data (tww the same); M_attr is read only, never written in WWHD or tww; was m02: create_heap 0231E670: dComIfG_getObjectRes("Ktaru_00", it) is the brk_data (tww the same) */
    be<u32> mHeapSize;  /* was m04: _create 0231F350: the size passed to fopAcM_entrySolidHeap (tww the same) */
    be<f32> mCylRadius;  /* was m08: _create 0231F350: mCyl's radius (tww mCyl.SetR) */
    be<f32> mCylHeight;  /* was m0C: _create 0231F350: mCyl's height (tww mCyl.SetH) */
    be<f32> mMoveCullRadius;  /* was m10: cull_set_move 0231FBA8: the cull sphere radius while moving (tww the same) */
    be<f32> mBarrelHeight;  /* was m14: set_pos_y 0231ED94 (float depth mFloatDepthRate * it), set_mtx 0231EEF4 / _create 0231F350 (the top, (it - 5) * scale), mode_afl 02321564 / mode_explode 02321D54 (mPosYOffset capped at 0.6 / 0.2 of it) (tww the same) */
    be<f32> mCenterHeight;  /* was m18: _create 0231F350: attention / eye position and cull sphere centre at it * scale above pos, the coming start depth -(it * scale) - 50; mode_exit_v 0232183C / mode_exit_h 023219D8 the exit depth from it (tww the same) */
    be<f32> m1C;
    be<f32> m20;
    be<f32> m24;
    be<f32> m28;
    be<f32> m2C;
    be<f32> m30;
    be<f32> mItemGiveDist;  /* was m34: execute_sub 02320AD4: item_give once mItemDistance < it squared (tww the same) */
    be<f32> mShipHitSpeed;  /* was m38: co_hitCB 0231E84C: a ship hit counts only above this speedF (tww the same) */
    be<f32> mShipHitDot;  /* was m3C: co_hitCB 0231E84C, mine_chk_range_damage 02320418: when dot(barrel - ship, ship speed) < it squared the ship gets flag 4 (tww the same) */
    be<f32> mFlashRange;  /* was m40: mine_chk_range_flash 023209E4: ship distance squared below it squared (tww the same) */
    be<f32> mExplodeRange;  /* was m44: mine_chk_range_explode 0231FD84: ship distance squared below it squared (tww the same) */
    be<f32> mDamageRange;  /* was m48: mine_chk_range_damage 02320418: ship distance squared below it squared (tww the same) */
    be<f32> mDamageHeight;  /* was m4C: mine_chk_range_damage 02320418: the ship counts only below pos.y + it (tww the same) */
    be<f32> mFloatDepthRate;  /* was m50: set_pos_y 0231ED94: the floating depth is it * mBarrelHeight; effect_position (in eff_break / eff_explode) puts effects at that depth (tww the same) */
    be<f32> mBobAmplitude;  /* was m54: set_pos_y 0231ED94: the bob it * sin(mBobAngle) (tww the same) */
    be<s16> mBobAngleStep;  /* was m58: mode_afl 02321564 / mode_explode 02321D54: mBobAngle += it * (cM_rnd() + 1) (tww the same) */
    u8 _5a[2];
    be<f32> mTiltTargetMax;  /* was m5C: afl_sway 0232115C: the water-normal tilt target is clamped to this length (tww the same) */
    be<f32> mTiltSpring;  /* was m60: afl_sway 0232115C: the tilt acceleration -(tilt - target) * it (tww the same) */
    be<f32> mTiltDamping;  /* was m64: afl_sway 0232115C: the tilt velocity damping -velocity * it (tww the same) */
    be<f32> mWaveTiltRate;  /* was m68: afl_sway 0232115C: the tilt target is mWaterNormal.x / .z * it (tww the same) */
    be<f32> mPillarScaleXZ;  /* was m6C: eff_explode 0231FE60: fopKyM_createWpillar's scaleXZ (tww the same) */
    be<f32> mPillarScaleY;  /* was m70: eff_explode 0231FE60: fopKyM_createWpillar's scaleY (tww the same) */
};
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_ITEM_SAVE_W = 7,
        PRM_ITEM_SAVE_S = 16,
        PRM_ITEM_NO_W = 6,
        PRM_ITEM_NO_S = 0,
        PRM_BUOY_W = 1,
        PRM_BUOY_S = 8,
        PRM_TYPE_W = 2,
        PRM_TYPE_S = 24,
        PRM_TEXTURE_W = 1,
        PRM_TEXTURE_S = 10,
        PRM_COMING_W = 1,
        PRM_COMING_S = 28
    };
    enum Mode_e {
        MODE_AFL = 0,
        MODE_EXIT_V = 1,
        MODE_EXIT_H = 2,
        MODE_EXIT_MINE = 3,
        MODE_EXPLODE = 4,
        MODE_DEMO_BREAK0 = 5,
        MODE_DEMO_BREAK1 = 6,
        MODE_DEMO_EXPLODE0 = 7,
        MODE_DEMO_EXPLODE1 = 8
    };
    bool demo_mode_chk();
    void mode_afl_init();
    void mode_exit_v_init();
    void mode_exit_h_init();
    void mode_exit_mine_init();
    void mode_demo_break1_init();
    void mode_demo_explode1_init();
    void mode_demo_explode0_init();
    void mode_demo_break0_init();
    void mode_explode_init();
    bool _delete();
    void item_give();
    u32 item_delete();
    void item_drop();
    void item_connect_check();
    void init_mtx();
    BOOL mode_proc_call();
    bool _draw();
    void cull_set_move();
    void cull_set_draw();
    void set_pos_y();
    bool mine_chk_range_explode();
    bool mine_chk_range_flash();
    bool mode_exit_v();
    bool mode_exit_h();
    bool mode_exit_mine();
    bool _execute();
    bool mode_demo_break1();
    bool mode_demo_explode1();
    bool mode_demo_break0();
    bool mode_demo_explode0();
    void item_drop_init(f32 speed);
    void set_item_position();
    void buoy_jump(f32 speed);
    bool mine_chk_range_damage();
    bool mode_explode();
    void afl_sway();
    bool create_heap();
    void set_water_pos();
    void eff_break();
    void eff_explode();
    void set_mtx();
    bool mode_afl();
    void execute_sub();
    s32 _create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;  /* was m298: create_heap 0231E670 creates the barrel's J3DModel; set_mtx 0231EEF4 / _draw 023210E0 use it */
    /* 0x3B8 */ gptr<mDoExt_brkAnm> mpBrkAnm;  /* was m29C: create_heap 0231E670 stores the brk animation; execute_sub 02320AD4 plays it in MODE_AFL; _draw 023210E0 enters it */
    /* 0x3BC */ dCcD_Stts mStts;
    /* 0x3F8 */ dCcD_Cyl mCyl;
    /* 0x528 */ be<s32> mMode;  /* was m40C: mode_afl_init 0231F344 sets MODE_AFL, mode_exit_v_init 02320918 ...; demo_mode_chk 0231FD50 / execute_sub 02320AD4 test it */
    /* 0x52C */ be<s32> mType;  /* was m410: the attribute index: attr() returns M_attr[it] (checked < 4); _create 0231F350 sets it; TWW attr() { return &M_attr[m410]; } */
    /* 0x530 */ be<f32> mDriftVelX;  /* was m414: mode_afl 02321564: X speed = (it + (x - home.x) * -0.002) * 0.9, added to current.pos.x with the collision push (the pull back home) (tww the same) */
    /* 0x534 */ be<f32> mDriftVelZ;  /* was m418: mode_afl 02321564: Z speed = (it + (z - home.z) * -0.002) * 0.9, added to current.pos.z with the collision push (see mDriftVelX) (tww the same) */
    /* 0x538 */ be<f32> mWaterY;  /* was m41C: set_water_pos 0231EA58: the mean of three water heights; set_pos_y 0231ED94 sets y = it + mPosYOffset */
    /* 0x53C */ cXyz mWaterNormal;  /* was m420: set_water_pos 0231EA58: the normalized cross product of the water surface; afl_sway 0232115C tilts the barrel by it */
    /* 0x548 */ be<f32> mWobbleX;  /* was m42C: set_water_pos 0231EA58: off the sea, a random spring value (velocity mWobbleVelX) used as mWaterNormal.x before normalizing; _create 0231F350 clears (tww the same) */
    /* 0x54C */ be<f32> mWobbleZ;  /* was m430: set_water_pos 0231EA58: off the sea, a random spring value (velocity mWobbleVelZ) used as mWaterNormal.z before normalizing; _create 0231F350 clears (tww the same) */
    /* 0x550 */ be<f32> mWobbleVelX;  /* was m434: set_water_pos 0231EA58: (it + mWobbleX * -0.2 + rnd) * 0.99, added to mWobbleX; _create 0231F350 clears (tww the same) */
    /* 0x554 */ be<f32> mWobbleVelZ;  /* was m438: set_water_pos 0231EA58: (it + mWobbleZ * -0.2 + rnd) * 0.99, added to mWobbleZ; _create 0231F350 clears (tww the same) */
    /* 0x558 */ be<s16> mBobAngle;  /* was m43C: mode_afl 02321564 / mode_explode 02321D54 advance it by attr m58 * (rnd + 1); set_pos_y 0231ED94 adds sin(it) times the sway height to the water y (tww the same) */
    u8 _55a[2];
    /* 0x55C */ be<f32> mTiltX;  /* was m440: set_mtx 0231EEF4: the X of the up axis (it, 1, mTiltZ) the model is tilted to; afl_sway 0232115C springs it toward the water normal; _create 0231F350 random for a coming mine, else 0 (tww the same) */
    /* 0x560 */ be<f32> mTiltZ;  /* was m444: set_mtx 0231EEF4: the Z of the up axis (mTiltX, 1, it); afl_sway 0232115C springs it toward the water normal; _create 0231F350 random for a coming mine, else 0 (tww the same) */
    /* 0x564 */ be<f32> mTiltVelX;  /* was m448: afl_sway 0232115C: the spring speed of mTiltX (damped by attr m64), added to it each frame; _create 0231F350 clears (tww the same) */
    /* 0x568 */ be<f32> mTiltVelZ;  /* was m44C: afl_sway 0232115C: the spring speed of mTiltZ (damped by attr m64), added to it each frame; _create 0231F350 clears (tww the same) */
    /* 0x56C */ be<f32> mPosYOffset;  /* was m450: set_pos_y 0231ED94 adds it to mWaterY; mode_afl 02321564 moves it with mPosYOffsetSpeed (bobbing); _create 0231F350 sets the sunk start */
    /* 0x570 */ be<f32> mPosYOffsetSpeed;  /* was m454: mode_afl 02321564 integrates mPosYOffset with it; co_hitCB 0231E84C sets -60 (pushed under); the exit modes set it */
    /* 0x574 */ be<u32> mItemId;
    /* 0x578 */ be<f32> mItemDistance;  /* was m45C: _create 0231F350 / set_item_position 023213F8 set barrelItemDistanceMax; item_drop_init 02320294 / item_drop 02321CEC use it */
    /* 0x57C */ be<u32> mChildPid;  /* was m460: _create 0231F350: process ID of the child actor 0x1D1 it creates, else fpcM_ERROR_PROCESS_ID_e; set_mtx 0231EEF4 / buoy_jump 023201B0 use it */
    /* 0x580 */ be<s32> mModeTimer;  /* was m464: mode_exit_v_init 02320918 sets 4, mode_explode_init 023203B0 30; the modes count it down */
    /* 0x584 */ be<u8> mMineFlash;  /* was m468: execute_sub 02320AD4: a mine (mType 2) sets 1 once mine_chk_range_flash finds the ship; in MODE_AFL it then plays mpBrkAnm and explodes when it ends, and blocks the mine exit; _create 0231F350 clears (tww the same) */
    u8 _585;
    /* 0x586 */ be<s16> mStagePoint;  /* was m46A: _create 0231F350: the current stage's start point (mCurStage.mPoint); _execute 0232101C tests != 1 */
    /* 0x588 */ be<u8> mEventRun;  /* was m46C: _execute 0232101C: 1 while an event runs (unless mStagePoint is 1), and then runs execute_sub off screen too; set_item_position 023213F8 sets the item by matrix while set (tww the same) */
    /* 0x589 */ be<u8> mCoHitTimer;  /* was m46D: co_hitCB 0231E84C sets 20 when it is 0; execute_sub 02320AD4 counts it down */
    u8 _58e[2];
    /* 0x58C */ be<s32> mHitType;  /* was m470: tg_hitCB 0231E800 3 (attack type 0x20), co_hitCB 0231E84C 1 (ship rams it) / 2 (ship touches); execute_sub 02320AD4 takes and clears it: 1 break demo, 2 push under, 3 break and drop (tww the same) */
    /* 0x590 */ be<u8> mExitReq;  /* was m474: execute_sub 02320AD4 starts the exit in MODE_AFL when set; TWW exit_req() { m474 = 1; } */
    /* 0x591 */ be<u8> mDeleteReq;  /* was m475: execute_sub 02320AD4 deletes the item and the barrel while set; _execute 0232101C runs execute_sub off screen while set (tww the same); never set in WWHD or tww */
    /* 0x592 */ be<u8> mItemGive;  /* was m476: item_give 023208BC sets 1; TWW chk_item_give() { return m476; } */
    u8 _593;
    /* 0x594 */ Mtx34 mBaseMtx;  /* was m478: set_mtx 0231EEF4 copies the model's base TR matrix (before the buoy flag offset) into it (tww cMtx_copy(mDoMtx_stack_c::get(), it)); never read in WWHD or tww */
};
} // namespace daObjBarrel2
WWHD_SIZE(daObjBarrel2::Attr_c, 0x74);
WWHD_OFFSET(daObjBarrel2::Act_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjBarrel2::Act_c, mStts, 0x3BC);
WWHD_OFFSET(daObjBarrel2::Act_c, mCyl, 0x3F8);
WWHD_OFFSET(daObjBarrel2::Act_c, mMode, 0x528);
WWHD_OFFSET(daObjBarrel2::Act_c, mExitReq, 0x590);
WWHD_OFFSET(daObjBarrel2::Act_c, mBaseMtx, 0x594);
WWHD_SIZE(daObjBarrel2::Act_c, 0x5C4);

void Barrel_assertion(const char *, s32, const char *);
void Barrel_raceItemForceGet(fopAc_ac_c *);
u32 Barrel_deleteActor(fopAc_ac_c *);
f32 Barrel_playerDistance2(fopAc_ac_c *, void *);
BOOL Barrel_searchById(u32, gptr<fopAc_ac_c> *);
BOOL Barrel_checkOffsetPos(fopAc_ac_c *);
void Barrel_polyInfoDestroy(void *, s32);
void Barrel_cylinderDestroy(dCcD_Cyl *, s32);
void Barrel_statusDestroy(dCcD_Stts *, s32);
void Barrel_actorDestroy(fopAc_ac_c *, s32);
void Barrel_destructorChain(void *);
void Barrel_byteConstructor(void *);
void Barrel_otherByteConstructor(void *);
void Barrel_setCullSphere(fopAc_ac_c *, f32, f32, f32, f32);
f32 Barrel_actorDistanceXZ2(fopAc_ac_c *, void *);
BOOL Barrel_cullingCheck(fopAc_ac_c *);
BOOL Barrel_eventEndCheck(void *, const char *);
s32 Barrel_eventIndex(void *, const char *, u8);
void *Barrel_eventData(void *, s32);
void Barrel_orderOtherEvent(fopAc_ac_c *, const char *, u16, u16);
void Barrel_endOffsetPos(fopAc_ac_c *, f32, cXyz *, f32, f32, csXyz *);
void Barrel_setItemMatrix(fopAc_ac_c *, cXyz *);
void Barrel_startOffsetPos(fopAc_ac_c *);
void *Barrel_searchActor(void *, be<u32> *);
void Barrel_copyMatrix(void *, void *);
f32 Barrel_innerProduct(cXyz *, cXyz *);
s32 Barrel_reverb(s8);
void Barrel_sound(u32, void *, s32, s32);
f32 Barrel_sqrt(f32);
void *Barrel_allocate(u32);
void *Barrel_brkConstruct(void *);
s32 Barrel_brkInit(void *, J3DModelData *, void *, s32, s32, f32, s32, s32, s32, s32);
BOOL Barrel_seaCheckArea(f32, f32);
f32 Barrel_seaCalcWave(f32, f32);
void Barrel_outProduct(cXyz *, cXyz *, cXyz *);
void Barrel_normalizeZP(cXyz *, cXyz *);
void Barrel_waterCheckConstruct(void *);
BOOL Barrel_specialGroupCheck(void *, void *);
void Barrel_normalize(cXyz *, cXyz *);
void *Barrel_particleSet(void *, s32, u32, cXyz *, csXyz *, cXyz *, u8, void *, s32, void *, void *,
                         void *);
void Barrel_particleBombSmoke(void *, u32, cXyz *, csXyz *, cXyz *, u8);
void Barrel_createWaterPillar(cXyz *, f32, f32, s32);
void Barrel_scaleVector(cXyz *, cXyz *, f32);
void Barrel_quaternionRotBaseY(void *, cXyz *);
void Barrel_quaternionMatrix(void *);
void Barrel_zxyRotation(Mtx34 *, s16, s16, s16);
void Barrel_zRotation(Mtx34 *, s16);
void Barrel_matrixVectorSR(Mtx34 *, cXyz *, cXyz *);
void Barrel_setCylinderCenter(void *, cXyz *);
void Barrel_registerCollision(void *, dCcD_Cyl *);
BOOL Barrel_brkPlay(mDoExt_brkAnm *);
void Barrel_statusConstruct(void *);
void Barrel_gameStatusConstruct(void *);
void Barrel_cylinderConstruct(dCcD_Cyl *);
void Barrel_cylinderShapeConstruct(void *);
BOOL Barrel_entrySolidHeap(fopAc_ac_c *, void *, u32);
void Barrel_setCylinder(dCcD_Cyl *, void *);
void Barrel_setCylinderRadius(void *, f32);
void Barrel_setCylinderHeight(void *, f32);
void Barrel_angleConstruct(csXyz *, s16, s16, s16);
u32 Barrel_createRaceItem(cXyz *, s32, s32, s8, csXyz *, cXyz *, BOOL);
u32 Barrel_createChild(s32, u32, u32, cXyz *, s8, csXyz *, cXyz *, s32, void *);
u32 PrmAbstract(fopAc_ac_c *, u32, u32);

namespace barrel2_hd {
struct Attributes {
    daObjBarrel2::Attr_c mData[4];
};
struct ItemPointer {
    gptr<fopAc_ac_c> mpActor;
};
using ModeFunction = BOOL (daObjBarrel2::Act_c::*)();
using ModePtmf = gabi::ptmf<ModeFunction>;
struct ModeTable {
    ModePtmf mData[9];
};
struct Ship : fopAc_ac_c {
    u8 mPrefix[0x644 - 0x3AC];
    be<u32> mFlags;
};
struct WaterCheck {
    u8 mPrefix[0x10];
    cBgS_PolyInfo mPolyInfo;
    be<u32> mVtable;
    be<u32> mPolyVtable;
    u8 mPadding[8];
    be<u32> mGroupVtable;
    u8 mPadding2[4];
    cXyz mPos;
    be<f32> mUpperY;
    be<f32> mHeight;
    u8 mTail[4];
};
struct DestructorChain {
    be<u32> mNext, mObject, mDestructor;
};
struct Angles {
    csXyz mZero;
};
struct SinCos {
    be<f32> mSin, mCos;
};
struct SinTable {
    SinCos mData[8192];
};
struct Emitter {
    u8 mPrefix[0x58];
    be<f32> mSpread;
    u8 mPadding[4];
    be<u16> mLifeTime;
    u8 mPadding2[10];
    be<f32> mAwayFromAxisSpeed;
    be<f32> mDirectionalSpeed;
};
struct ShapeVtables {
    u8 mPrefix[0x110];
    be<u32> mShapeVtable;
    be<u32> mAabVtable;
};
WWHD_SIZE(Attributes, 0x1D0);
WWHD_SIZE(ItemPointer, 4);
WWHD_SIZE(ModeTable, 72);
WWHD_SIZE(Ship, 0x648);
WWHD_OFFSET(Ship, mFlags, 0x644);
WWHD_OFFSET(WaterCheck, mPos, 0x38);
WWHD_OFFSET(WaterCheck, mHeight, 0x48);
WWHD_SIZE(WaterCheck, 0x50);
WWHD_SIZE(DestructorChain, 12);
WWHD_SIZE(Angles, 6);
WWHD_SIZE(SinCos, 8);
WWHD_SIZE(SinTable, 0x10000);
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mAwayFromAxisSpeed, 0x6C);
WWHD_SIZE(Emitter, 0x74);
WWHD_SIZE(ShapeVtables, 0x118);
} // namespace barrel2_hd

namespace barrel2_hd {
struct InitializerData {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mByte, mOtherByte;
    u8 mPadding[2];
    be<u32> mWords[4];
};
struct DestructorChains {
    DestructorChain mData[4];
};
WWHD_SIZE(InitializerData, 0x1C);
WWHD_SIZE(DestructorChains, 48);
enum : u32 {
    WaterVtable = 0x10025778,
    WaterPolyVtable = 0x10025798,
    WaterGroupVtable = 0x10025768,
    ActorVtable = 0x100257D8,
    StatusVtable = 0x1004AE88,
    GameStatusVtable = 0x1004AEC0,
    AabBaseVtable = 0x100015A8,
    ShapeVtable = 0x10025758,
    AabVtable = 0x1004B160,
    HitInfoVtable = 0x1004B108,
    CylinderVtable = 0x1004B150,
    ResourceStringVtable = 0x10025740
};
} // namespace barrel2_hd

namespace barrel2_hd {
enum : u32 { SearchByIdJudge = 0x025E1234 };
struct CylinderSource {
    dCcD_SrcCyl mData;
};
} // namespace barrel2_hd

WWHD_OFFSET(daObjBarrel2::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjBarrel2::Act_c, mpBrkAnm, 0x3B8);
WWHD_OFFSET(daObjBarrel2::Act_c, mType, 0x52C);
WWHD_OFFSET(daObjBarrel2::Act_c, mDriftVelX, 0x530);
WWHD_OFFSET(daObjBarrel2::Act_c, mWaterY, 0x538);
WWHD_OFFSET(daObjBarrel2::Act_c, mWaterNormal, 0x53C);
WWHD_OFFSET(daObjBarrel2::Act_c, mBobAngle, 0x558);
WWHD_OFFSET(daObjBarrel2::Act_c, mTiltX, 0x55C);
WWHD_OFFSET(daObjBarrel2::Act_c, mPosYOffset, 0x56C);
WWHD_OFFSET(daObjBarrel2::Act_c, mPosYOffsetSpeed, 0x570);
WWHD_OFFSET(daObjBarrel2::Act_c, mItemId, 0x574);
WWHD_OFFSET(daObjBarrel2::Act_c, mItemDistance, 0x578);
WWHD_OFFSET(daObjBarrel2::Act_c, mChildPid, 0x57C);
WWHD_OFFSET(daObjBarrel2::Act_c, mModeTimer, 0x580);
WWHD_OFFSET(daObjBarrel2::Act_c, mStagePoint, 0x586);
WWHD_OFFSET(daObjBarrel2::Act_c, mEventRun, 0x588);
WWHD_OFFSET(daObjBarrel2::Act_c, mHitType, 0x58C);
