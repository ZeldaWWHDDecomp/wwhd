/* f_pc_t60_picture.h (WWHD, true 60 build, 2026-11-20): the shown values of the whole forms only
 * around the draw (switch WWHD_T60_PIC_RESET, default OFF;).
 * Decomp build only.
 *
 * A whole form with shown values (d/t60_whole.h Ops n > 0; obj_barrel2's b2T60) moved its own current.pos / angles on the
 * frame's earlier steps for the picture, and every process that ran before it in the line on a later step read that
 * picture instead of A's state (pair: coming3 following its barrel; picreads: cc's pot). With the switch ON a
 * picture step only RECORDS its shown values here; fpcM_Management sets them for the step's draw (pre-pass before
 * fpcDw_Handler: values + the form's mtx) and puts the logic values back right after it (post-pass: values only; the
 * matrices keep the picture pose for the render, the actor's tick30 step re-runs its mtx from the frame start as
 * before). So the handlers, the play scene, every execute and the fapGm managers see logic state on every step.
 * Excluded: msw (its picture moves its background for its riders) and held actors (their
 * holder places them on every step). Debug check: an execute inside the draw window is logged (wwhd_t60_pic_window).
 * OFF: the records are never made and the passes do nothing: the original path. Never consulted at N = 1. */
#pragma once
#include "true60.h"
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
/* the switch (WWHD_T60_PIC_RESET=1), read once */
__attribute__((visibility("default"))) int wwhd_t60_pic_on(void);
/* a picture step's shown values for actor `ea`: apply(ea, ops, values, mtx) sets them (and runs the form's matrix step
 * when mtx), get(ea, ops, out) reads the logic values; n <= 8 */
__attribute__((visibility("default"))) void wwhd_t60_pic_record(uint32_t ea, const void* ops, int n, const float* shown,
                                                                void (*apply)(uint32_t, const void*, const float*, int),
                                                                void (*get)(uint32_t, const void*, float*));
__attribute__((visibility("default"))) void wwhd_t60_pic_pre(void);
__attribute__((visibility("default"))) void wwhd_t60_pic_post(void);
/* 1 between the pre- and post-pass (nothing that runs logic may run there) */
__attribute__((visibility("default"))) int wwhd_t60_pic_window(void);
/* the debug check's log (once): an execute of profile `prof` ran inside the draw window */
__attribute__((visibility("default"))) void wwhd_t60_pic_warn(int prof);
}
static inline bool t60pic_on() { return t60::steps() > 1 && wwhd_t60_pic_on() != 0; }
#else
static inline bool t60pic_on() { return false; }
static inline void wwhd_t60_pic_pre(void) {}
static inline void wwhd_t60_pic_post(void) {}
static inline int wwhd_t60_pic_window(void) { return 0; }
static inline void wwhd_t60_pic_warn(int) {}
#endif

#if defined(WWHD_T60_PICTURE_IMPL) && defined(GABI_GAME_BUILD) && GABI_T60
#include <cstdio>
#include <cstdlib>
namespace t60pic {
struct Rec {
    uint32_t ea;
    const void* ops;
    int n;
    float shown[8], saved[8];
    void (*apply)(uint32_t, const void*, const float*, int);
    void (*get)(uint32_t, const void*, float*);
};
enum { kMax = 256 };
static Rec s_rec[kMax];
static int s_n = 0;
static int s_window = 0;
static int s_on = -1;
static int s_warned = 0;
}  // namespace t60pic
extern "C" int wwhd_t60_pic_on(void) {
    using namespace t60pic;
    if (s_on < 0) {
        const char* e = getenv("WWHD_T60_PIC_RESET");
        s_on = (e && *e && *e != '0') ? 1 : 0;
        if (s_on)
            fprintf(stderr, "[t60] picture values only around the draw (WWHD_T60_PIC_RESET)\n");
    }
    return s_on;
}
extern "C" void wwhd_t60_pic_warn(int prof) {
    if (!t60pic::s_warned++)
        fprintf(stderr, "[t60] WARNING: an execute ran inside the picture draw window (profile %d)\n", prof);
}
extern "C" void wwhd_t60_pic_record(uint32_t ea, const void* ops, int n, const float* shown,
                                    void (*apply)(uint32_t, const void*, const float*, int),
                                    void (*get)(uint32_t, const void*, float*)) {
    using namespace t60pic;
    if (n > 8)
        n = 8;
    Rec* r = nullptr;
    for (int i = 0; i < s_n; i++)
        if (s_rec[i].ea == ea) {
            r = &s_rec[i];
            break;
        }
    if (!r) {
        if (s_n >= kMax)
            return; /* full: this actor is drawn at its logic values this step */
        r = &s_rec[s_n++];
    }
    r->ea = ea, r->ops = ops, r->n = n, r->apply = apply, r->get = get;
    for (int i = 0; i < n; i++)
        r->shown[i] = shown[i];
}
extern "C" void wwhd_t60_pic_pre(void) {
    using namespace t60pic;
    for (int i = 0; i < s_n; i++) {
        Rec& r = s_rec[i];
        r.get(r.ea, r.ops, r.saved);
        r.apply(r.ea, r.ops, r.shown, 1);
    }
    s_window = s_n > 0;
}
extern "C" void wwhd_t60_pic_post(void) {
    using namespace t60pic;
    for (int i = s_n - 1; i >= 0; i--) {
        Rec& r = s_rec[i];
        r.apply(r.ea, r.ops, r.saved, 0);
    }
    s_n = 0;
    s_window = 0;
}
extern "C" int wwhd_t60_pic_window(void) { return t60pic::s_window; }
#endif
