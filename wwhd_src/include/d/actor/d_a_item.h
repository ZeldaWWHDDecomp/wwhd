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
    typedef void (daItem_c::*ModeFunc)();
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
    // HD: the thrown pickup launches, bounces and either splashes or vanishes.
    enum ThrownItemState {
        THROWN_WAIT,
        THROWN_LAUNCH,
        THROWN_FLY,
        THROWN_VANISH,
        THROWN_SMOKE,
        THROWN_DELETE,
        THROWN_SPLASH,
        THROWN_FLOAT,
    };

    /* 0x750 */ cXyz mScaleTarget;
    /* 0x75C */ be<s32> mSpawnSwitchNo;
    /* 0x760 */ be<s32> mCollideSwitchNo;
    /* 0x764 */ be<f32> field_0x650; /* the last non-zero speed.y */
    /* 0x768 */ be<f32> mWaterAcceleration;
    /* 0x76C */ be<s16> mHDRotY; /* HD: added to the Y rotation of the model */
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
    /* 0x787 */ be<u8> mThrownItemState;
    /* 0x788 */ be<u8> mThrownItemTimer;
    /* 0x789 */ u8 _789[3];
    /* 0x78C */ cXyz mHDTargetPos; /* HD action 0xD: where the item flies to */
    /* 0x798 */ be<f32> mHDTargetSpeedF;
    /* 0x79C */ be<u8> mGroundAboveWater;
    /* 0x79D */ be<u8> mWaterFloatInitialized;
    /* 0x79E */ be<u8> mNoCollect; /* HD action 0xF */
    /* 0x79F */ u8 _79F;
    /* 0x7A0 */ be<u32> mDemoItemBsPcId;
    /* 0x7A4 */ u8 mPtclRippleCb[0x14]; /* dPa_rippleEcallBack */
    /* 0x7B8 */ dPa_followEcallBack mPtclFollowCb;
    /* 0x7CC */ u8 mPtclSmokeCb[0x20]; /* dPa_smokeEcallBack (vtable at +0) */
    /* 0x7EC */ gptr<JPABaseEmitter> mpParticleEmitter;

    void setFlag(u8 flag) { mFlag = mFlag | flag; }
    void clrFlag(u8 flag) { mFlag = mFlag & (u8)~flag; }
    bool checkFlag(u8 flag) { return (mFlag & flag) != 0; }

    f32 getYOffset();
    void set_mtx();
    void set_mtx_base(J3DModel *, cXyz *, csXyz *);
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
    void checkTingleBottleDelete();
    void waterFloat();
    BOOL itemActionForThrownItem();
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
inline u32 getType(daItem_c *i_this) { return (fopAcM_GetParam(i_this) & 0x03000000) >> 0x18; }
inline u32 getAction(daItem_c *i_this) { return (fopAcM_GetParam(i_this) & 0xFC000000) >> 0x1A; }
inline u32 getItemNo(daItem_c *i_this) { return (fopAcM_GetParam(i_this) & 0x000000FF) >> 0x00; }
inline u32 getItemBitNo(daItem_c *i_this) { return (fopAcM_GetParam(i_this) & 0x0000FF00) >> 0x08; }
inline u32 getSwitchNo(daItem_c *i_this) { return (i_this->home.angle.z & 0x00FF) >> 0; }
inline u32 getSwitchNo2(daItem_c *i_this) { return (fopAcM_GetParam(i_this) & 0x00FF0000) >> 0x10; }
} // namespace daItem_prm

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
const daItemBase_c_m_data *daItem_getData();

