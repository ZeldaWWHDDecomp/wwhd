/* Event binary layouts and staff special handlers. GameCube names, verified HD offsets. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
struct event_binary_data_header {
    be<u32> eventTop;
    be<s32> eventNum;
    be<u32> staffTop;
    be<s32> staffNum;
    be<u32> cutTop;
    be<s32> cutNum;
    be<u32> dataTop;
    be<s32> dataNum;
    be<u32> fDataTop;
    be<s32> fDataNum;
    be<u32> iDataTop;
    be<s32> iDataNum;
    be<u32> sDataTop;
    be<s32> sDataNum;
    u8 unk[8];
};
struct dEvDtData_c {
    enum DataType { TYPE_FLOAT, TYPE_VEC, TYPE_INT = 3, TYPE_STRING };
    char mName[32];
    be<s32> mIndex, mSubstanceType, mSubstanceIdx, mSubstanceSize, mNextIdx;
    be<u32> field_0x34[3];
};
struct dEvDtFlag_c {
    be<u32> mFlag[320];
    enum { FlagMax = 320 * 32 };
    u32 flagMaxCheck(s32);
    u32 flagCheck(u32);
    u32 flagSet(u32);
    void init();
};
struct dEvDtCut_c {
    char mName[32];
    be<u32> mTagId, mIndex, mStartFlag[3], mFlagIdx, mFirstDataIdx, mNextCutIdx;
    be<u32> field_0x40[4];
    u32 startCheck();
};
struct dEvDtStaff_c {
    char mName[32];
    be<s32> mTagID, mStaffIdx, m28, mStaffType, mFirstCutIdx;
    u8 m34[4];
    be<u32> mCurCutIdx, mCurActionIdx;
    be<s16> mWipeDirection, mTimer;
    be<u8> m44, m45, mAdvance, mbHasAction;
    u8 field_48[8];
    enum StaffType_e {
        NORMAL_e,
        ALL_e,
        CAMERA_e,
        EFFECT_e,
        TIMEKEEPER_e,
        FIVE_e,
        DIRECTOR_e,
        MESSAGE_e,
        SOUND_e,
        LIGHT_e,
        TEN_e,
        PACKAGE_e,
        CREATE_e
    };
    void init();
    void advanceCut(u32);
    void specialProc_WaitStart(u32);
    void specialProc_WaitProc(u32);
    void specialProc();
    void specialProcPackage();
    void specialProcTimekeeper();
    void specialProcLight();
    void specialProcCreate();
    void specialProcSound();
    void specialProcDirector();
    void specialProcMessage();
};
struct dEvDtEvent_c {
    char mName[32];
    be<u32> field_0x20, field_0x24, mPriority, mStaffIdx[20];
    be<s32> mNStaff, mFlagCheckStart[2], mFlagCheckFinish[3];
    be<u8> mEventEndSound;
    u8 pad95[3];
    be<u32> field_0x98, field_0x9c, field_0xa0;
    be<s32> mEventState;
    be<u32> field_0xa8, field_0xac;
    u32 finish_check();
    void specialStaffProc(dEvDtStaff_c*);
};
struct dEvDtBase_c {
    gptr<event_binary_data_header> mHeaderP;
    gptr<dEvDtEvent_c> mEventP;
    gptr<dEvDtStaff_c> mStaffP;
    gptr<dEvDtCut_c> mCutP;
    gptr<dEvDtData_c> mDataP;
    gptr<be<f32>> mFDataP;
    gptr<be<s32>> mIDataP;
    gptr<char> mSDataP;
    void init();
    void advanceCut(dEvDtEvent_c*);
    u32 advanceCutLocal(dEvDtStaff_c*);
};
struct EventManagerView {
    u8 prefix[0x20];
    be<u32> mCameraPlay;
    u8 beforeFlags[0x3C - 0x24];
    dEvDtFlag_c mFlags;
};
// HD: substance words are also read by their serialized low halfword or final byte.
union EventValue {
    be<u32> integer;
    be<f32> scalar;
    struct {
        be<u16> high, low;
    } halves;
    be<u8> bytes[4];
};
struct EventSafeStringVtable {
    be<u32> prefix[5], assureTermination;
};
struct EventSafeString {
    gptr<const char> mStringPtr;
    gptr<EventSafeStringVtable> __vtbl;
};
struct EventObjectName {
    char name[8];
    be<s16> procname;
    be<s8> argument;
    u8 padding;
};
struct EventHeaderStatics {
    be<f32> piMin, piMax;
    be<u8> byteObject, flagObject;
    u8 pad[2];
    be<u32> words[4];
};
struct EventMessageState {
    be<u32> l_msgId, l_msgNo;
};
struct EventDemoState {
    be<u32> frameNoMsg, mode;
};
struct EventFadeState {
    be<f32> rate;
    u8 pad4[0x17 - 4];
    be<u8> enabled;
};
WWHD_SIZE(dEvDtFlag_c, 0x500);
WWHD_SIZE(dEvDtCut_c, 0x50);
WWHD_SIZE(dEvDtStaff_c, 0x50);
WWHD_SIZE(dEvDtEvent_c, 0xB0);
WWHD_SIZE(dEvDtBase_c, 0x20);
WWHD_SIZE(EventSafeString, 8);
WWHD_OFFSET(dEvDtStaff_c, mTimer, 0x42);
WWHD_OFFSET(EventManagerView, mFlags, 0x3C);

// HD call signatures specific to these verified handlers.
void assertion(const char*, u32, const char*);
void registerGlobalObject(void*);
void initializeHeaderByte(void*);
void initializeHeaderFlag(void*);
void operator_delete(void*);
void dKy_set_nexttime(f32);
void dComIfGp_setNextStage(const char*, s32, s32, s32, u32, u32, s32, f32);
void* getObjectRes(void*, EventSafeString*, EventSafeString*);
void* dComIfG_getStageRes(const char*, EventValue*);
void dComIfGp_demo_create(void*, EventValue*, f32);
void dComIfGp_demo_remove();
void dKy_instant_timechg(f32);
void dKy_instant_rainchg();
EventObjectName* dStage_searchName(EventValue*);
void createActor(s32, u32, cXyz*, s32, csXyz*, cXyz*, s32, u32);
void mDoAud_bgmStreamPlay();
void mDoAud_bgmStreamPrepare(u32);
void mDoAud_seStart(u32);
void mDoAud_prepareLandingDemo(u32);
void mDoAud_startLandingDemo(EventValue*);
void mDoAud_endLandingDemo(EventValue*);
void mDoAud_bgmStop(u32);
void fadeOutColor(GXColor*, f32);
void fadeOut(f32);
void mDoAud_bgmStart(u32);
void startQuake(dVibration_c*, EventValue*, u32, u32, cXyz*);
void stopQuake(dVibration_c*, u32);
void wipeOut(f32);
void messageSetStatus(void*, u32);
u32 messageSet(void*, u32, u32);
u32 messageGetStatus(void*);

WWHD_SIZE(event_binary_data_header, 0x40);
WWHD_SIZE(dEvDtData_c, 0x40);

enum : u32 { daPyFlg0_NO_DRAW = 0x08000000 };
