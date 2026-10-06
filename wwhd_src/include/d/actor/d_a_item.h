/* daItem_c (field item: rupees, hearts, ...), WWHD layout.
 *
 * GameCube -> WWHD (measured from the verified functions and the constructor in _daItem_create):
 *  - daItemBase_c is 0x750 (GameCube 0x63C), so +0x114 up to mTargetAngleX... with HD fields:
 *  - 0x768: an HD float after field_0x650;
 *  - 0x76C: an HD s16 added to the model's Y rotation (set_mtx_base); mRotateSpeed and the rest
 *    of the s16 block follow (+0x11A);
 *  - 0x787/0x788 HD bytes after mOnGroundTimer; 0x78C cXyz/0x798 f32 (HD action 0xD: thrown to
 *    the player) and 0x79C..0x79E HD bytes (0x79E: HD action 0xF);
 *  - the particle callbacks follow (+0x130); size 0x7F0 (GameCube 0x6C0). */
#pragma once
#include "d/actor/d_a_itembase.h"

struct daItem_c : daItemBase_c {
    enum Status {
        STATUS_UNK0 = 0x0,
        STATUS_UNK1 = 0x1,
        STATUS_WAIT_MAIN = 0x2,
        STATUS_BRING_NEZUMI = 0x3,
        STATUS_UNK4 = 0x4,
        STATUS_INIT_NORMAL = 0x5,
        STATUS_MAIN_NORMAL = 0x6,
        STATUS_INIT_GET_DEMO = 0x7,
        STATUS_WAIT_GET_DEMO = 0x8,
        STATUS_MAIN_GET_DEMO = 0x9,
        STATUS_WAIT_BOSS1 = 0xA,
        STATUS_WAIT_BOSS2 = 0xB,
    };
    enum Flag {
        FLAG_UNK02 = 0x02,
        FLAG_UNK04 = 0x04,
        FLAG_BOOMERANG = 0x08,
        FLAG_UNK10 = 0x10,
        FLAG_QUAKE = 0x20,
        FLAG_HOOK = 0x40,
    };
    enum Mode { MODE_WAIT = 0x0, MODE_WATER = 0x2 };

    /* 0x750 */ cXyz mScaleTarget;
    /* 0x75C */ be<s32> mSpawnSwitchNo;
    /* 0x760 */ be<s32> mCollideSwitchNo;
    /* 0x764 */ be<f32> field_0x650;       /* the last non-zero speed.y */
    /* 0x768 */ be<f32> mHD768;
    /* 0x76C */ be<s16> mHDRotY;           /* HD: added to the Y rotation of the model */
    /* 0x76E */ be<s16> mRotateSpeed;
    /* 0x770 */ be<s16> mTargetAngleX;
    /* 0x772 */ be<s16> mWaitTimer;
    /* 0x774 */ be<s16> mDisappearTimer;
    /* 0x776 */ be<s16> mSimpleExistTimer;
    /* 0x778 */ be<s16> mExtraZRot;
    /* 0x77A */ be<s16> field_0x660;
    /* 0x77C */ u8 _77C[4];
    /* 0x780 */ be<u8> field_0x666;
    /* 0x781 */ be<u8> mType;
    /* 0x782 */ be<u8> mAction;
    /* 0x783 */ be<u8> mFlag;
    /* 0x784 */ be<u8> mMode;
    /* 0x785 */ be<u8> mItemStatus;
    /* 0x786 */ be<u8> mOnGroundTimer;
    /* 0x787 */ be<u8> mHD787;
    /* 0x788 */ be<u8> mHD788;
    /* 0x789 */ u8 _789[3];
    /* 0x78C */ cXyz mHDTargetPos;         /* HD action 0xD: where the item flies to */
    /* 0x798 */ be<f32> mHDTargetSpeedF;
    /* 0x79C */ be<u8> mHD79C;
    /* 0x79D */ be<u8> mHD79D;
    /* 0x79E */ be<u8> mHD79E;             /* HD action 0xF */
    /* 0x79F */ u8 _79F;
    /* 0x7A0 */ be<u32> mDemoItemBsPcId;
    /* 0x7A4 */ u8 mPtclRippleCb[0x14];    /* dPa_rippleEcallBack */
    /* 0x7B8 */ dPa_followEcallBack mPtclFollowCb;
    /* 0x7CC */ u8 mPtclSmokeCb[0x20];     /* dPa_smokeEcallBack (vtable at +0) */
    /* 0x7EC */ gptr<JPABaseEmitter> mpParticleEmitter;

    void setFlag(u8 flag) { mFlag = mFlag | flag; }
    void clrFlag(u8 flag) { mFlag = mFlag & (u8)~flag; }
    bool checkFlag(u8 flag) { return (mFlag & flag) != 0; }

