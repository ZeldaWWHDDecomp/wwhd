/* d_event_order_t60.h (WWHD, true 60 build): the event order queue at N steps (2026-11-18;
 * dt = 1/N the message-step rule, the first-step AI-timer rule). Decomp build only.
 *
 * dEvt_control_c::order appends to the order queue (this + 0, 8 entries of 0x18, count +0xC0) that check empties
 * once per 30 Hz frame (the play scene's frame-level work). An actor asks for its event (talk, door, item, demo) from its
 * execute, once per frame at 30 Hz. A converted actor runs its execute on every step, so the same request reaches order
 * N times in a frame: the queue would hold it N times (and fill up: 8 entries, the 9th order is refused), which A never
 * shows. The rule: a converted caller's order on a later step of a frame that repeats, field for field, an order queued
 * on an EARLIER step of the SAME frame is the frame's one request already queued: it answers 1 (queued) and writes
 * nothing. Orders on the frame's first step, from unconverted code (dt 1, the tick30 step only), at N = 1, and two equal
 * orders within one step (A's own repeats) are queued exactly as in the original.
 *
 * Host state (the split-actor state rule (1): this unit only): the frame tag (g_Counter.mCounter0, t60conv.h frameTag) and the step
 * counter (true60.h) of the step that queued each slot; cleared on the frame's first step. */
#pragma once
#include "true60.h"
#include "d/d_event_readable.h"

#if GABI_T60
namespace t60evo {
struct Tag {
    u32 frame, step;
    bool set;
};
static Tag s_tag[8];

static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); }

/* true: this call repeats an order a converted caller queued on an earlier step of this frame (answer 1, write
 * nothing); the queue walk reads only the slots below the count */
static inline bool repeat(u32 i_this, s32 cnt, u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                          u32 eventIdx, u8 info) {
    if (t60::dt() == 1.0f)
        return false;
    if (t60::first_step()) {
        for (Tag& t : s_tag)
            t.set = false;
        return false;
    }
    const u32 frame = frameTag(), step = t60_step_counter();
    for (s32 i = 0; i < cnt && i < 8; i++) {
        const Tag& t = s_tag[i];
        if (!t.set || t.frame != frame || t.step == step)
            continue;
        const EventOrder* e = gabi::at<EventOrder>(i_this + i * 0x18);
        if (static_cast<u16>(e->mEventType) == (u16)eventType && static_cast<u16>(e->mPriority) == (u16)priority &&
            static_cast<u16>(e->mFlag) == (u16)flag && static_cast<u16>(e->mHindFlag) == (u16)hindFlag &&
            static_cast<u32>(e->mActor1) == ac1 && static_cast<u32>(e->mActor2) == ac2 &&
            static_cast<u16>(e->mEventId) == (u16)eventIdx && static_cast<u8>(e->mEventInfoIdx) == info)
            return true;
    }
    return false;
}

/* order() queued slot `cnt` on this step */
static inline void queued(s32 cnt) {
    if (cnt < 0 || cnt >= 8)
        return;
    if (t60::dt() == 1.0f)
        s_tag[cnt].set = false; /* an unconverted / N = 1 order in the slot: nothing to repeat */
    else
        s_tag[cnt] = Tag{frameTag(), t60_step_counter(), true};
}
} // namespace t60evo
#define T60EVO_REPEAT(i_this, cnt, a, b, c, d, e, f, g, h) t60evo::repeat(i_this, cnt, a, b, c, d, e, f, g, h)
#define T60EVO_QUEUED(cnt) t60evo::queued(cnt)
#else
#define T60EVO_REPEAT(i_this, cnt, a, b, c, d, e, f, g, h) false
#define T60EVO_QUEUED(cnt) ((void)0)
#endif
