/* Great Sea enemy cannon.
 * HD: actor fields move by 0x11C; the collision sphere/status sizes become 0x12C/0x3C.
 * Members still named field_0xNNN follow zeldaret/tww; the `was field_0xNNN` comments keep tww's names of the renamed ones. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"

struct daObj_Canon_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e { MODE_WAIT, MODE_ATTACK, MODE_DELETE, MODE_SW_WAIT, MODE_APPEAR, MODE_COUNT };
    void _nodeControl(J3DNode* node, J3DModel* model);
    s32 _createHeap();
    void getArg();
    void modeProc(s32 proc, s32 mode);
    void setMtx();
    void createInit();
    s32 _create();
    s32 _delete();
    void setAttention();
    void setCollision();
    s32 _execute();
    void debugDraw();
    s32 _draw();
    void modeWaitInit();
    s32 checkTgHit();
    void modeWait();
    void modeAttackInit();
    void lockonCannon();
    void attackCannon();
    void modeAttack();
    void setEffect(u16 id);
    void modeDeleteInit();
    void modeSwWaitInit();
    void modeSwWait();
    void modeAppearInit();
    void modeAppear();
    void modeDelete();
    /* 0x3AC */ be<s32> mCurMode;
    /* 0x3B0 */ be<u8> mPathNo;  /* was field_0x294: getArg 02330100: parameter bits 16-23 (tww fopAcM_GetParamBit(param, 16, 0xFF)); createInit 02330304 gets the room path with it (tww dPath_GetRoomPath(it, current.roomNo)); _create 02330524 fails when it is 0xFF */
    /* 0x3B1 */ be<u8> mDeleteSwNo;  /* was field_0x295: getArg 02330100: parameter bits 24-31; modeDeleteInit 02331558 turns the switch on when it is not 0xFF; createInit 02330304 starts in MODE_DELETE when that switch is already on (tww dComIfGs_onSwitch / isSwitch(it, ..)) */
    /* 0x3B2 */ be<u8> mAppearSwNo;  /* was field_0x296: getArg 02330100: parameter bits 8-15; createInit 02330304 starts in MODE_SW_WAIT while that switch is off; modeSwWait 02331614 goes to MODE_APPEAR once it is on (tww dComIfGs_isSwitch(it, ..)) */
    /* 0x3B3 */ be<u8> mScaleArg;  /* was field_0x297: getArg 02330100: parameter bits 0-7; when not 0xFF the actor scale is 1 + it * 0.1 (tww scale.set(1.0 + it * 0.1, ..)) */
    /* 0x3B4 */ dNpc_PathRun_c mPathRun;  /* was field_0x298: createInit 02330304 gives it the room path (tww setInfDrct(mpPath)); modeWait 02330D3C only aims while the player and the target point are inside it (tww chkInside) */
    /* 0x3BC */ u8 field_0x2A0[0x10];
    /* 0x3CC */ gptr<dPath> mpPath;  /* was field_0x2B0: createInit 02330304: the room path from mPathNo (tww it = dPath_GetRoomPath(..)), handed to mPathRun */
    /* 0x3D0 */ u8 field_0x2B4[4];
    /* 0x3D4 */ request_of_phase_process_class mPhs;
    /* 0x3DC */ gptr<J3DModel> mpModel;
    /* 0x3E0 */ be<s16> mTargetYaw;  /* was field_0x2C4: lockonCannon 0233113C: target Y angle to mTargetPos minus shape_angle.y, limited to l_HIO.mYawMin..mYawMax (tww); modeWait 02330D3C 0 outside the path; _execute 02330760 eases mYaw to it */
    /* 0x3E2 */ be<s16> mYaw;  /* was field_0x2C6: _execute 02330760 eases it to mTargetYaw (tww cLib_addCalcAngleS2(&it, mTargetYaw, 6, 0x300)); _nodeControl 0232FDF0 turns the barrel by -it (Z); attackCannon 02331298 adds it to the shot's Y angle */
    /* 0x3E4 */ be<s16> mTargetPitch;  /* was field_0x2C8: lockonCannon 0233113C: target X angle to mTargetPos plus a distance lift, limited to l_HIO.mPitchMin..mPitchMax (tww); modeWait 02330D3C 0 outside the path; _execute 02330760 eases mPitch to it */
    /* 0x3E6 */ be<s16> mPitch;  /* was field_0x2CA: _execute 02330760 eases it to mTargetPitch (tww cLib_addCalcAngleS2(&it, mTargetPitch, 6, 0x300)); _nodeControl 0232FDF0 turns the barrel by -it (X); attackCannon 02331298 subtracts it from the shot's X angle */
    /* 0x3E8 */ cXyz mTargetPos;  /* was field_0x2CC: modeWait 02330D3C: the player position moved back along the player angle by the lead distance (tww it = player pos; it.x -= temp * sin ..); lockonCannon 0233113C aims at it */
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ dCcD_Stts mStts;
    /* 0x55C */ be<s32> mHitTimer;  /* was field_0x440: checkTgHit 02330A98: hits count only while cLib_calcTimer(&it) is 0; 5 after each hit (tww) */
    /* 0x560 */ be<s32> mModeTimer;  /* was field_0x444: modeWaitInit 02330A88 l_HIO.mWaitTime, modeAttackInit 0233112C l_HIO.mAttackTime; modeWait 02330D3C / modeAttack 023313B4 act when cLib_calcTimer(&it) reaches 0 (tww) */
    /* 0x564 */ u8 field_0x448[8];
    /* 0x56C */ cXyz mMuzzlePos;  /* was field_0x450: _execute 02330760: the shot joint matrix times the l_HIO muzzle offset (tww cMtx_multVec(.., &it)); attackCannon 02331298 creates the bomb there and starts the fire sound on it; _delete 02330640 deletes that sound */
    /* 0x578 */ cXyz mAttentionPos;  /* was field_0x45C: _execute 02330760: the shot joint matrix times (0, 60 * scale.x, 0) (our local attention); setAttention 0233069C puts attention_info.position and eyePos there (tww); setCollision 023306E4 sphere centre */
    /* 0x584 */ be<s16> mRecoilPhase;  /* was field_0x468: _nodeControl 0232FDF0: advanced by 0x1830 each call; the barrel X shake is mRecoil * 5 * sin(it) (tww; our locals phase / recoil) */
    /* 0x586 */ be<s16> mRecoil;  /* was field_0x46A: _nodeControl 0232FDF0: the shake amplitude (our local recoil); attackCannon 02331298 0xC8, checkTgHit 02330A98 0x190 on a boomerang hit, modeAppearInit 02331678 0x258, modeDeleteInit 02331558 0; _execute 02330760 eases it to 0 (tww) */
    /* 0x588 */ be<f32> mSinkY;  /* was field_0x46C: _nodeControl 0232FDF0 translates the barrel by -it (our local rise; tww temp2.y = -it); modeSwWaitInit 023315F8 / modeAppearInit 02331678 120; modeAppear 023316A0 eases it to 0 and ends the appearance below 2 */
    /* 0x58C */ dPa_followEcallBack mSmokeCallBack;  /* was field_0x470: setEffect 02331418: the follow callback of the ID_IT_JN_MJTAIHOU_SMOKE01 particle (tww dComIfGp_particle_set(.., &it, ..)); _execute 02330760 removes it when mSmokeTimer runs out; _delete 02330640 removes it */
    /* 0x5A0 */ be<s32> mSmokeTimer;  /* was field_0x484: setEffect 02331418 0xE6 with the smoke particle; _execute 02330760 removes mSmokeCallBack when cLib_calcTimer(&it) reaches 0 (tww) */
    /* 0x5A4 */ be<s32> mShipShotCount;  /* was field_0x488: modeWait 02330D3C: +1 each frame the ship's cannon shoots (tww ship->checkShootCannon()), 0 when the player status says swimming / UNK1000000; below 6 it adds 200 * (6 - it) to the lead distance (tww) */
};
WWHD_OFFSET(daObj_Canon_c, mCurMode, 0x3AC);
WWHD_OFFSET(daObj_Canon_c, mPathNo, 0x3B0);
WWHD_OFFSET(daObj_Canon_c, mPathRun, 0x3B4);
WWHD_OFFSET(daObj_Canon_c, mpPath, 0x3CC);
WWHD_OFFSET(daObj_Canon_c, mPhs, 0x3D4);
WWHD_OFFSET(daObj_Canon_c, mpModel, 0x3DC);
WWHD_OFFSET(daObj_Canon_c, mTargetYaw, 0x3E0);
WWHD_OFFSET(daObj_Canon_c, mTargetPos, 0x3E8);
WWHD_OFFSET(daObj_Canon_c, mSph, 0x3F4);
WWHD_OFFSET(daObj_Canon_c, mStts, 0x520);
WWHD_OFFSET(daObj_Canon_c, mHitTimer, 0x55C);
WWHD_OFFSET(daObj_Canon_c, mMuzzlePos, 0x56C);
WWHD_OFFSET(daObj_Canon_c, mAttentionPos, 0x578);
WWHD_OFFSET(daObj_Canon_c, mRecoil, 0x586);
WWHD_OFFSET(daObj_Canon_c, mSinkY, 0x588);
WWHD_OFFSET(daObj_Canon_c, mSmokeCallBack, 0x58C);
WWHD_OFFSET(daObj_Canon_c, mShipShotCount, 0x5A4);
WWHD_SIZE(daObj_Canon_c, 0x5A8);

