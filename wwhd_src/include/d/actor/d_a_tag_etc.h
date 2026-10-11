#pragma once
#include "gc_api/gc_api.h"

// HD: only the Medli status word used by this tag is exposed here.
struct TagEtcMedli : fopAc_ac_c {
    u8 mPadding[0x420C - sizeof(fopAc_ac_c)];
    be<u32> m30F0;
    bool checkStatusFly() const { return ((u32)m30F0 & 0x10) != 0; }
    void onBitCamTagIn() { m30F0 = (u32)m30F0 | 0x20; }
    void offBitCamTagIn() { m30F0 = (u32)m30F0 & ~0x20u; }
};
WWHD_OFFSET(TagEtcMedli, m30F0, 0x420C);
fopAc_ac_c* TagEtc_searchFromName(const char*, u32, u32);
u32 TagEtc_getPId(dEvt_control_c*, fopAc_ac_c*);
inline void TagEtc_setItemPartner(fopAc_ac_c* actor) {
    auto* event = &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl;
    event->mPtItem = TagEtc_getPId(event, actor);
}

enum Action {
    ACT_WAIT,
    ACT_SEARCH,
    ACT_HUNT,
    ACT_READY,
    ACT_EVENT,
};

struct daTag_Etc_c : fopAc_ac_c {
    daTag_Etc_c();
    void setActio(u8 action) { mAction = action; }

    u8 getEventNo();
    u8 getType2();
    BOOL rangeCheck(fopAc_ac_c*);
    BOOL otherCheck(fopAc_ac_c*);
    void demoProc();
    void demoInitProc();
    cPhs_State create();

    /* 0x3AC */ be<u8> mAction;
    /* 0x3AD */ be<u8> field_0x291;
    /* 0x3AE */ be<u8> field_0x292;
    /* 0x3AF */ be<u8> field_0x293;
    /* 0x3B0 */ be<u32> mMedliPID;
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ be<s16> field_0x29A;
};
WWHD_OFFSET(daTag_Etc_c, mMedliPID, 0x3B0);
WWHD_OFFSET(daTag_Etc_c, field_0x29A, 0x3B6);
WWHD_SIZE(daTag_Etc_c, 0x3B8);