// HD: private views of the item code's emitter, callback and save-data prefixes.
struct ItemEmitter {
    u8 pad0[0x5C];
    be<s32> mMaxFrame;
    u8 pad60[0x22C - 0x60];
    cXyz mGlobalTranslation;
    u8 pad238[0x254 - 0x238];
    be<u32> mFlags;
    u8 pad258[10];
    be<u8> mVersion;
};
WWHD_OFFSET(ItemEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(ItemEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(ItemEmitter, mFlags, 0x254);
WWHD_OFFSET(ItemEmitter, mVersion, 0x262);
WWHD_SIZE(ItemEmitter, 0x263);
struct ItemCallbackVtable {
    u8 pad0[0x44];
    be<u32> mRemove;
};
WWHD_OFFSET(ItemCallbackVtable, mRemove, 0x44);
WWHD_SIZE(ItemCallbackVtable, 0x48);
struct ItemCallback {
    gptr<ItemCallbackVtable> mVtable;
};
WWHD_SIZE(ItemCallback, 4);
struct ItemBaseVtable {
    u8 pad0[0x14];
    be<u32> mDrawBase;
};
WWHD_OFFSET(ItemBaseVtable, mDrawBase, 0x14);
WWHD_SIZE(ItemBaseVtable, 0x18);
struct ItemPlayerVtable {
    u8 pad0[0xE4];
    be<u32> mItemAction;
};
WWHD_OFFSET(ItemPlayerVtable, mItemAction, 0xE4);
WWHD_SIZE(ItemPlayerVtable, 0xE8);
struct ItemSave {
    u8 pad0[0x20];
    be<u8> mSave;
    u8 pad21[0x598 - 0x21];
    be<u8> mMemory;
    u8 pad599[0x12C0 - 0x599];
    be<u8> mGameMode;
};
WWHD_OFFSET(ItemSave, mSave, 0x20);
WWHD_OFFSET(ItemSave, mMemory, 0x598);
WWHD_OFFSET(ItemSave, mGameMode, 0x12C0);
WWHD_SIZE(ItemSave, 0x12C1);
struct ItemCollisionActor {
    u8 pad0[0xE];
    be<s16> mName;
};
WWHD_OFFSET(ItemCollisionActor, mName, 0xE);
WWHD_SIZE(ItemCollisionActor, 0x10);
struct ItemTevColors {
    u8 pad0[0x90];
    GXColorS10 mColorC0;
    GXColor mColorK0;
};
WWHD_OFFSET(ItemTevColors, mColorC0, 0x90);
WWHD_OFFSET(ItemTevColors, mColorK0, 0x98);
WWHD_SIZE(ItemTevColors, 0x9C);
struct ItemBottleState {
    u8 pad0[0x4FD4];
    be<u8> mDeleteBottle;
};
WWHD_OFFSET(ItemBottleState, mDeleteBottle, 0x4FD4);
WWHD_SIZE(ItemBottleState, 0x4FD5);
struct ItemThrowFrame {
    u8 pad0[0x20];
    cXyz mLanding;
};
WWHD_OFFSET(ItemThrowFrame, mLanding, 0x20);
WWHD_SIZE(ItemThrowFrame, 0x2C);
struct ItemRipple {
    u8 pad0[0x10];
    be<f32> mRate;
};
WWHD_OFFSET(ItemRipple, mRate, 0x10);
WWHD_SIZE(ItemRipple, 0x14);
struct ItemInfo {
    be<u8> mShadowSize, mHeight, mRadius, mFlags;
};
WWHD_SIZE(ItemInfo, 4);
struct ItemFieldResource {
    gptr<const be<u8>> mArcName;
    u8 pad4[0x18 - 4];
    be<u16> mHeapSize;
    u8 pad1A[2];
};
WWHD_OFFSET(ItemFieldResource, mHeapSize, 0x18);
WWHD_SIZE(ItemFieldResource, 0x1C);

struct ItemInfoTable {
    ItemInfo mItems[256];
};
WWHD_SIZE(ItemInfoTable, 0x400);
struct ItemResourceTable {
    ItemFieldResource mItems[256];
};
WWHD_SIZE(ItemResourceTable, 0x1C00);
struct ItemBoundSounds {
    be<u16> mSound[15];
};
WWHD_SIZE(ItemBoundSounds, 30);
struct ItemArrowSounds {
    be<u16> mSound[20];
};
WWHD_SIZE(ItemArrowSounds, 40);

struct ItemBottleGlobal {
    gptr<ItemBottleState> mpState;
};
WWHD_SIZE(ItemBottleGlobal, 4);
struct ItemWaterGlobal {
    gptr<be<u8>> mpState;
};
WWHD_SIZE(ItemWaterGlobal, 4);
struct ItemModeEntry {
    gabi::ptmf<daItem_c::ModeFunc> mProc;
};
WWHD_SIZE(ItemModeEntry, 8);

struct dEvent_manager_c;
s16 itemTargetAngleYHD(cXyz *from, cXyz *to);
void fopAcM_addAngleY(fopAc_ac_c *actor, s16 target, s16 step);
void fopAcM_orderItemEvent(fopAc_ac_c *actor, u16 param);
u32 fopAcM_createItemForPresentDemo(cXyz *pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo,
                                    csXyz *angle, cXyz *scale);
BOOL itemEndCheckOld(dEvent_manager_c *manager, const char *name);
BOOL checkSpecialEffect(u8 itemNo);
u16 getSpecialEffect(u8 itemNo);
void mDoAud_seStart_id(u32 id);
void mDoAud_seStartName(const char *name);
void itemClearCoHit(dCcD_Cyl *cyl);
void itemOnSwitch(be<u8> *memory, s32 bit);
void itemOnItem(be<u8> *save, s32 bit, s32 room);
void cXyz_normalize(const cXyz *vector, cXyz *result);
BOOL itemCheckQuake(dDetect_c *detect, cXyz *pos);
void itemDestroyCheck(dBgS_GndChk *check, s32 flags);
BOOL itemWaterPositionTest(be<u8> *state, cXyz *pos);
BOOL itemCheckAppearEffect(u8 itemNo);
u16 itemGetAppearEffect(u8 itemNo);
f32 itemStickValue(s32 port);
void itemRippleCtor(void *callback);
void itemSmokeCtor(void *callback, s32 mode);
u32 itemGetGameMode(be<u8> *save);
BOOL itemDisallowHearts(u32 mode);
void itemBaseDestructor(daItem_c *item, s32 flags);
