/* AND switch 2: GameCube method names, HD actor layout.
 * HD uses an outgoing linkage area for every nested call, including compiler helpers. */
#pragma once
#include "gc_api/gc_api.h"

// HD: preserve the verified 16-byte outgoing area and its backchain before each guest call.
template <class R = void, class... A> inline R andsw2Call(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
template <class R = void, class... A> inline R andsw2CallPtr(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_GLUE_FUNC
#define WWHD_GLUE_FUNC(addr, R, ...)                                                               \
    if (gabi::Activation::nested())                                                                \
        return andsw2Call<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                     \
    gabi::Activation wwhd_activation_

enum Action { ACT_ON_ALL, ACT_TIMER, ACT_ORDER, ACT_EVENT, ACT_OFF, ACT_WAIT };
enum Type { TYPE_ONE_OFF, TYPE_CONTINUOUS };

struct daAndsw2_c : fopAc_ac_c {
    /* 0x3AC */ be<u8> mAction;
    u8 _3AD;
    /* 0x3AE */ be<s16> mTimer;
    /* 0x3B0 */ be<s16> mEventIdx;
    u8 _3B2[2];
    daAndsw2_c() {
        andsw2Call(0x025D4ED0, this);
        __vtbl = 0x100071D4;
    }
    void setActio(u8 action) {
        mAction = action;
    }
    u8 getSwbit2();
    u8 getSwbit();
    u8 getTopSw();
    u8 getNum();
    BOOL chkAllSw2();
    u8 getTimer();
    u8 getType();
    u8 getEventNo();
    BOOL execute();
    s32 create();
};
WWHD_OFFSET(daAndsw2_c, mAction, 0x3AC);
WWHD_OFFSET(daAndsw2_c, mTimer, 0x3AE);
WWHD_OFFSET(daAndsw2_c, mEventIdx, 0x3B0);
WWHD_SIZE(daAndsw2_c, 0x3B4);

// HD: the saved global points to a 0x20-byte prefix before dSv_info_c.
struct Andsw2SaveData {
    u8 _0[0x20];
    dSv_info_c info;
};
WWHD_OFFSET(Andsw2SaveData, info, 0x20);
WWHD_SIZE(Andsw2SaveData, sizeof(dSv_info_c) + 0x20);

// HD: dispatch ignores the action return register, as the verified caller does.
struct Andsw2Action {
    be<u32> target;
    void operator()(daAndsw2_c* actor) {
        andsw2CallPtr<void>(target, actor);
    }
};
WWHD_SIZE(Andsw2Action, 4);
struct Andsw2ActionTable {
    Andsw2Action actions[6];
    Andsw2Action& operator[](u32 i) {
        return actions[i];
    }
};
WWHD_SIZE(Andsw2ActionTable, 24);

struct Andsw2HeaderStatics {
    be<f32> mPiLo, mPiHi;
    u8 mObjectA, mObjectB;
    u8 _A[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(Andsw2HeaderStatics, mZero, 0xC);
WWHD_SIZE(Andsw2HeaderStatics, 0x1C);
struct Andsw2HeaderRecords {
    u8 records[3][12];
};
WWHD_SIZE(Andsw2HeaderRecords, 36);

namespace Andsw2HD {
inline BOOL isSwitch(dSv_info_c* save, s32 sw, s32 room) {
    return andsw2Call<BOOL>(0x025BA0C0, save, sw, room);
}
inline void onSwitch(dSv_info_c* save, s32 sw, s32 room) {
    andsw2Call(0x025B9E38, save, sw, room);
}
inline void offSwitch(dSv_info_c* save, s32 sw, s32 room) {
    andsw2Call(0x025B9F7C, save, sw, room);
}
inline dComIfG_inf_c* getGameInfo() {
    return andsw2Call<dComIfG_inf_c*>(0x025200D4);
}
inline BOOL endCheck(dEvent_manager_c* manager, s32 idx) {
    return andsw2Call<BOOL>(0x025440C8, manager, idx);
}
inline s32 getEventIdx(dEvent_manager_c* manager, s32 name, u8 ev) {
    return andsw2Call<s32>(0x02543F10, manager, name, ev);
}
inline void orderOtherEventId(daAndsw2_c* actor, s32 idx, u8 ev) {
    andsw2Call(0x025D7A58, actor, idx, ev, 0xFFFF, 0, 1);
}
inline void copyActionTable(void* dst, const void* src) {
    andsw2Call(0xC000A848, dst, src, 24);
}
inline void registerGlobalObject(void* record) {
    andsw2Call(0x028F026C, record);
}
inline void initHeaderObjectA(u8* object) {
    andsw2Call(0x028ED6F8, object);
}
inline void initHeaderObjectB(u8* object) {
    andsw2Call(0x028EAB2C, object);
}
inline void destroyActor(daAndsw2_c* actor) {
    andsw2Call(0x025D50BC, actor, 0);
}
inline void deleteActor(daAndsw2_c* actor) {
    andsw2Call(0x0273AF40, actor);
}
} // namespace Andsw2HD
