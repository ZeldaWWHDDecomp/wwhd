/** HD layout animation: update frame bounds and dispatch layout animation lists.
 * Verified against LYT_animframe; no GameCube or SDK implementation template. */
#include "nw/lyt/LYT_animframe.h"
#include "true60.h"
#include "nw/lyt/LYT_t60.h"

using FrameConstant = be<f32>;
static FrameConstant l_zeroFrame;

/* 02872DFC */
void nw::lyt::Animator::UpdateFrame() {
    f32 zero = l_zeroFrame;
    u32 flags = mFlags;
    f32 speed = mSpeed;
    mFlags = flags & ~(FrameFlag_Ended | FrameFlag_ForwardWrap | FrameFlag_BackwardWrap);
    if (speed == zero) {
        // true60 (LYT_t60.h): a held animator is recorded too (the picture re-applies every animator in order)
        if (t60lyt_recording()) wwhd_t60_lyt_record(gabi::ea(this), mFrame, speed, mFrame, u32(mPlayType), u32(mFlags));
        return;
    }
    f32 before = mFrame; /* (true60: the picture's frame start, LYT_t60.h) */
    // true60 (the animation crossing-check rule and 7): the layout play speed is per 30 Hz frame, a step advances speed*dt; the
    // end/wrap flags stay a per-update latch (set on the step whose update ends or wraps, cleared by the next update),
    // as J3DFrameCtrl::update. dt = 1 is the original code.
    f32 frame = t60::dt() != 1.0f ? f32(mFrame) + speed * t60::dt() : f32(mFrame) + speed;
    if (speed > zero) {
        f32 end = f32(LYTAnimator_getFrameCount(this));
        if (!(frame < end)) {
            switch (u32(mPlayType)) {
            case PlayType_Once:
                frame = f32(LYTAnimator_getFrameCount(this));
                flags = mFlags;
                mSpeed = zero;
                mFlags = flags | FrameFlag_Ended;
                break;
            case PlayType_Loop: {
                f32 length = f32(LYTAnimator_getFrameCount(this));
                flags = mFlags;
                mFlags = flags | FrameFlag_ForwardWrap;
                frame = frame - length;
                break;
            }
            case PlayType_Mirror: {
                // HD: retain both count calls and reload speed/flags after they may clobber the animator.
                f32 end1 = f32(LYTAnimator_getFrameCount(this));
                f32 end2 = f32(LYTAnimator_getFrameCount(this));
                f32 currentSpeed = mSpeed;
                flags = mFlags;
                mSpeed = -currentSpeed;
                mFlags = flags | FrameFlag_ForwardWrap;
                frame = end1 - (frame - end2);
                break;
            }
            }
        }
    } else if (!(frame > zero)) {
        switch (u32(mPlayType)) {
        case PlayType_Once:
            flags = mFlags;
            mSpeed = zero;
            frame = zero;
            mFlags = flags | FrameFlag_Ended;
            break;
        case PlayType_Loop: {
            f32 length = f32(LYTAnimator_getFrameCount(this));
            flags = mFlags;
            mFlags = flags | FrameFlag_BackwardWrap;
            frame = frame + length;
            break;
        }
        case PlayType_Mirror:
            flags = mFlags;
            mSpeed = -speed;
            frame = -frame;
            mFlags = flags | FrameFlag_BackwardWrap;
            break;
        }
    }
    mFrame = frame;
    // true60 (LYT_t60.h): the first step's whole-frame update of an N > 1 frame, for the port's picture
    if (t60lyt_recording()) wwhd_t60_lyt_record(gabi::ea(this), before, speed, frame, u32(mPlayType), u32(mFlags));
}

/* 0287FB68 */
void nw::lyt::Layout::UpdateAnimFrame() {
    Animator* sentinel = reinterpret_cast<Animator*>(&mAnimators.root);
    Animator* animator = reinterpret_cast<Animator*>(mAnimators.root.next.get());
    while (animator != sentinel) {
        AnimatorVtable* table = animator->__vtbl;
        gabi::call_ptr<void>(table->mUpdateFrame, animator);
        animator = animator->mLink.mpNext;
    }
    PartLink* partSentinel = reinterpret_cast<PartLink*>(&mParts.root);
    PartLink* link = reinterpret_cast<PartLink*>(mParts.root.next.get());
    while (link != partSentinel) {
        Part* part = link->mpPart;
        PartVtable* table = part->__vtbl;
        gabi::call_ptr<void>(table->mUpdateAnimFrame, part);
        link = link->mpNext;
    }
}

/* WWHD glue (tools/verify/autoglue.py):
data l_zeroFrame 1017FFC8
ext 028713B8 u32 LYTAnimator_getFrameCount(nw::lyt::Animator*)
*/