struct daObj_Canon_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> debug_draw;
    /* 0x05 */ be<u8> mForceBreak;  /* was field_0x05: checkTgHit 02330A98: while set, health = 0 and MODE_DELETE (tww if (l_HIO.it) { health = 0; modeProcInit(2); }); daObj_Canon_HIO_c_ct 0233172C 0 */
    /* 0x06 */ u8 padding06[2];
    /* 0x08 */ be<f32> mLeadDist;  /* was field_0x08: modeWait 02330D3C: the base distance the aim point mTargetPos is put behind the player (our local prediction; tww s16 temp = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 150 */
    /* 0x0C */ be<f32> mAttentionY;  /* was field_0x0C: setAttention 0233069C: attention_info.position.y = mAttentionPos.y + it (tww); daObj_Canon_HIO_c_ct 0233172C sets nothing (0 from the zeroed static) */
    /* 0x10 */ be<f32> mEyeY;  /* was field_0x10: setAttention 0233069C: eyePos.y = mAttentionPos.y + it (tww); daObj_Canon_HIO_c_ct 0233172C sets nothing */
    /* 0x14 */ be<f32> mMuzzleX;  /* was field_0x14: _execute 02330760: x of the muzzle offset on the shot joint (our local muzzle; tww temp.x = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0 */
    /* 0x18 */ be<f32> mMuzzleY;  /* was field_0x18: _execute 02330760: y of the muzzle offset, times scale.x (tww temp.y = l_HIO.it * scale.x); daObj_Canon_HIO_c_ct 0233172C 180 */
    /* 0x1C */ be<f32> mMuzzleZ;  /* was field_0x1C: _execute 02330760: z of the muzzle offset (tww temp.z = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0 */
    /* 0x20 */ be<f32> mShotSpeed;  /* was field_0x20: attackCannon 02331298: the bomb's speedF = it * cos(pitch) and speed.y = -(it * sin(pitch)) (tww); daObj_Canon_HIO_c_ct 0233172C 90 */
    /* 0x24 */ be<f32> mShotGravity;  /* was field_0x24: attackCannon 02331298: the bomb's gravity (tww bomb->gravity = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C -1.1 */
    /* 0x28 */ be<s16> mShotNoGravityTime;  /* was field_0x28: attackCannon 02331298: bomb->setNoGravityTime(it) (tww); daObj_Canon_HIO_c_ct 0233172C 0x16 */
    /* 0x2A */ u8 padding2A[2];
    /* 0x2C */ be<f32> mPitchLiftDist;  /* was field_0x2C: lockonCannon 0233113C: beyond this XZ distance to mTargetPos the excess is added to the pitch, up to mPitchLiftMax (tww if (temp2 > l_HIO.it) temp2 -= l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 5000 */
    /* 0x30 */ be<s16> mPitchLiftMax;  /* was field_0x30: lockonCannon 0233113C: the cap of that distance lift (tww if (temp3 > l_HIO.it) temp3 = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0x3000 */
    /* 0x32 */ be<s16> mPitchMax;  /* was field_0x32: lockonCannon 0233113C: upper limit of mTargetPitch (tww cLib_minMaxLimit(.., l_HIO.field_0x34, l_HIO.it)); daObj_Canon_HIO_c_ct 0233172C 0x2328 */
    /* 0x34 */ be<s16> mPitchMin;  /* was field_0x34: lockonCannon 0233113C: lower limit of mTargetPitch (tww cLib_minMaxLimit(.., l_HIO.it, ..)); daObj_Canon_HIO_c_ct 0233172C -0x2328 */
    /* 0x36 */ be<s16> mYawMax;  /* was field_0x36: lockonCannon 0233113C: upper limit of mTargetYaw (tww cLib_minMaxLimit(.., l_HIO.field_0x38, l_HIO.it)); daObj_Canon_HIO_c_ct 0233172C 0x2328 */
    /* 0x38 */ be<s16> mYawMin;  /* was field_0x38: lockonCannon 0233113C: lower limit of mTargetYaw (tww cLib_minMaxLimit(.., l_HIO.it, ..)); daObj_Canon_HIO_c_ct 0233172C -0x2328 */
    /* 0x3A */ be<s16> field_0x3A;
    /* 0x3C */ be<s16> mAttackTime;  /* was field_0x3C: modeAttackInit 0233112C: mModeTimer = it; modeAttack 023313B4 fires when it runs out (tww); daObj_Canon_HIO_c_ct 0233172C 30 */
    /* 0x3E */ be<s16> mWaitTime;  /* was field_0x3E: modeWaitInit 02330A88 / modeWait 02330D3C: mModeTimer = it (ten times it while the player status has UNK1000000) (tww); daObj_Canon_HIO_c_ct 0233172C 30 */
    /* 0x40 */ be<f32> mLeadFarDist;  /* was field_0x40: modeWait 02330D3C: beyond this distance to mTargetPos the lead distance grows by (distance - it) * mLeadFarRate, and the ship-shot bonus applies (tww abs > l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 2000 */
    /* 0x44 */ be<f32> mLeadFarRate;  /* was field_0x44: modeWait 02330D3C: the factor of that growth (tww (abs - l_HIO.field_0x40) * l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0.2 */
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ u8 padding4A[2];
};
WWHD_SIZE(daObj_Canon_HIO_c, 0x4C);
WWHD_OFFSET(daObj_Canon_HIO_c, mShotNoGravityTime, 0x28);
WWHD_OFFSET(daObj_Canon_HIO_c, mWaitTime, 0x3E);

