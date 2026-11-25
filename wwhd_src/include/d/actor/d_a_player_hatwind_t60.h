/* d_a_player_hatwind_t60.h (WWHD, true 60 build; 2026-11-24). Decomp game
 * build only. Header only (no new functions or function-local statics in the
 * glued unit d_a_player_main_04.cpp).
 *
 * daPy_lk_c::setHatAngle runs the hat spring whole on the frame's tick30 step and reads the wind at the cap
 * root (dKyw_get_AllWind_vec at HATA: the global wind plus the point winds the wind sources registered). A reads it during
 * the frame's Link execute: the point winds of sources earlier in the process line at their frame F, later ones at F-1.
 * On B's tick30 step a converted source later in the line (wind_tag 0x187, the Wind Temple updrafts, its chase whole on
 * its first step since) has run frame F already: the hat read the bubble a frame ahead (glide route
 * the glide route, B switched in at its own origin, in: the hat words equal to A up to frame 136, the updraft's
 * edge, then 64 frames off, max 1425). The frame's first step samples the wind at HATA (its pose is the frame start
 * since: A's HATA) before the sources' tick30 work: every earlier-in-line source at F (converted: its
 * first step; unconverted ones run on the tick30 step, but A reads them at F too only when earlier in the line), every
 * later one at F-1, as A's read; the tick30 update uses that sample. dt 1 / N 1: the original call only (no extra reads).
 * NC: WWHD_T60_HATWIND_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60HatWind {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_HATWIND_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static bool s_valid = false;
static uint32_t s_stamp = 0;
static f32 s_dir[3], s_pow;
/* the frame's first step takes the sample (setHatAngle before its tick30 return) */
/* (step-based, not dt: inside execute an enclosing form can run at dt 1 on the first step) */
static inline bool wantSample() { return !s_nc && t60::steps() > 1 && t60::first_step(); }
static inline void keep(const cXyz* dir, const be<f32>* pow) {
    s_dir[0] = (f32)dir->x, s_dir[1] = (f32)dir->y, s_dir[2] = (f32)dir->z;
    s_pow = (f32)*pow;
    s_stamp = t60_step_counter();
    s_valid = true;
}
/* the tick30 update: the frame's first-step sample (false: read the wind live, the original call) */
static inline bool use(cXyz* dir, be<f32>* pow) {
    if (s_nc || t60::steps() <= 1 || t60::first_step() || !s_valid || t60_step_counter() - s_stamp >= t60::steps()) {
        return false;
    }
    dir->x = s_dir[0], dir->y = s_dir[1], dir->z = s_dir[2];
    *pow = s_pow;
    return true;
}
} // namespace lkT60HatWind
