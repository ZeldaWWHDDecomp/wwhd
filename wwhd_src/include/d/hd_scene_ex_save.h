/* HD file-select scene extension for completing a second quest. */
#pragma once
#include "d/hd_scene_boot_common.h"
#include "d/hd_scene_save_data.h"
#include "d/hd_sequence_window.h"
struct hdExSceneInput_l { u8 mPrefix[0x14]; be<u8> mBlocked; };
struct hdExSceneState_c {
    be<u32> mNumber;
    gptr<const char> mName;
    gptr<hdPartsStateIdVtbl_c> mpVtable;
    hdPartsPtmf_c mEnter, mExecute, mExit;
    be<u16> mReserved, mReservedIndex;
    gptr<void> mpCheck;
};
// The second-quest scene extends the canonical save-data scene prefix.
struct hdSceneExSave_c : hdSaveDataScene_c {
    hdSceneDisposer_c mDisposer;
    hdSequenceWindow_c* mainParts() { return static_cast<hdSequenceWindow_c*>(mpSaveDataScreen.get()); }
    hdTitleStartBackground_c* shortcut() { return static_cast<hdTitleStartBackground_c*>(mpCursor.get()); }
    hdScreenDeleteTable* sceneVtable() { return reinterpret_cast<hdScreenDeleteTable*>(mpTaskVtable.get()); }
    static hdSceneExSave_c* construct(hdSceneExSave_c*);
    static void createInstance(void* heap);
    static void destroyInstance();
    static void destroy(hdSceneExSave_c*, u32 flags);
    static void destroyDisposer(hdSceneDisposer_c*, u32 flags);
    static void staticInit();
    void enterExplainEX();
    void executeExplainEX();
    void enterSelectSaveDataEX();
    void executeSelectSaveDataEX();
    void restartExplainEX();
    void blockInput();
    u8 isBusyEX();
    u8 isSelectionBusyEX();
};
WWHD_OFFSET(hdExSceneInput_l, mBlocked, 0x14);
WWHD_OFFSET(hdExSceneState_c, mpVtable, 8);
WWHD_OFFSET(hdExSceneState_c, mEnter, 0xC);
WWHD_OFFSET(hdExSceneState_c, mExecute, 0x14);
WWHD_OFFSET(hdExSceneState_c, mExit, 0x1C);
WWHD_OFFSET(hdExSceneState_c, mpCheck, 0x28);
WWHD_SIZE(hdExSceneState_c, 0x2C);
WWHD_OFFSET(hdSceneExSave_c, mState, 0x10);
WWHD_OFFSET(hdSceneExSave_c, mpFileSelect, 0x134);
WWHD_OFFSET(hdSceneExSave_c, mDisposer, 0x150);
WWHD_SIZE(hdSceneExSave_c, 0x160);

WWHD_SIZE(hdSaveDataScene_c, 0x150);
WWHD_OFFSET(hdSaveDataScene_c, mpSaveDataScreen, 0x28);
WWHD_OFFSET(hdSaveDataScene_c, mpCursor, 0x12C);
