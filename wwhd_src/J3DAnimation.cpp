/** J3D frame-controller initialization, frame crossings and playback.
 * GameCube names with the verified HD layout, load/store order and remainder arithmetic. */
#include "J3DAnimation.h"
#include "true60.h"

using FrameConstant = be<f32>;
static FrameConstant l_initialFrame;
static FrameConstant l_initialRate;
static FrameConstant l_frameEpsilon;

/* 027F2BC0 */
void J3DFrameCtrl::init(s16 end) {
    mEnd = end;
    mState = 0;
    mAttribute = EMode_LOOP;
    f32 initialFrame = l_initialFrame;
    mStart = 0;
    mFrame = initialFrame;
    f32 initialRate = l_initialRate;
    mLoop = 0;
    mRate = initialRate;
}

/* 027F2BF8 */
s32 J3DFrameCtrl::checkPass(f32 pass_frame) {
    // HD: preserve the original field snapshot order.
    f32 frame = mFrame;
    f32 rate = mRate;
    s16 start = mStart;
    s16 end = mEnd;
    s16 loop = mLoop;
    u8 attribute = mAttribute;
    f32 cur_frame = frame;
    // true60 (the animation crossing-check rule): the window is this step's update interval, next = cur + rate*dt as in update, so a
    // frame passed inside one 30 Hz frame is reported on exactly the sub-step whose update crosses it
    f32 next_frame = t60::dt() != 1.0f ? cur_frame + rate * t60::dt() : cur_frame + rate;

    switch (attribute) {
    case EMode_NONE:
    case EMode_RESET:
        if (next_frame < start) {
            next_frame = start;
        }

        if (next_frame >= end) {
            next_frame = end - f32(l_frameEpsilon);
        }

        if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        }

        if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    case EMode_LOOP:
        if (cur_frame < start) {
            while (next_frame < start) {
                if (loop - start <= 0.0f) {
                    break;
                }
                next_frame += loop - start;
            }

            if (next_frame <= pass_frame && pass_frame < loop) {
                return true;
            } else {
                return false;
            }
        } else if (end <= cur_frame) {
            while (next_frame >= end) {
                if (end - loop <= 0.0f) {
                    break;
                }
                next_frame -= end - loop;
            }

            if (loop <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        } else if (next_frame < start) {
            while (next_frame < start) {
                if (loop - start <= 0.0f) {
                    break;
                }
                next_frame += loop - start;
            }

            if ((start <= pass_frame && pass_frame < cur_frame) || (next_frame <= pass_frame && pass_frame < loop)) {
                return true;
            } else {
                return false;
            }
        } else if (end <= next_frame) {
            while (next_frame >= end) {
                if (end - loop <= 0.0f) {
                    break;
                }

                next_frame -= end - loop;
            }

            if ((cur_frame <= pass_frame && pass_frame < end) || (loop <= pass_frame && pass_frame < next_frame)) {
                return true;
            } else {
                return false;
            }
        } else if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        } else if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    case EMode_REVERSE:
    case EMode_LOOP_REVERSE:
        if (next_frame >= end) {
            next_frame = end - f32(l_frameEpsilon);
        }

        if (next_frame < start) {
            next_frame = start;
        }

        if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        }

        if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    default:
        return false;
    }
}

/* true60 (2026-11-21,'s pot pair): the frame of a converted frame's steps
 * from the frame start, not accumulated per step. added rate x dt per step; float rounding of that sum is not the
 * one add of the original frame (A: 10 frames x 0.9 reach 9.0 exactly, B: 20 x 0.45 = 8.999998), so an end test on the
 * accumulated frame (GRAB_UP's stop at end 9: A frame 226, B a frame late) answered differently. Form: a frame control
 * that plays from the frame's first step on, with the same rate and nothing else writing its frame between the steps,
 * shows frameStart + rate x (k x dt) on step k (k = 1 .. N; N x dt = 1 exactly for N a power of two, so the last step is
 * frameStart + rate, bit-equal to A's add). A frame or rate changed in between (setFrame, a new animation, a stop or a
 * bounce of the switch below), a second play in one step, a play first reached on a later step, or a slot taken by
 * another control: that play restarts the form from its current value (the per-step add for that step). The boundary
 * logic below (stop, loop wrap, bounce) is unchanged and runs on the computed frame. Host memory (a table keyed by the
 * control's guest address and the step counter); dt 1: never (the original). */
namespace j3d_t60 {
struct FcLatch {
    uint32_t ea;        /* the frame control (guest) */
    uint32_t firstStep; /* step counter of the latch's first play */
    uint32_t lastStep;  /* step counter of its last play */
    uint32_t start, rate, last; /* f32 bits: the frame before the first play, the rate, the frame the last play left */
    bool fromFrameStart;        /* the first play was on the frame's first step */
};
constexpr int kSlots = 4096;
static FcLatch s_fc[kSlots];
static uint32_t s_frameFirst = 0xFFFFFFFFu; /* step counter of the running frame's first step (any play saw it) */
static inline uint32_t bits(f32 v) { uint32_t u; __builtin_memcpy(&u, &v, 4); return u; }
static inline f32 flt(uint32_t u) { f32 v; __builtin_memcpy(&v, &u, 4); return v; }
static inline FcLatch& slot(uint32_t ea) { return s_fc[(ea >> 2) & (kSlots - 1)]; }
/* the step's frame before the boundary logic */
static inline f32 advance(uint32_t ea, f32 frame, f32 rate) {
    const uint32_t now = t60_step_counter();
    if (t60::first_step())
        s_frameFirst = now;
    FcLatch& L = slot(ea);
    const bool inFrame = now - s_frameFirst < (uint32_t)t60::steps();
    const bool cont = inFrame && L.ea == ea && L.lastStep + 1 == now && now - L.firstStep < (uint32_t)t60::steps() &&
                      L.rate == bits(rate) && L.last == bits(frame);
    if (!cont) {
        L.ea = ea;
        L.firstStep = now;
        L.start = bits(frame);
        L.rate = bits(rate);
        L.fromFrameStart = inFrame && t60::first_step();
    }
    L.lastStep = now;
    const f32 k = (f32)(now - L.firstStep + 1);
    return flt(L.start) + rate * (k * t60::dt());
}
/* after the boundary logic: the frame this play left */
static inline void note(uint32_t ea, f32 frame, f32 rate) {
    FcLatch& L = slot(ea);
    if (L.ea != ea || L.lastStep != t60_step_counter())
        return;
    L.last = bits(frame);
    if (L.rate != bits(rate)) /* a stop / bounce changed the rate: the next play restarts */
        L.lastStep = 0xFFFFFFFEu;
}
} // namespace j3d_t60

