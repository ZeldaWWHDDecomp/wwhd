/* Event tag: GameCube actor tail at 0x290 becomes 0x3AC in HD.
 * HD adds the arrival delay and UTF-16 password frame; all guest layouts asserted. */
#pragma once
#include "f_op/f_op_actor.h"

struct daTag_Event_c : fopAc_ac_c {
    enum Action_e {
        ACTION_WAIT = 0,
        ACTION_ARRIVAL = 1,
        ACTION_HUNT = 2,
        ACTION_HUNT2 = 3,
        ACTION_READY = 4,
        ACTION_EVENT = 5,
        ACTION_SPE_ARRIVAL = 6,
        ACTION_SPE_HUNT = 7,
        ACTION_SPE_READY = 8,
        ACTION_SPE_EVENT = 9,
        ACTION_MJ_HUNT = 10,
        ACTION_MJ_READY = 11,
    };
    u8 getSwbit2();
    u8 getType();
    u16 getEventFlag();
    s32 arrivalTerms();
    u8 getSwbit();
    u8 getEventNo();
    s32 actionHunt2();
    s32 huntBlocked();
    s32 cancelShutter();
    s32 actionSpeEvent();
    s32 actionHunt();
    s32 actionMjHunt();
    s32 actionArrival();
    s32 actionEvent();
    s32 actionReady();
    s32 actionMjReady();
    s32 actionSpeHunt();
    s32 actionSpeArrival();
    s32 actionSpeReady();
    void demoEndProc();
    void demoProc();
    void demoInitProc();
    /* 0x3AC */ be<s16> mEventIdx;
    /* 0x3AE */ be<s16> mDemoTimer;
    /* 0x3B0 */ be<s16> mHuntTimer;
    /* 0x3B2 */ be<u8> mAction;
    /* 0x3B3 */ be<u8> mBkType;
    /* 0x3B4 */ be<u32> mArrivalDelay;
};
WWHD_OFFSET(daTag_Event_c, mEventIdx, 0x3AC);
WWHD_OFFSET(daTag_Event_c, mDemoTimer, 0x3AE);
WWHD_OFFSET(daTag_Event_c, mHuntTimer, 0x3B0);
WWHD_OFFSET(daTag_Event_c, mAction, 0x3B2);
WWHD_OFFSET(daTag_Event_c, mBkType, 0x3B3);
WWHD_OFFSET(daTag_Event_c, mArrivalDelay, 0x3B4);
WWHD_SIZE(daTag_Event_c, 0x3B8);

u32 TagEvent_getGameInfo();
s32 TagEvent_isSwitch(u32, s32, s32);
void TagEvent_onSwitch(u32, s32, s32);
s32 TagEvent_isEventBit(u32, u32);
void TagEvent_onEventBit(u32, u32);
void TagEvent_orderOtherEventId(daTag_Event_c*, s32, u32, u32, s32, s32);
s32 TagEvent_getEventIdx(u32, u32, u32);
void TagEvent_subtract(const cXyz*, cXyz*, const cXyz*);
f32 PSVECSquareMag(const cXyz*);
u32 TagEvent_getPId(u32, u32);
u32 TagEvent_searchFromName(const char*, u32, u32);
s32 TagEvent_endCheck(u32, s32);
void TagEvent_delete(void*);
void TagEvent_actorDtor(daTag_Event_c*, s32);
void mDoAud_subBgmStart(u32);
void mDoAud_seStart(u32);
void TagEvent_StartShock(u32, s32, s32, cXyz*);
void mDoAud_bgmStart(u32);
void mDoAud_bgmAllMute(s32);
f32 TagEvent_rndF(f32);
void TagEvent_setEventReg(u32, u32, u32);
u8 TagEvent_getEventReg(u32, u32);
void TagEvent_passwordGet(u32, be<u16>*, u32);
void TagEvent_memcpy(u32, u32, u32, u32);
void TagEvent_actorCtor(daTag_Event_c*);
void TagEvent_registerGlobalObject(u32);
void TagEvent_scalarCtor(u32);
void TagEvent_scalarCtor2(u32);

