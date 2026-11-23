/* true60 (the per-step chance rule): cM_rndChance, a per-30-Hz-frame chance drawn on a step of length g_dt (true60.h
 * t60_chance). Not for c_math.cpp itself (it defines cM_rnd). */
#pragma once
#include "bindings.h"
#include "true60.h"

/* one draw of cM_rnd (02019788) against the step's chance: at dt = 1 exactly `cM_rnd() < p` */
inline bool cM_rndChance(f32 p) { return cM_rnd() < t60_chance(p); }
