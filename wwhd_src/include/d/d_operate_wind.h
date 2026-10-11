#ifndef D_OPERATE_WIND_H
#define D_OPERATE_WIND_H
#include "d/d_com_inf_game.h"
#include "gabi.h"
#include "wwhd.h"

// HD: layout animations and GamePad controls replacing the GameCube screen effects.
enum dOwAnimation_e : u32 {
    dOwAnimation_InAll_e = 0,
    dOwAnimation_WindNow_e = 1,
    dOwAnimation_WindChange_e = 2,
    dOwAnimation_OutAll_e = 3,
    dOwAnimation_Count_e = 4,
};
enum dOwTouchMode_e : u32 { dOwTouch_Idle_e = 0, dOwTouch_Drag_e = 1 };
enum dOwPadFlag_e : u32 {
    dOwPad_TouchValid_e = 1,
    dOwPad_TouchHeld_e = 0x8000,
    dOwPad_RepeatMask_e = 0x00F00000,
    dOwPad_Clockwise_e = 0x00440000,
    dOwPad_CounterClockwise_e = 0x00880000,
};
enum dOwPaneFlag_e : u8 { dOwPane_Visible_e = 1, dOwPane_RotationChanged_e = 0x10 };
enum dOwAngle_e : u32 {
    dOwAngle_HalfStep_e = 0x1000,
    dOwAngle_Step_e = 0x2000,
    dOwAngle_DirectionMask_e = 0xE000
};
enum dOwStateSlot_e : u32 { dOwState_FirstMethod_e = 25, dOwState_MethodCount_e = 3 };

struct dOwLayout_c;
struct dOwPaneManager_c;
struct dOwPaneOwner_c;
struct dOwPane_c;
struct dOwStick_c;

// HD: the GamePad wind screen replaces the GameCube J2D screen with a
// layout and state machine. Preserve the verified 32-bit register ABI.
struct dOperate_wind_c {
    void _delete(u32 flag);
    void wait();
    void arrowRotate(f32 angle);
    void cursorRotate(f32 angle);
    void initialize();
    void initializeState_InAll();
    void executeState_InAll();
    void initializeState_Disp();
    void finalizeState_Disp();
    void windSet();
    u32 checkTouchBlocked();
    void touchAngle();
    u32 isTouchNearCursor();
    void requestChange();
    void touchControl();
    void buttonControl();
    void arrowDirection();
    void executeState_Disp();
    void executeState_OutWait();
    void initializeState_OutAll();
    void executeState_OutAll();
    void finalizeState_OutAll();
    void screenSet();
    void _execute();
    void _move();
    void _draw();
    u32 loadLayout(u32 archive, u32 a, u32 b, u32 c, u32 d);
    u32 _create(u32 archive, u32 a, u32 b);

