/** HD layout animation views reconstructed from verified fields.
 * No GameCube or SDK implementation template; opaque resource/base storage is retained. */
#pragma once
#include "bindings.h"
#include "nw/ut/FrameHeap.h"
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
struct LayoutPane; struct PaneGroupList; struct ResourceProvider; struct TextSearcher; struct GroupPlayback;
struct NamedPaneGroup; struct GroupArrayPlayback; struct AnimationArchive; struct LayoutDrawContext;
struct WideTagProcessor; struct LayoutParts; struct FontFile; struct PaneBuildContext;
struct LayoutSessionMethods; struct LayoutBuildListener; struct LayoutBuildOptions; struct LayoutPartIndex;
/* The layout object (0x34 bytes). Its two lists are nw::ut lists whose links sit inside the elements: an
 * Animator's mLink (offset 0) and a parts pane's part link (see LayoutParts). */
class Layout {
public:
    void UpdateAnimFrame();
    static Layout* create(Layout*);
    void destroy(u32 flags);
    u32 build(void* resource,ResourceProvider* resources,LayoutBuildListener* listener,TextSearcher* textSearcher);
    u32 buildByName(const be<u8>* name,ResourceProvider* resources,LayoutBuildListener* listener,TextSearcher* textSearcher);
    void* findAnimationResource(const be<u8>* name);
    void* findLayoutResource(const be<u8>* name);
    GroupPlayback* createAnimation(const be<u8>* name,NamedPaneGroup* group,u32 enabled);
    GroupArrayPlayback* createGroupArray(AnimationArchive* archive,u32 enabled);
    GroupPlayback* createGroupAnimation(const be<u8>* name,u32 enabled);
    void applyAnimations();
    void updateMatrices(LayoutDrawContext* context,u32 force);
    void traverse(LayoutDrawContext* context);
    void setTagProcessor(WideTagProcessor* processor);
    LayoutParts* findParts(const be<u8>* name);
    Layout* createPart(const be<u8>* name,LayoutPartIndex* overrides,const LayoutBuildOptions* options);
    u32 buildFromResource(FontFile* file,ResourceProvider* resources,const LayoutBuildOptions* options,LayoutPartIndex* overrides);
    LayoutPane* createPane(u32 tag,void* resource,void* alternate,PaneBuildContext* context);
    /* 0x00 */ nw::ut::HeapChildren mAnimators;
    /* 0x0C */ gptr<LayoutPane> mRoot;
    /* 0x10 */ gptr<PaneGroupList> mGroups;
    /* 0x14 */ be<f32> mWidth;
    /* 0x18 */ be<f32> mHeight;
    /* 0x1C */ gptr<be<u8>> mResourceName;
    /* 0x20 */ gptr<ResourceProvider> mResources;
    /* 0x24 */ nw::ut::HeapChildren mParts;
    /* 0x30 */ gptr<LayoutSessionMethods> mMethods;
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
WWHD_OFFSET(nw::lyt::Layout, mAnimators, 0);
WWHD_OFFSET(nw::lyt::Layout, mRoot, 0xC);
WWHD_OFFSET(nw::lyt::Layout, mGroups, 0x10);
WWHD_OFFSET(nw::lyt::Layout, mWidth, 0x14);
WWHD_OFFSET(nw::lyt::Layout, mResourceName, 0x1C);
WWHD_OFFSET(nw::lyt::Layout, mResources, 0x20);
WWHD_OFFSET(nw::lyt::Layout, mParts, 0x24);
WWHD_OFFSET(nw::lyt::Layout, mMethods, 0x30);
WWHD_SIZE(nw::lyt::Layout, 0x34);
