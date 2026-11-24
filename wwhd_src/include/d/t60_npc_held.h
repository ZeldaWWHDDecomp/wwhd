/* t60_npc_held.h (WWHD, true 60 build, 2026-11-23): a whole-form NPC that Link
 * carries (Medli d_a_npc_md, Makar d_a_npc_cb1) on a frame's earlier steps. Decomp build only.
 * Separate file: actor sources keep thin calls.
 *
 * The NPC form (d/t60_npc.h) runs the whole execute once per 30 Hz frame on the tick30 step; its earlier steps are
 * frame interpolation picture steps (the skeleton drawn between the frame ends). Carried (actor_status 0x2000), Link places the NPC
 * on each of his steps (setGrabItemPos: current.pos / shape_angle), so the blend between frame ends cut the chord of his
 * walk / turn while his hands followed it (the carried pot / stone before d/t60_held_pic.h, the held weapons before).
 *Held, an earlier step is a step of its own (t60w::begin notes no picture step) and its picture is
 * the NPC's own setBaseMtx from this step's placement at dt 1 (base matrix, the morfs' calc, the attached models), through
 * t60hp::picture (the held picture, with Link's carry bounce where the carry form gives it). Picture only:
 * the actor's own bytes, the morf objects and their kept poses (mpTransformInfo / mpQuat) are put back after it; the
 * models' matrices keep the picture. held is false on the tick30 step (the tick30 run and the latch keep the form
 * before); the latch's `shown` is set as the form before sets it (n 0: the tick30 step puts old back alike).
 * dt 1 (N 1): nothing. Negative control WWHD_T60_NPCHELD_NEGCTRL=1: the form before (never held, frame interpolation). */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h"
#include <cstdlib>
#include <cstring>

namespace t60nh {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_NPCHELD_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
template <class A> static inline bool held(A* a) {
    return !t60::tick30() && (static_cast<u32>(a->actor_status) & 0x2000) != 0 && !nc();
}
template <class A> struct Logic {
    static void get(A*, f32*) {}
    static void set(A*, const f32*) {}
    static void mtx(A*) {} /* not held: the picture frame interpolation (generic) */
    static bool h(A* a) { return held(a); }
    static constexpr t60w::Ops<A> kOps = {0, 0, get, set, mtx, h};
};
enum { kJoints = 128, kObj = 0x100, kActor = 0x4400 };
/* a morf kept around the picture: its object bytes and its kept pose (mpTransformInfo 0x20 / joint, mpQuat 0x10 / joint).
 * Layouts by guest offset (the units see differing host declarations): mDoExt_McaMorf 0xC8 bytes, mpModel +0x90,
 * mpTransformInfo +0xA8, mpQuat +0xAC; mDoExt_McaMorf2 0xD4 bytes, mpModel +0x90, mpTransformInfo +0x9C, mpQuat +0xA0 */
struct Morf {
    u32 ea, size, offT, offQ;
};
static inline Morf morf1(const void* m) { return {m ? gabi::ea(m) : 0u, 0xC8, 0xA8, 0xAC}; }
static inline Morf morf2(const void* m) { return {m ? gabi::ea(m) : 0u, 0xD4, 0x9C, 0xA0}; }
static inline Morf none() { return {0, 0, 0, 0}; }
struct Keep {
    Morf m;
    u32 t, q, n;
    u8 obj[kObj], pose[kJoints * 0x20], quat[kJoints * 0x10];
};
static inline void keep(Keep& k, const Morf& m) {
    k.m = m;
    k.n = 0;
    if (!m.ea)
        return;
    memcpy(k.obj, gabi::at<u8>(m.ea), m.size);
    k.t = *gabi::at<be<u32>>(m.ea + m.offT);
    k.q = *gabi::at<be<u32>>(m.ea + m.offQ);
    const u32 model = *gabi::at<be<u32>>(m.ea + 0x90);
    const u32 n = (k.t && k.q && model) ? (u32)gabi::at<J3DModel>(model)->getModelData()->getJointNum() : 0;
    if (n && n <= kJoints) {
        k.n = n;
        memcpy(k.pose, gabi::at<u8>(k.t), n * 0x20);
        memcpy(k.quat, gabi::at<u8>(k.q), n * 0x10);
    }
}
static inline void restore(const Keep& k) {
    if (!k.m.ea)
        return;
    memcpy(gabi::at<u8>(k.m.ea), k.obj, k.m.size);
    if (k.n) {
        memcpy(gabi::at<u8>(k.t), k.pose, k.n * 0x20);
        memcpy(gabi::at<u8>(k.q), k.quat, k.n * 0x10);
    }
}
/* the execute's earlier-step tail (the Guard skipped): held, the NPC's setBaseMtx from this step's placement (a
 * picture); m0..m2: its morfs (none(): unused) */
template <class A> static inline BOOL picture(A* a, Morf m0, Morf m1, Morf m2) {
    static_assert(sizeof(A) <= kActor, "actor");
    if (t60::dt() == 1.0f || t60::tick30() || !held(a))
        return TRUE;
    if (t60w::Latch* e = t60w::find(gabi::ea(a))) /* the latch as the form before leaves it (see above) */
        if (e->model)
            e->shown = true;
    static u8 s_actor[kActor];
    static Keep s_k[3];
    memcpy(s_actor, gabi::at<u8>(gabi::ea(a)), sizeof(A));
    keep(s_k[0], m0), keep(s_k[1], m1), keep(s_k[2], m2);
    t60hp::picture(a, Logic<A>::kOps, +[](A* x) {
        const f32 dt = t60_set_dt(1.0f); /* the morfs' calc at dt 1: the original code (no snapshot) */
        x->setBaseMtx();
        t60_set_dt(dt);
    });
    restore(s_k[2]), restore(s_k[1]), restore(s_k[0]);
    memcpy(gabi::at<u8>(gabi::ea(a)), s_actor, sizeof(A));
    return TRUE;
}
}  // namespace t60nh
