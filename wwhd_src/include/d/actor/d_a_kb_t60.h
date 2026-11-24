/* d_a_kb_t60.h (WWHD, true 60 build, 2026-11-19): pig (d_a_kb.cpp, d_a_kb_exec.cpp,
 * d_a_kb_move.cpp, kb_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum, carryables form: the pig's frame decides from Link (his
 * attack hits and the anger counter, the carry check / throw, the bait demo), the tick30 collision hits and pushes
 * (m_cc_move, first-step push), the ground / water checks and the event system, and its eight timers, RNG draws and morf play
 * advance once per 30 Hz frame. So daKb_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision with the other actors' current state, the colliders registered
 * once for the tick30 collision Move (one-sweep-per-frame attack), the move once, a delete request at the frame's end (first-step AI-timer addendum 2). The
 * earlier steps change no logic state. Held while carried (mMode 1, carry_move): Link places it on every step, so no
 * picture step is announced for it then.
 * The picture: frame interpolation (n 0: no values latched). The morf is calculated in the execute; daKb_Draw makes the light
 * state (settingTevStruct) on the tick30 step's draw only (rule 14), the rest of the draw (btp entry, the anger tint
 * into the copy mTevStr) writes no logic state. daKb_Create's draw_SUB pose is the original at dt 1 (createBegin / createEnd).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"
#include "d/t60_held_pic.h" /* the held picture */
#include <cstdlib>

struct kb_class;
namespace kb_t60 {
static inline void get(kb_class*, f32*) {}
static inline void set(kb_class*, const f32*) {}
static inline void mtx(kb_class*) {}
/* carried by Link (carry_move): placed by its holder on every step (read only at dt != 1) */
static inline bool held(kb_class* a) { return *gabi::at<be<s16>>(gabi::ea(a) + 0x766) /* mMode */ == 1; }
static constexpr t60w::Ops<kb_class> kOps = {0, 0, get, set, mtx, held};
static inline const t60w::Ops<kb_class>& ops() { return kOps; }
/* the execute's earlier-step tail (the Guard skipped); held (carry_move), the model's base matrix
 * from this step's placement: draw_SUB (on an earlier step only the base TR matrix and the matrix stack scratch; its
 * mDrawOffsetY ease runs on the tick30 step only, t60_draw_frame_pass). The skeleton's animation stays the frame's.
 * d/t60_held_pic.h */
/* (pig route pigF, N 2): the tick30 run does not draw the carried pig at Link's
 * placement: after carry_move it adds the collision push and moves it (speed_pos_set, BG_check), draws it there
 * (draw_SUB) and only then puts the position back (fopAcM_checkCarryNow: the copy taken before the move). A draws every
 * frame at that moved place; B's earlier steps drew at the bare placement: Link standing still holding the pig, its
 * drawn root jumped 3.49 back and forth on every pass (pig - hand z -6.16 on the earlier passes, -2.69 on the tick30
 * passes), a sawtooth up to 0.5 while walking. The picture: the tick30 run notes the draw's offset from the place the
 * run found (the holder's placement), and a held earlier step draws at this step's placement plus the previous frame's
 * offset (current.pos raised around draw_SUB, the words put back). Host memory only, game build, N > 1. Negative control
 * WWHD_T60_KBHELD_NEGCTRL=1: the bare placement. */
#if defined(GABI_GAME_BUILD) && GABI_T60
struct HeldAdj {
    u32 actor = 0, entryActor = 0, step = 0;
    f32 entry[3] = {0, 0, 0}, d[3] = {0, 0, 0};
    bool on = false;
};
static HeldAdj s_adj;
static inline bool adjNc() {
    static const bool v = [] { const char* e = getenv("WWHD_T60_KBHELD_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
}
static inline f32 posf(kb_class* a, int i) { return (f32)*gabi::at<be<f32>>(gabi::ea(a) + 0x314 + 4u * i); }
/* the tick30 run's head (after the Guard): the place the run found */
static inline void runEntry(kb_class* a) {
    if (t60::steps() <= 1)
        return;
    s_adj.entryActor = gabi::ea(a);
    for (int i = 0; i < 3; i++)
        s_adj.entry[i] = posf(a, i);
}
/* just before the run's draw_SUB: the draw's offset from that place, while carried */
static inline void runDraw(kb_class* a) {
    if (t60::steps() <= 1)
        return;
    s_adj.on = s_adj.entryActor == gabi::ea(a) && held(a);
    if (!s_adj.on)
        return;
    s_adj.actor = gabi::ea(a);
    s_adj.step = t60_step_counter();
    for (int i = 0; i < 3; i++)
        s_adj.d[i] = posf(a, i) - s_adj.entry[i];
}
#else
static inline void runEntry(kb_class*) {}
static inline void runDraw(kb_class*) {}
#endif
static inline BOOL held_picture(kb_class* a, void (*drawSub)(kb_class*)) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    const u32 ea = gabi::ea(a);
    if (!adjNc() && s_adj.on && s_adj.actor == ea && t60_step_counter() - s_adj.step <= (u32)t60::steps() &&
        t60::dt() != 1.0f && !t60::tick30() && held(a) && !t60hp::nc()) {
        be<u32>* w = gabi::at<be<u32>>(ea + 0x314);
        u32 w0[3];
        for (int i = 0; i < 3; i++) {
            w0[i] = w[i];
            *gabi::at<be<f32>>(ea + 0x314 + 4u * i) = posf(a, i) + s_adj.d[i];
        }
        t60hp::picture(a, kOps, drawSub);
        for (int i = 0; i < 3; i++)
            w[i] = w0[i];
        return TRUE;
    }
#endif
    t60hp::picture(a, kOps, drawSub);
    return TRUE;
}
/* daKb_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace kb_t60
