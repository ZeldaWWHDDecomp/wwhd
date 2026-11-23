/* LYT_t60.h (WWHD, true 60 build): the HD layouts' 60 Hz picture.
 *
 * The HD in-game UI (hdIngameUi_c::calc 027152B4, from the play scene's execute) runs once per 30 Hz frame on the
 * frame's first step at dt 1 (the first-step frame-level rule): every screen's state machine, counters and input decisions, and the
 * layout animation advance (nw::lyt::Animator::UpdateFrame 02872DFC, via hdParts_c::calcLayout -> hdPartsLyt_c::animate
 * / finishAnimate). That stays as it is: the logic is A's frame. The picture between frames is the port's: each such
 * update of an N > 1 frame is recorded (animator, frame before, frame after); on every step the port (runtime/src/t60/
 * t60_ui.cpp layout_present) shows the animators between the frame each applied in the previous frame and the one it
 * applied in this calc (at k/N), re-applies them to their panes, redoes the recorded pane matrix updates and puts every
 * frame back; on the tick30 step it re-applies them at exactly end (frame ends = A by construction), and
 * before any logic of a later step reads panes it re-applies end too. A destroyed animator is dropped from the record
 * (TransformAnimation / BoundAnimation::destroy). Game build only (the verification harness never records). Shared between units, so the table is the port's (extern "C"; true60.h's rule). */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
/* one first-step update of an N > 1 frame: the animator (EA), its frame and speed before, its frame after, play type
 * and end / wrap flags after */
__attribute__((visibility("default"))) void wwhd_t60_lyt_record(uint32_t animator, float before, float speed, float after,
                                                               uint32_t type, uint32_t flags);
__attribute__((visibility("default"))) void wwhd_t60_lyt_forget(uint32_t animator);
/* the same frame's pane matrix update of a layout (nw::lyt::Layout::updateMatrices 0287FBFC: the calc computes the
 * panes' matrices; the draw only reads them): the port keeps a copy of the draw context (0xC8 bytes) to redo it */
__attribute__((visibility("default"))) void wwhd_t60_lyt_matrices(uint32_t layout, uint32_t context, uint32_t force);
}
/* (every N: at N = 1 the port only traces them, WWHD_T60_LYT_TRACE, so a 30 Hz run can be compared with an N run) */
inline bool t60lyt_recording() { return t60::first_step() && t60::dt() == 1.0f; }
inline void t60lyt_forget(const void* animator) { wwhd_t60_lyt_forget(gabi::ea(animator)); }
#else
inline bool t60lyt_recording() { return false; }
inline void wwhd_t60_lyt_record(uint32_t, float, float, float, uint32_t, uint32_t) {}
inline void t60lyt_forget(const void*) {}
inline void wwhd_t60_lyt_matrices(uint32_t, uint32_t, uint32_t) {}
#endif
