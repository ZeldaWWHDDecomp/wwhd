/* d_a_player_bghit_t60.h (WWHD, true 60 build, 2026-11-19, rule: the BG-hit rule,
 * the first-step AI-timer rule family). Decomp build only.
 *
 * Link's decisions on a BG hit flag (mAcch ground / wall / roof / water hits, the circles' wall hits) act only on the
 * step that follows a frame-end BG check. The original runs one BG check per frame (CrrPos after posMove) and the next
 * frame's update decides on its result. At N steps every step runs its own check, and the frame's later steps decided
 * on an in-between check: Link reached the ground on a half step and the next step of the same frame already took the
 * landing (ksr31n2, Siren:3: procLargeDamage -> LargeDamageUp one frame early, without the frame's second half of
 * the horizontal move, pos 7.71 @275; the second hit at ~383 split from there). Form (generic, 2026-11-19): the
 * frame's first step records the hit bits (the last frame-end check's result); on the frame's later steps the decision
 * part of the update (from the stick read to the end of the procedure body, before posMove / CrrPos) sees those bits,
 * so the step keeps the in-air / held path with its dt share; after it the real bits are restored for the step's own
 * motion and BG check, with every bit the decision part wrote itself merged back exactly (a bit it changed against
 * the view keeps the written value, every other hit bit its real value; non-hit bits pass through). Host memory, one
 * Link. dt = 1 (N = 1): never (the original). */
#pragma once
#include "true60.h"
#include "d/d_bg_s.h"
#include <stdlib.h>

