/* f_op_actor: actor process methods (fopAc), WWHD. 
 *
 * Translation unit 025D4604..025D51D8: fopAc_IsActor, Draw, Execute, IsDelete, Delete, Create,
 * the fopAc_ac_c constructor and destructor, and the unit's __sinit (025D5148, header statics).
 * f_ap_game ends with its __sinit at 025D4564 before it; f_op_actor_iter follows at 025D51DC.
 * Ported from the GameCube f_op_actor.cpp (release build); the constructor, destructor and
 * __sinit are written from the WWHD code.
 *
 * Layout (HD, fopAc_ac_c 0x3AC): actor_type +0xC4, actor_tag +0xC8, draw_tag +0xDC, sub_method
 * +0xF0, C++ vtable +0xB4, eventInfo +0xF8 (mCommand +0xFA), tevStr +0x110 (three copies of a
 * 0x44-byte light block at +0x110/+0x1D0/+0x254 initialised by the constructor), status +0x2E0,
 * condition +0x2E4, attention distances +0x388, position +0x390. HD-only: a u8 construction
 * state at +0x3AA (0 raw, 1 constructed, 2 destroyed) that Delete uses to run the virtual
 * destructor. Condition bits: 2 NOEXEC, 4 NODRAW (GameCube 0x10). */
#include "bindings.h"

