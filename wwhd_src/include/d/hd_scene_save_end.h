/* HD post-credits save scene. Layout reconstructed from the binary. */
#pragma once
#include "bindings.h"
#include "d/hd_state_task.h"
#include "d/hd_title_parts.h"
#include "d/hd_scene_save_data.h"
#include "d/hd_sequence_window.h"
struct hdSaveStateObject_l { gptr<gfn> mVTable; };
// Scene state IDs store four direct callback descriptors in 0x2C bytes.
// Adjacent statics at 1049F554/1049F580 prove this extent; parts state IDs
// additionally store a null-state pointer at +0x2C and occupy 0x30 bytes.
struct hdSaveStateId_l {
 be<u32> mId; gptr<const char> mName; gptr<gfn> mVTable;
 be<u16> mEnterAdjustment; be<u16> mEnterVirtual; gptr<void> mEnter;
 be<u16> mCalcAdjustment; be<u16> mCalcVirtual; gptr<void> mCalc;
 be<u16> mExitAdjustment; be<u16> mExitVirtual; gptr<void> mExit;
 be<u16> mReserved; be<u16> mQueryVirtual; gptr<void> mQuery;
};
struct hdSaveEndScene_c : hdSaveDataScene_c {
 HdStateDisposer mDisposer;
 hdSequenceWindow_c* menu() { return static_cast<hdSequenceWindow_c*>(mpSaveDataScreen.get()); }
 // The ending scene reuses the base scene's cursor slot for its title background.
 hdTitleStartBackground_c* screen() { return static_cast<hdTitleStartBackground_c*>(mpCursor.get()); }
 void calcSaveInit(); void enterSaveInit(); u8 isFinished();
};
struct hdSaveSceneSingleton_l { gptr<hdSaveEndScene_c> mpInstance; };
struct hdSaveDisposerSingleton_l { gptr<HdStateDisposer> mpInstance; };
struct hdSaveController_l { u8 mReserved[0x14]; be<u8> mSaveRequested; };
struct hdSaveControllerSingleton_l { gptr<hdSaveController_l> mpInstance; };
struct hdSaveSlot_l { u8 mReserved[0x20]; be<s32> mSlot; };
struct hdSaveSlotSingleton_l { gptr<hdSaveSlot_l> mpInstance; };
WWHD_SIZE(hdSaveEndScene_c,0x160);
WWHD_OFFSET(hdSaveEndScene_c,mpSaveDataScreen,0x28);
WWHD_OFFSET(hdSaveEndScene_c,mpCursor,0x12C);
WWHD_OFFSET(hdSaveEndScene_c,mDisposer,0x150);
WWHD_SIZE(hdSaveStateId_l,0x2C);
WWHD_OFFSET(hdSaveStateId_l,mName,4);
WWHD_OFFSET(hdSaveStateId_l,mVTable,8);
WWHD_OFFSET(hdSaveStateId_l,mEnterAdjustment,0xC);
WWHD_OFFSET(hdSaveStateId_l,mCalcAdjustment,0x14);
WWHD_OFFSET(hdSaveStateId_l,mExitAdjustment,0x1C);
WWHD_OFFSET(hdSaveStateId_l,mReserved,0x24);
