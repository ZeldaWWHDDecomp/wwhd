/* t60conv.h (WWHD, true 60 build; 2026-11-17): the shared step helpers of actors converted by the
 * conversion tool (tools/t60conv). Decomp build only.
 *
 * Every helper returns the original's per-frame answer at dt = 1 (N = 1, the 30 Hz fallback, an unconverted actor, the
 * verification harness's dt-1 check pin), so the dt = 1 path runs the original arithmetic (rule 1.5.2). The forms follow the
 * worked conversions (dt = 1/N):
 *   t60c::first      the frame's FIRST step: AI timers, timer one-shots, decision draws (the first-step AI-timer rule)
 *   t60c::end        the frame's tick30 step: visual counters, visual draws, continuous (level) sounds (the per-step chance rule, 11)
 *   t60c::share(K)     an integer per-frame add, the step's truncated share and the rest on the tick30 step: every 30 Hz
 *                      frame adds exactly K (d_cloth_packet_t60.h cloth60_share, d_a_cc_t60.h cc_t60_step_s16)
 *   t60c::frameAdd   the same for a value that is not a constant (the tick30 step takes what the frame's earlier steps
 *                      did not; d_a_sail_t60.h sail60_wobbleAdd's idea, keyed by the field)
 *   t60c::step(v)      a per-frame float amount (a speed added to a position) for this step: v x dt (the per-step movement rule)
 *   t60c::damp(d)      a per-frame damping factor for this step: d^dt (the per-step smoothing rule; cloth60_drag)
 *   t60c::ratio(r)     a per-frame smoothing ratio for this step: 1 - (1 - r)^dt (the per-step smoothing rule; t60_ratio)
 *   t60c::hit(h, s, k) a collision hit read by converted code: the first step's hit held over the frame, a later
 *                      step's own hit still counts (the mixed-pair hit-flag rule)
 *   t60c::held(b,s,k)  a decision answer taken on the first step and held over the frame (the animation-driven decision rule form)
 *   t60c::heldInt    the same for a state / mode value (the frame's mode dispatched on every step)
 *   t60c::mark / unmark / marked  a mark one step of the frame sets for the frame's later steps (an arrival)
 *   t60c::glideBegin / glideStep  a value updated whole on the first step, shown on the line to its frame end value
 *                      on the later steps, exact on the tick30 step (ramps with clamps; tag_waterlevel)
 *   t60c::procBegin / procEnd  a procedure dispatch (member pointer or guest table) under the 30 Hz visual-motion rule (2): a procedure
 *                      set on a step inside the dispatch first runs its body on the next frame's first step (the rest
 *                      of the frame holds); one set before the dispatch on a later step runs on the tick30 step
 * Host state: per translation unit (the split-actor state rule (1): no state shared across units), keyed by guest address and site,
 * reset on the frame's first step. The live answer is the first argument so the original's reads come first (dt-1 check).
 *
 * Negative control (tools/t60conv): with T60C_NEGCTRL defined every helper takes the per-step original form (count,
 * draw and add on every step, nothing held); the tool's N-step spec run of that build must fail. */
#pragma once
#include "true60.h"
/* no standard headers here: <unordered_map> pulled stdio's ::remove into actor units that define a remove()
 * (d_a_obj_volcano, scale-up 2026-11-17); the per-frame host state uses the fixed table below */

namespace t60c {

static inline bool on() { return t60::dt() != 1.0f; }

#ifdef T60C_NEGCTRL
static inline bool first() { return true; }
static inline bool end() { return true; }
#else
/* the frame's first step (every step at dt = 1) */
static inline bool first() { return !on() || t60::first_step(); }
/* the frame's tick30 step (every step at dt = 1) */
static inline bool end() { return !on() || t60::tick30(); }
#endif

/* the 30 Hz frame tag (g_Counter.mCounter0; d_a_bk_t60.h bk_t60_frame_tag) */
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); }

/* an integer per-frame add for this step: exact per 30 Hz frame when the site runs on every step of the frame */
static inline s32 share(s32 perFrame) {
#ifdef T60C_NEGCTRL
    return perFrame;
#else
    const f32 dt = t60::dt();
    if (dt == 1.0f)
        return perFrame;
    const s32 part = (s32)((f32)perFrame * dt);
    if (t60::tick30())
        return perFrame - part * (s32)(t60::steps() - 1);
    return part;
#endif
}

struct AddEntry {
    u64 key;
    u32 tag;
    s32 given;
    bool used;
};
/* per-TU host table keyed by (guest address, site): an entry only matters within its 30 Hz frame (tag), so a slot whose
 * tag is not the current frame is free for another key (open addressing, linear probe) */
