/* d_camera_key_t60.h (WWHD, true 60 build, 2026-11-19, rule: per-camera keying).
 * Decomp build only.
 *
 * The HD game runs two dCamera_c bodies on every logic step (TV and GamePad: camera_execute once per camera process;
 * yr37 ride log: rideCamera twice per step with two independent work states). The true60 camera forms keep host state
 * between calls (frame-start snapshots, first-step edges, eases, mode-start stamps). Held in one static, the second
 * camera's call overwrote what the first stored on the same step (and Run's frame-start copy restored one camera from
 * the other's snapshot). Every true60 camera host static is therefore a PerCam: one value per camera body.
 *
 * Form: camera_execute enters its body's slot for the whole Run / NotRun (leave before its return); the body gets a slot of a
 * small fixed table (kCams = 2, a third live body is a hard error); camera_delete frees the slot. A slot taken by a new
 * body starts every PerCam value at its initial value (a generation per slot). Outside camera_execute (the verify harness's
 * dt-1 check / N-step spec calls) slot 0 is used, as the one static before. dt = 1 (N = 1): the camera forms do not read their state;
 * the slot choice changes nothing. */
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace camera_key_t60 {
constexpr int kCams = 2;
inline uint32_t g_ea[kCams];  /* the body's guest address per slot (0: free) */
inline uint32_t g_gen[kCams] = {1, 1}; /* bumped when a body takes the slot */
inline int g_cur = 0;         /* the running camera's slot */

static inline int claim(uint32_t ea) {
    for (int i = 0; i < kCams; i++)
        if (g_ea[i] == ea)
            return i;
    for (int i = 0; i < kCams; i++)
        if (g_ea[i] == 0) {
            g_ea[i] = ea;
            g_gen[i]++;
            return i;
        }
    std::fprintf(stderr, "[t60] camera key: a third live camera body %08X (slots %08X %08X)\n", (unsigned)ea,
                 (unsigned)g_ea[0], (unsigned)g_ea[1]);
    std::abort();
}
/* camera_delete: the body is gone */
static inline void release(uint32_t ea) {
    for (int i = 0; i < kCams; i++)
        if (g_ea[i] == ea)
            g_ea[i] = 0;
}
/* camera_execute: the body's calls of this step use its slot (enter at its top, leave before its return; host calls,
 * no local object: the decomp build places a function's locals in guest memory) */
static inline void enter(uint32_t ea) { g_cur = claim(ea); }
static inline void leave() { g_cur = 0; }

/* one value per camera slot; s() is the running camera's */
template <class T> struct PerCam {
    T v[kCams];
    uint32_t gen[kCams];
    T init;
    PerCam() : v{}, gen{}, init{} {}
    explicit PerCam(const T& i) : v{}, gen{}, init(i) {}
    T& operator()() {
        const int s = g_cur;
        if (gen[s] != g_gen[s]) {
            v[s] = init;
            gen[s] = g_gen[s];
        }
        return v[s];
    }
};
/* arrays (T[n]) */
template <class T, int N> struct PerCamArr {
    T v[kCams][N];
    uint32_t gen[kCams];
    PerCamArr() : v{}, gen{} {}
    T* operator()() {
        const int s = g_cur;
        if (gen[s] != g_gen[s]) {
            for (int i = 0; i < N; i++)
                v[s][i] = T{};
            gen[s] = g_gen[s];
        }
        return v[s];
    }
};
} // namespace camera_key_t60
