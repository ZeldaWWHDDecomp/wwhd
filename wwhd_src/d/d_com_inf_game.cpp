/* d_com_inf_game: game info (dComIfG_inf_c / dComIfG_play_c) and its out-of-line helpers, WWHD.
 *
 * Translation unit 0251FD14..02525C94, from the image: the function-less unit whose __sinit is
 * 0251FC80 (probably d_com_inf_actor) precedes it; this unit starts with dComIfG_play_c::init
 * (0251FD14, tail-called by dComIfG_play_c::ct) and ends with its __sinit 02525A48 and the
 * per-TU inline copies behind it (02525ADC..02525C80: dSv_memory_c constructor, phase_3,
 * SafeString/destructor copies, dComIfG_inf_c destructor). The matcher's
 * "__sinit_d_com_inf_game_cpp" (0251FD94) is the dComIfG_play_c constructor. Part 1 (this file):
 * 0251FD14..02522F6C; part 2 in d_com_inf_game_2.cpp.
 *
 * HD: the game info g_dComIfG_gameInfo is a function-local static at 1046F0B0 constructed on first
 * use by its accessor 025200D4 (dComIfGp_get in bindings.h); the play object is at +0x12A0, the
 * save data through the pointer at 101F84DC (info = save + 0x20). Strings are compared through
 * sead::SafeString temporaries {top, vtable 1004B8C0} with inlined strcmp loops (at most 0x40001
 * characters); fixed name buffers are filled with inlined strncpy loops. */
#include "bindings.h"