    /* 0x000 */ gabi::be<u32> mBaseVtable;
    /* 0x004 */ gabi::be<u32> mVtable;
    u8 mReserved08[0x10];
    /* 0x018 */ gabi::be<u32> mStateManager;
    u8 mReserved1C[0x4];
    /* 0x020 */ gabi::be<u32> mState;
    u8 mReserved24[0x8];
    /* 0x02C */ gabi::be<u32> mStateIdVtable;
    /* 0x030 */ gabi::be<u32> mStateFactoryVtable;
    /* 0x034 */ gabi::gptr<void> mpArchive;
    u8 mReserved38[0xC];
    /* 0x044 */ gabi::gptr<dOwLayout_c> mpLayout;
    u8 mReserved48[0x8];
    /* 0x050 */ gabi::gptr<dOwPane_c> mpCenterPane;
    /* 0x054 */ gabi::gptr<dOwPane_c> mpWindArrow;
    /* 0x058 */ gabi::gptr<dOwPane_c> mpCursorArrow;
    /* 0x05C */ gabi::gptr<dOwPane_c> mpCursorPane;
    /* 0x060 */ gabi::gptr<dOwStick_c> stick;
    /* 0x064 */ union {
        gabi::be<f32> mPosX;
        gabi::be<u32> mPosXBits;
    };
    /* 0x068 */ union {
        gabi::be<f32> mPosY;
        gabi::be<u32> mPosYBits;
    };
    /* 0x06C */ union {
        gabi::be<f32> mPosZ;
        gabi::be<u32> mPosZBits;
    };
    /* 0x070 */ gabi::be<f32> mTouchX;
    /* 0x074 */ gabi::be<f32> mTouchY;
    /* 0x078 */ gabi::be<u32> mArrowAngle;
    /* 0x07C */ gabi::be<u32> mTargetAngle;
    /* 0x080 */ gabi::be<f32> mStickValue;
    /* 0x084 */ gabi::be<f32> mCursorRadiusSquared;
    /* 0x088 */ gabi::be<f32> mCenterRadiusSquared;
    /* 0x08C */ gabi::be<u32> mTouchMode;
    /* 0x090 */ gabi::be<u16> mWindAngle;
    /* 0x092 */ gabi::be<u16> mDeg;
    /* 0x094 */ gabi::be<u16> mPreviousDeg;
    /* 0x096 */ gabi::be<u16> mTimer;
    /* 0x098 */ gabi::be<u8> mVisible;
    /* 0x099 */ gabi::be<u8> mTouchAngleSet;
    /* 0x09A */ gabi::be<u8> mTouchBlocked;
    /* 0x09B */ gabi::be<u8> mDragging;
    /* 0x09C */ gabi::be<u8> mChangeRequested;
    /* 0x09D */ gabi::be<u8> mChanged;
    /* 0x09E */ gabi::be<u8> mCheckChange;
    /* 0x09F */ gabi::be<u8> mChangeBlocked;
    /* 0x0A0 */ gabi::be<u8> mActive;
    u8 mReservedEnd[3];
};
WWHD_OFFSET(dOperate_wind_c, mBaseVtable, 0x0);
WWHD_OFFSET(dOperate_wind_c, mVtable, 0x4);
WWHD_OFFSET(dOperate_wind_c, mStateManager, 0x18);
WWHD_OFFSET(dOperate_wind_c, mState, 0x20);
WWHD_OFFSET(dOperate_wind_c, mStateIdVtable, 0x2C);
WWHD_OFFSET(dOperate_wind_c, mStateFactoryVtable, 0x30);
WWHD_OFFSET(dOperate_wind_c, mpArchive, 0x34);
WWHD_OFFSET(dOperate_wind_c, mpLayout, 0x44);
WWHD_OFFSET(dOperate_wind_c, mpCenterPane, 0x50);
WWHD_OFFSET(dOperate_wind_c, mpWindArrow, 0x54);
WWHD_OFFSET(dOperate_wind_c, mpCursorArrow, 0x58);
WWHD_OFFSET(dOperate_wind_c, mpCursorPane, 0x5C);
WWHD_OFFSET(dOperate_wind_c, stick, 0x60);
WWHD_OFFSET(dOperate_wind_c, mPosX, 0x64);
WWHD_OFFSET(dOperate_wind_c, mPosY, 0x68);
WWHD_OFFSET(dOperate_wind_c, mPosZ, 0x6C);
WWHD_OFFSET(dOperate_wind_c, mTouchX, 0x70);
WWHD_OFFSET(dOperate_wind_c, mTouchY, 0x74);
WWHD_OFFSET(dOperate_wind_c, mArrowAngle, 0x78);
WWHD_OFFSET(dOperate_wind_c, mTargetAngle, 0x7C);
WWHD_OFFSET(dOperate_wind_c, mStickValue, 0x80);
WWHD_OFFSET(dOperate_wind_c, mCursorRadiusSquared, 0x84);
WWHD_OFFSET(dOperate_wind_c, mCenterRadiusSquared, 0x88);
WWHD_OFFSET(dOperate_wind_c, mTouchMode, 0x8C);
WWHD_OFFSET(dOperate_wind_c, mWindAngle, 0x90);
WWHD_OFFSET(dOperate_wind_c, mDeg, 0x92);
WWHD_OFFSET(dOperate_wind_c, mPreviousDeg, 0x94);
WWHD_OFFSET(dOperate_wind_c, mTimer, 0x96);
WWHD_OFFSET(dOperate_wind_c, mVisible, 0x98);
WWHD_OFFSET(dOperate_wind_c, mTouchAngleSet, 0x99);
WWHD_OFFSET(dOperate_wind_c, mTouchBlocked, 0x9A);
WWHD_OFFSET(dOperate_wind_c, mDragging, 0x9B);
WWHD_OFFSET(dOperate_wind_c, mChangeRequested, 0x9C);
WWHD_OFFSET(dOperate_wind_c, mChanged, 0x9D);
WWHD_OFFSET(dOperate_wind_c, mCheckChange, 0x9E);
WWHD_OFFSET(dOperate_wind_c, mChangeBlocked, 0x9F);
WWHD_OFFSET(dOperate_wind_c, mActive, 0xA0);
WWHD_SIZE(dOperate_wind_c, 0xA4);

