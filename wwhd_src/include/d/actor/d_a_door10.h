/* WWHD door10 layouts. Derived game source: */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/f_op/f_op_actor.h"
#include "d/d_door_info.h"
// HD: these call sites narrow the out-of-line accessors to bytes.
struct Door10InfoHD : dDoor_info_c {
    u8 getType();
    u8 getSwbit();
    u8 getSwbit2();
    u8 getArg1();
    u8 getFRoomNo();
    u8 getBRoomNo();
    void openInitCom(s32 x);
    void closeEndCom();
    void initOpenDemo(s32 x);
    s32 getDemoAction();
    void setGoal();
    BOOL checkArea(f32 a, f32 b, f32 c);
    void initProc(s32 x);
    BOOL drawCheck(s32 x);
    s32 checkExecute();
    void startDemoProc();
};
struct Door10SmokeHD {
    u8 bytes[0x38];
    void smokeInit(void* p);
    void smokeProc(void* p);
    void smokeEnd();
};
// HD: the key animation contains the expanded matrix calculator before its animation pointer.
struct Door10MatrixCalcHD {
    u8 mState[0x78];
};
struct Door10BckAnimationHD {
    J3DFrameCtrl mFrameCtrl;
    Door10MatrixCalcHD mMatrixCalc;
    gptr<J3DAnmTransform> mpAnm;
};
WWHD_OFFSET(Door10BckAnimationHD, mMatrixCalc, 0x10);
WWHD_OFFSET(Door10BckAnimationHD, mpAnm, 0x88);
WWHD_SIZE(Door10BckAnimationHD, 0x8C);
struct Door10KeyHD {
    be<u8> mbEnabled;
    u8 _1[3];
    gptr<J3DModel> mpModel;
    Door10BckAnimationHD mBckAnim;
    request_of_phase_process_class mPhase;
    be<u8> m20, mbIsBossDoor;
    u8 _9e[2];
    BOOL keyCreate(s32 x);
    cPhs_State keyResLoad();
    void keyResDelete();
    void keyInit(void* p);
    BOOL keyProc();
    void calcMtx(void* p);
    void draw(void* p);
    void keyOn();
    void keyOff();
};
struct Door10StopHD {
    gptr<J3DModel> mpModel;
    be<f32> mOffsY;
    be<u8> m8, mFrontCheck, mA, mB;
    void calcMtx(void* p);
    void closeInit(void* p);
    BOOL closeProc(void* p);
    void openInit(void* p);
    BOOL openProc(void* p);
    BOOL create();
};
struct Door10HkyoHD {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    gptr<void> mpBrk;
    be<u8> mAnmIdx, mUse;
    u8 _12[2];
    bool chkUse() { return mUse != 0; }
    void onUse(u8 x) { mUse = x; }
    void offUse() { mUse = 0; }
    cPhs_State resLoad();
    void resDelete();
    BOOL create();
    void init();
    void calcMtx(void* p, f32 x);
    void draw(void* p);
    void proc(void* p);
    BOOL chkFirst();
    void onFirst();
    BOOL chkStart();
};
struct daDoor10_c : Door10InfoHD {
    static constexpr u32 __vt = 0x1000DE80;
    Door10SmokeHD m2D0;
    Door10KeyHD mKeyLock;
    Door10StopHD mStopBars;
    Door10HkyoHD mHkyo;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    be<u8> m354;
    u8 _4ed;
    be<u16> m356;
    be<f32> m358;
    request_of_phase_process_class mPhase;
    be<s32> m364;
    bool checkFlag(u16 f) { return (m356 & f) != 0; }
    void onFlag(u16 f) { m356 = m356 | f; }
    void offFlag(u16 f) { m356 = m356 & ~f; }
    void setAction(u8 x) { m354 = x; }
    s32 chkMakeKey();
    void setKey();
    BOOL chkMakeStop();
    s32 chkStopF();
    s32 chkStopB();
    void setStop();
    BOOL chkStopOpen();
    void setStopDemo();
    BOOL chkStopClose();
    const char* getBdlName();
    const char* getDzbName();
    f32 getSize2X();
    BOOL CreateHeap();
    void setEventPrm();
    void openInit();
    BOOL openProc();
    void openEnd();
    void closeInit();
    BOOL closeProc();
    void closeEnd();
    void calcMtx();
    BOOL CreateInit();
    cPhs_State create();
    void demoProc();
    BOOL draw();
};
WWHD_OFFSET(daDoor10_c, m2D0, 0x3EC);
WWHD_OFFSET(daDoor10_c, mKeyLock, 0x424);
WWHD_OFFSET(daDoor10_c, mStopBars, 0x4C4);
WWHD_OFFSET(daDoor10_c, mpModel, 0x4E4);
WWHD_SIZE(daDoor10_c, 0x500);

