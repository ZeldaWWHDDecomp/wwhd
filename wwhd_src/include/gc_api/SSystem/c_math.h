/* c_math (SSystem/SComponent/c_math.h), GameCube names -> WWHD.
 * Already in bindings.h: cM_ssin, cM_scos (sin/cos table 0x104A44F8), cM_atan2s, cM_rndF,
 * cM_rndFX, cM_rnd. */
#pragma once
#include "bindings.h"

/* GameCube inline: degrees -> s16 angle (x * 65536 / 360) */
inline s16 cM_deg2s(f32 deg) { return (s16)gabi::ftoi(deg * 182.04444885253906f); }
