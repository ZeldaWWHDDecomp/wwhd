#pragma once
/* true60 (decomp build only, 2026-11-18): Link's crawl in the exact frame form
 *  (on pair: reaching the crawl hole 3-5 frames late is gameplay-visible, not a speed-ramp position-gap / turn-factor sampling
 * difference). Separate file: d_a_player_main_04 (execute) and d_a_player_main_06 (the crawl
 * bodies) keep thin calls.
 *
 * The 30 Hz frame of a crawl (procCrawlStart 0x0F, procCrawlMove 0x10, procCrawlAutoMove 0x11) is one body (the turn
 * toward the stick or the side wall, the crawl speed from the animation frame, crawlBgCheck's push out of the front /
 * back wall), one whole-frame move (posMove) and one wall resolution (mAcch.CrrPos) from the frame's start state. Run
 * per step, each step moved half a frame into the wall and was pushed out on its own (A's frame-end x jumps 9.6 units
 * at the crawl start, B's ~5 a step; along the counter A's push-outs swing ~8 units a crawl cycle, B's ~4), and the
 * side-wall auto move chased from per-step side checks: B reached the hole 3-5 frames late. Form:
 *   - the frame's tick30 step runs the crawl body at dt 1 (the original's one update: its turn, chases, counters and
 *     animation lookaheads with A's window) and the whole frame's posMove at dt 1, from the frame-start state: Link's
 *     place (current / old), his world matrices (model base matrix, mInvWorldMtx; the body's wall and roof probes are
 *     placed with the model base matrix) are put back first; the wall resolution and the rest of the execute follow
 *     as at every step. The frame end is A's;
 *   - the frame's earlier steps show the picture only: no body, no move; Link's place stays the frame start for his
 *     own logic and is shown (to the draw, the camera and the other actors, between his executes) moved on from it by
 *     the previous frame's change times the frame's dt so far (position, shape and current yaw);
 *   - animations play on every step as before (the body reads the frame end's animation state on the tick30 step,
 *     as A after its animeUpdate).
 * The form holds for a frame whose first step finds Link in a crawl body; a procedure change before the dispatch on
 * an earlier step (damage, demo) ends it for the rest of that frame (per-step form from the frame start). A body
 * reached on a step before tick30 without a first-step crawl frame (a crawl entered on a middle step at N 4) is skipped
 * as well and runs on the tick30 step. dt 1 (the original, dt-1 check): never consulted, no extra guest reads.
 * Negative control: LKT60_CRAWL_NEGCTRL (the per-step form). */
#include <cstdlib>
#include "true60.h"

static inline bool lkT60Crawl_on() {
#ifdef LKT60_CRAWL_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f && t60::steps() > 1;
#endif
}
static inline bool lkT60Crawl_isProc(int32_t p) { return p >= 0x0F && p <= 0x11; /* CRAWL_START .. CRAWL_AUTO_MOVE */ }
/* (on pair frame 133, rule as pt / esa): procLargeDamage (0x68) after its launch
 * (mProcVar0 0: the hit stop is over) is in the same whole-frame form. Its landing is a decision on the frame move's BG
 * check: A's one CrrPos line check (old.pos -> pos, LINE_CHECK) over the whole frame move cuts it at the ground
 * crossing (LINE_CHECK_HIT, f133: z +18.47 of 25); the per-step half lines end above the crossing, the half
 * step's ground check landed Link without the cut (B z +25, 6.1 units off from there on). In the form the body
 * (d_a_player_main_05 procLargeDamage: its own first-step record, the same predicate) and the move run once, on the
 * tick30 step at dt 1 from the frame start, so the one line check is A's. The launch frame itself (mProcVar0 counted
 * to 0 on its first step) stays per step. */
static inline bool lkT60Crawl_isLargeDamageAir(int32_t p, int32_t procVar0) { return p == 0x68 && procVar0 == 0; }
template <class L> static inline bool lkT60Crawl_isWhole(L* lk) {
    const int32_t p = (int32_t)lk->mCurProc;
    return lkT60Crawl_isProc(p) || lkT60Crawl_isLargeDamageAir(p, (int32_t)(s16)lk->mProcVar0);
}

/* ---- the crawl bodies (d_a_player_main_06): `if (lkT60Crawl_skip()) return TRUE; lkT60Crawl_open();` at the top,
 * every `return x;` as `return lkT60Crawl_close(x);` (plain calls: no scope object) ---- */
