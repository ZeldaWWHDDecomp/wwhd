/* d_mesg_routing_t60.h (WWHD, true 60 build): message requests at N steps (2026-11-18;
 * dt = 1/N the message-step rule, the first-step AI-timer rule; the same rule as's event orders, d_event_order_t60.h). Decomp
 * build only.
 *
 * fopMsgM_messageSet / scopeMessageSet / tactMessageSet / messageSetDemo are the requests an actor makes to open or
 * continue a message (talk, item get, telescope, Tact). The message status machine that answers them is run by the
 * message processes, which are unconverted: they run on the tick30 step only, so the status a request reads does not
 * move between a frame's steps unless a request moved it. At 30 Hz an actor asks once per frame. A converted actor runs
 * its execute on every step, so the same request reaches these functions N times a frame:
 *   - status IDLE: the first call opens the message (status 1); the later steps' calls find status 1 and answer
 *     MESG_ERROR, an answer A's caller never saw for this request;
 *   - status CONTINUES (the next message of a conversation): the status does not change, so every later step selects the
 *     same message again (the renderer restarts it), N times a frame;
 *   - scope / Tact: the renderer is started again on every step.
 * The rule: a converted caller's request on a later step of a frame that repeats, argument for argument, the request it
 * (or another converted caller) made to the same function on an EARLIER step of the SAME frame is the frame's one
 * request already made: it answers that request's result and writes nothing. Requests on the frame's first step, from
 * unconverted code (dt 1, the tick30 step only), at N = 1, a different request (other message number, other manager)
 * and two equal requests within one step (A's own repeats) run exactly as in the original.
 *
 * Host state (the split-actor state rule (1): this unit only): per function the last converted request (frame tag
 * g_Counter.mCounter0, step counter true60.h arguments, result); cleared on the frame's first step. */
#pragma once
#include "true60.h"

#if GABI_T60
namespace t60msr {
enum Fn { kMessageSet, kScope, kTact, kDemo, kCount };
struct Req {
    u32 frame, step, mgr, msgNo, lookAt, result;
    bool set;
};
static Req s_req[kCount];

static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); }

/* true: this call repeats the frame's earlier-step request (answer *o_result, write nothing) */
static inline bool repeat(Fn fn, u32 mgr, u32 msgNo, u32 lookAt, u32* o_result) {
    if (t60::dt() == 1.0f)
        return false;
    if (t60::first_step()) {
        for (Req& r : s_req)
            r.set = false;
        return false;
    }
    const Req& r = s_req[fn];
    if (!r.set || r.frame != frameTag() || r.step == t60_step_counter() || r.mgr != mgr || r.msgNo != msgNo ||
        r.lookAt != lookAt)
        return false;
    *o_result = r.result;
    return true;
}

/* the request ran on this step and answered `result` */
static inline u32 made(Fn fn, u32 mgr, u32 msgNo, u32 lookAt, u32 result) {
    if (t60::dt() == 1.0f)
        s_req[fn].set = false; /* an unconverted / N = 1 request: nothing to repeat */
    else
        s_req[fn] = Req{frameTag(), t60_step_counter(), mgr, msgNo, lookAt, result, true};
    return result;
}
} // namespace t60msr
#define T60MSR_REPEAT(fn, mgr, no, at, out) t60msr::repeat(t60msr::fn, mgr, no, at, out)
#define T60MSR_MADE(fn, mgr, no, at, result) t60msr::made(t60msr::fn, mgr, no, at, result)
#else
#define T60MSR_REPEAT(fn, mgr, no, at, out) false
#define T60MSR_MADE(fn, mgr, no, at, result) (result)
#endif
