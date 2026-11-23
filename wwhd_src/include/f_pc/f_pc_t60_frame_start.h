/* f_pc_t60_frame_start.h (WWHD, true 60 build, 2026-11-18): the frame-start state of a
 * converted actor for readers before it in the process line (the process-order read rule, from pairs).
 *Decomp build only.
 *
 * the process-order read rule: "A converted actor that reads a converted actor LATER in the process order reads its frame-start state
 * (its state at the previous tick30 step), as A reads it. A converted actor that reads one EARLIER in the order reads its
 * current state on the tick30 step (whole-frame form addendum). Together, a converted reader always sees what A sees."
 *
 * Why: at 30 Hz Link executes before the Bokoblin, so his frame f reads the Bokoblin's frame f - 1 end. At N steps his
 * step k reads the Bokoblin after ITS step k - 1 of the same frame: a decision the Bokoblin took on its first step (first-step AI-timer:
 * the jump) moved its eye before Link's tick30 step, and Link faced it one frame early (frame 50: B -29889, A
 * -30531; A turns at 51).
 *
 * Form: the rate gate (fpcEx_Execute) records, before a converted actor's execute on the frame's FIRST step, its eye and
 * position (byte copies) and, on every step, the step it last executed on. A reader asks for a target: when the target
 * has such a record of this frame and has not executed on the reader's step yet (it is later in the line), the reader
 * gets the frame-start copy; otherwise (unconverted, held at 30 Hz, earlier in the line, N = 1) the live state. One
 * table for every unit: the functions are defined once, in f_pc_executor.cpp (WWHD_T60_FRAME_START_IMPL), and exported
 * (the fast build makes a unit's hidden definitions local, true60.h). Never consulted at N = 1 (the original). */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
/* the rate gate: process `ea` executes on this step (converted: it runs at the step length; first: the frame's first
 * step; actor: it is an actor, eye / pos at 0x37C / 0x314) */
__attribute__((visibility("default"))) void wwhd_t60_fs_exec(uint32_t ea, int converted, int actor);
/* the reader: 1 and the 12 bytes of the target's frame-start eye (what 0), position (what 1) or old position (what 2:
 * old.pos as A's frame f reads it = the target's frame f - 1 start position) when the process-order read rule gives the frame-start state,
 * else 0; what 3: the target's frame-start position = its old.pos as A reads it when the target is EARLIER in the line
 * (it has executed on this step), else 0; what 4 (additive, 2026-11-21, rule for mbdoor ACT_ADJUSTMENT):
 * the 6 bytes of the target's frame-start shape_angle (fopAc_ac_c 0x328), under the what 0-2 condition (later in the line).
 * Fields / layout of what 0-3 unchanged; only readers that ask for what 4 see it. what 5 (additive,
 * 2026-11-23, the process-order read rule addendum (a): state as well as position): the 4 bytes of the target's frame-start speedF
 * (fopAc_ac_c 0x370), under the what 0-2 condition (later in the line) */
__attribute__((visibility("default"))) int wwhd_t60_fs_read(uint32_t ea, int what, void* out12);
}
static inline bool t60_fs_read(uint32_t ea, int what, void* out12) {
    return t60::steps() > 1 && ea != 0 && wwhd_t60_fs_read(ea, what, out12) != 0;
}
#else
static inline bool t60_fs_read(uint32_t, int, void*) { return false; }
#endif

#if defined(WWHD_T60_FRAME_START_IMPL) && defined(GABI_GAME_BUILD) && GABI_T60
#include <cstring>
namespace t60fs {
struct Entry {
    uint32_t ea;
    uint32_t snap;   /* step counter of the frame's first step the copy was taken on */
    uint32_t stamp;  /* step counter of the last step the process executed on (as converted) */
    uint8_t eye[12];
    uint8_t pos[12];
    uint8_t old[12]; /* old.pos as A reads it: the previous frame's start position (A: old = current at each execute) */
    uint8_t shp[6];  /* fopAc_ac_c::shape_angle at the frame start (what 4; additive field) */
    uint8_t spd[4];  /* fopAc_ac_c::speedF at the frame start (what 5; additive field) */
};
enum { kSize = 512, kProbe = 32 };
static Entry s_tab[kSize];
static uint32_t s_frameFirst = ~0u; /* step counter of the running frame's first step */
static uint32_t s_prevFrameFirst = ~0u; /* ... and of the previous frame's first step */
static inline uint32_t hash(uint32_t ea) { return (ea >> 4) * 2654435761u; }
static inline Entry* find(uint32_t ea) {
    const uint32_t h = hash(ea);
    for (uint32_t i = 0; i < kProbe; i++) {
        Entry& e = s_tab[(h + i) & (kSize - 1)];
        if (e.ea == ea)
            return &e;
        if (e.ea == 0)
            return nullptr;
    }
    return nullptr;
}
static inline Entry* place(uint32_t ea) {
    const uint32_t h = hash(ea);
    Entry* old = nullptr;
    for (uint32_t i = 0; i < kProbe; i++) {
        Entry& e = s_tab[(h + i) & (kSize - 1)];
        if (e.ea == ea || e.ea == 0)
            return &e;
        if (!old && e.snap != s_frameFirst)
            old = &e; /* a record of an earlier frame: free (no reader of this frame can want it) */
    }
    return old;
}
} // namespace t60fs