static float s_lkT60CrawlDt = 0.0f; /* the step's dt while a tick30 body runs at dt 1 (0: none) */
static inline bool lkT60Crawl_skip() { return lkT60Crawl_on() && !t60::tick30(); }
static inline void lkT60Crawl_open() {
    s_lkT60CrawlDt = lkT60Crawl_on() ? t60_set_dt(1.0f) : 0.0f; /* (a run cut short leaves nothing behind) */
}
static inline BOOL lkT60Crawl_close(BOOL r) {
    if (s_lkT60CrawlDt != 0.0f) {
        t60_set_dt(s_lkT60CrawlDt);
        s_lkT60CrawlDt = 0.0f;
    }
    return r;
}

/* ----: procLargeDamage's body in the whole-frame form (d_a_player_main_05): `if (lkT60Ld_skip(this)) return TRUE;
 * lkT60Ld_open();` at the top, every return as `return lkT60Crawl_close(x);`. The body's own record of the frame's
 * first step (this TU's host state; the execute's record is d_a_player_main_04's, same predicate, same step). ---- */
struct LkT60LdFrame {
    uint32_t tag;
    bool on;
};
static LkT60LdFrame s_lkT60Ld;
template <class L> static inline bool lkT60Ld_skip(L* lk) {
    if (!lkT60Crawl_on()) {
        return false;
    }
    const uint32_t tag = *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */
    if (t60::first_step()) {
        s_lkT60Ld.tag = tag;
        s_lkT60Ld.on = lkT60Crawl_isWhole(lk);
    } else if (s_lkT60Ld.tag != tag) {
        s_lkT60Ld.on = false;
    }
    return s_lkT60Ld.on && !t60::tick30();
}
static inline void lkT60Ld_open() {
    s_lkT60CrawlDt = lkT60Crawl_on() && s_lkT60Ld.on && t60::tick30() ? t60_set_dt(1.0f) : 0.0f;
}

/* ---- the execute (d_a_player_main_04): host state of the frame ---- */
struct LkT60CrawlFrame {
    uint32_t tag;      /* the frame (g_Counter.mCounter0) */
    bool on;           /* the frame is in the form */
    bool shown;        /* this step moved Link's place for the picture */
    uint32_t cur[5];   /* current place words at the frame start (pos x y z, angle x y, angle z + room) */
    uint32_t old[5];   /* old place words at the frame start */
    int16_t shape[3];  /* shape_angle x y z */
    uint32_t mtx[24];  /* model base matrix (12 words), mInvWorldMtx (12 words) */
    float span;        /* the frame's dt so far */
    float dpos[3];     /* the previous frame's change (picture) */
    float dsy, dcy;
    float cc[3];       /*: the collision push m_cc_move as the frame's first step found it (the tick30 Move's) */
    uint32_t bg[5];    /* mAcch.m_flags and m_gnd's poly info (4 words) at the frame start */
    int16_t ga;        /* mGroundAngle at the frame start */
    int32_t gc;        /* mGroundCode at the frame start */
};
/* (2026-11-24; a crawl down a slope, frame 90):
 * the whole-frame move on the tick30 step reads the BG check of the frame start, as A's one posMove reads its previous
 * frame end's. The earlier steps run no body and no move, but the execute's BG check (mAcch.CrrPos) still ran on them
 * from the unmoved place and left no ground hit and mGroundAngle 0 (B 90 s0: flags 0x2000, ga 0; A 89: 0x20E0, ga
 * 2670): the tick30 step's slope test (posMoveFromFootPos: speedF = v x cos(ground angle)) then took the flat speed, B's
 * crawl 3.3% faster down the slope (speedF 7.399 vs A 7.162; x 0.22 off at 90, 0.49 at 91). Form: the frame start's
 * mAcch flags, ground poly info, mGroundAngle and mGroundCode are put back with Link's place on the tick30 step, before
 * the body and the move; the tick30 step's own CrrPos then runs as before. Negative control
 * WWHD_T60_CRAWLBG_NEGCTRL=1 (game build): the earlier step's check as it stands (the form before). */
static inline bool lkT60Crawl_bgNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CRAWLBG_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct LkT60CrawlPrev {
    uint32_t tag;
    float pos[3];
    int32_t sy, cy;
    bool valid;
};
static LkT60CrawlFrame s_lkT60Crawl;
static LkT60CrawlPrev s_lkT60CrawlPrev;

