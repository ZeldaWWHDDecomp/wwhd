/* d_s_play: the play scene (dScnPly), WWHD. 
 *
 * Translation unit 025AF2A4..025B37EC (a small unit 025AF208/025AF210 sits between d_s_open,
 * whose __sinit is 025AF174, and this one; d_s_room starts with objectSetCheck at 025B37EC).
 * HD order: the HD pause check, the message HIO, Draw, calcPauseTimer, Execute, IsDelete,
 * an HD heap release, Delete, heapSizeCheck, the scene constructor, the phases 00..5,
 * phase_compleate, Create, the HIO constructors, __sinit (025B34A4), then per-TU inline
 * copies (destructors, phase_6, small helpers).
 * Ported from the GameCube d_s_play.cpp where the code matches; HD-only functions are written
 * from the WWHD code.
 *
 * dScnPly_ply_c (HD 0xB68, constructor 025B0F2C): scene base +0..0x1D0, +0x1D0 zero word, HD
 * objects at +0x1D4 and +0x5D4 (wind input at +0xA04/+0xA08), +0x938, +0xA90, +0xAD0; +0xB60
 * u8 wind flag, +0xB64 mode (0..3, selects an HD system update after Execute).
 * dScnPly_msg_HIO_c (HD): mIsUpdate +4, field_0x06 +5, field_0x07 +6, mGroup +0xA, mID +0xC,
 * message id +0x10. pauseTimer 101EACB6, nextPauseTimer 101EACB7. */
#include "bindings.h"