namespace canonHD {
enum {
    dRes_INDEX_WALLBOM_BDL_WALLBOM_e = 3,
    WALLBOM_JNT_SHOT_e = 3,
    AT_TYPE_BOMB = 0x20,
    AT_TYPE_BOOMERANG = 0x40,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_LK_SHIP_CANNON_FIRE = 0x2852,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    ID_IT_SN_MJTAIHOU_HAHEN00 = 0x82E4,
    ID_IT_SN_MJTAIHOU_SMOKE00 = 0x82E5,
    ID_IT_SN_MJTAIHOU_SENKO00 = 0x82E6,
    ID_IT_JN_MJTAIHOU_SMOKE01 = 0x3E1,
    daPyStts0_UNK1000000_e = 0x1000000,
    daPyStts0_SWIM_e = 0x100000,
    SHIP_SHOOT_CANNON = 0x20000,
    MODEL_ANM_MTX_DIRTY = 0x10,
    MAP_ICON_MASK = 0x1F,
    MAP_ICON_CANNON = 0x15,
};

struct JointData {
    /* 0x00 */ u8 padding[4];
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_OFFSET(JointData, mJntNo, 4);
struct Joint {
    /* 0x00 */ u8 padding[8];
    /* 0x08 */ be<u32> mCallBack;
    /* 0x0C */ u8 padding2[4];
    /* 0x10 */ gptr<J3DMaterial> mMesh;
    /* 0x14 */ u8 padding3[8];
};
WWHD_OFFSET(Joint, mCallBack, 8);
WWHD_OFFSET(Joint, mMesh, 0x10);
WWHD_SIZE(Joint, 0x1C);
struct ModelData {
    /* 0x00 */ be<u32> mpResHeader;
    /* 0x04 */ be<u32> mJointNum;
    /* 0x08 */ gptr<Joint> mJointNodes;
};
WWHD_SIZE(ModelData, 0xC);
struct MatrixBuffer {
    /* 0x00 */ u8 padding[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 padding2[0xA];
    /* 0x10 */ gptr<Mtx34> mpAnmMtx;
};
WWHD_OFFSET(MatrixBuffer, mpAnmMtx, 0x10);
WWHD_SIZE(MatrixBuffer, 0x14);
struct Ship {
    /* 0x000 */ u8 padding[0x644];
    /* 0x644 */ be<u32> mState;
};
WWHD_OFFSET(Ship, mState, 0x644);
struct SaveBlock {
    /* 0x00 */ u8 padding[0x20];
    /* 0x20 */ dSv_info_c mSaveInfo;
};
WWHD_OFFSET(SaveBlock, mSaveInfo, 0x20);
struct HeaderZeros {
    be<u32> words[4];
};
struct DestructorRecord {
    u8 data[0xC];
};
struct DebugObject {
    u8 data[4];
};
} // namespace canonHD

using CanonModeProc = void (daObj_Canon_c::*)();
struct CanonModeEntry {
    /* 0x00 */ gabi::ptmf<CanonModeProc> init;
    /* 0x08 */ gabi::ptmf<CanonModeProc> run;
    /* 0x10 */ gptr<const char> name;
};
WWHD_OFFSET(CanonModeEntry, run, 8);
WWHD_OFFSET(CanonModeEntry, name, 0x10);
WWHD_SIZE(CanonModeEntry, 0x14);

WWHD_SIZE(canonHD::JointData, 6);
WWHD_SIZE(canonHD::Ship, 0x648);
WWHD_SIZE(canonHD::SaveBlock, sizeof(dSv_info_c) + 0x20);
WWHD_SIZE(canonHD::HeaderZeros, 0x10);
WWHD_SIZE(canonHD::DestructorRecord, 0xC);
WWHD_SIZE(canonHD::DebugObject, 4);
