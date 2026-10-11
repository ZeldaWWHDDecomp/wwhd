// Forbidden Woods door and animated barrier components. Local game-derived source.
// HD: the actor base and lighting expand; barrier arms omit two GameCube phase fields.
#pragma once
#include "bindings.h"
#include "d/d_door_info.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct KddoorSmokeVtable {
    u8 prefix[0x44];
    gfn mEnd;
};
struct KddoorSmokeCallback {
    gptr<KddoorSmokeVtable> __vtbl;
    u8 gap[0xD];
    be<u8> mRateOff;
    u8 tail[0xE];
    void end() {
        auto* vt = __vtbl.get();
        gabi::call_ptr(vt->mEnd.get(), this);
    }
};
struct dDoor_ssk_sub_c {
    void end();
    void closeInit();
    void openInit();
    void init();
    void calcMtx(dDoor_info_c* door, f32 x, f32 z, u32 mSide);
    BOOL openProc(dDoor_info_c* door);
    BOOL closeProc(dDoor_info_c* door);
    BOOL drawSet();

    /* 0x000 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x004 */ gptr<mDoExt_McaMorf> mpCap;
    /* 0x008 */ KddoorSmokeCallback mSmoke;
    /* 0x028 */ dCcD_Stts mStts;
    /* 0x064 */ dCcD_Cyl mCyl;
    /* 0x194 */ be<u8> mState;
    u8 pad195;
    /* 0x196 */ be<s16> mPhase;
    /* 0x198 */ be<s16> mAngle;
    /* 0x19A */ be<s16> mDelay;
    /* 0x19C */ cXyz mScale;
    /* 0x1A8 */ cXyz mCapScale;
    /* 0x1B4 */ cXyz mPos;
    /* 0x1C0 */ be<u8> mStarted;
    /* 0x1C1 */ be<u8> mSoundTimer;
    u8 pad1C2[2];
};
struct dDoor_ssk_c {
    void end();
    void closeInit();
    void openInit();
    void init(dDoor_info_c* door);
    void draw(dDoor_info_c* door);
    void calcMtx(dDoor_info_c* door);
    BOOL closeProc(dDoor_info_c* door);
    BOOL openProc(dDoor_info_c* door);
    void execute(dDoor_info_c* door);

    /* 0x000 */ dDoor_ssk_sub_c mArms[3];
    /* 0x54C */ be<u8> mbEnabled;
    /* 0x54D */ be<u8> mSide;
    /* 0x54E */ be<u8> mOtherEnabled;
    /* 0x54F */ be<u8> mMoving;
    /* 0x550 */ dKy_tevstr_c mTevStr;
};
struct KddoorKey {
    /* 0x00 */ be<u8> mbEnabled;
    u8 pad01[3];
    /* 0x04 */ gptr<J3DModel> mpModel;
    /* 0x08 */ mDoExt_bckAnm mBckAnim;
    /* 0x94 */ request_of_phase_process_class mPhs;
    /* 0x9C */ be<u8> mState, mbIsBossDoor;
    u8 pad9E[2];
};
struct KddoorDoorSmoke {
    KddoorSmokeCallback mSmokeCb;
    csXyz mRot;
    u8 pad26[2];
    cXyz mPos;
    be<u8> m34, m35;
    u8 pad36[2];
};
WWHD_SIZE(KddoorDoorSmoke, 0x38);
struct daKddoor_c : dDoor_info_c {
    enum Action { ACTION_INIT, ACTION_WAIT, ACTION_STOP_CLOSE, ACTION_DEMO };
    enum DemoAction {
        DEMO_OPEN_STOP = 1,
        DEMO_CLOSE_STOP,
        DEMO_OPEN,
        DEMO_CLOSE,
        DEMO_SMOKE_INIT,
        DEMO_SMOKE_END,
        DEMO_GOAL,
        DEMO_KEY,
        DEMO_END = 19
    };
    enum DoorType { TYPE_NORMAL, TYPE_ENEMY, TYPE_KEY, TYPE_SWITCH, TYPE_ENEMY_KEY };
    enum Flags { FLAG_OPEN = 1, FLAG_CLOSE = 2 };

    BOOL chkMakeKey();
    BOOL chkMakeStop();
    s32 chkStopF();
    s32 chkStopB();
    void setStop();
    BOOL chkGenocideCase();
    void setStopDemo();
    BOOL chkFeelerCase();
    BOOL chkStopOpen();
    BOOL chkStopClose();
    void calcMtx();
    void openInit();
    void closeInit();
    BOOL openProc();
    void openEnd();
    BOOL closeProc();
    void closeEnd();
    BOOL draw();
    void setKey();
    BOOL CreateHeap();
    BOOL CreateInit();
    s32 create();
    void setEventPrm();
    void demoProc();

    /* 0x3EC */ request_of_phase_process_class mPhase;
    /* 0x3F4 */ KddoorDoorSmoke mSmoke;
    /* 0x42C */ KddoorKey mKey;
    /* 0x4CC */ dDoor_ssk_c mStop;
    /* 0xBE4 */ gptr<J3DModel> mpModel;
    /* 0xBE8 */ gptr<dBgW> mpBgW;
    /* 0xBEC */ be<u8> mAction;
    u8 padBED;
    /* 0xBEE */ be<u16> mFlags;
    /* 0xBF0 */ be<f32> mTravel;
};

