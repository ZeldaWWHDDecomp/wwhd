/* Playback operations on the canonical merged Animator layout. */
#pragma once
#include "nw/lyt/LYT_animframe.h"
namespace nw { namespace lyt {
struct PlaybackController : Animator {
    static PlaybackController* create(PlaybackController* controller);
    void destroy(u32 flags);
    void stopAtFrame(f32 frame);
    void stop();
    void rewind();
    void seekEndAndStop();
    void startPlayback(u32 mode, f32 speed);
    void startResourcePlayback(f32 speed);
    void resumePlayback(u32 mode, f32 speed);
    void setEnabled(u32 enabled);
};
WWHD_SIZE(PlaybackController, sizeof(Animator));
struct AnimationResourceInfo {
    u8 prefix[8];
    be<u16> frameCount;
    be<u8> loop;
};
struct AnimationControlFlag { be<u8> enabled; };
WWHD_OFFSET(AnimationResourceInfo, frameCount, 8);
WWHD_OFFSET(AnimationResourceInfo, loop, 0xA);
WWHD_OFFSET(Animator, padding_10, 0x10);
} }
u32 layoutAnimationLoops(const nw::lyt::Animator* animator);
u32 layoutAnimationEmpty(const nw::lyt::Animator* animator);
u32 layoutAnimationFrameCount(nw::lyt::Animator* animator);
