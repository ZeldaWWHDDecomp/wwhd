/* HD staff-roll scene and bubble groups. Private binary-derived layouts. */
#pragma once
#include "bindings.h"
#include "d/hd_state_task.h"
#include "d/hd_scene_title.h"
#include "d/hd_scene_save_end.h"
#include "d/hd_screen_staff_roll_bubble.h"
#include "d/hd_screen_staff_roll_bg.h"
#include "d/hd_screen_fade_wipe_iris.h"
struct hdStaffSceneBubbleVtable_l : hdPartsVtbl_c { u8 mReservedCC[4]; gfn mRestart; };
WWHD_OFFSET(hdStaffSceneBubbleVtable_l,mRestart,0xCC);
struct hdStaffBubbleList_l { be<s32> mCount; be<s32> mCapacity; gptr<gptr<hdParts_c>> mpItems; };
struct hdStaffAmbientBubbles_c {
 enum { BUBBLE_COUNT=100 };
 gptr<hdStaffRollBubbleS_c> mpBubbles[BUBBLE_COUNT]; hdStaffBubbleList_l mDrawList;
 gptr<hdParts_c> mDrawStorage[BUBBLE_COUNT];
 void create(); void start(); void update(); void draw(void*); void release();
};
struct hdStaffCharacterBubbles_c {
 enum { BUBBLE_COUNT=30 };
 gptr<hdStaffRollBubble_c> mpBubbles[BUBBLE_COUNT]; hdStaffBubbleList_l mDrawList;
 gptr<hdParts_c> mDrawStorage[BUBBLE_COUNT]; be<u32> mCursor; be<u32> mFrame;
 struct Record { be<u32> mId; be<s32> mFrame; union { be<f32> mX; be<u32> mXBits; }; union { be<f32> mY; be<u32> mYBits; }; be<u32> mKind; } mRecords[300]; be<u32> mRecordCount;
 void loadRecords(); void create(); void start(); void update(); void draw(void*); void release();
};
struct hdStaffModel_l {
 gptr<gfn> mVTable; gptr<void> mpHeap; gptr<J3DModel> mpModel; gptr<mDoExt_btkAnm> mpBtk;
 struct Rotation { be<s32> x; be<s32> y; be<s32> z; };
 cXyz mPosition; cXyz mScale; Rotation mRotation; be<u32> mFirstFrame;
 cXyz mFirstPosition; cXyz mFirstScale; cXyz mFirstRotation; be<u32> mSecondFrame;
 cXyz mSecondPosition; cXyz mSecondScale; cXyz mSecondRotation; be<u32> mThirdFrame;
 cXyz mThirdPosition; cXyz mThirdScale; cXyz mThirdRotation;
 void draw(); void release();
};
struct hdStaffRenderTask_l { u8 mBase[0xC]; gptr<gfn> mVTable; SafeString mName; be<u32> mEnabled; be<u32> mReserved; HdStateDelegate mDelegate; u8 mTail[0x10]; };
struct hdStaffScene_c {
 gptr<gfn> mBaseVTable; gptr<gfn> mObjectVTable; gptr<hdStaffScene_c> mpSelf; be<u32> mReserved; HdStateMachine mState; gptr<gfn> mVTable; HdStateDisposer mDisposer;
 hdStaffRenderTask_l mStaffTask; hdStaffRenderTask_l mBackgroundTask; hdStaffRenderTask_l mFadeTask; u8 mTaskPad[4]; gptr<hdScreenStaffRoll_c> mpStaff; gptr<hdScreenStaffRollBG_c> mpBackground; gptr<hdFadeWipeIris_c> mpFade;
 hdStaffAmbientBubbles_c mAmbientBubbles;
 hdStaffCharacterBubbles_c mCharacterBubbles;
 be<u32> mFrame; be<f32> mFadeAlpha; be<u32> mFadePhase; be<u8> mDisplay; u8 mPad[3];
 hdStaffModel_l mModel;
 void createScreens(); void enter(); void execute(); void unloadLayouts(); void releaseScreens(); void release();
 void drawStaff(void*); void renderStaff(void*); void renderBackground(void*); void renderFade(void*);
 void drawModel(); void loadBgmCalc(); void startWaitCalc(); void fadeOutInEnter(); void fadeOutInCalc();
 void calcEnter(); void calc(); void calcExit(); void calcScreens(); void updateScreens();
};
struct hdStaffSceneSingleton_l { gptr<hdStaffScene_c> mpInstance; };
struct hdStaffDisposerSingleton_l { gptr<HdStateDisposer> mpInstance; };
struct hdStaffSoundTiming_l { be<u32> mFrame; be<u32> mStatus; };
struct hdStaffProcess_l { u8 mBase[0xC0]; be<u8> mNoDraw; u8 mReserved[0x1B]; u8 mDrawTag; };
WWHD_SIZE(hdStaffScene_c,0x1D68);
WWHD_OFFSET(hdStaffScene_c,mAmbientBubbles,0x108);
WWHD_OFFSET(hdStaffScene_c,mCharacterBubbles,0x434);
WWHD_OFFSET(hdStaffScene_c,mFrame,0x1CAC);
WWHD_OFFSET(hdStaffScene_c,mModel,0x1CBC);
WWHD_OFFSET(hdStaffAmbientBubbles_c,mDrawList,0x190);
WWHD_OFFSET(hdStaffCharacterBubbles_c,mDrawList,0x78);

