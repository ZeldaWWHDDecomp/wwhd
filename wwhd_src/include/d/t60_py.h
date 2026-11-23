/* t60_py.h (WWHD, true 60 build; 2026-11-20, the process-order read rule addendum): Link's
 * transient status bits latched per 30 Hz frame for whole-form readers. Decomp build only.
 * Separate file: sources keep thin calls.
 *
 * Link runs per step and holds a one-step status flag (daPy_py_c mResetFlg0 +0x3C0: every bit is reset each step, e.g.
 * 0x8000 getGrabUpStart, 0x20 getGrabUpEnd, 0x400 right foot on ground, 0x20000 hammer quake) for
 * ONE of his steps; at 30 Hz it stands for his whole frame. A whole-form actor runs its frame on the tick30 step, after
 * Link's last step, and missed a flag Link set on an earlier step (ppotx: the pot's mode_carry never saw the grab-up
 * start, m685 / m686 / model unset from f216, Link proc 115 instead of 113, the pot broke 75 frames late). So the
 * executor notes Link's status after each of his steps (note: one port latch per set bit, keyed by the status word's
 * address, the 30 Hz frame tag), and a whole-form reader asks status_frame(player, mask): the live word with the bits
 * of `mask` OR-ed over the frame's steps. Only the reader changes (Link's own per-step code keeps re-reading his live
 * bits: no double trigger). N = 1 / unconverted Link / dt 1 frames: no notes, the live word (the original). */
#pragma once
#include "true60.h"
/* (2026-11-20, the process-order read rule addendum 2) the central form for every tick30-only reader: most read Link's raw
 * +0x3C0 through their own views (cc, kanban, ks, bl, bo, obj_ikada, npc_mk,
 * d_detect, npc_so, bpw / pw, bomb / bomb2, himo2, kb, npc_cb1 / md / os, obj_correct, obj_leaves, bridge, ...), so an
 * accessor is not enough: around each tick30-only execute (an actor not in kT60Converted on the tick30 step, in
 * fpcEx_Execute; a whole-form actor's tick30 run, in t60w::Guard) Link's word is swapped to the frame-OR of his steps
 * (accumulated after each of his steps by wwhd_t60py_note) and restored afterwards with the reader's own set / cleared
 * bits merged back (a writer such as d_kankyo_rain's POISON_CURSE keeps its write). Converted per-step actors keep the
 * live word. Storage and functions: once, in f_pc_executor.cpp (WWHD_T60_PY_IMPL), exported. Game build only; N = 1, an
 * unconverted Link (no notes this frame) and the verify harness: no swap (the original). */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60py_note(uint32_t player);
__attribute__((visibility("default"))) void wwhd_t60py_swap_begin();
__attribute__((visibility("default"))) void wwhd_t60py_swap_end();
}
static inline void t60py_swap_begin() { wwhd_t60py_swap_begin(); }
static inline void t60py_swap_end() { wwhd_t60py_swap_end(); }
static inline void t60py_note_host(uint32_t player) { wwhd_t60py_note(player); }
#else
static inline void t60py_swap_begin() {}
static inline void t60py_swap_end() {}
static inline void t60py_note_host(uint32_t) {}
#endif
#if defined(WWHD_T60_PY_IMPL) && defined(GABI_GAME_BUILD) && GABI_T60
namespace t60py_impl {
static uint32_t s_player = 0, s_tag = 0, s_acc = 0;
struct Saved { uint32_t live, orw; bool on; };
static Saved s_stack[16];
static int s_depth = 0;
static uint32_t s_step = 0; /* the step the stack belongs to: an aborted run (a whole-form begin without its end) leaves
                             * no entry behind for a later step (Link clears his word at his next update anyway) */
static inline uint32_t tag() { return *gabi::at<be<u32>>(0x101FF558); }
}  // namespace t60py_impl
extern "C" __attribute__((visibility("default"))) void wwhd_t60py_note(uint32_t player) {
    using namespace t60py_impl;
    if (t60::dt() == 1.0f || player == 0)
        return;
    const uint32_t t = tag();
    if (t != s_tag || player != s_player) {
        s_tag = t;
        s_player = player;
        s_acc = 0;
    }
    s_acc |= gabi::load<u32>(player + 0x3C0);
}
extern "C" __attribute__((visibility("default"))) void wwhd_t60py_swap_begin() {
    using namespace t60py_impl;
    if (s_step != t60_step_counter()) {
        s_step = t60_step_counter();
        s_depth = 0;
    }
    if (s_depth >= 16) {
        s_depth++;
        return;
    }
    Saved& sv = s_stack[s_depth++];
    sv.on = false;
    if (t60::steps() <= 1 || !t60::tick30() || s_player == 0 || s_tag != tag())
        return;
    sv.live = gabi::load<u32>(s_player + 0x3C0);
    sv.orw = sv.live | s_acc;
    if (sv.orw == sv.live)
        return;
    sv.on = true;
    gabi::store<u32>(s_player + 0x3C0, sv.orw);
}
extern "C" __attribute__((visibility("default"))) void wwhd_t60py_swap_end() {
    using namespace t60py_impl;
    if (s_depth <= 0 || s_step != t60_step_counter())
        return;
    s_depth--;
    if (s_depth >= 16)
        return;
    const Saved& sv = s_stack[s_depth];
    if (!sv.on)
        return;
    const uint32_t now = gabi::load<u32>(s_player + 0x3C0);
    const uint32_t set = now & ~sv.orw, clr = sv.orw & ~now;
    gabi::store<u32>(s_player + 0x3C0, (sv.live | set) & ~clr);
}
#endif
namespace t60py {
static const u32 kStatusOff = 0x3C0; /* daPy_py_c's status word (tsubo_mode Player::mStatus) */
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }
static inline u32 bitKey(u32 player, int b) { return (player + kStatusOff) ^ (0x40000000u | ((u32)b << 24)); }
/* the executor, after a converted Link's step (dt != 1): the bits set in his status word now */
static inline void note(u32 player) {
#ifndef T60C_NEGCTRL
    if (t60::dt() == 1.0f || player == 0)
        return;
    const u32 st = gabi::load<u32>(player + kStatusOff);
    const u32 tag = frameTag();
    for (int b = 0; b < 32; b++)
        if (st & (1u << b))
            wwhd_t60_latch_set(bitKey(player, b), tag);
    t60py_note_host(player);
#else
    (void)player;
#endif
}
/* a whole-form reader (tick30 step): Link's status word with the bits of `mask` as any of his steps this frame had them */
static inline u32 status_frame(u32 player, u32 mask) {
    u32 st = gabi::load<u32>(player + kStatusOff);
#ifndef T60C_NEGCTRL
    if (t60::steps() > 1) {
        const u32 tag = frameTag();
        for (int b = 0; b < 32; b++)
            if ((mask & (1u << b)) && !(st & (1u << b)) && wwhd_t60_latch_pending(bitKey(player, b), tag))
                st |= 1u << b;
    }
#endif
    return st;
}
}  // namespace t60py
