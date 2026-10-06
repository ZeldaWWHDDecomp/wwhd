/* d_s_play part 8: dScnPly_Draw. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD dScnPly_Draw: the HD scene system (+0x1D4) gets the view matrix first. The stage change
 * keeps the GameCube structure (wipe type check, sea / Obombh wipes, day/night wipe choice,
 * l_wipeType at 101EAC4C) with HD additions: a change to "sea_E" waits for / restarts an HD
 * system, a change to "ENDING" is not requested here, the "ENDumi" check decides an HD flag
 * (101F8350). The per-frame part also runs while paused (vibration pause, collision and particle
 * updates) and the HD system for mode 2; the drawing goes through the HD draw list. */
#include "bindings.h"

namespace d_s_play_8_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline BOOL dMenu_flag_l() { return gabi::call<BOOL>(0x025986BC); }
static inline BOOL dScnPly_isPause_l() { return gabi::call<BOOL>(0x025AF2A4); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 SSTR_VT = 0x10052EEC;
struct sstr_l { be<u32> str, vt; };
struct mtx_l { be<f32> m[12]; };
static inline void assure(u32 s) { gabi::call_ptr(ld(ld(s + 4) + 0x14), s); }

/* the next stage name (play+0x5140) == literal (literal assured twice, then the stage name) */
static bool next_is(gabi::Local<sstr_l>& lit, gabi::Local<sstr_l>& stage, u32 str) {
    lit->str = str;
    lit->vt = SSTR_VT;
    u32 play = dComIfGp_ea();
    stage->vt = SSTR_VT;
    stage->str = play + 0x5140;
    u32 a = gabi::ea(lit.get()), b = gabi::ea(stage.get());
    assure(a);
    assure(a);
    u32 s1 = ld(a);
    assure(b);
    u32 s2 = ld(b);
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = ld8(s1);
        if (c != ld8(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

static inline s32 wipe() { return (s8)ld8(dComIfGp_ea() + 0x514D); } /* dComIfGp_getNextStageWipe() */

static inline void set_scene_name() { /* mDoAud_setSceneName(next stage, room, layer) */
    u32 name = dComIfGp_ea() + 0x5140;
    s32 room = (s8)ld8(dComIfGp_ea() + 0x514A);
    s32 layer = (s8)ld8(dComIfGp_ea() + 0x12A0 + 0x3EAB);
    gabi::call(0x025E17CC, name, room, layer);
}

/* the HD system's two virtual results compared (same pattern as dScnPly_isPause) */
static inline bool sys_is(u32 q, u32 obj) {
    u32 s = gabi::call<u32>(0x02006478, q + 0x10);
    u32 vt = ld(obj + 8);
    u32 a = gabi::call_ptr<u32>(ld(ld(s + 8) + 0x14), s);
    u32 b = gabi::call_ptr<u32>(ld(vt + 0x14), obj);
    return a == b;
}

/* the stage change part; returns through the per-frame part */
static void change_stage(u32 i_this) {
    s32 w = wipe();
    if ((u32)w >= 0xC) JUT_ASSERT_l(0x100531C0, 0x4C2, 0x100531D0);
    gabi::Local<sstr_l> l0, s0, l1, s1, l2, s2, l3, s3, l4, s4;
    if (next_is(l0, s0, 0x100531A0)) { /* "sea_E" */
        u32 q = ld(0x101F83D0);
        if (q == 0) return;
        u32 s = gabi::call<u32>(0x02006478, q + 0x10);
        u32 vt1 = ld(0x1049F694 + 8);
        u32 a = gabi::call_ptr<u32>(ld(ld(s + 8) + 0x14), s);
        u32 b = gabi::call_ptr<u32>(ld(vt1 + 0x14), 0x1049F694);
        if (a == b) {
            gabi::call(0x025DC86C, i_this, 7, 0, 5, 1); /* fopScnM_ChangeReq */
            gabi::call(0x025F05F8, 0x101D5E9C);
            gabi::call(0x025E1904, 0x1E);
            set_scene_name();
            return;
        }
        u32 q2 = ld(0x101F83D0);
        if (sys_is(q2, 0x1049F668)) return;
        u32 s2b = gabi::call<u32>(0x02006478, q2 + 0x10);
        gabi::call_ptr<u32>(ld(ld(s2b + 8) + 0x14), s2b);
        gabi::call_ptr<u32>(ld(vt1 + 0x14), 0x1049F694);
        return;
    }
    if (next_is(l1, s1, 0x100531A8)) return; /* "ENDING" */
    s32 type = wipe();
    bool night;
    if (type == 4) {
        if (next_is(l2, s2, 0x1005319C)) { /* "sea" */
            gabi::call(0x025DC86C, i_this, 7, (u32)(s32)(s16)ld16(0x101EAC4C + 0x12), 5, 0);
            type = 9;
            goto daytime;
        }
        type = next_is(l3, s3, 0x100531B0) ? 8 : 3; /* "Obombh" */
    }
    gabi::call(0x025DC86C, i_this, 7, (u32)(s32)(s16)ld16(0x101EAC4C + type * 2), 5, 0); /* l_wipeType[type] */
daytime : {
    s32 hour = gabi::call<s32>(0x02556C34); /* dKy_getdaytime_hour() */
    if (gabi::call<BOOL>(0x02556BC0))      /* dKy_checkEventNightStop() */
        night = true;
    else
        night = !((u32)(hour - 6) < 12);
}
    bool day_wipe;
    if (wipe() == 1 || wipe() == 2 || wipe() == 7) {
        day_wipe = true;
    } else {
        day_wipe = false;
        if (wipe() == 8 || wipe() == 0xA || type == 8) {
            if (night) day_wipe = true;
        }
        if (!day_wipe && (wipe() == 9 || wipe() == 0xB || type == 9)) {
            if (!night) day_wipe = true;
        }
    }
    gabi::call(0x025F05F8, day_wipe ? 0x101D5E9C : 0x101D5E94);
    if (!next_is(l4, s4, 0x100531B8)) st8(0x101F8350, 1); /* "ENDumi" */
    set_scene_name();
}

/* 025AF8A0 */
static BOOL dScnPly_Draw(u32 i_this) {
    WWHD_FUNC(0x025AF8A0, BOOL, i_this);
    {
        gabi::Local<mtx_l> view;
        const u32 j3d = 0x104B45C0;
        for (int i = 0; i < 12; i++) view->m[i] = ldf(j3d + 0x38 + 4 * i); /* j3dSys view matrix */
        gabi::call(0x0276C9B4, i_this + 0x1D4, gabi::ea(view.get()), j3d + 0x14C, 0);
    }
    gabi::call(0x02518798, dComIfGp_ea() + 0x26A4); /* dComIfG_Ccsp()->Move() */
    gabi::call(0x024EE9C8, dComIfGp_ea() + 0x12A0); /* dComIfG_Bgsp()->ClrMoveFlag() */
    if (!gabi::call<BOOL>(0x025DBE00) && !gabi::call<BOOL>(0x025202F8, i_this) && i_this != 0 &&
        (s16)ld16(i_this + 8) == 7 && (s8)ld8(dComIfGp_ea() + 0x514C) != 0) /* !IsPeek, !resetToOpening, PLAY_SCENE, isEnableNextStage */
        change_stage(i_this);
    bool paused = dMenu_flag_l() || dScnPly_isPause_l();
    if (!paused) {
        gabi::call(0x025CBB90, dComIfGp_ea() + 0x599C); /* dVibration_c::Run */
        gabi::call(0x02524CA0, dComIfGp_ea() + 0x12A0);
        if (ld(dComIfGp_ea() + 0x5AB8) != 0 || ld(dComIfGp_ea() + 0x5ABC) != 0 || ld(dComIfGp_ea() + 0x5AC0) != 0 ||
            ld(dComIfGp_ea() + 0x5AC4) != 0 || gabi::call<BOOL>(0x0256C8BC))
            gabi::call(0x02516E1C, dComIfGp_ea() + 0x4EF8); /* dCcMassS_Mng::Prepare */
        gabi::call(0x02524DA0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x02524EA0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x02524FA0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x025250A0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x0256C8FC); /* dKyr_poison_light_colision */
        u32 play = dComIfGp_ea();
        gabi::call_ptr(ld(ld(play + 0x26A0) + 0x24), play + 0x12A0); /* dComIfG_Bgsp() virtual +0x24 */
        gabi::call(0x025BE8DC);
        u32 pl = ld(dComIfGp_ea() + 0x5B34);
        if (pl == 0 || !gabi::call<BOOL>(0x02443664, pl)) /* daPy_lk_c::checkGameOverStart */
            gabi::call(0x025A81A0, ld(dComIfGp_ea() + 0x5AB0));
        gabi::call(0x025A8148, ld(dComIfGp_ea() + 0x5AB0));
        st(0x101FF560, ld(0x101FF560) + 1);
    } else {
        gabi::call(0x025CB6D4, dComIfGp_ea() + 0x599C); /* dVibration_c::Pause */
    }
    gabi::call(0x025187D0, dComIfGp_ea() + 0x26A4);
    gabi::call(0x025A81F8, ld(dComIfGp_ea() + 0x5AB0));
    if (ld(i_this + 0xB64) == 2) gabi::call(0x0271DF30, ld(0x101F83D0));
    gabi::call(0x025AF738, 0x1047C89C);              /* g_msgDHIO.dScnPly_msg_HIO_messageProc() */
    gabi::call(0x025C4364, dComIfGp_ea() + 0x51CC);  /* dStage_roomControl_c::checkDrawArea */
    for (u32 p = gabi::call<u32>(0x025DA7AC); p != 0; p = gabi::call<u32>(0x025DA7D0, p))
        gabi::call(0x025DF904, ld(p + 0xC));          /* the HD draw list */
    if (!dMenu_flag_l()) {
        gabi::call(0x025A4984, ld(ld(dComIfGp_ea() + 0x5AB0) + 0x130)); /* dPa_modelControl_c::draw */
        gabi::call(0x02524CB0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x02524DB0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x02524EB0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x02524FB0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x025250B0, dComIfGp_ea() + 0x12A0);
        gabi::call(0x0251879C, dComIfGp_ea() + 0x26A4); /* dCcS::Draw */
        gabi::call(0x025BED8C, dComIfGp_ea());
        gabi::call(0x024EDDF8, dComIfGp_ea() + 0x5804); /* dAttention_c::Draw */
    }
    return TRUE;
}
VERIFY(0x025AF8A0, dScnPly_Draw);

} // namespace d_s_play_8_cpp