struct hdStaffPlay_l { u8 mReserved[0x5D78]; be<u32> mDrawList; be<u32> mDrawBuffer; u8 mReservedToStaff[0x18]; be<u32> mStaffDrawList; };
struct hdStaffDrawState_l { be<u32> mList; be<u32> mBuffer; };


struct hdStaffResource_l { u8 mReserved[0x4C]; be<s32> mUserDataOffset; };
struct hdStaffFile_l { be<s32> mDataOffset; be<u32> mSize; };

struct hdStaffMaterialHeader_l { u8 mReserved[4]; be<s32> mNameOffset; };
struct hdStaffMaterial_l { gptr<hdStaffMaterialHeader_l> mpHeader; u8 mReserved[4]; gptr<J3DShape> mpShape; u8 mBody[0x39C-0xC]; };
struct hdStaffMaterialTable_l { u8 mReserved[0x24]; be<u16> mCount; };

struct hdStaffUiRanges_l { be<f32> mLower; be<f32> mUpper; be<f32> mTop; be<f32> mHeight; be<f32> mWidth; be<f32> mDepth; be<f32> mMinX; be<f32> mMaxY; be<f32> mMinY; be<u8> mSigned; be<u8> mUnsigned; u8 mPad[2]; };

WWHD_SIZE(hdStaffModel_l,0xAC);
WWHD_OFFSET(hdStaffModel_l,mPosition,0x10);
WWHD_OFFSET(hdStaffModel_l,mRotation,0x28);
WWHD_OFFSET(hdStaffModel_l,mFirstFrame,0x34);
WWHD_OFFSET(hdStaffModel_l,mSecondFrame,0x5C);
WWHD_OFFSET(hdStaffModel_l,mThirdFrame,0x84);
WWHD_SIZE(hdStaffRenderTask_l,0x40);
WWHD_OFFSET(hdStaffRenderTask_l,mName,0x10);
WWHD_OFFSET(hdStaffRenderTask_l,mDelegate,0x20);
WWHD_SIZE(hdStaffAmbientBubbles_c,0x32C);
WWHD_SIZE(hdStaffCharacterBubbles_c,0x1878);
WWHD_SIZE(hdStaffCharacterBubbles_c::Record,0x14);
WWHD_OFFSET(hdStaffCharacterBubbles_c,mRecords,0x104);
WWHD_OFFSET(hdStaffCharacterBubbles_c,mRecordCount,0x1874);
WWHD_OFFSET(hdStaffScene_c,mStaffTask,0x38);
WWHD_OFFSET(hdStaffScene_c,mpStaff,0xFC);
WWHD_OFFSET(hdStaffScene_c,mFadeAlpha,0x1CB0);
WWHD_OFFSET(hdStaffScene_c,mDisplay,0x1CB8);

// SafeString virtual interface used by the material-name comparison.
struct hdStaffStringVtable_l { u8 mPrefix[0x14]; gfn mAssureTermination; };
struct hdStaffStringView_l { gptr<be<u8>> mpText; gptr<hdStaffStringVtable_l> mVTable; };
WWHD_SIZE(hdStaffStringView_l,8);
WWHD_OFFSET(hdStaffStringVtable_l,mAssureTermination,0x14);