void dOw_operatorDelete(u32 a0);
void dOw_layoutScreenDtor(u32 a0, u32 a1);
u32 dOw_getWindAngle();
void dOw_changeState(u32 a0, u32 a1);
void dOw_callArrowRotate(u32 a0, f32 a1);
void dOw_callCursorRotate(u32 a0, f32 a1);
void dOw_startAnimation(u32 a0, u32 a1, u32 a2, f32 a3);
void dOw_callInitialize(u32 a0);
u32 dOw_isAnimationDone(u32 a0, u32 a1);
void dOw_tactWindSet(u32 a0, s16 a1);
u32 dOw_isInputBlocked(u32 a0);
u32 dOw_containsPoint(u32 a0, u32 a1);
u32 dOw_atan2sRegister(f32 a0, f32 a1);
void dOw_getPanePosition(u32 a0, u32 a1);
void dOw_notifyWindChange(u32 a0);
s32 dOw_callCheckTouchBlocked(u32 a0);
u32 dOw_callIsTouchHeld(u32 a0);
void dOw_playSound(u32 a0);
void dOw_callTouchAngle(u32 a0);
u32 dOw_callIsTouchNearCursor(u32 a0);
void dOw_callRequestChange(u32 a0);
void dOw_callTouchControl(u32 a0);
void dOw_callButtonControl(u32 a0);
void dOw_addCalcAngle(u32 a0, u32 a1, u32 a2);
void dOw_callArrowDirection(u32 a0);
void dOw_callWindSet(u32 a0);
u32 dOw_operatorNew(u32 a0);
void dOw_layoutScreenCtor(u32 a0);
u32 dOw_stControlCtor(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f32 a7, f32 a8);
void dOw_layoutScreenExecute(u32 a0);
void dOw_executeStateManager(u32 a0);
u32 dOw_heapNew(u32 a0, u32 a1, u32 a2);
u32 dOw_layoutCtor(u32 a0);
void dOw_bindAnimation(u32 a0, u32 a1, u32 a2, u32 a3);
void dOw_setLayoutName(u32 a0, u32 a1, u32 a2);
void dOw_callCenterPane(u32 a0);
void dOw_registerRange(u32 a0);
void dOw_initializeRangeFlag(u32 a0);
void dOw_initializeScaleFlag(u32 a0);

