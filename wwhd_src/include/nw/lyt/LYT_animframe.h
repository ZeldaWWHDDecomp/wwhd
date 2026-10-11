/** HD layout animation views reconstructed from verified fields.
 * No GameCube or SDK implementation template; opaque resource/base storage is retained. */
#pragma once
#include "bindings.h"
namespace nw { namespace lyt {
class Animator;
struct AnimatorVtable {
    u8 padding[0x1C];
    be<u32> mUpdateFrame;
};
struct PartVtable {
    u8 padding[0x54];
    be<u32> mUpdateAnimFrame;
};
struct Part {
    u8 padding[0x30];
    gptr<PartVtable> __vtbl;
};
struct AnimatorLink {
    gptr<Animator> mpNext, mpPrevious;
};
struct PartLink {
    gptr<PartLink> mpNext, mpPrevious;
    gptr<Part> mpPart;
};
class Animator {
public:
    enum PlayType { PlayType_Once, PlayType_Loop, PlayType_Mirror };
    enum FrameFlag { FrameFlag_Ended = 1, FrameFlag_ForwardWrap = 2, FrameFlag_BackwardWrap = 4 };
    void UpdateFrame();
    /* 0x00 */ AnimatorLink mLink;
    /* 0x08 */ gptr<void> mpResource;
    /* 0x0C */ be<f32> mFrame;
    u8 padding_10[4];
    /* 0x14 */ gptr<AnimatorVtable> __vtbl;
    u8 padding_18[0xC];
    /* 0x24 */ be<f32> mSpeed;
    /* 0x28 */ be<u32> mPlayType;
    /* 0x2C */ be<u32> mFlags;
};
class Layout {
public:
    void UpdateAnimFrame();
    u8 padding_0[4];
    /* 0x04 */ AnimatorLink mAnimators;
    u8 padding_C[0x1C];
    /* 0x28 */ PartLink mParts;
};
}}
u32 LYTAnimator_getFrameCount(nw::lyt::Animator*);
WWHD_SIZE(nw::lyt::Animator, 0x30);
WWHD_OFFSET(nw::lyt::Animator, mpResource, 8);
WWHD_OFFSET(nw::lyt::Animator, mFrame, 0xC);
WWHD_OFFSET(nw::lyt::Animator, __vtbl, 0x14);
WWHD_OFFSET(nw::lyt::Animator, mSpeed, 0x24);
WWHD_OFFSET(nw::lyt::Animator, mPlayType, 0x28);
WWHD_OFFSET(nw::lyt::Animator, mFlags, 0x2C);
WWHD_SIZE(nw::lyt::AnimatorLink, 8);
WWHD_SIZE(nw::lyt::PartLink, 0xC);
WWHD_OFFSET(nw::lyt::Part, __vtbl, 0x30);
WWHD_OFFSET(nw::lyt::AnimatorVtable, mUpdateFrame, 0x1C);
WWHD_OFFSET(nw::lyt::PartVtable, mUpdateAnimFrame, 0x54);
WWHD_OFFSET(nw::lyt::Layout, mAnimators, 4);
WWHD_OFFSET(nw::lyt::Layout, mParts, 0x28);
WWHD_SIZE(nw::lyt::Layout, 0x34);
