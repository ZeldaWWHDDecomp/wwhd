/* t60_objw.h (WWHD, true 60 build, 2026-11-18): the whole form for object actors without a
 * picture hook of their own (the whole-frame form rule + addendum; frame interpolation), through d/t60_whole.h. Decomp
 * build only. Separate file: actor sources keep thin
 * calls.
 *
 * An object whose frame decides from other actors or the collision (hits from the tick30 collision Move, Link's
 * position / grab / ride, switches, the event system) runs its whole execute once per 30 Hz frame on the tick30 step
 * at dt 1 from the frame-start state (t60w::Guard); the earlier steps change no logic state. The in-between picture
 * (base matrix and skeleton between the frame ends) comes from the generic mechanism frame interpolation (t60w::begin tells the
 * port the step is a picture step), so no values are latched (n 0) and the matrix step is empty. Same form as the NPC
 * form's d/t60_npc.h (t60npc::Logic).
 * Use: `t60w::Guard<A> g(this, t60objw::Logic<A>::kOps); if (g.skip) return TRUE;` at the execute's head (a background
 * actor's Execute(matrixOut) sets its output matrix first) and `g.model = true;` before its matrix step. A callback that
 * another actor calls on each of its steps (ride / push / BG callbacks) returns early with t60objw::cbSkip(). A draw that
 * changes logic state does so only on the tick30 step's draw (t60objw::drawStep(), rule 14). dt 1: the original.
 * Negative control: T60C_NEGCTRL (the execute on every step at dt). */
#pragma once
#include "d/t60_whole.h"
#include "f_pc/f_pc_t60_frame_start.h"
namespace t60objw {
/* a callback another actor calls on each of its steps: only the tick30 step's call counts */
static inline bool cbSkip() { return t60w::on() && !t60::tick30(); }
/* a draw-side state change: only on the tick30 step's draw (at N 1 and in the harness every step is a tick30 step) */
static inline bool drawStep() { return t60::tick30(); }
/* another converted actor's position as A reads it (the process-order read rule + addendum): true and `ea`'s frame-start position
 * (12 bytes) in out12 when it is later in the process line and has not executed on this step; false otherwise
 * (unconverted, earlier in the line, N = 1): the caller keeps its original live read (dt-1 check bit-exact) */
static inline bool fsPos(uint32_t ea, void* out12) { return t60_fs_read(ea, 1, out12); }
/* ... and its old.pos as A reads it (the previous frame's start position), under the same condition */
static inline bool fsOld(uint32_t ea, void* out12) { return t60_fs_read(ea, 2, out12); }
/* ... and, when it is EARLIER in the line (it has executed on this step), its old.pos as A reads it (this frame's start
 * position); false otherwise (later in the line, unconverted, N = 1) */
static inline bool fsOldEarlier(uint32_t ea, void* out12) { return t60_fs_read(ea, 3, out12); }
/* ... and its shape_angle (6 bytes, csXyz) as A reads it: its frame-start shape_angle when it is later in the line, under fsPos's
 * condition; false otherwise: the caller keeps its live read */
static inline bool fsShapeAngle(uint32_t ea, void* out6) { return t60_fs_read(ea, 4, out6); }
template <class A> struct Logic {
    static void get(A*, f32*) {}
    static void set(A*, const f32*) {}
    static void mtx(A*) {} /* the picture: frame interpolation (generic) */
    static bool held(A*) { return false; }
    static constexpr t60w::Ops<A> kOps = {0, 0, get, set, mtx, held};
};
}  // namespace t60objw
