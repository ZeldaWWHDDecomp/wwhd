/* c_damagereaction_facedown_t60.h (WWHD, true 60 build; frame 72, 2026-11-24): the
 * damage reaction's face-down flag once per 30 Hz frame. Decomp build only.
 * Separate file: the source keeps thin calls.
 *
 * dr_matrix_set (the end of damage_reaction, c_damagereaction.cpp) writes mFaceDown (a point 10000 ahead in the body
 * frame below current.pos); its readers (dr_joint_bg_check, dr_damage_set, dr_damage_anime: the face-down bend
 * mJntTargetAngle[19].z -0x960, the land terms, the recovery mode) all run before it in the same call. The original's
 * readers in frame f see frame f - 1's flag. At N > 1 a flag set on an earlier step reached the same frame's later steps:
 * the Moblin route on kan6c (moKm, Moblin 47C59A80, walking down the stairs): mFaceDown 1 from frame 71's first step, the
 * tick30 step's dr_damage_anime set the -2400 bend target in frame 71 where A sets it in 72; joint 19 a frame ahead
 * (A 0 / -1200 / -1800 at 71 / 72 / 73, B -1200 / -1800 / -1976), mLeftHandPos 9.5 off at 72 (the hand-pose row).
 * Form: the flag is written on the frame's tick30 step (its frame-end matrix, as A's); the earlier steps keep the value
 * the frame started with. dt = 1 (an unconverted enemy's one call per frame, on the tick30 step): written as before.
 * Negative control WWHD_T60_DRFACEDOWN_NEGCTRL=1 (game build): written on every step. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60drfd {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRFACEDOWN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: dr_matrix_set writes mFaceDown on this step */
inline bool write() { return nc() || t60::dt() == 1.0f || t60::tick30(); }
}  // namespace t60drfd
