/* f_op_actor_mng_t60_cull.h (WWHD, true 60 build; 2026-11-19): an execute's culling check reads the
 * frame-end view (the first-step AI-timer rule extended: from pair, barrel 9). Decomp build only,
 *. Separate file: f_pc_executor.cpp and
 * f_op_actor_mng_2.cpp keep thin calls.
 *
 * fopAcM_cullingCheck tests an actor's cull box / sphere against the view the last draw left behind: the view matrix
 * (viewMtx), the J3D clipper (clipper, systemFar) and the HD second view's cull object. In the original an execute of
 * frame f reads the view of the draw after frame f - 1. At N > 1 the game draws after every step, so the frame's tick30
 * step read the view of the in-between picture draw (barrel2 / barrel / stone / tsubo ... move-cull their execute:
 * barrel 9 of stopped a frame apart).
 *
 * Form: the rate gate (fpcEx_Execute) copies the view state on the frame's first step, before its first execute (the
 * draw after the previous frame's tick30 step left it), and marks the executes. A culling check inside an execute swaps
 * the copy in for the check and the live view back after it; a check from a draw (fopAc_Draw, a draw method) reads the
 * live view as before. Storage and the functions are defined once, in f_pc_executor.cpp (WWHD_T60_CULL_IMPL), and
 * exported (the fast build makes a unit's hidden definitions local, true60.h). Never active at N = 1 (the original).
 * The copied view matrix is the port's exact camera view of the last logic pass when it has one (wwhd_t60_exact_view):
 * with frame interpolation the live view at the frame's first step is the blended in-between picture's
 *. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
/* the rate gate: an execute begins (1) / ends (0) (nested: a depth); on the frame's first step the first begin copies
 * the view state */
__attribute__((visibility("default"))) void wwhd_t60_cull_exec(int begin);
/* fopAcM_cullingCheck: swap the frame-end view in (1, returns 1 when it did) / back out (0) */
__attribute__((visibility("default"))) int wwhd_t60_cull_swap(int in);
/* the port (interp.cpp): the exact camera's view of the last logic pass (0x30 bytes; 0 = none, keep the live view) */
__attribute__((visibility("default"))) int wwhd_t60_exact_view(void* out);
}
static inline void t60_cull_exec(bool begin) {
    if (t60::steps() > 1)
        wwhd_t60_cull_exec(begin ? 1 : 0);
}
static inline bool t60_cull_swap_in() { return t60::steps() > 1 && wwhd_t60_cull_swap(1) != 0; }
static inline void t60_cull_swap_out() { wwhd_t60_cull_swap(0); }
#else
static inline void t60_cull_exec(bool) {}
static inline bool t60_cull_swap_in() { return false; }
static inline void t60_cull_swap_out() {}
#endif

#if defined(WWHD_T60_CULL_IMPL) && defined(GABI_GAME_BUILD) && GABI_T60
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace t60cull {
/* guest addresses (f_op_actor_mng.cpp data lines): viewMtx 0x30, clipper 0x58, systemFar 4, viewRoot -> second view */
enum : uint32_t { kViewMtx = 0x104B45F8, kClipper = 0x1048CFF0, kSystemFar = 0x1048D04C, kViewRoot = 0x101F95D0 };
enum : uint32_t { kMtxSize = 0x30, kClipSize = 0x58, kFarSize = 4, kSecondViewPtr = 0x1024, kSecondSize = 0x7EC };
static uint8_t s_mtx[kMtxSize], s_clip[kClipSize], s_far[kFarSize], s_second[kSecondSize];
static uint8_t s_live_mtx[kMtxSize], s_live_clip[kClipSize], s_live_far[kFarSize], s_live_second[kSecondSize];
static uint32_t s_secondEa = 0;   /* the second view the copy was taken of (0: none) */
static uint32_t s_copyStep = ~0u; /* step counter of the frame's first step the copy was taken on */
static int s_inExec = 0;          /* execute depth (the rate gate; a scene's execute runs its layer's processes) */
static int s_swapped = 0;
/* WWHD_T60_CULL_TRACE=1 (test aid): stderr lines "[t60cull] swaps N differ M" every 256 swaps; differ = the copied view
 * matrix was not the live one (the case the form exists for) */
static int s_trace = -1;
static uint32_t s_swaps = 0, s_differ = 0;
static inline uint32_t secondView() {
    const uint32_t root = gabi::load<uint32_t>(kViewRoot);
    if (root == 0) return 0;
    const uint32_t pp = gabi::load<uint32_t>(root + kSecondViewPtr);
    return pp == 0 ? 0 : gabi::load<uint32_t>(pp);
}
static inline void swapBytes(uint32_t ea, uint8_t* copy, uint8_t* save, uint32_t n) {
    std::memcpy(save, gabi::at<uint8_t>(ea), n);
    std::memcpy(gabi::at<uint8_t>(ea), copy, n);
}
}  // namespace t60cull