namespace f_op_actor_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline BOOL fpcBs_Is_JustOfType_l(s32 a, s32 b) { return gabi::call<BOOL>(0x025DD258, a, b); }
static inline s32 fpcBs_MakeOfType_l(u32 p) { return gabi::call<s32>(0x025DD268, p); }
static inline BOOL dMenu_flag_l() { return gabi::call<BOOL>(0x025986BC); }
static inline BOOL dScnPly_isPause_l() { return gabi::call<BOOL>(0x025AF2A4); } /* HD: d_s_play */
static inline s32 dEvt_control_moveApproval_l(u32 ev, u32 a) { return gabi::call<s32>(0x02540158, ev, a); }
static inline BOOL fopAcM_cullingCheck_l(u32 a) { return gabi::call<BOOL>(0x025D6CE8, a); }
static inline s32 fpcLf_DrawMethod_l(u32 m, u32 a) { return gabi::call<s32>(0x025DF2C0, m, a); }
static inline s32 fpcMtd_Execute_l(u32 m, u32 a) { return gabi::call<s32>(0x025DFCC4, m, a); }
static inline s32 fpcMtd_IsDelete_l(u32 m, u32 a) { return gabi::call<s32>(0x025DFCCC, m, a); }
static inline s32 fpcMtd_Delete_l(u32 m, u32 a) { return gabi::call<s32>(0x025DFCD4, m, a); }
static inline s32 fpcMtd_Create_l(u32 m, u32 a) { return gabi::call<s32>(0x025DFCDC, m, a); }
static inline void dMap_drawActorPointMiniMap_l(u32 a) { gabi::call(0x0259029C, a); }
static inline BOOL fopAcM_delete_l(u32 a) { return gabi::call<BOOL>(0x025D57E0, a); }
static inline void fopAcTg_Init_l(u32 tag, u32 a) { gabi::call(0x025DA30C, tag, a); }
static inline void fopAcTg_ToActorQ_l(u32 tag) { gabi::call(0x025DA2F8, tag); }
static inline void fopAcTg_ActorQTo_l(u32 tag) { gabi::call(0x025DA308, tag); }
static inline void fopDwTg_Init_l(u32 tag, u32 a) { gabi::call(0x025DA888, tag, a); }
static inline void fopDwTg_ToDrawQ_l(u32 tag, s32 prio) { gabi::call(0x025DA874, tag, prio); }
static inline void fopDwTg_DrawQTo_l(u32 tag) { gabi::call(0x025DA884, tag); }
static inline s32 fpcM_DrawPriority_l(u32 a) { return gabi::call<s32>(0x025DF2B8, a); }
static inline void fopAcM_DeleteHeap_l(u32 a) { gabi::call(0x025D6134, a); }
static inline u32 dDemo_object_getActor_l(u32 obj, u8 id) { return gabi::call<u32>(0x02526E70, obj, id); }
static inline void dDemo_actor_setActor_l(u32 da, u32 a) { gabi::call(0x025277D8, da, a); }
static inline void mDoAud_seDeleteObject_l(u32 pos) { gabi::call(0x025E1B34, pos); }
static inline void dKy_tevstr_init_l(u32 tev, s8 room, u8 c) { gabi::call(0x0255FFF4, tev, room, c); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline void base_process_ct_l(u32 p) { gabi::call(0x025DD5F0, p); }        /* leafdraw base constructor */
static inline void base_process_dt_l(u32 p, s32 f) { gabi::call(0x025DD630, p, f); }
static inline void dEvt_info_c_ct_l(u32 p) { gabi::call(0x0253E948, p); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 g_fopAc_type = 0x101F3080;
static constexpr u32 stopStatus = 0x101F3084;       /* fopAc_ac_c::stopStatus */
static constexpr u32 stayNo = 0x1047E6C8;           /* dStage_roomControl_c::mStayNo (s8) */
static constexpr u32 FILE_STR = 0x10056E7C;         /* "f_op_actor.cpp" */

enum {
    ST_NOPAUSE = 0x20000, ST_NOCULLEXEC = 0x80, ST_CULL = 0x100, ST_NODRAW = 0x01000000,
    ST_SHOWMAP = 0x20, ST_NOFALLDEL = 0x04000000,
    CND_NOEXEC = 2, CND_NODRAW = 4,
};

/* 025D4604: HD: null-safe */
static BOOL fopAc_IsActor(u32 pProc) {
    WWHD_FUNC(0x025D4604, BOOL, pProc);
    BOOL ret = FALSE;
    if (pProc != 0 && fpcBs_Is_JustOfType_l(ld(g_fopAc_type), ld(pProc + 0xC4)) != 0) ret = TRUE;
    return ret;
}
VERIFY(0x025D4604, fopAc_IsActor);

/* 025D4654 */
static s32 fopAc_Draw(u32 actor) {
    WWHD_FUNC(0x025D4654, s32, actor);
    s32 ret = 1;
    if (!dMenu_flag_l()) {
        s32 moveApproval = dEvt_control_moveApproval_l(dComIfGp_ea() + 0x51D0, actor);
        bool draw;
        if (moveApproval == 2) {
            draw = !(ld(actor + 0x2E0) & ST_NODRAW);
        } else {
            u32 s = ld(actor + 0x2E0);
            if (s & ld(stopStatus)) draw = false;
            else if ((s & ST_CULL) && fopAcM_cullingCheck_l(actor)) draw = false;
            else draw = !(ld(actor + 0x2E0) & ST_NODRAW);
        }
        u32 s;
        if (draw) {
            st(actor + 0x2E4, ld(actor + 0x2E4) & ~CND_NODRAW);
            ret = fpcLf_DrawMethod_l(ld(actor + 0xF0), actor);
            s = ld(actor + 0x2E0) & ~ST_NODRAW;
            st(actor + 0x2E0, s);
        } else {
            u32 cnd = ld(actor + 0x2E4);
            s = ld(actor + 0x2E0) & ~ST_NODRAW;
            st(actor + 0x2E4, cnd | CND_NODRAW);
            st(actor + 0x2E0, s);
        }
        if ((s8)ld8(stayNo) >= 0 && (s & ST_SHOWMAP)) dMap_drawActorPointMiniMap_l(actor);
    }
    return ret;
}
VERIFY(0x025D4654, fopAc_Draw);

static inline bool isnan_bits(u32 a) { return (u32)(ld(a) << 1) > 0xFF000000u; }

/* the release asserts on current.pos (GameCube JUT_ASSERT !isnan + CHECK_VEC3_RANGE) */
static void assert_pos(u32 actor, s32 l0, s32 lr) {
    if (isnan_bits(actor + 0x314)) JUT_ASSERT_l(FILE_STR, l0, 0x10056E8C);
    if (isnan_bits(actor + 0x318)) JUT_ASSERT_l(FILE_STR, l0 + 1, 0x10056EE8);
    if (isnan_bits(actor + 0x31C)) JUT_ASSERT_l(FILE_STR, l0 + 2, 0x10056F44);
    const f32 lo = -1.0e32f, hi = 1.0e32f;
    f32 x = ldf(actor + 0x314);
    bool ok = lo < x && x < hi;
    if (ok) {
        f32 y = ldf(actor + 0x318);
        ok = lo < y && y < hi;
        if (ok) {
            f32 z = ldf(actor + 0x31C);
            ok = lo < z && z < hi;
        }
    }
    if (!ok) JUT_ASSERT_l(FILE_STR, lr, 0x10056FA0);
}

/* 025D475C (matcher: CHECK_VEC3_RANGE) HD: an enemy (group 2) that falls 15000 below the player is deleted */
static BOOL fopAc_Execute(u32 actor) {
    WWHD_FUNC(0x025D475C, BOOL, actor);
    BOOL ret = TRUE;
    assert_pos(actor, 0x2A4, 0x2A8);
    if (!(ld(actor + 0x2E0) & ST_NOPAUSE)) {
        if (dMenu_flag_l()) return ret;
        if (dScnPly_isPause_l()) return ret;
    }
    st16(actor + 0xFA, 0); /* eventInfo.beforeProc() */
    s32 moveApproval = dEvt_control_moveApproval_l(dComIfGp_ea() + 0x51D0, actor);
    bool exec;
    if (moveApproval == 2) {
        exec = true;
    } else if (moveApproval == 0 || (ld(actor + 0x2E0) & ld(stopStatus))) {
        exec = false;
    } else if (ld(actor + 0x2E0) & ST_NOCULLEXEC) {
        exec = !(ld(actor + 0x2E4) & CND_NODRAW);
    } else {
        exec = true;
    }
    if (exec) {
        for (u32 i = 0; i < 0x14; i += 4) st(actor + 0x300 + i, ld(actor + 0x314 + i)); /* old = current */
        st(actor + 0x2E4, ld(actor + 0x2E4) & ~CND_NOEXEC);
        ret = fpcMtd_Execute_l(ld(actor + 0xF0), actor);
        if (ld8(actor + 0x2DA) == 2 && !(ld(actor + 0x2E0) & ST_NOFALLDEL)) {
            u32 player = ld(dComIfGp_ea() + 0x5B34);
            if (player != 0 && ldf(actor + 0x318) < ldf(player + 0x318) - 15000.0f) fopAcM_delete_l(actor);
        }
    } else {
        st(actor + 0x2E4, ld(actor + 0x2E4) | CND_NOEXEC);
    }
    assert_pos(actor, 0x2EA, 0x2EE);
    return ret;
}
VERIFY(0x025D475C, fopAc_Execute);

/* 025D4AF8 */
static s32 fopAc_IsDelete(u32 actor) {
    WWHD_FUNC(0x025D4AF8, s32, actor);
    s32 ret = fpcMtd_IsDelete_l(ld(actor + 0xF0), actor);
    if (ret == 1) fopDwTg_DrawQTo_l(actor + 0xDC);
    return ret;
}
VERIFY(0x025D4AF8, fopAc_IsDelete);

/* 025D4B4C: HD: runs the virtual destructor of a constructed actor */
static s32 fopAc_Delete(u32 actor) {
    WWHD_FUNC(0x025D4B4C, s32, actor);
    if (ld8(actor + 0x3AA) != 1) JUT_ASSERT_l(0x1005706C, 0x376, 0x1005707C);
    s32 ret = fpcMtd_Delete_l(ld(actor + 0xF0), actor);
    if (ret == 1) {
        if (ld8(actor + 0x3AA) == 1) gabi::call_ptr(ld(ld(actor + 0xB4) + 0xC), actor, 2);
        fopAcTg_ActorQTo_l(actor + 0xC8);
        fopDwTg_DrawQTo_l(actor + 0xDC);
        fopAcM_DeleteHeap_l(actor);
        u8 id = ld8(actor + 0x2DC);
        if (id != 0 && id <= 0x20) {
            u32 demo = ld(0x101D5FFC);
            if (demo == 0) {
                JUT_ASSERT_l(0x10056E64, 0x23A, 0x10056E54);
                demo = ld(0x101D5FFC);
            }
            u32 da = dDemo_object_getActor_l(demo, id);
            if (da != 0) dDemo_actor_setActor_l(da, 0);
        }
        mDoAud_seDeleteObject_l(actor + 0x37C);
        mDoAud_seDeleteObject_l(actor + 0x314);
    }
    return ret;
}
VERIFY(0x025D4B4C, fopAc_Delete);

/* 025D4C70 */
static s32 fopAc_Create(u32 actor) {
    WWHD_FUNC(0x025D4C70, s32, actor);
    if (ld8(actor + 0xC) == 0) { /* fpcM_IsFirstCreating */
        u32 profile = ld(actor + 0x10);
        st(actor + 0xC4, fpcBs_MakeOfType_l(g_fopAc_type));
        st(actor + 0xF0, ld(profile + 0x24));
        fopAcTg_Init_l(actor + 0xC8, actor);
        fopAcTg_ToActorQ_l(actor + 0xC8);
        fopDwTg_Init_l(actor + 0xDC, actor);
        st(actor + 0x2E0, ld(profile + 0x28));
        st8(actor + 0x2DA, ld8(profile + 0x2C));
        st8(actor + 0x2DB, ld8(profile + 0x2D));
        u32 prm = ld(actor + 0xAC);
        if (prm != 0) {
            st(actor + 0xB0, ld(prm + 0x0));
            st(actor + 0x2EC, ld(prm + 0x4));
            st(actor + 0x2F0, ld(prm + 0x8));
            st(actor + 0x2F4, ld(prm + 0xC));
            st16(actor + 0x2F8, ld16(prm + 0x10));
            st16(actor + 0x2FA, ld16(prm + 0x12));
            st16(actor + 0x2FC, ld16(prm + 0x14));
            st16(actor + 0x328, ld16(prm + 0x10));
            st16(actor + 0x32A, ld16(prm + 0x12));
            st16(actor + 0x32C, ld16(prm + 0x14));
            st(actor + 0x2E8, ld(prm + 0x1C));
            st8(actor + 0x2DD, ld8(prm + 0x20));
            st8(actor + 0x2DE, ld8(prm + 0x1B));
            f32 sx = (f32)ld8(prm + 0x18), sy = (f32)ld8(prm + 0x19), sz = (f32)ld8(prm + 0x1A);
            stf(actor + 0x330, sx * 0.1f);
            stf(actor + 0x334, sy * 0.1f);
            stf(actor + 0x338, sz * 0.1f);
            st16(actor + 0x2D8, ld16(prm + 0x16));
            st8(actor + 0x2FE, ld8(prm + 0x21));
        }
        u32 px = ld(actor + 0x2EC), py = ld(actor + 0x2F0), pz = ld(actor + 0x2F4);
        u32 a0 = ld(actor + 0x2F8), a1 = ld(actor + 0x2FC);
        st(actor + 0x300, px); st(actor + 0x304, py); st(actor + 0x308, pz); /* old = home */
        st(actor + 0x30C, a0); st(actor + 0x310, a1);
        st(actor + 0x314, px); st(actor + 0x318, py); st(actor + 0x31C, pz); /* current = home */
        st(actor + 0x320, a0); st(actor + 0x324, a1);
        st(actor + 0x37C, px); st(actor + 0x380, py); st(actor + 0x384, pz); /* eyePos */
        stf(actor + 0x378, -100.0f);
        static const u8 dist[8] = {1, 2, 3, 7, 8, 16, 16, 15};
        for (int i = 0; i < 8; i++) st8(actor + 0x388 + i, dist[i]);
        st(actor + 0x390, px); st(actor + 0x394, py); st(actor + 0x398, pz); /* attention position */
        dKy_tevstr_init_l(actor + 0x110, (s8)ld8(actor + 0x2FE), 0xFF);
    }
    s32 status = fpcMtd_Create_l(ld(actor + 0xF0), actor);
    if (status == 4) {
        s32 priority = fpcM_DrawPriority_l(actor);
        fopDwTg_ToDrawQ_l(actor + 0xDC, priority);
    }
    return status;
}
VERIFY(0x025D4C70, fopAc_Create);

/* 025D4ED0: fopAc_ac_c::fopAc_ac_c (HD: allocates when this == NULL; construction state +0x3AA) */
static u32 fopAc_ac_c_ct(u32 self) {
    WWHD_FUNC(0x025D4ED0, u32, self);
    if (self == 0) {
        self = operator_new_l(0x3AC);
        if (self == 0) return self;
    }
    base_process_ct_l(self);
    st(self + 0xB4, 0x10057138);
    dEvt_info_c_ct_l(self + 0xF8);
    /* three copies of the default light block (0x44 bytes at 1016E414) in tevStr */
    const u32 T = 0x1016E414;
    f32 f[6];
    for (int i = 0; i < 6; i++) f[i] = ldf(T + 4 * i);
    u8 b[4];
    for (int i = 0; i < 4; i++) b[i] = ld8(T + 0x18 + i);
    u16 h[4];
    for (int i = 0; i < 4; i++) h[i] = ld16(T + 0x1C + 2 * i);
    f32 g[8];
    for (int i = 0; i < 8; i++) g[i] = ldf(T + 0x24 + 4 * i);
    static const u32 base[3] = {0x110, 0x1D0, 0x254};
    for (int k = 0; k < 3; k++) {
        u32 p = self + base[k];
        for (int i = 0; i < 6; i++) stf(p + 4 * i, f[i]);
        for (int i = 0; i < 4; i++) st8(p + 0x18 + i, b[i]);
        for (int i = 0; i < 4; i++) st16(p + 0x1C + 2 * i, h[i]);
        for (int i = 0; i < 8; i++) stf(p + 0x24 + 4 * i, g[i]);
    }
    if (ld8(self + 0x3AA) != 0) JUT_ASSERT_l(0x100570E4, 0x14F, 0x100570C4);
    st8(self + 0x3AA, 1);
    return self;
}
VERIFY(0x025D4ED0, fopAc_ac_c_ct);

/* 025D50BC: fopAc_ac_c::~fopAc_ac_c */
static void fopAc_ac_c_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x025D50BC, void, self, flags);
    if (self == 0) return;
    u8 state = ld8(self + 0x3AA);
    st(self + 0xB4, 0x10057138);
    if (state != 1) JUT_ASSERT_l(0x10057118, 0x157, 0x100570F4);
    st8(self + 0x3AA, 2);
    base_process_dt_l(self, 0);
    if (flags & 1) operator_delete_l(self);
}
VERIFY(0x025D50BC, fopAc_ac_c_dt);

/* 025D5148 */
static void __sinit_f_op_actor_cpp() {
    WWHD_FUNC(0x025D5148, void, (u32)0);
    sinit_header_statics(0x10487424, 0x101F305C);
}
VERIFY(0x025D5148, __sinit_f_op_actor_cpp);

} // namespace f_op_actor_cpp
