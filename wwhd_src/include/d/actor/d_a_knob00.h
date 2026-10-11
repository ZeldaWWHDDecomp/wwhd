// Regular knob doors, with the GameCube actor base expanded for WWHD.
// HD: embedded animation/message state and UTF-16 password buffers use larger layouts.
#pragma once
#include "bindings.h"
#include "d/d_door_hkyo.h"
#include "d/d_door_info.h"
#include "gc_api/f_op/f_op_actor.h"
#include <cstddef>
struct KnobAnimationBinding {
    u8 prefix[0x34];
    be<u32> __vtbl;
    u8 gap[12];
    be<u32> mpResource;
    u8 tail[0x20];
};
struct KnobMatrixCalc {
    be<u32> __vtbl;
    KnobAnimationBinding mBinding;
    be<u32> mSlots[3];
};
// HD: constructor view of the embedded bck wrapper; playback uses its shared API.
struct KnobBckStorage {
    J3DFrameCtrl mFrameCtrl;
    KnobMatrixCalc mCalc;
    gptr<J3DAnmTransform> mpAnimation;
};
struct KnobMessageWork {
    u8 prefix[0xB4];
    be<u32> __vtbl;
};
struct KnobStopBars {
    gptr<J3DModel> mpModel;
    be<f32> mOffsY;
    be<u8> mEnabled, mFrontCheck, mOpen, mClosed;
};
class daKnob00_c : public dDoor_info_c {
  public:
    enum Action {
        ACTION_INIT = 0,
        ACTION_WAIT,
        ACTION_DEMO,
        ACTION_TALK,
        ACTION_TALK_WAIT,
        ACTION_PASSWORD,
        ACTION_PASSWORD_TALK,
        ACTION_VILLA_TALK,
        ACTION_VILLA,
        ACTION_FIGURE_TALK,
        ACTION_FIGURE,
        ACTION_DEAD
    };
    enum DemoAction {
        DEMO_START = 9,
        DEMO_ANGLE = 10,
        DEMO_ADJUST = 11,
        DEMO_OPEN_FRONT = 12,
        DEMO_OPEN_BACK = 13,
        DEMO_OPEN_FRONT_CONTINUE = 14,
        DEMO_OPEN_BACK_CONTINUE = 15,
        DEMO_MESSAGE = 16,
        DEMO_SHIP_FRONT = 17,
        DEMO_SHIP_BACK = 18
    };
    enum Flags { FLAG_OPEN = 1, FLAG_AREA = 4 };
    dDoor_msg_c m2D0;
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    mDoExt_bckAnm mBckAnm;
    gptr<J3DModel> mpModel2;
    gptr<dBgW> mpBgW;
    KnobStopBars mStopBars;
    be<u8> mAction;
    be<s8> m_jnt;
    be<u8> mFlags, mHidden;
    be<s16> mAdjustTimer;
    be<u8> mPasswordState, mDoorType;
    KnobMessageWork* messageWork() { return gabi::at<KnobMessageWork>(gabi::ea(&m2D0) + 8); }
    KnobBckStorage* animationStorage() { return reinterpret_cast<KnobBckStorage*>(&mBckAnm); }
    BOOL drawCheckShip();
    BOOL CreateHeap();
    u8 getShapeType();
    void setEventPrm();
    u8 getType2();
    s32 chkPassward();
    BOOL msgDoor();
    void openInit(s32);
    BOOL openProc(s32);
    void openEnd();
    BOOL chkException();
    void calcMtx();
    BOOL CreateInit();
    s32 create();
    void setStart(f32, f32);
    void setAngle();
    BOOL adjustmentProc();
    BOOL demoProc();
    BOOL demoProc2();
    BOOL actionWait();
    BOOL actionDemo();
    BOOL actionTalk();
    BOOL actionTalkWait();
    BOOL actionPassward2();
    BOOL actionVilla();
    BOOL actionPassward();
    BOOL actionInit();
    BOOL actionFigure();
    BOOL draw();
    BOOL execute();
};
struct KnobPlayerVtable {
    u8 prefix[0x114];
    gfn mSetPlayerPosAndAngle;
};
struct KnobPlayer : public fopAc_ac_c {
    u8 gap[0x422 - 0x3AC];
    be<s16> mDemoMoveAngle;
    void setPlayerPosAndAngle(cXyz* position, s16 angle) {
        auto* vt = gabi::at<KnobPlayerVtable>(__vtbl);
        gabi::call_ptr(vt->mSetPlayerPosAndAngle.get(), this, position, angle);
    }
};
enum KnobProcessName { KNOB_PROCESS_SHIP = 0x39 };
struct KnobShip : public fopAc_ac_c {
    u8 gap[0x3E4 - 0x3AC];
    be<u8> mDoorVisible;
};
struct KnobMt : public fopAc_ac_c {
    u8 gap[0x8C7 - 0x3AC];
    be<u8> mAttnEnabled;
};
struct KnobWideVtable {
    u8 prefix[0x14];
    gfn mAssureTermination;
};
struct KnobWideString {
    gptr<be<u16>> mpData;
    be<u32> __vtbl;
    void assureTermination() {
        auto* vt = gabi::at<KnobWideVtable>(__vtbl);
        gabi::call_ptr(vt->mAssureTermination.get(), this);
    }
};
struct KnobWideBuffer : KnobWideString {
    be<s32> mCapacity;
    be<u16> mChars[18];
};
struct KnobGameInfo {
    u8 prefix[0x12A0];
    dBgS mBgS;
    u8 gap[0x51D0 - 0x12A0 - sizeof(dBgS)];
    dEvt_control_c mEvent;
    dEvent_manager_c mEventManager;
    u8 gap3[0x5B2C - 0x52C4 - sizeof(dEvent_manager_c)];
    gptr<KnobPlayer> mpPlayer;
    u8 gap4[0x5BDC - 0x5B30];
    be<u8> mPasswordOpen;
    u8 gap5[0x5BF0 - 0x5BDD];
    KnobWideString mInputPassword;
};
struct KnobSave {
    u8 prefix[0xD4];
    u8 mCollect;
    u8 gap[0x644 - 0xD5];
    dSv_event_c mEvent;
};
struct KnobSinCos {
    be<f32> mSin, mCos;
};
struct KnobTrigTable {
    KnobSinCos mEntries[8192];
};
struct KnobAnimationTable {
    be<s32> mResources[4];
};
struct KnobShapeResources {
    be<u8> mResources[8];
};
struct KnobNameTable {
    u8 prefix[0x10];
    be<u32> mNamesOffset;
};

