/* Warp departure actor: tww names, HD members +0x11C.
 * Private bindings preserve the original outgoing linkage for every call. */
#pragma once
#include "gc_api/gc_api.h"
// HD: reserve the outgoing linkage below the caller, including real nested helpers.
template <class R = void, class... A> static R warpfoutCall(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
template <class R = void, class... A> static R warpfoutCallPtr(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_GLUE_FUNC
#define WWHD_GLUE_FUNC(addr, R, ...)                                                               \
    if (gabi::Activation::nested())                                                                \
        return warpfoutCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                   \
    gabi::Activation wwhd_activation_

struct daWarpfout_c : fopAc_ac_c {
    void eventOrder();
    void CreateInit();
    s32 _create();
    void demo_proc();
    s32 _execute();
    u32 initWarp1(s32 staff);
    u32 get_effect_angle();
    void set_effect_wind01(cXyz* position, s16 offset);
    void initWarp2(s32 staff);
    void initWarp3(s32 staff);
    void initWarp4(s32 staff);
    s32 actWarp1(s32 staff);
    s32 actWarp2(s32 staff);
    s32 actWarp3(s32 staff);
    s32 actEnd(s32 staff);
    s32 actWarp4(s32 staff);
    void initEnd(s32 staff);
    /* 0x3AC */ be<s32> unk0;
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<s32> mStaffId;
    /* 0x3B8 */ be<s16> unknownInitFlag;
    u8 _3ba[2];
};
WWHD_OFFSET(daWarpfout_c, mTimer, 0x3B0);
WWHD_OFFSET(daWarpfout_c, mStaffId, 0x3B4);
WWHD_OFFSET(daWarpfout_c, unknownInitFlag, 0x3B8);
WWHD_SIZE(daWarpfout_c, 0x3BC);

struct WarpfoutGameInfo {
    u8 prefix[0x51D0];
    dEvt_control_c eventControl;
    dEvent_manager_c eventManager;
    u8 managerTail[0x5AB0 - 0x52C5];
    gptr<void> particle;
    u8 cameraPrefix[0x5AF8 - 0x5AB4];
    gptr<void> camera;
    u8 playerPrefix[0x5B34 - 0x5AFC];
    gptr<fopAc_ac_c> link;
};
WWHD_OFFSET(WarpfoutGameInfo, eventControl, 0x51D0);
WWHD_OFFSET(WarpfoutGameInfo, eventManager, 0x52C4);
WWHD_OFFSET(WarpfoutGameInfo, particle, 0x5AB0);
WWHD_OFFSET(WarpfoutGameInfo, camera, 0x5AF8);
WWHD_OFFSET(WarpfoutGameInfo, link, 0x5B34);
union WarpfoutAngle {
    be<u16> components[4];
    be<u32> words[2];
    csXyz rotation;
};
WWHD_SIZE(WarpfoutAngle, 8);
inline u32 warpfoutSecondResult() { return gabi::cpu->r[4]; }
inline void warpfoutSetSecondResult(u32 word) { gabi::cpu->r[4] = word; }
using WarpfoutEventActionFunc = s32 (daWarpfout_c::*)(s32);
struct WarpfoutAction {
    gabi::ptmf<WarpfoutEventActionFunc> function;
};
WWHD_SIZE(WarpfoutAction, 8);
struct WarpfoutDtorRecord {
    u8 bytes[12];
};
inline void Warpfout_actorConstruct(daWarpfout_c* a) { return warpfoutCall<void>(0x025D4ED0, a); }
inline WarpfoutGameInfo* Warpfout_getGame() { return warpfoutCall<WarpfoutGameInfo*>(0x025200D4); }
inline s32 Warpfout_getMyStaffId(dEvent_manager_c* manager, const char* name, s32 actor,
                                 s32 flags) {
    return warpfoutCall<s32>(0x02542D88, manager, name, actor, flags);
}
inline s32 Warpfout_getMyActIdx(dEvent_manager_c* manager, s32 staff, const void* actions,
                                s32 count, s32 flag, s32 extra) {
    return warpfoutCall<s32>(0x02542EDC, manager, staff, actions, count, flag, extra);
}
inline void Warpfout_cutEnd(dEvent_manager_c* manager, s32 staff) {
    return warpfoutCall<void>(0x02543280, manager, staff);
}
inline s32 Warpfout_getIsAddvance(dEvent_manager_c* manager, s32 staff) {
    return warpfoutCall<s32>(0x025447C8, manager, staff);
}
inline u32 Warpfout_seStart(u32 sound) { return warpfoutCall<u32>(0x025E1988, sound); }
inline s16 Warpfout_cameraAngleX(void* camera) { return warpfoutCall<s16>(0x024F8008, camera); }
inline s16 Warpfout_cameraAngleY(void* camera) { return warpfoutCall<s16>(0x024F8000, camera); }
inline s32 Warpfout_calcTimer(be<s32>* timer) { return warpfoutCall<s32>(0x0211D2F8, timer); }
inline void Warpfout_deleteActor(daWarpfout_c* actor) {
    return warpfoutCall<void>(0x025D57E0, actor);
}
inline void Warpfout_actorDestroy(daWarpfout_c* actor, s32 flags) {
    return warpfoutCall<void>(0x025D50BC, actor, flags);
}
inline void Warpfout_free(void* actor) { return warpfoutCall<void>(0x0273AF40, actor); }
inline void Warpfout_registerDestructor(void* record) {
    return warpfoutCall<void>(0x028F026C, record);
}
inline void Warpfout_headerConstructA(be<u8>* object) {
    return warpfoutCall<void>(0x028ED6F8, object);
}
inline void Warpfout_headerConstructB(be<u8>* object) {
    return warpfoutCall<void>(0x028EAB2C, object);
}
inline void Warpfout_particleSet(void* control, s32 group, u32 id, cXyz* pos, csXyz* angle,
                                 s32 scale, s32 alpha, s32 callback, s32 room, s32 a, s32 b,
                                 s32 c) {
    warpfoutCall(0x025A847C, control, group, id, pos, angle, scale, alpha, callback, room, a, b, c);
}
inline s32 warpfoutDispatch(const WarpfoutAction* entry, daWarpfout_c* actor, s32 staff) {
    s16 virtualIndex = entry->function.i, adjustment = entry->function.d;
    u32 object = gabi::ea(actor) + s32(adjustment), target;
    if (virtualIndex < 0)
        target = entry->function.f;
    else {
        // HD: virtual member pointers encode a signed vtable displacement in the target's low half.
        s16 vtableOffset = s16(u32(entry->function.f));
        u32 table = gabi::load<u32>(object + s32(vtableOffset));
        target = gabi::load<u32>(table + u32(s32(virtualIndex)) * 8 + 4);
    }
    return warpfoutCallPtr<s32>(target, gabi::at<daWarpfout_c>(object), staff);
}

struct headerZeroStorage {
    be<u32> values[4];
    auto& operator[](u32 i) { return values[i]; }
};

struct headerPiStorage {
    be<f32> values[2];
    auto& operator[](u32 i) { return values[i]; }
};

struct headerObjectsStorage {
    be<u8> values[2];
    auto& operator[](u32 i) { return values[i]; }
};

struct headerRecordsStorage {
    WarpfoutDtorRecord values[3];
    auto& operator[](u32 i) { return values[i]; }
};

// HD values for the GameCube sound and particle names.
enum {
    JA_SE_LK_BOSS_WARP_EFF_ED = 0x2887,
    ID_AK_SN_DEMOWARPWIND00 = 0x830E,
    ID_AK_SN_DEMOWARPWIND01 = 0x830F
};
