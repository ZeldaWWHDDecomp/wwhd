/* WWHD double door. Local HD door layouts; */
#pragma once
#include "bindings.h"
#include "d/d_door_info.h"
#include "d/d_event.h"
struct Door12Key {
    be<u8> mbEnabled;
    u8 pad01[0x17];
    u8 mMatrixCalc[0x7C];
    /* 0x094 */ request_of_phase_process_class mPhase;
    u8 tail9C[4];
    BOOL create(s32 n);
    void calcMtx(dDoor_info_c* d);
    void draw(dDoor_info_c* d);
};
WWHD_SIZE(Door12Key, 0xA0);
struct Door12StopBars {
    gptr<J3DModel> mpModel;
    be<f32> mOffsY;
    be<u8> mbEnabled, mFrontCheck, mA, mB;
    BOOL create();
    void calcMtx(dDoor_info_c* d);
    void closeInit(dDoor_info_c* d);
    s32 closeProc(dDoor_info_c* d);
    void openInit(dDoor_info_c* d);
    s32 openProc(dDoor_info_c* d);
};
WWHD_SIZE(Door12StopBars, 0xC);
struct daDoor12_c : dDoor_info_c {
    enum Action { ACTION_INIT, ACTION_WAIT, ACTION_STOP_CLOSE, ACTION_DEMO };
    enum Flags { FLAG_OPEN = 1, FLAG_CLOSE = 2 };
    enum DemoAction {
        DEMO_OPEN_STOP = 1,
        DEMO_CLOSE_STOP,
        DEMO_OPEN,
        DEMO_CLOSE,
        DEMO_GOAL = 7,
        DEMO_KEY,
        DEMO_END = 19,
        DEMO_PLAYER_FRONT,
        DEMO_PLAYER_BACK
    };
    enum Shape {
        SHAPE_NORMAL,
        SHAPE_STONE,
        SHAPE_BOSS,
        SHAPE_NORMAL_WIDE,
        SHAPE_STONE_WIDE,
        SHAPE_BOSS_WIDE
    };
    enum Resource { DZB_BOSS = 12, DZB_NORMAL = 13 };

    /* 0x3EC */ request_of_phase_process_class mPhase;
    /* 0x3F4 */ gptr<J3DModel> mpLeftModel;
    /* 0x3F8 */ gptr<J3DModel> mpRightModel;
    /* 0x3FC */ gptr<dBgW> mpBgw;
    /* 0x400 */ Door12Key mKeyLock;
    /* 0x4A0 */ Door12StopBars mStopBars;
    /* 0x4AC */ be<u8> mAction;
    u8 _4AD;
    /* 0x4AE */ be<u16> mFlags;
    /* 0x4B0 */ be<s32> mExecuteState;
    /* 0x4B4 */ be<f32> mOpen;
    s32 getShapeType();
    const char* getArcName();
    s32 getBdlLf();
    s32 getBdlRt();
    s32 getDzb();
    f32 openWide();
    void calcMtx();
    s32 chkMakeKey();
    BOOL chkMakeStop();
    BOOL CreateHeap();
    void openInit();
    void closeInit();
    s32 chkStopF();
    s32 chkStopB();
    void setStop();
    BOOL openProc();
    void openEnd();
    BOOL closeProc();
    void closeEnd();
    void demoProc();
    BOOL chkStopOpen();
    void setStopDemo();
    BOOL chkStopClose();
    void setEventPrm();
    s32 actionWait();
    s32 actionDemo();
    s32 actionStopClose();
    void setKey();
    s32 actionInit();
    s32 draw();
    BOOL CreateInit();
    cPhs_State create();
    void cutEnd() {
        s32 staff = mStaffId;
        dComIfGp_evmng_cutEnd(staff);
    }
    void sound(u32 id) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(id, &eyePos, 0, reverb);
    }
};
WWHD_OFFSET(daDoor12_c, mKeyLock, 0x400);
WWHD_OFFSET(daDoor12_c, mStopBars, 0x4A0);
WWHD_OFFSET(daDoor12_c, mOpen, 0x4B4);
WWHD_SIZE(daDoor12_c, 0x4B8);

// HD: private views keep the double-door fields distinct from shared singleton layouts.
struct Door12RoomStatus {
    be<u8> mFlags;
    u8 pad[0x22B];
};
struct Door12RoomTable {
    Door12RoomStatus mRooms[256];
};
struct Door12ShapeTable {
    be<u8> mShapes[13];
};
struct Door12ModelTable {
    be<s32> mResources[6];
};
struct Door12ActionTable {
    gfn mActions[4];
};
struct Door12PlayerVtable {
    u8 pad[0xBC];
    gfn mGetGrabActorId;
};
struct Door12Player {
    u8 pad[0xB4];
    gptr<Door12PlayerVtable> __vtbl;
};
struct Door12EventManager {
    u8 pad[0x24];
    u8 mStartDemo[1];
};
struct Door12GameInfo {
    u8 pad[0x51CC];
    u8 mRoomControl[4];
    dEvt_control_c mEvent;
    Door12EventManager mEventManager;
    u8 gap0[0x599C - 0x52C4 - sizeof(Door12EventManager)];
    u8 mVibration[1];
    u8 gap1[0x5B2C - 0x599D];
    gptr<Door12Player> mpPlayer;
    u8 gap2[0x5D70 - 0x5B30];
    be<u32> mBgDrawList, mBgXluDrawList, mDrawList, mXluDrawList;
};
struct Door12Save {
    u8 pad[0x798];
    u8 mDungeonCollect[0x20];
    be<u8> mKeyNum;
    u8 gap[0x1178 - 0x7B9];
    u8 mTmpEvent[1];
};
struct Door12BackgroundRoom {
    u8 pad[0xB8];
    be<u16> mRoomId;
};
WWHD_OFFSET(Door12Key, mMatrixCalc, 0x18);
WWHD_OFFSET(Door12GameInfo, mEvent, 0x51D0);
WWHD_OFFSET(Door12GameInfo, mEventManager, 0x52C4);
WWHD_OFFSET(Door12GameInfo, mVibration, 0x599C);
WWHD_OFFSET(Door12GameInfo, mpPlayer, 0x5B2C);
WWHD_OFFSET(Door12GameInfo, mBgDrawList, 0x5D70);
WWHD_OFFSET(Door12Save, mKeyNum, 0x7B8);
WWHD_OFFSET(Door12Save, mTmpEvent, 0x1178);
WWHD_SIZE(Door12RoomStatus, 0x22C);

WWHD_OFFSET(daDoor12_c, mpLeftModel, 0x3F4);
WWHD_OFFSET(daDoor12_c, mpRightModel, 0x3F8);
WWHD_OFFSET(daDoor12_c, mpBgw, 0x3FC);
WWHD_OFFSET(daDoor12_c, mAction, 0x4AC);
WWHD_OFFSET(daDoor12_c, mFlags, 0x4AE);
WWHD_OFFSET(daDoor12_c, mExecuteState, 0x4B0);
WWHD_OFFSET(Door12StopBars, mbEnabled, 8);
WWHD_OFFSET(Door12StopBars, mFrontCheck, 9);
WWHD_OFFSET(Door12StopBars, mA, 10);