WWHD_OFFSET(daKnob00_c, m2D0, 0x3EC);
WWHD_OFFSET(daKnob00_c, mPhase, 0x4F8);
WWHD_OFFSET(daKnob00_c, mpModel, 0x500);
WWHD_OFFSET(daKnob00_c, mBckAnm, 0x504);
WWHD_OFFSET(daKnob00_c, mpModel2, 0x590);
WWHD_OFFSET(daKnob00_c, mpBgW, 0x594);
WWHD_OFFSET(daKnob00_c, mStopBars, 0x598);
WWHD_OFFSET(daKnob00_c, mAction, 0x5A4);
WWHD_OFFSET(daKnob00_c, m_jnt, 0x5A5);
WWHD_OFFSET(daKnob00_c, mFlags, 0x5A6);
WWHD_OFFSET(daKnob00_c, mHidden, 0x5A7);
WWHD_OFFSET(daKnob00_c, mAdjustTimer, 0x5A8);
WWHD_OFFSET(daKnob00_c, mPasswordState, 0x5AA);
WWHD_SIZE(daKnob00_c, 0x5AC);
WWHD_SIZE(KnobBckStorage, 0x8C);
WWHD_OFFSET(KnobAnimationBinding, mpResource, 0x44);
WWHD_OFFSET(KnobGameInfo, mEvent, 0x51D0);
WWHD_OFFSET(KnobGameInfo, mEventManager, 0x52C4);
WWHD_OFFSET(KnobGameInfo, mpPlayer, 0x5B2C);
WWHD_OFFSET(KnobGameInfo, mPasswordOpen, 0x5BDC);
WWHD_OFFSET(KnobGameInfo, mInputPassword, 0x5BF0);
WWHD_SIZE(KnobWideBuffer, 0x30);
