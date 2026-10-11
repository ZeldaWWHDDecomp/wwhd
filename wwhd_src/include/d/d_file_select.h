/* dFile_select_c: save-data selection.
 * HD: replaces the GameCube J2D screen with a 0x2D8 layout and state
 * controller. Transition, pane and state views below describe the verified HD
 * members. */
#pragma once
#include "gabi.h"
#include "wwhd.h"
namespace FileSelectHD {
struct Vector {
    /* 0x000 */ be<f32> x;
    /* 0x004 */ be<f32> y;
    /* 0x008 */ be<f32> z;
};
WWHD_SIZE(Vector, 0xC);
WWHD_OFFSET(Vector, x, 0x0);
WWHD_OFFSET(Vector, y, 0x4);
WWHD_OFFSET(Vector, z, 0x8);
struct Transition {
    void initialize(u32 target, u32 start, u32 end, u32 a, u32 b, u32 c, f32 speed);
    void clear();
    u32 move();
    /* 0x000 */ Vector mPosition;
    /* 0x00C */ Vector mTarget;
    /* 0x018 */ be<f32> mSpeed;
    /* 0x01C */ be<f32> mCurrentSpeed;
    /* 0x020 */ be<u16> mAlpha;
    /* 0x022 */ be<u16> mTargetAlpha;
    /* 0x024 */ be<u16> mAlphaStep;
    u8 mPadding38[2];
    /* 0x028 */ be<u32> mDelay;
    /* 0x02C */ be<u32> mStage;
};
WWHD_SIZE(Transition, 0x30);
WWHD_OFFSET(Transition, mPosition, 0x0);
WWHD_OFFSET(Transition, mTarget, 0xC);
WWHD_OFFSET(Transition, mSpeed, 0x18);
WWHD_OFFSET(Transition, mCurrentSpeed, 0x1C);
WWHD_OFFSET(Transition, mAlpha, 0x20);
WWHD_OFFSET(Transition, mTargetAlpha, 0x22);
WWHD_OFFSET(Transition, mAlphaStep, 0x24);
WWHD_OFFSET(Transition, mDelay, 0x28);
WWHD_OFFSET(Transition, mStage, 0x2C);
struct Motion {
    void setPaneOrigins(u32 i, u32 pos);
    void setTransition(u32 index, u32 from, u32 to, u32 alphaStart, u32 alphaEnd, u32 delay,
                       u32 frames);
    void initialize();
    u8 mPadding0[12];
    /* 0x00C */ Vector mFirstChoice;
    /* 0x018 */ Vector mSecondChoice;
    /* 0x024 */ Vector mOrigin[4];
    /* 0x054 */ Vector mRight[4];
    /* 0x084 */ Vector mLeft[4];
    /* 0x0B4 */ Transition mTransition[4];
};
WWHD_SIZE(Motion, 0x174);
WWHD_OFFSET(Motion, mFirstChoice, 0xC);
WWHD_OFFSET(Motion, mSecondChoice, 0x18);
WWHD_OFFSET(Motion, mOrigin, 0x24);
WWHD_OFFSET(Motion, mRight, 0x54);
WWHD_OFFSET(Motion, mLeft, 0x84);
WWHD_OFFSET(Motion, mTransition, 0xB4);
struct Callback {
    /* 0x000 */ be<s16> mThisAdjust;
    /* 0x002 */ be<s16> mVirtualSlot;
    union {
        /* 0x004 */ be<u32> mTarget;
        struct {
            be<s16> mReserved;
            be<s16> mVtableOffset;
        };
    };
};
WWHD_OFFSET(Callback, mVtableOffset, 6);
WWHD_SIZE(Callback, 0x8);
WWHD_OFFSET(Callback, mThisAdjust, 0x0);
WWHD_OFFSET(Callback, mVirtualSlot, 0x2);
WWHD_OFFSET(Callback, mTarget, 0x4);
struct State {
    /* 0x000 */ be<u32> mId;
    /* 0x004 */ be<u32> mpName;
    /* 0x008 */ be<u32> mVtable;
    /* 0x00C */ Callback mInitialize;
    /* 0x014 */ Callback mExecute;
    /* 0x01C */ Callback mFinalize;
    /* 0x024 */ Callback mCheck;
};
WWHD_SIZE(State, 0x2C);
WWHD_OFFSET(State, mId, 0x0);
WWHD_OFFSET(State, mpName, 0x4);
WWHD_OFFSET(State, mVtable, 0x8);
WWHD_OFFSET(State, mInitialize, 0xC);
WWHD_OFFSET(State, mExecute, 0x14);
WWHD_OFFSET(State, mFinalize, 0x1C);
WWHD_OFFSET(State, mCheck, 0x24);
struct SafeString {
    /* 0x000 */ be<u32> mpText;
    /* 0x004 */ be<u32> mVtable;
};
WWHD_SIZE(SafeString, 0x8);
WWHD_OFFSET(SafeString, mpText, 0x0);
WWHD_OFFSET(SafeString, mVtable, 0x4);
struct Pane {
    u8 mPadding0[4];
    /* 0x004 */ be<u32> mVtable;
    u8 mPadding8[16];
    /* 0x018 */ be<u32> mpStateProcessor;
    /* 0x01C */ be<f32> mX;
    /* 0x020 */ be<f32> mY;
    /* 0x024 */ be<f32> mZ;
    u8 mPadding40[28];
    /* 0x044 */ be<u8> mFlags;
    /* 0x045 */ be<u8> mAlpha;
    u8 mPadding70[10];
    /* 0x050 */ be<u32> mId;
    u8 mPadding84[36];
    /* 0x078 */ be<u32> mState;
};
WWHD_SIZE(Pane, 0x7C);
WWHD_OFFSET(Pane, mVtable, 0x4);
WWHD_OFFSET(Pane, mpStateProcessor, 0x18);
WWHD_OFFSET(Pane, mX, 0x1C);
WWHD_OFFSET(Pane, mY, 0x20);
WWHD_OFFSET(Pane, mZ, 0x24);
WWHD_OFFSET(Pane, mFlags, 0x44);
WWHD_OFFSET(Pane, mAlpha, 0x45);
WWHD_OFFSET(Pane, mId, 0x50);
WWHD_OFFSET(Pane, mState, 0x78);
struct NamePane {
    /* 0x000 */ Pane mPane;
    u8 mPadding124[134];
    /* 0x102 */ be<u8> mBusy;
    u8 mPadding259[1];
};
WWHD_SIZE(NamePane, 0x104);
WWHD_OFFSET(NamePane, mPane, 0x0);
WWHD_OFFSET(NamePane, mBusy, 0x102);
struct Layout {
    u8 mPadding0[4];
    /* 0x004 */ be<u32> mpData;
    u8 mPadding8[204];
    /* 0x0D4 */ be<u32> mpAnimation;
    u8 mPadding216[8];
    /* 0x0E0 */ be<u32> mVtable;
    u8 mPadding228[44];
};
WWHD_SIZE(Layout, 0x110);
WWHD_OFFSET(Layout, mpData, 0x4);
WWHD_OFFSET(Layout, mpAnimation, 0xD4);
WWHD_OFFSET(Layout, mVtable, 0xE0);
struct LayoutData {
    u8 mPadding0[12];
    /* 0x00C */ be<u32> mpRootPane;
};
WWHD_SIZE(LayoutData, 0x10);
WWHD_OFFSET(LayoutData, mpRootPane, 0xC);
struct RootPane {
    u8 mPadding0[8];
    /* 0x008 */ be<u32> mVtable;
};
WWHD_SIZE(RootPane, 0xC);
WWHD_OFFSET(RootPane, mVtable, 0x8);
struct SlotData {
    u8 mPadding0[1];
    /* 0x001 */ be<u8> mMaxLife;
    /* 0x002 */ be<u16> mLife;
    u8 mPadding4[30];
    /* 0x022 */ be<u8> mTriforce;
    /* 0x023 */ be<u8> mSymbol;
    /* 0x024 */ be<u16> mStatus;
    /* 0x026 */ be<u8> mFlag;
    u8 mPadding39[1];
};
WWHD_SIZE(SlotData, 0x28);
WWHD_OFFSET(SlotData, mMaxLife, 0x1);
WWHD_OFFSET(SlotData, mLife, 0x2);
WWHD_OFFSET(SlotData, mTriforce, 0x22);
WWHD_OFFSET(SlotData, mSymbol, 0x23);
WWHD_OFFSET(SlotData, mStatus, 0x24);
WWHD_OFFSET(SlotData, mFlag, 0x26);
struct SavePrefix {
    u8 mPadding0[32];
    /* 0x020 */ be<u16> mMaxLife;
    /* 0x022 */ be<u16> mLife;
};
WWHD_SIZE(SavePrefix, 0x24);
WWHD_OFFSET(SavePrefix, mMaxLife, 0x20);
WWHD_OFFSET(SavePrefix, mLife, 0x22);
struct Service {
    u8 mPadding0[28];
    /* 0x01C */ be<u32> mCommand;
};
WWHD_SIZE(Service, 0x20);
WWHD_OFFSET(Service, mCommand, 0x1C);
struct AllocationManager {
    u8 mPadding0[16];
    /* 0x010 */ be<u32> mpHeap;
};
WWHD_SIZE(AllocationManager, 0x14);
WWHD_OFFSET(AllocationManager, mpHeap, 0x10);
struct Pad {
    u8 mPadding0[24];
    /* 0x018 */ be<u32> mTrigger;
    u8 mPadding28[4];
    /* 0x020 */ be<u32> mRepeat;
};
WWHD_SIZE(Pad, 0x24);
WWHD_OFFSET(Pad, mTrigger, 0x18);
WWHD_OFFSET(Pad, mRepeat, 0x20);
enum PaneNo {
    PANE_START = 0,
    PANE_DELETE = 1,
    PANE_COPY = 2,
    PANE_BACK = 3,
    PANE_INFO = 4,
    PANE_CHOICE = 5,
    PANE_SLOT_FIRST = 6,
    PANE_NAME = 9,
    PANE_COUNT = 10
};
enum MotionMode {
    MOTION_ORIGIN,
    MOTION_RIGHT,
    MOTION_LEFT,
    MOTION_FIRST,
    MOTION_SECOND,
    MOTION_FIRST_OUT,
    MOTION_SECOND_OUT,
    MOTION_BUSY_ORIGIN,
    MOTION_BUSY_OUT,
    MOTION_COPY_ORIGIN,
    MOTION_COPY_OUT,
    MOTION_CURRENT,
    MOTION_NAME_OUT
};
enum Event {
    EVENT_SLOT_FIRST = 0x56,
    EVENT_SLOT_SECOND,
    EVENT_SLOT_THIRD,
    EVENT_BACK,
    EVENT_DELETE,
    EVENT_COPY,
    EVENT_ACCEPT
};
struct StateWithOwner {
    State mState;
    be<u32> mpNullState;
};
WWHD_SIZE(StateWithOwner, 0x30);
WWHD_OFFSET(StateWithOwner, mState, 0);
WWHD_OFFSET(StateWithOwner, mpNullState, 0x2C);
struct Statics {
    /* 0x000 */ be<f32> mNegativePi;
    /* 0x004 */ be<f32> mPositivePi;
    /* 0x008 */ be<u8> mHeaderByte;
    /* 0x009 */ be<u8> mArchiveByte;
    u8 mPadding10[2];
    /* 0x00C */ SafeString mScreenPartNames[6];
    /* 0x03C */ be<u32> mHeaderWords[4];
    /* 0x04C */ SafeString mAnimationRoots[4];
    /* 0x06C */ SafeString mAnimationNames[7];
    /* 0x0A4 */ SafeString mLabelNames[6];
    /* 0x0D4 */ SafeString mPaneNames[10];
    /* 0x124 */ SafeString mModuleName;
    /* 0x12C */ SafeString mTypeNames[8];
    /* 0x16C */ be<u32> mScreenNamesInitialized;
    /* 0x170 */ StateWithOwner StateID_InAll;
    /* 0x1A0 */ StateWithOwner StateID_OutAllEnd;
    /* 0x1D0 */ SafeString mLayoutName;
    /* 0x1D8 */ State StateID_Wait;
    /* 0x204 */ State StateID_SelectSaveData;
    /* 0x230 */ State StateID_WaitNameInput;
    /* 0x25C */ State StateID_ShowSaveData;
    /* 0x288 */ State StateID_CancelSaveData;
    /* 0x2B4 */ State StateID_SelectDownButton;
    /* 0x2E0 */ State StateID_DecideSaveData;
    /* 0x30C */ State StateID_OutAll;
    /* 0x338 */ State StateID_DecideDeleteYesNo;
    /* 0x364 */ State StateID_WaitDeleting;
    /* 0x390 */ State StateID_EndDelete;
    /* 0x3BC */ State StateID_SelectFileCopyTo;
    /* 0x3E8 */ State StateID_DecideCopyYesNo;
    /* 0x414 */ State StateID_WaitCopying;
    /* 0x440 */ State StateID_EndCopy;
    /* 0x46C */ State StateID_SaveEXYesNo;
    /* 0x498 */ State StateID_SavingEX;
    /* 0x4C4 */ State StateID_EndSaveEX;
};
WWHD_SIZE(Statics, 0x4F0);
WWHD_OFFSET(Statics, mNegativePi, 0x0);
WWHD_OFFSET(Statics, mPositivePi, 0x4);
WWHD_OFFSET(Statics, mHeaderByte, 0x8);
WWHD_OFFSET(Statics, mArchiveByte, 0x9);
WWHD_OFFSET(Statics, mScreenPartNames, 0xC);
WWHD_OFFSET(Statics, mHeaderWords, 0x3C);
WWHD_OFFSET(Statics, mAnimationRoots, 0x4C);
WWHD_OFFSET(Statics, mAnimationNames, 0x6C);
WWHD_OFFSET(Statics, mLabelNames, 0xA4);
WWHD_OFFSET(Statics, mPaneNames, 0xD4);
WWHD_OFFSET(Statics, mModuleName, 0x124);
WWHD_OFFSET(Statics, mTypeNames, 0x12C);
WWHD_OFFSET(Statics, mScreenNamesInitialized, 0x16C);
WWHD_OFFSET(Statics, StateID_InAll, 0x170);
WWHD_OFFSET(Statics, StateID_OutAllEnd, 0x1A0);
WWHD_OFFSET(Statics, mLayoutName, 0x1D0);
WWHD_OFFSET(Statics, StateID_Wait, 0x1D8);
WWHD_OFFSET(Statics, StateID_SelectSaveData, 0x204);
WWHD_OFFSET(Statics, StateID_WaitNameInput, 0x230);
WWHD_OFFSET(Statics, StateID_ShowSaveData, 0x25C);
WWHD_OFFSET(Statics, StateID_CancelSaveData, 0x288);
WWHD_OFFSET(Statics, StateID_SelectDownButton, 0x2B4);
WWHD_OFFSET(Statics, StateID_DecideSaveData, 0x2E0);
WWHD_OFFSET(Statics, StateID_OutAll, 0x30C);
WWHD_OFFSET(Statics, StateID_DecideDeleteYesNo, 0x338);
WWHD_OFFSET(Statics, StateID_WaitDeleting, 0x364);
WWHD_OFFSET(Statics, StateID_EndDelete, 0x390);
WWHD_OFFSET(Statics, StateID_SelectFileCopyTo, 0x3BC);
WWHD_OFFSET(Statics, StateID_DecideCopyYesNo, 0x3E8);
WWHD_OFFSET(Statics, StateID_WaitCopying, 0x414);
WWHD_OFFSET(Statics, StateID_EndCopy, 0x440);
WWHD_OFFSET(Statics, StateID_SaveEXYesNo, 0x46C);
WWHD_OFFSET(Statics, StateID_SavingEX, 0x498);
WWHD_OFFSET(Statics, StateID_EndSaveEX, 0x4C4);

// HD: code/table identities retained by the native state initializer.
constexpr u32 kdFile_select_c_initializeWait = 0x026CBF5C;
constexpr u32 kdFile_select_c_initializeSelectSaveData = 0x026CCFD8;
constexpr u32 kdFile_select_c_executeSelectSaveData = 0x026CD740;
constexpr u32 kdFile_select_c_executeWaitNameInput = 0x026CD898;
constexpr u32 kdFile_select_c_initializeShowSaveData = 0x026CDBFC;
constexpr u32 kdFile_select_c_executeShowSaveData = 0x026CE118;
constexpr u32 kdFile_select_c_initializeCancelSaveData = 0x026CE394;
constexpr u32 kdFile_select_c_executeCancelSaveData = 0x026CE620;
constexpr u32 kdFile_select_c_initializeSelectDownButton = 0x026CEA60;
constexpr u32 kdFile_select_c_executeSelectDownButton = 0x026CED0C;
constexpr u32 kdFile_select_c_executeDecideSaveData = 0x026CEF90;
constexpr u32 kdFile_select_c_initializeDecideDeleteYesNo = 0x026CF144;
constexpr u32 kdFile_select_c_executeDecideDeleteYesNo = 0x026CF264;
constexpr u32 kdFile_select_c_finalizeDecideDeleteYesNo = 0x026CF3EC;
constexpr u32 kdFile_select_c_initializeWaitDeleting = 0x026CF408;
constexpr u32 kdFile_select_c_executeWaitDeleting = 0x026CF4E0;
constexpr u32 kdFile_select_c_initializeEndDelete = 0x026CF588;
constexpr u32 kdFile_select_c_executeEndDelete = 0x026CF660;
constexpr u32 kdFile_select_c_initializeSelectFileCopyTo = 0x026CF9B4;
constexpr u32 kdFile_select_c_executeSelectFileCopyTo = 0x026CFFB4;
constexpr u32 kdFile_select_c_initializeDecideCopyYesNo = 0x026D0420;
constexpr u32 kdFile_select_c_executeDecideCopyYesNo = 0x026D0608;
constexpr u32 kdFile_select_c_finalizeDecideCopyYesNo = 0x026D0848;
constexpr u32 kdFile_select_c_initializeWaitCopying = 0x026D0864;
constexpr u32 kdFile_select_c_executeWaitCopying = 0x026D0978;
constexpr u32 kdFile_select_c_initializeEndCopy = 0x026D0A20;
constexpr u32 kdFile_select_c_executeEndCopy = 0x026D0D84;
constexpr u32 kdFile_select_c_initializeSaveEXYesNo = 0x026D0FBC;
constexpr u32 kdFile_select_c_executeSaveEXYesNo = 0x026D100C;
constexpr u32 kdFile_select_c_initializeSavingEX = 0x026D11AC;
constexpr u32 kdFile_select_c_executeSavingEX = 0x026D12B0;
constexpr u32 kinitializeEndSaveEX = 0x026D15A8;
constexpr u32 kdFile_select_c_executeEndSaveEX = 0x026D15B8;
constexpr u32 kexecuteWait = 0x026D3758;
constexpr u32 kfinalizeWait = 0x026D375C;
constexpr u32 kcheckWait = 0x026D3760;
constexpr u32 kfinalizeSelectSaveData = 0x026D376C;
constexpr u32 kcheckSelectSaveData = 0x026D3770;
constexpr u32 kinitializeWaitNameInput = 0x026D3778;
constexpr u32 kfinalizeWaitNameInput = 0x026D377C;
constexpr u32 kcheckWaitNameInput = 0x026D3780;
constexpr u32 kfinalizeShowSaveData = 0x026D3788;
constexpr u32 kcheckShowSaveData = 0x026D378C;
constexpr u32 kfinalizeCancelSaveData = 0x026D3794;
constexpr u32 kcheckCancelSaveData = 0x026D3798;
constexpr u32 kfinalizeSelectDownButton = 0x026D37A0;
constexpr u32 kcheckSelectDownButton = 0x026D37A4;
constexpr u32 kinitializeDecideSaveData = 0x026D37AC;
constexpr u32 kfinalizeDecideSaveData = 0x026D37B0;
constexpr u32 kcheckDecideSaveData = 0x026D37B4;
constexpr u32 kinitializeOutAll = 0x026D37C0;
constexpr u32 kexecuteOutAll = 0x026D37C4;
constexpr u32 kfinalizeOutAll = 0x026D37C8;
constexpr u32 kcheckOutAll = 0x026D37CC;
constexpr u32 kcheckDecideDeleteYesNo = 0x026D37D4;
constexpr u32 kfinalizeWaitDeleting = 0x026D37DC;
constexpr u32 kcheckWaitDeleting = 0x026D37E0;
constexpr u32 kfinalizeEndDelete = 0x026D37E8;
constexpr u32 kcheckEndDelete = 0x026D37EC;
constexpr u32 kfinalizeSelectFileCopyTo = 0x026D37F4;
constexpr u32 kcheckSelectFileCopyTo = 0x026D37F8;
constexpr u32 kcheckDecideCopyYesNo = 0x026D3800;
constexpr u32 kfinalizeWaitCopying = 0x026D3808;
constexpr u32 kcheckWaitCopying = 0x026D380C;
constexpr u32 kfinalizeEndCopy = 0x026D3814;
constexpr u32 kcheckEndCopy = 0x026D3818;
constexpr u32 kfinalizeSaveEXYesNo = 0x026D3820;
constexpr u32 kcheckSaveEXYesNo = 0x026D3824;
constexpr u32 kfinalizeSavingEX = 0x026D382C;
constexpr u32 kcheckSavingEX = 0x026D3830;
constexpr u32 kfinalizeEndSaveEX = 0x026D3838;
constexpr u32 kcheckEndSaveEX = 0x026D383C;
constexpr u32 kSafeStringVtable = 0x1010205C;
constexpr u32 kStateVtable = 0x10102118;
constexpr u32 kVirtualStateVtable = 0x10102160;
constexpr u32 kFileSelectVtable = 0x101026C0;
constexpr u32 kStateInterfaceVtable = 0x101027B8;
constexpr u32 kDrawInterfaceVtable = 0x101027C8;
constexpr u32 kNullState = 0x101FF32C;
struct Vtable {
    u8 mPadding0[20];
    /* 0x014 */ be<u32> mGetId;
    u8 mPadding24[4];
    /* 0x01C */ be<u32> mInitialize;
    u8 mPadding32[60];
    /* 0x05C */ be<u32> mFindPane;
    u8 mPadding96[12];
    /* 0x06C */ be<u32> mDraw;
    u8 mPadding112[12];
    /* 0x07C */ be<u32> mCreateLayout;
    u8 mPadding128[4];
    /* 0x084 */ be<u32> mDeleteLayout;
    u8 mPadding136[4];
    /* 0x08C */ be<u32> mUpdateLayout;
    u8 mPadding144[20];
    /* 0x0A4 */ be<u32> mCreatePanes;
    u8 mPadding168[4];
    /* 0x0AC */ be<u32> mDeletePanes;
    u8 mPadding176[4];
    /* 0x0B4 */ be<u32> mUpdatePanes;
    u8 mPadding184[20];
    /* 0x0CC */ be<u32> mEnable;
    u8 mPadding208[292];
    /* 0x1F4 */ be<u32> mSetButtonState;
    u8 mPadding504[68];
    /* 0x23C */ be<u32> mReset;
    u8 mPadding576[4];
    /* 0x244 */ be<u32> mSetDisabled;
};
WWHD_SIZE(Vtable, 0x248);
WWHD_OFFSET(Vtable, mGetId, 0x14);
WWHD_OFFSET(Vtable, mInitialize, 0x1C);
WWHD_OFFSET(Vtable, mFindPane, 0x5C);
WWHD_OFFSET(Vtable, mDraw, 0x6C);
WWHD_OFFSET(Vtable, mCreateLayout, 0x7C);
WWHD_OFFSET(Vtable, mDeleteLayout, 0x84);
WWHD_OFFSET(Vtable, mUpdateLayout, 0x8C);
WWHD_OFFSET(Vtable, mCreatePanes, 0xA4);
WWHD_OFFSET(Vtable, mDeletePanes, 0xAC);
WWHD_OFFSET(Vtable, mUpdatePanes, 0xB4);
WWHD_OFFSET(Vtable, mEnable, 0xCC);
WWHD_OFFSET(Vtable, mSetButtonState, 0x1F4);
WWHD_OFFSET(Vtable, mReset, 0x23C);
WWHD_OFFSET(Vtable, mSetDisabled, 0x244);
struct StringVtable {
    u8 mPadding0[20];
    /* 0x014 */ be<u32> mAssureTermination;
};
WWHD_SIZE(StringVtable, 0x18);
WWHD_OFFSET(StringVtable, mAssureTermination, 0x14);
struct LayoutVtable {
    u8 mPadding0[20];
    /* 0x014 */ be<u32> mCreate;
};
WWHD_SIZE(LayoutVtable, 0x18);
WWHD_OFFSET(LayoutVtable, mCreate, 0x14);
struct EventRecord {
    /* 0x000 */ be<u32> mKind;
    /* 0x004 */ be<u32> mId;
    /* 0x008 */ be<u32> mPayload[8];
};
WWHD_SIZE(EventRecord, 0x28);
WWHD_OFFSET(EventRecord, mKind, 0x0);
WWHD_OFFSET(EventRecord, mId, 0x4);
WWHD_OFFSET(EventRecord, mPayload, 0x8);
struct Pair {
    /* 0x000 */ be<f32> x;
    /* 0x004 */ be<f32> y;
};
WWHD_SIZE(Pair, 0x8);
WWHD_OFFSET(Pair, x, 0x0);
WWHD_OFFSET(Pair, y, 0x4);
struct FrameLinkage {
    /* 0x000 */ be<u32> mBackChain;
    /* 0x004 */ be<u32> mSavedLR;
};
WWHD_SIZE(FrameLinkage, 0x8);
WWHD_OFFSET(FrameLinkage, mBackChain, 0x0);
WWHD_OFFSET(FrameLinkage, mSavedLR, 0x4);
struct CreateFrame {
    /* 0x000 */ be<u32> mBackChain;
    u8 mPadding4[4];
    /* 0x008 */ SafeString mName;
    /* 0x010 */ Pair mSecondSumReload;
    u8 mPadding24[4];
    /* 0x01C */ Pair mFirstSumReload;
    u8 mPadding36[4];
    /* 0x028 */ Pair mFirstDifference;
    u8 mPadding48[4];
    /* 0x034 */ Pair mFirstSum;
    u8 mPadding60[4];
    /* 0x040 */ Pair mFirstResult;
    u8 mPadding72[4];
    /* 0x04C */ Pair mSecondDifference;
    u8 mPadding84[4];
    /* 0x058 */ Pair mSecondSum;
    u8 mPadding96[4];
    /* 0x064 */ Pair mSecondResult;
    u8 mPadding108[8];
    /* 0x074 */ be<u32> mSavedGpr[7];
};
WWHD_SIZE(CreateFrame, 0x90);
WWHD_OFFSET(CreateFrame, mBackChain, 0x0);
WWHD_OFFSET(CreateFrame, mName, 0x8);
WWHD_OFFSET(CreateFrame, mSecondSumReload, 0x10);
WWHD_OFFSET(CreateFrame, mFirstSumReload, 0x1C);
WWHD_OFFSET(CreateFrame, mFirstDifference, 0x28);
WWHD_OFFSET(CreateFrame, mFirstSum, 0x34);
WWHD_OFFSET(CreateFrame, mFirstResult, 0x40);
WWHD_OFFSET(CreateFrame, mSecondDifference, 0x4C);
WWHD_OFFSET(CreateFrame, mSecondSum, 0x58);
WWHD_OFFSET(CreateFrame, mSecondResult, 0x64);
WWHD_OFFSET(CreateFrame, mSavedGpr, 0x74);
struct CreatePaneFrame {
    /* 0x000 */ be<u32> mBackChain;
    u8 mPadding4[8];
    /* 0x00C */ be<u32> mSavedGpr[11];
};
WWHD_SIZE(CreatePaneFrame, 0x38);
WWHD_OFFSET(CreatePaneFrame, mBackChain, 0x0);
WWHD_OFFSET(CreatePaneFrame, mSavedGpr, 0xC);
struct StateProcessor {
    /* 0x000 */ be<u32> mId;
    u8 mPadding4[40];
    /* 0x02C */ be<u32> mpDescriptor;
};
WWHD_SIZE(StateProcessor, 0x30);
WWHD_OFFSET(StateProcessor, mId, 0x0);
WWHD_OFFSET(StateProcessor, mpDescriptor, 0x2C);
struct VirtualEntry {
    /* 0x000 */ be<u32> mThisAdjust;
    /* 0x004 */ be<u32> mTarget;
};
WWHD_SIZE(VirtualEntry, 0x8);
WWHD_OFFSET(VirtualEntry, mThisAdjust, 0x0);
WWHD_OFFSET(VirtualEntry, mTarget, 0x4);
struct PaneInfo {
    u8 mPadding0[8];
    /* 0x008 */ SafeString mType;
    u8 mPadding16[4];
    /* 0x014 */ SafeString mName;
};
WWHD_SIZE(PaneInfo, 0x1C);
WWHD_OFFSET(PaneInfo, mType, 0x8);
WWHD_OFFSET(PaneInfo, mName, 0x14);
struct SaveView {
    u8 mPadding0[32];
    /* 0x020 */ be<u16> mMaxLife;
    /* 0x022 */ be<u16> mLife;
    u8 mPadding36[176];
    /* 0x0D4 */ be<u8> mCollect;
    u8 mPadding213[4587];
    /* 0x12C0 */ be<u8> mSlots;
    u8 mPadding4801[3];
};
WWHD_SIZE(SaveView, 0x12C4);
WWHD_OFFSET(SaveView, mMaxLife, 0x20);
WWHD_OFFSET(SaveView, mLife, 0x22);
WWHD_OFFSET(SaveView, mCollect, 0xD4);
WWHD_OFFSET(SaveView, mSlots, 0x12C0);
enum MessageMode {
    MESSAGE_SELECT = 1,
    MESSAGE_DELETE_CONFIRM,
    MESSAGE_DELETING,
    MESSAGE_DELETE_END,
    MESSAGE_COPY_SELECT,
    MESSAGE_COPY_CONFIRM,
    MESSAGE_COPYING,
    MESSAGE_COPY_END,
    MESSAGE_SAVE_SELECT,
    MESSAGE_SAVE_CONFIRM,
    MESSAGE_SAVING,
    MESSAGE_SAVE_END
};
enum PaneFlags { PANE_VISIBLE = 1, PANE_POSITION_DIRTY = 0x10 };
enum Sound {
    SOUND_TOUCH = 0x830,
    SOUND_SELECT = 0x831,
    SOUND_CANCEL = 0x832,
    SOUND_SAVE_END = 0x84A,
    SOUND_CONFIRM = 0x89A,
    SOUND_COMPLETE = 0x89B
};
} // namespace FileSelectHD
struct dFile_select_c {
    void initializeWait();
    void growMove();
    u32 slotIdle(u32 slot);
    u32 selectDone();
    void executeWaitNameInput();
    u32 slotFlag(u32 slot);
    u32 buttonOne();
    void initializeSelectDownButton();
    void executeDecideSaveData();
    void outInit();
    void outMove();
    void initializeDecideDeleteYesNo();
    u32 copySound();
    u32 finalizeDecideDeleteYesNo();
    void initializeSelectFileCopyTo();
    void initializeDecideCopyYesNo();
    u32 finalizeDecideCopyYesNo();
    u32 backSound();
    void initializeSaveEXYesNo();
    void executeEndSaveEX();
    void _delete();
    u32 _move();
    void reset();
    u32 slotStatus();
    u32 deleteSound();
    void refreshSlots();
    void initializeSelectSaveData();
    u32 takeEvent();
    u32 selectCancel();
    void enableSlots();
    void enableButtons();
    void initializeShowSaveData();
    void deleteButtons();
    void linkButtons();
    void enableDelete();
    void executeWaitDeleting();
    void executeWaitCopying();
    void enableCopySlots();
    void findParts();
    void showSlots();
    void initializeWaitDeleting();
    void initializeEndDelete();
    void initializeWaitCopying();
    void initializeSavingEX();
    void choiceButtons();
    void executeCancelSaveData();
    void animateSlotsIn();
    void enableSelection();
    u32 createLayout(u32 resource, u32 archive, u32 options, u32 a, u32 b);
    void clearNavigation();
    u32 updateTransitions();
    void growInit();
    void linkSlots();
    void executeSelectSaveData();
    void loadSlotData(u32 slot);
    void selectionTransition();
    void linkButtonsIn();
    void executeShowSaveData();
    void executeSelectDownButton();
    void initializeCancelSaveData();
    void dataLabels();
    void executeDecideDeleteYesNo();
    void executeEndDelete();
    void copyChoicesInit();
    void linkCopyChoices();
    void executeSelectFileCopyTo();
    void copyCancelInit();
    void executeDecideCopyYesNo();
    void deleteSlotsIn();
    void executeSaveEXYesNo();
    void executeEndCopy();
    void executeSavingEX();
    void findScreenParts();
    void _draw(u32 draw);
    u32 _create(u32 resource, u32 archive, u32 options);
    void initializeEndCopy();
    u32 createPane(u32 info);
    u32 cancelCopy();
    void copyConfirmInit();
    u8 mPadding0[4];
    /* 0x004 */ be<u32> mVtable;
    u8 mPadding8[16];
    /* 0x018 */ be<u32> mpStateProcessor;
    u8 mPadding28[4];
    /* 0x020 */ be<u32> mpState;
    /* 0x024 */ be<u32> mpPreviousState;
    u8 mPadding40[4];
    /* 0x02C */ be<u32> mStateInterfaceVtable;
    /* 0x030 */ be<u32> mDrawInterfaceVtable;
    /* 0x034 */ be<u32> mpHeap;
    u8 mPadding56[12];
    /* 0x044 */ be<u32> mpLayout;
    /* 0x048 */ be<u32> mPaneCount;
    /* 0x04C */ be<u32> mpPanes;
    /* 0x050 */ FileSelectHD::Motion mMotion;
    /* 0x1C4 */ be<u32> mFirstCopySlot;
    /* 0x1C8 */ be<u32> mSecondCopySlot;
    /* 0x1CC */ be<u8> mClose;
    /* 0x1CD */ be<u8> selectEnd;
    u8 mPadding462[2];
    /* 0x1D0 */ be<u32> mOriginalPane;
    /* 0x1D4 */ be<u32> mSelectedPane;
    /* 0x1D8 */ be<u32> mMenuPane;
    /* 0x1DC */ be<u8> mCancelSoundPlayed;
    u8 mPadding477[3];
    /* 0x1E0 */ FileSelectHD::Vector mSlotPosition[3];
    /* 0x204 */ FileSelectHD::Vector mSlotOffset[3];
    /* 0x228 */ FileSelectHD::Vector mCopyPosition[3];
    /* 0x24C */ be<u8> mSlotFlags[6];
    u8 mPadding594[2];
    /* 0x254 */ FileSelectHD::Vector mFirstChoiceOffset;
    /* 0x260 */ FileSelectHD::Vector mSecondChoiceOffset;
    /* 0x26C */ be<u32> mCompletionTimer;
    /* 0x270 */ be<u32> mStage;
    /* 0x274 */ be<u32> mDelayTimer;
    /* 0x278 */ be<u8> mUseType;
    u8 mPadding633[3];
    /* 0x27C */ be<u32> mMessage;
    /* 0x280 */ be<u32> selectNum;
    /* 0x284 */ be<u8> mInputDisabled;
    u8 mPadding645[7];
    /* 0x28C */ be<u32> mpScreenParts[6];
    /* 0x2A4 */ be<u32> mpExtraPart;
    /* 0x2A8 */ be<u32> mpParts[10];
    u8 mPadding720[4];
    /* 0x2D4 */ be<u32> mpService;
};
WWHD_SIZE(dFile_select_c, 0x2D8);
WWHD_OFFSET(dFile_select_c, mVtable, 0x4);
WWHD_OFFSET(dFile_select_c, mpStateProcessor, 0x18);
WWHD_OFFSET(dFile_select_c, mpState, 0x20);
WWHD_OFFSET(dFile_select_c, mpPreviousState, 0x24);
WWHD_OFFSET(dFile_select_c, mStateInterfaceVtable, 0x2C);
WWHD_OFFSET(dFile_select_c, mDrawInterfaceVtable, 0x30);
WWHD_OFFSET(dFile_select_c, mpHeap, 0x34);
WWHD_OFFSET(dFile_select_c, mpLayout, 0x44);
WWHD_OFFSET(dFile_select_c, mPaneCount, 0x48);
WWHD_OFFSET(dFile_select_c, mpPanes, 0x4C);
WWHD_OFFSET(dFile_select_c, mMotion, 0x50);
WWHD_OFFSET(dFile_select_c, mFirstCopySlot, 0x1C4);
WWHD_OFFSET(dFile_select_c, mSecondCopySlot, 0x1C8);
WWHD_OFFSET(dFile_select_c, mClose, 0x1CC);
WWHD_OFFSET(dFile_select_c, selectEnd, 0x1CD);
WWHD_OFFSET(dFile_select_c, mOriginalPane, 0x1D0);
WWHD_OFFSET(dFile_select_c, mSelectedPane, 0x1D4);
WWHD_OFFSET(dFile_select_c, mMenuPane, 0x1D8);
WWHD_OFFSET(dFile_select_c, mCancelSoundPlayed, 0x1DC);
WWHD_OFFSET(dFile_select_c, mSlotPosition, 0x1E0);
WWHD_OFFSET(dFile_select_c, mSlotOffset, 0x204);
WWHD_OFFSET(dFile_select_c, mCopyPosition, 0x228);
WWHD_OFFSET(dFile_select_c, mSlotFlags, 0x24C);
WWHD_OFFSET(dFile_select_c, mFirstChoiceOffset, 0x254);
WWHD_OFFSET(dFile_select_c, mSecondChoiceOffset, 0x260);
WWHD_OFFSET(dFile_select_c, mCompletionTimer, 0x26C);
WWHD_OFFSET(dFile_select_c, mStage, 0x270);
WWHD_OFFSET(dFile_select_c, mDelayTimer, 0x274);
WWHD_OFFSET(dFile_select_c, mUseType, 0x278);
WWHD_OFFSET(dFile_select_c, mMessage, 0x27C);
WWHD_OFFSET(dFile_select_c, selectNum, 0x280);
WWHD_OFFSET(dFile_select_c, mInputDisabled, 0x284);
WWHD_OFFSET(dFile_select_c, mpScreenParts, 0x28C);
WWHD_OFFSET(dFile_select_c, mpExtraPart, 0x2A4);
WWHD_OFFSET(dFile_select_c, mpParts, 0x2A8);
WWHD_OFFSET(dFile_select_c, mpService, 0x2D4);

