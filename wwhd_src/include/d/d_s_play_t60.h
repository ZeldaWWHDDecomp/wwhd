/* d_s_play_t60.h (WWHD, true 60 build): the hit-stop pause on a general step length (follow-up,,
 * dt = 1/N).
 *
 * The play scene's pause (dScnPly l_timers 101EACB6: pauseTimer, nextPauseTimer) is a 30 Hz mechanism: the scene
 * executes on the tick30 step only, and its calcPauseTimer moves nextPauseTimer into pauseTimer there. In the original
 * a hit (d_cc_uty at_power_check, the sword's hit stop) is read by an actor that executes after the scene, so the pause
 * starts with the next frame. A converted actor reads its hits on the frame's first step, before the tick30 scene:
 * without a latch the same frame was paused (from its tick30 step on), one frame early, and the frame's earlier steps
 * ran unpaused anyway.
 *   (a) a nextPauseTimer set on an in-between step (dt < 1, not the tick30 step) is latched with the 30 Hz frame tag
 *       (g_Counter.mCounter0); calcPauseTimer on that frame's tick30 step leaves the pending value for the next frame.
 *   (b) on in-between steps the actor pause check answers what the frame's calcPauseTimer will leave
 *       (t60_pause_timer_this_frame: the pending value, or the decremented timer; f_op_actor.cpp).
 *   (c) every process that runs on in-between steps asks (b)'s answer: the actors (fopAc_Execute) and the camera
 *       (fopCam_Execute: the camera process runs on every step since with the raw pause timer it ran the
 *       first N - 1 steps of a hit stop's first frame and froze on the tick30 step: N4) and the
 *       environment processes (fopKy_Execute: only when converted, e.g. KANKYO in the wind pairs).
 * dt = 1 and the tick30 step: no latch, the original answers. */
#pragma once
#include "true60.h"
#include "f_pc/f_pc_t60.h"

static inline u32 t60_pause_frame_tag() { return *gabi::at<be<u32>>(0x101FF558); }
static const u32 kT60PauseTimer = 0x101EACB6;     /* dScnPly l_timers.pauseTimer */
static const u32 kT60NextPauseTimer = 0x101EACB7; /* dScnPly l_timers.nextPauseTimer */

/* (a) call after a store to nextPauseTimer */
static inline void t60_next_pause_mark_set() {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(kT60NextPauseTimer, t60_pause_frame_tag());
}
/* (a) the stored nextPauseTimer was set on an in-between step of this frame: it starts next frame. Read by the play
 * scene (dt = 1 on its tick30 step), so no dt test: only in-between steps ever set the latch. */
static inline bool t60_next_pause_latched() {
    return wwhd_t60_latch_pending(kT60NextPauseTimer, t60_pause_frame_tag()) != 0;
}
/* (b) the pauseTimer the frame's calcPauseTimer will leave (the pending nextPauseTimer, else cLib_calcTimer's count) */
static inline s8 t60_pause_timer_this_frame() {
    s8 cur = *gabi::at<be<s8>>(kT60PauseTimer);
    s8 next = *gabi::at<be<s8>>(kT60NextPauseTimer);
    if (next != 0 && !t60_next_pause_latched())
        return next;
    return cur != 0 ? (s8)(cur - 1) : (s8)0;
}

/* (c) the frame's pause answer on any step; isPause is the process kind's dScnPly_ply_c::isPause (the native overlays'
 * part is asked without a timer that runs out this frame). dt = 1, the tick30 step and the play scene on the frame's
 * first step (the first-step frame-level rule: its calcPauseTimer has already run): the original check. */
template <class IsPause> static inline auto t60_frame_isPause(IsPause isPause) -> decltype(isPause()) {
    if (t60::dt() == 1.0f || t60::tick30() || t60_scene_first())
        return isPause();
    if (t60_pause_timer_this_frame() != 0)
        return 1;
    be<s8>& timer = *gabi::at<be<s8>>(kT60PauseTimer);
    s8 cur = timer;
    if (cur == 0)
        return isPause();
    timer = 0; /* the overlays' answer without the timer that runs out this frame */
    auto paused = isPause();
    timer = cur;
    return paused;
}

/* The world block of dScnPly_Draw on a general step length (world-block port step, 2026-11-16;
 * the port's runtime/src). The block steps world systems once per 30 Hz frame: the port holds it on
 * frame interpolation's in-between passes. A system converted here (a bit in kT60WorldConverted, only once its own code
 * and its movers - and the wind - are converted) steps on every logic step at the step's dt: the port lifts the hold on
 * the frame's earlier steps while the mask is not 0, and there only the converted systems run (each between
 * t60_set_dt(t60_step_dt()) and putting the previous value back; on the tick30 step too). Unconverted systems keep the
 * tick30 step at dt 1. With the mask 0 (today) the block is the original: tick30 step only (the port holds the rest). */
enum {
    kT60W_Vibration = 1u << 0,      /* dVibration_c::Run / Pause */
    kT60W_Magma = 1u << 1,          /* dMagma calc */
    kT60W_Grass = 1u << 2,          /* dGrass calc */
    kT60W_Tree = 1u << 3,           /* dTree calc */
    kT60W_Wood = 1u << 4,           /* dWood calc (bushes) */
    kT60W_Flower = 1u << 5,         /* dFlower calc */
    kT60W_Poison = 1u << 6,         /* dKyr_poison_light_colision */
    kT60W_BgMove = 1u << 7,         /* dBgS::Move (moving background collision) */
    kT60W_Snapshot = 1u << 8,       /* dSnap execute */
    kT60W_Particles = 1u << 9,      /* particle calc, direct draw, l_particleFrame */
    kT60W_All = (1u << 10) - 1,
    /* the mass-collision prepare runs when any of its users runs, on the tick30 step only (the one-sweep-per-frame attack rule: the users'
     * hit checks are once per 30 Hz frame, against what the tick30 step registered) */
    kT60W_MassUsers = kT60W_Grass | kT60W_Tree | kT60W_Wood | kT60W_Flower | kT60W_Poison,
};
static const u32 kT60WorldConverted = kT60W_Tree | kT60W_Wood | kT60W_Grass | kT60W_Flower | kT60W_Particles; /* (d_tree_t60.h), bushes (d_wood_t60.h), grass + flowers (d_grass_t60.h), particles (JParticle on dt: JPA_emitter_t60.h / JPA_particle_t60.h / JPA_field_t60.h / JPA_draw_t60.h; l_particleFrame per 30 Hz frame) */

/* the port's switch WWHD_T60_PTCL (wwhd_t60_ptcl, default 1; 0: the particles run as an unconverted world
 * system, the state before) */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) extern int wwhd_t60_ptcl;
}
static inline u32 t60_world_converted() { return wwhd_t60_ptcl ? kT60WorldConverted : (kT60WorldConverted & ~kT60W_Particles); }
#else
static inline u32 t60_world_converted() { return kT60WorldConverted; }
#endif

/* the system steps on this pass: the tick30 step (every system), or an earlier step of the frame (converted ones) */
static inline bool t60_world_runs(u32 sys) {
    return t60::tick30() || (t60_world_converted() & sys) != 0;
}
/* the dt a world system runs at: its step's share if converted, else 1 (it runs on the tick30 step only) */
static inline f32 t60_world_dt(u32 sys) {
    return (t60_world_converted() & sys) != 0 ? t60_step_dt() : 1.0f;
}

/* (game build only, defined in d_s_play_8.cpp: the port asks which systems step on the earlier steps, t60_world.h:
 * wwhd_t60_world_converted / wwhd_t60_world_all) */
