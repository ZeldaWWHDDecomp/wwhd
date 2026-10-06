/* WWHD double door. Local HD door layouts; */
#pragma once
#include "bindings.h"
struct dDoor_info12_l : fopAc_ac_c {
    be<s8> mRoomNo2;
    be<u8> mFromRoomNo, mToRoomNo;
    u8 _3AF;
    cXyz mAngleVec;
    be<u8> mFrontCheck, mEnemyTimer;
    be<s16> mEventIdx[12];
    be<u8> mToolId[12];
    be<u8> mEventAction, m3E3;
    be<s32> mStaffId;
    be<s8> mRoomNo;
    u8 _3E9[3];
    u8 getArg1() {
        return gabi::call<u8>(0x0252AA58, this);
    }
    u8 getType() {
        return gabi::call<u8>(0x0252AA2C, this);
    }
    u8 getSwbit() {
        return gabi::call<u8>(0x0252AA14, this);
    }
    u8 getSwbit2() {
        return gabi::call<u8>(0x0252AA20, this);
    }
    u8 getFRoomNo() {
        return gabi::call<u8>(0x0252A37C, this);
    }
    u8 getBRoomNo() {
        return gabi::call<u8>(0x0252A388, this);
    }
};
WWHD_SIZE(dDoor_info12_l, 0x3EC);
struct dDoor_key12_l {
    be<u8> mEnabled;
    u8 _01[0x9F];
    BOOL create(int n) {
        return gabi::call<BOOL>(0x0252B834, this, n);
    }
    void calcMtx(dDoor_info12_l *d) {
        gabi::call(0x0252B854, this, d);
    }
    void draw(dDoor_info12_l *d) {
        gabi::call(0x0252B9D4, this, d);
    }
};
WWHD_SIZE(dDoor_key12_l, 0xA0);
struct dDoor_stop12_l {
    gptr<J3DModel> mpModel;
    be<f32> mOffsetY;
    be<u8> mEnabled, mFrontCheck, mOtherEnabled, mB;
    BOOL create() {
        return gabi::call<BOOL>(0x0252BE48, this);
    }
    void calcMtx(dDoor_info12_l *d) {
        gabi::call(0x0252BA9C, this, d);
    }
    void closeInit(dDoor_info12_l *d) {
        gabi::call(0x0252BB9C, this, d);
    }
    s32 closeProc(dDoor_info12_l *d) {
        return gabi::call<s32>(0x0252BC3C, this, d);
    }
    void openInit(dDoor_info12_l *d) {
        gabi::call(0x0252BCF4, this, d);
    }
    s32 openProc(dDoor_info12_l *d) {
        return gabi::call<s32>(0x0252BD8C, this, d);
    }
};
WWHD_SIZE(dDoor_stop12_l, 0xC);
struct daDoor12_c : dDoor_info12_l {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModelLf, mpModelRt;
    gptr<dBgW> mpBgW;
    dDoor_key12_l mKey;
    dDoor_stop12_l mStop;
    be<u8> mAction;
    u8 _4AD;
    be<u16> mFlags;
    be<s32> mExecuteState;
    be<f32> mOpen;
    s32 getShapeType();
    const char *getArcName();
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
WWHD_OFFSET(daDoor12_c, mKey, 0x400);
WWHD_OFFSET(daDoor12_c, mStop, 0x4A0);
WWHD_OFFSET(daDoor12_c, mOpen, 0x4B4);
WWHD_SIZE(daDoor12_c, 0x4B8);