// HD: non-owning views of the layout, input and UI state collaborators.
struct dOwPane_c {
    u8 mReserved00[0x1C];
    /* 0x1C */ gabi::be<u32> mPositionXBits;
    /* 0x20 */ gabi::be<u32> mPositionYBits;
    /* 0x24 */ gabi::be<u32> mPositionZBits;
    union {
        gabi::be<u32> mRotationXBits;
        gabi::be<f32> mRotationX;
    };
    union {
        gabi::be<u32> mRotationYBits;
        gabi::be<f32> mRotationY;
    };
    union {
        gabi::be<u32> mRotationZBits;
        gabi::be<f32> mRotationZ;
    };
    u8 mReserved34[0x8];
    /* 0x3C */ gabi::be<f32> mWidth;
    /* 0x40 */ gabi::be<f32> mHeight;
    /* 0x44 */ gabi::be<u8> mFlags;
};
WWHD_OFFSET(dOwPane_c, mPositionXBits, 0x1C);
WWHD_OFFSET(dOwPane_c, mPositionYBits, 0x20);
WWHD_OFFSET(dOwPane_c, mPositionZBits, 0x24);
WWHD_OFFSET(dOwPane_c, mRotationXBits, 0x28);
WWHD_OFFSET(dOwPane_c, mRotationYBits, 0x2C);
WWHD_OFFSET(dOwPane_c, mRotationZBits, 0x30);
WWHD_OFFSET(dOwPane_c, mWidth, 0x3C);
WWHD_OFFSET(dOwPane_c, mHeight, 0x40);
WWHD_OFFSET(dOwPane_c, mFlags, 0x44);
WWHD_SIZE(dOwPane_c, 0x45);
// HD: the six wind-screen action/backup bytes are a private projection of game info.
// The shared game-info layout owns mOperateWind; access it through that layout.
struct dOwPlayView_c {
    dComIfG_play_c *getPlay() { return &gabi::at<dComIfG_inf_c>(gabi::ea(this))->play; }
    u8 mReserved00[0x5C28];
    /* 0x5C28 */ gabi::be<u8> mActionButtonA;
    /* 0x5C29 */ gabi::be<u8> mActionButtonB;
    /* 0x5C2A */ gabi::be<u8> mActionButtonR;
    /* 0x5C2B */ gabi::be<u8> mSavedActionButtonA;
    /* 0x5C2C */ gabi::be<u8> mSavedActionButtonB;
    /* 0x5C2D */ gabi::be<u8> mSavedActionButtonR;
};
WWHD_OFFSET(dOwPlayView_c, mActionButtonA, 0x5C28);
WWHD_OFFSET(dOwPlayView_c, mActionButtonB, 0x5C29);
WWHD_OFFSET(dOwPlayView_c, mActionButtonR, 0x5C2A);
WWHD_OFFSET(dOwPlayView_c, mSavedActionButtonA, 0x5C2B);
WWHD_OFFSET(dOwPlayView_c, mSavedActionButtonB, 0x5C2C);
WWHD_OFFSET(dOwPlayView_c, mSavedActionButtonR, 0x5C2D);
WWHD_SIZE(dOwPlayView_c, 0x5C2E);
struct dOwPadView_c {
    u8 mReserved00[0x18];
    /* 0x18 */ gabi::be<u32> mHoldButtons;
    u8 mReserved1C[0x4];
    /* 0x20 */ gabi::be<u32> mTriggerButtons;
    /* 0x24 */ gabi::be<u32> mTouchFlags;
    u8 mReserved28[0xFC];
    /* 0x124 */ gabi::be<u32> mRepeatButtons;
};
WWHD_OFFSET(dOwPadView_c, mHoldButtons, 0x18);
WWHD_OFFSET(dOwPadView_c, mTriggerButtons, 0x20);
WWHD_OFFSET(dOwPadView_c, mTouchFlags, 0x24);
WWHD_OFFSET(dOwPadView_c, mRepeatButtons, 0x124);
WWHD_SIZE(dOwPadView_c, 0x128);
struct dOwLayout_c {
    u8 mReserved00[0x4];
    /* 0x4 */ gabi::gptr<dOwPaneManager_c> mpPaneManager;
    u8 mReserved08[0xCC];
    /* 0xD4 */ gabi::be<u32> mpAnimator;
    u8 mReservedD8[0x8];
    /* 0xE0 */ gabi::be<u32> mVtable;
};
WWHD_OFFSET(dOwLayout_c, mpPaneManager, 0x4);
WWHD_OFFSET(dOwLayout_c, mpAnimator, 0xD4);
WWHD_OFFSET(dOwLayout_c, mVtable, 0xE0);
WWHD_SIZE(dOwLayout_c, 0xE4);
struct dOwPaneManager_c {
    u8 mReserved00[0xC];
    /* 0xC */ gabi::gptr<dOwPaneOwner_c> mpRootPane;
};
WWHD_OFFSET(dOwPaneManager_c, mpRootPane, 0xC);
WWHD_SIZE(dOwPaneManager_c, 0x10);
struct dOwPaneOwner_c {
    u8 mReserved00[0x8];
    /* 0x8 */ gabi::be<u32> mVtable;
};
WWHD_OFFSET(dOwPaneOwner_c, mVtable, 0x8);
WWHD_SIZE(dOwPaneOwner_c, 0xC);
struct dOwManager_c {
    u8 mReserved00[0x52];
    /* 0x52 */ gabi::be<u8> mConfirmRequested;
    /* 0x53 */ gabi::be<u8> mCancelRequested;
    /* 0x54 */ gabi::be<u8> mConfirmDone;
};
WWHD_OFFSET(dOwManager_c, mConfirmRequested, 0x52);
WWHD_OFFSET(dOwManager_c, mCancelRequested, 0x53);
WWHD_OFFSET(dOwManager_c, mConfirmDone, 0x54);
WWHD_SIZE(dOwManager_c, 0x55);
struct dOwRoot_c {
    u8 mReserved00[0x1C8];
    /* 0x1C8 */ gabi::be<u32> mpWindManager;
};
WWHD_OFFSET(dOwRoot_c, mpWindManager, 0x1C8);
WWHD_SIZE(dOwRoot_c, 0x1CC);
struct dOwHeapRoot_c {
    u8 mReserved00[0x10];
    /* 0x10 */ gabi::be<u32> mpHeap;
};
WWHD_OFFSET(dOwHeapRoot_c, mpHeap, 0x10);
WWHD_SIZE(dOwHeapRoot_c, 0x14);
struct dOwStick_c {
    u8 mReserved00[0x24];
    /* 0x24 */ gabi::be<u32> mVtable;
};
WWHD_OFFSET(dOwStick_c, mVtable, 0x24);
WWHD_SIZE(dOwStick_c, 0x28);
struct dOwSafeString_c {
    /* 0x0 */ gabi::be<u32> mpString;
    /* 0x4 */ gabi::be<u32> mVtable;
};
WWHD_OFFSET(dOwSafeString_c, mpString, 0x0);
WWHD_OFFSET(dOwSafeString_c, mVtable, 0x4);
WWHD_SIZE(dOwSafeString_c, 0x8);
struct dOwStateId_c {
    /* 0x0 */ gabi::be<s32> mId;
    /* 0x4 */ gabi::be<u32> mpName;
    /* 0x8 */ gabi::be<u32> mVtable;
};
WWHD_OFFSET(dOwStateId_c, mId, 0x0);
WWHD_OFFSET(dOwStateId_c, mpName, 0x4);
WWHD_OFFSET(dOwStateId_c, mVtable, 0x8);
WWHD_SIZE(dOwStateId_c, 0xC);
struct dOwPoint_c {
    /* 0x0 */ gabi::be<f32> x;
    /* 0x4 */ gabi::be<f32> y;
};
WWHD_OFFSET(dOwPoint_c, x, 0x0);
WWHD_OFFSET(dOwPoint_c, y, 0x4);
WWHD_SIZE(dOwPoint_c, 0x8);

