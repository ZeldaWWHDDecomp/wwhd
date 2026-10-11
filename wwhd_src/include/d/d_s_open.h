/** Opening scene. HD: scene_class grows four bytes; native UI replaces the GC panes.
 * Partial native UI/controller views describe only the fields used here. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "f_op/f_op_scene.h"
#include "SSystem/SComponent/c_phase.h"

struct dScnOpen_proc_c {
    struct Vtable { u8 padding[12]; be<u32> destroy; };
    gptr<Vtable> mVtable;
    u8 padding[0x84];
    be<u8> mIsEnd;
};
WWHD_OFFSET(dScnOpen_proc_c, mIsEnd, 0x88);
struct dScnOpen_c : scene_class {
    /* 0x1C8 */ request_of_phase_process_class mPhs;
    /* 0x1D0 */ gptr<void> solid_heap;
    /* 0x1D4 */ gptr<dScnOpen_proc_c> mpProc;
    /* 0x1D8 */ be<u8> field_0x1d4;
    u8 padding[3];
    /* 0x1DC */ be<u32> mStreamTimer;
    BOOL draw();
    BOOL execute();
    void changeGameScene();
    s32 create();
};
WWHD_OFFSET(dScnOpen_c, mPhs, 0x1c8);
WWHD_OFFSET(dScnOpen_c, solid_heap, 0x1d0);
WWHD_OFFSET(dScnOpen_c, mpProc, 0x1d4);
WWHD_OFFSET(dScnOpen_c, field_0x1d4, 0x1d8);
WWHD_OFFSET(dScnOpen_c, mStreamTimer, 0x1dc);
WWHD_SIZE(dScnOpen_c, 0x1e0);
struct dScnOpen_UI {
    be<s32> mTimer;
    u8 padding[0x90];
    gptr<dScnOpen_proc_c> mpProc;
};
WWHD_OFFSET(dScnOpen_UI, mpProc, 0x94);
struct dScnOpen_DrawList {
    u8 padding[0x1dc];
    gptr<void> mp2DOpaHead;
    gptr<void> mp2DOpaTail;
    u8 tail[0x26c - 0x1e4];
};
struct dScnOpen_GameInfoHD {
    u8 padding[0x5140];
    char mNextStageName[10];
    be<s8> mNextStageRoomNo;
    be<s8> mNextStageLayer;
    be<u8> mEnableNextStage;
    u8 paddingStage[0x5ac9 - 0x514d];
    be<u8> mWindowNum;
    u8 paddingWindow[2];
    u8 mWindow[0x5d30 - 0x5acc];
    dScnOpen_DrawList mDrawList;
    gptr<void> mpWindow;
    gptr<void> mpViewport;
};
WWHD_OFFSET(dScnOpen_GameInfoHD, mNextStageName, 0x5140);
WWHD_OFFSET(dScnOpen_GameInfoHD, mWindowNum, 0x5ac9);
WWHD_OFFSET(dScnOpen_GameInfoHD, mWindow, 0x5acc);
WWHD_OFFSET(dScnOpen_GameInfoHD, mDrawList, 0x5d30);
WWHD_OFFSET(dScnOpen_GameInfoHD, mpWindow, 0x5f9c);
WWHD_OFFSET(dScnOpen_GameInfoHD, mpViewport, 0x5fa0);
struct dScnOpen_SaveInfoHD {
    u8 padding[0x116c];
    be<u32> mRestartRoomParam;
};
struct dScnOpen_PadButtons { be<u32> mButtons; };
struct dScnOpen_PadHD {
    gptr<dScnOpen_PadButtons> mpButtons;
    u8 padding[0x14];
    be<u32> mTrigger;
};
struct dScnOpen_DrawTag {
    u8 padding[12];
    gptr<void> mpTagData;
};
// HD: full-register results are kept for UI and controller bindings.
dScnOpen_GameInfoHD* dScnOpen_getGameInfo();
dScnOpen_DrawTag* fopDwIt_Begin();
dScnOpen_DrawTag* fopDwIt_Next(dScnOpen_DrawTag*);
void fpcM_Draw(void*);
void dScnOpen_set2DOpa(dScnOpen_DrawList*, gptr<void>*, gptr<void>*, dScnOpen_proc_c*);
BOOL fopOvlpM_IsPeek();
void dComIfG_changeOpeningScene(dScnOpen_c*, u32);
BOOL fopScnM_ChangeReq(dScnOpen_c*, u32, u32, u32, u32);
void mDoAud_setSceneName(const char*, s32, s32);
void dScnOpen_UI_execute(dScnOpen_UI*);
void dScnOpen_UI_cleanup(dScnOpen_UI*);
void dScnOpen_UI_destroy();
void dScnOpen_UI_select(u32);
void dScnOpen_UI_initialize(dScnOpen_UI*);
void dScnOpen_proc_skip(dScnOpen_proc_c*);
u32 dScnOpen_requestLoad(void*, u32);
u32 dScnOpen_checkLoad(void*);
void mDoAud_bgmStreamPrepare(u32);
void mDoAud_bgmStreamPlay();
void mDoAud_bgmStop(u32);
BOOL mDoAud_isUsedHeapForStreamBuffer();
BOOL mDoCPd_checkA(u32);
BOOL mDoCPd_checkB(u32);
BOOL mDoCPd_checkStart(u32);
BOOL mDoCPd_checkX(u32);
BOOL mDoCPd_checkY(u32);
BOOL dComIfG_resetToOpening(dScnOpen_c*);
void mDoExt_destroySolidHeap(void*);
void dScnOpen_resDelete(request_of_phase_process_class*, const char*);
void base_process_class__ct(base_process_class*);
void base_process_class__dt(base_process_class*, u32);
void dScnOpen_initPhase(request_of_phase_process_class*, u32);