enum { kTabSize = 4096, kProbe = 64 };
struct Table {
    AddEntry e[kTabSize];
    AddEntry& at(u64 k, u32 tag) {
        u32 h = (u32)(k ^ (k >> 29) ^ (k >> 47)) * 2654435761u;
        AddEntry* free_slot = 0;
        for (u32 i = 0; i < kProbe; i++) {
            AddEntry& x = e[(h + i) & (kTabSize - 1)];
            if (x.used && x.key == k)
                return x;
            if (!free_slot && (!x.used || (x.tag != tag && x.tag != ~tag)))
                free_slot = &x;
        }
        AddEntry& x = free_slot ? *free_slot : e[h & (kTabSize - 1)];
        x.used = true;
        x.key = k;
        x.tag = ~tag ^ 1u;  /* not this frame: the caller starts it */
        x.given = 0;
        return x;
    }
};
static Table s_add;
static Table s_held;

static inline u64 key(const void* k, int site) { return ((u64)gabi::ea(k) << 16) ^ (u64)(u32)site; }

/* an integer per-frame add whose amount is computed on each step: the step's share of it, and on the tick30 step the
 * frame's amount less what the frame's earlier steps added (exact per 30 Hz frame) */
static inline s32 frameAdd(const void* field, int site, s32 perFrame) {
#ifdef T60C_NEGCTRL
    (void)field, (void)site;
    return perFrame;
#else
    const f32 dt = t60::dt();
    if (dt == 1.0f)
        return perFrame;
    const u32 tag = frameTag();
    AddEntry& e = s_add.at(key(field, site), tag);
    if (t60::first_step() || e.tag != tag) {
        e.tag = tag;
        e.given = 0;
    }
    s32 out;
    if (t60::tick30()) {
        out = perFrame - e.given;
        e.given = 0;
        e.tag = ~tag;
    } else {
        out = (s32)((f32)perFrame * dt);
        e.given += out;
    }
    return out;
#endif
}

/* a per-frame float amount for this step: v x dt */
static inline f32 step(f32 perFrame) {
#ifdef T60C_NEGCTRL
    return perFrame;
#else
    return on() ? gabi::fmuls_ppc(perFrame, t60::dt()) : perFrame;
#endif
}

/* a per-frame damping factor for this step: d^dt (factors outside (0, 1) as they are) */
static inline f32 damp(f32 d) {
#ifdef T60C_NEGCTRL
    return d;
#else
    if (!on() || !(d > 0.0f) || !(d < 1.0f))
        return d;
    return (f32)__builtin_pow((double)d, (double)t60::dt());
#endif
}

/* a per-frame smoothing ratio for this step */
static inline f32 ratio(f32 r) {
#ifdef T60C_NEGCTRL
    return r;
#else
    return on() ? t60_ratio(r, t60::dt()) : r;
#endif
}

/* a decision answer taken on the first step, held over the frame's later steps (a body that first runs on a later step
 * keeps the per-step answer) */
static inline bool held(bool live, int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
    return live;
#else
    if (!on())
        return live;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site), tag);
    if (t60::first_step()) {
        e.tag = tag;
        e.given = live ? 1 : 0;
        return live;
    }
    return e.tag == tag ? e.given != 0 : live;
#endif
}

/* a state (mode) the frame began with, held over the frame's later steps: a state change made on the first step
 * (a mode function gated to the first step) takes effect on the next frame, as in the original, and the later steps
 * keep running the frame's mode (2026-11-17, obj_volcano) */
static inline s32 heldInt(s32 live, int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
    return live;
#else
    if (!on())
        return live;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site) ^ 0x4000000000000000ull, tag);
    if (t60::first_step()) {
        e.tag = tag;
        e.given = live;
        return live;
    }
    return e.tag == tag ? e.given : live;
#endif
}

/* a mark set on a step of the frame and read by the frame's later steps (an arrival that ends the frame's motion:
 * wind_tag's path point). Clear it on the first step (unmark) before the first read: the harness keeps one frame tag. */
static inline void mark(int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site) ^ 0x2000000000000000ull, tag);
    e.tag = tag;
    e.given = 1;
#endif
}
static inline void unmark(int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site) ^ 0x2000000000000000ull, tag);
    e.tag = tag;
    e.given = 0;
#endif
}
static inline bool marked(int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
    return false;
#else
    if (!on())
        return false;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site) ^ 0x2000000000000000ull, tag);
    return e.tag == tag && e.given != 0;
#endif
}