/* the frame start and rate of a control playing from this frame's first step, unchanged since (Link's frame-end
 * projections, d_a_player_anm_t60.h): 1 = valid */
extern "C" __attribute__((visibility("default"))) int wwhd_t60_fc_frame_start(uint32_t ea, float* start, float* rate) {
    using namespace j3d_t60;
    if (t60::dt() == 1.0f)
        return 0;
    const FcLatch& L = slot(ea);
    const uint32_t now = t60_step_counter();
    if (L.ea != ea || !L.fromFrameStart || L.lastStep != now || now - s_frameFirst >= (uint32_t)t60::steps() ||
        L.firstStep != s_frameFirst)
        return 0;
    const J3DFrameCtrl* fc = gabi::at<J3DFrameCtrl>(ea);
    if (bits((f32)fc->mFrame) != L.last || bits((f32)fc->mRate) != L.rate)
        return 0;
    *start = flt(L.start);
    *rate = flt(L.rate);
    return 1;
}

/* 027F2FC4 */
void J3DFrameCtrl::update() {
    // HD: cached values survive the remainder call; only end and loop are reloaded below.
    f32 frame = mFrame;
    f32 rate = mRate;
    s16 start = mStart;
    s16 end = mEnd;
    s16 loop = mLoop;
    u8 attribute = mAttribute;
    u8 state = 0;
    mState = state;
    // true60 (the animation crossing-check rule and 7): the play rate is per 30 Hz frame, a step advances rate*dt; the stop/loop flags in
    // mState stay a per-update latch (set on the sub-step whose update stops or loops)
    if (t60::dt() != 1.0f)
        frame = j3d_t60::advance(gabi::ea(this), frame, rate); /* true60: from the frame start (see above) */
    else
        frame += rate;
    mFrame = frame;
    switch (attribute) {
    case EMode_NONE:
        if (frame < start) {
            frame = start;
            mFrame = frame;
            rate = 0.0f;
            mRate = rate;
            state |= STATE_STOP_E;
            mState = state;
        }
        if (frame >= end) {
            frame = end - f32(l_frameEpsilon);
            mFrame = frame;
            rate = 0.0f;
            mRate = rate;
            state |= STATE_STOP_E;
            mState = state;
        }
        break;
    case EMode_RESET:
        if (frame < start) {
            frame = start;
            mFrame = frame;
            rate = 0.0f;
            mRate = rate;
            state |= STATE_STOP_E;
            mState = state;
        }
        if (frame >= end) {
            frame = start;
            mFrame = frame;
            rate = 0.0f;
            mRate = rate;
            state |= STATE_STOP_E;
            mState = state;
        }
        break;
    case EMode_LOOP: {
        // HD: one double-precision remainder per crossed boundary; loop state stays clear.
        if (frame < start) {
            f32 span = f32(loop - start);
            if (span > 0.0f) {
                f64 rem = J3DFrameCtrl_fmod(f64(f32(frame - start)), f64(span));
                frame = f32(rem + f64(span));
                mFrame = frame;
            }
        }
        end = mEnd;
        if (!(frame < end)) {
            loop = mLoop;
            f32 span = f32(end - loop);
            if (span > 0.0f) {
                f64 rem = J3DFrameCtrl_fmod(f64(f32(frame - end)), f64(span));
                mFrame = f32(rem);
            }
        }
        break;
    }
    case EMode_REVERSE:
        if (frame >= end) {
            frame = end - f32(l_frameEpsilon);
            mFrame = frame;
            rate = -rate;
            mRate = rate;
        }
        if (frame < start) {
            frame = start;
            mFrame = frame;
            rate = 0.0f;
            mRate = rate;
            state |= STATE_STOP_E;
            mState = state;
        }
        break;
    case EMode_LOOP_REVERSE:
        if (frame >= end) {
            frame = end - f32(l_frameEpsilon);
            mFrame = frame;
            rate = -rate;
            mRate = rate;
        }
        if (frame < start) {
            frame = start;
            mFrame = frame;
            rate = -rate;
            mRate = rate;
            state |= STATE_LOOP_E;
            mState = state;
        }
        break;
    }
    if (t60::dt() != 1.0f) /* true60: the frame this play left (the boundary logic may have moved it) */
        j3d_t60::note(gabi::ea(this), mFrame, mRate);
}

/* WWHD glue (tools/verify/autoglue.py):
data l_initialFrame 1016E48C
data l_initialRate 1016E488
data l_frameEpsilon 1016E490
ext 028F4034 f64 J3DFrameCtrl_fmod(f64, f64)
*/