WWHD_SIZE(dDoor_ssk_sub_c, 0x1C4);
WWHD_OFFSET(dDoor_ssk_sub_c, mStts, 0x28);
WWHD_OFFSET(dDoor_ssk_sub_c, mCyl, 0x64);
WWHD_OFFSET(dDoor_ssk_sub_c, mPhase, 0x196);
WWHD_OFFSET(dDoor_ssk_sub_c, mScale, 0x19C);
WWHD_OFFSET(dDoor_ssk_sub_c, mCapScale, 0x1A8);
WWHD_OFFSET(dDoor_ssk_sub_c, mStarted, 0x1C0);
WWHD_SIZE(dDoor_ssk_c, 0x718);
WWHD_OFFSET(dDoor_ssk_c, mbEnabled, 0x54C);
WWHD_OFFSET(dDoor_ssk_c, mTevStr, 0x550);
WWHD_SIZE(KddoorKey, 0xA0);
WWHD_SIZE(daKddoor_c, 0xBF4);
WWHD_OFFSET(daKddoor_c, mPhase, 0x3EC);
WWHD_OFFSET(daKddoor_c, mSmoke, 0x3F4);
WWHD_OFFSET(daKddoor_c, mKey, 0x42C);
WWHD_OFFSET(daKddoor_c, mStop, 0x4CC);
WWHD_OFFSET(daKddoor_c, mpModel, 0xBE4);
WWHD_OFFSET(daKddoor_c, mpBgW, 0xBE8);
WWHD_OFFSET(daKddoor_c, mFlags, 0xBEE);
WWHD_OFFSET(daKddoor_c, mTravel, 0xBF0);

struct KddoorTrigEntry {
    be<f32> mSin, mCos;
};
struct KddoorTrigTable {
    KddoorTrigEntry mEntries[8192];
};
struct KddoorRoomStatus {
    be<u8> mFlags;
    u8 pad[0x22B];
};
struct KddoorRoomTable {
    KddoorRoomStatus mRooms[256];
};
struct KddoorSave {
    u8 prefix[0x20];
    u8 mSwitchInfo[0x7B8 - 0x20];
    be<u8> mKeyNum;
};
struct KddoorRoomControl {
    u8 prefix[4];
};
struct KddoorVibration {
    u8 prefix[1];
};
struct KddoorCollision {
    u8 prefix[1];
};
struct KddoorGameInfo {
    u8 prefix[0x12A0];
    dBgS mBgS;
    u8 gap0[0x26A4 - 0x12A0 - sizeof(dBgS)];
    KddoorCollision mCollision;
    u8 gap1[0x51CC - 0x26A4 - sizeof(KddoorCollision)];
    KddoorRoomControl mRoomControl;
    dEvt_control_c mEvent;
    dEvent_manager_c mEventManager;
    u8 gap2[0x599C - 0x52C4 - sizeof(dEvent_manager_c)];
    KddoorVibration mVibration;
    u8 gap3[0x5AB0 - 0x599C - sizeof(KddoorVibration)];
    gptr<void> mpParticle;
    u8 gap4[0x5D70 - 0x5AB4];
    be<u32> mBgDrawList, mBgXluDrawList, mDrawList, mXluDrawList;
};
struct KddoorArrayDestructor {
    u8 prefix[1];
};
struct KddoorLightBlock {
    be<f32> mLeading[6];
    be<u8> mBytes[4];
    be<s16> mShorts[4];
    be<f32> mTrailing[8];
};
struct KddoorHeaderStatics {
    be<f32> mMin, mMax;
    be<u8> mRandomByte0, mRandomByte1;
    u8 pad[2];
    be<u32> mSeeds[4];
};
struct KddoorActionTable {
    gfn mActions[4];
};
struct KddoorResourceName {
    gptr<char> text;
    be<u32> vtable;
};
WWHD_SIZE(KddoorSmokeCallback, 0x20);
WWHD_OFFSET(KddoorGameInfo, mEvent, 0x51D0);
WWHD_OFFSET(KddoorGameInfo, mEventManager, 0x52C4);
WWHD_OFFSET(KddoorGameInfo, mVibration, 0x599C);
WWHD_OFFSET(KddoorGameInfo, mpParticle, 0x5AB0);
WWHD_OFFSET(KddoorGameInfo, mBgDrawList, 0x5D70);
WWHD_SIZE(KddoorLightBlock, 0x44);

struct KddoorJointInfo {
    u8 prefix[4];
    be<u16> mIndex;
};
struct KddoorBackground : dBgW {
    u8 gap[0xB8 - sizeof(dBgW)];
    be<u16> mRoomId;
};
struct KddoorCylinderStorage {
    u8 prefix[0x110];
    be<u32> mAabVtable, mShapeVtable;
    cM3dGCyl mCylinder;
};
struct KddoorColliderSource {
    be<u32> mWords[17];
};

struct KddoorTevStorage {
    KddoorLightBlock mPrimary;
    u8 gap0[0xC0 - 0x44];
    KddoorLightBlock mSecondary;
    u8 gap1[0x144 - 0x104];
    KddoorLightBlock mTertiary;
};
WWHD_OFFSET(KddoorTevStorage, mSecondary, 0xC0);
WWHD_OFFSET(KddoorTevStorage, mTertiary, 0x144);
WWHD_OFFSET(KddoorSmokeCallback, mRateOff, 0x11);

WWHD_OFFSET(KddoorKey, mBckAnim, 8);
WWHD_OFFSET(KddoorKey, mPhs, 0x94);
WWHD_OFFSET(dDoor_ssk_sub_c, mPos, 0x1B4);
WWHD_OFFSET(dDoor_ssk_sub_c, mAngle, 0x198);
WWHD_OFFSET(dDoor_ssk_sub_c, mDelay, 0x19A);
WWHD_OFFSET(daKddoor_c, mAction, 0xBEC);

enum KddoorResource { dRes_INDEX_SSK_BDL_KTANA_00_e = 3, dRes_INDEX_SSK_BDL_TURU_02_e = 4 };
