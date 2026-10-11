/** J3D frame-controller initialization, frame crossings and playback.
 * GameCube names with the verified HD layout, load/store order and remainder arithmetic. */
#include "J3DAnimation.h"

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
    f32 next_frame = cur_frame + rate;

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
}

/* WWHD glue (tools/verify/autoglue.py):
data l_initialFrame 1016E48C
data l_initialRate 1016E488
data l_frameEpsilon 1016E490
ext 028F4034 f64 J3DFrameCtrl_fmod(f64, f64)
*/
