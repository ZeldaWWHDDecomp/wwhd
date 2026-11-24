/* d_a_grid_t60.h (WWHD, true 60 build; 2026-11-18): the ship's sail cloth (d_a_grid.cpp,
 * profile 0xAB, created by the ship) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_grid.cpp keeps thin calls.
 *
 * the whole-frame form rule (springs frame-exact) with its addendum (cross-actor reads; the mant / barrel2
 * form, 2026-11-18: the grid couples with the ship): the sail's frame reads the ship (daShip_c's sail angle
 * through the grid's ship pointer; the ship writes the grid's position, angles, scale.y, furl and the forced wind angle
 * on every one of its steps, the ship executes earlier in the line), the wind (KYEFF, tick30), the camera eye (the alpha)
 * and the play's alternate-sail flag; its state is per-frame chases and counters (relativeWindAngle cLib_addCalcAngleS2,
 * openingGain / oscillationGain cLib_addCalc2 / addCalc0, the opening and oscillation timers, wavePhase / twistAngle
 * adds, the alpha's +-5 step), from which ho_move rebuilds the 85 cloth vertices and their normals. So the whole
 * daGrid_c::_execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: the ship's frame
 * end (what A reads), every chase, counter and the vertices once per frame, as the original. The earlier steps change no
 * logic state and show the 85 vertices (mPacket.mPos, which the draw's updateMaterial copies on every pass) moved on by
 * the previous frame's change times the frame's dt so far (visual); the tick30 step puts the frame-start words back
 * first. The ship keeps placing the grid (position, angles, scale) on every step, so the sail follows the hull per step.
 * No picture when the previous frame did not reach ho_move (sail down: scale.y < 0.06), the grid did not run in the
 * frame just before (or is a new process at a freed address), or a vertex jumped more than kJump in that frame.
 * Normals (2026-11-23): the earlier steps showed the extrapolated
 * vertices with the frame-start normals (mNrm, mBackNrm = -mNrm, which the draw lights the cloth with), so the shading
 * moved once a frame (a stair on the sail). Now the same picture extrapolates each normal by the previous frame's change
 * times the frame's dt so far, scaled back to its frame-start length (ho_move's normals are unit length), and mBackNrm is
 * its negation, as setBackNrm builds it; the tick30 step puts the frame-start normal words back with the vertices (the
 * whole run then rebuilds them: the frame end is the original's). WWHD_T60_GRID_NRM=0: normals held (as before; the
 * negative control). dt 1: never consulted (the original). Host memory per translation unit (rule
 * the split-actor state rule (1)). Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60conv.h"
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include "d/t60_draw_words.h"

namespace grid_t60 {

enum { kN = 85 * 3, kSlots = 4 };
static const f32 kJump = 500.0f;

static inline bool on() {
#ifdef T60C_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}

struct Latch {
    u32 actor, tag, pcid; /* tag: the frame whose start w0 holds; pcid: the process id at that address */
    u32 w0[kN];           /* frame-start words of mPacket.mPos */
    f32 dv[kN];           /* the previous frame's change */
    f32 span;             /* the frame's dt so far */
    bool model, shown;    /* the previous frame reached ho_move; an earlier step moved the words */
};
static Latch s_latch[kSlots];
/* follow-up: the normals' frame-start words and previous-frame change, per latch slot */
struct NrmLatch {
    u32 n0[kN];
    f32 dn[kN];
};
static NrmLatch s_nrm[kSlots];
static inline bool nrm_on() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_GRID_NRM");
        return !(e && e[0] == '0');
    }();
    return on;
}
static inline NrmLatch& nrm(const Latch* e) { return s_nrm[e - s_latch]; }
static float s_prevDt = 1.0f;
static bool s_whole = false; /* inside the tick30 step's whole run */

