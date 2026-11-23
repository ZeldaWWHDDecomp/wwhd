/* d_timer_t60.h (WWHD, true 60 build): timer creation requests at N steps (2026-11-18;
 * dt = 1/N the message-step rule; the same rule as's message requests, d_mesg_routing_t60.h, and's event
 * orders). Decomp build only.
 *
 * The timer itself (dTimer_c, a process: dTimer_Execute) is unconverted: it counts down mRestFrames on the tick30 step
 * only, once per 30 Hz frame, so limits, getRestTimeMs (frames * 1000 / 30) and the countdown sound thresholds stay
 * exact. Its controls (start / stop / restart / end / startDelay / deleteRequest) are status transitions: a repeat on a
 * later step finds the new status and writes nothing.
 * dTimer_createTimer / dTimer_createStockTimer are guarded by play's timer mode (mTimerInfo.mTimerMode), which the new
 * timer's _create sets: in the next frame's process creation (frame-level work, the first-step frame-level rule), not within the frame.
 * At 30 Hz an actor asks once per frame and the next frame's ask finds the mode set; a converted actor asking on every
 * step would queue N timer processes in one frame. The rule: a converted caller's request on a later step of a frame
 * that repeats, argument for argument, the request made to the same function on an EARLIER step of the SAME frame
 * answers that request's result (the process id) and creates nothing. Everything else runs as in the original.
 *
 * Host state (the split-actor state rule (1): this unit only): per function the last converted request (frame tag
 * g_Counter.mCounter0, step counter, arguments, result); cleared on the frame's first step. */
#pragma once
#include "true60.h"
#include <string.h>

#if GABI_T60
namespace t60tmr {
enum Fn { kCreate, kStock, kCount };
struct Req {
    u32 frame, step, args[8], result;
    bool set;
};
static Req s_req[kCount];

static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); }
static inline u32 bits(f32 v) {
    u32 u;
    memcpy(&u, &v, 4);
    return u;
}

static inline bool repeat(Fn fn, const u32 (&args)[8], u32* o_result) {
    if (t60::dt() == 1.0f)
        return false;
    if (t60::first_step()) {
        for (Req& r : s_req)
            r.set = false;
        return false;
    }
    const Req& r = s_req[fn];
    if (!r.set || r.frame != frameTag() || r.step == t60_step_counter() || memcmp(r.args, args, sizeof(args)) != 0)
        return false;
    *o_result = r.result;
    return true;
}

static inline u32 made(Fn fn, const u32 (&args)[8], u32 result) {
    Req& r = s_req[fn];
    if (t60::dt() == 1.0f) {
        r.set = false;
    } else {
        r.frame = frameTag();
        r.step = t60_step_counter();
        memcpy(r.args, args, sizeof(args));
        r.result = result;
        r.set = true;
    }
    return result;
}
} // namespace t60tmr
#define T60TMR_ARGS(...) const u32 t60Args[8] = {__VA_ARGS__}
#define T60TMR_REPEAT(fn, out) t60tmr::repeat(t60tmr::fn, t60Args, out)
#define T60TMR_MADE(fn, result) t60tmr::made(t60tmr::fn, t60Args, result)
#define T60TMR_F(v) t60tmr::bits(v)
#else
#define T60TMR_ARGS(...) ((void)0)
#define T60TMR_REPEAT(fn, out) false
#define T60TMR_MADE(fn, result) (result)
#define T60TMR_F(v) 0u
#endif