static inline uint32_t lkT60Crawl_tag() { return *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */ }
static inline void lkT60Crawl_words(uint32_t dst, uint32_t src, int n) {
    for (int i = 0; i < n; i++) {
        gabi::store<uint32_t>(dst + 4 * i, gabi::load<uint32_t>(src + 4 * i));
    }
}
static inline void lkT60Crawl_save(uint32_t* out, uint32_t src, int n) {
    for (int i = 0; i < n; i++) {
        out[i] = gabi::load<uint32_t>(src + 4 * i);
    }
}
static inline void lkT60Crawl_load(uint32_t dst, const uint32_t* in, int n) {
    for (int i = 0; i < n; i++) {
        gabi::store<uint32_t>(dst + 4 * i, in[i]);
    }
}
static inline float lkT60Crawl_f(uint32_t w) {
    float f;
    __builtin_memcpy(&f, &w, 4);
    return f;
}

/* the execute's head (before anything reads Link's place). L: daPy_lk_c */
template <class L> static inline void lkT60Crawl_begin(L* lk) {
    LkT60CrawlFrame& c = s_lkT60Crawl;
    c.shown = false;
    if (!lkT60Crawl_on()) {
        c.on = false;
        return;
    }
    const uint32_t tag = lkT60Crawl_tag();
    const uint32_t curEa = gabi::ea(&lk->current), oldEa = gabi::ea(&lk->old);
    if (t60::first_step()) {
        /* the frame start (the framework copied current to old for this step) */
        LkT60CrawlPrev& p = s_lkT60CrawlPrev;
        const float x = lk->current.pos.x, y = lk->current.pos.y, z = lk->current.pos.z;
        const int32_t sy = (s16)lk->shape_angle.y, cy = (s16)lk->current.angle.y;
        const bool seq = p.valid && p.tag + 1 == tag;
        c.dpos[0] = seq ? x - p.pos[0] : 0.0f;
        c.dpos[1] = seq ? y - p.pos[1] : 0.0f;
        c.dpos[2] = seq ? z - p.pos[2] : 0.0f;
        c.dsy = seq ? (float)(s16)(sy - p.sy) : 0.0f;
        c.dcy = seq ? (float)(s16)(cy - p.cy) : 0.0f;
        p.tag = tag;
        p.pos[0] = x;
        p.pos[1] = y;
        p.pos[2] = z;
        p.sy = sy;
        p.cy = cy;
        p.valid = true;
        c.tag = tag;
        c.on = lkT60Crawl_isWhole(lk); /* (+ procLargeDamage in the air) */
        if (!c.on) {
            return;
        }
        c.span = 0.0f;
        lkT60Crawl_save(c.cur, curEa, 5);
        lkT60Crawl_save(c.old, oldEa, 5);
        c.shape[0] = (s16)lk->shape_angle.x;
        c.shape[1] = (s16)lk->shape_angle.y;
        c.shape[2] = (s16)lk->shape_angle.z;
        lkT60Crawl_save(c.mtx, gabi::ea(lk->mpCLModel->getBaseTRMtx()), 12);
        lkT60Crawl_save(c.mtx + 12, gabi::ea(&lk->mInvWorldMtx), 12);
        c.cc[0] = lk->mStts.m_cc_move.x;
        c.cc[1] = lk->mStts.m_cc_move.y;
        c.cc[2] = lk->mStts.m_cc_move.z;
        lkT60Crawl_save(c.bg, gabi::ea(&lk->mAcch) + 0x28, 1);     /* m_flags */
        lkT60Crawl_save(c.bg + 1, gabi::ea(&lk->mAcch) + 0xD4, 4); /* m_gnd (cBgS_PolyInfo, 0x10) */
        c.ga = (s16)lk->mGroundAngle;
        c.gc = (s32)lk->mGroundCode;
        return;
    }
    if (!c.on || c.tag != tag) {
        c.on = false;
        return;
    }
    /* a later step: the frame start back for Link's own logic */
    lkT60Crawl_load(curEa, c.cur, 5);
    lkT60Crawl_load(oldEa, c.old, 5);
    lk->shape_angle.x = c.shape[0];
    lk->shape_angle.y = c.shape[1];
    lk->shape_angle.z = c.shape[2];
    if (t60::tick30()) {
        lkT60Crawl_load(gabi::ea(lk->mpCLModel->getBaseTRMtx()), c.mtx, 12);
        lkT60Crawl_load(gabi::ea(&lk->mInvWorldMtx), c.mtx + 12, 12);
        if (!lkT60Crawl_bgNc()) { /* the frame start's BG check */
            lkT60Crawl_load(gabi::ea(&lk->mAcch) + 0x28, c.bg, 1);
            lkT60Crawl_load(gabi::ea(&lk->mAcch) + 0xD4, c.bg + 1, 4);
            lk->mGroundAngle = c.ga;
            lk->mGroundCode = c.gc;
        }
    }
}