namespace d_com_inf_game_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) { return gabi::call<u32>(0x0273AA24, file, line, msg); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void memzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline void memcpy_l(u32 d, u32 s, u32 n) { gabi::call(0xC000A848, d, s, n); }
static inline void construct_array_l(u32 p, u32 n, u32 size, u32 ct) { gabi::call(0x028EFFD0, p, n, size, ct); }
static inline u32 gi() { return gabi::call<u32>(0x025200D4); } /* dComIfG_inf_c accessor */
static inline u32 savep() { return gabi::load<u32>(0x101F84DC); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s8 lds8(u32 a) { return gabi::load<s8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 vcall(u32 obj, u32 slot) { return gabi::call_ptr<u32>(ld(ld(obj) + slot), obj); }

static constexpr u32 SS_VT = 0x1004B8C0;   /* sead::SafeString vtable of this unit */
static constexpr u32 FILE_ = 0x1004B9D0;   /* "d_com_inf_game.cpp" */

/* stage data (play + 0x3EB0 = info + 0x5150) virtuals */
static inline u32 stage() { return gi() + 0x5150; }
static inline u32 getStagInfo() { u32 s = stage(); return vcall(s, 0x15C); }
static inline u32 dSv_memBit_onTbox_l(u32 m, s32 no) { return gabi::call<u32>(0x025B8C0C, m, no); }
static inline u32 dSv_memBit_isTbox_l(u32 m, s32 no) { return gabi::call<u32>(0x025B8C74, m, no); }
static inline u32 dSv_memBit_onDungeonItem_l(u32 m, s32 no) { return gabi::call<u32>(0x025B9098, m, no); }
static inline u32 dSv_memBit_isDungeonItem_l(u32 m, s32 no) { return gabi::call<u32>(0x025B9100, m, no); }
static inline u32 getStatusRoomDt_l(u32 rc, u32 roomNo) { return gabi::call<u32>(0x025C11DC, rc, roomNo); }
static inline u32 isEventBit_l(u32 ev, u32 flag) { return gabi::call<u32>(0x025B8B94, ev, flag); }
static inline u32 getEventReg_l(u32 ev, u32 reg) { return gabi::call<u32>(0x025B8BB0, ev, reg); }
static inline void returnPlace_set_l(u32 p, u32 name, s32 room, u32 point) { gabi::call(0x025B50DC, p, name, room, point); }

/* sead::SafeString {top, vtable} */
struct SafeString_l {
    be<u32> top;
    be<u32> vt;
};

/* inlined strcmp of two SafeStrings: equal if the same pointer or the same characters up to a
 * NUL; a run of 0x40001 equal non-NUL characters counts as different */
static inline bool ss_equal(u32 a, u32 b) {
    if (a == b) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = ld8(a + i);
        u8 cb = ld8(b + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
/* inlined strncpy(dst, src, n) */
static inline void strncpy_l(u32 dst, u32 src, u32 n) {
    u32 i = 0;
    for (; i < n; i++) {
        u8 c = ld8(src + i);
        st8(dst + i, c);
        if (c == 0) break;
    }
    for (; i < n; i++) st8(dst + i, 0);
}

/* 0251FD14: dComIfG_play_c::init (player pointers) */
static void dComIfG_play_c_init(u32 i_this) {
    WWHD_FUNC(0x0251FD14, void, i_this);
    st(i_this + 0x488C, 0);
    st8(i_this + 0x4890, 0xFF);
    for (u32 i = 0; i < 3; i++) st(i_this + 0x4894 + 4 * i, 0);
}
VERIFY(0x0251FD14, dComIfG_play_c_init);

/* 0251FD3C: dComIfG_play_c::ct */
static void dComIfG_play_c_ct(u32 i_this) {
    WWHD_FUNC(0x0251FD3C, void, i_this);
    st(i_this + 0x4824, 0);
    st8(i_this + 0x4828, 0xFF);
    stf(i_this + 0x4A7C, 0.0f);
    st8(i_this + 0x4A75, 0xFF);
    stf(i_this + 0x4A80, 0.0f);
    st8(i_this + 0x4A74, 0xFF);
    st(i_this + 0x4820, 0);
    st8(i_this + 0x4A76, 0xFF);
    st(i_this + 0x4810, 0);
    st8(i_this + 0x4A77, 0xFF);
    st8(i_this + 0x4A8C, 0);
    st8(i_this + 0x4829, 0);
    st(i_this + 0x4814, 0);
    st(i_this + 0x481C, 0);
    stf(i_this + 0x4A78, 0.0f);
    st(i_this + 0x4818, 0);
    dComIfG_play_c_init(i_this);
}
VERIFY(0x0251FD3C, dComIfG_play_c_ct);

/* 0251FD94: dComIfG_play_c::dComIfG_play_c (matcher: __sinit_d_com_inf_game_cpp) */
static u32 dComIfG_play_c_ctor(u32 i_this) {
    WWHD_FUNC(0x0251FD94, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x4A90);
        if (i_this == 0) return 0;
    }
    gabi::call(0x02007FBC, i_this);           /* dBgS */
    st(i_this + 0x1400, 0x100436D8);
    gabi::call(0x02518704, i_this + 0x1404);  /* dCcS */
    gabi::call(0x025ABC48, i_this + 0x3DF8);
    memzero_l(i_this + 0x3E94, 0xC);
    gabi::call(0x025C3FAC, i_this + 0x3EA0);
    gabi::call(0x025C4548, i_this + 0x3EB0);
    gabi::call(0x0253EAB8, i_this + 0x3F30);  /* dEvt_control_c */
    gabi::call(0x025436A4, i_this + 0x4024);  /* dEvent_manager_c */
    gabi::call(0x024ED630, i_this + 0x4564);  /* dAttention_c */
    gabi::call(0x025CC0CC, i_this + 0x46FC);  /* dVibration_c */
    gabi::call(0x0252A0E0, i_this + 0x4780);  /* dDetect_c */
    for (u32 o = 0x4794; o <= 0x47AC; o += 4) st(i_this + o, 0);
    /* sead::FixedSafeString<8> at +0x47B0 (inline constructors allocate when null) */
    u32 fs = i_this + 0x47B0;
    bool ok = true;
    if (fs == 0) {
        fs = operator_new_l(0x14);
        ok = fs != 0;
    }
    if (ok) {
        u32 b = fs;
        bool bok = true;
        if (b == 0) {
            b = operator_new_l(0xC);
            bok = b != 0;
        }
        if (bok) {
            st(b + 0, fs + 0xC);
            st(b + 4, 0x1004B8D8);
            st(b + 8, 8);
            st8(fs + 0x13, 0);
        }
        u32 top = ld(fs + 0);
        st(fs + 4, 0x1004B948);
        st8(top, 0);
        st(fs + 4, 0x1004B960);
    }
    st(i_this + 0x47C4, 0);
    st8(i_this + 0x4829, 0);
    st8(i_this + 0x4828, 0xFF);
    static const u16 zw[] = {0x4818, 0x4814, 0x4804, 0x47F4, 0x4810, 0x4808, 0x480C, 0x4824, 0x481C, 0x47DC, 0x47D8,
                             0x47E4, 0x47E8, 0x47FC, 0x47EC, 0x47C8, 0x4800, 0x47F0, 0x47F8, 0x47CC, 0x4820, 0x47D4,
                             0x47E0, 0x47D0};
    for (u16 o : zw) st(i_this + o, 0);
    st16(i_this + 0x482A, 0);
    memzero_l(i_this + 0x48A0, 0xF0);
    /* sead::WFixedSafeString<17> at +0x4950 */
    u32 ws = i_this + 0x48A0 + 0xB0;
    bool wok = true;
    if (ws == 0) {
        ws = operator_new_l(0x30);
        wok = ws != 0;
    }
    if (wok) {
        st16(ws + 0x2C, 0);
        st(ws + 0, ws + 0xC);
        st16(ws + 0xC, 0);
        st(ws + 4, 0x1004B990);
        st(ws + 8, 0x11);
    }
    memzero_l(i_this + 0x4990, 0x30);
    memzero_l(i_this + 0x49C0, 0x72);
    st(i_this + 0x4A34, 0);
    memzero_l(i_this + 0x4A48, 8);
    st(i_this + 0x4A6C, 0);
    st(i_this + 0x4A64, 0);
    st8(i_this + 0x4A76, 0xFF);
    st8(i_this + 0x4A77, 0);
    st(i_this + 0x4A58, 0);
    st(i_this + 0x4A54, 0);
    st(i_this + 0x4A5C, 0xFFFFFFFF);
    st(i_this + 0x4A70, 0);
    st(i_this + 0x4A68, 0);
    st8(i_this + 0x4A75, 0xFF);
    st(i_this + 0x4A50, 0);
    st8(i_this + 0x4A74, 0xFF);
    st16(i_this + 0x4A60, 0);
    memzero_l(i_this + 0x4A78, 0xC);
    st(i_this + 0x4A88, 0);
    st8(i_this + 0x4A8C, 0);
    st(i_this + 0x4A84, 0);
    dComIfG_play_c_ct(i_this);
    return i_this;
}
VERIFY(0x0251FD94, dComIfG_play_c_ctor);

/* 02520020: dComIfG_inf_c::ct */
static void dComIfG_inf_c_ct(u32 i_this) {
    WWHD_FUNC(0x02520020, void, i_this);
    st8(i_this + 0x62F1, 0xFF);
    dComIfG_play_c_ct(i_this + 0x12A0);
}
VERIFY(0x02520020, dComIfG_inf_c_ct);

/* 02520030: dComIfG_inf_c::dComIfG_inf_c (HD 0x62F8) */
static u32 dComIfG_inf_c_ctor(u32 i_this) {
    WWHD_FUNC(0x02520030, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x62F8);
        if (i_this == 0) return 0;
    }
    memzero_l(i_this, 0x12A0);
    construct_array_l(i_this + 0x380, 0x10, 0x24, 0x02525ADC);
    gabi::call(0x025B9170, i_this + 0x778); /* dSv_memory_c::init */
    construct_array_l(i_this + 0x7A8, 0x20, 0x4C, 0x025B9774);
    dComIfG_play_c_ctor(i_this + 0x12A0);
    gabi::call(0x0252E9CC, i_this + 0x5D30); /* dDlst_list_c */
    st8(i_this + 0x62F1, 0xFF);
    dComIfG_inf_c_ct(i_this);
    return i_this;
}
VERIFY(0x02520030, dComIfG_inf_c_ctor);

/* 025200D4: HD accessor of g_dComIfG_gameInfo (function-local static, constructed on first use) */
static u32 dComIfG_getGameInfo() {
    WWHD_FUNC(0x025200D4, u32);
    if (ld(0x104753A8) == 0) {
        st(0x104753A8, 1);
        dComIfG_inf_c_ctor(0x1046F0B0);
        __register_global_object(0x101D5DE8);
    }
    return 0x1046F0B0;
}
VERIFY(0x025200D4, dComIfG_getGameInfo);

/* 0252012C */
static s32 dComIfGp_setNextStage(u32 i_stageName, u32 i_point, u32 i_roomNo, u32 i_layer, f32 i_lastSpeed, u32 i_lastMode,
                                 u32 i_setPoint, u32 i_wipe) {
    WWHD_FUNC(0x0252012C, s32, i_stageName, i_point, i_roomNo, i_layer, i_lastSpeed, i_lastMode, i_setPoint, i_wipe);
    u32 ns = gi() + 0x5140; /* dStage_nextStage_c */
    if (gabi::call<u32>(0x025C3FEC, ns, i_stageName, i_roomNo, i_point, i_layer, i_wipe) == 0) return 0;
    u32 player = ld(gi() + 0x5B34);
    if (player != 0) {
        u32 flags = ld(player + 0x3BC);
        if (flags & 1) i_lastMode |= 0x8000;
        i_lastMode |= (u32)(s32)lds16(player + 0x699E) << 16;
        if (flags & 0x8000) i_lastMode |= 0x4000;
    }
    u32 sv = savep();
    stf(sv + 0x1170, i_lastSpeed);
    st(sv + 0x1174, i_lastMode);
    if (i_setPoint != 0) st16(savep() + 0x115C, (u16)i_point);
    return 1;
}
VERIFY(0x0252012C, dComIfGp_setNextStage);

/* 02520230 */
static s32 dComIfG_changeOpeningScene(u32 i_scene, u32 i_procName) {
    WWHD_FUNC(0x02520230, s32, i_scene, i_procName);
    st8(gi() + 0x514C, 0); /* dComIfGp_offEnableNextStage */
    dComIfGp_setNextStage(0x1004B9C8 /* "sea_T" */, 0, 0x2C, 0, 0.0f, 0, 1, 0);
    u32 ns = gi() + 0x5140;
    s32 room = lds8(gi() + 0x514A);
    s32 layer = lds8(gi() + 0x12A0 + 0x3EAB);
    gabi::call(0x025E17CC, ns, room, layer); /* mDoAud_setSceneName */
    st(savep() + 0x116C, 0);
    gabi::call(0x025DC86C, i_scene, i_procName, 0, 0x1E, 1); /* fopScnM_ChangeReq */
    gabi::call(0x025DC964, i_procName, 0);                    /* fopScnM_ReRequest */
    return 1;
}
VERIFY(0x02520230, dComIfG_changeOpeningScene);

/* 025202F8 */
static s32 dComIfG_resetToOpening(u32 i_scene) {
    WWHD_FUNC(0x025202F8, s32, i_scene);
    if (ld(ld(0x101F4974)) == 0) return 0;
    dComIfG_changeOpeningScene(i_scene, 8);
    gabi::call(0x025E1904, 0x1E);
    gabi::call(0x025E1D40);
    return 1;
}
VERIFY(0x025202F8, dComIfG_resetToOpening);

/* 02520354: HD dComIfG_setObjectRes(name) through a SafeString */
static u32 dComIfG_setObjectRes(u32 i_arcName, u32 p2, u32 p3) {
    WWHD_FUNC(0x02520354, u32, i_arcName, p2, p3);
    gabi::Local<SafeString_l> ss;
    ss->top = i_arcName;
    ss->vt = SS_VT;
    return gabi::call<u32>(0x0260CBB4, ld(0x101F4F54), gabi::ea(ss.get()), 0);
}
VERIFY(0x02520354, dComIfG_setObjectRes);

/* 02520394: phase_1 */
static s32 phase_1(u32 i_arcName) {
    WWHD_FUNC(0x02520394, s32, i_arcName);
    s32 r = 2;
    if (dComIfG_setObjectRes(i_arcName, 0, 0) == 0) r = 5;
    return r;
}
VERIFY(0x02520394, phase_1);

/* 025203D8: HD dComIfG_syncObjectRes(name) */
static s32 dComIfG_syncObjectRes(u32 i_arcName) {
    WWHD_FUNC(0x025203D8, s32, i_arcName);
    gabi::Local<SafeString_l> ss;
    ss->top = i_arcName;
    ss->vt = SS_VT;
    return gabi::call<s32>(0x0260F414, ld(0x101F4F54), gabi::ea(ss.get()));
}
VERIFY(0x025203D8, dComIfG_syncObjectRes);

/* 02520414: phase_2 */
static s32 phase_2(u32 i_arcName) {
    WWHD_FUNC(0x02520414, s32, i_arcName);
    s32 r = dComIfG_syncObjectRes(i_arcName);
    if (r < 0) return 5;
    return r > 0 ? 0 : 2;
}
VERIFY(0x02520414, phase_2);

/* 02520460: dComIfG_resLoad = dComLbG_PhaseHandler(phase, l_method {phase_1, phase_2, phase_3}, name) */
static s32 dComIfG_resLoad(u32 i_phase, u32 i_arcName) {
    WWHD_FUNC(0x02520460, s32, i_phase, i_arcName);
    if (ld(i_phase + 4) == 2) return 4;
    return gabi::call<s32>(0x02525FE4, i_phase, 0x101D5DF4, i_arcName);
}
VERIFY(0x02520460, dComIfG_resLoad);

/* 02520488: HD dComIfG_deleteObjectRes(name) (matcher: dRes_control_c::deleteRes) */
static s32 dComIfG_deleteObjectRes(u32 i_arcName) {
    WWHD_FUNC(0x02520488, s32, i_arcName);
    gabi::Local<SafeString_l> ss;
    ss->top = i_arcName;
    ss->vt = SS_VT;
    gabi::call(0x0260E500, ld(0x101F4F54), gabi::ea(ss.get()));
    return 1;
}
VERIFY(0x02520488, dComIfG_deleteObjectRes);

/* 025204C8 */
static s32 dComIfG_resDelete(u32 i_phase, u32 i_resName) {
    WWHD_FUNC(0x025204C8, s32, i_phase, i_resName);
    if (ld(i_phase + 4) == 1) JUT_ASSERT_l(FILE_, 0x515, 0x1004B9E4 /* "i_phase->id != 1" */);
    if (ld(i_phase + 4) == 2) {
        dComIfG_deleteObjectRes(i_resName);
        st(i_phase + 4, 0);
    }
    return 0;
}
VERIFY(0x025204C8, dComIfG_resDelete);

/* 02520540 */
static u32 dComIfGp_getReverb(u32 i_roomNo) {
    WWHD_FUNC(0x02520540, u32, i_roomNo);
    u32 info = vcall(stage(), 0x64);
    return gabi::call<u32>(0x025C2150, info, i_roomNo);
}
VERIFY(0x02520540, dComIfGp_getReverb);

/* 02520584 */
static u32 dComIfGp_getShip(u32 i_roomNo, u32 i_id) {
    WWHD_FUNC(0x02520584, u32, i_roomNo, i_id);
    u32 roomDt = getStatusRoomDt_l(gi() + 0x51CC, i_roomNo);
    if (roomDt == 0) return 0;
    u32 ship = vcall(roomDt, 0x1FC);
    if (ship == 0) return 0;
    s32 n = (s32)ld(ship + 0);
    if (n <= 0) return 0;
    if (i_id == 0xFF) return 0;
    u32 p = ld(ship + 4);
    if (p == 0) return 0;
    for (s32 i = 0; i < n; i++, p += 0x10)
        if (i_id == ld8(p + 0xE)) return p;
    return 0;
}
VERIFY(0x02520584, dComIfGp_getShip);

/* 02520630 */
static u32 dComIfGp_getMapTrans(u32 i_roomNo, u32 o_transX, u32 o_transY, u32 o_angle) {
    WWHD_FUNC(0x02520630, u32, i_roomNo, o_transX, o_transY, o_angle);
    u32 multi = vcall(stage(), 0x20C);
    if (multi == 0) return 0;
    s32 n = (s32)ld(multi + 0);
    u32 p = ld(multi + 4);
    for (s32 i = 0; i < n; i++, p += 0xC) {
        if (i_roomNo == ld8(p + 0xA)) {
            stf(o_transX, ldf(p + 0));
            stf(o_transY, ldf(p + 4));
            st16(o_angle, (u16)lds16(p + 8));
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02520630, dComIfGp_getMapTrans);

/* 02520708 */
static u32 dComIfGp_getRoomCamera(u32 i_roomNo) {
    WWHD_FUNC(0x02520708, u32, i_roomNo);
    u32 roomDt = getStatusRoomDt_l(gi() + 0x51CC, i_roomNo);
    if (roomDt == 0) return 0;
    return vcall(roomDt, 0x24);
}
VERIFY(0x02520708, dComIfGp_getRoomCamera);

/* 02520770 */
static u32 dComIfGp_getRoomArrow(u32 i_roomNo) {
    WWHD_FUNC(0x02520770, u32, i_roomNo);
    u32 roomDt = getStatusRoomDt_l(gi() + 0x51CC, i_roomNo);
    if (roomDt == 0) return 0;
    return vcall(roomDt, 0x34);
}
VERIFY(0x02520770, dComIfGp_getRoomArrow);

static inline u32 curStageNo() { return (ld8(getStagInfo() + 9) >> 1) & 0x7F; }

/* 025207D8 */
static void dComIfGs_onStageTbox(u32 i_stageNo, u32 i_no) {
    WWHD_FUNC(0x025207D8, void, i_stageNo, i_no);
    if (i_stageNo == curStageNo()) dSv_memBit_onTbox_l(savep() + 0x798, i_no);
    dSv_memBit_onTbox_l(savep() + 0x3A0 + i_stageNo * 0x24, i_no);
}
VERIFY(0x025207D8, dComIfGs_onStageTbox);

/* 02520864 */
static u32 dComIfGs_isStageTbox(u32 i_stageNo, u32 i_no) {
    WWHD_FUNC(0x02520864, u32, i_stageNo, i_no);
    if (i_stageNo == curStageNo()) return dSv_memBit_isTbox_l(savep() + 0x798, i_no);
    return dSv_memBit_isTbox_l(savep() + 0x3A0 + i_stageNo * 0x24, i_no);
}
VERIFY(0x02520864, dComIfGs_isStageTbox);

static inline u32 isStageDungeonItem(u32 i_stageNo, s32 item) {
    u32 cur = curStageNo();
    u32 sv = savep();
    if (i_stageNo == cur) return dSv_memBit_isDungeonItem_l(sv + 0x798, item);
    return dSv_memBit_isDungeonItem_l(sv + 0x3A0 + i_stageNo * 0x24, item);
}

/* 025208F8: HD out-of-line dComIfGs_isStageMap */
static u32 dComIfGs_isStageMap(u32 i_stageNo) {
    WWHD_FUNC(0x025208F8, u32, i_stageNo);
    return isStageDungeonItem(i_stageNo, 0);
}
VERIFY(0x025208F8, dComIfGs_isStageMap);

/* 0252097C: HD out-of-line dComIfGs_isStageCompass */
static u32 dComIfGs_isStageCompass(u32 i_stageNo) {
    WWHD_FUNC(0x0252097C, u32, i_stageNo);
    return isStageDungeonItem(i_stageNo, 1);
}
VERIFY(0x0252097C, dComIfGs_isStageCompass);

/* 02520A00: HD out-of-line dComIfGs_isStageBossKey */
static u32 dComIfGs_isStageBossKey(u32 i_stageNo) {
    WWHD_FUNC(0x02520A00, u32, i_stageNo);
    return isStageDungeonItem(i_stageNo, 2);
}
VERIFY(0x02520A00, dComIfGs_isStageBossKey);

/* 02520A84 */
static u32 dComIfGs_isStageBossEnemy(u32 i_stageNo) {
    WWHD_FUNC(0x02520A84, u32, i_stageNo);
    return isStageDungeonItem(i_stageNo, 3);
}
VERIFY(0x02520A84, dComIfGs_isStageBossEnemy);

/* 02520B08 */
static void dComIfGs_onStageLife(u32 i_stageNo) {
    WWHD_FUNC(0x02520B08, void, i_stageNo);
    if (i_stageNo == curStageNo()) dSv_memBit_onDungeonItem_l(savep() + 0x798, 4);
    dSv_memBit_onDungeonItem_l(savep() + 0x3A0 + i_stageNo * 0x24, 4);
}
VERIFY(0x02520B08, dComIfGs_onStageLife);

/* 02520B88 */
static u32 dComIfGs_isStageLife(u32 i_stageNo) {
    WWHD_FUNC(0x02520B88, u32, i_stageNo);
    return isStageDungeonItem(i_stageNo, 4);
}
VERIFY(0x02520B88, dComIfGs_isStageLife);

/* dComIfGs_getItem(i) as inlined in HD: item slots save+0x5C (0..0x14), bag slots (0x18..0x1F at
 * save+0x96, 0x24..0x2B at save+0x9E, 0x30..0x37 at save+0xA6), 0xFF otherwise */
static inline u8 getItem_l(u32 sv, s32 i) {
    if (i < 0x15) return ld8(sv + 0x5C + i);
    if (i < 0x18) return 0xFF;
    if (i < 0x20) return ld8(sv + 0x7E + i);
    if (i < 0x24) return 0xFF;
    if (i < 0x2C) return ld8(sv + 0x7A + i);
    if ((u32)(i - 0x30) < 8) return ld8(sv + 0x76 + i);
    return 0xFF;
}
static inline u32 collect() { return savep() + 0xD4; }
static inline u32 isTact_l(s32 i) { return gabi::call<u32>(0x025B7B10, collect(), i); }
static inline u32 isTriforce_l(s32 i) { return gabi::call<u32>(0x025B7C50, collect(), i); }
static inline u32 isSymbol_l(s32 i) { return gabi::call<u32>(0x025B7D90, collect(), i); }
static inline u32 isCollect_l(s32 i, s32 j) { return gabi::call<u32>(0x025B7A2C, collect(), i, j); }

/* switch cases shared by checkGetItem / checkGetItemNum: -1 when not one of them */
static inline s32 checkSpecial(u32 item) {
    if (item == 0x42) return isCollect_l(3, 0) != 0;
    if (item == 0x43) return isCollect_l(4, 0) != 0;
    if (item >= 0x61 && item <= 0x72) {
        u32 k = item - 0x61;
        if (k < 8) return isTriforce_l(k) != 0;
        if (k < 0xB) return isSymbol_l(k - 8) != 0;
        if (k >= 0xC) return isTact_l(k - 0xC) != 0;
    }
    return -1;
}

/* 02520C0C */
static u8 dComIfGs_checkGetItem(u32 i_itemNo) {
    WWHD_FUNC(0x02520C0C, u8, i_itemNo);
    s32 sp = checkSpecial(i_itemNo);
    if (sp >= 0) return (u8)sp;
    u8 get_item = 0;
    u32 sv = savep();
    for (s32 i = 0; i < 0x3C; i++)
        if (getItem_l(sv, i) == i_itemNo) get_item++;
    for (u32 i = 0; i < 3; i++)
        if (ld8(sv + 0x2E + i) == i_itemNo) get_item++;
    if (i_itemNo - 0xBF < 0x40 && gabi::call<u32>(0x025B7F68, sv + 0xE4, 0xFE - i_itemNo) != 0) get_item++;
    return get_item;
}
VERIFY(0x02520C0C, dComIfGs_checkGetItem);

/* 02520FE4 */
static u8 dComIfGs_checkGetItemNum(u32 i_itemNo) {
    WWHD_FUNC(0x02520FE4, u8, i_itemNo);
    s32 sp = checkSpecial(i_itemNo);
    if (sp >= 0) return (u8)sp;
    u8 get_item = 0;
    if (i_itemNo == 0x27) { /* bow: arrows */
        u32 sv = savep();
        if (ld8(sv + 0x68) != 0) get_item = ld8(sv + 0x89);
        return get_item;
    }
    if (i_itemNo == 0x31) { /* bomb bag: bombs */
        u32 sv = savep();
        if (ld8(sv + 0x69) != 0) get_item = ld8(sv + 0x8A);
        return get_item;
    }
    s32 beast = -1;
    if (i_itemNo >= 0x45 && i_itemNo <= 0x4B) beast = i_itemNo - 0x45;
    else if (i_itemNo == 0x1F) beast = 7;
    if (beast >= 0) {
        u32 sv = savep();
        for (u32 i = 0; i < 8; i++)
            if (ld8(sv + 0x96 + i) == i_itemNo) get_item = ld8(sv + 0xBC + beast);
        return get_item;
    }
    u32 sv = savep();
    for (s32 i = 0; i < 0x3C; i++)
        if (getItem_l(sv, i) == i_itemNo) get_item = 1;
    for (u32 i = 0; i < 3; i++)
        if (ld8(sv + 0x2E + i) == i_itemNo) get_item = 1;
    if (i_itemNo - 0xBF < 0x40 && gabi::call<u32>(0x025B7F68, sv + 0xE4, 0xFE - i_itemNo) != 0) get_item = 1;
    return get_item;
}
VERIFY(0x02520FE4, dComIfGs_checkGetItemNum);

/* 025215BC */
static u32 getSceneList(s32 i_no) {
    WWHD_FUNC(0x025215BC, u32, i_no);
    u32 sclsInfo = vcall(stage(), 0x16C);
    if (sclsInfo == 0) JUT_ASSERT_l(0x1004BA40, 0x96D, 0x1004B9FC /* "sclsInfo != (0)" */);
    if (!(i_no >= 0 && i_no < (s32)ld(sclsInfo + 0)))
        JUT_ASSERT_l(0x1004BA40, 0x96E, 0x1004BA1C /* "0 <= i_no && i_no < sclsInfo->num" */);
    u32 sclsData = ld(sclsInfo + 4);
    if (sclsData == 0) JUT_ASSERT_l(0x1004BA40, 0x971, 0x1004BA0C /* "sclsData != (0)" */);
    return sclsData + (u32)i_no * 0xC;
}
VERIFY(0x025215BC, getSceneList);

/* 02521678 */
static u32 dComIfGd_getMeshSceneList(u32 i_vec) {
    WWHD_FUNC(0x02521678, u32, i_vec);
    f32 fx = ldf(i_vec + 0);
    f32 fz = ldf(i_vec + 8);
    s32 x = gabi::ftoi((fx + 350000.0f) * 2e-05f);
    s32 z = gabi::ftoi((fz + 350000.0f) * 2e-05f);
    if (x < 0) x = 0;
    else if (x > 13) x = 13;
    if (z < 0) z = 0;
    else if (z > 13) z = 13;
    return getSceneList((x & 1) + ((x >> 1) + (z >> 1) * 7) * 4 + (z & 1) * 2);
}
VERIFY(0x02521678, dComIfGd_getMeshSceneList);

/* 0252174C: l_landingEvent at 101D5E00 */
static u32 dComIfGs_checkSeaLandingEvent(u32 i_roomNo) {
    WWHD_FUNC(0x0252174C, u32, i_roomNo);
    u32 e = 0x101D5E00;
    for (u32 i = 0; i < 6; i++, e += 4) {
        if (i_roomNo == (u32)(s32)lds8(e) && isEventBit_l(savep() + 0x644, ld16(e + 2)) == 0) return 0;
    }
    return 1;
}
VERIFY(0x0252174C, dComIfGs_checkSeaLandingEvent);

/* SafeString temporaries compared as in the HD strcmp: a's assureTermination twice (the first
 * devirtualised to f), then b's */
static inline bool ss_cmp_eq(u32 a, u32 b, u32 first_direct) {
    if (first_direct) gabi::call(first_direct, a);
    else gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    u32 ta = ld(a + 0);
    gabi::call_ptr(ld(ld(b + 4) + 0x14), b);
    u32 tb = ld(b + 0);
    return ss_equal(ta, tb);
}

/* 025217F8: l_checkData at 101D5E18 (0xE-byte entries) */
static void dComIfGs_setGameStartStage() {
    WWHD_FUNC(0x025217F8, void);
    gabi::Local<u8[8]> name;
    u32 buf = gabi::ea(name.get());
    u32 d = 0x101D5E18;
    for (u32 i = 0; i < 5; i++, d += 0xE) {
        if (ld8(d) == 1 && isEventBit_l(savep() + 0x644, ld16(d + 2)) != 0) break;
    }
    bool emptyName;
    {
        gabi::Local<SafeString_l> a;
        gabi::Local<SafeString_l> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        st(aa + 4, SS_VT);
        st(bb + 0, 0x1004BA6C /* "" */);
        st(aa + 0, d + 4);
        st(bb + 4, SS_VT);
        /* the first assureTermination: direct call to the inline copy, then the virtual */
        gabi::call(0x02525C34, aa);
        gabi::call_ptr(ld(ld(aa + 4) + 0x14), aa);
        u32 ta = ld(aa + 0);
        gabi::call_ptr(ld(ld(bb + 4) + 0x14), bb);
        u32 tb = ld(bb + 0);
        emptyName = ss_equal(ta, tb);
    }
    if (!emptyName) {
        strncpy_l(buf, d + 4, 8);
        s32 room = lds8(d + 0xC);
        u32 point = ld8(d + 0xD);
        returnPlace_set_l(savep() + 0x50, buf, room, point);
        return;
    }
    u32 stage_type = (ld(getStagInfo() + 0xC) >> 16) & 7;
    u32 stageNo = curStageNo();
    bool pship;
    {
        gabi::Local<SafeString_l> a;
        gabi::Local<SafeString_l> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        st(aa + 4, SS_VT);
        st(aa + 0, 0x1004BA64 /* "PShip" */);
        u32 g = gi();
        st(bb + 0, g + 0x5134);
        st(bb + 4, SS_VT);
        pship = ss_cmp_eq(aa, bb, 0);
    }
    u32 sea = 0x1004BA60; /* "sea" */
    if (pship) {
        strncpy_l(buf, sea, 8);
        s32 room = (s8)getEventReg_l(savep() + 0x644, 0xC3FF);
        u32 point = getEventReg_l(savep() + 0x644, 0x85FF);
        gabi::call(0x02560728, 120.0f); /* dKy_set_nexttime */
        returnPlace_set_l(savep() + 0x50, buf, room, point);
        return;
    }
    if (stage_type == 7) { /* sea */
        u32 player = ld(gi() + 0x5B34);
        s32 room = lds8(player + 0x326);
        u32 point = ld8(player + 0x69E7);
        if (room >= 0 && point != 0xFF && dComIfGs_checkSeaLandingEvent((u32)room) != 0) {
            u32 g = gi();
            strncpy_l(buf, g + 0x5134, 7);
            st8(buf + 7, 0);
            returnPlace_set_l(savep() + 0x50, buf, room, point);
            return;
        }
        u32 ship = ld(gi() + 0x5B3C);
        u32 scls = dComIfGd_getMeshSceneList(ship != 0 ? ship + 0x314 : player + 0x314);
        strncpy_l(buf, scls, 8);
        s32 sroom = lds8(scls + 9);
        returnPlace_set_l(savep() + 0x50, buf, sroom, ld8(scls + 8));
        return;
    }
    if (stage_type == 1 || stage_type == 3 || stage_type == 6 || stage_type == 8 || stageNo == 9) {
        u32 scls = getSceneList(0);
        strncpy_l(buf, scls, 8);
        s32 sroom = lds8(scls + 9);
        returnPlace_set_l(savep() + 0x50, buf, sroom, ld8(scls + 8));
        return;
    }
    if (stageNo == 0xA) { /* ship: the raft's position before boarding */
        gabi::Local<u32[3]> pos;
        u32 p = gabi::ea(pos.get());
        u32 g = gi();
        st(p + 0, ld(g + 0x5D18));
        st(p + 4, ld(g + 0x5D1C));
        st(p + 8, ld(g + 0x5D20));
        u32 scls = dComIfGd_getMeshSceneList(p);
        strncpy_l(buf, scls, 8);
        s32 sroom = lds8(scls + 9);
        returnPlace_set_l(savep() + 0x50, buf, sroom, ld8(scls + 8));
        return;
    }
    if (stageNo - 0xB <= 2) {
        strncpy_l(buf, sea, 8);
        u32 mapInfo = vcall(stage(), 0x74);
        if (mapInfo == 0) JUT_ASSERT_l(0x1004BA70, 0xA78, 0x1004BA84 /* "mapInfo != (0)" */);
        u32 oz = gabi::call<u32>(0x025C1BB8, mapInfo);
        u32 t = oz * 7 + 0x15;
        u32 ox = gabi::call<u32>(0x025C1BA0, mapInfo);
        s32 room = (s8)(t + (ox + 3) + 1);
        returnPlace_set_l(savep() + 0x50, buf, room, 0);
        return;
    }
    strncpy_l(buf, sea, 8);
    returnPlace_set_l(savep() + 0x50, buf, 0xB, 0);
}
VERIFY(0x025217F8, dComIfGs_setGameStartStage);

/* 02522198 */
static void dComIfGs_gameStart() {
    WWHD_FUNC(0x02522198, void);
    st8(gi() + 0x514C, 0);
    u32 sv = savep();
    s32 room = lds8(sv + 0x58);
    u32 point = ld8(sv + 0x59);
    dComIfGp_setNextStage(sv + 0x50, point, (u32)room, 0xFFFFFFFF, 0.0f, 0, 1, 0);
}
VERIFY(0x02522198, dComIfGs_gameStart);

/* 025221F0: the boss stage's recollection table: DRC 3 -> 0, FW 4 -> 1, ET 6 -> 2, WT 7 -> 3 */
static void dComIfGs_copyPlayerRecollectionData() {
    WWHD_FUNC(0x025221F0, void);
    if (((ld(getStagInfo() + 0xC) >> 16) & 7) != 3) return;
    u32 tbl;
    if (curStageNo() == 3) tbl = 0;
    else if (curStageNo() == 4) tbl = 1;
    else if (curStageNo() == 6) tbl = 2;
    else if (curStageNo() == 7) tbl = 3;
    else return;
    gabi::Local<u8[0x70]> stts;
    u32 b = gabi::ea(stts.get());
    memcpy_l(b + 0, savep() + 0x20, 0x18);
    memcpy_l(b + 0x18, savep() + 0x5C, 0x15);
    memcpy_l(b + 0x2D, savep() + 0x88, 3);
    memcpy_l(b + 0x30, savep() + 0x8E, 3);
    memcpy_l(b + 0x33, savep() + 0x96, 0x18);
    memcpy_l(b + 0x4B, savep() + 0xBC, 0x18);
    memcpy_l(b + 0x63, savep() + 0xD4, 0xD);
    memcpy_l(savep() + 0x1DC + tbl * 0x70, b, 0x70);
}
VERIFY(0x025221F0, dComIfGs_copyPlayerRecollectionData);

/* 02522398 */
static void dComIfGs_setSelectEquip(u32 i_type, u32 i_itemNo) {
    WWHD_FUNC(0x02522398, void, i_type, i_itemNo);
    if (i_type > 2) {
        st8(savep() + i_type + 0x2E, (u8)i_itemNo);
        return;
    }
    s32 bit = -1;
    if (i_type == 0) {
        if (i_itemNo >= 0x38 && i_itemNo <= 0x3A) bit = ld8(0x1004BA5C + i_itemNo);
        else if (i_itemNo == 0x3E) bit = 3;
    } else if (i_type == 1) {
        if (i_itemNo == 0x3B) bit = 0;
        else if (i_itemNo == 0x3C) bit = 1;
    } else {
        if (i_itemNo == 0x28) bit = 0;
    }
    if (bit >= 0) gabi::call(0x025B7944, savep() + 0xD4, i_type, bit); /* onCollect */
    st8(savep() + i_type + 0x2E, (u8)i_itemNo);
}
VERIFY(0x02522398, dComIfGs_setSelectEquip);

/* the recollected status: copy the current status into the play buffer (gi + 0x5C60) and the
 * recollection (save + 0x1DC + tbl * 0x70) into the current status */
static inline void saveStatusToBuffer(u32 buf) {
    memcpy_l(buf, savep() + 0x20, 0x18);
    buf += 0x18;
    memcpy_l(buf, savep() + 0x5C, 0x15);
    buf += 0x15;
    memcpy_l(buf, savep() + 0x88, 3);
    buf += 3;
    memcpy_l(buf, savep() + 0x8E, 3);
    buf += 3;
    memcpy_l(buf, savep() + 0x96, 0x18);
    buf += 0x18;
    memcpy_l(buf, savep() + 0xBC, 0x18);
    memcpy_l(buf + 0x18, savep() + 0xD4, 0xD);
}
static inline void loadStatusFromBuffer(u32 src) {
    memcpy_l(savep() + 0x20, src, 0x18);
    src += 0x18;
    memcpy_l(savep() + 0x5C, src, 0x15);
    src += 0x15;
    memcpy_l(savep() + 0x88, src, 3);
    src += 3;
    memcpy_l(savep() + 0x8E, src, 3);
    src += 3;
    memcpy_l(savep() + 0x96, src, 0x18);
    src += 0x18;
    memcpy_l(savep() + 0xBC, src, 0x18);
    memcpy_l(savep() + 0xD4, src + 0x18, 0xD);
}
static inline u32 pictureInfo() { return gabi::call<u32>(0x02720144, savep() + 0x12C0); }

/* 025224A4 */
static void dComIfGs_setPlayerRecollectionData() {
    WWHD_FUNC(0x025224A4, void);
    gabi::call(0x02526118, 0); /* daArrow_c::setKeepType(TYPE_NORMAL) */
    static const u32 names[4] = {0x1004BA98, 0x1004BAA0, 0x1004BAA8, 0x1004BAB0}; /* "Xboss0".."Xboss3" */
    static const u16 events[4] = {0x3D80, 0x3D40, 0x3D20, 0x3D10};
    s32 tbl = -1;
    for (u32 k = 0; k < 4 && tbl < 0; k++) {
        gabi::Local<SafeString_l> a;
        gabi::Local<SafeString_l> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        st(aa + 0, names[k]);
        st(aa + 4, SS_VT);
        u32 g = gi();
        st(bb + 0, g + 0x5134);
        st(bb + 4, SS_VT);
        if (ss_cmp_eq(aa, bb, 0) && isEventBit_l(savep() + 0x644, events[k]) != 0) tbl = (s32)k;
    }
    if (tbl < 0) {
        u32 sv = savep();
        st8(sv + 0x29, 0xFF);
        st8(savep() + 0x2A, 0xFF);
        st8(savep() + 0x2B, 0xFF);
        return;
    }
    st8(gi() + 0x5CD0, (u8)(tbl + 1)); /* dComIfGp_setPlayerInfoBufferStageNo */
    u32 sv = savep();
    u32 off = (u32)tbl * 0x70;
    if (ld8(sv + 0x1F4 + off) != 0x20) { /* no telescope in the recollection */
        st8(sv + 0x20 + 9, 0xFF);
        st8(savep() + 0x2A, 0xFF);
        st8(savep() + 0x2B, 0xFF);
        return;
    }
    u8 v34 = ld8(sv + 0x34), v8f = ld8(sv + 0x8F), v89 = ld8(sv + 0x89), v90 = ld8(sv + 0x90);
    u8 maxLife = ld8(sv + 0x21), v33 = ld8(sv + 0x33), v8a = ld8(sv + 0x8A);
    u16 life = ld16(sv + 0x22);
    u8 picNum = ld8(pictureInfo() + 0x3C030C);
    sv = savep();
    u8 eq[5];
    for (u32 i = 0; i < 5; i++) eq[i] = ld8(sv + 0x2E + i);
    u8 bot[4] = {ld8(sv + 0x6A), ld8(sv + 0x6B), ld8(sv + 0x6C), ld8(sv + 0x6D)};
    u8 cam = ld8(sv + 0x64);
    u8 col[5];
    for (u32 i = 0; i < 5; i++) col[i] = ld8(sv + 0xD4 + i);
    saveStatusToBuffer(gi() + 0x5C60);
    loadStatusFromBuffer(savep() + 0x1DC + off);
    st16(savep() + 0x20, maxLife);
    st16(savep() + 0x22, life);
    st8(savep() + 0x33, v33);
    st8(savep() + 0x34, v34);
    st8(savep() + 0x8F, v8f);
    st8(savep() + 0x89, v89);
    st8(savep() + 0x90, v90);
    st8(savep() + 0x8A, v8a);
    gabi::call(0x027262B0, pictureInfo(), picNum);
    for (u32 i = 0; i < 5; i++) st8(savep() + 0xD4 + i, col[i]);
    for (u32 i = 0; i < 5; i++) dComIfGs_setSelectEquip(i, eq[i]);
    for (u32 i = 0; i < 5; i++) {
        u8 e = ld8(savep() + 0x2E + i);
        st8(gi() + 0x5BC0 + i, e);
    }
    for (u32 i = 0; i < 4; i++) st8(savep() + 0x6A + i, bot[i]);
    st8(savep() + 0x64, cam);
    st8(savep() + 0x29, 0xFF);
    st8(savep() + 0x2A, 0xFF);
    st8(savep() + 0x2B, 0xFF);
}
VERIFY(0x025224A4, dComIfGs_setPlayerRecollectionData);

/* 02522C60 */
static void dComIfGs_revPlayerRecollectionData() {
    WWHD_FUNC(0x02522C60, void);
    if (ld8(gi() + 0x5CD0) == 0) return;
    gi();
    st8(gi() + 0x5CD0, 0);
    u32 sv = savep();
    u8 v90 = ld8(sv + 0x90), v33 = ld8(sv + 0x33), v8f = ld8(sv + 0x8F);
    u16 life = ld16(sv + 0x22);
    u8 v34 = ld8(sv + 0x34);
    u16 maxLife = ld16(sv + 0x20);
    u8 v89 = ld8(sv + 0x89), v8a = ld8(sv + 0x8A);
    u8 picNum = ld8(pictureInfo() + 0x3C030C);
    sv = savep();
    u8 eq[5];
    for (u32 i = 0; i < 5; i++) eq[i] = ld8(sv + 0x2E + i);
    u8 col[5];
    for (u32 i = 0; i < 5; i++) col[i] = ld8(sv + 0xD4 + i);
    u8 bot[4] = {ld8(sv + 0x6A), ld8(sv + 0x6B), ld8(sv + 0x6C), ld8(sv + 0x6D)};
    u8 cam = ld8(sv + 0x64);
    loadStatusFromBuffer(gi() + 0x5C60);
    st16(savep() + 0x20, (u8)maxLife);
    st16(savep() + 0x22, life);
    st8(savep() + 0x33, v33);
    st8(savep() + 0x34, v34);
    st8(savep() + 0x8F, v8f);
    st8(savep() + 0x89, v89);
    st8(savep() + 0x90, v90);
    st8(savep() + 0x8A, v8a);
    gabi::call(0x027262B0, pictureInfo(), picNum);
    for (u32 i = 0; i < 5; i++) st8(savep() + 0xD4 + i, col[i]);
    for (u32 i = 0; i < 5; i++) dComIfGs_setSelectEquip(i, eq[i]);
    for (u32 i = 0; i < 5; i++) {
        u8 e = ld8(savep() + 0x2E + i);
        st8(gi() + 0x5BC0 + i, e);
    }
    for (u32 i = 0; i < 4; i++) st8(savep() + 0x6A + i, bot[i]);
    st8(savep() + 0x64, cam);
    st8(savep() + 0x29, 0xFF);
    st8(savep() + 0x2A, 0xFF);
    st8(savep() + 0x2B, 0xFF);
}
VERIFY(0x02522C60, dComIfGs_revPlayerRecollectionData);

/* 02522F50: HD, resource control flag (*(101F4F54) + 0xD8) != 0 */
static u32 dComIfG_isResFlag() {
    WWHD_FUNC(0x02522F50, u32);
    return ld(ld(0x101F4F54) + 0xD8) != 0 ? 1 : 0;
}
VERIFY(0x02522F50, dComIfG_isResFlag);

} // namespace d_com_inf_game_cpp