/* HD: sparse views keep the password frame and event control accesses explicit. */
struct TagEventControl {
    u8 _00[0xCC]; be<u32> mPtTalk; be<u32> mPtItem;
};
WWHD_OFFSET(TagEventControl, mPtTalk, 0xCC);
WWHD_OFFSET(TagEventControl, mPtItem, 0xD0);
WWHD_SIZE(TagEventControl, 0xD4);
struct TagGameInfo {
    u8 _00[0x5134]; be<u8> mStageName[8];
    u8 _513C[0x51D0-0x513C]; TagEventControl mEventControl;
    u8 _52A4[0x52B8-0x52A4]; be<u16> mEventFlag;
    u8 _52BA[0x5B34-0x52BA]; be<u32> mLinkPlayer;
    u8 _5B38[0x5B44-0x5B38]; be<f32> mArrivalChange;
    u8 _5B48[0x5BAC-0x5B48]; be<u16> mArrivalCount;
    u8 _5BAE[0x5BF0-0x5BAE]; be<u32> mInputPassword;
    u8 _5BF4[4]; be<s32> mInputPasswordCapacity;
    u8 _5BFC[0x5CD8-0x5BFC]; be<u32> mPlayerStatus;
};
WWHD_OFFSET(TagGameInfo, mStageName, 0x5134);
WWHD_OFFSET(TagGameInfo, mEventControl, 0x51D0);
WWHD_OFFSET(TagGameInfo, mEventFlag, 0x52B8);
WWHD_OFFSET(TagGameInfo, mLinkPlayer, 0x5B34);
WWHD_OFFSET(TagGameInfo, mArrivalChange, 0x5B44);
WWHD_OFFSET(TagGameInfo, mArrivalCount, 0x5BAC);
WWHD_OFFSET(TagGameInfo, mInputPassword, 0x5BF0);
WWHD_OFFSET(TagGameInfo, mInputPasswordCapacity, 0x5BF8);
WWHD_OFFSET(TagGameInfo, mPlayerStatus, 0x5CD8);
WWHD_SIZE(TagGameInfo, 0x5CDC);
struct TagStringVtable { u8 _00[0x14]; be<u32> mAssureTermination; };
WWHD_OFFSET(TagStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(TagStringVtable, 0x18);
struct TagDragon : fopAc_ac_c { u8 _3AC[0x3E4-0x3AC]; be<u8> mQuakeInfo; u8 _3E5[3]; };
WWHD_OFFSET(TagDragon, mQuakeInfo, 0x3E4);
WWHD_SIZE(TagDragon, 0x3E8);
struct TagBokoblin : fopAc_ac_c { u8 _3AC[0x137C-0x3AC]; be<u8> mDemoMode; u8 _137D[3]; };
WWHD_OFFSET(TagBokoblin, mDemoMode, 0x137C);
WWHD_SIZE(TagBokoblin, 0x1380);
struct TagCallerLinkage { be<u32> mBackchain; be<u32> mSavedLinkRegister; };
WWHD_OFFSET(TagCallerLinkage, mSavedLinkRegister, 4);
WWHD_SIZE(TagCallerLinkage, 8);
inline TagGameInfo* TagEvent_game(u32 address) { return gabi::at<TagGameInfo>(address); }
inline be<u8>* TagEvent_bytes(u32 address) { return gabi::at<be<u8>>(address); }
inline be<u16>* TagEvent_chars(u32 address) { return gabi::at<be<u16>>(address); }
inline u32 TagEvent_saveAddress() { return *gabi::at<be<u32>>(0x101F84DC); }
inline u32 TagEvent_messageAddress() { return *gabi::at<be<u32>>(0x101F4B5C); }

namespace TagEvent {
struct TagSafeString { be<u32> text; be<u32> vtable; };
// Failed password lookup leaves this buffer untouched. The HD length scan can
// therefore reach saved registers; retain their defined guest-frame bytes.
struct TagPasswordFrame {
    be<u32> backchain;
    u8 _04[4];
    TagSafeString text;
    u8 _10[8];
    be<u16> password[20];
    be<u32> saved[4];
};
WWHD_OFFSET(TagPasswordFrame, text, 0x08);
WWHD_OFFSET(TagPasswordFrame, password, 0x18);
WWHD_OFFSET(TagPasswordFrame, saved, 0x40);
WWHD_SIZE(TagPasswordFrame, 0x50);
}