/* after the procedure dispatch: an earlier step that left the crawl (a change before the dispatch) ends the form */
template <class L> static inline void lkT60Crawl_afterBody(L* lk) {
    LkT60CrawlFrame& c = s_lkT60Crawl;
    if (c.on && !t60::tick30() && !lkT60Crawl_isWhole(lk)) {
        c.on = false;
    }
}

/* posMove: skipped on the earlier steps; on the tick30 step at dt 1 (the whole frame). Use:
 * `if (!lkT60Crawl_moveSkip()) { f32 d = lkT60Crawl_moveOpen(); posMove(); lkT60Crawl_moveClose(d); }` */
static inline bool lkT60Crawl_moveSkip() { return s_lkT60Crawl.on && !t60::tick30(); }
static inline float lkT60Crawl_moveOpen() { return s_lkT60Crawl.on && t60::tick30() ? t60_set_dt(1.0f) : 0.0f; }

/* (pair frame 787-792; the first-step push rule): the frame's collision push. dCcS::Move runs after
 * every logic step (d_s_play_8.cpp) and clears the pushes it read; the tick30 Move's push for Link (an unconverted
 * partner: Puppet Ganon's body on) waits in m_cc_move for his next frame's first step, which consumes it in
 * posMove. A whole-form frame skips posMove on its earlier steps, so the first step's Move cleared the push before the
 * tick30 step's whole move: B lost (-29.6, +19.2) at 787 and every later frame of the knock flight (A: -31.2 / -23.5 /
 * -179.4 / -25.0 / -25.0 x, 282.6 apart at 792). Now the tick30 step's move adds the push the frame's first step found
 * (A's one push, once, at dt 1) to what the later Moves left (converted partners). Before posMove, tick30 step of a
 * whole-form frame only. Negative control WWHD_T60_LDCC_NEGCTRL=1 (game build): the push dropped (before). */
template <class L> static inline void lkT60Crawl_ccTick30(L* lk) {
    LkT60CrawlFrame& c = s_lkT60Crawl;
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_LDCC_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl || !c.on || !t60::tick30() || t60::first_step()) {
        return;
    }
    lk->mStts.m_cc_move.x = (f32)lk->mStts.m_cc_move.x + c.cc[0];
    lk->mStts.m_cc_move.y = (f32)lk->mStts.m_cc_move.y + c.cc[1];
    lk->mStts.m_cc_move.z = (f32)lk->mStts.m_cc_move.z + c.cc[2];
    c.cc[0] = c.cc[1] = c.cc[2] = 0.0f;
}
static inline void lkT60Crawl_moveClose(float d) {
    if (d != 0.0f) {
        t60_set_dt(d);
    }
}

/* before setWorldMatrix on an earlier step: Link's place shown moved on by the previous frame's change */
template <class L> static inline void lkT60Crawl_show(L* lk) {
    LkT60CrawlFrame& c = s_lkT60Crawl;
    if (!c.on || t60::tick30()) {
        return;
    }
    c.span = c.span + t60::dt();
    const float s = c.span;
    lk->current.pos.x = lkT60Crawl_f(c.cur[0]) + c.dpos[0] * s;
    lk->current.pos.y = lkT60Crawl_f(c.cur[1]) + c.dpos[1] * s;
    lk->current.pos.z = lkT60Crawl_f(c.cur[2]) + c.dpos[2] * s;
    lk->shape_angle.y = (s16)(c.shape[1] + (int32_t)(c.dsy * s));
    lk->current.angle.y = (s16)((s16)(c.cur[3] & 0xFFFF) + (int32_t)(c.dcy * s));
    c.shown = true;
}

/* the execute's end (after its l_debug copies): Link's own next execute starts from the frame start */
static inline void lkT60Crawl_end(cXyz& keepPos, csXyz& keepShape, csXyz& keepCurrent) {
    LkT60CrawlFrame& c = s_lkT60Crawl;
    if (!c.shown) {
        return;
    }
    c.shown = false;
    lkT60Crawl_load(gabi::ea(&keepPos), c.cur, 3);
    /* angle words: current (x y | z room); the keeps are csXyz (x y z) */
    keepCurrent.x = (s16)(c.cur[3] >> 16);
    keepCurrent.y = (s16)(c.cur[3] & 0xFFFF);
    keepCurrent.z = (s16)(c.cur[4] >> 16);
    keepShape.x = c.shape[0];
    keepShape.y = c.shape[1];
    keepShape.z = c.shape[2];
}