extern "C" __attribute__((visibility("default"))) void wwhd_t60_cull_exec(int begin) {
    using namespace t60cull;
    if (begin && t60::first_step() && s_copyStep != t60_step_counter()) {
        s_copyStep = t60_step_counter();
        std::memcpy(s_mtx, gabi::at<uint8_t>(kViewMtx), kMtxSize);
        /*: the live view here is what the last draw left, the tick30 pass's blended
         * in-between camera when frame interpolation draws; the frame's own (exact) view is the port's exact camera
         * view of that pass (pair: pot 47C58260 woke a frame late with the blended copy) */
        wwhd_t60_exact_view(s_mtx);
        std::memcpy(s_clip, gabi::at<uint8_t>(kClipper), kClipSize);
        std::memcpy(s_far, gabi::at<uint8_t>(kSystemFar), kFarSize);
        s_secondEa = secondView();
        if (s_secondEa) std::memcpy(s_second, gabi::at<uint8_t>(s_secondEa), kSecondSize);
    }
    if (begin)
        s_inExec++;
    else if (s_inExec > 0)
        s_inExec--;
}

extern "C" __attribute__((visibility("default"))) int wwhd_t60_cull_swap(int in) {
    using namespace t60cull;
    if (in) {
        /* only inside an execute, with a copy of this frame, the same second view, and not nested */
        if (s_inExec <= 0 || s_swapped || s_copyStep == ~0u || s_secondEa != secondView()) return 0;
        swapBytes(kViewMtx, s_mtx, s_live_mtx, kMtxSize);
        if (s_trace < 0) {
            const char* e = std::getenv("WWHD_T60_CULL_TRACE");
            s_trace = (e && *e == '1') ? 1 : 0;
        }
        if (s_trace) {
            s_swaps++;
            if (std::memcmp(s_mtx, s_live_mtx, kMtxSize) != 0) s_differ++;
            if ((s_swaps & 255u) == 0) std::fprintf(stderr, "[t60cull] swaps %u differ %u\n", s_swaps, s_differ);
        }
        swapBytes(kClipper, s_clip, s_live_clip, kClipSize);
        swapBytes(kSystemFar, s_far, s_live_far, kFarSize);
        if (s_secondEa) swapBytes(s_secondEa, s_second, s_live_second, kSecondSize);
        s_swapped = 1;
        return 1;
    }
    if (!s_swapped) return 0;
    std::memcpy(gabi::at<uint8_t>(kViewMtx), s_live_mtx, kMtxSize);
    std::memcpy(gabi::at<uint8_t>(kClipper), s_live_clip, kClipSize);
    std::memcpy(gabi::at<uint8_t>(kSystemFar), s_live_far, kFarSize);
    if (s_secondEa) std::memcpy(gabi::at<uint8_t>(s_secondEa), s_live_second, kSecondSize);
    s_swapped = 0;
    return 1;
}
#endif