namespace d_s_play_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline BOOL dMenu_flag_l() { return gabi::call<BOOL>(0x025986BC); }
static inline u32 f_02006478_l(u32 p) { return gabi::call<u32>(0x02006478, p); }
static inline BOOL f_027172E4_l() { return gabi::call<BOOL>(0x027172E4); }
static inline BOOL f_02717114_l(u32 p) { return gabi::call<BOOL>(0x02717114, p); }
static inline BOOL f_02717120_l(u32 p) { return gabi::call<BOOL>(0x02717120, p); }
/* m_Do_controller_pad: trigger/hold checks on a pad port */
static inline BOOL pad_trigZ_l(s32 port) { return gabi::call<BOOL>(0x02007814, port); }
static inline BOOL pad_trigUp_l(s32 port) { return gabi::call<BOOL>(0x02007764, port); }
static inline BOOL pad_trigDown_l(s32 port) { return gabi::call<BOOL>(0x02007790, port); }
static inline BOOL pad_trigRight_l(s32 port) { return gabi::call<BOOL>(0x020077E8, port); }
static inline BOOL pad_trigLeft_l(s32 port) { return gabi::call<BOOL>(0x020077BC, port); }
static inline BOOL pad_trigB_l(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline BOOL pad_holdA_l(s32 port) { return gabi::call<BOOL>(0x020076BC, port); }
static inline BOOL pad_holdX_l(s32 port) { return gabi::call<BOOL>(0x0200770C, port); }
static inline BOOL pad_holdY_l(s32 port) { return gabi::call<BOOL>(0x02007738, port); }
static inline u32 fopMsgM_messageSet_l(u32 mgr, u32 msg, u32 pos) { return gabi::call<u32>(0x025F7DB0, mgr, msg, pos); } /* HD: manager first */
static inline u32 fopMsgM_getStatus_l(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); } /* matcher: fopMsgM_SearchByID */
static inline void fopMsgM_setStatus_l(u32 id, u32 st) { gabi::call(0x025F74D0, id, st); } /* HD: by id */
static inline s8 cLib_calcTimer_s8_l(u32 p) { return gabi::call<s8>(0x0220CCB4, p); }
static inline BOOL fopOvlpM_IsPeek_l() { return gabi::call<BOOL>(0x025DBE00); }
static inline void mDoAud_sceneBgmStart_l() { gabi::call(0x025E18E0); }
static inline void mDoAud_load2ndDynamicWave_l() { gabi::call(0x025E18C4); }
static inline void dKy_itudemo_se_l() { gabi::call(0x02560484); }
static inline u32 dKyw_get_wind_vec_l() { return gabi::call<u32>(0x0257DAA8); }
static inline f32 dKyw_get_wind_pow_l() { return gabi::call<f32>(0x02578348); }
static inline void dAttention_Run_l(u32 att, s32 x) { gabi::call(0x024EDB80, att, x); }
static inline BOOL mDoAud_load1stDynamicWave_l() { return gabi::call<BOOL>(0x025E1828); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 pauseTimer = 0x101EACB6;
static constexpr u32 nextPauseTimer = 0x101EACB7;

struct cXyz_l { be<f32> x, y, z; };

/* 025AF2A4: HD-only dScnPly_ply_c::isPause(): the pause timer, or one of the HD system overlays
 * (probably the HOME menu / GamePad screens) being open */
static s32 dScnPly_isPause() {
    WWHD_FUNC(0x025AF2A4, s32);
    u32 sys = ld(0x101F8344);
    s32 ret = (s8)ld8(pauseTimer);
    if (sys != 0) {
        s32 open = 1;
        if (f_027172E4_l() == 0 && f_02717114_l(ld(0x101F8344)) == 0 && f_02717120_l(ld(0x101F8344)) == 0) open = 0;
        ret |= open;
    }
    u32 q = ld(0x101F83D0);
    if (q != 0) {
        s32 open = 1;
        u32 s = f_02006478_l(q + 0x10);
        u32 vt = ld(0x1049F668 + 8);
        u32 a = gabi::call_ptr<u32>(ld(ld(s + 8) + 0x14), s);
        u32 b = gabi::call_ptr<u32>(ld(vt + 0x14), 0x1049F668);
        if (a != b) {
            u32 s2 = f_02006478_l(q + 0x10);
            u32 vt2 = ld(0x1049F694 + 8);
            u32 a2 = gabi::call_ptr<u32>(ld(ld(s2 + 8) + 0x14), s2);
            u32 b2 = gabi::call_ptr<u32>(ld(vt2 + 0x14), 0x1049F694);
            if (a2 != b2) open = 0;
        }
        ret |= open;
    }
    return ret;
}
VERIFY(0x025AF2A4, dScnPly_isPause);

/* 025AF3D4 */
static void dScnPly_msg_HIO_checkUpdate(u32 self, u32 i_update) {
    WWHD_FUNC(0x025AF3D4, void, self, i_update);
    st8(self + 4, (u8)i_update);
}
VERIFY(0x025AF3D4, dScnPly_msg_HIO_checkUpdate);

/* 025AF3DC: HD: mID is clamped at 0 only (it is an s16) */
static void dScnPly_msg_HIO_numUpdate(u32 self, s32 i_addGroup, s32 i_addID) {
    WWHD_FUNC(0x025AF3DC, void, self, i_addGroup, i_addID);
    s16 group = (s16)((s16)ld16(self + 0xA) + i_addGroup);
    s16 id = (s16)((s16)ld16(self + 0xC) + i_addID);
    st16(self + 0xA, (u16)group);
    st16(self + 0xC, (u16)id);
    if (group > 99) {
        id = (s16)ld16(self + 0xC);
        st16(self + 0xA, 99);
    } else if (group < 0) {
        id = (s16)ld16(self + 0xC);
        st16(self + 0xA, 0);
    }
    if (id < 0) st16(self + 0xC, 0);
}
VERIFY(0x025AF3DC, dScnPly_msg_HIO_numUpdate);

/* 025AF43C */
static void dScnPly_msg_HIO_setUpdate(u32 self, u32 param_0) {
    WWHD_FUNC(0x025AF43C, void, self, param_0);
    st8(self + 6, (u8)param_0);
}
VERIFY(0x025AF43C, dScnPly_msg_HIO_setUpdate);

/* 025AF444: HD: no JUTReport */
static void dScnPly_msg_HIO_padCheck(u32 self) {
    WWHD_FUNC(0x025AF444, void, self);
    if (ld8(self + 4) == 0) return;
    if (pad_trigZ_l(3)) dScnPly_msg_HIO_checkUpdate(self, 0);
    if (pad_trigUp_l(3)) {
        if (pad_holdA_l(3)) dScnPly_msg_HIO_numUpdate(self, 10, 0);
        else dScnPly_msg_HIO_numUpdate(self, 1, 0);
    } else if (pad_trigDown_l(3)) {
        if (pad_holdA_l(3)) dScnPly_msg_HIO_numUpdate(self, -10, 0);
        else dScnPly_msg_HIO_numUpdate(self, -1, 0);
    } else if (pad_trigRight_l(3)) {
        if (pad_holdY_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, 1000);
        else if (pad_holdX_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, 100);
        else if (pad_holdA_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, 10);
        else dScnPly_msg_HIO_numUpdate(self, 0, 1);
    } else if (pad_trigLeft_l(3)) {
        if (pad_holdY_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, -1000);
        else if (pad_holdX_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, -100);
        else if (pad_holdA_l(3)) dScnPly_msg_HIO_numUpdate(self, 0, -10);
        else dScnPly_msg_HIO_numUpdate(self, 0, -1);
    } else if (pad_trigB_l(3)) {
        if (ld8(self + 5) == 0) st8(self + 5, 1);
    } else if (ld8(self + 6) != 0) {
        if (ld8(self + 5) == 0) st8(self + 5, 1);
        dScnPly_msg_HIO_setUpdate(self, 0);
    }
}
VERIFY(0x025AF444, dScnPly_msg_HIO_padCheck);

/* 025AF738: HD: the message number is not searched (group/ID reset to 0 instead), messages
 * are driven through the HD message manager by id */
static void dScnPly_msg_HIO_messageProc(u32 self) {
    WWHD_FUNC(0x025AF738, void, self);
    dScnPly_msg_HIO_padCheck(self);
    u32 mgr = ld(0x101F4B5C);
    if (ld8(self + 5) == 0) return;
    if ((s32)ld(self + 0x10) == -1) {
        s32 group = (s16)ld16(self + 0xA);
        u32 msg_num = ((u32)group << 16) | (u32)(s32)(s16)ld16(self + 0xC);
        gabi::Local<cXyz_l> pos;
        pos->x = 0.0f;
        pos->z = 0.0f;
        pos->y = 0.0f;
        if (group != 99 && group != 98 && group != 89) {
            st16(self + 0xA, 0);
            st16(self + 0xC, 0);
            st(self + 0x10, fopMsgM_messageSet_l(mgr, 0, gabi::ea(pos.get())));
            return;
        }
        st(self + 0x10, fopMsgM_messageSet_l(mgr, msg_num, gabi::ea(pos.get())));
        return;
    }
    if (fopMsgM_getStatus_l(mgr) == 0xE) {
        fopMsgM_setStatus_l(mgr, 0x10);
        return;
    }
    if (fopMsgM_getStatus_l(mgr) == 0x12) {
        fopMsgM_setStatus_l(mgr, 0x13);
        s16 id = (s16)ld16(self + 0xC);
        st(self + 0x10, (u32)-1);
        st8(self + 5, 0);
        st16(self + 0xC, (u16)(id + 1));
    }
}
VERIFY(0x025AF738, dScnPly_msg_HIO_messageProc);

/* 025B02E8 */
static s8 dScnPly_ply_c_calcPauseTimer(u32 self) {
    WWHD_FUNC(0x025B02E8, s8, self);
    s8 next = (s8)ld8(nextPauseTimer);
    if (next != 0) {
        st8(pauseTimer, (u8)next);
        st8(nextPauseTimer, 0);
        return next;
    }
    return cLib_calcTimer_s8_l(pauseTimer);
}
VERIFY(0x025B02E8, dScnPly_ply_c_calcPauseTimer);

/* the HD system update selected by the scene mode (+0xB64) */
static inline void mode_update(u32 i_this) {
    u32 mode = ld(i_this + 0xB64);
    if (mode == 0) gabi::call(0x0271F078, ld(0x101F83FC));
    else if (mode == 1) gabi::call(0x027152B4, ld(0x101F8344));
    else if (mode == 2) gabi::call(0x0271DC4C, ld(0x101F83D0));
    else if (mode == 3) gabi::call(0x02707E78, ld(0x101F8268));
}

/* 025B0314: HD: also stops for the HD pause; the scene's HD objects (+0x1D4, wind on +0x5D4)
 * are updated, then an HD system selected by +0xB64 */
static BOOL dScnPly_Execute(u32 i_this) {
    WWHD_FUNC(0x025B0314, BOOL, i_this);
    if (!fopOvlpM_IsPeek_l()) {
        if (ld8(0x101F4706) != 0) { /* mDoAud_zelAudio_c::isBgmSet() */
            mDoAud_sceneBgmStart_l();
            mDoAud_load2ndDynamicWave_l();
            st8(0x101F4706, 0);
        }
        if (dScnPly_ply_c_calcPauseTimer(i_this) != 0) {
            if (ld(i_this + 0xB64) == 1) gabi::call(0x027152B4, ld(0x101F8344));
            return TRUE;
        }
    }
    dKy_itudemo_se_l();
    if (!dMenu_flag_l() && !dScnPly_isPause()) {
        gabi::call(0x0276C8B8, i_this + 0x1D4);
        if (ld8(i_this + 0xB60) != 0) {
            u32 wv = dKyw_get_wind_vec_l();
            f32 pow = dKyw_get_wind_pow_l();
            f32 wz = ldf(wv + 8);
            f32 a = gabi::fmuls_ppc(0.0372f, wz);
            f32 wx = ldf(wv + 0);
            f32 d = gabi::fsubs_ppc(pow, 0.3f);
            f32 b = gabi::fmuls_ppc(0.0372f, wx);
            f32 p = d >= 0.0f ? pow : 0.3f; /* fsel */
            stf(i_this + 0xA08, gabi::fmuls_ppc(a, p));
            stf(i_this + 0xA04, gabi::fmuls_ppc(b, p));
            gabi::call(0x02792640, i_this + 0x5D4, 1.0f);
            u32 idx = (u32)(s32)(s16)ld16(ld(0x104A145C));
            u32 n1 = ld(i_this + 0x1DC);
            u32 arr = ld(i_this + 0x1E0);
            u32 e = idx < n1 ? arr + (idx << 2) : arr;
            u32 obj;
            if (ld16(e + 2) == 0) {
                obj = 0;
            } else {
                u32 n2 = ld(i_this + 0x1E4);
                u32 e2 = idx < n1 ? arr + (idx << 2) : arr;
                u32 k = ld16(e2);
                u32 p2 = 0;
                if (k < n2) p2 = ld(i_this + 0x1E8) + (k << 2);
                obj = ld(p2);
            }
            /* function-local static (an empty object with a vtable) */
            if (ld(0x101FD7E0) == 0) {
                st(0x101FD7E0, 1);
                st(0x101FDD14, 0x10052FAC);
            }
            if (obj != 0 && gabi::call_ptr<u32>(ld(ld(obj + 0x58) + 0x44), obj, 0x101FDD14u) == 0) obj = 0; /* isKindOf(&type descriptor 101FDD14): r4 is live at the bctrl (game test 2026-10-04) */
            gabi::call(0x02525C94, i_this + 0xA90, i_this + 0x938, obj + 0x200);
        }
        gabi::call(0x025291C8);
        gabi::call(0x02524BA8, dComIfGp_ea() + 0x12A0);
        u32 att = dComIfGp_ea() + 0x5804; /* dAttention_c */
        if (ld(att) != 0) {
            dAttention_Run_l(att, -1);
        } else {
            u32 pl = ld(dComIfGp_ea() + 0x5B2C);
            st(att + 0xC, 0); /* Init(player, 0) */
            st(att + 0x0, pl);
        }
        gabi::call(0x0252A154, dComIfGp_ea() + 0x5A20); /* dComIfGp_getDetect().proc() */
    }
    mode_update(i_this);
    return TRUE;
}
VERIFY(0x025B0314, dScnPly_Execute);

/* 025B05DC */
static BOOL dScnPly_IsDelete(u32 i_this) {
    WWHD_FUNC(0x025B05DC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x025B05DC, dScnPly_IsDelete);

/* 025B05E4: HD-only: destroys the extra actor heap 1047C8B4 (see fopAcM_entrySolidHeap) */
static void dScnPly_destroyExtraHeap() {
    WWHD_FUNC(0x025B05E4, void);
    u32 heap = ld(0x1047C8B4);
    if (heap != 0) {
        gabi::call_ptr(ld(ld(heap + 0xC) + 0x24), heap); /* destroy */
        st(0x1047C8B4, 0);
    }
}
VERIFY(0x025B05E4, dScnPly_destroyExtraHeap);

/* 025B0F2C: dScnPly_ply_c::dScnPly_ply_c (HD 0xB68) */
static u32 dScnPly_ply_c_ct(u32 self) {
    WWHD_FUNC(0x025B0F2C, u32, self);
    if (self == 0) {
        self = operator_new_l(0xB68);
        if (self == 0) return self;
    }
    gabi::call(0x025DD5F0, self); /* scene_class base */
    st(self + 0x1D0, 0);
    gabi::call(0x0276C1DC, self + 0x1D4);
    gabi::call(0x0279207C, self + 0x5D4);
    if (self + 0xA90 == 0) operator_new_l(0x40); /* inline constructor of an empty 0x40-byte member */
    gabi::call(0x027BE6B8, self + 0xAD0);
    st(self + 0xB64, 0);
    st8(self + 0xB60, 0);
    return self;
}
VERIFY(0x025B0F2C, dScnPly_ply_c_ct);

/* 025B13EC */
static s32 phase_01(u32 i_this) {
    WWHD_FUNC(0x025B13EC, s32, i_this);
    s32 r = mDoAud_load1stDynamicWave_l();
    if (r == 0) return r;
    return 2;
}
VERIFY(0x025B13EC, phase_01);

} // namespace d_s_play_cpp