static inline be<u32>* words(daGrid_c* a) { return reinterpret_cast<be<u32>*>(&a->mPacket.mPos[0]); }
static inline be<f32>* floats(daGrid_c* a) { return reinterpret_cast<be<f32>*>(&a->mPacket.mPos[0]); }
static inline be<u32>* nwords(daGrid_c* a) { return reinterpret_cast<be<u32>*>(&a->mPacket.mNrm[0]); }
static inline be<f32>* nfloats(daGrid_c* a) { return reinterpret_cast<be<f32>*>(&a->mPacket.mNrm[0]); }
static inline be<f32>* bfloats(daGrid_c* a) { return reinterpret_cast<be<f32>*>(&a->mPacket.mBackNrm[0]); }
/* debug: WWHD_T60_GRIDLOG=path: per step after the grid's execute, step counter, tick30, vertex 42's place and normal */
static inline void gridlog(daGrid_c* a) {
    static FILE* f = [] {
        const char* p = getenv("WWHD_T60_GRIDLOG");
        return p ? fopen(p, "w") : (FILE*)nullptr;
    }();
    if (!f) return;
    be<f32>* v = floats(a);
    be<f32>* n = nfloats(a);
    fprintf(f, "%u %d %.4f %.4f %.4f %.6f %.6f %.6f\n", t60_step_counter(), t60::tick30() ? 1 : 0, (f32)v[126], (f32)v[127],
            (f32)v[128], (f32)n[126], (f32)n[127], (f32)n[128]);
}
static inline f32 asf(u32 w) {
    f32 f;
    __builtin_memcpy(&f, &w, 4);
    return f;
}

static inline Latch* find(u32 actor) {
    for (u32 i = 0; i < kSlots; i++)
        if (s_latch[i].actor == actor)
            return &s_latch[i];
    return 0;
}
static inline Latch* make(u32 actor, u32 tag, u32 pcid) {
    Latch* e = find(actor);
    if (e && e->pcid == pcid)
        return e;
    Latch* stale = e;
    for (u32 i = 0; !stale && i < kSlots; i++) {
        Latch& x = s_latch[i];
        if (x.actor == 0 || x.tag + 2 < tag)
            stale = &x;
    }
    e = stale ? stale : &s_latch[(actor >> 4) % kSlots];
    e->actor = actor;
    e->pcid = pcid;
    e->tag = ~0u;
    e->model = false;
    e->shown = false;
    for (int i = 0; i < kN; i++)
        e->dv[i] = 0.0f;
    return e;
}

/* the execute's head: true on a step before the tick30 step (the picture only); on the tick30 step: the frame-start
 * words back, dt 1 for the whole run, false */
static inline bool begin(daGrid_c* a) {
    if (!on()) {
        s_whole = false; /* a whole run that never reached its end leaves nothing behind */
        return false;
    }
    const u32 actor = gabi::ea(a), tag = t60c::frameTag();
    Latch* e = make(actor, tag, *gabi::at<be<u32>>(actor + 0x04) /* base_process_class::mBsPcId */);
    if (t60::first_step() || e->tag != tag) {
        if (e->tag + 1 != tag)
            e->model = false;
        e->tag = tag;
        be<u32>* w = words(a);
        for (int i = 0; i < kN; i++)
            e->w0[i] = w[i].get();
        be<u32>* nw = nwords(a);
        for (int i = 0; i < kN; i++)
            nrm(e).n0[i] = nw[i].get();
        e->span = 0.0f;
        e->shown = false;
    }
    if (!t60::tick30()) {
        e->span = gabi::fadds_ppc(e->span, t60::dt());
        if (e->model) {
            be<f32>* f = floats(a);
            for (int i = 0; i < kN; i++)
                f[i] = asf(e->w0[i]) + e->dv[i] * e->span;
            if (nrm_on()) { /* the normals with the vertices (unit length kept), the back normals their negation */
                NrmLatch& n = nrm(e);
                be<f32>* nf = nfloats(a);
                be<f32>* bf = bfloats(a);
                for (int i = 0; i < kN; i += 3) {
                    f32 v[3], l0 = 0.0f, l1 = 0.0f;
                    for (int k = 0; k < 3; k++) {
                        const f32 o = asf(n.n0[i + k]);
                        v[k] = o + n.dn[i + k] * e->span;
                        l0 += o * o;
                        l1 += v[k] * v[k];
                    }
                    const f32 r = l1 > 1e-12f ? __builtin_sqrtf(l0 / l1) : 1.0f;
                    for (int k = 0; k < 3; k++) {
                        nf[i + k] = v[k] * r;
                        bf[i + k] = 0.0f - v[k] * r;
                    }
                }
            }
            e->shown = true;
        }
        gridlog(a);
        return true;
    }
    if (e->shown) {
        be<u32>* w = words(a);
        for (int i = 0; i < kN; i++)
            w[i] = e->w0[i];
        if (nrm_on()) { /* the frame-start normals back (mBackNrm = -mNrm, as setBackNrm left it) */
            be<u32>* nw = nwords(a);
            be<f32>* bf = bfloats(a);
            for (int i = 0; i < kN; i++) {
                nw[i] = nrm(e).n0[i];
                bf[i] = 0.0f - asf(nrm(e).n0[i]);
            }
        }
    }
    s_prevDt = t60_set_dt(1.0f);
    s_whole = true;
    return false;
}