extern "C" __attribute__((visibility("default"))) void wwhd_t60_fs_exec(uint32_t ea, int converted, int actor) {
    using namespace t60fs;
    const uint32_t now = t60_step_counter();
    if (t60::first_step() && s_frameFirst != now) {
        s_prevFrameFirst = s_frameFirst;
        s_frameFirst = now;
    }
    if (!converted || t60::steps() == 1)
        return;
    if (t60::first_step()) {
        /* `actor` is given on the first step only (the rate gate asks fopAc_IsActor there); a later step updates the
         * stamp of an existing record (only actors have one). Before, the `actor` test sat above this branch, so no later
         * step stamped its record: on the tick30 step every target looked later in the line (what 0-2 gave the
         * frame-start copy for earlier targets too, what 3 never answered; kitD1_2 [kitadbg] fs 0 on every frame) */
        if (!actor)
            return;
        Entry* e = place(ea);
        if (!e)
            return;
        /* old.pos as A's frame reads it: A's previous execute copied old = current at the previous frame's start, i.e.
         * the previous frame's record position. Without a record of the previous frame (the target was not converted
         * or held then: it executed once that frame) the live old.pos is A's. The live field cannot serve at N > 1:
         * its last step set it to that step's start. */
        if (e->ea == ea && e->snap == s_prevFrameFirst)
            std::memcpy(e->old, e->pos, 12);
        else
            std::memcpy(e->old, gabi::at<uint8_t>(ea + 0x300), 12); /* fopAc_ac_c::old.pos */
        e->ea = ea;
        e->snap = now;
        std::memcpy(e->eye, gabi::at<uint8_t>(ea + 0x37C), 12); /* fopAc_ac_c::eyePos */
        std::memcpy(e->pos, gabi::at<uint8_t>(ea + 0x314), 12); /* fopAc_ac_c::current.pos */
        std::memcpy(e->shp, gabi::at<uint8_t>(ea + 0x328), 6);  /* fopAc_ac_c::shape_angle (what 4) */
        std::memcpy(e->spd, gabi::at<uint8_t>(ea + 0x370), 4);  /* fopAc_ac_c::speedF (what 5) */
        e->stamp = now;
        return;
    }
    Entry* e = find(ea);
    if (e && e->snap == s_frameFirst)
        e->stamp = now;
}

extern "C" __attribute__((visibility("default"))) int wwhd_t60_fs_read(uint32_t ea, int what, void* out12) {
    using namespace t60fs;
    const Entry* e = find(ea);
    if (!e || e->snap != s_frameFirst)
        return 0;
    /* what 3: the target is earlier in the line (it has executed on this step): its frame-start position, which is its
     * old.pos as A's frame f reads it then (A's execute this frame copied old = current at the frame's start) */
    if (what == 3) {
        if (e->stamp != t60_step_counter())
            return 0;
        std::memcpy(out12, e->pos, 12);
        return 1;
    }
    /* a copy of this frame, and the target has not executed on this step: it is later in the line */
    if (e->stamp == t60_step_counter())
        return 0;
    if (what == 4) {
        std::memcpy(out12, e->shp, 6);
        return 1;
    }
    if (what == 5) {
        std::memcpy(out12, e->spd, 4);
        return 1;
    }
    std::memcpy(out12, what == 0 ? e->eye : what == 1 ? e->pos : e->old, 12);
    return 1;
}
#endif
