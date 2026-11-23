/* d_a_gnd_t60.h (WWHD, true 60 build, 2026-11-22): d_a_gnd*.cpp on a general step length
 * (dt = 1/N; / gnd_exec / gnd_move). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * Ganondorf (gnd, 0xF6): the final fight runs inside the JStudio demo (daGnd_Execute reads the demo object, Link's
 * state, the hits of the tick30 collision Move; parry windows and attack timers decide on frame counts). Whole form
 * (the whole-frame form rule + addendum, d/t60_objw.h), the bosses' form (bdk, bgn, bmd, btd, bst): the execute runs once per 30 Hz
 * frame on the tick30 step at dt 1 from the frame-start state, every frame-count window exactly A's; the earlier steps
 * change no logic state and are picture steps (frame interpolation). The draw's flash fades stay on the frame pass (t60_draw.h).
 *dt 1: the original. Negative control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"

#include <cstdlib>
#include <cstring>

/* (2026-11-23; the process-order read rule + addendum (a), f_pc/f_pc_t60_frame_start.h): the light-arrow defence's
 * shot read. gnd_move searches the shot (shot_s_sub: an arrow 0x1D8 / 0x1BE / 0x1B0 / 0xA9 with speedF >= 10) and
 * defends when it is within speedF x 10 of mArrowCheckPos. The arrow runs after Ganondorf in the process line, so A's
 * frame f reads its frame f - 1 end. At N steps the tick30 step read it after its first step of the same frame: on the
 * route Zelda's arrow launches on frame 316's first step (speedF 0 -> flight, halfway in), B's tick30 step found
 * it and Ganondorf took DEFENCE (0xA) at 316, A at 317; his whole action / mode timeline ran one frame early to 558,
 * his sword spheres' At (mWeponSph, set and registered on the tick30 step as in A) came a frame early, and Link's DAMAGE
 * at A 343 / 554 came at 342 / 553 (B's first step of the frame before: the tick30 Move of 341 / 552); his damage wait
 * then ended a frame early, his mCyl Tg came back at 598 (A 599) and the next hit (proc 0x11D, LARGE_DAMAGE) at 599
 * (A 600). Same with Ganondorf unconverted (he runs on the tick30 step either way). Form: for a shot later in the line
 * (converted, not executed on this step), the search's answer and the block's reads of its position and speedF are its
 * frame-start values (process-order read what 1 / 5): frame-start speedF < 10 -> no shot (shot_s_sub's test); else its frame-start
 * position and speedF are in place for the block (swapped in, the live bytes restored after it). An earlier or
 * unconverted shot, N = 1: the live read (the original). Not exact in one case: the live search returns the first
 * live-qualifying shot only, so a frame-start-qualifying shot after it in the line is not offered (no such scene seen).
 * Negative control WWHD_T60_GNDSHOT_NEGCTRL=1 (game build): the live read. */
namespace gndT60 {
struct ShotSwap {
    uint32_t ea;
    uint8_t pos[12];
    uint8_t spd[4];
    bool on;
    uint8_t fsPos[12], fsSpd[4]; /* the frame-start copies read */
};
/* host statics, not stack locals: the verify harness runs the candidate on a stack in guest memory, where a raw host
 * local is a guarded access (one Ganondorf: not re-entered) */
static ShotSwap s_gndT60Shot;
static inline bool shotNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_GNDSHOT_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
/* after the shot search: the shot A's frame reads (nullptr when A's search would not take it), its frame-start position
 * and speedF swapped in when it is later in the line */
static inline fopAc_ac_c* shotBegin(fopAc_ac_c* shot) {
    ShotSwap* const s = &s_gndT60Shot;
    s->on = false;
    if (shot == nullptr || shotNegctrl())
        return shot;
    const uint32_t ea = gabi::ea(shot);
    uint8_t* const pos = s->fsPos;
    uint8_t* const spd = s->fsSpd;
    if (!t60objw::fsSpeedF(ea, spd) || !t60objw::fsPos(ea, pos))
        return shot; /* earlier in the line, unconverted, N = 1: live */
    const uint32_t w = ((uint32_t)spd[0] << 24) | ((uint32_t)spd[1] << 16) | ((uint32_t)spd[2] << 8) | spd[3];
    if (gabi::f32_from_bits(w) < 10.0f)
        return nullptr; /* shot_s_sub on the frame-start state: !(speedF < 10) */
    s->ea = ea;
    std::memcpy(s->pos, gabi::at<uint8_t>(ea + 0x314), 12); /* fopAc_ac_c::current.pos */
    std::memcpy(s->spd, gabi::at<uint8_t>(ea + 0x370), 4);  /* fopAc_ac_c::speedF */
    std::memcpy(gabi::at<uint8_t>(ea + 0x314), pos, 12);
    std::memcpy(gabi::at<uint8_t>(ea + 0x370), spd, 4);
    s->on = true;
    return shot;
}
/* after the block: the shot's live bytes back */
static inline void shotEnd() {
    ShotSwap* const s = &s_gndT60Shot;
    if (!s->on)
        return;
    std::memcpy(gabi::at<uint8_t>(s->ea + 0x314), s->pos, 12);
    std::memcpy(gabi::at<uint8_t>(s->ea + 0x370), s->spd, 4);
    s->on = false;
}
}  // namespace gndT60
