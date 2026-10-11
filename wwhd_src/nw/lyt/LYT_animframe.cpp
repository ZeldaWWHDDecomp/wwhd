/** HD layout animation: update frame bounds and dispatch layout animation lists.
 * Verified against LYT_animframe; no GameCube or SDK implementation template. */
#include "nw/lyt/LYT_animframe.h"

using FrameConstant = be<f32>;
static FrameConstant l_zeroFrame;

/* 02872DFC */
void nw::lyt::Animator::UpdateFrame() {
    f32 zero = l_zeroFrame;
    u32 flags = mFlags;
    f32 speed = mSpeed;
    mFlags = flags & ~(FrameFlag_Ended | FrameFlag_ForwardWrap | FrameFlag_BackwardWrap);
    if (speed == zero) return;
    f32 frame = f32(mFrame) + speed;
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
}

/* 0287FB68 */
void nw::lyt::Layout::UpdateAnimFrame() {
    Animator* sentinel = reinterpret_cast<Animator*>(&mAnimators);
    Animator* animator = mAnimators.mpNext;
    while (animator != sentinel) {
        AnimatorVtable* table = animator->__vtbl;
        gabi::call_ptr<void>(table->mUpdateFrame, animator);
        animator = animator->mLink.mpNext;
    }
    PartLink* partSentinel = &mParts;
    PartLink* link = mParts.mpNext;
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
