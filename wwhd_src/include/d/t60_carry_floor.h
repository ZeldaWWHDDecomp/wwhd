/* t60_carry_floor.h (WWHD, true 60 build, 2026-11-23): the picture floor of a
 * carryable's earlier step on the ground PLANE the frame-start ground check found. Decomp build only.
 * Shared by the bombs (d_a_bomb_t60.h, d_a_bomb2_t60.h); offered to t60cs
 * (d/t60_carry_step.h groundFloor, the flat form).
 *
 * t60cs::check floors an earlier step's predicted place at the frame-start ground HEIGHT (m_ground_h + m_ground_up_h).
 * On a slope that height is the ground under the frame start: a bomb rolling downhill was held there on the in-between
 * pass and dropped by the frame's slope drop on the tick30 pass (pair btG bs5, passes 3708-3721: in-between y step 0,
 * frame-end y step -1.8 / -1.3 / -0.8: a 30 Hz staircase on the roll). The Acch keeps the plane of the polygon it stood
 * on (m_pla +0x9C: normal x y z, d; set in GroundCheck with the ground hit, d_bg_s_acch.cpp GroundCheck): the floor is
 * that plane's height at the step's xz, y = -(nx x + nz z + d) / ny, plus m_ground_up_h. Used only when the plane
 * agrees with the frame-start check (its height at the frame-start xz within 1 unit of m_ground_h, ny > 0.3); else the
 * flat floor. A picture value only (the tick30 step's original CrrPos decides). dt 1: never called. */
#pragma once
#include "true60.h"

namespace t60cf {
/* the floor height under (x, z) from the Acch at `acch` (dBgS_Acch), the frame start's xz (x0, z0); false: no ground */
static inline bool floorAt(const void* acch, f32 x0, f32 z0, f32 x, f32 z, f32* y) {
    if (!acch)
        return false;
    const u32 ac = gabi::ea(acch);
    const f32 gh = *gabi::at<be<f32>>(ac + 0x94), up = *gabi::at<be<f32>>(ac + 0x8C);
    if (!(gh > -1.0e9f))
        return false;
    const f32 nx = *gabi::at<be<f32>>(ac + 0x9C), ny = *gabi::at<be<f32>>(ac + 0xA0), nz = *gabi::at<be<f32>>(ac + 0xA4),
              d = *gabi::at<be<f32>>(ac + 0xA8);
    *y = gh + up;
    if (!(ny > 0.3f))
        return true;
    const f32 h0 = -(nx * x0 + nz * z0 + d) / ny;
    if (!(h0 - gh < 1.0f && gh - h0 < 1.0f))
        return true;
    *y = -(nx * x + nz * z + d) / ny + up;
    return true;
}
}  // namespace t60cf