/* a hit flag of the tick30 collision Move read by converted code: the first step's hit held, a later step's own hit
 * still counts (the mixed-pair hit-flag rule) */
static inline bool hit(bool live, int site, const void* k) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site;
    return live;
#else
    if (!on())
        return live;
    const u32 tag = frameTag();
    AddEntry& e = s_held.at(key(k, site) ^ 0x8000000000000000ull, tag);
    if (t60::first_step()) {
        e.tag = tag;
        e.given = live ? 1 : 0;
        return live;
    }
    return live || (e.tag == tag && e.given != 0);
#endif
}

/* a frame glide (2026-11-17; tag_waterlevel's ramped ease): a value whose per-frame update cannot be
 * split into steps exactly (a ramp with clamps and a reach threshold) is updated whole on the frame's first step, from
 * `before` to the frame's end value, and shown on the steps in between on the straight line between them; the tick30
 * step lands on the end value exactly. Usage:
 *     if (t60c::first()) { f32 before = X; <the original per-frame update of X>; t60c::glideBegin(&X, site, before); }
 *     else t60c::glideStep(&X, site);
 * dt = 1: first() holds on every step and both calls do nothing (the original). */
struct GlideEntry {
    u64 key;
    u32 tag;
    f32 from, to;
    u32 n;
    bool used;
};
struct GlideTable {
    GlideEntry e[1024];
    GlideEntry* at(u64 k, u32 tag, bool make) {
        u32 h = (u32)(k ^ (k >> 29) ^ (k >> 47)) * 2654435761u;
        GlideEntry* free_slot = 0;
        for (u32 i = 0; i < kProbe; i++) {
            GlideEntry& x = e[(h + i) & 1023u];
            if (x.used && x.key == k)
                return &x;
            if (!free_slot && (!x.used || x.tag != tag))
                free_slot = &x;
        }
        if (!make)
            return 0;
        GlideEntry& x = free_slot ? *free_slot : e[h & 1023u];
        x.used = true;
        x.key = k;
        return &x;
    }
};
static GlideTable s_glide;
template <class T> static inline void glideBegin(T* field, int site, f32 before) {
#ifdef T60C_NEGCTRL
    (void)field, (void)site, (void)before;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    GlideEntry* e = s_glide.at(key(field, site), tag, true);
    e->tag = tag;
    e->from = before;
    e->to = (f32)*field;
    e->n = 1;
    if (!t60::tick30())
        *field = e->from + (e->to - e->from) * t60::dt();
#endif
}
template <class T> static inline void glideStep(T* field, int site) {
#ifdef T60C_NEGCTRL
    (void)field, (void)site;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    GlideEntry* e = s_glide.at(key(field, site), tag, false);
    if (!e || e->tag != tag)
        return;  /* not begun this frame (a body that first runs on a later step): as it is */
    e->n++;
    if (t60::tick30())
        *field = e->to;
    else
        *field = e->from + (e->to - e->from) * ((f32)e->n * t60::dt());
#endif
}

/* a frame move (2026-11-17, rule: the tornado's x drift): a position integrated by a per-frame
 * velocity (fopAcM_posMove at dt) rounds N step adds where the original rounds one; at large coordinates that is a
 * systematic loss per frame (tornado x ~ -100000: one f32 step per frame, linear). The exact form: the frame's first step
 * latches the frame-start position; the tick30 step lands on the frame-start position + the whole frame's move (the
 * original's single add: the step's speed is the frame's speed); the earlier steps carry the start + the move x the
 * frame's dt so far, for display and the steps' reads only. Usage (x and z, around the move):
 *     t60c::moveBegin(&pos.x, &pos.z, site); fopAcM_posMoveF(...); t60c::moveEnd(&pos.x, &pos.z, &speed.x, &speed.z, site);
 * A move first run on a later step of the frame keeps the per-step form. dt = 1: both calls do nothing (the original). */
