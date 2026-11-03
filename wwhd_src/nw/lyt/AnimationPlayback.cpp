/* Animation playback state operations reconstructed from WWHD instructions.
 * Uses the merged Animator fields unchanged; no external reference used. */
#include "nw/lyt/AnimationPlayback.h"
#include "nw/lyt/AnimationBinding.h"
using namespace nw::lyt;
static u8 sPlaybackMethods;

/* 02872D38 */
void PlaybackController::stopAtFrame(f32 frame) {
    u32 flags = mFlags;
    mFrame = frame;
    mFlags = flags & ~u32(FrameFlag_Ended | FrameFlag_ForwardWrap | FrameFlag_BackwardWrap);
    mSpeed = 0.0f;
}

/* 02872D58 */
void PlaybackController::stop() {
    u32 flags = mFlags;
    mFlags = flags & ~u32(FrameFlag_Ended | FrameFlag_ForwardWrap | FrameFlag_BackwardWrap);
    mSpeed = 0.0f;
}

/* 02872D74 */
void PlaybackController::rewind() {
    u32 flags = mFlags;
    mSpeed = 0.0f;
    mFrame = 0.0f;
    mFlags = flags & ~u32(FrameFlag_Ended | FrameFlag_ForwardWrap | FrameFlag_BackwardWrap);
}

/* 028713B8 */
u32 layoutAnimationFrameCount(Animator* animator) {
    void* pointer = animator->mpResource;
    auto* resource = static_cast<AnimationResourceInfo*>(pointer);
    return resource->frameCount;
}

/* 028713C4 */
void PlaybackController::setEnabled(u32 enabled) {
    auto* control = reinterpret_cast<AnimationControlFlag*>(padding_10);
    control->enabled = enabled;
}

/* 028713CC */
u32 layoutAnimationLoops(const Animator* animator) {
    void* pointer = animator->mpResource;
    auto* resource = static_cast<AnimationResourceInfo*>(pointer);
    return (u8)resource->loop != 0;
}

/* 028713E0 */
u32 layoutAnimationEmpty(const Animator* animator) {
    void* pointer = animator->mpResource;
    auto* resource = static_cast<AnimationResourceInfo*>(pointer);
    return (u16)resource->frameCount == 0;
}

/* 02872998 */
PlaybackController* PlaybackController::create(PlaybackController* controller) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!controller) controller = static_cast<PlaybackController*>(animationAllocate(sizeof(PlaybackController)));
    if (controller) {
        BoundAnimation::create(reinterpret_cast<BoundAnimation*>(controller));
        controller->mPlayType = 0;
        controller->mSpeed = 0.0f;
        controller->__vtbl = reinterpret_cast<AnimatorVtable*>(&sPlaybackMethods);
        be<u32>* flags = &controller->mFlags;
        // GHS constructs this embedded word through the null-allocating constructor path.
        if (gabi::ea(flags) == 0) flags = static_cast<be<u32>*>(animationAllocate(sizeof(be<u32>)));
        if (flags) *flags = 0;
    }
    return controller;
}

/* 02872A24 */
void PlaybackController::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    reinterpret_cast<BoundAnimation*>(this)->destroy(0);
    if (flags & 1) animationDelete(this);
}

struct FrameCountConversion { be<u32> high, low; };

/* 02872D94 */
void PlaybackController::seekEndAndStop() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mSpeed = 0.0f;
    u32 count = layoutAnimationFrameCount(this);
    gabi::FrameLocal<FrameCountConversion> conversion(8);
    conversion->low = count;
    conversion->high = 0x43300000;
    u32 flags = mFlags;
    mFlags = flags & ~u32(7);
    mFrame = f32(count);
}

/* 02872A78 */
void PlaybackController::startPlayback(u32 mode, f32 speed) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<FrameCountConversion> conversion(8);
    auto duration = [&]() -> f32 {
        u32 count = layoutAnimationFrameCount(this);
        conversion->low = count;
        conversion->high = 0x43300000;
        return f32(count);
    };
    bool accepted = false;
    f32 firstEnd = duration();
    if (!(-firstEnd > speed)) {
        f32 secondEnd = duration();
        accepted = !(speed > secondEnd);
    }
    if (!accepted) {
        if (layoutAnimationEmpty(this) == 0 || mode != 0) {
            layoutAnimationEmpty(this);
            return;
        }
    }
    u32 flags = mFlags;
    mSpeed = speed;
    mPlayType = mode;
    mFlags = flags & ~u32(7);
    if (!(speed < 0.0f)) mFrame = 0.0f;
    else mFrame = duration();
}

struct PlaybackStartMethods { u8 prefix[0xA4]; be<u32> start; };

/* 02872BC4 */
void PlaybackController::startResourcePlayback(f32 speed) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* methods = reinterpret_cast<PlaybackStartMethods*>((AnimatorVtable*)__vtbl);
    u32 mode = layoutAnimationLoops(this);
    gabi::call_ptr<void>(methods->start, this, mode, speed);
}

/* 02872C30 */
void PlaybackController::resumePlayback(u32 mode, f32 speed) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<FrameCountConversion> conversion(8);
    auto duration = [&]() -> f32 {
        u32 count = layoutAnimationFrameCount(this);
        conversion->low = count;
        conversion->high = 0x43300000;
        return f32(count);
    };
    bool accepted = false;
    f32 firstEnd = duration();
    if (!(-firstEnd > speed)) {
        f32 secondEnd = duration();
        accepted = !(speed > secondEnd);
    }
    if (!accepted) {
        if (layoutAnimationEmpty(this) == 0 || mode != 0) {
            layoutAnimationEmpty(this);
            return;
        }
    }
    u32 flags = mFlags;
    mSpeed = speed;
    mPlayType = mode;
    mFlags = flags & ~u32(7);
}

/* WWHD glue
data sPlaybackMethods 1017FFD8 nocheck
*/
