#pragma once
/* d_a_player_at_t60.h (WWHD, true 60 build, decomp build only): Link's attack hit volumes, one sweep per 30 Hz frame
 * (dt = 1/N the one-sweep-per-frame attack rule).
 *
 * The original sets the sword's attack capsules (daPy_lk_c::setSwordAtCollision: mAtCps[0] root -> top, [1] the old
 * top -> top, [2] root -> the old top; "old" = the previous update's mSwordTopPos / mSwordRootPos) and the spin
 * attack's cylinder (mAtCyl: StartCAt on the cut's first AT frame, MoveCAt after it) once per frame, and registers them
 * once for the frame's collision Move: one straight sweep per 30 Hz frame from the frame's start to its end positions.
 * At N steps the play scene's Move runs on every step (d_s_play_8.cpp), so registering on every step checked N
 * per-step arcs: they follow the swing's curve and reach targets the 30 Hz chord misses (pair: a ChuChu
 * killed 24 frames earlier, 4.5 units outside the 30 Hz sweep), and against an unconverted target (tick30 Move only)
 * they checked the last step's arc alone (1 / N of the frame's sweep). The rule: hit checks work like the original;
 * only the drawing is smoother.
 *
 * Form (dt < 1 only; dt = 1 is the original, nothing here is consulted):
 *   - on the frame's first setCollision call the sword's old positions (execute copied the previous step's = the
 *     previous frame's end positions into mOldSword*) are kept as the frame's start (host memory, Link only);
 *   - an AT start (daPyRFlg0_UNK1) on any step of the frame is remembered for the frame;
 *   - the capsules / cylinder are set and registered (dCcS Set + the mass manager) on the frame's tick30 step only,
 *     from the frame's start positions (the start form, root -> top with no sweep, if the AT started in the frame);
 *     the frame's other steps register no attack volume (their effects, sounds and flags run as before).
 * Hit flags from the tick30 Move stay on the colliders until the next tick30 Move clears them (the colliders are not
 * in the other steps' lists), so the next frame's steps all read that frame's hit (the mixed-pair hit-flag rule's hold, naturally). */
#include "true60.h"

struct LkT60AtFrame {
    uint32_t stamp; /* t60_step_counter() of the frame's first call */
    uint32_t tag;   /* g_Counter.mCounter0 (the 30 Hz frame) */
    bool valid;
    bool start; /* an AT start (daPyRFlg0_UNK1) on a step of this frame */
    uint32_t top[3]; /* the raw words (bit-exact copies, as execute copies them) */
    uint32_t root[3];
};
static LkT60AtFrame s_lkT60At;

static inline uint32_t lkT60_atFrameTag() { return *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */ }

/* the frame's first call: keep the frame's start positions (the old positions execute just copied) */
static inline void lkT60_atCopyIn(uint32_t* dst, uint32_t ea) {
    for (int i = 0; i < 3; i++) {
        dst[i] = gabi::load<uint32_t>(ea + 4 * i);
    }
}
static inline void lkT60_atSwapWords(uint32_t* host, uint32_t ea) {
    for (int i = 0; i < 3; i++) {
        const uint32_t g = gabi::load<uint32_t>(ea + 4 * i);
        gabi::store<uint32_t>(ea + 4 * i, host[i]);
        host[i] = g;
    }
}

/* oldTop / oldRoot: guest addresses of mOldSwordTopPos / mOldSwordRootPos */
static inline void lkT60_atFrameEntry(uint32_t oldTop, uint32_t oldRoot) {
    if (t60::dt() == 1.0f) {
        s_lkT60At.valid = false;
        return;
    }
    LkT60AtFrame& s = s_lkT60At;
    const uint32_t now = t60_step_counter();
    if (t60::first_step() || !s.valid || s.tag != lkT60_atFrameTag() || now - s.stamp >= (uint32_t)t60::steps()) {
        s.stamp = now;
        s.tag = lkT60_atFrameTag();
        s.valid = true;
        s.start = false;
        lkT60_atCopyIn(s.top, oldTop);
        lkT60_atCopyIn(s.root, oldRoot);
    }
}

/* the attack volumes are set and registered on this step: dt 1 (every call) or the frame's tick30 step */
static inline bool lkT60_atStep() { return t60::dt() == 1.0f || t60::tick30(); }

/* the frame's tick30 step of a converted frame: the one place the frame's attack volumes are set */
static inline bool lkT60_atFrameSet() { return t60::dt() != 1.0f && t60::tick30(); }

/* an AT start (UNK1) on a step of a converted frame */
static inline void lkT60_atMarkStart() {
    if (t60::dt() != 1.0f && s_lkT60At.valid) {
        s_lkT60At.start = true;
    }
}

/* the AT started on an earlier step of this frame (only asked on the tick30 step of a converted frame) */
static inline bool lkT60_atStartedInFrame() { return t60::dt() != 1.0f && s_lkT60At.valid && s_lkT60At.start; }

/* swap the frame's start positions into mOldSword* for setSwordAtCollision (call twice: in, then back) */
static inline void lkT60_atSwapOld(uint32_t oldTop, uint32_t oldRoot) {
    if (!s_lkT60At.valid) {
        return;
    }
    lkT60_atSwapWords(s_lkT60At.top, oldTop);
    lkT60_atSwapWords(s_lkT60At.root, oldRoot);
}