// HD: 8-byte GHS member-function descriptors; the last halfword overlaps the target word.
struct dOwMemberFunction_c {
    gabi::be<s16> mThisAdjustment;
    gabi::be<s16> mVirtualIndex;
    union {
        gabi::be<u32> mFunction;
        struct {
            gabi::be<u16> mTargetHigh;
            gabi::be<s16> mVtableOffset;
        };
    };
};
WWHD_OFFSET(dOwMemberFunction_c, mThisAdjustment, 0);
WWHD_OFFSET(dOwMemberFunction_c, mVirtualIndex, 2);
WWHD_OFFSET(dOwMemberFunction_c, mFunction, 4);
WWHD_OFFSET(dOwMemberFunction_c, mVtableOffset, 6);
WWHD_SIZE(dOwMemberFunction_c, 8);
struct dOwStateDescriptor_c {
    gabi::be<u32> mId;
    gabi::be<u32> mpName;
    gabi::be<u32> mVtable;
    dOwMemberFunction_c mInitialize;
    dOwMemberFunction_c mExecute;
    dOwMemberFunction_c mFinalize;
    dOwMemberFunction_c mDefault;
    gabi::be<u32> mFactoryVtable;
};
WWHD_OFFSET(dOwStateDescriptor_c, mInitialize, 12);
WWHD_OFFSET(dOwStateDescriptor_c, mExecute, 20);
WWHD_OFFSET(dOwStateDescriptor_c, mFinalize, 28);
WWHD_OFFSET(dOwStateDescriptor_c, mDefault, 36);
WWHD_OFFSET(dOwStateDescriptor_c, mFactoryVtable, 44);
WWHD_SIZE(dOwStateDescriptor_c, 48);