WWHD_SIZE(Door10InfoHD, 0x3EC);
WWHD_SIZE(Door10SmokeHD, 0x38);
WWHD_OFFSET(Door10KeyHD, mpModel, 4);
WWHD_OFFSET(Door10KeyHD, mBckAnim, 8);
WWHD_OFFSET(Door10KeyHD, mPhase, 0x94);
WWHD_OFFSET(Door10KeyHD, m20, 0x9C);
WWHD_SIZE(Door10KeyHD, 0xA0);
WWHD_OFFSET(Door10StopHD, mOffsY, 4);
WWHD_OFFSET(Door10StopHD, m8, 8);
WWHD_SIZE(Door10StopHD, 0xC);
WWHD_OFFSET(Door10HkyoHD, mpModel, 8);
WWHD_OFFSET(Door10HkyoHD, mUse, 0x11);
WWHD_SIZE(Door10HkyoHD, 0x14);
WWHD_OFFSET(daDoor10_c, mHkyo, 0x4D0);
WWHD_OFFSET(daDoor10_c, mpBgW, 0x4E8);
WWHD_OFFSET(daDoor10_c, m354, 0x4EC);
WWHD_OFFSET(daDoor10_c, m356, 0x4EE);
WWHD_OFFSET(daDoor10_c, m358, 0x4F0);
WWHD_OFFSET(daDoor10_c, mPhase, 0x4F4);
WWHD_OFFSET(daDoor10_c, m364, 0x4FC);

// HD: the room status stride grows to 0x22C; this accessor consumes an unsigned byte room.
struct Door10RoomStatusHD {
    u8 mRoomState[0x21C];
    be<u8> mFlags;
    u8 mTail[0xF];
};
struct Door10RoomTableHD {
    Door10RoomStatusHD entries[256];
};
WWHD_OFFSET(Door10RoomStatusHD, mFlags, 0x21C);
WWHD_SIZE(Door10RoomStatusHD, 0x22C);
WWHD_SIZE(Door10RoomTableHD, 256 * 0x22C);

struct Door10EventManagerHD {
    u8 mEventState[0x24];
    u8 mStartDemo;
};
struct Door10BackgroundHD {
    u8 mBackgroundState[0xB8];
    be<u16> mRoomId;
};
WWHD_OFFSET(Door10EventManagerHD, mStartDemo, 0x24);
WWHD_SIZE(Door10EventManagerHD, 0x25);
WWHD_OFFSET(Door10BackgroundHD, mRoomId, 0xB8);
WWHD_SIZE(Door10BackgroundHD, 0xBA);
BOOL Door10_roomIsDisplay(dStage_roomControl_c*, u8);
void* Door10_searchEnemy(s8);
BOOL Door10_isDungeonItem(dSv_memory_c*, s32);
void Door10_orderEvent(daDoor10_c*, s16, u8, s32, s32, s32);
s32 Door10_staffId(dEvent_manager_c*, const char*, s32, s32);
BOOL Door10_checkStartDemo(dEvent_manager_c*);
void Door10_cancelStartDemo(u8*);
void Door10_infoConstruct(daDoor10_c*);
void Door10_smokeConstruct(Door10SmokeHD*);
void Door10_keyConstruct(Door10KeyHD*);
void Door10_stopConstruct(Door10StopHD*);
void Door10_hkyoConstruct(Door10HkyoHD*);
void Door10_animationDestroy(void*, s32);
void Door10_actorDestroy(daDoor10_c*, s32);

BOOL Door10_endCheck(dEvent_manager_c*, s16);
BOOL Door10_isAdvance(dEvent_manager_c*, s32);

// HD: GHS initializes the writable dispatch table with one ordered word copy.
struct Door10ActionTableHD {
    be<u32> entries[5];
    void invoke(u8 action, daDoor10_c* door) { gabi::call_ptr(entries[action], door); }
};
WWHD_SIZE(Door10ActionTableHD, 0x14);
enum Door10Action { DOOR10_INIT, DOOR10_WAIT, DOOR10_STOP_CLOSE, DOOR10_DEMO, DOOR10_HKYO };
enum Door10Flag { DOOR10_OPEN = 1, DOOR10_CLOSE = 2 };
enum Door10Sound {
    JA_SE_OBJ_STN_DOOR_MOVE_U = 0x6907,
    JA_SE_OBJ_STN_DOOR_MOVE_D = 0x6908,
    JA_SE_OBJ_STN_DOOR_STOP_U = 0x6909,
    JA_SE_OBJ_STN_DOOR_STOP_D = 0x690A,
};

// HD: getters return the guest address of each translation-unit string copy.
inline const char* Door10_guestString(const char* text) { return gabi::at<char>(gabi::guest_str(text)); }
