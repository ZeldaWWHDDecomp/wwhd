/* Layout animation factories reconstructed from the WWHD binary. */
#include "nw/lyt/LayoutSession.h"
#include "nw/lyt/AnimationBinding.h"
using namespace nw::lyt;
using nw::ut::HeapLink;
static u8 sGroupPlaybackMethods;
static u8 sGroupArrayPlaybackMethods;
struct SessionAttachMethods { u8 prefix[0x44]; be<u32> attach; };
void animationSessionInsert(nw::ut::HeapChildren*,gptr<HeapLink>*,void*);
/* 0287F740 */
GroupPlayback* Layout::createAnimation(const be<u8>* name,NamedPaneGroup* group,u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* resource=findAnimationResource(name);
    gabi::FrameLocal<AnimationArchive> archive(0xC);
    archive->attach(static_cast<FontFile*>(resource));
    FontBlock* animation=archive->animation;
    GroupPlayback* result=nullptr;
    if (animation) {
        result=static_cast<GroupPlayback*>(allocateLayoutStorage(0x34,4));
        if (result) {
            BoundAnimation::create(reinterpret_cast<BoundAnimation*>(result));
            result->group=nullptr;
            result->__vtbl=reinterpret_cast<AnimatorVtable*>(&sGroupPlaybackMethods);
            gabi::FrameLocal<gptr<HeapLink>> iterator(8);
            *iterator=&mAnimators.root;
            animationSessionInsert(&mAnimators,iterator.get(),result);
            auto* dispatch=reinterpret_cast<SessionAttachMethods*>(result->__vtbl.get());
            ResourceProvider* provider=mResources;
            gabi::call_ptr<void>(dispatch->attach,result,animation,provider);
        }
    }
    result->bindGroup(group,enabled);
    return result;
}
/* 0287F858 */
GroupArrayPlayback* Layout::createGroupArray(AnimationArchive* archive,u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    FontBlock* animation=archive->animation;
    if (!animation) return nullptr;
    u32 count=archive->groupCount();
    auto* result=static_cast<GroupArrayPlayback*>(allocateLayoutStorage(count*4+0x38,4));
    if (!result) return nullptr;
    BoundAnimation::create(reinterpret_cast<BoundAnimation*>(result));
    result->count=0;
    result->__vtbl=reinterpret_cast<AnimatorVtable*>(&sGroupArrayPlaybackMethods);
    result->groups=nullptr;
    auto* dispatch=reinterpret_cast<SessionAttachMethods*>(result->__vtbl.get());
    ResourceProvider* provider=mResources;
    gabi::call_ptr<void>(dispatch->attach,result,animation,provider);
    gabi::FrameLocal<gptr<HeapLink>> iterator(8);
    *iterator=&mAnimators.root;
    animationSessionInsert(&mAnimators,iterator.get(),result);
    PaneGroupList* groupList=mGroups;
    result->bindArchiveGroups(archive,groupList,layoutAtOffset<gptr<NamedPaneGroup>>(result,0x38),enabled);
    return result;
}
/* 0287F944 */
GroupPlayback* Layout::createGroupAnimation(const be<u8>* name,u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* resource=findAnimationResource(name);
    gabi::FrameLocal<AnimationArchive> archive(0xC);
    archive->attach(static_cast<FontFile*>(resource));
    if (archive->groupCount()>1) return reinterpret_cast<GroupPlayback*>(createGroupArray(archive.get(),enabled));
    FontBlock* animation=archive->animation;
    GroupPlayback* result=nullptr;
    if (animation) {
        result=static_cast<GroupPlayback*>(allocateLayoutStorage(0x34,4));
        if (result) {
            BoundAnimation::create(reinterpret_cast<BoundAnimation*>(result));
            result->group=nullptr;
            result->__vtbl=reinterpret_cast<AnimatorVtable*>(&sGroupPlaybackMethods);
            gabi::FrameLocal<gptr<HeapLink>> iterator(8);
            *iterator=&mAnimators.root;
            animationSessionInsert(&mAnimators,iterator.get(),result);
            auto* dispatch=reinterpret_cast<SessionAttachMethods*>(result->__vtbl.get());
            ResourceProvider* provider=mResources;
            gabi::call_ptr<void>(dispatch->attach,result,animation,provider);
        }
    }
    PaneGroupList* groupList=mGroups;
    result->bindArchiveGroup(archive.get(),groupList,0,enabled);
    return result;
}
/* WWHD glue
data sGroupPlaybackMethods 101800B8
data sGroupArrayPlaybackMethods 10180198
ext 02872998 nw::lyt::BoundAnimation* nw::lyt::BoundAnimation::create(nw::lyt::BoundAnimation*)
ext 028727E8 void nw::lyt::AnimationArchive::attach(nw::lyt::FontFile*)
ext 028728EC u32 nw::lyt::AnimationArchive::groupCount()
ext 02873058 void nw::lyt::GroupPlayback::bindGroup(nw::lyt::NamedPaneGroup*,u32)
ext 028730C4 void nw::lyt::GroupPlayback::bindArchiveGroup(nw::lyt::AnimationArchive*,nw::lyt::PaneGroupList*,u32,u32)
ext 02873134 void nw::lyt::GroupArrayPlayback::bindArchiveGroups(nw::lyt::AnimationArchive*,nw::lyt::PaneGroupList*,gptr<nw::lyt::NamedPaneGroup>*,u32)
ext 0286DA70 void animationSessionInsert(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,void*)
*/