struct dOwZeroBlock_c {
    gabi::be<u32> mWords[4];
};
WWHD_SIZE(dOwZeroBlock_c, 16);
struct dOwRegistration_c {
    u8 mRecord[12];
};
WWHD_SIZE(dOwRegistration_c, 12);
struct dOwVtableIdentity_c {
    gabi::be<u32> mHeader;
};
WWHD_SIZE(dOwVtableIdentity_c, 4);
struct dOwUiFlags_c {
    u8 mReserved[0x40];
    gabi::be<u8> mControlMode;
    u8 mReservedControl[0x19];
    gabi::be<u8> mWindControl;
};
WWHD_OFFSET(dOwUiFlags_c, mControlMode, 0x40);
WWHD_OFFSET(dOwUiFlags_c, mWindControl, 0x5A);
WWHD_SIZE(dOwUiFlags_c, 0x5B);

// HD: GHS virtual slots, including the four-byte adjustment word before each target.
struct dOwStateVtable_c {
    u8 mReserved00[0x14];
    /* 0x14 */ gabi::be<u32> mGetState;
    u8 mReserved18[0x4];
    /* 0x1C */ gabi::be<u32> mInitialize;
};
WWHD_OFFSET(dOwStateVtable_c, mGetState, 0x14);
WWHD_OFFSET(dOwStateVtable_c, mInitialize, 0x1C);
WWHD_SIZE(dOwStateVtable_c, 0x20);
struct dOwStickVtable_c {
    u8 mReserved00[0xC];
    /* 0xC */ gabi::be<u32> mDestroy;
    u8 mReserved10[0x4];
    /* 0x14 */ gabi::be<u32> mGetValue;
};
WWHD_OFFSET(dOwStickVtable_c, mDestroy, 0xC);
WWHD_OFFSET(dOwStickVtable_c, mGetValue, 0x14);
WWHD_SIZE(dOwStickVtable_c, 0x18);
struct dOwSafeStringVtable_c {
    u8 mReserved00[0x14];
    /* 0x14 */ gabi::be<u32> mAssureTermination;
};
WWHD_OFFSET(dOwSafeStringVtable_c, mAssureTermination, 0x14);
WWHD_SIZE(dOwSafeStringVtable_c, 0x18);
struct dOwPaneOwnerVtable_c {
    u8 mReserved00[0x5C];
    /* 0x5C */ gabi::be<u32> mFindPane;
};
WWHD_OFFSET(dOwPaneOwnerVtable_c, mFindPane, 0x5C);
WWHD_SIZE(dOwPaneOwnerVtable_c, 0x60);
struct dOwLayoutVtable_c {
    u8 mReserved00[0x14];
    /* 0x14 */ gabi::be<u32> mLoad;
};
WWHD_OFFSET(dOwLayoutVtable_c, mLoad, 0x14);
WWHD_SIZE(dOwLayoutVtable_c, 0x18);
struct dOwScreenVtable_c {
    u8 mReserved00[0x7C];
    /* 0x7C */ gabi::be<u32> mLoadLayout;
    u8 mReserved80[0x4];
    /* 0x84 */ gabi::be<u32> mExecuteLayout;
    u8 mReserved88[0x4];
    /* 0x8C */ gabi::be<u32> mAnimateLayout;
    u8 mReserved90[0xC];
    /* 0x9C */ gabi::be<u32> mDrawLayout;
    u8 mReservedA0[0xC];
    /* 0xAC */ gabi::be<u32> mBeforeExecute;
    u8 mReservedB0[0x4];
    /* 0xB4 */ gabi::be<u32> mBeforeMove;
};
WWHD_OFFSET(dOwScreenVtable_c, mLoadLayout, 0x7C);
WWHD_OFFSET(dOwScreenVtable_c, mExecuteLayout, 0x84);
WWHD_OFFSET(dOwScreenVtable_c, mAnimateLayout, 0x8C);
WWHD_OFFSET(dOwScreenVtable_c, mDrawLayout, 0x9C);
WWHD_OFFSET(dOwScreenVtable_c, mBeforeExecute, 0xAC);
WWHD_OFFSET(dOwScreenVtable_c, mBeforeMove, 0xB4);
WWHD_SIZE(dOwScreenVtable_c, 0xB8);
struct dOwStateInterface_c {
    gabi::be<u32> mVtable;
};
WWHD_SIZE(dOwStateInterface_c, 4);

#endif