namespace lkT60Bg {
/* the hit bits: the BG check's results (dBgS_Acch), not its request flags (GRND_NONE, WALL_NONE, LINE_CHECK, ...) */
static const uint32_t kAcchHit = dBgS_Acch::WALL_HIT | dBgS_Acch::GROUND_HIT | dBgS_Acch::GROUND_FIND |
                                 dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_AWAY | dBgS_Acch::ROOF_HIT |
                                 dBgS_Acch::WATER_HIT | dBgS_Acch::WATER_IN | dBgS_Acch::LINE_CHECK_HIT |
                                 dBgS_Acch::SEA_IN;
static const uint32_t kCirHit = dBgS_AcchCir::WALL_HIT;
enum { N_CIR = 3 };

/* (2026-11-25, long1 N 4 seg 1 frame 1094: B's game stopped on an assert in
 * procFrontRoll's wall crash, lk05GetMtrlSndId -> dBgS::GetPolyId on an invalid poly): the view gave a later step the
 * circles' frame-start WALL_HIT bit, but their wall poly (cBgS_PolyInfo, 0x10 bytes before m_flags) and wall angle
 * (m_wall_angle_y) stayed the step's own check's (no wall there: no poly). The circle's hit view now carries its poly and
 * wall angle with the bit: recorded on the first step, in place for the decision part, the real ones back after it (a
 * part that wrote them keeps its write). NC WWHD_T60_BGHITPOLY_NEGCTRL=1 (the bits only). */
static inline bool polyNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BGHITPOLY_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
enum { kCirPolyOff = 0x10, kCirAngleOff = 0x3C - 0x10, kPolyWords = 4 };
struct CirPoly {
    uint32_t w[kPolyWords];
    uint16_t ang;
};
static inline void cirGet(be<u32>* flags, CirPoly& o) {
    const uint32_t base = gabi::ea(flags) - kCirPolyOff;
    for (int k = 0; k < kPolyWords; k++)
        o.w[k] = *gabi::at<be<u32>>(base + 4 * k);
    o.ang = *gabi::at<be<u16>>(gabi::ea(flags) + kCirAngleOff);
}
static inline void cirPut(be<u32>* flags, const CirPoly& o) {
    const uint32_t base = gabi::ea(flags) - kCirPolyOff;
    for (int k = 0; k < kPolyWords; k++)
        *gabi::at<be<u32>>(base + 4 * k) = o.w[k];
    *gabi::at<be<u16>>(gabi::ea(flags) + kCirAngleOff) = o.ang;
}
static inline bool cirSame(const CirPoly& a, const CirPoly& b) {
    for (int k = 0; k < kPolyWords; k++)
        if (a.w[k] != b.w[k])
            return false;
    return a.ang == b.ang;
}
struct View {
    CirPoly poly0[3], polyReal[3], polySeen[3]; /* */
    uint32_t tag;           /* the frame the record is from */
    uint32_t acch0;         /* the frame's first-step hit bits */
    uint32_t cir0[N_CIR];
    uint32_t acchReal, acchSeen;
    uint32_t cirReal[N_CIR], cirSeen[N_CIR];
    bool valid;
    bool on;                /* the view is in place (a later step) */
};
static View s_view;

/* the merge: the decision part's own writes (bits it changed against the view) win, the other hit bits are real */
static inline uint32_t merge(uint32_t now, uint32_t seen, uint32_t real, uint32_t mask) {
    const uint32_t changed = (now ^ seen) & mask;
    return (now & ~mask) | (real & mask & ~changed) | (now & mask & changed);
}
static inline uint32_t viewOf(uint32_t real, uint32_t rec, uint32_t mask) { return (real & ~mask) | (rec & mask); }

/* before a decision part. execute has two windows (after the reader survey): (1) setGetDemo
 * (its early return calls end first), (2) setDemoData / endDemoMode -> checkNextMode -> setFrontWallType, setStickData
 * and the procedure dispatch up to lkT60Crawl_afterBody. MoveBgCrrPos between them (the moving ground's carry, physics)
 * sees the real bits. */
static inline void begin(be<u32>& acch, be<u32>& c0, be<u32>& c1, be<u32>& c2, uint32_t tag) {
    be<u32>* const cir[N_CIR] = {&c0, &c1, &c2}; /* (host array: header code, not glued) */
    View& v = s_view;
    v.on = false;
#ifdef LKT60_BGHIT_NEGCTRL
    return; /* negative control: the step's own hit bits (the per-step decisions before this rule) */
#endif
    if (t60::dt() == 1.0f)
        return;
    if (t60::first_step()) {
        if (v.valid && v.tag == tag)
            return; /* the frame's record is made at its first begin (execute has two windows) */
        v.acch0 = (uint32_t)acch & kAcchHit;
        for (int i = 0; i < N_CIR; i++) {
            v.cir0[i] = (uint32_t)*cir[i] & kCirHit;
            cirGet(cir[i], v.poly0[i]); /* */
        }
        v.tag = tag;
        v.valid = true;
        return;
    }
    if (!(v.valid && v.tag == tag))
        return; /* no record of this frame's first step: the step's own bits */
    v.acchReal = acch;
    v.acchSeen = viewOf(v.acchReal, v.acch0, kAcchHit);
    acch = v.acchSeen;
    for (int i = 0; i < N_CIR; i++) {
        v.cirReal[i] = *cir[i];
        v.cirSeen[i] = viewOf(v.cirReal[i], v.cir0[i], kCirHit);
        *cir[i] = v.cirSeen[i];
        if (!polyNc() && (v.cir0[i] & kCirHit)) { /* the recorded wall's poly / angle with its bit */
            cirGet(cir[i], v.polyReal[i]);
            cirPut(cir[i], v.poly0[i]);
            v.polySeen[i] = v.poly0[i];
        } else {
            cirGet(cir[i], v.polyReal[i]);
            v.polySeen[i] = v.polyReal[i];
        }
    }
    v.on = true;
}
/* after the decision part, before posMove */
static inline void end(be<u32>& acch, be<u32>& c0, be<u32>& c1, be<u32>& c2) {
    be<u32>* const cir[N_CIR] = {&c0, &c1, &c2};
    View& v = s_view;
    if (!v.on)
        return;
    v.on = false;
    acch = merge((uint32_t)acch, v.acchSeen, v.acchReal, kAcchHit);
    for (int i = 0; i < N_CIR; i++) {
        *cir[i] = merge((uint32_t)*cir[i], v.cirSeen[i], v.cirReal[i], kCirHit);
        CirPoly now; /* the real poly / angle back unless the part wrote them */
        cirGet(cir[i], now);
        if (cirSame(now, v.polySeen[i]))
            cirPut(cir[i], v.polyReal[i]);
    }
}

/* Addendum (BG-hit rule addendum; enemies pair, Siren
 * room 22: the Armos Knight's jump hits Link at frame 90). A ground-hit bit from the previous frame-end BG check must not
 * satisfy a check in a procedure entered during this frame: it waits for the next frame-end check. The original enters
 * procLargeDamage (0x68) at frame 90 and its second update (frame 91) reads frame 90's end check, Link already lifted by
 * the launch (y 810 -> 836): no ground, 0x69 at 99. At N steps the second update ran on frame 90's tick30 step, and the
 * window above showed it the frame-start record (Link standing): 0x69 on frame 90. Form (generic: a procedure-entry
 * step stamp against the BG check's frame, not per procedure): commonProcInit stamps the entry's frame and step
 * (port latch: commonProcInit is in d_a_player_main_01, the dispatch in d_a_player_main_04); a procedure body run on a
 * later step of the frame its procedure was entered in, at an earlier step, sees GROUND_HIT clear (the record, or the
 * step's own in-between check, predates the entry); after the body the bit comes back unless the body set it itself.
 * A body run on its entry step (entered before the dispatch of that step) reads the bits as before: in the original
 * that body runs in the entry frame's update on the previous frame-end check too. dt = 1 (N = 1): never. */
static inline uint32_t entryFrameKey(uint32_t curProcEa) { return curProcEa ^ 0x80000000u; } /* (outside guest EAs) */
static inline uint32_t entryStepKey(uint32_t curProcEa) { return curProcEa ^ 0xC0000000u; }
/* d_a_player_main_01 commonProcInit */
static inline void procEntered(uint32_t curProcEa, uint32_t tag) {
    if (t60::dt() == 1.0f)
        return;
    wwhd_t60_latch_set(entryFrameKey(curProcEa), tag);
    wwhd_t60_latch_set(entryStepKey(curProcEa), t60_step_counter());
}
/* d_a_player_main_04: around the procedure body; returns whether the bit was hidden */
static inline bool entryHideBegin(be<u32>& acch, uint32_t curProcEa, uint32_t tag) {
#ifdef LKT60_BGHIT_NEGCTRL
    return false;
#endif
    if (t60::dt() == 1.0f || t60::first_step())
        return false;
    if (!wwhd_t60_latch_pending(entryFrameKey(curProcEa), tag))
        return false; /* not entered in this frame */
    if (wwhd_t60_latch_pending(entryStepKey(curProcEa), t60_step_counter()))
        return false; /* entered on this step: the body's first run */
    if (!((uint32_t)acch & (uint32_t)dBgS_Acch::GROUND_HIT))
        return false;
    acch = (uint32_t)acch & ~(uint32_t)dBgS_Acch::GROUND_HIT;
    return true;
}
static inline void entryHideEnd(be<u32>& acch, bool hid) {
    if (hid && !((uint32_t)acch & (uint32_t)dBgS_Acch::GROUND_HIT))
        acch = (uint32_t)acch | (uint32_t)dBgS_Acch::GROUND_HIT;
}

/* s93 / (and (b'); jk1 / jk92: the Hyroom arrival,
 * rebirth_hyral). The original runs the execute's per-update procedure checks once per 30 Hz frame. At the arrival point
 * (DEMO_INIT_WAIT, no ground under it) changeDemoProc puts FALL back to the demo's WAIT and changeAutoJumpProc (no ground
 * hit) takes WAIT to FALL: a two-update cycle, one procedure per frame in A (WAIT, FALL, WAIT, ...); consulted on every
 * step, B changed on every step and every frame ended in the second step's FALL (link-procs). Form (b', frame form as
 * pt / esa): changeDemoProc / changeAutoJumpProc decide on the frame's first step only; on the frame's later steps the
 * execute does not consult them (only these two callers). The s93 form (b) held them only after an entry earlier in the
 * frame: a later step then read the first step's in-between m_ground_h and B ran one frame ahead in phase (jk93).
 * LKT60_DEMOCHG_NEGCTRL: the per-step form (negative control). dt = 1 (N = 1): never. */
static inline bool procChangeHeld() {
#ifdef LKT60_DEMOCHG_NEGCTRL
    return false;
#endif
    return !(t60::dt() == 1.0f || t60::first_step());
}

/* (2026-11-22; pair: Link's auto jump into the water). The
 * water entry (changeSwimProc 023F8F80, the same execute line as the two above) tests mWaterY - pos.y > 90 once per 30
 * Hz frame in the original, from the previous frame's end, and its midair branch then lifts Link by 90 and keeps the fall
 * speed: the dive's depth depends on where the test caught him. Per step, B's step 2 of frame 205 caught him 0.86 under
 * the threshold (A's frame 206 test 18.4 under), the dive ran 17.5 higher and SWIM_UP (and its sounds 1835 / 2840 /
 * 3807) came 3 frames early (A 217 / 222, B 214 / 219). Form (as procChangeHeld): changeSwimProc decides on the frame's
 * first step only, from the frame-start position (= A's). Negative control WWHD_T60_SWIMIN_NEGCTRL=1 (game build); dt
 * = 1 (N = 1): never. */
static inline bool swimInHeld() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_SWIMIN_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return false;
#endif
    return !(t60::dt() == 1.0f || t60::first_step());
}

/* (2026-11-23; gnd route: Link at life 0 with a fairy bottle, DAMAGE -> WAIT at
 * 852). changeDeadProc 023F7820 (the same execute line) tests life 0 with ModeFlg_DAMAGE off once per 30 Hz frame in the
 * original, before the dispatch: A's frame 852 still has the DAMAGE flag there (procDamage's end clears it in procWait_init,
 * after the test), so the LINK_DOWN fairy is made in frame 853 and its life count reaches the save in 853's meter (life
 * 20 visible to Link from 854: procWait's checkRestHPAnime ends WAITATOB and sets WAITS at 854). Per step, B's procDamage
 * ended on the first step of 852 and the tick30 step consulted changeDeadProc with the flag already off: the fairy came a
 * frame early, life 20 at 853 and WAITS set at 853 (anim 1.1 > tol 1 on 853-882). Form (as procChangeHeld): changeDeadProc
 * decides on the frame's first step only. Negative control WWHD_T60_DEADCHG_NEGCTRL=1 (game build); dt = 1 (N = 1):
 * never. */
static inline bool deadChangeHeld() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_DEADCHG_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return false;
#endif
    return !(t60::dt() == 1.0f || t60::first_step());
}
}  // namespace lkT60Bg
