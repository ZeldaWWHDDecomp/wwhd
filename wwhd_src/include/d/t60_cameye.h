/* t60_cameye.h (WWHD, true 60 build; 2026-11-20, fix on the Poe pair): the camera's
 * frame-start eye for a converted reader before the camera in the process line (the process-order read rule). Decomp build only.
 *
 * At 30 Hz a reader before the camera in the line reads the eye (dCamera_c + 0xDC) the camera left at the end of the
 * frame before. At N steps a whole-form reader (its logic on the tick30 step) or a reader on step k > 1 sees the eye
 * after the camera's step k - 1 of the same frame: a mid-frame eye A never reads (from frame 59: the Poe's
 * lantern place = hand + 150 x the vector to the eye). Form: the reader notes the eye on the frame's FIRST step (the
 * camera has not run in this frame yet: the frame-start eye; a whole-form reader in its skip step) and reads the note
 * on the frame's later steps. Valid only for readers before the camera in the line (a reader after it would note the
 * camera's step-1 eye). One latch per camera (host memory). N 1 (or no note in this frame): the live eye, the original. */
#pragma once
#include "true60.h"

namespace t60cam {
struct EyeLatch {
    uint32_t stamp; /* step counter of the frame's first step that noted it */
    u32 camera;     /* the dCamera_c (guest) */
    f32 eye[3];
    bool valid;
};
static EyeLatch s_eye[2];

/* on the frame's first step: note the camera's eye (no-op at N 1 and on later steps) */
static inline void noteFrameStartEye(u32 camera) {
    if (!camera || t60::steps() <= 1 || !t60::first_step())
        return;
    EyeLatch& e = s_eye[camera == s_eye[0].camera || !s_eye[0].valid ? 0 : 1];
    e.stamp = t60_step_counter();
    e.camera = camera;
    for (int i = 0; i < 3; i++)
        e.eye[i] = *gabi::at<be<f32>>(camera + 0xDC + 4 * i);
    e.valid = true;
}
/* the frame-start eye noted this frame, else nullptr (read the live eye then) */
static inline const f32* frameStartEye(u32 camera) {
    if (!camera || t60::steps() <= 1)
        return nullptr;
    for (const EyeLatch& e : s_eye)
        if (e.valid && e.camera == camera && t60_step_counter() - e.stamp < (uint32_t)t60::steps())
            return e.eye;
    return nullptr;
}
}  // namespace t60cam