/* the execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture. model: the frame
 * ran ho_move (the vertices were rebuilt) */
static inline void end(daGrid_c* a, bool model) {
    if (!s_whole)
        return;
    s_whole = false;
    t60_set_dt(s_prevDt);
    Latch* e = find(gabi::ea(a));
    if (!e)
        return;
    e->model = model;
    be<f32>* f = floats(a);
    for (int i = 0; i < kN && e->model; i++) {
        const f32 d = (f32)f[i] - asf(e->w0[i]);
        if (!(__builtin_fabsf(d) <= kJump)) /* a jump (or a non-finite value): no picture next frame */
            e->model = false;
        e->dv[i] = d;
    }
    be<f32>* nf = nfloats(a);
    for (int i = 0; i < kN; i++)
        nrm(e).dn[i] = e->model ? (f32)nf[i] - asf(nrm(e).n0[i]) : 0.0f;
    gridlog(a);
}

/* (2026-11-23): _draw's material update reads the packet matrix (built from the place
 * the ship gave the grid on its newest step) and the 85 vertices; the hull (a J3DModel) is drawn at the world's time,
 * one step back at N 2. Around the update both go to the port (d/t60_draw_words.h): the step at the world's time.
 * Normals with the vertices (2026-11-24): the update also reads the normals (mNrm, and mBackNrm
 * for the back side), which the earlier steps' picture moves with the vertices (follow-up above). They follow
 * in the same block as the vertices (mPos, mNrm, mBackNrm are contiguous), so the port keeps them per step and draws the
 * vertices and normals of one and the same step at W (frame ends exact, as the vertices). NC: WWHD_T60_GRID_NRM_WORDS=0
 * (the vertices only: the normals of the newest step, as before). Debug: WWHD_T60_GRIDDRAWLOG=path (vertex 42's place
 * and normal as the update reads them, once per draw). */
static_assert(offsetof(daHo_packet_c, mNrm) == offsetof(daHo_packet_c, mPos) + kN * 4 &&
                  offsetof(daHo_packet_c, mBackNrm) == offsetof(daHo_packet_c, mNrm) + kN * 4,
              "grid packet: mPos, mNrm, mBackNrm contiguous");
static inline bool nrmWordsOn() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_GRID_NRM_WORDS");
        return !(e && e[0] == '0');
    }();
    return on;
}
/* Frame-end blend (2026-11-24): at N 2 the grid is drawn at W, one step back, so
 * on an earlier step's draw the frame's next end is kept already: the block (vertices and normals) is drawn as the blend
 * of the two frame ends at W's place in the frame (the port's kFrameEnd: normals renormalized), not as the earlier
 * step's extrapolated picture; frame ends as before. NC: WWHD_T60_GRID_FRAMEEND=0 (the step at W, as before). */
static inline bool frameEndOn() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_GRID_FRAMEEND");
        return !(e && e[0] == '0');
    }();
    return on;
}
static inline void griddrawlog(daGrid_c* a) {
    static FILE* f = [] {
        const char* p = getenv("WWHD_T60_GRIDDRAWLOG");
        return p ? fopen(p, "w") : (FILE*)nullptr;
    }();
    if (!f) return;
    be<f32>* v = reinterpret_cast<be<f32>*>(&a->mPacket.mPos[0]);
    be<f32>* n = reinterpret_cast<be<f32>*>(&a->mPacket.mNrm[0]);
    fprintf(f, "%u %d %.4f %.4f %.4f %.6f %.6f %.6f\n", t60_step_counter(), t60::tick30() ? 1 : 0, (f32)v[126], (f32)v[127],
            (f32)v[128], (f32)n[126], (f32)n[127], (f32)n[128]);
}
static inline void drawBegin(daGrid_c* a) {
    t60dw::begin(gabi::ea(a), &a->mPacket.mMtx, 48, t60dw::kMatrix);
    t60dw::begin(gabi::ea(a), &a->mPacket.mPos[0], kN * 4 * (nrmWordsOn() ? 3 : 1),
                 nrmWordsOn() && frameEndOn() ? t60dw::kFrameEnd | t60dw::kUnitVecs : t60dw::kFloats);
    griddrawlog(a);
}
static inline void drawEnd(daGrid_c* a) {
    t60dw::end(&a->mPacket.mPos[0]);
    t60dw::end(&a->mPacket.mMtx);
}

}  // namespace grid_t60
