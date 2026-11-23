/* d_a_machine_t60.h (WWHD, true 60 build, 2026-11-22): d_a_machine.cpp on a general step length (the conversion rules
 * 1.5, 1.6). Decomp build only.
 *
 * Whole form (the whole-frame form rule addendum, cross-actor reads; d/t60_objw.h): the machine's frame reads other actors' state:
 * set_speed eases speedF towards l_speed x the windmill's mAngle[1] / m_max_rot_speed (daWindMill_c, itself a whole-form
 * actor that writes mAngle[1] on the tick30 step, earlier in the line), and attack tests Link's position against the
 * search capsule. The tick30 step runs the original execute whole at dt 1 from the frame-start state (the windmill's and
 * Link's current state there, as A reads them); the earlier steps change no state (the picture: frame interpolation).
 * Replaces the per-step form (t60conv + the c_lib-real sweep): its set_speed on the frame's first
 * step read the windmill's previous-frame angle (pair on kaze:2:2 with the windmill poked: B's speedF a frame behind
 * A from frame 96, the machine 30 units ahead by frame 450).
 *
 * Not in kT60Converted: the profile is added after the paired runs. */
#pragma once
#include "d/t60_objw.h"