// HD: private bindings retain the verified register widths at each call site.
void fileSelectHD_eventConstruct(u32, u32, u32, u32);
void fileSelectHD_eventQueuePop(u32, u32);
void fileSelectHD_eventDestruct(u32);
void fileSelectHD_eventQueuePush(u32, u32);
void fileSelectHD_bindAnimation(u32, u32, u32, u32);
u32 fileSelectHD_playAnimation(u32, u32, u32, f32);
void fileSelectHD_startAnimation(u32, u32, u32);
void fileSelectHD_stopAnimation(u32, u32, u32);
u32 fileSelectHD_isAnimationFinished(u32, u32);
u32 fileSelectHD_updateState(u32);
void fileSelectHD_changeState(u32, u32);
u32 fileSelectHD_getState(u32);
f32 fileSelectHD_addCalcPos(u32, u32, f32, f32, f32);
u32 fileSelectHD_chaseAlpha(u32, u32, u32);
u32 fileSelectHD_getInputManager();
void fileSelectHD_setInputEnabled(u32, u32);
void fileSelectHD_onTriforce(u32, u32);
void fileSelectHD_offTriforce(u32, u32);
void fileSelectHD_onSymbol(u32, u32);
void fileSelectHD_offSymbol(u32, u32);
void fileSelectHD_startSound(u32);
u32 fileSelectHD_setFocus(u32, u32);
void fileSelectHD_difficultySelectHero(u32);
void fileSelectHD_difficultySelectNormal(u32);
void fileSelectHD_savePaneReload(u32);
void fileSelectHD_savePaneUpdate(u32);
u32 fileSelectHD_savePaneGetStatus(u32);
void fileSelectHD_savePaneRefresh(u32);
void fileSelectHD_heartRefresh(u32);
void fileSelectHD_collectionRefresh(u32);
void fileSelectHD_fileSelectResetFocus(u32);
void fileSelectHD_dFile_select_c_clearNavigation(u32);
void fileSelectHD_Transition_initialize(u32, u32, u32, u32, u32, u32, u32, f32);
void fileSelectHD_Motion_setTransition(u32, u32, u32, u32, u32, u32, u32, u32);
u32 fileSelectHD_Transition_move(u32);
u32 fileSelectHD_dFile_select_c_updateTransitions(u32);
u32 fileSelectHD_dFile_select_c_slotIdle(u32, u32);
void fileSelectHD_dFile_select_c_refreshSlots(u32);
u32 fileSelectHD_dFile_select_c_takeEvent(u32);
void fileSelectHD_dFile_select_c_linkSlots(u32);
u32 fileSelectHD_dFile_select_c_selectCancel(u32);
void fileSelectHD_dFile_select_c_selectionTransition(u32);
u32 fileSelectHD_dFile_select_c_selectDone(u32);
void fileSelectHD_dFile_select_c_enableSlots(u32);
void fileSelectHD_dFile_select_c_loadSlotData(u32, u32);
u32 fileSelectHD_dFile_select_c_slotFlag(u32, u32);
void fileSelectHD_dFile_select_c_enableButtons(u32);
void fileSelectHD_dFile_select_c_dataLabels(u32);
void fileSelectHD_dFile_select_c_deleteButtons(u32);
void fileSelectHD_dFile_select_c_choiceButtons(u32);
u32 fileSelectHD_dFile_select_c_buttonOne(u32);
void fileSelectHD_dFile_select_c_linkButtonsIn(u32);
void fileSelectHD_dFile_select_c_animateSlotsIn(u32);
void fileSelectHD_dFile_select_c_enableSelection(u32);
void fileSelectHD_dFile_select_c_linkButtons(u32);
void fileSelectHD_dFile_select_c_copySound(u32);
void fileSelectHD_dFile_select_c_enableDelete(u32);
void fileSelectHD_dFile_select_c_copyChoicesInit(u32);
void fileSelectHD_dFile_select_c_linkCopyChoices(u32);
u32 fileSelectHD_dFile_select_c_cancelCopy(u32);
void fileSelectHD_dFile_select_c_enableCopySlots(u32);
void fileSelectHD_dFile_select_c_copyConfirmInit(u32);
void fileSelectHD_dFile_select_c_copyCancelInit(u32);
void fileSelectHD_dFile_select_c_deleteSlotsIn(u32);
void fileSelectHD_dFile_select_c_backSound(u32);
void fileSelectHD_dFile_select_c_findScreenParts(u32);
void fileSelectHD_dFile_select_c_findParts(u32);
void fileSelectHD_Transition_clear(u32);
void fileSelectHD_Motion_initialize(u32);
void fileSelectHD_Motion_setPaneOrigins(u32, u32, u32);
void fileSelectHD_paneSelect(u32);
void fileSelectHD_paneResetSelection(u32);
void fileSelectHD_paneSetConfirmSound(u32, u32, u32);
u32 fileSelectHD_paneGetId(u32);
void fileSelectHD_paneSetNavigation(u32, u32, u32, u32, u32, u32, u32, u32, u32);
void fileSelectHD_paneSetText(u32, u32, u32);
u32 fileSelectHD_eventIsValid(u32);
u32 fileSelectHD_eventQueueHasCancel(u32);
void fileSelectHD_paneSetTouchSound(u32, u32, u32);
void fileSelectHD_paneSetSelectionSound(u32, u32, u32);
void fileSelectHD_baseDestruct(u32, u32);
void fileSelectHD_baseConstruct(u32);
void fileSelectHD_setModuleName(u32, u32, u32);
void fileSelectHD_baseCalc(u32);
u32 fileSelectHD_baseCreatePane(u32, u32, u32);
u32 fileSelectHD_layoutConstruct(u32);
u32 fileSelectHD_slotGetHeroMode(u32);
void fileSelectHD_slotSetHeroMode(u32, u32);
void fileSelectHD_copySlot(u32, u32, u32);
void fileSelectHD_serviceRefresh(u32);
u32 fileSelectHD_serviceGetSlotData(u32, u32);
u32 fileSelectHD_operatorNew(u32);
void fileSelectHD_operatorDelete(u32);
u32 fileSelectHD_heapNew(u32, u32, u32);
void fileSelectHD_constructArchiveByte(u32);
void fileSelectHD_constructHeaderByte(u32);
void fileSelectHD_registerGlobalObject(u32);
f64 fileSelectHD_sqrt(f64);

void fileSelectHD_deleteSlot(u32, u32);
void fileSelectHD_saveSlot(u32, u32);

u32 fileSelectHD_savePaneConstruct(u32, u32, u32);
u32 fileSelectHD_collectionPaneConstruct(u32);
u32 fileSelectHD_namePaneConstruct(u32);
u32 fileSelectHD_difficultyPaneConstruct(u32, u32);
u32 fileSelectHD_savePaneCreate(u32, u32, u32, u32);
u32 fileSelectHD_collectionPaneCreate(u32, u32, u32, u32);
u32 fileSelectHD_namePaneCreate(u32, u32, u32, u32);
u32 fileSelectHD_difficultyPaneCreate(u32, u32, u32, u32);
u32 fileSelectHD_saveGetSlot(u32, u32);