struct MoveEntry {
    u64 key;
    u32 tag;
    f32 x0, z0, span;
    bool used;
};
struct MoveTable {
    MoveEntry e[256];
    MoveEntry* at(u64 k, u32 tag, bool make) {
        u32 h = (u32)(k ^ (k >> 29) ^ (k >> 47)) * 2654435761u;
        MoveEntry* free_slot = 0;
        for (u32 i = 0; i < kProbe; i++) {
            MoveEntry& x = e[(h + i) & 255u];
            if (x.used && x.key == k)
                return &x;
            if (!free_slot && (!x.used || x.tag != tag))
                free_slot = &x;
        }
        if (!make)
            return 0;
        MoveEntry& x = free_slot ? *free_slot : e[h & 255u];
        x.used = true;
        x.key = k;
        x.tag = ~tag;
        return &x;
    }
};
static MoveTable s_move;
template <class P> static inline void moveBegin(P* x, P* z, int site) {
#ifdef T60C_NEGCTRL
    (void)x, (void)z, (void)site;
#else
    if (!on() || !t60::first_step())
        return;
    MoveEntry* e = s_move.at(key(x, site), frameTag(), true);
    e->tag = frameTag();
    e->x0 = (f32)*x;
    e->z0 = (f32)*z;
    e->span = 0.0f;
#endif
}
template <class P, class V> static inline void moveEnd(P* x, P* z, const V* vx, const V* vz, int site) {
#ifdef T60C_NEGCTRL
    (void)x, (void)z, (void)vx, (void)vz, (void)site;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    MoveEntry* e = s_move.at(key(x, site), tag, false);
    if (!e || e->tag != tag)
        return;  /* not begun on this frame's first step: as it is */
    e->span += t60::dt();
    if (t60::tick30()) {
        *x = e->x0 + (f32)*vx;
        *z = e->z0 + (f32)*vz;
        e->tag = ~tag;
    } else {
        *x = e->x0 + (f32)*vx * e->span;
        *z = e->z0 + (f32)*vz * e->span;
    }
#endif
}

/* the 30 Hz visual-motion rule (2) (2026-11-18): a procedure dispatch. Usage around the original dispatch statement:
 *     if (t60c::procBegin(site, this, t60c::procWord(PROC))) { <dispatch>; t60c::procEnd(site, this, t60c::procWord(PROC)); }
 * PROC = the procedure word (the member pointer, or the guest-table index). In the original a procedure set inside the
 * dispatch (setProc, its init, the dispatcher returns) first runs its body on the next frame: procEnd sees the word
 * change on a non-tick30 step and holds the dispatch for the frame's remaining steps. A word that changed between
 * steps outside the dispatch (an earlier call in the execute set it, so A falls through to the new body in the same
 * frame) holds the dispatch until the frame's tick30 step, where the new body runs. A setter that calls the new body
 * itself (falls through inside the dispatch) is outside this form: the tool flags it for review. */
template <class T> static inline u32 procWord(const T& v) {
    const unsigned char* b = reinterpret_cast<const unsigned char*>(&v);
    u32 h = 2166136261u;
    for (unsigned i = 0; i < sizeof(T); i++)
        h = (h ^ b[i]) * 16777619u;
    return h;
}
/* a guest member pointer (gabi::ptmf in guest memory): read through its be<> fields; the raw bytes of a guest object are
 * not host-addressable (the verify harness traps such reads: d_a_arrow) */
template <class F> static inline u32 procWord(const gabi::ptmf<F>& p) {
    const u32 w[3] = {(u32)(s32)(s16)p.d, (u32)(s32)(s16)p.i, (u32)p.f};
    return procWord(w);
}
static inline bool procBegin(int site, const void* k, u32 word) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site, (void)word;
    return true;
#else
    if (!on())
        return true;
    const u32 tag = frameTag();
    /* each entry claimed for this frame before the next lookup (a fresh slot counts as free until it carries the tag) */
    AddEntry& w = s_held.at(key(k, site) ^ 0x1000000000000000ull, tag);
    const bool freshW = w.tag != tag;
    w.tag = tag;
    AddEntry& h = s_held.at(key(k, site) ^ 0x0800000000000000ull, tag);
    const bool freshH = h.tag != tag;
    h.tag = tag;
    if (t60::first_step() || freshW || freshH) {
        w.given = (s32)word;
        h.given = 0;
        return true;
    }
    if (h.given == 1)
        return false;  /* set inside the dispatch on an earlier step: the new body starts next frame */
    if ((s32)word != w.given) {
        w.given = (s32)word;
        h.given = 2;   /* set outside the dispatch on a later step: the new body runs on the tick30 step */
    }
    return h.given == 2 ? t60::tick30() : true;
#endif
}
static inline void procEnd(int site, const void* k, u32 word) {
#ifdef T60C_NEGCTRL
    (void)k, (void)site, (void)word;
#else
    if (!on())
        return;
    const u32 tag = frameTag();
    AddEntry& w = s_held.at(key(k, site) ^ 0x1000000000000000ull, tag);
    w.tag = tag;
    AddEntry& h = s_held.at(key(k, site) ^ 0x0800000000000000ull, tag);
    h.tag = tag;
    if ((s32)word != w.given) {
        w.given = (s32)word;
        if (!t60::tick30())
            h.given = 1;
    }
#endif
}

}  // namespace t60c