    f32 getYOffset();
    void set_mtx();
    void set_mtx_base(J3DModel*, cXyz*, csXyz*);
    void CreateInit();
    cPhs_State _daItem_create();
    BOOL _daItem_execute();
    void mode_proc_call();
    void execInitNormalDirection();
    void execMainNormalDirection();
    void execInitGetDemoDirection();
    void execWaitGetDemoDirection();
    void execMainGetDemoDirection();
    void orderItemEvent();
    void execBringNezumi();
    void execWaitMain();
    void execWaitMainFromBoss();
    void scaleAnimFromBossItem();
    BOOL _daItem_draw();
    void setTevStr();
    BOOL _daItem_delete();
    void itemGetExecute();
    void itemDefaultRotateY();
    BOOL checkItemDisappear();
    void setItemTimer(int);
    BOOL checkPlayerGet();
    BOOL itemActionForRupee();
    BOOL itemActionForHeart();
    BOOL itemActionForKey();
    BOOL itemActionForEmono();
    BOOL itemActionForSword();
    BOOL itemActionForArrow();
    void checkWall();
    void set_bound_se();
    void checkHD14Delete();
    void waterFloat();
    BOOL itemActionForHD13();
    BOOL checkGetItem();
    BOOL timeCount();
    void mode_wait_init();
    void mode_water_init();
    void mode_wait();
    void mode_water();
    BOOL initAction();

    BOOL checkControl();
    BOOL checkLock();
    BOOL setLock();
    BOOL releaseLock();
    BOOL checkActionNow();
};
WWHD_OFFSET(daItem_c, mScaleTarget, 0x750);
WWHD_OFFSET(daItem_c, mRotateSpeed, 0x76E);
WWHD_OFFSET(daItem_c, field_0x666, 0x780);
WWHD_OFFSET(daItem_c, mItemStatus, 0x785);
WWHD_OFFSET(daItem_c, mHDTargetPos, 0x78C);
WWHD_OFFSET(daItem_c, mDemoItemBsPcId, 0x7A0);
WWHD_OFFSET(daItem_c, mPtclFollowCb, 0x7B8);
WWHD_OFFSET(daItem_c, mpParticleEmitter, 0x7EC);
WWHD_SIZE(daItem_c, 0x7F0);

namespace daItem_prm {
inline u32 getType(daItem_c* i_this) { return (fopAcM_GetParam(i_this) & 0x03000000) >> 0x18; }
inline u32 getAction(daItem_c* i_this) { return (fopAcM_GetParam(i_this) & 0xFC000000) >> 0x1A; }
inline u32 getItemNo(daItem_c* i_this) { return (fopAcM_GetParam(i_this) & 0x000000FF) >> 0x00; }
inline u32 getItemBitNo(daItem_c* i_this) { return (fopAcM_GetParam(i_this) & 0x0000FF00) >> 0x08; }
inline u32 getSwitchNo(daItem_c* i_this) { return (i_this->home.angle.z & 0x00FF) >> 0; }
inline u32 getSwitchNo2(daItem_c* i_this) { return (fopAcM_GetParam(i_this) & 0x00FF0000) >> 0x10; }
}  // namespace daItem_prm

/* daItemBase_c::m_data (this TU's copy, .rodata 0x100121B0) */
struct daItemBase_c_m_data {
    /* 0x00 */ be<f32> mGravity;
    /* 0x04 */ be<f32> mGroundReflect;
    /* 0x08 */ be<f32> mLaunchSpeed;
    /* 0x0C */ be<f32> mScaleAnimSpeed;
    /* 0x10 */ be<f32> mSpeedH;
    /* 0x14 */ be<s16> mFlashCycleTime;
    /* 0x16 */ be<s16> mWaitTime;
    /* 0x18 */ be<s16> mDisappearTime;
    /* 0x1A */ be<s16> mRotateXSpeed;
    /* 0x1C */ be<s16> mRotateYSpeed;
    /* 0x1E */ u8 _1E[2];
    /* 0x20 */ be<f32> mHeartFallSpeed;
    /* 0x24 */ be<f32> mHeartAmplitude;
    /* 0x28 */ be<s16> mHeartFallCycleTime;
    /* 0x2A */ be<s16> mHeartTilt;
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> mGetDemoLaunchSpeed;
    /* 0x3C */ be<f32> mGetDemoGravity;
    /* 0x40 */ be<s16> mSimpleExistTime;
    /* 0x42 */ be<s16> mNoGetTime;
    /* 0x44 */ be<f32> field_0x44;
    /* 0x48 */ be<f32> mVelocityScale;
};
WWHD_SIZE(daItemBase_c_m_data, 0x4C);
inline const daItemBase_c_m_data* daItem_getData() { return gabi::at<daItemBase_c_m_data>(0x100121B0); }
